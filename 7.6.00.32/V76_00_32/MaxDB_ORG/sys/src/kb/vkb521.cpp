/****************************************************************************

  module      : vkb521.cpp

  -------------------------------------------------------------------------

  responsible : UlrichJ

  special area: SystemViews
  description : This module implements the "ckb521_SQLLockReader" class.

  last changed: 2002-08-14
  see also    :

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


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "ggg00.h"
#include "vak001.h"
#include "ggg91.h"
#include "gkb05.h"
#include "hkb51_2.h"
#include "hkb52.h"
#include "hgg06.h"
#include "Container/Container_List.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "SAPDBCommon/SAPDB_RangeCode.hpp"
#include "vkb521.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS  (PROTOTYPES)                                     *
 *===========================================================================*/

/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

ckb521_SQLLockReader::ckb521_SQLLockReader(SAPDBMem_IRawAllocator	&alloc,
                                         tgg00_TransContext     &trans)
: m_Allocator(alloc),
  m_Trans (trans)
{
    m_Current = 0;
    m_List = new(m_Allocator) SQLLockList(m_Allocator);
}

ckb521_SQLLockReader::~ckb521_SQLLockReader()
{
    FreeAll();
}

SAPDB_Bool ckb521_SQLLockReader::Begin()
{
    m_List->Delete();

    if (ReadList())
    {
        m_Current = m_List->Begin();
        return (true);
    }
    else
    {
        m_Current = m_List->End();
        return (false);
    }
}

void ckb521_SQLLockReader::FreeAll()
{
    if (m_List)
    {
        m_List->Delete();
        m_Current = m_List->End();

        m_Allocator.Deallocate(m_List);
        m_List = NULL;
    }
}

ckb521_SQLLockReader::SQLLockElement * ckb521_SQLLockReader::Next()
{
    SQLLockElement * eRet;

    if (m_Current != m_List->End())
    {
        eRet = &(*m_Current);
        ++m_Current;
        return (eRet);
    }
    else
    {
        return (NULL);
    }
}

SAPDB_Bool ckb521_SQLLockReader::ReadList()
{
    if (ReadLocks())
    {
        if (ReadCollisions())
        {
            return (true);
        }
    }

    return (false);
}

