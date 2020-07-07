/*****************************************************************************/
/*!
  @file         SQLMan_TempFileCollection.hpp
  @author       MartinKi
  @ingroup

  @brief        Header file for class SQLMan_TempFileCollection.

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

#ifndef SQLMAN_TEMPFILECOLLECTION_HPP
#define SQLMAN_TEMPFILECOLLECTION_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/


#include "SQLManager/SQLMan_Types.hpp"
#include "DataAccess/Data_TempFileTypes.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"
#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp"
#include "Container/Container_Vector.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"

#include "ggg00.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

class SAPDBMem_IRawAllocator;
class Msg_List;
class SQLMan_Context;
class SQLMan_LogicalFileNameGenerator;
class Kernel_Dump;

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/
/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
 * @brief Return codes of class SQLMan_TempFileCollection.
 */
enum SQLMan_TempFileReturnCode
{
    SQLMan_TempFileOkay = 0,
    SQLMan_TempInvalidLogicalName,
    SQLMan_TempFileCreationFailed,
    SQLMan_TempFileDestructionFailed,
    SQLMan_DuplicateTempFile,
    SQLMan_TempFileNotFound,
    SQLMan_TempFileOutOfMemory
};


/*!
 * @class   SQLMan_TempFileCollection
 * @author  MartinKi
 *
 */
class SQLMan_TempFileCollection
{
public:
    typedef SAPDB_Int  FileLevel;    ///< Type for file level information.
    typedef SAPDB_Int  FileSubLevel; ///< Type for file sublevel information.
    typedef SAPDB_Int4 FileIndex;    ///< Type for file index information.

    /*!
     * @param alloc   [in] allocator to use
     */
    SQLMan_TempFileCollection(SAPDBMem_IRawAllocator& alloc);

    /*!
     * @brief destructor
     */
    ~SQLMan_TempFileCollection();

    /*!
     * Initialises all internal structures.
     *
     * @return
     *  - true, on success
     *  - false, on error
     */
    SAPDB_Bool Initialize();

    /*!
     * @param trans [in] trans context
     * @param logicalFileName   [in]
     * @param logicalFileId [in/out]
     * @param message [out] message list containing possible error messages
     */
    SQLMan_TempFileReturnCode CreateFileFromFileName(
        SQLMan_TransContext&  trans,
        SQLMan_Surrogate&     logicalFileName,
        SQLMan_FileId&        logicalFileId,
        Msg_List& message);

    /*!
     * adds an entry for logicalFileId with a preset file no of
     * actualFileNo; no file is created and the actual file does not have
     * to exist when this method is called
     *
     * @param trans [in] trans context
     * @param actualFileNo [in]
     * @param logicalFileNo [in]
     * @param message [out] message list containing possible error messages
     */
    SQLMan_TempFileReturnCode AddFileEntry(
        SQLMan_TransContext&    trans,
        const SQLMan_Surrogate& actualFileNo,
        const SQLMan_Surrogate& logicalFileNo,
        Msg_List&               message);

    /*!
     * removes entry for file described
     */
    SQLMan_TempFileReturnCode RemoveFileEntry(
        const Data_TempFileTypes::FileType fileType,
        const FileLevel                    level,
        const FileSubLevel                 subLevel,
        const FileIndex                    index,
        Msg_List&              message);

    /*!
     * @param fileType [in]
     * @param level   [in]
     * @param indexNo [in]
     * @param fileNo  [out]
     * @param message [out] message list containing possible error messages
     */
    SQLMan_TempFileReturnCode GetFile(
        const Data_TempFileTypes::FileType fileType,
        const FileLevel       level,
        const FileSubLevel    subLevel,
        const FileIndex       indexNo,
        FileDir_FileNo&       fileNo,
        Msg_List&             message);


