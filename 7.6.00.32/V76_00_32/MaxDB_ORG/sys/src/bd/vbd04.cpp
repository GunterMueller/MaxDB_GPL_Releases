/****************************************************************************

  module      : vbd04.cpp

  -------------------------------------------------------------------------

  responsible : FerdiF

  special area: BD FileSystemInterface4: Operations for processing object files
  description : description ...


  version     : 7.5.0.0
  last changed: 2003-17-16  12:55
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
 
#include <string.h>

#ifndef GSP03_H
#   include "gsp03.h"
#endif
#ifndef GSP03_3_H
#   include "gsp03_3.h"
#endif
#ifndef GGG01_H
#   include "ggg01.h"   // PASCAL: OMS basis errors
#endif
#ifndef GGG11_H
#   include "ggg11.h"   // PASCAL: GG_vtrace_types
#endif
#ifndef GGG12_H
#   include "ggg12.h"
#endif
#ifndef GBD00_H
#   include "gbd00.h"
#endif
#ifndef GBD05_H
#   include "gbd05.h"
#endif
#ifndef GBD900_H
#   include "gbd900.h"
#endif
#ifndef HBD01_H
#   include "hbd01.h"
#endif
#ifndef HBD04_H
#   include "hbd04.h"
#endif
#ifndef HBD06_H
#   include "hbd06.h"
#endif
#ifndef HBD30_H
#   include "hbd30.h"
#endif
#ifndef HBD22_H
#   include "hbd22.h"    // OMS Key Management 
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
#ifndef HBD95_1_H
#   include "hbd95_1.h"
#endif
#ifndef HEO56_H
#   include "heo56.h"    // vreschedule_msec
#endif
#ifndef HEO58_H
#   include "heo58.h"    // vinit_akdump_ptr
#endif
#ifndef HGG01_1_H
#   include "hgg01_1.h"
#endif
#ifndef HGG01_3_H
#   include "hgg01_3.h"
#endif
#ifndef HKB50_H
#   include "hkb50.h"
#endif
#ifndef HKB53_H
#   include "hkb53.h"
#endif
#ifndef CONVERTER_ICOMMON_HPP
#   include "Converter/Converter_ICommon.hpp"
#endif
#ifndef LVC_ABSTRACTOBJKEYITERATOR_HPP
#   include "liveCache/LVC_AbstractObjKeyIterator.hpp"
#endif
#ifndef LVC_SINGLEFILEOBJKEYITERATOR_HPP
#   include "liveCache/LVC_SingleFileObjKeyIterator.hpp"
#endif
#ifndef LVC_MULTIFILEOBJKEYITERATOR_HPP
#   include "liveCache/LVC_MultiFileObjKeyIterator.hpp"
#endif
#ifndef LVC_INPROCDISPATCHER_HPP
#   include "liveCache/LVC_InProcDispatcher.hpp"
#endif
#ifndef SAPDBMEM_NEWDESTROY_HPP
#   include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#endif
#ifndef TRACE_ENTRY_HPP
#   include "Trace/Trace_Entry.hpp"
#endif
#ifndef KERNEL_VTRACE_HPP
#   include "KernelCommon/Kernel_VTrace.hpp"        /* PTS 1120151 */ 
#endif
#ifndef MSG_LIST_HPP
#   include "Messages/Msg_List.hpp"
#endif
#ifndef SAPDBMEM_IRAWALLOCATOR_HPP
#   include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#endif
#ifndef SAPDBMEM_RAWALLOCATOR_HPP
#   include "SAPDBCommon/MemoryManagement/SAPDBMem_RawAllocator.hpp"
#endif
#ifndef RTEMEM_BLOCKALLOCATOR_HPP
#   include "RunTime/MemoryManagement/RTEMem_BlockAllocator.hpp"
#endif
#ifndef RTEMEM_ALLOCATOR_HPP
#   include "RunTime/MemoryManagement/RTEMem_Allocator.hpp"
#endif
#ifndef SAPDB_SPRINTF_H
#   include "SAPDBCommon/SAPDB_sprintf.h"
#endif
#ifndef FILEDIR_IDIRECTORY_HPP
#   include "FileDirectory/FileDir_IDirectory.hpp"
#endif
#ifndef HBD300_H
// SetRootCheck 
#   include "hbd300.h"
#endif
#ifndef HAK3410_H
#   include "hak341.h"
#endif

#if COMPILEMODE_MEO00 >= SLOW_MEO00
#include "hta99.h"
#include "hta01_2.h"
#endif
 
/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define WRITE_VTRACE    true
#define WITH_OBJECT     true
#define MIN_RETRY_TIME  30000 //msec

#ifdef NO_INLINES
# define _INLINE
#else
# define _INLINE    inline
#endif

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/
    
#  define BD04_CHECK_FILE_SYSTEM( TRANS, OBJFILEID, TRACEOBJ)                   \
    if (b01downfilesystem)                                                      \
    {                                                                           \
        (TRANS).trError_gg00 = e_shutdown;                                      \
        bd04_VtraceErrClass ((TRANS), (OBJFILEID), (TRACEOBJ), (WRITE_VTRACE)); \
        return;                                                                 \
    }
 
/*---------------------------------------------------------------------------*/ 

#if COMPILEMODE_MEO00 >= QUICK_MEO00
 
#  define BD04_CHECK_OBJ_FILE_TYPE( TRANS, OBJFILEID, TRACEOBJ)                 \
    if (((OBJFILEID).fileTfn_gg00()         != tfnObj_egg00)           &&       \
        ((OBJFILEID).fileObjFileType_gg00() != oftUnknownType_egg00))           \
    {                                                                           \
        (TRANS).trError_gg00 = e_invalid_filetype;                              \
        bd04_VtraceErrClass ((TRANS), (OBJFILEID), (TRACEOBJ), (WRITE_VTRACE)); \
        return;                                                                 \
    }                                                                           \
 
#else
 
#  define BD04_CHECK_OBJ_FILE_TYPE( TRANS, OBJFILEID, TRACEOBJ)
 
#endif
 
/*--------------------------------------------------------------------------*/
 
#  define BD04_CHECK_OID( TRANS, OBJFILEID, TRACEOBJ, OID)                      \
    if (!bd04_OIdIsValid(OID) &&                                                \
        ((TRANS).trError_gg00 == e_ok)) /* PTS 1141554 */                       \
    {                                                                           \
        (TRANS).trError_gg00 = e_invalid_oid;                                   \
        bd04_VtraceErrClass ((TRANS), (OBJFILEID), (TRACEOBJ), (WRITE_VTRACE)); \
        return;                                                                 \
    } 

/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/


/*===========================================================================*
 *  LOCAL FUNCTIONS (PROTOTYPES)                                             *
 *===========================================================================*/

static const bool bExclusive = true;

inline static void 
bd04_MapPhysicalOntoLogicalError (tgg00_TransContext &TrError);

/*---------------------------------------------------------------------------*/
 
static _INLINE bool
bd04_OIdIsValid (tgg92_KernelOid  OId);

/*--------------------------------------------------------------------------*/

static _INLINE void
bd04_Reschedule (tgg00_TransContext &Trans,
                 tsp00_Int4         &StartTime,
                 tsp00_Int4         &WaitingTime,
                 bool               &bTryAgain);
 
/*--------------------------------------------------------------------------*/
 
static _INLINE void
bd04_ReuseObj (tgg00_TransContext   &Trans,
               tgg00_FileId         &ObjFileId,
               tgg92_KernelOid      &OId,
               tgg91_PageRef        &ResultObjVers,
               tsp00_Int4            KeySize,
               void                 *pKey,
               bool                 &bTryAgain,
               Msg_List             &ErrorList);
 
/*--------------------------------------------------------------------------*/
 
static _INLINE void
bd04_VtraceErrClass (tgg00_TransContext       &Trans,
                     const tgg00_FileId       &ObjFileId,
                     tgg00_VtraceType_Enum     TraceObject,
                     tsp00_Bool                bWriteVtrace);
 
/*--------------------------------------------------------------------------*/
 
static void
bd04_VtraceReturn (tgg00_TransContext       &Trans,
                   const tgg00_FileId       &ObjFileId,
                   tgg92_KernelOid           Oid,
                   tgg91_PageRef             PageRef,
                   tgg00_VtraceType_Enum     TraceObject,
                   tgg12_OmsTraceType2_Enum  TraceType2);

/*--------------------------------------------------------------------------*/
/* PTS 1111948 AK 26/09/2001 */
inline static SAPDB_Bool
bd04_ObjFileIdIsValid (const tgg00_FileId  &ObjFileId);

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/
          
class cbd04_ObjCurrent : public cbd05_Current
{
private: 
    tgg00_FileId *m_pFileId;
public:
    inline cbd04_ObjCurrent  (tgg00_TransContext   &Trans,
                              tgg00_FileId         &ObjFileId,
                              tgg00_MessType_Enum   MessType);
    // HPUX compiler does not like inline destructors even with forward decleration 
    // But if explicitly put here it is accepted...
    // The only other chance is to remove 'inline' completely... jrg 10.1.2000
    inline ~cbd04_ObjCurrent ()
    {
        if (NULL != m_pFileId)
        {
            *m_pFileId = curr_tree_id;
            
            /* check returned object file id */
            SAPDBERR_ASSERT_STATE ((e_ok != curr_trans->trError_gg00) || bd04_ObjFileIdIsValid(*m_pFileId));                
        }
    }
};
 
/*===========================================================================*
 *  GLOBAL FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

externCpp void
bd04CreateKeyIterator (tgg00_TransContext &Trans,
                       tgg00_FileId       &ObjFileId,
                       tsp00_Int4          KeyLen,
                       void               *pStartKey,
                       //void                 *pStopKey,         // PTS 1119480
                       void               *pLowerKey,            // PTS 1119480
                       void               *pUpperKey,            // PTS 1119480
                       void              *&pKeyIterator,
                       bool                bAscendingKeyOrder,   // PTS 1119480
                       Msg_List           &ErrorList)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd04CreateKeyIterator", ObjContainer_Trace, 5);
    
    tgg00_BasisError &TrError  = Trans.trError_gg00;
    
    /* write object file, start and stop key into vtrace */
    if (g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdObject_gg00)
    {
        {
            tgg12_OmsFilenameTrace TrLineIn;
            TrLineIn.ofnTrType_gg12.becomes(otFilename_egg12);
            TrLineIn.ofnTrType2_gg12.becomes(ot2Nil_egg12);
            TrLineIn.ofnFileName_gg12 = ObjFileId.fileName_gg00();
            
            Trace_CommonEntry entry( tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(b04CreateKeyIter), 
                REINTERPRET_CAST( tsp00_BytePtr, &TrLineIn ), sizeof (TrLineIn) );
        }
        {
            /* write start and stop key of the iterator into the vtrace PTS 1111934 AK 25/09/2001 */   
            tgg12_OmsTwoKeyTrace KeyTrace;
            KeyTrace.oskTrType_gg12.becomes(otTwoKeys_egg12);
            KeyTrace.oskTrType2_gg12.becomes(ot2Nil_egg12);
            KeyTrace.oskKeyLen_gg12   = KeyLen;
            KeyTrace.oskStartKey_gg12 = *((tgg00_VtraceKey *) pStartKey);
            //KeyTrace.oskStopKey_gg12  = *((tgg00_VtraceKey *) pStopKey);  // PTS 1119480
            //KeyTrace.oskStopKey_gg12  = *((tgg00_VtraceKey *) pLowerKey); // TODO !!!
            KeyTrace.oskStopKey_gg12  = *((tgg00_VtraceKey *) pUpperKey);   // PTS 1119480
            
            Trace_CommonEntry entry( tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(b04CreateKeyIter), 
                REINTERPRET_CAST( tsp00_BytePtr, &KeyTrace ), sizeof (KeyTrace) );
        }
    }
    
    BD04_CHECK_FILE_SYSTEM   (Trans, ObjFileId, b04CreateKeyIter);
    BD04_CHECK_OBJ_FILE_TYPE (Trans, ObjFileId, b04CreateKeyIter);


    /* create correct file id */
    const FileDir_FileNo    Fileno  = ObjFileId.fileDirFileId_gg00();

    /* try share lock to protect against omsDropContainer */
    tgg00_LockReqMode  ReqMode;
    ReqMode.becomes(lckTabShare_egg00);
    bd90LockObjContainer(Trans, ObjFileId.fileDirFileId_gg00(), ReqMode);
    if (e_ok != Trans.trError_gg00)
        return;

    FileDir_Oms     fd_smart;
    FileDir_ReturnCode  fd_rc = FileDir_IDirectory::SessionInstance(Trans).GetFile(
        Trans.trTaskId_gg00, Fileno, fd_smart, ErrorList);
    if (FileDir_Okay != fd_rc) {
        bd900MapFileDirError(Trans, fd_rc, e_file_not_found, (char*)__FILE__,  __LINE__ );
    }
    else if(ObjFileId.fileNumObjKeyFiles_gg00()<=0) {
        Kernel_OpError( csp3_bd_msg, csp3_n_obj )
            << __FILE__ << ":" << __LINE__ 
            << "file (ClassID:" << Fileno << ") has no key index";
        TrError = e_inv_not_found;
    }
    
    cbd911RecreateIndexLock lock(Trans);  
    if (lock.IsNeeded(Fileno)) 
        lock.Lock( !bExclusive );
    
    if (TrError == e_ok)
    {
        /* create the key Iterator on the heap */
        SAPDBMem_IRawAllocator &RawAllocator = *(REINTERPRET_CAST(SAPDBMem_IRawAllocator*, Trans.trAllocator_gg00));

        const tsp00_Int4 NumObjKeyFiles = fd_smart.GetKeyPartitionCount();

        if (NumObjKeyFiles == 1)
        {
            pKeyIterator =   new (RawAllocator) 
                LVC_SingleFileObjKeyIterator (Trans, ErrorList, ObjFileId, KeyLen, 
                                      pStartKey, pLowerKey, pUpperKey, 
                                      RawAllocator, bAscendingKeyOrder); 
        }
        else
        {
            pKeyIterator =   new (RawAllocator) 
                LVC_MultiFileObjKeyIterator (Trans, ErrorList, ObjFileId, NumObjKeyFiles, KeyLen, 
                                     pStartKey, pLowerKey, pUpperKey, 
                                     RawAllocator, bAscendingKeyOrder); 
        }
        
        if (NULL == pKeyIterator) {
            Kernel_OpError( csp3_bd_msg, csp3_n_obj )
                << __FILE__ << ":" << __LINE__ 
                << "no more memory to create key iterator";
            TrError=e_no_more_memory;
        }
    }   
    
    /* write trace entry  */
    bd04_VtraceErrClass (Trans, ObjFileId, b04CreateKeyIter,
        (g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdObject_gg00));
}

