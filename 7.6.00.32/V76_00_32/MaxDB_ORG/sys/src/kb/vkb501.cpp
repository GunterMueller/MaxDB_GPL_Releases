/*!***************************************************************************

  module      : vkb501.cpp
  responsible : FerdiF
  special area: KB_LockListe
  last changed: 2003-06-17
  description : shared object locks

  copyright   :    Copyright (c) 1998-2005 SAP AG


    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include "SAPDBCommon/Tracing/SAPDBTrace_Streaming.hpp"
#include "KernelCommon/Kernel_OpMsg.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "KernelCommon/Kernel_Trace.hpp"
#include "KernelCommon/Kernel_IAdminInfo.hpp"   /* &variant +fdir */
#include "ggg01.h"
#include "hgg01.h"      // g01maxlocks
#include "hgg08.h"
#include "heo55k.h"     // vbegexcl, vendexcl
#include "heo56.h"      // vsignal
#include "ggg92.h"
#include "hgg01_1.h"    // g01abort
#include "hgg01_3.h"    // g01vtrace
#include "gsp03_2.h"
#include "hkb50.h"
#include "hkb50_1.h"
#include "hkb51.h"
#include "hkb51_2.h"
#include "hkb52.h"
#include "gkb501.h"
#include "hkb501.h"
#include "hkb501_1.h"
#include "FileDirectory/FileDir_FileNo.hpp"

/*----------------------------------------------------------------------*/
static CSlockHash_kb501*   g_SlockEntryHashPtr = NULL;
static CShrLockSync_kb501* g_CSlockPtr         = NULL;

/*----------------------------------------------------------------------*/

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define ALL_REQ 0
#define SHARE_REQ 1

/*----------------------------------------------------------------------*/
/* implementation of local routines                                     */
/*----------------------------------------------------------------------*/

static void 
kb501TransToTrace(Kernel_VTrace& trace,
                  char*          name, 
                  tgg91_TransNo& transno)
{
    tsp00_Line line;
    int        linelen = 0;
    transno.gg90AppendToLine (linelen, line);
    line[linelen] = 0;
    trace << " " << name << "=" << line;
};

static tgg00_BasisError 
allocOidList( tgg00_TransContext &Trans
             ,COidList_kb501*  &list)
{
    tkb05_TransEntryPtr  ThisTrans;
    list = NULL;

    /* outside of region lock */
    k52acc_trans_entry (Trans.trIndex_gg00, ThisTrans);
    if ( ThisTrans->teWriteTransId_kb05.gg90IsNil() )
        return e_cancelled;

    if (NULL == ThisTrans->teShrLockListPtr_kb05)
    {
        SAPDBMem_IRawAllocator *alloc;
        if ( Kernel_IAdminInfo::Instance().KernelStateIsRedoLogProcessing() ) 
            alloc = &RTEMem_Allocator::Instance();
        else 
            alloc = reinterpret_cast<SAPDBMem_IRawAllocator*>(
            Trans.trAllocator_gg00);

        list = reinterpret_cast<COidList_kb501*>(
            alloc->Allocate(sizeof(COidList_kb501)));
        if ( NULL == list )
            return e_too_many_lock_requests;

        list = new(list) COidList_kb501(*alloc);
        if ( list->Init() )
            ThisTrans->teShrLockListPtr_kb05 = reinterpret_cast<tsp00_Addr>(list);
        else 
        {
            destroy(list,list->GetAllocator());
            return e_too_many_lock_requests;
        }
    };
    list = reinterpret_cast<COidList_kb501*>(ThisTrans->teShrLockListPtr_kb05);
    return e_ok;
}

/*----------------------------------------------------------------------*/

static tkb05_SupplySegmPtr
getSupplySegment( tgg00_TransContext &Trans ) 
{
    tkb05_SupplySegmPtr NewSegment;
    tkb05_SupplyPoolGlobPtr SupplyPoolGlobPtr;
    k52begexcl_supply_pool (Trans.trTaskId_gg00, SupplyPoolGlobPtr);
    kb51sget_segment       (Trans, *SupplyPoolGlobPtr, NewSegment);
    k52endexcl_supply_pool (Trans.trTaskId_gg00, SupplyPoolGlobPtr);
    if ( e_ok == Trans.trError_gg00 )
        return NewSegment;
    else
        return NULL;
};

/*----------------------------------------------------------------------*/
/* implementation of external routines                                  */
/*----------------------------------------------------------------------*/

externPascal void
k501DestroyTransLockInfo ( tkb05_TransEntry  &ThisTrans
                          ,tgg91_TransNo     &ThisWriteTransId )
{
    if (NULL != ThisTrans.teShrLockListPtr_kb05) 
    {
        COidList_kb501* list = reinterpret_cast<COidList_kb501*>(ThisTrans.teShrLockListPtr_kb05);
        for (COidList_kb501::OidList::Iterator Iter=(*list)()->Begin(); Iter!=(*list)()->End(); ++Iter)
        {
            tgg00_TransIndex        collLockIdx;
            tkb05_ObjCollGlobPtr    ObjCollGlobPtr = NULL;
            tkb05_ObjCollEntryPtr   CollEntry      = NULL;
            tgg00_BasisError        error          = e_ok;
            int                     hashval        = g_SlockEntryHashPtr->HashValue(Iter->oid);

            if (Iter->isExclLocked) 
            {
                /* if excl lock is committed, share lock info isn't needed anymore */
                /* and can be removed completely                                   */
                if ( list->IsCommitted() ) 
                {
                    if (g01vtrace.vtrKbLock_gg00)
                    {
                        Kernel_VTrace() << "k501Destroy:DestroyAllShareLockEntries OID "
                            << Iter->oid.gg92GetPno() << "." << Iter->oid.gg92GetPos()
                            << " (vers " << Iter->oid.gg92GetFrameVers() << ")  ExclLock aquired";
                    }
                    g_SlockEntryHashPtr->DestroyAllShareLockEntries(hashval, ThisTrans.teTaskId_kb05, Iter->oid);
                } 
                g_CSlockPtr->Lock(hashval);

                /* if a collision entry has been anchored into the share lock      */
                /* re-activate requesting transaction                              */
                collLockIdx = g_SlockEntryHashPtr->GetCollisionIndex( hashval, Iter->oid, error );
                g_CSlockPtr->Unlock(hashval);
                if ( ( e_ok == error ) && ( collLockIdx != cgg_nil_transindex ) ) {
                    k52begexcl_objcoll (ThisTrans.teTaskId_kb05, ObjCollGlobPtr);
                    g_CSlockPtr->Lock(hashval);
                    collLockIdx = g_SlockEntryHashPtr->GetCollisionIndex( hashval, Iter->oid, error );
                    g_CSlockPtr->Unlock(hashval);
                    if ( ( e_ok == error ) && ( collLockIdx != cgg_nil_transindex ) ) 
                    {
                        k52acc_objcoll_entry (*ObjCollGlobPtr, collLockIdx, CollEntry);
                        if  (CollEntry->objcFirstReq_kb05 == cgg_nil_transindex) 
                        {
                            int hashval = g_SlockEntryHashPtr->HashValue(CollEntry->objcOid_kb05);
                            g_SlockEntryHashPtr->RemoveCollisionEntry(hashval, *CollEntry);
                            if ( kb50IsValidCollisionEntry (*CollEntry, Iter->oid))
                            {
                                k50DelCollisionEntry (*ObjCollGlobPtr, ThisTrans, CollEntry);
                                if (g01vtrace.vtrKbLock_gg00) 
                                {
                                    Kernel_VTrace() << "  k501Destroy remove CollEntry Idx:" 
                                        << collLockIdx;
                                }
                            }
                        }
                        else 
                        {
                            if (g01vtrace.vtrKbLock_gg00)
                            {
                                Kernel_VTrace() 
                                    << "k501Destroy:k50SatisfyNextWaitingObjRequest CollEntry: "
                                    << collLockIdx;
                            }
                            tgg91_TransNo    AuxTransId;
                            AuxTransId.gg90SetNil();
                            k50SatisfyNextWaitingObjRequest (*ObjCollGlobPtr, *CollEntry, AuxTransId, ALL_REQ);
                        }
                    }
                    k52endexcl_objcoll (ThisTrans.teTaskId_kb05, ObjCollGlobPtr);
                } 
            } 
            else 
            {
                g_CSlockPtr->Lock(hashval);
                collLockIdx = g_SlockEntryHashPtr->GetCollisionIndex( hashval, Iter->oid, error );
                if ( ( e_ok == error ) && ( collLockIdx != cgg_nil_transindex ) ) 
                {
                    g_CSlockPtr->Unlock(hashval);
                    k52begexcl_objcoll (ThisTrans.teTaskId_kb05, ObjCollGlobPtr);
                    g_CSlockPtr->Lock(hashval);
                };
                if ( e_ok == error ) 
                {
                    tgg91_TransNo    ShareLockTransId;
                    ShareLockTransId.gg90SetNil();
                    tgg00_TransIndex collLockIdxExcl =
                        g_SlockEntryHashPtr->UnlockShare( hashval, ThisTrans.teTaskId_kb05, 
                        ThisTrans.teOwnIndex_kb05, *Iter, ThisWriteTransId, list->IsCommitted(), ShareLockTransId );
                    /* g_CSlockPtr->Unlock(hashval); done within 'UnlockShare' */
                    if ( collLockIdx != cgg_nil_transindex )
                    {
                        k52acc_objcoll_entry (*ObjCollGlobPtr, collLockIdx, CollEntry);
                        if  (CollEntry->objcFirstReq_kb05 == cgg_nil_transindex) 
                        {
                            int hashval = g_SlockEntryHashPtr->HashValue(CollEntry->objcOid_kb05);
                            g_SlockEntryHashPtr->RemoveCollisionEntry(hashval, *CollEntry);
                            if ( kb50IsValidCollisionEntry (*CollEntry, Iter->oid))
                            {
                                k50DelCollisionEntry (*ObjCollGlobPtr, ThisTrans, CollEntry);
                                if (g01vtrace.vtrKbLock_gg00) 
                                {
                                    Kernel_VTrace() << "  k501Destroy remove CollEntry Idx:" 
                                        << collLockIdx;
                                }
                            }
                        } 
                        else 
                        {
                            if (g01vtrace.vtrKbLock_gg00)
                            {
                                Kernel_VTrace() 
                                    << "k501Destroy:k50SatisfyNextWaitingObjRequest CollEntry: "
                                    << collLockIdx;
                            }
                            if (collLockIdxExcl == cgg_nil_transindex)
                                k50SatisfyNextWaitingObjRequest (*ObjCollGlobPtr, *CollEntry, ShareLockTransId, SHARE_REQ);
                            else
                                k50SatisfyNextWaitingObjRequest (*ObjCollGlobPtr, *CollEntry, ShareLockTransId, ALL_REQ);
                        }
                    }
                } else
                    g_CSlockPtr->Unlock(hashval);

                if ( NULL != ObjCollGlobPtr )
                    k52endexcl_objcoll (ThisTrans.teTaskId_kb05, ObjCollGlobPtr);
            }
        }
        destroy(list,list->GetAllocator());
        ThisTrans.teShrLockListPtr_kb05  = NULL;
    }
}

