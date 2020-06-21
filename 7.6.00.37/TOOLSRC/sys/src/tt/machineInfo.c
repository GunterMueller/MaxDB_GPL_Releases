/*!***************************************************************************

  module      : machineInfo.c

  -------------------------------------------------------------------------

  responsible : JoergM

  special area: RunTime
  description : OS Machine information


  last changed: 2001-07-02  9:04
  see also    : 

  -------------------------------------------------------------------------

  copyright:    Copyright by SAP AG, 2001

  ========== licence begin LGPL
  Copyright (C) 2001 SAP AG

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
  ========== licence end

*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>

#if defined(_WIN32) || defined(_WIN64)
#   if defined (_INTEGRAL_MAX_BITS) && _INTEGRAL_MAX_BITS >= 64
        typedef unsigned __int64    SAPDB_UInt8;     /* integer (8 byte, unsigned) */
        typedef signed __int64      SAPDB_Int8;      /* integer (8 byte, signed)   */
#   else
#       error __int64 type not supported
#   endif
#else
#   if defined (BIT64)
        typedef unsigned long       SAPDB_UInt8;     /* integer (8 byte, unsigned) */
        typedef signed long         SAPDB_Int8;      /* integer (8 byte, signed)   */
#   else
        typedef unsigned long long  SAPDB_UInt8;     /* integer (8 byte, unsigned) */
        typedef signed long long    SAPDB_Int8;      /* integer (8 byte, signed)   */
#   endif
#endif /* UNIX(tm) tested on AIX,DEC,LINUX,HP,SNI and SUN ;-) */

		/* Output of df -k is OS dependend */
#if defined(AIX) || defined(HPUX) || defined(SUN) || defined(SOLARIS) || defined(OSF1) || defined(NMP)
#include <sys/types.h>
#include <sys/statvfs.h>
#define USE_STATVFS
#elif defined(LINUX)
#include <sys/types.h>
#include <sys/statfs.h>
#define USE_STATFS
#endif

#if defined(AIX)
#  include <sys/vminfo.h>
#  include <dirent.h>
#endif /* AIX */

#if defined(OSF1)
#  include <sys/sysinfo.h>
#  include <machine/hal_sysinfo.h>
#endif /* OSF1 */

#if defined(OSF1) || defined(AIX) || defined(NMP)
#define USE_VMSTAT_FOR_FREE_MEMORY_
#endif /* OSF1 | AIX | NMP */

#if defined(SUN) || defined(OSF1) || defined(NMP) || defined(SOLARIS)

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

#endif /* SUN || NMP || OSF1 */

#include <string.h>

const char *progName = (const char *)0;

/* These calls output their result using printf */
static void showNodename(void);
static void showProcessors(void);
static void showOperatingSystem(void);
static void showTotalSwapspace(void);
static void showAvailSwapspace(void);
static void showTotalMemory(void);
static void showAvailMemory(void);
static void showAvailDiskspace(void);
static void showPagesize(void);
static void showByteorder(void);

static char const *info_DiskSpacePath = ".";

typedef struct info_
{
   const char *infoTag;
   int showInfo;
   char optionChar;
   const char *optionUsage;
   void (*showCall)(void);
} info;

#if defined(SUN) || defined(OSF1) || defined(NMP) || defined(AIX) || defined(SOLARIS)
#define MAXSTRSIZE 128
#endif

#define BYTES_PER_KBYTE 1024UL

