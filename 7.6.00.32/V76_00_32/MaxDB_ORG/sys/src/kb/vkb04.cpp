/****************************************************************************
  module      : vkb04.cpp
  author      : JuergenA,UweH
  responsible : UweH
  special area: KB_object_handling_interface
  last changed: 2003-10-01  12:00
  see also    : 
  description : 

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

#include <string.h>

#ifndef HKB04_H
#   include "hkb04.h"
#endif
#ifndef HBD04_H
#   include "hbd04.h"
#endif
#ifndef HBD90_H
#   include "hbd90.h"
#endif
#ifndef GGG01_H
#   include "ggg01.h"
#endif
#ifndef HGG01_3_H
#   include "hgg01_3.h"
#endif
#ifndef HKB05_H
#   include "hkb05.h"
#endif
#ifndef HKB50_H
#   include "hkb50.h"   /* PTS 1120151 FF 2003-JAN-29 */
#endif
#ifndef HKB51_H
#   include "hkb51.h"
#endif
#ifndef HKB512_H
#   include "hkb512.h"
#endif
//#include "hkb53.h"
#ifndef HKB67_H
#   include "hkb67.h"
#endif

#ifndef Log_Transaction_HPP
#   include "Logging/Log_Transaction.hpp"
#endif
#ifndef LOG_ACTIONOBJECT_HPP
#   include "Logging/Log_ActionObject.hpp"
#endif
#ifndef LOG_ACTIONOBJECTFILE_HPP
#   include "Logging/Log_ActionObjectFile.hpp"
#endif
#ifndef LVC_TYPES_HPP
#   include "liveCache/LVC_Types.hpp"
#endif
#ifndef TRACE_ENTRY_HPP
#   include "Trace/Trace_Entry.hpp"
#endif
#ifndef KERNEL_OPMSG_HPP
#   include "KernelCommon/Kernel_OpMsg.hpp"  
#endif
#ifndef HKB560_H
#   include "hkb560.h"                   // kb560ConvertWriteRedoEntryResultToBasisError
#endif
#ifndef KERNEL_VTRACE_HPP
#   include "KernelCommon/Kernel_VTrace.hpp"         
#endif
#ifndef SQLMAN_CONTEXT_HPP
#   include "SQLManager/SQLMan_Context.hpp"
#endif
#ifndef TRANS_CONTEXT_HPP
#   include "Transaction/Trans_Context.hpp"
#endif
#ifndef FILEDIR_IDIRECTORY_HPP
#   include "FileDirectory/FileDir_IDirectory.hpp"
#endif
#ifndef FILEDIR_OMSKEY_HPP
#   include "FileDirectory/FileDir_OmsKey.hpp"
#endif
#ifndef FILEDIR_GENERIC_HPP
#   include "FileDirectory/FileDir_Generic.hpp"
#endif
#ifndef MSG_LIST_HPP
#   include "Messages/Msg_List.hpp"
#endif

extern SAPDBTrace_Topic ObjContainer_Trace;

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define BEGIN_SUBTRANS_SET  true

struct t_TraceOutput
{
    tgg00_MessType     trcMessType;
    tgg00_MessType2    trcMessType2;
    tgg00_BasisError   trcError; 
    tgg91_TransNo      trcConsistView;
};

/*===========================================================================*
 *  GLOBAL FUNCTIONS                                                         *
 *===========================================================================*/

inline static void kb04_EnterKb (tgg00_TransContext    &Trans,
                                 tgg00_MessType_Enum    MessType,
                                 tgg00_MessType2_Enum   MessType2,
                                 bool                   WriteVtrace,
                                 bool                   SubtransRequested,
                                 bool                   ignoreCancelFlag = false ) 
{
    Trans_Context &TransCtx = Trans_Context::GetContext(Trans);

    if ( ! TransCtx.ErrorList().IsEmpty() )
    {
        RTE_Message(TransCtx.ErrorList());
        TransCtx.ErrorList().ClearMessageList();
    }

    if (e_ok != Trans.trError_gg00)
	{
        if ((g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdObject_gg00))
            Kernel_VTrace() << __FILE__ << " : " << __LINE__
            << " Error:" << Trans.trError_gg00;
    }
    else
    {
        Trans.trRteCommPtr_gg00->virt_reads = -MAX_INT2_SP00;
        if  (WriteVtrace)
        {
            t_TraceOutput TraceOutput;

            TraceOutput.trcMessType.becomes(MessType);
            TraceOutput.trcMessType2.becomes(MessType2);
            TraceOutput.trcError       = e_ok;
            TraceOutput.trcConsistView = Trans.trConsistView_gg00; 

            Trace_CommonEntry entry( tgg00_Debug::fromConst(kb), tgg00_VtraceType::fromConst(kb04oms), 
                REINTERPRET_CAST( tsp00_BytePtr, &TraceOutput ), sizeof (TraceOutput) );
        }

        if ( Trans.trRteCommPtr_gg00->to_cancel && ! ignoreCancelFlag ) 
        {
            Trans.trError_gg00 = e_cancelled;
            if (g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdObject_gg00)
                Kernel_VTrace() << __FILE__ << ":" << __LINE__ << " kb04_EnterKb e_cancelled"; 
        }
        else if (cgg_nil_transindex == Trans.trIndex_gg00)
        {
            Trans.trError_gg00 = e_nil_transindex;      // trans entry of this session not found
            if ((g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdObject_gg00))
                Kernel_VTrace() << __FILE__ << ":" << __LINE__ << " kb04_EnterKb e_nil_transindex"; 
        } 
        else if ( SubtransRequested )
        {
            Trans.trState_gg00.delElement (tsFunctRollbSet_egg00);
            REINTERPRET_CAST(Log_Transaction*,Trans.trLogContext_gg00)->BeginSubtrans();
            if ((Trans.trError_gg00 != e_ok) && ((g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdObject_gg00)))
                Kernel_VTrace() << __FILE__ << ":" << __LINE__ 
                << " BeginSubtrans error:"
                << Trans.trError_gg00; 
        }
    }   
#   if COMPILEMODE_MEO00 >= SLOW_MEO00
    t01trans (kb, "Trans (KB04)", Trans);
#   endif   
}


/*-----------------------------------------------------------------------------------*/

