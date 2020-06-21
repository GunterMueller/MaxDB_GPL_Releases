/*!**************************************************************************

  module:       vbd22.cpp
  special area: OMS_KeyManagement
  responsible:  UweH
  last changed: 1999-11-05  17:00
  version:      7.2.0.0
  see also: 
  copyright:    Copyright (c) 1998-2005 SAP AG
  description:  Map an object key to an OID, there is no consistent-check done.



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

#ifndef HBD22_H
#   include "hbd22.h"
#endif
#ifndef GGG01_H
#   include "ggg01.h" // OMS errors
#endif
#ifndef GGG12_H
#   include "ggg12.h"
#endif
#ifndef GBD900_H
#   include "gbd900.h"
#endif
#ifndef GSP03_3_H
#   include "gsp03_3.h"
#endif
#ifndef HBD01_H
#   include "hbd01.h"
#endif
#ifndef HBD03_H
#   include "hbd03.h"
#endif
#ifndef HBD90_H
#   include "hbd90.h"
#endif
#ifndef HBD911_H
#   include "hbd911.h"
#endif
#ifndef HBD92_H
#   include "hbd92.h"
#endif
#ifndef HGG01_1_H
#   include "hgg01_1.h"
#endif
#ifndef HGG01_3_H
#   include "hgg01_3.h"
#endif
#ifndef TRACE_ENTRY_HPP
#   include "Trace/Trace_Entry.hpp"
#endif
#ifndef KERNEL_VTRACE_HPP
#   include "KernelCommon/Kernel_VTrace.hpp"           
#endif
#ifndef KERNEL_OPMSG_HPP
#   include "KernelCommon/Kernel_OpMsg.hpp"            
#endif
#ifndef SAPDB_MEMCOPYMOVE_HPP
#   include "SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp"
#endif
#ifndef MSG_LIST_HPP
#   include "Messages/Msg_List.hpp"
#endif
#ifndef FILEDIR_IDIRECTORY_HPP
#   include "FileDirectory/FileDir_IDirectory.hpp"
#endif
#ifndef FILEDIR_OMSKEYPARTITION_HPP
#   include "FileDirectory/FileDir_OmsKeyPartition.hpp"
#endif
#ifndef RTESYS_MEMORYBARRIER_H
#   include "RunTime/System/RTESys_MemoryBarrier.h"
#endif

#if COMPILEMODE_MEO00 >= SLOW_MEO00 
#include "hta99.h"
#endif

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

//PTS 1112190 18/10/2001
#define CHECK_NUM_KEY_FILES( NUM_KEY_FILES) \
  if ((NUM_KEY_FILES) == 0)\
  {\
    Kernel_OpError( csp3_bd_msg, csp3_n_obj )\
    << __FILE__ << ":" << __LINE__\
    << " number of key partitions == 0 FileNo:" << fd_smart.GetFileNo();\
    Trans.trError_gg00 = e_illegal_filename;\
    return;\
  }

/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/

/*===========================================================================*
 *  LOCAL VARIABLES                                                          *
 *===========================================================================*/

static const bool bExclusive = true;

/*===========================================================================*
 *  LOCAL FUNCTIONS                                                          *
 *===========================================================================*/

inline static void 
bd22_GetOId (tgg00_TransContext   &Trans,
             tgg00_FileId          ObjKeyFileId,
             tsp00_Int4            KeyLen,
             void                 *pKey,
             tgg92_KernelOid      &Oid)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd22_GetOid", ObjContainer_Trace, 5);

    tgg00_BasisError &TrError = Trans.trError_gg00;
    if (TrError != e_ok) return;

    tsp00_Int4 DummyOIdLen;
    bd03OmsGetInv (Trans, ObjKeyFileId, tsp00_BytePtr (pKey), KeyLen, sizeof(Oid), tsp00_BytePtr(&Oid), DummyOIdLen);
    switch (TrError)
    {
    case e_ok                :  SAPDBERR_ASSERT_STATE (DummyOIdLen == sizeof(tgg92_KernelOid));break;
    case e_inv_list_not_found:
    case e_key_not_found     :  
        TrError = e_hash_key_not_found; 
        Oid.gg92SetNil();           /* PTS 1116881 to avoid that error-tracing uses uninitialized OID */
        break;
    case e_inv_not_found     :  
        TrError = e_hash_not_found;     
        Oid.gg92SetNil();           /* PTS 1116881 to avoid that error-tracing uses uninitialized OID */
        break;
    }
};

