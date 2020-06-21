/*!
 * \file    OMS_SessionDef.hpp
 * \author  MarkusSi, PeterG, Roterring
 * \brief   OMS session definition.
 */
/*

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



*/

#ifndef __OMS_SESSIONDEF_HPP
#define __OMS_SESSIONDEF_HPP

#include "Oms/OMS_Monitor.hpp"
#include "Oms/OMS_BeforeImageList.hpp"
#include "Oms/OMS_VarObj.hpp"
#include "Oms/OMS_MassDerefIterators.hpp"    // PTS 1120478
#include "Oms/OMS_StreamRegistry.hpp"
#include "Oms/OMS_ContainerHandle.hpp"  
#include "Oms/OMS_ClassDirectory.hpp"
#include "Oms/OMS_RWLock.hpp"
#include "Oms/OMS_KeyDesc.hpp"
#include "Oms/OMS_DynamicStack.hpp"
#include "Oms/OMS_UserAllocator.hpp"
#include "Oms/OMS_CheckedUserAllocator.hpp"

#include "geo00.h"
#include "ggg251.h"
#include "ggg01.h"

class OMS_ISessionLockObjects;
class OMS_ClassEntry;
class OmsCallbackInterface;
class OmsIOidReadIterator;
class OmsIOidAppender;
class OmsAbstractObject;
class OmsObjectContainer;

/// Implements an OMS Session representing a kernel session in the OMS.
class OMS_Session {   
#ifndef USE_SYSTEM_ALLOC_CO13
  class OMS_SessionAllocator : public SAPDBMem_RawAllocator
  {
  public :
      /*! constructor */
      OMS_SessionAllocator(tsp00_TaskId taskId);

      //void *Allocate(size_t size);
      //void Deallocate(void *p);
      
  private :
      /*! Tracer for the m_heap */
      OMS_AllocatorTracer m_heapTracer;    
  };
  OMS_SessionAllocator m_heap;    // caution! must be declared prior to m_classDir, m_rwLockDir etc to
                                  // ensure correct sequence of destruction
#endif

public:
#define RESCHEDULE_DISTANCE 1000 // call dispatcher every 1000 derefs

  class MethodCallEplilogScope
  {
  public:
    MethodCallEplilogScope(OMS_Session& session);
    ~MethodCallEplilogScope();
  private:
    OMS_Session& m_session;
  };

  friend class OMS_Session::MethodCallEplilogScope;

