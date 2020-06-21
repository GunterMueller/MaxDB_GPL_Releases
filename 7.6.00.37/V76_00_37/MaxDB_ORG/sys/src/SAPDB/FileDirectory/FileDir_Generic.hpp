/*****************************************************************************/
/*!

  @file         FileDir_Generic.hpp
  @author       MartinKi
  @ingroup      FileDirectory_Files

  @brief        Header file for interface and smart pointer class
                FileDir_Generic.

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

#ifndef FILEDIR_GENERIC_HPP
#define FILEDIR_GENERIC_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "FileDirectory/FileDir_Types.hpp"

#include "DataAccess/Data_FileTypes.hpp"
// for Data_PageNo:
#include "DataAccess/Data_Types.hpp"

// for RTE_TaskId:
#include "RunTime/RTE_Types.hpp"

// for tgg91_TransNo:
#include "ggg91.h"

#include "Converter/Converter_Version.hpp"

class FileDir_BaseEntry;
class SAPDBMem_IRawAllocator;
struct tgg00_FileId;

/*!
 * @interface FileDir_Generic
 * @ingroup FileDirectory_Classes
 * @author MartinKi
 *
 * @brief SmartPointer class also serving as an interface class to all
 *        FileDirectory entries.
 */
class FileDir_Generic
{
    friend class FileDir_IteratorObj;
    friend class FileDir_Directory;
    friend class FileDir_SharedDirectory;
    friend class FileDir_GenericDirectory;
    friend class FileDir_Test;
    friend class FileDir_PointerCache;

public:
    /*!
     * @brief The file directory entry the pointer is currently
     *        pointing to. Do not use!
     *
     * Should be protected, but needs to be public for the
     * Assign()-method template to work. Do not use!
     *
     * @warning Not to be used directly!
     */
    FileDir_BaseEntry* fileEntry;

    /*!
     * @brief Returns true if this pointer is currently used as a
     * secondary pointer.
     */
    bool IsSecondaryPointer() const;
public:
    /*!
     * @brief Returns the file id of this entry.
     */
    FileDir_FileNo GetFileNo() const;

    /*!
     * @brief Returns the file type of this entry.
     */
    Data_FileType GetFileType() const;

     /*!
      * @brief Returns the file state of this entry.
      *
      * @return file state of entry.
      */
    Data_FileState GetFileState() const;

    /*!
     * @brief Set the file state of this entry.
     *
     * @param newFileState [in] the new file state of this entry
     */
    void SetFileState(const Data_FileState newFileState);

    /*!
     * @brief Mark this entry as deleted.
     *
     * @param transNo [in] transaction number identifying transaction
     *                     deleting the file
     */
    FileDir_ReturnCode MarkFileDeleted(const tgg91_TransNo& transNo);

    /*!
     * @brief Removes the deleted mark from this entry and sets its
     * state to Data_FileOk.
     *
     * @copydoc FileDir_BaseEntry::RemoveFileDeletedMark
     */
    FileDir_ReturnCode RemoveFileDeletedMark();

    /*!
     * @brief Returns the delete stamp of this entry.
     *
     * @copydoc FileDir_BaseEntry::GetDeleteStamp
     */
    void GetDeleteStamp(tgg91_TransNo& deleteStamp) const;

    /*!
     * @brief Returns the root page of this entry.
     */
    Data_PageNo GetRootPage() const;

    /*!
     * @brief Sets the root page of this entry.
     *
     * @copydoc FileDir_BaseEntry::SetRootPage
     */
    void SetRootPage(
        const RTE_TaskId taskId,
        const Data_PageNo& rootPage);

    /*!
     * @brief Sets the leaf page count used on disk for this entry's data.
     *
     * @copydoc FileDir_BaseEntry::SetLeafPageCount
     */
    void SetLeafPageCount(
        const SAPDB_UInt4 pageCount,
        const SAPDB_Bool  setInitialized = true);

    /*!
     * @brief Sets the index (non-leaf) page count used on disk for
     * this entry's data.
     *
     * @copydoc FileDir_BaseEntry::SetIndexPageCount
     */
    void SetIndexPageCount(const SAPDB_UInt4 pageCount);

    /*!
     * @brief Sets the entry count of this entry.
     *
     * @copydoc FileDir_BaseEntry::SetEntryCount
     */
    void SetEntryCount(const SAPDB_UInt8 entryCount);

    /*!
     * @brief Initializes the statistics counters of this entry.
     *
     * @copydoc FileDir_BaseEntry::InitializeBasicCounters
     */
    void InitializeBasicCounters(
        const RTE_TaskId  taskId,
        const SAPDB_UInt4 leafPageCount,
        const SAPDB_UInt4 indexPageCount,
        const SAPDB_UInt8 entryCount);

