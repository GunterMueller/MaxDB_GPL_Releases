/*!
  -----------------------------------------------------------------------------
  module: DBMWeb_TemplateWizard.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager WebAgent Extension
  description:  Wizard
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
#include "DBM/Web/DBMWeb_TemplateWizard.hpp"
#include "DBM/Cli/DBMCli_Parameter.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define ISOBJ(obj)  (szName.Compare(obj) == 0)

#define TEMPLATE_NAME   "DBMWizard.htm"

#define OBJ_STEP        "Step"
#define OBJ_STATE       "State"
#define OBJ_GROUP       "Group"
#define OBJ_CLASS       "Class"
#define OBJ_LOSE        "Lose"

#define OBJ_FORM        "GlobalForm"

#define OBJ_MSG         "Message"

#define OBJ_STEP1       "Step1"
#define OBJ_STEP2       "Step2"
#define OBJ_STEP3       "Step3"
#define OBJ_STEP4       "Step4"
#define OBJ_STEP5       "Step5"
#define OBJ_STEP6       "Step6"
#define OBJ_STEP7       "Step7"
#define OBJ_STEP8       "Step8"
#define OBJ_STEP9       "Step9"

#define OBJ_FORM1       "Form1*"
#define OBJ_FORM2       "Form2*"
#define OBJ_FORM3       "Form3*"
#define OBJ_FORM4       "Form4*"
#define OBJ_FORM5       "Form5*"

#define OBJ_FORM7       "Form7*"
#define OBJ_FORM8       "Form8*"

#define OBJ_TABLEHEADER "TableHeader"

#define OBJ_ROWPAIR     "RowPair"
#define OBJ_EVENROW     "EvenRow*"
#define OBJ_ODDROW      "OddRow*"

#define OBJ_CHECKED     "checked"

#define OBJ_DB          "Database"
#define OBJ_SRV         "Server"
#define OBJ_USR         "User"
#define OBJ_PWD         "Password"
#define OBJ_REINSTALL   "Reinstall"
#define OBJ_NOREINST    "NoReinstall"

#define OBJ_VER         "Version"
#define OBJ_INST        "InstRoot"
#define OBJ_DESC        "Description"

#define OBJ_DBM         "DBMName"
#define OBJ_DBMPWD      "DBMPwd"

#define OBJ_DBA         "DBAName"
#define OBJ_DBAPWD      "DBAPwd"

#define OBJ_INITMODE    "InitMode"
#define OBJ_INITMODE1   "InitMode1"
#define OBJ_INITMODE2   "InitMode2"
#define OBJ_INITMODE3   "InitMode3"
#define OBJ_INITMODE4   "InitMode4"

#define OBJ_IMCUR       "IMCurrent"
#define OBJ_IMCURDIS    "IMCurrentDis"

#define OBJ_INSTTYPE    "InstanceType"


#define OBJ_TABGEN      "TabGeneral"
#define OBJ_TABEXT      "TabExtended"
#define OBJ_TABSUP      "TabSupport"

#define OBJ_NAME        "Name"
#define OBJ_CHANGE      "Change"
#define OBJ_VALUE       "Value"
#define OBJ_NEWVALUE    "NewValue"
#define OBJ_DESC        "Description"

#define OBJ_WITHSYSTEM         "WithSystem"
#define OBJ_WITHOUTSYSTEM      "WithoutSystem"

#define OBJ_TABLOG      "TabLog"
#define OBJ_TABDAT      "TabData"
#define OBJ_TABSYS      "TabSystem"

// #define OBJ_NAME        "Name"
#define OBJ_SIZE        "Size"
#define OBJ_TYPE        "Type"
#define OBJ_LOC         "Location"
#define OBJ_MLOC        "MLocation"
#define OBJ_MTYPE       "MType"

#define OBJ_INSTMODE    "InstallMode"

#define OBJ_PARAMINFO   "ParamInfo"
#define OBJ_PARAMLOSE   "ParamLose"

#define OBJ_TABSNG      "TabSingle"
#define OBJ_TABPAR      "TabParallel"

#define OBJ_TABLESNG    "TableSingle"
#define OBJ_TABLEPAR    "TableParallel"

#define OBJ_NORMAL      "Normal"
#define OBJ_MIRRORED    "Mirrored"

// #define OBJ_NAME        "Name"
// #define OBJ_LOC         "Location"
#define OBJ_DEVTYPE     "DeviceType"
#define OBJ_BAKTYPE     "BackupType"
#define OBJ_MOD         "Modified"
// #define OBJ_SIZE        "Size"
#define OBJ_OVW         "Overwrite"

#define OBJ_BTNADDSNG   "ButtonAddSingle"
#define OBJ_BTNADDPAR   "ButtonAddParallel"


/*
  =============================================================================
  class:        DBMWeb_TemplateWizard
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMWeb_TemplateWizard :: DBMWeb_TemplateWizard
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateWizard :: DBMWeb_TemplateWizard ( sapdbwa_WebAgent & wa,
                                                 DBMCli_Wizard * pWizard,
                                                 DBMCli_Database * pDatabase,
                                                 SAPDB_Int nStep,
                                                 SAPDB_Int nState,
                                                 const DBMCli_String & sMessage)
                       : Tools_Template( wa, _Tools_UTF8Ptr(TEMPLATE_NAME) ), 
                         m_Wizard( pWizard ),
                         m_Database( pDatabase ),
                         m_nStep( nStep ),
                         m_nState( nState ),
                         m_sMessage( sMessage )

{
  m_nItem = 0;
  m_nMediumType = DBMCLI_MEDIUMTYPE_SINGLE;
  m_nParamGroup = DBMCLI_PARAMGROUP_GENERAL;
  m_nDevspaceClass = DBMCLI_DEVSPACECLASS_DATA;
  m_bDevspaceMirrored = SAPDB_FALSE;
  m_bLose = SAPDB_FALSE;
  m_bReinstall = SAPDB_FALSE;
} 

/*
  -----------------------------------------------------------------------------
  public destructor DBMWeb_TemplateWizard :: ~DBMWeb_TemplateWizard
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateWizard :: ~DBMWeb_TemplateWizard (  )
{
} 

/*
  -----------------------------------------------------------------------------
  private functions ...
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateWizard :: askForWriteCount
  -----------------------------------------------------------------------------
*/
SAPDB_Int2 DBMWeb_TemplateWizard :: askForWriteCount ( const Tools_DynamicUTF8String & szName )
{
  SAPDB_Int2 nReturn = 0;

  if( ISOBJ( OBJ_STEP ) ) {
    nReturn = 1;
  } else if( ISOBJ( OBJ_STATE ) ) {
    nReturn = 1;
  } else if( ISOBJ( OBJ_MSG ) ) {
    nReturn = m_sMessage.IsEmpty( ) ? 0 : 1; 
  } else if( ISOBJ( OBJ_LOSE ) ) {
    nReturn = 1;
  } else {
    switch( m_nStep ) {
      case 1:
        nReturn = askForWriteCountStep1( szName );
        break;
      case 2:
        nReturn = askForWriteCountStep2( szName );
        break;
      case 3:
        nReturn = askForWriteCountStep3( szName );
        break;
      case 4:
        nReturn = askForWriteCountStep4( szName );
        break;
      case 5:
        nReturn = askForWriteCountStep5( szName );
        break;
      case 6:
        nReturn = askForWriteCountStep6( szName );
        break;
      case 7:
        nReturn = askForWriteCountStep7( szName );
        break;
      case 8:
        nReturn = askForWriteCountStep8( szName );
        break;
      case 9:
        nReturn = askForWriteCountStep9( szName );
        break;
    }
  }

  return nReturn;
} 

