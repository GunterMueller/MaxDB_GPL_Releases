/*!
  @file           ven41.c
  @author         JoergM
  @brief          Client/Kernel RunTime: ipc_resources
  @see


  Create and Delete IPC Resources

\if EMIT_LICENCE

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



//
//   FreeBSD portions added by Kai Mosebach,
//   For more informations see : http://www.komadev.de/sapdb
//
\endif
*/
#define MOD__ "ven41.c:"

/* exported functions */

#include "heo41.h"
#include "gen41.h"
/* PTS 1106477 */
#include "hen40.h"
#include "hsp77.h"
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include "heo01.h"
/* PTS 1108085 */
/* With RedHat6.2 the mmap() prototype was fixed. This prevented the kernel from */
/* starting. The message given was 'invalid argument'. This Message was given due */
/* to the last argument of mmap() which was defined as 'off_t' and later 'off64_t' */
/* So on RedHat6.2 the last argument was expected to be a 64bit value, therefore */
/* the extra 32bits are collected from the stack. Such an offset is not PAGESIZE */
/* aligned, so it leads to 'invalid argument'... */
#undef __USE_FILE_OFFSET64
#define __USE_LARGEFILE64
#include <sys/mman.h>
/* imported functions */
#include <dirent.h>
#include "geo007_1.h"
#include "SAPDBCommon/SAPDB_Names.h"
#include "RunTime/RTE_IniFileHandling.h"
#include "RunTime/RTE_UNIXAuthenticate.h"

/*
 * IPC Key Range: Begin with 0x44000000 and continue up to 0x44080000
 * allowing 128 simultanous database instances
 */
#define IPC_KEY_START_VALUE_EN41 (key_t)( 'D' << 24 )
#define IPC_KEY_STEP_EN41        (0x10000)
#define IPC_KEY_MAX_OFFSET_EN41 (0x800000)
#define MAX_SPEED_BUFFER_LEN_EN41 16
#define MAX_NUMBER_BUFFER_LEN_EN41 16

/* local functions */

static  int     sql41_remove_ipc_and_dir ( char *dir );
static  int     en41_RemoveSemaphore(int semid);
static  key_t   en41_ValidateKey(key_t key);
static  void    en41_GetPipeFDDirectory ( tsp00_Pathc file );
static  void    en41_GetDbSpeedDirectory ( tsp00_Pathc file );
static  int     en41_CreateTagFile( const char *dbname,
                                    tsp00_Pathc path,
                                    tsp00_TaskId pid );
static  int     en41_ReadTagFile( const char *dbname,
                                  tsp00_Pathc path,
                                  tsp00_TaskId *pid );
static  int     en41_Unlink( char *dir,
                             char *entry );
static int      en41_SearchFreeKeyAndAllocInitialShm( SAPDB_Int4 initialSize,
                                                      key_t *pShmKey,
                                                      int *pShmId );
externC	int	sql41_check_unprotected_dir (tsp00_Pathc Vfile);

/*
 * ===========================================================================
 */
static int en41_CreateSem (
int                             mode ,
uid_t                           uid ,
char                          * type ,
char                          * dbname )
{
#undef MF__
#define MF__ MOD__"en41_CreateSem"
int                             semid;
int                             rc ;
union semun                     arg ;
struct semid_ds                 sbuf ;


    DBGIN;

    semid = semget ( IPC_PRIVATE, 1 , mode);
#if defined(SAPDB_DEBUG_SEMAPHORES) /* normally not defined !!! See RTE_saveUNIXcalls.h */
    if ( semid >= 0 )
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
            sprintf(outputLine + strlen(outputLine), "en41: PID %8ld Create Semaphore %12d\n", getpid(), semid);
            write(fd, outputLine, strlen(outputLine));
            close(fd);
        }
    }
#endif

/*
 *  won't use a zero id
 */
	if ( semid == 0 )
    {
		semid = semget ( IPC_PRIVATE, 1, mode);
#if defined(SAPDB_DEBUG_SEMAPHORES) /* normally not defined !!! See RTE_saveUNIXcalls.h */
        if ( semid > 0 )
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
                sprintf(outputLine + strlen(outputLine), "en41: PID %8ld Create Semaphore %12d\n", getpid(), semid );
                write(fd, outputLine, strlen(outputLine));
                close(fd);
            }
        }
#endif
        (void) en41_RemoveSemaphore( 0 );
    }

    if ( semid < 0 )
    {
        DBG1 (( MF__,"semget parameter used:  nsems 1 semflg %d\n",
    		 mode|IPC_CREAT|IPC_EXCL ));
		MSGD (( ERR_IPC_SEMC_SEMGET, sqlerrs() ));
		return ( -1 );
    }
	DBG1 (( MF__," semid %d \n", semid ));

/*
 *  transfer ownership
 */
	arg.buf = & sbuf ;
	rc = semctl ( semid , 0 , IPC_STAT , SEMCTL_IPC_STAT(arg) );
	if ( rc < 0 )
    {
		MSGD (( ERR_IPC_SEMC_SEMCTL_STAT, /* semid, */ sqlerrs() ));
        (void) en41_RemoveSemaphore( semid );
		return ( -1 );
    }
	if ( sbuf.sem_perm.uid != uid )
    {
		sbuf.sem_perm.uid = uid ;
		arg.buf = & sbuf ;
		rc = semctl ( semid , 0 , IPC_SET , SEMCTL_IPC_SET(arg) );
		if ( rc < 0 )
		{
			MSGD (( ERR_IPC_SEMC_SEMCTL_SET, /* semid, */ sqlerrs() ));
            (void) en41_RemoveSemaphore( semid );
			return ( -1 );
		}
    }

	rc = sql41_create_idfile ( type , dbname , 's' , semid );
	if ( rc )
    {
		MSGD (( ERR_IPC_SEMC_IDFILE ));
        (void) en41_RemoveSemaphore( semid );
		return ( -1 );
    }

	DBG1 (( MF__,"semid %d \n", semid ));
    DBGOUT;
	return ( semid );
}

/* --------------------------------------------- */

externC	int	sql41_create_sem (
key_t                           key ,
int                             ref ,
int                             range ,
int                             mode ,
uid_t                           uid ,
char                            * type ,
char                            * dbname )
{
#undef MF__
#define MF__ MOD__"sql41_create_sem"
    tsp00_Pathc    IndepConfigPath ; /* PTS 1125117 */
    tsp00_Pathc    UPCContainerPath;
    tsp01_RteError RteError ;
    struct stat statVal;
    DBGPAS;
    /* key,ref and range are ignored since all semaphores can be created as IPC_PRIVATE. Other processes can access
       this semaphore by using the ID which is returned by 'semget' */

    /* The kernel must be able to remove the semaphore when going offline */
    /* So it is donated to the user the kernel is running under */
    /* This user is the one the .upc file belongs to. */
    if ( !sqlGetIndependentConfigPath(IndepConfigPath, TERM_WITH_DELIMITER_EO01, &RteError) )
	{
		MSGD (( ERR_IPC_IDFILE_OPEN, "GetIndepConfigPath", RteError.RteErrText ));
	    return -1;
	}

	if ( (strlen(dbname) + strlen(IndepConfigPath) + strlen(".upc")) >= sizeof(tsp00_Path) )
	{
		MSGD (( ERR_IPC_IDFILE_OPEN, "UserProfileContainer", "path too long" ));
	    return -1;
    }

    sp77sprintf(UPCContainerPath,sizeof(UPCContainerPath),"%s%s.upc", IndepConfigPath, dbname);
    if(0==stat(UPCContainerPath,&statVal))
    {
        uid = statVal.st_uid;
    }
	else
	{
		MSGD (( ERR_IPC_IDFILE_OPEN, "UserProfileContainer(stat)", sqlerrs() ));
		return -1;
	}
    return en41_CreateSem (mode, uid, type, dbname );

}

