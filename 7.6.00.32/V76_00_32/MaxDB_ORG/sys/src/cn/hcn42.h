/*! 
  -----------------------------------------------------------------------------
 
  module: hcn41.h
 
  -----------------------------------------------------------------------------
 
  responsible:  BerndV
 
  special area: DBMServer File Access
   
  description:  DBMServer File Access - Specification

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

#ifndef _HCN42_H_
#define _HCN42_H_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
 */
#include "gcn00.h"

/*
  -----------------------------------------------------------------------------
  specification public macros
  -----------------------------------------------------------------------------
 */
#define FGET_TOKENDEL_CN42         "#"

#define FGET_DBNAME                "!"

#define FGET_KNLDIAG_CN42           (*(tsp00_Pathc *) "KNLDIAG"           )
#define FGET_KNLDIAGERR_CN42        (*(tsp00_Pathc *) "KNLDIAGERR"        )
#define FGET_KNLDIAGOLD_CN42        (*(tsp00_Pathc *) "KNLDIAGOLD"        )
#define FGET_KNLMSG_CN42            (*(tsp00_Pathc *) "KNLMSG"            )
#define FGET_KNLMSGARC_CN42         (*(tsp00_Pathc *) "KNLMSGARC"         )   
#define FGET_KNLMSGOLD_CN42         (*(tsp00_Pathc *) "KNLMSGOLD"         )
#define FGET_KNLTRC_CN42            (*(tsp00_Pathc *) "KNLTRC"            )
#define FGET_KNLTRCDAT_CN42         (*(tsp00_Pathc *) "KNLTRCDAT"         )
#define FGET_KNLDUMP_CN42           (*(tsp00_Pathc *) "KNLDUMP"           )
#define FGET_KNLEVT_CN42            (*(tsp00_Pathc *) "KNLEVT"            )
#define FGET_RTEDUMP_CN42           (*(tsp00_Pathc *) "RTEDUMP"           )
#define FGET_UTLPRT_CN42            (*(tsp00_Pathc *) "UTLPRT"            )
#define FGET_BACKHIST_CN42          (*(tsp00_Pathc *) "BACKHIST"          )
#define FGET_BACKMDF_CN42           (*(tsp00_Pathc *) "BACKMDF"           )
#define FGET_BACKEBF_CN42           (*(tsp00_Pathc *) "BACKEBF"           )
#define FGET_BACKEBP_CN42           (*(tsp00_Pathc *) "BACKEBP"           )
#define FGET_BACKEBL_CN42           (*(tsp00_Pathc *) "BACKEBL"           )
#define FGET_LOADPRT_CN42           (*(tsp00_Pathc *) "LOADPRT"           )
#define FGET_XSRVPRT_CN42           (*(tsp00_Pathc *) "XSRVPRT"           )
#define FGET_XSRVPRTOLD_CN42        (*(tsp00_Pathc *) "XSRVPRTOLD"        )
#define FGET_REPMAN_CN42            (*(tsp00_Pathc *) "LOADER"            )
#define FGET_DBMPRT_CN42            (*(tsp00_Pathc *) "DBMPRT"            )
#define FGET_DBMMDF_CN42            (*(tsp00_Pathc *) "DBMMDF"            )
#define FGET_DBMPAHI_CN42           (*(tsp00_Pathc *) "DBMPAHI"           )
#define FGET_CTRLMDF_CN42           (*(tsp00_Pathc *) "CTRLMDF"           )
#define FGET_CTRLLOG_CN42           (*(tsp00_Pathc *) "CTRLLOG"           )
#define FGET_DBMCFG_CN42            (*(tsp00_Pathc *) "DBMCFG"            )
#define FGET_DBMINSTCFG_CN42        (*(tsp00_Pathc *) "DBMINSTCFG"        )
#define FGET_DBMGLOBCFG_CN42        (*(tsp00_Pathc *) "DBMGLOBCFG"        )
#define FGET_PARAM_CN42             (*(tsp00_Pathc *) "PARAM"             )
#define FGET_LCINIT_CN42            (*(tsp00_Pathc *) "LCINIT"            )
#define FGET_LCINITCMD_CN42         (*(tsp00_Pathc *) "LCINITCMD"         )
#define FGET_LCINITHIS_CN42         (*(tsp00_Pathc *) "LCINITHIS"         )
#define FGET_APOCOMTRC_CN42         (*(tsp00_Pathc *) "APOCOMTRC"         )
#define FGET_LCTRC_CN42             (*(tsp00_Pathc *) "LCTRC"             )
#define FGET_ATP_CN42               (*(tsp00_Pathc *) "ATP"               )
#define FGET_LCBIN_CN42             (*(tsp00_Pathc *) "LCBIN"             )
#define FGET_INSTPRT_CN42           (*(tsp00_Pathc *) "INSTPRT"           )
#define FGET_USER_CN42              (*(tsp00_Pathc *) "DBMUPC"            )
#define FGET_KNLTRCPRT_CN42         (*(tsp00_Pathc *) "KNLTRCPRT"         )
#define FGET_WIZOVER_CN42           (*(tsp00_Pathc *) "WIZ"               )
#define FGET_WIZDETAIL_CN42         (*(tsp00_Pathc *) "WTC"               )
#define FGET_DBAHIST_CN42           (*(tsp00_Pathc *) "DBAHIST"           )
#define FGET_DBADTL_CN42            (*(tsp00_Pathc *) "DBADTL"            )
#define FGET_DBMSRV_CN42            (*(tsp00_Pathc *) "DBMSRV"            )
#define FGET_AKDMP_CN42             (*(tsp00_Pathc *) "AKDMP"             )
#define FGET_AKBUF_CN42             (*(tsp00_Pathc *) "AKBUF"             )
#define FGET_AKSTM_CN42             (*(tsp00_Pathc *) "AKSTM"             )
#define FGET_DIAGPACK_CN42          (*(tsp00_Pathc *) "DIAGTGZ"           )
#define FGET_DIAGHIST_CN42          (*(tsp00_Pathc *) "DIAGHIST"          )
#define FGET_DIAGDIR_CN42           (*(tsp00_Pathc *) "DIAGDIR"           )
#define FGET_ANALYZER_CN42          (*(tsp00_Pathc *) "ANALYZER"          )
#define FGET_SDBINFO_CN42           (*(tsp00_Pathc *) "SDBINFO"           )
#define FGET_ANAPID_CN42            (*(tsp00_Pathc *) "ANAPID"            )
#define FGET_ANASID_CN42            (*(tsp00_Pathc *) "ANASID"            )
#define FGET_EVTDISPCONF_CN42       (*(tsp00_Pathc *) "EVTDISPCONF"       )
#define FGET_EVTDISPCDEF_CN42       (*(tsp00_Pathc *) "EVTDISPCDEF"       )
#define FGET_EVTDISPPRT_CN42        (*(tsp00_Pathc *) "EVTDISPPRT"        )
#define FGET_EXTDBPRT_CN42          (*(tsp00_Pathc *) "EXTDBPRT"          )
#define FGET_KNLCOR_CN42            (*(tsp00_Pathc *) "KNLCOR"            )
#define FGET_KNLBAD_CN42            (*(tsp00_Pathc *) "KNLBAD"            )
#define FGET_SEVTDISPCFG_CN42       (*(tsp00_Pathc *) "SEVTDISPCFG"       )
#define FGET_SHMDATA_CN42           (*(tsp00_Pathc *) "SHMDATA"           )
#define FGET_SHMADMIN_CN42          (*(tsp00_Pathc *) "SHMADMIN"          )
#define FGET_SCHEDULERCFG_CN42      (*(tsp00_Pathc *) "SCHEDULERCFG"      )
#define FGET_DBMSRVMINIDUMP_CN42    (*(tsp00_Pathc *) "DBMSRVMINIDUMP"    )
#define FGET_DBMSRVSTKTRC_CN42      (*(tsp00_Pathc *) "DBMSRVSTKTRC"      )
#define FGET_COMSEGDUMP_CN42        (*(tsp00_Pathc *) "COMSEGDUMP"        )
#define FGET_KNLMINIDUMP_CN42       (*(tsp00_Pathc *) "KNLMINIDUMP"       )

