/*!***************************************************************************

  @file           gkb501.h
  @author         FerdiF
  @ingroup        Locklist
  @brief          object share lock implementation

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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

#ifndef GKB501_H
#define GKB501_H

#include "hsp77.h"
#include "gkb05.h"
#include "RunTime/Synchronisation/RTESync_RWRegion.hpp"
#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp"
#include "Container/Container_List.hpp"
#include "Container/Container_Vector.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"

/*----------------------------------------------------------------------*/

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

class CShareLockSegment_kb501;
class CShareLockCntrlSegment_kb501;

/*----------------------------------------------------------------------*/

/*!
   @brief defines one entry with information needed for table
   SYSTEMVIEWS.OBJECTLOCKS  
*/
typedef struct LockInfo_kb501
{
    FileDir_FileNo        fileno;
    tgg92_KernelOid       oid;
    tsp00_TaskId          taskidOfTrans;
    pasbool               isExclusive;
    pasbool               isHolder;
    LockInfo_kb501() 
    {
		fileno = 0;
		taskidOfTrans = cgg_nil_pid;
        oid.gg92SetNil();
		isExclusive = FALSE;
		isHolder = FALSE;
    };
} LockInfo_kb501;

/*!
   @brief class used to hold information needed for table 
   SYSTEMVIEWS.OBJECTLOCKS  
*/
class CShareLockInfo_kb501 
{
public:
    CShareLockInfo_kb501(SAPDBMem_IRawAllocator *alloc)
        : m_allocator( alloc )
        , m_info( *alloc )
        , m_posInfo( 0 )
        , m_maxInfo( 0 )
        , m_posHash( 0 ) {}; 
    ~CShareLockInfo_kb501() { m_info.Delete(); };
    SAPDBMem_IRawAllocator *getAllocator() const { return m_allocator; };
    int        getInfoPos() const { return m_posInfo; };
    int        getInfoMax() const { return m_maxInfo; };
    int        getHashPos() const { return m_posHash; };
    void       getInfo(FileDir_FileNo       &fileno
                      ,tgg92_KernelOid       &oid
                      ,tgg91_TransNo         &transno
                      ,tsp00_TaskId          &taskidOfTrans
                      ,pasbool               &isExclusive
                      ,pasbool               &isHolder)
    {
        fileno        = m_info[ m_posInfo ].fileno;
        oid           = m_info[ m_posInfo ].oid;
        taskidOfTrans = m_info[ m_posInfo ].taskidOfTrans;
        isExclusive   = m_info[ m_posInfo ].isExclusive;
        isHolder      = m_info[ m_posInfo ].isHolder;
		++m_posInfo;
    }

	void	   setHashPos( int val ) { m_posHash = val; };
    int        ResetForRefill() { m_posInfo = 0; m_maxInfo = 0; return ++m_posHash; }; 
    SAPDB_Bool ResizeVector( int sz ) { return m_info.Resize( sz ); };
    bool       AddEntry( FileDir_FileNo  fileno
        ,tgg92_KernelOid       oid
        ,tsp00_TaskId          taskidOfTrans
        ,pasbool               isExclusive
        ,pasbool               isHolder) 
    {
        if ( m_maxInfo == ( m_info.GetCapacity()) ) {
            if (!m_info.Resize( m_info.GetCapacity() + 50 ))
                return false;
        }
        m_info[ m_maxInfo ].fileno        = fileno;
        m_info[ m_maxInfo ].oid           = oid;
        m_info[ m_maxInfo ].taskidOfTrans = taskidOfTrans;
        m_info[ m_maxInfo ].isExclusive   = isExclusive;
        m_info[ m_maxInfo ].isHolder      = isHolder;
        ++m_maxInfo;
        return true;
    };
private:
    SAPDBMem_IRawAllocator           *m_allocator;
    Container_Vector<LockInfo_kb501>  m_info;
    int                               m_posInfo;
    int                               m_maxInfo;
    int                               m_posHash;
};

/*!
   @brief defines one element of the transaction OID list for share locks 
   
   ( see also: type OidList defined within class COidList_kb501 )
*/
class COidListEntry_kb501 
{
public:
    /*!
    @brief default constructor for COidListEntry_kb501
    */
    COidListEntry_kb501() 
        : lockReq( NULL )
        , isShareLocked( false )
        , isExclLocked( false ) {};

