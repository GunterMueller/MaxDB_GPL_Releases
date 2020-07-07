/****************************************************************************

  module      : gbd910.h

  -------------------------------------------------------------------------

  responsible : FerdiF

  special area: object node classes
  description : description ...


  version     : 7.4.4.0
  last changed: 2003-04-04  14:47
  see also    : example.html ...

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

#ifndef GBD910_H
#define GBD910_H

/*===========================================================================*
*  INCLUDES                                                                 *
*===========================================================================*/

#ifndef GGG92_H
#   include "ggg92.h"
#endif
#ifndef GGG01_H
#   include "ggg01.h"
#endif
#ifndef GBD00_H
#   include "gbd00.h"
#endif
#ifndef GBD900_H
#   include "gbd900.h"
#endif
#ifndef HBD13_H
#   include "hbd13.h"
#endif
#ifndef HBD13_1_H
#   include "hbd13_1.h"
#endif
#ifndef HBD20_2_H
#   include "hbd20_2.h"
#endif
#ifndef HBD92_H
#   include "hbd92.h"
#endif
#ifndef HGG01_1_H
#   include "hgg01_1.h" 
#endif
#ifndef GSP03_H
#   include "gsp03.h" 
#endif
#ifndef GSP03_3_H
#   include "gsp03_3.h" 
#endif

#ifndef FILEDIR_OMS_HPP
#   include "FileDirectory/FileDir_Oms.hpp"        
#endif
#ifndef CONVERTER_ICOMMON_HPP
#   include "Converter/Converter_ICommon.hpp"
#endif
#ifndef FILEDIR_IDIRECTORY_HPP
#   include "FileDirectory/FileDir_IDirectory.hpp"
#endif

#ifndef KERNEL_VTRACE_HPP
#   include "KernelCommon/Kernel_VTrace.hpp"            
#endif
#ifndef KERNEL_OPMSG_HPP
#   include "KernelCommon/Kernel_OpMsg.hpp"         
#endif

#if COMPILEMODE_MSP00 >= QUICK_MSP00 
#   include "Messages/Msg_List.hpp"
#endif

#if COMPILEMODE_MEO00 >= SLOW_MEO00 
#include "hta99.h"   
#include "hta01.h" 
#endif

/*===========================================================================*
*  DEFINES                                                                  *
*===========================================================================*/

#ifndef TRUE
#define TRUE 1
#endif

/*===========================================================================*
*  MACROS                                                                   *
*===========================================================================*/

/*===========================================================================*
*  EXTERNAL VARIABLES                                                       *
*===========================================================================*/

/*===========================================================================*
*  FORWARD DECLARATUIN                                                      *
*===========================================================================*/

class cbd910_ObjNode;
class cbd910_ObjDataNode;
class cbd910_RootNode; 
class cbd910_SubRootNode;
class cbd910_ContObjDataNode;
class cbd910_PrimObjDataNode;

/*===========================================================================*
*  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
*===========================================================================*/

class cbd910_ObjNode : public tbd_node_ptrs
{
public:
    cbd910_ObjNode (cbd05_Current       &Current,
                    tbd_node_request_Enum   NodeRequest);
    cbd910_ObjNode (cbd05_Current       &Current, 
                    tsp00_PageNo            PageNo,
                    tbd_node_request_Enum   NodeRequest,
                    tsp00_Bool              bWaitForLock = true);
    ~cbd910_ObjNode();
    void bd910ReleasePage();
    void bd910AssignToPage (tsp00_PageNo  PageNo, 
                            tsp00_Bool    bWaitForLock = true);
    bool bd910IsEmptyPage();
    bool bd910IsPageInFreeChain();
    bool bd910IsSubRoot();
	bool bd910LockSubRootBeforeReleasingFrame() const; 
    void bd910FreePage();
    void bd910MoveToNextPage();
    void bd910AssignToNewPage (tsp00_Int4               ObjBodyLen,
                               tsp00_Int4               ObjKeyLen,
                               tsp00_Uint1              NumKeyFiles,
                               tsp00_PageNo             SubRoot,
                               teo00_Uint8              PageSeqNo);
    
    void bd910AssignToNewSpecifiedPage (tsp00_PageNo   PageNo,
                                        tsp00_Bool    &bPageGenerated);
    bool bd910PageIsAvailable();
    bool bd910PageIsRequested();
    void bd910GetAnotherObjPage (tsp00_PageNo                   NewPageNo,
                                 const teo00_Uint8              NewPageSeqNo,
                                 cbd910_ObjNode                &NptrsNewPage);
    
   	void bd910SetTo (cbd910_ObjNode & ObjNode, const bool movePrimary = false);
    
private:   // methods
    virtual void bd910_CheckNodeType ();
    void         bd910_InitNode (cbd05_Current      &Current,
                                 tbd_node_request_Enum  NodeRequest);
public: // methods
    void   bd910InitObjPage (tsp00_Int4             ObjBodyLen,
                             tsp00_Int4             ObjKeyLen,
                             tsp00_Uint1            NumKeyFiles,
                             tsp00_PageNo           SubRoot,
                             teo00_Uint8            PageSeqNo);
    
protected: // members
    cbd05_Current *m_ptrCurrent;
    
private:   //members
    tbd_node_request  m_NodeRequest;
    bool              m_PrimaryReference;
};

/*---------------------------------------------------------------------------*/

class cbd910_RootNode : public cbd910_ObjNode
{
public:
    cbd910_RootNode (cbd05_Current        &Current,
                     tbd_node_request_Enum    NodeRequest);
    cbd910_RootNode (cbd05_Current        &Current, 
                     tsp00_PageNo             PageNo,
                     tbd_node_request_Enum    NodeRequest,
                     tbd00_PageLockMode_Enum  PageLockMode = plmLock_ebd00);
private:
    virtual void bd910_CheckNodeType ();
};

/*---------------------------------------------------------------------------*/

