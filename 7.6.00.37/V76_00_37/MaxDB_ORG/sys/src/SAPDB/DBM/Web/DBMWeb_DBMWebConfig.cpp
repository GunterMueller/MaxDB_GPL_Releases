/*! 
  -----------------------------------------------------------------------------
  module: DBMWeb_DBMWebConfig.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager WebAgent Extension
  description:  Database configuration funtions of the DBMWeb_DBMWeb class
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
#include "DBM/Web/DBMWeb_TemplateParams.hpp"
#include "DBM/Web/DBMWeb_TemplateParam.hpp"
#include "DBM/Web/DBMWeb_TemplateDevspaces.hpp"
#include "DBM/Web/DBMWeb_TemplateDevspace.hpp"
#include "DBM/Web/DBMWeb_TemplateMedia.hpp"
#include "DBM/Web/DBMWeb_TemplateMedium.hpp"
#include "DBM/Web/DBMWeb_TemplateParmed.hpp"
#include "DBM/Web/DBMWeb_TemplateSysTabs.hpp"
#include "DBM/Web/DBMWeb_TemplateLogMode.hpp"
#include "DBM/Web/DBMWeb_TemplateUsers.hpp"
#include "DBM/Web/DBMWeb_TemplateUser.hpp"
#include "DBM/Web/DBMWeb_TemplateMsgBox.hpp"
#include "DBM/Web/DBMWeb_DBMWeb.hpp"
#include "DBM/Web/DBMWeb_Web.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/

#define PARAM_ACTION_VAL_INIT "INIT"

#define PARAM_GROUP         "Group"
#define PARAM_GROUP_VAL_GEN "GENERAL"
#define PARAM_GROUP_VAL_EXT "EXTENDED"
#define PARAM_GROUP_VAL_SUP "SUPPORT"

#define PARAM_CLASS          "Class"
#define PARAM_CLASS_VAL_SYS  "SYS"
#define PARAM_CLASS_VAL_DATA "DATA"
#define PARAM_CLASS_VAL_LOG  "LOG"

#define PARAM_LOCATION       "Location"
#define PARAM_TYPE           "Type"
#define PARAM_SIZE           "Size"
#define PARAM_MLOCATION      "MLocation"
#define PARAM_MTYPE          "MType"

#define PARAM_TYPE_VAL_SINGLE   "SINGLE"
#define PARAM_TYPE_VAL_PARALLEL "PARALLEL"

#define PARAM_GROUP          "Group"
#define PARAM_DEVICETYPE     "DeviceType"
#define PARAM_BACKUPTYPE     "BackupType"
#define PARAM_BLOCKSIZE      "BlockSize"
#define PARAM_OVERWRITE      "Overwrite"
#define PARAM_AUTOLOADER     "Autoloader"
#define PARAM_OSCOMMAND      "OSCommand"

#define PARAM_DBANAME        "DBAName"
#define PARAM_DBAPWD         "DBAPwd"
#define PARAM_DOMPWD         "DOMPwd" 

#define PARAM_ACTION_VAL_WAIT "WAIT"

#define DEV_ERR_NOTCOMPLETE  "Devspace definition not complete."

#define PARAM_NAME           "Name"
#define PARAM_PASSWORD       "Password"
#define PARAM_DISABLED       "Disabled"
#define PARAM_COMMENT        "Comment"
#define PARAM_PASSWORD2      "Password2"
#define PARAM_SRVRIGHT       "SrvRight" 

#define PARAM_ACTION_VAL_PWDCHANGE  "PWDCHANGE"
#define PARAM_ACTION_VAL_PWD2CHANGE "PWD2CHANGE"

#define PARAM_ACTION_VAL_CHANGEPWD  "CHANGEPWD"
#define PARAM_ACTION_VAL_CHANGEPWD2 "CHANGEPWD2"

#define LOGMODE_PARAM_NEWMODE       "Logmode"
#define LOGMODE_PARAM_DEVSPACE      "Devspace"
#define LOGMODE_PARAM_MTYPE         "MType"
#define LOGMODE_PARAM_MLOCATION     "MLocation"
#define LOGMODE_PARAM_RESTART       "Restart"
#define LOGMODE_VAL_RESTART         "ON"

#define LOGMODE_ACTION_REFRESH      "REFRESH"
#define LOGMODE_ACTION_EDIT         "EDIT"
#define LOGMODE_ACTION_CHANGE       "CHANGE"
#define LOGMODE_ACTION_SAVEDEV      "SAVEDEV"
#define LOGMODE_ACTION_CANCELDEV    "CANCELDEV"
#define LOGMODE_ACTION_SELECT       "SELECT"

/*    
  =============================================================================
  class:        DBMWeb_DBMWeb
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_DBMWeb :: configParameters
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMWeb_DBMWeb :: configParameters ( sapdbwa_WebAgent    & wa,
                                               sapdbwa_HttpRequest & request,
                                               sapdbwa_HttpReply   & reply )
{
  DBMCli_String sAction;
  DBMCli_String sGroup(PARAM_GROUP_VAL_GEN);

  GetParameterValue(PARAM_ACTION, request, sAction);
  GetParameterValue(PARAM_GROUP, request, sGroup);

  Msg_List oMsgList;
  DBMCli_Parameters & oParams = m_Database->GetParameters( );

  if( sAction == PARAM_ACTION_VAL_REFRESH ) {

    if( oParams.Refresh( oMsgList ) ) {

      DBMWeb_TemplateParams oTemplate( wa, oParams, oParams.GroupFromGroupName( sGroup ) );

      oTemplate.writePage( Tools_TemplateWriterWA( reply ) );

    } else {
      sendMsgListError( wa, request, reply, oMsgList, m_Database->DatabaseName() );
    }

  } else if( sAction == PARAM_ACTION_VAL_VIEW ) {

    DBMWeb_TemplateParams oTemplate( wa, oParams, oParams.GroupFromGroupName( sGroup ) );

    oTemplate.writePage( Tools_TemplateWriterWA( reply ) );
  }

  return SAPDB_TRUE;
} 

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_DBMWeb :: configParameter
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMWeb_DBMWeb :: configParameter ( sapdbwa_WebAgent    & wa,
                                              sapdbwa_HttpRequest & request,
                                              sapdbwa_HttpReply   & reply )
{
  DBMCli_String sAction;
  DBMCli_String sName;
  DBMCli_String sBack;

  GetParameterValue(PARAM_ACTION, request, sAction);
  GetParameterValue(PARAM_NAME, request, sName);
  GetParameterValue(PARAM_BACK, request, sBack);

  Msg_List oMsgList;
  DBMCli_Parameters & oParams = m_Database->GetParameters( );
  DBMCli_ParameterArray & aParams = oParams.ParameterArray( );

  SAPDB_Bool bParams = SAPDB_FALSE;

  if( aParams.GetSize( ) == 0 ) {
    if( oParams.Refresh( oMsgList ) ) {
      bParams = SAPDB_TRUE;
    } else {
      sendMsgListError( wa, request, reply, oMsgList, m_Database->DatabaseName() );
    }
  } else {
    bParams = SAPDB_TRUE;
  }

  if( bParams ) {

    oMsgList.ClearMessageList();

    if( sAction == PARAM_ACTION_VAL_EDIT ) {
      int nParam = oParams.IndexByName( sName );
      if( nParam >= 0 ) {

        DBMCli_Parameter & oParam = aParams[nParam];

        DBMWeb_TemplateParam oTemplate( wa, oParam, DBMCLI_PARAMETERSTAT_OK, "", sBack );
        oTemplate.writePage( Tools_TemplateWriterWA( reply ) );
      }

    } else if( sAction == PARAM_ACTION_VAL_PUT ) {

      DBMCli_String sValue;
      GetParameterValue(PARAM_VALUE, request, sValue);
      int nParam = oParams.IndexByName( sName );
      if( nParam >= 0 ) {

        DBMCli_Parameter & oParam = aParams[nParam];

        Msg_List oMsgList;

        if( oParams.Open( oMsgList ) ) {
          SAPDB_Bool bCommit = SAPDB_FALSE;

          if( oParam.Put( sValue, oMsgList ) ) {
          
            DBMCli_ParameterStatus nStatus;
            DBMCli_String sBadParam;
            DBMCli_String sOwnValue;
            DBMCli_String sCompValue;

            if( oParams.Check( DBMCLI_PARAMGROUP_GENERAL, nStatus, sBadParam, sOwnValue, sCompValue, oMsgList ) ) {
              if( nStatus == DBMCLI_PARAMETERSTAT_OK ) {

                if( !sBack.IsEmpty( ) ) {
                  MovetoURL( sBack, reply );
                } else {
                  DBMWeb_TemplateParams oTemplate( wa, oParams, oParam.Group( ) );
                  oTemplate.writePage( Tools_TemplateWriterWA( reply ) );
                }

                bCommit = SAPDB_TRUE;

              } else {

                if( oParam.Name( ) != sBadParam ) {
                  oParam = aParams[oParams.IndexByName( sBadParam )]; 
                }

                if( nStatus == DBMCLI_PARAMETERSTAT_REQUEST ) {
                  DBMWeb_TemplateParam oTemplate( wa, oParam, nStatus, sCompValue, sBack);
                  oTemplate.writePage( Tools_TemplateWriterWA( reply ) );
                } else {
                  DBMWeb_TemplateParam oTemplate( wa, oParam, nStatus, "", sBack);
                  oTemplate.writePage( Tools_TemplateWriterWA( reply ) );
                }
              }

            } else {
              sendMsgListError( wa, request, reply, oMsgList, m_Database->DatabaseName() );
            }
          } else {
            sendMsgListError( wa, request, reply, oMsgList, m_Database->DatabaseName() );
          }

          oParams.Close( bCommit, oMsgList ); 
        } else {
          sendMsgListError( wa, request, reply, oMsgList, m_Database->DatabaseName() );
        }
      }
    }
  }

  return SAPDB_TRUE;
} 

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_DBMWeb :: configDevspaces
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMWeb_DBMWeb :: configDevspaces ( sapdbwa_WebAgent    & wa,
                                              sapdbwa_HttpRequest & request,
                                              sapdbwa_HttpReply   & reply )
{
  DBMCli_String sAction;
  DBMCli_String sClass(PARAM_CLASS_VAL_DATA);
  DBMCli_String sBack;

  GetParameterValue(PARAM_ACTION, request, sAction);
  GetParameterValue(PARAM_CLASS, request, sClass);
  GetParameterValue(PARAM_BACK, request, sBack);

  Msg_List oMsgList;
  DBMCli_Devspaces & oDevspaces = m_Database->GetDevspaces( );

  if( sAction == PARAM_ACTION_VAL_REFRESH ) {

    if( oDevspaces.Refresh( oMsgList ) ) {

      DBMWeb_TemplateDevspaces oTemplate( wa, sClass, oDevspaces, oDevspaces.ClassFromClassName( sClass ) );

      oTemplate.writePage( Tools_TemplateWriterWA( reply ) );

    } else {
      sendMsgListError( wa, request, reply, oMsgList, m_Database->DatabaseName() );
    }

  } else if( sAction == PARAM_ACTION_VAL_VIEW ) {

    DBMWeb_TemplateDevspaces oTemplate( wa, sClass, oDevspaces, oDevspaces.ClassFromClassName( sClass ) );

    oTemplate.writePage( Tools_TemplateWriterWA( reply ) );

  } else if( sAction == PARAM_ACTION_VAL_ADD ) {

    DBMCli_Devspace & oDevspace = oDevspaces.NewDevspace( sClass ); 

    DBMWeb_TemplateDevspace oTemplate( wa, oDevspace, DBMWEB_TEMPLDEVSPACE_NEW, sBack );

    oTemplate.writePage( Tools_TemplateWriterWA( reply ) );
  }

  return SAPDB_TRUE;
} 

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_DBMWeb :: configDevspace
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMWeb_DBMWeb :: configDevspace ( sapdbwa_WebAgent    & wa,
                                             sapdbwa_HttpRequest & request,
                                             sapdbwa_HttpReply   & reply )
{
  DBMCli_String sAction;
  DBMCli_String sName;
  DBMCli_String sBack;

  GetParameterValue(PARAM_ACTION, request, sAction);
  GetParameterValue(PARAM_NAME, request, sName);
  GetParameterValue(PARAM_BACK, request, sBack);

  Msg_List oMsgList;
  DBMCli_Devspaces & oDevspaces = m_Database->GetDevspaces( );
  DBMCli_DevspaceArray & aDevspaces = oDevspaces.DevspaceArray( );

  if( sAction == PARAM_ACTION_VAL_EDIT ) {
    int nDevspace = oDevspaces.IndexByName( sName );
    if( nDevspace >= 0 ) {

      DBMCli_Devspace & oDevspace = aDevspaces[nDevspace];

      DBMWeb_TemplateDevspace oTemplate( wa, oDevspace, DBMWEB_TEMPLDEVSPACE_EDIT, sBack );

      oTemplate.writePage( Tools_TemplateWriterWA( reply ) );
    }

  } else if( sAction == PARAM_ACTION_VAL_INIT ) {
    int nDevspace = oDevspaces.IndexByName( sName );
    if( nDevspace >= 0 ) {

      DBMCli_Devspace & oDevspace = aDevspaces[nDevspace];

      oDevspaces.InitDevspace( oDevspace );

      DBMWeb_TemplateDevspace oTemplate( wa, oDevspace, DBMWEB_TEMPLDEVSPACE_INIT, sBack );

      oTemplate.writePage( Tools_TemplateWriterWA( reply ) );
    }

  } else if( sAction == PARAM_ACTION_VAL_ADD ) {
    DBMCli_String sClass;
    DBMCli_String sLocation;
    DBMCli_String sType;
    DBMCli_String sSize;
    DBMCli_String sMirroredLocation;
    DBMCli_String sMirroredType;

    GetParameterValue(PARAM_CLASS, request, sClass);
    GetParameterValue(PARAM_LOCATION, request, sLocation);
    GetParameterValue(PARAM_TYPE, request, sType);
    GetParameterValue(PARAM_SIZE, request, sSize);
    GetParameterValue(PARAM_MLOCATION, request, sMirroredLocation);
    GetParameterValue(PARAM_MTYPE, request, sMirroredType);

    if( oDevspaces.Add( sClass, sLocation, sType, sSize, sMirroredLocation, sMirroredType, oMsgList ) ) {
    
      if( !sBack.IsEmpty( ) ) {
        MovetoURL( sBack, reply );
      } else {
        if( oDevspaces.Refresh( oMsgList ) ) {
          DBMWeb_TemplateDevspaces oTemplate( wa, sClass, oDevspaces, oDevspaces.ClassFromClassName( sClass ) );
          oTemplate.writePage( Tools_TemplateWriterWA( reply ) );
        } else {
          sendMsgListError( wa, request, reply, oMsgList, m_Database->DatabaseName() );
        }
      }

    } else {
      sendMsgListError( wa, request, reply, oMsgList, m_Database->DatabaseName() );
    }

  } else if( sAction == PARAM_ACTION_VAL_PUT ) {
    int nDevspace = oDevspaces.IndexByName( sName );
    if( nDevspace >= 0 ) {
      DBMCli_Devspace & oDevspace = aDevspaces[nDevspace];

      DBMCli_String sClass;
      DBMCli_String sLocation;
      DBMCli_String sType;
      DBMCli_String sSize;
      SAPDB_Int4    nPages;
      SAPDB_Bool    bMirrored = SAPDB_FALSE;
      DBMCli_String sMirroredLocation;
      DBMCli_String sMirroredType;

      GetParameterValue(PARAM_CLASS, request, sClass);
      GetParameterValue(PARAM_LOCATION, request, sLocation);
      GetParameterValue(PARAM_TYPE, request, sType);
      GetParameterValue(PARAM_SIZE, request, sSize);

      if( oDevspace.Mirrored( ) ) {
        bMirrored = SAPDB_TRUE;
        GetParameterValue(PARAM_MLOCATION, request, sMirroredLocation);
        GetParameterValue(PARAM_MTYPE, request, sMirroredType);
      }

      nPages = atol( sSize );

      oDevspace.SetProperties( oDevspaces.ClassFromClassName( sClass ),
                               oDevspace.Number( ),
                               nPages,
                               sLocation,
                               oDevspaces.TypeFromTypeName( sType ),
                               bMirrored,
                               sMirroredLocation,
                               oDevspaces.TypeFromTypeName( sMirroredType ) );

      if( oDevspace.Complete( ) ) {
        if( oDevspace.Put( oMsgList ) ) {
    
          if( oDevspaces.Refresh( oMsgList ) ) {

            if( !sBack.IsEmpty( ) ) {
              MovetoURL( sBack, reply );
            } else {
              DBMWeb_TemplateDevspaces oTemplate( wa, sClass, oDevspaces, oDevspaces.ClassFromClassName( sClass ) );
              oTemplate.writePage( Tools_TemplateWriterWA( reply ) );
            }
          } else {
            sendMsgListError( wa, request, reply, oMsgList, m_Database->DatabaseName() );
          }

        } else {
          sendMsgListError( wa, request, reply, oMsgList, m_Database->DatabaseName() );
        }
      } else {
        DBMWeb_TemplateDevspace oTemplate( wa, oDevspace, DBMWEB_TEMPLDEVSPACE_INIT, sBack, DEV_ERR_NOTCOMPLETE );
        oTemplate.writePage( Tools_TemplateWriterWA( reply ) );
      }
    }
  }

  return SAPDB_TRUE;
} 

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_DBMWeb :: configMedia
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMWeb_DBMWeb :: configMedia ( sapdbwa_WebAgent    & wa,
                                          sapdbwa_HttpRequest & request,
                                          sapdbwa_HttpReply   & reply )
{
  DBMCli_String sAction;
  DBMCli_String sType(PARAM_TYPE_VAL_SINGLE);

  GetParameterValue(PARAM_ACTION, request, sAction);
  GetParameterValue(PARAM_TYPE, request, sType);

  Msg_List oMsgList;
  DBMCli_Media & oMedia = m_Database->GetMedia( );

  if( sAction == PARAM_ACTION_VAL_REFRESH ) {

    if( oMedia.Refresh( oMsgList ) ) {

      if( sType == PARAM_TYPE_VAL_SINGLE ) {
        DBMWeb_TemplateMedia oTemplate( wa, oMedia, DBMWEB_TEMPLMEDIA_SINGLE );
        oTemplate.writePage( Tools_TemplateWriterWA( reply ) );
      } else {
        DBMWeb_TemplateMedia oTemplate( wa, oMedia, DBMWEB_TEMPLMEDIA_PARALLEL );
        oTemplate.writePage( Tools_TemplateWriterWA( reply ) );
      }

    } else {
      sendMsgListError( wa, request, reply, oMsgList, m_Database->DatabaseName() );
    }

  } else if( sAction == PARAM_ACTION_VAL_VIEW ) {

    if( sType == PARAM_TYPE_VAL_SINGLE ) {
      DBMWeb_TemplateMedia oTemplate( wa, oMedia, DBMWEB_TEMPLMEDIA_SINGLE );
      oTemplate.writePage( Tools_TemplateWriterWA( reply ) );
    } else {
      DBMWeb_TemplateMedia oTemplate( wa, oMedia, DBMWEB_TEMPLMEDIA_PARALLEL );
      oTemplate.writePage( Tools_TemplateWriterWA( reply ) );
    }

  } else if( sAction == PARAM_ACTION_VAL_ADD ) {

    DBMCli_String sBack;
    DBMCli_String sBackupType;

    GetParameterValue(PARAM_BACK, request, sBack);
    GetParameterValue(PARAM_BACKUPTYPE, request, sBackupType);

    if( sType == PARAM_TYPE_VAL_SINGLE ) {
      DBMCli_Medium & oMedium = oMedia.NewMedium( "" ); 

      oMedium.SetProperties(oMedium.Group(),
                            oMedium.Name(),
                            oMedium.Location(),
                            oMedium.DeviceType(),
                            sBackupType,
                            oMedium.Pages(),
                            oMedium.BlockSize(),
                            oMedium.Overwrite(),
                            oMedium.Autoloader(),
                            oMedium.OSCommand(),
                            oMedium.Extern());

      DBMWeb_TemplateMedium oTemplate( wa, oMedium, DBMWEB_TEMPLMEDIUM_NEW, sBack );
      oTemplate.writePage( Tools_TemplateWriterWA( reply ) );
    } else {
      DBMCli_MediumParallel & oMediumParallel = oMedia.NewMediumParallel( "" ); 

      oMediumParallel.SetProperties(oMediumParallel.Name(),
                                    oMediumParallel.DeviceType(),
                                    sBackupType,
                                    oMediumParallel.BlockSize(),
                                    oMediumParallel.Overwrite(),
                                    oMediumParallel.Autoloader(),
                                    oMediumParallel.Extern());

      DBMWeb_TemplateParmed oTemplate( wa, oMediumParallel, DBMWEB_TEMPLPARMED_NEW, sBack );
      oTemplate.writePage( Tools_TemplateWriterWA( reply ) );
    }
  }

  return SAPDB_TRUE;
} 

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_DBMWeb :: configMedium
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMWeb_DBMWeb :: configMedium ( sapdbwa_WebAgent    & wa,
                                           sapdbwa_HttpRequest & request,
                                           sapdbwa_HttpReply   & reply )
{
  DBMCli_String sAction;
  DBMCli_String sType;
  DBMCli_String sBack;

  DBMCli_String sName;
  DBMCli_String sGroup;
  DBMCli_String sLocation;
  DBMCli_String sDeviceType;
  DBMCli_String sBackupType;
  DBMCli_String sSize;
  DBMCli_String sBlockSize;
  DBMCli_String sOverwrite;
  DBMCli_String sAutoloader;
  DBMCli_String sOSCommand;

  GetParameterValue(PARAM_ACTION, request, sAction);
  GetParameterValue(PARAM_TYPE, request, sType);
  GetParameterValue(PARAM_BACK, request, sBack);

  Msg_List oMsgList;

  DBMCli_Media & oMedia= m_Database->GetMedia( );
  DBMCli_MediumArray & aMedium = oMedia.MediumArray( );
  DBMCli_MediumParallelArray & aMediumParallel = oMedia.MediumParallelArray( );

  SAPDB_Bool bMedia = SAPDB_FALSE;

  if( aMedium.GetSize( ) == 0 ) {
    if( oMedia.Refresh( oMsgList ) ) {
      bMedia = SAPDB_TRUE;
    } else {
      sendMsgListError( wa, request, reply, oMsgList, m_Database->DatabaseName() );
    }
  } else {
    bMedia = SAPDB_TRUE;
  }

  if( bMedia ) {

    oMsgList.ClearMessageList();

    // edit medium

    if( sAction == PARAM_ACTION_VAL_EDIT ) {

      // edit single medium
      if( sType == PARAM_TYPE_VAL_SINGLE ) {
        GetParameterValue(PARAM_NAME, request, sName);

        int nMedium = oMedia.IndexByName( sName );
        if( nMedium >= 0 ) {

          DBMCli_Medium & oMedium = aMedium[nMedium];

          DBMWeb_TemplateMedium oTemplate( wa, oMedium, DBMWEB_TEMPLMEDIUM_EDIT, sBack );

          oTemplate.writePage( Tools_TemplateWriterWA( reply ) );
        }

      // edit parallel medium
      } else if( sType == PARAM_TYPE_VAL_PARALLEL ) {
        GetParameterValue(PARAM_NAME, request, sName);

        int nMediumParallel = oMedia.IndexByNameParallel( sName );
        if( nMediumParallel >= 0 ) {

          DBMCli_MediumParallel & oMediumParallel = aMediumParallel[nMediumParallel];

          DBMWeb_TemplateParmed oTemplate( wa, oMediumParallel, DBMWEB_TEMPLPARMED_EDIT, sBack );

          oTemplate.writePage( Tools_TemplateWriterWA( reply ) );
        }

      }

    // put medium

    } else if( sAction == PARAM_ACTION_VAL_PUT ) {

      // put single medium
      if( sType == PARAM_TYPE_VAL_SINGLE ) {
        GetParameterValue(PARAM_NAME, request, sName);
        GetParameterValue(PARAM_LOCATION, request, sLocation);
        GetParameterValue(PARAM_DEVICETYPE, request, sDeviceType);
        GetParameterValue(PARAM_BACKUPTYPE, request, sBackupType);
        GetParameterValue(PARAM_SIZE, request, sSize);
        GetParameterValue(PARAM_BLOCKSIZE, request, sBlockSize);
        GetParameterValue(PARAM_OVERWRITE, request, sOverwrite);
        GetParameterValue(PARAM_AUTOLOADER, request, sAutoloader);
        GetParameterValue(PARAM_OSCOMMAND, request, sOSCommand);

        SAPDB_Int nMedium = oMedia.IndexByName( sName );

        DBMCli_Medium & oMedium = oMedia.NewMedium( sName );

        if( nMedium >= 0 ) {
          oMedium = aMedium[nMedium];
        }

        if( oMedium.Put( sLocation, sDeviceType, sBackupType, 
                         sSize, sBlockSize, sOverwrite, sAutoloader, sOSCommand, 
                         "" /*sExtern*/, oMsgList ) ) {
          if( oMedia.Refresh( oMsgList ) ) {
            if( !sBack.IsEmpty( ) ) {
              MovetoURL( sBack, reply );
            } else {
              DBMWeb_TemplateMedia oTemplate( wa, oMedia, DBMWEB_TEMPLMEDIA_SINGLE );
              oTemplate.writePage( Tools_TemplateWriterWA( reply ) );
            }
          } else {
           sendMsgListError( wa, request, reply, oMsgList, m_Database->DatabaseName() );
          }
        } else {
         sendMsgListError( wa, request, reply, oMsgList, m_Database->DatabaseName() );
        }

      // put parallel medium
      } else if( sType == PARAM_TYPE_VAL_PARALLEL ) {
        GetParameterValue(PARAM_GROUP, request, sGroup);
        GetParameterValue(PARAM_DEVICETYPE, request, sDeviceType);
        GetParameterValue(PARAM_BACKUPTYPE, request, sBackupType);
        GetParameterValue(PARAM_BLOCKSIZE, request, sBlockSize);
        GetParameterValue(PARAM_OVERWRITE, request, sOverwrite);
        GetParameterValue(PARAM_AUTOLOADER, request, sAutoloader);

        SAPDB_Bool bOK = SAPDB_TRUE;
        SAPDB_Bool bNew = SAPDB_TRUE;

        SAPDB_Int nMediumParallel = oMedia.IndexByNameParallel( sGroup );

        DBMCli_MediumParallel & oMediumParallel = oMedia.NewMediumParallel( sGroup );

        if( nMediumParallel >= 0 ) {
          oMediumParallel = aMediumParallel[nMediumParallel];
          bNew = SAPDB_FALSE;
        }

        DBMCli_MediumArray & aMedium = oMediumParallel.MediumArray( );

        for( SAPDB_Int nMedium = 0; nMedium < aMedium.GetSize( ); nMedium++ )
        {
          GetParameterValueByIndex(PARAM_NAME, nMedium, request, sName);
          GetParameterValueByIndex(PARAM_LOCATION, nMedium , request, sLocation);
          GetParameterValueByIndex(PARAM_SIZE, nMedium , request, sSize);
          GetParameterValueByIndex(PARAM_OSCOMMAND, nMedium , request, sOSCommand);

          if( ( !bNew ) && ( aMedium[nMedium].Name( ) != sName ) ) {
            if( !aMedium[nMedium].Delete( oMsgList ) ) {
              bOK = SAPDB_FALSE;
              break;
            }
          }

          if( bOK ) {
            aMedium[nMedium].SetProperties( sGroup, sName );

            if( !sGroup.IsEmpty( ) && !sName.IsEmpty( ) ) {
              if( !aMedium[nMedium].Put( sLocation, sDeviceType, sBackupType, 
                                         sSize, sBlockSize, sOverwrite, sAutoloader, sOSCommand, 
                                         "" /*sExtern*/, oMsgList ) ) {
                bOK = SAPDB_FALSE;
                break;
              }
            }
          }
        }

        if( bOK ) {
          if( oMedia.Refresh( oMsgList ) ) {
            if( !sBack.IsEmpty( ) ) {
              MovetoURL( sBack, reply );
            } else {
              DBMWeb_TemplateMedia oTemplate( wa, oMedia, DBMWEB_TEMPLMEDIA_SINGLE );
              oTemplate.writePage( Tools_TemplateWriterWA( reply ) );
            }
          } else {
           sendMsgListError( wa, request, reply, oMsgList, m_Database->DatabaseName() );
          }
        } else {
         sendMsgListError( wa, request, reply, oMsgList, m_Database->DatabaseName() );
        }
      }

    // add medium

    } else if( sAction == PARAM_ACTION_VAL_ADD ) {

      // add parallel medium
      if( sType == PARAM_TYPE_VAL_SINGLE ) {
        GetParameterValue(PARAM_NAME, request, sName);
        GetParameterValue(PARAM_LOCATION, request, sLocation);
        GetParameterValue(PARAM_DEVICETYPE, request, sDeviceType);
        GetParameterValue(PARAM_BACKUPTYPE, request, sBackupType);
        GetParameterValue(PARAM_SIZE, request, sSize);
        GetParameterValue(PARAM_BLOCKSIZE, request, sBlockSize);
        GetParameterValue(PARAM_OVERWRITE, request, sOverwrite);
        GetParameterValue(PARAM_AUTOLOADER, request, sAutoloader);
        GetParameterValue(PARAM_OSCOMMAND, request, sOSCommand);

        DBMCli_Medium & oMedium = oMedia.NewMedium( sName );

        if( oMedium.Put( sLocation, sDeviceType, sBackupType, 
                         sSize, sBlockSize, sOverwrite, sAutoloader, sOSCommand, 
                         "" /*sExtern*/, oMsgList ) ) {

          if( oMedia.Refresh( oMsgList ) ) {
            if( !sBack.IsEmpty( ) ) {
              MovetoURL( sBack, reply );
            } else {
              DBMWeb_TemplateMedia oTemplate( wa, oMedia, DBMWEB_TEMPLMEDIA_SINGLE );
              oTemplate.writePage( Tools_TemplateWriterWA( reply ) );
            }
          } else {
           sendMsgListError( wa, request, reply, oMsgList, m_Database->DatabaseName() );
          }

        } else {
         sendMsgListError( wa, request, reply, oMsgList, m_Database->DatabaseName() );
        }

      // add parallel medium
      } else if( sType == PARAM_TYPE_VAL_PARALLEL ) {
        GetParameterValue(PARAM_GROUP, request, sGroup);
        GetParameterValue(PARAM_DEVICETYPE, request, sDeviceType);
        GetParameterValue(PARAM_BACKUPTYPE, request, sBackupType);
        GetParameterValue(PARAM_BLOCKSIZE, request, sBlockSize);
        GetParameterValue(PARAM_OVERWRITE, request, sOverwrite);
        GetParameterValue(PARAM_AUTOLOADER, request, sAutoloader);

        DBMCli_MediumParallel & oMediumParallel = oMedia.NewMediumParallel( sGroup );
        DBMCli_MediumArray & aMedium = oMediumParallel.MediumArray( );

        SAPDB_Bool bError = SAPDB_FALSE;

        for( SAPDB_Int nMedium = 0; nMedium < aMedium.GetSize( ); nMedium++ )
        {
          GetParameterValueByIndex(PARAM_NAME, nMedium, request, sName);
          GetParameterValueByIndex(PARAM_LOCATION, nMedium , request, sLocation);
          GetParameterValueByIndex(PARAM_SIZE, nMedium , request, sSize);
          GetParameterValueByIndex(PARAM_OSCOMMAND, nMedium , request, sOSCommand);

          aMedium[nMedium].SetProperties( sGroup, sName );

          if( !sGroup.IsEmpty( ) && !sName.IsEmpty( ) ) {
            if( !aMedium[nMedium].Put( sLocation, sDeviceType, sBackupType, 
                                       sSize, sBlockSize, sOverwrite, sAutoloader, sOSCommand, 
                                       "" /*sExtern*/, oMsgList ) ) {
              bError = SAPDB_TRUE;
              break;
            }
          }
        }

        if( !bError ) {
          if( oMedia.Refresh( oMsgList ) ) {
            if( !sBack.IsEmpty( ) ) {
              MovetoURL( sBack, reply );
            } else {
              DBMWeb_TemplateMedia oTemplate( wa, oMedia, DBMWEB_TEMPLMEDIA_SINGLE );
              oTemplate.writePage( Tools_TemplateWriterWA( reply ) );
            }
          } else {
           sendMsgListError( wa, request, reply, oMsgList, m_Database->DatabaseName() );
          }
        } else {
         sendMsgListError( wa, request, reply, oMsgList, m_Database->DatabaseName() );
        }
      }

    // delete medium

    } else if( sAction == PARAM_ACTION_VAL_DELETE ) {

      // delete single medium
      if( sType == PARAM_TYPE_VAL_SINGLE ) {
        GetParameterValue(PARAM_NAME, request, sName);

        int nMedium = oMedia.IndexByName( sName );

        if( nMedium >= 0 ) {

          DBMCli_Medium & oMedium = aMedium[nMedium];

          if( oMedium.Delete( oMsgList ) ) {
            if( oMedia.Refresh( oMsgList ) ) {
              if( !sBack.IsEmpty( ) ) {
                MovetoURL( sBack, reply );
              } else {
                DBMWeb_TemplateMedia oTemplate( wa, oMedia, DBMWEB_TEMPLMEDIA_SINGLE );
                oTemplate.writePage( Tools_TemplateWriterWA( reply ) );
              }
            } else {
              sendMsgListError( wa, request, reply, oMsgList, m_Database->DatabaseName() );
            }
          } else {
            sendMsgListError( wa, request, reply, oMsgList, m_Database->DatabaseName() );
          }
        }

      // delete parallel medium
      } else if( sType == PARAM_TYPE_VAL_PARALLEL ) {
        GetParameterValue(PARAM_GROUP, request, sGroup);

        int nMediumParallel = oMedia.IndexByNameParallel( sGroup );

        if( nMediumParallel >= 0 ) {
          DBMCli_MediumParallel & oMediumParallel = aMediumParallel[nMediumParallel];
          DBMCli_MediumArray & aMedium = oMediumParallel.MediumArray( );

          SAPDB_Bool bError = SAPDB_FALSE;

          for( SAPDB_Int nMedium = 0; nMedium < aMedium.GetSize( ); nMedium++ ) {
            if( !aMedium[nMedium].Name( ).IsEmpty( ) ) {
              if( !aMedium[nMedium].Delete( oMsgList ) ) {
                bError = SAPDB_TRUE;
                break;
              }
            }
          }

          if( !bError ) {
            if( oMedia.Refresh( oMsgList ) ) {
              if( !sBack.IsEmpty( ) ) {
                MovetoURL( sBack, reply );
              } else {
                DBMWeb_TemplateMedia oTemplate( wa, oMedia, DBMWEB_TEMPLMEDIA_PARALLEL );
                oTemplate.writePage( Tools_TemplateWriterWA( reply ) );
              }
            } else {
             sendMsgListError( wa, request, reply, oMsgList, m_Database->DatabaseName() );
            }
          } else {
           sendMsgListError( wa, request, reply, oMsgList, m_Database->DatabaseName() );
          }
        }
      }
    }
  }

  return SAPDB_TRUE;
} 