/*----------------------------------------------------------------------*/
externPascal tkb05_ObjShareLockCtrlPtr 
k501GetHashHdrList (tsp00_Int4  ListIndex) {
	g_CSlockPtr->Lock(ListIndex);
	return (g_SlockEntryHashPtr->GetHashHdrList(ListIndex));
};

externPascal void 
k501UnlockHashHdrList ( tsp00_Int4 ListIndex) {
	g_CSlockPtr->Unlock(ListIndex);
};

/*----------------------------------------------------------------------*/
externPascal tsp00_Int4 
k501GetHashHdrSize ()
{
	return g_SlockEntryHashPtr->GetHashHdrSize();
};

/*----------------------------------------------------------------------*/
externPascal void
k501GetShareLock2( tgg00_TransContext &Trans
				 ,tsp00_C8            &ObjFileNo
                 ,tkb05_KernelOid     &Oid 
				 ,pasbool              ignColEntry)
{
    tgg00_ObjTransInfo ObjTransInfo;
    ObjTransInfo.otrLockTransId_gg00.gg90SetZero();
    ObjTransInfo.otrLockTransIndex_gg00  = cgg_nil_transindex;
    k501GetShareLock( Trans, ObjFileNo, ObjTransInfo, Oid, ignColEntry );
};

/*----------------------------------------------------------------------*/
externPascal void
k501GetShareLock( tgg00_TransContext &Trans
				 ,tsp00_C8           &ObjFileNo
                 ,tgg00_ObjTransInfo &ObjTransInfo
                 ,tkb05_KernelOid    &Oid 
				 ,pasbool             ignColEntry)
{
    tgg00_BasisError    &TrError             = Trans.trError_gg00;
    COidList_kb501      *list                = NULL;
    bool                 bAlreadyLocked      = false;
    COidList_kb501::OidList::Iterator Iter;

    TrError = allocOidList(Trans, list);
    if (( e_ok != TrError ) && (g01vtrace.vtrKbLock_gg00))
        Kernel_VTrace() << "k501GetShareLock:allocOidList error:" << TrError;


    if ( e_ok == TrError )
    {
        tkb05_ObjShareLockPtr  lockReq;
        int hashval = g_SlockEntryHashPtr->HashValue(Oid);
        g_SlockEntryHashPtr->GetLockShare(hashval, Trans, ObjFileNo,
            ObjTransInfo, Oid, lockReq, ignColEntry, bAlreadyLocked );
        if ( e_ok == TrError ) 
        {
            if ( bAlreadyLocked ) 
            {
                for ( Iter=(*list)()->Begin(); Iter!=(*list)()->End(); ++Iter )
                {
                    if (Oid.gg92IsEqIgnoreFrameVers((*Iter).oid)) 
                    {
                        Iter->isShareLocked = TRUE;
                        Iter->lockReq       = lockReq;
                        break;
                    };
                }
                if (Iter ==(*list)()->End()) 
                {
                    /* if first exclusive locked, then it's possible that no entry exists yet */
                    if ( ObjTransInfo.otrLockTransId_gg00 != Trans.trWriteTransId_gg00 )
                    {
                        Kernel_OpWarning( kbMsg_csp03, csp3_n_lock )
                            << "k501GetShareLock already locked, but not found within OID list" 
                            << " OID:"  << Oid.gg92GetPno() << "."
                            << Oid.gg92GetPos()
                            << " (vers " << Oid.gg92GetFrameVers() << ")";
                    }
                    bAlreadyLocked = false; 
                }
            }
            if ( !bAlreadyLocked ) 
            {
                COidListEntry_kb501 oidListEntry;
                oidListEntry.isShareLocked = true;
                oidListEntry.oid           = Oid;
                oidListEntry.lockReq       = lockReq;
                /* in case transaction already owns an exclusive lock, but now          */
                /* requesting a share lock, the lock control structure has been created */
                if ( ObjTransInfo.otrLockTransId_gg00 == Trans.trWriteTransId_gg00 )
                    oidListEntry.isExclLocked = TRUE;

                if ( !(*list)()->InsertFront(oidListEntry).IsValid())
                {
                    tgg91_TransNo    AuxTransId;
                    AuxTransId.gg90SetNil();
                    bool isCommitted = true;
                    g_CSlockPtr->Lock(hashval);
                    g_SlockEntryHashPtr->UnlockShare( hashval, Trans.trTaskId_gg00, 
                        Trans.trIndex_gg00, oidListEntry, Trans.trWriteTransId_gg00,
                        !isCommitted, AuxTransId );
                    g_CSlockPtr->Unlock(hashval);
                    TrError = e_too_many_lock_requests;
                    if (g01vtrace.vtrKbLock_gg00) 
                        Kernel_VTrace() << "k501GetShareLock InsertFront failed";
                }
            }
        }
    }
    if (g01vtrace.vtrKbLock_gg00) 
    {
        Kernel_VTrace() << "k501GetShareLock OID "
            << Oid.gg92GetPno() << "." << Oid.gg92GetPos()
            << " (vers " << Oid.gg92GetFrameVers() << ")"
            << " TrError:" << TrError;
    }
};

/*----------------------------------------------------------------------*/
/* ignShrLckCsView is set to true within vkb67.cpp if called by         */
/*                 omsNewConsistentView                                 */
externPascal tgg00_TransIndex
k501GetExclusiveLock( tgg00_TransContext &Trans
					 ,tsp00_C8           &ObjFileNo
					 ,tkb05_KernelOid    &Oid
					 ,pasbool             ignShrLckCsView
					 ,pasbool             ignColEntry)
{
    tgg00_BasisError    &TrError             = Trans.trError_gg00;
    tgg00_TransIndex     CollTransIndex      = cgg_nil_transindex;
    bool                 bAlreadyLocked;
    COidList_kb501::OidList::Iterator    Iter;
    COidList_kb501            *list;

    TrError = allocOidList(Trans, list);
    if (( e_ok != TrError ) && (g01vtrace.vtrKbLock_gg00))
    {
        Kernel_VTrace() << "k501GetShareLock:allocOidList error:"
            << TrError << " OID " 
            << Oid.gg92GetPno() << "." << Oid.gg92GetPos()
            << " (vers " << Oid.gg92GetFrameVers() << ")";
    }
    if ( e_ok == TrError )
    {
        int hashvalue = g_SlockEntryHashPtr->HashValue(Oid);
        CollTransIndex = g_SlockEntryHashPtr->GetLockExcl(hashvalue, Trans, 
            ObjFileNo, Oid, ignShrLckCsView, ignColEntry, bAlreadyLocked );

        if (( e_ok == TrError ) && 
            (( CollTransIndex == cgg_nil_transindex ) || ignColEntry ))
        {
            if ( bAlreadyLocked ) 
            {
                /* if object has already been share locked by this transaction */
                /* mark the existing entry as exclusive locked                 */
                for (Iter=(*list)()->Begin(); Iter!=(*list)()->End(); ++Iter)
                    if (Oid.gg92IsEqIgnoreFrameVers(Iter->oid))
                    {
                        Iter->isExclLocked = TRUE;
                        break;
                    };
                if (Iter ==(*list)()->End()) 
                {
                    Kernel_OpWarning( kbMsg_csp03, csp3_n_lock )
                        << "k501GetExclusiveLock already locked, but not found within OID list" 
                        << " OID:"  << (*Iter).oid.gg92GetPno() << "."
                        << (*Iter).oid.gg92GetPos()
                        << " (vers " << (*Iter).oid.gg92GetFrameVers() << ")";
                    bAlreadyLocked = false; 
                }
            }
            else
            {
                COidListEntry_kb501 oidListEntry;
                oidListEntry.isExclLocked  = true;
                oidListEntry.oid           = Oid;
                oidListEntry.lockReq       = NULL;
                if ( !(*list)()->InsertFront(oidListEntry).IsValid())
                {
                    g_CSlockPtr->Lock(hashvalue);
                    g_SlockEntryHashPtr->UnlockExcl( Trans.trWriteTransId_gg00, hashvalue, oidListEntry );
                    g_CSlockPtr->Unlock(hashvalue);
                    TrError = e_too_many_lock_requests;
                    if (g01vtrace.vtrKbLock_gg00) 
                        Kernel_VTrace() << "k501GetExclusiveLock InsertFront failed";
                }
            }
        }
        if (g01vtrace.vtrKbLock_gg00) 
        {
            Kernel_VTrace() << "k501GetExclusiveLock OID "
                << Oid.gg92GetPno() << "." << Oid.gg92GetPos()
                << " (vers " << Oid.gg92GetFrameVers() << ")"
                << " CollEntry:" << CollTransIndex
                << " TrError:" << TrError;
        }
    }
    return CollTransIndex;
};

/*----------------------------------------------------------------------*/

externPascal void
k501GetStatistics ( tsp00_Int4  &LockCntrlEntries
                   ,tsp00_Int4  &LockCntrlEntriesUsed
                   ,tsp00_Int4  &ShareLockEntries
                   ,tsp00_Int4  &ShareLockEntriesUsed
                   ,tsp00_Int4  &CollisionEntriesUsed)
{
    g_SlockEntryHashPtr->GetStatistics( LockCntrlEntries, LockCntrlEntriesUsed,
        ShareLockEntries, ShareLockEntriesUsed, CollisionEntriesUsed);
};

/*----------------------------------------------------------------------*/
externPascal pasbool 
k501GetShareLockInfo ( tgg00_TransContext    &Trans
                      ,tsp00_Addr            &contextAddr
                      ,FileDir_FileNo        &fileno
                      ,tgg92_KernelOid       &oid
                      ,tgg91_TransNo         &transno
                      ,tsp00_TaskId          &taskidOfTrans
                      ,pasbool               &isExclusive
                      ,pasbool               &isHolder)
{
    int                   hashval;
    bool                  fillArray     = false;
    CShareLockInfo_kb501 *ShrLckInfoPtr = NULL;
    pasbool               rc            = FALSE;
    if ( contextAddr == NULL )
    {
        SAPDBMem_IRawAllocator *alloc = 
            reinterpret_cast<SAPDBMem_IRawAllocator*>(Trans.trAllocator_gg00);
        contextAddr = reinterpret_cast<tsp00_Addr>(
            alloc->Allocate(sizeof(CShareLockInfo_kb501)));
        if ( contextAddr )
            ShrLckInfoPtr = new (contextAddr) CShareLockInfo_kb501(alloc);
        ShrLckInfoPtr->ResizeVector( 50 );
        fillArray = true;
    } 
    else
    {
        ShrLckInfoPtr = reinterpret_cast<CShareLockInfo_kb501*>(contextAddr);
        if ( ShrLckInfoPtr->getInfoPos() == ShrLckInfoPtr->getInfoMax() )
        {
            ShrLckInfoPtr->ResetForRefill();
            fillArray = true;
        }
    }
    if ( ShrLckInfoPtr )
    {
        hashval = ShrLckInfoPtr->getHashPos();
        if ( hashval < g_SlockEntryHashPtr->GetHashHdrSize() )
        {
            if ( fillArray )
            {
                if ( g_SlockEntryHashPtr->GetShareLockInfo( hashval, ShrLckInfoPtr ) )
                {
                    ShrLckInfoPtr->getInfo(fileno, oid, transno, taskidOfTrans
                        , isExclusive, isHolder);
                    rc = TRUE;
                }
				ShrLckInfoPtr->setHashPos( hashval );
            }
            else
            {
                ShrLckInfoPtr->getInfo(fileno, oid, transno, taskidOfTrans
                    , isExclusive, isHolder);
                rc = TRUE;
            }
        }

        if ( rc == FALSE )
        {
            SAPDBMem_IRawAllocator *alloc = ShrLckInfoPtr->getAllocator();
            destroy( ShrLckInfoPtr, *alloc );
            contextAddr = NULL;
        }
    }
    return rc;
};


