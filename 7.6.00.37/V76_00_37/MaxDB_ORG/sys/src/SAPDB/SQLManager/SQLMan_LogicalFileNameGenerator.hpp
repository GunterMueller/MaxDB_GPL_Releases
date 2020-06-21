/*****************************************************************************/
/*!
  @file         SQLMan_LogicalFileNameGenerator
  @author       MartinKi
  @ingroup      SQLManager

  @brief        Header file for class SQLMan_LogicalFileNameGenerator.

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

#ifndef SQLMAN_LOGICALFILENAMEGENERATOR_HPP
#define SQLMAN_LOGICALFILENAMEGENERATOR_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SQLManager/SQLMan_Types.hpp"
#include "DataAccess/Data_TempFileTypes.hpp"
#include "Container/Container_Vector.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/
class SAPDBMem_IRawAllocator;
class FileDir_FileNo;

/*!
 * @class SQLMan_LogicalFileNameGenerator
 * @ingroup SQLManager
 * @author MartinKi
 *
 * @brief This class can be used to create and parse logical file
 * names for temporary files.
 *
 * @sa SQLMan_TempFileCollection
 */
class SQLMan_LogicalFileNameGenerator
{
public:
    /// Return codes.
    enum ReturnCode {
        SQLMan_Okay,
        SQLMan_OutOfMemory,
        SQLMan_LevelOutOfRange,
        SQLMan_IndexOutOfRange
    };

    /*!
     * @brief Constructor. File names start with level = 0 and index = 0.
     *
     * @param allocator [in] allocator to be used for memory requests
     * @param fileType [in] file type of this file name generator; all
     * logical file name will be created for this file type
     */
    SQLMan_LogicalFileNameGenerator(
        SAPDBMem_IRawAllocator&            allocator,
        const Data_TempFileTypes::FileType fileType);

    /*!
     * @brief Sets internal level and index to given values.
     *
     * @param level [in] new level value
     * @param index [in] new index value
     *
     * @return
     *    - SQLMan_Okay
     *    - SQLMan_LevelOutOfRange
     *    - SQLMan_IndexOutOfRange
     *    - SQLMan_OutOfMemory
     */
    ReturnCode SetState(
        const SAPDB_Int4 level,
        const SAPDB_Int4 index);

    /*!
     * @brief Increments the level. index is initialized with 0 for this level.
     * @param fileName [out] logical file name for increased level on success
     *
     * @return
     *    - SQLMan_Okay
     *    - SQLMan_LevelOutOfRange
     *    - SQLMan_OutOfMemory
     */
    ReturnCode IncreaseLevel(
        SQLMan_Surrogate& fileName);

    /*!
     * @brief Increments the index for the given level.
     *
     * @param level [in] level for which index should be incremented;
     * level must be within the range of levels already in use
     * @param fileName [out] logical file name for increased index on success
     *
     * @return
     *    - SQLMan_Okay
     *    - SQLMan_LevelOutOfRange
     *    - SQLMan_IndexOutOfRange
     *    - SQLMan_OutOfMemory
     */
    ReturnCode IncreaseIndex(
        const SAPDB_Int4  level,
        SQLMan_Surrogate& fileName);

    /*!
     * @brief Increments the index for the current level.
     *
     * @param fileName [out] logical file name for increased index on success
     *
     * @return
     *    - SQLMan_Okay
     *    - SQLMan_IndexOutOfRange
     *    - SQLMan_OutOfMemory
     */
    ReturnCode IncreaseCurrentIndex(
        SQLMan_Surrogate& fileName);

    /*!
     * @brief Resets the index counter for the given level.
     *
     * @param level [in] level for which index should be reset; level
     * must be within the range of levels already in use
     *
     * @return
     *    - SQLMan_Okay
     *    - SQLMan_LevelOutOfRange
     */
    ReturnCode ResetLevel(const SAPDB_Int4 level);

    /*!
     * @brief Resets the index counter for the current level.
     */
    void ResetCurrentLevel();

    /*!
     * @brief Resets the index counter for all levels.
     */
    void ResetAll();


    /*!
     * @brief Generates a logical file name for the current level and index.
     *
     * @param fileName [out] generated file name
     */
    void GenerateFileName(
        SQLMan_Surrogate& fileName) const;

    /*!
     * @brief Parses a logical file name.
     *
     * @param fileName [in] logical file name
     * @param fileType [out] file type of file
     * @param level [out] level of file
     * @param sublevel [in] sublevel of file
     * @param index [out] index of file
     *
     * @return
     *    - true, if file name was a valid logical file name
     *    - false, if file name was not a valid logical file name
     */
    static SAPDB_Bool ParseFileName(
        const SQLMan_Surrogate&       fileName,
        Data_TempFileTypes::FileType& fileType,
        SAPDB_Int&                    level,
        SAPDB_Int&                    sublevel,
        SAPDB_Int4&                   index);

    /*!
     * @brief Parses a logical file name.
     *
     * @param fileName [in] logical file name
     * @param fileType [out] file type of file
     * @param level [out] level of file
     * @param sublevel [in] sublevel of file
     * @param index [out] index of file
     *
     * @return
     *    - true, if file name was a valid logical file name
     *    - false, if file name was not a valid logical file name
     */
    static SAPDB_Bool ParseFileName(
        const FileDir_FileNo&         fileNo,
        Data_TempFileTypes::FileType& fileType,
        SAPDB_Int&                    level,
        SAPDB_Int&                    sublevel,
        SAPDB_Int4&                   index);

    /*!
     * @brief Generates a logical file name for the given type, level
     * and index.
     *
     * @param fileType [in] file type of file
     * @param level [in] level of file
     * @param sublevel [in] sublevel of file
     * @param index [in] index of file
     * @param fileName [out] generated file name
     */
    static ReturnCode GeneratePreparedFileName(
        const Data_TempFileTypes::FileType fileType,
        const SAPDB_Int4&                  level,
        const SAPDB_Int4&                  sublevel,
        const SAPDB_Int4&                  index,
        SQLMan_Surrogate&                  fileName);

    /*!
     * @brief Generates a logical file name for the given type, level
     * and index.
     *
     * @param fileType [in] file type of file
     * @param level [in] level of file
     * @param sublevel [in] sublevel of file
     * @param index [in] index of file
     * @param fileName [out] generated file name
     */
    static ReturnCode GeneratePreparedFileName(
        const Data_TempFileTypes::FileType fileType,
        const SAPDB_Int4&                  level,
        const SAPDB_Int4&                  sublevel,
        const SAPDB_Int4&                  index,
        FileDir_FileNo&                    fileName);

    /*!
     * @brief Returns the highest level for which logical file names
     * can be generated.
     */
    static SAPDB_Int4 GetMaxLevelAllowed();

    /*!
     * @brief Returns the highest index for which logical file names
     * can be generated.
     */
    static SAPDB_Int4 GetMaxIndexAllowed();

private:
    typedef Container_Vector<SAPDB_Int4> LevelVector;

    SAPDBMem_IRawAllocator&      m_allocator;
    Data_TempFileTypes::FileType m_fileType;
    SAPDB_Int4                   m_currentLevel;
    SAPDB_Int4                   m_currentIndex;
    LevelVector                  m_indexVector;

    // no copy-construction
    SQLMan_LogicalFileNameGenerator(
        const SQLMan_LogicalFileNameGenerator& p)
        : m_allocator(p.m_allocator),
          m_indexVector(p.m_allocator)
    {}
};

#endif // SQLMAN_LOGICALFILENAMEGENERATOR_HPP
