/*! 
  -----------------------------------------------------------------------------
  module: DBMCli_Media.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager Client Library
  description:  Media Class
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
#include "DBM/Cli/DBMCli_Media.hpp"
#include "DBM/Cli/DBMCli_DateTime.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define PARAM_MAXBACKUPDEVS "MAXBACKUPDEVS"

#define FIELD_SEP "\t\n"

#define KEY_BACKSLASH '\\'
#define KEY_SLASH     '/'

#define MEDIUMTYPENAME_UNKNOWN  "UNKNOWN"
#define MEDIUMTYPENAME_PARALLEL "PARALLEL"
#define MEDIUMTYPENAME_SINGLE   "SINGLE"

#define MEDIUMDEVICETYPENAME_UNKNOWN "UNKNOWN"
#define MEDIUMDEVICETYPENAME_FILE    "FILE"
#define MEDIUMDEVICETYPENAME_TAPE    "TAPE"
#define MEDIUMDEVICETYPENAME_PIPE    "PIPE"
#define MEDIUMDEVICETYPEKERN_FILE    "vftFile"
#define MEDIUMDEVICETYPEKERN_TAPE    "vftTapeRew"
#define MEDIUMDEVICETYPEKERN_PIPE    "vftPipe"

#define MEDIUMEXTERNTYPENAME_UNKNOWN "UNKNOWN"
#define MEDIUMEXTERNTYPENAME_NONE    "NONE"
#define MEDIUMEXTERNTYPENAME_NSR     "NSR"
#define MEDIUMEXTERNTYPENAME_ADSM    "ADSM"
#define MEDIUMEXTERNTYPENAME_BACK    "BACK"
#define MEDIUMEXTERNTYPENAME_OMNI    "OMNI"

/*    
  =============================================================================
  class: DBMCli_MediumType
  =============================================================================
*/
/*
  -----------------------------------------------------------------------------
  public constructor DBMCli_MediumType :: DBMCli_MediumType
  -----------------------------------------------------------------------------
*/
DBMCli_MediumType :: DBMCli_MediumType ( )
{
  m_nMediumType = DBMCLI_MEDIUMTYPE_UNKNOWN;
}

DBMCli_MediumType :: DBMCli_MediumType ( const DBMCli_String & sMediumType )
{
  SetName( sMediumType );
}

DBMCli_MediumType :: DBMCli_MediumType ( const DBMCli_MediumTypeEnum nMediumType )
                   : m_nMediumType( nMediumType )
{
}

