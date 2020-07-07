/*!
  @file           RTEHSS_StandbyNodeList.hpp
  @author         JoergM
  @ingroup        Runtime
  @brief          HotStandbyStorage Kernel Interface

  Standby Node List Singleton and NodeChain iteration class

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



#ifndef RTEHSS_STANDBYNODELIST_HPP
#define RTEHSS_STANDBYNODELIST_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/SAPDB_Types.h"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "RunTime/Synchronisation/RTESync_NamedSpinlock.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
  @class          RTEHSS_StandbyNodeList
  @brief          Container for standby node handling

  This container handles a list of all ever known standby nodes and the statistic
  information about these nodes for the hot standby system view.

  Each StandbyNode is added to the list only once. Nodes that have been added,
  occupy the same slot until the SAPDB kernel stops. If the node is removed,
  its statistic entry is modified, but the slot is not removed. The NodeChain class
  can decide if to retrieve only those nodes that are active or all nodes.
  
  The node name are stored in uppercase internally.

  Node statistic entries are modified by specific calls.
 */

class RTEHSS_StandbyNodeList
{
public:
    /*!
      @brief          Return reference to single instance of RTEHSS_StandbyNodeList
      @return         Reference to RTEMem_StandbyNodeList singleton instance
     */
    static RTEHSS_StandbyNodeList & Instance();

    /*!
      @brief          Internal known state of standby node
     */
    enum State
    {
        StateUnknown,           ///< State is unknown (Should not happen...)
        StateAdded,             ///< Node was added to the list
        StateRemoved,           ///< standby node explicitly removed
        StateInPrepareStandby,  ///< received PREPARE STANDBY command 
        StateInRegisterStandby, ///< received REGISTER STANDBY command
        StateDeactivated,       ///< standby node deactivated due to timeout
        StateWaitForSynchronize ///< ready to receive SYNCHRONIZE commands
    };

    /*!
      @brief Convert internal known state of standby node into readable string
      @param nodeState [in] enumeration value to convert
      @return readable string for enumeration value
     */
    SAPDB_Char const * GetNodeStateAsString(State nodeState) const
    {
        SAPDB_Char const *nodeStateName;
        switch( nodeState )
        {
        case StateUnknown:
            nodeStateName = "UNKNOWN";
            break;
        case StateAdded:
            nodeStateName = "ADDED";
            break;
        case StateRemoved:
            nodeStateName = "REMOVED";
            break;
        case StateInPrepareStandby:
            nodeStateName = "PREPARE STANDBY";
            break;
        case StateInRegisterStandby:
            nodeStateName = "REGISTER STANDBY";
            break;
        case StateDeactivated:
            nodeStateName = "DEACTIVATED";
            break;
        case StateWaitForSynchronize:
            nodeStateName = "WAIT FOR SYNCHRONIZE";
            break;
        default:
            nodeStateName = "UNKNOWN";
            break;
        }
        return nodeStateName;
    }

    /*!
      @brief          System view content per standby node

      Timestamp are given typedef SAPDB_UInt8 TimeStamp; ///< micro seconds since 1.1.1970
     */
    /* see RTESys_Time.h and RTESys_MicroTime.h ... */
    /* RTESys_MicroSecondTime() returns the actual value */
    /* RTESys_BuildSQLTimeStamp() allows to convert in readable local timezone timestamp */
    /* RTESys_BuildSQLGMTTimeStamp() allow to convert in readable GMT zone timestamp */
    struct Statistic
    {
        RTE_Nodename   nodeName;                ///< Uppercase node name (set by AddNode)
        State          nodeState;               ///< see enumeration above (set by several member functions)

        // the time stamps of the standby node specific commands
        SAPDB_UInt8    timeOfAddNode;           ///< time node was added to this list (set by AddNode)
        SAPDB_UInt8    timeOfRemoveNode;        ///< time node was removed from this list (set by RemoveNode)
        SAPDB_UInt8    timeOfPrepareStandby;    ///< time PREPARE STANDBY was received (set by GotPrepareStandby)
        SAPDB_UInt8    timeOfRegisterStandby;   ///< time REGISTER STANDBY was received (set by GotRegisterStandby)

