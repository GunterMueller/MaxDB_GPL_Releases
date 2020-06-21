/*!
  -----------------------------------------------------------------------------
  module: DBMWeb_DBMWebBackup.hpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager WebAgent Extension
  description:  the "Backup" section of "the" DBMWeb Class
  remarks:      ATTENTION! This not a full qualified header file!
                It contains only some private member function for the
                class DBMWeb_DBMWeb and must only included within the
                class declaration in DBMWeb_DBMWeb.hpp.
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

#ifndef _DBMWEB_DBMWEBBACKUP_HPP_
#define _DBMWEB_DBMWEBBACKUP_HPP_

/*!
  -----------------------------------------------------------------------------
  Chapter: Backup Database
  -----------------------------------------------------------------------------
*/

/*!
  -----------------------------------------------------------------------------
  function:     backupDB
  -----------------------------------------------------------------------------

  description:  backup the database 
  
                This function provides all backup activities (complete, incremetal,
                log backup and automatic log backup) for the database

  arguments:    wa      [INOUT] - WebAgent object
                request [INOUT] - Request object of the WebAgent
                reply   [INOUT] - Reply object of the WebAgent

  returnvalue:  SAPDB_TRUE for "request accepted", otherwise SAPDB_FALSE
  -----------------------------------------------------------------------------
*/
SAPDB_Bool backupDB ( sapdbwa_WebAgent      &wa,
                      sapdbwa_HttpRequest   &request,
                      sapdbwa_HttpReply     &reply );

SAPDB_Bool backupDB_View       ( sapdbwa_WebAgent    & wa,
                                 sapdbwa_HttpRequest & request,
                                 sapdbwa_HttpReply   & reply,
                                 DBMCli_Backup       & oBackup,
                                 DBMCli_Media        & oMedia,
                                 DBMCli_BackupType   & oBackupType,
                                 DBMCli_MediumType   & oMediumType );

SAPDB_Bool backupDB_Select     ( sapdbwa_WebAgent    & wa,
                                 sapdbwa_HttpRequest & request,
                                 sapdbwa_HttpReply   & reply,
                                 DBMCli_Backup       & oBackup,
                                 DBMCli_Media        & oMedia,
                                 DBMCli_BackupType   & oBackupType,
                                 DBMCli_MediumType   & oMediumType );

SAPDB_Bool backupDB_Start      ( sapdbwa_WebAgent    & wa,
                                 sapdbwa_HttpRequest & request,
                                 sapdbwa_HttpReply   & reply,
                                 DBMCli_Backup       & oBackup,
                                 DBMCli_Media        & oMedia,
                                 DBMCli_BackupType   & oBackupType,
                                 DBMCli_MediumType   & oMediumType );

SAPDB_Bool backupDB_Replace    ( sapdbwa_WebAgent    & wa,
                                 sapdbwa_HttpRequest & request,
                                 sapdbwa_HttpReply   & reply,
                                 DBMCli_Backup       & oBackup,
                                 DBMCli_Media        & oMedia,
                                 DBMCli_BackupType   & oBackupType,
                                 DBMCli_MediumType   & oMediumType );

SAPDB_Bool backupDB_Ignore     ( sapdbwa_WebAgent    & wa,
                                 sapdbwa_HttpRequest & request,
                                 sapdbwa_HttpReply   & reply,
                                 DBMCli_Backup       & oBackup,
                                 DBMCli_Media        & oMedia,
                                 DBMCli_BackupType   & oBackupType,
                                 DBMCli_MediumType   & oMediumType );

SAPDB_Bool backupDB_Cancel     ( sapdbwa_WebAgent    & wa,
                                 sapdbwa_HttpRequest & request,
                                 sapdbwa_HttpReply   & reply,
                                 DBMCli_Backup       & oBackup,
                                 DBMCli_Media        & oMedia,
                                 DBMCli_BackupType   & oBackupType,
                                 DBMCli_MediumType   & oMediumType );

SAPDB_Bool backupDB_State      ( sapdbwa_WebAgent    & wa,
                                 sapdbwa_HttpRequest & request,
                                 sapdbwa_HttpReply   & reply,
                                 DBMCli_Backup       & oBackup,
                                 DBMCli_Media        & oMedia );

