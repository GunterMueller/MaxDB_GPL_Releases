/****************************************************************************

  module      : vbd95.cpp

  -------------------------------------------------------------------------

  responsible : FerdiF

  special area: rollforward
  description : methods for the roll forward of object file operations,
                during the recovery 


  last changed: 2003-05-16  16:55
  see also    : 

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2001-2005 SAP AG

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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

#include <stdlib.h>

#ifndef HGG06_H
#   include "hgg06.h"
#endif
#ifndef HBD95_H
#   include "hbd95.h"
#endif
#ifndef HBD95_1_H
#   include "hbd95_1.h"
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
#ifndef GBD930_H
#   include "gbd930.h"
#endif
#include "hbd04.h"
#include "hbd06.h"
#include "hbd20_9.h"
#include "hbd22.h"
#include "hbd90.h"
#include "hbd91.h"
#include "hbd92.h"
#include "hbd93.h"
#include "SAPDB/SAPDBCommon/SAPDB_RangeCode.hpp" // Kernel_move_and_fill
#include "hkb67.h"

#include "liveCache/LVC_ErrorTraceObject.hpp"
#include "RunTime/RTE_Crash.hpp"
#include "Logging/Log_UndoFile.hpp"
#include "Logging/Log_Transaction.hpp"
#include "DataAccess/Data_FileTypes.hpp"
#include "KernelCommon/Kernel_OpMsg.hpp"        /* PTS 1115198 FF 2002-04-19 */
#include "KernelCommon/Kernel_IAdminInfo.hpp"
#ifndef MSG_LIST_HPP
#   include "Messages/Msg_List.hpp"
#endif
#ifndef TRANS_CONTEXT_HPP
#   include "Transaction/Trans_Context.hpp"
#endif

#if COMPILEMODE_MEO00 >= SLOW_MEO00 
#include "hta99.h"
#include "hta01.h"
#endif

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define FIRST_CHAIN_NO 0

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

 /*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/

// PTS 112198 FF 7.4.01 -> 7.4.02 Merge 29/11/2001
extern cbd930_GarbCollController  bd91GarbCollController;
   
/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/

static _INLINE void
bd95_RemoveAllPagesFromObjFile (cbd05_Current  &Current, 
                                cbd910_RootNode   &NptrsRoot);
  
/*---------------------------------------------------------------------------*/

static _INLINE void
bd95_RollforwardObj (tgg00_TransContext       &Trans,
                     tbd_nodeptr               pPage,
                     tbd900_ObjFrame          *pObjFrame,
                     tgg92_KernelOid           OId,
                     const tgg91_PageRef      &ObjVers,
                     FileDir_FileNo      &ObjFileNo,
                     tsp00_Int4                AfterObjKeyLen,
                     void                     *pAfterObjKey,
                     const tsp00_Bool          bOIdIsReused,
                     const Log_EntrySequence  &RedoEntrySequence,
                     const tgg91_TransNo      &RedoUpdTransNo,
                     SAPDB_Int1                ContVarObj);       /* PTS 1127661 FF */

/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

externCpp void
bd95Rollforward_CreateObjFile (tgg00_TransContext  &Trans,
                               const FileDir_FileNo &ObjFileNo,
                               Data_FileType        filetype,
                               tsp00_Int4           ObjBodyLen,
                               tsp00_Int4           ObjKeyLen,
                               Data_PageNo          rootPage,
                               LVC_FileNoArray      &filenoArr)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd95Rollforward_CreateObjFile", ObjContainer_Trace, 5);

    /* this function can be called only during a restart */
    SAPDBERR_ASSERT_STATE (Kernel_IAdminInfo::Instance().KernelStateIsRedoLogProcessing());
    
    Msg_List& ErrorList = Trans_Context::GetContext(Trans).ErrorList();
    
    /* create an appropriate file id */
    tgg00_FileId    ObjFileId = b01niltree_id;
    const tsp00_Bool bCreateFileIsOnline = false;
     
    ObjFileId.fileDirFileId_gg00() = *reinterpret_cast<const tsp00_C8*>(ObjFileNo.GetCharArray());
    if (Data_OmsVarFile == filetype || Data_OmsContFile == filetype)
        ObjFileId.fileObjFileType_gg00().becomes(oftVarLenObjFile_egg00);
    else
        ObjFileId.fileObjFileType_gg00().becomes(oftFixLenObjFile_egg00);
   
    int numKeyPartitions = 0;
    if (Data_OmsKeyFile == filetype )
        numKeyPartitions = filenoArr.GetNumUsedElements();
    bd04CreateObjFile (Trans, ObjFileId, ObjBodyLen, ObjKeyLen, (numKeyPartitions > 1), 
        bCreateFileIsOnline, rootPage, PRIM_CONT_OBJ_FILE_NO_BD900, filenoArr, ErrorList ); 
}


/*---------------------------------------------------------------------------*/


externCpp void
bd95Rollforward_DropObjFile (tgg00_TransContext  &Trans,
                             const FileDir_FileNo &ObjFileNo)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd95Rollforward_DropObjFile", ObjContainer_Trace, 5);

    /* this function can be called only during a restart */
    SAPDBERR_ASSERT_STATE (Kernel_IAdminInfo::Instance().KernelStateIsRedoLogProcessing());

    Msg_List& ErrorList = Trans_Context::GetContext(Trans).ErrorList();

    const SAPDB_Bool    bConsistent = true;
    FileDir_Oms  fd_smart;
    FileDir_IDirectory &IFDC  = FileDir_IDirectory::SessionInstance(Trans);
    FileDir_ReturnCode  fd_rc = IFDC.GcGetFile(Trans.trTaskId_gg00, ObjFileNo, fd_smart, ErrorList);

    if (FileDir_Okay != fd_rc) 
    {
        // PTS 1137309 UH 2005-08-12 added condition to ignore the error
        if ( FileDir_FileNoNotFound == fd_rc
             &&
             reinterpret_cast<Log_Transaction*>(Trans.trLogContext_gg00)->WasOpenInLastSavepoint() )
        {
            Kernel_OpWarning(csp3_bd_msg, csp3_n_obj)
                << __FILE__ << ":" << __LINE__ 
                << " bd95Rollforward_DropObjFile File:" << ObjFileNo
                << " is already deleted";
            return;
        }
        
        bd900MapFileDirError(Trans, fd_rc, e_file_not_found, (char*)__FILE__,  __LINE__ );
        return;
    }

    if ((Data_FileDeleted == fd_smart.GetFileState())
        &&
        !((Log_Transaction *) Trans.trLogContext_gg00)->WasOpenInLastSavepoint())
    {
        Kernel_OpWarning(csp3_bd_msg, csp3_n_obj)
            << __FILE__ << ":" << __LINE__ 
            << " bd95Rollforward_DropObjFile File:" << ObjFileNo
            << " is already marked for delete";
    }

    if (Data_OmsKeyFile == fd_smart.GetFileType()) 
    {
        const tsp00_Int4 NumKeyFiles = fd_smart.GetKeyPartitionCount();
        for (tsp00_Int4 iKeyFileNo = 1; iKeyFileNo <=NumKeyFiles; ++iKeyFileNo)
        {
            FileDir_FileNo fd_fileid = fd_smart.GetKeyPartition(iKeyFileNo);
            if ( fd_fileid.IsValid() ) 
            {
                FileDir_Oms     fd_smart2;
                IFDC.GcGetFile(Trans.trTaskId_gg00, fd_fileid, fd_smart2, ErrorList);
                if ((FileDir_Okay == fd_rc) && (Data_FileDeleted != fd_smart2.GetFileState()))
                {
                    tgg91_TransNo DummyTransNo;               /* *** TODO *** */
                    gg06SetNilTrans (DummyTransNo);           /* *** TODO *** */
                    fd_smart2.MarkFileDeleted(DummyTransNo);  /* *** TODO *** */
                }
            }
        }
    }

    if (Data_FileDeleted != fd_smart.GetFileState())
    {
        tgg91_TransNo DummyTransNo;               /* *** TODO *** */
        gg06SetNilTrans (DummyTransNo);           /* *** TODO *** */
        fd_smart.MarkFileDeleted(DummyTransNo);   /* *** TODO *** */
    }

    /* create file id and current for the file which is to delete */
    cbd05_Current  Current (Trans, m_update);           

    fd_smart.BuildFileId(Current.curr_tree_id);
    fd_smart.Detach();

    bd95RemoveObjFile(Current);
}

/*---------------------------------------------------------------------------*/

