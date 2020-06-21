/*!***************************************************************************

  module      : RTE_UniqueId.c

  -------------------------------------------------------------------------

  responsible : JoergM

  special area: RunTime
  description : Unique Id Generator


  last changed: 2001-05-25  13:53
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



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include "RunTime/RTE_UniqueId.h"
#include "RunTime/System/RTESys_Spinlock.h"
#include "SAPDBCommon/SAPDB_string.h"
#if defined(_WIN32)
# include <windows.h>
#else
# include <unistd.h>
#endif
# include <stdlib.h>

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  STRUCTURES, TYPES, UNIONS ...                                            *
 *===========================================================================*/

static RTE_Lock     uniqueIdSpinlock;
static RTE_UniqueId lastId = { 0 };

/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/

static SAPDB_UTF8 * FillHexString(SAPDB_UTF8 *ptr, const SAPDB_Byte *val, SAPDB_Int4 len);
static SAPDB_UTF8 * FillHexUInt4(SAPDB_UTF8 *ptr, SAPDB_UInt4 val);

/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

/* Not multithread save! Must be called in serial code */
externC void RTE_InitIdGenerator()
{
    RTESys_InitLock(&uniqueIdSpinlock);

    lastId.sequenceId = 0;
#if defined(_WIN32)
    lastId.processId = (SAPDB_UInt4)GetCurrentProcessId();
    {
        union
        {
            FILETIME now;
            SAPDB_UInt8 copyValue;
        } u;
        u.copyValue = 0;

        GetSystemTimeAsFileTime(&u.now);
        srand((unsigned int)u.copyValue);
    }
    lastId.randomValue = (SAPDB_UInt4)rand();
    {
        GUID myGuid;
        CoCreateGuid(&myGuid);

        /* last 6 characters in GUID are machine specific id */
        SAPDB_memcpy(&lastId.machineId, myGuid.Data4, sizeof(lastId.machineId));
    }
#else
    lastId.processId = (SAPDB_UInt4)getpid();
    srand((unsigned int)time((time_t*)0));
    lastId.randomValue = (SAPDB_UInt4)rand();
    lastId.machineId = (SAPDB_UInt8)gethostid();
#endif
    lastId.secondsSinceEpoch = 0;
}

/*---------------------------------------------------------------------------*/

externC bool RTE_FillUniqueId(RTE_UniqueId *pId)
{
    if ( lastId.processId == 0 
       && lastId.randomValue == 0
       && lastId.machineId == 0 )
    {
        return false;
    }

    /* Sequence number is spinlocked */
    while ( RTESys_TestAndLock(&uniqueIdSpinlock) )
    {
        ;
    }

    pId->sequenceId        = ++lastId.sequenceId;

    RTESys_Unlock(&uniqueIdSpinlock);

    /* timestamp generation is system dependend... */
#if defined(_WIN32)
    {
        /* 
           Ok... it is not rearly seconds since epoch,
           but 100-nanosecond intervals since 1.1.1601.
           But we need a unique number only, not an exact number...
         */
        union
        {
            FILETIME now;
            SAPDB_UInt8 copyValue;
        } u;
        u.copyValue = 0;

        GetSystemTimeAsFileTime(&u.now);
        pId->secondsSinceEpoch = (SAPDB_UInt4)((u.copyValue/10000)/1000);
    }
#else
    pId->secondsSinceEpoch = (SAPDB_UInt4)time((time_t *)0);
#endif

    /* copy fixed values */
    pId->processId         =   lastId.processId;
    pId->randomValue       =   lastId.randomValue;
    pId->machineId         =   lastId.machineId;

    return true;
}

/*---------------------------------------------------------------------------*/

externC void RTE_DumpUniqueId(const RTE_UniqueId *pId, RTE_UniqueIdBuffer buffer)
{
    SAPDB_UTF8 *ptr = &buffer[0];

    ptr = FillHexUInt4(ptr, pId->sequenceId);
    *ptr++ = (SAPDB_UTF8)'-';
    ptr = FillHexUInt4(ptr, pId->secondsSinceEpoch);
    *ptr++ = (SAPDB_UTF8)'-';
    ptr = FillHexUInt4(ptr, pId->processId);
    *ptr++ = (SAPDB_UTF8)'-';
    ptr = FillHexUInt4(ptr, pId->randomValue);
    *ptr++ = (SAPDB_UTF8)'-';
    ptr = FillHexString(ptr, (SAPDB_Byte *)&pId->machineId, sizeof(SAPDB_UInt8));
    *ptr   = (SAPDB_UTF8)0;
}

/*---------------------------------------------------------------------------*/

externC SAPDB_Bool RTE_GetUID 
    (
        RTE_UniqueIdBuffer                   uniqueIDName
    )
{
    RTE_UniqueId        uniqueID;
    
    if (!RTE_FillUniqueId(&uniqueID))
    {
        return false;
    }

    RTE_DumpUniqueId (&uniqueID, uniqueIDName);
    return true;
}

/*---------------------------------------------------------------------------*/

static SAPDB_UTF8 * FillHexUInt4(SAPDB_UTF8 *ptr, SAPDB_UInt4 val)
{
    SAPDB_UTF8 *tmpPtr = ptr;
    SAPDB_Int4  len = sizeof(SAPDB_UInt4);

    ptr += (2*len);

    while ( len-- > 0 )
    {
        tmpPtr[2*len]   = (((val)&0x0F0)>>4)["0123456789abcdef"];
        tmpPtr[2*len+1] =   ((val)&0x0F)    ["0123456789abcdef"];
        val >>= 8;
    }

    return ptr;
}

/*---------------------------------------------------------------------------*/

static SAPDB_UTF8 * FillHexString(SAPDB_UTF8 *ptr, const SAPDB_Byte *val, SAPDB_Int4 len)
{
    while ( len-- > 0 )
    {
        *ptr++ = (((*val)&0x0F0)>>4)["0123456789abcdef"];
        *ptr++ =   ((*val)&0x0F)    ["0123456789abcdef"];
        ++val;
    }
    return ptr;
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
