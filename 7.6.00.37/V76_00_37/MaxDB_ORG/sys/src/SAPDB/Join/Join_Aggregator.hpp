#ifndef JOIN_AGGREGATOR_HPP
#define JOIN_AGGREGATOR_HPP
/*!
 * @file
 * @brief implements an aggregator
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
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp"
#include "Table/Table_TempHashTable.hpp"
#include "Container/Container_Vector.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "Join/Join_Types.hpp"
#include "Join/Join_Trace.hpp"
#include "Join/Join_Aggregation.hpp"
#include "ggg00.h"
#include "gsp00.h"

struct tgg00_FileId;

//! implements an aggregator
class Join_Aggregator : public Table_TempHashTableAggregate_IDataMerger {
public:
    //! @name constructor / destructor
    //@{
    //! constructor
    /*! 
     * @param[in] acv global context
     * @param[in] rec_desc pointer to result record description
     * @param[in] DistFileTemplate template name for distinct files
     * @param[in] DistFileRoots vector of distinct file roots 
     */
    Join_Aggregator(
            SQLMan_Context          &acv, 
            const tgg00_StackList   &rec_desc,
            const tgg00_FileId      &DistFileTemplate,
            const Container_Vector<Join_RootPair>& DistFileRoots ) :
    m_AggrContext( acv, rec_desc, DistFileTemplate, DistFileRoots )
    {
        SAPDBTRACE_METHOD_DEBUG( "Join_Aggregator::ctor", Join_Trace, 1 );
    }
    //@}
    
    void MergeData(
            const SAPDB_Byte    *src_data,
            SAPDB_Byte          *new_or_act_data,
            SAPDB_UInt2         length,
            SAPDB_Bool          found,
            SAPDB_Bool          SourceIsPartialAggregate)
    {
        if ( ! found )
        {
            SAPDBTRACE_WRITELN( Join_Trace, 3, "length: " << length << "\trecLen_gg00: " << ((tgg00_Rec*)src_data)->recLen_gg00() );
            SAPDBERR_ASSERT_STATE( length >= ((tgg00_Rec*)src_data)->recLen_gg00() );
            SAPDB_MemCopyNoCheck( new_or_act_data, src_data, length );
            Join_Aggregation::new_group( m_AggrContext, 
                       *(tgg00_Rec*)new_or_act_data );
        }
        else
            Join_Aggregation::update_group( m_AggrContext,
                          *(tgg00_Rec*)src_data,
                          *(tgg00_Rec*)new_or_act_data,
                          SourceIsPartialAggregate );
    }

private:
    
    tgg00_BasisError new_group( tgg00_Rec&, tgg00_Rec& );
    tgg00_BasisError update_group( tgg00_Rec&, tgg00_Rec& );

    // member variables
    Join_AggregationContext     m_AggrContext;
};

#endif // JOIN_AGGREGATOR_HPP