  OMS_Session(
    IliveCacheSink* lcSink,
    int cntRegions, 
    pasbool* pToCancel,
    bool &optimizedStreamCommunication, // PTS 1130281
    tsp00_TaskId taskId);
  ~OMS_Session();
  inline void* operator new(size_t sz);
#if defined(OMS_PLACEMENT_DELETE)
  inline void  operator delete(void* p);
#endif
  inline void                  ActivateDefaultContext();
  void                         AssignLcSink(IliveCacheSink* pSink); /* PTS 1109371 */
  inline void                  AssertNotReadOnly(const char* callerMsg);
  const  OMS_ObjectId8&        CastOid(ClassID castToGuid, const OMS_ObjectId8& oid);
  void                         ChangedConsistentView();
#if defined(OMSTST)
  inline void                  CheckCancelCountDown();    // PTS 1112358
  inline void                  CheckExceptionCountDown(); // PTS 1112358
#endif
  void                         RescheduleAndCheckCancelFlag();
  inline void                  CheckHeapThresholds(); /* PTS 1110149 */
  void                         ClearDefaultContext();
  inline bool                  ClientSupportsOptimizedStreams() const; // PTS 1130281
  inline void                  CloseVersion();
  inline void                  CommitSubtrans(int requiredLevel);
  inline void                  ClearTransVersions(bool isCommit, int &phase);
  inline void                  CreateDefaultContext();
  void                         CreateVersion(const OmsVersionId& versionId, const OmsTypeWyde* desc);  // PTS 1117690
  inline int                   CurrentSubtransLevel() const;
  inline OMS_Context*          CurrentContext() const;
  void                         CurrentMethodCallEpilog(tsp00_Int4 runTime);
  inline void                  DecSubtransLevel();
  void                         DeleteAll(ClassID guid, OmsSchemaHandle Schema, OmsContainerNo ContainerNo);
  inline void                  DeleteSession();
  void                         DeleteVarObject(const OMS_ObjectId8& oid);
  inline OmsAbstractObject*    DeRef (const OMS_ObjectId8& oid, ClassID guid, bool forUpd, bool doLock, bool shared);
  int                          MassDeref(OmsIDerefIter &derefIter);                                                // PTS 1120478
  int                          MassDerefForUpd(OmsIDerefIterForUpd &derefIter, bool doLock, bool shared);          // PTS 1120478
  int                          MassDerefViaKey(OmsIDerefKeyIter &derefIter);                                       // PTS 1120478
  int                          MassDerefViaKeyForUpd(OmsIDerefKeyIterForUpd &derefIter, bool doLock, bool shared); // PTS 1120478
  inline OmsAbstractObject*    DeRefViaKey (unsigned char* key, ClassID, bool forUpd, bool doLock, bool shared, OmsSchemaHandle, OmsContainerNo);
  const  void*                 DeRefVarObject(const OMS_ObjectId8& oid);
  void*                        DeRefVarObjForUpd(const OMS_ObjectId8& oid, unsigned long &size);
  void                         DropVersionEpilog(OMS_Context* context); 
  void                         DropVersionProlog(OMS_Context* context);
  void                         Dump(OMS_DumpInterface& dumpObj) const;
  inline void                  ExecuteSubtransCommit(int requiredLevel);
  inline void                  deallocate(void* p);
  inline bool                  ForceReadOnly();
  inline void                  FreeBeforeImages(int level);
  inline OmsAbstractObject*    ForUpdPtr(OmsObjectContainer* pObj);
  inline OmsCallbackInterface* GetCallbackInterface() const;
  inline OmsCallbackInterface* GetCallbackInterface(ClassID guid) const;
  inline OMS_ClassDirectory&   GetClassDir();
  bool                         GetMethodCallMonitorInfo (OmsMonitorData& info) const;
  inline unsigned int          GetObjectRevisionNumber (const OMS_ObjectId8& oid);
  inline bool                  GetReadOnly() { return m_read_only; }
  inline OMS_ISessionLockObjects* GetSessionLockObject() const;
  inline long                  GetStoredErrorCode() const;           // PTS 1122839
  inline OMS_StreamRegistry&   GetStreamRegistry();
  inline tsp00_TaskId          GetTaskId() const; // PTS 1110315
  inline unsigned char*        GetVarObjMemory(size_t sz);
  inline size_t                GetVarObjSize(const OMS_ObjectId8& oid);
  bool                         HistoryInUse(const OMS_ObjectId8&, bool ignoreVersions, OmsTypeUInt8& OldestTransNo); // PTS 1130354
  inline bool                  InVersion();
  inline void                  IncRefCnt();
  inline void                  IncSubtransLevel ();
  inline void                  IncCacheHit(const OMS_ObjectId8& oid); // PTS 1117571
  inline void                  IncCacheMiss();                      // PTS 1117571
  inline void                  IncDeref();
  inline void                  IncDerefIter_LC(); // PTS 1107819 TA 13/09/2000  
  inline void                  IncDereflC();
  inline void                  IncDerefKey();
  inline void                  IncStore(); 
  inline void                  IncStore_lC();
  inline void                  IncOmsTerminate();
  inline void                  IncDelete (); 
  inline void                  IncDelete_lC(); 
  inline void                  IncDelete_lC(tsp00_Int4 cntDeleted); 
  inline void                  IncLock();
  inline void                  IncLogHop(int cnt); 
  inline void                  IncLogHopIter(int cnt); // PTS 1107819 TA 13/09/2000  
  inline void                  IncExceptions();
  inline void                  IncOutOfDate();
  inline void                  IncOutOfMemory();
  inline void                  IncTimeout(); 
  inline void                  IncSubtransCommit(); 
  inline void                  IncSubtransRollback(); 
  inline void                  IncLoadVarObj(); 
  inline void                  IncLoadVarObjLC(); 
  inline void                  IncStoreVarObj();
  inline void                  IncStoreVarObjLC();
  inline void                  IncReadStreamBuffer();
  inline void                  IncRehash();               // PTS 1118855
  inline void                  IncWriteStreamBuffer();
  inline void                  IncReadStreamRow();
  inline void                  IncWriteStreamRow();
  inline void                  InsertTransVersion(OMS_Context* context, bool create);
  inline bool                  IsCancelledByProgram() const;
  inline bool                  IsProvokedByTestkernel() const;
  inline bool                  IsTestKnlExceptionActive() const;
  inline bool                  IsTestKnlBadAllocActive() const;
  inline bool                  IsLocked (const OmsObjectContainer *pObj, bool shared);
  inline bool                  IsLocked (const OMS_ObjectId8& oid, bool shared);
  bool                         IsLockedInKernel (const OMS_ObjectId8& oid, bool shared);
  inline OmsObjectContainer*   LoadVarObject (const OMS_ObjectId8& oid, OMS_VarObjLockType lockReq, size_t size, void* buf);
  inline void                  MaxHashChainLen (int len); // PTS 1118855
  inline void                  MonitorSubtransLevel(); 
  inline void                  SetCacheSize(tsp00_Uint4 sz); 
  inline void                  IncCreateVersion (); 
  inline void                  IncOpenVersion (); 
  inline void                  IncCloseVersion (); 
  inline void                  IncDropVersion (); 
  inline void                  IncDeleteVarObject (); 
  inline void                  IncDeleteVarObjectLC (); 
  inline void                  IncCntWaitOmsLockObj();
  inline void                  IncWaitOmsLockObjSecs(tsp00_Int4);
  inline bool                  InsertBeforeImage (OmsObjectContainer* p);
  inline void                  InsertNewBeforeImage (OmsObjectContainer* p, OMS_ContainerEntry* pContainerInfo);
  inline bool                  IsDefaultContext(OMS_Context* context);
  inline bool                  IsSubtransOpen();
  inline void                  LockObj(OmsObjectContainer* p);
  inline void                  LockSharedObj(OmsObjectContainerPtr p);
  inline bool                  TryLockObj(OmsObjectContainer* p, bool shared);
  void                         LockObjMass(OmsIOidReadIterator& oids, short& timeout, 
                                           OmsIOidAppender& errOids,
                                           bool shared, bool newConsistentView,
                                           cgg250AvlTree<OMS_ObjectId8,OMS_ObjectId8,OMS_Session> *pUnlockedObjects = NULL);  // PTS 1121449
  inline void*                 allocate(size_t sz);
  inline void                  MonitorAlloc(SAPDB_ULong chunkSize, bool isUsrRequest);
  inline void                  MonitorDealloc(SAPDB_ULong chunkSize, bool isUsrRequest);
  inline void                  MonitorVarObjSize(tsp00_Int4);
  inline void                  MonitorWaitNewConsistentView(tsp00_Int4);
  void                         NewConsistentView(OmsIOidReadIterator& exclOids, OmsIOidReadIterator& sharedOids, 
                                                 short timeout, OmsIOidAppender& exclErrOids, OmsIOidAppender& sharedErrOids);   // PTS 1115027
  OmsAbstractObject*           NewKeyedObject(ClassID guid, const unsigned char* key, OmsSchemaHandle Schema, OmsContainerNo ContainerNo);
  OmsAbstractObject*           NewObject(ClassID guid, OmsSchemaHandle Schema, OmsContainerNo ContainerNo, int arrayByteSize);
  OMS_ObjectId8                NewVarObject(OmsSchemaHandle Schema, OmsContainerNo ContainerNo, ClassID guid);
  inline void                  OpenVersion(OMS_Context* context, bool create);
  inline void                  OpenVersionProlog(const OmsVersionId &versionId, bool create);  // PTS 1129082
  inline void                  OpenVersionEpilog();  // PTS 1129082
  void                         RegClass(ClassID            guid,
                                        const char        *pClassName,
                                        size_t             objectSize,
                                        const OMS_KeyDesc &keyDesc,
                                        ClassID            baseGuid,
                                        OmsAbstractObject *pObj,
                                        bool               isVarObject,
                                        bool               isArrayObject);
  void                         RegisterCallbackInterface(ClassID guid, OmsCallbackInterface* pInterface);
  inline void                  ResetSubtransLevel ();
  inline void                  ReleaseAllUnchanged();    // PTS 1128262
  void                         ReleaseCallbackInterface();
  inline void                  ReleaseObj(const OMS_ObjectId8& oid);
  inline void                  ReleaseObj(OmsObjectContainer* p, bool unchangedOnly = false);
  inline void                  ReleaseObjUnconditional(const OMS_ObjectId8& oid);
  void                         ReleaseVarObject(const OMS_ObjectId8& oid);
  inline void                  Sleep(const OmsTypeUInt1 sleepTime); /* PTS 1132163 */  
  void                         RemoveFromTransVersion(OMS_Context* context);
  inline void                  RollbackSubtrans(int requiredLevel, bool callFromRollbackTrans);
  inline void                  SetCancelledByProgram(bool);
  inline void                  SetProvokedByTestkernel(bool);
  inline void                  SetTestKnlExceptionActive(bool);
  inline void                  SetContext(OMS_Context* context);
  void                         SetCurrentMonitorMethod(tgg01_COMMonitorInfo* p); // PTS 1107731
#if defined(OMSTST)
  inline void                  SetCancelCountDown(int countDown);
  inline void                  SetExceptionCountDown(int countDown);
#endif
  void                         SetReadOnly(bool readOnly);
  inline void                  SetSessionLockObject(OMS_ISessionLockObjects* pLockObj);
  void                         SetTimeout(short timeout);
  inline void                  Signal(tsp00_TaskId TaskId, tsp00_Bool ok); // PTS 1110315
  inline int                   StartSubtrans();
  inline void                  StoreErrorCode(long errorCode);   // PTS 1122839
  inline void                  StoreVarObject (const OMS_ObjectId8& oid, const void* pVarObj, unsigned long objLength);
  void                         ThrowDBError(tsp00_Int2 e, const char* msg, const char* pFile, unsigned int line);
  void                         ThrowDBError(tsp00_Int2 e, const char* msg, const OMS_ObjectId8& oid, const char* pFile, unsigned int line);
  void                         ThrowDBError(tsp00_Int2 e, const char* msg, const OmsVersionId& v, const char* pFile, unsigned int line);
  void                         Trace( const char*fmt, ... );
  void                         TransEnd();
  void                         TransEndEpilog(bool isCommit, int &phase);
  inline void                  UnforceReadOnly(bool);
  void                         UnlockObj (const OMS_ObjectId8& oid, bool shared);
  bool                         VersionBoundByMe(OMS_Context* context);
  void                         Wait();