externCpp void
bd95RollforwardObj_DelObj (tgg00_TransContext       &Trans,           
                           tgg92_KernelOid           OId,
                           const tgg91_PageRef      &ObjVers,
                           const FileDir_FileNo     &FileNo,
                           const Log_EntrySequence  &RedoEntrySequence,
                           const tgg91_TransNo      &RedoUpdTransNo,
                           SAPDB_Int1                ContVarObj,            /* PTS 1127661 FF */
                           bool                      Reused,                /* PTS 1127661 FF */
                           Msg_List                 &ErrorList)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd95RollforwardObj_DelObj", ObjContainer_Trace, 5);    
    
    /* this function can be called only during a restart */
    SAPDBERR_ASSERT_STATE (Kernel_IAdminInfo::Instance().KernelStateIsRedoLogProcessing());
    
    tgg00_BasisError  &TrError = Trans.trError_gg00;
    
    /* create file id and current for the file containing the object to be updated */
    cbd05_Current  Current (Trans, m_update);           

    /* this object writes a message into the vtrace at beginning and end of this routine */
    LVC_ErrorTraceObject   ErrorTraceObject  (*Current.curr_trans, Current.curr_tree_id, FileNo, OId, b95redo_del);

    FileDir_FileNo ObjFileNo = FileNo;
    FileDir_Oms         fd_smart;
    FileDir_IDirectory &IFDC  = FileDir_IDirectory::SessionInstance(Trans);
    FileDir_ReturnCode  fd_rc = IFDC.GetFile(
        Trans.trTaskId_gg00, ObjFileNo, fd_smart, ErrorList);
    if (FileDir_Okay != fd_rc) {
        bd900MapFileDirError(Trans, fd_rc, e_ok, (char*)__FILE__,  __LINE__ );
        ErrorTraceObject.SetStatus (rosFileAlreadyDeleted_gg12);
        return;
    }
    fd_smart.BuildFileId(Current.curr_tree_id);

  
    /* get page pointer */
    cbd910_PrimObjDataNode  NptrsPage(Current, OId.gg92GetPno(), nr_for_update);
    if (TrError != e_ok)        
    {
        if ((e_page_in_wrong_tree == TrError) || (e_no_converter_entry == TrError)) /* PTS 1105630 Alexander Kley 16/02/2000 */
        {
            FileDir_ReturnCode  fd_rc = IFDC.GetFile(
                Trans.trTaskId_gg00, ObjFileNo, fd_smart, ErrorList);
            if (FileDir_Okay != fd_rc) {
                bd900MapFileDirError(Trans, fd_rc, e_ok, (char*)__FILE__,  __LINE__ );
                ErrorTraceObject.SetStatus (rosFileAlreadyDeleted_gg12);
            }
            else 
            {
                /* check if this error could due to the fact that this delete was         */
                /* already performed during the online time. This can happen for the      */
                /* rollforwards of transactions which were open during the last savepoint */
                /* PTS 1111338 AK 10/08/2001                                              */
                if (((Log_Transaction *) Trans.trLogContext_gg00)->WasOpenInLastSavepoint())
                    TrError = e_ok;
            }
        }
        if (e_ok != TrError)
        {
            Kernel_OpError(csp3_bd_msg, csp3_n_obj)
                << __FILE__ << ":" << __LINE__ 
                << " DelObj:" << TrError
                << " ObjFileNo:" << ObjFileNo
                << " OID=" << OId.gg92GetPno() << "." << OId.gg92GetPos()
                << " (vers " << OId.gg92GetFrameVers() << ")";
        }
        return; 
    }

    Converter_Version converterVersion = 
        bd20GetDestinationConverterVersion ( Trans.trTaskId_gg00, NptrsPage.np_cbptr() );

    // PTS 1119428 FF 2002-DEC-09
    FileDir_FileNo contHndl;
    bd900GetFileNo(NptrsPage.np_ptr(), contHndl);
    if (contHndl != ObjFileNo)
    {
        ErrorTraceObject.SetStatus (rosFileAlreadyDeleted_gg12);
        return;
    }
    
    /* get pointer to subroot page which is required if this operation forces an update of the page chain */    
    cbd910_SubRootNode    NptrsSubRoot  (Current, nr_for_update);
    
	if ( NptrsPage.bd910LockSubRootBeforeReleasingFrame() ) 
    {
        NptrsSubRoot.bd910AssignToSubroot (NptrsPage);
        if (e_ok != TrError)
        {
            if ((e_page_in_wrong_tree == TrError) || (e_no_converter_entry == TrError)) /* PTS 1105630 Alexander Kley 16/02/2000 */
            {
                /* check if this error occured since the wanted container was dropped, PTS 1109582 AK 09/03/2001*/
                FileDir_ReturnCode  fd_rc = IFDC.GetFile(
                    Trans.trTaskId_gg00, ObjFileNo, fd_smart, ErrorList);
                if (FileDir_Okay != fd_rc) {
                    bd900MapFileDirError(Trans, fd_rc, e_ok, (char*)__FILE__,  __LINE__ );
                    ErrorTraceObject.SetStatus (rosFileAlreadyDeleted_gg12);
                }
            }
            if (e_ok != TrError)
            {
                Kernel_OpError opErrMsg(csp3_bd_msg, csp3_n_obj);
                opErrMsg << __FILE__ << ":" << __LINE__ 
                    << "DelObj:" << TrError;
                if (NptrsPage.np_ptr())  // PTS 1119428 FF 2002-DEC-09
                    opErrMsg << " root:" << NptrsPage.np_ptr()->nd_root();
                opErrMsg << " ObjFileNo:" 
                    << ObjFileNo
                    << " OID=" << OId.gg92GetPno() << "." << OId.gg92GetPos()
                    << " (vers " << OId.gg92GetFrameVers() << ")";
            }
            return; 
        }
        
        /* check if chaining will still be needed ( meahnwhile the page may have changed a lot */
	    if ( !NptrsPage.bd910LockSubRootBeforeReleasingFrame() )
        {   
            /* no chaining will be needed anymore ==> release subroot */
            NptrsSubRoot.bd910ReleasePage();
        }
    }
    
    /* check if the page  belongs to the wanted file  */
    bd900GetFileNo(NptrsPage.np_ptr(), contHndl);
    if ((ObjFileNo != Current.curr_tree_id.fileDirFileId_gg00()) ||
        (ObjFileNo != contHndl))
    {
        /* check if this error occured since the container was dropped */
        FileDir_ReturnCode  fd_rc = IFDC.GetFile(
            Trans.trTaskId_gg00, ObjFileNo, fd_smart, ErrorList);
        if (FileDir_Okay != fd_rc) {
            bd900MapFileDirError(Trans, fd_rc, e_ok, (char*)__FILE__,  __LINE__ );
            ErrorTraceObject.SetStatus (rosFileAlreadyDeleted_gg12);
        }
        else if (((Log_Transaction *) Trans.trLogContext_gg00)->WasOpenInLastSavepoint()) // PTS 1111338 AK 10/08/2001  
            TrError = e_ok;
        else
        {
            bd900GetFileNo(NptrsPage.np_ptr(), contHndl);
            ErrorTraceObject.WriteVersionErrorMessage (
                contHndl,
                ObjVers, NULL);
            TrError = e_wrong_class_id;
            
        }
        if (e_ok != TrError)
        {
            FileDir_FileNo fileno;
            bd900GetFileNo( NptrsPage.np_ptr(), fileno);

            Kernel_OpError  opErrMsg( csp3_bd_msg, csp3_n_obj );
            opErrMsg << __FILE__ << ":" << __LINE__ 
                << "DelObj:" << TrError
                << " parm ObjFileNo:" << ObjFileNo;
            if (NptrsPage.np_ptr())
                opErrMsg << " page FileNo:" << fileno;
            opErrMsg << " OID=" << OId.gg92GetPno() << "." << OId.gg92GetPos()
                << " (vers " << OId.gg92GetFrameVers() << ")";
        }
        return; 
    }
    
    /* determine pointer to object frame */
    BD92CHECK_OID_POSITION ("bd95RollforwardObj_DelObj",TrError, OId, NptrsPage);
    tbd900_ObjFrame  *pObjFrame = NptrsPage.bd910ObjFramePtr (OId.gg92GetPos());
    
    /* check if the object state is correct for the wanted rollforward */
    if ((pObjFrame->objHeader_bd900.ohdState_gg92 != obsReserved_egg00    ) &&
        (pObjFrame->objHeader_bd900.ohdState_gg92 != obsKeyReserved_egg00 ) &&
        (pObjFrame->objHeader_bd900.ohdState_gg92 != obsOccupied_egg00   ))
    {
        if (((Log_Transaction *) Trans.trLogContext_gg00)->WasOpenInLastSavepoint()) // PTS 1111338 AK 10/08/2001  
            return;
        
        TrError = e_wrong_object_state;
        Kernel_OpError(csp3_bd_msg, csp3_n_obj)
            << __FILE__ << ":" << __LINE__ 
            << " DelObj:" << TrError
            << " objstate:" << pObjFrame->objHeader_bd900.ohdState_gg92
            << " OID=" << OId.gg92GetPno() << "." << OId.gg92GetPos()
            << " (vers " << OId.gg92GetFrameVers() << ")";
        bd900GetFileNo(NptrsPage.np_ptr(), contHndl);
        ErrorTraceObject.WriteVersionErrorMessage (
            contHndl,
            ObjVers, pObjFrame);
        return;
    }   
    
    /* check and update the object header, in principle there is no  */
    /* need for the update since the object is detroyed any way      */
    if (!kb67RollforwardObj(pObjFrame->objHeader_bd900, RedoUpdTransNo, RedoEntrySequence, ObjVers))
    {
        TrError = e_wrong_object_version;
        Kernel_OpError(csp3_bd_msg, csp3_n_obj)
            << __FILE__ << ":" << __LINE__ 
            << "DelObj:" << TrError
            << " OID=" << OId.gg92GetPno() << "." << OId.gg92GetPos()
            << " (vers " << OId.gg92GetFrameVers() << ")";
        bd900GetFileNo(NptrsPage.np_ptr(), contHndl);
        ErrorTraceObject.WriteVersionErrorMessage (
            contHndl,
            ObjVers, pObjFrame);
        return;
    }
    
    /* =========================================================== */
    /* ----- after all checks the object can be released now ----- */
    /* =========================================================== */
    SAPDBMem_IRawAllocator &allocator = *(reinterpret_cast<SAPDBMem_IRawAllocator*>(Trans.trAllocator_gg00));
    Data_PageAccessManager  dummyPam      (Trans, Data_UndoFile, Data_PageRecoveryMode(), Data_PageNo());
    Log_UndoFile::Iterator  dummyIterator (dummyPam, allocator);

    const bool bKeyAlreadyRemoved = true; // new PTS 1128894 FF 31-MAR-2004
    bd91RemoveObjFromFile (Current, NptrsPage, NptrsSubRoot, pObjFrame, OId,
        converterVersion, fd_smart, dummyIterator, ErrorTraceObject, 
        ! bKeyAlreadyRemoved, ErrorList );
    if (e_ok != TrError)
    {
        FileDir_ReturnCode  fd_rc = IFDC.GetFile(
            Trans.trTaskId_gg00, ObjFileNo, fd_smart, ErrorList);
        if (FileDir_Okay != fd_rc) {
            bd900MapFileDirError(Trans, fd_rc, e_ok, (char*)__FILE__,  __LINE__ );
            ErrorTraceObject.SetStatus (rosFileAlreadyDeleted_gg12);
        }
        else
        {
            Kernel_OpError(csp3_bd_msg, csp3_n_obj)
                << __FILE__ << ":" << __LINE__ 
                << "DelObj:" << TrError
                << " ObjFileNo:" << ObjFileNo
                << " OID=" << OId.gg92GetPno() << "." << OId.gg92GetPos()
                << " (vers " << OId.gg92GetFrameVers() << ")";
        }
    }
}

