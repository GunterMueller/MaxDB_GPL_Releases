/****************************************************************************

  module      : vbd910.cpp

  -------------------------------------------------------------------------

  responsible : FerdiF

  special area: object node classes
  description : non inline methods of the classes cbd910_ObjDataNode 
                cbd910_ObjNode

  last changed: 2003-07-18  13:12
  see also    : example.html ...

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2000-2005 SAP AG



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


*****************************************************************************/

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#ifndef GBD900_H
#   include "gbd900.h"
#endif
#ifndef GBD910_H
#   include "gbd910.h"
#endif
#ifndef GBD930_H
#   include "gbd930.h"
#endif
#ifndef HGG06_H
#   include "hgg06.h"
#endif
#ifndef HKB67_H
#	include "hkb67.h"
#endif
#ifndef KERNEL_IADMININFO_HPP
#	include "KernelCommon/Kernel_IAdminInfo.hpp"
#endif
#if defined(TESTCHAINING)
#include "heo51.h"      /* no check */
static  int g_cnt;      
#endif

/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/

extern cbd930_GarbCollController  bd91GarbCollController;

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

static bool g_IsOnline = false;

/*===========================================================================*
 *  LOCAL FUNCTIONS (PROTOTYPES)                                             *
 *===========================================================================*/

static void
bd910_InitObjPageBody (cbd910_ObjNode        &NptrsPage,
					   const tsp00_Int4       AlignedObjFrameLen);

/*---------------------------------------------------------------------------*/

static void
bd910_InitContObjPageBody (cbd910_ObjNode      &NptrsPage,
						   const tsp00_Int4     AlignedObjFrameLen);

/*===========================================================================*
 *  GLOBAL FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

/*---------------------------------------------------------------------------*/
/* METHODS of cbd910_ObjNode                                                 */
/*---------------------------------------------------------------------------*/

void
cbd910_ObjNode::bd910GetAnotherObjPage (tsp00_PageNo           NewPageNo,
				                        const teo00_Uint8      NewPageSeqNo,
						                cbd910_ObjNode        &NptrsNewPage)
{
    SAPDBTRACE_ROUTINE_DEBUG("cbd910_ObjNode::bd910GetAnotherObjPage", 
        ObjContainer_Trace, 5);	
	
	/* This function determines the pointer to a second page under the */
	/* constraint that the order of page pointer accesses has to       */
	/* follow the inverse order of page sequence numbers. In case the  */
	/* first page has a lower sequence number an optimistic            */
	/* page access is tried first                                      */

    tgg00_BasisError    &TrError = m_ptrCurrent->curr_trans->trError_gg00;

#   if COMPILEMODE_MSP00 >= QUICK_MSP00 
	if (this->np_ptr() == NULL)
		g01opmsg (sp3p_knldiag, sp3m_error, csp3_bd_msg, csp3_n_obj, "BD910GetNode: Nptr==NULL", NewPageNo);
#   endif
	
	if (bd900GetPageSeqNo(this->np_ptr()->ndLockRequestSeqNoHigh_bd00(),
        this->np_ptr()->ndLockRequestSeqNoLow_bd00()) > NewPageSeqNo)
		NptrsNewPage.bd910AssignToPage (NewPageNo);
	else
	{
		/* optimistic try */
#       if defined(TESTCHAINING)
        g_cnt++;
        if (( g_cnt % 30 ) == 1)
            TrError = e_bd_page_collision;
        else
#       endif
        NptrsNewPage.bd910AssignToPage (NewPageNo, false /*=dont wait for lock*/); 
		
		if (TrError == e_bd_page_collision)
		{
            TrError                    = e_ok;
            const tsp00_PageNo PageNo  = this->np_ptr()->nd_id();
            teo00_Uint8  this_LockSeqNo = bd900GetPageSeqNo(this->np_ptr()->ndLockRequestSeqNoHigh_bd00(),
                this->np_ptr()->ndLockRequestSeqNoLow_bd00()); 

            /* try to get pages in other order */
			this->bd910ReleasePage();

#           if defined(TESTCHAINING)
            vsleep(m_ptrCurrent->curr_trans->trTaskId_gg00, 0);
#           endif

			NptrsNewPage.bd910AssignToPage (NewPageNo);
			if (TrError != e_ok) return;

            if (bd900GetPageSeqNo(NptrsNewPage.np_ptr()->ndLockRequestSeqNoHigh_bd00(),
                NptrsNewPage.np_ptr()->ndLockRequestSeqNoLow_bd00()) > this_LockSeqNo)
                this->bd910AssignToPage (PageNo);   
            else
            {
                
                /* get pages in correct order */
#               if defined(TESTCHAINING)
                g_cnt++;
                if (( g_cnt % 10 ) == 1)
                    TrError = e_bd_page_collision;
#               else
#               endif
                this->bd910AssignToPage (PageNo, false /*=dont wait for lock*/);   
                if (TrError == e_bd_page_collision)
                {
                    TrError = e_ok;
                    NptrsNewPage.bd910ReleasePage();

#                   if defined(TESTCHAINING)
                    vsleep(m_ptrCurrent->curr_trans->trTaskId_gg00, 0);
#                   endif

                    this->bd910AssignToPage (PageNo);   
                    if (TrError != e_ok) return;
                    NptrsNewPage.bd910AssignToPage (NewPageNo);                    
                }
            }
        }
    }
}

