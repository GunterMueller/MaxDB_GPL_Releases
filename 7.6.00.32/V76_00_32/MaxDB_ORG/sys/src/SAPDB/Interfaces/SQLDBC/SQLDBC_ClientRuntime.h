/*!
  @file           SQLDBC_ClientRuntime
  @author         D039759
  @ingroup        SQLDBC
  @brief          Interface to the runtime environment.
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
#ifndef SQLDBC_CLIENTRUNTIME_H
#define SQLDBC_CLIENTRUNTIME_H

#include "SAPDB/Interfaces/SQLDBC/SQLDBC_IRuntime.h"
#include "SAPDB/Interfaces/SQLDBC/SQLDBC.h"
#include "Interfaces/Runtime/IFR_Common.h"
#include "Interfaces/Runtime/Util/IFRUtil_DefaultRawAllocator.h"
#include "Interfaces/Runtime/Util/IFRUtil_TraceSharedMemory.h"

#include "SAPDBCommon/SAPDB_ReuseLib.h"
#include "heo07.h"
#include "RunTime/Synchronisation/RTESync_Spinlock.hpp"
#include "RunTime/Security/RTESec_Authentication.hpp"
#include "Interfaces/Runtime/IFR_Environment.h"
#include "Interfaces/Runtime/IFR_ConnectProperties.h"

namespace SQLDBC {
    class SQLDBC_Connection;
}

//----------------------------------------------------------------------
class SQLDBC_ClientRuntime_TraceWriter;

SQLDBC_DLLEXPORT SQLDBC_IRuntime *
SQLDBC_ClientRuntimeInit();

struct SQLDBC_ClientAuthenticationInfo
{
    char                             *name;
    RTESec_ClientAuthenticationBuffer buffer;
    void *                            challenge_response;
    SAPDB_Int4                        challenge_response_length;
    void *                            propertylist;
    SAPDB_Int4                        propertylist_length;
};
//----------------------------------------------------------------------
class SQLDBC_ClientRuntime
    : public SQLDBC_IRuntime
{
public:
    SQLDBC_ClientRuntime();
    virtual ~SQLDBC_ClientRuntime();

    virtual const char *getIdentifier() const;

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
                                      SQLDBC_IRuntime::Error& error);

    virtual SAPDBMem_IRawAllocator& getGlobalAllocator();

    virtual SAPDB_Bool getSession(const char  *connectUrl,
                                  const char  *connectCommand,
                                  const char  *password,
                                  SAPDB_Int4    passwordLength,
                                  const SQLDBC_StringEncoding commandEncoding,
                                  SAPDB_Int8&  sessionID,
                                  SAPDB_UInt4& packetSize,
                                  SAPDB_Int4&  packetListSize,
                                  void       **packetList,  
                                  void       **sessionInfoReply,
                                  SQLDBC_IRuntime::Error& error,
                                  SAPDBMem_IRawAllocator *allocator=0);
    
    virtual SAPDB_Bool getSession(const char *connectUrl,
                                  const char *connectCommand,
                                  const char *username,
                                  SAPDB_Int4  usernamelength,
                                  const char *password,
                                  SAPDB_Int4  passwordLength,
                                  const SQLDBC_StringEncoding commandEncoding,
                                  SAPDB_Int8&  sessionID,
                                  SAPDB_UInt4& packetSize,
                                  SAPDB_Int4& packetListSize,
                                  void **packetList,  
                                  void **sessionInfoReply,
                                  SQLDBC_IRuntime::Error& error,
                                  SAPDBMem_IRawAllocator* allocator = 0);


    SAPDB_Bool connectSession(const char   *connectUrl,
                              SAPDB_Int8&   sessionID,
                              SAPDB_UInt4&  packetSize,
                              SAPDB_Int4&   packetListSize,
                              void        **packetList,  
                              SQLDBC_IRuntime::Error&     error,
                              SAPDBMem_IRawAllocator *allocator,
                              SQLDBC_Int4 connectType);


    virtual SAPDB_Bool releaseSession(SAPDB_Int8 sessionID,
                                      SQLDBC_IRuntime::Error& error);

    virtual SAPDB_Bool request(SAPDB_Int8  sessionID,
                               void       *requestData,
                               SAPDB_UInt4 requestDataLength,
                               SQLDBC_IRuntime::Error& error); 

    virtual SAPDB_Bool receive(SAPDB_Int8 sessionID,
                               void **replyData,
                               SAPDB_Int4& replyDataLength,
                               SQLDBC_IRuntime::Error& error);
    
    virtual SAPDB_Bool isReplyAvailable(SAPDB_Int8 sessionID);
    
    virtual SAPDB_Bool cancelCurrentCommand(SAPDB_Int8 sessionID,
                                            SQLDBC_IRuntime::Error& error);
    
    // -- synchronisation primitives
    virtual SQLDBC_Spinlock* createSpinlock(SAPDBMem_IRawAllocator& allocator);

    virtual SAPDB_Bool releaseSpinlock(SAPDBMem_IRawAllocator& allocator, SQLDBC_Spinlock* lock);
    virtual SAPDB_Bool createMutex(MutexHandle& mutexHandle, 
                                   SAPDBMem_IRawAllocator& allocator,
                                   SQLDBC_IRuntime::Error& error);
    virtual SAPDB_Bool lockMutex(MutexHandle mutexHandle);
    virtual SAPDB_Bool releaseMutex(MutexHandle mutexHandle);
    virtual SAPDB_Bool destroyMutex(MutexHandle& mutexHandle, 
                                    SAPDBMem_IRawAllocator& allocator,
                                    SQLDBC_IRuntime::Error& error);


    virtual SAPDB_Bool  createCounter(CounterHandle& counterHandle,
                                      SAPDBMem_IRawAllocator& allocator,
                                      SQLDBC_IRuntime::Error& error);
    virtual SAPDB_UInt4 nextCounter(CounterHandle counterHandle);
    virtual SAPDB_Bool  destroyCounter(CounterHandle counterHandle,
                                       SAPDBMem_IRawAllocator& allocator,
                                       SQLDBC_IRuntime::Error& error);
    
    virtual TaskID getCurrentTaskID();                                    
                                           
    virtual SAPDB_Bool createSemaphore(SemaphoreHandle& semaphoreHandle,
                                       SAPDB_Int4 initialValue,
                                       SAPDBMem_IRawAllocator& allocator,
                                       SQLDBC_IRuntime::Error& error);
                                       
    virtual SAPDB_Bool waitSemaphore(SemaphoreHandle semaphoreHandle);   
    virtual SAPDB_Bool signalSemaphore(SemaphoreHandle semaphoreHandle);                                        
    virtual SAPDB_Bool destroySemaphore(SemaphoreHandle& semaphoreHandle,
                                        SAPDBMem_IRawAllocator& allocator,
                                        SQLDBC_IRuntime::Error& error);

    virtual TaskTraceContext* getTaskTraceContext();
    virtual void write(const char *s, SAPDB_Int4 size);
    virtual void writeln(const char *s, SAPDB_Int4 size);
    virtual void updateTraceFlags(SAPDB_Bool& traceEnabled);
    virtual SAPDB_Bool traceError(SAPDB_Int4 errorcode);
    
    virtual void setTraceOptions(const char *options);
    virtual SAPDB_Int4 getTraceOptions(char *options, SAPDB_Int4 optionslength);
    

    void getTraceOptionsFromConfig();
    
    void checkTraceUpdate();
    
    void getFlags(char *flags);
    
    void addTraceFlags(unsigned int flags);
    void removeTraceFlags(unsigned int flags);


    void write_singlethreaded(TaskTraceContext *context, const char *s, SAPDB_Int4 size);
    void writeln_singlethreaded(TaskTraceContext *context, const char *s, SAPDB_Int4 size);


    virtual SAPDB_Bool getSessionInformation(SQLDBC::SQLDBC_Connection *connection,
                                             SAPDB_Int8& sessionID,
                                             SAPDB_UInt4& packetSize,
                                             SAPDB_UInt4& pagesSize,
                                             SAPDB_Int4&  packetListSize,
                                             void **packetList,
                                             char *usergroup,
                                             char *sysdba,
                                             SQLDBC_StringEncoding& userencoding);
    
protected:
    SAPDB_Bool parseConnectURL(const char *connectURL,
                               IFR_ConnectProperties& properties,
                               SQLDBC_IRuntime::Error& error);
    
private:
    friend class SQLDBC_SingleThreadedRuntime;
    void dumpProfile();
    void setTraceOptions(const char *options, SAPDB_Bool fromAPI);

    IFR_Retcode initiateChallengeResponse(SAPDB_Int8& sessionID,
                                          const char *username,
                                          SAPDB_Int4  usernamelength,
                                          IFR_StringEncoding encoding,
                                          int client_unicode,
                                          int client_sqlmode,
                                          const char *client_application,
                                          const char *client_version,
                                          IFRUtil_Vector<SQLDBC_ClientAuthenticationInfo>& client_authentication,
                                          void      **packetList,
                                          SAPDB_UInt4 packetSize,
                                          SQLDBC_IRuntime::Error& error,
                                          SAPDBMem_IRawAllocator* allocator);
    
    /**
     * @ingroup IFR_Connection
     * Lock instance for global connection lock.
     * 
     * The methods for establishing a connection in the Runtime Environment
     * must not interfere with the request/receive functions in the 
     * following way:
     * - There must be only one thread doing an <code>SqlAConnect</code>
     *   call, and no other thread can do a <code>sqlarequest</code>,
     *   <code>sqlareceive</code>, <code>sqlacancel</code> 
     *   or <code>sqlarelease</code> meanwhile.
     * - There can be an unlimited number of threads that perform
     *   <code>sqlareceive</code>,<code>sqlarequest</code>, or <code>
     *   <code>sqlarelease</code>.
     * - The restriction applies only under UNIX-like operating systems.
     * 
     * The implementation makes a read-write lock, <code>sqlarequest</code>
     * or <code>sqlareceive</code> impose a read lock, the other cited 
     * methods impose a write lock. Read locks (existing connections doing
     * requests have priority, that means they will do queue-jumping if they
     * arrive while a write-locking request is on the way.
     */
    class ConnectLock
    {
    public:
#ifdef WIN32
        inline ConnectLock()    {}
        inline ~ConnectLock()   {}
        inline void lockExecute() {}
        inline void releaseExecute() {}
        inline void lockConnect() {}
        inline void releaseConnect() {}
#else
        ConnectLock();
        ~ConnectLock();
        void lockExecute();
        void releaseExecute();
        void lockConnect();
        void releaseConnect();
    private:
        IFR_Bool              m_writelocking;
        IFR_Int4              m_waitingreaders;
        IFR_Int4              m_waitingwriters;
        IFR_Int4              m_activereaders;
        RTESync_Spinlock      m_datalock;
        teo07_Mutex           m_writerlock;
        teo07_ThreadSemaphore m_readersem;
#endif
    };

    struct TraceSettings
    {
        IFR_Int1 flags;
        IFR_Int4 traceableLimit;
        IFR_Int4 filesizeLimit;
        IFR_Bool timestampPrefix;
        IFR_Bool stopOnError;
        IFR_Int4 stopOnErrorCount;
        IFR_Int4 stopOnErrorCode;
        IFRUtil_TraceSharedMemory_TraceProfileAction profileAction;
    };

