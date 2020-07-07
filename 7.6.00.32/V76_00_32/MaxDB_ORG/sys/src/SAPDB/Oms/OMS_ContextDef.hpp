/*!
 * \file    OMS_ContextDef.hpp
 * \author  MarkusSi, PeterG, Roterring
 * \brief   OMS context definition.
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

#ifndef __OMS_CONTEXTDEF_HPP
#define __OMS_CONTEXTDEF_HPP

#include "ggg01.h"
#include "Oms/OMS_RawAllocator.hpp"
#include "Oms/OMS_OidHash.hpp"
#include "Oms/OMS_ContainerDirectoryDef.hpp"
#include "Oms/OMS_VarObj.hpp"
#include "Oms/OMS_ContainerHandle.hpp"      // FF 2003-09-23 new FileDir
#include "liveCache/LVC_IliveCacheSink.hpp"
#include "Oms/OMS_DynamicStack.hpp"
#include "Oms/OMS_StackAllocator.hpp"
#include "Oms/OMS_AllocatorTracer.hpp"
#include "Container/Container_Hash.hpp"

class OMS_Session;
class OMS_UnknownContainerId;

typedef Container_Hash<OMS_ObjectId8,OmsObjectContainer*,/*AllowDuplicates=*/false> OMS_OidHashReadOnlyType;

/// Implements an OMS Context. 
/*! A Context is a consistent view together 
** with cached Objects belonging to this view. */
class OMS_Context {

#ifndef USE_SYSTEM_ALLOC_CO13
    class OMS_ContextAllocator : public SAPDBMem_RawAllocator
    {
    public :
        /*! constructor */
        OMS_ContextAllocator(tsp00_TaskId taskId, const OmsVersionId* pVersionId);

        void *allocate(size_t size){return Allocate(size);} // methods used in OMS_DynamicStack
        void deallocate(void *p){Deallocate(p);}
    private :
        /*! Tracer for the m_heap */
        OMS_AllocatorTracer m_heapTracer;
    };

  /*! Allocator for default context. This allocator can not deallocate memory. For the default
  ** context, this allocator is sufficient as all the memory will be freed on commit/rollback.
  ** As versions can live longer this allocator is not used if the context belongs to a version. */
  OMS_ContextAllocator m_heap; // caution! must be declared prior to m_oidDir, m_directory etc to
                               // ensure correct sequence of destruction
  OMS_StackAllocator   m_stackHeap;
    
  bool m_useStackHeap;
  OMS_DynamicStack<void*,OMS_ContextAllocator> m_memBlockStack;
#endif

public :
  enum LockType {
    NoLock,
    Lock,
    TryLock,
    LockShared,
    TryLockShared
  };

  /*! Dictionary of the persistent objects */
  OMS_OidHash              m_oidDir;
  /*! Contains the value of the consistent view in which objects from the kernel must be 
  ** read. For normal contexts the value is determined when the first object is read from 
  ** the kernel and for versions the value is set when the version is opened. */
  tgg91_TransNo            m_consistentView;

  OMS_Context(OMS_Session* session, const OmsVersionId* vid = NULL, const tgg01_OmsVersionContext* vctxt = NULL);
  ~OMS_Context();