  inline OmsAllocatorInterface& GetSessionUserAllocator();
  inline OmsAllocatorInterface& GetTransactionUserAllocator();
  inline OmsAllocatorInterface& GetCOMRoutineUserAllocator();
  inline void                   UserAllocatorCtl(int msg, void  *param);

  inline bool                  IsDataChanged() const { return m_isDataChanged;  }
  inline void                  SetDataChanged()      { m_isDataChanged = true;  }
  inline void                  ResetDataChanged()    { m_isDataChanged = false; }

  inline void                  SetCancelFlag(bool b);
  inline bool                  IsCancelled() const;

  inline bool                  IsReadUnchangedObjectsActive() const { return m_readReadUnchangedObjectsActive; }
  inline void                  ReadUnchangedObjectsStart();
  inline void                  ReadUnchangedObjectsEnd();

  inline void                  AfterKernelTransEnd(bool value) { m_afterKernelTransEnd = value; }
  inline bool                  IsAfterKernelTransEnd() const    { return m_afterKernelTransEnd;   }

  inline bool                  CheckCorruptFlag() { return m_corrupt; }
  inline void                  SetCorruptFlag()   { m_corrupt = true; }

  inline OMS_RWLockDirectory&   GetRWLockDir();
  /*! R/W lock area for internal liboms. */
  enum {LIBOMS_RWLOCK_AREA = 0}; 

