/*!
  -----------------------------------------------------------------------------
  module: DBMWeb_TemplateDevspaces.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager WebAgent Extension
  description:  devspaces
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
#include "DBM/Web/DBMWeb_TemplateDevspaces.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define ISOBJ(obj)  (szName.Compare(obj) == 0)

#define TEMPLATE_NAME          "DBMDevspaces.htm"

#define OBJ_TITLE              "GroupBoxTitle"

#define OBJ_REFRESHBUTTON      "RefreshButton"

#define OBJ_DATAINFO           "DataInfo"
#define OBJ_MAXDATA            "MaxData"
#define OBJ_USEDDATA           "UsedData"
#define OBJ_MAXDATADEV         "MaxDataDevspaces"
#define OBJ_MAXDATAVOL         "MaxDataVolumes"

#define OBJ_LOGINFO            "LogInfo"
#define OBJ_LOGMODE            "LogMode"
#define OBJ_MAXLOG             "MaxLog"
#define OBJ_USEDLOG            "UsedLog"
#define OBJ_MAXARCLOG          "MaxArchiveLogs"
#define OBJ_MAXLOGVOL          "MaxLogVolumes"

#define OBJ_WITHSYSTEM         "WithSystem"
#define OBJ_WITHOUTSYSTEM      "WithoutSystem"

#define OBJ_TABLOG             "TabLog"
#define OBJ_TABDAT             "TabData"
#define OBJ_TABSYS             "TabSystem"

#define OBJ_NORMAL             "Normal"
#define OBJ_MIRRORED           "Mirrored"

#define OBJ_TABLEHEADER        "TableHeader"

#define OBJ_ROWPAIR            "RowPair"

#define OBJ_EVENROW            "EvenRow*"
#define OBJ_ODDROW             "OddRow*"

#define OBJ_NAME               "Name"
#define OBJ_SIZE               "Size"
#define OBJ_TYPE               "Type"
#define OBJ_LOC                "Location"
#define OBJ_MTYPE              "MType"
#define OBJ_MLOC               "MLocation"

#define OBJ_TOOLBAR            "Toolbar"
#define OBJ_BTNADDDATA         "ButtonAddData"
#define OBJ_BTNADDDATADIS      "ButtonAddDataDisabled"
#define OBJ_BTNADDLOG          "ButtonAddLog"
#define OBJ_BTNADDLOGDIS       "ButtonAddLogDisabled"


/*
  =============================================================================
  class:        DBMWeb_TemplateDevspaces
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMWeb_TemplateDevspaces:: DBMWeb_TemplateDevspaces
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateDevspaces :: DBMWeb_TemplateDevspaces ( sapdbwa_WebAgent & wa,
                                                       DBMCli_String & sClass, 
                                                       DBMCli_Devspaces & oDevspaces,
                                                       DBMCli_DevspaceClass nClass)
                        : Tools_Template( wa, _Tools_UTF8Ptr( TEMPLATE_NAME )),
                            m_sClass( sClass ),
                            m_oDevspaces( oDevspaces ),
                            m_nClass( nClass )
{
  m_nDevspace = 0;

  if( ( m_nClass == DBMCLI_DEVSPACECLASS_LOG ) &&
      ( m_oDevspaces.LogMode( ) == DBMCLI_LOGMODE_DUAL ) ) {
    m_bMirrored = SAPDB_TRUE;
  } else {
    m_bMirrored = SAPDB_FALSE;
  }
} 

/*
  -----------------------------------------------------------------------------
  public destructor DBMWeb_TemplateDevspaces :: ~DBMWeb_TemplateDevspaces
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateDevspaces :: ~DBMWeb_TemplateDevspaces (  )
{
} 

/*
  -----------------------------------------------------------------------------
  private functions ...
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateDevspaces :: askForWriteCount
  -----------------------------------------------------------------------------
*/
SAPDB_Int2 DBMWeb_TemplateDevspaces :: askForWriteCount  ( const Tools_DynamicUTF8String & szName )
{
  SAPDB_Int2 nReturn = 0;

//ISOBJ( OBJ_MODEUSE )
  if( ISOBJ( OBJ_TITLE ) ) {
    nReturn = 1;
  } else if( ISOBJ( OBJ_REFRESHBUTTON ) ) {
    nReturn = 1;
  } else if ( ISOBJ( OBJ_WITHSYSTEM ) ) {
    nReturn = FindSystemDevspace() ? 1 : 0;
  } else if ( ISOBJ( OBJ_WITHOUTSYSTEM ) ) {
    nReturn = FindSystemDevspace() ? 0 : 1;
  } else if( ISOBJ( OBJ_TABLOG ) ) {
    nReturn = (m_nClass == DBMCLI_DEVSPACECLASS_LOG) ? 1 : 0;
  } else if( ISOBJ( OBJ_TABDAT ) ) {
    nReturn = (m_nClass == DBMCLI_DEVSPACECLASS_DATA) ? 1 : 0;
  } else if( ISOBJ( OBJ_TABSYS ) ) {
    nReturn = (m_nClass == DBMCLI_DEVSPACECLASS_SYS) ? 1 : 0;
  } else if( ISOBJ( OBJ_DATAINFO ) ) {
    nReturn = (m_nClass == DBMCLI_DEVSPACECLASS_DATA) ? 1 : 0;
  } else if( ( ISOBJ( OBJ_MAXDATA ) )  ||
             ( ISOBJ( OBJ_USEDDATA ) )    ){
    nReturn = 1;
  } else if( ISOBJ( OBJ_LOGINFO ) ) {
    nReturn = (m_nClass == DBMCLI_DEVSPACECLASS_LOG) ? 1 : 0;
  } else if (( ISOBJ( OBJ_LOGMODE ) ) ||
             ( ISOBJ( OBJ_MAXLOG ) )  || 
             ( ISOBJ( OBJ_USEDLOG ) )    ) {
    nReturn = 1;
  } else if( ISOBJ( OBJ_NORMAL ) ) {
    nReturn = ( m_bMirrored ) ? 0 : 1;
  } else if( ISOBJ( OBJ_MIRRORED ) ) {
    nReturn = ( m_bMirrored ) ? 1 : 0;
  } else if( ISOBJ( OBJ_MAXDATADEV ) ) {
    return ( m_oDevspaces.GetDatabase().NodeInfo().Version().GetNumber() < 0x7040400 ) ? 1 : 0;
  } else if( ISOBJ( OBJ_MAXDATAVOL ) ) {
    return ( m_oDevspaces.GetDatabase().NodeInfo().Version().GetNumber() >= 0x7040400 ) ? 1 : 0;
  } else if( ISOBJ( OBJ_MAXARCLOG ) ) {
    return ( m_oDevspaces.GetDatabase().NodeInfo().Version().GetNumber() < 0x7040400 ) ? 1 : 0;
  } else if( ISOBJ( OBJ_MAXLOGVOL ) ) {
    return ( m_oDevspaces.GetDatabase().NodeInfo().Version().GetNumber() >= 0x7040400 ) ? 1 : 0;
  } else if( ISOBJ( OBJ_TABLEHEADER ) ) {
    nReturn = 1;
  } else if( ISOBJ( OBJ_ROWPAIR ) ) {
    nReturn = (m_nDevspace < m_oDevspaces.DevspaceArray( ).GetSize( )) ? -1 : 0; 
  } else if( ( ISOBJ( OBJ_EVENROW ) ) || 
             ( ISOBJ( OBJ_ODDROW ) )     ) {
    nReturn = (FindNextDevspace( ) == true) ? 1 : 0;
  } else if( ISOBJ( OBJ_TOOLBAR ) ) {
    nReturn = ( m_nClass == DBMCLI_DEVSPACECLASS_DATA ||
                m_nClass == DBMCLI_DEVSPACECLASS_LOG     ) ? 1 : 0;
  } else if( ISOBJ( OBJ_BTNADDDATA ) ) {
    DBMCli_State & oState = m_oDevspaces.GetDatabase( ).GetState( );

    nReturn = ( ( m_nClass == DBMCLI_DEVSPACECLASS_DATA ) && 
                ( oState.Value( ) == DBMCLI_DBSTATE_COLD || 
                  oState.Value( ) == DBMCLI_DBSTATE_WARM ||
                  oState.Value( ) == DBMCLI_DBSTATE_UNKNOWN ) &&
                ( m_oDevspaces.MaxDataDevs( ) > m_oDevspaces.UsedDataDevs( ) ) 
              ) ? 1 : 0;
  } else if( ISOBJ( OBJ_BTNADDDATADIS ) ) {
    DBMCli_State & oState = m_oDevspaces.GetDatabase( ).GetState( );

    nReturn = ( ( m_nClass == DBMCLI_DEVSPACECLASS_DATA ) && 
                (
                  ( oState.Value( ) == DBMCLI_DBSTATE_OFFLINE ) ||
                  ( m_oDevspaces.MaxDataDevs( ) <= m_oDevspaces.UsedDataDevs( ) )
                ) 
              ) ? 1 : 0;
  } else if( ISOBJ( OBJ_BTNADDLOG ) ) {
    DBMCli_State & oState = m_oDevspaces.GetDatabase( ).GetState( );

    nReturn = ( ( m_nClass == DBMCLI_DEVSPACECLASS_LOG ) &&
                ( oState.Value( ) == DBMCLI_DBSTATE_COLD ||
                  oState.Value( ) == DBMCLI_DBSTATE_WARM ||
                  oState.Value( ) == DBMCLI_DBSTATE_UNKNOWN ) &&
                ( m_oDevspaces.MaxLogDevs( ) > m_oDevspaces.UsedLogDevs( ) )
              ) ? 1 : 0;
  } else if( ISOBJ( OBJ_BTNADDLOGDIS ) ) {
    DBMCli_State & oState = m_oDevspaces.GetDatabase( ).GetState( );

    nReturn = ( ( m_nClass == DBMCLI_DEVSPACECLASS_LOG ) &&
                (
                  ( oState.Value( ) == DBMCLI_DBSTATE_OFFLINE ) ||
                  ( m_oDevspaces.MaxLogDevs( ) <= m_oDevspaces.UsedLogDevs( ) )
                )
              ) ? 1 : 0;
  }
  return nReturn;
} 

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateDevspaces :: askForValue
  -----------------------------------------------------------------------------
