/*!
  @file           IFRUtil_DefaultRawAllocator.cpp
  @author         D030044
  @ingroup        IFR_Mem
  @brief          Memory allocator.
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



\endif
*/
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include "SAPDBCommon/SAPDB_ReuseLib.h"
#include "SAPDB/RunTime/RTE_CallDebugger.h"
#include "SAPDB/Interfaces/Runtime/IFR_Common.h"
#include "SAPDB/Interfaces/Runtime/Util/IFRUtil_DefaultRawAllocator.h"

#define MEMORY_ALLOCATION_TECHNIQUE_LOG              1
#define MEMORY_ALLOCATION_TECHNIQUE_SQLALLOCAT       2
#define MEMORY_ALLOCATION_TECHNIQUE_PATTERNCHECK     3

#define MEMORY_ALLOCATION_TECHNIQUE MEMORY_ALLOCATION_TECHNIQUE_SQLALLOCAT

#if MEMORY_ALLOCATION_TECHNIQUE == MEMORY_ALLOCATION_TECHNIQUE_SQLALLOCAT
#  define SQLALLOCAT(bytecount, result, wasOk, forArray) \
  sqlallocat((bytecount), (result), (wasOk))
#  define SQLFREE(ptr, forArray) sqlfree((ptr))
#  define SQLREALLOCAT(bytecount, result, wasOk, forArray) \
  sqlreallocat((bytecount), (result), (wasOk))
#define MEMORY_LOG 0
#elif MEMORY_ALLOCATION_TECHNIQUE == MEMORY_ALLOCATION_TECHNIQUE_LOG
#  define SQLALLOCAT   DebugSqlAllocat
#  define SQLFREE      DebugSqlFree
#  define SQLREALLOCAT DebugSqlReAllocat
#  define MEMORY_LOG  1
#elif MEMORY_ALLOCATION_TECHNIQUE == MEMORY_ALLOCATION_TECHNIQUE_PATTERNCHECK

void PatternCheckSqlAllocat(IFR_UInt4     size,
                            tsp00_Uint1 **result,
                            tsp00_Bool *wasOk,
                            IFR_Int4    mode);

#  define SQLALLOCAT   PatternCheckSqlAllocat

void PatternCheckSqlFree(tsp00_Uint1 *ptr,
                         IFR_Int4  mode);

#  define SQLFREE      PatternCheckSqlFree

void PatternCheckSqlReAllocat(IFR_UInt4 size,
                              tsp00_Uint1 **result,
                              tsp00_Bool *wasOk,
                              IFR_Int4    mode);

#  define SQLREALLOCAT PatternCheckSqlReAllocat
#endif

#if MEMORY_ALLOCATION_TECHNIQUE == MEMORY_ALLOCATION_TECHNIQUE_LOG

#include "gsp05.h" /* nocheck */
#include "heo03.h" /* nocheck */
#include "hsp77.h" /* nocheck */
#include "heo06.h" /* nocheck */
#include "heo13.h" /* nocheck */


#ifndef MEMORY_LOGFILE
#  define MEMORY_LOGFILE  "memorylog.sql"
#  define MEMORY_HALTFILE "memoryhalt.txt"

#endif


static tsp00_Int4                              DebugFileHandle = -1;
static tsp00_Int4                              DebugFileHandleInitialized = 0;
static IFR_UInt4                               DebugIndex;
static IFR_UInt4                              *DebugBreakIndices;
static IFR_UInt4                               DebugBreakIndicesLength;

extern "C" void DebugFileHandleExit(void)
{
// The file is not closed, as static or quasi-static objects are destructed 
// after 'atexit' has run this function.

//     if(DebugFileHandle != -1) {
//         fprintf(stderr, "Closing file.\n");
//         tsp05_RteFileError ignored;
//         sqlfclosec(DebugFileHandle, sp5vf_close_normal, &ignored);
//         DebugFileHandle = -1;
//     }
}

