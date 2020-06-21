/*!
  @file           gen500.h
  @author         JoergM
  @special area   Kernel LZU UNIX Messages
  @brief          Defines all messages of UNIX Kernel
  @see            example.html ...

\if EMIT_LICENCE

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



\endif
*/



#ifndef GEN500_H
#define GEN500_H


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define IERR_TYPE	0
#define ERR_TYPE	1
#define WRN_TYPE	2
#define INFO_TYPE	3

/********************************** I E R R ***********************************/

/* ============================ veo35k.cpp ================================ */

#define IERR_DCOM_NYI                11000,IERR_TYPE,"DCOM    ","Not yet implemented"

/* ============================ veo36.cpp + veo38.cpp ===================== */

#define ERR_COINITIALIZE             11000,ERR_TYPE, "DCOM    ","CoInitialize failed, HRESULT = %d "
#define ERR_DCOM_COMETHOD_CRASHED    11000,ERR_TYPE, "DCOM    ","COM-OBJECT : unhandled exception IID:{%8.8X-%4.4X-%4.4X-%2.2X%2.2X-%2.2X%2.2X%2.2X%2.2X%2.2X%2.2X} FKTNO:%d"
#define ERR_DCOM_DBPROC_CRASHED      11000,ERR_TYPE, "DCOM    ","COM-DBPROC : unhandled exception IID:{%8.8X-%4.4X-%4.4X-%2.2X%2.2X-%2.2X%2.2X%2.2X%2.2X%2.2X%2.2X} FKTNO:%d"
#define ERR_DCOM_DUMP_INFO           11000,ERR_TYPE, "DCOM-DMP","%s"
#define ERR_DCOM_COCREATE            11000,ERR_TYPE, "DCOM    ","%s - HRESULT: %d, Context: %s "
#define ERR_DCOM_GUID                11000,ERR_TYPE, "DCOM    ","%s: {%8.8X-%4.4X-%4.4X-%2.2X%2.2X-%2.2X%2.2X%2.2X%2.2X%2.2X%2.2X}"
#define ERR_DCOM_HRESULT             11000,ERR_TYPE, "DCOM    ","%s - HRESULT: %d"
#define ERR_DCOM_NOMEM               11000,ERR_TYPE, "DCOM    ","%s - no more memory"
#define ERR_DCOM_PROXY_MSG           11000,ERR_TYPE, "DCOM-PXY","%s"

/* ============================ ven41c ================================ */

#define ERR_IPC_SEMC_SEMGET          11987, ERR_TYPE, "IPC     ", "create_sem: semget error, %s"
#define ERR_IPC_SEMC_SEMCTL_STAT     11987, ERR_TYPE, "IPC     ", "create_sem: semctl (stat) error, %s"
#define ERR_IPC_SEMC_SEMCTL_SET      11987, ERR_TYPE, "IPC     ", "create_sem: semctl (set) error, %s"
#define ERR_IPC_SEMC_IDFILE          11987, ERR_TYPE, "IPC     ", "create_sem: creating id file failed"
#define ERR_IPC_SEMR_SEMCTL          11987, ERR_TYPE, "IPC     ", "remove_sem: id %d semctl (remove) error, %s"
#define ERR_IPC_SHMC_SHMGET          11987, ERR_TYPE, "IPC     ", "create_shm: shmget error, %s"
#define ERR_IPC_SHMC_SHMCTL_STAT     11987, ERR_TYPE, "IPC     ", "create_shm: shmctl (stat) error, %s"
#define ERR_IPC_SHMC_SHMCTL_SET      11987, ERR_TYPE, "IPC     ", "create_shm: shmctl (set) error, %s"
#define ERR_IPC_SHMC_IDFILE          11987, ERR_TYPE, "IPC     ", "create_shm: creating id file failed"
#define ERR_IPC_SHMR_SHMCTL          11987, ERR_TYPE, "IPC     ", "remove_shm: id %d shmctl (remove) error, %s"
#define ERR_IPC_SHMA_SHMAT           11987, ERR_TYPE, "IPC     ", "attach_shm: shmat error, %s id %d flag 0%o addr 0x%p"
#define ERR_IPC_SHMA_WRONG_ADDR      11987, ERR_TYPE, "IPC     ", "attach_shm: got wrong address 0x%p"
#define ERR_IPC_SHMD_SHMDT           11987, ERR_TYPE, "IPC     ", "detach_shm: shmdt address 0x%p, %s"
#define ERR_IPC_SEMS_WHICH           11987, ERR_TYPE, "IPC     ", "stat_sem: id                   %d"
#define ERR_IPC_SEMS_SHMCTL          11987, ERR_TYPE, "IPC     ", "stat_sem: id %d semctl (stat) error, %s"
#define ERR_IPC_SEMS_VALUE           11987, ERR_TYPE, "IPC     ", "stat_sem: semaphore value      %d"
#define ERR_IPC_SEMS_NUM_SEMS        11987, ERR_TYPE, "IPC     ", "stat_sem: number of sems       %d"
#define ERR_IPC_SEMS_LASTOP_TIME     11987, ERR_TYPE, "IPC     ", "stat_sem: last operation time  %s"
#define ERR_IPC_SEMS_LASTCHG_TIME    11987, ERR_TYPE, "IPC     ", "stat_sem: last change time     %s"
#define ERR_IPC_SEMS_LASTOP_PID      11987, ERR_TYPE, "IPC     ", "stat_sem: pid of last operation %d"
#define ERR_IPC_SEMS_SEMNCNT         11987, ERR_TYPE, "IPC     ", "stat_sem: semncnt %d"
#define ERR_IPC_SEMS_SEMZCNT         11987, ERR_TYPE, "IPC     ", "stat_sem: semzcnt %d"
#define ERR_IPC_SHMS_WHICH           11987, ERR_TYPE, "IPC     ", "stat_shm: id           %d"
#define ERR_IPC_SHMS_SHMCTL          11987, ERR_TYPE, "IPC     ", "stat_shm: shmctl (stat) error, %s"
#define ERR_IPC_SHMS_SIZE            11987, ERR_TYPE, "IPC     ", "stat_shm: shm size     %ld"
#define ERR_IPC_SHMS_ATTACHES        11987, ERR_TYPE, "IPC     ", "stat_shm: attaches     %ld"
#define ERR_IPC_SHMS_CREATOR_UID     11987, ERR_TYPE, "IPC     ", "stat_shm: creator uid  %ld"
#define ERR_IPC_SHMS_OWNER_UID       11987, ERR_TYPE, "IPC     ", "stat_shm: owner uid    %ld"
#define ERR_IPC_SHMS_PERMISSIONS     11987, ERR_TYPE, "IPC     ", "stat_shm: permissions  0%4lo"
#define ERR_IPC_SHMS_CREATOR_PID     11987, ERR_TYPE, "IPC     ", "stat_shm: creator pid  %ld"
#define ERR_IPC_SHMS_LAST_OP_PID     11987, ERR_TYPE, "IPC     ", "stat_shm: last op pid  %ld"
#define ERR_IPC_FIFOC_MKNOD          11987, ERR_TYPE, "IPC     ", "create_fifo: '%s' 0%4o mknod error, %s"
#define ERR_IPC_FIFOC_CHOWN          11987, ERR_TYPE, "IPC     ", "create_fifo: '%s' chown error, %s"
#define ERR_IPC_FIFOR_UNLINK         11987, ERR_TYPE, "IPC     ", "remove_fifo: '%s' unlink error, %s"
#define ERR_IPC_IDFILE_OPEN          11987, ERR_TYPE, "IPC     ", "idfile: '%s' open error, %s"
#define ERR_IPC_IDFILE_FOPEN         11987, ERR_TYPE, "IPC     ", "idfile: '%s' fopen error, %s"
#define ERR_IPC_KEY_STAT             11987, ERR_TYPE, "IPC     ", "key: cannot stat '%s': %s"
#define ERR_IPC_CHECK_DIR_STAT       11987, ERR_TYPE, "IPC     ", "cannot stat directory '%s', %s"
#define ERR_IPC_CHECK_DIR_MKNOD      11987, ERR_TYPE, "IPC     ", "cannot create directory '%s', %s"
#define ERR_IPC_CHECK_DIR_NOT_DIR    11987, ERR_TYPE, "IPC     ", "'%s' is not a directory !"
#define ERR_IPC_REMOVE_POPEN         11987, ERR_TYPE, "IPC     ", "remove_all:popen for '%s' failed, %s"
#define ERR_IPC_REMOVE_ENTRY         11987, ERR_TYPE, "IPC     ", "error removing ipc for '%s', %s"
#define ERR_IPC_REMOVE_FILE          11987, ERR_TYPE, "IPC     ", "error removing file '%s', %s"
#define ERR_IPC_REMOVE_COMMAND       11987, ERR_TYPE, "IPC     ", "'%s' failed with %d"
#define ERR_IPC_REMOVE_RM_COMMAND    11987, ERR_TYPE, "IPC     ", "rm -rf %s failed with %d"

/* ============================ ven42c ================================ */
#define ERR_CONN_SERV_REJECT         11009,IERR_TYPE,"COMMUNIC","server rejected connection"
#define ERR_COM_CERT_BUFFER_TOO_SMALL 11009,IERR_TYPE,"COMMUNIC","Buffer too small to hold server certificate: buflen: %d, certlen: %d"
#define ERR_CONN_BROKEN              11009,IERR_TYPE,"COMMUNIC","Connection broken"
#define ERR_COM_CERT_UNPACK_CERT     11009,IERR_TYPE,"COMMUNIC","Unpack server certificate failed"

/* ============================ gen50.h ================================ */

#define ERR_GOTODISP_SETCONTEXT         11987,IERR_TYPE,"GOTODISP","setcontext: errno = %d"
#define ERR_GOTODISP_SWAPCONTEXT        11987,IERR_TYPE,"GOTODISP","swapcontext: errno = %d"
#define ERR_KERN_PARAM_PID_OUT_OF_RANGE 11987,IERR_TYPE,"KERNPARA","task id out-of-range %d"

/* ============================ ven51c ================================ */

#define IERR_VINITOK_NOT_TIMEOUT     11000,IERR_TYPE,"vinitok ","called by other than timeout"
#define IERR_WRONG_COMM_REC_SIZE     11001,IERR_TYPE,"vinit   ","Inconsistent size of 'RTE_COMM_REC' %d"
#define IERR_VPTYPE_WRONG_TASK       11002,IERR_TYPE,"vptype  ","tried to get type of other task T%ld"
#define IERR_VPTYPE_WRONG_TASK_TYPE  11003,IERR_TYPE,"vptype  ","unknown task type: %d"
#define IERR_SHUTDOWN_NOT_UTILITY    11004,IERR_TYPE,"shutdwon","called by other than utility task (T%d)"
#define IERR_ILL_SHUTDOWN_MODE       11005,IERR_TYPE,"shutdwon","Illegal shutdown mode"
#define IERR_VFINISH_TW_DUR_NORM_OP  11006,IERR_TYPE,"vfinish ","finishing tracewriter during normal operation"

/* ============================ ven53c ================================ */

#define INFO_VCON_WAITING            11007,INFO_TYPE,"COMMUNIC","wait for connection T%d"
#define IERR_VREC_WRONG_PACKET_REQ   11007,IERR_TYPE,"COMMUNIC","wrong packet requested, idx: %d, T%d"
#define IERR_VREC_ILL_LENGTH         11008,IERR_TYPE,"COMMUNIC","illegal length, act:%d, max:%d (T%d)"
#define IERR_VREPLY_WRONG_PACK_ADDR  11009,IERR_TYPE,"COMMUNIC","wrong packet addr 0x%p, expected 0x%p (T%d)"
#define IERR_VREPLY_ILL_PACK_LEN     11009,IERR_TYPE,"COMMUNIC","ill packet len %d, max %d (T%d)"

/* ============================ ven54c ================================ */

