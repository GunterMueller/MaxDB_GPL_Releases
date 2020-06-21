/****************************************************************************

  module      : vbd90.cpp

  -------------------------------------------------------------------------

  responsible : FerdiF

  special area: BD :       Operations for processing object files
  description : 


  version     : 7.4.4.0
  last changed: 2002-12-02
  see also    : 

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 1998-2005 SAP AG



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

/*  WAIT FOR LOCK RELEASE
*
* kb50LockObj
*   \kb67LockObj
*     |\kb67NewObj
*     |  |\bd90ReuseObj
*     |  |   \bd04_ReuseObj
*     |  |      \bd04NewObjKey (wait)
*     |   \bd90_NewObjFromThisPage (nowait)
*     |\bd90LockObj
*     |   \bd04LockObj (wait)
*     |\bd90_DelAllObjOnThisPage (nowait)
*      \bd90NextObjConsistentOnThisPage (nowait)
*         \bd90NextObjConsistentOnThisPage
*/

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include <stdlib.h>

#ifndef HBD90_H
#   include "hbd90.h"
#endif
#ifndef GSP03_H
#   include "gsp03.h"
#endif
#ifndef GSP03_3_H
#   include "gsp03_3.h"
#endif
#ifndef GGG01_H
#   include "ggg01.h" // OMS errors
#endif
#ifndef GBD05_H
#   include "gbd05.h"
#endif
#ifndef GBD900_H
#   include "gbd900.h"
#endif
#ifndef GBD910_H
#   include "gbd910.h"
#endif
#ifndef GBD920_H
#   include "gbd920.h"
#endif
#ifndef GBD930_H
#   include "gbd930.h"
#endif

#ifndef HBD06_H
#   include "hbd06.h"
#endif
#ifndef HBD20_1_H
#   include "hbd20_1.h"
#endif
#ifndef HBD20_9_H
#   include "hbd20_9.h"
#endif
#ifndef HBD22_H
#   include "hbd22.h"
#endif
#ifndef HBD902_H
#   include "hbd902.h"
#endif
#ifndef HBD91_H
#   include "hbd91.h"
#endif
#ifndef HBD911_H
#   include "hbd911.h"
#endif
#ifndef HBD92_H
#   include "hbd92.h"
#endif
#ifndef HBD93_H
#   include "hbd93.h"
#endif
#ifndef HBD95_1_H
#   include "hbd95_1.h"
#endif
#ifndef HBD300_H
#   include "hbd300.h" // SetRootCheck
#endif
#ifndef HGG01_H
#   include "hgg01.h" 
#endif
#ifndef HGG01_1_H
#   include "hgg01_1.h" 
#endif
#ifndef HGG06_H
#   include "hgg06.h"
#endif
#ifndef SAPDB_RANGECODE_HPP
#   include "SAPDB/SAPDBCommon/SAPDB_RangeCode.hpp" // Kernel_move_and_fill
#endif
#ifndef HGG17_H
#   include "hgg17.h"
#endif
#ifndef HKB50_H
#   include "hkb50.h"
#endif
#ifndef HKB53_H
#   include "hkb53.h"
#endif
#ifndef HKB67_H
#   include "hkb67.h"
#endif

#ifndef LVC_ERRORTRACEOBJECT_HPP
#   include "liveCache/LVC_ErrorTraceObject.hpp"
#endif
#ifndef Log_History_HPP
#   include "Logging/Log_History.hpp"
#endif
#ifndef Log_Transaction_HPP
#   include "Logging/Log_Transaction.hpp"              // PTS 1114914 FF 2002-03-13
#endif
#ifndef KERNEL_OPMSG_HPP
#   include "KernelCommon/Kernel_OpMsg.hpp"            /* PTS 1114891 FF 2002-03-14 */
#endif
#ifndef KERNEL_VTRACE_HPP
#   include "KernelCommon/Kernel_VTrace.hpp"           /* PTS 1120151 */ 
#endif
#ifndef KERNEL_IADMININFO_HPP
#   include "KernelCommon/Kernel_IAdminInfo.hpp"
#endif
#ifndef FILEDIR_OMSVAR_HPP
#   include "FileDirectory/FileDir_OmsVar.hpp"
#endif
#ifndef CONVERTER_ICOMMON_HPP
#   include "Converter/Converter_ICommon.hpp"
#endif
#ifndef MSG_LIST_HPP
#   include "Messages/Msg_List.hpp"
#endif
#ifndef TRANS_CONTEXT_HPP
#   include "Transaction/Trans_Context.hpp"
#endif

#include "SAPDBCommon/Messages/SDBMsg_ObjectContainer.h"

#if COMPILEMODE_MEO00 >= SLOW_MEO00 
#include "hta99.h"
#include "hta01.h"
#endif

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#ifdef NO_INLINES
#  define _INLINE
#else
#  define _INLINE   inline
#endif

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

#if COMPILEMODE_MEO00 >= SLOW_MEO00 

#   define BD90_CHECK_KEY_SIZE(TR_ERROR, HASH_KEY_SIZE, OBJ_KEY_SIZE) \
            if (HASH_KEY_SIZE != OBJ_KEY_SIZE)                        \
            {                                                         \
                TR_ERROR = e_illegal_keylength;                       \ 
                return;                                               \ 
            }                                                        

#else

#   define BD90_CHECK_KEY_SIZE(TR_ERROR, HASH_KEY_SIZE, OBJ_KEY_SIZE) 

#endif

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

enum ebd90_FunctionName {Delete, Get, KeyGet, GetCons, KeyGetCons,
                         Lock, Reuse, Unlock, Update, Redo};

/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/

extern cbd930_GarbCollController  bd91GarbCollController;

/*===========================================================================*
 *  GLOBAL VARIABLES                                                          *
 *===========================================================================*/

static        long g_Committed_obsReserved_Cnt  = 0;          

static const  bool bExclusive                   = true;
static const  bool cDoShareLock                 = true;
static        bool isOnline                     = false;


/*===========================================================================*
 *  LOCAL FUNCTIONS (PROTOTYPES)                                             *
 *===========================================================================*/

class CFreePageListEntry 
{
public:
    CFreePageListEntry() 
        : m_pageno(NIL_PAGE_NO_GG00)
        , m_PageReleased(false)
    { };
    CFreePageListEntry(tsp00_PageNo pageno) 
        : m_pageno(pageno)
        , m_PageReleased(false)
    { };
    const CFreePageListEntry & assign(const CFreePageListEntry& right, tgg00_BasisError&) {
        m_pageno = right.m_pageno;
        m_PageReleased  = right.m_PageReleased;
        return *this;
    };
    tsp00_PageNo& PageNo() { 
        return m_pageno; 
    };
    bool& PageReleased() { 
        return m_PageReleased; 
    };
private:
    tsp00_PageNo m_pageno;
    bool         m_PageReleased;
};

#define FREEPAGELISTHASH_NUM_ENTRIES_ALLOCATE  50 
#define FREEPAGELISTHASH_NUM_ENTRIES_EXTEND    50 

class  CFreePageListHash : public cbd920_ChainedListHash <SAPDB_UInt8, CFreePageListEntry
	, FREEPAGELISTHASH_NUM_ENTRIES_ALLOCATE, FREEPAGELISTHASH_NUM_ENTRIES_EXTEND>
{
public:
	CFreePageListHash(SAPDBMem_IRawAllocator& alloc)
		: cbd920_ChainedListHash <SAPDB_UInt8, CFreePageListEntry
		, FREEPAGELISTHASH_NUM_ENTRIES_ALLOCATE, FREEPAGELISTHASH_NUM_ENTRIES_EXTEND>(alloc)
	{};
};

/*---------------------------------------------------------------------------*/

typedef cbd920_HashIter <SAPDB_UInt8, CFreePageListEntry, 
FREEPAGELISTHASH_NUM_ENTRIES_ALLOCATE, FREEPAGELISTHASH_NUM_ENTRIES_EXTEND> 
CFreePageListHashIter;

/*---------------------------------------------------------------------------*/
static CFreePageListEntry DummyFreePageEntry;

static CFreePageListEntry&
bd90_GetCachedFreePage(tgg00_TransContext &Trans, FileDir_FileNo FileNo)
{
    if (NULL == Trans.trFreePagesForIns_gg00)
    {
        SAPDBMem_IRawAllocator &RawAllocator = *(REINTERPRET_CAST(SAPDBMem_IRawAllocator*, Trans.trAllocator_gg00));
        Trans.trFreePagesForIns_gg00 = new (RawAllocator) CFreePageListHash(RawAllocator);
    }
    if (NULL == Trans.trFreePagesForIns_gg00)
    {
        DummyFreePageEntry.PageNo()       = NIL_PAGE_NO_GG00;
        DummyFreePageEntry.PageReleased() = false;
        return DummyFreePageEntry;
    }
    CFreePageListHash& freePageList = 
        *reinterpret_cast<CFreePageListHash*>(Trans.trFreePagesForIns_gg00);
    return freePageList[FileNo.GetUInt8()];
};

/*---------------------------------------------------------------------------*/

static bool 
bd90_PutCachedFreePage(tgg00_TransContext &Trans, FileDir_FileNo FileNo, tsp00_PageNo PageNo)
{
    bool bRC = false;
    if (NULL != Trans.trFreePagesForIns_gg00)
    {
        CFreePageListHash& freePageList =
        *reinterpret_cast<CFreePageListHash*>(Trans.trFreePagesForIns_gg00);
        CFreePageListEntry entry(PageNo);
        freePageList.bd920InsItem (Trans.trTaskId_gg00, FileNo.GetUInt8(), &entry, Trans.trError_gg00);
        if (e_ok == Trans.trError_gg00)
            bRC = true;
        else 
            Trans.trError_gg00 = e_ok;
    }
    return bRC;
}

/*---------------------------------------------------------------------------*/

static void
bd90_DelCachedFreePage(tgg00_TransContext &Trans, FileDir_FileNo FileNo)
{
    if (NULL != Trans.trFreePagesForIns_gg00)
    {
        CFreePageListHash& freePageList = 
        *reinterpret_cast<CFreePageListHash*>(Trans.trFreePagesForIns_gg00);
        freePageList.bd920DelItem (Trans.trTaskId_gg00, FileNo.GetUInt8());
    }
}

/*---------------------------------------------------------------------------*/

static _INLINE void
bd90_DelAllObjOnThisPage (cbd05_Current           &Current,
                          FileDir_Oms         &fd_smart,
                          cbd910_PrimObjDataNode  &NptrsPage,
                          tsp00_Int4              &NumDelObj,
                          tgg92_KernelOid         &ErrorOid,
                          Msg_List                &ErrorList);

/*---------------------------------------------------------------------------*/

static _INLINE void
bd90_GetObjBody   (tgg00_TransContext &Trans,
                   tgg92_KernelOid     OId,
                   tbd_nodeptr         pPage,
                   tsp00_Int4          ExtObjBodySize,
                   void               *pExtObjBody,
                   tbd900_ObjFrame    *pObjFrame);

/*---------------------------------------------------------------------------*/

static void
bd90_NewObjFromThisPage (cbd05_Current            &Current,
                         cbd910_PrimObjDataNode   &NptrsPage,
                         tsp00_Int4                KeyLen,
                         void                     *pKey,
                         SAPDB_Int1                bcontObj,
                         tgg92_KernelOid          &OId,
                         tgg91_PageRef            &ResultObjVers,
                         Msg_List                 &ErrorList);

/*---------------------------------------------------------------------------*/

static _INLINE void
bd90_NoteVersionErrorInKnldiag (ebd90_FunctionName  FunctionName,
                                tgg92_KernelOid     WantedOId,
                                tgg00_ObjFrameVers  ExpectedVersion);

/*---------------------------------------------------------------------------*/
/* PTS 1111513 13/12/2001 */
static void
bd90_ReadAheadFile (tgg00_TransContext &Trans,
                    cbd910_ObjNode     &NptrsPage);

/*---------------------------------------------------------------------------*/

static void
bd90_UpdateObjBody (cbd05_Current            &Current,
                    tgg92_KernelOid           OId,
                    tgg91_PageRef             ObjVers,
                    tbd_nodeptr               pPage,
                    tbd900_ObjFrame          *pObjFrame,
                    tsp00_Int4                NewObjBodyLen,
                    void                     *pNewObjBody,
                    const Log_EntrySequence  &RedoEntrySequence,
                    const tgg91_TransNo      &RedoUpdTransNo,
                    SAPDB_Int1                contVarObj,   
                    bool                      bOmsLayerReusedObj,
                    Msg_List                 &ErrorList);

/*---------------------------------------------------------------------------*/

static _INLINE tsp00_Int4
bd90_VarObjBodySize (tbd900_ObjFrame *pObjFrame);

/*---------------------------------------------------------------------------*/

static _INLINE tsp00_Uint1 
bd90_VarObjPrimContObjType(tbd900_ObjFrame *pObjFrame);

/*---------------------------------------------------------------------------*/

static void
bd90_GetConsistentObjImage (cbd05_Current           &Current,             //[in/out]
                            FileDir_Oms         &fd_smart,
                            tgg92_KernelOid         &OId,                 //[in/out] frame version may change
                            tgg91_PageRef           &ResultObjVers,
                            tgg91_PageRef           &ObjHistRef,          // PTS 1120478 FF
                            tsp00_Bool               bWithObject,
                            bool                     bOnlyHdrVarObjects,  // used for omsVarObjIter
                            tsp00_Int4               ExtObjBodySize,
                            void                    *pExtObjBody,
                            cbd910_PrimObjDataNode  &NptrsPage,
                            tsp00_Int4              &ObjBodySize,         //[out=length of the object]
                            tsp00_Bool              &bConsistentObjImageObjFound,
                            tsp00_Int4              &LogReadAccesses,
                            Msg_List                &ErrorList);

/*===========================================================================*
 *  GLOBAL FUNCTIONS (CODE)                                                  * 
 *===========================================================================*/

externCpp void 
bd902FreeFreePageListCache(tgg00_TransContext &Trans)
{
    if (Trans.trFreePagesForIns_gg00)
    {
        SAPDBMem_IRawAllocator &RawAllocator = 
            *(REINTERPRET_CAST(SAPDBMem_IRawAllocator*, Trans.trAllocator_gg00));
        CFreePageListHash *freePageListPtr = 
            reinterpret_cast<CFreePageListHash*>(Trans.trFreePagesForIns_gg00);
        destroy(freePageListPtr,RawAllocator);
        Trans.trFreePagesForIns_gg00         = 0;
    };
};

/*---------------------------------------------------------------------------*/

externCpp void 
bd90TransEnd(tgg00_TransContext  &Trans)
{
    bd902RemoveLockShareSchemaFlag(Trans);

    if (NULL != Trans.trFreePagesForIns_gg00)
    {
        tgg00_BasisError      &TrError  = Trans.trError_gg00;
        CFreePageListHash& freePageList = 
            *reinterpret_cast<CFreePageListHash*>(Trans.trFreePagesForIns_gg00);
        CFreePageListHashIter Iter (freePageList);
        for  (Iter.bd920Begin(); !Iter.bd920End() ; ++Iter) 
        {   
            tsp00_PageNo PageNo = Iter()->PageNo();
            if (NIL_PAGE_NO_GG00 != PageNo)
            {
   		        Msg_List& ErrorList = Trans_Context::GetContext(Trans).ErrorList();
			    FileDir_Oms    fd_smart;
			    if ( !bd90GetFile( Trans, fd_smart,  Iter.bd920Key(), cDoShareLock, __LINE__, ErrorList))
						return;
                cbd05_Current Current(Trans, m_update);
                fd_smart.BuildFileId (Current.curr_tree_id);

                const tsp00_PageNo  SubRoot  = fd_smart.GetRootPage();
                if ( !(Iter()->PageReleased()) )
                {
                    cbd910_SubRootNode  NptrsSubRoot (Current, SubRoot, nr_for_update);
                    if (e_ok != TrError)
                    {
                        Kernel_OpWarning (csp3_bd_msg, csp3_n_obj)
                            << "bd90TransEnd access to subroot failed"
                            << " FileNo:" << Iter.bd920Key()
                            << " SubRoot:" << SubRoot
                            << " Error:" << TrError;
                        TrError = e_ok;
                    }

                    cbd910_PrimObjDataNode NptrsPage (Current, PageNo, nr_for_update);
                    if (e_ok != TrError)
                    {
                        Kernel_OpWarning (csp3_bd_msg, csp3_n_obj)
                            << "bd90TransEnd access to page failed"
                            << " FileNo:" << Iter.bd920Key()
                            << " Pageno:" << PageNo
                            << " Error:" << TrError;
                        TrError = e_ok;
                    }

                    if ( NptrsSubRoot.bd910PageIsAvailable())
                    {
                        if ( NptrsPage.bd910PageIsAvailable())
                        {
							NptrsPage.np_ptr()->nd_objpage_state().delElement(objp_no_chain_to_freelist); /* re-insert allowed */
							Converter_Version converterVersion = 
								bd20GetDestinationConverterVersion ( Trans.trTaskId_gg00, NptrsPage.np_cbptr() );
							NptrsSubRoot.bd910InsertPageIntoFreeChain (NptrsPage, converterVersion, fd_smart); 
						}
					}
                    else if ( NptrsPage.bd910PageIsAvailable())
                        NptrsPage.np_ptr()->nd_objpage_state().delElement(objp_no_chain_to_freelist); /* re-insert allowed */

                }
                Iter()->PageReleased() = false;
                Iter()->PageNo() = NIL_PAGE_NO_GG00;
            } 
        }
        freePageList.bd920DelAllItems(Trans.trTaskId_gg00);
    }
};

/*---------------------------------------------------------------------------*/

externCpp bool
bd90GetFile(tgg00_TransContext      &Trans,
            FileDir_Oms             &fd_smart,
            const FileDir_FileNo    &fileId,
            bool                     doShareLock,
            int                      line,
            Msg_List                &ErrorList)
{
    {      
        FileDir_ReturnCode    fd_rc;
        fd_rc = FileDir_IDirectory::SessionInstance(Trans).GcGetFile(
            Trans.trTaskId_gg00, fileId, fd_smart, ErrorList);
        if (FileDir_Okay != fd_rc) {
            bd900MapFileDirError(Trans, fd_rc, e_container_dropped, (char*)__FILE__,  line );
            if (FileDir_FileNoNotFound == fd_rc)
                ErrorList.ClearMessageList();
            return false;
        };
    }

    /* get share lock before trying to access information within the filedir */
    /* therefore only committed OmsDropContainer are seen bey the following  */
    /* call to the file directory                                            */
    if (( doShareLock ) && !fd_smart.GetLockState())
    {
        bd902LockShareSchema(Trans, fd_smart, fileId);
        if (e_ok != Trans.trError_gg00)
            return false;

        tgg00_LockReqMode  ReqMode;
        ReqMode.becomes(lckTabShare_egg00);
        tgg00_Surrogate  TableId;
        SAPDB_MemCopyNoCheck (&TableId[0], &fileId, sizeof(TableId));
        bd90LockObjContainer(Trans, TableId, ReqMode);
        if (e_ok != Trans.trError_gg00)
            return false;
        if (isOnline) 
            fd_smart.SetLockState(1);
    }
    if (Data_FileDeleted == fd_smart.GetFileState())
    {
        Trans.trError_gg00 = e_container_dropped;
        if ((g01vtrace.vtrBdObject_gg00) || 
            (g01vtrace.vtrOmsGet_gg00 )  ||
            (g01vtrace.vtrOmsNew_gg00 )  ||
            (g01vtrace.vtrOmsUpd_gg00 ))
        {
            Kernel_VTrace() << __FILE__ << ":" << __LINE__ 
                << " Container dropped:"
                << fileId;
        }
        return false;
    }
    return true;
}

/*---------------------------------------------------------------------------*/

externCpp void 
bd90LockObjContainer( tgg00_TransContext  &Trans
                     ,tgg00_Surrogate     &TableId
                     ,tgg00_LockReqMode   &ReqMode )
{
    tgg00_LockReqState  ReqState;
    tgg00_LockReqMode   GrantedLock;
    tgg00_Lkey          DummyKey;

     // PTS 1132041
    if ( !isOnline )
    {
        isOnline = Kernel_IAdminInfo::Instance().KernelStateIsOnline(); 
        if ( !isOnline )
            return;
    }

    ReqState.clear();
    if ( lckTabExcl_egg00 == ReqMode)
        ReqState.addElement(lrsEotExcl_egg00);
    GrantedLock.becomes(lckFree_egg00);

    bool bTryAgain = true;
    while ((e_ok == Trans.trError_gg00) && bTryAgain)  
    {
        k53lock (Trans, TableId, DummyKey, ReqMode, ReqState,
            0 /*nowait*/, 0 /*collision test*/, GrantedLock);
        if (e_wait_for_lock_release == Trans.trError_gg00)
        {   
            Trans.trError_gg00 = e_ok;
            kb50WaitAndRetryObjLock (Trans, m_lock, mm_object);
            bTryAgain = (e_ok == Trans.trError_gg00);  
        }
        else
            bTryAgain = false;
    };
}

/*---------------------------------------------------------------------------*/

externCpp void
bd90CreateObjFile (cbd05_Current         &Current,
                   tsp00_Int4             ObjBodyLen,
                   tsp00_Int4             ObjKeyLen,
                   tsp00_Int4             NumObjKeyFiles,
                   cbd910_ObjDataNode    &NptrsRoot,
                   Converter_Version     &converterVersion,
                   Msg_List              &ErrorList)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd90CreateObjFile", ObjContainer_Trace, 5);

    /* please if you change this function change the */
    /* function bd90Rollforward_CreateObjFile too    */

    tgg00_BasisError      &TrError    = Current.curr_trans->trError_gg00;
    tgg00_TransContext    &Trans      = *Current.curr_trans;
    LVC_RootArray          RootArray;

    /* check precondition for calling this function */
    SAPDBERR_ASSERT_STATE (!Kernel_IAdminInfo::Instance().KernelStateIsRedoLogProcessing() ||
                           (PRIM_CONT_OBJ_FILE_NO_BD900 != Current.curr_tree_id.fileContObjFileNo_gg00()));

    /* create root page */
    NptrsRoot.bd910AssignToNewPage (ObjBodyLen, ObjKeyLen, NumObjKeyFiles, NIL_PAGE_NO_GG00,
                                    MAX_LOCK_REQUEST_SEQ_NO_BD900);
    if (e_ok != TrError)
    {
        Kernel_OpError( csp3_bd_msg, csp3_n_obj ) << __FILE__ << ":" << __LINE__  
            << " bd910AssignToNewPage error:" << TrError;
        return;
    }

    /* because of missing savepoint synchronization, releases page and re-access it PTS 1134329 */
    const tsp00_PageNo   NewRootPno = NptrsRoot.np_ptr()->nd_id();
    NptrsRoot.bd910ReleasePage();  
    NptrsRoot.bd910AssignToPage(NewRootPno);
    if (e_ok != TrError)
    {
        Kernel_OpError( csp3_bd_msg, csp3_n_obj ) << __FILE__ << ":" << __LINE__  
            << " bd910AssignToPage error:" << TrError;
        return;
    }

    converterVersion = 
        bd20GetDestinationConverterVersion ( Trans.trTaskId_gg00, NptrsRoot.np_cbptr() );

    /* update file id */
    Current.curr_tree_id.fileNumObjKeyFiles_gg00() = NumObjKeyFiles;
    Current.curr_tree_id.fileRoot_gg00()           = NptrsRoot.np_ptr()->nd_id();
    bd300SetRootCheck (Current.curr_tree_id);

    if  (Current.curr_tree_id.fileObjFileType_gg00() == oftVarLenObjFile_egg00)
    {
        SAPDB_UInt1 ix;
        for (ix=1; ix <= 6; ix++ ) 
        {
            cbd05_Current CurrentCont (Trans, m_create_file);
            FileDir_IDirectory::SessionInstance(Trans).BuildNewOmsFileId(
                Current.curr_tree_id.fileDirFileId_gg00(), 
                Current.curr_tree_id.fileObjFileType_gg00(), 
                ix, CurrentCont.curr_tree_id);
            cbd910_RootNode   NptrsContRoot (CurrentCont, nr_for_update);
            NptrsContRoot.bd910AssignToNewPage (bd93GetContFileBodySizes(ix), 0, 0, NIL_PAGE_NO_GG00,
                MAX_LOCK_REQUEST_SEQ_NO_BD900);
            if (e_ok != TrError)
            {
                Kernel_OpError( csp3_bd_msg, csp3_n_obj )
                    << __FILE__ << ":" << __LINE__  
                    << " bd910AssignToNewPage error:" << TrError;
                return;
            }
            RootArray[ix-1] = NptrsContRoot.np_ptr()->nd_id();
        }
    }

    bd91AddFileToObjFileDir (Trans, Current.curr_tree_id, NptrsRoot, converterVersion, 
        NumObjKeyFiles, RootArray, ErrorList);

    FileDir_FileNo      FileNo = Current.curr_tree_id.fileDirFileId_gg00();
    FileDir_Oms         fd_smart;
    FileDir_ReturnCode  fd_rc;
    fd_rc = FileDir_IDirectory::SessionInstance(Trans).GetFile(
        Trans.trTaskId_gg00, FileNo, fd_smart, ErrorList);
    if (FileDir_Okay != fd_rc) {
        bd900MapFileDirError(Trans, fd_rc, e_container_dropped, (char*)__FILE__,  __LINE__ );
        return;
    }
    if (NptrsRoot.np_ptr()->nd_max_obj_cnt() > 0) 
    {
        fd_smart.ModifyFreePageCount(Current.curr_trans->trTaskId_gg00,converterVersion, 1); 
        if ( ObjContainer_Trace.TracesLevel(4) )
        {
            Kernel_VTrace() << __FILE__ << ":" << __LINE__ 
                << " bd90CreateObjFile:ModifyFreePageCount +1"
                << " root:"
                << NptrsRoot.np_ptr()->nd_id();
        }
    }

    kb67CreateContainer (Trans, Current.curr_tree_id, NptrsRoot, ErrorList);

    /* remove pages if the file existed already in the file directory */
    if (e_duplicate_filename == TrError)
    {
        TrError = e_ok;
        NptrsRoot.bd910FreePage();
        TrError = e_duplicate_filename;
    }
}

