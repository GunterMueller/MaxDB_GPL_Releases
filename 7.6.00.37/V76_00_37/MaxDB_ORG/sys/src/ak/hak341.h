/*******************************************************************************
*
* module:       hak341.h
*
* ------------------------------------------------------------------------------
*
* responsible:  T.Anhaus
*
* lastChanged:  1999-07-20
*
* purpose:      Kernel Sink
*
* generatedBy:  
*
* ------------------------------------------------------------------------------
*
* Copyright (c) 1999-2005 SAP AG
*


    ========== licence begin  GPL
    Copyright (c) 1999-2005 SAP AG

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


********************************************************************************/

#ifndef HAK341_H
#define HAK341_H

/* ========================================================================== */
/* ==========                   INCLUDES                           ========== */
/* ========================================================================== */

#include "ggg00.h"
#include "ggg01.h"
#include "ggg92.h"
#include "vak001.h"
#include "hkb04.h"
#include "hak34.h"
#include "hgg08.h"
#include "heo55k.h"

/* ========================================================================== */
/* ==========                EXPORT FUNCTION DECLARATION           ========== */
/* ========================================================================== */

externCpp void
ak341CancelOmsVersion (tgg91_TransNo &ConsistViewNo);

externCpp void
ak341UserAllocatorCtl (int msg, void  *param);

externCpp void
ak341CreateVersion (tak_all_command_glob    &acv,
                    tgg01_OmsVersionContext &lpVersionContext,
                    tgg00_BasisError        &e);


inline static void
ak341DeleteObj (tak_all_command_glob    *acv,
                tgg91_TransNo           *ConsistentView,
                tgg01_ContainerId       *ObjFile,
                tgg01_OmsVersionContext *VersionContext,
                tgg92_KernelOid         *Oid,
                tgg91_PageRef           *WantedObjVers,
                bool                     reused,  
                bool                     contObj, 
                short                   *e);

inline static void
ak341DeleteClusteredObjects (tak_all_command_glob    *acv,
                tgg91_TransNo           &consistentView,
                tgg01_ContainerId       &objFile,
                tgg01_OmsVersionContext *pVersionContext,
                tsp00_Int4               cnt,
                tgg92_KernelOid         *pOid,
                tgg91_PageRef           *pObjVers,
                bool                    *pReused, 
                tsp00_Int4              &errCnt,
                short                   *pError);

void
ak341DropVersion (tak_all_command_glob&    acv,
                  tgg01_OmsVersionContext& VersionContext,
                  tgg00_BasisError&        e);

externC void
ak341EndConsistentTrans (tgg00_TransContext &Trans);

inline static void
ak341GetDebugContext (tak_all_command_glob *acv,
            tgg91_SessionNo &session,
            tsp00_Addr      &sink_addr,
            tsp1_packet_ptr &packet);

externC
void ak341HeapCallStackMonitoring(int level);

externC
void ak341OmsHeapInfo(tak_all_command_glob& acv, tsp00_8ByteCounter& totalHeap, 
                                                 tsp00_8ByteCounter& freeSpaceInFreeChunks,
                                                 tsp00_8ByteCounter& reserved,
                                                 tsp00_8ByteCounter& mallocated,
                                                 tsp00_8ByteCounter& emergencySize,
                                                 tsp00_8ByteCounter& emergencyInUse,
                                                 tsp00_8ByteCounter& emergencyMaxUsed
                                                 );
                                                 
externC
tsp00_Bool ak341NextVersion (tak_all_command_glob& acv,
                       tsp00_Bool          isFirstCall,
                       tsp00_C24&          versionId,
                       tsp00_Date&         createDate,
                       tsp00_Time&         createTime,
                       tsp00_Date&         openDate,
                       tsp00_Time&         openTime,
                       tgg91_TransNo&      consistentView,
                       tsp00_Bool&         isMarked,
                       tsp00_Bool&         isOpen,
                       tsp00_Bool&         isUnloaded,
                       tsp00_8ByteCounter& heapUsage,
                       tsp00_Int4&         hashSize,
                       tgg00_FileId&       versionTree,
	                     tgg00_FileId&       versionInvTree,
                       tsp00_C512&         versionDesc);   // PTS 1117690

// PTS 1117690
externC
tsp00_Bool ak341NextDiagnoseEntry (tak_all_command_glob& acv,
                       tsp00_Int2          &phase,
                       tsp00_Int4          &session,
                       tsp00_C24           &component,
                       tsp00_C512          &entry); 

tsp00_Uint4 ak341NextVal (tak_all_command_glob&  acv,
                          const tgg00_Surrogate& sequenceId,
                          tgg00_SeqInfo&         sequenceInfo,
                          short&                 sqlError);

// PTS 1120478
inline static void
ak341GetContainerId (tak_all_command_glob           *acv,
                     tgg01_ContainerId&              ContainerId,
                     const tgg01_OmsVersionContext  *VersionContext,
                     const tgg92_KernelOid          &Oid,
                     tgg00_BasisError               &e); 

inline static void
ak341GetObjWithKey (tak_all_command_glob *acv,
             tgg91_TransNo               *ConsistentView,
             tgg01_ContainerId           *ObjFile,
             tgg01_OmsVersionContext     *VersionContext,
             boolean                      getConsistent,
             tsp00_Int4                   ObjBodySize,
             void                        *pObjBody,
             tsp00_Int4                   KeyLen,
             void                        *pKey,
             tgg91_PageRef               *ResultObjVers,
             tgg91_TransNo               *pUpdTransId,
             tgg92_KernelOid             *ResultOid,
             tsp00_Int4                  *ObjHistReadCount, /* +++ Change Request 1000139 +++ */
             tsp00_Int2                  *e);

externC void
ak341LoadLibOms(tak_all_command_glob& acv);