// Databasename

SAPDB_Int2 DBMWeb_TemplateWizard :: askForWriteCountStep1 ( const Tools_DynamicUTF8String & szName )
{
  SAPDB_Int2 nReturn = 0;

  if( ISOBJ( OBJ_STEP1 ) ) {
    nReturn = 1;
  } else if( ISOBJ( OBJ_FORM ) ) {
    nReturn = 1;
  } else if( ISOBJ( OBJ_FORM1 ) ) {
    nReturn = 1;
  } else if( ISOBJ( OBJ_NOREINST ) ) {
    nReturn = (m_bReinstall) ? 0 : 1;
  } else if( ISOBJ( OBJ_REINSTALL ) ) {
    nReturn = (m_bReinstall) ? 1 : 0;
  }

  return nReturn;
}

// Installations

SAPDB_Int2 DBMWeb_TemplateWizard :: askForWriteCountStep2 ( const Tools_DynamicUTF8String & szName )
{
  SAPDB_Int2 nReturn = 0;

  if( ISOBJ( OBJ_STEP2 ) ) {
    nReturn = 1;
  } else if( ISOBJ( OBJ_FORM ) ) {
    nReturn = 1;
  } else if( ISOBJ( OBJ_FORM2 ) ) {
    nReturn = 1;
  } else if( ISOBJ( OBJ_ROWPAIR ) ) {
    if( m_Wizard != NULL ) {
      nReturn = (m_Wizard->GetNode( ).EnumInstallationArray( ).GetSize( )  / 2) + 
                (m_Wizard->GetNode( ).EnumInstallationArray( ).GetSize( ) % 2); 
    }
  } else if (( ISOBJ( OBJ_EVENROW ) ) || 
             ( ISOBJ( OBJ_ODDROW ) )     ) {
    if( m_Wizard != NULL ) {
      nReturn = (m_nItem < m_Wizard->GetNode( ).EnumInstallationArray( ).GetSize( )) ? 1 : 0;
    }
    m_nItem++;
  }

  return nReturn;
}

