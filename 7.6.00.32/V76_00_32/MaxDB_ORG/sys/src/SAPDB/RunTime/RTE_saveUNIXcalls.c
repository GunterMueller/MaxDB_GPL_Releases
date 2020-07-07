/*!***************************************************************************

  module      : RTE_saveUNIXcalls.c

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

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#if defined(SUN) || defined(SOLARIS)
/* necessary to find prototypes for shm_open and shm_unlink */
#define     _POSIX_C_SOURCE 199506L
#define     _XPG4_2
#include <time.h>
#include <wait.h>
#endif

#include "RunTime/RTE_saveUNIXcalls.h"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageOutput.h"
#include <errno.h>

/*==========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*
  Input paramreter: return type type_
                    calling sequence call_
 */
#define RTESYS_CALL_IGNORING_EINTR(type_, call_) \
    type_ result; \
    do { \
        result = call_ ; \
    } while ( result == ((type_)-1) \
           && errno == EINTR ); \
    return result

/*
  Input paramreter: function name function_
                    file descriptor fd_
                    return type type_
                    calling sequence call_
 */
#define RTESYS_CALL_IGNORING_EAGAIN_ENOMEM_AND_EINTR(function_, fd_, type_, call_) \
    type_ result; \
    SAPDB_Long gotResourceProblem = 0; \
\
    for ( ;; ) { \
        result = call_; \
        if ( result == ((type_)-1) ) \
        { \
            if ( (ENOMEM == errno) || (EAGAIN == errno) ) \
            { \
                if ( !gotResourceProblem ) \
                { \
                    SAPDBErr_MessageOutput( SAPDB_MSG_TO_DIAGFILE, \
                                            19911, \
                                            SAPDB_MESSAGE_TYPE_ERROR, \
                                            "UNIXCALL", \
                                            "Resource problem detected for '%s' on '%d'", \
                                            function_, \
                                            (int)fd_ ); \
                } \
                ++gotResourceProblem; if ( !gotResourceProblem ) { gotResourceProblem = 1; } \
                sleep(0); \
            } \
            else if ( EINTR != errno ) \
            { \
                break; \
            } \
        } \
        else \
        { \
            break; \
        } \
    } \
\
    if ( gotResourceProblem ) \
    { \
        SAPDBErr_MessageOutput( SAPDB_MSG_TO_DIAGFILE, \
                                19911, \
                                SAPDB_MESSAGE_TYPE_ERROR, \
                                "UNIXCALL", \
                                "Resource problem for '%s' on '%d' solved after %ld retries", \
                                function_, \
                                (int)fd_, \
                                (long)gotResourceProblem ); \
    } \
\
    return result

/*
  Input paramreter: function name function_
                    file descriptor fd_
                    return type type_
                    calling sequence call_
 */
#define RTESYS_CALL_IGNORING_ENOMEM_AND_EINTR(function_, fd_, type_, call_) \
    type_ result; \
    SAPDB_Long gotResourceProblem = 0; \
\
    for ( ;; ) { \
        result = call_; \
        if ( result == ((type_)-1) ) \
        { \
            if ( (ENOMEM == errno) ) \
            { \
                if ( !gotResourceProblem ) \
                { \
                    SAPDBErr_MessageOutput( SAPDB_MSG_TO_DIAGFILE, \
                                            19911, \
                                            SAPDB_MESSAGE_TYPE_ERROR, \
                                            "UNIXCALL", \
                                            "Resource problem detected for '%s' on '%d'", \
                                            function_, \
                                            (int)fd_ ); \
                } \
                ++gotResourceProblem; if ( !gotResourceProblem ) { gotResourceProblem = 1; } \
                sleep(0); \
            } \
            else if ( EINTR != errno ) \
            { \
                break; \
            } \
        } \
        else \
        { \
            break; \
        } \
    } \
\
    if ( gotResourceProblem ) \
    { \
        SAPDBErr_MessageOutput( SAPDB_MSG_TO_DIAGFILE, \
                                19911, \
                                SAPDB_MESSAGE_TYPE_ERROR, \
                                "UNIXCALL", \
                                "Resource problem for '%s' on '%d' solved after %ld retries", \
                                function_, \
                                (int)fd_, \
                                (long)gotResourceProblem ); \
    } \
\
    return result

/*===========================================================================*
 *  GLOBAL FUNCTIONS                                                         *
 *===========================================================================*/

SAPDB_Int RTE_save_open(SAPDB_Char const *pathname, SAPDB_Int flags)
{
    RTESYS_CALL_IGNORING_EINTR( SAPDB_Int, open(pathname, flags) );
}

SAPDB_Int RTE_save_openCreate(SAPDB_Char const *pathname, SAPDB_Int flags, SAPDB_Int mode)
{
    RTESYS_CALL_IGNORING_EINTR( SAPDB_Int, open(pathname, flags, (mode_t)mode) );
}