/*---------------------------------------------------------------------------*/

void
cbd910_ObjNode::bd910InitObjPage (tsp00_Int4     ObjBodyLen,
                                  tsp00_Int4     ObjKeyLen,
                                  tsp00_Uint1    NumObjKeyFiles,
                                  tsp00_PageNo   SubRoot,
                                  teo00_Uint8    PageSeqNo)
{
    SAPDBTRACE_ROUTINE_DEBUG("cbd910_ObjNode::bd910InitObjPage", 
        ObjContainer_Trace, 5);	
	
	const tgg00_FileId  &FileId  = m_ptrCurrent->curr_tree_id;
    tgg00_BasisError    &TrError = m_ptrCurrent->curr_trans->trError_gg00;
	tbd_nodeptr         &pPage   = this->np_ptr();
	
	tsp00_Int4      ObjFrameLen;
	tsp00_Int4      AlignedObjFrameLen;
	
	/* ---------------------- */
	/* initialize page header */
	/* ---------------------- */
	
	// determine object frame length
	switch (FileId.fileObjFileType_gg00())
	{
	case oftVarLenObjFile_egg00:
		
		if  (PRIM_CONT_OBJ_FILE_NO_BD900 != FileId.fileContObjFileNo_gg00())
		{	// continuation object 
			ObjFrameLen = ObjBodyLen + sizeof (tbd900_ContObjHeader);
			pPage->nd_pt2().becomes(pt2ContObject_egg00);
		}
		else
		{	// variable length object 
			ObjFrameLen = ObjBodyLen + sizeof (tgg92_ObjHeader) + sizeof (tbd900_VarObjBodyHeader);
			pPage->nd_pt2().becomes(pt2VarObject_egg00);
		} 
		break;
	case oftFixLenObjFile_egg00:
		// fixed length object 
		ObjFrameLen = ObjBodyLen + sizeof (tgg92_ObjHeader);
		pPage->nd_pt2().becomes(pt2Object_egg00);
		break;
		
	default:
		TrError = e_wrong_obj_file_id;
		break;
	}
	
	/* find position of first object on this page */
	if ((pPage->nd_root() == pPage->nd_id()) 
        )
		pPage->nd_first_obj() = BD92ALIGN_OBJ_FRAME (NODE_HEADER_MXBD00);
	else
		pPage->nd_first_obj() = NODE_HEADER_MXBD00;
	
	/* determine maximum number of objects which can be stored on this page */
	AlignedObjFrameLen = BD92ALIGN_OBJ_FRAME(ObjFrameLen);
	if  ((BODY_END_BD00 - pPage->nd_first_obj()) >= ObjFrameLen)
	{
		pPage->nd_max_obj_cnt()    = 1 + (BODY_END_BD00 - pPage->nd_first_obj() - ObjFrameLen)/AlignedObjFrameLen;
		pPage->nd_first_free_obj() = pPage->nd_first_obj(); /* PTS 1107145 AK 10/07/2000 */
	}
	else
	{
		pPage->nd_max_obj_cnt() = 0;
		pPage->nd_first_free_obj() = NIL_OBJ_PAGE_POS_GG92; /* PTS 1107145 AK 10/07/2000 */
		/* pPage->nd_first_obj()   = NIL_OBJ_PAGE_POS_GG92;  PTS 1107145 AK 10/07/2000 */

		if (FULLCOVERING_BD00 < ObjFrameLen)
		{
			g01opmsg (sp3p_knldiag, sp3m_error, csp3_bd_msg, csp3_n_obj,
					"BD910InitObjP:TooLongObj", ObjFrameLen);
			TrError = e_too_long_objects;
			return;
		}
	}
	
	pPage->nd_occ_obj_cnt()          = 0;
    bd900SetPageSeqNo(PageSeqNo, pPage->ndLockRequestSeqNoHigh_bd00(), pPage->ndLockRequestSeqNoLow_bd00());
	pPage->nd_obj_frame_len()        = tsp00_Uint2 (ObjFrameLen);
	pPage->nd_obj_key_len()          = tsp00_Uint2 (ObjKeyLen);
    bd900SetFileNo( pPage, FileId.fileDirFileId_gg00());
	pPage->nd_next_free()            = NIL_PAGE_NO_GG00;
	pPage->nd_prev_free()            = NIL_PAGE_NO_GG00;  /* PTS 1115760 2002-05-15 FF */
	pPage->nd_in_free_chain()        = false;
	pPage->nd_objpage_state().clear();
	pPage->nd_filler5_2()            = 0;
	pPage->nd_num_key_files()        = NumObjKeyFiles;
	pPage->nd_filler5_4()            = 0;
	pPage->nd_free_eot_obj_cnt()     = 0;
	pPage->nd_free_obj_cnt()         = pPage->nd_max_obj_cnt();
	
	/* ---------------------- */
	/* initialize page bodies */
	/* ---------------------- */
	
	if (pPage->nd_root() == pPage->nd_id())
	{
		/* a subroot is the header of the free chain and therefore  */
		/* automatically member of the free chain                   */
		if (pPage->nd_max_obj_cnt() > 0)
		{
		   pPage->nd_in_free_chain()           = true;
		}
		else
		{
		   pPage->nd_in_free_chain()           = false;
		}
	}

	if (FileId.fileContObjFileNo_gg00() == PRIM_CONT_OBJ_FILE_NO_BD900)
		bd910_InitObjPageBody (*this, AlignedObjFrameLen);
	else
		bd910_InitContObjPageBody (*this, AlignedObjFrameLen);
			
	/* ----------------------- */
	/* initialize page trailor */
	/* ----------------------- */
			
	pPage->nd_checksum() = 0;
	pPage->nd_trailer()  = pPage->nd_header();
}
				
