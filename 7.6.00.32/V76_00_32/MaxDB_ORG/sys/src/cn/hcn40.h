/*! 
  -----------------------------------------------------------------------------
 
  module: hcn40.h
 
  -----------------------------------------------------------------------------
 
  responsible:  BerndV
 
  special area: DBMServer directory and system access
   
  description:  DBMServer directory and system access - Specification

  version:      7.2.* / 7.1.* / 6.2.9.* 

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


 
  -----------------------------------------------------------------------------
*/

#ifndef _HCN40_H_
#define _HCN40_H_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
 */
#include "gcn00.h"
#include "SAPDB/DBM/Srv/DBMSrv_Reply.hpp"

/*!
  -----------------------------------------------------------------------------
  function:     cn40DBMNext
  -----------------------------------------------------------------------------

  description:  ptoviede the next "next" command (if one available)

  arguments:    vcontrol    [INOUT]  - DBMServer session data
                command     [IN]     - DBMServer command
                replyData   [OUT]    - reply buffer
                replyLen    [OUT]    - lenght of reply
                replyLenMax [IN]     - max. length of reply buffer

  return value: error code specified in gcn00c.h
 
  -----------------------------------------------------------------------------
 */
tcn00_Error cn40DBMNext
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax);

/*!
  -----------------------------------------------------------------------------
  function:     cn40ExecLcInit
  -----------------------------------------------------------------------------

  description:  executes %DBROOT%\sap\lcinit.bat

  arguments:    vcontrol    [INOUT]  - DBMServer session data
                command     [IN]     - DBMServer command
                replyData   [OUT]    - reply buffer
                replyLen    [OUT]    - lenght of reply
                replyLenMax [IN]     - max. length of reply buffer

  return value: error code specified in gcn00c.h
 
  -----------------------------------------------------------------------------
 */
tcn00_Error cn40ExecLcInit
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax);

/*!
  -----------------------------------------------------------------------------
  function:     cn40ExecSDBInfo
  -----------------------------------------------------------------------------

  description:  executes sdbinfo

  arguments:    vcontrol    [INOUT]  - DBMServer session data
                command     [IN]     - DBMServer command
                replyData   [OUT]    - reply buffer
                replyLen    [OUT]    - lenght of reply
                replyLenMax [IN]     - max. length of reply buffer

  return value: error code specified in gcn00c.h
 
  -----------------------------------------------------------------------------
 */
tcn00_Error cn40ExecSDBInfo
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax);

/*!
  -----------------------------------------------------------------------------
  function:     cn40ExecSDBInst
  -----------------------------------------------------------------------------

  description:  executes sdbinst

  arguments:    vcontrol    [INOUT]  - DBMServer session data
                command     [IN]     - DBMServer command
                replyData   [OUT]    - reply buffer
                replyLen    [OUT]    - lenght of reply
                replyLenMax [IN]     - max. length of reply buffer

  return value: error code specified in gcn00c.h
 
  -----------------------------------------------------------------------------
 */
tcn00_Error cn40ExecSDBInst
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax);

/*!
  -----------------------------------------------------------------------------
  function:     cn40ExecNiPingSrv
  -----------------------------------------------------------------------------

  description:  executes %DBROOT%\sap\niping -s -I <idletime>

  arguments:    vcontrol    [INOUT]  - DBMServer session data
                command     [IN]     - DBMServer command
                replyData   [OUT]    - reply buffer
                replyLen    [OUT]    - lenght of reply
                replyLenMax [IN]     - max. length of reply buffer

  return value: error code specified in gcn00c.h
 
  -----------------------------------------------------------------------------
 */
tcn00_Error cn40ExecNiPingSrv
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax);

/*!
  -----------------------------------------------------------------------------
  function:     cn40ExecXPU
  -----------------------------------------------------------------------------

  description:  executes xpu

  arguments:    vcontrol    [INOUT]  - DBMServer session data
                command     [IN]     - DBMServer command
                replyData   [OUT]    - reply buffer
                replyLen    [OUT]    - lenght of reply
                replyLenMax [IN]     - max. length of reply buffer

  return value: error code specified in gcn00c.h
 
  -----------------------------------------------------------------------------
 */