/*
 * ===========================================================================
 */

externC	int     sql41_remove_sem (
int                             * semid ,
char                            * type ,
char                            * dbname )
{
#undef MF__
#define MF__ MOD__"sql41_remove_sem"
    int rc ;

	if ( * semid <= 0 ) return ( 0 );

    rc = en41_RemoveSemaphore( *semid );

    if ( rc == 0 )
    {
    	rc = sql41_remove_idfile ( type , dbname , 's' , * semid );
    	DBG1 (( MF__,"removed semid %d \n", * semid ));
    	* semid = -1 ;
    }

	return ( rc );
}

/*
 * ===========================================================================
 */

static int	en41_CreateShm (
key_t                           key ,
int                             ref ,
int                             size ,
int                             mode ,
uid_t                           uid ,
char                            * type ,
char                            * dbname )
{
#undef MF__
#define MF__ MOD__"en41_CreateShm"
int                             shmid , rc ;
struct shmid_ds                 mbuf ;
key_t                           wantedKey;

    DBGIN;

    if ( key == IPC_PRIVATE )
    {
        wantedKey = key;
		shmid = shmget ( wantedKey , size , mode|IPC_CREAT|IPC_EXCL );
    }
    else
    {
        wantedKey = en41_ValidateKey( (key_t)((int)key + ref) );
        do {
	  shmid = shmget ( wantedKey , size , mode|IPC_CREAT|IPC_EXCL );
          if ( shmid >=0 )
            break ;
          wantedKey = (key_t)(1+(int)wantedKey);
        } while ( (errno == EEXIST)
               && ( ((int)wantedKey) < (IPC_KEY_START_VALUE_EN41+IPC_KEY_MAX_OFFSET_EN41) ) );
    }
/*
 *  won't use a zero id
 */
	if ( shmid == 0 )
    {
		(void) shmctl ( shmid , IPC_RMID , & mbuf );
		shmid = shmget ( wantedKey, size, mode|IPC_CREAT|IPC_EXCL );
    }

	if ( shmid < 0 )
    {
#ifdef DEBUG_RTE
        if ( key != IPC_PRIVATE )
        {
            /* It is just debugging code, to be able to report right input... */
            wantedKey = (key_t)(-1+(int)wantedKey);
        }
#endif /* DEBUG_RTE */
    	DBG1 (( MF__,"shmget parameter used: key 0x%0x size %d, shmflg %d\n",
    		    (int)wantedKey, size, mode|IPC_CREAT|IPC_EXCL ));
		MSGD (( ERR_IPC_SHMC_SHMGET, /* (int)wantedKey, */ sqlerrs() ));
		return ( -1 );
    }
	DBG1 (( MF__,"key 0x%0x shmid %d \n", (int) wantedKey, shmid ));

/*
 *  transfer ownership
 */
	rc = shmctl ( shmid , IPC_STAT , & mbuf );
	if ( rc < 0 )
    {
		MSGD (( ERR_IPC_SHMC_SHMCTL_STAT, sqlerrs() ));
		(void) shmctl ( shmid , IPC_RMID , & mbuf );
		return ( -1 );
    }
	if ( mbuf.shm_perm.uid != uid )
    {
		mbuf.shm_perm.uid = uid ;
		rc = shmctl ( shmid , IPC_SET , & mbuf );
		if ( rc < 0 )
		{
			MSGD (( ERR_IPC_SHMC_SHMCTL_SET, sqlerrs() ));
			(void) shmctl ( shmid , IPC_RMID , & mbuf );
			return ( -1 );
		}
    }

	rc = sql41_create_idfile ( type , dbname , 'm' , shmid );
	if ( rc )
    {
		MSGD(( ERR_IPC_SHMC_IDFILE ));
		(void) shmctl ( shmid , IPC_RMID , & mbuf );
		return ( -1 );
    }

	DBG1 (( MF__,"key 0x%0x shmid %d \n", (int)wantedKey, shmid ));
    DBGOUT;
	return ( shmid );
}

/* --------------------------------------------- */

#undef MF__
#define MF__ MOD__"sql41_create_shm"
externC	int	sql41_create_shm (
key_t                           key ,
int                             ref ,
int                             range ,
int                             size ,
int                             mode ,
uid_t                           uid ,
char                            * type ,
char                            * dbname )
{
    SAPDB_Int4 sdbOwnerUserId;
    DBGPAS;
    if ( RTE_GetSapdbOwnerUserId(&sdbOwnerUserId) )
    {
        uid = (uid_t)sdbOwnerUserId;
    }
    return en41_CreateShm(key == 0 ? (IPC_KEY_START_VALUE_EN41 + getpid()) : key,
                          ref, size, mode, uid, type, dbname );
}

/*
 * ===========================================================================
 */

externC	int     sql41_remove_shm ( int  * shmid ,
                                   char * type ,
                                   char * dbname )
{
#undef MF__
#define MF__ MOD__"sql41_remove_shm"
int                             rc ;
struct shmid_ds                 mbuf ;

	if ( * shmid <= 0 )
		return ( 0 );

	DBG1 (( MF__,"removing  shmid %d \n", * shmid  ));
	rc = shmctl ( * shmid , IPC_RMID , & mbuf );
	if ( rc )
	{
		if ( errno != EIDRM && errno != EINVAL )
		{
			MSGD (( ERR_IPC_SHMR_SHMCTL, * shmid, sqlerrs() ));
			return ( -1 );
		}
	}

	rc = sql41_remove_idfile ( type , dbname , 'm' , * shmid );

	DBG1 (( MF__,"removed shmid %d \n", * shmid ));
	* shmid = -1 ;
	return ( rc );
}

/*
 * !!!!!!!!! CR 1000130 !!!!!!!!! *
 * ===========================================================================
 */

externC	char    *sql41AttachShm ( int   shmid ,
                                  char *address,
                                  int   shmflag )
{
#undef MF__
#define MF__ MOD__"sql41AttachShm"
char * addr ;

	DBG1 (( MF__,"attaching id %d at 0x%08lx \n",
				shmid , (long) address ));
	addr = shmat ( shmid , address , shmflag );
	if ( addr == (char*) -1 )
	{
		MSGD (( ERR_IPC_SHMA_SHMAT, sqlerrs(), shmid, shmflag, address ));
		sql41_stat_shm ( shmid );
		return ( NULL );
	}
	DBG1 (( MF__,"attached  shmseg 0x%08lx \n", (long) addr ));

	if ( address && address != addr )
	{
		MSGD (( ERR_IPC_SHMA_WRONG_ADDR, addr ));
		(void) shmdt ( addr );
		return ( NULL );
	}
	DBG1 (( MF__,"attached id %d at 0x%08lx \n",
					    shmid , (long) addr ))

	return ( addr );
}

/*
 * ===========================================================================
 */
externC	char    *sql41_attach_shm ( int   shmid ,
                                    char *address )
{
#undef MF__
#define MF__ MOD__"sql41_attach_shm"
int                             flag ;

/*IST 1994-11-22* if ( address == NULL ) flag = SHM_RND ; */
flag = 0 ;

return ( sql41AttachShm (shmid, address, flag)  );
}

/*
 * ===========================================================================
 */