inline static void
ak341LockObj (tak_all_command_glob *acv,
             tgg91_TransNo         *ConsistentView,
             tgg01_ContainerId     *ObjFile,       
             tgg92_KernelOid       *Oid,
             tgg91_PageRef         *WantedObjVers,
             short                 *e);

inline static void
ak341LockUpdObj (tak_all_command_glob *acv,
             tgg91_TransNo         *ConsistentView,
             tgg01_ContainerId     *ObjFile,       
             tgg92_KernelOid       *Oid,
             tgg91_PageRef         *WantedObjVers,
             short                 *e);

inline static void
ak341LockUpdClusteredObjects (tak_all_command_glob *acv,
              tgg91_TransNo        &consistentView,
              tgg01_ContainerId    &objFile,  
              tsp00_Int4            cnt,
              tgg92_KernelOid      *pOid,
              tgg91_PageRef        *pObjVers,
              tsp00_Int4           &errCnt,
              short                *pError);           

externCpp void 
ak341MultiPurpose (tak_all_command_glob* acv,
            tgg00_MessType  MessType,
            tgg00_MessType2 MessType2,
            tsp00_Int4*     BufLen,
            void*           Buf,
            tsp00_Int2*     e);

//externCpp void
//ak341NextObj (tak_all_command_glob  *acv,
//            tgg91_TransNo           *ConsistentView,
//            tgg01_ContainerId       *ObjFile,
//            tgg01_OmsVersionContext *VersionContext,
//            tgg92_KernelOid         *CurrOid,
//            tsp00_Int4              ObjBodySize,
//            void*                   pObjBody,
//            tgg91_PageRef           *ResultObjVers,
//            tsp00_Int4&             ObjHistReadCount, // PTS 1107819 TA 13/09/2000
//            short                   *e);

// PTS 1120478
externCpp void
ak341NextOids (tak_all_command_glob    &acv,
               tgg91_TransNo           &ConsistentView,
               tgg01_ContainerId       &ObjFile,
               tgg01_OmsVersionContext *VersionContext,
               tsp00_Bool               bOnlyHdrVarObjects,
               tgg92_KernelOid         &CurrOid,
               tsp00_Int4              &noOfOid,
               tgg92_KernelOid         *pOid,
               tgg91_PageRef           *pObjVers,
               tsp00_Int4              &LogReadAccesses,
               tgg00_BasisError        &e);

inline static void
ak341NewObj (tak_all_command_glob    *acv,
             tgg91_TransNo           *ConsistentView,
             tgg01_ContainerId       *ObjFile,
             tgg01_OmsVersionContext *VersionContext,
             tsp00_Int4              ObjKeyPos,
             tsp00_Int4              ObjKeySize,
             bool                     contObj, // PTS 1124278
             void                    *pObjKey,
             tgg92_KernelOid         *ResultOid,
             tgg91_PageRef           *ResultObjVers,
             tgg91_TransNo           *pUpdTransId,
             short                   *e);

externC void
ak341NilConsistentTrans (tgg00_TransContext&Trans);

externCpp void
ak341OpenVersion (tak_all_command_glob& acv,
                  tgg91_TransNo&        ConsistentView,
                  tgg00_BasisError&     e);

/* PTS 1115134 */
void
ak341ResetVersion (tak_all_command_glob&    acv,
                  tgg01_OmsVersionContext&  VersionContext,
                  tgg00_BasisError&         e);

externCpp
bool ak341SetOmsTraceLevel(const char* lvl, pasbool enable); 

externC void
ak341StartConsistentTrans (tgg00_TransContext &Trans,
						   bool                isOmsVersion);  /* PTS 1130354 */

inline static void
ak341UnlockObj (tak_all_command_glob *acv,
             tgg01_ContainerId       *ObjFile,       
             tgg92_KernelOid         *Oid,
             short                   *e);

inline static void
ak341UpdateObj (tak_all_command_glob   *acv,
               tgg91_TransNo           *ConsistentView,
               tgg01_ContainerId       *ObjFile,
               tgg01_OmsVersionContext *VersionContext,
               tgg92_KernelOid         *Oid,
               tgg91_PageRef           *ObjVers,
               tsp00_Int4               NewBodySize,
               void                    *pNewBody,
               tsp00_Int4               KeyPos,   // PTS 1122540
               tsp00_Int4               KeyLen,   // PTS 1122540
               bool                     contObj,  // PTS 1124278
               bool                     reused,
               short                   *e);

inline static void
ak341UpdateClusteredObjects (tak_all_command_glob   *acv,
               tgg91_TransNo           &consistentView,
               tgg01_ContainerId       &objFile,
               tgg01_OmsVersionContext *pVersionContext,
               tsp00_Int4               cnt,
               tgg92_KernelOid         *pOid,
               tgg91_PageRef           *pObjVers,
               tsp00_Int4              *pNewBodySize,
               unsigned char          **ppNewBody,
               size_t                  *pKeyPos,   
               size_t                  *pKeyLen,   
               bool                    *pReused,
               tsp00_Int4              &errCnt,
               short                   *pDbError);

externCpp
bool ak341UpdatedInVersion (tak_all_command_glob& acv,
                        tgg00_FileId&          VersionFile,
                        tgg01_ContainerId&     ObjFile, /* PTS XYZ */
                        tgg92_KernelOid&       Oid,
                        tsp00_Int4             ObjBodySize,
                        tsp00_Int4&            ObjBodyLen,
                        tsp00_MoveObj*         Body,
                        tgg00_BasisError&      e,
                        tsp00_Bool             bWithObject = true);  // PTS 1119480

externCpp
void ak341VersionDelObj(tak_all_command_glob&    acv,
                        tgg00_FileId&            VersionFile,
                        const tgg01_ContainerId& ObjFile, // PTS XYZ
                        tgg92_KernelOid&         Oid,
                        bool                     DoInsert,
                        tgg00_BasisError&        e);