inline static void kb04_ErrorHandling (tgg00_TransContext   &Trans,
                                       tgg00_MessType_Enum   MessType,
                                       bool                  WriteVtrace,
                                       bool                  SubtransRequested)
{
    Trans_Context &TransCtx = Trans_Context::GetContext(Trans);
    if ( ! TransCtx.ErrorList().IsEmpty() ) // UH 2004-09-09 errlist handling added
    {
        RTE_Message(TransCtx.ErrorList());
        TransCtx.ErrorList().ClearMessageList();
    }

    if  (e_ok != Trans.trError_gg00)
    {
        kb05ReturnErrorHandling (Trans, SubtransRequested, MessType);
    }
    else
    {
        if ( SubtransRequested )
            REINTERPRET_CAST(Log_Transaction*,Trans.trLogContext_gg00)->EndSubtrans();
    }
    
    if  (WriteVtrace)
    {
        t_TraceOutput TraceOutput;
        
        TraceOutput.trcMessType.becomes(m_return_error);
        TraceOutput.trcMessType2.becomes((m_begin == MessType) ? mm_trans : mm_nil); // mark StartConsistTrans
        TraceOutput.trcError       = Trans.trError_gg00;
        TraceOutput.trcConsistView = Trans.trConsistView_gg00;
        
        Trace_CommonEntry entry( tgg00_Debug::fromConst(kb), tgg00_VtraceType::fromConst(kb04oms), 
            REINTERPRET_CAST( tsp00_BytePtr, &TraceOutput ), sizeof (TraceOutput) );
    }
}

/*-----------------------------------------------------------------------------------*/

externCpp void
kb04CreateObjFile (tgg00_TransContext   &Trans,
                   tgg00_FileId         &ObjFile,
                   tsp00_Int4            ObjBodyLen,
                   tsp00_Int4            ObjKeyLen,
                   tsp00_Bool            bCreateSeveralObjKeyFiles)
{ 
    SAPDBTRACE_ROUTINE_DEBUG("kb04CreateObjFile", ObjContainer_Trace, 5);

    Msg_List& ErrorList = Trans_Context::GetContext(Trans).ErrorList();

    const tgg00_MessType_Enum   ThisMessType      = m_create_file;
    const tgg00_MessType2_Enum  ThisMessType2     = mm_object;
    const bool                  WriteVtrace       = (g01vtrace.vtrAll_gg00 || g01vtrace.vtrOmsNew_gg00 );
    const bool                  bOnlineCreateFile = true;

    kb04_EnterKb (Trans, ThisMessType, ThisMessType2, WriteVtrace, ! BEGIN_SUBTRANS_SET);
    if (e_ok != Trans.trError_gg00) return; 

    tgg00_LockReqMode  ReqMode;
    ReqMode.becomes(lckTabExcl_egg00);
    bd90LockObjContainer(Trans, ObjFile.fileDirFileId_gg00(), ReqMode);

    if  (e_ok == Trans.trError_gg00)
    {
        ObjFile.fileVersion_gg00().gg90SetZero();

        Data_PageNo         Root;
        tsp00_Uint1         contContainerIdx = PRIM_CONT_OBJ_FILE_NO_BD900;

        LVC_FileNoArray     FileNoArray;
        bd04CreateObjFile(Trans, ObjFile, ObjBodyLen, ObjKeyLen, bCreateSeveralObjKeyFiles, 
            bOnlineCreateFile, Root, contContainerIdx, FileNoArray, ErrorList);

        if  (e_ok == Trans.trError_gg00)
        {
            Log_Transaction     &logtrans    = *(reinterpret_cast<Log_Transaction*>(Trans.trLogContext_gg00));
            Log_ActionObjectFile Action;

            FileDir_FileNo         ContFileno;
            FileDir_FileNo         Fileno = ObjFile.fileDirFileId_gg00();
            FileDir_ReturnCode     fd_rc;
            FileDir_Generic        fd_smart;
            fd_rc = FileDir_IDirectory::Instance(Trans.trIndex_gg00).GetFile(
                Trans.trTaskId_gg00, Fileno, fd_smart, ErrorList);
            if (FileDir_Okay != fd_rc)
                bd900MapFileDirError(Trans, fd_rc, Trans.trError_gg00, (char*)__FILE__, __LINE__ );

            Action.InitCreation (Fileno, fd_smart.GetFileType(), ObjBodyLen, ObjKeyLen, fd_smart.GetRootPage() );

            switch( fd_smart.GetFileType() )
            {
            case Data_OmsKeyFile:
                {
                    Action.AppendFileNoList ( FileNoArray );
                }
                break;
            default:
                break;
            }
            Trans.trError_gg00 = kb560ConvertWriteRedoEntryResultToBasisError(
                logtrans.WriteRedoEntry (Action, ErrorList));
        }
    }
    kb04_ErrorHandling (Trans, ThisMessType, WriteVtrace, ! BEGIN_SUBTRANS_SET);
}

/*----------------------------------------------------------------------------*/
externCpp void
kb04CreateKeyIterator (tgg00_TransContext &Trans,
                       tgg00_FileId       &ObjFile,
                       tsp00_Int4          KeyLen,
                       void               *pStartKey,
                       void               *pLowerKey,   
                       void               *pUpperKey,   
                       void              *&pKeyIterator,
                       bool                bAscendingKeyOrder)   
{
    SAPDBTRACE_ROUTINE_DEBUG("kb04CreateKeyIterator", ObjContainer_Trace, 5);
    
    Msg_List& ErrorList = Trans_Context::GetContext(Trans).ErrorList();
    const tgg00_MessType_Enum   ThisMessType  = m_select;
    const tgg00_MessType2_Enum  ThisMessType2 = mm_object;
    const bool                  WriteVtrace   = (cgg04_is_true == g01vtrace.vtrOmsGet_gg00);
    
    kb04_EnterKb (Trans,  ThisMessType, ThisMessType2, WriteVtrace, ! BEGIN_SUBTRANS_SET);
    if (Trans.trError_gg00 != e_ok) return;
    
    bd04CreateKeyIterator (Trans,  ObjFile, KeyLen,  pStartKey, 
        pLowerKey, pUpperKey, pKeyIterator, bAscendingKeyOrder, ErrorList);
    kb04_ErrorHandling (Trans, ThisMessType, WriteVtrace, ! BEGIN_SUBTRANS_SET);      
}

/*----------------------------------------------------------------------------*/

externCpp void
kb04DeleteAllObj (tgg00_TransContext  &Trans,
                  tgg00_FileId        &ObjFile,
                  tsp00_Int4          &NumDelObj,
                  tgg92_KernelOid     &ErrorOid)                  
{ 
    SAPDBTRACE_ROUTINE_DEBUG("kb04DeleteAllObj", ObjContainer_Trace, 5);
    
    Msg_List& ErrorList = Trans_Context::GetContext(Trans).ErrorList();
    const tgg00_MessType_Enum   ThisMessType  = m_delete;
    const tgg00_MessType2_Enum  ThisMessType2 = mm_object;
    const bool                  WriteVtrace   = (cgg04_is_true == g01vtrace.vtrOmsUpd_gg00);
    
    kb04_EnterKb (Trans,  ThisMessType, ThisMessType2, WriteVtrace, BEGIN_SUBTRANS_SET);
    if (Trans.trError_gg00 != e_ok) return; // timeout !

    bd04DelAllObj (Trans, ObjFile, NumDelObj, ErrorOid, ErrorList);
    kb04_ErrorHandling (Trans, ThisMessType, WriteVtrace, BEGIN_SUBTRANS_SET);      
}