#define MAX_OPTION_USAGE 128 /* Mamimum length of option usage message */
static info infoArray[] =
{
    { "NODENAME:                     ", 0, 'N', "nodename               (%s Name)",      showNodename },
    { "PROCESSORS:                   ", 0, 'P', "number of processors   (%s Number)",    showProcessors },
    { "OPERATING_SYSTEM:             ", 0, 'O', "operating system       (%s Name)",      showOperatingSystem },
    { "BYTE_ORDER:                   ", 0, 'B', "byte order             (%s 1234/4321)", showByteorder },
    { "MEMORY_PAGE_SIZE(Bytes):      ", 0, 'M', "memory page size       (%s Bytes)",     showPagesize },
    { "TOTAL_MEMORY(KBytes):         ", 0, 'T', "total memory limit     (%s KBytes)",    showTotalMemory },
    { "AVAILABLE_MEMORY(KBytes):     ", 0, 'A', "available memory       (%s KBytes)",    showAvailMemory },
#define LAST_TO_SHOW 6 /* options below are hidden */
    { "TOTAL_SWAP_SPACE(KBytes):     ", 0, 'S', "total swap space       (%s KBytes)",    showTotalSwapspace },
    { "AVAILABLE_SWAP_SPACE(KBytes): ", 0, 'V', "available swap space   (%s KBytes)",    showAvailSwapspace },
    { "FREE_DISKSPACE(KBytes):       ", 0, 'F', "[path] free disk space (%s KBytes)",    showAvailDiskspace },
};
static int nInfo = sizeof(infoArray)/sizeof(info);

/*
  Function: Usage
  Description: Display usage information
  Arguments: msg[in] optional message (if 0 ignored)
             badArg [in] optional bad argument for message (if 0 ignored)
 */
void Usage(const char *msg, const char *badArg)
{
    char outputFormat[MAX_OPTION_USAGE]; /* some usefull line length... */
    int iInfo;

    if ( msg )
    {
        if ( badArg )
        {
            printf("%s:%s\n", msg, badArg);
        }
        else
        {
            printf("%s\n", msg);
        }
    }

    printf("Syntax: %s {<option>}\n", progName);

    sprintf(outputFormat, "Options: -%c %s\n", infoArray[0].optionChar, infoArray[0].optionUsage); 
    printf(outputFormat, infoArray[0].infoTag);

    for ( iInfo = 1; iInfo < nInfo; iInfo++ )
    {
        sprintf(outputFormat, "         -%c %s\n", infoArray[iInfo].optionChar, infoArray[iInfo].optionUsage); 
        printf(outputFormat, infoArray[iInfo].infoTag);
    }

    printf("If no option is given first %d options are shown in this order.\n",
        LAST_TO_SHOW+1);
    exit(1);
}

/*
   Function: main
   Description: program main routine
   Arguments: argc [in] argument count
              argv [in] argument vector
   Return value: exit code
 */
int main(int argc, char *argv[])
{
    int wantDiskSpacePath = 0;
    int iInfo;

    progName = argv[0];

    if ( argc > 1 )
    {
        int iArg = 1;
        while ( iArg < argc )
        {
            if ( argv[iArg][0] == '-' )
            {
                int iOpt = 1;

                if ( 0 == argv[iArg][1] )
                {
                    Usage("Only '-' given", (const char *)0);
                }

                do
                {
                    if ( 0 != argv[iArg][iOpt] )
                    {
                        char optionChar = toupper(argv[iArg][iOpt]);
                        
                        if ( '?' == optionChar
                           || 'H' == optionChar )
                        {
                            Usage((const char *)0, (const char *)0);
                        }

                        if ( 'F' == optionChar )
                        {
                            if ( argv[iArg][iOpt+1] )
                            {
                                info_DiskSpacePath = (char const *)strdup(&argv[iArg][iOpt+1]);
                                iOpt += strlen(info_DiskSpacePath);
                            }
                            else
                            {
                                wantDiskSpacePath = 1;
                            }
                        }
                        else
                        {
                            wantDiskSpacePath = 0;
                        }

                        for ( iInfo = 0; iInfo < nInfo; iInfo++ )
                        {
                            if ( optionChar == infoArray[iInfo].optionChar )
                            {
                                infoArray[iInfo].showInfo = 1;
                                break;
                            }
                        }

                        if ( iInfo >= nInfo )
                        {
                            Usage("Illegal option", &(argv[iArg][iOpt]) );
                        }

                        ++iOpt;
                    }
                } while ( 0 != argv[iArg][iOpt] );
            }
            else
            {
                if ( wantDiskSpacePath )
                {
                    info_DiskSpacePath = (char const *)strdup(argv[iArg]);
                }
                else
                {
                    Usage("Missing option leadin", argv[iArg]);
                }
            }
            ++iArg;
        }
    }
    else
    {
        for ( iInfo = 0; iInfo <= LAST_TO_SHOW; iInfo++ )
        {
            infoArray[iInfo].showInfo = 1;
        }
    }

    for ( iInfo = 0; iInfo < nInfo; iInfo++ )
    {
        if ( 1 == infoArray[iInfo].showInfo )
        {
            printf("%s ", infoArray[iInfo].infoTag);
            infoArray[iInfo].showCall();
            printf("\n", infoArray[iInfo].infoTag);
        }
    }

    return 0;
}