externCpp
void ak341VersionGetObjViaKey (tak_all_command_glob& acv,
                              tgg01_OmsVersionContext* VersionContext,
                              tsp00_Int4               KeyLen,
                              tsp00_MoveObj*           pKey,
                              tsp00_C8                 ClsId,
                              tsp00_Int4               ObjBodySize,
                              tsp00_MoveObj*           Body,
                              tgg92_KernelOid&         Oid,
                              tgg00_BasisError&        e);

// Interface changed: PTS 1119480
externC
void ak341VersionNextKey (tak_all_command_glob&    acv,
                          tgg01_OmsVersionContext* VersionContext,
                          const tgg01_ContainerId& ObjFile,
                          void*                    pKey,
                          tsp00_Int4               KeyLen,
                          bool                     Inclusive,
                          void*                    pUpperKey,
                          tgg92_KernelOid&         Oid,
                          tgg00_BasisError&        e);

// New function: PTS 1119480
externC
void ak341VersionPrevKey (tak_all_command_glob&    acv,
                          tgg01_OmsVersionContext* VersionContext,
                          const tgg01_ContainerId& ObjFile,
                          void*                    pKey,
                          tsp00_Int4               KeyLen,
                          bool                     Inclusive,
                          void*                    pLowerKey,
                          tgg92_KernelOid&         Oid,
                          tgg00_BasisError&        e);

externC
void ak341VersionNextObj (tak_all_command_glob& acv,
                          tgg01_OmsVersionContext* VersionContext,
                          tgg92_KernelOid&         Oid,
                          tsp00_C8&                wantedContainerId,
                          tsp00_Int4               ObjBodySize,
                          tsp00_MoveObj*           Body,
                          tgg91_PageRef&           ResultObjVers,
                          tgg00_BasisError&        e);

// new function PTS 1120478
externC
void ak341VersionNextOids (tak_all_command_glob    &acv,
                           tgg01_OmsVersionContext *VersionContext,
                           tsp00_C8&                wantedContainerId,
                           tgg92_KernelOid         &CurrOid,
                           tsp00_Int4              &noOfOid,
                           tgg92_KernelOid         *pOid,
                           tgg00_BasisError        &e);

externCpp
void ak341VersionUpdObj(tak_all_command_glob&    acv,
                        tgg01_OmsVersionContext* VersionContext,
                        const tgg01_ContainerId& ObjFile,
                        tgg92_KernelOid&         Oid,
                        bool                     DoInsert,
                        tsp00_Int4               NewBodySize,
                        tsp00_MoveObj*           NewBody,
                        tsp00_Int4               KeyPos,    // PTS 1122540
                        tsp00_Int4               KeyLen,    // PTS 1122540
                        bool                     contObj,   // PTS 1124278
                        tgg00_BasisError&        e);

externC
pasbool ak341IsMonitorOn();

/* ========================================================================== */
/* ==========                LOCAL FUNCTION IMPLEMENTATION         ========== */
/* ========================================================================== */

inline void ak341SetConsistentView (tak_all_command_glob    * acv,
                                    tgg91_TransNo           * ConsistentView)
{
  acv->a_transinf.tri_trans.trError_gg00       = e_ok;
  acv->a_transinf.tri_trans.trConsistView_gg00 = *ConsistentView;
}

/* ========================================================================== */
/* ==========                EXPORT FUNCTION IMPLEMENTATION        ========== */
/* ========================================================================== */

inline static void
ak341CloseVersion (tak_all_command_glob *acv,
                   short                *e)
{
  ak341NilConsistentTrans (acv->a_transinf.tri_trans);
  *e = e_ok;
}

/*----------------------------------------------------------------------------*/
// Interface changed: PTS 1119480   
inline static void
ak341CreateKeyRangeIterator
(
 tak_all_command_glob&   acv,
 tgg91_TransNo&          consistentView,
 tgg01_OmsVersionContext *lpVersionContext,
 tgg00_FileId&           objFile,
 tsp00_Int4              keyLen,
 const void*             pStartKey,  
 const void*             pLowerKey,           
 const void*             pUpperKey,           
 void**                  pKeyRangeIterator,
 bool                    bAscendingKeyOrder,  
 short&                  e)
{
  ak341SetConsistentView (&acv, &consistentView);
  if (acv.a_transinf.tri_trans.trConsistView_gg00.gg90IsNil()) {
    kb04StartConsistentTrans (acv.a_transinf.tri_trans, false /* isOmsVersion */ );
    e = acv.a_transinf.tri_trans.trError_gg00;
    if (e_ok != e) {
      return;
    }
    consistentView = acv.a_transinf.tri_trans.trConsistView_gg00; 
  }
  kb04CreateKeyIterator (acv.a_transinf.tri_trans, 
                         objFile, 
                         keyLen, 
                         CONST_CAST(void*, pStartKey),
                         CONST_CAST(void*, pLowerKey),    
                         CONST_CAST(void*, pUpperKey),   
                         *pKeyRangeIterator, 
                         bAscendingKeyOrder);             
  e = acv.a_transinf.tri_trans.trError_gg00;
}

/*----------------------------------------------------------------------------*/

//inline static void
//ak341CreateOpenVersion (tak_all_command_glob *acv,
//                     tgg91_TransNo           *ConsistentView,
//                     tak_oms_version_id      *lpVersionId,
//                     boolean                  createVersion,
//                     tgg01_OmsVersionContext *lpVersionContext,
//                     short                   *e)
//{
//  if (createVersion) {
//    ak341CreateVersion (*acv, *lpVersionId, *lpVersionContext, *e);
//  }
//  else {
//    ak341OpenVersion (*acv, *ConsistentView, *e);
//  }
//}

/*----------------------------------------------------------------------------*/