/*---------------------------------------------------------------------------*/
externC void
bd04CreateObjFile (tgg00_TransContext   &Trans,
                   tgg00_FileId         &ObjFileId,
                   tsp00_Int4            ObjBodyLen,
                   tsp00_Int4            ObjKeyLen,
                   tsp00_Bool            bCreateSeveralObjKeyFiles,
                   tsp00_Bool            bCreateFileIsOnline,
                   Data_PageNo           Root,
                   tsp00_Uint1           contIndex,
                   LVC_FileNoArray      &FileNoArray,
                   Msg_List             &ErrorList)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd04CreateObjFile", ObjContainer_Trace, 5);

    /* An object file and all its additional files    */
    /* (as key file, continuation files) are created  */

    tgg00_BasisError &TrError        = Trans.trError_gg00;
    const int         cZeroObjKeyLen = 0;

    if (g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdObject_gg00)
    {
        tgg12_OmsFilenameTrace TrLineIn;

        TrLineIn.ofnTrType_gg12.becomes(otFilename_egg12);
        TrLineIn.ofnTrType2_gg12.becomes(ot2Nil_egg12);
        TrLineIn.ofnFileName_gg12 = ObjFileId.fileName_gg00();

        Trace_CommonEntry entry( tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(b04create), 
            REINTERPRET_CAST( tsp00_BytePtr, &TrLineIn ), sizeof (TrLineIn) );
    }
    
    BD04_CHECK_FILE_SYSTEM   (Trans, ObjFileId, b04create); 

    /* create appropriate file id in the currrent */
    cbd05_Current Current (Trans, m_create_file);

    FileDir_IDirectory::SessionInstance(Trans).BuildNewOmsFileId(
        ObjFileId.fileDirFileId_gg00(), 
        ObjFileId.fileObjFileType_gg00(), 
        contIndex, Current.curr_tree_id);

    /* check key len */
    if  ((ObjFileId.fileObjFileType_gg00() == oftVarLenObjFile_egg00) &&
        (ObjKeyLen != cZeroObjKeyLen))
    {
        Kernel_OpError( csp3_bd_msg, csp3_n_obj )
            << __FILE__ << ":" << __LINE__ 
            << "varobject with key length:" << ObjKeyLen 
            << " > 0 not allowed";
        TrError = e_illegal_keylength;
        return;
    }

    /* determine how many object key file should be created */ 
    tsp00_Int4 NumObjKeyFiles = 0;
    if(ObjKeyLen > cZeroObjKeyLen)
        NumObjKeyFiles = (bCreateSeveralObjKeyFiles ? NUM_KEY_PARTITIONS_BD900:1); 
    
    if (e_ok != TrError) return; 

    cbd910_ObjDataNode   NptrsRoot (Current, nr_for_update);
    Converter_Version    converterVersion;  

    /* create object file */
    if (bCreateFileIsOnline)
    {
        for (SAPDB_UInt1 i=0; i < NumObjKeyFiles; i++)
            FileNoArray[i] = FileDir_IDirectory::SessionInstance(Trans).GetNewFileNo();
        bd90CreateObjFile (Current, ObjBodyLen, ObjKeyLen, NumObjKeyFiles, NptrsRoot,
            converterVersion, ErrorList);
    }
    else  
    {
        bd90Rollforward_CreateObjFile (Current, ObjBodyLen, ObjKeyLen, NumObjKeyFiles, Root, NptrsRoot, 
            converterVersion, ErrorList);
    }
    FileDir_FileNo  Fileno = ObjFileId.fileDirFileId_gg00();

    /* create key file */
    if ((e_ok == TrError) && (NumObjKeyFiles>0))
        if (bCreateFileIsOnline)
        {
            bd22CreateObjKeyFiles (Trans, Fileno, NumObjKeyFiles, FileNoArray, ErrorList);
            FileDir_OmsKey     fd_smart;
            FileDir_ReturnCode fd_rc = 
                FileDir_IDirectory::Instance(Trans.trIndex_gg00).GetFile(
                    Trans.trTaskId_gg00, Fileno, fd_smart, ErrorList);
            if (FileDir_Okay != fd_rc)
            {
                bd900MapFileDirError(Trans, fd_rc, e_file_not_found, (char*)__FILE__, __LINE__ );
                Kernel_OpError( csp3_bd_msg, csp3_n_obj )
                    << __FILE__ << ":" << __LINE__ 
                    << "file (ClassID:" << Fileno << ") FileDir Error:" << fd_rc;
            }
            else
            {
                for (SAPDB_UInt1 i=0; i < NumObjKeyFiles; i++)
                    FileNoArray[i] = fd_smart.GetKeyPartition(i+1);
            }
        }
        else
            bd22Rollforward_CreateObjKeyFiles (Trans, Fileno, NumObjKeyFiles, FileNoArray, ErrorList);

    /* update the file id to return */
    ObjFileId = Current.curr_tree_id;

    /* write trace entry  */
    bd04_VtraceErrClass (Trans, ObjFileId, b04create,
        (g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdObject_gg00));
    };

/*---------------------------------------------------------------------------*/
 
externCpp void
bd04DelAllObj (tgg00_TransContext   &Trans,
               tgg00_FileId         &ObjFileId,
               tsp00_Int4           &NumDelObj,
               tgg92_KernelOid      &ErrorOid,
               Msg_List             &ErrorList)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd04DelAllObj", ObjContainer_Trace, 5);
 
    /* This routine marks all objects of this file as free after eot */
    /* This command removes either all objects or no object if there */
    /* is any object which cannot be deleted because it is locked    */
    /* the  complete command is rollbacked, i.e. all objects         */ 
    /* previously removed are recreated.                             */

    if (g01vtrace.vtrOmsUpd_gg00)
    {
        tgg12_OmsFilenameTrace TrLineIn;
        
        TrLineIn.ofnTrType_gg12.becomes(otFilename_egg12);
        TrLineIn.ofnTrType2_gg12.becomes(ot2Nil_egg12);
        TrLineIn.ofnFileName_gg12 = ObjFileId.fileName_gg00();
        
        Trace_CommonEntry entry( tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(b04DelAll), 
            REINTERPRET_CAST( tsp00_BytePtr, &TrLineIn ), sizeof (TrLineIn) );
    }
    
    BD04_CHECK_FILE_SYSTEM   (Trans, ObjFileId, b04del);
    BD04_CHECK_OBJ_FILE_TYPE (Trans, ObjFileId, b04del);

    cbd04_ObjCurrent Current (Trans, ObjFileId, m_update);
    
    bool bTryAgain = true;
    while ((e_ok == Trans.trError_gg00) && bTryAgain)  
    {
        bd90DelAllObj (Current, NumDelObj, ErrorOid, ErrorList);
        if (e_wait_for_lock_release == Trans.trError_gg00)
        {   
            Trans.trError_gg00 = e_ok;
            kb50WaitAndRetryObjLock (Trans, m_lock, mm_object);
            bTryAgain = (e_ok == Trans.trError_gg00);  
        }
        else
            bTryAgain = false;
    };
    
    /* write vtrace entry */
    bd04_VtraceErrClass (Trans, ObjFileId, b04DelAll, (g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdObject_gg00));
}

/*---------------------------------------------------------------------------*/

externCpp void
bd04DelObj (tgg00_TransContext  &Trans,
            tgg00_FileId        &ObjFileId,
            tgg92_KernelOid      OId,
            tgg91_PageRef        WantedObjVers,
            SAPDB_Int1           contObj,            
            bool                 bOmsLayerReusedObj,
            Msg_List            &ErrorList)
{
    SAPDBTRACE_ROUTINE_DEBUG("kb04DeleteObj", ObjContainer_Trace, 5);

    /*  This routine marks an objects as free after eot    */
    if (g01vtrace.vtrOmsUpd_gg00)
    {
        tgg12_OmsOidTrace TrLineOIdSeq;

        TrLineOIdSeq.ooidTrType_gg12.becomes(otOid_egg12);
        TrLineOIdSeq.ooidTrType2_gg12.becomes(ot2InputSeqVers_egg12);
        TrLineOIdSeq.ooidObjId_gg12    = OId;
        TrLineOIdSeq.ooidObjVers_gg12  = WantedObjVers;
        TrLineOIdSeq.ooidConsView_gg12 = Trans.trConsistView_gg00;

        Trace_CommonEntry entry( tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(b04del), 
            REINTERPRET_CAST( tsp00_BytePtr, &TrLineOIdSeq ), sizeof (TrLineOIdSeq) );
    }

    BD04_CHECK_FILE_SYSTEM   (Trans, ObjFileId, b04del);
    BD04_CHECK_OBJ_FILE_TYPE (Trans, ObjFileId, b04del);
    BD04_CHECK_OID           (Trans, ObjFileId, b04del, OId);

    cbd04_ObjCurrent Current (Trans, ObjFileId, m_update);

    bool bTryAgain = true;
    while ((e_ok == Trans.trError_gg00) && bTryAgain)  
    {
        bd90DelObj (Current, OId, WantedObjVers, contObj, bOmsLayerReusedObj, ErrorList);
        if (e_wait_for_lock_release == Trans.trError_gg00)
        {   
            Trans.trError_gg00 = e_ok;
            kb50WaitAndRetryObjLock (Trans, m_lock, mm_object);
            bTryAgain = (e_ok == Trans.trError_gg00);  
        }
        else
            bTryAgain = false;
    };

    bd04_MapPhysicalOntoLogicalError (Trans);
 
    if (g01vtrace.vtrOmsUpd_gg00)
        bd04_VtraceReturn (Trans, Current.curr_tree_id, OId, WantedObjVers, b04del, ot2Output_egg12);
}

/*---------------------------------------------------------------------------*/

externCpp void
bd04DestroyKeyIterator (tgg00_TransContext  &Trans,
                        void               *&pUntypedKeyIterator)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd04DestroyKeyIterator", ObjContainer_Trace, 5);
    
    if (pUntypedKeyIterator != NULL)
    {
        LVC_AbstractObjKeyIterator * pKeyIterator = (LVC_AbstractObjKeyIterator *) pUntypedKeyIterator;

        if (g01vtrace.vtrOmsGet_gg00 )
        {
            tgg12_OmsFilenameTrace TrLineIn;
            
            TrLineIn.ofnTrType_gg12.becomes(otFilename_egg12);
            TrLineIn.ofnTrType2_gg12.becomes(ot2Nil_egg12);
            TrLineIn.ofnFileName_gg12 = pKeyIterator->GetObjFileId().fileName_gg00();
            
            Trace_CommonEntry entry( tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(b04DestroyIter), 
                REINTERPRET_CAST( tsp00_BytePtr, &TrLineIn ), sizeof (TrLineIn) );
        }
                
        /* check if the casted iterator is really an iterator */
        SAPDBERR_ASSERT_STATE (pKeyIterator->IsValid());
        
        SAPDBMem_IRawAllocator &RawAllocator = *(REINTERPRET_CAST(SAPDBMem_IRawAllocator*, Trans.trAllocator_gg00));
        destroy (pKeyIterator, RawAllocator);
    }
}


/*---------------------------------------------------------------------------*/
 