/*----------------------------------------------------------------------*/

externPascal tgg00_TransIndex
k501GetCollEntryIdx ( tgg00_TransContext       &Trans
                     ,tkb05_KernelOid          &Oid)
{
    tgg00_TransIndex transIdx;
    int hashval = g_SlockEntryHashPtr->HashValue(Oid);
    g_CSlockPtr->Lock(hashval);
    transIdx = g_SlockEntryHashPtr->GetCollisionIndex(hashval, Oid, Trans.trError_gg00);
    g_CSlockPtr->Unlock(hashval);
    if (g01vtrace.vtrKbLock_gg00)
    {
        Kernel_VTrace() << "k501GetCollEntryIdx Oid:"
            << Oid.gg92GetPno() << "." << Oid.gg92GetPos()
            << " (vers " << Oid.gg92GetFrameVers() << ")"
            << " Idx:" << transIdx
            << " TrError:" << Trans.trError_gg00;
    }
    return transIdx;
};

/*----------------------------------------------------------------------*/

externPascal void 
k501InitOMSShareLocks ()
{
    if ( NULL == g_SlockEntryHashPtr) {                   
        g_SlockEntryHashPtr = reinterpret_cast<CSlockHash_kb501*>(
            ( RTEMem_Allocator::Instance())
            .Allocate(sizeof(CSlockHash_kb501)) );
        g_SlockEntryHashPtr = new(g_SlockEntryHashPtr) CSlockHash_kb501();
        g_CSlockPtr = reinterpret_cast<CShrLockSync_kb501*>(
            ( RTEMem_Allocator::Instance())
            .Allocate(sizeof(CShrLockSync_kb501)) );
        g_CSlockPtr = new(g_CSlockPtr) CShrLockSync_kb501();
    }
};

/*----------------------------------------------------------------------*/

externPascal pasbool 
k501IsObjTransUsed ( tsp00_TaskId      TaskId
                    ,tkb05_KernelOid  &Oid
										,pasbool           ignoreVersions
										,tgg91_TransNo    &UpdTransId_gg00
										,tgg91_TransNo    &ActTransNo)
{
    pasbool bIsUsed = FALSE;
    int hashval = g_SlockEntryHashPtr->HashValue(Oid);
    bIsUsed = g_SlockEntryHashPtr->IsObjTransUsed(hashval, TaskId, Oid, ignoreVersions, UpdTransId_gg00, ActTransNo);
    return bIsUsed;
};


/*----------------------------------------------------------------------*/

externPascal void
k501IsSelfLocked ( tgg00_TransContext &Trans
                  ,tkb05_KernelOid    &Oid
				  ,pasbool             bIsSharedLocked)
{
    int hashval = g_SlockEntryHashPtr->HashValue(Oid);
    g_CSlockPtr->Lock(hashval);
    g_SlockEntryHashPtr->IsSelfLocked(hashval, Trans, Oid, bIsSharedLocked);
    g_CSlockPtr->Unlock(hashval);
};

/*----------------------------------------------------------------------*/

externPascal void
k501SetMinTrans ( tsp00_TaskId    TaskId
                 ,tgg91_TransNo  &MinTrans )
{
    g_SlockEntryHashPtr->SetMinTrans( TaskId, MinTrans );
};

/*----------------------------------------------------------------------*/

externPascal void
k501Unlock ( tkb05_TransEntry     &ThisTrans
            ,tkb05_KernelOid      &Oid
            ,pasbool               isExclLock )
{
    COidList_kb501* list = reinterpret_cast<COidList_kb501*>(ThisTrans.teShrLockListPtr_kb05);
    if ( NULL != list )
    {
        bool commit = list->IsCommitted();
        for (COidList_kb501::OidList::Iterator Iter=(*list)()->Begin(); Iter!=(*list)()->End(); ++Iter)
        {
            if (Oid.gg92IsEqIgnoreFrameVers(Iter->oid))
            {
                tkb05_ObjCollGlobPtr  ObjCollGlobPtr = NULL;
                tgg00_BasisError      error          = e_ok;
                tgg00_TransIndex      collLockIdx;
                tgg00_TransIndex      collLockIdxExcl   = cgg_nil_transindex;
                tgg00_TransIndex      collLockIdxShared = cgg_nil_transindex;
                tgg91_TransNo    ShareLockTransId;
                ShareLockTransId.gg90SetNil();                   

                int hashval = g_SlockEntryHashPtr->HashValue(Oid);
                g_CSlockPtr->Lock(hashval);
                collLockIdx = g_SlockEntryHashPtr->GetCollisionIndex( hashval, Iter->oid, error );
                if ( ( e_ok == error ) && ( collLockIdx != cgg_nil_transindex ) ) 
                {
                    g_CSlockPtr->Unlock(hashval);
                    k52begexcl_objcoll (ThisTrans.teTaskId_kb05, ObjCollGlobPtr);
                    g_CSlockPtr->Lock(hashval);
                }
                if ( isExclLock ) 
                {
                    collLockIdxExcl = g_SlockEntryHashPtr->UnlockExcl(ThisTrans.teWriteTransId_kb05, hashval, *Iter);
                    Iter->isExclLocked  = FALSE;
                } 
                else 
                {
                    Iter->lockReq->objrLockRollbacked = TRUE;
                    collLockIdxShared = g_SlockEntryHashPtr->UnlockShare( hashval, ThisTrans.teTaskId_kb05,
                        ThisTrans.teOwnIndex_kb05, *Iter, ThisTrans.teWriteTransId_kb05, 
                        list->IsCommitted(), ShareLockTransId);
                    Iter->isShareLocked  = FALSE;
                }
                g_CSlockPtr->Unlock(hashval);
                if ( collLockIdx != cgg_nil_transindex )
                {
                    tkb05_ObjCollEntryPtr   CollEntry;
                    k52acc_objcoll_entry (*ObjCollGlobPtr, collLockIdx, CollEntry);
                    g_CSlockPtr->Lock(hashval);
                    collLockIdx = g_SlockEntryHashPtr->GetCollisionIndex( hashval, Iter->oid, error );
                    g_CSlockPtr->Unlock(hashval);
                    if ( collLockIdx != cgg_nil_transindex ) 
                    {
                        k52acc_objcoll_entry (*ObjCollGlobPtr, collLockIdx, CollEntry);
                        if  (CollEntry->objcFirstReq_kb05 == cgg_nil_transindex) 
                        {
                            int hashval = g_SlockEntryHashPtr->HashValue(CollEntry->objcOid_kb05);
                            g_SlockEntryHashPtr->RemoveCollisionEntry(hashval, *CollEntry);
                            if ( kb50IsValidCollisionEntry (*CollEntry, Iter->oid))
                            {
                                k50DelCollisionEntry (*ObjCollGlobPtr, ThisTrans, CollEntry);
                                if (g01vtrace.vtrKbLock_gg00) 
                                {
                                    Kernel_VTrace() << "  k501Unlock remove CollEntry Idx:" 
                                        << collLockIdx;
                                }
                            }
                        }
                        else 
                        {
                            if (g01vtrace.vtrKbLock_gg00)
                            {
                                Kernel_VTrace() 
                                    << "k501Unlock:k50SatisfyNextWaitingObjRequest CollEntry: "
                                    << collLockIdx;
                            }
                            if (( collLockIdxShared != cgg_nil_transindex ) || ( collLockIdxExcl != cgg_nil_transindex ))
                                k50SatisfyNextWaitingObjRequest (*ObjCollGlobPtr, *CollEntry, ShareLockTransId, ALL_REQ);
                            else
                                k50SatisfyNextWaitingObjRequest (*ObjCollGlobPtr, *CollEntry, ShareLockTransId, SHARE_REQ);
                        }
                    }
                }
                if ( NULL != ObjCollGlobPtr ) 
                    k52endexcl_objcoll (ThisTrans.teTaskId_kb05, ObjCollGlobPtr);
                
                if (g01vtrace.vtrKbLock_gg00)
                {
                    Kernel_VTrace() << "k501UnLock OID "
                        << Iter->oid.gg92GetPno() << "." << Iter->oid.gg92GetPos()
                        << " (vers " << Iter->oid.gg92GetFrameVers() << ")"
                        << " exclusive:" << isExclLock;
                }
                
                if ( !Iter->isExclLocked && !Iter->isShareLocked )
                    (*list)()->Delete(Iter);
                
                break;
            }
        }
    }
};

/*----------------------------------------------------------------------*/

externPascal void
k501MarkTransend ( tgg00_TransIndex  TransIdx
                  ,pasbool           committed )
{
    /* Routine flags sharelock list as committed              */
    /* On transaction end all request for share locks flagged */
    /* as committed will have to be preserved to ensure       */
    /* correct checkin if object should be locked exclusively */

    /* All requests for share locks not flagged as committed  */
    /* can be unchained and memory may be freed               */

    /* outside of region lock */
    tkb05_TransEntryPtr  ThisTrans;
    k52acc_trans_entry ( TransIdx, ThisTrans );
    if (NULL != ThisTrans->teShrLockListPtr_kb05)
    {
        COidList_kb501 *list;
        list = reinterpret_cast<COidList_kb501*>(ThisTrans->teShrLockListPtr_kb05);
        if ( !committed ) 
        {
            list->SetRollback();
            for (COidList_kb501::OidList::Iterator Iter=(*list)()->Begin(); Iter!=(*list)()->End(); ++Iter)
                if ( Iter->isShareLocked ) 
                    Iter->lockReq->objrLockRollbacked = TRUE;
        }
        else
            list->SetCommit();
    }
    if (g01vtrace.vtrKbLock_gg00) 
    {
        Kernel_VTrace() << "  k501MarkTransend TransIdx" << TransIdx
            << " committed:" << committed;
    }
};

/*----------------------------------------------------------------------*/

externPascal void
k501RemoveCollisionEntry ( tgg00_TransContext      &Trans
                          ,tkb05_ObjCollisionEntry &CollEntry )
{
    tgg00_TransIndex collidx;
    int hashval = g_SlockEntryHashPtr->HashValue(CollEntry.objcOid_kb05);
    collidx = g_SlockEntryHashPtr->RemoveCollisionEntry(hashval, CollEntry);

    if (g01vtrace.vtrKbLock_gg00) 
    {
        Kernel_VTrace() << "  k501RmvCollEntry OID "
            << CollEntry.objcOid_kb05.gg92GetPno() << "."
            << CollEntry.objcOid_kb05.gg92GetPos()
            << " (vers " << CollEntry.objcOid_kb05.gg92GetFrameVers() << ")"
            << " CollEntryIdx:" << collidx;
    }
};