/*---------------------------------------------------------------------------*/

externCpp void
bd95RollforwardObj_NewObj (tgg00_TransContext       &Trans,           
                           tgg92_KernelOid           OId,
                           const tgg91_PageRef      &ObjVers,
                           const FileDir_FileNo     &contNo,
                           tsp00_Int4                AfterObjKeyLen,
                           void                     *pAfterObjKey,
                           const Log_EntrySequence  &RedoEntrySequence,
                           const tgg91_TransNo      &RedoUpdTransNo,
                           SAPDB_Int1                ContVarObj,      /* PTS 1127661 FF */
                           Msg_List                 &ErrorList)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd95RollforwardObj_NewObj", ObjContainer_Trace, 5);    

    /* this function can be called only during a restart */
    SAPDBERR_ASSERT_STATE (Kernel_IAdminInfo::Instance().KernelStateIsRedoLogProcessing());

    tgg00_BasisError  &TrError = Trans.trError_gg00;
    tsp00_Bool         bPageGenerated;
    tsp00_Bool         bOIdIsReused = false;    /* OID of keyed object has been reused by basis       */
    tsp00_Bool         bKeyedObjectWithoutKey = false; // PTS 1122392 FF 30-May-2003

    FileDir_FileNo ObjFileNo = contNo;

    /* construct file id for the wanted file */
    cbd05_Current  Current (Trans, m_update); 

    /* this object writes a message into the vtrace at beginning and end of this routine */
    LVC_ErrorTraceObject   ErrorTraceObject  (Trans, Current.curr_tree_id, ObjFileNo, OId, b95redo_new);


    FileDir_Oms     fd_smart;
    FileDir_IDirectory &IFDC  = FileDir_IDirectory::SessionInstance(Trans);
    FileDir_ReturnCode  fd_rc = IFDC.GetFile(
        Trans.trTaskId_gg00, ObjFileNo, fd_smart, ErrorList);
    if (FileDir_Okay != fd_rc) {
        bd900MapFileDirError(Trans, fd_rc, e_ok, (char*)__FILE__,  __LINE__ );
        ErrorTraceObject.SetStatus (rosFileAlreadyDeleted_gg12);
        return;
    }

    fd_smart.BuildFileId(Current.curr_tree_id);

    /* check if the key is already used by an other object */
    if (AfterObjKeyLen > 0)
    {
        tgg92_KernelOid CurrOIdOwningTheKey;
        bd22GetOId (Trans, ObjFileNo, AfterObjKeyLen, pAfterObjKey, CurrOIdOwningTheKey, ErrorList);

        if (e_hash_key_not_found == TrError)
            TrError = e_ok;
        else
        {
            LVC_ErrorTraceObject   ErrorTraceKeyObject  (Trans, Current.curr_tree_id, ObjFileNo, CurrOIdOwningTheKey, b95delkey);

            /* check if the object frame is reused, which can be used to optimize the rollforward */
            if ((CurrOIdOwningTheKey.gg92GetPos() == OId.gg92GetPos()) && 
                (CurrOIdOwningTheKey.gg92GetPno() == OId.gg92GetPno()))
            {
                ErrorTraceKeyObject.SetStatus (rosOIdReused_gg12);
                TrError      = e_ok;    
                bOIdIsReused = true;
            }
            else
            {
                /* remove the object which is currently owning the key */

                /* get page pointer */
                cbd910_PrimObjDataNode  NptrsPage(Current, CurrOIdOwningTheKey.gg92GetPno(), nr_for_update);
                if (e_ok != TrError)
                {
                    FileDir_ReturnCode  fd_rc = IFDC.GetFile(
                        Trans.trTaskId_gg00, ObjFileNo, fd_smart, ErrorList);
                    if (FileDir_Okay != fd_rc) {
                        /* if the file was removed during this rollforward there remain the    */
                        /* rollforward is obsolet and one can stop here without doing anything */
                        bd900MapFileDirError(Trans, fd_rc, e_ok, (char*)__FILE__,  __LINE__ );
                        ErrorTraceObject.SetStatus (rosFileAlreadyDeleted_gg12);
                        return;
                    }

                    //741??
                    tgg00_BasisError AuxError = TrError;
                    TrError  = e_ok;

                    /* if another rollforward task can have removed the page */  
                    bd22GetOId (Trans, ObjFileNo, AfterObjKeyLen, pAfterObjKey, CurrOIdOwningTheKey, ErrorList);
                    if (e_hash_key_not_found == TrError)
                        TrError = e_ok;
                    else
                    {
                        TrError = AuxError;
                        if  ((e_ok != Trans.trError_gg00) && ! ErrorList.IsEmpty())
                            RTE_Message(ErrorList);
                        return;
                    }
                }
                else
                {
                    Converter_Version converterVersion = 
                        bd20GetDestinationConverterVersion ( Trans.trTaskId_gg00, NptrsPage.np_cbptr() );

                    tsp00_Bool bCurrOwningOIdIsAlreadyRemoved = false;
                    /* get pointer to subroot page which is required if this operation forces an update of the page chain */    
                    cbd910_SubRootNode    NptrsSubRoot  (Current, nr_for_update);

                    /* PTS 1114737 FF 2002-03-08 check if Object is still owning the key */
                    /* get pointer to subroot page which is required if this operation forces an update of the page chain */    
                    bd22GetOId (Trans, ObjFileNo, AfterObjKeyLen, pAfterObjKey, CurrOIdOwningTheKey, ErrorList);
                    if (e_hash_key_not_found == TrError)
                    {
                        TrError = e_ok;
                        bCurrOwningOIdIsAlreadyRemoved = true; 
                    }
                    else
                    {                       
					    if ( NptrsPage.bd910LockSubRootBeforeReleasingFrame() )
                        {
                            tgg00_BasisError AuxError = e_ok;    // PTS 1122026 FF
                            NptrsSubRoot.bd910AssignToSubroot (NptrsPage);
                            if (e_ok != TrError)
                            {
                                FileDir_ReturnCode  fd_rc = IFDC.GetFile(
                                    Trans.trTaskId_gg00, ObjFileNo, fd_smart, ErrorList);
                                if (FileDir_Okay != fd_rc) {
                                    /* if the file was removed during this rollforward there remain the    */
                                    /* rollforward is obsolet and one can stop here without doing anything */
                                    bd900MapFileDirError(Trans, fd_rc, e_ok, (char*)__FILE__,  __LINE__ );
                                    ErrorTraceObject.SetStatus (rosFileAlreadyDeleted_gg12);
                                    return;
                                }

                                AuxError = TrError;         // PTS 1122026 FF
                                TrError  = e_ok;
                            }                               // PTS 1122026 FF

                            /* bd910AssignToSubroot releases NptrsPage and re-accesses it   */
                            /* another rollforward task can have removed the page meanwhile */  
                            bd22GetOId (Trans, ObjFileNo, AfterObjKeyLen, pAfterObjKey, CurrOIdOwningTheKey, ErrorList);
                            if (e_hash_key_not_found == TrError)
                            {
                                TrError = e_ok;
                                bCurrOwningOIdIsAlreadyRemoved = true; 
                            }
                            else
                            {
                                if ( e_ok != AuxError )     // PTS 1122026
                                {
                                    TrError = AuxError;
                                    Kernel_OpError ( csp3_bd_msg, csp3_n_obj )
                                        << __FILE__ << ":" << __LINE__ 
                                        << " bd910AssignToSubroot:"
                                        << TrError
                                        << " Root:" << fd_smart.GetRootPage()
                                        << " FileNo:" << ObjFileNo;
                                    return;
                                }
                                // PTS 1122026
                                if ( e_ok != TrError )     
                                {
                                    Kernel_OpError ( csp3_bd_msg, csp3_n_obj )
                                        << __FILE__ << ":" << __LINE__ 
                                        << " bd22GetOId:" << TrError;
                                    return;
                                }
                            }
                        }
                    }

                    if (!bCurrOwningOIdIsAlreadyRemoved)
                    {
                        /* check if chaining will still be needed ( meahnwhile the page may have changed a lot */
					    if ( !NptrsPage.bd910LockSubRootBeforeReleasingFrame() )
                        {   
                            /* no chaining will be needed anymore ==> release subroot */
                            NptrsSubRoot.bd910ReleasePage();
                        }

                        /* determine pointer to object frame */
#                       if COMPILEMODE_MEO00 >= QUICK_MEO00 
                        BD92CHECK_OID_POSITION ("bd95RollforwardObj_NewObj",TrError, CurrOIdOwningTheKey, NptrsPage);
#                       endif
                        tbd900_ObjFrame  *pObjFrame = NptrsPage.bd910ObjFramePtr (CurrOIdOwningTheKey.gg92GetPos());

                        if (obsFreeAfterEot_egg00 != pObjFrame->objHeader_bd900.ohdState_gg92) {
                            //TrError = e_wrong_object_state; // PTS 1122392 FF 30-May-2003
                            if (g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdObject_gg00) 
                            {
                                Kernel_VTrace vtrace;
                                vtrace << __FILE__ << ":" << __LINE__ << 
                                    " Rollforward NewObject OID=" << OId.gg92GetPno() << "." 
                                    << OId.gg92GetPos() << " (vers " << OId.gg92GetFrameVers() << ") KeyLen:"
                                    << AfterObjKeyLen << " Key=";
                                for (int i=0; i < AfterObjKeyLen; i++)
                                    vtrace << ToStr((SAPDB_UInt1)*((SAPDB_UInt1*)pAfterObjKey + i),  _T_h);
                                vtrace << FlushLine;
                                vtrace << "Object found via key OID=" 
                                    << CurrOIdOwningTheKey.gg92GetPno() << "." 
                                    << CurrOIdOwningTheKey.gg92GetPos() << " (vers " 
                                    << CurrOIdOwningTheKey.gg92GetFrameVers() << " State:"
                                    << pObjFrame->objHeader_bd900.ohdState_gg92;
                            }
                            bOIdIsReused = true;            
                            bKeyedObjectWithoutKey = true;  
                        } else {                            
                            SAPDBMem_IRawAllocator &allocator = *(reinterpret_cast<SAPDBMem_IRawAllocator*>(Trans.trAllocator_gg00));
                            Data_PageAccessManager  dummyPam      (Trans, Data_UndoFile, Data_PageRecoveryMode(), Data_PageNo());
                            Log_UndoFile::Iterator  dummyIterator (dummyPam, allocator);

                            const bool bKeyAlreadyRemoved = true; // new PTS 1128894 FF 31-MAR-2004
                            bd91RemoveObjFromFile (Current, NptrsPage, NptrsSubRoot, pObjFrame, CurrOIdOwningTheKey,
                                converterVersion, fd_smart, dummyIterator, ErrorTraceKeyObject,
                                ! bKeyAlreadyRemoved, ErrorList);
                        }   
                    }
                }
            }
        }
    }

    /* get page pointer */
    cbd910_PrimObjDataNode  NptrsPage(Current, nr_for_update);
    NptrsPage.bd910AssignToNewSpecifiedPage (OId.gg92GetPno(), bPageGenerated);
    if (TrError != e_ok )
    {
        Kernel_OpError(csp3_bd_msg, csp3_n_obj)
            << __FILE__ << ":" << __LINE__ 
            << " bd910AssignToNewSpecifiedPage:" << TrError
            << " PNo:"  << OId.gg92GetPno() 
            << " Root:" << fd_smart.GetRootPage()
            << " FileNo:" << ObjFileNo;
        return;
    }

    Converter_Version converterVersion = 
        bd20GetDestinationConverterVersion ( Trans.trTaskId_gg00, NptrsPage.np_cbptr() );

    /* +++++ PTS 1119987 FF 2003-Jan-20 +++++ */
    cbd910_SubRootNode  NptrsSubRoot  (Current, nr_for_update); 
    if(bPageGenerated)
    {
        /* Don't release page here, so that no other running server may be able to */
        /* use this page before it is initialized and chained to its container.    */
        /* As this page isn't yet chained, only another bd95RollforwardObj_NewObj  */
        /* may collide with this page using bd910AssignToNewSpecifiedPage          */
        /* They MUST wait on this page until the first bd95RollforwardObj_NewObj   */
        /* has released it.                                                        */
        NptrsSubRoot.bd910AssignToPage(fd_smart.GetRootPage(), false /*=dont wait for lock*/);
        if (TrError == e_bd_page_collision)
        {
            /* PTS 1135361                                                        */
            /* page MUST NOT be released because it isn't yet inilitialized       */
            /* page lock MUST NOT be held when aquiring the root-page to avoid    */
            /* deadlocks.                                                         */
            /* Therefore release page as 'non used' page to converter and         */
            /* reaccess it later                                                  */
            TrError = e_ok;
            NptrsPage.bd910FreePage(); 
            NptrsSubRoot.bd910AssignToPage(fd_smart.GetRootPage());
        } 
        if (TrError != e_ok)
        {
            Kernel_OpError (csp3_bd_msg, csp3_n_obj)
                << __FILE__ << ":" << __LINE__ 
                << " bd910AssignToPage(SubRoot):" << TrError
                << " OID=" << OId.gg92GetPno() << "." 
                << OId.gg92GetPos() << " (vers " << OId.gg92GetFrameVers() << ")" 
                << " Root:" << fd_smart.GetRootPage()
                << " FileNo:" << ObjFileNo;
            return;
        }
        if (!NptrsPage.bd910PageIsAvailable())  /* PTS 1135361 */
        {
            NptrsPage.bd910AssignToNewSpecifiedPage (OId.gg92GetPno(), bPageGenerated);
            if (TrError != e_ok )
            {
                Kernel_OpError(csp3_bd_msg, csp3_n_obj)
                    << __FILE__ << ":" << __LINE__ 
                    << " bd910AssignToNewSpecifiedPage:" << TrError
                    << " PNo:"  << OId.gg92GetPno() 
                    << " Root:" << fd_smart.GetRootPage()
                    << " FileNo:" << ObjFileNo;
                return;
            }
        }
    }

    FileDir_FileNo lContainerNo;
    bd900GetFileNo(NptrsPage.np_ptr(), lContainerNo);
    /* if chaining is involved by this operation because of dechain from FreeList  */
    if (!bPageGenerated && 
        lContainerNo == ObjFileNo &&
        ((NptrsPage.np_ptr()->nd_free_obj_cnt() == 1) ||
        NptrsPage.bd910IsEmptyPage()))           /* PTS 1127493 FF 2004-FEB-02 */     
    {
        if( ! NptrsSubRoot.bd910PageIsAvailable())
            NptrsSubRoot.bd910AssignToSubroot(NptrsPage); 

        /* PTS 1125143 17-NOV-2003                                                    */
        /* check if during access to subroot NptrsPage had been freed and couldn't be */
        /* reaccessed because of being release meanwhile by a rollforward_del running */
        /* in parallel                                                                */
        if ((TrError == e_no_converter_entry) && 
            NptrsSubRoot.bd910PageIsAvailable() &&
            !NptrsPage.bd910PageIsAvailable())
        {
            TrError = e_ok;
            NptrsPage.bd910AssignToNewSpecifiedPage (OId.gg92GetPno(), bPageGenerated);
            if (TrError != e_ok) {
                Kernel_OpError  opErrMsg( csp3_bd_msg, csp3_n_obj );
                opErrMsg << __FILE__ << ":" << __LINE__ << " Error:" << TrError
                    << " page generated:" << bPageGenerated
                    << " OID=" << OId.gg92GetPno() << "." 
                    << OId.gg92GetPos() << " (vers " << OId.gg92GetFrameVers() << ")" 
                    << " Root:" << fd_smart.GetRootPage()
                    << " FileNo:" << ObjFileNo;
                if (NptrsPage.bd910PageIsAvailable()) {
                    FileDir_FileNo fileno;
                    bd900GetFileNo( NptrsPage.np_ptr(), fileno);
                    opErrMsg << " FileNo in Page:" << fileno;
                }
            }
            if (!bPageGenerated) {
                Kernel_OpError( csp3_bd_msg, csp3_n_obj )
                    << __FILE__ << ":" << __LINE__ << " page already existed !"; 
                TrError = e_no_converter_entry;
                return;
            }
        }

        /* check if during access to subroot NptrsPage had been freed and couldn't be */
        /* reaccessed because of being release meanwhile by a rollforward_del running */
        /* in parallel                                                                */
        if ((TrError == e_no_converter_entry) && 
            NptrsSubRoot.bd910PageIsAvailable() &&
            !NptrsPage.bd910PageIsAvailable() &&
            lContainerNo == ObjFileNo)
        {
            TrError = e_ok;
            NptrsPage.bd910AssignToNewSpecifiedPage (OId.gg92GetPno(), bPageGenerated);
            if (TrError != e_ok) {
                Kernel_OpError  opErrMsg( csp3_bd_msg, csp3_n_obj );
                opErrMsg << __FILE__ << ":" << __LINE__ << " Error:" << TrError
                    << " page generated:" << bPageGenerated
                    << " OID=" << OId.gg92GetPno() << "." 
                    << OId.gg92GetPos() << " (vers " << OId.gg92GetFrameVers() << ")" 
                    << " Root:" << fd_smart.GetRootPage()
                    << " FileNo:" << ObjFileNo;
                if (NptrsPage.bd910PageIsAvailable()) {
                    FileDir_FileNo fileno;
                    bd900GetFileNo( NptrsPage.np_ptr(), fileno);
                    opErrMsg << " FileNo in Page:" << fileno;
                }
            }
            if (!bPageGenerated) {
                Kernel_OpError( csp3_bd_msg, csp3_n_obj )
                    << __FILE__ << ":" << __LINE__ << " page already existed !"; 
                TrError = e_no_converter_entry;
                return;
            }
        }
        if (TrError != e_ok)
        {
            Kernel_OpError  opErrMsg( csp3_bd_msg, csp3_n_obj );
            opErrMsg << __FILE__ << ":" << __LINE__ 
                << " bd910AssignToSubroot:" << TrError
                << " OID=" << OId.gg92GetPno() << "." 
                << OId.gg92GetPos() << " (vers " << OId.gg92GetFrameVers() << ")" 
                << " Root:" << fd_smart.GetRootPage()
                << " FileNo:" << ObjFileNo;
            if (NptrsPage.bd910PageIsAvailable()) {
                FileDir_FileNo fileno;
                bd900GetFileNo( NptrsPage.np_ptr(), fileno);
                opErrMsg << " FileNo in Page:" << fileno;
            }
            if (NptrsSubRoot.bd910PageIsAvailable()) {
                FileDir_FileNo fileno;
                bd900GetFileNo( NptrsSubRoot.np_ptr(), fileno);
                opErrMsg << " FileNo in Subroot:" << fileno;
            }
            return;
        }
    }
    
  /* check if the page already belongs to the wanted file */
    if(!bPageGenerated && 
        lContainerNo != ObjFileNo)
    {
        /* the page does not belong to the wanted file and has to     */
        /* be removed from the file currently owning the object frame */
        bd95RemovePageFromObjFile (Trans, converterVersion, NptrsPage);
        if (TrError != e_ok )
        {
            Kernel_OpWarning(csp3_bd_msg, csp3_n_obj)
                << __FILE__ << ":" << __LINE__ 
                << " bd95RemovePageFromObjFile:" << TrError
                << " PNo:"  << OId.gg92GetPno()
                << " FileNo:" << lContainerNo;
            TrError = e_ok; //ignore all errors, may be too soft
        }

        /* reconstruct file id for the wanted file (destroyed by bd95RemovePageFromObjFile) */
        fd_smart.BuildFileId(Current.curr_tree_id);

        /* now first access subroot because later on it is needed for chaining    */
        NptrsSubRoot.bd910AssignToPage(fd_smart.GetRootPage());
        if (TrError != e_ok)
        {
            Kernel_OpError(csp3_bd_msg, csp3_n_obj)
                << __FILE__ << ":" << __LINE__ 
                << " bd910AssignToPage(root):"
                << TrError << " OID=" << OId.gg92GetPno() << "." 
                << OId.gg92GetPos() << " (vers " << OId.gg92GetFrameVers() << ")" 
                << " Root:" << fd_smart.GetRootPage()
                << " FileNo:" << ObjFileNo;
            return;
        }
        
        /* reassign the wanted page */
        NptrsPage.bd910AssignToNewSpecifiedPage (OId.gg92GetPno(), bPageGenerated);
        if (TrError != e_ok )
        {
            Kernel_OpError(csp3_bd_msg, csp3_n_obj)
                << __FILE__ << ":" << __LINE__ 
                << " bd910AssignToNewSpecifiedPage:" << TrError
                << " PNo:" << OId.gg92GetPno()  
                << " Root:" << fd_smart.GetRootPage()
                << " FileNo:" << ObjFileNo;
            return;
        }

        /* PTS 1116879 FF 2002-07-31 && PTS 1124681 FF 2003-OCT-15 */
        /* page may have been integrated into the object file by   */
        /* another rollforward_new running in parallel             */
        FileDir_FileNo pageObjFileNo;
        bd900GetFileNo(NptrsPage.np_ptr(), pageObjFileNo);

        if (!bPageGenerated && 
            pageObjFileNo != ObjFileNo)
        {
            Kernel_OpError(csp3_bd_msg, csp3_n_obj)
                << __FILE__ << ":" << __LINE__ 
                << " bd910AssignToNewSpecifiedPage (bPageGenerated == false) ! PNo:" 
                << OId.gg92GetPno();
            TrError = e_wrong_obj_file_id;
            return;
        }
    }   

    if(!bPageGenerated)
    {
        /* the page existed already and belongs  to the wanted file */
        SAPDBERR_ASSERT_STATE (NptrsPage.np_ptr()->nd_root()        == fd_smart.GetRootPage());

        /* get shortcut to the object page */
        tbd_nodeptr &pPage  = NptrsPage.np_ptr();

        /* get pointer to the wanted object frame           */
        BD92CHECK_OID_POSITION ("bd95RollforwardObj_NewObj",TrError, OId, NptrsPage);
        tbd900_ObjFrame  *pObjFrame = NptrsPage.bd910ObjFramePtr (OId.gg92GetPos());

        if (obsFreeAfterEot_egg00 == pObjFrame->objHeader_bd900.ohdState_gg92)
        {
            if (bOIdIsReused)
            {
#               if defined(FOR_TESTING_ONLY)
                tgg92_KernelOid CurrOIdOwningTheKey;
                bd22GetOId (Trans, ObjFileNo, AfterObjKeyLen, pAfterObjKey, CurrOIdOwningTheKey, ErrorList);
                if (e_hash_key_not_found == TrError)
                {
                    Kernel_OpError(csp3_bd_msg, csp3_n_obj)
                        << __FILE__ << ":" << __LINE__ 
                        << "OID reused, but key not found within index";
                    TrError = e_ok;
                }
#               endif
                bd95_RollforwardObj (Trans, pPage, pObjFrame,  OId, ObjVers,  ObjFileNo,
                    AfterObjKeyLen, pAfterObjKey, bOIdIsReused, RedoEntrySequence, RedoUpdTransNo, ContVarObj );

                /* update page header */
                -- pPage->nd_free_eot_obj_cnt();
                ++ pPage->nd_occ_obj_cnt();
            }
            else
            {
                {
                    /* create an object that logs errors and reports them into the vtrace  during its destruction */
                    LVC_ErrorTraceObject   ErrorTraceObject  (Trans, Current.curr_tree_id, ObjFileNo, OId, b95removeobj);

                    /* remove history entry pointing onto this object and update object header */
                    kb67DropObjHistory (Trans, pObjFrame->objHeader_bd900, OId, ErrorList);
                    if (e_obj_history_not_found == TrError) // PTS 1113251 UH 2001-12-27
                        TrError = e_ok;
                    if  (e_ok != Trans.trError_gg00)
                    {
                        if( ! ErrorList.IsEmpty())
                            RTE_Message(ErrorList);
                        return;
                    }

                    /* clear the object frame and insert it into the free obj chain */
                    const bool bKeyAlreadyRemoved = true; // new PTS 1128894 FF 31-MAR-2004
                    bd91RemoveObjFromPage (Current, NptrsPage, pObjFrame,  OId, converterVersion, 
                        fd_smart, ErrorTraceObject, ! bKeyAlreadyRemoved);
                    if (TrError != e_ok) return; 
                }
                
                /* remove the object from the free obj chain PTS 1111317 */
                tsp00_Int4         FreeObjCnt      = 0;
                tgg00_ObjPagePos * pNextFreeObjPos = &(pPage->nd_first_free_obj());

                /*   find the predecessor object in the free chain   * PTS 1127804 FF 2004-FEB-17 */  
                while (*pNextFreeObjPos != OId.gg92GetPos())
                {
                    /* PTS 1127804 FF 2004-FEB-17 */
                    if (((++FreeObjCnt) > pPage->nd_max_obj_cnt()) ||
                        ( *pNextFreeObjPos == NIL_OBJ_PAGE_POS_GG92 ) ||
                        ( NptrsPage.bd910ObjFramePtr(*pNextFreeObjPos)->objHeader_bd900.ohdState_gg92 != obsFree_egg00))
                    {
                        Kernel_OpError ( csp3_bd_msg, csp3_n_obj )
                            << __FILE__ << ":" << __LINE__
                            << " bd95RollforwardObj_NewObj:" << " OID=" << OId.gg92GetPno() << "." 
                            << OId.gg92GetPos() << " (vers " << OId.gg92GetFrameVers() << ")" 
                            << " not found in freelist"
                            << " LoopCnt:" << FreeObjCnt
                            << "page dumped into: d" 
                            << NptrsPage.np_ptr()->nd_id() << ".obj"; 

                        b06dump_bad_page (Trans.trTaskId_gg00,'d', ".obj", NptrsPage.np_ptr()->nd_id(), 
                            (void *) NptrsPage.np_ptr(), 1);
                        g01abort (csp3_bd_msg, csp3_n_obj, "bd95RollFwdOID    root: ", pPage->nd_root());
                    }
                    pNextFreeObjPos = &(NptrsPage.bd910ObjFramePtr (*pNextFreeObjPos)->objHeader_bd900.ohdNextFreeObj_gg92);
                }
                *pNextFreeObjPos = pObjFrame->objHeader_bd900.ohdNextFreeObj_gg92;
                //SAPDBERR_ASSERT_STATE (pPage->nd_first_free_obj() == OId.gg92GetPos());
                //pPage->nd_first_free_obj() = pObjFrame->objHeader_bd900.ohdNextFreeObj_gg92;

                /* update obj counter in page header  */
                -- pPage->nd_free_obj_cnt();
                ++ pPage->nd_occ_obj_cnt();
                /* rollforward the object */
                bd95_RollforwardObj (Trans, pPage, pObjFrame,  OId, ObjVers,  ObjFileNo,
                    AfterObjKeyLen, pAfterObjKey, bOIdIsReused, RedoEntrySequence, RedoUpdTransNo, ContVarObj );
            }

            /* there is no need to update the free page chain */
        }
        else if (obsFree_egg00 == pObjFrame->objHeader_bd900.ohdState_gg92)
        {                   
            if ( bOIdIsReused &&
                 ! bKeyedObjectWithoutKey ) // PTS 1122392 FF 30-May-2003
            {
                tgg92_KernelOid CurrOIdOwningTheKey;
                bd22GetOId (Trans, ObjFileNo, AfterObjKeyLen, pAfterObjKey, CurrOIdOwningTheKey, ErrorList);
                if (e_hash_key_not_found == TrError)
                {
                    bOIdIsReused = false;
                    TrError = e_ok;
                }
                else
                {
                    if (((CurrOIdOwningTheKey.gg92GetPos() == OId.gg92GetPos()) && 
                        (CurrOIdOwningTheKey.gg92GetPno() == OId.gg92GetPno())) &&
                        (e_ok == TrError))
                    {
                        Kernel_OpError(csp3_bd_msg, csp3_n_obj)
                            << __FILE__ << ":" << __LINE__ 
                            << " OID=" << OId.gg92GetPno() << "." 
                            << OId.gg92GetPos() << " (vers " << OId.gg92GetFrameVers() << ")" 
                            << "object state=obsFree but key referencing OID found within index";
                    }
                }
                if (e_ok == TrError)
                    bOIdIsReused = false;
            }

            /* remove the frame from the chain of free objects frames */
            tsp00_Int4         FreeObjCnt      = 0;
            tgg00_ObjPagePos * pNextFreeObjPos = &(pPage->nd_first_free_obj());

            /*   find the predecessor object in the free chain */  
            while (*pNextFreeObjPos != OId.gg92GetPos())
            {
                /* PTS 1127804 FF 2004-FEB-17 */
                if (((++FreeObjCnt) > pPage->nd_max_obj_cnt()) ||
                    ( *pNextFreeObjPos == NIL_OBJ_PAGE_POS_GG92 ) ||
                    ( NptrsPage.bd910ObjFramePtr(*pNextFreeObjPos)->objHeader_bd900.ohdState_gg92 != obsFree_egg00))
                {
                    Kernel_OpError ( csp3_bd_msg, csp3_n_obj )
                        << __FILE__ << ":" << __LINE__
                        << " bd95RollforwardObj_NewObj:" << " OID=" << OId.gg92GetPno() << "." 
                        << OId.gg92GetPos() << " (vers " << OId.gg92GetFrameVers() << ")" 
                        << " not found in freelist"
                        << " LoopCnt:" << FreeObjCnt
                        << "page dumped into: d" 
                        << NptrsPage.np_ptr()->nd_id() << ".obj"; 

                    b06dump_bad_page (Trans.trTaskId_gg00,'d', ".obj", NptrsPage.np_ptr()->nd_id(), 
                        (void *) NptrsPage.np_ptr(), 1);
                    g01abort (csp3_bd_msg, csp3_n_obj, "bd95RollFwdOID    root: ", pPage->nd_root());
                }
                //SAPDBERR_ASSERT_STATE (NptrsPage.bd910ObjFramePtr (*pNextFreeObjPos)->objHeader_bd900.ohdState_gg92 == obsFree_egg00);
                pNextFreeObjPos = &(NptrsPage.bd910ObjFramePtr (*pNextFreeObjPos)->objHeader_bd900.ohdNextFreeObj_gg92);
                //SAPDBERR_ASSERT_STATE ((++FreeObjCnt) <= pPage->nd_free_obj_cnt());
            }
            *pNextFreeObjPos = pObjFrame->objHeader_bd900.ohdNextFreeObj_gg92;

            /* rollforward the object */
            bd95_RollforwardObj (Trans, pPage, pObjFrame,  OId, ObjVers,  ObjFileNo,
                AfterObjKeyLen, pAfterObjKey, bOIdIsReused, RedoEntrySequence, RedoUpdTransNo, ContVarObj );

            /* update page header */
            -- pPage->nd_free_obj_cnt();
            ++ pPage->nd_occ_obj_cnt();

            fd_smart.ModifyEntryCount(Trans.trTaskId_gg00, converterVersion, 1);

            /* remove page from free page chain if there are no more free object frame on the page and           */
            /* update the empty page count on the root page if the page was empty before insering the new object */
            if ( NptrsPage.bd910IsEmptyPage())
            {
                /* bd910RemovePageFromChainOfAllPages will release subroot PTS 1135422 */
                NptrsSubRoot.bd910RemovePageFromChainOfAllPages(NptrsPage, Current.curr_trans->trTaskId_gg00,
                    converterVersion, fd_smart);  
            }
            else 
                if (NptrsPage.bd910PageMustBeRemovedFromFreeChain())
                    NptrsSubRoot.bd910RemovePageFromFreeChain(NptrsPage, converterVersion, fd_smart, false /*NoReInsert*/);
        }
        else
        {
            /* check whether this action was already performed during the */
            /* online operation and therefore it must not be repeated     */
            if ( ((Log_Transaction*) Trans.trLogContext_gg00)->WasOpenInLastSavepoint()
                 &&
                 ( obsReserved_egg00    == pObjFrame->objHeader_bd900.ohdState_gg92
                   ||
                   obsKeyReserved_egg00 == pObjFrame->objHeader_bd900.ohdState_gg92
                   ||
                   obsOccupied_egg00    == pObjFrame->objHeader_bd900.ohdState_gg92 )        // PTS 104113 UH 2005-08-11
                 &&                                                                          // Changed condition so that this New operation is ignored
                 pObjFrame->objHeader_bd900.ohdFrameVersion_gg92 == OId.gg92GetFrameVers() ) // if the current object is the one which was intended to be created.
            {
#               if defined(FOR_TESTING_ONLY)
                if ((AfterObjKeyLen>0) && (!bOIdIsReused))
                {
                    const tsp00_Int4  ObjBodySize = pPage->nd_obj_frame_len() - sizeof(tgg92_ObjHeader);

                    Kernel_OpError(csp3_bd_msg, csp3_n_obj)
                        << __FILE__ << ":" << __LINE__ 
                        << " operation already done online, but index-entry missing";
                    /* move object key into object frame */
                    SAPDB_RangeMove( __FILE__, 1, 
                        AfterObjKeyLen, ObjBodySize,
                        pAfterObjKey, 1, &(pObjFrame->objBody_bd900), ObjBodySize - AfterObjKeyLen + 1, 
                        AfterObjKeyLen, TrError);  
                    if (TrError != e_ok) return;  
                    
                    /* insert object key into key tree */
                    tgg92_KernelOid DummyFoundOId;
                    bd22InsertOId (Trans, ObjFileNo, AfterObjKeyLen, pAfterObjKey,  OId, DummyFoundOId);
                }
#               endif
                return;
            }
            
            bd900GetFileNo(NptrsPage.np_ptr(), lContainerNo);

            TrError = e_wrong_object_state;
            ErrorTraceObject.WriteVersionErrorMessage (
                lContainerNo,
                ObjVers, pObjFrame);
            return;       
        }
    }
    else
    {  
        /* before the page can be used it must be initialized */
        /* and integrated into the wanted object file         */        
        const tsp00_PageNo    SubRoot   = fd_smart.GetRootPage(); 
        
        /* get shortcut to the object page */
        tbd_nodeptr &pPage  = NptrsPage.np_ptr();

        /* integrate the new page into the object container */
        const bool bPageMustNotBeCreated = true;
        const Converter_Version &converterVersion = Converter_ICommon::Instance().Version();
        NptrsPage.bd910AssignToNewPageInDataChain (NptrsSubRoot, false /*insert into freelist*/,
			Trans.trTaskId_gg00,  fd_smart, converterVersion, bPageMustNotBeCreated);
        if (TrError != e_ok) return;

        /* get pointer to the wanted object frame */
        BD92CHECK_OID_POSITION ("bd95RollforwardObj_NewObj",TrError, OId, NptrsPage);
        tbd900_ObjFrame  *pObjFrame = NptrsPage.bd910ObjFramePtr (OId.gg92GetPos());

        /* update page header */
        -- pPage->nd_free_obj_cnt();
        ++ pPage->nd_occ_obj_cnt();

        /* remove the frame from the chain of free objects frames */
        if (OId.gg92GetPos() == pPage->nd_first_free_obj())
            pPage->nd_first_free_obj() = pObjFrame->objHeader_bd900.ohdNextFreeObj_gg92;
        else
        {
#           if COMPILEMODE_MEO00 >= QUICK_MEO00 
            BD92CHECK_OBJ_POSITION ("bd95RollforwardObj_NewObj",TrError, OId.gg92GetPos() - BD92ALIGN_OBJ_FRAME(pPage->nd_obj_frame_len()), NptrsPage);
#           endif   
            tbd900_ObjFrame  *pPreObjFrame = NptrsPage.bd910ObjFramePtr (OId.gg92GetPos() - BD92ALIGN_OBJ_FRAME(pPage->nd_obj_frame_len()));

            SAPDBERR_ASSERT_STATE (pPreObjFrame->objHeader_bd900.ohdNextFreeObj_gg92 == OId.gg92GetPos());

            pPreObjFrame->objHeader_bd900.ohdNextFreeObj_gg92 = pObjFrame->objHeader_bd900.ohdNextFreeObj_gg92;
        }

        /* rollforward the object */
        bd95_RollforwardObj (Trans, pPage, pObjFrame,  OId, ObjVers,  ObjFileNo,
            AfterObjKeyLen, pAfterObjKey, bOIdIsReused, RedoEntrySequence, RedoUpdTransNo, ContVarObj);
        if (TrError != e_ok) 
            return;

        /* PTS 1127493 FF 2004-FEB-02 */ 
        /* chaining not done within bd910AssignToNewPageInDataChain if bPageMustNotBeCreated = true */
        /* this has been changed to avoid above dechaining from page freelist to get wrong          */
        if ( NptrsPage.bd910PageMustBeInsertedIntoFreeChain())
            NptrsSubRoot.bd910InsertPageIntoFreeChain (NptrsPage, converterVersion, fd_smart);   

        /* remove page from free page chain if there are no more free object frame on the page */
        NptrsSubRoot.bd910RestoreFreePageChain (NptrsPage, converterVersion, fd_smart);
        fd_smart.ModifyEntryCount(Trans.trTaskId_gg00, converterVersion, 1);
    }
    if  ((e_ok != Trans.trError_gg00) && ! ErrorList.IsEmpty())
        RTE_Message(ErrorList);
}