#if !defined(_WIN32)
#ifdef HPUX
#define USE_HPUX_PSTAT
#endif /* HPUX */

#ifdef NMP
#define USE_SNI_MPCNTL
#endif /* NMP */

#ifdef LINUX
#define USE_LINUX_PROCFS
#include <stdio.h>
#include <sys/sysinfo.h>
/* The field mem_unit in the sysinfo structure had been introduced with linux 2.4 */
/* It gives a multiplier for the elements in the structure that describe amounts  */
/* of memory. To make the code compilable under all releases of the linux kernel, */
/* the structure is definde here and not taken from /usr/include/linux/kernel.h   */
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
#define LINUX_SYSINFO
#endif /* LINUX */

#include <unistd.h>
#include <sys/utsname.h>
#ifdef FREEBSD
#include <machine/resource.h>
#else
#include <sys/resource.h>
#endif

#ifdef USE_HPUX_PSTAT
#include <sys/param.h>
#include <sys/pstat.h>
#endif /* USE_HPUX_PSTAT */

#ifdef USE_SNI_MPCNTL
#include <sys/types.h>
#include <sys/mpcntl.h>
#endif /* USE_SNI_MPCNTL */

#ifdef USE_LINUX_PROCFS
/*
  Function: searchTaggedValueInFile
  Description: Search first line matching tag and copy it into given buffer
  
    Isolate first argument found in buffer after tag was skipped.

  Arguments: fileName [in] The file to scan
             tag [in] The tag to search
  Return value: Pointer to isolated first value or pointer to empty string if not found
 */
char *searchTaggedValueInFile(const char *fileName, const char *tag)
{
#define MAX_LINE_LENGTH BYTES_PER_KBYTE
    static char buffer[MAX_LINE_LENGTH];
    FILE *file;
    char *retValue;
    char *findEnd;

    file = fopen(fileName, "r");
    if ( 0 == file )
    {
        buffer[0] = 0;
        retValue = buffer;
    }
    else
    {
        do
        {
            if ( fgets(buffer, MAX_LINE_LENGTH, file) == NULL )
            {
                buffer[0] = 0;
                return buffer;
            }
        } while ( strncmp(buffer, tag, strlen(tag)) != 0 );

        retValue = &buffer[strlen(tag)];
        while( *retValue != 0
           && (*retValue == ' ' || *retValue == '\t') )
        {
            ++retValue;
        }

        findEnd = retValue;
        while( *findEnd != 0 
            && *findEnd != ' ' 
            && *findEnd != '\t'
            && *findEnd != '\n' )
        {
            ++findEnd;
        }
        *findEnd = 0;

        fclose(file);
    }

    return retValue;
}
#endif

static void showNodename(void)
{
    struct utsname name;
    uname (&name);
    printf("%s", name.nodename);
}