/*---------------------------------------------------------------------------*/
/* METHODS of cbd910_ObjDataNode                                             */
/*---------------------------------------------------------------------------*/

void  
cbd910_ObjDataNode::bd910AssignToNewPageInDataChain (cbd910_SubRootNode      &NptrsSubRoot,
													 bool                     bNoInsertToFreeList,
                                                     const RTE_TaskId         taskId,
                                                     FileDir_Oms             &fd_smart,
                                                     const Converter_Version &converterVersion,
                                                     tsp00_Bool               bPageMustNotBeCreated)
{
    SAPDBTRACE_ROUTINE_DEBUG("cbd910_ObjDataNode::bd910AssignToNewPageInDataChain", 
        ObjContainer_Trace, 5);

    /* this function inserts a new and empty object page into  */
    /* the chain of all pages and into the chain of free pages */

    tgg00_BasisError    &TrError = m_ptrCurrent->curr_trans->trError_gg00;
	tbd_nodeptr         &pSubRoot = NptrsSubRoot.np_ptr();
	tsp00_Int4           ObjBodySize;
	
	/*  determine object len from root page  */
	switch (NptrsSubRoot.np_ptr()->nd_pt2())
	{
	case pt2Object_egg00: 
		ObjBodySize = pSubRoot->nd_obj_frame_len() - sizeof(tgg92_ObjHeader);
		break;
		
	case pt2ContObject_egg00: 
		ObjBodySize = pSubRoot->nd_obj_frame_len() - sizeof(tbd900_ContObjHeader);
		break;
		
	case pt2VarObject_egg00: 
		ObjBodySize = pSubRoot->nd_obj_frame_len() - sizeof(tgg92_ObjHeader) - sizeof (tbd900_VarObjBodyHeader);
		break;
		
	default:
		TrError = e_inconsistent_nodetype;
		g01opmsg (sp3p_knldiag, sp3m_warning, csp3_bd_msg, csp3_n_obj, 
			"BD910Insert:      PageId", pSubRoot->nd_id());
		g01opmsg (sp3p_knldiag, sp3m_warning, csp3_bd_msg, csp3_n_obj, 
			"BD910Insert:Wrong PType2", pSubRoot->nd_pt2());
		return;
	}

    SAPDB_UInt8 PageSeqNo;

    fd_smart.IncrementPageSequenceNo(m_ptrCurrent->curr_trans->trTaskId_gg00, converterVersion);
    PageSeqNo = fd_smart.GetCurrentPageSequenceNo();

	if (bPageMustNotBeCreated)
		this->bd910InitObjPage (ObjBodySize, pSubRoot->nd_obj_key_len(), pSubRoot->nd_num_key_files(), pSubRoot->nd_id(), PageSeqNo);
	else
	    this->bd910AssignToNewPage (ObjBodySize, pSubRoot->nd_obj_key_len(), pSubRoot->nd_num_key_files(), pSubRoot->nd_id(), PageSeqNo);
	if (TrError != e_ok) return;
	   
    fd_smart.ModifyLeafPageCount(taskId, converterVersion, 1);

    /* introduce page into chain of all pages, i.e. update nd_left_nd_right pointers */
    if (pSubRoot->nd_right() != NIL_PAGE_NO_GG00) 
    {
        cbd910_ObjDataNode  NptrsPageRight (*m_ptrCurrent, pSubRoot->nd_right(), nr_for_update);
        if (TrError != e_ok) return;
        
        NptrsPageRight.np_ptr()->nd_left() = this->np_ptr()->nd_id();
        this->np_ptr()->nd_right()         = NptrsPageRight.np_ptr()->nd_id();           
    };

    this->np_ptr()->nd_left()          = NptrsSubRoot.np_ptr()->nd_id();
    NptrsSubRoot.np_ptr()->nd_right()  = this->np_ptr()->nd_id();

    /* update free chain, i.e put new page at the beginning */ 
	/* of the chain of pages with free object frames        */
    /* PTS 1127493 FF 2004-FEB-02 */ 
	if (!bPageMustNotBeCreated && !bNoInsertToFreeList) 
		NptrsSubRoot.bd910InsertPageIntoFreeChain (*this, converterVersion, fd_smart); 
}

