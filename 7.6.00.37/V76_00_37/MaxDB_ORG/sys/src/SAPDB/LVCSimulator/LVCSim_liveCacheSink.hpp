/****************************************************************************

  module    : LVCSim_liveCacheSink.hpp

  -------------------------------------------------------------------------

  responsible : ChristophR

  special area: liveCache Simulator
  description : liveCache Sink implementation

  -------------------------------------------------------------------------

  copyright:  Copyright (c) 2002-2005 SAP AG




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




*****************************************************************************/


#ifndef LVCSIM_LIVECACHESINK_HPP
#define LVCSIM_LIVECACHESINK_HPP

/*===========================================================================*
 *  INCLUDES                                 *
 *===========================================================================*/

#include "LVCSimulator/LVCSim_Internal.hpp"
#include "LVCSimulator/LVCSim_FastHash.hpp"
#include "Oms/OMS_RawAllocator.hpp"
#include "LVCSimulator/LVCSim_CheckpointIO.hpp"

#include "liveCache/LVC_IliveCacheSink.hpp"
#include "LVCSimulator/LVCSim_ProcServerInterface.hpp"
#include "geo07.h"


/// Maximum count of sink pointers (and thus auto-assigned OMS handles).
#define LVCSIM_MAX_SINK_PTR   1024


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                   *
 *===========================================================================*/


class LVCSim_TXList;
//class LVCSim_CheckedAllocator;
//class LVCSim_CheckedAllocatorInst;
//class LVCSim_GlobalAllocator;
class LVCSim_ContainerID;
class LVCSim_ObjectLockChainItem;

class LVC_Dispatcher;
class LVC_ProcServerExecutor;
class LVC_ProcServerInterface;


/*!
 * \brief LiveCache session-specific kernel sink.
 *
 * Definition of those functions that can be called from the liboms to obtain
 * kernel support. In this case, kernel means simulator.
 *
 * An instance of this class practically represents a transaction.
 */
class LVCSim_liveCacheSink : public IliveCacheSink
{
public:
  class SchemaName;
  class ConsViewList;
  class UndoList;
  class ActiveVersion;
  class SubTxList;

private:
  /// Count of defined schemas.
  static int                  schemaCount;
  /// Schema directory.
  static LVCSim_FastHash<SchemaName,
    int, LVCSim_SchemaMeta, false>     *schemaDir;
  /// Spinlock protecting schema directory.
  static RTESync_Spinlock           *schemaLock;

  /// Count of registered containers.
  static int                  containerCount;
  /// Container directory.
    // PTS 1124356 FF 2003-09-23 new FileDir
  static LVCSim_FastHash<LVCSim_ContainerID,
    OMS_ContainerHandle, LVCSim_ContainerMeta, false> *containerDir;
  /// Spinlock protecting container directory.
  static RTESync_Spinlock           *containerLock;

  /// Spinlocks for emulating regions.
  static RTESync_Spinlock           *regionLocks;

  /// Spinlock protecting temporary sink pointers.
  static RTESync_Spinlock           sinkPtrLock;
  /// Temporary sink pointers used in LVCSim::Attach().
  static LVCSim_liveCacheSink*        sinkPtr[LVCSIM_MAX_SINK_PTR];
  /// Temporary sink cancel flags used in LVCSim::Attach().
  static bool                 sinkPtrC[LVCSIM_MAX_SINK_PTR];
  /// Free pointer for temporary sinks used in LVCSim::Attach().
  static int                  sinkPtrFree;

  /// Simulator state dump sequence counter.
  static int                  dumpSequence;

  /// Dummy default context.
  void                    *m_defaultContext;
  /// Dummy cancel byte.
  pasbool                   m_cancelByte;
  /// Sink reference count.
  int                     m_cRef;

  /// Current read view.
  LVCSim_TID                  m_readView;
  /// Unclosed read view.
  LVCSim_TID                  m_oldReadView;
  /// Current undo list (for active subtransaction).
  UndoList                                    *m_undoList;
  /// Is a object update performed (create/delete/update)
  bool                                     m_objectsUpdated;
  /// Current subtransaction list.
  SubTxList                 *m_subTxList;
  /// Current lock chain.
    class LVCSim_LockChainItem                   *m_lockChain;
  /// Return current undo list, creating it, if necessary.
    UndoList *undoList();
  /// Unlock all objects in lock chain (after COMMIT/ROLLBACK).
    void unlockAll();
    
  /// Create underlying raw allocator for session-specific allocators.
    //void createRawAllocator();

    tsp00_TaskId         m_taskId;
    static tsp00_TaskId  m_taskIdCnt;

protected:
  /// Lock for transaction-specific variables.
  static RTESync_Spinlock           txCounterLock; 

  /// Current TX counter.
  static LVCSim_TID             txCounter;
  /// List of existing consistent views.
  static ConsViewList             *consViewList;
  /// Oldest running TX.
  static LVCSim_TID             txOldest;
  /// First session in linked list of all sessions.
    static LVCSim_liveCacheSink                 *firstSession; 