/*---------------------------------------------------------------------------*/

externCpp  void 
bd95Rollforward_UpdObj (tgg00_TransContext       &Trans,
                        const tgg92_KernelOid    &OId,
                        const tgg91_PageRef      &ObjVers,
                        const FileDir_FileNo     &ObjFileNo,
                        tsp00_Int4                AfterObjLen,
                        void                     *pAfterObj,
                        const Log_EntrySequence  &RedoEntrySequence,
                        const tgg91_TransNo      &RedoUpdTransNo,
                        SAPDB_Int1                contVarObj,            /* PTS 1127661 FF */
                        bool                      bReused)               /* PTS 1127661 FF */
{
    SAPDBTRACE_ROUTINE_DEBUG("bd95Rollforward_UpdObj", ObjContainer_Trace, 5);    
    
    Msg_List ErrorList;

    /* this function can be called only during a restart */
    SAPDBERR_ASSERT_STATE (Kernel_IAdminInfo::Instance().KernelStateIsRedoLogProcessing());
    
    /* create file id and current for the file containing the object to be updated */
    cbd05_Current  Current (Trans, m_update);           
    FileDir_Oms         fd_smart;
    FileDir_IDirectory &IFDC  = FileDir_IDirectory::SessionInstance(Trans);
    FileDir_ReturnCode  fd_rc = IFDC.GetFile(Trans.trTaskId_gg00, ObjFileNo, fd_smart, ErrorList);
    if (FileDir_Okay != fd_rc) {
        bd900MapFileDirError(Trans, fd_rc, e_ok, (char*)__FILE__,  __LINE__ );
        return;
    }

    fd_smart.BuildFileId(Current.curr_tree_id);

    /* this object writes a message into the vtrace at beginning and end of this routine */
    LVC_ErrorTraceObject   ErrorTraceObject  (Trans, Current.curr_tree_id, ObjFileNo, OId, b95redo_upd);
    if ( Data_FileDeleted == fd_smart.GetFileState() )
    {
        ErrorTraceObject.SetStatus (rosFileAlreadyDeleted_gg12);
        return;
    }
 
	tgg92_KernelOid AuxOId     = OId;      // if it would exist a special rollforward call instead of bd90UpdateObj
	tgg91_PageRef   AuxObjVers = ObjVers;  // then these two assignments could be removed. UH 2005-06-14 (TODO)
    
    bd90UpdateObj (Current, AuxOId, AuxObjVers, AfterObjLen, pAfterObj, RedoEntrySequence, RedoUpdTransNo, 
        contVarObj, bReused, true/*bForRollforward*/, ErrorList);
    if  ((e_ok != Trans.trError_gg00) && ! ErrorList.IsEmpty())
        RTE_Message(ErrorList);
}


