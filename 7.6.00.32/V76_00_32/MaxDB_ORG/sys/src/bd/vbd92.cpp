/****************************************************************************

  module      : vbd92.cpp

  -------------------------------------------------------------------------

  responsible : FerdiF

  special area: BD_AuxilleryFunctionsForObjectHandling
  description : description ...


  version     : 7.4.0.0
  last changed: 2002-02-15  8:27
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


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
 
#ifndef HBD92_H
#   include "hbd92.h"
#endif
#ifndef GSP03_H
#   include "gsp03.h"
#endif
#ifndef GSP03_3_H
#   include "gsp03_3.h"
#endif
#ifndef GGG12_H
#   include "ggg12.h"
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
#ifndef GBD930_H
#   include "gbd930.h"
#endif
#ifndef HBD06_H
#   include "hbd06.h"
#endif
#ifndef HBD20_9_H
#   include "hbd20_9.h"
#endif
#ifndef HBD22_H
#   include "hbd22.h"
#endif
#ifndef HBD90_H
#   include "hbd90.h"
#endif
#ifndef HGG01_H
#   include "hgg01.h"
#endif
#ifndef HGG01_1_H
#   include "hgg01_1.h"
#endif
#ifndef HGG04_H
#   include "hgg04.h"
#endif
#ifndef HKB50_H
#   include "hkb50.h"
#endif

#ifndef CONVERTER_IVERIFY_HPP
#   include "Converter/Converter_IVerify.hpp"
#endif
#ifndef LOG_HISTORY_HPP
#   include "Logging/Log_History.hpp"
#endif
#ifndef TRACE_ENTRY_HPP
#   include "Trace/Trace_Entry.hpp"
#endif
#ifndef KERNEL_OPMSG_HPP
#   include "KernelCommon/Kernel_OpMsg.hpp"  
#endif
#ifndef KERNEL_IADMININFO_HPP
#   include "KernelCommon/Kernel_IAdminInfo.hpp"
#endif
#ifndef SAPDB_MEMCOPYMOVE_HPP
#   include "SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp"
#endif
#ifndef MSG_LIST_HPP
#   include "Messages/Msg_List.hpp"
#endif
#ifndef RTETASK_ITASK_HPP
#   include "RunTime/Tasking/RTETask_ITask.hpp"
#endif

#if COMPILEMODE_MEO00 >= SLOW_MEO00 
#include "hta99.h"
#include "hta01.h"
#endif

#ifndef HEO51_H
#   include "heo51.h"           // vgetpid 
#endif

/*===========================================================================*
*  DEFINES                                                                  *
*===========================================================================*/

/*===========================================================================*
*  MACROS                                                                   *
*===========================================================================*/

/*===========================================================================*
*  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
*===========================================================================*/

struct  tbd92_ObjPageChainStat
{
    unsigned int    FreePageCnt;           
    int             PageWithFreeLabelCnt;          
    unsigned int  FreePageInFreeChainCnt;           
    unsigned int  PageInFreeChainCnt;                    
    int    PageWithFreeLabelInFreeChainCnt;       
    int    FreePageWithFreeLabelInFreeChainCnt;    
    
    int    FeotPageCnt;
    
    int    Filler0;
    int    Filler1;       // PageWithEmptyLabelCnt
    int    Filler2;       // PageWithEmptyLabelInFreeChainCnt
    int    Filler3;
    int    Filler4;       // EmptyPageWithEmptyLabelInFreeChainCnt
    
    unsigned int AllPageCnt; 
    
    int    FreeObjCnt;
    int    FeotObjCnt;
    int    OccObjCnt;
    int    ResObjCnt;
    
    inline void ResetAllCountersToZero ()   
    {
        FreePageCnt                          = 0;
        PageWithFreeLabelCnt                 = 0;
        FreePageInFreeChainCnt               = 0;
        PageInFreeChainCnt                   = 0;
        PageWithFreeLabelInFreeChainCnt      = 0;
        FreePageWithFreeLabelInFreeChainCnt  = 0;      
        FeotPageCnt                          = 0;
        Filler0                              = 0;
        Filler1                              = 0;
        Filler2                              = 0;
        Filler3                              = 0;
        Filler4                              = 0;
        AllPageCnt                           = 0; 
        FreeObjCnt                           = 0;
        FeotObjCnt                           = 0;
        OccObjCnt                            = 0;
        ResObjCnt                            = 0;
    };
};

/*---------------------------------------------------------------------------*/

struct tbd92_ObjPageContainerStat
{
    int FreeObjCnt;
    int FeotObjCnt;
    int OccObjCnt;  
    int ResObjCnt;
    int AllPageCnt;
    int FreePageCnt;
    int FeotPageCnt;
    int Filler0;
    
    tbd92_ObjPageContainerStat ()   
        :FreeObjCnt(0),
        FeotObjCnt(0),
        OccObjCnt(0),
        ResObjCnt (0),
        AllPageCnt(0),
        FreePageCnt(0),
        FeotPageCnt(0),
        Filler0(0)
    {};
};

/*===========================================================================*
*  EXTERNAL VARIABLES                                                       *
*===========================================================================*/

extern cbd930_GarbCollController  bd91GarbCollController;

/*===========================================================================*
*  GLOBAL VARIABLES                                                         *
*===========================================================================*/

/*===========================================================================*
*  LOCAL VARIABLES                                                          *
*===========================================================================*/

/*===========================================================================*
*  LOCAL FUNCTIONS (PROTOTYPES)                                             *
*===========================================================================*/


static void
bd92_CheckBeforeImage (tgg00_TransContext   &Trans,      
                       tgg92_KernelOid       WantedOid,
                       tgg00_ObjTransInfo   &ObjTransInfo);  

/*---------------------------------------------------------------------------*/

static void
bd92_CheckHashEntry (cbd05_Current       &Current, 
                     tbd900_ObjFrame         *pObjFrame, 
                     tgg92_KernelOid        &CurrentOId,
                     int                    ObjFrameLen,
                     int                    ObjKeyLen); 

/*---------------------------------------------------------------------------*/

static void
bd92_CheckAndUpdObjChainStatistics (cbd05_Current                &Current,
                                    FileDir_FileNo               &ObjFileNo,
                                    const tbd92_ObjPageChainStat &ChainStat,
                                    FileDir_Oms                  &fd_smart);

/*---------------------------------------------------------------------------*/

static void
bd92_VerifyObjPageChain (cbd05_Current           &Current,
                         boolean                  bConverterUpdate,
                         tsp00_PageNo             SubRoot,
                         tbd92_ObjPageChainStat  &ChainStat,
                         FileDir_Oms             &fd_smart,
                         const tsp00_Bool         bChainIsDeleted);

/*---------------------------------------------------------------------------*/

static void
bd92_VerifyObjRootPage (cbd05_Current  &Current,
                        tbd_node_ptrs     &NptrsRoot);

/*---------------------------------------------------------------------------*/

static void
bd92_VerifyObjSubRootPage (cbd05_Current  &Current,
                           tbd_node_ptrs     &NptrsSubRoot);

/*---------------------------------------------------------------------------*/

static void
bd92_VerifyContObjDataPage (cbd05_Current &Current,
                            tbd_nodeptr       pPage);

/*---------------------------------------------------------------------------*/

static void
bd92_VerifyObjDataPage (cbd05_Current &Current,
                        tbd_nodeptr       pPage,
                        tsp00_Int4       &ResObjCnt,
                        const tsp00_Bool bChainIsDeleted);

/*---------------------------------------------------------------------------*/

static void
bd92_VerifyObjPage (cbd05_Current  &Current,
                    tbd_node_ptrs     &NptrsPage,
                    tsp00_Int4        &ResObjCnt,
                    const tsp00_Bool   bChainIsDeleted);

/*---------------------------------------------------------------------------*/

static void
bd92_VerifyObjPageHeader (cbd05_Current  &Current,
                          tbd_nodeptr        pPage);

/*---------------------------------------------------------------------------*/

static void
bd92_WriteContainerStateToVtrace (cbd05_Current                     &Current, 
                                  const  tbd92_ObjPageContainerStat &ContainerStat,
                                  FileDir_Oms                       &fd_smart);

/*---------------------------------------------------------------------------*/

static void
bd92_WriteChainStateToVtrace (cbd05_Current                &Current, 
                              const  tbd92_ObjPageChainStat   &ChainStat,
                              cbd910_SubRootNode              &NptrsSubRoot,
                              FileDir_Oms                     &fd_smart,
                              const tsp00_Bool                 bChainIsDeleted);

/*---------------------------------------------------------------------------*/

static void
bd92_WritePageStateToVtrace (tgg00_TransContext    &Trans,
                             tbd_nodeptr            pPage,
                             tsp00_Int4             ResObjCnt);

/*===========================================================================*
 *  GLOBAL FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

externCpp bool 
bd92ObjPosIsValid (tgg00_ObjPagePos  ObjPos,
                   tgg00_ObjPagePos  FirstObjPos,
                   tsp00_Uint2       ObjFrameLen)
{
    ROUTINE_DBG_MEO00 ("bd92ObjPosIsValid");
    
    /* This function returns true if the ObjPos points    */
    /* onto a valid position of an object otherwise false */
    /* is returned.                                       */
    
    bool      bValidPos;
    const int AlignedObjFrameLen = BD92ALIGN_OBJ_FRAME (ObjFrameLen);
    
    bValidPos = (
        (FirstObjPos >= NODE_HEADER_MXBD00                     ) &&  /* PTS 1107145 AK 10/07/2000 */
        (ObjPos >= FirstObjPos                                 ) &&
        ((ObjPos + ObjFrameLen) <= BODY_END_BD00               ) &&
        (((ObjPos - FirstObjPos) % AlignedObjFrameLen)     == 0) &&
        ((FirstObjPos - BD92ALIGN_OBJ_FRAME (FirstObjPos)) == 0) 
        );
    
    if (! bValidPos)
        bd92WriteWrongPosToVtrace ("bd92ObjPosIsValid", ObjPos, FirstObjPos,  ObjFrameLen);
    
    return (bValidPos);
}

/*---------------------------------------------------------------------------*/