tcn00_Error cn40ExecXPU
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax);

/*!
  -----------------------------------------------------------------------------
  function:     cn40LoadSysTab
  -----------------------------------------------------------------------------

  description:  loads the system tables

  arguments:    vcontrol    [INOUT]  - DBMServer session data
                command     [IN]     - DBMServer command
                replyData   [OUT]    - reply buffer
                replyLen    [OUT]    - lenght of reply
                replyLenMax [IN]     - max. length of reply buffer

  return value: error code specified in gcn00c.h
 
  -----------------------------------------------------------------------------
 */
tcn00_Error cn40LoadSysTab
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax);

/*!
  -----------------------------------------------------------------------------
  function:     cn40LoadR3Tab
  -----------------------------------------------------------------------------

  description:  loads the R/3 tables

  arguments:    vcontrol    [INOUT]  - DBMServer session data
                command     [IN]     - DBMServer command
                replyData   [OUT]    - reply buffer
                replyLen    [OUT]    - lenght of reply
                replyLenMax [IN]     - max. length of reply buffer

  return value: error code specified in gcn00c.h
 
  -----------------------------------------------------------------------------
 */
tcn00_Error cn40LoadR3Tab
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax);

/*!
  -----------------------------------------------------------------------------
  function:     cn40LoadLcApps
  -----------------------------------------------------------------------------

  description:  loads the com objects

  arguments:    vcontrol    [INOUT]  - DBMServer session data
                command     [IN]     - DBMServer command
                replyData   [OUT]    - reply buffer
                replyLen    [OUT]    - lenght of reply
                replyLenMax [IN]     - max. length of reply buffer

  return value: error code specified in gcn00c.h
 
  -----------------------------------------------------------------------------
 */
tcn00_Error cn40LoadLcApps
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax);

/*!
  -----------------------------------------------------------------------------
  function:     cn40LoadTutorial
  -----------------------------------------------------------------------------

  description:  loads the com objects

  arguments:    vcontrol    [INOUT]  - DBMServer session data
                command     [IN]     - DBMServer command
                replyData   [OUT]    - reply buffer
                replyLen    [OUT]    - lenght of reply
                replyLenMax [IN]     - max. length of reply buffer

  return value: error code specified in gcn00c.h
 
  -----------------------------------------------------------------------------
 */
tcn00_Error cn40LoadTutorial
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax);

/*!
  -----------------------------------------------------------------------------
  function:     cn40ExecXKernprot
  -----------------------------------------------------------------------------

  description:  execute xkernprot

  arguments:    vcontrol    [INOUT]  - DBMServer session data
                command     [IN]     - DBMServer command
                replyData   [OUT]    - reply buffer
                replyLen    [OUT]    - lenght of reply
                replyLenMax [IN]     - max. length of reply buffer

  return value: error code specified in gcn00c.h
 
  -----------------------------------------------------------------------------
 */
tcn00_Error cn40ExecXKernprot
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax);

/*!
  -----------------------------------------------------------------------------
  function:     cn40GetVersion
  -----------------------------------------------------------------------------

  description:  shows DBMServer version

  arguments:    vcontrol    [INOUT]  - DBMServer session data
                command     [IN]     - DBMServer command
                replyData   [OUT]    - reply buffer
                replyLen    [OUT]    - lenght of reply
                replyLenMax [IN]     - max. length of reply buffer

  return value: error code specified in gcn00c.h
 
  -----------------------------------------------------------------------------
 */
tcn00_Error cn40GetVersion
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax);

/*!
  -----------------------------------------------------------------------------
  function:     cn40DBMGetPath
  -----------------------------------------------------------------------------

  description:  get a path

  arguments:    vcontrol    [INOUT]  - DBMServer session data
                command     [IN]     - DBMServer command
                replyData   [OUT]    - reply buffer
                replyLen    [OUT]    - lenght of reply
                replyLenMax [IN]     - max. length of reply buffer

  return value: error code specified in gcn00c.h
 
  -----------------------------------------------------------------------------
 */
tcn00_Error cn40DBMGetPath
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax);

