/****************************************************************************

  module      : vbd91.cpp

  -------------------------------------------------------------------------

  responsible : FerdiF

  special area: interface for the pascal routines to the OMSGarbageCollection
  description : description ...


  version     : 7.5.0.0
  last changed: 2003-01-22  11:32
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
#ifndef HBD91_H
#   include "hbd91.h"
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
#ifndef GBD900_H
#   include "gbd900.h"
#endif
#ifndef GBD910_H
#   include "gbd910.h"
#endif
#ifndef GBD930_H
#   include "gbd930.h"
#endif
#ifndef HBD01_H
#   include "hbd01.h"
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
#ifndef HBD911_H
#   include "hbd911.h"
#endif
#ifndef HBD92_H
#   include "hbd92.h"
#endif
#ifndef HBD93_H
#   include "hbd93.h"
#endif
#ifndef HBD93_H
#   include "hbd93.h"
#endif
#ifndef HBD902_H
#   include "hbd902.h"
#endif
#ifndef HGG01_1_H
#   include "hgg01_1.h"
#endif
#ifndef HGG01_3_H
#   include "hgg01_3.h"
#endif
#ifndef HGG011_H
#   include "hgg11.h"  // g11ParseVersion
#endif
#ifndef HEO55K_H
#   include "heo55k.h" // vbegexcl/vendexcl
#endif
#ifndef RTEMEM_ALLOCATOR_HPP
#   include "RunTime/MemoryManagement/RTEMem_Allocator.hpp"
#endif
#ifndef SAPDBMEM_NEWDESTROY_HPP
#   include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#endif
#ifndef MSG_LIST_HPP
#   include "Messages/Msg_List.hpp"
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
#ifndef RTECONF_PARAMETERACCESSKERNELINTERFACE_HPP
#   include "RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.hpp"
#endif
#ifndef KERNEL_VTRACE_HPP
#   include "KernelCommon/Kernel_VTrace.hpp"
#endif

#if COMPILEMODE_MEO00 >= SLOW_MEO00
#include "hta99.h"
#include "hta01.h"
#endif

#ifndef GSP100_H
#   include "gsp100.h"
#endif

#ifndef HKB53_H
#   include "hkb53.h"
#endif
#ifndef FILEDIR_IDIRECTORY_HPP
#   include "FileDirectory/FileDir_IDirectory.hpp"
#endif
#ifndef FILEDIR_OMS_HPP
#   include "FileDirectory/FileDir_Oms.hpp"
#endif
#ifndef FILEDIR_OMSVAR_HPP
#   include "FileDirectory/FileDir_OmsVar.hpp"
#endif
#ifndef FILEDIR_OMSFIXED_HPP
#   include "FileDirectory/FileDir_OmsFixed.hpp"
#endif
#ifndef FILEDIR_OMSKEY_HPP
#   include "FileDirectory/FileDir_OmsKey.hpp"
#endif
#ifndef FILEDIR_OMSKEYPARTITION_HPP
#   include "FileDirectory/FileDir_OmsKeyPartition.hpp"
#endif
#ifndef FILEDIR_ITERATOR_HPP
#   include "FileDirectory/FileDir_Iterator.hpp"
#endif
#ifndef RTETASK_ITASK_HPP
#   include "RunTime/Tasking/RTETask_ITask.hpp"
#endif
#ifndef RTETASK_SUSPENDREASON_HPP
#   include "RunTime/Tasking/RTETask_SuspendReason.h"
#endif

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define RESTART true
#define WITHIN_GC_REGION true       /*PTS 1115596*/

#ifndef TRUE
 #define TRUE 1
#endif

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

#if COMPILEMODE_MSP00 >= QUICK_MEO00
# define    ABORT_OR_MESSAGE(MESSAGE, PARAMETER) \
        g01opmsg (sp3p_knldiag, sp3m_error, csp3_bd_msg, csp3_n_obj, MESSAGE, PARAMETER)
#else
# define    ABORT_OR_MESSAGE(MESSAGE, PARAMETER) \
        g01abort (csp3_bd_msg, csp3_n_obj, MESSAGE, PARAMETER)
#endif

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

/*===========================================================================*
 *  LOCAL FUNCTION DECLARATION                                               *
 *===========================================================================*/

/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/

/*===========================================================================*
 *  LOCAL VARIABLES                                                          *
 *===========================================================================*/

static const bool bExclusive = true;

cbd930_GarbCollController bd91GarbCollController;

GC_IGCController&
GC_IGCController::Instance()
{
    return * STATIC_CAST(GC_IGCController*,&bd91GarbCollController);
};

/*===========================================================================*
 *  LOCAL FUNCTIONS (CODE)                                                   *
 *===========================================================================*/

static const tsp00_Char hexDigits[22] =
{'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
 'a', 'b', 'c', 'd', 'e', 'f','A', 'B', 'C', 'D', 'E', 'F'};

static tgg00_BasisError 
bd91_convertFileno(tsp00_C16& ObjFileNoStr, FileDir_FileNo& Fileno)
{
    tsp00_C8  ObjFileNo;
    tgg00_BasisError berr = e_ok;
    int ix;
    unsigned char iy;
    int iz = 15;
    for ( ix=0; ix < sizeof(ObjFileNo); ix++ )
        ObjFileNo[ix] = 0;
    for ( ix=15; (ix >= 0) && (ObjFileNoStr[ix] == ' '); ix-- )
        ;
    if ( 0 > ix )
    {
       berr = e_const_incompatible_with_typ;
    }
    else
    {
        for ( ; ix >= 0; ix-- )
        {
            for (iy = 0; iy <= sizeof(hexDigits); iy++)
            {
                if (ObjFileNoStr[ix] == hexDigits[iy])
                    break;
            }
            if (iy > sizeof(hexDigits))
            {
                berr = e_const_incompatible_with_typ;
                break;
            }
            if (iy > 15) 
                iy = iy - 6;
            if ( 0 == (iz % 2) )
                iy = iy << 4;
            ObjFileNo[iz/2] = ObjFileNo[iz/2] | iy;
            iz--;
        }
        Fileno = ObjFileNo;
    }
    return berr;
};

/*===========================================================================*
 *  GLOBAL FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

externC void
bd91FreeCaches(tgg00_TransContext &Trans)
{
    if ( Trans.trFreePagesForIns_gg00 )
        bd902FreeFreePageListCache(Trans);

    if ( Trans.trSchemaShrLockHash_gg00 )
        bd902FreeLockShareSchemaCache(Trans);
};

/*---------------------------------------------------------------------------*/

externC void
bd91TransEnd(tgg00_TransContext &Trans,
             pasbool             commit)
{
    bd90TransEnd(Trans);
};

/*---------------------------------------------------------------------------*/

externPascal void 
bd91InitRWRecreateIndex(void)
{
    ROUTINE_DBG_MEO00 ("bd91InitRWRecreateIndex");

    bd911InitRWRecreateIndex();
}

/*---------------------------------------------------------------------------*/


    externC void
bd91CheckIndex (tgg00_TransContext   &Trans,
                tsp00_C16            &ObjFileNoStr )
{
    ROUTINE_DBG_MEO00 ("bd91CheckIndex");
    
    FileDir_FileNo     Fileno;
    Trans.trError_gg00 = bd91_convertFileno(ObjFileNoStr, Fileno);
    bd911CheckIndex(Trans, Fileno);
};

/*---------------------------------------------------------------------------*/

externC void
bd91RecreateIndex (tgg00_TransContext   &Trans,
                   tsp00_C16            &ObjFileNoStr )
{
    ROUTINE_DBG_MEO00 ("bd91RecreateIndex");
    
    FileDir_FileNo     Fileno;
    Trans.trError_gg00 = bd91_convertFileno(ObjFileNoStr, Fileno);
    bd911RecreateIndex(Trans, Fileno);
};

/*---------------------------------------------------------------------------*/