/*----------------------------------------------------------------------*/

externPascal void
k501StoreCollisionEntry( tgg00_TransContext      &Trans
                        ,tkb05_ObjCollisionEntry &CollEntry )
{
    int hashval = g_SlockEntryHashPtr->HashValue(CollEntry.objcOid_kb05);
    g_SlockEntryHashPtr->StoreCollisionEntry(hashval, CollEntry);

    if (g01vtrace.vtrKbLock_gg00) 
        Kernel_VTrace() << " k501PutCollEntry Idx:" << CollEntry.objcOwnIndex_kb05;
};

/*----------------------------------------------------------------------*/
/* implementation of CShareLockSegment_kb501 routines                   */
/*----------------------------------------------------------------------*/

CShareLockSegment_kb501::CShareLockSegment_kb501() {
    int i;
    EntryCnt()      = ( sizeof(tkb05_SupplySegment) - SizeOfHdr() ) / 
        sizeof(tkb05_ObjShareLock);
    EntryUseCnt()   = 0;
    FreeList()      = NULL;

    tkb05_ObjShareLockPtr currEntry = 
        reinterpret_cast<tkb05_ObjShareLockPtr> 
        ( reinterpret_cast<char*>(this) + SizeOfHdr() );

    for ( i = 0; i < EntryCnt(); i++ )
    {
        currEntry->objrOwnIndex_kb05     = i;
        currEntry->objrNxtShareLock_kb05 = reinterpret_cast<tkb05_ObjShareLockPtr>(FreeList());
        FreeList()                       = reinterpret_cast<tsp00_Addr>(currEntry);
        currEntry++;
    }
};

/*----------------------------------------------------------------------*/
/* implementation of CShareLockCntrlSegment_kb501 routines              */
/*----------------------------------------------------------------------*/

CShareLockCntrlSegment_kb501::CShareLockCntrlSegment_kb501() {
    int i;
    EntryCnt()      = ( sizeof(tkb05_SupplySegment) - SizeOfHdr() ) / 
        sizeof(tkb05_ObjShareLockCtrl);
    EntryUseCnt()   = 0;
    FreeList()      = NULL;
    
    tkb05_ObjShareLockCtrlPtr currEntry = 
        reinterpret_cast<tkb05_ObjShareLockCtrlPtr> 
        ( reinterpret_cast<char*>(this) + SizeOfHdr() );
    
    for ( i = 0; i < EntryCnt(); i++ )
    {
        currEntry->objsOwnIndex_kb05         = i;
        currEntry->objsNxtShareLockCtrl_kb05 = reinterpret_cast<tkb05_ObjShareLockCtrlPtr>(FreeList());
        FreeList()                           = reinterpret_cast<tsp00_Addr>(currEntry);
        currEntry++;
    }
};

/*----------------------------------------------------------------------*/

void 
CShareLockCntrlSegment_kb501::ReleaseEntry( RTESync_Spinlock         &lock
                                           ,tkb05_ObjShareLockCtrlPtr curr
                                           ,tsp00_TaskId              TaskId ) 
{
    curr->objsNxtShareLockCtrl_kb05   = reinterpret_cast<tkb05_ObjShareLockCtrlPtr>(FreeList());
    FreeList()                        = reinterpret_cast<tsp00_Addr>(curr);
    EntryUseCnt()--;
    if ( EntryUseCnt() == 0 ) 
    {
        tkb05_SupplyPoolGlobPtr SupplyPoolGlobPtr;
        g_SlockEntryHashPtr->UnchainFromLockCtrlFreeList( this );
        lock.Unlock();
        k52begexcl_supply_pool (TaskId, SupplyPoolGlobPtr);
        tkb05_SupplySegment *p = &this->v.SupplySegment_kb05;
        kb51sfree_segment (*SupplyPoolGlobPtr, p);
        k52endexcl_supply_pool (TaskId, SupplyPoolGlobPtr);
        //lock.Lock();
    }
    else
        lock.Unlock();
}

/*----------------------------------------------------------------------*/

void 
CShareLockSegment_kb501::ReleaseEntry( RTESync_Spinlock     &lock
                                      ,tkb05_ObjShareLockPtr curr
                                      ,tsp00_TaskId          TaskId ) 
{
    curr->objrNxtShareLock_kb05 = reinterpret_cast<tkb05_ObjShareLockPtr>(FreeList());
    FreeList()                  = reinterpret_cast<tsp00_Addr>(curr);
    EntryUseCnt()--;
    if ( EntryUseCnt() == 0 ) 
    {
        tkb05_SupplyPoolGlobPtr SupplyPoolGlobPtr;
        g_SlockEntryHashPtr->UnchainFromLockFreeList( this );
        lock.Unlock();
        k52begexcl_supply_pool (TaskId, SupplyPoolGlobPtr);
        tkb05_SupplySegment *p = &this->v.SupplySegment_kb05;
        kb51sfree_segment (*SupplyPoolGlobPtr, p);
        k52endexcl_supply_pool (TaskId, SupplyPoolGlobPtr);
        //lock.Lock();
    }
    else
        lock.Unlock();
}

/*----------------------------------------------------------------------*/
/* implementation of CSlockHash_kb501 routines                          */
/*----------------------------------------------------------------------*/

void
CSlockHash_kb501::UnchainFromLockFreeList( CShareLockSegment_kb501* entry )
{
    CShareLockSegment_kb501 *currListSeg = m_ShareLockSupplySegments;
    if ( entry == m_ShareLockSupplySegments ) 
        m_ShareLockSupplySegments = entry->NextSeg();
    else 
    {
        while ( currListSeg != NULL )
        {
            if ( entry == currListSeg->NextSeg()) 
            {
                currListSeg->NextSeg() = entry->NextSeg();
                break;
            }
            currListSeg = currListSeg->NextSeg();
        }
    }
    entry->NextSeg() = NULL;
};

/*----------------------------------------------------------------------*/

void
CSlockHash_kb501::UnchainFromLockCtrlFreeList( CShareLockCntrlSegment_kb501* entry )
{
    CShareLockCntrlSegment_kb501 *currListSeg = m_LockCntrlSupplySegments;
    if ( entry == m_LockCntrlSupplySegments ) 
        m_LockCntrlSupplySegments = entry->NextSeg();
    else 
    {
        while ( currListSeg != NULL )
        {
            if ( entry == currListSeg->NextSeg()) 
            {
                currListSeg->NextSeg() = entry->NextSeg();
                break;
            }
            currListSeg = currListSeg->NextSeg();
        }
    }
    entry->NextSeg() = NULL;
};
/*----------------------------------------------------------------------*/

tkb05_ObjShareLockCtrlPtr
CSlockHash_kb501::AllocLockCntrlEntry( tgg00_TransContext  &Trans
                                      ,tsp00_C8            &ObjFileNo)
{
    tkb05_ObjShareLockCtrlPtr curr = NULL;

    m_SupplySegmentsCntrl_Lock.Lock();
    CShareLockCntrlSegment_kb501 *currListSeg = m_LockCntrlSupplySegments;
    while ( currListSeg != NULL )
    {
        curr = currListSeg->GetFirstFree();
        if  ( curr != NULL ) break;
        currListSeg = currListSeg->NextSeg();
    }
    m_SupplySegmentsCntrl_Lock.Unlock();

    while (( NULL == curr ) && ( e_ok == Trans.trError_gg00 )) 
    {
        tkb05_SupplySegmPtr NewSegment = getSupplySegment( Trans ); 
        if ( NULL != NewSegment )
        {
            currListSeg = new(NewSegment) CShareLockCntrlSegment_kb501;
            m_SupplySegmentsCntrl_Lock.Lock();

            /* chain to freelist */
            currListSeg->NextSeg() = m_LockCntrlSupplySegments;
            m_LockCntrlSupplySegments = currListSeg;
            curr = currListSeg->GetFirstFree();
            m_SupplySegmentsCntrl_Lock.Unlock();
        }
        if ( curr != NULL) 
        {
            if (g01vtrace.vtrKbLock_gg00) 
            {
                Kernel_VTrace()
                    << " CSlockHash_kb501 new CShareLockCntrlSegment_kb501 allocated";
            }
        } 
        else 
        {
            Trans.trError_gg00 = e_too_many_lock_requests;
            if (g01vtrace.vtrKbLock_gg00) 
                Kernel_VTrace() << "CSlockHash_kb501::AllocLockCntrlEntry - *** no more memory *** "
                    << Trans.trError_gg00;
        }
    } 

    if ( curr != NULL) 
    {
        curr->objsNxtShareLockCtrl_kb05   = cgg_nil_transindex;
        curr->objsObjShareLock_kb05       = NULL;
        curr->objsOid_kb05.gg92SetNil();
        curr->objsCollLock_kb05           = cgg_nil_transindex;
        curr->objsExclLockTransIndex_kb05 = cgg_nil_transindex;
        curr->objsExclLockTransId_kb05.gg90SetNil();
        curr->objsInuseWithoutLock_kb05   = 0;
        curr->objsReleaseLocks_kb05       = FALSE;
        curr->objsFiller1_kb05            = 0;
        curr->objsFileDirFileId_kb05()    = ObjFileNo;
        /* currReq->objrOwnIndex_kb05 MUST NOT be changed */
        /* objsOwnIndex_kb05 MUST NOT be changed */
    }
    
    return curr;
};

/*----------------------------------------------------------------------*/

tkb05_ObjShareLockPtr
CSlockHash_kb501::AllocShrLockEntry(tgg00_TransContext  &Trans)
{
    tkb05_ObjShareLockPtr currReq = NULL;

    m_SupplySegmentsShare_Lock.Lock();
    CShareLockSegment_kb501 *currListSeg = m_ShareLockSupplySegments;
    while ( currListSeg != NULL ) 
    {
        currReq = currListSeg->GetFirstFree();
        if  ( currReq != NULL ) break;
        currListSeg = currListSeg->NextSeg();
    }
    m_SupplySegmentsShare_Lock.Unlock();

    while (( currReq == NULL ) && ( e_ok == Trans.trError_gg00 )) 
    {
        tkb05_SupplySegmPtr NewSegment = getSupplySegment( Trans ); 
        if ( NULL != NewSegment )
        {
            currListSeg = new(NewSegment) CShareLockSegment_kb501;
            m_SupplySegmentsShare_Lock.Lock();  

            /* chain to freelist */
            currListSeg->NextSeg() = m_ShareLockSupplySegments;
            m_ShareLockSupplySegments          = currListSeg;
            currReq = currListSeg->GetFirstFree();
            m_SupplySegmentsShare_Lock.Unlock();
        }
        if ( currReq != NULL) 
        {
            if (g01vtrace.vtrKbLock_gg00) 
            {
                Kernel_VTrace()
                    << " CSlockHash_kb501 - new CShareLockSegment_kb501 allocated";
            }
        } 
        else 
        {
            Trans.trError_gg00 = e_too_many_lock_requests;
            if (g01vtrace.vtrKbLock_gg00) 
                Kernel_VTrace() << " CSlockHash_kb501::AllocShrLockEntry - *** no more memory *** "
                    << Trans.trError_gg00;
        }
    }

    if ( currReq != NULL) 
    {
        currReq->objrNxtShareLock_kb05    = NULL;
        currReq->objrSlockTransId_kb05.gg90SetNil();
        currReq->objrSlockTransIndex_kb05 = cgg_nil_transindex;
        currReq->objrLockRollbacked       = FALSE;
        currReq->objrFiller1_kb05         = 0;
        currReq->objrFiller2_kb05         = 0;
        currReq->objrFiller3_kb05         = 0;
    }
    
    return currReq;
};