#define IERR_VCURRDEVSIZE_INVALID_DEVNO          11000, IERR_TYPE, "vcurrdev", "Invalid devno: %d"
#define ERR_VCURRDEVSIZE_ERR_FSTAT               11000, ERR_TYPE,  "vcurrdev", "Error during fstat(%s): %s"
#define ERR_VDEVSIZE_INVALID_DEVSPACE            11000, ERR_TYPE,  "vdevsize", "Invalid volume (type:%d / devno:%d)"
#define ERR_VDEVSIZE_ERR_STAT                    11000, ERR_TYPE,  "vdevsize", "Stat error on '%s'"
#define ERR_VDEVSIZE_NO_FIFO                     11000, ERR_TYPE,  "vdevsize", "No volume on fifo '%s' !"
#define ERR_VDEVSIZE_NO_DIR                      11000, ERR_TYPE,  "vdevsize", "No volume on directory '%s' !"
#define ERR_VDEVSIZE_IOERROR                     11000, ERR_TYPE,  "vdevsize", "Formatting volume '%s' failed: %s!"
#define ERR_TOUCH_DEVSPACE                       11000, ERR_TYPE,  "vdevsize", "Can't touch dev '%s':%s"
#define INFO_VDEVSIZE_SUCCEEDED					 11000, INFO_TYPE, "vdevsize", "'%s', %ld succeeded"
#define INFO_VDEVSIZE_FAILED                     11000, INFO_TYPE, "vdevsize", "'%s', %ld failed: %s"
#define IERR_DEV0_VATTACH_BLOCKSIZE_ERROR        11000, IERR_TYPE, "d0_vatta", "Attach error! Can't handle different blocksizes"
#define IERR_DEV0_VATTACH_BLOCKSIZE_ERROR_CONT   11000, IERR_TYPE, "d0_vatta", "previous defined block_size: %d, now %d "
#define IERR_DEV0_VATTACH_INVALID_DEVSPACE       11000, IERR_TYPE, "d0_vatta", "Invalid volume ( type:%d / devno:%d )"
#define IERR_DEV0_VATTACH_FAILED                 11000, IERR_TYPE, "d0_vatta", "Attach error"
#define IERR_DEV0_VATTACH_REQUEST_MISMATCH       11000, IERR_TYPE, "d0_vatta", "Request mismatch on %s"
#define ERR_DEV0_VATTACH_DEV0_ERROR              11000, ERR_TYPE,  "vattach ", "dev0_vattach returned FALSE"
#define INFO_VATTACH_SUCCEEDED					 11000, INFO_TYPE, "vattach ", "'%s' devno %d T%d succeeded"
#define INFO_VATTACH_FAILED						 11000, INFO_TYPE, "vattach ", "'%s' T%d failed"
#define INFO_VDETACH_CALLED						 11000, INFO_TYPE, "vdetach ", "'%s' devno %d T%d"
#define IERR_VDETACH_INVALID_DEVNO               11000, IERR_TYPE, "vdetach ", "detach illegal devno %d"
#define IERR_VDETACH_NOT_ATTACHED                11000, IERR_TYPE, "vdetach ", "trying to detach not attached devno %d"
#define IERR_VDETACH_WRONG_EVENT                 11000, IERR_TYPE, "vdetach ", "awoke from wrong event 0x%p"
#define IERR_VDETACH_WRONG_EVENT_CONT            11000, IERR_TYPE, "vdetach ", "         correct event would be 0x%p"
#define IERR_VBLOCKIO_INVALID_DEVNO              11000, IERR_TYPE, "vblockio", "Invalid devno "
#define IERR_VBLOCKIO_WRONG_EVENT                11000, IERR_TYPE, "vblockio", "awoke from wrong event 0x%p"
#define IERR_VBLOCKIO_WRONG_EVENT_CONT           11000, IERR_TYPE, "vblockio", "         correct event would be 0x%p"
#define IERR_VVECTORIO_INVALID_DEVNO             11000, IERR_TYPE, "vectorio", "Invalid devno "
#define IERR_VVECTORIO_WRONG_EVENT               11000, IERR_TYPE, "vectorio", "awoke from wrong event 0x%p"
#define IERR_VVECTORIO_WRONG_EVENT_CONT          11000, IERR_TYPE, "vectorio", "         correct event would be 0x%p"
#define IERR_VVECTORIO_WRONG_EVENT_CONT2         11000, IERR_TYPE, "vectorio", "                                   or 0x%p"
#define IERR_VVECTORIO_NOT_ATTACHED              11000, IERR_TYPE, "vectorio", "SYSTEM ERROR: Volume not attached"
#define IERR_VVECTORIO_NOT_ATTACHED1             11000, IERR_TYPE, "vectorio", "SYSTEM ERROR: First volume not attached"
#define IERR_VVECTORIO_NOT_ATTACHED2             11000, IERR_TYPE, "vectorio", "SYSTEM ERROR: Second volume not attached"
#define ERR_VVECTORIO_SYSTEMERR                  11000, ERR_TYPE,  "vectorio", "SYSTEM ERROR on devno %d"
#define ERR_VBLOCKIO_SYSTEMERR                   11000, ERR_TYPE,  "vblockio", "SYSTEM ERROR on devno %d"
#define IERR_VBLOCKIO_NOT_ATTACHED               11000, IERR_TYPE, "vblockio", "SYSTEM ERROR: Volume not attached "
#define IERR_ASYNOPEN_ERR_OPEN                   11000, IERR_TYPE, "asynopen", "asynopen error: %s"
#define IERR_ASYNOPEN_WRONG_EVENT                11000, IERR_TYPE, "asynopen", "ABEND: asynopen awoke from wrong event 0x%p "
#define IERR_ASYNOPEN_WRONG_EVENT_CONT           11000, IERR_TYPE, "asynopen", "                correct event would be 0x%p "
#define IERR_ASYNOPEN_WRONG_EVENT_CONT2          11000, IERR_TYPE, "asynopen", "                         wrong request %d "
#define IERR_ASYNOPEN_WRONG_EVENT_CONT3          11000, IERR_TYPE, "asynopen", "              correct request would be %d "
#define INFO_VASYNOPEN_SUCCEEDED				 11000, INFO_TYPE, "vasynopen","'%s' devno %d T%d succeeded"
#define INFO_VASYNOPEN_FAILED					 11000, INFO_TYPE, "vasynopen","'%s' T%d failed"
#define INFO_VASYNCLOSE_CALLED                    11000, INFO_TYPE, "vasynclos","'%s' devno %d T%d"
#define IERR_ASYNCLOSE_INVALID_FILENO            11000, IERR_TYPE, "asynclos", "illegal hostfileno %ld"
#define IERR_ASYNCLOSE_NOT_OPEN                  11000, IERR_TYPE, "asynclos", "device not open %ld "
#define IERR_ASYNCLOSE_WRON_EVENT                11000, IERR_TYPE, "asynclos", "awoke from wrong event 0x%p "
#define IERR_ASYNCLOSE_WRON_EVENT_CONT           11000, IERR_TYPE, "asynclos", "         correct event would be 0x%p "
#define IERR_ASYNCLOSE_WRON_EVENT_CONT2          11000, IERR_TYPE, "asynclos", "                 wrong request %d "
#define IERR_ASYNCLOSE_WRON_EVENT_CONT3          11000, IERR_TYPE, "asynclos", "               correct request would be %d "
#define IERR_ASYNCLOSE_ERR_OPEN_FOR_REWIND       11000, IERR_TYPE, "asynclos", "tape open error (%s) for rewind: %s"
#define IERR_ASYNIO_INVALID_FILENO               11000, IERR_TYPE, "asynio  ", "illegal hostfileno %ld"
#define IERR_ASYNIO_NOT_OPEN                     11000, IERR_TYPE, "asynio  ", "device not open %ld "
#define IERR_ASYNWAIT_INVALID_FILENO             11000, IERR_TYPE, "asynwait", "illegal hostfileno %ld"
#define IERR_ASYNWAIT_NO_REQUEST_ACTIVE          11000, IERR_TYPE, "asynwait", "no I/O requests active "
#define IERR_ASYNWAIT_WRONG_REQUEST              11000, IERR_TYPE, "asynwait", "awoke from wrong request %d"
#define IERR_ASYNWAIT_WRONG_REQUEST_CONT         11000, IERR_TYPE, "asynwait", "         correct request would be %d"
#define IERR_ASYNINIT_NOT_OPEN                   11000, IERR_TYPE, "asyninit", "device not open %ld "
#define IERR_ASYNINIT_WRONG_EVENT                11000, IERR_TYPE, "asyninit", "awoke from wrong event 0x%p "
#define IERR_ASYNINIT_WRONG_EVENT_CONT           11000, IERR_TYPE, "asyninit", "          correct event would be 0x%p "
#define IERR_ASYNINIT_WRONG_EVENT_CONT2          11000, IERR_TYPE, "asyninit", "                  wrong request %d "
#define IERR_ASYNINIT_WRONG_EVENT_CONT3          11000, IERR_TYPE, "asyninit", "               correct request would be %d "
#define IERR_ASYNINIT_FAILED                     11000, IERR_TYPE, "asyninit", "failed to get filetype %ld"
#define ERR_FIND_DEVNO_INSUFFICIENT_DEVICES      11000, ERR_TYPE,  "find_dev", "insufficient devices for %s "
#define IERR_FIND_DEVNO_ALREADY_ATTACHED         11000, IERR_TYPE, "find_dev", "device already attached '%s'"
#define ERR_DEV0_VATTACH_CANNOT_OPEN             11000, ERR_TYPE,  "d0_vatta", "Cannot open volume, %s "
#define ERR_DEV0_VATTACH_CANNOT_OPEN_CONT        11000, ERR_TYPE,  "d0_vatta", "Volume name '%s' "
#define ERR_DEV0_VATTACH_ERR_FCNTL               11000, ERR_TYPE,  "d0_vatta", "vattach (DEV0): error in fcntl, %s"
#define ERR_DEV0_VATTACH_DEVSPACE_LOCKED         11000, ERR_TYPE,  "d0_vatta", "volume '%s' locked by other process "
#define ERR_DEV0_VATTACH_DEVSPACE_NOT_LOCKABLE   11000, ERR_TYPE,  "d0_vatta", "volume '%s' not lockable "
#define ERR_DEV0_VATTACH_DEVSPACE_NOT_UNLOCKABLE 11000, ERR_TYPE,  "d0_vatta", "cannot unlock (fcntl), %s "
#define ERR_DEV0_VATTACH_ERR_FORK                11000, ERR_TYPE,  "d0_vatta", "Error during fork, %s "
#define ERR_DEV0_VATTACH_ERR_FCNTL2              11000, ERR_TYPE,  "d0_vatta", "error in fcntl, %s"
#define IERR_DEV0_VATTACH_ABEND                  11000, IERR_TYPE, "d0_vatta", "ABEND: dev0_vattach should never arrive here"
#define ERR_DEV0_ASYNOPEN_ERR_OPEN               11000, ERR_TYPE,  "d0_aopen", "Cannot open device, %s "
#define ERR_DEV0_ASYNOPEN_ERR_OPEN_CONT          11000, ERR_TYPE,  "d0_aopen", "Device name '%s' "
#define IERR_DEV0_ASYNOPEN_MAXBLOCK_CHANGED      11000, IERR_TYPE, "d0_aopen", "max_block_cnt changed to %d"
#define ERR_DEV0_ASYNOPEN_WRONG_FTYPE            11000, ERR_TYPE,  "d0_aopen", "filetype found %s, required %s"
#define ERR_DEV0_ASYNOPEN_ERR_STAT               11000, ERR_TYPE,  "d0_aopen", "check of file type failed: %s"
#define ERR_DEV0_ASYNOPEN_ERR_SEMAPHORE          11000, ERR_TYPE,  "d0_aopen", "Error during creating semaphore, %s "
#define ERR_DEV0_ASYNOPEN_ERR_THREAD_CREATE      11000, ERR_TYPE,  "d0_aopen", "Error during creating worker thread, %s "
#define IERR_EEVI_VBLOCKIO_TEST_IO_ERROR         11000, IERR_TYPE, "eevi_vbl", "User requested I/O error %s devno %ld page %ld "
#define IERR_DEVI_ASYNCNTL_INVALID_REQUEST       11000, IERR_TYPE, "di_acntl", "ABEND: devi_asyncntl: illegal request %d"
#define IERR_DEV0_VDETACH_WRONG_DEVI             11000, IERR_TYPE, "d0_vdeta", "ABEND: Termination of wrong DEVi, pid %ld"
#define IERR_DEV0_VDETACH_WRONG_DEVI_CONT        11000, IERR_TYPE, "d0_vdeta", "ABEND: Expected termination of pid    %ld"
#define IERR_DEVI_VDETACH_DEVI_DIED              11000, IERR_TYPE, "di_vdeta", "ABEND: DEVi Process died, pid = %ld"
#define IERR_DEVI_VDETACH_DEVI_DIED_CONT         11000, IERR_TYPE, "di_vdeta", "ABEND: status ( wait ) = 0x%x"
#define IERR_DEVI_VDETACH_ERR_CLOSE              11000, IERR_TYPE, "di_vdeta", "close error: %s"
#define IERR_WRONGCALL                           11000, IERR_TYPE, "wrongcal", "Wrongcall:====> REALLY BAD ERROR "
#define IERR_DEV0_MAIN_INVALID_REQUEST           11000, IERR_TYPE, "d0_main ", "illegal request %d"
#define IERR_DEVI_MAIN_INVALID_REQUEST           11000, IERR_TYPE, "di_main ", "ABEND: devi_main: illegal request %d"
#define IERR_DEVI_MAIN_ERR_SEMOP                 11000, IERR_TYPE, "di_main ", "semop error: %s"
#define ERR_PAGE0IO_DEVSP_MARKED_AS_BAD          11000, ERR_TYPE,  "page0io_", "volume marked as bad at pos %d"
#define IERR_PAGE0IO_INVALID_FORMAT              11000, IERR_TYPE, "page0io_", "Illegal file format "
#define IERR_PAGE0IO_INVALID_FORMAT_CONT         11000, IERR_TYPE, "page0io_", "blocksize wanted: %u, blocksize found: %u"
#define ERR_READBLCOK_ERR_MALLOC                 11000, ERR_TYPE,  "readblco", "malloc error, %s"
#define ERR_AIO_READBLOCK_ERR_READ               11000, ERR_TYPE,  "aio_read", "read error: %s"
#define ERR_AIO_READBLOCK_ERR_READ_SIZE          11000, ERR_TYPE,  "aio_read", "read lgt %u, wanted %u "
#define ERR_AIO_READBLOCK_ERR_READ2              11000, ERR_TYPE,  "aio_read", "Error on read, %s"
#define ERR_DEVIO_ERR_READ_FD                    11000, ERR_TYPE,  "devio   ", "read error (fd = %d): %s"
#define ERR_DEVIO_ERR_READ_FD_EOF                11000, ERR_TYPE,  "devio   ", "read error (fd = %d): at end of file (got %ld wanted %ld)"
#define ERR_DEVIO_ERR_WRITE_FD                   11000, ERR_TYPE,  "devio   ", "write error (fd = %d): %s"
#define ERR_DEVIO_ERR_WRITE_FD_DISK_FULL         11000, ERR_TYPE,  "devio   ", "write error (fd = %d): Sorry, but your file system may be full (wrote %ld wanted %ld)"
#define ERR_DEVIO_ERR_SEEK_FD                    11000, ERR_TYPE,  "devio   ", "seek (fd = %d) error: %s"
#define IERR_DEVOPEN_UNKNOWN_MODE                11000, IERR_TYPE, "devopen ", "Unknown file MODE '%s'"
#define ERR_DEVOPEN_ERR_OPEN                     11000, ERR_TYPE,  "devopen ", "dev_open, error during open; %s"
#define IERR_DEV0_RECEIVE_ERR_SEMOP              11000, IERR_TYPE, "d0_recei", "ABEND: dev0_receive: semop error: %s "
#define IERR_DEV0_RECEIVE_ERR_SEMOP2             11000, IERR_TYPE, "d0_recei", "semop error: %s"
#define ERR_DIOSIZE_ERR_OPEN                     11000, ERR_TYPE,  "diosize ", "Cannot open volume, %s "
#define ERR_DIOSIZE_ERR_FSTAT                    11000, ERR_TYPE,  "diosize ", "Error during fstat: %s "
#define ERR_DIOSIZE_ERR_SIZE                     11000, ERR_TYPE,  "diosize ", "Illegal volume size %ld "
#define ERR_DIOSIZE_ERR_SIZE2                    11000, ERR_TYPE,  "diosize ", "Illegal device size %ld "
#define ERR_RIOSIZE_ERR_OPEN                     11000, ERR_TYPE,  "riosize ", "Could not open raw device '%s': %s"
#define ERR_RIOSIZE_ERR_SIZE                     11000, ERR_TYPE,  "riosize ", "illegal devsize = %d"
#define INFO_RIOSIZE_CALCULATED_SIZE             11000, INFO_TYPE, "riosize ", "calculated devsize = %d"
#define ERR_RIOSIZE_ERR_FSTAT                    11000, ERR_TYPE,  "riosize ", "SYSTEM ERROR fstat failed, %s"
#define ERR_NEWDEVSIZE_INVALID_SIZE              11000, ERR_TYPE,  "newdevsi", "Illegal device size %ld "
#define ERR_NEWDEVSIZE_ERR_MALLOC                11000, ERR_TYPE,  "newdevsi", "cannot malloc, %s "
#define ERR_NEWDEVSIZE_ERR_READ                  11000, ERR_TYPE,  "newdevsi", "Could not read, %s"
#define ERR_NEWDEVSIZE_ERR_WRITE                 11000, ERR_TYPE,  "newdevsi", "Could not write, %s"
#define ERR_NEWDEVSIZE_ERR_TRUNCATE              11000, ERR_TYPE,  "newdevsi", "Could not truncate, %s"
#define ERR_NEWDEVSIZE_TOO_SMALL                 11000, ERR_TYPE,  "newdevsi", "last page too small %d"
#define ERR_NEWDEVSIZE_ERR_SEEK                  11000, ERR_TYPE,  "newdevsi", "lseek error %s"
#define ERR_CLOSESINGLE_ERR_CLOSE                11000, ERR_TYPE,  "closesin", "close error: %s"
#define IERR_SINGLEIO_INVALID_DEVNO              11000, IERR_TYPE, "singleio", "I/O on unattached devno %d"
#define ERR_SINGLEIO_FILE_LIMIT                  11000, ERR_TYPE,  "singleio", "single attach file limit: %s"
#define ERR_SINGLEIO_ERR_OPEN                    11000, ERR_TYPE,  "singleio", "open error devno %d: %s"
#define IERR_SINGLEIO_TEST_IO_ERROR              11000, IERR_TYPE, "singleio", "User requested I/O error %s devno %ld page %ld "
#define ERR_SINGLEIO_ERR_SEEK_CONT               11000, ERR_TYPE,  "singleio", "devno is %d"
#define IERR_TODEV0_REQUEST_NOT_FREE             11000, IERR_TYPE, "todev0_r", "io_request is not free !!!"
#define IERR_TODEVI_REQUEST_NOT_FREE             11000, IERR_TYPE, "todevi_r", "io_request is not free !!!"
#define IERR_VALIDFILE_INVALID_FILENO            11000, IERR_TYPE, "validfil", "%s: illegal hostfileno %ld"
#define IERR_VALIDFILE_NOT_OPEN                  11000, IERR_TYPE, "validfil", "%s: device not open %ld "
#define ERR_AMALLOC_ERR_MALLOC                   11000, ERR_TYPE,  "amalloc ", "malloc error %d bytes, %s"
#define ERR_SINGLE_IOERR                         11000, ERR_TYPE,  "single_i", "single_blockio: AIO devno is %d"
#define ERR_VMARK_BAD_DEV_NO_MEMORY              11000, ERR_TYPE,  "vmarkbad", "ABEND: malloc failed for %d bytes, %s"
#define ERR_DEVSP_MARKED_AS_BAD                  11000, ERR_TYPE,  "vmarkbad", "pos %d marked as bad"
#define ERR_PAGE_0_IO_NO_MEMORY                  11000, ERR_TYPE,  "page0_io", "ABEND: malloc failed for %d bytes, %s"
#define INFO_DUMP_RTE_DONE                       11987, INFO_TYPE, "dump_rte", "rtedump written to file '%s'"
#define WRN_DUMP_RTE_OPEN                        11987, WRN_TYPE,  "dump_rte", "can't open rte_dumpfile <%s>: %s"
#define WRN_DUMP_RTE_FAILED                      11987, INFO_TYPE, "dump_rte", "'%s' failed"
#define INFO_ASYNC_OPEN_USER_ABORT               11987, INFO_TYPE, "ASYNOPEN", "dev0_vasynopen aborted by user"
#define ERR_DEV0_DETACH_BEFORE_SLEEP             11987, ERR_TYPE,  "dev0deta", "before sleep devi state %ld io_work 0x%p io_request 0x%p"
#define ERR_DEV0_DETACH_AFTER_SLEEP              11987, ERR_TYPE,  "dev0deta", "after sleep devi state %ld io_work 0x%p io_request 0x%p"
#define ERR_IO_READBLOCK_ERR_READ_SIZE_TRY_AGAIN 11987, ERR_TYPE,  "IO/READ ", "read error: rc=%u, wanted=%u, '%s', try again"
#define ERR_VXXXX_WRONG_TASK                     11025, ERR_TYPE,  "TASKING ", "'%s' from wrong task T%d"