/*---------------------------------------------------------------------------*/

externCpp void
bd90Rollforward_CreateObjFile (cbd05_Current         &Current,
                               tsp00_Int4             ObjBodyLen,
                               tsp00_Int4             ObjKeyLen,
                               tsp00_Int4             NumObjKeyFiles,
                               Data_PageNo            rootPage,
                               cbd910_ObjDataNode    &NptrsRoot,
                               Converter_Version     &converterVersion,
                               Msg_List              &ErrorList)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd90Rollforward_CreateObjFile", ObjContainer_Trace, 5);

    /* please if you change this function change the */
    /* function bd90CreateObjFile too                */

    tgg00_BasisError      &TrError    = Current.curr_trans->trError_gg00;
    tgg00_TransContext    &Trans      = *Current.curr_trans;
    tsp00_Bool             bPageGenerated;
    LVC_RootArray          RootArray;

    /* check that the rollforward of a create takes place only during a restart */
    SAPDBERR_ASSERT_STATE (Kernel_IAdminInfo::Instance().KernelStateIsRedoLogProcessing());

    FileDir_FileNo CurrentObjId = Current.curr_tree_id.fileDirFileId_gg00();

    /* create root page */
    NptrsRoot.bd910AssignToNewSpecifiedPage (rootPage, bPageGenerated);
    if (e_ok != TrError )
    {
        ErrorList.AppendNewMessage( Msg_List( Msg_List::Error
            , SDBMSG_OBJECTCONTAINER_ROOTPAGE_RECREATION_FAILED
            , Msg_Arg(SDBMSGTAG_OBJECTCONTAINER_ROOTPAGE_RECREATION_FAILED__FILENO 
            , FileDir_FileNoToString( CurrentObjId ))
            , Msg_Arg(SDBMSGTAG_OBJECTCONTAINER_ROOTPAGE_RECREATION_FAILED__PAGENO, rootPage)
            , Msg_Arg(SDBMSGTAG_OBJECTCONTAINER_ROOTPAGE_RECREATION_FAILED__BASIS_ERROR, TrError)));
        return;
    }

    converterVersion = 
        bd20GetDestinationConverterVersion ( Trans.trTaskId_gg00, NptrsRoot.np_cbptr() );

    if ( !bPageGenerated )
    {
        FileDir_FileNo filenoOnPage;
        bd900GetFileNo(NptrsRoot.np_ptr(), filenoOnPage);

        if ( CurrentObjId == filenoOnPage )
        {
            /* only ignore error if transaction was open in last savepoint */
            if (!((Log_Transaction *) Trans.trLogContext_gg00)->WasOpenInLastSavepoint())
            {
                TrError = e_bad_datapage;
                ErrorList.AppendNewMessage( Msg_List( Msg_List::Error
                    , SDBMSG_OBJECTCONTAINER_ROOTPAGE_RECREATION_FAILED
                    , Msg_Arg(SDBMSGTAG_OBJECTCONTAINER_ROOTPAGE_RECREATION_FAILED__FILENO 
                    , FileDir_FileNoToString( CurrentObjId ))
                    , Msg_Arg(SDBMSGTAG_OBJECTCONTAINER_ROOTPAGE_RECREATION_FAILED__PAGENO, rootPage)
                    , Msg_Arg(SDBMSGTAG_OBJECTCONTAINER_ROOTPAGE_RECREATION_FAILED__BASIS_ERROR, TrError)));
                return;
            }
        }
        else
        {
            /* the page does not belong to the wanted file and has to     */
            /* be removed from the file currently owning the object frame */

            /* because the current pointed to by NptrsRoot is changed by the */
            /* bd95RemovePageFromObjFile save fileid                         */
            const tgg00_FileId savFileId = Current.curr_tree_id; 
            bd95RemovePageFromObjFile (Trans, converterVersion, NptrsRoot);
            if (TrError != e_ok )
            {
                Kernel_OpWarning( csp3_bd_msg, csp3_n_obj )
                    << __FILE__ << ":" << __LINE__  
                    << " bd95RemovePageFromObjFile error:" << TrError
                    << " root page no:" << rootPage
                    << " ignored";
                // it's ok to ignore errors,
                // because the !bPageGenerated (on nect statement) checks if operation was successfull
                TrError = e_ok; 
            }

            /* reconstruct file id for the wanted file */
            Current.curr_tree_id = savFileId;

            /* reassign the wanted page */
            NptrsRoot.bd910AssignToNewSpecifiedPage (rootPage, bPageGenerated);
            if ((TrError != e_ok ) || !bPageGenerated ) 
            {
                if ( e_ok == TrError )
                    TrError = e_bad_datapage;

                ErrorList.AppendNewMessage( Msg_List( Msg_List::Error
                    , SDBMSG_OBJECTCONTAINER_ROOTPAGE_RECREATION_FAILED
                    , Msg_Arg(SDBMSGTAG_OBJECTCONTAINER_ROOTPAGE_RECREATION_FAILED__FILENO 
                    , FileDir_FileNoToString( CurrentObjId ))
                    , Msg_Arg(SDBMSGTAG_OBJECTCONTAINER_ROOTPAGE_RECREATION_FAILED__PAGENO, rootPage)
                    , Msg_Arg(SDBMSGTAG_OBJECTCONTAINER_ROOTPAGE_RECREATION_FAILED__BASIS_ERROR, TrError)));
                return;
            }
        }
    }
    if ( bPageGenerated ) // PTS 1137325
    {
        NptrsRoot.bd910InitObjPage (ObjBodyLen, ObjKeyLen, NumObjKeyFiles, NIL_PAGE_NO_GG00,
            MAX_LOCK_REQUEST_SEQ_NO_BD900);
    }

    if (e_ok != TrError)
    {
        ErrorList.AppendNewMessage( Msg_List( Msg_List::Error
            , SDBMSG_OBJECTCONTAINER_ROOTPAGE_RECREATION_FAILED
            , Msg_Arg(SDBMSGTAG_OBJECTCONTAINER_ROOTPAGE_RECREATION_FAILED__FILENO 
            , FileDir_FileNoToString( CurrentObjId ))
            , Msg_Arg(SDBMSGTAG_OBJECTCONTAINER_ROOTPAGE_RECREATION_FAILED__PAGENO, rootPage)
            , Msg_Arg(SDBMSGTAG_OBJECTCONTAINER_ROOTPAGE_RECREATION_FAILED__BASIS_ERROR, TrError)));
        return;
    }

    /* because of missing savepoint synchronization, releases page and re-access it PTS 1134329 */
    NptrsRoot.bd910ReleasePage();  
    NptrsRoot.bd910AssignToPage(rootPage);
    if (e_ok != TrError)
    {
        Kernel_OpError( csp3_bd_msg, csp3_n_obj ) << __FILE__ << ":" << __LINE__  
            << " bd910AssignToPage error:" << TrError;
        return;
    }

    /* update file id */
    Current.curr_tree_id.fileNumObjKeyFiles_gg00() = NumObjKeyFiles;
    Current.curr_tree_id.fileRoot_gg00()           = NptrsRoot.np_ptr()->nd_id();
    bd300SetRootCheck (Current.curr_tree_id);

    if  (Current.curr_tree_id.fileObjFileType_gg00() == oftVarLenObjFile_egg00)
    {
        SAPDB_UInt1 ix;
        for (ix=1; ix <= 6; ix++ ) 
        {
            cbd05_Current CurrentCont (Trans, m_update);
            FileDir_IDirectory::SessionInstance(Trans).BuildNewOmsFileId(
                Current.curr_tree_id.fileDirFileId_gg00(), 
                Current.curr_tree_id.fileObjFileType_gg00(), 
                ix, CurrentCont.curr_tree_id);
            cbd910_RootNode   NptrsContRoot (CurrentCont, nr_for_update);

            /* if not accessable create a new root page */
            NptrsContRoot.bd910AssignToNewPage (bd93GetContFileBodySizes(ix), 0, 0, NIL_PAGE_NO_GG00,
                MAX_LOCK_REQUEST_SEQ_NO_BD900);
            if (e_ok != TrError)
            {
                Kernel_OpError( csp3_bd_msg, csp3_n_obj )
                    << __FILE__ << ":" << __LINE__  
                    << " bd910AssignToNewPage error:" << TrError;
                return;
            }
            RootArray[ix-1] = NptrsContRoot.np_ptr()->nd_id();
        }
    }

    bd91AddFileToObjFileDir (*Current.curr_trans, Current.curr_tree_id, NptrsRoot, converterVersion, 
        NumObjKeyFiles, RootArray, ErrorList);

    /* ignore duplicate error, if transaction was open during last savepoint */
    if ((e_duplicate_filename == TrError) &&
        ((Log_Transaction *) Trans.trLogContext_gg00)->WasOpenInLastSavepoint())
    {
        TrError = e_ok;
        ErrorList.ClearMessageList();
    }
    else // 1137309
    {
        if ((e_ok == TrError) && (NptrsRoot.np_ptr()->nd_max_obj_cnt() > 0)) 
        {
            FileDir_Oms     fd_smart;
            FileDir_ReturnCode fd_rc = 
                FileDir_IDirectory::Instance(Trans.trIndex_gg00).GetFile(
                Trans.trTaskId_gg00, CurrentObjId, fd_smart, ErrorList);
            if (FileDir_Okay != fd_rc)
            {
                bd900MapFileDirError(Trans, fd_rc, e_file_not_found, (char*)__FILE__, __LINE__ );
                Kernel_OpError( csp3_bd_msg, csp3_n_obj )
                    << __FILE__ << ":" << __LINE__ 
                    << " File (ClassID:" << CurrentObjId << ") FileDir Error:" << fd_rc;
            }
            else
            {
                fd_smart.ModifyFreePageCount(Trans.trTaskId_gg00,converterVersion, 1); 
                if ( ObjContainer_Trace.TracesLevel(4) )
                {
                    Kernel_VTrace() << __FILE__ << ":" << __LINE__ 
                        << " bd90Rollforward_CreateObjFile:ModifyFreePageCount +1"
                        << " root:"
                        << NptrsRoot.np_ptr()->nd_id();
                }
            }
        }
    }
}

/*---------------------------------------------------------------------------*/

externCpp void 
bd90DelObj (cbd05_Current         &Current,             //[inp\out]
            tgg92_KernelOid       &OId,                 //[inp\out] 
            tgg91_PageRef          WantedObjVers,       //[inp]
            SAPDB_Int1             contVarObj,          //[inp] 
            bool                   bOmsLayerReusedObj,  //[inp]
            Msg_List              &ErrorList)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd90DelObj", ObjContainer_Trace, 5);

    /*  This routine marks an object as deleted, i.e. the state  */
    /*  it's state  switched to free after eot                   */

    tgg00_BasisError    &TrError = Current.curr_trans->trError_gg00;

    /*  get page pointer  */
    cbd910_PrimObjDataNode    NptrsPage (Current, OId.gg92GetPno(), nr_for_update);
    if (e_ok != TrError) return;

    // PTS 1119647 FF 2002-DEC-05
    FileDir_FileNo FileNo;
    bd900GetFileNo(NptrsPage.np_ptr(), FileNo);
    tgg00_TransContext    &Trans       = *Current.curr_trans;
    FileDir_Oms    fd_smart;
    if ( !bd90GetFile( *Current.curr_trans, fd_smart,  FileNo, cDoShareLock, __LINE__, ErrorList))
        return;

    /* determine pointer to object frame */
    BD92CHECK_OID_POSITION ("BD90DelObj", TrError, OId, NptrsPage);

    tbd900_ObjFrame  *pObjFrame = NptrsPage.bd910ObjFramePtr (OId.gg92GetPos());

    /*  check object state  */
    if ((pObjFrame->objHeader_bd900.ohdState_gg92 != obsReserved_egg00    ) &&
            (pObjFrame->objHeader_bd900.ohdState_gg92 != obsKeyReserved_egg00 ) &&
            (pObjFrame->objHeader_bd900.ohdState_gg92 != obsOccupied_egg00   ))
    {
        TrError = e_wrong_object_state;
        return;
    }

    tgg00_ObjFrameVers newFrameVers = OId.gg92GetFrameVers();  

    /* check object version  */
    if (pObjFrame->objHeader_bd900.ohdFrameVersion_gg92 != OId.gg92GetFrameVers())
    {
        /* PTS 1125361 FF 2003-Nov-17 for omsKeyedObjects FrameVersion may have */
        /*                            been incremented by upper layers          */
        /* PTS 1127661 FF 2004-Apr-26 for any object upper layers may have changed                      */
        /*                            the FrameVersion. This is signalled by bOmsLayerReusedObj == true */
        if ( ! bOmsLayerReusedObj )
        {
            TrError = e_wrong_object_version;
            bd90_NoteVersionErrorInKnldiag
                (Delete,OId,pObjFrame->objHeader_bd900.ohdFrameVersion_gg92);
            return;
        }
        else {
          /* make sure, the right OID is written to undo/history entries */
          OId.gg92SetFrameVers(pObjFrame->objHeader_bd900.ohdFrameVersion_gg92);
        }
    }

    /* check if object is locked and create a log entry  */
    const bool  bPageIsForVarObj = (pt2VarObject_egg00 == NptrsPage.np_ptr()->nd_pt2());

    if (bPageIsForVarObj)
        bd93LogDelVarObj (Current, OId, WantedObjVers, NptrsPage.np_ptr(), pObjFrame,
        contVarObj, bOmsLayerReusedObj, ErrorList);  
    else
    {
        const int ObjBodySize = NptrsPage.np_ptr()->nd_obj_frame_len() - sizeof(tgg92_ObjHeader);

        kb67DelUpdObj (*Current.curr_trans, m_delete, Current.curr_tree_id,
            OId, NptrsPage.np_ptr()->nd_obj_key_len(), WantedObjVers,
            pObjFrame->objHeader_bd900, ObjBodySize, pObjFrame->objBody_bd900,
            contVarObj, bOmsLayerReusedObj, ErrorList);         
    }

    /* reset OID to original objFrameVers */ 
    OId.gg92SetFrameVers(newFrameVers);

    if (TrError != e_ok) {
      if ((g01vtrace.vtrBdObject_gg00) || (g01vtrace.vtrOmsUpd_gg00 ))
        Kernel_VTrace() << __FILE__ << " : " << __LINE__ 
        << " DelObj Error:" << TrError
        << " Oid=" << OId.gg92GetPno() << "." << OId.gg92GetPos()
        << " (vers " << OId.gg92GetFrameVers() << ")"
        << " ohdFrameVersion:" << pObjFrame->objHeader_bd900.ohdFrameVersion_gg92
        << " ObjVers" << WantedObjVers.gg91RefPno() << "." << WantedObjVers.gg91RefPos();
      return;
    }

    /* write OID.FrameVers into object frame */ 
    pObjFrame->objHeader_bd900.ohdFrameVersion_gg92 = newFrameVers; 

    /* mark the object as deleted  */
    pObjFrame->objHeader_bd900.ohdState_gg92.becomes(obsFreeAfterEot_egg00);

    /* update page header */
    ++NptrsPage.np_ptr()->nd_free_eot_obj_cnt();
    --NptrsPage.np_ptr()->nd_occ_obj_cnt();
}

/*---------------------------------------------------------------------------*/

externCpp void 
bd90DelAllObj (cbd05_Current        &Current,           //[inp\out]
               tsp00_Int4           &NumDelObj,         //[out]
               tgg92_KernelOid      &ErrorOid,
               Msg_List             &ErrorList)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd90DelAllObj", ObjContainer_Trace, 5);

    /* This routine marks all objects of the current file as deleted */
    /* Completely changed by PTS 1107145 AK 10/07/2000               */

    tgg00_BasisError         &TrError      = Current.curr_trans->trError_gg00;
    tgg00_TransContext       &Trans        = *Current.curr_trans;

    const FileDir_FileNo FileNo = Current.curr_tree_id.fileDirFileId_gg00(); 
    FileDir_Oms     fd_smart;
    if ( !bd90GetFile( Trans, fd_smart,  FileNo, cDoShareLock, __LINE__, ErrorList))
        return;

    const tsp00_PageNo          FirstSubRoot = fd_smart.GetRootPage();

    cbd910_PrimObjDataNode    NptrsPage (Current, FirstSubRoot, nr_for_update);
    if (e_ok != TrError)
    {
        Kernel_OpWarning( csp3_bd_msg, csp3_n_obj ) 
            << __FILE__ << ":" << __LINE__  
            << " error:" << TrError
            << " accessing root page:" << FirstSubRoot
            << " fileno:" 
            << FileNo;
        return;
    }

    NumDelObj = 0;      /* initialize counters */

    /* go through all pages of the chain and delete all objects */
    do
    {
        /* delete objects on this page */
        bd90_DelAllObjOnThisPage (Current, fd_smart, NptrsPage, NumDelObj, ErrorOid, ErrorList);
        if (TrError != e_ok) return;

        /* move to next page */
        NptrsPage.bd910MoveToNextPage();

    } while ((e_ok == TrError) && NptrsPage.bd910PageIsAvailable());
}

/*---------------------------------------------------------------------------*/

externCpp void
bd90DropObjFile (cbd05_Current         &Current,
                 Msg_List              &ErrorList)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd90DropObjFile", ObjContainer_Trace, 5);
    
    /* This function marks a file as to be deleted.                       */
    /* After calling this function no other operation is possible, because*/
    /* all those operation have to wait for the table (container) lock to */
    /* be released.                                                       */
    /* If DropObjFile is committed, then all further accesses to will be  */
    /* rejected ( file not found ).                                       */
    /* If DropObjFile is rollbacked, then all other operations get their  */
    /* table ( container ) share lock and may access the container as     */
    /* required.                                                          */

    tgg00_BasisError     &TrError   = Current.curr_trans->trError_gg00;
    tgg00_TransContext   &Trans     = *Current.curr_trans;
    FileDir_FileNo        FileNo    = Current.curr_tree_id.fileDirFileId_gg00();
    FileDir_Oms           fd_smart;

    if ( !bd90GetFile( Trans, fd_smart,  FileNo, !cDoShareLock, __LINE__, ErrorList))
        return;

    tgg91_TransNo DummyTransNo;               /* *** TODO *** */
    gg06SetNilTrans (DummyTransNo);           /* *** TODO *** */

    kb67DropContainer(Trans, FileNo, ErrorList);

    if ((Data_OmsKeyFile == fd_smart.GetFileType())&& (TrError == e_ok))
    {
        const tsp00_Int4 NumKeyFiles = fd_smart.GetKeyPartitionCount();
        for (tsp00_Int4 iKeyFileNo = 1; ((iKeyFileNo <=NumKeyFiles) && (TrError == e_ok)); ++iKeyFileNo)
        {
            FileDir_FileNo fd_fileid = fd_smart.GetKeyPartition(iKeyFileNo);
            if ( fd_fileid.IsValid() ) 
            {
                FileDir_Oms     fd_smart2;
                if ( bd90GetFile( Trans, fd_smart2,  fd_fileid, !cDoShareLock, __LINE__, ErrorList)) 
                {
                    fd_smart2.MarkFileDeleted(DummyTransNo);  /* *** TODO *** */
                    if ( ObjContainer_Trace.TracesLevel(2) )
                        Kernel_VTrace() << __FILE__ << ":" << __LINE__  
                        << " MarkFileDeleted -"
                        << " FileId:" << FileNo
                        << " KeyFileId:" << fd_fileid
                        << " KeyFileIndex:" << iKeyFileNo;
                }
            }
        }
    }

    fd_smart.MarkFileDeleted(DummyTransNo);   /* *** TODO *** */
    if ( ObjContainer_Trace.TracesLevel(2) )
        Kernel_VTrace() << __FILE__ << ":" << __LINE__  
        << " MarkFileDeleted -"
        << " FileId:" 
        << FileNo;
}

/*---------------------------------------------------------------------------*/
externCpp bool
bd90GetKeyedFile (tgg00_TransContext    &Trans,
             FileDir_OmsKey             &fd_key_smart,
             const FileDir_FileNo       &fileId,
             Msg_List                   &ErrorList)
{
    ROUTINE_DBG_MEO00 ("bd90GetKeyedFile ");

    return (bd90GetFile( Trans, fd_key_smart,  fileId, cDoShareLock, __LINE__, ErrorList));
}

/*---------------------------------------------------------------------------*/
externCpp void
bd90GetContainerId (cbd05_Current  &Current,            //[in\out]
                    tgg92_KernelOid    OId)                //[in]
{
    ROUTINE_DBG_MEO00 ("bd90GetContainerId ");

    tgg00_BasisError   &TrError = Current.curr_trans->trError_gg00;
    /* PTS 1118931 FF 14-Nov-2002 */
    if (NIL_PAGE_NO_GG00 == OId.gg92GetPno()) {
        TrError = e_invalid_oid;
        return;
    }

    /* get page pointer */
    cbd910_PrimObjDataNode   NptrsPage (Current, OId.gg92GetPno(), nr_for_read);
    if (e_ok != TrError) return;

}

/*---------------------------------------------------------------------------*/