/*----------------------------------------------------------------------*/

pasbool
CSlockHash_kb501::IsObjTransUsed( int               hashvalue
                                 ,tsp00_TaskId      TaskId
                                 ,tkb05_KernelOid  &Oid
                                 ,pasbool           ignoreVersions
                                 ,tgg91_TransNo    &UpdTransId_gg00
                                 ,tgg91_TransNo    &ActTransNo)
{
    pasbool bIsUsed = FALSE;
    tgg91_TransNo DummyMinTransId;
    DummyMinTransId.gg90SetZero();
    
    g_CSlockPtr->Lock(hashvalue);
    tkb05_ObjShareLockCtrlPtr curr = FindLockCntrlEntry(hashvalue, Oid);

    if ( NULL != curr ) 
    {
        /* to make sure no entry is deleted if CSlock is released                     */
        /* but be aware, new share lock entries may be inserted if CSlock is released */
        curr->objsInuseWithoutLock_kb05++;    

		UpdTransId_gg00  = curr->objsExclLockTransId_kb05;	 
		if ( ! curr->objsExclLockTransId_kb05.gg90IsNil() && 
			IsTransActive( curr->objsExclLockTransIndex_kb05
			, curr->objsExclLockTransId_kb05 ))
		{
			tkb05_TransEntryPtr ThisTrans;
			k52acc_trans_entry( curr->objsExclLockTransIndex_kb05, ThisTrans );
			ActTransNo = ThisTrans->teSelectTransId_kb05;                 /* may have to be synchronized !!! */
			bIsUsed = TRUE;
		}
		else
		{
			tkb05_ObjShareLockPtr nxtReq   = curr->objsObjShareLock_kb05;
			tkb05_ObjShareLockPtr firstReq = nxtReq;
			while (( NULL != nxtReq ))
			{
				if ( UpdTransId_gg00.gg90IsNil() )
					UpdTransId_gg00 = nxtReq->objrSlockTransId_kb05;
				else
				{
					if ( UpdTransId_gg00 > nxtReq->objrSlockTransId_kb05 )
						UpdTransId_gg00 = nxtReq->objrSlockTransId_kb05;
				}

				if ( IsTransActive( nxtReq->objrSlockTransIndex_kb05
					, nxtReq->objrSlockTransId_kb05 ))
				{
					tkb05_TransEntryPtr ThisTrans;
					k52acc_trans_entry( nxtReq->objrSlockTransIndex_kb05, ThisTrans );
					ActTransNo = ThisTrans->teSelectTransId_kb05;     /* may have to be synchronized !!! */
					bIsUsed = TRUE;
				}                    
				if ( firstReq != curr->objsObjShareLock_kb05 )
					bIsUsed = TRUE;
				else
					nxtReq = nxtReq->objrNxtShareLock_kb05;
			}
        }
        curr->objsInuseWithoutLock_kb05--;
    }

	g_CSlockPtr->Unlock(hashvalue);

	return bIsUsed;
};

/*----------------------------------------------------------------------*/

void
CSlockHash_kb501::IsSelfLocked( int                  hashvalue
                               ,tgg00_TransContext  &Trans
                               ,tkb05_KernelOid     &Oid
							   ,pasbool              bIsSharedLocked)
{
    tkb05_ObjShareLockCtrlPtr curr = FindLockCntrlEntry(hashvalue, Oid);
    if ( NULL != curr ) 
    {
		if (0 == bIsSharedLocked) 
		{
			if ( curr->objsExclLockTransId_kb05 != Trans.trWriteTransId_gg00 )
        
				Trans.trError_gg00 = e_object_not_locked;
		}
		else
		{
            tkb05_ObjShareLockPtr currReq = curr->objsObjShareLock_kb05;
			while (currReq)
			{
				if ( currReq->objrSlockTransId_kb05 == Trans.trWriteTransId_gg00 )
					return;
				currReq = currReq->objrNxtShareLock_kb05;			
			}
			Trans.trError_gg00 = e_object_not_locked;
		}
    }
    else
        Trans.trError_gg00 = e_object_not_locked;

    if (( Trans.trError_gg00 != e_ok ) && (g01vtrace.vtrKbLock_gg00)) 
    {
        Kernel_VTrace vtrace;
        vtrace << "IsSelfLocked - not locked OID:" 
            << Oid.gg92GetPno() << "."
            << Oid.gg92GetPos()
            << " (vers " << Oid.gg92GetFrameVers() << ")"
			<< " bShare " << bIsSharedLocked;
		if (( 0 == bIsSharedLocked ) && ( NULL != curr ))
            kb501TransToTrace(vtrace, "LockTr", curr->objsExclLockTransId_kb05);
    }
};

/*----------------------------------------------------------------------*/
void
CSlockHash_kb501::DeallocShrLockEntry( tsp00_TaskId          TaskId
                                      ,tkb05_ObjShareLockPtr currReq
                                   ) 
{
    m_SupplySegmentsShare_Lock.Lock();
    CShareLockSegment_kb501 *currListSeg = reinterpret_cast<CShareLockSegment_kb501*>(
        reinterpret_cast<char*>(currReq - currReq->objrOwnIndex_kb05) - sizeof(currListSeg->v.s));
    currListSeg->ReleaseEntry( m_SupplySegmentsShare_Lock, currReq, TaskId);
    // m_SupplySegmentsShare_Lock.Unlock(); done within ReleaseEntry
};

/*----------------------------------------------------------------------*/

tgg00_TransIndex
CSlockHash_kb501::RemoveCollisionEntry( int                      hashvalue
                                  ,tkb05_ObjCollisionEntry &CollEntry )
{
    tgg00_TransIndex oldcollidx = cgg_nil_transindex;

    g_CSlockPtr->Lock(hashvalue);
    tkb05_ObjShareLockCtrlPtr curr = FindLockCntrlEntry(hashvalue, CollEntry.objcOid_kb05);
    if (( NULL != curr ) &&
       ( curr->objsCollLock_kb05 == CollEntry.objcOwnIndex_kb05 )) 
    {
        oldcollidx = curr->objsCollLock_kb05;
        curr->objsCollLock_kb05 = cgg_nil_transindex;
        m_StatisticCounter_Lock.Lock();
        m_CollisionEntryUseCnt--;
        m_StatisticCounter_Lock.Unlock();
    }
    g_CSlockPtr->Unlock(hashvalue);

    return oldcollidx;
};

/*----------------------------------------------------------------------*/

void
CSlockHash_kb501::StoreCollisionEntry( int                      hashvalue
                                      ,tkb05_ObjCollisionEntry &CollEntry )
{
    tgg00_TransIndex collidx = cgg_nil_transindex;

    g_CSlockPtr->Lock(hashvalue);
    tkb05_ObjShareLockCtrlPtr curr = FindLockCntrlEntry(hashvalue, CollEntry.objcOid_kb05);
    if (( NULL != curr ) && ( cgg_nil_transindex == curr->objsCollLock_kb05 ))
    {
        collidx = curr->objsCollLock_kb05;
        curr->objsCollLock_kb05 = CollEntry.objcOwnIndex_kb05;
        m_StatisticCounter_Lock.Lock();
        m_CollisionEntryUseCnt++;
        m_StatisticCounter_Lock.Unlock();
    }
    else
    {
        Kernel_OpError( kbMsg_csp03, csp3_n_lock )
            << "StoreCollisionEntry " 
            << " OID:"  << CollEntry.objcOid_kb05.gg92GetPno() << "."
            << CollEntry.objcOid_kb05.gg92GetPos()
            << " (vers " << CollEntry.objcOid_kb05.gg92GetFrameVers() << ")"
            << " found collision index:" << collidx;
        g01abort (kbMsg_csp03, csp3_n_lock,
            "StoreCollEntry          ", CollEntry.objcOwnIndex_kb05);
    }
    g_CSlockPtr->Unlock(hashvalue);
};

/*----------------------------------------------------------------------*/
CSlockHash_kb501::CSlockHash_kb501( )
    : m_LockCntrlSupplySegments(NULL)
    , m_ShareLockSupplySegments(NULL)
    , m_CollisionEntryUseCnt(0)
{
   m_MinTrans.gg90SetZero(); 
   m_SupplySegmentsShare_Lock.SetIdentifier((SAPDB_UTF8*)"OmsShareLock SupplySegment ShrLckEntry");
   m_SupplySegmentsCntrl_Lock.SetIdentifier((SAPDB_UTF8*)"OmsShareLock SupplySegment CntrlEntry");
   m_StatisticCounter_Lock.SetIdentifier((SAPDB_UTF8*)"OmsShareLock StatisticCounter");
};

/*----------------------------------------------------------------------*/

void
CSlockHash_kb501::GetStatistics ( int &LockCntrlEntries
                                 ,int &LockCntrlEntriesUsed
                                 ,int &ShareLockEntries
                                 ,int &ShareLockEntriesUsed
                                 ,int &CollisionEntriesUsed)
{

    LockCntrlEntries       = 0;
    LockCntrlEntriesUsed   = 0;
    m_SupplySegmentsCntrl_Lock.Lock();
    CShareLockCntrlSegment_kb501 *currListCntrlSeg = m_LockCntrlSupplySegments;
    while ( currListCntrlSeg != NULL )
    {
        LockCntrlEntries       = LockCntrlEntries + currListCntrlSeg->EntryCnt();
        LockCntrlEntriesUsed   = LockCntrlEntriesUsed + currListCntrlSeg->EntryUseCnt();
        currListCntrlSeg = currListCntrlSeg->NextSeg();
    }
    m_SupplySegmentsCntrl_Lock.Unlock();

    ShareLockEntries       = 0;
    ShareLockEntriesUsed   = 0;
    m_SupplySegmentsShare_Lock.Lock();
    CShareLockSegment_kb501 *currListSeg = m_ShareLockSupplySegments;
    while ( currListSeg != NULL )
    {
        ShareLockEntries       = ShareLockEntries + currListSeg->EntryCnt();
        ShareLockEntriesUsed   = ShareLockEntriesUsed + currListSeg->EntryUseCnt();
        currListSeg = currListSeg->NextSeg();
    }
    m_SupplySegmentsShare_Lock.Unlock();
    CollisionEntriesUsed =  m_CollisionEntryUseCnt;
};  


/*----------------------------------------------------------------------*/

