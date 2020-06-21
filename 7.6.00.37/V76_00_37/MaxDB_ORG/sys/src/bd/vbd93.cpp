/****************************************************************************

  module      : vbd93.cpp

  -------------------------------------------------------------------------

  responsible : FerdiF

  special area: continuation files for handling objects of variable length
  description : description ...


  version     : 7.4.4.1
  last changed: 2003-09-08  11:49
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



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#ifndef GBD93_H
#   include "hbd93.h"
#endif
#ifndef GSP03_H
#   include "gsp03.h"
#endif
#ifndef GSP03_3_H
#   include "gsp03_3.h"
#endif
#ifndef GGG01_H
#   include "ggg01.h"
#endif
#ifndef GBD05_H
#   include "gbd05.h"
#endif
#ifndef HBD06_H
#   include "hbd06.h"      /* PTS 1127804 FF 2004-FEB-17 */
#endif
#ifndef GBD900_H
#   include "gbd900.h"
#endif
#ifndef GBD910_H
#   include "gbd910.h"
#endif
#ifndef GBD930_H
#   include "gbd930.h"
#endif
#ifndef HBD04_H
#   include "hbd04.h"
#endif
#ifndef HBD91_H
#   include "hbd91.h"
#endif
#ifndef HEO66_H
#   include "heo66.h"
#endif
#ifndef HGG01_1_H
#   include "hgg01_1.h"
#endif
#ifndef HGG01_3_H
#   include "hgg01_3.h"
#endif
#ifndef SAPDB_RANGECODE_HPP
#   include "SAPDB/SAPDBCommon/SAPDB_RangeCode.hpp" // Kernel_move_and_fill
#endif
#ifndef SAPDB_MEMCOPYMOVE_HPP
#   include "SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp"
#endif
#ifndef HKB53_H
#   include "hkb53.h"
#endif
#ifndef HKB67_H
#   include "hkb67.h" 
#endif
#ifndef LVC_TYPES_HPP
#   include "liveCache/LVC_Types.hpp"
#endif
#ifndef KERNEL_OPMSG_HPP
#   include "KernelCommon/Kernel_OpMsg.hpp"            /* PTS 1114891 FF 2002-03-14 */
#endif
#ifndef MSG_LIST_HPP
#   include "Messages/Msg_List.hpp"
#endif
#ifndef FILEDIR_OMS_HPP
#   include "FileDirectory/FileDir_Oms.hpp"
#endif
#ifndef FILEDIR_OMSVAR_HPP
#include "FileDirectory/FileDir_OmsVar.hpp"
#endif

#if COMPILEMODE_MEO00 >= SLOW_MEO00 
#include "hta99.h"
#endif

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#ifdef NO_INLINES
# define _INLINE
#else
# define _INLINE    inline
#endif

#define NO_ALIGNMENT 1

#if defined (BIT64)
# define  ALIGNMENT 8
#else
# define  ALIGNMENT 4
#endif

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/

extern cbd930_GarbCollController  bd91GarbCollController;

/*===========================================================================*
 *  LOCAL VARIABLES                                                          *
 *===========================================================================*/
 
/*===========================================================================*
 *  LOCAL FUNCTIONS (PROTOTYPES)                                             *
 *===========================================================================*/

static _INLINE void
bd93_CheckContObjRoot (const tgg92_KernelOid      &RootOId,         //[inp] 
					   const tbd900_ContObjHeader &ContObjHeader    //[inp]
					   );

/*---------------------------------------------------------------------------*/

_INLINE static void
bd93_FindFileNoForThisObjBodySize (
                                   FileDir_Oms        &fd_smart,       //[inp]
								   const tsp00_Uint4       NewObjBodySize, //[inp]  
								   const FileDir_FileNo   &ObjFileNo,      //[inp] 
								   tsp00_Int4             &ContFileNo,     //[out]
								   tgg00_BasisError       &TrError);       //[out]

/*---------------------------------------------------------------------------*/

_INLINE static void
bd93_GetContObjBodyPointer (cbd05_Current          &Current,         //[inp/out]
                            FileDir_OmsVar         &fd_var_smart,             
						    const tsp00_Int4        ContObjFileNo,   //[inp]     
						    const tsp00_PageNo      ContObjPno,      //[inp] 
						    const tgg00_ObjPagePos  ContObjPos ,     //[inp]
						    const tgg92_KernelOid  &OId,             //[inp] 
						    tgg00_MessType_Enum     MessType,        //[inp]
						    cbd05_Current          &ContCurrent,     //[out]
						    cbd910_ContObjDataNode &NptrsBody,       //[out]
							tsp00_MoveObjPtr       &pObjBody);       //[out]

/*---------------------------------------------------------------------------*/

static void 
bd93_NewContObj (cbd05_Current           &Current,                 // [inp]
                 tsp00_Int4               ContFileNo,              // [inp]
                 const tgg92_KernelOid   &RootOId,                 // [inp] 
                 FileDir_OmsVar          &fd_smart,                // [inp]
                 cbd05_Current           &ContCurrent,             // [out]
                 cbd910_ContObjDataNode  &NptrsBody,               // [out]
                 tgg00_ObjPagePos        &ContObjPos,              // [out]
                 Msg_List    &ErrorList);

/*---------------------------------------------------------------------------*/
			
_INLINE static void 
bd93_NewContObjFromThisPage (cbd05_Current        &Current,                  // [inp]
							 cbd910_ContObjDataNode  &NptrsBody,                // [inp]
							 const tgg92_KernelOid   &RootOId,                  // [inp]
							 tgg00_ObjPagePos        &ContObjPos);              // [out]

/*---------------------------------------------------------------------------*/

_INLINE static double
bd93_OccupancyRate (int ObjBodyLen,
					int Alignment);

/*---------------------------------------------------------------------------*/

_INLINE static void
bd93_ReleaseContObjFrame (cbd05_Current           &Current,
                          const Converter_Version &converterVersion,
                          FileDir_OmsVar          &fd_cont_smart,
						  const tgg92_KernelOid   &OId,            
						  cbd910_ContObjDataNode  &NptrsPage,
						  const tgg00_ObjPagePos   ContObjPos);
	