externCpp void
bd91AddFileToObjFileDir (tgg00_TransContext     &Trans,
                         tgg00_FileId           &FileId,
                         cbd910_ObjDataNode     &NptrsRoot,
                         Converter_Version      &converterVersion,
                         SAPDB_UInt1             numKeyFiles,
                         LVC_RootArray          &RootArray,
                         Msg_List               &ErrorList)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd91AddFileToObjFileDir", ObjContainer_Trace, 5);

    tbd_nodeptr            &pRoot            = NptrsRoot.np_ptr();
    const FileDir_FileNo   ObjFileNo = FileId.fileDirFileId_gg00();
    const bool              bFileIsForVarObj = (pt2VarObject_egg00 == pRoot->nd_pt2());
    const int               ObjFrameLen      = pRoot->nd_obj_frame_len();
    int                     MaxObjBodySize;
    int                     MaxObjPerPageCnt;
    tgg00_BasisError       &TrError          = Trans.trError_gg00;

    /* determine maximum number of objects which can be stored on this page */
    const int AlignedObjFrameLen = BD92ALIGN_OBJ_FRAME(ObjFrameLen);
    if  (FULLCOVERING_BD00 >= ObjFrameLen)
        MaxObjPerPageCnt = 1 + ((FULLCOVERING_BD00 - ObjFrameLen)/AlignedObjFrameLen);
    else
    {
        Kernel_OpError( csp3_bd_msg, csp3_n_obj )
            << __FILE__ << ":" << __LINE__ << "object size:" << ObjFrameLen
            << " > max object size:" << FULLCOVERING_BD00
            << " fileid:"
            << ObjFileNo
            << " error:" << e_too_long_objects;
        Trans.trError_gg00 = e_too_long_objects;
        return;
    }

    /* register object file at the global structure bd90ObjFileDir */
    if (bFileIsForVarObj)
        MaxObjBodySize = ObjFrameLen - sizeof (tgg92_ObjHeader) - sizeof (tbd900_VarObjBodyHeader);
    else
        MaxObjBodySize = ObjFrameLen - sizeof (tgg92_ObjHeader);

    FileDir_IDirectory &fd = FileDir_IDirectory::SessionInstance(Trans);
    FileDir_ReturnCode fd_rc;

    if (bFileIsForVarObj) {                               /* Type: omsVarObject */
        FileDir_OmsVar  fd_var_smart;
        tsp00_Int4      NextContObjBodyLen = MaxObjBodySize;

        if ( ObjContainer_Trace.TracesLevel(2) )
        {
            Kernel_VTrace()
                << __FILE__ << ":" << __LINE__ << " AddOmsVarFile"
                << NewLine << "->" << " TaskId:" << Trans.trTaskId_gg00
                << NewLine << "->" << " FileId:" << ObjFileNo
                << NewLine << "->" << " ConverterVersion:" << converterVersion
                << NewLine << "->" << " Root:" << NptrsRoot.np_ptr()->nd_id()
                << NewLine << "->" << " MaxObjBodySize:" << MaxObjBodySize
                << NewLine << "->" << " MaxObjObjPerPage:" << MaxObjPerPageCnt;
        }

        fd_rc = fd.AddOmsVarFile( Trans.trTaskId_gg00, ObjFileNo, converterVersion,
            NptrsRoot.np_ptr()->nd_id(), MaxObjBodySize, MaxObjPerPageCnt, 0,
            ErrorList, &fd_var_smart);

        if ( FileDir_Okay == fd_rc )
        {
            fd_var_smart.SetLeafPageCount(1, true);

            tsp00_Int4  iContFile = 0;
            for ( iContFile = 1; iContFile <= MAX_NUM_CONT_OBJ_FILES_BD900; iContFile++ )
            {
                int ObjFrameLen              = bd93GetContFileBodySizes(iContFile) + sizeof (tbd900_ContObjHeader);
                int MaxObjPerContPageCnt     = 1 + (BODY_END_BD00 - NODE_HEADER_MXBD00 - ObjFrameLen) /
                    BD92ALIGN_OBJ_FRAME(ObjFrameLen);

                if ((g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdObject_gg00))
                {
                    if ( ObjContainer_Trace.TracesLevel(2) )
                        Kernel_VTrace() << __FILE__ << ":" << __LINE__
                        << NewLine << "->" << " VarFileId:" << ObjFileNo
                        << NewLine << "->" << " Root:" << RootArray[iContFile - 1]
                        << NewLine << "->" << " ContObjFileIdx:" << iContFile;
                }

                fd_var_smart.AddContPartition(RootArray[iContFile - 1], iContFile );
                fd_var_smart.ModifyLeafPageCount(Trans.trTaskId_gg00, converterVersion, 1);
            }
        }
    }
    else if (0 < numKeyFiles) {    /* Type: omsKeyObject */
        if ( ObjContainer_Trace.TracesLevel(2) )
        {
            Kernel_VTrace() << __FILE__ << ":" << __LINE__
                << " AddOmsKeyFile"
                << NewLine << "->" << " TaskId:" << Trans.trTaskId_gg00
                << NewLine << "->" << " FileId:"
                << ObjFileNo
                << NewLine << "->" << " ConverterVersion:" << converterVersion
                << NewLine << "->" << " Root:" << NptrsRoot.np_ptr()->nd_id()
                << NewLine << "->" << " MaxObjBodySize:" << MaxObjBodySize
                << NewLine << "->" << " MaxObjObjPerPage:" << MaxObjPerPageCnt
                << NewLine << "->" << " NumKeyIndexTrees:" << numKeyFiles;
        }

        FileDir_OmsKey  fd_key_smart;
        fd_rc = fd.AddOmsKeyFile( Trans.trTaskId_gg00, ObjFileNo, converterVersion,
            NptrsRoot.np_ptr()->nd_id(), MaxObjBodySize, MaxObjPerPageCnt, 0,
            numKeyFiles, ErrorList, &fd_key_smart );
        if ( FileDir_Okay == fd_rc )
            fd_key_smart.SetLeafPageCount(1, true);
    }
    else {                                                /* Type: omsObject */
        if ( ObjContainer_Trace.TracesLevel(2) )
        {
            Kernel_VTrace() << __FILE__ << ":" << __LINE__
                << " AddOmsFixedFile"
                << NewLine << "->" << " TaskId:" << Trans.trTaskId_gg00
                << NewLine << "->" << " FileId:"
                << ObjFileNo
                << NewLine << "->" << " ConverterVersion:" << converterVersion
                << NewLine << "->" << " Root:" << NptrsRoot.np_ptr()->nd_id()
                << NewLine << "->" << " MaxObjBodySize:" << MaxObjBodySize
                << NewLine << "->" << " MaxObjObjPerPage:" << MaxObjPerPageCnt;
        }

        FileDir_OmsFixed  fd_smart;
        fd_rc = fd.AddOmsFixedFile( Trans.trTaskId_gg00, ObjFileNo, converterVersion,
            NptrsRoot.np_ptr()->nd_id(), MaxObjBodySize, MaxObjPerPageCnt, 0, ErrorList, &fd_smart );
        if ( FileDir_Okay == fd_rc )
            fd_smart.SetLeafPageCount(1, true);
    }
    if ( FileDir_Okay != fd_rc )
    {
        switch( fd_rc )
        {
        case FileDir_FileNoExists:
            TrError = e_duplicate_filename;
            break;
        case FileDir_OutOfMemory:
            TrError = e_no_more_memory;
            break;
        default:
            TrError = e_hash_memory_exceeded;   /* *** TO DO *** */
        }
        if ( TrError != e_duplicate_filename)
        {
            Kernel_OpWarning( csp3_bd_msg, csp3_n_obj )
                << __FILE__ << ":" << __LINE__
                << " FileDir_IDirectory::AddOms[Key|Var|Fixed]File error:" << fd_rc
                << " FileId:"
                << ObjFileNo
                << " basis_error:" << TrError;
        }
        if ((g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdObject_gg00))
        {
            Kernel_VTrace() << __FILE__ << ":" << __LINE__
                <<  " AddOms[Key|Var|Fixed]File"
                << NewLine << "->" << "FileId:"
                << ObjFileNo
                << NewLine << "->" << "FileDir_ReturnCode:" << fd_rc;
        }
    }
    else
    {
        if ( Kernel_IAdminInfo::Instance().KernelStateIsRedoLogProcessing() )
        {
            fd.Restart_SetFileNoHighWaterMark(ObjFileNo);
        }
    }
};

/*---------------------------------------------------------------------------*/
externC void
bd91DumpGarbColl (tgg00_VfFileref     &HostFile,
                  tsp00_Int4            BufferSize,
                  tsp00_Page           &Buffer,
                  tsp00_Int4           &OutPno,
                  tsp00_Int4           &OutPos,
                  tsp00_VfReturn       &HostError,
                  tsp00_ErrText        &ErrText)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd91DumpGarbColl", ObjContainer_Trace, 5);

    bd91GarbCollController.bd930DumpGCController (HostFile, BufferSize,  Buffer, OutPno,  OutPos, HostError, ErrText);
}

/*---------------------------------------------------------------------------*/

externC tsp00_Int4 b91MaxRetentionTime() {
    return g01max_retention_time();
};

/*---------------------------------------------------------------------------*/

externC tsp00_Int4 b91MinRetentionTime() {
    return g01min_retention_time();
};

/*---------------------------------------------------------------------------*/
// PTS 1113452 FF 16-Jan-2002

/*----------------- PTS 1115641 FF 25-Apr-2002 ------------------------------*/

