/*****************************************************************************/
/*!
 @file    FileDir_OmsKeyEntry.hpp
 @author  MartinKi
 @ingroup FileDirectory_Files

 @brief   Header file for class FileDir_OmsKeyEntry.

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

#ifndef FILEDIR_OMSKEYENTRY_HPP
#define FILEDIR_OMSKEYENTRY_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "FileDirectory/FileDir_OmsEntry.hpp"
#include "FileDirectory/FileDir_IOmsKey.hpp"
#include "FileDirectory/FileDir_Types.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"

#include "DataAccess/Data_Types.hpp"
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
class SAPDBFields_Field;
class tgg91_TransNo;

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/


/*!
 * @class FileDir_OmsKeyEntry
 * @ingroup FileDirectory_Classes
 * @copydoc FileDir_OmsEntry
 * @author MartinKi
 *
 * @brief A file directory entry representing an OMS key object.
 */
class FileDir_OmsKeyEntry : public FileDir_OmsEntry,
                            public FileDir_IOmsKey
{
    friend class FileDir_SharedDirectory;
    friend class FileDir_Directory;

    // info variables
    struct PersistentOmsKeyData : FileDir_OmsEntry::PersistentOmsData
    {
        // we reserve space for only one file no here but when creating
        // the entry we actually might reserve more
        FileDir_FileNo keyPartitionFileNos[ 1 ];
    };

    PersistentOmsKeyData* PersistentOmsKeyPointer() const {
        return static_cast<PersistentOmsKeyData*>(
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
    FileDir_OmsKeyEntry(
        const RTE_TaskId        taskId,
        SAPDBMem_IRawAllocator& allocator,
        FileDir_Entry&          persistentEntry,
        FileDir_EntryLayouts    currentPersistentLayout,
        SAPDB_Byte*&            curPersistentPos);

public:
    /*!
     * @brief Constructs a file directory entry for an OMS key object.
     *
     * @param taskId            [in] task id of calling task
     * @param allocator         [in] allocator to use
     * @param fileNo            [in] file id of this table entry
     * @param converterVersion  [in] converter version this entry is
     *                               created in
     * @param rootPageNo        [in] rootPageNo of the table belonging
     *                               to this entry
     * @param objBodySize       [in]
     * @param objPerPageCount   [in]
     * @param chainCount        [in]
     * @param keyPartitionCount [in]
     */
   FileDir_OmsKeyEntry(const RTE_TaskId         taskId,
                       SAPDBMem_IRawAllocator& allocator,
                       const FileDir_FileNo     fileNo,
                       const Converter_Version& converterVersion,
                       const Data_PageNo&       rootPageNo,
                       const SAPDB_UInt2        objBodySize,
                       const SAPDB_UInt2        objPerPageCount,
                       const SAPDB_UInt2        chainCount,
                       const SAPDB_UInt2        keyPartitionCount);

    /*!
     * @brief Recreates table file directory entry from its persistent data.
     *
     * @param allocator         [in] allocator to use
     * @param persistentEntry [in] entry pointing to the persistent data
     *                             to be used.
     */
    FileDir_OmsKeyEntry(
        SAPDBMem_IRawAllocator& allocator,
        FileDir_Entry& persistentEntry);

    /*!
     * @brief Destructor. Does nothing.
     */
    ~FileDir_OmsKeyEntry() {};

    /*!
     * @brief Returns the file type of this entry (Data_OmsFile).
     */
    Data_FileType GetFileType() const;

    /*!
     * @brief Returns the number of key partitions for this entry
     *        (either 1 or 16).
     */
    SAPDB_UInt2 GetKeyPartitionCount() const;

    /*!
     * @brief Returns the file no of the partitionNo'th key partition.
     */
    FileDir_FileNo GetKeyPartition(const SAPDB_UInt2 partitionNo) const;

    /*!
     * @brief Adds a key partition to this entry.
     *
     * @returns
     *    - FileDir_Okay on success
     *    - FileDir_OutOfMemory if necessary memory could not be allocated
     */
    FileDir_ReturnCode AddKeyPartition(
        const FileDir_FileNo keyPartitionFileNo,
        const SAPDB_UInt2    partitionNo);

    /*!
     * @brief Removes a key partition from this entry.
     *
     * @returns
     *    - FileDir_Okay on success
     *    - FileDir_FileIdNotFound if keyPartitionFileNo was not found
     */
    FileDir_ReturnCode RemoveKeyPartition(
        const FileDir_FileNo keyPartitionFileNo);

    /*!
     * @brief Applies deltas accumulated during savepoint to various counters.
     *
     * @param taskId           [in] task id of calling task
     * @param converterVersion [in] current converter version
     */
    void UpdateAfterSavepoint(
        const RTE_TaskId taskId,
        const Converter_Version& converterVersion);

    /*!
     * @brief Prepares fileId for this file entry for use in bd
     *        procedures.
     *
     * @copydoc FileDir_BaseEntry::BuildFileId
     */
    void BuildFileId(tgg00_FileId& fileId) const;

    SAPDB_Bool FillDumpEntry(
        const RTE_TaskId       taskId,
        SAPDBFields_Field&     dumpBuffer,
        SAPDB_Int&             startPos);

    FileDir_ReturnCode Initialize();

private:
    FileDir_IOmsKey* AsOmsKey();

    friend class FileDir_Directory;
};

#endif // FILEDIR_OMSKEYENTRY_HPP
