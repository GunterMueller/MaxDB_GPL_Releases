/*!---------------------------------------------------------------------
  @file           RTESys_SystemInfo.c
  @author         JoergM, RobinW
  @brief          System Calls: Retrieving various machine information
  @see

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
---------------------------------------------------------------------*/




/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/System/RTESys_SystemInfo.h"
#include "RunTime/System/RTESys_CommandPipe.h"
#include "SAPDBCommon/SAPDB_sprintf.h"
#include "geo60.h"      /* MSGD */
#include "geo007_1.h"   /* error messages */
#include "heo01.h"      /* one single annoying tsp01_RteError... */
#include <errno.h>

#if defined(WIN32)
#  include <windows.h>
#  include "RunTime/RTE_WindowsReleaseDependend.h" /* nocheck */
#  include "RunTime/System/RTESys_NTLoadDLLFunc.h" /* nocheck */
#else
#  include <stdlib.h>
#  include <unistd.h>
#  include <stdio.h>
#  include <string.h>
#  include <sys/utsname.h>
#  include <sys/resource.h>
#  include "RunTime/RTE_saveUNIXcalls.h" /* nocheck */
#endif /* WIN32 */

#if defined(HPUX)
#  include <sys/param.h>
#  include <sys/pstat.h>
#  include <sys/dk.h>
#endif /* HPUX */

#if defined(NMP)
#  include <sys/types.h>
#  include <sys/mpcntl.h>
#endif /* NMP */

#if defined(AIX)
#  include <sys/vminfo.h>
#  include <dirent.h>
#  include <libperfstat.h>
#endif /* AIX */

#if defined(OSF1)
#  include <sys/sysinfo.h>
#  include <machine/hal_sysinfo.h>
#  include <sys/systm.h>
#endif /* OSF1 */

#if defined(LINUX)
#  include <sys/sysinfo.h>
#endif /* LINUX */

#if defined(SUN) || defined(SOLARIS)
#  include <sys/sysinfo.h>
#  include <kstat.h>
#  include "RunTime/System/RTESys_Spinlock.h" /* nocheck */
#endif /* SUN || SOLARIS */


#if defined(FREEBSD)
#  include <sys/types.h>
#  include <sys/sysctl.h>
#  include <sys/stat.h>
#endif

#if defined(NMP)||defined(SUN)||defined(SOLARIS)||defined(OSF1)
#  include <sys/stat.h>

/* The combination of a 32 bit environment and _FILE_OFFSET_BITS=32 does not work properly on Solaris.      */
/* This can be solved by redefining swapent and swaptable (see under DEFINES). Unfortunately, in Solaris 8  */
/* swap.h cannot be included under such circumstances. As swap.h is needed for the definition of SC_GETNSWP */
/* and SC_LIST, this nasty workaround has to be done...*/

#  if (defined(SUN) || defined(SOLARIS)) && !defined(BIT64) && _FILE_OFFSET_BITS == 64
#    undef _FILE_OFFSET_BITS
#    define _FILE_OFFSET_BITS 32

#    include <sys/swap.h>

#    undef _FILE_OFFSET_BITS
#    define _FILE_OFFSET_BITS 64

#  else
#    include <sys/swap.h>
#  endif

#endif /* NMP || SUN || SOLARIS || OSF1 */



/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#ifdef HPUX
#define USE_HPUX_PSTAT
#endif /* HPUX */

#ifdef NMP
#define USE_SNI_MPCNTL
#endif /* NMP */


#if defined(SUN) || defined(SOLARIS) || defined(OSF1) || defined(NMP) || defined(AIX) || defined(FREEBSD)
#define MAXSTRSIZE 128
#endif

#if defined(SUN) || defined(SOLARIS) || defined(OSF1) || defined(NMP)

#if !defined(BIT64)

/* in <sys/swap.h> swapent and swaptable is defined like this, but ste_start and ste_length are of type off_t. Unfortunately, on 32 bit systems */
/* (Sun AND Sni!) the size of off_t is 8 bytes if _FILE_OFFSET_BITS is defined to be 64, while the library assumes the size of the fields to be */
/* 4 bytes. */

typedef struct myswapent {
        char    *ste_path;      /* get the name of the swap file */
        long    ste_start;      /* starting block for swapping */
        long    ste_length;     /* length of swap area */
        long    ste_pages;      /* numbers of pages for swapping */
        long    ste_free;       /* numbers of ste_pages free */
        int     ste_flags;      /* see below */
} myswapent_t;

typedef struct myswaptable {
        int     swt_n;                  /* number of swapents following */
        struct  myswapent swt_ent[1];     /* array of swt_n swapents */
} myswaptbl_t;

#else

#define myswapent swapent
#define myswapent_t swapent_t
#define myswaptable swaptable
#define myswaptbl_t swaptbl_t

#endif /* !BIT64 */

#endif /* SUN || SOLARIS || NMP || OSF1 */

#if defined(WIN32)
#  define USE_GLOBAL_MEMORY_STATUS_FOR_FREE_MEMORY_
#elif defined(NMP) || defined(OSF1) || defined(FREEBSD)
#  define USE_VMSTAT_FOR_FREE_MEMORY_
#  if defined(NMP)
#    define VMSTAT_MULTIPLIER_  BYTES_PER_KILOBYTE
#  elif defined(OSF1) || defined(AIX) || defined(FREEBSD)
#    define VMSTAT_MULTIPLIER_  (4*BYTES_PER_KILOBYTE)
#  endif
#elif defined(LINUX)
#  define USE_SYSINFO_FOR_FREE_MEMORY_
#elif defined(SUN) || defined(SOLARIS)
#  define USE_SYSCONF_FOR_FREE_MEMORY_
#elif defined(HPUX)
#  define USE_PSTAT_FOR_FREE_MEMORY_
#elif defined(AIX)
#  define USE_PERFSTAT_FOR_FREE_MEMORY
#endif

#define BYTES_PER_MEGABYTE      1048576
#define KILOBYTES_PER_MEGABYTE  1024
#define BYTES_PER_KILOBYTE      1024
#define BYTES_PER_GIGABYTE      (1024*1024*1024)

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  STRUCTURES, TYPES, UNIONS ...                                            *
 *===========================================================================*/
/* The field mem_unit in the sysinfo structure had been introduced with linux 2.4 */
/* It gives a multiplier for the elements in the structure that describe amounts  */
/* of memory. To make the code compilable under all releases of the linux kernel, */
/* the structure is definde here and not taken from /usr/include/linux/kernel.h   */

#ifdef LINUX
    struct new_sysinfo {
        long uptime;             /* Seconds since boot */
        unsigned long loads[3];  /* 1, 5, and 15 minute load averages */
        unsigned long totalram;  /* Total usable main memory size */
        unsigned long freeram;   /* Available memory size */
        unsigned long sharedram; /* Amount of shared memory */
        unsigned long bufferram; /* Memory used by buffers */
        unsigned long totalswap; /* Total swap space size */
        unsigned long freeswap;  /* swap space still available */
        unsigned short procs;    /* Number of current processes */
        unsigned long totalhigh; /* Total high memory size */
        unsigned long freehigh;  /* Available high memory size */
        unsigned int mem_unit;   /* Memory unit size in bytes */
        char _f[20-2*sizeof(long)-sizeof(int)]; /* Padding for libc5 */
    };
#endif

/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/

#if defined(WIN32)
static SYSTEM_INFO *getSystemInfo();
static SAPDB_UInt4 getNumOfLogicalProcessors();
#endif /* WIN32 */

#if defined(WIN32) || defined(LINUX)
static SAPDB_UInt4 getNumOfPhysicalProcessors();
#endif

#if defined(AIX)
/* unfortunately, there is no headerfile for swapqry() */
int swapqry(char *, struct pginfo *);

#endif /* AIX */
/*===========================================================================*
 *  GLOBAL FUNCTIONS                                                         *
 *===========================================================================*/

