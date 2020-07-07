/*!***************************************************************************

  module      : vkb67.cpp
  author      : JuergenA, UweH
  responsible : UweH
  special area: KB_object_handling_interface
  last changed: 2000-12-20
  Copyright (c) 2000-2005 SAP AG
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


#include "gsp03.h"
#include "gsp03_2.h"

#include "heo55k.h"
#include "ggg01.h"
#include "hgg01_1.h"
#include "hgg08.h"
#include "hkb50.h"
#include "hkb51.h"
#include "hsp77.h"    // sp77sprintf

#if COMPILEMODE_MEO00 >= SLOW_MEO00 
#include "hta99.h"  
#include "hta01.h"  
#include "hta01_2.h"
#endif

#include "KernelCommon/Kernel_IAdminInfo.hpp"
#include "KernelCommon/Kernel_OpMsg.hpp"           
#include "KernelCommon/Kernel_VTrace.hpp"         
#include "RunTime/RTE_Message.hpp"
#include "Logging/Log_Transaction.hpp"
#include "Logging/Log_BeforeImage.hpp"
#include "Logging/Log_ActionObject.hpp"
#include "Logging/Log_ActionObjectFile.hpp"
#include "Logging/Log_History.hpp"
#include "SQLManager/SQLMan_Context.hpp"

#include "hkb560.h" // kb560ConvertWriteRedoEntryResultToBasisError
#include "hkb67.h"
#include "hkb52.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

// #define __CHECK_TIME       true

#define ALIGN_LOGENTRY_END true
#define INS_OBJ_MISSING    "InsObjLog missing       "
#define NIL_OBJ_FILLCHAR   0
#define WRONG_LOG_OID      "GetConsist:OID <> LogOID"
#define REDO_ENTRY_SEQ_LABEL MAX_INT2_SP00

#ifdef __CHECK_TIME
externC void vclock (tsp00_Int4 *Sec, tsp00_Int4 *MicroSec);
#endif

#define CHANGE_ENTRY true

/*===========================================================================*
 *   LOCAL FUNCTIONS (PROTOTYPES)                                            *
 *===========================================================================*/

/*---------------------------------------------------------------------------*/

inline void kb67_PrintError (const char *s /*tsp00_C40*/)
{
    if (g01vtrace.vtrKbLock_gg00)
        g01optextmsg (sp3p_syscon, sp3m_info, kbMsg_csp03, csp3_n_history, s);
}

/*---------------------------------------------------------------------------*/
inline tgg00_MessType_Enum kb67_OldAction (Log_ActionType actionType)
{
    switch (actionType)
    {
    case Log_NewObject:    return m_new;
    case Log_InsertObject: return m_insert;
    case Log_UpdateObject: return m_update;
    case Log_DeleteObject: return m_delete;
    case Log_LockObject:   return m_lock;
    default: return m_nil;
    }
}

/*===========================================================================*
 *  GLOBAL FUNCTIONS                                                         *
 *===========================================================================*/

externCpp void
kb67CreateContainer (tgg00_TransContext   &Trans,
                     tgg00_FileId         &FileId,
                     tbd_node_ptrs        &Nptrs,
                     Msg_List &ErrorList)
{
    ROUTINE_DBG_MSP00 ("kb67CreateContainer");

    if (e_ok != Trans.trError_gg00) return;
    FileDir_FileNo       Fileno      = FileId.fileDirFileId_gg00();
    Data_FileType        Filetype    = Data_OmsFixedFile;
    if ( pt2VarObject_egg00 == Nptrs.np_ptr()->nd_pt2() )
        Filetype = Data_OmsVarFile;
    if ( FileId.fileNumObjKeyFiles_gg00() > 0 )
        Filetype = Data_OmsKeyFile;
    SAPDB_UInt           ObjBodySize = Nptrs.np_ptr()->nd_obj_frame_len() - sizeof(tgg92_ObjHeader);
    SAPDB_UInt           ObjKeyLen   = Nptrs.np_ptr()->nd_obj_key_len();
    Data_PageNo          RootPage    = Nptrs.np_ptr()->nd_id();
    
    Log_Transaction&     logtrans = *(reinterpret_cast<Log_Transaction*>(Trans.trLogContext_gg00));
    Log_BeforeImage      image;
    Log_ActionObjectFile action;
    Data_PageNo          pageno;
    Data_PageOffset      offset;

    action.InitCreation (Fileno, Filetype, ObjBodySize, ObjKeyLen, RootPage);
    switch ( logtrans.WriteUndoEntry ( image, action, pageno, offset, ErrorList) )
    {
        case Log_UndoFile::writeBeforeImageOk:
            break;
        case Log_UndoFile::writeBeforeImageError:
            Trans.trError_gg00 = e_undo_log_write_error;
            break;
        case Log_UndoFile::writeBeforeImageNoSpaceAvailable:
            Trans.trError_gg00 = e_work_rolled_back;
            break;
        default:
            Trans.trError_gg00 = e_undo_log_write_error;
            break;
    }
}

/*---------------------------------------------------------------------------*/

