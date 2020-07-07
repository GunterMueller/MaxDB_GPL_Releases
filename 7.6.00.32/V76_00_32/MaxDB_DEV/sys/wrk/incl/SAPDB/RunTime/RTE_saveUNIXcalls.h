/*!***************************************************************************

  module      : RTE_saveUNIXcalls.h

  -------------------------------------------------------------------------

  responsible : JoergM

  special area: System Calls
  description : System Calls for UNIX wrapped to prevent LINUX signal problems
  
  last changed: 2001-04-03  16:25
  see also    : 

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2001-2005 SAP AG


    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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

#if defined(_WIN32)

/* UNIX only */

#else

/*
// the following define is used to protocol each semaphore create/remove in /SAP_DB/data/wrk/remove_semaphores
// and last_client_semop_error_ipcs_output in current rundirectory of database
//
// #if defined(LINUX) && defined(X86_64)
// #define SAPDB_DEBUG_SEMAPHORES Scharfschaltung fuer Torsten
// #endif
//
 */

#ifndef externC
#    if defined(__cplusplus)
#        define externC extern "C"
#    else
#        define externC
#    endif /* __cplusplus */
#endif /* externC */

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h> /* for stat and fstat */
#include <fcntl.h>
#include <sys/poll.h> /* for poll */
#include <sys/uio.h> /* for writev */
#include <sys/ipc.h> /* for semop */
#include <sys/sem.h> /* for semop */
#include <sys/shm.h> /* for shmctl */
#include <stdarg.h>
#include <sys/mman.h>

#include "gen00.h"
#include "RunTime/RTE_Types.h"

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

/*===========================================================================*
 *  GLOBAL FUNCTIONS                                                         *
 *===========================================================================*/

externC RTE_FileHandle RTE_save_open(SAPDB_Char const *pathname, SAPDB_Int flags);

externC RTE_FileHandle RTE_save_openCreate(SAPDB_Char const *pathname, SAPDB_Int flags, SAPDB_Int mode);

externC SAPDB_Int      RTE_save_close(RTE_FileHandle fd);

externC SAPDB_Int      RTE_save_stat(SAPDB_Char const *pathname, struct stat *statBuf);

externC SAPDB_Int      RTE_save_fstat(RTE_FileHandle fd, struct stat *statBuf);

externC RTE_FileOffset RTE_save_lseek(RTE_FileHandle fd, RTE_FileOffset offset, SAPDB_Int whence);
 
externC SAPDB_Long     RTE_save_read(RTE_FileHandle fd, void *buf, SAPDB_Long nbyte);

externC SAPDB_Long     RTE_save_nonblocking_read(RTE_FileHandle fd, void *buf, SAPDB_Long nbyte);

externC SAPDB_Long     RTE_save_write(RTE_FileHandle fd, void *buf, SAPDB_Long nbyte);

externC SAPDB_Long     RTE_save_nonblocking_write(RTE_FileHandle fd, void *buf, SAPDB_Long nbyte);

externC SAPDB_Long     RTE_save_writev(RTE_FileHandle fd, struct iovec *vector, SAPDB_Long count);

externC SAPDB_Int      RTE_save_poll(struct pollfd fds[], SAPDB_Int nfds, SAPDB_Int timeout);

externC SAPDB_Int      RTE_save_unlink(SAPDB_Char const *pathname);

externC SAPDB_Int      RTE_save_semop(SAPDB_Int semid, struct sembuf *semops, SAPDB_UInt nsemops);

externC SAPDB_Int      RTE_save_semctl(SAPDB_Int semid, SAPDB_Int semnum, SAPDB_Int cmd, union semun arg);

externC SAPDB_Int      RTE_save_shmctl(SAPDB_Int shmid, SAPDB_Int cmd, struct shmid_ds *shmState);

externC SAPDB_Int4     RTE_save_waitpid(SAPDB_Int4 pidToWaitFor, SAPDB_Int *pStatus, SAPDB_Int flags);

externC SAPDB_Int4     RTE_save_chmod(SAPDB_Char const *pathname, SAPDB_Int mode);

externC SAPDB_Int4     RTE_save_access(SAPDB_Char const *pathname, SAPDB_Int mode);

externC SAPDB_Int4     RTE_save_rename(SAPDB_Char const *oldPathname, SAPDB_Char const *newPathname);
  
externC SAPDB_Char *   RTE_save_getcwd(SAPDB_Char *pathBuffer, SAPDB_ULong bufferSize);

externC void *         RTE_save_malloc(SAPDB_ULong size);

externC void           RTE_save_free(void * pointer);

externC SAPDB_Int4     RTE_save_fork();

externC SAPDB_Int4     RTE_save_vfork();

externC SAPDB_Int4     RTE_save_dup2(SAPDB_Int fd, SAPDB_Int fdTarget);

externC SAPDB_Int4     RTE_save_pipe(SAPDB_Int fdPair[2]);

externC SAPDB_Int4     RTE_save_execvp(SAPDB_Char const * executable, SAPDB_Char * const argv[]);

externC SAPDB_Int4     RTE_save_execve(SAPDB_Char const * executable,
                                       SAPDB_Char * const argv[],
                                       SAPDB_Char * const envp[]);

externC SAPDB_Int4     RTE_save_execv (SAPDB_Char const * executable,
                                       SAPDB_Char * const argv[]);

externC void           RTE_save_exit(SAPDB_Int status);

externC void           RTE_save__exit(SAPDB_Int status);

externC SAPDB_Int4     RTE_save_chown(SAPDB_Char const *pathname, SAPDB_Int4 userId, SAPDB_Int4 groupId);

externC SAPDB_Int4     RTE_save_fchown(SAPDB_Int4 fildes, SAPDB_Int4 userId, SAPDB_Int4 groupId);

externC SAPDB_Int4     RTE_save_geteuid();

externC SAPDB_Int4     RTE_save_getuid();

externC SAPDB_Int4     RTE_save_getegid();

externC SAPDB_Int4     RTE_save_getgid();

externC SAPDB_Int4     RTE_save_getpid();

externC SAPDB_Int4     RTE_save_getppid();

externC SAPDB_Int4     RTE_save_setuid(SAPDB_Int4 userId);

externC SAPDB_Int4     RTE_save_setreuid(SAPDB_Int4 userId, SAPDB_Int4 effectivUserId);

externC SAPDB_Int4     RTE_save_setgid(SAPDB_Int4 groupId);

externC SAPDB_Int4     RTE_save_setregid(SAPDB_Int4 groupId, SAPDB_Int4 effectivGroupId);

externC SAPDB_Int4     RTE_save_ftruncate (SAPDB_Int fildes, SAPDB_ULong length);

externC void *         RTE_save_mmap(void * pBaseAddress, SAPDB_ULong size, SAPDB_Int prot, 
                                     SAPDB_Int flags, SAPDB_Int fildes, SAPDB_ULong off);

externC SAPDB_Int4     RTE_save_munmap(void * addr, SAPDB_ULong len);

externC SAPDB_Int4     RTE_save_shm_open(SAPDB_Char const * name, SAPDB_Int oflag, 
                                         SAPDB_Int mode);

externC SAPDB_Int4     RTE_save_shm_unlink(SAPDB_Char const *name);

externC SAPDB_Char const *  RTE_save_strerror (RTE_SystemRc const  err);

externC SAPDB_Int4     RTE_save_kill(SAPDB_Int4 processId, SAPDB_Int4 signalCode);

externC SAPDB_Int4     RTE_save_getsid(SAPDB_Int4 processId);

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/

#endif /* UNIX only */