SAPDB_Bool ckb521_SQLLockReader::ReadLocks()
{
    tkb05_RegionGlobPtr     TransRgnGlobPtr;
    tkb05_RegionNo          CurrRegion;
    tkb05_TransEntryPtr     CurrTrans;
    tkb05_TabTransPtr       CurrTab;
    tkb05_RowTransPtr       CurrRow;
    SAPDB_Int4              TransLimit;
    SAPDB_Int4              ItemLimit;
    SAPDB_Int4              TransCnt;
    SAPDB_Int4              TabItemCnt;
    SAPDB_Int4              ItemCnt;
    SAPDB_Bool              IsTabLock;
    SAPDB_Bool              IsTabReq;

    
    m_Trans.trError_gg00 = e_ok;

    CurrRegion = 1;

    while ((e_ok == m_Trans.trError_gg00) && 
           (CurrRegion <= k51glob.glTransRegions_kb05))
    {
    
        k52begexcl_region( m_Trans.trTaskId_gg00, CurrRegion, TransRgnGlobPtr );

        TransLimit  = k52used_trans_of_region(*TransRgnGlobPtr);
        ItemLimit   = k52used_items_of_region(*TransRgnGlobPtr);
        CurrTrans   = TransRgnGlobPtr->rglTransGlob_kb05->trFirst_kb05;
        TransCnt    = 0;

        while ((e_ok == m_Trans.trError_gg00) &&
               (NULL != CurrTrans) &&
               (TransCnt <= TransLimit))
        {
        
            CurrTab         = CurrTrans->teFirstTab_kb05;
            TabItemCnt      = 0;

            while ((e_ok == m_Trans.trError_gg00) &&
                   (NULL != CurrTab) &&
                   (TabItemCnt <= ItemLimit))
            {

                // -------------- Check table locks ------------

                IsTabLock = (CurrTab->ttLockMode_kb05 != lckFree_egg00);
                IsTabReq  = (CurrTab->ttReqMode_kb05 == lckSysShare_egg00) ||
                            (CurrTab->ttReqMode_kb05 == lckSysExcl_egg00 ) ||
                            (CurrTab->ttReqMode_kb05 == lckTabShare_egg00) ||
                            (CurrTab->ttReqMode_kb05 == lckTabExcl_egg00 );

                if (IsTabLock || IsTabReq)
                {
                    AddTableLock(CurrTrans, CurrTab, IsTabLock, IsTabReq);
                }

                // -------------- Check row locks ------------

                CurrRow = CurrTab->ttFirstRow_kb05;

                if ((e_ok == m_Trans.trError_gg00) &&
                    (NULL != CurrRow))
                {
                    ItemCnt = 0;
                    
                    // -------------- Process rows backwards --------------

                    while ((NULL != CurrRow->rtNextRow_kb05) &&
                           (ItemCnt <= ItemLimit))
                    {
                        CurrRow = CurrRow->rtNextRow_kb05;
                        ItemCnt++;
                    }

                    if (ItemCnt > ItemLimit)
                    {
                        PutLoopError ("kb521 : ReadList", (SAPDB_Int4)1);
                    }

                    ItemCnt = 0;

                    while ((e_ok == m_Trans.trError_gg00) &&
                           (NULL != CurrRow) &&
                           (ItemCnt <= ItemLimit))
                    {
                        if (NULL != CurrRow->rtRow_kb05)
                        {
                            AddRowLock (CurrTrans, CurrTab, CurrRow);
                        }

                        CurrRow = CurrRow->rtPrevRow_kb05;
                        ItemCnt++;
                    }

                    if (ItemCnt > ItemLimit)
                    {
                        PutLoopError ("kb521 : ReadList" , 2);
                    }
                }

                TabItemCnt++;
                CurrTab = CurrTab->ttNextTab_kb05;
            }

            if (TabItemCnt > ItemLimit)
            {
                PutLoopError ("kb521 : ReadList", 3);
                return (false);
            }

            TransCnt++;
            CurrTrans = CurrTrans->teNextTrans_kb05;
        }

        k52endexcl_region (m_Trans.trTaskId_gg00, TransRgnGlobPtr);

        if (TransCnt > TransLimit)
        {
            PutLoopError ("kb521 : ReadList", 4);
            return (false);
        }

        CurrRegion++;
    }

    

    return (true);
}

SAPDB_Bool ckb521_SQLLockReader::ReadCollisions()
{
    SAPDB_Int4              CurrCollision;
    tkb05_ObjCollGlobPtr    ObjCollGlobPtr;
    tkb05_ObjCollEntryPtr   CurrObjColl;
    SAPDB_Int4              ReqCnt;
    tgg00_TransIndex        CurrReq;
    tkb05_ObjReqEntryPtr    ReqEntry;
    SAPDB_Bool              IsRequest = true;


    m_Trans.trError_gg00    = e_ok;
    CurrCollision           = 1;

    k52begexcl_objcoll (m_Trans.trTaskId_gg00, ObjCollGlobPtr);

    while ((m_Trans.trError_gg00 == e_ok) && 
           (CurrCollision <= k51glob.glTransEntries_kb05))
    {
    
        k52acc_objcoll_entry (*ObjCollGlobPtr, CurrCollision, CurrObjColl);

        if (!gg06IsNilOid (CurrObjColl->objcOid_kb05))
        {
        
            // ------- Obj collisions -----------
            
            AddObjToList (CurrObjColl->objcLockTransIndex_kb05, CurrObjColl->objcOid_kb05, !IsRequest);

            ReqCnt = 0;
            CurrReq = CurrObjColl->objcFirstReq_kb05;

            while ((m_Trans.trError_gg00 == e_ok) &&
                   (CurrReq != cgg_nil_transindex) &&
                   (ReqCnt <= k51glob.glTransEntries_kb05))
            {
            
                // ------- Requests ----------------
            
                AddObjToList (CurrReq, CurrObjColl->objcOid_kb05, IsRequest);

                k52acc_objreq_entry (*ObjCollGlobPtr, CurrReq, ReqEntry);
                CurrReq = ReqEntry->objrNextReq_kb05;
                ReqCnt++;

            }

            if (ReqCnt > k51glob.glTransEntries_kb05)
            {
                PutLoopError ("kb521 : ReadCollisions", 1);
                return (false);
            }
        }

        CurrCollision++;
    }

    k52endexcl_objcoll (m_Trans.trTaskId_gg00, ObjCollGlobPtr);

    return (true);
}