externCpp void
kb67DropContainer (tgg00_TransContext   &Trans,
                   FileDir_FileNo       &fileno,
                   Msg_List &ErrorList)
{
    ROUTINE_DBG_MSP00 ("kb67DropContainer");

    if (e_ok != Trans.trError_gg00) return;
    
    Log_Transaction& logtrans = *(reinterpret_cast<Log_Transaction*>(Trans.trLogContext_gg00));
    
    Log_BeforeImage      image;
    Log_ActionObjectFile action;
    Data_PageNo          pageno;
    Data_PageOffset      offset;

    action.InitDeletion (fileno);
    
    switch ( logtrans.WriteUndoEntry ( image, action, pageno, offset, ErrorList) )
    {
        case Log_UndoFile::writeBeforeImageOk:
            break;
        case Log_UndoFile::writeBeforeImageError:
            Trans.trError_gg00 = e_undo_log_write_error;
            break;
        case Log_UndoFile::writeBeforeImageNoSpaceAvailable:
            Trans.trError_gg00 = e_work_rolled_back;
            break;
        default:
            Trans.trError_gg00 = e_undo_log_write_error;
            break;
    }
};

/*---------------------------------------------------------------------------*/

externCpp void
kb67DelUpdObj (tgg00_TransContext   &Trans,
               tgg00_MessType_Enum   WantedMessType,
               tgg00_FileId         &ObjFile,
               tgg92_KernelOid       Oid,
               tsp00_Int4            ObjKeySize,
               tgg91_PageRef         WantedObjVers,
               tgg92_ObjHeader      &ObjHeader,     
               tsp00_Int4            ObjBodySize,
               void                 *pObjBody,
               SAPDB_Int1            ContVarObject,       
               bool                  bOmsLayerReusedObj,
               Msg_List &ErrorList)
{ 
    ROUTINE_DBG_MSP00 ("kb67DelUpdObj");
    
    if (e_ok != Trans.trError_gg00) return;
    
    tgg00_MessType      MessType;
    tgg00_ObjState     &ObjState  = ObjHeader.ohdState_gg92;
    tgg00_ObjTransInfo &TransInfo = ObjHeader.ohdTransInfo_gg92;
   
    if (m_update == WantedMessType)
    {
        if (obsOccupied_egg00 == ObjState)
            MessType.becomes (m_update);
        else
            MessType.becomes (m_insert);   
    }
    else
        MessType.becomes (WantedMessType);

    // PTS 1113342 UH 2002-01-10 begin
    if ( Kernel_IAdminInfo::Instance().KernelStateIsRedoLogProcessing() )
        TransInfo.otrLockTransId_gg00 = Trans.trWriteTransId_gg00;
    else
        kb50CheckObjSelfLocked (Trans, Oid, TransInfo, 0/*false bIsShareLock*/);
    // PTS 1113342 UH 2002-01-10 end
    
    if (e_ok != Trans.trError_gg00) return;
    
    if ((m_insert != MessType) && (WantedObjVers != TransInfo.otrBeforeRef_gg00))
    {
        Trans.trError_gg00 = e_object_dirty;

        if (g01vtrace.vtrKbLock_gg00)
        {
            kb50XxxObjTrace ( Trans, "  kb67DelUpd ",
                              Oid,
                              TransInfo.otrUpdTransId_gg00,
                              TransInfo.otrBeforeRef_gg00,
                              ObjState );
        }
        return;
    }

    Log_BeforeImage BeforeImage;
    Log_ActionType  actionType;

    BeforeImage.SetTransInfoAndObjState ( TransInfo, ObjState );

    
    switch ( MessType )
    {
    case m_delete:
        actionType = Log_DeleteObject;
        break;
    case m_drop: // PTS 1113342 UH 2002-01-10
        // auxiliary entry for garbage collector only
        actionType = Log_DeleteObject;
        BeforeImage.SetRollbacked(false);
        break;
    case m_insert:
        actionType = Log_InsertObject;
        break;
    case m_update:
        actionType = Log_UpdateObject;
        break;
    case m_lock:
        actionType = Log_LockObject;
        break;
    default:
        Trans.trError_gg00 = e_undo_log_write_error;
        return;
    }
    Log_ActionObject RollbackAction;

    if ( ! RollbackAction.InitInsUpd ( actionType,
                                ObjFile,
                                Oid,
                                TransInfo.otrBeforeRef_gg00,
                                ObjKeySize,
                                ObjBodySize,
                                REINTERPRET_CAST (SAPDB_Byte*,pObjBody),
                                ContVarObject, bOmsLayerReusedObj,   /* PTS 1127661 FF */
                                ErrorList ) )
    {
        Trans.trError_gg00 = e_undo_log_write_error;
        return;
    }

    Log_Transaction &TransContext = *reinterpret_cast<Log_Transaction*>(Trans.trLogContext_gg00);

    Data_PageNo          pageno;
    Data_PageOffset      offset;

    switch ( TransContext.WriteUndoEntry ( BeforeImage, RollbackAction, pageno, offset, ErrorList) )
    {
        case Log_UndoFile::writeBeforeImageOk:
            break;
        case Log_UndoFile::writeBeforeImageError:
            Trans.trError_gg00 = e_undo_log_write_error;
            break;
        case Log_UndoFile::writeBeforeImageNoSpaceAvailable:
            Trans.trError_gg00 = e_work_rolled_back;
            break;
        default:
            Trans.trError_gg00 = e_undo_log_write_error;
            break;
    }

    if( Trans.trError_gg00 != e_ok )
        return;

    TransInfo.otrBeforeRef_gg00.gg91BuildRef (pageno, offset);
    TransInfo.otrUpdTransId_gg00 = Trans.trWriteTransId_gg00;
}

