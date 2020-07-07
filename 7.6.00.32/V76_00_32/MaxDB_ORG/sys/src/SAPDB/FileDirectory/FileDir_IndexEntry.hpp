/*****************************************************************************/
/*!

  @file         FileDir_IndexEntry.hpp
  @author       MartinKi
  @ingroup      FileDirectory_Files

  @brief        Header file for class FileDir_IndexEntry.

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

#ifndef FILEDIR_INDEXENTRY_HPP
#define FILEDIR_INDEXENTRY_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "FileDirectory/FileDir_Entry.hpp"
#include "FileDirectory/FileDir_IIndex.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"
#include "DataAccess/Data_FileTypes.hpp"

#include "Container/Container_Vector.hpp"

#include "RunTime/RTE_Types.hpp"

#include "SAPDBCommon/SAPDB_Types.hpp"

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
 * @class FileDir_IndexEntry
 * @ingroup FileDirectory_Classes
 * @author MartinKi
 * @brief A FileDirectory entry representing an index.
 */
class FileDir_IndexEntry : public FileDir_Entry,
                           public FileDir_IIndex
{
    friend class FileDir_SharedDirectory;
    friend class FileDir_Directory;

protected:
    struct PersistentIndexData;
    friend struct PersistentIndexData;

    /*!
     * @brief Data structure used to store index data on disk.
     */
    struct PersistentIndexData : public FileDir_Entry::PersistentData
    {
        FileDir_FileNo parentFileNo;    ///< fileNo of table index is defined on
        SAPDB_UInt2    indexId;         ///< indexId
        SAPDB_UInt1    fileTypeSet;     ///< fileTypeSet of index
        SAPDB_UInt1    indexAttributes; ///< index attributes
        SAPDB_UInt4    usageCount;      ///< number of times index was used
    };

    PersistentIndexData* PersistentIndexPointer() const {
        return static_cast<PersistentIndexData*>(
            FileDir_Entry::PersistentPointer());
    }

protected:
    /*!
     * @brief Attributes of index file
     */
    enum IndexAttributes {
        iAttrNone          = 0,
        iAttrRecordsPacked = 1
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
     *              which the current entry's data starts
     */
    FileDir_IndexEntry(
        const RTE_TaskId        taskId,
        SAPDBMem_IRawAllocator& allocator,
        FileDir_Entry&          persistentEntry,
        FileDir_EntryLayouts    currentPersistentLayout,
        SAPDB_Byte*&            curPersistentPos);

public:
    /*!
     * @brief Constructor for an index entry.
     */
    FileDir_IndexEntry(
        const RTE_TaskId         taskId,
        SAPDBMem_IRawAllocator&  allocator,
        const FileDir_FileNo     fileNo,
        const Converter_Version& converterVersion,
        const FileDir_FileNo     parentFileNo,
        const SAPDB_UInt2        indexId,
        const Data_PageNo        rootPageNo,
        const tgg00_FiletypeSet& fileTypeSet,
        const tgg91_FileVersion& fileVersion);

    /// Recreate from persistent data.
    FileDir_IndexEntry(
        SAPDBMem_IRawAllocator& allocator,
        FileDir_Entry&          persistentEntry);

    /// Destructor.
    ~FileDir_IndexEntry();

    Data_FileType GetFileType() const;

    /*!
     * @brief Returns the file id of the table this index was defined on.
     */
    FileDir_FileNo GetParentFileNo() const;

    /*!
     * @brief Returns the id of this index as it is used by its parent.
     */
    SAPDB_UInt2 GetIndexId() const;

    /*!
     * @brief Returns the number of columns used for this index.
     */
// superseded and probably to be removed soon:
//    SAPDB_UInt2 GetColumnCount() const;

    /*!
     * @brief Assign this index to a different table.
     *
     * @warning The caller has to make sure that the index is removed
     *          from the previous table if necessary.
     */
    void Reassign(
        const RTE_TaskId      taskId,
        const FileDir_FileNo& tableFileNo);


    SAPDB_Bool FillDumpEntry(
        const RTE_TaskId   taskId,
        SAPDBFields_Field& dumpBuffer,
        SAPDB_Int&         startPos);

    /*!
     * @brief Initialize member (and heap) variables for this entry.
     */
    FileDir_ReturnCode Initialize();

    /*!
     * @brief Fills fileInfo for this index.
     */
    void FillFileInfo(tbd_fileinfo& fileInfo) const;

    /*!
     * @brief Sets the file info for this index.
     */
    void SetFileInfo(const RTE_TaskId taskId, const tbd_fileinfo& fileInfo);

    /*!
     * @brief Removes all information about the parent from this
     * info. Does not actually unregister this index with the parent.
     */
    void Detach(const RTE_TaskId taskId);

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
     * @name Index file attributes
     */
    /*@{*/

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
    /*@}*/

    /*!
     * @name Index statistics
     */
    /*@{*/

    /*!
     * @brief Increases the usage counter of this index.
     *
     * The usage counter is a measure of how often the optimizer has
     * decided to use this index.
     */
    void IncrementUsageCount(const RTE_TaskId taskId);

    /*!
     * @brief Returns the current usage count.
     */
    SAPDB_Int GetUsageCount() const;

    /*!
     * @brief Resets the usage counter of this index.
     */
    void ResetUsageCount(const RTE_TaskId taskId);

    /*@}*/
private:
    /*!
     * @brief Applies deltas accumulated during savepoint to various counters.
     *
     * @param taskId           [in] task id of calling task
     * @param converterVersion [in] current converter version
     */
    void UpdateAfterSavepoint(
        const RTE_TaskId taskId,
        const Converter_Version& converterVersion);

    FileDir_FileNo m_newParentFileNo;

    FileDir_IIndex* AsIndex();

    friend class FileDir_Directory;
};

#endif // FILEDIR_INDEXENTRY_HPP
