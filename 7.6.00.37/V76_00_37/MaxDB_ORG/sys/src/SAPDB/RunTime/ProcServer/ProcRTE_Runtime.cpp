/*!*********************************************************************

    module:       ProcRTE_Runtime.cpp

    --------------------------------------------------------------------

    responsible:  DanielD

    special area:

    description:

    --------------------------------------------------------------------


    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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




***********************************************************************/

#include "RunTime/ProcServer/ProcRTE_Runtime.hpp"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include "SAPDBCommon/MemoryManagement/SAPDBMem_DefaultRawAllocator.hpp"

#if defined SAPDB_KERNEL

#include "hsp77.h"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"

extern SAPDBTrace_Topic DBProc_Trace; // really from SQLMan_Tracing.hpp

#endif

static bool toDoLogEnabled = false;

/*----------------------------------------*/

static const char * logPrefix = "";

void   ProcRTE_Runtime::setLogPrefix (
    const char * prefix)
{
    logPrefix = prefix;
}

static SAPDBMem_IRawAllocator * globalAllocator = NULL;

#if defined SAPDB_KERNEL

#define GlobalAllocatorClass RTEMem_Allocator
//#define GlobalAllocatorClass SAPDBMem_DefaultRawAllocator

/*----------------------------------------*/

char * ProcRTE_Runtime::alloc (
    int size)
{
    if (globalAllocator == NULL) {
        globalAllocator = &GlobalAllocatorClass::Instance ();
    }
    return static_cast<char *> (globalAllocator->Allocate (size));
}

/*----------------------------------------*/

void *  ProcRTE_Runtime::free (
    void * ptr)
{
    if (ptr != NULL) {
        globalAllocator->Deallocate (ptr);
    }
    return NULL;
}

/*----------------------------------------*/

SAPDBMem_IRawAllocator * ProcRTE_Runtime::allocator ()
{
    if (globalAllocator == NULL) {
        globalAllocator = &GlobalAllocatorClass::Instance ();
    }
    return globalAllocator;
}

/*----------------------------------------*/

static void
logAny (
    const char * prefix,
    const char * format,
    va_list      argptr)
{
    char line [255];
    if (prefix == NULL) {
        sp77sprintf (line, sizeof (line), "%s ", logPrefix);
    }
    else {
        sp77sprintf (line, sizeof (line), "%s %s ", logPrefix, prefix);
    }
    int prefixLen = strlen (line);
    sp77sprintf (line + prefixLen, sizeof (line) - prefixLen,
        format, argptr);
    SAPDBTRACE_WRITELN (DBProc_Trace, 1, line);
}

/*----------------------------------------*/

void   ProcRTE_Runtime::log (
    const char * format, ...)
{
    va_list argptr;

    va_start (argptr, format);
    logAny (NULL, format, argptr);
    va_end (argptr);
}

/*----------------------------------------*/

void   ProcRTE_Runtime::logError (
    const char * format, ...)
{
    va_list argptr;

    va_start (argptr, format);
    logAny ("ERR:", format, argptr);
    va_end (argptr);
}

/*----------------------------------------*/

void   ProcRTE_Runtime::logToDo (
    const char * format, ...)
{
    va_list argptr;

    va_start (argptr, format);
    if (toDoLogEnabled) {
        logAny ("TODO:", format, argptr);
    }
    va_end (argptr);
}

#else

char * ProcRTE_Runtime::alloc (
    int size)
{
    return (char *) malloc (size);
}

/*----------------------------------------*/

void * ProcRTE_Runtime::free (
    void * ptr)
{
    if (ptr != NULL) {
        ::free (ptr);
    }
    return NULL;
}

/*----------------------------------------*/

SAPDBMem_IRawAllocator * ProcRTE_Runtime::allocator ()
{
    if (globalAllocator == NULL) {
        globalAllocator = &SAPDBMem_DefaultRawAllocator::Instance ();
    }
    return globalAllocator;
}

/*----------------------------------------*/

void   ProcRTE_Runtime::log (
    const char * format, ...)
{
    va_list argptr;

    va_start (argptr, format);
    fprintf (stdout, "%s ", logPrefix);
    vfprintf (stdout, format, argptr);
    fprintf (stdout, "\n");
    fflush (stdout);
    va_end (argptr);
}

/*----------------------------------------*/

void   ProcRTE_Runtime::logError (
    const char * format, ...)
{
    va_list argptr;

    va_start (argptr, format);
    fprintf (stdout, "%s ERR:  ", logPrefix);
    vfprintf (stdout, format, argptr);
    fprintf (stdout, "\n");
    fflush (stdout);
    va_end (argptr);
}

/*----------------------------------------*/

void   ProcRTE_Runtime::logToDo (
    const char * format, ...)
{
    va_list argptr;

    va_start (argptr, format);
    if (toDoLogEnabled) {
        fprintf (stdout, "%s TODO: ", logPrefix);
        vfprintf (stdout, format, argptr);
        fprintf (stdout, "\n");
        fflush (stdout);
    }
    va_end (argptr);
}

#endif

/*----------------------------------------*/

void   ProcRTE_Runtime::enableToDoLog ()
{
    toDoLogEnabled = true;
}

/*----------------------------------------*/

void   ProcRTE_Runtime::disableToDoLog ()
{
    toDoLogEnabled = false;
}