/*---------------------------------------------------------------------------*/

externCpp void
bd95RemovePageFromObjFile (tgg00_TransContext       &Trans,
                           const Converter_Version  &converterVersion,
                           cbd910_ObjDataNode       &NptrsPage)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd95RemovePageFromObjFile", ObjContainer_Trace, 5);    

    SAPDBERR_ASSERT_STATE (Kernel_IAdminInfo::Instance().KernelStateIsRedoLogProcessing());

    tbd900_ObjFrame          *pObjFrame; 
    tgg92_KernelOid           OId;
    const tsp00_Int4          ObjFrameLen        = NptrsPage.np_ptr()->nd_obj_frame_len();
    const tsp00_Int4          AlignedObjFrameLen = BD92ALIGN_OBJ_FRAME(ObjFrameLen);  
    tgg00_BasisError         &TrError            = Trans.trError_gg00;
    Msg_List                 &ErrorList          = Trans_Context::GetContext(Trans).ErrorList();

    FileDir_FileNo ObjFileNo;
    bd900GetFileNo(NptrsPage.np_ptr(), ObjFileNo);
    const SAPDB_Bool    bConsistent = true;
    FileDir_Oms         fd_smart;
    FileDir_IDirectory &IFDC  = FileDir_IDirectory::SessionInstance(Trans);
    FileDir_ReturnCode  fd_rc = IFDC.GcGetFile(Trans.trTaskId_gg00, ObjFileNo, fd_smart, ErrorList);
    if (FileDir_Okay != fd_rc) {
        bd900MapFileDirError(Trans, fd_rc, e_ok, (char*)__FILE__,  __LINE__ );
        NptrsPage.bd910ReleasePage();
        return;
    }
    cbd05_Current  Current (Trans, m_update);           
    fd_smart.BuildFileId(Current.curr_tree_id);
    if ((NptrsPage.np_ptr()->nd_id() == NptrsPage.np_ptr()->nd_root())
        ||
        (Data_FileDeleted == fd_smart.GetFileState()))
    {
       if (Data_FileDeleted != fd_smart.GetFileState())
        {
            {
                Kernel_OpError(csp3_bd_msg, csp3_n_obj)
                    << __FILE__ << ":" << __LINE__ 
                    << " bd95RemovePageFromObjFile: root needed, but file not marked deleted" 
                    << " Filestate:" << fd_smart.GetFileState()
                    << " page dumped into: d" 
                    << NptrsPage.np_ptr()->nd_id() << ".obj"; 
            }
            b06dump_bad_page (Trans.trTaskId_gg00,'d', ".obj", 
                NptrsPage.np_ptr()->nd_id(), 
                (void *) NptrsPage.np_ptr(), 1);
            eo670_CTraceStack();
            g01abort (csp3_bd_msg, csp3_n_obj, 
                "bd95RmvPagFObjFil: Pno: ", NptrsPage.np_ptr()->nd_id());
        }

        /* removing a root or a subroot page from a file requires the removal of the complete  */
        /* file  since roots and subroots are the anchor page for all other pages of the file  */
        /* which cannot be found after the root or a subroot are deleted                       */ 
        /* since a root or a subroot can be removed only when the file ist deleted it is not   */
        /* necessary to drop the history of each deleted object since the garbage collector    */
        /* ignores for deleted files all errors which occur when it wants to remove an object  */
        /* for which exists an history entry but no data  object                               */
        
        fd_smart.Detach();
        NptrsPage.bd910ReleasePage();

        // PTS 1112228 AK 19/10/2001 FF Merge 7.4.01 -> 7.4.02 29/11/2001
        bd95RemoveObjFile(Current);
    }
    else
    {
        /*----------------------------------------------------------*/
        /* remove history entries, keys and continuation objects    */
        /* belonging to the the feot objects on this page           */
        /*----------------------------------------------------------*/
        
        /* create a new page handle with the appropriate  current     */
        /* and release the old page handle. PTS 1112198 AK 18/10/2001 */
        tsp00_PageNo PageNoToRemove = NptrsPage.np_ptr()->nd_id();
        tsp00_PageNo SubRootPageNo  = NptrsPage.np_ptr()->nd_root();
        NptrsPage.bd910ReleasePage();     

        cbd910_SubRootNode    NptrsSubRoot  (Current,SubRootPageNo, nr_for_update);
        if (TrError != e_ok )
        {
            /* PTS 1124681 FF 2003-OCT-15 */
            /* page may already have been released by another rollforward_new   */
            /* it even may already belong now to another container              */
            if (( TrError == e_no_converter_entry ) ||
                ( TrError == e_page_in_wrong_tree ))
                TrError = e_ok;
            else
                Kernel_OpWarning(csp3_bd_msg, csp3_n_obj)
                << __FILE__ << ":" << __LINE__ 
                << " create new page handle for subroot failed:" << TrError
                << " PNo:" << SubRootPageNo; 
            return;
        }
        
        cbd910_PrimObjDataNode  NptrsPage2 (Current, PageNoToRemove, nr_for_update);
        if (TrError != e_ok ) 
        {
            /* PTS 1124681 FF 2003-OCT-15 */
            /* page may already have been released by another rollforward_new   */
            /* it even may already belong now to another container              */
            if (( TrError == e_no_converter_entry ) ||
                ( TrError == e_page_in_wrong_tree ))
                TrError = e_ok;
            else
                Kernel_OpWarning(csp3_bd_msg, csp3_n_obj)
                << __FILE__ << ":" << __LINE__             
                << " create new page handle for page to remove failed:" << TrError 
                << " PNo:" << PageNoToRemove; 
            return;
        }

        for (tgg00_ObjPagePos ObjPos = NptrsPage2.np_ptr()->nd_first_obj();
        ((ObjPos + ObjFrameLen) <= BODY_END_BD00) && 
            (NptrsPage2.np_ptr()->nd_free_eot_obj_cnt() > 0);             
        ObjPos += AlignedObjFrameLen)
        { 
            /*  get pointer to the current object frame  */
            pObjFrame = NptrsPage2.bd910ObjFramePtr (ObjPos);            
            
            /* check the object frame state */
            if ((pObjFrame->objHeader_bd900.ohdState_gg92 != obsFreeAfterEot_egg00) &&
                (pObjFrame->objHeader_bd900.ohdState_gg92 != obsFree_egg00))
            {
                {
                    Kernel_OpError(csp3_bd_msg, csp3_n_obj)
                        << __FILE__ << ":" << __LINE__ 
                        << " wrong object state: " 
                        << pObjFrame->objHeader_bd900.ohdState_gg92
                        << "page dumped into: d" 
                        << NptrsPage2.np_ptr()->nd_id() << ".obj"; 
                }
                b06dump_bad_page (Trans.trTaskId_gg00,'d', ".obj", 
                    NptrsPage2.np_ptr()->nd_id(), 
                    (void *) NptrsPage2.np_ptr(), 1);
                eo670_CTraceStack();
                g01abort (csp3_bd_msg, csp3_n_obj, 
                    "bd95RmvPagFObjFil: Pno: ", NptrsPage2.np_ptr()->nd_id());
            };
            
            if (pObjFrame->objHeader_bd900.ohdState_gg92 == obsFreeAfterEot_egg00) 
            {
                /* drop object history */
                OId.gg92Set (NptrsPage2.np_ptr()->nd_id(), ObjPos, pObjFrame->objHeader_bd900.ohdFrameVersion_gg92);
                
                /* create an object that logs errors and reports them into the vtrace  during its destruction */
                LVC_ErrorTraceObject   ErrorTraceObject  (Trans, Current.curr_tree_id, ObjFileNo, OId, b95removeobj);
                
                kb67DropObjHistory (*Current.curr_trans, pObjFrame->objHeader_bd900, OId, ErrorList);
                if (e_obj_history_not_found == TrError) // PTS 1113251 UH 2001-12-27
                    TrError = e_ok;
                if (TrError != e_ok)
                { 
                    if  (! ErrorList.IsEmpty())
                        RTE_Message(ErrorList);
                    return;
                }

                /* clear the object frame, i.e. remove the key or the continuation object  */
                const bool bKeyAlreadyRemoved = true; // new PTS 1128894 FF 31-MAR-2004
                bd91RemoveObjFromPage (Current, NptrsPage2, pObjFrame,  OId, converterVersion, fd_smart, 
                    ErrorTraceObject, ! bKeyAlreadyRemoved);
                if (TrError != e_ok) return;            
            }
        }

        /* check that the page was completely cleared  PTS 112198 FF 7.4.01 -> 7.4.02 Merge 29/11/2001 */
        if (!NptrsPage2.bd910IsEmptyPage())
        {
            {
                Kernel_OpError(csp3_bd_msg, csp3_n_obj)
                    << __FILE__ << ":" << __LINE__ 
                    << "bd95RemovePageFromObjFile: nd_free_obj_cnt != nd_max_obj_cnt " 
                    << "page dumped into: d" 
                    << NptrsPage2.np_ptr()->nd_id() << ".obj"; 
            }
            b06dump_bad_page (Trans.trTaskId_gg00,'d', ".obj", 
                NptrsPage2.np_ptr()->nd_id(), 
                (void *) NptrsPage2.np_ptr(), 1);
            eo670_CTraceStack();
            g01abort (csp3_bd_msg, csp3_n_obj, 
                "bd95RmvPagFObjFil: Pno: ", NptrsPage2.np_ptr()->nd_id());
        }
        
        /*--------------------------------------------------------------------*/
        /* if the page has to be an empty page by removing the objects        */ 
        /* and so it can be released                                          */
        /*--------------------------------------------------------------------*/
        /* bd910RemovePageFromChainOfAllPages will release subroot PTS 1135422 */
        if (!NptrsSubRoot.bd910RemovePageFromChainOfAllPages(NptrsPage2, 
            Current.curr_trans->trTaskId_gg00, converterVersion, fd_smart))
        {        
            /* check if this error occured since the file was dropped */
            if (e_ok != TrError)
            {
                /* reread the object fide directory */
                FileDir_ReturnCode  fd_rc = IFDC.GetFile(
                    Trans.trTaskId_gg00, ObjFileNo, fd_smart, ErrorList);
                if (FileDir_Okay != fd_rc) 
                    bd900MapFileDirError(Trans, fd_rc, e_ok, (char*)__FILE__,  __LINE__ );
            }

            if (e_ok != TrError)
            {
                {
                    Kernel_OpError(csp3_bd_msg, csp3_n_obj)
                        << __FILE__ << ":" << __LINE__ 
                        << " page could't be removed. Error:" << TrError
                        << " page dumped into: d" 
                        << NptrsPage2.np_ptr()->nd_id() << ".obj"; 
                }
                b06dump_bad_page (Trans.trTaskId_gg00,'d', ".obj", 
                    NptrsPage2.np_ptr()->nd_id(), 
                    (void *) NptrsPage2.np_ptr(), 1);
                eo670_CTraceStack();
                g01abort (csp3_bd_msg, csp3_n_obj, 
                    "bd95RmvPagFObjFil: Pno: ", NptrsPage2.np_ptr()->nd_id());
            };
        }
    }
    if  ((e_ok != Trans.trError_gg00) && ! ErrorList.IsEmpty())
        RTE_Message(ErrorList);
}