#define FGET_TEST_CN42              (*(tsp00_Pathc *) "TESTFILE"          )


#define FOBS_KNLDIAGSTART_CN42      (*(tsp00_Pathc *) "KNLDIAGSTART"      )
#define FOBS_KNLDIAGRUN_CN42        (*(tsp00_Pathc *) "KNLDIAGRUN"        )
#define FOBS_KNLDIAGOLDSTART_CN42   (*(tsp00_Pathc *) "KNLDIAGOLDSTART"   )
#define FOBS_KNLDIAGOLDRUN_CN42     (*(tsp00_Pathc *) "KNLDIAGOLDRUN"     )
#define FOBS_KNLDIAGERRCOMP_CN42    (*(tsp00_Pathc *) "KNLDIAGERRCOMP"    )
#define FOBS_DBMUTLCOMP_CN42        (*(tsp00_Pathc *) "DBMUTLCOMP"        )
#define FOBS_KNLDIAGCLASSIC_CN42    (*(tsp00_Pathc *) "KNLDIAGCLASSIC"    )
#define FOBS_KNLDIAGVERBOSE_CN42    (*(tsp00_Pathc *) "KNLDIAGVERBOSE"    )
#define FOBS_KNLDIAGOLDCLASSIC_CN42 (*(tsp00_Pathc *) "KNLDIAGOLDCLASSIC" )
#define FOBS_KNLDIAGOLDVERBOSE_CN42 (*(tsp00_Pathc *) "KNLDIAGOLDVERBOSE" )
#define FOBS_KNLDIAGERRCLASSIC_CN42 (*(tsp00_Pathc *) "KNLDIAGERRCLASSIC" )
#define FOBS_KNLDIAGERRVERBOSE_CN42 (*(tsp00_Pathc *) "KNLDIAGERRVERBOSE" )
#define FOBS_DBMUTLCLASSIC_CN42     (*(tsp00_Pathc *) "DBMUTLClASSIC"     )
#define FOBS_DBMUTLVERBOSE_CN42     (*(tsp00_Pathc *) "DBMUTLVERBOSE"     )