/*!
  -----------------------------------------------------------------------------
  function:     cn40DBMSetPath
  -----------------------------------------------------------------------------

  description:  set a path

  arguments:    vcontrol    [INOUT]  - DBMServer session data
                command     [IN]     - DBMServer command
                replyData   [OUT]    - reply buffer
                replyLen    [OUT]    - lenght of reply
                replyLenMax [IN]     - max. length of reply buffer

  return value: error code specified in gcn00c.h
 
  -----------------------------------------------------------------------------
 */
tcn00_Error cn40DBMSetPath
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax);

/*!
  -----------------------------------------------------------------------------
  function:     cn40DBCreate
  -----------------------------------------------------------------------------

  description:  creates a database

  arguments:    vcontrol    [INOUT]  - DBMServer session data
                command     [IN]     - DBMServer command
                replyData   [OUT]    - reply buffer
                replyLen    [OUT]    - lenght of reply
                replyLenMax [IN]     - max. length of reply buffer

  return value: error code specified in gcn00c.h
 
  -----------------------------------------------------------------------------
 */
tcn00_Error cn40DBCreate
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax);

/*!
  -----------------------------------------------------------------------------
  function:     cn40DBEnum
  -----------------------------------------------------------------------------

  description:  shows all database on the server

  arguments:    vcontrol    [INOUT]  - DBMServer session data
                command     [IN]     - DBMServer command
                replyData   [OUT]    - reply buffer
                replyLen    [OUT]    - lenght of reply
                replyLenMax [IN]     - max. length of reply buffer

  return value: error code specified in gcn00c.h
 
  -----------------------------------------------------------------------------
 */
tcn00_Error cn40DBEnum
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax);

/*!
  -----------------------------------------------------------------------------
  function:     cn40DBStart
  -----------------------------------------------------------------------------

  description:  starts the db

                <offline> -> <cold>

  arguments:    vcontrol    [INOUT]  - DBMServer session data
                command     [IN]     - DBMServer command
                replyData   [OUT]    - reply buffer
                replyLen    [OUT]    - lenght of reply
                replyLenMax [IN]     - max. length of reply buffer

  return value: error code specified in gcn00c.h
 
  -----------------------------------------------------------------------------
 */
tcn00_Error cn40DBStart
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax);

/*!
  -----------------------------------------------------------------------------
  function:     cn40DBStop
  -----------------------------------------------------------------------------

  description:  stops the database ("hard offline")

                <cold>|<warm> -> <offline>

  arguments:    vcontrol    [INOUT]  - DBMServer session data
                command     [IN]     - DBMServer command
                replyData   [OUT]    - reply buffer
                replyLen    [OUT]    - lenght of reply
                replyLenMax [IN]     - max. length of reply buffer

  return value: error code specified in gcn00c.h
 
  -----------------------------------------------------------------------------
 */
tcn00_Error cn40DBStop
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax);

/*!
  -----------------------------------------------------------------------------
  function:     cn40DBClear
  -----------------------------------------------------------------------------

  description:  runs xclear for database 

  arguments:    vcontrol    [INOUT]  - DBMServer session data
                command     [IN]     - DBMServer command
                replyData   [OUT]    - reply buffer
                replyLen    [OUT]    - lenght of reply
                replyLenMax [IN]     - max. length of reply buffer

  return value: error code specified in gcn00c.h
 
  -----------------------------------------------------------------------------
 */
tcn00_Error cn40DBClear
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax);

/*!
  -----------------------------------------------------------------------------
  function:     cn40DBOffline
  -----------------------------------------------------------------------------

  description:  brings the database in offline state
                <current state> -> <offline>

  arguments:    vcontrol    [INOUT]  - DBMServer session data
                command     [IN]     - DBMServer command
                replyData   [OUT]    - reply buffer
                replyLen    [OUT]    - lenght of reply
                replyLenMax [IN]     - max. length of reply buffer

  return value: error code specified in gcn00c.h
 
  -----------------------------------------------------------------------------
 */
tcn00_Error cn40DBOffline
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax);

