/*!
  @file           veo06.c
  @author         DanielD
  @special area
  @brief          RTE - Virtual File for Applications
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

/*LINTLIBRARY*/

#define MOD__ "veo06.c: "

#include  "gsp00.h"
#include  "geo007_2.h"
#include  "heo01.h"
#if !defined (_WIN32)
#include  "gen00.h"     /* nocheck */
#endif
#include  "heo06.h"

#if defined (_WIN32)   /*&if $OSSPEC = WIN32*/
#include "vsp002c.h"   /* nocheck */
#include "gos00.h"     /* nocheck */
#include "hos06.h"     /* nocheck */
#include "geo007_1.h"  /* nocheck */
#include "gos44.h"     /* nocheck */
#include "gsp01.h"     /* nocheck */
#else                  /*&else*/
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 199506L
#endif
#include <limits.h>
#include "heo06.h"     /* nocheck */
#include "heo602.h" /* nocheck */
#endif                 /*&endif*/

#include "hsp78_0.h"
#include "heo46.h"
#include "geo54.h"

#include "SAPDBCommon/SAPDB_string.h"
#include "RunTime/RTE_EnvironmentVariables.h"

/*
 * Convention for macro names:
 *      *_OS : macro hides differences between operating systems
 *      *_V  : virtual method dispatch
 *      *_INL: inline functions
 *      *_DBG: debugging and testing
 */


#define vf_t_tape               6
#define vf_t_directory          7

#ifdef  DEBUG_RTE
#if defined (_WIN32)
#define TRCIN       { }
#define TRCIN_P(x)  { }
#define TRCOUT      { }
#define TRCOUT_P(x) { }
#define TRCPAS      { }
#define TRCPAS_P(x) { }
#else
#define TRCIN       { sqldbgwrite  (MF__"--> called\n"); }
#define TRCIN_P(x)  { sqldbgwrite  (MF__"--> called: \n"); sqldbgwrite x; }
#define TRCOUT      { sqldbgwrite  (MF__"<-- returning\n"); }
#define TRCOUT_P(x) { sqldbgwrite x; sqldbgwrite  (MF__"<-- returning: \n"); }
#define TRCPAS      { sqldbgwrite  (MF__"-- passed -->\n"); }
#define TRCPAS_P(x) { sqldbgwrite x; sqldbgwrite  (MF__"-- passed -->\n"); }
#endif
#else
#define TRCIN       { }
#define TRCIN_P(x)  { }
#define TRCOUT      { }
#define TRCOUT_P(x) { }
#define TRCPAS      { }
#define TRCPAS_P(x) { }
#endif


#if defined(_WIN32)
  #define EO06_PATH_MAX_C FILENAME_MAX
#else
  #define EO06_PATH_MAX_C PATH_MAX
#endif

/**\
--------------------------------------- MF__ MOD__ (local imports)
\**/

#if defined (_WIN32)
LONG sql06_io_read ( HANDLE    hfDevice,
                     ULONG     ulDevType,
                     PVOID     pBlockPtr,
                     ULONG     ulBlockLength,
                     PULONG    pulBytesRead );

LONG sql06_io_write( HANDLE    hfDevice,
                     ULONG     ulDevType,
                     PVOID     pBlockPtr,
                     ULONG     ulBlockLength,
                     PULONG    pulBytesWritten );

BOOLEAN sql06c_handle_known_errors ( const char*        pszName,
                                     LONG                rc,
                                     PCHAR               pcErrText);
#else
    extern char *         sqlerrs();
#endif

/**\
--------------------------------------- MF__ MOD__ (local declarations)
\**/

#if defined (LINT)
#define USEVAR_DBG(var)     var
#else
#define USEVAR_DBG(var)
#endif
#define SWITCH_ERROR_DBG(str,val)
#define OUT_OF_ENUM_INL(val, low, high)     (val > high)
    /* MACRO MF__ MOD__ (OUT_OF_ENUM_INL) */
typedef unsigned char byte;
typedef struct FileT FileT;
#define eo06_VALIDPOS(file)     (file->filePos >= 0)
static void eo06_osError (tsp05_RteFileError *ferr);
static void eo06_lockError (tsp05_RteFileError *ferr);
static tsp00_Longint eo06_writeBufferedText (FileT* file, const byte* buf,
    tsp00_Longint bufLen, tsp05_RteFileError* err);

static const byte * eo06_encchr (const void * buf,
        const tsp77charConst * charConst, int bufSize);
static int eo06_enclen (const tsp77encoding * encoding, const void * buf);

#if defined (_WIN32)
static LONG    eo06c_tape_open         ( const char*         tapeName,
                                            FileT*              file,
                                            tsp05_RteFileError*  ferr,
                                            BOOLEAN             reading);

static LONG    eo06c_con_open          (const char*                 fname,
                                        FileT*                      file,
                                        tsp05_RteFileError*          ferr);

static LONG    eo06c_pipe_write_open   (const char*                 fname,
                                        FileT*                      file,
                                        tsp05_RteFileError*          ferr);

static LONG    eo06c_pipe_read_open    (const char           *HostFileName,
                                         FileT               *file,
                                         tsp05_RteFileError   *ferr);


 static LONG eo06c_init_dlt_tape       ( FileT   *file ) ;

 static LONG eo06c_get_drive_info      ( FileT   *file,
                                          tsp05_RteFileError        *ferr);

 static LONG eo06c_get_media_info      ( FileT   *file,
                                          tsp05_RteFileError        *ferr);

 static LONG eo06c_init_tape_dev       ( FileT   *file,
                                          tsp05_RteFileError        *ferr);

 static LONG eo06c_tape_rewind         ( FileT   *file,
                                          tsp05_RteFileError        *ferr);

 static LONG eo06c_tape_locking        ( FileT   *file,
                                          tsp05_RteFileError        *ferr,
                                          ULONG                ulOperation );

 static LONG eo06c_tape_loading        ( FileT   *file,
                                          tsp05_RteFileError        *ferr,
                                          ULONG                ulOperation );

 static LONG eo06c_tape_write_mark     ( FileT   *file,
                                          tsp05_RteFileError        *ferr);
#else
 static void eo06_mkfifoUnix( const char *PipeName,
                                          tsp05_RteFileError *ferr);
#endif

/**\
--------------------------------------- MF__ MOD__ (errors)
\**/

static char File_At_End_ErrText [] ="End of file";
static char File_Not_Eraseable_ErrText [] = "File not deletable";
static char File_Not_Found_ErrText [] = "File not found";
static char File_Open_For_Read_ErrText [] = "File open for read only";
static char File_Open_For_Write_ErrText [] = "File open for write only";
static char Invalid_Handle_ErrText [] = "Invalid handle";
static char Invalid_Parameter_ErrText [] = "Parameter error: ";
static char No_Reg_File_ErrText [] = "Not a regular file";
static char Out_Of_Memory_ErrText [] = "Out of memory";
static char No_Seek_ErrText [] = "Seek not allowed";
static char File_Not_Lockable_ErrText [] = "File cannot be locked";
static char Seek_Range_ErrText [] = "Seek out of range";
static char Invalid_FileType_ErrText [] = "File type invalid for this operation";
static char Check_Before_Read_ErrText [] = "Check filetype before first read";
static char Invalid_Open_ErrText [] = "Invalid combination of open parameters";
static char Pagesize_Not_OK_ErrText [] = "Page size given %d, required %d";
static char Wrong_Encoding_ErrText [] = "File not encoded as %s";
static char Missing_Encoding_ErrText [] = "Encoding required";
static char General_Encoding_ErrText [] = "Encoding error '%s'";

/*------------------------------*/

static void
eo06_clearError (
    tsp05_RteFileError *ferr)
{
#undef MF__
#define MF__ MOD__"eo06_clearError"
    ferr->sp5fe_result = VF_OK;
    ferr->sp5fe_warning = sp5vfw_no_warning;
    ferr->sp5fe_text [0] = '\0';
}

/*------------------------------*/

static void
eo06_ferror (
    tsp05_RteFileError *ferr,
    const char* errText)
{
#undef MF__
#define MF__ MOD__"eo06_ferror"
    ferr->sp5fe_result = VF_NOTOK;
    strcpy ((char *)ferr->sp5fe_text, errText);
    TRCPAS_P (("VF_NOTOK: '%s'\n", errText));
}

/*------------------------------*/

static void
eo06_eofError (
    tsp05_RteFileError *ferr)
{
#undef MF__
#define MF__ MOD__"eo06_eofError"
    ferr->sp5fe_result = VF_EOF;
    strcpy ((char *)ferr->sp5fe_text, File_At_End_ErrText);
    TRCPAS_P (("VF_EOF: '%s'\n",File_At_End_ErrText ));
}

/*------------------------------*/

#if defined (_WIN32)

static void
eo06_osError (
    tsp05_RteFileError *ferr)
{
#undef MF__
#define MF__ MOD__"eo06_osError"  /* WIN32 */
    LONG        rc = GetLastError ();

    ferr->sp5fe_result = VF_NOTOK;
    if (!sql06c_handle_known_errors ("", rc, (char *)ferr->sp5fe_text)) {
        tsp00_C40c msg;
        sp77sprintf(msg,sizeof(tsp00_C40),"OS error: [%d]", GetLastError ());
        eo46CtoP ( ferr->sp5fe_text, msg, sizeof(tsp00_C40) );
    }
    TRCPAS_P (("VF_NOTOK: '%s'\n", (char *)ferr->sp5fe_text));
}
#else
/*------------------------------*/


static void
eo06_osError (
    tsp05_RteFileError *ferr)
{
#undef MF__
#define MF__ MOD__"eo06_osError"  /* UNIX */
    tsp00_C40c msg;
    ferr->sp5fe_result = VF_NOTOK;
    sp77sprintf(msg,sizeof(tsp00_C40),"OS error: '%s'", sqlerrs ());
    eo46CtoP ( ferr->sp5fe_text, msg, sizeof(tsp00_C40) );
    TRCPAS_P (("VF_NOTOK: '%s'\n", (char *)ferr->sp5fe_text));
}
#endif

/*=================================================================== */
#ifndef DIRECTORY_OPS_ONLY
/*=================================================================== */

/**\
--------------------------------------- MF__ MOD__ (NT)
\**/

#if defined (_WIN32)
#include <windows.h>

typedef HANDLE          OSFileHandleT;
#define invalidHandleC  INVALID_HANDLE_VALUE
#define errorResultC    (-1)
#define envVarCharC     '%'
#define eolMarkerSizeC  2

#define eo06_ISLINK_OS(xstat)       FALSE

static BOOLEAN
eo06_fileOpenWin32 (
    const char* fname,
    tsp05_RteFileMode_Param fileMode,
    tsp05_RteBufferingKind_Param buffering,
    FileT* file,
    tsp05_RteFileError *ferr);

static void eo06_fileClose (
    FileT* file,
    tsp05_RteCloseOption_Param option,
    tsp05_RteFileError* err);

#define E06_FILEOPEN_OS(fname, fileMode, buffering, file, ferr) eo06_fileOpenWin32\
    (fname, fileMode, buffering, file, ferr)


#define eo06_LONGSEEK_UNBUFFERED eo06_longseekUnbuffered
#define eo06_LONGSEEK_BUFFERED_READ eo06_longseekBufferedRead
#define eo06_LONGSEEK_BUFFERED_WRITE eo06_longseekBufferedWrite
#define SPECIAL_LONGSEEK

# if defined (_WIN64)
  typedef tsp00_Longint LargeIntegerT;

# define eo06_LONG_X_LONG_2_LARGE_OS(longVal1, longVal2, calcOK) \
           ((LargeIntegerT) (longVal1) * (LargeIntegerT) (longVal2))
# define eo06_LARGE_MINUS_LONG(large, longVal) ((large) - (LargeIntegerT) (longVal))

# else  /*!!!!! NOT _WIN64 !!!!!*/

#if _MSC_VER > 1200
 LARGE_INTEGER WINAPI LargeIntegerAdd ( LARGE_INTEGER Addend1, LARGE_INTEGER Addend2 );
 LARGE_INTEGER WINAPI ConvertLongToLargeInteger ( LONG SignedInteger );
 LARGE_INTEGER WINAPI EnlargedIntegerMultiply ( LONG Multiplicand, LONG Multiplier );
#else
# include <largeint.h>
#endif
  typedef LARGE_INTEGER LargeIntegerT;

# define eo06_LONG_X_LONG_2_LARGE_OS(longVal1, longVal2, calcOK) \
              EnlargedIntegerMultiply (longVal1, longVal2)
# define eo06_LARGE_MINUS_LONG(large, longVal) \
              LargeIntegerAdd (large, ConvertLongToLargeInteger (-longVal))

# endif

#else

/**\
--------------------------------------- MF__ MOD__ (unix variants)
\**/

#include <assert.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>

typedef int             OSFileHandleT;
#define invalidHandleC   (-1)
#define systemCallErrorC (-1)
#define envVarCharC     '$'
#define eolMarkerSizeC  1

    /* TYPE MF__ MOD__ (lstat_function_eo06) */
typedef int eo06_lstat_function (const char*, struct stat*);
#if defined (S_ISLNK)
#define SYMLINKS_SUPPORTED
static eo06_lstat_function* eo06_lstat = (eo06_lstat_function*) lstat;
    /* FUNCTIONP MF__ MOD__ (eo06_lstat) */
#define eo06_ISLINK_OS(xstat)       S_ISLNK(xstat.st_mode)
    /* MACRO MF__ MOD__ (eo06_ISLINK_OS) */
#else
static eo06_lstat_function* eo06_lstat = (eo06_lstat_function*) stat;
#define eo06_ISLINK_OS(xstat)       FALSE
#endif

static BOOLEAN
eo06_fileOpenUnix (
    const char* fname,
    tsp05_RteFileMode_Param fileMode,
    tsp05_RteBufferingKind_Param buffering,
    FileT* file,
    tsp05_RteFileError *ferr);

static void eo06_fileClose (
    FileT* file,
    tsp05_RteCloseOption_Param option,
    tsp05_RteFileError* err);

#define E06_FILEOPEN_OS(fname, fileMode, buffering, file, ferr) eo06_fileOpenUnix\
    (fname, fileMode, buffering, file, ferr)

#if defined (BIT64) || defined (_IBMR2)

#if defined (_IBMR2)
/* special compiler defined type for 64 bit integer */
typedef long long LargeIntegerT;
#define SPECIAL_LONGSEEK
#else
/* long is 64 bits */
typedef long LargeIntegerT;
#endif

#define eo06_LONG_X_LONG_2_LARGE_OS(longVal1, longVal2, calcOK) \
    ((LargeIntegerT) (longVal1) * (LargeIntegerT) (longVal2))
#define eo06_LARGE_MINUS_LONG(large, longVal) ((large) - (LargeIntegerT) (longVal))

#else

/* no 64 bit integer */
typedef long LargeIntegerT;

static long
eo06_32bit_longXlong2large (long val1, long val2, BOOLEAN* calcOK)
{
#undef MF__
#define MF__ MOD__"eo06_32bit_longXlong2large"
    long result;

    TRCIN;
    result = val1 * val2;
    if (val2 == 0) {
        *calcOK = TRUE;
        TRCOUT_P (("result: %ld\n", 0));
        return 0;
    }
    if ((result / val2) != val1) {
        *calcOK = FALSE;
    }
    else {
        *calcOK = TRUE;
    }
    TRCOUT_P (("result: %ld (%s)\n", result, *calcOK ? "OK" : "ERROR"));
    return result;
}

/*------------------------------*/

#define eo06_LONG_X_LONG_2_LARGE_OS(longVal1, longVal2, calcOK) \
    eo06_32bit_longXlong2large (longVal1, longVal2, calcOK)
#define eo06_LARGE_MINUS_LONG(large, longVal) ((large) - (LargeIntegerT) (longVal))

#endif

#if defined SPECIAL_LONGSEEK

#define eo06_LONGSEEK_UNBUFFERED eo06_longseekUnbuffered
#define eo06_LONGSEEK_BUFFERED_READ eo06_longseekBufferedRead
#define eo06_LONGSEEK_BUFFERED_WRITE eo06_longseekBufferedWrite

#else

/* use ordinary seek */
#define eo06_LONGSEEK_UNBUFFERED eo06_seekUnbuffered
#define eo06_LONGSEEK_BUFFERED_READ eo06_seekBufferedRead
#define eo06_LONGSEEK_BUFFERED_WRITE eo06_seekBufferedWrite

#endif


#endif  /* defined (_WIN32) */

/**\
--------------------------------------- MF__ MOD__ (Memory class)
\**/

#define slotsPerChunkC  8
#define lotsOfMemoryC   1

typedef struct MemChunkT {
    /* TYPE MF__ MOD__ (MemChunkT) */
    void* ptr [slotsPerChunkC];
    char memory [lotsOfMemoryC];
} MemChunkT;

typedef struct ChunkMemT {
    /* TYPE MF__ MOD__ (ChunkMemT) */
    MemChunkT** chunks;
    int bytesPerItem;
    int slotsInUse;
    int maxSlot;
    int chunksInUse;
    int chunksAvailable;
} ChunkMemT;

#define firstValidHandleC       1

#define alignmentC      8
#define chunkGrowRateC  8
#define HANDLE_TO_POINTER_INL(mem,handle) (mem->chunks [handle / slotsPerChunkC]\
    ->ptr [handle % slotsPerChunkC])
    /* MACRO MF__ MOD__ (HANDLE_TO_POINTER_INL) */

static tsp00_Int4 eo06_allocSlot (ChunkMemT* memory, int byteSize);

static BOOLEAN
eo06_initMemory (
    ChunkMemT* memory,
    int bytesPerItem)
{
#undef MF__
#define MF__ MOD__"eo06_initMemory"
    TRCIN;
    memory->chunks = (MemChunkT**) malloc (chunkGrowRateC * sizeof (MemChunkT*));
    if (memory->chunks == NULL) {
        TRCOUT;
        return FALSE;
    }
    if ((bytesPerItem % alignmentC) != 0) {
        bytesPerItem += alignmentC - (bytesPerItem % alignmentC);
    }
    memory->bytesPerItem = bytesPerItem;
    memory->slotsInUse = 0;
    memory->maxSlot = 0;
    memory->chunksInUse = 0;
    memory->chunksAvailable = chunkGrowRateC;
    /* use first slot so 0 is not passed to client */
    eo06_allocSlot (memory, bytesPerItem);
    TRCOUT;
    return TRUE;
}

/*------------------------------*/

static BOOLEAN
eo06_addChunk (
    ChunkMemT* memory,
    int* chunkIndex,
    int* slotIndex)
{
#undef MF__
#define MF__ MOD__"eo06_addChunk"
    int i;
    int bytesPerChunk;
    MemChunkT* chunk;
    MemChunkT** newChunks;

    TRCIN;
    if (memory->chunksInUse == memory->chunksAvailable) {
        /* realloc */
        newChunks = (MemChunkT**) realloc (memory->chunks,
            (memory->chunksAvailable + chunkGrowRateC) * sizeof (MemChunkT*));
        if (newChunks == NULL) {
          TRCOUT;
          return FALSE;
        }
        memory->chunks = newChunks;
        memory->chunksAvailable += chunkGrowRateC;
    }
    *chunkIndex = memory->chunksInUse;
    bytesPerChunk = sizeof (MemChunkT) - lotsOfMemoryC
        + (slotsPerChunkC * memory->bytesPerItem);
    chunk = (MemChunkT*) malloc (bytesPerChunk);
    if (chunk == NULL) {
      TRCOUT;
      return FALSE;
    }
    memset (chunk, '\0', bytesPerChunk);
    for (i = 0; i < slotsPerChunkC; ++i) {
        chunk->ptr [i] = NULL;
    }
    memory->chunks [*chunkIndex] = chunk;
    ++memory->chunksInUse;
    memory->maxSlot += slotsPerChunkC;
    *slotIndex = 0;
    TRCOUT;
    return TRUE;
}

/*------------------------------*/

static tsp00_Int4
eo06_allocSlot (
    ChunkMemT* memory,
    int byteSize)
{
#undef MF__
#define MF__ MOD__"eo06_allocSlot"
    tsp00_Int4 result = UNDEF;
    MemChunkT* chunk;
    int chunkIndex;
    int slotIndex;
    int found = FALSE;

    TRCIN;
    if (memory->chunks == NULL) {
        if (!eo06_initMemory (memory, byteSize)) {
            TRCOUT;
            return UNDEF;
        }
    }
    if (memory->slotsInUse == memory->maxSlot) {
        if (!eo06_addChunk (memory, &chunkIndex, &slotIndex)) {
            TRCOUT;
            return UNDEF;
        }
    }
    else {
        /* find free slot */
        for(chunkIndex = 0; (chunkIndex < memory->chunksInUse) && !found; ++chunkIndex) {
            chunk = memory->chunks [chunkIndex];
            for(slotIndex = 0; (slotIndex < slotsPerChunkC) && !found; ++slotIndex) {
                if (chunk->ptr [slotIndex] == NULL) {
                    found = TRUE;
                }
            }
        }
        /* undo last loop increment */
        --chunkIndex;
        --slotIndex;
    }
    chunk = memory->chunks [chunkIndex];
    chunk->ptr [slotIndex] = chunk->memory
        + (slotIndex * memory->bytesPerItem);
    ++memory->slotsInUse;
    result = (chunkIndex * slotsPerChunkC) + slotIndex;
    TRCOUT;
    return result;
}

/*------------------------------*/

static FileT*
eo06_getPointer (
    ChunkMemT* memory,
    tsp00_Int4 handle)
{
#undef MF__
#define MF__ MOD__"eo06_getPointer"
    if ((firstValidHandleC <= handle) && (handle < memory->maxSlot)) {
        return (FileT*)HANDLE_TO_POINTER_INL (memory, handle);
    }
    else {
        return NULL;
    }
}

/*------------------------------*/

static void
eo06_freeSlot (
    ChunkMemT* memory,
    tsp00_Int4 handle)
{
#undef MF__
#define MF__ MOD__"eo06_freeSlot"
    TRCPAS_P (("handle %d\n", handle));;
    if ((firstValidHandleC <= handle) && (handle < memory->maxSlot)) {
        HANDLE_TO_POINTER_INL (memory, handle) = NULL;
        --memory->slotsInUse;
    }
}

/*------------------------------*/

static BOOLEAN
eo06_nextUsedHandle (
    ChunkMemT* memory,
    tsp00_Int4* handle)
{
#undef MF__
#define MF__ MOD__"eo06_nextUsedHandle"
    int chunkIndex;
    int slotIndex;
    MemChunkT* chunk;
    int found = FALSE;

    TRCIN;
    if (*handle < firstValidHandleC) {
        *handle = firstValidHandleC;
    }
    chunkIndex = *handle / slotsPerChunkC;
    slotIndex = *handle % slotsPerChunkC;
    /* walk current chunk */
    chunk = memory->chunks [chunkIndex];
    for (; (slotIndex < slotsPerChunkC) && !found; ++slotIndex) {
        if (chunk->ptr [slotIndex] != NULL) {
            found = TRUE;
        }
    }
    /* walk remaining chunks */
    if (!found) {
        for (; (chunkIndex < memory->chunksInUse) && !found; ++chunkIndex) {
            for (slotIndex = 0; (slotIndex < slotsPerChunkC) && !found; ++slotIndex) {
                if (chunk->ptr [slotIndex] != NULL) {
                    found = TRUE;
                }
            }
        }
    }
    if (found) {
        *handle = (chunkIndex / slotsPerChunkC) + slotIndex;
    }
    else {
        *handle = UNDEF;
    }
    TRCOUT;
    return (BOOLEAN) found;
}

/**\
--------------------------------------- MF__ MOD__ (common)
\**/

static void
eo06_putInt (
    tsp00_Byte* buf,
    int value,
    int digits)
{
#undef MF__
#define MF__ MOD__"eo06_putInt"
    for (; digits > 0; --digits) {
        buf [digits-1] = (char)((value % 10) + '0');
        value /= 10;
    }
}

/*------------------------------*/

static int
eo06_trimLen (
    const tsp00_Byte* str,
    int len)
{
#undef MF__
#define MF__ MOD__"eo06_trimLen"
    while (len > 0) {
        if (str [len - 1] != ' ') {
            return len;
        }
    --len;
    }
    return 0;
}

/*------------------------------*/

static void
eo06_padLen (
    tsp00_Byte * str,
    int bufLen)
{
    int len;

    len = (int)strlen ((char*)str);
    if (len < bufLen) {
        memset (str + len, ' ', bufLen - len);
    }
}

/*------------------------------*/

static const char*
eo06_cookName (
    char*          nameBuf,
    unsigned int   bufSize,
    const char*    rawFName,
    int            len )
{
#undef MF__
#define MF__ MOD__"eo06_cookName"
    BOOLEAN nullTerminated;
    const char* result;

    /*TRCIN;*/

    if (len == UNDEF) {
        len = (int)strlen ((char*)rawFName);
        nullTerminated = TRUE;
    }
    else {
        nullTerminated = (BOOLEAN)(rawFName [len] == '\0');
    }
    if (memchr (rawFName, envVarCharC, len) != NULL) 
    {
        /* We don't check the return code here, on error the output buffer becomes the 
           input string. */
        RTE_ExpandEnvVars ( rawFName, nameBuf, &bufSize );
        TRCPAS_P (("from %s to %s\n", rawFName, nameBuf));
        result = nameBuf;
    }
    else {
        if (nullTerminated) {
            result = rawFName;
        }
        else {
            /* make a null terminated copy */
            SAPDB_memcpy (nameBuf, rawFName, len);
            nameBuf [len] = '\0';
            result = nameBuf;
        }
    }
    /*TRCOUT;*/
    return result;
}


