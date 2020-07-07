/*****************************************************************************/
/*!
  @file         SQLMan_LogicalFileNameGenerator.hpp
  @author       MartinKi
  @ingroup      SQLManager

  @brief        Implementation file for class SQLMan_LogicalFileNameGenerator.

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

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include "SQLManager/SQLMan_LogicalFileNameGenerator.hpp"
#include "SQLManager/SQLMan_Types.hpp"

#include "DataAccess/Data_TempFileTypes.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"

#include "ggg00.h" // for cgg_zero_id
#include "gsp00.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

const SAPDB_Int4 c_maxLevel = SAPDB_MAX_INT2;
const SAPDB_Int4 c_maxIndex = SAPDB_MAX_INT4;

/**************************************************************************/

SQLMan_LogicalFileNameGenerator::SQLMan_LogicalFileNameGenerator(
    SAPDBMem_IRawAllocator& allocator,
    const Data_TempFileTypes::FileType fileType)
    : m_allocator(allocator),
      m_fileType(fileType),
      m_currentLevel(0),
      m_currentIndex(0),
      m_indexVector(allocator)
{
    m_indexVector.InsertEnd( 0 );
}

/**************************************************************************/

SQLMan_LogicalFileNameGenerator::ReturnCode SQLMan_LogicalFileNameGenerator::SetState(
    const SAPDB_Int4 level,
    const SAPDB_Int4 index)
{
    if ( (level < 0) || (level >= c_maxLevel) ) {
        return SQLMan_LevelOutOfRange;
    }

    if ( (index < 0) || (index >= c_maxIndex) ) {
        return SQLMan_IndexOutOfRange;
    }

    SQLMan_Surrogate dummyName;
    SQLMan_LogicalFileNameGenerator::ReturnCode rc = SQLMan_Okay;
    while ( (m_currentLevel < level) && (rc == SQLMan_Okay) ) {
        rc = this->IncreaseLevel( dummyName );
    }
    m_indexVector[ m_currentLevel ] = index;
    return SQLMan_Okay;
}

/**************************************************************************/

SQLMan_LogicalFileNameGenerator::ReturnCode
SQLMan_LogicalFileNameGenerator::IncreaseLevel(
    SQLMan_Surrogate& fileName)
{
    if ( m_currentLevel >= c_maxLevel ) {
        return SQLMan_LevelOutOfRange;
    }

    if ( m_indexVector.GetSize() <=
         static_cast<LevelVector::SizeType>(m_currentLevel)  ) {
        if ( !m_indexVector.Reserve( m_currentLevel * 2 + 1 ) ) {
            return SQLMan_OutOfMemory;
        }
    }
    m_indexVector.InsertEnd( 0 );
    ++m_currentLevel;
    this->GenerateFileName( fileName );
    return SQLMan_Okay;
}

/**************************************************************************/

SQLMan_LogicalFileNameGenerator::ReturnCode
SQLMan_LogicalFileNameGenerator::IncreaseIndex(
    const SAPDB_Int4 level,
    SQLMan_Surrogate& fileName)
{
    if ( (m_indexVector.GetSize() == 0) && (level == 0) ) {
        // level 0 should always be initialized automatically
        if ( m_indexVector.Reserve( m_currentLevel * 2 + 1 ) ) {
            m_indexVector.InsertEnd( 0 );
        } else {
            return SQLMan_OutOfMemory;
        }
    }

    if ( (level < 0)
         ||
         (static_cast<LevelVector::SizeType>(level) >= m_indexVector.GetSize())
        ) {
        return SQLMan_LevelOutOfRange;
    }

    if ( m_indexVector[ level ] >= c_maxIndex ) {
        return SQLMan_IndexOutOfRange;
    }
    ++m_indexVector[ level ];
    this->GenerateFileName( fileName );
    return SQLMan_Okay;
}

/**************************************************************************/

SQLMan_LogicalFileNameGenerator::ReturnCode
 SQLMan_LogicalFileNameGenerator::IncreaseCurrentIndex(
    SQLMan_Surrogate& fileName)
{
    if ( m_indexVector.GetSize() == 0 ) {
        SAPDBERR_ASSERT_STATE( m_currentLevel == 0 );
        // level 0 should always be initialized automatically
        if ( m_indexVector.Reserve( 1 ) ) {
            m_indexVector.InsertEnd( 0 );
        } else {
            return SQLMan_OutOfMemory;
        }
    }

    if ( m_indexVector[ m_currentLevel ] >= c_maxIndex ) {
        return SQLMan_IndexOutOfRange;
    }
    ++m_indexVector[ m_currentLevel ];
    this->GenerateFileName( fileName );
    return SQLMan_Okay;
}

/**************************************************************************/

SQLMan_LogicalFileNameGenerator::ReturnCode
SQLMan_LogicalFileNameGenerator::ResetLevel(const SAPDB_Int4 level)
{
    if ( (level < 0) ||
         (static_cast<LevelVector::SizeType>(level) >= m_indexVector.GetSize())
        ) {
        return SQLMan_LevelOutOfRange;
    }

    m_indexVector[ level ] = 0;
    return SQLMan_Okay;
}

/**************************************************************************/

void SQLMan_LogicalFileNameGenerator::ResetAll()
{
    m_indexVector.Clear();
    m_indexVector.InsertEnd( 0 );
    m_currentLevel = 0;
}