externC	int     sql41_detach_shm (
char                            ** address )
{
#undef MF__
#define MF__ MOD__"sql41_detach_shm"
int                             rc ;

	if ( * address == NULL || * address == (char*) -1 )
			return ( 0 );

	DBG1 (( MF__,"detaching shmseg 0x%08lx \n", (long) *address ));
	rc = shmdt ( * address );
	if ( rc < 0 )
	{
		MSGD (( ERR_IPC_SHMD_SHMDT, *address, sqlerrs() ));
		return ( -1 );
	}
	DBG1 (( MF__,"detached shmseg 0x%08lx \n", (long) *address ));
	* address = NULL ;

	return ( 0 );
}

/*
 * ===========================================================================
 */
externC	void    sql41_stat_sem (
int                             semid )
{
#undef MF__
#define MF__ MOD__"sql41_stat_sem"
int                             rc ;
union semun                     arg ;
struct semid_ds                 sbuf ;

	MSGD (( ERR_IPC_SEMS_WHICH, semid ));
	if ( semid <= 0 )
    {
		return ;
	}

	arg.buf = & sbuf ;
	rc = semctl ( semid , 0 , IPC_STAT , SEMCTL_IPC_STAT(arg) );
	if ( rc < 0 )
	{
		MSGD (( ERR_IPC_SEMS_SHMCTL, semid, sqlerrs() ));
		return ;
	}

	MSGD (( ERR_IPC_SEMS_VALUE,    semctl ( semid , 0 , GETVAL, 0 ) /**/ ));
	MSGD (( ERR_IPC_SEMS_NUM_SEMS, sbuf.sem_nsems));
    {
        char timeBuffer[32]; /* manual page for time: at least 26 bytes... */
#if (defined(SUN) || defined(SOLARIS)) && !defined(_POSIX_PTHREAD_SEMANTICS)
        MSGD (( ERR_IPC_SEMS_LASTOP_TIME, ctime_r( &sbuf.sem_otime, &timeBuffer[0], sizeof(timeBuffer)) ));
        MSGD (( ERR_IPC_SEMS_LASTCHG_TIME, ctime_r( &sbuf.sem_ctime, &timeBuffer[0], sizeof(timeBuffer)) ));
#else
        MSGD (( ERR_IPC_SEMS_LASTOP_TIME, ctime_r( &sbuf.sem_otime, &timeBuffer[0]) ));
        MSGD (( ERR_IPC_SEMS_LASTCHG_TIME, ctime_r( &sbuf.sem_ctime, &timeBuffer[0]) ));
#endif
    }
	MSGD (( ERR_IPC_SEMS_LASTOP_PID, semctl ( semid , 0 , GETPID , 0 ) /**/ ));
	MSGD (( ERR_IPC_SEMS_SEMNCNT, semctl ( semid , 0 , GETNCNT, 0 ) /**/ ));
	MSGD (( ERR_IPC_SEMS_SEMZCNT, semctl ( semid , 0 , GETZCNT, 0 ) /**/ ));
}

/*
 * ===========================================================================
 */
externC	void    sql41_stat_shm (
int                             shmid )
{
#undef MF__
#define MF__ MOD__"sql41_stat_shm"
int                             rc ;
struct shmid_ds                 mbuf ;

	MSGD (( ERR_IPC_SHMS_WHICH, shmid ));
	if ( shmid <= 0 ) return ;

	rc = shmctl ( shmid , IPC_STAT , & mbuf );
	if ( rc < 0 )
	{
		MSGD (( ERR_IPC_SHMS_SHMCTL, sqlerrs() ));
		return ;
	}

	MSGD (( ERR_IPC_SHMS_SIZE, (long) mbuf.shm_segsz ));
	MSGD (( ERR_IPC_SHMS_ATTACHES, (long) mbuf.shm_nattch ));
	MSGD (( ERR_IPC_SHMS_CREATOR_UID, (long) mbuf.shm_perm.cuid ));
	MSGD (( ERR_IPC_SHMS_OWNER_UID, (long) mbuf.shm_perm.uid ));
	MSGD (( ERR_IPC_SHMS_PERMISSIONS, (long) mbuf.shm_perm.mode ));
	MSGD (( ERR_IPC_SHMS_CREATOR_PID, (long) mbuf.shm_cpid ));
	MSGD (( ERR_IPC_SHMS_LAST_OP_PID, (long) mbuf.shm_lpid ));
}

/*
 * ===========================================================================
 */
externC	SAPDB_ULong sql41_getShmSize (int shmid)
{
    int             rc ;
    struct shmid_ds mbuf ;

	if ( shmid < 0 ) return (SAPDB_ULong)0;

	rc = shmctl ( shmid , IPC_STAT , & mbuf );
	if ( rc < 0 )
	{
		return (SAPDB_ULong)0;
	}

	return (SAPDB_ULong)mbuf.shm_segsz;
}

/*
 * ===========================================================================
 */
externC	int     sql41_create_fifo (
                                   tsp00_Pathc file ,
                                   int         perm )
{
#undef MF__
#define MF__ MOD__"sql41_create_fifo"
  /* PTS 1105151 */
  /* For fifo connections it is needed to have '0666' access to allow all users to use it */
  mode_t                          umask_old ;
  int                             rc ;

  DBG1 (( MF__,"create fifo '%s' \n", (char *)&file[0] ));

  /* Absolut file permission specified, so save old umask setting */
  umask_old = umask ( 0 );
#ifndef FREEBSD /* mkfifo fails */
  rc = mknod ( file , S_IFIFO | (perm & 0666) , 0 );
#else
  rc = mkfifo ( file , (perm & 0666) );
#endif
  if ( rc < 0 )
  {
    MSGD (( ERR_IPC_FIFOC_MKNOD, file, perm, sqlerrs() ));
    (void) umask ( umask_old );
    return ( -1 );
  }
  (void) umask ( umask_old );

  return ( 0 );
}

/*
 * ===========================================================================
 */
externC	int     sql41_remove_fifo (
                                   tsp00_Pathc file )
{
#undef MF__
#define MF__ MOD__"sql41_remove_fifo"
  int                             rc ;

  DBG1 (( MF__,"remove fifo '%s' \n", &file[0] ));

  rc = en41_Unlink ( "", file );
  if ( (rc < 0) && (errno != ENOENT) )
  {
    MSGD (( ERR_IPC_FIFOR_UNLINK, file, sqlerrs() ));
    return ( -1 );
  }
  return ( 0 );
}

/*
 * ===========================================================================
 */
externC	void     sql41_get_request_fifo_name (
tsp00_Pathc  file ,
char		*dbname )
{
#undef MF__
#define MF__ MOD__"sql41_get_request_fifo_name"
	sql41_get_fifo_dir ( file );
	(void) strcat ( file , dbname );
	DBG1 (( MF__,"file '%s' \n", file ));
}

/*
 * ===========================================================================
 */
externC	void     sql41_get_reply_fifo_name (
tsp00_Pathc  file ,
char		*dbname ,
pid_t	 	 pid ,
tsp00_Int4	 ref )
{
#undef MF__
#define MF__ MOD__"sql41_get_reply_fifo_name"
	sql41_get_ipc_dir ( file );
	sp77sprintf ( file + strlen(file), sizeof(tsp00_Pathc) - strlen(file), SAPDB_USER_DIRECTORY_PREFIX "%s/", dbname );
    (void) sql41_check_unprotected_dir ( file );
	sp77sprintf ( file + strlen(file), sizeof(tsp00_Pathc) - strlen(file), "FIFO-%ld.%d", (long) pid , (int) ref );
	DBG1 (( MF__,"file '%s' \n", file ));
}

/*
 * ===========================================================================
 */
