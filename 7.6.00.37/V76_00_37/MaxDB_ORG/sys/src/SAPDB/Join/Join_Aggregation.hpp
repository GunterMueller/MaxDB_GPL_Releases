#ifndef JOIN_AGGREGATION_HPP
#define JOIN_AGGREGATION_HPP
/*!
 * @file
 * @brief implements functions for aggregations
 *
 * @author GertG
 * @ingroup Join
 *
 */
/*

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



*/

#include "Join/Join_Types.hpp"
#include "Container/Container_Vector.hpp"
#include "ggg00.h"
#include "ggg07.h"

namespace Join_Aggregation {
tgg00_BasisError new_group( 
        Join_AggregationContext&, 
        tgg00_Rec&);
tgg00_BasisError update_group( 
        Join_AggregationContext&, 
        tgg00_Rec&, 
        tgg00_Rec&,
        SAPDB_Bool);
tgg00_BasisError post_processing( 
        Join_AggregationContext&,
        tgg00_Rec&, 
        const SAPDB_UInt4,
        const SAPDB_UInt4,
        const Container_Vector<SAPDB_Int4>&,
        const tgg07_setfunc_handling_Enum );
tgg00_BasisError initialize( 
        SQLMan_Context&,
        const tgg00_StackList&,
        tgg00_FileId&,
        Container_Vector<Join_RootPair>&,
        const tgg00_FileId&,
        SAPDB_UInt4&,
        tgg07_setfunc_handling_Enum&);
}

#endif // JOIN_AGGREGATION_HPP