externC SAPDB_UInt4 RTESys_NumberOfCPU()
{
    SAPDB_UInt4 nProc = 0;
#if defined(WIN32)
    nProc = getNumOfLogicalProcessors();
#elif defined(_SC_NPROCESSORS_ONLN)
    /* AIX + SUN + SOLARIS + DEC + LINUX + FREEBSD */
    nProc = (SAPDB_UInt4)sysconf(_SC_NPROCESSORS_ONLN);
#elif defined(USE_HPUX_PSTAT)
    /* HPUX */
    struct pst_processor buf;

    while ( 0 != pstat_getprocessor(&buf, sizeof(struct pst_processor), 1, nProc) )
        nProc++;
#elif defined(USE_SNI_MPCNTL)
    /* NMP */
    nProc = (SAPDB_UInt4)mpcntl(MPCNTL_CPUCNT, (void *)0);
#endif

    return ( nProc > 0 ? nProc : (SAPDB_UInt4)1 );
}

/*-----------------*/

externC SAPDB_UInt4 RTESys_NumberOfPhysicalCPUs()
{
#if defined(WIN32) || defined(LINUX)
    return getNumOfPhysicalProcessors();
#else
    return RTESys_NumberOfCPU();
#endif
}

/*-----------------*/

externC SAPDB_UInt4 RTESys_SystemPageSize()
{
  SAPDB_UInt4 pageSize;

#if defined(WIN32)
  pageSize = (SAPDB_UInt4)(getSystemInfo()->dwPageSize);
#elif defined(_SC_PAGESIZE)
  pageSize = (SAPDB_UInt4)sysconf(_SC_PAGESIZE);
#elif defined(_SC_PAGE_SIZE)
  pageSize = (SAPDB_UInt4)sysconf(_SC_PAGE_SIZE);
#endif
  return pageSize;
}

/*-----------------*/

externC SAPDB_UInt4 RTESys_SystemAllocationGranularity()
{
    SAPDB_UInt4 granularity;

#if defined(WIN32)
    granularity = (SAPDB_UInt4)(getSystemInfo()->dwAllocationGranularity );
#elif defined(_SC_PAGESIZE)
    granularity = (SAPDB_UInt4)sysconf(_SC_PAGESIZE);
#elif defined(_SC_PAGE_SIZE)
    granularity = (SAPDB_UInt4)sysconf(_SC_PAGE_SIZE);
#endif
    return granularity;
}

/*-----------------*/
#ifndef _WIN32
static SAPDB_Bool GetPath(SAPDB_Char *fileName,SAPDB_Char *path)
{
    SAPDB_Bool found;
    SAPDB_Char *ptr ;
    SAPDB_Int4 fileNameLen = strlen(fileName) ;
    SAPDB_Int4 pathNameLen  ;
    SAPDB_Char *pathFromEnviron = getenv("PATH") ;
    SAPDB_Char *copyOfPath;

    /* checking PATH */
    if ( NULL == pathFromEnviron )
        return false;

    copyOfPath = alloca(strlen(pathFromEnviron)+1);
    strcpy(copyOfPath,pathFromEnviron);
    do
    {
        if ( (ptr = strchr(copyOfPath, ':')) )
            *ptr = '\0' ; /* seperate one dir, if Ptr == NULL handle last dir in path */

        pathNameLen = strlen ( copyOfPath ) ;
        if ( pathNameLen + fileNameLen < sizeof(RTE_Path) )
        {
            if ( *copyOfPath == PATH_DELIMITER_EO01 ) /* absolute Path */
            {
                strcpy(path, copyOfPath);
                if ( copyOfPath [ pathNameLen - 1 ] != PATH_DELIMITER_EO01 )
                {
                    path[pathNameLen] = PATH_DELIMITER_EO01;
                    path[pathNameLen+1] = 0;
                }
                strcat(path, fileName);
                /* checking File FullPath */
                found = RTE_save_access(path, X_OK) == 0 ;
                if(found)
                {
                    return true;
                }
            }
            if ( ptr )
                copyOfPath = ++ptr ;
        }
    }
    while ( ptr ) ;
}
#endif
/*-----------------*/

static SAPDB_ULong convertedFromSystemPagesToMegabytes(SAPDB_UInt8 valueInSystemPages)
{
    SAPDB_UInt4 pageSize = RTESys_SystemPageSize();
    if( 0 != pageSize ) 
    {
        if( pageSize > BYTES_PER_MEGABYTE )
        {
            return ((SAPDB_ULong)valueInSystemPages) * ( pageSize / BYTES_PER_MEGABYTE );
        }
        else
        {
            return ((SAPDB_ULong)valueInSystemPages)  / ( BYTES_PER_MEGABYTE / pageSize );
        }
    }
    else
    {
        return 0;
    }
}

/*-----------------*/

#if !(defined(AIX) && defined(LIBOMS))  /* keep the following code out of liboms to keep it small... */
externC SAPDB_ULong RTESys_PhysicalMemorySize()
{
#if defined(AIX)
    perfstat_memory_total_t totalMemory;

    if ( 1 != perfstat_memory_total (0, &totalMemory, 144, 1) ) 
    // the size of the perfstat_memory_total_t structure grows from AIX release to AIX release, 
    // even within AIX 5.2-releases... As we cannot guarantee that the AIX we are running on is 
    // as new as the one we were built on, the smallest size of a perfstat_memory_total_t structure 
    // known is being used. It is the one from AIX 5.1 and is 144 bytes.
    {
/* If perfstat_memory_total() fails (due to possible binary icompatibilities...) use the output of a call to lsattr */       
        char receiveBuffer[MAXSTRSIZE];
        char *beginOfNumber,*endOfNumber;
        SAPDB_Int4 exitStatus;
        RTESys_CommandOutputPipeContext context;
        RTE_SystemRc rc;
        tsp01_RteError  rteError;

        SAPDB_Char path[sizeof(RTE_Path)+sizeof(" -E -l sys0 -a realmem")+1];

        if(!GetPath ( "lsattr", path ))
        {
            MSGD (( ERR_POPEN_SYSCALL, "lsattr -E -l sys0 -a realmem", ENOENT ));
            return 0;
        }

        strcat(path," -E -l sys0 -a realmem");

        rc = RTESys_OpenCommandOutputPipe( &context, path );

        if(RTE_SYSTEMRC_NO_ERROR != rc)
        {
            MSGD (( ERR_POPEN_SYSCALL, "lsattr -E -l sys0 -a realmem", errno));
            return 0;
        }
        RTE_save_read(context.cmdOutput, receiveBuffer, MAXSTRSIZE);
        if(RTE_SYSTEMRC_NO_ERROR != RTESys_CloseCommandOutputPipe( &context, &exitStatus))
        {
            MSGD (( ERR_SYSCALL, "pclose", errno));
            return 0;
        }

        /* the lsattr command gives the size of the memory in a string like */
        /* "realmem 8388608 Amount of usable physical memory in Kbytes False" */
        /* so the output has to be parsed */
        beginOfNumber = strchr(receiveBuffer,' ');
        if(0 == beginOfNumber)
        {
            MSGD (( ERR_COULD_NOT_FIND_NUMBER_IN_STRING ));
            return 0;
        }
        beginOfNumber++;
        endOfNumber = strchr(beginOfNumber,' ');
        if(0 == endOfNumber)
        {
            MSGD (( ERR_COULD_NOT_FIND_NUMBER_IN_STRING ));
            return 0;
        }
        *endOfNumber = 0;
        return strtoul(beginOfNumber,NULL,10)/KILOBYTES_PER_MEGABYTE;
    }
    else
    {
        return convertedFromSystemPagesToMegabytes(totalMemory.real_total);
    }
#endif /*AIX*/

#if defined(HPUX)
    struct pst_static pst;
    union pstun pu;

    pu.pst_static = &pst;
    if ( pstat( PSTAT_STATIC, pu,(size_t)sizeof(pst), (size_t)0, 0 ) != -1 )
    {
        return (unsigned long)( (double)pst.physical_memory * pst.page_size / BYTES_PER_MEGABYTE ) ;
    }
    else
    {
        MSGD (( ERR_SYSCALL, "pstat", errno));
        return 0;
    }
#endif /*HPUX*/

#if defined(LINUX)
    struct new_sysinfo info;

    unsigned long totalBytes;
    memset(&info,0,sizeof(info));
    if(-1 == sysinfo((struct sysinfo *)&info))
    {
        MSGD (( ERR_SYSCALL, "sysinfo",errno));
        return 0;
    }

    totalBytes = info.totalram;

    if(0 != info.mem_unit)
    {
        totalBytes *= info.mem_unit;
    }
    return (totalBytes / BYTES_PER_MEGABYTE);
#endif /*LINUX*/

#if defined(NMP)
    char receiveBuffer[MAXSTRSIZE];
    FILE *fileHandle;
    char *beginOfNumber;

    RTESys_CommandOutputPipeContext context;
    tsp01_RteError  rteError;
    SAPDB_Char path[sizeof(RTE_Path)+sizeof(" -M")+1];
    if(!GetPath ( "uname", path ))
    {
        MSGD (( ERR_POPEN_SYSCALL, "uname -M", ENOENT ));
        return 0;
    }
    strcat(path," -M");

    RTE_SystemRc rc = RTESys_OpenCommandOutputPipe( &context, path);
    SAPDB_Int4 exitStatus;

    if(RTE_SYSTEMRC_NO_ERROR != rc)
    {
        MSGD (( ERR_POPEN_SYSCALL, "/bin/uname -M", errno));
        return 0;
    }
    RTE_save_read(context.cmdOutput, receiveBuffer, MAXSTRSIZE);
    if(RTE_SYSTEMRC_NO_ERROR != RTESys_CloseCommandOutputPipe( &context, &exitStatus))
    {
        MSGD (( ERR_SYSCALL, "pclose", errno));
        return;
    }

    beginOfNumber = strchr(receiveBuffer,'/');
    if(0 == beginOfNumber)
    {
        MSGD (( ERR_COULD_NOT_FIND_NUMBER_IN_STRING ));
        return 0;
    }
    else
    {
        beginOfNumber++;
        return strtoul(beginOfNumber,NULL,10);
    }
#endif /*NMP*/

#if defined(OSF1)
    int memsize;
    getsysinfo(GSI_PHYSMEM,(char *)&memsize,sizeof(int),0,0,0);
    return memsize/KILOBYTES_PER_MEGABYTE;
#endif /*OSF1*/

#if defined(SUN) || defined(SOLARIS)
    return (unsigned long)( (double)sysconf(_SC_PHYS_PAGES) * sysconf(_SC_PAGE_SIZE) / BYTES_PER_MEGABYTE );
#endif /*SUN||SOLARIS*/

#if defined(FREEBSD)
// GET Memory directly from sysctl
    long phys_mem;
    int sz=sizeof(phys_mem);
    sysctlbyname("hw.physmem",&phys_mem,&sz,0,0);
    return (unsigned long) phys_mem / BYTES_PER_MEGABYTE ;
#endif

#if defined(_WIN32)

    MEMORYSTATUSEX memoryStatus;
    memoryStatus.dwLength = sizeof(MEMORYSTATUSEX);

    if(0==GlobalMemoryStatusEx (&memoryStatus))
    {
        return 0;
    }
    else
    {
        return (SAPDB_ULong)(memoryStatus.ullTotalPhys/BYTES_PER_MEGABYTE);
    }

#endif /*_WIN32*/
}

