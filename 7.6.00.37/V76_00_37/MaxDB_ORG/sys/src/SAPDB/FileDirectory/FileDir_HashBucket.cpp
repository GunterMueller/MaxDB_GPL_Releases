/*****************************************************************************/
/*!
 @file    FileDir_HashBucket.cpp
 @author  MartinKi
 @ingroup FileDirectory_Files

 @brief Implementation of class FileDir_HashBucket.

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

#include "FileDirectory/FileDir_BaseEntry.hpp"
#include "FileDirectory/FileDir_HashBucket.hpp"
#include "FileDirectory/FileDir_Common.hpp"
#include "FileDirectory/FileDir_Generic.hpp"
#include "FileDirectory/FileDir_Types.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"
#include "FileDirectory/FileDir_RWRegionWrapper.hpp"

#include "RunTime/System/RTESys_MemoryBarrier.h"
#include "RunTime/RTE_Types.hpp"

#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
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

/**************************************************************************/

SAPDB_Bool FileDir_HashBucket::GetNextEntry(
    FileDir_Node*&   node,
    FileDir_Generic& pointer)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_HashBucket::GetNextEntry", FileDir_Trace, 6 );

    if ( node == 0 ) {
        node = m_startNode;
    } else {
        node = node->next;
    }

    if ( node != 0 ) {
        pointer.AssignEntry( node->entry );
    } else {
        pointer.Detach();
    }

    return pointer.IsAssigned();
}

/**************************************************************************/

SAPDB_Bool FileDir_HashBucket::GetNextNode(FileDir_Node*&   node)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_HashBucket::GetNextNode", FileDir_Trace, 6 );

    if ( node == 0 ) {
        node = m_startNode;
    } else {
        node = node->next;
    }

    return node != 0;
}

/**************************************************************************/

FileDir_ReturnCode FileDir_HashBucket::AddNode(
    const RTE_TaskId taskId,
    FileDir_Node*    node,
    SAPDB_UInt&      bucketSize,
    Concurrency      concurrencyType)
{
    SAPDBERR_ASSERT_STATE( node != 0 );
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_HashBucket::AddNode", FileDir_Trace, 6 );

    // safety check: is entry already in bucket?
    FileDir_FileNo newFileNo = node->entry->GetFileNo();
    FileDir_Node* n          = m_startNode;
    FileDir_Node* lastNode   = 0;
    bucketSize = 1;
    while (n != 0) {
        if ( n->entry->GetFileNo() == newFileNo ) {
            return FileDir_FileNoExists;
        }
        lastNode = n;
        n        = n->next;
        ++bucketSize;
    }

    node->next = 0;

    if ( concurrencyType == ConcurrentReads ) {
        /*
         * if we do not use proper synchronisation we need to make
         * sure that the chain contents are updated before making the
         * new node visible
         */
        RTESys_WriteMemoryBarrier();
    }

    /*
     * entry is added at the end of the chain because this is safe
     * even with readers running concurrently
     */
    if ( lastNode != 0 ) {
        lastNode->next = node;
    } else {
        m_startNode = node;
    }

    ++m_size;

    return FileDir_Okay;
}

/**************************************************************************/

FileDir_Node* FileDir_HashBucket::RemoveFirstNode()
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_HashBucket::RemoveFirstNode", FileDir_Trace, 6 );
    SAPDBERR_ASSERT_STATE( !IsEmpty() );

    FileDir_Node* n = m_startNode;
    m_startNode = n->next;

    n->next = 0;  // returned node has no successor
    return n;
}

/**************************************************************************/

FileDir_BaseEntry* FileDir_HashBucket::Remove(
    const RTE_TaskId      taskId,
    const FileDir_FileNo& fileNo)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_HashBucket::Remove", FileDir_Trace, 6 );
    FileDir_BaseEntry* entry = 0;

    FileDir_Node* n    = m_startNode;
    FileDir_Node* prev = 0;
    while (n != 0) {
        if ( n->entry->GetFileNo() == fileNo ) {
            if ( prev != 0 ) {
                prev->next = n->next;
            } else {
                m_startNode = n->next;
            }
            entry = n->entry;
            --m_size;
            break;
        }
        prev = n;
        n    = n->next;
    }
    SAPDBERR_ASSERT_STATE( m_size >= 0 );

    // make sure that m_startNode is != 0 if something is in bucket...
    SAPDBERR_ASSERT_STATE ( (m_size > 0) ? (m_startNode != 0) : true );

    return entry;
}

/**************************************************************************/

void FileDir_HashBucket::Clear()
{
    m_startNode = 0;
    m_size      = 0;
}

/**************************************************************************/