/* old keys needed for protconv */
#define FGET_KNLMSGERR_CN42        (*(tsp00_Pathc *) "KNLMSGERR"          )
#define FGET_KNLMSGOLD_CN42        (*(tsp00_Pathc *) "KNLMSGOLD"          )
#define FGET_UTLMSG_CN42           (*(tsp00_Pathc *) "UTLMSG"             )
#define FGET_KNLDIAGERRNEW_CN42    (*(tsp00_Pathc *) "KNLDIAGERRNEW"      )
#define FGET_KNLDIAGSTART_CN42     (*(tsp00_Pathc *) "KNLDIAGSTART"       )
#define FGET_KNLDIAGRUN_CN42       (*(tsp00_Pathc *) "KNLDIAGRUN"         )
#define FGET_KNLDIAGSTARTOLD_CN42  (*(tsp00_Pathc *) "KNLDIAGSTARTOLD"    )
#define FGET_KNLDIAGRUNOLD_CN42    (*(tsp00_Pathc *) "KNLDIAGRUNOLD"      )

/*!
  -----------------------------------------------------------------------------
  function:     cn42FileGet
  -----------------------------------------------------------------------------

  description:  opens a special db file and reads the first block of contents

  arguments:    vcontrol    [INOUT]  - DBMServer session data
                command     [IN]     - DBMServer command
                replyData   [OUT]    - reply buffer
                replyLen    [OUT]    - lenght of reply
                replyLenMax [IN]     - max. length of reply buffer

  return value: error code specified in gcn00c.h
 
  -----------------------------------------------------------------------------
 */