  inline bool                  IsBoundToTrans() { return m_boundToTrans; }
  inline void                  ChangeNewObjectsToFlush(int cnt); 
  inline void                  CheckDBError(tsp00_Int2 e, const char* msg, const OMS_ObjectId8& oid);
  inline void                  CheckCompatibility(ClassID, OmsObjectContainerPtr);
  inline bool                  CheckOid (const OMS_ObjectId8& oid, ClassID guid);
  inline bool                  CheckOid (const OMS_ObjectId8& oid);
  inline void                  ClearObjCache(bool deleteObjects, bool adaptOidHash);
  inline void                  ClearVarObjData();
  inline void                  CloseVersion();
  inline tsp00_Int4            CurrentVersionKey() const;
  void                  DeleteSelf();
  inline void                  DecNewObjectsToFlush(OmsObjectContainer *p);
  inline void                  DeleteObjInContext (OmsObjectContainerPtr p, OMS_ContainerEntry*);
  void                  Dump(OMS_DumpInterface& dumpObj) const;
  void                  DumpObjCache(OmsHandle&);
  void                  EmptyObjCache(OMS_ContainerHandle &containerHandle);  // FF 2003-09-23 new FileDir
  inline bool                  ExistsConsistentView();
  inline OmsObjectContainerPtr FindObjInContext (const OMS_ObjectId8* pOid, bool ignoreGeneration=false, bool checkDropped=true,
                                  bool checkLocallyCreated=true);  
  inline OmsObjectContainerPtr FindVarObjInContext (const OMS_ObjectId8& oid, OMS_VarObjLockType, size_t size, void* buf);
  void                  FlushObj(OmsObjectContainerPtr p);
  void                  FlushDelete (OmsObjectContainerPtr pObj, bool deleteInHash); 
  void                  FlushObjCache(bool emptyObjCache);
  void                  FlushLockUpdObj(OmsObjectContainerPtr p);
  void                  FlushVarObject (OmsObjectContainerPtr p); 
  // Begin PTS 1121449
  void                  FlushObjCacheMass(bool emptyObjectCache, bool adaptOidHash); 
  void                  FlushVarObjMass(OmsObjectContainer*& pHeadOneClass, OmsObjectContainer*& pHeadOtherClasses, 
                                        OmsObjectContainer*& pFreeList);
  // End PTS 1121449
  void                  FlushVersionObjCache(bool isCommit);  // PTS 1122280
  inline void           deallocate(void* p);
  inline void           FreeStackHeap();
  inline void           FreeVarObj(unsigned char* p, size_t sz);
  inline OMS_ContainerDirectory& GetContainerDir();
  inline tsp00_Int4        GetHashSize() const;
  inline OMS_Context*      GetNext() const;
  inline OMS_Context*      GetNextUnloaded() const;
  inline OmsObjectContainerPtr GetObj(const OMS_ObjectId8& oid, bool doLock, bool shared);
  inline OmsObjectContainerPtr GetObj(ClassID guid, const OMS_ObjectId8& oid, bool doLock, bool shared);
  inline OmsObjectContainerPtr GetObjFromLiveCacheBase(const OMS_ObjectId8& oid, LockType doLock, bool mayBeVarObject = true);
  inline OmsObjectContainerPtr GetObjFromLiveCacheBase(ClassID, const OMS_ObjectId8& oid, LockType, tgg91_PageRef *objVers = NULL);
  inline OmsObjectContainerPtr GetObjViaCachedKey(OMS_ContainerEntry& containerInfo,    // PTS 1117571
    unsigned char* key, bool doLock, bool shared);
  inline OmsObjectContainerPtr GetObjViaKey(OMS_ContainerEntry& containerInfo, unsigned char* key, bool doLock, bool shared);
  const tsp00_Date&            GetCreateDate() const { return m_date; }
  const tsp00_Time&            GetCreateTime() const { return m_time; }
  const tsp00_Date&            GetOpenDate() const { return m_lastOpenDate; }
  const tsp00_Time&            GetOpenTime() const { return m_lastOpenTime; }
  inline OMS_Session*          GetSession(bool checkNull=true) const; 
  const OmsVersionId&          GetVersionId() { return m_version; }
  const OmsTypeWyde*           GetVersionDesc() const { return m_versionDesc; }  // PTS 1117690
  inline size_t                GetVarObjFromLiveCacheBase(const OMS_ObjectId8& oid, bool doLock, bool shared=false);
  inline void                  IncNewObjectsToFlush(OmsObjectContainer *p);
  inline void                  IncVersionKey ();
  bool                         IsDropped() const { return m_isDropped; }
  bool                         IsMarked() { return m_marked; } 
  inline bool                  IsVersionOid (const OMS_ObjectId8& oid) const;   // PTS 1117571
  inline bool                  IsVersionOpen();
  inline bool                  IsOpenVersion (const OmsVersionId& versionId);
  inline bool                  IsUnloadable();
  inline bool                  IsUnloaded() const { return (m_pVersionContext != NULL); }
  inline bool                  IsVersion();
  // PTS 1120478
  inline int                   LoadObjsFromLiveCacheBase(int noOfOid, const OMS_ObjectId8 *pOid,  tgg91_PageRef *pObjVers, tgg91_TransNo *pUpdTransId,
                                 bool doLock, bool shared, OmsObjectContainer **ppObjContainer, tgg00_BasisError *pDBError,  
                                 OmsAbstractObject **ppObj, OMS_ClassEntry **ppClassInfo, tgg01_ContainerId *pContainerId, 
                                 size_t *pObjSize);
  inline int                   LoadBaseClsObjsFromLiveCacheBase(int noOfOid, const OMS_ObjectId8 *pOid, tgg91_PageRef *pObjVers, tgg91_TransNo *pUpdTransId,
                                 bool doLock, bool shared, OmsObjectContainer **ppObjContainer, tgg00_BasisError *pDBError, 
                                 OmsAbstractObject **ppObj, OMS_ClassEntry **ppClassInfo, tgg01_ContainerId *pContainerId, 
                                 size_t *pObjSize);
  // PTS 1122194
  inline int                   LoadObjsViaKeyFromLCBase(int noOfObj, size_t *pKeyLen, unsigned char **ppBinaryKey, OMS_ObjectId8 *pOid, 
                                 tgg91_PageRef *pObjVers, tgg91_TransNo *pUpdTransId, bool doLock, bool shared, OmsObjectContainer **ppObjContainer, 
                                 tgg00_BasisError *pDBError, OmsAbstractObject **ppObj, OMS_ContainerEntry **ppContainerInfo, 
                                 tgg01_ContainerId *pContainerId, size_t *pObjSize);
  inline OmsObjectContainerPtr LoadVarObject (const OMS_ObjectId8& oid, OMS_VarObjLockType, size_t objSize, OMS_VarObjChunk& chunk, void* buf);
  inline void                  LockObj(const OMS_ObjectId8&oid);
  inline void                  LockSharedObj(const OMS_ObjectId8&oid);
  inline bool                  TryLockObj(const OMS_ObjectId8 &oid, bool shared);
  inline void                  LockResult (tsp00_Int2 DBError, OmsObjectContainerPtr p, const OMS_ObjectId8& oid, const char* msg, bool shared); 
  inline void                  LockResult (int cnt, tsp00_Int2* DBError, OmsObjectContainer** p, const OMS_ObjectId8 *pOid, const char* msg, bool shared); // PTS 1121449
  void                         LockObjWithBeforeImage();   // PTS 1128108
  inline bool                  TryLockResult (tsp00_Int2 DBError, bool shared, OmsObjectContainerPtr p, const OMS_ObjectId8& oid, const char* msg); 
  inline void*                 allocate(size_t sz);
  inline void                  MarkBoundToTrans(OMS_Session *pSession);
  void                         MarkDropped() { m_isDropped = true; }
  void                         MarkNotBoundToTrans();
  void                         MarkVersion() { m_marked = true; }
  void                         NewConsistentView();
  void                         OpenVersion(OMS_Session* session);
  inline void                  PutObjectIntoContext (OmsObjectContainerPtr p, OMS_ContainerHandle ContainerHandle); /*FF 2003-09-23 new FileDir*/
  inline OmsObjectContainerPtr ReactivateReplacedVersion(OmsObjectContainer* p);   // PTS 1125361
  void                         ReleaseAllUnchanged();   // PTS 1128262
  inline void                  RenameVersion(const OmsVersionId& OldId, const OmsVersionId& NewId);
  void                         ReduceHeapUsage(); /* PTS 1109340 */
  inline void                  ResetNewObjectsToFlush();
  inline void                  ResetConsistentView ();
  void                         ResetVersion(OMS_Session&); /* PTS 1115134 */
  inline void                  RemoveUnlockedObjFromCache();
  void                         RemoveCorruptVarObjects(bool checkOnly, const ClassID &guid, OmsSchemaHandle schema, OmsContainerNo containerNo);
  inline void                  SetConsistentView(const tgg91_TransNo&);
  void                         SetVersionDesc(const OmsTypeWyde*);       // PTS 1117690
  inline void                  SetNext(OMS_Context*);
  inline void                  SetNextUnloaded(OMS_Context*);
  inline void                  SetSession(OMS_Session *pSession);
  inline void*                 StackHeapMalloc(size_t size);
  void                         UnLoad(bool badAllocHandling);
  void                         VersionClearObjCache(bool changedObjAreNotAllowed);
  inline tgg01_OmsVersionContext*  VersionContext();
  inline OMS_ObjectId8           VersionNewOid();
  inline void*                 operator new(size_t sz);
#if defined(OMS_PLACEMENT_DELETE)
  inline void                  operator delete (void* p);
#endif
  inline tsp00_8ByteCounter    HeapUsed();
  tgg00_FileId&         VersionFile()    { return m_versionContext.ovc_tree; }
  tgg00_FileId&         VersionInvFile() { return m_versionContext.ovc_index_tree; }