        // the SYNCHRONIZE statistic information set by AddSynchronizeCallResult
        SAPDB_UInt8    timeOfSynchronize;       ///< time last SYNCHRONIZE was tried send to this node
        SAPDB_UInt8    timeOfLastSuccessfulSynchronize; ///< time last successfull SYNCHRONIZE
        SAPDB_UInt4    lastSavepointOffset;     ///< log offset of last savepoint from this node
        SAPDB_UInt4    lastValidLogOffsetSent;  ///< last valid log offset sent to this node
        SAPDB_Int4     synchronizeSendCount;    ///< number of successfull synchronize send
        SAPDB_Int4     synchronizeRetryCount;   ///< number of needed retries of synchronize send
    };

    /*!
      @brief          Add a node to list of known nodes
 
      To keep the time where the node list is locked via spinlock as small as possible,
      the addition of a node first reserves a slot and only if necessary extends the
      node list. The node added gets a timeOfAddNode timestamp, and the state 'Added'.

      Use this call only once per node! If it fails, you can use 'FindNode' to verify
      node does not just already exist. Otherwise you are in a out of resource situation... 

      @param     nodeName [in] node to add
      @param     nodeIndex [out] index of this node in list
      @return    true if node was successfull added or already existed in node list, false else
     */
    SAPDB_Bool AddNode(RTE_Nodename const & nodeName, SAPDB_Int4 &nodeIndex);

    /*!
      @brief          Find a node to list of known nodes

      This call does only lock the list to retrieve the reference to the 'Statistic' entry.
      It leaves the loop after the the entry is found. Doubled entries are impossible,
      since 'AddNode' always checks first...

      @param          nodeName [in] node to add
      @param          nodeIndex [out] node index in list if node exists
      @return         true if node exists, false else
     */
    SAPDB_Bool FindNode(RTE_Nodename const & nodeName, SAPDB_Int4 &nodeIndex);

    /*!
      @brief          Remove a node from list of known active nodes

      Updates timeOfRemoveNode and sets state to 'Removed'.

      @param          nodeIndex [in] index node to mark as removed (result of FindNode call)
     */
    void RemoveNode(SAPDB_Int4 nodeIndex);

    /*!
      @brief          Notify that a PREPARE STANDBY command was received for the node

      Updates timeOfPrepareStandby and sets state to 'InPrepareStandby'.

      @param          nodeIndex [in] index node to mark as removed (result of FindNode call)
     */
    void GotPrepareStandby(SAPDB_Int4 nodeIndex);

    /*!
      @brief          Notify that a REGISTER STANDBY command was received for the node

      @param          nodeIndex [in] index of node
      @param          lastSavepointOffset [in] last savepoint log offset received from node
     */
    void GotRegisterStandby( SAPDB_Int4 nodeIndex,
                             SAPDB_UInt4 lastSavepointOffset );

    /*!
      @brief          Notify that SYNCHRONIZE commands can be send since REGISTER STANDBY command was completed

      Sets state to 'WaitForSynchronize' and this way allows node to be retrieved using
      RTEHSS_NodeChain class. Does not set any time stamp.

      @param          nodeIndex [in] index node to mark as removed (result of FindNode call)
     */
    void SetWaitForSynchronize( SAPDB_Int4 nodeIndex );

    /*!
      @brief          Notify that a SYNCHRONIZE command was send to this node

      Updates timeOfSynchronize and timeOfLastSuccessfullSynchronize but does not modify state!
      The synchronizeSendCount is increased and synchronizeRetryCount is reset.

      If SYNCHRONIZE failed, synchronizeRetryCount is incresed and only timeOfSynchronize is set.

      @param          nodeIndex [in] index node to mark as removed (result of FindNode call)
      @param          callOk [in] flag to indicate synchronize call succeeded if true, retry needed if false
      @param          lastValidOffset [in] last valid log offset send to standby
      @param          lastSavepointOffset [in] last savepoint log offset received from node
     */
    void AddSynchronizeCallResult( SAPDB_Int4  nodeIndex,
                                   SAPDB_Bool  callOk,
                                   SAPDB_UInt4 lastValidOffset,
                                   SAPDB_UInt4 lastSavepointOffset );