  /// Next session in sessions chain.
    LVCSim_liveCacheSink                        *m_next;

  friend class LVCSim_Initer;
  friend class LVCSim_TXList;
  friend class LVCSim;
  friend class LVCSim_ObjectHandle;
  friend class ConsViewList;
  friend class LVCSim_ContainerMeta;
  friend class LVCSim_SchemaMeta;
  friend class LVCSim_SchemaLockChainItem;
  friend class LVCSim_ContainerLockChainItem;


  /// Initialize the static variables.
  void init();

  /// Reset the sink (e.g., on simulator reset).
  void reset();

  /// Shut down the sink.
  void done();

  /// Internal commit processing.
  void SimCommit();
  /// Internal rollback processing.
  void SimRollback();

  /// Get new temporary sink pointer for pure OmsHandle.
  static LVCSim_liveCacheSink **getSinkPtr(bool **cancelFlag);
  /// Return temporary sink pointer for pure OmsHandle.
  static void returnSinkPtr(LVCSim_liveCacheSink **ptr);

  /// Write contents of the simulator persistent state (checkpoint).
  static void writeToStream(LVCSim_CheckpointWriter &o);
  /// Read contents of the simulator persistent state.
  static void readFromStream(LVCSim_CheckpointReader &i);

  /// Dump internal simulator state for given handle. TODO: params.
  void omsSimDump(OmsHandle &handle, int param);
  /// Dump single container into dump file. TODO: params.
  static long omsSimDumpContainer(OmsHandle &handle, FILE *out, 
    LVCSim_liveCacheSink *tid, const char *datetime, 
    OMS_LibOmsInterfaceClassInfo &cinfo);
  /// Count all non-var and var objects in all containers and the size of the objects.
  static void countObjects(OmsHandle &handle, const OmsSchemaHandle schema, int &objCount, size_t &objSize, int &varObjCount, size_t &varObjSize);
  /// Set file to use as VTRACE.
  static void SetVTraceFile(FILE *f);

  /// Set dump count.
  static void SetDumpCount(int param);

  /// Start a routine in simulated ProcServer.
  void StartInUDEServer(const char *libname, const char *methodname, va_list &args);

  /// Start a routine in simulated ProcServer.
  void StartInUDEServer(const char *libname, const char *methodname, int dummy, ...);

  /// COMMIT in simulated ProcServer.
  void CommitInProcServer(const char *libname);

  /// ROLLBACK in simulated ProcServer.
  void RollbackInProcServer(const char *libname);

  /// Create new sink.
  LVCSim_liveCacheSink(bool appendToList = true);
  /// Destroy the sink.
  ~LVCSim_liveCacheSink();

  /// Commit (write) transaction timestamp.
  LVCSim_TID      m_commitTime;
  /// Reference into list of consistent views.
    ConsViewList    *m_commitTimeRef;

  /// Get or create read consistent view.
    void getReadConsistentView(void *lpView, short *lpb_error);
  /// Get or create write consistent view.
    void getWriteConsistentView(void *lpView, short *lpb_error);
  /// Return true, if transaction sees changes made by otherTX.
    bool sees(LVCSim_TID otherTX);
  /// Return true, if current transaction holds a read view.
    inline bool hasReadView() const { return m_otherTXs != NULL; }
  /// Return commit transaction timestamp.
    LVCSim_TID getCommitTime();
  /// Check if we see timestamp tx as committed.
    static bool checkCommited(LVCSim_TID tx);

  /// Get container for given container id
  LVCSim_ContainerMeta *getContainer(tgg00_FileId& desc, short *lpb_error);

  /// Get container for given oid
  LVCSim_ContainerMeta *getContainer(tgg00_FileId& desc, const OMS_ObjectId8 *lpoid, short *lpb_error);

  /// Timestamps of parallel transactions that ran as we started.
    LVCSim_TXList           *m_otherTXs;
  /// Unclosed timestamps of parallel transactions that ran as we started.
    LVCSim_TXList           *m_oldOtherTXs;
  /// Currently active persistent version or NULL.
    ActiveVersion           *m_activeVersion;   

  /// Communication buffer for simulated ProcServer.
  LVCSim_ProcServerBuffer   *m_procServerBuffer;
  /// Local end for ProcServer communication interface for simulated ProcServer.
  LVC_ProcServerInterface   *m_procServerLocal;
  /// Local end for ProcServer communication interface for simulated ProcServer.
  LVC_ProcServerInterface   *m_procServerRemote;
  /// Thread of the other end in ProcServer simulator.
  LVCSim_Thread             *m_procServerThread;


public:

  STDMETHODIMP QueryInterface(const IID& iid, LPVOID* ppv) 
  {  
    if (iid == IID_IUnknown)
      *ppv = STATIC_CAST(IUnknown*, this); 
    else {
      *ppv = NULL ;
      return E_NOINTERFACE ;
    }
    (REINTERPRET_CAST(IUnknown*,*ppv))->AddRef() ;
    return S_OK ;
  }