externCpp void
bd04DropObjFile (tgg00_TransContext   &Trans,
                 tgg00_FileId         &ObjFileId,
                 Msg_List             &ErrorList)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd04DropObjFile", ObjContainer_Trace, 5);
 
    /* This procedure marks an object file as deleted                 */
    /* the file will be removed by the garbagecollector when finding  */ 
    /* the Undo/History entry                                         */

    if (g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdObject_gg00)
    {
        tgg12_OmsFilenameTrace TrLineIn;
 
        TrLineIn.ofnTrType_gg12.becomes(otFilename_egg12);
        TrLineIn.ofnTrType2_gg12.becomes(ot2Nil_egg12);
        TrLineIn.ofnFileName_gg12 = ObjFileId.fileName_gg00();

        Trace_CommonEntry entry( tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(b04drop), 
                REINTERPRET_CAST( tsp00_BytePtr, &TrLineIn ), sizeof (TrLineIn) );
    };
 
#   if COMPILEMODE_MEO00 >= SLOW_MEO00
    t01treeid (bi, "ObjFileId   ", ObjFileId);
#   endif
 
    BD04_CHECK_FILE_SYSTEM   (Trans, ObjFileId, b04drop);
    BD04_CHECK_OBJ_FILE_TYPE (Trans, ObjFileId, b04drop);

    cbd04_ObjCurrent Current (Trans, ObjFileId, m_update);
    
    if (e_ok == Trans.trError_gg00)
        bd90DropObjFile (Current, ErrorList);
    
    bd04_VtraceErrClass (Trans, ObjFileId, b04drop,
        (g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdObject_gg00));
}

/*---------------------------------------------------------------------------*/
// PTS 1120478
externCpp void
bd04GetContainerId (tgg00_TransContext  &Trans,              //[inp/out]  
                    tgg00_FileId        &ObjFileId,          //[inp]
                    tgg92_KernelOid     &Oid)                //[inp/out] 
{
    SAPDBTRACE_ROUTINE_DEBUG("bd04GetContainerId", ObjContainer_Trace, 5);
  
    if (g01vtrace.vtrOmsGet_gg00 )
    {
        tgg12_OmsOidTrace TrLineOIdSeq;
        
        TrLineOIdSeq.ooidTrType_gg12.becomes (otOid_egg12);
        TrLineOIdSeq.ooidTrType2_gg12.becomes(ot2InputVers_egg12);
        TrLineOIdSeq.ooidObjId_gg12    = Oid;
        TrLineOIdSeq.ooidConsView_gg12 = Trans.trConsistView_gg00;
        
        Trace_CommonEntry entry( tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(b04get_consistent), 
            REINTERPRET_CAST( tsp00_BytePtr, &TrLineOIdSeq ), sizeof (TrLineOIdSeq) );
    }
 
#   if COMPILEMODE_MEO00 >= SLOW_MEO00
    t01treeid (bi, "ObjFileId   ", ObjFileId);
    ta01Oid   (bi, "OID         ", Oid);
#   endif
 
    BD04_CHECK_FILE_SYSTEM   (Trans, ObjFileId, b04get_consistent);
    BD04_CHECK_OBJ_FILE_TYPE (Trans, ObjFileId, b04get_consistent); 
    BD04_CHECK_OID           (Trans, ObjFileId, b04get_consistent, Oid);
 
    cbd04_ObjCurrent Current (Trans, ObjFileId, m_select);
    
    if (Trans.trError_gg00 == e_ok){
        bd90GetContainerId (Current, Oid);
    }
        
    bd04_MapPhysicalOntoLogicalError (Trans);
}

/*---------------------------------------------------------------------------*/

externCpp void
bd04GetObjConsistent (tgg00_TransContext    &Trans,              //[inp/out]  
                      tgg00_FileId          &ObjFileId,          //[inp]
                      tgg92_KernelOid       &OId,                //[inp/out] 
                      tsp00_Int4             ExtObjBodySize,     //[inp=size of buffer supplied for the object]
                      void                  *pExtObjBody,        //[inp=position to which the object hast to be copied]
                      tsp00_Int4             HashKeySize,        //[inp]
                      void                  *pHashKey,           //[inp]
                      const bool             bDoLock,            //[inp=true if the object should be locked] 
                      const bool             bShareLock,         //[inp]   
                      tsp00_Int4            &ObjBodySize,        //[out=real size of object]
                      tsp00_Int4            &KeyLen,             //[out=key len of the object]
                      tsp00_Byte*           *pKey,               //[out] 
                      tgg91_PageRef         &ResultObjVers,      //[out]
                      tgg91_TransNo         &UpdTransId,         //[out]
                      tsp00_Int4            &LogReadAccesses,    //[out] PTS 1107819 AK 12/09/2000
                      Msg_List              &ErrorList)

{
    SAPDBTRACE_ROUTINE_DEBUG("kb04GetObjConsistent", ObjContainer_Trace, 5);
  
    if (g01vtrace.vtrOmsGet_gg00 )
    {
        tgg12_OmsOidTrace TrLineOIdSeq;
        
        TrLineOIdSeq.ooidTrType_gg12.becomes (otOid_egg12);
        TrLineOIdSeq.ooidTrType2_gg12.becomes(ot2InputVers_egg12);
        TrLineOIdSeq.ooidObjId_gg12    = OId;
        TrLineOIdSeq.ooidConsView_gg12 = Trans.trConsistView_gg00;
        TrLineOIdSeq.ooidObjVers_gg12  = ResultObjVers;

        Trace_CommonEntry entry( tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(b04get_consistent), 
            REINTERPRET_CAST( tsp00_BytePtr, &TrLineOIdSeq ), sizeof (TrLineOIdSeq) );
    }
 
#   if COMPILEMODE_MEO00 >= SLOW_MEO00
    t01treeid (bi, "ObjFileId   ", ObjFileId);
    ta01Oid   (bi, "OID         ", OId);
#   endif
 
    BD04_CHECK_FILE_SYSTEM   (Trans, ObjFileId, b04get_consistent);
    BD04_CHECK_OBJ_FILE_TYPE (Trans, ObjFileId, b04get_consistent); 
    BD04_CHECK_OID           (Trans, ObjFileId, b04get_consistent, OId);
 
    cbd04_ObjCurrent Current (Trans, ObjFileId, m_select);

    bool bTryAgain = true;
    while ((e_ok == Trans.trError_gg00) && bTryAgain)  
    {
        if (bDoLock)
        {
            bd90GetObjConsistentWithLock (Current, OId, ExtObjBodySize,
                pExtObjBody, HashKeySize, pHashKey, bShareLock, ObjBodySize, KeyLen,
                pKey, ResultObjVers, UpdTransId, LogReadAccesses, ErrorList); 
        }
        else
        {
            bd90GetObjConsistent (Current, OId, ExtObjBodySize, 
                pExtObjBody, HashKeySize, pHashKey, ObjBodySize,
                KeyLen, pKey, ResultObjVers, UpdTransId, LogReadAccesses, ErrorList);
        }

        if (e_wait_for_lock_release == Trans.trError_gg00)
        {   
            Trans.trError_gg00 = e_ok;
            kb50WaitAndRetryObjLock (Trans, m_lock, mm_object);
            bTryAgain = (e_ok == Trans.trError_gg00);  
        }
        else
            bTryAgain = false;
    }

    bd04_MapPhysicalOntoLogicalError (Trans);

#   if COMPILEMODE_MEO00 >= SLOW_MEO00
    t01page_ref (bi, "Out ObjVers ", ResultObjVers);
#   endif
    
    if (g01vtrace.vtrOmsGet_gg00)
        bd04_VtraceReturn (Trans, Current.curr_tree_id, OId, ResultObjVers, b04get_consistent, ot2OutputSeq_egg12);
}

/*---------------------------------------------------------------------------*/

externCpp void
bd04GetObjWithKeyConsistent (tgg00_TransContext    &Trans,            //[inp/out]
                             tgg00_FileId          &ObjFileId,        //[inp]
                             tsp00_Int4             ExtObjBodySize,   //[inp]
                             void                  *pExtObjBody,      //[inp] 
                             tsp00_Int4            &KeyLen,           //[inp]
                             void                  *pKey,             //[inp]
                             const bool             bDoLock,          //[inp] 
                             const bool             bShareLock,       //[inp]   
                             tgg92_KernelOid       &ResultOid,        //[out]
                             tgg91_PageRef         &ResultObjVers,    //[out]
                             tgg91_TransNo         &UpdTransId,       //[out]
                             tsp00_Int4            &LogReadAccesses,  //[out] 
                             Msg_List              &ErrorList)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd04GetObjWithKeyConsistent", ObjContainer_Trace, 5);
 
    tsp00_Byte  *pAuxKey       = NULL;
    tsp00_Int4   AuxKeyLen     = 0;
    tsp00_Int4   AuxObjBodyLen = 0;

    if (g01vtrace.vtrOmsGet_gg00)
        bd92VtraceObjKey (Trans, ObjFileId.fileDirFileId_gg00(), KeyLen, pKey, b04GetObjWithKey);
 
    BD04_CHECK_FILE_SYSTEM   (Trans, ObjFileId, b04GetObjWithKey);
    BD04_CHECK_OBJ_FILE_TYPE (Trans, ObjFileId, b04GetObjWithKey);
 
    FileDir_FileNo  Fileno = ObjFileId.fileDirFileId_gg00();

    bd22GetOId (Trans, Fileno, KeyLen, pKey, ResultOid, ErrorList);
 
    bool bTryAgain = true;
    while ((e_ok == Trans.trError_gg00) && bTryAgain)  
    {
        ResultObjVers.gg91SetNilRef(); 
        bd04GetObjConsistent (Trans, ObjFileId, ResultOid,
            ExtObjBodySize, pExtObjBody, KeyLen, pKey, bDoLock, bShareLock,
            AuxObjBodyLen, AuxKeyLen, &pAuxKey, ResultObjVers, UpdTransId, 
            LogReadAccesses, ErrorList);

        if (e_wait_for_lock_release == Trans.trError_gg00)
        {   
            Trans.trError_gg00 = e_ok;
            kb50WaitAndRetryObjLock (Trans, m_lock, mm_object);
            bTryAgain = (e_ok == Trans.trError_gg00);  
        }
        else
            bTryAgain = false;
    };
            
    /* PTS 1114891 FF 2002-03-14 */
    bd04_MapPhysicalOntoLogicalError (Trans);   
    //if ( e_too_old_oid == Trans.trError_gg00 )
    //    Trans.trError_gg00 = e_object_not_found;

    if (g01vtrace.vtrOmsGet_gg00)
    {
        bd04_VtraceReturn (Trans, ObjFileId, ResultOid, ResultObjVers,
            b04GetObjWithKey, ot2OutputSeq_egg12);
    }
}
 
/*---------------------------------------------------------------------------*/
 
externCpp void
bd04GetObjFileStatistics (tgg00_TransContext  &Trans,
                          tgg00_FileId        &ObjFileId,
                          tsp00_Int4          &NumAllPages,
                          tsp00_Int4          &NumFreePages,
                          tsp00_Int4          &NumEmptyPages,
                          tsp00_Int4          &NumKeyLeaves,        /* PTS 1107153 AK 17/07/2000 */
                          tsp00_Int4          &Occupancy)           /* PTS 1107153 AK 17/07/2000 */ 
{
    SAPDBTRACE_ROUTINE_DEBUG("bd04GetObjFileStatistics", ObjContainer_Trace, 5);
 
    /*  This function returns the number af all pages, the number    */
    /*  of all pages containing free and free_after_eot objects and  */
    /*  the number of completely empty pages included in the file    */
 
    if (g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdObject_gg00)
    {
        tgg12_OmsFilenameTrace TrLineIn;
        
        TrLineIn.ofnTrType_gg12.becomes(otFilename_egg12);
        TrLineIn.ofnTrType2_gg12.becomes(ot2Nil_egg12);
        TrLineIn.ofnFileName_gg12 = ObjFileId.fileName_gg00();
        
        Trace_CommonEntry entry( tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(b04info), 
            REINTERPRET_CAST( tsp00_BytePtr, &TrLineIn ), sizeof (TrLineIn) );
    }
 
#   if COMPILEMODE_MEO00 >= SLOW_MEO00
    t01treeid (bi, "ObjFileId   ", ObjFileId);
#   endif
    
    BD04_CHECK_FILE_SYSTEM   (Trans, ObjFileId, b04info);
    BD04_CHECK_OBJ_FILE_TYPE (Trans, ObjFileId, b04info);
    
    cbd04_ObjCurrent Current (Trans, ObjFileId, m_select);
    
    if (e_ok == Trans.trError_gg00 )
    {
        bd90GetObjFileStatistics (Current, NumAllPages, NumFreePages, 
            NumEmptyPages, NumKeyLeaves, Occupancy);
    }
    
    if (g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdObject_gg00)
    {
        if (e_ok == Trans.trError_gg00)
        {
            tgg12_OmsFileInfoTrace TrLineInfo;
            
            TrLineInfo.oinfTrType_gg12.becomes(otFileInfo_egg12);
            TrLineInfo.oinfTrType2_gg12.becomes(ot2Nil_egg12);
            TrLineInfo.oinfNumAllPages_gg12     = NumAllPages;
            TrLineInfo.oinfNumFreePages_gg12    = NumFreePages;
            TrLineInfo.oinfNumEmptyPages_gg12   = NumEmptyPages;
            TrLineInfo.oinfRoot_gg12            = ObjFileId.fileRoot_gg00();
            
            Trace_CommonEntry entry( tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(b04info), 
                REINTERPRET_CAST( tsp00_BytePtr, &TrLineInfo ), sizeof (TrLineInfo) );
        }
        else
        {
            bd04_VtraceErrClass (Trans, ObjFileId, b04info, WRITE_VTRACE);
        }
    }
}