tcn00_Error cn42FileGet
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax);

/*!
  -----------------------------------------------------------------------------
  function:     cn42FileGetNext
  -----------------------------------------------------------------------------

  description:  continue reading special db file

  arguments:    vcontrol    [INOUT]  - DBMServer session data
                command     [IN]     - DBMServer command
                replyData   [OUT]    - reply buffer
                replyLen    [OUT]    - lenght of reply
                replyLenMax [IN]     - max. length of reply buffer

  return value: error code specified in gcn00c.h
 
  -----------------------------------------------------------------------------
 */
tcn00_Error cn42FileGetNext
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax);

/*!
  -----------------------------------------------------------------------------
  function:     cn42FileGetList
  -----------------------------------------------------------------------------

  description:  list "getable" files

  arguments:    vcontrol    [INOUT]  - DBMServer session data
                command     [IN]     - DBMServer command
                replyData   [OUT]    - reply buffer
                replyLen    [OUT]    - lenght of reply
                replyLenMax [IN]     - max. length of reply buffer

  return value: error code specified in gcn00c.h
 
  -----------------------------------------------------------------------------
 */
tcn00_Error cn42FileGetList
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax);

/*!
  -----------------------------------------------------------------------------
  function:     cn42FileOperation
  -----------------------------------------------------------------------------

  description:  executes the "file oepration"

  arguments:    vcontrol    [INOUT]  - DBMServer session data
                command     [IN]     - DBMServer command
                replyData   [OUT]    - reply buffer
                replyLen    [OUT]    - lenght of reply
                replyLenMax [IN]     - max. length of reply buffer

  return value: error code specified in gcn00c.h
 
  -----------------------------------------------------------------------------
 */
tcn00_Error cn42FileOperation
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax);

/*!
  -----------------------------------------------------------------------------
  function:     cn42FileBackup
  -----------------------------------------------------------------------------

  description:  backups the specified special db file

  arguments:    vcontrol    [INOUT]  - DBMServer session data
                command     [IN]     - DBMServer command
                replyData   [OUT]    - reply buffer
                replyLen    [OUT]    - lenght of reply
                replyLenMax [IN]     - max. length of reply buffer

  return value: error code specified in gcn00c.h
 
  -----------------------------------------------------------------------------
 */
tcn00_Error cn42FileBackup
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax);

/*!
  -----------------------------------------------------------------------------
  function:     cn42FileRestore
  -----------------------------------------------------------------------------

  description:  restores the specified special db file

  arguments:    vcontrol    [INOUT]  - DBMServer session data
                command     [IN]     - DBMServer command
                replyData   [OUT]    - reply buffer
                replyLen    [OUT]    - lenght of reply
                replyLenMax [IN]     - max. length of reply buffer

  return value: error code specified in gcn00c.h
 
  -----------------------------------------------------------------------------
 */
tcn00_Error cn42FileRestore
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax);

/*!
  -----------------------------------------------------------------------------
  function:     cn42DiagHistList
  -----------------------------------------------------------------------------

  description:  lists the diagnose history

  arguments:    vcontrol    [INOUT]  - DBMServer session data
                command     [IN]     - DBMServer command
                replyData   [OUT]    - reply buffer
                replyLen    [OUT]    - lenght of reply
                replyLenMax [IN]     - max. length of reply buffer

  return value: error code specified in gcn00c.h
 
  -----------------------------------------------------------------------------
 */
tcn00_Error cn42DiagHistList
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax);

/*!
  -----------------------------------------------------------------------------
  function:     cn42DiagPack
  -----------------------------------------------------------------------------

  description:  pack a diagnose package

  arguments:    vcontrol    [INOUT]  - DBMServer session data
                command     [IN]     - DBMServer command
                replyData   [OUT]    - reply buffer
                replyLen    [OUT]    - lenght of reply
                replyLenMax [IN]     - max. length of reply buffer

  return value: error code specified in gcn00c.h
 
  -----------------------------------------------------------------------------
 */