/*===========================================================================*
 *  GLOBAL FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

externCpp void
bd93GetVarObjBody (cbd05_Current         &Current,            //[inp\out] 
                   FileDir_OmsVar        &fd_smart,
				   const tgg92_KernelOid &OId,                //[inp] 
				   tsp00_Int4             ExtObjBodySize,     //[inp]
				   void                  *pExtObjBody,        //[inp]
				   tbd900_ObjFrame       *pObjFrame)          //[inp]
{
    SAPDBTRACE_ROUTINE_DEBUG("bd93GetVarObjBody", ObjContainer_Trace, 5);

    /* this function copies the object body identified by       */ 
    /* the OId at the position given by the pointer pExtObjBody */

    tgg00_BasisError      &TrError    = Current.curr_trans->trError_gg00;

    if (pObjFrame->objHeader_bd900.ohdState_gg92 != obsOccupied_egg00)
    {
        SAPDB_RangeFill (__FILE__, 1, 
            ExtObjBodySize, pExtObjBody, 1, 
            ExtObjBodySize, NIL_OBJ_FILLCHAR_BD900, 
            TrError);
    }
    else
    {
        /*  move object body into output array assigned by pExtObjBody */

        /*  get pointer to the object body*/
        tsp00_MoveObj          *pObjBody;
        cbd05_Current           ContObjCurrent;	
        cbd910_ContObjDataNode  NptrsBody (ContObjCurrent, nr_for_read);
        tbd900_VarObjBody      &VarObjBody    = *((tbd900_VarObjBody*) (pObjFrame->objBody_bd900));
        const tsp00_Int4       &ObjBodySize   = VarObjBody.vbdHeader_bd900.vhdObjSize_bd900;
        const tsp00_Int4       &ContObjFileNo = VarObjBody.vbdHeader_bd900.vhdContObjFileNo_bd900;
        tsp00_Int4              MaxObjBodySize;
        const FileDir_FileNo ObjFileNo        = Current.curr_tree_id.fileDirFileId_gg00(); 
        const tsp00_Bool       &ObjectIsSplit = VarObjBody.vbdHeader_bd900.vhdObjectIsSplit_bd900; // PTS 1121823

        /* check if the given memory is large enough to accomodate the object */
        if (ExtObjBodySize < ObjBodySize)
        {
            Kernel_OpWarning( csp3_bd_msg, csp3_n_obj )
                << __FILE__ << ":" << __LINE__
                << " Wrong object version -"
                << " ExtObjBodySize:" << ExtObjBodySize 
                << " ObjBodySize:" << ObjBodySize
                << " ObjFileNo:" << ObjFileNo
                << " ContObjFileNo:" << ContObjFileNo
                << " OID=" << OId.gg92GetPno() << "." << OId.gg92GetPos() << " (vers " << OId.gg92GetFrameVers() << ")";
            TrError = e_wrong_object_version; //e_buffer_limit PTS 1114891 FF 2002-03-15 ;
            return;
        }
        tgg00_TransContext    &Trans        = *Current.curr_trans;

        /*  get pointer to the var object body */
        if (0==ContObjFileNo)
        {
            // Object is completely stored in the primary container
            pObjBody       = (tsp00_MoveObj*) VarObjBody.vbdBody_bd900;
            MaxObjBodySize = fd_smart.GetObjBodySize();
            SAPDB_RangeMove (__FILE__, 21, 
                MaxObjBodySize, ExtObjBodySize,
                pObjBody,    1,
                pExtObjBody, 1,
                ObjBodySize, 
                TrError);
        }
        else if (!ObjectIsSplit)
        {
            // Object is completely stored in the continuation container. This case should only occur
            // for objects which have been stored before the modification of PTS 1121823
            const tsp00_PageNo     &ContObjPno    = VarObjBody.vbdHeader_bd900.vhdContObjPno_bd900;
            const tgg00_ObjPagePos &ContObjPos    = VarObjBody.vbdHeader_bd900.vhdContObjPos_bd900;
            MaxObjBodySize = bd93GetContFileBodySizes(ContObjFileNo);
            bd93_GetContObjBodyPointer (Current, fd_smart, ContObjFileNo, ContObjPno, ContObjPos, OId,
                m_select, ContObjCurrent, NptrsBody, pObjBody);
            if (TrError != e_ok) return;

            SAPDB_RangeMove (__FILE__, 22, 
                MaxObjBodySize, ExtObjBodySize,
                pObjBody,    1,
                pExtObjBody, 1,
                ObjBodySize, 
                TrError);
        }
        else 
        {
            // Object is split across the primary and the continuation container

            // First read the portion stored in the primary container...
            pObjBody       = (tsp00_MoveObj*) VarObjBody.vbdBody_bd900;
            tsp00_Int4  PrimBodySize = fd_smart.GetObjBodySize();
            MaxObjBodySize = PrimBodySize;

            SAPDB_RangeMove (__FILE__, 23, 
                MaxObjBodySize, ExtObjBodySize,
                pObjBody,    1,
                pExtObjBody, 1,
                PrimBodySize, 
                TrError);

            // ... then read portion stored in the continuation container and copy it
            // beyond the data of the primary container
            const tsp00_PageNo     &ContObjPno    = VarObjBody.vbdHeader_bd900.vhdContObjPno_bd900;
            const tgg00_ObjPagePos &ContObjPos    = VarObjBody.vbdHeader_bd900.vhdContObjPos_bd900;
            MaxObjBodySize = bd93GetContFileBodySizes(ContObjFileNo);
            bd93_GetContObjBodyPointer (Current, fd_smart, ContObjFileNo, ContObjPno, ContObjPos, OId,
                m_select, ContObjCurrent, NptrsBody, pObjBody);
            if (TrError != e_ok) return;

            SAPDB_RangeMove (__FILE__, 24, 
                MaxObjBodySize, ExtObjBodySize,
                pObjBody,    1,
                pExtObjBody, 1 + PrimBodySize,
                ObjBodySize-PrimBodySize, 
                TrError);
        }
    }
}

/*---------------------------------------------------------------------------*/