externC void
bd92VerifyObjFile (tbd_current_tree       &curr,
                   boolean                 bConverterUpdate)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd92VerifyObjFile", ObjContainer_Trace, 5);
    
    /* This function checks all object page chains which */
    /* belong to the current object file.      if any    */
    /* error is detected an op message is sent           */
    /* Moreover, if bConverterUpdate = true all pages    */
    /* of the container are flaged in the FBM as used    */
    
    cbd05_Current              Current(curr);     

    tbd92_ObjPageContainerStat ContainerStat;
    tbd92_ObjPageChainStat     ChainStat;
    FileDir_FileNo             FileNo       = Current.curr_tree_id.fileDirFileId_gg00();
    const tsp00_Int4           ContFileNo   = Current.curr_tree_id.fileContObjFileNo_gg00();
    Msg_List                   errorList;

    b06file_opmsg (csp3_check_file, Current.curr_tree_id);
    
    /* check if the file to be checked is really an object file */
    if (Current.curr_tree_id.fileTfn_gg00() != tfnObj_egg00)
    {
        Kernel_OpError( csp3_bd_msg, csp3_n_obj )
            << "BD92VerObjFile - file with root:" << Current.curr_tree_id.fileRoot_gg00()
            << " is not an obj file! File type:"
            << Current.curr_tree_id.fileTfn_gg00();
        Kernel_VTrace()
            << "BD92VerObjFile - file with root:" << Current.curr_tree_id.fileRoot_gg00()
            << " is not an obj file! File type:"
            << Current.curr_tree_id.fileTfn_gg00();
        Current.curr_trans->trError_gg00 = e_invalid_filetype;
        return;
    }
    
    /* get shortcut to the object file info */
    FileDir_ReturnCode     fd_rc;
    FileDir_Oms            fd_smart;
    tgg00_TransContext    &Trans    = *Current.curr_trans;
    vgetpid( Trans.trTaskId_gg00 );
    fd_rc = FileDir_IDirectory::Instance(cgg_nil_transindex).GcGetFile( Trans.trTaskId_gg00, FileNo, fd_smart, errorList);
    if (FileDir_Okay != fd_rc) {
        bd900MapFileDirError(Trans, fd_rc, e_file_not_found, (char*)__FILE__,  __LINE__ );
        Kernel_OpError ( csp3_bd_msg, csp3_n_obj )
            << __FILE__ << ":" << __LINE__ 
            << " BD92VerObjFile - FileId:" << FileNo
            << " file not found FDirRc:" << fd_rc;
        Kernel_VTrace()
            << __FILE__ << ":" << __LINE__ 
            << " BD92VerObjFile - FileId:" << FileNo
            << " file not found FDirRc:" << fd_rc;
        return;
    }
    Data_PageNo root = fd_smart.GetRootPage();

    /* check if the  root is correctly stored in the ObjFileInfo */
    if (root != Current.curr_tree_id.fileRoot_gg00())
    {
        Kernel_OpError  opErrMsg( csp3_bd_msg, csp3_n_obj );
        opErrMsg << __FILE__ << ":" << __LINE__ 
            << " BD92VerObjFile - fileno:" << FileNo
            << " ObjFileInfo.root:" << root
            << " NEQ Current.root:" << Current.curr_tree_id.fileRoot_gg00();
        Current.curr_trans->trError_gg00 = e_invalid_root;
        return;
    }    

    /* check each container chain of the file */
    const tsp00_Bool bFileIsDeleted = (Data_FileDeleted == fd_smart.GetFileState());

    if (NIL_PAGE_NO_GG00 == root)
    {
        if (!bFileIsDeleted)
        {
            Kernel_OpError  opErrMsg( csp3_bd_msg, csp3_n_obj );
            opErrMsg << "BD92VerObjFile - fileno:" << FileNo;
            if (PRIM_CONT_OBJ_FILE_NO_BD900 != ContFileNo)
                opErrMsg << " cont fileno:" << ContFileNo;
            opErrMsg << " root is NIL";
            Current.curr_trans->trError_gg00 = e_data_page_corrupted; // PTS 1121056
            return;
        } 
    }
    else 
    { 
        bd92_VerifyObjPageChain (Current, bConverterUpdate, root, ChainStat, fd_smart, bFileIsDeleted);
        if ( e_ok != Current.curr_trans->trError_gg00 )
            return;

        /* count objects */
        ContainerStat.FreeObjCnt    += ChainStat.FreeObjCnt;
        ContainerStat.FeotObjCnt    += ChainStat.FeotObjCnt;
        ContainerStat.OccObjCnt     += ChainStat.OccObjCnt;
        ContainerStat.ResObjCnt     += ChainStat.ResObjCnt;

        /* count pages */
        ContainerStat.AllPageCnt    += ChainStat.AllPageCnt;
        ContainerStat.FreePageCnt   += ChainStat.FreePageCnt;
        ContainerStat.FeotPageCnt   += ChainStat.FeotPageCnt;

        /* verify continuation files */
        if ((oftVarLenObjFile_egg00 == Current.curr_tree_id.fileObjFileType_gg00()) &&
            (PRIM_CONT_OBJ_FILE_NO_BD900 == ContFileNo))                             
        {
            FileDir_OmsVar  fd_var_smart;
            fd_var_smart.Assign(fd_smart);
            for (tsp00_Int4 iContFileNo=1; iContFileNo <= MAX_NUM_CONT_OBJ_FILES_BD900; ++iContFileNo)
            {
                cbd05_Current ContCurrent (*Current.curr_trans, m_select);
                Data_PageNo ContObjFileRoot = fd_var_smart.GetContPartition(iContFileNo);
                fd_var_smart.BuildContFileId(iContFileNo, ContCurrent.curr_tree_id);
                if (NIL_PAGE_NO_GG00 != ContObjFileRoot)
                {
                    bd92_VerifyObjPageChain (ContCurrent, bConverterUpdate, ContObjFileRoot, ChainStat, fd_smart, bFileIsDeleted);
                    if ( e_ok != Current.curr_trans->trError_gg00 )
                        return;

                    /* count pages */
                    ContainerStat.AllPageCnt    += ChainStat.AllPageCnt;
                }
            }

            ChainStat.FreeObjCnt  = ContainerStat.FreeObjCnt;
            ChainStat.FeotObjCnt  = ContainerStat.FeotObjCnt;
            ChainStat.OccObjCnt   = ContainerStat.OccObjCnt;
            ChainStat.ResObjCnt   = ContainerStat.ResObjCnt;

            ChainStat.AllPageCnt  = ContainerStat.AllPageCnt;
            ChainStat.FreePageCnt = ContainerStat.FreePageCnt;
            ChainStat.FeotPageCnt = ContainerStat.FeotPageCnt;
        }


        /* check the number of pages and objects stored in ObjFileInfo */
        bd92_CheckAndUpdObjChainStatistics (Current, FileNo, ChainStat, 
            fd_smart);
    }
    
    /* write a vtrace entry */
    bd92_WriteContainerStateToVtrace (Current, ContainerStat, fd_smart);
    
}

/*---------------------------------------------------------------------------*/

externCpp void
bd92VerifyObjPage (cbd05_Current  &Current,
                   tbd_node_ptrs  &NptrsPage)
{
    ROUTINE_DBG_MEO00 ("bd92VerifyObjPage");
    
    /* This routine checks the consistency */
    /* of the contents of an object page   */
    
    /* declare dummy return */
    
    tsp00_Int4 DummyResObjCnt = 0;
    
    /* true signals that no before-image-check is needed ) */
    bd92_VerifyObjPage (Current, NptrsPage, DummyResObjCnt, true );
}

/*---------------------------------------------------------------------------*/

externCpp void
bd92VtraceObjKey (tgg00_TransContext    &Trans, 
                  tsp00_C8              &Fileno,
                  tsp00_Int4             KeyLen,
                  void                  *pKey,
                  tgg00_VtraceType_Enum  TraceObject)
{
    /*  This function writes the fileno and the key of a   */
    /*  BD-function at the KB-BD interface into the trace  */
    
    const int         MaxVtraceKey = sizeof (tsp00_Line) - sizeof (tgg12_OmsTraceKeyHeader);   
    tgg12_OmsKeyTrace TrLineOut;
    int               OutKeyLen;
    
    TrLineOut.okyHead_gg12.okhTrType_gg12.becomes  ( otKey_egg12);
    TrLineOut.okyHead_gg12.okhTrType2_gg12.becomes ( ot2Nil_egg12);
    TrLineOut.okyHead_gg12.okh.ObjFileNo_C8_gg12() = Fileno;
    TrLineOut.okyHead_gg12.okhConsView_gg12        = Trans.trConsistView_gg00;
    
    TrLineOut.okyHead_gg12.okhKeyLen_gg12 = 
        ((KeyLen >= 0) && (KeyLen <= MAX_UINT1_SP00)) ? KeyLen : MAX_UINT1_SP00;
    
    OutKeyLen = (TrLineOut.okyHead_gg12.okhKeyLen_gg12 > MaxVtraceKey)
        ? MaxVtraceKey : TrLineOut.okyHead_gg12.okhKeyLen_gg12;
    
    if (OutKeyLen > 0)
    {
        SAPDB_MemCopyNoCheck (&TrLineOut.okyKey_gg12, pKey, OutKeyLen);
    }
    Trace_CommonEntry entry( tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(TraceObject), 
        REINTERPRET_CAST( tsp00_BytePtr, &TrLineOut ), 
        (tsp00_Int2)(sizeof (TrLineOut.okyHead_gg12) + OutKeyLen) );
}

/*---------------------------------------------------------------------------*/

externCpp void
bd92WriteWrongPosToConsole (const char * funcName,
                            tgg00_ObjPagePos  ObjPos,
                            tgg00_ObjPagePos  FirstObjPos,
                            tsp00_Uint2       ObjFrameLen)
{
    Kernel_OpError( csp3_bd_msg, csp3_n_obj )
        << funcName << " - invalid oid.pos "
        << " oid.pos:" << ObjPos 
        << " First Pos:" << FirstObjPos 
        << " FrameLen:" << ObjFrameLen 
        << " AlignedFrameLen:" << BD92ALIGN_OBJ_FRAME (ObjFrameLen);
    bd92WriteWrongPosToVtrace(funcName, ObjPos, FirstObjPos, ObjFrameLen);
}

/*---------------------------------------------------------------------------*/

externCpp void
bd92WriteWrongPosToVtrace (const char * funcName,
                            tgg00_ObjPagePos  ObjPos,
                            tgg00_ObjPagePos  FirstObjPos,
                            tsp00_Uint2       ObjFrameLen)
{
    if (g01vtrace.vtrBdObject_gg00)
    {
        Kernel_VTrace()
            << funcName << " - invalid oid.pos "
            << " oid.pos:" << ObjPos 
            << " First Pos:" << FirstObjPos 
            << " FrameLen:" << ObjFrameLen 
            << " AlignedFrameLen:" << BD92ALIGN_OBJ_FRAME (ObjFrameLen);
    }
}

/*----------------------------------------------------------------------------*/

externCpp void
bd92WriteWrongOidToConsole (const char *             funcName, 
                            const tgg92_KernelOid   &OId)
{
    Kernel_OpError ( csp3_bd_msg, csp3_n_obj )
        << funcName << " - invalid oid "
        << OId.gg92GetPno() << "." << OId.gg92GetPos() 
        << " (vers " << OId.gg92GetFrameVers() << ")";
    bd92WriteWrongOidToVtrace(funcName, OId);
};

/*----------------------------------------------------------------------------*/

externCpp void
bd92WriteWrongOidToVtrace (const char *             funcName, 
                            const tgg92_KernelOid   &OId)
{
    if (g01vtrace.vtrBdObject_gg00)
    {
        Kernel_VTrace() << funcName << " - invalid oid "
            << OId.gg92GetPno() << "." << OId.gg92GetPos() 
            << " (vers " << OId.gg92GetFrameVers() << ")";
    }
};

/*===========================================================================*
*  LOCAL FUNCTIONS (CODE)                                                   *
*===========================================================================*/

