/*****************************************************************************/
/*!
 @file    FileDir_OmsEntry.hpp
 @author  MartinKi
 @ingroup FileDirectory_Files

 @brief   Header file for class FileDir_OmsEntry.

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

#ifndef FILEDIR_OMSENTRY_HPP
#define FILEDIR_OMSENTRY_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "FileDirectory/FileDir_Entry.hpp"
#include "FileDirectory/FileDir_IOms.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"

#include "DataAccess/Data_FileTypes.hpp"
#include "DataAccess/Data_Types.hpp"

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
class SAPDBFields_Field;

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/


/*!
 * @class FileDir_OmsEntry
 * @ingroup FileDirectory_Classes
 * @copydoc FileDir_Entry
 * @author MartinKi
 *
 * @brief A file directory entry representing an OMS object.
 */
class FileDir_OmsEntry : public FileDir_Entry,
                         public FileDir_IOms
{
    friend class FileDir_SharedDirectory;
    friend class FileDir_Directory;

protected:
    /// Persistent state info.
    struct PersistentOmsData : FileDir_Entry::PersistentData
    {
        SAPDB_UInt2 chainCount;       ///< chainCount
        SAPDB_UInt2 objBodySize;      ///< objBodySize
        SAPDB_UInt2 objPerPageCount;  ///< objPerPageCount
        SAPDB_UInt2 partitionCounter; ///< partitionCounter
        SAPDB_UInt8 maxPageSeqNo;     ///< maxPageSeqNo
    };

    /*!
     * @brief Modifies the page counter without caring about savepoint
     * synchronisation issues.
     */
    void ModifyPageCounterUnprotected(
        const RTE_TaskId        taskId,
        const Converter_Version converterVersion,
        PageCount&              pageCounter,
        SAPDB_Int4&             deltaCounter,
        const SAPDB_Int4        delta);

    /*!
     * @brief Increments a sequence counter taking savepoint
     * synchronisation issues in account.
     */
    void IncrementSequenceCounter(
        const RTE_TaskId        taskId,
        const Converter_Version converterVersion,
        SAPDB_UInt8&            sequenceCounter);

private:
    PersistentOmsData* PersistentOmsPointer() const {
        return static_cast<PersistentOmsData*>(
            FileDir_Entry::PersistentPointer());
    }

protected:

    /// Constructor to be used by subclasses of this class.
    FileDir_OmsEntry(
        const RTE_TaskId         taskId,
        SAPDBMem_IRawAllocator&  allocator,
        const SAPDB_UInt2        requiredSize,
        const FileDir_FileNo     fileNo,
        const Converter_Version& converterVersion,
        const Data_PageNo&       rootPageNo,
        const SAPDB_UInt2        objBodySize,
        const SAPDB_UInt2        objPerPageCount,
        const SAPDB_UInt2        chainCount,
        const SAPDB_UInt2        partitionCounter);

    /// to be used by subclasses to check partitionCounter
    SAPDB_UInt2 GetPartitionCounter() const;

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
    FileDir_OmsEntry(
        const RTE_TaskId        taskId,
        SAPDBMem_IRawAllocator& allocator,
        FileDir_Entry&          persistentEntry,
        const SAPDB_UInt2       requiredSize,
        FileDir_EntryLayouts    currentPersistentLayout,
        SAPDB_Byte*&            curPersistentPos);

public:
    /*!
     * @brief Constructs a file directory entry for an OMS object.
     *
     * @param taskId            [in] task id of calling task
     * @param allocator         [in] allocator to use
     * @param fileNo            [in] file no of this table entry
     * @param converterVersion  [in] converter version this entry is
     *                               created in
     * @param rootPageNo        [in] rootPageNo of the table belonging
     *                               to this entry
     * @param objBodySize       [in]
     * @param objPerPageCount   [in]
     * @param chainCount        [in]
     */
   FileDir_OmsEntry(const RTE_TaskId       taskId,
                    SAPDBMem_IRawAllocator& allocator,
                    const FileDir_FileNo     fileNo,
                    const Converter_Version& converterVersion,
                    const Data_PageNo&       rootPageNo,
                    const SAPDB_UInt2        objBodySize,
                    const SAPDB_UInt2        objPerPageCount,
                    const SAPDB_UInt2        chainCount);

    /*!
     * @brief Recreates table file directory entry from its persistent data.
     *
     * @param allocator         [in] allocator to use
     * @param persistentEntry [in] entry pointing to the persistent data
     *                             to be used.
     */
    FileDir_OmsEntry(
        SAPDBMem_IRawAllocator& allocator,
        FileDir_Entry& persistentEntry);

    /*!
     * @brief Destructor. Does nothing.
     */
    ~FileDir_OmsEntry() {};

    /*!
     * @brief Returns the file type of this entry (Data_OmsFile).
     */
    Data_FileType GetFileType() const;

    /*!
     * @brief FIXME: Returns.
     */
    SAPDB_UInt2 GetObjBodySize() const;

    /*!
     * @brief FIXME: Returns.
     */
    SAPDB_UInt4 GetObjPerPageCount() const;

    /*!
     * @brief FIXME: Returns.
     */
    SAPDB_UInt2 GetChainCount() const;

    /*!
     * @brief Modifies the free page counter by delta.
     */
    void ModifyFreePageCount(
        const RTE_TaskId        taskId,
        const Converter_Version converterVersion,
        const SAPDB_Int4        delta);

    /*!
     * @brief Returns the free page count.
     */
    SAPDB_UInt4 GetFreePageCount(const RTE_TaskId taskId) const;

    /*!
     * @brief Returns the current value of the page sequence counter.
     */
    SAPDB_UInt8 GetCurrentPageSequenceNo() const;

    /*!
     * @brief Increments the page sequence counter.
     */
    void IncrementPageSequenceNo(
        const RTE_TaskId        taskId,
        const Converter_Version converterVersion);

    /*!
     * @brief Cannot be be used.
     */
    void ModifyIndexPageCount(
        const RTE_TaskId        taskId,
        const Converter_Version converterVersion,
        const SAPDB_Int4        delta);

    /*!
     * @brief Cannot be be used.
     */
    SAPDB_UInt4 GetIndexPageCount(const RTE_TaskId taskId) const;

    SAPDB_Bool FillDumpEntry(
        const RTE_TaskId       taskId,
        SAPDBFields_Field&     dumpBuffer,
        SAPDB_Int&             startPos);

    FileDir_ReturnCode Initialize();

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

protected:
    /*!
     * @brief Applies deltas accumulated during savepoint to various counters.
     *
     * @param taskId           [in] task id of calling task
     * @param converterVersion [in] current converter version
     */
    void UpdateAfterSavepoint(
        const RTE_TaskId taskId,
        const Converter_Version& converterVersion);

    FileDir_IOms* AsOms();
};

/**************************************************************************/

inline SAPDB_UInt2 FileDir_OmsEntry::GetPartitionCounter() const
{
    return PersistentOmsPointer()->partitionCounter;
}

#endif // FILEDIR_OMSENTRY_HPP