    /*!
     * @brief Returns true if the page and entry counter values are
     * initialised and can be used, false otherwise.
     *
     * @copydoc FileDir_BaseEntry::AreCountersInitialized
     */
    SAPDB_Bool AreCountersInitialized() const;

    /*!
     * @brief Returns the leaf page count used on disk for this entry's data.
     *
     * @copydoc FileDir_BaseEntry::GetLeafPageCount
     */
    SAPDB_UInt4 GetLeafPageCount(const RTE_TaskId taskId) const;

    /*!
     * @brief Returns the index (non-leaf) page count used on disk
     *        for this entry's data.
     *
     * @copydoc FileDir_BaseEntry::SetIndexPageCount
     */
    SAPDB_UInt4 GetIndexPageCount(const RTE_TaskId taskId) const;

    /*!
     * @brief Returns the entry count of this entry.
     *
     * @copydoc FileDir_BaseEntry::GetEntryCount
     */
    SAPDB_UInt8 GetEntryCount(const RTE_TaskId taskId) const;

    /*!
     * @brief Returns true if the entry is already marked for
     *        deletion and no write operations are allowed on its
     *        content.
     *
     * @copydoc FileDir_BaseEntry::IsReadOnly
     */
    SAPDB_Bool IsReadOnly() const;

    /*!
     * @brief Sets the temporary comment for this entry.
     *
     * @copydoc FileDir_BaseEntry::SetComment
     */
    void SetComment(const char* comment);

    /*!
     * @brief Sets the temporary comment for this entry.
     *
     * @copydoc FileDir_BaseEntry::SetComment
     */
    const char* GetComment() const;

    /*!
     * @brief Adds delta pages to count of tree-leaves of
     *        this entry.
     *
     * @copydoc FileDir_BaseEntry::ModifyLeafPageCount
     */
    void ModifyLeafPageCount(
        const RTE_TaskId        taskId,
        const Converter_Version converterVersion,
        const SAPDB_Int4        delta);

    /*!
     * @brief Adds delta pages to count of index (non-tree) leaves of
     *        this entry.
     *
     * @copydoc FileDir_BaseEntry::ModifyIndexPageCount
     */
    void ModifyIndexPageCount(
        const RTE_TaskId        taskId,
        const Converter_Version converterVersion,
        const SAPDB_Int4        delta);

    /*!
     * @brief Adds delta entries to entry count of this entry.
     *
     * @copydoc FileDir_BaseEntry::ModifyEntryCount
     */
    void ModifyEntryCount(
        const RTE_TaskId        taskId,
        const Converter_Version converterVersion,
        const SAPDB_Int8        delta);

    /*!
     * @brief Returns true if this entry is persistent.
     *
     * @copydoc FileDir_BaseEntry::IsPersistent
     */
    SAPDB_Bool IsPersistent() const;

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

    /*!
     * @brief Increases the cache-hit counter by one.
     *
     * @copydoc FileDir_BaseEntry::IncrementCacheHits
     */
    void IncrementCacheHits();

    /*!
     * @brief Increases the cache-miss counter by one.
     *
     * @copydoc FileDir_BaseEntry::IncrementCacheMisses
     */
    void IncrementCacheMisses();

    /*!
     * @brief Returns the cache hitrate based on the cache-hit/-miss
     *        counters in percent.
     *
     * @copydoc FileDir_BaseEntry::GetCacheHitrate
     */
    SAPDB_Real8 GetCacheHitrate() const;

    /*!
     * @name Cached lock information
     */
    /*@{*/
    /*!
     * @brief Sets the current cached lock state for OMS. A value of 0
     * represents the state 'undefined'.
     */
    void SetLockState(const SAPDB_UInt2 lockState);

    /*!
     * @brief Returns the current cached lock state for OMS. A value
     * of 0 represents the state 'undefined'.
     */
    SAPDB_UInt2 GetLockState() const;

    /*!
     * @brief Sets the current cached schema hash key for OMS. A value of 0
     * represents the state 'undefined'.
     */
    void SetSchemaHashkey(const SAPDB_UInt2 SchemaHashkey);

    /*!
     * @brief Returns the current cached schema hash key for OMS. A value
     * of 0 represents the state 'undefined'.
     */
    SAPDB_UInt2 GetSchemaHashkey() const;

    /*@}*/

    /*!
     * @name File attributes
     */
    /*@{*/

    /*!
     * @copydoc FileDir_Entry::SetPagesClustered
     */
    void SetPagesClustered(
        const RTE_TaskId taskId,
        const SAPDB_Bool pagesClustered);

    /*!
     * @copydoc FileDir_Entry::ArePagesClustered
     */
    SAPDB_Bool ArePagesClustered() const;

    /*@}*/

    /*!
     * @brief Default constructor.
     */
    FileDir_Generic();

