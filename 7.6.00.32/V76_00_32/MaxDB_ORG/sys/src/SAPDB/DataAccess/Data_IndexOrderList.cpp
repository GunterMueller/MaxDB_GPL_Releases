/*****************************************************************************/
/*!
  @file         Data_IndexOrderList.cpp
  @author       MartinKi
  @ingroup      DataAccess

  @brief        Implementation file for class Data_IndexOrderList.

\if EMIT_LICENCE
  ========== licence begin  GPL
  Copyright (C) 2005 SAP AG

  This program is free software; you can redisytribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
  ========== licence end
\endif
*/
/*****************************************************************************/


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "DataAccess/Data_IndexOrderList.hpp"

#include "Container/Container_Stack.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"

#include "hgg01_3.h" // g01vtrace

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

const SAPDB_UInt cMinCapacity = 4;

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/**************************************************************************/

void
Data_IndexOrderList::WriteToVtrace(const SAPDB_Bool force) const
{
    if ( (! force ) && (! g01vtrace.vtrBdIndex_gg00) ) {
        return;
    }

    Kernel_VTrace trace;
    trace << "order count: " << this->GetSize() << NewLine;

    for ( SAPDB_UInt orderNo = 0; orderNo < this->GetSize(); ++orderNo ) {
        trace << "=== OrderNo " << orderNo + 1 << ": ";

        switch ( this->operator[]( orderNo ).txoOp_bd00 ) {
            case opAdd_ebd00:
                trace << "Add Separator";
                break;
            case opDel_ebd00:
                trace << "Del Separator";
                break;
            case opRepl_ebd00:
                trace << "Repl Separator";
                break;
            default:
                trace << "Unknown";
        }
        trace << NewLine;
        trace.Flush();
    }
}

/**************************************************************************/