static void
bd92_CheckBeforeImage (tgg00_TransContext   &Trans,      
                       tgg92_KernelOid       WantedOid,
                       tgg00_ObjTransInfo   &ObjTransInfo)               
{
    /* check if there exists an before image. for */
    /* a feot object there must always exist one  */
    tsp00_Bool bIsActive;

    tgg00_BasisError    saveError = Trans.trError_gg00; // PTS 1121056
    Trans.trError_gg00            = e_ok;               // PTS 1121056   

    /* PTS 1127493 FF 2004-FEB-02       */
    /* check if lock collision occurred */
    if (ObjTransInfo.otrLockTransId_gg00.gg90IsDummy())
        bIsActive = true;
    else
        kb50CheckActiveTrans (Trans.trTaskId_gg00, 
        ObjTransInfo.otrLockTransId_gg00,
        ObjTransInfo.otrLockTransIndex_gg00,
        bIsActive);

    if (! bIsActive)
    {
        if ( ! Log_History::GetInstance().BeforeImageExists ( Trans,
            WantedOid,
            ObjTransInfo.otrBeforeRef_gg00 )) 
        {
            Kernel_OpError  ( csp3_bd_msg, csp3_n_obj )
                << __FILE__ << ":" << __LINE__ 
                << " BD92_CheckBeforeImage - before image not found OID "
                << WantedOid.gg92GetPno() << "." << WantedOid.gg92GetPos() 
                << " (vers " << WantedOid.gg92GetFrameVers() << ")"
                /* PTS 1127493 FF 2004-FEB-02       */
                << " BeforeRefPno:" << ObjTransInfo.otrBeforeRef_gg00.gg91RefPno()
                << " BeforeRefPos:" << ObjTransInfo.otrBeforeRef_gg00.gg91RefPos();
            if (e_ok == Trans.trError_gg00)
                Trans.trError_gg00 = e_obj_history_not_found;
        }
    }

    if ( e_ok != saveError ) Trans.trError_gg00 = saveError;   
}

/*---------------------------------------------------------------------------*/

static void
bd92_CheckHashEntry (cbd05_Current          &Current, 
                     tbd900_ObjFrame        *pObjFrame, 
                     tgg92_KernelOid        &CurrentOId,
                     int                    ObjFrameLen,
                     int                    ObjKeyLen)                      
{
    ROUTINE_DBG_MEO00 ("bd92_CheckHashEntry");
    
    /* This function checks if the OId found from the hash        */
    /* by using the key of the current object points at           */ 
    /* the current object                                         */
    /* Note, for an object of the feot state one cannot check     */
    /* the hash since there can exist an object with the same key */
    /* which was reused. In this case the current object was      */
    /* deleted and is of the state free after eot now.            */
    /* The Version ist not checked because it is not any more     */
    /* correct after an object is reused                          */
    
    tgg00_BasisError    &TrError  = Current.curr_trans->trError_gg00;
    tgg00_BasisError    saveError = TrError;            // PTS 1121056
    tgg92_KernelOid     HashOId;
    Msg_List            ErrorList;

    TrError = e_ok;                                     // PTS 1121056
    
    void * pObjKey = (void *) ((tsp00_Byte *) pObjFrame  + ObjFrameLen - ObjKeyLen);
    
    if  (pObjFrame->objHeader_bd900.ohdState_gg92 == obsOccupied_egg00)
    {
        FileDir_FileNo  objFileNo = Current.curr_tree_id.fileDirFileId_gg00();
        bd22GetOIdForVerify (*Current.curr_trans, objFileNo, ObjKeyLen, pObjKey, HashOId, ErrorList);        
        if (TrError != e_ok) {
            Kernel_OpError  opErrMsg( csp3_bd_msg, csp3_n_obj );
            opErrMsg << " bd92_CheckHashEntry - Key not found in index! OID "
                << CurrentOId.gg92GetPno() << "." << CurrentOId.gg92GetPos() 
                << " (vers " << CurrentOId.gg92GetFrameVers() << ")"
                << " Object state:" << pObjFrame->objHeader_bd900.ohdState_gg92;
            if (saveError != e_ok) TrError = saveError; 
            return;
        }

        if (! HashOId.gg92IsEqIgnoreFrameVers(CurrentOId) )
        {
            /*-----------------------------------*/
            /* an error is detected in the hash  */
            /*-----------------------------------*/
            
            /* write a message into knldiag */
            {
                Kernel_OpError  opErrMsg( csp3_bd_msg, csp3_n_obj );
                opErrMsg << " bd92_CheckHashEntry - wrong OId in KeyIndex: OID "
                    << HashOId.gg92GetPno() << "." << HashOId.gg92GetPos() 
                    << " (vers " << HashOId.gg92GetFrameVers() << ")"
                    << " expected OID "
                    << CurrentOId.gg92GetPno() << "." << CurrentOId.gg92GetPos() 
                    << " (vers " << CurrentOId.gg92GetFrameVers() << ")";
                if (e_ok == TrError)
                    TrError = e_invalid_index_structure;
            }
            
            /* output of input OId and input key into vtrace */
            {
                tgg12_OmsOidTrace TrLineOId;
                
                TrLineOId.ooidTrType_gg12.becomes(otOid_egg12);
                TrLineOId.ooidTrType2_gg12.becomes(ot2Input_egg12);
                TrLineOId.ooidObjId_gg12   = CurrentOId;
                
                Trace_CommonEntry entry( tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(b92check_hash), 
                    REINTERPRET_CAST( tsp00_BytePtr, &TrLineOId ), sizeof (TrLineOId) );
            }
            bd92VtraceObjKey (*Current.curr_trans,
                Current.curr_tree_id.fileDirFileId_gg00(), 
                ObjKeyLen, pObjKey, b92check_hash);
            
            /* output of output OId or error reason into vtrace */
            
            if (Current.curr_trans->trError_gg00 == e_ok) 
            {
                tgg12_OmsOidTrace TrLineOId;

                TrLineOId.ooidTrType_gg12.becomes(otOid_egg12);
                TrLineOId.ooidTrType2_gg12.becomes(ot2Output_egg12);
                TrLineOId.ooidObjId_gg12   = HashOId;
               
                Trace_CommonEntry entry( tgg00_Debug::fromConst(bd), 
                    tgg00_VtraceType::fromConst(b92check_hash), 
                    REINTERPRET_CAST( tsp00_BytePtr, &TrLineOId ), sizeof (TrLineOId) );
            }
            else
            {
                tgg12_OmsErrorTrace TrLineOut;
                
                TrLineOut.oerrTrType_gg12.becomes  ( otError_egg12);
                TrLineOut.oerrTrType2_gg12.becomes ( ot2Nil_egg12);
                TrLineOut.oerrRoot_gg12            = Current.curr_tree_id.fileRoot_gg00();
                TrLineOut.oerr.ObjFileNo_C8_gg12() = Current.curr_tree_id.fileDirFileId_gg00();   
                TrLineOut.oerrError_gg12           = Current.curr_trans->trError_gg00;
                
                Trace_CommonEntry entry( tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(b92check_hash), 
                    REINTERPRET_CAST( tsp00_BytePtr, &TrLineOut ), sizeof (TrLineOut) );
            }
        }
    }
    if (saveError != e_ok) TrError = saveError; // PTS 1121056
}

/*---------------------------------------------------------------------------*/

static  void
bd92_CheckAndUpdObjChainStatistics (cbd05_Current                &Current,
                                    FileDir_FileNo               &ObjFileNo,
                                    const tbd92_ObjPageChainStat &ChainStat,
                                    FileDir_Oms                  &fd_smart)
{
    ROUTINE_DBG_MEO00 ("bd92_CheckAndUpdObjChainStatistics");

    tgg00_TransContext      &Trans  = *Current.curr_trans;

    if ((ChainStat.AllPageCnt != fd_smart.GetLeafPageCount(Trans.trTaskId_gg00))
        &&
        (Data_FileDeleted != fd_smart.GetFileState()))
    {

        if ( Kernel_IAdminInfo::Instance().KernelStateIsOnline()) 
        {
            Kernel_OpInfo(  csp3_n_obj )
                << " statistic page counter for container file:" << ObjFileNo
                << " is:" << fd_smart.GetLeafPageCount(Trans.trTaskId_gg00)
                << " instead of:" << ChainStat.AllPageCnt;
        }
        else
        {
            Kernel_OpInfo(  csp3_n_obj )
                << " statistic page counter for container file:" << ObjFileNo
                << " updated from:" << fd_smart.GetLeafPageCount(Trans.trTaskId_gg00)
                << " to:" << ChainStat.AllPageCnt;

            cbd910_SubRootNode   NptrsSubRoot (Current, fd_smart.GetRootPage(), nr_for_read);
            if (Current.curr_trans->trError_gg00 == e_ok) 
            {
                Converter_Version converterVersion = 
                    bd20GetDestinationConverterVersion ( Trans.trTaskId_gg00, NptrsSubRoot.np_cbptr() );
                SAPDB_Int4 diff = ChainStat.AllPageCnt
                    - fd_smart.GetLeafPageCount(Current.curr_trans->trTaskId_gg00 );
                fd_smart.ModifyLeafPageCount(Trans.trTaskId_gg00, converterVersion, diff);
            }
        }
    }

    if (Data_OmsVarFile == fd_smart.GetFileType())
        return;

    if ((ChainStat.FreePageCnt != fd_smart.GetFreePageCount(Trans.trTaskId_gg00))
        &&
        (Data_FileDeleted != fd_smart.GetFileState()))
    {
        if ( !Kernel_IAdminInfo::Instance().KernelStateIsOnline()) 
        {
            Kernel_OpInfo( csp3_n_obj )
                << " statistic free page counter for container file:" << ObjFileNo
                << " updated from:" << fd_smart.GetFreePageCount(Trans.trTaskId_gg00)
                << " to:" << ChainStat.FreePageCnt;

            cbd910_SubRootNode   NptrsSubRoot (Current, fd_smart.GetRootPage(), nr_for_read);
            if (Current.curr_trans->trError_gg00 == e_ok) 
            {
                Converter_Version converterVersion = 
                    bd20GetDestinationConverterVersion ( Trans.trTaskId_gg00, NptrsSubRoot.np_cbptr() );
                SAPDB_Int4 diff = ChainStat.FreePageCnt
                    - fd_smart.GetFreePageCount(Current.curr_trans->trTaskId_gg00 );
                fd_smart.ModifyFreePageCount(Trans.trTaskId_gg00, converterVersion, diff);
            }
        }
    }

    if ((( ChainStat.FeotObjCnt + ChainStat.OccObjCnt + ChainStat.ResObjCnt )
        != fd_smart.GetEntryCount(Trans.trTaskId_gg00))
        &&
        (Data_FileDeleted != fd_smart.GetFileState()))
    {
        if ( ! Kernel_IAdminInfo::Instance().KernelStateIsOnline()) 
        {
            Kernel_OpInfo( csp3_n_obj )
                << " statistic entry counter for container file:" << ObjFileNo
                << " corrected from:" << fd_smart.GetEntryCount(Trans.trTaskId_gg00)
                << " to:" << ( ChainStat.FeotObjCnt + ChainStat.OccObjCnt + ChainStat.ResObjCnt);

            cbd910_SubRootNode   NptrsSubRoot (Current, fd_smart.GetRootPage(), nr_for_read);
            if (Current.curr_trans->trError_gg00 == e_ok) 
            {
                Converter_Version converterVersion = 
                    bd20GetDestinationConverterVersion ( Trans.trTaskId_gg00, NptrsSubRoot.np_cbptr() );
                SAPDB_Int8 diff = ( ChainStat.FeotObjCnt + ChainStat.OccObjCnt + ChainStat.ResObjCnt )
                    - fd_smart.GetEntryCount(Current.curr_trans->trTaskId_gg00 );
                fd_smart.ModifyEntryCount(Trans.trTaskId_gg00, converterVersion, diff);
            }
        }
    }
}

