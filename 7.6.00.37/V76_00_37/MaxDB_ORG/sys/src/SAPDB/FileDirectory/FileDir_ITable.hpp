/*****************************************************************************/
/*!

  @file         FileDir_ITable.hpp
  @author       MartinKi
  @ingroup      FileDirectory_Files

  @brief        Header file for interface class FileDir_ITable.

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

#ifndef FILEDIR_ITABLE_HPP
#define FILEDIR_ITABLE_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "FileDirectory/FileDir_FileNo.hpp"
#include "DataAccess/Data_FileTypes.hpp"

// for Data_PageNo:
#include "DataAccess/Data_Types.hpp"

#include "RunTime/RTE_Types.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"

class Converter_Version;
class Kernel_BlobDescription;
class Kernel_IndexDescription;
struct tbd_fileinfo;


/*!
 * @interface FileDir_ITable
 * @ingroup FileDirectory_Classes
 * @author MartinKi
 *
 * @brief Additional interface methods required for tables.
 */
class FileDir_ITable
{
public:
    /*!
     * @brief Destructor. Does nothing.
     */
    virtual ~FileDir_ITable() {};

    /*!
     * @copydoc FileDir_TableEntry::AddBlobColumn
     */
    virtual FileDir_ReturnCode AddBlobColumn(
        const Converter_Version& converterVersion,
        const SAPDB_UInt2 columnNumber) = 0;

    /*!
     * @copydoc FileDir_TableEntry::RemoveBlobColumn
     */
    virtual FileDir_ReturnCode RemoveBlobColumn(
        const Converter_Version& converterVersion,
        const SAPDB_UInt2 columnNumber) = 0;

    /*!
     * @copydoc FileDir_TableEntry::GetBlobColCount
     */
    virtual SAPDB_UInt2 GetBlobColCount() const = 0;

    /*!
     * @copydoc FileDir_TableEntry::GetMaxIndexId
     */
    virtual SAPDB_UInt2 GetMaxIndexId() const = 0;

    /*!
     * @brief Returns the file no of an index.
     *
     * @copydoc FileDir_TableEntry::GetIndexFileNo
     */
    virtual FileDir_FileNo GetIndexFileNo(SAPDB_UInt2 indexNo) const = 0;

    /*!
     * @copydoc FileDir_TableEntry::AddIndex
     */
    virtual FileDir_ReturnCode AddIndex(
        const FileDir_FileNo& indexFileNo,
        const SAPDB_UInt2     indexId) = 0;

    /*!
     * @copydoc FileDir_TableEntry::RemoveIndex
     */
    virtual FileDir_ReturnCode RemoveIndex(
        const FileDir_FileNo& indexFileNo) = 0;

    /*!
     * @copydoc FileDir_TableEntry::SetShortColumnFileNo
     */
    virtual void SetShortColumnFileNo(
        const FileDir_FileNo shortColumnFileNo) = 0;

    /*!
     * @copydoc FileDir_TableEntry::GetShortColumnFileNo
     */
    virtual void GetShortColumnFileNo(
        FileDir_FileNo& shortColumnFileNo) const = 0;

    /*!
     * @copydoc FileDir_TableEntry::GetShortColumnFileNo
     */
    virtual FileDir_FileNo GetShortColumnFileNo() const = 0;

    /*!
     * @copydoc FileDir_TableEntry::HasShortColumnFile
     */
    virtual SAPDB_Bool HasShortColumnFile() const = 0;

    /*!
     * @copydoc FileDir_TableEntry::IsLobPageCounterInitialized
     */
    virtual SAPDB_Bool IsLobPageCounterInitialized() const = 0;

    /*!
     * @copydoc FileDir_TableEntry::GetLobPageCount
     */
    virtual SAPDB_UInt8 GetLobPageCount() const = 0;

    /*!
     * @copydoc FileDir_TableEntry::ModifyLobPageCount
     */
    virtual void ModifyLobPageCount(
        const RTE_TaskId        taskId,
        const Converter_Version converterVersion,
        const SAPDB_Int4        delta) = 0;

    /*!
     * @copydoc FileDir_TableEntry::SetLobPageCount
     */
    virtual void SetLobPageCount(
        const SAPDB_UInt8 lobPageCount,
        const SAPDB_Bool  setInitialized = true) = 0;

    /*!
     * @brief Initializes the statistics counters with a given set of
     *        values.
     *
     * @copydoc FileDir_TableEntry::InitializeTableCounters
     */
    virtual void InitializeTableCounters(
        const RTE_TaskId  taskId,
        const SAPDB_UInt4 leafPageCount,
        const SAPDB_UInt4 indexPageCount,
        const SAPDB_UInt8 entryCount,
        const SAPDB_UInt8 lobPageCount ) = 0;

    /*!
     * @brief Fills fileInfo for this table.
     *
     * @copydoc FileDir_TableEntry::FillFileInfo
     */
    virtual void FillFileInfo(tbd_fileinfo& fileInfo) const = 0;

    /*!
     * @brief Sets the file info for this table.
     *
     * @copydoc FileDir_TableEntry::SetFileInfo
     */
    virtual void SetFileInfo(
        const RTE_TaskId taskId,
        const tbd_fileinfo& fileInfo) = 0;

    /*!
     * @name Table attributes
     */
    /*@{*/

    /*!
     * @copydoc FileDir_TableEntry::SetArchive
     */
    virtual void SetArchive(SAPDB_Bool isArchive) = 0;

    /*!
     * @copydoc FileDir_TableEntry::IsArchive
     */
    virtual SAPDB_Bool IsArchive() const = 0;

    /*!
     * @copydoc FileDir_TableEntry::SetRecordsPacked
     */
    virtual void SetRecordsPacked(
        const RTE_TaskId taskId,
        const SAPDB_Bool recordsPacked) = 0;

    /*!
     * @copydoc FileDir_TableEntry::AreRecordsPacked
     */
    virtual SAPDB_Bool AreRecordsPacked() const = 0;

    /*!
    * @copydoc FileDir_TableEntry::IsDynamic
    */
    virtual SAPDB_Bool IsDynamic() const = 0;
    /*@}*/
};

#endif // FILEDIR_ITABLE_HPP