/*-----------------*/

#ifndef _WIN32
static void readOneLine(RTE_FileHandle fd, void *buf, SAPDB_Long nbyte)
{
    SAPDB_Long bytesRead=0;

    while(bytesRead < nbyte-1)  /* the very last character is filled with the terminating zero-byte */
    {
        if(1 != RTE_save_read(fd,(SAPDB_Char *)buf+bytesRead,1))
        {
            break;
        }
        if('\n' == ((SAPDB_Char *)buf)[bytesRead])
        {
            break;
        }
        bytesRead++;
    }
    ((SAPDB_Char *)buf)[bytesRead] = 0;
}
#endif

externC SAPDB_ULong RTESys_PhysicalMemoryAvailable()
{
#if defined(USE_GLOBAL_MEMORY_STATUS_FOR_FREE_MEMORY_)
    MEMORYSTATUSEX memoryStatus;
    memoryStatus.dwLength = sizeof(MEMORYSTATUSEX);

    if(0==GlobalMemoryStatusEx (&memoryStatus))
    {
        return 0;
    }
    else
    {
        return (SAPDB_ULong)(memoryStatus.ullAvailPhys/BYTES_PER_MEGABYTE);
    }
#elif defined (USE_VMSTAT_FOR_FREE_MEMORY_)
    char receiveBuffer[MAXSTRSIZE];
    char *positionOfFre;    /* */
    char *beginningOfNumber;
    char *endOfNumber;
    SAPDB_ULong number;
    RTE_SystemRc rc;
    SAPDB_Int4 exitStatus;

    RTESys_CommandOutputPipeContext context;
    tsp01_RteError  rteError;
    RTE_Path path;
    if(!GetPath ( "vmstat", path ))
    {
        MSGD (( ERR_POPEN_SYSCALL, "vmstat", ENOENT ));
        return 0;
    }

    rc = RTESys_OpenCommandOutputPipe( &context, path);

    if(RTE_SYSTEMRC_NO_ERROR != rc)
    {
        MSGD (( ERR_POPEN_SYSCALL, "vmstat", errno));
        return 0;
    }

    do
    {
        readOneLine(context.cmdOutput, receiveBuffer, MAXSTRSIZE);
    }
    while(0 == (positionOfFre = strstr(receiveBuffer,"fre")));

    readOneLine(context.cmdOutput, receiveBuffer, MAXSTRSIZE);

    beginningOfNumber = positionOfFre;
    if(' ' == *beginningOfNumber)
    {
        while(' ' == *beginningOfNumber)
        {
            beginningOfNumber++;
        }
    }
    else
    {
        while(' ' != *beginningOfNumber)
        {
            beginningOfNumber--;
        }
        beginningOfNumber++;
    }

    endOfNumber = beginningOfNumber;
    while(' ' != *endOfNumber)
    {
        endOfNumber++;
    }
    *endOfNumber = 0;

    number = strtoul(beginningOfNumber,NULL,10);

    switch(*(endOfNumber-1))
    {
    case 'k':
    case 'K':
        number *= BYTES_PER_KILOBYTE;
        break;
    case 'm':
    case 'M':
        number *= BYTES_PER_MEGABYTE;
        break;
    case 'g':
    case 'G':
        number *= BYTES_PER_GIGABYTE;
        break;
    }

    number *= VMSTAT_MULTIPLIER_;
    number /= BYTES_PER_MEGABYTE;

    if(RTE_SYSTEMRC_NO_ERROR != RTESys_CloseCommandOutputPipe( &context, &exitStatus))
    {
        MSGD (( ERR_SYSCALL, "pclose", errno));
        return 0;
    }
    return number;

#elif defined(USE_PSTAT_FOR_FREE_MEMORY_)
	union pstun pu;

    struct pst_static pst;
    pu.pst_static = &pst;

    if ( pstat( PSTAT_STATIC, pu,(size_t)sizeof(pst), (size_t)0, 0 ) != -1 )
    {
        struct pst_dynamic dynamic;
    	pu.pst_dynamic = &dynamic;
        if ( pstat( PSTAT_DYNAMIC, pu,(size_t)sizeof(struct pst_dynamic), (size_t)0, 0 ) != -1 )
	    {
            return (unsigned long)( (double)dynamic.psd_free * ((double)pst.page_size  / BYTES_PER_MEGABYTE)) ;
        }
	}
#elif defined(USE_SYSCONF_FOR_FREE_MEMORY_)
    return (unsigned long)( ((double)sysconf(_SC_AVPHYS_PAGES) * sysconf(_SC_PAGE_SIZE)) / BYTES_PER_MEGABYTE);
#elif defined (USE_SYSINFO_FOR_FREE_MEMORY_)
    SAPDB_ULong numberAccordingToSysInfo;

    struct new_sysinfo info;
	memset(&info,0,sizeof(info));
	sysinfo((struct sysinfo *)&info);

	numberAccordingToSysInfo = (info.freeram);

	if(0 != info.mem_unit)
	{
		numberAccordingToSysInfo *= info.mem_unit;
	}
    return numberAccordingToSysInfo / BYTES_PER_MEGABYTE;
#elif defined(USE_PERFSTAT_FOR_FREE_MEMORY)
    perfstat_memory_total_t totalMemory;

    if ( 1 != perfstat_memory_total (0, &totalMemory, 144, 1) ) 
    // the size of the perfstat_memory_total_t structure grows from AIX release to AIX release, 
    // even within AIX 5.2-releases... As we cannot guarantee that the AIX we are running on is 
    // as new as the one we were built on, the smallest size of a perfstat_memory_total_t structure 
    // known is being used. It is the one from AIX 5.1 and is 144 bytes.
    {
        // If perfstat_memory_total() fails here, no external program is used as fallback. This is
        // because calling external programs can be very expensive and this routine is used frequently
        // by the dbanalyzer
        return 0;
    }
    else
    {
        return convertedFromSystemPagesToMegabytes(totalMemory.real_free);
    }
#else
    return 0;
#endif

}
#endif /* LIBOMS */

