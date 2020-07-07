/*!

  @file           RTEHSS_PascalInterface.cpp
  @author         JoergM
  @ingroup        Runtime
  @brief          HotStandbyStorage Kernel Interface Pascal Wrapper

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

#include "RunTime/HotStandbyStorage/RTEHSS_PascalInterface.h"
#include "RunTime/HotStandbyStorage/RTEHSS_KernelInterface.hpp"
#include "RunTime/HotStandbyStorage/RTEHSS_StandbyNodeList.hpp"
#include "RunTime/HotStandbyStorage/RTEHSS_ApiDll.hpp"
#include "RunTime/RTE_KGS.hpp"
#include "RunTime/MemoryManagement/RTEMem_RteAllocator.hpp"
#include "RunTime/RTE_Message.hpp"
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

static inline void CopyNodeIdToNodename(tsp00_NodeId & nodeId, RTE_Nodename & nodeName)
{
    SAPDB_MemFillNoCheck(nodeName, 0, sizeof(RTE_Nodename));
    SAPDB_MemCopyNoCheck(nodeName, &nodeId, nodeId.length() );
}

/*!
  @brief Check if current configuration is a HotStandby configuration

  The call is needed to decide between normal and hot standby configuration.
  The call will only return false, if the configuration is completely inacceptable (i.e. 
  nodes are configured, but own node is not configured...). A normal configuration would
  not support any HotStandby API calls, but return true nevertheless...

  The call is used inside I/O runtime to decide, if log volume access is made read/only...

  @param isHotStandbyConfiguration [out] if call succeeded, true if hot standby configuration was found
  @return   true if call suceeded, false if call failed
 */ 
externC SAPDB_Bool RTEHSS_CheckConfiguration ( SAPDB_Bool & isHotStandbyConfiguration )
{
    RTEHSS_KernelInterface::Configuration found;
    SAPDBErr_MessageList errList;
    SAPDB_Bool callResult;

    callResult = RTEHSS_KernelInterface::Instance().CheckForValidHotStandbyConfiguration( found, errList );
    if ( !callResult )
    {
        RTE_Message(errList);
    }
    isHotStandbyConfiguration = ( RTEHSS_KernelInterface::ConfigurationHotStandby == found );
    return callResult;
}

/*!
   @brief    Return the answer to the question if caller is default master

   The default master is simply the node which local node name is identical with node
   name entry HS_NODE_001.
   @return   true only if caller is default master
 */
externC SAPDB_Bool RTEHSS_IsDefaultMaster ( )
{
    SAPDB_Bool isDefaultMaster = false;
    SAPDBErr_MessageList errList;

    if ( !RTEHSS_KernelInterface::Instance().IsDefaultMaster( isDefaultMaster, errList ) )
    {
        RTE_Message(errList);
    }
    return isDefaultMaster;
}

/*!
   @brief    Return current recommended log volume open mode

   This call is called by I/O runtime routines to find out, if log volumes have to opened
   read only. Note that in a 'normal configuration' this call will return an error!
   It internally checks the current hot standby role. Calling this function in a not hot
   standby configuration is handled as an error.

   @return   true only if log volumes should be opened read only
 */
externC SAPDB_Bool RTEHSS_IsLogReadOnly( )
{
    SAPDB_Bool isLogReadOnly = false;
    SAPDBErr_MessageList errList;

    if ( !RTEHSS_KernelInterface::Instance().IsLogReadOnly( isLogReadOnly, errList ) )
    {
        if ( KGS->isHotStandbyConfiguration ) // Only new errors are reported...
        {
            RTE_Message(errList);
        }
    }
    return isLogReadOnly;
}

/*!
  @brief Identify the current role as potential standby candidate for runtime support

  The call is needed to identify the master node in subsequent calls which rely on it.
  If already master role was successfully set by RTEHSS_SetRoleToMaster(), this call is rejected.
  The call will also set the log volume access to read only if possible.
 
  @param masterNodeName [in] the current master node name
  @return  true if call suceeded, false if call failed
 */ 