  STDMETHODIMP_(ULONG) AddRef() 
  {
    return ++m_cRef;
  }

  STDMETHODIMP_(ULONG) GetRefCount() 
  {
    return m_cRef;
  }
  
  STDMETHODIMP_(ULONG) Release()
  {
    if (--m_cRef == 0) {
      delete this;
      return 0;
    }
    return m_cRef;
  }

  /// Get container metadata for given container ID.
    // PTS 1124356 FF 2003-09-23 new FileDir
  static class LVCSim_ContainerMeta *getContainer(OMS_ContainerHandle id);

  /// Get schema metadata for given schema ID.
  static class LVCSim_SchemaMeta *getSchema( int id);

  /// Get oldest running transaction timestamp.
  static inline LVCSim_TID getOldestTID() { return txOldest; }


  /*----------------------------------------------------------------**
  ** Documentation for the sink-methods is given in IliveCacheSink  **
  **----------------------------------------------------------------*/

  virtual HRESULT STDMETHODCALLTYPE ABAPRead( 
        tgg01_StreamDescriptor &descriptor,
    short                  &error);
  
  virtual HRESULT STDMETHODCALLTYPE ABAPWrite(
        tgg01_StreamDescriptor &descriptor,
    short                  &error);
    
  virtual HRESULT STDMETHODCALLTYPE AllocPacket( 
    unsigned char  **p,
    tsp00_Int4      *sz);

  virtual HRESULT STDMETHODCALLTYPE CloseVersion( 
    short        *lpb_error);
    
  virtual HRESULT STDMETHODCALLTYPE ConsoleMsg( 
    tsp00_Int4      buflen,
    unsigned char  *lpv);

  virtual HRESULT STDMETHODCALLTYPE CreateVersion( 
    unsigned char  *lpConsistentView,
    unsigned char  *lpVersionContext,
    short          *lpb_error);

   virtual HRESULT STDMETHODCALLTYPE CreateKeyRangeIterator(
    tgg91_TransNo            &consistentView,
    tgg00_FileId             &objFile,
    tgg01_OmsVersionContext  *lpVersionContext,
    void                    **pKeyRangeIterator,
    tsp00_Int4                objBodySize,
    size_t                    keyLen,
    const void               *pStartKey,              
    const void               *pLowerKey,             
    const void               *pUpperKey,              
    tsp00_Int4               &noOfOid,
    SAPDB_UInt8              *pResultOId,  
    tgg91_PageRef            *pObjSeq,
    unsigned char            *pBody,
    tsp00_Int4               &LogReadAccesses,
    tgg00_BasisError         &e,
    OmsIterDirection          direction = OMS_ASCENDING);

  virtual HRESULT STDMETHODCALLTYPE StartUnloadedVersionIter(
    tgg91_TransNo           &consistentView,
    tgg01_OmsVersionContext *lpVersionContext,
    tgg00_FileId            &objFile,
    size_t                   keyLen,
    void                    *pStartKey,            
    const void              *pLowerKey,
    const void              *pUpperKey,
    size_t                   ObjBodySize,           
    void                    *pObjBody,             
    SAPDB_UInt8             &ResultOId,  
    short                   &e,
    OmsIterDirection        direction);             
  
  virtual HRESULT STDMETHODCALLTYPE DeleteAllObj(
    tgg91_TransNo&  consistentView,
    unsigned char  *lpContainerId,
    unsigned char  *lpVersionFile,
    int            &lpDeleted,
    short          &lpb_error,
    SAPDB_UInt8    &errorOid);

  virtual HRESULT STDMETHODCALLTYPE DeleteObj( 
    unsigned char  *lpConsistentView,
    unsigned char  *lpContainerId,
    unsigned char  *lpVersionContext,
    unsigned char  *lpoid,
    unsigned char  *lpobjseq,
    bool            reused,       // PTS 1127661
    bool            contObj,      // PTS 1124278
    short          *lpb_error);

  virtual HRESULT STDMETHODCALLTYPE DeleteObjMass(
    tsp00_Int4               cnt,
    tgg91_TransNo           *pConsistentView,
    tgg01_ContainerId      **ppContainerId,
    unsigned char           *pVersionContext,
    SAPDB_UInt8             *pOid,
    tgg91_PageRef           *pObjSeq,
    bool                    *pReused,  // PTS 1127661 
    bool                    *pContObj, // PTS 1124278
    bool                     clustered,
    tsp00_Int4              &errCnt,
    tgg00_BasisError        *pDBError);

  virtual HRESULT STDMETHODCALLTYPE DestroyKeyRangeIterator(
    void* pKeyRangeIterator);
  
  virtual HRESULT STDMETHODCALLTYPE EndConsistentView( 
    unsigned char  *lpConsistentView,
    short          *lpb_error);