inline static void
ak341DeleteAllObj (tak_all_command_glob *acv,
                tgg91_TransNo&          ConsistentView,
                tgg01_ContainerId       *ObjFile,
                tgg01_OmsVersionContext *VersionContext,
                tsp00_Int4              &cntDeleted,
                short                   &e,
                tgg92_KernelOid         &errorOid)
{
  ak341SetConsistentView (acv, &ConsistentView);
  if (VersionContext) {
    e = e_not_implemented;
    return;
  }
  else {
      /* PTS 1116566 FF 2002-07-09                                 */
      if (acv->a_transinf.tri_trans.trConsistView_gg00.gg90IsNil()) {
	      kb04StartConsistentTrans (acv->a_transinf.tri_trans, false /* isOmsVersion */ );
          e = acv->a_transinf.tri_trans.trError_gg00;
          if (e_ok != e) {
              return;
          }
          ConsistentView = acv->a_transinf.tri_trans.trConsistView_gg00; 
      }
      kb04DeleteAllObj (acv->a_transinf.tri_trans, *ObjFile, cntDeleted, errorOid);
  }
  e = acv->a_transinf.tri_trans.trError_gg00;
}

/*----------------------------------------------------------------------------*/

inline static void
ak341DeleteObj (tak_all_command_glob    * acv,
                tgg91_TransNo           * ConsistentView,
                tgg01_ContainerId       * ObjFile,
                tgg01_OmsVersionContext * VersionContext,
                tgg92_KernelOid         * Oid,
                tgg91_PageRef           * WantedObjVers,
                bool                      reused,
                bool                      contObj, // PTS 1124278
                short                   * e)
{
  ak341SetConsistentView (acv, ConsistentView);
  if (VersionContext) {
    ak341VersionDelObj   (*acv, VersionContext->ovc_tree, 
      *ObjFile, // PTS XYZ
      *Oid, !WantedObjVers->gg91IsDummyRef (), *e);
  }
  else {
    kb04DeleteObj (acv->a_transinf.tri_trans, *ObjFile, *Oid,
      *REINTERPRET_CAST(tgg91_PageRef*, WantedObjVers), reused, contObj);  
  }
  *e = acv->a_transinf.tri_trans.trError_gg00;
}

/*----------------------------------------------------------------------------*/

inline static void
ak341DeleteClusteredObjects (tak_all_command_glob    *acv,
                tgg91_TransNo           &consistentView,
                tgg01_ContainerId       &objFile,
                tgg01_OmsVersionContext *pVersionContext,
                tsp00_Int4               cnt,
                tgg92_KernelOid         *pOid,
                tgg91_PageRef           *pObjVers,
                bool                    *pReused,  
                tsp00_Int4              &errCnt,
                short                   *pError)
{
  ak341SetConsistentView (acv, &consistentView);
  if (pVersionContext) {
    for (int i=0; i<cnt; ++i){
      short e = 0;
      ak341VersionDelObj(*acv, pVersionContext->ovc_tree, 
        objFile, 
        pOid[i], 
        !pObjVers[i].gg91IsDummyRef(), 
        e);
      pError[i] = e;
      if (e){
        ++errCnt;
      }
    }
  }
  else {
    // TODO!! replace by mass-operation
    for (int i=0; i<cnt; ++i){
      kb04DeleteObj (acv->a_transinf.tri_trans, objFile, 
        pOid[i], *reinterpret_cast<tgg91_PageRef*>(&pObjVers[i]), pReused[i], /*cont=*/false);  
      pError[i] = acv->a_transinf.tri_trans.trError_gg00;
      if (pError[i]){
        ++errCnt;
      }
    }
  }
}

/*----------------------------------------------------------------------------*/

inline static void
ak341DestroyKeyRangeIterator(tak_all_command_glob& acv,
                             void* pKeyRangeIterator)
{
  acv.a_transinf.tri_trans.trError_gg00 = e_ok;
  kb04DestroyKeyIterator(acv.a_transinf.tri_trans, pKeyRangeIterator);
}

/*----------------------------------------------------------------------------*/

inline static void
ak341EndConsistentView (tak_all_command_glob *acv,
                        tgg91_TransNo        *ConsistentView,
                        tsp00_Int2           *e)
{
  ak341SetConsistentView (acv, ConsistentView);
  kb04EndConsistentTrans (acv->a_transinf.tri_trans);
  *ConsistentView = acv->a_transinf.tri_trans.trConsistView_gg00; 
  *e = acv->a_transinf.tri_trans.trError_gg00;
}

/*----------------------------------------------------------------------------*/

inline static void ak341EnterCriticalSection(tak_all_command_glob *acv,
                                             short regionId)
{
  vbegexcl (acv->a_transinf.tri_trans.trTaskId_gg00, g08oms1 + regionId);
}

/*----------------------------------------------------------------------------*/

inline static void
ak341GetDebugContext (tak_all_command_glob *acv,
                      tgg91_SessionNo &session,
                      tsp00_Addr      &sink_addr,
                      tsp1_packet_ptr &packet)
{
  session   = acv->a_transinf.tri_trans.trSessionId_gg00;
  sink_addr = REINTERPRET_CAST(tsp00_Addr, &acv->a_kernel_sink);
  packet    = acv->a_dbp_packet_ptr;
}

/*----------------------------------------------------------------------------*/

