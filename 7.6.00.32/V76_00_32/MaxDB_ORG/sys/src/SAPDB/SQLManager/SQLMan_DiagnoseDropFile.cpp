/*****************************************************************************/
/*!
  @file         SQLMan_DiagnoseDropFile.cpp
  @author       MartinKi
  @ingroup

  @brief        Implementation file for class SQLMan_DiagnoseDropFile.

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

#include "SQLManager/SQLMan_DiagnoseDropFile.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "SQLManager/SQLMan_Tracing.hpp"
#include "SQLManager/SQLMan_Types.hpp"
#include "SAPDB/SQLManager/Catalog/Catalog_TableDescriptor.hpp"
#include "SAPDB/SQLManager/Catalog/Catalog_IndexDescriptor.hpp"

#include "FileDirectory/FileDir_ShortColumn.hpp"
#include "FileDirectory/FileDir_Index.hpp"
#include "FileDirectory/FileDir_Table.hpp"
#include "FileDirectory/FileDir_Generic.hpp"
#include "FileDirectory/FileDir_Iterator.hpp"
#include "FileDirectory/FileDir_ISharedDirectory.hpp"
#include "FileDirectory/FileDir_IDirectory.hpp"

#include "Messages/Msg_List.hpp"
#include "DataAccess/Data_Types.hpp"

#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include "SAPDBCommon/SAPDB_Types.h"

#include "hbd01.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

SQLMan_DiagnoseDropFile::ReturnCode
SQLMan_DiagnoseDropFile::Execute(
    SQLMan_Context&       sqlContext,
    const FileDir_FileNo& fileNo,
    Msg_List&             errMsg)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "SQLMan_DiagnoseDropFile::Execute", SQLMan_Trace, 1 );

    const RTE_TaskId& taskId = sqlContext.GetTaskId();
    FileDir_IDirectory& fdir = FileDir_ISharedDirectory::IFileDirInstance();

    FileDir_Generic fileEntry;
    if ( fdir.GcGetFile( taskId, fileNo, fileEntry, errMsg )
         != FileDir_Okay ) {
        return FileNotFound;
    }

    // check if related catalog objects exist for entry
    FileDir_Table tableFile;
    FileDir_Index indexFile;
    FileDir_ShortColumn shortColumnFile;
    switch ( fileEntry.GetFileType() ) {
        case Data_TableFile:
        {
            Catalog_TableDescriptor tableDesc( sqlContext, fileNo );
            if ( tableDesc ) {
                // FIXME: add message
                return CatalogObjectsExist;
            }
            sqlContext.ResetReturnCode();
            break;
        }
        case Data_IndexFile:
        {
            indexFile.Assign( fileEntry );
            if ( indexFile.GetParentFileNo().IsValid() ) {
                fdir.GcGetFile(
                    taskId, indexFile.GetParentFileNo(), tableFile, errMsg );
            }
            // if table for index exists, it will be assigned to tableFile
            break;
        }
        case Data_ShortColumnFile:
        {
            shortColumnFile.Assign( fileEntry );
            if ( shortColumnFile.GetParentFileNo().IsValid() ) {
                fdir.GcGetFile(
                    taskId, shortColumnFile.GetParentFileNo(),
                    tableFile, errMsg );
            }
            // if table for short column file exists, it will be
            // assigned to tableFile
            break;
        }
    }

    if ( tableFile.IsAssigned() ) {
            Catalog_TableDescriptor tableDesc(
                sqlContext, tableFile.GetFileNo() );
            if ( tableDesc ) {
                if ( indexFile.IsAssigned() ) {
                    if ( tableFile.GetIndexFileNo( indexFile.GetIndexId() )
                         == indexFile.GetFileNo() ) {
                        // table considers this index its own
                        Catalog_IndexDescriptor indexDesc =
                            tableDesc.GetIndexDescriptor(
                                indexFile.GetIndexId() );
                        if ( indexDesc ) {
                            // FIXME: add error message
                            return CatalogObjectsExist;
                        }
                    }
                } else if ( shortColumnFile.IsAssigned()
                            && ( tableFile.GetShortColumnFileNo()
                                 == shortColumnFile.GetFileNo() ) ) {
                    // table considers this short column file its own
                    // FIXME: add error message
                    return CatalogObjectsExist;
                }
            }
            sqlContext.ResetReturnCode();
    }

    // if we reach here then the file and all dependent files can be dropped
    tgg00_FileId dropFileId;
    if ( fileEntry.GetFileType() == Data_TableFile ) {
        // drop all files related to the table
        tableFile.Assign( fileEntry );
        FileDir_Generic dropFile;
        for (int i = 1; i <= tableFile.GetMaxIndexId(); ++i ) {
            FileDir_FileNo indexFileNo( tableFile.GetIndexFileNo( i ) );
            if ( indexFileNo.IsInvalid() ) {
                continue;
            }

            fdir.GcGetFile( taskId, indexFileNo, dropFile, errMsg );
            if ( ! dropFile.IsAssigned() ) {
                continue;
            }

            SAPDBERR_ASSERT_STATE( dropFile.GetFileType() == Data_IndexFile );

            dropFile.BuildFileId( dropFileId );
            b01pdestroy_perm_file( sqlContext.TransContext(), dropFileId );
            sqlContext.TransContext().trError_gg00 = e_ok; // ignore errors
        }

        if ( tableFile.GetShortColumnFileNo().IsValid()
             && ( fdir.GcGetFile(
                      taskId, tableFile.GetShortColumnFileNo(),
                      dropFile, errMsg )
                  == FileDir_Okay ) ) {
            dropFile.BuildFileId( dropFileId );
            b01pdestroy_perm_file( sqlContext.TransContext(), dropFileId );
            sqlContext.TransContext().trError_gg00 = e_ok; // ignore errors
        }
    }

    // finally, drop the file that was requested
    fileEntry.BuildFileId( dropFileId );
    b01pdestroy_perm_file( sqlContext.TransContext(), dropFileId );
    sqlContext.TransContext().trError_gg00 = e_ok; // ignore errors

    return DropOk;
}
