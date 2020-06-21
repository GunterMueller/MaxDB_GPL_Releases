/*****************************************************************************/
/*!
 @file    FileDir_TempTableEntry.hpp
 @author  MartinKi
 @ingroup FileDirectory_Files

 @brief   Header file for class FileDir_TempTableEntry.

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

#ifndef FILEDIR_TEMPTABLEENTRY_HPP
#define FILEDIR_TEMPTABLEENTRY_HPP

#include "FileDirectory/FileDir_TempEntry.hpp"
#include "FileDirectory/FileDir_ITable.hpp"
#include "FileDirectory/FileDir_Vector.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"

#include "DataAccess/Data_FileTypes.hpp"
// for Data_PageNo:
#include "DataAccess/Data_Types.hpp"

#include "RunTime/RTE_Types.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"

#include "ggg91.h"
#include "ggg00.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/
/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/
/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/

class Converter_Version;
class SAPDBMem_IRawAllocator;

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
 * @class FileDir_TempTableEntry
 * @ingroup FileDirectory_Classes
 * @copydoc FileDir_Entry
 * @author MartinKi
 *
 * @brief A file directory entry representing a table.
 */
class FileDir_TempTableEntry : public FileDir_TempEntry,
                               public FileDir_ITable
{
    typedef FileDir_Vector<FileDir_FileNo> IndexList;

    IndexList m_indexList;
    SAPDB_UInt2 m_indexCount;
    SAPDB_UInt2 m_blobColCount;
    tgg00_FiletypeSet m_fileTypeSet;  ///< For compatibility.
    tgg91_FileVersion m_fileVersion;
public:
    /*!
     * @brief Constructs a file directory entry for a table.
     *
     * @param allocator         [in] allocator entry should use
     * @param taskId            [in] task id of calling task
     * @param fileNo            [in] file id of this table entry
     * @param converterVersion  [in] converter version this entry is
     *                               created in
     * @param rootPage          [in] rootPageNo of the table belonging
     *                               to this entry
     * @param blobColCount     [in] number of blob columns of the table
     * @param fileTypeSet      [in] file type set of the index file
     */
   FileDir_TempTableEntry(
       SAPDBMem_IRawAllocator&  allocator,
       const RTE_TaskId         taskId,
       const FileDir_FileNo&    fileNo,
       const Converter_Version& converterVersion,
       const Data_PageNo&       rootPage,
       const SAPDB_UInt2        blobColCount,
       const tgg00_FiletypeSet& fileTypeSet);

    /*!
     * @copydoc FileDir_TableEntry::AddBlobColumn
     */
    FileDir_ReturnCode AddBlobColumn(
        const Converter_Version& converterVersion,
        const SAPDB_UInt2 columnNumber);

    /*!
     * @copydoc FileDir_TableEntry::RemoveBlobColumn
     */
    FileDir_ReturnCode RemoveBlobColumn(
        const Converter_Version& converterVersion,
        const SAPDB_UInt2 columnNumber);

    /*!
     * @brief Returns the file type of this entry (File_TempTable).
     */
    Data_FileType GetFileType() const;

    /*!
     * @copydoc FileDir_TableEntry::GetBlobColCount
     */
    SAPDB_UInt2 GetBlobColCount() const;

    /*!
     * @copydoc FileDir_TableEntry::GetMaxIndexId
     */
    SAPDB_UInt2 GetMaxIndexId() const;

    /*!
     * @copydoc FileDir_TableEntry::GetIndexFileNo
     */
    FileDir_FileNo GetIndexFileNo(SAPDB_UInt2 indexNo) const;

    /*!
     * @copydoc FileDir_TableEntry::AddIndex
     */
    FileDir_ReturnCode AddIndex(
        const FileDir_FileNo& indexFileNo,
        const SAPDB_UInt2     indexId);

    /*!
     * @copydoc FileDir_TableEntry::RemoveIndex
     */
    FileDir_ReturnCode RemoveIndex(
        const FileDir_FileNo& indexFileNo);

    void SetShortColumnFileNo(
        const FileDir_FileNo shortColumnFileNo);

    void GetShortColumnFileNo(
        FileDir_FileNo& shortColumnFileNo) const;

    FileDir_FileNo GetShortColumnFileNo() const;

    SAPDB_Bool HasShortColumnFile() const;

    /*!
     * @copydoc FileDir_TableEntry::IsLobPageCounterInitialized
     */
    SAPDB_Bool IsLobPageCounterInitialized() const;

    /*!
     * @copydoc FileDir_TableEntry::GetLobPageCount
     */
    SAPDB_UInt8 GetLobPageCount() const;

    /*!
     * @copydoc FileDir_TableEntry::ModifyLobPageCount
     */
    void ModifyLobPageCount(
        const RTE_TaskId        taskId,
        const Converter_Version converterVersion,
        const SAPDB_Int4        delta);

    /*!
     * @copydoc FileDir_TableEntry::SetLobPageCount
     */
    void SetLobPageCount(
        const SAPDB_UInt8 lobPageCount,
        const SAPDB_Bool  setInitialized = true);

    /*!
     * @brief Initializes the counters with a given set of values.
     */
    void InitializeTableCounters(
        const RTE_TaskId  taskId,
        const SAPDB_UInt4 leafPageCount,
        const SAPDB_UInt4 indexPageCount,
        const SAPDB_UInt8 entryCount,
        const SAPDB_UInt8 lobPageCount );

    void FillFileInfo(tbd_fileinfo& fileInfo) const;

    void SetFileInfo(const RTE_TaskId taskId, const tbd_fileinfo& fileInfo);

    /*!
     * Set whether table is an archive table or not.
     */
    void SetArchive(SAPDB_Bool isArchive);

    /*!
     * @returns true if table is an archive table
     */
    SAPDB_Bool IsArchive() const;

    /*!
     * @brief Add this entry's contents to dumpBuffer.
     */
    SAPDB_Bool FillDumpEntry(
        const RTE_TaskId   taskId,
        SAPDBFields_Field& dumpBuffer,
        SAPDB_Int&         startPos);

    /*!
     * @brief Prepares fileId for this file entry for use in bd
     *        procedures.
     *
     * @copydoc FileDir_BaseEntry::BuildFileId
     */
    void BuildFileId(tgg00_FileId& fileId) const;

    /*!
     * @brief Adds the file root page and file type to an already
     * prepared file id. fileVersion_gg00 will <em>not</em> be changed.
     *
     * @copydoc FileDir_BaseEntry::FillPreparedFileId
     */
    void FillPreparedFileId(tgg00_FileId& fileId) const;

    /*!
     * No-op for temp tables.
     */
    void SetRecordsPacked(
        const RTE_TaskId taskId,
        const SAPDB_Bool recordsPacked);

    /*!
     * Always returns false.
     */
    SAPDB_Bool AreRecordsPacked() const;

    /*!
     * Always returns false.
     */
    SAPDB_Bool IsDynamic() const;

protected:
    FileDir_ReturnCode Initialize();

private:
    bool m_initialized;
    SAPDB_UInt2 GetPersistentLength();
    SAPDB_UInt2 GetFirstBlobDescOffset();

    FileDir_ITable* AsTable();
};

#endif // FILEDIR_TEMPTABLEENTRY_HPP