/*------------------------------*/

static void
eo06_lockError (
    tsp05_RteFileError *ferr)
{
#undef MF__
#define MF__ MOD__"eo06_lockError"
    ferr->sp5fe_result = VF_NOTOK;
    strcpy ((char *)ferr->sp5fe_text, File_Not_Lockable_ErrText);
    TRCPAS_P (("VF_NOTOK: '%s'\n",File_Not_Lockable_ErrText ));
}

/*------------------------------*/


static void
eo06_seekError (
    tsp05_RteFileError *ferr)
{
#undef MF__
#define MF__ MOD__"eo06_seekError"
    /* PTS 1114049 */
    ferr->sp5fe_result = VF_NOSEEK;
    strcpy ((char *)ferr->sp5fe_text, No_Seek_ErrText);
    TRCPAS_P (("VF_NOSEEK: '%s'\n", (char *)ferr->sp5fe_text));
}

/*------------------------------*/

static void
eo06_paramError (
    tsp05_RteFileError *ferr,
    const char * paramName)
{
#undef MF__
#define MF__ MOD__"eo06_paramError"
    ferr->sp5fe_result = VF_NOTOK;
    strcpy ((char *)ferr->sp5fe_text, Invalid_Parameter_ErrText);
    strcat ((char *)ferr->sp5fe_text, paramName);
    TRCPAS_P (("VF_NOTOK: '%s'\n", (char *)ferr->sp5fe_text));
}

/*------------------------------*/

static void
eo06_pagesizeError (
    tsp05_RteFileError *ferr,
    int pageSizeGiven,
    int pageSizeRead)
{
#undef MF__
#define MF__ MOD__"eo06_pagesizeError"
    tsp00_C40c msg;
    ferr->sp5fe_result = vf_notok;
    sp77sprintf(msg,sizeof(tsp00_C40), (char*)Pagesize_Not_OK_ErrText,
             pageSizeGiven, pageSizeRead);
    eo46CtoP ( ferr->sp5fe_text, msg, sizeof(tsp00_C40) );
    TRCPAS_P (("VF_NOTOK: '%s'\n", (char *)ferr->sp5fe_text));
}

/*------------------------------*/

static void
eo06_encodingError (
    tsp05_RteFileError *ferr,
    const tsp77encoding * givenEncoding)
{
#undef MF__
#define MF__ MOD__"eo06_encodingError"
    ferr->sp5fe_result = vf_notok;
    if (givenEncoding == NULL) {
        eo46CtoP ( ferr->sp5fe_text, (char*) Missing_Encoding_ErrText, sizeof(tsp00_C40) );
    }
    else {
        tsp00_C40c msg;
        sp77sprintf(msg,sizeof(tsp00_C40),(char*)Wrong_Encoding_ErrText,
                 givenEncoding->whichEncodingClass);
        eo46CtoP ( ferr->sp5fe_text, msg, sizeof(tsp00_C40) );
    }
    TRCPAS_P (("VF_NOTOK: '%s'\n", (char *)ferr->sp5fe_text));
}

/*------------------------------*/

static void
eo06_generalEncodingError (
    tsp05_RteFileError *ferr,
    tsp78ConversionResult convResult)
{
#undef MF__
#define MF__ MOD__"eo06_generalEncodingError"
    tsp00_C40c msg;
    ferr->sp5fe_result = vf_notok;
    sp77sprintf(msg,sizeof(tsp00_C40),(char*) General_Encoding_ErrText,
        sp78errString (convResult));
    eo46CtoP ( ferr->sp5fe_text, msg, sizeof(tsp00_C40) );
    TRCPAS_P (("VF_NOTOK: '%s'\n", (char *)ferr->sp5fe_text));
}

/*------------------------------*/

/**\
--------------------------------------- MF__ MOD__ (File class)
\**/

static ChunkMemT allFilesV = {NULL, 0, 0, 0, 0, 0};

typedef tsp00_Longint eo06_readFT (
    FileT* file,
    byte* buf,
    tsp00_Longint bufLen,
    tsp05_RteFileError* err,
    BOOLEAN forC);

typedef tsp00_Longint eo06_writeFT (
    FileT* file,
    const byte* buf,
    tsp00_Longint bufLen,
    tsp05_RteFileError* err);

typedef void eo06_flushFT (
    FileT* file,
    tsp05_RteFileError* err);

typedef void eo06_seekFT (
    FileT* file,
    tsp00_Longint distance,
    tsp05_RteSeekKind_Param whence,
    tsp05_RteFileError* err);

typedef void eo06_longseekFT (
    FileT* file,
    LargeIntegerT distance,
    tsp05_RteSeekKind_Param whence,
    tsp05_RteFileError* err);

typedef void eo06_closeFT (
    FileT* file,
    tsp05_RteCloseOption_Param option,
    tsp05_RteFileError* err);

typedef struct FileClassT {
    /* TYPE MF__ MOD__ (FileClassT) */
    const char*         className;
    eo06_readFT*        lowLevelReadFunction;
    eo06_readFT*        readFunction;
    eo06_writeFT*       writeFunction;
    eo06_flushFT*       flushFunction;
    eo06_seekFT*        seekFunction;
    eo06_longseekFT*    longseekFunction;
    eo06_closeFT*       closeFunction;
} FileClassT;

typedef struct CacheT {
    /* TYPE MF__ MOD__ (CacheT) */
    byte         * membase;      /* memory to be freed                    */
    byte         * mem;          /*                                       */
    tsp00_Longint  current;      /* index of the current character in mem */
    tsp00_Longint  extent;       /* number of characters loaded into mem  */
    tsp00_Longint  memSize;      /* size of mem                           */
} CacheT;

#define CACHEPOINTER(cache) ((cache)->mem + (cache)->current)
#define READSLACK(cache)    ((cache)->extent - (cache)->current)
#define WRITESLACK(cache)   ((cache)->memSize - (cache)->current)

struct FileT {
    /* TYPE MF__ MOD__ (FileT) */
    FileClassT *        classDesc;
    OSFileHandleT       osHandle;
    CacheT              cache;
    tsp00_Longint       filePos;
    const tsp77encoding * fileEncoding;
    const tsp77encoding * clientEncoding;
    boolean             isStdHandle;
    int                 eolMarkerSize;
#if defined (_WIN32)
    LONG                lDevType;
    ULONG               ulTapeFeatures;
    ULONG               ulMaxTapeBlkSize;
    BOOLEAN             fECC;
    BOOLEAN             fCompression;
    BOOLEAN             fDataPadding;
#else
    tsp00_Bool          seekBySkip;
#endif
};

#define eo06_ISWRITER_V(file)   (file->classDesc->writeFunction != eo06_writeReadStream)
#define eo06_LOW_LEVEL_READ_V(file, buf, len, err, forC) (file->classDesc->lowLevelReadFunction \
    (file, buf, len, err, forC))
#define eo06_READ_V(file, buf, len, err, forC) (file->classDesc->readFunction \
    (file, buf, len, err, forC))
#define eo06_WRITE_V(file, buf, len, err) (file->classDesc->writeFunction \
    (file, buf, len, err))
#define eo06_FLUSH_V(file, err) (file->classDesc->flushFunction (file, err))
#define eo06_SEEK_V(file, distance, whence, err) (file->classDesc->seekFunction \
    (file, distance, whence, err))
#define eo06_LONGSEEK_V(file, distance, whence, err) (file->classDesc->longseekFunction \
    (file, distance, whence, err))
#define eo06_CLOSE_V(file, options, err) (file->classDesc->closeFunction \
    (file, options, err))

static tsp00_Longint /*ARGSUSED*/
eo06_readWriteStream (
    FileT* file,
    byte* buf,
    tsp00_Longint bufLen,
    tsp05_RteFileError* err,
    BOOLEAN forC)
{
#undef MF__
#define MF__ MOD__"eo06_readWriteStream"
    TRCIN;
    USEVAR_DBG (file);
    USEVAR_DBG (buf);
    USEVAR_DBG (bufLen);
    USEVAR_DBG (forC);
    eo06_ferror (err, File_Open_For_Write_ErrText);
    TRCOUT;
    return UNDEF;
}

/*------------------------------*/

static tsp00_Longint
eo06_writeReadStream ( /*ARGSUSED*/
    FileT* file,
    const byte* buf,
    tsp00_Longint bufLen,
    tsp05_RteFileError* err)
{
#undef MF__
#define MF__ MOD__"eo06_writeReadStream"
    TRCIN;
    USEVAR_DBG (file);
    USEVAR_DBG (buf);
    USEVAR_DBG (bufLen);
    eo06_ferror (err, File_Open_For_Read_ErrText);
    TRCOUT;
    return UNDEF;
}

/*------------------------------*/

#if defined (_WIN32)

static tsp00_Longint /*ARGSUSED*/
eo06_readUnbufferedBinary (
    FileT* file,
    byte* buf,
    tsp00_Longint bufLen,
    tsp05_RteFileError* err,
    BOOLEAN forC)
{
#undef MF__
#define MF__ MOD__"eo06_readUnbufferedBinary"   /* WIN32 */
    LONG        rc;
    ULONG       bytesRead;

    TRCIN;
    USEVAR_DBG (forC);
    rc = sql06_io_read ( (HANDLE)file->osHandle, file->lDevType,
                          buf, (long)bufLen, &bytesRead );
    switch (rc) {
    case NO_ERROR:
    case ERROR_HANDLE_EOF:
    case ERROR_NO_DATA_DETECTED:
    case ERROR_FILEMARK_DETECTED:
    case ERROR_END_OF_MEDIA:
    case ERROR_PIPE_NOT_CONNECTED:
    case ERROR_BROKEN_PIPE:
        if (bytesRead == 0) {
            eo06_eofError (err);
        }
        else if (eo06_VALIDPOS (file)) {
            file->filePos += bytesRead;
        }
        break;
    default:
        bytesRead = 0;
        eo06_osError (err);
        break;
    }
    TRCOUT;
    return (tsp00_Longint)bytesRead;
}

/*------------------------------*/

static tsp00_Longint /*ARGSUSED*/
eo06_readConsoleEncoded (
    FileT* file,
    byte* buf,
    tsp00_Longint bufLen,
    tsp05_RteFileError* err,
    BOOLEAN forC)
{
#undef MF__
#define MF__ MOD__"eo06_readConsoleEncoded"   /* WIN32 */
    LONG        rc;
    ULONG       charsRead;
    ULONG       bytesRead;
    const tsp77charConst * dosEOF;

    TRCIN;
    USEVAR_DBG (forC);
    if (ReadConsoleW (file->osHandle, buf, (DWORD) (bufLen / 2), &charsRead, NULL)) {
        rc = NO_ERROR;
        bytesRead = charsRead * 2;
        dosEOF = &sp77encodingUCS2Native->charTable->dosEOF;
        if (sp77charIsEqual (dosEOF, buf, bytesRead)) {
            rc = ERROR_HANDLE_EOF;
        }
    }
    else {
        rc = GetLastError ();
    }
    switch (rc) {
    case NO_ERROR:
    case ERROR_HANDLE_EOF:
    case ERROR_NO_DATA_DETECTED:
    case ERROR_FILEMARK_DETECTED:
    case ERROR_END_OF_MEDIA:
    case ERROR_PIPE_NOT_CONNECTED:
    case ERROR_BROKEN_PIPE:
        if (bytesRead == 0) {
        eo06_eofError (err);
        }
        if (eo06_VALIDPOS (file)) {
            file->filePos += bytesRead;
        }
        break;
    default:
        bytesRead = 0;
        eo06_osError (err);
        break;
    }
    TRCOUT;
    return (tsp00_Longint)bytesRead;
}

/*------------------------------*/

static tsp00_Longint
eo06_writeUnbufferedBinary (
    FileT* file,
    const byte* buf,
    tsp00_Longint bufLen,
    tsp05_RteFileError* err)
{
#undef MF__
#define MF__ MOD__"eo06_writeUnbufferedBinary"  /* WIN32 */
    LONG        rc;
    ULONG       bytesWritten;

    TRCIN;
    if (bufLen > 0) {
        rc = sql06_io_write ((HANDLE)file->osHandle, DT_UNKNOWN,
            (void*)buf, (long)bufLen, &bytesWritten);
        if ((rc != NO_ERROR) || ((tsp00_Longint)bytesWritten != bufLen)) {
            eo06_osError (err);
        }
    }
    else
    {
        bytesWritten = 0;
    }
    TRCOUT;
    return (tsp00_Longint) bytesWritten;
}

/*------------------------------*/

#else
static tsp00_Longint /*ARGSUSED*/
eo06_readUnbufferedBinary (
    FileT* file,
    byte* buf,
    tsp00_Longint bufLen,
    tsp05_RteFileError* err,
    BOOLEAN forC)
{
#undef MF__
#define MF__ MOD__"eo06_readUnbufferedBinary"   /* UNIX */
    tsp00_Longint bytesRead;

    TRCIN;
    USEVAR_DBG (forC);
    bytesRead = read (file->osHandle, buf, (unsigned) bufLen);
    switch (bytesRead) {
        case systemCallErrorC:
            eo06_osError (err);
            bytesRead = 0;
            break;
        case 0:
            eo06_eofError (err);
            break;
        default:
            /* OK */
            if (eo06_VALIDPOS (file)) {
                file->filePos += bytesRead;
            }
            break;
    }
    TRCOUT;
    return bytesRead;
}

/*------------------------------*/

static tsp00_Longint
eo06_writeUnbufferedBinary (
    FileT* file,
    const byte* buf,
    tsp00_Longint bufLen,
    tsp05_RteFileError* err)
{
#undef MF__
#define MF__ MOD__"eo06_writeUnbufferedBinary"  /* UNIX */
    int bytesWritten = 0;

    TRCIN;
    if (bufLen > 0) {
        bytesWritten = write (file->osHandle, (char*) buf, bufLen);
        if (bytesWritten != bufLen) {
            eo06_osError (err);
            bytesWritten = 0;
        }
        else {
            if (eo06_VALIDPOS (file)) {
                file->filePos += bytesWritten;
            }
        }
    }
    TRCOUT;
    return bytesWritten;
}

/*------------------------------*/

#endif

static tsp00_Longint
eo06_readBufferedBinary (
    FileT* file,
    byte* buf,
    tsp00_Longint bufLen,
    tsp05_RteFileError* err,
    BOOLEAN forC)
{
#undef MF__
#define MF__ MOD__"eo06_readBufferedBinary"
    CacheT*             cache = &file->cache;
    tsp00_Longint         bytesRead = 0;
    tsp00_Longint         partRead;
    BOOLEAN             atEnd = FALSE;
    tsp00_Longint         oldPos = file->filePos;

    TRCIN;
    /* cache -> client */
    partRead = min (bufLen, cache->extent - cache->current);
    if (partRead > 0) {
        SAPDB_memcpy (buf, cache->mem + cache->current, partRead);
        cache->current += partRead;
        buf += partRead;
        bytesRead += partRead;
        bufLen -= partRead;
    }

    /* file -> client */
    if (bufLen > cache->memSize) {
        partRead = eo06_LOW_LEVEL_READ_V (file, buf,
            bufLen, err, forC);
        buf += partRead;
        bytesRead += partRead; /* PTS 1109326 */
        bufLen = 0; /* partRead < bufLen: at end of file */
        if (err->sp5fe_result == VF_EOF) {
            atEnd = TRUE;
        }
    }

    /* file -> cache */
    if ((bufLen > 0) && !atEnd) {
        cache->current = 0;
        cache->extent = eo06_LOW_LEVEL_READ_V (file, cache->mem,
            cache->memSize, err, forC);
        if (cache->extent > 0) {
            /* cache -> client */
            partRead = min (bufLen, cache->extent - cache->current);
            SAPDB_memcpy (buf, cache->mem + cache->current, partRead);
            cache->current += partRead;
            bytesRead += partRead;
        }
    }
    if ((err->sp5fe_result == VF_EOF) && (bytesRead > 0)) {
        eo06_clearError (err);
    }
    if (oldPos >= 0) {
        file->filePos = oldPos + bytesRead;
    }
    TRCOUT;
    return bytesRead;
}

/*------------------------------*/

static tsp00_Longint
eo06_writeBufferedBinary (
    FileT* file,
    const byte* buf,
    tsp00_Longint bufLen,
    tsp05_RteFileError* err)
{
#undef MF__
#define MF__ MOD__"eo06_writeBufferedBinary"
    CacheT*             cache = &file->cache;
    tsp00_Longint         bytesWritten;
    tsp00_Longint         oldPos = file->filePos;

    TRCIN;
    if (bufLen <= min (bufLen, cache->memSize - cache->current)) {
        /* client -> cache */
        SAPDB_memcpy (cache->mem + cache->current, buf, bufLen);
        cache->current += bufLen;
        bytesWritten = bufLen;
    }
    else {
        /* cache -> file */
        eo06_writeUnbufferedBinary (file, cache->mem, cache->current, err);
        if (err->sp5fe_result == VF_OK) {
            cache->current = 0;
            /* client -> file */
            bytesWritten = eo06_writeUnbufferedBinary (file, buf, bufLen, err);
        }
    }
    if (oldPos >= 0) {
        file->filePos = oldPos + bytesWritten;
    }
    TRCOUT;
    return bytesWritten;
}

/*------------------------------*/

static tsp00_Longint
eo06_writeUnbufferedText (
    FileT* file,
    const byte* buf,
    tsp00_Longint bufLen,
    tsp05_RteFileError* err)
{
#undef MF__
#define MF__ MOD__"eo06_writeUnbufferedText"
    tsp00_Longint bytesWritten;

    TRCIN;
    bytesWritten = eo06_writeBufferedText (file, buf, bufLen, err);
    if (err->sp5fe_result == VF_OK) {
        eo06_FLUSH_V (file, err);
    }
    TRCOUT;
    return bytesWritten;
}

/*------------------------------*/

static tsp00_Longint
eo06_readBufferedText (
    FileT* file,
    byte* buf,
    tsp00_Longint bufLen,
    tsp05_RteFileError* err,
    BOOLEAN forC)
{
#undef MF__
#define MF__ MOD__"eo06_readBufferedText"
    CacheT*             cache = &file->cache;
    BOOLEAN             finished = FALSE;
    BOOLEAN             eof = FALSE;
    tsp00_Longint       bytesRead = 0;
    tsp00_Longint       partRead;
    byte*               posNL = 0;
    tsp00_Longint       oldPos = file->filePos;

    TRCIN;
    if (forC) {
        --bufLen;
    }
    while (!finished) {
        if ((cache->extent - cache->current) > 0) {
            /* find nl */
            posNL = (byte*)memchr (cache->mem + cache->current, '\n',
                cache->extent - cache->current);
            if (posNL == NULL) {
                partRead = cache->extent - cache->current;
            }
            else {
                partRead = posNL - cache->mem - cache->current;
                finished = TRUE;
            }

            /* copy to buf */
            if (partRead > bufLen) {
                partRead = bufLen;
                finished = TRUE;
                posNL = NULL;
            }
            SAPDB_memcpy (buf, cache->mem + cache->current, partRead);
            cache->current += partRead;
            if (posNL != NULL) {
                ++cache->current;
            }
            buf += partRead;
            bytesRead += partRead;
            bufLen -= partRead;
        }
        if (eof) {
            finished = TRUE;
        }
        if (!finished) {
            /* read more data */
            cache->current = 0;
            cache->extent = eo06_LOW_LEVEL_READ_V (file, cache->mem,
                cache->memSize, err, forC);
            switch (err->sp5fe_result) {
                case VF_OK:
                    break;
                case VF_EOF:
                    eof = TRUE;
                    if (cache->extent == 0) {
                        finished = TRUE;
                    }
                    break;
                default:
                    finished = TRUE;
                    break;
            }
        }
    }
    if ((posNL == NULL) && (err->sp5fe_result != VF_EOF)) {
        err->sp5fe_warning = sp5vfw_no_eol_found;
    }
    if ((err->sp5fe_result == VF_EOF) && (bytesRead > 0)) {
        err->sp5fe_result = VF_OK;
    }
    if (err->sp5fe_result == VF_OK) {
        if (oldPos >= 0) {
            file->filePos = oldPos + bytesRead + 1; /* + 1: for '\n' */
        }
        /*#if defined (_WIN32), allow DOS files under UNIX*/
        if ((bytesRead > 0) && (buf [-1] == '\r')) {
            /* get rid of CR/LF combination */
            --bytesRead;
            --buf;
        }
        if (forC) {
            buf[0] = '\0';
        }
        else {
            buf [0] = ' ';
        }
    }
    TRCOUT;
    return bytesRead;
}

/*------------------------------*/

static tsp00_Longint
eo06_writeBufferedText (
    FileT* file,
    const byte* buf,
    tsp00_Longint bufLen,
    tsp05_RteFileError* err)
{
#undef MF__
#define MF__ MOD__"eo06_writeBufferedText"
    tsp00_Longint         bytesWritten;
    CacheT*             cache = &file->cache;
    tsp00_Longint         oldPos = file->filePos;

    TRCIN;
    if (bufLen < 0) {
        bufLen = strlen ((char*)buf);
    }
    bytesWritten = eo06_writeBufferedBinary (file, buf, bufLen, err);
    if (err->sp5fe_result == VF_OK) {
        if ((cache->memSize - cache->current) < eolMarkerSizeC) {
            eo06_FLUSH_V (file, err);
        }
        if (err->sp5fe_result == VF_OK) {
            if (oldPos >= 0) {
                file->filePos = oldPos + bytesWritten + 1;
            }
#if defined (_WIN32)
            cache->mem [cache->current] = '\r';
            ++cache->current;
            if (eo06_VALIDPOS (file)) {
                ++file->filePos;
            }
#endif
            cache->mem [cache->current] = '\n';
            ++cache->current;
        }
    }
    TRCOUT;
    return bytesWritten;
}

/*------------------------------*/

static tsp00_Longint
eo06_readBufferedEncoded (
    FileT* file,
    byte* buf,
    tsp00_Longint bufLen,
    tsp05_RteFileError* err,
    BOOLEAN forC)
{
#undef MF__
#define MF__ MOD__"eo06_readBufferedEncoded"
    CacheT*                cache = &file->cache;
    BOOLEAN                finished          = FALSE;
    tsp00_Longint          bytesRead         = 0;
    tsp00_Longint          bytesReadFromFile = 0;
    tsp00_Uint4            bytesRequested;
    const byte*            posNL;
    tsp00_Longint          oldPos            = file->filePos;
    tsp78ConversionResult  convResult;
    tsp00_Uint4            chunkWritten;
    tsp00_Uint4            chunkRead;
    const tsp77charConst * terminator = &file->clientEncoding->charTable->terminator;
    const tsp77charConst * nl = &file->fileEncoding->charTable->nl;
    const tsp77charConst * cr = &file->fileEncoding->charTable->cr;
    int                    eolSkip;
    const byte           * posCR;

    TRCIN;
    if (forC) {
        bufLen -= terminator->byteCount;
    }
    while (!finished) {
        eolSkip = 0;
        posNL   = eo06_encchr (CACHEPOINTER (cache), nl, (int) READSLACK (cache));
        if (posNL == NULL)
        {
            /* No nl found in cached data line; we'll process the complete remainder found in cache */
            bytesRequested = (tsp00_Uint4) READSLACK (cache);
        }
        else
        {
            /* nl found; get all data before nl for further processing */
            bytesRequested = (tsp00_Uint4) (posNL - CACHEPOINTER (cache));
            eolSkip        = nl->byteCount;

            if (0 != bytesRequested)
            {
                /* Check if the Byte(s) before nl represent cr, and if so remove them */
                posCR = posNL - cr->byteCount;
                if (sp77charIsEqual (cr, posCR, cr->byteCount)) {
                    bytesRequested -= cr->byteCount;
                    eolSkip        += cr->byteCount;
                }
            }
            else
            {
                /* nl found at pos 0 in cache -> in the preceding run the cache was exhausted exactly after an cr;
                                                 thus everything before and including cr has already been converted
                                                 into buf -> we have to modify buf to remove cr (if existing) */

                if (0 != bytesRead) /* make sure there is something in buf */
                {
                    const tsp77charConst* Client_cr = &file->clientEncoding->charTable->cr;
                    posCR = buf - Client_cr->byteCount;

                    /* Check if last bytes in already written/converted buffer
                       represent carriage return and if so remove them */
                    if (sp77charIsEqual (Client_cr, posCR, Client_cr->byteCount))
                    {
                        bytesRead -= Client_cr->byteCount;
                        buf       -= Client_cr->byteCount;
                    }
                }
            }
        }   /* end else of if (posNL == NULL)*/

        /*
         * Convert read data to client encoding
         */
        convResult = sp78convertString (
            file->clientEncoding, buf, (tsp00_Uint4) bufLen, &chunkWritten, false,
            file->fileEncoding, CACHEPOINTER (cache), bytesRequested, &chunkRead);

        switch (convResult) {
            case sp78_SourceExhausted:  /* last character only partial in source buffer */
            {
                /*
                 *  Source can be exhausted only if no [cr]nl is found;
                 *  copy last not converted characters to beginning of cache buffer and
                 *  read next characters from file
                 */
                tsp00_Uint4 _lCharCnt = (bytesRequested - chunkRead);

                bytesRead         += chunkWritten;
                buf               += chunkWritten;
                cache->current    += chunkRead;
                bytesReadFromFile += chunkRead;

                SAPDB_memcpy (cache->mem, CACHEPOINTER(cache), _lCharCnt);
                cache->current = _lCharCnt;
                break;
            }
            case sp78_Ok:
            case sp78_TargetExhausted:
                bytesRead         += chunkWritten;
                buf               += chunkWritten;
                cache->current    += chunkRead;
                bytesReadFromFile += chunkRead;

                if ((posNL != 0) && (chunkRead == bytesRequested)) {
                    /* read until [cr] nl */
                    finished = TRUE;
                    cache->current    += eolSkip;
                    bytesReadFromFile += eolSkip;
                }
                else
                {
                    cache->current = 0;
                }
                break;
            case sp78_SourceCorrupted:
            case sp78_TargetNotTerminated:
                /* can't happen as bufLen is decremented */
            case sp78_BadSourceEncoding:
            case sp78_BadTargetEncoding:
            case sp78_NullArgument:
                eo06_generalEncodingError (err, convResult);
                finished = TRUE;
                break;
        }

        /*
         * read more data
         */
        if (!finished)
        {
            /*
             * We have to deal with convResult = sp78_SourceExhausted here, too;
             * cache->current is set to right value for finished == false;
             * we have to reinitialize it after reading, though
             */
            cache->extent = cache->current + eo06_LOW_LEVEL_READ_V (file,
                                                                    cache->mem + cache->current,
                                                                    cache->memSize - cache->current,
                                                                    err, forC);
            cache->current = 0;

            if (err->sp5fe_result != VF_OK) {
                finished = TRUE;
            }
        }
    }   /* end while (!finished)  */

    if ((posNL == NULL) && (err->sp5fe_result != VF_EOF)) {
        err->sp5fe_warning = sp5vfw_no_eol_found;
    }
    if ((err->sp5fe_result == VF_EOF) && (bytesRead > 0)) {
        err->sp5fe_result = VF_OK;
    }
    if (err->sp5fe_result == VF_OK) {
        if (oldPos >= 0)
        {
            file->filePos = oldPos + bytesReadFromFile;
        }
        if (forC) {
            sp77charMove (buf, terminator->byteCount, terminator);
        }
    }
    TRCOUT;
    return bytesRead;
}