void
CSlockHash_kb501::GetLockShare( int                       hashvalue
                               ,tgg00_TransContext       &Trans
                               ,tsp00_C8                 &ObjFileNo
                               ,tgg00_ObjTransInfo       &ObjTransInfo
                               ,tkb05_KernelOid          &Oid
                               ,tkb05_ObjShareLockPtr    &LockReq
                               ,pasbool                   ignColEntry
                               ,bool                     &bAlreadyLocked )
{
    bool bAlreadyShareLocked = false;
    bAlreadyLocked           = false;
   
    tkb05_ObjShareLockCtrlPtr delCtrlEntry = NULL;
    tkb05_ObjShareLockCtrlPtr curr         = NULL;
    {
        CShrLockSync_autoptr_kb501 lock( g_CSlockPtr, hashvalue );
        tkb05_ObjShareLockPtr currReq = AllocShrLockEntry( Trans );
        if ( NULL != currReq ) 
        {
            lock.Lock();
            curr = FindLockCntrlEntry( hashvalue, Oid );

            /* if a file is dropped, lock-entries may be removed */ 
            if (( NULL != curr ) && (curr->objsFileDirFileId_kb05() != ObjFileNo ))
            {
                lock.Unlock();
                if (g01vtrace.vtrKbLock_gg00)
                {
                    Kernel_VTrace() << "GetLockShare - DestroyAllShareLockEntries";
                }
                DestroyAllShareLockEntries(hashvalue, Trans.trTaskId_gg00, Oid);
                lock.Lock();
                curr = FindLockCntrlEntry(hashvalue, Oid);
            }

            if ( curr == NULL ) 
            {
                bool bChainLockCntrlEntry = true;
                lock.Unlock();
                curr = AllocLockCntrlEntry( Trans, ObjFileNo );
                lock.Lock();
                tkb05_ObjShareLockCtrlPtr lce = FindLockCntrlEntry( hashvalue, Oid );
                if ( NULL != lce )
                {
                    curr->objsNxtShareLockCtrl_kb05 = delCtrlEntry;
                    delCtrlEntry = curr;
                    curr = lce;
                    bChainLockCntrlEntry = false;
                }
                else
                {
                    if ( NULL != curr ) 
                    {
                        curr->objsOid_kb05                = Oid;
                        /* move object header information to lockentry, because */
                        /* object header will be overwritten with marker for    */
                        /* sharelock-entry used (TransId_kb05.gg90SetZero())    */
                        if ( ! ObjTransInfo.otrLockTransId_gg00.gg90IsZero() ) 
                        {
                            curr->objsExclLockTransId_kb05    = ObjTransInfo.otrLockTransId_gg00;
                            curr->objsExclLockTransIndex_kb05 = ObjTransInfo.otrLockTransIndex_gg00;
                        }
                        if (bChainLockCntrlEntry)
                            ChainLockCtrlEntryToHash(hashvalue, curr);
                    }
                    else
                        Trans.trError_gg00 = e_too_many_lock_requests;
                }
            }

            if ( e_ok == Trans.trError_gg00 ) 
            {
                bool bMoveLockInfo = false;
                if (  !curr->objsExclLockTransId_kb05.gg90IsNil() ) 
                {
                    if ( Trans.trWriteTransId_gg00 != curr->objsExclLockTransId_kb05 ) 
                    {
                        if ( IsTransActive( curr->objsExclLockTransIndex_kb05
                            , curr->objsExclLockTransId_kb05 ))
                            Trans.trError_gg00 = e_lock_collision;
                        else
                            bMoveLockInfo = true;
                    }
                    else {
                        bAlreadyLocked = true;
                    }
                }
                else 
                    bMoveLockInfo = true;
                
                if (bMoveLockInfo)
                {
                    /* rollback of exclusive lock before share lock may have */
                    /* resetted the object header without releasing this     */
                    /* structure                                             */
                    if ( ! ObjTransInfo.otrLockTransId_gg00.gg90IsZero() ) 
                    {
                        curr->objsExclLockTransId_kb05    = ObjTransInfo.otrLockTransId_gg00;
                        curr->objsExclLockTransIndex_kb05 = ObjTransInfo.otrLockTransIndex_gg00;
                        if ( Trans.trWriteTransId_gg00 == curr->objsExclLockTransId_kb05 ) 
                            bAlreadyLocked = true;  
                    }
                }
            }
        } 
        else 
            Trans.trError_gg00 = e_too_many_lock_requests;

        if (g01vtrace.vtrKbLock_gg00)
        {
            Kernel_VTrace vtrace;
            vtrace << "GetLockShare" << "ExclLockTransIdx:" << curr->objsExclLockTransIndex_kb05;
            kb501TransToTrace(vtrace, "ExclLockTransId", curr->objsExclLockTransId_kb05);
        }

        if ( e_ok != Trans.trError_gg00 )  
        {
            lock.Unlock();
            if ( NULL != currReq )
                DeallocShrLockEntry( Trans.trTaskId_gg00, currReq );
        }
        else
        {
            tkb05_ObjShareLockPtr nxtReq = curr->objsObjShareLock_kb05;
            while ( NULL != nxtReq ) 
            {
                /* check if locked by same transaction */
                if ( Trans.trWriteTransId_gg00 == nxtReq->objrSlockTransId_kb05 ) 
                {
                    bAlreadyLocked      = true;
                    bAlreadyShareLocked = true;
                    LockReq             = nxtReq;
                    break;
                }
                nxtReq = nxtReq->objrNxtShareLock_kb05;
            }

            /* if an lock is waiting, don't allow new share locks */
            if ((!bAlreadyShareLocked && !bAlreadyLocked && !ignColEntry)
                &&
                (curr->objsCollLock_kb05 != cgg_nil_transindex))
                Trans.trError_gg00 = e_lock_collision;

            if (( !bAlreadyShareLocked ) && ( e_ok == Trans.trError_gg00))
            {
                currReq->objrSlockTransIndex_kb05 = Trans.trIndex_gg00;
                currReq->objrSlockTransId_kb05    = Trans.trWriteTransId_gg00;
                currReq->objrNxtShareLock_kb05    = curr->objsObjShareLock_kb05;
                curr->objsObjShareLock_kb05       = currReq;
                LockReq = currReq;
            } 
            else 
            {
                lock.Unlock();
                DeallocShrLockEntry(Trans.trTaskId_gg00,currReq);
            }
        }
    }

    /* outside of lock */
    tkb05_ObjShareLockCtrlPtr p = delCtrlEntry;
    while ( NULL != p )
    {
        delCtrlEntry = p->objsNxtShareLockCtrl_kb05;
        DeallocLockCntrlEntry(  Trans.trTaskId_gg00, p );
        p = delCtrlEntry;
    }

};

/*----------------------------------------------------------------------*/
tgg00_TransIndex
CSlockHash_kb501::GetLockExcl( int                 hashvalue
                              ,tgg00_TransContext &Trans
                               ,tsp00_C8           &ObjFileNo
                              ,tkb05_KernelOid    &Oid
                              ,pasbool             ignShrLckCsView
                              ,pasbool             ignColEntry
                              ,bool               &bAlreadyLocked )
{
    tgg00_TransIndex collidx = cgg_nil_transindex;
    bAlreadyLocked           = false;

    CShrLockSync_autoptr_kb501 lock( g_CSlockPtr, hashvalue );
    lock.Lock();
    tkb05_ObjShareLockCtrlPtr curr = FindLockCntrlEntry(hashvalue, Oid);

    /* if a file is dropped, lock-entries may be removed */ 
    if (( NULL != curr ) && (curr->objsFileDirFileId_kb05() != ObjFileNo ))
    {
        lock.Unlock();
        DestroyAllShareLockEntries(hashvalue, Trans.trTaskId_gg00, Oid);
        lock.Lock();
        curr = FindLockCntrlEntry(hashvalue, Oid);
    }

    /* if no entry is found, then objectframe assumed to be free */
    /* no further checks have to be done to lock the object      */
    /* garbagecollection may have removed the entry              */
    /* or after a crash and restart, some objectframes may still */
    /* be marked as share locked                                 */
    if ( NULL != curr )
    {
        collidx = curr->objsCollLock_kb05;

        /* check if already locked exclusively  */
        if ( curr->objsExclLockTransId_kb05 != Trans.trWriteTransId_gg00 ) 
        {
            /* check if last excl lock has been released by trans and can be accessed */
            if (  !curr->objsExclLockTransId_kb05.gg90IsNil() ) 
            {
                if ( IsTransActive( curr->objsExclLockTransIndex_kb05
                    , curr->objsExclLockTransId_kb05 ))
                {
                    if (( Trans.trError_gg00 != e_ok ) && (g01vtrace.vtrKbLock_gg00)) 
                    {
                        Kernel_VTrace vtrace;
                        vtrace << "GetLockExcl - e_lock_collision";
                        kb501TransToTrace(vtrace, "LockTr", curr->objsExclLockTransId_kb05);
                    }
                    Trans.trError_gg00 = e_lock_collision;
                }
            }

            /* now check if there are share locks, which have to be considered */
            bool NeedConsistentCheck = false;
            tkb05_ObjShareLockPtr firstReq;
            tkb05_ObjShareLockPtr prevReq;
            tkb05_ObjShareLockPtr currReq = curr->objsObjShareLock_kb05;

            while (( e_ok == Trans.trError_gg00) && ( NULL != currReq )) 
            {
                /* check if locked by same transaction */
                if ( Trans.trWriteTransId_gg00 != currReq->objrSlockTransId_kb05 ) 
                {
                    /* in case of omsNewConsistentView or                   */
                    /* rollback of transaction which aquired the lock       */
                    /* only end of transaction has to be checked            */
                    if ( ignShrLckCsView || currReq->objrLockRollbacked ) 
                    {
                        if ( IsTransActive( currReq->objrSlockTransIndex_kb05
                            , currReq->objrSlockTransId_kb05 ))
                            Trans.trError_gg00 = e_lock_collision;
                    } 
                    else 
                        NeedConsistentCheck = true;
                } else 
                    bAlreadyLocked = true;
                currReq = currReq->objrNxtShareLock_kb05;
            }

            /* if there are entries, which are still in use or have been aquired                */
            /* by a transaction which has been committed -> consistent view has to bee checked  */
            if (( NeedConsistentCheck ) && (Trans.trError_gg00 == e_ok)) do 
            {
                firstReq = curr->objsObjShareLock_kb05;
                currReq = firstReq;
                curr->objsInuseWithoutLock_kb05++;
                lock.Unlock();

                pasbool bCancelled;
                pasbool bConsistent = FALSE;
                while (( e_ok == Trans.trError_gg00) && ( NULL != currReq )) 
                {
                    if ( Trans.trWriteTransId_gg00 != currReq->objrSlockTransId_kb05 ) 
                    {  
                        if ( IsTransActive( currReq->objrSlockTransIndex_kb05
                            , currReq->objrSlockTransId_kb05 ))
                            Trans.trError_gg00 = e_lock_collision;

                        if ((( e_ok == Trans.trError_gg00 ) && !currReq->objrLockRollbacked ) &&
                            !Trans.trConsistView_gg00.gg90IsNil() )
                        {
                            kb50ObjConsistentCheck (Trans, currReq->objrSlockTransId_kb05, bConsistent, bCancelled);
                            if ( !bConsistent ) 
                                if (ignShrLckCsView)
                                    Trans.trError_gg00 = e_lock_collision;
                                else
                                    Trans.trError_gg00 = e_request_timeout;
                        }
                    }
                    currReq = currReq->objrNxtShareLock_kb05;
                }

                lock.Lock();
                curr->objsInuseWithoutLock_kb05--;
                currReq = curr->objsObjShareLock_kb05;
            } while (( e_ok == Trans.trError_gg00) && ( firstReq != currReq ));

            if ( e_ok == Trans.trError_gg00)
            {
                /* if an excl lock is waiting, don't allow this entry to be locked */
                if (( collidx == cgg_nil_transindex ) || ignColEntry )  
                {
                        curr->objsExclLockTransId_kb05    = Trans.trWriteTransId_gg00;
                        curr->objsExclLockTransIndex_kb05 = Trans.trIndex_gg00;
                } 
                else 
                {
                    if ( collidx != cgg_nil_transindex )
                            Trans.trError_gg00 = e_lock_collision;
                }
            }


            if (( curr->objsReleaseLocks_kb05 != FALSE )
                && ( curr->objsInuseWithoutLock_kb05 == 0 ))
            {
                tkb05_ObjShareLockPtr delEntryList = NULL;
                prevReq = NULL;
                currReq = curr->objsObjShareLock_kb05;
                while ( currReq != NULL ) 
                {
                    if ( currReq->objrSlockTransId_kb05.gg90IsNil() ) 
                    {
                        if ( prevReq == NULL ) 
                        {
                            curr->objsObjShareLock_kb05    = currReq->objrNxtShareLock_kb05;
                            currReq->objrNxtShareLock_kb05 = delEntryList;
                            delEntryList                   = currReq;
                            currReq = curr->objsObjShareLock_kb05;
                        } 
                        else 
                        {
                            prevReq->objrNxtShareLock_kb05 = currReq->objrNxtShareLock_kb05;
                            currReq->objrNxtShareLock_kb05 = delEntryList;
                            delEntryList                   = currReq;
                            currReq = prevReq->objrNxtShareLock_kb05;
                        }
                    } 
                    else 
                    {
                        prevReq = currReq;
                        currReq = currReq->objrNxtShareLock_kb05;
                    }
                }
                curr->objsReleaseLocks_kb05 = FALSE;

                lock.Unlock();
                /* now release all entries outside of spin-lock */
                currReq = delEntryList;
                while ( NULL != currReq )
                {
                    delEntryList = currReq->objrNxtShareLock_kb05;
                    DeallocShrLockEntry(Trans.trTaskId_gg00,currReq);
                    currReq = delEntryList;
                }
                // lock.Lock();    why ????
            }
        } else
            bAlreadyLocked = true;
    } else {
        Trans.trError_gg00 = e_object_not_locked;
    }

    if (( Trans.trError_gg00 == e_ok ) && (g01vtrace.vtrKbLock_gg00)) 
    {
        Kernel_VTrace vtrace;
        vtrace << "GetLockExcl - e_ok";
        kb501TransToTrace(vtrace, "LockTr", curr->objsExclLockTransId_kb05);
        if ( ignColEntry )
            vtrace << " may be locked";
        else
            vtrace << " has been locked";
    }
    return collidx;
};

