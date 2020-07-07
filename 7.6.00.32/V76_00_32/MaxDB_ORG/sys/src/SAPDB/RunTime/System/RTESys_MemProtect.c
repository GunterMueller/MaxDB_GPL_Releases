/*!---------------------------------------------------------------------
  @file           RTESys_MemProtect.c
  @author         RaymondR
  @brief          Memory Protection
  @see            

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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
---------------------------------------------------------------------*/




/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#if defined(_WIN32)
#include <windows.h>
#else
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/mman.h>
#endif

#include "RunTime/RTE_Types.h"
#include "RunTime/System/RTESys_MemProtect.h"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  STRUCTURES, TYPES, UNIONS ...                                            *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/



/*===========================================================================*
 *  FUNCTIONS                                                                *
 *===========================================================================*/

SAPDB_ULong RTESys_MemProtect( void *pvAdr, SAPDB_ULong ulSize, SAPDB_Int type )
{
	int protection;

#if defined(_WIN32)
	SAPDB_ULong ulOldProtect;

	switch(type)
	{
	case MEMORYPROTECTION_READWRITE:
		protection = PAGE_READWRITE;
		break;
	case MEMORYPROTECTION_NOACCESS:
		protection = PAGE_NOACCESS;
		break;
	case MEMORYPROTECTION_READONLY:
		protection = PAGE_READONLY;
		break;
	}

	if(VirtualProtect(pvAdr,ulSize,protection,&ulOldProtect))
		return NO_ERROR;
	else
		return GetLastError();

#else /* UNIX */

	switch(type)
	{
	case MEMORYPROTECTION_READWRITE:
		protection = PROT_READ|PROT_WRITE;
		break;
	case MEMORYPROTECTION_NOACCESS:
		protection = PROT_NONE;
		break;
	case MEMORYPROTECTION_READONLY:
		protection = PROT_READ;
		break;
	}
	if(mprotect((char *)pvAdr,ulSize,protection)==0)
		return 0;
	else
		return errno;

#endif
}

/* ---------------------------------------------*/

SAPDB_ULong RTESys_MemProtectReadWrite( void *pvAdr, SAPDB_ULong ulSize )
{
  return RTESys_MemProtect( pvAdr, ulSize, MEMORYPROTECTION_READWRITE );
}

/* ---------------------------------------------*/

SAPDB_ULong RTESys_MemProtectNoAccess( void *pvAdr, SAPDB_ULong ulSize )
{
  return RTESys_MemProtect( pvAdr, ulSize, MEMORYPROTECTION_NOACCESS );
}

/* ---------------------------------------------*/

SAPDB_ULong RTESys_MemProtectReadOnly( void *pvAdr, SAPDB_ULong ulSize )
{
  return RTESys_MemProtect( pvAdr, ulSize, MEMORYPROTECTION_READONLY );
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/