/*------------------------------*/

static tsp00_Longint
eo06_writeBufferedEncoded (
    FileT* file,
    const byte* buf,
    tsp00_Longint bufLen,
    tsp05_RteFileError* err)
{
#undef MF__
#define MF__ MOD__"eo06_writeBufferedEncoded"
    tsp00_Longint         bytesWritten = 0;
    CacheT*             cache = &file->cache;
    tsp00_Longint         oldPos = file->filePos;
    tsp78ConversionResult convResult;
    tsp00_Uint4         chunkWritten;
    tsp00_Uint4         chunkRead;

    TRCIN;
    if (bufLen < 0) {
        bufLen = eo06_enclen (file->clientEncoding, buf);
    }
    while ((bufLen > 0) && (err->sp5fe_result == VF_OK)) {
        /* copy into cache */
        convResult = sp78convertString (
            file->fileEncoding, CACHEPOINTER (cache), (tsp00_Uint4) WRITESLACK (cache), &chunkWritten, FALSE,
            file->clientEncoding, buf, (tsp00_Uint4) bufLen, &chunkRead);
        switch (convResult) {
            case sp78_Ok:
            case sp78_TargetExhausted:
                buf += chunkRead;
                bufLen -= chunkRead;
                cache->current += chunkWritten;
                bytesWritten += chunkWritten;
                if (convResult == sp78_TargetExhausted) {
                    eo06_FLUSH_V (file, err);
                }
                break;
            case sp78_SourceExhausted: /* buffer incomplete and no refill possible... Prevent endless loop.... */
            case sp78_SourceCorrupted:
            case sp78_TargetNotTerminated:
            case sp78_BadSourceEncoding:
            case sp78_BadTargetEncoding:
            case sp78_NullArgument:
                eo06_generalEncodingError (err, convResult);
                break;
        }
    }
    if (err->sp5fe_result == VF_OK) {
        if (WRITESLACK (cache) < file->eolMarkerSize) {
            eo06_FLUSH_V (file, err);
        }
        if (err->sp5fe_result == VF_OK) {
            /*
             * add end of line marker
             */
            const tsp77charTable * chars = file->fileEncoding->charTable;
#if defined (_WIN32)
            cache->current += sp77charMove (
                CACHEPOINTER (cache), (int) WRITESLACK (cache), &chars->cr);
#endif
            cache->current += sp77charMove (
                CACHEPOINTER (cache), (int) WRITESLACK (cache), &chars->nl);
            file->filePos = oldPos + bytesWritten + file->eolMarkerSize;
        }
    }
    TRCOUT;
    return bytesWritten;
}

/*------------------------------*/

static tsp00_Longint
eo06_writeUnbufferedEncoded (
    FileT* file,
    const byte* buf,
    tsp00_Longint bufLen,
    tsp05_RteFileError* err)
{
#undef MF__
#define MF__ MOD__"eo06_writeUnbufferedEncoded"
    tsp00_Longint bytesWritten;

    TRCIN;
    bytesWritten = eo06_writeBufferedEncoded (file, buf, bufLen, err);
    if (err->sp5fe_result == VF_OK) {
        eo06_FLUSH_V (file, err);
    }
    TRCOUT;
    return bytesWritten;
}

/*------------------------------*/

static void /*ARGSUSED*/
eo06_flushDummy (
    FileT* file,
    tsp05_RteFileError* err)
{
#undef MF__
#define MF__ MOD__"eo06_flushDummy"
    USEVAR_DBG (file);
    USEVAR_DBG (err);
}

/*------------------------------*/

static void
eo06_flushBuffer (
    FileT* file,
    tsp05_RteFileError* err)
{
#undef MF__
#define MF__ MOD__"eo06_flushBuffer"
    CacheT* cache = &file->cache;

    eo06_writeUnbufferedBinary (file, cache->mem, cache->current, err);
    cache->current = 0;
}

/*------------------------------*/

#if defined (_WIN32)

static void
eo06_flushConsoleBuffer (
    FileT* file,
    tsp05_RteFileError* err)
{
#undef MF__
#define MF__ MOD__"eo06_flushConsoleBuffer"
    CacheT* cache = &file->cache;
    DWORD   charsWritten;
    BOOL    ok;

    ok = WriteConsoleW (file->osHandle, cache->mem, (DWORD) (cache->current / 2),
            &charsWritten, NULL);
    if (!ok) {
        eo06_osError (err);
    }
    cache->current = 0;
}
#endif

/*------------------------------*/

#if defined (_WIN32)

static void
eo06_seekUnbuffered (
    FileT* file,
    tsp00_Longint distance,
    tsp05_RteSeekKind_Param whence,
    tsp05_RteFileError* err)
{
#undef MF__
#define MF__ MOD__"eo06_seekUnbuffered"  /* WIN32 */
    DWORD seekResult;
    DWORD method;
    static DWORD whence2method[] = {FILE_BEGIN, FILE_CURRENT, FILE_END};
    DWORD fileSize;
#if defined (_WIN64)
     DWORD HighPart;
# endif

    TRCIN;
    method = whence2method [whence];
#if defined (_WIN64)
    HighPart = (LONG)(distance >> 32);
    seekResult = SetFilePointer (file->osHandle, (LONG)distance, &HighPart, method);
#else
    seekResult = SetFilePointer (file->osHandle, distance, NULL, method);
#endif

    if (seekResult == errorResultC) {
        eo06_seekError (err);
        file->filePos = UNDEF;
    }
    else {
        switch (whence) {
            case sp5vf_seek_begin:
                file->filePos = distance;
                break;
            case sp5vf_seek_cur:
                if (eo06_VALIDPOS (file)) {
                    file->filePos += distance;
                }
                break;
            case sp5vf_seek_end:
                fileSize = GetFileSize (file->osHandle, NULL);
                if (fileSize == 0xFFFFFFFF){
                    file->filePos = UNDEF;
                }
                else {
                    file->filePos = fileSize + distance;
                }
                break;
        }
    }
    TRCOUT;
}

/*------------------------------*/

#else

static void
eo06_seekBySkip (
    FileT* file,
    tsp00_Longint distance,
    tsp05_RteSeekKind_Param whence,
    tsp05_RteFileError* err)
{
#undef MF__
#define MF__ MOD__"eo06_seekBySkip"  /* UNIX */
    switch (whence) {
        case sp5vf_seek_begin:
            eo06_seekBySkip (file, distance - file->filePos,
                    sp5vf_seek_cur, err);
            break;
        case sp5vf_seek_cur:
            if (distance < 0) {
                eo06_seekError (err);
            }
            else {
                byte buf [1024];
                tsp00_Longint bytesRead;
                while ((distance > 0) && (err->sp5fe_result == vf_ok)) {
                    bytesRead = eo06_readUnbufferedBinary (
                        file, buf, MIN_EO00 (distance, sizeof (buf)),
                        err, FALSE);
                    distance -= bytesRead;
                }
            }
            break;
        case sp5vf_seek_end:
            eo06_seekError (err);
            break;
    }
}

/*------------------------------*/

static void
eo06_seekUnbuffered (
    FileT* file,
    tsp00_Longint distance,
    tsp05_RteSeekKind_Param whence,
    tsp05_RteFileError* err)
{
#undef MF__
#define MF__ MOD__"eo06_seekUnbuffered"  /* UNIX */
    int         seekResult;
    struct stat statRecord;
    int         osRC;

    TRCIN;
#if ! defined (WIN32)
    if (file->seekBySkip) {
        eo06_seekBySkip (file, distance, whence, err);
        return;
    }
#endif
    seekResult = lseek (file->osHandle, distance, whence);
    if (seekResult == systemCallErrorC) {
        eo06_seekError (err);
        file->filePos = UNDEF;
    }
    else {
        switch (whence) {
            case sp5vf_seek_begin:
                file->filePos = distance;
                break;
            case sp5vf_seek_cur:
                if (eo06_VALIDPOS (file)) {
                    file->filePos += distance;
                }
                break;
            case sp5vf_seek_end:
                osRC = fstat (file->osHandle, &statRecord);
                if (osRC == systemCallErrorC) {
                    file->filePos = UNDEF;
                }
                else {
                    file->filePos = statRecord.st_size + distance;
                }
                break;
        }
    }
    TRCOUT;
}

/*------------------------------*/

#endif

static void
eo06_seekBufferedRead (
    FileT* file,
    tsp00_Longint distance,
    tsp05_RteSeekKind_Param whence,
    tsp05_RteFileError* err)
{
#undef MF__
#define MF__ MOD__"eo06_seekBufferedRead"
    TRCIN;
    if (whence == sp5vf_seek_cur) {
        /* physical file pos is post logical filepos */
        distance -= file->cache.extent - file->cache.current;
    }
    eo06_seekUnbuffered (file, distance, whence, err);
    if (err->sp5fe_result == VF_OK) {
        /* empty cache */
        file->cache.extent = 0;
        file->cache.current = 0;
    }
    TRCOUT;
}

/*------------------------------*/

static void
eo06_seekBufferedWrite (
    FileT* file,
    tsp00_Longint distance,
    tsp05_RteSeekKind_Param whence,
    tsp05_RteFileError* err)
{
#undef MF__
#define MF__ MOD__"eo06_seekBufferedWrite"
    TRCIN;
    eo06_FLUSH_V(file, err);
    if (err->sp5fe_result == VF_OK) {
        eo06_seekUnbuffered (file, distance, whence, err);
    }
    TRCOUT;
}

/*------------------------------*/

#if defined (SPECIAL_LONGSEEK)

#if defined (_WIN32)

static void
eo06_longseekUnbuffered (
    FileT* file,
    LargeIntegerT distance,
    tsp05_RteSeekKind_Param whence,
    tsp05_RteFileError* err)
{
#undef MF__
#define MF__ MOD__"eo06_longseekUnbuffered"  /* WIN32 */
    DWORD seekResult;
    DWORD method;
    static DWORD whence2method[] = {FILE_BEGIN, FILE_CURRENT, FILE_END};
#if defined (_WIN64)
     DWORD HighPart;
# endif


    TRCIN;
    method = whence2method [whence];

#if defined (_WIN64)
    HighPart = (LONG)(distance >> 32);
    seekResult = SetFilePointer (file->osHandle, (LONG)distance, &HighPart, method);
#else
    seekResult = SetFilePointer (file->osHandle, distance.LowPart,
                                  &distance.HighPart, method);
#endif

    if (seekResult == errorResultC) {
        eo06_seekError (err);
        file->filePos = UNDEF;
    }
    TRCOUT;
}

/*------------------------------*/

#else

static void
eo06_longseekUnbuffered (
    FileT* file,
    LargeIntegerT distance,
    tsp05_RteSeekKind_Param whence,
    tsp05_RteFileError* err)
{
#undef MF__
#define MF__ MOD__"eo06_longseekUnbuffered"  /* UNIX */
    int         seekResult;
    struct stat statRecord;
    int         osRC;

    TRCIN;
    seekResult = llseek (file->osHandle, distance, whence);
    if (seekResult == systemCallErrorC) {
        eo06_seekError (err);
        file->filePos = UNDEF;
    }
    else {
        file->filePos = UNDEF;
        /* should really calculate new filepos, but this is a
           tsp00_Longint and does not support large files */
    }
    TRCOUT;
}

/*------------------------------*/

#endif

static void
eo06_longseekBufferedRead (
    FileT* file,
    LargeIntegerT distance,
    tsp05_RteSeekKind_Param whence,
    tsp05_RteFileError* err)
{
#undef MF__
#define MF__ MOD__"eo06_longseekBufferedRead"
    TRCIN;
    if (whence == sp5vf_seek_cur) {
        /* physical file pos is post logical filepos */
        distance = eo06_LARGE_MINUS_LONG (distance,
            file->cache.extent - file->cache.current);
    }
    eo06_longseekUnbuffered (file, distance, whence, err);
    if (err->sp5fe_result == VF_OK) {
        /* empty cache */
        file->cache.extent = 0;
        file->cache.current = 0;
    }
    TRCOUT;
}

/*------------------------------*/

static void
eo06_longseekBufferedWrite (
    FileT* file,
    LargeIntegerT distance,
    tsp05_RteSeekKind_Param whence,
    tsp05_RteFileError* err)
{
#undef MF__
#define MF__ MOD__"eo06_longseekBufferedWrite"
    TRCIN;
    eo06_FLUSH_V(file, err);
    if (err->sp5fe_result == VF_OK) {
        eo06_longseekUnbuffered (file, distance, whence, err);
    }
    TRCOUT;
}
#endif
/*------------------------------*/


static FileClassT eo06_readUnbufferedBinaryVMT = {
    "readUnbufferedBinary",
    eo06_readUnbufferedBinary,
    eo06_readUnbufferedBinary,
    eo06_writeReadStream,
    eo06_flushDummy,
    eo06_seekUnbuffered,
    eo06_LONGSEEK_UNBUFFERED,
    eo06_fileClose
};

static FileClassT eo06_writeUnbufferedBinaryVMT = {
    "writeUnbufferedBinary",
    eo06_readWriteStream,
    eo06_readWriteStream,
    eo06_writeUnbufferedBinary,
    eo06_flushBuffer,
    eo06_seekUnbuffered,
    eo06_LONGSEEK_UNBUFFERED,
    eo06_fileClose
};

static FileClassT eo06_readwriteUnbufferedBinaryVMT = {
    "readwriteUnbufferedBinary",
    eo06_readUnbufferedBinary,
    eo06_readUnbufferedBinary,
    eo06_writeUnbufferedBinary,
    eo06_flushBuffer,
    eo06_seekUnbuffered,
    eo06_LONGSEEK_UNBUFFERED,
    eo06_fileClose
};

static FileClassT eo06_readBufferedBinaryVMT = {
    "readBufferedBinary",
    eo06_readUnbufferedBinary,
    eo06_readBufferedBinary,
    eo06_writeReadStream,
    eo06_flushDummy,
    eo06_seekBufferedRead,
    eo06_LONGSEEK_BUFFERED_READ,
    eo06_fileClose
};

static FileClassT eo06_writeBufferedBinaryVMT = {
    "writeBufferedBinary",
    eo06_readWriteStream,
    eo06_readWriteStream,
    eo06_writeBufferedBinary,
    eo06_flushBuffer,
    eo06_seekBufferedWrite,
    eo06_LONGSEEK_BUFFERED_WRITE,
    eo06_fileClose
};

static FileClassT eo06_readUnbufferedTextVMT = {
    "readUnbufferedText",
    eo06_readUnbufferedBinary,
    eo06_readBufferedText, /* reading text always buffered */
    eo06_writeReadStream,
    eo06_flushDummy,
    eo06_seekUnbuffered,
    eo06_LONGSEEK_UNBUFFERED,
    eo06_fileClose
};

static FileClassT eo06_writeUnbufferedTextVMT = {
    "writeUnbufferedText",
    eo06_readWriteStream,
    eo06_readWriteStream,
    eo06_writeUnbufferedText,
    eo06_flushBuffer,
    eo06_seekUnbuffered,
    eo06_LONGSEEK_UNBUFFERED,
    eo06_fileClose
};

static FileClassT eo06_readBufferedTextVMT = {
    "readBufferedText",
    eo06_readUnbufferedBinary,
    eo06_readBufferedText,
    eo06_writeReadStream,
    eo06_flushDummy,
    eo06_seekBufferedRead,
    eo06_LONGSEEK_BUFFERED_READ,
    eo06_fileClose
};

static FileClassT eo06_writeBufferedTextVMT = {
    "writeBufferedText",
    eo06_readWriteStream,
    eo06_readWriteStream,
    eo06_writeBufferedText,
    eo06_flushBuffer,
    eo06_seekBufferedWrite,
    eo06_LONGSEEK_BUFFERED_WRITE,
    eo06_fileClose
};

static FileClassT eo06_readUnbufferedEncodedVMT = {
    "readUnbufferedEncoded",
    eo06_readUnbufferedBinary,
    eo06_readBufferedEncoded, /* reading encoded always buffered */
    eo06_writeReadStream,
    eo06_flushDummy,
    eo06_seekBufferedRead,
    eo06_LONGSEEK_BUFFERED_READ,
    eo06_fileClose
};

static FileClassT eo06_writeUnbufferedEncodedVMT = {
    "writeUnbufferedEncoded",
    eo06_readWriteStream,
    eo06_readWriteStream,
    eo06_writeUnbufferedEncoded,
    eo06_flushBuffer,
    eo06_seekBufferedWrite,
    eo06_LONGSEEK_BUFFERED_WRITE,
    eo06_fileClose
};

static FileClassT eo06_readBufferedEncodedVMT = {
    "readBufferedEncoded",
    eo06_readUnbufferedBinary,
    eo06_readBufferedEncoded,
    eo06_writeReadStream,
    eo06_flushDummy,
    eo06_seekBufferedRead,
    eo06_LONGSEEK_BUFFERED_READ,
    eo06_fileClose
};

static FileClassT eo06_writeBufferedEncodedVMT = {
    "writeBufferedEncoded",
    eo06_readWriteStream,
    eo06_readWriteStream,
    eo06_writeBufferedEncoded,
    eo06_flushBuffer,
    eo06_seekBufferedWrite,
    eo06_LONGSEEK_BUFFERED_WRITE,
    eo06_fileClose
};

#if defined (_WIN32)
/*
 * these are classes for console IO on NT
 *
 * they are choosen when all of the following
 * conditions are met:
 *      - sqlfopenEncodedc
 *      - is std handle
 *      - handle points to console
 */
static FileClassT eo06_readConsoleEncodedVMT = {
    "readConsoleEncoded",
    eo06_readConsoleEncoded,
    eo06_readBufferedEncoded,
    eo06_writeReadStream,
    eo06_flushDummy,
    eo06_seekBufferedRead,
    eo06_LONGSEEK_BUFFERED_READ,
    eo06_fileClose
};

static FileClassT eo06_writeConsoleEncodedVMT = {
    "writeConsoleEncoded",
    eo06_readWriteStream,
    eo06_readWriteStream,
    eo06_writeUnbufferedEncoded,
    eo06_flushConsoleBuffer,
    eo06_seekBufferedWrite,
    eo06_LONGSEEK_BUFFERED_WRITE,
    eo06_fileClose
};
#endif

static FileClassT* classDesc [3] [3] [2] = {
    /* dim 0: SP5VF_READ/SP5VF_WRITE/SP5VF_READWRITE    */
    /* dim 1: SP5VF_TEXT/SP5VF_BINARY/SP5VF_ENCODED */
    /* dim 2: unbuffered/buffered */
{
    {&eo06_readUnbufferedTextVMT, &eo06_readBufferedTextVMT},
    {&eo06_readUnbufferedBinaryVMT, &eo06_readBufferedBinaryVMT},
    {&eo06_readUnbufferedEncodedVMT, &eo06_readBufferedEncodedVMT}
},
{
    {&eo06_writeUnbufferedTextVMT, &eo06_writeBufferedTextVMT},
    {&eo06_writeUnbufferedBinaryVMT, &eo06_writeBufferedBinaryVMT},
    {&eo06_writeUnbufferedEncodedVMT, &eo06_writeBufferedEncodedVMT}
},
{
    {NULL, NULL},
    {&eo06_readwriteUnbufferedBinaryVMT, NULL},
    {NULL, NULL}
}
};

#define unbufferedC     FALSE
#define bufferedC       TRUE

static FileClassT*
eo06_fileClass (
    tsp05_RteDataKind_Param dataKind,
    tsp05_RteFileMode_Param fileMode,
    tsp05_RteBufferingKind_Param bufKind)
{
#undef MF__
#define MF__ MOD__"eo06_fileClass"
    FileClassT* result;
    int buffering;

    if (fileMode == sp5vf_append) {
        fileMode = sp5vf_write;
    }
    if (bufKind == sp5bk_buffered) {
        buffering = bufferedC;
    }
    else {
        buffering = unbufferedC;
    }
    result = classDesc [fileMode] [dataKind] [buffering];
    TRCPAS_P (("class '%s'\n", result->className));
    return result;
}

/*------------------------------*/

static const tsp77encoding *
eo06_detectEncoding (
    const byte * buf,
    int          bufLen)
{
#undef MF__
#define MF__ MOD__"eo06_detectEncoding"
    static const tsp77encoding * testableEncodings [] = {
        NULL,
        NULL,
        NULL,
        NULL
    };
    int i;

    if (testableEncodings [0] == NULL) {
        testableEncodings [0] = sp77encodingUCS2;
        testableEncodings [1] = sp77encodingUCS2Swapped;
        testableEncodings [2] = sp77encodingUTF8;
    }
    for (i = 0; testableEncodings [i] != NULL; ++i) {
        const tsp77charConst * indicator = &testableEncodings [i]->charTable->indicator;
        if (sp77charIsEqual (indicator, buf, bufLen)) {
            return testableEncodings [i];
        }
    }
    /* no match found */
    return NULL;
}

/*------------------------------*/

static void
eo06_checkFileEncoding (
    FileT * file,
    tsp05_RteFileError *ferr)
{
#undef MF__
#define MF__ MOD__"eo06_checkFileEncoding"
    CacheT * cache = &file->cache;

    if (cache->extent > 0) {
        const tsp77charConst * indicator = &file->fileEncoding->charTable->indicator;
        if (!sp77charIsEqual (indicator, CACHEPOINTER (cache), (int) READSLACK (cache))) {
            eo06_encodingError (ferr, file->fileEncoding);
        }
    }
}

/*------------------------------*/

static void
eo06_detectFileEncoding (
    FileT * file,
    tsp05_RteFileError *ferr)
{
#undef MF__
#define MF__ MOD__"eo06_detectFileEncoding"
    CacheT * cache = &file->cache;

    if (cache->extent > 0) {
        file->fileEncoding = eo06_detectEncoding (
                CACHEPOINTER (cache), (int) READSLACK (cache));
    }
    if (file->fileEncoding == NULL) {
        /* ascii is the default */
        file->fileEncoding = sp77encodingAscii;
    }
}

/*------------------------------*/

static void
eo06_skipIndicator (
    FileT * file,
    tsp05_RteFileMode_Param fileMode)
{
#undef MF__
#define MF__ MOD__"eo06_skipIndicator"
#ifndef WIN32
    struct stat statRecord;
    int         osRC;
#endif
    unsigned long fileSize;
    if (!file->isStdHandle) {
        CacheT * cache = &file->cache;
        const tsp77charConst * indicator = &file->fileEncoding->charTable->indicator;

        switch (fileMode) {
            case sp5vf_read:
                if (READSLACK (cache) >= indicator->byteCount) {
                    cache->current += indicator->byteCount;
                }
                break;
            case sp5vf_write:
                SAPDB_memcpy (CACHEPOINTER (cache), indicator->bytes, indicator->byteCount);
                cache->current += indicator->byteCount;
                break;
            case sp5vf_append:
                /* append encoding indicator only when the file is empty */
#ifdef WIN32
                fileSize = GetFileSize (file->osHandle, NULL);
#else
                osRC = fstat (file->osHandle, &statRecord);
                if (osRC != systemCallErrorC) 
                {
                    fileSize = statRecord.st_size;
                }
#endif
                if (0 == fileSize)
                {
                    SAPDB_memcpy (CACHEPOINTER (cache), indicator->bytes, indicator->byteCount);
                    cache->current += indicator->byteCount;
                }
                break;
            default:
                /* do nothing */
                break;
        }
    }
}