/*----------------------------------------------------------------------*/

void
CSlockHash_kb501::DeallocLockCntrlEntry( tsp00_TaskId TaskId
                               ,tkb05_ObjShareLockCtrlPtr curr )
{
    if (curr != NULL)
    {
        m_SupplySegmentsCntrl_Lock.Lock();
        CShareLockCntrlSegment_kb501 *currListSeg = reinterpret_cast<CShareLockCntrlSegment_kb501*>(
            reinterpret_cast<char*>(curr - curr->objsOwnIndex_kb05) - sizeof(currListSeg->v.s));
        currListSeg->ReleaseEntry( m_SupplySegmentsCntrl_Lock, curr, TaskId);
        // m_SupplySegmentsCntrl_Lock.Unlock();  done within ReleaseEntry
    }
};

/*----------------------------------------------------------------------*/

tgg00_TransIndex
CSlockHash_kb501::GetCollisionIndex( int                  hashvalue
                                    ,tkb05_KernelOid     &Oid
                                    ,tgg00_BasisError    &error)
{
    tkb05_ObjShareLockCtrlPtr curr = FindLockCntrlEntry(hashvalue, Oid);
    if ( NULL != curr )
        return curr->objsCollLock_kb05;
    else 
    {
        error = e_object_not_locked;
        return cgg_nil_transindex;
    }
};

/*----------------------------------------------------------------------*/

void
CSlockHash_kb501::DestroyAllShareLockEntries( int              hashvalue
                                             ,tsp00_TaskId     TaskId
                                             ,tkb05_KernelOid &Oid )
{
    tkb05_ObjShareLockPtr      currReq;
    tkb05_ObjShareLockPtr      prevReq      = NULL;
    tkb05_ObjShareLockPtr      delEntryList = NULL;
    tkb05_ObjShareLockCtrlPtr  delCtrlEntry = NULL;
    g_CSlockPtr->Lock(hashvalue);
    tkb05_ObjShareLockCtrlPtr curr = FindLockCntrlEntry(hashvalue, Oid);
    if (NULL != curr) 
    {
        currReq = curr->objsObjShareLock_kb05;

        while ( NULL != currReq )
        {
            /* check if lock has already been aquired by a new transaction */
            if ( IsTransActive( currReq->objrSlockTransIndex_kb05
                , currReq->objrSlockTransId_kb05 ))
            {
                prevReq = currReq;
                currReq = currReq->objrNxtShareLock_kb05;
            }
            else
            {
                if ( curr->objsInuseWithoutLock_kb05 > 0 ) {
                    curr->objsReleaseLocks_kb05       = TRUE;
                    currReq->objrSlockTransId_kb05.gg90SetNil();
                    currReq->objrSlockTransIndex_kb05 = cgg_nil_transindex;
                    currReq->objrLockRollbacked       = TRUE;
                    currReq = currReq->objrNxtShareLock_kb05;
                } 
                else 
                {
                    /* unchain */
                    if ( prevReq == NULL )
                    {
                        curr->objsObjShareLock_kb05    = currReq->objrNxtShareLock_kb05;
                        /* chain to 'to be deleted' list */
                        currReq->objrNxtShareLock_kb05 = delEntryList;
                        delEntryList                   = currReq;
                        currReq                        = curr->objsObjShareLock_kb05;
                    }
                    else
                    {
                        prevReq->objrNxtShareLock_kb05 = currReq->objrNxtShareLock_kb05;
                        /* chain to 'to be deleted' list */
                        currReq->objrNxtShareLock_kb05 = delEntryList;
                        delEntryList                   = currReq;
                        currReq                        = prevReq->objrNxtShareLock_kb05;
                    }
                }
            }
        }

        /* remove complete share lock entry if possible */
        if (( NULL == curr->objsObjShareLock_kb05 ) &&
            ( cgg_nil_transindex == curr->objsCollLock_kb05 ) &&
            ( curr->objsInuseWithoutLock_kb05 == 0) &&
            ( !IsTransActive( curr->objsExclLockTransIndex_kb05
            , curr->objsExclLockTransId_kb05 )))
        {
            if (g01vtrace.vtrKbLock_gg00) 
            {
                Kernel_VTrace() << "RemoveLockCtrl:" 
                    << curr->objsOid_kb05.gg92GetPno() << "."
                    << curr->objsOid_kb05.gg92GetPos()
                    << " (vers " << curr->objsOid_kb05.gg92GetFrameVers() << ")";
            }
            RemoveLockCtrlEntryFromHash(hashvalue, curr);
            delCtrlEntry = curr;
        }
    }
    g_CSlockPtr->Unlock(hashvalue);

    /* now release all entries outside of spin-lock */
    currReq = delEntryList;
    while ( NULL != currReq )
    {
        delEntryList = currReq->objrNxtShareLock_kb05;
        DeallocShrLockEntry(TaskId,currReq);
        currReq = delEntryList;
    }

    /* now release cntrl entry outside of spin-lock */
    if ( NULL != delCtrlEntry )
    {
        DeallocLockCntrlEntry( TaskId, delCtrlEntry );
    }
}

/*----------------------------------------------------------------------*/

void
CSlockHash_kb501::SetMinTrans ( tsp00_TaskId    TaskId
                               ,tgg91_TransNo  &MinTrans )
{
    CShareLockSegment_kb501 dummySeq;
    int LockCntrlEntriesPerSupplySegments = 
        ( sizeof(dummySeq) - dummySeq.SizeOfHdr() ) / 
        sizeof(tkb05_ObjShareLockCtrl);
    int ShareLockEntriesPerSupplySegments = 
        ( sizeof(dummySeq) - dummySeq.SizeOfHdr() ) / 
        sizeof(tkb05_ObjShareLock);
    int LockCntrlEntries;
    int LockCntrlEntriesUsed;
    int ShareLockEntries;
    int ShareLockEntriesUsed;
    int CollisionEntriesUsed;
    GetStatistics( LockCntrlEntries, LockCntrlEntriesUsed, ShareLockEntries,
        ShareLockEntriesUsed, CollisionEntriesUsed );

    m_StatisticCounter_Lock.Lock();
    m_MinTrans = MinTrans;
    m_StatisticCounter_Lock.Unlock();

    if ((( LockCntrlEntries - LockCntrlEntriesUsed ) > ( LockCntrlEntriesPerSupplySegments + 50 )) ||
        (( ShareLockEntries - ShareLockEntriesUsed ) > ( ShareLockEntriesPerSupplySegments + 100 )))
    {
        CollectGarbage( TaskId, m_MinTrans );
    } 
};

/*----------------------------------------------------------------------*/

tgg00_TransIndex
CSlockHash_kb501::UnlockExcl(tgg91_TransNo       &UpdTransId_gg00
                             ,int                  hashvalue
                             ,COidListEntry_kb501 &ListEntry)
{
    tkb05_ObjShareLockCtrlPtr     curr      = FindLockCntrlEntry(hashvalue, ListEntry.oid);
    tgg00_TransIndex collEntry = cgg_nil_transindex;

    if ((NULL != curr) && (curr->objsExclLockTransId_kb05 == UpdTransId_gg00))
    {
        curr->objsExclLockTransId_kb05.gg90SetNil();
        curr->objsExclLockTransIndex_kb05 = cgg_nil_transindex;
        collEntry = curr->objsCollLock_kb05;
    }
    return collEntry;
};

/*----------------------------------------------------------------------*/