    tkb05_KernelOid           oid;              // oid of the locked object
    tkb05_ObjShareLockPtr     lockReq;          // pointer to the share-lock cntrlblock
    bool                      isShareLocked;    // transaction has share lock on object
    bool                      isExclLocked;     // transaction has exclusive lock on object
};


/*!
   @brief defines the transaction OID list for share locks 

   If a transaction gets a share lock for an object, the OID of the locked object
   is inserted into an OID list maintained by each transaction.
   This OID list is used:
   - within transaction rollback, to mark all entries as 'ROLLBACKED'
   - within transaction commit, to do some garbage collection
   - to re-activate other transactions waiting for the lock to get free
*/
class COidList_kb501
{
private:
    /*!
    @brief status of transaction

    This status is used on transaction end to handle locks correctly.
    Rollbacked locks may be deleted as soon as tranaction has terminated
    Committed locks MUST NOT be deleted
    */
    enum e_transend_status { e_active   // transaction active
        ,e_rollback                     // transaction rollback occurred
        ,e_commit };                    // transaction commit occuurred

public:
    typedef Container_List<COidListEntry_kb501> OidList;

    /*!
    @brief default constructor for COidList_kb501
    @param        alloc   [in]   allocator used
    */
    inline COidList_kb501(SAPDBMem_IRawAllocator &alloc);

    /*!
    @brief default destructor for COidList_kb501
    */
    inline ~COidList_kb501();

    /*!
    @brief operator () to pointer access to OID list
    */
    inline OidList* operator () () const;

    /*!
    @brief accessory function to allocator
    */    
    inline SAPDBMem_IRawAllocator &GetAllocator() const;

    /*!
    @brief function to allocate OID list
    */
    inline bool Init(); 

    /*!
    @brief returns true if transaction status is e_commit
    */
    inline bool IsCommitted() const;

    /*!
    @brief returns true if transaction status is e_rollback
    */
    inline bool IsRollbacked() const;

    /*!
    @brief sets transaction status to e_commit
    */
    inline void SetCommit();

    /*!
    @brief sets transaction status to e_rollback
    */
    inline void SetRollback();

    /*!
    @brief checks if collision OID is NilOid
    */
    inline bool IsCollisionOidNil() const;

    /*!
    @brief gets collision OID
    */
    inline tkb05_KernelOid& GetCollisionOid();

    /*!
    @brief resets collision OID to NilOid
    */
    inline void ResetCollisionOid();
    /*!
    @brief sets collision OID
    */
    inline void SetCollisionOid(tkb05_KernelOid &oid);
private:
    SAPDBMem_IRawAllocator   &m_alloc;         // allocator used
    OidList                  *m_List;          // oid list
    e_transend_status         m_status;        // transaction commit / rollback    
    tkb05_KernelOid           m_CollisionOid;  // in case when waiting for collision to be resolved
};
/*! endclass: COidList_kb501 */

inline COidList_kb501::COidList_kb501(SAPDBMem_IRawAllocator &alloc)
        : m_alloc(alloc)
        , m_List(NULL)
        , m_status( e_active )
{
    ResetCollisionOid();
};

inline COidList_kb501::~COidList_kb501()
{
    destroy( m_List, m_alloc );
}

inline COidList_kb501::OidList* 
COidList_kb501::operator () () const 
{ 
    return m_List; 
};

inline SAPDBMem_IRawAllocator&
COidList_kb501::GetAllocator() const 
{ 
    return m_alloc; 
};

inline bool COidList_kb501::Init() 
{
    m_List = new(m_alloc) OidList(m_alloc);
    if ( m_List )
        return true;
    else
        return false;
};

inline bool COidList_kb501::IsCommitted() const 
{
    if (m_status == e_commit)
        return true;
    else
        return false;
};

inline bool COidList_kb501::IsRollbacked() const 
{
    if (m_status == e_rollback)
        return true;
    else
        return false;
};

inline void COidList_kb501::SetCommit() 
{ 
    m_status = e_commit; 
};