  void setCurrentMethod(unsigned char * name);                    /* PTS 1135058, PG */
  void resetCurrentMethod();                                      /* PTS 1135058, PG */
  unsigned char * getCurrentMethod() { return &m_currMethod[0]; } /* PTS 1135058, PG */
public:
  // GUID of the callback interface
  ClassID					          m_callbackInterfaceGuid;
  /*! Pointer to a callback interface which can be used by application programmer to react 
  ** on certain events like e.g. on commit, on rollback,... */
  OmsCallbackInterface*     m_callbackInterface;
  /*! Pointer to the sink. The sink is needed for the communication with the kernel. */
  IliveCacheSink*           m_lcSink;
  OmsTypeInt64              m_sizeVarobj; 
  OmsTypeInt64              m_cntVarobj;
  /*! Duration of the lock timeout of the kernel */
  tsp00_Int4                m_timeout;
  /*! Number of COM-objects which are connected to this session */
  int                       m_refCnt;
  /*! Actual level of subtransactions. This value is incremented (resp. decremented) 
  ** every time a new subtransaction is opened (resp. closed). If there is no subtransaction 
  ** open, which means that we are working in the basic transaction, then this variable 
  ** has the value 1. */
  int                       m_subtrans_lvl;
  /*! Minimal level of subtransaction. This value is either 1 for normal liveCache or
  ** 0 for simulator, in order to force before images in main transaction. This is
  ** needed to check for updates without stores */
  int                       m_min_subtrans_lvl;
  bool                      m_stream_io;
  /*! If this variable equals true, then it is not possible to execute any changing operation 
  ** (e.g. update, delete) on persistent objects. If such operations are started, then 
  ** the exception e_oms_read_only is thrown. (This member is set to true e.g. during 
  ** certain phases of the commit and rollback handling.) */
  bool                      m_read_only;
  /*! This flag equals true if the transaction has already been finished (commit or rollback)
  **  in the kernel (omsTransEndEpilog) */
  bool                      m_afterKernelTransEnd;
  /*! This flag is set, if an error occurs during the after-commit handling (omsTransEndEpilog)
  **  This is needed, as in this situation it is not possible to propagate the error to the
  **  application. So the application would not recognize, that there was an error on this session.
  **  As this could lead to inconsistencies, the session is flagged and each time before calling
  **  a com-routine it is checked, whether the session is flagged as corrupt (LVC_Dispatcher::dispDispatch)
  */
  bool m_corrupt;
  /*! Array to control the state of the application programmer controlled critical sections 
  ** (OmsCriticalSections). For each possible critical section there is an entry in the 
  ** array which either equals true if section is in use or false otherwise. The size 
  ** of this array (and thereby the number of critical sections) is determined via the 
  ** kernel interface when the session is created.   */
  bool*                     m_critical_section_in_use;
  /*! Similar to the critical sections this structure can be used by the application 
  ** programmers for synchronization. In contrast to critical section it is not only 
  ** possible to hold exclusive locks, but also to acquire shared locks which means, that 
  ** several task can hold the same lock. Furthermore the locks are released at the end of 
  ** the transaction. */
  OMS_ISessionLockObjects* m_lockObjects;
  /*! Pointer to the actual context which might be the default context or a version context. 
  ** If no version is open, than this pointer equals OMS_Session::m_defaultContext */
  OMS_Context*          m_context;
  /*! Pointer to the default context  */
  OMS_Context*          m_defaultContext;
  /*! Dictionary of the before images. For each open subtransaction level there exists a chain 
  ** of before images which can be used to rollback the corresponding subtransaction level. */
  OMS_BeforeImageList   m_beforeImages;  
  OMS_VarObjChunk       m_currVarObjChunk;
  /*! List of pointers to versions which are bound to this session. Although it is not 
  ** possible to have more than one version open in a session, there might be several 
  ** versions bound to a session. A version keeps bound to a session until the version 
  ** is closed and the transaction is committed or until the version is dropped. */
  cgg251DCUList<OMS_Context*,OMS_Session> m_versionsBoundToTrans;
  /*! List of versions created in this transaction. These versions may not be opened,
  ** if any changes have been made to IS-data. */
  cgg251DCUList<OMS_Context*,OMS_Session> m_createdVersionsInTrans;
  /*! List of pointers to omsHandles (COM-objects) which are connected with this session. 
  ** This list is used to call methods of the class OmsHandle. The number of entries in 
  ** this list should be equal to OMS_Session::m_refCnt. 
  ** \since PTS 1116693 */
  cgg251DCList<OmsHandle*,OMS_Session>        m_handleList;

private :
  /*! User allocator instances */
  OMS_UserAllocator           m_userAllocator;
  OMS_UserAllocator           m_comRoutineUserAllocator;
  OMS_CheckedUserAllocator    m_sessionUserAllocatorWrapper;
  OMS_CheckedUserAllocator    m_transactionUserAllocatorWrapper;
  OMS_CheckedUserAllocator    m_comRoutineUserAllocatorWrapper;
  /*! A directory with local information about the classes */
  OMS_ClassDirectory       m_classDir;
  /*! A directory with the local OMS_RWLocks */
  OMS_RWLockDirectory       m_rwLockDir;           
  /*! This member determines whether the private member OMS_Session::m_read_only can  
  ** be set to true or not.  */
  bool                     m_allowReadOnly;
  bool                     m_inMethodCallEpilog;
  /*! Set to true, if any IS-data have been changed in basis (e.g., omsRelease). */
  bool                     m_isDataChanged;
  /*! true, if client supports optimized stream protocol; otherwise false */
  bool                     m_optimizedStreamCommunication;
  /*! Aggregated monitoring information of several calls. */ 
  OMS_Monitor              m_monitor;
  /*! Monitoring information of the last call. When a new procedure call is executed, 
  ** this information is reset. */
  tgg01_COMMonitorInfo*    m_monitor_curr;
  /*! registry of all active streams and abap tables */
  OMS_StreamRegistry       m_streamRegistry;

