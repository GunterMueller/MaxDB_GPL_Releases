/****************************************************************************

module      : LVCSim_Internal.hpp

-------------------------------------------------------------------------

responsible : IvanS

special area: liveCache Simulator
description : liveCache Sink implementation

-------------------------------------------------------------------------

copyright:    Copyright (c) 2002-2005 SAP AG




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


#ifndef LVCSIM_INTERNAL_HPP
#define LVCSIM_INTERNAL_HPP

/*===========================================================================*
*  INCLUDES                                                                 *
*===========================================================================*/

#define	LVCSIM_ONLY_GLOBALS
#include "LVCSimulator/LVCSim_Client.hpp"
#include "LVCSimulator/LVCSim_CheckpointIO.hpp"

#include "ggg00.h"
#include "ggg01.h"
#include "RunTime/Synchronisation/RTESync_Spinlock.hpp"
#include "geo573.h"

#undef global	// to prevent errors in iostream & company on Sun

#undef	NIL_OBJ_PAGE_NO	// to stop one warning
#include "liveCache/LVC_LibOmsInterface.hpp"

#ifdef  WIN32
#ifndef _DEBUG
#define	_DEBUG
#endif
#include <crtdbg.h>
#define LVCSIM_ASSERT(x) if (!x) _CrtDbgBreak();
#else
#include <assert.h>
#define LVCSIM_ASSERT(x) assert(x);
#endif


/*===========================================================================*
*  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
*===========================================================================*/




/*
* Internal structures for liveCache simulator
*
*/


/// #bits/container ID
#define	LVCSIM_CONTID_BITS		22
/// #bits/object address within container
#define	LVCSIM_OADDR_BITS		  26


#ifndef	MAX_SCHEMA_NAME_LENGTH_CO10
#define MAX_SCHEMA_NAME_LENGTH_CO10	32
#endif

#ifndef	NULL
#define	NULL	0
#endif

/// max. # of concurrent transactions
#define MAX_CONCURRENT_TX       128

#define	LVCSIM_THROW(txt)	throw DbpError(9999, txt)




// Wyde helper functions

/*!
 * \brief Compares two Wyde strings
 *
 * \param s1 [in] pointer to first string
 * \param s2 [in] pointer to second string
 * \return 0 if s1 == s2, -1 if s1 < s2, 1 if s1 > s2
 */
inline int LVCSim_wstrcmp(const OmsTypeWyde *s1, const OmsTypeWyde *s2)
{
  for (;;) {
    if (!*s1) {
      if (*s2) return -1;
      else return 0;
    }
    if (!*s2) return 1;
    if (*s1 < *s2) return -1;
    if (*s1 > *s2) return 1;
    ++s1;
    ++s2;
  }
}

/*!
 * \brief Copies a Wyde string
 *
 * \param dest [in] pointer to destination
 * \param src [in] pointer to source
 * \return pointer to destination
 */
inline OmsTypeWyde *LVCSim_wstrcpy(OmsTypeWyde *dest, const OmsTypeWyde *src)
{
  OmsTypeWyde *p = dest;
  while (*src) *p++ = *src++;
  *p = 0;
  return dest;
}



/*!
 * \brief This type is used internally to represent transaction timestamp.
 */
typedef	tsp00_Int4	LVCSim_TID;

/// TransID of no transaction.
#define	LVCSim_TID_NONE	-1
/// Maximum TransID (infinity).
#define	LVCSim_TID_MAX	0x7fffffff


class LVCSim_ContainerID {
public:
  GUID  guid; // container GUID
  int   schema; // schema ID
  int   cno;  // sub-container number

  LVCSim_ContainerID()
  {
  }

  LVCSim_ContainerID(LVCSim_CheckpointReader &i)
  {
    readFromStream(i);
  }

  LVCSim_ContainerID(const GUID &_guid, int _schema, int _cno)
    : guid(_guid), schema(_schema), cno(_cno)
  {
  }

  unsigned int hash() const
  {
    return guid.Data1 ^ (guid.Data1 >> 16) ^ guid.Data2 ^ guid.Data3 ^ guid.Data4[0] ^
      (guid.Data4[1] << 8) ^ (guid.Data4[2] << 16) ^ (guid.Data4[3] << 24) ^
      guid.Data4[4] ^ (guid.Data4[5] << 8) ^ (guid.Data4[6] << 16) ^
      (guid.Data4[7] << 24) ^ schema ^ (cno << 16);
  }

