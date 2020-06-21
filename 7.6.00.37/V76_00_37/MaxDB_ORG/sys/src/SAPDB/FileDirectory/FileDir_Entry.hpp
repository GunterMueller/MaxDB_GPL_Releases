/*****************************************************************************/
/*!

  @file         FileDir_Entry.hpp
  @author       MartinKi
  @ingroup      FileDirectory_Files

  @brief        Header file for class FileDir_Entry.

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

#ifndef FILEDIR_ENTRY_HPP
#define FILEDIR_ENTRY_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "FileDirectory/FileDir_SharedDirectory.hpp"
#include "FileDirectory/FileDir_BaseEntry.hpp"
#include "FileDirectory/FileDir_Page.hpp"
#include "FileDirectory/FileDir_Common.hpp"
#include "FileDirectory/FileDir_Types.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"
#include "FileDirectory/FileDir_Exception.hpp"
#include "FileDirectory/FileDir_Messages.hpp"

// for Data_PageNo:
#include "DataAccess/Data_Types.hpp"
#include "DataAccess/Data_FileTypes.hpp"

#include "Converter/Converter_Version.hpp"

#include "RunTime/System/RTESys_AtomicOperation.hpp"
#include "RunTime/RTE_Types.hpp"

// tgg91_TransNo:
#include "ggg91.h"

#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
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

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/
class SAPDBFields_Field;

/**************************************************************************/

class SAPDBMem_IRawAllocator;

/*!
 * @class FileDir_Entry
 * @ingroup FileDirectory_Classes
 * @brief Base class for all FileDirectory entries.
 *
 * @copydoc FileDir_BaseEntry
 */
class FileDir_Entry : public FileDir_BaseEntry
{
    friend class FileDir_OmsKeyEntry; // FIXME
    friend class FileDir_GenericDirectory;
    friend class FileDir_SharedDirectory;
    friend class FileDir_Directory;
    friend class FileDir_Test;  // because of GetEntryConverterVersion()

#if defined(_WIN32) && !defined(_WIN64)
#  if _MSC_VER <= 1200
    // VC6.0 does not allow subclasses to inherit from protected data
    // structures, so use 'public:'
public:
#  else
protected:
#  endif
#else
protected:
#endif

    struct EntryHeader;
    friend struct EntryHeader;     ///< Entry header.

    struct PersistentData;
    friend struct PersistentData;  ///< Persistent data.

/**************************************************************************/

    /// Typedef for RecordHeader.
    typedef FileDir_Page::RecordHeader RecordHeader;

    /*!
     * @brief The header in front of each persistent record on the page.
     */
    struct EntryHeader
    {
        RecordHeader recordHeader;      ///< Record header of entry. (Size: 4 bytes)
        SAPDB_Bool   hasBackref;        ///< True if record contains a
                                        ///  back reference.
        SAPDB_Bool   filler;            ///< filler.
        SAPDB_Int2   filler1;           ///< filler1.
        SAPDB_UInt4  converterVersion;  ///< Converter version record was
                                        ///  created in.
    }; // size: 12 bytes

    /*!
     * @brief Struct describing a back reference to an earlier entry version.
     */
    struct Backref
    {
        SAPDB_Int4 backrefPNo;        ///< page number of backward reference
        SAPDB_Int2 backrefEntryLocation;  ///< record number of
                                          ///referenced record
                                      ///  on page backrefPNo;
        SAPDB_Int2 filler;
    };