  int  EmptyObjCache(OMS_DynamicStack<OmsObjectContainer*,OMS_Session> &containerObjects,
                     OMS_ContainerEntry *pContainerInfo);
  bool  CheckForUpdatedObj(OMS_ContainerHandle &containerHandle, 
                           OMS_DynamicStack<OmsObjectContainer*,OMS_Session> &containerObjects);

  void        ReadUnchangedObjectsStart();
  void        ReadUnchangedObjectsEnd();
  bool        IsReadOnlyContext() const { return m_isReadOnlyContext; }
  inline void ClearReadOnlyObjCache(bool deleteObjects);
  inline void ClearReadOnlyObjCache(bool deleteObjects, OMS_ContainerEntry *pContainerInfo);



private :
  /*! Pointer to the corresponding omsSession */
  OMS_Session*             m_session;
  /*! Dictionary of the containers */
  OMS_ContainerDirectory   m_containerDir;

  /*! Creation date of the context */
  tsp00_Date               m_date;
  /*! Creation time of the context */
  tsp00_Time               m_time;

  /*! This counter is increased when new objects are created and it is decreased/reset if 
  ** the corresponding objects are flushed. This counter is only used for checking purposes 
  ** during the flush. (There is one exception in which a new object will not increase 
  ** this counter, namely if a new object has been created, then is dropped and is  
  ** created again without flushing the cache in between. In this case the section creation 
  ** will not be counted.)
  ** \since PTS 1115545 */
  int                      m_cntNewObjectsToFlush;    