extern "C" void *DebugFileHandleInit(void *)
{
    tsp05_RteFileError error;
    sqlfopenc(MEMORY_LOGFILE,
              sp5vf_binary,
              sp5vf_write,
              sp5bk_unbuffered,
              &DebugFileHandle,
              &error);
    if(error.sp5fe_result != vf_ok) {
        DebugFileHandle = -1;
    } else {
        atexit(DebugFileHandleExit); 
        ++DebugIndex;
    }

    tsp00_Int4 haltfile;
    sqlfopenc(MEMORY_HALTFILE,
              sp5vf_binary,
              sp5vf_read,
              sp5bk_unbuffered,
              &haltfile,
              &error);
    if(error.sp5fe_result != vf_ok) {
        return 0;
    }
    // determine the size
    sqlfseekc(haltfile, 0, sp5vf_seek_end, &error);
    if(error.sp5fe_result != vf_ok) {
        return 0;
    }
    tsp00_Longint filesize=0;
    sqlftellc(haltfile, &filesize, &error);
    if(error.sp5fe_result != vf_ok) {
        return 0;
    }
    sqlfseekc(haltfile, 0, sp5vf_seek_begin, &error);
    if(error.sp5fe_result != vf_ok) {
        return 0;
    }
    
    unsigned char *buffer=0;
    tsp00_Bool     wasok;
    sqlallocat(filesize + 1, &buffer, &wasok);
    memset(buffer, 0, filesize+1);
    tsp00_Longint outlen;
    sqlfreadc(haltfile, buffer, filesize, &outlen, &error);
    if(error.sp5fe_result != vf_ok) {
        sqlfree(buffer);
        return(0);
    }
    // now count the ,
    DebugBreakIndicesLength=1;
    char *p=(char *)buffer;
    while(strchr(p, ',')) {
        p=strchr(p, ',')+1;
        ++DebugBreakIndicesLength;
    }
    sqlallocat(DebugBreakIndicesLength * sizeof(IFR_UInt4),
               (tsp00_Uint1 **) &DebugBreakIndices,
               &wasok);
    IFR_UInt4 index=0;
    p=(char *)buffer;
    while(p) {
        char *p_pos = strchr(p, ',');
        if(p_pos) {
            *p_pos=0;
        }
        DebugBreakIndices[index]=atoi(p);
        ++index;
        if(p_pos) {
            p=p_pos + 1;
        } else {
            p=0;
        }
    }
    sqlfree(buffer);
    tsp05_RteFileError ignored;
    sqlfclosec(haltfile, sp5vf_close_normal, &ignored);
    
    return 0;
}

void DebugSqlAllocat(IFR_UInt4     size,
                     tsp00_Uint1 **result,
                     tsp00_Bool *wasOk,
                     IFR_Int4    mode)
{
    sqlallocat(size,result, wasOk);
    if(DebugFileHandle == -1) {
        if(!DebugFileHandleInitialized) {
            DebugFileHandleInit(0);
            DebugFileHandleInitialized=1;
        }
    }
    if(DebugFileHandle != -1) {
        IFR_UInt4 index = ++DebugIndex;

        for(IFR_UInt4 i=0; i<DebugBreakIndicesLength; ++i) {
            if(DebugBreakIndices[i]==index) {
#ifdef _WIN32
                DebugBreak();
#else
                RTE_CallDebugger();
#endif
            }
        }

        char sqlCommand[1024];
        sp77sprintf(sqlCommand, 1024,
                    "INSERT INTO MEMORYPROFILE VALUES (%u,'%s', %u, '%#lx', %d)\n//\n", 
                    index,
                    "unknown",
                    size,
                    *result,
                    mode);
        tsp05_RteFileError error;
        sqlfwritec(DebugFileHandle,
                   sqlCommand,
                   strlen(sqlCommand),
                   &error);
        if(error.sp5fe_result != vf_ok) {
            DebugFileHandleExit();
        }
    }
}

void DebugSqlReAllocat(IFR_UInt4 size,
                       tsp00_Uint1 **result,
                       tsp00_Bool *wasOk,
                       IFR_Int4    mode)
{
    tsp00_Uint1 *oldpointer=*result;
    sqlreallocat(size, (tsp00_Uint1 **)result, wasOk);
    if(DebugFileHandle != -1) {
        char sqlCommand[1024];
        sp77sprintf(sqlCommand, 1024,
                    "UPDATE MEMORYPROFILE SET POINTER = '%#lx', SIZE = %u WHERE POINTER = '%#lx' AND ISARRAY = %d\n//\n",
                    *result,
                    size,
                    oldpointer, 
                    mode);
        tsp05_RteFileError error;
        sqlfwritec(DebugFileHandle,
                   sqlCommand,
                   strlen(sqlCommand),
                   &error);
        if(error.sp5fe_result != vf_ok) {
            DebugFileHandleExit();
        }
    }
}