/*-----------------*/

externC void RTESys_SwapSize(SAPDB_ULong *totalMB,SAPDB_ULong *freeMB)
{
#if defined(AIX)
    perfstat_memory_total_t totalMemory;

    if ( 1 != perfstat_memory_total (0, &totalMemory, 144, 1) ) 
    // the size of the perfstat_memory_total_t structure grows from AIX release to AIX release, 
    // even within AIX 5.2-releases... As we cannot guarantee that the AIX we are running on is 
    // as new as the one we were built on, the smallest size of a perfstat_memory_total_t structure 
    // known is being used. It is the one from AIX 5.1 and is 144 bytes.
    {
        *totalMB = 0;
        *freeMB  = 0;
    }
    else
    {
        *totalMB = convertedFromSystemPagesToMegabytes(totalMemory.pgsp_total);
        *freeMB  = convertedFromSystemPagesToMegabytes(totalMemory.pgsp_free);
    }
#endif /*AIX*/

#if defined(HPUX)
    struct pst_swapinfo info;
    struct pst_static pst;
    union pstun pu;

    int counter;
    counter = 0;

    *totalMB = 0;
    *freeMB  = 0;

    while(pstat_getswap(&info,sizeof(info),1,counter))
    {
        *totalMB += info.pss_un.Pss_blk.Pss_nblks;
        *freeMB  += info.pss_nfpgs;
        counter++;
    }
    *totalMB /= BYTES_PER_KILOBYTE;

    pu.pst_static = &pst;

    if ( pstat( PSTAT_STATIC, pu,(size_t)sizeof(pst), (size_t)0, 0 ) != -1 )
    {
        *freeMB *= pst.page_size;
        *freeMB /= BYTES_PER_MEGABYTE;
    }
    else
    {
        freeMB = 0;
        MSGD (( ERR_SYSCALL, "pstat", errno));
    }
#endif /*HPUX*/

#if defined(LINUX)
    struct new_sysinfo info;
    double totalMemUnits=0.0,freeMemUnits=0.0;

    memset(&info,0,sizeof(info));
    if(-1 == sysinfo((struct sysinfo *)&info))
    {
        MSGD (( ERR_SYSCALL, "sysinfo", errno));
        return;
    }
    totalMemUnits = info.totalswap;
    freeMemUnits  = info.freeswap;

    /* in older linux kernels, the entries in the sysinfo struct were always given in bytes */
    /* in newer kernels, the entries are given in "mem_unit"s and a field mem_unit was added */
    /* in older versions there is a 0 at the space of this field. */
    if(0 != info.mem_unit)
    {
        totalMemUnits *= info.mem_unit;
        freeMemUnits  *= info.mem_unit;
    }
    *totalMB = totalMemUnits / (double)BYTES_PER_MEGABYTE;
    *freeMB  = freeMemUnits  / (double)BYTES_PER_MEGABYTE;
#endif /*LINUX*/

#if defined(SUN) || defined(SOLARIS) || defined(OSF1) || defined(NMP)
    /* the interface for swapctl() is a little difficult to use: When called with SC_LIST, it fills an array of structures. */
    /* Each swap device has an entry in this array. Unfortunately, the space for the array as well as space for the path    */
    /* entries in this structure has to be allocated before calling swapctl(). So swaptctl() has to be called with SC_GETNSWP */
    /* in order to determine the number of swap devices. Theoretically, the number of swap devices can change between */
    /* swapctl(SC_GETNSWP,..) and swapctl(SC_GETLIST,..). The latter call returns the number of swap devices actually found. */
    /* If this number differs from the one returned by swapctl(SC_GETNSWP..), the space for the array has to be allocated */
    /* once again and the whole process has to be repeated */
    myswaptbl_t    *swaptable;
    int            numberOfSwapTableEntries;        /* return value from swapctl(SC_GETNSWP,..) */
    int            actualNumberOfSwapTableEntries;  /* return value from swapctl(SC_LIST,..) */
    int            counter;
    char           *stringTable;    /* string table for path names */

    double totalPages=0.0,freePages=0.0;

again:
    if ((numberOfSwapTableEntries = swapctl(SC_GETNSWP, 0)) <= 0)
    {
        MSGD (( ERR_SYSCALL, "swapctl", errno));
        return;
    }
    /* allocate swaptable for num+1 entries */
    if ((swaptable = (myswaptbl_t *)malloc(numberOfSwapTableEntries * sizeof(myswapent_t) + sizeof(struct myswaptable))) == (void *) 0)
    {
        MSGD (( ERR_ALLOC_MEMORY, errno));
        return;
    }

    memset(swaptable,0,numberOfSwapTableEntries * sizeof(myswapent_t) + sizeof(struct myswaptable));
    /* allocate num+1 string holders */
    if ((stringTable = (char *)malloc((numberOfSwapTableEntries + 1) * MAXSTRSIZE)) == (void *) 0)
    {
        MSGD (( ERR_ALLOC_MEMORY, errno));
        return;
    }
    memset(stringTable,0,(numberOfSwapTableEntries + 1) * MAXSTRSIZE);
    /* initialize string pointers */
    for (counter = 0; counter < (numberOfSwapTableEntries + 1); counter++)
    {
        swaptable->swt_ent[counter].ste_path = stringTable + (counter * MAXSTRSIZE);
    }

    swaptable->swt_n = numberOfSwapTableEntries + 1;
    if ((actualNumberOfSwapTableEntries = swapctl(SC_LIST, swaptable)) < 0)
    {
        MSGD (( ERR_SYSCALL, "swapctl", errno));
        return;
    }
    if (actualNumberOfSwapTableEntries > numberOfSwapTableEntries)
    {        /* more were added */
        free(swaptable);
        free(stringTable);
        goto again;
    }
    /* If we have reached here, we finally have all swap devices in our swaptable...*/
    for (counter = 0; counter < actualNumberOfSwapTableEntries; counter++)
    {
        totalPages += swaptable->swt_ent[counter].ste_pages;
        freePages  += swaptable->swt_ent[counter].ste_free;
    }

    *totalMB = (SAPDB_UInt8)(totalPages * (double)RTESys_SystemPageSize() / (double)BYTES_PER_MEGABYTE);
    *freeMB  = (SAPDB_UInt8)(freePages  * (double)RTESys_SystemPageSize() / (double)BYTES_PER_MEGABYTE);

    free(swaptable);
    free(stringTable);
#endif /*SUN || SOLARIS || OSF1 || NMP*/

#if defined(_WIN32)
    MEMORYSTATUSEX memoryStatus;
    memoryStatus.dwLength = sizeof(MEMORYSTATUSEX);

    if(0==GlobalMemoryStatusEx (&memoryStatus))
    {
        *totalMB = 0;
        *freeMB  = 0;
    }
    else
    {
        *totalMB = (SAPDB_ULong)(memoryStatus.ullTotalPageFile/BYTES_PER_MEGABYTE);
        *freeMB  = (SAPDB_ULong)(memoryStatus.ullAvailPageFile/BYTES_PER_MEGABYTE);
    }
#endif /*_WIN32*/
}

/*-----------------*/

externC void RTESys_MemInfo(SAPDB_ULong *totalPhysicalMB,SAPDB_ULong *freePhysicalMB,SAPDB_ULong *totalSwapMB,SAPDB_ULong *freeSwapMB)
{
    RTESys_SwapSize(totalSwapMB,freeSwapMB);
    *totalPhysicalMB = RTESys_PhysicalMemorySize();
    *freePhysicalMB  = RTESys_PhysicalMemoryAvailable();
}

/*-----------------*/