    /*!
     * @param fileNo  [in]
     * @param fileType [out]
     * @param level   [out]
     * @param indexNo [out]
     * @param message [out] message list containing possible error messages
     */
    SQLMan_TempFileReturnCode GetFileInfo(
        const FileDir_FileNo&         fileNo,
        Data_TempFileTypes::FileType& fileType,
        FileLevel&                    level,
        FileSubLevel&                 subLevel,
        FileIndex&                    indexNo,
        Msg_List&         message);

    /*!
     * @brief Destroys a file identified by its fileNo. The logical
     *        file name is restored in fileNo.
     *
     * @param trans   [in/out]
     * @param fileNo  [in/out]
     * @param message [out] message list containing possible error messages
     */
    SQLMan_TempFileReturnCode DestroyFileByFileNo(
        SQLMan_TransContext&  trans,
        FileDir_FileNo&       fileNo,
        Msg_List& message);

    /*!
     * @param trans [in/out]
     * @param fileType   [in]
     * @param level      [in]
     * @param message    [out]
     */
    SQLMan_TempFileReturnCode DestroyAtLevel(
        SQLMan_TransContext&               trans,
        const Data_TempFileTypes::FileType fileType,
        const FileLevel                    level,
        const FileSubLevel                 subLevel,
        Msg_List&                          message);

    /*!
     * @param trans [in/out]
     * @param message    [out]
     */
    SQLMan_TempFileReturnCode DestroyAll(
        SQLMan_TransContext&  trans,
        Msg_List& message);

    /*!
     * @brief Dump contents to dump file.
     *
     * @param dump [in] the dump file to write the contents to
     */
    void Dump(Kernel_Dump& dump);

    /*!
     * @brief Creates a new logical fileNo for a given fileNo or
     * returns the associated logical fileNo if available.
     *
     * The association between fileNoToMap and logicalFileNo will be
     * automatically removed once the instance of logicalFileNo is
     * deleted.
     *
     * @warning If you use MapFile for a temp file type you cannot use
     * CreateFileFromFilename with logical fileNos you have generated
     * yourself as these and thos created by MapFile might collide.
     *
     * @param tempFileType [in] file type of logical file
     * @param fileNoToMap [in] if fileNoToMap is not yet in the
     *  mapper, a new unused logical fileNo will be created, otherwise
     *  the already existing logical fileNo will be returned
     * @param logicalFileNo [out] either the newly create logical
     *  fileNo or the stored fileNo associated with fileNoToMap
     * @param message [out] message list containing possible error messages
     *
     * @return
     *   - SQLMan_TempFileOkay on success
     *   - SQLMan_TempFileOutOfMemory if no memory was available to store
     *     the logical fileNo
     */
    SQLMan_TempFileReturnCode MapFile(
        const Data_TempFileTypes::FileType tempFileType,
        const FileDir_FileNo&              fileNoToMap,
        FileDir_FileNo&                    logicalFileNo,
        Msg_List&                          message);

    /*!
     * Same as variant with FileDir_FileNo, but puts the logical
     * fileNo at the correct position in logicalFileId.
     */
    SQLMan_TempFileReturnCode MapFile(
        const Data_TempFileTypes::FileType tempFileType,
        const FileDir_FileNo&              fileNoToMap,
        SQLMan_FileId&                     logicalFileId,
        Msg_List&                          message);

    /*!
     * @name Procedure/Recursion level
     */
    /*@{*/

    /*!
     * @brief Increments the current proc/recursion level.
     *
     * Almost all logical file names will be implicitly prefixed with
     * the current proc level during create, lookup and destroy
     * operations and therefore not collide with or return files
     * created in a different proc level.
     *
     * The list of file types that do not consider the current proc
     * level is maintained in method GetFileTypeProcLevel().
     *
     */
    void IncrementProcLevel();

    /*!
     * @brief Decrements the current proc/recursion level.
     *
     * In slowknl a check verifies that the collection contains no
     * files of the current or a higher proc level.
     *
     * @sa IncrementProcLevel
     */
    void DecrementProcLevel();

    /*!
     * @brief Returns the current proc/recursion level.
     *
     * @sa IncrementProcLevel
     */
    SAPDB_Int GetProcLevel() const;