/*------------------------------*/

static void
eo06_handleEncoding (
    FileT * file,
    tsp05_RteFileMode_Param fileMode,
    tsp05_RteFileError *ferr)
{
#undef MF__
#define MF__ MOD__"eo06_handleEncoding"
    CacheT * cache = &file->cache;

#if defined (_WIN32)
    if (file->isStdHandle) {
        DWORD   consoleMode;

        if (GetConsoleMode (file->osHandle, &consoleMode)) {
            switch (fileMode) {
                case sp5vf_read:
                    file->classDesc = &eo06_readConsoleEncodedVMT;
                    break;
                case sp5vf_write:
                case sp5vf_append:
                    file->classDesc = &eo06_writeConsoleEncodedVMT;
                    break;
                default:
                    SWITCH_ERROR_DBG_MSP00 (fileMode);
                    break;
            }
            file->fileEncoding = sp77encodingUCS2Native;
            file->isStdHandle = true;
        }
        else {
            file->isStdHandle = false;
            if (file->fileEncoding == NULL) {
                file->fileEncoding = sp77encodingUCS2Native;
            }
        }
    }
#else
    if (file->isStdHandle) {
        if (isatty (file->osHandle)) {
            const tsp77encoding * localeEncoding = eo602getConsoleEncoding ();

            file->fileEncoding = localeEncoding;
        }
        else {
            file->isStdHandle = false;
        }
        if (file->fileEncoding == NULL) {
            file->fileEncoding = sp77encodingUCS2Native;
        }
    }
#endif
    if (!file->isStdHandle) {
        switch (fileMode) {
            case sp5vf_read:
                cache->current = 0;
                cache->extent = eo06_LOW_LEVEL_READ_V (file, cache->mem,
                    cache->memSize, ferr, FALSE);
                if(VF_EOF == ferr->sp5fe_result)    /* EOF is no problem, may be an empty file (treated as ASCII) PTS 1130243*/
                {
                    ferr->sp5fe_result = VF_OK;
                }
                if (file->fileEncoding == NULL) {
                    eo06_detectFileEncoding (file, ferr);
                }
                else {
                    eo06_checkFileEncoding (file, ferr);
                }
                break;
            default:
                if (file->fileEncoding == NULL) {
                    eo06_encodingError (ferr, NULL);
                }
                break;
        }
        if (ferr->sp5fe_result == VF_OK) {
            eo06_skipIndicator (file, fileMode);
        }
    }
    /*
     * calculate eolMarkerSize
     */
    if (file->fileEncoding != NULL) {
        file->eolMarkerSize = file->fileEncoding->charTable->nl.byteCount;
#if defined (_WIN32)
        file->eolMarkerSize += file->fileEncoding->charTable->cr.byteCount;
#endif
    }
}

/*------------------------------*/

static const byte *
eo06_encchr (
    const void * buf,
    const tsp77charConst * charConst,
    int bufSize)
{
#undef MF__
#define MF__ MOD__"eo06_encchr"
    const byte * bufPos = (const byte *) buf;
    const byte * bufEnd = bufPos + bufSize;
    BOOLEAN matchFound = FALSE;

    while ((bufPos != NULL) && !matchFound) {
        bufPos = memchr (bufPos, charConst->bytes [0], bufEnd - bufPos);
        if (bufPos == NULL) {
            /* nothing found */
        }
        else if ((bufEnd - bufPos) < charConst->byteCount) {
            /* to small for match */
            bufPos = NULL;
        }
        else if (memcmp (bufPos, charConst->bytes, charConst->byteCount) == 0) {
            /* match found */
            matchFound = TRUE;
        }
        else {
            /* try next character */
            ++bufPos;
        }
    }
    return bufPos;
}

/*------------------------------*/

static int
eo06_enclen (
    const tsp77encoding * encoding,
    const void * buf)
{
#undef MF__
#define MF__ MOD__"eo06_encchr"
    const byte * bufStart = (const byte *) buf;
    const tsp77charConst * terminator = &encoding->charTable->terminator;
    const byte * termPos = eo06_encchr (buf, terminator, INT_MAX);
    return (int) (termPos - bufStart);
}

/**\
--------------------------------------- MF__ MOD__ (sqlfopen)
\**/

static BOOLEAN
eo06_openArgsOK (
    tsp05_RteDataKind_Param dataKind,
    tsp05_RteFileMode_Param fileMode,
    tsp05_RteBufferingKind_Param buffering,
    tsp05_RteFileError *ferr)
{
#undef MF__
#define MF__ MOD__"eo06_openArgsOK"
    TRCIN;
    if (OUT_OF_ENUM_INL (dataKind, sp5vf_text, sp5vf_encoded)) {
        eo06_paramError (ferr, "data kind");
        TRCOUT;
        return FALSE;
    }
    if (OUT_OF_ENUM_INL (fileMode, sp5vf_read, sp5vf_append)) {
        eo06_paramError (ferr, "file mode");
        TRCOUT;
        return FALSE;
    }
    if (OUT_OF_ENUM_INL (buffering, BK_NORMAL, sp5bk_disksynced)) {
        eo06_paramError (ferr, "buffering");
        TRCOUT;
        return FALSE;
    }
    TRCOUT;
    return TRUE;
}

/*------------------------------*/

static BOOLEAN
eo06_allocCacheMem (
    CacheT *cache,
    int bufferSize,
    BOOLEAN aligned)
{
    static const int  memAlignmentC = 4096;
    tsp00_Longuint      memAlias;

    if (UNDEF != bufferSize) {
        if (aligned) {
            cache->membase = malloc (bufferSize + memAlignmentC);
            if (cache->membase != NULL) {
                memAlias = (tsp00_Longuint) cache->membase;
                memAlias = (((memAlias + memAlignmentC - 1) / memAlignmentC)
                            * memAlignmentC);
                cache->mem = (byte*) memAlias;
            }
        }
        else {
            cache->membase = malloc (bufferSize);
            cache->mem = cache->membase;
        }
        if (cache->membase == NULL) {
            return FALSE;
        }
        cache->memSize = bufferSize;
    }
    else {
        cache->membase = NULL;
        cache->mem = NULL;
        cache->memSize = 0;
    }
    cache->current = 0;
    cache->extent = 0;
    return TRUE;
}

/*------------------------------*/

#define standardCacheSizeC      4096
#define smallCacheSizeC          255

static void
eo06_sqlfopen (
    const char *fname,
    const tsp77encoding ** fileEncoding,
    const tsp77encoding * clientEncoding,
    tsp05_RteDataKind_Param dataKind,
    tsp05_RteFileMode_Param fileMode,
    tsp05_RteBufferingKind_Param buffering,
    tsp00_Int4 *fileHandle,
    tsp05_RteFileError *ferr)
{
#undef MF__
#define MF__ MOD__"eo06_sqlfopen"
    FileT*      file;
    int         bufferSize = UNDEF;

    TRCIN_P (("'%s' datakind %d  filemode %d  buffering %d\n",
              fname, dataKind, fileMode, buffering));
    eo06_clearError (ferr);
    if (!eo06_openArgsOK (dataKind, fileMode, buffering, ferr)) {
      TRCOUT;
      return;
    }
    /* get file struct */
    *fileHandle = eo06_allocSlot (&allFilesV, sizeof (FileT));
    if (*fileHandle == UNDEF) {
        eo06_ferror (ferr, Out_Of_Memory_ErrText);
        TRCOUT;
        return;
    }
    file = eo06_getPointer (&allFilesV, *fileHandle);

    if ( (sp5vf_text == dataKind) || (sp5vf_encoded == dataKind) ) {
        if (sp5vf_read == fileMode) {
            buffering = sp5bk_buffered;
            bufferSize = standardCacheSizeC;
        }
        else {
            /* a small buffer is required, as eo06_writeUnbufferedText   */
            /* uses eo06_writeBufferedText to concatenate the Text       */
            /* and the newline                                          */
            bufferSize = smallCacheSizeC;
        }
    }
    else if (sp5bk_buffered == buffering) {
        bufferSize = standardCacheSizeC;
    }
#if ! defined (WIN32)
    file->seekBySkip = FALSE;
#endif
    E06_FILEOPEN_OS (fname, fileMode, buffering, file, ferr);
    if (ferr->sp5fe_result != VF_OK) {
        eo06_freeSlot (&allFilesV, *fileHandle);
        *fileHandle = UNDEF;
        TRCOUT;
        return;
    }

    file->classDesc = eo06_fileClass (dataKind, fileMode, buffering);
    if (file->classDesc == NULL) {
        eo06_freeSlot (&allFilesV, *fileHandle);
        *fileHandle = UNDEF;
        eo06_ferror (ferr, Invalid_Open_ErrText);
        TRCOUT;
        return;
    }

    /* initialize cache */
    if (!eo06_allocCacheMem (&file->cache, bufferSize, FALSE)) {
        sqlfclosec (*fileHandle, sp5vf_close_normal, ferr);
        eo06_ferror (ferr, Out_Of_Memory_ErrText);
        *fileHandle = UNDEF;
    }
    file->filePos = 0;
    file->fileEncoding = fileEncoding ? *fileEncoding : NULL;
    file->clientEncoding = clientEncoding;
    file->eolMarkerSize = eolMarkerSizeC;
    if (dataKind == sp5vf_encoded) {
        eo06_handleEncoding (file, fileMode, ferr);
        if (ferr->sp5fe_result != VF_OK) {
            sqlfclosec (*fileHandle, sp5vf_close_normal, NULL);
            *fileHandle = UNDEF;
        }
    }
	if (NULL == clientEncoding)
	{
		file->clientEncoding = file->fileEncoding;
	}
    if(NULL != fileEncoding)
    {
        *fileEncoding = file->fileEncoding;
    }
    TRCOUT_P (("handle %d\n", *fileHandle));
}

/*------------------------------*/

externC void
sqlfopenc (
    const char *rawFName,
    tsp05_RteDataKind_Param dataKind,
    tsp05_RteFileMode_Param fileMode,
    tsp05_RteBufferingKind_Param buffering,
    tsp00_Int4 *fileHandle,
    tsp05_RteFileError *ferr)
{
#undef MF__
#define MF__ MOD__"sqlfopenc"
    char nameBuf [EO06_PATH_MAX_C];

    TRCIN;
    eo06_sqlfopen (eo06_cookName (nameBuf, sizeof(nameBuf), rawFName, UNDEF),
        NULL, NULL, dataKind, fileMode, buffering, fileHandle, ferr);
    TRCOUT;
}

/*------------------------------*/

externC void
sqlfopenp (
    const tsp00_VFilename rawFName,
    tsp05_RteDataKind dataKind,
    tsp05_RteFileMode fileMode,
    tsp05_RteBufferingKind buffering,
    tsp00_Int4 *fileHandle,
    tsp05_RteFileError *ferr)
{
#undef MF__
#define MF__ MOD__"sqlfopenp"
    char trimmedBuf [EO06_PATH_MAX_C];
    char nameBuf [EO06_PATH_MAX_C];
    int nameLen;

    TRCIN;
    nameLen = eo06_trimLen (rawFName, sizeof (tsp00_VFilename));
    SAPDB_memcpy (trimmedBuf, rawFName, nameLen);
    trimmedBuf [nameLen] = '\0';

    eo06_sqlfopen (eo06_cookName (nameBuf, sizeof(nameBuf), trimmedBuf, nameLen),
        NULL, NULL, dataKind, fileMode, buffering, fileHandle, ferr);

    eo06_padLen (ferr->sp5fe_text, MAXERRORTEXT);
    TRCOUT;
}

/*------------------------------*/

externC void
sqlfopenEncodedc (
    const char *rawFName,
    const tsp77encoding ** fileEncoding,
    const tsp77encoding * clientEncoding,
    tsp05_RteFileMode_Enum fileMode,
    tsp05_RteBufferingKind_Enum buffering,
    tsp00_Int4 *fileHandle,
    tsp05_RteFileError *ferr)
{
#undef MF__
#define MF__ MOD__"sqlfopenEncodedc"
    char nameBuf [EO06_PATH_MAX_C];

    TRCIN;
    eo06_sqlfopen (eo06_cookName (nameBuf, sizeof(nameBuf), rawFName, UNDEF),
        fileEncoding, clientEncoding, sp5vf_encoded,
        fileMode, buffering, fileHandle, ferr);
    TRCOUT;
}

/*------------------------------*/

#if defined (_WIN32)

static void
eo06_setBlockSize (
    FileT* file,
    tsp00_Int4 blockSize)
{
#undef MF__
#define MF__ MOD__"eo06_setBlockSize"
    TAPE_SET_MEDIA_PARAMETERS   mediaInfo;
    APIRET                      rc;
    int                         retry = 3;

    memset ( &mediaInfo, 0, sizeof(mediaInfo));
    mediaInfo.BlockSize = blockSize;
    do {
        --retry;
        rc = SetTapeParameters( file->osHandle,
                                SET_TAPE_MEDIA_INFORMATION,
                                &mediaInfo );
    } while (( rc == ERROR_BUS_RESET
               || rc == ERROR_IO_DEVICE
               || rc == ERROR_END_OF_MEDIA
               || rc == ERROR_BEGINNING_OF_MEDIA
               || rc == ERROR_MEDIA_CHANGED )
               && (retry > 0));
}

#endif

/*------------------------------*/

static void
eo06_page0_read (
    FileT* file,
    tsp05_RteFileMode_Param fileMode,
    tsp00_Int4* bufferSize,
    tsp05_RteFileError *ferr)
{
#undef MF__
#define MF__ MOD__"eo06_page0_read"
    CacheT* cache = &file->cache;

#if defined (_WIN32)
    if (DT_TAPE == file->lDevType) 
    {
        eo06_setBlockSize (file, *bufferSize);
    }
#endif
    /* done in a loop because read may only be possible in small portions (seen using pipes on HPIA64 and AIX) PTS 1138838*/
    {
        tsp00_Longint leftover = (tsp00_Int4 )*bufferSize;
        tsp00_Longint bytesReadThisTime;
        unsigned char *addr = cache->mem;
        do
        {
            bytesReadThisTime = eo06_READ_V (file, addr, leftover, ferr, FALSE);
            leftover -= bytesReadThisTime;
            addr += bytesReadThisTime;
        }
        while( (leftover > 0) && (ferr->sp5fe_result == 0) );
    }
#if defined (_WIN32)
    if ( GetLastError() == ERROR_LOCK_VIOLATION )
#else
    if( errno == EACCES )
#endif
    {
        ferr->sp5fe_warning = sp5vfw_already_locked;
    }
}

/*------------------------------*/

static boolean
eo06_page0_io (
    FileT* file,
    tsp05_RteFileMode_Param fileMode,
    tsp00_Int4* bufferSize,
    tsp05_RteFileError *ferr)
{
    BOOLEAN ok;
    CacheT* cache = &file->cache;
    tsp00_Int4 expectedBufferSize;

    /* nt: set blocksize */
    switch (fileMode) {
    case sp5vf_read:
    case sp5vf_readwrite:
        eo06_page0_read (file, fileMode, bufferSize, ferr);
        if (ferr->sp5fe_result != 0) {
            ok = FALSE;
        }
        else {
            tsp00_Int4 bufferSizeRead;

            teo54Page0Info *pInfo = (teo54Page0Info *)(cache->mem + PAGE_0_RECORD_OFFSET_EO54);

            expectedBufferSize = *bufferSize;
            if ( !strcmp(pInfo->magicHeader_eo54, MAGIC_HEADER_EO54) )
            {
                /* New Unix Page0 Layout */
                bufferSizeRead = eo54RestoredInt4(pInfo->blockSize_eo54);
            }
            else
            {
#if defined(_WIN32)
                bufferSizeRead = ((byte)cache->mem [4] << 24)
                            + ((byte)cache->mem [5] << 16)
                            + ((byte)cache->mem [2] << 8)
                            +  (byte)cache->mem [3];
#else
                /*
                * Compatibility with old tapes must be enforced...
                */
                /* Old Page0 Layout contained only two bytes for bufferSize */
                expectedBufferSize = expectedBufferSize & 0xFFFF;
                bufferSizeRead =
                  ((unsigned char)(((teo54OldUnixPage0Info *)cache->mem)->blockSizeByte1_Ueo54) << 8)
                 | (unsigned char)(((teo54OldUnixPage0Info *)cache->mem)->blockSizeByte0_Ueo54);
#endif /* _WIN32 */
            }
            if (bufferSizeRead == expectedBufferSize) {
                ok = TRUE;
            }
            else {
                eo06_pagesizeError (ferr, *bufferSize, bufferSizeRead);
                *bufferSize = bufferSizeRead;
                ok = FALSE;
            }
        }
        break;
    case sp5vf_write:
#if defined (_WIN32)
        if (file->lDevType == DT_TAPE) {
            eo06_setBlockSize (file, *bufferSize);
        }
#endif
        memset (cache->mem, '\0', cache->memSize);
#if defined(_WIN32)
        cache->mem [3] = (byte) (  *bufferSize        & 0xff);
        cache->mem [2] = (byte) ( (*bufferSize >> 8)  & 0xff);
        cache->mem [5] = (byte) ( (*bufferSize >> 16) & 0xff);
        cache->mem [4] = (byte) ( (*bufferSize >> 24) & 0xff);
#else
        {
            teo54Page0Info *pInfo = (teo54Page0Info *)(cache->mem + PAGE_0_RECORD_OFFSET_EO54);
            pInfo->blockSize_eo54 = eo54SavableInt4(*bufferSize);
        }
#endif /* _WIN32 */
        eo06_WRITE_V (file, cache->mem, cache->memSize, ferr);
        ok = (ferr->sp5fe_result == 0);
        break;
    case sp5vf_append:
    default:
        /* not suitable for saves or devspaces */
        ok = FALSE;
        break;
    }
    return ok;
}

/*------------------------------*/


static void
eo06_sqlfsaveopen (
    const char *fname,
    tsp05_RteFileMode_Param fileMode,
    tsp00_Int4* bufferSize,
    tsp00_Int4 *fileHandle,
    void** bufptr,
    tsp05_RteFileError *ferr)
{
#undef MF__
#define MF__ MOD__"eo06_sqlfsaveopen"
    FileT*      file;
    tsp00_Int4  usedBufferSize;

    TRCIN_P (("'%s' filemode %d\n", fname, fileMode));
    eo06_clearError (ferr);
    if (!eo06_openArgsOK (sp5vf_binary, fileMode, sp5bk_unbuffered, ferr)) {
      TRCOUT;
      return;
    }
    if (*bufferSize <= 0) {
        eo06_paramError (ferr, "buffersize");
        TRCOUT;
        return;
    }
    /* get file struct */
    *fileHandle = eo06_allocSlot (&allFilesV, sizeof (FileT));
    if (*fileHandle == UNDEF) {
        eo06_ferror (ferr, Out_Of_Memory_ErrText);
        TRCOUT;
        return;
    }
    file = eo06_getPointer (&allFilesV, *fileHandle);

    E06_FILEOPEN_OS (fname, fileMode, sp5bk_unbuffered, file, ferr);
    if (ferr->sp5fe_result != VF_OK) {
        eo06_freeSlot (&allFilesV, *fileHandle);
        *fileHandle = UNDEF;
        TRCOUT;
        return;
    }

    file->classDesc = eo06_fileClass (sp5vf_binary, fileMode, sp5bk_unbuffered);
    if (file->classDesc == NULL) {
        eo06_freeSlot (&allFilesV, *fileHandle);
        *fileHandle = UNDEF;
        eo06_ferror (ferr, Invalid_Open_ErrText);
        TRCOUT;
        return;
    }

    /* initialize cache */
    usedBufferSize = *bufferSize;
    /* PTS 1117408 override user given bufferSize if it is too small */
#define MINIMUM_PAGE0_SIZE_EO06 4096
    if ( usedBufferSize < MINIMUM_PAGE0_SIZE_EO06 ) {
        usedBufferSize = MINIMUM_PAGE0_SIZE_EO06;
    }

    if (!eo06_allocCacheMem (&file->cache, usedBufferSize, TRUE)) {
        sqlfclosec (*fileHandle, sp5vf_close_normal, ferr);
        eo06_ferror (ferr, Out_Of_Memory_ErrText);
        *fileHandle = UNDEF;
        return;
    }
    file->filePos = 0;
    *bufptr = file->cache.mem;

    /* page0_io */
    if (!eo06_page0_io (file, fileMode, bufferSize, ferr)) {
        tsp05_RteFileError dummyError;

        sqlfclosec (*fileHandle, sp5vf_close_normal, &dummyError);
        *fileHandle = UNDEF;
        *bufptr = NULL;
        TRCOUT;
        return;
    }
    TRCOUT_P (("handle %d\n", *fileHandle));
}

/*------------------------------*/

externC void
sqlfsaveopenc (
    const char *rawFName,
    tsp05_RteFileMode_Param fileMode,
    tsp00_Int4* buffersize,
    tsp00_Int4 *fileHandle,
    void** bufptr,
    tsp05_RteFileError *ferr)
{
#undef MF__
#define MF__ MOD__"sqlfsaveopenc"
    char nameBuf [EO06_PATH_MAX_C];

    TRCIN;
    eo06_sqlfsaveopen (eo06_cookName (nameBuf, sizeof(nameBuf), rawFName, UNDEF),
        fileMode, buffersize, fileHandle, bufptr, ferr);
    TRCOUT;
}

/*------------------------------*/

externC void
sqlfsaveopenp (
    const tsp00_VFilename rawFName,
    tsp05_RteFileMode fileMode,
    tsp00_Int4* buffersize,
    tsp00_Int4 *fileHandle,
    void** bufptr,
    tsp05_RteFileError *ferr)
{
#undef MF__
#define MF__ MOD__"sqlfsaveopenp"
    char trimmedBuf [EO06_PATH_MAX_C];
    char nameBuf [EO06_PATH_MAX_C];
    int nameLen;

    TRCIN;
    nameLen = eo06_trimLen (rawFName, sizeof (tsp00_VFilename));
    SAPDB_memcpy (trimmedBuf, rawFName, nameLen);
    trimmedBuf [nameLen] = '\0';

    eo06_sqlfsaveopen (eo06_cookName (nameBuf, sizeof(nameBuf), trimmedBuf, nameLen),
        fileMode, buffersize, fileHandle, bufptr, ferr);

    eo06_padLen (ferr->sp5fe_text, MAXERRORTEXT);
    TRCOUT;
}

/*------------------------------*/


/**\
--------------------------------------- MF__ MOD__ (sqlfclose)
\**/

externC void
sqlfclosec (
    tsp00_Int4 fileHandle,
    tsp05_RteCloseOption_Param option,
    tsp05_RteFileError *ferr)
{
#undef MF__
#define MF__ MOD__"sqlfclosec"
    FileT* file;
    tsp05_RteFileError dummy;

    TRCIN_P (("handle %d, option %d\n", fileHandle, option));
    if (ferr == NULL) {
        ferr = &dummy;
    }
    eo06_clearError (ferr);
    if (OUT_OF_ENUM_INL (option, sp5vf_close_normal, sp5vf_close_next_tape)) {
        eo06_paramError (ferr, "option");
        TRCOUT;
        return;
    }

    file = eo06_getPointer (&allFilesV, fileHandle);
    if (file == NULL) {
        eo06_ferror (ferr, Invalid_Handle_ErrText);
        TRCOUT;
        return;
    }

    eo06_FLUSH_V (file, ferr);
    if (file->cache.membase != NULL) {
        free (file->cache.membase);
    }
    eo06_CLOSE_V (file, option, ferr);
    eo06_freeSlot (&allFilesV, fileHandle);
    TRCOUT;
}

/*------------------------------*/

externC void
sqlfclosep (
    tsp00_Int4 fileHandle,
    tsp05_RteCloseOption option,
    tsp05_RteFileError *ferr)
{
#undef MF__
#define MF__ MOD__"sqlfclosep"
    TRCIN;
    sqlfclosec (fileHandle, option, ferr);

    eo06_padLen (ferr->sp5fe_text, MAXERRORTEXT);
    TRCOUT;
}

/*------------------------------*/


/**\
--------------------------------------- MF__ MOD__ (sqlfread)
\**/

static void
eo06_sqlfread (
    tsp00_Int4 fileHandle,
    void* buf,
    tsp00_Longint bufSize,
    tsp00_Longint* outLen,
    tsp05_RteFileError *ferr,
    BOOLEAN forC)
{
#undef MF__
#define MF__ MOD__"eo06_sqlfread"
    FileT* file;

    TRCIN;
    eo06_clearError (ferr);
    file = eo06_getPointer (&allFilesV, fileHandle);
    if (file != NULL) {
        *outLen = eo06_READ_V (file, buf, bufSize, ferr, forC);
    }
    else {
        eo06_ferror (ferr, Invalid_Handle_ErrText);
        *outLen = 0;
    }
    TRCOUT;
}

/*------------------------------*/

externC void
sqlfreadc (
    tsp00_Int4 fileHandle,
    void* buf,
    tsp00_Longint bufSize,
    tsp00_Longint* outLen,
    tsp05_RteFileError *ferr)
{
#undef MF__
#define MF__ MOD__"sqlfreadc"
    TRCIN;
    eo06_sqlfread (fileHandle, buf, bufSize, outLen, ferr, TRUE);
    TRCOUT;
}