/*---------------------------------------------------------------------------*/
/* PTS 1115760 2002-05-15 FF                                                 */
void  
cbd910_SubRootNode::bd910InsertPageIntoFreeChain(cbd910_ObjDataNode      &NptrsPage,
                                                 const Converter_Version &converterVersion,
                                                 FileDir_Oms             &fd_smart)
{
    ROUTINE_DBG_MEO00 ("bd910InsertPageIntoFreeChain");
    
    /* check preconditions */
    SAPDBERR_ASSERT_STATE (this->bd910PageIsAvailable());
    SAPDBERR_ASSERT_STATE (NptrsPage.bd910PageIsAvailable());
    SAPDBERR_ASSERT_STATE (NIL_PAGE_NO_GG00 == NptrsPage.np_ptr()->nd_next_free() || 
        (NptrsPage.np_ptr()->nd_id() == this->np_ptr()->nd_id()));
    SAPDBERR_ASSERT_STATE (NIL_PAGE_NO_GG00 == NptrsPage.np_ptr()->nd_prev_free() || 0 == NptrsPage.np_ptr()->nd_prev_free());  
    SAPDBERR_ASSERT_STATE (NIL_PAGE_NO_GG00 == NptrsPage.np_ptr()->nd_prev_free() || 0 == NptrsPage.np_ptr()->nd_prev_free());        
    SAPDBERR_ASSERT_STATE (NptrsPage.bd910PageMustBeInsertedIntoFreeChain());

	/* PTS 1135542 don't insert page assigned to one specific task for new object */
	if (NptrsPage.np_ptr()->nd_objpage_state().includes(objp_no_chain_to_freelist)) 
	{
		if (!g_IsOnline)
		{
			if (!Kernel_IAdminInfo::Instance().KernelStateIsOnline())
				NptrsPage.np_ptr()->nd_objpage_state().delElement(objp_no_chain_to_freelist);
			else 
				g_IsOnline = true;
		}
		if (g_IsOnline)
		{
			bool isAssigendToSpecTrans = false;
			tgg00_ObjPagePos   ObjPos;
			const int ObjFrameLen        = NptrsPage.np_ptr()->nd_obj_frame_len();
			const int AlignedObjFrameLen = BD92ALIGN_OBJ_FRAME(ObjFrameLen);

			for ( ObjPos = NptrsPage.np_ptr()->nd_first_obj();
				((ObjPos + ObjFrameLen) <= BODY_END_BD00); ObjPos += AlignedObjFrameLen )
			{
				tbd900_ObjFrame* pObjFrame = NptrsPage.bd910ObjFramePtr (ObjPos);
				if (kb67IsTransActive(pObjFrame->objHeader_bd900))
				{
					isAssigendToSpecTrans = true;
					break;
				}
			}
			if (isAssigendToSpecTrans)
				return;
			else
				NptrsPage.np_ptr()->nd_objpage_state().delElement(objp_no_chain_to_freelist);
		}
	}

	if ( ObjContainer_Trace.TracesLevel(4) )
    {
        Kernel_VTrace() << __FILE__ << ":" << __LINE__ 
            << " -->bd910InsertPageIntoFreeChain"
            << " Page:" << NptrsPage.np_ptr()->nd_id()
            << " NextPage:" << NptrsPage.np_ptr()->nd_next_free()
            << " PrevPage:" << NptrsPage.np_ptr()->nd_prev_free()
            << " Root:" << this->np_ptr()->nd_id()
            << " FreeCnt:" << NptrsPage.np_ptr()->nd_free_obj_cnt()
            << " MaxCnt:" << NptrsPage.np_ptr()->nd_max_obj_cnt()
            << " InFreeChain:" << NptrsPage.np_ptr()->nd_in_free_chain();
    }
    
    /* PTS 1127493 FF 2004-FEB-02 */
    if ( !NptrsPage.np_ptr()->nd_in_free_chain())
    {
        /* insert page into free page chain */
        /* during chaining the subroot is never released. Therefore no other task may change  */
        /* any page chain during this operation, because evry chaining is forced to allocated */
        /* the corresponding subroot first !                                   FF 2004-FEB-02 */
        if (NptrsPage.np_ptr()->nd_id() != this->np_ptr()->nd_id())
        {
            if (NIL_PAGE_NO_GG00 != this->np_ptr()->nd_next_free())
            {
                /* if (NptrsPage.np_ptr()->nd_free_obj_cnt() == 1)          PTS 1127493 FF 2004-FEB-02 */
                /*     --NptrsPage.np_ptr()->nd_free_obj_cnt();             PTS 1127493 FF 2004-FEB-02 */
                cbd910_ObjDataNode   NptrsPageNext (*m_ptrCurrent, nr_for_update);
                NptrsPage.bd910GetAnotherObjPage (this->np_ptr()->nd_next_free(), 
                    MAX_LOCK_REQUEST_SEQ_NO_BD900, NptrsPageNext);
                SAPDBERR_ASSERT_STATE (NptrsPage.bd910PageIsAvailable());
                NptrsPage.np_ptr()->nd_next_free()	    = this->np_ptr()->nd_next_free();
                NptrsPageNext.np_ptr()->nd_prev_free()  = NptrsPage.np_ptr()->nd_id();
                /* it can happend, that nd_free_obj_cnt has been changed by another server (rollforward delete) */
                /* or by the garbage collector                                                   FF 2004-FEB-02 */
                /* if (NptrsPage.np_ptr()->nd_free_obj_cnt() == 0)          PTS 1127493 FF 2004-FEB-02 */
                /*     ++NptrsPage.np_ptr()->nd_free_obj_cnt();             PTS 1127493 FF 2004-FEB-02 */

                if ( ObjContainer_Trace.TracesLevel(4) )
                {
                    Kernel_VTrace() << __FILE__ << ":" << __LINE__ 
                        << " NEXT Page:" << NptrsPageNext.np_ptr()->nd_id()
                        << " NextPage:" << NptrsPageNext.np_ptr()->nd_next_free()
                        << " PrevPage:" << NptrsPageNext.np_ptr()->nd_prev_free()
                        << " FreeCnt:" << NptrsPageNext.np_ptr()->nd_free_obj_cnt()
                        << " MaxCnt:" << NptrsPageNext.np_ptr()->nd_max_obj_cnt()
                        << " InFreeChain:" << NptrsPageNext.np_ptr()->nd_in_free_chain();
                }
            }
            this->np_ptr()->nd_next_free()		    = NptrsPage.np_ptr()->nd_id();
            NptrsPage.np_ptr()->nd_prev_free()      = this->np_ptr()->nd_id();
        }

        NptrsPage.np_ptr()->nd_in_free_chain() = true;

        fd_smart.ModifyFreePageCount(m_ptrCurrent->curr_trans->trTaskId_gg00,converterVersion, +1); 
        if ( ObjContainer_Trace.TracesLevel(4) )
        {
            Kernel_VTrace() << __FILE__ << ":" << __LINE__ 
                << " bd910InsertPageIntoFreeChain:ModifyFreePageCount +1"
                << " root:"
                << this->np_ptr()->nd_id();
        }
        //fd_smart.ModifyLeafPageCount(m_ptrCurrent->curr_trans->trTaskId_gg00,converterVersion, +1); 

        /* perform some simple checks */
#       if COMPILEMODE_MEO00 >= QUICK_MEO00 	
        NptrsPage.bd910_CheckFreePageCnt (*this);
#       endif

        /* write a vtrace entry */		
#       if COMPILEMODE_MEO00 >= SLOW_MEO00 
        t01name (bi,"ins into free list");
        t01int4 (bi,"ins page no=",NptrsPage.np_ptr()->nd_id()); 
#       endif
    }

    if ( ObjContainer_Trace.TracesLevel(4) )
    {
        Kernel_VTrace() << __FILE__ << ":" << __LINE__ 
            << " <--bd910InsertPageIntoFreeChain"
            << " Page:" << NptrsPage.np_ptr()->nd_id()
            << " NextPage:" << NptrsPage.np_ptr()->nd_next_free()
            << " PrevPage:" << NptrsPage.np_ptr()->nd_prev_free()
            << " Root:" << this->np_ptr()->nd_id()
            << " FreeCnt:" << NptrsPage.np_ptr()->nd_free_obj_cnt()
            << " MaxCnt:" << NptrsPage.np_ptr()->nd_max_obj_cnt()
            << " InFreeChain:" << NptrsPage.np_ptr()->nd_in_free_chain();
    }
};