externCpp void  
bd93LogDelVarObj (cbd05_Current         &Current,
                  tgg92_KernelOid        OId, 
                  tgg91_PageRef          WantedObjVers,      
                  tbd_nodeptr            pPage, 
                  tbd900_ObjFrame       *pObjFrame,
                  SAPDB_Int1             contVarObj,         
                  bool                   bOmsLayerReusedObj,    
                  Msg_List  &ErrorList)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd93LogDelVarObj", ObjContainer_Trace, 5);

    char buffer[8100];  // PTS 1121823

    /* This function writes the removing on var object into the log */

    /*  determine  pointer to the object body */
    cbd05_Current           ContObjCurrent;	
    tgg00_BasisError       &TrError      = Current.curr_trans->trError_gg00;
    cbd910_ContObjDataNode  NptrsBody (ContObjCurrent, nr_for_read);
    tbd900_VarObjBody      &VarObjBody    = *((tbd900_VarObjBody*) (pObjFrame->objBody_bd900));
    tsp00_Int4              ObjBodySize   = VarObjBody.vbdHeader_bd900.vhdObjSize_bd900;
    tsp00_Int4              ContObjFileNo = VarObjBody.vbdHeader_bd900.vhdContObjFileNo_bd900;
    tsp00_MoveObj          *pObjBody;
    const tsp00_Bool       &ObjectIsSplit = VarObjBody.vbdHeader_bd900.vhdObjectIsSplit_bd900; // PTS 1121823

    const FileDir_FileNo  ObjFileNo  = Current.curr_tree_id.fileDirFileId_gg00(); 
    tgg00_TransContext   &Trans      = *Current.curr_trans;
    FileDir_OmsVar         fd_smart;
    FileDir_ReturnCode     fd_rc;
    fd_rc = FileDir_IDirectory::SessionInstance(Trans).GetFile(
        Trans.trTaskId_gg00, ObjFileNo, fd_smart, ErrorList);
    if (FileDir_Okay != fd_rc) 
    {
        Kernel_OpError( csp3_bd_msg, csp3_n_obj ) 
            << __FILE__ << ":" << __LINE__ 
            << " ObjFileNo:" << ObjFileNo
            << " GetFile error:" << fd_rc;
        g01abort (csp3_bd_msg, csp3_n_obj, "bd93LogDelVarObj FilNfnd", fd_rc);
    }

    // Remember whether object is a head- or a continuation-object (PTS 1124278)
    switch (VarObjBody.vbdHeader_bd900.vhdObjectIsCont_bd900)
    {
    case 0:
		break;
    case 1:     
    case 2:
        if ( contVarObj != VarObjBody.vbdHeader_bd900.vhdObjectIsCont_bd900) {
            TrError = e_wrong_object_version;
            Kernel_OpError( csp3_bd_msg, csp3_n_obj ) << __FILE__ << " : " << __LINE__ 
                << " TrError:" << TrError
                << " FileNo:"  << ObjFileNo
                << " VarObject Marker not consistent"
                << " on page:" << VarObjBody.vbdHeader_bd900.vhdObjectIsCont_bd900
                << " wanted:" << contVarObj;
            if ((g01vtrace.vtrBdObject_gg00) || (g01vtrace.vtrOmsUpd_gg00 ))
            {
                Kernel_VTrace() << __FILE__ << " : " << __LINE__ 
                    << " TrError:" << TrError
                    << " FileNo:" << ObjFileNo
                    << " VarObject Marker not consistent"
                    << " on page:" << VarObjBody.vbdHeader_bd900.vhdObjectIsCont_bd900
                    << " wanted:" << contVarObj;
            }
        }
        break;
    }

    if (0==ContObjFileNo)
        // Object is completely stored in the primary container
        pObjBody = (tsp00_MoveObj*) VarObjBody.vbdBody_bd900;
    else if (!ObjectIsSplit)
    {
        // Object is completely stored in the continuation container
        const tsp00_PageNo     &ContObjPno = VarObjBody.vbdHeader_bd900.vhdContObjPno_bd900;
        const tgg00_ObjPagePos &ContObjPos = VarObjBody.vbdHeader_bd900.vhdContObjPos_bd900;
        bd93_GetContObjBodyPointer (Current, 
            fd_smart,
            ContObjFileNo, ContObjPno, ContObjPos, OId,
            m_select, ContObjCurrent,  NptrsBody, pObjBody);
    }
    else 
    {
        // Object is split across the primary and the continuation container (see PTS 1121823)

        tsp00_Int4  PrimBodySize = fd_smart.GetObjBodySize();

        // First read the portion stored in the primary container...
        pObjBody  = (tsp00_MoveObj*) VarObjBody.vbdBody_bd900;
        SAPDB_MemCopyNoCheck (&buffer[0], pObjBody, PrimBodySize);

        // ... then read portion stored in the continuation container and copy it
        // beyond the data of the primary container
        const tsp00_PageNo     &ContObjPno    = VarObjBody.vbdHeader_bd900.vhdContObjPno_bd900;
        const tgg00_ObjPagePos &ContObjPos    = VarObjBody.vbdHeader_bd900.vhdContObjPos_bd900;

        bd93_GetContObjBodyPointer (Current, fd_smart, ContObjFileNo, ContObjPno, ContObjPos, OId,
            m_select, ContObjCurrent, NptrsBody, pObjBody);

        SAPDB_MemCopyNoCheck (&buffer[PrimBodySize], pObjBody, ObjBodySize-PrimBodySize); 

        pObjBody = (tsp00_MoveObj*) &buffer[0];
    }

    /* check if object is locked and create a log entry  */ 
    kb67DelUpdObj (*Current.curr_trans, m_delete, Current.curr_tree_id,
        OId, pPage->nd_obj_key_len(), WantedObjVers,
        pObjFrame->objHeader_bd900, ObjBodySize, pObjBody,
        contVarObj,
        bOmsLayerReusedObj,
        ErrorList);                 
}

/*---------------------------------------------------------------------------*/
static const 
unsigned short VarContFileBodySize [] = { 126 ,252, 496, 1004, 2016, 4040, 8092 };

externCpp SAPDB_UInt1
bd93GetNumOfContFiles (void)
{
    return 6;
};

/*---------------------------------------------------------------------------*/

externCpp unsigned short
bd93GetContFileBodySizes (unsigned short index)
{
    if (index <= 6)
        return VarContFileBodySize[ index ];
    else 
        return 0;
}

/*---------------------------------------------------------------------------*/

externCpp void
bd93RemoveContObj (cbd05_Current           &Current,        //[inp]
                   const Converter_Version &converterVersion,
                   FileDir_OmsVar          &fd_smart,
                   tgg92_KernelOid         &OId,            //[inp]    
                   const tsp00_Int4         ContObjFileNo,  //[inp]     
                   const tsp00_PageNo       ContObjPno,     //[inp] 
                   const tgg00_ObjPagePos   ContObjPos)     //[inp]
{
    SAPDBTRACE_ROUTINE_DEBUG("bd93RemoveContObj", ObjContainer_Trace, 5);

    /* This function removes an  continuation object */

    tgg00_BasisError      &TrError   = Current.curr_trans->trError_gg00;

    /* construct currrent for the continuation objects */
    cbd05_Current ContCurrent (*Current.curr_trans, m_update);
    tgg00_TransContext    &Trans        = *Current.curr_trans;
    
    fd_smart.BuildContFileId(ContObjFileNo, ContCurrent.curr_tree_id);

	/* get pointer to the page where the object is located */
	cbd910_ContObjDataNode  NptrsBody(ContCurrent, ContObjPno, nr_for_update);	                                                                  
	if (TrError != e_ok) return;

	/* release continuation object which is not used any more, the Nptrs are released too */
	bd93_ReleaseContObjFrame (ContCurrent, converterVersion, fd_smart, OId, NptrsBody, ContObjPos);
}