/*------------------------------*/

externC void
sqlfreadp (
    tsp00_Int4 fileHandle,
    void* buf,
    tsp00_Longint bufSize,
    tsp00_Longint* outLen,
    tsp05_RteFileError *ferr)
{
#undef MF__
#define MF__ MOD__"sqlfreadp"
    TRCIN;
    eo06_sqlfread (fileHandle, buf, bufSize, outLen, ferr, FALSE);

    eo06_padLen (ferr->sp5fe_text, MAXERRORTEXT);
    TRCOUT;
}

/*------------------------------*/


/**\
--------------------------------------- MF__ MOD__ (sqlfwrite)
\**/

externC void
sqlfwritec (
    tsp00_Int4 fileHandle,
    const void* buf,
    tsp00_Longint inLen,
    tsp05_RteFileError *ferr)
{
#undef MF__
#define MF__ MOD__"sqlfwritec"
    FileT* file;

    TRCIN;
    eo06_clearError (ferr);
    file = eo06_getPointer (&allFilesV, fileHandle);
    if (file != NULL) {
        eo06_WRITE_V (file, buf, inLen, ferr);
    }
    else {
        eo06_ferror (ferr, Invalid_Handle_ErrText);
    }
    TRCOUT;
}

/*------------------------------*/

externC void
sqlfwritep (
    tsp00_Int4 fileHandle,
    const void* buf,
    tsp00_Longint inLen,
    tsp05_RteFileError *ferr)
{
#undef MF__
#define MF__ MOD__"sqlfwritep"
    TRCIN;
    sqlfwritec (fileHandle, buf, inLen, ferr);

    eo06_padLen (ferr->sp5fe_text, MAXERRORTEXT);
    TRCOUT;
}

/*------------------------------*/


/**\
--------------------------------------- MF__ MOD__ (sqlfseek)
\**/

externC void
sqlfseekc (
    tsp00_Int4 fileHandle,
    tsp00_Longint distance,
    tsp05_RteSeekKind_Param whence,
    tsp05_RteFileError *ferr)
{
#undef MF__
#define MF__ MOD__"sqlfseekc"
    FileT* file;

    TRCIN_P (("handle %d, distance %ld  whence %d\n",
              fileHandle, distance, whence));
    eo06_clearError (ferr);
    if (OUT_OF_ENUM_INL (whence, sp5vf_seek_begin, sp5vf_seek_end)) {
        eo06_paramError (ferr, "whence");
        TRCOUT;
        return;
    }
    file = eo06_getPointer (&allFilesV, fileHandle);
    if (file != NULL) {
        eo06_SEEK_V (file, distance, whence, ferr);
    }
    else {
        eo06_ferror (ferr, Invalid_Handle_ErrText);
    }
    TRCOUT;
}

/*------------------------------*/

externC void
sqlfseekp (
    tsp00_Int4 fileHandle,
    tsp00_Longint distance,
    tsp05_RteSeekKind whence,
    tsp05_RteFileError *ferr)
{
#undef MF__
#define MF__ MOD__"sqlfseekp"
    TRCIN;
    sqlfseekc (fileHandle, distance, whence, ferr);

    eo06_padLen (ferr->sp5fe_text, MAXERRORTEXT);
    TRCOUT;
}

/*------------------------------*/

externC void
sqlflongseekc (
    tsp00_Int4 fileHandle,
    tsp00_Longint distance,
    tsp00_Longint recordCount,
    tsp05_RteSeekKind_Param whence,
    tsp05_RteFileError *ferr)
{
#undef MF__
#define MF__ MOD__"sqlflongseekc"
    FileT* file;
    LargeIntegerT large;
    BOOLEAN calcOK = TRUE;

    TRCIN_P (("handle %d, distance %ld, recordcount %ld, whence %d\n",
              fileHandle, distance, recordCount, whence));
    eo06_clearError (ferr);
    if (OUT_OF_ENUM_INL (whence, sp5vf_seek_begin, sp5vf_seek_end)) {
        eo06_paramError (ferr, "whence");
        TRCOUT;
        return;
    }
    file = eo06_getPointer (&allFilesV, fileHandle);
    if (file != NULL) {
        large = eo06_LONG_X_LONG_2_LARGE_OS (distance, recordCount, &calcOK);
        if (calcOK) {
            eo06_LONGSEEK_V (file, large, whence, ferr);
        }
        else {
            eo06_ferror (ferr, Seek_Range_ErrText);
        }
    }
    else {
        eo06_ferror (ferr, Invalid_Handle_ErrText);
    }
    TRCOUT;
}

/*------------------------------*/

externC void
sqlflongseekp (
    tsp00_Int4 fileHandle,
    tsp00_Longint distance,
    tsp00_Longint recordCount,
    tsp05_RteSeekKind whence,
    tsp05_RteFileError *ferr)
{
#undef MF__
#define MF__ MOD__"sqlflongseekp"
    TRCIN;
    sqlflongseekc (fileHandle, distance, recordCount, whence, ferr);

    eo06_padLen (ferr->sp5fe_text, MAXERRORTEXT);
    TRCOUT;
}

/*------------------------------*/

/**\
--------------------------------------- MF__ MOD__ (sqlftell)
\**/

externC void
sqlftellc (
    tsp00_Int4 fileHandle,
    tsp00_Longint *pos,
    tsp05_RteFileError *ferr)
{
#undef MF__
#define MF__ MOD__"sqlftellc"
    FileT* file;

    TRCIN_P (("handle %d\n", fileHandle));
    eo06_clearError (ferr);
    file = eo06_getPointer (&allFilesV, fileHandle);
    if (file != NULL) {
        if (eo06_VALIDPOS (file)) {
            *pos = file->filePos;
        }
        else {
            eo06_seekError (ferr);
        }
    }
    else {
        eo06_ferror (ferr, Invalid_Handle_ErrText);
    }
    TRCOUT_P (("result pos %ld\n", *pos));
}

/*------------------------------*/

externC void
sqlftellp (
    tsp00_Int4 fileHandle,
    tsp00_Longint *pos,
    tsp05_RteFileError *ferr)
{
#undef MF__
#define MF__ MOD__"sqlftellp"
    TRCIN;
    sqlftellc (fileHandle, pos, ferr);

    eo06_padLen (ferr->sp5fe_text, MAXERRORTEXT);
    TRCOUT;
}

/*------------------------------*/

/**\
--------------------------------------- MF__ MOD__ (sqlftruncate)
\**/

#if defined (_WIN32)
static void
eo06_sqlftruncate (
    FileT *     file,
    tsp00_Longint newSize,
    tsp05_RteFileError *ferr)
{
#undef MF__
#define MF__ MOD__"eo06_sqlftruncate"  /* WIN32 */
  DWORD       seekResult;
  tsp00_Longint oldPos;
#if defined (_WIN64)
   DWORD HighPart;
# endif

  TRCIN;
  eo06_clearError (ferr);
  if (eo06_VALIDPOS (file))
  {
    if ( file->classDesc->seekFunction == eo06_seekUnbuffered )
    {
      oldPos = SetFilePointer (file->osHandle, 0, NULL, FILE_CURRENT);
    }
    else
    {
      oldPos = file->filePos;
    }
#if defined (_WIN64)
    HighPart = (LONG)(newSize >> 32);
    seekResult = SetFilePointer (file->osHandle, (LONG)newSize, &HighPart, FILE_BEGIN);
#else
    seekResult = SetFilePointer (file->osHandle, newSize, NULL, FILE_BEGIN);
#endif

    if (  (oldPos == errorResultC)
       || (seekResult == errorResultC)
       || !SetEndOfFile(file->osHandle) )
    {
        eo06_osError (ferr);
    }
    else
    {
#if defined (_WIN64)
    HighPart = (LONG)(oldPos >> 32);
    seekResult = SetFilePointer (file->osHandle, (LONG)oldPos, &HighPart, FILE_BEGIN);
#else
    seekResult = SetFilePointer (file->osHandle, oldPos, NULL, FILE_BEGIN);
#endif

      if ( seekResult == errorResultC )
      {
        eo06_osError (ferr);
      }
    }
  } else {
    eo06_seekError (ferr);
  }
  TRCOUT;
}

/*------------------------------*/

#else /* UNIX */
static void
eo06_sqlftruncate (
  FileT * file,
  tsp00_Longint newSize,
  tsp05_RteFileError *ferr)
{
#undef MF__
#define MF__ MOD__"eo06_sqlftruncate"  /* UNIX */
    int     rc;

    TRCIN;
    eo06_clearError (ferr);
    rc = ftruncate(file->osHandle, newSize);
    if ( rc < 0 ) {
        eo06_osError (ferr);
    }
    TRCOUT;
}
#endif /* WIN32/UNIX */

/*------------------------------*/

externC void
sqlftruncatec (
    tsp00_Int4  fileHandle,
    tsp00_Longint newSize,
    tsp05_RteFileError *ferr)
{
#undef MF__
#define MF__ MOD__"sqlftruncatec"
    FileT * file;

    TRCIN_P (("handle %d, new size %ld\n",
            fileHandle, newSize));
    file = eo06_getPointer (&allFilesV, fileHandle);

    if ( file != NULL ) {
        eo06_sqlftruncate (file, newSize, ferr);
    }
    else {
        eo06_ferror (ferr, Invalid_Handle_ErrText);
    }
    TRCOUT;
}

/*------------------------------*/

externC void
sqlftruncatep (
  tsp00_Int4  fileHandle,
  tsp00_Longint newSize,
  tsp05_RteFileError *ferr)
{
#undef MF__
#define MF__ MOD__"sqlftruncatep"
    TRCIN;
    sqlftruncatec (fileHandle, newSize, ferr);
    eo06_padLen (ferr->sp5fe_text, MAXERRORTEXT);
    TRCOUT;
}

/**\
--------------------------------------- MF__ MOD__ (sqlferase)
\**/

#if defined (_WIN32)
static void
eo06_sqlferase (
    const char *fname,
    tsp05_RteFileError *ferr)
{
#undef MF__
#define MF__ MOD__"eo06_sqlferase"  /* WIN32 */
    int         rc;
    LONG        attribute;
    ULONG       dummy1;
    ULONG       dummy2;

    TRCIN_P (("filename '%s'\n", fname));
    eo06_clearError (ferr);
    /* check file for regular file. if true delete file */
    rc = sql44c_get_file_info ((char*)fname, &attribute, &dummy1, &dummy2);

    if (( rc == NO_ERROR ) &&
        !(attribute & FILE_DIRECTORY) &&
        !(attribute & FILE_READONLY)) {
        if (!DeleteFile(fname)) {
            eo06_osError (ferr);
        }
    }
    else {
        eo06_ferror (ferr, No_Reg_File_ErrText);
    }
    TRCOUT;
}

/*------------------------------*/

#else

static void
eo06_sqlferase (
    const char *fname,
    tsp05_RteFileError *ferr)
{
#undef MF__
#define MF__ MOD__"eo06_sqlferase"  /* UNIX */
    struct stat         xstat;
    int                 rc;

    TRCIN_P (("filename '%s'\n", fname));
    eo06_clearError (ferr);
    rc = stat (fname, &xstat);
    if (rc != 0) {
        eo06_ferror (ferr, File_Not_Found_ErrText);
        TRCOUT;
        return;
    }
    if (S_ISREG(xstat.st_mode)) {
        rc = unlink (fname);
        if (rc != 0) {
            eo06_ferror (ferr, File_Not_Eraseable_ErrText);
        }
    }
    else {
        eo06_ferror (ferr, No_Reg_File_ErrText);
    }
    TRCOUT;
}
#endif
/*------------------------------*/


externC void
sqlferasec (
    const char *rawFName,
    tsp05_RteFileError *ferr)
{
#undef MF__
#define MF__ MOD__"sqlferasec"
    char nameBuf [EO06_PATH_MAX_C];

    TRCIN;
    eo06_sqlferase (eo06_cookName (nameBuf, sizeof(nameBuf), rawFName, UNDEF), ferr);
    TRCOUT;
}

/*------------------------------*/

externC void
sqlferasep (
    const tsp00_VFilename rawFName,
    tsp05_RteFileError *ferr)
{
#undef MF__
#define MF__ MOD__"sqlferasep"
    int nameLen;
    char trimmedBuf [EO06_PATH_MAX_C];
    char nameBuf [EO06_PATH_MAX_C];

    TRCIN;
    nameLen = eo06_trimLen (rawFName, sizeof (tsp00_VFilename));
    SAPDB_memcpy (trimmedBuf, rawFName, nameLen);
    trimmedBuf [nameLen] = '\0';

    eo06_sqlferase (eo06_cookName (nameBuf, sizeof(nameBuf), trimmedBuf, nameLen), ferr);

    eo06_padLen (ferr->sp5fe_text, MAXERRORTEXT);
    TRCOUT;
}

/*------------------------------*/


/**\
--------------------------------------- MF__ MOD__ (sqlfinfo)
\**/

static void
eo06_fillUnknown (
    tsp05_RteFileInfo *finfo)
{
#undef MF__
#define MF__ MOD__"eo06_fillUnknown"
    finfo->sp5fi_writeable = FALSE;
    finfo->sp5fi_is_link = FALSE;
    finfo->sp5fi_size = 0;
    finfo->sp5fi_size_x = 0;
    finfo->sp5fi_size64 = 0;
    memset (finfo->sp5fi_date_modified, '0', sizeof (tsp00_Date));
    memset (finfo->sp5fi_time_modified, '0', sizeof (tsp00_Time));
    finfo->sp5fi_media_kind = vf_t_unknown;
}

/*------------------------------*/

#if defined (_WIN32)
#define FILE_READ (STANDARD_RIGHTS_READ | FILE_READ_DATA | FILE_READ_ATTRIBUTES | SYNCHRONIZE)
#define FILE_WRITE (STANDARD_RIGHTS_WRITE | FILE_WRITE_DATA | FILE_WRITE_ATTRIBUTES | SYNCHRONIZE)

static void
eo06_fillPrivilegesWin32 (
    tsp05_RteFileInfo *finfo,
    const char* fname)
{
#undef MF__
#define MF__ MOD__"eo06_fillPrivilegesWin32"
#if !defined (USE_SECURITY)
    HANDLE fhandle;

    TRCIN;
    /* try read */
    fhandle = CreateFile (fname, GENERIC_READ, FILE_SHARE_READ,
        NULL, OPEN_EXISTING, 0, 0);
    if (fhandle == invalidHandleC) {
        finfo->sp5fi_readable = FALSE;
    }
    else {
        finfo->sp5fi_readable = TRUE;
        CloseHandle (fhandle);
    }
    /* try write */
    fhandle = CreateFile (fname, GENERIC_WRITE, 0,
        NULL, OPEN_EXISTING, 0, 0);
    if (fhandle == invalidHandleC) {
        finfo->sp5fi_writeable = FALSE;
    }
    else {
        finfo->sp5fi_writeable = TRUE;
        CloseHandle (fhandle);
    }
#else
    BOOLEAN infoOK;
    SECURITY_DESCRIPTOR* securityDesc;
    DWORD lengthRequired;
    HANDLE currentUser = NULL;
    HANDLE impersonator = NULL;
    GENERIC_MAPPING genericMapping;
    PRIVILEGE_SET privilegeSet;
    DWORD setLength;
    DWORD grantedAccess = 0;
    BOOLEAN accessStatus = FALSE;
    DWORD rc;

    TRCIN;
    finfo->sp5fi_readable = FALSE;
    finfo->sp5fi_writeable = FALSE;
    infoOK = GetFileSecurity(fname, DACL_SECURITY_INFORMATION, NULL, 0, &lengthRequired);
    securityDesc = (SECURITY_DESCRIPTOR*) GlobalAlloc (GPTR, lengthRequired);
    infoOK = GetFileSecurity(fname, DACL_SECURITY_INFORMATION, securityDesc,
        lengthRequired, &lengthRequired);

    if (infoOK) {
        infoOK =ImpersonateSelf (SecurityImpersonation);
        infoOK = OpenThreadToken (GetCurrentThread(), TOKEN_IMPERSONATE | TOKEN_QUERY,
            FALSE, &currentUser);

    }
    if (infoOK) {
        IsValidSecurityDescriptor (securityDesc);
        rc = GetLastError();
        setLength = sizeof (privilegeSet);
        infoOK = AccessCheck(securityDesc, currentUser,
            FILE_READ /*| GENERIC_WRITE*/, &genericMapping, &privilegeSet,
            &setLength, &grantedAccess, &accessStatus);
    }
    if (!infoOK) {
        rc = GetLastError ();
    }
    RevertToSelf();
    CloseHandle (impersonator);
    CloseHandle (currentUser);
    GlobalFree (securityDesc);
#endif
    TRCOUT;
}

/*------------------------------*/

const DWORD halfMAXDWORD = (MAXDWORD / 2) ;

static void
eo06_fillSizeWin32 (
    tsp05_RteFileInfo *finfo,
    WIN32_FIND_DATA* ntInfo)
{
#undef MF__
#define MF__ MOD__"eo06_fillSizeWin32"
    int overflow;

    TRCIN;
    if (ntInfo->nFileSizeLow <= halfMAXDWORD) {
        finfo->sp5fi_size = ntInfo->nFileSizeLow;
        overflow = 0;
    }
    else {
        finfo->sp5fi_size = ntInfo->nFileSizeLow - halfMAXDWORD;
        overflow = 1;
    }

    /* PTS 1105322 */

    finfo->sp5fi_size_x = ( ntInfo->nFileSizeHigh * 2 ) + overflow;

    finfo->sp5fi_size64 = MAXDWORD ;
    finfo->sp5fi_size64 ++ ;
    finfo->sp5fi_size64 *= ntInfo->nFileSizeHigh ;
    finfo->sp5fi_size64 += ntInfo->nFileSizeLow ;

    TRCOUT;
}

/*------------------------------*/

static void
eo06_fillTimeWin32 (
    tsp05_RteFileInfo *finfo,
    FILETIME ftime)
{
#undef MF__
#define MF__ MOD__"eo06_fillTimeWin32"
    FILETIME localTime;
    SYSTEMTIME systemTime;

    TRCIN;
    FileTimeToLocalFileTime (&ftime, &localTime);
    FileTimeToSystemTime (&localTime, &systemTime);
    memset (finfo->sp5fi_date_modified, ' ', sizeof (tsp00_Date));
    memset (finfo->sp5fi_time_modified, ' ', sizeof (tsp00_Time));

    eo06_putInt (finfo->sp5fi_date_modified, systemTime.wYear, 4);
    eo06_putInt (finfo->sp5fi_date_modified + 4, systemTime.wMonth, 2);
    eo06_putInt (finfo->sp5fi_date_modified + 6, systemTime.wDay, 2);

    eo06_putInt (finfo->sp5fi_time_modified, systemTime.wHour, 2);
    eo06_putInt (finfo->sp5fi_time_modified + 2, systemTime.wMinute, 2);
    eo06_putInt (finfo->sp5fi_time_modified + 4, systemTime.wSecond, 2);
    TRCOUT;
}

/*------------------------------*/

static tsp00_VfType FileType_os2sp[] = {
  vf_t_unknown,         /* DT_UNKNOWN   */
  vf_t_raw,             /* DT_CONOUT    */
  vf_t_raw,             /* DT_CONIN     */
  vf_t_raw,             /* DT_PRN       */
  vf_t_raw,             /* DT_LPTx      */
  vf_t_raw,             /* DT_COMx      */
  vf_t_file,            /* DT_OTHER     */
  vf_t_tape_norew,      /* DT_TAPE      */
  vf_t_raw,             /* DT_DRIVE     */
  vf_t_pipe,            /* DT_PIPE      */
  vf_t_raw              /* DT_NUL       */
};

static void
eo06_fillMediaWin32 (
    tsp05_RteFileInfo *finfo,
    DWORD attributes,
    const char* fname)
{
#undef MF__
#define MF__ MOD__"eo06_fillMediaWin32"
    TRCIN;
    if (attributes & FILE_ATTRIBUTE_DIRECTORY)
        finfo->sp5fi_media_kind = vf_t_directory;
    else {
        finfo->sp5fi_media_kind = FileType_os2sp [
            sql44c_get_dev_type_by_filename ((char*)fname)];
    }
    TRCOUT;
}

/*------------------------------*/

static void
eo06_sqlfinfo (
    const char *fname,
    tsp05_RteFileInfo *finfo,
    tsp05_RteFileError *ferr)
{
#undef MF__
#define MF__ MOD__"eo06_sqlfinfo"  /* WIN32 */
    HANDLE fhandle;
    WIN32_FIND_DATA ntInfo;

    TRCIN;
    eo06_clearError (ferr);
    fhandle = FindFirstFile (fname, &ntInfo);
    if (fhandle == INVALID_HANDLE_VALUE) {
        finfo->sp5fi_exists = FALSE;
        eo06_fillUnknown (finfo);
    }
    else {
        finfo->sp5fi_exists = TRUE;
        /* is link */
        finfo->sp5fi_is_link = FALSE;
        /* privileges */
        eo06_fillPrivilegesWin32 (finfo, fname);
        /* size */
        eo06_fillSizeWin32 (finfo, &ntInfo);
        /* date */
        eo06_fillTimeWin32 (finfo, ntInfo.ftLastWriteTime);
        /* media */
        eo06_fillMediaWin32 (finfo, ntInfo.dwFileAttributes, fname);
        FindClose (fhandle);
    }
    TRCOUT;
}

/*------------------------------*/

#else
static void
eo06_fillPrivilegesUnix (
    tsp05_RteFileInfo *finfo,
    struct stat *xstat)
{
#undef MF__
#define MF__ MOD__"eo06_fillPrivilegesUnix"
    unsigned short userID;
    unsigned short groupID;

    TRCIN;
    userID = geteuid ();
    groupID = getegid ();

    if ((userID == xstat->st_uid) && (xstat->st_mode & S_IRUSR)) {
        finfo->sp5fi_readable = TRUE;
    }
    else if ((groupID == xstat->st_gid) && (xstat->st_mode & S_IRGRP)) {
        finfo->sp5fi_readable = TRUE;
    }
    else if (xstat->st_mode & S_IROTH) {
        finfo->sp5fi_readable = TRUE;
    }
    else {
        finfo->sp5fi_readable = FALSE;
    }

    if ((userID == xstat->st_uid) && (xstat->st_mode & S_IWUSR)) {
        finfo->sp5fi_writeable = TRUE;
    }
    else if ((groupID == xstat->st_gid) && (xstat->st_mode & S_IWGRP)) {
        finfo->sp5fi_writeable = TRUE;
    }
    else if (xstat->st_mode & S_IWOTH) {
        finfo->sp5fi_writeable = TRUE;
    }
    else {
        finfo->sp5fi_writeable = FALSE;
    }
    TRCOUT;
}

/*------------------------------*/

static void
eo06_fillTimeUnix (
    tsp05_RteFileInfo *finfo,
    time_t ftime)
{
#undef MF__
#define MF__ MOD__"eo06_fillTimeUnix"
#ifdef _REENTRANT
    struct tm  tm_buffer;
#endif
    struct tm* tmbuf;

    TRCIN;
#ifdef _REENTRANT
    tmbuf = localtime_r (&ftime, &tm_buffer);
#else
    tmbuf = localtime (&ftime);
#endif
    memset (finfo->sp5fi_date_modified, ' ', sizeof (tsp00_Date));
    memset (finfo->sp5fi_time_modified, ' ', sizeof (tsp00_Time));

    eo06_putInt (finfo->sp5fi_date_modified, tmbuf->tm_year + 1900, 4);
    eo06_putInt (finfo->sp5fi_date_modified + 4, tmbuf->tm_mon + 1, 2);
    eo06_putInt (finfo->sp5fi_date_modified + 6, tmbuf->tm_mday, 2);

    eo06_putInt (finfo->sp5fi_time_modified, tmbuf->tm_hour, 2);   /* PTS 1104738 */
    eo06_putInt (finfo->sp5fi_time_modified + 2, tmbuf->tm_min, 2);
    eo06_putInt (finfo->sp5fi_time_modified + 4, tmbuf->tm_sec, 2);
    TRCOUT;
}

/*------------------------------*/

static void
eo06_fillDirectoryInfoUnix (
    tsp05_RteFileInfo *finfo)
{
#undef MF__
#define MF__ MOD__"eo06_fillDirectoryInfoUnix"
    finfo->sp5fi_size_x = 0;
}

/*------------------------------*/

static void /*ARGSUSED*/
eo06_fillMediaUnix (
    tsp05_RteFileInfo *finfo,
    const char* fname,
    struct stat *xstat)
{
#undef MF__
#define MF__ MOD__"eo06_fillMediaUnix"
    TRCIN;
    if (xstat->st_mode & S_IFDIR) {
        finfo->sp5fi_media_kind = vf_t_directory;
        eo06_fillDirectoryInfoUnix (finfo);
    }
    else if ((xstat->st_mode & S_IFBLK) || (xstat->st_mode & S_IFCHR)) {
        finfo->sp5fi_media_kind = vf_t_raw;
    }
    else if (xstat->st_mode & S_IFIFO) {
        finfo->sp5fi_media_kind = vf_t_pipe;
    }
    else if (xstat->st_mode & S_IFREG) {
        finfo->sp5fi_media_kind = vf_t_file;
    }
    else {
        finfo->sp5fi_media_kind = vf_t_unknown;
    }
    TRCOUT;
}

/*------------------------------*/

