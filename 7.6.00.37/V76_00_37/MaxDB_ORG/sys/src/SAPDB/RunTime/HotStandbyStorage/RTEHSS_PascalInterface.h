/*!
  @file           RTEHSS_PascalInterface.h
  @author         JoergM
  @ingroup        Runtime
  @brief          HotStandbyStorage Kernel Interface

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



#ifndef RTEHSS_PASCALINTERFACE_H
#define RTEHSS_PASCALINTERFACE_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/RTE_CompilerFeatures.h"
#include "SAPDBCommon/SAPDB_Types.h"
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

/*!
  @brief check the current configuration

  The call is needed to decide between normal and hot standby configuration.
  The call will only return false, if the configuration is completely inacceptable (i.e. 
  nodes are configured, but own node is not configured...). A normal configuration would
  not support any HotStandby API calls, but return true nevertheless...

  @param isHotStandbyConfiguration [out] if call succeeded, true if hot standby configuration was found
  @return   true if call suceeded, false if call failed
 */ 
externC SAPDB_Bool RTEHSS_CheckConfiguration ( SAPDB_Bool VAR_VALUE_REF isHotStandbyConfiguration );

/*!
   @brief    Return the answer to the question if caller is default master

   The default master is simply the node which local node name is identical with node
   name entry HS_NODE_001.
   @return   true if caller is default master
 */
externC SAPDB_Bool RTEHSS_IsDefaultMaster ( );

/*!
   @brief    Return current recommended log volume open mode

   This call is called by I/O runtime routines to find out, if log volumes have to opened
   read only. Note that in a 'normal configuration' this call will return an error!
   It internally checks the current hot standby role. Calling this function in a not hot
   standby configuration is handled as an error.

   @return   true if log volumes should be opened read only
 */
externC SAPDB_Bool RTEHSS_IsLogReadOnly( );

/*!
  @brief Identify the current role as potential standby candidate for runtime support

  The call is needed to identify the master node in subsequent calls which rely on it.
  If already master role was successfully set by RTEHSS_SetRoleToMaster(), this call is rejected.
  The call will also set the log volume access to read only if possible.
 
  @param masterNodeName [in] the current master node name
  @return  true if call suceeded, false if call failed
 */ 
externC SAPDB_Bool
RTEHSS_SetRoleToStandby ( tsp00_NodeId VAR_ARRAY_REF masterNodeName );

/*!
  @brief Identify the current role as potential standby candidate for runtime support

  The call is needed to identify the master node in subsequent calls which rely on it.
  The call will also set the log volume access to read/write for master only if possible.
 
  @return   true if call suceeded, false if call failed
 */ 
externC SAPDB_Bool RTEHSS_SetRoleToMaster ( );

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
	SAPDB_Int4   VAR_VALUE_REF ownMappingInfoSize,
	SAPDB_Char * VAR_VALUE_REF ownMappingInfo );

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
	SAPDB_Char *    otherMappingInfo );

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
	SAPDB_Char *    otherMappingInfo );

/*!
   @brief    Get state while waiting until storage is synchronized

   Synchronization means either a mirror is synchronized or a split
   has completed all outstanding I/O. If synchronized isSynchronized
   is returned as true. The status message must be free after use using
   a RTEHSS_FreeMemory call.
   @param          otherMappingInfoSize [in] number of hex digits given
   @param          otherMappingInfo [in] other machine mapping info
   @param          isSynchronized [out] set to true if already synchronized
   @param          statusMessageLength [out] status message string length in bytes
   @param          statusMessage [out] status message string
   @return   true if call suceeded, false if call failed
 */
externC SAPDB_Bool
RTEHSS_GetStorageState( SAPDB_Int4                       otherMappingInfoSize,
                        SAPDB_Char *                     otherMappingInfo,
                        SAPDB_Bool         VAR_VALUE_REF isSynchronized,
                        SAPDB_Int4         VAR_VALUE_REF statusMessageLength,
                        SAPDB_Char       * VAR_VALUE_REF statusMessage );
	
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
    tsp00_NodeId  VAR_ARRAY_REF otherNodeName,
    SAPDB_Int4    VAR_VALUE_REF otherMappingInfoSize,
    SAPDB_Char *  VAR_VALUE_REF otherMappingInfo );