/*---------------------------------------------------------------------------*/

externCpp void
kb67DropObjHistory (tgg00_TransContext    &trans,
                    const tgg92_ObjHeader &objHeader,
                    const tgg92_KernelOid &oid,
                    Msg_List  &ErrorList)
{
    const tgg91_PageRef             &pageref = objHeader.ohdTransInfo_gg92.otrBeforeRef_gg00;
    const Log_History::DeleteResult  result  = Log_History::DeleteBeforeImage (trans,oid,pageref,ErrorList);

    switch ( result )
    {
    case Log_History::delIllegalPageref :
    case Log_History::delInitError :
    case Log_History::delReadError :
    case Log_History::delAccessingUndoEntryFailed :
    case Log_History::delActionMustBeDelete :
    case Log_History::delWrongEntry :
        if ( trans.trError_gg00 != e_corrupted_datapage_from_io ) // PTS 1117126 UH 2002-08-07
            trans.trError_gg00 = e_obj_history_not_found;
#       ifdef SAPDB_QUICK
        RTE_Crash(ErrorList);
#       endif
        break;
    case Log_History::delImageNotFound :
    case Log_History::delAlreadyDeleted :
        trans.trError_gg00 = e_obj_history_not_found;
        break;
    case Log_History::delSuccessfull :
        trans.trError_gg00 = e_ok;
        break;
    default :
        ErrorList = ErrorList + Log_Exception(__CONTEXT__,LOG_UNKNOWN_RESULT,
            "Log_History::DeleteBeforeImage()",
            SAPDB_ToString(result));
#       ifdef SAPDB_QUICK
        RTE_Crash(ErrorList);
#       endif
    }
}

/*---------------------------------------------------------------------------*/

externCpp void
kb67UpdObj (tgg00_TransContext       &Trans,
            tgg00_FileId             &ObjFile,
            tgg92_KernelOid           OId,
            tsp00_Int4                ObjKeySize,
            tgg91_PageRef             ObjVers,
            tgg92_ObjHeader          &ObjHeader,     
            tsp00_Int4                ObjBodySize,
            void                     *pObjBody,
            const Log_EntrySequence  &RedoEntrySequence,
            const tgg91_TransNo      &RedoUpdTransNo,
            SAPDB_Int1                contVarObject,      
            bool                      bOmsLayerReusedObj,
            Msg_List     &ErrorList)
{
    ROUTINE_DBG_MEO00 ("kb67UpdObj");
    
    /* check if this update belongs to an online update or a redo of an update */
    if (RedoEntrySequence.IsInvalid())
    {
        /*                 online update                     */
        /*  lock object and  creates an entry in the history.*/
        /*  if the object is locked already an error         */
        /*  code = e_lock_collision is returned.             */
        /*  if the input sequence number ObjVers  is too old */
        /*  an error  code is sent                           */
        SAPDBERR_ASSERT_STATE (! Kernel_IAdminInfo::Instance().KernelStateIsRedoLogProcessing() );
               
        kb67DelUpdObj (Trans, m_update, ObjFile, OId, ObjKeySize, ObjVers,
                       ObjHeader, ObjBodySize, pObjBody,
                       contVarObject, bOmsLayerReusedObj, ErrorList );                           
    }
    else
    {
         /*                  redo update                         */
         /* for a redo no history entry needs to be written, but */
         /* the transinfo in the object header has to be updated */     
          SAPDBERR_ASSERT_STATE ( Kernel_IAdminInfo::Instance().KernelStateIsRedoLogProcessing() );
           
         if (!kb67RollforwardObj(ObjHeader, RedoUpdTransNo, RedoEntrySequence, ObjVers))
                           Trans.trError_gg00 = e_wrong_object_version;
    }
}

/*-----------------------------------------------------------------------------------*/