#if !defined(LIBOMS) /* keep the following code out of liboms to keep it small... */
externC char const *RTESys_Hostname()
{
    static char const *pHostname = 0;
    if ( !pHostname )
    {
        char *myHostname;
#if defined(WIN32)
        static char myPhysicalHostname[MAX_COMPUTERNAME_LENGTH + 1];
        DWORD nameLength = MAX_COMPUTERNAME_LENGTH + 1;

        if ( !GetComputerName( myPhysicalHostname, &nameLength ) )
        {
            MSGD ((ERR_MISSING_HOSTNAME));
            return "MISSING HOSTNAME";
        }

        myHostname = &myPhysicalHostname[0];
#else
        struct utsname unam ;
        int rc = uname ( &unam );

        if ( rc < 0 )
        {
            char *lastError = strerror(errno);
            if ( lastError )
            {
                MSGD (( ERR_MGH_UNAME_FAILED, lastError ));
            }
            else
            {
                char numberBuffer[20]; /* strlen("errno()") + space for a number */
                sprintf(numberBuffer, "errno(%d)", errno );
                MSGD (( ERR_MGH_UNAME_FAILED, numberBuffer ));
            }
            return "MISSING HOSTNAME";
        }

        myHostname = malloc(strlen(unam.nodename)+1);
	    if ( !myHostname )
        {
            MSGD (( ERR_MGH_UNAME_TOO_LONG, unam.nodename ));
            return "NO MEMORY FOR HOSTNAME";
        }
        strcpy(myHostname, unam.nodename);
#endif
        pHostname = myHostname;
        while ( *myHostname )
        {
            *myHostname = toupper(*myHostname);
            ++myHostname;
        }
    }
    return pHostname;
}
#endif

#if !( defined(LIBOMS) || defined(LIBHSSCOPY) )  /* keep the following code out of liboms & libhsscopy to keep it tall... */
/*-----------------*/

#define BUFSIZE 80

externC SAPDB_Bool RTESys_GetOSVersion(SAPDB_Char *string,SAPDB_UInt4 length)
{
#ifdef _WIN32
    OSVERSIONINFOEX osvi;
    BOOL bOsVersionInfoEx;
    /* Try calling GetVersionEx using the OSVERSIONINFOEX structure.
     * If that fails, try using the OSVERSIONINFO structure.
     */

    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

    if( !(bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi)) )
    {
        osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
        if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) )
            return false;
    }

    switch (osvi.dwPlatformId)
    {
    /* Test for the Windows NT product family. */
    case VER_PLATFORM_WIN32_NT:

        /* Test for the specific product family. */
        if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 )
            SAPDB_sprintf (string, length, "Windows 2003 Server ");

        if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )
            SAPDB_sprintf (string, length, "Windows XP ");

        if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
            SAPDB_sprintf (string, length, "Windows 2000 ");

        if ( osvi.dwMajorVersion <= 4 )
            SAPDB_sprintf (string, length, "Windows NT ");

        /* Test for specific product on Windows NT 4.0 SP6 and later. */
        if( bOsVersionInfoEx )
        {
            /* Test for the workstation type. */
            if ( osvi.wProductType == VER_NT_WORKSTATION )
            {
                if( osvi.dwMajorVersion == 4 )
                    SAPDB_sprintf (string, length, "%sWorkstation 4.0 ",string );
                else if( osvi.wSuiteMask & VER_SUITE_PERSONAL )
                    SAPDB_sprintf (string, length, "%sHome Edition ",string  );
                else
                    SAPDB_sprintf (string, length,  "%sProfessional ",string  );
            }

            /* Test for the server type. */
            else if ( osvi.wProductType == VER_NT_SERVER )
            {
                if( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 )
                {
                    if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
                        SAPDB_sprintf (string, length,  "%sDatacenter Edition ",string  );
                    else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
                        SAPDB_sprintf (string, length,  "%sEnterprise Edition ",string  );
                    else if ( osvi.wSuiteMask == VER_SUITE_BLADE )
                        SAPDB_sprintf (string, length,  "%sWeb Edition ",string  );
                    else
                        SAPDB_sprintf (string, length,  "%sStandard Edition ",string  );
                }

                else if( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
                {
                    if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
                        SAPDB_sprintf (string, length,  "%sDatacenter Server ",string  );
                    else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
                        SAPDB_sprintf (string, length,  "%sAdvanced Server ",string  );
                    else
                        SAPDB_sprintf (string, length,  "%sServer ",string  );
                }

                else  /* Windows NT 4.0 */
                {
                    if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
                        SAPDB_sprintf (string, length, "%sServer 4.0, Enterprise Edition ",string  );
                    else
                        SAPDB_sprintf (string, length, "%sServer 4.0 ",string  );
                }
            }
        }
        else  /* Test for specific product on Windows NT 4.0 SP5 and earlier */
        {
            HKEY hKey;
            char szProductType[BUFSIZE];
            DWORD dwBufLen=BUFSIZE;
            LONG lRet;

            lRet = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
            "SYSTEM\\CurrentControlSet\\Control\\ProductOptions",
            0, KEY_QUERY_VALUE, &hKey );
            if( lRet != ERROR_SUCCESS )
                return false;

            lRet = RegQueryValueEx( hKey, "ProductType", NULL, NULL,
            (LPBYTE) szProductType, &dwBufLen);
            if( (lRet != ERROR_SUCCESS) || (dwBufLen > BUFSIZE) )
                return false;

            RegCloseKey( hKey );

            if ( lstrcmpi( "WINNT", szProductType) == 0 )
                SAPDB_sprintf (string, length,  "%sWorkstation ",string  );
            if ( lstrcmpi( "LANMANNT", szProductType) == 0 )
                SAPDB_sprintf (string, length,  "%sServer ",string  );
            if ( lstrcmpi( "SERVERNT", szProductType) == 0 )
                SAPDB_sprintf (string, length,  "%sAdvanced Server ",string  );

            SAPDB_sprintf (string,length,  "%s%d.%d ", string, osvi.dwMajorVersion, osvi.dwMinorVersion );
        }

        /* Display service pack (if any) and build number. */

        if( osvi.dwMajorVersion == 4 &&
            lstrcmpi( osvi.szCSDVersion, "Service Pack 6" ) == 0 )
        {
            HKEY hKey;
            LONG lRet;

            /* Test for SP6 versus SP6a. */
            lRet = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
            "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Hotfix\\Q246009",
            0, KEY_QUERY_VALUE, &hKey );
            if( lRet == ERROR_SUCCESS )
            {
                SAPDB_sprintf (string, length, "%s Service Pack 6a (Build %u)", string, osvi.dwBuildNumber & 0xFFFF );
            }
            else /* Windows NT 4.0 prior to SP6a */
            {
                SAPDB_sprintf (string,length,  "%s%s (Build %u)",string,osvi.szCSDVersion,osvi.dwBuildNumber & 0xFFFF);
            }

            RegCloseKey( hKey );
        }
        else /* Windows NT 3.51 and earlier or Windows 2000 and later */
        {
            SAPDB_sprintf (string,length,  "%s%s (Build %u)",string,osvi.szCSDVersion,osvi.dwBuildNumber & 0xFFFF);
        }


        break;

    /* Test for the Windows 95 product family. */
    case VER_PLATFORM_WIN32_WINDOWS:

        if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0)
        {
            SAPDB_sprintf (string, length, "Windows 95 ");
            if ( osvi.szCSDVersion[1] == 'C' || osvi.szCSDVersion[1] == 'B' )
                SAPDB_sprintf (string, length, "%sOSR2 ",string  );
        }

        if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10)
        {
            SAPDB_sprintf (string, length, "Windows 98 ");
            if ( osvi.szCSDVersion[1] == 'A' )
                SAPDB_sprintf (string, length, "%sSE ",string  );
        }

        if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90)
        {
            SAPDB_sprintf (string, length, "Windows Millennium Edition");
        }
        break;

    case VER_PLATFORM_WIN32s:

        SAPDB_sprintf (string, length, "Win32s");
        break;
    }
#else       /* now for the unix platforms: take the info from uname() */
    struct utsname utsNameStruct;
    uname(&utsNameStruct);
#ifdef AIX  /* special treatment for AIX: version is the major number, release the minor */
    SAPDB_sprintf(string,length,"%s %s.%s",
        utsNameStruct.sysname,
        utsNameStruct.version,
        utsNameStruct.release);
#else       /* all the others: release is major & minor, version contains furter information */
    SAPDB_sprintf(string,length,"%s %s %s",
        utsNameStruct.sysname,
        utsNameStruct.release,
        utsNameStruct.version);
#endif
#endif
   return true;
}

/*-----------------*/

