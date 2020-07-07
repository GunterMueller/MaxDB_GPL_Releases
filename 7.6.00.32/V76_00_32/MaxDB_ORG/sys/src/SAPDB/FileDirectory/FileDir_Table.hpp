/*****************************************************************************/
/*!
 @file    FileDir_Table.hpp
 @author  MartinKi
 @ingroup FileDirectory_Files

 @brief   Header file for smart point and interface class
          FileDir_Table.

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

#ifndef FILEDIR_TABLE_HPP
#define FILEDIR_TABLE_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "FileDirectory/FileDir_Generic.hpp"
#include "FileDirectory/FileDir_ITable.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"
#include "DataAccess/Data_FileTypes.hpp"

#include "RunTime/RTE_Types.hpp"

#include "ggg00.h" // tgg00_Surrogate

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/
/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/
/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/

class FileDir_TableEntry;

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/


/*!
 * @class FileDir_Table
 * @ingroup FileDirectory_Classes
 * @copydoc FileDir_Generic
 * @author MartinKi
 *
 * @brief SmartPointer class also serving as an interface class to all
 *        file directory table entries.
 */
class FileDir_Table : public FileDir_Generic,
                      public FileDir_ITable
{
public:
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
        const SAPDB_UInt2    indexId);

    /*!
     * @copydoc FileDir_TableEntry::RemoveIndex
     */
    FileDir_ReturnCode RemoveIndex(
        const FileDir_FileNo& indexFileNo);

    /*!
     * @name Table attributes
     */
    /*@{*/

    /*!
     * @copydoc FileDir_TableEntry::SetArchive
     */
    void SetArchive(SAPDB_Bool isArchive);

    /*!
     * @copydoc FileDir_TableEntry::IsArchive
     */
    SAPDB_Bool IsArchive() const;

    /*!
     * @copydoc FileDir_TableEntry::SetRecordsPacked
     */
    void SetRecordsPacked(
        const RTE_TaskId taskId,
        const SAPDB_Bool recordsPacked);

    /*!
     * @copydoc FileDir_TableEntry::AreRecordsPacked
     */
    SAPDB_Bool AreRecordsPacked() const;

    /*!
    * @copydoc FileDir_TableEntry::IsDynamic
    */
    SAPDB_Bool IsDynamic() const;

    /*@}*/

    /*!
     * @copydoc FileDir_TableEntry::SetShortColumnFileNo
     */
    void SetShortColumnFileNo(
        const FileDir_FileNo shortColumnFileNo);

    /*!
     * @copydoc FileDir_TableEntry::GetShortColumnFileNo
     */
    void GetShortColumnFileNo(
        FileDir_FileNo& shortColumnFileNo) const;

    /*!
     * @copydoc FileDir_TableEntry::GetShortColumnFileNo
     */
    FileDir_FileNo GetShortColumnFileNo() const;

    /*!
     * @copydoc FileDir_TableEntry::HasShortColumnFile
     */
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
     * @copydoc FileDir_TableEntry::InitializeTableCounters
     */
    void InitializeTableCounters(
        const RTE_TaskId  taskId,
        const SAPDB_UInt4 leafPageCount,
        const SAPDB_UInt4 indexPageCount,
        const SAPDB_UInt8 entryCount,
        const SAPDB_UInt8 lobPageCount);

    /*!
     * @copydoc FileDir_TableEntry::FillFileInfo
     */
    void FillFileInfo(tbd_fileinfo& fileInfo) const;

    /*!
     * @copydoc FileDir_TableEntry::SetFileInfo
     */
    void SetFileInfo(const RTE_TaskId taskId, const tbd_fileinfo& fileInfo);

    /*!
     * @copydoc FileDir_Generic::Assign
     */
    template <class T>
    void Assign(T& Ptr);

    /*!
     * @name SysKey generation
     */
    /*@{*/
    enum SysKeyRc {
        skOkay,
        skNotInitialized,
        skOutOfMemory,
        skSystemError
    };

    /*!
     * @brief Returns a new syskey for the table.
     *
     * Currently this method can be called for any table and no check
     * is made whether the table actually has a syskey defined.
     *
     * @param sysKey [out] will contain a new unique syskey on
     *                     success, undefined otherwise
     *
     * @return
     * - skOkay, on success
     * - skNotInitialized, if there is not yet an initial value set
     *   for the syskey, call InitAndGetNewSysKey in this case
     */
    SysKeyRc GetNewSysKey(tgg00_Surrogate& sysKey);

    /*!
     * @brief Initializes the syskey cache for the table and returns a
     *        new syskey.
     *
     * If the cache of the table is already initialized, a new syskey
     * will be returned but the cache will not be reinitialized.
     *
     * Currently this method can be called for any table and no check
     * is made whether the table actually has a syskey defined.
     *
     * @param sysKey [in/out] used as initial value for the syskey
     * counter; on success, it will contain a new, unique syskey
     *
     * @return
     * - skOkay, on success
     * - skOutOfMemory, if no memory could be allocated to cache the
     *   syskey information
    */
     SysKeyRc InitAndGetNewSysKey(tgg00_Surrogate& sysKey);
    /*@}*/

/**************************************************************************/
/*************************************************************************/

    /*!
     * @brief Default constructor.
     */
    FileDir_Table();

    /*!
     * @brief Constructor.
     *
     * @param fe [in] Pointer to FileDir_Table or subclass.
     */
    FileDir_Table(FileDir_TableEntry* fe);

    /*!
     * @brief Copy-constructor.
     */
    FileDir_Table(const FileDir_Table& fe);
};

/*************************************************************************/

template <class T>
void FileDir_Table::Assign(T& Ptr)
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

#endif // FILEDIR_TABLE_HPP