class cbd910_SubRootNode : public cbd910_ObjNode
{
public:
    cbd910_SubRootNode (cbd05_Current          &Current,
                        tbd_node_request_Enum   NodeRequest);
    cbd910_SubRootNode (cbd05_Current          &Current, 
                        tsp00_PageNo            PageNo,
                        tbd_node_request_Enum   NodeRequest,
                        tbd00_PageLockMode_Enum PageLockMode = plmLock_ebd00);
    
    void    bd910AssignToSubroot               (cbd910_ObjDataNode      &NptrsPage);
    void    bd910InsertPageIntoFreeChain	   (cbd910_ObjDataNode      &NptrsPage,
                                                const Converter_Version &converterVersion,
                                                FileDir_Oms             &fd_smart);
    void    bd910RemovePageFromFreeChain       (cbd910_ObjDataNode      &NptrsPage,
                                                const Converter_Version &converterVersion,
                                                FileDir_Oms             &fd_smart,
												bool				    bNoReInsert);
    void    bd910RestoreFreePageChain		   (cbd910_ObjDataNode      &NptrsPage,
                                                const Converter_Version &converterVersion,
                                                FileDir_Oms             &fd_smart);
    bool    bd910RemovePageFromChainOfAllPages (cbd910_ObjDataNode      &NptrsSubRoot, 
                                                const RTE_TaskId         taskId,
                                                const Converter_Version &converterVersion,
                                                FileDir_Oms             &fd_smart);
private:
    virtual void bd910_CheckNodeType ();
};

/*---------------------------------------------------------------------------*/

class cbd910_ObjDataNode : public cbd910_ObjNode
{
public:
    cbd910_ObjDataNode (cbd05_Current        &Current,
                        tbd_node_request_Enum    NodeRequest);
    cbd910_ObjDataNode (cbd05_Current        &Current, 
                        tsp00_PageNo             PageNo,
                        tbd_node_request_Enum    NodeRequest,
                        tbd00_PageLockMode_Enum  PageLockMode = plmLock_ebd00);
    void                    bd910AssignToNewPageInDataChain (cbd910_SubRootNode      &NptrsSubRoot,
                                                             bool                     bNoInsertToFreeList, 
                                                             const RTE_TaskId         taskId,
                                                             FileDir_Oms             &fd_smart,
                                                             const Converter_Version &converterVersion,
                                                             tsp00_Bool               bPageMustNotBeCreated = false);
    bool                    bd910PageMustBeInsertedIntoFreeChain ();
    bool                    bd910PageMustBeRemovedFromFreeChain  ();
    tbd900_ObjFrame        *bd910ObjFramePtr (tsp00_Int4  ObjFramePos);
    static tbd900_ObjFrame *bd910ObjFramePtr (tsp00_Byte *pPageBodyBeg, tsp00_Int4 ObjFramePos);
    
    void   bd910_CheckFreePageCnt (cbd910_SubRootNode &NptrsSubRoot);
};

/*---------------------------------------------------------------------------*/

class cbd910_ContObjDataNode : public cbd910_ObjDataNode
{
public:
    cbd910_ContObjDataNode (cbd05_Current        &Current,
                            tbd_node_request_Enum     NodeRequest);
    cbd910_ContObjDataNode (cbd05_Current        &Current, 
                            tsp00_PageNo             PageNo,
                            tbd_node_request_Enum    NodeRequest,
                            tbd00_PageLockMode_Enum  PageLockMode = plmLock_ebd00);
    
private:
    virtual void bd910_CheckNodeType ();
};

/*---------------------------------------------------------------------------*/

class cbd910_PrimObjDataNode : public cbd910_ObjDataNode
{
public:
    cbd910_PrimObjDataNode (cbd05_Current           &Current,
                            tbd_node_request_Enum    NodeRequest);
    cbd910_PrimObjDataNode (cbd05_Current           &Current, 
                            tsp00_PageNo             PageNo,
                            tbd_node_request_Enum    NodeRequest,
                            tbd00_PageLockMode_Enum  PageLockMode = plmLock_ebd00);
    
    
private:
    virtual void bd910_CheckNodeType ();
};


/*===========================================================================*
*  DEFINITION OF METHODS                                                    *
*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* METHODS of cbd910_ObjNode                                                 */
/*---------------------------------------------------------------------------*/