#define ERR_PIPECHECK_PATH                       11100, ERR_TYPE,  "PIPE_IO ", "Path %s%s too long. Pipe input detection cannot be tested"
#define ERR_PIPECHECK_CREATE                     11101, ERR_TYPE,  "PIPE_IO ", "Creating pipe %s failed:%s"
#define ERR_PIPECHECK_OPEN                       11102, ERR_TYPE,  "PIPE_IO ", "Opening pipe %s for write failed:%s"
#define ERR_PIPECHECK_WRITE                      11103, ERR_TYPE,  "PIPE_IO ", "Writing to pipe %s failed"
#define ERR_PIPECHECK_REOPEN                     11104, ERR_TYPE,  "PIPE_IO ", "Reopening pipe %s for read failed:%s"
#define ERR_VVECTORIO_VDETACH_PENDING            11105, ERR_TYPE,  "VECTORIO", "Detach already pending for devno %d"
#define ERR_VBLOCKIO_VDETACH_PENDING             11106, ERR_TYPE,  "VBLOCKIO", "Detach already pending for devno %d"

/* ============================ ven54async ============================ */

#define ERR_SINGLEIO_ERR_READ                    11000, ERR_TYPE,  "singleio", "single_blockio: AIO  read_error! %s"
#define ERR_SINGLEIO_ERR_WRITE                   11000, ERR_TYPE,  "singleio", "single_blockio: AIO write_error! %s"
#define IERR_SINGLEIO_ERR_IO_CONT                11000, IERR_TYPE, "singleio", "single_blockio: errno = %d, return = %d"
#define ERR_AIO_ERRNO                            11000, ERR_TYPE,  "singleio", "aio_error() returned errno (%d)"

#define ERR_AIO_SIG_BAD_SIGNO                    11000, ERR_TYPE,  "aio_sig ", "bad sig (%d) catched for aio completion"
#define ERR_AIO_SIG_BAD_SI_CODE                  11000, ERR_TYPE,  "aio_sig ", "bad si_code (%d), SI_ASYNCIO (%d) wanted"
#define ERR_AIO_SIG_NO_TCB                       11000, ERR_TYPE,  "aio_sig ", "sig for aio completion catched but no Task given"
#define ERR_AIO_SIG_FOR_NONWAITING_TASK          11000, ERR_TYPE,  "aio_sig ", "sig aio completion for nonwaiting Task T%d"
#define ERR_AIO_SIG_BUT_EINPROGRESS              11000, ERR_TYPE,  "aio_sig ", "sig aio completion for Task T%d, but EINPROGRESS"

/* ============================ ven55c ================================ */

#define IERR_EXCL_TW_LOOPS_INDEF     11010,IERR_TYPE,"tas_coll","%s loops indefinitely on region %d"
#define IERR_SECOND_REGION_REQUEST   11011,IERR_TYPE,"vbegexcl","%s: second request on region: %u (%s)"
#define IERR_VENDEXCL_WITHOUT_VBEG   11012,IERR_TYPE,"vendexcl","without vbegexcl from T%d on region %d (%s)"
#define IERR_TOO_MANY_REGIONS_REQ    11013,IERR_TYPE,"vsemaphi","too many regions requested"

/* ============================ ven56c ================================ */

#define IERR_WRONG_SUSPEND_LEVEL     11014,IERR_TYPE,"vsuspend","wrong suspend level %d for T%d"
#define IERR_SUSPEND_WRONG_TASK      11015,IERR_TYPE,"vsuspend","T%d tries to suspend other task (T%d)"
#define IERR_SUSPEND_WRONG_REASON    11016,IERR_TYPE,"vsuspend","reason (%d) for task T%ld out of range(%d)"
#define IERR_VSUSPEND_WRONG_EVENT    11017,IERR_TYPE,"vsuspend","T%d awoke from wrong event"
#define IERR_VSUSPEND_ERR_EVENT_INFO 11017,IERR_TYPE,"vsuspend","queue 0x%p, event 0x%p, event->req_type %d"
#define IERR_VSUSPEND_WRONG_REQ      11018,IERR_TYPE,"vsuspend","T%d awoke by wrong request (%d)"
#define IERR_VRESUME_MYSELF          11019,IERR_TYPE,"vresume ","T%d resume myself"
#define IERR_VRESUME_NOT_SUSP_STATE  11020,IERR_TYPE,"vresume ","T%d in state %d resumed by T%ld"
#define IERR_VWAIT_WRONG_TASK        11021,IERR_TYPE,"vwait   ","T%d tries to vwait other task (T%d)"
#define IERR_VWAIT_WRONG_EVENT       11022,IERR_TYPE,"vwait   ","T%d awoke from wrong event"
#define IERR_VWAIT_WRONG_REQ         11023,IERR_TYPE,"vwait   ","T%d awoke by wrong request (%d)"
#define IERR_VWAIT_ILL_BOOL          11024,IERR_TYPE,"vwait   ","illegal bool: %d, T%d"
#define IERR_VSLEEP_WRONG_TASK       11025,IERR_TYPE,"vsleep  ","T%d tries to vsleep other task (T%d)"
#define IERR_VSLEEP_WRONG_EVENT      11026,IERR_TYPE,"vsleep  ","T%d awoke from wrong event"
#define IERR_VSLEEP_WRONG_REQ        11027,IERR_TYPE,"vsleep  ","T%d awoke by wrong request (%d)"

/* ============================ ven57c ================================ */

#define IERR_WRONG_VMNEWBUF_COUNT    11028,IERR_TYPE,"vmnewbuf","requested count '%d' too large"
#define IERR_VMALLOC_MALLOC_SYS_ERR  11029,IERR_TYPE,"vmalloc ","malloc error, %s"
#define IERR_VMALLOC_SBRK_VAL        11029,IERR_TYPE,"vmalloc ","sbrk 0x%p"
#define IERR_VMALLOC_ULIMIT_ERR      11029,IERR_TYPE,"vmalloc ","can't get ulimit, %s"
#define IERR_VMALLOC_SEGEND_ADDR     11029,IERR_TYPE,"vmalloc ","Segment end adress = %x"
#define IERR_VREALLOC_MALLOC_SYS_ERR 11030,IERR_TYPE,"vrealloc","malloc error, %s"
#define IERR_VREALLOC_ULIMIT_ERR     11030,IERR_TYPE,"vrealloc","can't get ulimit, %s"
#define IERR_VREALLOC_SEGEND_ADDR    11030,IERR_TYPE,"vrealloc","Segment end adress = %x"