externC SAPDB_Bool
RTEHSS_SetRoleToStandby ( tsp00_NodeId & masterNodeName )
{
    SAPDBErr_MessageList errList;
    SAPDB_Bool callResult;
	RTE_Nodename nodeName;
    CopyNodeIdToNodename(masterNodeName, nodeName);

    callResult = RTEHSS_KernelInterface::Instance().SetRoleToStandby( nodeName, errList );

    if ( !callResult )
    {
        RTE_Message(errList);
    }
    return callResult;
}

/*!
  @brief Identify the current role as potential standby candidate for runtime support

  The call is needed to identify the master node in subsequent calls which rely on it.
  The call will also set the log volume access to read/write for master only if possible.
 
  @return   true if call suceeded, false if call failed
 */ 
externC SAPDB_Bool
RTEHSS_SetRoleToMaster ( )
{
    SAPDBErr_MessageList errList;
    SAPDB_Bool callResult;

    callResult = RTEHSS_KernelInterface::Instance().SetRoleToMaster( errList );

    if ( !callResult )
    {
        RTE_Message(errList);
    }
    return callResult;
}

/*!
   @brief    Get the mapping info string


  Returns a pointer to own mapping info string. This string contains all
  information needed to allow establishing a mirror of all data volumes.
  Mapping info is string build of Hexdigits only [0-9A-F].
   @param          ownMappingInfoSize [out] number of hex digits returned
   @param          ownMappingInfo [out] own mapping info string
   @return   true if call suceeded, false if call failed
 */
externC SAPDB_Bool
RTEHSS_GetMappingInfo(
	SAPDB_Int4    & ownMappingInfoSize,
	SAPDB_Char *  & ownMappingInfo )
{
    RTEHSS_KernelInterface::Mapping ownMapping;
    SAPDBErr_MessageList errList;
    SAPDB_Bool callResult;

    callResult = RTEHSS_KernelInterface::Instance().GetOwnMappingInfo( ownMapping, errList );

    if ( !callResult )
    {
        RTE_Message(errList);
    }
    else
    {
        ownMappingInfoSize = ownMapping.lengthInBytes;
        ownMappingInfo     = ownMapping.mappingString;
    }
    return callResult;
}

/*!
   @brief    Establish the mirror using given mapping info

   This routine triggers establishing a mirror of the data volumes using
   the given mapping information of the other machine
   @param          otherMappingInfoSize [in] number of hex digits given
   @param          otherMappingInfo [in] other machine mapping info
   @return   true if call suceeded, false if call failed
 */
externC SAPDB_Bool
RTEHSS_EstablishMirror(
	SAPDB_Int4      otherMappingInfoSize,
	SAPDB_Char *    otherMappingInfo )
{
    RTEHSS_KernelInterface::Mapping otherMapping;
    SAPDBErr_MessageList errList;
    SAPDB_Bool callResult;

    otherMapping.lengthInBytes = otherMappingInfoSize;
    otherMapping.mappingString = otherMappingInfo;

    callResult = RTEHSS_KernelInterface::Instance().EstablishMirror( otherMapping, errList );

    if ( !callResult )
    {
        RTE_Message(errList);
    }
    return callResult;
}

/*!
   @brief    Split a previously established mirror of data volumes

   This routine triggers splitting the previouly established mirror
   of the data volumes.
   @param          otherMappingInfoSize [in] number of hex digits given
   @param          otherMappingInfo [in] other machine mapping info
   @return   true if call suceeded, false if call failed
 */
externC SAPDB_Bool
RTEHSS_SplitMirror(
    SAPDB_Int4      otherMappingInfoSize,
	SAPDB_Char *    otherMappingInfo )
{
    RTEHSS_KernelInterface::Mapping otherMapping;
    SAPDBErr_MessageList errList;
    SAPDB_Bool callResult;

    otherMapping.lengthInBytes = otherMappingInfoSize;
    otherMapping.mappingString = otherMappingInfo;

    callResult = RTEHSS_KernelInterface::Instance().SplitMirror( otherMapping, errList );

    if ( !callResult )
    {
        RTE_Message(errList);
    }
    return callResult;
}

/*!
   @brief    Get state while waiting until storage is synchronized

   Synchronization means either a mirror is synchronized or a split
   has completed all outstanding I/O. If synchronized isSynchronized
   is returned as true.
   @param          otherMappingInfoSize [in] number of hex digits given
   @param          otherMappingInfo [in] other machine mapping info
   @param          isSynchronized [out] set to true if already synchronized
   @param          statusMessageLength [out] length of status message
   @param          statusMessage [out] status message string
   @return   true if call suceeded, false if call failed
 */