/*
  -----------------------------------------------------------------------------
  public destructor DBMCli_MediumType :: ~DBMCli_MediumType
  -----------------------------------------------------------------------------
*/
DBMCli_MediumType :: ~DBMCli_MediumType ( )
{
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_MediumType :: Name
  -----------------------------------------------------------------------------
*/
const DBMCli_String DBMCli_MediumType :: Name ( )
{
  DBMCli_String sName;

  switch( m_nMediumType ) {
    case DBMCLI_MEDIUMTYPE_SINGLE:
      sName = MEDIUMTYPENAME_SINGLE;
      break;
    case DBMCLI_MEDIUMTYPE_PARALLEL:
      sName = MEDIUMTYPENAME_PARALLEL;
      break;
    default:
      sName = MEDIUMTYPENAME_UNKNOWN;
      break;
  }

  return sName;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_MediumType :: SetName
  -----------------------------------------------------------------------------
*/
void DBMCli_MediumType :: SetName ( const DBMCli_String & sName )
{
  if( sName == MEDIUMTYPENAME_SINGLE ) {
    m_nMediumType = DBMCLI_MEDIUMTYPE_SINGLE;
  } else if( sName == MEDIUMTYPENAME_PARALLEL ) {
    m_nMediumType = DBMCLI_MEDIUMTYPE_PARALLEL;
  } else {
    m_nMediumType = DBMCLI_MEDIUMTYPE_UNKNOWN;
  }
}

/*    
  =============================================================================
  class: DBMCli_MediumDeviceType
  =============================================================================
*/
/*
  -----------------------------------------------------------------------------
  public constructor DBMCli_MediumDeviceType :: DBMCli_MediumDeviceType
  -----------------------------------------------------------------------------
*/
DBMCli_MediumDeviceType :: DBMCli_MediumDeviceType ( )
{
  m_nMediumDeviceType = DBMCLI_MEDIUMDEVICETYPE_UNKNOWN;
}

DBMCli_MediumDeviceType :: DBMCli_MediumDeviceType ( const DBMCli_String & sMediumDeviceType )
{
  SetName( sMediumDeviceType );
}

DBMCli_MediumDeviceType :: DBMCli_MediumDeviceType ( const DBMCli_MediumDeviceTypeEnum nMediumDeviceType )
                         : m_nMediumDeviceType( nMediumDeviceType )
{
}

/*
  -----------------------------------------------------------------------------
  public destructor DBMCli_MediumDeviceType :: ~DBMCli_MediumDeviceType
  -----------------------------------------------------------------------------
*/
DBMCli_MediumDeviceType :: ~DBMCli_MediumDeviceType ( )
{
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_MediumDeviceType :: Name
  -----------------------------------------------------------------------------
*/
DBMCli_String DBMCli_MediumDeviceType :: Name ( )
{
  DBMCli_String sName;

  switch( m_nMediumDeviceType ) {
    case DBMCLI_MEDIUMDEVICETYPE_FILE:
      sName = MEDIUMDEVICETYPENAME_FILE;
      break;
    case DBMCLI_MEDIUMDEVICETYPE_TAPE:
      sName = MEDIUMDEVICETYPENAME_TAPE;
      break;
    case DBMCLI_MEDIUMDEVICETYPE_PIPE:
      sName = MEDIUMDEVICETYPENAME_PIPE;
      break;
    default:
      sName = MEDIUMDEVICETYPENAME_UNKNOWN;
      break;
  }

  return sName;
} // end DBMCli_MediumDeviceType :: Name

/*
  -----------------------------------------------------------------------------
  public function DBMCli_MediumDeviceType :: SetName
  -----------------------------------------------------------------------------
*/
void DBMCli_MediumDeviceType :: SetName ( const DBMCli_String & sName )
{
  if( sName == MEDIUMDEVICETYPENAME_FILE || 
      sName == MEDIUMDEVICETYPEKERN_FILE     ) {
    m_nMediumDeviceType = DBMCLI_MEDIUMDEVICETYPE_FILE;
  } else if( sName == MEDIUMDEVICETYPENAME_TAPE || 
             sName == MEDIUMDEVICETYPEKERN_TAPE     ) {
    m_nMediumDeviceType = DBMCLI_MEDIUMDEVICETYPE_TAPE;
  } else if( sName == MEDIUMDEVICETYPENAME_PIPE || 
             sName == MEDIUMDEVICETYPEKERN_PIPE     ) {
    m_nMediumDeviceType = DBMCLI_MEDIUMDEVICETYPE_PIPE;
  } else {
    m_nMediumDeviceType = DBMCLI_MEDIUMDEVICETYPE_UNKNOWN;
  } // end if
} // end DBMCli_MediumDeviceType :: SetName

/*    
  =============================================================================
  class: DBMCli_MediumExternType
  =============================================================================
*/
/*
  -----------------------------------------------------------------------------
  public constructor DBMCli_MediumExternType :: DBMCli_MediumExternType
  -----------------------------------------------------------------------------
*/
DBMCli_MediumExternType :: DBMCli_MediumExternType ( )
{
  m_nMediumExternType = ExtTypeUnknown;
} // end DBMCli_MediumExternType :: DBMCli_MediumExternType
DBMCli_MediumExternType :: DBMCli_MediumExternType ( const DBMCli_String & sMediumExternType )
{
  if (sMediumExternType == MEDIUMEXTERNTYPENAME_NONE) {
    m_nMediumExternType = ExtTypeNone;
  } else if (sMediumExternType == MEDIUMEXTERNTYPENAME_NSR) {
    m_nMediumExternType = ExtTypeNsr;
  } else if (sMediumExternType == MEDIUMEXTERNTYPENAME_ADSM) {
    m_nMediumExternType = ExtTypeAdsm;
  } else if (sMediumExternType == MEDIUMEXTERNTYPENAME_BACK) {
    m_nMediumExternType = ExtTypeBack;
  } else if (sMediumExternType == MEDIUMEXTERNTYPENAME_OMNI) {
    m_nMediumExternType = ExtTypeOmni;
  } else {
    m_nMediumExternType = ExtTypeUnknown;
  } // end if
  
} // end DBMCli_MediumExternType :: DBMCli_MediumExternType
DBMCli_MediumExternType :: DBMCli_MediumExternType ( const ExtType nMediumExternType )
                         : m_nMediumExternType( nMediumExternType )
{
} // end DBMCli_MediumExternType :: DBMCli_MediumExternType

/*
  -----------------------------------------------------------------------------
  public function DBMCli_MediumExternType :: Name
  -----------------------------------------------------------------------------
*/
DBMCli_String DBMCli_MediumExternType :: Name ( ) const 
{
  DBMCli_String sName;

  switch( m_nMediumExternType ) {
    case ExtTypeNone:
      sName = MEDIUMEXTERNTYPENAME_NONE;
      break;
    case ExtTypeNsr:
      sName = MEDIUMEXTERNTYPENAME_NSR;
      break;
    case ExtTypeAdsm:
      sName = MEDIUMEXTERNTYPENAME_ADSM;
      break;
    case ExtTypeBack:
      sName = MEDIUMEXTERNTYPENAME_BACK;
      break;
    case ExtTypeOmni:
      sName = MEDIUMEXTERNTYPENAME_OMNI;
      break;
    default:
      sName = MEDIUMEXTERNTYPENAME_UNKNOWN;
      break;
  }

  return sName;
} // end DBMCli_MediumExternType :: Name

/*    
  =============================================================================
  class: DBMCli_Medium
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMCli_Medium :: DBMCli_Medium
  -----------------------------------------------------------------------------
*/
DBMCli_Medium :: DBMCli_Medium ( )
{
}

DBMCli_Medium :: DBMCli_Medium ( const DBMCli_String & sGroup,
                                 const DBMCli_String & sName,
                                 const DBMCli_String & sLocation,
                                 const DBMCli_String & sDeviceType,
                                 const DBMCli_String & sBackupType,
                                 const DBMCli_String & sPages,
                                 const DBMCli_String & sBlockSize,
                                 const DBMCli_String & sOverwrite,
                                 const DBMCli_String & sAutoloader,
                                 const DBMCli_String & sOSCommand,
                                 const DBMCli_String & sExtern,
                                 const DBMCli_String & sCreated,
                                 const DBMCli_String & sModified,
                                 const DBMCli_String & sOldDefinition)
              : m_sGroup( sGroup ),
                m_sLocation( sLocation ),
                m_sDeviceType( sDeviceType ),
                m_sBackupType( sBackupType ),
                m_sPages( sPages ),
                m_sBlockSize( sBlockSize ),
                m_sOverwrite( sOverwrite ),
                m_sAutoloader( sAutoloader ),
                m_sOSCommand( sOSCommand ),
                m_sExtern( sExtern ),
                m_sCreated( sCreated ),
                m_sModified( sModified ),
                m_sOldDefinition( sOldDefinition )
{
}



/*
  -----------------------------------------------------------------------------
  public destructor DBMCli_Medium :: ~DBMCli_Medium
  -----------------------------------------------------------------------------
*/
DBMCli_Medium :: ~DBMCli_Medium ( )
{
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Medium :: Clear
  -----------------------------------------------------------------------------
*/
void DBMCli_Medium :: Clear ( )
{
  m_sGroup.Empty( );
  m_sName.Empty( );
  m_sLocation.Empty( );
  m_sDeviceType.Empty( );
  m_sBackupType.Empty( );
  m_sPages.Empty( );
  m_sBlockSize.Empty( );
  m_sOverwrite.Empty( );
  m_sAutoloader.Empty( );
  m_sOSCommand.Empty( );
  m_sExtern.Empty( );
  m_sCreated.Empty( );
  m_sModified.Empty( );
  m_sOldDefinition.Empty( );
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Medium :: Refresh
  -----------------------------------------------------------------------------
*/

SAPDB_Bool DBMCli_Medium :: Refresh ( Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  DBMCli_Database & oDB = GetDatabase();
  DBMCli_Result & oResult = oDB.GetResult( ); 

  
  DBMCli_String sCmd( DBMCLI_CMD_MEDIUM_GET );
  sCmd += " " + FullName( );
  
  Clear( );
  
  if ( oDB.Execute( sCmd, oMsgList ) ) {
    DBMCli_ResultBuf oLine;
    if( oResult.GetLine( oLine ) ) {
      SetPropertiesByLine( oLine );
    }
    bRC = true;
  }
   
  return bRC;
} 

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Medium :: Refresh
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Medium :: Put ( const DBMCli_String & sLocation,
                                  const DBMCli_String & sDeviceType,
                                  const DBMCli_String & sBackupType,
                                  const DBMCli_String & sSize,
                                  const DBMCli_String & sBlockSize,
                                  const DBMCli_String & sOverwrite,
                                  const DBMCli_String & sAutoloader,
                                  const DBMCli_String & sOSCommand,
                                  const DBMCli_String & sExtern,
                                  Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  DBMCli_Database & oDB = GetDatabase();

  DBMCli_String sCmd( DBMCLI_CMD_MEDIUM_PUT );

  sCmd += " "; 
  sCmd += "\"" + FullName( ) + "\"";
  sCmd += " "; 
  sCmd += "\"" + sLocation + "\"";
  sCmd += " "; 
  sCmd += sDeviceType;
  sCmd += " ";
  sCmd += sBackupType;
  
  sCmd += " ";
  if( sSize.IsEmpty( ) ) {
    sCmd += "0";
  } else {
    sCmd += sSize;
  }

  sCmd += " ";
  if( sBlockSize.IsEmpty( ) ) {
    sCmd += "8";
  } else {
    sCmd += sBlockSize;
  }

  sCmd += " ";
  if( sOverwrite.IsEmpty( ) ) {
    sCmd += DBMCLI_VAL_NO;
  } else {
    if( sOverwrite == DBMCLI_VAL_YES ) {
      sCmd += DBMCLI_VAL_YES;
    } else {
      sCmd += DBMCLI_VAL_NO;
    }
  }

  sCmd += " ";
  if( sAutoloader.IsEmpty( ) ) {
    sCmd += DBMCLI_VAL_NO;
  } else {
    if( sAutoloader == DBMCLI_VAL_YES ) {
      sCmd += DBMCLI_VAL_YES;
    } else {
      sCmd += DBMCLI_VAL_NO;
    }
  }

  sCmd += " ";
  if( sOSCommand.IsEmpty( ) ) {
    sCmd += "\"\"";
  } else {
    sCmd += "\"" + sOSCommand + "\"";
  }

  sCmd += " ";
  if( !sExtern.IsEmpty( ) ) {
    sCmd += sExtern;
  }

  if( oDB.Execute( sCmd, oMsgList ) ) {
    bRC = Refresh( oMsgList );
  }

  return bRC;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Medium :: Delete
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Medium :: Delete ( Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  DBMCli_Database & oDB = GetDatabase();

  DBMCli_String sCmd( DBMCLI_CMD_MEDIUM_DELETE );

  sCmd += " "; 
  sCmd += "\"" + FullName( ) + "\"";

  if( oDB.Execute( sCmd, oMsgList ) ) {
    bRC = true;
  }

  return bRC;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Medium :: Refresh
  -----------------------------------------------------------------------------
*/
void DBMCli_Medium :: SetPropertiesByLine (DBMCli_ResultBuf & oLine )
{
  DBMCli_String sFullName;
  DBMCli_String sCreated;
  DBMCli_String sModified;

  DBMCli_DateTime oDT;

  Clear( );

  if( oLine.GetField( sFullName, FIELD_SEP ) ) {
  
    if( !sFullName.IsEmpty( ) ) {
      SAPDB_Int nPos = -1;
      nPos = sFullName.Find( KEY_BACKSLASH );
      if( nPos < 0 ) {
        nPos = sFullName.Find( KEY_SLASH );
      }
      if( nPos >= 0 ) {
        m_sGroup = sFullName.Left( nPos );
        if( nPos + 1 < sFullName.GetLength( ) ) {
          m_sName = sFullName.Mid( nPos + 1 );
        } else {
          m_sName.Empty( );
        }
      } else {
        m_sGroup.Empty( );
        m_sName = sFullName;
      }
    }

    if( oLine.GetField( m_sLocation,      FIELD_SEP ) ) {
      if( oLine.GetField( m_sDeviceType,    FIELD_SEP ) ) {
        if( oLine.GetField( m_sBackupType,    FIELD_SEP ) ) {
          if( oLine.GetField( m_sPages,         FIELD_SEP ) ) {
            if( oLine.GetField( m_sBlockSize,     FIELD_SEP ) ) {
              if( oLine.GetField( m_sOverwrite,     FIELD_SEP ) ) {
                if( oLine.GetField( m_sAutoloader,    FIELD_SEP ) ) {
                  if( oLine.GetField( m_sOSCommand,     FIELD_SEP ) ) {
                    if( oLine.GetField( sCreated,         FIELD_SEP ) ) { 
                      oDT.Set( sCreated, DBMCLI_DT_NUM_DATETIME_FMT ); 
                      m_sCreated = oDT.Get( DBMCLI_DT_INT_DATETIME_FMT );
                      if( oLine.GetField( sModified,        FIELD_SEP ) ) {
                        oDT.Set( sModified, DBMCLI_DT_NUM_DATETIME_FMT ); 
                        m_sModified = oDT.Get( DBMCLI_DT_INT_DATETIME_FMT );
                        if (oLine.GetField( m_sOldDefinition, FIELD_SEP ) ) {
                          oLine.GetField( m_sExtern, FIELD_SEP);
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }

  }
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Medium :: SetProperties
  -----------------------------------------------------------------------------
*/
void DBMCli_Medium :: SetProperties ( const DBMCli_String & sGroup,
                                      const DBMCli_String & sName,
                                      const DBMCli_String & sLocation,
                                      const DBMCli_String & sDeviceType,
                                      const DBMCli_String & sBackupType,
                                      const DBMCli_String & sSize,
                                      const DBMCli_String & sBlockSize,
                                      const DBMCli_String & sOverwrite,
                                      const DBMCli_String & sAutoloader,
                                      const DBMCli_String & sOSCommand,
                                      const DBMCli_String & sExtern )
{
  m_sGroup = sGroup;
  m_sName = sName;
  m_sLocation = sLocation;
  m_sDeviceType = sDeviceType;
  m_sBackupType = sBackupType;
  m_sPages = sSize;
  m_sBlockSize = sBlockSize;
  m_sOverwrite = sOverwrite;
  m_sAutoloader = sAutoloader;
  m_sOSCommand = sOSCommand;
  m_sExtern = sExtern;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Medium :: FullName
  -----------------------------------------------------------------------------
*/
DBMCli_String DBMCli_Medium :: FullName ( )
{
  DBMCli_String sFullName;

  if( !m_sGroup.IsEmpty( ) ) {
    sFullName = m_sGroup + KEY_BACKSLASH;
  }
  sFullName += m_sName;

  return sFullName;
}

/*
  -----------------------------------------------------------------------------
  private functions ...
  -----------------------------------------------------------------------------
*/

/*    
  =============================================================================
  class: DBMCli_MediumParallel
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMCli_MediumParallel :: DBMCli_MediumParallel
  -----------------------------------------------------------------------------
*/
DBMCli_MediumParallel :: DBMCli_MediumParallel ( )
{
}

DBMCli_MediumParallel :: DBMCli_MediumParallel ( const DBMCli_String & sName,
                                                 const DBMCli_String & sDeviceType,
                                                 const DBMCli_String & sBackupType,
                                                 const DBMCli_String & sBlockSize,
                                                 const DBMCli_String & sOverwrite,
                                                 const DBMCli_String & sAutoloader,
                                                 const DBMCli_String & sExtern,
                                                 const DBMCli_String & sCreated,
                                                 const DBMCli_String & sModified,
                                                 const DBMCli_String & sOldDefinition)
                              : m_sName( sName ),
                                m_sDeviceType( sDeviceType ),
                                m_sBackupType( sBackupType ),
                                m_sBlockSize( sBlockSize ),
                                m_sOverwrite( sOverwrite ),
                                m_sAutoloader( sAutoloader ),
                                m_sExtern( sExtern ),
                                m_sCreated( sCreated ),
                                m_sModified( sModified ),
                                m_sOldDefinition( sOldDefinition )
{
}

/*
  -----------------------------------------------------------------------------
  public destructor DBMCli_MediumParallel :: ~DBMCli_MediumParallel
  -----------------------------------------------------------------------------
*/
DBMCli_MediumParallel :: ~DBMCli_MediumParallel ( )
{
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_MediumParallel :: Clear
  -----------------------------------------------------------------------------
*/
void DBMCli_MediumParallel :: Clear ( )
{
  m_sName.Empty( );
  m_sDeviceType.Empty( );
  m_sBackupType.Empty( );
  m_sBlockSize.Empty( );
  m_sOverwrite.Empty( );
  m_sAutoloader.Empty( );
  m_sExtern.Empty( );
  m_sCreated.Empty( );
  m_sModified.Empty( );
  m_sOldDefinition.Empty( );
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_MediumParallel :: SetProperties
  -----------------------------------------------------------------------------
*/
void DBMCli_MediumParallel :: SetProperties ( const DBMCli_String & sName,
                                              const DBMCli_String & sDeviceType,
                                              const DBMCli_String & sBackupType,
                                              const DBMCli_String & sBlockSize,
                                              const DBMCli_String & sOverwrite,
                                              const DBMCli_String & sAutoloader,
                                              const DBMCli_String & sExtern)
{
  m_sName = sName;
  m_sDeviceType = sDeviceType;
  m_sBackupType = sBackupType;
  m_sBlockSize = sBlockSize;
  m_sOverwrite = sOverwrite;
  m_sAutoloader = sAutoloader;
  m_sExtern = sExtern;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_MediumParallel :: MediaCount
  -----------------------------------------------------------------------------
*/
SAPDB_Int DBMCli_MediumParallel :: MediaCount ( )
{

  int nSize  = MediumArray().GetSize();
  int nCount = 0;

  for (int i = 0; i < nSize; ++i) {
    if (!MediumArray()[i].Name().IsEmpty()) {
      ++nCount;
    } // end if
  } // end for

  return nCount;
} // end DBMCli_MediumParallel :: MediaCount

/*
  -----------------------------------------------------------------------------
  public function DBMCli_MediumParallel :: IndexByLocation
  -----------------------------------------------------------------------------
*/
SAPDB_Int DBMCli_MediumParallel :: IndexByLocation ( const DBMCli_String & sLocation )
{

  int nSize  = MediumArray().GetSize();
  int nIndex = -1;

  for (int i = 0; i < nSize; ++i) {
    if (MediumArray()[i].Location() == sLocation) {
      nIndex = i;;
    } // end if
  } // end for

  return nIndex;
} // end DBMCli_MediumParallel :: IndexByLocation

/*
  -----------------------------------------------------------------------------
  private functions ...
  -----------------------------------------------------------------------------
*/

/*    
  =============================================================================
  class: DBMCli_Media
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMCli_Media :: DBMCli_Media
  -----------------------------------------------------------------------------
*/
DBMCli_Media :: DBMCli_Media ( )
{
  Clear( );
}

/*
  -----------------------------------------------------------------------------
  public destructor DBMCli_Media :: ~DBMCli_Media
  -----------------------------------------------------------------------------
*/
DBMCli_Media :: ~DBMCli_Media ( )
{
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Media :: Clear
  -----------------------------------------------------------------------------
*/
void DBMCli_Media :: Clear ( )
{
  m_nMaxParallelDevices = 0;
  m_aMediumParallel.RemoveAll( );
  m_aMedium.RemoveAll( );
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Media :: Refresh
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Media :: Refresh ( Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  Clear( );

  DBMCli_Database & oDB = GetDatabase();
  DBMCli_Result & oResult = oDB.GetResult( ); 

  DBMCli_String sCmd;

  // get param MAXBACKUPDEVS

  sCmd = DBMCLI_CMD_PARAM_DIRECTGET;
  sCmd += " ";
  sCmd += PARAM_MAXBACKUPDEVS;

  if( oDB.Execute( sCmd, oMsgList ) ) {
    DBMCli_ResultBuf oLine;
    DBMCli_String sMaxDevs("0");

    if( oResult.GetLine( oLine ) ) {
      oLine.SkipField( FIELD_SEP );
      oLine.GetField( sMaxDevs, FIELD_SEP );
    }

    m_nMaxParallelDevices = atoi( sMaxDevs );
  }

  oMsgList.ClearMessageList();

  // get all media

  sCmd = DBMCLI_CMD_MEDIUM_GETALL;

  if( oDB.Execute( sCmd, oMsgList ) ) {
    DBMCli_ResultBuf oLine;

    DBMCli_DateTime oDT;

    while( oResult.GetLine( oLine ) ) {

      DBMCli_Medium oMedium;

      oMedium.SetDatabase( m_pDatabase );
      oMedium.SetPropertiesByLine( oLine ); 

      // add medium to array
      if( !oMedium.Group( ).IsEmpty( ) ) {

        // parallel medium
        SAPDB_Int nMedium;
        nMedium = IndexByNameParallel( oMedium.Group( ) );
        if( nMedium < 0 ) {
          DBMCli_MediumParallel oParallel( oMedium.Group( ), oMedium.DeviceType( ), oMedium.BackupType( ),
                                           oMedium.BlockSize( ), oMedium.Overwrite( ), oMedium.Autoloader( ),
                                           oMedium.Extern(), oMedium.Created( ), oMedium.Modified( ), 
                                           oMedium.OldDefinition( ) );
          oParallel.SetDatabase( m_pDatabase );
          m_aMediumParallel.Add( oParallel );
        }

        nMedium = IndexByNameParallel( oMedium.Group( ) );
        if( nMedium >= 0 ) {
          DBMCli_MediumParallel & oParallel = m_aMediumParallel[nMedium];

          oParallel.MediumArray( ).Add( oMedium );
        }

      } else {

        // single medium
        m_aMedium.Add( oMedium );

      }

      oLine.Clear( );
    }
    
    // add empty medium objects to all parallel medium
    for( SAPDB_Int nParallel = 0; nParallel < m_aMediumParallel.GetSize( ); nParallel++ ) {
      DBMCli_MediumParallel & oMediumParallel = m_aMediumParallel[nParallel];
      if( oMediumParallel.MediumArray( ).GetSize( ) < m_nMaxParallelDevices ) {
        DBMCli_MediumArray & aMedium = oMediumParallel.MediumArray( );
        for( SAPDB_Int nMedium = aMedium.GetSize( ); nMedium < m_nMaxParallelDevices; nMedium++ ) {
          DBMCli_Medium oMedium;

          oMedium.SetDatabase( m_pDatabase );

          oMediumParallel.MediumArray( ).Add( oMedium );

        }
      }
    }

    bRC = true;

  } else {

    if( !oMsgList.IsEmpty( ) ) {
      if( oMsgList.ID() == (SAPDB_UInt4)DBMCLI_SRV_RC_NOMEDIA ) {
        oMsgList.ClearMessageList();
        bRC = true;
      }
    }

  }

  return bRC;
} 

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Media:: NewMedium
  -----------------------------------------------------------------------------
*/
DBMCli_Medium & DBMCli_Media :: NewMedium( const DBMCli_String & sName )
{
  m_oNewMedium.SetDatabase( m_pDatabase );

  m_oNewMedium.SetProperties( "", sName );

  return m_oNewMedium;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Media :: IndexByName
  -----------------------------------------------------------------------------
*/
SAPDB_Int DBMCli_Media :: IndexByName ( const DBMCli_String & sName )
{
  SAPDB_Bool bFound = false;
  SAPDB_Int nMedium = 0;

  while( nMedium < m_aMedium.GetSize( ) && !bFound ) {
    if( m_aMedium[nMedium].Name( ) == sName ) {
      bFound = true;
    } else {
      nMedium++;
    }
  }

  if( bFound ) {
    return nMedium;
  } else {
    return -1;
  }
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Media :: NewMediumParallel
  -----------------------------------------------------------------------------
*/
DBMCli_MediumParallel & DBMCli_Media :: NewMediumParallel( const DBMCli_String & sName )
{
  m_oNewMediumParallel.SetDatabase( m_pDatabase );
  m_oNewMediumParallel.SetProperties( sName );

  m_oNewMediumParallel.MediumArray( ).RemoveAll( );

  for(SAPDB_Int nMedium = 0; nMedium < m_nMaxParallelDevices; nMedium++ )
  {
    DBMCli_Medium oMedium;

    oMedium.SetDatabase( m_pDatabase );

    m_oNewMediumParallel.MediumArray( ).Add( oMedium );
  }

  return m_oNewMediumParallel;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Media :: IndexByNameParallel
  -----------------------------------------------------------------------------
*/
SAPDB_Int DBMCli_Media :: IndexByNameParallel ( const DBMCli_String & sName )
{
  SAPDB_Bool bFound = false;
  SAPDB_Int nMedium = 0;

  while( nMedium < m_aMediumParallel.GetSize( ) && !bFound ) {
    if( m_aMediumParallel[nMedium].Name( ) == sName ) {
      bFound = true;
    } else {
      nMedium++;
    }
  }

  if( bFound ) {
    return nMedium;
  } else {
    return -1;
  }
}

/*
  -----------------------------------------------------------------------------
  private functions ...
  -----------------------------------------------------------------------------
*/