/*-----------------------------------------------------------------------------------*/
externCpp tsp00_Int4
bd22ObjKeyFileIndex (const tsp00_Int4      KeyLen,
                     void                 *pKey,
                     const tsp00_Uint4     NumHashValues)
{
    /* this function hashes a key onto an integer value h with 0<= h <  NumHashValues */
    
    tsp00_Int4 sum = 0;
    for (tsp00_Int4 i=0; i<KeyLen; ++i)
        sum += *((char *) pKey + i);
    
    return (sum % NumHashValues);
}

/*===========================================================================*
 *  GLOBAL FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

externCpp void 
bd22CreateObjKeyFiles (tgg00_TransContext    &Trans,
                       FileDir_FileNo        &Fileno,
                       tsp00_Int4             NumObjKeyFiles,
                       const LVC_FileNoArray &FileNoArray,
                       Msg_List              &ErrorList)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd22CreateObjKeyFile", ObjContainer_Trace, 5);

    FileDir_OmsKey    fd_smart;
    if ( !bd90GetKeyedFile (Trans, fd_smart, Fileno, ErrorList ))
        return;

    FileDir_IDirectory &IFDC  = FileDir_IDirectory::SessionInstance(Trans);
    for (tsp00_Int4 iKeyFile=0; 
        (iKeyFile < NumObjKeyFiles) && (e_ok == Trans.trError_gg00); 
        ++iKeyFile)
    {
        tgg00_FileId            ObjKeyFileId;
        tgg00_ObjFileType       FileType;
        FileType.becomes(oftUnknownType_egg00);

        IFDC.BuildNewOmsFileId(FileNoArray[iKeyFile], FileType, iKeyFile, ObjKeyFileId);
        
        bd03CreateOmsKeyPartitionFile( Trans, ErrorList, ObjKeyFileId, FileNoArray[iKeyFile], 
            Fileno, iKeyFile + 1, SAPDB_TRUE /*addToFileDir*/ );
    }
}

/*-----------------------------------------------------------------------------------*/

externCpp void 
bd22Rollforward_CreateObjKeyFiles (tgg00_TransContext    &Trans,
                                   FileDir_FileNo        &Fileno,
                                   tsp00_Int4             NumObjKeyFiles,
                                   const LVC_FileNoArray &FileNoArray,
                                   Msg_List              &ErrorList)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd22Rollforward_CreateObjKeyFiles", ObjContainer_Trace, 5);

    FileDir_OmsKey    fd_smart;
    if ( !bd90GetKeyedFile (Trans, fd_smart, Fileno, ErrorList ))
        return;

    FileDir_IDirectory &IFDC  = FileDir_IDirectory::SessionInstance(Trans);
    for (tsp00_Int4 iKeyFile=0; 
        (iKeyFile < NumObjKeyFiles) && (e_ok == Trans.trError_gg00); 
        ++iKeyFile)
    {
        tgg00_FileId      ObjKeyFileId;
        tgg00_ObjFileType FileType;
        FileType.becomes(oftUnknownType_egg00);

        IFDC.BuildNewOmsFileId(FileNoArray[iKeyFile], FileType, iKeyFile, ObjKeyFileId);
        
        bd03CreateOmsKeyPartitionFile( Trans, ErrorList, ObjKeyFileId, FileNoArray[iKeyFile],
            Fileno, iKeyFile + 1, SAPDB_TRUE /*addToFileDir*/ );
        
        if ( e_duplicate_filename == Trans.trError_gg00 ) // PTS 1137268 UH 2005-08-10
        {
            Trans.trError_gg00 = e_ok;
            // root could be checked here
        }
    }
}

/*-----------------------------------------------------------------------------------*/
/* PTS 1107153 AK 17/07/2000 */