externC SAPDB_Bool
RTEHSS_GetStorageState( SAPDB_Int4           otherMappingInfoSize,
                        SAPDB_Char *         otherMappingInfo,
                        SAPDB_Bool         & isSynchronized,
                        SAPDB_Int4         & statusMessageLength,
                        SAPDB_Char *       & statusMessage )
{
    RTEHSS_KernelInterface::Mapping otherMapping;
    SAPDBErr_MessageList errList;
    SAPDB_Bool callResult;
    SAPDB_Char const *returnedMessage;

    otherMapping.lengthInBytes = otherMappingInfoSize;
    otherMapping.mappingString = otherMappingInfo;
    callResult = RTEHSS_KernelInterface::Instance().GetStorageState( otherMapping,
                                                                     isSynchronized, 
                                                                     returnedMessage,
                                                                     errList );
    if ( !callResult )
    {
        RTE_Message(errList);
    }
    else
    {
        statusMessage = (SAPDB_Char *)returnedMessage;
        statusMessageLength = strlen(statusMessage);
    }
    return callResult;
}
	
/*!
   @brief    Retrieve mapping information from other node

  The called instance returns its mapping info to allow establishing of a mirror or splitting of a mirror.
   @param          otherNodeName [in] the node to call
   @param          otherMappingInfoSize [out] size of master instance mapping
   @param          otherMappingInfo [out] the master instance mapping
   @return   true if call suceeded, false if call failed
 */
externC SAPDB_Bool
RTEHSS_SendInfoStandby (
    tsp00_NodeId  & otherNodeName,
    SAPDB_Int4    & otherMappingInfoSize,
    SAPDB_Char *  & otherMappingInfo )
{
    SAPDBErr_MessageList errList;
    SAPDB_Bool callResult;

    RTE_Nodename calledNodeName;
    CopyNodeIdToNodename(otherNodeName, calledNodeName);

    RTEHSS_KernelInterface::Mapping otherMapping;
    otherMapping.lengthInBytes = otherMappingInfoSize;
    otherMapping.mappingString = otherMappingInfo;

    callResult = RTEHSS_KernelInterface::Instance().SendInfoStandby( calledNodeName, 
                                                                     otherMapping,
                                                                     errList );
    if ( !callResult )
    {
        RTE_Message(errList);
    }
    else
    {
        otherMappingInfoSize = otherMapping.lengthInBytes;
        otherMappingInfo     = otherMapping.mappingString;
    }
    return callResult;
}

/*!
   @brief    Return the no longer needed mapping info
   
   This call is needed to return the storage consumed by the result of a previous SendInfoStandby or GetStorageState call
   @param          memoryStart [in] memory start
   @return   true if call suceeded, false if call failed
 */
externC SAPDB_Bool
RTEHSS_FreeMemory ( SAPDB_Char *  memoryStart )
{
    SAPDB_Bool callResult;
    SAPDBErr_MessageList errList;

    callResult = RTEHSS_KernelInterface::Instance().FreeMemory( memoryStart, errList );
    if ( !callResult )
    {
        RTE_Message(errList);
    }
    return callResult;
}

/*!
   @brief    Prepare the standby mode establishing

   This routine will first retrieve mapping info from master node.
   After that it will establish the mirror using the retrieved mapping
   and its own mapping. After that the PREPARE STANDBY command is
   send to the master and the call blocks until the answer is received.
   This call is issued by standby nodes only during INIT STANDBY command execution.

   @return   true if call suceeded, false if call failed
 */
externC SAPDB_Bool 
RTEHSS_SendPrepareStandby( )
{
    SAPDBErr_MessageList errList;
    SAPDB_Bool callResult;

    callResult = RTEHSS_KernelInterface::Instance().SendPrepareStandby( errList );
    if ( !callResult )
    {
        RTE_Message(errList);
    }
    return callResult;
}

/*!
   @brief    Send command to register standby node into internal list of master node

  Send from standby candidate to master instance.
  Since this call always targets the master node name, which was already supplied in a previous 
  RTEHSS_SetRoleToStandby() call the parameter is not needed.

   @param          firstStillNeededOffset [in] first still needed offset of standby node
   @param          lastValidOffset [out] last valid offset position to read
   @return   true if call suceeded, false if call failed
 */ 