    /*!
      @brief          Mark a node from list as deactivated

      Due to timeout etc. a node is deactivated.
      Updates timeOfRemoveNode and sets state to 'Deactivated'.

      @param          nodeIndex [in] index node to mark as removed (result of FindNode call)
     */
    void Deactivate( SAPDB_Int4  nodeIndex );

    /*!
      @brief          Return system view content for HOTSTANDBYGROUP
      @param          nodeIndex [in] index node to select (result of FindNode call)
      @return         pointer to selected node specific statistic record or NULL if invalid index
     */
    Statistic const * GetStatistic(SAPDB_Int4 nodeIndex) const
    {
        return ( ( nodeIndex < 0 || nodeIndex >= m_NodeCount ) ? (Statistic const *)0 : m_NodeList[nodeIndex] );
    }
    
    /*!
      @brief          Checks if there are node in the RTEHSS_NodeList
      @return         TRUE if there are nodes, otherwise false
     */
    SAPDB_Bool IsEmpty()
    {
        return 0 == m_NodeCount;
    }

private:
    /*!
      @brief constructor of the RTEHSS_StandbyNodeList singleton

      It registers the node list spinlock and initializes the members. Called only once and is private...
     */
	RTEHSS_StandbyNodeList();

    /*!
      @brief Fill node chain with copy of current node list

      Private function only used by friend class RTE_NodeChain.
  
      This call copies the complete NodeName entries. The list must
      be freed by caller with FreeNodeChain. The complete list and
      all its content can be freed later on with a single Deallocation
      of the returned firstInNodeChain list pointer.

      If an empty list is to be copied, true is returned but both pointers are
      set set to NULL.

      The list returned is a copy in responsibility of the caller.

      @param  allocator [in] allocator to use
      @param  firstInNodeChain [out] filled with pointer to complete list
      @param  lastInNodeChain [out] filled with pointer to last node in list
      @param  listModificationCounter [out] counter used to decide for a refill 
	  @return true if got node 8even if empty), false if copy failed
   */
    SAPDB_Bool FillNodeChain( SAPDBMem_IRawAllocator & allocator,
                              RTE_Nodename **        & firstInNodeChain,
                              RTE_Nodename **        & lastInNodeChain,
                              SAPDB_Int4             & listModificationCounter );
    /*!
      @brief Return uppercase version of node name
      @param nodeName [in] given node name
      @param uppercaseNodeName [out] uppercase converted node name
     */
    void UppercaseNode(RTE_Nodename const &nodeName, RTE_Nodename &uppercaseNodeName);

	SAPDB_Int4                      m_NodesAllocated;           ///< number of nodes allocated
	SAPDB_Int4                      m_NodeCount;                ///< the number of nodes in array
    SAPDB_Int4                      m_ListModificationCounter;  ///< the counter that counts modifications to number of active entries
	Statistic **                    m_NodeList;                 ///< the pointer to the node statistic array
	RTESync_NamedSpinlock           m_Spinlock;                 ///< spinlock used to protect node list
	static RTEHSS_StandbyNodeList * m_Instance;                 ///< the singleton hook
    friend class RTEHSS_NodeChain;
}; 

/*!
  @class          RTEHSS_NodeChain
  @brief          Container for standby nodes snapshot


  This container handles a list of known standby nodes and the actions
  to parse the list and fill it using the RTEHSS_StandbyNodeList singleton.
  A RTEHSS_NodeChain is working on its own copy of node list entries.

 */