/*----------------------------------------------------------------------------*/

externCpp void
kb04DeleteObj (tgg00_TransContext   &Trans,
               tgg00_FileId         &ObjFile,
               tgg92_KernelOid       Oid,
               tgg91_PageRef         WantedObjVers,
               bool                  bOmsLayerReusedObj,    // PTS 1127661
               bool                  bContVarObject)        // PTS 1124278,1127661               
{ 
    SAPDBTRACE_ROUTINE_DEBUG("kb04DeleteObj", ObjContainer_Trace, 5);
    
    Msg_List& ErrorList = Trans_Context::GetContext(Trans).ErrorList();
    const tgg00_MessType_Enum   ThisMessType  = m_delete;
    const tgg00_MessType2_Enum  ThisMessType2 = mm_object;
    const bool                  WriteVtrace   = (cgg04_is_true == g01vtrace.vtrOmsUpd_gg00);

    /* PTS 1127661 FF reused & bContVarObject new parameters */
    SAPDB_Int1 contVarObj = tbd900_VarObjBodyHeader::VarPrimUndefType;
    if  (ObjFile.fileObjFileType_gg00() == oftVarLenObjFile_egg00) {
        contVarObj = ( bContVarObject ? tbd900_VarObjBodyHeader::VarPrimContObj 
            : tbd900_VarObjBodyHeader::VarPrimHeadObj );
    }

    kb04_EnterKb (Trans,  ThisMessType, ThisMessType2, WriteVtrace, BEGIN_SUBTRANS_SET);
    if (e_ok != Trans.trError_gg00) return;

    FileDir_FileNo    Fileno = ObjFile.fileDirFileId_gg00();
    bd04DelObj (Trans, ObjFile, Oid, WantedObjVers, contVarObj, bOmsLayerReusedObj, ErrorList);
    if (e_ok == Trans.trError_gg00)
        kb67WriteDeleteObjectAfterImage (Trans, Fileno, Oid, WantedObjVers, contVarObj, bOmsLayerReusedObj, ErrorList);
    kb04_ErrorHandling (Trans, ThisMessType, WriteVtrace, BEGIN_SUBTRANS_SET);      
}


/*----------------------------------------------------------------------------*/

externCpp void
kb04DestroyKeyIterator (tgg00_TransContext &Trans,
                        void              *&pKeyIterator)
{
    SAPDBTRACE_ROUTINE_DEBUG("kb04DestroyKeyIterator", ObjContainer_Trace, 5);

    Msg_List& ErrorList = Trans_Context::GetContext(Trans).ErrorList();
    const tgg00_MessType_Enum   ThisMessType  = m_drop;
    const tgg00_MessType2_Enum  ThisMessType2 = mm_object;
    const bool                  WriteVtrace   = (cgg04_is_true == g01vtrace.vtrOmsUpd_gg00);

    kb04_EnterKb (Trans,  ThisMessType, ThisMessType2, WriteVtrace, ! BEGIN_SUBTRANS_SET);
    if (Trans.trError_gg00 != e_ok) return;

    bd04DestroyKeyIterator(Trans, pKeyIterator);
    kb04_ErrorHandling (Trans, ThisMessType, WriteVtrace, ! BEGIN_SUBTRANS_SET);    
}

/*----------------------------------------------------------------------------*/

externCpp void
kb04DropObjFile (tgg00_TransContext    &Trans,
                 tgg00_FileId          &ObjFileId)
{
    SAPDBTRACE_ROUTINE_DEBUG("kb04DropObjFile", ObjContainer_Trace, 5);
    
    Msg_List& ErrorList = Trans_Context::GetContext(Trans).ErrorList();
    const tgg00_MessType_Enum   ThisMessType  = m_drop;
    const tgg00_MessType2_Enum  ThisMessType2 = mm_object;
    const bool                  WriteVtrace   = (g01vtrace.vtrAll_gg00 || g01vtrace.vtrOmsNew_gg00 );

    kb04_EnterKb (Trans,  ThisMessType, ThisMessType2, WriteVtrace, ! BEGIN_SUBTRANS_SET);
    if (e_ok != Trans.trError_gg00) return;
        
    tgg00_LockReqMode  ReqMode;
    ReqMode.becomes(lckTabExcl_egg00);
    bd90LockObjContainer(Trans, ObjFileId.fileDirFileId_gg00(), ReqMode);

    if  (e_ok == Trans.trError_gg00)
    {
        bd04DropObjFile (Trans, ObjFileId, ErrorList);
        if  (e_ok == Trans.trError_gg00)
        {
            // PTS 1117603 UH 2002-09-12 written shorter
            Log_Transaction     &logtrans  = *(reinterpret_cast<Log_Transaction*>(Trans.trLogContext_gg00));
            Log_ActionObjectFile Action;
            FileDir_FileNo       fileno    = ObjFileId.fileDirFileId_gg00();
            Action.InitDeletion (fileno);

            Trans.trError_gg00 = kb560ConvertWriteRedoEntryResultToBasisError(
                logtrans.WriteRedoEntry (Action, ErrorList));
        }
    }
    kb04_ErrorHandling (Trans, ThisMessType, WriteVtrace, ! BEGIN_SUBTRANS_SET);
}

/*----------------------------------------------------------------------------*/

externCpp void
kb04EndConsistentTrans (tgg00_TransContext &Trans)
{ 
    SAPDBTRACE_ROUTINE_DEBUG("kb04EndConsistentTrans", ObjContainer_Trace, 5);

    Msg_List& ErrorList = Trans_Context::GetContext(Trans).ErrorList();
    const tgg00_MessType_Enum   ThisMessType  = m_end;
    const tgg00_MessType2_Enum  ThisMessType2 = mm_trans;
    const bool                  WriteVtrace   = (cgg04_is_true == g01vtrace.vtrOmsGet_gg00);
    const bool                  IgnoreCancelFlag = true; // PTS 1117708 FF 2002-SEP-09

    kb04_EnterKb (Trans,  ThisMessType, ThisMessType2, WriteVtrace, ! BEGIN_SUBTRANS_SET, IgnoreCancelFlag);
    if (e_ok != Trans.trError_gg00) return; 

    kb51DelConsistTrans (Trans.trTaskId_gg00, Trans.trConsistView_gg00 );
    Trans.trConsistView_gg00.gg90SetNil();
    kb04_ErrorHandling (Trans, ThisMessType, WriteVtrace, ! BEGIN_SUBTRANS_SET);   
}