    /*!
     * @brief Data structure that every FileDirectory
     *        entry with persistent data uses.
     *
     * This data structure is the first part of every persistent data
     * record on a FileDir_Page. It is used during restart to determine
     * the FileEntry-type and the FileNo of the record.
     *
     * All subclasses of FileDir_Entry that have additional
     * persistent data entries should subclass this struct.
     */
    struct PersistentData
    {
        EntryHeader    header;      ///< Header (size: 12 bytes)
        SAPDB_UInt1    fileType;    ///< File type.
        SAPDB_UInt1    fileState;   ///< File state of this entry.
        SAPDB_UInt1    fileAttributes; ///< Attributes of this entry.
        SAPDB_Int1     pdFiller;    ///< Filler.
        FileDir_FileNo fileNo;      ///< File no.
        SAPDB_UInt4    rootPage;    ///< Root page of this entry.
        SAPDB_Int4     pdFiller1;   ///< Filler.
        EntryCount     entryCount;  ///< Number of entries.
        PageCount      leafPageCount; ///< Number of leaf pages of this entry.
        PageCount      indexPageCount; ///< Number of index pages of this entry.
    };

    /*!
     * @brief Marks this entry as valid on the persistent data
     *        page. Does not care about locking.
     *
     * @warning It is the caller's responsibility to lock and unlock
     *          the data page for update.
     */
    void SetValid();

    /*!
     * @brief Marks this entry as invalid on the persistent data
     *        page. Does not care about locking.
     *
     * @warning It is the caller's responsibility to lock and unlock
     *          the data page for update.
     */
    void SetInvalid();

    void SetLazyDelete(const RTE_TaskId taskId);

    SAPDB_Bool IsLazyDeleteSet();

    /// Constructor to be used by subclasses of this class.
    FileDir_Entry(
        const RTE_TaskId         taskId,
        SAPDBMem_IRawAllocator&  allocator,
        const SAPDB_UInt2        requiredSize,
        const FileDir_FileNo     fileNo,
        const Converter_Version& converterVersion,
        const Data_PageNo&       rootPageNo);

    /*!
     * @brief Constructor to be used during file directory migrations
     *
     * This constructor will migrate the persistent part that is
     * covered by this entry to a new layout if necessary. It will
     * allocate a new entry where the migrated part will be
     * stored. Once the entry's part covered by this constructor has
     * been migrated <em>curPersistentPos</em> will point behind that
     * part in the original persistent entry data, so that a
     * subclass's constructor can convert its part of the entry based
     * on that position.
     *
     * If a constructor is designed to be subclassed, it will keep the
     * new entry locked and the unmigrated entry (persistentEntry)
     * will still be valid. It is the subclass's constructor's
     * responsibility to:
     *  - mark the new entry as valid
     *  - invalidate the old entry
     *  - unlock the file directory data page
     *
     * @param taskId [in] id of current task
     * @param allocator [in] allocator that should be used for this
     *                       entry; should be multi-thread safe
     * @param persistentEntry [in/out] pointer to the persistent entry
     *                                 that should be migrated
     * @param requiredSize [in] amount of space that should be
     *                          reserved on a file directory page
     * @param currentPersistentLayout [in] layout version of
     *              persistentEntry, this determines which migration steps
     *              will be taken
     * @param curPersistentPos [in/out] points to the memory position at
     *              which the current entry's data starts
     */
    FileDir_Entry(
        const RTE_TaskId        taskId,
        SAPDBMem_IRawAllocator& allocator,
        FileDir_Entry&          persistentEntry,
        const SAPDB_UInt2       requiredSize,
        FileDir_EntryLayouts    currentPersistentLayout,
        SAPDB_Byte*&            curPersistentPos);

    /*!
     * @brief Returns the file type of this file directory entry
     * depending on the persistent layout version.
     *
     * This method is required during file directory recreation.
     */
    Data_FileType GetFileType(
        const FileDir_EntryLayouts    currentPersistentLayout) const;

    static SAPDB_UInt GetFirstByteOffset();

protected:

    /// The lock protecting the pointer to the persistent data
    /// m_persistentData.
    SAPDB_Int4 m_persistentLock;

    /// Perisistent lock version (FIXME: should be removed)
    SAPDB_UInt4 m_persistentLockVersion;

    /*!
     * @brief Constructs a file directory entry. Only transient
     *        structures will be initialized.
     */
    FileDir_Entry(SAPDBMem_IRawAllocator& allocator);

