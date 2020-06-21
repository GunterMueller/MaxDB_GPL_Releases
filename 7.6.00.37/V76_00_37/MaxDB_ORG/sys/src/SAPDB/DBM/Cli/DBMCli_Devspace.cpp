/*! 
  -----------------------------------------------------------------------------
  module: DBMCli_Devspace.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager Client Library
  description:  Devspace Class
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
#include "DBM/Cli/DBMCli_Devspace.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define PARAM_LOGMODE       "LOG_MODE"
#define PARAM_MAXLOGDEVS    "MAXARCHIVELOGS"
#define PARAM_MAXDATADEVS   "MAXDATADEVSPACES"  
#define PARAM_MAXDATAPAGES  "MAXDATAPAGES"
#define PARAM_LOGPARAM      "ARCHIVE"
#define PARAM_DATAPARAM     "DATADEV"
#define PARAM_MLOGPARAM     "M_ARCHI"
#define PARAM_MDATAPARAM    "M_DATAD"

/* >= 7.4.4.0 */
#define PARAM_MAXLOGVOLS    "MAXLOGVOLUMES"
#define PARAM_MAXDATAVOLS   "MAXDATAVOLUMES"  
#define PARAM_LOGPARAM744   "LOG_VOL"
#define PARAM_DATAPARAM744  "DATA_VO"
#define PARAM_MLOGPARAM744  "M_LOG_V"
#define PARAM_MDATAPARAM744 "M_DATA_"

#define PARAM_VAL_LOGMODE_DUAL     "DUAL"
#define PARAM_VAL_LOGMODE_SINGLE   "SINGLE"
#define PARAM_VAL_LOGMODE_DEMO     "DEMO"

#define PARAM_VAL_DEVSPACECLASS_SYS  "SYS"
#define PARAM_VAL_DEVSPACECLASS_DATA "DATA"
#define PARAM_VAL_DEVSPACECLASS_LOG  "LOG"

#define PARAM_VAL_DEVSPACETYPE_FILE  "F"
#define PARAM_VAL_DEVSPACETYPE_RAW   "R"
#define PARAM_VAL_DEVSPACETYPE_LINK  "L"

#define NUMFORMAT_SYS  "%03d" 
#define NUMFORMAT_LOG  "%03d" 
#define NUMFORMAT_DATA "%04d" 

#define TYPENAME_FILE "FILE"
#define TYPENAME_RAW  "RAW"
#define TYPENAME_LINK "LINK"

#define MIRROR_PREFIX "M"

#define MAX_CONV_REC_BD00       1855
#define SYS_DEV_BLOCK_CNT_BD01  8
#define OF_CONV_0_BD15          4

/*    
  =============================================================================
  class: DBMCli_Devspace
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMCli_Devspace :: DBMCli_Devspace
  -----------------------------------------------------------------------------
*/
DBMCli_Devspace :: DBMCli_Devspace ( )
{
  m_nClass = DBMCLI_DEVSPACECLASS_UNKNOWN;
  m_nNumber = 0;

  m_bMirrored = false;
  m_sMirroredLocation.Empty( );
  m_nMirroredType = DBMCLI_DEVSPACETYPE_UNKNOWN;
  m_sMirroredTypeName.Empty( );

  m_sPreparedMirroredLocation.Empty( );
  m_nPreparedMirroredType = DBMCLI_DEVSPACETYPE_UNKNOWN;
  m_sPreparedMirroredTypeName.Empty( );

}

DBMCli_Devspace :: DBMCli_Devspace ( const DBMCli_DevspaceClass nClass,
                                     const SAPDB_Int            nNumber )
                 : m_nClass( nClass ),
                   m_nNumber( nNumber )
{
  AssignName( nClass, nNumber );
  AssignClassName( m_nClass );

  m_bMirrored = false;
  m_sMirroredLocation.Empty( );
  m_nMirroredType = DBMCLI_DEVSPACETYPE_UNKNOWN;
  m_sMirroredTypeName.Empty( );

  m_sPreparedMirroredLocation.Empty( );
  m_nPreparedMirroredType = DBMCLI_DEVSPACETYPE_UNKNOWN;
  m_sPreparedMirroredTypeName.Empty( );
}

DBMCli_Devspace :: DBMCli_Devspace ( const DBMCli_DevspaceClass nClass,
                                     const SAPDB_Int            nNumber,
                                     const SAPDB_Int4           nPages,
                                     const DBMCli_String      & sLocation,
                                     const DBMCli_DevspaceType  nType )
                 : m_nClass( nClass ),
                   m_nNumber( nNumber ),
                   m_nPages( nPages ),
                   m_sLocation( sLocation ),
                   m_nType( nType )
{
  AssignName( m_nClass, m_nNumber );
  AssignClassName( m_nClass );
  AssignTypeName( m_nType );

  m_bMirrored = false;
  m_sMirroredLocation.Empty( );
  m_nMirroredType = DBMCLI_DEVSPACETYPE_UNKNOWN;
  m_sMirroredTypeName.Empty( );

  m_sPreparedMirroredLocation.Empty( );
  m_nPreparedMirroredType = DBMCLI_DEVSPACETYPE_UNKNOWN;
  m_sPreparedMirroredTypeName.Empty( );
}