void
eo06_sqlfinfo (
    const char *fname,
    tsp05_RteFileInfo *finfo,
    tsp05_RteFileError *ferr)
{
#undef MF__
#define MF__ MOD__"eo06_sqlfinfo"  /* UNIX */
    int         rc;
    struct stat xstat;

    TRCIN;
    eo06_clearError (ferr);
    rc = eo06_lstat (fname, &xstat);
    if (rc != 0) {
        finfo->sp5fi_exists = FALSE;
        eo06_fillUnknown (finfo);
        /* check errno to set ferr */
    }
    else {
        finfo->sp5fi_exists = TRUE;
        /* is link */
        if (eo06_ISLINK_OS(xstat)) {
            finfo->sp5fi_is_link = TRUE;
            stat (fname, &xstat);
        }
        else
            finfo->sp5fi_is_link = FALSE;
        /* privileges */
        eo06_fillPrivilegesUnix (finfo, &xstat);
        /* size */
        finfo->sp5fi_size = xstat.st_size;
        finfo->sp5fi_size_x = 0;
        finfo->sp5fi_size64 = xstat.st_size ;
        /* date */
        eo06_fillTimeUnix (finfo, xstat.st_mtime);
        /* media */
        eo06_fillMediaUnix (finfo, fname, &xstat);
    }
    TRCOUT;
}
#endif
/*------------------------------*/


externC void
sqlfinfoc (
    const char *rawFName,
    tsp05_RteFileInfo *finfo,
    tsp05_RteFileError *ferr)
{
#undef MF__
#define MF__ MOD__"sqlfinfoc"
    char nameBuf [EO06_PATH_MAX_C];

    TRCIN;
    eo06_sqlfinfo (eo06_cookName (nameBuf, sizeof(nameBuf), rawFName, UNDEF), finfo, ferr);
    TRCOUT;
}

/*------------------------------*/

externC void
sqlfinfop (
    const tsp00_VFilename rawFName,
    tsp05_RteFileInfo *finfo,
    tsp05_RteFileError *ferr)
{
#undef MF__
#define MF__ MOD__"sqlfinfop"
    int nameLen;
    char trimmedBuf [EO06_PATH_MAX_C];
    char nameBuf [EO06_PATH_MAX_C];

    TRCIN;
    nameLen = eo06_trimLen (rawFName, sizeof (tsp00_VFilename));
    SAPDB_memcpy (trimmedBuf, rawFName, nameLen);
    trimmedBuf [nameLen] = '\0';

    eo06_sqlfinfo (eo06_cookName (nameBuf, sizeof(nameBuf), trimmedBuf, nameLen), finfo, ferr);

    eo06_padLen (ferr->sp5fe_text, MAXERRORTEXT);
    TRCOUT;
}

/*------------------------------*/

/**\
--------------------------------------- MF__ MOD__ (sqlffinish)
\**/

externC void
sqlffinishc (void)
{
#undef MF__
#define MF__ MOD__"sqlffinishc"
    tsp00_Int4 handle;
    tsp05_RteFileError err;

    TRCIN;
    handle = UNDEF;
    while (eo06_nextUsedHandle (&allFilesV, &handle)) {
        sqlfclosec (handle, sp5vf_close_normal, &err);
    }
    TRCOUT;
}

/*------------------------------*/

externC void
sqlffinishp (void)
{
#undef MF__
#define MF__ MOD__"sqlffinishp"
    TRCIN;
    sqlffinishc ();
    TRCOUT;
}

/*------------------------------*/

/**\
--------------------------------------- MF__ MOD__ (NT specifics)
\**/

#if defined (_WIN32)

 #define TAPE_ERROR_RETRY                          2
 #define MAX_TAPE_LOAD_RETRY_TIME                 60

 #if defined (KERNEL)
  #define  SECURITY_ATTR        &kgs.AdminSA
 #else
  #define  SECURITY_ATTR        NULL
 #endif

typedef struct StdHandleMapT {
    const char * name;
    int          handleKind;
} StdHandleMapT;

static BOOLEAN
eo06_tryStdWin32 (
    const char* fname,
    tsp05_RteFileMode_Param fileMode,
    FileT* file,
    tsp05_RteFileError *ferr)
{
#undef MF__
#define MF__ MOD__"eo06_tryStdWin32"
    static StdHandleMapT readHandles [] = {
        {"-", STD_INPUT_HANDLE},
        {"stdin", STD_INPUT_HANDLE},
        {NULL, -1}
    };
    static StdHandleMapT writeHandles [] = {
        {"-", STD_OUTPUT_HANDLE},
        {"stdout", STD_OUTPUT_HANDLE},
        {"stderr", STD_ERROR_HANDLE},
        {NULL, -1}
    };
    StdHandleMapT     * current;

    if (fileMode == sp5vf_read) {
        current = &readHandles [0];
    }
    else {
        current = &writeHandles [0];
    }
    while (current->name != NULL) {
        if (stricmp (current->name, fname) == 0) {
            file->osHandle = GetStdHandle (current->handleKind);
            if (file->osHandle == INVALID_HANDLE_VALUE) {
                eo06_osError (ferr);
            }
            file->isStdHandle = TRUE;
            return TRUE;
        }
        ++current;
    }
    /* no match found */
    return FALSE;
}

/*------------------------------*/

static BOOLEAN
eo06_fileOpenWin32 (
    const char* fname,
    tsp05_RteFileMode_Param fileMode,
    tsp05_RteBufferingKind_Param buffering,
    FileT* file,
    tsp05_RteFileError *ferr)
{
#undef MF__
#define MF__ MOD__"eo06_fileOpenWin32"
    int                 openFlags = 0;
    DWORD               accessMode;
    DWORD               shareMode;
    DWORD               creationMode;
    DWORD               attributes;
    BOOLEAN             reading;

    TRCIN;
    if (eo06_tryStdWin32 (fname, fileMode, file, ferr)) {
        return (file->osHandle != INVALID_HANDLE_VALUE);
    }
    switch (fileMode) {
        case sp5vf_read:
            accessMode = GENERIC_READ;
            shareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
            creationMode = OPEN_EXISTING;
            reading = TRUE;
            break;
        case sp5vf_write:
            accessMode = GENERIC_WRITE;
            shareMode = FILE_SHARE_READ;
            creationMode = CREATE_ALWAYS;
            reading = FALSE;
            break;
        case sp5vf_readwrite:
            accessMode = GENERIC_READ | GENERIC_WRITE ;
            shareMode = FILE_SHARE_READ;
            creationMode = OPEN_ALWAYS;
            reading = FALSE;
            break;
        case sp5vf_append:
            accessMode = GENERIC_WRITE;
            shareMode = FILE_SHARE_READ;
            creationMode = OPEN_ALWAYS;
            reading = FALSE;
            break;
        default:
            SWITCH_ERROR_DBG ("Invalid file mode", fileMode);
            break;
    }
    if (sp5bk_disksynced == buffering)
        attributes = FILE_FLAG_WRITE_THROUGH;
    else
        attributes = 0;
    file->lDevType = sql44c_get_dev_type_by_filename ((char*)fname);
    switch (file->lDevType) {
        case DT_OTHER:
        case DT_NUL:
            file->osHandle = CreateFile (fname, accessMode, shareMode, 0,
                creationMode, attributes, NULL);
            if (file->osHandle == invalidHandleC) {
                if ( GetLastError() == EACCES )
                {
                  ferr->sp5fe_warning = sp5vfw_already_locked;
                }
                eo06_osError (ferr);
            }
            if ((file->osHandle != invalidHandleC) && (fileMode == sp5vf_append)) {
                SetFilePointer (file->osHandle, 0, NULL, FILE_END);
            }
            break;
        case DT_TAPE:
            eo06c_tape_open (fname, file, ferr, reading);
            break;
        case DT_PIPE:
            if (reading) {
                eo06c_pipe_read_open (fname, file, ferr);
            }
            else {
                eo06c_pipe_write_open (fname, file, ferr);
            }
            break;
        case DT_CON:
            if (!reading) {
                eo06c_con_open (fname, file, ferr);
            }
            else {
                /* error */
            }
            break;
        case DT_CONIN:
            if (reading) {
                eo06c_con_open (fname, file, ferr);
            }
            else {
                /* error */
            }
            break;
        default:
            SWITCH_ERROR_DBG ("invalid device type %d", file->lDevType);
            break;
    }
    TRCOUT;
    return (file->osHandle != invalidHandleC);
}

/*------------------------------*/

static void
eo06_fileClose (
    FileT* file,
    tsp05_RteCloseOption_Param option,
    tsp05_RteFileError *ferr)
{
#undef MF__
#define MF__ MOD__"eo06_fileClose"   /* WIN32 */
    tsp05_RteFileError  dummyError;
    tsp05_RteFileError* usedferror = ferr;
    BOOLEAN             ok;

    if (file->lDevType == DT_TAPE ) {
        eo06_clearError (&dummyError);
        if (eo06_ISWRITER_V (file)) {
            eo06c_tape_write_mark (file, ferr);
        }
        if (ferr->sp5fe_result != 0) {
            usedferror = &dummyError;
        }

        if ((option == sp5vf_close_rewind) || (option == sp5vf_close_next_tape)) {
            eo06c_tape_rewind(file, usedferror );
        }
        if (ferr->sp5fe_result != 0) {
            usedferror = &dummyError;
        }

        eo06c_tape_locking(file, usedferror, TAPE_UNLOCK);
        if ((ferr->sp5fe_result == 0) && (option == sp5vf_close_next_tape)) {
            /* unload tape so next open loads new tape */
            eo06c_tape_loading(file, &dummyError , TAPE_UNLOAD);
            if (dummyError.sp5fe_result != 0) {
                ferr->sp5fe_warning = sp5vfw_no_next_tape;
            }
        }
    }
    else if (file->lDevType == DT_PIPE ) {
        FlushFileBuffers(file->osHandle);
    }
    ok = CloseHandle (file->osHandle);
    if (!ok) {
        eo06_osError (usedferror);
    }
}

/*------------------------------*/

  #undef  MF__
  #define MF__ MOD__"eo06c_tape_open"
static LONG eo06c_tape_open (
    const char*         tapeName,
    FileT*              file,
    tsp05_RteFileError*  ferr,
    BOOLEAN             reading)
{
    APIRET             rc = NO_ERROR;
    LONG               lError;
    TRCIN;

    DBG3 ((MF__, "Open tape '%s'", tapeName ));

    file->osHandle = CreateFile( tapeName,
                                          GENERIC_READ | GENERIC_WRITE,
                                          0,
                                          SECURITY_ATTR,
                                          OPEN_EXISTING,
                                          0,
                                          NULL);
    if ( file->osHandle == INVALID_HANDLE_VALUE ) {
        rc = GetLastError();
        ferr->sp5fe_result = VF_NOTOK;
        file->osHandle = INVALID_HANDLE_VALUE;
        if (sql06c_handle_known_errors (tapeName, rc, (char *)ferr->sp5fe_text) == FALSE ) {
            DBG1 (( MF__, "Cannot open tape '%s', rc = %d", tapeName, rc    ));
            MSGD (( ERR_OPEN, tapeName, rc                                  ));
            eo46BuildPascalErrorStringRC (ferr->sp5fe_text, ERRMSG_VF_OPEN,  rc  );
        }
     }
   else /* -- no error */ {
       eo06c_init_dlt_tape (file);

       if ( ((lError = eo06c_get_drive_info(file, ferr)) == VF_OK )
           && ((lError = eo06c_tape_loading  (file, ferr, TAPE_LOAD))
                                                               == VF_OK )
           && ((lError = eo06c_tape_locking  (file, ferr, TAPE_LOCK))
                                                               == VF_OK )
           && ((lError = eo06c_get_media_info(file, ferr)) == VF_OK )
           && ((lError = eo06c_init_tape_dev (file, ferr)) == VF_OK ))
           {
             /* OK */
           }

        if (lError == VF_NOTOK) {
            eo06c_tape_locking(file, ferr , TAPE_UNLOCK);
        }
    }

    DBG3 (( MF__, "lError              %d", lError ));

    TRCOUT;
    return (ferr->sp5fe_result);
}

/*------------------------------*/

  #undef  MF__
  #define MF__ MOD__"eo06c_con_open"
static LONG eo06c_con_open (
    const char*                 fname,
    FileT                       *file,
    tsp05_RteFileError*          ferr)
{
    APIRET      rc     = NO_ERROR;

    TRCIN;

    if (file->lDevType == DT_CONIN ) {
        file->osHandle = GetStdHandle(STD_INPUT_HANDLE);
    }
    else {
        file->osHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    }

    if (file->osHandle == INVALID_HANDLE_VALUE ) {
        rc                = GetLastError();
        ferr->sp5fe_result = VF_NOTOK;
        file->osHandle    = INVALID_HANDLE_VALUE;

        DBG1 (( MF__, "Cannot open file '%s', rc = %d", fname, rc));
        MSGD (( ERR_OPEN, fname, rc ));
        eo46BuildPascalErrorStringRC (ferr->sp5fe_text, ERRMSG_VF_OPEN, rc);
    }

    TRCOUT;
    return (ferr->sp5fe_result);
}

/*------------------------------*/

  #undef  MF__
  #define MF__ MOD__"eo06c_pipe_write_open"
static LONG eo06c_pipe_write_open (
    const char*                 fname,
    FileT*                      file,
    tsp05_RteFileError*         ferr)
{
      APIRET       rc      = NO_ERROR;
      PATHNAME     szPhysFileName;
      SAPDB_UInt4  sizeOfPhysFileName = sizeof(szPhysFileName);
      ULONG        ulPipeBuffSize;

      TRCIN;

      /* We don't check the return code here, on error the output buffer becomes the 
         input string. */
      RTE_ExpandEnvVars ( fname, szPhysFileName, &sizeOfPhysFileName );

      ulPipeBuffSize = KBYTE_64;
      file->osHandle = CreateNamedPipe( szPhysFileName,
                                               PIPE_ACCESS_OUTBOUND,
                                               PIPE_TYPE_BYTE
                                               | PIPE_READMODE_BYTE
                                               | PIPE_WAIT,
                                               1,
                                               ulPipeBuffSize,
                                               ulPipeBuffSize,
                                               INFINITE,
                                               NULL);

    if (file->osHandle == INVALID_HANDLE_VALUE) {
        rc = GetLastError();
    }

    if (rc == NO_ERROR) {
        if (!ConnectNamedPipe(file->osHandle, NULL)) {
            rc = GetLastError();
            CloseHandle(file->osHandle);
        }
    }
    else if (rc == ERROR_PIPE_BUSY) {
        file->osHandle = CreateFile(szPhysFileName,
                                            GENERIC_WRITE,
                                            FILE_SHARE_READ,
                                            NULL,
                                            OPEN_EXISTING,
                                            FILE_ATTRIBUTE_NORMAL,
                                            NULL );

        if (file->osHandle == INVALID_HANDLE_VALUE ) {
            rc = GetLastError();
        }
        else {
            rc = NO_ERROR;
        }
    }

    if ( rc != NO_ERROR ) {
        ferr->sp5fe_result = VF_NOTOK;
        file->osHandle = INVALID_HANDLE_VALUE;

        if ( sql06c_handle_known_errors ( fname,
                                      rc , ferr->sp5fe_text ) == FALSE )
        {
            DBG1 (( MF__, "Cannot open file '%s', rc = %d", fname, rc ));
            MSGD (( ERR_OPEN, fname, rc                               ));
            eo46BuildPascalErrorStringRC (ferr->sp5fe_text, ERRMSG_VF_OPEN,  rc );
        }
    }
    else { /* -- no error */
        DBG3 ((MF__, "Open file '%s'", fname));
    }

    DBG3 (( MF__, "lError              %d", ferr->sp5fe_result));

    TRCOUT;
    return (ferr->sp5fe_result);
}

/*------------------------------*/

/*------------------------------*/

  #undef  MF__
  #define MF__ MOD__"eo06c_pipe_read_open"
static LONG eo06c_pipe_read_open (const  char*  hostFileName,
                             FileT  *file,
                             tsp05_RteFileError        *ferr)
  {
  APIRET       rc     = NO_ERROR;
  LONG         lError = VF_OK;
  PATHNAME     szPhysFileName;
  SAPDB_UInt4  sizeOfPhysFileName = sizeof(szPhysFileName);
  ULONG        ulPipeBuffSize     = sizeof(VF_BLOCK) * 2;


    TRCIN;

   /* We don't check the return code here, on error the output buffer becomes the 
      input string. */
   RTE_ExpandEnvVars ( hostFileName, szPhysFileName, &sizeOfPhysFileName );

   file->osHandle = CreateNamedPipe( szPhysFileName,
                                               PIPE_ACCESS_INBOUND,
                                               PIPE_TYPE_BYTE     |
                                               PIPE_READMODE_BYTE |
                                               PIPE_WAIT,
                                               1,
                                               ulPipeBuffSize,
                                               ulPipeBuffSize,
                                               INFINITE,
                                               NULL);

   if (file->osHandle == INVALID_HANDLE_VALUE )
     rc = GetLastError();

   if (rc == NO_ERROR)
     {
     if (!ConnectNamedPipe(file->osHandle, NULL))
       {
       rc = GetLastError();
       CloseHandle(file->osHandle);
       }
     }
   else if (rc == ERROR_PIPE_BUSY)
     {
     file->osHandle = CreateFile( szPhysFileName,
                                            GENERIC_READ,
                                            FILE_SHARE_WRITE,
                                            NULL,
                                            OPEN_EXISTING,
                                            FILE_ATTRIBUTE_NORMAL,
                                            NULL );

     if (file->osHandle == INVALID_HANDLE_VALUE )
       rc = GetLastError();
     else
       rc = NO_ERROR;
     }

  if ( rc != NO_ERROR )
    {
    lError            = VF_NOTOK;
    file->osHandle = INVALID_HANDLE_VALUE;

    if ( sql06c_handle_known_errors ( hostFileName,
                                      rc , ferr->sp5fe_text ) == FALSE )
      {
      DBG1 (( MF__, "Cannot open file '%s', rc = %d", hostFileName, rc ));
      MSGD (( ERR_OPEN, hostFileName, rc                               ));
      eo46BuildPascalErrorStringRC ( ferr->sp5fe_text, ERRMSG_VF_OPEN, rc  );
      }
    }
  else /* -- no error */
    {
    }

  TRCOUT;
  return (lError);
  }

/*------------------------------*/

  #undef  MF__
  #define MF__ MOD__"eo06c_init_dlt_tape"
static LONG eo06c_init_dlt_tape ( FileT  *file )
{

  APIRET                     rc             = NO_ERROR;
  INT                        Retry          = TAPE_ERROR_RETRY;

  TRCIN;

  /*
   * --- try to load tape.
   *     it's needed by some tape drives (DLT) as first operation
   *     Daniel 28.02.97: changed to TAPE_LOCK as
   *            TAPE_LOAD rewinds tape
   */
  do
    {
    rc = PrepareTape ( file->osHandle, TAPE_LOCK , FALSE);
    }
  while (( rc == ERROR_MEDIA_CHANGED        ||
           rc == ERROR_END_OF_MEDIA         ||
           rc == ERROR_BEGINNING_OF_MEDIA   ||
           rc == ERROR_IO_DEVICE            ||
           rc == ERROR_BUS_RESET            ) && --Retry);
  /*
   * --- no error handling, ignore all errors
   */

  TRCOUT;
  return ( TRUE );
  }

/*------------------------------*/

  #undef  MF__
  #define MF__ MOD__"eo06c_get_drive_info"
static LONG eo06c_get_drive_info ( FileT  *file,
                                    tsp05_RteFileError        *ferr)
  {

   APIRET                     rc         = NO_ERROR;
   LONG                       lError     = VF_OK;
   INT                        Retry      = TAPE_ERROR_RETRY;
   TAPE_GET_DRIVE_PARAMETERS  DriveInfo;
   DWORD                      dwInfoSize;

   TRCIN;

   /*
    *  --- get tape device information
    */
   do
     {
     dwInfoSize  = sizeof ( DriveInfo );
     memset ( &DriveInfo, 0, sizeof(DriveInfo));

     rc = GetTapeParameters( file->osHandle,
                             GET_TAPE_DRIVE_INFORMATION,
                             &dwInfoSize, &DriveInfo );
     }
   while (( rc == ERROR_BUS_RESET          ||
            rc == ERROR_IO_DEVICE          ||
            rc == ERROR_END_OF_MEDIA       ||
            rc == ERROR_BEGINNING_OF_MEDIA ||
            rc == ERROR_MEDIA_CHANGED )    && --Retry);

   if (( rc != NO_ERROR )                 &&
       ( rc != ERROR_MEDIA_CHANGED )      &&
       ( rc != ERROR_END_OF_MEDIA )       &&
       ( rc != ERROR_BEGINNING_OF_MEDIA ) &&
       ( rc != ERROR_INVALID_FUNCTION ))
     {
     lError            = VF_NOTOK;

     if ( sql06c_handle_known_errors ( "unknown",
                                       rc , ferr->sp5fe_text ) == FALSE )
       {
       DBG1 (( MF__, ERRMSG_VF_TAPE_PARAM ));
       MSGD (( ERR_TAPE_PARAM, rc         ));
       eo46BuildPascalErrorStringRC ( ferr->sp5fe_text, ERRMSG_VF_TAPE_PARAM, rc );
       }

     TRCOUT;
     return (lError);
     }
   else if ( rc != ERROR_INVALID_FUNCTION )
     {
     file->ulMaxTapeBlkSize = DriveInfo.MaximumBlockSize;
     file->fECC             = DriveInfo.ECC;
     file->fCompression     = DriveInfo.Compression;
     file->fDataPadding     = DriveInfo.DataPadding;
     file->ulTapeFeatures   = 0;

     if (( DriveInfo.FeaturesHigh & TAPE_DRIVE_SET_COMPRESSION ) &&
         ( DriveInfo.FeaturesLow  & TAPE_DRIVE_COMPRESSION ))
       file->ulTapeFeatures |= TAPE_SET_COMPRESSION;
     if (( DriveInfo.FeaturesHigh & TAPE_DRIVE_SET_ECC ) &&
         ( DriveInfo.FeaturesLow  & TAPE_DRIVE_ECC ))
       file->ulTapeFeatures |= TAPE_SET_ECC;
     if ( DriveInfo.FeaturesHigh & TAPE_DRIVE_SET_BLOCK_SIZE )
       file->ulTapeFeatures |= TAPE_SET_BLOCK_SIZE;
     if ( DriveInfo.FeaturesHigh & TAPE_DRIVE_LOCK_UNLOCK )
       file->ulTapeFeatures |= TAPE_LOCK_UNLOCK;
     if ( DriveInfo.FeaturesHigh & TAPE_DRIVE_LOAD_UNLOAD )
       file->ulTapeFeatures |= TAPE_LOAD_UNLOAD;
     if ( DriveInfo.FeaturesHigh & TAPE_DRIVE_REPORT_SMKS )
       file->ulTapeFeatures |= TAPE_REPORT_SMKS;
     if ( DriveInfo.FeaturesHigh & TAPE_DRIVE_WRITE_LONG_FMKS )
       file->ulTapeFeatures |= TAPE_WRITE_LONG_FMKS;
     if ( DriveInfo.FeaturesHigh & TAPE_DRIVE_WRITE_SHORT_FMKS )
       file->ulTapeFeatures |= TAPE_WRITE_SHORT_FMKS;
     if ( DriveInfo.FeaturesHigh & TAPE_DRIVE_WRITE_FILEMARKS )
       file->ulTapeFeatures |= TAPE_WRITE_FILEMARKS;
     if ( DriveInfo.FeaturesHigh & TAPE_DRIVE_WRITE_MARK_IMMED )
       file->ulTapeFeatures |= TAPE_WRITE_MARK_IMMED;
     }

   TRCOUT;
   return (lError);
  }

/*------------------------------*/

  #undef  MF__
  #define MF__ MOD__"eo06c_get_media_info"
static LONG eo06c_get_media_info ( FileT  *file,
                                    tsp05_RteFileError        *ferr)
  {

   APIRET                     rc         = NO_ERROR;
   LONG                       lError     = VF_OK;
   INT                        Retry      = TAPE_ERROR_RETRY;
   TAPE_GET_MEDIA_PARAMETERS  MediaInfo;
   DWORD                      dwInfoSize;

   TRCIN;

   do
     {
     /*
      *  --- get media information
      */
     dwInfoSize  = sizeof ( MediaInfo );
     memset ( &MediaInfo, 0, sizeof(MediaInfo));

     rc = GetTapeParameters( file->osHandle,
                             GET_TAPE_MEDIA_INFORMATION,
                             &dwInfoSize, &MediaInfo );
     }
   while (( rc == ERROR_BUS_RESET            ||
            rc == ERROR_IO_DEVICE            ||
            rc == ERROR_END_OF_MEDIA         ||
            rc == ERROR_BEGINNING_OF_MEDIA   ||
            rc == ERROR_MEDIA_CHANGED ) && --Retry);

   if (( rc != NO_ERROR )                 &&
       ( rc != ERROR_MEDIA_CHANGED )      &&
       ( rc != ERROR_END_OF_MEDIA )       &&
       ( rc != ERROR_BEGINNING_OF_MEDIA ) &&
       ( rc != ERROR_INVALID_FUNCTION ))
     {
     lError            = VF_NOTOK;

     if ( sql06c_handle_known_errors ( "unknown",
                                       rc , ferr->sp5fe_text ) == FALSE )
       {
       DBG1 (( MF__, ERRMSG_VF_TAPE_PARAM ));
       MSGD (( ERR_TAPE_PARAM, rc         ));
       eo46BuildPascalErrorStringRC ( ferr->sp5fe_text, ERRMSG_VF_TAPE_PARAM, rc );
       }
     TRCOUT;
     return (lError);
     }
   else if ( rc != ERROR_INVALID_FUNCTION )
     {
     if ( MediaInfo.PartitionCount > 1)
       MSGD (( INFO_TAPE_WRONG_PART_CNT ));
     }

   TRCOUT;
   return (lError);
  }

