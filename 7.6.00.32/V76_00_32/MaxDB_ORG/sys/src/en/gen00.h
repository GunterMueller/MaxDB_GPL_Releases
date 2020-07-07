/*!
  @file           gen00.h
  @author         JoergM
  @special area   Operating System Specific Defines
  @brief          During replacement of vsp00c by gsp00 missing parts had
                              been moved into gos00.h. This files trys to cover the
                              parts missed from vsp00c...
  @see

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



//
//   FreeBSD portions added by Kai Mosebach,
//   For more informations see : http://www.komadev.de/sapdb
//

\endif
*/



#ifndef GEN00_H
#define GEN00_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "gsp100.h"
#include "geo60.h"
#include "geo00_2.h"
#include "vsp002.h"
#include "geo00_2.h"
#include "SAPDBCommon/SAPDB_string.h"

#if	defined(HP9)
#define	_CLASSIC_XOPEN_TYPES
#endif

#ifdef FREEBSD
#define SIGCLD SIGCHLD
#endif

/* for void *alloca(size_t) */
#ifdef AIX
# include       <malloc.h>
#else
#if !defined NMP && !defined(FREEBSD)
# include       <alloca.h>
#endif
#endif
#include	    <stdlib.h>
#include        <unistd.h>
#include        <stdio.h>
#if !(defined(SUN) || defined(SOLARIS))
#include        <memory.h>
#endif
#include        <errno.h>
#if defined(_IBMR2) || defined(SUN) || defined(SOLARIS) || defined(HPUX)
extern int    sys_nerr;
extern char * sys_errlist[];
#endif /* _IBMR2 || SUN || SOLARIS || HPUX */
#include        <sys/types.h>
#include        <fcntl.h>
#include        <signal.h>
#include        <sys/ipc.h>
#include        <sys/msg.h>
#include        <sys/sem.h>
#if defined(LINUX) || defined(FREEBSD)
struct region { struct region * n ; };
#endif
#if defined(HP9)
#include        <sys/param.h>
#endif
#include        <sys/shm.h>
#include        <sys/stat.h>
#include        <time.h>
#include        <sys/time.h>
#include        <sys/times.h>
#include        "RunTime/System/RTESys_Spinlock.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#if defined(BIT64)
#define     FMTA    "0x%016p"   /* address format */
#define     SIZL    "16l"       /* part of long format */
#else
#define     FMTA    "0x%08x"    /* address format */
#define     SIZL    "8l"        /* part of long format */
#endif

#if ! defined(OSF1)
#undef          NULL
#define         NULL                    0L
#endif
#undef          FALSE
#define         FALSE                   0
#undef          TRUE
#define         TRUE                    1

#define         UNDEF                   undef_ceo00

#define			REGISTER

#define         DEF_KERNEL_TASKS        6   /* ti,al,tc,dw,ut,cs */

#if defined(SUN) || defined(SOLARIS) || defined(HP9)
/* MP-SOLARIS, Solbourne || RM/600 || HP9000/8xx MP || SEQUENT */
/* 'big' machines = multi CPU machines (now or in near future) */
#define         MAX_USERS               2450 /* includes server tasks */
#define         MAX_TPP                 350+DEF_KERNEL_TASKS
#else
/* 'small' machines = single CPU machines */
#define         MAX_USERS               1024 /* includes server tasks */
#define         MAX_TPP                 350+DEF_KERNEL_TASKS
#endif  /* SUN || SOLARIS || HP9 */

#define         MAX_UKP                 32
#define         MAX_QUU_PER_TASK        2
#define         MAXSLEEPTIME            10      /* KWe : 09.07.92
                         * new here; old in ven83c */

/*
 *  Old values for database state (see geo00_2.h and RTE_KGS.hpp)
 */
#define         SERVER_OFFLINE          SERVER_OFFLINE_EO00
#define         SERVER_STARTING         SERVER_STARTING_EO00
#define         SERVER_COLD             SERVER_ADMIN_EO00
#define         SERVER_WARM             SERVER_ONLINE_EO00
#define         SERVER_SHUTDOWN         SERVER_SHUTDOWN_EO00
#define         SERVER_SHUTDOWNREINIT   SERVER_SHUTDOWNREINIT_EO00
#define         SERVER_SHUTDOWNKILL     SERVER_SHUTDOWNKILL_EO00
#define         SERVER_STOP             SERVER_STOP_EO00
#define         SERVER_KILL             SERVER_KILL_EO00