    /*!
     * @brief Returns true if pointer is assigned to a file directory entry.
     */
    SAPDB_Bool IsAssigned() const;

/*===========================================================================*/

    /*!
     * @brief Constructor.
     *
     * @param fe [in] Pointer to FileDir_BaseEntry or subclass.
     */
    FileDir_Generic(FileDir_BaseEntry* fe);

    /*!
     * @brief Copy-constructor.
     */
    FileDir_Generic(const FileDir_Generic& fe);

    /*!
     * @brief Destructor.
     */
    virtual ~FileDir_Generic();

    /*!
     * @brief Detaches smart pointer from entry it is current pointing
     *        to.
     */
    void Detach();

    /*!
     * @brief Assigns pointer to file directory entry described by
     *        entryPtr.
     *
     * @param entryPtr [in] pointer to file directory entry
     */
    void AssignEntry( const FileDir_BaseEntry* entryPtr );

    /*!
     * @brief Assigns pointer to file directory entry described by
     *        entryPtr.
     *
     * You can use Assign() to up- and downcast entry
     * types. Assignmnents that are not typesafe result in a compile
     * error.
     *
     * @param Ptr [in] reference to file directory entry smart pointer
     */
    template <class T>
    void Assign(T& Ptr);
protected:

    /*!
     * @brief Returns a pointer to the file directory entry this smart
     *        pointer is pointing to.
     */
    FileDir_BaseEntry* GetPointer() const;

     /*!
      * @brief Frees all resources for this entry.
      *
      * All allocated memory for this entry (except for the object
      * itself) will be freed and the persistent data marked for
      * deletion.
      */
    void Remove(const RTE_TaskId taskId);

    /*!
     * Assignment operator.
     */
    FileDir_Generic operator=(FileDir_Generic const& fip);

    /*!
     * Update persistent entry parts after a savepoint has finished.
     */
    void UpdateAfterSavepoint(
        const RTE_TaskId taskId,
        const Converter_Version& converterVersion);

    /*!
     * @return the converter version of the last DDL statement on the entry,
     *         used by FileDir_Test
     */
    Converter_Version GetEntryConverterVersion();

private:
    SAPDB_Bool              m_isPrimaryCounter;
    SAPDB_Bool              m_selfDestruct;
    FileDir_Generic*        m_primaryCounter;
    SAPDB_UInt              m_secondaryRefCounts;
    SAPDBMem_IRawAllocator* m_allocator;
    FileDir_Generic(SAPDB_Bool isPrimaryCounter);

    SAPDB_UInt2             m_lockState;
    SAPDB_UInt2             m_schemaHashkey;

    void IncRefCount();
    /*!
     * @brief Decreases the reference count of the file entry and
     *        checks for a pending lazy delete if necessary.
     */
    void DecRefCount();
    void IncreaseSecondaryRefCounter();
    void DecreaseSecondaryRefCounter();
    void SetAllocator(SAPDBMem_IRawAllocator& allocator);
    void SetSelfDestruct();

protected:
    /*!
     * @brief Makes this pointer a secondary pointer to the entry p is
     * assigned to.
     */
    void AssignSecondary(FileDir_Generic& p);

    /*!
     * @brief Makes the to pointer point to the entry from is pointing
     * to and detach from pointer from the entry.
     *
     * Both pointers must be primary pointers. The transfer of the
     * reference works without having to change the reference count of
     * the entry.
     */
    static void TransferReference(
        FileDir_Generic& to,
        FileDir_Generic& from);
};

/*************************************************************************/

inline SAPDB_Bool FileDir_Generic::IsAssigned() const
{
    return fileEntry != 0;
}

/*************************************************************************/

inline bool FileDir_Generic::IsSecondaryPointer() const
{
    return ! m_isPrimaryCounter;
}

/*************************************************************************/

template <class T>
void FileDir_Generic::Assign(T& Ptr)
{
    /*
     * the following statement makes sure that down- and upcasts
     * via Assign() are typesafe; if you encounter a compile error
     * this probably means that you tried to Assign() a smart
     * pointer to unrelated entry types
     */
    T* test = static_cast<T*>(this); // check previous comment if you get a compile error here
    if ( Ptr.IsSecondaryPointer() ) {
        AssignSecondary( Ptr );
    } else {
        AssignEntry( Ptr.fileEntry );
    }
}

/*************************************************************************/

inline FileDir_Generic::FileDir_Generic()
    : fileEntry(0),
      m_isPrimaryCounter(true),
      m_selfDestruct(false),
      m_primaryCounter(0),
      m_secondaryRefCounts(0),
      m_allocator(0),
      m_lockState(0),
      m_schemaHashkey()
{}

/*************************************************************************/

#endif // FILEDIR_GENERIC_HPP