externC	int     sql41_create_idfile (
char                            * usertype ,
char                            * dbname ,
char                            restype ,
int                             ipcid )
{
#undef MF__
#define MF__ MOD__"sql41_create_idfile"
int                             rc ;
tsp00_Pathc                file ;

	sql41_get_idfile_name ( file , usertype , dbname , restype , ipcid );
	DBG1 (( MF__,"create file '%s' \n", file ));

	rc = open ( file , O_CREAT , 0400 );
	if ( rc < 0 )
    {
		MSGD (( ERR_IPC_IDFILE_OPEN, file, sqlerrs() ));
		return ( -1 );
    }
	(void) close ( rc );

	return ( 0 );
}

/*
 * ===========================================================================
 */
externC	int     sql41_remove_idfile (
char                            * usertype ,
char                            * dbname ,
char                            restype ,
int                             ipcid )
{
#undef MF__
#define MF__ MOD__"sql41_remove_idfile"
tsp00_Pathc file ;

	sql41_get_idfile_name ( file , usertype , dbname , restype , ipcid );
	DBG1 (( MF__,"remove file '%s' \n", file ));

	return ( en41_Unlink ( "", file ) );
}

/*
 * ===========================================================================
 */
static int
en41_SearchFreeKeyAndAllocInitialShm( SAPDB_Int4 initialSize,
                                      key_t *pShmKey,
                                      int *pShmId)
{
#undef MF__
#define MF__ MOD__"en41_SearchFreeKeyAndAllocInitialShm"

    key_t testKey;
    int testId;
    int i;

    for ( i = 0; i < IPC_KEY_MAX_OFFSET_EN41; i += IPC_KEY_STEP_EN41 )
    {
        do
        {
            testKey = (key_t) ( IPC_KEY_START_VALUE_EN41 + i );
            testId = shmget(testKey, initialSize, 0660|IPC_CREAT|IPC_EXCL);
            if ( testId > 0 )
                break;
        } while ( 0 == testId );

        if ( testId > 0 )
                break;
    }

    if ( testId < 0 )
    {
    	MSGD(( ERR_KERNEL_BASE_KEY,
            IPC_KEY_START_VALUE_EN41,
            IPC_KEY_START_VALUE_EN41 + IPC_KEY_MAX_OFFSET_EN41,
            IPC_KEY_STEP_EN41 ));
    }
    else
    {
        MSGD(( INFO_FOR_IPC_RESOURCES , (long)testKey ))
    }

    *pShmId = testId;
    *pShmKey = testKey;

    return ( testId >= 0 );
}
/*
 * ===========================================================================
 */
externC	int
en41CreateServerKeyAndFirstSegment ( tsp00_DbNamec dbname,
                                     SAPDB_Int4 initialSize,
                                     key_t *pBaseKey,
                                     int   *pBaseShmId )
{
#undef MF__
#define MF__ MOD__"en41CreateServerKeyAndFirstSegment"

    tsp00_Pathc                     file ;
    int                             fd;
    size_t                          result;

    DBGIN;

    if ( !en41_SearchFreeKeyAndAllocInitialShm( initialSize,
                                                pBaseKey,
                                                pBaseShmId) )
    {
        return false;
    }

	if ( sql41_create_idfile ( "db" ,
                               dbname ,
                               'm' ,
                               *pBaseShmId ) )
    {
		MSGD(( ERR_IPC_SHMC_IDFILE ));

        (void) shmctl ( *pBaseShmId ,
                        IPC_RMID ,
                        (struct shmid_ds*) 0 );
		return false;
    }

	sql41_get_ipc_dir ( file );

    sp77sprintf ( file + strlen(file),
                  sizeof(tsp00_Pathc) - strlen(file),
                  SAPDB_DATABASE_DIRECTORY_PREFIX "%s/",
                  dbname );

    if ( sql41_check_dir ( file ) )
    {
        sql41_remove_shm( pBaseShmId,
                          "db",
                          dbname );
        return false;
    }

	sp77sprintf ( file + strlen(file),
                  sizeof(tsp00_Pathc) - strlen(file),
                  "%s",
                  dbname );

    /*
     * Special keyfile containing 'commonRTEKey as binary...'
     */
    fd = open( file,
               O_WRONLY|O_CREAT|O_EXCL,
               0644 );

    if ( fd < 0 )
    {
        MSGD(( ERR_OPEN_SYSCALL,
               file,
               O_WRONLY|O_CREAT|O_EXCL,
               sqlerrs() ));

        sql41_remove_shm( pBaseShmId,
                          "db",
                          dbname );
        return false;
    }

    result = write( fd,
                    pBaseKey,
                    sizeof(key_t));

    if ( result != sizeof(key_t) )
    {
        MSGD(( ERR_WRITE_SYSCALL,
               file,
               sizeof(key_t),
               result,
               sqlerrs() ));

        close(fd);

        sql41_remove_shm( pBaseShmId,
                          "db",
                          dbname );

        (void) en41_Unlink( "",
                            file );
        return false;
    }

    close(fd);

    DBGOUT;

    return true;
}

/*
 * ===========================================================================
 */
externC	key_t
sql41_get_server_key ( tsp00_DbNamec dbname )
{
#undef MF__
#define MF__ MOD__"sql41_get_server_key"

    key_t                      serverKey ;
    tsp00_Pathc                file ;
    int                        fd;

	sql41_get_ipc_dir ( file );
	sp77sprintf ( file + strlen(file), sizeof(tsp00_Pathc) - strlen(file), SAPDB_DATABASE_DIRECTORY_PREFIX "%s/%s", dbname, dbname );

    fd = open( file, O_RDONLY);
    if ( fd < 0 )
    {
        return (key_t)-2;
    }

    if ( read(fd, &serverKey, sizeof(key_t)) != sizeof(key_t))
    {
        close(fd);
        return (key_t)-2;
    }

    close(fd);

	DBG1 (( MF__,"key 0x%08lx \n", (long) serverKey ));

	return ( serverKey );
}
/*
 * ===========================================================================
 */

externC	void    sql41_get_idfile_name (
tsp00_Pathc                  Vfile ,
char                            * usertype ,
char                            * dbname ,
char                            restype ,
int                             ipcid )
{
#undef MF__
#define MF__ MOD__"sql41_get_idfile_name"
char *file = (char *)&Vfile[0];

	sql41_get_ipc_dir ( file );
	sp77sprintf ( file + strlen(file), sizeof(tsp00_Pathc) - strlen(file), "%s:%s/", usertype, dbname );
    if ( !strcmp(SAPDB_USER_DIRECTORY_PREFIX, usertype) )
    {
    	sql41_check_unprotected_dir ( file );
    }
    else
    {
    	sql41_check_dir ( file );
    }
	sp77sprintf ( file + strlen(file), sizeof(tsp00_Pathc) - strlen(file), "-%c%d", restype , ipcid );
	DBG1 (( MF__,"file '%s' \n", file ));
}

/*
 * ===========================================================================
 */
/*
  Function: en41_CreateTagFile
  Description: Create a tag file and store a pid
  Arguments: dbname [in] database name
             path [inout] path to tag file directory
             pid [in] pid to store in file
  Return value: -1 on error, 0 on success
 */
