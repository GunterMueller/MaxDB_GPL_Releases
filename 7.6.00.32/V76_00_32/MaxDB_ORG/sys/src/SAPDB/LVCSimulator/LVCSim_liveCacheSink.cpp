/*!***************************************************************************

  module    : LVCSim_liveCacheSink.cpp

  -------------------------------------------------------------------------

  responsible : ChristophR,MarkusSi

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


/*===========================================================================*
 *  INCLUDES                                 *
 *===========================================================================*/

#ifdef HPUX
// work around include problem on HP
#include <limits>
#endif

#include "liveCache/LVC_ProcServerExecutor.hpp"
#include "liveCache/LVC_LoadComponentLibrary.hpp"
#include "liveCache/LVC_Dispatcher.hpp"
#include "DBProc/liveCache/LVCPS_ParamHandler.hpp"
#include "LVCSimulator/LVCSim_liveCacheSink.hpp"
#include "LVCSimulator/LVCSim_ObjectHandle.hpp"
#include "LVCSimulator/LVCSim_ObjectTree.hpp"
#include "LVCSimulator/LVCSim_KernelInterface.hpp"
#include "LVCSimulator/LVCSim_ProcServerInterface.hpp"
#include "Oms/OMS_Session.hpp"
#include "Oms/OMS_VarObj.hpp"
#include "Oms/OMS_VarObjByClsIter.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"


#include <string>

#include "heo52.h"
#include "gsp01.h"
#include "heo01.h"
#include "hsp77.h"
#include "heo02x.h"

#ifdef  WIN32
#include <sys/timeb.h>
#include <time.h>
#include <direct.h>
#else
#include <sys/time.h>
#include <time.h>
#endif

#define MAX_REGION_CNT  (7+1024)  /* INTERNAL_CRITICAL_SECTIONS_CO001 in gco001.h */


#undef  SIM_DO_TRACE

#define SIM_TX  1
#define SIM_VER 2

#ifdef  SIM_DO_TRACE
#define SIM_TRACE(level, msg, param)                \
  if (sim_trace_level & level) printf("%08x " msg "\n", this, param);
#define SIM_TRACE2(level, msg, param, param2)           \
  if (sim_trace_level & level) printf("%08x " msg "\n", this, param, param2);
#else
#define SIM_TRACE(level, msg, param)
#define SIM_TRACE2(level, msg, param, param2)
#endif


///*!
// * Locker class for concurrent container access.
// *
// * Instance of this class protects container metadata from concurrent access by
// * multiple transactions. It does NOT do locking of containers as such, there
// * is LVCSim_ContainerMeta::m_lock member for that.
// *
// */
//class LVCSim_ContainerMetaObjLock {
//public:
//    enum {
//        /// Count of different container spinlocks.
//        MAX_OBJ_LOCKS   = 59
//    };
//
//    /*!
//     * Initialize lock instance and lock the lock.
//     *
//     * \param ref reference to object handle to lock,
//     * \param sid sink ID.
//     */
//    LVCSim_ContainerMetaObjLock(LVCSim_ContainerMeta *ref, LVCSim_SID sid)
//        : m_spinlock(&locks[ref->hash() % MAX_OBJ_LOCKS])
//    {
//        m_spinlock->Lock();
//    }
//
//    /// Unlock the lock and destroy the instance.
//    ~LVCSim_ContainerMetaObjLock()
//    {
//        if (m_spinlock) {
//            m_spinlock->Unlock();
//        }
//    }
//private:
//	static RTESync_Spinlock	locks[MAX_OBJ_LOCKS];   ///< Underlying spinlocks.
//protected:
//    RTESync_Spinlock        *m_spinlock;            ///< Spinlock for this instance.
//};


/*!
 * \brief Sequence handle.
 */
class LVCSim_Sequence {
public:
    LVCSim_Sequence() : m_value(0)
    {
    }

    LVCSim_Sequence(const LVCSim_Sequence &x) : m_value(x.m_value)
    {
    }

    tsp00_Uint4 getNextValue()
    {
        RTESync_LockedScope lck(m_lock);
        return ++m_value;
    }

private:
    RTESync_Spinlock    m_lock;
    tsp00_Uint4         m_value;
};

typedef LVCSIM_STD map<tsp00_KnlIdentifier, LVCSim_Sequence>  LVCSim_SequenceMap;
typedef LVCSIM_STD pair<const tsp00_KnlIdentifier, LVCSim_Sequence>  LVCSim_SequencePair;

static RTESync_Spinlock     sequenceLock;
static LVCSim_SequenceMap   *sequenceMap;


static int sim_trace_level = SIM_TX | SIM_VER;
static FILE *traceFile = NULL;
static RTESync_Spinlock traceLock;

static double raw_alloc_space[sizeof(SAPDBMem_RawAllocator) / sizeof(double) + 1];

/// How many times to do HTML dump.
static int  dump_count  = 1<<30;


tsp00_TaskId LVCSim_liveCacheSink::m_taskIdCnt = 1;


/*===========================================================================*
 *  GLOBALS                                  *
 *===========================================================================*/

class LVCSim_liveCacheSink::SchemaName {
public:
  OmsTypeWyde   m_name[MAX_SCHEMA_NAME_LENGTH_CO10 + 1];

  SchemaName()
  {
    m_name[0] = 0;
  }

  SchemaName(LVCSim_CheckpointReader &inp)
  {
    readFromStream(inp);
  }

  SchemaName(const OmsTypeWyde *ptr)
  {
    LVCSim_wstrcpy(m_name, ptr);
  }

  SchemaName(const SchemaName &x)
  {
    LVCSim_wstrcpy(m_name, x.m_name);
  }

  bool operator ==(const SchemaName &x) const
  {
    return LVCSim_wstrcmp(m_name, x.m_name) == 0;
  }

  const OmsTypeWyde *operator()() const
  {
    return m_name;
  }

  unsigned int hash() const
  {
    int sum = 0;
    const OmsTypeWyde *p = m_name;
    while (*p) sum = (sum << 1) ^ (sum >> 30) ^ *p++;
    return sum;
  }

  void readFromStream(LVCSim_CheckpointReader &inp)
  {
    inp.read(&m_name, sizeof(m_name));
  }

  void writeToStream(LVCSim_CheckpointWriter &o) const
  {
    o.write(&m_name, sizeof(m_name));
  }
};

class LVCSim_liveCacheSink::ConsViewList {
public:
  LVCSim_TID          m_tid;
  ConsViewList       *m_next;
  int                 m_refCnt;
  bool                m_isOmsVersion;

  ConsViewList()
  {
      // no init, will be inited later with second constructor
  }

  ConsViewList(LVCSim_TID _tid, bool isOmsVersion, ConsViewList *_next) 
        : m_tid(_tid), m_isOmsVersion(isOmsVersion), m_next(_next), m_refCnt(1)
  {
  }

  ~ConsViewList()
  {
    delete m_next;
  }

    void AddRef()
    {
        ++m_refCnt;
    }

    void Release()
    {
        if (m_refCnt > 1) {
            --m_refCnt;
        } else {
            Release(m_tid);
        }
    }

    static void Release(LVCSim_TID tid)
    {
    ConsViewList *last = NULL, *cur = LVCSim_liveCacheSink::consViewList;
    while (cur && cur->m_tid != tid) {
      last = cur;
      cur = cur->m_next;
    }
    if (!cur) {
      // this shouldn't happen
            LVCSIM_ASSERT(false)
    }
        if (cur->m_refCnt > 1) {
            --(cur->m_refCnt);
        } else {
#ifdef SIM_DO_TRACE
            cur->traceRelease();
#endif
        if (last) {
                if ((last->m_next = cur->m_next) == NULL) {
                    LVCSim_liveCacheSink::txOldest = last->m_tid;
                    LVCSim_ContainerMeta::freeDropped(LVCSim_liveCacheSink::txOldest);
                }
            } else {
                if ((LVCSim_liveCacheSink::consViewList = cur->m_next) == NULL) {
                    LVCSim_liveCacheSink::txOldest = LVCSim_TID_MAX;
                    LVCSim_ContainerMeta::freeAllDropped();
                }
            }
            cur->m_next = NULL;
            delete cur;
        }
    }

private:
    void traceRelease()
    {
        SIM_TRACE(SIM_TX, "release cons view: %d", m_tid);
    }
};


class LVCSim_liveCacheSink::UndoList {
public:
    class Item {
    public:
        Item(LVCSim_ObjectHandle::OpType typ): m_typ(typ), m_next(NULL)
        {
        }
        Item *m_next;
        LVCSim_ObjectHandle::OpType m_typ;
        virtual void doEndTxLL(LVCSim_SID sid, bool commit) = 0;
    };

    class ContainerItem : public Item {
    public:
        ContainerItem(LVCSim_ContainerMeta *con, LVCSim_ObjectHandle::OpType typ): Item(typ), m_con(con)
        {
        }
        LVCSim_ContainerMeta *m_con;
        void doEndTxLL(LVCSim_SID sid, bool commit) {m_con->doEndTxLL(sid,commit,m_typ);}
    };

    class SchemaItem : public Item {
    public:
        SchemaItem(LVCSim_SchemaMeta *schema, LVCSim_ObjectHandle::OpType typ): Item(typ), m_schema(schema)
        {
        }
        LVCSim_SchemaMeta *m_schema;
        void doEndTxLL(LVCSim_SID sid, bool commit) {m_schema->doEndTxLL(sid,commit,m_typ);}
    };

    class ObjectItem : public Item {
    public:
        ObjectItem(LVCSim_ObjectHandle *obj, LVCSim_ObjectHandle::OpType typ) : Item(typ), m_obj(obj)
        {
        }
        LVCSim_ObjectHandle *m_obj;
        void doEndTxLL(LVCSim_SID sid, bool commit) {m_obj->doEndTxLL(sid,commit,m_typ);}
    };

    class ItemList {
    public:
        ItemList   *m_next;
        int         m_count;
        Item       *m_item;

        ItemList(ItemList *_next) : m_next(_next), m_count(0), m_item(NULL)
        {
        }

        ~ItemList()
        {
          Item *next,*curr;
          for(curr=m_item; curr; curr = next){
            next = curr->m_next;
            delete curr;
          }
        }

        bool hasFreeItem() const    { return m_count < 256; }
        void addItem(Item *item)
        {
          item->m_next = m_item;
          m_item = item;
        }
    };

protected:

    ItemList   *items;

    void addObj(LVCSim_ObjectHandle *obj, LVCSim_ObjectHandle::OpType typ)
    {
        if (!items || !items->hasFreeItem()) {
            items = new ItemList(items);
        }
        items->addItem(new ObjectItem(obj,typ));
    }

    void addObj(LVCSim_ContainerMeta *con, LVCSim_ObjectHandle::OpType typ)
    {
        if (!items || !items->hasFreeItem()) {
            items = new ItemList(items);
        }
        items->addItem(new ContainerItem(con,typ));
    }

    void addObj(LVCSim_SchemaMeta *schema, LVCSim_ObjectHandle::OpType typ)
    {
      if (!items || !items->hasFreeItem()) {
          items = new ItemList(items);
      }
      items->addItem(new SchemaItem(schema,typ));
    }

    void loopAll(LVCSim_SID sid, bool commit)
    {
        ItemList *cur = items;
        while (cur) {
            for (Item *item = cur->m_item; item != NULL ; item = item->m_next) {
                item->doEndTxLL(sid, commit);
            }
            cur = cur->m_next;
        }
    }

public:
    UndoList() : items(NULL)
    {
    }

    ~UndoList()
    {     
        ItemList *next,*curr;
        for(curr=items; curr; curr = next){
          next = curr->m_next;
          delete curr;
        }
    }

    void append(UndoList *link)
    {
        // newest changes to front, our changes to back
        if (!link) {
            return;
        }
        if (!link->items) {
            delete link;
            return;
        }
        if (items) {
            ItemList *cur = link->items;
            while (cur->m_next) {
                cur = cur->m_next;
            }
            cur->m_next = items;
        }
        items = link->items;
        link->items = NULL;
        delete link;
    }
    
    void addDelete(LVCSim_ObjectHandle *obj)
    {
        addObj(obj, LVCSim_ObjectHandle::o_delete);
    }

    void addInsert(LVCSim_ObjectHandle *obj)
    {
        addObj(obj, LVCSim_ObjectHandle::o_new);
    }

    void addUpdate(LVCSim_ObjectHandle *obj)
    {
        addObj(obj, LVCSim_ObjectHandle::o_update);
    }

    void addDelete(LVCSim_ContainerMeta *con)
    {
        addObj(con, LVCSim_ObjectHandle::o_delete);
    }

    void addInsert(LVCSim_ContainerMeta *con)
    {
        addObj(con, LVCSim_ObjectHandle::o_new);
    }
  
    void addDelete(LVCSim_SchemaMeta *schema)
    {
        addObj(schema, LVCSim_ObjectHandle::o_delete);
    }

    void addInsert(LVCSim_SchemaMeta *schema)
    {
        addObj(schema, LVCSim_ObjectHandle::o_new);
    }

    void commit(LVCSim_SID sid)
    {
        loopAll(sid, true);
    }

    void rollback(LVCSim_SID sid)
    {
        loopAll(sid, false);
    }
};

class LVCSim_liveCacheSink::SubTxList {
public:
  UndoList            *m_undoList;    // saved undo list
  SubTxList     *m_next;        // next level subtrans

  SubTxList(UndoList *_undo, SubTxList *_next) 
        : m_undoList(_undo), m_next(_next)
  {
  }

  ~SubTxList()
  {
    delete m_next;
    delete m_undoList;
  }
};


class LVCSim_liveCacheSink::ActiveVersion {
friend class LVCSim_liveCacheSink;
public:
    class XOmsVersionId {
    public:
        OmsVersionId    m_id;
        XOmsVersionId(const OmsVersionId &id)
        {
            SAPDB_MemCopyNoCheck(m_id, id, sizeof(m_id));
        }

        bool operator ==(const XOmsVersionId &id) const
        {
            return memcmp(m_id, id.m_id, sizeof(m_id)) == 0;
        }

        bool operator <(const XOmsVersionId &id) const
        {
            return memcmp(m_id, id.m_id, sizeof(m_id)) < 0;
        }
    };
    typedef LVCSIM_STD map<LVCSim_TID, ActiveVersion*>     TXMap;
    typedef LVCSIM_STD pair<const LVCSim_TID, ActiveVersion*>     TXPair;
    typedef LVCSIM_STD pair<const XOmsVersionId, ActiveVersion*>  IDPair;

private:
    LVCSim_TID      m_readView;
    LVCSim_TXList   *m_otherTXs;
    bool            m_delete;
    bool            m_inUse;

    static RTESync_Spinlock     lock;
    static TXMap                txMap;

public:
    ActiveVersion() 
        : m_readView(LVCSim_TID_MAX), m_otherTXs(NULL),
          m_delete(false), m_inUse(true)
    {
        RTESync_LockedScope lck(lock);
    }

    ~ActiveVersion()
    {
        closeView();

        RTESync_LockedScope lck(lock);
    }

    inline LVCSim_TID getReadView() const   { return m_readView; }
    inline LVCSim_TXList *getOtherTXs()
    {
        if (m_otherTXs) {
            m_otherTXs->AddRef();
        }
        return m_otherTXs;
    }

    inline void markInUse()                 { m_inUse = true; }
    inline void unmarkInUse()               
    { 
        m_inUse = false; 
        if (m_delete) {
            delete this;
        }
    }
    inline bool isInUse() const             { return m_inUse; }
    inline bool isMarkedForDelete() const   { return m_delete; }

    void destroy()
    { 
        if (!m_inUse) {
            delete this;
        } else {
            m_delete = true;
        }
    }

    static ActiveVersion *find(LVCSim_TID txid)
    {
        RTESync_LockedScope lck(lock);
        TXMap::iterator i = txMap.find(txid);
        if (i == txMap.end()) {
            return NULL;
        } else {
            return i->second;
        }
    }

    void closeView()
    {
        if (m_readView != LVCSim_TID_MAX) {
            if (m_otherTXs) {
                m_otherTXs->Release(true);
            }
            m_otherTXs = NULL;

            RTESync_LockedScope lck(lock);
            txMap.erase(m_readView);
            m_readView = LVCSim_TID_MAX;
        
        }
    }

    void openView(LVCSim_TXList *other, LVCSim_TID txid)
    {
        closeView();
        if (other) {
            other->AddRef();
            m_otherTXs = other;
        }
        m_readView = txid;

        RTESync_LockedScope lck(lock);
        txMap.insert(TXPair(txid, this));
    }
};

LVCSim_liveCacheSink::ActiveVersion::TXMap  LVCSim_liveCacheSink::ActiveVersion::txMap;
RTESync_Spinlock                            LVCSim_liveCacheSink::ActiveVersion::lock;


class LVCSim_ContainerMeta::DropInfo {
public:
    OMS_ContainerHandle m_cHandle;    // container Handle
    LVCSim_TID  m_dropper;            // dropping transaction ID
    LVCSim_TID  m_dropTime;           // when should this container be dropped
    DropInfo    *m_nextDrop;          // next container to drop
    LVCSim_ContainerMeta    *m_meta;  // dropped container metadata

    DropInfo(OMS_ContainerHandle &cHandle, LVCSim_TID dropper, LVCSim_TID dropTime,
        LVCSim_ContainerMeta *m) 
        : m_cHandle(cHandle), m_dropper(dropper), m_dropTime(dropTime), m_nextDrop(NULL), m_meta(m)
    {
    }
    ~DropInfo()
    {
        delete m_meta;
        delete m_nextDrop;
    }
};


class KeyRangeIterator {
public:
  LVCSim_ObjectHandle   *cur;
  LVCSim_ContainerMeta  *cont;
  size_t          keySize;
  char          *startKey;
  char          *stopKey;

  KeyRangeIterator(const void *start, const void *stop, size_t keyLen, 
    LVCSim_ContainerMeta *_cont, LVCSim_ObjectHandle *_cur) 
    : cur(_cur), cont(_cont), keySize(keyLen), startKey(NULL), stopKey(NULL)
  {
    if(start){
      startKey = (char*)&stopKey + sizeof(char*);
      if (start && keyLen && cur) {
        if (cont->m_tree->compareKey(cur, start, keySize) < 0) {
          cur = NULL;
          return;
        } else {
          SAPDB_MemCopyNoCheck(startKey, start, keyLen);
        }
      }
    }
    if (stop){
      stopKey = (char*)&stopKey + sizeof(char*) + keySize;
      if (stop && keyLen && cur) {
        if (cont->m_tree->compareKey(cur, stop, keySize) > 0) {
          cur = NULL;
          return;
        } else {
          SAPDB_MemCopyNoCheck(stopKey, stop, keyLen);
        }
      }
    }
  }

  LVCSim_ObjectHandle *currObject()  // PTS 1119480
  {
    return cur;
  }

  LVCSim_ObjectHandle *findKey(void                 *pRestartKey, 
                               OmsIterDirection      direction,
                               LVCSim_liveCacheSink *pLCSink,
                               bool                  bInclusive)  // PTS 1119480
  {
    cur = cont->m_tree->findKey(pRestartKey, keySize, pLCSink, true, direction);

    if (direction == OMS_ASCENDING){
      if (!bInclusive && cur && cont->m_tree->compareKey(cur, pRestartKey, keySize) == 0) {
        cur = nextObject(pLCSink);
      }
      if (cur && stopKey && cont->m_tree->compareKey(cur, stopKey, keySize) > 0) {
        // the object is outside the interval
        cur = NULL;
      }
    }
    else {
      if (!bInclusive && cur && cont->m_tree->compareKey(cur, pRestartKey, keySize) == 0) {
        cur = prevObject(pLCSink);
      }
      if (cur && startKey && cont->m_tree->compareKey(cur, startKey, keySize) < 0) {
        // the object is outside the interval
        cur = NULL;
      }
    }

    return cur;
  }

  LVCSim_ObjectHandle *nextObject(LVCSim_SID sid)  // PTS 1119480
  {
    if (cur){
      cur = cont->m_tree->nextKey(cur, sid);
    }
    else {
      cur = cont->m_tree->firstKey(sid);
    }

    if (cur && stopKey && cont->m_tree->compareKey(cur, stopKey, keySize) > 0) {
      // the object is outside the interval
      return NULL;
    } else {
      return cur;
    }
  }

  LVCSim_ObjectHandle *prevObject(LVCSim_SID sid)  // PTS  1119480
  {
    if (cur){
      cur = cont->m_tree->prevKey(cur, sid);
    }
    else {
      cur = cont->m_tree->lastKey(sid);
    }

    if (cur && startKey && cont->m_tree->compareKey(cur, startKey, keySize) < 0) {
      // the object is outside the interval
      return NULL;
    } else {
      return cur;
    }
  }

  void *operator new(size_t sz1, size_t sz2)
  {
    return new char[sz1 + 2*sz2];  // PTS  1119480
  }

  void operator delete(void *ptr)
  {
    delete[] ((char*)ptr);
  }

  void operator delete(void *ptr, size_t sz)
  {
    delete[] ((char*)ptr);
  }
};

#ifdef WIN32
#pragma warning(disable: 4251)
#endif

class LVCSIM_API LVCSim_CStr : public OmsSimDumpCollector, public LVCSIM_STD string
{
public:
  virtual void printf(const char *fmt, ...);
  virtual const char* str() const;
  virtual void flush();
};

#ifdef WIN32
#pragma warning(default: 4251)
#endif


void LVCSim_CStr::flush()
{
  assign(""); 
}

void LVCSim_CStr::printf(const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  char buf[65536];
  sp77vsprintf(buf, sizeof(buf), fmt, args);
  va_end(args);
  append(buf);
}

const char* LVCSim_CStr::str() const
{
  return c_str();
}


// type-free iterator to iterate over a container
class OmsDumpClsIterator : public OmsObjByClsIterBase {
public:
  inline OmsDumpClsIterator(const OmsDumpClsIterator &iter) : OmsObjByClsIterBase(iter)
  {
  }

  inline operator bool () const
  {
    return OmsObjByClsIterBase::operator bool();
  }

  inline const OMS_ObjectId8& operator()() const
  {
    return OmsObjByClsIterBase::operator()();
  }

  inline OmsAbstractObject* omsDeRef() const
  {
    return OmsObjByClsIterBase::omsDeRef(false, false);
  }

  inline void operator++ ()
  {
    OmsObjByClsIterBase::operator++();
  }
};


// type-free object for calls of virtual HTML dump functions
class OmsDumpObject : public OmsAbstractObject
{
public:
  static OmsDumpClsIterator omsAllOids(OmsHandle &h, ClassID guid, bool isVarObject,
    OmsSchemaHandle sh, OmsContainerNo ContainerNo)
  {
    if(isVarObject)
    {
      OmsObjByClsIterBase iter = h.omsAllVarOids(guid, sh, ContainerNo);
      return *reinterpret_cast<OmsDumpClsIterator*>(&iter);
    }
    else
    {
      OmsObjByClsIterBase iter = OmsAbstractObject::omsAllOids(h, guid, sh, ContainerNo, 20);
      return *reinterpret_cast<OmsDumpClsIterator*>(&iter);
    }
  }
};


/*===========================================================================*
 *  STATICS & DEFINES                            *
 *===========================================================================*/

int LVCSim_liveCacheSink::schemaCount;
int LVCSim_liveCacheSink::containerCount;

LVCSim_FastHash<LVCSim_liveCacheSink::SchemaName, int,
  LVCSim_SchemaMeta, false>  *LVCSim_liveCacheSink::schemaDir;
RTESync_Spinlock        *LVCSim_liveCacheSink::schemaLock;

// PTS 1124356 FF 2003-09-23 new FileDir
LVCSim_FastHash<LVCSim_ContainerID, OMS_ContainerHandle,
  LVCSim_ContainerMeta, false>  *LVCSim_liveCacheSink::containerDir;