/*---------------------------------------------------------------------------*/

static void
bd92_VerifyObjPageChain (cbd05_Current           &Current,
                         boolean                  bConverterUpdate,
                         tsp00_PageNo             SubRoot,
                         tbd92_ObjPageChainStat  &ChainStat,
                         FileDir_Oms             &fd_smart,
                         const tsp00_Bool         bChainIsDeleted)
{
    ROUTINE_DBG_MEO00 ("bd92_VerifyObjPageChain");
    
    /* This function checks the counter of  empty, free,          */
    /* free_after_eot and all pages contained in the info         */
    /* block on the root page. The counters of occupied, free and */ 
    /* free_after_eot objects are verified for each page          */
    /* in the chain, too. In case the chain does not belong to a  */
    /* file which is marked as to be deleted it is also checked   */
    /* if the chains of free and free_after eot pages and objects */
    /* are consistent                                             */
    
    tgg00_TransContext      &Trans        = *Current.curr_trans;
    tgg00_BasisError        &TrError      = Current.curr_trans->trError_gg00;
    tgg00_BasisError        saveError     = TrError;             
    teo00_Uint8             PrevPageSeqNo = MAX_LOCK_REQUEST_SEQ_NO_BD900;
    tsp00_Int4              ResObjCnt;
    tsp00_PageNo            NextPageNo;
    tsp00_PageNo            PrevPageNo;
    
    ChainStat.ResetAllCountersToZero();
    
    /*------------------------------------------------------------*/
    /* go through chains of all pages following the right pointer */
    /* and count the number of free, empty and free_after_eot     */
    /* pages. For each page the object counters are verified      */
    /* as well as the free object chain on each page              */
    /*------------------------------------------------------------*/
    
    /* keep subroot to avoid any change of the chain structure    */
    cbd910_SubRootNode   NptrsSubRoot (Current, SubRoot, nr_for_read);
    if (TrError != e_ok) 
    {
        Kernel_OpError( csp3_bd_msg, csp3_n_obj )
            << __FILE__ << ":" << __LINE__ 
            << " Error:" << TrError 
            << " trying read access to subroot:" << SubRoot;
        return;
    }
    
    /* create page handler which runs over all pages of  */
    /* this chain and set it to the subroot (start) page */
    cbd910_ObjDataNode    NptrsPage (Current, nr_for_read);
    NptrsPage.bd910SetTo (NptrsSubRoot);
    
    /* traverse chain of all pages  */
    do
    {
        if (bConverterUpdate)
        {
            const RTETask_ITask &task  = *RTETask_ITask::ByTaskID( Current.curr_trans->trTaskId_gg00 );

            Converter_IVerify::Instance().MarkPageNoAsUsed (
                task, NptrsPage.np_ptr()->nd_id(), 
                (gg04IsPermStaticTfn (Current.curr_tree_id.fileTfn_gg00())? Data_Static:Data_Dynamic));
        }
        
        /* check that the page sequence numbers decreases */
        if ((bd900GetPageSeqNo(NptrsPage.np_ptr()->ndLockRequestSeqNoHigh_bd00(), 
            NptrsPage.np_ptr()->ndLockRequestSeqNoLow_bd00()) >= PrevPageSeqNo) &&
            (NptrsPage.np_ptr()->nd_id() != NptrsPage.np_ptr()->nd_root()))   
        {
            Kernel_OpError  opErrMsg( csp3_bd_msg, csp3_n_obj );
            opErrMsg << "BD92VerObjPageChain - root:" << NptrsPage.np_ptr()->nd_root()
                << " PageSeqNo not decreasing";
            if (e_ok == saveError) saveError = e_data_page_corrupted; 
        }
        
        PrevPageSeqNo = bd900GetPageSeqNo(NptrsPage.np_ptr()->ndLockRequestSeqNoHigh_bd00(),
            NptrsPage.np_ptr()->ndLockRequestSeqNoLow_bd00());
        
        /* check object counters and object chains on current page */
        bd92_VerifyObjPage (Current, NptrsPage, ResObjCnt, bChainIsDeleted);
        if (TrError != e_ok) 
        {
            if (e_ok == saveError) saveError = TrError; 
            TrError = e_ok;
        }

        /* update page and object counter */
        ++ChainStat.AllPageCnt;        
        
        if (NptrsPage.np_ptr()->nd_free_obj_cnt() > 0)      
            ++ChainStat.FreePageCnt;
        if (NptrsPage.np_ptr()->nd_free_eot_obj_cnt() > 0)      
            ++ChainStat.FeotPageCnt;
        
        if (NptrsPage.np_ptr()->nd_in_free_chain())     
            ++ChainStat.PageWithFreeLabelCnt;
        
        ChainStat.FreeObjCnt += NptrsPage.np_ptr()->nd_free_obj_cnt();
        ChainStat.FeotObjCnt += NptrsPage.np_ptr()->nd_free_eot_obj_cnt();
        ChainStat.OccObjCnt  += NptrsPage.np_ptr()->nd_occ_obj_cnt();
        ChainStat.ResObjCnt  += ResObjCnt;
        
        /* goto next page in chain */
        NextPageNo = NptrsPage.np_ptr()->nd_right();
        PrevPageNo = NptrsPage.np_ptr()->nd_id();
        
        if (NIL_PAGE_NO_GG00 == NextPageNo)
        {
            break;
        }
        else
        {
            NptrsPage.bd910MoveToNextPage();
            if (e_ok != TrError) 
            {
                Kernel_OpError( csp3_bd_msg, csp3_n_obj )
                    << __FILE__ << ":" << __LINE__ 
                    << " root:" << SubRoot              
                    << " page:" <<PrevPageNo       
                    << " :" << TrError 
                    << " trying access to next page:"  << NextPageNo;
                if (e_ok != saveError) TrError = saveError; 
                return;
            }
            if (PrevPageNo != NptrsPage.np_ptr()->nd_left())
            {
                Kernel_OpError( csp3_bd_msg, csp3_n_obj )
                    << __FILE__ << ":" << __LINE__ 
                    << " root:" << NptrsPage.np_ptr()->nd_root()
                    << " page:" << NptrsPage.np_ptr()->nd_id()
                    << " back pointer wrong"
                    << " nd_left:" << NptrsPage.np_ptr()->nd_left(); 
                if (e_ok != saveError) 
                    TrError = saveError;
                else
                    TrError = e_data_page_corrupted; 
                return;
            }
        }
    }
    while (ChainStat.AllPageCnt <= fd_smart.GetLeafPageCount(Current.curr_trans->trTaskId_gg00));
    
    /* PTS 1121106 FF 2003-MAR-13 */
    /* bd910MoveToNextPage may have marked NptrsPage with 'm_PrimaryReference = true;' */
    PrevPageNo = NptrsPage.np_ptr()->nd_id();   // PTS 1121106  FF 
    NptrsPage.bd910ReleasePage();   
    if (e_ok != TrError) 
    {
        Kernel_OpError( csp3_bd_msg, csp3_n_obj )
            << __FILE__ << ":" << __LINE__ 
            << " root:"  << SubRoot         
            << " page:" << PrevPageNo   
            << " error:" << TrError 
            << " trying release page";
        if (e_ok == saveError) saveError = TrError;  
        TrError = e_ok;
    }

  /* traverse chain of free pages (= pages with free object frames ) */
    if (! bChainIsDeleted)
    {   
        /* set page handler to fisrt page of free page chain */
        NptrsPage.bd910SetTo (NptrsSubRoot);
        do
        { 
            if ((NptrsPage.np_ptr()->nd_id() != NptrsPage.np_ptr()->nd_root()) ||
                (NptrsPage.np_ptr()->nd_free_obj_cnt() > 0)) 
                ++ChainStat.PageInFreeChainCnt;
            
            if (NptrsPage.np_ptr()->nd_free_obj_cnt() > 0)
                ++ChainStat.FreePageInFreeChainCnt;
            
            if (NptrsPage.np_ptr()->nd_in_free_chain()) 
            {
                ++ChainStat.PageWithFreeLabelInFreeChainCnt;
                if (NptrsPage.np_ptr()->nd_free_obj_cnt() > 0) 
                    ++ChainStat.FreePageWithFreeLabelInFreeChainCnt;
            }
            
            /* goto next page in free chain */
            NextPageNo = NptrsPage.np_ptr()->nd_next_free();
            PrevPageNo = NptrsPage.np_ptr()->nd_id();   // PTS 1121106  FF 
            NptrsPage.bd910ReleasePage();
            if (e_ok != TrError) 
            {
                Kernel_OpError( csp3_bd_msg, csp3_n_obj )
                    << __FILE__ << ":" << __LINE__ 
                    << " root:"  << SubRoot         
                    << " page:" << PrevPageNo   
                    << " error:" << TrError 
                    << " trying release page";
                if (e_ok == saveError) saveError = TrError;  
                TrError = e_ok;
            }
            
            if (NIL_PAGE_NO_GG00 == NextPageNo)
                break;
            else
            {
                NptrsPage.bd910AssignToPage (NextPageNo);
                if (e_ok != TrError) 
                {
                    Kernel_OpError( csp3_bd_msg, csp3_n_obj )
                        << __FILE__ << ":" << __LINE__ 
                        << " root:" 
                        << SubRoot         
                        << " page:" << PrevPageNo   
                        << " error:" << TrError 
                        << " trying access to next free page:" << NextPageNo;
                    if (e_ok != saveError) TrError = saveError;     
                    return;
                }
            }
        }
        while (ChainStat.PageInFreeChainCnt <= ChainStat.AllPageCnt);
    }
    if (e_ok != saveError) TrError = saveError;

    /* write vtrace entry */
    bd92_WriteChainStateToVtrace (Current, ChainStat, NptrsSubRoot, fd_smart, bChainIsDeleted);
}

/*---------------------------------------------------------------------------*/