static int en41_CreateTagFile(const char *dbname, tsp00_Pathc path, tsp00_TaskId pid)
{
#undef MF__
#define MF__ MOD__"en41_CreateTagFile"
    char *mostSignificantDigit;
    char numberBuffer[MAX_NUMBER_BUFFER_LEN_EN41];
    unsigned int pidNumber = (unsigned int)pid;
    int fd;
    mode_t umask_old ;


    /* check and create directory if not existing */
	if ( sql41_check_dir ( path ) != 0 )
    {
        return -1;
    }

    /* Build file name */
    if ( (strlen(path) + strlen(dbname) + 1 ) > sizeof(tsp00_Pathc) )
    {
		MSGD (( ERR_IPC_IDFILE_FOPEN, path, "dbname too long" ));
        return -1;
    }
    strcat(path, dbname);

    /* Fill pid number into numberBuffer */
    SAPDB_memset(numberBuffer, 0, sizeof(numberBuffer));
    numberBuffer[sizeof(numberBuffer)-2] = '\n';
    mostSignificantDigit = &numberBuffer[sizeof(numberBuffer)-2];
    do
    {
        --mostSignificantDigit;
        *mostSignificantDigit = '0' + (pidNumber%10);
        pidNumber /= 10;
    } while ( pidNumber > 0 );

    /* write it into file */
    umask_old = umask (0);
    fd = open( path, O_RDWR|O_CREAT, 0644 );
    umask (umask_old);
    if ( fd < 0 )
    {
		MSGD (( ERR_IPC_IDFILE_OPEN, path, sqlerrs() ));
    }
    if ( fd >= 0 )
    {
        size_t numberLength = strlen(mostSignificantDigit);
        if ( write(fd, mostSignificantDigit, numberLength) != numberLength )
        {
            MSGD ((ERR_WRITE_FILE, path, errno ));
        }
        close(fd);
    }

    return ( fd >= 0 ? 0 : -1 );
}

/*
 * ===========================================================================
 */
/*
  Function: en41_ReadTagFile
  Description: Read a pid stored in tag file
  Arguments: dbname [in] database name
             path [inout] path to tag file directory
             pid [out] pid read from file
  Return value: -1 on error, 0 on success
 */
static int en41_ReadTagFile(const char *dbname, tsp00_Pathc path, tsp00_TaskId *pid)
{
#undef MF__
#define MF__ MOD__"en41_ReadTagFile"
    int fd;

    /* check and create directory if not existing */
	if ( sql41_check_dir ( path ) != 0 )
    {
        return -1;
    }

    /* Build file name */
    if ( (strlen(path) + strlen(dbname) + 1 ) > sizeof(tsp00_Pathc) )
    {
		MSGD (( ERR_IPC_IDFILE_FOPEN, path, "dbname too long" ));
        return -1;
    }
    strcat(path, dbname);

    /* read it from file */
    fd = open( path, O_RDONLY );
    if ( fd < 0 )
    {
        if ( ENOENT == errno )
        {
            *pid = 0;
            return 0;
        }
    }

    if ( fd >= 0 )
    {
        int readLength;
        char numberBuffer[MAX_NUMBER_BUFFER_LEN_EN41];

        SAPDB_memset(numberBuffer, 0, sizeof(numberBuffer));

        while ( (readLength = (int)read(fd, numberBuffer, sizeof(numberBuffer)-1)) == -1 )
        {
            if ( errno != EINTR )
                break;
        }

        if ( readLength <= 0 )
        {
            MSGD ((ERR_READ, errno ));
            close(fd);
            fd = -1;
        }
        else
        {
            close(fd);
            *pid = (tsp00_TaskId)strtol(numberBuffer, (char **)0, 10);
            if ( *pid == 0 )
            {
                fd = -1;
            }
        }
    }

    return ( fd >= 0 ? 0 : -1 );
}

/*
 * ===========================================================================
 */
externC int en41RemovePipeFDFile(char *dbname)
{
    tsp00_Pathc path;

    en41_GetPipeFDDirectory ( path );

    return en41_Unlink(path,dbname);
}

/*
 * ===========================================================================
 */
externC int en41SetPipeFDInFile(const char *dbname, SAPDB_Int pipeFD)
{
    tsp00_Pathc path;

    en41_GetPipeFDDirectory ( path );

    return en41_CreateTagFile(dbname, path, pipeFD);
}

/*
 * ===========================================================================
 */
externC int en41GetPipeFDFromFile(const char *dbname, int *pipeFD)
{
    tsp00_Pathc path;
    tsp00_TaskId pid;
    int returnValue;

	en41_GetPipeFDDirectory ( path );

    returnValue = en41_ReadTagFile(dbname, path, &pid);
    if(0 == returnValue)
        *pipeFD = pid;
    return returnValue;
}

/*
 * ===========================================================================
 */
externC int en41SetPIDInFile(const char *dbname, tsp00_TaskId pid)
{
    tsp00_Pathc path;

	en41GetPIDDirectory ( path );

    return en41_CreateTagFile(dbname, path, pid);
}

/*
 * ===========================================================================
 */
externC int en41GetPIDFromFile(const char *dbname, tsp00_TaskId *pid)
{
    tsp00_Pathc path;

	en41GetPIDDirectory ( path );

    return en41_ReadTagFile(dbname, path, pid);
}

/*
 * ===========================================================================
 */
externC int en41SetPPIDInFile(const char *dbname, tsp00_TaskId pid)
{
    tsp00_Pathc path;

	en41GetPPIDDirectory ( path );

    return en41_CreateTagFile(dbname, path, pid);
}

/*
 * ===========================================================================
 */
externC int en41GetPPIDFromFile(const char *dbname, tsp00_TaskId *pid)
{
    tsp00_Pathc path;

	en41GetPPIDDirectory ( path );

    return en41_ReadTagFile(dbname, path, pid);
}

/*
 * ===========================================================================
 */
externC int en41SetDbSpeedInFile(const char *dbname, const char *speed)
{
    tsp00_Pathc path;
    char speedBuffer[MAX_SPEED_BUFFER_LEN_EN41];
    int  speedLength;
    int fd;

	en41_GetDbSpeedDirectory ( path );

    /* Build file name */
    if ( (strlen(path) + strlen(dbname) + 1 ) > sizeof(tsp00_Pathc) )
    {
		MSGD (( ERR_IPC_IDFILE_FOPEN, path, "dbname too long" ));
        return -1;
    }
    strcat(path, dbname);

    speedLength = (int)strlen(speed);
    if ( speedLength > (sizeof(speedBuffer)-2) )
    {
		MSGD (( ERR_IPC_IDFILE_FOPEN, path, "speed name too long" ));
        return -1;
    }
    strcpy(speedBuffer, speed);
    strcat(speedBuffer, "\n");

    /* write it into file */
    fd = open( path, O_RDWR|O_CREAT, 0640 );
    if ( fd < 0 )
    {
		MSGD (( ERR_IPC_IDFILE_OPEN, path, sqlerrs() ));
    }
    if ( fd >= 0 )
    {
        if ( write(fd, speedBuffer, speedLength+1) != (speedLength+1) )
        {
            MSGD ((ERR_WRITE_FILE, path, errno ));
        }
        close(fd);
    }

    return ( fd >= 0 ? 0 : -1 );
}

/*
 * ===========================================================================
 */
externC int en41GetDbSpeedFromFile(const char *dbname, char *speed, int size)
{
    int fd;
    tsp00_Pathc path;

	en41_GetDbSpeedDirectory ( path );

    /* Build file name */
    if ( (strlen(path) + strlen(dbname) + 1 ) > sizeof(tsp00_Pathc) )
    {
		MSGD (( ERR_IPC_IDFILE_FOPEN, path, "dbname too long" ));
        return -1;
    }
    strcat(path, dbname);

    fd = open( path, O_RDONLY );
    if ( fd < 0 )
    {
		MSGD (( ERR_IPC_IDFILE_OPEN, path, sqlerrs() ));
    }
    if ( fd >= 0 )
    {
        int readLength;
        char speedBuffer[MAX_SPEED_BUFFER_LEN_EN41];

        SAPDB_memset(speedBuffer, 0, sizeof(speedBuffer));
        while ( (readLength = (int)read(fd, speedBuffer, sizeof(speedBuffer)-1)) == -1 )
        {
            if ( errno != EINTR )
                break;
        }
        close(fd);
        if ( readLength <= 0 )
        {
            fd = -1;
        }

        if ( speedBuffer[readLength-1] != '\n' )
        {
            fd = -1;
        }

        if ( readLength > size )
        {
            fd = -1;
        }

        if ( fd > 0 )
        {
            SAPDB_memcpy(speed, speedBuffer, readLength-1);
            speed[readLength-1] = 0;
        }
    }

    return ( fd >= 0 ? 0 : -1 );
}

