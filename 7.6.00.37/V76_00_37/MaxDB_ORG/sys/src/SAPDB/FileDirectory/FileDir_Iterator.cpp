/*****************************************************************************/
/*!
 @file    FileDir_Iterator.cpp
 @author  MartinKi
 @ingroup FileDirectory_Files

 @brief Implementation of class FileDir_Iterator.

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

#include "FileDirectory/FileDir_Iterator.hpp"
#include "FileDirectory/FileDir_IteratorObj.hpp"
#include "FileDirectory/FileDir_Common.hpp"
#include "DataAccess/Data_FileTypes.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

class FileDir_Generic;
class FileDir_IteratorObj;
class SAPMem_IRawAllocator;

/**************************************************************************/

FileDir_Iterator::FileDir_Iterator(SAPDBMem_IRawAllocator& allocator)
    : m_iterator(0),
      m_allocator(&allocator)
{}

/**************************************************************************/

FileDir_Iterator::FileDir_Iterator(
    SAPDBMem_IRawAllocator& allocator,
    FileDir_IteratorObj* iterator)
    : m_iterator(iterator),
      m_allocator(&allocator)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Iterator::Iterator", FileDir_Trace, 5 );
}

/**************************************************************************/

FileDir_Iterator::FileDir_Iterator(const FileDir_Iterator& it)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Iterator::Iterator-cc",
                             FileDir_Trace, 5 );

    m_allocator = it.m_allocator;
    m_iterator  = it.m_iterator;
    if ( m_iterator != 0 ) {
        m_iterator->IncRefCounter();
    }
}

/**************************************************************************/

FileDir_Iterator::~FileDir_Iterator()
{
    DecRefCounter();
}

/**************************************************************************/

FileDir_Iterator& FileDir_Iterator::operator=(
    const FileDir_Iterator& it)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Iterator::operator=",
                             FileDir_Trace, 5 );
    if ( this == &it ) {
        return *this;
    }

    DecRefCounter();  // handle possible previously assigned iterator

    m_allocator = it.m_allocator;
    m_iterator  = it.m_iterator;
    if ( m_iterator != 0 ) {
        m_iterator->IncRefCounter();
    }
    return *this;
}

/**************************************************************************/
SAPDB_Bool FileDir_Iterator::GetNext(FileDir_Generic& p)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Iterator::GetNext",
                             FileDir_Trace, 7 );

    SAPDBERR_ASSERT_STATE( m_iterator != 0 );
    return m_iterator->GetNext( p );
}

/**************************************************************************/

SAPDB_Bool FileDir_Iterator::AddFileTypeToFilter(const Data_FileType fileType)
{
    if ( !CreateIteratorObj() ) {
        return SAPDB_FALSE;
    }

    return m_iterator->AddFileTypeToFilter( fileType );
}

/**************************************************************************/

SAPDB_Bool FileDir_Iterator::AddHideDeletedFilesFilter()
{
    if ( !CreateIteratorObj() ) {
        return SAPDB_FALSE;
    }

    return m_iterator->AddHideDeletedFilesFilter();
}

/*************************************************************************/

// void FileDir_Iterator::Assign(FileDir_IteratorObj& it)
// {
//     SAPDBTRACE_METHOD_DEBUG( "FileDir_Iterator::Assign",
//                              FileDir_Trace, 5 );
//     if ( m_iterator != 0 ) {
//         destroy( m_iterator, m_allocator );
//     }
//     m_iterator    = &it;
// }

/**************************************************************************/

SAPDB_Bool FileDir_Iterator::CreateIteratorObj()
{
    if ( m_iterator == 0 ) {
        m_iterator = new (*m_allocator) FileDir_IteratorObj(*m_allocator);
        if ( m_iterator == 0 ) {
            return SAPDB_FALSE;
        }
        m_iterator->IncRefCounter();
    }
    return SAPDB_TRUE;
}

/**************************************************************************/

FileDir_IteratorObj* FileDir_Iterator::GetIteratorObj() const
{
    return m_iterator;
}

/**************************************************************************/

void FileDir_Iterator::ClearFilter()
{
    SAPDBERR_ASSERT_STATE( m_iterator != 0 );
    m_iterator->ClearFilter();
}

/**************************************************************************/

void FileDir_Iterator::DecRefCounter()
{
    if ( m_iterator != 0 ) {
        m_iterator->DecRefCounter();
        if ( m_iterator->GetRefCounter() == 0 ) {
            destroy( m_iterator, *m_allocator );
        }
    }
}

/**************************************************************************/

FileDir_IDirectory* FileDir_Iterator::GetFileDirectory()
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Iterator::Assign",
                             FileDir_Trace, 5 );
    if ( m_iterator == 0 ) {
        return 0;
    }

    return m_iterator->GetFileDirectory();
}

/**************************************************************************/

void FileDir_Iterator::Detach()
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Iterator::Detach",
                             FileDir_Trace, 5 );

    if ( m_iterator == 0 ) {
        return;
    }

    return m_iterator->Detach();
}

/**************************************************************************/