/*---------------------------------------------------------------------------*/

externCpp bool 
cbd910_SubRootNode::bd910RemovePageFromChainOfAllPages(cbd910_ObjDataNode      &NptrsPage,
                                                       const RTE_TaskId         taskId,
                                                       const Converter_Version &converterVersion,
                                                       FileDir_Oms             &fd_smart
                                                       )
{
    SAPDBTRACE_ROUTINE_DEBUG("cbd910_SubRootNode::bd910RemovePageFromChainOfAllPages", 
        ObjContainer_Trace, 5);

    /* this function removes the page from the double linked chain of  */
    /* all pages and updated the counter of all pages on the root page */
    tgg00_BasisError    &TrError    = m_ptrCurrent->curr_trans->trError_gg00;

    if (NptrsPage.bd910IsPageInFreeChain()) 
        this->bd910RemovePageFromFreeChain(NptrsPage, converterVersion, fd_smart, false /*NoReInsert*/);

    /* PTS 1127493 FF 2004-FEB-02 */   
    if ( !NptrsPage.bd910IsEmptyPage()) 
    {
        if ( ObjContainer_Trace.TracesLevel(4) )
            Kernel_VTrace() << __FILE__ << ":" << __LINE__ 
            << " <--bd910RemovePageFromChainOfAllPages page not empty";
        return false;
    }

    if (NptrsPage.bd910IsPageInFreeChain())
    {
        if (!NptrsPage.bd910IsSubRoot())
        {
#           if COMPILEMODE_MEO00 >= QUICK_MEO00 	
            if (NptrsPage.bd910IsEmptyPage()) 
                Kernel_OpWarning(csp3_bd_msg, csp3_n_obj)
                << __FILE__ << ":" << __LINE__ 
                << " bd910RemovePageFromChainOfAllPages " << NptrsPage.np_ptr()->nd_id()
                << " root:" << NptrsPage.np_ptr()->nd_root()
                << " nd_free_obj_cnt:" << NptrsPage.np_ptr()->nd_free_obj_cnt()
                << " nd_max_obj_cnt:" << NptrsPage.np_ptr()->nd_max_obj_cnt()
                << " nd_in_free_chain:" << NptrsPage.np_ptr()->nd_in_free_chain();
#           endif
            return (false);
        }
    }

    if ( ObjContainer_Trace.TracesLevel(4) )
    {
        Kernel_VTrace() << __FILE__ << ":" << __LINE__ 
            << " -->bd910RemovePageFromChainOfAllPages"
            << " Page:" << NptrsPage.np_ptr()->nd_id()
            << " Left:" << NptrsPage.np_ptr()->nd_left()
            << " Right:" << NptrsPage.np_ptr()->nd_right()
            << " Root:" << this->np_ptr()->nd_id()
            << " FreeCnt:" << NptrsPage.np_ptr()->nd_free_obj_cnt()
            << " MaxCnt:" << NptrsPage.np_ptr()->nd_max_obj_cnt();
    }

    /* check precondition that the page is not included in the free chain */
    SAPDBERR_ASSERT_STATE (this->bd910PageIsAvailable());
    SAPDBERR_ASSERT_STATE (NptrsPage.bd910PageIsAvailable());
    /* SAPDBERR_ASSERT_STATE ((! NptrsPage.np_ptr()->nd_in_free_chain()) ||
        (NptrsPage.np_ptr()->nd_id() == this->np_ptr()->nd_id())); */
#   if COMPILEMODE_MEO00 >= QUICK_MEO00 	
    if (( NptrsPage.np_ptr()->nd_in_free_chain()) &&
        ( NptrsPage.np_ptr()->nd_id() != this->np_ptr()->nd_id())) 
    {
            Kernel_OpError(csp3_bd_msg, csp3_n_obj)
            << __FILE__ << ":" << __LINE__ 
            << " bd910RemovePageFromChainOfAllPages " << NptrsPage.np_ptr()->nd_id()
            << " root:" << NptrsPage.np_ptr()->nd_root()
            << " nd_free_obj_cnt:" << NptrsPage.np_ptr()->nd_free_obj_cnt()
            << " nd_max_obj_cnt:" << NptrsPage.np_ptr()->nd_max_obj_cnt()
            << " nd_in_free_chain:" << NptrsPage.np_ptr()->nd_in_free_chain();
        g01abort (csp3_bd_msg, csp3_n_obj, 
            "bd910RmvPageFrmAllPages:", np_ptr()->nd_id());
    }
#   endif

    cbd910_ObjDataNode   NptrsLeftPage (*m_ptrCurrent, nr_for_update);
    cbd910_ObjDataNode   NptrsRightPage(*m_ptrCurrent, nr_for_update);
    
    if(!(NptrsPage.np_ptr()->nd_id() == this->np_ptr()->nd_id()))
    {
        if(NptrsPage.np_ptr()->nd_left() != this->np_ptr()->nd_id())
        {
            NptrsPage.bd910GetAnotherObjPage (NptrsPage.np_ptr()->nd_left(), MAX_LOCK_REQUEST_SEQ_NO_BD900, NptrsLeftPage);
            if (TrError != e_ok) return (false);
        }
        
        if (NptrsPage.np_ptr()->nd_right() != NIL_PAGE_NO_GG00)
        {
            NptrsPage.bd910GetAnotherObjPage (NptrsPage.np_ptr()->nd_right(), MIN_LOCK_REQUEST_SEQ_NO_BD00, NptrsRightPage);
            if (TrError != e_ok) return (false);
        }
        
        /* remove page from chain ,i.e. update neighbor pages */
        if(NptrsPage.np_ptr()->nd_left() != this->np_ptr()->nd_id())	
            NptrsLeftPage.np_ptr()->nd_right() = NptrsPage.np_ptr()->nd_right();
        else
            this->np_ptr()->nd_right() = NptrsPage.np_ptr()->nd_right();
        
        if (NptrsPage.np_ptr()->nd_right() != NIL_PAGE_NO_GG00)
            NptrsRightPage.np_ptr()->nd_left() = NptrsPage.np_ptr()->nd_left();
        
        /* write a vtrace entry */
#       if COMPILEMODE_MEO00 >= SLOW_MEO00 
        t01name (bi,"rem page from file");
        t01int4 (bi,"rem page no=",NptrsPage.np_ptr()->nd_id());  
#       endif
        
        /* release page */
        if ( ObjContainer_Trace.TracesLevel(4) )
        {
            Kernel_VTrace() << __FILE__ << ":" << __LINE__ 
                << " free page"
                << " Page:" << NptrsPage.np_ptr()->nd_id();
        }
        fd_smart.ModifyLeafPageCount(taskId, converterVersion, -1);        

        /* release subroot beore freeing page PTS 1135421 */
        this->bd910ReleasePage();
        NptrsPage.bd910FreePage();
    }
    else
    {
        /* in case 'nd_free_obj_cnt == nd_max_obj_cnt == 1' page may not be */
        /* within freechain -> insert it !                                  */
        if (NptrsPage.bd910PageMustBeInsertedIntoFreeChain())
            this->bd910InsertPageIntoFreeChain (NptrsPage, converterVersion, fd_smart);    
        NptrsPage.bd910ReleasePage(); /* subroot isn't deleted !!! */
    }    

    if ( ObjContainer_Trace.TracesLevel(4) )
        Kernel_VTrace() << __FILE__ << ":" << __LINE__ 
        << " <--bd910RemovePageFromChainOfAllPages ret: true";
    return (true);
}

