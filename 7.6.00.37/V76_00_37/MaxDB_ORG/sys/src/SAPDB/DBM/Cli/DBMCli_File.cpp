/*! 
  -----------------------------------------------------------------------------
  module: DBMCli_File.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager Client Library
  description:  File Class
  version:      7.3 and higher 
  -----------------------------------------------------------------------------
                          Copyright (c) 2000-2005 SAP AG
  -----------------------------------------------------------------------------



    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
    ========== licence end




*/

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "DBM/Cli/DBMCli_Database.hpp"
#include "DBM/Cli/DBMCli_File.hpp"
#include "DBM/Cli/DBMCli_DateTime.hpp"

/*    
  =============================================================================
  class: DBMCli_File
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMCli_File :: DBMCli_File
  -----------------------------------------------------------------------------
*/
DBMCli_File :: DBMCli_File ( )
{
  m_sName.Empty( );
  m_nMode = DBMCLI_FILEMODE_UNKNOWN;
  m_sSize.Empty( );
  m_sModified.Empty( );
  m_sComment.Empty( );
  m_sFilename.Empty( );
} 

DBMCli_File :: DBMCli_File ( const DBMCli_String   & sName,
                                   DBMCli_FileMode   nMode,
                             const DBMCli_String   & sSize,
                             const DBMCli_String   & sModified,
                             const DBMCli_String   & sComment,
                             const DBMCli_String   & sFilename )
              : m_sName ( sName ),
                m_nMode ( nMode ),
                m_sSize ( sSize ),
                m_sModified ( sModified ),
                m_sComment ( sComment ),
                m_sFilename ( sFilename )
{
}

/*
  -----------------------------------------------------------------------------
  public destructor DBMCli_File :: ~DBMCli_File
  -----------------------------------------------------------------------------
*/
DBMCli_File :: ~DBMCli_File ( )
{
} 

/*
  -----------------------------------------------------------------------------
  public function DBMCli_File :: Open
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_File :: Open ( const DBMCli_String        & sName,
                                       Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  DBMCli_Database & oDB = GetDatabase();
  DBMCli_Result & oResult = oDB.GetResult( ); 

  DBMCli_String sCmd( DBMCLI_CMD_FILEGETFIRST );
  sCmd += " ";
  sCmd += sName;

  if( oDB.Execute( sCmd, oMsgList ) ) {
    m_sName = sName;

    DBMCli_String sLine;

    // handle
    oResult.GetLine( m_sHandle ); 
    m_sHandle.Trim( );

    // continue or end 
    oResult.GetLine( sLine ); 
    m_bContinue = ( sLine == DBMCLI_VAL_CONTINUE );
    m_bOpen = m_bContinue;

    // size and transferred
    oResult.GetLine( sLine ); 

    bRC = true;
  }

  return bRC;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_File :: GetLine
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_File :: GetLine ( DBMCli_String        & sLine,
                                    Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  DBMCli_Database & oDB = GetDatabase();
  DBMCli_Result & oResult = oDB.GetResult( ); 

  if( oResult.GetFullLine( sLine ) ) {
    bRC = true;
  } else {

    DBMCli_String sRest;
    oResult.GetLine( sRest ); 

    if( m_bContinue ) {
      if ( Next( oMsgList ) ) {
        if( oResult.GetLine( sLine ) ) {
          sLine = sRest + sLine;
          bRC = true;
        }
      }
    }
  }

  return bRC;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_File :: GetPart
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_File :: GetPart ( DBMCli_String        & sPart,
                                    SAPDB_Int              nMaxLen,
                                    Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  DBMCli_Database & oDB = GetDatabase();
  DBMCli_Result & oResult = oDB.GetResult( ); 

  if( oResult.GetPart( sPart, nMaxLen ) ) {
    bRC = true;
  } else {
    if( m_bContinue ) {
      if ( Next( oMsgList ) ) {
        if( oResult.GetPart( sPart, nMaxLen ) ) {
          bRC = true;
        }
      }
    }
  }

  return bRC;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_File :: Close
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_File :: Close ( Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  if( m_bOpen ) {
    DBMCli_Database & oDB = GetDatabase();

    DBMCli_String sCmd( DBMCLI_CMD_FILECLOSE );
    sCmd += " ";
    sCmd += m_sHandle;

    bRC = oDB.Execute( sCmd, oMsgList );
  } else {
    bRC = true;
  }

  return bRC;
}


/*
  -----------------------------------------------------------------------------
  private functions ...
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  private function DBMCli_File :: Next
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_File :: Next ( Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  DBMCli_Database & oDB = GetDatabase();
  DBMCli_Result & oResult = oDB.GetResult( ); 

  DBMCli_String sCmd( DBMCLI_CMD_FILEGETNEXT );
  sCmd += " ";
  sCmd += m_sName;
  sCmd += " ";
  sCmd += m_sHandle;

  if( oDB.Execute( sCmd, oMsgList ) ) {
    DBMCli_String sLine;

    // handle
    oResult.SkipLine( );
    
    // continue or end
    oResult.GetLine( sLine ); 
    m_bContinue = ( sLine == DBMCLI_VAL_CONTINUE );
    m_bOpen = m_bContinue;

    // size and transferred
    oResult.SkipLine( );

    bRC = true;
  }

  return bRC;
}

/*    
  =============================================================================
  class: DBMCli_Files
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMCli_Files :: DBMCli_Files
  -----------------------------------------------------------------------------
*/
DBMCli_Files :: DBMCli_Files ( )
{
} 

