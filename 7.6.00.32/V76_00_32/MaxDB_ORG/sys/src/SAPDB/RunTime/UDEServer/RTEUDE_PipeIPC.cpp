/*!*********************************************************************

    @file         RTEUDE_PipeIPC.cpp

    @author       DanielD

    @brief

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2004-2005 SAP AG

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
***********************************************************************/

#include "RunTime/UDEServer/RTEUDE_PipeIPC.hpp"

#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"

#include <stdio.h>
#include <string.h>

#if defined (_WIN32)    /*&if $OSSPEC == WIN32*/
#include <windows.h>
#else                   /*&else*/
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "RunTime/RTE_saveUNIXcalls.h"  /* nocheck but_imkdep */
#endif                  /*&endif*/

#if defined (_WIN32)
const RTEUDE_PipeIPC::pipeHandle_t invalidHandleC
    = (RTEUDE_PipeIPC::pipeHandle_t) INVALID_HANDLE_VALUE;
typedef DWORD counter_t;
#else
const RTEUDE_PipeIPC::pipeHandle_t invalidHandleC = -1;
typedef int counter_t;

#if defined (open)
/* some header #define open to open64 */
#undef open
#endif
#endif

/*----------------------------------------*/

RTEUDE_PipeIPC::
RTEUDE_PipeIPC ()
{
    this->handle = invalidHandleC;
    this->allocator = 0;
}

/*----------------------------------------*/

RTEUDE_PipeIPC::
RTEUDE_PipeIPC (
    pipeHandle_t pipeHandle_init,
    SAPDBMem_IRawAllocator * allocator_init)
{
    this->handle = pipeHandle_init;
    this->allocator = allocator_init;
}

/*----------------------------------------*/

RTEUDE_PipeIPC::
~RTEUDE_PipeIPC ()
{
    this->close ();
}

/*----------------------------------------*/

bool RTEUDE_PipeIPC::
isValid () const
{
    return this->handle != invalidHandleC;
}

/*----------------------------------------*/

void RTEUDE_PipeIPC::
close (
    pipeHandle_t & handleToClose)
{
    if (handleToClose != invalidHandleC) {
#if defined (_WIN32)
        CloseHandle ((HANDLE) handleToClose);
#else
        ::close (handleToClose);
#endif
    }
    handleToClose = invalidHandleC;
}

/*----------------------------------------*/

void RTEUDE_PipeIPC::
close ()
{
    this->close (this->handle);
}

/*----------------------------------------*/

void RTEUDE_PipeIPC::
destroy ()
{
    this->close ();
    if (this->allocator != NULL) {
        SAPDBMem_IRawAllocator * alloc = this->allocator;
        this->~RTEUDE_PipeIPC ();
        alloc->Deallocate (this);
    }
}

/*----------------------------------------*/

bool RTEUDE_PipeIPC::
rawSend (
    const void * buf,
    int length)
{
    counter_t bytesWritten;

#if defined (_WIN32)
     BOOL ok = WriteFile ((HANDLE) this->handle, buf, (counter_t) length,
         &bytesWritten, (LPOVERLAPPED) NULL);
     return ok && (bytesWritten == length);
#else
     bytesWritten = ::write (this->handle, buf, length);
     return bytesWritten == length;
#endif
}

/*----------------------------------------*/

bool RTEUDE_PipeIPC::
rawReceive (
    void * target,
    int targetSize,
    int & outLength)
{
    counter_t bytesRead;
    bool ok;

#if defined (_WIN32)
     ok = ReadFile ((HANDLE) this->handle, target, (counter_t) targetSize,
         &bytesRead, (LPOVERLAPPED) NULL) ? true : false;
#else
     bytesRead = ::read (this->handle, target, targetSize);
     ok = bytesRead >= 0;
#endif
     if (!ok) {
         outLength = 0;
     }
     else {
         outLength = bytesRead;
     }
     return ok;
}

/*----------------------------------------*/

static const char *
expandPipename (
    char * buf,
    const char * pipeName)
{
    strcpy (buf, "\\\\.\\pipe\\");
    strcat (buf, pipeName);
    return buf;
}

/*----------------------------------------*/

RTEUDE_PipeClient::
RTEUDE_PipeClient (
    const char * pipeName)
{
    this->open (pipeName);
}


/*----------------------------------------*/

bool RTEUDE_PipeClient::
open (
    const char * pipeName)
{
#if defined (_WIN32)
    char fullname [255];
    expandPipename (fullname, pipeName);
    this->handle = (pipeHandle_t) CreateFile (fullname,
        GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ,
        (LPSECURITY_ATTRIBUTES) NULL,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
        (HANDLE) NULL);
    if (this->handle == invalidHandleC) {
        int error = GetLastError ();
        int noerror = error - error;
    }
#else
    this->handle = RTE_save_open (pipeName, O_WRONLY);
#endif
    return this->handle != invalidHandleC;
}

/*----------------------------------------*/

RTEUDE_PipeServer::
RTEUDE_PipeServer (
    const char * pipeName_init,
    SAPDBMem_IRawAllocator * allocator_init)
{
    strcpy (this->pipename, pipeName_init);
    this->open ();
    this->allocator = allocator_init;
}

/*----------------------------------------*/

RTEUDE_PipeServer::
~RTEUDE_PipeServer ()
{
#if defined (_WIN32)
#else
    if (this->pipename [0] != '\0') {
        unlink (this->pipename);
    }
#endif
}

/*----------------------------------------*/

void RTEUDE_PipeServer::
close ()
{
}

/*----------------------------------------*/

bool RTEUDE_PipeServer::
open ()
{
#if defined (_WIN32)
    /*
     * windows
     */
    char fullname [255];
    expandPipename (fullname, this->pipename);
    this->handle = (pipeHandle_t) CreateNamedPipe (fullname,
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_BYTE | PIPE_WAIT,
        20, 0, 0, 150,
        (LPSECURITY_ATTRIBUTES) NULL);
    return this->handle != invalidHandleC;
#else
    /*
     * Unix
     */
    umask(0);
    unlink (this->pipename);
    int rc = mknod (this->pipename, S_IFIFO | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP, 0);
    return (rc == 0);
#endif
}

/*----------------------------------------*/

RTEUDE_PipeIPC * RTEUDE_PipeServer::
accept ()
{
    RTEUDE_PipeIPC * result = NULL;

#if defined (_WIN32)
    BOOL ok = ConnectNamedPipe ((HANDLE) this->handle,
            (LPOVERLAPPED) NULL);
    if (ok) {
        /*
         * create new pipe object
         */
        result = new (*this->allocator)
            RTEUDE_PipeIPC (this->handle, this->allocator);
        if (result == NULL) {
            CloseHandle ((HANDLE) this->handle);
        }
        /*
         * reopen named pipe
         */
        this->open ();
    }
#else
    fd_set fds;
    int rc;

    if (this->handle == invalidHandleC) {
        this->handle = RTE_save_open (this->pipename, O_RDONLY);
        if (this->handle == invalidHandleC) {
            return NULL;
        }
    }
    FD_ZERO(&fds);
    FD_SET(this->handle, &fds);

    rc = select(this->handle +1, &fds, NULL, NULL, NULL);
    if (rc < 0) {
        return NULL;
    }

    if (FD_ISSET(this->handle,&fds)) {
        result = new (*this->allocator)
            RTEUDE_PipeIPC (this->handle, this->allocator);
    }
#endif

    return result;
}

/*----------------------------------------*/