/* ============================ ven58c ================================ */

#define IERR_TRACEFILE_OPEN_ERR      11031,IERR_TYPE,"vdiagini","cannot open trace volume '%s': %s"
#define IERR_TRACEFILE_WRITE_ERR     11032,IERR_TYPE,"vdiagini","trace volume fwrite error (%d bytes): %s" 

/* ============================ ven62c ================================ */

#define IERR_INIT_STACK_FAILED       11033,IERR_TYPE,"vcreate ","init stack for T%d failed"

/* ============================ ven63c ================================ */

#define IERR_NETW_NOT_RESTARTED      11034,IERR_TYPE,"distrib ","'%s' network not restarted"
#define IERR_NETW_COMSEG_CORRUPT     11035,IERR_TYPE,"distrib ","e63_open: corrupted comseg"
#define IERR_NETW_CANT_CONNECT_TO    11036,IERR_TYPE,"distrib ","e63_loc_con: cannot connect to '%s': %s"
#define IERR_NETW_SERVER_REJECT_CONN 11037,IERR_TYPE,"distrib ","e63_loc_con: server rejected connection: %d"
#define IERR_NETW_MESSCALASS_EXP     11038,IERR_TYPE,"distrib ","e63_loc_con: server messclass %d expected %d"
#define IERR_NETW_REFERENCE_EXP      11039,IERR_TYPE,"distrib ","e63_loc_con: server reference %d expected %d"
#define IERR_NETW_SERVICE_EXP        11040,IERR_TYPE,"distrib ","e63_loc_con: server service %d expected %d"
#define IERR_NETW_DBNAME_EXP         11041,IERR_TYPE,"distrib ","e63_loc_con: server dbname '%s' expected '%s'"
#define IERR_NETW_RET_NO_SHMID       11042,IERR_TYPE,"distrib ","e63_loc_con: server didn't return shmid"
#define IERR_NETW_RET_NO_OFFSET      11043,IERR_TYPE,"distrib ","e63_loc_con: server didn't return offset"
#define IERR_NETW_DATA_SIZE_ERROR    11044,IERR_TYPE,"distrib ","e63_loc_con: server maxdata %ld expected <=%ld"
#define IERR_NETW_DIFF_PACKET_SIZE   11044,IERR_TYPE,"distrib ","e63_loc_con: different packet sizes unsupported"
#define IERR_NETW_WRONG_PACK         11045,IERR_TYPE,"distrib ","e63_loc_con: wrong packet values"
#define IERR_NETW_WRONG_PACK_VALUE   11045,IERR_TYPE,"distrib ","        pid %ld ref %d cnt %d"
#define IERR_NETW_WRONG_PACK_WANTED  11045,IERR_TYPE,"distrib ","wanted: pid %ld ref %d cnt %d"
#define IERR_NETW_PATHID_ERR         11047,IERR_TYPE,"distrib ","%s %s %d"
#define IERR_NETW_ILL_REQ_ADDR       11048,IERR_TYPE,"distrib ","e63_request: illegal request pointer 0x%p"
#define IERR_NETW_ILL_REQ_ADDR_EXP   11048,IERR_TYPE,"distrib ","                            expected 0x%p"
#define IERR_NETW_ILL_REQ_LEN        11049,IERR_TYPE,"distrib ","e63_request: illegal request length %d"
#define IERR_NETW_UNBAL_REQ_REPL     11050,IERR_TYPE,"distrib ","e63_request: unbalanced request/reply"
#define IERR_NETW_UNBAL_REQ_REPL_I   11050,IERR_TYPE,"distrib ","             cliflg,srvflg %d,%d"
#define IERR_NETW_REQ_CONN_BROKEN    11051,IERR_TYPE,"distrib ","e63_request: connection broken, state %d"
#define IERR_NETW_RCV_ILL_IDX        11052,IERR_TYPE,"distrib ","e63_receive: illegal conn_idx %d"
#define IERR_NETW_RCV_TSK_NOT_CONN   11053,IERR_TYPE,"distrib ","e63_receive: ERROR: T%d,%d not connected"
#define IERR_NETW_REQ_ILL_REF        11054,IERR_TYPE,"distrib ","e63_request: ill pid/ref %ld/%d"
#define IERR_NETW_REQ_WANTED_REF     11054,IERR_TYPE,"distrib ","             should be   %ld/%d"
#define IERR_NETW_RCV_ILL_REF        11054,IERR_TYPE,"distrib ","e63_receive: ill pid/ref %ld/%d"
#define IERR_NETW_RCV_WANTED_REF     11054,IERR_TYPE,"distrib ","             should be   %ld/%d"
#define IERR_NETW_RCV_CONN_BROKEN    11055,IERR_TYPE,"distrib ","e63_receive: connection broken, state %d class %d"
#define IERR_PATHID_ERR              11056,IERR_TYPE,"distrib ","%s invalid pathid %d"
#define IERR_CLR_PATHID_NOT_CONN     11056,IERR_TYPE,"distrib ","e63_clear: path not connected %d"
#define IERR_NETW_ILLEGAL_PROTOCAL   -11626, IERR_TYPE, "distrib ", "illegal protocol type %d"

/* ============================ ven69c ================================ */

#define IERR_ACCOUNT_WRONG_TASK      11057,IERR_TYPE,"account ","ABEND: T%d tried to %s other task T%ld"

/* ============================ ven70c ================================ */

#define IERR_INITSHM_TOO_MANY_SHMIDS 11058,IERR_TYPE,"init_shm","ABEND:not enough space for kgs shmids!"
#define IERR_INITSHM_IDS_AVAIL_WANT  11058,IERR_TYPE,"init_shm","available: %d, wanted : %d"
#define IERR_INITSHM_TSK_NOT_LSTTSK  11059,IERR_TYPE,"init_shm","ABEND: tcb != last task 0x%p"
#define IERR_INITSHM_LSTTSK_INFO     11059,IERR_TYPE,"init_shm","              last task 0x%p"
#define IERR_INITSHM_PAS_OVERLAP_KGS 11060,IERR_TYPE,"init_shm","ABEND: Pascal globals overlap KGS 0x%p"
#define IERR_INITSHM_TO_MANY_DEVSP   11061,IERR_TYPE,"init_shm","too many volumes !"
#define IERR_INITSHM_TCB_AFTER_LST   11062,IERR_TYPE,"init_shm","ABEND: tcb after last task 0x%p"
#define IERR_INITSHM_TCB_AFTER_LST_I 11062,IERR_TYPE,"init_shm","                 last task 0x%p"
#define IERR_INITSHM_PCB_AFTER_LST   11063,IERR_TYPE,"init_shm","ABEND: pcb ptr after last kp 0x%p"
#define IERR_INITSHM_PCB_AFTER_LST_I 11063,IERR_TYPE,"init_shm","                     last kp 0x%p"

/* ============================ ven71c ================================ */

#define IERR_DISP_ENQUE_WRONG_UKP    11028,IERR_TYPE,"dispatch","%s tried to enqueue resource availability to other process"
#define IERR_DISP_REMOTEFUNC_NOT_SET 11043,IERR_TYPE,"dispatch","remote func for T%d (param %d) not set"
#define INFO_DISP_SHUTKILL_STATE1    11598, INFO_TYPE, "dispatcher", "state %d before shutkill(1)"
#define INFO_DISP_SHUTKILL_STATE2    11598, INFO_TYPE, "dispatcher", "state %d before shutkill(2)"

/* ============================ ven72c ================================ */
#define WRN_WAKE_IGNORING_ERROR      11987, WRN_TYPE, "TASKING ", "ignoring wake semid %d semsnd error, %s"

/* ============================ ven73c ================================ */

#define IERR_QUE_NON_FREEL_ELEMENT   11029,IERR_TYPE,"del_enqu","ABEND: freeing non-freelist element"
#define IERR_QUE_FREEL_FIRST_LAST    11029,IERR_TYPE,"del_enqu","       elem 0x%p, first 0x%p, last 0x%p"
#define IERR_QUE_FREEING_TWICE       11029,IERR_TYPE,"del_enqu","ABEND: e73_dl_enqu: freeing twice !!!" 

/* ============================ ven75c ================================ */

#define IERR_CONNECT_ILL_IDX         11030,IERR_TYPE,"COMMUNIC","e75_connect: got illegal index T%d,%d !"
#define IERR_CONNECT_UNEXP_STATE     11031,IERR_TYPE,"COMMUNIC","e75_connect: unexpected state %d"
#define IERR_RELEASE_ILL_IDX         11032,IERR_TYPE,"COMMUNIC","e75_rel_req: got illegal index T%d,%d !"

/* ============================ ven76c ================================ */

#define IERR_CLEARLOCK_OWNER         11033,IERR_TYPE,"clr_lock","ABEND : ALONE Error (e76_clearlock), owner is %ld"

/* ============================ ven79c ================================ */

#define IERR_ILL_XPARAMTYPE_FOR      11034,IERR_TYPE,"readpara","ABEND: illegal param type for '%s'"
#define IERR_ILL_XPARAMTYPE_EXP_LEN  11034,IERR_TYPE,"readpara","       param length %d, expected %d"
#define IERR_ILL_XPARAMTYPE          11035,IERR_TYPE,"readpara","Illegal xp_type %d" 

/* ============================ ven83c ================================ */

#define IERR_TIMER_MISS_COORD        11036,IERR_TYPE,"timer   ","ABEND: is missing COORDINATOR"

/* ============================ ven84c ================================ */

#define IERR_REQUESTOR_CONN_NULL_TSK 11037,IERR_TYPE,"COMMUNIC","tried to connect NULL task"
#define IERR_REQUESTOR_CONN_WRONG_ST 11038,IERR_TYPE,"COMMUNIC","connecting wrong task state %d"
#define IERR_REQUESTOR_CANCEL_ILL_T  11039,IERR_TYPE,"COMMUNIC","Cancel illegal task id T%d"
#define IERR_REQUESTOR_ILL_CON_STRAT 11040,IERR_TYPE,"COMMUNIC","invalid connect strategy %d"
#define IERR_REQUESTOR_TEST_ILL_TASK 11041,IERR_TYPE,"COMMUNIC","e84_test_application no user/util task"

/* ============================ ven79c === Fortsetzung IERR =========== */

#define IERR_GATEWAY_PARM_FOUND      11042,IERR_TYPE,"readpara","GATEWAY configuration found - KERNEL can't be started"

/************************************ E R R ***********************************/

#define ERR_TASK_INDEX_OUT_OF_RANGE  11987,ERR_TYPE,"rteentry",": task index %d out of range"
/* ============================ ven51c ================================ */

#define ERR_PARAM_AND_KERNEL_INCOMP  11199,ERR_TYPE,"vversion","Incompatible version of running kernel and param file!\n\tRunning kernel version is:  %s\n\tParam   kernel version is:  %s"
#define ERR_ABORT_CALLED             11200,ERR_TYPE,"dbcrash ","Emergency Shutdown"
#define ERR_ABORT_CALLED_FROM        11200,ERR_TYPE,"dbcrash ","Emergency Shutdown from T%d"

#define ERR_RESUME_IOTHREAD          11201,ERR_TYPE,"TASKING ","Could not resume IO thread: '%s', rc = %d"
#define ERR_SUSPEND_IOTHREAD         11202,ERR_TYPE,"TASKING ","Could not resume IO thread: '%s', rc = %d"
#define ERR_CREATING_IOTHREAD        11203,ERR_TYPE,"TASKING ","Could not create thread: '%s', rc = %d"

/* ============================ ven58c ================================ */

#define ERR_INCORRECT_TR_DEVSIZE     11201,ERR_TYPE,"vdiagini","size incorrect of kernel trace file %s"
#define ERR_VDEVSIZE_UNSUCCSESSFUL   11202,ERR_TYPE,"vdiagini","vdevsize of kernel trace file unsuccsessful"

/* ============================ ven59c ================================ */

#define ERR_VEXEC_WRONG_ENVVAR_SYN   11203,ERR_TYPE,"vexec   ","syntax is '${envvar}', wrong in '%s'"
#define ERR_VEXEC_ENVVAR_NOT_FOUND   11204,ERR_TYPE,"vexec   ","'${%s}' not found"
#define ERR_VEXEC_COMMAND_TOO_LONG   11205,ERR_TYPE,"vexec   ","command name too long, truncated '%s'"
#define ERR_VEXEC_ABSOLUT_PROG_EXP   11206,ERR_TYPE,"vexec   ","prog name '%s' must be absolute"
#define ERR_VEXEC_PROG_NOT_FOUND     11207,ERR_TYPE,"vexec   ","program '%s' not found: %s" 
#define ERR_VEXEC_PROG_NOT_EXEC      11208,ERR_TYPE,"vexec   ","'%s' not executable for public"
#define ERR_VEXEC_SETIUD_NOT_SET     11209,ERR_TYPE,"vexec   ","setuid bit of '%s' must be set"
#define ERR_VEXEC_FORK_ERROR         11210,ERR_TYPE,"vexec   ","Cannot fork process, %s" 
#define ERR_VEXEC_WAIT_FAILED        11211,ERR_TYPE,"vexec   ","wait failed , %s"
#define ERR_VEXEC_CANT_EXECUTE_PROC  11212,ERR_TYPE,"vexec   ","cannot execute process"
#define ERR_VEXEC_EXEC_ERROR         11213,ERR_TYPE,"vexec   ","error on exec, %s"