  bool operator ==(const LVCSim_ContainerID &x) const
  {
    return schema == x.schema && cno == x.cno && guid == x.guid;
  }

  void writeToStream(LVCSim_CheckpointWriter &o) const
  {
    o.write(&guid, sizeof(guid));
    o.write(&schema, sizeof(schema));
    o.write(&cno, sizeof(cno));
  }

  void readFromStream(LVCSim_CheckpointReader &i)
  {
    i.read(&guid, sizeof(guid));
    i.read(&schema, sizeof(schema));
    i.read(&cno, sizeof(cno));
  }
};

/*!
 * \brief This type is used internally to represent sink ID.
 */
typedef	class LVCSim_liveCacheSink	*LVCSim_SID;

class LVCSim_ContainerMeta;
class LVCSim_SchemaMeta;
class LVCSim_ObjectHandle;
class LVCSim_LockChainItem;
/*!
 * \brief Shared lock list class.
 */
class LVCSim_SharedLockListItem{
public:
  LVCSim_SharedLockListItem(LVCSim_SID sid, LVCSim_SharedLockListItem *prev, LVCSim_SharedLockListItem *next) 
    : m_sid(sid), m_next(next), m_prev(prev)
  {}
  LVCSim_SID m_sid;
  LVCSim_SharedLockListItem *m_prev;
  LVCSim_SharedLockListItem *m_next;
  LVCSim_LockChainItem      *m_correspondingLockChainItem;
};   

/*!
 * \brief lock chain item base class.
 */
class LVCSim_LockChainItem{
public:
  LVCSim_LockChainItem(bool isExclusive, LVCSim_SharedLockListItem *lockItem, LVCSim_LockChainItem *next):
      m_isExclusive(isExclusive),m_isShare(!isExclusive),m_correspondingObjectLock(lockItem),m_next(next),m_prev(NULL)
  {
    if(m_next)
      m_next->m_prev = this;
  }
  virtual void unlock(LVCSim_SID sid) = 0;
  bool m_isExclusive;
  bool m_isShare;
  LVCSim_SharedLockListItem *m_correspondingObjectLock;
  LVCSim_LockChainItem *m_next;
  LVCSim_LockChainItem *m_prev;
};

/*!
 * \brief schema lock chain item .
 */
class LVCSim_SchemaLockChainItem : public LVCSim_LockChainItem {
public:
  LVCSim_SchemaLockChainItem(LVCSim_SchemaMeta *schemaMeta, bool isExclusive, LVCSim_SharedLockListItem *lockItem, LVCSim_LockChainItem *next)
    : LVCSim_LockChainItem(isExclusive,lockItem,next), m_schemaMeta(schemaMeta)
  {}
  void unlock(LVCSim_SID sid);
  LVCSim_SchemaMeta *m_schemaMeta;  
};

/*!
 * \brief container lock chain item.
 */
class LVCSim_ContainerLockChainItem : public LVCSim_LockChainItem {
public:
  LVCSim_ContainerLockChainItem(LVCSim_ContainerMeta *containerMeta, bool isExclusive, LVCSim_SharedLockListItem *lockItem, LVCSim_LockChainItem *next)
    : LVCSim_LockChainItem(isExclusive,lockItem,next), m_containerMeta(containerMeta)
  {}
  void unlock(LVCSim_SID sid);
  LVCSim_ContainerMeta *m_containerMeta;
};

/*!
 * \brief object lock chain item.
 */
class LVCSim_ObjectLockChainItem : public LVCSim_LockChainItem {
public:
  LVCSim_ObjectLockChainItem(LVCSim_ObjectHandle *objectHandle, bool isExclusive, LVCSim_SharedLockListItem *lockItem, LVCSim_LockChainItem *next)
    : LVCSim_LockChainItem(isExclusive,lockItem,next), m_objectHandle(objectHandle)
  {}
  void unlock(LVCSim_SID sid);
  LVCSim_ObjectHandle *m_objectHandle;
};


/*!
 * \brief Schema metadata.
 *
 * Schema metadata are held in hashtable LVCSim_liveCacheSink::schemaDir.
 *
 * \see \ref lvcsim_tech_dir
 */
class LVCSim_SchemaMeta {

protected:
  typedef enum {
      o_new,      ///< The object was created.
      o_update,   ///< The object was updated.
      o_delete    ///< The object was deleted.
  } OpType;

  friend class	LVCSim_liveCacheSink;
  friend class	LVCSim_SchemaLockChainItem;