/*----------------------------------------------------------------------------*/

externCpp bool 
bd04GetNewFileId(tgg00_TransContext &Trans, FileDir_FileNo &objFileNo)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd04GetNewFileId", ObjContainer_Trace, 5);
    objFileNo = FileDir_IDirectory::SessionInstance(Trans).GetNewFileNo();
    return true;
};

/*----------------------------------------------------------------------------*/

externCpp void 
bd04vStateFileversion (tgg00_TransContext &Trans,
                       tgg00_FileId       &FileId,
                       Msg_List           &ErrorList)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd04vStateFileversion", ObjContainer_Trace, 5);

    FileDir_IDirectory &IFDC = FileDir_IDirectory::SessionInstance(Trans);
    if (e_ok == Trans.trError_gg00)
    {
        FileDir_Oms   fd_smart;
        const FileDir_FileNo fileno = FileId.fileDirFileId_gg00();
        FileDir_ReturnCode  fd_rc = IFDC.GetFile(
            Trans.trTaskId_gg00, fileno, fd_smart, ErrorList);
        if (FileDir_Okay != fd_rc) { 
            bd900MapFileDirError(Trans, fd_rc, e_file_not_found, (char*)__FILE__,  __LINE__ );
            return;
        }
        fd_smart.BuildFileId(FileId);
    }
    else
        Trans.trError_gg00 = e_file_not_found; 
};

/*---------------------------------------------------------------------------*/
externCpp void
bd04IsObjHistoryUsed (tgg00_TransContext &Trans,
                      tgg00_FileId       &ObjFileId,
                      tgg92_KernelOid     OId,
                      bool                ignoreVersions,   /* PTS 1130354 */
                      bool               &bHistoryIsUsed,
                      SAPDB_UInt8        &OldestTransNo,    /* PTS 1130354 */
                      Msg_List           &ErrorList)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd04IsObjHistoryUsed", ObjContainer_Trace, 5);
 
    if (g01vtrace.vtrOmsGet_gg00)
    {
        tgg12_OmsOidTrace TrLineOIdSeq;
        TrLineOIdSeq.ooidTrType_gg12.becomes(otOid_egg12);
        TrLineOIdSeq.ooidTrType2_gg12.becomes(ot2Input_egg12);
        TrLineOIdSeq.ooidObjId_gg12   = OId;
        
        Trace_CommonEntry entry( tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(b04IsHistUsed), 
            REINTERPRET_CAST( tsp00_BytePtr, &TrLineOIdSeq ), sizeof (TrLineOIdSeq) );
    }
 
#   if COMPILEMODE_MEO00 >= SLOW_MEO00
    t01treeid (bi, "ObjFileId   ", ObjFileId);
    ta01Oid   (bi, "OID         ", OId);
#   endif
 
    BD04_CHECK_FILE_SYSTEM   (Trans, ObjFileId, b04IsHistUsed);
    BD04_CHECK_OBJ_FILE_TYPE (Trans, ObjFileId, b04IsHistUsed);
    BD04_CHECK_OID           (Trans, ObjFileId, b04IsHistUsed, OId);
  
    cbd04_ObjCurrent Current (Trans, ObjFileId, m_select);
 
    bool bTryAgain = true;
    while ((e_ok == Trans.trError_gg00) && bTryAgain)  
    {
        bd90IsObjHistoryUsed (Current, OId, ignoreVersions, bHistoryIsUsed, OldestTransNo, ErrorList);
        if (e_wait_for_lock_release == Trans.trError_gg00)
        {   
            Trans.trError_gg00 = e_ok;
            kb50WaitAndRetryObjLock (Trans, m_lock, mm_object);
            bTryAgain = (e_ok == Trans.trError_gg00);  
        }
        else
            bTryAgain = false;
    };
  
    bd04_MapPhysicalOntoLogicalError (Trans);

#   if COMPILEMODE_MSP00 >= SLOW_MSP00
    t01bool (bi, "bHistIsUsed ", bHistoryIsUsed);
#   endif

    /* write trace entry  */
    bd04_VtraceErrClass (Trans, ObjFileId, b04IsHistUsed,
        (g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdObject_gg00));
}

                      
/*-----------------------------------------------------------------------------------*/
/* PTS 1001745 UH 23-11-1998 new */

externCpp void
bd04IsObjSelfLocked (tgg00_TransContext& Trans,
                     tgg00_FileId&       ObjFileId,
                     tgg92_KernelOid&    OId,
					 bool                bShared,
                     bool&               IsSelfLocked,
                     Msg_List&           ErrorList)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd04IsObjSelfLocked", ObjContainer_Trace, 5);
 
    if (g01vtrace.vtrOmsGet_gg00)
    {
        tgg12_OmsOidTrace TrLineOIdSeq;
        TrLineOIdSeq.ooidTrType_gg12.becomes(otOid_egg12);
        TrLineOIdSeq.ooidTrType2_gg12.becomes(ot2Input_egg12);
        TrLineOIdSeq.ooidObjId_gg12   = OId;
        
        Trace_CommonEntry entry( tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(b04IsSelfLocked), 
            REINTERPRET_CAST( tsp00_BytePtr, &TrLineOIdSeq ), sizeof (TrLineOIdSeq) );
    }
 
#   if COMPILEMODE_MEO00 >= SLOW_MEO00
    t01treeid (bi, "ObjFileId   ", ObjFileId);
    ta01Oid   (bi, "OID         ", OId);
#   endif
 
    BD04_CHECK_FILE_SYSTEM   (Trans, ObjFileId, b04IsSelfLocked);
    BD04_CHECK_OBJ_FILE_TYPE (Trans, ObjFileId, b04IsSelfLocked);
    BD04_CHECK_OID           (Trans, ObjFileId, b04IsSelfLocked, OId);
  
    cbd04_ObjCurrent Current (Trans, ObjFileId, m_select);
 
    bool bTryAgain = true;
    while ((e_ok == Trans.trError_gg00) && bTryAgain)  
    {
        bd90IsObjSelfLocked (Current, OId, bShared, IsSelfLocked, ErrorList);
        if (e_wait_for_lock_release == Trans.trError_gg00)
        {   
            Trans.trError_gg00 = e_ok;
            kb50WaitAndRetryObjLock (Trans, m_lock, mm_object);
            bTryAgain = (e_ok == Trans.trError_gg00);  
        }
        else
            bTryAgain = false;
    };
  
    bd04_MapPhysicalOntoLogicalError (Trans);

#   if COMPILEMODE_MSP00 >= SLOW_MSP00
    t01bool (bi, "IsSelfLocked", IsSelfLocked);
#   endif

    bd04_VtraceErrClass (Trans, ObjFileId, b04IsSelfLocked,
        (g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdObject_gg00));
}
 
/*---------------------------------------------------------------------------*/
 
externCpp void
bd04LockObj (tgg00_TransContext& Trans,
             tgg00_FileId&       ObjFileId,
             tgg92_KernelOid&    OId,
             tgg91_PageRef&      WantedObjVers,
             Msg_List&           ErrorList) 
{
    SAPDBTRACE_ROUTINE_DEBUG("bd04LockObj", ObjContainer_Trace, 5);
 
 
    if (g01vtrace.vtrOmsUpd_gg00 )
    {
        tgg12_OmsOidTrace TrLineOIdSeq;
 
        TrLineOIdSeq.ooidTrType_gg12.becomes(otOid_egg12);
        TrLineOIdSeq.ooidTrType2_gg12.becomes(ot2InputSeqVers_egg12);
        TrLineOIdSeq.ooidObjId_gg12    = OId;
        TrLineOIdSeq.ooidObjVers_gg12  = WantedObjVers;
        TrLineOIdSeq.ooidConsView_gg12 = Trans.trConsistView_gg00;

        Trace_CommonEntry entry( tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(b04lock), 
            REINTERPRET_CAST( tsp00_BytePtr, &TrLineOIdSeq ), sizeof (TrLineOIdSeq) );
    }
 
 
#   if COMPILEMODE_MEO00 >= SLOW_MEO00
    t01treeid   (bi, "ObjFileId   ", ObjFileId);
    ta01Oid     (bi, "OID         ", OId);
    t01page_ref (bi, "Inp ObjVers ", WantedObjVers);
#   endif
 
    BD04_CHECK_FILE_SYSTEM   (Trans, ObjFileId, b04lock);
    BD04_CHECK_OBJ_FILE_TYPE (Trans, ObjFileId, b04lock);
    BD04_CHECK_OID           (Trans, ObjFileId, b04lock, OId);
    
    cbd04_ObjCurrent Current (Trans, ObjFileId, m_update);
    
    bool bTryAgain = true;
    while ((e_ok == Trans.trError_gg00) && bTryAgain)  
    {        
        bd90LockObj (Current, OId, WantedObjVers, ErrorList);
        if (e_wait_for_lock_release == Trans.trError_gg00)
        {   
            Trans.trError_gg00 = e_ok;
            kb50WaitAndRetryObjLock (Trans, m_lock, mm_object);
            bTryAgain = (e_ok == Trans.trError_gg00);  
        }
        else
            bTryAgain = false;
    }
        
    bd04_MapPhysicalOntoLogicalError (Trans);   
    if ( e_object_not_found == Trans.trError_gg00 )                
       Trans.trError_gg00 = e_object_dirty;

    if (g01vtrace.vtrOmsUpd_gg00)
        bd04_VtraceReturn (Trans, Current.curr_tree_id, OId, WantedObjVers, b04lock, ot2Output_egg12);
}

/*---------------------------------------------------------------------------*/
 
externCpp void
bd04LockShareObj (tgg00_TransContext& Trans,
                  tgg00_FileId&       ObjFileId,
                  tgg92_KernelOid&    OId,
                  tgg91_PageRef&      WantedObjVers,
                  Msg_List&           ErrorList)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd04LockShareObj", ObjContainer_Trace, 5);
 
    if (g01vtrace.vtrOmsUpd_gg00 )
    {
        tgg12_OmsOidTrace TrLineOIdSeq;
 
        TrLineOIdSeq.ooidTrType_gg12.becomes(otOid_egg12);
        TrLineOIdSeq.ooidTrType2_gg12.becomes(ot2InputSeqVers_egg12);
        TrLineOIdSeq.ooidObjId_gg12    = OId;
        TrLineOIdSeq.ooidObjVers_gg12  = WantedObjVers;
        TrLineOIdSeq.ooidConsView_gg12 = Trans.trConsistView_gg00;

        Trace_CommonEntry entry( tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(b04lock), 
            REINTERPRET_CAST( tsp00_BytePtr, &TrLineOIdSeq ), sizeof (TrLineOIdSeq) );
    }
 
 
#   if COMPILEMODE_MEO00 >= SLOW_MEO00
    t01treeid   (bi, "ObjFileId   ", ObjFileId);
    ta01Oid     (bi, "OID         ", OId);
    t01page_ref (bi, "Inp ObjVers ", WantedObjVers);
#   endif
 
    BD04_CHECK_FILE_SYSTEM   (Trans, ObjFileId, b04lock);
    BD04_CHECK_OBJ_FILE_TYPE (Trans, ObjFileId, b04lock);
    BD04_CHECK_OID           (Trans, ObjFileId, b04lock, OId);
    
    cbd04_ObjCurrent Current (Trans, ObjFileId, m_update);
    
    bool bTryAgain = true;
    while ((e_ok == Trans.trError_gg00) && bTryAgain)  
    {        
        bd90LockShareObj (Current, OId, WantedObjVers, ErrorList);
        if (e_wait_for_lock_release == Trans.trError_gg00)
        {   
            Trans.trError_gg00 = e_ok;
            kb50WaitAndRetryObjLock (Trans, m_lock, mm_object);
            bTryAgain = (e_ok == Trans.trError_gg00);  
        }
        else
            bTryAgain = false;
    }
        
    bd04_MapPhysicalOntoLogicalError (Trans);   
    if ( e_object_not_found == Trans.trError_gg00 )                
       Trans.trError_gg00 = e_object_dirty;

    if (g01vtrace.vtrOmsUpd_gg00)
        bd04_VtraceReturn (Trans, Current.curr_tree_id, OId, WantedObjVers, b04lock, ot2Output_egg12);
}