/*===========================================================================*
 *  LOCAL FUNCTIONS (CODE)                                                   *
 *===========================================================================*/

static void
bd910_InitContObjPageBody (cbd910_ObjNode        &NptrsPage,
						   const tsp00_Int4       AlignedObjFrameLen)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd910_InitContObjPageBody", ObjContainer_Trace, 5);
    ROUTINE_DBG_MSP00 ("bd90_InitContObjPageBody");
    
    /* this function initializes the continuation object */
    /* frames at an object page                          */
    
    tsp00_Int4           ContObjFramePos = NptrsPage.np_ptr()->nd_first_obj();
    const tsp00_Int4     MaxObjCnt       = NptrsPage.np_ptr()->nd_max_obj_cnt();
    tbd900_ContObjFrame *pContObjFrame;
    
    for (int iObjCnt=1; iObjCnt<=MaxObjCnt; ++iObjCnt)
    {
        /* set pointer on current objectframe */
        pContObjFrame = (tbd900_ContObjFrame*) ((char*) NptrsPage.np_ptr() + ContObjFramePos);

        /* calculate position of next object */
        ContObjFramePos += AlignedObjFrameLen;
        
        /* initialize next free obj pointers */
        if  (iObjCnt < MaxObjCnt)
            pContObjFrame->cofHeader_bd900.cvoNextFreeObj_bd900 = ContObjFramePos ;
        else
            pContObjFrame->cofHeader_bd900.cvoNextFreeObj_bd900 = NIL_OBJ_PAGE_POS_GG92;
        
        pContObjFrame->cofHeader_bd900.cvoRootOIdPno_bd900  = NIL_PAGE_NO_GG00;
        pContObjFrame->cofHeader_bd900.cvoRootOIdPos_bd900  = NIL_OBJ_PAGE_POS_GG92;      
    }
}