  /*! This member is set to RESCHEDULE_DISTANCE (=1000) and when a DeRef is executed 
  ** the value is decreased by one. If the value equals 0 then the 
  ** OMS_Session::m_toCancel flag is checked and a rescheduling of the process 
  ** is forced. This prevents that COM-routines are running in a loop without releasing 
  ** the CPU and thereby without the change to stop the processing. */
  int                      m_rescheduleDistance;
  /*! When a new procedure call is executed, the current heap consumption is determined 
  ** and stored in this member. This information is needed later when the heap 
  ** consumption of this procedure itself must be determined. */
  tsp00_8ByteCounter       m_heapAtMethodBegin;
  /*! This member is a pointer to the cancel flag in the kernel. As this flag is checked 
  ** regularily (see OMS_Session::m_scheduleDistance) long running transactions can 
  ** be terminated by setting this flag in the kernel. If a routine is canceled because 
  ** of this flag, then the exception e_OMS_cancelled is thrown. 
  ** \since PTS 1107849 */
  pasbool&                 m_toCancel;
  /*! Error code which can be set and recalled by the application 
  ** \see OMS_Handle::omsGetStoredErrorCode 
  ** \see OMS_Handle::omsStoreErrorCode
  ** \since PTS 1122839   */
  long                     m_errorCode;               
  /*! This id is used to identify the task which has aquired a lock resp. which is waiting 
  ** for a lock (see OMS_Session::m_lockObjects)  
  ** \since PTS 1110315 */
  tsp00_TaskId             m_taskId;  
  /// @name Only for test purposes
  //@{
  /*! This member controls at what time an exception should be thrown. 
  ** \since PTS 1112358 */ 
  int                      m_exceptionCountDown;      
  /*! This member controls at what time the cancel flag should be set. 
  ** \since PTS 1112358 */ 
  int                      m_cancelCountDown;  