static void showProcessors(void)
{
#if defined(_SC_NPROCESSORS_ONLN)
    /* AIX + SOLARIS + DEC + LINUX */
    long nProc = sysconf(_SC_NPROCESSORS_ONLN);
    printf("%ld", nProc);
#elif defined(USE_HPUX_PSTAT)
    /* HPUX */
    struct pst_processor buf;
    int nProc = 0;

    while ( 0 != pstat_getprocessor(&buf, sizeof(struct pst_processor), 1, nProc) )
        nProc++;

    printf("%d", nProc);
#elif defined(USE_SNI_MPCNTL)
    /* NMP */
    int nProc = mpcntl(MPCNTL_CPUCNT, (void *)0);
    printf("%d", nProc);
#else
    printf("?");
#endif
}

static void showPagesize(void)
{
#if defined(_SC_PAGESIZE)
    /* AIX + SOLARIS + SNI + IBM + HPUX */
    long nPageSize = sysconf(_SC_PAGESIZE);
    printf("%lu", nPageSize);
#elif defined(_SC_PAGE_SIZE)
    /* AIX + SOLARIS */
    long nPageSize = sysconf(_SC_PAGE_SIZE);
    printf("%lu", nPageSize);
#endif
}

static SAPDB_UInt8 info_totalSwapSpace = 0;
static SAPDB_UInt8 info_availSwapSpace = 0;

