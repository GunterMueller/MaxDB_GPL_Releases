/*!***************************************************************************

  module      : gsp09.h

  -------------------------------------------------------------------------

  responsible : FrankH

  special area: RTE Server functions
  description : Server and client specific declarations and function

  Functions used by different servers, i.e. control server and replication manager

  last changed: 2000-06-21  16:59

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 1999-2005 SAP AG




    ========== licence begin  GPL
    Copyright (c) 1999-2005 SAP AG

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




*****************************************************************************/

#ifndef GSP09_H
#define GSP09_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "gsp01.h"
#include "RunTime/RTE_Types.h"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/
#define CtrlPacketSize_csp09 16384

#define CtrlCmdOk_csp09      0
#define CtrlCmdErr_csp09     1
#define CtrlRelease_csp09    2

#define NoOptions_csp09         0
#define StartAutomatic_csp09    1

#define XrteOk_csp09      0
#define XrteNotOk_csp09   1
#define XrteAtEnd_csp09   2

#define Kernel_csp09     FALSE
#define Gateway_csp09    TRUE

#define VersionDigits_csp09     4
#define InvalidVersion_csp09  (-1)


#define StateUnknown_csp09  0
#define StateOff_csp09      1
#define StateRunning_csp09  2
#define StateRuin_csp09     3

/*
 * If tsp09_RteInstallationInfo is modified added a new definition here
 */
#define INSTALLATION_RECORD_VERSION_V721 0

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

typedef int         tsp09_bool;
typedef const char* tsp09_cstr;

typedef tsp00_Int4       tsp09_server_options;


/*+!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifndef IST_UNSEREN_KONVENTIONEN_ANGEPASST
/*+!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

typedef enum {
    csp9_fast_pgm, csp9_quick_pgm, csp9_slow_pgm, csp9_test_pgm, csp9_diag_pgm, csp9_any_pgm
} tsp9_pgm_kind, tsp09_pgm_kind;

typedef enum {
    csp9_read_first, csp9_read_next, csp9_read_again
} tsp9_rte_readdirection, tsp09_rte_readdirection;

/*+!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#endif
/*+!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/


typedef struct _tsp09_rte_xerror {
    int         xe_result;
    int         xe_native_error;
    char        xe_text[200];
} tsp09_rte_xerror;


typedef struct _tsp09_release_version {
    signed char  no[VersionDigits_csp09];
} tsp09_release_version;

typedef struct _tsp09_rte_dbinfo {
    tsp00_DbNamec         dbname;
    tsp00_Pathc           dbroot;
    tsp09_release_version version;
    tsp09_pgm_kind        pgmKind;
    tsp09_server_options  options;
    int                   state;
} tsp09_rte_dbinfo;

typedef struct _tsp09_rte_installationinfo {
    tsp00_Pathc           dbroot;
    tsp09_release_version version;
} tsp09_rte_installationinfo;

typedef struct _tsp09_RteInstallationInfoNew {
    tsp00_Int4            RecordVersion; /* INSTALLATION_RECORD_VERSION_VXXX     */
    tsp00_Int4            RecordLength;  /* sizeof(tsp09_RteInstallationInfoNew) */
    tsp00_Pathc           dbroot;
    tsp09_release_version version;
    tsp00_Pathc           key;           /* on request to be able to use DBROOT path as key... */
} tsp09_RteInstallationInfoNew;

/**\
--------------------------------------- MF__ MOD__ (remote control)
\**/
/**\
--------------------------------------- MF__ MOD__ (control interface)
\**/