externCpp void
bd22GetLeaveCountOfObjKeyFiles (tgg00_TransContext        &Trans,
                                const FileDir_FileNo      &Fileno,
                                FileDir_Oms               &fd_smart,
                                tsp00_Int4                &NumKeyLeaves,
                                Msg_List                  &ErrorList)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd22GetLeaveCountOfObjKeyFiles", ObjContainer_Trace, 5);

    const tsp00_Bool       cCountLeavesOnly = true;
    tsp00_Int4             DummyAvgPrimKeyCountPerPage;
    tsp00_Int4             NumKeyLeavesOfOneFile;
    NumKeyLeaves = 0;          

    FileDir_IDirectory &IFDC  = FileDir_IDirectory::SessionInstance(Trans);

    for (tsp00_Uint4 iKeyFile=0; 
        (iKeyFile <  fd_smart.GetKeyPartitionCount() && ( e_ok == Trans.trError_gg00 )); 
        ++iKeyFile)
    {
        FileDir_OmsKeyPartition   fd_keypart_smart;
        FileDir_FileNo KeyFileno  = fd_smart.GetKeyPartition(iKeyFile + 1);
        FileDir_ReturnCode  fd_rc = IFDC.GetFile(
            Trans.trTaskId_gg00, KeyFileno, fd_keypart_smart, ErrorList);
        if (FileDir_Okay != fd_rc) {
            bd900MapFileDirError(Trans, fd_rc, e_wrong_obj_file_id, (char*)__FILE__,  __LINE__ );
            return;
        }     
        tgg00_FileId  ObjKeyFileId;
        fd_keypart_smart.BuildFileId(ObjKeyFileId);
        bd03StatisticsUpdate (Trans, ObjKeyFileId, NumKeyLeavesOfOneFile,
            DummyAvgPrimKeyCountPerPage, cCountLeavesOnly);
        NumKeyLeaves += NumKeyLeavesOfOneFile;
    }   
}

/*-----------------------------------------------------------------------------------*/

externCpp void
bd22ReleaseObjKeyFiles (tgg00_TransContext     &Trans,
                        FileDir_FileNo         &Fileno,
                        FileDir_Oms            &fd_smart,
                        Msg_List               &ErrorList)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd22ReleaseObjKeyFiles", ObjContainer_Trace, 5);
    
    FileDir_IDirectory &IFDC  = FileDir_IDirectory::SessionInstance(Trans);

#   if COMPILEMODE_MEO00 >= QUICK_MEO00
    int numKeyFiles = 0;
    numKeyFiles = fd_smart.GetKeyPartitionCount();
    if (numKeyFiles <= 0) 
    {
        Kernel_OpError( csp3_bd_msg, csp3_n_obj )
            << "bd22ReleaseObjKeyFiles (0 == GetKeyPartitionCount()) Fileno:" << Fileno; 
        g01abort (csp3_bd_msg, csp3_n_obj, "BD22RelKeyFile:NoKeyFile", 0);
    }
#   endif //COMPILEMODE_MEO00 >= QUICK_MEO00
    
    for (tsp00_Uint4 iKeyFile=0; 
        ((iKeyFile < fd_smart.GetKeyPartitionCount()) && ( e_ok == Trans.trError_gg00 ));
        ++iKeyFile)
    {
        FileDir_OmsKeyPartition   fd_keypart_smart;
        FileDir_FileNo KeyFileno  = fd_smart.GetKeyPartition(iKeyFile + 1);
        FileDir_ReturnCode  fd_rc = IFDC.GcGetFile(Trans.trTaskId_gg00, KeyFileno, fd_keypart_smart, ErrorList);
        if (FileDir_Okay != fd_rc) {
            bd900MapFileDirError(Trans, fd_rc, e_wrong_obj_file_id, (char*)__FILE__,  __LINE__ );
            return;
        }     
        tgg00_FileId  ObjKeyFileId;
        fd_keypart_smart.BuildFileId(ObjKeyFileId);
        bd03ReleaseInvTree(Trans, ObjKeyFileId);
        fd_rc = IFDC.DelFile(Trans.trTaskId_gg00, KeyFileno, ErrorList);
        if (FileDir_Okay != fd_rc) 
        {
            if ( fd_rc == FileDir_FileNoNotFound ) {
                Trans.trError_gg00 = e_file_not_found;
            } else 
                bd900MapFileDirError(Trans, fd_rc, e_wrong_obj_file_id, (char*)__FILE__,  __LINE__ );
        }
        if (e_ok != Trans.trError_gg00)
        {
            Kernel_OpError( csp3_bd_msg, csp3_n_obj )
                << __FILE__ << ":" << __LINE__
                << " FileNo:" << Fileno
                << " FileIndex:" << iKeyFile
                << " PartFileNo:" << KeyFileno
                << " basis_error:" << Trans.trError_gg00;
            return;
        }
    }
}

/*-----------------------------------------------------------------------------------*/