externCpp void
kb67GetObjConsistent(tgg00_TransContext   &Trans,
                     tgg92_KernelOid       Oid,
                     tgg00_ObjState        ObjState,
                     tgg00_ObjTransInfo   &ObjTransInfo,
                     bool                  bWithObj,
                     tsp00_Int4            KeyLen,
                     tsp00_Int4           &ObjBodyLen,
                     tsp00_Int4            ObjBodySize,
                     void                 *pObjBody,
                     tgg00_ObjFrameVers   &FrameVers,
                     tgg91_PageRef        &ResultObjVers,
                     tgg91_PageRef        &ObjHistRef,        
                     tgg91_TransNo        &HistUpdTransId,         // PTS 1137480
                     tsp00_Int4           &LogReadAccesses,
                     Msg_List &ErrorList)
{ 
    ROUTINE_DBG_MEO00 ("kb67GetObjConsistent");

    tsp00_Bool bConsistent;  
    tsp00_Bool bCancelled;          /* PTS 1120151 FF 2003-JAN-29 */

    ObjHistRef.gg91SetNilRef();

    if (e_ok != Trans.trError_gg00) return;  

    kb50ObjConsistentCheck (Trans, ObjTransInfo.otrUpdTransId_gg00, bConsistent, bCancelled);
    if ((e_ok == Trans.trError_gg00) && bCancelled) /* PTS 1120151 FF 2003-JAN-29 */
    {
        if (g01vtrace.vtrKbLock_gg00 && 
            (e_ok == Trans.trError_gg00) && bCancelled) /* PTS 1120151 FF 2003-JAN-29 */
        {
            Kernel_VTrace     vTrace;
            vTrace << "cancelled ConsistView:";
            for (int i=0; i < sizeof(Trans.trConsistView_gg00); i++)
                vTrace << ToStr((SAPDB_UInt1)Trans.trConsistView_gg00[i], _T_h);
            vTrace << " found";
        }
        Trans.trError_gg00 = e_consview_cancelled;
    }
    if (e_ok != Trans.trError_gg00) return;  

    if (bConsistent)
    {
        LogReadAccesses = 0; // PTS 1107819 AK 12/09/2000
        ResultObjVers   = ObjTransInfo.otrBeforeRef_gg00;

        if ( ObjState == obsFreeAfterEot_egg00 )
        {
            // object not existent
            Trans.trError_gg00 = e_object_not_found;

            if (g01vtrace.vtrKbLock_gg00)
            {
                kb50XxxObjTrace (Trans, "  kb67GetCns",
                    Oid,
                    ObjTransInfo.otrUpdTransId_gg00,
                    ResultObjVers,
                    ObjState);
            }
            return;
        } 
        HistUpdTransId = ObjTransInfo.otrUpdTransId_gg00;
    }
    else
    {   
        kb67GetBeforeImage ( Trans,
            Oid,
            ObjTransInfo.otrBeforeRef_gg00,
            bWithObj,
            KeyLen,
            ObjBodyLen,
            ObjBodySize,
            pObjBody,
            FrameVers,
            ResultObjVers,
            ObjHistRef,  
            HistUpdTransId,      // PTS 1137480
            LogReadAccesses,
            ErrorList); 

        if (g01vtrace.vtrKbLock_gg00 && 
            ( e_obj_history_not_found == Trans.trError_gg00))
        {
            Kernel_VTrace() 
                << "kb67GetBeforeImage delivered e_obj_history_not_found";
        }
    }
}

/*---------------------------------------------------------------------------*/
/* PTS 1135542 FF used within vdb910.cpp to check if page should be inserted */
/*                to freepage-chain                                          */
externCpp bool
kb67IsTransActive(tgg92_ObjHeader &ObjHeader)
{
    bool bRC;
    tkb05_TransEntryPtr ThisTrans;
    switch (ObjHeader.ohdState_gg92)
    {
    case obsFree_egg00:
        bRC = false;
        break;
    case obsReserved_egg00:
    case obsKeyReserved_egg00:
        bRC = true;
        break;
    case obsFreeAfterEot_egg00:
    case obsOccupied_egg00:
        {
            if (ObjHeader.ohdTransInfo_gg92.otrLockTransId_gg00.gg90IsNil())
                bRC = false;
            else if (ObjHeader.ohdTransInfo_gg92.otrLockTransId_gg00.gg90IsDummy() ||
                ObjHeader.ohdTransInfo_gg92.otrLockTransId_gg00.gg90IsZero())
                bRC = true;  /* isn't really exact, because lock may not be active */
            else
            {
                k52acc_trans_entry( ObjHeader.ohdTransInfo_gg92.otrLockTransIndex_gg00, ThisTrans );
                if (( ObjHeader.ohdTransInfo_gg92.otrLockTransId_gg00 == ThisTrans->teWriteTransId_kb05 ) &&
                    ( m_delete != ThisTrans->teState_kb05 ))
                    bRC = true;
                else
                    bRC = false;
            }
        }
        break;
    default:
        bRC = false;
    }
    return bRC;
}

/*---------------------------------------------------------------------------*/

externCpp bool
kb67IsObjHistoryUsed (tgg00_TransContext &Trans,
                      tgg92_KernelOid    &Oid,
                      tgg00_ObjTransInfo &ObjTransInfo,
                      bool                ignoreVersions,        /* PTS 1130354 */
                      SAPDB_UInt8         &OldestTransNo)        /* PTS 1130354 */
{
    tgg91_TransNo l_OldestTransNo;
    OldestTransNo = 0;

    /* PTS 1106284 UH 07-04-2000 new */
    ROUTINE_DBG_MSP00 ("kb67IsObjHistoryUsed");
    if (e_ok != Trans.trError_gg00) return false;
        if ( kb51IsObjTransUsed (Trans.trTaskId_gg00, Oid, ObjTransInfo, (ignoreVersions ? 1 : 0), l_OldestTransNo) &&
        (e_ok == Trans.trError_gg00))
    {
        OldestTransNo = l_OldestTransNo.gg91GetUInt8();
        return true;
    }
    return false;
}