static void getSwapspace(void)
{
    if ( 0 == info_totalSwapSpace || 0 == info_availSwapSpace )
    {
#if defined(HPUX)
    struct pst_swapinfo info;

    int counter;
    for ( counter = 0; pstat_getswap(&info,sizeof(info),1,counter); ++counter )
    {
        info_totalSwapSpace  += (((SAPDB_UInt8)info.pss_un.Pss_blk.Pss_nblksavail) * BYTES_PER_KBYTE);
        info_availSwapSpace  += (((SAPDB_UInt8)info.pss_nfpgs) * ((SAPDB_UInt8)sysconf(_SC_PAGESIZE)));
    }
#endif /*HPUX*/

#if defined(LINUX)
    struct new_sysinfo info;

    memset(&info,0,sizeof(info));
    if(-1 == sysinfo((struct sysinfo *)&info))
    {
        perror("sysinfo:");
    }
    else
    {
        info_totalSwapSpace  = (SAPDB_UInt8)info.totalswap;
        info_availSwapSpace  = (SAPDB_UInt8)info.freeswap;

        /* in older linux kernels, the entries in the sysinfo struct were always given in bytes */
        /* in newer kernels, the entries are given in "mem_unit"s and a field mem_unit was added */
        /* in older versions there is a 0 at the space of this field. */
        if(0 != info.mem_unit)
        {
            info_totalSwapSpace  *= info.mem_unit;
            info_availSwapSpace  *= info.mem_unit;
        }
    }
#endif /*LINUX*/

#if defined(AIX)
    /* AIX only offers a function to get details about a given swap devices */
    /* So all block devices in /dev/ are enumerated and given to this function */
    struct dirent *dirEntry;
    DIR *directoryHandle;

    static struct pginfo    pginfo;
    double totalPages=0.0;
    double freePages=0.0;
    char *deviceName;

    if (NULL == (directoryHandle = opendir("/dev")))
    {
        perror("opendir(/dev):");
    }
    else
    {
        struct stat statStruct;

        /* read all entries from /dev/ */
        while (dirEntry = readdir(directoryHandle))
        {
            deviceName = malloc(strlen(dirEntry->d_name) + strlen("/dev/") + 1);
            if(NULL == deviceName)
            {    /* NULL == deviceName --> could not allocate memory */
                perror("malloc:");
                break;
            }
            else 
            {
                strcpy(deviceName,"/dev/");
                strcat(deviceName,dirEntry->d_name);
                /* check if it is a block device. The function would work without this check, but the swapqry() call seems to */
                /* take much more time than the stat() call */
                if(-1 == stat(deviceName,&statStruct))
                {
                    perror("stat:");
                    break;
                }
                if(_S_IFBLK == (statStruct.st_mode & _S_IFBLK))
                {
                    if (swapqry(deviceName, &pginfo) != -1) 
                    {
                        /* If we reach here, we have really found a swap device and can add the size */
                        totalPages += pginfo.size;
                        freePages  += pginfo.free;
                    }
                }
                free(deviceName);
            }
        }
        if(-1 == closedir(directoryHandle))
        {
            perror("closedir:");
        }
    }

    info_totalSwapSpace = ((SAPDB_UInt8)totalPages) * sysconf(_SC_PAGESIZE);
    info_availSwapSpace = ((SAPDB_UInt8)freePages) * sysconf(_SC_PAGESIZE);
#endif /* AIX */

#if defined(SUN) || defined(OSF1) || defined(NMP) || defined(SOLARIS)
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

    double totalPages=0.0;
    double freePages=0.0;

again:
    if ((numberOfSwapTableEntries = swapctl(SC_GETNSWP, 0)) <= 0) 
    {
        perror("swapctl(SC_GETNSWP):");
    }
    else if ((swaptable = (myswaptbl_t *)malloc(numberOfSwapTableEntries * sizeof(myswapent_t) + sizeof(struct myswaptable))) == (void *) 0) 
    {
        perror("malloc:");
    }
    else
    {
        memset(swaptable,0,numberOfSwapTableEntries * sizeof(myswapent_t) + sizeof(struct myswaptable));
        /* allocate num+1 string holders */
        if ((stringTable = (char *)malloc((numberOfSwapTableEntries + 1) * MAXSTRSIZE)) == (void *) 0) 
        {
            perror("malloc:");
        }
        else
        {
            memset(stringTable,0,(numberOfSwapTableEntries + 1) * MAXSTRSIZE);
            /* initialize string pointers */
            for (counter = 0; counter < (numberOfSwapTableEntries + 1); counter++) 
            {
                swaptable->swt_ent[counter].ste_path = stringTable + (counter * MAXSTRSIZE);
            }

            swaptable->swt_n = numberOfSwapTableEntries + 1;
            if ((actualNumberOfSwapTableEntries = swapctl(SC_LIST, swaptable)) < 0) 
            {
                perror("swapctl(SC_LIST):");
            } else if (actualNumberOfSwapTableEntries > numberOfSwapTableEntries) 
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

#if defined(_SC_PAGESIZE)
            info_totalSwapSpace = ((SAPDB_UInt8)totalPages) * sysconf(_SC_PAGESIZE);
            info_availSwapSpace = ((SAPDB_UInt8)freePages) * sysconf(_SC_PAGESIZE);
#elif defined(_SC_PAGE_SIZE)
            info_totalSwapSpace = ((SAPDB_UInt8)totalPages) * sysconf(_SC_PAGE_SIZE);
            info_availSwapSpace = ((SAPDB_UInt8)freePages) * sysconf(_SC_PAGE_SIZE);
#endif
            free(swaptable);
            free(stringTable);
        }
    }
#endif /*SUN || SOLARIS || OSF1 || NMP*/
    }
}

static void showTotalSwapspace(void)
{
    getSwapspace();
    if ( info_totalSwapSpace > 0 )
    {
        printf("%ld", (unsigned long)(info_totalSwapSpace/BYTES_PER_KBYTE));
    }
    else
    {
        printf("?");
    }
}

static void showAvailSwapspace(void)
{
    getSwapspace();
    if ( info_availSwapSpace > 0 )
    {
        printf("%ld", (unsigned long)(info_availSwapSpace/BYTES_PER_KBYTE));
    }
    else
    {
        printf("?");
    }
}