externCpp void 
bd22GetOId (tgg00_TransContext  &Trans,
            FileDir_FileNo      &Fileno,
            tsp00_Int4           KeyLen,
            void                *pKey,
            tgg92_KernelOid     &Oid,
            Msg_List            &ErrorList)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd22GetOid", ObjContainer_Trace, 5);
     
    tgg00_BasisError &TrError = Trans.trError_gg00;

    FileDir_OmsKey    fd_smart;
    if ( !bd90GetKeyedFile (Trans, fd_smart, Fileno, ErrorList ))
        return;

    cbd911RecreateIndexLock lock(Trans);   
    if (lock.IsNeeded(Fileno)) 
        lock.Lock( !bExclusive );

    FileDir_IDirectory &IFDC  = FileDir_IDirectory::SessionInstance(Trans);
    const tsp00_Int4    NumObjKeyFiles = fd_smart.GetKeyPartitionCount();
    CHECK_NUM_KEY_FILES (NumObjKeyFiles); //PTS 1112190 18/10/2001
    tsp00_Int4 ObjKeyFileIndex = (NumObjKeyFiles == 1 ? 0 : bd22ObjKeyFileIndex (KeyLen, pKey, NumObjKeyFiles)); 
    FileDir_FileNo KeyFileno = fd_smart.GetKeyPartition(ObjKeyFileIndex + 1);
    FileDir_OmsKeyPartition   fd_keypart_smart;
    FileDir_ReturnCode fd_rc = 
        IFDC.GetFile(Trans.trTaskId_gg00, KeyFileno, fd_keypart_smart, ErrorList);
    if (FileDir_Okay != fd_rc) {
        bd900MapFileDirError(Trans, fd_rc, e_wrong_obj_file_id, (char*)__FILE__,  __LINE__ );
        return;
    }     
    tgg00_FileId  ObjKeyFileId;
    fd_keypart_smart.BuildFileId(ObjKeyFileId);
    bd22_GetOId (Trans, ObjKeyFileId , KeyLen, pKey, Oid);      
}

/*-----------------------------------------------------------------------------------*/
/* verify MUST NOT lock container file nor ignore files marked to be deleted */
externCpp void 
bd22GetOIdForVerify (tgg00_TransContext  &Trans,
                     FileDir_FileNo      &Fileno,
                     tsp00_Int4           KeyLen,
                     void                *pKey,
                     tgg92_KernelOid     &Oid,
                     Msg_List            &ErrorList)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd22GetOid", ObjContainer_Trace, 5);
     
    tgg00_BasisError &TrError = Trans.trError_gg00;

    FileDir_OmsKey        fd_smart;
    FileDir_ReturnCode    fd_rc;
    FileDir_IDirectory &IFDC  = FileDir_IDirectory::Instance(cgg_nil_transindex);
    fd_rc = IFDC.GcGetFile(Trans.trTaskId_gg00, Fileno, fd_smart, ErrorList);
    if (FileDir_Okay != fd_rc) {
        bd900MapFileDirError(Trans, fd_rc, e_container_dropped, (char*)__FILE__,  __LINE__ );
        if (FileDir_FileNoNotFound == fd_rc)
            ErrorList.ClearMessageList();
        return;
    };

    cbd911RecreateIndexLock lock(Trans);   
    if (lock.IsNeeded(Fileno)) 
        lock.Lock( !bExclusive );

    const tsp00_Int4    NumObjKeyFiles = fd_smart.GetKeyPartitionCount();
    CHECK_NUM_KEY_FILES (NumObjKeyFiles); //PTS 1112190 18/10/2001
    tsp00_Int4 ObjKeyFileIndex = (NumObjKeyFiles == 1 ? 0 : bd22ObjKeyFileIndex (KeyLen, pKey, NumObjKeyFiles)); 
    FileDir_FileNo KeyFileno = fd_smart.GetKeyPartition(ObjKeyFileIndex + 1);
    FileDir_OmsKeyPartition   fd_keypart_smart;
    fd_rc = IFDC.GcGetFile(Trans.trTaskId_gg00, KeyFileno, fd_keypart_smart, ErrorList);
    if (FileDir_Okay != fd_rc) {
        bd900MapFileDirError(Trans, fd_rc, e_wrong_obj_file_id, (char*)__FILE__,  __LINE__ );
        return;
    }     
    tgg00_FileId  ObjKeyFileId;
    fd_keypart_smart.BuildFileId(ObjKeyFileId);
    bd22_GetOId (Trans, ObjKeyFileId , KeyLen, pKey, Oid);      
}