/*-----------------------------------------------------------------------------------*/

externCpp void
kb67LockObj (tgg00_TransContext  &Trans,
             tgg00_FileId        &ObjFile,
             tgg92_KernelOid      Oid,
             tgg91_PageRef        WantedObjVers,
             tgg00_ObjState_Enum  ObjState,
             tgg00_ObjTransInfo  &ObjTransInfo)
{ 
    // NOTE! if this function returns e_wait_for_lock_release then wait outside bd-locking 
    
    ROUTINE_DBG_MSP00 ("kb67LockObj");
    
    if (e_ok != Trans.trError_gg00) return;

    pasbool ignShrLckCsView = WantedObjVers.gg91IsDummyRef();
    kb50LockObj (Trans, ObjFile.fileDirFileId_gg00(), Oid, ObjTransInfo, ignShrLckCsView);  
    if (e_ok != Trans.trError_gg00) return;

    if (obsFreeAfterEot_egg00 == ObjState)
    {
        kb50UnlockObj (Trans.trTaskId_gg00, Trans.trWriteTransId_gg00, Trans.trIndex_gg00,
            Oid, ObjTransInfo);
        Trans.trError_gg00 = e_object_not_found;
        return;
    }

    if ( ! ignShrLckCsView && (WantedObjVers != ObjTransInfo.otrBeforeRef_gg00)) 
    {
        kb50UnlockObj (Trans.trTaskId_gg00, Trans.trWriteTransId_gg00, Trans.trIndex_gg00,
            Oid, ObjTransInfo);
        Trans.trError_gg00 = e_object_dirty;
        if (g01vtrace.vtrKbLock_gg00)
        {
            kb50XxxObjTrace (Trans, "  kb67LckObj",
                Oid, ObjTransInfo.otrUpdTransId_gg00, ObjTransInfo.otrBeforeRef_gg00, ObjState);
        }
        return;
    }
    
#   if COMPILEMODE_MEO00 >= SLOW_MEO00
    t01page_ref    (kb, "WntedObjVers", WantedObjVers);
    t01page_ref    (kb, "otrBeforeRef", ObjTransInfo.otrBeforeRef_gg00);
    t01basis_error (kb, "trError     ", Trans.trError_gg00);
#   endif 
}


/*-----------------------------------------------------------------------------------*/

externCpp void
kb67LockShareObj (tgg00_TransContext  &Trans,
                  tgg00_FileId        &ObjFile,
                  tgg92_KernelOid      Oid,
                  tgg91_PageRef        WantedObjVers,
                  tgg00_ObjState_Enum  ObjState,
                  tgg00_ObjTransInfo  &ObjTransInfo)
{    
    ROUTINE_DBG_MSP00 ("kb67LockShareObj");
    
    if (e_ok != Trans.trError_gg00) return;

    kb50LockShareObj (Trans, ObjFile.fileDirFileId_gg00(), Oid, ObjTransInfo); 
    if (e_ok != Trans.trError_gg00) return;

    if (obsFreeAfterEot_egg00 == ObjState)
    {
        kb50UnlockShareObj (Trans.trTaskId_gg00, Trans.trWriteTransId_gg00, 
            Trans.trIndex_gg00, Oid, ObjTransInfo);
        Trans.trError_gg00 = e_object_not_found;
        return;
    }

    if ( ! WantedObjVers.gg91IsDummyRef() && (WantedObjVers != ObjTransInfo.otrBeforeRef_gg00)) 
    {
        kb50UnlockShareObj (Trans.trTaskId_gg00, Trans.trWriteTransId_gg00, 
            Trans.trIndex_gg00, Oid, ObjTransInfo);
        Trans.trError_gg00 = e_object_dirty;
        if (g01vtrace.vtrKbLock_gg00)
        {
            kb50XxxObjTrace (Trans, "  kb67LckShr",
                Oid, ObjTransInfo.otrUpdTransId_gg00, ObjTransInfo.otrBeforeRef_gg00, ObjState);
        }
        return;
    }

#   if COMPILEMODE_MEO00 >= SLOW_MEO00
    t01page_ref    (kb, "WntedObjVers", WantedObjVers);
    t01page_ref    (kb, "otrBeforeRef", ObjTransInfo.otrBeforeRef_gg00);
    t01basis_error (kb, "trError     ", Trans.trError_gg00);
#   endif 
    
}

/*-----------------------------------------------------------------------------------*/

externCpp void
kb67LockUpdateObj (tgg00_TransContext   &Trans,
                   tgg00_FileId         &ObjFile,
                   tgg92_KernelOid       Oid,
                   tgg91_PageRef	     WantedObjVers,
                   tgg92_ObjHeader      &ObjHeader,
                   tgg00_ObjTransInfo   &ObjTransInfo,
                   Msg_List &ErrorList)
{    
    ROUTINE_DBG_MSP00 ("kb67LockUpdateObj");

    if (e_ok != Trans.trError_gg00) return;

    SAPDB_Int1 contVarObject = 0;       
    if  (ObjFile.fileObjFileType_gg00() == oftVarLenObjFile_egg00)
        contVarObject = tbd900_VarObjBodyHeader::VarPrimHeadObj;        
    const bool bOmsLayerReusedObj = false;        
    kb67DelUpdObj(Trans, m_lock, ObjFile, Oid, 0, WantedObjVers, ObjHeader, 0, NULL,
        contVarObject, bOmsLayerReusedObj, ErrorList);        
}