/* ============================ ven62c ================================ */

#define ERR_NO_FREE_SERVER_TASK      11214,ERR_TYPE, "vcreate","Not enough server tasks"
#define ERR_DYNAMIC_STACK_NOT_SET    11215,ERR_TYPE, "vcreate","dynamic task stack not set!"

/* ============================ ven63c ================================ */

#define ERR_ILLEGAL_PROTOCOL         11626,ERR_TYPE, "distrib ","illegal protocol type %d"
#define ERR_VNOPEN_TOO_MANY_CONN     11216,ERR_TYPE, "distrib ","e63_open: too many sessions"

/* ============================ ven66c ================================ */

#define ERR_VFOPEN_ERR               11217,ERR_TYPE, "vfopen  ","cannot open '%s': %s"
#define ERR_VFOPEN_DUP_ERR           11218,ERR_TYPE, "vfopen  ","cannot dup 0: %s"
#define ERR_VFOPEN_LCK_ERR           11219,ERR_TYPE, "vfopen  ","cannot lock: %s"
#define ERR_VFOPEN_PAGE_0_WRITE      11220,ERR_TYPE, "vfopen  ","cannot write: %s"
#define ERR_VFOPEN_PAGE_0_SIZE       11221,ERR_TYPE, "vfopen  ","EOF after %d bytes"
#define ERR_VFWRITE_ERR              11222,ERR_TYPE, "vfwrite ","cannot write : %s"
#define ERR_VFWRITE_EOF              11223,ERR_TYPE, "vfwrite "," EOF after %d bytes"

/* ============================ ven67c ================================ */

#define ERR_TC_TOKEN_TO_LONG         11224,ERR_TYPE, "tc_analy","ABEND: token '%s' too long (max %d chrs)"
#define ERR_TC_ILL_NUMBER            11225,ERR_TYPE, "tc_analy","ABEND: illegal %s number '%s'"
#define ERR_TC_MISSING_CNT           11226,ERR_TYPE, "tc_analy","ABEND: missing count on first '%s'"
#define ERR_TC_NOT_ONE               11227,ERR_TYPE, "tc_analy","ABEND: not alone '%s'"
#define ERR_TC_UNKNOWN_TOKEN         11228,ERR_TYPE, "tc_analy","ABEND: unknown token '%s'"
#define ERR_TC_ILL_CHAR              11229,ERR_TYPE, "tc_analy","ABEND: illegal char '%c'"
#define ERR_TC_MISSING_TASKS         11230,ERR_TYPE, "tc_analy","ABEND: not all task types present"
#define ERR_TC_ONLY_CMPR_OR_EQU      11231,ERR_TYPE, "tc_analy","ABEND: COMPRESS and EQUALIZE specified"
#define ERR_TC_REALLOC_ERR           11232,ERR_TYPE, "tc_analy","realloc error, %s"
#define ERR_TC_TOKEN_MULTI_DEF       11233,ERR_TYPE, "tc_analy","ABEND: multiply defined '%s'"

/* ============================ ven60c ================================ */

#define ERR_OPMSG_OPEN               11234,ERR_TYPE, "opmsg   ","cannot open %s '%s'"
#define ERR_OPMSG_DIAG_MV_ERR        11235,ERR_TYPE, "opmsg   ","cannot move old diag file, %s"
#define ERR_OPMSG_DIAG_NO_REG_FILE   11236,ERR_TYPE, "opmsg   ","diag file must be regular file"
#define ERR_OPMSG_REDIRECT_FAILED    11237,ERR_TYPE, "opmsg   ","cannot redirect %s to '%s'"

/* ============================ ven70c ================================ */

#define ERR_INITSHM_MALLOC_FAILED    11238,ERR_TYPE, "init_shm","malloc for %d shmids failed '%s'" 
#define ERR_INITSHM_BAD_TC           11239,ERR_TYPE, "init_shm","bad taskcluster %s"
#define ERR_TOO_MANY_UKPS            11240,ERR_TYPE, "init_shm","ABEND: maximum UKTs allowed: %d"
#define ERR_INITSHM_CR_KGS_ERR       11241,ERR_TYPE, "init_shm","ABEND: cannot create kernel shared section"
#define ERR_INITSHM_LOCK_ERR         11242,ERR_TYPE, "init_shm","ABEND: cannot lock kernel shared section, ret = %d"
#define ERR_INITSHM_CANT_OPEN_IDFILE 11243,ERR_TYPE, "init_shm","ABEND: cannot open idfile of kgs"
#define ERR_INITSHM_DBNAME_TO_LONG   11244,ERR_TYPE, "init_shm","ABEND: DBNAME too long, allowed are %d characters"
#define ERR_NOT_ENOUGH_MEMORY        11245,ERR_TYPE, "init_shm","Not enough memory for %s wanted : %ld kB!"

/* ============================ ven71c ================================ */

#define ERR_DISP_SEMOP_ERR           11245,ERR_TYPE, "dispatch","ABEND: dispatcher semop error %s"
    
/* ============================ ven72c ================================ */

#define ERR_WAKE_SEMSND_ERR          11246,ERR_TYPE, "wake    ","ABEND: semsnd (semid %d) error: %s"

/* ============================ ven74c ================================ */

#define ERR_INITCOM_CREATE_BIGCOM    11247,ERR_TYPE,"COMMUNIC","ABEND: cannot create shared memory (BIG_COMSEG): %s"
#define ERR_INITCOM_ATTACH_BIGCOM    11248,ERR_TYPE,"COMMUNIC","UKT cannot attach shared memory (BIG_COMSEG): %s" 
#define ERR_INITCOM_ATT_ADDR_BIGCOM  11249,ERR_TYPE,"COMMUNIC","UKT attached BIG_COMSEG at wrong address 0x%p"
#define ERR_INITCOM_WANT_ADDR_BIGCOM 11249,ERR_TYPE,"COMMUNIC","                                 wanted  0x%p"
#define ERR_INITCOM_OPEN_NETPIPE     11250,ERR_TYPE,"COMMUNIC","UKT: cannot open NET pipe '%s': %s"

/* ============================ ven75c ================================ */
#define ERR_DISTRIB_PIPE_WRITE       11251,ERR_TYPE,"COMMUNIC","e75_wake: pipe write error: %s"
#define ERR_DISTRIB_PIPE_WRITE_SZ    11252,ERR_TYPE,"COMMUNIC","e75_wake: pipe write size %d"
#define ERR_DISTRIB_WAKE_SEMOP       11253,ERR_TYPE,"COMMUNIC","e75_wake: semop error %s"

/* ============================ ven76c ================================ */

#define ERR_INTERLOCK_CR_SEM         11254,ERR_TYPE,"int_lock","Cannot create inter_sema, %s"
#define ERR_INTERLOCK_INIT_SEM       11255,ERR_TYPE,"int_lock","Cannot init inter_sema, %s"

/* ============================ ven79c ================================ */

#define ERR_CANT_READ_PARAM_FILE     11256,ERR_TYPE,"readpara","Could not read xparam file: %s"
#define ERR_PARAM_MISSING_FILENAME   11257,ERR_TYPE,"readpara","ABEND: One or more of filenames needed are not initialized!"
#define ERR_PARAM_MISSING_NAMES      11257,ERR_TYPE,"readpara","       (dumpfile, tracefile, sysdevice, logdevice, diagfile)"
#define ERR_PARAM_TOO_MANY_BUPDEVS   11258,ERR_TYPE,"readpara","ABEND: NO_OF_BACKUP_DEVS greater %d"
#define ERR_PARAM_DYNDATA_TOO_SMALL  11259,ERR_TYPE,"readpara","ABEND: SHAREDDYNDATA too small (%d)" 
#define ERR_PARAM_DYNDATA_MIN_REQU   11259,ERR_TYPE,"readpara","              minimum required: %d"
#define ERR_PARAM_COLDPASSWD_INCOMPL 11260,ERR_TYPE,"readpara","ABEND: COLDPASSWORD incomplete"

/* ============================ ven82c ================================ */

#define ERR_DEATH_CAUGHT_SIG         11261,ERR_TYPE,"death   ","ABEND: DEATH caught signal"
#define ERR_DEATH_SEMOP_ERR          11262,ERR_TYPE,"death   ","ABEND: semop error, %s"
#define ERR_DEATH_FOUND_COORD_DIED   11263,ERR_TYPE,"death   ","ABEND: found COORDINATOR died"

/* ============================ ven83c ================================ */

#define ERR_TIMER_SIGN_STARTER       11264,ERR_TYPE,"timer   ","signalling starter error: %s"
#define ERR_TIMER_SIGN_STARTER_INF   11264,ERR_TYPE,"timer   ","return code is %d, starter pid is: %ld"
#define ERR_TIMER_SEMOP_RCV_ERR      11265,ERR_TYPE,"timer   ","ABEND: semop (receive) error, %s"
#define ERR_TIMER_SIGNAL_ERROR       11266,ERR_TYPE,"timer   ","ABEND: signal error, %s"

/* ============================ ven84c ================================ */

#define ERR_REQUESTOR_CR_FIFO        11267,ERR_TYPE,"COMMUNIC","ABEND: cannot create request fifo"
#define ERR_REQUESTOR_OPEN_REQ_FIFO  11268,ERR_TYPE,"COMMUNIC","ABEND: cannot open request fifo: %s"
#define ERR_REQUESTOR_NO_MORE_DUMP   11269,ERR_TYPE,"COMMUNIC","One more Dump request"
#define ERR_REQUESTOR_OPEN_REPL_FIFO 11270,ERR_TYPE,"COMMUNIC","cannot open reply fifo '%s': %s"
#define ERR_REQUESTOR_ATT_SHM        11271,ERR_TYPE,"COMMUNIC","cannot attach shmid %d: %s"
#define ERR_REQUESTOR_REL_SEMID      11272,ERR_TYPE,"COMMUNIC","cannot release semid %d: %s"
#define ERR_REQUESTOR_FREE_SEMID     11273,ERR_TYPE,"COMMUNIC","cannot free semid %d: %s"

/* ============================ ven85c ================================ */

#define ERR_INCOM_CONNECT_BAD_RECV   11999,ERR_TYPE,"distrib ","en85_incom_conn: recv_conpkt: %s \n"
#define ERR_CONNECT_REPLAY_BAD_SEND  11999,ERR_TYPE,"distrib ","en85_conn_repl: send_conpkt: %s \n"
#define ERR_CONNECT_PEER_BAD_SEND    11999,ERR_TYPE,"distrib ","en85_conn_peer: send_conpkt: %s \n"
#define ERR_CONNECT_PEER_BAD_RECV    11999,ERR_TYPE,"distrib ","en85_conn_peer: recv_conpkt: %s \n"
#define ERR_DIST_ALLOC_CACHE         11987,ERR_TYPE,"distrib ","cannot allocate cache"
#define ERR_DIST_ALLOC_TEMP_PACKET   11987,ERR_TYPE,"distrib ","NETWORK: cannot allocate temp packet"
#define ERR_DIST_SET_SIGNAL_HANDLER  11987,ERR_TYPE,"distrib ","cannot set signal handler: %s"
#define ERR_DIST_OPEN_UKT_PIPE       11987,ERR_TYPE,"distrib ","NET: cannot open UKT pipe '%s': %s"
#define ERR_DIST_ATTACH_UKT_COMSEG   11987,ERR_TYPE,"distrib ","NETWORK: cannot attach UKT's big comseg"
#define ERR_DIST_OPEN_SOCKET         11987,ERR_TYPE,"distrib ","socket open error: %s"
#define ERR_DIST_BIND_SOCKET         11987,ERR_TYPE,"distrib ","socket bind error: %s"
#define ERR_DIST_LISTEN_SOCKET       11987,ERR_TYPE,"distrib ","socket listen error: %s"
#define ERR_DIST_GETSOCKNAME         11987,ERR_TYPE,"distrib ","getsockname error: %s"
#define ERR_DIST_CREATE_SOCKET       11987,ERR_TYPE,"distrib ","NETWORK failed to create socket, exiting %d"

/* ============================ ven87c ================================ */

#define ERR_CONSOLE_FIFO_OPEN        11267,ERR_TYPE,"console ","ABEND: diag fifo open error, %s"
#define ERR_CONSOLE_FIFO_READ        11268,ERR_TYPE,"console ","fifo read error, %s"
#define ERR_CONSOLE_FIFO_CREATE      11269,ERR_TYPE,"console ","e87_create_diag_fifo: mknod error, %s"
#define ERR_CONSOLE_FIFO_CHOWN       11270,ERR_TYPE,"console ","e87_create_diag_fifo: chown error, %s"