    /*!
     * @brief Copy constructor - only uses pointer to persistent
     *        data; all other values are initialized to 0.
     */
    FileDir_Entry(SAPDBMem_IRawAllocator& allocator, FileDir_Entry& pageEntry);

    /// Accumulated delta for leaf page count while it can't be written
    /// directly on page during savepoint.
    SAPDB_Int4 m_leafPageCountDelta;

    /// Accumulated delta for index page count while it can't be written
    /// directly on page during savepoint.
    SAPDB_Int4 m_indexPageCountDelta;

    /// Accumulated delta for entry count while it can't be written
    /// directly on page during savepoint.
    SAPDB_Int4 m_entryCountDelta;

    /// If the file state is changed during a savepoint the new state
    /// is temporarily stored here to avoid flushing it to disk one
    /// savepoint too early. This variable has to be kept in sync with
    /// PersistentPointer->fileState during normal operation!
    Data_FileState m_newPersistentState;
    /// This variable has to be kept in sync with
    /// PersistentPointer->fileState during normal operation!
    Data_PageNo m_newRootPage;

    // variables describing internal state of entry:

    /// True, if deltas have to be applied to various counters once
    /// savepoint has finished.
    bool m_needsUpdateAfterSavepoint;

    /// Pointer to data page handler the persistent part of this entry
    /// is residing on.
    FileDir_Page* m_dataPage;

    /// Pointer to persistent data of entry.
    PersistentData* m_persistentData;

    /*!
     * @brief Copy body of persistent data, i.e. contents of
     *        *m_persistentData <em>excluding</em> header, from src to dst.
     *
     * @param src   address of source persistent data block
     * @param dst   address to copy content of src to
     */
    void CopyBody(PersistentData* dst, PersistentData* src);

    /*!
     * @brief Set pointer to persistent data to a new value.
     *
     * newDataPage must already be locked. The old page should not be
     * locked. Upon exit the old page will be unlocked again, while
     * the newDataPage will still be locked.
     */
    void RechainPersistentPointer(
        const RTE_TaskId taskId,
        FileDir_Page*    newDataPage,
        PersistentData*  newAddress);

    /// Queue entry for update of persistent statistic counters.
    void AddToNeedUpdateList(const RTE_TaskId taskId);

    /// Returns true, if it is safe to write to the page, false if a
    /// delta counter has to be used.
    bool IsWriteToPageAllowed(
        const RTE_TaskId taskId,
        const Converter_Version converterVersion) const;

    /*!
     * Intended for subclasses that provide a functionality similar to
     * InitializeBasicCounters with additional counters.
     *
     * @copydoc InitializeBasicCounters
     *
     * @param bKeepPageLocked [in] do not unlock data page on exit;
     *        the caller will have to do it
     */
    void InternalInitializeBasicCounters(
        const RTE_TaskId  taskId,
        const SAPDB_UInt4 leafPageCount,
        const SAPDB_UInt4 indexPageCount,
        const SAPDB_UInt8 entryCount,
        const SAPDB_Bool  bKeepPageLocked);

public:  // FIXME: should be protected
    /*!
     * @brief Moves persistent data to another data page. No backref
     *        to the old location is set. The converter version and an
     *        already existing backref is preserved during the move.
     *
     * This method allows an implicit defragmentation of persistent
     * data pages by moving entries to other data pages. The space
     * freed by the move can be reused immediately once the move has
     * been completed.
     *
     * @warning This method has to have a context of a DDL statement,
     *          i.e. the table/index/whatever should be locked exclusively
     *          during the move.
     * @return
     *      - <em>true</em>  if move was successful
     *      - <em>false</em> if no memory was available for move
     */
    SAPDB_Bool MoveEntry();
protected:
    enum FileAttributes {
        fAttrNone           = 0x0,
        fAttrPagesClustered = 0x1
    };

    /*!
     * @brief Returns a pointer to the persistent data part of this entry.
     */
    PersistentData* PersistentPointer() const;

    /*!
     * @brief Locks the persistent data part of this entry.
     */
    void SetPersistentLock();