inline void COidList_kb501::SetRollback() 
{ 
    m_status = e_rollback; 
};

inline bool COidList_kb501::IsCollisionOidNil() const 
{
    return m_CollisionOid.gg92IsNil();
};

inline tkb05_KernelOid& COidList_kb501::GetCollisionOid()
{
    return m_CollisionOid;
};

inline void COidList_kb501::ResetCollisionOid()
{
    m_CollisionOid.gg92SetNil();
};

inline void COidList_kb501::SetCollisionOid(tkb05_KernelOid &oid)
{
    m_CollisionOid = oid;
};

/*!
@brief class used for synchronisation of share lock control structure
*/
class CShrLockSync_kb501
{
    /*!
    @brief number of spinlocks to be used for hash synchronisation
    */
    enum { used_spinlock_num = 53 };

public:
    CShrLockSync_kb501() 
    {
        int i;
        SAPDB_UTF8 name[30];
        for ( i=0; i < used_spinlock_num; i++ )
        {
            sp77sprintfUnicode( sp77encodingUTF8, &name[0], sizeof(name),
                "OmsShareLock Hash %d", i);
            m_slots[i].SetIdentifier( name );
        }
    };

    /*!
    @brief lock spinlock object for OID hash slot
    */    
    inline void Lock   (int OidhashValue);

    /*!
    @brief unlock spinlock object for OID hash slot
    */    
    inline void Unlock (int OidhashValue); 

private:
    mutable RTESync_NamedSpinlock m_slots[ used_spinlock_num ];
};
/*! endclass: CShrLockSync_kb501 */

inline void CShrLockSync_kb501::Lock (int OidhashValue) 
{
    m_slots[OidhashValue % used_spinlock_num].Lock(); 
};

inline void CShrLockSync_kb501::Unlock (int OidhashValue) 
{
    m_slots[OidhashValue % used_spinlock_num].Unlock();
};


/*!
   @brief autoptr class for CShrLockSync_kb501 pointer
*/
class CShrLockSync_autoptr_kb501 
{
public:
    /*!
    @brief constructor - doesn't aquire a lock
    @param p [in] pointer to CShrLockSync_kb501
    @param i [in] index of share lock hash 
    */    
    explicit CShrLockSync_autoptr_kb501(
        CShrLockSync_kb501 *p, int i) 
        : m_pointee(p), m_index(i), m_locked(false) 
    {};

    /*!
    @brief destructor releases lock if aquired
    */    
    inline ~CShrLockSync_autoptr_kb501();

    /*!
    @brief lock spinlock object for OID hash slot
    */    
    void Lock();

    /*!
    @brief unlock spinlock object for OID hash slot
    */    
    inline void Unlock();

private:
    CShrLockSync_kb501*  m_pointee;
    int                  m_index;
    boolean              m_locked;
};
/*! endclass: CShrLockSync_autoptr_kb501 */

inline CShrLockSync_autoptr_kb501::~CShrLockSync_autoptr_kb501() 
{ 
    if (m_pointee && m_locked) 
        m_pointee->Unlock(m_index); 
};

inline void CShrLockSync_autoptr_kb501::Lock() 
{
    if (m_pointee && !m_locked) 
    {
        m_pointee->Lock(m_index); 
        m_locked = true;
    }
}

inline void CShrLockSync_autoptr_kb501::Unlock() 
{
    if (m_pointee && m_locked) 
    {
        m_pointee->Unlock(m_index); 
        m_locked = false;
    }
}

/*!
   @brief defines global control structure used for shared locks
*/
class CSlockHash_kb501
{
public :
    CSlockHash_kb501();
private:
    /*!
    @brief allocates a tkb05_ObjShareLockCtrl struct  
    @param        Trans   [in]   tgg00_TransContext
    @return       pointer to a tkb05_ObjShareLockCtrl 
    */
    tkb05_ObjShareLockCtrlPtr  AllocLockCntrlEntry( 
        tgg00_TransContext  &Trans,
        tsp00_C8            &ObjFileNo);

    /*!
    @brief allocates a tkb05_ObjShareLockPtr struct  
    @param        Trans   [in]   tgg00_TransContext
    @return       pointer to a tkb05_ObjShareLockPtr 
    */
    tkb05_ObjShareLockPtr AllocShrLockEntry(tgg00_TransContext  &Trans);

