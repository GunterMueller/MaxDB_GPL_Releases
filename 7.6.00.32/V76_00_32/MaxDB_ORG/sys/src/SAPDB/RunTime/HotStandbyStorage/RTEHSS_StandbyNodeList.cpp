/*!
  @file           RTEHSS_StandbyNodeList.cpp
  @author         JoergM
  @ingroup        Runtime
  @brief          HotStandbyStorage Kernel Interface
  
  Standby Node List Singleton implementation

  @see            

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/HotStandbyStorage/RTEHSS_StandbyNodeList.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "RunTime/MemoryManagement/RTEMem_RteAllocator.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "RunTime/System/RTESys_MicroTime.h"   
#include <ctype.h>
/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*! Reduce number of allocation per 'AddNode()'... */
#define RTEHSS_MINIMUM_NODE_ALLOCATION 8

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
  @brief          Return reference to single instance of RTEHSS_StandbyNodeList
  @return         Reference to RTEMem_StandbyNodeList singleton instance
*/
RTEHSS_StandbyNodeList & RTEHSS_StandbyNodeList::Instance()
{
    SAPDBMEM_STATIC_RAW_ALLOCATE(Space, sizeof(RTEHSS_StandbyNodeList));
    
    if ( !m_Instance )
    {
        m_Instance = new (Space) RTEHSS_StandbyNodeList();
    }
    
    SAPDBERR_ASSERT_STATE(m_Instance != NULL);
    
    return *m_Instance;
}

RTEHSS_StandbyNodeList * RTEHSS_StandbyNodeList::m_Instance = 0;

/*!
  @brief constructor of the RTEHSS_StandbyNodeList singleton

  It registers the node list spinlock and initializes the members. Called only once and is private...
 */
RTEHSS_StandbyNodeList::RTEHSS_StandbyNodeList()
: m_NodesAllocated(0),
  m_NodeCount(0),
  m_ListModificationCounter(0),
  m_NodeList(0),
  m_Spinlock((const SAPDB_UTF8 *)"RTEHSS_StandbyNodeList")
{}

/*!
  @brief Return uppercase version of node name
  @param nodeName [in] given node name
  @param uppercaseNodeName [out] uppercase converted node name
 */
void RTEHSS_StandbyNodeList::UppercaseNode(RTE_Nodename const &nodeName, RTE_Nodename &uppercaseNodeName)
{
    SAPDB_MemFillNoCheck( &uppercaseNodeName, 0, sizeof(RTE_Nodename) );
    for ( int iByte = 0; iByte < sizeof(RTE_Nodename); iByte++ )
    {
        uppercaseNodeName[iByte] = toupper(nodeName[iByte]);

        if ( !nodeName[iByte] )
        {
            break;
        }
    }
}

/*!
   @brief     Add a node to list of known nodes
   @param     nodeName [in] node to add
   @return    true if node was successfull added or already existed in node list, false else
 */
SAPDB_Bool RTEHSS_StandbyNodeList::AddNode(RTE_Nodename const & nodeName,
                                           SAPDB_Int4 & nodeIndex)
{
    SAPDB_Int4 newNodeIndex;

    if ( FindNode(nodeName, newNodeIndex) )
    {
        nodeIndex = newNodeIndex;
        return true;
    }

    Statistic *newEntry = (Statistic *)RTEMem_RteAllocator::Instance().Allocate( 
                              sizeof(Statistic) );
    if ( !newEntry )
    {
        return false;
    }
    SAPDB_MemFillNoCheck(newEntry, 0, sizeof(Statistic));
    UppercaseNode(nodeName, newEntry->nodeName);
    newEntry->nodeState = StateAdded;
    newEntry->timeOfAddNode = RTESys_MicroSecondTime();

    // yes i know... It could be someone adding the same node in this list
    // simultanously and we would produce a resource leak... But think about
    // the likelihood of such an action... And nevertheless, the only thing
    // that woluld be confusing is a 'doubled' system view entry...
    if ( FindNode(nodeName, newNodeIndex) )
    {
        return false;
    }

    /* In case of to many collisions we have to reduce this spinlock scope to exclude Allocate call... */
    {
        RTESync_LockedScope Lock(m_Spinlock);
        newNodeIndex = m_NodeCount;

        /* Only if new node exceeds MINIMUM_NODE_ALLOCATION another allocate is needed */
        if ( (m_NodeCount+1) > m_NodesAllocated )
        {
            Statistic **oldList = m_NodeList;
            Statistic **newList = (Statistic **)RTEMem_RteAllocator::Instance().Allocate( 
                              (m_NodeCount+RTEHSS_MINIMUM_NODE_ALLOCATION) * sizeof(Statistic *) );
            if ( !newList )
            {
                return false;
            }
            m_NodesAllocated = m_NodeCount+RTEHSS_MINIMUM_NODE_ALLOCATION;

            if ( m_NodeCount > 0 )
            {
                SAPDB_MemCopyNoCheck(newList, oldList, m_NodeCount * sizeof(Statistic *) );
            }
            SAPDB_MemFillNoCheck( newList, 0, (m_NodesAllocated-m_NodeCount) * sizeof(Statistic *) );

            if ( oldList )
            {
                RTEMem_RteAllocator::Instance().Deallocate( oldList );
            }
            m_NodeList = newList;
        }

        /* inside locked scope since depending on 'm_NodeList and m_NodeCount'... */
        m_NodeList[m_NodeCount++] = newEntry;
        ++m_ListModificationCounter;
    }

    if ( FindNode(nodeName, newNodeIndex) )
    {
        nodeIndex = newNodeIndex;
        return true;
    }
    else
    {
        return false;
    }
}