inline void
cbd910_ObjNode::bd910AssignToPage(tsp00_PageNo  PageNo, 
                                  tsp00_Bool    bWaitForLock)
{
#   if COMPILEMODE_MSP00 >= QUICK_MSP00 
    if (np_ptr() != NULL)
        g01abort (csp3_bd_msg, csp3_n_obj, "BD910Assign: Nptr!=NULL ", np_ptr()->nd_id());
#   endif	
    
    SAPDBERR_ASSERT_STATE ((m_NodeRequest == nr_for_update) ||(m_NodeRequest == nr_for_read));
    SAPDBERR_ASSERT_STATE ((m_NodeRequest == nr_for_update) || bWaitForLock);
    
    tbd00_PageLockMode PageLockMode;

    if (m_NodeRequest == nr_for_read)
        PageLockMode.becomes (plmLock_ebd00);
    else
    {
        if (bWaitForLock)
            PageLockMode.becomes(plmLockWithSVPSync_ebd00);
        else
            PageLockMode.becomes(plmLockWithSVPSyncNoWait_ebd00);
    }
    
    bd13GetNode (*m_ptrCurrent, PageNo, PageLockMode, m_NodeRequest, *this); 
    
    m_PrimaryReference = true;
    
    if ((oftUnknownType_egg00 == m_ptrCurrent->curr_tree_id.fileObjFileType_gg00()) &&
        (e_ok                 == m_ptrCurrent->curr_trans->trError_gg00))
    {
        tgg00_FileId  &ObjFileId = m_ptrCurrent->curr_tree_id;

        FileDir_FileNo      fileno;
        bd900GetFileNo(this->np_ptr(),fileno);

        ObjFileId                           = b01niltree_id;
        ObjFileId.fileRoot_gg00()           = this->np_ptr()->nd_root();
        ObjFileId.fileContObjFileNo_gg00()  = PRIM_CONT_OBJ_FILE_NO_BD900;
        ObjFileId.fileTfn_gg00().becomes(tfnObj_egg00); 
        ObjFileId.fileType_gg00().clear();
        ObjFileId.fileType_gg00().addElement (ftsObject_egg00);      
        ObjFileId.fileType_gg00().addElement (ftsConcurrent_egg00); 
        ObjFileId.fileType_gg00().addElement (ftsPerm_egg00);
        ObjFileId.fileDirFileId_gg00()      = *reinterpret_cast<const tsp00_C8*>(fileno.GetCharArray());
        ObjFileId.fileNumObjKeyFiles_gg00() = this->np_ptr()->nd_num_key_files();

        if ( this->np_ptr()->nd_pt2() == pt2VarObject_egg00 )
            ObjFileId.fileObjFileType_gg00().becomes (oftVarLenObjFile_egg00);
        else if ( this->np_ptr()->nd_pt2() == pt2Object_egg00 )
            ObjFileId.fileObjFileType_gg00().becomes (oftFixLenObjFile_egg00);
        else 
            g01abort (csp3_bd_msg, csp3_n_obj, "BD910Assign: Bad FileId ", this->np_ptr()->nd_id());
    }

#   if COMPILEMODE_MSP00 >= QUICK_MSP00 
    Msg_List ErrorList;
    if ((oftUnknownType_egg00 == m_ptrCurrent->curr_tree_id.fileObjFileType_gg00()) &&
        (e_ok                 == m_ptrCurrent->curr_trans->trError_gg00))
    {
        tgg00_FileId  localFileId;
        
        FileDir_FileNo ObjFileNo;
        bd900GetFileNo(this->np_ptr(), ObjFileNo);
        tgg00_TransContext &Trans       = *m_ptrCurrent->curr_trans;
        FileDir_Oms   fd_smart;
        FileDir_ReturnCode  fd_rc = FileDir_IDirectory::SessionInstance(Trans).GcGetFile( 
            Trans.trTaskId_gg00, ObjFileNo, fd_smart, ErrorList );
        if (FileDir_Okay != fd_rc) {
            Kernel_OpError(csp3_bd_msg, csp3_n_obj)
                << __FILE__ << ":" << __LINE__ 
                << " LVC_IFileDirCache::GetFile error:" << fd_rc
                << " fileId:" << ObjFileNo;
            g01abort (csp3_bd_msg, csp3_n_obj, 
                "bd910AssignToPage:FileId", fd_rc);
        }

        if (pt2ContObject_egg00 == this->np_ptr()->nd_pt2())
            g01abort (csp3_bd_msg, csp3_n_obj, "BD910Assign: Wrong Type ", this->np_ptr()->nd_id());
        if (this->np_ptr()->nd_root() != fd_smart.GetRootPage())
            g01abort (csp3_bd_msg, csp3_n_obj, "BD910Assign: Wrong Root ", this->np_ptr()->nd_id());

        fd_smart.BuildFileId(localFileId);
        if (memcmp(&m_ptrCurrent->curr_tree_id, &localFileId, sizeof(localFileId)))
            g01abort (csp3_bd_msg, csp3_n_obj, "BD910Assign: Bad FileId ", this->np_ptr()->nd_id());
    }	

    if (e_ok == m_ptrCurrent->curr_trans->trError_gg00)
        bd910_CheckNodeType();
#   endif
}
/*---------------------------------------------------------------------------*/

inline void 
cbd910_ObjNode::bd910_InitNode (cbd05_Current      &Current,
                                tbd_node_request_Enum  NodeRequest)
{
    np_ptr()      = NULL;
    np_cbptr()    = NULL;
    m_ptrCurrent  = &Current;
    m_NodeRequest.becomes(NodeRequest);
    m_PrimaryReference = true;
}

/*---------------------------------------------------------------------------*/

inline 
cbd910_ObjNode::cbd910_ObjNode (cbd05_Current    &Current,
                                tbd_node_request_Enum  NodeRequest)
{
    bd910_InitNode (Current, NodeRequest);
}

/*---------------------------------------------------------------------------*/

inline 
cbd910_ObjNode::cbd910_ObjNode (cbd05_Current        &Current, 
                                tsp00_PageNo             PageNo,
                                tbd_node_request_Enum    NodeRequest,
                                tsp00_Bool               bWaitForLock)
{
    bd910_InitNode     (Current, NodeRequest);
    bd910AssignToPage  (PageNo,  bWaitForLock);
}

/*---------------------------------------------------------------------------*/

inline void
cbd910_ObjNode::bd910ReleasePage()
{
    if (NULL != np_ptr())
    { 
        if (m_PrimaryReference)
        {	
#           if COMPILEMODE_MEO00 >= QUICK_MEO00 	
            /* Assertion: free_obj_cnt and free_chain flag have to be consistent!     */
            /* This is essential to implemented 'lock' protocol!                      */
            /* ignore check if page is empty! see: bd910RemovePageFromChainOfAllPages */
            if (np_ptr()->nd_free_obj_cnt() > 0 && 
                !np_ptr()->nd_in_free_chain() &&
                np_ptr()->nd_free_obj_cnt()!= np_ptr()->nd_max_obj_cnt())  
            {
                Kernel_VTrace()
                    << __FILE__ << ":" << __LINE__ 
                    << " bd910ReleasePage " << np_ptr()->nd_id()
                    << " root:" << np_ptr()->nd_root()
                    << " nd_free_obj_cnt:" << np_ptr()->nd_free_obj_cnt()
                    << " nd_max_obj_cnt:" << np_ptr()->nd_max_obj_cnt()
                    << " nd_in_free_chain:" << np_ptr()->nd_in_free_chain();
            }
#           endif

            if ( (nr_for_update == m_NodeRequest)
                /* PTS 1119609 UH 2002-12-17
                * LockObj sets an error e_lock_collision but updates the header
                * with the dummy transno in the lock field which must be flushed to disk.
                * && (e_ok == m_ptrCurrent->curr_trans->trError_gg00)
                * */ ) 
                b13w_release_node (*this, *m_ptrCurrent);
            else  // m_NodeRequest == nr_for_read 
                b13r_release_node (*this, *m_ptrCurrent, lru_normal);
        }
        else
        {
            np_ptr()      = NULL;
            np_cbptr()    = NULL;
        }
    }
}

