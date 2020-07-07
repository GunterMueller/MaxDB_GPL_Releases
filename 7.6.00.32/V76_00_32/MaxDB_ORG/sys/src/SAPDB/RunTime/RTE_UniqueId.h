/*!***************************************************************************

  module      : RTE_UniqueId.h

  -------------------------------------------------------------------------

  responsible : JoergM

  special area: RunTime
  description : Unique Id Generator

  On WIN32 you need to link with libole32 to get reference to CoCreateGuid() resolved.

  last changed: 2001-05-22  16:20
  see also    : 

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2001-2005 SAP AG

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


*****************************************************************************/


#ifndef RTE_UNIQUEID_H
#define RTE_UNIQUEID_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/SAPDB_C_Cplusplus.h"
#include "RunTime/RTE_Types.h"

/*===========================================================================*
 *  PROTOTYPE, TYPES                                                         *
 *===========================================================================*/

typedef struct RTE_UniqueId_
{
    SAPDB_UInt4 sequenceId;        /* spinlock protected counter */
    SAPDB_UInt4 secondsSinceEpoch; /* number of seconds since 1.1.1990 GMT */
    SAPDB_UInt4 processId;         /* fixed process id info */
    SAPDB_UInt4 randomValue;       /* some random value */
    SAPDB_UInt8 machineId;         /* some fixed machine info */
} RTE_UniqueId; /* 24 bytes... */

typedef SAPDB_UTF8 RTE_UniqueIdBuffer[sizeof(RTE_UniqueId)*2+4+1]; /* 53 bytes */

/*!
  Function: RTE_InitIdGenerator
  Description: Initialize resources for Id Generator

  This call gets all fixed information (processId and machineId) of RTE_UniqueId 
  and initializes the Spinlock used to protect sequenceId and generates the
  random number used in all uniqueIds of this process and its subprocesses.

  This call is not multi thread save!
 */
externC void RTE_InitIdGenerator();

/*!
  Function: RTE_FillUniqueId
  Description: Fills Unique id

  Every call to this function returns a different id. This is due to sequenceId
  and timestamp (GMT seconds since 1.1.1990).

  Before any call to this function is issued, RTE_InitIdGenerator MUST be called.
  Otherwise the call will return false. This call is multi thread save.

  Arguments: pId [out] id to fill
  Return value: true if unique id was returned, false if not

  Hint in case of trouble check pId and make sure RTE_InitIdGenerator was called!
 */
externC bool RTE_FillUniqueId(RTE_UniqueId *pId);

/*!
  Function: RTE_FillUniqueId
  Description: Dump Unique id as hex digits

  Dump is separated by hyphen into five distinct parts.
  If you use the given type, a zero terminated string is returned.

  SequenceId-Timestamp-ProcessId-RandomValue-MachineId

  Arguments: pId [in] id to fill
             buffer [out] buffer to fill
 */
externC void RTE_DumpUniqueId(const RTE_UniqueId *pId, RTE_UniqueIdBuffer buffer);

/*!
  Function: RTE_DumpUniqueId
  Description: Generate a Unique ID as hex digits 

  This function combines a call to RTE_FillUniqueId followed by a call to RTE_FillUniqueId
  (see above)

  Arguments: uniqueIDName [out] buffer to fill

  Return value:   true/false
*/
externC SAPDB_Bool RTE_GetUID(RTE_UniqueIdBuffer    uniqueIDName);

#endif  /* RTE_UNIQUEID_H */