private:
    struct SessionInfo 
    {
        SAPDB_Int8   session_id;
        SAPDB_UInt4  packet_size;
        SAPDB_Int4   page_size;
        SAPDB_Int4   packet_list_size;
        void *       packet_list[10];
        char         user_info[64 * 3 + 16];
        SAPDB_Bool   user_info_ucs2;
        SessionInfo *next;
    };

private:
    void createSessionInfo(SAPDB_Int8 session_id,
                           SAPDB_UInt4 packet_size,
                           SAPDB_Int4  packet_list_size,
                           void     ** packet_list,
                           char        user_info[64 * 3 + 16],
                           SAPDB_Bool  user_info_ucs2);

    IFRUtil_DefaultRawAllocator       m_allocator;
    TraceSettings                     m_tracesettings;
    SQLDBC_Int4                       m_errorcounter;
    SQLDBC_ClientRuntime_TraceWriter *m_tracewriter;
    IFRUtil_TraceSharedMemory         m_sharedmem;
    TaskTraceContext                  m_traceoptions;
    SessionInfo                      *m_sessioninfolist;
protected:
    ConnectLock                       m_connectlock;
    IFR_Profile*                      m_profilePointer;
};

class SQLDBC_SingleThreadedRuntime_SpinLock
  : public SQLDBC_Spinlock {
  public:
    SQLDBC_SingleThreadedRuntime_SpinLock (){};
    virtual ~SQLDBC_SingleThreadedRuntime_SpinLock() {}
  
    virtual SAPDB_Bool lock() {   
      return SQLDBC_TRUE;}
    
    virtual SAPDB_Bool unlock() {
      return SQLDBC_TRUE;}

    virtual SAPDB_Bool deleteLock() {
      return SQLDBC_TRUE;}
};