DBMCli_Devspace :: DBMCli_Devspace ( const DBMCli_String & sName,
                                     const DBMCli_String & sPages,
                                     const DBMCli_String & sLocation,
                                     const DBMCli_String & sType )
{
  DBMCli_String sLog( PARAM_LOGPARAM );
  DBMCli_String sClass;
  DBMCli_String sNumber;

  if( sName.GetLength( ) >= sLog.GetLength( ) ) {
    sClass = sName.Left( sLog.GetLength( ) );
    AssignClass( sClass );
  
    switch( m_nClass ) {
      case DBMCLI_DEVSPACECLASS_LOG: 
        sNumber = sName.Right( 3 );
        break;

      case DBMCLI_DEVSPACECLASS_DATA:
        sNumber = sName.Right( 4 );
        break;

      default:
        sNumber = "0";
        break;
    }

    m_nNumber = atoi( sNumber );
    AssignName( m_nClass, m_nNumber );
  }
  
  m_nPages = atol( sPages );
  m_sLocation = sLocation;

  AssignType( sType );
  AssignClassName( m_nClass );
  AssignTypeName( m_nType );

  m_bMirrored = false;
  m_sMirroredLocation.Empty( );
  m_nMirroredType = DBMCLI_DEVSPACETYPE_UNKNOWN;
  m_sMirroredTypeName.Empty( );

  m_sPreparedMirroredLocation.Empty( );
  m_nPreparedMirroredType = DBMCLI_DEVSPACETYPE_UNKNOWN;
  m_sPreparedMirroredTypeName.Empty( );
}