/*
  -----------------------------------------------------------------------------
  public destructor DBMCli_Files :: ~DBMCli_Files
  -----------------------------------------------------------------------------
*/
DBMCli_Files :: ~DBMCli_Files ( )
{
} 

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Files :: Refresh
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Files :: Refresh ( Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  m_aFile.RemoveAll( );

  DBMCli_Database & oDB = GetDatabase();
  DBMCli_Result & oResult = oDB.GetResult( ); 

  DBMCli_String sCmd( DBMCLI_CMD_FILEGETLIST );

  if( oDB.Execute( sCmd, oMsgList ) ) {
    DBMCli_ResultBuf oLine;

    DBMCli_String sName;
    DBMCli_String sMode;
    DBMCli_FileMode nMode;
    DBMCli_String sSize;
    DBMCli_String sDate;
    DBMCli_String sTime;
    DBMCli_String sModified;
    DBMCli_String sComment;
    DBMCli_String sFilename;

    DBMCli_DateTime oDT;

    oResult.SkipLine( ); 

    while( oResult.GetLine( oLine ) ) {

      if( oLine.GetField( sName, "\t\n" ) ) {
        if( oLine.GetField( sMode, "\t\n" ) ) {
          AssignFileMode ( sMode, &nMode );
          if( oLine.GetField( sSize, "\t\n" ) )
            if( oLine.GetField( sDate, "\t\n" ) )
              if( oLine.GetField( sTime, "\t\n" ) ) {
                sModified = sDate + sTime;
                oDT.Set( sModified, DBMCLI_DT_NUM_DATETIME_FMT );
                sModified = oDT.Get( DBMCLI_DT_INT_DATETIME_FMT ); 
                if( oLine.GetField( sComment, "\t\n" ) )
                  oLine.GetField( sFilename, "\t\n" );
              }
        }

        if( nMode == DBMCLI_FILEMODE_ASCII ) {
          DBMCli_File oFile( sName, nMode, sSize, sModified, sComment, sFilename );
          oFile.SetDatabase( m_pDatabase );
          m_aFile.Add( oFile );
        }
      }
      oLine.Clear( );
    }

    bRC = true;
  }

  return bRC;
}

/*
  -----------------------------------------------------------------------------
  private functions ...
  -----------------------------------------------------------------------------
*/

void DBMCli_Files:: AssignFileMode ( const DBMCli_String   & sVal,
                                           DBMCli_FileMode * pVal )
{
  if( sVal == DBMCLI_FILEMODE_STRING_ASCII ) {
    *pVal = DBMCLI_FILEMODE_ASCII;
  } else if( sVal == DBMCLI_FILEMODE_STRING_BINARY ) {
    *pVal = DBMCLI_FILEMODE_BINARY;
  } else {
    *pVal = DBMCLI_FILEMODE_UNKNOWN;
  }
}


/*    
  =============================================================================
  class: DBMCli_Diagnosises
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Diagnosises :: Refresh
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Diagnosises :: Refresh ( Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  m_oDiagnosises.RemoveAll();
  oMsgList.ClearMessageList();

  DBMCli_Database & oDB     = GetDatabase();
  DBMCli_Result   & oResult = oDB.GetResult( ); 

  DBMCli_String sCmd( DBMCLI_CMD_DIAGLIST );

  if( oDB.Execute( sCmd, oMsgList ) ) {
    DBMCli_ResultBuf oLine;

    DBMCli_String   sPath;
    DBMCli_String   sTimestamp;
    DBMCli_DateTime oTimestamp;

    while( oResult.GetLine( oLine ) ) {

      if( oLine.GetField( sTimestamp, "\t\n" ) ) {
        if( oLine.GetField( sPath, "\t\n" ) ) {
          oTimestamp.Set(sTimestamp, DBMCLI_DT_NUM_DATETIME_FMT);
          DBMCli_Diagnosis oDiagnosis(oTimestamp, sPath);
          oDiagnosis.SetDatabase( m_pDatabase );
          m_oDiagnosises.Add(oDiagnosis);
        } // end if
      } // end if

      oLine.Clear( );
    } // end if

    bRC = true;
  } // end if

  return bRC;
} // end DBMCli_Diagnosises :: Refresh

/*    
  =============================================================================
  class: DBMCli_Diagnosis
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMCli_Diagnosis :: DBMCli_Diagnosis
  -----------------------------------------------------------------------------
*/
DBMCli_Diagnosis :: DBMCli_Diagnosis ( const DBMCli_DateTime & oTimestamp,
                                       const DBMCli_String   & sPath)
                  : m_oTimestamp(oTimestamp),
                    m_sPath(sPath)
{
} // DBMCli_Diagnosis :: DBMCli_Diagnosis

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Diagnosis :: Refresh
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Diagnosis :: Refresh ( Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  m_oFiles.RemoveAll();
  oMsgList.ClearMessageList();

  DBMCli_Database & oDB     = GetDatabase();
  DBMCli_Result   & oResult = oDB.GetResult( ); 

  DBMCli_String sCmd( DBMCLI_CMD_DIAGLIST );
  sCmd = sCmd + " " + m_oTimestamp.Get(DBMCLI_DT_NUM_DATETIME_FMT);

  if( oDB.Execute( sCmd, oMsgList ) ) {
    DBMCli_ResultBuf oLine;

    DBMCli_String   sName;

    while( oResult.GetLine( oLine ) ) {

      if( oLine.GetField( sName, "\t\n" ) ) {
        DBMCli_File oFile( sName, DBMCLI_FILEMODE_UNKNOWN, "", "", "", "" );
        oFile.SetDatabase( m_pDatabase );
        m_oFiles.Add( oFile );
      } // end if

      oLine.Clear( );
    } // end if

    bRC = true;
  } // end if

  return bRC;
} // end DBMCli_Diagnosis :: Refresh

