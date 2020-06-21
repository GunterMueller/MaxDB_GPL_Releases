/*!**************************************************************************

  module      : Trace_Wrapper.cpp

  -------------------------------------------------------------------------

  responsible : TorstenS

  special area:
  description : 

  last changed: 2000-12-21  13:48
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



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"   // PASCAL: SP_basic_constants_and_types
#include "ggg00.h"   // PASCAL: GG_kernel_constants_and_types
#include "gsp03.h"
#include "hgg01_1.h"


#include "SAPDBCommon/SAPDB_Types.hpp"
#include "Trace/Trace_Entry.hpp"
#include "Trace/Trace_Manager.hpp"
#include "Trace/Trace_MessBlockEntry.hpp"
#include "Trace/Trace_PacketEntry.hpp"
#include "Trace/Trace_Writer.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define ALLOCATE_FAILED_MSG1 "TraceManager: bad alloc "
#define ALLOCATE_FAILED_MSG2 "TraceWriter: bad alloc  "

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

externC void b120ClearTrace( tsp00_TaskId TaskId )
{
    Trace_Manager::GetInstance().PrepareAndWaitForFlush( TaskId, true );
};

/*---------------------------------------------------------------------------*/

externC void b120FlushTrace( tsp00_TaskId TaskId )
{
    Trace_Manager::GetInstance().PrepareAndWaitForFlush( TaskId );
}

/*---------------------------------------------------------------------------*/

externC void b120MessBlockTrace (
    tgg00_TransContext    &Trans,
    tgg00_VtraceType_Param Type,
    tgg00_MessBlock       &MessBlock )
{
    tgg00_Debug Layer = tgg00_Debug::fromConst( ak_messbuf );

    Trace_MessBlockEntry Entry( MessBlock, Layer,
                                tgg00_VtraceType::fromConst(Type) );
};

/*---------------------------------------------------------------------------*/

externC void b120MoveObjectTrace(
    tgg00_TransContext &Trans,
    tgg00_Debug_Param  Layer,
    tgg00_VtraceType_Param Type,
    SAPDB_Int4         Length,
    tsp00_BytePtr      pEntry )
{
    Trace_PacketEntry Entry( tgg00_Debug::fromConst(Layer),
                             tgg00_VtraceType::fromConst(Type), pEntry, Length );
};

/*---------------------------------------------------------------------------*/

externC void b120InsertTrace(
    tgg00_TransContext   &Trans,
    tgg00_Debug_Param    Layer,
    tgg00_VtraceType_Param Type,
    SAPDB_Int4           Length,
    tsp00_BytePtr        pEntry )
{
    Trace_CommonEntry Entry( tgg00_Debug::fromConst(Layer),
                             tgg00_VtraceType::fromConst(Type), pEntry, Length );
};

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