/*---------------------------------------------------------------------------*/

inline
cbd910_ObjNode::~cbd910_ObjNode() 
{     
    bd910ReleasePage();
}

/*---------------------------------------------------------------------------*/

inline void 
cbd910_ObjNode::bd910FreePage()
{
#   if COMPILEMODE_MSP00 >= QUICK_MSP00 
    if (nr_for_update != m_NodeRequest)
        g01abort (csp3_bd_msg, csp3_n_obj, "BD910FreePage: wrong Req", np_ptr()->nd_id());
    if (! m_PrimaryReference)
        g01abort (csp3_bd_msg, csp3_n_obj, "BD910FreePage: NoPrimRef", np_ptr()->nd_id());
#   endif
    
    b13free_node (*this, *m_ptrCurrent);
}    

/*---------------------------------------------------------------------------*/

inline void
cbd910_ObjNode::bd910MoveToNextPage()
{                       
    /* get pointer to next page while keeping the current  */
    /* page to ensure that the nd_right pointer is valid   */
    if (NULL == np_ptr())
        return;
    else
    {
        const tsp00_PageNo  RightPageNo = np_ptr()->nd_right();
        cbd910_ObjNode      NptrsPrevPage(*this);     
        
        /* reset page pointer */
        np_ptr()   = NULL;
        np_cbptr() = NULL;
        
        if (NIL_PAGE_NO_GG00 != RightPageNo)
            bd910AssignToPage (RightPageNo);
        else
            return;
        
        /* check that the page moved to has a lower LockRequestSeqNo */
#       if COMPILEMODE_MSP00 >= QUICK_MSP00 
        if ((np_ptr() != NULL) && (np_ptr()->nd_id() != np_ptr()->nd_root())) /* PTS 1107145 AK 10/07/2000*/
        {
            if (bd900GetPageSeqNo(this->np_ptr()->ndLockRequestSeqNoHigh_bd00(),this->np_ptr()->ndLockRequestSeqNoLow_bd00()) >=
                bd900GetPageSeqNo(NptrsPrevPage.np_ptr()->ndLockRequestSeqNoHigh_bd00(),NptrsPrevPage.np_ptr()->ndLockRequestSeqNoLow_bd00()))
            {
                g01opmsg (sp3p_knldiag, sp3m_error, csp3_bd_msg, csp3_n_obj, "BD910MoveTo: wrong SeqNo",  
                    NptrsPrevPage.np_ptr()->nd_id());
                g01abort (csp3_bd_msg, csp3_n_obj, "BD910MoveTo: wrong SeqNo", np_ptr()->nd_id());
            }
        }
#       endif	
    }
}

/*---------------------------------------------------------------------------*/

inline bool 
cbd910_ObjNode::bd910PageIsAvailable()
{
    return (NULL != this->np_ptr());
};

/*---------------------------------------------------------------------------*/

inline bool
cbd910_ObjNode::bd910PageIsRequested()
{
    return (TRUE == bd20IsPageRequested(this->np_cbptr()));
}


/*---------------------------------------------------------------------------*/

inline void
cbd910_ObjNode::bd910_CheckNodeType ()
{
    if ((this->np_ptr()->nd_pt2() != pt2ContObject_egg00)         &&
        (this->np_ptr()->nd_pt2() != pt2Object_egg00)             &&
        (this->np_ptr()->nd_pt2() != pt2VarObject_egg00))
    {
        g01abort (csp3_bd_msg, csp3_n_obj, "BD910_Check: No ObjNode ", np_ptr()->nd_id());
    }
}


/*---------------------------------------------------------------------------*/

inline	void
cbd910_ObjNode::bd910SetTo (cbd910_ObjNode & ObjNode, const bool movePrimary /*PTS 1130601*/)
{
#   if COMPILEMODE_MSP00 >= QUICK_MSP00 
    if (np_ptr() != NULL)
    {
        g01abort (csp3_bd_msg, csp3_n_obj, "BD910SetTo: Nptr!=NULL  ", np_ptr()->nd_id());
    }
#   endif
    
    *this              = ObjNode;
    if ( movePrimary ) /*PTS 1130601*/
    {
        ObjNode.m_PrimaryReference = false;
        m_PrimaryReference = true;
    }
    else
        m_PrimaryReference = false;
}

/*---------------------------------------------------------------------------*/
/* METHODS of cbd910_ObjDataNode                                             */
/*---------------------------------------------------------------------------*/

inline
cbd910_ObjDataNode::cbd910_ObjDataNode (cbd05_Current        &Current,
                                        tbd_node_request_Enum    NodeRequest)
                                        :cbd910_ObjNode (Current, NodeRequest)
{}

/*---------------------------------------------------------------------------*/

inline
cbd910_ObjDataNode::cbd910_ObjDataNode (cbd05_Current        &Current, 
                                        tsp00_PageNo             PageNo,
                                        tbd_node_request_Enum    NodeRequest,
                                        tbd00_PageLockMode_Enum  PageLockMode)
                                        :cbd910_ObjNode (Current, PageNo, NodeRequest,
                                        (plmLockWithSVPSyncNoWait_ebd00 != PageLockMode))
{}

/*---------------------------------------------------------------------------*/