SAPDB_Bool backupDB_AutoOn     ( sapdbwa_WebAgent    & wa,
                                 sapdbwa_HttpRequest & request,
                                 sapdbwa_HttpReply   & reply,
                                 DBMCli_Backup       & oBackup,
                                 DBMCli_Media        & oMedia,
                                 DBMCli_BackupType   & oBackupType,
                                 DBMCli_MediumType   & oMediumType );

SAPDB_Bool backupDB_AutoOff    ( sapdbwa_WebAgent    & wa,
                                 sapdbwa_HttpRequest & request,
                                 sapdbwa_HttpReply   & reply,
                                 DBMCli_Backup       & oBackup,
                                 DBMCli_Media        & oMedia,
                                 DBMCli_BackupType   & oBackupType,
                                 DBMCli_MediumType   & oMediumType );

SAPDB_Bool backupDB_AutoCancel ( sapdbwa_WebAgent    & wa,
                                 sapdbwa_HttpRequest & request,
                                 sapdbwa_HttpReply   & reply,
                                 DBMCli_Backup       & oBackup,
                                 DBMCli_Media        & oMedia,
                                 DBMCli_BackupType   & oBackupType,
                                 DBMCli_MediumType   & oMediumType );

/*!
  -----------------------------------------------------------------------------
  EndChapter: Backup Database
  -----------------------------------------------------------------------------
*/

/*!
  -----------------------------------------------------------------------------
  Chapter: Recover Database
  -----------------------------------------------------------------------------
*/

/*!
  -----------------------------------------------------------------------------
  function:     recoverDB
  -----------------------------------------------------------------------------

  description:  recover the database 
  
                This function provides all recover activities to recover the
                database from a former created backup

  arguments:    wa      [INOUT] - WebAgent object
                request [INOUT] - Request object of the WebAgent
                reply   [INOUT] - Reply object of the WebAgent

  returnvalue:  SAPDB_TRUE for "request accepted", otherwise SAPDB_FALSE
  -----------------------------------------------------------------------------
*/
SAPDB_Bool recoverDB ( sapdbwa_WebAgent      &wa,
                       sapdbwa_HttpRequest   &request,
                       sapdbwa_HttpReply     &reply );

SAPDB_Bool recoverDB_Refresh         ( sapdbwa_WebAgent      &wa,
                                       sapdbwa_HttpRequest   &request,
                                       sapdbwa_HttpReply     &reply );

SAPDB_Bool recoverDB_SelectRecType   ( sapdbwa_WebAgent      &wa,
                                       sapdbwa_HttpRequest   &request,
                                       sapdbwa_HttpReply     &reply );

SAPDB_Bool recoverDB_SelectDataSave  ( sapdbwa_WebAgent      &wa,
                                       sapdbwa_HttpRequest   &request,
                                       sapdbwa_HttpReply     &reply );

SAPDB_Bool recoverDB_SelectMedium    ( sapdbwa_WebAgent      &wa,
                                       sapdbwa_HttpRequest   &request,
                                       sapdbwa_HttpReply     &reply );

SAPDB_Bool recoverDB_CommitList      ( sapdbwa_WebAgent      &wa,
                                       sapdbwa_HttpRequest   &request,
                                       sapdbwa_HttpReply     &reply );

SAPDB_Bool recoverDB_State           ( sapdbwa_WebAgent      &wa,
                                       sapdbwa_HttpRequest   &request,
                                       sapdbwa_HttpReply     &reply );

SAPDB_Bool recoverDB_ViewDialog      ( sapdbwa_WebAgent      & wa,
                                       sapdbwa_HttpRequest   & request,
                                       sapdbwa_HttpReply     & reply,
                                       const DBMCli_String   & sDialog,
                                       const DBMCli_String   & sRecInstMode,
                                       const DBMCli_String   & sRecType,
                                       const DBMCli_String   & sMedType,
                                       const DBMCli_DateTime & oUntil,
                                       const DBMCli_String   & sCheck);

/*!
  -----------------------------------------------------------------------------
  EndChapter: Recover Database
  -----------------------------------------------------------------------------
*/

#endif // _DBMWEB_DBMWEBBACKUP_HPP_