tgg00_TransIndex
CSlockHash_kb501::UnlockShare( int                  hashvalue
                              ,tsp00_TaskId         TaskId
                              ,tgg00_TransIndex     TransIdx
                              ,COidListEntry_kb501 &ListEntry
                              ,tgg91_TransNo       &ThisWriteTransId 
                              ,bool                 isCommitted
                              ,tgg91_TransNo       &ShareLockTransId)
{
    tkb05_ObjShareLockCtrlPtr  curr          = FindLockCntrlEntry(hashvalue, ListEntry.oid);
    tgg00_TransIndex      collEntry          = cgg_nil_transindex;
    bool                  existActiveShrLock = false;
    tkb05_ObjShareLockPtr delEntryList       = NULL;
    ShareLockTransId.gg90SetNil();

    if (NULL != curr) 
    {
        tkb05_ObjShareLockPtr currReq = curr->objsObjShareLock_kb05;

        m_StatisticCounter_Lock.Lock();
        tgg91_TransNo  MinTrans = m_MinTrans;
        m_StatisticCounter_Lock.Unlock();

        if ( ListEntry.isShareLocked ) 
        {
            tkb05_ObjShareLockPtr prevReq = NULL;
            while ( NULL != currReq )
            {
                /* rollbacked share locks from same session aren't needed anymore  */
                if (( currReq->objrLockRollbacked && 
                    ( currReq->objrSlockTransIndex_kb05 == TransIdx ))
                    ||
                    /* share locks being committed by older transactions on this session  */
                    /* aren't needed anymore, if actual transaction committed.            */
                    ( isCommitted &&
                    ( currReq->objrSlockTransIndex_kb05 == TransIdx ) &&
                    ( ThisWriteTransId > currReq->objrSlockTransId_kb05 )) 
                    ||
                    /* remove share locks created by transactions, which run before actual Mintrans */
                    ( MinTrans > currReq->objrSlockTransId_kb05 ))
                {
                    if ( curr->objsInuseWithoutLock_kb05 > 0 ) 
                    {
                        curr->objsReleaseLocks_kb05       = TRUE;
                        currReq->objrSlockTransId_kb05.gg90SetNil();
                        currReq->objrSlockTransIndex_kb05 = cgg_nil_transindex;
                        currReq->objrLockRollbacked       = TRUE;
                        prevReq                           = currReq;
                        currReq                           = currReq->objrNxtShareLock_kb05;
                    } 
                    else 
                    {
                        if ( NULL == prevReq ) 
                        {
                            curr->objsObjShareLock_kb05 = currReq->objrNxtShareLock_kb05;
                            currReq->objrNxtShareLock_kb05 = delEntryList;
                            delEntryList                   = currReq;
                            currReq                        = curr->objsObjShareLock_kb05;
                        } 
                        else 
                        {
                            prevReq->objrNxtShareLock_kb05 = currReq->objrNxtShareLock_kb05;
                            currReq->objrNxtShareLock_kb05 = delEntryList;
                            delEntryList                   = currReq;
                            currReq                        = prevReq->objrNxtShareLock_kb05;
                        }
                    }
                } 
                else 
                {
                    if ( !existActiveShrLock || !ShareLockTransId.gg90IsNil()) 
                    { 
                        if ( IsTransActive( currReq->objrSlockTransIndex_kb05
                            , currReq->objrSlockTransId_kb05 ))
                        {
                            if ( !existActiveShrLock)
                                ShareLockTransId = currReq->objrSlockTransId_kb05;
                            else
                                ShareLockTransId.gg90SetNil();
                            existActiveShrLock = true;
                        }
                    }
                    prevReq = currReq;
                    currReq = currReq->objrNxtShareLock_kb05;
                }
            }
        }

        if ( ListEntry.isExclLocked || !existActiveShrLock) 
        {
            collEntry = curr->objsCollLock_kb05;
        }
    }
    g_CSlockPtr->Unlock(hashvalue);

    /* now release all entries outside of spin-lock */
    tkb05_ObjShareLockPtr currReq = delEntryList;
    while ( NULL != currReq )
    {
        delEntryList = currReq->objrNxtShareLock_kb05;
        DeallocShrLockEntry(TaskId,currReq);
        currReq = delEntryList;
    }
    return collEntry;
}

/*----------------------------------------------------------------------*/
bool
CSlockHash_kb501::GetShareLockInfo( int &hashval
                                   ,CShareLockInfo_kb501 *ShrLckInfoPtr )
{
    const pasbool isExclusive = TRUE;
    const pasbool isShare     = FALSE;
    const pasbool isHolder    = TRUE;

    bool rc = false;
    do
    {
        g_CSlockPtr->Lock(hashval);
        tkb05_ObjShareLockCtrlPtr curr = m_Head[ hashval ].m_List;

        while ( curr )
        {
            /* check if exclusive locked */
            if ( ! curr->objsExclLockTransId_kb05.gg90IsNil()&& 
                IsTransActive( curr->objsExclLockTransIndex_kb05
                , curr->objsExclLockTransId_kb05 ))
            {
                tkb05_TransEntryPtr ThisTrans;

                k52acc_trans_entry( curr->objsExclLockTransIndex_kb05, ThisTrans );

                ShrLckInfoPtr->AddEntry( curr->objsFileDirFileId_kb05()
                    ,curr->objsOid_kb05
                    ,ThisTrans->teTaskId_kb05
                    ,isExclusive
                    ,isHolder); 
                rc = true;
            }
            else
            {
                FileDir_FileNo   fileno = curr->objsFileDirFileId_kb05();
                tgg92_KernelOid  oid    = curr->objsOid_kb05; 
                tkb05_ObjShareLockPtr currReq = curr->objsObjShareLock_kb05;
                while ( currReq )
                {
                    if ( ! currReq->objrSlockTransId_kb05.gg90IsNil()  &&
                        IsTransActive( currReq->objrSlockTransIndex_kb05
                        , currReq->objrSlockTransId_kb05 ))
                    {
                        tkb05_TransEntryPtr ThisTrans;
                        k52acc_trans_entry( currReq->objrSlockTransIndex_kb05, ThisTrans );
                        ShrLckInfoPtr->AddEntry( fileno
                            ,oid
                            ,ThisTrans->teTaskId_kb05
                            ,isShare
                            ,isHolder); 
                        rc = true;
                    }
                    currReq = currReq->objrNxtShareLock_kb05;
                }
            };

            curr = curr->objsNxtShareLockCtrl_kb05;
        }
        g_CSlockPtr->Unlock(hashval);
        ++hashval;
    }
    while ( !rc && ( hashval < HashHdrSize ));
    return rc;
};

/*----------------------------------------------------------------------*/

void  
CSlockHash_kb501::CollectGarbage( tsp00_TaskId    TaskId
                                 ,tgg91_TransNo  &MinTrans ) 
{
    int i;

    for ( i = 0; i < HashHdrSize; i++ ) 
    {
        tkb05_ObjShareLockPtr delEntryList     = NULL;
        tkb05_ObjShareLockCtrlPtr          delCtrlEntryList = NULL;
        g_CSlockPtr->Lock(i);
        tkb05_ObjShareLockCtrlPtr prev = NULL;
        tkb05_ObjShareLockCtrlPtr curr = m_Head[ i ].m_List;
        while (curr)
        {
            /* remove share lock req entries if possible */
            tkb05_ObjShareLockPtr prevReq = NULL;
            tkb05_ObjShareLockPtr currReq = curr->objsObjShareLock_kb05;
            while ( NULL != currReq ) 
            {
                if (  MinTrans > currReq->objrSlockTransId_kb05 ) 
                {
                    if ( IsTransActive( currReq->objrSlockTransIndex_kb05
                        , currReq->objrSlockTransId_kb05 ))
                    {
                        prevReq = currReq;
                        currReq = currReq->objrNxtShareLock_kb05;
                    } 
                    else 
                    {
                        if ( curr->objsInuseWithoutLock_kb05 > 0 ) 
                        {
                            curr->objsReleaseLocks_kb05       = TRUE;
                            currReq->objrSlockTransId_kb05.gg90SetNil();
                            currReq->objrSlockTransIndex_kb05 = cgg_nil_transindex;
                            currReq->objrLockRollbacked       = TRUE;
                            prevReq                           = currReq;
                            currReq                           = currReq->objrNxtShareLock_kb05;
                        } 
                        else 
                        {
                            if ( NULL == prevReq ) 
                            {
                                curr->objsObjShareLock_kb05    = currReq->objrNxtShareLock_kb05;
                                currReq->objrNxtShareLock_kb05 = delEntryList;
                                delEntryList                   = currReq;
                                currReq                        = curr->objsObjShareLock_kb05;
                            } 
                            else 
                            {
                                prevReq->objrNxtShareLock_kb05 = currReq->objrNxtShareLock_kb05;
                                currReq->objrNxtShareLock_kb05 = delEntryList;
                                delEntryList                   = currReq;
                                currReq                        = prevReq->objrNxtShareLock_kb05;
                            }
                        }
                    }
                } 
                else 
                {
                    prevReq = currReq;
                    currReq = currReq->objrNxtShareLock_kb05;
                }
            }

            if ((g01vtrace.vtrKbLock_gg00) && ( NULL == curr->objsObjShareLock_kb05 )) 
            {
                Kernel_VTrace() << " CSlockHash_kb501::GC all share locks released OID:"
                    << curr->objsOid_kb05.gg92GetPno() << "."
                    << curr->objsOid_kb05.gg92GetPos()
                    << " (vers " << curr->objsOid_kb05.gg92GetFrameVers() << ")";
            }

            /* remove complete share lock entry if possible */
            if (( NULL == curr->objsObjShareLock_kb05 ) &&
                ( 0 == curr->objsInuseWithoutLock_kb05 ) &&  
                ( cgg_nil_transindex == curr->objsCollLock_kb05 ) &&
                (( MinTrans > curr->objsExclLockTransId_kb05 ) ||
                ( curr->objsExclLockTransId_kb05.gg90IsNil())) &&
                ( !curr->objsReleaseLocks_kb05 )) 
            {
                if ( IsTransActive( curr->objsExclLockTransIndex_kb05
                    , curr->objsExclLockTransId_kb05 ))
                {
                    prev = curr;
                    curr = curr->objsNxtShareLockCtrl_kb05;
                } 
                else 
                {
                    tkb05_ObjShareLockCtrlPtr p = curr;
                    if ( prev ) 
                    {
                        prev->objsNxtShareLockCtrl_kb05 = curr->objsNxtShareLockCtrl_kb05;
                        curr = prev->objsNxtShareLockCtrl_kb05;
                    } 
                    else 
                    {
                        m_Head[ i ].m_List = curr->objsNxtShareLockCtrl_kb05;
                        curr = m_Head[ i ].m_List;
                    }
                    p->objsNxtShareLockCtrl_kb05 = delCtrlEntryList;
                    delCtrlEntryList             = p;

                    if ( g01vtrace.vtrKbLock_gg00 && ( NULL == p->objsObjShareLock_kb05 )) 
                    {
                        Kernel_VTrace() << " CSlockHash_kb501::GC lock ctrl entry released OID:"
                            << p->objsOid_kb05.gg92GetPno() << "."
                            << p->objsOid_kb05.gg92GetPos()
                            << " (vers " << p->objsOid_kb05.gg92GetFrameVers() << ")";
                    }
                }
            } 
            else 
            {
                prev = curr;
                curr = curr->objsNxtShareLockCtrl_kb05;
            }
        }
        g_CSlockPtr->Unlock(i);

        /* now release all entries outside of spin-lock */
        tkb05_ObjShareLockPtr currReq = delEntryList;
        while ( NULL != currReq )
        {
            delEntryList = currReq->objrNxtShareLock_kb05;
            DeallocShrLockEntry(TaskId,currReq);
            currReq = delEntryList;
        }

        /* now release all entries outside of spin-lock */
        tkb05_ObjShareLockCtrlPtr p = delCtrlEntryList;
        while ( NULL != p )
        {
            delCtrlEntryList = p->objsNxtShareLockCtrl_kb05;
            DeallocLockCntrlEntry( TaskId, p );
            p = delCtrlEntryList;
        }
    }
};

/*----------------------------------------------------------------------*/