/*
 *  Values for database stop modes
 */
#define         MODE_REINIT             1
#define         MODE_KILL               2
#define         MODE_DUMP               3

/*
 *  Values for kernel process state
 */
#define         KT_INITIALIZED          1
#define         KT_RUNNING              2
#define         KT_TERMINATING          3
#define         KT_TERMINATED           4
#define         KT_SLEEPING             99

/*
 *  Values for devspace process state
 */
#define         KT_DETACHED             98
#define         KT_ATTACHED             97

/* interacting processes in devio */
#define         DEVIO_DEV0_PROCESS      0
#define         DEVIO_DEVI_PROCESS      1
#define         DEVIO_DEVI_CHILD        2
#define         NR_OF_DEVIO_PROC        3

/* request types internal to devspace processes */
#define         DEVINT_INVALID          0
#define         DEVINT_VATTACH          1
#define         DEVINT_VBLOCKIO         2
#define         DEVINT_VDETACH          3
#define         NR_OF_INT_REQ           4

/* Reply codes */
#define         DEVIO_NOREPLY           0
#define         DEVIO_REPLY             1

/* File modes */
#define         MOD_UNKNOWN             -1
#define         MOD_FIFOSPEC            S_IFIFO
#define         MOD_CHARSPEC            S_IFCHR
#define         MOD_DIR                 S_IFDIR
#define         MOD_BLCKSPEC            S_IFBLK
#define         MOD_ORDIFILE            S_IFREG

/*
 *  lseek
 */
#define         ABS_OFFSET              0   /* SEEK_SET unistd.h */
#define         SIZE_OFFSET             2   /* SEEK_END unistd.h */
#define         NO_OFFSET               ((off_t)0)  /* offset value */

/*
 * ASYNCNTL request types
 */
#define         ASYNCNTL_FILETYPE       0
#define         ASYNCNTL_REWIND         1


#define         MAXBUFPOOLBLKS          16
#define         MAX_ASYN_REQUESTS       2

#define         STRAT_EQUALIZE          0
#define         STRAT_COMPRESS          1

#define         MAXTIMEVALUE            2147483647

#define         WRITE_CORE              1


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*
 *  Macros for PASCAL-functions
 */
#ifndef odd
#define odd(i)          ( (i) & 1 )
#endif
#ifndef even
#define even(i)         ( ~ (i) & 1 )
#endif
#ifndef min
#define min(a,b)        ( ( (a) <= (b) ) ? (a) : (b) )
#endif
#ifndef max
#define max(a,b)        ( ( (a) >= (b) ) ? (a) : (b) )
#endif

#define         MX_PRLINE               256
#define         VP_PAGE                 4
#define         MAXERRORTEXT            40

#define         MAXVFNLENGTH            64
#define         VREAD_EN00              0
#define         VWRITE_EN00             1
#define         VAPPEND_EN00            2
#define         VOVERWRITE_EN00         3
#define         VREAD_ORA_EN00          4
#define         VF_OK                   0
#define         VF_NOTOK                1
#define         VF_EOF                  2
#define         VF_NOSEEK               3
#define         MAX_VF_LABEL_LENGTH

#define         MX_EXECLINE             512
#define         E_OK                    0
#define         E_NOTOK                 1
#define         E_NO_EXEC               2
#define         E_NO_ASYNC              3
#define         E_NO_SYNC               4
#define         E_NO_SYNC_NEW           5
#define         E_NO_SYNC_SAME          6
#define         ASYNC                   0
#define         SYNC_NEW_SESSION        1
#define         SYNC_SAME_SESSION       2

#define         is_ascii                0
#define         is_ebcdic               1
#define         is_codeneutral          2

#define         MX_LINE                 132
#define         MX_DBNAME               18
#define         MAXNAMELENGTH           18

#define         SQLOK                   0
#define         SQLNOTOK                1

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*
 *  conditional int. used for strncpy(arg3),... fread(arg2),...
 */
#   if defined(OSF1)
#       define          CINT            size_t
#   else
#       define          CINT            int
#   endif