tcn00_Error cn42DiagPack
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax);

/*!
  -----------------------------------------------------------------------------
  function:     cn42BackupFile
  -----------------------------------------------------------------------------

  description:  backup one file 

  arguments:    szDbName    [IN]  - database name
                szType      [IN]  - file to backup (FGET_*_CN42 value)
                oError      [OUT] - filled in case of errors

  return value: success or not
  -----------------------------------------------------------------------------
 */
bool cn42BackupFile
      ( const tsp00_DbNamec & szDbName,
        const tsp00_Pathc   & szType,  
        DBMSrvMsg_Error     & oError );

/*!
  -----------------------------------------------------------------------------
  function:     cn42RestoreFile
  -----------------------------------------------------------------------------

  description:  restore one file 

  arguments:    szDbName    [IN]  - database name
                szType      [IN]     - file to restore (FGET_*_CN42 value)
                oError      [OUT] - filled in case of errors

  return value: success or not
  -----------------------------------------------------------------------------
 */
bool cn42RestoreFile
      ( const tsp00_DbNamec & szDbName,
        const tsp00_Pathc   & szType,  
        DBMSrvMsg_Error     & oError );

/*!
  -----------------------------------------------------------------------------
  function:     cn42GetFileName
  -----------------------------------------------------------------------------

  description:  compute filenames for a special db file

  arguments:    szDbName     [IN]     - Name of database
                szType       [IN]     - file key (FGET_*_CN42 value)
                szFileName   [OUT]    - full filename
                szFileDir    [OUT]    - file directory
                szBackupName [OUT]    - full backupname
                szBackupDir  [OUT]    - backup directory
                bBinary      [OUT]    - is this a binary 
  return value: error code specified in gcn00c.h
 
  prototypes:   9
  -----------------------------------------------------------------------------
 */
tcn00_Error cn42GetFileName
      ( const tsp00_DbNamec & szDbName,
        const tsp00_Pathc   & szType,  
        tsp00_Pathc         & szFileName,
        tsp00_Pathc         & szBackupName);
tcn00_Error cn42GetFileName
      ( const tsp00_DbNamec & szDbName,
        const tsp00_Pathc   & szType,  
        tsp00_Pathc         & szFileName);

/*
    @brief compute file name for a given file key

    Parameter errOut is cleared at first. If the return value
    is false, errOut is not empty. If the return value is true,
    errOut remains empty.

    @param szDbName [IN] name of database
    @param szType [IN] file key (FGET_*_CN42 value)
    @param szFileName [OUT] absolute file name
    @param errOut [OUT] errors encountered
    @return true if calculation was successful, false otherwise.
*/
bool cn42GetFileName
      ( const tsp00_DbNamec & szDbName,
        const tsp00_Pathc   & szType,  
        tsp00_Pathc         & szFileName,
        DBMSrvMsg_Error& errOut);

/*!
  -----------------------------------------------------------------------------
  function:     cn42RemoveFiles
  -----------------------------------------------------------------------------

  description:  removes all databasefiles with the remove flag

  arguments:    szDbName     [IN]     - Name of database
  -----------------------------------------------------------------------------
 */
void cn42RemoveFiles
      ( const tsp00_DbNamec & szDbName);

/*!
  -----------------------------------------------------------------------------
  function:     cn42RemoveFiles
  -----------------------------------------------------------------------------

  description:  removes all databasefiles with the remove flag

  arguments:    szDbName     [IN]     - Name of database
                szType       [IN]     - file key (FGET_*_CN42 value)
  -----------------------------------------------------------------------------
 */
void cn42RemoveOneFile
      ( const tsp00_DbNamec & szDbName,
        const tsp00_Pathc   & szType );

#endif /* _HCN42_H_ */