void ckb521_SQLLockReader::PutLoopError (const SAPDB_Char    *msg,
                                        SAPDB_Int4          iError)
{
    k52x_loop_error ((tsp00_C24 &)msg, iError);
}

void ckb521_SQLLockReader::AddTableLock (tkb05_TransEntryPtr     &CurrTrans,
                                        tkb05_TabTransPtr       &CurrTab,
                                        SAPDB_Bool                    &IsTabLock,
                                        SAPDB_Bool                    &IsTabReq)
{
    SQLLockElement    toAdd;
    SAPDB_UInt4       i;

    toAdd.sleTransID_kb521 = CurrTrans->teSelectTransId_kb05;

    if (cgg_nil_transindex != CurrTrans->teObjReq_kb05)
    {
        toAdd.sleLockReqTimeout_kb521 = CurrTrans->teObjReqTimeout_kb05;
    }
    else
    {
        toAdd.sleLockReqTimeout_kb521 = CurrTrans->teReqTimeout_kb05;
    }

    if (IsTabLock)
    {
        toAdd.sleLockMode_kb521  = CurrTab->ttLockMode_kb05;
        toAdd.sleLockState_kb521 = CurrTab->ttLockState_kb05;
    }
    else
    {
        toAdd.sleLockMode_kb521  = lckFree_egg00;
        toAdd.sleLockState_kb521.clear();
    }

    if (IsTabReq)
    {
        toAdd.sleReqMode_kb521   = CurrTab->ttReqMode_kb05;
        toAdd.sleReqState_kb521  = CurrTab->ttReqState_kb05;
    }
    else
    {
        toAdd.sleReqMode_kb521   = lckFree_egg00;
        toAdd.sleReqState_kb521.clear();
    }

    toAdd.sleTblID_kb521      = CurrTab->ttTabId_kb05;
    toAdd.sleRowIDUndef_kb521 = true;
    toAdd.sleRowIDLen_kb521   = 0;

    for (i=0; i<sizeof(toAdd.sleRowIDHex_kb521); i++)
    {
        toAdd.sleRowIDHex_kb521[i] = '\0';
    }

    m_List->InsertEnd(toAdd);
}