void DebugSqlFree(tsp00_Uint1 *ptr,
                  IFR_Int4  mode)
{
    if(DebugFileHandle != -1) {
        char sqlCommand[1024];
        sp77sprintf(sqlCommand, 1024,
                    "DELETE FROM MEMORYPROFILE WHERE POINTER = '%#lx' AND ISARRAY = %d\n//\n",
                    ptr,
                    mode);
        tsp05_RteFileError error;
        sqlfwritec(DebugFileHandle,
                   sqlCommand,
                   strlen(sqlCommand),
                   &error);
        if(error.sp5fe_result != vf_ok) {
            DebugFileHandleExit();
        }
    }
    sqlfree(ptr);
}
#endif

//----------------------------------------------------------------------
IFRUtil_DefaultRawAllocator::IFRUtil_DefaultRawAllocator()
{}

//----------------------------------------------------------------------
IFRUtil_DefaultRawAllocator::~IFRUtil_DefaultRawAllocator()
{}

//----------------------------------------------------------------------
void *
IFRUtil_DefaultRawAllocator::Allocate(SAPDB_ULong byteCount)
{
    if(byteCount > MAX_IFR_UINT4) { 
        return 0;
    } else {
        IFR_UInt1* result=0;
        tsp00_Bool wasOk;
        
        SQLALLOCAT((IFR_UInt4)byteCount, &result, &wasOk, 2);
        if(wasOk) {
            return (void*)result;
        } else {
            return 0;
        }
    }
}

//----------------------------------------------------------------------
void
IFRUtil_DefaultRawAllocator::Deallocate(void *p)
{
    // Force a segmentation fault on  Deallocate(0)
    if(p==0) {
        char *x=(char *)p;
        *x = 'O';
    }
    SQLFREE((IFR_UInt1*)p, 2);
}

//----------------------------------------------------------------------
void *
IFRUtil_DefaultRawAllocator::Allocate(SAPDB_ULong byteCount, 
                                      const void *hint)
{
        return Allocate(byteCount);
}

//----------------------------------------------------------------------
void
IFRUtil_DefaultRawAllocator::GetBaseAllocatorCallStatistics(SAPDB_ULong& CountAlloc,
                                                            SAPDB_ULong& CountDealloc) const
{
    CountAlloc=CountAlloc=0;
    return;
}

//----------------------------------------------------------------------
void
IFRUtil_DefaultRawAllocator::GetCallStatistics(SAPDB_ULong& CountAlloc,
                                               SAPDB_ULong& CountDealloc) const
{
    CountAlloc=CountAlloc=0;
    return;
}

//----------------------------------------------------------------------

#if IFR_MEMORY_DEBUG

static tsp00_Int4    AllocationLogFileHandle            = -1;
static tsp00_Int4    AllocationLogFileHandleInitialized = 0;

//----------------------------------------------------------------------
static void AllocationLogFileHandleInit()
{
    tsp00_Int4 pid;
    sqlgetpid(&pid);
    char filename[1024];
    sp77sprintf(filename, 1024, "memorylog-%d.txt", pid);
    tsp05_RteFileError error;
    sqlfopenc(filename,
              sp5vf_binary,
              sp5vf_write,
              sp5bk_unbuffered,
              &AllocationLogFileHandle,
              &error);
    if(error.sp5fe_result != vf_ok) {
        AllocationLogFileHandle = -1;
    }
}


//----------------------------------------------------------------------
extern "C" void IFR_AllocateCallback(size_t size,
                                     void *pointer,
                                     const char *file,
                                     const int line)
{
    if(AllocationLogFileHandle == -1) {
        if(!AllocationLogFileHandleInitialized) {
            AllocationLogFileHandleInit();
            AllocationLogFileHandleInitialized = 1;
        }
    }
    
    char output[1024];
    sp77sprintf(output, 1024, "A\t%#lx\t%u\t%s\t%d\n", pointer, size, file, line);
    tsp05_RteFileError error;
    sqlfwritec(AllocationLogFileHandle,
               output,
               strlen(output),
               &error);
    
}