/*----------------------------------------------------------------------------*/
// PTS 1120478
externCpp void
kb04GetContainerId (tgg00_TransContext &Trans,
                    tgg00_FileId       &ObjFile,     
                    tgg92_KernelOid     Oid)
{ 
    SAPDBTRACE_ROUTINE_DEBUG("kb04GetContainerId", ObjContainer_Trace, 5);
    
    Msg_List& ErrorList = Trans_Context::GetContext(Trans).ErrorList();
    const tgg00_MessType_Enum   ThisMessType  = m_get_object;
    const tgg00_MessType2_Enum  ThisMessType2 = mm_consistent;
    const bool                  WriteVtrace   = (cgg04_is_true == g01vtrace.vtrOmsGet_gg00);
    
    kb04_EnterKb (Trans,  ThisMessType, ThisMessType2, WriteVtrace, ! BEGIN_SUBTRANS_SET);
    if (e_ok != Trans.trError_gg00) return; // timeout !

    bd04GetContainerId (Trans, ObjFile, Oid); 
#   if COMPILEMODE_MEO00 >= SLOW_MEO00  
    t01treeid (kb, "kb04GetCntId", ObjFile);
#   endif
    kb04_ErrorHandling (Trans, ThisMessType, WriteVtrace, ! BEGIN_SUBTRANS_SET);
}

/*----------------------------------------------------------------------------*/

externCpp bool 
kb04GetNewFileId(tgg00_TransContext &Trans, FileDir_FileNo &Fileno)
{
    SAPDBTRACE_ROUTINE_DEBUG("kb04GetNewFileId", ObjContainer_Trace, 5);
    return bd04GetNewFileId(Trans, Fileno);
};

/*----------------------------------------------------------------------------*/

externCpp void 
kb04vStateFileversion (tgg00_TransContext &Trans,
                       tgg00_FileId&       versionTree)
{
    SAPDBTRACE_ROUTINE_DEBUG("kb04vStateFileversion", ObjContainer_Trace, 5);

    Msg_List& ErrorList = Trans_Context::GetContext(Trans).ErrorList();
    bd04vStateFileversion(Trans, versionTree, ErrorList);
    if ( SQLMan_Context::GetContext() != 0 )
        SQLMan_Context::GetContext()->GetErrorList() = ErrorList;
    else
    {
        RTE_Message(ErrorList);
        ErrorList.ClearMessageList();
    }
};

/*----------------------------------------------------------------------------*/

externCpp void
kb04GetObjConsistent (tgg00_TransContext &Trans,
                      tgg00_FileId       &ObjFile,     
                      tgg92_KernelOid     Oid,
                      tsp00_Int4         ObjBodySize,      //[inp=buffer size for object] 
                      void               *pObjBody,        //[inp]
                      bool                bDoLock,         //[inp] 
                      bool                bShareLock,      //[inp]
                      tsp00_Int4         &ObjBodyLen,      //[out=real object length]
                      tsp00_Int4         &LogReadAccesses, //[out] PTS 1107890 TA 13/09/2000
                      tgg91_PageRef      &ResultObjVers,
                      tgg91_TransNo      &UpdTransId)
{ 
    SAPDBTRACE_ROUTINE_DEBUG("kb04GetObjConsistent", ObjContainer_Trace, 5);
    
    Msg_List& ErrorList = Trans_Context::GetContext(Trans).ErrorList();
    const tgg00_MessType_Enum   ThisMessType  = m_get_object;
    const tgg00_MessType2_Enum  ThisMessType2 = mm_consistent;
    const bool                  WriteVtrace   = (cgg04_is_true == g01vtrace.vtrOmsGet_gg00);
    UpdTransId.gg90SetNil();
    
    tsp00_Byte *pKey   = NULL;
    tsp00_Int4  KeyLen = 0;
    
    kb04_EnterKb (Trans,  ThisMessType, ThisMessType2, WriteVtrace, ! BEGIN_SUBTRANS_SET);
    if (e_ok != Trans.trError_gg00) return; // timeout !
    
    if ((cgg_nil_transindex == Trans.trIndex_gg00)
        ||
        Trans.trConsistView_gg00.gg90IsNil() )
    {
        Kernel_OpError( csp3_bd_msg, csp3_n_obj )
            << __FILE__ << ":" << __LINE__ 
            << "trIndex OR trConsistView_gg00 is NIL";
        Trans.trError_gg00 = e_unknown_consist_vers;
    }
    else
    {
        const tsp00_Int4  ZeroHashKeyLen  = 0;
        void             *DummyHashKeyPtr = NULL;
        
        bd04GetObjConsistent (Trans, ObjFile, Oid, ObjBodySize, pObjBody,
            ZeroHashKeyLen, DummyHashKeyPtr, bDoLock, bShareLock, ObjBodyLen,
            KeyLen, &pKey, ResultObjVers, UpdTransId, LogReadAccesses, ErrorList); 
    }
    
    kb04_ErrorHandling (Trans, ThisMessType, WriteVtrace, ! BEGIN_SUBTRANS_SET);
}

/*----------------------------------------------------------------------------*/

externCpp void
kb04GetObjWithKeyConsistent (tgg00_TransContext   &Trans,
                             tgg00_FileId         &ObjFile,
                             tsp00_Int4            ObjBodySize,
                             void                 *pObjBody,
                             tsp00_Int4            KeyLen,
                             void                 *pKey,
                             bool                  bDoLock,      //[inp] 
                             bool                  bShareLock,   //[inp]
                             tsp00_Int4           &LogReadAccesses, //[out] PTS 1107890 TA 13/09/2000
                             tgg92_KernelOid      &ResultOid,
                             tgg91_PageRef        &ResultObjVers,
                             tgg91_TransNo        &UpdTransId)
{ 
    ROUTINE_DBG_MEO00 ("kb04GetObjWithKeyConsistent");
    
    Msg_List& ErrorList = Trans_Context::GetContext(Trans).ErrorList();
    const tgg00_MessType_Enum   ThisMessType  = m_get_object;
    const tgg00_MessType2_Enum  ThisMessType2 = mm_object_id;
    const bool                  WriteVtrace   = (cgg04_is_true == g01vtrace.vtrOmsGet_gg00);
    UpdTransId.gg90SetNil();
    
    kb04_EnterKb (Trans,  ThisMessType, ThisMessType2, WriteVtrace, ! BEGIN_SUBTRANS_SET);
    if (e_ok != Trans.trError_gg00) return;
    
    if ((cgg_nil_transindex == Trans.trIndex_gg00)
        ||
        Trans.trConsistView_gg00.gg90IsNil() )
    {
        Kernel_OpError( csp3_bd_msg, csp3_n_obj )
            << __FILE__ << ":" << __LINE__ 
            << "trIndex OR trConsistView_gg00 is NIL";
        Trans.trError_gg00 = e_unknown_consist_vers;
    }
    else
    {      
        bd04GetObjWithKeyConsistent (Trans, ObjFile, ObjBodySize,
            pObjBody, KeyLen, pKey, bDoLock, bShareLock, ResultOid, 
            ResultObjVers, UpdTransId, LogReadAccesses, ErrorList);
        if (e_object_not_found == Trans.trError_gg00)
            Trans.trError_gg00 = e_hash_key_not_found;
    }
    
#   if COMPILEMODE_MEO00 >= SLOW_MEO00
    if (e_ok == Trans.trError_gg00) t01moveobj (kb, pObjBody, 1, ObjBodySize);
    t01treeid (kb, "kb04GetOKyCs", ObjFile);
#   endif   

    kb04_ErrorHandling (Trans, ThisMessType, WriteVtrace, ! BEGIN_SUBTRANS_SET); 
}