/*-----------------------------------------------------------------------------------*/

externCpp void 
bd22InsertOId (tgg00_TransContext &Trans,
               FileDir_FileNo     &Fileno,
               tsp00_Int4          KeyLen,
               void               *pKey,
               tgg92_KernelOid     OId,
               tgg92_KernelOid    &FoundOId,
               Msg_List           &ErrorList)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd22InsertOId", ObjContainer_Trace, 5);

    tgg00_BasisError &TrError   = Trans.trError_gg00;
    if ( TrError != e_ok) return;

    FileDir_IDirectory &IFDC  = FileDir_IDirectory::SessionInstance(Trans);
    FileDir_OmsKey    fd_smart;
    if ( !bd90GetKeyedFile (Trans, fd_smart, Fileno, ErrorList ))
        return;

    cbd911RecreateIndexLock lock(Trans);   
    if (lock.IsNeeded(Fileno)) 
        lock.Lock( !bExclusive );

    const tsp00_Int4       NumObjKeyFiles =  fd_smart.GetKeyPartitionCount();

    CHECK_NUM_KEY_FILES (NumObjKeyFiles); //PTS 1112190 18/10/2001
    tsp00_Int4 ObjKeyFileIndex = (NumObjKeyFiles == 1 ? 0 :bd22ObjKeyFileIndex (KeyLen, pKey, NumObjKeyFiles)); 
    FileDir_FileNo KeyFileno =  fd_smart.GetKeyPartition(ObjKeyFileIndex + 1);
    FileDir_OmsKeyPartition   fd_keypart_smart;
    FileDir_ReturnCode fd_rc = 
        IFDC.GetFile(Trans.trTaskId_gg00, KeyFileno, fd_keypart_smart, ErrorList);
    if (FileDir_Okay != fd_rc) {
        bd900MapFileDirError(Trans, fd_rc, e_wrong_obj_file_id, (char*)__FILE__,  __LINE__ );
        return;
    }     
    tgg00_FileId  ObjKeyFileId;
    fd_keypart_smart.BuildFileId(ObjKeyFileId);

    tsp00_Int4 FoundOIdLen;
    bd03OmsAddInv (Trans, ObjKeyFileId,
        tsp00_BytePtr (pKey), KeyLen, tsp00_BytePtr(&OId), sizeof(OId),
        sizeof(FoundOId), tsp00_BytePtr(&FoundOId), FoundOIdLen);

    switch (TrError)
    {
    case e_duplicate_invkey:
    case e_duplicate_key   : TrError = e_duplicate_hash_key; break;
    case e_inv_not_found   : TrError = e_hash_not_found;     break;
    }
    // PTS 1134645
    if (lock.IsNeeded(Fileno) && (e_ok == TrError )) 
    {
        bd03OmsAddInv (Trans, bd911GetKeyFileId(ObjKeyFileIndex),
            tsp00_BytePtr (pKey), KeyLen, tsp00_BytePtr(&OId), sizeof(OId),
            sizeof(FoundOId), tsp00_BytePtr(&FoundOId), FoundOIdLen);
        if (e_ok != TrError)
        {			
            Kernel_OpError( csp3_bd_msg, csp3_n_obj ) 
                << "bd22InsertOId for RecreateIndex - error:" << TrError
                << " FileNo:" << Fileno
                << " OId=" 
                << OId.gg92GetPno() << "." 
                << OId.gg92GetPos() << " (vers " 
                << OId.gg92GetFrameVers() << " )";
            bd911KeyToConsole(pKey, KeyLen);
            RTESys_ReadMemoryBarrier();
            if ( bd911RecreateIndexError() == e_ok )
                bd911RecreateIndexError() = TrError;
            RTESys_WriteMemoryBarrier();
            TrError = e_ok;
        }
    }
}