  virtual HRESULT STDMETHODCALLTYPE EnterCriticalSection(
    short regionId);
 
  virtual HRESULT STDMETHODCALLTYPE GetContainerId(   
    tgg01_ContainerId              *pcontainerId, 
    const tgg01_OmsVersionContext  *pVersionContext,
    tsp00_Int4                      noOfOid,
    const SAPDB_UInt8              *pOid,
    tgg00_BasisError               *pDBError);

  virtual HRESULT STDMETHODCALLTYPE GetObj( 
    tgg91_TransNo                 &consistentView,
    tgg01_ContainerId             &containerId, 
    const tgg01_OmsVersionContext *pVersionContext,
    const SAPDB_UInt8             &oid,
    bool                           doLock,
    bool                           shared,
    tgg91_PageRef                 &objSeq,
    tgg91_TransNo                 &updTransId,
    size_t                         bodySize,
    unsigned char                 *pBody,
    size_t                        &bodyLen,
    tsp00_Int4                    &histLogCount,
    tgg00_BasisError              &e); 

  virtual HRESULT STDMETHODCALLTYPE GetObjects(
    tgg91_TransNo                 &consistentView,
    tgg01_ContainerId             *pContainerId, 
    const tgg01_OmsVersionContext *pVersionContext,
    tsp00_Int4                     noOfOid,
    const SAPDB_UInt8             *pOid,
    tgg91_PageRef                 *pObjVers,
    tgg91_TransNo                 *pUpdTransId,
    size_t                        *pBodySize,
    bool                           doLock,
    bool                           shared,
    unsigned char                **ppObj,
    tsp00_Int4                    &histLogCount,
    tsp00_Int4                    &errorCount,
    tgg00_BasisError              *e);

	virtual HRESULT STDMETHODCALLTYPE GetObjWithKey( 
		unsigned char  *lpConsistentView,
		unsigned char  *lptree,
		unsigned char  *lpVersionContext,
		size_t          keylen,
		unsigned char  *lpkey,
		bool            doLock,
    bool            shared,
		size_t          bodysize,
		unsigned char  *lpbody,
		SAPDB_UInt8    *lpoid,
		unsigned char  *lpobjseq,
    tgg91_TransNo  *pUpdTransId,
		int            *hist_log_count,
		short          *lpb_error);

  virtual HRESULT STDMETHODCALLTYPE GetObjectsWithKey( 
    tgg91_TransNo            *pConsistentView,
    tgg01_ContainerId        *pContainerId,
    tgg01_OmsVersionContext  *pVersionContext,
    size_t                   *pKeyLen,
    tsp00_Int4                noOfObj,
    unsigned char           **ppKey,
    bool                      doLock,
    bool                      shared,
    size_t                   *pBodySize,
    unsigned char           **ppBody,
    SAPDB_UInt8              *pOid,
    tgg91_PageRef            *pObjSeq,
    tgg91_TransNo            *pUpdTransId,
    tsp00_Int4               &histLogCnt,
    tsp00_Int4               &errorCnt,
    tgg00_BasisError         *pDBError);

  virtual HRESULT STDMETHODCALLTYPE HResultError( 
    HRESULT h_error);

  virtual HRESULT STDMETHODCALLTYPE IsLocked( 
    unsigned char  *lpContainerId,
    SAPDB_UInt8    *lpoid,
    bool            shared,
    short          *lpb_error);
  
  virtual HRESULT STDMETHODCALLTYPE LeaveCriticalSection(
    short regionId);

  virtual HRESULT STDMETHODCALLTYPE LockObj( 
    unsigned char  *lpConsistentView,
    unsigned char  *lpContainerId,
    SAPDB_UInt8    *lpoid,
    unsigned char  *lpobjseq,
    short          *lpb_error);

  virtual HRESULT STDMETHODCALLTYPE LockObjMass(
    tsp00_Int4              &cnt,
    tgg91_TransNo           *pConsistentView,
    tgg01_ContainerId      **ppContainerId,
    SAPDB_UInt8             *pOid,
    tgg91_PageRef           *pObjSeq,
    tsp00_Int8              &timeout,
    tsp00_Int4              &errCnt,
    tgg00_BasisError        *pDBError);
 
  virtual HRESULT STDMETHODCALLTYPE MultiPurpose( 
    unsigned char   MessType,
    unsigned char   MessType2,
    tsp00_Int4     *lpbuflen,
    unsigned char  *lpv,
    short          *lpb_error);

  virtual HRESULT STDMETHODCALLTYPE NewConsistentView( 
        bool            isOmsVersion,             /* PTS 1130354 */
    unsigned char  *lpConsistentView,
    short          *lpb_error);