/*---------------------------------------------------------------------------*/

externCpp void
bd93UpdateVarObjBody (cbd05_Current           &Current,
                      const Converter_Version &converterVersion,
                      FileDir_OmsVar          &fd_smart,
                      tgg92_KernelOid          OId,
                      tgg91_PageRef            ObjVers,
                      cbd910_PrimObjDataNode  &NptrsAnchor,
                      tbd900_ObjFrame         *&pObjFrame, 
                      tsp00_Int4               NewObjBodySize,
                      void                    *pNewObjBody,
                      bool                     bRollBack,
                      bool                    &bTooManyEmptyPagesInContFile,
                      tsp00_Int4              &NewContObjFileNo,
                      tsp00_Int4              &ContObjFileNo,
                      const Log_EntrySequence &RedoEntrySequence,
                      const tgg91_TransNo     &RedoUpdTransNo,
                      SAPDB_Int1               contObj,           
                      bool                     bOmsLayerReusedObj,
                      Msg_List    &ErrorList)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd93UpdateVarObjBody", ObjContainer_Trace, 5);

    /* This function moves the object given by the pointer  */
    /* pNewObjBody into the object frame given by the OId   */

    char Buffer[8100];  // PTS 1121823

    tgg00_TransContext     &Trans          = *Current.curr_trans;
    tgg00_BasisError       &TrError        = Current.curr_trans->trError_gg00;
    const FileDir_FileNo    ObjFileNo      = Current.curr_tree_id.fileDirFileId_gg00(); 
    bool                    bMoveBodyToAnotherContFile = false;
    tsp00_Int4              MaxObjBodySize = 0;
    tsp00_Int4              MinObjBodySize = 0;
    tsp00_MoveObj          *pObjBody       = NULL;
    tsp00_MoveObj          *pOldObjBody    = NULL;

    /* check frame state */
    if ((pObjFrame->objHeader_bd900.ohdState_gg92  != obsReserved_egg00   ) &&
        (pObjFrame->objHeader_bd900.ohdState_gg92  != obsOccupied_egg00   ))
    {   
        TrError = e_wrong_object_state;
        return;
    }

    cbd05_Current            ContObjCurrent;	
    cbd910_ContObjDataNode   NptrsBody (ContObjCurrent, nr_for_update);

    tsp00_Int4               PrimBodySize   = fd_smart.GetObjBodySize();
    tbd900_VarObjBodyHeader *pHdr           = &(((tbd900_VarObjBody*) pObjFrame->objBody_bd900)->vbdHeader_bd900);
    tsp00_MoveObj           *pPrimBody      = (tsp00_MoveObj*) &(((tbd900_VarObjBody*) pObjFrame->objBody_bd900)->vbdBody_bd900);

    ContObjFileNo = pHdr->vhdContObjFileNo_bd900;

    // Remember whether object is a head- or a continuation-object (PTS 1124278)
    switch (pHdr->vhdObjectIsCont_bd900)
    {
    case 0:
        pHdr->vhdObjectIsCont_bd900 = contObj;
		break;
    case 1:     
    case 2:
        if ( contObj != pHdr->vhdObjectIsCont_bd900) {
            TrError = e_wrong_object_version;
            Kernel_OpError( csp3_bd_msg, csp3_n_obj ) << __FILE__ << " : " << __LINE__ 
                << " TrError:" << TrError
                << " FileNo:"  << ObjFileNo
                << " VarObject Marker not consistent"
                << " on page:" << pHdr->vhdObjectIsCont_bd900
                << " wanted:" << contObj;
            if ((g01vtrace.vtrBdObject_gg00) || (g01vtrace.vtrOmsUpd_gg00 ))
            {
                Kernel_VTrace() << __FILE__ << " : " << __LINE__ 
                    << " TrError:" << TrError
                    << " FileNo:" << ObjFileNo
                    << " VarObject Marker not consistent"
                    << " on page:" << pHdr->vhdObjectIsCont_bd900
                    << " wanted:" << contObj;
            }
        }
        break;
    }

    if (PRIM_CONT_OBJ_FILE_NO_BD900 == pHdr->vhdContObjFileNo_bd900)
    {
        // Old object is stored completely in the primary container
        pObjBody       = pPrimBody;
        MaxObjBodySize = fd_smart.GetObjBodySize();
        MinObjBodySize = 0;
    }
    else if (!pHdr->vhdObjectIsSplit_bd900)
    {
        // Old object is stored completely in the continuation container
        bd93_GetContObjBodyPointer (Current, 
            fd_smart,
            pHdr->vhdContObjFileNo_bd900,
            pHdr->vhdContObjPno_bd900, 
            pHdr->vhdContObjPos_bd900, 
            OId, m_update, 
            ContObjCurrent, NptrsBody, pOldObjBody);
        if (TrError != e_ok) return;

        MaxObjBodySize =  bd93GetContFileBodySizes(pHdr->vhdContObjFileNo_bd900);
        if (ContObjFileNo > 1) 
        {
            //MinObjBodySize = (ContObjFileInfo[pHdr->vhdContObjFileNo_bd900-1].GetObjBodySize()*BALANCE_LIMIT_BD900)/100;
            MaxObjBodySize = bd93GetContFileBodySizes(pHdr->vhdContObjFileNo_bd900-1);
        }
        else
            //MinObjBodySize = (ObjFileInfo.GetObjBodySize()*BALANCE_LIMIT_BD900)/100;
            MaxObjBodySize = fd_smart.GetObjBodySize() + 1;

        /* store current ChainNo, for a later call of the garbage collector */ 

        pObjBody = pOldObjBody;  // PTS 1123495
    }
    else
    {
        // Old object is split across the primary and the continuation container.
        // Therefore the object must be merged. (PTS 1121823)

        bd93_GetContObjBodyPointer (Current, 
            fd_smart,
            pHdr->vhdContObjFileNo_bd900, 
            pHdr->vhdContObjPno_bd900, 
            pHdr->vhdContObjPos_bd900, 
            OId, m_update, 
            ContObjCurrent, NptrsBody, pOldObjBody);
        if (TrError != e_ok) return;

        MaxObjBodySize =  bd93GetContFileBodySizes(pHdr->vhdContObjFileNo_bd900);
        if (pHdr->vhdContObjFileNo_bd900 > 1)		
        {
            //MinObjBodySize = (ContObjFileInfo[pHdr->vhdContObjFileNo_bd900-1].GetObjBodySize()*BALANCE_LIMIT_BD900)/100;
            MinObjBodySize = bd93GetContFileBodySizes(pHdr->vhdContObjFileNo_bd900-1) + 1;
        }
        else
            //MinObjBodySize = (ObjFileInfo.GetObjBodySize()*BALANCE_LIMIT_BD900)/100;
            MinObjBodySize = fd_smart.GetObjBodySize() + 1;

        /* store current ChainNo, for a later call of the garbage collector */ 

        if (!bRollBack)
        {
            // First copy portion of primary container...
            SAPDB_MemCopyNoCheck(&Buffer[0], pPrimBody, PrimBodySize);

            // ...then copy portion of continuation container
            SAPDB_MemCopyNoCheck(&Buffer[PrimBodySize], pOldObjBody, pHdr->vhdObjSize_bd900 - PrimBodySize);

            pObjBody = (tsp00_MoveObj*) &Buffer[0];
        }
    }

    // find out where the new obj body has to be accomodated 
    // If the object fits into the primary container, the object is always (independend of
    // the setting of BALANCE_LIMIT_BD900) moved completely into the primary container
    NewContObjFileNo = pHdr->vhdContObjFileNo_bd900;
    if ((NewObjBodySize > MaxObjBodySize) || (NewObjBodySize < MinObjBodySize))
    {
        bMoveBodyToAnotherContFile = true;

        if (NewObjBodySize <= PrimBodySize)
            NewContObjFileNo = PRIM_CONT_OBJ_FILE_NO_BD900;	
        else if (NewObjBodySize <= 2*PrimBodySize)
            // Problem: If the remainder of the object (without the portion which is stored in
            //   the primary container is smaller than the upper bound for the primary container,
            //   then the following function would return the primary container.
            // Workaround: If the size is between PrimBodySize and 2*PrimBodySize, then the function
            //   is called with the original size. This prevent, that the primary container is returned
            bd93_FindFileNoForThisObjBodySize (fd_smart, NewObjBodySize, ObjFileNo, NewContObjFileNo, TrError);
        else
            // Get a container which is large enough to store the remainder of the object
            bd93_FindFileNoForThisObjBodySize (fd_smart, NewObjBodySize-PrimBodySize, ObjFileNo, NewContObjFileNo, TrError);		
        if (TrError != e_ok) return;
    }

    if (!bRollBack)
    {
        /* update object transinfo and write a log entry */
        kb67UpdObj (*Current.curr_trans, Current.curr_tree_id,
            OId, NptrsAnchor.np_ptr()->nd_obj_key_len(), ObjVers,
            pObjFrame->objHeader_bd900, pHdr->vhdObjSize_bd900, pObjBody,
            RedoEntrySequence, RedoUpdTransNo,
            contObj, bOmsLayerReusedObj, ErrorList);    		
        if (TrError != e_ok) return;
    }

    if (PRIM_CONT_OBJ_FILE_NO_BD900 == NewContObjFileNo)
    {
        if (bMoveBodyToAnotherContFile)
        {
            /* release the current body frame and page pointer     */ 
        	bd93_ReleaseContObjFrame (ContObjCurrent, converterVersion, fd_smart, OId, NptrsBody, pHdr->vhdContObjPos_bd900);

            // Reset pointer to continuation container
            pHdr->vhdContObjFileNo_bd900 = PRIM_CONT_OBJ_FILE_NO_BD900;
            pHdr->vhdContObjPno_bd900    = NIL_PAGE_NO_GG00;
            pHdr->vhdContObjPos_bd900    = NIL_OBJ_PAGE_POS_GG92;
        }

        // update var object header 
        pHdr->vhdObjSize_bd900       = NewObjBodySize;
        pHdr->vhdObjectIsSplit_bd900 = false;  // PTS 1121823

        // update object in primary container
        if ( NewObjBodySize > 0 )
        {
            SAPDB_RangeMove (__FILE__, 31,
                NewObjBodySize,  
                fd_smart.GetObjBodySize(),
                pNewObjBody,  1, 
                pPrimBody,    1,
                NewObjBodySize,  
                TrError); 
        }
    }
    else
    {
        // First update data in primary container
        if ( PrimBodySize > 0 )
        {
            SAPDB_RangeMove (__FILE__, 32,
                PrimBodySize,  
                fd_smart.GetObjBodySize(),
                pNewObjBody, 1, 
                pPrimBody,   1,
                PrimBodySize,  
                TrError); 
        }

        // Update data in continuation container
        if (bMoveBodyToAnotherContFile)
        {

            /* release the current body frame and page pointer     */ 
            if (PRIM_CONT_OBJ_FILE_NO_BD900 != pHdr->vhdContObjFileNo_bd900)
                bd93_ReleaseContObjFrame (ContObjCurrent, converterVersion, fd_smart, OId, NptrsBody, pHdr->vhdContObjPos_bd900);

            /* get a new object frame of an appropriate length */
            bd93_NewContObj (Current, NewContObjFileNo, OId, fd_smart,
                ContObjCurrent, NptrsBody, pHdr->vhdContObjPos_bd900, ErrorList);
            if (TrError != e_ok) return;

            /* update var object header */
            pHdr->vhdContObjPno_bd900    = NptrsBody.np_ptr()->nd_id();
            //pHdr->vhdContObjPos_bd900  = ;  // changed already by call of bd93_NewContObj
            pHdr->vhdContObjFileNo_bd900 = NewContObjFileNo;

            // Get pointer to new destination
            const tbd900_ContObjFrame *pContObjFrame = (tbd900_ContObjFrame *) ((char*) NptrsBody.np_ptr() + pHdr->vhdContObjPos_bd900);
            pObjBody       = (tsp00_MoveObj *) (pContObjFrame->cofBody_bd900);
            MaxObjBodySize = bd93GetContFileBodySizes(NewContObjFileNo);
        }
        else
        {
            // Get pointer to old destination
            pObjBody = pOldObjBody;
            MaxObjBodySize = bd93GetContFileBodySizes(pHdr->vhdContObjFileNo_bd900);
        }

        // Update header
        pHdr->vhdObjectIsSplit_bd900 = true;   // PTS 1121823
        pHdr->vhdObjSize_bd900       = NewObjBodySize;

        // Update object
        SAPDB_RangeMove (__FILE__, 33,
            NewObjBodySize-PrimBodySize,  MaxObjBodySize, 
            (tsp00_MoveObj*) pNewObjBody + PrimBodySize, 1, 
            pObjBody,                                    1,
            NewObjBodySize-PrimBodySize,  
            TrError); 
    }
}

