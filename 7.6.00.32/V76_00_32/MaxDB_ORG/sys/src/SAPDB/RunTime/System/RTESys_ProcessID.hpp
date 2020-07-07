/*!========================================================================

  @file         RTESys_ProcessID.hpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        Determine the current process id

  @since        2003-10-22  14:23
  @sa           

  ==========================================================================

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
 ============================================================================*/


#ifndef RTESYS_PROCESSID_HPP
#define RTESYS_PROCESSID_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/RTE_Types.h"

#if defined (_WIN32) 
# include <windows.h>
#else
# include "RunTime/RTE_saveUNIXcalls.h" /* nocheck */
#endif


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
  @brief    Get process id.

  Get the ID of the current process.

  @return   process id
*/
inline RTE_OSPid RTESys_ProcessID()
{
#if defined (_WIN32) 
    return (RTE_OSPid)GetCurrentProcessId();
#else
    return (RTE_OSPid)RTE_save_getpid();
#endif
}


#endif  /* RTESYS_PROCESSID_HPP */
