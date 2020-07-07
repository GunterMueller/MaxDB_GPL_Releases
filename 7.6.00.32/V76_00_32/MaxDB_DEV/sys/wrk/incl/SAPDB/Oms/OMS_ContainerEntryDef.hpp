/*!
 * \file    OMS_ContainerEntryDef.hpp
 * \author  MarkusSi, Roterring
 * \brief   Single entry of OMS Container Dictionary.
 */
/*


    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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
#ifndef __OMS_CONTAINERENTRYDEF_HPP
#define __OMS_CONTAINERENTRYDEF_HPP

//#include "livecachetypes.h" // nocheck
#include "ggg250.h"
//#include "ggg00.h"
#include "ggg01.h"
#include "Oms/OMS_CacheMissEntry.hpp"
#include "Oms/OMS_ContainerHandle.hpp"  // PTS 1124356 FF 2003-09-23 new FileDir
#include "Oms/OMS_DynamicStack.hpp"     // PTS 1127661
#include "Oms/OMS_List.hpp"
#include "Oms/OMS_Oid.hpp"              // PTS 1127661
#include "Oms/OMS_Defines.h"
#include "Oms/OMS_GUID.hpp"
#include "Oms/OMS_ClassEntry.hpp"

class OMS_Context;
class OMS_FreeListHeader;
class OmsObjectContainer;
class OmsAbstractObject;

/// Entry of the container dictionary
class OMS_ContainerEntry {
public:
  OMS_ContainerEntry (OMS_Context             *pContext,
                      OMS_ClassEntry          *pClassEntry, 
                      const tgg01_ContainerId &containerId, 
                      OmsSchemaHandle          schema, 
                      OmsContainerNo           containerNo,
                      bool                     useCachedKeys,
                      bool                     partitionedKeys); 
  ~OMS_ContainerEntry();

  void* operator new(size_t sz, OMS_Context *pContext);
#if defined(OMS_PLACEMENT_DELETE)
  void  operator delete (void* p, OMS_Context *pContext);
#endif
  void           DeleteSelf();

  void                       CheckClassEntryVersionError();
  inline OMS_ClassEntry&     GetClassEntry();   
  void                       ResetClassEntry() { m_pClassEntry = NULL;   }
  OMS_ClassEntry&            SetClassEntry();

  const OMS_ContainerHandle& GetContainerHandle() const { return m_containerHandle; }  
  OmsContainerNo             GetContainerNo()     const { return m_containerNo; }
  tgg01_ContainerId&         GetFileId()                { return m_containerId; }
  OmsSchemaHandle            GetSchema()          const { return m_schema;      }
  ClassID                    GetGuid()            const { return m_guid;        }


  /// \name Handling of Drop-Flag
  //@{
  inline bool                IsDropped();  
  void                       MarkDropped()              { m_dropped = true;     }
  void                       ResetDropped()             { m_dropped = false;    }
  inline void                CheckIfNotDropped(const char *pFile, unsigned int line);
  //@}

  /// \name Handling of Create-Flag
  //@{
  void                       MarkCreated()      { m_created = true; }
  bool                       IsCreated() const  { return m_created; }
  void                       ResetCreated()     { m_created = false;    }
  //@}

  /// \name Accessing and modifying key-tree (used for objects created in a version or for cached keys)
  //@{
  inline tgg00_BasisError    VersionAddKey(OmsObjectContainer*, bool noThrowIfExist = false);
  inline void                VersionDelKey(OmsObjectContainer*);
  void                       VersionDelIndex(bool cachedKeysOnly);
  inline OmsObjectContainer* VersionFindKey(const unsigned char*);
  inline void                VersionReplaceOrAddKeyContainerPtr(OmsObjectContainer* p);
  inline OmsObjectContainer* VersionGetInfoFromNode(const unsigned char* const*);  // PTS 1117571
  //@}

  cgg250AvlTree<unsigned char*,OMS_ContainerEntry,OMS_Context>::Iterator VersionIterStart(unsigned char* pStartKey, OmsIterDirection direction);

  bool                       SetCachedKeys(bool);                          // PTS 1117571 
  inline bool                UseCachedKeys() const;                        // PTS 1117571

  bool                       PartitionedKeys() const { return m_partitionedKeys; }

  /// \name Accessing and modifying cache-miss structure (used for cached keys)
  //@{
  inline bool                IsCacheMiss(const unsigned char* key);        // PTS 1117571
  inline void                DeleteCacheMiss(const unsigned char* key);    // PTS 1117571
  inline void                InsertCacheMiss(const unsigned char* key);    // PTS 1117571
  void                       DropCacheMisses();                            // PTS 1117571
  inline int                 Compare(const unsigned char*, const unsigned char*);
  //@}

  /*! \name Accessing and modifying structure with reusable oids (Oid which have been deleted
  **  in the current transaction can be reused for newly created objects belonging to the same
  **  container 
  */
  //@{
  inline void                ClearReusableOids();                        // PTS 1127661
  inline bool                ExistsReusableOid() const;                  // PTS 1127661
  inline const OMS_ObjectId8*  GetReusableOid();                           // PTS 1127661
  inline void                InsertReusableOid(const OMS_ObjectId8 &oid);  // PTS 1127661
  //@}

  /*! \name Accessing and modifying structure, which contains objects which have been newly created
  **  in current version. This structure is needed for the class-iterator.)
  */
  //@{
  inline OMS_List<OmsObjectContainer>* GetNewVersionObjects() { return &m_newVersionObjects; }
  void                       ClearNewVersionObjects();
  inline void                RegisterNewVersionObject(OmsObjectContainer *p);
  inline void                RemoveNewVersionObject(OmsObjectContainer *p);
  //@}

  /// Get(reuse) / release object frames from a free-list
  //@{
  inline OmsObjectContainer* GetMemory(bool forNewVersionObj=false); 
  inline OMS_FreeListHeader& GetFreeList(bool inVersion) const;
  inline void                ChainFree (OmsObjectContainer *&pObj, int caller);
  //@}

  inline void                SetLocked()      { m_locked = true;  }
  inline void                ResetLocked()    { m_locked = false; m_sharedLockInKernel = false; }
  inline bool                IsLocked() const { return (m_locked || m_created); }

  void                       DecNewObjectsToFlush()       { --m_cntNewObjectsToFlush;      }
  void                       IncNewObjectsToFlush()       { ++m_cntNewObjectsToFlush;      }
  int                        GetNewObjectsToFlush() const { return m_cntNewObjectsToFlush; }
  void                       ResetNewObjectsToFlush()     { m_cntNewObjectsToFlush = 0;    }


  /*! Pointer to link together entries which belong to the same hash value 
  ** (see OMS_ContainerDirectory::m_ppContHandleHead) */
  OMS_ContainerEntry  *m_pContHandleHashNext;
  /*! Pointer to link together entries which belong to the same hash value 
  ** (see OMS_ContainerDirectory::m_ppContIdHead) */
  OMS_ContainerEntry  *m_pContIdHashNext;

  OMS_ContainerEntry  *m_nextInHistory;