/*-----------------------------------------------------------------------------------*/
  
 externCpp void 
 bd22InsertOIdReCreate (tgg00_TransContext &Trans,
                        FileDir_FileNo     &Fileno,
                        tsp00_Int4          KeyLen,
                        void               *pKey,
                        tgg92_KernelOid     OId,
                        tgg92_KernelOid    &FoundOId,
                        Msg_List           &ErrorList)
 {
     ROUTINE_DBG_MEO00 ("bd22InsertOIdReCreate");

     tgg00_BasisError &TrError   = Trans.trError_gg00;
     if ( TrError != e_ok) return;

     FileDir_IDirectory &IFDC  = FileDir_IDirectory::SessionInstance(Trans);
     FileDir_OmsKey    fd_smart;
     if ( !bd90GetKeyedFile (Trans, fd_smart, Fileno, ErrorList ))
         return;

    const tsp00_Int4       NumObjKeyFiles =  fd_smart.GetKeyPartitionCount();

    CHECK_NUM_KEY_FILES (NumObjKeyFiles); //PTS 1112190 18/10/2001
    tsp00_Int4 ObjKeyFileIndex = (NumObjKeyFiles == 1 ? 0 :bd22ObjKeyFileIndex (KeyLen, pKey, NumObjKeyFiles)); 

    tsp00_Int4 FoundOIdLen;
    bd03OmsAddInv (Trans, bd911GetKeyFileId(ObjKeyFileIndex),
        tsp00_BytePtr (pKey), KeyLen, tsp00_BytePtr(&OId), sizeof(OId),
        sizeof(FoundOId), tsp00_BytePtr(&FoundOId), FoundOIdLen);

    switch (TrError)
    {
    case e_ok:
        break;
    case e_duplicate_invkey:
    case e_duplicate_key   : 
        if ( OId.gg92IsEqIgnoreFrameVers(FoundOId))
            TrError = e_ok; 
        else
        {
            Kernel_OpError( csp3_bd_msg, csp3_n_obj ) 
                << "bd22InsertOId for RecreateIndex - error:" << TrError
                << " FileNo:" << Fileno
                << " OId=" 
                << OId.gg92GetPno() << "." 
                << OId.gg92GetPos() << " (vers " 
                << OId.gg92GetFrameVers() << " )"
                << " and OId=" 
                << FoundOId.gg92GetPno() << "." 
                << FoundOId.gg92GetPos() << " (vers " 
                << FoundOId.gg92GetFrameVers() << " )"
                << " have same key";
            bd911KeyToConsole(pKey, KeyLen);
            RTESys_ReadMemoryBarrier();
            if ( bd911RecreateIndexError() == e_ok )
                bd911RecreateIndexError() = TrError;
            RTESys_WriteMemoryBarrier();
            TrError = e_ok;
        }
        break;
    default:
        {
            Kernel_OpError( csp3_bd_msg, csp3_n_obj ) 
                << "bd22InsertOId for RecreateIndex - error:" << TrError
                << " FileNo:" << Fileno
                << " OId=" 
                << OId.gg92GetPno() << "." 
                << OId.gg92GetPos() << " (vers " 
                << OId.gg92GetFrameVers() << " )";
            bd911KeyToConsole(pKey, KeyLen);
            RTESys_ReadMemoryBarrier();
            if ( bd911RecreateIndexError() == e_ok )
                bd911RecreateIndexError() = TrError;
            RTESys_WriteMemoryBarrier();
            TrError = e_ok;
        }
    }
 }
 
/*-----------------------------------------------------------------------------------*/