// Users

SAPDB_Int2 DBMWeb_TemplateWizard :: askForWriteCountStep3 ( const Tools_DynamicUTF8String & szName )
{
  SAPDB_Int2 nReturn = 0;

  if( ISOBJ( OBJ_STEP3 ) ) {
    nReturn = 1;
  } else if( ISOBJ( OBJ_FORM ) ) {
    nReturn = 1;
  } else if( ISOBJ( OBJ_FORM3 ) ) {
    nReturn = 1;
  }

  return nReturn;
}

// InitMode

SAPDB_Int2 DBMWeb_TemplateWizard :: askForWriteCountStep4 ( const Tools_DynamicUTF8String & szName )
{
  SAPDB_Int2 nReturn = 0;

  if( ISOBJ( OBJ_STEP4 ) ) {
    nReturn = 1;
  } else if( ISOBJ( OBJ_FORM ) ) {
    nReturn = 1;
  } else if( ISOBJ( OBJ_FORM4 ) ) {
    nReturn = 1;
  } else if( ISOBJ( OBJ_IMCUR ) ) {
    if( m_Wizard != NULL ) {
      nReturn = (m_Wizard->GetCreatedFlag( )) ? 0 : 1;
    }
  } else if( ISOBJ( OBJ_IMCURDIS ) ) {
    if( m_Wizard != NULL ) {
      nReturn = (m_Wizard->GetCreatedFlag( )) ? 1 : 0;
    }
  } else if( ISOBJ( OBJ_INITMODE1 ) ) {
    if( m_Wizard != NULL ) {
      nReturn = (m_Wizard->GetInitMode( ) == DBMCLI_WIZARD_INITMODE_DEF) ? 1 : 0; 
    }
  } else if( ISOBJ( OBJ_INITMODE2 ) ) {
    if( m_Wizard != NULL ) {
      nReturn = (m_Wizard->GetInitMode( ) == DBMCLI_WIZARD_INITMODE_CUR) ? 1 : 0; 
    }
  } else if( ISOBJ( OBJ_INITMODE3 ) ) {
    if( m_Wizard != NULL ) {
      nReturn = (m_Wizard->GetInitMode( ) == DBMCLI_WIZARD_INITMODE_CPY) ? 1 : 0; 
    }
  } else if( ISOBJ( OBJ_INITMODE4 ) ) {
    if( m_Wizard != NULL ) {
      nReturn = (m_Wizard->GetInitMode( ) == DBMCLI_WIZARD_INITMODE_RES) ? 1 : 0; 
    }
  } else {
    if( m_Wizard != NULL ) {
      if (m_Wizard->GetInitMode( ) == DBMCLI_WIZARD_INITMODE_CPY) {
        if( ISOBJ( OBJ_ROWPAIR ) ) {
          nReturn = (m_Wizard->GetNode( ).EnumDatabaseArray( ).GetSize( )  / 2) + 
                    (m_Wizard->GetNode( ).EnumDatabaseArray( ).GetSize( ) % 2); 
        } else if (( ISOBJ( OBJ_EVENROW ) ) || 
                  ( ISOBJ( OBJ_ODDROW ) )     ) {
          nReturn = (m_nItem < m_Wizard->GetNode( ).EnumDatabaseArray( ).GetSize( )) ? 1 : 0;
          m_nItem++;
        }
      } else if (m_Wizard->GetInitMode( ) == DBMCLI_WIZARD_INITMODE_RES) {
        if( ISOBJ( OBJ_ROWPAIR ) ) {
          if( m_Database != NULL ) {
            nReturn = (m_Database->GetMedia( ).MediumArray( ).GetSize( ) / 2) + 
                      (m_Database->GetMedia( ).MediumArray( ).GetSize( ) % 2); 
          }
        } else if (( ISOBJ( OBJ_EVENROW ) ) || 
                  ( ISOBJ( OBJ_ODDROW ) )     ) {
          if( m_Database != NULL ) {
            nReturn = (m_nItem < m_Database->GetMedia( ).MediumArray( ).GetSize( )) ? 1 : 0;
          }
          m_nItem++;
        } else if( ISOBJ( OBJ_TABSNG ) ) {
          nReturn = (m_nMediumType == DBMCLI_MEDIUMTYPE_SINGLE) ? 1 : 0;
        } else if( ISOBJ( OBJ_TABPAR ) ) {
          nReturn = (m_nMediumType == DBMCLI_MEDIUMTYPE_PARALLEL) ? 1 : 0;
        } else if( ISOBJ( OBJ_TABLESNG ) ) {
          nReturn = (m_nMediumType == DBMCLI_MEDIUMTYPE_SINGLE) ? 1 : 0;
        } else if( ISOBJ( OBJ_TABLEPAR ) ) {
          nReturn = (m_nMediumType == DBMCLI_MEDIUMTYPE_PARALLEL) ? 1 : 0;
        } else if( ISOBJ( OBJ_BTNADDSNG ) ) {
          nReturn = (m_nMediumType == DBMCLI_MEDIUMTYPE_SINGLE) ? 1 : 0;
        } else if( ISOBJ( OBJ_BTNADDPAR ) ) {
          nReturn = (m_nMediumType == DBMCLI_MEDIUMTYPE_PARALLEL) ? 1 : 0;
        }
      }
    }
  }

  return nReturn;
}