*/
Tools_DynamicUTF8String DBMWeb_TemplateDevspaces :: askForValue ( const Tools_DynamicUTF8String & szName )
{
  const SAPDB_Char * pReturn = NULL;

  DBMCli_String sValue;

  sValue.Empty( );
  
  if( ISOBJ( OBJ_TITLE ) ) {
    sValue = m_oDevspaces.GetDatabase().DatabaseName();  
  } else if( ISOBJ( OBJ_REFRESHBUTTON ) ) {
    sValue = m_sClass;
  } else if( ISOBJ( OBJ_TABLEHEADER ) ) {
    switch( m_nClass ) {
      case DBMCLI_DEVSPACECLASS_LOG: 
        sValue = "Log ";
        break;
      case DBMCLI_DEVSPACECLASS_DATA: 
        sValue = "Data ";
        break;
      case DBMCLI_DEVSPACECLASS_SYS: 
        sValue = "System ";
        break;
      default:
        sValue = "???";
        break;
    }
  } else if( ISOBJ( OBJ_MAXDATA ) ) {
    sValue.Format( "%d", m_oDevspaces.MaxDataDevs( ) );
  } else if( ISOBJ( OBJ_USEDDATA ) ) {
    sValue.Format( "%d", m_oDevspaces.UsedDataDevs( ) );
  } else if( ISOBJ( OBJ_LOGMODE ) ) {
    sValue = m_oDevspaces.LogModeName();
  } else if( ISOBJ( OBJ_MAXLOG ) ) {
    sValue.Format("%d", m_oDevspaces.MaxLogDevs());
  } else if( ISOBJ( OBJ_USEDLOG ) ) {
    sValue.Format( "%d", m_oDevspaces.UsedLogDevs( ) );
  } else if( ISOBJ( OBJ_NAME ) ) {
    DBMCli_DevspaceArray & aDevspaces = m_oDevspaces.DevspaceArray( );
    sValue = aDevspaces[m_nDevspace].Name( );
  } else if( ISOBJ( OBJ_SIZE ) ) {
    DBMCli_DevspaceArray & aDevspaces = m_oDevspaces.DevspaceArray( );
    sValue.Format( "%d", aDevspaces[m_nDevspace].Pages( ) );
  } else if( ISOBJ( OBJ_TYPE ) ) {
    DBMCli_DevspaceArray & aDevspaces = m_oDevspaces.DevspaceArray( );
    sValue = aDevspaces[m_nDevspace].TypeName( );
  } else if( ISOBJ( OBJ_LOC ) ) {
    DBMCli_DevspaceArray & aDevspaces = m_oDevspaces.DevspaceArray( );
    sValue = aDevspaces[m_nDevspace].Location( );
    if( !m_bMirrored ) {
      m_nDevspace++;
    }
  } else if( ISOBJ( OBJ_MLOC ) ) {
    DBMCli_DevspaceArray & aDevspaces = m_oDevspaces.DevspaceArray( );
    sValue = aDevspaces[m_nDevspace].MirroredLocation( );
  } else if( ISOBJ( OBJ_MTYPE ) ) {
    DBMCli_DevspaceArray & aDevspaces = m_oDevspaces.DevspaceArray( );
    sValue = aDevspaces[m_nDevspace].MirroredTypeName( );
    m_nDevspace++;
  }

  pReturn = sValue;

  return _Tools_UTF8Ptr(pReturn);
} 

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateFile :: askForContinue
  -----------------------------------------------------------------------------