    /*!
    @brief chain a LockCntrlEntry to CSlockHash_kb501 
    @param        hashvalue   [in]   OID hash value
    @param        pLockEntry  [in]   tkb05_ObjShareLockCtrlPtr
    @return       none
    */
    inline void ChainLockCtrlEntryToHash( 
        int                       hashvalue,
        tkb05_ObjShareLockCtrlPtr pLockEntry );

    /*!
    @brief deallocates a LockCntrlEntry  
    @param        TaskId      [in]   task identification
    @param        pLockEntry  [in]   pointer to LockCntrlEntry to be released
    @return       none
    */
    void DeallocLockCntrlEntry( 
        tsp00_TaskId              TaskId,
        tkb05_ObjShareLockCtrlPtr pLockEntry );

    /*!
    @brief deallocates a ShareLockEntry  
    @param        TaskId    [in]   task identification
    @param        currReq   [in]   pointer to ShareLockEntry to be released
    @return       none
    */
    void DeallocShrLockEntry(
        tsp00_TaskId          TaskId,
        tkb05_ObjShareLockPtr currReq );

    /*!
    @brief search LockCntrlEntry within CSlockHash_kb501 
    @param        hashvalue   [in]   OID hash value
    @param        Oid         [in]   Oid to be found
    @return       pointer to a tkb05_ObjShareLockCtrl if found
                  NULL if OID if not found
    */
    inline tkb05_ObjShareLockCtrlPtr FindLockCntrlEntry( 
        int              hashvalue,
        tkb05_KernelOid &Oid );

    /*!
    @brief checks if a tgg91_TransNo belongs to a still active transaction 
    @param        TransIndex   [in]   index of entry in global transaction list
    @param        TransId      [in]   update transno of transaction, which aquired lock
    @return       bool      true = transaction active; false = transaction not active
    */
    inline bool IsTransActive( 
        tgg00_TransIndex  TransIndex,
        tgg91_TransNo    &TransId );

    /*!
    @brief remove a LockCntrlEntry from CSlockHash_kb501 
    @param        hashvalue   [in]   OID hash value
    @param        pLockEntry  [in]   tkb05_ObjShareLockCtrlPtr
    @return       none
    */
    inline void RemoveLockCtrlEntryFromHash( 
        int                       hashvalue,
        tkb05_ObjShareLockCtrlPtr pLockEntry );

public:

    void             UnchainFromLockFreeList( CShareLockSegment_kb501* entry );
    void             UnchainFromLockCtrlFreeList( CShareLockCntrlSegment_kb501* entry );

    /*!
    @brief Destroy all share lock entries of one object 
    @param        hashvalue     [in]   OID hash value
    @param        TaskId        [in]   task identification
    @param        Oid           [in]   OID of locked object
    @return       none

    This method is called after committed transaction end for exclusively locked objects,
    which had been share locked before the exclusive lock was aquired.
    Committing such an exclusive lock causes all share lock information to become useless.
    Even the complete lock control structure may be removed, if there is no collision entry
    chained to it.    
    */
    void DestroyAllShareLockEntries( 
        int                  hashvalue,
        tsp00_TaskId         TaskId,
        tkb05_KernelOid     &Oid );

    /*!
    @brief Unlock an exclusive lock by setting objsExclLockTransId_kb05 to nil
    @param        UpdTransId_gg00 [in]   WriteTransId of transaction
    @param        hashvalue       [in]   OID hash value
    @param        oidListEntry    [in]   Ptr to entry of OID transaction lock list
    @return       transindex of chained collision entry
                  cgg_nil_transindex if no collision entry has been chained
    */
    tgg00_TransIndex UnlockExcl( 
        tgg91_TransNo       &UpdTransId_gg00,
        int                  hashvalue,
        COidListEntry_kb501 &oidListEntry);