static void showTotalMemory(void)
{
    SAPDB_UInt8 nTotalPhysMemoryBytes = 0;

#if defined(LINUX_SYSINFO)

    struct new_sysinfo info;

    memset(&info,0,sizeof(info));

    if(-1 == sysinfo((struct sysinfo *)&info))
    {
        perror("sysinfo");
    }
    else
    {
        nTotalPhysMemoryBytes = (SAPDB_UInt8)info.totalram;

        if(0 != info.mem_unit)
        {
            nTotalPhysMemoryBytes *= info.mem_unit;
        }
    }

#elif defined(USE_HPUX_PSTAT)

    struct pst_static pst;
    union pstun pu;
    
    pu.pst_static = &pst;
    if ( pstat( PSTAT_STATIC, pu,(size_t)sizeof(pst), (size_t)0, 0 ) != -1 ) 
    {
        nTotalPhysMemoryBytes = (SAPDB_UInt8)( (double)pst.physical_memory * pst.page_size ) ;
    }
    else 
    {
        perror("pstat:");
    }

#elif defined(OSF1)

    int memsize;
    getsysinfo(GSI_PHYSMEM,(char *)&memsize,sizeof(int),0,0,0); /* returns KBYTE */
    nTotalPhysMemoryBytes = ((SAPDB_UInt8)memsize) * BYTES_PER_KBYTE;

#elif defined(AIX)

    char receiveBuffer[MAXSTRSIZE];
    FILE *fileHandle;
    char *beginOfNumber,*endOfNumber;

    fileHandle = popen("lsattr -E -l sys0 -a realmem","r");
    if(NULL == fileHandle)
    {
        perror("popen(lsattr -E -l sys0 -a realmem):");
    }
    else
    {
        fgets(receiveBuffer,MAXSTRSIZE,fileHandle);
        if(-1 == pclose(fileHandle))
        {
            perror("pclose:");
        }
        else
        {
            /* the lsattr command gives the size of the memory in a string like */
            /* "realmem 8388608 Amount of usable physical memory in Kbytes False" */
            /* so the output has to be parsed */
            beginOfNumber = strchr(receiveBuffer,' ');
            if(0 != beginOfNumber)
            {
                endOfNumber = strchr(beginOfNumber+1,' ');
                if(0 != endOfNumber)
                {
                    *endOfNumber = 0;
                    nTotalPhysMemoryBytes = (SAPDB_UInt8)strtoul(beginOfNumber+1,NULL,10) * BYTES_PER_KBYTE;
                }
            }
        }
    }

#elif defined(NMP)

    char receiveBuffer[MAXSTRSIZE];
    FILE *fileHandle;
    char *beginOfNumber;

    fileHandle = popen("uname -M","r");
    if(NULL == fileHandle)
    {
        perror("popen(uname -M):");
    }
    else
    {
        fgets(receiveBuffer,MAXSTRSIZE,fileHandle);
        if(-1 == pclose(fileHandle))
        {
             perror("pclose:");
        }
        else
        {
            beginOfNumber = strchr(receiveBuffer,'/'); /* result is number of CPU / Megabytes installed */
            if(0 != beginOfNumber)
            {
                nTotalPhysMemoryBytes = ((SAPDB_UInt8)strtoul(beginOfNumber+1,NULL,10)) * BYTES_PER_KBYTE * BYTES_PER_KBYTE;
            }
        }
    }
#elif defined(_SC_PHYS_PAGES)
    /* SUN */
    nTotalPhysMemoryBytes = ((SAPDB_UInt8)sysconf(_SC_PHYS_PAGES)) * sysconf(_SC_PAGE_SIZE);
#endif

    if ( nTotalPhysMemoryBytes > 0 )
    {
        printf("%ld", (long)(nTotalPhysMemoryBytes/BYTES_PER_KBYTE));
    }
    else
    {
        printf("?");
    }
}