  virtual HRESULT STDMETHODCALLTYPE NewObj( 
    unsigned char  *lpConsistentView,
    unsigned char  *lpContainerId,
    unsigned char  *lpVersionContext,
    size_t          KeyPos,
    size_t          KeyColSize,
    unsigned char  *KeyColumn,
    bool            contObj,    // PTS 1124278
    SAPDB_UInt8    *lpoid,
    unsigned char  *lpobjseq,
    tgg91_TransNo  *pUpdTransId,
    short          *lpb_error);

  virtual HRESULT STDMETHODCALLTYPE NewObjMass( 
    tsp00_Int4               cnt,
    tgg91_TransNo           *pConsistentView,
    tgg01_ContainerId      **ppContainerId,
    unsigned char           *pVersionContext,
    size_t                  *pKeyPos,
    size_t                  *pKeyColSize,
    unsigned char          **ppKeyColumn, 
    bool                    *pContObj,    // PTS 1124278
    SAPDB_UInt8             *pOid,
    tgg91_PageRef           *pObjSeq,
    tgg91_TransNo           *pUpdTransId,
    tsp00_Int4              &errCnt,
    tgg00_BasisError        *pDBError);

  virtual HRESULT STDMETHODCALLTYPE NextOids(
    tgg91_TransNo           &ConsistentView,
    tgg00_FileId            &ContainerId,
    tgg01_OmsVersionContext *pVersionContext,
    bool                     bOnlyHdrVarObjects,
    SAPDB_UInt8              CurrOid,
    tsp00_Int4              &noOfOid,
    SAPDB_UInt8             *pOid,
    tgg91_PageRef           *pObjVers,
    tsp00_Int4              &LogReadAccesses,
    tgg00_BasisError        *pb_error);

  virtual HRESULT STDMETHODCALLTYPE NextObjFromKeyIterator (
    const tgg91_TransNo     &ConsistentView,  // PTS 1127520 
    tgg01_ContainerId       &ObjFile,
    tgg01_OmsVersionContext *VersionContext,
    void                    *pKeyIterator,
    tsp00_Int4               objBodySize,
    size_t                   KeyLen,
    void                    *pRestartKey, 
    tsp00_Int4              &NoOfOId,
    SAPDB_UInt8             *pResultOId, 
    tgg91_PageRef           *pObjVers,
    unsigned char           *pBody,
    tsp00_Int4              &objHistReadCount,
    tgg00_BasisError        &e);

  virtual HRESULT STDMETHODCALLTYPE VersionNextKey(
    tgg01_OmsVersionContext  *VersionContext,
    const tgg01_ContainerId  &ObjFile,
    void                     *pKey,
    size_t                    KeyLen,
    bool                      Inclusive,
    const void               *pStopKey,
    SAPDB_UInt8              &Oid,
    tgg00_BasisError         &e);

  virtual HRESULT STDMETHODCALLTYPE PrevObjFromKeyIterator ( 
    const tgg91_TransNo     &ConsistentView,  // PTS 1127520 
    tgg01_ContainerId       &ObjFile,
    tgg01_OmsVersionContext *VersionContext,
    void                    *pKeyIterator,
    tsp00_Int4               objBodySize,
    size_t                   KeyLen,
    void                    *pRestartKey, 
    tsp00_Int4              &NoOfOId,
    SAPDB_UInt8             *pResultOId, 
    tgg91_PageRef           *pObjVers,
    unsigned char           *pBody,
    tsp00_Int4              &objHistReadCount,
    tgg00_BasisError        &e);

  virtual HRESULT STDMETHODCALLTYPE VersionPrevKey(   
    tgg01_OmsVersionContext  *VersionContext,
    const tgg01_ContainerId  &ObjFile,
    void                     *pKey,
    size_t                    KeyLen,
    bool                      Inclusive,
    const void               *pStopKey,
    SAPDB_UInt8              &Oid,
    tgg00_BasisError         &e);

  virtual HRESULT STDMETHODCALLTYPE CreateContainer(
    tsp00_Int4           SchemaId,
    GUID                *lpGUID,
    tsp00_KnlIdentifier *lpClassName,
    size_t               size,
    OmsContainerNo       ContainerNo,
    size_t               keyPos,          // PTS 1122540
    size_t               keyLen,          // PTS 1122540
    bool                 keyPartitioned,  // PTS 1122540
    bool                 useCachedKeys,
    bool                 isVarObject,
    bool                 useInternalTrans,
    bool                 noWait,
    unsigned char       *lpContainerId,
    short               *lpb_error);

  virtual HRESULT STDMETHODCALLTYPE ResetVersion( 
    unsigned char *lpVersionContext,
    short         &error);
 
  virtual HRESULT STDMETHODCALLTYPE SetError( 
    short                 errorno,
    tsp00_Int4            buflen,
    const unsigned char  *lpv,
    tsp00_CodeType        codeType );
  
  virtual HRESULT STDMETHODCALLTYPE SQLRequest( 
    tsp00_Int4      buflen,
    unsigned char  *lpv);

  virtual HRESULT STDMETHODCALLTYPE SubtransCommit( 
    short  *lpb_error);