    /*!
    @brief Unlock a share lock 
    @param        hashvalue     [in]   OID hash value
    @param        TaskId        [in]   task identification
    @param        TransIdx      [in]   transaction index into global transaction list
    @param        oidListEntry  [in]   entry of OID transaction lock list
    @param        isCommitted   [in]   boolean telling if lock has been committed 
    @return       transindex of chained collision entry
                  cgg_nil_transindex if no collision entry has been chained

    If lock hasn't been committed, it may be destroyed completely.
    If the lock has been committed, all share locks with the same TransIdx may be removed.
    */
    tgg00_TransIndex UnlockShare( 
        int                  hashvalue,
        tsp00_TaskId         TaskId,
        tgg00_TransIndex     TransIdx,
        COidListEntry_kb501 &oidListEntry,
        tgg91_TransNo       &ThisWriteTransId,
        bool                 isCommitted,
        tgg91_TransNo       &ShareLockTransId);

     /*!
    @brief Unlock a share lock 
    @param        hashvalue     [in]   OID hash value
    @param        ObjCollGlob   [in]   global control block for object collision handling  
    @param        thisTrans     [in]   global transaction entry used by transaction
    @param        CollEntry     [in]   collision control block
    @param        isCommit      [in]   boolean telling if transaction has been committed 
    @return       none
    */
    void RemoveCollisionEntry( 
        int                       hashvalue,
        tkb05_ObjCollGlob        &ObjCollGlob,
        tkb05_TransEntry         &thisTrans,
        tkb05_ObjCollisionEntry  &CollEntry,
        bool                      isCommit );

    /*!
    @brief Aquires a share lock for the object with the specified OID 
    @param        hashvalue      [in]   OID hash value
    @param        Trans          [in]   transaction context  
    @param        ObjTransInfo   [in]   transaction info of object header
    @param        Oid            [in]   OID of object to be locked
    @param        LockReq        [out]  ptr to share lock control block ( for optimization reasons )
    @param        ignColEntry    [in]   ignore collision entry for waiting requests
    @param        bAlreadyLocked [out]  boolean telling if object was already locked
    @return       none
    */
    void GetLockShare( 
        int                       hashvalue,
        tgg00_TransContext       &Trans, 
        tsp00_C8                 &ObjFileNo,
        tgg00_ObjTransInfo       &ObjTransInfo,
        tkb05_KernelOid          &Oid,
        tkb05_ObjShareLockPtr    &LockReq,
        pasbool                   ignColEntry, 
        bool                     &bAlreadyLocked );

     /*!
    @brief Aquires an exclusive lock for the object which has been shared locked before 
    @param        hashvalue       [in]   OID hash value
    @param        Trans           [in]   transaction context  
    @param        Oid             [in]   OID of object to be locked
    @param        ignShrLckCsView [in]   check only if there are active transaction ( no consistent view check )
    @param        bAlreadyLocked  [out]  boolean telling if object was already locked
    @return       collision transindex if a collision has occurred on the object
                  else cgg_nil_transindex
    */
    tgg00_TransIndex GetLockExcl( 
        int                 hashvalue, 
        tgg00_TransContext &Trans,
        tsp00_C8           &ObjFileNo,
        tkb05_KernelOid    &Oid,
        pasbool             ignShrLckCsView, 
        pasbool             ignColEntry, 
        bool               &bAlreadyLocked );

	/*!
    @brief get first entry of hashlist
    */    
	inline tkb05_ObjShareLockCtrlPtr GetHashHdrList(tsp00_Int4 ListIndex);

    /*!
    @brief get size of hash header array
    */    
    inline tsp00_Int4 GetHashHdrSize() const;

    /*!
    @brief gets the information for systemview OBJECTLOCKS
    */    
    bool GetShareLockInfo( int &hashval, CShareLockInfo_kb501 *ShrLckInfoPtr );

    /*!
    @brief Gets statistic values for share lock handling
    @param        LockCntrlEntries     [out]  number of lock control entries allocated
    @param        LockCntrlEntriesUsed [out]  number of lock control entries actually used
    @param        ShareLockEntries     [out]  number of share lock entries allocated
    @param        ShareLockEntriesUsed [out]  number of share lock entries actually used
    @param        CollisionEntriesUsed [out]  number of collision entries anchored with share lock handling
    @return       none
    */
    void  GetStatistics ( 
        int &LockCntrlEntries,
        int &LockCntrlEntriesUsed,
        int &ShareLockEntries,
        int &ShareLockEntriesUsed,
        int &CollisionEntriesUsed);