inline static void
ak341GetContainerId (tak_all_command_glob           *acv,
                     tgg01_ContainerId              &ContainerId,
                     const tgg01_OmsVersionContext  *VersionContext,
                     const tgg92_KernelOid          &Oid,
                     tgg00_BasisError               &e) 
{
  if (NULL != VersionContext) {
    tsp00_Int4   ObjBodyLen = 0;
    void*        pObjBody   = NULL;
    if (!ak341UpdatedInVersion (*acv, CONST_CAST(tgg00_FileId&, VersionContext->ovc_tree), 
      ContainerId, CONST_CAST(tgg92_KernelOid&, Oid), 0, ObjBodyLen, 
      REINTERPRET_CAST(tsp00_MoveObj*, pObjBody),
      acv->a_transinf.tri_trans.trError_gg00, false)) 
    {
      if (VERSION_OBJ_PAGE_NO_GG92 == Oid.gg92GetPno()){
        e = e_object_not_found;
        return;
      }
      else{
        kb04GetContainerId (acv->a_transinf.tri_trans, ContainerId, Oid);
      }
    }
    else {
      // ContainerId is filled by call of ak341UpdatedInVersion
    }
  }
  else {
    kb04GetContainerId (acv->a_transinf.tri_trans, ContainerId, Oid);
  }
  e = acv->a_transinf.tri_trans.trError_gg00;
}

/*----------------------------------------------------------------------------*/

inline static void
ak341GetObj (tak_all_command_glob           *acv,
             tgg91_TransNo&                 ConsistentView,
             tgg01_ContainerId&             ObjFile,
             const tgg01_OmsVersionContext* VersionContext,
             const tgg92_KernelOid&         Oid,
             bool                           doLock,
             bool                           shared,
             tsp00_Int4                     ObjBodySize,
             void                           *pObjBody,
             tsp00_Int4&                    ObjBodyLen, /* +++ Change Request 1000203, T.A. 01.12.1998  +++ */
             tgg91_PageRef&                 ResultObjVers,
             tgg91_TransNo&                 updTransId,
             tsp00_Int4&                    ObjHistReadCount, /* +++ Change Request 1000139 +++ */
             tsp00_Int2&                    e)
                       
{
  ak341SetConsistentView (acv, &ConsistentView);
  ObjHistReadCount = 0;
  if (acv->a_transinf.tri_trans.trConsistView_gg00.gg90IsNil()) {
    kb04StartConsistentTrans (acv->a_transinf.tri_trans, false /* isOmsVersion */ );
    e = acv->a_transinf.tri_trans.trError_gg00;
    if (e_ok != e) {
      return;
    }
    ConsistentView = acv->a_transinf.tri_trans.trConsistView_gg00; 
  }

  if (VERSION_OBJ_PAGE_NO_GG92 != Oid.gg92GetPno()){
    // Try to read object from the kernel. This makes only sense, if object has not been created in a version
    kb04GetObjConsistent (acv->a_transinf.tri_trans, 
      ObjFile, Oid, ObjBodySize, pObjBody, doLock, shared, ObjBodyLen, ObjHistReadCount,  
      ResultObjVers, updTransId);
    e = acv->a_transinf.tri_trans.trError_gg00;
  }
  else if (VersionContext == NULL){
    // Oid corresponds to an object which has been created in a version, 
    // but the version is not unloaded
    e = e_object_not_found;
    return;
  }

  if (VersionContext != NULL){
    if (e == 0 || e == e_object_not_found){
      // check whether object has been updated or newly created in the unloaded version
      if (ak341UpdatedInVersion (*acv, CONST_CAST(tgg00_FileId&, VersionContext->ovc_tree), 
        ObjFile, CONST_CAST(tgg92_KernelOid&, Oid), ObjBodySize, ObjBodyLen, 
        REINTERPRET_CAST(tsp00_MoveObj*, pObjBody), e))
      {
        ResultObjVers.gg91SetDummyRef ();

        if (VERSION_OBJ_PAGE_NO_GG92 == Oid.gg92GetPno()) {
          // Object was newly created in the version
          updTransId.gg90SetZero();
        }
        //else {
        //  // Object exists in the kernel and was updated in the version
        //  // updTransId was already read when dereferencing the object from the kernel
        //}
      }
    }
  }
}
    
/*----------------------------------------------------------------------------*/

inline static void
ak341GetObjWithKey (tak_all_command_glob *acv,
             tgg91_TransNo               *ConsistentView,
             tgg01_ContainerId           *ObjFile,
             tgg01_OmsVersionContext     *VersionContext,
             bool                        doLock,
             bool                        shared,
             tsp00_Int4                  ObjBodySize,
             void                        *pObjBody,
             tsp00_Int4                  KeyLen,
             void                        *pKey,
             tgg91_PageRef               *ResultObjVers,
             tgg91_TransNo               *pUpdTransId,
             tgg92_KernelOid             *ResultOid,
             tsp00_Int4                  *ObjHistReadCount, /* +++ Change Request 1000139 +++ */
             tsp00_Int2                  *e)
                       
{
  *ObjHistReadCount = 0;
  ak341SetConsistentView (acv, ConsistentView);
  if (acv->a_transinf.tri_trans.trConsistView_gg00.gg90IsNil()) {
    kb04StartConsistentTrans (acv->a_transinf.tri_trans, false /* isOmsVersion */ );
    *e = acv->a_transinf.tri_trans.trError_gg00;
    if (e_ok != *e) {
      return;
    }
    *ConsistentView = acv->a_transinf.tri_trans.trConsistView_gg00; 
  }

  // Try to read object from kernel
  kb04GetObjWithKeyConsistent (acv->a_transinf.tri_trans, *ObjFile, ObjBodySize, pObjBody,
    KeyLen, pKey, doLock, shared, *ObjHistReadCount,   
    *ResultOid, *ResultObjVers, *pUpdTransId);
  *e = acv->a_transinf.tri_trans.trError_gg00;

  if (VersionContext) {
    if (*e == 0){
      // check whether object has been updated in the unloaded version
      tsp00_Int4 ObjBodyLen;
      if (ak341UpdatedInVersion (*acv, VersionContext->ovc_tree, *ObjFile, 
                                  *ResultOid, ObjBodySize, ObjBodyLen, 
                                  REINTERPRET_CAST(tsp00_MoveObj*, pObjBody), *e)) 
      {
        // There exists an updated revision of the object in the unloaded version
        ResultObjVers->gg91SetDummyRef ();
        // pUpdTransId was set already when reading the object from the kernel
      }
    }
    else if (*e != e_object_not_found){
      // Check whether the object was created in the unloaded version
      ak341VersionGetObjViaKey (*acv, VersionContext, KeyLen, 
        REINTERPRET_CAST(tsp00_MoveObj*, pKey), ObjFile->fileDirFileId_gg00(), ObjBodySize,  
        REINTERPRET_CAST(tsp00_MoveObj*, pObjBody), *ResultOid, *e);
      if (0 == *e) {
        ResultObjVers->gg91SetDummyRef ();
        pUpdTransId->gg90SetZero();
      }
    }
  }
}