*/
bool DBMWeb_TemplateDevspaces :: askForContinue ( const Tools_DynamicUTF8String & szName )
{
  bool bRC = false;
  
  DBMCli_DevspaceArray & aDevspaces = m_oDevspaces.DevspaceArray( );

  if( ISOBJ( OBJ_ROWPAIR ) ) {
    bRC = ( m_nDevspace < aDevspaces.GetSize( ) );
  }

  return bRC;
}

bool DBMWeb_TemplateDevspaces :: FindNextDevspace ( )
{
  bool bFound = false;

  DBMCli_DevspaceArray & aDevspaces = m_oDevspaces.DevspaceArray( );

  while( ( m_nDevspace < aDevspaces.GetSize( ) ) && ( bFound == false ) ) {
    if( aDevspaces[m_nDevspace].Class( ) == m_nClass ) {
      bFound = true;
    } else {
      m_nDevspace++;
    }
  }

  return bFound;
}

bool DBMWeb_TemplateDevspaces :: FindSystemDevspace ( )
{
  bool        bFound = false;
  DBMCli_DevspaceArray & aDevspaces = m_oDevspaces.DevspaceArray( );

  for (SAPDB_Int2  nDevspace = 0; nDevspace < aDevspaces.GetSize( ); ++nDevspace) {
    if( aDevspaces[nDevspace].Class( ) == DBMCLI_DEVSPACECLASS_SYS ) {
      bFound = true;
      break;
    } // end if
  }  // end if

  return bFound;
} // end DBMWeb_TemplateDevspaces :: FindSystemDevspace