// Parameters

SAPDB_Int2 DBMWeb_TemplateWizard :: askForWriteCountStep5 ( const Tools_DynamicUTF8String & szName )
{
  SAPDB_Int2 nReturn = 0;

  if( ISOBJ( OBJ_STEP5 ) ) {
    nReturn = 1;
  } else if( ISOBJ( OBJ_PARAMINFO ) ) {
    nReturn = (!m_bLose) ? 1 : 0;
  } else if( ISOBJ( OBJ_PARAMLOSE ) ) {
    nReturn = (m_bLose) ? 1 : 0;
  } else if( ISOBJ( OBJ_GROUP ) ) {
    nReturn = 1;
  } else if( ISOBJ( OBJ_TABGEN ) ) {
    nReturn = (m_nParamGroup == DBMCLI_PARAMGROUP_GENERAL) ? 1 : 0;
  } else if( ISOBJ( OBJ_TABEXT ) ) {
    nReturn = (m_nParamGroup == DBMCLI_PARAMGROUP_EXTENDED) ? 1 : 0;
  } else if( ISOBJ( OBJ_TABSUP ) ) {
    nReturn = (m_nParamGroup == DBMCLI_PARAMGROUP_SUPPORT) ? 1 : 0;
  } else if( ISOBJ( OBJ_TABLEHEADER ) ) {
    nReturn = 1;
  } else if( ISOBJ( OBJ_ROWPAIR ) ) {
    if( m_Database != NULL ) {
      DBMCli_Parameters & oParams = m_Database->GetParameters( ); 
      nReturn = (m_nItem < oParams.ParameterArray( ).GetSize( )) ? -1 : 0; 
    }
  } else if (( ISOBJ( OBJ_EVENROW ) ) || 
             ( ISOBJ( OBJ_ODDROW ) )     ) {
    nReturn = (FindNextParam( ) == true) ? 1 : 0;
  }
  return nReturn;
}

// Devspaces

SAPDB_Int2 DBMWeb_TemplateWizard :: askForWriteCountStep6 ( const Tools_DynamicUTF8String & szName )
{
  SAPDB_Int2 nReturn = 0;

  if( ISOBJ( OBJ_STEP6 ) ) {
    nReturn = 1;
  } else if( ISOBJ( OBJ_CLASS ) ) {
    nReturn = 1;
  } else if ( ISOBJ( OBJ_WITHSYSTEM ) ) {
    nReturn = FindSystemDevspace() ? 1 : 0;
  } else if ( ISOBJ( OBJ_WITHOUTSYSTEM ) ) {
    nReturn = FindSystemDevspace() ? 0 : 1;
  } else if( ISOBJ( OBJ_TABLOG ) ) {
    nReturn = (m_nDevspaceClass == DBMCLI_DEVSPACECLASS_LOG) ? 1 : 0;
  } else if( ISOBJ( OBJ_TABDAT ) ) {
    nReturn = (m_nDevspaceClass == DBMCLI_DEVSPACECLASS_DATA) ? 1 : 0;
  } else if( ISOBJ( OBJ_TABSYS ) ) {
    nReturn = (m_nDevspaceClass == DBMCLI_DEVSPACECLASS_SYS) ? 1 : 0;
  } else if( ISOBJ( OBJ_NORMAL ) ) {
    nReturn = ( !m_bDevspaceMirrored ) ? 1 : 0;
  } else if( ISOBJ( OBJ_MIRRORED ) ) {
    nReturn = ( m_bDevspaceMirrored ) ? 1 : 0;
  } else if( ISOBJ( OBJ_TABLEHEADER ) ) {
    nReturn = 1;
  } else if( ISOBJ( OBJ_ROWPAIR ) ) {
    if( m_Database != NULL ) {
      DBMCli_Devspaces & oDevspaces = m_Database->GetDevspaces( ); 
      nReturn = (m_nItem < oDevspaces.DevspaceArray( ).GetSize( )) ? -1 : 0; 
    }
  } else if (( ISOBJ( OBJ_EVENROW ) ) || 
             ( ISOBJ( OBJ_ODDROW ) )     ) {
    nReturn = (FindNextDevspace( ) == true) ? 1 : 0;
  }
  return nReturn;
}