externC void
bd91InfoContainerRoots (tgg00_TransContext       &Trans,
                        FileDir_Iterator          *&pObjFileIter,
                        FileDir_Iterator          *&pContObjFileIter,
                        tsp00_Addr                pStackarea,
                        tsp00_Int2                pStackareaLen,
                        tsp00_C8                 &ClassId,
                        tsp00_Int4                &Root,
                        tsp00_Int4                &ContainerSequelNo,
                        tsp00_Int2                &NumKeyPartitions,
                        tsp00_Bool                &VarContainer,
                        tsp00_Bool                &DeleteFlag,
                        tsp00_Int4                &MaxObjBodySize,
                        tsp00_Int4                &MaxObjPerPage,
                        tsp00_Int4                &MaxChains,
                        tsp00_Int2                &UnusedBytesPerPage) // PTS 1118335 FF 11-OCT-2002
{
    SAPDBTRACE_ROUTINE_DEBUG("bd91InfoContainerRoots", ObjContainer_Trace, 5);

    if ( pStackareaLen > sizeof(void*) ) {
        if ( NULL == pObjFileIter ) {
            FileDir_IDirectory &IFDC = FileDir_IDirectory::SessionInstance(Trans);
            SAPDBMem_IRawAllocator &alloc = *reinterpret_cast<SAPDBMem_IRawAllocator*>(
            Trans.trAllocator_gg00);
            pObjFileIter = new(alloc) FileDir_Iterator(alloc);
            if ( !pObjFileIter )
                return;
            pObjFileIter->AddFileTypeToFilter(Data_OmsFixedFile);
            pObjFileIter->AddFileTypeToFilter(Data_OmsVarFile);
            pObjFileIter->AddFileTypeToFilter(Data_OmsKeyFile);
            pObjFileIter->AddFileTypeToFilter(Data_OmsContFile);
            IFDC.GetSharedFileDirIterator(*pObjFileIter);
        }
        FileDir_Oms   fd_smart;
        if (pObjFileIter->GetNext(fd_smart)) {
            SAPDB_MemCopyNoCheck (&ClassId[0], 
                fd_smart.GetFileNo().GetCharArray(), sizeof(ClassId));
            Root              = fd_smart.GetRootPage();
            ContainerSequelNo = 0;
            NumKeyPartitions  = 0;
            if (Data_OmsKeyFile == fd_smart.GetFileType()) {
                NumKeyPartitions  = fd_smart.GetKeyPartitionCount();
            }
            VarContainer      = (Data_OmsVarFile == fd_smart.GetFileType());
            DeleteFlag        = (Data_FileDeleted == fd_smart.GetFileState());
            MaxObjBodySize    = fd_smart.GetObjBodySize();
            MaxObjPerPage     = fd_smart.GetObjPerPageCount();
            MaxChains         = 1;
            if ( VarContainer )
                UnusedBytesPerPage = -1;
            else
                UnusedBytesPerPage = FULLCOVERING_BD00 % BD92ALIGN_OBJ_FRAME(MaxObjBodySize+ sizeof (tgg92_ObjHeader));
        } else {
            destroy(pObjFileIter,*reinterpret_cast<SAPDBMem_IRawAllocator*>(
                Trans.trAllocator_gg00));
            pObjFileIter = NULL; /* eof */
        }
    } else {
        if (NULL != pObjFileIter) {
            destroy(pObjFileIter,*reinterpret_cast<SAPDBMem_IRawAllocator*>(
                Trans.trAllocator_gg00));
        }
        pObjFileIter = NULL; /* eof */
    }
}


externC void
bd91FDirNextObjFileId (tgg00_TransContext       &Trans,
                       FileDir_Iterator         *&pObjFileIter,
                       tgg00_FileId             &ObjFileId)
{
    tgg00_BasisError    &TrError = Trans.trError_gg00;

    if ( NULL == pObjFileIter )
    {
      SAPDBMem_IRawAllocator &alloc = RTEMem_Allocator::Instance();
      pObjFileIter = new(alloc) FileDir_Iterator(alloc);
      if ( !pObjFileIter ) 
          return;
      pObjFileIter->AddFileTypeToFilter(Data_OmsFixedFile);
      pObjFileIter->AddFileTypeToFilter(Data_OmsVarFile);
      pObjFileIter->AddFileTypeToFilter(Data_OmsKeyFile);

      FileDir_IDirectory::Instance(cgg_nil_transindex).GetSharedFileDirIterator(*pObjFileIter);
    }

    FileDir_Oms   fd_smart;
    if (pObjFileIter->GetNext(fd_smart))
    {
        ObjFileId = b01niltree_id;
        ObjFileId.fileRoot_gg00()          = fd_smart.GetRootPage();
        FileDir_FileNo contHndl            = fd_smart.GetFileNo();
        ObjFileId.fileDirFileId_gg00()     = *reinterpret_cast<const tsp00_C8*>(contHndl.GetCharArray());
        ObjFileId.fileContObjFileNo_gg00() = PRIM_CONT_OBJ_FILE_NO_BD900;
        ObjFileId.fileTfn_gg00().becomes(tfnObj_egg00);
        ObjFileId.fileType_gg00().clear();
        ObjFileId.fileType_gg00().addElement (ftsObject_egg00);
        ObjFileId.fileType_gg00().addElement (ftsConcurrent_egg00);
        ObjFileId.fileType_gg00().addElement (ftsPerm_egg00);
        ObjFileId.fileNumObjKeyFiles_gg00() = 0;
        if ( Data_OmsKeyFile == fd_smart.GetFileType())
        {
            ObjFileId.fileNumObjKeyFiles_gg00() = STATIC_CAST(tsp00_Uint1,
                fd_smart.GetKeyPartitionCount());
        }

        if ( Data_OmsVarFile == fd_smart.GetFileType())
            ObjFileId.fileObjFileType_gg00().becomes (oftVarLenObjFile_egg00);
        else
            ObjFileId.fileObjFileType_gg00().becomes (oftFixLenObjFile_egg00);
    }
    else
    {
        destroy(pObjFileIter,RTEMem_Allocator::Instance());
        pObjFileIter = NULL;
        TrError = e_file_not_found;
    }
}

/*----------------- PTS 1115641 FF 25-Apr-2002 ------------------------------*/

externC void
bd91InfoContainerChains (tgg00_TransContext       &Trans,
                         FileDir_Iterator          *&pObjFileIter,
                         FileDir_Iterator          *&pContObjFileIter,
                         tsp00_Addr                pStackarea,
                         tsp00_Int2                pStackareaLen,
                         tsp00_C8                 &ClassId,
                         tsp00_Int4               &ContainerSequelNo,
                         tsp00_Int4               &ChainId,
                         tsp00_Int4               &Subroot,
                         tsp00_Int4               &ObjCount,
                         tsp00_Int4               &PageCount,
                         tsp00_Int4               &ActiveGC,
                         tsp00_Bool               &RelEmptyPages)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd91InfoContainerChains", ObjContainer_Trace, 5);

    if ( pStackareaLen > sizeof(void*) )
    {
        if ( NULL == pObjFileIter )
        {
            FileDir_IDirectory &IFDC = FileDir_IDirectory::SessionInstance(Trans);
            SAPDBMem_IRawAllocator &alloc = *reinterpret_cast<SAPDBMem_IRawAllocator*>(
            Trans.trAllocator_gg00);
            pObjFileIter = new(alloc) FileDir_Iterator(alloc);
            if ( !pObjFileIter ) 
                return;
            pObjFileIter->AddFileTypeToFilter(Data_OmsFixedFile);
            pObjFileIter->AddFileTypeToFilter(Data_OmsVarFile);
            pObjFileIter->AddFileTypeToFilter(Data_OmsKeyFile);
            pObjFileIter->AddFileTypeToFilter(Data_OmsContFile);
            IFDC.GetSharedFileDirIterator(*pObjFileIter);
        }
        FileDir_Oms   fd_smart;
        if (pObjFileIter->GetNext(fd_smart))
        {
            SAPDB_MemCopyNoCheck (&ClassId[0], 
                fd_smart.GetFileNo().GetCharArray(), sizeof(ClassId));
            ContainerSequelNo  = 0;
            Subroot            = fd_smart.GetRootPage();
            ObjCount           = (tsp00_Int4)fd_smart.GetEntryCount(Trans.trTaskId_gg00);
            PageCount          = (tsp00_Int4)fd_smart.GetLeafPageCount(Trans.trTaskId_gg00);
            ActiveGC           = 0;
            RelEmptyPages      = false;
        } else {
            destroy(pObjFileIter,*reinterpret_cast<SAPDBMem_IRawAllocator*>(
                Trans.trAllocator_gg00));
            pObjFileIter = NULL; /* eof */
        }
    } else {
        if (NULL != pObjFileIter) {
            destroy(pObjFileIter,*reinterpret_cast<SAPDBMem_IRawAllocator*>(
                Trans.trAllocator_gg00));
        }
        pObjFileIter = NULL; /* eof */
    }
}