/* ============================ ven81c ================================ */

#define ERR_SERVER_CRASHING          11280,ERR_TYPE,"dbcrash ","ABEND: %s CRASHING ................"
#define ERR_SERVER_DIED              11281,ERR_TYPE,"dbcrash ","ABEND: %s DIED ................"
#define ERR_SERVERNAME_CANT_GETENV   11282,ERR_TYPE,"dbstart ","ABEND: cannot getenv SERVERDB/DBNAME"
#define ERR_SERVERNAME_TO_LONG       11283,ERR_TYPE,"dbstart ","ABEND: DBNAME too long, allowed are %d characters"
#define ERR_CANT_GET_HOSTNAME        11284,ERR_TYPE,"dbstart ","ABEND: cannot get my host name"
#define ERR_CANT_GET_OFFICIAL_HOSTNAME 11285,ERR_TYPE,"dbstart ","ABEND: cannot get my official node name"
#define ERR_SPOOL_DIR_MISSING        11286,ERR_TYPE,"dbstart ","ABEND: spool directories missing"
#define ERR_READING_PARAMFILE        11287,ERR_TYPE,"dbstart ","ABEND: error reading parameter file"
#define ERR_PARA_RUNDIR_MISSING      11288,ERR_TYPE,"dbstart ","ABEND: parameter 'RUNDIRECTORY' missig"
#define ERR_CANT_CHANGE_TO_RUNDIR    11289,ERR_TYPE,"dbstart ","ABEND: cannot chdir to rundirectory: %s"
#define ERR_CANT_OPEN_DIAGFILE       11290,ERR_TYPE,"dbstart ","ABEND: cannot open diag file"
#define ERR_NODENAME_TO_LONG         11291,ERR_TYPE,"dbstart ","ABEND: node name too long, allowed are %d characters"
#define ERR_SEMCTL_TIMER             11292,ERR_TYPE,"dbstart ","ABEND: semctl (setval TIMER %d) error: %s"
#define ERR_SEMOP_TIMER              11293,ERR_TYPE,"dbstart ","ABEND: semop (rcv/undo TIMER %d) error: %s"
#define ERR_FORK_ERR                 11294,ERR_TYPE,"dbstart ","fork error, %s"
#define ERR_CANT_OPTAIN_KEY          11295,ERR_TYPE,"dbstart ","ABEND: cannot obtain key from DBNAME"
#define ERR_COORD_RETURNED_FROM      11296,ERR_TYPE,"dbstart ","ABEND: coordinator returned from %s"
#define ERR_COORD_WAIT               11297,ERR_TYPE,"COORDI  ","ABEND: coordinator wait error, %s"
#define ERR_DIED_PID_INFO            11298,ERR_TYPE,"dbcrash ","child DIED pid %ld, stat 0x%04x"
#define ERR_SWAPCONEXT_FAILS         11299,ERR_TYPE,"dbstart ","ABEND: swapcontext fails %s"
#define ERR_ILL_RET_FROM_STACK       11300,ERR_TYPE,"dbstart ","ABEND: illegal return from chgstack"
#define ERR_COREHANDER_ABORT         11301,ERR_TYPE,"corehand","ABORTING due to signal %d"
#define ERR_CREATE_UKP_NETWORK_PIPE  11987, ERR_TYPE, "distrib ", "cannot create UKT-NETWORK-pipe: %s"

/* ============================ ven84c ================================ */

#define ERR_REQUESTOR_SIGALARM_SET   11000, ERR_TYPE, "TASKING ", "cannot set SIGALRM: %s"
#define WRN_REQUESTOR_REOPEN_FIFO    11000, WRN_TYPE, "TASKING ", "rc %d (%s), re-opening fifo '%s'"

/* ============================ ven85c ================================ */
/* NO_DISTRIBUTION... */

/* ============================ ven88c ================================ */
#define ERR_UKT_SWAPCONTEXT		 11088, ERR_TYPE, "TASKING ", "swapcontext failed %d"
#define ERR_UKT_BAD_ENV_DYNSTACK 11088, ERR_TYPE, "TASKING ", "%s not set but needed for dynamic stack"
#define ERR_UKT_BAD_ENV_STARTING 11088, ERR_TYPE, "TASKING ", "%s not set but needed for starting"
#define ERR_UKT_ASYNCIO_SIGNAL   11987, ERR_TYPE, "TASKING ", "UKT: can't install signalhandler for ASYNC_IO, %s"
#define INFO_UKT_ASYNCIO_SIGNAL  11987, INFO_TYPE, "TASKING ", "UKT: signalhandler for ASYNC_IO installed"
#define WRN_UKT_NICE_SETTING     11987, WRN_TYPE, "TASKING ", "UKT: nice error, %s"
#define WRN_UKT_PREEMPT_CTRL     11987, WRN_TYPE, "TASKING ", "UKT: process preemption control failed, %s"
#define INFO_UKT_BIND_CPU        11234, INFO_TYPE, "TASKING ", "cpu %ld bind pid, %ld"
#define WRN_UKT_BIND_CPU         11234, WRN_TYPE, "TASKING ", "cpu bind failed, %s"
#define WRN_UKT_NO_FIXED_PRIO    11987, WRN_TYPE, "TASKING ", "fixed priority not implemented"
#define WRN_UKT_FIX_PRIO_FAILED  11987, WRN_TYPE, "TASKING ", "fixed priority failed, %s"
#define WRN_UKT_RTPRIO_FAILED    11987, WRN_TYPE, "TASKING ", "rtprio(0,%d) error, %s"
#define INFO_UKT_RTPRIO_SETTING  11987, INFO_TYPE, "TASKING ", "realtime priority set to %d!"
#define WRN_UKT_GETPRI_FAILED    11987, WRN_TYPE, "TASKING ", "getpri error, %s"
#define INFO_UKT_GETPRI_RANGE    11987, WRN_TYPE, "TASKING ", "priority is %d (best %d .. worst %d)"
#define INFO_UKT_STACK_SIZE      11088, INFO_TYPE, "TASKING ", "allocated %ld bytes for stacks"
#define INFO_UKT_DYNSTACK_SIZE   11088, INFO_TYPE, "TASKING ", "allocated %ld bytes for dynamic-stacks"
#define INFO_UKT_MAXSTACK_SIZE   11088, INFO_TYPE, "TASKING ", "maximal required stack %ld bytes"
#define INFO_UKT_STACK_AREA      11088, INFO_TYPE, "TASKING ", "area for task-stack-allocation is %s"
#define ERR_UKT_NOC_STACK_OVERFLOW 11088, ERR_TYPE, "TASKING ", "stack overflow with no context"
#define ERR_UKT_WC_STACK_OVERFLOW 11088, ERR_TYPE, "TASKING ", "stack overflow with wrong context 0x%p"
#define ERR_UKT_WC_TCB            11088, ERR_TYPE, "TASKING ", "   task control block would be       0x%p"
#define ERR_UKT_WC_CONTEXT        11088, ERR_TYPE, "TASKING ", "   task context       would be       0x%p"
#define ERR_UKT_WC_FIRST_TASK     11088, ERR_TYPE, "TASKING ", "   first task                        0x%p"
#define ERR_UKT_WC_LAST_TASK     11088, ERR_TYPE, "TASKING ", "   last task                         0x%p"
#define ERR_UKT_STACK_OVERFLOW   11088, ERR_TYPE, "TASKING ", "stack overflow task T%d"
#define ERR_UKT_STACK_DUMP_SIZE   11088, ERR_TYPE, "TASKING ", " stack size %d"
#define ERR_UKT_STACK_DUMP_BOTTOM 11088, ERR_TYPE, "TASKING ", " stack bottom   0x%p"
#define ERR_UKT_STACK_DUMP_SP     11088, ERR_TYPE, "TASKING ", " stack pointer  0x%p"
#define ERR_UKT_STACK_DUMP_TOP    11088, ERR_TYPE, "TASKING ", " stack top      0x%p"
#if (ALPHA && BIT64)
#define ERR_UKT_STACK_DUMP_TOP0   11088, ERR_TYPE, "TASKING ", " stack top[0]   0x%016lx"
#define ERR_UKT_STACK_DUMP_TOP1   11088, ERR_TYPE, "TASKING ", " stack top[1]   0x%016lx"
#else
#define ERR_UKT_STACK_DUMP_TOP0   11088, ERR_TYPE, "TASKING ", " stack top[0]  0x%p"
#define ERR_UKT_STACK_DUMP_TOP1   11088, ERR_TYPE, "TASKING ", " stack top[1]   0x%p"
#endif
#define INFO_UKT_STACK_USAGE      11088, INFO_TYPE, "TASKING ", "T%d stack usage %d"
#define ERR_UKT_WRONG_FREELIST    11088, ERR_TYPE, "TASKING ", "e88_call_a91: curr_quu non-freelist T%d"
#define ERR_UKT_WRONG_REQUEST     11088, ERR_TYPE, "TASKING ", "e88_call_a91: curr_quu request %d T%d"
#define ERR_UKT_ILL_RETURN        11088, ERR_TYPE, "TASKING ", "UKT returned from first task"
#define WRN_UKT_SUPER_USER        11088, WRN_TYPE, "TASKING ", "Server should not run as superuser"
#define WRN_UKT_MISSING_SETUID    11088, WRN_TYPE, "TASKING ", "setuid for superuser is missing, euid %ld"
#define ERR_UKT_SUPER_SETUID      11088, ERR_TYPE, "TASKING ", "setuid (to superuser) failed, %s" 
#define ERR_UKT_BAD_SETUID        11088, ERR_TYPE, "TASKING ", "setuid failed (expected 0; got euid %d, uid %d)"
#define ERR_UKT_BAD_RESET_UID     11088, ERR_TYPE, "TASKING ",  "setuid (to old value) error, %s"
#define ERR_UKT_BAD_RESET_GID     11088, ERR_TYPE, "TASKING ", "setgid (to old value) error, %s"
#define ERR_UKT_STACK_ALLOC       11088, ERR_TYPE, "TASKING ", "Insufficient memory for stacks, %s"
#define ERR_UKT_GET_CONTEXT       11088, ERR_TYPE, "TASKING ", "getcontext failed for tcb 0x%x"
#define WRN_UKT_DANGER_CODE       11088, WRN_TYPE, "TASKING ", "USING DANGER CODE FOR DYNAMIC STACK !!"
#define WRN_UKT_NO_DYNSTACK       11088, WRN_TYPE, "TASKING ", "dynamic-stack-allocation is not available"
#define ERR_UKT_RM_STACK_CONTEXT  11088, ERR_TYPE, "TASKING ", "can't init rm_task_stack_context"
#define INFO_UKT_FREE_TASK_STACK  11088, INFO_TYPE, "TASKING ", "freed stack of task  T%d"
#define ERR_UKT_POD_SETMAXPRI     11088, ERR_TYPE, "TASKING ", "pod_setmaxpri error, %s"
#define ERR_UKT_LWP_CREATE        11088, ERR_TYPE, "TASKING ", "lwp_create    errno = %d tcb 0x%x"
#define INFO_UKT_LWP_NUM_TASKS    11088, INFO_TYPE, "TASKING ", "e88_lwp_status: task count   %d"
#define INFO_UKT_LWP_TASK         11088, INFO_TYPE, "TASKING ", "e88_lwp_status: task ------> %d"
#define INFO_UKT_LWP_THREAD_ID    11088, INFO_TYPE, "TASKING ", "e88_lwp_status: thread_id  = %d"
#define INFO_UKT_LWP_THREAD_KEY   11088, INFO_TYPE, "TASKING ", "e88_lwp_status: thread_key = %d"
#define ERR_DBSTART_UKT_COINITIALIZE_FAILED   110888, ERR_TYPE,"DBSTART ","CoInitialize for UKT%d failed!"

/********************************** I N F O ***********************************/

#define INFO_CHANGE_STATE            11000, INFO_TYPE,"        ","New Kernel State '%s'(%ld)"

/* ============================ ven51c ================================ */

#define INFO_KERNEL_VERSION          11500,INFO_TYPE,"version ","Kernel version: %s"
#define INFO_VSHUTDOWN_CALLED        11501,INFO_TYPE,"shutdown","called, T%d, mode %d"
#define INFO_SHUTDOWN_NORMAL_REQ     11502,INFO_TYPE,"shutdown","normal requested"
#define INFO_SHUTDOWN_KILL_REQ       11503,INFO_TYPE,"shutdown","kill requested"
#define INFO_SHUTDOWN_REINIT_REQ     11504,INFO_TYPE,"shutdown","reinit requested"
#define INFO_UKP_STOPPED             11505,INFO_TYPE,"vfinish ","UKT stopped"
#define INFO_COMM_TIMEOUT            11507,INFO_TYPE,"COMMUNIC","COMMAND TIMEOUT, T%d"
#define INFO_CONN_BROKEN_NO_COMSEG   11508,INFO_TYPE,"COMMUNIC","Connection broken for T%d (no comseg)"
#define INFO_CONN_BROKEN_BY_APPL_ST  11509,INFO_TYPE,"COMMUNIC","Connection broken by appl state %d T%d"
#define INFO_CONN_BROKEN_BAD_COMSEG  11510,INFO_TYPE,"COMMUNIC","Connection broken for T%d, bad comseg 0x%p"
#define INFO_CONN_BROKEN_CI_INFO     11511,INFO_TYPE,"COMMUNIC","ci_pid %ld ref %d sem %d"
#define INFO_CONN_BROKEN_CS_INFO     11512,INFO_TYPE,"COMMUNIC","cs_pid %ld ref %d sem %d state %d"
#define INFO_EXCL_TW_IS_SLEEPING     11513,INFO_TYPE,"tas_coll","TW is sleeping for 1 sec ..."
#define INFO_SV_TASK_CREATED         11514,INFO_TYPE,"vcreate ","server-task T%d created"
#define INFO_SV_STACK_FREED          11515,INFO_TYPE,"vkill   ","freed stack of server-task T%d"
#define INFO_NETW_CLOSE_PATH         11516,INFO_TYPE,"distrib ","e63_close_conn: closing pathid %d"
#define INFO_NETW_CLEAR_PATH         11516,INFO_TYPE,"distrib ","e63_clear_conn: clearing pathid %d"