/*
 * ===========================================================================
 */

externC	int	sql41_check_spool_dirs ( )
{
int			rc ;
tsp00_Pathc file ;

	sql41_get_diag_dir ( file );
	rc = sql41_check_dir ( file );
	if ( rc ) return ( rc );

	sql41_get_fifo_dir ( file );
	rc = sql41_check_dir ( file );
	if ( rc ) return ( rc );

	sql41_get_ipc_dir ( file );
	rc = sql41_check_dir ( file );
	if ( rc ) return ( rc );

	en41GetPPIDDirectory ( file );
	rc = sql41_check_dir ( file );
	if ( rc ) return ( rc );

	en41GetPIDDirectory ( file );
	rc = sql41_check_dir ( file );
	if ( rc ) return ( rc );

	en41_GetDbSpeedDirectory ( file );
	rc = sql41_check_dir ( file );
	if ( rc ) return ( rc );

	return ( 0 );
}

/*
 * ===========================================================================
 */

externC	int	sql41_check_dir (
tsp00_Pathc Vfile )
{
char *file = (char *)&Vfile[0];
int			rc ;
struct stat		statbuf ;

	rc = stat ( file , &statbuf );
	if ( rc == -1 )
	{
        mode_t umask_old ;
		if ( errno != ENOENT )
		{
			MSGCD (( ERR_IPC_CHECK_DIR_STAT, file , sqlerrs() ));
			return ( -1 );
		}

		rc = mkdir ( file , 0750 );
		if ( rc == -1 )
		{
			MSGCD (( ERR_IPC_CHECK_DIR_MKNOD, file , sqlerrs() ));
			return ( -1 );
		}
	}
	else
	{
		if ( (statbuf.st_mode & S_IFMT) != S_IFDIR )
		{
			MSGCD (( ERR_IPC_CHECK_DIR_NOT_DIR, file ));
			return ( -1 );
		}
	}

	return ( 0 );
}

/*
 * ===========================================================================
 */

externC	int	sql41_check_unprotected_dir (
tsp00_Pathc Vfile )
{
char *file = (char *)&Vfile[0];
int			rc ;
struct stat		statbuf ;

	rc = stat ( file , &statbuf );
	if ( rc == -1 )
	{
        mode_t umask_old ;
		if ( errno != ENOENT )
		{
			MSGCD (( ERR_IPC_CHECK_DIR_STAT, file , sqlerrs() ));
			return ( -1 );
		}

       /* For unprotected IPC directorys '0777' is needed to allow all users to use it */
        umask_old = umask(0);
		rc = mkdir ( file , 0777 );
		if ( rc == -1 )
		{
			MSGCD (( ERR_IPC_CHECK_DIR_MKNOD, file , sqlerrs() ));
            umask(umask_old);
			return ( -1 );
		}
        umask(umask_old);
	}
	else
	{
		if ( (statbuf.st_mode & S_IFMT) != S_IFDIR )
		{
			MSGCD (( ERR_IPC_CHECK_DIR_NOT_DIR, file ));
			return ( -1 );
		}
	}

	return ( 0 );
}

/*
 * ===========================================================================
 */

externC	void    sql41_get_ipc_dir (
tsp00_Pathc                  file )
{
#undef MF__
#define MF__ MOD__"en41_get_ipc_dir"
	sql41_get_spool_dir ( file );
	(void) strcat ( (char *)&file[0] , SAPDB_SPOOL_IPC_DIRECTORY "/" );
	DBG1 (( MF__,"file '%s' \n", &file[0] ));
}

/*
 * ===========================================================================
 */

externC	void    sql41_get_diag_dir (
tsp00_Pathc                  file )
{
#undef MF__
#define MF__ MOD__"en41_get_diag_dir"
	sql41_get_spool_dir ( file );
	(void) strcat ( (char *)&file[0] , SAPDB_SPOOL_DIAG_DIRECTORY "/" );
	DBG1 (( MF__,"file '%s' \n", &file[0] ));
}

/*
 * ===========================================================================
 */

externC	void    sql41_get_fifo_dir (
tsp00_Pathc                  file )
{
#undef MF__
#define MF__ MOD__"en41_get_fifo_dir"
	sql41_get_spool_dir ( file );
	(void) strcat ( (char *)&file[0] , SAPDB_SPOOL_FIFO_DIRECTORY "/" );
	DBG1 (( MF__,"file '%s' \n", file ));
}

/*
 * ===========================================================================
 */

externC void en41GetPIDDirectory (
tsp00_Pathc                  file )
{
#undef MF__
#define MF__ MOD__"en41GetPIDDirectory"
	sql41_get_spool_dir ( file );
	(void) strcat ( (char *)&file[0] , SAPDB_SPOOL_PID_DIRECTORY "/" );
	DBG1 (( MF__,"file '%s' \n", file ));
}

/*
 * ===========================================================================
 */

externC void en41GetPPIDDirectory (
tsp00_Pathc                  file )
{
#undef MF__
#define MF__ MOD__"en41GetPPIDDirectory"
	sql41_get_spool_dir ( file );
	(void) strcat ( (char *)&file[0] , SAPDB_SPOOL_PPID_DIRECTORY "/" );
	DBG1 (( MF__,"file '%s' \n", file ));
}

/*
 * ===========================================================================
 */

static void en41_GetPipeFDDirectory (
tsp00_Pathc                  file )
{
#undef MF__
#define MF__ MOD__"en41_GetPipeFDDirectory"
	sql41_get_spool_dir ( file );
	(void) strcat ( (char *)&file[0] , SAPDB_SPOOL_PIPE_DIRECTORY "/" );
	DBG1 (( MF__,"file '%s' \n", file ));
}

/*
 * ===========================================================================
 */

static void en41_GetDbSpeedDirectory (
tsp00_Pathc                  file )
{
#undef MF__
#define MF__ MOD__"en41_GetDbSpeedDirectory"
	sql41_get_spool_dir ( file );
	(void) strcat ( (char *)&file[0] , SAPDB_SPOOL_DBSPEED_DIRECTORY "/" );
	DBG1 (( MF__,"file '%s' \n", file ));
}

/*
 * ===========================================================================
 */

externC	void    sql41_get_spool_dir (
tsp00_Pathc      file )
{
#undef MF__
#define MF__ MOD__"en41_get_spool_dir"
    RTE_Path dataPath;
    tsp00_ErrTextc ErrText;
    if ( RTE_GetCommonDataPath ( dataPath,
                                 true,
                                 ErrText ) )
    {
        strcpy( (char *)&file[0], &dataPath[0] );
    }
    else
    {
    	strcpy ( (char *)&file[0] , SAPDB_OLD_GLOBAL_DIRECTORY );
    }
	DBG1 (( MF__,"file '%s' \n", file ));
}

/*==========================================================================*/