/*----------------- PTS 1115641 FF 25-Apr-2002 ------------------------------*/

externC void
bd91InfoContainerKeys (tgg00_TransContext       &Trans,
                       FileDir_Iterator        *&pObjFileIter,
                       tsp00_Addr                pStackarea,
                       tsp00_Int2                pStackareaLen,
                       tsp00_Int2               &KeyIdx,
                       tsp00_C8                 &ClassId,
                       tsp00_Int4               &KeyRoot)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd91InfoContainerKeys", ObjContainer_Trace, 5);

    Msg_List errorList;
    if ( pStackareaLen > sizeof(void*) )
    {
        FileDir_FileNo contHndl(ClassId);
        FileDir_ReturnCode  fd_rc;
        FileDir_IDirectory &IFDC   = FileDir_IDirectory::SessionInstance(Trans);

        if ( NULL == pObjFileIter )
        {
            SAPDBMem_IRawAllocator &alloc = *reinterpret_cast<SAPDBMem_IRawAllocator*>(
            Trans.trAllocator_gg00);
            pObjFileIter = new(alloc) FileDir_Iterator(alloc);
            if ( !pObjFileIter )
                return;
            pObjFileIter->AddFileTypeToFilter(Data_OmsKeyFile);
            IFDC.GetSharedFileDirIterator(*pObjFileIter);
            KeyIdx  = 0;
            contHndl.Invalidate();
        }

        ++KeyIdx;
        FileDir_OmsKey         fd_key_smart;
        if ( contHndl.IsValid() )
        {
            const SAPDB_Bool       bConsistent = true;
            fd_rc = IFDC.GcGetFile(Trans.trTaskId_gg00, contHndl, fd_key_smart, errorList);
            if (FileDir_Okay != fd_rc) {
                destroy(pObjFileIter,*reinterpret_cast<SAPDBMem_IRawAllocator*>(
                    Trans.trAllocator_gg00));
                pObjFileIter = NULL; /* eof */
                return;
            }
        }
        if (( !contHndl.IsValid() ) || (KeyIdx > fd_key_smart.GetKeyPartitionCount())) 
        {
            KeyIdx = 1;
            if (!pObjFileIter->GetNext(fd_key_smart)) {
                destroy(pObjFileIter,*reinterpret_cast<SAPDBMem_IRawAllocator*>(
                    Trans.trAllocator_gg00));
                pObjFileIter = NULL; /* eof */
                return;
            }
        }

        SAPDB_MemCopyNoCheck (&ClassId[0], 
            fd_key_smart.GetFileNo().GetCharArray(), sizeof(ClassId));
        FileDir_FileNo KeyPartfileNo = fd_key_smart.GetKeyPartition(KeyIdx);
        FileDir_OmsKeyPartition  fd_key_part_smart;
        fd_rc = IFDC.GcGetFile(Trans.trTaskId_gg00, KeyPartfileNo, fd_key_part_smart, errorList);
        if (FileDir_Okay != fd_rc) {
            destroy(pObjFileIter,*reinterpret_cast<SAPDBMem_IRawAllocator*>(
                Trans.trAllocator_gg00));
                pObjFileIter = NULL; /* eof */
            return;
        }
        KeyRoot = fd_key_part_smart.GetRootPage();
    } else {
        if (NULL != pObjFileIter) {
            destroy(pObjFileIter,*reinterpret_cast<SAPDBMem_IRawAllocator*>(
                Trans.trAllocator_gg00));
        }
        pObjFileIter = NULL; /* eof */
    }
}

/*---------------------------------------------------------------------------*/
externC void
bd91InfoGCReset( void )
{
    SAPDBTRACE_ROUTINE_DEBUG("bd91InfoGCReset", ObjContainer_Trace, 5);

    for ( int i = 0; i < bd91GarbCollController.bd930GetNumGCs(); ++i )
    {
        bd91GarbCollController.gccGCCounters_bd930[i].gccntActivateCount = 0;
        bd91GarbCollController.gccGCCounters_bd930[i].gccntObjectsDeleted = 0;
        bd91GarbCollController.gccGCCounters_bd930[i].gccntHistEntriesReleased = 0;
        bd91GarbCollController.gccGCCounters_bd930[i].gccntEmtyPagesReleased = 0;
        bd91GarbCollController.gccGCCounters_bd930[i].gccntDropPagesReleased = 0;
        bd91GarbCollController.gccGCCounters_bd930[i].gccntDropFiles = 0;
        bd91GarbCollController.gccGCCounters_bd930[i].gccntHistCreateObjFile = 0;
        bd91GarbCollController.gccGCCounters_bd930[i].gccntHistDeleteObj = 0;
        bd91GarbCollController.gccGCCounters_bd930[i].gccntHistDropObjFile = 0;
        bd91GarbCollController.gccGCCounters_bd930[i].gccntHistLockObj = 0;
        bd91GarbCollController.gccGCCounters_bd930[i].gccntHistInsertObj = 0;
        bd91GarbCollController.gccGCCounters_bd930[i].gccntHistNewObj = 0;
        bd91GarbCollController.gccGCCounters_bd930[i].gccntHistUpdateObj = 0;

    }
};

/*---------------------------------------------------------------------------*/

externC void
bd91InfoGCStats (tsp00_Int4            idxGC,
                 tsp00_TaskId         &taskId,
                 tsp00_Bool           &active,
                 tsp00_8ByteCounter   &activateCount,
                 tsp00_8ByteCounter   &deleteObj,
                 tsp00_8ByteCounter   &relHistEntryNormal,
                 tsp00_8ByteCounter   &relHistCreateObjFile,
                 tsp00_8ByteCounter   &relHistDeleteObj,
                 tsp00_8ByteCounter   &relHistDropObjFile,
                 tsp00_8ByteCounter   &relHistLockObj,
                 tsp00_8ByteCounter   &relHistInsertObj,
                 tsp00_8ByteCounter   &relHistNewObj,
                 tsp00_8ByteCounter   &relHistUpdateObj,
                 tsp00_8ByteCounter   &relPageEmpty,
                 tsp00_8ByteCounter   &relPageDropFile,
                 tsp00_8ByteCounter   &relDropFile )
{
    SAPDBTRACE_ROUTINE_DEBUG("bd91InfoGCStats", ObjContainer_Trace, 5);

    taskId                = bd91GarbCollController.gccGCState_bd930[idxGC-1].gcsTaskId_bd930;
    active                = bd91GarbCollController.gccGCState_bd930[idxGC-1].gcsActive_bd930;
    activateCount         = bd91GarbCollController.gccGCCounters_bd930[idxGC-1].gccntActivateCount;
    deleteObj             = bd91GarbCollController.gccGCCounters_bd930[idxGC-1].gccntObjectsDeleted;
    relHistEntryNormal    = bd91GarbCollController.gccGCCounters_bd930[idxGC-1].gccntHistEntriesReleased;
    relHistCreateObjFile  = bd91GarbCollController.gccGCCounters_bd930[idxGC-1].gccntHistCreateObjFile;
    relHistDeleteObj      = bd91GarbCollController.gccGCCounters_bd930[idxGC-1].gccntHistDeleteObj;
    relHistDropObjFile    = bd91GarbCollController.gccGCCounters_bd930[idxGC-1].gccntHistDropObjFile;
    relHistLockObj        = bd91GarbCollController.gccGCCounters_bd930[idxGC-1].gccntHistLockObj;
    relHistInsertObj      = bd91GarbCollController.gccGCCounters_bd930[idxGC-1].gccntHistInsertObj;
    relHistNewObj         = bd91GarbCollController.gccGCCounters_bd930[idxGC-1].gccntHistNewObj;
    relHistUpdateObj      = bd91GarbCollController.gccGCCounters_bd930[idxGC-1].gccntHistUpdateObj;
    relPageEmpty          = bd91GarbCollController.gccGCCounters_bd930[idxGC-1].gccntEmtyPagesReleased;
    relPageDropFile       = bd91GarbCollController.gccGCCounters_bd930[idxGC-1].gccntDropPagesReleased;
    relDropFile           = bd91GarbCollController.gccGCCounters_bd930[idxGC-1].gccntDropFiles;
}

/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
externCpp void
bd91CheckRetentionTime(tsp00_TaskId  TaskId)
{
    bd91GarbCollController.bd930CheckRetentionTime (TaskId);
}

/*---------------------------------------------------------------------------*/
externC void
bd91InitOMSGarbageCollection ()
{
    SAPDBTRACE_ROUTINE_DEBUG("bd91InitOMSGarbageCollection", ObjContainer_Trace, 5);
    bd91GarbCollController.bd930Init ();
}