    /*!
    @brief gets the collision entry anchored within a share lock control entry
    @param        hashvalue      [in]   OID hash value
    @param        Oid            [in]   OID of locked object 
    @param        error          [out]  error code  ( e_ok, e_object_not_locked )
    @return       collision transindex if a collision has occurred on the object
                  else cgg_nil_transindex
    */
    tgg00_TransIndex GetCollisionIndex(
        int                  hashvalue,
        tkb05_KernelOid     &Oid,
        tgg00_BasisError    &error);

    /*!
    @brief Checks if an object is exclusive locked by actual transaction
    @param        hashvalue      [in]  OID hash value
    @param        Trans          [in/out]  transaction context 
                                 ( [in] trWriteTransId_gg00, [out] trError_gg00 )
    @param        Oid            [in]  OID of locked object 
    @return       none
    */
    void IsSelfLocked(
        int                  hashvalue,
        tgg00_TransContext  &Trans,
        tkb05_KernelOid     &Oid,
		pasbool             bIsSharedLocked);

    /*!
    @brief Checks if actual object is used by any transaction
    @param        hashvalue      [in]  OID hash value
    @param        TaskId         [in]  index to global transaction context 
    @param        Oid            [in]  OID of locked object 
    @return       TRUE if used by transaction, else FALSE
    */
    pasbool IsObjTransUsed (
        int               hashvalue,
        tsp00_TaskId      TaskId,
        tkb05_KernelOid  &Oid,
			  pasbool           ignoreVersions,
		    tgg91_TransNo    &UpdTransId_gg00,
		    tgg91_TransNo    &ActTransNo);

    /*!
    @brief Computes hash value for an OID
    @param        Oid    [in]  OID  
    @return       hashvalue of OID
    */
    inline int HashValue( tkb05_KernelOid &Oid ) const;

    /*!
    @brief Remove a collision entry from share lcok control block
    @param        hashvalue      [in]  OID hash value
    @param        CollEntry      [in]  collision control block  
    @return       index of collision entry remove
                  else cgg_nil_transindex
    */
    tgg00_TransIndex RemoveCollisionEntry( 
        int                      hashvalue, 
        tkb05_ObjCollisionEntry &CollEntry );

    /*!
    @brief Store a collision entry into share lock control block
    @param        hashvalue      [in]  OID hash value
    @param        CollEntry      [in]  collision control block  
    @return       none
    */
    void StoreCollisionEntry( 
        int                      hashvalue, 
        tkb05_ObjCollisionEntry &CollEntry);

    /*!
    @brief Set new minimal transaction for garbagecollection
    @param        TaskId      [in]  Task ID
    @param        MinTrans    [in]  minimal transaction
    @return       none
    */
    void SetMinTrans ( 
        tsp00_TaskId    TaskId,
        tgg91_TransNo  &MinTrans );

    /*!
    @brief Set new minimal transaction for garbagecollection
    @param        TaskId      [in]  Task ID
    @param        MinTrans    [in]  minimal transaction
    @return       none
    */
    void CollectGarbage( 
        tsp00_TaskId    TaskId,
        tgg91_TransNo  &MinTrans );
private :
    enum {  HashHdrSize    = 1013 };

    struct CHashSlot
    {
        tkb05_ObjShareLockCtrlPtr   m_List;
        int            m_ListEntryCnt;
        CHashSlot() 
            : m_List(NULL)
            , m_ListEntryCnt(0)
        {};
    };

    CHashSlot                      m_Head[HashHdrSize];
    CShareLockCntrlSegment_kb501  *m_LockCntrlSupplySegments;
    CShareLockSegment_kb501       *m_ShareLockSupplySegments;
    int                            m_CollisionEntryUseCnt;
    tgg91_TransNo                  m_MinTrans;
    mutable RTESync_NamedSpinlock  m_SupplySegmentsShare_Lock;
    mutable RTESync_NamedSpinlock  m_SupplySegmentsCntrl_Lock;
    mutable RTESync_NamedSpinlock  m_StatisticCounter_Lock;
};
/*! endclass: CSlockHash_kb501 */