  virtual HRESULT STDMETHODCALLTYPE SubtransRollback( 
    short  *lpb_error);

  virtual HRESULT STDMETHODCALLTYPE SubtransStart( 
    short  *lpb_error);

  virtual HRESULT STDMETHODCALLTYPE UnlockObj( 
    unsigned char  *lpContainerId,
    SAPDB_UInt8    *lpoid,
    short          *lpb_error);
  
  virtual HRESULT STDMETHODCALLTYPE UnlockSharedObj( 
    unsigned char  *lpContainerId,
    SAPDB_UInt8    *lpoid,
    short          *lpb_error);
  
  virtual HRESULT STDMETHODCALLTYPE UpdateObj( 
    unsigned char  *lpConsistentView,
    unsigned char  *lpContainerId,
    unsigned char  *lpVersionContext,
    unsigned char  *lpoid,
    unsigned char  *lpobjseq,
    size_t          bodysize,
    unsigned char  *lpbody,
    size_t          keyPos,            // PTS 1122540
    size_t          keyLen,            // PTS 1122540
    bool            contObj,           // PTS 1124278
    bool            reused,            // PTS 1127661
    short          *lpb_error);

  virtual HRESULT STDMETHODCALLTYPE UpdateObjMass(
    tsp00_Int4               cnt,
    tgg91_TransNo           *pConsistentView,
    tgg01_ContainerId      **ppContainerId,
    unsigned char           *pVersionContext,
    SAPDB_UInt8             *pOid,
    tgg91_PageRef           *pObjSeq,
    size_t                  *pBodySize,
    unsigned char          **ppBody,
    size_t                  *pKeyPos,       // PTS 1122540
    size_t                  *pKeyLen,       // PTS 1122540
    bool                    *pReused,       // PTS 1127661
    bool                     clustered,
    tsp00_Int4              &errCnt,
    tgg00_BasisError        *pDBError);

  virtual HRESULT STDMETHODCALLTYPE UpdateVarObjMass(
    tsp00_Int4               cnt,
    tgg91_TransNo           *pConsistentView,
    tgg01_ContainerId      **ppContainerId,
    unsigned char           *pVersionContext,
    SAPDB_UInt8             *pOid,
    tgg91_PageRef           *pObjSeq,
    size_t                  *pObjSize,
    size_t                  *pBodySize,
    unsigned char          **ppBody,
    SAPDB_UInt8             *pNextOid,
    bool                    *pContObj,  // PTS 1124278
    bool                    *pReused,   // PTS 1127661
    tsp00_Int4              &errCnt,
    tgg00_BasisError        *pDBError);

  virtual HRESULT STDMETHODCALLTYPE Vtrace( 
    tsp00_Int4      buflen,
    unsigned char  *lpv);

  virtual HRESULT STDMETHODCALLTYPE GetDefaultContext(
    void         **p, 
    pasbool      **pCancelByte,
        bool          &optimizedStreamCommunication, /* PTS 1130281 */
    tsp00_TaskId  &taskId /* PTS 1110315 */);
  
  virtual HRESULT STDMETHODCALLTYPE SetDefaultContext(
    void* p);

  virtual HRESULT STDMETHODCALLTYPE GetSqlSessionContext(
    void** p,
    tsp00_TaskId  &taskId);
        
 virtual HRESULT STDMETHODCALLTYPE SetSqlSessionContext(
   void* p);

  virtual HRESULT STDMETHODCALLTYPE OpenFile(
    char        *lpFileName,
    tsp00_Uint4  FileSize,
    boolean      doAppend, 
    tsp00_Int4  *FileHandle,
    short       *lpb_error);
    
  virtual HRESULT STDMETHODCALLTYPE WriteFile(
    tsp00_Int4 FileHandle,
    char      *msg,
    short     *lpb_error); 

  virtual HRESULT STDMETHODCALLTYPE CloseFile(
    tsp00_Int4  FileHandle,
    short      *lpb_error);

  virtual HRESULT STDMETHODCALLTYPE SyncFile(
    char       *lpFileName,
    tsp00_Int4  FileHandle,
    short      *lpb_error);

  virtual HRESULT STDMETHODCALLTYPE GetDateTime(
    tsp00_Date* Date,
    tsp00_Time* Time);
  
  virtual HRESULT STDMETHODCALLTYPE GetClock(
    tsp00_Int4* Sec,
    tsp00_Int4* MSec);

  virtual HRESULT STDMETHODCALLTYPE DropVersion( 
    unsigned char  *lpVersionContext,
    short          *lpb_error);
  
  virtual HRESULT STDMETHODCALLTYPE CreateSchema(
    const OmsTypeWyde *lpSchemaName,
    tsp00_Int4        *lpSchemaId,
    short             *lpb_error);

  virtual HRESULT STDMETHODCALLTYPE ExistsSchema(
    const OmsTypeWyde *lpSchemaName,
    tsp00_Int4        *lpSchemaId,
    short             *lpb_error);