class RTEHSS_NodeChain
{
public:
   /*!
      @brief          construct a node chain
  
      Using the user supplied allocator a complete copy of the current node chain is created, which allow to iterate independend
      of modifications on RTEHSS_StandbyNodeList singleton.

      @param allocator [in] the allocator used to copy node list
    */
    RTEHSS_NodeChain(SAPDBMem_IRawAllocator &allocator)
    : m_Allocator(allocator),
      m_FirstInNodeList(0),
      m_LastInNodeList(0),
      m_NextNodeEntryToReturn(0),
      m_LastListModificationCounter(0)
    {}

   /*!
      @brief          destruct a node chain
    */
    ~RTEHSS_NodeChain()
    {
        Empty();
    }

    /*!
       @brief          Fill the NodeChain with current content of StandbyNodeList
 
       If called again, the previous content is silently discarded. After this call
       'NextNode()' returns the first node in chain
       @return         true if filling succeeded, false if copy failed
     */

    SAPDB_Bool Fill()
    {
        SAPDB_Bool result;
        RTE_Nodename ** firstInNodeList = m_FirstInNodeList;
        RTE_Nodename ** lastInNodeList  = m_LastInNodeList;

        result = RTEHSS_StandbyNodeList::Instance().FillNodeChain( m_Allocator,
                                                                   firstInNodeList,
                                                                   lastInNodeList,
                                                                   m_LastListModificationCounter );
        if ( result 
          && (m_FirstInNodeList != firstInNodeList) )
        {
            Empty();
            m_FirstInNodeList = firstInNodeList;
            m_LastInNodeList  = lastInNodeList;
        }

        m_NextNodeEntryToReturn = ( result ? m_FirstInNodeList : 0 );
        
        return result;
    }

    /*!
       @brief          Empty the NodeChain

       The previous content is silently discarded. After this call
       all retrieved names are no longer valid. If used after this
       call, they must have been copied.
     */

    void Empty()
    {
        if ( m_FirstInNodeList )
        {
            m_Allocator.Deallocate(m_FirstInNodeList);
        }
        m_FirstInNodeList = m_LastInNodeList = m_NextNodeEntryToReturn = 0;
    }

    /*!
      @brief          Get the first node name from chain

      names become invalid with next 'Fill()' call or with destruction of NodeChain.
      Calling FirstNode resets the internal scanning, so next node will retrieve second
      entry etc.
      Since names are not copied, they must be copied by caller if used 
      Arguments: nodeName [in] next node in chain
       @return         true if got nodeName, false if chain is completely empty

     */

     SAPDB_Bool FirstNode( RTE_Nodename &nodeName )
     {
         m_NextNodeEntryToReturn = m_FirstInNodeList;
         return NextNode(nodeName);
     }

    /*!
       @brief          Get next node name from chain

       names become invalid with next 'Fill()' call or with destruction of NodeChain
       Arguments: nodeName [out] filled with next node in chain
       @return         true if got nodeName, false if chain is completely parsed (or was empty)

     */

     SAPDB_Bool NextNode( RTE_Nodename &nodeName )
     {
         SAPDB_Bool gotNodeName;

         if ( m_NextNodeEntryToReturn )
         {
             SAPDB_MemCopyNoCheck( &nodeName, *m_NextNodeEntryToReturn, sizeof(RTE_Nodename));
             gotNodeName = true;

             if ( m_NextNodeEntryToReturn == m_LastInNodeList )
             {
                 m_NextNodeEntryToReturn = 0;
             }
             else
             {
                 ++m_NextNodeEntryToReturn;
             }
         }
         else
         {
             gotNodeName = false;
         }

         return ( gotNodeName );
     }

private:
     SAPDBMem_IRawAllocator & m_Allocator;                    ///< the allocator given in constructor
     RTE_Nodename **          m_FirstInNodeList;              ///< the first node in the node list copy
     RTE_Nodename **          m_LastInNodeList;               ///< the last node in the node list copy
     RTE_Nodename **          m_NextNodeEntryToReturn;        ///< the next node to be returned
     SAPDB_Int4               m_LastListModificationCounter;  ///< the counter used to trigger refresh of node list 
}; 


#endif /* RTEHSS_STANDBYNODELIST_HPP */