/*------------------------------*/

  #undef  MF__
  #define MF__ MOD__"eo06c_init_tape_dev"
static LONG eo06c_init_tape_dev( FileT *file,
                                  tsp05_RteFileError        *ferr)
  {

   APIRET                     rc         = NO_ERROR;
   LONG                       lError     = VF_OK;
   INT                        Retry      = TAPE_ERROR_RETRY;
   TAPE_SET_DRIVE_PARAMETERS  DriveInfo;
   TAPE_SET_MEDIA_PARAMETERS  MediaInfo;
   char*                        TapeCompressEnabled;

   TRCIN;

   DriveInfo.ECC                = file->fECC;
   DriveInfo.Compression        = file->fCompression;
   DriveInfo.DataPadding        = file->fDataPadding;
   DriveInfo.ReportSetmarks     = FALSE;
   DriveInfo.EOTWarningZoneSize = 0;

   if ( file->ulTapeFeatures & TAPE_SET_COMPRESSION )
     {
     rc = GETENV ( TAPE_COMPRESS_ENV_VAR, &TapeCompressEnabled );

     if (( rc == NO_ERROR )  && ( *TapeCompressEnabled )  &&
         ( ! strcmp ( "OFF", strupr ( TapeCompressEnabled ))))
       DriveInfo.Compression  = FALSE;
     else
       DriveInfo.Compression  = TRUE;
     }

   if ( file->ulTapeFeatures & TAPE_SET_ECC )
     DriveInfo.ECC = TRUE;


   do
     {
     rc = SetTapeParameters( file->osHandle,
                             SET_TAPE_DRIVE_INFORMATION,
                             &DriveInfo );
     }
   while (( rc == ERROR_BUS_RESET            ||
            rc == ERROR_IO_DEVICE            ||
            rc == ERROR_END_OF_MEDIA         ||
            rc == ERROR_BEGINNING_OF_MEDIA   ||
            rc == ERROR_MEDIA_CHANGED ) && --Retry);

   if (( rc != NO_ERROR )                 &&
       ( rc != ERROR_MEDIA_CHANGED )      &&
       ( rc != ERROR_END_OF_MEDIA )       &&
       ( rc != ERROR_BEGINNING_OF_MEDIA ) &&
       ( rc != ERROR_INVALID_FUNCTION ))
     {
     lError            = VF_NOTOK;

     if ( sql06c_handle_known_errors ( "unknown",
                                       rc , ferr->sp5fe_text ) == FALSE )
       {
       DBG1 (( MF__, ERRMSG_VF_TAPE_PARAM ));
       MSGD (( ERR_TAPE_PARAM, rc         ));
       eo46BuildPascalErrorStringRC ( ferr->sp5fe_text, ERRMSG_VF_TAPE_PARAM, rc );
       }
     TRCOUT;
     return (lError);
     }

   if ( ( file->ulMaxTapeBlkSize != 0 )  &&
       ( file->ulTapeFeatures & TAPE_SET_BLOCK_SIZE ))
     {
     if ( file->ulMaxTapeBlkSize < sizeof (tsp00_Buf) )
       MediaInfo.BlockSize = file->ulMaxTapeBlkSize;
     else
       MediaInfo.BlockSize = sizeof (tsp00_Buf);

     do
       {
       rc = SetTapeParameters( file->osHandle,
                               SET_TAPE_MEDIA_INFORMATION,
                               &MediaInfo );
       }
     while (( rc == ERROR_BUS_RESET            ||
              rc == ERROR_IO_DEVICE            ||
              rc == ERROR_END_OF_MEDIA         ||
              rc == ERROR_BEGINNING_OF_MEDIA   ||
              rc == ERROR_MEDIA_CHANGED ) && --Retry);

     if (( rc != NO_ERROR )                 &&
         ( rc != ERROR_MEDIA_CHANGED )      &&
         ( rc != ERROR_END_OF_MEDIA )       &&
         ( rc != ERROR_BEGINNING_OF_MEDIA ) &&
         ( rc != ERROR_INVALID_FUNCTION ))
       {
       lError            = VF_NOTOK;

       if ( sql06c_handle_known_errors ( "unknown",
                                         rc , ferr->sp5fe_text ) == FALSE )
         {
         DBG1 (( MF__, ERRMSG_VF_TAPE_PARAM ));
         MSGD (( ERR_TAPE_PARAM, rc         ));
         eo46BuildPascalErrorStringRC ( ferr->sp5fe_text, ERRMSG_VF_TAPE_PARAM, rc );
         }
       TRCOUT;
       return (lError);
       }
     }

   TRCOUT;
   return (lError);
  }

/*------------------------------*/

  #undef  MF__
  #define MF__ MOD__"eo06c_tape_rewind"
static LONG eo06c_tape_rewind( FileT  *file,
                                tsp05_RteFileError        *ferr)
  {

   APIRET       rc      = NO_ERROR;
   LONG         lError  = VF_OK;
   INT          Retry   = TAPE_ERROR_RETRY;

   TRCIN;

   do
     {
     rc = SetTapePosition(file->osHandle, TAPE_REWIND,
                          0, 0, 0, FALSE);
     }
   while (( rc == ERROR_BUS_RESET          ||
            rc == ERROR_IO_DEVICE          ||
            rc == ERROR_MEDIA_CHANGED ) && --Retry);

   if (( rc != NO_ERROR ) && ( rc != ERROR_MEDIA_CHANGED ))
     {
     lError            = VF_NOTOK;

     if ( sql06c_handle_known_errors ( "unknown",
                                       rc, ferr->sp5fe_text ) == FALSE)
       {
       DBG1 (( MF__, ERRMSG_VF_REWIND_TAPE ));
       MSGD (( ERR_REWIND_TAPE, rc         ));
       eo46BuildPascalErrorStringRC ( ferr->sp5fe_text, ERRMSG_VF_REWIND_TAPE, rc );
       }
     TRCOUT;
     return (lError);
     }

   TRCOUT;
   return (lError);
  }

/*------------------------------*/

  #undef  MF__
  #define MF__ MOD__"eo06c_tape_locking"
static LONG eo06c_tape_locking ( FileT  *file,
                                  tsp05_RteFileError        *ferr,
                                  ULONG               ulOperation )
  {

   APIRET       rc          = NO_ERROR;
   LONG         lError      = VF_OK;
   INT          Retry       = TAPE_ERROR_RETRY;

   TRCIN;

   if ( file->ulTapeFeatures & TAPE_LOCK_UNLOCK )
     {
     do
       {
       rc = PrepareTape ( file->osHandle, ulOperation, FALSE);
       }
     while (( rc == ERROR_BUS_RESET            ||
              rc == ERROR_IO_DEVICE            ||
              rc == ERROR_END_OF_MEDIA         ||
              rc == ERROR_BEGINNING_OF_MEDIA   ||
              rc == ERROR_MEDIA_CHANGED ) && --Retry);

     if (( rc != NO_ERROR )                 &&
         ( rc != ERROR_MEDIA_CHANGED )      &&
         ( rc != ERROR_END_OF_MEDIA )       &&
         ( rc != ERROR_BEGINNING_OF_MEDIA ) &&
         ( rc != ERROR_INVALID_FUNCTION ))
       {
       lError            = VF_NOTOK;
       file->osHandle = INVALID_HANDLE_VALUE;

       if ( sql06c_handle_known_errors ( "unknown",
                                         rc, ferr->sp5fe_text ) == FALSE)
         {
         DBG1 (( MF__, ERRMSG_VF_TAPE_LOCK_UNLOCK ));
         MSGD (( ERR_TAPE_LOCK_UNLOCK, rc ));
         eo46BuildPascalErrorStringRC ( ferr->sp5fe_text, ERRMSG_VF_TAPE_LOCK_UNLOCK, rc );
         }
       TRCOUT;
       return (lError);
       }
     }

   TRCOUT;
   return (lError);
  }

/*------------------------------*/

static BOOL eo06c_tape_in_drive (FileT* file)
  {
  #undef  MF__
  #define MF__ MOD__"sql65k_tape_in_drive"
  APIRET                     rc             = NO_ERROR;
  BOOL                       result;

  DBGIN;
  rc = GetTapeStatus (file->osHandle);
#if defined (DEBUG_RTE)
  DBG1 ((MF__, "GetTapeStatus: %d", rc));
#endif
  switch ( rc )
    {
    case ERROR_NOT_READY:
    case ERROR_MEDIA_CHANGED:
    case ERROR_NO_MEDIA_IN_DRIVE:
      result = FALSE;
      break;
    default:
      result = TRUE;
      break;
    }
  DBGOUT;
  return result;
  }

/*------------------------------*/

  #undef  MF__
  #define MF__ MOD__"eo06c_tape_loading"
static LONG eo06c_tape_loading ( FileT  *file,
                                  tsp05_RteFileError        *ferr,
                                  ULONG               ulOperation )
  {

   APIRET       rc          = NO_ERROR;
   INT          Retry       = TAPE_ERROR_RETRY;
   LONG         lError      = VF_OK;

   TRCIN;

  if ( file->ulTapeFeatures & TAPE_LOAD_UNLOAD )
    {
    if (( ulOperation == TAPE_LOAD) && eo06c_tape_in_drive (file))
      {
      DBG1 ((MF__, "tape already loaded"));
      return TRUE;
      }
    do
      {
      rc = PrepareTape ( file->osHandle, ulOperation , FALSE);
      if ( ulOperation == TAPE_LOAD && rc == ERROR_NO_MEDIA_IN_DRIVE )
        {
        INT WaitSec  = MAX_TAPE_LOAD_RETRY_TIME ;
        do
          {
          SLEEP(1000) ;
          rc = PrepareTape ( file->osHandle, ulOperation , FALSE);
          }
        while ( --WaitSec && rc == ERROR_NO_MEDIA_IN_DRIVE ) ;
        }
      }
    while (( rc == ERROR_MEDIA_CHANGED        ||
             rc == ERROR_END_OF_MEDIA         ||
             rc == ERROR_BEGINNING_OF_MEDIA   ||
             rc == ERROR_IO_DEVICE            ||
             rc == ERROR_BUS_RESET            ) && --Retry);

     if (( rc != NO_ERROR )                 &&
         ( rc != ERROR_MEDIA_CHANGED )      &&
         ( rc != ERROR_END_OF_MEDIA )       &&
         ( rc != ERROR_BEGINNING_OF_MEDIA ) &&
         ( rc != ERROR_INVALID_FUNCTION ))
       {
       lError            = VF_NOTOK;
       file->osHandle = INVALID_HANDLE_VALUE;

       if ( sql06c_handle_known_errors ( "unknown",
                                         rc, ferr->sp5fe_text ) == FALSE)
         {
         DBG1 (( MF__, ERRMSG_VF_TAPE_LOAD_UNLOAD ));
         MSGD (( ERR_TAPE_LOAD_UNLOAD, rc ));
         eo46BuildPascalErrorStringRC ( ferr->sp5fe_text, ERRMSG_VF_TAPE_LOAD_UNLOAD, rc );
         }
       TRCOUT;
       return ( FALSE );
       }
     }
   else
   if ( ulOperation == TAPE_UNLOAD )
     {                            /* autoloader in use, don't ignore error */
       eo46BuildPascalErrorStringRC ( ferr->sp5fe_text, "operation not supported",  0 );
       TRCOUT;
       return ( VF_NOTOK );
     }
   TRCOUT;
   return (VF_OK);
  }

/*------------------------------*/

  #undef  MF__
  #define MF__ MOD__"eo06c_tape_write_mark"
static LONG eo06c_tape_write_mark( FileT  *file,
                                    tsp05_RteFileError        *ferr)
  {

   APIRET       rc             = NO_ERROR;
   LONG         lError         = VF_OK;
   LONG         lCount         = 1;
   DWORD        dwOperation    = 0;
   BOOLEAN         fRetImmediate  = TRUE;
   INT          Retry          = TAPE_ERROR_RETRY;

   TRCIN;

   if ( file->ulTapeFeatures & TAPE_WRITE_LONG_FMKS )
     dwOperation = TAPE_LONG_FILEMARKS;
   else if ( file->ulTapeFeatures & TAPE_WRITE_SHORT_FMKS )
     dwOperation = TAPE_SHORT_FILEMARKS;
   else if ( file->ulTapeFeatures & TAPE_WRITE_FILEMARKS )
     dwOperation = TAPE_FILEMARKS;
   else
     {
     TRCOUT;
     return ( VF_OK );
     }

   if ( file->ulTapeFeatures & TAPE_WRITE_MARK_IMMED )
     fRetImmediate  = FALSE;

   do
     {
     rc = WriteTapemark (file->osHandle, dwOperation, lCount, FALSE);
     }
   while (( rc == ERROR_BUS_RESET ||
            rc == ERROR_IO_DEVICE ) && --Retry);

   if (( rc != NO_ERROR ) && ( rc != ERROR_INVALID_FUNCTION ))
     {
     lError            = VF_NOTOK;
     file->osHandle = INVALID_HANDLE_VALUE;

     if ( sql06c_handle_known_errors ( "unknown",
                                       rc, ferr->sp5fe_text ) == FALSE)
       {
       DBG1 (( MF__, ERRMSG_VF_TAPE_WRITE_MARK ));
       MSGD (( ERR_TAPE_WRITE_MARK, rc ));
       eo46BuildPascalErrorStringRC ( ferr->sp5fe_text, ERRMSG_VF_TAPE_WRITE_MARK, rc );
       }
     TRCOUT;
     return (lError);
     }

   TRCOUT;
   return (lError);
  }

#else

/**\
--------------------------------------- MF__ MOD__ (Unix specifics)
\**/

typedef struct StdHandleMapT {
    const char * name;
    int          handleKind;
} StdHandleMapT;

static BOOLEAN
eo06_tryStdUnix (
    const char* fname,
    tsp05_RteFileMode_Param fileMode,
    FileT* file,
    tsp05_RteFileError *ferr)
{
#undef MF__
#define MF__ MOD__"eo06_tryStdUnix"
    static StdHandleMapT readHandles [] = {
        {"-", 0},
        {"stdin", 0},
        {NULL, -1}
    };
    static StdHandleMapT writeHandles [] = {
        {"-", 1},
        {"stdout", 1},
        {"stderr", 2},
        {NULL, -1}
    };
    StdHandleMapT     * current;

    if (fileMode == sp5vf_read) {
        current = &readHandles [0];
    }
    else {
        current = &writeHandles [0];
    }
    while (current->name != NULL) {
        if (strcmp (current->name, fname) == 0) {
            file->osHandle = current->handleKind;
            return TRUE;
        }
        ++current;
    }
    /* no match found */
    return FALSE;
}

/*------------------------------*/
static BOOLEAN
eo06_lockFileUnix (
    OSFileHandleT osHandle,
    tsp05_RteFileMode_Param fileMode,
    tsp05_RteFileError *ferr)
{
#undef MF__
#define MF__ MOD__"eo06_lockFileUnix"
    int         osRC;
    short       lockType;
    struct flock lockInfo;

    TRCIN;
    if (sp5vf_read == fileMode) {
        return TRUE;
        /*lockType = F_RDLCK;*/
    }
    else {
        lockType = F_WRLCK;
    }
    lockInfo.l_type = lockType;
    lockInfo.l_whence = 0;
    lockInfo.l_start = 0;
    lockInfo.l_len = 0;
    lockInfo.l_pid = 1;
    osRC = fcntl (osHandle, F_GETLK , &lockInfo);
    lockInfo.l_type = lockType;
    osRC = fcntl (osHandle, F_SETLK , &lockInfo);
    if (0 != osRC  && ENOLCK != errno ) {   /* ENOLCK means that the file system does not allow locking (e.g. NFS) - this is ignored */
        eo06_lockError (ferr);
        ferr->sp5fe_warning = sp5vfw_already_locked;
    }
    osRC = fcntl (osHandle, F_GETLK , &lockInfo);
    TRCOUT;
    return (osRC == 0);
}

/*------------------------------*/

/* PTS 1106917 jrg 16.6.2000 */
void
sqlmkfifoc (
    const char *rawFName,
    tsp05_RteFileError *ferr)
{
#undef MF__
#define MF__ MOD__"sqlmkfifoc"
    char nameBuf [EO06_PATH_MAX_C];

    TRCIN;
    eo06_mkfifoUnix (eo06_cookName (nameBuf, sizeof(nameBuf), rawFName, UNDEF), ferr);
    TRCOUT;
}

/*------------------------------*/

void
sqlmkfifop (
    const tsp00_VFilename rawFName,
    tsp05_RteFileError *ferr)
{
#undef MF__
#define MF__ MOD__"sqlmkfifop"
    char trimmedBuf [EO06_PATH_MAX_C];
    char nameBuf [EO06_PATH_MAX_C];
    int nameLen;

    TRCIN;
    nameLen = eo06_trimLen (rawFName, sizeof (tsp00_VFilename));
    SAPDB_memcpy (trimmedBuf, rawFName, nameLen);
    trimmedBuf [nameLen] = '\0';

    eo06_mkfifoUnix (eo06_cookName (nameBuf, sizeof(nameBuf), trimmedBuf, nameLen), ferr);

    eo06_padLen (ferr->sp5fe_text, MAXERRORTEXT);
    TRCOUT;
}

/*-------------------------------*/

static void
eo06_mkfifoUnix( const char *PipeName,
    tsp05_RteFileError *ferr)
{
#undef MF__
#define MF__ MOD__"eo06_mkfifoUnix"
  if ( mkfifo( PipeName, 0666) != 0 )
  {
    eo06_osError (ferr);
  }
  else
  {
    eo06_clearError (ferr);
  }
}

/*-------------------------------*/

static BOOLEAN
eo06_isLockable (
    struct stat * statInfo)
{
#if defined(OSF1)
    /* see ven54c... OSF1 did(does?) not support locking on raw devices */
    return S_ISREG (statInfo->st_mode);
#else
    return !S_ISFIFO (statInfo->st_mode);
#endif
}

/*-------------------------------*/

static BOOLEAN
eo06_fileOpenUnix (
    const char* fname,
    tsp05_RteFileMode_Param fileMode,
    tsp05_RteBufferingKind_Param buffering,
    FileT* file,
    tsp05_RteFileError *ferr)
{
#undef MF__
#define MF__ MOD__"eo06_fileOpenUnix"
    int openFlags = 0;

    if (eo06_tryStdUnix (fname, fileMode, file, ferr)) {
        return TRUE;
    }
    switch (fileMode) {
        case sp5vf_read:
            openFlags = O_RDONLY;
            break;
        case sp5vf_write:
            openFlags = O_WRONLY | O_CREAT | O_TRUNC;
            break;
        case sp5vf_readwrite:
            openFlags = O_RDWR | O_CREAT;
            break;
        case sp5vf_append:
            openFlags = O_WRONLY | O_CREAT;
            break;
        default:
            SWITCH_ERROR_DBG ("Invalid file mode", fileMode);
            break;
    }
    if ((buffering == sp5bk_disksynced) && (openFlags & O_WRONLY)) {
        openFlags |= O_SYNC;
    }

/* PTS 1105660 */
#if !defined(BIT64) && defined(O_LARGEFILE)
       openFlags |= O_LARGEFILE ;
#endif

    /* open file */
    file->osHandle = open (fname, openFlags, 0666);
    if (file->osHandle == invalidHandleC) {
        eo06_osError (ferr);
    }
    else {
        struct stat statInfo;

        if ( fstat (file->osHandle, &statInfo) == 0) {
            /* PTS 1106917 jrg 16.6.2000 */
            /* lock only regular files */
            if (eo06_isLockable (&statInfo)) {
                eo06_lockFileUnix (file->osHandle, fileMode, ferr);
                if (VF_OK != ferr->sp5fe_result) {
                    close (file->osHandle);
                    file->osHandle = invalidHandleC;
                }
            }
            if (S_ISFIFO (statInfo.st_mode)) {
                file->seekBySkip = TRUE;
            }
        }
    }
    /* append */
    if ((file->osHandle != invalidHandleC) && (sp5vf_append == fileMode)) {
        lseek (file->osHandle, 0, SEEK_END);
    }
    TRCOUT;
    return (file->osHandle != invalidHandleC);
}

/*------------------------------*/

#if defined (SCO)

#include <sys/tape.h>
#define SIMPLE_REWIND_OS        1
#define REWIND_COMMAND_OS       MT_REWIND

#elif defined (SVR4) && defined (I386)

#include <sys/tape.h>
#define SIMPLE_REWIND_OS        1
#define REWIND_COMMAND_OS       T_RWD

#elif defined (_IBMR2)

#include <sys/tape.h>
#define COMPLEX_REWIND_OS       1
#define REWIND_COMMAND_OS       STIOCTOP
static struct stop rewindCommandStructC = {STREW, 1};

#elif defined (HPUX) || defined (OSF1) || defined (SVR4) || defined (SOLARIS) || defined(LINUX) || defined(FREEBSD)

#include <sys/ioctl.h>
#include <sys/mtio.h>
#define COMPLEX_REWIND_OS       1
#define REWIND_COMMAND_OS       MTIOCTOP
static struct mtop rewindCommandStructC = {MTREW, 1};

#else
#error "Must define something to include tape header"
#endif

static BOOLEAN
eo06_rewindTapeUnix (
    FileT* file)
{
#undef MF__
#define MF__ MOD__"eo06_rewindTapeUnix"
    struct stat statInfo;
    BOOLEAN ok = TRUE;
    int rc;

    fstat (file->osHandle, &statInfo);
    if (S_ISCHR (statInfo.st_mode)) {
        errno = 0;
#if defined (SIMPLE_REWIND_OS)
        rc = ioctl (file->osHandle, REWIND_COMMAND_OS);
#else
        rc = ioctl (file->osHandle, REWIND_COMMAND_OS, &rewindCommandStructC);
#endif
        if ((rc == systemCallErrorC) && (errno != EINVAL)) {
            ok = FALSE;
        }
    }
    return ok;
}

/*------------------------------*/

static void
eo06_fileClose (
    FileT* file,
    tsp05_RteCloseOption_Param option,
    tsp05_RteFileError *ferr)
{
#undef MF__
#define MF__ MOD__"eo06_fileClose"          /* UNIX */
    BOOLEAN rewindOK = TRUE ; /*IST 1997-04-24 */
    BOOLEAN closeOK;

    if ((option == sp5vf_close_rewind) || (option == sp5vf_close_next_tape)) {
        rewindOK = eo06_rewindTapeUnix (file);
    }
    closeOK = (close (file->osHandle) != systemCallErrorC);
    if (!closeOK) {
        eo06_osError (ferr);
    }
    else {
        if (!rewindOK) {
            eo06_osError (ferr);
        }
        /* tape loader not implemented */
        if (option == sp5vf_close_next_tape) {
            ferr->sp5fe_warning = sp5vfw_no_next_tape;
        }
    }
}

#endif

/*-----------------------------------------------------------*/

boolean
sqlfilecopyc( char           *Src,
              char           *Dst,
              tsp01_RteError *RteError )
{
#undef MF__
#define MF__ MOD__"sqlfilecopyc"
#ifdef WIN32
    return ( (boolean )os44Filecopy ( Src, Dst, RteError ) );
#else
    int e541_filecopy();
    return ( e541_filecopy   ( Src, Dst, RteError ) != 0 );
#endif
}

/*===================================================================*/
#endif /* DIRECTORY_OPS_ONLY */
/*=================================================================== */


/**\
--------------------------------------- MF__ MOD__ (directory ops)
\**/

#if defined (_WIN32)
static void
eo06_copyFoundName (
    tsp00_C256           *fname,
    WIN32_FIND_DATA    *findData)
{
#undef MF__
#define MF__ MOD__"eo06_copyFoundName"  /* win32 */
    strncpy (*fname, findData->cFileName, sizeof (tsp00_C256));
    (*fname)[sizeof (tsp00_C256) - 1] = '\0';
}

/*------------------------------*/

externC void
sqlfopendirc (
    tsp00_C256           *dirname,
    void              **dirHandleP,
    tsp00_C256           *firstname,
    tsp05_RteFileError *ferr)
{
#undef MF__
#define MF__ MOD__"sqlfopendirc"          /* win32 */
    WIN32_FIND_DATA     findData;
    char                nameBuf [EO06_PATH_MAX_C];
    char               *cookedName;

    eo06_clearError (ferr);
    cookedName = (char *)eo06_cookName (nameBuf, sizeof(nameBuf), (char*) dirname, UNDEF);
    if (cookedName != nameBuf) {
        strcpy (nameBuf, cookedName);
    }
    strcat (nameBuf, "\\*");
    *dirHandleP = (void*)FindFirstFile (nameBuf, &findData);
    if ((HANDLE)*dirHandleP != INVALID_HANDLE_VALUE) {
        eo06_copyFoundName (firstname, &findData);
    }
    else {
        eo06_osError (ferr);
    }

}