externCpp void
bd90GetObjConsistent (cbd05_Current        &Current,            //[inp\out] 
                      tgg92_KernelOid      &OId,                //[inp\out] 
                      tsp00_Int4            ExtObjBodySize,     //[inp= buffersize for object]
                      void                 *pExtObjBody,        //[inp]
                      tsp00_Int4            HashKeySize,        //[inp]
                      void                 *pHashKey,           //[inp]  
                      tsp00_Int4           &ObjBodySize,        //[out=length of the object] 
                      tsp00_Int4           &KeyLen,             //[out]
                      tsp00_Byte*          *pKey,               //[out]
                      tgg91_PageRef        &ResultObjVers,      //[out]
                      tgg91_TransNo        &UpdTransId,         //[out] 
                      tsp00_Int4           &LogReadAccesses,    //[out] 
                      Msg_List             &ErrorList)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd90GetObjConsistent", ObjContainer_Trace, 5);

    tgg00_BasisError    &TrError        = Current.curr_trans->trError_gg00;
    tgg00_TransContext  &Trans          = *Current.curr_trans;
    const bool          bWithObject     = true;
    tgg91_PageRef       historyPageRef  = ResultObjVers;    // PTS 1124797 FF 2003-10-22
    ResultObjVers.gg91SetNilRef();                          // PTS 1116881 FF 2002-07-25
    LogReadAccesses                 = 0;                    

    /* PTS 1118931 FF 14-Nov-2002 */
    if (NIL_PAGE_NO_GG00 == OId.gg92GetPno()) {
        TrError = e_invalid_oid;
        return;
    }

    /*  get page pointer  */
    cbd910_PrimObjDataNode   NptrsPage (Current, OId.gg92GetPno(), nr_for_read);
    if (e_ok != TrError) return;

    // PTS 1119647 FF 2002-DEC-05
    FileDir_FileNo FileNo;
    bd900GetFileNo(NptrsPage.np_ptr(), FileNo);
    FileDir_Oms    fd_smart;
    if ( !bd90GetFile( Trans, fd_smart,  FileNo, cDoShareLock, __LINE__, ErrorList))
        return;

    /* determine pointer to object frame */
    BD92CHECK_OID_POSITION ("bd90GetObjConsistent", TrError, OId, NptrsPage);
    tbd900_ObjFrame  *pObjFrame = NptrsPage.bd910ObjFramePtr (OId.gg92GetPos());

    /* check the object state  */
    if (pObjFrame->objHeader_bd900.ohdState_gg92 == obsFree_egg00)
    {
        TrError = e_wrong_object_state;
        return;
    }

    /* set parameters needed for the search in the log */
    tgg00_ObjState     ObjState        = pObjFrame->objHeader_bd900.ohdState_gg92;
    tgg00_ObjTransInfo ObjTransInfo    = pObjFrame->objHeader_bd900.ohdTransInfo_gg92;
    tgg00_ObjFrameVers ResultFrameVers = pObjFrame->objHeader_bd900.ohdFrameVersion_gg92; //???NIL_OBJ_FRAME_VERS_GG92;

    /* get object body */
    const bool  bPageIsForVarObj = (pt2VarObject_egg00 == NptrsPage.np_ptr()->nd_pt2());

    if (bPageIsForVarObj)
    {
        FileDir_OmsVar   fd_var_smart;
        fd_var_smart.Assign(fd_smart);

        ObjBodySize = bd90_VarObjBodySize (pObjFrame);
        bd93GetVarObjBody (Current, fd_var_smart, OId, ExtObjBodySize, pExtObjBody, pObjFrame);
    }
    else
    {
        /* If a key is defined the key length and the pointer at */
        /* key within the returned ObjBody are determined        */
        ObjBodySize  = NptrsPage.np_ptr()->nd_obj_frame_len() - sizeof(tgg92_ObjHeader);
        KeyLen = NptrsPage.np_ptr()->nd_obj_key_len();
        if (KeyLen > 0) *pKey = (tsp00_Byte*)pExtObjBody + ObjBodySize - KeyLen;

        /* if the OId is found with a key from the hash (displayed by HashKeyLen>0)  */
        /* the key in the object and the key used for searching in the hash have     */
        /* to be equal                                                               */
        if (HashKeySize > 0)
        {
            BD90_CHECK_KEY_SIZE (TrError, HashKeySize, KeyLen);

            const void *pKeyInObjFrame = pObjFrame->objBody_bd900 + ObjBodySize - KeyLen;

            if (memcmp (pHashKey, pKeyInObjFrame, KeyLen))
            {
              TrError = e_wrong_object_version;
              if ((g01vtrace.vtrBdObject_gg00) || (g01vtrace.vtrOmsGet_gg00 ))
                Kernel_VTrace() << __FILE__ << " : " << __LINE__ 
                << " Error:" << TrError  
                << " OID=" << OId.gg92GetPno() << "." << OId.gg92GetPos()
                << " (vers " << OId.gg92GetFrameVers() << ")"
                << " key doesn't match";  
              return;
            }
        }

        /*  move object and key into output array assigned by pExtObjBody */
        if ( historyPageRef.gg91IsNilRef()) {  // PTS 1124797 FF 2003-10-22
          bd90_GetObjBody (*Current.curr_trans, OId, NptrsPage.np_ptr(),
            ExtObjBodySize, pExtObjBody, pObjFrame);
        } else {
            /* copy key only */
            const tsp00_Uint2   KeyLen      = NptrsPage.np_ptr()->nd_obj_key_len();
            const tsp00_Int4    ObjBodySize = NptrsPage.np_ptr()->nd_obj_frame_len() - sizeof(tgg92_ObjHeader);
            SAPDB_RangeMove (__FILE__, 1,
                PAGE_MXSP00 - sizeof (tgg00_PageCheckTrailer),
                ExtObjBodySize,
                NptrsPage.np_ptr(),
                OId.gg92GetPos() + POS_OFF_DIFF_BD00 + sizeof (tgg92_ObjHeader) + ObjBodySize - KeyLen,
                pExtObjBody, 1 + ObjBodySize - KeyLen,
                KeyLen,
                TrError);
            if (TrError != e_ok) {return;}
        }
    }
    if (TrError != e_ok)  return;

    /*  the current object may be inconsistent with the wanted view        */
    /*  which consists of objects commited before the current              */
    /*  transaction had been started in that case the old object image has */
    /*  to be restored from the log. Now  the  current page is released    */
    /*  since the check if the object is consistent and the search for the */
    /*  old image of the object takes places at the log pages and not here */

    /*  release page pointer  */
    NptrsPage.bd910ReleasePage();  /* is this allowed here ? I think it's wrong ! FF */

    /*  the log pages are searched for the right image    */
    tgg91_PageRef     ObjHistRef;       /* PTS 1120478 FF */

    if ( !historyPageRef.gg91IsNilRef()) {
        kb67GetBeforeImage (*Current.curr_trans, OId, historyPageRef,
            bWithObject, KeyLen, ObjBodySize, ExtObjBodySize, pExtObjBody,
            ResultFrameVers, ResultObjVers, ObjHistRef, UpdTransId, LogReadAccesses,
            ErrorList);   
    }
    else
        kb67GetObjConsistent (*Current.curr_trans, OId, ObjState, ObjTransInfo,
        bWithObject, KeyLen, ObjBodySize, ExtObjBodySize, pExtObjBody,
        ResultFrameVers, ResultObjVers, ObjHistRef, UpdTransId, LogReadAccesses, ErrorList);   
    if (TrError != e_ok)  return;                                            

    if ( !historyPageRef.gg91IsNilRef() &&
        historyPageRef != ObjHistRef ) 
    {
        Kernel_OpError( csp3_bd_msg, csp3_n_obj )
            << "bd90GetObjConsistent: History PageNo in:" <<  historyPageRef.gg91RefPno()
            << " PagePos in::" << historyPageRef.gg91RefPos()
            << " PageNo out:"  << ObjHistRef.gg91RefPno()
            << " PagePos out:" << ObjHistRef.gg91RefPos()
            << " OID=" << OId.gg92GetPno() << "." << OId.gg92GetPos()
            << " (vers " << OId.gg92GetFrameVers() << ")" 
            << " KeyLen:" << KeyLen
            << " ObjBodySize:" << ObjBodySize;
                   
        eo670_CTraceStack();

        ResultObjVers.gg91SetNilRef();
        tgg91_TransNo         DummyUpdTransId;
        bd90GetObjConsistent (Current, OId, ExtObjBodySize, pExtObjBody, HashKeySize,
            pHashKey, ObjBodySize, KeyLen, pKey, ResultObjVers, DummyUpdTransId, LogReadAccesses, ErrorList);
        return;
    }

    if (HashKeySize > 0)
    {
        // set OId version, because it is wrong, if it is coming from the
        // key-management
        OId.gg92SetFrameVers (ResultFrameVers);
    }
    else
    {
        /*  check the object version */
        if (OId.gg92GetFrameVers() != ResultFrameVers)
        {
            TrError = e_wrong_object_version;
            if ((g01vtrace.vtrBdObject_gg00) || (g01vtrace.vtrOmsGet_gg00 ))
              Kernel_VTrace() << __FILE__ << " : " << __LINE__ 
              << " Error:" << TrError  
              << " OID=" << OId.gg92GetPno() << "." << OId.gg92GetPos()
              << " (vers " << OId.gg92GetFrameVers() << ")"
              << " found Vers:" << ResultFrameVers;    
            bd90_NoteVersionErrorInKnldiag(GetCons, OId, ResultFrameVers);  
        }
    }

    /* PTS 1120322 FF 2003-Feb-07*/
    if ((TrError == e_ok) &&
        ((ObjState == obsReserved_egg00) || 
        (ObjState == obsKeyReserved_egg00)) &&
        (LogReadAccesses == 0) &&
        (ObjTransInfo.otrUpdTransId_gg00 != Current.curr_trans->trWriteTransId_gg00))
    {
        if ( 0 == g_Committed_obsReserved_Cnt )                    
        {                                                                       
            g_Committed_obsReserved_Cnt++;
            Kernel_OpError( csp3_bd_msg, csp3_n_obj ) 
            << "Committed object frame without stored data ( state: obsReserved ) found";
        }
        Kernel_OpWarning  opWarnMsg( csp3_bd_msg, csp3_n_obj );   /* PTS 1128934 FF */
        opWarnMsg << "bd90GetObjConsistent: object within state:"; 
        if (ObjState == obsReserved_egg00)
            opWarnMsg << "obsReserved_egg00"; 
        else
            opWarnMsg << "obsKeyReserved_egg00"; 
        opWarnMsg << " read, but not created by this trans!"
            << " OID=" << OId.gg92GetPno() << "." << OId.gg92GetPos()
            << " (vers " << OId.gg92GetFrameVers() << ")"
            << " (page dumped to file d" 
            << OId.gg92GetPno() <<".obj)"; 
        cbd910_PrimObjDataNode   NptrsPage (Current, OId.gg92GetPno(), nr_for_read);
        if( TrError != e_ok) return;
        b06dump_bad_page (Current.curr_trans->trTaskId_gg00,'d', ".obj", 
            NptrsPage.np_ptr()->nd_id(), (void *) NptrsPage.np_ptr(), 1);
        TrError = e_too_old_oid;
    }
}

/*---------------------------------------------------------------------------*/
externCpp void
bd90GetObjConsistentWithLock (cbd05_Current        &Current,            //[inp\out] 
                              tgg92_KernelOid      &OId,                //[inp\out] 
                              tsp00_Int4            ExtObjBodySize,     //[inp= buffersize for object]
                              void                 *pExtObjBody,        //[inp]
                              tsp00_Int4            HashKeySize,        //[inp]
                              void                 *pHashKey,           //[inp]  
                              const bool            bShareLock,         //[inp]   
                              tsp00_Int4           &ObjBodySize,        //[out=length of the object] 
                              tsp00_Int4           &KeyLen,             //[out]
                              tsp00_Byte*          *pKey,               //[out]
                              tgg91_PageRef        &ResultObjVers,      //[out]
                              tgg91_TransNo        &UpdTransId,         //[out] 
                              tsp00_Int4           &LogReadAccesses,    //[out] 
                              Msg_List             &ErrorList)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd90GetObjConsistentWithLock", ObjContainer_Trace, 5);

    tgg00_BasisError   &TrError         = Current.curr_trans->trError_gg00;
    tgg00_TransContext &Trans           = *Current.curr_trans;

    const bool          bDontGetObject  = false;
    tsp00_Bool          bConsistent;
    tsp00_Bool          bCancelled;                         /* PTS 1120151 FF 2003-JAN-29 */

    /* PTS 1107819 AK 12/09/2000; note: only in case of the error=e_object_dirty        */
    /* a value for LogReadAccesses will be returned which is not zero will be returned  */
    LogReadAccesses                     = 0;

    /* PTS 1118931 FF 14-Nov-2002 */
    if (NIL_PAGE_NO_GG00 == OId.gg92GetPno()) {
        TrError = e_invalid_oid;
        return;
    }

    /* get page pointer */
    cbd910_PrimObjDataNode   NptrsPage (Current, OId.gg92GetPno(), nr_for_update);
    if (e_ok != TrError) return;

    // PTS 1119647 FF 2002-DEC-05
    FileDir_FileNo FileNo;
    bd900GetFileNo(NptrsPage.np_ptr(), FileNo);
    FileDir_Oms    fd_smart;
    if ( !bd90GetFile( Trans, fd_smart,  FileNo, cDoShareLock, __LINE__, ErrorList))
        return;

    if ( ! ResultObjVers.gg91IsNilRef()) {  // PTS 1124797 FF 2003-10-22
      TrError = e_object_dirty;             // you can't lock history !
      return;
    }

    /* determine pointer to object frame */
    BD92CHECK_OID_POSITION ("bd90GetObjConsistentWithLock", TrError, OId, NptrsPage);
    tbd900_ObjFrame *const pObjFrame = NptrsPage.bd910ObjFramePtr (OId.gg92GetPos());

    /* check the object state  */
    switch (pObjFrame->objHeader_bd900.ohdState_gg92)
    {
    case obsFree_egg00 :
        TrError = e_wrong_object_state;
        return;
    case obsFreeAfterEot_egg00 :
        tgg00_ObjFrameVers  ResultFrameVers = NIL_OBJ_FRAME_VERS_GG92;
        tgg91_PageRef       ObjHistRef;       // unused parameter PTS 1120478 FF
        kb67GetObjConsistent (Trans, OId, pObjFrame->objHeader_bd900.ohdState_gg92,
            pObjFrame->objHeader_bd900.ohdTransInfo_gg92, bDontGetObject, KeyLen, ObjBodySize,
            ExtObjBodySize, pExtObjBody, ResultFrameVers, ResultObjVers, ObjHistRef, UpdTransId,
            LogReadAccesses, ErrorList);  
        if (e_ok == TrError) 
            TrError = e_object_dirty;
        return;
    }

    /* check object frame version */
    const tgg00_ObjFrameVers & CurrFrameVers = pObjFrame->objHeader_bd900.ohdFrameVersion_gg92;
    ObjBodySize = NptrsPage.np_ptr()->nd_obj_frame_len() - sizeof(tgg92_ObjHeader);
    KeyLen      = NptrsPage.np_ptr()->nd_obj_key_len();
    if (KeyLen > 0) *pKey = (tsp00_Byte*)pExtObjBody + ObjBodySize - KeyLen;

    if (HashKeySize > 0)
    {
        /* if the OId is found with a key from the hash (displayed by HashKeyLen>0)  */
        /* the key in the object and the key used for searching in the hash have     */
        /* to be equal                                                               */

        BD90_CHECK_KEY_SIZE (TrError, HashKeySize, KeyLen);
        const void *pKeyInObjFrame = pObjFrame->objBody_bd900 + ObjBodySize - KeyLen;

        if (memcmp (pHashKey, pKeyInObjFrame, KeyLen))
        {
          TrError = e_wrong_object_version;
          if ((g01vtrace.vtrBdObject_gg00) || (g01vtrace.vtrOmsGet_gg00 ))
            Kernel_VTrace() << __FILE__ << " : " << __LINE__ 
            << " Error:" << TrError 
            << " OID=" << OId.gg92GetPno() << "." << OId.gg92GetPos()
            << " (vers " << OId.gg92GetFrameVers() << ")"
            << " key doesn't match";    
          return;
        }
        OId.gg92SetFrameVers (CurrFrameVers);
    }
    else
    {
        if (CurrFrameVers != OId.gg92GetFrameVers())
        {
            TrError = e_wrong_object_version;
            if ((g01vtrace.vtrBdObject_gg00) || (g01vtrace.vtrOmsGet_gg00 ))
              Kernel_VTrace() << __FILE__ << " : " << __LINE__ 
              << " Error:" << TrError  
              << " OID=" << OId.gg92GetPno() << "." << OId.gg92GetPos()
              << " (vers " << OId.gg92GetFrameVers() << ")"
              << " found Vers:" << CurrFrameVers;    
            return;
        }
    }

    /* check if the current object version is consistent with this view*/
    tgg00_ObjTransInfo & ObjTransInfo = pObjFrame->objHeader_bd900.ohdTransInfo_gg92;
    kb50ObjConsistentCheck (Trans, ObjTransInfo.otrUpdTransId_gg00, bConsistent,
        bCancelled);
    //                      without otrBeforeRef:  PTS 1105492 JA 2000-03-15
    if (bCancelled) /* PTS 1120151 FF 2003-JAN-29 */
    {
        Kernel_OpWarning  opWarnMsg( csp3_bd_msg, csp3_n_obj );
        opWarnMsg << "cancelled ConsistView:";
        for (int i=0; i < sizeof(Trans.trConsistView_gg00); i++)
            opWarnMsg << ToStr((SAPDB_UInt1)Trans.trConsistView_gg00[i], 
            2, _T_h);
        opWarnMsg << " found";
        TrError = e_consview_cancelled;
    }
    if (e_ok != TrError) return;

    /* return if the consistent version does not correspond to the current version */
    if (!bConsistent)
    {
         TrError = e_object_dirty;
        return;
    }

    /* lock object */
		ResultObjVers = ObjTransInfo.otrBeforeRef_gg00;
		if ( bShareLock )
			kb67LockShareObj (Trans, Current.curr_tree_id, OId, ResultObjVers,
			pObjFrame->objHeader_bd900.ohdState_gg92, pObjFrame->objHeader_bd900.ohdTransInfo_gg92);
		else
			kb67LockObj (Trans, Current.curr_tree_id, OId, ResultObjVers,
			pObjFrame->objHeader_bd900.ohdState_gg92, pObjFrame->objHeader_bd900.ohdTransInfo_gg92);
    if (e_ok != TrError) return;

    UpdTransId = pObjFrame->objHeader_bd900.ohdTransInfo_gg92.otrUpdTransId_gg00;

    /* get object body */
    if (pt2VarObject_egg00 == NptrsPage.np_ptr()->nd_pt2())
    {
        FileDir_OmsVar   fd_var_smart;
        fd_var_smart.Assign(fd_smart);
        ObjBodySize = bd90_VarObjBodySize (pObjFrame);
        bd93GetVarObjBody (Current, fd_var_smart, OId, ExtObjBodySize, pExtObjBody, pObjFrame);
    }
    else
    {
        ObjBodySize  = NptrsPage.np_ptr()->nd_obj_frame_len() - sizeof(tgg92_ObjHeader);
        bd90_GetObjBody (Trans, OId, NptrsPage.np_ptr(),ExtObjBodySize, pExtObjBody, pObjFrame);
    }

    /* PTS 1120322 FF 2003-Feb-07*/
    if ((e_ok == TrError) &&
        ((pObjFrame->objHeader_bd900.ohdState_gg92 == obsReserved_egg00) || 
        (pObjFrame->objHeader_bd900.ohdState_gg92 == obsKeyReserved_egg00)) &&
        (LogReadAccesses == 0) &&
        (pObjFrame->objHeader_bd900.ohdTransInfo_gg92.otrUpdTransId_gg00 != 
        Trans.trWriteTransId_gg00))
    {
        if ( 0 == g_Committed_obsReserved_Cnt )                    
        {                                                                       
            g_Committed_obsReserved_Cnt++;
            Kernel_OpError( csp3_bd_msg, csp3_n_obj ) 
            << "Committed object frame without stored data ( state: obsReserved ) found";
        }
        Kernel_OpWarning  opWarnMsg( csp3_bd_msg, csp3_n_obj );   /* PTS 1128934 FF */
        opWarnMsg << "bd90GetObjConsistent: object within state:"; 
        if (pObjFrame->objHeader_bd900.ohdState_gg92 == obsReserved_egg00)
            opWarnMsg << "obsReserved_egg00"; 
        else
            opWarnMsg << "obsKeyReserved_egg00"; 
        opWarnMsg << " read, but not created by this trans!" 
            << " OID=" << OId.gg92GetPno() << "." << OId.gg92GetPos()
            << " (vers " << OId.gg92GetFrameVers() << ")"
            << " (page dumped to file d" 
            << OId.gg92GetPno() <<".obj)"; 
        b06dump_bad_page (Trans.trTaskId_gg00,'d', ".obj", 
            NptrsPage.np_ptr()->nd_id(), (void *) NptrsPage.np_ptr(), 1);
        TrError = e_too_old_oid;
    }
}

/* ----------------------------------------------------------------------------------*/

externCpp void
bd90GetObjFileStatistics (cbd05_Current      &Current,
                          tsp00_Int4         &NumAllPages,
                          tsp00_Int4         &NumFreePages,
                          tsp00_Int4         &NumEmptyPages,
                          tsp00_Int4         &NumKeyLeaves,        /* PTS 1107153 AK 17/07/2000 */
                          tsp00_Int4         &Occupancy)           /* PTS 1107153 AK 17/07/2000 */
{
    SAPDBTRACE_ROUTINE_DEBUG("bd90GetObjFileStatistics", ObjContainer_Trace, 5);

    /* initialize counters */
    NumAllPages   = 0;
    NumFreePages  = 0;
    NumEmptyPages = 0;
    NumKeyLeaves  = 0;

    Msg_List                errorList;
    tgg00_TransContext     &Trans     = *Current.curr_trans;

    const FileDir_FileNo   ObjFileNo = Current.curr_tree_id.fileDirFileId_gg00(); 
    FileDir_Oms        fd_smart;
    FileDir_ReturnCode     fd_rc;
    tsp00_Bool             bConsistent  = false;
    fd_rc = FileDir_IDirectory::SessionInstance(Trans).GcGetFile(Trans.trTaskId_gg00, ObjFileNo, fd_smart, errorList);
    if (FileDir_Okay != fd_rc) {
        Current.curr_trans->trError_gg00 = e_wrong_class_id;
        return;
    }

    cbd911RecreateIndexLock lock(Trans);  
    if (lock.IsNeeded(ObjFileNo) && (fd_smart.GetKeyPartitionCount() > 0 ))
        lock.Lock( !bExclusive );

    NumAllPages  = fd_smart.GetLeafPageCount(Trans.trTaskId_gg00);
    NumFreePages = fd_smart.GetFreePageCount(Trans.trTaskId_gg00);

    /* get number of leaves from the key tree  PTS 1107153 AK 17/07/2000 */
    if (Data_OmsKeyFile == fd_smart.GetFileType())
    {
        bd22GetLeaveCountOfObjKeyFiles (*Current.curr_trans, ObjFileNo, fd_smart, NumKeyLeaves, errorList);
    }

    /* determines the average occupancy rate (in %) of the pages of this file */
    const SAPDB_UInt8 cHundertPercent = 100;
    SAPDB_UInt8       NumOccPages;

    if ( fd_smart.GetObjPerPageCount() )
        NumOccPages = ( fd_smart.GetEntryCount(Current.curr_trans->trTaskId_gg00) * cHundertPercent ) 
        / fd_smart.GetObjPerPageCount(); 
    else
        NumOccPages = fd_smart.GetEntryCount(Current.curr_trans->trTaskId_gg00);

    SAPDB_UInt8    NumAllPages2 =  fd_smart.GetLeafPageCount(Current.curr_trans->trTaskId_gg00);

    if (NumAllPages > 0)
    {
        Occupancy = STATIC_CAST(tsp00_Int4,(NumOccPages/NumAllPages)); // PTS 1114534 FF 2002-02-28
        if ( 100 < Occupancy )    // Occupancy is based on 'dirty' counted ociAllObjCnt_bd900
            Occupancy = 100;        // PTS 1114534 FF 2002-02-28
    }
    else
        Occupancy = 0;

    /* check if the numbers of pages from the objfiledir and the subroots are equal */
#   if COMPILEMODE_MEO00 >= QUICK_MEO00 
    if (NumAllPages != NumAllPages2)
    {
        Kernel_OpWarning( csp3_bd_msg, csp3_n_obj )
            << "bd90GetObjFileStatistics Fileno:" << ObjFileNo
            << " filedir page count:" << NumAllPages
            << " subroot page count:" << NumAllPages2; 
    }
#   endif
}