    /*!
     * @brief Unlocks the persistent data part of this entry.
     */
    void ClearPersistentLock();

    /*!
     * @brief Adds a back reference pointing to this entry to the entry
     *        whose persistent data starts at newRecordData.
     *
     * @param newRecordData [in] pointer to persistent data of entry
     *                           to which back reference is to be added
     */
    void AddBackrefToNewRecord(PersistentData* newRecordData) const;

    /*!
     * @brief Removes the back reference from this entry. If the entry
     *        does not have a back reference nothing will be
     *        done. This should only be used during the restart of the
     *        file directory as the data page is not locked.
     */
    void Restart_SetHasNoBackref();

    /*!
     * @brief Returns a pointer to the back reference of this entry.
     *
     * @return a pointer to the back reference of this entry or 0 if
     *         this entry does not have a back reference.
     */
    Backref* GetBackrefAddress() const;

    /*!
     * @brief Returns a pointer to the back reference of the entry
     *        whose persistent data starts at persistentData.
     *
     * @param persistentData [in] pointer to the persistent data of
     *                            entry whose back reference should
     *                            be retrieved
     *
     * @return a pointer to the back reference of the entry or 0 if
     *         the entry does not have a back reference.
     */
    Backref* GetBackrefAddress(PersistentData* persistentData) const;

    virtual void UpdateAfterSavepoint(
        const RTE_TaskId taskId,
        const Converter_Version& converterVersion);

    FileDir_ReturnCode MarkFileDeleted(const tgg91_TransNo& transNo);

    FileDir_ReturnCode RemoveFileDeletedMark();

    /*!
     * @brief Modifies a page counter. If a savepoint is running and
     * the modification belongs to the next converter version the
     * delta counter is used to store the modification.
     */
    void ModifyPageCounter(
        const RTE_TaskId        taskId,
        const Converter_Version converterVersion,
        PageCount&              pageCounter,
        SAPDB_Int4&             deltaCounter,
        const SAPDB_Int4        delta);

    /*!
     * Used to synchronises UpdateAfterSavepoint with
     * RechainPersistentPointer()and to protect delta counters
     */
//    mutable FileDir_RWRegionWrapper m_rechainAndDeltaLock;


/**************************************************************************/

    template<class Counter, class DeltaCounter>
    void ModifyCounter(
    const RTE_TaskId        taskId,
    const Converter_Version converterVersion,
    Counter&                counter,
    DeltaCounter&           deltaCounter,
    const DeltaCounter      delta);

private:
    /*!
     * @return the converter version of the last DDL statement on the entry
     */
    Converter_Version GetEntryConverterVersion();

    /*!
     * @brief Returns true if the entry is valid.
     */
    SAPDB_Bool IsValid() const;


    /*!
     * @brief Returns the delete stamp of the entry.
     */
    void GetDeleteStamp(tgg91_TransNo& deleteStamp) const;

    /*!
     * @brief Returns true if the entry has a back reference.
     */
    SAPDB_Bool GetHasBackref() const;

    /*!
     * @brief Returns the offset to the back reference from start of
     *        persistent data.
     */
    SAPDB_UInt2 GetBackrefOffset() const;

    /*!
     * @brief Returns the offset to the back reference from persistentData.
     *
     * @param persistentData [in] pointer to start of persistent data
     */
    SAPDB_UInt2 GetBackrefOffset(PersistentData* persistentData) const;

    SAPDB_Int GetBackrefPageNumber() const;

    SAPDB_Int GetBackrefEntryLocation() const;

    void SetPersistentDataAddress(FileDir_Page* dataPage, char* address);

    FileDir_Node* GetNodePtr() const;
public:
    FileDir_ReturnCode Initialize();

    /*!
     * @brief Returns the file type of this file directory entry.
     */
    Data_FileType GetFileType() const;

    /*!
     * @brief Returns the file id of this file directory entry.
     */
    FileDir_FileNo GetFileNo() const;

    /*!
     * @brief Returns the file state of this file directory entry.
     */
    Data_FileState GetFileState() const;

