/*****************************************************************************/
/*!

  @file         FileDir_TempEntry.hpp
  @author       MartinKi
  @ingroup      FileDirectory_Files

  @brief        Header file for class FileDir_TempEntry.

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

#ifndef FILEDIR_TEMPENTRY_HPP
#define FILEDIR_TEMPENTRY_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "FileDirectory/FileDir_BaseEntry.hpp"
#include "FileDirectory/FileDir_Common.hpp"
#include "FileDirectory/FileDir_Types.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"

#include "DataAccess/Data_FileTypes.hpp"
// for Data_PageNo:
#include "DataAccess/Data_Types.hpp"

// for Converter_Version:
#include "Converter/Converter_Version.hpp"

#include "RunTime/System/RTESys_AtomicOperation.hpp"
#include "RunTime/RTE_Types.hpp"

// tgg91_TransNo:
#include "ggg91.h"

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

class Kernel_Dump;
class SAPDBMem_IRawAllocator;
class SAPDBFields_Field;

/**************************************************************************/

/*!
 * @class FileDir_TempEntry
 * @ingroup FileDirectory_Classes
 * @brief Base class for all FileDirectory entries.
 *
 * @copydoc FileDir_BaseEntry
 */
class FileDir_TempEntry : public FileDir_BaseEntry
{
    friend class FileDir_Directory;
    friend class FileDir_Test;  // because of GetEntryConverterVersion()

protected:
    /*!
     * @brief Base data structure that every temporary FileDirectory
     *        entry uses.
     *
     */
    SAPDB_Bool  m_isValid;
    Data_FileType   m_fileType;    ///< File type.
    FileDir_FileNo  m_fileNo;      ///< File id.
    EntryCount      m_entryCount;  ///< Number of entries.
    PageCount       m_leafPageCount;   ///< Number of leaf pages used for this entry.
    PageCount       m_indexPageCount;   ///< Number of index pages used for this entry.
    Data_FileState  m_fileState;   ///< File state of this entry.
    Data_PageNo     m_rootPage;    ///< Root page of this entry.
    Converter_Version m_converterVersion; ///< Converter version entry
                                          ///was created/last
                                          ///changed(DDL) in

    SAPDB_Bool m_lazyDelete;

    /*!
     * @brief Constructs a file directory entry. Only transient
     *        structures will be initialized.
     */
    FileDir_TempEntry(
        SAPDBMem_IRawAllocator&  allocator,
        const FileDir_FileNo&    fileNo,
        const Converter_Version& converterVersion,
        const Data_PageNo&       rootPage,
        const Data_FileType      fileType,
        const Data_FileState     fileState,
        const EntryCount         entryCount,
        const PageCount          pageCount)
        : FileDir_BaseEntry(allocator),
          m_isValid(false),
          m_fileNo(fileNo),
          m_converterVersion(converterVersion),
          m_rootPage(rootPage),
          m_fileType(fileType),
          m_fileState(fileState),
          m_entryCount(entryCount),
          m_leafPageCount(pageCount),
          m_indexPageCount(pageCount),
          m_lazyDelete(false)
    {
        m_node.entry = this;
    }

    /*!
     * @brief Marks this entry as valid.
     */
    void SetValid();

    /*!
     * @brief Marks this entry as invalid.
     */
    void SetInvalid();

    void SetLazyDelete(const RTE_TaskId taskId);

    SAPDB_Bool IsLazyDeleteSet();

    /*!
     * @brief Returns the delete stamp of the entry.
     */
    void GetDeleteStamp(tgg91_TransNo& deleteStamp) const;

protected:
    FileDir_ReturnCode Initialize();
    FileDir_ReturnCode MarkFileDeleted(const tgg91_TransNo& transNo);
    FileDir_ReturnCode RemoveFileDeletedMark();

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
     * @brief Returns the length of the entry record (including back
     *        references).
     */
    int GetLength() const;

    void Remove(const RTE_TaskId taskId);

public:
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
        const RTE_TaskId   taskId,
        const Data_PageNo& rootPage);

    SAPDB_UInt4 GetLeafPageCount(const RTE_TaskId taskId) const;

    SAPDB_UInt4 GetIndexPageCount(const RTE_TaskId taskId) const;

    SAPDB_UInt8 GetEntryCount(const RTE_TaskId taskId) const;

     /*!
      * @brief Sets the page count used on disk for this entry's data.
      */
    void SetLeafPageCount(
        const SAPDB_UInt4 pageCount,
        const SAPDB_Bool  setInitialized = true);

     /*!
      * @brief Sets the page count used on disk for this entry's data.
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

    void UpdateAfterSavepoint(
        const RTE_TaskId         taskId,
        const Converter_Version& converterVersion)
    {}

    SAPDB_Bool IsPersistent() const;

    SAPDB_Bool IsReadOnly() const;

    SAPDB_Bool FillDumpEntry(
        const RTE_TaskId   taskId,
        SAPDBFields_Field& dumpBuffer,
        SAPDB_Int&         startPos);

    /*!
     * No-op for temp tables.
     */
    void SetPagesClustered(
        const RTE_TaskId taskId,
        const SAPDB_Bool pagesClustered);

    /*!
     * Always returns false.
     */
    SAPDB_Bool ArePagesClustered() const;

    /*!
     * @brief Checks reference count.
     */
    virtual ~FileDir_TempEntry();
};

/**************************************************************************/

inline SAPDB_Bool FileDir_TempEntry::IsValid() const
{
    return m_isValid;
}

/**************************************************************************/

inline Data_FileType FileDir_TempEntry::GetFileType() const
{
    return m_fileType;
}

/**************************************************************************/


#endif // FILEDIR_TEMPENTRY_HPP
