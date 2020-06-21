/*!
  @file           oomtest.cpp
  @author         D039759
  @ingroup        Tests
  @brief          Out-Of-Memory Situations.

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

#include "SAPDB/Interfaces/SQLDBC/tests/SQLDBCTestUtils.h"
#include "SAPDB/Interfaces/SQLDBC/SQLDBC_IRuntime.h"
#include "SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDB/Interfaces/SQLDBC/tests/allsqldbc.h"

#ifndef _WIN32
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#endif

#define CURRENT_NASTY_CONSTANT -1

//----------------------------------------------------------------------
// Runtime instance for OOM Simulation
class OOM_Allocator : public SAPDBMem_IRawAllocator
{
public: 
    OOM_Allocator(SAPDBMem_IRawAllocator *allocator,
                  SQLDBC_Int4 threshold)
    :m_alloc_count(0),
     m_alloc_threshold(threshold),
     m_wrapped_instance(allocator)
    {}

    virtual void *Allocate(SAPDB_ULong byteCount)
    {
        m_alloc_count++;
        if(m_alloc_threshold >= 0 && m_alloc_count >= m_alloc_threshold) {
            if(m_alloc_threshold == CURRENT_NASTY_CONSTANT) {
#ifdef _WIN32
                DebugBreak();
#else
                // Send a SIGTRAP
                kill(getpid(), 5);
#endif
            }
            return 0;
        } else {
            return m_wrapped_instance->Allocate(byteCount);
        }
    }
    
    virtual void* Allocate(SAPDB_ULong ByteCount, const void* Hint)
    {
        m_alloc_count++;
        if(m_alloc_threshold >=0 && m_alloc_count >= m_alloc_threshold) {
            if(m_alloc_threshold == CURRENT_NASTY_CONSTANT) {
#ifdef _WIN32
                DebugBreak();
#else
                // Send a SIGTRAP
                kill(getpid(), 5);
#endif              
            }
            return 0;
        } else {
            return m_wrapped_instance->Allocate(ByteCount, Hint);
        }
    }

    virtual void Deallocate(void* p)
    {
        m_wrapped_instance->Deallocate(p);
    }

    virtual void GetBaseAllocatorCallStatistics(SAPDB_ULong &,SAPDB_ULong &) const
    {
        return;
    }

    virtual void GetCallStatistics(SAPDB_ULong &,SAPDB_ULong &) const
    {
        return;
    }
    
    SQLDBC_Int4 getAllocCount()
    {
        return m_alloc_count;
    }

private:
    SQLDBC_Int4 m_alloc_count;
    SQLDBC_Int4 m_alloc_threshold;
    SAPDBMem_IRawAllocator *m_wrapped_instance;
};


class OOM_Runtime
    : public SQLDBC_IRuntime
{
public:
    OOM_Runtime(SQLDBC_IRuntime *wrappedRuntime,
                SQLDBC_Int4      failAtAlloc)
    :m_allocator(&(wrappedRuntime->getGlobalAllocator()), failAtAlloc),
     m_wrapped_instance(wrappedRuntime),
     SQLDBC_IRuntime(0)
    {
    }

    ~OOM_Runtime()
    {}
    
    virtual SAPDB_Bool checkKeyOption(const char *keyoption,
                                      char *servernode,
                                      SAPDB_Int4& servernodelength,
                                      char *serverdb,
                                      SAPDB_Int4& serverdblength,
                                      SAPDB_Int4& isolationlevel,
                                      SAPDB_Int4& cachelimit,
                                      SAPDB_Int4& sqlmode,
                                      SAPDB_Int4& timeout,
                                      char *username,
                                      SAPDB_Int4& usernamelength,
                                      SQLDBC_StringEncoding& usernameEncoding,
                                      SQLDBC_IRuntime::Error& error)
    {
        return m_wrapped_instance->checkKeyOption(keyoption, 
                                                  servernode,
                                                  servernodelength,
                                                  serverdb,
                                                  serverdblength,
                                                  isolationlevel,
                                                  cachelimit,
                                                  sqlmode,
                                                  timeout,
                                                  username,
                                                  usernamelength,
                                                  usernameEncoding,
                                                  error);
    }

    virtual SAPDBMem_IRawAllocator& getGlobalAllocator()
    {
        return m_allocator;
    }

    virtual SAPDB_Bool getSession(const char *connectUrl,
                                  const char *connectCommand,
                                  const char *password,
                                  SAPDB_Int4  passwordLength,
                                  const SQLDBC_StringEncoding commandEncoding,
                                  SAPDB_Int8&  sessionID,
                                  SAPDB_UInt4& packetSize,
                                  SAPDB_Int4& packetListSize,
                                  void **packetList,  
                                  void **sessionInfoReply,
                                  SQLDBC_IRuntime::Error& error,
                                  SAPDBMem_IRawAllocator* allocator)
    {
        return m_wrapped_instance->getSession(connectUrl,
                                              connectCommand,
                                              password,
                                              passwordLength,
                                              commandEncoding,
                                              sessionID,
                                              packetSize,
                                              packetListSize,
                                              packetList,
                                              sessionInfoReply,
                                              error,
                                              allocator);       
    }

    virtual SAPDB_Bool getSession(const char *connectUrl,
                                  const char *connectCommand,
                                  const char *username,
                                  SAPDB_Int4  usernameLength,
                                  const char *password,
                                  SAPDB_Int4  passwordLength,
                                  const SQLDBC_StringEncoding commandEncoding,
                                  SAPDB_Int8&  sessionID,
                                  SAPDB_UInt4& packetSize,
                                  SAPDB_Int4& packetListSize,
                                  void **packetList,  
                                  void **sessionInfoReply,
                                  SQLDBC_IRuntime::Error& error,
                                  SAPDBMem_IRawAllocator* allocator)
    {
        return m_wrapped_instance->getSession(connectUrl,
                                              connectCommand,
                                              username,
                                              usernameLength,
                                              password,
                                              passwordLength,
                                              commandEncoding,
                                              sessionID,
                                              packetSize,
                                              packetListSize,
                                              packetList,
                                              sessionInfoReply,
                                              error,
                                              allocator);       
    }
    
    virtual SAPDB_Bool releaseSession(SAPDB_Int8 sessionID,
                                      SQLDBC_IRuntime::Error& error)
    {
        return m_wrapped_instance->releaseSession(sessionID, error);
    }

    virtual SAPDB_Bool request(SAPDB_Int8  sessionID,
                               void       *requestData,
                               SAPDB_UInt4 requestDataLength,
                               SQLDBC_IRuntime::Error& error)
    {
        return m_wrapped_instance->request(sessionID, requestData, requestDataLength, error);
    }

    virtual SAPDB_Bool receive(SAPDB_Int8 sessionID,
                               void **replyData,
                               SAPDB_Int4& replyDataLength,
                               SQLDBC_IRuntime::Error& error)
    {
        return m_wrapped_instance->receive(sessionID, replyData, replyDataLength, error);
    }

    virtual SAPDB_Bool cancelCurrentCommand(SAPDB_Int8 sessionID,
                                            SQLDBC_IRuntime::Error& error)
    {
        return m_wrapped_instance->cancelCurrentCommand(sessionID, error);
    }

    virtual SQLDBC_Spinlock* createSpinlock(SAPDBMem_IRawAllocator& allocator){
        return m_wrapped_instance->createSpinlock(allocator);
    }

    virtual SAPDB_Bool releaseSpinlock(SAPDBMem_IRawAllocator& allocator, SQLDBC_Spinlock* lock){
        return m_wrapped_instance->releaseSpinlock(allocator, lock);
    }
    
    virtual SAPDB_Bool createMutex(MutexHandle& mutexHandle, 
                                   SAPDBMem_IRawAllocator& allocator,
                                   SQLDBC_IRuntime::Error& error)
    {
        return m_wrapped_instance->createMutex(mutexHandle, allocator, error);
    }
    
    virtual SAPDB_Bool lockMutex(MutexHandle mutexHandle)
    {
        return m_wrapped_instance->lockMutex(mutexHandle);
    }
    
    virtual SAPDB_Bool releaseMutex(MutexHandle mutexHandle)
    {
        return m_wrapped_instance->releaseMutex(mutexHandle);
    }
    
    virtual SAPDB_Bool destroyMutex(MutexHandle& mutexHandle, 
                                    SAPDBMem_IRawAllocator& allocator,
                                    SQLDBC_IRuntime::Error& error)
    {
        return m_wrapped_instance->destroyMutex(mutexHandle, allocator, error);
    }
    
    virtual SAPDB_Bool  createCounter(CounterHandle& counterHandle,
                                      SAPDBMem_IRawAllocator& allocator,
                                      SQLDBC_IRuntime::Error& error)
    {
        return m_wrapped_instance->createCounter(counterHandle, allocator, error);
    }

    virtual SAPDB_UInt4 nextCounter(CounterHandle counterHandle)
    {
        return m_wrapped_instance->nextCounter(counterHandle);
    }
    
    virtual SAPDB_Bool  destroyCounter(CounterHandle counterHandle,
                                      SAPDBMem_IRawAllocator& allocator,
                                      SQLDBC_IRuntime::Error& error)
    {
        return m_wrapped_instance->destroyCounter(counterHandle, allocator, error);
    }
           
    virtual TaskID getCurrentTaskID()
    {
        return m_wrapped_instance->getCurrentTaskID();
    }
                                      
    virtual SAPDB_Bool createSemaphore(SemaphoreHandle& semaphoreHandle,
                                       SAPDB_Int4 initialValue,
                                       SAPDBMem_IRawAllocator& allocator,
                                       SQLDBC_IRuntime::Error& error)
    {
        return m_wrapped_instance->createSemaphore(semaphoreHandle, initialValue, allocator, error);
    }

    virtual SAPDB_Bool waitSemaphore(SemaphoreHandle semaphoreHandle)
    {
        return m_wrapped_instance->waitSemaphore(semaphoreHandle);
    }

    virtual SAPDB_Bool signalSemaphore(SemaphoreHandle semaphoreHandle)
    {
        return m_wrapped_instance->signalSemaphore(semaphoreHandle);
    }
    
    virtual SAPDB_Bool destroySemaphore(SemaphoreHandle& semaphoreHandle,
                                        SAPDBMem_IRawAllocator& allocator,
                                        SQLDBC_IRuntime::Error& error)
    {
        return m_wrapped_instance->destroySemaphore(semaphoreHandle, allocator, error);
    }
    
    virtual TaskTraceContext* getTaskTraceContext()
    {
        return m_wrapped_instance->getTaskTraceContext();
    }
    
	virtual void write(const char *s, SAPDB_Int4 size)
    {
        return m_wrapped_instance->write(s, size);
    }
    
    virtual void writeln(const char *s, SAPDB_Int4 size)
    {
        return m_wrapped_instance->writeln(s, size);
    }
    
    SQLDBC_Int4 getAllocCount()
    {
        return m_allocator.getAllocCount();
    }

    void addTraceFlags(unsigned int flags)
    {
	m_wrapped_instance->addTraceFlags(flags);
    }

    virtual void removeTraceFlags(unsigned int flags)
    {
	m_wrapped_instance->removeTraceFlags(flags);
    }

private:
    OOM_Allocator    m_allocator;
    SQLDBC_IRuntime *m_wrapped_instance;  

};

int run_one_test(int argc, char **argv, SQLDBC_Int4& alloccount)
{
    char errorText[512];
    OOM_Runtime oom_runtime(SQLDBC::GetClientRuntime(errorText, sizeof(errorText)), 
                            alloccount);
    SQLDBC_TestFrame testframe(argc, argv, &oom_runtime, true);
    
    if(testframe.getLastError() != 0) {
        if(testframe.getLastError() != -10760) {
            fprintf(testframe.errorstream(), "ERROR:oomtest: Found error %d %s instead of -10760.\n",
                    testframe.getLastError(),
                    testframe.getLastErrorMessage());
            return 1;
        } else {
            fprintf(testframe.infostream(), "INFO: Expected error was found for run %d.\n", alloccount);          
            return 0;
        }
    }
    testframe.addTestCase (new ConnectProperties());
    testframe.addTestCase (new Connection());
    testframe.addTestCase (new Environment());
    testframe.addTestCase (new ErrorHndl());
    testframe.addTestCase (new ParameterMetaData());
    testframe.addTestCase (new PreparedStatement());
    testframe.addTestCase (new ResultSet());
    testframe.addTestCase (new ResultSetMetaData());
    testframe.addTestCase (new RowSet());
    testframe.addTestCase (new UpdatableRowSet());
    testframe.addTestCase (new SQLWarning());
    testframe.addTestCase (new Statement());

    testframe.run();
    if(alloccount == -1) {
        alloccount = oom_runtime.getAllocCount();
        return 0;
    } else if(testframe.getLastError() != -10760 && testframe.getLastError()!=0) {
        fprintf(testframe.errorstream(), "ERROR:oomtest: Found error %d %s instead of -10760.\n",
                testframe.getLastError(),
                testframe.getLastErrorMessage());
        return 1;
    } else {
        fprintf(testframe.infostream(), "INFO: Expected error was found for run %d.\n", alloccount);
        return 0;
    }
}



int main (int argc, char **argv)
{

    SQLDBC_Int4 startcount = 0;
    SQLDBC_Int4 endcount = 0;
    for(int i=1; i<argc; ++i) {
        if((strcmp(argv[i], "-startcount") == 0) && i != argc - 1) {
            startcount = atoi(argv[i+1]);
        }
        if((strcmp(argv[i], "-endcount") == 0) && i != argc - 1) {
            endcount = atoi(argv[i+1]);
        }
    }
    SQLDBC_Int4 alloc_count = -1;
    int succ = run_one_test(argc, argv, alloc_count);
    if(succ) {
        return succ;
    } else {
        if(alloc_count > endcount && endcount != 0) {
            alloc_count = endcount;
        }
        for(SQLDBC_Int i=startcount; i<alloc_count; ++i) {
            fprintf(stdout, "INFO:oomtest: Loop %d of %d.\n", i, alloc_count);
            succ = run_one_test(argc, argv, i);
            if(succ) {
                exit(1);
            }
        }
    }
    return 0;
}