static void
bd92_VerifyContObjDataPage (cbd05_Current &Current,
                            tbd_nodeptr       pPage)
{
    ROUTINE_DBG_MEO00 ("bd92_VerifyContObjDataPage");
    
    /* This function checks the counter of free, free_after_eot */
    /* and occupied objects for continuation object data pages. */
    /* Moreover, the free object chain is verified              */
    
    tgg00_BasisError      &TrError           = Current.curr_trans->trError_gg00;
    tgg00_BasisError      saveError          = TrError;                
    const int             MaxObjCnt          = pPage->nd_max_obj_cnt();
    const int             ObjFrameLen        = pPage->nd_obj_frame_len();
    const int             AlignedObjFrameLen = BD92ALIGN_OBJ_FRAME(ObjFrameLen);
    const int             NoReservedObjects  = 0;
    int                   ObjPos             = pPage->nd_first_obj();
    int                   NextFreeObjPos;
    int                   OccObjCnt          = 0;
    int                   FreeObjCnt         = 0;
    int                   FreeChainObjCnt    = 0;
    tbd900_ContObjFrame   *pContObjFrame;  
    
    /* recalculate object counters */ 
    for (int ObjCnt=1; ObjCnt <= MaxObjCnt; ++ObjCnt)
    {
        /*  construct position of the object frame  header             */   
        pContObjFrame = (tbd900_ContObjFrame *) ((tsp00_Byte*) pPage + ObjPos);
        
        /* count occurence of the various object  states */
        if (pContObjFrame->cofHeader_bd900.cvoRootOIdPno_bd900  != NIL_PAGE_NO_GG00      &&
            pContObjFrame->cofHeader_bd900.cvoRootOIdPos_bd900  != NIL_OBJ_PAGE_POS_GG92 &&
            pContObjFrame->cofHeader_bd900.cvoNextFreeObj_bd900 == NIL_OBJ_PAGE_POS_GG92)
        {
            ++OccObjCnt;
        };  
        ObjPos += AlignedObjFrameLen;
    }
    FreeObjCnt = MaxObjCnt - OccObjCnt;
    
    
    /* traverse free obj chain */ 
    NextFreeObjPos = pPage->nd_first_free_obj();
    
    while (NextFreeObjPos != NIL_OBJ_PAGE_POS_GG92)
    {
        ++FreeChainObjCnt;
        
        if (!bd92ObjPosIsValid (NextFreeObjPos, pPage->nd_first_obj(), ObjFrameLen))
        {
            Kernel_OpError  opErrMsg( csp3_bd_msg, csp3_n_obj );
            bd92_WritePageStateToVtrace (*Current.curr_trans, pPage, NoReservedObjects);  
            opErrMsg << "BD92VerContObjPage - root:" << pPage->nd_root()
                << " page:" << pPage->nd_id()
                << " wrong first free object pos:" << NextFreeObjPos;
            if (e_ok == saveError) saveError = e_data_page_corrupted; 
        }
        
        pContObjFrame = (tbd900_ContObjFrame *) ((tsp00_Byte*) pPage + NextFreeObjPos);
        
        if (pContObjFrame->cofHeader_bd900.cvoRootOIdPno_bd900  != NIL_PAGE_NO_GG00      ||
            pContObjFrame->cofHeader_bd900.cvoRootOIdPos_bd900  != NIL_OBJ_PAGE_POS_GG92 )
        {
            Kernel_OpError  opErrMsg( csp3_bd_msg, csp3_n_obj );
            bd92_WritePageStateToVtrace (*Current.curr_trans, pPage, NoReservedObjects); 
            opErrMsg << "BD92VerContObjPage - root:" << pPage->nd_root()
                << " page:" << pPage->nd_id()
                << " wrong cont obj" << NextFreeObjPos;
            if (e_ok == saveError) saveError = e_data_page_corrupted; // PTS 1121056
        }
        
        if (FreeChainObjCnt > FreeObjCnt)
        {
            Kernel_OpError  opErrMsg( csp3_bd_msg, csp3_n_obj );
            bd92_WritePageStateToVtrace (*Current.curr_trans, pPage, NoReservedObjects); 
            opErrMsg << "BD92VerContObjPage - root:" << pPage->nd_root()
                << " page:" << pPage->nd_id()
                << " wrong freechain obj count";
            if (e_ok == saveError) saveError = e_data_page_corrupted; // PTS 1121056
            break;
        }
        
        NextFreeObjPos = pContObjFrame->cofHeader_bd900.cvoNextFreeObj_bd900;
    }
    
    
    /* compare calculated counters with counters on page */
    if (FreeObjCnt        != pPage->nd_free_obj_cnt()     ||
        FreeChainObjCnt   != pPage->nd_free_obj_cnt()     ||
        0                 != pPage->nd_free_eot_obj_cnt() ||
        OccObjCnt         != pPage->nd_occ_obj_cnt())
    {
        bd92_WritePageStateToVtrace (*Current.curr_trans, pPage, NoReservedObjects);  
        
        if (FreeObjCnt != pPage->nd_free_obj_cnt())
        {
            Kernel_OpError  opErrMsg( csp3_bd_msg, csp3_n_obj );
            opErrMsg << "BD92VerContObjPage - root:" << pPage->nd_root()
                << " page:" << pPage->nd_id()
                << " wrong free count:" << FreeObjCnt;
            if (e_ok == saveError) saveError = e_data_page_corrupted; 
        }       
        
        if (0  != pPage->nd_free_eot_obj_cnt())
        {
            Kernel_OpError  opErrMsg( csp3_bd_msg, csp3_n_obj );
            opErrMsg << "BD92VerContObjPage - root:" << pPage->nd_root()
                << " page:" << pPage->nd_id()
                << " wrong feot count:" << pPage->nd_free_eot_obj_cnt();
            if (e_ok == saveError) saveError = e_data_page_corrupted; 
        }       
        
        if (OccObjCnt  != pPage->nd_occ_obj_cnt())
        {
            Kernel_OpError  opErrMsg( csp3_bd_msg, csp3_n_obj );
            opErrMsg << "BD92VerContObjPage - root:" << pPage->nd_root()
                << " page:" << pPage->nd_id()
                << " wrong occupied count:" << OccObjCnt;
            if (e_ok == TrError) TrError = e_data_page_corrupted; 
        }       
        
        if (FreeChainObjCnt != pPage->nd_free_obj_cnt())
        {
            Kernel_OpError  opErrMsg( csp3_bd_msg, csp3_n_obj );
            opErrMsg << "BD92VerContObjPage - root:" << pPage->nd_root()
                << " page:" << pPage->nd_id()
                << " wrong freeobj count:" << FreeChainObjCnt;
            if (e_ok == saveError) saveError = e_data_page_corrupted; // PTS 1121056;
        }       
    }
    if (e_ok != saveError) TrError = saveError;
}

/*---------------------------------------------------------------------------*/