  virtual HRESULT STDMETHODCALLTYPE DropSchema(
    tsp00_Int4  SchemaId,
    short      *lpb_error);

  virtual HRESULT STDMETHODCALLTYPE GetSchemaName(
    tsp00_Int4    SchemaId,
    OmsTypeWyde  *lpSchemaName,
    short        *lpb_error);

  virtual HRESULT STDMETHODCALLTYPE ExistsFile(
    char    *lpFileName,
    bool    &existsFile,
    bool    &isDirectory,
    short   &error);
  
  virtual HRESULT STDMETHODCALLTYPE DropFile(
    char  *lpFileName,
    short &error); 

  virtual HRESULT STDMETHODCALLTYPE GetWorkingDirectory(
    tsp00_Pathc &pathName,
    short       &error); 

  virtual HRESULT STDMETHODCALLTYPE GetDbrootSapPath(
    tsp00_Pathc &pathName,
    short       &error);
    
  virtual HRESULT STDMETHODCALLTYPE LoadLibraryEx( 
    const char    *path, 
    bool           loadGlobal,
    HANDLE        *hd,
    tsp00_ErrText &errtext,
    int            errtext_size);

  virtual HRESULT STDMETHODCALLTYPE FreeLibrary( 
    HANDLE         hd,
    bool          &ok,
    tsp00_ErrText &errtext,
    int            errtext_size);

  virtual HRESULT STDMETHODCALLTYPE DropContainer(
    OmsSchemaHandle  schema,
    const GUID       &guid,
    OmsContainerNo    cno,
    tgg00_BasisError &error);

  virtual HRESULT STDMETHODCALLTYPE GetContainerInfo(
        OMS_ContainerHandle &containerHandle, // PTS 1124356 FF 2003-09-23 new FileDir
    GUID            &guid,
    OmsSchemaHandle &schema,
    OmsContainerNo  &cno,
    short           &error);

  virtual HRESULT STDMETHODCALLTYPE IsObjHistoryUsed( 
    const SAPDB_UInt8 &oid,
    bool              ignoreVersions,  /*PTS 1130354*/
    bool              &isUsed,
    tgg00_BasisError  &e,
    OmsTypeUInt8       &OldestTransNo); /*PTS 1130354*/

  virtual HRESULT STDMETHODCALLTYPE Reschedule(); /* PTS 1107849 */

  virtual HRESULT STDMETHODCALLTYPE Sleep(
    const OmsTypeUInt1 sleepTime); /* PTS 1132163 */
  
  virtual HRESULT STDMETHODCALLTYPE LockUpdObj( 
    unsigned char   *lpConsistentView,
    unsigned char   *lpContainerId,
    SAPDB_UInt8     *lpoid,
    unsigned char   *lpobjseq,
    short           *lpb_error);

  virtual HRESULT STDMETHODCALLTYPE LockUpdObjMass(
    tsp00_Int4               cnt,
    tgg91_TransNo           *pConsistentView,
    tgg01_ContainerId      **ppContainerId,
    SAPDB_UInt8             *pOid,
    tgg91_PageRef           *pObjSeq,
    bool                     clustered,
    tsp00_Int4              &errCnt,
    tgg00_BasisError        *pDBError);

  virtual HRESULT STDMETHODCALLTYPE LockSharedObjMass(
    tsp00_Int4              &cnt,
    tgg91_TransNo           *pConsistentView,
    tgg01_ContainerId      **ppContainerId,
    SAPDB_UInt8             *pOid,
    tgg91_PageRef           *pObjSeq,
    tsp00_Int8              &timeout,
    tsp00_Int4              &errCnt,
    tgg00_BasisError        *pDBError);

  virtual HRESULT STDMETHODCALLTYPE LockSharedObj(
    unsigned char  * lpConsistentView,
    unsigned char  * lpContainerId,
    SAPDB_UInt8    * lpoid,
    unsigned char  * lpobjseq,
    short          * lpb_error);

  virtual HRESULT STDMETHODCALLTYPE Signal(
    tsp00_TaskId TaskId, 
    tsp00_Bool   ok); // PTS 1110315

  virtual HRESULT STDMETHODCALLTYPE Wait(
    tsp00_Bool& ok); // PTS 1110315

  virtual HRESULT STDMETHODCALLTYPE ExistsContainer(
    tsp00_Int4            schemaId,
    const GUID           &guid,
    OmsContainerNo        containerNo,
    size_t                objectSize,
    size_t                keyLen,
    tgg01_ContainerId    &containerId,
    bool                 &useCachedKeys,
    bool                 &partitionedKeys,
    short                &error); // PTS 1110334

  virtual HRESULT STDMETHODCALLTYPE LockContainer(
    const tgg00_FileId &containerId,
    short              &error);

  virtual HRESULT STDMETHODCALLTYPE GetSessionHeapUsage(
    tsp00_8ByteCounter& heapUsage); 