/*-----------------------------------------------------------------------------------*/

externCpp void
kb67NewObj (tgg00_TransContext   &Trans,
            tgg00_FileId         &ObjFile,
            tgg92_KernelOid       Oid,
            tgg92_ObjHeader      &ObjHeader,     
            tsp00_Int4            ObjBodySize,
            tsp00_Int4            ObjKeySize,
            SAPDB_Int1            ContVarObject,
            Msg_List &ErrorList)
{       
    ROUTINE_DBG_MEO00 ("kb67NewObj");
    
    tgg00_ObjState_Enum  WantedObjState = (ObjKeySize == 0) ? obsReserved_egg00 : obsKeyReserved_egg00;
    tgg00_ObjState      &ObjState     = ObjHeader.ohdState_gg92;
    tgg00_ObjTransInfo  &ObjTransInfo = ObjHeader.ohdTransInfo_gg92;
    tgg91_PageRef       &ObjBeforeRef = ObjTransInfo.otrBeforeRef_gg00;
    
    if (e_ok != Trans.trError_gg00) return;
    
    
    if ((0 == ObjKeySize) || (obsFree_egg00 == ObjState))
    {
        ObjBeforeRef.gg91SetNilRef();
        ObjTransInfo.otrLockTransIndex_gg00 = cgg_nil_transindex;
        ObjTransInfo.otrLockTransId_gg00.gg90SetNil();
        ObjTransInfo.otrUpdTransId_gg00.gg90SetNil();   
    }
    
    kb67LockObj (Trans, ObjFile, Oid, ObjBeforeRef, WantedObjState, ObjTransInfo);    
    
    if (e_ok != Trans.trError_gg00) return;
    
    if ((obsKeyReserved_egg00 == ObjState) ||
        (obsOccupied_egg00    == ObjState)  )
    {
        Trans.trError_gg00 = e_duplicate_hash_key;
        
        if (g01vtrace.vtrKbLock_gg00)
        {
            kb50XxxObjTrace (Trans, "  kb67NewObj",
                Oid,
                ObjTransInfo.otrUpdTransId_gg00,
                ObjBeforeRef,
                ObjState);
        }
        return;
    }

    Log_BeforeImage  BeforeImage;
    Log_ActionObject RollbackAction;

    BeforeImage.SetTransInfoAndObjState ( ObjTransInfo, ObjState );

    if ( ! RollbackAction.InitNew ( ObjFile, Oid, ObjBeforeRef, 0, NULL, ContVarObject, ErrorList ) )
    {
        Trans.trError_gg00 = e_undo_log_write_error;
        return;
    }
    
    Data_PageNo     pageno;
    Data_PageOffset offset;

    Log_Transaction& TransContext = *reinterpret_cast<Log_Transaction*>(Trans.trLogContext_gg00);

    switch ( TransContext.WriteUndoEntry ( BeforeImage, RollbackAction, pageno, offset, ErrorList) )
    {
        case Log_UndoFile::writeBeforeImageOk:
            break;
        case Log_UndoFile::writeBeforeImageError:
            Trans.trError_gg00 = e_undo_log_write_error;
            break;
        case Log_UndoFile::writeBeforeImageNoSpaceAvailable:
            Trans.trError_gg00 = e_work_rolled_back;
            break;
        default:
            Trans.trError_gg00 = e_undo_log_write_error;
            break;
    }
    if( Trans.trError_gg00 != e_ok )
        return;
    ObjBeforeRef.gg91BuildRef (pageno, offset);
    ObjHeader.ohdTransInfo_gg92.otrUpdTransId_gg00 = Trans.trWriteTransId_gg00;
}

/*---------------------------------------------------------------------------*/

externCpp tsp00_Bool
kb67RollforwardObj(tgg92_ObjHeader          &ObjHeader,
                   const tgg91_TransNo      &RedoUpdTransNo, 
                   const Log_EntrySequence  &RedoEntrySequence,
                   const tgg91_PageRef      &ObjBeforeRef)
{
    ROUTINE_DBG_MSP00 ("kb67RollforwardObj");

    tgg00_ObjTransInfo& TransInfo = ObjHeader.ohdTransInfo_gg92;

    if (ObjHeader.ohdState_gg92 != obsFree_egg00)
    {
        /* check whether the before reference in the object frame header */
        /* is really an before reference or an redo entry sequenc number */
        if ( REDO_ENTRY_SEQ_LABEL == TransInfo.otrBeforeRef_gg00.gg91RefPos()
             &&
             TransInfo.otrUpdTransId_gg00 == RedoUpdTransNo
             &&
             TransInfo.otrBeforeRef_gg00.gg91RefPno() > (int)RedoEntrySequence )
        {
            /* interpret reference page number as redo entry sequence number */
            g01opmsg (sp3p_knldiag, sp3m_error, kbMsg_csp03, csp3_n_objlog, 
                "kb67Rollfor:WrongRedoSeq", RedoEntrySequence);
            g01opmsg (sp3p_knldiag, sp3m_error, kbMsg_csp03, csp3_n_objlog, 
                "kb67Rollfor:WrongRedoSeq", TransInfo.otrBeforeRef_gg00.gg91RefPno());
            return false;
        }
    }
    TransInfo.otrBeforeRef_gg00.gg91BuildRef(RedoEntrySequence, REDO_ENTRY_SEQ_LABEL);
    TransInfo.otrUpdTransId_gg00 = RedoUpdTransNo;
    return true;
}