/*----------------------------------------------------------------------------*/

inline static void
ak341IsObjHistoryUsed(tak_all_command_glob& acv,
              tgg01_ContainerId&     ObjFile,     
              const tgg92_KernelOid& Oid,
              bool                   ignoreVersions,        /* PTS 1130354 */
              bool&                  isUsed,
              short&                 e,
              SAPDB_UInt8&           OldestTransNo)     /* PTS 1130354 */
{
  acv.a_transinf.tri_trans.trError_gg00 = e_ok;
  kb04IsObjHistoryUsed (acv.a_transinf.tri_trans, ObjFile, Oid, ignoreVersions, isUsed, OldestTransNo);
  e = acv.a_transinf.tri_trans.trError_gg00;
}

/*----------------------------------------------------------------------------*/

inline static void
ak341IsLocked (tak_all_command_glob *acv,
              tgg01_ContainerId     *ObjFile,     
              tgg92_KernelOid       *Oid,
              bool                   shared,
              short                 *e)
{
  bool IsSelfLocked;
  acv->a_transinf.tri_trans.trError_gg00 = e_ok;
  kb04IsObjSelfLocked (acv->a_transinf.tri_trans, *ObjFile, *Oid, IsSelfLocked, shared); 
  *e = acv->a_transinf.tri_trans.trError_gg00;
  if (e_ok == *e) {
    if (!IsSelfLocked) {
      *e = e_object_not_locked;
    }
  }
}

/*----------------------------------------------------------------------------*/

inline static void ak341LeaveCriticalSection(tak_all_command_glob *acv,
                                             short regionId)
{
  vendexcl (acv->a_transinf.tri_trans.trTaskId_gg00, g08oms1 + regionId);
}

/*----------------------------------------------------------------------------*/

inline static void
ak341LockObj (tak_all_command_glob *acv,
              tgg91_TransNo        *ConsistentView,
              tgg01_ContainerId    *ObjFile,     
              tgg92_KernelOid      *Oid,
              tgg91_PageRef        *WantedObjVers,
              short                *e)
              
{
  ak341SetConsistentView (acv, ConsistentView);
  kb04LockObj (acv->a_transinf.tri_trans, *ObjFile, *Oid, *WantedObjVers);
  *e = acv->a_transinf.tri_trans.trError_gg00;
}

/*----------------------------------------------------------------------------*/

inline static void
ak341LockUpdObj (tak_all_command_glob *acv,
              tgg91_TransNo        *ConsistentView,
              tgg01_ContainerId    *ObjFile,     
              tgg92_KernelOid      *Oid,
              tgg91_PageRef        *WantedObjVers,
              short                *e)            
{
  ak341SetConsistentView (acv, ConsistentView);
  kb04LockUpdObj (acv->a_transinf.tri_trans, *ObjFile, *Oid, *WantedObjVers);
  *e = acv->a_transinf.tri_trans.trError_gg00;
}

/*----------------------------------------------------------------------------*/

inline static void
ak341LockUpdClusteredObjects (tak_all_command_glob *acv,
              tgg91_TransNo        &consistentView,
              tgg01_ContainerId    &objFile,  
              tsp00_Int4            cnt,
              tgg92_KernelOid      *pOid,
              tgg91_PageRef        *pObjVers,
              tsp00_Int4           &errCnt,
              short                *pError)           
{  
  ak341SetConsistentView (acv, &consistentView);

  // TODO!! Replace by mass-operations
  for (int i=0; i<cnt; ++i){
    kb04LockUpdObj (acv->a_transinf.tri_trans, 
      objFile, 
      pOid[i],
      pObjVers[i]);
    pError[i] = acv->a_transinf.tri_trans.trError_gg00;
    if (pError[i]){
      ++errCnt;
    }
  }
}

/*----------------------------------------------------------------------------*/
// PTS 1130435
inline static void
ak341LockSharedObj (tak_all_command_glob *acv,
              tgg91_TransNo        *ConsistentView,
              tgg01_ContainerId    *ObjFile,     
              tgg92_KernelOid      *Oid,
              tgg91_PageRef        *WantedObjVers,
              short                *e)
              
{
  ak341SetConsistentView (acv, ConsistentView);
  kb04LockShareObj (acv->a_transinf.tri_trans, *ObjFile, *Oid, *WantedObjVers);
  *e = acv->a_transinf.tri_trans.trError_gg00;
}

/*----------------------------------------------------------------------------*/

inline static void
ak341NewConsistentView (tak_all_command_glob *acv,
             bool                 isOmsVersion,
             tgg91_TransNo        *ConsistentView,
             tsp00_Int2           *e)
{
  ak341SetConsistentView   (acv, ConsistentView);
  kb04StartConsistentTrans (acv->a_transinf.tri_trans, isOmsVersion );
  *e = acv->a_transinf.tri_trans.trError_gg00;
  if (e_ok == *e) {
    *ConsistentView = acv->a_transinf.tri_trans.trConsistView_gg00;
  }
}

/*----------------------------------------------------------------------------*/