/*----------------------------------------------------------------------------*/
/* CRS 1106284 AK 11-03-2000 */

externCpp void
bd90IsObjHistoryUsed (cbd05_Current     &Current,
                      tgg92_KernelOid   &OId,
                      bool               ignoreVersions,	/* PTS 1130354 */
                      bool              &bHistoryIsUsed,
                      SAPDB_UInt8        &OldestTransNo,    /* PTS 1130354 */
                      Msg_List           &ErrorList)
{
	SAPDBTRACE_ROUTINE_DEBUG("bd90IsObjHistoryUsed", ObjContainer_Trace, 5);

    tgg00_BasisError  &TrError = Current.curr_trans->trError_gg00;

    /*  get page pointer  */
    cbd910_PrimObjDataNode  NptrsPage (Current, OId.gg92GetPno(),  nr_for_read);
    if (TrError != e_ok) return;

    /* determine pointer to object frame */
    BD92CHECK_OID_POSITION  ("bd90IsObjSelfLocked",TrError, OId, NptrsPage);
    tbd900_ObjFrame  *pObjFrame = NptrsPage.bd910ObjFramePtr (OId.gg92GetPos());

    FileDir_FileNo FileNo;
    bd900GetFileNo(NptrsPage.np_ptr(), FileNo);
    FileDir_Oms     fd_smart;
    if ( !bd90GetFile( *Current.curr_trans, fd_smart,  FileNo, cDoShareLock, __LINE__, ErrorList))
        return;

    /* begin * PTS 1125361 FF 2003-Nov-17 
    if (pObjFrame->objHeader_bd900.ohdFrameVersion_gg92 != OId.gg92GetFrameVers())
    {
        TrError = e_wrong_object_version;
        bd90_NoteVersionErrorInKnldiag
        (Lock, OId, pObjFrame->objHeader_bd900.ohdFrameVersion_gg92);
        return;
    } 
     * end PTS 1125361 ***/

    /* check object state */
		switch (pObjFrame->objHeader_bd900.ohdState_gg92)
		{
		case obsOccupied_egg00 :
		case obsFreeAfterEot_egg00 :
			break;
		case obsReserved_egg00 :
		case obsKeyReserved_egg00 :
			bHistoryIsUsed = false;
			OldestTransNo  = 0;
			return; 
			break;
		case obsFree_egg00 :
		default :
			TrError = e_wrong_object_state;
			break;
		}

    bHistoryIsUsed =
        kb67IsObjHistoryUsed (*Current.curr_trans, OId
				, pObjFrame->objHeader_bd900.ohdTransInfo_gg92, ignoreVersions, OldestTransNo);
}

/*----------------------------------------------------------------------------*/

externCpp void
bd90IsObjSelfLocked (cbd05_Current&    Current,
                     tgg92_KernelOid&  OId,
					 bool              bShared,
                     bool&             IsSelfLocked,
                     Msg_List&         ErrorList)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd90IsObjSelfLocked", ObjContainer_Trace, 5);

    tgg00_BasisError  &TrError = Current.curr_trans->trError_gg00;

    IsSelfLocked = false;

    /*  get page pointer  */
    cbd910_PrimObjDataNode  NptrsPage (Current, OId.gg92GetPno(),  nr_for_read);
    if (e_ok != TrError) return;

    FileDir_FileNo FileNo;
    bd900GetFileNo(NptrsPage.np_ptr(), FileNo);
    FileDir_Oms     fd_smart;
    if ( !bd90GetFile( *Current.curr_trans, fd_smart,  FileNo, cDoShareLock, __LINE__, ErrorList))
        return;

    /* determine pointer to object frame */
    BD92CHECK_OID_POSITION  ("bd90IsObjSelfLocked",TrError, OId, NptrsPage);
    tbd900_ObjFrame  *pObjFrame = NptrsPage.bd910ObjFramePtr (OId.gg92GetPos());

    /*  check the object version */
    if (pObjFrame->objHeader_bd900.ohdFrameVersion_gg92 != OId.gg92GetFrameVers())
    {
        TrError = e_wrong_object_version;
        bd90_NoteVersionErrorInKnldiag
        (Lock, OId, pObjFrame->objHeader_bd900.ohdFrameVersion_gg92);
        return;
    }

    /* check object state */
    if (pObjFrame->objHeader_bd900.ohdState_gg92 == obsFree_egg00)
    {
        TrError = e_wrong_object_state;
        return;
    }

	kb50CheckObjSelfLocked (*Current.curr_trans, OId, pObjFrame->objHeader_bd900.ohdTransInfo_gg92, (bShared ? 1 : 0));
    if (e_ok == TrError)
        IsSelfLocked = true;
    else
        if (e_object_not_locked == TrError)
            TrError = e_ok;
}

/*---------------------------------------------------------------------------*/

externCpp void
bd90LockObj (cbd05_Current&      Current,
             tgg92_KernelOid&    OId,
             tgg91_PageRef&      WantedObjVers,    
             Msg_List&           ErrorList)
{
    // NOTE! if this function returns e_wait_for_lock_release then wait outside bd-locking

    SAPDBTRACE_ROUTINE_DEBUG("bd90LockObj", ObjContainer_Trace, 5);

    tgg00_BasisError    &TrError = Current.curr_trans->trError_gg00;
    tgg00_TransContext  &Trans   = *Current.curr_trans;

    cbd910_PrimObjDataNode    NptrsPage(Current, OId.gg92GetPno(),  nr_for_update);
    if (e_ok != TrError) return;

    // PTS 1119647 FF 2002-DEC-05
    FileDir_FileNo FileNo;
    bd900GetFileNo(NptrsPage.np_ptr(), FileNo);
    FileDir_Oms     fd_smart;
    if ( !bd90GetFile( Trans, fd_smart,  FileNo, cDoShareLock, __LINE__, ErrorList))
        return;

    /* determine pointer to object frame */
    BD92CHECK_OID_POSITION ("bd90LockObj",TrError, OId, NptrsPage);
    tbd900_ObjFrame  *pObjFrame = NptrsPage.bd910ObjFramePtr (OId.gg92GetPos());

    /*  check object version  */
    if (pObjFrame->objHeader_bd900.ohdFrameVersion_gg92 != OId.gg92GetFrameVers())
    {
        TrError = e_wrong_object_version;
    }
    else
    {
        switch (pObjFrame->objHeader_bd900.ohdState_gg92)
        {
        case obsFree_egg00 :
            TrError = e_wrong_object_state;
            break;
        /* k67LockObj MUST be called even for state obsFreeAfterEot_egg00
           reason: 1. call encounters obsOccupied but locked
                   2. transaction will wait for lock ( if timeout is set )
                   3. frame is deleted by other transaction
                   4. wait is resolved and lock is "partially" assigned
                   This "partially" assignment should be reverted by kb67LockObj
            case obsFreeAfterEot_egg00 :  
            TrError = e_object_dirty;
            break; */
        default :
            kb67LockObj (*Current.curr_trans, Current.curr_tree_id, OId, WantedObjVers,
                pObjFrame->objHeader_bd900.ohdState_gg92,
                pObjFrame->objHeader_bd900.ohdTransInfo_gg92);
        }
    }
}

/*---------------------------------------------------------------------------*/

externCpp void
bd90LockUpdObj (cbd05_Current&        Current,
                tgg92_KernelOid&      OId,
                tgg91_PageRef&        WantedObjVers,    
                Msg_List&             ErrorList)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd90LockUpdObj", ObjContainer_Trace, 5);

    tgg00_BasisError    &TrError = Current.curr_trans->trError_gg00;
    tgg00_TransContext  &Trans   = *Current.curr_trans;

    cbd910_PrimObjDataNode    NptrsPage(Current, OId.gg92GetPno(),  nr_for_update);
    if (TrError != e_ok) return;

    // PTS 1119647 FF 2002-DEC-05
    FileDir_FileNo FileNo;
    bd900GetFileNo(NptrsPage.np_ptr(), FileNo);
    FileDir_Oms      fd_smart;
    if ( !bd90GetFile( Trans, fd_smart,  FileNo, !cDoShareLock, __LINE__, ErrorList))
        return;

    /* determine pointer to object frame */
    BD92CHECK_OID_POSITION ("bd90LockUpdObj",TrError, OId, NptrsPage);
    tbd900_ObjFrame  *pObjFrame = NptrsPage.bd910ObjFramePtr (OId.gg92GetPos());

    /*  check object version  */
    if (pObjFrame->objHeader_bd900.ohdFrameVersion_gg92 != OId.gg92GetFrameVers())
    {
        TrError = e_wrong_object_version; 
    }
    else
    {
        switch (pObjFrame->objHeader_bd900.ohdState_gg92)
        {
        case obsFree_egg00 :
            TrError = e_wrong_object_state;
            break;
        case obsFreeAfterEot_egg00 :
            TrError = e_object_dirty;
            break;
        default :
            kb67LockUpdateObj (*Current.curr_trans, Current.curr_tree_id, OId, WantedObjVers,
                               pObjFrame->objHeader_bd900,
                               pObjFrame->objHeader_bd900.ohdTransInfo_gg92,
                               ErrorList);
        }
    }
}

/*---------------------------------------------------------------------------*/

externCpp void
bd90LockShareObj (cbd05_Current&      Current,
                  tgg92_KernelOid&    OId,
                  tgg91_PageRef&      WantedObjVers,   
                  Msg_List&           ErrorList)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd90LockShareObj", ObjContainer_Trace, 5);

    tgg00_BasisError    &TrError = Current.curr_trans->trError_gg00;
    tgg00_TransContext  &Trans   = *Current.curr_trans;

    /* share lock needs WRITE ACCESS to page, because header may be changed    */
    /* and checking of frame-header is synchronized by using exclusive access  */
    cbd910_PrimObjDataNode    NptrsPage(Current, OId.gg92GetPno(),  nr_for_update);
    if (e_ok != TrError) return;

    // PTS 1119647 FF 2002-DEC-05
    FileDir_FileNo FileNo;
    bd900GetFileNo(NptrsPage.np_ptr(), FileNo);
    FileDir_Oms     fd_smart;
    if ( !bd90GetFile( Trans, fd_smart,  FileNo, cDoShareLock, __LINE__, ErrorList))
        return;

    /* determine pointer to object frame */
    BD92CHECK_OID_POSITION ("bd90LockObj",TrError, OId, NptrsPage);
    tbd900_ObjFrame  *pObjFrame = NptrsPage.bd910ObjFramePtr (OId.gg92GetPos());

    /*  check object version  */
    if (pObjFrame->objHeader_bd900.ohdFrameVersion_gg92 != OId.gg92GetFrameVers())
    {
        TrError = e_wrong_object_version;
    }
    else
    {
        switch (pObjFrame->objHeader_bd900.ohdState_gg92)
        {
        case obsFree_egg00 :
            TrError = e_wrong_object_state;
            break;
            /* k67LockShareObj MUST be called even for state obsFreeAfterEot_egg00
            reason: 
            1. call encounters obsOccupied but locked
            2. transaction will wait for lock ( if timeout is set )
            3. frame is deleted by other transaction
            4. wait is resolved and lock is "partially" assigned
            This "partially" assignment should be reverted by kb67LockObj
            case obsFreeAfterEot_egg00 :  
            TrError = e_object_dirty;
            break; */
        case obsFreeAfterEot_egg00 :
        case obsReserved_egg00:
        case obsKeyReserved_egg00:
        case obsOccupied_egg00 :
            kb67LockShareObj (*Current.curr_trans, Current.curr_tree_id, OId, WantedObjVers,
                pObjFrame->objHeader_bd900.ohdState_gg92,
                pObjFrame->objHeader_bd900.ohdTransInfo_gg92);
            break;
        default :
            TrError = e_lock_collision;
        }
    }
}

/*---------------------------------------------------------------------------*/

externCpp void
bd90NewObj (cbd05_Current           &Current,                  // [in\out]
            tsp00_Int4               KeyLen,                   // [in]
            void                    *pKey,                     // [in]
            SAPDB_Int1               contObj,
            tgg92_KernelOid         &ResultOId,                // [out]
            tgg91_PageRef           &ResultObjVers,            // [out]
            Msg_List                &ErrorList)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd90NewObj", ObjContainer_Trace, 5);

    /*  This function yields a new OId. First it tries to get    */
    /*  the new OId from the free chain. If it does not find     */
    /*  a new OId in this chain a new page is included into the  */
    /*  object file and an OId from this page is served          */

    tgg00_BasisError      &TrError     = Current.curr_trans->trError_gg00;
    tgg00_TransContext    &Trans       = *Current.curr_trans;
    const FileDir_FileNo   FileNo      = Current.curr_tree_id.fileDirFileId_gg00();

    FileDir_Oms     fd_smart;
    if ( !bd90GetFile( Trans, fd_smart,  FileNo, cDoShareLock, __LINE__, ErrorList))
        return;

    CFreePageListEntry&   FreePageCacheListEntry = bd90_GetCachedFreePage(Trans, FileNo);
	tsp00_PageNo          CacheFreePage = FreePageCacheListEntry.PageNo();
	if (!FreePageCacheListEntry.PageReleased() && (NIL_PAGE_NO_GG00 != CacheFreePage))
	{
        cbd910_PrimObjDataNode   NptrsPage   (Current, CacheFreePage, nr_for_update);
        if (e_ok != TrError)
        {
            Kernel_OpError ( csp3_bd_msg, csp3_n_obj )
                << "bd90NewObj:cached freepage access error" << TrError
                << " PageNo:" << CacheFreePage;

            FreePageCacheListEntry.PageNo() = NIL_PAGE_NO_GG00;
            FreePageCacheListEntry.PageReleased() = false;
            return;
        }

		Converter_Version converterVersion = 
			bd20GetDestinationConverterVersion ( Trans.trTaskId_gg00, NptrsPage.np_cbptr() );
		fd_smart.ModifyEntryCount(Trans.trTaskId_gg00, converterVersion, 1);
	    bd90_NewObjFromThisPage (Current, NptrsPage, KeyLen, pKey, contObj, ResultOId, ResultObjVers, ErrorList);
        if (e_ok != TrError)
        {
            Kernel_OpError ( csp3_bd_msg, csp3_n_obj )
                << "bd90NewObj:cached freepage access error" << TrError
                << " PageNo:" << CacheFreePage
                << " no frame available";
            FreePageCacheListEntry.PageNo()       = NIL_PAGE_NO_GG00;
            FreePageCacheListEntry.PageReleased() = false;
            return;
        }
		if (NptrsPage.np_ptr()->nd_first_free_obj() == NIL_OBJ_PAGE_POS_GG92)
		{
			NptrsPage.np_ptr()->nd_objpage_state().delElement(objp_no_chain_to_freelist); /* re-insert allowed */
			NptrsPage.bd910ReleasePage();
			FreePageCacheListEntry.PageReleased() = true;
		}
	}
	else
	{
		cbd910_PrimObjDataNode   NptrsPage(Current, nr_for_update);

		const tsp00_PageNo       SubRoot          = fd_smart.GetRootPage();
		cbd910_SubRootNode       NptrsSubRoot(Current, SubRoot, nr_for_update);
		if (e_ok != TrError) {
			Kernel_OpError( csp3_bd_msg, csp3_n_obj ) 
				<< __FILE__ << ":" << __LINE__ 
				<< " FileId:" 
				<< FileNo
				<< " Root:" << SubRoot << " Error:" << TrError;
			return;
		}

		Converter_Version converterVersion = 
			bd20GetDestinationConverterVersion ( Trans.trTaskId_gg00, NptrsSubRoot.np_cbptr() );

		/* check if this subroot page belongs really to this file */
		if (NptrsSubRoot.np_ptr()->nd_root() != fd_smart.GetRootPage())   
		{
			tbd_nodeptr &pPage = NptrsSubRoot.np_ptr();
			Kernel_OpError( csp3_bd_msg, csp3_n_obj )
				<< __FILE__ << ":" << __LINE__  
				<< " Root page in file directory : " << fd_smart.GetRootPage()
				<< " root page on page:" << pPage->nd_root()
				<< " PNo:" <<  NptrsSubRoot.np_ptr()->nd_id() 
				<< " (page dumped to file:" << pPage->nd_id() << "d)";
			b06dump_bad_page (Trans.trTaskId_gg00,'d', ".obj", pPage->nd_id(), (void *) pPage, 1); 
			g01abort (csp3_bd_msg, csp3_n_obj, "BD90_NewObj:wrong SRoot ", pPage->nd_id());
		}

		bool bRootPageReleased = false;
		bool noChainToFreelist = true;  

		if (NptrsSubRoot.np_ptr()->nd_free_obj_cnt() > 0)
			NptrsPage.bd910SetTo (NptrsSubRoot);
		else
		{
			bool bNewAllocatedPage = false;
			if (NIL_PAGE_NO_GG00 != NptrsSubRoot.np_ptr()->nd_next_free())
			{
				NptrsPage.bd910AssignToPage(NptrsSubRoot.np_ptr()->nd_next_free());
			}
			else
			{
				bNewAllocatedPage = true;
				NptrsPage.bd910AssignToNewPageInDataChain (NptrsSubRoot, noChainToFreelist, 
					Trans.trTaskId_gg00, fd_smart, converterVersion);
			}
			if (e_ok != TrError) return;

            if ( !isOnline )
                isOnline = Kernel_IAdminInfo::Instance().KernelStateIsOnline(); 

			/* PTS 1130940 FF check if chaining will be needed anymore >>= else release subroot */
            if (isOnline && 
                (bNewAllocatedPage || (NptrsPage.np_ptr()->nd_free_obj_cnt() > 2)))
            {
                NptrsPage.np_ptr()->nd_objpage_state().addElement(objp_no_chain_to_freelist);; /* no re-insert into freechain */
                if (FreePageCacheListEntry.PageReleased() && (NptrsPage.np_ptr()->nd_free_obj_cnt() > 1))
                {
                    FreePageCacheListEntry.PageNo() = NptrsPage.np_ptr()->nd_id();
                    FreePageCacheListEntry.PageReleased() = false;
                }
                else
                {
                    if (NptrsPage.np_ptr()->nd_free_obj_cnt() == 1)
                    {
                        NptrsPage.np_ptr()->nd_objpage_state().delElement(objp_no_chain_to_freelist);
                    }
                    else if (!bd90_PutCachedFreePage(Trans, FileNo, NptrsPage.np_ptr()->nd_id()))
                    {
                        NptrsPage.np_ptr()->nd_objpage_state().delElement(objp_no_chain_to_freelist);
                        if (NptrsPage.bd910PageMustBeInsertedIntoFreeChain())
                            NptrsSubRoot.bd910InsertPageIntoFreeChain (NptrsPage, converterVersion, fd_smart);   
                        bNewAllocatedPage = false;
                    }
                }
                if (bNewAllocatedPage)
                {
                    NptrsSubRoot.bd910ReleasePage();
                    bRootPageReleased = true;
                }
            }
        }

		/* get a new OId and initialize the corresponding object */
		bd90_NewObjFromThisPage (Current, NptrsPage, KeyLen, pKey, contObj, ResultOId, ResultObjVers, ErrorList);
		if (e_ok != TrError) 
			return;

		if (!bRootPageReleased) 
		{
			if (NptrsPage.np_ptr()->nd_in_free_chain() && NptrsPage.np_ptr()->nd_objpage_state().includes(objp_no_chain_to_freelist))
				NptrsSubRoot.bd910RemovePageFromFreeChain(NptrsPage, converterVersion, fd_smart, true /*NoReinsert*/);

			/* remove page from free page chain if there are no more free object frame on the page */
			NptrsSubRoot.bd910RestoreFreePageChain (NptrsPage, converterVersion, fd_smart ); 
			NptrsSubRoot.bd910ReleasePage();
		}

		fd_smart.ModifyEntryCount(Trans.trTaskId_gg00, converterVersion, 1);
	}
}

/*---------------------------------------------------------------------------*/

externCpp void 
bd90NextObjConsistent (cbd05_Current        &Current,
                       tgg92_KernelOid      &OId,
                       bool                  bWithObject,
                       tsp00_Int4            ExtObjBodySize,
                       void                 *pExtObjBody, 
                       tsp00_Int4           &ObjBodySize,   // [out=length of the object] 
                       tgg91_PageRef        &ObjVers,
                       tsp00_Int4           &AllLogReadAccesses,  
                       Msg_List             &ErrorList)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd90NextObjConsistent", ObjContainer_Trace, 5);

    tgg00_BasisError      &TrError         = Current.curr_trans->trError_gg00;
    tgg00_TransContext    &Trans           = *Current.curr_trans;
    tsp00_Bool             bNextObjIsFound = false;
    cbd910_PrimObjDataNode NptrsPage(Current, nr_for_read);
    tsp00_Int4             LogReadAccessesPerPage;   

    /*---------------------------------*/
    /*  find starting page for search  */
    /*---------------------------------*/

    FileDir_Oms    fd_smart;

    if (!OId.gg92IsNil() )
    {
        /* if OID is not nil look for object following the given OID  */
        NptrsPage.bd910AssignToPage(OId.gg92GetPno());
        if (e_ok != TrError) return;

        FileDir_FileNo FileNo;
        bd900GetFileNo(NptrsPage.np_ptr(), FileNo);
        if ( !bd90GetFile( Trans, fd_smart,  FileNo, !cDoShareLock, 
            __LINE__, ErrorList))
            return;
        
        /* check fileno and state of input object */
        BD92CHECK_OID_POSITION ("bd90NextObjConsistent",TrError, OId, NptrsPage);
        tbd900_ObjFrame *pObjFrame = NptrsPage.bd910ObjFramePtr (OId.gg92GetPos());

        if (pObjFrame->objHeader_bd900.ohdState_gg92 == obsFree_egg00)
        {
            Kernel_OpWarning  opErrMsg( csp3_bd_msg, csp3_n_obj );
            opErrMsg << "bd90NextObjConsistent:Invalid iterator!"
            << " State:obsFree"
            << " OID=" << OId.gg92GetPno() << "." << OId.gg92GetPos()
            << " (vers " << OId.gg92GetFrameVers() << ")";

            /* note: The previous check is very simple in principle one              */
            /*       should even check if the input OId belongs to an object which   */
            /*       owns a consistent image with respect to the current transaction */

            TrError = e_wrong_object_state;
            return;
        }
    }
    else
    {
        /*  look for very first object in file, which  */
        /*  should be on the  subroot                  */
        const FileDir_FileNo  FileNo(Current.curr_tree_id.fileDirFileId_gg00()); //PTS 1111901 AK 24/09/2001
        if ( !bd90GetFile( Trans, fd_smart,  FileNo, cDoShareLock, __LINE__, ErrorList))
            return;

        const tsp00_PageNo     SubRoot = fd_smart.GetRootPage();

        NptrsPage.bd910AssignToPage(SubRoot);
        if (e_ok != TrError) return;
    }

    /*-------------------------------------------------*/
    /*  start looping over chains and pages of chains  */
    /*-------------------------------------------------*/

    /* loop over  object page */
    do
    {
		const bool bOnlyHdrVarObjects = true;
        tgg91_PageRef          ObjHistRef;              // unused parameter PTS 1120478 FF
        /*  look for the next object on current page  */
        bd90NextObjConsistentOnThisPage (Current, fd_smart, OId, ObjVers, ObjHistRef, bWithObject, !bOnlyHdrVarObjects, 
										  ExtObjBodySize, pExtObjBody, NptrsPage, ObjBodySize, bNextObjIsFound, 
										  LogReadAccessesPerPage, ErrorList);

        /* accumulalte the number of read accesses to object history pages; PTS 1107819 AK 12/09/2000 */
        AllLogReadAccesses += LogReadAccessesPerPage;

        /* return immediately if the next object is found */
        if (bNextObjIsFound || TrError != e_ok) return;

        /* check if the command has been canceled. PTS 1105431 Alexander Kley 25/01/2000 */
        if (Current.curr_trans->trRteCommPtr_gg00->to_cancel)
        {
            TrError = e_cancelled;
            break;
        }

        /* reset OId and  goto next page */
        OId.gg92SetNil();
        NptrsPage.bd910MoveToNextPage();
    }
    while ((e_ok == TrError) && NptrsPage.bd910PageIsAvailable());
    /* end of loop over pages */

    /* nothing was found */
    OId.gg92SetNil();
    if (e_ok == TrError)
        TrError = e_no_next_object;
}