  virtual HRESULT STDMETHODCALLTYPE EvalCallStatistics(
     bool            AllocatorOnly,
     OmsMonitorData &MonitorData); 

  virtual HRESULT STDMETHODCALLTYPE SimCtlDumpToHtml(
    OmsHandle                 &handle,
    SAPDB_UInt8                obj,
    class OmsSimDumpCollector &str); 

  virtual HRESULT STDMETHODCALLTYPE SimCtlSetObjectName(
    OmsHandle         &handle,
    SAPDB_UInt8        obj,
    const char        *name); 

  virtual HRESULT STDMETHODCALLTYPE SimCtlGetObjectName(
    OmsHandle          &handle,
    SAPDB_UInt8         obj,
    const char        **name);

  virtual HRESULT STDMETHODCALLTYPE SimCtlDestroyHandleCB(
    class OmsHandle          &handle,
    struct IliveCacheSink   **session); 

  //virtual HRESULT STDMETHODCALLTYPE GetSessionAllocator(
  //  SAPDBMem_IRawAllocator *&allocator); 
  //
  //virtual HRESULT STDMETHODCALLTYPE GetOmsSessionAllocator(
  //  SAPDBMem_IRawAllocator *&allocator); 
  
  virtual HRESULT STDMETHODCALLTYPE SimCtlGetHTMLRef(
    class OmsHandle           &handle,
    SAPDB_UInt8                obj,
    const char              **data,
    int                      flags); 

    virtual HRESULT STDMETHODCALLTYPE IsInRegion(
      int    regionId,
      bool  &inRegion,
      short &error); 

    virtual HRESULT STDMETHODCALLTYPE AttachSequence(
      const tsp00_KnlIdentifier &sequenceName,
      tgg00_Surrogate           &sequenceId,
      tgg00_SeqInfo             &sequenceInfo,
      short                     &sqlError); 

    virtual HRESULT STDMETHODCALLTYPE NextVal(
      const tgg00_Surrogate &sequenceId,
      tgg00_SeqInfo         &sequenceInfo,
      tsp00_Uint4           &nextVal,
      short                 &sqlError);

    virtual HRESULT STDMETHODCALLTYPE IsUnicodeInstance(
      bool  &isUnicodeInstance,
      short &sqlError);

    virtual HRESULT STDMETHODCALLTYPE VersionDictInsert(
      const OmsVersionId  &versionId,
      const OMS_Context   *pVersionContext,
      tgg00_BasisError    &error);

    virtual HRESULT STDMETHODCALLTYPE VersionDictFind(
      const OmsVersionId   &versionId,
      OMS_Context         **ppVersionContext);

    virtual HRESULT STDMETHODCALLTYPE VersionDictDrop(
      const OmsVersionId  &versionId,
      tgg00_BasisError    &error);

    virtual HRESULT STDMETHODCALLTYPE VersionDictShutdown(); 

    virtual HRESULT STDMETHODCALLTYPE VersionDictUnloadOldestVersion(
      bool &unloaded); 

    virtual HRESULT STDMETHODCALLTYPE VersionDictMarkNotUnloadable(
      OMS_Context *pVersionContext);

    virtual HRESULT STDMETHODCALLTYPE VersionDictMarkUnloadable(
      OMS_Context *pVersionContext);

    virtual HRESULT STDMETHODCALLTYPE VersionDictCreateIter(
      OMS_LockMode   lockMode, 
      void        **ppIter, 
      OMS_Context **ppContext);

    virtual HRESULT STDMETHODCALLTYPE VersionDictGetNext(
      void         *pIter, 
      OMS_Context **ppContext);

    virtual HRESULT STDMETHODCALLTYPE VersionDictDestructIter(
      void        *pIter);

    virtual int STDMETHODCALLTYPE GetSinkType();

    /* PTS 1130965 FF */
    virtual HRESULT STDMETHODCALLTYPE SetTransactionComment(
      tsp00_Int2         commentLength,
      const OmsTypeWyde *pComment,
      short             &Error);

    virtual HRESULT STDMETHODCALLTYPE IsTestKnlBadAllocActive(bool &active);

    virtual HRESULT STDMETHODCALLTYPE ResetMonitor();

private:
};

/*! endclass: IliveCacheSink */

class LVCSim_TXList {
public:
    LVCSim_TXList() {}  // no init, will be inited later
    LVCSim_TXList(LVCSim_TID &ourTx);
    ~LVCSim_TXList();
    bool sees(LVCSim_TID otherTx) const;

    void AddRef();
    void Release(bool doLock);
private:
    int         m_refCnt;
    int         m_otherTxCount;                   // count of other transactions
    LVCSim_TID  m_ourTx;                          // our TX ID (read view)
    LVCSim_liveCacheSink::ConsViewList*  m_otherTxList[MAX_CONCURRENT_TX]; // start timestamps of concurrent TXs at start time
};


#endif 