inline static void
ak341NewObj (tak_all_command_glob    *acv,
             tgg91_TransNo           *ConsistentView,
             tgg01_ContainerId       *ObjFile,
             tgg01_OmsVersionContext *VersionContext,
             tsp00_Int4               ObjKeyPos,
             tsp00_Int4               ObjKeySize,
             void                    *pObjKey,
             bool                     contObj, // PTS 1124278
             tgg92_KernelOid         *ResultOid,
             tgg91_PageRef           *ResultObjVers,
             tgg91_TransNo           *pUpdTransId,
             short                   *e)
{
  ak341SetConsistentView (acv, ConsistentView);
  kb04NewObj (acv->a_transinf.tri_trans, *ObjFile, ObjKeySize, pObjKey, 
    contObj, *ResultOid, *ResultObjVers, *pUpdTransId);
  *e = acv->a_transinf.tri_trans.trError_gg00;
}

/*----------------------------------------------------------------------------*/

// Interface changed: PTS 1119480 
inline static void
ak341NextObjFromKeyIterator (tak_all_command_glob& acv,
                             const tgg91_TransNo     &ConsistentView,  // PTS 1127520 
                             tgg01_ContainerId       &ObjFile,
                             tgg01_OmsVersionContext *VersionContext,
                             void                    *pKeyIterator,
                             bool                     bInclusive, 
                             tsp00_Int4               objBodySize,
                             tsp00_Int4               KeyLen,       
                             void                    *pRestartKey, 
                             tsp00_Int4              &noOfOid,
                             tgg92_KernelOid         *pResultOId, 
                             tgg91_PageRef           *pObjVers,
                             tsp00_MoveObj           *pBody,
                             tsp00_Int4              &LogReadAccesses,
                             tgg00_BasisError        &e)
{
    ak341SetConsistentView (&acv, const_cast<tgg91_TransNo*>(&ConsistentView));  // PTS 1127520
    acv.a_transinf.tri_trans.trError_gg00 = e_ok; 
    tsp00_Int4 maxNoOfOid = noOfOid;
    do {
        LogReadAccesses = 0;
        noOfOid         = maxNoOfOid;
        e               = e_ok;
        kb04NextObjFromKeyIterator(acv.a_transinf.tri_trans, 
            pKeyIterator,
            bInclusive,
            objBodySize, 
            KeyLen,
            pRestartKey,
            noOfOid,
            pResultOId, 
            pObjVers,
            pBody,
            LogReadAccesses);
        e = acv.a_transinf.tri_trans.trError_gg00;
        if (e_ok == e || e_no_next_object == e){
          if (VersionContext) {
            int i,j;
            for (i=0,j=0; i<noOfOid; ++i){
              // unloaded version, look if already updated/deleted 
              tsp00_Int4 ObjBodyLen;
              if (ak341UpdatedInVersion (acv, VersionContext->ovc_tree, ObjFile, pResultOId[i], 0, ObjBodyLen, NULL, e, false)) {
                if (e==e_object_not_found){
                  // Object is marked as deleted in the version, so reset error and ignore it.
                  e = e_ok;
                }
                // Ignore entry, as entry is delivered by unloaded-version-iterator
                //pResultOId[i].gg92SetNil();
              }
              else {
                if (i != j){
                  // Compress OId-array if possible
                  pResultOId[j] = pResultOId[i];
                  pObjVers[j] = pObjVers[i];
                }
                ++j;
              }
            }
            noOfOid = j;
            if (noOfOid == 0){
              // Read next portion of OIds from kernel. The last key is remembered
              // in the iterator and if the pRestartKey equals NULL, then this locally
              // remembered key is used instead.
              pRestartKey = NULL;
            }
          }
        }
    }
    while (noOfOid == 0 && e_ok == e);

    if (e_ok != e && e_no_next_object != e){
      noOfOid = 0;
    }
}

/*----------------------------------------------------------------------------*/
// New function: PTS 1119480 
inline static void
ak341PrevObjFromKeyIterator (tak_all_command_glob& acv,
                             const tgg91_TransNo     &ConsistentView,  // PTS 1127520 
                             tgg01_ContainerId       &ObjFile,
                             tgg01_OmsVersionContext *VersionContext,
                             void                    *pKeyIterator,
                             bool                     bInclusive,  
                             tsp00_Int4               objBodySize,
                             tsp00_Int4               KeyLen,       
                             void                    *pRestartKey, 
                             tsp00_Int4              &noOfOid,
                             tgg92_KernelOid         *pResultOId, 
                             tgg91_PageRef           *pObjVers,
                             tsp00_MoveObj           *pBody,
                             tsp00_Int4              &LogReadAccesses,
                             tgg00_BasisError        &e)
{
    ak341SetConsistentView (&acv, const_cast<tgg91_TransNo*>(&ConsistentView));  // PTS 1127520
    acv.a_transinf.tri_trans.trError_gg00 = e_ok; 
    tsp00_Int4 maxNoOfOid = noOfOid;
    do {
        LogReadAccesses = 0;
        noOfOid         = maxNoOfOid;
        e               = e_ok;
        kb04PrevObjFromKeyIterator(acv.a_transinf.tri_trans, 
            pKeyIterator,
            bInclusive,
            objBodySize,
            KeyLen,
            pRestartKey,
            noOfOid,
            pResultOId, 
            pObjVers,
            pBody,
            LogReadAccesses);
        e = acv.a_transinf.tri_trans.trError_gg00;
        if (e_ok == e || e_no_next_object == e){
          if (VersionContext) {
            int i,j;
            for (i=0,j=0; i<noOfOid; ++i){
              // unloaded version, look if already updated/deleted 
              tsp00_Int4 ObjBodyLen;
              if (ak341UpdatedInVersion (acv, VersionContext->ovc_tree, ObjFile, pResultOId[i], 0, ObjBodyLen, NULL, e, false)) {
                if (e==e_object_not_found){
                  // Object is marked as deleted in the version, so reset error and ignore it.
                  e = e_ok;
                }
                // Ignore entry, as entry is delivered by unloaded-version-iterator
                //pResultOId[i].gg92SetNil();
              }
              else {
                if (i != j){
                  // Compress OId-array if possible
                  pResultOId[j] = pResultOId[i];
                  pObjVers[j] = pObjVers[i];
                }
                ++j;
              }
            }
            noOfOid = j;
            if (noOfOid == 0){
              // Read next portion of OIds from kernel. The last key is remembered
              // in the iterator and if the pRestartKey equals NULL, then this locally
              // remembered key is used instead.
              pRestartKey = NULL;
            }
          }
        }
    }
    while (noOfOid == 0 && e_ok == e);

    if (e_ok != e && e_no_next_object != e){
      noOfOid = 0;
    }
}

