/*! 
  -----------------------------------------------------------------------------
  module: DBMWeb_DBMWebWizard.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager WebAgent Extension
  description:  Database wizard funtions of the DBMWeb_DBMWeb class
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
#include "DBM/Web/DBMWeb_TemplateMsgBox.hpp"
#include "DBM/Web/DBMWeb_DBMWeb.hpp"
#include "DBM/Web/DBMWeb_Web.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define PARAM_ACTION_VAL_NEXT     "NEXT"
#define PARAM_ACTION_VAL_PREV     "PREV"
#define PARAM_ACTION_VAL_INSTALL  "INSTALL"

#define PARAM_STEP             "Step"
#define PARAM_LOSE             "Lose"

#define PARAM_DATABASE         "Database"
#define PARAM_SERVER           "Server"
#define PARAM_USER             "User"
#define PARAM_PASSWORD         "Password"

#define PARAM_REINSTALL         "Reinstall"
#define PARAM_REINSTALL_VAL_YES "YES"

#define PARAM_VERSION          "Version"
#define PARAM_INSTROOT         "InstRoot"

#define PARAM_DBMNAME          "DBMName"
#define PARAM_DBMPWD1          "DBMPwd1"
#define PARAM_DBMPWD2          "DBMPwd2"

#define PARAM_DBANAME          "DBAName"
#define PARAM_DBAPWD1          "DBAPwd1"
#define PARAM_DBAPWD2          "DBAPwd2"

#define PARAM_INITMODE         "InitMode"
#define PARAM_INITMODE_VAL_DEF "DEFAULT"
#define PARAM_INITMODE_VAL_CUR "CURRENT"
#define PARAM_INITMODE_VAL_CPY "COPY"
#define PARAM_INITMODE_VAL_RES "RESTORE"

#define PARAM_INSTANCETYPE            "InstanceType"
#define PARAM_INSTANCETYPE_VAL_OLTP   "OLTP"
#define PARAM_INSTANCETYPE_VAL_LC     "LC"
#define PARAM_INSTANCETYPE_VAL_CS     "CS"
#define PARAM_INSTANCETYPE_VAL_BW     "BW"

#define PARAM_MEDIUMTYPE       "MediumType"

#define PARAM_GROUP            "Group"
#define PARAM_CLASS            "Class"

#define PARAM_INSTALLMODE             "InstallMode"
#define PARAM_INSTALLMODE_VAL_INSTALL "INSTALL"
#define PARAM_INSTALLMODE_VAL_RESTORE "RESTORE"

#define WIZ_ERR_NODBNAME    "Please enter a valid name for the new database."
#define WIZ_ERR_DBEXISTS    "Database instance already exists. Please choose a different name."
#define WIZ_ERR_NOINST      "Please mark the installation you want to use."
#define WIZ_ERR_NODBM       "Please enter a valid user name for the database manager (DBM)."
#define WIZ_ERR_NODBMP      "Please enter a password for the database manager (DBM)."
#define WIZ_ERR_NODBA       "Please enter a valid user name for the database administrator (DBA)."
#define WIZ_ERR_NODBAP      "Please enter a password for the database administrator (DBA)."
#define WIZ_ERR_SAMENAME    "The database manager and the database administrator can not have the same name."
#define WIZ_ERR_PWD         "The passwords you typed do not match. Type the password in both text boxes."
#define WIZ_ERR_NOINITMODE  "Please mark a valid initiallization mode for parameters." 
#define WIZ_ERR_NOINSTTYPE  "Please mark a valid instance type for the database."
#define WIZ_ERR_NOINSTMODE  "Please mark a valid installation mode for the database."
#define WIZ_ERR_DEVSPACE    "Devspace definition not complete."
#define WIZ_ERR_NODB        "Please mark a database."
#define WIZ_ERR_NOMEDIUM    "Please mark a medium."
#define WIZ_ERR_NOVALIDDB   "No valid database object."
#define WIZ_ERR_NOVALIDWIZ  "No valid wizard object."

/*    
  =============================================================================
  class:        DBMWeb_DBMWeb
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_DBMWeb :: wizardDB
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMWeb_DBMWeb :: wizardDB ( sapdbwa_WebAgent    & wa,
                                       sapdbwa_HttpRequest & request,
                                       sapdbwa_HttpReply   & reply )
{
  SAPDB_Bool bOK = SAPDB_TRUE;

  DBMCli_String sAction;
  DBMCli_String sStep;
  SAPDB_Int nStep = 1;
  SAPDB_Int nState = DBMWEB_TEMPLWIZARD_STATE_WORK;

  SAPDB_Bool bReinstall = SAPDB_FALSE;

  DBMCli_String sLose;
  SAPDB_Bool bLose = SAPDB_FALSE;

  DBMCli_MediumTypeEnum nMediumType = DBMCLI_MEDIUMTYPE_SINGLE;
  DBMCli_ParameterGroup nParamGroup = DBMCLI_PARAMGROUP_GENERAL; 
  DBMCli_DevspaceClass nDevspaceClass = DBMCLI_DEVSPACECLASS_DATA;
  SAPDB_Bool bDevspaceMirrored = SAPDB_FALSE;

  SAPDB_Bool bInstalled = SAPDB_FALSE;

  Msg_List oMsgList;

  DBMCli_String sMsg;

  GetParameterValue(PARAM_ACTION, request, sAction);
  GetParameterValue(PARAM_STEP, request, sStep);
  GetParameterValue(PARAM_LOSE, request, sLose);

  if( !sStep.IsEmpty( ) ) {
    nStep = atoi( sStep );
  }

  if( sLose == "1" ) {
    bLose = SAPDB_TRUE;
  }

  sMsg.Empty();

  if( m_Wizard != NULL ) {
    if( sAction == PARAM_ACTION_VAL_NEXT ) {
      
      switch( nStep ) {
        // Databasename
        case 1: {
          DBMCli_String sDatabase;
          DBMCli_String sServer;
          DBMCli_String sUser;
          DBMCli_String sPassword;
          DBMCli_String sReinstall;

          sDatabase.Empty( );
          sServer.Empty( );
          sUser.Empty( );
          sPassword.Empty( );
          sReinstall.Empty( );

          GetParameterValue(PARAM_DATABASE, request, sDatabase);

          if( !sDatabase.IsEmpty( ) ) {
            GetParameterValue(PARAM_SERVER, request, sServer);
            GetParameterValue(PARAM_USER, request, sUser);
            GetParameterValue(PARAM_PASSWORD, request, sPassword);

            sServer.Trim( );
      
            m_Wizard->SetDatabaseName( sDatabase );
            m_Wizard->SetServerName( sServer );
            m_Wizard->SetOSUserName( sUser );
            m_Wizard->SetOSPassword( sPassword );

            DBMCli_Node & oNode = m_Wizard->GetNode( );

            oNode.SetServerName( sServer );
            if( oNode.Connect( oMsgList ) ) {
              if( oNode.EnumerateDatabases( oMsgList )  ) {

                SAPDB_Int nItem = oNode.DatabaseIndexByName( sDatabase );
                
                if( nItem < 0 ) {
                  if( oNode.EnumerateInstallations( oMsgList ) ) {
                    nStep++;
                  } else {
                    bOK = SAPDB_FALSE;
                  } 
                } else {

                  GetParameterValue(PARAM_REINSTALL, request, sReinstall);
                  if( sReinstall == PARAM_REINSTALL_VAL_YES ) {
                                   
                    DBMCli_EnumDatabaseArray & aDatabase = oNode.EnumDatabaseArray( );

                    m_Wizard->SetVersion( aDatabase[nItem].Version( ).GetName( ) );
                    m_Wizard->SetInstRoot( aDatabase[nItem].InstRoot( ) );

                    m_Wizard->SetReinstall( SAPDB_TRUE );

                    nStep += 2;
                  } else {

                    bReinstall = SAPDB_TRUE;
                  }
                }
              } else {
                bOK = SAPDB_FALSE;
              } 
              oNode.Disconnect( );
            }
          } else {
            sMsg = WIZ_ERR_NODBNAME;
            nState = DBMWEB_TEMPLWIZARD_STATE_ERROR;
          }
        }
        break;

        // Version
        case 2: {
          DBMCli_String sValue;
          DBMCli_String sVersion;
          DBMCli_String sInstRoot;

          GetParameterValue(PARAM_VERSION, request, sValue);

          if( !sValue.IsEmpty( ) ) {
            SAPDB_Int nPos = sValue.Find(" ");
            if( nPos > 0 ) {
              sVersion  = sValue.Left(nPos);
              sInstRoot = sValue.Right(sValue.GetLength () - nPos - 1);
            }
            m_Wizard->SetVersion(sVersion);
            m_Wizard->SetInstRoot(sInstRoot);
          }

          if( !sVersion.IsEmpty( ) ) {
            nStep++;
          } else {
            sMsg = WIZ_ERR_NOINST;
            nState = DBMWEB_TEMPLWIZARD_STATE_ERROR;
          }
        }
        break;

        // Users
        case 3: {
          DBMCli_String sDBMName;
          DBMCli_String sDBMPwd1;
          DBMCli_String sDBMPwd2;

          DBMCli_String sDBAName;
          DBMCli_String sDBAPwd1;
          DBMCli_String sDBAPwd2;

          GetParameterValue(PARAM_DBMNAME, request, sDBMName);
          GetParameterValue(PARAM_DBMPWD1, request, sDBMPwd1);
          GetParameterValue(PARAM_DBMPWD2, request, sDBMPwd2);

          GetParameterValue(PARAM_DBANAME, request, sDBAName);
          GetParameterValue(PARAM_DBAPWD1, request, sDBAPwd1);
          GetParameterValue(PARAM_DBAPWD2, request, sDBAPwd2);

          m_Wizard->SetDBMName( sDBMName );
          m_Wizard->SetDBMPwd( sDBMPwd1 );
          m_Wizard->SetDBAName( sDBAName );
          m_Wizard->SetDBAPwd( sDBAPwd1 );

          SAPDB_Bool bInputOK = SAPDB_FALSE;

          if( !sDBMName.IsEmpty( ) ) {
            if( !( sDBMPwd1.IsEmpty( ) && sDBMPwd2.IsEmpty( ) ) ) {
              if( sDBMPwd1 == sDBMPwd2 ) {
                if( !sDBAName.IsEmpty( ) ) {
                  if( sDBMName != sDBAName ) {
                    if( !( sDBAPwd1.IsEmpty( ) && sDBAPwd1.IsEmpty( ) ) ) {
                      if( sDBAPwd1 == sDBAPwd2 ) {
                        bInputOK = SAPDB_TRUE;
                      } else {
                        m_Wizard->SetDBAPwd( "" );
                        sMsg = WIZ_ERR_PWD;
                        nState = DBMWEB_TEMPLWIZARD_STATE_ERROR;
                      }
                    } else {
                      sMsg = WIZ_ERR_NODBAP;
                      nState = DBMWEB_TEMPLWIZARD_STATE_ERROR;
                    }
                  } else {
                    m_Wizard->SetDBAPwd( "" );
                    sMsg = WIZ_ERR_SAMENAME;
                    nState = DBMWEB_TEMPLWIZARD_STATE_ERROR;
                  }
                } else {
                  sMsg = WIZ_ERR_NODBA;
                  nState = DBMWEB_TEMPLWIZARD_STATE_ERROR;
                }
              } else {
                m_Wizard->SetDBMPwd( "" );
                sMsg = WIZ_ERR_PWD;
                nState = DBMWEB_TEMPLWIZARD_STATE_ERROR;
              }
            } else {
              sMsg = WIZ_ERR_NODBMP;
              nState = DBMWEB_TEMPLWIZARD_STATE_ERROR;
            }
          } else {
            sMsg = WIZ_ERR_NODBM;
            nState = DBMWEB_TEMPLWIZARD_STATE_ERROR;
          }

          if( bInputOK ) {
            bOK = SAPDB_FALSE;
            if( m_Wizard->GetReinstall( ) ) {
              if( m_Wizard->UseDatabase( &m_Database, oMsgList ) ) {
                bOK = SAPDB_TRUE;
                nStep++;
              }
            } else {
              if( m_Wizard->CreateDatabase( &m_Database, oMsgList ) ) {
                bOK = SAPDB_TRUE;
                nStep++;
              }
            }
            if( bOK ) {
              if( m_Database != NULL) {
                bOK = SAPDB_FALSE;
                DBMCli_Media & oMedia = m_Database->GetMedia( );
                if( oMedia.Refresh( oMsgList ) ) {
                  bOK = SAPDB_TRUE;
                }
              } else {
                sMsg = WIZ_ERR_NOVALIDDB;
                nState = DBMWEB_TEMPLWIZARD_STATE_ERROR;
              } 
            }
          }
        }
        break;

        // Parameter init mode
        case 4: {

          if( m_Database != NULL) {
            DBMCli_Parameters & oParams= m_Database->GetParameters( );

            DBMCli_String sInitMode;

            sInitMode = m_Wizard->GetInitMode( );

            if( !sInitMode.IsEmpty( ) ) {

              if( sInitMode == PARAM_INITMODE_VAL_DEF ) {

                DBMCli_String sInstanceType;
                GetParameterValue(PARAM_INSTANCETYPE, request, sInstanceType);
                m_Wizard->SetInstanceType( sInstanceType );

                if( sInstanceType.IsEmpty( ) ) {
                  sMsg = WIZ_ERR_NOINSTTYPE;
                  nState = DBMWEB_TEMPLWIZARD_STATE_ERROR;
                } else {
                  bOK = SAPDB_FALSE;
                  if( oParams.Open( oMsgList ) ) {
                    if( oParams.Init( m_Wizard->GetInstanceType( ), oMsgList ) ) {

                      DBMCli_ParameterStatus nStatus;
                      DBMCli_String sBadParam;
                      DBMCli_String sOwnValue;
                      DBMCli_String sCompValue;

                      if( oParams.Check( DBMCLI_PARAMGROUP_GENERAL, nStatus, sBadParam, sOwnValue, sCompValue, oMsgList ) ) {
                        if( oParams.Refresh( oMsgList ) ) {
                          bOK = SAPDB_TRUE;
                          nStep++;
                        }
                      }
                    }
                    oParams.Close( bOK, oMsgList ); 
                  }
                }
              } else if( sInitMode == PARAM_INITMODE_VAL_CUR ) {

                bOK = SAPDB_FALSE;
                if( oParams.Refresh( oMsgList ) ) {
                  bOK = SAPDB_TRUE;
                  nStep++;
                }

              } else if( sInitMode == PARAM_INITMODE_VAL_CPY ) {

                DBMCli_String sDatabase;
                GetParameterValue(PARAM_DATABASE, request, sDatabase);

                if( sDatabase.IsEmpty( ) ) {
                  sMsg = WIZ_ERR_NODB;
                  nState = DBMWEB_TEMPLWIZARD_STATE_ERROR;
                } else {
                  bOK = SAPDB_FALSE;
                  if( oParams.Open( oMsgList ) ) {
                    if( oParams.Copy( sDatabase, oMsgList ) ) {

                      DBMCli_ParameterStatus nStatus;
                      DBMCli_String sBadParam;
                      DBMCli_String sOwnValue;
                      DBMCli_String sCompValue;

                      if( oParams.Check( DBMCLI_PARAMGROUP_EXTENDED, nStatus, sBadParam, sOwnValue, sCompValue, oMsgList ) ) {
                        if( oParams.Refresh( oMsgList ) ) {
                          bOK = SAPDB_TRUE;
                          nStep++;
                        }
                      }
                    }
                    oParams.Close( bOK, oMsgList ); 
                  }
                } 

              } else if( sInitMode == PARAM_INITMODE_VAL_RES ) {

                DBMCli_String sMediumName;
                GetParameterValue(PARAM_NAME, request, sMediumName);

                if( sMediumName.IsEmpty( ) ) {
                  sMsg = WIZ_ERR_NOMEDIUM;
                  nState = DBMWEB_TEMPLWIZARD_STATE_ERROR;
                } else {
                  bOK = SAPDB_FALSE;
                  if( oParams.Recover( sMediumName, oMsgList ) ) {
                    if( oParams.Refresh( oMsgList ) ) {
                      bOK = SAPDB_TRUE;
                      nStep++;
                    }
                  }
                }
              }

            } else {
              sMsg = WIZ_ERR_NOINITMODE;
              nState = DBMWEB_TEMPLWIZARD_STATE_ERROR;
            }
          } else {
            sMsg = WIZ_ERR_NOVALIDDB;
            nState = DBMWEB_TEMPLWIZARD_STATE_ERROR;
          } 
        }
        break;

        // Parameter
        case 5: {
          if( m_Database != NULL) {
            bOK = SAPDB_FALSE;

            DBMCli_Devspaces & oDevspaces = m_Database->GetDevspaces( );

            if( oDevspaces.Init( oMsgList ) ) {
              bOK = SAPDB_TRUE;
              nStep++;
            }
          } else {
            sMsg = WIZ_ERR_NOVALIDDB;
            nState = DBMWEB_TEMPLWIZARD_STATE_ERROR;
          }

        }
        break;

        // Devspaces
        case 6: {
          if( m_Database != NULL) {
            DBMCli_Devspaces & oDevspaces = m_Database->GetDevspaces( );

            if( oDevspaces.Complete( ) ) {
              nStep++;
            } else {
              sMsg = WIZ_ERR_DEVSPACE;
              nState = DBMWEB_TEMPLWIZARD_STATE_ERROR;
            }
          } else {
            sMsg = WIZ_ERR_NOVALIDDB;
            nState = DBMWEB_TEMPLWIZARD_STATE_ERROR;
          }
        }
        break;

        // Install mode
        case 7: {
          DBMCli_String sInstallMode;

          GetParameterValue(PARAM_INSTALLMODE, request, sInstallMode);

          m_Wizard->SetInstallMode( sInstallMode );

          if( !sInstallMode.IsEmpty( ) ) {
            nStep++;
          } else {
            sMsg = WIZ_ERR_NOINSTMODE;
            nState = DBMWEB_TEMPLWIZARD_STATE_ERROR;
          }

        }
        break;

      }

    } else if( sAction == PARAM_ACTION_VAL_PREV ) {

      switch( nStep ) {
        // Databasename
        case 1: {
          nStep--;
        }
        break;

        // Version
        case 2: {
          nStep--;
        }
        break;

        // Users
        case 3: {
          if( m_Wizard->GetReinstall( ) ) {
            nStep -= 2;
          } else {
            nStep--;
          }
        }
        break;

        // Parameter init mode
        case 4: {
          if( m_Wizard->GetCreatedFlag( ) ) {
            bOK = SAPDB_FALSE;
            if( m_Wizard->DropDatabase( &m_Database, oMsgList ) ) {
              bOK = SAPDB_TRUE;
              nStep--;
            }
          } else {
            nStep--;
          }
        }
        break;

        // Parameters
        case 5: {
          if( bLose  ) {

            if( m_Database != NULL) {

              DBMCli_Parameters & oParams= m_Database->GetParameters( );

              bOK = SAPDB_FALSE;
              if( m_Wizard->GetReinstall( ) ) {
                if( oParams.Restore( 1, oMsgList ) ) {
                  bOK = SAPDB_TRUE;
                }
              } else {
                if( oParams.Remove( oMsgList ) ) {
                  bOK = SAPDB_TRUE;
                }
              }
              nStep--;
              bLose = SAPDB_FALSE;

            } else {
              sMsg = WIZ_ERR_NOVALIDDB;
              nState = DBMWEB_TEMPLWIZARD_STATE_ERROR;
            }
          } else {
            bLose = SAPDB_TRUE;
          }
        }
        break;

        // Devspaces
        case 6: {
          nStep--;
        }
        break;

        // Install mode
        case 7: {
          nStep--;
        }
        break;

        // Install
        case 8: {
          nStep--;
        }
        break;

      }

    } else if( sAction == PARAM_ACTION_VAL_VIEW ) {
      switch( nStep ) {
        // Parameter init mode
        case 4: {
          DBMCli_String sInitMode;
          GetParameterValue(PARAM_INITMODE, request, sInitMode);
          m_Wizard->SetInitMode( sInitMode );

          DBMCli_String sMediumType;
          GetParameterValue(PARAM_MEDIUMTYPE, request, sMediumType);
          DBMCli_MediumType oMediumType( sMediumType );
          if( oMediumType.Value( ) == DBMCLI_MEDIUMTYPE_UNKNOWN ) {
            nMediumType = DBMCLI_MEDIUMTYPE_SINGLE;
          } else {
            nMediumType = oMediumType.Value( );
          }
        }
        break;

        // Parameters
        case 5: {
          DBMCli_String sGroup;
          GetParameterValue(PARAM_GROUP, request, sGroup);
          nParamGroup =  m_Database->GetParameters( ).GroupFromGroupName( sGroup );
        }
        break;

        // Devspaces
        case 6: {
          if( m_Database != NULL) {
            DBMCli_Devspaces & oDevspaces = m_Database->GetDevspaces( );

            if( oDevspaces.Init( oMsgList ) ) {
              DBMCli_String sClass;
              GetParameterValue(PARAM_CLASS, request, sClass);

              nDevspaceClass =  m_Database->GetDevspaces( ).ClassFromClassName( sClass );
              if( ( nDevspaceClass == DBMCLI_DEVSPACECLASS_LOG ) &&
                  ( m_Database->GetDevspaces( ).LogMode( ) == DBMCLI_LOGMODE_DUAL ) ) {
                bDevspaceMirrored = SAPDB_TRUE;
              }
            } else {
              bOK = SAPDB_FALSE;
            }
          } else {
            sMsg = WIZ_ERR_NOVALIDDB;
            nState = DBMWEB_TEMPLWIZARD_STATE_ERROR;
          }
        }
        break; 
      }

    } else if( sAction == PARAM_ACTION_VAL_INSTALL ) {

      if( m_Database != NULL ) {
        bOK = SAPDB_FALSE;
        if( m_Wizard->InstallDatabase( m_Database, oMsgList ) ) {
          bOK = SAPDB_TRUE;
          bInstalled = SAPDB_TRUE;

          nStep = 9;

          DBMWeb_TemplateWizard oTemplate( wa, m_Wizard, m_Database, nStep, nState, sMsg);
          oTemplate.SetLose( bLose );
          oTemplate.SetParamGroup( nParamGroup );
          oTemplate.SetDevspaceClass( nDevspaceClass );
          oTemplate.SetDevspaceMirrored( bDevspaceMirrored );
          oTemplate.writePage( Tools_TemplateWriterWA( reply ) );

          m_sLastWorkURL = "";
          delete m_Wizard;
          m_Wizard = NULL;
        }
      } else {
        sMsg = WIZ_ERR_NOVALIDDB;
        nState = DBMWEB_TEMPLWIZARD_STATE_ERROR;
      }
    }

    if( !bInstalled ) {
      if( bOK ) {
        DBMWeb_TemplateWizard oTemplate( wa, m_Wizard, m_Database, nStep, nState, sMsg);
        oTemplate.SetReinstall( bReinstall );
        oTemplate.SetLose( bLose );
        oTemplate.SetMediumType( nMediumType );
        oTemplate.SetParamGroup( nParamGroup );
        oTemplate.SetDevspaceClass( nDevspaceClass );
        oTemplate.SetDevspaceMirrored( bDevspaceMirrored );
        oTemplate.writePage( Tools_TemplateWriterWA( reply ) );
      } else {
        sMsg.Format( "%d %s",  oMsgList.ID( ), oMsgList.Message( ) );
        nState = DBMWEB_TEMPLWIZARD_STATE_ERROR;

        DBMWeb_TemplateWizard oTemplate( wa, m_Wizard, m_Database, nStep, nState, sMsg);
        oTemplate.SetReinstall( bReinstall );
        oTemplate.SetLose( bLose );
        oTemplate.SetMediumType( nMediumType );
        oTemplate.SetParamGroup( nParamGroup );
        oTemplate.SetDevspaceClass( nDevspaceClass );
        oTemplate.SetDevspaceMirrored( bDevspaceMirrored );
        oTemplate.writePage( Tools_TemplateWriterWA( reply ) );
      }
    }
  } else {
    DBMWeb_TemplateMsgBox oMsgBox(wa, DBMWEB_TEMPLMSGBOX_ERROR, "", WIZ_ERR_NOVALIDWIZ);
    oMsgBox.writePage(Tools_TemplateWriterWA(reply));
  }

  return SAPDB_TRUE;
} 