/*!
   @brief    Return the no longer needed mapping info
   
   This call is needed to return the storage consumed by the result of a previous SendInfoStandby or GetStorageState call
   @param          memoryStart [in] memory start
   @return   true if call suceeded, false if call failed
 */
externC SAPDB_Bool
RTEHSS_FreeMemory ( SAPDB_Char *  memoryStart );

/*!
   @brief    Inform master node that standby mode may begin

  Send from standby candidate to master instance.
  The master node is informed, that its standby mode may begin now. This call will trigger
  the save point and mirror split on the master instance site.
  It includes informing all other nodes, which are already in standby mode about a new node in
  standby mode (but only if beginning standby mode is confirmed). Any hosts, that did
  not react on this information are removed from local list of standby nodes...

  Since this call always targets the master node name, which was already supplied in a previous 
  RTEHSS_SetRoleToStandby() call the parameter is not needed.

   @return   true if call suceeded, false if call failed
 */
 externC SAPDB_Bool
RTEHSS_SendPrepareStandby ( );

/*!
   @brief    Send command to register standby node into internal list of master node

  Send from standby candidate to master instance.
  Since this call always targets the master node name, which was already supplied in a previous 
  RTEHSS_SetRoleToStandby() call the parameter is not needed.

  Pascal code does not know about differenz between signed and unsigned integer. Therefore the interface
  uses SAPDB_Int4 for LogOffsets, but knows it handles SAPDB_UInt4...

   @param          firstStillNeededOffset [in] first still needed offset of standby node
   @param          lastValidOffset [out] last valid offset position to read
   @return   true if call suceeded, false if call failed
 */ 
externC SAPDB_Bool
RTEHSS_SendRegisterStandby (
    SAPDB_Int4                 firstStillNeededOffset, 
    SAPDB_Int4   VAR_VALUE_REF lastValidOffset );

/*!
   @brief    Send command to insert standby node into internal list of given node

   @param          nodeName [in] node to send command
   @param          newStandbyNodeName [in] node name to remove
   @return   true if call suceeded, false if call failed
 */
externC SAPDB_Bool
RTEHSS_SendInsertStandby (
	tsp00_NodeId  VAR_ARRAY_REF nodeName,
	tsp00_NodeId  VAR_ARRAY_REF newStandbyNodeName );

/*!
   @brief    Send command to remove standby node from internal list of given node

   @param          nodeName [in] node to send command
   @param          oldStandbyNodeName [in] node name to remove
   @return   true if call suceeded, false if call failed
 */
externC SAPDB_Bool
RTEHSS_SendRemoveStandby (
	tsp00_NodeId  VAR_ARRAY_REF nodeName,
	tsp00_NodeId  VAR_ARRAY_REF oldStandbyNodeName );

/*!
   @brief    Send command to exchange LOG-volume synchronization information

   Send from master instance to standby instance.

   Pascal code does not know about differenz between signed and unsigned integer. Therefore the interface
   uses SAPDB_Int4 for LogOffsets, but knows it handles SAPDB_UInt4...

   @param          nodeName [in] node to send command
   @param          lastValidOffset [in] last valid offset position to transfer
   @param          firstStillNeededOffset [out] first still needed offset of 'nodename'
   @return   true if call suceeded, false if call failed
 */
externC SAPDB_Bool
RTEHSS_SendSynchronize(
	tsp00_NodeId  VAR_ARRAY_REF nodeName,
    SAPDB_Int4                  lastValidOffset, 
    SAPDB_Int4    VAR_VALUE_REF firstStillNeededOffset );

#endif /* RTEHSS_PASCALINTERFACE_H */
