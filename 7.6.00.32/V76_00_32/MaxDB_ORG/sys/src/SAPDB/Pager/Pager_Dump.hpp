/*!
  @file           Pager_Dump.hpp
  @author         Henrik
  @brief          Layout of pager dump structures
  @see            

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

*/



#ifndef PAGER_DUMP_HPP
#define PAGER_DUMP_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

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

/*! 
    @struct PagerDump
    @brief  data structure for dump of pager
*/
struct PagerDump 
{
    /// number of pager_writer
    SAPDB_UInt4     dmpNumPagerWriter;
    /// number of registered pager_writer
    SAPDB_UInt4     dmpRegisteredPagerWriter;
    /// number of suspended pager_writer
    SAPDB_UInt4     dmpSuspendPagerWriter;
    /// current pager_writer order
    SAPDB_UInt4     dmpPagerOrder;
    /// flag for active savepoint
    SAPDB_Bool1     dmpSVPIsActive;
    /// flag for active flushing
    SAPDB_Bool1     dmpFlushIsActive;
    /// pager_writer is in prepare savepoint phase
    SAPDB_Bool1     dmpPreparePagerWriter;
    /// pager_writer is disabled
    SAPDB_Bool1     dmpPagerWriterDisabled;
};

#endif  /* PAGER_DUMP_HPP */