  OmsTypeWyde		m_name[MAX_SCHEMA_NAME_LENGTH_CO10 + 1];  ///< Schema name.
  int				    m_id;                                     ///< Schema ID.
  int           m_next;                                   ///< Unused (left for compatibility of dump files).

  static RTESync_Spinlock   m_objLock;                 ///< object lock

  bool          m_isDropped;                          ///< schema dropped?

  LVCSim_SID				m_lock;		        ///< Sink ID of transaction that holds the lock on the schema.

  LVCSim_SharedLockListItem *m_sharedLockList;        ///< Sink ID of transaction that holds the lock on the object
  LVCSim_SchemaMeta *m_lockChain;                     ///< list of all locked schemata in this transaction
  LVCSim_LockChainItem    *m_correspondingLockChainItem;  /// Lock chain item that is stored in the sink, provides faster unlock

  LVCSim_SchemaMeta *m_history;                       ///< list of the schemata history in this transaction

  /*
  static LVCSim_FastHash<LVCSim_ContainerID,
    OMS_ContainerHandle, LVCSim_ContainerMeta, false> *m_containerDir;*/

public:
  /// Create metadata from saved state.
  LVCSim_SchemaMeta(LVCSim_CheckpointReader &inp) 
  {
    inp.read(&m_id, sizeof(m_id));
    inp.read(&m_next, sizeof(m_next));
    inp.read(&m_name[0], sizeof(m_name));
  }

  /*!
   * \brief Create new schema
   *
   * \param _id new schema ID.
   * \param _name new schema name.
   */
  LVCSim_SchemaMeta(int _id, const OmsTypeWyde *_name) 
    : m_id(_id), m_next(-1), m_lock(NULL), m_isDropped(false), 
    m_lockChain(NULL), m_history(NULL), m_sharedLockList(NULL), m_correspondingLockChainItem(NULL)
  {
    LVCSim_wstrcpy(m_name, _name);
  } 

  /// Save schema metadata to saved state.
  void writeToStream(LVCSim_CheckpointWriter &o) const
  {
    o.write(&m_id, sizeof(m_id));
    o.write(&m_next, sizeof(m_next));
    o.write(&m_name[0], sizeof(m_name));
  }

  /*!
   * \brief get schema name
   */
  const OmsTypeWyde *getSchemaName() {return m_name;}

  /*!
   * \brief get unique schema id
   */
  int getSchemaId() {return m_id;}

  /*!
   * \brief Check if schema is cleared
   */
  bool isMarkAsDropped() {return m_isDropped;}

  /*!
   * \brief Check if schema is dropped
   */
  short markDropped(LVCSim_SID sid);

  /*!
   * \brief Check if schema is locked
   */
  bool isLocked(){ return m_lock != NULL;}
 
  /*!
   * \brief Check if schema is locked by the given sink
   */
  bool isLocked(LVCSim_SID sid){ return m_lock == sid;}

  /*!
   * \brief do end transaction cleanup
   */
  void doEndTxLL(LVCSim_SID sid, bool commit, char operation);

  /*!
   * \brief lock schema
   */
  short lockSchema(LVCSim_SID sid, bool exclusive);

  /*!
   * \brief unlock schema
   */
  short unlockSchema(LVCSim_SID sid);

  /*!
   * \brief drop all container of this schema
   */
  short dropAllContainer(LVCSim_SID sid);
};

/*!
 * \brief Container metadata and management of dropped containers.
 *
 * Metadata are held in hash table LVCSim_liveCacheSink::containerDir.
 *
 * \see \ref lvcsim_tech_dir
 */
class LVCSim_ContainerMeta {

protected:
  /// Operation type, what has happened to the container in transaction.
  typedef enum {
      o_new,      ///< The object was created.
      o_update,   ///< The object was updated.
      o_delete    ///< The object was deleted.
  } OpType;
  class DropInfo;

  friend class		LVCSim_liveCacheSink;
  friend class		LVCSim_ObjectHandle;
  friend class		KeyRangeIterator;
  friend class		LVCSim_ContainerLockChainItem;
  friend class	  LVCSim_ObjectLockChainItem;

  size_t				    m_size;					    ///< object body size
  tgg00_FileId		  m_containerDesc;		///< container info
  LVCSim_ContainerID m_containerId;     ///< container id (guid,schemaId,containerNo)
  tgg00_ObjColDesc	m_colDesc;				  ///< column info

