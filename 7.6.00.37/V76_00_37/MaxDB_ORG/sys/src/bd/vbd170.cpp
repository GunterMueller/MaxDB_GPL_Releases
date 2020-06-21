/*****************************************************************************/
/*!

  @file         vbd170.cpp
  @author       MartinKi
  @ingroup      FileDirectory_Files

  @brief        pascal interface

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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

\endif
*/
/*****************************************************************************/

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "hbd170.h"
#include "ggg00.h"
#include "gsp00.h"

#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"

#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"

#include "FileDirectory/FileDir_Iterator.hpp"
#include "FileDirectory/FileDir_Table.hpp"
#include "FileDirectory/FileDir_Generic.hpp"
#include "FileDirectory/FileDir_Common.hpp"
#include "FileDirectory/FileDir_Directory.hpp"
#include "FileDirectory/FileDir_IDirectory.hpp"
#include "KernelCommon/Kernel_IAdminInfo.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp"
#include "RunTime/System/RTESys_MemoryBarrier.h"
#include "RunTime/RTE_Crash.hpp"
#include "Messages/Msg_List.hpp"

#include "hbd01_1.h"
#include "hbd03.h"
#include "hbd30.h"
#include "ggg00.h"
#include "gbd00.h"
#include "gsp00_2.h"

class SAPDBMem_IRawAllocator;

/*===========================================================================*/

externPascal void bd170CreateUserFiledirectory (
    tgg00_TransContext    VAR_VALUE_REF  Trans,
    tsp00_Addr            VAR_VALUE_REF  pFiledir)
{
    SAPDBTRACE_ROUTINE_DEBUG( "bd170CreateUserFiledirectory",
                              FileDir_Trace, 5 );

    if ( pFiledir != 0 )
    {
        Trans.trError_gg00 = e_not_implemented; // ++++++ crash ?!
        return;
    }

    SAPDBMem_IRawAllocator *allocator;
    if ( Kernel_IAdminInfo::Instance().KernelStateIsRedoLogProcessing() ) {
        allocator = &RTEMem_Allocator::Instance();
    } else {
        allocator = reinterpret_cast<SAPDBMem_IRawAllocator*>(
            Trans.trAllocator_gg00);
    }


    FileDir_IDirectory *filedir =
        new (*allocator) FileDir_Directory(*allocator);

    if ( 0 == filedir )
    {
        Trans.trError_gg00 = e_no_more_memory;
        return;
    }

    if ( ! filedir->Initialize() )
    {
        filedir->~FileDir_IDirectory();

        allocator->Deallocate(filedir);

        pFiledir = 0;

        Trans.trError_gg00 = e_no_more_memory;
        return;
    }

    // we need a memory barrier for synchronisation with Trans_Iter:
    RTESys_WriteMemoryBarrier();
    pFiledir = reinterpret_cast<tsp00_Addr>(filedir);
}

/*===========================================================================*/

externPascal void bd170DeleteUserFiledirectory (
    tgg00_TransContext    VAR_VALUE_REF  Trans,
    tsp00_Addr            VAR_VALUE_REF  pFiledir)
{
    SAPDBTRACE_ROUTINE_DEBUG( "bd170DeleteUserFiledirectory",
                              FileDir_Trace, 5 );

    if ( 0 == pFiledir )
    {
        Trans.trError_gg00 = e_not_implemented; // ++++++ crash ?!
        return;
    }

    FileDir_IDirectory* auxFileDir =
        reinterpret_cast<FileDir_IDirectory*>(pFiledir);

    SAPDBMem_IRawAllocator& allocator = auxFileDir->GetAllocator();

    // slowknl    : dump content of file dir if not empty and abort
    // quick, fast: delete all remaining files in directory

    { // do all this in new block to have iterator object destroyed
      // before calling shutdown
        FileDir_Iterator it( allocator );
        auxFileDir->GetPrivateFileDirIterator( it, &Trans );
        FileDir_Generic fp;
#ifdef SAPDB_SLOW
        if ( auxFileDir->GetEntryCount() > 0 ) {
            Kernel_VTrace() << NewLine
                            << "***Session file directory not empty at end of session***"
                            << NewLine
                            << "Dumping contents:";
            while ( it.GetNext( fp ) ) {
                if ( fp.GetComment() != 0 ) {
                    Kernel_VTrace() << "fileNo: " << fp.GetFileNo()
                                    << ", " << fp.GetComment();
                } else {
                    Kernel_VTrace() << "fileNo: " << fp.GetFileNo();
                }
                Kernel_VTrace() << "        LeafPageCount: "
                                << fp.GetLeafPageCount(Trans.trTaskId_gg00);
            }
//         RTE_Crash(
//             SAPDBErr_Exception(
//                 __CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED,
//                 "'session file directory must be empty, check vtrace'") );
        }

#else

        tgg00_FileId fileId;
        while ( it.GetNext( fp ) && (Trans.trError_gg00 == e_ok) ) {

            tsp00_Int blobColCount = 0;
            fp.BuildFileId (fileId);

            switch ( fp.GetFileType() ) {
                case Data_TableFile:
                {
                    FileDir_Table ft;
                    ft.Assign( fp );
                    blobColCount = ft.GetBlobColCount();
                    ft.Detach();
                    break;
                }
            }
            const SAPDB_Bool isIndex = ( fp.GetFileType() == Data_IndexFile );
            fp.Detach();

            tbd_current_tree current;
            const bool c_only_empty = true;

            fileId.fileBdUse_gg00().addElement( bd_release_acc );
            bd30GetTree( Trans, fileId, current, m_drop,
                    LOCK_TREE_EXCL_BD00, ! SYNC_TREE_LOCK_BD00 );
            if ( isIndex ) {
                bd03ReleaseInvTree (Trans, fileId);
            } else {
                bd30DropTree (current, blobColCount, !c_only_empty);
            }
            bd30ReleaseTree (current);

            // ignore errors
            Trans.trError_gg00 = e_ok;
        }
#endif
    }

    if ( Trans.trError_gg00 == e_file_not_found ) {
            Trans.trError_gg00 = e_ok;
    }

    auxFileDir->Shutdown (Trans.trTaskId_gg00);

    auxFileDir->~FileDir_IDirectory();

    allocator.Deallocate(static_cast<FileDir_Directory*>(auxFileDir));

    pFiledir = 0;
    RTESys_WriteMemoryBarrier();
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