externC int sql41_remove_ipc_resources ( char *dbname )
{
#undef MF__
#define MF__ MOD__"sql41_remove_ipc_resources"
    int                     dirlen , rc ;
    tsp00_Pathc             path;

    DBGIN;
    DBG1 (( MF__,"dbname '%s'\n", dbname ));
    rc = 0 ;

    /*
     *  Remove the request fifo.
     */
    sql41_get_request_fifo_name ( path , dbname );
    (void) sql41_remove_fifo ( path );

    /*
     *  Remove the console fifo.
     */
    sql41_get_diag_dir ( path );
    (void) strcat ( path , dbname );
    (void) sql41_remove_fifo ( path );

    /*
     *  Remove the pid tag files and speed entry
     */
    en41GetPPIDDirectory ( path );
    en41_Unlink(path, dbname);

    en41GetPIDDirectory ( path );
    en41_Unlink(path, dbname);

    en41_GetPipeFDDirectory ( path );
    en41_Unlink(path, dbname);

    en41_GetDbSpeedDirectory ( path );
    en41_Unlink(path, dbname);

    sql41_get_ipc_dir ( path );
    dirlen = strlen ( path );

    sp77sprintf ( path + dirlen , sizeof(tsp00_Pathc) - dirlen, SAPDB_DATABASE_DIRECTORY_PREFIX "%s" , dbname );
    rc |= sql41_remove_ipc_and_dir ( path ) ;

    sp77sprintf ( path + dirlen , sizeof(tsp00_Pathc) - dirlen, SAPDB_USER_DIRECTORY_PREFIX     "%s" , dbname );
    rc |= sql41_remove_ipc_and_dir ( path ) ;

    DBG1 (( MF__,"returning %d \n", rc ));
    DBGOUT;
    return ( rc ) ;
}


/*==========================================================================*/


static  int     sql41_remove_ipc_and_dir ( char *dir )
{
#undef MF__
#define MF__ MOD__"sql41_remove_ipc_and_dir"
    int                     id , ec , rc ;
    DIR                     * dp ;
    struct dirent           * de ;	
    char *                  entryName;

    DBGIN;
    DBG1 (( MF__,"directory '%s' \n", dir ));

    rc = access ( dir , 0 );

	/* if directory does not exist, nothing to do... */
    if ( (0 != rc)
	  && (ENOENT == errno) )
    {
		return ( 0 );
    }

	/* PTS 1113160 */
    dp = opendir ( dir );
    if ( NULL == dp )
    {
		MSGD (( ERR_IPC_REMOVE_DIROPEN, dir, sqlerrs() ));
		return ( 2 );
    }

    ec = 0 ;
	/* PTS 1113160 */
    while ( (de = readdir (dp)) != NULL )
    {
		entryName = (char *)(de->d_name);
		if ( strcmp ( entryName, "." ) == 0
		 || strcmp ( entryName, ".." ) == 0 )
		{
			continue;
		}

		if ( entryName[0] != '-' )
		{
  	        DBG1 (( MF__,"non-IPC object '%s' \n" , entryName ));
		    /* PTS 1109708 */
	    	rc = en41_Unlink(dir, entryName);
		    if ( rc && (errno != ENOENT) )
			{
                MSGCD (( ERR_IPC_REMOVE_FILE, entryName, sqlerrs() ));
	    	}
			continue ;
		}

		if ( 0 != entryName[1] )
		{
			id = atoi ( entryName + 2 );
		}

		switch ( entryName[1] )
		{
		    case 'q' :
            do {
				rc = msgctl ( id , IPC_RMID , (struct msqid_ds*) 0 );
            } while ( rc == -1 && errno == EINTR );
				    break ;

			case 'm' :
            /* PTS 1108941 */
            do {
                rc = shmctl ( id , IPC_RMID , (struct shmid_ds*) 0 );
            } while ( rc == -1 && errno == EINTR );
				    break ;

			case 's' :
                    rc = en41_RemoveSemaphore(id);
				    break ;

			default :
				    DBG1 (( MF__,"should be named pipe '%s/%s' \n" , dir, entryName ));
				    rc = 0 ;
		}

		if ( rc )
		{
			if ( errno != EIDRM && errno != EINVAL )
			{
				  MSGCD (( ERR_IPC_REMOVE_ENTRY, entryName, sqlerrs() ));
				  ec ++ ;
				  continue ;
			}
		}

		rc = en41_Unlink ( dir, entryName ) ;
		if ( rc && (errno != ENOENT) )
		{
			  MSGCD (( ERR_IPC_REMOVE_FILE, entryName, sqlerrs() ));
			  ec ++ ;
			  continue ;
		}
    }

	/* PTS 1113160 */
    if ( closedir ( dp ) == -1 )
    {
		MSGD(( WRN_IPC_CLOSEDIR, dir, sqlerrs() ));
	}

    if ( ! ec )
    {
        /* PTS 1109706 */
		rc = access ( dir , 0 );
		if ( rc && (errno == ENOENT) ) return ( 0 );

		rc = remove(dir);
		if ( rc )
		{
			MSGD (( ERR_IPC_REMOVE_DIRECTORY, dir, sqlerrs() ));
			return ( 2 );
		}
    }
    else
    {
        rc = ec ;
    }
    if ( rc < 0 ) rc = 2 ;

    DBG1 (( MF__,"returning %d \n", rc ));
    DBGOUT;
    return ( rc ) ;
}

/*---------------------------------------------------------------------------*/

static int en41_RemoveSemaphore(int semid)
{
    union semun arg;
    int rc;
    arg.val = 0;

        /* PTS 1108941 */
    do
    {
        rc = semctl ( semid, 0, IPC_RMID, SEMCTL_SETVAL(arg) );
    } while ( rc == -1 && errno == EINTR );


#if defined(SAPDB_DEBUG_SEMAPHORES) /* normally not defined !!! See RTE_saveUNIXcalls.h */
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
            sprintf(outputLine + strlen(outputLine), "en41: PID %8ld Remove Semaphore %12d rc %d\n", getpid(), semid, rc );
            write(fd, outputLine, strlen(outputLine));
            close(fd);
        }
    }
#endif
    return rc;
}

/*---------------------------------------------------------------------------*/

static key_t en41_ValidateKey(key_t key)
{
  if (  ((int)key) < IPC_KEY_START_VALUE_EN41
     || ((int)key) >= (IPC_KEY_START_VALUE_EN41+IPC_KEY_MAX_OFFSET_EN41) )
    return ((key_t)IPC_KEY_START_VALUE_EN41);
  else
    return key;
}

/*---------------------------------------------------------------------------*/