  class LVCSim_ObjectTree		*m_tree;			///< key tree pointing to objects
  class LVCSim_OIDAllocator	*m_alloc;			///< allocator for record heads

  static DropInfo           *m_firstDrop;       ///< first container to drop
  static DropInfo           *m_lastDrop;        ///< last container to drop
  static RTESync_Spinlock   m_objLock;         ///< object lock

  LVCSim_SID				m_lock;		    ///< Sink ID of transaction that holds the lock on the container.
  //LVCSim_SID				m_sharedLock;		    ///< Sink ID of transaction that holds the shared lock on the container.

  bool              m_isDropped;

  bool              m_isVarObject;
  bool              m_useCachedKeys;
  bool              m_keyPartitioned;

  LVCSim_SharedLockListItem *m_sharedLockList;  ///< Sink ID of transaction that holds a shared lock on the object.
  LVCSim_ContainerMeta *m_lockChain;
  LVCSim_LockChainItem    *m_correspondingLockChainItem;  /// Lock chain item that is stored in the sink, provides faster unlock

  LVCSim_ContainerMeta *m_history;

  void *allocate(size_t sz);
  void deallocate(void *ptr);

public:
  /// Create new container info from saved state.
  LVCSim_ContainerMeta(LVCSim_CheckpointReader &i);
  /*!
   * \brief Create new container info.
   *
   * \param newID new container ID.
   * \param _size object size.
   * \param coldesc column descriptor for the key.
   * \param Is the container used to store var-sized objects.
   */
  LVCSim_ContainerMeta(OMS_ContainerHandle newHandle, size_t _size, LVCSim_ContainerID contId, tgg00_ObjColDesc *coldesc,
    bool isVarObject, bool useCachedKeys, bool keyPartitioned);
  ~LVCSim_ContainerMeta();

  /// Get container handle
  inline OMS_ContainerHandle getContHandle() const    // PTS 1124356 FF 2003-09-23 new FileDir
  {
    return m_containerDesc.fileDirFileId_gg00(); // PTS 1124356 FF 2003-09-23 new FileDir
  }

  /// Get container ID
  inline LVCSim_ContainerID getContID() const    
  {
    return m_containerId;
  }

  /// Write container info to saved state.
  void writeToStream(LVCSim_CheckpointWriter &o) const;

  /// Get allocator for allocating OIDs (persistent objects).
  class LVCSim_OIDAllocator *getAlloc() const
  {
    return m_alloc;
  }
  /*!
   * \brief Mark a container as dropped
   *
   * \param dropper sink.
   */
  short markDropped(LVCSim_SID dropper);

  /*!
   * \brief Add a container to dropped container list
   *
   * \param cid container ID.
   * \param tid newest transaction ID (which TX can see the container yet).
   * \param dropper transaction ID of dropping transaction.
   * \param c container metadata.
   */
  static void addDropped(
    class OMS_ContainerHandle &cHandle,
    LVCSim_TID tid,
    LVCSim_TID dropper,
    LVCSim_ContainerMeta *c);

  /*!
   * \brief Actually drop outdated containers up to tid.
   *
   * \param tid transaction ID of a view that is not needed anymore.
   */
  static void freeDropped(LVCSim_TID tid);

  /// Actually drop all outdated containers.
  static void freeAllDropped();

  /*!
   * \brief Find in dropped container list (used to check for dropped containers).
   *
   * \param container ID.
   * \param tid current transaction ID.
   * \return dropped container metadata if dropped or NULL if not in list.
   */
  static LVCSim_ContainerMeta *findDropped(
    class OMS_ContainerHandle &cHandle, 
    LVCSim_TID tid);  

  //int hash() {/*TODO*/ return m_tid_min;}

  /*!
   * \brief is container mark as dropped
   */
  bool isMarkAsDropped() {return m_isDropped;}

  /*!
   * \brief Check if container is locked
   */
  bool isLocked(){ return m_lock != NULL;}
 
  /*!
   * \brief Check if container is locked by the given sink
   */
  bool isLocked(LVCSim_SID sid){ return m_lock == sid;}

  /*!
   * \brief Do end transaction cleanup
   */
  void doEndTxLL(LVCSim_SID sid, bool commit, char operation);

  /*!
   * \brief lock container by sid
   */
  short lockContainer(LVCSim_SID sid,bool exclusive);

  /*!
   * \brief lock container by sid
   */
  short unlockContainer(LVCSim_SID sid);

};


extern OMS_LibOmsInterface		*omsInterface;

#endif