/*
  -----------------------------------------------------------------------------
  public destructor DBMCli_Devspace :: ~DBMCli_Devspace
  -----------------------------------------------------------------------------
*/
DBMCli_Devspace :: ~DBMCli_Devspace ( )
{
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Devspace :: Clear
  -----------------------------------------------------------------------------
*/
void DBMCli_Devspace :: Clear ( )
{
  m_nPages = 0;
  m_sLocation.Empty( );
  m_nType = DBMCLI_DEVSPACETYPE_UNKNOWN;
  m_sTypeName.Empty( );
  m_sMirroredLocation.Empty( );
  m_nMirroredType = DBMCLI_DEVSPACETYPE_UNKNOWN;
  m_sMirroredTypeName.Empty( );
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Devspace :: SetProperties
  -----------------------------------------------------------------------------
*/
void DBMCli_Devspace :: SetProperties ( const DBMCli_DevspaceClass nClass,
                                        const SAPDB_Int            nNumber,
                                        const SAPDB_Int4           nPages,
                                        const DBMCli_String      & sLocation,
                                        const DBMCli_DevspaceType  nType,
                                        const SAPDB_Bool           bMirrored,
                                        const DBMCli_String      & sMirroredLocation,
                                        const DBMCli_DevspaceType  nMirroredType )
{
  m_nClass = nClass;
  m_nNumber = nNumber;
  m_nPages = nPages;
  m_sLocation = sLocation;
  m_nType = nType;

  AssignName( m_nClass, m_nNumber );
  AssignClassName( m_nClass );
  AssignTypeName( m_nType );

  m_bMirrored = bMirrored;
  m_sMirroredLocation = sMirroredLocation;
  m_nMirroredType = nMirroredType;  

  AssignTypeName( m_nMirroredType, true );
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Devspace :: SetMirroredProperties
  -----------------------------------------------------------------------------
*/
void DBMCli_Devspace :: SetMirroredProperties ( const SAPDB_Bool      bMirrored,
                                                const DBMCli_String & sMirroredLocation,
                                                const DBMCli_String   sMirroredType )
{
  m_bMirrored = bMirrored;
  m_sMirroredLocation = sMirroredLocation;

  AssignType( sMirroredType, true );
  AssignTypeName( m_nMirroredType, true );
}

void DBMCli_Devspace :: SetMirroredProperties ( const SAPDB_Bool           bMirrored,
                                                const DBMCli_String      & sMirroredLocation,
                                                const DBMCli_DevspaceType  nMirroredType )
{
  m_bMirrored = bMirrored;
  m_sMirroredLocation = sMirroredLocation;
  m_nMirroredType = nMirroredType;  

  AssignTypeName( m_nMirroredType, true );
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Devspace :: PrepareMirroredProperties
  -----------------------------------------------------------------------------
*/
void DBMCli_Devspace :: PrepareMirroredProperties (const DBMCli_String & sMirroredLocation,
                                                   const DBMCli_String & sMirroredType )
{
  m_sPreparedMirroredLocation = sMirroredLocation;
  m_sPreparedMirroredTypeName = sMirroredType;

  if( sMirroredType == PARAM_VAL_DEVSPACETYPE_FILE ) {
    m_nPreparedMirroredType = DBMCLI_DEVSPACETYPE_FILE;
  } else if( sMirroredType == PARAM_VAL_DEVSPACETYPE_RAW ) {
    m_nPreparedMirroredType = DBMCLI_DEVSPACETYPE_RAW;
  } else if( sMirroredType == PARAM_VAL_DEVSPACETYPE_LINK ) {
    m_nPreparedMirroredType = DBMCLI_DEVSPACETYPE_LINK;
  } else {
    m_nPreparedMirroredType     = DBMCLI_DEVSPACETYPE_UNKNOWN;
    m_sPreparedMirroredTypeName = "";
  }

} // end DBMCli_Devspace :: PrepareMirroredProperties

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Devspace :: Refresh
  -----------------------------------------------------------------------------
*/

SAPDB_Bool DBMCli_Devspace :: Refresh ( Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  DBMCli_Database & oDB = GetDatabase();
  DBMCli_Result & oResult = oDB.GetResult( ); 

  DBMCli_String sNum;
  sNum.Format("%d", m_nNumber);
  
  DBMCli_String sCmd( DBMCLI_CMD_PARAM_GETDEVSPACE );
  sCmd += " ";
  sCmd += sNum;
  sCmd += " ";
  sCmd += m_sClassName;
  
  Clear( );
  
  if( oDB.Execute( sCmd, oMsgList ) ) {
    DBMCli_String sType;
    DBMCli_String sPages;

    if( oResult.GetLine( m_sLocation ) ) {
      m_sLocation.Trim();

      if( oResult.GetLine( sType ) ) {
        sType.Trim( );
        AssignType( sType );
        AssignTypeName( m_nType );

        if( oResult.GetLine( sPages ) ) {
          sPages.Trim( );
          m_nPages = atol( sPages );
        }
      }
    }

    bRC = true;
  }
   
  return bRC;
} 

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Devspace :: Complete
  -----------------------------------------------------------------------------
*/

SAPDB_Bool DBMCli_Devspace :: Complete ( )
{
  SAPDB_Bool bRC = false;

  switch( m_nClass ) {
    case DBMCLI_DEVSPACECLASS_SYS:
      if( ( m_nType != DBMCLI_DEVSPACETYPE_UNKNOWN ) && ( !m_sLocation.IsEmpty( ) ) ) {
        bRC = true;
      }
      break;

    case DBMCLI_DEVSPACECLASS_DATA:
    case DBMCLI_DEVSPACECLASS_LOG:
      if( ( m_nType != DBMCLI_DEVSPACETYPE_UNKNOWN ) && ( !m_sLocation.IsEmpty( ) ) && ( m_nPages > 0 ) ) {
        if( m_bMirrored ) {
          if( ( m_nMirroredType != DBMCLI_DEVSPACETYPE_UNKNOWN ) && ( !m_sMirroredLocation.IsEmpty( ) ) ) {
            bRC = true;
          }
        } else {
          bRC = true;
        }
      }
      break;

    default:
      break;
  }

  return bRC;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Devspace :: Put
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Devspace :: Put ( Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  DBMCli_Database & oDB = GetDatabase();
  DBMCli_Result & oResult = oDB.GetResult( ); 

  DBMCli_String sTypeChar = oDB.GetDevspaces( ).TypeCharFromType( m_nType );
   
  DBMCli_String sCmd;

  sCmd.Format( "%s %d %s %s %s %d", DBMCLI_CMD_PARAM_ADDDEVSPACE,
                                    m_nNumber,
                                    (const char *)m_sClassName,
                                    (const char *)m_sLocation,
                                    (const char *)sTypeChar,
                                    m_nPages );

  if( oDB.Execute( sCmd, oMsgList ) ) {

    if( m_bMirrored ) {
      sTypeChar = oDB.GetDevspaces( ).TypeCharFromType( m_nMirroredType );

      sCmd.Format( "%s %d M%s %s %s", DBMCLI_CMD_PARAM_ADDDEVSPACE,
                                      m_nNumber,
                                      (const char *)m_sClassName,
                                      (const char *)m_sMirroredLocation,
                                      (const char *)sTypeChar );

      if( oDB.Execute( sCmd, oMsgList ) ) {
        bRC = true;
      }
    } else {

      bRC = true;
    }
  }

  return bRC;
} 

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Devspace :: PutPreparedMirror
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Devspace :: PutPreparedMirror ( Msg_List & oMsgList )
{
  DBMCli_String sTypeChar;
  DBMCli_String sCmd;

  sTypeChar = GetDatabase().GetDevspaces( ).TypeCharFromType( m_nPreparedMirroredType );
  sCmd.Format( "%s %d M%s %s %s", DBMCLI_CMD_PARAM_ADDDEVSPACE,
                                  m_nNumber,
                                  (const char *)m_sClassName,
                                  (const char *)m_sPreparedMirroredLocation,
                                  (const char *)sTypeChar );

  return GetDatabase().Execute( sCmd, oMsgList );
} // end DBMCli_Devspace :: PutPreparedMirror

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Devspace :: DeleteMirror
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Devspace :: DeleteMirror ( Msg_List & oMsgList )
{
  DBMCli_String sCmd;

  sCmd.Format( "%s %d M%s", DBMCLI_CMD_PARAM_DELDEVSPACE,
                            m_nNumber,
                            (const char *)m_sClassName);
  
  return GetDatabase().Execute( sCmd, oMsgList );
} // end DBMCli_Devspace :: DeleteMirror

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Devspace :: Reintegrate
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Devspace :: Reintegrate ( Msg_List & oMsgList )
{
  SAPDB_Bool             bRc = true;
  DBMCli_Database      & oDB = GetDatabase();
  Msg_List   oTmpList;
  DBMCli_String          sCmd;


  bRc = oDB.UTLConnect(oMsgList);
  if (bRc) {
    sCmd = DBMCLI_CMD_RECOVER_DEVSPACE;
    sCmd += " ";
    sCmd += m_sMirroredLocation;

    bRc = oDB.Execute(sCmd, oMsgList);
    oDB.UTLRelease(oTmpList);
  } // end if

  return bRc;
} // end DBMCli_Devspace :: Reintegrate

/*
  -----------------------------------------------------------------------------
  private functions ...
  -----------------------------------------------------------------------------
*/
void DBMCli_Devspace :: AssignClass ( const DBMCli_String & sClass )
{
  if( sClass == PARAM_LOGPARAM ) {
    m_nClass = DBMCLI_DEVSPACECLASS_LOG;

  } else if( sClass == PARAM_LOGPARAM744 ) {
    m_nClass = DBMCLI_DEVSPACECLASS_LOG;

  } else if( sClass == PARAM_DATAPARAM ) {
    m_nClass = DBMCLI_DEVSPACECLASS_DATA;

  } else if( sClass == PARAM_DATAPARAM744 ) {
    m_nClass = DBMCLI_DEVSPACECLASS_DATA;

  } else {
    m_nClass = DBMCLI_DEVSPACECLASS_UNKNOWN;
  }
}

void DBMCli_Devspace :: AssignName ( const DBMCli_DevspaceClass nClass,
                                     const SAPDB_Int            nNumber )
{
  DBMCli_String sNumber;

  switch( nClass ) {
    case DBMCLI_DEVSPACECLASS_SYS: 
      m_sName = PARAM_VAL_DEVSPACECLASS_SYS;
      sNumber.Format( NUMFORMAT_SYS, nNumber );
      m_sName += sNumber;
      break;

    case DBMCLI_DEVSPACECLASS_LOG: 
      m_sName = PARAM_VAL_DEVSPACECLASS_LOG;
      sNumber.Format( NUMFORMAT_LOG, nNumber );
      m_sName += sNumber;
      break;

    case DBMCLI_DEVSPACECLASS_DATA:
      m_sName = PARAM_VAL_DEVSPACECLASS_DATA;
      sNumber.Format( NUMFORMAT_DATA, nNumber );
      m_sName += sNumber;
      break;

    default:
      sNumber = "0";
      break;
  }
}

void DBMCli_Devspace :: AssignClassName ( const DBMCli_DevspaceClass nClass )
{

  switch( nClass )
  {
    case DBMCLI_DEVSPACECLASS_SYS: 
      m_sClassName = PARAM_VAL_DEVSPACECLASS_SYS;
      break;
    case DBMCLI_DEVSPACECLASS_DATA: 
      m_sClassName = PARAM_VAL_DEVSPACECLASS_DATA;
      break;
    case DBMCLI_DEVSPACECLASS_LOG: 
      m_sClassName = PARAM_VAL_DEVSPACECLASS_LOG;
      break;
    default:
      m_sClassName = "";
      break;
  }
}

void DBMCli_Devspace :: AssignType ( const DBMCli_String & sType, 
                                     const SAPDB_Bool      bMirrored )
{
  DBMCli_DevspaceType nDT = DBMCLI_DEVSPACETYPE_UNKNOWN;

  if( sType == PARAM_VAL_DEVSPACETYPE_FILE ) {
    nDT = DBMCLI_DEVSPACETYPE_FILE;
  } else if( sType == PARAM_VAL_DEVSPACETYPE_RAW ) {
    nDT = DBMCLI_DEVSPACETYPE_RAW;
  } else if( sType == PARAM_VAL_DEVSPACETYPE_LINK ) {
    nDT = DBMCLI_DEVSPACETYPE_LINK;
  }

  if( bMirrored ) {
    m_nMirroredType = nDT;
  } else {
    m_nType = nDT;
  }
}

void  DBMCli_Devspace :: AssignTypeName ( const DBMCli_DevspaceType nType,
                                          const SAPDB_Bool          bMirrored )
{
  DBMCli_String sTN;

  switch( nType ) {
    case DBMCLI_DEVSPACETYPE_FILE:
      sTN = TYPENAME_FILE;
      break;
    case DBMCLI_DEVSPACETYPE_RAW:
      sTN = TYPENAME_RAW;
      break;
    case  DBMCLI_DEVSPACETYPE_LINK:
      sTN = TYPENAME_LINK;
      break;
    default:
      sTN = "";
      break;
  }

  if( bMirrored ) {
    m_sMirroredTypeName = sTN;
  } else {
    m_sTypeName = sTN; 
  }
}

/*    
  =============================================================================
  class: DBMCli_Devspaces
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMCli_Devspaces :: DBMCli_Devspaces
  -----------------------------------------------------------------------------
*/
DBMCli_Devspaces :: DBMCli_Devspaces ( )
{
  Clear( );
}

/*
  -----------------------------------------------------------------------------
  public destructor DBMCli_Devspaces :: ~DBMCli_Devspaces
  -----------------------------------------------------------------------------
*/
DBMCli_Devspaces :: ~DBMCli_Devspaces ( )
{
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Devspaces :: Clear
  -----------------------------------------------------------------------------
*/
void DBMCli_Devspaces :: Clear ( )
{
  m_aDevspace.RemoveAll( );

  m_nLogMode = DBMCLI_LOGMODE_UNKNOWN;
  m_nMaxLogDevs = 0;
  m_nUsedLogDevs = 0;
  m_nMaxDataDevs = 0;
  m_nUsedDataDevs = 0;
  m_nMaxDataPages = 0;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Devspaces :: Refresh
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Devspaces :: Refresh ( Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  Clear( );

  // add fixed system devspace
  DBMCli_Devspace oSysDevspace( DBMCLI_DEVSPACECLASS_SYS, 1 );
  oSysDevspace.SetDatabase( m_pDatabase );
  if( oSysDevspace.Refresh( oMsgList ) ) {
    m_aDevspace.Add( oSysDevspace );
  } else {
    oMsgList.ClearMessageList();
  } // end if

  // get params and data/log devspaces
  DBMCli_Database & oDB = GetDatabase();
  DBMCli_Result & oResult = oDB.GetResult( ); 

  DBMCli_String sCmd( DBMCLI_CMD_PARAM_GETDEVSALL );
  
  if ( oDB.Execute( sCmd, oMsgList ) ) {
    DBMCli_ResultBuf sLine;

    while( oResult.GetLine( sLine ) ) {
      DBMCli_String sName;
      DBMCli_String sValue;
      DBMCli_String sType;
      DBMCli_String sLocation;

      if( sLine.GetField( sName, " \n" ) ) {
        sName.Trim( );
        sLine.SkipBlanks( );

        DBMCli_String sLog( PARAM_LOGPARAM );
        DBMCli_String sClass;

        if( sName.GetLength( ) >= sLog.GetLength( ) ) {
          sClass = sName.Left( sLog.GetLength( ) );
        }

        if( ( sClass == PARAM_MLOGPARAM )     || 
            ( sClass == PARAM_MDATAPARAM )    ||
            ( sClass == PARAM_MLOGPARAM744 )  ||
            ( sClass == PARAM_MDATAPARAM744 )    ) {

          if( sLine.GetField( sType, " \n" ) ) {
            sType.Trim( );
            sLine.SkipBlanks( );
            if( sLine.GetField( sLocation, "\n\0" ) ) {
              m_aDevspace[m_aDevspace.GetSize( ) - 1].SetMirroredProperties( true, sLocation, sType );
            } 
          }

        } else {

          if( sLine.GetField( sValue, " \n" ) ) {
            sValue.Trim( );
            sLine.SkipBlanks( );

            if( sName == PARAM_LOGMODE ) {
              AssignLogMode( sValue );
            } else if( sName == PARAM_MAXLOGDEVS ) {
              m_nMaxLogDevs = atoi( sValue );
            } else if( sName == PARAM_MAXLOGVOLS ) {
              m_nMaxLogDevs = atoi( sValue );
            } else if( sName == PARAM_MAXDATADEVS ) {
              m_nMaxDataDevs = atoi( sValue );
            } else if( sName == PARAM_MAXDATAVOLS ) {
              m_nMaxDataDevs = atoi( sValue );
            } else if( sName == PARAM_MAXDATAPAGES ) {
              m_nMaxDataPages = atoi( sValue );
            } else {

              if( sLine.GetField( sType, " \n" ) ) {
                sType.Trim( );
                sLine.SkipBlanks( );
                if( sLine.GetField( sLocation, "\n\0" ) ) {

                  DBMCli_Devspace oDevspace( sName, sValue, sLocation, sType );
                  oDevspace.SetDatabase( m_pDatabase );
                  m_aDevspace.Add( oDevspace );

                  switch( oDevspace.Class( ) ) {
                    case DBMCLI_DEVSPACECLASS_LOG: 
                      m_nUsedLogDevs++;
                      break;
                    case DBMCLI_DEVSPACECLASS_DATA:
                      m_nUsedDataDevs++;
                      break;
                    default:
                      break;
                  }
                }
              }
            }
          }
        }
      }

      sLine.Clear( );  
    }

    bRC = true;
  }

  if (m_aDevspace.GetSize() > 0) {
    if (m_aDevspace[0].Class() == DBMCLI_DEVSPACECLASS_SYS) {
      m_aDevspace[0].SetPages( CalcSysDevspacePages( ) );
    } // end if
  } // end if
    
  return bRC;
} 

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Devspaces :: Init
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Devspaces :: Init ( Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;
  SAPDB_Int nItem;

  if( Refresh( oMsgList ) ) {
    
    if( m_nUsedDataDevs < m_nMaxDataDevs ) {
      for( nItem = m_nUsedDataDevs + 1; nItem <= m_nMaxDataDevs; nItem++ ) {
        DBMCli_Devspace oDevspace;
        oDevspace.SetDatabase( m_pDatabase );
        oDevspace.SetProperties( DBMCLI_DEVSPACECLASS_DATA, nItem );
        m_aDevspace.Add( oDevspace );
      }
    }

    if( m_nUsedLogDevs < m_nMaxLogDevs ) {
      for( nItem = m_nUsedLogDevs + 1; nItem <= m_nMaxLogDevs; nItem++ ) {
        DBMCli_Devspace oDevspace;
        oDevspace.SetDatabase( m_pDatabase );
        oDevspace.SetProperties( DBMCLI_DEVSPACECLASS_LOG, nItem );
        m_aDevspace.Add( oDevspace );
      }
    }

    for( nItem = 0; nItem < m_aDevspace.GetSize( ); nItem++ ) {
      if( m_aDevspace[nItem].Class( ) == DBMCLI_DEVSPACECLASS_LOG ) {
        if( m_nLogMode == DBMCLI_LOGMODE_DUAL ) {
          m_aDevspace[nItem].SetMirrored( true );
        }
      }
    }

    bRC = true;
  }
  
  return bRC;
}


/*
  -----------------------------------------------------------------------------
  public function DBMCli_Devspaces :: Add
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Devspaces :: Add ( const DBMCli_String        & sClass,
                                     const DBMCli_String        & sLocation,
                                     const DBMCli_String        & sType,
                                     const DBMCli_String        & sSize,
                                     const DBMCli_String        & sMirroredLocation,
                                     const DBMCli_String        & sMirroredType,
                                           Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  DBMCli_Database & oDB = GetDatabase();
  
  DBMCli_String sCmd;

  sCmd = DBMCLI_CMD_DBADDDEVICE + DBMCli_String(" ");
  sCmd += sClass + " ";
  sCmd += sLocation + " ";

  if( !sType.IsEmpty( ) ) {
    sCmd += sType.Left( 1 ) + " ";
  }

  sCmd += sSize + " ";

  if( !sMirroredLocation.IsEmpty( ) ) {
    sCmd += sMirroredLocation + " ";
  }

  if( !sMirroredType.IsEmpty( ) ) {
    sCmd += sMirroredType.Left( 1 ) + " ";
  }

  if( oDB.Execute( sCmd, oMsgList ) ) {
    bRC = true;
  }

  return bRC;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Devspaces :: Reintegrate
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Devspaces :: Reintegrate ( const DBMCli_String        & sName,
                                                   Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  DBMCli_Database & oDB = GetDatabase();
  
  DBMCli_String sCmd;

  sCmd = DBMCLI_CMD_RECOVER_DEVSPACE;
  sCmd += " ";
  sCmd += sName;

  if( oDB.Execute( sCmd, oMsgList ) ) {
    bRC = true;
  }

  return bRC;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Devspaces :: NewDevspace
  -----------------------------------------------------------------------------
*/
DBMCli_Devspace & DBMCli_Devspaces :: NewDevspace( const DBMCli_String & sClass )
{
  DBMCli_DevspaceClass nClass;
  SAPDB_Bool bFound = false;
  SAPDB_Int nDevspace;
  SAPDB_Int nNextNum;

  nClass = ClassFromClassName( sClass );

  switch( nClass ) {
    case DBMCLI_DEVSPACECLASS_DATA:
      nNextNum = m_nUsedDataDevs + 1;
      break;
    case DBMCLI_DEVSPACECLASS_LOG:
      nNextNum = m_nUsedLogDevs + 1;
      break;
    default:
      nNextNum = 0;
      break;
  }

  m_oNewDevspace.SetDatabase( m_pDatabase );

  nDevspace = m_aDevspace.GetSize( ) - 1;
  while( ( nDevspace >= 0 ) && ( bFound == false ) ) {
    if( m_aDevspace[nDevspace].Class( ) == nClass ) {
      bFound = true;
    } else {
      nDevspace--;
    }
  }

  if( bFound ) {
    m_oNewDevspace.SetProperties( nClass, 
                                  nNextNum, 
                                  m_aDevspace[nDevspace].Pages( ), 
                                  GetNextLocation( m_aDevspace[nDevspace].Location( ) ),
                                  m_aDevspace[nDevspace].Type( ),
                                  m_aDevspace[nDevspace].Mirrored( ),
                                  GetNextLocation( m_aDevspace[nDevspace].MirroredLocation( ) ),
                                  m_aDevspace[nDevspace].MirroredType( ) );
  } else {
    m_oNewDevspace.SetProperties( nClass, nNextNum);
  }


  return m_oNewDevspace;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Devspaces :: InitDevspace
  -----------------------------------------------------------------------------
*/
void DBMCli_Devspaces :: InitDevspace( DBMCli_Devspace & oDevspace )
{
  SAPDB_Bool bFound = false;
  SAPDB_Int nDevspace;

  if( oDevspace.Class( ) != DBMCLI_DEVSPACECLASS_SYS ) {
    nDevspace = m_aDevspace.GetSize( ) - 1;
    while( ( nDevspace >= 0 ) && ( bFound == false ) ) {
      if( ( m_aDevspace[nDevspace].Class( ) == oDevspace.Class( ) )   && 
          ( m_aDevspace[nDevspace].Number( ) != oDevspace.Number( ) )    ) { 
        bFound = true;
      } else {
        nDevspace--;
      }
    }

    if( bFound ) {
      if( oDevspace.Pages( ) == 0 ) {
        oDevspace.SetPages( m_aDevspace[nDevspace].Pages( ) );
      }
      if( oDevspace.Location( ).IsEmpty( ) ) {
        oDevspace.SetLocation( GetNextLocation( m_aDevspace[nDevspace].Location( ) ) );
      }
      if( oDevspace.Type( ) == DBMCLI_DEVSPACETYPE_UNKNOWN ) {
        oDevspace.SetDeviceType( m_aDevspace[nDevspace].Type( ) );
      }
      if( oDevspace.MirroredLocation( ).IsEmpty( ) ) {
        oDevspace.SetMirroredLocation( GetNextLocation( m_aDevspace[nDevspace].MirroredLocation( ) ) );
      }
      if( oDevspace.MirroredType( ) == DBMCLI_DEVSPACETYPE_UNKNOWN ) {
        oDevspace.SetMirroredDeviceType( m_aDevspace[nDevspace].MirroredType( ) );
      }
    } else {
      oDevspace.SetPages( 0 );
      switch( oDevspace.Class( ) ) {
        case DBMCLI_DEVSPACECLASS_DATA:
          oDevspace.SetLocation( "DAT_0001" );
          oDevspace.SetMirroredLocation( "MDAT_0001" );
          break;
        case DBMCLI_DEVSPACECLASS_LOG:
          oDevspace.SetLocation( "LOG_001" );
          oDevspace.SetMirroredLocation( "MLOG_001" );
          break;
        default:
          oDevspace.SetLocation( "" );
          oDevspace.SetMirroredLocation( "" );
          break;
      }
      oDevspace.SetDeviceType( DBMCLI_DEVSPACETYPE_FILE );
      oDevspace.SetMirroredDeviceType( DBMCLI_DEVSPACETYPE_FILE );
    }
  } else {
    oDevspace.SetPages( CalcSysDevspacePages( ) );
    oDevspace.SetLocation( "SYS_001" );
    oDevspace.SetDeviceType( DBMCLI_DEVSPACETYPE_FILE );
  }
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Devspaces :: ClassFromName
  -----------------------------------------------------------------------------
*/
DBMCli_DevspaceClass DBMCli_Devspaces :: ClassFromClassName ( const DBMCli_String & sClass )
{
  DBMCli_String s( sClass );

  s.Trim( );
  s.MakeUpper( );
  
  if( s == PARAM_VAL_DEVSPACECLASS_SYS ) {
    return DBMCLI_DEVSPACECLASS_SYS;
  } else if( s == PARAM_VAL_DEVSPACECLASS_DATA ) {
    return DBMCLI_DEVSPACECLASS_DATA;
  } else if( s == PARAM_VAL_DEVSPACECLASS_LOG ) {
    return DBMCLI_DEVSPACECLASS_LOG;
  } else {
    return DBMCLI_DEVSPACECLASS_UNKNOWN;
  }
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Devspaces :: ClassNameFromClass
  -----------------------------------------------------------------------------
*/
DBMCli_String DBMCli_Devspaces :: ClassNameFromClass ( const DBMCli_DevspaceClass nClass )
{
  switch( nClass ) {
    case DBMCLI_DEVSPACECLASS_SYS:
      return PARAM_VAL_DEVSPACECLASS_SYS;
    case DBMCLI_DEVSPACECLASS_DATA:
      return PARAM_VAL_DEVSPACECLASS_DATA;
    case DBMCLI_DEVSPACECLASS_LOG:
      return PARAM_VAL_DEVSPACECLASS_LOG;
    default:
      return "";
  }
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Devspaces :: TypeFromName
  -----------------------------------------------------------------------------
*/
DBMCli_DevspaceType DBMCli_Devspaces :: TypeFromTypeName ( const DBMCli_String & sType )
{
  DBMCli_String s( sType );

  s.Trim( );
  s.MakeUpper( );
  
  if( s == TYPENAME_FILE ) {
    return DBMCLI_DEVSPACETYPE_FILE;
  } else if( s == TYPENAME_RAW ) {
    return DBMCLI_DEVSPACETYPE_RAW;
  } else if( s == TYPENAME_LINK ) {
    return DBMCLI_DEVSPACETYPE_LINK;
  } else {
    return DBMCLI_DEVSPACETYPE_UNKNOWN;
  }
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Devspaces :: TypeNameFromType
  -----------------------------------------------------------------------------
*/
DBMCli_String DBMCli_Devspaces :: TypeNameFromType ( const DBMCli_DevspaceType nType )
{
  switch( nType ) {
    case DBMCLI_DEVSPACETYPE_FILE:
      return TYPENAME_FILE;
    case DBMCLI_DEVSPACETYPE_RAW:
      return TYPENAME_RAW;
    case DBMCLI_DEVSPACETYPE_LINK:
      return TYPENAME_LINK;
    default:
      return "";
  }
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Devspaces :: TypeCharFromType
  -----------------------------------------------------------------------------
*/
DBMCli_String DBMCli_Devspaces :: TypeCharFromType ( const DBMCli_DevspaceType nType )
{
  switch( nType ) {
    case DBMCLI_DEVSPACETYPE_FILE:
      return PARAM_VAL_DEVSPACETYPE_FILE;
    case DBMCLI_DEVSPACETYPE_RAW:
      return PARAM_VAL_DEVSPACETYPE_RAW;
    case DBMCLI_DEVSPACETYPE_LINK:
      return PARAM_VAL_DEVSPACETYPE_LINK;
    default:
      return PARAM_VAL_DEVSPACETYPE_FILE;
  }
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Devspaces :: IndexByName
  -----------------------------------------------------------------------------
*/
SAPDB_Int DBMCli_Devspaces :: IndexByName ( const DBMCli_String & sName )
{
  SAPDB_Bool bFound = false;
  SAPDB_Int nDevspace = 0;

  while( nDevspace < m_aDevspace.GetSize( ) && !bFound ) {
    if( m_aDevspace[nDevspace].Name( ) == sName ) {
      bFound = true;
    } else {
      nDevspace++;
    }
  }

  if( bFound ) {
    return nDevspace;
  } else {
    return -1;
  }
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Devspaces :: Complete
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Devspaces :: Complete ( )
{
  SAPDB_Bool bRC = false;

  SAPDB_Int nDevspace = 0;

  SAPDB_Bool bSys = false;
  SAPDB_Bool bDat = false;
  SAPDB_Bool bLog = false;

  for( nDevspace = 0; nDevspace < m_aDevspace.GetSize( ); nDevspace++ ) {
    if( m_aDevspace[nDevspace].Pages( ) > 0 ) {
      if( ( m_aDevspace[nDevspace].Class( ) == DBMCLI_DEVSPACECLASS_DATA ) ||
          ( m_aDevspace[nDevspace].Class( ) == DBMCLI_DEVSPACECLASS_LOG )     ) {
        if( m_aDevspace[nDevspace].Complete( ) ){
          bRC = true;
        }
      }

    }
  }

  if( bRC ) {
    bRC  = false;
    bSys = true;
    for( nDevspace = 0; nDevspace < m_aDevspace.GetSize( ); nDevspace++ ) {
  
      switch( m_aDevspace[nDevspace].Class( ) ) {
        case DBMCLI_DEVSPACECLASS_SYS:
          bSys = false;
          if( !bSys && m_aDevspace[nDevspace].Complete( ) ) {
            bSys = true;
          }
          break; 
        case DBMCLI_DEVSPACECLASS_DATA:
          if( !bDat && m_aDevspace[nDevspace].Complete( ) ) {
            bDat = true;
          }
          break; 
        case DBMCLI_DEVSPACECLASS_LOG:
          if( !bLog && m_aDevspace[nDevspace].Complete( ) ) {
            bLog = true;
          }
          break; 
        default:
          break; 
      }

      if( bSys && bLog && bDat ) {
        bRC = true;
      }
    }
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
  private function DBMCli_Devspaces :: AssignLogMode
  -----------------------------------------------------------------------------
*/
void DBMCli_Devspaces :: AssignLogMode ( const DBMCli_String  & sLogMode )
{
  m_sLogMode = sLogMode;

  if( sLogMode == PARAM_VAL_LOGMODE_DUAL ) {
    m_nLogMode = DBMCLI_LOGMODE_DUAL;
  } else if( sLogMode == PARAM_VAL_LOGMODE_SINGLE ) {
    m_nLogMode = DBMCLI_LOGMODE_SINGLE;
  } else if( sLogMode == PARAM_VAL_LOGMODE_DEMO ) {
    m_nLogMode = DBMCLI_LOGMODE_DEMO;
  } else {
    m_nLogMode = DBMCLI_LOGMODE_UNKNOWN;
  }
}

/*
  -----------------------------------------------------------------------------
  private function DBMCli_Devspaces :: GetNextLocation
  -----------------------------------------------------------------------------
*/
DBMCli_String DBMCli_Devspaces :: GetNextLocation( const DBMCli_String & sLocation )
{
  DBMCli_String sNew;

  SAPDB_Int nPos1;
  SAPDB_Int nPos2;
  DBMCli_String sPath;
  DBMCli_String sName;
  DBMCli_String sLeft;
  DBMCli_String sRight;
  DBMCli_String sNum;
  SAPDB_Int nNum; 
  SAPDB_Int nFormat; 
  DBMCli_String sFormat;

  if( !sLocation.IsEmpty( ) ) {
    // get path and name
    nPos1 = sLocation.GetLength( ) - 1;
    while( nPos1 >= 0 ) {
      if( ( sLocation[ nPos1 ] == '\\' ) || ( sLocation[ nPos1 ] == '/' ) )
        break;
      nPos1--;
    }

    if( nPos1 >= 0 ) {
      sPath = sLocation.Left( nPos1 + 1);
      sName = sLocation.Right( sLocation.GetLength( ) - nPos1 - 1);
    } else {
      sPath.Empty( );
      sName = sLocation;
    }

    // find number part in name
    nPos2 = sName.GetLength( ) - 1;
    while( nPos2 >= 0 ) {
      if( ( sName[ nPos2 ] >= '0' ) && ( sName[ nPos2 ] <= '9' ) )
        break;
      nPos2--;
    }

    if( nPos2 >= 0 ) {
      nPos1 = nPos2 - 1;
      while( nPos1 >= 0 ) {
        if( ( sName[ nPos1 ] < '0' ) || ( sName[ nPos1 ] > '9' ) )
          break;
        nPos1--;
      }

      // increment number
      if( nPos1 + 1 >= 0 ) {
        sLeft = sName.Left( nPos1 + 1 );
        sNum = sName.Mid( nPos1 + 1, nPos2 - nPos1 );
      }
      if( nPos2 < sName.GetLength( ) - 1 )
        sRight = sName.Mid( nPos2 + 1);

      nFormat = sNum.GetLength( );
      sFormat.Format( "%%.%dd", nFormat );

      nNum = atoi( sNum ) + 1;
      sNum.Format(sFormat, nNum );

      sNew = sPath + sLeft + sNum + sRight;
    } else {
      sNew = sLocation + "2";
    }
  }

  return sNew;        
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Devspaces :: DataPagesAfterRestart
  -----------------------------------------------------------------------------
*/
SAPDB_Int4 DBMCli_Devspaces :: DataPagesAfterRestart ( )
{
  SAPDB_Int nDevspace = 0;

  SAPDB_Int4 nSum = 0;
  SAPDB_Int4 nMax = 0;

  for( nDevspace = 0; nDevspace < m_aDevspace.GetSize( ); nDevspace++ ) {
    if( m_aDevspace[nDevspace].Class( ) ==  DBMCLI_DEVSPACECLASS_DATA ) {
      nSum += m_aDevspace[nDevspace].Pages( );
      if( m_aDevspace[nDevspace].Pages( ) > nMax ) {
        nMax = m_aDevspace[nDevspace].Pages( );
      }
    }
  }

  return( nSum + nMax );
}

/*
  -----------------------------------------------------------------------------
  private function DBMCli_Devspaces :: CalcSysDevspacePages
  -----------------------------------------------------------------------------
*/
SAPDB_Int4 DBMCli_Devspaces :: CalcSysDevspacePages( )
{
  SAPDB_Int4 nDataPagesAfterRestart = 0;
  SAPDB_Int4 nDataPages = 0;
  SAPDB_Int4 nSingleConvSize = 0;
  SAPDB_Int4 nIOBlockSizeDelta = 0;
  
  nDataPagesAfterRestart = DataPagesAfterRestart( );
  
  if( nDataPagesAfterRestart > m_nMaxDataPages ) {
    nDataPages = nDataPagesAfterRestart;
  } else {
    nDataPages = m_nMaxDataPages;
  }
  
  nSingleConvSize = (m_nMaxDataPages / MAX_CONV_REC_BD00) + 1;
  nIOBlockSizeDelta = SYS_DEV_BLOCK_CNT_BD01 - (nSingleConvSize % SYS_DEV_BLOCK_CNT_BD01);
  
  return( (nSingleConvSize * 2) + OF_CONV_0_BD15 + nIOBlockSizeDelta );
}