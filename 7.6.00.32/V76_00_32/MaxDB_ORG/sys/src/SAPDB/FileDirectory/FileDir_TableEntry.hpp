/*****************************************************************************/
/*!
 @file    FileDir_TableEntry.hpp
 @author  MartinKi
 @ingroup FileDirectory_Files

 @brief   Header file for class FileDir_TableEntry.

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

#ifndef FILEDIR_TABLEENTRY_HPP
#define FILEDIR_TABLEENTRY_HPP

#include "FileDirectory/FileDir_Entry.hpp"
#include "FileDirectory/FileDir_ITable.hpp"
#include "DataAccess/Data_FileTypes.hpp"
#include "FileDirectory/FileDir_Vector.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"

// for Data_PageNo:
#include "DataAccess/Data_Types.hpp"

#include "ggg91.h"

#include "RunTime/RTE_Types.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"

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
struct tgg00_FileId;

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
 * @class FileDir_TableEntry
 * @ingroup FileDirectory_Classes
 * @copydoc FileDir_Entry
 * @author MartinKi
 *
 * @brief A file directory entry representing a table.
 *
 */
class FileDir_TableEntry : public FileDir_Entry,
                           public FileDir_ITable
{
    friend class FileDir_SharedDirectory;
    friend class FileDir_Directory;

   // info variables
    struct PersistentTableData : FileDir_Entry::PersistentData
    {
        SAPDB_UInt2 blobColCount;
        SAPDB_Int2  fileVersion;
        SAPDB_UInt1 fileTypeSet;
        SAPDB_UInt1 tableAttributes;
        SAPDB_Int2  filler;
        SAPDB_Int8  lobPageCount;
    };

    typedef FileDir_Vector<FileDir_FileNo> IndexList;
    IndexList m_indexList;
    SAPDB_UInt2 m_indexCount;

    PersistentTableData* PersistentTablePointer() const {
        return static_cast<PersistentTableData*>(
            FileDir_Entry::PersistentPointer());
    }

protected:
    enum TableAttributes {
        tAttrNone          = 0,
        tAttrRecordsPacked = 1
    };

    /*!
     * @brief Constructor to be used during file directory migrations
     *
     * Creates a new entry in the current persistent layout and
     * removes the old entry from the persistent file directory data
     * pages.
     *
     * @param taskId [in] id of current task
     * @param allocator [in] allocator that should be used for this
     *                       entry; should be multi-thread safe
     * @param persistentEntry [in/out] pointer to the persistent entry
     *                                 that should be migrated
     * @param currentPersistentLayout [in] layout version of
     *              persistentEntry, this determines which migration steps
     *              will be taken
     * @param curPersistentPos [in/out] points to the memory position at
     *              which the current entry's data starts, for internal
     *              use only
     */
    FileDir_TableEntry(
        const RTE_TaskId        taskId,
        SAPDBMem_IRawAllocator& allocator,
        FileDir_Entry&          persistentEntry,
        FileDir_EntryLayouts    currentPersistentLayout,
        SAPDB_Byte*&            curPersistentPos);

public:
    /*!
     * @brief Constructs a file directory entry for a table.
     *
     * @param taskId            [in] task id of calling task
     * @param allocator         [in] allocator to use
     * @param fileNo            [in] file id of this table entry
     * @param converterVersion  [in] converter version this entry is
     *                               created in
     * @param rootPageNo        [in] rootPageNo of the table belonging
     *                               to this entry
     * @param blobColCount      [in] number of blob columns of the table
     * @param fileTypeSet       [in] file type set of table
     *                               (vbd17 compatibility)
     * @param fileVersion       [in] initial file version
     */
   FileDir_TableEntry(const RTE_TaskId taskId,
                      SAPDBMem_IRawAllocator&  allocator,
                      const FileDir_FileNo&    fileNo,
                      const Converter_Version& converterVersion,
                      const Data_PageNo&       rootPageNo,
                      const SAPDB_UInt2        blobColCount,
                      const tgg00_FiletypeSet& fileTypeSet,
                      const tgg91_FileVersion& fileVersion);

    /*!
     * @brief Recreates table file directory entry from its persistent data.
     *
     * @param allocator       [in] allocator to use
     * @param persistentEntry [in] entry pointing to the persistent data
     *                             to be used.
     */
    FileDir_TableEntry(
        SAPDBMem_IRawAllocator& allocator,
        FileDir_Entry&          persistentEntry);

    /*!
     * @brief Adds a blob column to the description of the table.
     *
     * @param converterVersion  [in] converter version in which the blob
     *                               column is being added
     * @param columnNumber      [in] column number of blob column
     *
     * @return
     *      - <em>FileDir_Okay</em> if successful
     *      - <em>FileDir_DuplicateBlobColumn</em> if a blob column with
     *        columnNumber already exists
     *      - <em>FileDir_OutOfMemory</em> if memory for new entry could
     *        not be allocated
     */
    FileDir_ReturnCode AddBlobColumn(
        const Converter_Version& converterVersion,
        const SAPDB_UInt2        columnNumber);

    /*!
     * @brief Removes the blob column with column number columnNumber
     *        from the description of the table.
     *
     * @param converterVersion  [in] converter version in which the blob
     *                               column is being added
     * @param columnNumber      [in] column number of blob column to
     *                               be removed
     * @return
     *      - <em>FileDir_Okay</em> if successful
     *      - <em>FileDir_BlobColumnNotFound</em> if columnNumber was
     *        not found
     */
    FileDir_ReturnCode RemoveBlobColumn(
        const Converter_Version& converterVersion,
        const SAPDB_UInt2        columnNumber);

    /*!
     * @brief Returns the file type of this entry (Data_TableFile).
     */
    Data_FileType GetFileType() const;

    /*!
     * @brief Returns the number of blob columns defined for the table.
     */
    SAPDB_UInt2 GetBlobColCount() const;

    /*!
     * @brief Returns the mapped blob column number for colNo.
     *
     * @param colNo [in] blob column number for which mapped number
     *                   should be returned.
     *
     * @return mapped blob column number
     */
    SAPDB_UInt2 GetBlobColNo(const SAPDB_UInt2 colNo) const;

    /*!
     * @brief Returns the number of indices defined on this table.
     *
     * @return Returns the highest id of any of the indices defined on
     * this table. Note that not all ids between 1 and GetMaxIndexId
     * necessarily contain a valid fileNo.
     */
    SAPDB_UInt2 GetMaxIndexId() const;

    /*!
     * @brief Returns the file id of an index.
     */
    FileDir_FileNo GetIndexFileNo(SAPDB_UInt2 indexNo) const;

    /*!
     * @brief Applies deltas accumulated during savepoint to various counters.
     *
     * @param taskId           [in] task id of calling task
     * @param converterVersion [in] current converter version
     */
    void UpdateAfterSavepoint(
        const RTE_TaskId         taskId,
        const Converter_Version& converterVersion);

    /*!
     * @brief Adds an index description to the table.
     *
     * @param indexFileNo [in] file no of index description
     * @param indexId     [in] index no of index to be added
     *                         (first index starts with index no 1)
     *
     * @return
     *     - <em>FileDir_Okay</em> on success
     *     - <em>FileDir_DuplicateIndex</em> if index with indexFileNo
     *        already exists
     */
    FileDir_ReturnCode AddIndex(
        const FileDir_FileNo& indexFileNo,
        const SAPDB_UInt2     indexId);

    /*!
     * @brief Removes an index description from the table.
     *
     * @param indexFileNo [in] file no to be removed
     *
     * @return
     *    - FileDir_Okay on success
     *    - FileDir_IndexNotFound if indexFileDir_FileNo was not found
     */
    FileDir_ReturnCode RemoveIndex(
        const FileDir_FileNo& indexFileNo);

    /*!
     * @brief Removes an index description from the table.
     *
     * @param indexNo [in] index to be removed (first index has index no 1)
     *
     * @return
     *    - FileDir_Okay on success
     *    - FileDir_IndexNotFound if indexNo does not exist
     */
    FileDir_ReturnCode RemoveIndexNo(
        const SAPDB_UInt2 indexNo);

    /*!
     * @brief Sets the file no of the short column file of this table.
     *
     * If there is already a valid value for a short column file set,
     * this value will be silently overwritten.
     */
    void SetShortColumnFileNo(
        const FileDir_FileNo shortColumnFileNo);

    /*!
     * @brief Sets shortColumnFileNo to the file no of the short
     *        column file of this table.
     *
     * If no short column file has been set for this table,
     * shortColumnFileNo will be invalid upon return.
     */
    void GetShortColumnFileNo(
        FileDir_FileNo& shortColumnFileNo) const;

    /*!
     * @brief Returns the fileNo of the short column file.
     *
     * @return fileNo of the short column file of this table. fileNo
     * is invalid if the table does not have a short column file.
     */
    FileDir_FileNo GetShortColumnFileNo() const;

    /*!
     * @brief Returns true, if a short column file has been set for
     *        this table.
     */
    SAPDB_Bool HasShortColumnFile() const;

    /*!
     * @brief Returns true if the lob page counter has been
     * initialized and its return value can be used
     */
    SAPDB_Bool IsLobPageCounterInitialized() const;

    /*!
     * @brief Adds delta pages to the lob page counter of this entry
     *
     * @warning Keep in mind that this counter only tracks the number
     * of lob pages stored in real lob files. Pages used by the short
     * column file should be tracked only there.
     *
     * @param taskId           [in] task id of calling task
     * @param converterVersion [in] the converterVersion the pages
     *                              were added in
     * @param delta            [in] number of pages added/removed
     */
    virtual void ModifyLobPageCount(
        const RTE_TaskId        taskId,
        const Converter_Version converterVersion,
        const SAPDB_Int4        delta);

    /*!
     * @brief Returns the number of lob pages that are stored in
     * separate B* trees
     *
     * The return value does not include the lob pages that are stored
     * in the short column file that belongs to this entry.
     *
     * - If the table does not have any lob columns, 0 is returned.
     * - If the counter has not yet been initialized, -1 is
     * returned. (You should use IsLobPageCounterInitialized() to
     * check if the counter values are initialized).
     */
    SAPDB_UInt8 GetLobPageCount() const;

    /*!
     * @brief Sets the lob page count for this entry
     *
     * @param lobPageCount [in] the new lob page count
     * @param setInitialized [in] if true, then the lob page counter
     * is considered to be initialized and will be updated during
     * ModifyLobPageCount; if false, then the lob page counter will be
     * invalidated
     */
    void SetLobPageCount(
        const SAPDB_UInt8 lobPageCount,
        const SAPDB_Bool  setInitialized = true);

    /*!
     * @brief Initializes the statistics counters with a given set of
     *        values.
     *
     * @param taskId         [in] id of current task
     * @param leafPageCount  [in] new leaf page count
     * @param indexPageCount [in] new leaf page count
     * @param entryCount     [in] new entry count
     * @param lobPageCount   [in] new lob page count
     */
    void InitializeTableCounters(
        const RTE_TaskId  taskId,
        const SAPDB_UInt4 leafPageCount,
        const SAPDB_UInt4 indexPageCount,
        const SAPDB_UInt8 entryCount,
        const SAPDB_UInt8 lobPageCount );

    /*!
     * @brief Returns a tbd_fileinfo structure describing the
     *        table.
     *
     * <em>For compatibility reasons with old file directory
     * (vbd17)-interface only.</em>
     */
    void FillFileInfo(tbd_fileinfo& fileInfo) const;

    /*!
     * @brief Initialises the entry according to the information in fileInfo.
     */
    void SetFileInfo(const RTE_TaskId taskId, const tbd_fileinfo& fileInfo);

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
     * @brief Returns the file version of the entry. If the entry has
     * no file version associated with it, the dummy file version is
     * returned.
     *
     * @copydoc FileDir_BaseEntry::GetFileVersion
     */
    void GetFileVersion(tgg91_FileVersion& fileVersion) const;

    FileDir_ReturnCode Initialize();

    SAPDB_Bool FillDumpEntry(
        const RTE_TaskId   taskId,
        SAPDBFields_Field& dumpBuffer,
        SAPDB_Int&         startPos);

    /*!
     * @brief Returns the file version of the table.
     */
    const tgg91_FileVersion& GetFileVersion() const;

    /*!
     * @brief Sets the new file version of the table.
     */
    void SetFileVersion(const tgg91_FileVersion& newFileVersion);

    /*!
     * @brief Sets the blob column count of this table.
     */
    void SetBlobColCount(const SAPDB_UInt2 newBlobColCount);

    /*!
     * @name Table attributes
     */
    /*@{*/

    /*!
     * Set whether table is an archive table or not.
     *
     * @param isArchive [in] new archive state of entry
     */
    void SetArchive(SAPDB_Bool isArchive);

    /*!
     * @return true if table is an archive table.
     */
    SAPDB_Bool IsArchive() const;

    /*!
     * Sets whether records are stored in compressed format on the
     * b*-tree pages.
     *
     * You are not allowed to change this parameter once there have
     * been records written into the file.
     *
     * @param taskId [in] task id of running task
     * @param recordsPacked [in] records are stored in compressed
     * format (if true), records are stored in normal format (if
     * false)
     */
    void SetRecordsPacked(
        const RTE_TaskId taskId,
        const SAPDB_Bool recordsPacked);

    /*!
     * @return true, if records are stored in compressed format on the
     * b*-tree pages
     */
    SAPDB_Bool AreRecordsPacked() const;

    /*!
     * @return true, if ftsDynamic is set in tgg00_FiletypeSet,
     * implies that certain tree operations will be skipped and that
     * the tree might degrade.
     */
    SAPDB_Bool IsDynamic() const;
    /*@}*/

private:
    SAPDB_UInt2 GetPersistentLength();
    SAPDB_UInt2 GetFirstBlobDescOffset();
    FileDir_FileNo m_shortColumnFileNo;

    tgg91_FileVersion m_newPersistentFileVersion;
    SAPDB_UInt2       m_newPersistentBlobColCount;
    SAPDB_Int4        m_lobPageCountDelta;

    FileDir_ITable* AsTable();
};

#endif // FILEDIR_TABLEENTRY_HPP