static void showAvailMemory(void)
{
    SAPDB_UInt8 nTotalAvailMemoryBytes = 0;
#if defined(LINUX_SYSINFO)

    struct new_sysinfo info;

    memset(&info,0,sizeof(info));

    if(-1 == sysinfo((struct sysinfo *)&info))
    {
        perror("sysinfo");
    }
    else
    {
        nTotalAvailMemoryBytes = (SAPDB_UInt8)info.freeram;

        if(0 != info.mem_unit)
        {
            nTotalAvailMemoryBytes *= info.mem_unit;
        }
    }

#elif defined(USE_HPUX_PSTAT)

    struct pst_dynamic pst;
    union pstun pu;
    
    pu.pst_dynamic = &pst;
    if ( pstat( PSTAT_DYNAMIC, pu,(size_t)sizeof(pst), (size_t)0, 0 ) != -1 ) 
    {
        nTotalAvailMemoryBytes = (SAPDB_UInt8)( (double)pst.psd_free * sysconf(_SC_PAGESIZE) ) ;
    }
    else 
    {
        perror("pstat:");
    }

#elif defined (USE_VMSTAT_FOR_FREE_MEMORY_)

	FILE *fileHandle;
    char receiveBuffer[MAXSTRSIZE];
    char *positionOfFre;    /* */
    char *beginningOfNumber;
    char *endOfNumber;

    fileHandle = popen("vmstat","r");
    if(NULL == fileHandle)
    {
        perror("popen(vmstat):");
    }
	else
	{
		do
		{
			fgets(receiveBuffer,MAXSTRSIZE,fileHandle);
		}
		while(0 == (positionOfFre = strstr(receiveBuffer,"fre")));

		fgets(receiveBuffer,MAXSTRSIZE,fileHandle);

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

		nTotalAvailMemoryBytes = (SAPDB_UInt8)strtoul(beginningOfNumber,NULL,10);
#if defined(NMP)
	    nTotalAvailMemoryBytes *= BYTES_PER_KBYTE;
#elif defined(_SC_PAGESIZE)
	    nTotalAvailMemoryBytes *= sysconf(_SC_PAGESIZE);
#elif defined(_SC_PAGE_SIZE)
	    nTotalAvailMemoryBytes *= sysconf(_SC_PAGE_SIZE);
#endif

        switch(*(endOfNumber-1))
		{
		case 'k':
		case 'K':
			nTotalAvailMemoryBytes *= BYTES_PER_KBYTE;
			break;
		case 'm':
		case 'M':
			nTotalAvailMemoryBytes *= BYTES_PER_KBYTE * BYTES_PER_KBYTE;
			break;
		case 'g':
		case 'G':
			nTotalAvailMemoryBytes *= BYTES_PER_KBYTE * BYTES_PER_KBYTE * BYTES_PER_KBYTE;
			break;
		default:
			/* alreadsy in bytes */
			break;
		}

		if(-1 == pclose(fileHandle))
		{
            perror("pclose:");
		}
	}

#elif defined(_SC_AVPHYS_PAGES)
    /* SOLARIS */
    nTotalAvailMemoryBytes = ((SAPDB_UInt8)sysconf(_SC_AVPHYS_PAGES)) * sysconf(_SC_PAGE_SIZE);

#endif
    if ( nTotalAvailMemoryBytes > 0 )
    {
        printf("%ld", (long)(nTotalAvailMemoryBytes/BYTES_PER_KBYTE));
    }
    else
    {
        printf("?");
    }
}