RTESync_Spinlock        *LVCSim_liveCacheSink::containerLock;

RTESync_Spinlock        *LVCSim_liveCacheSink::regionLocks;

RTESync_Spinlock  LVCSim_liveCacheSink::txCounterLock;
LVCSim_TID      LVCSim_liveCacheSink::txCounter;

LVCSim_TID            LVCSim_liveCacheSink::txOldest;
LVCSim_liveCacheSink           *LVCSim_liveCacheSink::firstSession = NULL;  // first session
LVCSim_liveCacheSink::ConsViewList  *LVCSim_liveCacheSink::consViewList;


RTESync_Spinlock    LVCSim_liveCacheSink::sinkPtrLock;
LVCSim_liveCacheSink* LVCSim_liveCacheSink::sinkPtr[LVCSIM_MAX_SINK_PTR];
bool          LVCSim_liveCacheSink::sinkPtrC[LVCSIM_MAX_SINK_PTR];
int           LVCSim_liveCacheSink::sinkPtrFree;

int LVCSim_liveCacheSink::dumpSequence = -1;


LVCSim_ContainerMeta::DropInfo *LVCSim_ContainerMeta::m_firstDrop = NULL;   // first container to drop
LVCSim_ContainerMeta::DropInfo *LVCSim_ContainerMeta::m_lastDrop = NULL;    // last container to drop
RTESync_Spinlock                LVCSim_ContainerMeta::m_objLock;           // container drop lock

RTESync_Spinlock                LVCSim_SchemaMeta::m_objLock;              // schema drop lock

// get pointer to schema with given ID

//#define SCHEMA(id)    schemaDir->find(id)
//#define CONTAINER(id) containerDir->find(id)
//
//#define GET_CONTAINER(cont, desc, sid)                     \
//  if ((desc).fileTfn_gg00() != tfnObj_egg00) {              \
//    *lpb_error = e_invalid_filetype;                  \
//    return S_OK;                            \
//  }                                   \
//    /* PTS 1124356 FF 2003-09-23 new FileDir */                             \
//  LVCSim_ContainerMeta *cont = CONTAINER((desc).fileDirFileId_gg00());  \
//  if (!cont ) {                              \
//    *lpb_error = e_OMS_file_not_found;  /* PTS 1124935 */                   \
//    return S_OK;                            \
//  }                                    \
//  if (cont->isLocked() && !cont->isLocked(sid)){        \
//    *lpb_error = e_OMS_request_timeout;                      \
//    return S_OK;                                  \
//  }                                         \
//  if (cont->isCleared()) {                              \
//    *lpb_error = e_OMS_file_not_found;            \
//    return S_OK;                            \
//  }                                    \
//
//
//#define GET_CONTAINER2(cont, desc, lpoid, sid)                 \
//  LVCSim_ContainerMeta *cont;                       \
//  if ((desc).fileObjFileType_gg00() == oftUnknownType_egg00) {      \
//    if (((OmsTypeOid *) lpoid)->pno == NIL_OBJ_PAGE_NO) {       \
//      /* not allowed - we must know either OID or container ID */   \
//      *lpb_error = e_OMS_file_not_found;                                     \
//      return S_OK;                          \
//    }                                 \
//    /* fill in the container ID according to OID */           \
//    cont = CONTAINER(reinterpret_cast<const LVCSim_OIDObject *>(lpoid)->getContainer());    \
//    if (!cont) {                            \
//      *lpb_error = e_OMS_file_not_found;                                     \
//      return S_OK;                          \
//    }                                                 \
//    if (cont->isLocked() && !cont->isLocked(sid)){        \
//      *lpb_error = e_OMS_request_timeout;                      \
//      return S_OK;                                  \
//    }                                         \
//    if (cont->isCleared()) {                              \
//      *lpb_error = e_OMS_file_not_found;            \
//      return S_OK;                            \
//    }                                    \
//                                 \
//    /* copy into desc */                        \
//    desc = cont->m_containerDesc;                   \
//  } else {                                \
//        /* PTS 1124356 FF 2003-09-23 new FileDir */                         \
//    cont = CONTAINER((desc).fileDirFileId_gg00());            \
//  }

#define CHECK_TID_READ(x) getReadConsistentView(x, lpb_error);              \
    if (*lpb_error != e_ok) return S_OK;

#define CHECK_TID_WRITE(x) getWriteConsistentView(x, lpb_error);            \
    if (*lpb_error != e_ok) return S_OK;                  \
    CHECK_TID_READ(x)

const GUID VAR_OBJ_GUID_GCO10 = { 0x8fb0963d, 0xe5aa, 0x11d1, { 0xa9, 0x5c, 0x0, 0xa0, 0xc9, 0x43, 0x7, 0x31 } };;



//
//class LVCSim_AllocatorBase : public OmsAllocatorInterface
//{
//public:
//    LVCSim_AllocatorBase() : m_omsCallback(NULL)
//    {
//    }
//
//    virtual void omsRegisterCallback(OmsCallbackInterface *cb)
//    {
//        m_omsCallback = cb;
//    }
//
//    virtual OmsCallbackInterface *omsGetCallback() const
//    {
//        return m_omsCallback;
//    }
//
//protected:
//    OmsCallbackInterface *m_omsCallback;
//};
//
//
//class LVCSim_GlobalAllocator : public LVCSim_AllocatorBase
//{
//public:
//    virtual void *omsAllocate(size_t size) // allocation
//    {
//        return omsMalloc(size);
//    }
//
//    virtual void omsDeallocate(void *p)    // deallocation
//    {
//        omsFree(p);
//    }
//
//    virtual bool omsCheckAllocatorEmpty()  // check for emptyness
//    {
//        return true;
//    }
//
//    virtual int omsGetAllocatorType() const
//    {
//      return 1;
//    }
//};
//
//class LVCSim_CheckedAllocator : public LVCSim_AllocatorBase, public SAPDBMem_RawAllocator {
//
//public:
//    LVCSim_CheckedAllocator(SAPDBMem_IRawAllocator &a) : 
//        SAPDBMem_RawAllocator((SAPDB_UTF8*) "LCSim Allocator", 
//        a, 65536, 65536, SAPDBMem_RawAllocator::NOT_FREE_RAW_EXTENDS),
//        m_first(NULL)
//    {
//    }
//
//    virtual void* Allocate(SAPDB_ULong ByteCount)
//    {
//        return omsMalloc(ByteCount);
//    }
//
//    virtual void* Allocate(SAPDB_ULong ByteCount, const void *Hint)
//    {
//        return omsMalloc(ByteCount);
//    }
//
//    virtual void Deallocate(void* p)
//    {
//        omsFree(p);
//    }
//    
//    virtual SAPDB_ULong GetChunkSize(const void* p)
//    {
//        const BlkHead *h = reinterpret_cast<const BlkHead*>(p) - 1;
//        return h->m_size;
//    }
//
//    virtual void *omsAllocate(size_t size) // allocation
//    {
//        return LLAllocate(size, 0);
//    }
//
//    virtual void omsDeallocate(void *p)    // deallocation
//    {
//        LLDeallocate(p, 0);
//    }
//
//    virtual bool omsCheckAllocatorEmpty()  // check for emptyness
//    {
//        return LLCheckEmpty(0);
//    }
//
//protected:
//    friend class LVCSim_CheckedAllocatorInst;
//    
//    class BlkHead {
//    public:
//        BlkHead *m_next, *m_prev;
//#ifndef BIT64
//        size_t  m_filler; // 4 Byte filler to ensure 8 Byte alignement on 32 Bit systems. PTS 1134414
//#endif
//        size_t  m_size;
//        char    m_data[1];
//
//#ifndef BIT64
//        enum {
//            // data offset on 32 Bit systems.  PTS 1134414
//            blkOfs = sizeof(BlkHead*) + sizeof(BlkHead*) + sizeof(size_t) + sizeof(size_t) 
//        };
//#else
//        enum {
//            blkOfs = sizeof(BlkHead*) + sizeof(BlkHead*) + sizeof(size_t)  // data offset
//        };
//#endif
//
//        size_t getSize() const
//        {
//            return m_size & ((1<<28) - 1);
//        }
//
//        size_t getCode() const
//        {
//            return m_size >> 28;
//        }
//
//        void setSize(size_t size, size_t code)
//        {
//            m_size = size | (code << 28);
//        }
//
//        void setCode(size_t newCode)
//        {
//            m_size = getSize() | (newCode << 28);
//        }
//    };
//      
//    void *LLAllocate(size_t size, unsigned int code)
//    {
//        BlkHead *h = (BlkHead*) omsMalloc(size + sizeof(BlkHead) - 1);
//        h->setSize(size, code);
//        h->m_prev = NULL;
//        {
//            RTESync_LockedScope lck(m_lock);
//            h->m_next = m_first;
//            if (m_first) {
//                m_first->m_prev = h;
//            }
//            m_first = h;
//        }
//        return h->m_data;
//    }
//
//    void LLDeallocate(void *p, unsigned int code)
//    {
//        BlkHead *h = reinterpret_cast<BlkHead*>(reinterpret_cast<char*>(p) - BlkHead::blkOfs);
//        if (h->getCode() != code) {
//            // error - freeing in other allocator
//      LVCSim_KernelInterface::DebugAllocatorErrorTrace("Block freed using invalid allocator", h);
//            h->setCode(0);
//            if (m_omsCallback) {
//                m_omsCallback->omsMemInvalidFree(h->m_data, h->getSize(), 
//                  (OmsCallbackInterface::BlockType) code, (OmsCallbackInterface::BlockType) h->getCode());
//            }
//            return;
//        }
//        {
//            RTESync_LockedScope lck(m_lock);
//            if (h->m_prev) {
//                h->m_prev->m_next = h->m_next;
//            } else {
//                m_first = h->m_next;
//            }
//            if (h->m_next) {
//                h->m_next->m_prev = h->m_prev;
//            }
//        }
//        omsFree(h);
//    }
//
//    size_t LLBlockSize(void *p)
//    {
//        BlkHead *h = reinterpret_cast<BlkHead*>(reinterpret_cast<char*>(p) - BlkHead::blkOfs);
//        return h->getSize();
//    }
//
//    bool LLCheckEmpty(int code)
//    {
//        bool empty = true;
//        RTESync_LockedScope lck(m_lock);
//        BlkHead *h = m_first;
//        while (h) {
//            if (h->getCode() >= (size_t) code) {
//                // report block
//        LVCSim_KernelInterface::DebugAllocatorErrorTrace("Block left over", h);
//                h->setCode(0);
//                if (m_omsCallback) {
//                    m_omsCallback->omsMemLeftOver(h->m_data, h->getSize(), (OmsCallbackInterface::BlockType) code);
//                }
//                empty = false;
//            }
//            h = h->m_next;
//        }
//        return empty;
//    }
//
//    virtual int omsGetAllocatorType() const
//    {
//      return 0; // invalid - never called
//    }
//private:
//    RTESync_Spinlock  m_lock;
//    BlkHead           *m_first;
//};
//
//class LVCSim_CheckedAllocatorInst : public LVCSim_AllocatorBase
//{
//public:
//    LVCSim_CheckedAllocatorInst(LVCSim_CheckedAllocator *parent, int code) :
//        m_pAllocator(parent), m_code(code), m_allocSize(0)
//    {
//    }
//
//    ~LVCSim_CheckedAllocatorInst()
//    {
//        m_pAllocator->LLCheckEmpty(m_code);
//    }
//
//    virtual void *omsAllocate(size_t size) // allocation
//    {
//        m_allocSize += size;
//        return m_pAllocator->LLAllocate(size, m_code);
//    }
//
//    virtual void omsDeallocate(void *p)    // deallocation
//    {
//        m_allocSize -= m_pAllocator->LLBlockSize(p);
//        m_pAllocator->LLDeallocate(p, m_code);
//    }
//
//    virtual bool omsCheckAllocatorEmpty()  // check for emptyness
//    {
//        if (m_allocSize != 0) {
//            m_allocSize = 0;
//            return m_pAllocator->LLCheckEmpty(m_code);
//        } else {
//            return true;
//        }
//    }
//
//    virtual void omsRegisterCallback(OmsCallbackInterface *cb)
//    {
//        m_pAllocator->omsRegisterCallback(cb);
//    }
//
//    virtual OmsCallbackInterface *omsGetCallback() const
//    {
//        return m_pAllocator->omsGetCallback();
//    }
//
//    virtual int omsGetAllocatorType() const
//    {
//      return m_code;
//    }
//private:
//    LVCSim_CheckedAllocator *m_pAllocator;
//    int                     m_code;
//    unsigned long           m_allocSize;
//};


/*===========================================================================*
 *  TX LIST METHODS                                *
 *===========================================================================*/

static int compareTID(const void *_a, const void *_b)
{
    LVCSim_liveCacheSink::ConsViewList *a = *((LVCSim_liveCacheSink::ConsViewList**) _a);
    LVCSim_liveCacheSink::ConsViewList *b = *((LVCSim_liveCacheSink::ConsViewList**) _b);
    if (a->m_tid < b->m_tid) return -1;
    if (a->m_tid > b->m_tid) return 1;
    return 0;
}

LVCSim_TXList::LVCSim_TXList(LVCSim_TID &ourTx) 
    : m_refCnt(1), m_otherTxCount(0)
{
    // we know we have TX lock
    if (ourTx == LVCSim_TID_NONE) {
        // we don't have a TX ID yet, get it
        ourTx = ++LVCSim_liveCacheSink::txCounter;
        ++LVCSim_liveCacheSink::txCounter;
    }
    m_ourTx = ourTx;

    // load all other TX IDs
    LVCSim_liveCacheSink *list = LVCSim_liveCacheSink::firstSession;
    while (list) {
        LVCSim_liveCacheSink::ConsViewList *item = list->m_commitTimeRef;
        LVCSim_TID tid = list->m_commitTime;
        list = list->m_next;
        if (tid > 0 && tid < ourTx) {
            m_otherTxList[m_otherTxCount++] = item;
            item->AddRef();
        }
    }
    
    // TODO: sort all tids in order to use binary search later
    //qsort(m_otherTxList, (size_t) m_otherTxCount, sizeof(LVCSim_liveCacheSink::ConsViewList *), compareTID);
}

LVCSim_TXList::~LVCSim_TXList()
{
    for (int i = 0; i < m_otherTxCount; ++i)
        m_otherTxList[i]->Release();
}

bool LVCSim_TXList::sees(LVCSim_TID otherTx) const
{
    if (otherTx == m_ourTx) {
        // we made the modification, we must see it
        return true;
    }
    if (otherTx > m_ourTx) {
        // a transaction that started after us made the modification, we don't see it
        return false;
    }
    // a transaction that started before us made the modification
    // TODO: implement binary search here
    for (int i = 0; i < m_otherTxCount; ++i) {
        if (m_otherTxList[i]->m_tid == otherTx) {
            // this transaction was running as we started, we don't see it
            return false;
        }
    }

    // the transaction was already commited as we started, we see it
    return true;
}

void LVCSim_TXList::AddRef()
{
    ++m_refCnt;
}

void LVCSim_TXList::Release(bool doLock)
{
  if (--m_refCnt == 0) {
    if (doLock) {
      RTESync_LockedScope lck(LVCSim_liveCacheSink::txCounterLock);
      delete this;
    } else {
      delete this;
    }
  }
}

/*===========================================================================*
 *  SINK METHODS                               *
 *===========================================================================*/

LVCSim_liveCacheSink::LVCSim_liveCacheSink(bool appendToList)
  : m_defaultContext(NULL), m_cancelByte(false), m_cRef(0), 
    m_readView(LVCSim_TID_NONE), m_oldReadView(LVCSim_TID_NONE), m_undoList(NULL), m_objectsUpdated(false), m_subTxList(NULL), 
      m_lockChain(NULL), m_commitTime(0), m_commitTimeRef(NULL), m_otherTXs(NULL), m_oldOtherTXs(NULL),
      m_activeVersion(NULL), m_next(NULL),
      m_procServerBuffer(NULL), m_procServerLocal(NULL), m_procServerRemote(NULL),
      m_procServerThread(NULL)//,
{
    if (appendToList) {
        RTESync_LockedScope lck(txCounterLock);
        m_next = firstSession;
        firstSession = this;
    }

    m_taskId = m_taskIdCnt;
    ++m_taskIdCnt;
}


LVCSim_liveCacheSink::~LVCSim_liveCacheSink()
{
  // LVCSim_Detach destroys the sink properly and rolls back the transaction,
  // but to make sure we don't hold any locks if someone forgets to call detach,
  // do rollback again
  SimRollback();

    RTESync_LockedScope lck(txCounterLock);
    LVCSim_liveCacheSink *last = NULL, *cur = firstSession;
    while (cur && cur != this) {
        last = cur;
        cur = cur->m_next;
    }
    if (cur) {
        if (last) last->m_next = cur->m_next;
        else firstSession = cur->m_next;
    }
    if (m_otherTXs) {
        m_otherTXs->Release(false);
        m_otherTXs = NULL;
    }
    if (m_procServerLocal) {
      // send terminate request
      LVC_ProcServerExecutor executor(this, m_procServerLocal);
      executor.executorTerminate();
      delete m_procServerLocal;
    }
    if (m_procServerRemote) {
      delete m_procServerRemote;
    }
    if (m_procServerBuffer) {
      delete m_procServerBuffer;
    }
    if (m_procServerThread) {
      LVCSim::ThreadJoin(m_procServerThread);
    }
}


LVCSim_liveCacheSink::UndoList *LVCSim_liveCacheSink::undoList()
{
    if (!m_undoList) {
        m_undoList = new UndoList;
    }
    return m_undoList;
}


/*!
 * This method initializes all internal state variables for the simulator.
 * It should be called as the first method in simulator testprogram main()
 * function.
 */
void LVCSim_liveCacheSink::init()
{
  regionLocks = new RTESync_Spinlock[MAX_REGION_CNT];

  // TODO: make sure constants for hashtables are good, eventually settable
  schemaLock = new RTESync_Spinlock;
  containerLock = new RTESync_Spinlock;
  schemaDir = new LVCSim_FastHash<SchemaName, int, LVCSim_SchemaMeta, false>;
    // PTS 1124356 FF 2003-09-23 new FileDir
  containerDir = new LVCSim_FastHash<LVCSim_ContainerID, OMS_ContainerHandle, LVCSim_ContainerMeta, false>;
    sequenceMap = new LVCSim_SequenceMap;
  consViewList = NULL;
  reset();
}

/*!
 * Resetting the simulator causes unregistering all containers and releasing
 * all "persistent" objects.
 *
 * \note It is important that no transaction is running (e.g., there is no
 *  OmsHandle instantiated) when calling this function. Otherwise, an access
 *  violation and data errors may result.
 */
void LVCSim_liveCacheSink::reset()
{
    LVCSim_FastHash<LVCSim_liveCacheSink::SchemaName, int,
      LVCSim_SchemaMeta, false>::Iterator schemaIter(schemaDir->begin());
    while (schemaIter.isValid()) {
        delete &(schemaIter.currentItem());
        ++schemaIter;
    }
    schemaDir->clear();
    schemaCount = OMS_DEFAULT_SCHEMA_HANDLE;

    // PTS 1124356 FF 2003-09-23 new FileDir
    LVCSim_FastHash<LVCSim_ContainerID, OMS_ContainerHandle, LVCSim_ContainerMeta, false>::Iterator
        iter = containerDir->begin();
    bool containerFound = false;
    while (iter.isValid()) {
        delete &(iter.currentItem());
        ++iter;
    }
    containerDir->clear();
    LVCSim_ContainerMeta::freeAllDropped();
  containerCount = 1;

    sequenceMap->clear();

    
  txCounter = 1;
  txOldest = LVCSim_TID_MAX;

  while(consViewList){
    if(!consViewList->m_isOmsVersion) {
      fprintf(stderr, "ERROR: Unclosed transaction found in TX list (ignored)\n");
      fprintf(stderr, " - %d\n", consViewList->m_tid);
    }
    if(consViewList->m_isOmsVersion) {
      fprintf(stderr, "ERROR: Nondropped Version found in TX list\n");
      fprintf(stderr, " - %d\n", consViewList->m_tid);
    }
    ConsViewList *del = consViewList;
    consViewList = consViewList->m_next;
    del->m_next = 0;
    delete del;
  }
  if( ActiveVersion::txMap.size())
    ActiveVersion::txMap.clear();

  const OmsTypeWyde name[4] = {'O', 'M', 'S', 0};
  tsp00_Int4 id;
  short err;
  CreateSchema(name, &id, &err);

  sinkPtrFree = -2;
}

/*!
 * Release all resources held by the simulator.
 */
void LVCSim_liveCacheSink::done()
{
  delete schemaDir;
  delete schemaLock;

  delete containerDir;
  delete containerLock;

    delete sequenceMap;

  delete[] regionLocks;
}

// PTS 1124356 FF 2003-09-23 new FileDir
LVCSim_ContainerMeta *LVCSim_liveCacheSink::getContainer(OMS_ContainerHandle id)
{
  return containerDir->find(id);  
}

LVCSim_SchemaMeta *LVCSim_liveCacheSink::getSchema(int id)
{
  return schemaDir->find(id);  
}

LVCSim_liveCacheSink **LVCSim_liveCacheSink::getSinkPtr(bool **cancelFlag)
{
  RTESync_LockedScope lck(sinkPtrLock);

  if (sinkPtrFree < 0) {
    if (sinkPtrFree == -1) {
      // no free sinks anymore
      LVCSIM_THROW("Sorry, no free sinks for handles without COM object. "
        "Use COM object instances instead of OmsHandle instances.");
    }
    // create new free array
    for (int i = 0; i < LVCSIM_MAX_SINK_PTR; ++i)
      *((int*) &sinkPtr[i]) = (i + 1);
    *((int*) &sinkPtr[LVCSIM_MAX_SINK_PTR - 1]) = -1;
    sinkPtrFree = 0;
  }
  int ptr = sinkPtrFree;
  sinkPtrFree = *((int*) (&sinkPtr[sinkPtrFree]));
  *cancelFlag = &sinkPtrC[ptr];
  sinkPtrC[ptr] = false;
  sinkPtr[ptr] = NULL;
  return &sinkPtr[ptr];
}

void LVCSim_liveCacheSink::returnSinkPtr(LVCSim_liveCacheSink **ptr)
{
  int idx = ptr - sinkPtr;
  if (idx >= 0 && idx < LVCSIM_MAX_SINK_PTR) {
    // returning to freelist
    RTESync_LockedScope lck(sinkPtrLock);
    *((int*) &sinkPtr[idx]) = sinkPtrFree;
    sinkPtrFree = idx;
  }
}

void LVCSim_liveCacheSink::unlockAll()
{
  LVCSim_LockChainItem *h = m_lockChain;
  while (h) {
    LVCSim_LockChainItem *p = h;
    h = h->m_next;
    p->unlock(this);
    delete p;
  }
  m_lockChain = NULL;
}



//
//    LVCSim_ContainerMeta *c = m_containerLockChain;
//    while (c) {
//        LVCSim_ContainerMeta *p = c;
//        c = c->m_lockChain;
//        p->m_lockChain = NULL;
//
//        if (p->isCleared()) {
//          // is already cleared, throw away   
//          p->m_history = NULL;
//          p->m_lock = NULL;
//          OMS_ContainerHandle handle = p->getContHandle();
//          if( containerDir->find(handle) == p)
//            containerDir->remove(handle);
//          LVCSim_ContainerMeta::addDropped(handle,m_commitTime,m_commitTime,p);
//        } else {
//            // unlock only
//            p->m_history = NULL;
//            p->m_lock = NULL;
//        }
//    }
//    m_containerLockChain = NULL;
//
//    LVCSim_SchemaMeta *s = m_schemaLockChain;
//    while (s) {
//        LVCSim_SchemaMeta *p = s;
//        s = s->m_lockChain;
//        p->m_lockChain = NULL;
//        if (p->isCleared()) {
//          // is already cleared, throw away   
//          p->m_history = NULL;
//          int id = p->m_id;
//          if( schemaDir->find(id) == p)
//            schemaDir->remove(id);
//          delete p;
//        } else {
//            // unlock only
//            p->m_history = NULL;
//            p->m_lock = NULL;
//        }
//    }
//    m_schemaLockChain = NULL;
//
//}