externCpp void 
bd22RemoveOid (tgg00_TransContext     &Trans,
               FileDir_Oms            &fd_smart,
               tsp00_Int4              KeyLen,
               void                   *pKey,
               tgg92_KernelOid         Oid,
               Msg_List               &ErrorList)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd22RemoveOid", ObjContainer_Trace, 5);

    tgg00_BasisError    &TrError = Trans.trError_gg00;
    tgg92_KernelOid      OidOfKey;
    
    FileDir_IDirectory &IFDC  = FileDir_IDirectory::SessionInstance(Trans);
    const tsp00_Int4       NumObjKeyFiles = fd_smart.GetKeyPartitionCount();
    CHECK_NUM_KEY_FILES (NumObjKeyFiles); //PTS 1112190 18/10/2001
    tsp00_Int4 ObjKeyFileIndex = (NumObjKeyFiles == 1 ? 0:bd22ObjKeyFileIndex (KeyLen, pKey, NumObjKeyFiles)); 
    FileDir_FileNo KeyFileno = fd_smart.GetKeyPartition(ObjKeyFileIndex + 1);
    FileDir_OmsKeyPartition   fd_keypart_smart;
    FileDir_ReturnCode  fd_rc =
        IFDC.GcGetFile(Trans.trTaskId_gg00, KeyFileno, fd_keypart_smart, ErrorList);
    if (FileDir_Okay != fd_rc) {
        bd900MapFileDirError(Trans, fd_rc, e_wrong_obj_file_id, (char*)__FILE__,  __LINE__ );
        return;
    }     
    tgg00_FileId  ObjKeyFileId;
    fd_keypart_smart.BuildFileId(ObjKeyFileId);
    
    tsp00_Int4 OidOfKeyLen;
    tsp00_Int4 OidPrefixToCompareLen = sizeof(OidOfKey) - sizeof(teo00_Uint2);
    bd03OmsDelInv (Trans, ObjKeyFileId,             /* PTS 1131960 FF */ 
        tsp00_BytePtr (pKey), KeyLen, tsp00_BytePtr(&Oid), sizeof(Oid),
        sizeof(OidOfKey), tsp00_BytePtr(&OidOfKey), OidPrefixToCompareLen, OidOfKeyLen );
    if (TrError == e_primkey_does_not_match)
    {
        TrError = e_ok;

        if (g01vtrace.vtrOmsUpd_gg00)
        {
            {
                tgg12_OmsOidTrace TrLineOId;
                
                TrLineOId.ooidTrType_gg12.becomes(otOid_egg12);
                TrLineOId.ooidTrType2_gg12.becomes(ot2Input_egg12);
                TrLineOId.ooidObjId_gg12   = Oid;

                Trace_CommonEntry entry( tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(b22rem_oid), 
                    REINTERPRET_CAST( tsp00_BytePtr, &TrLineOId ), sizeof (TrLineOId) );
            }
            {
                tgg12_OmsOidTrace TrLineOId;
                
                TrLineOId.ooidTrType_gg12.becomes(otOid_egg12);
                TrLineOId.ooidTrType2_gg12.becomes(ot2Output_egg12);
                TrLineOId.ooidObjId_gg12   = OidOfKey;
                
                Trace_CommonEntry entry( tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(b22rem_oid), 
                    REINTERPRET_CAST( tsp00_BytePtr, &TrLineOId ), sizeof (TrLineOId) );
            }
            tsp00_C8  Fileno_c8;
            memset(&Fileno_c8,0,sizeof(Fileno_c8));
            SAPDB_MemCopyNoCheck(&Fileno_c8,&KeyFileno,sizeof(KeyFileno));
            bd92VtraceObjKey (Trans, Fileno_c8, KeyLen, pKey, b22rem_oid);
        }
        return;   
    }

    switch (TrError)
    {
    case e_ok                :  break;
    case e_inv_list_not_found:
    case e_key_not_found     :  
        TrError = e_hash_key_not_found; 
        Oid.gg92SetNil();       
        break;
    case e_inv_not_found     :  
        TrError = e_hash_not_found;     
        Oid.gg92SetNil();           
        break;
    }
    // PTS 1134645
    cbd911RecreateIndexLock lock(Trans);   
    if (lock.IsNeeded(fd_smart.GetFileNo()) && ( e_ok == TrError )) 
    {
        bd03OmsDelInv (Trans, bd911GetKeyFileId(ObjKeyFileIndex),             
            tsp00_BytePtr (pKey), KeyLen, tsp00_BytePtr(&Oid), sizeof(Oid),
            sizeof(OidOfKey), tsp00_BytePtr(&OidOfKey), OidPrefixToCompareLen, OidOfKeyLen );
        switch (TrError)
        {
        case e_ok                :  break;
        case e_inv_list_not_found:
        case e_key_not_found     :  
            TrError = e_ok; 
            break;
        case e_inv_not_found     :  
            TrError = e_ok;     
            break;
        default:
            {
                Kernel_OpError( csp3_bd_msg, csp3_n_obj ) 
                    << "bd22RemoveOid for RecreateIndex - error:" << TrError
                    << " FileNo:" << fd_smart.GetFileNo()
                    << " Oid=" 
                    << Oid.gg92GetPno() << "." 
                    << Oid.gg92GetPos() << " (vers " 
                    << Oid.gg92GetFrameVers() << " )";
                bd911KeyToConsole(pKey, KeyLen);
                RTESys_ReadMemoryBarrier();
                if ( bd911RecreateIndexError() == e_ok )
                    bd911RecreateIndexError() = TrError;
                RTESys_WriteMemoryBarrier();
                TrError = e_ok;
            }
        }
    }
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/ 