// InstalMode

SAPDB_Int2 DBMWeb_TemplateWizard :: askForWriteCountStep7 ( const Tools_DynamicUTF8String & szName )
{
  SAPDB_Int2 nReturn = 0;

  if( ISOBJ( OBJ_STEP7 ) ) {
    nReturn = 1;
  } else if( ISOBJ( OBJ_FORM ) ) {
    nReturn = 1;
  } else if( ISOBJ( OBJ_FORM7 ) ) {
    nReturn = 1;
  }

  return nReturn;
}

// Install

SAPDB_Int2 DBMWeb_TemplateWizard :: askForWriteCountStep8 ( const Tools_DynamicUTF8String & szName )
{
  SAPDB_Int2 nReturn = 0;

  if( ISOBJ( OBJ_STEP8 ) ) {
    nReturn = 1;
  } else if( ISOBJ( OBJ_FORM ) ) {
    nReturn = 1;
  } else if( ISOBJ( OBJ_FORM8 ) ) {
    nReturn = 1;
  }

  return nReturn;
}

// Finish

SAPDB_Int2 DBMWeb_TemplateWizard :: askForWriteCountStep9 ( const Tools_DynamicUTF8String & szName )
{
  SAPDB_Int2 nReturn = 0;

  if( ISOBJ( OBJ_STEP9 ) ) {
    nReturn = 1;
  }

  return nReturn;
}

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateWizard :: askForValue
  -----------------------------------------------------------------------------
*/
Tools_DynamicUTF8String DBMWeb_TemplateWizard :: askForValue ( const Tools_DynamicUTF8String & szName )
{
  const SAPDB_Char * pReturn = NULL;

  m_sValue.Empty();

  if( ISOBJ( OBJ_STEP ) ) {
    m_sValue.Format("%d", m_nStep);
  } else if( ISOBJ( OBJ_STATE ) ) {
    m_sValue.Format("%d", m_nState);
  } else if( ISOBJ( OBJ_MSG ) ) {
    m_sValue = m_sMessage; 
  } else if( ISOBJ( OBJ_LOSE ) ) {
    if( m_bLose ) {
      m_sValue = "1";
    } else {
      m_sValue = "0";
    }
  } else {
    switch( m_nStep ) {
      case 1:
        askForValueStep1( szName );
        break;
      case 2:
        askForValueStep2( szName );
        break;
      case 3:
        askForValueStep3( szName );
        break;
      case 4:
        askForValueStep4( szName );
        break;
      case 5:
        askForValueStep5( szName );
        break;
      case 6:
        askForValueStep6( szName );
        break;
      case 7:
        askForValueStep7( szName );
        break;
      case 8:
        askForValueStep8( szName );
        break;
      case 9:
        askForValueStep9( szName );
        break;
    }
  }

  pReturn = m_sValue;
  return _Tools_UTF8Ptr(pReturn);
} 

// Databasename

void DBMWeb_TemplateWizard :: askForValueStep1 ( const Tools_DynamicUTF8String & szName )
{
  m_sValue.Empty( );

  if( ISOBJ( OBJ_DB ) ) {
    m_sValue = m_Wizard->GetDatabaseName( );
  } else if( ISOBJ( OBJ_SRV ) ) {
    m_sValue = m_Wizard->GetServerName( );
  } else if( ISOBJ( OBJ_USR ) ) {
    m_sValue = m_Wizard->GetOSUserName( );
  } else if( ISOBJ( OBJ_PWD ) ) {
    m_sValue = m_Wizard->GetOSPassword( );
  } 
}

// Installations