static void
bd92_VerifyObjDataPage (cbd05_Current &Current,
                        tbd_nodeptr       pPage,
                        tsp00_Int4       &ResObjCnt,
                        const tsp00_Bool  bChainIsDeleted)  /* PTS XXXXXXX FF */
{
    ROUTINE_DBG_MEO00 ("bd92_VerifyObjDataPage");
    
    /* This function checks the counter of free, free_after_eot */
    /* and occupied objects for primary object data pages.      */
    /* Moreover, the free object chain is verified              */
    
    tgg00_BasisError  &TrError           = Current.curr_trans->trError_gg00;
    tgg00_BasisError  saveError          = TrError;                
    const int         MaxObjCnt          = pPage->nd_max_obj_cnt();
    const int         ObjFrameLen        = pPage->nd_obj_frame_len();
    const int         ObjKeyLen          = pPage->nd_obj_key_len();
    const int         AlignedObjFrameLen = BD92ALIGN_OBJ_FRAME(ObjFrameLen);
    int               ObjPos             = pPage->nd_first_obj();
    int               NextFreeObjPos;
    int               OccObjCnt          = 0;
    int               FreeObjCnt         = 0;
    int               FeotObjCnt         = 0;
    int               FreeChainObjCnt    = 0;
    ResObjCnt          = 0;
    bool              bThisTaskIsAGarbColl = bd91GarbCollController.bd930CheckIfTaskIsAGC (Current.curr_trans->trTaskId_gg00);
    tsp00_Bool        bIsActive;
    tbd900_ObjFrame  *pObjFrame;
    tgg92_KernelOid   OId;
    
    /* preset common parameters of OId */
    OId.gg92SetPno    (pPage->nd_id());
    
    /* recalculate  object counters */
    
    if (ObjKeyLen <= 0)
    {
        /* check data page without keys */    
        for (int ObjCnt=1; ObjCnt <= MaxObjCnt; ++ObjCnt)
        {
            /*  construct position of the object frame  header */   
            pObjFrame = cbd910_ObjDataNode::bd910ObjFramePtr ((tsp00_Byte *) pPage, ObjPos);  
            
            /* create OId for this object frame */
            OId.gg92SetPos       (ObjPos);
            OId.gg92SetFrameVers (pObjFrame->objHeader_bd900.ohdFrameVersion_gg92);
            
            /* count occurence of the various object  states */
            switch (pObjFrame->objHeader_bd900.ohdState_gg92)
            {
            case obsOccupied_egg00     : ++OccObjCnt  ; break; 
            case obsFree_egg00         : ++FreeObjCnt ; break;
            case obsFreeAfterEot_egg00 : 
                ++FeotObjCnt ;
                /* the following check isn't permitted, because if a GC is working actually on */
                /* an entry, then this entry may be marked as deleted within the history,      */
                /* but the object frame remains still unchanged             FF 2004-FEB-02     */
                break; 
                if (!bThisTaskIsAGarbColl && // a GC could create a dead lock
                    Kernel_IAdminInfo::Instance().KernelStateIsOnline() && // PTS 1121056  LockList not yet initialized
                    !bChainIsDeleted )     /* PTS XXXXXXX FF */
                {
                    bd92_CheckBeforeImage (*Current.curr_trans, OId, 
                        pObjFrame->objHeader_bd900.ohdTransInfo_gg92);
                }
                break;
            case obsReserved_egg00     : 
                ++OccObjCnt; 
                ++ResObjCnt;
                if (( !bChainIsDeleted ) &&
                    Kernel_IAdminInfo::Instance().KernelStateIsOnline())      // PTS 1121056
                {
                    /* since reserved and commited objects must not exist */ 
                    /* reserved objects must belong to open transactions  */

                    /* PTS 1127493 FF 2004-FEB-02       */
                    /* check if lock collision occurred */
                    if (pObjFrame->objHeader_bd900.ohdTransInfo_gg92.otrLockTransId_gg00.gg90IsDummy())
                        bIsActive = true;
                    else
                        kb50CheckActiveTrans (Current.curr_trans->trTaskId_gg00, 
                        pObjFrame->objHeader_bd900.ohdTransInfo_gg92.otrLockTransId_gg00,
                        pObjFrame->objHeader_bd900.ohdTransInfo_gg92.otrLockTransIndex_gg00,
                        bIsActive);
                    if (!bIsActive)
                    {
                        Kernel_OpError  opErrMsg( csp3_bd_msg, csp3_n_obj );
                        opErrMsg << "BD92VerObjDataPage - root:" << pPage->nd_root()
                            << " page:" << pPage->nd_id()
                            << " obj pos:" << ObjPos
                            << " committed obj with state reserved";
                        if (e_ok == saveError) saveError = e_data_page_corrupted; // PTS 1121056
                    }
                }
                break;  
            case obsKeyReserved_egg00  :
                {
                    Kernel_OpError  opErrMsg( csp3_bd_msg, csp3_n_obj );
                    opErrMsg << "BD92VerObjDataPage - root:" << pPage->nd_root()
                        << " page:" << pPage->nd_id()
                        << " obj pos:" << ObjPos
                        << " wrong state KeyReserved";
                    if (e_ok == saveError) saveError = e_data_page_corrupted; // PTS 1121056
                }
                break;
            default                     :
                {
                    Kernel_OpError  opErrMsg( csp3_bd_msg, csp3_n_obj );
                    opErrMsg << "BD92VerObjDataPage - root:" << pPage->nd_root()
                        << " page:" << pPage->nd_id()
                        << " obj pos:" << ObjPos
                        << " unknown state:" << pObjFrame->objHeader_bd900.ohdState_gg92;
                    if (e_ok == saveError) saveError = e_data_page_corrupted; // PTS 1121056
                }
                break; 
            };
            ObjPos += AlignedObjFrameLen;
        }
    }
    else
    {
        /* check data page with keys */   
        for (int ObjCnt=1; ObjCnt <= MaxObjCnt; ++ObjCnt)
        {
            /*  construct position of the object frame  header */
            pObjFrame = cbd910_ObjDataNode::bd910ObjFramePtr ((tsp00_Byte *) pPage, ObjPos);  
            
            /* create OId for this object frame */
            OId.gg92SetPos       (ObjPos);
            OId.gg92SetFrameVers (pObjFrame->objHeader_bd900.ohdFrameVersion_gg92);
            
            /* count occurence of the various object  states and check the hash entries */
            switch (pObjFrame->objHeader_bd900.ohdState_gg92)
            {
            case obsOccupied_egg00     : 
                bd92_CheckHashEntry (Current, pObjFrame, OId, ObjFrameLen, ObjKeyLen);
                ++OccObjCnt;
                break; 
            case obsFreeAfterEot_egg00 :
                /* the hash is not tested because of the objects     */
                /* created temporaly  when an older object is reused */
                ++FeotObjCnt;
                /* the following check isn't permitted, because if a GC is working actually on */
                /* an entry, then this entry may be marked as deleted within the history,      */
                /* but the object frame remains still unchanged             FF 2004-FEB-02     */
                break; 
                if (!bThisTaskIsAGarbColl && // a GC could create a dead lock
                    Kernel_IAdminInfo::Instance().KernelStateIsOnline() && // PTS 1121056 LockList not yet initialized
                    !bChainIsDeleted )     
                {
                    bd92_CheckBeforeImage (*Current.curr_trans, OId, 
                        pObjFrame->objHeader_bd900.ohdTransInfo_gg92);
                }
                break;
            case obsFree_egg00         : 
                ++FreeObjCnt;
                break;
            case obsKeyReserved_egg00  :
                /* The hash is not tested since an object could be Keyreserved  */ 
                /* but not yet inserted into the hash (see NewObj in vkb04.cpp) */
                ++OccObjCnt;
                ++ResObjCnt;
                if ( !bChainIsDeleted && 
                    Kernel_IAdminInfo::Instance().KernelStateIsOnline())      // PTS 1121056
                {
                    /* since reserved and commited objects must not exist */ 
                    /* reserved objects must belong to open transactions  */

                    /* PTS 1127493 FF 2004-FEB-02       */
                    /* check if lock collision occurred */
                    if (pObjFrame->objHeader_bd900.ohdTransInfo_gg92.otrLockTransId_gg00.gg90IsDummy())
                        bIsActive = true;
                    else
                        kb50CheckActiveTrans (Current.curr_trans->trTaskId_gg00, 
                        pObjFrame->objHeader_bd900.ohdTransInfo_gg92.otrLockTransId_gg00,
                        pObjFrame->objHeader_bd900.ohdTransInfo_gg92.otrLockTransIndex_gg00,
                        bIsActive);
                    if (e_ok != TrError) 
                    {
                        if (e_ok != saveError) saveError = TrError; 
                        TrError = e_ok;
                    }
                    if (!bIsActive)
                    {
                        Kernel_OpError  opErrMsg( csp3_bd_msg, csp3_n_obj );
                        opErrMsg << "BD92VerObjDataPage - root:" << pPage->nd_root()
                            << " page:" << pPage->nd_id()
                            << " obj pos:" << ObjPos
                            << " committed obj with state reserved";
                        if (e_ok == saveError) saveError = e_data_page_corrupted; 
                    }
                }
                break;  
            case obsReserved_egg00     : 
                {
                    Kernel_OpError  opErrMsg( csp3_bd_msg, csp3_n_obj );
                    opErrMsg << "BD92VerObjDataPage - root:" << pPage->nd_root()
                        << " page:" << pPage->nd_id()
                        << " obj pos:" << ObjPos
                        << " wrong state Reserved Keylen:" << ObjKeyLen;
                    if (e_ok == saveError) saveError = e_data_page_corrupted; 
                }
                break; 
            default                    :
                {
                    Kernel_OpError  opErrMsg( csp3_bd_msg, csp3_n_obj );
                    opErrMsg << "BD92VerObjDataPage - root:" << pPage->nd_root()
                        << " page:" << pPage->nd_id()
                        << " obj pos:" << ObjPos
                        << " unknown state:" << pObjFrame->objHeader_bd900.ohdState_gg92;
                    if (e_ok == saveError) saveError = e_data_page_corrupted; // PTS 1121056
                }
                break; 
            }     
            ObjPos += AlignedObjFrameLen;
        }
    }
    
    /* compare object counters */
    if (FreeObjCnt != pPage->nd_free_obj_cnt()     ||
        FeotObjCnt != pPage->nd_free_eot_obj_cnt() ||
        OccObjCnt  != pPage->nd_occ_obj_cnt()     )
    {
        bd92_WritePageStateToVtrace (*Current.curr_trans, pPage, ResObjCnt);  
        
        if (FreeObjCnt != pPage->nd_free_obj_cnt())
        {
            Kernel_OpError  opErrMsg( csp3_bd_msg, csp3_n_obj );
            opErrMsg << "BD92VerObjDataPage - root:" << pPage->nd_root()
                << " page:" << pPage->nd_id()
                << " wrong free count:" << FreeObjCnt;
            if (e_ok == saveError) saveError = e_data_page_corrupted; // PTS 1121056
        }       
        
        if (FeotObjCnt != pPage->nd_free_eot_obj_cnt())
        {
            Kernel_OpError  opErrMsg( csp3_bd_msg, csp3_n_obj );
            opErrMsg << "BD92VerObjDataPage - root:" << pPage->nd_root()
                << " page:" << pPage->nd_id()
                << " wrong feot count:" << FeotObjCnt;
            if (e_ok == saveError) saveError = e_data_page_corrupted; // PTS 1121056
        }       
        
        if (OccObjCnt  != pPage->nd_occ_obj_cnt())
        {
            Kernel_OpError  opErrMsg( csp3_bd_msg, csp3_n_obj );
            opErrMsg << "BD92VerObjDataPage - root:" << pPage->nd_root()
                << " page:" << pPage->nd_id()
                << " wrong occupied count:" << OccObjCnt;
            if (e_ok == saveError) saveError = e_data_page_corrupted; // PTS 1121056
        }
    }
    
    /* check if all pages with free  objects are             */
    /* contained in the corresponding object page chains     */
    /* note that it may happen that the GC has changed the   */
    /* page state to free but the page is still not icluded  */
    /* in the free list since the GC was interupted by an    */
    /* user. This user finds the page free but not included  */
    /* in the free chain.                                    */
    
    /* traverse free obj chain */ 
    NextFreeObjPos = pPage->nd_first_free_obj();
    
    while (NextFreeObjPos != NIL_OBJ_PAGE_POS_GG92)
    {
        ++FreeChainObjCnt;
        
        if (!bd92ObjPosIsValid (NextFreeObjPos, pPage->nd_first_obj(), ObjFrameLen))
        {
            Kernel_OpError  opErrMsg( csp3_bd_msg, csp3_n_obj );
            bd92_WritePageStateToVtrace (*Current.curr_trans, pPage, ResObjCnt);  
            opErrMsg << "BD92VerObjDataPage - root:" << pPage->nd_root()
                << " page:" << pPage->nd_id()
                << " wrong next free object pos:" << NextFreeObjPos;
            if (e_ok == saveError) saveError = e_data_page_corrupted; 
        }
        
        pObjFrame = cbd910_ObjDataNode::bd910ObjFramePtr ((tsp00_Byte *) pPage, NextFreeObjPos);  
        
        if (pObjFrame->objHeader_bd900.ohdState_gg92 != obsFree_egg00)
        {
            Kernel_OpError  opErrMsg( csp3_bd_msg, csp3_n_obj );
            bd92_WritePageStateToVtrace (*Current.curr_trans, pPage, ResObjCnt); 
            opErrMsg << "BD92VerObjDataPage - root:" << pPage->nd_root()
                << " page:" << pPage->nd_id()
                << " wrong object state:" << pObjFrame->objHeader_bd900.ohdState_gg92;
            if (e_ok == saveError) saveError = e_data_page_corrupted; 
        }
        
        if (FreeChainObjCnt > FreeObjCnt)
        {
            Kernel_OpError  opErrMsg( csp3_bd_msg, csp3_n_obj );
            bd92_WritePageStateToVtrace (*Current.curr_trans, pPage, ResObjCnt); 
            opErrMsg << "BD92VerObjDataPage - root:" << pPage->nd_root()
                << " page:" << pPage->nd_id()
                << " wrong freechain obj count:" << FreeObjCnt;
            if (e_ok == saveError) saveError = e_data_page_corrupted; 
            break;
        }
        
        NextFreeObjPos = pObjFrame->objHeader_bd900.ohdNextFreeObj_gg92;
    }
    
    /* compare calculated counters with counters on page */
    if (FreeChainObjCnt != pPage->nd_free_obj_cnt())
    {
        Kernel_OpError  opErrMsg( csp3_bd_msg, csp3_n_obj );
        opErrMsg << "BD92VerContObjPage - root:" << pPage->nd_root()
            << " page:" << pPage->nd_id()
            << " wrong free chain len:" << FreeChainObjCnt;
        if (e_ok == saveError) saveError = e_data_page_corrupted; 
    } 
    if (e_ok != saveError) TrError = saveError;
}

/*---------------------------------------------------------------------------*/