externC SAPDB_Bool RTESys_GetCPUType(SAPDB_Char *string,SAPDB_UInt4 length)
{
#ifdef _WIN32
    switch (getSystemInfo()->wProcessorArchitecture)
    {
    case PROCESSOR_ARCHITECTURE_INTEL:
        SAPDB_sprintf(string,length,"Intel IA32");
        break;
    case PROCESSOR_ARCHITECTURE_IA64:
        SAPDB_sprintf(string,length,"Intel IA64");
        break;
    case PROCESSOR_ARCHITECTURE_IA32_ON_WIN64:
        SAPDB_sprintf(string,length,"Intel IA32 on WIN64");
        break;
    case PROCESSOR_ARCHITECTURE_AMD64:
        SAPDB_sprintf(string,length,"AMD64");
        break;
    }
    SAPDB_sprintf(string,length,"%s level %x revision %x",
        string,
        getSystemInfo()->wProcessorLevel,
        getSystemInfo()->wProcessorRevision);
#else   /* UNIX: take the info from uname */
    struct utsname utsNameStruct;
    uname(&utsNameStruct);
#if defined (AIX)
    SAPDB_sprintf(string,length,"IBM RS 6000 (%s)", utsNameStruct.machine);
#elif defined (HPUX)
/*
 * we do not use MaxDB defines ( HP9, PA11, PA20W, HP_IA64 ),
 * because HP9 is defined on both architectures ( __hppa, __ia64 )
 */
#if defined (__hppa)
	SAPDB_sprintf(string,length,"HP PA-RISC (%s)", utsNameStruct.machine);
#elif defined(__ia64)
    SAPDB_sprintf(string,length,"HP IA64 (%s)", utsNameStruct.machine);
#else
	SAPDB_sprintf(string,length,"HP (%s)", utsNameStruct.machine);
#endif
#elif defined (SUN) || defined (SOLARIS)
/* any kind of solaris */
#  if defined(__sparc) || defined(__sparcv9)
    SAPDB_sprintf(string,length,"SPARC (%s)", utsNameStruct.machine);
#  elif defined(__amd64)
    SAPDB_sprintf(string,length,"AMD64 (%s)", utsNameStruct.machine);
#  elif defined(__i386)
    SAPDB_sprintf(string,length,"Intel IA32 (%s)", utsNameStruct.machine);
#  else
	SAPDB_sprintf(string,length,"%s", utsNameStruct.machine);
#  endif
#else
    SAPDB_sprintf(string,length,"%s",
        utsNameStruct.machine);
#endif
#endif  /* WIN32 */
    return true;
}

/*-----------------*/