void DBMWeb_TemplateWizard :: askForValueStep2 ( const Tools_DynamicUTF8String & szName )
{
  m_sValue.Empty( );

  DBMCli_EnumInstallationArray & aInst = m_Wizard->GetNode( ).EnumInstallationArray( );

  if( ISOBJ( OBJ_SRV ) ) {
    m_sValue = m_Wizard->GetServerName( );
    if( m_sValue.IsEmpty( ) ) {
      m_sValue = " the local server";
    }
  } else if( ISOBJ( OBJ_VER ) ) {
    m_sValue = aInst[m_nItem - 1].Version( ).GetName( );
  } else if( ISOBJ( OBJ_INST ) ) {
    m_sValue = aInst[m_nItem - 1].InstRoot( );
  } else if( ISOBJ( OBJ_DESC ) ) {
    m_sValue.Empty( );
  } else if( ISOBJ( OBJ_CHECKED ) ) {
    if( m_Wizard->GetVersion( ).GetNumber( ) == 0 && m_nItem == 1 ) {
        m_sValue = OBJ_CHECKED;
    } else {
      if( aInst[m_nItem - 1].Version( ).GetNumber( ) == m_Wizard->GetVersion( ).GetNumber( ) ) {
        m_sValue = OBJ_CHECKED;
      } else {
        m_sValue.Empty( );
      }
    }
  } 
}

// Users

void DBMWeb_TemplateWizard :: askForValueStep3 ( const Tools_DynamicUTF8String & szName )
{
  m_sValue.Empty( );

  if( ISOBJ( OBJ_DBM ) ) {
    m_sValue = m_Wizard->GetDBMName( );
  } else if( ISOBJ( OBJ_DBMPWD ) ) {
    m_sValue = m_Wizard->GetDBMPassword( );
  } else if( ISOBJ( OBJ_DBA ) ) {
    m_sValue = m_Wizard->GetDBAName( );
  } else if( ISOBJ( OBJ_DBAPWD ) ) {
    m_sValue = m_Wizard->GetDBAPassword( );
  } 
}

// InitMode

void DBMWeb_TemplateWizard :: askForValueStep4 ( const Tools_DynamicUTF8String & szName )
{
  m_sValue.Empty( );

  DBMCli_EnumDatabaseArray & aDatabase = m_Wizard->GetNode( ).EnumDatabaseArray( );

  if( ISOBJ( OBJ_INITMODE ) ) {
    m_sValue = m_Wizard->GetInitMode( );
  } else {

    if (m_Wizard->GetInitMode( ) == DBMCLI_WIZARD_INITMODE_DEF) {
      if( ISOBJ( OBJ_INSTTYPE ) ) {
        m_sValue = m_Wizard->GetInstanceType( );
      }
    } else if (m_Wizard->GetInitMode( ) == DBMCLI_WIZARD_INITMODE_CPY) {
      if( ISOBJ( OBJ_DB ) ) {
        m_sValue = aDatabase[m_nItem - 1].Name( );
      } else if( ISOBJ( OBJ_VER ) ) {
        m_sValue = aDatabase[m_nItem - 1].Version( ).GetName( );
      } else if( ISOBJ( OBJ_INST ) ) {
        m_sValue = aDatabase[m_nItem - 1].InstRoot( );
      } 
    } else if (m_Wizard->GetInitMode( ) == DBMCLI_WIZARD_INITMODE_RES) {
      if (m_nMediumType == DBMCLI_MEDIUMTYPE_SINGLE) {
        DBMCli_MediumArray & aMedium = m_Database->GetMedia( ).MediumArray( );
        if( ISOBJ( OBJ_NAME ) ) {
          m_sValue = aMedium[m_nItem - 1].Name( );
        } else if( ISOBJ( OBJ_LOC ) ) {
          m_sValue = aMedium[m_nItem - 1].Location( );
        } else if( ISOBJ( OBJ_DEVTYPE ) ) {
          m_sValue = aMedium[m_nItem - 1].DeviceType( );
        } else if( ISOBJ( OBJ_BAKTYPE ) ) {
          m_sValue = aMedium[m_nItem - 1].BackupType( );
        } else if( ISOBJ( OBJ_MOD ) ) {
          m_sValue = aMedium[m_nItem - 1].Modified( );
        } else if( ISOBJ( OBJ_SIZE ) ) {
          if(aMedium[m_nItem - 1].Pages( ) == "0") {
            m_sValue.Empty();
          } else {
            m_sValue = aMedium[m_nItem - 1].Pages( );
          }
        } else if( ISOBJ( OBJ_OVW ) ) {
          m_sValue = aMedium[m_nItem - 1].Overwrite( );
        }
      } else {
        DBMCli_MediumParallelArray & aMediumParallel = m_Database->GetMedia( ).MediumParallelArray( );
        if( ISOBJ( OBJ_NAME ) ) {
          m_sValue = aMediumParallel[m_nItem - 1].Name( );;
        } else if( ISOBJ( OBJ_DEVTYPE ) ) {
          m_sValue = aMediumParallel[m_nItem - 1].DeviceType( );
        } else if( ISOBJ( OBJ_BAKTYPE ) ) {
          m_sValue = aMediumParallel[m_nItem - 1].BackupType( );;
        } else if( ISOBJ( OBJ_MOD ) ) {
          m_sValue = aMediumParallel[m_nItem - 1].Modified( );
        } else if( ISOBJ( OBJ_OVW ) ) {
          m_sValue = aMediumParallel[m_nItem - 1].Overwrite( );
        }
      }
    }
  }
}