  /// @name Only relevant if context belongs to a version
  //@{
  /*! Name of the version  */
  OmsVersionId             m_version;
  /*! Description of the b*-tree in which the persistent objects are stored when a 
  ** version is unloaded. */
  tgg01_OmsVersionContext  m_versionContext;
  /*! If a version is not unloaded then this pointer equals NULL; otherwise it contains the 
  ** address of OMS_Context::m_versionContext.(So this pointer can be used to determine   
  ** whether a version is unloaded or not.) */
  tgg01_OmsVersionContext* m_pVersionContext;
  /*! Pointer to another version. This pointer is used to chain the entries in the
  **  version directory hash. */
  OMS_Context*             m_next;
  /*! Pointer to another version. Normally this pointer equals NULL. Only those versions which are not 
  ** allowed to be unloaded are linked together using this pointer. */
  OMS_Context*             m_nextUnloaded;
  /*! Is version open, which means is version bound to a session and active? */
  bool                     m_isOpen;
  /*! If the deletion of the version fails with an error-code -5001, then the version is 
  ** marked as deleted. Later when accessing this version again this flag is recognized 
  ** and then the deletion is triggered again. The deletion can fail because this operation 
  ** is done outside of the critical section (because of contention/performance issues) */
  bool                     m_isDropped;
  /*! Does the context belong to a version? */
  bool                     m_isVersion;
  /*! Is this version bound to a session? This member ensures that a version is only open 
  ** in one session. (see OMS_Session::m_versionsBoundToTrans) */
  bool                     m_boundToTrans;
  /*! The version is marked by a call of OmsHandle::omsMarkVersion. This flag is only used
  ** by the application programmer and is not considered in the oms-layer */
  bool                     m_marked;
  /*!
  */
  bool                     m_isReadOnlyContext;
  /*! Date of the last opening of the version */
  tsp00_Date               m_lastOpenDate;
  /*! Time of the last opening of the version */
  tsp00_Time               m_lastOpenTime;
  /*! A null-terminated string which contains the (unicode) description of the version. 
  ** If no description exists, then the member equals NULL.
  ** \since PTS 1117690 */
  OmsTypeWyde*             m_versionDesc;  
  /*!
  */
  OMS_OidHashReadOnlyType *m_pOidDirReadOnly;
  //@}

  friend class OMS_VersionDictionary;
};

typedef OMS_Context* OMS_ContextPtr;

#endif // __OMS_CONTEXTDEF_HPP