/*---------------------------------------------------------------------------*/
externCpp void
bd90NextOidsConsistent (cbd05_Current          &Current,
                        bool                    bOnlyHdrVarObj,
                        const tgg92_KernelOid  &CurrOid,
                        tsp00_Int4             &noOfOid,
                        tgg92_KernelOid        *pOid,
                        tgg91_PageRef          *pObjHistRef,
                        tsp00_Int4             &AllLogReadAccesses,  
                        Msg_List               &ErrorList)
{
    // NOTE! if this function returns e_wait_for_lock_release then wait outside bd-locking
    // within bd90_GetConsistentObjImage lock timeout is set to 0 therefore 
    // e_wait_for_lock_release won't be returned anymore                    FF 2003-Apr-01

    SAPDBTRACE_ROUTINE_DEBUG("bd90NextOidsConsistent", ObjContainer_Trace, 5);

    tgg00_BasisError      &TrError         = Current.curr_trans->trError_gg00;
    tgg00_TransContext    &Trans           = *Current.curr_trans;
    tsp00_Bool             bNextObjIsFound = false;
    cbd910_PrimObjDataNode NptrsPage(Current, nr_for_read);
    tsp00_Int4             LogReadAccessesPerPage;   

    const FileDir_FileNo  FileNo = Current.curr_tree_id.fileDirFileId_gg00();     
    FileDir_Oms     fd_smart;

    /*---------------------------------*/
    /*  find starting page for search  */
    /*---------------------------------*/

    if (!CurrOid.gg92IsNil() )
    {
        if ( !bd90GetFile( Trans, fd_smart,  FileNo, !cDoShareLock, __LINE__, ErrorList))
            return;

        /* if OID is not nil look for object following the given OID  */
        NptrsPage.bd910AssignToPage(CurrOid.gg92GetPno());
        if (e_ok != TrError) return;

        /* check fileno and state of input object */
        BD92CHECK_OID_POSITION ("bd90NextOidsConsistent",TrError, CurrOid, NptrsPage);
        tbd900_ObjFrame *pObjFrame = NptrsPage.bd910ObjFramePtr (CurrOid.gg92GetPos());

        if (pObjFrame->objHeader_bd900.ohdState_gg92 == obsFree_egg00)
        {
            Kernel_OpWarning  opErrMsg( csp3_bd_msg, csp3_n_obj );
            opErrMsg << "bd90NextOidsConsistent:Invalid iterator!"
                << " State:obsFree"
                << " OID=" << CurrOid.gg92GetPno() << "." << CurrOid.gg92GetPos()
                << " (vers " << CurrOid.gg92GetFrameVers() << ")";

            /* note: The previous check is very simple in principle one              */
            /*       should even check if the input OId belongs to an object which   */
            /*       owns a consistent image with respect to the current transaction */

            TrError = e_wrong_object_state;
            return;
        }
    }
    else
    {
        if ( !bd90GetFile( Trans, fd_smart,  FileNo, cDoShareLock, __LINE__, ErrorList))
            return;
        /*  look for very first object in file, which  */
        /*  should be on the  subroot                  */
        const tsp00_PageNo     SubRoot = fd_smart.GetRootPage();
        NptrsPage.bd910AssignToPage(SubRoot);
        if (e_ok != TrError) return;
    }

    /*-------------------------------------------------*/
    /*  start looping over chains and pages of chains  */
    /*-------------------------------------------------*/


    // loop over object page 
    tsp00_Int4 maxNoOfOid = noOfOid;
    noOfOid = 0;
    pOid[0] = CurrOid;
    while (noOfOid < maxNoOfOid && TrError == e_ok) {
        //  look for the next object on current page  
        tsp00_Int4    ObjBodySize = 0; // unused parameter 
        tgg91_PageRef ObjVers;         // unused parameter PTS 1120478 FF
        bd90NextObjConsistentOnThisPage (Current, 
                                          fd_smart,
                                          pOid[noOfOid], 
                                          ObjVers,      // PTS 1120478 FF
                                          pObjHistRef[noOfOid], 
                                          false, //bWithObject, 
										                      bOnlyHdrVarObj, 
                                          0,     //ExtObjBodySize,
                                          NULL,  //pExtObjBody, 
                                          NptrsPage, 
                                          ObjBodySize, 
                                          bNextObjIsFound, 
                                          LogReadAccessesPerPage,
                                          ErrorList);

        // accumulate the number of read accesses to object history pages; PTS 1107819 AK 12/09/2000 
        AllLogReadAccesses += LogReadAccessesPerPage;
        if (TrError == e_ok)
        {
          if (bNextObjIsFound) {
            if ((g01vtrace.vtrBdObject_gg00) || (g01vtrace.vtrOmsGet_gg00 )) {
              Kernel_VTrace vtrace;
              vtrace << "OidIter-NextOid="
                << pOid[noOfOid].gg92GetPno() << "." << pOid[noOfOid].gg92GetPos()
                << " (vers " << pOid[noOfOid].gg92GetFrameVers() << ")"
                << " ObjVers:";
              if (pObjHistRef[noOfOid].gg91IsNilRef())
                vtrace << " NilRef";
              else
                vtrace << pObjHistRef[noOfOid].gg91RefPno() 
                << "." << pObjHistRef[noOfOid].gg91RefPos();
            }
            // Object was found on this page. Therefore increment number of objects read 
            // and initialize next buffer element with last oid 
            ++noOfOid;
            if (noOfOid < maxNoOfOid){
              pOid[noOfOid] = pOid[noOfOid-1];
            }
          }
          else if (!bNextObjIsFound){
            // No (further) object was found on this page. 
            // Reset current oid and goto next page
            pOid[noOfOid].gg92SetNil();
            NptrsPage.bd910MoveToNextPage();
            if (!NptrsPage.bd910PageIsAvailable()){
              // There is no next page in the page chain, so all objects have been read
              TrError = e_no_next_object;
              break;
            }
          } 
          
          // check if the command has been canceled. PTS 1105431 Alexander Kley 25/01/2000 
          if (Current.curr_trans->trRteCommPtr_gg00->to_cancel){
            TrError = e_cancelled;
            break;
          }
        }
    }
}

/*---------------------------------------------------------------------------*/

externCpp void
bd90NextObjFromKeyIterator (tgg00_TransContext          &Trans,
                            LVC_AbstractObjKeyIterator  &KeyIterator,    
                            bool                         bInclusive,  
                            tsp00_Int4                   objBodySize,
                            tsp00_Int4                   KeyLen,      
                            void                        *pRestartKey, 
                            tsp00_Int4                  &noOfOid,
                            tgg92_KernelOid             *pResultOId, 
                            tgg91_PageRef               *pResultObjVers,
                            tsp00_MoveObj               *pBody,
                            tsp00_Int4                  &LogReadAccesses,
                            Msg_List                    &ErrorList)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd90NextObjFromKeyIterator", ObjContainer_Trace, 5);

    tgg00_BasisError &TrError                      = Trans.trError_gg00;
    tsp00_Int4        LogReadAccessesForThisObject = 0;
    tsp00_Bool        bConsistentObjImageObjFound  = false;
    
    const FileDir_FileNo  FileNo = KeyIterator.GetObjFileId().fileDirFileId_gg00();
    cbd911RecreateIndexLock lock(Trans);  
    if (lock.IsNeeded(FileNo))
        lock.Lock( !bExclusive );
    
    // PTS 1119480
    if (!KeyIterator.AscendingKeyOrder()){  
      KeyIterator.ChangeDirection(pRestartKey, 
                                  true,         // bAscendingKeyOrder
                                  ErrorList);  
    }

    // PTS 1119647 FF 2002-DEC-05
    cbd05_Current  Current(Trans, m_update, KeyIterator.GetObjFileId());

    FileDir_Oms    fd_smart;
    if ( !bd90GetFile( Trans, fd_smart,  FileNo, !cDoShareLock, __LINE__, ErrorList))
        return;

    // Remember number of maximal Oids (size of the return array)
    tsp00_Int4 maxNoOfOid = noOfOid;
    noOfOid = 0;
    while (!KeyIterator.IsEmpty()) {
      noOfOid = maxNoOfOid;
      KeyIterator.GetNextOIds(pRestartKey, pResultOId, noOfOid, bInclusive, ErrorList);        

      int i,j;
      for (i=0,j=0; i<noOfOid; ++i){
        cbd910_PrimObjDataNode   NptrsPage (Current, pResultOId[i].gg92GetPno(), nr_for_read);
        if(TrError != e_ok) {
          /* PTS 1121757 FF 24-Apr-2003 */
          if ((e_page_in_wrong_tree == TrError) || (e_no_converter_entry == TrError))
          {
            TrError = e_ok;
            continue;   // Skip Oid
          }
          Kernel_OpError  opErrMsg( csp3_bd_msg, csp3_n_obj );
          opErrMsg << "bd90NextObjFromKeyIterator-1-: error:" << TrError
                   << " Fileno:" << KeyIterator.GetObjFileId().fileObjFileNo_gg00()
                   << " OID=" << pResultOId[i].gg92GetPno() << "." << pResultOId[i].gg92GetPos()
                   << " (vers " << pResultOId[i].gg92GetFrameVers() << ")";
          noOfOid = 0;
          return;
        }

        tsp00_Int4      ResultObjBodySize;
        tgg91_PageRef   ObjHistRef;  // not used parameter PTS 1120478 FF

        /* in case the object body is delivered, the otrBeforeRef_gg00 Pointer is needed */
        /* for further operation on this object ( omsLock, omsStore, ... )               */
        /* in case that no body is delivered, a history position is returned to enable   */
        /* following omsDeref Operations by the iterator to be handled faster.           */
        if (pBody == NULL){
          bd90_GetConsistentObjImage (Current, 
                                      fd_smart,
                                      pResultOId[i], 
                                      ObjHistRef,
                                      pResultObjVers[i],  // return history position
                                      false,              // bWithObject
                                      false,              // bOnlyHdrVarObjects,    
                                      objBodySize, 
                                      NULL, 
                                      NptrsPage, 
                                      ResultObjBodySize, 
                                      bConsistentObjImageObjFound,
                                      LogReadAccessesForThisObject,
                                      ErrorList);
        }
        else {
          bd90_GetConsistentObjImage (Current, 
                                      fd_smart,
                                      pResultOId[i], 
                                      pResultObjVers[i],  // return BeforeRef
                                      ObjHistRef,
                                      true,               // bWithObject
                                      false,              // bOnlyHdrVarObjects,    
                                      objBodySize, 
                                      pBody, 
                                      NptrsPage, 
                                      ResultObjBodySize, 
                                      bConsistentObjImageObjFound,
                                      LogReadAccessesForThisObject,
                                      ErrorList);
        }

        LogReadAccesses += LogReadAccessesForThisObject;

        if(TrError != e_ok) {  // PTS 1124265
          Kernel_OpError  opErrMsg( csp3_bd_msg, csp3_n_obj );
          opErrMsg << "bd90NextObjFromKeyIterator:-2- error:" << TrError
                   << " Fileno:" << KeyIterator.GetObjFileId().fileObjFileNo_gg00()
                   << " OID=" << pResultOId[i].gg92GetPno() << "." << pResultOId[i].gg92GetPos()
                   << " (vers " << pResultOId[i].gg92GetFrameVers() << ")";
          noOfOid = 0;
          return;
        }
      
        if (bConsistentObjImageObjFound){
          if (i != j){
            // Compress array with OIds
            pResultOId[j]     = pResultOId[i];
            pResultObjVers[j] = pResultObjVers[i];
          }
          if ((g01vtrace.vtrBdObject_gg00) || (g01vtrace.vtrOmsGet_gg00 )) {
            Kernel_VTrace vtrace;
            vtrace << "KeyIter-NextOid="
              << pResultOId[j].gg92GetPno() << "." << pResultOId[j].gg92GetPos()
              << " (vers " << pResultOId[j].gg92GetFrameVers() << ")"
              << " ObjVers:";
            if (pResultObjVers[j].gg91IsNilRef())
              vtrace << " NilRef";
            else
              vtrace << pResultObjVers[j].gg91RefPno() 
              << "." << pResultObjVers[j].gg91RefPos();
          }
          ++j;
        }
        else {
          // Skip Oid
          //pResultOId[i].gg92SetNil();
        }
      }
      noOfOid = j;
      if (noOfOid > 0) {
        // There is at least one OID found
        break;
      }
      else {
        // Read next portion of OIds from kernel. The last key is remembered
        // in the iterator and if the pRestartKey equals NULL, then this locally
        // remembered key is used instead.
        pRestartKey = NULL;
      }
    }

    if (e_ok == TrError && KeyIterator.IsEmpty()){
      TrError = e_no_next_object;
    }
}

/*---------------------------------------------------------------------------*/
externCpp void
bd90PrevObjFromKeyIterator (tgg00_TransContext          &Trans,
                            LVC_AbstractObjKeyIterator  &KeyIterator,    
                            bool                         bInclusive,  
                            tsp00_Int4                   objBodySize,
                            tsp00_Int4                   KeyLen,      
                            void                        *pRestartKey, 
                            tsp00_Int4                  &noOfOid,
                            tgg92_KernelOid             *pResultOId, 
                            tgg91_PageRef               *pResultObjVers,
                            tsp00_MoveObj               *pBody,
                            tsp00_Int4                  &LogReadAccesses,
                            Msg_List                    &ErrorList)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd90PrevObjFromKeyIterator", ObjContainer_Trace, 5);

    tgg00_BasisError &TrError                      = Trans.trError_gg00;
    tsp00_Int4        LogReadAccessesForThisObject = 0;
    tsp00_Bool        bConsistentObjImageObjFound  = false;
    
    if (KeyIterator.AscendingKeyOrder()){  // PTS 1119480
      KeyIterator.ChangeDirection(pRestartKey, 
                                  false,        // bAscendingKeyOrder
                                  ErrorList);  
    }

    // PTS 1119647 FF 2002-DEC-05
    cbd05_Current  Current(Trans, m_update, KeyIterator.GetObjFileId());

    const FileDir_FileNo  FileNo = KeyIterator.GetObjFileId().fileDirFileId_gg00();
    FileDir_Oms    fd_smart;
    if ( !bd90GetFile( Trans, fd_smart,  FileNo, !cDoShareLock, __LINE__, ErrorList))
        return;
    
    // Remember number of maximal Oids (size of the return array)
    tsp00_Int4 maxNoOfOid = noOfOid;
    noOfOid = 0;
    while (!KeyIterator.IsEmpty()) {
      noOfOid = maxNoOfOid;
      KeyIterator.GetPrevOIds(pRestartKey, pResultOId, noOfOid, bInclusive, ErrorList);        

      int i,j;
      for (i=0,j=0; i<noOfOid; ++i){
        cbd910_PrimObjDataNode   NptrsPage (Current, pResultOId[i].gg92GetPno(), nr_for_read);
        if(TrError != e_ok) {
          /* PTS 1121757 FF 24-Apr-2003 */
          if ((e_page_in_wrong_tree == TrError) || (e_no_converter_entry == TrError))
          {
            TrError = e_ok;
            continue;   // Skip Oid
          }
          Kernel_OpError  opErrMsg( csp3_bd_msg, csp3_n_obj );
          opErrMsg << "bd90NextObjFromKeyIterator: error:" << TrError
                   << " Fileno:" << KeyIterator.GetObjFileId().fileObjFileNo_gg00()
                   << " OID=" << pResultOId[i].gg92GetPno() << "." << pResultOId[i].gg92GetPos()
                   << " (vers " << pResultOId[i].gg92GetFrameVers() << ")";
          return;
        }

        tsp00_Int4      ResultObjBodySize;
        tgg91_PageRef   ObjHistRef;  // not used parameter PTS 1120478 FF

        /* in case the object body is delivered, the otrBeforeRef_gg00 Pointer is needed */
        /* for further operation on this object ( omsLock, omsStore, ... )               */
        /* in case that no body is delivered, a history position is returned to enable   */
        /* following omsDeref Operations by the iterator to be handled faster.           */
        if (pBody == NULL){
          bd90_GetConsistentObjImage (Current,
                                      fd_smart,
                                      pResultOId[i], 
                                      ObjHistRef,
                                      pResultObjVers[i],  // return history position
                                      false,              // bWithObject
                                      false,              // bOnlyHdrVarObjects,    
                                      objBodySize, 
                                      pBody, 
                                      NptrsPage, 
                                      ResultObjBodySize, 
                                      bConsistentObjImageObjFound,
                                      LogReadAccessesForThisObject,
                                      ErrorList);
        }
        else {
          bd90_GetConsistentObjImage (Current,
                                      fd_smart,
                                      pResultOId[i], 
                                      pResultObjVers[i],  // return BeforeRef
                                      ObjHistRef,
                                      true,               // bWithObject
                                      false,              // bOnlyHdrVarObjects,    
                                      objBodySize, 
                                      pBody, 
                                      NptrsPage, 
                                      ResultObjBodySize, 
                                      bConsistentObjImageObjFound,
                                      LogReadAccessesForThisObject,
                                      ErrorList);
        }
        LogReadAccesses += LogReadAccessesForThisObject;
      
        if (bConsistentObjImageObjFound){
          if (i != j){
            // Compress array with OIds
            pResultOId[j]     = pResultOId[i];
            pResultObjVers[j] = pResultObjVers[i];
          }
          if ((g01vtrace.vtrBdObject_gg00) || (g01vtrace.vtrOmsGet_gg00 )) {
            Kernel_VTrace vtrace;
            vtrace << "KeyIter-PrevOid="
              << pResultOId[j].gg92GetPno() << "." << pResultOId[j].gg92GetPos()
              << " (vers " << pResultOId[j].gg92GetFrameVers() << ")"
              << " ObjVers:";
            if (pResultObjVers[j].gg91IsNilRef())
              vtrace << " NilRef";
            else
              vtrace << pResultObjVers[j].gg91RefPno() 
              << "." << pResultObjVers[j].gg91RefPos();
          }
          ++j;
        }
        else {
          // Skip Oid
          //pResultOId[i].gg92SetNil();
        }
      }
      noOfOid = j;
      if (noOfOid > 0) {
        // There is at least one OID found
        break;
      }
      else {
        // Read next portion of OIds from kernel. The last key is remembered
        // in the iterator and if the pRestartKey equals NULL, then this locally
        // remembered key is used instead.
        pRestartKey = NULL;
      }
    }

    if (e_ok == TrError && KeyIterator.IsEmpty()){
      TrError = e_no_next_object;
    }
}

/*---------------------------------------------------------------------------*/

externCpp void
bd90ReadAhead (cbd05_Current  &Current)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd90ReadAhead", ObjContainer_Trace, 5);

    /* this function reads all pages of a file in order to load them into the data cache       */
    /* however reading is stopped if the data cache is full (i.e. occupied by more than 99.9%) */
    /* PTS 1107145 AK 11/07/00 function rewritten                                              */

    Msg_List              errorList;
    tgg00_BasisError     &TrError         = Current.curr_trans->trError_gg00;
    tgg00_TransContext   &Trans           = *Current.curr_trans;
    const FileDir_FileNo  ObjFileNo       = Current.curr_tree_id.fileDirFileId_gg00();
    FileDir_Oms           fd_smart;
    FileDir_ReturnCode    fd_rc;
    fd_rc = FileDir_IDirectory::SessionInstance(Trans).GetFile(
        Trans.trTaskId_gg00, ObjFileNo, fd_smart, errorList);
    if (FileDir_Okay != fd_rc) {
        TrError = e_ok;
        return;
    }
    const tsp00_PageNo       RootPno         = fd_smart.GetRootPage();

    cbd910_ObjNode           NptrsPage (Current, RootPno, nr_for_read);
    if (e_ok != TrError)     
        return;

    bd90_ReadAheadFile (*Current.curr_trans, NptrsPage);
    if (Current.curr_tree_id.fileObjFileType_gg00() == oftVarLenObjFile_egg00)
    {
        cbd05_Current       ContCurrent     (*Current.curr_trans, m_update);  
        FileDir_OmsVar      fd_var_smart;
        fd_var_smart.Assign(fd_smart);
        for (SAPDB_UInt1 iContFileNo=1; iContFileNo <= MAX_NUM_CONT_OBJ_FILES_BD900; ++iContFileNo)
        {
            Data_PageNo cont_root = fd_var_smart.GetContPartition(iContFileNo);
            if (NIL_PAGE_NO_GG00 != cont_root) 
            {
                fd_var_smart.BuildContFileId(iContFileNo,ContCurrent.curr_tree_id);
                cbd910_ObjNode  NptrsPage (ContCurrent, cont_root, nr_for_read);
                if (e_ok != TrError)     
                    return;
                bd90_ReadAheadFile (*ContCurrent.curr_trans, NptrsPage);
            }
        }
    }
}

/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/

externCpp void
bd90ReuseObj (cbd05_Current        &Current,
              tgg92_KernelOid      &OId,
              tgg91_PageRef        &ResultObjVers,
              tsp00_Int4            KeySize,
              void                 *pKey,
              bool                 &bTryAgain,
              Msg_List             &ErrorList)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd90ReuseObj", ObjContainer_Trace, 5);

    tgg00_BasisError    &TrError = Current.curr_trans->trError_gg00;
    bTryAgain = false;

    cbd910_PrimObjDataNode    NptrsPage (Current, OId.gg92GetPno(),  nr_for_update);
    if (TrError != e_ok)
    {
        if ((TrError == e_page_in_wrong_tree) || (TrError == e_no_converter_entry))
        {
            // OID provided from key-management isn't valid yet
            TrError = e_too_old_oid;
            bTryAgain = true;
        }
        return;
    }

    /* get pointer to object frame*/
    BD92CHECK_OID_POSITION ("bd90ReuseObj",TrError, OId, NptrsPage);
    tbd900_ObjFrame  *pObjFrame = NptrsPage.bd910ObjFramePtr (OId.gg92GetPos());

    if (pObjFrame->objHeader_bd900.ohdState_gg92 == obsFree_egg00)
    {
        TrError   = e_wrong_object_state;
        bTryAgain = true;
        return;
    }

    const int  ObjBodySize = NptrsPage.np_ptr()->nd_obj_frame_len() - sizeof(tgg92_ObjHeader);
    void   *pKeyInObjFrame = pObjFrame->objBody_bd900 + ObjBodySize - KeySize;

    if (memcmp (pKey, pKeyInObjFrame, KeySize))
    {
        // meanwhile the information from the hash that there exists an
        // object frame with the key pKey may be wrong since the garbage
        // collector removed this object, therefore this frame cannot be
        // bOmsLayerReusedObj and one has to try again to insert the key pKey into the hash

        TrError   = e_wrong_object_version;
        bTryAgain = true;
        return;
    }

    tgg00_ObjTransInfo ObjTransInfo  = pObjFrame->objHeader_bd900.ohdTransInfo_gg92;
    const bool         bWithObj      = false;
    tgg00_ObjFrameVers ResultFrameVers = NIL_OBJ_FRAME_VERS_GG92;
    tgg91_PageRef      ObjHistRef; 
    tsp00_Int4         LogReadAccesses = 0;
    ResultObjVers.gg91SetNilRef();
    ObjHistRef.gg91SetNilRef();
    int ObjBdySz = ObjBodySize;

    /* omsDeleteAll or SubtransRollback may have delete the object within same transaction */
    /* and another transaction may already be trying to reuse the object frame             */
    if (( ObjTransInfo.otrUpdTransId_gg00 != Current.curr_trans->trWriteTransId_gg00 ) &&
        ( pObjFrame->objHeader_bd900.ohdState_gg92 == obsFreeAfterEot_egg00 ) && 
        ( !gg06IsNilTrans (Current.curr_trans->trConsistView_gg00) ))
    {
        tgg91_TransNo       DummyUpdTransId;
        kb67GetObjConsistent (*Current.curr_trans, 
            OId, 
            pObjFrame->objHeader_bd900.ohdState_gg92, 
            ObjTransInfo,
            bWithObj, 
            KeySize, 
            ObjBdySz, 
            0,              // ExtObjBodySize, 
            NULL,           // pExtObjBody, 
            ResultFrameVers, 
            ResultObjVers, 
            ObjHistRef, 
            DummyUpdTransId,
            LogReadAccesses,
            ErrorList);  

        if ((TrError == e_ok) && !ObjHistRef.gg91IsNilRef() )
        {
            TrError = e_key_still_visible;
            return;           
        }
        else
            TrError = e_ok;
    }