void ckb521_SQLLockReader::AddRowLock (tkb05_TransEntryPtr     &CurrTrans,
                                      tkb05_TabTransPtr       &CurrTab,
                                      tkb05_RowTransPtr       &CurrRow)
{
    SQLLockElement    toAdd;
    SAPDB_UInt4       iSize;
    SAPDB_UInt4       i;

    iSize = 6;

    toAdd.sleTransID_kb521 = CurrTrans->teSelectTransId_kb05;

    if (cgg_nil_transindex != CurrTrans->teObjReq_kb05)
    {
        toAdd.sleLockReqTimeout_kb521 = CurrTrans->teObjReqTimeout_kb05;
    }
    else
    {
        toAdd.sleLockReqTimeout_kb521 = CurrTrans->teReqTimeout_kb05;
    }

    toAdd.sleLockMode_kb521     = CurrRow->rtLockMode_kb05;
    toAdd.sleLockState_kb521    = CurrRow->rtLockState_kb05;;
    toAdd.sleReqMode_kb521      = CurrRow->rtReqMode_kb05;;
    toAdd.sleReqState_kb521     = CurrRow->rtReqState_kb05;;
    toAdd.sleTblID_kb521        = CurrTab->ttTabId_kb05;
    toAdd.sleRowIDUndef_kb521   = false;
    toAdd.sleRowIDLen_kb521     = CurrRow->rtKeyLen_kb05;

    if (CurrRow->rtKeyLen_kb05 > sizeof (CurrRow->rtShortKey_kb05))
    {
        if (CurrRow->rtKeyLen_kb05 > sizeof(toAdd.sleRowIDHex_kb521))
        {
            SAPDB_MemMoveNoCheck (toAdd.sleRowIDHex_kb521, CurrRow->rtRow_kb05->rlRowIdItem_kb05->riRowId_kb05, sizeof(toAdd.sleRowIDHex_kb521));
        }
        else
        {
            SAPDB_MemMoveNoCheck (toAdd.sleRowIDHex_kb521, CurrRow->rtRow_kb05->rlRowIdItem_kb05->riRowId_kb05, CurrRow->rtKeyLen_kb05);
        }
    }
    else
    {
        SAPDB_MemMoveNoCheck( toAdd.sleRowIDHex_kb521, CurrRow->rtShortKey_kb05, CurrRow->rtKeyLen_kb05);

        for (i=CurrRow->rtKeyLen_kb05;i<sizeof(toAdd.sleRowIDHex_kb521);i++)
        {
            toAdd.sleRowIDHex_kb521[i] = '\0';
        }
    }

    m_List->InsertEnd(toAdd);
}


void ckb521_SQLLockReader::AddObjToList (tgg00_TransIndex    &TransIndex, 
                                        tgg92_KernelOid      &Oid, 
                                        SAPDB_Bool           bIsRequest)
{
    SQLLockElement          toAdd;
    tkb05_TransEntryPtr     CurrTrans;
    SAPDB_Int4              len = 5;
    SAPDB_UInt4             i;


    k52acc_trans_entry (TransIndex, CurrTrans);

    toAdd.sleTransID_kb521 = CurrTrans->teSelectTransId_kb05;
    
    if (cgg_nil_transindex != CurrTrans->teObjReq_kb05)
    {
        toAdd.sleLockReqTimeout_kb521 = CurrTrans->teObjReqTimeout_kb05;
    }
    else
    {
        toAdd.sleLockReqTimeout_kb521 = CurrTrans->teReqTimeout_kb05;
    }

    if (bIsRequest)
    {
        toAdd.sleLockMode_kb521 = lckRowExcl_egg00;
        toAdd.sleReqMode_kb521 = lckFree_egg00;
    }
    else
    {
        toAdd.sleLockMode_kb521 = lckFree_egg00;
        toAdd.sleReqMode_kb521 = lckRowExcl_egg00;
    }

    toAdd.sleLockState_kb521.clear();
    toAdd.sleReqState_kb521.clear();

    SAPDB_MemMoveNoCheck (toAdd.sleTblID_kb521, cgg_zero_id, sizeof(cgg_zero_id));

    toAdd.sleRowIDHex_kb521[0] = ' ';
    toAdd.sleRowIDHex_kb521[1] = 'O';
    toAdd.sleRowIDHex_kb521[2] = 'I';
    toAdd.sleRowIDHex_kb521[3] = 'D';
    toAdd.sleRowIDHex_kb521[4] = ' ';

    gg06OidToLine (Oid, len, *(reinterpret_cast<tsp00_C132 *>(&(toAdd.sleRowIDHex_kb521))));

    toAdd.sleRowIDLen_kb521 = len;
    toAdd.sleRowIDUndef_kb521 = false;

    for (i=len; i<sizeof(toAdd.sleRowIDHex_kb521); i++)
    {
        toAdd.sleRowIDHex_kb521[i] = '\0';
    }

    m_List->InsertEnd(toAdd);
}


/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/