/*----------------------------------------------------------------------------*/

inline static void
ak341UnlockObj (tak_all_command_glob *acv,
             tgg01_ContainerId       *ObjFile,       
             tgg92_KernelOid         *Oid,
             short                   *e)
{
  acv->a_transinf.tri_trans.trError_gg00 = e_ok;
  kb04UnlockObj (acv->a_transinf.tri_trans, *ObjFile, *Oid);
  *e = acv->a_transinf.tri_trans.trError_gg00;
}

/*----------------------------------------------------------------------------*/

inline static void
ak341UnlockSharedObj (tak_all_command_glob    *acv,
                      tgg01_ContainerId       *ObjFile,       
                      tgg92_KernelOid         *Oid,
                      short                   *e)
{
  acv->a_transinf.tri_trans.trError_gg00 = e_ok;
  kb04UnlockSharedObj (acv->a_transinf.tri_trans, *ObjFile, *Oid);
  *e = acv->a_transinf.tri_trans.trError_gg00;
}

/*----------------------------------------------------------------------------*/

inline static void
ak341UpdateObj (tak_all_command_glob    *acv,
                tgg91_TransNo           *ConsistentView,
                tgg01_ContainerId       *ObjFile,
                tgg01_OmsVersionContext *VersionContext,
                tgg92_KernelOid         *Oid,
                tgg91_PageRef           *ObjVers,
                tsp00_Int4               NewBodySize,
                void                    *pNewBody,
                tsp00_Int4               KeyPos,   // PTS 1122540
                tsp00_Int4               KeyLen,   // PTS 1122540
                bool                     contObj,  // PTS 1124278
                bool                     reused,   // PTS 1127661
                short                   *e)
{
  ak341SetConsistentView (acv, ConsistentView);
  if (VersionContext) {
    ak341VersionUpdObj   (*acv, VersionContext, 
      *ObjFile, 
      *Oid, 
      !ObjVers->gg91IsDummyRef (), NewBodySize, 
      REINTERPRET_CAST(tsp00_MoveObj*, pNewBody),
      KeyPos, KeyLen, contObj, *e);
    ObjVers->gg91SetDummyRef ();
  }
  else {
    kb04UpdateObj (acv->a_transinf.tri_trans, *ObjFile, *Oid,
      *ObjVers, NewBodySize, pNewBody, contObj, reused); 
  }
  *e = acv->a_transinf.tri_trans.trError_gg00;
} 

/*----------------------------------------------------------------------------*/

inline static void
ak341UpdateClusteredObjects (tak_all_command_glob   *acv,
               tgg91_TransNo           &consistentView,
               tgg01_ContainerId       &objFile,
               tgg01_OmsVersionContext *pVersionContext,
               tsp00_Int4               cnt,
               tgg92_KernelOid         *pOid,
               tgg91_PageRef           *pObjVers,
               size_t                  *pNewBodySize,
               unsigned char          **ppNewBody,
               size_t                  *pKeyPos,   
               size_t                  *pKeyLen,   
               bool                    *pReused,
               int                     &errCnt,
               short                   *pError)
{
  ak341SetConsistentView (acv, &consistentView);

  if (pVersionContext) {
    for (int i=0; i<cnt; ++i){
      short e = 0;
      ak341VersionUpdObj   (*acv, pVersionContext, 
        objFile, 
        pOid[i], 
        !pObjVers[i].gg91IsDummyRef(), 
        pNewBodySize[i], 
        reinterpret_cast<tsp00_MoveObj*>(ppNewBody[i]),
        pKeyPos ? pKeyPos[i] : 0,
        pKeyLen ? pKeyLen[i] : 0,
        pReused[i], 
        e);
      pObjVers[i].gg91SetDummyRef ();
      pError[i] = e;
      if (e){
        ++errCnt;
      }
    }
  }
  else {
    // TODO!! Replace by mass-operations
    for (int i=0; i<cnt; ++i){
      kb04UpdateObj (acv->a_transinf.tri_trans, 
        objFile, 
        pOid[i],
        pObjVers[i], 
        pNewBodySize[i], 
        ppNewBody[i], 
        false /*cont*/,
        pReused[i]); 
      pError[i] = acv->a_transinf.tri_trans.trError_gg00;
      if (pError[i]){
        ++errCnt;
      }
    }
  }
} 

/*----------------------------------------------------------------------------*/

void
ak341SetTransactionComment (tak_all_command_glob& acv,
                            tsp00_Int2            commentLength,
                            const tak_WydeArray*  pComment);

/*----------------------------------------------------------------------------*/

inline static void
ak341LockContainer(
  tak_all_command_glob &acv,
  const tgg00_FileId   &fileId)
{
  kb04LockContainer(acv.a_transinf.tri_trans, fileId);
}

/*----------------------------------------------------------------------------*/

externC
void ak341OmsInspectTimeout();

#endif