#if defined(FOR_TESTING_ONLY)
    tgg92_KernelOid     ResultOid;
    bool                fIndexNotOk = false;
    tgg00_BasisError    KeyTrError = e_ok;
    bd22GetOId (*Current.curr_trans, Current.curr_tree_id.fileObjFileNo_gg00(), KeySize, pKey, ResultOid, ErrorList);
    if (e_ok != TrError)
    {
        KeyTrError = TrError;
        fIndexNotOk = true;
    }

    if (!((ResultOid.gg92GetPos() == OId.gg92GetPos()) &&
            (ResultOid.gg92GetPno() == OId.gg92GetPno())))
    {
        fIndexNotOk = true;
    }
#endif

    SAPDB_Int1    contVarObj    = 0;
    tsp00_Uint2 AuxReqTimeout = Current.curr_trans->trObjReqTimeout_gg00; //save
    kb67NewObj (*Current.curr_trans, Current.curr_tree_id, OId, pObjFrame->objHeader_bd900,
                ObjBodySize, KeySize, contVarObj, ErrorList);
    if (e_wait_for_lock_release == TrError) 
    {
        bTryAgain = true;
        return;
    }

#if defined(FOR_TESTING_ONLY)
    if ( fIndexNotOk )
    {
        {
            Kernel_OpError  opErrMsg( csp3_bd_msg, csp3_n_obj );
            opErrMsg << "bd90ReuseObj: Key-Index is not ok ! TrError:"
            << KeyTrError << " OID=" << OId.gg92GetPno() << "." << OId.gg92GetPos()
            << " Idx OID=" << ResultOid.gg92GetPno() << "." << ResultOid.gg92GetPos();
        }
        g01abort (csp3_bd_msg, csp3_n_obj,
                  "bd90ReuseObj: IDX not ok", Current.curr_trans->trError_gg00);
    }
#endif

    // PTS 1132118 UH 2004-10-25 ResultObjVers is always assigned
    ResultObjVers = pObjFrame->objHeader_bd900.ohdTransInfo_gg92.otrBeforeRef_gg00;
    OId.gg92SetFrameVers(pObjFrame->objHeader_bd900.ohdFrameVersion_gg92);

    if (TrError == e_ok)
    {
        // PTS 1132118 UH 2004-10-25 ResultObjVers is always assigned
        // ResultObjVers = pObjFrame->objHeader_bd900.ohdTransInfo_gg92.otrBeforeRef_gg00;

        -- NptrsPage.np_ptr()->nd_free_eot_obj_cnt();
        ++ NptrsPage.np_ptr()->nd_occ_obj_cnt();

        pObjFrame->objHeader_bd900.ohdState_gg92.becomes(obsKeyReserved_egg00);

        if(pObjFrame->objHeader_bd900.ohdFrameVersion_gg92 >= MAX_OBJ_FRAME_VERS_GG92)
            pObjFrame->objHeader_bd900.ohdFrameVersion_gg92 = MIN_OBJ_FRAME_VERS_GG92;
        else
            pObjFrame->objHeader_bd900.ohdFrameVersion_gg92 ++;

        OId.gg92SetFrameVers (pObjFrame->objHeader_bd900.ohdFrameVersion_gg92);
    }
}

/*---------------------------------------------------------------------------*/

externCpp void 
bd90RollbackObj (cbd05_Current        &Current,            //[inp\out]
                 tgg00_MessType_Enum   RollbackType,       //[inp]
                 tgg92_KernelOid      &OId,                //[inp]
                 const FileDir_FileNo &ObjFileNo,
                 tgg00_ObjState        NewState,           //[inp]
                 tgg91_PageRef         BeforeObjRef,       //[inp]
                 tgg91_TransNo         BeforeObjUpdVers,   //[inp]
                 tsp00_Int4            BeforeObjSize,      //[inp]
                 void                 *pBeforeObj,         //[inp]
                 SAPDB_Int1            contVarObject,
                 bool                  bOmsLayerReusedObj,
                 Msg_List             &ErrorList)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd90RollbackObj", ObjContainer_Trace, 5);

    /* possible actions which can be rollbacked by this procedure */

    /* current state  |   action   |   new state    */
    /* ---------------------------------------------*/
    /*  reserved     --- m_new    ---> feot         */ // PTS 1113342 UH 2002-01-10
    /*  key_reserved --- m_new    ---> feot         */ // PTS 1113342 UH 2002-01-10
    /*  key_reserved --- m_new    ---> feot         */
    /*  occupied     --- m_insert ---> reserved     */
    /*  occupied     --- m_insert ---> key_reserved */
    /*  occupied     --- m_update ---> occupied     */
    /*  feot         --- m_delete ---> occupied     */
    /*  feot         --- m_delete ---> reserved     */
    /*  feot         --- m_delete ---> key_reserved */

    tgg00_BasisError     &TrError                      = Current.curr_trans->trError_gg00;
    tgg00_TransContext   &Trans                        = *Current.curr_trans;
    bool                  bTooManyEmptyPagesInContFile = false;
    tsp00_Int4            ContObjFileNo;

    /* get page pointer */
    cbd910_PrimObjDataNode  NptrsPage(Current, OId.gg92GetPno(), nr_for_update);
    if (TrError != e_ok)
    {
        if ((e_page_in_wrong_tree == TrError) || (e_no_converter_entry == TrError)) /* PTS 1105630 Alexander Kley 16/02/2000 */
        {
            /* PTS 1114914 FF 2002-03-13 */
            if ((m_new == RollbackType) &&
                    ((obsFreeAfterEot_egg00 == NewState) || // for REUSED KeyedObjects   PTS 1119318 FF 2002-Dec-4
                    (obsFree_egg00 == NewState)) &&  // NewState == obsFree is mapped to obsFreeAfterEot (see below)
                    Kernel_IAdminInfo::Instance().KernelStateIsRedoLogProcessing() &&
                    ((Log_Transaction *) Current.curr_trans->trLogContext_gg00)->WasOpenInLastSavepoint() )
            {
                Kernel_OpWarning  opWarnMsg( csp3_bd_msg, csp3_n_obj );
                opWarnMsg << "bd90RollbackObj error:" << TrError << " ignored!"
                << " RollbackType:" << RollbackType 
                << " NewState:" << NewState
                << " OID=" << OId.gg92GetPno() << "." << OId.gg92GetPos() << " (vers " << OId.gg92GetFrameVers() << ")";
                TrError = e_ok;
            }
        }
        return;
    }

    Converter_Version converterVersion = 
        bd20GetDestinationConverterVersion ( Trans.trTaskId_gg00, NptrsPage.np_cbptr() );

    tbd_nodeptr &pPage            = NptrsPage.np_ptr();
    const bool   bPageIsForVarObj = (pt2VarObject_egg00 == NptrsPage.np_ptr()->nd_pt2());

    FileDir_FileNo CurrentObjId(Current.curr_tree_id.fileDirFileId_gg00());
    FileDir_FileNo pageObjId;
    bd900GetFileNo(pPage, pageObjId);
    if ((ObjFileNo != CurrentObjId) || //PTS 1109582 AK 09/03/2001
        (ObjFileNo != pageObjId))
    {
        /* PTS 1114818 FF 2002-03-13 */
        if ((m_new == RollbackType) &&
            ((obsFreeAfterEot_egg00 == NewState) || // for REUSED KeyedObjects   PTS 1119318 FF 2002-Dec-4
            (obsFree_egg00 == NewState)) &&  // NewState == obsFree is mapped to obsFreeAfterEot (see below)
            Kernel_IAdminInfo::Instance().KernelStateIsRedoLogProcessing() &&
            ((Log_Transaction *) Current.curr_trans->trLogContext_gg00)->WasOpenInLastSavepoint() )
        {
            FileDir_FileNo fileno;
            bd900GetFileNo( pPage, fileno);
            Kernel_OpWarning  opWarnMsg( csp3_bd_msg, csp3_n_obj );
            opWarnMsg << "bd90RollbackObj error:" << TrError << " ignored!"
                << " RollbackType:" << RollbackType 
                << " NewState:" << NewState
                << " OID=" << OId.gg92GetPno() << "." << OId.gg92GetPos()
                << " (vers " << OId.gg92GetFrameVers() << ")"
                << " FileNo:" << fileno << " expected FileNo:" 
                << ObjFileNo;
            TrError = e_ok;
        }
        else
        {
            FileDir_FileNo fileno;
            bd900GetFileNo( pPage, fileno);
            Kernel_OpError  opErrMsg( csp3_bd_msg, csp3_n_obj );
            opErrMsg << "bd90RollbackObj error:" << TrError
                << " RollbackType:" << RollbackType 
                << " NewState:" << NewState
                << " OID=" << OId.gg92GetPno() << "." << OId.gg92GetPos()
                << " (vers " << OId.gg92GetFrameVers() << ")"
                << " FileNo:" << fileno << " expected FileNo:" 
                << ObjFileNo;
            TrError = e_wrong_class_id;
        }
        return;
    }
    
    /* determine pointer to object frame */
    BD92CHECK_OID_POSITION ("bd90RollbackObj",TrError, OId, NptrsPage);
    tbd900_ObjFrame  *pObjFrame = NptrsPage.bd910ObjFramePtr (OId.gg92GetPos());

    /* check if the wanted rollback is possible */
    switch (pObjFrame->objHeader_bd900.ohdState_gg92)
    {
    case obsReserved_egg00:
        if (m_new != RollbackType || obsFree_egg00 != NewState)
        {
            /* PTS 1114818 FF 2002-03-13 */
            if (((( m_insert == RollbackType ) || ( m_delete == RollbackType )) &&
                    (obsReserved_egg00 == NewState) &&
                    Kernel_IAdminInfo::Instance().KernelStateIsRedoLogProcessing() &&
                    ((Log_Transaction *) Current.curr_trans->trLogContext_gg00)->WasOpenInLastSavepoint() )
                ||
                /* PTS 1127279  if update for new object fails with storing data within continous container */
                (( pt2VarObject_egg00 == NptrsPage.np_ptr()->nd_pt2() ) &&
                ( m_insert == RollbackType ) && 
                ( obsReserved_egg00 == NewState )))
            {
                Kernel_OpWarning( csp3_bd_msg, csp3_n_obj )
                    << __FILE__ << ":" << __LINE__
                    << " bd90RollbackObj invalid_state 'obsReserved' ignored!"
                    << " RollbackType:" << RollbackType
                    << " OID=" << OId.gg92GetPno() << "." << OId.gg92GetPos()
                    << " (vers " << OId.gg92GetFrameVers() << ")";
                TrError = e_ok;
            }
            else
            {
                Kernel_OpWarning( csp3_bd_msg, csp3_n_obj )
                    << __FILE__ << ":" << __LINE__
                    << " bd90RollbackObj invalid_state 'obsReserved' found!"
                    << " RollbackType:" << RollbackType
                    << " OID=" << OId.gg92GetPno() << "." << OId.gg92GetPos()
                    << " (vers " << OId.gg92GetFrameVers() << ")";
                TrError = e_wrong_object_state;
            }
            return;
        }
        break;

    case obsKeyReserved_egg00:
        if (m_new != RollbackType                 ||
                (obsFree_egg00         != NewState  &&
                 obsFreeAfterEot_egg00 != NewState))
        {
            /* PTS 1114818 FF 2002-03-13 */
            if ((( m_insert == RollbackType ) || ( m_delete == RollbackType )) &&
                    (obsKeyReserved_egg00 == NewState) &&
                    Kernel_IAdminInfo::Instance().KernelStateIsRedoLogProcessing() &&
                    ((Log_Transaction *) Current.curr_trans->trLogContext_gg00)->WasOpenInLastSavepoint() )
            {
                Kernel_OpWarning( csp3_bd_msg, csp3_n_obj )
                    << __FILE__ << ":" << __LINE__
                    << " bd90RollbackObj invalid_state 'obsKeyReserved' ignored!"
                    << " RollbackType:" << RollbackType
                    << " OID=" << OId.gg92GetPno() << "." << OId.gg92GetPos()
                    << " (vers " << OId.gg92GetFrameVers() << ")";
                TrError = e_ok;
            }
            else
            {
                Kernel_OpWarning( csp3_bd_msg, csp3_n_obj )
                    << __FILE__ << ":" << __LINE__
                    << " bd90RollbackObj invalid_state 'obsKeyReserved' found!"
                    << " RollbackType:" << RollbackType
                    << " OID=" << OId.gg92GetPno() << "." << OId.gg92GetPos()
                    << " (vers " << OId.gg92GetFrameVers() << ")";
                TrError = e_wrong_object_state;
            }
            return;
        }
        break;

    case obsOccupied_egg00:
        if (!((m_insert == RollbackType                                            &&
                ((obsReserved_egg00    == NewState && pPage->nd_obj_key_len() == 0)  ||
                 (obsKeyReserved_egg00 == NewState && pPage->nd_obj_key_len() > 0 )))    ||
                ((m_lock == RollbackType /* PTS 1109296 UH 2001-01-26 */ || m_update == RollbackType) && obsOccupied_egg00 == NewState)))
        {
            /* PTS 1114818 FF 2002-03-13 */
            if ((( m_update == RollbackType ) || ( m_delete == RollbackType )) &&
                    (obsOccupied_egg00 == NewState) &&
                    Kernel_IAdminInfo::Instance().KernelStateIsRedoLogProcessing() &&
                    ((Log_Transaction *) Current.curr_trans->trLogContext_gg00)->WasOpenInLastSavepoint() )
            {
                Kernel_OpWarning( csp3_bd_msg, csp3_n_obj )
                    << __FILE__ << ":" << __LINE__
                    << " bd90RollbackObj invalid_state 'obsOccupied' ignored!"
                    << " RollbackType:" << RollbackType
                    << " OID=" << OId.gg92GetPno() << "." << OId.gg92GetPos()
                    << " (vers " << OId.gg92GetFrameVers() << ")";
                TrError = e_ok;
            }
            else
            {
                Kernel_OpWarning( csp3_bd_msg, csp3_n_obj )
                    << __FILE__ << ":" << __LINE__
                    << " bd90RollbackObj invalid_state 'obsOccupied' found!"
                    << " RollbackType:" << RollbackType
                    << " OID=" << OId.gg92GetPno() << "." << OId.gg92GetPos()
                    << " (vers " << OId.gg92GetFrameVers() << ")";
                TrError = e_wrong_object_state;
            }
            return;
        }
        break;

    case obsFreeAfterEot_egg00:
        if (m_delete != RollbackType
                ||
                (obsOccupied_egg00    != NewState &&
                 obsReserved_egg00    != NewState &&
                 obsKeyReserved_egg00 != NewState))
        {
            /* PTS 1114818 FF 2002-03-13 */
            if (( m_new == RollbackType ) &&
                    ((obsFreeAfterEot_egg00 == NewState) || // for REUSED KeyedObjects   PTS 1119318 FF 2002-Dec-4
                    (obsFree_egg00 == NewState)) &&  // NewState == obsFree is mapped to obsFreeAfterEot (see below)
                    Kernel_IAdminInfo::Instance().KernelStateIsRedoLogProcessing() &&
                    ((Log_Transaction *) Current.curr_trans->trLogContext_gg00)->WasOpenInLastSavepoint() )
            {
                Kernel_OpWarning( csp3_bd_msg, csp3_n_obj )
                    << __FILE__ << ":" << __LINE__
                    << " bd90RollbackObj invalid_state 'obsFreeAfterEot' ignored!"
                    << " RollbackType:" << RollbackType 
                    << " NewState:" << NewState
                    << " OID=" << OId.gg92GetPno() << "." << OId.gg92GetPos()
                    << " (vers " << OId.gg92GetFrameVers() << ")";
                TrError = e_ok;
            }
            else
            {
                Kernel_OpWarning( csp3_bd_msg, csp3_n_obj )
                    << __FILE__ << ":" << __LINE__
                    << " bd90RollbackObj invalid_state 'obsFreeAfterEot' found!"
                    << " RollbackType:" << RollbackType
                    << " NewState:" << NewState
                    << " OID=" << OId.gg92GetPno() << "." << OId.gg92GetPos()
                    << " (vers " << OId.gg92GetFrameVers() << ")";
                TrError = e_wrong_object_state;
            }
            return;
        }
        break;

    default:
        /* PTS 1114818 FF 2002-03-13 */
        if ((m_new == RollbackType) &&
                (obsFree_egg00 == NewState) &&
                (obsFree_egg00 == pObjFrame->objHeader_bd900.ohdState_gg92) &&
                Kernel_IAdminInfo::Instance().KernelStateIsRedoLogProcessing() &&
                ((Log_Transaction *) Current.curr_trans->trLogContext_gg00)->WasOpenInLastSavepoint() )
        {
            Kernel_OpWarning( csp3_bd_msg, csp3_n_obj )
                << __FILE__ << ":" << __LINE__
                << " bd90RollbackObj invalid_state 'obsFree' ("
                << pObjFrame->objHeader_bd900.ohdState_gg92 << ") ignored!"
                << " RollbackType:" << RollbackType
                << " NewState:" << NewState
                << " OID=" << OId.gg92GetPno() << "." << OId.gg92GetPos()
                << " (vers " << OId.gg92GetFrameVers() << ")";
            TrError = e_ok;
        }
        else
        {
            Kernel_OpWarning( csp3_bd_msg, csp3_n_obj )
                << __FILE__ << ":" << __LINE__
                << " bd90RollbackObj invalid_state 'obsFree' ("
                << pObjFrame->objHeader_bd900.ohdState_gg92 << ") found!"
                << " RollbackType:" << RollbackType
                << " NewState:" << NewState
                << " OID=" << OId.gg92GetPno() << "." << OId.gg92GetPos()
                << " (vers " << OId.gg92GetFrameVers() << ")";
            TrError = e_wrong_object_state;
        }
        return;
    }

    if (m_lock == RollbackType) /* PTS 1109296 UH 2001-01-26 */
    {
        if ( BeforeObjSize != 0 || pBeforeObj != NULL )
        {
            TrError = e_undo_log_execute_error;
            return;
        }
    }
    else
    {
        /* rollback object body if necessary */
        switch (NewState)
        {
        case obsReserved_egg00:

            if (bPageIsForVarObj)
            {
                /* remove continuation object */
                tbd900_VarObjBodyHeader & VarObjBodyHeader = ((tbd900_VarObjBody*) (pObjFrame->objBody_bd900))->vbdHeader_bd900;

                if (PRIM_CONT_OBJ_FILE_NO_BD900 != VarObjBodyHeader.vhdContObjFileNo_bd900)
                {
                    const FileDir_FileNo ObjFileNo = Current.curr_tree_id.fileDirFileId_gg00(); 
                    FileDir_OmsVar  fd_var_smart;
                    FileDir_ReturnCode     fd_rc;
                    fd_rc = FileDir_IDirectory::SessionInstance(Trans).GetFile(
                        Trans.trTaskId_gg00, ObjFileNo, fd_var_smart, ErrorList);
                    if (FileDir_Okay != fd_rc) 
                    {
                        Kernel_OpError( csp3_bd_msg, csp3_n_obj ) 
                            << __FILE__ << ":" << __LINE__ 
                            << " ObjFileNo:" << ObjFileNo
                            << " GetFile error:" << fd_rc;                                                          
                        g01abort (csp3_bd_msg, csp3_n_obj, "bd90RollbackObj FileNotf", fd_rc);
                    }
                    bd93RemoveContObj (Current, converterVersion, fd_var_smart, OId, VarObjBodyHeader.vhdContObjFileNo_bd900,
                                       VarObjBodyHeader.vhdContObjPno_bd900, VarObjBodyHeader.vhdContObjPos_bd900);

                    /* reset pointer to continuation object */
                    VarObjBodyHeader.vhdContObjFileNo_bd900 = PRIM_CONT_OBJ_FILE_NO_BD900;
                    VarObjBodyHeader.vhdContObjPno_bd900    = NIL_PAGE_NO_GG00;
                    VarObjBodyHeader.vhdContObjPos_bd900    = NIL_OBJ_PAGE_POS_GG92;
                    VarObjBodyHeader.vhdObjSize_bd900       = 0;
                }
            }
            break;

        case obsOccupied_egg00:
            if (pObjFrame->objHeader_bd900.ohdState_gg92 != obsFreeAfterEot_egg00)
            {
                /* in case the object state is obsFreeAfterEot_egg00 there */
                /* is nothing to do since the object body is still as it   */
                /* was before the object was deleted otherwise the before  */
                /* image of the object hast to be inserted                 */

                if (bPageIsForVarObj)
                {
                    const bool        bRollBack = true;
                    tsp00_Int4        NewContObjFile;
                    Log_EntrySequence DummyRedoEntrySequence;
                    tgg91_TransNo     DummyRedoUpdTransNo;
                    tgg91_PageRef     DummyObjVers;
                    DummyObjVers.gg91SetDummyRef();

                    FileDir_ReturnCode  fd_rc;
                    FileDir_OmsVar      fd_var_smart;
                    FileDir_FileNo      FileNo = Current.curr_tree_id.fileDirFileId_gg00();
                    fd_rc = FileDir_IDirectory::SessionInstance(*Current.curr_trans).GetFile(
                        Trans.trTaskId_gg00, FileNo, fd_var_smart, ErrorList);
                    if (FileDir_Okay != fd_rc) {
                        bd900MapFileDirError(Trans, fd_rc, e_container_dropped, (char*)__FILE__,  __LINE__ );
                        return;
                    }
                    bd93UpdateVarObjBody (Current, converterVersion, fd_var_smart, OId, DummyObjVers, NptrsPage,
                        pObjFrame, BeforeObjSize, pBeforeObj, bRollBack, 
                        bTooManyEmptyPagesInContFile, NewContObjFile, ContObjFileNo, 
                        DummyRedoEntrySequence, DummyRedoUpdTransNo, contVarObject,
                        bOmsLayerReusedObj,ErrorList); 
                }
                else
                {
                    /* check before object size */
                    if (0 != BeforeObjSize || (obsOccupied_egg00 == NewState))
                    {
                        if (BeforeObjSize != tsp00_Int4(pPage->nd_obj_frame_len() - sizeof(tgg92_ObjHeader) -
                                                        pPage->nd_obj_key_len()))
                        {
                            TrError = e_illegal_object_length;
                            return;
                        }

                        SAPDB_RangeMove (__FILE__, 2,
                            BeforeObjSize,
                            PAGE_MXSP00 - sizeof (tgg00_PageCheckTrailer),
                            pBeforeObj, 1,
                            pPage,
                            OId.gg92GetPos() + POS_OFF_DIFF_BD00 + sizeof (tgg92_ObjHeader),
                            BeforeObjSize,
                            TrError);
                    }
                }
                if (TrError != e_ok) return;
            }
            break;
        }
    }

    /* update page header */
    if (obsFreeAfterEot_egg00 == pObjFrame->objHeader_bd900.ohdState_gg92)
    {
        ++pPage->nd_occ_obj_cnt();
        --pPage->nd_free_eot_obj_cnt();
    }

    if ( obsFree_egg00 == NewState ) // PTS 1113342 UH 2002-01-10 added because rollback to Free is not allowed
        NewState.becomes(obsFreeAfterEot_egg00);

    /* update object header */
    if (obsFreeAfterEot_egg00 != NewState)
    {
        pObjFrame->objHeader_bd900.ohdState_gg92                        = NewState;
        pObjFrame->objHeader_bd900.ohdTransInfo_gg92.otrBeforeRef_gg00  = BeforeObjRef;
        pObjFrame->objHeader_bd900.ohdTransInfo_gg92.otrUpdTransId_gg00 = BeforeObjUpdVers;
        /* because OMS implementation may have change an ObjectVersion and may   */
        /* have written this change to persistent data because of omsRelease(),  */
        /* rollback has to recover the original version                          */
        /* only update and delete may change a frameversion via liboms           */
        if (obsOccupied_egg00 == NewState)
            pObjFrame->objHeader_bd900.ohdFrameVersion_gg92 = OId.gg92GetFrameVers();
    }
    else
    {
        // PTS 1113342 UH 2002-01-10
        // Objects are only marked for delete if a NewObj is rollbacked and
        // not rollbacked to state obsFree_egg00.

        // Write new undo entry only for the garbage collectors so the gc can change the objects state
        // to obsFree_egg00.
        // This step is necessary to keep the lock of the object until end of transaction.
        // The flag m_drop means m_delete but the flag IsDone is true in this entry so it is
        // invisible during rollbacks.

        const int ObjBodySize = pPage->nd_obj_frame_len() - sizeof(tgg92_ObjHeader);

        kb67DelUpdObj (*Current.curr_trans, m_drop, Current.curr_tree_id,
            OId, pPage->nd_obj_key_len(),
            pObjFrame->objHeader_bd900.ohdTransInfo_gg92.otrBeforeRef_gg00,
            pObjFrame->objHeader_bd900, ObjBodySize, pObjFrame->objBody_bd900,
            contVarObject, bOmsLayerReusedObj, ErrorList);           

        pObjFrame->objHeader_bd900.ohdState_gg92.becomes(obsFreeAfterEot_egg00);

        /* update page header */
        ++pPage->nd_free_eot_obj_cnt();
        --pPage->nd_occ_obj_cnt();
    }

    // PTS 1113342 UH 2002-01-10 removed handling for NewState: obsFree_egg00
}