/*!
   @brief     Add a node to list of known nodes
   @param     nodeName [in] node to add
   @return    true if node was successfull added or already existed in node list, false else
 */
SAPDB_Bool RTEHSS_StandbyNodeList::FindNode( RTE_Nodename const & nodeName,
                                             SAPDB_Int4 & nodeIndex )
{
    Statistic *nodeEntry;
    SAPDB_Int4 nodesToSearch;
    RTE_Nodename searchNodeName;

    UppercaseNode(nodeName, searchNodeName);

    SAPDB_Int4 iNode;
    for ( iNode = 0; iNode < (nodesToSearch = m_NodeCount); iNode++ )
    {
        {
            RTESync_LockedScope Lock(m_Spinlock);
            nodeEntry = m_NodeList[iNode];
        }

        if ( 0 == strcmp(nodeEntry->nodeName, searchNodeName) )
        {
            nodeIndex = iNode;
            break;
        }
    }

    return ( iNode < nodesToSearch );
}

/*!
   @brief    Remove a node from list of known nodes
   @param    nodeName [in] node to remove
 */
void RTEHSS_StandbyNodeList::RemoveNode(SAPDB_Int4 nodeIndex)
{
    Statistic *nodeEntry;
    SAPDB_UInt8 now = RTESys_MicroSecondTime();
    {
        RTESync_LockedScope Lock(m_Spinlock);
        nodeEntry = m_NodeList[nodeIndex];

        if ( StateRemoved == nodeEntry->nodeState )
        {
            return; // remove only once...
        }

        if ( StateWaitForSynchronize == nodeEntry->nodeState )
        {
            --m_ListModificationCounter;
        }

        nodeEntry->nodeState = StateRemoved;
        nodeEntry->timeOfRemoveNode = now;
    }
}

/*!
  @brief          Notify that a PREPARE STANDBY command was received for the node

  Updates timeOfPrepareStandby and sets state to 'InPrepareStandby'.

  @param          nodeIndex [in] index node to mark as removed (result of FindNode call)
 */
void RTEHSS_StandbyNodeList::GotPrepareStandby(SAPDB_Int4 nodeIndex)
{
    Statistic *nodeEntry;
    SAPDB_UInt8 now = RTESys_MicroSecondTime();
    {
        RTESync_LockedScope Lock(m_Spinlock);

        nodeEntry = m_NodeList[nodeIndex];

        if ( StateWaitForSynchronize == nodeEntry->nodeState )
        {
            --m_ListModificationCounter;
        }
        nodeEntry->nodeState = StateInPrepareStandby;
        nodeEntry->timeOfPrepareStandby = now;
    }
}

/*!
  @brief          Notify that a REGISTER STANDBY command was received for the node

  Updates timeOfRegisterStandby, lastSavepointOffset and sets state to 'InRegisterStandby'.
  @param          nodeIndex [in] index node to mark as removed (result of FindNode call)
 */
void RTEHSS_StandbyNodeList::GotRegisterStandby( SAPDB_Int4 nodeIndex,
                                                 SAPDB_UInt4 lastSavepointOffset )
{
    Statistic *nodeEntry;
    SAPDB_UInt8 now = RTESys_MicroSecondTime();
    {
        RTESync_LockedScope Lock(m_Spinlock);

        nodeEntry = m_NodeList[nodeIndex];

        if ( StateWaitForSynchronize == nodeEntry->nodeState )
        {
            --m_ListModificationCounter;
        }
        nodeEntry->nodeState = StateInRegisterStandby;
        nodeEntry->lastSavepointOffset = lastSavepointOffset;
        nodeEntry->timeOfRegisterStandby = now;
    }
}

/*!
  @brief          Notify that a REGISTER STANDBY command was completed

  Sets state to 'WaitForSynchronize' and this way allows node to be retrieved using
  RTEHSS_NodeChain class.

  @param          nodeIndex [in] index node to mark as removed (result of FindNode call)
 */
void RTEHSS_StandbyNodeList::SetWaitForSynchronize( SAPDB_Int4 nodeIndex )
{
    Statistic *nodeEntry;
    {
        RTESync_LockedScope Lock(m_Spinlock);

        nodeEntry = m_NodeList[nodeIndex];

        if ( StateWaitForSynchronize != nodeEntry->nodeState )
        {
            ++m_ListModificationCounter;
            nodeEntry->nodeState = StateWaitForSynchronize;
        }
    }
}