/*---------------------------------------------------------------------------*/
 
externCpp void
bd04NewObjKey (tgg00_TransContext   &Trans,
               tgg00_FileId         &ObjFileId,
               tsp00_Int4            KeyLen,
               void                 *pKey,
               tgg92_KernelOid       OId,            
               tgg92_KernelOid      &ReUsedOId,
               tgg91_PageRef        &NewObjVers,
               Msg_List             &ErrorList)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd04NewObjKey", ObjContainer_Trace, 5);
 
    if (g01vtrace.vtrOmsNew_gg00 )
        bd92VtraceObjKey (Trans, ObjFileId.fileDirFileId_gg00(), KeyLen, pKey, b04newhash);
 
    tsp00_Int4       StartTime   = 0;
    tsp00_Int4       WaitingTime = Trans.trObjReqTimeout_gg00 * 1000; //msec
    bool             bTryAgain   = false;
    bool             bReCreateAddIndex = false;  // PTS 1134645

    BD04_CHECK_FILE_SYSTEM   (Trans, ObjFileId, b04newhash); 
    BD04_CHECK_OBJ_FILE_TYPE (Trans, ObjFileId, b04newhash); 
    BD04_CHECK_OID           (Trans, ObjFileId, b04newhash, OId);
    
    if (0 == WaitingTime) WaitingTime = MIN_RETRY_TIME;
    
    FileDir_FileNo  objFileNo = ObjFileId.fileDirFileId_gg00();
    cbd911RecreateIndexLock lock(Trans);  
    if (lock.IsNeeded(objFileNo)) 
        lock.Lock( !bExclusive );
    
    do
    {
        ReUsedOId.gg92SetNil(); // PTS 1105663 Alexander Kley 29/02/2000

        bd22InsertOId (Trans, objFileNo, KeyLen, pKey, OId, ReUsedOId, ErrorList);
        
        if (e_ok == Trans.trError_gg00) break;
        
        if (e_duplicate_hash_key != Trans.trError_gg00)
            bTryAgain = false;
        else
        {
            Trans.trError_gg00 = e_ok;
            bd04_ReuseObj (Trans, ObjFileId, ReUsedOId, NewObjVers, KeyLen, pKey, bTryAgain, ErrorList);   
            
            if (bTryAgain)
            {                
                if (e_wait_for_lock_release == Trans.trError_gg00)
                {   
                    Trans.trError_gg00 = e_ok;
                    kb50WaitAndRetryObjLock (Trans, m_new, mm_object);
                    bTryAgain = (e_ok == Trans.trError_gg00);  
                }
                else
                {
                    bd04_Reschedule (Trans, StartTime, WaitingTime, bTryAgain);
                    
                    if (! bTryAgain && e_ok == Trans.trError_gg00) 
                        Trans.trError_gg00 = e_cancelled;
                }
            }
            else if ((e_ok == Trans.trError_gg00) && !bTryAgain)
                bReCreateAddIndex = true;
        }
    }
    while (bTryAgain);
  
    if (lock.IsNeeded(objFileNo) && bReCreateAddIndex) 
    {
        tgg92_KernelOid    DummyOId;
        bd22InsertOIdReCreate (Trans, objFileNo, KeyLen, pKey, ReUsedOId, DummyOId, ErrorList);
    }
    
    if (g01vtrace.vtrOmsNew_gg00 || ((Trans.trError_gg00 != e_ok) && (Trans.trError_gg00 != e_duplicate_hash_key)))
    {
        if (! ReUsedOId.gg92IsNilPno() )
            bd04_VtraceReturn (Trans, ObjFileId, ReUsedOId, NewObjVers, b04newhash, ot2Output_egg12);
        else
            bd04_VtraceReturn (Trans, ObjFileId, OId, NewObjVers, b04newhash, ot2Output_egg12);
    }
}

/*-----------------------------------------------------------------------------------*/

externCpp void
bd04LockUpdObj (tgg00_TransContext&   Trans,
                tgg00_FileId&         ObjFileId,
                tgg92_KernelOid&      OId,
                tgg91_PageRef&        WantedObjVers,
                Msg_List&             ErrorList)
{
    /* PTS 1109298 UH 2001-01-26 new */
    SAPDBTRACE_ROUTINE_DEBUG("bd04LockUpdObj", ObjContainer_Trace, 5);
 
    if (g01vtrace.vtrOmsUpd_gg00 )
    {
        tgg12_OmsOidTrace TrLineOIdSeq;
 
        TrLineOIdSeq.ooidTrType_gg12.becomes(otOid_egg12);
        TrLineOIdSeq.ooidTrType2_gg12.becomes(ot2InputSeqVers_egg12);
        TrLineOIdSeq.ooidObjId_gg12    = OId;
        TrLineOIdSeq.ooidObjVers_gg12  = WantedObjVers;
        TrLineOIdSeq.ooidConsView_gg12 = Trans.trConsistView_gg00;

        Trace_CommonEntry entry( tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(vttBdLockUpd_egg00), 
            REINTERPRET_CAST( tsp00_BytePtr, &TrLineOIdSeq ), sizeof (TrLineOIdSeq) );
    }
  
    BD04_CHECK_FILE_SYSTEM   (Trans, ObjFileId, vttBdLockUpd_egg00);
    BD04_CHECK_OBJ_FILE_TYPE (Trans, ObjFileId, vttBdLockUpd_egg00);
    BD04_CHECK_OID           (Trans, ObjFileId, vttBdLockUpd_egg00, OId);
    
    cbd04_ObjCurrent Current (Trans, ObjFileId, m_update);
    
    bool bTryAgain = true;
    while ((e_ok == Trans.trError_gg00) && bTryAgain)  
    {        
        bd90LockUpdObj (Current, OId, WantedObjVers, ErrorList);
        if (e_wait_for_lock_release == Trans.trError_gg00)
        {   
            Trans.trError_gg00 = e_ok;
            kb50WaitAndRetryObjLock (Trans, m_lock, mm_object);
            bTryAgain = (e_ok == Trans.trError_gg00);  
        }
        else
            bTryAgain = false;
    }
        
    bd04_MapPhysicalOntoLogicalError (Trans);
    if ( e_object_not_found == Trans.trError_gg00 )                
       Trans.trError_gg00 = e_object_dirty;

    if (g01vtrace.vtrOmsUpd_gg00)
    {
        bd04_VtraceReturn (Trans, Current.curr_tree_id, OId, WantedObjVers, 
            vttBdLockUpd_egg00, ot2Output_egg12);
    }
}
 
/*---------------------------------------------------------------------------*/
 
externCpp void
bd04NewObj (tgg00_TransContext     &Trans,
            tgg00_FileId           &ObjFileId,
            tsp00_Int4              KeyLen,
            void                   *pKey,
            SAPDB_Int1              contObj,
            tgg92_KernelOid        &ResultOId,
            tgg91_PageRef          &ResultObjVers,
            Msg_List               &ErrorList)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd04NewObj", ObjContainer_Trace, 5);
 
    /* This function provides a new OId and reserves */
    /* the corresponding object frame                */

    if (g01vtrace.vtrOmsNew_gg00 )
    {
        tgg12_OmsFilenameTrace TrLineIn;
        
        TrLineIn.ofnTrType_gg12.becomes(otFilename_egg12);
        TrLineIn.ofnTrType2_gg12.becomes(ot2Nil_egg12);
        TrLineIn.ofnFileName_gg12 = ObjFileId.fileName_gg00();
        
        Trace_CommonEntry entry( tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(b04new), 
            REINTERPRET_CAST( tsp00_BytePtr, &TrLineIn ), sizeof (TrLineIn) );
    }
     
    BD04_CHECK_FILE_SYSTEM   (Trans, ObjFileId, b04new);
    BD04_CHECK_OBJ_FILE_TYPE (Trans, ObjFileId, b04new);
        
    cbd04_ObjCurrent Current (Trans, ObjFileId, m_update);

    bool bTryAgain = true;
    while ((e_ok == Trans.trError_gg00) && bTryAgain)  
    {      
        bd90NewObj (Current, KeyLen, pKey, contObj, ResultOId, ResultObjVers, ErrorList);
        if (e_wait_for_lock_release == Trans.trError_gg00)
        {   
            Trans.trError_gg00 = e_ok;
            kb50WaitAndRetryObjLock (Trans, m_lock, mm_object);
            bTryAgain = (e_ok == Trans.trError_gg00);  
        }
        else
            bTryAgain = false;
    }
    
    if (g01vtrace.vtrOmsNew_gg00)
        bd04_VtraceReturn (Trans, ObjFileId, ResultOId, ResultObjVers, b04new, ot2OutputSeq_egg12);

    BD04_CHECK_OID     (Trans, ObjFileId, b04new, ResultOId);
}
 
/*---------------------------------------------------------------------------*/

externCpp void
bd04NextObjConsistent (tgg00_TransContext     &Trans,
                       tgg00_FileId           &ObjFileId,
                       tgg92_KernelOid        &CurrOId,          //[inp,out]
                       bool                    bWithObject,      
                       tsp00_Int4              ExtObjBodySize,   //[inp=size of buffer supplied for the object]
                       void                   *pExtObjBody,
                       tsp00_Int4             &ObjBodySize,      //[out=real size of object]
                       tgg91_PageRef          &ResultObjVers,    
                       tsp00_Int4             &LogReadAccesses,  //[out] 
                       Msg_List               &ErrorList)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd04NextObjConsistent", ObjContainer_Trace, 5);
 
    /* This routine supplies the next object (following the given CurrOId)  */
    /* which was commited before the Current Trans had been started         */
    /* in case bWithObject is false only the next OId is returned otherwise */
    /* the next OId and the object body is supplied                         */

    if (g01vtrace.vtrOmsGet_gg00 )
    {
        tgg12_OmsOidTrace TrLineOIdSeq;
        
        TrLineOIdSeq.ooidTrType_gg12.becomes(otOid_egg12);
        TrLineOIdSeq.ooidTrType2_gg12.becomes(ot2InputVers_egg12);
        TrLineOIdSeq.ooidObjId_gg12    = CurrOId;
        TrLineOIdSeq.ooidConsView_gg12 = Trans.trConsistView_gg00;
        
        Trace_CommonEntry entry( tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(b04next_consistent), 
            REINTERPRET_CAST( tsp00_BytePtr, &TrLineOIdSeq ), sizeof (TrLineOIdSeq) );
    }
 
#   if COMPILEMODE_MEO00 >= SLOW_MEO00
    t01treeid (bi, "ObjFileId   ", ObjFileId);
    ta01Oid   (bi, "OID         ", CurrOId);
#   endif
    
    BD04_CHECK_FILE_SYSTEM   (Trans, ObjFileId, b04next_consistent);
    BD04_CHECK_OBJ_FILE_TYPE (Trans, ObjFileId, b04next_consistent);
        
    cbd04_ObjCurrent Current (Trans, ObjFileId, m_select);
    
    bool bTryAgain = true;
    while ((e_ok == Trans.trError_gg00) && bTryAgain)  
    {        
        do {
            bd90NextObjConsistent (Current, CurrOId, bWithObject,
                ExtObjBodySize, pExtObjBody, ObjBodySize, ResultObjVers, 
                LogReadAccesses, ErrorList);
        } while (e_bd_page_collision == Trans.trError_gg00);

        if (e_wait_for_lock_release == Trans.trError_gg00)
        {   
            Trans.trError_gg00 = e_ok;
            kb50WaitAndRetryObjLock (Trans, m_lock, mm_object);
            bTryAgain = (e_ok == Trans.trError_gg00);  
        }
        else
            bTryAgain = false;
    }
    
    /* PTS 1114891 FF 2002-03-14 */
    bd04_MapPhysicalOntoLogicalError (Trans);   

    if (g01vtrace.vtrOmsGet_gg00)
    {
        bd04_VtraceReturn (Trans, Current.curr_tree_id, CurrOId, ResultObjVers,
            b04next_consistent, ot2OutputSeq_egg12);
    }
}
   
/*---------------------------------------------------------------------------*/

externCpp void
bd04NextOidsConsistent (tgg00_TransContext     &Trans,
                        tgg00_FileId           &ObjFileId,
                        bool                    bOnlyHdrVarObj,
                        const tgg92_KernelOid  &CurrOid, 
                        tsp00_Int4             &noOfOid,
                        tgg92_KernelOid        *pOid,
                        tgg91_PageRef          *pObjVers,
                        tsp00_Int4             &LogReadAccesses,  
                        Msg_List               &ErrorList)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd04NextOidsConsistent", ObjContainer_Trace, 5);
 
    /* This routine supplies the next oids (following the given CurrOId)    */
    /* which was committed before the Current Trans had been started.        */

    if (g01vtrace.vtrOmsGet_gg00 )
    {
        tgg12_OmsOidTrace TrLineOIdSeq;
        
        TrLineOIdSeq.ooidTrType_gg12.becomes(otOid_egg12);
        TrLineOIdSeq.ooidTrType2_gg12.becomes(ot2InputVers_egg12);
        TrLineOIdSeq.ooidObjId_gg12    = CurrOid;
        TrLineOIdSeq.ooidConsView_gg12 = Trans.trConsistView_gg00;
        
        Trace_CommonEntry entry( tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(b04next_consistent), 
            REINTERPRET_CAST( tsp00_BytePtr, &TrLineOIdSeq ), sizeof (TrLineOIdSeq) );
    }
 