externC SAPDB_Bool RTESys_Is64BitPlatform ()
{
#ifdef _WIN32
    SYSTEM_INFO systemInfo;

    if (RTESys_NTGetNativeSystemInfo (&systemInfo))
    {
        if (PROCESSOR_ARCHITECTURE_IA64 == systemInfo.wProcessorArchitecture ||
            PROCESSOR_ARCHITECTURE_IA32_ON_WIN64 == systemInfo.wProcessorArchitecture ||
            PROCESSOR_ARCHITECTURE_AMD64 == systemInfo.wProcessorArchitecture)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {/* GetNativeSystemInfo could not be called. Therefore we are not running on Windows XP, Windows 2003
        Server or higher OS versions. This means especially that we are nor running on a 64 bit NT platform */
        return false;
    }
#else
    return ( sizeof(void *) == 8 );
#endif
}



/*-----------------*/

#if defined (AIX)

#include <nlist.h>
#define KERNEL_EXE_NAME "/unix"
static struct nlist namelist[] = {{"avenrun"},{NULL}};

#elif defined (OSF1)

#include <sys/table.h>

#elif defined(SUN) || defined (SOLARIS)

#include <sys/loadavg.h>

#endif



externC SAPDB_Real8 RTESys_GetCPULoad(void)
{
#if defined (WIN32)
    FILETIME idleTime,kernelTime,userTime;
    BOOL success;
    SAPDB_Real8 result = -1.0;
    /* call a wrapper that calls the windows API routine GetSystemTimes(). The wrapper call is necessary because
     * GetSystemTimes() is not available in Windows 2000
     */
    if(RTE_WindowsReleaseDependendGetSystemTimes(&idleTime,&kernelTime,&userTime,&success))
    {
        ULARGE_INTEGER idleTime64;
        ULARGE_INTEGER kernelTime64;
        ULARGE_INTEGER userTime64;

        idleTime64.LowPart  = idleTime.dwLowDateTime;
        idleTime64.HighPart = idleTime.dwHighDateTime;

        kernelTime64.LowPart  = kernelTime.dwLowDateTime;
        kernelTime64.HighPart = kernelTime.dwHighDateTime;

        userTime64.LowPart  = userTime.dwLowDateTime;
        userTime64.HighPart = userTime.dwHighDateTime;

        result = 1 -
            ((SAPDB_Real8)((SAPDB_Int8)idleTime64.QuadPart) /
            (((SAPDB_Int8)kernelTime64.QuadPart) + ((SAPDB_Int8)userTime64.QuadPart)));
    }
    return result;
#elif defined (AIX)
    static SAPDB_Bool initialized = false;
    static SAPDB_Int loadavg_seek,kmem;
    SAPDB_Long result = 0;
    if(!initialized)
    {
        nlist(KERNEL_EXE_NAME,namelist);
        loadavg_seek = namelist[0].n_value;
        kmem = open("/dev/kmem",0);
        if ( -1 != kmem )
        {
            initialized = true;
        }
    }
    if ( initialized )
    {
        lseek(kmem,loadavg_seek,0);

        read (kmem,(char *)&result,sizeof(result));
    }
    return (SAPDB_Real8)result / (SAPDB_Real8)65536;
#elif defined (OSF1)
    struct tbl_loadavg load_data;
    double scale_factor = 1.0;
    table ( TBL_LOADAVG , 0 , (char *)&load_data,1,sizeof(load_data));
    if ( 0 == load_data.tl_lscale )
    {
        return load_data.tl_avenrun.d[1];
    }
    else
    {
        return (SAPDB_Real8)load_data.tl_avenrun.l[1] / (SAPDB_Real8)load_data.tl_lscale;
    }
#elif defined (LINUX)
    static SAPDB_Bool initialized = false;
    static SAPDB_Int fd;
    SAPDB_Real8 result;
    SAPDB_Byte buf[10];

    if(!initialized)
    {
        fd = open("/proc/loadavg",0);
        if(fd < 0)
            return 0;
        initialized = true;
    }
    else
    {
        if(lseek(fd,0,0) < 0)
            return 0;
    }
    if(read(fd,buf,sizeof(buf)-1) < 0)
        return 0;

    sscanf(buf,"%lf",&result);

    return result;
#elif defined (HPUX)
    struct pst_dynamic pstd;
    pstat_getdynamic(&pstd,sizeof(pstd),0,0);
    return pstd.psd_avg_1_min;
#elif defined (SUN) || defined (SOLARIS)
    double loadavg[3];
    if(-1 == getloadavg(loadavg,3) )
        return -1.0;
    else
        return loadavg[LOADAVG_1MIN];
#else
    return -1.0;
#endif
}

/*-----------------*/

void RTESys_GetProcessTimes(SAPDB_UInt8 *secondsUser,SAPDB_UInt8 *secondsSystem)
{
#ifdef WIN32
    FILETIME creationTime;
    FILETIME exitTime;
    FILETIME kernelTime;
    FILETIME userTime;
    ULARGE_INTEGER time64;
    GetProcessTimes(GetCurrentProcess(),&creationTime,&exitTime,&kernelTime,&userTime);

    time64.LowPart  = kernelTime.dwLowDateTime;
    time64.HighPart = kernelTime.dwHighDateTime;
    *secondsSystem   = time64.QuadPart / 10000000;

    time64.LowPart  = userTime.dwLowDateTime;
    time64.HighPart = userTime.dwHighDateTime;
    *secondsUser     = time64.QuadPart / 10000000;
#else
    struct rusage r_usage;
    getrusage(RUSAGE_SELF,&r_usage);
    *secondsUser   = r_usage.ru_utime.tv_sec;
    *secondsSystem = r_usage.ru_stime.tv_sec;
#endif
}

/*-----------------*/

void RTESys_GetMachineTimes(SAPDB_UInt8 *secondsUser,SAPDB_UInt8 *secondsSystem,SAPDB_UInt8 *secondsIdle)
{
/* initialize the values so that all zero means failure */
    *secondsIdle    = 0;
    *secondsSystem  = 0;
    *secondsUser    = 0;
#ifdef WIN32
    {
        FILETIME idleTime;
        FILETIME kernelTime;
        FILETIME userTime;
        ULARGE_INTEGER time64;
        BOOL success;

        if(RTE_WindowsReleaseDependendGetSystemTimes(&idleTime,&kernelTime,&userTime,&success))
        {
            time64.LowPart  = idleTime.dwLowDateTime;
            time64.HighPart = idleTime.dwHighDateTime;
            *secondsIdle    = (time64.QuadPart / 10000000);

            time64.LowPart  = kernelTime.dwLowDateTime;
            time64.HighPart = kernelTime.dwHighDateTime;
            *secondsSystem  = (time64.QuadPart / 10000000) - *secondsIdle;

            time64.LowPart  = userTime.dwLowDateTime;
            time64.HighPart = userTime.dwHighDateTime;
            *secondsUser    = (time64.QuadPart / 10000000);
        }
    }
#elif defined (AIX)
    {
        SAPDB_UInt nHz   = (SAPDB_UInt)sysconf(_SC_CLK_TCK);
        perfstat_cpu_total_t cpuPerfStat;
        if ( 1 != perfstat_cpu_total( NULL,&cpuPerfStat,240/*sizeof(cpuPerfStat)*/,1))
        // the size of the perfstat_cpu_total_t structure grows from AIX release to AIX release, 
        // even within AIX 5.1-releases... As we cannot guarantee that the AIX we are running on is 
        // as new as the one we were built on, the smallest size of a perfstat_memory_cpu_t structure 
        // known is being used. It is the one from a very old AIX 5.1 and is 240 bytes.
        {
            *secondsIdle    = 0;
            *secondsSystem  = 0;
            *secondsUser    = 0;
        }
        else
        {
            *secondsIdle    = (cpuPerfStat.idle)/nHz;
            *secondsSystem  = (cpuPerfStat.sys )/nHz;
            *secondsUser    = (cpuPerfStat.user)/nHz;
        }
    }
#elif defined (HPUX)
    {
        struct pst_dynamic pstd;
        SAPDB_UInt nHz   = (SAPDB_UInt)sysconf(_SC_CLK_TCK);
        SAPDB_UInt4 nProc = RTESys_NumberOfCPU();
        pstat_getdynamic(&pstd,sizeof(pstd),0,0);
        *secondsIdle    = ((pstd.psd_cpu_time[CP_IDLE])/nHz)*nProc;
        *secondsSystem  = ((pstd.psd_cpu_time[CP_SYS] + pstd.psd_cpu_time[CP_WAIT])/nHz)*nProc;
        *secondsUser    = ((pstd.psd_cpu_time[CP_USER] + pstd.psd_cpu_time[CP_NICE])/nHz)*nProc;
    }
#elif defined (LINUX)
    {
        SAPDB_UInt nProc = (SAPDB_UInt)sysconf(_SC_NPROCESSORS_ONLN);
        SAPDB_UInt nHz   = (SAPDB_UInt)sysconf(_SC_CLK_TCK);
        static SAPDB_Bool initialized = false;
        static SAPDB_Int fd;
        SAPDB_UInt8 user=0,nice=0,system=0,idle=0;
        SAPDB_Char buf[80];
        SAPDB_Char *pBegin=&buf[0];
        if(!initialized)
        {
            fd = open("/proc/stat",0);
            if(fd < 0)
                return;
            initialized = true;
        }
        else
        {
            if(lseek(fd,0,0) < 0)
                return;
        }
        if(read(fd,buf,sizeof(buf)-1) < 0)
            return;

       /* the first line of /proc/stat is in the form "cpu  <user time> <nice time> <system time> <idle time> <....>"  */
       /* it is parsed here */
       /* first, skip until the first number is reached */
       while(isalpha(*pBegin)||isblank(*pBegin))
           pBegin++;
       user = atoll(pBegin);

       pBegin = strchr(pBegin,' ')+1;
       nice = atoll(pBegin);

       pBegin = strchr(pBegin,' ')+1;
       system = atoll(pBegin);

       pBegin = strchr(pBegin,' ')+1;
       idle = atoll(pBegin);

        *secondsIdle    = (idle)/nHz;
        *secondsSystem  = (system)/nHz;
        *secondsUser    = ((user+nice))/nHz;
    }
#elif defined (OSF1)
    {
        struct tbl_sysinfo sysinfo;
        SAPDB_UInt nProc = (SAPDB_UInt)sysconf(_SC_NPROCESSORS_ONLN);
        table ( TBL_SYSINFO , 0 , (char *)&sysinfo,1,sizeof(sysinfo));

        *secondsIdle    = (sysinfo.si_idle/sysinfo.si_hz);
        *secondsSystem  = (sysinfo.si_sys/sysinfo.si_hz);
        *secondsUser    = ((sysinfo.si_user+sysinfo.si_nice)/sysinfo.si_hz);
    }
#elif defined (SUN) || defined (SOLARIS)
    {
        static kstat_ctl_t    *kc  = NULL;
        static kstat_t        *ksp = NULL;
        static SAPDB_Bool initialized = false;
        static RTE_Lock     lock = {0};
        SAPDB_UInt nHz   = (SAPDB_UInt)sysconf(_SC_CLK_TCK);
        if(!initialized)
        {
            RTESys_Lock(&lock);
            kc  = kstat_open();
            if( kc )
            {
                ksp = kstat_lookup(kc, "cpu_stat", 0, "cpu_stat0");
                if( ksp )
                {
                    initialized = true;
                }
                else
                {
                    kstat_close( kc );  /* maybe next time... */
                    kc = NULL;
                }
            }
            RTESys_Unlock(&lock);
        }
        if(kc && ksp)
        {
            SAPDB_UInt4 nProc = (SAPDB_UInt4)sysconf(_SC_NPROCESSORS_ONLN);
            cpu_stat_t     statlast, cpu_stat;
            RTESys_Lock(&lock);
            kstat_read(kc, ksp, &cpu_stat);
            RTESys_Unlock(&lock);
            *secondsIdle    = ((cpu_stat.cpu_sysinfo.cpu[CPU_IDLE])/nHz)*nProc;
            *secondsUser    = ((cpu_stat.cpu_sysinfo.cpu[CPU_USER])/nHz)*nProc;
            *secondsSystem  = ((cpu_stat.cpu_sysinfo.cpu[CPU_KERNEL]+cpu_stat.cpu_sysinfo.cpu[CPU_WAIT])/nHz)*nProc;
        }
    }
#endif
}

#endif

/*===========================================================================*
 *  STATIC FUNCTIONS                                                         *
 *===========================================================================*/

#if defined(WIN32)

SAPDB_UInt4 RTESys_GetPhysicalProcessorInfomation(  SAPDB_UInt4*       processorRelatedness,
                                                    SAPDB_UInt4  const arraySize,
                                                    SAPDB_Bool   const affinitMaskDependent )
{
    BOOL                                    ok; 
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION   pBuffer                 = 0;
    DWORD                                   retry                   = 5;
    DWORD                                   returnLength            = 0;
    SAPDB_UInt4                             numOfPhysicalProcessors = 0;
    SAPDB_UInt4                             numOfLogicalProcessors;
    SAPDB_UInt4                             idx;
    SAPDB_UInt4                             processor;
    HMODULE                                 hKernel32;
    SAPDB_ULong                             systemAffinityMask;
    SAPDB_ULong                             processAffinityMask;


    numOfLogicalProcessors = getSystemInfo()->dwNumberOfProcessors;

    //  processorRelatedness array to small?
    if ( numOfLogicalProcessors > arraySize ) 
        return 0;

    if ( affinitMaskDependent )
    {
        // Get the process affinity
        if ( !GetProcessAffinityMask( GetCurrentProcess(), &processAffinityMask, &systemAffinityMask ))
            processAffinityMask = getSystemInfo()->dwActiveProcessorMask; // step through the active processor mask
    }
    else
        processAffinityMask = getSystemInfo()->dwActiveProcessorMask; // step through the active processor mask

    memset ( processorRelatedness, 0, sizeof(SAPDB_UInt4) * arraySize );

    hKernel32 = LoadLibrary( "kernel32.dll" );

    if ( 0 != hKernel32 )
    {
        typedef BOOL (__stdcall *tGLPI)( PSYSTEM_LOGICAL_PROCESSOR_INFORMATION Buffer, PDWORD ReturnLength ); 

        tGLPI pGetLogicalProcessorInformation = (tGLPI)GetProcAddress( hKernel32, "GetLogicalProcessorInformation" );

        if ( 0 != pGetLogicalProcessorInformation )
        {
            ok = pGetLogicalProcessorInformation( pBuffer, &returnLength );

            while ( retry-- && !ok && GetLastError() == ERROR_INSUFFICIENT_BUFFER )
            {
                pBuffer = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)alloca(returnLength);

                memset ( pBuffer, 0, returnLength );
                ok = pGetLogicalProcessorInformation( pBuffer, &returnLength );
            }

            if ( ok )
            {
                int        records = returnLength / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION) - 1;
                int        record;
                SAPDB_Bool found;

                // Step through the records of logical processor informations
                for ( record = 0; record < records ; record++ )
                {
                    // Is it a processor core relationship information?
                    if ( pBuffer[record].Relationship == RelationProcessorCore )
                    {
                        // Find all logical processors of this core
                        for( found = false, idx = 0; idx < numOfLogicalProcessors; idx++)
                        {
                            // Processor relationship found and is it allowed to run on it?
                            if (( pBuffer[record].ProcessorMask & ((SAPDB_UInt8)1) << idx ) &&
                                ( processAffinityMask           & ((SAPDB_UInt8)1) << idx ))
                            {
                                found = true; // one more phys. processor 
                                processorRelatedness[idx] = numOfPhysicalProcessors + 1;
                            }
                        }
                        if ( found )
                          numOfPhysicalProcessors++;
                    }
                }
            }
        }
        FreeLibrary(hKernel32);
    }
    if ( 0 == numOfPhysicalProcessors ) 
    {   // We could not determine any physical processor information. 
        OSVERSIONINFOEX  VerInfo;

        memset ( &VerInfo, 0, sizeof(VerInfo) );
        VerInfo.dwOSVersionInfoSize = sizeof(VerInfo);
        VerInfo.wProductType        = VER_NT_WORKSTATION;

        GetVersionEx ((LPOSVERSIONINFO)&VerInfo );

        if ( VerInfo.wProductType == VER_NT_WORKSTATION )
        {
            numOfPhysicalProcessors = 1;  // there is only one physical processor
            processor = 0;
        }
        else
        {
            numOfPhysicalProcessors = 0;
            processor = 1;
        }

        for( idx = 0; idx < numOfLogicalProcessors; idx++ )
        {
            numOfPhysicalProcessors += processor;

            if ( processAffinityMask & ((SAPDB_UInt8)1) << idx ) 
                processorRelatedness[idx] = numOfPhysicalProcessors;
        }
    }

    return numOfPhysicalProcessors;
}