/*!
  @brief          Mark a node from list as deactivated

  Due to timeout etc. a node is deactivated.
  Updates timeOfRemoveNode and sets state to 'Deactivated'.

  @param          nodeIndex [in] index node to mark as removed (result of FindNode call)
 */
void RTEHSS_StandbyNodeList::Deactivate( SAPDB_Int4  nodeIndex )
{
    Statistic *nodeEntry;
    SAPDB_UInt8 now = RTESys_MicroSecondTime();
    {
        RTESync_LockedScope Lock(m_Spinlock);

        nodeEntry = m_NodeList[nodeIndex];

        if ( StateWaitForSynchronize == nodeEntry->nodeState )
        {
            --m_ListModificationCounter;
        }
        nodeEntry->nodeState = StateDeactivated;
        nodeEntry->timeOfRemoveNode = now;
    }
}

/*!
   @brief    Fill node chain with copy of current node list

  This call copies the complete NodeName entries. The list must
  be freed by caller with FreeNodeChain. The complete list and
  all its content can be freed later on with a single Deallocation
  of the returned firstInNodeChain list pointer.
  If an empty list is to be copied, true is returned but both pointers are
  set set to NULL.

  Only elements in the node list that are in state 'WaitForSynchronize' are copied.

   @param          allocator [in] allocator to use
   @param          firstInNodeChain [out] filled with pointer to complete list
   @param          lastNodeInChain [out] filled with pointer to last node in list
   @return         true if got node 8even if empty), false if copy failed
 */
SAPDB_Bool RTEHSS_StandbyNodeList::FillNodeChain( SAPDBMem_IRawAllocator & allocator,
                                                  RTE_Nodename **        & firstInNodeChain,
                                                  RTE_Nodename **        & lastInNodeChain,
                                                  SAPDB_Int4             & listModificationCounter )
{
    {
        // If list was not modified since last call, the call is void
        RTESync_LockedScope LockModifyCheck(m_Spinlock);

        if ( 0 != listModificationCounter
          && listModificationCounter == m_ListModificationCounter )
        {
            return true;
        }
    }

    SAPDB_Int4 copyCount = m_NodeCount;
    RTE_Nodename **copyList = 0;

    if ( 0 == copyCount )
    {
        firstInNodeChain = lastInNodeChain = 0;
        return true;
    }

    SAPDB_Int4 totalLength = copyCount * sizeof(RTE_Nodename *);
    // Chunk contain the list of node name pointers and the node names behind that list
    copyList = (RTE_Nodename **)allocator.Allocate(totalLength);
    if ( !copyList )
    {
        firstInNodeChain = lastInNodeChain = 0;
        return false;
    }

    SAPDB_Int4 nodesActive = 0;

    for ( SAPDB_Int4 iNode = 0; iNode < copyCount; iNode++ )
    {
        Statistic *nodeEntry;
        {
            RTESync_LockedScope Lock(m_Spinlock);
            nodeEntry = m_NodeList[iNode];
        }

        if ( StateWaitForSynchronize == nodeEntry->nodeState )
        {
            copyList[nodesActive++] = &nodeEntry->nodeName;
        }
    }

    if ( 0 == nodesActive )
    {
        allocator.Deallocate(copyList);
        firstInNodeChain = lastInNodeChain = 0;
        return true;
    }

    firstInNodeChain = copyList;
    lastInNodeChain = &firstInNodeChain[nodesActive-1];

    return true;
}

/*!
  @brief          Notify that a SYNCHRONIZE command was send to this node

  Updates timeOfSynchronize and timeOfLastSuccessfulSynchronize but does not modify state!
  The synchronizeSendCount is increased and synchronizeRetryCount is reset.

  @param          nodeIndex [in] index node to mark as removed (result of FindNode call)
  @param          lastSavepointOffset [in] flag set to true 
 */
void RTEHSS_StandbyNodeList::AddSynchronizeCallResult( SAPDB_Int4  nodeIndex,
                                                       SAPDB_Bool  callOk,
                                                       SAPDB_UInt4 lastValidOffset,
                                                       SAPDB_UInt4 lastSavepointOffset )
{
    Statistic *nodeEntry;
    SAPDB_UInt8 now = RTESys_MicroSecondTime();
    {
        RTESync_LockedScope Lock(m_Spinlock);
        {
            nodeEntry = m_NodeList[nodeIndex];
            if ( callOk )
            {
                ++(nodeEntry->synchronizeSendCount);
                nodeEntry->synchronizeRetryCount = 0;
                nodeEntry->lastValidLogOffsetSent = lastValidOffset;
                nodeEntry->lastSavepointOffset = lastSavepointOffset;
                nodeEntry->timeOfLastSuccessfulSynchronize = now;
            }
            else
            {
                ++(nodeEntry->synchronizeRetryCount);
            }
            nodeEntry->timeOfSynchronize = now;
        }
    }
}