/*+!!!!!!!!!!!!!!! VSP009C !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#ifndef _VSP009C_
#define _VSP009C_
#endif

#define csp9_ctrlcmd_ok        CtrlCmdOk_csp09
#define csp9_ctrlcmd_err       CtrlCmdErr_csp09
#define csp9_ctrl_release      CtrlRelease_csp09

#define csp9_xrte_ok           CtrlCmdOk_csp09
#define csp9_xrte_notok        CtrlCmdErr_csp09
#define csp9_xrte_atend        CtrlRelease_csp09

#define csp9_kernel            Kernel_csp09
#define csp9_gateway           Gateway_csp09

#define csp9_version_digits    VersionDigits_csp09
#define csp9_invalid_version   InvalidVersion_csp09
#define csp9_ctrl_packet_size  CtrlPacketSize_csp09

#define csp9_state_unknown     StateUnknown_csp09
#define csp9_state_off         StateOff_csp09
#define csp9_state_running     StateRunning_csp09
#define csp9_state_ruin        StateRuin_csp09

#define csp9_no_options        NoOptions_csp09
#define csp9_start_automatic   StartAutomatic_csp09

typedef tsp09_bool                 tsp9_bool;
typedef tsp09_cstr                 tsp9_cstr;
typedef tsp09_server_options       tsp9_server_options;
typedef tsp09_rte_xerror           tsp9_rte_xerror;
typedef tsp09_release_version      tsp9_release_version;
typedef tsp09_rte_dbinfo           tsp9_rte_dbinfo;
typedef tsp09_rte_installationinfo tsp9_rte_installationinfo;

/*+!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/


externC void
ctrlservopen ( void**         vcontrolData,
               tsp09_cstr     dbname,
               tsp09_cstr     dbroot,
               tsp00_BoolAddr cancelAddr,
               tsp00_Bool     loggedOn,
               tsp00_Bool     couldLogOn,
               int            callerId,
               tsp00_Bool     isLocalCommunication,
               const char*    clientMachineID,
               SAPDB_Int8     clientPIDonClientMachine);

externC  void
ctrlservclose ( void* vcontrolData);

/*! 
    @brief exception handling
*/
#if defined( _WIN32 )
externC int
ctrlservExceptionHandler( unsigned int exCode,
                          struct _EXCEPTION_POINTERS *ep);
#else
externC void
ctrlservExceptionHandler( int signalCode );
#endif

/*!
    @brief allow server implementation to handle the error (e.g. at least log it)
*/
externC void
ctrlservHandleCommError ( int callerId,
                          tsp00_Bool connected,
                          tsp00_Bool cancelByte,
                          tsp01_CommErr commErr);

externC int
ctrlservcommand ( void* vcontrolData,
                  char* requestData,
                  int   requestLen,
                  char* replyData,
                  int*  replyLen,
                  int   replyLenmax);

externC void
sqlxservinit ( void         **rteData,
               int            argc,
               char          *argv [],
               char          *dbname,
               char          *dbroot,
               SAPDB_Char *   senderNodeId,
               RTE_OSPid VAR_VALUE_REF  senderPID,
               tsp00_BoolAddr loggedOnPtr,
               tsp00_BoolAddr couldLogOnPtr,
               int           *callerid,
               tsp00_BoolAddr isLocalCommunication,
               tsp01_CommErr* commErr);

externC void
sqlxservrecv ( void          *voidRTE,
               char         **recvPacket,
               int           *recvLen,
               char         **sendPacket,
               int           *maxSendLen,
               tsp01_CommErr *commErr);

externC void
sqlxservsend ( void          *voidRTE,
               int            sendLen,
               tsp01_CommErr *commErr);

externC void sqlxservexit ( void *voidRTE);

/**\
--------------------------------------- MF__ MOD__ (control implementation)
\**/


externC  void sqlxinit (
    tsp09_cstr dbname,
    tsp09_cstr dbroot);

externC  void sqlxfinish (void);

externC void
sqlxcanstart ( tsp9_rte_xerror *xerror);

/*externC tsp00_Uint4
sqlxdbstate ( tsp9_cstr        dbname,
              tsp9_rte_xerror  *xerror);
*/
externC void sqlxstart (
    tsp09_cstr dbname,
    tsp09_pgm_kind pgmKind,
    tsp09_cstr dbroot,
    boolean is_gateway,
    int argc,
    char* argv[],
    tsp09_rte_xerror* xerror);

externC void sqlxstop (
    tsp09_cstr dbname,
    tsp09_cstr dbroot,
    boolean dumpflag,
    tsp09_rte_xerror* xerror);

externC void sqlxclear (
    tsp09_cstr dbname,
    tsp09_cstr dbroot,
    tsp09_rte_xerror* xerror);

externC void sqlxkernelcntl (
    tsp09_cstr cmdstring,
    tsp09_rte_xerror* xerror);

externC void sqlxknlopenmonitor (
    tsp09_cstr topic,
    int flags,
    void** monitorHandle,
    tsp09_cstr* description,
    tsp09_rte_xerror* xerror);

externC void sqlxknlmonitor (
    void* monitorHandle,
    tsp09_rte_readdirection direction,
    char* description,
    int maxDescLen,
    tsp09_rte_xerror* xerror);