SAPDB_Int RTE_save_close(RTE_FileHandle fd)
{
    RTESYS_CALL_IGNORING_EINTR( SAPDB_Int, close(fd) );
}

SAPDB_Int RTE_save_stat(SAPDB_Char const *pathname, struct stat *statBuf)
{
    RTESYS_CALL_IGNORING_EINTR( SAPDB_Int, stat(pathname, statBuf) );
}

SAPDB_Int RTE_save_fstat(RTE_FileHandle fd, struct stat *statBuf)
{
    RTESYS_CALL_IGNORING_EINTR( SAPDB_Int, fstat(fd, statBuf) );
}

RTE_FileOffset RTE_save_lseek(RTE_FileHandle fd, RTE_FileOffset offset, SAPDB_Int whence)
{
    RTESYS_CALL_IGNORING_EINTR( RTE_FileOffset, (RTE_FileOffset)lseek(fd, (off_t)offset, whence) );
}
 
SAPDB_Long RTE_save_read(RTE_FileHandle fd, void *buf, SAPDB_Long nbyte)
{
    RTESYS_CALL_IGNORING_EAGAIN_ENOMEM_AND_EINTR( "read", fd, SAPDB_Long, (SAPDB_Long)read( fd, (char *)buf, (size_t)nbyte ) );
}

SAPDB_Long RTE_save_nonblocking_read(RTE_FileHandle fd, void *buf, SAPDB_Long nbyte)
{
    RTESYS_CALL_IGNORING_ENOMEM_AND_EINTR( "read", fd, SAPDB_Long, (SAPDB_Long)read( fd, (char *)buf, (size_t)nbyte ) );
}

SAPDB_Long RTE_save_write(RTE_FileHandle fd, void *buf, SAPDB_Long nbyte)
{
    RTESYS_CALL_IGNORING_EAGAIN_ENOMEM_AND_EINTR( "write", fd, SAPDB_Long, (SAPDB_Long)write( fd, (char *)buf, (size_t)nbyte ) );
}

SAPDB_Long RTE_save_nonblocking_write(RTE_FileHandle fd, void *buf, SAPDB_Long nbyte)
{
    RTESYS_CALL_IGNORING_ENOMEM_AND_EINTR( "write", fd, SAPDB_Long, (SAPDB_Long)write( fd, (char *)buf, (size_t)nbyte ) );
}

SAPDB_Long RTE_save_writev(RTE_FileHandle fd, struct iovec *vector, SAPDB_Long count)
{
    RTESYS_CALL_IGNORING_EAGAIN_ENOMEM_AND_EINTR( "writev", fd, SAPDB_Long, (SAPDB_Long)writev( fd, vector, (size_t)count ) );
}

SAPDB_Int RTE_save_poll(struct pollfd fds[], SAPDB_Int nfds, SAPDB_Int timeout)
{
    RTESYS_CALL_IGNORING_EINTR( SAPDB_Int, poll(fds, nfds, timeout) );
}

SAPDB_Int RTE_save_unlink(SAPDB_Char const *pathname)
{
    RTESYS_CALL_IGNORING_EINTR( SAPDB_Int, unlink(pathname) );
}

SAPDB_Int RTE_save_semop(SAPDB_Int semid, struct sembuf *semops, SAPDB_UInt nsemops)
{
    RTESYS_CALL_IGNORING_EINTR( SAPDB_Int, (SAPDB_Int)semop(semid, semops, nsemops) );
}

SAPDB_Int RTE_save_semctl(SAPDB_Int semid, SAPDB_Int semnum, SAPDB_Int cmd, union semun arg)
{
#if defined(SAPDB_DEBUG_SEMAPHORES) /* normally not defined !!! See RTE_saveUNIXcalls.h */
    if ( cmd == IPC_RMID )
    {
        SAPDB_Int result;
        do {
            result = (SAPDB_Int)semctl(semid, semnum, cmd, arg);
        } while ( result == ((SAPDB_Int)-1)
               && errno == EINTR );
        {
            char outputLine[1024];
            int fd = open("/SAP_DB/data/wrk/remove_semaphores", O_CREAT|O_WRONLY|O_APPEND, 0666);
            if ( fd < 0 )
            {
                fd = open("remove_semaphores", O_CREAT|O_WRONLY|O_APPEND, 0666);
            }
            if ( fd >= 0 )
            {
                time_t now = time(0);
                ctime_r(&now, outputLine);
                sprintf(outputLine + strlen(outputLine), "RTE_: PID %8ld Create Semaphore %12d rc %d\n", getpid(), semid, result );
                write(fd, outputLine, strlen(outputLine));
                close(fd);
            }
        }
        return result;
    }
#endif
    RTESYS_CALL_IGNORING_EINTR( SAPDB_Int, (SAPDB_Int)semctl(semid, semnum, cmd, arg) );
}