//----------------------------------------------------------------------
extern "C" void IFR_DeallocateCallback(void *pointer,
                                       const char *file,
                                       const int line)
{
    if(AllocationLogFileHandle == -1) {
        if(!AllocationLogFileHandleInitialized) {
            AllocationLogFileHandleInit();
            AllocationLogFileHandleInitialized = 1;
        }
    }
    char output[1024];
    sp77sprintf(output, 1024, "D\t%#lx\t\t%s\t%d\n", pointer, file, line);
    tsp05_RteFileError error;
    sqlfwritec(AllocationLogFileHandle,
               output,
               strlen(output),
               &error);
}


#endif

#if MEMORY_ALLOCATION_TECHNIQUE == MEMORY_ALLOCATION_TECHNIQUE_PATTERNCHECK

#define PATTERN_AREA_SIZE 32
#define PATTERN_BEGIN     0xAA
#define PATTERN_END       0xEE

void PatternCheckSqlAllocat(IFR_UInt4     size,
                            tsp00_Uint1 **result,
                            tsp00_Bool *wasOk,
                            IFR_Int4    mode)
{
    IFR_UInt4 used_size = size + PATTERN_AREA_SIZE + PATTERN_AREA_SIZE + sizeof(IFR_UInt4);
    tsp00_Uint1*tmp_result;
    sqlallocat(used_size,&tmp_result, wasOk);
    if(!*wasOk) {
        return;
    }
    memcpy(tmp_result, &size, sizeof(IFR_UInt4));
    tmp_result += sizeof(IFR_UInt4);
    for(int i=0; i<PATTERN_AREA_SIZE; ++i) {
        tmp_result[i] = PATTERN_BEGIN;
    }
    tmp_result += PATTERN_AREA_SIZE;
    *result = tmp_result;
    tmp_result += size;
    for(int j=0; j<PATTERN_AREA_SIZE; ++j) {
        tmp_result[j] = PATTERN_END;
    }
    return;
}

void PatternCheckSqlFree(tsp00_Uint1 *ptr,
                         IFR_Int4  mode)
{
    if(ptr == 0) {
        fprintf(stderr, "NULL POINTER\n");
        RTE_CallDebugger();
    }

    tsp00_Uint1 *p = ptr - PATTERN_AREA_SIZE - sizeof(IFR_UInt4);
    IFR_UInt4 netto_size;
    memcpy(&netto_size, p, sizeof(IFR_UInt4));
    for(int i=0; i<sizeof(IFR_UInt4); ++i) {
        p[i] = 0xCC;
    }
    p += sizeof(IFR_UInt4);
    if(netto_size == 0xCCCCCCCC) {
        fprintf(stderr, "FREE OF ALREADY FREED BLOCK!\n");
        RTE_CallDebugger();
    }
    for(int j=0; j<PATTERN_AREA_SIZE; ++j) {
        if(p[j] !=  PATTERN_BEGIN) {
            fprintf(stderr, "BEGIN PATTERN NOT FOUND!");
            RTE_CallDebugger();
        }
    }
    p += (netto_size + PATTERN_AREA_SIZE);
    for(int k=0; k<PATTERN_AREA_SIZE; ++k) {
        if(p[k] !=  PATTERN_END) {
            fprintf(stderr, "END PATTERN NOT FOUND!");
            RTE_CallDebugger();
        }
    }
    p = ptr - PATTERN_AREA_SIZE - sizeof(IFR_UInt4);
    sqlfree(p);
}


void PatternCheckSqlReAllocat(IFR_UInt4 size,
                              tsp00_Uint1 **result,
                              tsp00_Bool *wasOk,
                              IFR_Int4    mode)
{
    tsp00_Uint1 *original = *result;
    tsp00_Uint1 *original_data = *result;
    original -= PATTERN_AREA_SIZE;
    original -= sizeof(IFR_UInt4);
    IFR_UInt4 oldsize;
    memcpy(&oldsize, &original, sizeof(IFR_UInt4));
    if(oldsize == 0xCCCCCCCC) {
        fprintf(stderr, "REALLOC OF ALREADY FREED BLOCK!\n");
        RTE_CallDebugger();
    }
    tsp00_Uint1 *new_buffer;
    PatternCheckSqlAllocat(size, &new_buffer, wasOk, mode);
    if(*wasOk) {
        if(oldsize > size) {
            memcpy(new_buffer, original_data, size);
        } else {
            memcpy(new_buffer, original_data, oldsize);
        }
        PatternCheckSqlFree(*result, mode);
        *result = new_buffer;
    }
    return;
}

#endif