/*---------------------------------------------------------------------------*/
externC void
bd91StartOMSGarbageCollection (tgg00_TransContext   &Trans)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd91StartOMSGarbageCollection",
        ObjContainer_Trace, 5);
    
    bd91GarbCollController.bd930StartAllGCs (Trans.trTaskId_gg00);
}

/*---------------------------------------------------------------------------*/
externC void
bd91EnableOMSHistoryRemoval (tsp00_TaskId  TaskId)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd91EnableOMSHistoryRemoval",
        ObjContainer_Trace, 5);
    bd91GarbCollController.bd930EnableOMSHistoryRemoval (TaskId);
}

/*---------------------------------------------------------------------------*/

externC void
bd91StartOMSGarbageCollectionAfterSpecialOp (tsp00_TaskId  TaskId)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd91StartOMSGarbageCollectionAfterSpecialOp",
        ObjContainer_Trace, 5);
    bd91GarbCollController.bd930StartAllGCsAfterSpecialOp (TaskId);
}

/*---------------------------------------------------------------------------*/

externC void
bd91StopOMSGarbageCollectionForSpecialOp (tsp00_TaskId  TaskId)

{
    SAPDBTRACE_ROUTINE_DEBUG("bd91StopOMSGarbageCollectionForSpecialOp",
        ObjContainer_Trace, 5);

    /* this function stops all garbage collectors, which remain suspended  */
    /* until the method bd930StartAllGCsAfterSpecialOp is called           */
    bd91GarbCollController.bd930StopAllGCsForSpecialOp (TaskId);
}

/*---------------------------------------------------------------------------*/
externC void
bd91StartOMSGarbageCollectionIfRequired (tsp00_TaskId  TaskId)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd91StartOMSGarbageCollectionIfRequired",
        ObjContainer_Trace, 5);
    bd91GarbCollController.bd930StartGCsIfRequired(TaskId);
}

/*---------------------------------------------------------------------------*/

void
bd91_ReleaseAllPagesOfOneFile (tgg00_TransContext     &Trans,
                               tgg00_FileId           &ObjFileId,
                               FileDir_Oms            &fd_smart,
                               tsp00_Int4              iContFileNo,
                               const int               gcoIndex_bd930)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd91_ReleaseAllPagesOfOneFile", ObjContainer_Trace, 5);

    /*----------------------------------------------------------------------------*/
    /* This function removes all pages of an chain following the right pointer    */
    /* The subroot page is removed as last page                                   */
    /* note that the next free chain is not updated when the the chain is deleted */
    /*----------------------------------------------------------------------------*/

    tgg00_BasisError       &TrError    = Trans.trError_gg00;
    cbd05_Current Current (Trans, m_update, ObjFileId);
    Current.curr_tree_id.fileBdUse_gg00().addElement(bd_release_acc);

    /* write vtrace */
    if (g01vtrace.vtrOmsFree_gg00 )
    {
        tgg12_OmsFilenameTrace TrLineIn;

        TrLineIn.ofnTrType_gg12.becomes(otFilename_egg12);
        TrLineIn.ofnTrType2_gg12.becomes(ot2Nil_egg12);
        TrLineIn.ofnFileName_gg12 = ObjFileId.fileName_gg00();

        Trace_CommonEntry entry( tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(b930DropFile),
            REINTERPRET_CAST( tsp00_BytePtr, &TrLineIn ), sizeof (TrLineIn) );
    }

    bool               waitForSavepoint  = false;
    bool               notfinished       = true;
    FileDir_OmsVar     fd_var_smart;
    Converter_Version  converterVersion;

    while(notfinished)
    {
        {
            cbd910_ObjDataNode NptrsPage    (Current, nr_for_update);
            Data_PageNo rootPage;
            if ( iContFileNo == PRIM_CONT_OBJ_FILE_NO_BD900 )
            {
                rootPage = fd_smart.GetRootPage();
            }
            else
            {
                fd_var_smart.Assign(fd_smart);  
                rootPage = fd_var_smart.GetContPartition(iContFileNo);
                if ( NIL_PAGE_NO_GG00 == rootPage ) 
                    return;
            }

            cbd910_SubRootNode NptrsSubRoot (Current, rootPage, nr_for_update);
            if ( e_ok != TrError ) {
                Kernel_OpWarning errmsg( csp3_bd_msg, csp3_n_obj );
                errmsg  << __FILE__ << ":" << __LINE__
                    << " NptrsSubRoot TrError:" << TrError
                    << " RootPage:" << fd_smart.GetRootPage()
                    << " FileId:" << ObjFileId.fileObjFileNo_gg00();
                if ( iContFileNo != PRIM_CONT_OBJ_FILE_NO_BD900 )
                {
                    errmsg  << " ContFileRoot:" << rootPage
                        << " ContFileNo:" << iContFileNo;
                }
                if ((e_no_converter_entry == TrError) || 
					(e_old_fileversion == TrError)) // PTS 1141373
                    TrError = e_ok;
                return;
            }

            converterVersion = 
                bd20GetDestinationConverterVersion ( Trans.trTaskId_gg00, NptrsSubRoot.np_cbptr() );

            tsp00_PageNo RightPageNo = NptrsSubRoot.np_ptr()->nd_right();
            while (RightPageNo != NIL_PAGE_NO_GG00)
            {
                if (bd91GarbCollController.GCMustStop()) {
                    waitForSavepoint = true;
                    break;
                }

                NptrsPage.bd910AssignToPage(RightPageNo);
                if (TrError != e_ok) {
                    Kernel_OpWarning( csp3_bd_msg, csp3_n_obj )
                        << __FILE__ << ":" << __LINE__
                        << " bd910AssignToPage(RightPageNo) TrError:" << TrError
                        << " RightPageNo:" << RightPageNo
                        << " FileId:" << ObjFileId.fileObjFileNo_gg00();
                    break;
                }

                /* update all page counters on sub root page and in filedirectory these */
                /* counters are used by check routines in the quick kernel only         */
                fd_smart.ModifyLeafPageCount(Trans.trTaskId_gg00, converterVersion, -1);
                if (0 < NptrsPage.np_ptr()->nd_free_obj_cnt())
                {
                    fd_smart.ModifyFreePageCount(Trans.trTaskId_gg00,converterVersion, -1);  
                    if ( ObjContainer_Trace.TracesLevel(4) )
                    {
                        Kernel_VTrace() << __FILE__ << ":" << __LINE__ 
                            << " bd91_ReleaseAllPagesOfOneFile:ModifyFreePageCount -1"
                            << " root:"
                            << NptrsPage.np_ptr()->nd_root();
                    }
                }

                RightPageNo = NptrsPage.np_ptr()->nd_right();
                NptrsPage.bd910FreePage();
                if (TrError != e_ok) {
                    Kernel_OpWarning( csp3_bd_msg, csp3_n_obj )
                        << __FILE__ << ":" << __LINE__
                        << " bd910FreePage TrError:" << TrError
                        << " FileId:" << ObjFileId.fileObjFileNo_gg00();
                    break;
                }

                ++bd91GarbCollController.gccGCCounters_bd930[gcoIndex_bd930].gccntDropPagesReleased;

                /* update the right pointers on subroot page, all other  */
                /* chains as the feot and the free chain are not updated */
                NptrsSubRoot.np_ptr()->nd_right() = RightPageNo;
            };

            if ((e_ok == TrError) && !waitForSavepoint){
                NptrsSubRoot.bd910FreePage();
                if ( iContFileNo != PRIM_CONT_OBJ_FILE_NO_BD900 )
                {
                    fd_var_smart.AddContPartition( NIL_PAGE_NO_GG00, iContFileNo );
                }
                notfinished = false;
            }
        }

        if (e_ok != TrError)
        {
            Kernel_OpWarning( csp3_bd_msg, csp3_n_obj )
                << __FILE__ << ":" << __LINE__
                << " basis error:" << TrError;
            notfinished = false;
        };

        if (notfinished && waitForSavepoint)
        {
            tsp00_TaskId   ResumeTaskId = cgg_nil_pid;
            vbegexcl (Trans.trTaskId_gg00, g08garbage);
            bd91GarbCollController.TerminateGarbagecollector(gcoIndex_bd930, ResumeTaskId);
            vendexcl (Trans.trTaskId_gg00, g08garbage);
            if (cgg_nil_pid != ResumeTaskId)
            {
                /*=======================================================================*/
                vresume (ResumeTaskId);
                /*=======================================================================*/

                ResumeTaskId = cgg_nil_pid;
            }

            waitForSavepoint = false;
            /*=====================================================*/
            vsuspend (Trans.trTaskId_gg00, TSK_VSUSPEND_REASON_NO_WORK);
            /*=====================================================*/
        };
    };
    /* write vtrace entry */
    if  (g01vtrace.vtrOmsFree_gg00 || (e_ok != TrError))
    {
        tgg12_OmsErrorTrace TrLineOut;

        TrLineOut.oerrTrType_gg12.becomes  (otError_egg12);
        TrLineOut.oerrTrType2_gg12.becomes (ot2Nil_egg12);
        TrLineOut.oerrRoot_gg12            = ObjFileId.fileRoot_gg00();
        TrLineOut.oerr.ObjFileNo_C8_gg12() = ObjFileId.fileDirFileId_gg00();
        TrLineOut.oerrError_gg12           = TrError;

        Trace_CommonEntry entry( tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(b930DropFile),
            REINTERPRET_CAST( tsp00_BytePtr, &TrLineOut ), sizeof (TrLineOut) );
    };
}