SAPDB_Int RTE_save_shmctl(SAPDB_Int shmid, SAPDB_Int cmd, struct shmid_ds *shmState)
{
    RTESYS_CALL_IGNORING_EINTR( SAPDB_Int, (SAPDB_Int)shmctl(shmid, cmd, shmState) );
}
   
SAPDB_Int4 RTE_save_waitpid(SAPDB_Int4 pidToWaitFor, SAPDB_Int *pStatus, SAPDB_Int flags)
{
    RTESYS_CALL_IGNORING_EINTR( SAPDB_Int4, (SAPDB_Int4)waitpid(pidToWaitFor, pStatus, flags));
}
   
SAPDB_Int4 RTE_save_chmod(SAPDB_Char const *pathname, SAPDB_Int mode)
{
    RTESYS_CALL_IGNORING_EINTR( SAPDB_Int4, (SAPDB_Int4)chmod((char const *)pathname, (mode_t)mode));
}

SAPDB_Int4 RTE_save_access(SAPDB_Char const *pathname, SAPDB_Int mode)
{
    RTESYS_CALL_IGNORING_EINTR( SAPDB_Int4, (SAPDB_Int4)access((char const *)pathname, (mode_t)mode));
}

SAPDB_Int4 RTE_save_rename(SAPDB_Char const *oldPathname, SAPDB_Char const *newPathname)
{
    RTESYS_CALL_IGNORING_EINTR( SAPDB_Int4, (SAPDB_Int4)rename((char const *)oldPathname, (char const *)newPathname));
}

SAPDB_Char *RTE_save_getcwd(SAPDB_Char *pathBuffer, SAPDB_ULong bufferSize)
{
    RTESYS_CALL_IGNORING_EINTR( SAPDB_Char *, (SAPDB_Char *)getcwd((char *)pathBuffer, (size_t)bufferSize));
}

void *RTE_save_malloc(SAPDB_ULong size)
{
   if ( 0 == size )
   {
       size = 1;
   }

   {
       RTESYS_CALL_IGNORING_EINTR( void *, (void *)malloc(size));
   }
}
   
void RTE_save_free(void * pointer)
{
   if ( pointer )
   {
       free(pointer);
   }
}
   
SAPDB_Int4 RTE_save_fork()
{
    RTESYS_CALL_IGNORING_EINTR( SAPDB_Int4, (SAPDB_Int4)fork());
}
   
SAPDB_Int4 RTE_save_vfork()
{
    RTESYS_CALL_IGNORING_EINTR( SAPDB_Int4, (SAPDB_Int4)vfork());
}
   
SAPDB_Int4 RTE_save_dup2(SAPDB_Int fd, SAPDB_Int fdTarget)
{
    RTESYS_CALL_IGNORING_EINTR( SAPDB_Int4, (SAPDB_Int4)dup2(fd, fdTarget));
}
   
SAPDB_Int4 RTE_save_pipe(SAPDB_Int fdPair[2])
{
    RTESYS_CALL_IGNORING_EINTR( SAPDB_Int4, (SAPDB_Int4)pipe(fdPair));
}

SAPDB_Int4 RTE_save_execv (SAPDB_Char const * executable,
                           SAPDB_Char * const argv[])
{
    RTESYS_CALL_IGNORING_EINTR( SAPDB_Int4, (SAPDB_Int4)execv(executable, argv));
}

SAPDB_Int4 RTE_save_execvp(SAPDB_Char const * executable, SAPDB_Char * const argv[])
{
    RTESYS_CALL_IGNORING_EINTR( SAPDB_Int4, (SAPDB_Int4)execvp(executable, argv));
}

SAPDB_Int4 RTE_save_execve(SAPDB_Char const * executable,
                           SAPDB_Char * const argv[],
                           SAPDB_Char * const envp[])
{
    RTESYS_CALL_IGNORING_EINTR( SAPDB_Int4, (SAPDB_Int4)execve(executable, argv, envp));
}

void       RTE_save_exit(SAPDB_Int status)
{
    exit(status);
}

void       RTE_save__exit(SAPDB_Int status)
{
    _exit(status);
}

SAPDB_Int4 RTE_save_chown(SAPDB_Char const * path, SAPDB_Int4 userId, SAPDB_Int4 groupId)
{
    RTESYS_CALL_IGNORING_EINTR( SAPDB_Int4, (SAPDB_Int4)chown(path, (uid_t)userId, (gid_t)groupId));
}

SAPDB_Int4 RTE_save_fchown(SAPDB_Int4 fildes, SAPDB_Int4 userId, SAPDB_Int4 groupId)
{
    RTESYS_CALL_IGNORING_EINTR( SAPDB_Int4, (SAPDB_Int4)fchown(fildes, (uid_t)userId, (gid_t)groupId));
}