static void
bd92_VerifyObjPage (cbd05_Current     &Current,
                    tbd_node_ptrs     &NptrsPage,
                    tsp00_Int4        &ResObjCnt,
                    const tsp00_Bool  bChainIsDeleted)  
{
    ROUTINE_DBG_MEO00 ("bd92_VerifyObjPage");
    
    /* This routine checks the consistency */
    /* of the contents of an object page   */
    
    /* initialize return parameter */
    ResObjCnt = 0;
    tgg00_BasisError        &TrError  = Current.curr_trans->trError_gg00;
    tgg00_BasisError        saveError = TrError;                

    tbd_nodeptr   & pPage = NptrsPage.np_ptr();
    
    /* check page type */
    if (pPage->nd_pt()   !=  ptData_egg00        ||
        (pPage->nd_pt2() !=  pt2Object_egg00     &&
        pPage->nd_pt2()  !=  pt2VarObject_egg00  && 
        pPage->nd_pt2()  !=  pt2ContObject_egg00)) 
    {
        Kernel_OpError( csp3_bd_msg, csp3_n_obj )
            << __FILE__ << ":" << __LINE__
            << " root:" << NptrsPage.np_ptr()->nd_root()
            << " actual page:" << NptrsPage.np_ptr()->nd_id()
            << " wrong page type nd_pt:" << pPage->nd_pt()
            << " nd_pt2:" << pPage->nd_pt2();
        if ( e_ok == TrError )                  
            TrError = e_data_page_corrupted;    
    }
    else
    {
        /* check that the page has no next free page */
        /* if it is not a member of the free chain   */
        if (!pPage->nd_in_free_chain()                      && 
            (pPage->nd_id()        != pPage->nd_root()) &&
            (pPage->nd_next_free() != NIL_PAGE_NO_GG00))
        {
            Kernel_OpError( csp3_bd_msg, csp3_n_obj )
                << __FILE__ << ":" << __LINE__
                << " root:" << NptrsPage.np_ptr()->nd_root()
                << " actual page:" << NptrsPage.np_ptr()->nd_id()
                << " bad NextFree pointer ";
            if ( e_ok == saveError )
                saveError = e_data_page_corrupted;    
        }
        
        /* check that the page has no next free page */
        /* if it is not a member of the free chain   */
        if (!pPage->nd_in_free_chain() && (pPage->nd_free_obj_cnt() > 1)
	        && !NptrsPage.np_ptr()->nd_objpage_state().includes(objp_no_chain_to_freelist))
        {
            Kernel_OpError  opErrMsg( csp3_bd_msg, csp3_n_obj );
            opErrMsg << "BD92VerObjPage - root:" << NptrsPage.np_ptr()->nd_root()
                << " actual page:" << NptrsPage.np_ptr()->nd_id()
                << " bad FreeCount";
            if ( e_ok == saveError )
                saveError = e_data_page_corrupted; 
        }
        
        /* check page header */
        bd92_VerifyObjPageHeader (Current, pPage);
        if ( e_ok == saveError ) saveError = TrError;    
        TrError = e_ok;
        
        /* check page contents */
        if (pPage->nd_id() == pPage->nd_root()) 
        {
            bd92_VerifyObjRootPage (Current, NptrsPage);  
            if ( e_ok == saveError ) saveError = TrError;    
            TrError = e_ok;
        }

        if (pPage->nd_id() == pPage->nd_root())
        {
            bd92_VerifyObjSubRootPage (Current, NptrsPage);
            if ( e_ok == saveError ) saveError = TrError;    
            TrError = e_ok;
        }

        
        if (pPage->nd_pt2()  ==  pt2ContObject_egg00)
            bd92_VerifyContObjDataPage (Current, pPage ); 
        else
            bd92_VerifyObjDataPage (Current, pPage, ResObjCnt, bChainIsDeleted ); 

        if ( e_ok != saveError )
            TrError = saveError;
    }
}

/*---------------------------------------------------------------------------*/

static void
bd92_VerifyObjPageHeader (cbd05_Current     &Current,
                          tbd_nodeptr        pPage)
{             
    /* check if the data in the page header are consistent */
    /* with those in the object file directory             */
    
    bool                  bHeaderOk     = true;
    tgg00_BasisError      &TrError      = Current.curr_trans->trError_gg00;
    tgg00_BasisError       saveError    = TrError; 

    FileDir_ReturnCode     fd_rc;
    FileDir_Oms            fd_smart;
    tgg00_TransContext    &Trans    = *Current.curr_trans;
    FileDir_FileNo         FileNo;
    Msg_List               errorList;

    bd900GetFileNo(pPage, FileNo);
    fd_rc = FileDir_IDirectory::Instance(cgg_nil_transindex).GcGetFile( Trans.trTaskId_gg00, FileNo, fd_smart, errorList);
    if (FileDir_Okay != fd_rc) {
        Kernel_OpError ( csp3_bd_msg, csp3_n_obj ) 
            << __FILE__ << ":" << __LINE__
            << "BD92VerObjPageHdr - FileId:" << FileNo
            << " not found"; 
        if ( e_ok == TrError ) TrError = e_file_not_found;
        bHeaderOk = false;
        return;
    }

    Data_PageNo root = fd_smart.GetRootPage();
    if (pPage->nd_pt2() == pt2ContObject_egg00)
        root = fd_smart.GetContPartition(Current.curr_tree_id.fileContObjFileNo_gg00());

    /* check maximum number of objects and position of first object */ 
    if (pPage->nd_root() == pPage->nd_id())
    {
        if (pPage->nd_max_obj_cnt() > fd_smart.GetObjPerPageCount())
        {
            Kernel_OpError ( csp3_bd_msg, csp3_n_obj )
                << __FILE__ << ":" << __LINE__
                << " BD92VerObjPageHdr - root:" << pPage->nd_root()
                << " page:" << pPage->nd_id() 
                << " wrong MaxObjCnt:" << pPage->nd_max_obj_cnt()
                << " ObjFileInfo.MaxObjCnt:" 
                << fd_smart.GetObjPerPageCount();
            if ( e_ok == saveError ) saveError = e_invalid_root;
            bHeaderOk = false;
        }
        
        if (pPage->nd_first_obj() != BD92ALIGN_OBJ_FRAME (NODE_HEADER_MXBD00))
        {
            Kernel_OpError ( csp3_bd_msg, csp3_n_obj )
                << __FILE__ << ":" << __LINE__
                << " BD92VerObjPageHdr - root:" << pPage->nd_root()
                << " page:" << pPage->nd_id() 
                << " wrong FirstObjPos:" << pPage->nd_first_obj();
            if ( e_ok == saveError ) saveError = e_invalid_root;
            bHeaderOk = false;
        }
    }
    else
    {
        if (pPage->nd_max_obj_cnt() > fd_smart.GetObjPerPageCount())
        {
            Kernel_OpError ( csp3_bd_msg, csp3_n_obj )
                << __FILE__ << ":" << __LINE__
                << " BD92VerObjPageHdr - root:" << pPage->nd_root()
                << " page:" << pPage->nd_id() 
                << " wrong MaxObjCnt:" << pPage->nd_max_obj_cnt()
                << " ObjFileInfo.MaxObjCnt:" 
                << fd_smart.GetObjPerPageCount();
            if ( e_ok == saveError ) saveError = e_invalid_root;
            bHeaderOk = false;
        }
        
        if (pPage->nd_first_obj() != NODE_HEADER_MXBD00)
        {
            Kernel_OpError ( csp3_bd_msg, csp3_n_obj )
                << __FILE__ << ":" << __LINE__
                << " BD92VerObjPageHdr - root:" << pPage->nd_root()
                << " page:" << pPage->nd_id() 
                << " wrong FirstObjPos:" << pPage->nd_first_obj();
            if ( e_ok == saveError ) saveError = e_invalid_root;
            bHeaderOk = false;
        }
    }
    
    if ((pPage->nd_first_free_obj() != NIL_OBJ_PAGE_POS_GG92) && 
        !bd92ObjPosIsValid (pPage->nd_first_free_obj(),  pPage->nd_first_obj(), pPage->nd_obj_frame_len()))
        bHeaderOk = false;
    
    /* check page type */
    if ((pPage->nd_pt2() == pt2VarObject_egg00 &&
        !Data_OmsVarFile == fd_smart.GetFileType())      ||
        (pPage->nd_pt2() == pt2Object_egg00 &&
        !(Data_OmsFixedFile == fd_smart.GetFileType() ||
          Data_OmsKeyFile   == fd_smart.GetFileType()))  ||
        (pPage->nd_pt2() == pt2ContObject_egg00    &&
        !Data_OmsContFile == fd_smart.GetFileType()))
    {
        Kernel_OpError ( csp3_bd_msg, csp3_n_obj )
            << __FILE__ << ":" << __LINE__
            << " BD92VerObjPage - root:" << pPage->nd_root()
            << " actual page:" << pPage->nd_id()
            << " wrong page type nd_pt:" << pPage->nd_pt()
            << " nd_pt2:" << pPage->nd_pt2()
            << " FileType:" 
            << fd_smart.GetFileType();
        if ( e_ok == saveError ) saveError = e_data_page_corrupted; // PTS 1121056
        bHeaderOk = false;
    }

    if ( e_ok != saveError )
        TrError = saveError;
}
                          
/*---------------------------------------------------------------------------*/

static void
bd92_VerifyObjRootPage (cbd05_Current  &Current,
                        tbd_node_ptrs  &NptrsRoot)
{
    ROUTINE_DBG_MEO00 ("bd92VerifyObjRootPage");
    
    /* check root page and compare data with ObjFileInfo */
    
    tbd_nodeptr          &pRoot        = NptrsRoot.np_ptr();
    tgg00_BasisError     &TrError      = Current.curr_trans->trError_gg00;

    FileDir_FileNo   FileNo;
    bd900GetFileNo(pRoot, FileNo);
    FileDir_ReturnCode     fd_rc;
    FileDir_Oms            fd_smart;
    tgg00_TransContext    &Trans    = *Current.curr_trans;
    Msg_List               errorList;

    fd_rc = FileDir_IDirectory::Instance(cgg_nil_transindex).GcGetFile( Trans.trTaskId_gg00, FileNo, fd_smart, errorList);
    if (FileDir_Okay != fd_rc) {
        Kernel_OpError ( csp3_bd_msg, csp3_n_obj ) 
            << __FILE__ << ":" << __LINE__
            << " bd92VerifyObjRootPage - FileId:" << FileNo
            << " FileDir error:" << fd_rc; 
        if ( e_ok == TrError ) TrError = e_file_not_found;
        return;
    }

    if (pRoot->nd_pt2() == pt2ContObject_egg00)
    {
        FileDir_OmsVar     fd_var_smart;
        fd_rc = FileDir_IDirectory::Instance(cgg_nil_transindex).GcGetFile( Trans.trTaskId_gg00, FileNo, fd_var_smart, errorList);
        if (FileDir_Okay != fd_rc) {
            Kernel_OpError ( csp3_bd_msg, csp3_n_obj ) 
                << __FILE__ << ":" << __LINE__
                << " bd92VerifyObjRootPage - FileId:" << FileNo
                << " FileDir error:" << fd_rc; 
            if ( e_ok == TrError ) TrError = e_file_not_found;
            return;
        }

        SAPDB_UInt2 iContFileNo;
        for (iContFileNo=1; iContFileNo <= MAX_NUM_CONT_OBJ_FILES_BD900; ++iContFileNo)           
        {
            if (pRoot->nd_root() == fd_var_smart.GetContPartition(iContFileNo))
                break;
        }
        if (!fd_var_smart.IsAssigned() || (pRoot->nd_root() != fd_var_smart.GetContPartition(iContFileNo)))
            g01abort (csp3_bd_msg, csp3_n_obj,"BD92_VER:ContRoot NoReg ",pRoot->nd_id());
    }
}