/*---------------------------------------------------------------------------*/

externCpp void
bd91DropObjFile (tgg00_TransContext   &Trans,
                 FileDir_FileNo        ObjFileNo)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd91DropObjFile", ObjContainer_Trace, 5);

    cbd05_Current          Current     (Trans, m_update);
    tgg00_BasisError      &TrError     = Trans.trError_gg00;
    const SAPDB_Bool       bConsistent = true;
    FileDir_IDirectory     &IFDC       = FileDir_IDirectory::SessionInstance(Trans);
    Msg_List                ErrorList;

    /* evaluate garbage collector index */
    int gcoIndex_bd930 = -1;
    for (int i=0; i < MAX_OBJ_GARB_COLL_BD00; i++) {
        if (bd91GarbCollController.gccGCState_bd930[i].gcsTaskId_bd930 ==
            Trans.trTaskId_gg00 ) {
            gcoIndex_bd930 = i;
            break;
        }
    }
    if ( -1 == gcoIndex_bd930 ) {
        Kernel_OpError( csp3_bd_msg, csp3_n_obj )
            << __FILE__ << ":" << __LINE__
            << " actual gcoIndex_bd930 not found";
        return;
    }

    FileDir_ReturnCode  fd_rc;
    FileDir_Oms         fd_smart;
    fd_rc = IFDC.GcGetFile(Trans.trTaskId_gg00, ObjFileNo, fd_smart, ErrorList);
    if (FileDir_Okay != fd_rc)
    {
        bd900MapFileDirError(Trans, fd_rc, e_ok, (char*)__FILE__,  __LINE__ );
        /* Files may have been dropped during rollforward. To avoid too many */
        /* misleading messages skip messages if error = "file not found"     */
        if (FileDir_FileNoNotFound != fd_rc)
        {
            Kernel_OpWarning( csp3_bd_msg, csp3_n_obj )
                << __FILE__ << ":" << __LINE__
                << " LVC_IFileDirCache::GetFile error:" << fd_rc
                << " FildId:"
                << ObjFileNo;
        }
        return;
    }

    fd_smart.BuildFileId(Current.curr_tree_id);
    if (Data_OmsVarFile == fd_smart.GetFileType()) 
    {
        cbd05_Current         ContCurrent (Current);
        FileDir_OmsVar fd_var_smart;
        fd_var_smart.Assign(fd_smart);
        for (tsp00_Int4 iContFileNo = 1; iContFileNo <=MAX_NUM_CONT_OBJ_FILES_BD900; ++iContFileNo)
        {
            fd_var_smart.BuildContFileId(iContFileNo,ContCurrent.curr_tree_id);
            bd91_ReleaseAllPagesOfOneFile (Trans, ContCurrent.curr_tree_id, fd_var_smart, 
                iContFileNo, gcoIndex_bd930);
        }
    } 
    else if (Data_OmsKeyFile == fd_smart.GetFileType()) 
    {
        bd22ReleaseObjKeyFiles (Trans, ObjFileNo, fd_smart, ErrorList);
        if ( e_ok != TrError ) {
            Kernel_OpWarning( csp3_bd_msg, csp3_n_obj )
                << __FILE__ << ":" << __LINE__
                << " bd22ReleaseObjKeyFiles TrError:" << TrError;
            if ( e_wrong_obj_file_id == TrError )  // PTS 1137309
                TrError = e_ok;
        }
    }

    bd91_ReleaseAllPagesOfOneFile (Trans, Current.curr_tree_id, fd_smart, 
        PRIM_CONT_OBJ_FILE_NO_BD900, gcoIndex_bd930);
    fd_smart.Detach();      /* decrement global counter */
    fd_rc = IFDC.DelFile(Trans.trTaskId_gg00, ObjFileNo, ErrorList);
    if (( e_ok != TrError ) || (fd_rc != FileDir_Okay))
    {
        Kernel_OpWarning( csp3_bd_msg, csp3_n_obj )
            << __FILE__ << ":" << __LINE__
            << " TrError:" << TrError
            << " DeFile error:" << fd_rc
            << " ObjFileNo:"
            << ObjFileNo;
        if (e_no_converter_entry == TrError)
            TrError = e_ok;
    }
};

/*---------------------------------------------------------------------------*/