/*---------------------------------------------------------------------------*/
/* PTS 1132537 FF */
externCpp void
bd95RemoveObjFile (cbd05_Current  &Current )
{
    SAPDBTRACE_ROUTINE_DEBUG("bd95_RemoveObjFile", ObjContainer_Trace, 5);    
    
    FileDir_FileNo ObjFileNo = Current.curr_tree_id.fileDirFileId_gg00();

    SAPDB_Bool            bDoSuspend = false;
    vbegexcl (Current.curr_trans->trTaskId_gg00, g08garbage);
        bd91GarbCollController.InsertIntoWaitForDelObjFileQueue(Current.curr_trans->trTaskId_gg00, ObjFileNo);
        bDoSuspend = true;
    vendexcl (Current.curr_trans->trTaskId_gg00, g08garbage);
    bd91StartOMSGarbageCollection (*Current.curr_trans);
    
    //=============================================================================
    if (bDoSuspend) 
        vsuspend (Current.curr_trans->trTaskId_gg00, 2 );
    //=============================================================================
}

/*===========================================================================*
 *  LOCAL FUNCTIONS (CODE)                                                   *
 *===========================================================================*/

static _INLINE void
bd95_RemoveAllPagesFromObjFile (cbd05_Current   &Current, 
                                cbd910_RootNode &NptrsRoot)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd95_RemoveAllPagesFromObjFile", ObjContainer_Trace, 5);    
    
    /* this function removes all pages from the object file except the root page */
    /* note that the root and subroot of the first chain are identically         */  
    
    tgg00_BasisError  &TrError     = Current.curr_trans->trError_gg00;
    tsp00_PageNo       RightPageNo = NptrsRoot.np_ptr()->nd_right();
    
    while (RightPageNo != NIL_PAGE_NO_GG00)
    {
        cbd910_ObjNode NptrsPage (Current, RightPageNo, nr_for_update);
        if (e_ok != TrError) 
            return;
        RightPageNo = NptrsPage.np_ptr()->nd_right();
        NptrsPage.bd910FreePage();
    }
    NptrsRoot.bd910FreePage();
}