/*---------------------------------------------------------------------------*/

externCpp void
kb04IsObjHistoryUsed (tgg00_TransContext &Trans,
					  tgg00_FileId       &ObjFile,
					  tgg92_KernelOid     Oid,
					  bool                ignoreVersions,		/* PTS 1130354 */
					  bool               &bHistoryIsUsed,
					  SAPDB_UInt8         &OldestTransNo)   /* PTS 1130354 */
{
	/* PTS 1106284 UH 07-04-2000 new */
    ROUTINE_DBG_MEO00 ("kb04IsObjHistoryUsed");
    if (e_ok != Trans.trError_gg00) return;

    Msg_List& ErrorList = Trans_Context::GetContext(Trans).ErrorList();
    const tgg00_MessType_Enum   ThisMessType  = m_history;
    const tgg00_MessType2_Enum  ThisMessType2 = mm_test;
    const bool                  WriteVtrace   = (cgg04_is_true == g01vtrace.vtrOmsUpd_gg00);

    kb04_EnterKb (Trans,  ThisMessType, ThisMessType2, WriteVtrace, ! BEGIN_SUBTRANS_SET);
    if (e_ok != Trans.trError_gg00) return;

    bd04IsObjHistoryUsed (Trans, ObjFile, Oid, ignoreVersions, bHistoryIsUsed, OldestTransNo, ErrorList);
#   if COMPILEMODE_MEO00 >= SLOW_MEO00
    t01treeid (kb, "kb04IsObjHis", ObjFile);
#   endif   
    kb04_ErrorHandling (Trans, ThisMessType, WriteVtrace, ! BEGIN_SUBTRANS_SET); 
}


/*----------------------------------------------------------------------------*/

externCpp void
kb04IsObjSelfLocked (tgg00_TransContext  &Trans,
                     tgg00_FileId        &ObjFile,
                     tgg92_KernelOid      Oid,
                     bool                &IsSelfLocked,
                     bool                 bShared)
                     /* PTS 1001745 UH 23-11-1998 new */
{
    ROUTINE_DBG_MEO00 ("kb04IsObjSelfLocked");
    
    Msg_List& ErrorList = Trans_Context::GetContext(Trans).ErrorList();
    const tgg00_MessType_Enum   ThisMessType  = m_lock;
    const tgg00_MessType2_Enum  ThisMessType2 = mm_test;
    const bool                  WriteVtrace   = (cgg04_is_true == g01vtrace.vtrOmsUpd_gg00);
    
    kb04_EnterKb (Trans,  ThisMessType, ThisMessType2, WriteVtrace, ! BEGIN_SUBTRANS_SET);
    if (e_ok != Trans.trError_gg00) return;
    
    bd04IsObjSelfLocked (Trans, ObjFile, Oid, bShared, IsSelfLocked, ErrorList);    
#   if COMPILEMODE_MEO00 >= SLOW_MEO00
    t01treeid (kb, "kb04IsSlfLcK", ObjFile);
#   endif   
    kb04_ErrorHandling (Trans, ThisMessType, WriteVtrace, ! BEGIN_SUBTRANS_SET); 
}

/*----------------------------------------------------------------------------*/

externCpp void
kb04LockObj (tgg00_TransContext &Trans,
             tgg00_FileId       &ObjFile,      
             tgg92_KernelOid     Oid,
             tgg91_PageRef       WantedObjVers)
{ 
    ROUTINE_DBG_MEO00 ("kb04LockObj");
    
    Msg_List& ErrorList = Trans_Context::GetContext(Trans).ErrorList();
    const tgg00_MessType_Enum   ThisMessType  = m_lock;
    const tgg00_MessType2_Enum  ThisMessType2 = mm_object;
    const bool                  WriteVtrace   = (cgg04_is_true == g01vtrace.vtrOmsUpd_gg00);
    
    kb04_EnterKb (Trans,  ThisMessType, ThisMessType2, WriteVtrace, ! BEGIN_SUBTRANS_SET);
    if (e_ok != Trans.trError_gg00) return;
    
    bd04LockObj (Trans, ObjFile, Oid, WantedObjVers, ErrorList);
#   if COMPILEMODE_MEO00 >= SLOW_MEO00
    t01treeid (kb, "kb04IsLcKObj", ObjFile);
#   endif   
    kb04_ErrorHandling (Trans, ThisMessType, WriteVtrace, ! BEGIN_SUBTRANS_SET); 
}

/*---------------------------------------------------------------------------*/

externCpp void
kb04LockUpdObj (tgg00_TransContext &Trans,
                tgg00_FileId       &ObjFile,    
                tgg92_KernelOid     Oid,
                tgg91_PageRef       WantedObjVers)
{ 
    /* PTS 1109298 UH 2001-01-26 new */
    SAPDBTRACE_ROUTINE_DEBUG("kb04LockUpdObj", ObjContainer_Trace, 5);
    
    const tgg00_MessType_Enum   ThisMessType  = m_change;
    const tgg00_MessType2_Enum  ThisMessType2 = mm_object;
    const bool                  WriteVtrace   = (cgg04_is_true == g01vtrace.vtrOmsUpd_gg00);
    
    Msg_List& ErrorList = Trans_Context::GetContext(Trans).ErrorList();
    kb04_EnterKb (Trans,  ThisMessType, ThisMessType2, WriteVtrace, BEGIN_SUBTRANS_SET);
    if (e_ok != Trans.trError_gg00) return;
    
    bd04LockUpdObj (Trans, ObjFile, Oid, WantedObjVers, ErrorList);
#   if COMPILEMODE_MEO00 >= SLOW_MEO00
    t01treeid (kb, "kb04LcKUpObj", ObjFile);
#   endif   
    kb04_ErrorHandling (Trans, ThisMessType, WriteVtrace, BEGIN_SUBTRANS_SET); 
}


/*----------------------------------------------------------------------------*/

externCpp void
kb04LockShareObj (tgg00_TransContext &Trans,
                  tgg00_FileId       &ObjFile,      
                  tgg92_KernelOid     Oid,
                  tgg91_PageRef       WantedObjVers)
{ 
    ROUTINE_DBG_MEO00 ("kb04LockShareObj");
    
    Msg_List& ErrorList = Trans_Context::GetContext(Trans).ErrorList();
    const tgg00_MessType_Enum   ThisMessType  = m_lock;
    const tgg00_MessType2_Enum  ThisMessType2 = mm_object;
    const bool                  WriteVtrace   = (cgg04_is_true == g01vtrace.vtrOmsUpd_gg00);
    
    kb04_EnterKb (Trans,  ThisMessType, ThisMessType2, WriteVtrace, ! BEGIN_SUBTRANS_SET);
    if (e_ok != Trans.trError_gg00) return;
    
    bd04LockShareObj (Trans, ObjFile, Oid, WantedObjVers, ErrorList);
#   if COMPILEMODE_MEO00 >= SLOW_MEO00
    t01treeid (kb, "kb04IsLcKObj", ObjFile);
#   endif   
    kb04_ErrorHandling (Trans, ThisMessType, WriteVtrace, ! BEGIN_SUBTRANS_SET); 
}