    /*!
     * @brief Sets the file state of this file directory entry.
     *        <em>File_Deleted cannot be set via this method. Use
     *        MarkFileDeleted() instead!</em>
     *
     * @param newFileState [in] new file state of this entry
     */
    void SetFileState(const Data_FileState newFileState);

    Data_PageNo GetRootPage() const;

    void SetRootPage(
        const RTE_TaskId taskId,
        const Data_PageNo& rootPage);

     /*!
      * @brief Sets the leaf page count used on disk for this entry's data.
      */
    void SetLeafPageCount(
        const SAPDB_UInt4 pageCount,
        const SAPDB_Bool  setInitialized = true);

     /*!
      * @brief Sets the index page count used on disk for this entry's data.
      */
    void SetIndexPageCount(const SAPDB_UInt4 pageCount);

     /*!
      * @brief Sets the entry count of this entry.
      */
    void SetEntryCount(const SAPDB_UInt8 entryCount);

    void InitializeBasicCounters(
        const RTE_TaskId  taskId,
        const SAPDB_UInt4 leafPageCount,
        const SAPDB_UInt4 indexPageCount,
        const SAPDB_UInt8 entryCount);

    SAPDB_Bool AreCountersInitialized() const;

    SAPDB_UInt4 GetLeafPageCount(const RTE_TaskId taskId) const;

    SAPDB_UInt4 GetIndexPageCount(const RTE_TaskId taskId) const;

    SAPDB_UInt8 GetEntryCount(const RTE_TaskId taskId) const;

    void ModifyLeafPageCount(
        const RTE_TaskId        taskId,
        const Converter_Version converterVersion,
        const SAPDB_Int4        delta);

    void ModifyIndexPageCount(
        const RTE_TaskId        taskId,
        const Converter_Version converterVersion,
        const SAPDB_Int4        delta);

    void ModifyEntryCount(
        const RTE_TaskId        taskId,
        const Converter_Version converterVersion,
        const SAPDB_Int8        delta);

    SAPDB_Bool IsPersistent() const;

    SAPDB_Bool IsReadOnly() const;

    SAPDB_Bool FillDumpEntry(
        const RTE_TaskId taskId,
        SAPDBFields_Field& dumpBuffer,
        SAPDB_Int& startPos);

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
     * @name File attributes
     */
    /*@{*/

    /*!
     * Sets whether pages should be stored in clusters.
     *
     * @param taskId [in] task id of running task
     * @param pagesClustered [in] pages should be clustered (if true),
     * pages need not be clustered (if false)
     */
    void SetPagesClustered(
        const RTE_TaskId taskId,
        const SAPDB_Bool pagesClustered);

    /*!
     * @return true, if pages should be clustered on disk.
     */
    SAPDB_Bool ArePagesClustered() const;

    /*@}*/

    /*!
     * @brief Removes the entry from the data page
     *
     * The entry is automatically invalidated.
     */
    void Remove(const RTE_TaskId taskId);

    /*!
     * @brief Returns the length of the entry record (including back
     *        references).
     */
    int GetLength() const;

    /*!
     * @brief Returns the page no of the page the persistent entry
     * data is located on.
     */
    Data_PageNo GetFileDirPageNo() const;

    /*!
     * @brief Checks reference count.
     */
    virtual ~FileDir_Entry();
};

/**************************************************************************/

inline void FileDir_Entry::SetPersistentLock()
{
    // This lock should only be set during the rechaining of
    // m_persistentData and this should only happen with an exclusive
    // lock on the entry, so when calling SetPersistentLock the lock
    // (in principle) should never be set!
    SAPDBERR_ASSERT_STATE( m_persistentLock == 0 );

    // FIXME: CompareAndExchange to make new value known to all
    // processors on an MP machine at once; should be replaced by an
    // assignment and a memory barrier once memory barriers are
    // available
    SAPDB_Int4 oldVal;
    ++m_persistentLockVersion;
    while ( !RTESys_CompareAndExchange( m_persistentLock, 0, 1, oldVal ) );
}