externC void sqlxknlclosemonitor (
    void* monitorHandle);

/* PTS 1104330 */
externC tsp00_Bool sqlxregisterglobal  (
    const char      *GlobalKey,
    char            *GlobalValue,
    tsp01_RteError  *RteError ) ;

/* PTS 1104330 */
externC tsp00_Bool sqlxgetglobal (
    const char      *GlobalKey,
    char            *GlobalValue,
    int              MaxValueLen,
    tsp01_RteError  *RteError );

externC void sqlxregisterinst (
    tsp09_cstr           dbroot,
    tsp09_release_version* version,
    tsp09_rte_xerror*    xerror);

externC void sqlxunregisterinst (
    tsp09_cstr           dbroot,
    tsp09_rte_xerror*    xerror);

externC void sqlxopen_installation_enum (
    void**              handle,
    tsp09_rte_xerror*    xerror);

externC void sqlxnext_installation (
    void*               handle,
    tsp09_rte_installationinfo* releaseinfo,
    tsp09_rte_xerror*    xerror);

externC void sqlxclose_installation_enum (
    void*               handle);

/*!
  function: sqlxlogon

  description: Impersonate current program to UNIX-username/password combination

  Only a superuser (or setuid root) program can change its impersonation to a
  different user. This call is a one way ticket!

  NOTE: UNIX only

  arguments: username [in] UNIX-Username
             password [in] NOT CRYPTED Password (if NULL this is not checked! PTS 1104488 )
             xerror   [out] Returned Error
 */
externC void sqlxlogon (
    const char          *username,
    const char          *password,
    tsp09_rte_xerror    *xerror);

/*!
  function: sqlxidlogon

  description: Impersonate current program to UNIX-userid

  Only a superuser (or setuid root) program can change its impersonation to a
  different user. This call is a one way ticket!

  NOTE: UNIX only

  arguments: userid   [in] UNIX-Userid
             xerror   [out] Returned Error
 */
externC void sqlxidlogon (
    int                   userid,
    tsp09_rte_xerror    *xerror);

/* PTS 1104488 */
/*!
  function: sqlxusername

  description: Retrieve UNIX-Username of currently effective user

  This routine supports retrieving readable username of the effectiv user
  who started the calling program.

  NOTE: UNIX only, NT currently not supported (since sqlxlogon/sqlxchown only available on UNIX...)

  arguments: username [out] OS Username
             xerror [out] Returned Error
 */
externC const char *sqlxusername ();

/* PTS 1106827 */
/*!
  function: sqlxchown

  description: Set owner/group of given file to those of given UNIX-username

  This function uses the given username to find the coresponding password file
  entry. If found, the gid/uid is used to chown file found at given path.

  NOTE: UNIX only, NT does not allow to modify an owner of a file other than via 'GetOwnership'

  arguments: username [in] OS Username
             path [in] Path to file
             xerror [out] Returned Error
 */
externC void sqlxchown (
    const char                   *username,
    tsp00_Pathc     VAR_ARRAY_REF path,
    tsp09_rte_xerror             *xerror);

externC void
sqlx2connectc ( tsp00_Int4                     pid,
                char                          *servernode,
                char                          *serverdb,
                char                          *dbroot,
                char                          *serverpgm,
                tsp00_Int4                    *reference,
                tsp00_Int4                    *sql_packet_size,
                void                         **sql_packet,
                tsp00_ErrTextc VAR_ARRAY_REF   errtext,
                tsp01_CommErr                 *returncode);

externC void sqlxregisterinstNew (
    tsp09_RteInstallationInfoNew VAR_VALUE_REF info,
    tsp09_rte_xerror*    xerror);

externC void sqlxnext_installationNew (
    void*                                      handle,
    tsp09_RteInstallationInfoNew VAR_VALUE_REF info,
    tsp09_rte_xerror*                          xerror);

/* PTS 1109858 */
/*
  Function: sqlDBSpeed
  Description: Get speed of currently running database

  All possible database speeds are check. If database is not running csp9_any_pgm is returned.

  Return value: csp9_fast_pgm,csp9_quick_pgm,csp9_slow_pgm or csp9_any_pgm if no database running
 */
externC tsp9_pgm_kind sqlDBSpeed( const tsp00_DbNamec szDbName );
/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

#endif  /* GSP09_H */