class SQLDBC_SingleThreadedRuntime : public SQLDBC_IRuntime {
public:
    static  SQLDBC_IRuntime *  GetSingleThreadClientRuntime(char *errorText, const SQLDBC_Int4 errorTextSize );

    virtual const char *getIdentifier() const;

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
                                      SQLDBC_IRuntime::Error& error);

    virtual SAPDBMem_IRawAllocator& getGlobalAllocator();

    virtual SAPDB_Bool getSession(const char  *connectUrl,
                                  const char  *connectCommand,
                                  const char  *password,
                                  SAPDB_Int4    passwordLength,
                                  const SQLDBC_StringEncoding commandEncoding,
                                  SAPDB_Int8&  sessionID,
                                  SAPDB_UInt4& packetSize,
                                  SAPDB_Int4&  packetListSize,
                                  void       **packetList,  
                                  void       **sessionInfoReply,
                                  SQLDBC_IRuntime::Error& error,
                                  SAPDBMem_IRawAllocator *allocator=0);
    
    virtual SAPDB_Bool getSession(const char *connectUrl,
                                  const char *connectCommand,
                                  const char *username,
                                  SAPDB_Int4  usernamelength,
                                  const char *password,
                                  SAPDB_Int4  passwordLength,
                                  const SQLDBC_StringEncoding commandEncoding,
                                  SAPDB_Int8&  sessionID,
                                  SAPDB_UInt4& packetSize,
                                  SAPDB_Int4& packetListSize,
                                  void **packetList,  
                                  void **sessionInfoReply,
                                  SQLDBC_IRuntime::Error& error,
                                  SAPDBMem_IRawAllocator* allocator = 0);


    SAPDB_Bool connectSession(const char   *connectUrl,
                              SAPDB_Int8&   sessionID,
                              SAPDB_UInt4&  packetSize,
                              SAPDB_Int4&   packetListSize,
                              void        **packetList,  
                              SQLDBC_IRuntime::Error&     error,
                              SAPDBMem_IRawAllocator *allocator);
                              
    virtual SAPDB_Bool releaseSession(SAPDB_Int8 sessionID,
                                      SQLDBC_IRuntime::Error& error);

    virtual SAPDB_Bool request(SAPDB_Int8  sessionID,
                               void       *requestData,
                               SAPDB_UInt4 requestDataLength,
                               SQLDBC_IRuntime::Error& error); 

    virtual SAPDB_Bool receive(SAPDB_Int8 sessionID,
                               void **replyData,
                               SAPDB_Int4& replyDataLength,
                               SQLDBC_IRuntime::Error& error);
    
    virtual SAPDB_Bool isReplyAvailable(SAPDB_Int8 sessionID);
    
    virtual SAPDB_Bool cancelCurrentCommand(SAPDB_Int8 sessionID,
                                            SQLDBC_IRuntime::Error& error);
    
    virtual SQLDBC_Spinlock* createSpinlock(SAPDBMem_IRawAllocator& allocator){
         return &m_dummySpinlock;
    }
    virtual SAPDB_Bool releaseSpinlock(SAPDBMem_IRawAllocator& allocator, SQLDBC_Spinlock* lock){
      return true;
    }
        
    virtual SAPDB_Bool createMutex(MutexHandle& mutexHandle, 
                                   SAPDBMem_IRawAllocator& allocator,
                                   SQLDBC_IRuntime::Error& error);
    virtual SAPDB_Bool lockMutex(MutexHandle mutexHandle);
    virtual SAPDB_Bool releaseMutex(MutexHandle mutexHandle);
    virtual SAPDB_Bool destroyMutex(MutexHandle& mutexHandle, 
                                    SAPDBMem_IRawAllocator& allocator,
                                    SQLDBC_IRuntime::Error& error);


    virtual SAPDB_Bool  createCounter(CounterHandle& counterHandle,
                                      SAPDBMem_IRawAllocator& allocator,
                                      SQLDBC_IRuntime::Error& error);
    virtual SAPDB_UInt4 nextCounter(CounterHandle counterHandle);
    virtual SAPDB_Bool  destroyCounter(CounterHandle counterHandle,
                                       SAPDBMem_IRawAllocator& allocator,
                                       SQLDBC_IRuntime::Error& error);
    
    virtual TaskID getCurrentTaskID();                                    
                                           
    virtual SAPDB_Bool createSemaphore(SemaphoreHandle& semaphoreHandle,
                                       SAPDB_Int4 initialValue,
                                       SAPDBMem_IRawAllocator& allocator,
                                       SQLDBC_IRuntime::Error& error);
                                       
    virtual SAPDB_Bool waitSemaphore(SemaphoreHandle semaphoreHandle);   
    virtual SAPDB_Bool signalSemaphore(SemaphoreHandle semaphoreHandle);                                        
    virtual SAPDB_Bool destroySemaphore(SemaphoreHandle& semaphoreHandle,
                                        SAPDBMem_IRawAllocator& allocator,
                                        SQLDBC_IRuntime::Error& error);

    virtual TaskTraceContext* getTaskTraceContext();
    virtual void write(const char *s, SAPDB_Int4 size);
    virtual void writeln(const char *s, SAPDB_Int4 size);
    virtual void updateTraceFlags(SAPDB_Bool& traceEnabled);
    virtual void addTraceFlags(unsigned int flags);
    virtual void removeTraceFlags(unsigned int flags);

    virtual SAPDB_Bool traceError(SAPDB_Int4 errorcode);
    
    virtual void setTraceOptions(const char *options);
    virtual SAPDB_Int4 getTraceOptions(char *options, SAPDB_Int4 optionslength);

    virtual SAPDB_Bool getSessionInformation(SQLDBC::SQLDBC_Connection *connection,
                                             SAPDB_Int8& sessionID,
                                             SAPDB_UInt4& packetSize,
                                             SAPDB_UInt4& pagesSize,
                                             SAPDB_Int4&  packetListSize,
                                             void **packetList,
                                             char *usergroup,
                                             char *sysdba,
                                             SQLDBC_StringEncoding& userencoding);
    void getTraceOptionsFromConfig();
    
    void checkTraceUpdate();
    
    void getFlags(char *flags);
    
    void write_singlethreaded(TaskTraceContext *context, const char *s, SAPDB_Int4 size);
    void writeln_singlethreaded(TaskTraceContext *context, const char *s, SAPDB_Int4 size);

private:
	SQLDBC_SingleThreadedRuntime(SQLDBC_ClientRuntime *baseruntime);
    SQLDBC_ClientRuntime* m_runtime;
    IFR_Int4 m_dummy;
    SQLDBC_SingleThreadedRuntime_SpinLock m_dummySpinlock;
};


#endif