/*---------------------------------------------------------------------------*/

externCpp void
kb67WriteDeleteObjectAfterImage (tgg00_TransContext    &trans,
                                 FileDir_FileNo        &fileno,
                                 const tgg92_KernelOid &oid,
                                 const tgg91_PageRef   &version,
                                 SAPDB_Int1             contVarObj,          
                                 bool                   bOmsLayerReusedObj,
                                 Msg_List  &ErrorList)
{
    ROUTINE_DBG_MSP00 ("kb67WriteDeleteObjectAfterImage");
    Log_Transaction &logtrans = *(reinterpret_cast<Log_Transaction*>(trans.trLogContext_gg00));
    Log_ActionObject action;

    action.InitDelete (fileno, oid, version, bOmsLayerReusedObj, contVarObj);
    trans.trError_gg00 = kb560ConvertWriteRedoEntryResultToBasisError(
        logtrans.WriteRedoEntry (action, ErrorList));
}

/*---------------------------------------------------------------------------*/

externCpp void
kb67GetBeforeImage (tgg00_TransContext   &Trans,     
                    tgg92_KernelOid       Oid,
                    tgg91_PageRef         FirstBeforeRef,
                    bool                  bWithObject,
                    tsp00_Int4            KeyLen,
                    tsp00_Int4           &ObjBodyLen,
                    tsp00_Int4            ObjBodySize,
                    void                 *pObjBody,
                    tgg00_ObjFrameVers   &FrameVers,
                    tgg91_PageRef        &ResultObjVers,
                    tgg91_PageRef        &ObjHistRef,         
                    tgg91_TransNo        &HistUpdTransId,         // PTS 1137480
                    tsp00_Int4           &LogReadAccesses,
                    Msg_List &ErrorList)
{
    ROUTINE_DBG_MEO00 ("kb67GetBeforeImage");
        
    bool           bObjFound     = false;
    tgg91_PageRef  CurrEntryRef;
    tgg91_PageRef  PrevHistPosWithBody;
    tgg91_PageRef  NextPageRef;
    tgg91_TransNo  ConsistentView;
    //tgg91_TransNo  HistUpdTransId;          PTS 1137480
    tgg00_MessType Action;

    LogReadAccesses = 0;                                      
    CurrEntryRef    = FirstBeforeRef;
    PrevHistPosWithBody.gg91SetNilRef();

    if (Trans.trError_gg00 != e_ok) return;
    
#   if COMPILEMODE_MEO00 >= SLOW_MEO00  
    ta01Oid     (kb, "Oid         ", Oid);
    ta01TransNo (kb, "trCnsistVers", Trans.trConsistView_gg00);
#   endif
    
    do
    {
        if ( NIL_PAGE_NO_GG00 == CurrEntryRef.gg91RefPno() )
        {
            Trans.trError_gg00 = e_object_not_found;
            
            if (g01vtrace.vtrKbLock_gg00)
            {
                kb50XxxObjTrace (Trans, "  kb67_Get1 ",
                    Oid,
                    Trans.trWriteTransId_gg00, // dummy param: UpdTransId not available
                    CurrEntryRef,
                    obsFree_egg00);            // dummy param: ObjState not available

            kb67_PrintError ("ObjSeq is nil                           ");
            }
        }
        else
        {
            Log_ActionType                ActionType = Log_NoOp;
            SAPDB_UInt4                   BodyLen;
            SAPDB_Byte                   *pBody      = reinterpret_cast<SAPDB_Byte*>(pObjBody);
            const Log_History::GetResult  result =
                Log_History::GetBeforeImage (Trans,
                                             Oid,
                                             CurrEntryRef,
                                             bWithObject ? ObjBodySize : 0,
                                             bWithObject ? pBody       : NULL,
                                             false, // PTS 1113317 UH 2002-01-08
                                             BodyLen,
                                             FrameVers,
                                             ActionType,
                                             ConsistentView,
                                             HistUpdTransId,
                                             NextPageRef,
                                             ErrorList);
            
            switch ( result )
            {
                case Log_History::getImageFound :
                    break;
                case Log_History::getIllegalPageRef :
                case Log_History::getWrongEntry :
                case Log_History::getActionNotPreparedForConsistentRead :
                case Log_History::getImageNotFoundBecauseDeleted :
                case Log_History::getAccessingUndoEntryFailed :
                case Log_History::getImageNotFound :
                case Log_History::getReadError :
                    if ( Trans.trError_gg00 != e_corrupted_datapage_from_io )
                        Trans.trError_gg00 = e_obj_history_not_found;
                    break;
                case Log_History::getInitError :
                    Trans.trError_gg00 = e_no_more_memory;
                    #ifdef SAPDB_QUICK
                    RTE_Crash(ErrorList);
                    #endif
                    break;
                case Log_History::getBodySizeTooSmall :
                case Log_History::getReleasingImageFailed :
                    Trans.trError_gg00 = e_history_read_error;
                    #ifdef SAPDB_QUICK
                    RTE_Crash(ErrorList);
                    #endif
                    break;
                default:
                    ErrorList = ErrorList + Log_Exception(__CONTEXT__,LOG_UNKNOWN_RESULT,
                                                      "Log_History::GetBeforeImage",
                                                      SAPDB_ToString(result));
                    #ifdef SAPDB_QUICK
                    RTE_Crash(ErrorList);
                    #endif
            }

            if ( ActionType != Log_LockObject )
            {
                ObjBodyLen = BodyLen; // the original length must be kept
                PrevHistPosWithBody = CurrEntryRef;
            }

            pObjBody       = pBody;
            
            Action.becomes(kb67_OldAction(ActionType));
            ++LogReadAccesses;
        }
                
        if ((e_ok == Trans.trError_gg00) && ! bObjFound)     
        {
            // CurrEntryRef = NextPageRef; PTS 1120478 FF
            
            // check before image
            
            pasbool bConsistent;  
            pasbool bCancelled;         /* PTS 1120151 FF 2003-JAN-29 */
            
            kb50ObjConsistentCheck (Trans, HistUpdTransId, bConsistent, bCancelled);
            //                      without NextPageRef:  PTS 1105492 JA 2000-03-15 
            
            if ((bCancelled && e_ok == Trans.trError_gg00))  /* PTS 1120151 FF 2003-JAN-29 */
            {
                if (g01vtrace.vtrKbLock_gg00)                   // PTS 1122466
                {
                    Kernel_VTrace     vTrace;
                    vTrace << "cancelled ConsistView:";
                    for (int i=0; i < sizeof(Trans.trConsistView_gg00); i++)
                        vTrace << ToStr((SAPDB_UInt1)Trans.trConsistView_gg00[i], _T_h);
                    vTrace << " found";
                }
                Trans.trError_gg00 = e_consview_cancelled;
                return;
            }
          
            if (bConsistent && e_ok == Trans.trError_gg00)
            {
                // before image is consistent
                
                if (m_new == Action)
                {
                    // there is no before image
                    Trans.trError_gg00 = e_object_not_found;
                    
                    if (g01vtrace.vtrKbLock_gg00)
                    {
                        kb50XxxObjTrace (Trans, "  kb67_Get4 ",
                            Oid,
                            HistUpdTransId,
                            CurrEntryRef,
                            obsReserved_egg00);   // Reserved instead of m_new
                    }
                }
                else
                {
                  ObjHistRef    = PrevHistPosWithBody;  // PTS 1120478 FF
                  ResultObjVers = NextPageRef;
                  bObjFound     = true;
                }
            } 
            else // NOT bConsistent
            {
                if  ( e_ok == Trans.trError_gg00 && g01vtrace.vtrKbLock_gg00 ) 
                {
                    kb50XxxObjTrace (Trans, "  kb67GetPrv",
                        Oid,
                        HistUpdTransId,
                        NextPageRef,
                        obsUpdBeforeImage_egg00);
                }
            }
            CurrEntryRef = NextPageRef;         // PTS 1120478 FF
        }
    } while (! bObjFound && (e_ok == Trans.trError_gg00) );
    
    if ( e_ok == Trans.trError_gg00 )
    {
        if ( ! bObjFound )
            Trans.trError_gg00 = e_object_not_found;
    }
    else if (( e_object_not_found != Trans.trError_gg00 ) && 
        ( e_consview_cancelled != Trans.trError_gg00 ))         /* PTS 1120151 FF 2003-JAN-29 */
    {
        char Msg[sizeof(tsp00_C40)+1];
        sp77sprintf (Msg, sizeof(tsp00_C40), "NoHist: %-10d.%10d %10d",
                     CurrEntryRef.gg91RefPno(), CurrEntryRef.gg91RefPos(),
                     Trans.trTaskId_gg00);
        kb67_PrintError (Msg);
        sp77sprintf (Msg, sizeof(tsp00_C40), "   OID: %-10d.%10d %10d",
                     Oid.gg92GetPno(), Oid.gg92GetPos(),
                     Trans.trError_gg00);
        kb67_PrintError (Msg);
        sp77sprintf (Msg, sizeof(tsp00_C40), "CONSVIEW: %d", Trans.trConsistView_gg00.gg90GetInt4());
        kb67_PrintError (Msg);
        Trans.trError_gg00 = e_obj_history_not_found;
    }
    
#   ifdef __CHECK_TIME
    g01time_vtrace (Trans, "OBJLOG CONS READ  ", StartSec, StartUsec);
#   endif
}

// end of program
                             
 