/*---------------------------------------------------------------------------*/

static void
bd92_VerifyObjSubRootPage (cbd05_Current     &Current,
                           tbd_node_ptrs     &NptrsSubRoot)
{
    ROUTINE_DBG_MEO00 ("bd92VerifyObjSubRootPage");
    
    tgg00_BasisError       &TrError       = Current.curr_trans->trError_gg00;
    tgg00_BasisError        saveError     = TrError; 
    tbd_nodeptr            &pSubRoot      = NptrsSubRoot.np_ptr();
    
    FileDir_FileNo   FileNo;
    bd900GetFileNo(pSubRoot, FileNo);
    FileDir_ReturnCode     fd_rc;
    FileDir_Oms            fd_smart;
    tgg00_TransContext    &Trans    = *Current.curr_trans;
    Msg_List               errorList;

    fd_rc = FileDir_IDirectory::Instance(cgg_nil_transindex).GcGetFile( Trans.trTaskId_gg00, FileNo, fd_smart, errorList);
    if (FileDir_Okay != fd_rc) {
        Kernel_OpError ( csp3_bd_msg, csp3_n_obj ) 
            << __FILE__ << ":" << __LINE__
            << " bd92_VerifyObjSubRootPage - FileId:" << FileNo
            << " FileDir error:" << fd_rc; 
        if ( e_ok == TrError ) TrError = e_file_not_found;
        return;
    }

    if (pSubRoot->nd_pt2() == pt2ContObject_egg00)
    {
      FileDir_OmsVar     fd_var_smart;
      fd_rc = FileDir_IDirectory::Instance(cgg_nil_transindex).GcGetFile( Trans.trTaskId_gg00, FileNo, fd_var_smart, errorList);
      SAPDB_UInt2 iContFileNo;
      for (iContFileNo=1; iContFileNo <= MAX_NUM_CONT_OBJ_FILES_BD900; ++iContFileNo)           
      {
          if (pSubRoot->nd_root() == fd_var_smart.GetContPartition(iContFileNo))
              break;
      }
      if (!fd_var_smart.IsAssigned() || (pSubRoot->nd_root() != fd_var_smart.GetContPartition(iContFileNo)))
      {
          Kernel_OpError ( csp3_bd_msg, csp3_n_obj ) 
              << __FILE__ << ":" << __LINE__
              << " bd92_VerifyObjSubRootPage - subroot:" << pSubRoot->nd_id()
              << " not found in directory";
          if ( e_ok == saveError ) saveError = e_data_page_corrupted; // PTS 1121056 
      }
    }

    /* PTS 1140252 */
    /* For release with change numbers between 96750 and 107371 the pagecount for OmsVarObject container has been initialized */
    /* to 1 instead of 7.  ( 7.6.0 Build 10 up to Build 15 ) Therefore ignore check                                           */
    if (((fd_smart.GetFreePageCount(Trans.trTaskId_gg00) < 0) ||
        (fd_smart.GetFreePageCount(Trans.trTaskId_gg00) > (int)fd_smart.GetLeafPageCount(Current.curr_trans->trTaskId_gg00)))
        && (pSubRoot->nd_pt2() != pt2VarObject_egg00) && (pSubRoot->nd_pt2() !=  pt2ContObject_egg00)) 
    {
        Kernel_OpError ( csp3_bd_msg, csp3_n_obj ) 
            << __FILE__ << ":" << __LINE__
            << " bd92_VerifyObjSubRootPage - subroot:" << pSubRoot->nd_id()
            << " wrong FreePageCount SubRootInfo:" << fd_smart.GetFreePageCount(Trans.trTaskId_gg00);
        if (e_ok == saveError)  saveError = e_wrong_all_page_count;
    }    
    if (e_ok != saveError)  TrError = saveError;
}

/*---------------------------------------------------------------------------*/

static void
bd92_WriteChainStateToVtrace (cbd05_Current                   &Current, 
                              const  tbd92_ObjPageChainStat   &ChainStat,
                              cbd910_SubRootNode              &NptrsSubRoot,
                              FileDir_Oms                     &fd_smart,
                              const tsp00_Bool                 bChainIsDeleted)
{
    ROUTINE_DBG_MEO00 ("bd92_WriteChainStateToVtrace");
    
    {
        tgg12_OmsFilenameTrace TrLineIn;
        
        TrLineIn.ofnTrType_gg12.becomes(otFilename_egg12);
        TrLineIn.ofnTrType2_gg12.becomes(ot2Nil_egg12);
        TrLineIn.ofnFileName_gg12 = Current.curr_tree_id.fileName_gg00();
        
        Trace_CommonEntry entry( tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(b92verify), 
            REINTERPRET_CAST( tsp00_BytePtr, &TrLineIn ), sizeof (TrLineIn) );
    }
    {
        tgg12_OmsPageChainStatTrace TrLineChain;
        
        TrLineChain.opctSubRoot_gg12             =  NptrsSubRoot.np_ptr()->nd_id();
        TrLineChain.opctTrType_gg12.becomes(otPageChainStat_egg12);
        TrLineChain.opctAllPageCnt_gg12          =  ChainStat.AllPageCnt;
        TrLineChain.opctEmptyPageCnt_gg12        =  0;
        TrLineChain.opctFreePageCnt_gg12         =  ChainStat.FreePageCnt;
        TrLineChain.opctFeotPageCnt_gg12         =  ChainStat.FeotPageCnt;
        
        TrLineChain.opctAllPageRootCnt_gg12      =  fd_smart.GetLeafPageCount(Current.curr_trans->trTaskId_gg00);
        TrLineChain.opctEmptyPageRootCnt_gg12    =  0;
        TrLineChain.opctFreePageRootCnt_gg12     =  fd_smart.GetFreePageCount(Current.curr_trans->trTaskId_gg00);
        
        TrLineChain.opctPageInFreeChainCnt_gg12  =  ChainStat.PageInFreeChainCnt;
        
        TrLineChain.opctFreePageInFreeChainCnt_gg12  = ChainStat.FreePageInFreeChainCnt; 
        TrLineChain.opctEmptyPageInFreeChainCnt_gg12 = 0;
        
        TrLineChain.opctPageWithEmptyLabelCnt_gg12 = 0;
        TrLineChain.opctPageWithFreeLabelCnt_gg12  = ChainStat.PageWithFreeLabelCnt;
        
        TrLineChain.opctPageWithFreeLabelInFreeChainCnt_gg12 = ChainStat.PageWithFreeLabelInFreeChainCnt;
        
        TrLineChain.opctFreePageWithFreeLabelInFreeChainCnt_gg12 = ChainStat.FreePageWithFreeLabelInFreeChainCnt;
        
        TrLineChain.opctFreeObjCnt_gg12 = ChainStat.FreeObjCnt;
        TrLineChain.opctFeotObjCnt_gg12 = ChainStat.FeotObjCnt;
        TrLineChain.opctOccObjCnt_gg12  = ChainStat.OccObjCnt;
        TrLineChain.opctResObjCnt_gg12  = ChainStat.ResObjCnt;
        
        TrLineChain.opctIsChainDeleted_gg12 = bChainIsDeleted;
        
        Trace_CommonEntry entry( tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(b92verify), 
            REINTERPRET_CAST( tsp00_BytePtr, &TrLineChain ), sizeof (TrLineChain) );
    }
}


/*---------------------------------------------------------------------------*/

static void
bd92_WriteContainerStateToVtrace (cbd05_Current                        &Current, 
                                  const  tbd92_ObjPageContainerStat    &ContainerStat,
                                  FileDir_Oms                          &fd_smart)
{ 
    ROUTINE_DBG_MEO00 ("bd92_WriteContainerStateToVtrace");
    
    {
        tgg12_OmsFilenameTrace TrLineIn;
        
        TrLineIn.ofnTrType_gg12.becomes(otFilename_egg12);
        TrLineIn.ofnTrType2_gg12.becomes(ot2Nil_egg12);
        TrLineIn.ofnFileName_gg12 = Current.curr_tree_id.fileName_gg00();
        
        Trace_CommonEntry entry( tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(b92verify), 
            REINTERPRET_CAST( tsp00_BytePtr, &TrLineIn ), sizeof (TrLineIn) );
    }
    {
        tgg12_OmsPageContainerStatTrace TrLineCon;
        
        TrLineCon.opcoTrType_gg12.becomes(otPageContainerStat_egg12);
        TrLineCon.opcoFreeObjCnt_gg12    =  ContainerStat.FreeObjCnt;       
        TrLineCon.opcoFeotObjCnt_gg12    =  ContainerStat.FeotObjCnt; 
        TrLineCon.opcoOccObjCnt_gg12     =  ContainerStat.OccObjCnt;
        TrLineCon.opcoResObjCnt_gg12     =  ContainerStat.ResObjCnt;
        TrLineCon.opcoAllPageCnt_gg12    =  ContainerStat.AllPageCnt; 
        TrLineCon.opcoEmptyPageCnt_gg12  =  0;
        TrLineCon.opcoFreePageCnt_gg12   =  ContainerStat.FreePageCnt;
        TrLineCon.opcoFeotPageCnt_gg12   =  ContainerStat.FeotPageCnt;
        TrLineCon.opcoNumSubRoots_gg12   =  1;
        TrLineCon.opcoIsFileDeleted_gg12 =  (Data_FileDeleted == fd_smart.GetFileState());
        
        Trace_CommonEntry entry( tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(b92verify), 
            REINTERPRET_CAST( tsp00_BytePtr, &TrLineCon ), sizeof (TrLineCon) );
    }
}

/*---------------------------------------------------------------------------*/

static void
bd92_WritePageStateToVtrace (tgg00_TransContext     &Trans,
                             tbd_nodeptr             pPage,
                             tsp00_Int4              ResObjCnt)
{                             
    ROUTINE_DBG_MEO00 ("bd92_WritePageStateToVtrace");
    
    tgg12_OmsPageStatTrace     TrLinePage;
    
    TrLinePage.opsTrType_gg12.becomes(otPageStat_egg12);
    TrLinePage.opsPno_gg12           = pPage->nd_id();
    TrLinePage.opsOccObjCnt_gg12     = pPage->nd_occ_obj_cnt();
    TrLinePage.opsFreeObjCnt_gg12    = pPage->nd_free_obj_cnt();
    TrLinePage.opsFreeEotObjCnt_gg12 = pPage->nd_free_eot_obj_cnt();
    TrLinePage.opsResObjCnt_gg12     = ResObjCnt;                
    
    Trace_CommonEntry entry( tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(b92verify), 
        REINTERPRET_CAST( tsp00_BytePtr, &TrLinePage ), sizeof (TrLinePage) );
}

/*===========================================================================*
*  END OF CODE                                                              *
*===========================================================================*/