/*!
 * Process COMMIT of a transaction. First try to commit all subtransactions
 * by calling SubtransCommit() for each of them, then do commit processing
 * on undo file and throw undo file away. As the last step, release all the
 * locks.
 *
 * If a commit processing fails, do transaction rollback instead.
 */
void LVCSim_liveCacheSink::SimCommit()
{
  SIM_TRACE2(SIM_TX, "commit R: %d, W: %d", m_readView, m_commitTime);

  while (m_subTxList) {
    short err;
    SubtransCommit(&err);
    if (err != e_ok) {
      // TODO: this is seriously wrong
      SIM_TRACE(SIM_TX, "error at commit: %d", err);
      LVCSIM_ASSERT(false);
      SimRollback();
      throw DbpError(err);
    }
  }
  if (m_procServerRemote) {
    // we have procserver, call commit on the other side
    short err;
    LVC_ProcServerExecutor executor(this, m_procServerLocal);
    executor.executorTransEnd(err, true);
    if (err != e_ok) {
      // TODO: this shouldn't happen at all, except for ProcServer error
      SIM_TRACE(SIM_TX, "error at commit: %d", err);
      LVCSIM_ASSERT(false);
      SimRollback();
      throw DbpError(err);
    }
  }
  if (m_undoList) {
    m_undoList->commit(this);
    delete m_undoList;
    m_undoList = NULL;
    m_objectsUpdated = false;
  }
  unlockAll();
  if(m_oldReadView != LVCSim_TID_NONE){
      m_oldReadView = LVCSim_TID_NONE;
      m_oldOtherTXs->Release(true);
      m_oldOtherTXs = NULL;
  }
  if (m_commitTime > 0) {
    m_commitTime = 0;
    RTESync_LockedScope lck(txCounterLock);
    m_commitTimeRef->Release();
    m_commitTimeRef = NULL;
  }
}

/*!
 * Process ROLLBACK of the transaction. Rollback all open subtransactions and
 * then rollback all changes on current undo list. As the last step, unlock all
 * the locks.
 */
void LVCSim_liveCacheSink::SimRollback()
{
  SIM_TRACE2(SIM_TX, "rollback R: %d, W: %d", m_readView, m_commitTime);

  while (m_subTxList) {
    short err;
    SubtransRollback(&err);
    if (err != e_ok) {
      throw DbpError(err);
    }
  }
  if (m_undoList) {
    m_undoList->rollback(this);
    delete m_undoList;
    m_undoList = NULL;
    m_objectsUpdated = false;
  }
  if (m_procServerRemote) {
    // we have procserver, call rollback on the other side
    short err;
    LVC_ProcServerExecutor executor(this, m_procServerLocal);
    executor.executorTransEnd(err, false);
    if (err != e_ok) {
      // TODO: this shouldn't happen at all, except for ProcServer error
      SIM_TRACE(SIM_TX, "error at rollback: %d", err);
      LVCSIM_ASSERT(false);
    }
  }
  unlockAll();
  if (m_commitTime > 0) {
    m_commitTime = 0;
    RTESync_LockedScope lck(txCounterLock);
    m_commitTimeRef->Release();
    m_commitTimeRef = NULL;
  }
}

/*!
 * Start the routine in simulated ProcServer.
 * 
 * \param libname library name with COM object,
 * \param methodname name of the method to execute,
 * \param args variable arguments - parameters.
 *
 * \warning If the parameter count or type do not match the actual
 *  parameters for the COM routine, you will probably become a crash.
 */
void LVCSim_liveCacheSink::StartInUDEServer(const char *libname,
  const char *methodname, va_list &args)
{
  if (!m_procServerRemote) {
    // create (pseudo) communication interface
    m_procServerBuffer = new LVCSim_ProcServerBuffer;
    m_procServerLocal = new LVCSim_ProcServerInterface(
      LVCSim_KernelInterface::Instance()->getAllocator(),
      m_procServerBuffer->getChannel1());
    m_procServerRemote = new LVCSim_ProcServerInterface(
      LVCSim_KernelInterface::Instance()->getAllocator(),
      m_procServerBuffer->getChannel2());
    // create thread and sink for the ProcServer
    char errbuf[256];
    int err = sizeof(errbuf);
    // TODO: make stack size configurable (default 256KB)
    LVCSim_ProcServerThreadParam param(m_procServerRemote, &m_cancelByte);
    m_procServerThread = LVCSim::ThreadBegin(256*1024, LVCSim_ThreadCreateNormal,
      LVCSim_ProcServerInterface::thread, &param, &err, errbuf);
    if (!m_procServerThread) {
      delete m_procServerRemote;
      delete m_procServerLocal;
      delete m_procServerBuffer;
      m_procServerRemote = m_procServerLocal = NULL;
      m_procServerBuffer = NULL;
      // TODO: proper message
      throw DbpError(e_file_not_found, errbuf);  
    }
  }

  // load the library
  tsp00_C32 md5;
  HANDLE hInstance;
  HRESULT res;
  tsp00_C8 packageId;
  SAPDB_MemFillNoCheck(&packageId[0], 0, sizeof(packageId));
  res = LVC_LoadComponentLibrary(libname, packageId, &hInstance, md5);
  if (res != S_OK) {
    // couldn't find the library
    throw DbpError(e_file_not_found);
  }

  LVCPS_ParamHandler paramHandler(libname, md5);
  if (!paramHandler.isLoaded()) {
    // couldn't find object in library
    // TODO: proper message
    throw DbpError(e_file_not_found);
  }

  enum {
    MAX_PARAM = 128
  };
  tak_dbproc_stack_param_debug  paramStack[MAX_PARAM];
  unsigned short paramcnt;
  int dispid = 0;

  if (strcmp(methodname, "sysRequest") == 0) {
    // prepare parameters for system request
    paramcnt = 2;
    paramStack[0].dbg_value.ptr() = va_arg(args, tsp00_MoveObjPtr);
    paramStack[0].dbg_inout.becomes(sp1io_inout);
    paramStack[0].dbg_is_ptr = 1;
    paramStack[0].dbg_length = 4;
    paramStack[0].dbg_vt = 26;
    paramStack[1].dbg_value.ptr() = va_arg(args, tsp00_MoveObjPtr);
    paramStack[1].dbg_inout.becomes(sp1io_inout);
    paramStack[1].dbg_is_ptr = 1;
    paramStack[1].dbg_length = sizeof(LVC_DispatcherTransEndBufMsg);
    paramStack[1].dbg_vt = 26;
    dispid = -1;
  } else {
    if (!paramHandler.selectMethod(methodname)) {
      // couldn't find method in object
      // TODO: proper message
      throw DbpError(e_dbproc_proc_not_found);
    }

    paramcnt = paramHandler.getParamCnt();
    if (paramcnt > MAX_PARAM) {
      // too many parameters
      // TODO: proper message
      throw DbpError(e_dbproc_proc_not_found);
    }

    // create parameter stack
    tak_dbproc_stack_param_debug *pp = paramStack;
    for (unsigned short i = 0; i < paramcnt; ++i) {
      // copy one parameter, advance i for subparameters
      i += paramHandler.createParam(i, pp, args);
      ++pp;
    }
    dispid = paramHandler.getDispID();
    // compute real parameter count
    paramcnt = pp - paramStack;
  }

  // create local executor handling OMS calls on behalf of ProcServer object
  LVC_ProcServerExecutor executor(this, m_procServerLocal);

  // let the COM routine execute in the executor
  short err;
  tsp00_KnlIdentifier knlName;
  knlName.c2p(methodname);
  res = executor.executorExecute(paramcnt, paramStack, libname,
    md5, paramHandler.getCLSID(), paramHandler.getIID(), dispid,
    knlName, knlName, err);
  if (res != S_OK) {
    // error on communication
    // TODO: proper message
    throw DbpError(e_dbproc_comm_error);
  }
  if (err != 0) {
    // Uncaught DbpError in COM routine
    throw DbpError(err);
  }
}

void LVCSim_liveCacheSink::StartInUDEServer(const char *libname, const char *methodname, int dummy, ...)
{
  va_list args;
  va_start(args, dummy);
  StartInUDEServer(libname, methodname, args);
  va_end(args);
}

void LVCSim_liveCacheSink::CommitInProcServer(const char *libname)
{
  tsp00_Int4 buflen = sizeof(LVC_DispatcherTransEndBufMsg);
  union _aligned_buf
  {
    LVC_DispatcherTransEndBuf   l_buf;
    integer                     align;
  } aligned_buf;

  aligned_buf.l_buf.err = 0;
  aligned_buf.l_buf.msg.mtype.becomes(m_commit);
  aligned_buf.l_buf.msg.mtype2.becomes(mm_nil);
  StartInUDEServer(libname, "sysRequest", 0, &buflen, &aligned_buf.l_buf);
  if (aligned_buf.l_buf.err != 0) {
    throw DbpError(aligned_buf.l_buf.err);
  }
}

void LVCSim_liveCacheSink::RollbackInProcServer(const char *libname)
{
  tsp00_Int4 buflen = sizeof(LVC_DispatcherTransEndBufMsg);
  union _aligned_buf
  {
    LVC_DispatcherTransEndBuf   l_buf;
    integer                     align;
  } aligned_buf;

  aligned_buf.l_buf.err = 0;
  aligned_buf.l_buf.msg.mtype.becomes(m_rollback);
  aligned_buf.l_buf.msg.mtype2.becomes(mm_nil);
  StartInUDEServer(libname, "sysRequest", 0, &buflen, &aligned_buf.l_buf);
  if (aligned_buf.l_buf.err != 0) {
    throw DbpError(aligned_buf.l_buf.err);
  }
}



void LVCSim_liveCacheSink::readFromStream(LVCSim_CheckpointReader &i)
{
  i.read(&schemaCount, sizeof(schemaCount));
  i.read(&containerCount, sizeof(containerCount));
  i.read(&txCounter, sizeof(txCounter));

  schemaDir->readFromStream(i);
  containerDir->readFromStream(i);

  // TODO: save sequences
}

void LVCSim_liveCacheSink::writeToStream(LVCSim_CheckpointWriter &o)
{
  o.write(&schemaCount, sizeof(schemaCount));
  o.write(&containerCount, sizeof(containerCount));
  o.write(&txCounter, sizeof(txCounter));

  schemaDir->writeToStream(o);
  containerDir->writeToStream(o);

  // TODO: load sequences
}










HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::ABAPRead( 
    tgg01_StreamDescriptor& descriptor,
    short     &error)
{
    descriptor.sdCount                  = 1;
  LVCSim_StreamIOBase *io = LVCSim::GetStream(descriptor.sdDesc[0].osdStreamId);
  if (!io) {
    error = e_oms_istream_error;
    return S_OK;
  }
  if(descriptor.sdDesc[0].osdResetStream){
    io->resetStream(/*isInStream*/true);
  }

  descriptor.sdDesc[0].osdBufPtr      = (tgg00_VoidPtr) io->readRow();
  descriptor.sdDesc[0].osdBufSize     = (descriptor.sdDesc[0].osdBufPtr != NULL) ? ((OmsTypeInt4) io->getRowSize()) : 0;
  descriptor.sdDesc[0].osdRowCount    = (descriptor.sdDesc[0].osdBufPtr != NULL) ? 1 : 0;
    descriptor.sdDesc[0].osdEndOfStream = (NULL == descriptor.sdDesc[0].osdBufPtr);
  error = e_ok;
  return S_OK;
}


HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::ABAPWrite(
    tgg01_StreamDescriptor& descriptor,
  short  &error)
{
    descriptor.sdCount = 1;
    LVCSim_StreamIOBase *io = LVCSim::GetStream(descriptor.sdDesc[0].osdStreamId);
    if (!io) {
        error = e_oms_ostream_error;
        return S_OK;
    }
    if(descriptor.sdDesc[0].osdResetStream){
      io->resetStream(/*isInStream*/false);
    }

    SAPDB_Byte* pBuf = reinterpret_cast<SAPDB_Byte*>(descriptor.sdDesc[0].osdBufPtr);
    int rowCount     = descriptor.sdDesc[0].osdRowCount;
    descriptor.sdDesc[0].osdBufSize = 0;
    while (rowCount--) {
        io->writeRow(pBuf);
        pBuf += descriptor.sdDesc[0].osdRowSize;
        descriptor.sdDesc[0].osdBufSize += descriptor.sdDesc[0].osdRowSize;
    }

    error = e_ok;
    return S_OK;
}

    
HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::AllocPacket( 
    unsigned char  **p,
    tsp00_Int4      *sz)
{
  *p = new unsigned char[8192];
  *sz = 8192;
  return S_OK;
}


HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::CloseVersion( 
    short        *lpb_error)
{
    if (!m_activeVersion) {
        SIM_TRACE(SIM_VER, "No active version", 0)
        *lpb_error = e_ok;  //e_unknown_version;
        return S_OK;
    }
    SIM_TRACE2(SIM_VER, "close version %08x, view %d", 
        m_activeVersion, m_activeVersion->getReadView())

  // no m_versions in persistent state => only keep time consistent
  m_readView = LVCSim_TID_NONE;
    if (m_otherTXs) {
        m_otherTXs->Release(true);
        m_otherTXs = NULL;
    }
    LVCSIM_ASSERT(m_activeVersion->isInUse())
    m_activeVersion->unmarkInUse();
    m_activeVersion = NULL;
  *lpb_error = e_ok;
  return S_OK;
}
    

HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::ConsoleMsg( 
    tsp00_Int4      buflen,
    unsigned char  *lpv)
{
  // TODO: log into a file as well
  fwrite(lpv, buflen, 1, stdout);
  printf("\n");
  return S_OK;
}


HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::CreateVersion( 
    unsigned char  *lpConsistentView,
    unsigned char  *lpVersionContext,
    short          *lpb_error)
{
  EndConsistentView(lpConsistentView, lpb_error);
    if (*lpb_error != e_ok) {
        return S_OK;
    }

  SIM_TRACE2(SIM_TX, "createopen version: %d, open: %d", m_readView, createVersion)

  // if any updates performed, then cannot create version
  if (m_objectsUpdated) {
    *lpb_error = e_transaction_end_required;
    return S_OK;
  }

  // simply return OK, new view will be created later
  reinterpret_cast<tgg01_OmsVersionContext*>(lpVersionContext)->ovc_key = 0;
      m_activeVersion = new ActiveVersion();
  SIM_TRACE(SIM_VER, "Create version %08x", m_activeVersion)
  *lpb_error = e_ok;
  return S_OK;
}


HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::CreateKeyRangeIterator(
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
    SAPDB_UInt8               *pResultOId,  
    tgg91_PageRef            *pObjSeq,
    unsigned char            *pBody,
    tsp00_Int4               &LogReadAccesses,
    tgg00_BasisError         &e,
    OmsIterDirection          direction)
{
  short *lpb_error = &e;
  CHECK_TID_READ(&consistentView)
  LVCSim_ContainerMeta *cont = getContainer(objFile,lpb_error);
  if(*lpb_error != e_ok)
    return S_OK;

  if (!cont->m_tree) {
    // there is no m_tree
    e = e_invalid_obj_col_desc; // TODO: OK?
    return S_OK;
  }

  LVCSim_ObjectHandle *h = cont->m_tree->findKey(pStartKey, keyLen, this, true, direction);
  
  *pKeyRangeIterator = new(keyLen) KeyRangeIterator(pLowerKey, pUpperKey, keyLen, cont, h);
  KeyRangeIterator* it = (KeyRangeIterator*) *pKeyRangeIterator;

  tsp00_Int4 maxNoOfOid = noOfOid;
  noOfOid = 0;
  h = it->currObject();
  while (noOfOid < maxNoOfOid && h){
    int idx = (direction == OMS_ASCENDING ? noOfOid : (maxNoOfOid-(noOfOid+1)));

    if (h->isVisible(this)){ 
      h->getOid(this, (OMS_ObjectId8*)&(pResultOId[idx]));
      h->getRef(&(pObjSeq[idx]));
      ++noOfOid;

      if (pBody != NULL){
        if (cont->m_size != (unsigned) objBodySize || maxNoOfOid != 1){
          // ERROR
        }
        LVCSim_ObjectVerHandle *v = h->lookupObject(this, LogReadAccesses);
        // the node is visible, but can be in new state. in this case, do not copy
        if (v){
          SAPDB_MemCopyNoCheck(pBody, v->m_data.fix, objBodySize);
        }
      }

      if (direction == OMS_ASCENDING){
        h = it->nextObject(this);
      }
      else {
        h = it->prevObject(this);
      }
    }
  }

  if (direction == OMS_DESCENDING && noOfOid < maxNoOfOid){
    // If array is not filled completely, then shift the entries in the array 
    // to the left side
    int i;
    for (i=0; i<noOfOid; ++i){
      pResultOId[i] = pResultOId[i+maxNoOfOid-noOfOid];
    }
  }

  e = (h ? e_ok : e_no_next_object);
  return S_OK;
}

    
HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::DeleteAllObj(
    tgg91_TransNo&  consistentView,
    unsigned char  *lpContainerId,
    unsigned char  *lpVersionFile,
    int            &lpDeleted,
    short          &error,
    SAPDB_UInt8     &errorOid)
{
    short* lpb_error = &error;
  tgg00_FileId *cid = (tgg00_FileId *) lpContainerId;
  CHECK_TID_WRITE(&consistentView)
  LVCSim_ContainerMeta *cont = getContainer(*cid, lpb_error);
  if(*lpb_error != e_ok)
    return S_OK;

  SubtransStart(lpb_error);
  if (*lpb_error != e_ok) return S_OK;

  lpDeleted = 0;

  try {
    
    for (int lastOid = cont->m_alloc->getNextOid(0); lastOid; 
      lastOid = cont->m_alloc->getNextOid(lastOid)) {

      LVCSim_ObjectHandle *h = (LVCSim_ObjectHandle*) cont->m_alloc->map(lastOid);

      if(!h->isVisible(this)) // Object not visible, skip it
        continue;

      // if can delete, then delete
      *lpb_error = h->lockObject(this);
      if (*lpb_error == e_ok) {
        *lpb_error = h->remove(this);
                if (*lpb_error == e_ok) {
                    undoList()->addDelete(h);
                    m_objectsUpdated = true;
                }
      }

      // if not successful, return dirty
      if (*lpb_error != e_ok) {
        SubtransRollback(lpb_error);
                if (*lpb_error == e_ok) {
                    *lpb_error = e_object_dirty;
                }
        return S_OK;
      }
      ++lpDeleted;
    }

  } catch (const DbpError &e) {
    SubtransRollback(lpb_error);
    throw e;
  }

  SubtransCommit(lpb_error);
  return S_OK;
}


// PTS 1121449
HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::DeleteObjMass(
    tsp00_Int4               cnt,
    tgg91_TransNo           *pConsistentView,
    tgg01_ContainerId      **ppContainerId,
    unsigned char           *pVersionContext,
    SAPDB_UInt8              *pOid,
    tgg91_PageRef           *pObjSeq,
    bool                    *pReused,  // PTS 1127661
    bool                    *pContObj, // PTS 1124278
    bool                     clustered,
    tsp00_Int4              &errCnt,
    tgg00_BasisError        *pDBError)
{
  errCnt = 0;
  for (int i=0; i<cnt; ++i){
    DeleteObj(
      (unsigned char*) pConsistentView,
      (unsigned char*) ppContainerId[i],
      pVersionContext,
      (unsigned char*) &pOid[i],
      (unsigned char*) &pObjSeq[i],
      pReused[i],   
      pContObj[i],
      &pDBError[i]);
    if (pDBError[i] != e_ok){
      ++errCnt;
    }
  }
  return S_OK;
}

HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::DeleteObj( 
    unsigned char  *lpConsistentView,
    unsigned char  *lpContainerId,
    unsigned char  *lpVersionContext,
    unsigned char  *lpoid,
    unsigned char  *lpobjseq,
    bool            reused, // PTS 1127661
    bool            contObj,// PTS 1124278
    short          *lpb_error)
{
  CHECK_TID_WRITE(lpConsistentView)

  tgg00_FileId *cid = (tgg00_FileId *) lpContainerId;

  LVCSim_ContainerMeta *cont = getContainer( *cid, (OMS_ObjectId8*)lpoid, lpb_error);
  if(*lpb_error != e_ok)
    return S_OK;

  LVCSim_ObjectHandle *h = (LVCSim_ObjectHandle*) cont->m_alloc->
    map(reinterpret_cast<LVCSim_OIDObject*>(lpoid)->getCurrentOid());

  if (!h || (!reused && h->getGeneration() != ((OMS_ObjectId8*) lpoid)->getGeneration())) {
    *lpb_error = e_object_not_found;
    return S_OK;
  }

  if (reused && h->getGeneration() !=  ((OMS_ObjectId8*) lpoid)->getGeneration()){
    // Adapt generation if it has been changed.  // PTS 1125361
    h->setGeneration(((OMS_ObjectId8*) lpoid)->getGeneration());
  }

  *lpb_error = h->remove(this);
  if (*lpb_error == e_ok) {
      undoList()->addDelete(h);
      m_objectsUpdated = true;
  }
  return S_OK;
}


HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::DestroyKeyRangeIterator(
    void* pKeyRangeIterator)
{
  delete ((KeyRangeIterator*) pKeyRangeIterator);
  return S_OK;
}

HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::EnterCriticalSection(
    short regionId)
{
  // TODO: make better
  if (regionId >= 0 && regionId < MAX_REGION_CNT) {
    regionLocks[regionId].Lock();
  } else {
    LVCSIM_THROW("Trying to enter critical section with unknown region ID");
  }
  return S_OK;
}
 

// PTS 1120873
HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::GetContainerId(   
    tgg01_ContainerId              *pContainerId, 
    const tgg01_OmsVersionContext  *pVersionContext,
    tsp00_Int4                      noOfOid,
    const SAPDB_UInt8               *pOid,
    tgg00_BasisError               *pDBError)
{
  for (int i=0; i<noOfOid; ++i){
    LVCSim_ContainerMeta *cont;                       
    if ((pContainerId[i]).fileObjFileType_gg00() == oftUnknownType_egg00) {     
      if (((OMS_ObjectId8 *)&pOid[i])->getPno() == NIL_OBJ_PAGE_NO) {       
        /* not allowed - we must know either OID or container ID */ 
        pDBError[i] = e_too_old_oid;                                    
      }                                 
      /* fill in the container ID according to OID */           
      cont = getContainer(reinterpret_cast<const LVCSim_OIDObject *>(&pOid[i])->getContainer());   
      if (!cont) {                            
        pDBError[i] = e_too_old_oid;                                     
      }
      /* copy into desc */                        
      pContainerId[i] = cont->m_containerDesc;                    
      } else {                                
          /* PTS 1124356 FF 2003-09-23 new FileDir */                         
          cont = containerDir->find((pContainerId[i]).fileDirFileId_gg00());           
    }
  }
  return S_OK;
}

  
//HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::GetErrorMsg( 
//    short          errorno,
//    tsp00_Int4    *lpbuflen,
//    unsigned char *lpv)
//{
//  char buf[16];
//  sprintf(buf, "Error #%d", errno);
//
//  size_t len = strlen(buf);
//  if (len >= (size_t) *lpbuflen) len = *lpbuflen - 1;
//
//  strncpy((char*) lpv, buf, len);
//  lpv[len] = 0;
//  *lpbuflen = (tsp00_Int4) len;
//
//  return S_OK;
//}


HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::GetObj( 
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
    tgg00_BasisError              &e)
{
  if (m_cancelByte){
    e = e_OMS_cancelled;
    return S_OK;
  }

  short *lpb_error = &e;
  CHECK_TID_READ(&consistentView)
  LVCSim_ContainerMeta *cont = getContainer(containerId, (OMS_ObjectId8*)&oid, lpb_error);
  if(*lpb_error != e_ok)
    return S_OK;

  LVCSim_ObjectHandle *h = (LVCSim_ObjectHandle*) cont->m_alloc->
    map(reinterpret_cast<const LVCSim_OIDObject*>(&oid)->getCurrentOid());
  if (!h || !h->isVisible(this)) {
    e = e_object_not_found;
    return S_OK;
  }
  h->getRef(&objSeq);

  int hist = 0;
  LVCSim_ObjectVerHandle *v = h->lookupObject(this, hist);  //PTS
  if(v == NULL) {
    if ( h->getGeneration() != ((OMS_ObjectId8*)&oid)->getGeneration() )
      e = e_object_not_found;
    else {
      updTransId.gg90SetZero();
      updTransId.becomes(h->m_tid_min);

      //e = e_wrong_object_state;
      e = e_ok;
    }
    return S_OK;
  }

  if (v->getGeneration() != ((OMS_ObjectId8*) &oid)->getGeneration()) {
    e = e_object_not_found;
    return S_OK;
  }

	if (doLock) {
    if (!shared){
		  LockObj((unsigned char *) &consistentView, (unsigned char*) &containerId,
			  (SAPDB_UInt8*) &oid, (unsigned char*) &objSeq, (short*) &e);
    }
    else {
		  LockSharedObj((unsigned char *) &consistentView, (unsigned char*) &containerId,
			  (SAPDB_UInt8*) &oid, (unsigned char*) &objSeq, (short*) &e);
    }
		if (e != e_ok) return S_OK;
	}

  if (cont->m_size == 0) {
    SAPDB_MemCopyNoCheck(pBody, v->m_data.var.data, v->m_data.var.size >= (unsigned) bodySize ?
      bodySize : v->m_data.var.size);
    bodyLen = (tsp00_Int4) v->m_data.var.size;
  } else {
    SAPDB_MemCopyNoCheck(pBody, v->m_data.fix, cont->m_size >= (unsigned) bodySize ? bodySize : cont->m_size);
    bodyLen = (tsp00_Int4) cont->m_size;
  }
  e = e_ok;
  updTransId.gg90SetZero();
  updTransId.becomes(v->m_tid_min);
  histLogCount = (tsp00_Int4) hist;
  return S_OK;
}


HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::GetObjects( 
    tgg91_TransNo                 &consistentView,
    tgg01_ContainerId             *pContainerId, 
    const tgg01_OmsVersionContext *pVersionContext,
    tsp00_Int4                     noOfOid,
    const SAPDB_UInt8              *pOid,
    tgg91_PageRef                 *pObjVers,
    tgg91_TransNo                 *pUpdTransId,
    size_t                        *pBodySize,
    bool                           doLock,
    bool                           shared,
    unsigned char                **ppObj,
    tsp00_Int4                    &histLogCount,
    tsp00_Int4                    &errorCount,
    tgg00_BasisError              *e)
{
  tsp00_Int4 cnt      = 0;
  histLogCount        = 0;
  size_t dummyLen     = 0;
  errorCount          = 0;
  for (int i=0; i<noOfOid; ++i){
    GetObj (consistentView, 
            pContainerId[i],
            pVersionContext,
            pOid[i],
            doLock,
            shared,
            pObjVers[i],
            pUpdTransId[i],
            pBodySize[i],
            ppObj[i],
            dummyLen,
            cnt,
            e[i]);
    if (e[i] != e_ok){
      ++errorCount;
    }
    histLogCount += cnt;
  }

  return S_OK;
}

// PTS 1122194
HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::GetObjectsWithKey( 
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
    SAPDB_UInt8               *pOid,
    tgg91_PageRef            *pObjSeq,
    tgg91_TransNo            *pUpdTransId,
    tsp00_Int4               &histLogCnt,
    tsp00_Int4               &errorCnt,
    tgg00_BasisError         *pDBError)
{
  tsp00_Int4 cnt = 0;   
  histLogCnt     = 0;
  errorCnt       = 0;
  for (int i=0; i<noOfObj; ++i){
    GetObjWithKey ( 
      (unsigned char*) pConsistentView,
      (unsigned char*) &pContainerId[i],
      (unsigned char*) pVersionContext,
      pKeyLen[i],
      ppKey[i],
      doLock,
      shared,
      pBodySize[i],
      ppBody[i],
      &pOid[i],
      (unsigned char*) &pObjSeq[i],
      &pUpdTransId[i],
      &cnt,
      &pDBError[i]);
    if (pDBError[i] != e_ok){
      ++errorCnt;
    }
    histLogCnt += cnt;
  }
  return S_OK;
}


HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::GetObjWithKey( 
		unsigned char  *lpConsistentView,
		unsigned char  *lptree,
		unsigned char  *lpVersionContext,
		size_t          keylen,
		unsigned char  *lpkey,
		bool            doLock,
    bool            shared,
		size_t          bodysize,
		unsigned char  *lpbody,
		SAPDB_UInt8	   *lpoid,
		unsigned char  *lpobjseq,
    tgg91_TransNo  *pUpdTransId,
		int            *hist_log_count,
		short          *lpb_error)
{
  CHECK_TID_READ(lpConsistentView)

  tgg00_FileId *cid = (tgg00_FileId *) lptree;

  LVCSim_ContainerMeta *cont = getContainer(*cid, lpb_error);
  if(*lpb_error != e_ok)
    return S_OK;

  LVCSim_ObjectHandle *h = cont->m_tree->findKey(lpkey, keylen, this, false);
  if (!h) {
    *lpb_error = e_hash_key_not_found; //e_object_not_found;
    return S_OK;
  }

    h->getRef((tgg91_PageRef *) lpobjseq);
	if (doLock) {
    if (!shared){
		  LockObj(lpConsistentView, lptree,
			  reinterpret_cast<SAPDB_UInt8*>(h), lpobjseq, lpb_error);
    }
    else {
		  LockSharedObj(lpConsistentView, lptree,
			  reinterpret_cast<SAPDB_UInt8*>(h), lpobjseq, lpb_error);
    }
		if (*lpb_error != e_ok) return S_OK;
	}

  // the node is visible, but can be in new state. in this case, do not copy
  int hist = 0;
  LVCSim_ObjectVerHandle *v = h->lookupObject(this, hist);
  if (v) {
    SAPDB_MemCopyNoCheck(lpbody, v->m_data.fix, cont->m_size >= (unsigned) bodysize ? bodysize : cont->m_size);
    //bodyLen = cont->m_size;
  }
  *lpb_error = e_ok;
  pUpdTransId->gg90SetZero();
  if (v) {
    pUpdTransId->becomes(v->m_tid_min);
  }
  else {
    pUpdTransId->becomes(h->m_tid_min);
  }
  h->getOid(this,(OMS_ObjectId8*)lpoid);
  *hist_log_count = hist;
  return S_OK;
}


HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::HResultError( 
  HRESULT h_error)
{
  // TODO: what should we do here?
  fprintf(stderr, "HResultError called: %d\n", h_error);
  return S_OK;
}


HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::IsLocked( 
    unsigned char  *lpContainerId,
    SAPDB_UInt8     *lpoid,
    bool            shared,
    short          *lpb_error)
{
  tgg00_FileId *cid = (tgg00_FileId *) lpContainerId;
  LVCSim_ContainerMeta *cont = getContainer(*cid, (OMS_ObjectId8*)lpoid, lpb_error);
  if(*lpb_error != e_ok)
    return S_OK;


  LVCSim_ObjectHandle *h = (LVCSim_ObjectHandle*) cont->m_alloc->
    map(reinterpret_cast<LVCSim_OIDObject*>(lpoid)->getCurrentOid());
  if (!h || h->getGeneration() != ((OMS_ObjectId8*) lpoid)->getGeneration()) {
    *lpb_error = e_object_not_found;
    return S_OK;
  }

  *lpb_error = h->isLocked(this,shared);

  return S_OK;
}
    

HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::LeaveCriticalSection(
  short regionId)
{
  // TODO: make better
  if (regionId >= 0 && regionId < MAX_REGION_CNT) {
    regionLocks[regionId].Unlock();
  } else {
    LVCSIM_THROW("Trying to leave critical section with unknown region ID");
  }
  return S_OK;
}

// PTS 1121449
HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::LockObjMass(
    tsp00_Int4              &cnt,
    tgg91_TransNo           *pConsistentView,
    tgg01_ContainerId      **ppContainerId,
    SAPDB_UInt8              *pOid,
    tgg91_PageRef           *pObjSeq,
    tsp00_Int8              &timeout,
    tsp00_Int4              &errCnt,
    tgg00_BasisError        *pDBError)
{
  errCnt = 0;
  for (int i=0; i<cnt; ++i){
    LockObj(
      (unsigned char*) pConsistentView,
      (unsigned char*) ppContainerId[i],
      &pOid[i],
      (unsigned char*) &pObjSeq[i],
      &pDBError[i]);
    if (pDBError[i] != e_ok){
      ++errCnt;
    }

    if (timeout > 0 && pDBError[i] == e_OMS_request_timeout){   // PTS 1124935
      timeout = -1;
      cnt     = i+1;
      break;
    }
  }
  return S_OK;
}

HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::LockObj( 
  unsigned char  *lpConsistentView,
  unsigned char  *lpContainerId,
  SAPDB_UInt8     *lpoid,
  unsigned char  *lpobjseq,
  short          *lpb_error)
{
  CHECK_TID_WRITE(lpConsistentView)

  tgg00_FileId *cid = (tgg00_FileId *) lpContainerId;
  LVCSim_ContainerMeta *cont = getContainer(*cid, (OMS_ObjectId8*)lpoid, lpb_error);
  if(*lpb_error != e_ok)
    return S_OK;

  LVCSim_ObjectHandle *h = (LVCSim_ObjectHandle*) cont->m_alloc->
    map(reinterpret_cast<LVCSim_OIDObject*>(lpoid)->getCurrentOid());
  if (!h || h->getGeneration() != ((OMS_ObjectId8*) lpoid)->getGeneration()) {
    // return object dirty_instead of object_not_found, even if object does not exist or is not visible!
    *lpb_error = e_object_dirty; 
    return S_OK;
  }

  *lpb_error = h->lockObject(this, 0, reinterpret_cast<const tgg91_PageRef*>(
      lpobjseq)->gg91IsDummyRef());
  return S_OK;
}

 
extern void co90SysRequest (void* oms_handle, void* sql_handle,
                            int* lpbuflen, unsigned char* lpv);

HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::MultiPurpose( 
    unsigned char   MessType,
    unsigned char   MessType2,
    tsp00_Int4     *lpbuflen,
    unsigned char  *lpv,
    short          *lpb_error)
{
  switch (MessType)
  {
  case m_show:
    printf("%.*s\n", *lpbuflen, lpv);
    *lpb_error = e_ok;
    return S_OK;
  case m_change:
    printf("ERROR: Kernel-mapping for error is called!!");
    *lpb_error = e_OMS_not_implemented;
    *lpbuflen = 0;
    return S_OK;
  case m_set:
    switch (MessType2) 
    {
    case mm_lock_timeout:
      {
        static bool msg_printed = false;
        static const char *msg = "WARNING: Timeouts are not supported in simulator!\n";
        if (!msg_printed) {
          msg_printed = true;
          ConsoleMsg(strlen(msg), (unsigned char*) msg);
        }
        *lpb_error = e_ok;
      }
      return S_OK;;
    }
  case m_commit:
    // commit
    {
      short err = reinterpret_cast<OMS_Session*>(m_defaultContext)->
        m_handleList.front()->omsCommitHandling();
      if (err != 0) {
        SimRollback();
        lpv[0] = m_afterRollback;
      }
      else {
        SimCommit();
        lpv[0] = m_afterCommit;
      }
      *lpbuflen = 1;
      // TODO: Session Release (see OMS_COMMonitor)
      co90SysRequest (reinterpret_cast<OMS_Session*>(m_defaultContext)->m_handleList.front(), 
                      NULL, lpbuflen, lpv);
    }
    return S_OK;
  case m_rollback:
    // rollback
    {
      reinterpret_cast<OMS_Session*>(m_defaultContext)->
        m_handleList.front()->omsRollbackHandling(false, false);
      SimRollback();
      lpv[0] = m_afterRollback;
      *lpbuflen = 1;
      // TODO: Session Release (see OMS_COMMonitor)
      co90SysRequest(reinterpret_cast<OMS_Session*>(m_defaultContext)->m_handleList.front(), 
                     NULL, lpbuflen, lpv);
      return S_OK;
    }
  case m_describe:
    // dump
    omsSimDump(*(reinterpret_cast<OMS_Session*>(m_defaultContext)->
      m_handleList.front()), (MessType2 == 2) ? -1 : MessType2);
    return S_OK;
  case m_verify:
    if (MessType2 == mm_object) {
      // error message - commited modified, not stored object
      struct _DATA {
        OMS_ObjectId8 oid;
        const char  *name;

        // needed to make compiler happy about constructors
        _DATA(OMS_ObjectId8 &o, const char *n) : oid(o), name(n) {}
      } *data;
      data = reinterpret_cast<_DATA*>(lpv);

      if (LVCSim::m_callback) {
        LVCSim::m_callback->simNotStored(data->oid, data->name);
      } else {
        LVCSim_CallbackInterface::simBreak();
      }
      *lpb_error = e_ok;
      return S_OK;
    }
    break;
  case m_diagnose:
    return S_OK;
    break;
  }

  // TODO
  *lpb_error = e_OMS_not_implemented;   // PTS 1124935
  LVCSIM_ASSERT(false);
  return S_OK;
}


HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::NewConsistentView( 
        bool            isOmsVersion,             /* PTS 1130354 */
    unsigned char  *lpConsistentView,
    short          *lpb_error)
{
  // TODO: this is probably not quite OK, we can theoretically
  // close more than one view here - check m_versions

  //CHECK_TID(lpConsistentView)
  LVCSim_TID *tid = (LVCSim_TID*) lpConsistentView;
  if(*tid != LVCSim_TID_NONE && *tid != m_readView){
      CHECK_TID_READ(lpConsistentView);
  }
  //LVCSIM_ASSERT((*tid == m_readView))
  if (*tid != LVCSim_TID_NONE) {
    // end old consistent view
    EndConsistentView(lpConsistentView, lpb_error);
    if (*lpb_error != e_ok) return S_OK;
  }
  if (m_readView != LVCSim_TID_NONE) {
    // end old consistent view
    *tid = m_readView;
    ActiveVersion *version = ActiveVersion::find(m_readView);
    if(version == NULL){
        EndConsistentView(lpConsistentView, lpb_error);
        if (*lpb_error != e_ok) return S_OK;
    }
    else{
        CloseVersion(lpb_error);   
        m_readView = LVCSim_TID_NONE;
    }  
  }

  // generate new consistent view ID
    if (m_otherTXs) {
        m_otherTXs->Release(true);
        m_otherTXs = NULL;
    }

    {
        ConsViewList *pview = new ConsViewList;
        m_otherTXs = new LVCSim_TXList;

        RTESync_LockedScope lck(txCounterLock);
        new(m_otherTXs) LVCSim_TXList(m_readView);
        *tid = m_readView;

      // set 2 high bytes to 0
      *((tsp00_Int2*) (tid + 1)) = 0;

      // enter consistent view into TID list
    consViewList = new(pview) ConsViewList(m_readView, isOmsVersion, consViewList);
        if (m_commitTime <= 0) {
            m_commitTime = m_readView;
            m_commitTimeRef = pview;
            pview->AddRef();
        }
        if (txOldest == LVCSim_TID_MAX) {
            txOldest = m_readView;
        }
    }

    if (m_activeVersion) {
        SIM_TRACE(SIM_VER | SIM_TX, "new view version %08x", m_activeVersion)
        m_activeVersion->openView(m_otherTXs, m_readView);
    }


  SIM_TRACE(SIM_TX, "new read view: %d", m_readView)
#ifdef SIM_DO_TRACE
    RTESync_LockedScope rlock(txCounterLock);
    const ConsViewList *v = consViewList;
    while (v) {
        SIM_TRACE(SIM_TX, " - %d", v->m_tid);
        v = v->m_next;
    }
    SIM_TRACE(SIM_TX, "oldest read view: %d", txOldest);
#endif

  *lpb_error = e_ok;
  return S_OK;
}

    
HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::EndConsistentView( 
  unsigned char  *lpConsistentView,
  short          *lpb_error)
{
  // TODO: this is probably not quite OK, we can theoretically
  // close more than one view here - check m_versions

  //CHECK_TID(lpConsistentView)

  // remove consistent view from table and update txOldest
  LVCSim_TID *tid = (LVCSim_TID*) lpConsistentView;
  if (*tid == -1) {
    *lpb_error = e_ok;
    return S_OK;
  }
  if(*tid != LVCSim_TID_NONE && *tid != m_readView){
    CHECK_TID_READ(lpConsistentView);
  }
  SIM_TRACE(SIM_TX, "end cons view: %d", *tid);
  {
    RTESync_LockedScope lck(txCounterLock);
        ConsViewList::Release(*tid);
  }
  if(*tid == m_oldReadView){
    m_oldReadView = LVCSim_TID_NONE;
    m_oldOtherTXs->Release(true);
    m_oldOtherTXs = NULL;
  }

  if (*tid == m_readView) {
      m_readView = LVCSim_TID_NONE;
      if (m_otherTXs) {
          m_otherTXs->Release(true);
          m_otherTXs = NULL;
      }
      if (m_activeVersion) {
          SIM_TRACE(SIM_VER | SIM_TX, "end view version %08x", m_activeVersion)
          m_activeVersion->closeView();
          m_activeVersion = NULL;
      }
  }
  *tid = -1;
  // set 2 high bytes to -1
  *((tsp00_Int2*) (tid + 1)) = -1;

#ifdef SIM_DO_TRACE
    RTESync_LockedScope rlock(txCounterLock);
    const ConsViewList *v = consViewList;
    while (v) {
        SIM_TRACE(SIM_TX, " - %d", v->m_tid);
        v = v->m_next;
    }
    SIM_TRACE(SIM_TX, "oldest read view: %d", txOldest);
#endif
  *lpb_error = e_ok;
  return S_OK;
}


HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::NewObjMass( 
    tsp00_Int4               cnt,
    tgg91_TransNo           *pConsistentView,
    tgg01_ContainerId      **ppContainerId,
    unsigned char           *pVersionContext,
    size_t                  *pKeyPos,
    size_t                  *pKeyColSize,
    unsigned char          **ppKeyColumn, 
    bool                    *pContObj,    // PTS 1124278
    SAPDB_UInt8              *pOid,
    tgg91_PageRef           *pObjSeq,
    tgg91_TransNo           *pUpdTransId,
    tsp00_Int4              &errCnt,
    tgg00_BasisError        *pDBError)
{
  errCnt = 0;
  for (int i=0; i<cnt; ++i){
    NewObj ((unsigned char*) pConsistentView,
            (unsigned char*) ppContainerId[i],
            (unsigned char*) pVersionContext,
            pKeyPos     == NULL ? 0    : pKeyPos[i],
            pKeyColSize == NULL ? 0    : pKeyColSize[i],
            ppKeyColumn == NULL ? NULL : ppKeyColumn[i],
            pContObj[i],
            &pOid[i],
            (unsigned char*) &pObjSeq[i],
            &pUpdTransId[i],
            &pDBError[i]);
    if (pDBError[i] != e_ok){
      ++errCnt;
    }
  }
  return S_OK;
}


HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::NewObj( 
    unsigned char  *lpConsistentView,
    unsigned char  *lpContainerId,
    unsigned char  *lpVersionContext,
    size_t          KeyPos,
    size_t          KeyColSize,
    unsigned char  *KeyColumn,
    bool            contObj,    // PTS 1124278
    SAPDB_UInt8     *lpoid,
    unsigned char  *lpobjseq,
    tgg91_TransNo  *pUpdTransId,
    short          *lpb_error)
{
  CHECK_TID_WRITE(lpConsistentView)

  tgg00_FileId *cid = (tgg00_FileId *) lpContainerId;
  LVCSim_ContainerMeta *cont = getContainer(*cid, lpb_error);
  if(*lpb_error != e_ok)
    return S_OK;

  LVCSim_TID tid = *((LVCSim_TID*) lpConsistentView);

  // add a new object into container
  // contObj is not passed as parameter to the constructor, as this
  // property is only bound to a ObjectVersionHandle, and such a 
  // VersionHandle is only created during an update.
  LVCSim_ObjectHandle *oh = new(cont) LVCSim_ObjectHandle((OMS_ObjectId8*)lpoid,
    (tgg91_PageRef*) lpobjseq, this);

  if (cont->m_tree) {
    // add key to cont->m_tree
    // TODO: do we need KeyPos at all? probably not...
    //CHECK_TID_READ(lpConsistentView)

    *lpb_error = cont->m_tree->addKey(KeyColumn, KeyColSize, oh, this);
    if (*lpb_error != e_ok){
      //oh->destroy(cont, this);
      oh->clear();
    }
  }
  
  pUpdTransId->gg90SetZero();
  pUpdTransId->becomes(oh->m_tid_min);

  undoList()->addInsert(oh);
  m_objectsUpdated = true;

  return S_OK;
}

// PTS 1120478
HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::NextOids(
  tgg91_TransNo           &ConsistentView,
  tgg00_FileId            &FileId,
  tgg01_OmsVersionContext *pVersionContext,
  bool                     bOnlyHdrVarObjects,
  SAPDB_UInt8               CurrOid,
  tsp00_Int4              &noOfOid,
  SAPDB_UInt8              *pOid,
  tgg91_PageRef           *pObjVers,
  tsp00_Int4              &LogReadAccesses,
  tgg00_BasisError        *lpb_error)
{
  LVCSim_ContainerMeta *cont = getContainer(FileId, lpb_error);
  if(*lpb_error != e_ok)
    return S_OK;

  LogReadAccesses = 0;

  unsigned int lastOid;
  if (((OMS_ObjectId8&)CurrOid).getPno() == NIL_OBJ_PAGE_NO) {
    lastOid = 0;
  } else {
    lastOid = reinterpret_cast<LVCSim_OIDObject*>(&CurrOid)->getCurrentOid();
  }

  CHECK_TID_READ(&ConsistentView)

  int maxNoOfOid = noOfOid;
  noOfOid = 0;
  while (noOfOid < maxNoOfOid) {
    // pick the next OID from OID allocator
    lastOid = cont->m_alloc->getNextOid(lastOid);
    if (!lastOid) { 
      // All oids are read
      *lpb_error = e_no_next_object;
      return S_OK;
    }

    LVCSim_ObjectHandle *h = (LVCSim_ObjectHandle*) cont->m_alloc->map(lastOid);
    if (h->isVisible(this) && (!bOnlyHdrVarObjects || !h->isContObj(this))) {
      h->getOid(this,(OMS_ObjectId8*)&pOid[noOfOid]);
      h->getRef(&pObjVers[noOfOid]);
      ++noOfOid;
    }
  }

  *lpb_error = e_ok;
  return S_OK;
}

    
HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::NextObjFromKeyIterator (
  const tgg91_TransNo     &ConsistentView,  // PTS 1127520 
  tgg00_FileId&            objFile,
  tgg01_OmsVersionContext *lpVersionContext,
  void                    *pKeyIterator,  
  tsp00_Int4               objBodySize,
  size_t                   KeyLen,
  void                    *pRestartKey,
  tsp00_Int4              &noOfOid,
  SAPDB_UInt8              *pResultOId, 
  tgg91_PageRef           *pObjVers,
  unsigned char           *pBody,
  tsp00_Int4              &objHistReadCount,
  tgg00_BasisError        &e)
{
  short *lpb_error = &e;
  LVCSim_ContainerMeta *cont = getContainer(objFile, lpb_error);
  if(*lpb_error != e_ok)
    return S_OK;

  if (!cont->m_tree) {
    // there is no m_tree
    e = e_invalid_obj_col_desc; // TODO: OK?
    return S_OK;
  }

  KeyRangeIterator *it = (KeyRangeIterator*) pKeyIterator;
  LVCSim_ObjectHandle *h = it->findKey(pRestartKey, OMS_ASCENDING, this, false);

  tsp00_Int4 maxNoOfOid = noOfOid;
  noOfOid = 0;
  while (noOfOid < maxNoOfOid && h){
    if (h->isVisible(this)){
      h->getOid(this,(OMS_ObjectId8*)&pResultOId[noOfOid]);
      h->getRef(&pObjVers[noOfOid]);
      ++noOfOid;
      if (pBody != NULL){
        if (cont->m_size != (unsigned) objBodySize || maxNoOfOid != 1){
          // ERROR
        }
        LVCSim_ObjectVerHandle *v = h->lookupObject(this, objHistReadCount);
        // the node is visible, but can be in new state. in this case, do not copy
        if (v){
          SAPDB_MemCopyNoCheck(pBody, v->m_data.fix, objBodySize);
        }
      }
    }

    h = it->nextObject(this);
  }

  e = (h ? e_ok : e_no_next_object);
  return S_OK;
}


HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::VersionNextKey(
  tgg01_OmsVersionContext   *VersionContext,
  const tgg01_ContainerId   &ObjFile,
  void                      *pKey,
  size_t                     KeyLen,
  bool                      Inclusive,
  const void                *pStopKey,
  SAPDB_UInt8                &Oid,
  tgg00_BasisError          &e)
{
  // TODO: not needed, all m_versions are in memory
  e = e_OMS_not_implemented;   // PTS 1124935
    LVCSIM_ASSERT(false)
  return S_OK;
}


// PTS 1119480
HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::PrevObjFromKeyIterator ( 
  const tgg91_TransNo     &ConsistentView,  // PTS 1127520 
  tgg00_FileId&            objFile,
  tgg01_OmsVersionContext *lpVersionContext,
  void                    *pKeyIterator,   
  tsp00_Int4               objBodySize,
  size_t                   KeyLen,
  void                    *pRestartKey,
  tsp00_Int4              &noOfOid,
  SAPDB_UInt8              *pResultOId, 
  tgg91_PageRef           *pObjVers,
  unsigned char           *pBody,
  tsp00_Int4              &objHistReadCount,
  tgg00_BasisError        &e)
{
  short *lpb_error = &e;
  LVCSim_ContainerMeta *cont = getContainer(objFile, lpb_error);
  if(*lpb_error != e_ok)
    return S_OK;

  if (!cont->m_tree) {
    // there is no m_tree
    e = e_invalid_obj_col_desc; // TODO: OK?
    return S_OK;
  }

  KeyRangeIterator *it = (KeyRangeIterator*) pKeyIterator;
  LVCSim_ObjectHandle *h = it->findKey(pRestartKey, OMS_DESCENDING, this, false);

  tsp00_Int4 maxNoOfOid = noOfOid;
  noOfOid = 0;
  while (noOfOid < maxNoOfOid && h){
    if (h->isVisible(this)){
      h->getOid(this,(OMS_ObjectId8*)&(pResultOId[maxNoOfOid-(noOfOid+1)]));
      h->getRef(&(pObjVers[maxNoOfOid-(noOfOid+1)]));
      ++noOfOid;
      if (pBody != NULL){
        if (cont->m_size != (unsigned) objBodySize || maxNoOfOid != 1){
          // ERROR
        }
        LVCSim_ObjectVerHandle *v = h->lookupObject(this, objHistReadCount);
        // the node is visible, but can be in new state. in this case, do not copy
        if (v){
          SAPDB_MemCopyNoCheck(pBody, v->m_data.fix, objBodySize);
        }
      }
    }

    h = it->prevObject(this);
  }

  if (noOfOid < maxNoOfOid){
    // If array is not filled completely, then shift the entries in the array 
    // to the left side
    int i;
    for (i=0; i<noOfOid; ++i){
      pResultOId[i] = pResultOId[i+maxNoOfOid-noOfOid];
      pObjVers[i]   = pObjVers[i+maxNoOfOid-noOfOid];
    }
  }

  e = (h ? e_ok : e_no_next_object);
  return S_OK;
}


// PTS 1119480
HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::VersionPrevKey(   
  tgg01_OmsVersionContext   *VersionContext,
  const tgg01_ContainerId   &ObjFile,
  void                      *pKey,
  size_t                     KeyLen,
  bool                      Inclusive,
  const void                *pStopKey,
  SAPDB_UInt8                &Oid,
  tgg00_BasisError          &e)
{
  // TODO: not needed, all m_versions are in memory
  e = e_OMS_not_implemented;   // PTS 1124935
    LVCSIM_ASSERT(false)
  return S_OK;
}


// PTS 1119480
HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::StartUnloadedVersionIter(
  tgg91_TransNo           &consistentView,
  tgg01_OmsVersionContext *lpVersionContext,
  tgg00_FileId            &objFile,
  size_t                   keyLen,
  void                    *pStartKey,            
  const void              *pLowerKey,
  const void              *pUpperKey,
  size_t                   ObjBodySize,           
  void                    *pObjBody,             
  SAPDB_UInt8              &ResultOId,  
  tgg00_BasisError        &e,
  OmsIterDirection        direction)             
{
  // TODO: not needed, all m_versions are in memory
  e = e_OMS_not_implemented;   // PTS 1124935
    LVCSIM_ASSERT(false)
  return S_OK;
}

HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::CreateContainer(
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
    short               *lpb_error)
{
  // TODO Save value of useCachedKeys for later requests (ExistsContainer)
  //      Consider isVarObject

  LVCSim_SchemaMeta *schemaMeta;
  if ((schemaMeta = schemaDir->find(SchemaId)) == NULL) {
    *lpb_error = e_unknown_schema;
    return S_OK;
  }
  *lpb_error = schemaMeta->lockSchema(this, false /*share*/);
  if(*lpb_error != e_ok)
    return S_OK;

  tgg00_ObjColDesc    InvColDesc;
  tgg00_StackEntry    KeyDesc;
  if (keyLen == 0){
    // Initialize kernel-structure with no-key data // PTS 1122540
    InvColDesc.colHashCount_gg00 = 0;           
    InvColDesc.colInvCount_gg00  = 0;          
    InvColDesc.colHashList_gg00  = NULL;      
    InvColDesc.colInvList_gg00   = NULL;      
  }
  else {
    // Initialize kernel-structure with key data // PTS 1122540
    InvColDesc.colHashCount_gg00 = 1;
    InvColDesc.colInvCount_gg00  = keyPartitioned;
    InvColDesc.colHashList_gg00  = REINTERPRET_CAST(tgg00_StackListPtr, &KeyDesc);
    InvColDesc.colInvList_gg00   = NULL;
    KeyDesc.etype().becomes(st_object_col);
    KeyDesc.eop().becomes(op_none);
    KeyDesc.epos()     = keyPos;
    KeyDesc.elen_var() = keyLen;
    KeyDesc.ecol_pos() = -1;
  }

  OMS_ContainerHandle newHandle,oldHandle; // PTS 1124356 FF 2003-09-23 new FileDir
  LVCSim_ContainerMeta *old;

  {
    RTESync_LockedScope lck(*containerLock);

    if (containerCount >= ((1 << LVCSIM_CONTID_BITS) - 1)) {
      // sorry, maximum count of containers exceeded
      *lpb_error = e_hash_memory_exceeded;
      return S_OK;
    }
    ++containerCount;                             // PTS 1124356 FF 2003-09-23 new FileDir
    newHandle.SetOldContainerHandle(containerCount);  // PTS 1124356 FF 2003-09-23 new FileDir
  }

  LVCSim_ContainerID key(*lpGUID, SchemaId, ContainerNo);
  LVCSim_ContainerMeta *meta = new LVCSim_ContainerMeta(newHandle, size,key,
    (tgg00_ObjColDesc*) &InvColDesc,isVarObject,useCachedKeys,keyPartitioned);
  meta->lockContainer(this, true /*exclusive*/);
  
  if ((old = containerDir->insert(key,newHandle,meta)) != NULL) {
    if(old->isLocked(this) && old->isMarkAsDropped()) {
      meta->m_history = old;
      containerDir->forceInsert(key,meta->getContHandle(),meta);
      undoList()->addInsert(meta);
    }
    else{
      size_t newsz = meta->m_size;
      bool hastree = meta->m_tree != NULL;
      bool useCachedKeys = meta->m_useCachedKeys;  
      
      meta->unlockContainer(this);
      delete meta;
      meta = old;
      
      {
        RTESync_LockedScope lck(*containerLock);
        if(newHandle.GetOldContainerHandle() == containerCount)
          --containerCount;
      }

      *lpb_error = meta->lockContainer(this, false /*share*/);  
      if(*lpb_error != e_ok)
        return S_OK;

      // check size
      if (newsz != meta->m_size) {
        *lpb_error = e_illegal_object_length;
        return S_OK;
      }
      if ((hastree && meta->m_tree == NULL) || (!hastree && meta->m_tree != NULL)) {
        *lpb_error = e_invalid_obj_col_desc;
        return S_OK;  
      }
      meta->m_useCachedKeys = useCachedKeys;
    }
  } else {
    undoList()->addInsert(meta);
  }
  // copy container info
  SAPDB_MemCopyNoCheck(lpContainerId, &meta->m_containerDesc, sizeof(tgg00_FileId));
  *lpb_error = e_ok;
  return S_OK;
}


HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::ResetVersion( 
    unsigned char *lpVersionContext,
    short         &error)
{
  // m_versions only in RAM => NOP
  error = e_ok;
  return S_OK;
}
 

HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::SetError( 
    short                 errorno,
    tsp00_Int4            buflen,
    const unsigned char  *lpv,
    tsp00_CodeType        codeType )
{
  // TODO: what is this?
  fprintf(stderr, "SetError called: %d(%d) %.*s\n", errorno, codeType, buflen, lpv);
  return S_OK;
}


HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::SQLRequest( 
    tsp00_Int4      buflen,
    unsigned char  *lpv)
{
  // TODO
    fprintf(stderr, "SQLRequest\n");
  return S_OK;
}

    
HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::SubtransCommit( 
    short  *lpb_error)
{
  if (!m_subTxList) {
    *lpb_error = e_OMS_no_open_subtrans;  // PTS 1124935
    return S_OK;
  }

  SubTxList *o = m_subTxList;
  m_subTxList = o->m_next;
  o->m_next = NULL;
    if (o->m_undoList) {
        // join undo lists
        o->m_undoList->append(m_undoList);
        m_undoList = o->m_undoList;
        o->m_undoList = NULL;
    }
  delete o;
  *lpb_error = e_ok;
  return S_OK;
}

    
HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::SubtransRollback( 
  short  *lpb_error)
{
  if (!m_subTxList) {
    *lpb_error = e_OMS_no_open_subtrans;  // PTS 1124935
    return S_OK;
  }

  SubTxList *o = m_subTxList;
  m_subTxList = o->m_next;
  if (m_undoList) {
      m_undoList->rollback(this);
      delete m_undoList;
  }
  m_undoList = o->m_undoList;
  o->m_undoList = NULL;
  o->m_next = NULL;
  delete o;

  if(m_undoList == NULL)
    m_objectsUpdated = false;

  *lpb_error = e_ok;
  return S_OK;
}

    
HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::SubtransStart( 
  short  *lpb_error)
{
  m_subTxList = new SubTxList(m_undoList, m_subTxList);
  m_undoList = NULL;
  *lpb_error = e_ok;
  return S_OK;
}


HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::UnlockObj( 
    unsigned char  *lpContainerId,
    SAPDB_UInt8     *lpoid,
    short          *lpb_error)
{
  tgg00_FileId *cid = (tgg00_FileId *) lpContainerId;
  LVCSim_ContainerMeta *cont = getContainer(*cid, (OMS_ObjectId8*)lpoid, lpb_error);
  if(*lpb_error != e_ok)
    return S_OK;

  LVCSim_ObjectHandle *h = (LVCSim_ObjectHandle*) cont->m_alloc->
    map(reinterpret_cast<LVCSim_OIDObject*>(lpoid)->getCurrentOid());
  if (!h || h->getGeneration() != ((OMS_ObjectId8*) lpoid)->getGeneration()) {
    *lpb_error = e_object_not_found;
    return S_OK;
  }

  *lpb_error = h->unlockObject(this);
  return S_OK;
}

HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::UnlockSharedObj( 
    unsigned char  *lpContainerId,
    SAPDB_UInt8     *lpoid,
    short          *lpb_error)
{
  tgg00_FileId *cid = (tgg00_FileId *) lpContainerId;
  LVCSim_ContainerMeta *cont = getContainer(*cid, (OMS_ObjectId8*)lpoid, lpb_error);
  if(*lpb_error != e_ok)
    return S_OK;

  LVCSim_ObjectHandle *h = (LVCSim_ObjectHandle*) cont->m_alloc->
    map(reinterpret_cast<LVCSim_OIDObject*>(lpoid)->getCurrentOid());
  if (!h || h->getGeneration() != ((OMS_ObjectId8*) lpoid)->getGeneration()) {
    *lpb_error = e_object_not_found;
    return S_OK;
  }

  *lpb_error = h->unlockSharedObject(this);
  return S_OK;
}



// PTS 1121449
HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::UpdateVarObjMass(
    tsp00_Int4               cnt,
    tgg91_TransNo           *pConsistentView,
    tgg01_ContainerId      **ppContainerId,
    unsigned char           *pVersionContext,
    SAPDB_UInt8              *pOid,
    tgg91_PageRef           *pObjSeq,
    size_t                  *pObjSize,
    size_t                  *pBodySize,
    unsigned char          **ppBody,
    SAPDB_UInt8              *pNextOid,
    bool                    *pContObj,  // PTS 1124278
    bool                    *pReused,   // PTS 1127661
    tsp00_Int4              &errCnt,
    tgg00_BasisError        *pDBError)
{
  OMS_VarObjContainer varObjContainer;

  errCnt = 0;
  for (int i=0; i<cnt; ++i) {
    if (pBodySize[i] != 0){
      SAPDB_MemCopyNoCheck(&varObjContainer.vobjInfo[0], ppBody[i], pBodySize[i]);
    }
    varObjContainer.m_vobjSize = pObjSize[i];
    varObjContainer.m_vobjNext = (OMS_ObjectId8&)pNextOid[i];

    UpdateObj(
      (unsigned char*) pConsistentView,
      (unsigned char*) ppContainerId[i],
      pVersionContext,
      (unsigned char*) &pOid[i],
      (unsigned char*) &pObjSeq[i],
      reinterpret_cast<tgg92_KernelOid*>(&pNextOid[i])->gg92IsNil() 
         ? pBodySize[i]
         : sizeof(OMS_VarObjContainer),
      (unsigned char*) &varObjContainer,
      0, 0,    // Key
      pContObj[i],
      pReused[i],   
      &pDBError[i]);
    if (pDBError[i] != e_ok){
      ++errCnt;
    }
  }
  return S_OK;
}

// PTS 1121449
HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::UpdateObjMass(
    tsp00_Int4               cnt,
    tgg91_TransNo           *pConsistentView,
    tgg01_ContainerId      **ppContainerId,
    unsigned char           *pVersionContext,
    SAPDB_UInt8              *pOid,
    tgg91_PageRef           *pObjSeq,
    size_t                  *pBodySize,
    unsigned char          **ppBody,
    size_t                  *pKeyPos,       // PTS 1122540
    size_t                  *pKeyLen,       // PTS 1122540
    bool                    *pReused,       // PTS 1127661
    bool                     clustered,
    tsp00_Int4              &errCnt,
    tgg00_BasisError        *pDBError)
{
  errCnt = 0;
  for (int i=0; i<cnt; ++i){
    UpdateObj(
      (unsigned char*) pConsistentView,
      (unsigned char*) ppContainerId[i],
      pVersionContext,
      (unsigned char*) &pOid[i],
      (unsigned char*) &pObjSeq[i],
      pBodySize[i],
      (unsigned char*) ppBody[i],
      pKeyPos ? pKeyPos[i] : NULL,
      pKeyLen ? pKeyLen[i] : NULL,
      false,
      pReused[i],           
      &pDBError[i]);
    if (pDBError[i] != e_ok){
      ++errCnt;
    }
  }
 return S_OK;
}

      
HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::UpdateObj( 
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
    short          *lpb_error)
{
  CHECK_TID_WRITE(lpConsistentView)

  tgg00_FileId *cid = (tgg00_FileId *) lpContainerId;
  LVCSim_ContainerMeta *cont = getContainer(*cid, (OMS_ObjectId8*) lpoid, lpb_error);
  if(*lpb_error != e_ok)
    return S_OK;

  LVCSim_ObjectHandle *h = (LVCSim_ObjectHandle*) cont->m_alloc->
    map(reinterpret_cast<LVCSim_OIDObject*>(lpoid)->getCurrentOid());

  if (!h || (!reused && h->getGeneration() != ((OMS_ObjectId8*) lpoid)->getGeneration())) {
    *lpb_error = e_object_not_found;
    return S_OK;
  }

  if (reused && h->getGeneration() !=  ((OMS_ObjectId8*) lpoid)->getGeneration()){
    // Adapt generation if it has been changed.  // PTS 1125361
    h->setGeneration(((OMS_ObjectId8*) lpoid)->getGeneration());
  }

  if (cont->m_size == 0) {
    // variable-sized obj
    cont = cont;
  } else if (cont->m_size != (unsigned) bodysize) {
    LVCSIM_THROW("liveCacheSink::UpdateObj - invalid object size");
  }

  // we update only body, key cannot change
  *lpb_error = h->update(this, lpbody, bodysize, cont->m_size == 0, contObj);
  if (*lpb_error == e_ok) {
        undoList()->addUpdate(h);
        m_objectsUpdated = true;
    } else {
        *lpb_error = *lpb_error;
  }
  return S_OK;
}

  
HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::Vtrace( 
    tsp00_Int4      buflen,
    unsigned char  *lpv)
{
  RTESync_LockedScope lck(traceLock);

    if (!traceFile) {
        traceFile = stderr;
    }
#if defined( WIN32 ) && !defined( _WIN64 )
  fprintf(traceFile, "%08p %6d ", (const void*) this, m_readView);
#else
  fprintf(traceFile, "%016p %6d ", (const void*) this, m_readView);
#endif
    fwrite(lpv, 1, buflen, traceFile);
    fputs("\n", traceFile);
  fflush(traceFile);
  return S_OK;
}


void LVCSim_liveCacheSink::SetVTraceFile(
  FILE *f)
{
  RTESync_LockedScope lck(traceLock);

    if (traceFile && traceFile != stderr) {
        fclose(traceFile);
    }
  traceFile = f;
}


void LVCSim_liveCacheSink::getReadConsistentView(void *lpConsistentView, short *lpb_error)
{
    *lpb_error = e_ok;
  if (m_readView == LVCSim_TID_NONE) {
      LVCSim_TID consistentView = *((LVCSim_TID *) lpConsistentView);
      ActiveVersion *version = ActiveVersion::find(consistentView);
      if (version) {
          if (m_activeVersion) {
            if(version != m_activeVersion) {
              CloseVersion(lpb_error);
              m_activeVersion = version;
              version->markInUse();
              m_readView = consistentView;
              //m_otherTXs->Release(true);
              m_otherTXs = m_activeVersion->getOtherTXs();
            }
          } else {
              //LVCSIM_ASSERT(!(version->isInUse()))
              version->markInUse();
              m_activeVersion = version;
              m_readView = consistentView;
              m_otherTXs = m_activeVersion->getOtherTXs();
          }
      } else {
          if (m_activeVersion){
              CloseVersion(lpb_error);
              m_activeVersion = NULL;
          }
          if(consistentView != LVCSim_TID_NONE && (consistentView == m_oldReadView)){ 
              m_readView = consistentView;
              m_otherTXs = m_oldOtherTXs;
              m_oldReadView = LVCSim_TID_NONE;
              m_oldOtherTXs = 0;
          }
          else{
              if(consistentView != LVCSim_TID_NONE)
                LVCSIM_ASSERT(false);
          }
          //LVCSIM_ASSERT((m_activeVersion == NULL))
      }
  } else if (*((LVCSim_TID *) lpConsistentView) != m_readView) {
      //*lpb_error = e_unknown_consist_vers;
      LVCSim_TID consistentView = *((LVCSim_TID *) lpConsistentView);
      ActiveVersion *version = ActiveVersion::find(consistentView);
      if (version) {
          if (m_activeVersion) {
              if(version != m_activeVersion) {
                  CloseVersion(lpb_error);
                  version->markInUse();
                  m_activeVersion = version;   
                  m_readView = consistentView;
                  //m_otherTXs->Release(true);
                  m_otherTXs = m_activeVersion->getOtherTXs();
              }
          } else {
              //LVCSIM_ASSERT(!(version->isInUse()))
              m_oldReadView = m_readView;
              m_oldOtherTXs = m_otherTXs;
              version->markInUse();
              m_activeVersion = version;
              m_readView = consistentView;
              m_otherTXs = m_activeVersion->getOtherTXs();
          }
      } else {
          if (m_activeVersion){
              CloseVersion(lpb_error);
              if(consistentView != LVCSim_TID_NONE && (consistentView == m_oldReadView)){ 
                  m_readView = consistentView;
                  m_otherTXs = m_oldOtherTXs;
                  m_oldReadView = LVCSim_TID_NONE;
                  m_oldOtherTXs = 0;
              }
              else if(consistentView != LVCSim_TID_NONE)
                  LVCSIM_ASSERT(false)
          }
          else {
              if(consistentView != LVCSim_TID_NONE)
                  LVCSIM_ASSERT(false)
              if(m_readView != LVCSim_TID_NONE){
                  m_readView = consistentView;
                  m_otherTXs->Release(true);
                  m_otherTXs = 0;
              }
          }
         
          //LVCSIM_ASSERT((m_activeVersion == NULL))
      }
  }

  if (m_readView == LVCSim_TID_NONE) {
    /* must create consistent view */
    NewConsistentView(false /* isOmsVersion PTS 1130354 */, (unsigned char*) lpConsistentView, lpb_error);
  }

    LVCSIM_ASSERT((m_readView >= txOldest));
}


void LVCSim_liveCacheSink::getWriteConsistentView(void *lpView, short *lpb_error)
{
    if (m_commitTime <= 0) {
        //if (m_readView != LVCSim_TID_NONE) {
        //    //m_commitTime = m_readView;  - also Ref
        //    LVCSIM_ASSERT(false)
        //} else 
            {
            m_commitTimeRef = new ConsViewList;
            RTESync_LockedScope lck(txCounterLock);
            m_commitTime = ++txCounter;
            ++txCounter;
        consViewList = new(m_commitTimeRef) ConsViewList(m_commitTime, false, consViewList);
            if (txOldest == LVCSim_TID_MAX) {
                txOldest = m_readView;
            }
        }
      SIM_TRACE(SIM_TX, "new write view: %d", m_commitTime)
    }
    *lpb_error = e_ok;
}