  /*! This member is set to true, if the cancel-flag was set by a method call (in the 
  **  current session) in contrast bo beeing set by a different session (e.g. by a dbmcli
  **  call. If a test-knl is running this variable is also set to true if the test-knl 
  **  sets the cancel flag to check the error-handling. */
  bool                     m_cancelledByProgram;
  /*! This member is set to true if the countdown of OMS_Session::m_exceptionCountDown 
  **  or OMS_Session::m_cancelCountDown has reached zero and so an exeption was 
  **  thrown or the cancel flag has been set. The flag is reset as soon as the corresponding
  **  exception has been thrown. In this exception the flag 'provokedByTestKnl' is set, so
  **  that the application can filter out this provoked exceptions. Exceptions which are thrown
  **  later in the same transaction (before the omsTerminate is executed) are not marked, 
  **  as these exceptions must not be ignored.
  **  \since PTS 1112358 */
  bool                     m_requiredExceptionThrown; 
  /*! This member is set to true if the countdown of OMS_Session::m_exceptionCountDown 
  **  or OMS_Session::m_cancelCountDown has reached zero and so an exeption was 
  **  thrown or the cancel flag has been set. In contrast to m_requiredExceptionThrown, this
  **  flag is not reset after the corresponding exception has been thrown. This is needed as
  **  the omsTerminate-method must know whether it is called because of a provoked exception
  **  or because of a regular error. In the first case the omsTerminate should not lead to
  **  an error in the application (ABAP-shortdump). */
  bool                     m_testKnlExceptionActive;
    /*! As the read-only-flag (m_read_only) might have been changed by omsReadUnchangedObjectsStart
  **  the old value is remembered in this varialbe, so that omsReadUnchangedObjectsEnd 
  **  can restore the previous value.
  ** \since PTS 1137538
  */
  bool                      m_read_only_old;
  /*! This flag is set if objects should be read in the state they were at the beginning. 
  **  This means, that local changes in the transaction can not be seen. If this mode is
  **  active, then it is not allowed to update the objects and therefore m_read_only is
  **  set to true and it is forbitten to change the variable m_read_only via a user-call.
  ** \since PTS 1137538
  */
  bool                      m_readReadUnchangedObjectsActive;
//@}

  /*! This member holds the name of the currently running routine
  **  in ASCII7 representation with null-terminator.
  ** \since PTS 1135058 */ 
  unsigned char            m_currMethod[sizeof(tsp00_KnlIdentifier)/2+1];  

  void  ReleaseLocks (OmsIOidReadIterator &oids, int cnt, bool shared);  // PTS 1121449
};

#endif  // __OMS_SESSIONDEF_HPP