/**************************************************************************/

void SQLMan_LogicalFileNameGenerator::GenerateFileName(
    SQLMan_Surrogate& fileName) const
{
    SAPDB_MemCopyNoCheck( &fileName, &cgg_zero_id, sizeof(SQLMan_Surrogate) );

    fileName[0] = static_cast<char>(m_fileType);
    fileName[1] = 0;
    fileName[2] = static_cast<char>(m_currentLevel / 255 );
    fileName[3] = static_cast<char>(m_currentLevel % 255 );
    // FIXME: should the following be byte-swap independent?
    SAPDB_Int4* fnIndex = reinterpret_cast<SAPDB_Int4*>(&fileName[4]);
    *fnIndex    = m_indexVector[ m_currentLevel ];
}

/**************************************************************************/

SAPDB_Bool SQLMan_LogicalFileNameGenerator::ParseFileName(
    const SQLMan_Surrogate& fileName,
    Data_TempFileTypes::FileType& fileType,
    SAPDB_Int&                    level,
    SAPDB_Int&                    sublevel,
    SAPDB_Int4& index)
{
    if ( (fileName[1] != 0) ) {
        return false;
    }
    fileType = (Data_TempFileTypes::FileType) (fileName[0]);

    switch (fileType) {
    case Data_TempFileTypes::FromSelect:
    case Data_TempFileTypes::ComplexSelect:
    case Data_TempFileTypes::ComplexView:
    case Data_TempFileTypes::Subquery:
    case Data_TempFileTypes::Union:
        level    = fileName[ 2 ];
        sublevel = fileName[ 3 ];
        break;
    default:
        tsp00_IntMapC2 i2c;
        i2c.mapC2_sp00()[ 0 ] = fileName[ 2 ];
        i2c.mapC2_sp00()[ 1 ] = fileName[ 3 ];
        level    = i2c.mapInt_sp00();
        sublevel = 0;
    } /* switch */

    SAPDB_MemCopyNoCheck( &index, &fileName[4], sizeof(index) );
    return true;
}

/**************************************************************************/

SQLMan_LogicalFileNameGenerator::ReturnCode
SQLMan_LogicalFileNameGenerator::GeneratePreparedFileName(
        const Data_TempFileTypes::FileType fileType,
        const SAPDB_Int4& level,
        const SAPDB_Int4& sublevel,
        const SAPDB_Int4& index,
        SQLMan_Surrogate& fileName)
{
    if ( (level < 0) || (level >= c_maxLevel) ) {
        return SQLMan_LevelOutOfRange;
    }

    if ( (index < 0) || (index >= c_maxIndex) ) {
        return SQLMan_IndexOutOfRange;
    }

    fileName[0] = static_cast<char>(fileType);
    fileName[1] = 0;

    switch (fileType) {
    case Data_TempFileTypes::FromSelect:
    case Data_TempFileTypes::ComplexSelect:
    case Data_TempFileTypes::ComplexView:
    case Data_TempFileTypes::Subquery:
    case Data_TempFileTypes::Union:
        if ( (level > 255) || (sublevel > 255) ) {
            return SQLMan_LevelOutOfRange;
        }
        fileName[2] = static_cast<char>( level );
        fileName[3] = static_cast<char>( sublevel );
        break;
    default:
        tsp00_IntMapC2 i2c;
        i2c.mapInt_sp00() = level;
        fileName[ 2 ]     = i2c.mapC2_sp00()[ 0 ];
        fileName[ 3 ]     = i2c.mapC2_sp00()[ 1 ];
    }

    SAPDB_MemCopyNoCheck( &fileName[4], &index, sizeof(index) );

    return SQLMan_Okay;
}

/**************************************************************************/

SAPDB_Int4
SQLMan_LogicalFileNameGenerator::GetMaxLevelAllowed()
{
    return c_maxLevel;
}

/**************************************************************************/

SAPDB_Int4
SQLMan_LogicalFileNameGenerator::GetMaxIndexAllowed()
{
    return c_maxIndex;
}

/**************************************************************************/

SAPDB_Bool
SQLMan_LogicalFileNameGenerator::ParseFileName(
    const FileDir_FileNo&         fileNo,
    Data_TempFileTypes::FileType& fileType,
    SAPDB_Int&                    level,
    SAPDB_Int&                    sublevel,
    SAPDB_Int4&                   index)
{
    return SQLMan_LogicalFileNameGenerator::ParseFileName (
        reinterpret_cast<const tgg00_Surrogate&>( fileNo ),
        fileType,
        level,
        sublevel,
        index );
}

/**************************************************************************/

SQLMan_LogicalFileNameGenerator::ReturnCode
SQLMan_LogicalFileNameGenerator::GeneratePreparedFileName(
    const Data_TempFileTypes::FileType fileType,
    const SAPDB_Int4&                  level,
    const SAPDB_Int4&                  sublevel,
    const SAPDB_Int4&                  index,
    FileDir_FileNo&                    fileNo)
{
    return SQLMan_LogicalFileNameGenerator::GeneratePreparedFileName (
        fileType,
        level,
        sublevel,
        index,
        reinterpret_cast<tgg00_Surrogate&>( fileNo ) );
}