/**************************************************************************/

inline void FileDir_Entry::ClearPersistentLock()
{
    SAPDBERR_ASSERT_STATE( m_persistentLock == 1 );

    // FIXME: CompareAndExchange to make new value known to all
    // processors on an MP machine at once; should be replaced by an
    // assignment and a memory barrier once memory barriers are
    // available
    SAPDB_Int4 oldVal;
    ++m_persistentLockVersion;
    while ( !RTESys_CompareAndExchange( m_persistentLock, 1, 0, oldVal ) );
}

/**************************************************************************/

inline SAPDB_Bool FileDir_Entry::IsValid() const
{
    return m_persistentData->header.recordHeader.isValid;
}

/**************************************************************************/

inline SAPDB_Bool FileDir_Entry::GetHasBackref() const
{
    return m_persistentData->header.hasBackref;
}

/**************************************************************************/

inline int FileDir_Entry::GetLength() const
{
    return m_persistentData->header.recordHeader.length;
}

/**************************************************************************/

inline Data_FileType FileDir_Entry::GetFileType() const
{
    return static_cast<Data_FileType>( m_persistentData->fileType );
}

/**************************************************************************/

inline FileDir_Entry::PersistentData*
FileDir_Entry::PersistentPointer() const
{
    return m_persistentData;
}

/**************************************************************************/

template<class Counter, class DeltaCounter>
void FileDir_Entry::ModifyCounter(
    const RTE_TaskId        taskId,
    const Converter_Version converterVersion,
    Counter&                counter,
    DeltaCounter&           deltaCounter,
    const DeltaCounter      delta)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Entry::ModifyCounter",
                             FileDir_Trace, 6 );

    SAPDBERR_ASSERT_STATE( m_persistentData != 0 );

    if ( m_persistentData == 0 ) {
        FileDir_Exception errMsg(
            __CONTEXT__, FILEDIR_REFCOUNT_NOT_NULL_REMOVE,
            SAPDB_ToString(m_refCounter),
            FileDir_FileNoToString( m_persistentData->fileNo ) );
        RTE_Crash(errMsg);
    }

    FileDir_SharedDirectory& sharedFDir = FileDir_SharedDirectory::Instance();
    if ( ! this->IsWriteToPageAllowed(taskId, converterVersion) ) {
        SAPDB_Bool deltaCounterUsed = false;
        SAPDB_Bool useWriteLock = ! m_needsUpdateAfterSavepoint;
        if ( useWriteLock ) {
            sharedFDir.WriteLockNeedUpdateList( taskId );
        } else {
            sharedFDir.ReadLockNeedUpdateList( taskId );
        }
        // now that we have the lock, we know that the file directory
        // converter version can be trusted
        if ( ! this->IsWriteToPageAllowed(taskId, converterVersion) ) {
            RTESys_AtomicModify( deltaCounter, delta );
            deltaCounterUsed = true;
            if ( useWriteLock && (! m_needsUpdateAfterSavepoint) ) {
                m_needsUpdateAfterSavepoint = true;
                sharedFDir.AddToNeedUpdateList(
                    taskId,
                    PersistentPointer()->fileNo,
                    FileDir_SharedDirectory::euaUpdateEntry );
            }
        }
        if ( useWriteLock ) {
            sharedFDir.WriteUnlockNeedUpdateList( taskId );
        } else {
            sharedFDir.ReadUnlockNeedUpdateList( taskId );
        }

        if ( deltaCounterUsed ) {
            return;
        }
    }

    // this check is dirty and might skew the counter in a worst
    // case scenario
    m_dataPage->SetDirty();
    if ( ( delta < 0 )
         && ( counter < static_cast<Counter>( -delta ) ) ) {
        RTESys_AtomicWrite( counter, static_cast<Counter>( 0 ) );
    } else {
        RTESys_AtomicModify( counter, static_cast<Counter>( delta ) );
    }
}

/**************************************************************************/

#endif // FILEDIR_ENTRY_HPP