// Parameter

void DBMWeb_TemplateWizard :: askForValueStep5 ( const Tools_DynamicUTF8String & szName )
{
  m_sValue.Empty( );

  if( ISOBJ( OBJ_GROUP ) ) {
    m_sValue = m_Database->GetParameters( ).GroupNameFromGroup( m_nParamGroup );
  } else if( ISOBJ( OBJ_TABLEHEADER ) ) {
    switch( m_nParamGroup ) {
     case DBMCLI_PARAMGROUP_GENERAL: 
        m_sValue = "General ";
        break;
      case DBMCLI_PARAMGROUP_EXTENDED: 
        m_sValue = "Extended ";
        break;
      case DBMCLI_PARAMGROUP_SUPPORT: 
        m_sValue = "Support ";
        break;
      default:
        m_sValue = "???";
        break;
    }
    m_sValue += "Parameters";

  } else if( ISOBJ( OBJ_NAME ) ) {
    DBMCli_ParameterArray & aParams = m_Database->GetParameters( ).ParameterArray( );
    m_sValue = aParams[m_nItem].Name( );

  } else if( ISOBJ( OBJ_CHANGE ) ) {
    DBMCli_ParameterArray & aParams = m_Database->GetParameters( ).ParameterArray( );
    if (aParams[m_nItem].Change( )) {
      m_sValue = "1";
    } else {
      m_sValue = "0";
    }

  } else if( ISOBJ( OBJ_VALUE ) ) {
    DBMCli_ParameterArray & aParams = m_Database->GetParameters( ).ParameterArray( );

    m_sValue = aParams[m_nItem].LastKnownGood( );

    if( m_sValue.IsEmpty() )
      m_sValue = "&nbsp";

  } else if( ISOBJ( OBJ_NEWVALUE ) ) {
    DBMCli_ParameterArray & aParams = m_Database->GetParameters( ).ParameterArray( );

    m_sValue.Empty( );

    if( aParams[m_nItem].Value( ) != aParams[m_nItem].LastKnownGood() ) {
      m_sValue = aParams[m_nItem].Value( );
    }
    
    if( m_sValue.IsEmpty() )
      m_sValue = "&nbsp";

  } else if( ISOBJ( OBJ_DESC ) ) {
    DBMCli_ParameterArray & aParams = m_Database->GetParameters( ).ParameterArray( );
    
    m_sValue = aParams[m_nItem].Help( );

    if( m_sValue.IsEmpty() )
      m_sValue = "&nbsp";

    m_nItem++;
  }
}

// Devspaces

void DBMWeb_TemplateWizard :: askForValueStep6 ( const Tools_DynamicUTF8String & szName )
{
  m_sValue.Empty( );

  if( ISOBJ( OBJ_CLASS ) ) {
    m_sValue = m_Database->GetDevspaces( ).ClassNameFromClass( m_nDevspaceClass );
  } else if( ISOBJ( OBJ_TABLEHEADER ) ) {
    switch( m_nDevspaceClass ) {
      case DBMCLI_DEVSPACECLASS_LOG: 
        m_sValue = "Log ";
        break;
      case DBMCLI_DEVSPACECLASS_DATA: 
        m_sValue = "Data ";
        break;
      case DBMCLI_DEVSPACECLASS_SYS: 
        m_sValue = "System ";
        break;
      default:
        m_sValue = "???";
        break;
    }
    m_sValue += "Devspaces";
  } else if( ISOBJ( OBJ_NAME ) ) {
    DBMCli_DevspaceArray & aDevspaces = m_Database->GetDevspaces( ).DevspaceArray( );
    m_sValue = aDevspaces[m_nItem].Name( );
  } else if( ISOBJ( OBJ_SIZE ) ) {
    DBMCli_DevspaceArray & aDevspaces = m_Database->GetDevspaces( ).DevspaceArray( );
    if( aDevspaces[m_nItem].Pages( ) > 0 ) {
      m_sValue.Format( "%d", aDevspaces[m_nItem].Pages( ) );
    }
  } else if( ISOBJ( OBJ_TYPE ) ) {
    DBMCli_DevspaceArray & aDevspaces = m_Database->GetDevspaces( ).DevspaceArray( );
    if( aDevspaces[m_nItem].Pages( ) > 0 ) {
      m_sValue = aDevspaces[m_nItem].TypeName( );
    }
  } else if( ISOBJ( OBJ_LOC ) ) {
    DBMCli_DevspaceArray & aDevspaces = m_Database->GetDevspaces( ).DevspaceArray( );
    if( aDevspaces[m_nItem].Pages( ) > 0 ) {
      m_sValue = aDevspaces[m_nItem].Location( );
    }
    if( !m_bDevspaceMirrored ) {
      m_nItem++;
    }
  } else if( ISOBJ( OBJ_MLOC ) ) {
    DBMCli_DevspaceArray & aDevspaces = m_Database->GetDevspaces( ).DevspaceArray( );
    if( aDevspaces[m_nItem].Pages( ) > 0 ) {
      m_sValue = aDevspaces[m_nItem].MirroredLocation( );
    }
  } else if( ISOBJ( OBJ_MTYPE ) ) {
    DBMCli_DevspaceArray & aDevspaces = m_Database->GetDevspaces( ).DevspaceArray( );
    if( aDevspaces[m_nItem].Pages( ) > 0 ) {
      m_sValue = aDevspaces[m_nItem].MirroredTypeName( );
    }
    m_nItem++;
  }
}