/*===========================================================================*
 *  LOCAL FUNCTIONS (CODE)                                                   *
 *===========================================================================*/
				 
static _INLINE void
bd93_CheckContObjRoot (
					   const tgg92_KernelOid      &RootOId,         //[inp] 
					   const tbd900_ContObjHeader &ContObjHeader    //[inp]
					   )
{
	ROUTINE_DBG_MSP00 ("bd93_CheckContObjRoot");
	
	/* This function checks if the given RootOid is really */ 
	/* the root of the given continuation object frame     */ 	
	
	if ((RootOId.gg92GetPos()  != ContObjHeader.cvoRootOIdPos_bd900) ||
		(RootOId.gg92GetPno()  != ContObjHeader.cvoRootOIdPno_bd900) ||
		(NIL_OBJ_PAGE_POS_GG92 != ContObjHeader.cvoNextFreeObj_bd900))
	{
		/* report error in knldiag and vtrace */
		g01optextmsg (sp3p_knldiag, sp3m_error, csp3_bd_msg, csp3_n_obj, 
			"BD93_Check: RootOId <> ContObj.RootOId  ");
		
		g01opmsg (sp3p_knldiag, sp3m_error, csp3_bd_msg, csp3_n_obj, 
			"           RootOId.Page=", RootOId.gg92GetPno() );
		g01opmsg (sp3p_knldiag, sp3m_error, csp3_bd_msg, csp3_n_obj, 
			"   ContObj.RootOId.Page=",  ContObjHeader.cvoRootOIdPno_bd900);
		g01opmsg (sp3p_knldiag, sp3m_error, csp3_bd_msg, csp3_n_obj, 
			"            RootOId.Pos=", RootOId.gg92GetPos() );
		g01opmsg (sp3p_knldiag, sp3m_error, csp3_bd_msg, csp3_n_obj, 
			"    ContObj.RootOId.Pos=", ContObjHeader.cvoRootOIdPos_bd900);
		g01opmsg (sp3p_knldiag, sp3m_error, csp3_bd_msg, csp3_n_obj, 
			"  ContObj.RootOId.NextF=", ContObjHeader.cvoRootOIdPos_bd900);
	}
}