/*------------------------------*/

externC void
sqlfclosedirc (
    void               *dirHandle,
    tsp05_RteFileError *ferr)
{
#undef MF__
#define MF__ MOD__"sqlfclosedirc"         /* win32 */

    eo06_clearError (ferr);
    FindClose ((HANDLE) dirHandle);
}

/*------------------------------*/

externC void
sqlfreaddirc (
    void               *dirHandle,
    tsp00_C256           *fname,
    tsp05_RteFileError *ferr)
{
#undef MF__
#define MF__ MOD__"sqlfreaddirc"          /* win32 */
    WIN32_FIND_DATA     findData;
    BOOL                ok;

    eo06_clearError (ferr);
    ok = FindNextFile ((HANDLE)dirHandle, &findData);
    if (ok) {
        eo06_copyFoundName (fname, &findData);
    }
    else {
        eo06_osError (ferr);
    }
}

/*------------------------------*/

externC boolean
sqlgetcwdc (
    char           * dirbuf,
    int              buflen,
    tsp01_RteError * RteError)
{
#undef MF__
#define MF__ MOD__"sqlgetcwdc"            /* win32 */
    DWORD dirNameLen;

    eo46_rte_error_init (RteError);
    dirNameLen = GetCurrentDirectory (buflen, dirbuf);
    if (dirNameLen == 0) {
        eo46_set_rte_error (RteError, undef_ceo00, ERRMSG_VF_GETCWD, NULL);
    }
    return (dirNameLen != 0);
}

/*------------------------------*/

externC boolean
sqlchdirc (
    const char     * dirname,
    tsp01_RteError * RteError)
{
#undef MF__
#define MF__ MOD__"sqlchdirc"             /* win32 */
    BOOL cdOK;

    eo46_rte_error_init (RteError);
    cdOK = SetCurrentDirectory (dirname);
    if (!cdOK) {
        eo46_set_rte_error (RteError, undef_ceo00, ERRMSG_VF_CHANGE_DIR, dirname);
    }
    return cdOK;
}

/*------------------------------*/

static boolean
eo06_isAbsolute (
    const char * filename,
    boolean      resolveEnvironment)
{
#undef MF__
#define MF__ MOD__"eo06_isAbsolute"       /* win32 */
    boolean result = FALSE;

    if ((isalpha (filename [0])) && (filename [1] == ':')) {
        result = TRUE;
    }
    return result;
}

/*------------------------------*/

static boolean
eo06_endsWithDirDelimiter (
    const char * filename,
    int nameLen)
{
#undef MF__
#define MF__ MOD__"seo06_endsWithDirDelimiter" /* win32 */
    return (filename [nameLen - 1] == '\\');
}

#else

/*------------------------------*/

#include <sys/types.h>
#include <dirent.h>

externC void
sqlfreaddirc (
    void               *dirHandle,
    tsp00_C256           *fname,
    tsp05_RteFileError *ferr);

externC void
sqlfopendirc (
    tsp00_C256           *dirname,
    void              **dirHandleP,
    tsp00_C256           *firstname,
    tsp05_RteFileError *ferr)
{
#undef MF__
#define MF__ MOD__"sqlfopendirc"          /* UNIX */
    DIR* dirp;

    eo06_clearError (ferr);
    dirp = opendir ((char *)*dirname);
    if (dirp != NULL) {
        *dirHandleP = (void*) dirp;
        sqlfreaddirc (*dirHandleP, firstname, ferr);
    }
    else {
        eo06_osError (ferr);
        *dirHandleP = NULL;
    }
}

/*------------------------------*/

externC void
sqlfclosedirc (
    void               *dirHandle,
    tsp05_RteFileError *ferr)
{
#undef MF__
#define MF__ MOD__"sqlfclosedirc"         /* UNIX */
    eo06_clearError (ferr);
    closedir ((DIR*) dirHandle);
}

/*------------------------------*/

externC void
sqlfreaddirc (
    void               *dirHandle,
    tsp00_C256           *fname,
    tsp05_RteFileError *ferr)
{
#undef MF__
#define MF__ MOD__"sqlfreaddirc"          /* UNIX */
    struct dirent* dirEntry;

    eo06_clearError (ferr);
    dirEntry = readdir ((DIR*) dirHandle);
    if (dirEntry != NULL) {
        strncpy ((char *)*fname, dirEntry->d_name, sizeof (tsp00_C256));
        (*fname)[sizeof (tsp00_C256) - 1] = '\0';
    }
    else {
        eo06_eofError (ferr);
    }
}

/*------------------------------*/

externC boolean
sqlgetcwdc (
    char           * dirbuf,
    int              buflen,
    tsp01_RteError * RteError)
{
#undef MF__
#define MF__ MOD__"sqlgetcwdc"            /* UNIX */
    char * getcwdResult;

    eo46_rte_error_init (RteError);
    getcwdResult = getcwd (dirbuf, buflen);
    if (getcwdResult == NULL) {
        eo46_set_rte_error (RteError, undef_ceo00, ERRMSG_VF_GETCWD, NULL);
    }
    return (getcwdResult != NULL);
}

/*------------------------------*/

externC boolean
sqlchdirc (
    const char     * dirname,
    tsp01_RteError * RteError)
{
#undef MF__
#define MF__ MOD__"sqlchdirc"             /* UNIX */
    int cdOK;

    eo46_rte_error_init (RteError);
    cdOK = chdir (dirname);
    if (cdOK < 0) {
        eo46_set_rte_error (RteError, undef_ceo00, ERRMSG_VF_CHANGE_DIR, dirname);
    }
    return ( cdOK == 0 );
}

/*------------------------------*/

static boolean
eo06_isAbsolute (
    const char * filename,
    boolean      resolveEnvironment)
{
#undef MF__
#define MF__ MOD__"eo06_isAbsolute"       /* UNIX */
    boolean result = FALSE;

    if (filename [0] == '/') {
        result = TRUE;
    }
    return result;
}

/*------------------------------*/

static boolean
eo06_endsWithDirDelimiter (
    const char * filename,
    int nameLen)
{
#undef MF__
#define MF__ MOD__"seo06_endsWithDirDelimiter" /* UNIX */
    return (filename [nameLen - 1] == '/');
}

#endif

/*-----------------------------------------------------------*/

externC const char *
sqljoindirc (
    char           * target,
    const char     * part1,
    const char     * part2,
    unsigned int     targetSize)
{
#undef MF__
#define MF__ MOD__"sqljoindirc"
    boolean putDelimiter;
    int delimiterLen = 0;
    const char * result;
    int part1Len;

    if (eo06_isAbsolute (part2, TRUE)) {
        if (strlen (part2) > (targetSize - 1)) {
            result = NULL;
        }
        else {
            strcpy (target, part2);
            result = target;
        }
    }
    else {
        part1Len = (int)strlen (part1);
        putDelimiter = ! eo06_endsWithDirDelimiter (part1, part1Len);
        if (putDelimiter)
            delimiterLen = 1;
        if ((part1Len + delimiterLen + strlen (part2))> (targetSize - 1)) {
            result = NULL;
        }
        else {
            SAPDB_memcpy (target, part1, part1Len);
            if (putDelimiter) {
                target [part1Len] = PATH_DELIMITER_EO01;
                strcpy (target + part1Len + 1, part2);
            }
            else {
                strcpy (target + part1Len, part2);
            }
            result = target;
        }
    }
    return result;
}

/*-----------------------------------------------------------*/

static boolean eo06_create_dir ( char           *PathToCreate,
                                 tsp01_RteError *RteError )
{
  #undef  MF__
  #define MF__ MOD__"eo01_create_dir"

  boolean fCreateOk = TRUE ;
  tsp00_Int4 OsErrorCode ;
  char    *act_pos = PathToCreate , *end_pos;

  TRCIN;

  eo46_rte_error_init (RteError);

  DBG3 ((MF__, "PathToCreate = %s", PathToCreate )) ;

#ifdef WIN32
  if ( act_pos[1] == ':' )
      act_pos += 2;
#endif
  if ( *act_pos == PATH_DELIMITER_EO01 )
      act_pos++ ;

  do
  {
      if ( (end_pos = (char *)strchr ( act_pos, PATH_DELIMITER_EO01 )) != NULL )
          *end_pos = '\0' ;
      DBG3 ((MF__, "creating directory <%s>", PathToCreate ));
#ifdef WIN32
      fCreateOk = CreateDirectory ( PathToCreate, NULL ) != 0 ;
      /* if creating failed because a directory of the name already exists, return no error */
      if(!fCreateOk)
      {
        OsErrorCode = GetLastError();
        if( ERROR_ALREADY_EXISTS == OsErrorCode )
        {
            fCreateOk = (FILE_ATTRIBUTE_DIRECTORY == (FILE_ATTRIBUTE_DIRECTORY & GetFileAttributes(PathToCreate)));
        }
      }
#else
      fCreateOk = mkdir ( PathToCreate, 0775 ) == 0 ;
      /* if creating failed because a directory of the name already exists, return no error */
      if(!fCreateOk)
      {
        OsErrorCode = errno;
        if( EEXIST == OsErrorCode )
        {
          struct stat testBuf;
          if ( stat( PathToCreate, &testBuf ) == 0 )
          {
            fCreateOk = ( (testBuf.st_mode & S_IFDIR) == S_IFDIR );
          }
        }
      }
#endif
      if (! fCreateOk )
      {
          RteError->RteErrCode = RTE_ERROR_SP01;
          RteError->OsErrCode  = OsErrorCode ;
          eo46_rte_errtext_with_filename ( ERRMSG_VF_CREATE_DIR,
                  PathToCreate ,
                  RteError->RteErrText ,
                  sizeof( RteError->RteErrText ) );

          sqlos_errtext_by_errcode ( RteError->OsErrCode,
                  RteError->OsErrText,
                  sizeof ( RteError->OsErrText) ) ;
      }
      if ( end_pos != NULL )
      {
          *end_pos = PATH_DELIMITER_EO01;
          act_pos = end_pos+1 ;
      }
    } while ( fCreateOk && end_pos != NULL ) ;

  TRCOUT;

  return ( fCreateOk ) ;

}

/*-----------------------------------------------------------*/

boolean
sqlcreate_dirc ( char           *PathToCreate,
                 tsp01_RteError *RteError )
{
#undef MF__
#define MF__ MOD__"sqlcreate_dirc"
    TRCIN;
    return ( eo06_create_dir (PathToCreate, RteError) ) ;
    TRCOUT;
}

/* PTS 1105255 */
/*-----------------------------------------------------------*/

boolean
sqlremove_empty_dirc ( const char           *PathToRemove,
                       tsp01_RteError *RteError )
{
#undef MF__
#define MF__ MOD__"sqlremove_empty_dirc"
    boolean RemoveOk ;

    TRCIN;

    eo46_rte_error_init (RteError);

#ifdef WIN32
    RemoveOk = RemoveDirectory ( PathToRemove );
#else
    RemoveOk = rmdir ( PathToRemove ) == 0 ;
#endif

    if ( !RemoveOk )
        eo46_set_rte_error (RteError, undef_ceo00, ERRMSG_VF_REMOVE_DIR, PathToRemove);

    TRCOUT;

    return ( RemoveOk ) ;
}

/*-----------------------------------------------------------*/

typedef tsp05_SpecialEnumeration * enumConstructor_FT (
    boolean              longInfo,
    const char         * additionalInfo,
    char               * header,
    int                  headerSize,
    tsp05_RteFileError * ferr);

typedef void enumNext_FT (
    tsp05_SpecialEnumeration * listHandle,
    char               * buf,
    int                  bufSize,
    tsp05_RteFileError * ferr);

typedef void enumCLose_FT (
    tsp05_SpecialEnumeration * listHandle);

typedef struct EnumClassT {
    const char   * name;
    const char   * headerString;
    enumNext_FT  * nextFunction;
    enumCLose_FT * closeFunction;
} EnumClassT;

/*-----------------------------------------------------------*/

static boolean
putEnumHeader (
    EnumClassT * classDesc,
    char       * header,
    int          headerSize,
    tsp05_RteFileError * ferr)
{
#undef MF__
#define MF__ MOD__"putEnumHeader"
    int headerLen;
    boolean result = TRUE;

    headerLen = (int)strlen (classDesc->headerString);
    if (headerLen + 1 > headerSize) {
        eo06_ferror (ferr, "buffer too small");
        result = FALSE;
    }
    strcpy (header, classDesc->headerString);
    return result;
}

#if defined (_WIN32)

/*
 * win32 tape class
 */

typedef struct TapeEnumWin32 {
    EnumClassT * classDesc;
    int          currentTape;
} TapeEnumWin32;

static EnumClassT tapeEnumClass;

#define maxTapeWin32C   32
/*-----------------------------------------------------------*/

static tsp05_SpecialEnumeration *
newTapeEnumWin32 (
    boolean              longInfo,
    const char         * additionalInfo,
    char               * header,
    int                  headerSize,
    tsp05_RteFileError * ferr)
{
#undef MF__
#define MF__ MOD__"newTapeEnumWin32"
    TapeEnumWin32 * self;
    EnumClassT    * classDesc;

    classDesc = &tapeEnumClass;
    if (!putEnumHeader (classDesc, header, headerSize, ferr)) {
        return NULL;
    }
    self = malloc (sizeof (TapeEnumWin32));
    if (self == NULL) {
        eo06_ferror (ferr, Out_Of_Memory_ErrText);
        return NULL;
    }
    self->classDesc = classDesc;
    self->currentTape = UNDEF;
    return (tsp05_SpecialEnumeration *) self;
}

/*-----------------------------------------------------------*/

static void
nextTapeWin32 (
    tsp05_SpecialEnumeration * selfP,
    char               * buf,
    int                  bufSize,
    tsp05_RteFileError * ferr)
{
#undef MF__
#define MF__ MOD__"nextTapeWin32"
    TapeEnumWin32 * self = (TapeEnumWin32*) selfP;
    char tapeName [10];
    HANDLE tapeHandle;
    boolean tapeFound;

    if (bufSize < 13) {
        eo06_ferror (ferr, "buffer too small");
    }
    do {
        ++self->currentTape;
        if (self->currentTape > maxTapeWin32C) {
            eo06_eofError (ferr);
            return;
        }
        sp77sprintf (tapeName,sizeof(tapeName), "\\\\.\\tape%d", self->currentTape);
        tapeHandle = CreateFile (tapeName, GENERIC_READ, FILE_SHARE_READ,
                NULL, OPEN_EXISTING, 0, 0);
        if (tapeHandle == INVALID_HANDLE_VALUE) {
            int errCode = GetLastError ();
            if (errCode == 2) {
                /* 2 : cannot find file */
                tapeFound = FALSE;
            }
            else {
                tapeFound = TRUE;
            }
        }
        else {
            tapeFound = TRUE;
            CloseHandle (tapeHandle);
        }
    } while (!tapeFound);
    sp77sprintf (buf,bufSize, "\"%s\"\n", tapeName);
}

/*-----------------------------------------------------------*/

static void closeTapeEnumWin32 (
    tsp05_SpecialEnumeration * selfP)
{
#undef MF__
#define MF__ MOD__"closetapeEnumWin32"
    free (selfP);
}

static EnumClassT tapeEnumClass = {
    "TapeEnum (*, WIN32)",
    "name\n",
    nextTapeWin32,
    closeTapeEnumWin32
};

/*
 * win32 disk class
 */

typedef struct DiskEnumWin32 {
    EnumClassT * classDesc;
    DWORD        logicalDrives;
    int          currentDrive;
} DiskEnumWin32;

static EnumClassT longDiskEnumClass;
static EnumClassT shortDiskEnumClass;

#define maxDiskWin32C   26
/*-----------------------------------------------------------*/

static tsp05_SpecialEnumeration *
newDiskEnumWin32 (
    boolean              longInfo,
    const char         * additionalInfo,
    char               * header,
    int                  headerSize,
    tsp05_RteFileError * ferr)
{
#undef MF__
#define MF__ MOD__"newDiskEnumWin32"
    DiskEnumWin32 * self;
    EnumClassT    * classDesc;

    if (longInfo) {
        classDesc = &longDiskEnumClass;
    }
    else {
        classDesc = &shortDiskEnumClass;
    }
    if (!putEnumHeader (classDesc, header, headerSize, ferr)) {
        return NULL;
    }
    self = malloc (sizeof (DiskEnumWin32));
    if (self == NULL) {
        eo06_ferror (ferr, Out_Of_Memory_ErrText);
        return NULL;
    }
    self->classDesc = classDesc;
    self->logicalDrives = GetLogicalDrives ();
    self->currentDrive = UNDEF;
    return (tsp05_SpecialEnumeration *) self;
}

/*-----------------------------------------------------------*/

static int
nextValidDriveWin32 (
    DiskEnumWin32 * self)
{
    boolean     noDrive;
    DWORD       bitPattern;

    do {
        ++self->currentDrive;
        bitPattern = 1 << self->currentDrive;
        noDrive = (self->logicalDrives & bitPattern) == 0;
    } while (noDrive && (self->currentDrive <= maxDiskWin32C));
    if (self->currentDrive > maxDiskWin32C) {
        self->currentDrive = UNDEF;
    }
    return self->currentDrive;
}

/*-----------------------------------------------------------*/

static void
nextDiskShortWin32 (
    tsp05_SpecialEnumeration * selfP,
    char               * buf,
    int                  bufSize,
    tsp05_RteFileError * ferr)
{
#undef MF__
#define MF__ MOD__"nextDiskShortWin32"
    DiskEnumWin32 * self = (DiskEnumWin32*) selfP;
    int drive;

    if (bufSize < 5) {
        eo06_ferror (ferr, "buffer too small");
    }
    drive = nextValidDriveWin32 (self);
    if (drive == UNDEF) {
        eo06_eofError (ferr);
        return;
    }
    sp77sprintf (buf,bufSize, "\"%c\"\n", drive + 'A');
}

/*-----------------------------------------------------------*/

static void
nextDiskLongWin32 (
    tsp05_SpecialEnumeration * selfP,
    char                * buf,
    int                   bufSize,
    tsp05_RteFileError  * ferr)
{
#undef MF__
#define MF__ MOD__"nextDiskLongWin32"
    DiskEnumWin32 * self = (DiskEnumWin32*) selfP;
    int drive;
    int driveKind;
    char rootName [4];
    unsigned long sectorsPerCluster;
    unsigned long bytesPerSector;
    unsigned long freeClusters;
    unsigned long clusters;
    unsigned long blockPerCluster;
    unsigned long kbCapacity;
    unsigned long kbUsed;
    unsigned long kbFree;
    boolean ok;
    static const char * kindNamesC [] = {
        "UNKNOWN", "NO_ROOT_DIR", "REMOVABLE", "FIXED",
        "REMOTE", "CDROM", "RAMDISK"};

    if (bufSize < 100) {
        eo06_ferror (ferr, "buffer too small");
    }
    do {
        drive = nextValidDriveWin32 (self);
        if (drive == UNDEF) {
            eo06_eofError (ferr);
            return;
        }
        strcpy (rootName, "0:\\");
        rootName [0] = (char) (drive + 'a');
        driveKind = GetDriveType (rootName);
    } while (driveKind <= 1);
    if (driveKind != DRIVE_REMOVABLE) {
        ok = GetDiskFreeSpace (rootName, &sectorsPerCluster, &bytesPerSector,
                &freeClusters, &clusters);
    }
    else {
        ok = FALSE;
    }
    if (ok) {
        blockPerCluster = (bytesPerSector * sectorsPerCluster) / 512;
        kbCapacity = clusters * blockPerCluster * 2;
        kbFree = freeClusters * blockPerCluster * 2;
        kbUsed = kbCapacity - kbFree;
    }
    else {
        kbCapacity = 0;
        kbUsed = 0;
        kbFree = 0;
    }
    sp77sprintf (buf,bufSize, "\"%c\",\"%s\",%d,%d,%d\n", (drive + 'A'),
            kindNamesC [driveKind], kbCapacity, kbUsed, kbFree);
}

/*-----------------------------------------------------------*/

static void closeDiskEnumWin32 (
    tsp05_SpecialEnumeration * selfP)
{
#undef MF__
#define MF__ MOD__"closeDiskEnumWin32"
    free (selfP);
}

static EnumClassT longDiskEnumClass = {
    "DiskEnum (long, WIN32)",
    "name,kind,capacityKB,usedKB,freeKB\n",
    nextDiskLongWin32,
    closeDiskEnumWin32
};

static EnumClassT shortDiskEnumClass = {
    "DiskEnum (short, WIN32)",
    "name\n",
    nextDiskShortWin32,
    closeDiskEnumWin32
};

static enumConstructor_FT * enumConstructors [] = {
    NULL,
    newTapeEnumWin32,
    newDiskEnumWin32,
};

struct tsp05_SpecialEnumeration {
    EnumClassT * classDesc;
};

externC void
sqlspecialenum (
    tsp05_SpecialdevKind_Param   devKind,
    boolean                      longInfo,
    const char                 * additionalInfo,
    tsp05_SpecialEnumeration  ** listHandle,
    char                       * header,
    int                          headerSize,
    tsp05_RteFileError         * ferr)
{
#undef MF__
#define MF__ MOD__"sqlspecialenum"
    enumConstructor_FT * constructor;

    eo06_clearError (ferr);
    switch (devKind) {
        case sdk_tapes_sp05:
        case sdk_drives_sp05:
            constructor = enumConstructors [devKind];
            *listHandle = constructor (longInfo, additionalInfo,
                    header, headerSize, ferr);
            break;
        default:
            eo06_ferror (ferr, "Invalid device kind");
            *listHandle = NULL;
            break;
    }
}

/*-----------------------------------------------------------*/

externC void
sqlspecialnext (
    tsp05_SpecialEnumeration  * self,
    char                      * buf,
    int                         bufSize,
    tsp05_RteFileError        * ferr)
{
#undef MF__
#define MF__ MOD__"sqlspecialnext"
    eo06_clearError (ferr);
    self->classDesc->nextFunction (self, buf, bufSize, ferr);
}

/*-----------------------------------------------------------*/

externC void
sqlspecialclose (
    tsp05_SpecialEnumeration  * self)
{
#undef MF__
#define MF__ MOD__"sqlspecialclose"
    if (self != NULL) {
        self->classDesc->closeFunction (self);
    }
}

#else

/*
 * unix tape class
 */

/*
 * unix disk class
 */

#endif

/*-----------------------------------------------------------*/
/* PTS 1104193 */
/* PTS 1106627 */
externC tsp00_Bool
sqlgetpass(const char *         Prompt,
           tsp00_Namec          Password,
           tsp05_RteFileError * ferr)
{
    tsp00_Bool Result = true;

#if defined(WIN32)

    HANDLE hConsoleHandle;
    HANDLE hConsoleOutputHandle;
    DWORD  dwMode;
    DWORD  CharactersWritten;
    DWORD  CharactersRead;

#else

    char *GivenPassword;
    extern char *getpass(const char *);

#endif

    eo06_clearError (ferr);

    memset(Password, 0, sizeof(tsp00_Namec));

#if defined(WIN32)

    hConsoleHandle = GetStdHandle(STD_INPUT_HANDLE);
    hConsoleOutputHandle = GetStdHandle(STD_OUTPUT_HANDLE);

    if ( hConsoleHandle == INVALID_HANDLE_VALUE
      || hConsoleOutputHandle == INVALID_HANDLE_VALUE )
    {
        eo06_ferror (ferr, "Invalid standard handle");
        Result = false;
    }
    else if ( !WriteConsole(hConsoleOutputHandle, &Prompt[0], (DWORD)strlen(Prompt), &CharactersWritten, NULL) )
    {
        eo06_ferror (ferr, "Can't write prompt");
        Result = false;
    }
    else if ( !GetConsoleMode(hConsoleHandle, &dwMode) )
    {
        eo06_ferror (ferr, "Can't get console mode");
        Result = false;
    }
    else if ( !SetConsoleMode(hConsoleHandle, (dwMode&(~ENABLE_ECHO_INPUT))|ENABLE_LINE_INPUT ) )
    {
        eo06_ferror (ferr, "Can't set console mode");
        Result = false;
    }
    else if ( !ReadConsole(hConsoleHandle, &Password[0], sizeof(tsp00_Name), &CharactersRead, NULL) )
    {
        (void)SetConsoleMode(hConsoleHandle, dwMode);
        eo06_ferror (ferr, "Can't read from console");
        Result = false;
    }
    else
    {
        if ( CharactersRead > 1 )
        {
            if (  Password[CharactersRead-1] == '\n'
               && Password[CharactersRead-2] == '\r' )
            {
                (void)WriteConsole(hConsoleOutputHandle, &Password[CharactersRead-2], 2, &CharactersWritten, NULL);
                Password[CharactersRead-2] = 0;
            }
            else if (  Password[CharactersRead-1] == '\n' )
            {
                (void)WriteConsole(hConsoleOutputHandle, &Password[CharactersRead-1], 1, &CharactersWritten, NULL);
                Password[CharactersRead-1] = 0;
            }
        }
        (void)SetConsoleMode(hConsoleHandle, dwMode);
    }

#else

    memset(Password, 0, sizeof(tsp00_Namec));
    GivenPassword = getpass(Prompt);
    if ( !GivenPassword )
    {
        eo06_ferror (ferr, "Can't read from console");
        Result = false;
    }
    else
    {
        strncpy(Password, GivenPassword, sizeof(tsp00_Namec));
    }

#endif
    return Result;
}