// InstallMode

void DBMWeb_TemplateWizard :: askForValueStep7 ( const Tools_DynamicUTF8String & szName )
{
  m_sValue.Empty( );

  if( ISOBJ( OBJ_INSTMODE ) ) {
    m_sValue = m_Wizard->GetInstallMode( );
  } 
}

// Install

void DBMWeb_TemplateWizard :: askForValueStep8 ( const Tools_DynamicUTF8String & szName )
{
  m_sValue.Empty( );
}

// Finish

void DBMWeb_TemplateWizard :: askForValueStep9 ( const Tools_DynamicUTF8String & szName )
{
  m_sValue.Empty( );
}

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateWizard :: askForContinue
  -----------------------------------------------------------------------------
*/
bool DBMWeb_TemplateWizard :: askForContinue ( const Tools_DynamicUTF8String & szName )
{
  switch( m_nStep ) {
    case 5:
      return askForContinueParams( szName );
    case 6:
      return askForContinueDevspaces( szName );
    default:
      return false;
  }
}

bool DBMWeb_TemplateWizard :: askForContinueParams ( const Tools_DynamicUTF8String & szName )
{
  bool bRC = false;
  
  if( m_Database != NULL ) {
    DBMCli_ParameterArray & aParams = m_Database->GetParameters( ).ParameterArray( );

    if( ISOBJ(OBJ_ROWPAIR ) ) {
      bRC = ( m_nItem < aParams.GetSize( ) );
    }
  }

  return bRC;
}

bool DBMWeb_TemplateWizard  :: FindNextParam ( )
{
  bool bFound = false;

  if( m_Database != NULL ) {
    DBMCli_ParameterArray & aParams = m_Database->GetParameters( ).ParameterArray( );

    while( ( m_nItem < aParams.GetSize( ) ) && ( bFound == false ) ) {
      if( aParams[m_nItem].Group( ) == m_nParamGroup ) { 
        bFound = true;
      } else {
        m_nItem++;
      }
    }
  }

  return bFound;
}

bool DBMWeb_TemplateWizard :: askForContinueDevspaces ( const Tools_DynamicUTF8String & szName )
{
  bool bRC = false;
  
  if( m_Database != NULL ) {
    DBMCli_DevspaceArray & aDevspaces = m_Database->GetDevspaces( ).DevspaceArray( );

    if( ISOBJ( OBJ_ROWPAIR ) ) {
      bRC = ( m_nItem< aDevspaces.GetSize( ) );
    }
  }

  return bRC;
}

bool DBMWeb_TemplateWizard :: FindNextDevspace ( )
{
  bool bFound = false;

  if( m_Database != NULL) {
    DBMCli_DevspaceArray & aDevspaces = m_Database->GetDevspaces( ).DevspaceArray( );

    while( ( m_nItem < aDevspaces.GetSize( ) ) && ( bFound == false ) ) {
      if( aDevspaces[m_nItem].Class( ) == m_nDevspaceClass ) {
        bFound = true;
      } else {
        m_nItem++;
      }
    }
  }

  return bFound;
}

bool DBMWeb_TemplateWizard :: FindSystemDevspace ( )
{
  bool        bFound = false;

  if( m_Database != NULL ) {
    DBMCli_DevspaceArray & aDevspaces = m_Database->GetDevspaces( ).DevspaceArray( );

    for (SAPDB_Int2  nDevspace = 0; nDevspace < aDevspaces.GetSize( ); ++nDevspace) {
      if( aDevspaces[nDevspace].Class( ) == DBMCLI_DEVSPACECLASS_SYS ) {
        bFound = true;
        break;
      } 
    }  
  }

  return bFound;
} 