/*----------------------------------------------------------------------*/

inline void CSlockHash_kb501::ChainLockCtrlEntryToHash( 
    int                        hashvalue,
    tkb05_ObjShareLockCtrlPtr pLockEntry )
{
    pLockEntry->objsNxtShareLockCtrl_kb05  = m_Head[ hashvalue ].m_List;
    m_Head[ hashvalue ].m_List              = pLockEntry;
    m_Head[ hashvalue ].m_ListEntryCnt++;
};

inline tkb05_ObjShareLockCtrlPtr 
CSlockHash_kb501::GetHashHdrList(tsp00_Int4 ListIndex) {
	return m_Head[ ListIndex ].m_List;
};

inline tsp00_Int4 
CSlockHash_kb501::GetHashHdrSize() const
{
	return HashHdrSize;
};


inline bool CSlockHash_kb501::IsTransActive( 
    tgg00_TransIndex  TransIndex,
    tgg91_TransNo    &TransId )
{
    tkb05_TransEntryPtr ThisTrans;
    if (cgg_nil_transindex == TransIndex)
        return false;
    k52acc_trans_entry( TransIndex, ThisTrans );
    if (( TransId == ThisTrans->teWriteTransId_kb05 ) &&
        ( m_delete != ThisTrans->teState_kb05 ))
        return true;
    else
        return false;
}

inline void CSlockHash_kb501::RemoveLockCtrlEntryFromHash( 
    int                        hashvalue
    ,tkb05_ObjShareLockCtrlPtr pLockEntry )
{
    if ( pLockEntry == m_Head[ hashvalue ].m_List )
        m_Head[ hashvalue ].m_List = pLockEntry->objsNxtShareLockCtrl_kb05;
    else
    {
        tkb05_ObjShareLockCtrlPtr curr = m_Head[ hashvalue ].m_List;
        while ( curr && ( curr->objsNxtShareLockCtrl_kb05 != pLockEntry ))
            curr = curr->objsNxtShareLockCtrl_kb05;
        if ( curr )
            curr->objsNxtShareLockCtrl_kb05 = pLockEntry->objsNxtShareLockCtrl_kb05;
    }
    m_Head[ hashvalue ].m_ListEntryCnt--;
    pLockEntry->objsNxtShareLockCtrl_kb05 = NULL;
}

/*----------------------------------------------------------------------*/

inline tkb05_ObjShareLockCtrlPtr CSlockHash_kb501::FindLockCntrlEntry( 
    int               hashvalue
    ,tkb05_KernelOid &Oid )
{
    tkb05_ObjShareLockCtrlPtr curr = m_Head[ hashvalue ].m_List;
    while (curr)
    {
        if (curr->objsOid_kb05.gg92IsEqIgnoreFrameVers(Oid))
            return curr;
        else
            curr = curr->objsNxtShareLockCtrl_kb05;
    }
    return NULL;
};

/*----------------------------------------------------------------------*/
inline int 
CSlockHash_kb501::HashValue(tkb05_KernelOid &Oid) const
{
    // normal OID: all oids with the same pno and pagePos should have the same
    // hash-value. This is needed for the oid-hash, to ensure, that these objects
    // are placed in the same bucket-chain.
    return ((Oid.gg92GetPno() ^ (((unsigned long)Oid.gg92GetPno() ) << 7) ^ 
        (Oid.gg92GetPno() >> 7) ^ (Oid.gg92GetPos() >> 3) ^ 
        (((unsigned long)Oid.gg92GetPos()) << 9)) % HashHdrSize);
}