/*---------------------------------------------------------------------------*/

externCpp void
bd90UnlockObj (cbd05_Current    &Current,
               tgg92_KernelOid  &OId,
               bool              bSharedLock)
{
    ROUTINE_DBG_MEO00 ("bd90UnlockObj");

    tgg00_BasisError    &TrError = Current.curr_trans->trError_gg00;

    /*  get page pointer  */
    cbd910_PrimObjDataNode    NptrsPage(Current, OId.gg92GetPno(), nr_for_update);
    if (TrError != e_ok) return;

    /* determine pointer to object frame */
    BD92CHECK_OID_POSITION ("bd90UnlockObj",TrError, OId, NptrsPage);
    tbd900_ObjFrame  *pObjFrame = NptrsPage.bd910ObjFramePtr (OId.gg92GetPos());

    /*  check object version  */
    if (pObjFrame->objHeader_bd900.ohdFrameVersion_gg92 != OId.gg92GetFrameVers())
    {
        TrError = e_wrong_object_version;
        bd90_NoteVersionErrorInKnldiag
        (Unlock, OId, pObjFrame->objHeader_bd900.ohdFrameVersion_gg92);
        return;
    }

    /* check object state */
    if ((pObjFrame->objHeader_bd900.ohdState_gg92 != obsReserved_egg00   ) &&
        (pObjFrame->objHeader_bd900.ohdState_gg92 != obsKeyReserved_egg00) &&
        (pObjFrame->objHeader_bd900.ohdState_gg92 != obsOccupied_egg00   ))
    {
        TrError = e_wrong_object_state;
        return;
    }

    if (bSharedLock)
    {
        kb50UnlockShareObj (Current.curr_trans->trTaskId_gg00,
            Current.curr_trans->trWriteTransId_gg00, Current.curr_trans->trIndex_gg00,
            OId, pObjFrame->objHeader_bd900.ohdTransInfo_gg92);
    }
    else
    {
        kb50UnlockObj (Current.curr_trans->trTaskId_gg00,
            Current.curr_trans->trWriteTransId_gg00, Current.curr_trans->trIndex_gg00,
            OId, pObjFrame->objHeader_bd900.ohdTransInfo_gg92);
    }
}

/*---------------------------------------------------------------------------*/

externCpp  void 
bd90UpdateObj (cbd05_Current            &Current,
               tgg92_KernelOid          &OId,
               tgg91_PageRef            &ObjVers,
               tsp00_Int4                NewObjBodyLen,
               void                     *pNewObjBody,
               const Log_EntrySequence  &RedoEntrySequence,
               const tgg91_TransNo      &RedoUpdTransNo,
               SAPDB_Int1                contObj,             // PTS 1124278
               bool                      bOmsLayerReusedObj,  // PTS 1127661
               bool                      bForRollforward,   
               Msg_List                 &ErrorList)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd90UpdateObj", ObjContainer_Trace, 5);

    tgg00_BasisError      &TrError                      = Current.curr_trans->trError_gg00;
    tgg00_TransContext    &Trans                        = *Current.curr_trans;
    bool                   bTooManyEmptyPagesInContFile = false;
    tsp00_Int4       ContFileNo;
    tsp00_Int4             NewContFileNo;

    /*  get page pointer  */
    cbd910_PrimObjDataNode    NptrsPage (Current, OId.gg92GetPno(), nr_for_update);
    if (TrError != e_ok) return;

    FileDir_FileNo FileNo;
    bd900GetFileNo(NptrsPage.np_ptr(), FileNo);
    FileDir_Oms     fd_smart;

    if (bForRollforward)
    {
        /* no lock is aquired during rollforward */
        FileDir_ReturnCode    fd_rc;
        fd_rc = FileDir_IDirectory::SessionInstance(Trans).GetFile(
            Trans.trTaskId_gg00, FileNo, fd_smart, ErrorList);
        if (FileDir_Okay != fd_rc) {
            bd900MapFileDirError(Trans, fd_rc, TrError, (char*)__FILE__,  __LINE__ );
            return;
        };
    }
    else if ( !bd90GetFile( Trans, fd_smart,  FileNo, !cDoShareLock, __LINE__, ErrorList)) {
        return;
    }

    /* determine pointer to object frame */
    BD92CHECK_OID_POSITION ("bd90UpdateObj",TrError, OId, NptrsPage);
    tbd900_ObjFrame  *pObjFrame = NptrsPage.bd910ObjFramePtr (OId.gg92GetPos());

    tgg00_ObjFrameVers newFrameVers = OId.gg92GetFrameVers();

    /* check object version */
    if ( RedoEntrySequence.IsInvalid()
        &&
        pObjFrame->objHeader_bd900.ohdFrameVersion_gg92 != OId.gg92GetFrameVers() )
    {
        /* PTS 1125361 FF 2003-Nov-17 for omsKeyedObjects FrameVersion may have            */
        /*                            been incremented by upper layers                     */
        /* PTS 1127661 FF 2004-Apr-26 for any object upper layers may have changed         */
        /*                            the FrameVersion. This is signalld by bOmsLayerReusedObj == true */
        if ( ! bOmsLayerReusedObj )
        {
            TrError = e_wrong_object_version;
            bd90_NoteVersionErrorInKnldiag
                (Update, OId, pObjFrame->objHeader_bd900.ohdFrameVersion_gg92);
            return;
        }
        else {
            /* make sure, the right OID is written to undo/history entries */
            OId.gg92SetFrameVers(pObjFrame->objHeader_bd900.ohdFrameVersion_gg92);
        }
    }

    const bool bPageIsForVarObj = (pt2VarObject_egg00 == NptrsPage.np_ptr()->nd_pt2());

    /* update object body */
    if (bPageIsForVarObj)
    {
        Converter_Version converterVersion = 
            bd20GetDestinationConverterVersion ( Trans.trTaskId_gg00, NptrsPage.np_cbptr() );

        FileDir_OmsVar      fd_var_smart;
        fd_var_smart.Assign(fd_smart);
        const bool bNoRollBack = false;
        bd93UpdateVarObjBody (Current, converterVersion, fd_var_smart, OId, ObjVers, NptrsPage, pObjFrame, NewObjBodyLen,
                              pNewObjBody, bNoRollBack, bTooManyEmptyPagesInContFile,
                              NewContFileNo, ContFileNo, RedoEntrySequence, RedoUpdTransNo,
                              contObj, bOmsLayerReusedObj, ErrorList); 
    }
    else
    {
        bd90_UpdateObjBody (Current, OId, ObjVers, NptrsPage.np_ptr(),
                            pObjFrame,  NewObjBodyLen, pNewObjBody, RedoEntrySequence, RedoUpdTransNo,
                            contObj, bOmsLayerReusedObj, ErrorList); 
    }

    /* reset OID to original objFrameVers */ 
    OId.gg92SetFrameVers(newFrameVers);  

    if (TrError != e_ok) {
      if ((g01vtrace.vtrBdObject_gg00) || (g01vtrace.vtrOmsUpd_gg00 ))
        Kernel_VTrace() << __FILE__ << " : " << __LINE__ 
        << " UpdObj Error:" << TrError
        << " Oid=" << OId.gg92GetPno() << "." << OId.gg92GetPos()
        << " (vers " << OId.gg92GetFrameVers() << ")"
        << " ohdFrameVersion:" << pObjFrame->objHeader_bd900.ohdFrameVersion_gg92
        << " ObjVers" << ObjVers.gg91RefPno() << "." << ObjVers.gg91RefPos();
      return;
    }

    /* write OID.FrameVers into object frame */ 
    pObjFrame->objHeader_bd900.ohdFrameVersion_gg92 = newFrameVers; 

    /*  set return value of result object sequence  */
    ObjVers = pObjFrame->objHeader_bd900.ohdTransInfo_gg92.otrBeforeRef_gg00;

    /*  update object frame header */
    pObjFrame->objHeader_bd900.ohdState_gg92.becomes(obsOccupied_egg00);

	if (bForRollforward && (NptrsPage.np_ptr()->nd_objpage_state().includes(objp_no_chain_to_freelist)))
	{
		NptrsPage.np_ptr()->nd_objpage_state().delElement(objp_no_chain_to_freelist);
		if (NptrsPage.bd910PageMustBeInsertedIntoFreeChain())
		{
			cbd910_SubRootNode  NptrsRoot (Current, nr_for_update);
			if (NptrsPage.np_ptr()->nd_root() == NptrsPage.np_ptr()->nd_id())
				NptrsRoot.bd910SetTo (NptrsPage);
			else
				NptrsRoot.bd910AssignToSubroot (NptrsPage);
			Converter_Version converterVersion = 
				bd20GetDestinationConverterVersion ( Trans.trTaskId_gg00, NptrsPage.np_cbptr() );
			NptrsRoot.bd910InsertPageIntoFreeChain (NptrsPage, converterVersion, fd_smart); 
		}
	}
}

/*===========================================================================*
 *  LOCAL FUNCTIONS (CODE)                                                   *
 *===========================================================================*/


/*---------------------------------------------------------------------------*/
/* PTS 1111856 FF 2002-06-05 logic has been completely recoded               */
static _INLINE void
bd90_DelAllObjOnThisPage (cbd05_Current           &Current,
                          FileDir_Oms         &fd_smart,
                          cbd910_PrimObjDataNode  &NptrsPage,
                          tsp00_Int4              &NumDelObj,
                          tgg92_KernelOid         &OId,      
                          Msg_List                &ErrorList)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd90_DelAllObjOnThisPage", ObjContainer_Trace, 5);

    tgg00_BasisError    &TrError            = Current.curr_trans->trError_gg00;
    tbd_nodeptr         &pPage              = NptrsPage.np_ptr();
    tsp00_Uint2         &ObjFrameLen        = pPage->nd_obj_frame_len();

    const int            AlignedObjFrameLen = BD92ALIGN_OBJ_FRAME (ObjFrameLen);
    tgg00_ObjPagePos     ObjPos;
    tbd900_ObjFrame     *pObjFrame;
    // tgg92_KernelOid    OId;        PTS 1116441
    tsp00_Int4           ObjBodySize;
    tsp00_Bool           bNextObjIsFound = false;
    tsp00_Int4           LogReadAccesses = 0;
    tsp00_Int4           ExtObjBodySize = 0;
    void                *pExtObjBody = NULL;
    tgg91_PageRef        WantedObjVers;
    
    /* go trough all objects on this page */
    for (ObjPos = pPage->nd_first_obj();
        ((ObjPos + ObjFrameLen) <= BODY_END_BD00) && (e_ok == TrError);
        ObjPos += AlignedObjFrameLen)
    {           
        /* create OId for current object */
        OId.gg92SetPno (NptrsPage.np_ptr()->nd_id());
        OId.gg92SetPos (ObjPos);

        const bool          bWithObject        = false;
		const bool  		bOnlyHdrVarObjects = true;    

        tgg91_PageRef       ObjHistRef;              // unused parameter PTS 1120478 FF
        bd90_GetConsistentObjImage (Current, fd_smart, OId, WantedObjVers, ObjHistRef, bWithObject, ! bOnlyHdrVarObjects,
			ExtObjBodySize, pExtObjBody, NptrsPage, ObjBodySize, bNextObjIsFound, LogReadAccesses, ErrorList);
        if (bNextObjIsFound && (TrError == e_ok) && ( LogReadAccesses > 0))
          TrError = e_object_dirty;
        if ((bNextObjIsFound ) && (TrError == e_ok))
        {            
            tsp00_Uint2 AuxReqTimeout = Current.curr_trans->trObjReqTimeout_gg00; //save
            Current.curr_trans->trObjReqTimeout_gg00 = 0;                         // set nowait;
            pObjFrame = NptrsPage.bd910ObjFramePtr(ObjPos);

            /* check if kb67LockObj has alread be done by bd90_GetConsistentObjImage PTS 1120036 FF 2003-JAN-16 */
            if ((pObjFrame->objHeader_bd900.ohdState_gg92 == obsOccupied_egg00    )    ||
                (pObjFrame->objHeader_bd900.ohdState_gg92 == obsFreeAfterEot_egg00)    ||
                ((pObjFrame->objHeader_bd900.ohdState_gg92 == obsKeyReserved_egg00) &&
                !pObjFrame->objHeader_bd900.ohdTransInfo_gg92.otrBeforeRef_gg00.gg91IsNilRef()))
            {
                kb67LockObj (*Current.curr_trans, Current.curr_tree_id, OId, WantedObjVers,
                pObjFrame->objHeader_bd900.ohdState_gg92,
                pObjFrame->objHeader_bd900.ohdTransInfo_gg92);
            }
            else
            {
                /* PTS 1120036 FF 2003-JAN-16 */
                WantedObjVers = pObjFrame->objHeader_bd900.ohdTransInfo_gg92.otrBeforeRef_gg00;
            }
                        
            Current.curr_trans->trObjReqTimeout_gg00 = AuxReqTimeout;             // restore timeout

            if (TrError == e_ok)
            {
                /* check if object is locked and create a log entry  */
                const bool bPageIsForVarObj = (pt2VarObject_egg00 == NptrsPage.np_ptr()->nd_pt2());
                const bool bOmsLayerReusedObj = false;     /* PTS 1127661 FF */    
                tbd900_VarObjBodyHeader *pHdr = &(((tbd900_VarObjBody*) pObjFrame->objBody_bd900)->vbdHeader_bd900);
                SAPDB_Int1 contVarObj = tbd900_VarObjBodyHeader::VarPrimUndefType ; 

                if (bPageIsForVarObj) 
                {
                    tbd900_VarObjBodyHeader *pHdr = &(((tbd900_VarObjBody*) pObjFrame->objBody_bd900)->vbdHeader_bd900);
                    contVarObj = pHdr->vhdContObjFileNo_bd900; 
                    SAPDB_Int1 contVarObj = pHdr->vhdContObjFileNo_bd900; 
                    bd93LogDelVarObj (Current, OId, WantedObjVers, NptrsPage.np_ptr(), pObjFrame, 
                        contVarObj, bOmsLayerReusedObj, ErrorList);    
                }
                else
                {
                    /* calculate ObjBodySize PTS 1120036 FF 2003-JAN-16 */
                    const int ObjBodySize = NptrsPage.np_ptr()->nd_obj_frame_len() - sizeof(tgg92_ObjHeader);
                    kb67DelUpdObj (*Current.curr_trans, m_delete, Current.curr_tree_id,
                                   OId, pPage->nd_obj_key_len(), WantedObjVers,
                                   pObjFrame->objHeader_bd900, ObjBodySize, pObjFrame->objBody_bd900,
                                   contVarObj, bOmsLayerReusedObj,ErrorList );            
                }
                if (TrError == e_ok)
                {
                    /* increase counter of deleted objects */
                    ++NumDelObj;

                    /* mark the object as deleted  */
                    pObjFrame->objHeader_bd900.ohdState_gg92.becomes(obsFreeAfterEot_egg00);

                    /* update page header */
                    ++pPage->nd_free_eot_obj_cnt();
                    --pPage->nd_occ_obj_cnt();

                    FileDir_FileNo  objFileNo(Current.curr_tree_id.fileDirFileId_gg00());

                    kb67WriteDeleteObjectAfterImage ( *Current.curr_trans,
                                                      objFileNo,
                                                      OId,
                                                      WantedObjVers, 
                                                      contVarObj, 
                                                      bOmsLayerReusedObj,
                                                      ErrorList);    
                }
            }
        }
        if (TrError != e_ok) break;
    }
}

/*---------------------------------------------------------------------------*/

static _INLINE void
bd90_GetObjBody   (
    tgg00_TransContext &Trans,
    tgg92_KernelOid     OId,
    tbd_nodeptr         pPage,
    tsp00_Int4          ExtObjBodySize,
    void               *pExtObjBody,
    tbd900_ObjFrame     *pObjFrame
)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd90_GetObjBody", ObjContainer_Trace, 5);

    /* this function copiess the object body identified by      */
    /* the OId at the position given by the pointer pExtObjBody */

    tgg00_BasisError    &TrError    = Trans.trError_gg00;
    const tsp00_Uint2   KeyLen      = pPage->nd_obj_key_len();
    const tsp00_Int4    ObjBodySize = pPage->nd_obj_frame_len() - sizeof(tgg92_ObjHeader);

    /* check if the given memory is large enough to accomodate the object */
    if (ExtObjBodySize < ObjBodySize)
    {
        Kernel_OpWarning( csp3_bd_msg, csp3_n_obj )
            << __FILE__ << ":" << __LINE__  
            << " wrong obj body size: OMS application size:" << ExtObjBodySize
            << " internal size:" << ObjBodySize
            << " PNo:" <<  pPage->nd_id() << " (page dumped to file:" << pPage->nd_id() << "d)"
            << " OID=" << OId.gg92GetPno() << "." << OId.gg92GetPos()
            << " (vers " << OId.gg92GetFrameVers() << ")";
        b06dump_bad_page (Trans.trTaskId_gg00,'d', ".obj", pPage->nd_id(), (void *) pPage, 1); //PTS 1107471 AK 09/08/2000
        TrError = e_wrong_object_version;  //e_buffer_limit PTS 1114891 FF 2002-03-15 ;
        return;
    }

    if (pObjFrame->objHeader_bd900.ohdState_gg92 == obsOccupied_egg00)
    {
        /* copy object */
        SAPDB_RangeMove (__FILE__, 3,
            PAGE_MXSP00 - sizeof (tgg00_PageCheckTrailer),
            ExtObjBodySize,
            pPage,
            OId.gg92GetPos()+ POS_OFF_DIFF_BD00 + sizeof (tgg92_ObjHeader),
            pExtObjBody,         1,
            ObjBodySize,
            TrError);
    }
    else
    {
        if (KeyLen > 0)
        {
            /* copy key only */
            SAPDB_RangeMove (__FILE__, 4,
                PAGE_MXSP00 - sizeof (tgg00_PageCheckTrailer),
                ExtObjBodySize,
                pPage,
                OId.gg92GetPos() + POS_OFF_DIFF_BD00 + sizeof (tgg92_ObjHeader) + ObjBodySize - KeyLen,
                pExtObjBody, 1 + ObjBodySize - KeyLen,
                KeyLen,
                TrError);

            if (TrError != e_ok) {return;}
        }

        /* fill the remaining object body with NIL_OBJ_FILLCHAR_BD900 */
        SAPDB_RangeFill (__FILE__, 5,
                ExtObjBodySize, pExtObjBody, 1, ObjBodySize - KeyLen, NIL_OBJ_FILLCHAR_BD900, TrError);
    }

#   if COMPILEMODE_MEO00 >= SLOW_MEO00
    if (e_ok == TrError) t01moveobj (bi, pExtObjBody, 1, ObjBodySize);
#   endif   

}

/*---------------------------------------------------------------------------*/

static void
bd90_NewObjFromThisPage (cbd05_Current            &Current,
                         cbd910_PrimObjDataNode   &NptrsPage,
                         tsp00_Int4                KeyLen,
                         void                     *pKey,
                         SAPDB_Int1                ContObj,
                         tgg92_KernelOid          &OId,
                         tgg91_PageRef            &ResultObjVers,
                         Msg_List                 &ErrorList)

{
    ROUTINE_DBG_MEO00 ("bd90_NewObjFromThisPage");


    /*  This routine finds the OId on a page presuming that at least one free  */
    /*  object frame exists. Moreover all pointers are restored corresponding  */
    /*  to changing the state of the frame whichs OId is returned              */

    tgg00_BasisError    &TrError          = Current.curr_trans->trError_gg00;
    tbd_nodeptr         &pPage            = NptrsPage.np_ptr();
    tsp00_Int4           ObjBodySize;
    const bool           bPageIsForVarObj = (pt2VarObject_egg00 == pPage->nd_pt2());

    /* construct position of the object frame  header */
    tbd900_ObjFrame  *pObjFrame = NptrsPage.bd910ObjFramePtr (pPage->nd_first_free_obj());

    /* check if the object frame is really free    PTS 1127804 FF 2004-FEB-17 */
    /* (PTS 1134505) check if the position of the first free obj is valid     */
    /* and                                                                    */                                                                    
    /* check if the object frame is really free                               */
    if ( !bd92ObjPosIsValid (pPage->nd_first_free_obj(), pPage->nd_first_obj(), pPage->nd_obj_frame_len())
        ||
        (obsFree_egg00 != pObjFrame->objHeader_bd900.ohdState_gg92)) 
    {
        Kernel_OpError ( csp3_bd_msg, csp3_n_obj ) << __FILE__ << ":" << __LINE__
            << " wrong position: " << pPage->nd_first_free_obj()
            << " or objectframe in freelist not marked as 'obsFree_egg00'"
            << " page:" << pPage->nd_id()
            << " first free pos: " << pPage->nd_first_free_obj()
            << " state:" << pObjFrame->objHeader_bd900.ohdState_gg92
            << " page dumped into: d" 
            << pPage->nd_id() << ".obj"; 

        b06dump_bad_page (Current.curr_trans->trTaskId_gg00,'d', ".obj", pPage->nd_id(), 
            (void *) NptrsPage.np_ptr(), 1);
        if ( bd92ObjPosIsValid (pPage->nd_first_free_obj(), pPage->nd_first_obj(), pPage->nd_obj_frame_len()))
            TrError = e_wrong_object_state;
        else
            TrError = e_illegal_object_pos;
        return;
    }

    /* construct new OId   */
    OId.gg92Set (
        pPage->nd_id(),
        pPage->nd_first_free_obj(),
        MIN_OBJ_FRAME_VERS_GG92);

    if (pObjFrame->objHeader_bd900.ohdFrameVersion_gg92 < MAX_OBJ_FRAME_VERS_GG92)
        OId.gg92SetFrameVers (pObjFrame->objHeader_bd900.ohdFrameVersion_gg92 + 1);
    else
        OId.gg92SetFrameVers (MIN_OBJ_FRAME_VERS_GG92);

    /* determine objbodysize */
    if (bPageIsForVarObj)
        ObjBodySize = pPage->nd_obj_frame_len() - sizeof(tgg92_ObjHeader) - sizeof (tbd900_VarObjBodyHeader);
    else
        ObjBodySize = pPage->nd_obj_frame_len() - sizeof(tgg92_ObjHeader);

    /* check if the KeyLen corresponds to the KeyLen stored in the page header */
    SAPDBERR_ASSERT_STATE (pPage->nd_obj_key_len() == KeyLen);

    /* insert key into object body */
    if  (KeyLen > 0)
    {
        SAPDB_RangeMove (__FILE__, 6,
            KeyLen, ObjBodySize,
            pKey, 1, &(pObjFrame->objBody_bd900), ObjBodySize - KeyLen + 1,
            KeyLen, Current.curr_trans->trError_gg00);
        if (TrError != e_ok) {return;}

        // PTS 1134645
        pObjFrame->objHeader_bd900.ohdTransInfo_gg92.otrUpdTransId_gg00.gg90SetNil();
    }

    // new unused object ==> lock collision impossible ==> set nowait to avoid any kind of request
    tsp00_Uint2 AuxReqTimeout = Current.curr_trans->trObjReqTimeout_gg00; // save timeout
    Current.curr_trans->trObjReqTimeout_gg00 = 0; // set nowait

    /* put current lock id into object frame header and create a log entry   */
    kb67NewObj (*Current.curr_trans, Current.curr_tree_id, OId, pObjFrame->objHeader_bd900,
                ObjBodySize, KeyLen, ContObj, ErrorList);

    Current.curr_trans->trObjReqTimeout_gg00 = AuxReqTimeout; // restore timeout

    if (TrError != e_ok) return;

    // necessary for NewHashKey
    ResultObjVers = pObjFrame->objHeader_bd900.ohdTransInfo_gg92.otrBeforeRef_gg00;

    /* update page header */
    -- pPage->nd_free_obj_cnt();
    ++ pPage->nd_occ_obj_cnt();
    pPage->nd_first_free_obj() = pObjFrame->objHeader_bd900.ohdNextFreeObj_gg92;

    /* set the object frame header to the state reserved and initialize the  body */
    pObjFrame->bd900SetToReservedFrame (pPage->nd_obj_frame_len(), KeyLen, OId.gg92GetFrameVers(), bPageIsForVarObj);
    if (bPageIsForVarObj)
    {
        tbd900_VarObjBodyHeader *pHdr = &(((tbd900_VarObjBody*) pObjFrame->objBody_bd900)->vbdHeader_bd900);
        pHdr->vhdObjectIsCont_bd900 = ContObj;
    }
};

