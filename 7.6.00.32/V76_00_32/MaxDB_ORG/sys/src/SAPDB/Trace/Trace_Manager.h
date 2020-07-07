/*!
  @file     Trace_Manager.h
  @author   TorstenS
  @ingroup  Trace
  @brief    This class defines the singleton component trace manager and 
            offers the possibility to insert trace entries into the UKT 
            specific memory trace buffers. Furthermore it's possible to
            write the materialized trace entries into the kernel trace file
            The writeable trace entries are derived from the abstract
            classes Trace_IBaseEntry and Trace_ILargeEntry found in 
            Trace_Types.hpp. The concretely implementations of the trace
            entries are in Trace_[A-Z*]Entry.hpp, e.g.Trace_PacketEntry.hpp.

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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



#ifndef TRACE__MANAGER_H
#define TRACE__MANAGER_H


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"   // PASCAL: SP_basic_constants_and_types

#include "SAPDBCommon/SAPDB_Types.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

externC SAPDB_Bool Trace_Flush( const tsp00_TaskId taskId );


#endif  /* TRACE__MANAGER_H */