#   if COMPILEMODE_MEO00 >= SLOW_MEO00
    t01treeid (bi, "ObjFileId   ", ObjFileId);
    ta01Oid   (bi, "OID         ", *(CONST_CAST(tgg92_KernelOid*, &CurrOid)));
#   endif
    
    BD04_CHECK_FILE_SYSTEM   (Trans, ObjFileId, b04next_consistent);
    BD04_CHECK_OBJ_FILE_TYPE (Trans, ObjFileId, b04next_consistent);
        
    cbd04_ObjCurrent Current (Trans, ObjFileId, m_select);
    
    bool bTryAgain = true;
    while ((e_ok == Trans.trError_gg00) && bTryAgain)  
    {        
        bd90NextOidsConsistent (Current, bOnlyHdrVarObj, CurrOid, noOfOid, pOid, pObjVers, 
            LogReadAccesses, ErrorList);
        if (e_wait_for_lock_release == Trans.trError_gg00)
        {   
            Trans.trError_gg00 = e_ok;
            kb50WaitAndRetryObjLock (Trans, m_lock, mm_object);
            bTryAgain = (e_ok == Trans.trError_gg00);  
        }
        else
            bTryAgain = false;
    }
    
    /* PTS 1114891 FF 2002-03-14 */
    bd04_MapPhysicalOntoLogicalError (Trans);   

    if (g01vtrace.vtrOmsGet_gg00)
    {
        for (int i=0; i<noOfOid; ++i){
            bd04_VtraceReturn (Trans, Current.curr_tree_id, pOid[i], pObjVers[i],
                b04next_consistent, ot2OutputSeq_egg12);
        }
    }
}
   
/*---------------------------------------------------------------------------*/

externCpp void
bd04NextObjFromKeyIterator (tgg00_TransContext   &Trans,
                            void                 *pUntypedKeyIterator,    
                            bool                  bInclusive,   
                            tsp00_Int4            objBodySize,
                            tsp00_Int4            KeyLen,        
                            void                 *pRestartKey, 
                            tsp00_Int4           &noOfOid,
                            tgg92_KernelOid      *pResultOId, 
                            tgg91_PageRef        *pObjVers,
                            tsp00_MoveObj        *pBody,
                            tsp00_Int4           &LogReadAccesses,
                            Msg_List             &ErrorList)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd04NextObjFromKeyIterator", ObjContainer_Trace, 5);

    /* This routine supplies the next object in the scope of the input */
    /* iterator in case bWithObject is false only the next OId is      */
    /* returned otherwise the next OId and the object body is supplied */              

    SAPDBERR_ASSERT_STATE (pUntypedKeyIterator != NULL);

    /* cast the input pointer to a iterator pointer */
    LVC_AbstractObjKeyIterator & KeyIterator = *((LVC_AbstractObjKeyIterator *) pUntypedKeyIterator);

    /* check if the casted iterator is really an iterator */
    SAPDBERR_ASSERT_STATE (KeyIterator.IsValid());

    if (g01vtrace.vtrOmsGet_gg00 ){
        tgg12_OmsFilenameTrace TrLineIn;

        TrLineIn.ofnTrType_gg12.becomes(otFilename_egg12);
        TrLineIn.ofnTrType2_gg12.becomes(ot2Nil_egg12);
        TrLineIn.ofnFileName_gg12 = KeyIterator.GetObjFileId().fileName_gg00();

        Trace_CommonEntry entry( tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(b04NextKey), 
            REINTERPRET_CAST( tsp00_BytePtr, &TrLineIn ), sizeof (TrLineIn) );
    }

    BD04_CHECK_FILE_SYSTEM   (Trans, KeyIterator.GetObjFileId(), b04next_consistent);
    BD04_CHECK_OBJ_FILE_TYPE (Trans, KeyIterator.GetObjFileId(), b04next_consistent);

    bool bTryAgain = true;
    while ((e_ok == Trans.trError_gg00) && bTryAgain)  
    {        
        bd90NextObjFromKeyIterator (Trans, KeyIterator,  bInclusive, objBodySize, KeyLen, pRestartKey,
            noOfOid, pResultOId, pObjVers, pBody, LogReadAccesses, ErrorList);
        if (e_wait_for_lock_release == Trans.trError_gg00)
        {   
            Trans.trError_gg00 = e_ok;
            kb50WaitAndRetryObjLock (Trans, m_lock, mm_object);
            bTryAgain = (e_ok == Trans.trError_gg00);  
        }
        else
            bTryAgain = false;
    }

    /* PTS 1114891 FF 2003-01-07 */
    bd04_MapPhysicalOntoLogicalError (Trans);   

    if (g01vtrace.vtrOmsGet_gg00){
        for (int i=0; i<noOfOid; ++i){
            bd04_VtraceReturn (Trans, KeyIterator.GetObjFileId(), pResultOId[i], pObjVers[i],
                b04NextKey, ot2OutputSeq_egg12);
        }
    }
}

/*---------------------------------------------------------------------------*/

externCpp void
bd04PrevObjFromKeyIterator (tgg00_TransContext   &Trans,
                            void                 *pUntypedKeyIterator,    
                            bool                  bInclusive,  
                            tsp00_Int4            objBodySize,
                            tsp00_Int4            KeyLen,        
                            void                 *pRestartKey, 
                            tsp00_Int4           &noOfOid,
                            tgg92_KernelOid      *pResultOId, 
                            tgg91_PageRef        *pObjVers,
                            tsp00_MoveObj        *pBody,
                            tsp00_Int4           &LogReadAccesses,
                            Msg_List             &ErrorList)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd04PrevObjFromKeyIterator", ObjContainer_Trace, 5);

    /* This routine supplies the next object in the scope of the input */
    /* iterator in case bWithObject is false only the next OId is      */
    /* returned otherwise the next OId and the object body is supplied */              

    SAPDBERR_ASSERT_STATE (pUntypedKeyIterator != NULL);

    /* cast the input pointer to a iterator pointer */
    LVC_AbstractObjKeyIterator & KeyIterator = *((LVC_AbstractObjKeyIterator *) pUntypedKeyIterator);

    /* check if the casted iterator is really an iterator */
    SAPDBERR_ASSERT_STATE (KeyIterator.IsValid());

    if (g01vtrace.vtrOmsGet_gg00 ){
        tgg12_OmsFilenameTrace TrLineIn;

        TrLineIn.ofnTrType_gg12.becomes(otFilename_egg12);
        TrLineIn.ofnTrType2_gg12.becomes(ot2Nil_egg12);
        TrLineIn.ofnFileName_gg12 = KeyIterator.GetObjFileId().fileName_gg00();

        Trace_CommonEntry entry( tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(b04NextKey), 
            REINTERPRET_CAST( tsp00_BytePtr, &TrLineIn ), sizeof (TrLineIn) );
    }

    BD04_CHECK_FILE_SYSTEM   (Trans, KeyIterator.GetObjFileId(), b04next_consistent);
    BD04_CHECK_OBJ_FILE_TYPE (Trans, KeyIterator.GetObjFileId(), b04next_consistent);

    bool bTryAgain = true;
    while ((e_ok == Trans.trError_gg00) && bTryAgain)  
    {        
        bd90PrevObjFromKeyIterator (Trans, KeyIterator,  bInclusive, objBodySize, KeyLen, pRestartKey,
            noOfOid, pResultOId, pObjVers, pBody, LogReadAccesses, ErrorList);
        if (e_wait_for_lock_release == Trans.trError_gg00)
        {   
            Trans.trError_gg00 = e_ok;
            kb50WaitAndRetryObjLock (Trans, m_lock, mm_object);
            bTryAgain = (e_ok == Trans.trError_gg00);  
        }
        else
            bTryAgain = false;
    }

    /* PTS 1114891 FF 2003-01-07 */
    bd04_MapPhysicalOntoLogicalError (Trans);   

    if (g01vtrace.vtrOmsGet_gg00){
        for (int i=0; i<noOfOid; ++i){
            bd04_VtraceReturn (Trans, KeyIterator.GetObjFileId(), pResultOId[i], pObjVers[i],
                b04NextKey, ot2OutputSeq_egg12);
        }
    }
}
   
/*---------------------------------------------------------------------------*/

externC void
bd04OmsReadAheadParticipant (tgg00_TransContext  &Trans,
                             tgg00_FileId        &ObjFileId)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd04OmsReadAheadParticipant", ObjContainer_Trace, 5);   

#   if COMPILEMODE_MSP00 >= SLOW_MSP00
    t01treeid (bi, "ObjFileId   ", ObjFileId);
#   endif

    if (!b01downfilesystem)
    {
        cbd04_ObjCurrent Current (Trans, ObjFileId, m_get_object);

        if (e_ok == Trans.trError_gg00)
            bd90ReadAhead (Current);
    }
    Trans.trError_gg00 = e_ok; // ignore errors
}

/*---------------------------------------------------------------------------*/

externC void
bd04CancelOmsVersion ( tsp00_TaskId   TaskId
                      ,tgg91_TransNo &ConsistViewNo)
{
    /* function my be call on timer process without any session and transaction */ 
    if ( NULL == vGetAcvPtrFromCurrentTask())
    {
        Msg_List errlist;

        /* create a new session & transaction context */
        tgg00_TransContext Trans;
        tgg00_TransContext AuxTrans;
        SAPDB_MemFill( &Trans, 0x00, sizeof(tgg00_TransContext), "bd04CancelOmsVersion", __CONTEXT__, errlist);
        Trans.trTaskId_gg00 = TaskId;
        Trans.trError_gg00  = e_ok;
        Trans.trState_gg00.clear();

        /* allocator isneeded by local filedirectory */
        SAPDB_UTF8 identifier[40];
        SAPDB_sprintf((char *)identifier, sizeof(identifier), "TransContext(intern) T%03d", TaskId);
        SAPDBMem_RawAllocator *pAllocator = new(RTEMem_Allocator::Instance()) SAPDBMem_RawAllocator(
            identifier,
            RTEMem_BlockAllocator::Instance(), 
            1000,
            1000,
            SAPDBMem_RawAllocator::FREE_RAW_EXTENDS_EXCEPT_INITIAL); 
        Trans.trAllocator_gg00  = pAllocator;

        tsp00_RteCommRec rteComRec;
        rteComRec.to_cancel  = FALSE;
        rteComRec.virt_reads = 0;
        rteComRec.file_root  = 0;
        rteComRec.file_record_cnt = 0;
        Trans.trRteCommPtr_gg00 = &rteComRec;

        Trans.trLocalFileDirPtr_gg00 = NULL;
        k53StartInternalSession (Trans, AuxTrans, false, false);

        /* provide ACV and ILiveCacheSink for CancelOmsVersion request */
        /* needed to be able to remove OmsVersion TempFiles & to call  */
        /* vbegexcl & vendexcl                                         */
        tak_all_command_glob *acv = reinterpret_cast<tak_all_command_glob*>(
            pAllocator->Allocate(sizeof(tak_all_command_glob))); 
        if ( NULL != acv)
        {
            if ( SAPDB_MemFill( acv, 0x00, sizeof(tak_all_command_glob), "bd04CancelOmsVersion", __CONTEXT__, errlist) )
            {
                Trans.trErrorList_gg00    = &errlist;
                acv->a_transinf.tri_trans = Trans;

                /* anchor ACV within TCB of runtime */
                vinit_akdump_ptr ( acv, NULL /* no dump */ );

                tsp_dcom_init_sink_parms sink_info;
                sink_info.acv_addr      = reinterpret_cast<tsp00_Addr>(acv);
                sink_info.sink_addr     = reinterpret_cast<tsp00_Addr>(&(acv->a_kernel_sink));
                sink_info.sink_size     = sizeof (acv->a_kernel_sink);
                vdcom_init_sink (&sink_info);

                ak341CancelOmsVersion(ConsistViewNo);

                vinit_akdump_ptr ( NULL, NULL );
            }
            pAllocator->Deallocate(acv);
        }
        k53StopInternalSession (Trans, AuxTrans, m_commit);
        destroy(pAllocator,RTEMem_Allocator::Instance());
    }
    else
        ak341CancelOmsVersion(ConsistViewNo);
};

/*---------------------------------------------------------------------------*/
 