/*!
  -----------------------------------------------------------------------------
  function:     cn40DBCold
  -----------------------------------------------------------------------------

  description:  brings the database in cold state
                <current state> -> <cold>

  arguments:    vcontrol    [INOUT]  - DBMServer session data
                command     [IN]     - DBMServer command
                replyData   [OUT]    - reply buffer
                replyLen    [OUT]    - lenght of reply
                replyLenMax [IN]     - max. length of reply buffer

  return value: error code specified in gcn00c.h
 
  -----------------------------------------------------------------------------
 */
tcn00_Error cn40DBCold
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax);

/*!
  -----------------------------------------------------------------------------
  function:     cn40DBWarm
  -----------------------------------------------------------------------------

  description:  brings the database in warm state
                <current state> -> <warm>

  arguments:    vcontrol    [INOUT]  - DBMServer session data
                command     [IN]     - DBMServer command
                replyData   [OUT]    - reply buffer
                replyLen    [OUT]    - lenght of reply
                replyLenMax [IN]     - max. length of reply buffer

  return value: error code specified in gcn00c.h
 
  -----------------------------------------------------------------------------
 */
tcn00_Error cn40DBWarm
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax);

/*!
  -----------------------------------------------------------------------------
  function:     cn40Restart
  -----------------------------------------------------------------------------

  description:  Restarts the database
   
                <current state> -> <offline> -> <warm>

  arguments:    vcontrol    [INOUT]  - DBMServer session data
                command     [IN]     - DBMServer command
                replyData   [OUT]    - reply buffer
                replyLen    [OUT]    - lenght of reply
                replyLenMax [IN]     - max. length of reply buffer

  return value: error code specified in gcn00c.h
 
  -----------------------------------------------------------------------------
 */
tcn00_Error cn40DBRestart
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax);

/*!
  -----------------------------------------------------------------------------
  function:     cn40ShowSomething
  -----------------------------------------------------------------------------

  description:  execute xcons show ... for this database

  arguments:    vcontrol    [INOUT]  - DBMServer session data
                command     [IN]     - DBMServer command
                replyData   [OUT]    - reply buffer
                replyLen    [OUT]    - lenght of reply
                replyLenMax [IN]     - max. length of reply buffer

  return value: error code specified in gcn00c.h
 
  -----------------------------------------------------------------------------
 */
tcn00_Error cn40ShowSomething
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax);

/*!
  -----------------------------------------------------------------------------
  function:     cn40ShowList
  -----------------------------------------------------------------------------

  description:  list available show commands

  arguments:    vcontrol    [INOUT]  - DBMServer session data
                command     [IN]     - DBMServer command
                replyData   [OUT]    - reply buffer
                replyLen    [OUT]    - lenght of reply
                replyLenMax [IN]     - max. length of reply buffer

  return value: error code specified in gcn00c.h
 
  -----------------------------------------------------------------------------
 */
tcn00_Error cn40ShowList
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax);

/*!
  -----------------------------------------------------------------------------
  function:     cn40ShowNext
  -----------------------------------------------------------------------------

  description:  read more output from x_cons

  arguments:    vcontrol    [INOUT]  - DBMServer session data
                command     [IN]     - DBMServer command
                replyData   [OUT]    - reply buffer
                replyLen    [OUT]    - lenght of reply
                replyLenMax [IN]     - max. length of reply buffer

  return value: error code specified in gcn00c.h
 
  -----------------------------------------------------------------------------
 */
tcn00_Error cn40ShowNext
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax);

/*!
  -----------------------------------------------------------------------------
  function:     cn40DBConsole
  -----------------------------------------------------------------------------

  description:  execute x_cons command

  arguments:    vcontrol    [INOUT]  - DBMServer session data
                command     [IN]     - DBMServer command
                replyData   [OUT]    - reply buffer
                replyLen    [OUT]    - lenght of reply
                replyLenMax [IN]     - max. length of reply buffer

  return value: error code specified in gcn00c.h
 
  -----------------------------------------------------------------------------
 */
tcn00_Error cn40DBConsole
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax);

/*!
  -----------------------------------------------------------------------------
  function:     cn40InstEnum
  -----------------------------------------------------------------------------

  description:  list installations

  arguments:    vcontrol    [INOUT]  - DBMServer session data
                command     [IN]     - DBMServer command
                replyData   [OUT]    - reply buffer
                replyLen    [OUT]    - lenght of reply
                replyLenMax [IN]     - max. length of reply buffer

  return value: error code specified in gcn00c.h
 
  -----------------------------------------------------------------------------
 */
