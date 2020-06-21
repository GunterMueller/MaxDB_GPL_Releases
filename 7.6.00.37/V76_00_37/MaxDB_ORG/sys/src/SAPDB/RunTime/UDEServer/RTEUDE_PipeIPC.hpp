/*!*********************************************************************

    @file         RTEUDE_PipeIPC.hpp

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

#ifndef RTEUDE_PIPEIPC_HPP
#define RTEUDE_PIPEIPC_HPP

#include "RunTime/UDEServer/RTEUDE_IPC.hpp"

class SAPDBMem_IRawAllocator;

/*!

  @class RTEUDE_PipeIPC

 */
class RTEUDE_PipeIPC : public RTEUDE_IPC
{
public:
#if defined (_WIN32)
    typedef unsigned long pipeHandle_t;
#else
    typedef int pipeHandle_t;
#endif

public:
    RTEUDE_PipeIPC (pipeHandle_t pipeHandle, SAPDBMem_IRawAllocator * allocator);
    virtual ~RTEUDE_PipeIPC ();
    virtual bool isValid () const;
    virtual void close ();
    virtual void destroy ();

protected:

    RTEUDE_PipeIPC ();
    virtual bool rawSend (const void * buf, int length);
    virtual bool rawReceive (void * target, int targetSize, int & outLength);
    void close (pipeHandle_t & handleToClose);
    pipeHandle_t handle;
    SAPDBMem_IRawAllocator * allocator;
};

/*----------------------------------------*/

/*!

  @class RTEUDE_PipeClient

 */
class RTEUDE_PipeClient : public RTEUDE_PipeIPC
{
public:
    RTEUDE_PipeClient (const char * pipeName);
    bool open (const char * pipeName);

protected:
};

/*----------------------------------------*/

/*!

  @class RTEUDE_PipeServer

 */
class RTEUDE_PipeServer
    : public RTEUDE_IPCListener
{
public:
    RTEUDE_PipeServer (const char * pipeName, SAPDBMem_IRawAllocator * allocator);
    virtual ~RTEUDE_PipeServer ();
    virtual RTEUDE_PipeIPC * accept ();
    virtual void close ();

protected:
    typedef RTEUDE_PipeIPC::pipeHandle_t pipeHandle_t;
    bool open ();

    pipeHandle_t handle;
    char pipename [255];
    SAPDBMem_IRawAllocator * allocator;

};


#endif