bool LVCSim_liveCacheSink::sees(LVCSim_TID otherTX)
{
    if (m_otherTXs) {
        return m_otherTXs->sees(otherTX);
    }

    // we shouldn't get here
    LVCSIM_ASSERT(false)
    throw DbpError(e_unknown_consist_vers);
}

LVCSim_TID LVCSim_liveCacheSink::getCommitTime()
{
    short err;
    getWriteConsistentView(NULL, &err);
    return m_commitTime;
}

bool LVCSim_liveCacheSink::checkCommited(LVCSim_TID tx)
{
    RTESync_LockedScope lck(txCounterLock);
    const LVCSim_liveCacheSink *p = firstSession;
    while (p) {
        if (p->m_commitTime == tx) {
            return false;
        }
        p = p->m_next;
    }
    return true;
}

LVCSim_ContainerMeta *LVCSim_liveCacheSink::getContainer(tgg00_FileId& desc, short *lpb_error){							
  if ((desc).fileTfn_gg00() != tfnObj_egg00) {                           
    *lpb_error = e_invalid_filetype;
    return NULL;
  }                                   
    /* PTS 1124356 FF 2003-09-23 new FileDir */                             
  LVCSim_ContainerMeta *cont = containerDir->find((desc).fileDirFileId_gg00());  
  if (!cont ) {                              
    *lpb_error = e_OMS_file_not_found;  /* PTS 1124935 */                                                
    return NULL;
  }                                    
  *lpb_error = cont->lockContainer(this, false /*share*/);
  if(*lpb_error != e_ok)
    return NULL;

  return cont;
}
 
LVCSim_ContainerMeta *LVCSim_liveCacheSink::getContainer( tgg00_FileId& desc, const OMS_ObjectId8 *lpoid, short *lpb_error){																				
	LVCSim_ContainerMeta *cont;
  if ((desc).fileObjFileType_gg00() == oftUnknownType_egg00) {      
    if (((OMS_ObjectId8 *) lpoid)->getPno() == NIL_OBJ_PAGE_NO) {       
      /* not allowed - we must know either OID or container ID */   
      *lpb_error = e_object_not_found;
      return NULL;
    }                                 
    /* fill in the container ID according to OID */          
    cont = containerDir->find(reinterpret_cast<const LVCSim_OIDObject *>(lpoid)->getContainer());    
    if (!cont) {                            
      *lpb_error = e_object_not_found;   
      return NULL;
    }     
    *lpb_error = cont->lockContainer(this, false /*share*/);
    if(*lpb_error == e_OMS_request_timeout)
      return NULL;
    else if(*lpb_error != e_ok){
      *lpb_error = e_object_not_found;
      return NULL;
    }                                
                                  
    /* copy into desc */                        
    desc = cont->m_containerDesc;

  } else {                                
        /* PTS 1124356 FF 2003-09-23 new FileDir */                         
    cont = containerDir->find((desc).fileDirFileId_gg00());
    if (!cont) {                            
      *lpb_error = e_OMS_file_not_found;   
      return NULL;
    }
    *lpb_error = cont->lockContainer(this, false /*share*/);
    if(*lpb_error != e_ok)
      return NULL;
  }
  *lpb_error = e_ok;
  return cont;
}


HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::GetDefaultContext(
    void         **p, 
    pasbool      **pCancelByte,
        bool          &optimizedStreamCommunication, /* PTS 1130281 */
    tsp00_TaskId  &taskId /* PTS 1110315 */)
{
  *p = m_defaultContext;
  *pCancelByte = &m_cancelByte;
    optimizedStreamCommunication = false;
  taskId = m_taskId;
  return S_OK;
}
    
    
HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::SetDefaultContext(
  void* p)
{
  m_defaultContext = p;
  return S_OK;
}


/* PTS 1120794 */
HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::GetSqlSessionContext (
  void** p,
  tsp00_TaskId  &taskId)
{
  return S_OK;
}


/* PTS 1120794 */
HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::SetSqlSessionContext (
  void* p)
{
  return S_OK;
}


// TODO: move this stuff in declarations and properly RESET

class FileDirEntry {
public:
  RTESync_Spinlock  lock;
  tsp00_Uint4     refcnt;
  tsp00_Uint4     maxsize;
  FILE        *fptr;

  FileDirEntry(FILE *f, tsp00_Uint4 max) : refcnt(1), maxsize(max), fptr(f) { }
  ~FileDirEntry() { fclose(fptr); }
};

class FileName {
public:
  char  name[256];

  FileName(const char *p)
  {
    strncpy(name, p, 255);
    name[255] = 0;
  }

  size_t hash() const
  {
    size_t sum = 0, shift = 0;
    const char *p = name;
    while (*p) {
      sum ^= (size_t) *(p++) << shift;
      if (++shift == 25) shift = 0;
    }
    return sum;
  }

  bool operator ==(const FileName &x) const
  {
    return strcmp(name, x.name) == 0;
  }
};

typedef LVCSim_FastHash<FileName, tsp00_Int4, class FileDirEntry, false>    FileDirHash;

RTESync_Spinlock    fileDirLock;
FileDirHash         *fileDir = NULL;
tsp00_Int4          fileDirID = 0;


HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::OpenFile(
    char        *lpFileName,
    tsp00_Uint4  FileSize,
    boolean      doAppend, 
    tsp00_Int4  *FileHandle,
    short       *lpb_error)
{
  RTESync_LockedScope lck(fileDirLock);

  FileName name(lpFileName);

    if (!fileDir) {
        LVCSim::DebugAllocatorEscape(LVCSIM_DA_ENTER_NOLEAK);
        fileDir = new FileDirHash;
        LVCSim::DebugAllocatorEscape(LVCSIM_DA_EXIT_NOLEAK);
    }
  FileDirEntry *e = fileDir->find(name, *FileHandle);
  if (e) {
    // increase reference count only
    e->refcnt++;
  } else {
    // insert new one
    FILE *f = fopen(lpFileName, doAppend ? "ab+" : "wb");
    /*
    fputs("---EOF---\n", f);
    fseek(f, 0, SEEK_SET);
    */
    if (!f) {
      *lpb_error = e_OMS_file_not_accessible;  // PTS 1124935 // TODO: OK? 
      return S_OK;
    }
    fileDir->insert(name, ++fileDirID, new FileDirEntry(f, FileSize));
    *FileHandle = fileDirID;
  }
  *lpb_error = e_ok;
  return S_OK;
}
      

HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::WriteFile(
    tsp00_Int4 FileHandle,
    char      *msg,
    short     *lpb_error)
{
    FileDirEntry *e = fileDir ? fileDir->find(FileHandle) : NULL;
  if (!e) {
    *lpb_error = e_OMS_file_not_found;  // PTS 1124935
    return S_OK;
  }

  tsp00_Date dat;
  tsp00_Time tim;
  GetDateTime(&dat, &tim);

  RTESync_LockedScope lck(e->lock);
  fwrite(msg, 1, strlen(msg), e->fptr);
/*
  fwrite(&dat, sizeof(dat), 1, e->fptr);
  fputc(' ', e->fptr);
  fwrite(&tim, sizeof(tim), 1, e->fptr);
  fputc(' ', e->fptr);

  fputs(msg, e->fptr);
  fputs("\n", e->fptr);
  long pos = ftell(e->fptr);
  if (pos >= e->maxsize) {
    pos = 0;
    fseek(e->fptr, 0, SEEK_SET);
  }
  fputs("---EOF---\n", e->fptr);
  fseek(e->fptr, 0, pos);
*/
  *lpb_error = e_ok;
  return S_OK;
}


HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::CloseFile(
    tsp00_Int4  FileHandle,
    short      *lpb_error)
{
    FileDirEntry *e = fileDir ? fileDir->find(FileHandle) : NULL;
  if (!e) {
    *lpb_error = e_OMS_file_not_found;  // PTS 1124935
    return S_OK;
  }

  {
    RTESync_LockedScope lck(e->lock);
    fflush(e->fptr);
  }

  RTESync_LockedScope lck(fileDirLock);

  if (--e->refcnt == 0) {
    // close file
    fileDir->remove(FileHandle);
    delete e;
  }

  *lpb_error = e_ok;
  return S_OK;
}


HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::SyncFile(
    char       *lpFileName,
    tsp00_Int4  FileHandle,
    short      *lpb_error)
{
  FileDirEntry *e;
    if (fileDir) {
      if (lpFileName) {
        FileName name(lpFileName);
        e = fileDir->find(name, FileHandle);
      } else {
        e = fileDir->find(FileHandle);
      }
    } else {
        e = NULL;
    }
  if (!e) {
    *lpb_error = e_OMS_file_not_found;  // PTS 1124935
    return S_OK;
  }

  RTESync_LockedScope lck(e->lock);
  fflush(e->fptr);

  *lpb_error = e_ok;
  return S_OK;
}


HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::GetDateTime(
  tsp00_Date* dat,
  tsp00_Time* tim)
{
  // TODO: use platform-independent functions instead of time() and localtime()

    time_t              time_sec;
    long                usec;
    struct tm     *tmbuf ;
    tsp00_Byte        *ptr ;

    time_sec = time(NULL);
    tmbuf = localtime(&time_sec);

    /*
     * ASCII  '0'
     */
    (void) memset ( dat , '0' , sizeof(tsp00_Date) );
    (void) memset ( tim , '0' , sizeof(tsp00_Time) );

    ptr = ((unsigned char*) &dat[0]) + sizeof(tsp00_Date) ;
     usec = tmbuf->tm_mday ;
    *(-- ptr) += (tsp00_Byte) (usec %  10);
     usec /= 10 ;
    *(-- ptr) += (tsp00_Byte) (usec %  10);
     usec = tmbuf->tm_mon + 1 ;
    *(-- ptr) += (tsp00_Byte) (usec %  10);
     usec /= 10 ;
    *(-- ptr) += (tsp00_Byte) (usec %  10);
     usec = tmbuf->tm_year + 1900 ;
    *(-- ptr) += (tsp00_Byte) (usec %  10);
     usec /= 10 ;
    *(-- ptr) += (tsp00_Byte) (usec %  10);
     usec /= 10 ;
    *(-- ptr) += (tsp00_Byte) (usec %  10);
     usec /= 10 ;
    *(-- ptr) += (tsp00_Byte) (usec %  10);

    ptr = ((unsigned char*) &tim[0]) + sizeof(tsp00_Time) ;
     usec = tmbuf->tm_sec ;
    *(-- ptr) += (tsp00_Byte) (usec %  10);
     usec /= 10 ;
    *(-- ptr) += (tsp00_Byte) (usec %  10);
     usec = tmbuf->tm_min ;
    *(-- ptr) += (tsp00_Byte) (usec %  10);
     usec /= 10 ;
    *(-- ptr) += (tsp00_Byte) (usec %  10);
     usec = tmbuf->tm_hour ;
    *(-- ptr) += (tsp00_Byte) (usec %  10);
     usec /= 10 ;
    *(-- ptr) += (tsp00_Byte) (usec %  10);

  return S_OK;
}
    

HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::GetClock(
  tsp00_Int4* Sec,
  tsp00_Int4* MSec)
{
#ifdef  WIN32
  _timeb dtm;
  _ftime(&dtm);
  *Sec = dtm.time;
  *MSec = dtm.millitm * 1000;
#else
  struct timeval tm;
  gettimeofday(&tm, NULL);
  *Sec = tm.tv_sec;
  *MSec = tm.tv_usec;
#endif
  return S_OK;
}


HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::DropVersion( 
  unsigned char  *lpVersionContext,
  short          *lpb_error)
{
  // end consistent view, TX in undefined cons. view
    tgg01_OmsVersionContext* dropVersion = 
        reinterpret_cast<tgg01_OmsVersionContext*>(lpVersionContext);
    LVCSim_TID readView = *reinterpret_cast<LVCSim_TID*>(
        &dropVersion->ovc_trans_version);
    ActiveVersion *version = ActiveVersion::find(readView);
    LVCSIM_ASSERT((version != NULL))

  SIM_TRACE2(SIM_TX, "drop version %08x, view: %d", version, version->getReadView())
  EndConsistentView((unsigned char*) &dropVersion->ovc_trans_version, lpb_error);
    if (*lpb_error != e_ok) {
        return S_OK;
    }
    version->unmarkInUse();
    version->destroy();
  *lpb_error = e_ok;
  return S_OK;
}
    

HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::CreateSchema(
    const OmsTypeWyde *lpSchemaName,
    tsp00_Int4        *lpSchemaId,
    short             *lpb_error)
{
  {
    RTESync_LockedScope lck(*schemaLock);
    *lpSchemaId = schemaCount++;
  }
  
  LVCSim_SchemaMeta *old = NULL;
  LVCSim_SchemaMeta *n = new LVCSim_SchemaMeta(*lpSchemaId, lpSchemaName);
  
  if (n->getSchemaId() == OMS_DEFAULT_SCHEMA_HANDLE) {
    if((old = schemaDir->insert(lpSchemaName,*lpSchemaId,n)) != NULL)
      *lpb_error = e_duplicate_schema;
    else
      *lpb_error = e_ok;
    return S_OK;
  }
  
  n->lockSchema(this, true /*exclusive*/); 
  if ((old = schemaDir->insert(lpSchemaName,*lpSchemaId,n)) != NULL) {
      if(old->isLocked(this) && old->isMarkAsDropped()){
        n->m_history = old;
        schemaDir->forceInsert(lpSchemaName, *lpSchemaId, n);  
        undoList()->addInsert(n);
        *lpb_error = e_ok;
      }
      else{
        n->unlockSchema(this);
        delete n;

        *lpb_error = old->lockSchema(this, false /*share*/);
        if(*lpb_error == e_ok)
          *lpb_error = e_duplicate_schema;
        
        RTESync_LockedScope lck(*schemaLock);
        if (*lpSchemaId == (schemaCount-1)) 
          --schemaCount;     
      }
  } else {
      undoList()->addInsert(n);
      *lpb_error = e_ok;
  }
  return S_OK;
}


HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::ExistsSchema(
    const OmsTypeWyde *lpSchemaName,
    tsp00_Int4        *lpSchemaId,
    short             *lpb_error)
{
  int id;
  LVCSim_SchemaMeta *meta = NULL;
  if ((meta = schemaDir->find(lpSchemaName, id)) != NULL) {
    *lpb_error = meta->lockSchema(this, false /*share*/);
    if(*lpb_error == e_ok){
      *lpSchemaId = id;
    }
    else{
      *lpSchemaId = 0;
      if(*lpb_error == e_OMS_file_not_found)
        *lpb_error = e_unknown_name;
    }   
  } else {
    *lpSchemaId = 0;
    *lpb_error = e_unknown_name;
  }
  return S_OK;
}


HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::DropSchema(
    tsp00_Int4  SchemaId,
    short      *lpb_error)
{
  LVCSim_SchemaMeta *meta = NULL;
  if (SchemaId == OMS_DEFAULT_SCHEMA_HANDLE) {
    *lpb_error = e_OMS_missing_privilege;  // PTS 1124935
  } 
  else{
    meta = schemaDir->find(SchemaId);
    if (meta == NULL){
      *lpb_error = e_unknown_schema;
    } 
    else{
      *lpb_error = meta->lockSchema(this, true /*exclusive*/);
      if( *lpb_error == e_ok){
        *lpb_error = meta->dropAllContainer(this);
        if(*lpb_error == e_ok){ 
          meta->m_isDropped = true;
          undoList()->addDelete(meta);
        }
      }
    }
  }
  return S_OK;
}


HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::GetSchemaName(
    tsp00_Int4    SchemaId,
    OmsTypeWyde  *lpSchemaName,
    short        *lpb_error)
{
  LVCSim_SchemaMeta *s = schemaDir->find(SchemaId);
  if (s) {
    *lpb_error = s->lockSchema(this, false /*share*/);
    if( *lpb_error == e_ok){
      LVCSim_wstrcpy(lpSchemaName, s->m_name);
      *lpb_error = e_ok;
    }
  } else {
    *lpb_error = e_unknown_schema;
  }
  return S_OK;
}


HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::ExistsFile(
    char    *lpFileName,
    bool    &existsFile,
    bool    &isDirectory,
    short   &error)
{
  error = e_ok;

#if defined(_WIN32)
  DWORD Attributes = GetFileAttributes(lpFileName);
  
  existsFile = ( Attributes != 0xFFFFFFFF );
  
  if (existsFile)
  {
    isDirectory = ((Attributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY);
  } else {
    if (GetLastError() != ERROR_FILE_NOT_FOUND) {
      error = -903; /* according to vak07 */
    }
    isDirectory = false;
  }
#else
  struct stat testBuf;
  
  // stat follows symbolic links...
  existsFile = (stat(lpFileName, &testBuf) == 0);
  
  if (existsFile)
  {
    isDirectory = (testBuf.st_mode == S_IFDIR);
  } else {
    if (errno != ENOENT) {
      error = -903; /* according to vak07 */
    }
    isDirectory = false;
  }
#endif
  if (error != e_ok) {
    // TODO: write diag message
      //char buf[132];                                  
    //sprintf(&buf[0], "ExistsFile: %.100s  err: %d ", lpFileName, Error);
    //MSGD ((ERR_DCOM_DUMP_INFO,buf));
  }
  return S_OK;
}
    
      
HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::DropFile(
    char  *lpFileName,
    short &error)
{
  FileName name(lpFileName);
  tsp00_Int4 FileHandle;
    FileDirEntry *e = fileDir ? fileDir->find(name, FileHandle) : NULL;

  if (e) {
    error = e_file_not_accessible;  // PTS 1124935; // TODO: proper error constant
    return S_OK;
  }

  // TODO
  unlink(lpFileName);

  error = e_ok;
  return S_OK;
}


HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::GetWorkingDirectory(
    tsp00_Pathc &pathName,
    short       &error)
{
  static tsp00_Pathc currentWorkingDirectory; /* never changed after first getcwd */
  static bool wasSet = false;

  if (!wasSet) {
    wasSet = ( 
#if defined(_WIN32)
      GetCurrentDirectory(sizeof(tsp00_Pathc), &currentWorkingDirectory[0]) != 0xFFFFFFFF 
#else
      getcwd(&currentWorkingDirectory[0], sizeof(tsp00_Pathc)) != NULL 
#endif
        );
  }
  if (wasSet) {
    strcpy(pathName, currentWorkingDirectory);
    error = e_ok;
  } else {
    error = e_OMS_hostfile_error;  // PTS 1124935
  }

  return S_OK;
}


HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::GetDbrootSapPath(
    tsp00_Pathc &pathName,
    short       &error)
{
  tsp01_RteError rteError;
  if (sqlGetDbrootSapPath (pathName, TERM_WITH_DELIMITER_EO01, &rteError)) {
    error = e_ok;
  } else {
    error = e_hostfile_error;  // PTS 1124935
/*
    char buf[256];                                  
    SAPDB_MemCopyNoCheck(&pathName[0], &rteError.RteErrText[0], sizeof(rteError.RteErrText));
    pathName[sizeof(rteError.RteErrText)] = 0;
    sprintf(&buf[0],"GetDbrootSapPath  rteError: %d %.80s", rteError.RteErrCode, rteError.RteErrText );
    MSGD ((ERR_DCOM_DUMP_INFO,buf));
    sprintf(&buf[0],"GetDbrootSapPath  osError: %d %.80s", rteError.OsErrCode, rteError.OsErrText );
    MSGD ((ERR_DCOM_DUMP_INFO,buf));
    error = a07_return_code (e_hostfile_error, sqlm_internal);
*/  
  }
  return S_OK;
}
      
HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::LoadLibraryEx( 
    const char    *path, 
    bool           loadGlobal,
    HANDLE        *hd,
    tsp00_ErrText &errtext,
    int            errtext_size)
{
  // TODO: keep in a list to prevent duplicates?
  char buffer[1024];
  *hd = sqlLoadLibraryEx(CONST_CAST(char*,path), (loadGlobal) ? RTLD_GLOBAL_EO02 : RTLD_LOCAL_EO02, 
    buffer, sizeof(buffer));
  if (!*hd) {
    ConsoleMsg(strlen(buffer), (unsigned char*) buffer);
  }
  return S_OK;
}


HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::FreeLibrary( 
    HANDLE         hd,
    bool          &ok,
    tsp00_ErrText &errtext,
    int            errtext_size)
{
  char buffer[1024];
  if (sqlFreeLibrary(hd, buffer, sizeof(buffer))) {
    ok = true;
  } else {
    ok = false;
    ConsoleMsg(strlen(buffer), (unsigned char*) buffer);
  }
  return S_OK;
}


HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::DropContainer(
    OmsSchemaHandle  schema,
    const GUID       &guid,
    OmsContainerNo    cno,
    tgg00_BasisError &error)
{
  LVCSim_SchemaMeta *schemaMeta;
  if ((schemaMeta = schemaDir->find(schema)) == NULL) {
    error = e_unknown_schema;
    return S_OK;
  }
  error = schemaMeta->lockSchema(this, false /*share*/);
  if(error != e_ok)
    return S_OK;

  OMS_ContainerHandle id; // PTS 1124356 FF 2003-09-23 new FileDir
  LVCSim_ContainerID cid(guid, schema, cno);
  LVCSim_ContainerMeta *m = containerDir->find(cid, id);  
  
  if (!m) {
    error = e_OMS_file_not_found;  // PTS 1124935
    return S_OK;
  } 
  error = m->lockContainer(this, true /*exclusive*/);
  if( error == e_ok){
    // mark container as dropped
    error = m->markDropped(this);
    if( error == e_ok){
      undoList()->addDelete(m);
    }
  }
  return S_OK;
}

HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::GetContainerInfo(
    OMS_ContainerHandle &containerHandle, // PTS 1124356 FF 2003-09-23 new FileDir
    GUID            &guid,
    OmsSchemaHandle &schema,
    OmsContainerNo  &cno,
    short           &error)
{
  LVCSim_ContainerID info;
  LVCSim_ContainerMeta *m = containerDir->find(containerHandle, info);
  if (!m) {
    error = e_OMS_file_not_found;  // PTS 1124935;
    return S_OK;
  }
  if(m->isLocked() && !m->isLocked(this)){ // container is locked by other transaction, can not access
    error = e_OMS_request_timeout;
    return S_OK;
  }
  if (m->isMarkAsDropped()) {  // container is marked as dropped, can not access
    error = e_OMS_file_not_found;  
    return S_OK;
  }

  guid = info.guid;
  schema = info.schema;
  cno = info.cno;

  error = e_ok;
  return S_OK;
}


HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::IsObjHistoryUsed( 
  const SAPDB_UInt8  &oid,
  bool              ignoreVersions,  /*PTS 1130354*/
  bool              &isUsed,
  tgg00_BasisError  &e,
  OmsTypeUInt8      &OldestTransNo /*PTS 1130354*/)
{
  // no m_versions on disk, no GC => NOP
  short *lpb_error = &e;
  tgg01_ContainerId containerId;
  containerId.fileObjFileType_gg00().becomes(oftUnknownType_egg00);
  LVCSim_ContainerMeta *cont = getContainer(containerId, (OMS_ObjectId8*)&oid, lpb_error);
  if(*lpb_error != e_ok)
    return S_OK;

  LVCSim_ObjectHandle *h = (LVCSim_ObjectHandle*) cont->m_alloc->map(reinterpret_cast<const LVCSim_OIDObject*>(&oid)->getCurrentOid());
  
  isUsed = h->isObjectHistoryUsed(this);
  e = e_ok;
  return S_OK;
}


HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::Reschedule()
{
  // TODO
  return S_OK;
}
 

/* PTS 1132163 */
HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::Sleep(
    const OmsTypeUInt1 sleepTime)
{
  // TODO
  return S_OK;
}


// PTS 1121449
HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::LockUpdObjMass(
    tsp00_Int4               cnt,
    tgg91_TransNo           *pConsistentView,
    tgg01_ContainerId      **ppContainerId,
    SAPDB_UInt8              *pOid,
    tgg91_PageRef           *pObjSeq,
    bool                     clustered,
    tsp00_Int4              &errCnt,
    tgg00_BasisError        *pDBError)
{
  errCnt = 0;
  for (int i=0; i<cnt; ++i){
    LockUpdObj(
      (unsigned char*) pConsistentView,
      (unsigned char*) ppContainerId[i],
      &pOid[i],
      (unsigned char*) &pObjSeq[i],
      &pDBError[i]);
    if (pDBError[i] != e_ok){
      ++errCnt;
    }
  }
  return S_OK;
}


HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::LockUpdObj( 
    unsigned char   *lpConsistentView,
    unsigned char   *lpContainerId,
    SAPDB_UInt8      *lpoid,
    unsigned char   *lpobjseq,
    short           *lpb_error)
{
  CHECK_TID_READ(lpConsistentView)

  tgg00_FileId *cid = (tgg00_FileId *) lpContainerId;
  LVCSim_ContainerMeta *cont = getContainer(*cid, (OMS_ObjectId8*)lpoid, lpb_error);
  if(*lpb_error != e_ok)
    return S_OK;

  LVCSim_ObjectHandle *h = (LVCSim_ObjectHandle*) cont->m_alloc->
    map(reinterpret_cast<LVCSim_OIDObject*>(lpoid)->getCurrentOid());
  if (!h || h->getGeneration() != ((OMS_ObjectId8*) lpoid)->getGeneration()) {
    *lpb_error = e_object_not_found;
    return S_OK;
  }

  // TODO: check - when new varobj with no data will be locked
    size_t updateLen = cont->m_size ? cont->m_size :
    (h->m_versions ? h->m_versions->m_data.var.size + sizeof(size_t) : 0);
  *lpb_error = h->lockObject(this, updateLen);
    if (*lpb_error == e_ok && updateLen > 0) {
        undoList()->addUpdate(h);
        m_objectsUpdated = true;
    }
  return S_OK;
}

HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::LockSharedObj(
                                      unsigned char  * lpConsistentView,
                                      unsigned char  * lpContainerId,
                                      SAPDB_UInt8     * lpoid,
                                      unsigned char  * lpobjseq,
                                      short          * lpb_error)
{
  // TODO
	 CHECK_TID_READ(lpConsistentView)

  tgg00_FileId *cid = (tgg00_FileId *) lpContainerId;
  LVCSim_ContainerMeta *cont = getContainer(*cid, (OMS_ObjectId8*)lpoid, lpb_error);
  if(*lpb_error != e_ok)
    return S_OK;

  LVCSim_ObjectHandle *h = (LVCSim_ObjectHandle*) cont->m_alloc->
    map(reinterpret_cast<LVCSim_OIDObject*>(lpoid)->getCurrentOid());
  if (!h || h->getGeneration() != ((OMS_ObjectId8*) lpoid)->getGeneration()) {
    *lpb_error = e_object_not_found;
    return S_OK;
  }

  *lpb_error = h->lockObjectShared(this, reinterpret_cast<const tgg91_PageRef*>(
        lpobjseq)->gg91IsDummyRef());
   /* if (*lpb_error == e_ok && updateLen > 0) {
        undoList()->addUpdate(h);
    }*/
  return S_OK;
}

HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::LockSharedObjMass(
                                        tsp00_Int4              &cnt,
                                        tgg91_TransNo           *pConsistentView,
                                        tgg01_ContainerId      **ppContainerId,
                                        SAPDB_UInt8              *pOid,
                                        tgg91_PageRef           *pObjSeq,
                                        tsp00_Int8              &timeout,
                                        tsp00_Int4              &errCnt,
                                        tgg00_BasisError        *pDBError)
{
  errCnt = 0;
  for (int i=0; i<cnt; ++i){
    LockSharedObj(
      (unsigned char*) pConsistentView,
      (unsigned char*) ppContainerId[i],
      &pOid[i],
      (unsigned char*) &pObjSeq[i],
      &pDBError[i]);
    if (pDBError[i] != e_ok){
      ++errCnt;
    }

    if (timeout > 0 && pDBError[i] == e_OMS_request_timeout){   // PTS 1124935
      timeout = -1;
      cnt     = i+1;
      break;
    }
  }

  return S_OK;
}



HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::Signal(
  tsp00_TaskId TaskId, 
  tsp00_Bool ok)
{
  // TODO
  return S_OK;
}


HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::Wait(
  tsp00_Bool& ok)
{
  // TODO
  ok = false;
  return S_OK;
}


HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::ExistsContainer(
    tsp00_Int4            schemaId,
    const GUID           &guid,
    OmsContainerNo        containerNo,
    size_t                objectSize,
    size_t                keyLen,
    tgg01_ContainerId    &containerId,
    bool                 &useCachedKeys, 
    bool                 &partitionedKeys,
    short                &error) // PTS 1110334
{
  OMS_ContainerHandle id; // PTS 1124356 FF 2003-09-23 new FileDir
  
  LVCSim_ContainerMeta *m = containerDir->find(LVCSim_ContainerID(guid, schemaId, containerNo), id);
  if (m != NULL) {
    error = m->lockContainer(this, false /*share*/);
    if(error == e_OMS_request_timeout)
      return S_OK;
    else if(error != e_ok){
      error = e_OMS_sysinfo_not_found;
      return S_OK;
    }
    if (objectSize != 0 && m->m_size != objectSize){
      //Check that objectsize is still the same     
        error = e_illegal_object_length;
        return S_OK;
    }
    // copy container info
    SAPDB_MemCopyNoCheck(&containerId, &m->m_containerDesc, sizeof(tgg00_FileId));

    useCachedKeys   = m->m_useCachedKeys; 
    partitionedKeys = m->m_keyPartitioned;
  } 
  else {
    error = e_OMS_sysinfo_not_found;  // PTS 1124935
  }

  return S_OK;
}


HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::LockContainer(
  const tgg00_FileId &containerId,
  short              &error)
{
  // getContainer acquires a shared lock on the container
  getContainer(*const_cast<tgg00_FileId*>(&containerId), &error);
  if (error == -108){
    error = e_container_dropped;
  }
  return S_OK;
}



HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::GetSessionHeapUsage(
  tsp00_8ByteCounter& heapUsage)
{
  // TODO
  heapUsage = 1;
  return S_OK;
} 


HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::EvalCallStatistics(
     bool            AllocatorOnly,
     OmsMonitorData &MonitorData) 
{
  // TODO
  return S_OK;
}

HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::SimCtlDumpToHtml(
  OmsHandle     &handle,
  SAPDB_UInt8    obj,
  class OmsSimDumpCollector &str)
{
  const char *data;
  SimCtlGetHTMLRef(handle, obj, &data, 3);
  str.printf(" %s ", data);
  return S_OK;
}

HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::SimCtlGetHTMLRef(
  class OmsHandle       &handle,
  SAPDB_UInt8           obj,
  const char          **data,
  int             flags)
{
  static char htmlref[512];

  OMS_ObjectId8& oid = (OMS_ObjectId8&)obj;
  unsigned int   pno  = oid.getPno();
  unsigned short ppos = oid.getPagePos();
  unsigned short gen  = oid.getGeneration();
  
  sprintf(htmlref, "oid[<A HREF=\"lc%u-%u-%u-%d.html\">%u.%u;%u</A>]",
    pno, ppos, gen, dumpSequence, pno, ppos, gen);
  if (dumpSequence > 0) {
    sprintf(htmlref + strlen(htmlref), "[<A HREF=\"lc%u-%u-%u-%d.html\">&lt;&lt;</A>]",
      pno, ppos, gen, dumpSequence - 1, pno, ppos, gen);
  }
  if (dumpSequence >= 0) {
    sprintf(htmlref + strlen(htmlref), "[<A HREF=\"lc%u-%u-%u-%d.html\">&gt;&gt;</A>]",
      pno, ppos, gen, dumpSequence + 1, pno, ppos, gen);
  }

  *data = htmlref;
  return S_OK;
}

HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::SimCtlSetObjectName(
  OmsHandle     &handle,
  SAPDB_UInt8    obj,
  const char    *name)
{
  // TODO
  return S_OK;
}

HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::SimCtlGetObjectName(
  OmsHandle     &handle,
  SAPDB_UInt8    obj,
  const char   **name)
{
  // TODO: return class GUID or name
  *name = "";
  return S_OK;
}

HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::SimCtlDestroyHandleCB(
  class OmsHandle       &handle,
  struct IliveCacheSink   **session)
{
  //bool doClear = (((LVCSim_liveCacheSink*)*session)->GetRefCount() == 1);
  LVCSim::Detach(handle);
  //if (doClear) *session = NULL;
  return S_OK;
}

long LVCSim_liveCacheSink::omsSimDumpContainer(OmsHandle &handle, FILE *out,
  LVCSim_liveCacheSink *tid, const char *datetime,
  OMS_LibOmsInterfaceClassInfo &cinfo)
{
  char buf[64];
  sprintf(buf, "lvcsimdump/lcc%d.html", cinfo.handle.GetOldContainerHandle());
  FILE *co = fopen(buf, "w");
  if (!co) {
    fprintf(stderr, "Error opening dump file '%s' for container %d\n", 
      buf, cinfo.handle.GetOldContainerHandle());
    return 0;
  }

  char guidstr[64];
  sprintf(guidstr, "%08x-%04x-%04x-%02x%02x%02x%02x%02x%02x%02x%02x", 
    cinfo.guid.Data1, cinfo.guid.Data2, cinfo.guid.Data3, cinfo.guid.Data4[0],
    cinfo.guid.Data4[1], cinfo.guid.Data4[2], cinfo.guid.Data4[3], cinfo.guid.Data4[4],
    cinfo.guid.Data4[5], cinfo.guid.Data4[6], cinfo.guid.Data4[7]);

  //printf("Dumping container: %d, size: %d\n", cinfo.handle, cinfo.persSize);

  LVCSim_ContainerMeta *m = getContainer(cinfo.handle);
  long objsize = 0; // size of all objects
  int count = 0, visible = 0, txcount = 0;
  int keysize = m ? (m->m_tree ? cinfo.keyLen : 0) : 0;   // PTS 1122540

  fprintf(co, "<HTML><BODY><H1>%s</H1><P>Dump generated %s<BR>Persistent size: ", 
    cinfo.name, datetime);
  if (cinfo.isVarObject) {
    fprintf(co, "variable");
  } else {
    fprintf(co, "%dB", cinfo.persSize);
  }
  fprintf(co, ", key size: %dB<BR>[<A HREF=\"lvcsim.html\">all "
    "containers</A>]</P><OL>\n", keysize);

  OmsDumpClsIterator iter(OmsDumpObject::omsAllOids(handle, 
        (int)cinfo.guid.Data1,cinfo.isVarObject, cinfo.schema, cinfo.container));
  while (iter) {
    const OmsAbstractObject *obj = NULL;
    const OMS_ObjectId8 &oid = iter();
    LVCSim_CStr data;
    size_t varsize = 0;
    if (cinfo.isVarObject) {
      const char *ref;
      tid->SimCtlGetHTMLRef(handle, (SAPDB_UInt8&)oid, &ref, 3);
      varsize = handle.omsGetVarObjectSize(*reinterpret_cast<const OmsVarOid*>(&oid));
      fprintf(co, "<LI>%s: size = %dB\n", ref, varsize);
    } else {
      obj = iter.omsDeRef();
      obj->omsSimDumpToHtmlShort(handle, data);
      fprintf(co, "<LI>%s\n", data.str());
    }

    char fname[256];
    sprintf(fname, "lvcsimdump/lc%u-%u-%u-%d.html", oid.getPno(), oid.getPagePos(), 
      oid.getGeneration(), dumpSequence);
    FILE *loc = fopen(fname, "w");
    if (loc) {
      fprintf(loc, "<HTML><BODY><H1>%s</H1><P>Dump generated %s "
        "[<A HREF=\"lcc%d.html\">list all</A>]</P>\n",
        cinfo.name, datetime, cinfo.handle.GetOldContainerHandle());
      data.flush();
      if (obj) {
        // normal object
        obj->omsSimDumpToHtmlLong(handle, data);
        fprintf(loc, "<P>%s</P>\n", data.str());
      } else {
        // variable-sized object - dump data
        const char *ref;
        tid->SimCtlGetHTMLRef(handle, (SAPDB_UInt8&)oid, &ref, 3);
        fprintf(loc, "<P>%s Object size: %dB (0x%xB)</P>\n", ref, varsize, varsize);
        unsigned char buf[16384];
        if (varsize > 16384) {
          fprintf(loc, "<P><B>Truncating: Actual size: %dB, printing only "
            "first 16KB</B></P>\n", varsize);
          unsigned char *tmpbuf = new unsigned char[varsize];
          handle.omsLoadVarObject(*reinterpret_cast<const OmsVarOid*>(&oid), 
            (unsigned long) varsize, tmpbuf);
          varsize = 16384;
          SAPDB_MemCopyNoCheck(buf, tmpbuf, 16384);
          delete[] tmpbuf;
        } else {
          handle.omsLoadVarObject(*reinterpret_cast<const OmsVarOid*>(&oid), 
            (unsigned long) varsize, buf);
        }
        handle.omsReleaseVarObject(*reinterpret_cast<const OmsVarOid*>(&oid));

        fprintf(loc, "<P><PRE>\n");
        bool star = false;
        for (int i = 0; i < (int) varsize; )
        {
          int j;
          if (i && i + 16 < (int) varsize) {
            // test for 0-row
            bool nulls = true;
            for (j = 0; j < 16; ++j) {
              if (buf[i+j]) {
                nulls = false;
                break;
              }
            }
            if (nulls) {
              if (!star) {
                star = true;
                fputs("*\n", loc);
              }
              i += 16;
              continue;
            }
          }
          star = false;
          fprintf(loc, "%04x: ", i);
          for (j = 0; j < 16 && i < (int) varsize; ++j) {
            fprintf(loc, "%02x ", (unsigned int) buf[i]);
            ++i;
          }
          i -= j;
          while (j < 16) {
            fprintf(loc, "   ");
            ++j;
          }
          for (j = 0; j < 16 && i < (int) varsize; ++j) {
            char c = buf[i];
            if (c < 32 || c > 126) c = '.';
            switch (c) {
            case '<':
              fputs("&lt;", loc);
              break;
            case '>':
              fputs("&gt;", loc);
              break;
            case '&':
              fputs("&amp;", loc);
              break;
            default:
              fputc(c, loc);
              break;
            }
            ++i;
          }
          fprintf(loc, "\n");
        }
        fprintf(loc, "</PRE></P>");
      }
      fprintf(loc, "</BODY></HTML>\n");
      fclose(loc);
    } else {
      fprintf(co, " <B>ERROR opening objdump file '%s'</B>", fname);
    }
    txcount++;
    ++iter;
  }

  if (m) {
    unsigned int oid = m->getAlloc()->getNextOid(0);
    while (oid) {
      LVCSim_ObjectHandle *h = reinterpret_cast<LVCSim_ObjectHandle*>(m->getAlloc()->map(oid));
      int hist;
      LVCSim_ObjectVerHandle *v = h->m_versions;
      while (v) {
        if (cinfo.isVarObject) objsize += (long) v->m_data.var.size;
        else objsize += (long) cinfo.persSize;
        v = v->m_next;
      }
      v = h->lookupObject(tid, hist);
      ++count;
      if (v) {
        ++visible;
        //LVCSim_CStr data;
        //h->omsSimDumpToHtmlShort(data);
        //fprintf(co, "<LI><A HREF=\"\">[%u;%u;%u]</A>: %s\n", 
        //  h->getPageNr(), h->getPagePos(), h->getGeneration(), data.str());
      }
      oid = m->getAlloc()->getNextOid(oid);
    }
  } else {
    fprintf(co, "<P>ERROR: Cannot look up persistent container with ID %d</P>\n",
      cinfo.handle.GetOldContainerHandle());
  }

  char schemaname[MAX_SCHEMA_NAME_LENGTH_CO10 + 1];
  handle.omsGetSchemaName(cinfo.schema, schemaname, MAX_SCHEMA_NAME_LENGTH_CO10);
  fprintf(out, "<TR><TD ALIGN=\"right\">%d</TD><TD>%s (%d)</TD>"
    "<TD ALIGN=\"right\">%d</TD><TD><A HREF=\"lcc%d.html\">%s</A></TD><TD>%s</TD>"
    "<TD ALIGN=\"right\">%d</TD><TD ALIGN=\"right\">%d</TD><TD ALIGN=\"right\">%d</TD>"
    "<TD ALIGN=\"right\">%d</TD><TD ALIGN=\"right\">%d</TD></TR>\n",
    cinfo.handle.GetOldContainerHandle(), schemaname, cinfo.schema, cinfo.container,
    cinfo.handle.GetOldContainerHandle(), cinfo.name, 
    guidstr, txcount, count, visible, cinfo.persSize, keysize);

  fprintf(co, "</OL><P>Visible %d of %d persistent objects (%d visible in TX); total "
    "persistent size: %ld B.</P></BODY></HTML>\n", visible, count, txcount, objsize);
  fclose(co);
  return objsize;
}

void LVCSim_liveCacheSink::SetDumpCount(int param)
{
  dump_count = param;
}

void LVCSim_liveCacheSink::omsSimDump(OmsHandle &handle, int param)
{
  if (dump_count <= 0) return;
  --dump_count;

  if (param < 0) dumpSequence = -1;
  else if (param == 0) dumpSequence = 0;
  else ++dumpSequence;

  if (dumpSequence <= 0) {
    // TODO: remove all old files
  }

#ifdef  WIN32
  _mkdir("lvcsimdump");
#else
  mkdir("lvcsimdump", 0777);
#endif

  FILE *out = fopen("lvcsimdump/lvcsim.html", "w");
  if (!out) {
    fprintf(stderr, "Error opening lvcsim.html, cannot write dump file\n");
    return;
  }

  time_t timedata = time(NULL);
  const char *datetime = ctime(&timedata);
  SubTxList *subTxList = m_subTxList;
  int subtxlevel = 0, txcount = 0;
  while (subTxList) {
    ++subtxlevel;
    subTxList = subTxList->m_next;
  }
  ConsViewList *txlist = consViewList;
  while (txlist) {
    ++txcount;
    txlist = txlist->m_next;
  }

  fprintf(out, "<HTML><BODY><H1>liveCache Simulator Dump</H1><P>Dump generated %s</P>\n"
    "<P>Current transaction counter: <B>%d</B>, oldest consistent view: <B>%d</B><BR>\n"
    "Current consistent view: <B>%d</B>, subtransaction level: <B>%d</B><BR>\n"
    "Number of consistent views: <B>%d</B>, dump version: <B>%d</B></P>\n",
    datetime, txCounter, (txOldest == LVCSim_TID_MAX) ? -1 : txOldest,
    m_readView, subtxlevel, txcount, dumpSequence + 1);

  fprintf(out, "<H2>Containers</H2>\n"
    "<TABLE BORDER=1><TR><TH>ID</TH><TH>Schema</TH><TH>CNo</TH><TH>Name</TH>"
    "<TH>GUID</TH><TH>TX visible</TH><TH>Pers.count</TH><TH>Pers.visible</TH>"
    "<TH>Size</TH><TH>Keysize</TH></TR>\n");

  // TODO
  //OMS_LibOmsInterfaceClassInfo cinfo;
  //bool hasMore = omsInterface->GetFirstClassInfo(cinfo);
  //long memsize = 0;

  //while (hasMore) {
  //  memsize += omsSimDumpContainer(handle, out, this, datetime, cinfo);
  //  hasMore = omsInterface->GetNextClassInfo(cinfo);
  //}

  //fprintf(out, "</TABLE>\n"
  //  "<P>Total size of allocated memory (netto): %d</P>\n"
  //  "<P>List of active m_versions:</P><UL>\n", memsize);

  {
    tsp00_C24          versionId;
    tsp00_Date         createDate;
    tsp00_Time         createTime;
      tsp00_Date         openDate;
      tsp00_Time         openTime;
      tgg91_TransNo      consistentView;
      tsp00_Bool         isMarked;
      tsp00_Bool         isOpen;
      tsp00_Bool         isUnloaded;
    tsp00_8ByteCounter heapUsage;
    tsp00_Int4         hashSize;
    tgg00_FileId       versionTree;
    tgg00_FileId       versionInvTree;
    tsp00_C512         versionDesc;
    short              error;  // PTS 1126695

    tsp00_Bool next = omsInterface->GetFirstVersion(this, versionId, createDate, 
      createTime, openDate, openTime, consistentView, isMarked, isOpen, 
      isUnloaded, heapUsage, hashSize, versionTree, versionInvTree, versionDesc, error); 
    if (error){
      fprintf(out, "ERROR while reading version dictionary: %d\n", error);
    }

    while (next) {
      fprintf(out, "<LI>%s (created %.8s/%.6s, open %d(%.8s/%.6s), view: %d, "
        "heap: %ld, hash: %d)\n", (const char*) versionId,
        (const char*) createDate, ((const char*) createTime) + 2,
        isOpen ? 1 : 0, (const char*) openDate, ((const char*) openTime) + 2, 
        *reinterpret_cast<int*>(&consistentView), (long) heapUsage,
        (int) hashSize);
      next = omsInterface->GetNextVersion(this, versionId, createDate, 
        createTime, openDate, openTime, consistentView, isMarked, isOpen, 
        isUnloaded, heapUsage, hashSize, versionTree, versionInvTree, versionDesc, error); 
      if (error){
        fprintf(out, "ERROR while reading version dictionary: %d\n", error);
      }
    }
  }

  fprintf(out, "</UL><P>List of active consistent views:</P><OL>\n");
  ConsViewList *t = consViewList;
  while (t) {
    fprintf(out, "<LI>View #%d\n", (int) t->m_tid);
    t = t->m_next;
  }
  fprintf(out, "</OL><P>List of registered schemas:</P><UL>\n");

  LVCSim_FastHash<LVCSim_liveCacheSink::SchemaName, int,
    LVCSim_SchemaMeta, false>::Iterator schemaIter(schemaDir->begin());
  while (schemaIter.isValid()) {
    char name[MAX_SCHEMA_NAME_LENGTH_CO10 + 1], *pd = name;
    const OmsTypeWyde *ps = schemaIter.currentItem().m_name;
    while (*ps) *pd++ = (char) (*ps++);
    *pd = 0;
    fprintf(out, "<LI> %s (%d)\n", name, schemaIter.currentID());
    ++schemaIter;
  }

  fprintf(out, "</UL></BODY></HTML>\n");
  fclose(out);
}

void LVCSim_liveCacheSink::countObjects(OmsHandle &handle, const OmsSchemaHandle schema, int &objCount, size_t &objSize, int &varObjCount, size_t &varObjSize)
{
  //OMS_LibOmsInterfaceClassInfo cinfo;
  
  objCount = 0;
  objSize = 0;
  varObjCount = 0;
  varObjSize = 0;

  //bool hasMore = omsInterface->GetFirstClassInfo(cinfo);
  LVCSim_liveCacheSink *sink = (LVCSim_liveCacheSink *) LVCSim::GetCurrentSink();
  LVCSim_FastHash<LVCSim_ContainerID, OMS_ContainerHandle,LVCSim_ContainerMeta>::Iterator iterator 
    = sink->containerDir->begin();

  for( ; iterator.isValid(); ++iterator) {
    LVCSim_ContainerMeta &meta = *iterator;
    LVCSim_ContainerID &key = iterator.currentKey();
    if( schema == 0 || schema == key.schema) {
      OmsDumpClsIterator iter(OmsDumpObject::omsAllOids(handle, 
      (int)key.guid.Data1, meta.m_isVarObject, key.schema, key.cno));   
      for( ; iter; ++iter) {  
        if( meta.m_isVarObject){
          ++varObjCount;
          const OMS_ObjectId8 oid = iter();
          varObjSize += handle.omsGetVarObjectSize(*reinterpret_cast<const OmsVarOid*>(&oid));
        }
        else {
          ++objCount;
          objSize += meta.m_size;
        }
      }
    }
  }
}


HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::IsInRegion(int    regionId, 
                                                           bool  &inRegion,
                                                           short &error)
{
    // must work correctly only if version can be unloaded in simulator
    // TODO: check, if we are really in the region
    inRegion = false;
    error    = 0;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::AttachSequence(const tsp00_KnlIdentifier& sequenceName,
                                         tgg00_Surrogate&           sequenceId,
                                         tgg00_SeqInfo&             sequenceInfo,
                                         short&                     sqlError) 
{
    RTESync_LockedScope lck(sequenceLock);
    LVCSim::DebugAllocatorEscape(LVCSIM_DA_ENTER_NOLEAK);
    LVCSIM_STD pair<LVCSim_SequenceMap::iterator, bool> ptr = sequenceMap->
        insert(LVCSim_SequencePair(sequenceName, LVCSim_Sequence()));
    LVCSim::DebugAllocatorEscape(LVCSIM_DA_EXIT_NOLEAK);

    *reinterpret_cast<LVCSim_Sequence**>(&sequenceInfo) = &ptr.first->second;
    sqlError = e_ok;
    return S_OK;
}


HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::NextVal(const tgg00_Surrogate& sequenceId,
                                  tgg00_SeqInfo&         sequenceInfo,
                                  tsp00_Uint4&           nextVal,
                                  short&                 sqlError) 
{
    nextVal = (*reinterpret_cast<LVCSim_Sequence**>(&sequenceInfo))->getNextValue();
    sqlError = 0;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::IsUnicodeInstance(bool& isUnicodeInstance, short& sqlError)
{
    isUnicodeInstance = false;
    sqlError          = 0;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::VersionDictInsert(
    const OmsVersionId  &versionId,
    const OMS_Context   *pVersionContext,
    tgg00_BasisError    &error) 
{ return S_OK; }

HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::VersionDictFind(
    const OmsVersionId   &versionId,
    OMS_Context         **ppVersionContext) 
{ return S_OK; }

HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::VersionDictDrop(
    const OmsVersionId  &versionId,
    tgg00_BasisError    &error) 
{ return S_OK; }

HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::VersionDictShutdown() 
{ return S_OK; }

HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::VersionDictUnloadOldestVersion(
  bool &unloaded) 
{ return S_OK; }

HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::VersionDictMarkNotUnloadable(
  OMS_Context *pVersionContext)
{ return S_OK; }

HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::VersionDictMarkUnloadable(
  OMS_Context *pVersionContext)
{ return S_OK; }

HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::VersionDictCreateIter(
  OMS_LockMode   lockMode, 
  void         **ppIter, 
  OMS_Context  **ppContext)
{ return S_OK; }

HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::VersionDictGetNext(
  void         *pIter, 
  OMS_Context **ppContext)
{ return S_OK; }

HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::VersionDictDestructIter(
  void  *pIter)
{ return S_OK; }

int STDMETHODCALLTYPE LVCSim_liveCacheSink::GetSinkType()
{
  return 1;
}

HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::SetTransactionComment(
  tsp00_Int2        commentLength,
  const OmsTypeWyde *pComment,
  short             &Error)
{ return S_OK; }

HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::IsTestKnlBadAllocActive(bool &active)
{
  active = false;
  return S_OK;
}

HRESULT STDMETHODCALLTYPE LVCSim_liveCacheSink::ResetMonitor()
{
  return S_OK;
}


/*!
 * Unlock the Schema
 */
void LVCSim_SchemaLockChainItem::unlock(LVCSim_SID sid){
  RTESync_LockedScope lck(LVCSim_SchemaMeta::m_objLock); 
  if(m_isShare) {
    // unlock shared lock 
    LVCSim_SharedLockListItem *curr = m_correspondingObjectLock;
    if(curr){
      if(curr->m_prev)
        curr->m_prev->m_next = curr->m_next;
      if(curr->m_next)
        curr->m_next->m_prev = curr->m_prev;
      if(!curr->m_prev)
        m_schemaMeta->m_sharedLockList = curr->m_next;
      delete curr;
    }
  }
  if (m_isExclusive){
    m_schemaMeta->m_correspondingLockChainItem = NULL;
    m_schemaMeta->m_history = NULL;
    // unlock
    m_schemaMeta->m_lock = NULL;
    if (m_schemaMeta->isMarkAsDropped()) {
      // is already cleared, remove from schema directory    
      if( sid->schemaDir->find(m_schemaMeta->m_id) == m_schemaMeta)
        sid->schemaDir->remove(m_schemaMeta->m_id);
      delete m_schemaMeta;
    }
   
  }
}

/*!
 * Unlock the Container
 */
void LVCSim_ContainerLockChainItem::unlock(LVCSim_SID sid){
  RTESync_LockedScope lck(LVCSim_ContainerMeta::m_objLock);
  if(m_isShare) {
    // unlock shared lock
    LVCSim_SharedLockListItem *curr = m_correspondingObjectLock;
    if(curr){
      if(curr->m_prev)
        curr->m_prev->m_next = curr->m_next;
      if(curr->m_next)
        curr->m_next->m_prev = curr->m_prev;
      if(!curr->m_prev)
        m_containerMeta->m_sharedLockList = curr->m_next;
      delete curr;
    }
  }
  if (m_isExclusive){
      m_containerMeta->m_correspondingLockChainItem = NULL;
      m_containerMeta->m_history = NULL;
      // unlock
      m_containerMeta->m_lock = NULL;
      if (m_containerMeta->isMarkAsDropped()) {
        // is already cleared, remove from container directory   
        OMS_ContainerHandle handle = m_containerMeta->getContHandle();
        if( sid->containerDir->find(handle) == m_containerMeta)
          sid->containerDir->remove(handle);
        LVCSim_ContainerMeta::addDropped(handle,sid->txCounter,sid->m_commitTime,m_containerMeta);
      } 
  }
}

// PTS 1124356 FF 2003-09-23 new FileDir
LVCSim_ContainerMeta::LVCSim_ContainerMeta(OMS_ContainerHandle newHandle, size_t _size, LVCSim_ContainerID contId,
  tgg00_ObjColDesc *coldesc, bool isVarObject, bool useCachedKeys, bool keyPartitioned) 
  : m_size(_size), m_colDesc(*coldesc), m_containerId(contId), m_tree(NULL), m_isVarObject(isVarObject), m_useCachedKeys(useCachedKeys),
  m_keyPartitioned(keyPartitioned),m_lock(NULL), m_isDropped(false),
  m_history(NULL),m_sharedLockList(NULL),m_correspondingLockChainItem(NULL)
{
  memset(&m_containerDesc, 0, sizeof(m_containerDesc));
  m_containerDesc.fileTfn_gg00().becomes(tfnObj_egg00);
  m_containerDesc.fileDirFileId_gg00() = *reinterpret_cast<const tsp00_C8*>(&newHandle); 

  if (isVarObject) {
    m_containerDesc.fileObjFileType_gg00().becomes(oftVarLenObjFile_egg00);
    m_size = 0;
  } else {
    m_containerDesc.fileObjFileType_gg00().becomes(oftFixLenObjFile_egg00);
  }

    // PTS 1124356 FF 2003-09-23 new FileDir
  m_alloc = new LVCSim_OIDAllocator(newHandle.GetOldContainerHandle(), sizeof(LVCSim_ObjectHandle));
  if (coldesc->colHashCount_gg00 > 0) {
    // has a key
    m_tree = new LVCSim_ObjectTree;
  }
}

LVCSim_ContainerMeta::~LVCSim_ContainerMeta()
{
  // delete all objects and version information
  for (int lastOid = m_alloc->getNextOid(0); lastOid; lastOid = m_alloc->getNextOid(lastOid)) {
    LVCSim_ObjectHandle *h = (LVCSim_ObjectHandle*) m_alloc->map(lastOid);
    h->destroy(this, NULL); // TODO: NULL here is problematic - locking won't work
  }
  delete m_alloc;

  // delete key m_tree
  if (m_tree) delete m_tree;
}


void *LVCSim_ContainerMeta::allocate(size_t sz)
{
  return m_alloc->allocate();
}

void LVCSim_ContainerMeta::deallocate(void *ptr)
{
  if (!ptr) return;
  m_alloc->deallocate(((LVCSim_ObjectHandle*)ptr)->getCurrentOid());
}


LVCSim_ContainerMeta::LVCSim_ContainerMeta(LVCSim_CheckpointReader &i)
{
  i.read(&m_size, sizeof(m_size));
  i.read(&m_containerDesc, sizeof(m_containerDesc));
  i.read(&m_colDesc, sizeof(m_colDesc));
  m_alloc = new LVCSim_OIDAllocator(i, m_size);

  char t;
  i.read(&t, 1);
  if (t) m_tree = new LVCSim_ObjectTree(i, m_alloc);
  else m_tree = NULL;
}

void LVCSim_ContainerMeta::writeToStream(LVCSim_CheckpointWriter &o) const
{
  o.write(&m_size, sizeof(m_size));
  o.write(&m_containerDesc, sizeof(m_containerDesc));
  o.write(&m_colDesc, sizeof(m_colDesc));
  m_alloc->writeToStream(o, m_size);

  char t;
  if (m_tree) {
    t = 1;
    o.write(&t, 1);
    m_tree->writeToStream(o);
  } else {
    t = 0;
    o.write(&t, 1);
  }
}

  void LVCSim_ContainerMeta::doEndTxLL(LVCSim_SID sid, bool commit, char operation)
  {
    LVCSim_ContainerID key;
    if(!commit) //rollback
    {
      switch (operation) {
      case o_delete:
          // only unset the drop flag
          m_isDropped = false;
          break;
      case o_new:
          // clear - no need for this object anymore, it was just created in this TX
          markDropped(sid);
          // set next container in history as current container
          if(sid->containerDir->find(getContHandle(),key)) {
            if(this->m_history){
              sid->containerDir->remove(getContHandle());
              sid->containerDir->insert(getContID(),this->m_history->getContHandle(),this->m_history);
            }
          }
          break;
      default:
          LVCSIM_ASSERT(false);
      }
    }
    else //commit
    {
      switch (operation) {
      case o_delete:
          // set next container in history as current container
          if(sid->containerDir->find(getContHandle(),key)){
            if(this->m_history){
              sid->containerDir->remove(getContHandle());
              sid->containerDir->insert(getContID(),this->m_history->getContHandle(),this->m_history);
            }
          }
          break;
      case o_new:
          // nothing to do
          break;
      default:
          LVCSIM_ASSERT(false);
      }
    }

    //LVCSim_ContainerMeta *meta = sid->containerDir->find(getContHandle());
  }

short LVCSim_ContainerMeta::lockContainer(LVCSim_SID sid, bool isExclusive)
{
  RTESync_LockedScope lck(m_objLock);
  if(isExclusive){      
    if (m_lock != sid && m_lock != NULL) {
      // TODO: now wait until the m_lock is possible, then retest
      return e_OMS_request_timeout;
    }
    if (isMarkAsDropped()) {
      return e_OMS_file_not_found;
    }
    if (m_lock == sid) {
      return e_ok;
    }
    if (m_sharedLockList != NULL){  // PTS 1131580
      if(m_sharedLockList->m_sid != sid || m_sharedLockList->m_next != NULL)
        // TODO: now wait until the m_lock is possible, then retest
        return e_OMS_request_timeout;
    }
    m_lock = sid;

    if(m_sharedLockList != NULL){ // we have a share lock and so a LockChainItem exists
      m_sharedLockList->m_correspondingLockChainItem->m_isExclusive = true;
      m_correspondingLockChainItem = m_sharedLockList->m_correspondingLockChainItem;
    }
    else {
      // add object to the transaction lock chain
      m_correspondingLockChainItem = new LVCSim_ContainerLockChainItem(this,true /*exclusive*/,NULL,sid->m_lockChain);
      sid->m_lockChain = m_correspondingLockChainItem;
    }
  }
  else {
    if (m_lock != sid && m_lock != NULL) {
      // TODO: now wait until the m_lock is possible, then retest
      return e_OMS_request_timeout;
    }
    if (isMarkAsDropped()) {
      return e_OMS_file_not_found;
    }
    for (LVCSim_SharedLockListItem *o = m_sharedLockList; o != NULL; o = o->m_next)
      if (o->m_sid == sid) //we already have a shared lock  
        return e_ok;

    // add sid to the object shared lock list
    m_sharedLockList = new LVCSim_SharedLockListItem(sid,NULL,m_sharedLockList);
    if (m_sharedLockList->m_next != NULL)
      m_sharedLockList->m_next->m_prev = m_sharedLockList;

    // add object to the transaction lock chain
    if(m_correspondingLockChainItem == NULL){
      sid->m_lockChain = new LVCSim_ContainerLockChainItem(this,false /*share*/,m_sharedLockList,sid->m_lockChain);
      m_sharedLockList->m_correspondingLockChainItem = sid->m_lockChain;
    }else {
      m_correspondingLockChainItem->m_isShare = true;
      m_correspondingLockChainItem->m_correspondingObjectLock = m_sharedLockList;
      m_sharedLockList->m_correspondingLockChainItem = m_correspondingLockChainItem;
    }
  }
  return e_ok;
}

short LVCSim_ContainerMeta::unlockContainer(LVCSim_SID sid)
{
  RTESync_LockedScope lck(m_objLock);
  if(!isLocked()){
    return e_object_not_locked; //TODO
  }
  if(isLocked() && !isLocked(sid)){ // container is locked by other transaction, can not access
    return e_OMS_request_timeout;
  }
  if (isMarkAsDropped()) {  // container is marked as dropped, can not access
    return e_OMS_file_not_found;  
  }
  // delete exclusive lock chain item of the container
  LVCSim_LockChainItem *curr = m_correspondingLockChainItem;
  m_correspondingLockChainItem = NULL;
  if(curr != NULL && curr->m_isExclusive){
    if(curr->m_isShare)
      curr->m_isExclusive = false;
    else{
      if(curr->m_prev)
        curr->m_prev->m_next = curr->m_next;
      if(curr->m_next)
        curr->m_next->m_prev = curr->m_prev;
      if(!curr->m_prev)
        sid->m_lockChain = curr->m_next;
      delete curr;
    }
  }else{
    LVCSIM_THROW("unlocking not locked container???");
	}
  m_lock = NULL;
  return e_ok;
}

short LVCSim_ContainerMeta::markDropped(LVCSim_SID dropper)
{
  RTESync_LockedScope lck(m_objLock);
  if( m_lock == NULL || m_lock != dropper){
    return e_OMS_request_timeout;
  }
  if( isMarkAsDropped()){
    return e_OMS_file_not_found;
  }
  m_isDropped = true;
  return e_ok;
}

void LVCSim_ContainerMeta::addDropped(            
            OMS_ContainerHandle &cHandle,
            LVCSim_TID tid,
            LVCSim_TID dropper,
            LVCSim_ContainerMeta *c)
{
    //RTESync_LockedScope lck(m_objLock);
    DropInfo *i = new DropInfo(cHandle, dropper, tid, c);
    if (m_lastDrop) {
        m_lastDrop->m_nextDrop = i;
        m_lastDrop = i;
    } else {
        m_firstDrop = m_lastDrop = i;
    }
}

void LVCSim_ContainerMeta::freeDropped(LVCSim_TID tid)         // free outdated containers up to tid
{
    RTESync_LockedScope lck(m_objLock);
    while (m_firstDrop) {
        DropInfo *i = m_firstDrop;
        if (i->m_dropTime >= tid) {
            break;
        }
        m_firstDrop = i->m_nextDrop;
        if (!m_firstDrop) {
            m_lastDrop = NULL;
        }
        i->m_nextDrop = NULL;
        delete i;
    }
}

void LVCSim_ContainerMeta::freeAllDropped()                    // free all outdated containers
{
    if (m_firstDrop) {
        RTESync_LockedScope lck(m_objLock);
        delete m_firstDrop;
        m_firstDrop = m_lastDrop = NULL;
    }
}

LVCSim_ContainerMeta *LVCSim_ContainerMeta::findDropped(       // find in dropped container list
        OMS_ContainerHandle &cHandle, 
        LVCSim_TID tid)
{    
    RTESync_LockedScope lck(m_objLock);
    DropInfo *i = m_firstDrop;
    while (i) {
        if (i->m_dropTime > tid) {
            if (i->m_cHandle == cHandle) {
                return i->m_meta;
            }
        }
        i = i->m_nextDrop;
    }
    return NULL;
}

void LVCSim_SchemaMeta::doEndTxLL(LVCSim_SID sid, bool commit, char operation)  // do end transaction cleanup
{
    if(!commit) //rollback
    {
      switch (operation) {
      case o_delete:
          // only unset the drop flag
          m_isDropped = false;
          break;
      case o_new:
          // clear - no need for this object anymore, it was just created in this TX
          markDropped(sid);
          // set next schema in history as new current schema
          if(sid->schemaDir->find(m_id)) {
            if(this->m_history){
              sid->schemaDir->remove(m_id);
              sid->schemaDir->insert(m_name,this->m_history->m_id,this->m_history);
            }
          }
          break;
      default:
          LVCSIM_ASSERT(false);
      }
    }
    else //commit
    {
      switch (operation) {
      case o_delete:
          // set next schema in history as new current schema
          if(sid->schemaDir->find(m_id)){
            if(this->m_history){
              sid->schemaDir->remove(m_id);
              sid->schemaDir->insert(m_name,this->m_history->m_id,this->m_history);
            }
          }
          break;
      case o_new:
          // nothing to do
          break;
      default:
          LVCSIM_ASSERT(false);
      }
    }
}

 
short LVCSim_SchemaMeta::dropAllContainer(LVCSim_SID sid)   // drop all container of this schema
{
  short error=0;
  int contCnt;
  LVCSim_FastHash<LVCSim_ContainerID, OMS_ContainerHandle,LVCSim_ContainerMeta>::Iterator iterator 
    = sid->containerDir->begin();
  for(contCnt=0; iterator.isValid(); ++iterator, ++contCnt) { //first, try to lock all container of this schema
    LVCSim_ContainerMeta &meta = *iterator;
    if( meta.getContID().schema == m_id) {
      error = meta.lockContainer(sid, true /*exclusive*/);
      if( error != e_ok)
        break;
    }
  }
  if(error != e_ok){ //not all container locked, so unlock all allready locked container
    short errorUnlock;
    iterator = sid->containerDir->begin();
    for(int cnt=0 ; iterator.isValid() && cnt < contCnt; ++iterator, ++cnt) {
      LVCSim_ContainerMeta &meta = *iterator;
      if( meta.getContID().schema == m_id) {
        errorUnlock = meta.unlockContainer(sid);
        if( errorUnlock != e_ok)
          break;
      }
    }
    if(errorUnlock != e_ok) 
      return errorUnlock;  // container could not be unlocked, return unlock error number
    else        
      return error;   // all locked container are unlocked, return lock error number
  }
  iterator = sid->containerDir->begin();
  for( ; iterator.isValid(); ++iterator) {
    LVCSim_ContainerMeta &meta = *iterator;
    if( meta.getContID().cno == m_id) {
      meta.markDropped(sid);
      sid->undoList()->addDelete(&meta);
    }
  }
  return e_ok;
}

short LVCSim_SchemaMeta::lockSchema(LVCSim_SID sid, bool isExclusive)
{
  RTESync_LockedScope lck(m_objLock);
  if(isExclusive){ 
    if (m_lock != sid && m_lock != NULL) {
      // TODO: now wait until the m_lock is possible, then retest
      return e_OMS_request_timeout;
    }
    if (isMarkAsDropped()) {
      return e_unknown_schema;
    }
    if (m_lock == sid) {
      return e_ok;
    }
    if (m_sharedLockList != NULL){  // PTS 1131580
      if(m_sharedLockList->m_sid != sid || m_sharedLockList->m_next != NULL)
        // TODO: now wait until m_lock is possible, then retest
        return e_OMS_request_timeout;
    }
    m_lock = sid;
    if(m_sharedLockList != NULL){ // we have a share lock and so a LockChainItem exists
      m_sharedLockList->m_correspondingLockChainItem->m_isExclusive = true;
      m_correspondingLockChainItem = m_sharedLockList->m_correspondingLockChainItem;
    }
    else {
      // add object to the transaction lock chain
      m_correspondingLockChainItem = new LVCSim_SchemaLockChainItem(this,true /*exclusive*/,NULL,sid->m_lockChain);
      sid->m_lockChain = m_correspondingLockChainItem;
    }
  }
  else {
    if (m_lock != sid && m_lock != NULL) {
      // TODO: now wait until the m_lock is possible, then retest
      return e_OMS_request_timeout;
    }
    if (isMarkAsDropped()) {
      return e_unknown_schema;
    }
    for (LVCSim_SharedLockListItem *o = m_sharedLockList; o != NULL; o = o->m_next){
      if (o->m_sid == sid) //we already have a shared lock  
        return e_ok;
    }

    // add sid to the object shared lock list
    m_sharedLockList = new LVCSim_SharedLockListItem(sid,NULL,m_sharedLockList);
    if (m_sharedLockList->m_next != NULL)
      m_sharedLockList->m_next->m_prev = m_sharedLockList;

    // add object to the transaction lock chain
    if(m_correspondingLockChainItem == NULL){
      sid->m_lockChain = new LVCSim_SchemaLockChainItem(this,false /*share*/,m_sharedLockList,sid->m_lockChain);
      m_sharedLockList->m_correspondingLockChainItem = sid->m_lockChain;
    }else {
      m_correspondingLockChainItem->m_isShare = true;
      m_correspondingLockChainItem->m_correspondingObjectLock = m_sharedLockList;
      m_sharedLockList->m_correspondingLockChainItem = m_correspondingLockChainItem;
    }
  }
  return e_ok;
}

short LVCSim_SchemaMeta::unlockSchema(LVCSim_SID sid)
{
  RTESync_LockedScope lck(m_objLock);
  if(!isLocked()){
    return e_object_not_locked; //TODO
  }
  if(isLocked() && !isLocked(sid)){ // container is locked by other transaction, can not access
    return e_OMS_request_timeout;
  }
  if (isMarkAsDropped()) {  // container is marked as dropped, can not access
    return e_unknown_schema;  
  }
  // delete exclusive lock chain item of the container
  LVCSim_LockChainItem *curr = m_correspondingLockChainItem;
  m_correspondingLockChainItem = NULL;
  if(curr != NULL && curr->m_isExclusive){
    if(curr->m_isShare)
      curr->m_isExclusive = false;
    else{
      if(curr->m_prev)
        curr->m_prev->m_next = curr->m_next;
      if(curr->m_next)
        curr->m_next->m_prev = curr->m_prev;
      if(!curr->m_prev)
        sid->m_lockChain = curr->m_next;
      delete curr;
    }
  }else{
    LVCSIM_THROW("unlocking not locked container???");
	}
  m_lock = NULL;
  return e_ok;
}

short LVCSim_SchemaMeta::markDropped(LVCSim_SID dropper)
{
  RTESync_LockedScope lck(m_objLock);
  if( m_lock == NULL || m_lock != dropper){
    return e_OMS_request_timeout;
  }
  if( isMarkAsDropped()){
    return e_unknown_schema;
  }
  m_isDropped = true;
  return e_ok; 
}