/*----------------------------------------------------------------------------*/

externCpp void
kb04NewObj (tgg00_TransContext  &Trans,
            tgg00_FileId        &ObjFile,
            tsp00_Int4           KeyLen,
            void                *Key,
            bool                 bContVarObject, // PTS 1124278,1127661
            tgg92_KernelOid     &ResultOid,
            tgg91_PageRef       &ResultObjVers,
            tgg91_TransNo       &UpdTransId)
{ 
    SAPDBTRACE_ROUTINE_DEBUG("kb04NewObj", ObjContainer_Trace, 5);
    
    Msg_List& ErrorList = Trans_Context::GetContext(Trans).ErrorList();
    const tgg00_MessType_Enum   ThisMessType  = m_new;
    const tgg00_MessType2_Enum  ThisMessType2 = mm_object;
    const bool                  WriteVtrace   = (cgg04_is_true == g01vtrace.vtrOmsNew_gg00);

    /* PTS 1127661 FF bContVarObject new parameter */
    SAPDB_Int1 contVarObj = tbd900_VarObjBodyHeader::VarPrimUndefType;
    if  (ObjFile.fileObjFileType_gg00() == oftVarLenObjFile_egg00) {
        contVarObj = ( bContVarObject ? tbd900_VarObjBodyHeader::VarPrimContObj 
            : tbd900_VarObjBodyHeader::VarPrimHeadObj );
    }

    kb04_EnterKb (Trans,  ThisMessType, ThisMessType2, WriteVtrace, BEGIN_SUBTRANS_SET);
    if (e_ok != Trans.trError_gg00) return; 
    
    // PTS 1117603 UH 2002-09-12 The AfterImageHandling was rewritten
    Log_Transaction &logtrans = *(reinterpret_cast<Log_Transaction*>(Trans.trLogContext_gg00));

    bd04NewObj (Trans, ObjFile, KeyLen, Key, contVarObj, ResultOid, ResultObjVers, ErrorList);
    if ( e_ok == Trans.trError_gg00 )
    {

        Log_ActionObject newObjectAction;
    
        if ( ! newObjectAction.InitNew ( ObjFile, ResultOid, ResultObjVers,
                                         KeyLen, reinterpret_cast<SAPDB_Byte*>(Key),
                                         contVarObj, ErrorList ) )
        {
            Trans.trError_gg00 = e_redo_log_write_error;
        }
        else
            Trans.trError_gg00 = kb560ConvertWriteRedoEntryResultToBasisError(
                logtrans.WriteRedoEntry (newObjectAction, ErrorList));
    }

    if ( e_ok == Trans.trError_gg00
         &&
         KeyLen > 0 )
    {
        tgg91_PageRef   ReUsedObjVers;
        tgg92_KernelOid ReUsedOId;
        
        // PTS 1116896 mb 2002-07-25 // PTS 1117603 UH 2002-09-12 removed again
        
        bd04NewObjKey (Trans, ObjFile, KeyLen, Key, ResultOid, ReUsedOId, ReUsedObjVers, ErrorList);      
        if ( e_ok == Trans.trError_gg00 )
        {
            if ( ! ReUsedOId.gg92IsNilPno() )
            {
                // An object could be reused for the given key,
                // so we have to delete the newly created object.
                // The Key is not deleted, because the key's oid doesn't match.

                const bool bOmsLayerReusedObj = false;      // PTS 1127661
                bd04DelObj (Trans, ObjFile, ResultOid, ResultObjVers, contVarObj, bOmsLayerReusedObj, ErrorList);

                FileDir_FileNo  Fileno = ObjFile.fileDirFileId_gg00();

                if (e_ok == Trans.trError_gg00)
                    kb67WriteDeleteObjectAfterImage ( Trans,
                                                      Fileno,
                                                      ResultOid, 
                                                      ResultObjVers, 
                                                      contVarObj,
                                                      bOmsLayerReusedObj,
                                                      ErrorList);   
                
                ResultOid     = ReUsedOId;
                ResultObjVers = ReUsedObjVers;
                
                if ( e_ok == Trans.trError_gg00 )
                {
                    Log_ActionObject reusedObjectAction;
                
                    if ( ! reusedObjectAction.InitNew ( ObjFile, ResultOid, ResultObjVers,
                                                        KeyLen, reinterpret_cast<SAPDB_Byte*>(Key),
                                                        contVarObj, ErrorList ) )
                    {
                        Trans.trError_gg00 = e_redo_log_write_error;
                    }
                    else
                        Trans.trError_gg00 = kb560ConvertWriteRedoEntryResultToBasisError(
                            logtrans.WriteRedoEntry (reusedObjectAction, ErrorList));
                }
            }
        }
        else
        {
            // PTS 1132118 UH 2004-10-25 ResultOid is always assigned
            /* return OId of object which already belongs to the   */
            /* current key, PTS 1105726 Alexander Kley 29/02/2000  */     

            ResultOid     = ReUsedOId;
            ResultObjVers = ReUsedObjVers;

            // PTS 1116896 mb 2002-07-25 write rollback-information in order to perform
            // a rollback of the newObj at restart-time
            // PTS 1117603 UH 2002-09-12 removed again
        }
    }

#   if COMPILEMODE_MEO00 >= SLOW_MEO00
    t01treeid (kb, "kb04NewObj  ", ObjFile);
#   endif   

    if (e_ok == Trans.trError_gg00)
        UpdTransId = Trans.trWriteTransId_gg00;
    else
        UpdTransId.gg90SetNil();

    kb04_ErrorHandling (Trans, ThisMessType, WriteVtrace, BEGIN_SUBTRANS_SET);
}

/*----------------------------------------------------------------------------*/