/*----------------------------------------------------------------------*/
struct tkb501_SupplySegment;
typedef tkb501_SupplySegment *tkb501_SupplySegmentPtr;
struct tkb501_SupplySegment
{
    union
    {
        tkb05_SupplySegment             SupplySegment_kb05;
        struct kb501_SupplySegment_Variante {
            tkb501_SupplySegmentPtr sgmNext_kb501;
            tsp00_Addr              sgmHrdFreeList;
            tsp00_Int2              sgmHrdEntryCnt;
            tsp00_Int2              sgmHdrEntryUseCnt; 
        } s;
    } v;
    /* accessor functions */
    tkb501_SupplySegmentPtr &NextSeg      () { return this->v.s.sgmNext_kb501; };
    tsp00_Int2              &EntryCnt     () { return this->v.s.sgmHrdEntryCnt; };
    tsp00_Int2              &EntryUseCnt  () { return this->v.s.sgmHdrEntryUseCnt;  };
    tsp00_Addr              &FreeList     () { return this->v.s.sgmHrdFreeList;  };
    /* const accessor functions */
    const tkb501_SupplySegmentPtr &NextSeg () const { return this->v.s.sgmNext_kb501; };
    const tsp00_Int2        &EntryCnt     () const { return this->v.s.sgmHrdEntryCnt; };
    const tsp00_Int2        &EntryUseCnt  () const { return this->v.s.sgmHdrEntryUseCnt;  };
    const tsp00_Addr        &FreeList     () const { return this->v.s.sgmHrdFreeList;  };
} ;


/*----------------------------------------------------------------------*/
typedef  CShareLockSegment_kb501 *CShareLockSegmentPtr_kb501;
class CShareLockSegment_kb501 : public tkb501_SupplySegment
{
public :
    CShareLockSegment_kb501();
    CShareLockSegmentPtr_kb501 &NextSeg () { 
        return (reinterpret_cast<CShareLockSegmentPtr_kb501 &>(v.s.sgmNext_kb501));
    };
    inline tkb05_ObjShareLockPtr GetFirstFree();
    void  ReleaseEntry( RTESync_Spinlock &lock, tkb05_ObjShareLockPtr curr, tsp00_TaskId TaskId );
    inline size_t SizeOfHdr();
};
/*! endclass: CShareLockSegment_kb501 */

/*----------------------------------------------------------------------*/

inline tkb05_ObjShareLockPtr 
CShareLockSegment_kb501::GetFirstFree() {
    tkb05_ObjShareLockPtr curr = NULL;
    if ( NULL != FreeList() ) 
    {
        curr                        = reinterpret_cast<tkb05_ObjShareLockPtr>(FreeList());
        FreeList()                  = reinterpret_cast<tsp00_Addr>(curr->objrNxtShareLock_kb05);
        curr->objrNxtShareLock_kb05 = NULL;
        EntryUseCnt()++;
    }
    return curr;
}

/*----------------------------------------------------------------------*/

inline size_t 
CShareLockSegment_kb501::SizeOfHdr() 
{ 
    return sizeof(this->v.s); 
};

/*----------------------------------------------------------------------*/
typedef  CShareLockCntrlSegment_kb501 *CShareLockCntrlSegmentPtr_kb501;
class CShareLockCntrlSegment_kb501 : public tkb501_SupplySegment
{
public :
    CShareLockCntrlSegment_kb501();
    CShareLockCntrlSegmentPtr_kb501 &NextSeg () {
        return (reinterpret_cast<CShareLockCntrlSegmentPtr_kb501 &>(v.s.sgmNext_kb501));
    };
    inline tkb05_ObjShareLockCtrlPtr GetFirstFree();
    void  ReleaseEntry( RTESync_Spinlock &lock, tkb05_ObjShareLockCtrlPtr curr, tsp00_TaskId TaskId );
    inline size_t SizeOfHdr();
};
/*! endclass: CShareLockCntrlSegment_kb501 */

/*----------------------------------------------------------------------*/

inline tkb05_ObjShareLockCtrlPtr 
CShareLockCntrlSegment_kb501::GetFirstFree() 
{
    tkb05_ObjShareLockCtrlPtr curr = NULL;
    if ( NULL != FreeList() ) 
    {
        curr                            = reinterpret_cast<tkb05_ObjShareLockCtrlPtr>(FreeList());
        FreeList()                      = reinterpret_cast<tsp00_Addr>(curr->objsNxtShareLockCtrl_kb05);
        curr->objsNxtShareLockCtrl_kb05 = NULL;
        EntryUseCnt()++;
    }
    return curr;
};

/*----------------------------------------------------------------------*/

inline size_t 
CShareLockCntrlSegment_kb501::SizeOfHdr() 
{ 
    return sizeof(this->v.s); 
};


#endif /*GKB501_H*/