/*---------------------------------------------------------------------------*/

static _INLINE void
bd95_RollforwardObj (tgg00_TransContext       &Trans,
                     tbd_nodeptr               pPage,
                     tbd900_ObjFrame          *pObjFrame,
                     tgg92_KernelOid           OId,
                     const tgg91_PageRef      &ObjVers,
                     FileDir_FileNo           &ObjFileNo,
                     tsp00_Int4                AfterObjKeyLen,
                     void                     *pAfterObjKey,
                     const tsp00_Bool          bOIdIsReused,
                     const Log_EntrySequence  &RedoEntrySequence,
                     const tgg91_TransNo      &RedoUpdTransNo,
                     SAPDB_Int1                ContVarObj)       /* PTS 1127661 FF */
{
    SAPDBTRACE_ROUTINE_DEBUG("bd95_RollforwardObj", ObjContainer_Trace, 5);    

    tgg00_BasisError    &TrError = Trans.trError_gg00;
    Msg_List             ErrorList;
     
    SAPDBERR_ASSERT_STATE ((tsp00_Byte *) pObjFrame == (((tsp00_Byte *) pPage) + OId.gg92GetPos()));
    SAPDBERR_ASSERT_STATE (pPage->nd_obj_key_len() == AfterObjKeyLen);
    SAPDBERR_ASSERT_STATE ((AfterObjKeyLen == 0) || (pt2VarObject_egg00 != pPage->nd_pt2()));
    SAPDBERR_ASSERT_STATE (((obsFree_egg00         == pObjFrame->objHeader_bd900.ohdState_gg92)) ||
                           ((obsFreeAfterEot_egg00 == pObjFrame->objHeader_bd900.ohdState_gg92) && ( bOIdIsReused)))

    /* check if the object can be rolled forward and update the transinfo in the object frame header */
    if (!kb67RollforwardObj(pObjFrame->objHeader_bd900, RedoUpdTransNo, RedoEntrySequence, ObjVers))
    {
       TrError = e_wrong_object_version;
       return;
    }

    
    /* set the object frame header to the state reserved and initialize the body */
    pObjFrame->bd900SetToReservedFrame (pPage->nd_obj_frame_len(), AfterObjKeyLen,
        OId.gg92GetFrameVers(), (pt2VarObject_egg00 == pPage->nd_pt2()));

    /* insert key into object frame and key tree */
    if ((AfterObjKeyLen>0) && (!bOIdIsReused))
    {
        const tsp00_Int4  ObjBodySize = pPage->nd_obj_frame_len() - sizeof(tgg92_ObjHeader);

		/* move object key into object frame */
        SAPDB_RangeMove( __FILE__, 1, 
            AfterObjKeyLen, ObjBodySize,
            pAfterObjKey, 1, &(pObjFrame->objBody_bd900), ObjBodySize - AfterObjKeyLen + 1, 
            AfterObjKeyLen, TrError);  
        if (TrError != e_ok) 
            return;  

        /* insert object key into key tree */
        tgg92_KernelOid DummyFoundOId;
        bd22InsertOId (Trans, ObjFileNo, AfterObjKeyLen, pAfterObjKey,  OId, DummyFoundOId, ErrorList);
    }

    if (pt2VarObject_egg00 == pPage->nd_pt2())
    {
        tbd900_VarObjBodyHeader *pHdr = &(((tbd900_VarObjBody*) pObjFrame->objBody_bd900)->vbdHeader_bd900);
        pHdr->vhdObjectIsCont_bd900 = ContVarObj;
    }
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