inline void
cbd910_ObjNode::bd910AssignToNewPage (tsp00_Int4             ObjBodyLen,
                                      tsp00_Int4             ObjKeyLen,
                                      tsp00_Uint1            NumKeyFiles,
                                      tsp00_PageNo           SubRoot,
                                      teo00_Uint8            PageSeqNo)
{
    SAPDBERR_ASSERT_STATE ((np_ptr() == NULL));
    SAPDBERR_ASSERT_STATE ((nr_for_update == m_NodeRequest));
    
    b13new_object_node (*this, *m_ptrCurrent);
    m_PrimaryReference = true;
    
    this->bd910InitObjPage (ObjBodyLen, ObjKeyLen, NumKeyFiles, SubRoot, PageSeqNo);
    
#   if COMPILEMODE_MSP00 >= QUICK_MSP00 
    bd910_CheckNodeType();
#   endif
}

/*---------------------------------------------------------------------------*/

inline void
cbd910_ObjNode::bd910AssignToNewSpecifiedPage (tsp00_PageNo   PageNo,
                                               tsp00_Bool    &bPageGenerated)
{
    SAPDBERR_ASSERT_STATE ((np_ptr() == NULL) && (nr_for_update == m_NodeRequest));
    
    bd13GetNewSpecifiedNode (*(m_ptrCurrent->curr_trans), 
        m_ptrCurrent->curr_tree_id, PageNo,*this, bPageGenerated);
    
    m_PrimaryReference = true;
}

/*---------------------------------------------------------------------------*/

inline	bool                    
cbd910_ObjNode::bd910IsEmptyPage ()
{
    return ((this->np_ptr()->nd_free_obj_cnt() == this->np_ptr()->nd_max_obj_cnt()));   
}

/*---------------------------------------------------------------------------*/

inline	bool                    
cbd910_ObjNode::bd910IsSubRoot ()
{
    return (this->np_ptr()->nd_id() == this->np_ptr()->nd_root()); 
}

/*---------------------------------------------------------------------------*/

inline	bool                    
cbd910_ObjNode::bd910IsPageInFreeChain ()
{
    SAPDBERR_ASSERT_STATE ((0 != this->np_ptr()->nd_in_free_chain()) ||
        this->np_ptr()->nd_root() == this->np_ptr()->nd_id() ||
        (0 == this->np_ptr()->nd_in_free_chain() && 
        NIL_PAGE_NO_GG00 == this->np_ptr()->nd_next_free() &&
        ( NIL_PAGE_NO_GG00 == this->np_ptr()->nd_prev_free()) || 0 == this->np_ptr()->nd_prev_free()));
    return (0 != this->np_ptr()->nd_in_free_chain()); 
}

/*---------------------------------------------------------------------------*/

inline bool         
cbd910_ObjNode::bd910LockSubRootBeforeReleasingFrame() const
{
	return  ((this->np_ptr()->nd_free_obj_cnt() >= (this->np_ptr()->nd_max_obj_cnt()-1)) ||
		(this->np_ptr()->nd_free_obj_cnt() == 0) || !this->np_ptr()->nd_in_free_chain() );
}; 


/*---------------------------------------------------------------------------*/

inline void
cbd910_ObjDataNode::bd910_CheckFreePageCnt (cbd910_SubRootNode &NptrsSubRoot)
{    
    int MinFreePageCnt = 0;
    int MaxFreePageCnt = 0;
    
    if (NptrsSubRoot.np_ptr()->nd_in_free_chain())
        ++MinFreePageCnt;
    
    if (NptrsSubRoot.np_ptr()->nd_next_free() == NIL_PAGE_NO_GG00)
        MaxFreePageCnt = MinFreePageCnt;
    else
    {
        ++MinFreePageCnt;
        if (NptrsSubRoot.np_ptr()->nd_id() != np_ptr()->nd_id())
        {
            if (NptrsSubRoot.np_ptr()->nd_next_free() == np_ptr()->nd_id())
            {
                if (np_ptr()->nd_next_free() == NIL_PAGE_NO_GG00)
                    MaxFreePageCnt = MinFreePageCnt;
                else
                    ++MinFreePageCnt;
            }
            else
            { 
                if (np_ptr()->nd_in_free_chain())
                    ++MinFreePageCnt;
                
                if (np_ptr()->nd_next_free() != NIL_PAGE_NO_GG00)
                    ++MinFreePageCnt;
            }
        }
    }
    
//    if ((MinFreePageCnt > SubRootPageInfo.bd900FreePageCnt()) ||
//        ((MaxFreePageCnt !=0) && (MaxFreePageCnt < SubRootPageInfo.bd900FreePageCnt())))
//    {
//        g01opmsg (sp3p_knldiag, sp3m_error, csp3_bd_msg, csp3_n_obj,
//            "BD910InsFree:Odd FreeCnt",  	np_ptr()->nd_id());
//    }
}