/* ============================ ven54.c ================================ */
#define INFO_PIPECHECK_FSTAT_CODE_SUPPORTED      11520, INFO_TYPE, "PIPE_IO ", "Input check for pipe with fstat supported"
#define INFO_PIPECHECK_FSTAT_CODE_NOT_SUPPORTED  11521, INFO_TYPE, "PIPE_IO ", "Operating system does not support fstat for input check of pipes"
#define INFO_PIPECHECK_POLL_CODE_SUPPORTED       11522, INFO_TYPE, "PIPE_IO ", "Input check for pipe with poll supported"
#define INFO_PIPECHECK_POLL_CODE_NOT_SUPPORTED   11523, INFO_TYPE, "PIPE_IO ", "Operating system does not support poll for input check of pipes"
#define INFO_PIPECHECK_TIMEOUT                   11524, INFO_TYPE, "PIPE_IO ", "Timeout waiting for child pipe write after %d seconds"
#define INFO_FORMATTING_VOLUME_START             11525, INFO_TYPE, "vdevsize", "Start formatting %ld pages"
#define INFO_FORMATTED_PAGES_LEFT                11526, INFO_TYPE, "vdevsize", "Formatting remaining %ld pages"
#define INFO_FORMATTING_VOLUME_DONE              11527, INFO_TYPE, "vdevsize", "Finished formatting %ld pages"

/* ============================ ven63c ================================ */

#define INFO_VNRESTART_ENTER         11999,INFO_TYPE,"distrib ", "vnrestart:   entered T%d"
#define INFO_VNRESTART_LEAVE         11999,INFO_TYPE,"distrib ", "vnrestart:   leaving T%d"
#define INFO_VNSHUTDOWN_ENTER        11999,INFO_TYPE,"distrib ", "vnshutdown:   entered T%d"
#define INFO_VNSHUTDOWN_LEAVE        11999,INFO_TYPE,"distrib ", "vnshutdown:   entered T%d"
#define INFO_VNOPEN_ENTER            11999,INFO_TYPE,"distrib ", "vnopen:      entered T%d '%s:%s'"
#define INFO_VNOPEN_LEAVE            11999,INFO_TYPE,"distrib ", "vnopen:      leaving T%d pathid %d rc %d"
#define INFO_VNCLOSE_ENTER           11999,INFO_TYPE,"distrib ", "vnclose:     entered T%d pathid %d"
#define INFO_VNCLOSE_LEAVE           11999,INFO_TYPE,"distrib ", "vnclose:     leaving T%d rc %d"
#define INFO_VNCLEAR_ENTER           11999,INFO_TYPE,"distrib ", "vnclear:     entered T%d pathid %d"
#define INFO_VNCLEAR_LEAVE           11999,INFO_TYPE,"distrib ", "vnclear:     leaving T%d"
#define INFO_VNREQUEST_ENTER         11999,INFO_TYPE,"distrib ", "vnrequest: entered T%d pathid %d lgt %d"
#define INFO_VNREQUEST_LEAVE         11999,INFO_TYPE,"distrib ", "vnrequest: leaving T%d rc %d"
#define INFO_VNRECEIVE_ENTER         11999,INFO_TYPE,"distrib ", "vnreceive: entered T%d timeout %d"
#define INFO_VNRECEIVE_LEAVE         11999,INFO_TYPE,"distrib ", "vnreceive: leaving T%d pathid %d lgt %d rc %d"
#define INFO_VNACKNOWLEDGE_ENTER     11999,INFO_TYPE,"distrib ", "vnacknowledge: entered T%d pathid %d"
#define INFO_VNACKNOWLEDGE_LEAVE     11999,INFO_TYPE,"distrib ", "vnacknowledge: leaving T%d pathid %d rc %d"
#define INFO_SENDING_SHUTDOWN		 11999,INFO_TYPE,"distrib ", "e63_shutdown: sending request to T%d"
#define INFO_SHUTDOWN_WAITING		 11999,INFO_TYPE,"distrib ", "e63_shutdown: waiting for clear T%d"
#define INFO_SHUTDOWN_ALL_CLEARED    11999,INFO_TYPE,"distrib ", "e63_shutdown: all conns cleared T%d"
#define INFO_OPEN_PATHID             11999,INFO_TYPE,"distrib ", "e63_open:               pathid %d"
#define INFO_LOC_CON_ENTER           11999,INFO_TYPE,"distrib ", "e63_loc_con: entered T%d pathid %d '%s' service %d"
#define INFO_REM_CON_ENTER           11999,INFO_TYPE,"distrib ", "e63_rem_con: entered T%d pathid %d '%s:%s' service %d"
#define INFO_REM_CON_WAKE            11999,INFO_TYPE,"distrib ", "e63_rem_con: waking semid %d comseg 0x%p"
#define INFO_REM_CON_RETURNCODE      11999,INFO_TYPE,"distrib ", "e63_rem_con: NETWORK returncode %d"
#define INFO_CLOSE_PATHID            11999,INFO_TYPE,"distrib ", "e63_close: entered T%d pathid %d"
#define INFO_CLOSE_SENDING_REQUEST   11999,INFO_TYPE,"distrib ", "e63_close: sending request to T%d"
#define INFO_CLOSE_LEAVING           11999,INFO_TYPE,"distrib ", "e63_close: leaving T%d"
#define INFO_CLOSE_WAKE              11999,INFO_TYPE,"distrib ", "e63_close: waking semid %d comseg 0x%p"
#define INFO_CLOSE_LEAVE             11999,INFO_TYPE,"distrib ", "e63_close: leaving T%d"
#define INFO_CLEAR_ENTER             11999,INFO_TYPE,"distrib ", "e63_clear: entered T%d pathid %d"
#define INFO_CLEAR_LEAVE             11999,INFO_TYPE,"distrib ", "e63_clear: leaving T%d"
#define INFO_RELEASING_ENTER         11000,INFO_TYPE,"distrib ", "Releasing  T%d,%d reason %d"

/* ============================ ven70c ================================ */
#define INFO_INITSHM_TC              11530,INFO_TYPE,"init_shm","taskcluster: %s"
#define INFO_INITSHM_NUM_XXX_TSK     11531,INFO_TYPE,"init_shm","number of %s   %d" 
#define INFO_SHMCHUNK_SIZE           11532,INFO_TYPE,"init_shm","SHMCHUNK size 0x%X"
#define INFO_INITSHM_CR_SIZE         11533,INFO_TYPE,"init_shm","creating %s size %10ld"
#define INFO_INITSHM_ALIGN_GAP       11534,INFO_TYPE,"init_shm","alignment gaps    total size %10ld"
#define INFO_INITSHM_TOO_TIGHT_DATA  11535,INFO_TYPE,"init_shm","section too tight above data 0x%p"
#define INFO_INITSHM_ATTACHED_AT     11536,INFO_TYPE,"init_shm","attached %s at   0x%p"
#define INFO_INITSHM_ENDS_AT         11537,INFO_TYPE,"init_shm","%s ends at       0x%p"
#define INFO_INITSHM_LOCKED          11538,INFO_TYPE,"init_shm","locked kernel shared section, ret = %d" 
#define INFO_INITSHM_ASYNIO_SUPP     11539,INFO_TYPE,"init_shm","KERNEL performs ASYNCRONOUS I/O"
#define INFO_INITSHM_USING_SUN_ISM   11540,INFO_TYPE,"init_shm","KERNEL performs SUN ISM mechanism"
#define INFO_INITSHM_MEM_ALLOCATED   11541,INFO_TYPE,"init_shm","%d Pages allocated for %s"
#define INFO_USE_THREAD_FOR_TASK     11541,INFO_TYPE,"init_shm","KERNEL uses thread for task"
#define INFO_USE_INTERNAL_TASKING    11542,INFO_TYPE,"init_shm","KERNEL uses internal tasking"

/* ============================ ven71c ================================ */

#define INFO_DISP_UKP_STOPPED        11550,INFO_TYPE,"dispatch","UKT stopped"
#define INFO_SHUTKILL_UKP_STOPPED    11551,INFO_TYPE,"shutkill","UKT stopped"
#define INFO_DISP_IGN_SIGNAL         11552,INFO_TYPE,"dispatch","dispatcher ignored signal"

/* ============================ ven74c ================================ */

#define INFO_INITCOM_UKP_COMS_SZ     11553,INFO_TYPE,"COMMUNIC","UKT%d comseg size %ld"
#define INFO_INITCOM_BIGCOM_ATT_AT   11554,INFO_TYPE,"COMMUNIC","UKT%d attached big comseg at  0x%p"
#define INFO_INITCOM_BIGCOM_ENDS_AT  11555,INFO_TYPE,"COMMUNIC","UKT%d big comseg ends at      0x%p"

/* ============================ ven75c ================================ */

#define INFO_RELEASING_TSK           11560,INFO_TYPE,"COMMUNIC","Releasing  T%d"

#define INFO_CONNECTING_TASK         11561,INFO_TYPE,"COMMUNIC","%s"

/* ============================ ven82c ================================ */

#define INFO_PROCESS_STARTED         11565,INFO_TYPE,"startup ","%s started" 
#define INFO_PROCESS_STOPPED         11566,INFO_TYPE,"stop    ","%s stopped" 

/* ============================ ven83c ================================ */

#define INFO_TIMER_START_COMPLETE    11570,INFO_TYPE,"startup ","complete"

/* ============================ ven84c ================================ */

#define INFO_REQUESTOR_REJECT        11575,INFO_TYPE,"COMMUNIC","Requestor rejecting (server state %d)"
#define INFO_REQUESTOR_CONN_TSK      11576,INFO_TYPE,"COMMUNIC","%s"
#define INFO_REQUESTOR_CANCEL_REQ    11577,INFO_TYPE,"COMMUNIC","Cancel request for T%d"
#define INFO_REQUESTOR_DUMP_REQ      11578,INFO_TYPE,"COMMUNIC","Dump request"
#define INFO_REQUESTOR_FIND_TSK      11579,INFO_TYPE,"COMMUNIC","e84_find: T%d,%d conn %ld state %d"
#define INFO_REQUESTOR_FIND_ALL_TSK  11580,INFO_TYPE,"COMMUNIC","e84_find: UKT%d  T%-3d conn %ld '%s'"
#define INFO_REQUESTOR_FIND_UKP_TSK  11581,INFO_TYPE,"COMMUNIC","e84_find: UKT%d%c T%-3d conn %ld '%s'"

/* ============================ ven81c ================================ */

#define INFO_SERVER_STOPPED          11590,INFO_TYPE,"dbstop  "," %s STOPPED ----------------"
#define INFO_RTE_VERSION             11591,INFO_TYPE,"dbstart ","%s" 
#define INFO_FOR_IPC_RESOURCES       11592,INFO_TYPE,"dbstart ","key for ipc resources 0x%0"SIZL"x"
#define INFO_RESUMED_TW              11593,INFO_TYPE,"dbstop  ","Resumed TRACEWRITER"
#define INFO_CAUGHT_SIG              11594,INFO_TYPE,"dbstop  ","caught signal %d"
#define INFO_CURR_INST_ADDR          11595,INFO_TYPE,"corehand","current inst.addr.reg 0x%p"
#define INFO_CALLED_FROM_CODE        11596,INFO_TYPE,"corehand","called from code addr 0x%p"
#define INFO_GATEWAY_STARTING        11081,INFO_TYPE,"DBSTART ","GATEWAY STARTING  ++++++++++++++++"
#define INFO_GATEWAY_DBNAME          11081,INFO_TYPE,"DBSTART ","GATEWAY  DBNAME   '%s'"
#define INFO_GATEWAY_DBNODE          11081,INFO_TYPE,"DBSTART ","GATEWAY  DBNODE   '%s'"
#define INFO_KERNEL_STARTING         11081,INFO_TYPE,"DBSTART ","KERNEL STARTING  ++++++++++++++++"
#define INFO_KERNEL_DBNAME           11081,INFO_TYPE,"DBSTART ","KERNEL  DBNAME   '%s'"
#define INFO_KERNEL_DBNODE           11081,INFO_TYPE,"DBSTART ","KERNEL  DBNODE   '%s'"
#define INFO_OPEN_FD                 11597,INFO_TYPE,"IO      ","Open '%s' successfull, fd: %d"