    /*!
     * @brief Returns the current proc/recursion level.
     *
     * In slowknl a check verifies that the collection contains no
     * files of proc level that is higher than the new proc level
     *
     * If newProcLevel equals the current proc level, no checks are
     * performed and the method always returns true.
     *
     * @param newProcLevel [in] new proc level
     *
     * @return
     *  - true, on success
     *  - false, if there exist files of a higher proc level than
     *           newProcLevel in the collection; the new proc level
     *           is set nevertheless
     */
    SAPDB_Bool SetProcLevel(const SAPDB_Int newProcLevel);
    /*@}*/

private:
    // no copy-construction or assignment
    SQLMan_TempFileCollection(const SQLMan_TempFileCollection&);
    SQLMan_TempFileCollection& operator=(const SQLMan_TempFileCollection&);

    struct FileTypeInfo {
        Data_TempFileTypes::FileType m_fileType;
        SAPDB_Int         m_procLevel;
        FileLevel         m_level;
        FileSubLevel      m_subLevel;
        FileIndex         m_index;
        FileDir_FileNo    m_fileNo;

        FileTypeInfo()
            : m_fileType(Data_TempFileTypes::None),
              m_procLevel(0),
              m_level(0),
              m_subLevel(0),
              m_index(0)
        {}

        FileTypeInfo(
            const Data_TempFileTypes::FileType fileType,
            const SAPDB_Int         procLevel,
            const FileLevel         level,
            const FileSubLevel      subLevel,
            const FileIndex         index,
            const FileDir_FileNo&   fileNo )
            : m_fileType(fileType),
              m_procLevel(procLevel),
              m_level(level),
              m_subLevel(subLevel),
              m_index(index),
              m_fileNo(fileNo)
        {}
    };

    SQLMan_TempFileReturnCode FindFile(
        const Data_TempFileTypes::FileType fileType,
        const FileLevel       level,
        const FileSubLevel    subLevel,
        const FileIndex       indexNo,
        FileTypeInfo&         fileInfo);

    SQLMan_TempFileReturnCode NewFile(
        const tgg00_TransContext& trans,
        const Data_TempFileTypes::FileType fileType,
        const FileLevel         level,
        const FileSubLevel      subLevel,
        const FileIndex         index,
        FileDir_FileNo&         fileNo,
        Msg_List&               message);

    SQLMan_TempFileReturnCode UnmapFile(
        const FileDir_FileNo& logicalFileNo,
        Msg_List&             message);

    SQLMan_TempFileReturnCode UnmapFile(
        const Data_TempFileTypes::FileType tempFileType,
        const FileLevel                    level,
        const FileSubLevel                 sublevel,
        const FileIndex                    index,
        Msg_List&                          message);

    typedef Container_Vector<FileTypeInfo> FileInfoList;

    typedef FileInfoList::SizeType SizeType;

    struct FileNoMapEntry {
        FileDir_FileNo keyFileNo;
        FileDir_FileNo logicalFileNo;

        /// Default constructor results in invalid fileNos.
        FileNoMapEntry() {};

        FileNoMapEntry(
            const FileDir_FileNo& newKey,
            const FileDir_FileNo& newValue )
            : keyFileNo(newKey),
              logicalFileNo(newValue)
        {}
    };

    typedef Container_Vector<FileNoMapEntry> FileNoMap;

    bool FindFileByFileNo(
        const FileDir_FileNo&         fileNo,
        FileInfoList::Iterator&       it);

    SAPDB_Int GetFileTypeProcLevel(
        const Data_TempFileTypes::FileType fileType) const;

    SAPDB_Int               m_procLevel;
    SAPDBMem_IRawAllocator& m_allocator;
    FileInfoList            m_fileInfoList;
    FileNoMap               m_fileNoMapper;
};

/****************************************************************************/

inline
SAPDB_Int
SQLMan_TempFileCollection::GetProcLevel() const
{
    return m_procLevel;
}

/****************************************************************************/

#endif // SQLMAN_TEMPFILECOLLECTION_HPP
