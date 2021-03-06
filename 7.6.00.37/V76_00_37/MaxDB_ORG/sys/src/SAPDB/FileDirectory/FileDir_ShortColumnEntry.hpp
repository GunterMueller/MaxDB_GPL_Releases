/*****************************************************************************/
/*!

  @file         FileDir_ShortColumnEntry.hpp
  @author       MartinKi
  @ingroup      FileDirectory_Files

  @brief        Header file for class FileDir_ShortColumnEntry.

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

#ifndef FILEDIR_SHORTCOLUMNENTRY_HPP
#define FILEDIR_SHORTCOLUMNENTRY_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "FileDirectory/FileDir_Entry.hpp"
#include "FileDirectory/FileDir_IShortColumn.hpp"
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
 * @class FileDir_ShortColumnEntry
 * @ingroup FileDirectory_Classes
 * @author MartinKi
 * @brief A FileDirectory entry representing an index.
 */
class FileDir_ShortColumnEntry : public FileDir_Entry,
                                 public FileDir_IShortColumn
{
    friend class FileDir_SharedDirectory;
    friend class FileDir_Directory;

    struct PersistentShortColumnData : FileDir_Entry::PersistentData
    {
        FileDir_FileNo parentFileNo;
        SAPDB_Int2     fileVersion;
        SAPDB_Int2     filler;
        SAPDB_Int4     fileTypeSet;
    };

    PersistentShortColumnData* PersistentShortColumnPointer() const {
        return static_cast<PersistentShortColumnData*>(
            FileDir_Entry::PersistentPointer());
    }

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
    FileDir_ShortColumnEntry(
        const RTE_TaskId        taskId,
        SAPDBMem_IRawAllocator& allocator,
        FileDir_Entry&          persistentEntry,
        FileDir_EntryLayouts    currentPersistentLayout,
        SAPDB_Byte*&            curPersistentPos);

public:
    /*!
     * @brief Constructor for an short column entry.
     */
    FileDir_ShortColumnEntry(
        const RTE_TaskId         taskId,
        SAPDBMem_IRawAllocator&  allocator,
        const FileDir_FileNo&    fileNo,
        const Converter_Version& converterVersion,
        const FileDir_FileNo&    parentFileNo,
        const Data_PageNo        rootPage,
        const tgg00_FiletypeSet& fileTypeSet,
        const tgg91_FileVersion& fileVersion);

    /// Recreate from persistent data.
    FileDir_ShortColumnEntry(
        SAPDBMem_IRawAllocator& allocator,
        FileDir_Entry& persistentEntry);

    /// Destructor.
    ~FileDir_ShortColumnEntry();

    Data_FileType GetFileType() const;

    /*!
     * @brief Returns the file no of the table for this short column file.
     */
    FileDir_FileNo GetParentFileNo() const;

    /*!
     * @brief Assign this index to a different table.
     *
     * @warning The caller has to make sure that the short column file
     * is removed from the previous table if necessary.
     */
    void Reassign(
        const RTE_TaskId      taskId,
        const FileDir_FileNo& tableFileNo);

    SAPDB_Bool FillDumpEntry(
        const RTE_TaskId   taskId,
        SAPDBFields_Field& dumpBuffer,
        SAPDB_Int&         startPos);

    FileDir_ReturnCode Initialize();

    /*!
     * @brief Fills fileInfo for this short column file.
     */
    void FillFileInfo(tbd_fileinfo& fileInfo) const;

    /*!
     * @brief Sets the file info for this short column file.
     */
    void SetFileInfo(const RTE_TaskId taskId, const tbd_fileinfo& fileInfo);

    /*!
     * @brief Removes all information about the parent from this short
     * column file. Does not actually unregister this short column
     * file with the parent.
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

    FileDir_IShortColumn* AsShortColumn();

    FileDir_FileNo m_newParentFileNo;

    friend class FileDir_Directory;
};

#endif // FILEDIR_SHORTCOLUMNENTRY_HPP