void * sqlAllocSharedMem(const char *Path, const unsigned int SizeInBytes)
{
#undef MF__
#define MF__ MOD__"sqlAllocSharedMem"

    void * ShmAddress = NULL;
    int    Fd;

    /* PTS 1105151 */
    /* For SharedMemory '0666' is default to allow all users to use it. use UMASK if you want to restrict it */
    Fd = open(Path, O_RDWR|O_CREAT|O_SYNC, 0666);
    if ( Fd >= 0 )
    {
        char Buffer = 0;
        struct stat statbuf ;

        /* PTS 1108861 */
        /* Check that R+W for all (0666) is still set for the file */
        if ( (fstat(Fd, &statbuf) < 0) )
        {
			MSGD (( ERR_IPC_CHECK_DIR_STAT, Path , sqlerrs() ));
			close(Fd);
			Fd = -1;
        }
        else
        {
        /* Make sure file has at least this size! (Solaris would dump core otherwise...) */
        if ( (off_t)-1 != lseek(Fd, SizeInBytes-1, SEEK_SET) )
        {
            if ( read(Fd, &Buffer, 1) == 0 )
            {
                /* We are at or beyond end of file... Write a single 0 byte at end position helps */
                if ( (off_t)-1 != lseek(Fd, SizeInBytes-1, SEEK_SET) )
                {
                    if ( write(Fd, &Buffer, 1) != 1 )
                    {
                       MSGD ((ERR_WRITE_FILE, Path, errno ));
                       close(Fd);
                       Fd = -1;
                    }
                }
                else
                {
                    MSGD ((ERR_LSEEK_SYSCALL, Path, SizeInBytes, "SET", sqlerrs()));
                    close(Fd);
                    Fd = -1;
                }
            }
            if ( Fd >= 0 )
            {
                ShmAddress = mmap( (caddr_t)0, (size_t)SizeInBytes, (PROT_READ|PROT_WRITE),
                                MAP_SHARED, Fd, (off_t)0 );
                if ( -1 == ( ((char *)ShmAddress) - ((char *)0) ) )
                {
                    ShmAddress = 0;
                    MSGD ((ERR_MMAP_SYSCALL, (caddr_t)0, SizeInBytes, sqlerrs()));
                }
            }
        }
        else
        {
            MSGD ((ERR_LSEEK_SYSCALL, Path, SizeInBytes, "SET", sqlerrs()));
        }
        }

        if ( Fd >= 0 )
        {
            close(Fd);
        }
    }
    else
    {
      /* PTS 1106834 */
      MSGD (( ERR_OPEN_SYSCALL, Path, O_RDWR|O_CREAT|O_SYNC, sqlerrs() ));
    }

    return (ShmAddress);
}

/*---------------------------------------------------------------------------*/

void   sqlFreeSharedMem(void *ShmAddress, const unsigned int SizeInBytes)
{
#undef MF__
#define MF__ MOD__"sqlFreeSharedMem"

    if ( munmap( (char *)ShmAddress, SizeInBytes ) != 0 )
    {
        MSGD ((ERR_MUNMAP_SYSCALL, ShmAddress, SizeInBytes, sqlerrs()));
    }

}

/*---------------------------------------------------------------------------*/

/* Needed to support database releases < 7.4 */

/*
 * This should collect all 'ps' grepping code...
 *
 * Use 'Source' (command that produces or reproduces a 'ps' output
 * to find database named 'db:DbName' running with kernel program 'kernelpgm'
 *
 * If 'Source' is empty string "" it is replaced by 'ps_all'
 * If 'kernelpgm' is empty string "" any speed matches...
 */
int en41FindDatabaseProcess(const char *Source, const char *DbName, const char *kernelpgm)
{
    int  rc;
    char cmdBuf[255];

    sp77sprintf ( cmdBuf, sizeof(cmdBuf), "%s | egrep -v ' egrep | awk | grep ' | egrep '"
                                            SAPDB_DATABASE_DIRECTORY_PREFIX "%s %s' > /dev/null",
                Source == NULL ? "ps_all" : Source,
                DbName,
                kernelpgm == NULL ? "" : kernelpgm );
    rc = system (cmdBuf);
    if ( rc == -1 )
    {
        MSGD(( ERR_SYSTEM_SYSCALL, cmdBuf, sqlerrs() ));
    }
    return (rc == 0);
}

/*---------------------------------------------------------------------------*/

int en41BuildDatabaseProcessScan(const char *DbName, char *Buffer, int BufferSize)
{
    /* ps -e   produces one line for each running programm where last in line is executable name */
    /*         second value of each line is always the process id                                */
    /* Prevent to retrieve Process Ids of grep, egrep or awk                                     */
    sp77sprintf( Buffer, BufferSize,
            "ps_all | awk '/"
            SAPDB_DATABASE_DIRECTORY_PREFIX "%s$/&&!/ awk /&&!/ grep /&&!/ egrep / { printf $2 \" \\n\"}' " ,
             DbName );
    return 0;
}

/*---------------------------------------------------------------------------
  Function: en41BuildXShowCommand
  Description: Create x_show report output command

  Since x_show has to know ALL database (even very old ones...) it is a little bit tricky to get them.

  First get all processes into a tmp file.
  ps_all is an OS dependend 'ps -ef' that will produce output where database name will be visible

  Now find all probably running databases.
  Database are found due to the directory they create in IpcPath which is named db:DBNAME and for
  older ones (before 7.4) using the link they create on start db:DBNAME.

  Use each database name to grep in the ps output. If database is not found it is shown
  as (not stopped correctly) otherwise only its name is given.

  Arguments: PgmPath [in] path to program directory
             cmdline [out] the created command line
             size    [in] size of command buffer
 */
int en41BuildXShowCommand(const char *PgmPath, char *cmdline, int size)
{
    tsp00_Pathc IpcPath;

    sql41_get_ipc_dir( IpcPath );

    if ( ( strlen(IpcPath) +
           strlen(PgmPath) +
           strlen( "ps_all > /tmp/tmp.$$; "
                 "for DB in `ls   | grep " SAPDB_DATABASE_DIRECTORY_PREFIX " | cut -c4- | sort -u`; "
                 "do if egrep -v ' egrep | grep | awk ' /tmp/tmp.$$ | egrep \"" SAPDB_DATABASE_DIRECTORY_PREFIX "$DB|kernel $DB|slowknl $DB|quickknl $DB\" > /dev/null ;"
                 "    then echo \"$DB\" ;"
                 "    else echo \"$DB   (not stopped correctly)\" ;"
                 "   fi ;"
                 "done ;"
                 "rm -f /tmp/tmp.$$ " ) + 1 ) > size )
    {
        strncpy(cmdline, "echo cmdbuffer too small", size);
        return 1;
    }

    sp77sprintf ( cmdline, size,
                       "ps_all > /tmp/tmp.$$; "
                       "for DB in `ls %s %s | grep " SAPDB_DATABASE_DIRECTORY_PREFIX " | cut -c4- | sort -u`; "
                       "do if egrep -v ' egrep | grep | awk ' /tmp/tmp.$$ | egrep \"" SAPDB_DATABASE_DIRECTORY_PREFIX
                       "$DB|kernel $DB|slowknl $DB|quickknl $DB\" > /dev/null ;"
                       "    then echo \"$DB\" ;"
                       "    else echo \"$DB   (not stopped correctly)\" ;"
                       "   fi ;"
                       "done ;"
                       "rm -f /tmp/tmp.$$ ",
                       IpcPath,
                       PgmPath );
    return 0;
}
/*
 * PTS 1104922
 */
const char *en41CheckFifo(const char *file, int mode)
{
    int rc ;
    struct stat statbuf ;

	rc = stat ( file , &statbuf );
	if ( rc == -1 )
	{
        return sqlerrs();
    }

    if ( (statbuf.st_mode & S_IFMT) != S_IFIFO )
    {
        return "File is not a fifo!";
    }

    if ( (statbuf.st_mode & mode) != mode )
    {
        return "Fifo protection differs!";
    }

    return (const char *)0;
}

/* PTS 1113160 */
/*---------------------------------------------
  Function: en41_Unlink
  Description: allows to unlink without restriction to size of file name
  Return value: Result of unlink
 */
static int en41_Unlink(char *dir, char *entry)
{
	int rc;
	char *filePath;
	if ( 0 == dir || 0 == dir[0] )
	{
		filePath = entry;
	}
	else
	{
		filePath = (char *)alloca(strlen(dir) + strlen("/") + strlen(entry) + 1);

		strcpy(filePath, dir);
		if ( 0 != dir[0] && '/' != dir[strlen(dir)-1] )
		{
			strcat(filePath, "/");
		}
		strcat(filePath, entry);
	}

	rc = unlink ( filePath ) ;
	if ( rc < 0 && ENOENT != errno )
	{
		MSGD(( ERR_UNLINK_SYSCALL, filePath, sqlerrs() ));
	}
	return rc;
}