/*---------------------------------------------------------------------------*/

_INLINE static void
bd93_FindFileNoForThisObjBodySize (
                                   FileDir_Oms           &fd_smart,       //[inp]
                                   const tsp00_Uint4       NewObjBodySize, //[inp]  
								   const FileDir_FileNo   &ObjFileNo,    
								   tsp00_Int4             &ContFileNo,     //[out]
								   tgg00_BasisError       &TrError)        //[out]
{
	ROUTINE_DBG_MSP00 ("bd93_FindFileNoForNewBody");
	
	/* This function determines the number of the file which has the right  */
	/* object frames to accomodate objects of the given size NewObjBodySize */
	
    /* check if the object fits into the primary var object file */
    if (NewObjBodySize <= fd_smart.GetObjBodySize())
	{
		ContFileNo = PRIM_CONT_OBJ_FILE_NO_BD900;
		return;
	}
	else
	{
		/* find the suiting continuation file */
		/* check if there really exists an entry in the continuation file directory */
        ContFileNo = (MAX_NUM_CONT_OBJ_FILES_BD900 +1)/2;

		bool        bFound = false;	
		tsp00_Int4  Delta  = ContFileNo;		
		do
		{   
			if (NewObjBodySize <= bd93GetContFileBodySizes(ContFileNo))
			{
				if ((ContFileNo==1) ||
					(NewObjBodySize > 
                    bd93GetContFileBodySizes(ContFileNo-1)))
				{
					bFound = true;
				}
				else
				{
					Delta       = (Delta+1)/2;
					ContFileNo -=  Delta;
				}
			}
			else
			{
				Delta       = (Delta+1)/2;
				ContFileNo +=  Delta;
				if (ContFileNo > MAX_NUM_CONT_OBJ_FILES_BD900)
				{
					g01opmsg (sp3p_knldiag, sp3m_error, csp3_bd_msg, csp3_n_obj, 
						"BD93_Find:WrongVarObjLen", NewObjBodySize);
					TrError = e_illegal_object_length;
					return;
				}
			}
		}
		while (!bFound);
	}
}
		
		
/*---------------------------------------------------------------------------*/

_INLINE static void
bd93_GetContObjBodyPointer (cbd05_Current          &Current,         //[inp/out]
                            FileDir_OmsVar         &fd_smart,             
                            const tsp00_Int4        ContObjFileNo,   //[inp]     
                            const tsp00_PageNo      ContObjPno,      //[inp] 
                            const tgg00_ObjPagePos  ContObjPos ,     //[inp]
                            const tgg92_KernelOid  &OId,             //[inp] 
                            tgg00_MessType_Enum     MessType,        //[inp]
                            cbd05_Current          &ContCurrent,     //[out]
                            cbd910_ContObjDataNode &NptrsBody,       //[out]
                            tsp00_MoveObjPtr       &pObjBody)        //[out]
{
	ROUTINE_DBG_MSP00 ("bd93_GetContObjBodyPointer");
	
	/* this function supplies the pointer to an continuation object body     */
	/* which is derived from the page number and page position of the object */
	
	tbd900_ContObjFrame     *pContObjFrame; 
	tgg00_BasisError        &TrError   = Current.curr_trans->trError_gg00;
    tgg00_TransContext      &Trans     = *Current.curr_trans;
	tgg00_FileId             ObjFileId;	

    fd_smart.BuildContFileId(ContObjFileNo, ObjFileId);
    ContCurrent.bd05SetCurrent (*Current.curr_trans, MessType, ObjFileId);
	NptrsBody.bd910AssignToPage (ContObjPno);
	if (TrError != e_ok) return;
	
	/* get object pointer */
	pContObjFrame = (tbd900_ContObjFrame *) ((char*) NptrsBody.np_ptr() + ContObjPos);
	pObjBody      = (tsp00_MoveObj *) (pContObjFrame->cofBody_bd900);
	
	/* check if the found object frame belongs to the root OId */
	if (g01vtrace.vtrCheck_gg00)
        bd93_CheckContObjRoot (OId, pContObjFrame->cofHeader_bd900); 
}

/*---------------------------------------------------------------------------*/