/*---------------------------------------------------------------------------*/

externCpp void
bd90NextObjConsistentOnThisPage (cbd05_Current           &Current,
                                  FileDir_Oms         &fd_smart,
                                  tgg92_KernelOid         &OId,
                                  tgg91_PageRef           &ResultObjVers,
                                  tgg91_PageRef           &ObjHistRef,             // PTS 1120478 FF
                                  bool                     bWithObject,
                                  bool                     bOnlyHdrVarObjects,     // used for omsVarObjIter
                                  tsp00_Int4               ExtObjBodySize,
                                  void                    *pExtObjBody,
                                  cbd910_PrimObjDataNode  &NptrsPage,
                                  tsp00_Int4              &ObjBodySize,             //[out=length of the object]
                                  tsp00_Bool              &bNextObjIsFound,
                                  tsp00_Int4              &LogReadAccessesPerPage,  //[out] PTS 1107819 AK 12/09/2000
                                  Msg_List                &ErrorList)
{
    ROUTINE_DBG_MEO00 ("bd90NextObjConsistentOnThisPage");

    bNextObjIsFound = false;
    LogReadAccessesPerPage = 0; // PTS 1107819 AK 12/09/2000

    tgg00_ObjPagePos   ObjPos;
    tsp00_Int4         ObjPosBeg;
    tsp00_Int4         LogReadAccesses ;
    const tsp00_Int4   ObjFrameLen  = NptrsPage.np_ptr()->nd_obj_frame_len();
    tgg00_BasisError  &TrError      = Current.curr_trans->trError_gg00;

    ObjHistRef.gg91SetNilRef(); // PTS 1120478 FF

    /* find starting object position for search */
    const int AlignedObjFrameLen = BD92ALIGN_OBJ_FRAME(ObjFrameLen);

    if ( ! OId.gg92IsNil() )
        ObjPosBeg = OId.gg92GetPos() + AlignedObjFrameLen;
    else
    {
        /* return immediately if there are no objects on this page. PTS 1107750 AK 05/09/2000 */
        if (NptrsPage.np_ptr()->nd_max_obj_cnt() == NptrsPage.np_ptr()->nd_free_obj_cnt())
            return;

        ObjPosBeg = NptrsPage.np_ptr()->nd_first_obj();
    }

    /*  start search */
    for ( ObjPos = ObjPosBeg;
            ((ObjPos + ObjFrameLen) <= BODY_END_BD00) && (!bNextObjIsFound) && (e_ok == TrError);
            ObjPos += AlignedObjFrameLen )
    {
        /* create OId for current object */
        OId.gg92SetPno (NptrsPage.np_ptr()->nd_id());
        OId.gg92SetPos (ObjPos);

        bd90_GetConsistentObjImage (Current, fd_smart, OId, ResultObjVers, ObjHistRef, bWithObject, bOnlyHdrVarObjects, 
			ExtObjBodySize, pExtObjBody, NptrsPage, ObjBodySize, bNextObjIsFound, LogReadAccesses, ErrorList);

        LogReadAccessesPerPage += LogReadAccesses;
    }
}

/*---------------------------------------------------------------------------*/

static _INLINE void
bd90_NoteVersionErrorInKnldiag (
    ebd90_FunctionName FunctionName,
    tgg92_KernelOid    WantedOId,
    tgg00_ObjFrameVers ExpectedVersion
)
{
#if COMPILEMODE_MEO00 >= SLOW_MEO00 

    ROUTINE_DBG_MEO00 ("bd90_NoteVersionErrorInKnldiag");

    /* first line */
    {
        char Message[] = bsp_c40 ;
        int Len        = 0;

        switch (FunctionName)
        {
        case Delete     : g17nameto_line ("Del:              ",Len,Message);break;
        case Get        : g17nameto_line ("Get:              ",Len,Message);break;
        case GetCons    : g17nameto_line ("GetCons:          ",Len,Message);break;
        case KeyGet     : g17nameto_line ("KeyGet:           ",Len,Message);break;
        case KeyGetCons : g17nameto_line ("KeyGetCons:       ",Len,Message);break;
        case Lock       : g17nameto_line ("Lock:             ",Len,Message);break;
        case Reuse      : g17nameto_line ("Reuse:            ",Len,Message);break;
        case Unlock     : g17nameto_line ("Unlock:           ",Len,Message);break;
        case Update     : g17nameto_line ("Update:           ",Len,Message);break;
        default         : g17nameto_line ("OMS:              ",Len,Message);break;
        }

        ++Len;
        g17nameto_line ("Wrong Vers=         ", Len, Message);
        g17trimint4_to_line (WantedOId.gg92GetFrameVers(), Len, Message);
        ++Len;
        g17nameto_line ("Corr Vers=          ", Len, Message);
        g17trimint4_to_line (ExpectedVersion, Len, Message);
        g01optextmsg (sp3p_knldiag, sp3m_error, csp3_bd_msg, csp3_n_obj, Message);
    }

    /* second line */
    {
        char Message[] = bsp_c40 ;
        int Len        = 0;

        g17nameto_line ("Wanted OId :        ", Len, Message);
        ++Len;
        WantedOId.gg92AppendToLine (Len, *(tsp00_Line*) &Message);

        g01optextmsg (sp3p_knldiag, sp3m_error, csp3_bd_msg, csp3_n_obj, Message);
    }
# endif
}

/*---------------------------------------------------------------------------*/
/* PTS 1111513 13/12/2001 */
static void
bd90_ReadAheadFile (tgg00_TransContext &Trans,
                    cbd910_ObjNode     &NptrsPage)
{
    tgg00_BasisError        &TrError         = Trans.trError_gg00;
    const tsp00_Int4         c100_Percent    = 1000;
    const tsp00_Int4         c99Dot9_Percent =  999;
    const tsp00_Int4         MaxNumPagesToReadTillNextCheck = 20;
    tsp00_Int4               NumPagesToReadTillNextCheck    = 0;
    tsp00_Int4               DataCacheSize;
    tsp00_Int4               OmsHistoryPageCnt;
    tsp00_Int4               OmsDataPageCnt;
    tsp00_Int4               OmsUnloadedVersPageCnt;
    tsp00_Int4               SqlDataPageCnt;
    double                   DataCacheOccupancy;  // tsp00_Int4 PTS 1114868 FF 202-03-15
    double                   DataCacheSize_double;

    while ((e_ok == TrError) && NptrsPage.bd910PageIsAvailable())
    {
        /* Stop reading if the cache is completely filled; PTS 1107820 AK 21/09/2000 */
        if (NumPagesToReadTillNextCheck <= 0)
        {
            NumPagesToReadTillNextCheck = MaxNumPagesToReadTillNextCheck;

            /* get data cache occupation */
            /* PTS 1111572 E.Z. */

            bd20GetWorkloadCounter (DataCacheSize, OmsHistoryPageCnt, 
                OmsDataPageCnt, OmsUnloadedVersPageCnt,SqlDataPageCnt);

            DataCacheOccupancy = OmsHistoryPageCnt + OmsDataPageCnt + OmsUnloadedVersPageCnt +SqlDataPageCnt;
            DataCacheSize_double = DataCacheSize;           // PTS 1114868 FF 202-03-15

            /* if the datacache is too full not only a cheap but not accurate check       */
            /* must be peforemed to decide  if an object can be released but an exact one */
            if  (DataCacheSize_double * c99Dot9_Percent < DataCacheOccupancy * c100_Percent)
                break;
        }
        /* move to next page, i.e. read next page into the data cache */
        NptrsPage.bd910MoveToNextPage();

        --NumPagesToReadTillNextCheck;
    }
}

/*---------------------------------------------------------------------------*/

static void
bd90_UpdateObjBody (cbd05_Current            &Current,
                    tgg92_KernelOid           OId,
                    tgg91_PageRef             ObjVers,
                    tbd_nodeptr               pPage,
                    tbd900_ObjFrame          *pObjFrame,
                    tsp00_Int4                NewObjBodyLen,
                    void                     *pNewObjBody,
                    const Log_EntrySequence  &RedoEntrySequence,
                    const tgg91_TransNo      &RedoUpdTransNo,
                    SAPDB_Int1                contVarObj,   
                    bool                      bOmsLayerReusedObj,
                    Msg_List                 &ErrorList)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd90_UpdateObjBody", ObjContainer_Trace, 5);

    /* This function moves the object given by the pointer  */
    /* pNewObjBody into the object frame given by the OId   */

    tgg00_BasisError    &TrError = Current.curr_trans->trError_gg00;

    /* check object state */
    if ((pObjFrame->objHeader_bd900.ohdState_gg92 != obsReserved_egg00   ) &&
            (pObjFrame->objHeader_bd900.ohdState_gg92 != obsKeyReserved_egg00) &&
            (pObjFrame->objHeader_bd900.ohdState_gg92 != obsOccupied_egg00   ))
    {
        TrError = e_wrong_object_state;
        return;
    }

    /*  check if the NewObjectBody fits into the ObjectFrame */
    tsp00_Int4  ObjBodySize = pPage->nd_obj_frame_len() - sizeof(tgg92_ObjHeader);
    if (NewObjBodyLen != ObjBodySize)
    {
        TrError = e_illegal_object_length;
        return;
    }

    /* update object transinfo and write a log entry */
    kb67UpdObj (*Current.curr_trans, Current.curr_tree_id,
                OId, pPage->nd_obj_key_len(), ObjVers,
                pObjFrame->objHeader_bd900, ObjBodySize,
                pObjFrame->objBody_bd900, RedoEntrySequence, RedoUpdTransNo,
                contVarObj,
                bOmsLayerReusedObj, ErrorList );  
    if (TrError != e_ok) {return;}

    /*  insert new object body into the object frame */
    SAPDB_RangeMove (__FILE__, 7,
        ObjBodySize,
        PAGE_MXSP00 - sizeof (tgg00_PageCheckTrailer),
        pNewObjBody,         1,
        pPage,
        OId.gg92GetPos() + POS_OFF_DIFF_BD00 + sizeof (tgg92_ObjHeader),
        NewObjBodyLen - pPage->nd_obj_key_len(), // no key update
        TrError);
}

/*---------------------------------------------------------------------------*/

static _INLINE tsp00_Int4
bd90_VarObjBodySize (tbd900_ObjFrame *pObjFrame)
{
    ROUTINE_DBG_MEO00 ("bd90_VarObjBodySize");

    tbd900_VarObjBody       &VarObjBody = *((tbd900_VarObjBody*) (pObjFrame->objBody_bd900));
    return VarObjBody.vbdHeader_bd900.vhdObjSize_bd900;
};

/*---------------------------------------------------------------------------*/

static _INLINE tsp00_Uint1 
bd90_VarObjPrimContObjType(tbd900_ObjFrame *pObjFrame)
{
    ROUTINE_DBG_MEO00 ("bd90_VarObjPrimContObjType");

    tbd900_VarObjBody       &VarObjBody = *((tbd900_VarObjBody*) (pObjFrame->objBody_bd900));
    return VarObjBody.vbdHeader_bd900.vhdObjectIsCont_bd900;

};

/*---------------------------------------------------------------------------*/

static void
bd90_GetConsistentObjImage (cbd05_Current           &Current,             //[in/out]
                            FileDir_Oms         &fd_smart,
                            tgg92_KernelOid         &OId,                 //[in/out] frame version may change
                            tgg91_PageRef           &ResultObjVers,
                            tgg91_PageRef           &ObjHistRef,          // PTS 1120478 FF
                            tsp00_Bool               bWithObject,
                            bool                     bOnlyHdrVarObjects,  // used for omsVarObjIter
                            tsp00_Int4               ExtObjBodySize,
                            void                    *pExtObjBody,
                            cbd910_PrimObjDataNode  &NptrsPage,
                            tsp00_Int4              &ObjBodySize,         //[out=length of the object]
                            tsp00_Bool              &bConsistentObjImageObjFound,
                            tsp00_Int4              &LogReadAccesses,
                            Msg_List                &ErrorList)
{
    ROUTINE_DBG_MEO00 ("bd90_GetConsistentObjImage");

    bConsistentObjImageObjFound = false;

    tgg00_ObjPagePos          ObjPos =  OId.gg92GetPos();
    tsp00_Int4                ObjKeyLen        = NptrsPage.np_ptr()->nd_obj_key_len();
    tgg00_BasisError         &TrError          = Current.curr_trans->trError_gg00;
    const bool                bPageIsForVarObj = (pt2VarObject_egg00 == NptrsPage.np_ptr()->nd_pt2());
    LogReadAccesses = 0;

    ObjHistRef.gg91SetNilRef(); // PTS 1120478 FF

    /*  get pointer to the current object frame  */
#   if COMPILEMODE_MEO00 >= QUICK_MEO00 
    BD92CHECK_OBJ_POSITION ("bd90_UpdateObjBody",TrError, ObjPos, NptrsPage);
#   endif       
    tbd900_ObjFrame* pObjFrame = NptrsPage.bd910ObjFramePtr (ObjPos);

    /* continue if the frame does not accomodate an object, PTS 1107750 AK 05/09/2000 */
    if (pObjFrame->objHeader_bd900.ohdState_gg92 == obsFree_egg00)
        return;

    /*  check current object frame and get a consistent image (if possible) */
    if ((pObjFrame->objHeader_bd900.ohdState_gg92 == obsOccupied_egg00    )    ||
            (pObjFrame->objHeader_bd900.ohdState_gg92 == obsFreeAfterEot_egg00)    ||
            ((pObjFrame->objHeader_bd900.ohdState_gg92 == obsKeyReserved_egg00) &&
             !pObjFrame->objHeader_bd900.ohdTransInfo_gg92.otrBeforeRef_gg00.gg91IsNilRef())) /* PTS 1001768 Torsten Strahl 1998-11-30 */
    {
        /* get object body length */
        if (bPageIsForVarObj)
		{
            ObjBodySize = bd90_VarObjBodySize (pObjFrame);
  		    switch (bd90_VarObjPrimContObjType(pObjFrame))
			{
			case tbd900_VarObjBodyHeader::VarPrimHeadObj:
				break;
			case tbd900_VarObjBodyHeader::VarPrimContObj:
        if (bOnlyHdrVarObjects){
          bConsistentObjImageObjFound = false;
          return;  
        }
        break;
			default:
				TrError = e_invalid_iterator;
				return;
			}
		}
        else
            ObjBodySize = NptrsPage.np_ptr()->nd_obj_frame_len() - sizeof(tgg92_ObjHeader);

        /* move object and key into given array assigned by pExtObjBody */
        if (bWithObject)
        {
            if (bPageIsForVarObj)  
            {
                FileDir_OmsVar   fd_var_smart;
                fd_var_smart.Assign(fd_smart);
                bd93GetVarObjBody (Current, fd_var_smart, OId, ExtObjBodySize, pExtObjBody, pObjFrame);
            }
            else
                bd90_GetObjBody (*Current.curr_trans, OId, NptrsPage.np_ptr(),
                                 ExtObjBodySize, pExtObjBody, pObjFrame);
            if (TrError != e_ok) {return;}
        }

        /* initialize the result obj frame version */
        tgg00_ObjFrameVers ResultFrameVers = pObjFrame->objHeader_bd900.ohdFrameVersion_gg92;

        /*  the current object may be inconsistent with the wanted view        */
        /*  which consists of objects commited before the current              */
        /*  transaction had been started in that case the old object image has */
        /*  to be restored from the log.                                       */
        tgg91_TransNo         DummyUpdTransId;
        kb67GetObjConsistent (*Current.curr_trans, OId,
                              pObjFrame->objHeader_bd900.ohdState_gg92,
                              pObjFrame->objHeader_bd900.ohdTransInfo_gg92,
                              bWithObject, ObjKeyLen, ObjBodySize, ExtObjBodySize, pExtObjBody,
                              ResultFrameVers,
                              ResultObjVers, 
                              ObjHistRef,                       // PTS 1120478 FF   
                              DummyUpdTransId,
                              LogReadAccesses,
                              ErrorList);  
        /* evaluate error message from kb67GetObjConsistent */
        switch (TrError)
        {
        case e_ok:
            /* PTS 1120322 FF 2003-Feb-07*/
            if ((pObjFrame->objHeader_bd900.ohdState_gg92 == obsKeyReserved_egg00) &&
                (LogReadAccesses == 0) &&
                (pObjFrame->objHeader_bd900.ohdTransInfo_gg92.otrUpdTransId_gg00 != 
                Current.curr_trans->trWriteTransId_gg00))
            {
                if ( 0 == g_Committed_obsReserved_Cnt )                    
                {                                                                       
                    g_Committed_obsReserved_Cnt++;
                    Kernel_OpError( csp3_bd_msg, csp3_n_obj ) 
                        << "Committed object frame without stored data ( state: obsReserved ) found";
                }
                Kernel_OpWarning  opWarnMsg( csp3_bd_msg, csp3_n_obj );   /* PTS 1128934 FF */
                opWarnMsg << "bd90GetObjConsistent: object within state:obsKeyReserved_egg00"
                    << " read, but not created by this trans!" 
                    << " OID=" << OId.gg92GetPno() << "." << OId.gg92GetPos()
                    << " (vers " << OId.gg92GetFrameVers() << ")"
                    << " (page dumped to file d" 
                    << OId.gg92GetPno() <<".obj)"; 
                b06dump_bad_page (Current.curr_trans->trTaskId_gg00,'d', ".obj", NptrsPage.np_ptr()->nd_id(), 
                    (void *) NptrsPage.np_ptr(), 1);
            }
            else
            {
                /*  a consistent image was found  */
                bConsistentObjImageObjFound = true;
                OId.gg92SetFrameVers (ResultFrameVers);  // PTS 1001332 Alexander Kley 31.7.98
            }
            break;

        case e_object_not_found:
            /*  there is no consistent image of this object the */
            /*  search for the next object has to be continued  */
            /*  reset error to e_ok                             */
            TrError = e_ok;
            break;

        default :
          if ((g01vtrace.vtrBdObject_gg00) || (g01vtrace.vtrOmsGet_gg00 ))
                Kernel_VTrace() << "bd90_GetConsistentObjImage:kb67GetObjConsistent Error:" << TrError;
            /* a real error occured */
            break;
        }
    }
    else  // T.A.
    {
        /* now the frame state must be either obsReserved_egg00 or obsKeyReserved_egg00 */
        SAPDBERR_ASSERT_STATE ((pObjFrame->objHeader_bd900.ohdState_gg92 == obsReserved_egg00) ||
                               (pObjFrame->objHeader_bd900.ohdState_gg92 == obsKeyReserved_egg00))

        /* check if new object has been created by current transaction */
        tsp00_Uint2 AuxReqTimeout = Current.curr_trans->trObjReqTimeout_gg00; //save
        Current.curr_trans->trObjReqTimeout_gg00 = 0; // set nowait;

        // check that the update transid corresponds to the own transid (see PTS 1111149)
        kb67LockObj(*Current.curr_trans, Current.curr_tree_id, OId,
                    pObjFrame->objHeader_bd900.ohdTransInfo_gg92.otrBeforeRef_gg00,
                    pObjFrame->objHeader_bd900.ohdState_gg92,
                    pObjFrame->objHeader_bd900.ohdTransInfo_gg92);

        Current.curr_trans->trObjReqTimeout_gg00 = AuxReqTimeout; // restore timeout

        if (e_ok == TrError)
        {
            /* PTS 1120322 FF 2003-Feb-07*/
            if (pObjFrame->objHeader_bd900.ohdTransInfo_gg92.otrUpdTransId_gg00 != 
                Current.curr_trans->trWriteTransId_gg00)
            {
                if ( 0 == g_Committed_obsReserved_Cnt )                    
                {                                                                       
                    g_Committed_obsReserved_Cnt++;
                    Kernel_OpError( csp3_bd_msg, csp3_n_obj ) 
                        << "Committed object frame without stored data ( state: obsReserved ) found";
                }
                Kernel_OpWarning  opWarnMsg( csp3_bd_msg, csp3_n_obj );   /* PTS 1128934 FF */
                opWarnMsg << "bd90GetObjConsistent: object within state:"; 
                if (pObjFrame->objHeader_bd900.ohdState_gg92 == obsReserved_egg00)
                    opWarnMsg << "obsReserved_egg00"; 
                else
                    opWarnMsg << "obsKeyReserved_egg00"; 
                opWarnMsg << " read, but not created by this trans!" 
                    << " OID=" << OId.gg92GetPno() << "." << OId.gg92GetPos()
                    << " (vers " << OId.gg92GetFrameVers() << ")"
                    << " (page dumped to file d" 
                    << OId.gg92GetPno() <<".obj)"; 
                b06dump_bad_page (Current.curr_trans->trTaskId_gg00,'d', ".obj", 
                    NptrsPage.np_ptr()->nd_id(), (void *) NptrsPage.np_ptr(), 1);
            }
            else
            {
                /*  object has been created by current transaction, return OId */
                /*  note : Object Body is of no account                        */
                bConsistentObjImageObjFound = true;
                OId.gg92SetFrameVers (pObjFrame->objHeader_bd900.ohdFrameVersion_gg92);  // PTS 1001332 Alexander Kley 31.7.98
            }
        }
        else
        {
            if ((e_lock_collision == TrError) || (e_object_dirty == TrError))
            {
                TrError = e_ok;
                if ((g01vtrace.vtrBdObject_gg00) || (g01vtrace.vtrOmsGet_gg00))
                    Kernel_VTrace() << "bd90_GetConsistentObjImage:kb67LockObj Error:" << TrError << " mapped to e_ok";
            }
            else
              if ((g01vtrace.vtrBdObject_gg00) || (g01vtrace.vtrOmsGet_gg00 ))
                    Kernel_VTrace() << "bd90_GetConsistentObjImage:kb67LockObj Error:" << TrError;
        }
    }
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/ 