/************************************ W R N ***********************************/

/* ============================ ven42c ================================ */
#define WRN_CONNECT_SERVER_RETRY     12345, WRN_TYPE, "COMMUNIC", "retry connect: %s"

/* ============================ ven42c ================================ */
#define ERR_MGH_GETHOSTNAME_FAILED   11987, ERR_TYPE, "DBSTART ", "gethostname error, %s"
#define ERR_MGH_UNAME_FAILED         11987, ERR_TYPE, "DBSTART ", "uname error, %s"
#define ERR_MGH_UNAME_TOO_LONG       11987, ERR_TYPE, "DBSTART ", "nodename too long '%s'"
#define INFO_GETSERVBYNAME_RETRY     11987, INFO_TYPE, "DBSTART ", "getservbyname(%s) retry #%d"
#define WRN_GETSERVBYNAME_UNKNOWN    11987, WRN_TYPE, "DBSTART ", "unknown service '%s' (see /etc/services)"
#define INFO_GETHOSTBYNAME_RETRY     11987, INFO_TYPE, "DBSTART ", "gethostbyname(%s) retry #%d"
#define WRN_GETHOSTBYNAME_UNKNOWN    11987, WRN_TYPE, "DBSTART ", "unknown host '%s' (see /etc/hosts)"
#define WRN_GETHOSTBYNAME_TOO_LONG   11987, ERR_TYPE, "DBSTART ", "address too long %d allowed %d"
#define INFO_GETHOSTBYADDR_RETRY     11987, INFO_TYPE, "DBSTART ", "gethostbyaddr retry #%d"
#define WRN_GETHOSTBYADDR_UNKNOWN    11987, WRN_TYPE, "DBSTART ", "unknown host address %s (see /etc/hosts)"
#define WRN_GETHOSTBYADDR_TOO_LONG   11987, ERR_TYPE, "DBSTART ", "nodename too long '%s'"

/* ============================ ven45c ================================ */

#define WRN_CANCEL_THREAD_FAILED     11987, WRN_TYPE, "ABORT   ", "cancel thread %ld error, %s"

/* ============================ ven46c ================================ */

#define WRN_DUMPFILE_OPEN_FAILED     11987, WRN_TYPE, "DUMPING ", "cannot open 'DUMPFILE', %s"

/* ============================ ven51c ================================ */

#define WRN_VRESTART_WRONG_TASK      11800,WRN_TYPE,"restart ","from wrong task T%ld"
#define WRN_VSHUTDOWN_WRONG_TASK     11801,WRN_TYPE,"vshutdwn","from wrong task T%ld"
#define WRN_SHUTDOWN_NOT_WARM_STATE  11802,WRN_TYPE,"shutdown","from wrong state"

/* ============================ ven52c ================================ */

#define WRN_ILL_HZ_ENV_VAR           11003,WRN_TYPE,"timeinit","illegal environment HZ=%s"
#if defined(OSF1)
#define INFO_TIMEDEV_NOT_AVAILABLE   11987, INFO_TYPE, "vclock  ", "/dev/timedev not available: %s"
#define INFO_TIMESTAMP_MMAP_FAILED   11987, INFO_TYPE, "vclock  ", "mmap : %s"
#define INFO_TIMEDEV_AVAILABLE       11987, INFO_TYPE, "vclock  ", "/dev/timedev available for quick time access"
#endif

/* ============================ ven53c ================================ */

#define WRN_VRECV_APPLICATION_DEAD   11804,WRN_TYPE,"COMMUNIC","application dead for T%d"
#define WRN_VRECV_IN_WRONG_STATE     11805,WRN_TYPE,"COMMUNIC","call in wrong state %d T%d"
#define WRN_VRECV_BAD_SERVERSTATE    11806,WRN_TYPE,"COMMUNIC","server state %d"
#define WRN_REPLY_PACK_SIZE          11807,WRN_TYPE,"COMMUNIC","pktsiz %ld datsiz %ld reqlen %ld"
#define WRN_VREPLY_IN_WRONG_STATE    11808,WRN_TYPE,"COMMUNIC","call in wrong state %d T%d"

/* ============================ ven54.c =============================== */

#define WRN_ATTACH_DEVSPACE                      11900, WRN_TYPE,  "vattach ", "Detected during attach volume '%s':%s"
#define WRN_TOUCH_DEVSPACE                       11900, WRN_TYPE,  "vdevsize", "Detected during touch volume '%s':%s"
#define WRN_PIPECHECK_INPUT_CHECK_NOT_SUPPORTED  11900, WRN_TYPE,  "PIPE_IO ", "Operating system does not support any known input checks for pipes"
#define WRN_VDETACH_IGNORE_SELF_IO               11900, WRN_TYPE,  "VDETACH ", "Ignoring pending self I/O operation UKT%d on device %d"

/* ============================ ven541c =============================== */

#define WRN_DEVSZ_OPEN_RAW_FAILED    11987,WRN_TYPE,"I/O     ","sqldevsize: Could not open raw device '%s': %s"
#define WRN_DEVSZ_FSTAT_FAILED       11987,WRN_TYPE,"I/O     ","sqldevsize: fstat failed on %s, %s"
#define INFO_DEVSZ_CALCULATED_SIZE   11987,INFO_TYPE,"I/O     ","sqldevsize: volume %s devsize = %d"
#define WRN_DEVSZ_ILLEGAL_DEVSIZE    11987,WRN_TYPE,"I/O     ","sqldevsize: volume %s illegal devsize = %d"
#define WRN_DEVSZ_ILLEGAL_DEVTYPE    11987,WRN_TYPE,"I/O     ","sqldevsize: device type must be RAW!"
#define WRN_NODEINFO_STAT_ERROR      11987,WRN_TYPE,"I/O     ","get_nodinfo: stat error for file '%s', %s"
#define IERR_NODEINFO_UNKNOWN_MODE   11987,IERR_TYPE,"I/O     ","get_nodinfo: Unknown node-mode: %d"
#define WRN_DEVSZ0_MALLOC_ERROR      11987,WRN_TYPE,"I/O     ","get_devsize0: malloc error, %s"

/* ============================ ven57c ================================ */

#define WRN_SHRD_DYN_DATA_TOO_SMALL  11809,WRN_TYPE,"vnewbuf ","shareddyndata too small in XPARAM"
#define WRN_SHRD_DYN_POOL_TOO_SMALL  11810,WRN_TYPE,"vallocat","shareddynpool too small in XPARAM"
#define WRN_SHRD_DYN_DATA_TOO_SMALL  11809,WRN_TYPE,"vnewbuf ","shareddyndata too small in XPARAM"
#define WRN_SHRD_DATA_CACHE_TOO_SMALL  11809,WRN_TYPE,"alloc_da","datacache too small in XPARAM"
#define WRN_SHRD_CONV_CACHE_TOO_SMALL  11809,WRN_TYPE,"alloc_co","convertercache too small in XPARAM"

/* ============================ ven63c ================================ */

#define WRN_NETW_ALREADY_RESTARTED   11811,WRN_TYPE,"distrib ","Network already restarted"
#define WRN_NETW_CONN_REFUSED        11812,WRN_TYPE,"distrib ","e63_loc_con: connection refused %d"
#define WRN_NETW_SHUTDOWN            11813,WRN_TYPE,"distrib ","e63_rem_con: net shutdown"
#define WRN_NETW_CONN_TIMEOUT        11814,WRN_TYPE,"distrib ","e63_rem_con: TIMEOUT"
#define WRN_NETW_REQ_NET_SHUTDOWN    11815,WRN_TYPE,"distrib ","e63_request: net shutdown, pathid %d" 
#define WRN_NETW_REQ_NET_TIMEOUT     11816,WRN_TYPE,"distrib ","e63_request: TIMEOUT, pathid %d"
#define WRN_NETW_RCV_HANGING_TASK    11817,WRN_TYPE,"distrib ","e63_receive: hanging T%d"
#define WRN_NETW_RCV_NET_SHUTDOWN    11818,WRN_TYPE,"distrib ","e63_receive: net shutdown T%d"
#define WRN_NETW_RCV_NET_TIMEOUT     11819,WRN_TYPE,"distrib ","e63_receive: TIMEOUT  T%d"
#define WRN_NETW_NAME_TOO_LANG       11101,WRN_TYPE,"distrib ","WARNING: node+dbname longer than %d, using def-com"
#define WRN_NETW_SERVERTYPE          11101,WRN_TYPE,"distrib ","WARNING: servertypes: ignoring '%s'"

/* ============================ ven67c ================================ */

#define WRN_TC_CNT_GT_MAX            11820,WRN_TYPE,"tc_analy","taskcluster specifies %d %s (max=%d)"

/* ============================ ven71c ================================ */

#define WRN_DISP_EXIT_DUE_SV_STOP    11821,WRN_TYPE,"dispatch","dispatcher exiting due to server stop" 

/* ============================ ven71c ================================ */

#define WRN_INITCOM_DEF_PACKET_SZ    11822,WRN_TYPE,"COMMUNIC","default Packet Size %lu"

/* ============================ ven75c ================================ */

#define WRN_DISTRIB_WAKE_ERR         11823,WRN_TYPE,"COMMUNIC","e75_wake: write interrupted, retrying"
#define WRN_DISTRIB_WAKE_SEMOP_ERR   11823,WRN_TYPE,"COMMUNIC","e75_wake: semop interrupted, retrying"
#define WRN_RELEASE_TSK_BY_REASON    11824,WRN_TYPE,"COMMUNIC","Releasing  T%d %s"
#define WRN_CONNECT_LAST_COMSEG      11825,WRN_TYPE,"COMMUNIC","e75_connect: last comseg 0x%p"
#define WRN_CONNECT_CLIENT_LOST      11826,WRN_TYPE,"COMMUNIC","e75_connect: client lost"
#define WRN_CONNECT_NET_DOWN         11827,WRN_TYPE,"COMMUNIC","e75_connect: net down"
#define WRN_CONNECT_NO_STACK_MEM     11828,WRN_TYPE,"COMMUNIC","insufficient memory for stack! T%d"

/* ============================ ven84c ================================ */

#define WRN_REQUESTOR_REJ_REM_REQ    11829,WRN_TYPE,"COMMUNIC","rejecting remote info request"
#define WRN_REQUESTOR_REJ_BIG_REQ    11830,WRN_TYPE,"COMMUNIC","rejecting big-comseg request"
#define WRN_REQUESTOR_REJ_DIST_REQ   11831,WRN_TYPE,"COMMUNIC","rejecting distrib request"
#define WRN_REQUESTOR_REJ_REQ        11832,WRN_TYPE,"COMMUNIC","rejecting request"
#define WRN_REQUESTOR_ILL_MSG_TYPE   11833,WRN_TYPE,"COMMUNIC","Unknown message type %d"
#define WRN_REQUESTOR_PACK_CNT_NE_1  11834,WRN_TYPE,"COMMUNIC","packet_cnt != 1 not yet supported"
#define WRN_REQUESTOR_UT_IS_BUSY     11835,WRN_TYPE,"COMMUNIC","UTILITY is busy state %d conn %ld"
#define WRN_REQUESTOR_MAXSVDB_EXCEED 11836,WRN_TYPE,"COMMUNIC","MAXSERVERDB %d exceeded"
#define WRN_REQUESTOR_ALL_UKPS_BUSY  11837,WRN_TYPE,"COMMUNIC","ALL are busy (in all UKTs)"
#define WRN_REQUESTOR_ALL_IN_UKP_BUS 11838,WRN_TYPE,"COMMUNIC","ALL are busy (in UKT%d)"
#define WRN_REQUESTOR_CNCL_FROM_CONN 11839,WRN_TYPE,"COMMUNIC","Cancelling T%d from connect, state %d"
#define WRN_REQUESTOR_CNCL_CONN_TIME 11839,WRN_TYPE,"COMMUNIC","           T%d connected %ld time %ld"
#define WRN_KILL_TSK_DIED_APPL       11840,WRN_TYPE,"COMMUNIC","Killing T%d for died apid %ld"
#define WRN_POSSIBLE_UNUSED_CONN     11841,WRN_TYPE,"COMMUNIC","possibly unused connection T%d"
#define WRN_KILL_RCV_TSK_DIED_APPL   11842,WRN_TYPE,"COMMUNIC","Killing T%d,%d for died apid %ld"
#define WRN_REQUESTOR_CNCL_RCV_CONN  11843,WRN_TYPE,"COMMUNIC","Cancelling T%d,%d from connect"
#define WRN_REQUESTOR_CNCL_ABORT_0   11845,WRN_TYPE,"COMMUNIC","Cancelling T%d leads to connection abort.."
#define WRN_REQUESTOR_CNCL_ABORT_1   11845,WRN_TYPE,"COMMUNIC","Client may had problem to attach to shared memory"

/* ============================ ven81c ================================ */

#define WRN_RESOURCE_LIMITS          11844,WRN_TYPE,"dbstart ","cannot obtain resource limits"

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/


/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/



#endif  /* GEN500_H */