externCpp void
bd91ReleaseUnusedObject (tgg00_TransContext      &Trans,
                         Log_UndoFile::Iterator  &historyIterator,
                         const tgg92_KernelOid   &OId,
                         FileDir_FileNo           ObjFileNo,
                         const tgg91_PageRef     &WantedObjVers,
                         GC_IGarbageCollector    &IGarbColl, // PTS 1113774 FF 2002-01-29
                         Msg_List                &errlist)
{
    /* this routine removes a single object and updates the object */
    /* counter and the free page chain, if this is neccessary      */

    tgg00_BasisError   &TrError    = Trans.trError_gg00;
    tgg00_FileId        ObjFileId;

    /* this object writes a message into the vtrace at beginning and end of this routine */
    LVC_ErrorTraceObject   ErrorTraceObject  (Trans, ObjFileId, ObjFileNo, OId, b91garbcol);

    const SAPDB_Bool       bConsistent = true;
    FileDir_Oms   fd_smart;
    FileDir_ReturnCode  fd_rc = FileDir_IDirectory::SessionInstance(Trans).GetFile(
        Trans.trTaskId_gg00, ObjFileNo, fd_smart, errlist);
    if (FileDir_Okay != fd_rc) {
        ErrorTraceObject.SetStatus (rosFileAlreadyDeleted_gg12);
        return;
    }
    fd_smart.BuildFileId(ObjFileId);

    cbd911RecreateIndexLock lock(Trans);  
    if (lock.IsNeeded(ObjFileNo) && (fd_smart.GetKeyPartitionCount() > 0 ))
        lock.Lock( !bExclusive );

    /* create current to access page */
    cbd05_Current Current (Trans, m_update, ObjFileId);

    /*  get page pointer  */
    cbd910_PrimObjDataNode    NptrsPage (Current, nr_for_update);
    NptrsPage.bd910AssignToPage(OId.gg92GetPno()); // PTS 1140252 FF , false /*=dont wait for lock*/); 

    /* release history page to avoid a deadlock; PTS 1106180 AK 29/03/2000 */
    /* PTS 1140252 FF
    if (e_bd_page_collision == TrError)
    {
        TrError = e_ok;
        if(!historyIterator.Invalidate(errlist))
			if(TrError==e_ok)TrError = e_history_remove_error;
        NptrsPage.bd910AssignToPage(OId.gg92GetPno());
    }
    */

    if (e_ok != TrError)
    {
        /* check if the error is due to a drop of the file */
        if ((e_page_in_wrong_tree == TrError) || (e_no_converter_entry == TrError))
        {
            int KeyPartCnt = 0;
            if (Data_FileDeleted == fd_smart.GetFileState()) {
                ErrorTraceObject.SetStatus (rosFileAlreadyDeleted_gg12);
                TrError = e_ok;
            }
            else if (( Data_OmsKeyFile == fd_smart.GetFileType()) ||
                Kernel_IAdminInfo::Instance().KernelStateIsRedoLogProcessing())
            {
                KeyPartCnt = fd_smart.GetKeyPartitionCount();
            }
            if ( KeyPartCnt > 0 )
            {
                ErrorTraceObject.SetStatus (rosPageNotFound_gg12);
                TrError = e_ok;
            }
        }
        return;
    }

    /* BEGIN PTS 1128894 FF 31-MAR-2004 ***************************************** */

    /* determine pointer to the page accomodating the object frame */
    tbd_nodeptr         &pPage     = NptrsPage.np_ptr();
    const tsp00_Int4     ObjKeyLen = pPage->nd_obj_key_len();

    bool bKeyAlreadyRemoved = false;

    FileDir_FileNo fileno;
    bd900GetFileNo(pPage, fileno);
    if ((ObjKeyLen > 0) && (ObjFileNo == fileno))
    {
        BD92CHECK_OID_POSITION ("bd91ReleaseUnusedObject", TrError, OId, NptrsPage);
        tbd900_ObjFrame  *pObjFrame = NptrsPage.bd910ObjFramePtr (OId.gg92GetPos());

        if ((WantedObjVers == pObjFrame->objHeader_bd900.ohdTransInfo_gg92.otrBeforeRef_gg00) &&
            (obsFreeAfterEot_egg00 == pObjFrame->objHeader_bd900.ohdState_gg92))
        {
            /* construct pointer to the key */
            void *const pKey = (tsp00_Byte*) pPage + OId.gg92GetPos() + pPage->nd_obj_frame_len() - ObjKeyLen;

            /* remove OId from hash */
            bd22RemoveOid (*Current.curr_trans, fd_smart, ObjKeyLen, pKey, OId, errlist);
            if (e_hash_key_not_found == TrError)
                TrError = e_ok;
            if ( e_ok == TrError )
            {
                bKeyAlreadyRemoved = true;
                *(unsigned char*)(pKey) = ~(*(unsigned char*)(pKey)); /* modify first byte of key to prevent reuse */
            }
            TrError = e_ok;   /* do handling as done before this change */
        }
    }
    /* END PTS 1128894 FF 31-MAR-2004 ***************************************** */

    /* get pointer to subroot page which is required if this operation forces an update of the page chain */
    cbd910_SubRootNode    NptrsSubRoot  (Current, nr_for_update);

	if ( NptrsPage.bd910LockSubRootBeforeReleasingFrame())
	{
        /* release history page to avoid a deadlock;  PTS 1114691 FF 2002-03-07 */
        /* PTS 1140252 FF
        if(!historyIterator.Invalidate(errlist))
			if(TrError==e_ok)TrError=e_history_remove_error;
        */
        NptrsSubRoot.bd910AssignToSubroot (NptrsPage);
        if (e_ok != TrError)
        {
            /* check if the error is due to a drop of the file */
            if ((e_page_in_wrong_tree == TrError) || (e_no_converter_entry == TrError))
            {
                int KeyPartCnt = 0;
                if (Data_FileDeleted == fd_smart.GetFileState()) {
                    ErrorTraceObject.SetStatus (rosFileAlreadyDeleted_gg12);
                    TrError = e_ok;
                }  else if ( Data_OmsKeyFile == fd_smart.GetFileType()) {
                    KeyPartCnt = fd_smart.GetKeyPartitionCount();
                }
                if (( KeyPartCnt > 0 ) ||
                    Kernel_IAdminInfo::Instance().KernelStateIsRedoLogProcessing())
                {
                        ErrorTraceObject.SetStatus (rosPageNotFound_gg12);
                        TrError = e_ok;
                }
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

    /*------------------------------------------------------------------*/
    /* check if the found object frame contains realy the wanted object */
    /*------------------------------------------------------------------*/

    /* determine pointer to the page accomodating the object frame */
    pPage     = NptrsPage.np_ptr();

    /*  check consistency of key infos stored on the page and stored in the obj file directory */
#   if COMPILEMODE_MEO00 >= QUICK_MEO00
    if ((ObjKeyLen > 0) != (Data_OmsKeyFile == fd_smart.GetFileType()))
    {
        ErrorTraceObject.SetStatus (rosWrongKeyInfo_gg12);
        TrError = e_hash_not_found;
        return;
    }
#   endif

    /* check the obj file no on page */
    FileDir_FileNo contHndl;
    bd900GetFileNo(pPage, contHndl);

    if (ObjFileNo != contHndl)
    {
        /* check if the error is due to a drop of the file */
        if (Data_FileDeleted == fd_smart.GetFileState())
        {
            ErrorTraceObject.SetStatus (rosFileAlreadyDeleted_gg12);
            TrError = e_ok;
        }
        else if ((ObjKeyLen > 0) || ( Kernel_IAdminInfo::Instance().KernelStateIsRedoLogProcessing() ))  /* PTS 1114300 FF 2002-02-20 */
        {
            ErrorTraceObject.SetStatus (rosWrongFileNo_gg12);
            TrError = e_ok;
        }
        else
        {
            FileDir_FileNo contHndl;
            bd900GetFileNo(pPage, contHndl);
            TrError = e_wrong_class_id;
            ErrorTraceObject.WriteVersionErrorMessage (
                contHndl,
                WantedObjVers, NULL);
        }
        return;
    }

  /* determine pointer to object frame */
  BD92CHECK_OID_POSITION ("bd91ReleaseUnusedObject", TrError, OId, NptrsPage);
  tbd900_ObjFrame  *pObjFrame = NptrsPage.bd910ObjFramePtr (OId.gg92GetPos());

  /*--------------------------------------------------------------------------------------------------*/
  /* It can happen that the GC cannot find the object which has to be  deleted. See for               */
  /* instance the history sequence : => new - upd - del - new - upd - del. Releasing an object frame  */
  /* due to the first delete must result in the error 'wrong object version', which has to be ignored */
  /* However, this can only happen in case that the object has a key and its frame was reused         */
  /*--------------------------------------------------------------------------------------------------*/

  /* check object version */
  if (WantedObjVers != pObjFrame->objHeader_bd900.ohdTransInfo_gg92.otrBeforeRef_gg00)
  {
    /* check if this could be regular error which can be ignored */
    if ((ObjKeyLen > 0) || ( Kernel_IAdminInfo::Instance().KernelStateIsRedoLogProcessing() ))  /* PTS 1114300 FF 2002-02-20 */
    {
      ErrorTraceObject.SetStatus (rosWrongObjVersion_gg12);
      TrError = e_ok;
    }
    else
    {
        FileDir_FileNo contHndl;
        bd900GetFileNo(pPage, contHndl);
        TrError = e_wrong_object_version;
        ErrorTraceObject.WriteVersionErrorMessage (
            contHndl,
            WantedObjVers, pObjFrame);
    }
    return;
  }

# if COMPILEMODE_MEO00 >= QUICK_MEO00
  /*  check object state  */
  if (obsFreeAfterEot_egg00  != pObjFrame->objHeader_bd900.ohdState_gg92)
    // note: the Frameversion is not decreased in case of a rollback
    //       therefore the next equation need not be valid
    // (OId.gg92GetFrameVers() != pObjFrame->objHeader_bd900.ohdFrameVersion_gg92)
  {
    TrError = e_wrong_object_version;
    FileDir_FileNo fileno;
    bd900GetFileNo(pPage, fileno);
    ErrorTraceObject.WriteVersionErrorMessage (fileno, WantedObjVers, pObjFrame);
    return;
  }
# endif

  /* ================================================================== */
  /* ----- after passing all checks the object can now be removed ----- */
  /* ================================================================== */
  tgg92_KernelOid auxOid = OId; // only to work around the missing const qualifiers
  if (NptrsPage.np_ptr()->nd_free_obj_cnt() == (NptrsPage.np_ptr()->nd_max_obj_cnt()-1))
      IGarbColl.IncReleaseEmptyPageCount();

  Converter_Version converterVersion =
      bd20GetDestinationConverterVersion ( Trans.trTaskId_gg00, NptrsPage.np_cbptr() );

  bd91RemoveObjFromFile (Current, NptrsPage, NptrsSubRoot, pObjFrame, auxOid,
      converterVersion, fd_smart,
      historyIterator, ErrorTraceObject, bKeyAlreadyRemoved,
       errlist); // new bKeyAlreadyRemoved PTS 1128894
  IGarbColl.IncObjReleaseCount();
}

/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/

externCpp void
bd91RemoveObjFromFile (cbd05_Current           &Current,
                       cbd910_PrimObjDataNode  &NptrsPage,
					   cbd910_SubRootNode      &NptrsSubRoot,
                       tbd900_ObjFrame         *pObjFrame,
                       tgg92_KernelOid         &OId,
                       const Converter_Version &converterVersion,
                       FileDir_Oms             &fd_smart,
                       Log_UndoFile::Iterator  &historyIterator,
                       LVC_ErrorTraceObject    &ErrorTraceObject,
                       bool                     bKeyAlreadyRemoved, // new PTS 1128894 FF 31-MAR-2004
                       Msg_List                &errlist)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd91RemoveObjFromFile", ObjContainer_Trace, 5);

    tgg00_BasisError  &TrError  = Current.curr_trans->trError_gg00;

    /*-------------------------------------------------------*/
    /* remove the object from the page                       */
    /*-------------------------------------------------------*/
    bd91RemoveObjFromPage (Current,  NptrsPage,  pObjFrame,  OId, converterVersion, fd_smart,
        ErrorTraceObject, bKeyAlreadyRemoved); // new PTS 1128894 FF 31-MAR-2004
    if (e_ok != TrError)
    {
        if (Data_FileDeleted == fd_smart.GetFileState())
        {
            ErrorTraceObject.SetStatus (rosFileJustDeleted_gg12);
            TrError = e_ok;
        }
        return;
    }

    /*--------------------------------------------------------------------*/
    /* if the page becomes an empty page by removing the object it is     */
    /* released                                                           */
    /*--------------------------------------------------------------------*/
    if ( NptrsPage.bd910IsEmptyPage())
    {
        /* bd910RemovePageFromChainOfAllPages will release subroot PTS 1135422 */
        NptrsSubRoot.bd910RemovePageFromChainOfAllPages(NptrsPage,
            Current.curr_trans->trTaskId_gg00, converterVersion, fd_smart);
    }
    /*--------------------------------------------------------------------*/
    /* if the page becomes a free page by removing the object             */
    /* the free page chain on root page is updated                        */
    /*--------------------------------------------------------------------*/
	else if (NptrsPage.bd910PageMustBeInsertedIntoFreeChain())
    {
        /* release history page to avoid a deadlock; PTS 1106180 AK 29/03/2000 */
        /* PTS 1140252 FF
        if(!historyIterator.Invalidate(errlist))
			if(TrError==e_ok)TrError = e_history_remove_error;
        */

        NptrsSubRoot.bd910InsertPageIntoFreeChain (NptrsPage, converterVersion, fd_smart);

        if (e_ok != TrError)
        {
            if (Data_FileDeleted == fd_smart.GetFileState()) {
                ErrorTraceObject.SetStatus (rosFileJustDeleted_gg12);
                TrError = e_ok;
            }
            else
            {
                g01opmsg (sp3p_knldiag, sp3m_error, csp3_bd_msg, csp3_n_obj,
                    "bd91RelObj:InsPageFailed", Current.curr_tree_id.fileObjFileNo_gg00());
            }
            return;
        }
#       if COMPILEMODE_MEO00 >= QUICK_MEO00
        if (!(NptrsPage.np_ptr()->nd_objpage_state().includes(objp_no_chain_to_freelist)) &&
            (NptrsPage.np_ptr()->nd_in_free_chain() != (NptrsPage.np_ptr()->nd_free_obj_cnt() > 0)))
        {
            Kernel_OpError( csp3_bd_msg, csp3_n_obj ) << __FILE__ << ":" << __LINE__
                << " root:" << NptrsPage.np_ptr()->nd_root()
                << " actual page:" << NptrsPage.np_ptr()->nd_id()
                << " bad FreeCount";
            g01abort (csp3_bd_msg, csp3_n_obj, "Wrong free page chain   ", 0);
        }
#       endif
    }
}


/*---------------------------------------------------------------------------*/

externCpp void
bd91RemoveObjFromPage (cbd05_Current           &Current,
                       cbd910_PrimObjDataNode  &NptrsPage,
                       tbd900_ObjFrame         *pObjFrame,
                       tgg92_KernelOid         &OId,
                       const Converter_Version &converterVersion,
                       FileDir_Oms             &fd_smart,
                       LVC_ErrorTraceObject    &ErrorTraceObject,
                       bool                     bKeyAlreadyRemoved) // new PTS 1128894 FF 31-MAR-2004
{
    ROUTINE_DBG_MEO00("bd91RemoveObjFromPage");

    tgg00_BasisError  &TrError  = Current.curr_trans->trError_gg00;
    tbd_nodeptr       &pPage    = NptrsPage.np_ptr();
    const tsp00_Int4   ObjKeyLen = pPage->nd_obj_key_len();
    Msg_List           errlist;

    SAPDBERR_ASSERT_STATE (ObjKeyLen == pPage->nd_obj_key_len());
    SAPDBERR_ASSERT_STATE (pObjFrame == NptrsPage.bd910ObjFramePtr (OId.gg92GetPos()));
    SAPDBERR_ASSERT_STATE (fd_smart.GetRootPage() == pPage->nd_root());
    SAPDBERR_ASSERT_STATE (pObjFrame->objHeader_bd900.ohdState_gg92 != obsFree_egg00);


    /*-----------------------*/
    /* remove OMS key        */
    /*-----------------------*/
    if ((ObjKeyLen > 0) &&
        ( !bKeyAlreadyRemoved )) // new PTS 1128894 FF 31-MAR-2004
    {
        /* construct pointer to the key */
        void *const pKey = (tsp00_Byte*) pPage + OId.gg92GetPos() + pPage->nd_obj_frame_len() - ObjKeyLen;

        /* remove OId from hash */
        bd22RemoveOid (*Current.curr_trans, fd_smart, ObjKeyLen, pKey, OId, errlist);
        if (e_hash_key_not_found == TrError)  TrError = e_ok;
        if (e_ok != TrError)
        {
            ErrorTraceObject.SetStatus (rosDelKeyFailed_gg12);   /* PTS 1106760  Alexander Kley 25/06/2000 */
            if (Data_FileDeleted == fd_smart.GetFileState())
            {
                ErrorTraceObject.SetStatus (rosFileJustDeleted_gg12);
                TrError = e_ok;
            }
            else
                g01opmsg (sp3p_knldiag, sp3m_error, csp3_bd_msg, csp3_n_obj, "b91RmObjFP:bd22RemoveOid", TrError);
            return;
        }
    }

    /*--------------------------------*/
    /* remove the continuation object */
    /*--------------------------------*/
    if (pt2VarObject_egg00 == pPage->nd_pt2())
    {
        tbd900_VarObjBody &VarObjBody  = *((tbd900_VarObjBody*) (pObjFrame->objBody_bd900));
        tsp00_Int4 ContObjFileNo = VarObjBody.vbdHeader_bd900.vhdContObjFileNo_bd900;

        /* check if there is really a continuation object */
        if (PRIM_CONT_OBJ_FILE_NO_BD900 != ContObjFileNo)
        {
            tsp00_PageNo     ContObjPno = VarObjBody.vbdHeader_bd900.vhdContObjPno_bd900;
            tgg00_ObjPagePos ContObjPos = VarObjBody.vbdHeader_bd900.vhdContObjPos_bd900;


            FileDir_OmsVar   fd_var_smart;
            fd_var_smart.Assign(fd_smart);

            /* remove continuation object */
            bd93RemoveContObj (Current, converterVersion, fd_var_smart, OId, ContObjFileNo, ContObjPno, ContObjPos);
            if (e_ok != TrError)
            {
                if (Data_FileDeleted == fd_smart.GetFileState()) {
                    ErrorTraceObject.SetStatus (rosFileJustDeleted_gg12);
                    TrError = e_ok;
                }
                else
                {
                    g01opmsg (sp3p_knldiag, sp3m_error, csp3_bd_msg, csp3_n_obj, "b91RmObjFP:bd23RmContObj", TrError);
                    ErrorTraceObject.SetStatus (rosDelContObjFailed_gg12);   /* PTS 1106760  Alexander Kley 25/06/2000 */
                    return;
                }
            }
        }

        /* reset var object header */
        VarObjBody.vbdHeader_bd900.bd900SetToEmptyHeader();
    }

    /*-----------------------*/
    /* remove primary object */
    /*-----------------------*/

    /* the primary object is removed as last object, to ensure that free-list-counter and chaining to free-list */
    /* is only done, if all other resources have been cleaned up completely                                     */
    /*  update object counter in page header  */
    ++pPage->nd_free_obj_cnt();

    if (pObjFrame->objHeader_bd900.ohdState_gg92 == obsFreeAfterEot_egg00)
        --pPage->nd_free_eot_obj_cnt();
    else
    {
        SAPDBERR_ASSERT_STATE (pObjFrame->objHeader_bd900.ohdState_gg92 != obsFree_egg00);
        SAPDBERR_ASSERT_STATE (Kernel_IAdminInfo::Instance().KernelStateIsRedoLogProcessing());
        --pPage->nd_occ_obj_cnt();
    }

    /*  update object state  */
    pObjFrame->objHeader_bd900.ohdState_gg92.becomes(obsFree_egg00);

  	/* invalidate the object version PTS 1113123 13/12/2001 */
	pObjFrame->objHeader_bd900.ohdTransInfo_gg92.otrBeforeRef_gg00.gg91SetNilRef();

    /*  insert object into free object chain */
    pObjFrame->objHeader_bd900.ohdNextFreeObj_gg92 = pPage->nd_first_free_obj();
    pPage->nd_first_free_obj()                     = OId.gg92GetPos();

    /*---------------------------------------------*/
    /* update counter of all objects in chaininfo  */
    /*---------------------------------------------*/
    fd_smart.ModifyEntryCount(Current.curr_trans->trTaskId_gg00,converterVersion, -1);
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