SAPDB_Int4 RTE_save_geteuid()
{
    RTESYS_CALL_IGNORING_EINTR( SAPDB_Int4, (SAPDB_Int4)geteuid() );
}

SAPDB_Int4 RTE_save_getuid()
{
    RTESYS_CALL_IGNORING_EINTR( SAPDB_Int4, (SAPDB_Int4)getuid() );
}

SAPDB_Int4 RTE_save_getegid()
{
    RTESYS_CALL_IGNORING_EINTR( SAPDB_Int4, (SAPDB_Int4)getegid() );
}

SAPDB_Int4 RTE_save_getgid()
{
    RTESYS_CALL_IGNORING_EINTR( SAPDB_Int4, (SAPDB_Int4)getgid() );
}

SAPDB_Int4 RTE_save_getppid()
{
    RTESYS_CALL_IGNORING_EINTR( SAPDB_Int4, (SAPDB_Int4)getppid() );
}

SAPDB_Int4 RTE_save_getpid()
{
    RTESYS_CALL_IGNORING_EINTR( SAPDB_Int4, (SAPDB_Int4)getpid() );
}

SAPDB_Int4 RTE_save_setuid(SAPDB_Int4 userId)
{
    RTESYS_CALL_IGNORING_EINTR( SAPDB_Int4, (SAPDB_Int4)setuid((uid_t)userId) );
}

SAPDB_Int4 RTE_save_setreuid(SAPDB_Int4 userId, SAPDB_Int4 effectivUserId)
{
    RTESYS_CALL_IGNORING_EINTR( SAPDB_Int4, (SAPDB_Int4)setreuid((uid_t)userId, (uid_t)effectivUserId) );
}

SAPDB_Int4 RTE_save_setgid(SAPDB_Int4 groupId)
{
    RTESYS_CALL_IGNORING_EINTR( SAPDB_Int4, (SAPDB_Int4)setgid((gid_t)groupId) );
}

SAPDB_Int4 RTE_save_setregid(SAPDB_Int4 groupId, SAPDB_Int4 effectivGroupId)
{
    RTESYS_CALL_IGNORING_EINTR( SAPDB_Int4, (SAPDB_Int4)setregid((gid_t)groupId, (gid_t)effectivGroupId) );
}

void *     RTE_save_mmap (void *pBaseAddress, SAPDB_ULong size, SAPDB_Int prot, 
                          SAPDB_Int flags, SAPDB_Int fildes, SAPDB_ULong off)
{
    RTESYS_CALL_IGNORING_EINTR(void *, (void *)mmap ((char *)pBaseAddress, size, prot, 
        flags, fildes, off));
}

SAPDB_Int4 RTE_save_munmap(void * addr, SAPDB_ULong len)
{
    RTESYS_CALL_IGNORING_EINTR(SAPDB_Int4, (SAPDB_Int4)munmap ((char *)addr, len));
}

SAPDB_Int4 RTE_save_ftruncate (SAPDB_Int fildes, SAPDB_ULong length)
{
    RTESYS_CALL_IGNORING_EINTR(SAPDB_Int4, (SAPDB_Int4)ftruncate (fildes, length));
}


SAPDB_Int4 RTE_save_shm_open (SAPDB_Char const * name, SAPDB_Int oflag, 
                              SAPDB_Int mode)
{
    RTESYS_CALL_IGNORING_EINTR(SAPDB_Int4, (SAPDB_Int4)shm_open (name, oflag, (mode_t)mode));
}

SAPDB_Int4 RTE_save_shm_unlink(SAPDB_Char const *name)
{
    RTESYS_CALL_IGNORING_EINTR(SAPDB_Int4, (SAPDB_Int4)shm_unlink (name));
}

SAPDB_Char const *  RTE_save_strerror (RTE_SystemRc const  err)
{
    SAPDB_Char *    errMsg;

    errMsg = strerror  (err);
    if ( !errMsg )
    {
        return "unknown";
    }
    else
    {
        return errMsg;
    }
}

SAPDB_Int4 RTE_save_kill(SAPDB_Int4 pidToSignal, SAPDB_Int4 signalCode)
{
    RTESYS_CALL_IGNORING_EINTR( SAPDB_Int4, (SAPDB_Int4)kill((pid_t)pidToSignal, signalCode));
}
   
SAPDB_Int4 RTE_save_getsid(SAPDB_Int4 pidToGetSessionIdFrom)
{
    RTESYS_CALL_IGNORING_EINTR( SAPDB_Int4, (SAPDB_Int4)getsid((pid_t)pidToGetSessionIdFrom) );
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/

#endif /* UNIX only */