//externCpp void
//kb04NextObjConsistent (tgg00_TransContext &Trans,
//                       tgg00_FileId       &ObjFile,    
//                       tgg92_KernelOid    &CurrOid,
//                       bool                bWithObject,
//                       tsp00_Int4          ObjBodySize,   //[inp=buffer size for object]
//                       void               *pObjBody,
//                       tsp00_Int4&         LogReadAccesses,// PTS 1107890 TA 13/09/2000
//                       tgg91_PageRef      &ResultObjVers)
//{ 
//    SAPDBTRACE_ROUTINE_DEBUG("kb04NextObjConsistent", ObjContainer_Trace, 5);
//    
//    Msg_List& ErrorList = Trans_Context::GetContext(Trans).ErrorList();
//    const tgg00_MessType_Enum   ThisMessType  = m_get_object;
//    const tgg00_MessType2_Enum  ThisMessType2 = mm_qual;
//    const bool                  WriteVtrace   = (cgg04_is_true == g01vtrace.vtrOmsGet_gg00);
//    tsp00_Int4                  ObjBodyLen;
//
//    kb04_EnterKb (Trans,  ThisMessType, ThisMessType2, WriteVtrace, ! BEGIN_SUBTRANS_SET);
//    if (e_ok != Trans.trError_gg00) return;
//
//    bd04NextObjConsistent (Trans, ObjFile, CurrOid, bWithObject,
//        ObjBodySize, pObjBody, ObjBodyLen, ResultObjVers, LogReadAccesses, ErrorList); 
//    
//#   if COMPILEMODE_MEO00 >= SLOW_MEO00
//    if (e_ok == Trans.trError_gg00) t01moveobj (kb, pObjBody, 1, ObjBodySize);
//    t01treeid (kb, "kb04NxtObjCs", ObjFile);
//#   endif   
//    
//    kb04_ErrorHandling (Trans, ThisMessType, WriteVtrace, ! BEGIN_SUBTRANS_SET); 
//}

/*----------------------------------------------------------------------------*/

externCpp void
kb04NextOidsConsistent (tgg00_TransContext    &Trans,
                       tgg00_FileId           &ObjFile,  
                       bool                    bOnlyHdrVarObj,
                       const tgg92_KernelOid  &CurrOid,
                       tsp00_Int4             &noOfOid,
                       tgg92_KernelOid        *pOid,
                       tgg91_PageRef          *pObjVers,
                       tsp00_Int4             &LogReadAccesses)
{ 
    SAPDBTRACE_ROUTINE_DEBUG("kb04NextOidsConsistent", ObjContainer_Trace, 5);
    
    Msg_List& ErrorList = Trans_Context::GetContext(Trans).ErrorList();
    const tgg00_MessType_Enum   ThisMessType  = m_get_object;
    const tgg00_MessType2_Enum  ThisMessType2 = mm_qual;
    const bool                  WriteVtrace   = (cgg04_is_true == g01vtrace.vtrOmsGet_gg00);

    kb04_EnterKb (Trans,  ThisMessType, ThisMessType2, WriteVtrace, ! BEGIN_SUBTRANS_SET);
    if (e_ok != Trans.trError_gg00) return;

    bd04NextOidsConsistent (Trans, ObjFile, bOnlyHdrVarObj, CurrOid, noOfOid, pOid, pObjVers, 
        LogReadAccesses, ErrorList); 
    
#   if COMPILEMODE_MEO00 >= SLOW_MEO00
    t01treeid (kb, "kb04NxtOidCs", ObjFile);
#   endif   
    kb04_ErrorHandling (Trans, ThisMessType, WriteVtrace, ! BEGIN_SUBTRANS_SET); 
}

/*----------------------------------------------------------------------------*/
externCpp void
kb04NextObjFromKeyIterator (tgg00_TransContext &Trans,
                            void               *pUntypedKeyIterator,    
                            bool                bInclusive,  
                            tsp00_Int4          objBodySize,
                            tsp00_Int4          KeyLen,      
                            void               *pRestartKey, 
                            tsp00_Int4         &noOfOid,
                            tgg92_KernelOid    *pResultOId, 
                            tgg91_PageRef      *pObjVers,
                            tsp00_MoveObj      *pBody,
                            tsp00_Int4         &LogReadAccesses)
{ 
    SAPDBTRACE_ROUTINE_DEBUG("kb04NextObjFromKeyIterator", ObjContainer_Trace, 5);
    
    Msg_List& ErrorList = Trans_Context::GetContext(Trans).ErrorList();
    const tgg00_MessType_Enum   ThisMessType  = m_get_object;
    const tgg00_MessType2_Enum  ThisMessType2 = mm_qual;
    const bool                  WriteVtrace   = (cgg04_is_true == g01vtrace.vtrOmsGet_gg00);

    kb04_EnterKb (Trans,  ThisMessType, ThisMessType2, WriteVtrace, ! BEGIN_SUBTRANS_SET);
    if (e_ok != Trans.trError_gg00) return;
    
    bd04NextObjFromKeyIterator (Trans, pUntypedKeyIterator, bInclusive, objBodySize, KeyLen, pRestartKey, 
                                noOfOid, pResultOId, pObjVers, pBody, LogReadAccesses, ErrorList);
    
    kb04_ErrorHandling (Trans, ThisMessType, WriteVtrace, ! BEGIN_SUBTRANS_SET); 
}

/*----------------------------------------------------------------------------*/
externCpp void
kb04PrevObjFromKeyIterator (tgg00_TransContext &Trans,
                            void               *pUntypedKeyIterator,    
                            bool                bInclusive,  
                            tsp00_Int4          objBodySize,
                            tsp00_Int4          KeyLen,      
                            void               *pRestartKey, 
                            tsp00_Int4         &noOfOid,
                            tgg92_KernelOid    *pResultOId, 
                            tgg91_PageRef      *pObjVers,
                            tsp00_MoveObj      *pBody,
                            tsp00_Int4         &LogReadAccesses)
{ 
    SAPDBTRACE_ROUTINE_DEBUG("kb04PrevObjFromKeyIterator", ObjContainer_Trace, 5);
    
    Msg_List& ErrorList = Trans_Context::GetContext(Trans).ErrorList();
    const tgg00_MessType_Enum   ThisMessType  = m_get_object;
    const tgg00_MessType2_Enum  ThisMessType2 = mm_qual;
    const bool                  WriteVtrace   = (cgg04_is_true == g01vtrace.vtrOmsGet_gg00);

    kb04_EnterKb (Trans,  ThisMessType, ThisMessType2, WriteVtrace, ! BEGIN_SUBTRANS_SET);
    if (e_ok != Trans.trError_gg00) return;
    
    bd04PrevObjFromKeyIterator (Trans, pUntypedKeyIterator, bInclusive, objBodySize, KeyLen, pRestartKey, 
                                noOfOid, pResultOId, pObjVers, pBody, LogReadAccesses, ErrorList);

    
    kb04_ErrorHandling (Trans, ThisMessType, WriteVtrace, ! BEGIN_SUBTRANS_SET); 
}

/*---------------------------------------------------------------------------*/

