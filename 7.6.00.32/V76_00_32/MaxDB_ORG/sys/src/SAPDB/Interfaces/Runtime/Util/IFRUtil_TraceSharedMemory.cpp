/*!
  @file           IFRUtil_TraceSharedMemory.cpp
  @author         D039759
  @ingroup        IFR_Trace
  @brief          Shared memory for trace options.

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
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h> 

#include "Interfaces/Runtime/Util/IFRUtil_TraceSharedMemory.h"
#include "Interfaces/Runtime/Util/IFRUtil_Configuration.h"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_Alloca.h"
#include "RunTime/RTE_IniFileHandling.h"

//----------------------------------------------------------------------
IFRUtil_TraceSharedMemory::IFRUtil_TraceSharedMemory()
:m_header(0),
 m_ownpart(0),
 m_totalsize(SHAREDMEM_HEADER_SIZE)
{
    memset(&m_partcopy, 0, sizeof(Part));
}

//----------------------------------------------------------------------
IFRUtil_TraceSharedMemory::~IFRUtil_TraceSharedMemory()
{
    detach();
}

//----------------------------------------------------------------------
IFR_Retcode
IFRUtil_TraceSharedMemory::attach(IFR_ErrorHndl& error)
{
    // First get the name of the shared memory to attach.
    RTE_Path            sharedMemoryLocation;
    tsp00_ErrTextc      errTextc;
    
    if(!RTE_GetUserSpecificConfigPath(sharedMemoryLocation, true, errTextc)) {
        error.setRuntimeError(IFR_ERR_ALLOCSHMEM_FAILED_S, errTextc.asCharp ());
        return IFR_NOT_OK;
    }
    char sharedMemoryName[1024];
    char errortext[512];
    IFR_Retcode rc = IFRUtil_Configuration::getSharedMemoryName(0, sharedMemoryName, 1024, errortext, 512);
    if(rc == IFR_NOT_OK) {
        char buffer[1024];
        sp77sprintf(buffer, 1024, "Could not get shared memory name (%s).", errortext);
        error.setRuntimeError(IFR_ERR_ALLOCSHMEM_FAILED_S, buffer);
        return IFR_NOT_OK;
    } else if(rc == IFR_DATA_TRUNC) {
        error.setRuntimeError(IFR_ERR_ALLOCSHMEM_FAILED_S, "Shared memory name exceeds 1024 characters.");
        return IFR_NOT_OK;
    }
    
    if(strlen(sharedMemoryLocation) + strlen(sharedMemoryName) + 1 >= 1024) {
        error.setRuntimeError(IFR_ERR_ALLOCSHMEM_FAILED_S, "Shared memory path exceeds 1024 characters.");
        return IFR_NOT_OK;
    }
        
    strcpy(m_sharedmemorypath, sharedMemoryLocation);
    strcat(m_sharedmemorypath, sharedMemoryName);
    
    m_header = (Header *) sqlAllocSharedMem(m_sharedmemorypath, SHAREDMEM_HEADER_SIZE);
    if(m_header == 0) {
        error.setRuntimeError(IFR_ERR_ALLOCSHMEM_FAILED_SI, m_sharedmemorypath, SHAREDMEM_HEADER_SIZE);
        return IFR_NOT_OK;
    }
    if(m_header->totalSize > SHAREDMEM_HEADER_SIZE) {
        m_totalsize = m_header->totalSize;
        sqlFreeSharedMem((void *)m_header, SHAREDMEM_HEADER_SIZE);
        m_header = (Header *) sqlAllocSharedMem(m_sharedmemorypath, m_totalsize);
        
        if(m_header == 0) {
            error.setRuntimeError(IFR_ERR_ALLOCSHMEM_FAILED_SI, 
                                  m_sharedmemorypath, 
                                  m_totalsize);
            return IFR_NOT_OK;
        }
    } else {
        m_header->totalSize = SHAREDMEM_HEADER_SIZE;
        // this may be a race condition. 
        // but it appears only if two processes will start the 1st time on this
        // machine, and even here only if they do it exactly at the same time
        if(m_header->initialized == 0) {
            RTESync_Spinlock spinInitializer(&(m_header->spinLock),
                                             true,
                                             false);
            m_header->initialized = 1;
        }
        
    }

    // This abandons the internal spinlock in favor of the 
    // lock in the shared memory
    new (&m_spinlock) RTESync_Spinlock(&(m_header->spinLock), false, false);
    return IFR_OK;
}

//----------------------------------------------------------------------
void
IFRUtil_TraceSharedMemory::detach()
{
    if(m_header) {
        // Avoid segmentation faults if something is called on the spin lock
        new (&m_spinlock) RTESync_Spinlock();
        sqlFreeSharedMem((void *)m_header, m_header->totalSize);
        memset(&m_partcopy, 0, sizeof(Part));
        m_header  = 0;
        m_ownpart = 0;
    }
}
    
//----------------------------------------------------------------------
IFR_Bool
IFRUtil_TraceSharedMemory::isCurrent()
{
    if(m_header == 0) {
        return true;
    } else {
        if(m_header->totalSize > m_totalsize) {
            // printf("HEADER SIZE %d, STORED SIZE %d\n", m_header->totalSize, m_totalsize);
            IFR_Int4 oldTotalSize=m_totalsize;
            m_totalsize = m_header->totalSize;
            new (&m_spinlock) RTESync_Spinlock();
            sqlFreeSharedMem((void *)m_header, oldTotalSize);
            m_header = (Header *) sqlAllocSharedMem(m_sharedmemorypath, m_totalsize);
            
            if(m_header == 0) {
                return true;
            } else {
                new (&m_spinlock) RTESync_Spinlock(&(m_header->spinLock), false, false);
                m_ownpart=getPart();
                return false;
            }
        }
        if(m_partcopy.readCount == m_header->updateCount) {
            return true;
        } else {
            return false;
        }
    }
}

//----------------------------------------------------------------------
IFR_Bool
IFRUtil_TraceSharedMemory::mustReReadConfiguration()
{
    lock();
    if(m_header->globalReRead) {
        Part *p=getPart();
        if(p) {
            p->readCount=m_header->updateCount;
            m_partcopy.readCount=p->readCount;
        } else {
            m_partcopy.readCount=m_header->updateCount;
        }   
        unlock();
        return true;
    } else {
        unlock();
        return false;
    }
}

//----------------------------------------------------------------------
IFR_Bool
IFRUtil_TraceSharedMemory::settingsQueried()
{
    Part *p=getPart();
    if(p == 0) {
        return false;
    } else {
        return p->flags[0] == 'Q';
    }
}

//----------------------------------------------------------------------
IFR_Bool
IFRUtil_TraceSharedMemory::refreshRequested()
{
    Part *p=getPart();
    if(p == 0) {
        return false;
    } else {
        return p->flags[0] == 'R';
    }
}

//----------------------------------------------------------------------
char *
IFRUtil_TraceSharedMemory::flagsChanged()
{
    Part *p=getPart();
    if(p == 0) {
        return 0;
    }
    if(memcmp(p->flags, m_partcopy.flags, 256) == 0) {
        return 0;
    } else {
        lock();
        p->readCount = m_header->updateCount;
        memcpy(&m_partcopy, p, sizeof(Part));
        unlock();
        return m_partcopy.flags;
    }
}
    
//----------------------------------------------------------------------
void
IFRUtil_TraceSharedMemory::setFlags(char *flags)
{
    Part *p = getPart();
    if(p == 0) {
        return;
    }
    lock();
    p->readCount=m_header->updateCount;
    strcpy(p->flags, flags);
    memcpy(&m_partcopy, p, sizeof(Part));
    unlock();
}

//----------------------------------------------------------------------
void
IFRUtil_TraceSharedMemory::acknowledge()
{
    Part *p = getPart();
    if(p == 0) {
        m_partcopy.readCount = m_header->updateCount;
    } else {
        lock();
        p->readCount=m_header->updateCount;
        memcpy(&m_partcopy, p, sizeof(Part));
        unlock();
    }
}



//----------------------------------------------------------------------
static IFR_Int4 IFRUtil_TraceSharedMemory_pid = -1;

//----------------------------------------------------------------------
IFRUtil_TraceSharedMemory::Part *
IFRUtil_TraceSharedMemory::getPart() 
{
    if(m_ownpart == 0) {
        // save the expensive getpid() system call
        // no synchronization, as anyone who wins 
        // will do it right.
        if(IFRUtil_TraceSharedMemory_pid == -1) {
            sqlgetpid(&IFRUtil_TraceSharedMemory_pid);
        }
        return m_ownpart = getPart(IFRUtil_TraceSharedMemory_pid);
    } else {
        return m_ownpart;
    }
}


//----------------------------------------------------------------------
IFRUtil_TraceSharedMemory::Part *
IFRUtil_TraceSharedMemory::getPart(IFR_Int4 pid, IFR_Bool create) 
{
    if(m_header == 0 || pid==0) {
        return 0;
    } else {
        IFR_Int4 i        = 0;
        IFR_Int4 numParts = (m_header->totalSize - SHAREDMEM_HEADER_SIZE) / sizeof(Part);
        char *cp = (char *)m_header;
        cp += SHAREDMEM_HEADER_SIZE;
        Part *p =(Part *) cp;
        for(i=0; i<numParts; ++i) {
            if(p[i].processID == pid) {
                return (Part *)(p + i);
            }
        }
        if(!create) {
            return 0;
        }
        for(i=0; i<numParts; ++i) {
            RTEProc_Handle hnd;
            lock();
            if(!RTEProc_Open(p[i].processID, &hnd)) {
                memset((void *) (p+i), 0, sizeof(Part));
                p[i].processID=pid;
                unlock();
                return (Part *)(p + i);
            }
            if(RTEProc_GetState(hnd) != Alive) {
                RTEProc_Close(hnd);
                memset((void *) (p+i), 0, sizeof(Part));
                p[i].processID=pid;
                unlock();
                return (Part *)(p + i);
            } else {
                unlock();
                RTEProc_Close(hnd);
            }
        }
        // we must expand.
        // It is a little bit nasty - we lock on a spinlock, throw it away, but know as it is
        // in shared memory, it will persist, until we come to unlock it.
        lock();
        m_totalsize += sizeof(Part);
        new (&m_spinlock) RTESync_Spinlock();
        sqlFreeSharedMem((void *)m_header, m_header->totalSize);
        m_header = (Header *) sqlAllocSharedMem(m_sharedmemorypath, m_totalsize);
        if(m_header == 0) {
            // the allocation of more space has failed ... try at least to free the spinlock.
            Header *h = (Header *)sqlAllocSharedMem(m_sharedmemorypath, SHAREDMEM_HEADER_SIZE);
            if(h == 0) {
                // no hope, bail out.
                memset(&m_partcopy, 0, sizeof(Part));
                m_header  = 0;
                m_ownpart = 0;
                return 0;
                
            }
            // at least we can let the other processes go ...
            RTESync_Spinlock unlocker(&(h->spinLock), false, false);
            unlocker.Unlock();
            sqlFreeSharedMem((void *)h, SHAREDMEM_HEADER_SIZE);
            memset(&m_partcopy, 0, sizeof(Part));
            m_header  = 0;
            m_ownpart = 0;
            return 0;
        } else {
            new (&m_spinlock) RTESync_Spinlock(&(m_header->spinLock), false, false);
        }
        m_header->totalSize = m_totalsize;
        p=reinterpret_cast<Part *> (m_header + 1);
        p[numParts].processID = pid;
        unlock();
        return (Part *) p + numParts;
    }
}

//----------------------------------------------------------------------
void 
IFRUtil_TraceSharedMemory::lock()
{
    m_spinlock.Lock();
}

//----------------------------------------------------------------------
void 
IFRUtil_TraceSharedMemory::unlock()
{
    m_spinlock.Unlock();
}


//----------------------------------------------------------------------
void
IFRUtil_TraceSharedMemory::update(IFR_Bool forceGlobalReRead)
{
    if(m_header) {
        lock();
        m_header->globalReRead = forceGlobalReRead ? 1 : 0;
        m_header->updateCount++;
        unlock();
    }
    return;
}

//----------------------------------------------------------------------
IFRUtil_TraceSharedMemory::Part *
IFRUtil_TraceSharedMemory::setFlags(IFR_Int4 pid, char *flags)
{
    Part *p = getPart(pid, true);
    if(p == 0) {
        return 0;
    }
    lock();
    strcpy(p->flags, flags);
    m_header->globalReRead=0;
    m_header->updateCount++;
    unlock();
    return p;
}

//----------------------------------------------------------------------
IFRUtil_TraceSharedMemory::Part *
IFRUtil_TraceSharedMemory::refreshConfig(IFR_Int4 pid)
{
    return setFlags(pid, (char *)"R");
}

//----------------------------------------------------------------------
IFRUtil_TraceSharedMemory::Part *
IFRUtil_TraceSharedMemory::querySettings(IFR_Int4 pid)
{
    return setFlags(pid, (char *)"Q");
}