externCpp void
bd04RollbackObj (tgg00_TransContext  &Trans,
                 tgg00_MessType_Enum RollbackType,
                 tgg92_KernelOid      OId,
                 FileDir_FileNo       Fileno,     
                 tgg00_ObjState       NewState,
                 tgg91_PageRef        BeforeObjVers,
                 tgg91_TransNo        ObjUpdTransNo,
                 tsp00_Int4           BeforeObjSize,
                 void                *pBeforeObj,
                 SAPDB_Int1           contVarObject,
                 bool                 bOmsLayerReusedObj)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd04RollbackObj", ObjContainer_Trace, 5);   
 
    Msg_List        ErrorList;

    /* This is the inverse function of bd04Update  */

    if (g01vtrace.vtrOmsUpd_gg00 )
    {
        tgg12_OmsOidTrace TrLineOId;

        TrLineOId.ooidMessType_gg12.becomes(RollbackType);
        TrLineOId.ooidMessType2_gg12.becomes(mm_nil);
        TrLineOId.ooidTrType_gg12.becomes(otOid_egg12);
        TrLineOId.ooidTrType2_gg12.becomes(ot2InputSeqVers_egg12);
        TrLineOId.ooidObjId_gg12     = OId;
        TrLineOId.ooidObjVers_gg12   = BeforeObjVers;
        TrLineOId.ooidConsView_gg12  = Trans.trConsistView_gg00;

        Trace_CommonEntry entry( tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(b04rupd), 
            REINTERPRET_CAST( tsp00_BytePtr, &TrLineOId ), sizeof (TrLineOId) );
    }
    
    tgg00_FileId ObjFileId;
    ObjFileId.fileObjFileType_gg00().becomes (oftUnknownType_egg00);

    BD04_CHECK_FILE_SYSTEM   (Trans, ObjFileId, b04rupd);

#   if COMPILEMODE_MEO00 >= QUICK_MEO00
    /* check if OId is valid  */
    if (!bd04_OIdIsValid(OId))                                                  \
    {
        Trans.trError_gg00 = e_invalid_oid;
        bd04_VtraceErrClass (Trans, ObjFileId, b04rupd, WRITE_VTRACE);
        if  (! ErrorList.IsEmpty())
            RTE_Message(ErrorList);
        return;
    }
#   endif
    
    cbd04_ObjCurrent Current (Trans, ObjFileId, m_update);

    if (e_ok == Trans.trError_gg00)  
    {        
        bd90RollbackObj (Current, RollbackType, OId, Fileno, 
            NewState, BeforeObjVers, ObjUpdTransNo, BeforeObjSize, pBeforeObj,
            contVarObject, bOmsLayerReusedObj, ErrorList); 
    }

    /* write vtrace entry (CRS 1109559 AK 07/03/2001) */    
    if (g01vtrace.vtrOmsUpd_gg00 || (e_ok != Trans.trError_gg00))
        bd04_VtraceReturn (Trans, Current.curr_tree_id, OId, BeforeObjVers, b04rupd, ot2Output_egg12);

    if  ((e_ok != Trans.trError_gg00) && ! ErrorList.IsEmpty())
        RTE_Message(ErrorList);
}

/*---------------------------------------------------------------------------*/

externCpp void
bd04RollbackCreateObjContainer (tgg00_TransContext   &Trans,
                                const FileDir_FileNo &Fileno)
{
    Msg_List        errorList;
    FileDir_Oms     fd_smart;
    FileDir_IDirectory &IFDC = FileDir_IDirectory::SessionInstance(Trans);
    FileDir_ReturnCode  fd_rc = IFDC.GcGetFile(Trans.trTaskId_gg00, Fileno, fd_smart, errorList);
    if (FileDir_Okay != fd_rc) {
        bd900MapFileDirError(Trans, fd_rc, e_file_not_found, (char*)__FILE__,  __LINE__ );
    }
    else
    {
        fd_smart.MarkFileDeleted(Trans.trWriteTransId_gg00);
        if ((g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdObject_gg00)) 
        {
            Kernel_VTrace () << __FILE__ << ":" << __LINE__  
                << " MarkFileDeleted -"
                << " FileId:" << Fileno;
        }
    }

    if ((e_ok == Trans.trError_gg00) && (Data_OmsKeyFile == fd_smart.GetFileType()))
    {
        const tsp00_Int4 NumKeyFiles = fd_smart.GetKeyPartitionCount();
        for (tsp00_Int4 iKeyFileNo = 1; iKeyFileNo <=NumKeyFiles; ++iKeyFileNo)
        {
            FileDir_FileNo fd_fileid = fd_smart.GetKeyPartition(iKeyFileNo);
            if ( fd_fileid.IsValid() ) 
            {
                FileDir_Oms     fd_smart2;
                IFDC.GcGetFile(Trans.trTaskId_gg00, fd_fileid, fd_smart2, errorList);
                if (FileDir_Okay != fd_rc)
                {
                    fd_rc = FileDir_Okay; /* ignore errors */
                    Trans.trError_gg00 = e_ok;
                }
                else
                {
                    fd_smart2.MarkFileDeleted(Trans.trWriteTransId_gg00);
                    if ((g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdObject_gg00)) {
                        Kernel_VTrace () << __FILE__ << ":" << __LINE__  
                            << " RemoveFileDeletedMark -"
                            << " FileId:" << Fileno
                            << " KeyFileId:" << fd_fileid
                            << " KeyFileIndex:" << iKeyFileNo;
                    }
                    fd_smart2.Detach();
                }
            }
        }
    }

    cbd05_Current  Current (Trans, m_update);           
    fd_smart.BuildFileId(Current.curr_tree_id);
    bd95RemoveObjFile (Current);
};

/*---------------------------------------------------------------------------*/

externCpp void
bd04RollbackDropObjContainer (tgg00_TransContext   &Trans,
                              const FileDir_FileNo &Fileno)
{
    Msg_List        errorList;
    FileDir_Oms     fd_smart;
    FileDir_IDirectory &IFDC = FileDir_IDirectory::SessionInstance(Trans);
    FileDir_ReturnCode  fd_rc = IFDC.GcGetFile(Trans.trTaskId_gg00, Fileno, fd_smart, errorList);
    if (FileDir_Okay != fd_rc) {
        bd900MapFileDirError(Trans, fd_rc, e_file_not_found, (char*)__FILE__,  __LINE__ );
    }
    else
    {
        fd_smart.RemoveFileDeletedMark();
        if ((g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdObject_gg00)) 
        {
            Kernel_VTrace () << __FILE__ << ":" << __LINE__  
                << " RemoveFileDeletedMark -"
                << " FileId:" << Fileno;
        }
    }

    if ((e_ok == Trans.trError_gg00) && (Data_OmsKeyFile == fd_smart.GetFileType()))
    {
        const tsp00_Int4 NumKeyFiles = fd_smart.GetKeyPartitionCount();
        for (tsp00_Int4 iKeyFileNo = 1; iKeyFileNo <=NumKeyFiles; ++iKeyFileNo)
        {
            FileDir_FileNo fd_fileid = fd_smart.GetKeyPartition(iKeyFileNo);
            if ( fd_fileid.IsValid() ) 
            {
                FileDir_Oms     fd_smart2;
                IFDC.GcGetFile(Trans.trTaskId_gg00, fd_fileid, fd_smart2, errorList);
                if (FileDir_Okay != fd_rc) 
                {
                    bd900MapFileDirError(Trans, fd_rc, e_file_not_found, (char*)__FILE__,  __LINE__ );
                }
                else
                {
                    fd_smart2.RemoveFileDeletedMark();
                    if ((g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdObject_gg00)) {
                        Kernel_VTrace () << __FILE__ << ":" << __LINE__  
                            << " RemoveFileDeletedMark -"
                            << " FileId:" << Fileno
                            << " KeyFileId:" << fd_fileid
                            << " KeyFileIndex:" << iKeyFileNo;
                    }
                }
            }
        }
    }
};

/*---------------------------------------------------------------------------*/
 
externCpp void
bd04UnlockObj (tgg00_TransContext &Trans,
               tgg00_FileId       &ObjFileId,
               tgg92_KernelOid     OId,
               bool                bSharedLock)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd04UnlockObj", ObjContainer_Trace, 5);   

    /* this function unlocks an object */

    tgg91_PageRef DummySeq;
    DummySeq.gg91SetDummyRef();

    if (g01vtrace.vtrOmsUpd_gg00 )
    {
        tgg12_OmsOidTrace TrLineOIdSeq;

        TrLineOIdSeq.ooidTrType_gg12.becomes(otOid_egg12);
        TrLineOIdSeq.ooidTrType2_gg12.becomes(ot2Input_egg12);
        TrLineOIdSeq.ooidObjId_gg12   = OId;

        Trace_CommonEntry entry( tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(b04unlock), 
            REINTERPRET_CAST( tsp00_BytePtr, &TrLineOIdSeq ), sizeof (TrLineOIdSeq) );
    }

#   if COMPILEMODE_MEO00 >= SLOW_MEO00
    t01treeid (bi, "ObjFileId   ", ObjFileId);
    ta01Oid   (bi, "OID         ", OId);
#   endif

    BD04_CHECK_FILE_SYSTEM   (Trans, ObjFileId, b04unlock);
    BD04_CHECK_OBJ_FILE_TYPE (Trans, ObjFileId, b04unlock);
    BD04_CHECK_OID           (Trans, ObjFileId, b04unlock, OId);

    cbd04_ObjCurrent Current (Trans, ObjFileId, m_update);

    if (e_ok == Trans.trError_gg00)  
    {        
        bd90UnlockObj (Current, OId, bSharedLock);
    }

    bd04_MapPhysicalOntoLogicalError (Trans);

    if (g01vtrace.vtrOmsUpd_gg00)
        bd04_VtraceReturn (Trans, Current.curr_tree_id, OId, DummySeq, b04unlock, ot2Output_egg12);
}
 
/*---------------------------------------------------------------------------*/
 
externCpp void
bd04UpdateObj (tgg00_TransContext    &Trans,
               tgg00_FileId          &ObjFileId,
               tgg92_KernelOid        OId,
               tgg91_PageRef         &ObjVers,
               tsp00_Int4             NewObjBodySize,
               void                  *pNewObjBody,
               SAPDB_Int1             contObj,  
               bool                   bOmsLayerReusedObj,
               Msg_List              &ErrorList)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd04UpdateObj", ObjContainer_Trace, 5);
 
    if (g01vtrace.vtrOmsUpd_gg00 )
    {
        tgg12_OmsOidTrace TrLineOIdSeq;
        
        TrLineOIdSeq.ooidTrType_gg12.becomes(otOid_egg12);
        TrLineOIdSeq.ooidTrType2_gg12.becomes(ot2InputSeqVers_egg12);
        TrLineOIdSeq.ooidObjId_gg12    = OId;
        TrLineOIdSeq.ooidObjVers_gg12  = ObjVers;
        TrLineOIdSeq.ooidConsView_gg12 = Trans.trConsistView_gg00;
        
        Trace_CommonEntry entry( tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(b04upd), 
            REINTERPRET_CAST( tsp00_BytePtr, &TrLineOIdSeq ), sizeof (TrLineOIdSeq) );
    }
 
#   if COMPILEMODE_MEO00 >= SLOW_MEO00
    t01treeid   (bi, "ObjFileId   ", ObjFileId);
    ta01Oid     (bi, "OID         ", OId);
    t01int4     (bi, "Newsize     ", NewObjBodySize);
    t01page_ref (bi, "Inp ObjVers ", ObjVers);
#   endif
 
    BD04_CHECK_FILE_SYSTEM   (Trans, ObjFileId, b04upd);
    BD04_CHECK_OBJ_FILE_TYPE (Trans, ObjFileId, b04upd);
    BD04_CHECK_OID           (Trans, ObjFileId, b04upd, OId);
 
    cbd04_ObjCurrent Current (Trans, ObjFileId, m_update);

    bool bTryAgain = true;
    while ((e_ok == Trans.trError_gg00) && bTryAgain)  
    {        
        Log_EntrySequence DummyRedoEntrySequence;
        tgg91_TransNo     DummyRedoUpdTransNo;

        bd90UpdateObj (Current, OId, ObjVers, NewObjBodySize, pNewObjBody,
            DummyRedoEntrySequence, DummyRedoUpdTransNo, contObj, bOmsLayerReusedObj,
            false /*bForRollforward*/, ErrorList);
        if (e_wait_for_lock_release == Trans.trError_gg00)
        {   
            Trans.trError_gg00 = e_ok;
            kb50WaitAndRetryObjLock (Trans, m_lock, mm_object);
            bTryAgain = (e_ok == Trans.trError_gg00);  
        }
        else
            bTryAgain = false;
    }
        
    bd04_MapPhysicalOntoLogicalError (Trans);
    
    if (g01vtrace.vtrOmsUpd_gg00)
        bd04_VtraceReturn (Trans, Current.curr_tree_id, OId, ObjVers, b04upd, ot2OutputSeq_egg12);
}

/*===========================================================================*
 *  LOCAL FUNCTIONS (CODE)                                                   *
 *===========================================================================*/