tcn00_Error cn40InstEnum
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax);

/*!
  -----------------------------------------------------------------------------
  function:     cn40InstReg
  -----------------------------------------------------------------------------

  description:  register installation

  arguments:    vcontrol    [INOUT]  - DBMServer session data
                command     [IN]     - DBMServer command
                replyData   [OUT]    - reply buffer
                replyLen    [OUT]    - lenght of reply
                replyLenMax [IN]     - max. length of reply buffer

  return value: error code specified in gcn00c.h
 
  -----------------------------------------------------------------------------
 */
tcn00_Error cn40InstReg
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax);

/*!
  -----------------------------------------------------------------------------
  function:     cn40InstUnreg
  -----------------------------------------------------------------------------

  description:  unregister installation

  arguments:    vcontrol    [INOUT]  - DBMServer session data
                command     [IN]     - DBMServer command
                replyData   [OUT]    - reply buffer
                replyLen    [OUT]    - lenght of reply
                replyLenMax [IN]     - max. length of reply buffer

  return value: error code specified in gcn00c.h
 
  -----------------------------------------------------------------------------
 */
tcn00_Error cn40InstUnreg
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax);

/*!
  -----------------------------------------------------------------------------
  function:     cn40DbAddDevice
  -----------------------------------------------------------------------------

  description:  add a devspace to the DB

  arguments:    vcontrol    [INOUT]  - DBMServer session data
                command     [IN]     - DBMServer command
                replyData   [OUT]    - reply buffer
                replyLen    [OUT]    - lenght of reply
                replyLenMax [IN]     - max. length of reply buffer

  return value: error code specified in gcn00c.h
 
  -----------------------------------------------------------------------------
 */
tcn00_Error cn40DbAddDevice
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax);

/*!
    @brief add a data volume to the DB

    Name, size and type are taken from the preceding data volume.

    @param vcontrol [INOUT] DBMServer session data
    @param command [IN] DBMServer command
    @param replyData [OUT] reply buffer
    @param replyLen [OUT] lenght of reply
    @param replyLenMax [IN] max. length of reply buffer
    @return error code specified in gcn00c.h
    @see gcn00c.h
*/
tcn00_Error cn40DbAddDataVolumeAuto
    ( VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax);

/*!
  -----------------------------------------------------------------------------
  function:     cn40Test/cn40Crash
  -----------------------------------------------------------------------------

  description:  for test only

                This function is only for tests during DBMServer-Implementation

  arguments:    vcontrol    [INOUT]  - DBMServer session data
                command     [IN]     - DBMServer command
                replyData   [OUT]    - reply buffer
                replyLen    [OUT]    - lenght of reply
                replyLenMax [IN]     - max. length of reply buffer

  return value: error code specified in gcn00c.h
 
  -----------------------------------------------------------------------------
 */
tcn00_Error cn40Test
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax);
tcn00_Error cn40Crash
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax);


/*!
  -----------------------------------------------------------------------------
  function:     cn40FreeCommandObj
  -----------------------------------------------------------------------------

  description:  frees the command object in the vcontrol structiure

  arguments:    vcontrol    [INOUT]  - DBMServer session data

  -----------------------------------------------------------------------------
 */
void cn40FreeCommandObj
      ( VControlDataT * vcontrol );

/*!
  -----------------------------------------------------------------------------
  function:     cn40ExecCommand
  -----------------------------------------------------------------------------

  description:  executes a operating system command

  arguments:    replyData   [OUT]    - reply buffer
                replyLen    [OUT]    - lenght of reply
                replyLenMax [IN]     - max. length of reply buffer
                szProgramm  [IN]     - command line to execute
                szOutput    [IN]     - command line for output (w/o passwords)

  return value: error code specified in gcn00.h
  -----------------------------------------------------------------------------
 */
tcn00_Error cn40ExecCommand
      ( char          * replyData,
        int           * replyLen,
        const int       replyLenMax,
        const char    * szProgram,
        const char    * szOutput,
        bool            bPipe,
        bool            bSynchron);

#endif /* _HCN40_H_ */