/*---------------------------------------------------------------------------*/

static void
bd910_InitObjPageBody (cbd910_ObjNode        &NptrsPage,
					   const tsp00_Int4       AlignedObjFrameLen)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd910_InitObjPageBody", ObjContainer_Trace, 5);
	
    tsp00_Int4         ObjFramePos = NptrsPage.np_ptr()->nd_first_obj();
    const tsp00_Int4   MaxObjCnt   = NptrsPage.np_ptr()->nd_max_obj_cnt();
    tbd900_ObjFrame     *pObjFrame;
    
    for (int iObjCnt=1; iObjCnt<=MaxObjCnt; ++iObjCnt)
    {
        /* set pointer on current objectframe */        
        pObjFrame = cbd910_PrimObjDataNode::bd910ObjFramePtr((tsp00_Byte *)NptrsPage.np_ptr(), ObjFramePos);

        /* calculate position of next object */
        ObjFramePos += AlignedObjFrameLen;
        
        gg06InitObjTransInfo (pObjFrame->objHeader_bd900.ohdTransInfo_gg92);     
        pObjFrame->objHeader_bd900.ohdState_gg92.becomes(obsFree_egg00);
        
        FileDir_FileNo fileno;
        bd900GetFileNo( NptrsPage.np_ptr(), fileno);

        /* set object version */
        /* PTS 1125289 FF 2003-Nov-07 */
        /* pObjFrame->objHeader_bd900.ohdFrameVersion_gg92 = NIL_OBJ_FRAME_VERS_GG92; */
        if ( -1 == NptrsPage.np_ptr()->ndLockRequestSeqNoLow_bd00()) /* is subroot */
            pObjFrame->objHeader_bd900.ohdFrameVersion_gg92 = (tgg00_ObjFrameVers)( fileno.GetUInt8() % MAX_OBJ_FRAME_VERS_GG92);
        else
            pObjFrame->objHeader_bd900.ohdFrameVersion_gg92 = 
            ( NptrsPage.np_ptr()->ndLockRequestSeqNoLow_bd00() << 4 ) % MAX_OBJ_FRAME_VERS_GG92;
        if(pObjFrame->objHeader_bd900.ohdFrameVersion_gg92 < MIN_OBJ_FRAME_VERS_GG92)
            pObjFrame->objHeader_bd900.ohdFrameVersion_gg92 = MIN_OBJ_FRAME_VERS_GG92;                    
        
        /* initialize next free obj pointers */
        if  (iObjCnt < MaxObjCnt)
            pObjFrame->objHeader_bd900.ohdNextFreeObj_gg92 = ObjFramePos ;
        else
            pObjFrame->objHeader_bd900.ohdNextFreeObj_gg92 = NIL_OBJ_PAGE_POS_GG92;
		
        /* initialize continuation info for variable object pages */
		if (pt2VarObject_egg00 == NptrsPage.np_ptr()->nd_pt2())
		{
			tbd900_VarObjBody & VarObjBody = *((tbd900_VarObjBody*) (pObjFrame->objBody_bd900));
			
			VarObjBody.vbdHeader_bd900.vhdContObjPno_bd900    = NIL_PAGE_NO_GG00;
			VarObjBody.vbdHeader_bd900.vhdContObjPos_bd900    = NIL_OBJ_PAGE_POS_GG92;
            VarObjBody.vbdHeader_bd900.vhdContObjFileNo_bd900 = PRIM_CONT_OBJ_FILE_NO_BD900;
			VarObjBody.vbdHeader_bd900.vhdObjSize_bd900       = 0;
		}
    }
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