static void 
bd93_NewContObj (cbd05_Current           &Current,                 // [inp]
				 tsp00_Int4               ContFileNo,              // [inp]
				 const tgg92_KernelOid   &RootOId,                 // [inp] 
                 FileDir_OmsVar          &fd_smart,             
				 cbd05_Current           &ContCurrent,             // [out]
				 cbd910_ContObjDataNode  &NptrsBody,               // [out]
				 tgg00_ObjPagePos        &ContObjPos,              // [out]
                 Msg_List                &ErrorList)
{
    ROUTINE_DBG_MSP00 ("bd93_NewContObj");

    /* This function yields a new continuation object frame */
    /* which is identified by ContObjPno and ContObjPos     */
    /* The new frame is searched in the the free chain.     */

    tgg00_BasisError       &TrError   = Current.curr_trans->trError_gg00;
    tgg00_TransContext     &Trans     = *Current.curr_trans;
    tgg00_FileId            ObjFileId;

    fd_smart.BuildContFileId(ContFileNo, ObjFileId);
    ContCurrent.bd05SetCurrent (*Current.curr_trans, m_update, ObjFileId);

    const Converter_Version &converterVersion = Converter_ICommon::Instance().Version();

    /* get handle for the current subroot */
	Data_PageNo    SubRoot    = fd_smart.GetContPartition(ContFileNo); 
    cbd910_SubRootNode   NptrsSubRoot (ContCurrent, SubRoot, nr_for_update);
    if (TrError != e_ok) 
    {
        Kernel_OpError( csp3_bd_msg, csp3_n_obj )
            << __FILE__ << ":" << __LINE__
            << " error:" << TrError
            << " accessing root:" << SubRoot
            << " ContFile FileId:" << ContFileNo
            << " VarFile FileId:" << fd_smart.GetFileNo();
        g01opmsg (sp3p_knldiag, sp3m_error, csp3_bd_msg, csp3_n_obj,   
            "bd93new: SubRoot=       ", SubRoot);
        return;
    }
	
#   if COMPILEMODE_MEO00 >= QUICK_MEO00 
    /* check if this subroot page belongs really to this file */
    if (NptrsSubRoot.np_ptr()->nd_root() != fd_smart.GetContPartition(ContFileNo)) 
    {
        FileDir_FileNo fileno;
        bd900GetFileNo( NptrsSubRoot.np_ptr(), fileno);
        Kernel_OpError( csp3_bd_msg, csp3_n_obj )
            << __FILE__ << ":" << __LINE__
            << " page doesn't belong to VarFile with FileId:" << fileno
            << " FileId on page:" << fileno 
            << " root:" << fd_smart.GetContPartition(ContFileNo)
            << " root on page::" << NptrsSubRoot.np_ptr()->nd_root() 
            << " subroot:" << SubRoot
            << " ContFileIdx:" << ContFileNo;
        g01abort (csp3_bd_msg, csp3_n_obj, 
            "BD93_NewObj:wrong SRoot ", NptrsSubRoot.np_ptr()->nd_id());
    }
#   endif

    /* get page handle for the first page with free objects */
    /* CRS 1106534 Alexander Kley 2000-05-11                */
    if (NptrsSubRoot.np_ptr()->nd_free_obj_cnt() > 0)
		NptrsBody.bd910SetTo (NptrsSubRoot, true /*movePrimary PTS 1130598*/);
    else
    {
		if (NIL_PAGE_NO_GG00 != NptrsSubRoot.np_ptr()->nd_next_free())
			NptrsBody.bd910AssignToPage (NptrsSubRoot.np_ptr()->nd_next_free());
		else
			NptrsBody.bd910AssignToNewPageInDataChain (NptrsSubRoot, false /*insert into freelist*/,
			Trans.trTaskId_gg00, fd_smart, converterVersion);
		if (e_ok != TrError) return;
	}
	
	/* get a new OId and initialize the corresponding object */
	bd93_NewContObjFromThisPage (ContCurrent, NptrsBody, RootOId, ContObjPos);
    if (e_ok != TrError) return;

    /* remove page from free page chain if there are no more free object frame on the page */
    NptrsSubRoot.bd910RestoreFreePageChain (NptrsBody, converterVersion, fd_smart); 
}

/*---------------------------------------------------------------------------*/

_INLINE static void 
bd93_NewContObjFromThisPage (cbd05_Current        &Current,                  // [inp]
							 cbd910_ContObjDataNode  &NptrsBody,                // [inp]
							 const tgg92_KernelOid   &RootOId,                  // [inp]
							 tgg00_ObjPagePos        &ContObjPos)               // [out]  
{
	ROUTINE_DBG_MSP00 ("bd93_NewContObjFromThisPage");
	
	/*  This routine finds a free frame on a page presuming that at least one free  */
	/*  object frame exists. Moreover all pointers are restored corresponding       */
	/*  to changing the state of the frame whichs OId is returned                   */
	
	tbd900_ContObjFrame   *pContObjFrame; 
	tgg00_BasisError      &TrError   = Current.curr_trans->trError_gg00;
    tbd_nodeptr           &pPage     = NptrsBody.np_ptr();

    /* (PTS 1134505) check if the position of the first free obj is valid   */
    if ( !bd92ObjPosIsValid (pPage->nd_first_free_obj(), pPage->nd_first_obj(), pPage->nd_obj_frame_len()))
    {
        Kernel_OpError ( csp3_bd_msg, csp3_n_obj ) << __FILE__ << ":" << __LINE__
            << " wrong position: " << pPage->nd_first_free_obj()
            << " page:" << pPage->nd_id()
            << " first free pos: " << pPage->nd_first_free_obj()
            << " page dumped into: d" 
            << pPage->nd_id() << ".obj"; 
        b06dump_bad_page (Current.curr_trans->trTaskId_gg00,'d', ".obj", pPage->nd_id(), 
            (void *) NptrsBody.np_ptr(), 1);

        if ( bd92ObjPosIsValid (pPage->nd_first_free_obj(), pPage->nd_first_obj(), pPage->nd_obj_frame_len()))
            TrError = e_wrong_object_state;
        else
            TrError = e_illegal_object_pos;
        return;
    }
 		
	/*  construct position of the object frame  header             */
	pContObjFrame = (tbd900_ContObjFrame *)	((char*) pPage + pPage->nd_first_free_obj());
	
	/*  get position of free object frame */
	ContObjPos   = pPage->nd_first_free_obj();
	
	/*  update page header */
	--pPage->nd_free_obj_cnt();
	++pPage->nd_occ_obj_cnt();
	pPage->nd_first_free_obj() = pContObjFrame->cofHeader_bd900.cvoNextFreeObj_bd900;
	
	/*  update object frame header */
	pContObjFrame->cofHeader_bd900.cvoNextFreeObj_bd900 = NIL_OBJ_PAGE_POS_GG92;
	pContObjFrame->cofHeader_bd900.cvoRootOIdPno_bd900  = RootOId.gg92GetPno();
	pContObjFrame->cofHeader_bd900.cvoRootOIdPos_bd900  = RootOId.gg92GetPos();
	
	/*   initialize the object body, i.e. the first four bytes     */
	/*   of the body are filled with hex zeros and the body header */
	/*   of variable length objects are initialized                */
	const int  MaxObjBodySize = pPage->nd_obj_frame_len() - sizeof (tbd900_ContObjHeader);
	char       InitString[5]  = "\x00\x00\x00\x00";
	SAPDB_MemCopyNoCheck (pContObjFrame->cofBody_bd900, InitString, MIN_EO00 (4,MaxObjBodySize)); 
};