typedef unsigned char           UINT1 ;
typedef unsigned short          UINT2 ;
#ifdef BIT64
typedef unsigned int            UINT4 ;
#else
typedef unsigned long           UINT4 ;
#endif
#ifndef WIN32
#ifndef _CHAR
#define _CHAR
typedef unsigned char           CHAR ;
#endif
#endif
#ifndef	_UCHAR
#define	_UCHAR
typedef unsigned char           UCHAR ;
#endif
typedef unsigned char           enum1 ;
typedef float                   FLOAT4 ;
typedef double                  FLOAT8 ;
typedef long                    LONG ;
#ifndef	_ULONG
#define	_ULONG
typedef unsigned long           ULONG;
#endif

typedef tsp00_Int4                PROCESS_ID ;
typedef tsp00_Int4                REGION_ID ;
typedef tsp00_Int2                isolation_level ;

typedef tsp00_DbName     SQL_DBNAME ;
typedef tsp00_DbNamec    SQL_DBNAMEC ;
typedef tsp00_NodeId     SQL_NODEID ;
typedef tsp00_NodeIdc    SQL_NODEIDC;
typedef tsp00_VFilenamec VF_FILENAMEC;
typedef tsp00_C64c       PRINTERNAMEC;
typedef char             tsp_vf_label [ 132 ];
typedef tsp00_KnlIdentifierc tsp_knl_identifierC ;
typedef char             tsp2_devnamec [ ALIGN_4BYTE_EO00(sizeof(tsp2_devname)+1) ];

typedef void                    (* VOIDFUNC_PTR )();

#if !defined(FREEBSD)
#if !defined(__GNU_LIBRARY) || defined(_SEM_SEMUN_UNDEFINED)
union   semun
{
    int                               val ;
    struct  semid_ds                * buf ;
    unsigned short                  * array ;
};
#endif /* __GNU_LIBRARY__ && !_SEM_SEMUN_UNDEFINED */
#endif

/*
 * This macro is used to specify different interfaces to semctl()s forth argument...
 * Some OS use 'int', some OS use union semun... Currently all use union semun as argument...
 */
#if 1
#define SEMCTL_SETVAL(_a) _a
#define SEMCTL_IPC_STAT(_a) _a
#define SEMCTL_IPC_SET(_a) _a
#else
#define SEMCTL_SETVAL(_a) (_a).val
#define SEMCTL_IPC_STAT(_a) (_a).buf
#define SEMCTL_IPC_SET(_a) (_a).buf
#endif

/* ************************************************************ */

#if defined(ALPHA)
#  define     LOCK_INT    2
#elif defined(PA20W) || defined(PA11)
#  define     LOCK_INT    32
#  define     ADDR64(lock)     ((int *)(((unsigned long)lock + 63) & ~63))
#else
#  define     LOCK_INT    1
#endif

typedef     UINT4       ALONE_UKP_STRUCT [ LOCK_INT ] ;

#  if   defined(AIX) && defined(_LONG_LONG)
/* 1995-Sep-04 J.Bruehe  RS/6000 cache line is 128 byte, make
   the spin lock variable (4 byte) sole user of that line
   to prevent "false sharing". */
typedef         UCHAR       ALONE_IN_CACHELINE [ 124 ] ;
#define         ALONE_IN_CACHE( dummy ) ALONE_IN_CACHELINE dummy ;
#define         GUARDWORD( guard )      tsp00_Int4           guard ;
#else
#define         ALONE_IN_CACHE( dummy )
#define         GUARDWORD( guard )
#endif

typedef     UINT4       COMSEG_LOCK;

/*
 * Macros for Interlock Handling ( _addr is of type ALONE_UKP_STRUCT )
 */
#define INIT_LOCK(addr)    RTESys_InitLock((RTE_Lock *)(&addr[0]))
#define INTERLOCK(addr) RTESys_TestAndLock((RTE_Lock *)(&addr[0]))
#define CLEARLOCK(addr)      RTESys_Unlock((RTE_Lock *)(&addr[0]))
/*
  Watch out!!! You cannot use RTESys_Lock() here, since WAIT_UNTIL_ALONE is called inside dispatcher coding...
 */
#define WAIT_UNTIL_ALONE(addr) while ( RTESys_TestAndLock((RTE_Lock *)(&addr[0])) ) RTESys_GiveUpTimeSlice()

#if defined(SUN) || defined(SOLARIS)
externC int usleep(unsigned int useconds);
#endif /* SUN||SOLARIS */

#define SLEEP(_msec) usleep(1000*_msec)

#define         SIGNALFUNCTYPE          void

#define MF__    "undefined"

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/


/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/



#endif  /* GEN00_H */