SAPDB_Bool DBMWeb_DBMWeb :: configSysTabs( sapdbwa_WebAgent    & wa,
                                           sapdbwa_HttpRequest & request,
                                           sapdbwa_HttpReply   & reply )
{
  DBMCli_String sAction;

  GetParameterValue(PARAM_ACTION, request, sAction);

  Msg_List oMsgList;

  if( sAction == PARAM_ACTION_VAL_REFRESH ) {

    DBMWeb_TemplateSysTabs oTemplate( wa, "", "", "", DBMWEB_TEMPLSYSTABS_MODE_START );
    oTemplate.writePage( Tools_TemplateWriterWA( reply ) );

  } else if( sAction == PARAM_ACTION_VAL_WAIT ) {

    DBMCli_String sDBAName;
    DBMCli_String sDBAPwd;
    DBMCli_String sDOMPwd;

    GetParameterValue(PARAM_DBANAME, request, sDBAName);
    GetParameterValue(PARAM_DBAPWD, request, sDBAPwd);
    GetParameterValue(PARAM_DOMPWD, request, sDOMPwd);

    DBMWeb_TemplateSysTabs oTemplate( wa, sDBAName, sDBAPwd, sDOMPwd, DBMWEB_TEMPLSYSTABS_MODE_WAIT );
    oTemplate.writePage( Tools_TemplateWriterWA( reply ) );

  } else if ( sAction == PARAM_ACTION_VAL_START ) {

    DBMCli_String sDBAName;
    DBMCli_String sDBAPwd;
    DBMCli_String sDOMPwd;

    GetParameterValue(PARAM_DBANAME, request, sDBAName);
    GetParameterValue(PARAM_DBAPWD, request, sDBAPwd);
    GetParameterValue(PARAM_DOMPWD, request, sDOMPwd);

    if( m_Database->LoadSysTab( sDBAName, sDBAPwd, sDOMPwd, oMsgList ) ) {

      DBMWeb_TemplateSysTabs oTemplate( wa, "", "", "", DBMWEB_TEMPLSYSTABS_MODE_RESULT );
      oTemplate.writePage( Tools_TemplateWriterWA( reply ) );

    } else {
      sendMsgListError( wa, request, reply, oMsgList, m_Database->DatabaseName() );
    }
  }

  return SAPDB_TRUE;
}

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_DBMWeb :: configLogMode
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMWeb_DBMWeb :: configLogMode( sapdbwa_WebAgent    & wa,
                                           sapdbwa_HttpRequest & request,
                                           sapdbwa_HttpReply   & reply )
{
  DBMCli_String        sAction;
  DBMCli_String        sLogmode;
  DBMCli_String        sRestart;
  Msg_List oMsgList;

  GetParameterValue(PARAM_ACTION,  request, sAction);
  GetParameterValue(LOGMODE_PARAM_RESTART, request, sRestart);
  GetParameterValue(LOGMODE_PARAM_NEWMODE, request, sLogmode);

  DBMCli_LogModeValue oNewMode(sLogmode);
  SAPDB_Bool          bRestart = (sRestart == LOGMODE_VAL_RESTART);

  if( sAction == LOGMODE_ACTION_REFRESH ) {

    if (m_Database->GetLogModeObj().Refresh(oMsgList)) {
      DBMWeb_TemplateLogMode oTemplate( wa, DBMWeb_TemplateLogMode::View, DBMCli_LogModeValue::Unknown, false, m_Database );
      oTemplate.writePage( Tools_TemplateWriterWA( reply ) );
    } else {
      sendMsgListError( wa, request, reply, oMsgList, m_Database->DatabaseName() );
    } // end if

  } else if( sAction == LOGMODE_ACTION_SELECT ) {
    bRestart = true;
    DBMWeb_TemplateLogMode oTemplate( wa, DBMWeb_TemplateLogMode::View, oNewMode.GetValue(), bRestart, m_Database );
    oTemplate.writePage( Tools_TemplateWriterWA( reply ) );

  } else if( sAction == LOGMODE_ACTION_EDIT ) {
    DBMCli_String sDevspace;

    GetParameterValue(LOGMODE_PARAM_DEVSPACE, request, sDevspace);

    DBMWeb_TemplateLogMode oTemplate( wa, DBMWeb_TemplateLogMode::Edit, oNewMode.GetValue(), bRestart, m_Database );
    oTemplate.SetCurrentDevspace(sDevspace)
             .writePage( Tools_TemplateWriterWA( reply ) );

  } else if( sAction == LOGMODE_ACTION_SAVEDEV ) {
    DBMCli_String sDevspace;
    DBMCli_String sLocation;
    DBMCli_String sType;

    GetParameterValue(LOGMODE_PARAM_DEVSPACE,  request, sDevspace);
    GetParameterValue(LOGMODE_PARAM_MLOCATION, request, sLocation);
    GetParameterValue(LOGMODE_PARAM_MTYPE,     request, sType);

    SAPDB_Int nDevspace = m_Database->GetDevspaces().IndexByName(sDevspace);
    m_Database->GetDevspaces().DevspaceArray()[nDevspace].PrepareMirroredProperties(sLocation, sType);

    DBMWeb_TemplateLogMode oTemplate( wa, DBMWeb_TemplateLogMode::View, oNewMode.GetValue(), bRestart, m_Database );
    oTemplate.writePage( Tools_TemplateWriterWA( reply ) );
  
  } else if( sAction == LOGMODE_ACTION_CANCELDEV ) {
    DBMWeb_TemplateLogMode oTemplate( wa, DBMWeb_TemplateLogMode::View, oNewMode.GetValue(), bRestart, m_Database );
    oTemplate.writePage( Tools_TemplateWriterWA( reply ) );

  } else if( sAction == LOGMODE_ACTION_CHANGE ) {

    DBMCli_String sErrTitle  = "Error - " + m_Database->DatabaseName();
    DBMCli_String sErrAction = "javascript:parent.GotoWebDBMURL(this, \\'Event=CONFIG_LOGMODE";
    sErrAction = sErrAction + "&" + PARAM_ACTION   + "=" + PARAM_ACTION_VAL_REFRESH + "\\')";

    if (m_Database->GetLogModeObj().Change(oNewMode.GetValue(), bRestart, oMsgList)) {
      if (m_Database->GetLogModeObj().Refresh(oMsgList)) {
        DBMWeb_TemplateLogMode oTemplate( wa, DBMWeb_TemplateLogMode::View, DBMCli_LogModeValue::Unknown, false, m_Database );
        oTemplate.SetRefreshState(bRestart)
                 .writePage( Tools_TemplateWriterWA( reply ) );
      } else {
        // show error
        DBMWeb_TemplateMsgBox oMsgBox(wa, DBMWEB_TEMPLMSGBOX_ERROR, oMsgList, sErrTitle);
        oMsgBox.SetButtonAction(sErrAction).writePage(Tools_TemplateWriterWA(reply));
      } // end if
    } else {
      // show error
      DBMWeb_TemplateMsgBox oMsgBox(wa, DBMWEB_TEMPLMSGBOX_ERROR, oMsgList, sErrTitle);
      oMsgBox.SetButtonAction(sErrAction).writePage(Tools_TemplateWriterWA(reply));
    } // end if

  } else {
    DBMWeb_TemplateMsgBox oMsgBox(wa, DBMWEB_TEMPLMSGBOX_INFO, "", "Web DBM service error (unknown action)!");
    oMsgBox.SetButtonAction("WARoot/HTML/DBMEmpty.htm")
           .writePage(Tools_TemplateWriterWA(reply));
  } // end if

  return SAPDB_TRUE;
} // end DBMWeb_DBMWeb :: configLogMode

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_DBMWeb :: configUsers
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMWeb_DBMWeb :: configUsers( sapdbwa_WebAgent    & wa,
                                         sapdbwa_HttpRequest & request,
                                         sapdbwa_HttpReply   & reply )
{
  DBMCli_String        sAction;

  GetParameterValue(PARAM_ACTION, request, sAction);

  Msg_List oMsgList;
  DBMCli_Users & oUsers = m_Database->GetUsers( );

  if( sAction == PARAM_ACTION_VAL_REFRESH ) {

    if( m_Database->GetUsers().Refresh( oMsgList ) ) {
      DBMWeb_TemplateUsers oTemplate( wa, oUsers );
      oTemplate.writePage( Tools_TemplateWriterWA( reply ) );
    } else {
      sendMsgListError( wa, request, reply, oMsgList, m_Database->DatabaseName() );
    } 

  } else if( sAction == PARAM_ACTION_VAL_ADD ) {

    DBMCli_String sBack;
    GetParameterValue(PARAM_BACK, request, sBack);

    DBMCli_User & oUser = oUsers.NewUser( "" ); 
    DBMWeb_TemplateUser oTemplate( wa, oUser, DBMWEB_TEMPLUSER_NEW, sBack );
    oTemplate.writePage( Tools_TemplateWriterWA( reply ) );

  } else {
    DBMWeb_TemplateMsgBox oMsgBox(wa, DBMWEB_TEMPLMSGBOX_INFO, "", "Web DBM service error (unknown action)!");
    oMsgBox.SetButtonAction("WARoot/HTML/DBMEmpty.htm")
           .writePage(Tools_TemplateWriterWA(reply));
  } 

  return SAPDB_TRUE;
} 

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_DBMWeb :: configUser
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMWeb_DBMWeb :: configUser( sapdbwa_WebAgent    & wa,
                                        sapdbwa_HttpRequest & request,
                                        sapdbwa_HttpReply   & reply )
{
  DBMCli_String sAction;
  DBMCli_String sBack;

  GetParameterValue(PARAM_ACTION, request, sAction);
  GetParameterValue(PARAM_BACK, request, sBack);

  Msg_List oMsgList;
  DBMCli_Users & oUsers = m_Database->GetUsers( );
  DBMCli_UserArray & aUser = oUsers.UserArray( );

  if( sAction == PARAM_ACTION_VAL_EDIT ) {
    DBMCli_String sName;

    GetParameterValue(PARAM_NAME, request, sName);

    int nUser = oUsers.IndexByName( sName );
    if( nUser >= 0 ) {
      DBMCli_User & oUser = aUser[nUser];
      DBMWeb_TemplateUser oTemplate( wa, oUser, DBMWEB_TEMPLUSER_EDIT, sBack );
      oTemplate.writePage( Tools_TemplateWriterWA( reply ) );
    }

  } else if( ( sAction == PARAM_ACTION_VAL_ADD ) ||
             ( sAction == PARAM_ACTION_VAL_PUT )    ) {
    
    SAPDB_Bool bOK = SAPDB_FALSE;

    DBMCli_String sName;
    DBMCli_String sPassword;
    DBMCli_String sDisabled;
    DBMCli_String sComment;
    DBMCli_String sPassword2;
    SAPDB_Bool    bPassword2 = SAPDB_FALSE;

    GetParameterValue(PARAM_NAME, request, sName);
    GetParameterValue(PARAM_PASSWORD, request, sPassword);
    GetParameterValue(PARAM_DISABLED, request, sDisabled);
    GetParameterValue(PARAM_COMMENT, request, sComment);
    GetParameterValue(PARAM_PASSWORD2, request, sPassword2);
    
    if( sDisabled.IsEmpty( ) ) {
      sDisabled = "NO";    
    }

    // serverrights
    SAPDB_Int nItem = 0;
    SAPDB_Int nRight = 0;
    DBMCli_String sRight;
    DBMCli_User & oUser = oUsers.NewUser( sName ); 

    for( nItem = 0; nItem  < oUser.ServerRightArray( ).GetSize( ); ++nItem ) {
      GetParameterValueByIndex( PARAM_SRVRIGHT, nRight, request, sRight );
      if( sRight == oUser.ServerRightArray( )[nItem].Name( ) ) {
        oUser.ServerRightArray( )[nItem].SetValue(SAPDB_TRUE);
        ++nRight;
      } else {
        oUser.ServerRightArray( )[nItem].SetValue(SAPDB_FALSE);
      } 
      sRight.Empty( );
    } 

    // second password handling
    if( sAction == PARAM_ACTION_VAL_PUT ) {
      int nUser = oUsers.IndexByName( sName );
      if( nUser >= 0 ) {
        DBMCli_User & oOldUser = aUser[nUser];
        if( oOldUser.SecondPwd( ) == "YES" ) {
          bPassword2 = SAPDB_FALSE;
        } else {
          if( !sPassword2.IsEmpty( ) ) {
            bPassword2 = SAPDB_TRUE;
          }
        }
      }
    }
   
    // add new nuser
    bOK = SAPDB_FALSE;
    if( sAction == PARAM_ACTION_VAL_ADD ) {
      if( oUsers.AddUser( sName, sPassword, oMsgList ) ) {
        bOK = SAPDB_TRUE;
      }
      if( !sPassword2.IsEmpty( ) ) {
        bPassword2 = SAPDB_TRUE;
      }
    } else {
      bOK = SAPDB_TRUE;
    }
    
    // put user properties
    if( bOK ) {
      if( oUser.Put( sDisabled, sComment, sPassword2, bPassword2, oMsgList ) ) {
        if( oUsers.Refresh( oMsgList ) ) {
          if( !sBack.IsEmpty( ) ) {
            MovetoURL( sBack, reply );
          } else {
            DBMWeb_TemplateUsers oTemplate( wa, oUsers );
            oTemplate.writePage( Tools_TemplateWriterWA( reply ) );
          }
        } else {
         sendMsgListError( wa, request, reply, oMsgList, m_Database->DatabaseName() );
        }
      } else {
       sendMsgListError( wa, request, reply, oMsgList, m_Database->DatabaseName() );
      }
    } else {
     sendMsgListError( wa, request, reply, oMsgList, m_Database->DatabaseName() );
    }

  } else if( sAction == PARAM_ACTION_VAL_DELETE ) {
    DBMCli_String sName;

    GetParameterValue(PARAM_NAME, request, sName);

    if( oUsers.DeleteUser( sName, oMsgList ) ) {
      if( !sBack.IsEmpty( ) ) {
        MovetoURL( sBack, reply );
      } else {
        DBMWeb_TemplateUsers oTemplate( wa, oUsers );
        oTemplate.writePage( Tools_TemplateWriterWA( reply ) );
      }
    } else {
     sendMsgListError( wa, request, reply, oMsgList, m_Database->DatabaseName() );
    }

  } else if( sAction == PARAM_ACTION_VAL_PWDCHANGE ) {

    DBMCli_String sName;
    GetParameterValue(PARAM_NAME, request, sName);
    int nUser = oUsers.IndexByName( sName );
    if( nUser >= 0 ) {
      DBMCli_User & oUser = aUser[nUser];
      DBMWeb_TemplateUser oTemplate( wa, oUser, DBMWEB_TEMPLUSER_CHANGEPWD, sBack );
      oTemplate.writePage( Tools_TemplateWriterWA( reply ) );
    }

  } else if( sAction == PARAM_ACTION_VAL_CHANGEPWD ) {

    DBMCli_String sName;
    DBMCli_String sPassword;

    GetParameterValue(PARAM_NAME, request, sName);
    GetParameterValue(PARAM_PASSWORD, request, sPassword);

    int nUser = oUsers.IndexByName( sName );
    if( nUser >= 0 ) {
      DBMCli_User & oUser = aUser[nUser];
      if( oUser.PutPassword( sPassword, oMsgList ) ) {
        if( oUsers.Refresh( oMsgList ) ) {
          if( !sBack.IsEmpty( ) ) {
            MovetoURL( sBack, reply );
          } else {
            DBMWeb_TemplateUsers oTemplate( wa, oUsers );
            oTemplate.writePage( Tools_TemplateWriterWA( reply ) );
          }
        } else {
         sendMsgListError( wa, request, reply, oMsgList, m_Database->DatabaseName() );
        }
      } else {
       sendMsgListError( wa, request, reply, oMsgList, m_Database->DatabaseName() );
      }
    }

  } else if( sAction == PARAM_ACTION_VAL_PWD2CHANGE ) {

    DBMCli_String sName;
    GetParameterValue(PARAM_NAME, request, sName);
    int nUser = oUsers.IndexByName( sName );
    if( nUser >= 0 ) {
      DBMCli_User & oUser = aUser[nUser];
      DBMWeb_TemplateUser oTemplate( wa, oUser, DBMWEB_TEMPLUSER_CHANGEPWD2, sBack );
      oTemplate.writePage( Tools_TemplateWriterWA( reply ) );
    }

  } else if( sAction == PARAM_ACTION_VAL_CHANGEPWD2 ) {

    DBMCli_String sName;
    DBMCli_String sPassword2;

    GetParameterValue(PARAM_NAME, request, sName);
    GetParameterValue(PARAM_PASSWORD2, request, sPassword2);

    int nUser = oUsers.IndexByName( sName );
    if( nUser >= 0 ) {
      DBMCli_User & oUser = aUser[nUser];
      if( oUser.PutSecondPwd( sPassword2, oMsgList ) ) {
        if( oUsers.Refresh( oMsgList ) ) {
          if( !sBack.IsEmpty( ) ) {
            MovetoURL( sBack, reply );
          } else {
            DBMWeb_TemplateUsers oTemplate( wa, oUsers );
            oTemplate.writePage( Tools_TemplateWriterWA( reply ) );
          }
        } else {
         sendMsgListError( wa, request, reply, oMsgList, m_Database->DatabaseName() );
        }
      } else {
       sendMsgListError( wa, request, reply, oMsgList, m_Database->DatabaseName() );
      }
    }

  } else {
    DBMWeb_TemplateMsgBox oMsgBox(wa, DBMWEB_TEMPLMSGBOX_INFO, "", "Web DBM service error (unknown action)!");
    oMsgBox.SetButtonAction("WARoot/HTML/DBMEmpty.htm")
           .writePage(Tools_TemplateWriterWA(reply));
  } 

  return SAPDB_TRUE;
} 