static void showAvailDiskspace(void)
{
    SAPDB_UInt8 nDiskSpaceAvailable = 0;
#if defined(USE_STATVFS)
    struct statvfs fsInfo;
    if ( statvfs(info_DiskSpacePath, &fsInfo) < 0 )
	{
		perror("statvfs(.):");
	}
	else
	{
		nDiskSpaceAvailable = ((SAPDB_UInt8)fsInfo.f_bavail) * fsInfo.f_frsize;
	}
#elif defined(USE_STATFS)
    struct statfs fsInfo;
    if ( statfs(info_DiskSpacePath, &fsInfo) < 0 )
	{
		perror("statfs(.):");
	}
	else
	{
		nDiskSpaceAvailable = ((SAPDB_UInt8)fsInfo.f_bavail) * fsInfo.f_bsize;
	}
#endif
	if ( nDiskSpaceAvailable > 0 )
    {
        printf("%ld", (long)(nDiskSpaceAvailable/BYTES_PER_KBYTE));
    }
    else
    {
        printf("?");
    }
}

#else

#include <windows.h>

SYSTEM_INFO   systemInfo;
SYSTEM_INFO *pSystemInfo = 0;

MEMORYSTATUS   memoryStatus;
MEMORYSTATUS *pMemoryStatus = 0;

static void UpdateSystemInfo()
{
    if ( !pSystemInfo )
    {
        GetSystemInfo(&systemInfo);
        pSystemInfo = &systemInfo;
    }

    if ( !pMemoryStatus )
    {
        GlobalMemoryStatus(&memoryStatus);
        pMemoryStatus = &memoryStatus;
    }
}

static void showNodename(void)
{
    char hostnameBuffer[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD bufferLen = MAX_COMPUTERNAME_LENGTH + 1;
    if ( GetComputerName(hostnameBuffer, &bufferLen) )
    {
        printf(hostnameBuffer);
    }
    else
    {
        printf("?");
    }
}
static void showProcessors(void)
{
    UpdateSystemInfo();
    printf("%d", (int)(systemInfo.dwNumberOfProcessors) );
}
static void showTotalSwapspace(void)
{
    UpdateSystemInfo();
    printf("%d", (int)(memoryStatus.dwTotalPageFile/BYTES_PER_KBYTE) );
}
static void showAvailSwapspace(void)
{
    UpdateSystemInfo();
    printf("%d", (int)(memoryStatus.dwAvailPageFile/BYTES_PER_KBYTE) );
}
static void showTotalMemory(void)
{
    UpdateSystemInfo();
    printf("%d", (int)(memoryStatus.dwTotalPhys/BYTES_PER_KBYTE) );
}
static void showAvailMemory(void)
{
    UpdateSystemInfo();
    printf("%d", (int)(memoryStatus.dwAvailPhys/BYTES_PER_KBYTE) );
}
static void showAvailDiskspace(void)
{
    ULARGE_INTEGER freeBytesAvailable;
    ULARGE_INTEGER totalBytes;
    ULARGE_INTEGER freeBytesOnDisk;
    if ( !GetDiskFreeSpaceEx(
        info_DiskSpacePath,
        &freeBytesAvailable,
        &totalBytes,
        &freeBytesOnDisk ) )
    {
        printf("GetDiskFreeSpaceEx(%s) failed:%d\n", info_DiskSpacePath, GetLastError());
    }
    else
    {
        printf("%ld\n", (long)(freeBytesAvailable.QuadPart/BYTES_PER_KBYTE) );
    }
}

static void showPagesize(void)
{
    UpdateSystemInfo();
    printf("%d", (int)(systemInfo.dwPageSize) );
}
#endif

static void showOperatingSystem(void)
{
#if defined(OSF1)
    printf("TRUE64");
#elif defined(LINUX)
    printf("LINUX");
#elif defined(AIX)
    printf("AIX");
#elif defined(NMP)
    printf("SNI");
#elif defined(SUN)
    printf("SUN");
#elif defined(SOLARIS)
    printf("SOLARIS");
#elif defined(HPUX)
    printf("HPUX");
#elif defined(_WIN32)
    printf("WIN");
#endif
}

static void showByteorder(void)
{
    unsigned int testVal = 0x34333231;
    char *ptr = (char *)&testVal;
    putchar(*ptr++);
    putchar(*ptr++);
    putchar(*ptr++);
    putchar(*ptr++);
}