private:
  // Implemented as separate method because of include problems
  void Throw(short errorNo, const char* msg, const char* pFile, unsigned int line);
  void Throw(short errorNo, const char* msg, const OMS_ObjectId8 &oid, const char* pFile, unsigned int line);

   /*! Context in which entry has been created */
  OMS_Context         *m_pContext;
  /*! Pointer to the corresponding class information in the class dictionary*/
  OMS_ClassEntry*      m_pClassEntry;
  /*! kernel structure (24 Bytes long), which contains the FileNo (fileDirFileId_gg00 8 Byte)
  **  and further information about the container like: FileType, number of key-partitions, 
  **  number of VarContinousContainers,… 
  **  \attention: m_containerId must be 8 Byte aligned on 64 Bit platforms due to optimized compares,
  ** therefore always keep it behind a pointer (m_pClassEntry)
  */
  tgg01_ContainerId    m_containerId;
  OMS_ContainerHandle  m_containerHandle;
  /*! Class-guid is remembered, as the pointer to the class info must be invalidated, if a version
  **  is closed. Later, if the version is reopened again, the class-guid is the only possibility
  **  to determine the corresponding class entry again.
  **  To allow trace-output even if the class entry is not set currently, the guid is accessible 
  **  via the method GetGuid
  */
  ClassID              m_guid;
  /*! Number of the container */
  OmsContainerNo       m_containerNo;
  /*! Schema identifier */
  OmsSchemaHandle      m_schema;
  /*! Has the container been dropped in the current transaction */  
  bool                 m_dropped;
  /*! Has the container been created in the current transaction. */
  bool                 m_created;
  /*! */
  bool                 m_locked;
  /*! This flag is used to remember whether a shared lock has been acquired already 
  **  in the kernel for this container. 'This lock is only used if context is a version context.*/
  bool                 m_sharedLockInKernel;
  /*! This variable is incremented for every object which is newly created in
  **  this container. This information is needed to check the number of objects
  **  which must be flushed during commit (see also OMS_Context::m_cntNewObjectsToFlush).
  **  The counter is kept separately for every container to allow fast changes
  **  of the 'session-global' counter when a container is dropped, created,...
  */
  int                  m_cntNewObjectsToFlush;
  /*! If true, several B*-trees are used to store the keys. When inserting a new object
  **  the tree in which the object has to be stored is determined using a hash function. 
  **  This distribution on several trees should prevent bottlenecks caused by concurrent 
  **  operations on the tree. Currently the keys are always distributed among 16 trees. 
  **  \attention Iterating on objects which are stored with partitioned keys might lead 
  **  to performance degradation, as the different trees must be merged during the iteration. 
  **  Partitioned keys are even worse if the running direction of the iterator is changed 
  **  during a single run.
  */
  bool                 m_partitionedKeys;

  /*! This structure contains all objects which are created newly in a version. 
  **  This structure needed for accesses via class iterator because the kernel is not 
  **  informed about new objects in versions and so the iterator would not find these objects. 
  */
  OMS_List<OmsObjectContainer>  m_newVersionObjects;

  /*! AVL-tree in which all the keyed objects are stored which are newly created in a 
  ** version. This is necessary as the kernel is not informed about the creation of such 
  ** objects and so there must be a possibility to locate these objects without the kernel. 
  ** If the cached key feature (PTS 1117571) is switched on, then if possible objects 
  ** which are loaded into the local cache are inserted into this structure, too. But as 
  ** this is not always possible the following invariant should be valid: Every object 
  ** which is stored in this structure must be stored in the local cache but not every 
  ** object in the local cache must be stored in the search structure. The advantage of 
  ** storing the objects in this structure is, that now key accesses to these objects can 
  ** be executed without asking the kernel first and therefore this should result in a 
  ** performance improvement for accesses to objects which are already stored in the local 
  ** cache. */
  cgg250AvlTree<unsigned char*,OMS_ContainerEntry,OMS_Context>  m_index;

  /// /name Members for cache miss structure      // PTS 1117571
  //@{ 
  /*! If the cached key feature is switched on, then this additional AVL-tree is used 
  ** to store the keys of those key accesses which does not find a corresponding object. 
  ** The second time a key access to such an object is started, there is no need to ask 
  ** the kernel again, as this access can be answered by this search structure.*/
  cgg250AvlTree<OMS_CacheMissEntry*,OMS_CacheMissEntry,OMS_Context>    m_cacheMiss;      
  /*! Is the cached key feature switched on or off? */
  bool                     m_useCachedKeys;   
  /*! If the cached key feature is switched off, then this pointer equals NULL; otherwise 
  ** it points to an item of the same type as those items which are stored in the 
  ** m_cacheMiss tree. This object is used to define a compare function for the tree 
  ** and furthermore this object is used for queries on this structure. To do this first 
  ** the search components are copied temporarily into this object and then this object is 
  ** given as parameter to the methods. (The advantage is, that the allocation of the object 
  ** occurs only once and as the copying is much cheaper this results in performance 
  ** improvement.) */
  OMS_CacheMissEntry*      m_cacheMissCmp;  
  //@}

  /*! Contains oids of objects which have been deleted in the current transaction and
  **  whose oids could be reused for newly created objects 
  **  \since PTS 1127661 */
  OMS_DynamicStack<OMS_ObjectId8,OMS_Context> m_reusableOids;

  OMS_FreeListHeader   *m_pFreeHead;
  OMS_FreeListHeader   *m_pFreeHeadInVersion;

  /*! This variable is used to check whether the class-entry pointer is still pointing to
  **  the correct version of the class-entry.
  */
  unsigned int          m_classEntryVersion;
};


/*------------------------------------------------------------------------*/

inline OMS_ClassEntry&   OMS_ContainerEntry::GetClassEntry()   
{ 
  if (m_pClassEntry != NULL){
    if (m_pClassEntry->GetVersion() != m_classEntryVersion){
      // Separate method is called to ensure, that GetClassEntry is inlined.
      CheckClassEntryVersionError();
    }

    return *m_pClassEntry;
  }
  else {
    return SetClassEntry();
  }
}

/*------------------------------------------------------------------------*/

#endif  // __OMS_CONTAINERENTRYDEF_HPP