/*---------------------------------------------------------------------------*/

_INLINE static double
bd93_OccupancyRate (int ObjBodyLen,
					int Alignment)
{
	ROUTINE_DBG_MSP00 ("bd93_OccupancyRate");

	/* this function returns the occupancy rate of an object page  */
	/* occupied with continuation objects of the length ObjBodylen */
	/* The calculation considers the  alignment                    */ 

	const int ObjDistance = (((sizeof(tbd900_ContObjHeader)+ObjBodyLen-1)/Alignment)+1)*Alignment; 
	const int NumObj      = FULLCOVERING_BD00/ObjDistance;	
	return ((100.0*NumObj*ObjBodyLen)/FULLCOVERING_BD00);
}

/*---------------------------------------------------------------------------*/

_INLINE static void
bd93_ReleaseContObjFrame (cbd05_Current           &Current,
                          const Converter_Version &converterVersion,
                          FileDir_OmsVar          &fd_cont_smart,
						  const tgg92_KernelOid   &OId,            
						  cbd910_ContObjDataNode  &NptrsContObjPage,
						  const tgg00_ObjPagePos   ContObjPos)
{
	ROUTINE_DBG_MSP00 ("bd93_ReleaseContObjFrame");
	
	/* release an  continuation object, i.e. change its    */
	/* state to free and introduce the frame into the free */
	/* frame chain. Moreover the page pointer is released  */

    Msg_List               errorList;
	tbd_nodeptr           &pContObjPage = NptrsContObjPage.np_ptr(); 
    tgg00_BasisError      &TrError      = Current.curr_trans->trError_gg00;
    tgg00_TransContext    &Trans        = *Current.curr_trans;
	const FileDir_FileNo   ObjFileNo    = Current.curr_tree_id.fileDirFileId_gg00();
    FileDir_IDirectory    &IFDC         = FileDir_IDirectory::SessionInstance(Trans);

    /* get pointer to subroot page */	
	cbd910_SubRootNode   NptrsSubRoot (Current, nr_for_update);  

	if ( NptrsContObjPage.bd910LockSubRootBeforeReleasingFrame() ) 
    {
        NptrsSubRoot.bd910AssignToSubroot (NptrsContObjPage);
		if (e_ok != TrError) 
		{
            /* check if the error is due to a drop of the file */
            tgg00_TransContext &Trans        = *Current.curr_trans;
            FileDir_Oms  fd_smart;
            FileDir_ReturnCode  fd_rc = IFDC.GetFile(
                Trans.trTaskId_gg00, ObjFileNo, fd_smart, errorList);
            if (FileDir_Okay != fd_rc)
            {
                bd900MapFileDirError(Trans, fd_rc, e_ok, (char*)__FILE__,  __LINE__ );
                return;
            }
        }

        /* check if chaining will still be needed ( meahnwhile the page may have changed a lot */
		if ( !NptrsContObjPage.bd910LockSubRootBeforeReleasingFrame() ) 
        {	
            /* no chaining will be needed anymore ==> release subroot */
            NptrsSubRoot.bd910ReleasePage();
        }
    }


	/* get pointer to the continuation object */	
	tbd900_ContObjHeader & ContObjHeader  = *(tbd900_ContObjHeader *) ((char *) pContObjPage+ContObjPos);

	/* check if the found object frame belongs to the root OId */
	if (g01vtrace.vtrCheck_gg00)
		bd93_CheckContObjRoot (OId, ContObjHeader); 

	/* insert object frame into chain of free frames	*/	
	ContObjHeader.cvoNextFreeObj_bd900 = pContObjPage->nd_first_free_obj();
	pContObjPage->nd_first_free_obj()  = ContObjPos;

	/* reset frame header */
	ContObjHeader.cvoRootOIdPno_bd900 = NIL_PAGE_NO_GG00;
    ContObjHeader.cvoRootOIdPos_bd900 = NIL_OBJ_PAGE_POS_GG92;

	/* update page header */  
	++pContObjPage->nd_free_obj_cnt();
    --pContObjPage->nd_occ_obj_cnt();

	const tsp00_Int4 ContObjFileNo  = Current.curr_tree_id.fileContObjFileNo_gg00();
    FileDir_OmsVar         fd_smart;
    FileDir_ReturnCode     fd_rc;
    const SAPDB_Bool       bConsistent = true;
    fd_rc = IFDC.GetFile(Trans.trTaskId_gg00, ObjFileNo, fd_smart, errorList);
    if (FileDir_Okay != fd_rc) {
        bd900MapFileDirError(Trans, fd_rc, e_wrong_class_id, (char*)__FILE__,  __LINE__ );
        return;
    }
    
    /*--------------------------------------------------------------------*/
    /* if the page becomes an empty page by removing the object it is     */ 
    /* released                                                           */
    /*--------------------------------------------------------------------*/
    if ( NptrsContObjPage.bd910IsEmptyPage())
    {
        /* bd910RemovePageFromChainOfAllPages will release subroot PTS 1135422 */
        NptrsSubRoot.bd910RemovePageFromChainOfAllPages(NptrsContObjPage, 
            Current.curr_trans->trTaskId_gg00, converterVersion, fd_smart);         
    }
    else 
    {
        /* update free chain and free page counter */ 
        if (NptrsContObjPage.bd910PageMustBeInsertedIntoFreeChain())
        {	
            NptrsSubRoot.bd910InsertPageIntoFreeChain (NptrsContObjPage, converterVersion, fd_smart);    
        }    
        
        /* release pointer to continuation page */
        NptrsContObjPage.bd910ReleasePage();
    }
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