/*---------------------------------------------------------------------------*/
/* PTS 1115760 2002-05-15 FF                                                 */
inline void 
cbd910_SubRootNode::bd910RemovePageFromFreeChain (cbd910_ObjDataNode      &NptrsPage,
                                                  const Converter_Version &converterVersion,
                                                  FileDir_Oms             &fd_smart,
                                                  bool                     bNoReInsert)
{
    ROUTINE_DBG_MEO00 ("bd910RemovePageFromFreeChain");
    
    /* check preconditions */
    SAPDBERR_ASSERT_STATE (NptrsPage.np_ptr()->nd_in_free_chain());
    SAPDBERR_ASSERT_STATE (((NptrsPage.np_ptr()->nd_free_obj_cnt() == NptrsPage.np_ptr()->nd_max_obj_cnt())) ||
        (NptrsPage.np_ptr()->nd_objpage_state().includes(objp_no_chain_to_freelist)) ||
        ((NptrsPage.np_ptr()->nd_first_free_obj() == NIL_OBJ_PAGE_POS_GG92)     &&
        (NptrsPage.np_ptr()->nd_free_obj_cnt()   == 0)));

    if ( ObjContainer_Trace.TracesLevel(4) )
    {
        Kernel_VTrace() << __FILE__ << ":" << __LINE__ 
            << " -->bd910RemovePageFromFreeChain"
            << " Page:" << NptrsPage.np_ptr()->nd_id()
            << " NextPage:" << NptrsPage.np_ptr()->nd_next_free()
            << " PrevPage:" << NptrsPage.np_ptr()->nd_prev_free()
            << " Root:" << this->np_ptr()->nd_id()
            << " FreeCnt:" << NptrsPage.np_ptr()->nd_free_obj_cnt()
            << " MaxCnt:" << NptrsPage.np_ptr()->nd_max_obj_cnt()
            << " InFreeChain:" << NptrsPage.np_ptr()->nd_in_free_chain();
    }

    /* PTS 1127804 FF 2004-FEB-17 */
    if ( NptrsPage.np_ptr()->nd_in_free_chain()) 
    {
        /* remove page from free page chain */
        /* during chaining the subroot is never released. Therefore no other task may change  */
        /* any page chain during this operation, because evry chaining is forced to allocated */
        /* the corresponding subroot first !                                   FF 2004-FEB-02 */
        if (this->np_ptr()->nd_id() != NptrsPage.np_ptr()->nd_id())
        {
            if ( this->np_ptr()->nd_id() != NptrsPage.np_ptr()->nd_prev_free() )
            {
                cbd910_ObjDataNode   NptrsPrevPage (*m_ptrCurrent, nr_for_update);
                NptrsPage.bd910GetAnotherObjPage (NptrsPage.np_ptr()->nd_prev_free(), 
                    MAX_LOCK_REQUEST_SEQ_NO_BD900, NptrsPrevPage);
                if (m_ptrCurrent->curr_trans->trError_gg00 != e_ok) 
                {
                    if ( ObjContainer_Trace.TracesLevel(4) )
                    {
                        Kernel_VTrace () << __FILE__ << ":" << __LINE__
                            << " Error:" << m_ptrCurrent->curr_trans->trError_gg00
                            << " trying to access prev free page:" << NptrsPage.np_ptr()->nd_prev_free()
                            << " Page Pno:" << NptrsPage.np_ptr()->nd_id()
                            << " Root:" << this->np_ptr()->nd_id();
                    }
                    return;
                }

                /* PTS 1127493 FF 2004-FEB-02 */
                if (NptrsPage.np_ptr()->nd_id() != NptrsPrevPage.np_ptr()->nd_next_free())
                {
                    Kernel_OpError ( csp3_bd_msg, csp3_n_obj )
                        << __FILE__ << ":" << __LINE__
                        << " Freechain corrupt"
                        << " Prevpage Pno:" << NptrsPrevPage.np_ptr()->nd_id()
                        << " Prevpage nextfree:" << NptrsPrevPage.np_ptr()->nd_next_free()
                        << " Page Pno:" << NptrsPage.np_ptr()->nd_id()
                        << " Page prevfree:" << NptrsPage.np_ptr()->nd_prev_free()
                        << " Root:" << this->np_ptr()->nd_id();
                    g01abort (csp3_bd_msg, csp3_n_obj, "bd910RmvePageFrFreeChn  ", 0);
                }

                /* check if preconditions are still true, else return */
                if (!((NptrsPage.np_ptr()->nd_free_obj_cnt() == 0 && 
                    NptrsPage.np_ptr()->nd_first_free_obj() == NIL_OBJ_PAGE_POS_GG92) ||
                    NptrsPage.np_ptr()->nd_free_obj_cnt() == NptrsPage.np_ptr()->nd_max_obj_cnt())) 
                {
                    return;
                }
                NptrsPrevPage.np_ptr()->nd_next_free() = NptrsPage.np_ptr()->nd_next_free();
            }
            else
            {
                this->np_ptr()->nd_next_free() = NptrsPage.np_ptr()->nd_next_free();

                if ( ObjContainer_Trace.TracesLevel(4) )
                {
                    Kernel_VTrace() << __FILE__ << ":" << __LINE__ 
                        << " PREV Page(root):" << this->np_ptr()->nd_id()
                        << " NextPage:" << this->np_ptr()->nd_next_free()
                        << " PrevPage:" << this->np_ptr()->nd_prev_free()
                        << " FreeCnt:" << this->np_ptr()->nd_free_obj_cnt()
                        << " MaxCnt:" << this->np_ptr()->nd_max_obj_cnt()
                        << " InFreeChain:" << this->np_ptr()->nd_in_free_chain();
                }
            }

            if ( NIL_PAGE_NO_GG00 != NptrsPage.np_ptr()->nd_next_free())
            {
                cbd910_ObjDataNode   NptrsNextPage (*m_ptrCurrent, nr_for_update);
                NptrsPage.bd910GetAnotherObjPage (NptrsPage.np_ptr()->nd_next_free(), 
                    MAX_LOCK_REQUEST_SEQ_NO_BD900, NptrsNextPage);
                if (m_ptrCurrent->curr_trans->trError_gg00 != e_ok) 
                {
                    if ( ObjContainer_Trace.TracesLevel(4) )
                    {
                        Kernel_VTrace () << __FILE__ << ":" << __LINE__
                            << " Error:" << m_ptrCurrent->curr_trans->trError_gg00
                            << " trying to access next free page:" << NptrsPage.np_ptr()->nd_next_free()
                            << " Page Pno:" << NptrsPage.np_ptr()->nd_id()
                            << " Root:" << this->np_ptr()->nd_id();
                    }
                    return;
                }

                /* PTS 1127493 FF 2004-FEB-02 */
                if (NptrsPage.np_ptr()->nd_id() != NptrsNextPage.np_ptr()->nd_prev_free())
                {
                    Kernel_OpError ( csp3_bd_msg, csp3_n_obj )
                        << __FILE__ << ":" << __LINE__
                        << " Freechain corrupt"
                        << " Nextpage Pno:" << NptrsNextPage.np_ptr()->nd_id()
                        << " Nextpage prevfree:" << NptrsNextPage.np_ptr()->nd_prev_free()
                        << " Page Pno:" << NptrsPage.np_ptr()->nd_id()
                        << " Page nextfree:" << NptrsPage.np_ptr()->nd_next_free()
                        << " Root:" << this->np_ptr()->nd_id();
                    g01abort (csp3_bd_msg, csp3_n_obj, "bd910RmvePageFrFreeChn  ", 0);
                }

                NptrsNextPage.np_ptr()->nd_prev_free() = NptrsPage.np_ptr()->nd_prev_free();

                if ( ObjContainer_Trace.TracesLevel(4) )
                {
                    Kernel_VTrace() << __FILE__ << ":" << __LINE__ 
                        << " NEXT Page:" << NptrsNextPage.np_ptr()->nd_id()
                        << " NextPage:" << NptrsNextPage.np_ptr()->nd_next_free()
                        << " PrevPage:" << NptrsNextPage.np_ptr()->nd_prev_free()
                        << " FreeCnt:" << NptrsNextPage.np_ptr()->nd_free_obj_cnt()
                        << " MaxCnt:" << NptrsNextPage.np_ptr()->nd_max_obj_cnt()
                        << " InFreeChain:" << NptrsNextPage.np_ptr()->nd_in_free_chain();
                }
            }

            NptrsPage.np_ptr()->nd_next_free() = NIL_PAGE_NO_GG00;
            NptrsPage.np_ptr()->nd_prev_free() = NIL_PAGE_NO_GG00;
        }

        // if ((this->np_ptr()->nd_id() != NptrsPage.np_ptr()->nd_id()) 
        //    || (0 == this->np_ptr()->nd_free_obj_cnt()))
        {
            NptrsPage.np_ptr()->nd_in_free_chain() = false; /* subroot stays within "free chain" */

            fd_smart.ModifyFreePageCount(m_ptrCurrent->curr_trans->trTaskId_gg00,converterVersion, -1);   
            if ( ObjContainer_Trace.TracesLevel(4) )
            {
                Kernel_VTrace() << __FILE__ << ":" << __LINE__ 
                    << " bd910RemovePageFromFreeChain:ModifyFreePageCount -1"
                    << " root:"
                    << this->np_ptr()->nd_id();
            }
            //fd_smart.ModifyLeafPageCount(m_ptrCurrent->curr_trans->trTaskId_gg00,converterVersion, -1); 
        }

        /* PTS 1127493 FF 2004-FEB-02                                            */
        /*     because nd_free_obj_cnt may have been changed by some other task  */
        /* during this operation, check if still precondition for this procedure */
        /* are valid. If not -> re-insert page into freechain to guarantee the   */
        /* consistency of page-header information                                */
        if ( !NptrsPage.bd910IsEmptyPage() && NptrsPage.bd910PageMustBeInsertedIntoFreeChain()
			&& !bNoReInsert)
        {
            if ( ObjContainer_Trace.TracesLevel(4) )
                Kernel_VTrace() << __FILE__ << ":" << __LINE__ 
                << " re-insert page to free-chain";
            bd910InsertPageIntoFreeChain( NptrsPage, converterVersion, fd_smart );
        }

        /* perform some simple checks */
#       if COMPILEMODE_MEO00 >= QUICK_MEO00 	
        NptrsPage.bd910_CheckFreePageCnt (*this);
#       endif
    }

    if ( ObjContainer_Trace.TracesLevel(4) )
    {
        Kernel_VTrace() << __FILE__ << ":" << __LINE__ 
            << " <--bd910RemovePageFromFreeChain"
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

inline void
cbd910_SubRootNode::bd910RestoreFreePageChain (cbd910_ObjDataNode      &NptrsPage,
                                               const Converter_Version &converterVersion,
                                               FileDir_Oms             &fd_smart)
{
    /* this function can be called to restore the free page chain  */
    /* after an object has been reserved on the first free page in */
    /* the free page chain                                         */
    
    /* check preconditions */
    SAPDBERR_ASSERT_STATE (this->bd910PageIsAvailable());
    SAPDBERR_ASSERT_STATE (NptrsPage.bd910PageIsAvailable());
    SAPDBERR_ASSERT_STATE (NptrsPage.np_ptr()->nd_root() == this->np_ptr()->nd_id());
    
    if ((NptrsPage.np_ptr()->nd_free_obj_cnt() == 0) && (0 != NptrsPage.np_ptr()->nd_in_free_chain()))
    {
        SAPDBERR_ASSERT_STATE (NptrsPage.np_ptr()->nd_first_free_obj() == NIL_OBJ_PAGE_POS_GG92);
        this->bd910RemovePageFromFreeChain (NptrsPage, converterVersion, fd_smart, false /*NoReInsert*/);
    }
}

/*---------------------------------------------------------------------------*/

inline tbd900_ObjFrame*
cbd910_ObjDataNode::bd910ObjFramePtr (tsp00_Byte *pPageBegin,
                                      tsp00_Int4  ObjFramePos)
{
    return ((tbd900_ObjFrame *) (pPageBegin + ObjFramePos));   
}

/*---------------------------------------------------------------------------*/

inline tbd900_ObjFrame*
cbd910_ObjDataNode::bd910ObjFramePtr (tsp00_Int4 ObjFramePos)
{
    return bd910ObjFramePtr ((tsp00_Byte *) np_ptr(),ObjFramePos);
}


/*---------------------------------------------------------------------------*/

inline	bool                    
cbd910_ObjDataNode::bd910PageMustBeInsertedIntoFreeChain ()
{
    return ((this->np_ptr()->nd_free_obj_cnt() > 0) && 
        !this->np_ptr()->nd_in_free_chain());
}


/*---------------------------------------------------------------------------*/

inline	bool                    
cbd910_ObjDataNode::bd910PageMustBeRemovedFromFreeChain ()
{
    SAPDBERR_ASSERT_STATE (0 < this->np_ptr()->nd_max_obj_cnt());
    return ((this->np_ptr()->nd_free_obj_cnt() == 0) && 
	       this->np_ptr()->nd_in_free_chain());
}


/*---------------------------------------------------------------------------*/
/* METHODS of cbd910_ContObjDataNode                                         */
/*---------------------------------------------------------------------------*/

inline
cbd910_ContObjDataNode::cbd910_ContObjDataNode (cbd05_Current        &Current,
                                                tbd_node_request_Enum    NodeRequest)
                                                :cbd910_ObjDataNode (Current, NodeRequest)
{}

/*---------------------------------------------------------------------------*/

inline
cbd910_ContObjDataNode::cbd910_ContObjDataNode (cbd05_Current        &Current, 
                                                tsp00_PageNo             PageNo,
                                                tbd_node_request_Enum    NodeRequest,
                                                tbd00_PageLockMode_Enum  PageLockMode)
                                                :cbd910_ObjDataNode (Current, PageNo, NodeRequest, PageLockMode)
{}

/*---------------------------------------------------------------------------*/

inline void
cbd910_ContObjDataNode::bd910_CheckNodeType ()
{
    if (this->np_ptr()->nd_pt2() != pt2ContObject_egg00)   
    {
        g01abort (csp3_bd_msg, csp3_n_obj, "BD910_Check: No ContNode", np_ptr()->nd_id());
    }
}

/*---------------------------------------------------------------------------*/
/* METHODS of cbd910_PrimObjDataNode                                             */
/*---------------------------------------------------------------------------*/

inline
cbd910_PrimObjDataNode::cbd910_PrimObjDataNode (cbd05_Current        &Current,
                                                tbd_node_request_Enum    NodeRequest)
                                                :cbd910_ObjDataNode (Current, NodeRequest)
{}

/*---------------------------------------------------------------------------*/

inline
cbd910_PrimObjDataNode::cbd910_PrimObjDataNode (cbd05_Current        &Current, 
                                                tsp00_PageNo             PageNo,
                                                tbd_node_request_Enum    NodeRequest,
                                                tbd00_PageLockMode_Enum  PageLockMode)
                                                :cbd910_ObjDataNode (Current, PageNo, NodeRequest, PageLockMode)
{}

/*---------------------------------------------------------------------------*/

inline void
cbd910_PrimObjDataNode::bd910_CheckNodeType ()
{
    if ((this->np_ptr()->nd_pt2() != pt2Object_egg00)  &&
        (this->np_ptr()->nd_pt2() != pt2ContObject_egg00) && /* because of Rollforward_CreateObjFile (Cont) */
        (this->np_ptr()->nd_pt2() != pt2VarObject_egg00))
    {
        g01abort (csp3_bd_msg, csp3_n_obj, "BD910_Check: No PObjNode", np_ptr()->nd_id());
    }
}

/*---------------------------------------------------------------------------*/
/* METHODS of cbd910_SubRootNode                                         */
/*---------------------------------------------------------------------------*/

inline
cbd910_SubRootNode::cbd910_SubRootNode (cbd05_Current        &Current,
                                        tbd_node_request_Enum    NodeRequest)
                                        :cbd910_ObjNode (Current, NodeRequest)
{}

/*---------------------------------------------------------------------------*/

inline 
cbd910_SubRootNode::cbd910_SubRootNode (cbd05_Current        &Current, 
                                        tsp00_PageNo             PageNo,
                                        tbd_node_request_Enum    NodeRequest,
                                        tbd00_PageLockMode_Enum  PageLockMode)
                                        :cbd910_ObjNode (Current, PageNo, NodeRequest,
                                        (plmLockWithSVPSyncNoWait_ebd00 != PageLockMode))
{}

/*---------------------------------------------------------------------------*/

inline void
cbd910_SubRootNode::bd910_CheckNodeType ()
{
    if ((this->np_ptr()->nd_id()   != this->np_ptr()->nd_root())  ||
        ((this->np_ptr()->nd_pt2() != pt2ContObject_egg00)         &&
        (this->np_ptr()->nd_pt2() != pt2Object_egg00)             &&
        (this->np_ptr()->nd_pt2() != pt2VarObject_egg00)))
    {
        g01abort (csp3_bd_msg, csp3_n_obj, "BD910_Check: No SubRoot ", np_ptr()->nd_id());
    }
}

/*---------------------------------------------------------------------------*/
inline void
cbd910_SubRootNode::bd910AssignToSubroot (cbd910_ObjDataNode & NptrsPage)
{
    if (NptrsPage.np_ptr()->nd_root() == NptrsPage.np_ptr()->nd_id())
        this->bd910SetTo (NptrsPage);
    else
        NptrsPage.bd910GetAnotherObjPage (NptrsPage.np_ptr()->nd_root(), MAX_LOCK_REQUEST_SEQ_NO_BD900, *this);
}

/*---------------------------------------------------------------------------*/
/* METHODS of cbd910_RootNode                                                */
/*---------------------------------------------------------------------------*/

inline
cbd910_RootNode::cbd910_RootNode (cbd05_Current        &Current,
                                  tbd_node_request_Enum    NodeRequest)
                                  :cbd910_ObjNode (Current, NodeRequest)
{}

/*---------------------------------------------------------------------------*/

inline 
cbd910_RootNode::cbd910_RootNode (cbd05_Current        &Current, 
                                  tsp00_PageNo             PageNo,
                                  tbd_node_request_Enum    NodeRequest,
                                  tbd00_PageLockMode_Enum  PageLockMode)
                                  :cbd910_ObjNode (Current, PageNo, NodeRequest,
                                  (plmLockWithSVPSyncNoWait_ebd00 != PageLockMode))
{}

/*---------------------------------------------------------------------------*/

inline void
cbd910_RootNode::bd910_CheckNodeType ()
{
    if ((this->np_ptr()->nd_id()   != this->np_ptr()->nd_root())  ||
        ((this->np_ptr()->nd_pt2() != pt2ContObject_egg00)         &&
        (this->np_ptr()->nd_pt2() != pt2Object_egg00)             &&
        (this->np_ptr()->nd_pt2() != pt2VarObject_egg00)))
    {
        g01abort (csp3_bd_msg, csp3_n_obj, "BD910_Check: No    Root ", np_ptr()->nd_id());
    }
}

/*---------------------------------------------------------------------------*/

#endif  /* GBD910_H */