/*-----------------*/

static SAPDB_UInt4 getNumOfPhysicalProcessors()
{
    static SAPDB_UInt4  numOfPhysicalProcessors = 0;

    if ( 0 == numOfPhysicalProcessors )
    {
        SAPDB_UInt4  processorRelatedness[MAX_PROCESSORS];
        numOfPhysicalProcessors = RTESys_GetPhysicalProcessorInfomation(  processorRelatedness, MAX_PROCESSORS, false );
    }

    return numOfPhysicalProcessors;
}


/*-----------------*/

static SAPDB_UInt4 getNumOfLogicalProcessors()
{
    return (SAPDB_UInt4)getSystemInfo()->dwNumberOfProcessors;
}

/*-----------------*/

static SYSTEM_INFO *getSystemInfo()
{
    static SYSTEM_INFO   systemInfo;
    static SYSTEM_INFO *pSystemInfo = 0;

    if ( !pSystemInfo )
    {
        GetSystemInfo(&systemInfo);
        pSystemInfo = &systemInfo;
    }
    return pSystemInfo;
}

#endif

#if defined(LINUX)

#define SIZEOFBUFFER 128

static SAPDB_UInt4 RTESys_GetPhysicalProcessorInfomation( SAPDB_UInt4* processorRelatedness,
                                                          SAPDB_UInt4  const arraySize )
{
    SAPDB_Int fdCpuInfo;
    SAPDB_Char Buffer[SIZEOFBUFFER];
    SAPDB_UInt4 NumberOfLogicalProcessors;
	SAPDB_UInt4 NumberOfPhysicalProcessors;
	SAPDB_Int Index;
	SAPDB_Int HasLogicalProcessors;

	NumberOfLogicalProcessors = ( SAPDB_UInt4 ) sysconf( _SC_NPROCESSORS_ONLN );
	HasLogicalProcessors = 0;
	
    //  processorRelatedness array to small?
    if ( NumberOfLogicalProcessors > arraySize )
        return 0;

    memset( processorRelatedness, 0, sizeof( SAPDB_UInt4 ) * arraySize );

    fdCpuInfo = open( "/proc/cpuinfo", O_RDONLY );
    if( fdCpuInfo >=0 )
	{
        memset( Buffer, 0, SIZEOFBUFFER );
        for( ;; )
		{
            SAPDB_Int got;
            SAPDB_Int want;
            SAPDB_Int offset;
            SAPDB_Char *ptr;
            SAPDB_Char *next;
            SAPDB_Char *key;
            SAPDB_Char *value;

            offset = strlen( Buffer );

            /* skip oversized line */
            if( offset >= SIZEOFBUFFER - 1 )
                offset = 0;

            want = SIZEOFBUFFER - offset - 1;
            got = read( fdCpuInfo, Buffer + offset, want );
            if( got < 0 )
            {
			    memset( processorRelatedness, 0, sizeof( SAPDB_UInt4 ) * arraySize );
                break;
            }

            if( got == 0 )
                break;

            for( ;; )
			{
                key = 0;
                value = 0;

				next = strchr( Buffer, '\n' );
                if( !next )
                    break;

                *next++ = '\0';

                key = Buffer;
                value = strchr( Buffer, ':' );
                if( value )
                {
                    *value++ = '\0';

                    while( *key && *key <= ' ' )
                        key++;

                    for( ;; )
                    {
                        ptr = key + strlen( key ) - 1;
                        if( ptr <= key || *ptr > ' ' )
                            break;

                        *ptr = '\0';
                    }

                    while( *value && *value <= ' ' )
                        value++;

                    for( ;; )
                    {
                        ptr = value + strlen( value ) - 1;
                        if( ptr <= value || *ptr > ' ' )
                            break;

                         *ptr = '\0';
                    }
                }

                if( key && value )
                {
                    if( strcmp( key, "processor" ) == 0 )
					{
						Index = strtol( value, 0, 10 );
					}
					else if( strcmp( key, "physical id" ) == 0  && Index < NumberOfLogicalProcessors )
					{
						processorRelatedness[Index] = strtol( value, 0, 10 );
						HasLogicalProcessors++;
					}
				}

                memcpy( Buffer, next, strlen( next ) + 1 );
            }

        }

        close( fdCpuInfo );
	}

	if( HasLogicalProcessors )
	{
		/* find any physical id */
		NumberOfPhysicalProcessors = 1;
		for( Index = 0; Index < NumberOfLogicalProcessors; Index++ )
		{
			SAPDB_Int SubIndex;
			SAPDB_Int PhysicalId;
			if( processorRelatedness[Index] )
			{
				/* remove all processors with current physical id */
				PhysicalId = processorRelatedness[Index];
				for( SubIndex = Index; SubIndex < NumberOfLogicalProcessors; SubIndex++ )
				{
					if( processorRelatedness[SubIndex] == PhysicalId )
						processorRelatedness[SubIndex] = 0;
				}

				NumberOfPhysicalProcessors++;
			}
		}
	}
	else
	{
		NumberOfPhysicalProcessors = NumberOfLogicalProcessors;
	}

	return NumberOfPhysicalProcessors;
}

static SAPDB_UInt4 getNumOfPhysicalProcessors()
{
    static SAPDB_UInt4 numOfPhysicalProcessors = 0;

    if ( 0 == numOfPhysicalProcessors )
    {
        SAPDB_UInt4 processorRelatedness[4096];
        numOfPhysicalProcessors = RTESys_GetPhysicalProcessorInfomation( processorRelatedness, 4096 );
    }

    return numOfPhysicalProcessors;
}

#endif

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