externCpp void
kb04StartConsistentTrans (tgg00_TransContext &Trans,
						  bool                isOmsVersion) /* PTS 1130354 */
{ 
    SAPDBTRACE_ROUTINE_DEBUG("kb04StartConsistentTrans", ObjContainer_Trace, 5);
    
    Msg_List& ErrorList = Trans_Context::GetContext(Trans).ErrorList();
    const tgg00_MessType_Enum   ThisMessType  = m_begin;
    const tgg00_MessType2_Enum  ThisMessType2 = mm_trans;
    const bool                  WriteVtrace   = (cgg04_is_true == g01vtrace.vtrOmsGet_gg00);
    
    kb04_EnterKb (Trans,  ThisMessType, ThisMessType2, WriteVtrace, ! BEGIN_SUBTRANS_SET);
    if (e_ok != Trans.trError_gg00) return;
    
	kb51StartConsistTrans (Trans, (isOmsVersion ? 1 : 0));
    kb04_ErrorHandling (Trans, ThisMessType, WriteVtrace, ! BEGIN_SUBTRANS_SET); 
}


/*----------------------------------------------------------------------------*/

externCpp void
kb04UnlockObj (tgg00_TransContext   &Trans,
               tgg00_FileId         &ObjFile,      
               tgg92_KernelOid       Oid)
{ 
    SAPDBTRACE_ROUTINE_DEBUG("kb04UnlockObj", ObjContainer_Trace, 5);
    
    Msg_List& ErrorList = Trans_Context::GetContext(Trans).ErrorList();
    const tgg00_MessType_Enum   ThisMessType  = m_unlock;
    const tgg00_MessType2_Enum  ThisMessType2 = mm_object;
    const bool                  WriteVtrace   = (cgg04_is_true == g01vtrace.vtrOmsUpd_gg00);
    
    kb04_EnterKb (Trans,  ThisMessType, ThisMessType2, WriteVtrace, ! BEGIN_SUBTRANS_SET);
    if (e_ok != Trans.trError_gg00) return;
    
    bd04UnlockObj (Trans, ObjFile, Oid, false /*bSharedLock*/);

    kb04_ErrorHandling (Trans, ThisMessType, WriteVtrace, ! BEGIN_SUBTRANS_SET); 
}

/*----------------------------------------------------------------------------*/

externCpp void
kb04UnlockSharedObj (tgg00_TransContext   &Trans,
                     tgg00_FileId         &ObjFile,      
                     tgg92_KernelOid       Oid)
{ 
    SAPDBTRACE_ROUTINE_DEBUG("kb04UnlockSharedObj", ObjContainer_Trace, 5);
    
    Msg_List& ErrorList = Trans_Context::GetContext(Trans).ErrorList();
    const tgg00_MessType_Enum   ThisMessType  = m_unlock;
    const tgg00_MessType2_Enum  ThisMessType2 = mm_object;
    const bool                  WriteVtrace   = (cgg04_is_true == g01vtrace.vtrOmsUpd_gg00);
    
    kb04_EnterKb (Trans,  ThisMessType, ThisMessType2, WriteVtrace, ! BEGIN_SUBTRANS_SET);
    if (e_ok != Trans.trError_gg00) return;
    
    bd04UnlockObj (Trans, ObjFile, Oid, true /*bSharedLock*/);

    kb04_ErrorHandling (Trans, ThisMessType, WriteVtrace, ! BEGIN_SUBTRANS_SET); 
}

/*----------------------------------------------------------------------------*/

externCpp void
kb04UpdateObj (tgg00_TransContext  &Trans,
               tgg00_FileId        &ObjFile,
               tgg92_KernelOid      Oid,
               tgg91_PageRef       &ObjVers,
               tsp00_Int4           NewBodySize,
               void                *pNewBody,
               bool                 bContVarObject,
               bool                 bOmsLayerReusedObj)  // PTS 1127661
{ 
    SAPDBTRACE_ROUTINE_DEBUG("kb04UpdateObj", ObjContainer_Trace, 5);
    
    Msg_List& ErrorList = Trans_Context::GetContext(Trans).ErrorList();
    const tgg00_MessType_Enum   ThisMessType  = m_update;
    const tgg00_MessType2_Enum  ThisMessType2 = mm_object;
    const bool                  WriteVtrace   = (cgg04_is_true == g01vtrace.vtrOmsUpd_gg00);

    /* PTS 1127661 FF reused & bContVarObject new parameters */
    SAPDB_Int1 contVarObj = tbd900_VarObjBodyHeader::VarPrimUndefType;
    if  (ObjFile.fileObjFileType_gg00() == oftVarLenObjFile_egg00) {
        contVarObj = ( bContVarObject ? tbd900_VarObjBodyHeader::VarPrimContObj 
            : tbd900_VarObjBodyHeader::VarPrimHeadObj );
    }
    
    kb04_EnterKb (Trans,  ThisMessType, ThisMessType2, WriteVtrace, BEGIN_SUBTRANS_SET);
    if (e_ok != Trans.trError_gg00) return; // timeout !
    
    tgg91_PageRef PrevObjVers = ObjVers;
    
    bd04UpdateObj (Trans, ObjFile, Oid, ObjVers, NewBodySize, pNewBody, contVarObj, 
        bOmsLayerReusedObj, ErrorList);
    
    if (e_ok == Trans.trError_gg00)
    {
        // PTS 1117603 UH 2002-09-12 written shorter
        Log_Transaction &logtrans      = *(reinterpret_cast<Log_Transaction*>(Trans.trLogContext_gg00));
        Log_ActionObject Action;

        if ( ! Action.InitInsUpd (Log_UpdateObject,
                           ObjFile, Oid, PrevObjVers,
                           0, NewBodySize, REINTERPRET_CAST(SAPDB_Byte*,pNewBody),
                           contVarObj, bOmsLayerReusedObj,   // PTS 1127661
                           ErrorList ) )
            Trans.trError_gg00 = e_redo_log_write_error;
        else
            Trans.trError_gg00 = kb560ConvertWriteRedoEntryResultToBasisError(
                logtrans.WriteRedoEntry (Action, ErrorList));
    }

    kb04_ErrorHandling (Trans, ThisMessType, WriteVtrace, BEGIN_SUBTRANS_SET); 
}
/*----------------------------------------------------------------------------*/

externCpp void
kb04SetTransactionComment (tgg00_TransContext &Trans,
                            tsp00_Int2         commentLength,
                            SAPDB_Char*        pComment)
{
  k512SetTransactionComment(Trans.trTaskId_gg00, Trans.trIndex_gg00,
    commentLength, pComment);
}; 

/*----------------------------------------------------------------------------*/

externCpp void
kb04LockContainer(tgg00_TransContext &Trans,
                  const tgg00_FileId &fileId)
{
    const  bool cDoShareLock  = true;
    Msg_List& ErrorList = Trans_Context::GetContext(Trans).ErrorList();
    const FileDir_FileNo FileNo = fileId.fileDirFileId_gg00(); 
    FileDir_Oms     fd_smart;
    bd90GetFile( Trans, fd_smart,  FileNo, cDoShareLock, __LINE__, ErrorList);
    if ( ! ErrorList.IsEmpty() )
    {
        RTE_Message(ErrorList);
        ErrorList.ClearMessageList();
    }
    Trans.trError_gg00 = e_ok;
}

