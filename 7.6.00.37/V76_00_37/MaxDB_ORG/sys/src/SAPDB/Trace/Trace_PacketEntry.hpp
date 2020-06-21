/*!**************************************************************************

  module      : Trace_PacketEntry.hpp

  -------------------------------------------------------------------------

  responsible : TorstenS

  special area: Handling of SAPDB order packet trace entries
  description : 

  last changed: 2000-11-27  18:15
  see also    :

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2000-2005 SAP AG




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




*****************************************************************************/


#ifndef TRACE_PACKETENTRY_HPP
#define TRACE_PACKETENTRY_HPP


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "ggg00.h"

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "Trace/Trace_Entry.hpp"
#include "Trace/Trace_Manager.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/


/*===========================================================================*
 *  IMPLEMENTATION OF METHODS                                                *
 *===========================================================================*/

class Trace_PacketEntry : public Trace_LargeEntry
{
public:

    // Use implementaion of Trace_LargeEntry. Only an constructor and
    // a destructor is added. The destructor is needed to insert the
    // trace entry into the trace.

    Trace_PacketEntry(
        tgg00_Debug         Layer,
        tgg00_VtraceType    Type,
        tsp00_BytePtr       pEntry,
        SAPDB_Int4          EntrySize )
    :
    Trace_LargeEntry( Layer, Type, pEntry, EntrySize )
    {}

    ~Trace_PacketEntry(){
        Trace_Manager::GetInstance().InsertLarge( *this );
    }
};

#endif  /* TRACE_PACKETENTRY_HPP */