inline
cbd04_ObjCurrent::cbd04_ObjCurrent (tgg00_TransContext   &Trans,
                                    tgg00_FileId         &ObjFileId,
                                    tgg00_MessType_Enum  MessType)
                                    :cbd05_Current(Trans, MessType),
                                    m_pFileId    (NULL)
{
    SAPDBTRACE_ROUTINE_DEBUG("cbd04_ObjCurrent::ctor", ObjContainer_Trace, 5);

    /* This function constructs the structure Current */

#   if COMPILEMODE_MEO00 >= SLOW_MEO00
    t01treeid (bi, "ObjFileId-> ", curr_tree_id);
#   endif

    if (oftUnknownType_egg00 == ObjFileId.fileObjFileType_gg00())
    {
        m_pFileId = &ObjFileId; 
        // the following line  is already included in the constructor of cbd05_Current
        // curr_tree_id = b01niltree_id; 
        curr_tree_id.fileContObjFileNo_gg00() = PRIM_CONT_OBJ_FILE_NO_BD900;
        curr_tree_id.fileTfn_gg00().becomes(tfnObj_egg00); 
        curr_tree_id.fileType_gg00().delElement (ftsTemp_egg00);
        curr_tree_id.fileType_gg00().addElement (ftsObject_egg00);      
        curr_tree_id.fileType_gg00().addElement (ftsConcurrent_egg00); 
        curr_tree_id.fileType_gg00().addElement (ftsPerm_egg00);
        curr_tree_id.fileObjFileType_gg00().becomes(oftUnknownType_egg00);
    }
    else
    {
#       if COMPILEMODE_MEO00 >= QUICK_MEO00 
        /* store file id to check it in the destructor */
        m_pFileId = &ObjFileId; 
#       endif

        /* check if the input file id corresponds to a correct */
        /* object file id PTS 1111948 AK 26/09/2001            */
        SAPDBERR_ASSERT_STATE (bd04_ObjFileIdIsValid(ObjFileId));   
        
        curr_tree_id = ObjFileId;
    }

#   if COMPILEMODE_MEO00 >= SLOW_MEO00
    t01treeid (bi, "ObjFileId<- ", curr_tree_id);
#   endif
}

/*---------------------------------------------------------------------------*/

inline static void 
bd04_MapPhysicalOntoLogicalError (tgg00_TransContext &Trans)
{
    tgg00_BasisError TrError = Trans.trError_gg00;
    if (e_ok != TrError)
    {
        if ((e_page_in_wrong_tree    == TrError) || 
            (e_no_converter_entry    == TrError) ||
            (e_inconsistent_nodetype == TrError) ||
            /* PTS 1114891 FF 2002-03-14 following checks added */
            (e_wrong_object_version  == TrError) ||
            (e_wrong_object_state    == TrError) ||
            (e_illegal_object_length == TrError) ||
            (e_too_old_oid           == TrError) ||
            /*(e_object_dirty          == TrError) ||  PTS 1119647 && PTS 1123210 */ 
            /*(e_container_dropped     == TrError) ||  PTS 1119647 && PTS 1139917 */ 
            /*(e_consview_cancelled    == TrError) ||  PTS 1120151 && PTS 1134026 */ 
            (e_obj_history_not_found == TrError) || /* PTS 1134026 */
            (e_unknown_consist_vers  == TrError) || /* PTS 1120151 FF 2003-JAN-20 */ 
            (e_illegal_object_pos    == TrError))
        {
            pasbool bCancelled  = false;            
            pasbool bConsistent = false;
            Trans.trError_gg00 = e_ok;
            if ((!Trans.trConsistView_gg00.gg90IsNil()) && (e_consview_cancelled != TrError))
                kb50ObjConsistentCheck (Trans, Trans.trWriteTransId_gg00, bConsistent, bCancelled);
            if (bCancelled ||(e_consview_cancelled == TrError))
            {
                if (g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdObject_gg00)
                {
                    Kernel_VTrace() << "b04_Map Error:" << TrError << " onto e_obj_history_not_found";
                }
                Trans.trError_gg00 = e_consview_cancelled;  /* PTS 1134026 */
            }
            else 
            {
                if (e_obj_history_not_found != TrError) /* PTS 1134026 */
                    Trans.trError_gg00 = e_object_not_found;

                if (g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdObject_gg00)
                {
                    Kernel_VTrace() << "b04_Map Error:" << TrError << " onto e_object_not_found";
                }
            }
        }
        else
            if (g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdObject_gg00)
            {
                Kernel_VTrace() << "b04_Map Error:" << TrError << " not mapped";
            }
    }
};

/*---------------------------------------------------------------------------*/
 
static _INLINE bool
bd04_OIdIsValid (tgg92_KernelOid  OId)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd04_OIdIsValid", ObjContainer_Trace, 5);
 
    /* This function checks if the OId  */
    /* meets some basic requirements    */
 
    return  ( ! OId.gg92IsNil()                                             &&
#               if COMPILEMODE_MEO00 >= QUICK_MEO00  // PTS 1120151 FF to get a clean returncode
                OId.gg92GetPno()       >= 0                                 &&
                OId.gg92GetPno()       <= Converter_ICommon::Instance().MaxStaticPageNo() &&
#               endif
                OId.gg92GetPos()       >= NODE_HEADER_MXBD00                &&
                OId.gg92GetPos()       <  BODY_END_BD00                     &&
                OId.gg92GetFrameVers() >= MIN_OBJ_FRAME_VERS_GG92           &&
                OId.gg92GetFrameVers() <= MAX_OBJ_FRAME_VERS_GG92); 
}
 
/*---------------------------------------------------------------------------*/

static _INLINE void
bd04_Reschedule (tgg00_TransContext &Trans,
                 tsp00_Int4         &StartTime,
                 tsp00_Int4         &WaitingTime,
                 bool               &bTryAgain)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd04_Reschedule", ObjContainer_Trace, 5);
    
    vreschedule_msec (Trans.trTaskId_gg00, StartTime, WaitingTime);
    bTryAgain = (WaitingTime > 0);
    if (bTryAgain) Trans.trError_gg00 = e_ok;
    
    /* do not try again if the session has been canceled */
    if (Trans.trRteCommPtr_gg00->to_cancel)
    {
        bTryAgain           = false;
        Trans.trError_gg00  = e_cancelled;
    }
}

/*---------------------------------------------------------------------------*/

static _INLINE void
bd04_ReuseObj (tgg00_TransContext   &Trans,
               tgg00_FileId         &ObjFileId,
               tgg92_KernelOid      &OId,
               tgg91_PageRef        &ResultObjVers,
               tsp00_Int4            KeySize,
               void                 *pKey,
               bool                 &bTryAgain,
               Msg_List             &ErrorList)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd04_ReuseObj", ObjContainer_Trace, 5);
    
    BD04_CHECK_OID (Trans, ObjFileId, b04newhash, OId);
    
#   if COMPILEMODE_MEO00 >= SLOW_MEO00
    ta01Oid (bi, "OId         ", OId);
#   endif
    
    cbd04_ObjCurrent Current (Trans, ObjFileId, m_new);
        
    if (Trans.trError_gg00 == e_ok)
        bd90ReuseObj (Current, OId, ResultObjVers, KeySize, pKey, bTryAgain, ErrorList);
}

/*---------------------------------------------------------------------------*/

static _INLINE void
bd04_VtraceErrClass (tgg00_TransContext     &Trans,
                    const tgg00_FileId      &ObjFileId,
                    tgg00_VtraceType_Enum    TraceObject,
                    tsp00_Bool               bWriteVtrace)
{
    /*  This function writes the class and the error of an */
    /*  BD-function at the KB-BD interface into the trace  */
    
    if  (
        (bWriteVtrace                            ) ||
        (
        (Trans.trError_gg00 != e_ok            ) && 
        (Trans.trError_gg00 != e_no_next_object)
        )
        )
    {
        tgg12_OmsErrorTrace TrLineOut;
        
        TrLineOut.oerrTrType_gg12.becomes  ( otError_egg12);
        TrLineOut.oerrTrType2_gg12.becomes ( ot2Nil_egg12);
        TrLineOut.oerrRoot_gg12            = ObjFileId.fileRoot_gg00();
        TrLineOut.oerr.ObjFileNo_C8_gg12() = ObjFileId.fileDirFileId_gg00();   
        TrLineOut.oerrError_gg12           = Trans.trError_gg00;

        Trace_CommonEntry entry( tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(TraceObject), 
            REINTERPRET_CAST( tsp00_BytePtr, &TrLineOut ), sizeof (TrLineOut) );
    }
}

/*---------------------------------------------------------------------------*/

static void
bd04_VtraceReturn (tgg00_TransContext       &Trans, 
                   const tgg00_FileId       &ObjFileId,
                   tgg92_KernelOid           Oid,
                   tgg91_PageRef             PageRef,
                   tgg00_VtraceType_Enum     TraceObject,
                   tgg12_OmsTraceType2_Enum  TraceType2)
{
    /*  This function writes the Oid or an error of a      */
    /*  BD-function at the KB-BD interface into the trace  */
    
    if ((e_hash_key_not_found != Trans.trError_gg00) &&
        (e_no_next_object     != Trans.trError_gg00))
    {
        tgg12_OmsOidTrace TrLineOIdSeq; 
        
        TrLineOIdSeq.ooidTrType_gg12.becomes(otOid_egg12);
        TrLineOIdSeq.ooidTrType2_gg12.becomes(TraceType2);
        TrLineOIdSeq.ooidObjId_gg12   = Oid;
        TrLineOIdSeq.ooidObjVers_gg12 = PageRef;
        
        Trace_CommonEntry entry( tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(TraceObject), 
            REINTERPRET_CAST( tsp00_BytePtr, &TrLineOIdSeq ), sizeof (TrLineOIdSeq) );
    }
    
    if (e_ok != Trans.trError_gg00)
        bd04_VtraceErrClass (Trans, ObjFileId, TraceObject, WRITE_VTRACE);
}

/*---------------------------------------------------------------------------*/
/* PTS 1111948 AK 26/09/2001 */
inline static SAPDB_Bool
bd04_ObjFileIdIsValid (const tgg00_FileId &ObjFileId)
{
    /* check if input file id fullfils basic requirements */ 
    SAPDB_Bool valid =
        ((ObjFileId.fileContObjFileNo_gg00() == PRIM_CONT_OBJ_FILE_NO_BD900)    &&
        (tfnObj_egg00 == ObjFileId.fileTfn_gg00())                              && 
        (!ObjFileId.fileType_gg00().includes (ftsTemp_egg00))                   &&
        ObjFileId.fileType_gg00().includes (ftsObject_egg00)                    &&      
        ObjFileId.fileType_gg00().includes (ftsConcurrent_egg00)                && 
        ObjFileId.fileType_gg00().includes (ftsPerm_egg00)                      &&
        (ObjFileId.fileObjFileType_gg00() != oftUnknownType_egg00)              &&
        (ObjFileId.fileRoot_gg00() != NIL_PAGE_NO_GG00));

    if ((!valid) && (g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdObject_gg00))
    {
        Kernel_VTrace trace;
        trace << __FILE__ << ":" << __LINE__ 
            << " bd04_ObjFileIdIsValid for file:" << ObjFileId.fileObjFileNo_gg00() << " failed";
        if (!(ObjFileId.fileContObjFileNo_gg00() == PRIM_CONT_OBJ_FILE_NO_BD900))
            trace << NewLine << "->" << " FileId.fileContObjFileNo_gg00:" 
            << ObjFileId.fileContObjFileNo_gg00() << " Not PRIM_CONT_OBJ_FILE_NO_BD900";
        if (!( ObjFileId.fileType_gg00().includes (ftsObject_egg00) ))
            trace << NewLine << "->" << " FileId.fileType_gg00 doesn't include ftsObject_egg00";
        if (!( ObjFileId.fileType_gg00().includes (ftsConcurrent_egg00) ))
            trace << NewLine << "->" << " FileId.fileType_gg00 doesn't include ftsConcurrent_egg00";
        if (!( ObjFileId.fileType_gg00().includes (ftsPerm_egg00) ))
            trace << NewLine << "->" << " FileId.fileType_gg00 doesn't include ftsPerm_egg00";
        if ( ObjFileId.fileType_gg00().includes (ftsTemp_egg00) )
            trace << NewLine << "->" << " FileId.fileType_gg00 includes ftsTemp_egg00";
        if (!( tfnObj_egg00 == ObjFileId.fileTfn_gg00() ))
            trace << NewLine << "->" << " FileId.fileTfn_gg00:" << ObjFileId.fileTfn_gg00() 
            << " Not tfnObj_egg00";
        if (ObjFileId.fileObjFileType_gg00() == oftUnknownType_egg00)
            trace << NewLine << "->" << " FileId.fileObjFileType_gg00 == oftUnknownType_egg00";
        if (ObjFileId.fileRoot_gg00() == NIL_PAGE_NO_GG00)
            trace << NewLine << "->" << " FileId.fileRoot_gg00 == NIL_PAGE_NO_GG00";
    };

    return valid;
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/ 

