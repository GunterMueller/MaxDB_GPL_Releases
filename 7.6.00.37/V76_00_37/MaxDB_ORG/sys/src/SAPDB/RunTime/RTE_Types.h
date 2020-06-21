/*!
  @file           RTE_Types.h
  @author         JoergM
  @ingroup        RunTime
  @brief          Contains all common RunTime Type definitions

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

#ifndef RTE_TYPES_H
#define RTE_TYPES_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/SAPDB_Types.h"

#if defined (_WIN32) && !defined (NO_WINDOWSH)

/*! ??? */
#define NEEDED_WIN32_WINNT 0x0500

#if !defined(_WIN32_WINNT)
/*! ??? */
# define _WIN32_WINNT  NEEDED_WIN32_WINNT
#else
# if _WIN32_WINNT < NEEDED_WIN32_WINNT
#  undef _WIN32_WINNT
/*! ??? */
#  define _WIN32_WINNT  NEEDED_WIN32_WINNT
# endif
#endif

#include <windows.h>
#include <tchar.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#endif

/* Special include files for atomic operations */

#if defined(AIX)
# include  <sys/atomic_op.h>
#endif /*AIX*/

#if defined(S390X)
# include <asm/spinlock.h>
#endif /* S390X */

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*!
  UNIX special memory allocation definitions...
  Two different system calls exists to allocate system page aligned memory.

  vmalloc() is an extension of malloc() and those pages are freed using the free() function.
  It has the disadvantage of unwanted overhead for the internal stored size information

  mmap() allows to directly map memory pages. So it has no overhead, but is problematic on some platforms.

  On Tru64(OSF1) a mmap() does block the 'growing' of heap. This leads to 'out of memory' situation
  when system calls internally use malloc()... Therefore mmap() always use fixed addresses on Tru64(OSF1)

  On AIX a mmap() call with MAP_PRIVATE|MAP_ANONYMOUS option always allocates a second page which is not
  accessable by anyone, but consumes physical memory... On this platform MAP_SHARED|MAP_ANONYMOUS is the
  only working solution.

  On all platforms MAP_FIXED|MAP_SHARED|MAP_ANONYMOUS seems to run into problems in combination with pthread
  creation... MAP_FIXED|MAP_PRIVATE|MAP_ANONYMOUS does not run into that problem (except for AIX...
 */

#if !defined(WIN32)
/*!
  On UNIX memory page allocation is done in different variations.
 */
# if defined(BIT64)
/*! 64Bit is not really a limited virtual address space...
    It allows to use gaps between each task stack
 */
#  undef HAS_VERY_LIMITED_VIRTUAL_ADDRESS_SPACE
#  if defined(OSF1)
/*! Tru64 (OSF1) has another very unusal problem. The number of mmap section is very limited...
    Therefore i.e. the task stack chunk is allocated as a single chunk...
 */
#   define HAS_LIMITED_NUMBER_OF_MMAP_SECTIONS
#  else
/*! other platforms do not have such a limited number of mmap sections */
#   undef HAS_LIMITED_NUMBER_OF_MMAP_SECTIONS
#  endif /* OSF1 */
# else
/*! 32Bit is really a limited virtual address space...Especially true for 32bit LINUX ... */
#  define HAS_VERY_LIMITED_VIRTUAL_ADDRESS_SPACE
# endif /* BIT64 */

# undef USE_FIXED_MMAP_ADDRESS
# undef USE_MMAP_ANONYMOUS
# undef USE_VMALLOC_NOT_MMAP

/*---------------------- vmalloc() using platforms ------------*/
# if (defined(LINUX) && (defined(I386) || defined(X86_64) || defined(SDBonPPC64))) || defined(AIX)
/*! For platforms that do not support mmap sufficiently */
#  define USE_VMALLOC_NOT_MMAP

# else

/*! ---------------------- mmap() using platforms ------------*/
#  define USE_MMAP_ANONYMOUS

#  if defined(BIT64)

#   if defined(HPUX)
/*! For HPUX 64Bit special mmap address area start here */
#    define USE_FIXED_MMAP_ADDRESS 0x8000020000000000UL
#   elif defined(OSF1)
/*! For OSF1 64Bit special mmap address area start here */
#    define USE_FIXED_MMAP_ADDRESS 0x0000010000000000UL
#elif defined(S390X)
/* Linux zSeries 64 Bit Address space layout
      ---------------- 0x3FFFFFFFFFF
      |              |
      |    Stack     |
      |              |
      ----------------
      |              |
      | Shared Libs  |
      |              |
      ---------------- 0x20000000000
      |              |
      | User Program |
      |  and Heap    |
      |              |
      ---------------- 0x00080000000
      |              |
      |    unused    |
      |              |
      ---------------- 0x00000000000

    start fixed mmap address at
       start of User Programm and Head  + 2 GByte
       0x80000000 + 0x80000000 = 0x100000000
 */
#    define USE_FIXED_MMAP_ADDRESS 0x0000000100000000UL
#   else
/*! For all other platforms except AIX,HPUX and OSF1 special mmap address area starts here */
#    define USE_FIXED_MMAP_ADDRESS 0x0000020000000000UL
#   endif

#  endif /* BIT64 */

# endif /* VMALLOC/MMAP */

#endif /* !WIN32 */

/*! --- mapped system return codes (see also: RTE_SystemRc) */
#if defined(_WIN32)
/*! call succeeded */
# define RTE_SYSTEMRC_NO_ERROR                   NO_ERROR
/*! file not found */
# define RTE_SYSTEMRC_FILE_NOT_FOUND             ERROR_FILE_NOT_FOUND
/*! path not found (only windows makes a difference to RTE_SYSTEMRC_FILE_NOT_FOUND) */
# define RTE_SYSTEMRC_PATH_NOT_FOUND             ERROR_PATH_NOT_FOUND
/*! file already exists */
# define RTE_SYSTEMRC_ALREADY_EXISTS             ERROR_ALREADY_EXISTS
/*! access denied */
# define RTE_SYSTEMRC_ACCESS_DENIED              ERROR_ACCESS_DENIED
/*! memory limit exceeded */
# define RTE_SYSTEMRC_NOT_ENOUGH_MEMORY          ERROR_NOT_ENOUGH_MEMORY
/*! out of memory (only windows makes a difference to RTE_SYSTEMRC_NOT_ENAUGH_MEMORY) */
# define RTE_SYSTEMRC_OUTOFMEMORY                ERROR_OUTOFMEMORY
#else /* UNIX */
/*! call succeeded */
# define RTE_SYSTEMRC_NO_ERROR                   0
/*! file not found */
# define RTE_SYSTEMRC_FILE_NOT_FOUND             ENOENT
/*! path not found (only windows makes a difference to RTE_SYSTEMRC_FILE_NOT_FOUND) */
# define RTE_SYSTEMRC_PATH_NOT_FOUND             ENOENT
/*! file already exists */
# define RTE_SYSTEMRC_ALREADY_EXISTS             EEXIST
/*! access denied */
# define RTE_SYSTEMRC_ACCESS_DENIED              EACCES
/*! memory limit exceeded */
# define RTE_SYSTEMRC_NOT_ENOUGH_MEMORY          ENOMEM
/*! out of memory (only windows makes a difference to RTE_SYSTEMRC_NOT_ENAUGH_MEMORY) */
# define RTE_SYSTEMRC_OUTOFMEMORY                ENOMEM
#endif

/*! --- formatting modes for Volumes */
/*! let runtime decide if formatting must be serialized */
#define RTE_FORMATTING_MODE_AUTO        0
/*! always enforce parallel formatting */
#define RTE_FORMATTING_MODE_PARALLEL    1
/*! always enforce serial formatting */
#define RTE_FORMATTING_MODE_SERIAL      2


#if !defined(_WIN32) && defined(BIT64)
/*! undefined physical volume id */
#define RTE_UNDEF_ID            0x7FFFFFFFFFFFFFF0
#else
/*! undefined physical volume id */
#define RTE_UNDEF_ID            0x7FFFFFF0
#endif

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
 Task Scheduler Identification (UKT)
 Identification of an SAPDB Kernel Task Scheduler
 */
typedef SAPDB_Int4   RTE_TaskSchedulerId;
#if defined(__cplusplus)
const   RTE_TaskSchedulerId   RTE_UnknownTaskSchedulerId = 0;  /*!< Reserved for returning unknown Task Scheduler Identifier */
#else
/*! this prevents C compiler from producing redefinition warnings.... */
#define RTE_UnknownTaskSchedulerId ((const RTE_TaskSchedulerId)0)
#endif

/*!
 Task Identification
 Identification of an SAPDB Kernel Task
 */
typedef SAPDB_Int4   RTE_TaskId;
#if defined(__cplusplus)
const   RTE_TaskId   RTE_UnknownTaskId = 0; /*!< Reserved for returning unknown Task Identifier */
#else
/*! this prevents C compiler from producing redefinition warnings.... */
#define RTE_UnknownTaskId ((const RTE_TaskId)0)
#endif



typedef SAPDB_UInt2  RTE_Reason;             /*!< Suspend Reason used in Suspend/Resume calls    */
typedef SAPDB_UInt4  RTE_TLSKey;             /*!< Thread Local Storage Key                       */

/*!
  Physical device identifier
 */
#if defined(_WIN32)
    typedef SAPDB_UInt4     RTE_DevID;
#else /* UNIX */
#if defined (BIT64)
    typedef SAPDB_Int8      RTE_DevID;
#else
    typedef SAPDB_Int4      RTE_DevID;
#endif
#endif


/*!
  Description: Command line argument string type
  
               Its the type of the command line argument string (argv) as passed to main().
               On Windows plattforms we might using _tmain() to pass unicode character string
               to the program.
*/
#if defined (_WIN32)
  typedef _TCHAR      RTE_ArgChar;    // on Windows plattforms we might using _tmain()
#else
  typedef char        RTE_ArgChar;
#endif


/*!
  Common Runtime Process ID
*/
#if defined(_WIN32)
 typedef DWORD           RTE_OSPid;
#else
#ifdef LINUX
 /* special case... pid_t must not be used in typedef */
 typedef int             RTE_OSPid;
#else
 typedef pid_t           RTE_OSPid;
#endif
#endif

/*! used to specify undefined process id */
#define RTE_UNDEF_OSPID        ((RTE_OSPid)-1)


/*!
  Memory object needed as core of a SpinLock

  The following system depend typedefinitions are needed to support backward compatibility
  with Interlocks created i.e. on shared memory segments allocated by older clients (<7.3...)

  The RTE_Lock should not be used directly with one exception: For communication with older
  clients via shared memory segments. In such a case this type is used to describe the lock.

  New implementation should use the classes defined in RTESync_Spinlock.hpp
 */
#if defined(_WIN32)
  typedef long       RTE_Lock;
#endif

#if defined(ALPHA)
# define SAPDB_CACHELINE_SIZE 128   
  typedef SAPDB_Long RTE_Lock[1]; /* atomic_p is a pointer to a long on ALPHA */
#endif

#if defined (FREEBSD)
  typedef SAPDB_Long RTE_Lock[1];
#endif

#if defined(AIX) || defined(SUN) || defined(SOLARIS) || (defined(LINUX) && !defined(S390X)) || (defined (HPUX) && defined (HP_IA64))
  typedef SAPDB_Int4 RTE_Lock[1]; /* atomic_p is a pointer to a 32bit word on AIX */
#endif

#if defined(PA20W) || defined(PA11)
  /*! Specifies cacheline size. Warning: If HP uses new architecture, cacheline size may change... */
# define SAPDB_CACHELINE_SIZE 64           /* Spinlock must be in seperate chacheline to */
                                           /* guarantie read/write consistency           */
  /* 32bit words are ok in 64bit mode too */
  typedef SAPDB_Int4 RTE_Lock[(SAPDB_CACHELINE_SIZE/sizeof(SAPDB_Int4))*2];
#endif /* PA20W || PA11 */

#if defined(S390X)
  typedef spinlock_t RTE_Lock;
#endif

/*
  End of this ugly RTE_Lock...
 *-------------------------------------------------------------------------------------------*/

  /*! C Strings are Zero byte terminate character arrays. The type definitions for C strings
     derived from a guarantuid PASCAL string length, must always use this macro. Otherwise
     on DEC i.e. we generated alignment problems, that produced core dumps due to unaligned data.
     This phenomenon seems no longer reproducable...                                              */
#define SAPDB_ALIGNED_CSTRING_SIZE(StringLength_) ((StringLength_+4)&(~3))

/*! path definitions */
  typedef       SAPDB_Char     RTE_Path     [ SAPDB_ALIGNED_CSTRING_SIZE( 256 ) ];
/*! filename definitions */
  typedef       SAPDB_Char     RTE_Filename [ SAPDB_ALIGNED_CSTRING_SIZE(  64 ) ];
/*! database name definitions */
  typedef       SAPDB_Char     RTE_DBName   [ SAPDB_ALIGNED_CSTRING_SIZE(  18 ) ];
/*! database version definitions */
  typedef       SAPDB_Char     RTE_Version  [ SAPDB_ALIGNED_CSTRING_SIZE(  40 ) ];
/*! nodename definitions */
  typedef       SAPDB_Char     RTE_Nodename [ SAPDB_ALIGNED_CSTRING_SIZE(  64 ) ];

/*!
  System return code

  Common to operating system interface is the type of the last error.
  Examples are 'int errno' on UNIX and 'DWORD(unsiged long) GetLastError()' on NT.
 */
#if defined(_WIN32) || defined(WIN32)
  typedef unsigned long   RTE_SystemRc;
#else
  typedef int             RTE_SystemRc;
#endif

/*! used to specify undefined return code */
# define RTE_UNDEF_ERROR        ((RTE_SystemRc)-1)

/*!
  SpinLoop instance statistic record

  The statistic contains the number of locks and collisions. For each collision the
  number of spin loops (if any) and the number of yield loops are counted and the
  total number as well as the maximum number is kept. All these values are written
  after the lock succeeded, so they are consistent, even if another task is already
  spinning. The currentLoops count is the only value that is 'dirty written', that
  means each time a spinning tasks tests the lock, it updates this counter to its
  individual value. This value is consistent only for the task.
 */
typedef struct RTE_SpinlockStatistic_ {
  SAPDB_Int8 locks;           /*!< total number of locks */
  SAPDB_Int8 collisions;      /*!< total number of collisions */
  SAPDB_Int4 totalSpinLoops;  /*!< total number of spin loops needed */
  SAPDB_Int4 totalYieldLoops; /*!< total number of yield loops needed */
  SAPDB_Int4 maxSpinLoops;    /*!< maximum number of spin loops needed */
  SAPDB_Int4 maxYieldLoops;   /*!< maximum number of yield loops needed */
  SAPDB_Int4 currentLoops;    /*!< current number of loops needed (spin loops or yield loops) */
} RTE_SpinlockStatistic;


#if defined(_WIN32)         /* PTS 1110642 */
/*! windows specific extended memory status structure */
typedef MEMORYSTATUSEX  RTE_NTMEMORYSTATUSEX;
#endif

/*!
   Version strings maximum allowed string length
 */
#define     RTE_MAXSPEEDLEN     6
/*! Fast kernel : kernel */
#define     RTE_FAST    "FAST"
/*! Quick kernel : quickknl */
#define     RTE_QUICK   "QUICK"
/*! Slow kernel : slowknl */
#define     RTE_SLOW    "SLOW"
/*! Test kernel : omststknl */
#define     RTE_TEST    "TEST"


#if defined(_WIN32)
/*! File handle */
typedef HANDLE              RTE_FileHandle;
/*! Semaphore handle */
typedef HANDLE              RTE_SemaphoreHandle;
/*! File offset */
typedef SAPDB_Int8          RTE_FileOffset;
/*! Security attributes */
typedef SECURITY_ATTRIBUTES RTE_SECURITY_ATTRIBUTES;
#else
/*! Security attributes */
typedef int                 RTE_SECURITY_ATTRIBUTES;
/*! File handle */
typedef int                 RTE_FileHandle;
/*! Semaphore handle */
typedef int                 RTE_SemaphoreHandle;
/*! File offset */
typedef off_t               RTE_FileOffset;
#endif

/*! specifies an undefined handle value */
#if defined(_WIN32)
#define RTE_UNDEF_HANDLE            (INVALID_HANDLE_VALUE)
#else
#define RTE_UNDEF_HANDLE            ((int)-1)
#endif


/*!
  Describes how to handle symbols in DLL.

  RTE_LoadSymbolsGlobal means all symbols are tried to be imported into global scope.

  RTE_LoadSymbolsLocal means all symbols are DLL local only. This allows to load two DLLs
  with the same symbols defined.
 */
typedef enum {
    RTE_LoadSymbolsGlobal = 1 /*!< means all symbols are tried to be imported into global scope. */
  , RTE_LoadSymbolsLocal = 2  /*!< means all symbols are DLL local only. */
} RTE_LoadMode;

/*!
   Describes the open mode. The type may descibe the way to open a
   IPC or synchronisation object.

               RTE_OpenNew:      Open a new object
               RTE_OpenExisting: Open an existing object for read + write access
               RTE_OpenExistingReadOnly: Open an existing object for read only access
 */

typedef enum {RTE_OpenUndef = 0,
              RTE_OpenNew,
              RTE_OpenExisting,
              RTE_OpenExistingReadOnly
} RTE_OpenMode;

/*!
   Handle for dynamically loaded shared library
 */
#if defined(_WIN32)
typedef HMODULE    RTE_SharedLibHandle;
#else
typedef void *     RTE_SharedLibHandle;
#endif
/*! used to specifiy illegal shared library handle */
#define RTE_ILLEGAL_SHARED_LIB_HANDLE ((RTE_SharedLibHandle)0)

/*!
   Function address used to call inside a dynamically loaded shared library (extern "C" !!!)
 */
typedef void (*RTE_SharedLibProcAddress)();

/*!
   Describes the type of a Volume
 */
typedef enum
{
    RTE_VolumeTypeSystem       = 0,
    RTE_VolumeTypeLog          = 1,
    RTE_VolumeTypeMirrorLog    = 2,
    RTE_VolumeTypeData         = 3,
    RTE_VolumeTypeTrace        = 4,
    RTE_VolumeTypeIgnore       = -1
}
RTE_VolumeType ;

/*!
   Describes the type of an I/O Medium
 */
typedef enum
{
    RTE_MediumTypeUnknown      = 0 /*!< type not specified (default) */
   ,RTE_MediumTypeFile         = 1 /*!< file medium */
   ,RTE_MediumTypePipe         = 2 /*!< pipe medium (unidirectional) */
   ,RTE_MediumTypeRaw          = 3 /*!< raw device */
   ,RTE_MediumTypeNorewindTape = 4 /*!< tape that does not automatically rewind on close */
   ,RTE_MediumTypeRewindTape   = 5 /*!< tape that does automatically rewind on close */
   ,RTE_MediumTypeDafs_File    = 6 /*!< file medium accessed via DAFS */
}
RTE_MediumType ;

/*!
  Describes the access mode of a volume

  Normal: No restirction on usage (random or streaming)
  Sequential: Streaming mode preferred
 */
typedef enum
{
    RTE_VolumeAccessModeUnknown    = 0,
    RTE_VolumeAccessModeNormal     = 1,
    RTE_VolumeAccessModeSequential = 2
}
RTE_VolumeAccessMode ;

/* for DAFS support PTS 1124541 */
/*!
  Describes the file system used to acces a volume

  Normal: Operating system provides file system
  DAFS: Special user level DirectAccessFileSystem used
 */
typedef enum
{
    RTE_FileSystemAccessTypeUnknown    = 0,
    RTE_FileSystemAccessTypeNormal     = 1,
    RTE_FileSystemAccessTypeDAFS       = 2
}
RTE_FileSystemAccessType ;

/*!
    @brief Return values for asynchronous Io calls (replaces tsp00_VaReturn)

    @li RTE_AsyncIoOk        was former va_ok
    @li RTE_AsyncIoFailed    was former va_notok
    @li RTE_AsyncIoEof       was former va_eof
    @li RTE_AsyncIoRetry     was former va_wait_first
    @li RTE_AsyncIoPending   is new and used internally only
 */
typedef enum
{
    RTE_AsyncIoOk=0    /*!< async I/O call succeeded */
   ,RTE_AsyncIoFailed  /*!< async I/O call failed */
   ,RTE_AsyncIoEof     /*!< async I/O call hit end of file */
   ,RTE_AsyncIoRetry   /*!< async I/O call needs retry (out of async I/O resources...) */
   ,RTE_AsyncIoPending /*!< async I/O call still pending */
}
RTE_AsyncIoResult ;


typedef SAPDB_Int4   RTE_RegionId;             /*!< Identification of an SAPDB Kernel Task Region        */

/*!
    @brief values to specify 

    @li RTE_StartAutomatic  start database on system start
    @li RTE_StartOnDemand   database must be started by hand 
    @li RTE_StartUnchanged  leave the current configuration unchanged 
 */
#define RTE_SERVICE_ENTRY_UNCHANGED -1
typedef enum 
{
    RTE_StartAutomatic  = true,                             /*!< start database on system start */
    RTE_StartOnDemand   = false,                            /*!< database must be started by hand */
    RTE_StartUnchanged  = RTE_SERVICE_ENTRY_UNCHANGED       /*!< leave the current configuration unchanged */
} 
RTE_StartOption;



#endif  /* RTE_TYPES_H */
