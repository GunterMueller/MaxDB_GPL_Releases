/*!
  @file           IFRUtil_TraceSharedMemory.h
  @author         D039759
  @ingroup        IFR_Trace
  @brief          Support class for shared memory where the information about trace flags is kept.

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
#ifndef IFRUTIL_TRACESHAREDMEMORY_H
#define IFRUTIL_TRACESHAREDMEMORY_H

#include "Interfaces/Runtime/IFR_Common.h"
#include "Interfaces/Runtime/IFR_ErrorHndl.h"
#include "SAPDBCommon/SAPDB_ReuseLib.h"
#include "SAPDB/Interfaces/SQLDBC/SQLDBC_IRuntime.h"
#include "SAPDB/RunTime/Synchronisation/RTESync_Spinlock.hpp"

IFR_BEGIN_NAMESPACE

#include <stdio.h>

#define SHAREDMEM_HEADER_SIZE 256

enum IFRUtil_TraceSharedMemory_TraceProfileAction {
      IFR_TraceProfileAction_NOTSET  = 0,
      IFR_TraceProfileAction_RESET   = 1,
      IFR_TraceProfileAction_COLLECT = 2
    };

/**
 * Class for accessing the trace shared memory.
 */
class IFRUtil_TraceSharedMemory
{
public:
    struct Header
    {
        IFR_Int4 updateCount;
        IFR_Int4 totalSize;
        IFR_Int4 version;
        IFR_Int4 globalReRead;
        IFR_Int4 initialized;
        RTE_Lock          spinLock;
    };
    
    struct Part
    {
        IFR_Int4 processID;
        IFR_Int4 readCount;
        char     flags[256];
    };

    
    /**
     * Constructor.
     * Does not attach to the shared memory.
     */
    IFRUtil_TraceSharedMemory();

    /**
     * Destructor. Does detach from the shared memory, 
     * if not yet done.
     */
    ~IFRUtil_TraceSharedMemory();
    
    IFR_Retcode attach(IFR_ErrorHndl& error);
    void        detach();
    
    Part *getPart();
    Part *getPart(IFR_Int4 pid, IFR_Bool create=false);
    IFR_Bool isCurrent();
    IFR_Bool mustReReadConfiguration();
    IFR_Bool settingsQueried();
    IFR_Bool refreshRequested();
    char *flagsChanged();
    
    void setFlags(char *flags);
    
    void acknowledge();
    
    inline Header *getHeader() 
    { 
        return (Header *)m_header; 
    }
    
    inline Part  *getParts() 
    { 
        if(m_header == 0) {
            return 0;
        } else {
            char *hp = (char *)m_header;
            hp += SHAREDMEM_HEADER_SIZE;
            return (Part *) hp;
        }
    }
        
    void update(IFR_Bool forceGlobalReRead);
    Part *setFlags(IFR_Int4 pid, char *flags);
    Part *refreshConfig(IFR_Int4 pid);
    Part *querySettings(IFR_Int4 pid);
    
private:    
    void lock();
    void unlock();
    
    Header          *m_header;
    Part            *m_ownpart;
    IFR_Int4         m_totalsize;
    Part             m_partcopy; 
    RTESync_Spinlock m_spinlock;
    char             m_sharedmemorypath[1024];
};

IFR_END_NAMESPACE

#endif