externC SAPDB_Bool
RTEHSS_SendRegisterStandby (
    SAPDB_Int4     firstStillNeededOffset, 
    SAPDB_Int4   & lastValidOffset )
{
    SAPDBErr_MessageList errList;
    SAPDB_Bool callResult;
    SAPDB_UInt4 resultOffset;

    callResult = RTEHSS_KernelInterface::Instance().SendRegisterStandby( (SAPDB_UInt4)firstStillNeededOffset,
                                                                         resultOffset,
                                                                         errList );
    if ( !callResult )
    {
        RTE_Message(errList);
    }
    else
    {
        lastValidOffset = (SAPDB_Int4)resultOffset; /* Pascal cannot handle SAPDB_UInt4... it still needs tsp00_Int4 */
    }
    return callResult;
}

/*!
   @brief    Send command to insert standby node into internal list of given node

   @param          nodeName [in] node to send command
   @param          newStandbyNodeName [in] node name to insert
   @return   true if call suceeded, false if call failed
 */
externC SAPDB_Bool
RTEHSS_SendInsertStandby (
	tsp00_NodeId  & nodeName,
	tsp00_NodeId  & newStandbyNodeName )
{
    SAPDBErr_MessageList errList;
    SAPDB_Bool callResult;

    RTE_Nodename calledNodeName;
    CopyNodeIdToNodename(nodeName, calledNodeName);

    RTE_Nodename addedNodeName;
    CopyNodeIdToNodename(newStandbyNodeName, addedNodeName);

    callResult = RTEHSS_KernelInterface::Instance().SendInsertStandby( calledNodeName,
                                                                       addedNodeName,
                                                                       errList );
    if ( !callResult )
    {
        RTE_Message(errList);
    }
    return callResult;
}

/*!
   @brief    Send command to remove standby node from internal list of given node

   @param          nodeName [in] node to send command
   @param          oldStandbyNodeName [in] node name to remove
   @return   true if call suceeded, false if call failed
 */
externC SAPDB_Bool
RTEHSS_SendRemoveStandby (
	tsp00_NodeId  & nodeName,
	tsp00_NodeId  & oldStandbyNodeName )
{
    SAPDBErr_MessageList errList;
    SAPDB_Bool callResult;

    RTE_Nodename calledNodeName;
    CopyNodeIdToNodename(nodeName, calledNodeName);

    RTE_Nodename removedNodeName;
    CopyNodeIdToNodename(oldStandbyNodeName, removedNodeName);

    callResult = RTEHSS_KernelInterface::Instance().SendRemoveStandby( calledNodeName,
                                                                       removedNodeName,
                                                                       errList );
    if ( !callResult )
    {
        RTE_Message(errList);
    }
    return callResult;
}

/*!
   @brief    Send command to exchange LOG-volume synchronization information

   Send from master instance to standby instance.
   @param          nodeName [in] node to send command
   @param          lastValidOffset [in] last valid offset position to transfer
   @param          firstStillNeededOffset [out] first still needed offset of 'nodename'
   @return   true if call suceeded, false if call failed
 */
externC SAPDB_Bool
RTEHSS_SendSynchronize(
	tsp00_NodeId & nodeName,
    SAPDB_Int4     lastValidOffset, 
    SAPDB_Int4   & firstStillNeededOffset )
{
    SAPDBErr_MessageList errList;
    SAPDB_Bool callResult;
    SAPDB_UInt4 resultOffset;
    RTE_Nodename calledNodeName;
    CopyNodeIdToNodename(nodeName, calledNodeName);

    callResult = RTEHSS_KernelInterface::Instance().SendSynchronize( calledNodeName,
                                                                     (SAPDB_UInt4)lastValidOffset,
                                                                     resultOffset,
                                                                     errList );
    if ( !callResult )
    {
        RTE_Message(errList);
    }
    else
    {
        firstStillNeededOffset = (SAPDB_Int4)resultOffset; /* Pascal cannot handle SAPDB_UInt4... it still needs tsp00_Int4 */
    }
    return callResult;
}
