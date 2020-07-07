#ifndef JOIN_RESULTSETDISTINCTONERESULT_HPP
#define JOIN_RESULTSETDISTINCTONERESULT_HPP
/*!
 * @file
 * @brief implements interface Join_IResultSetOperator which contains only 
 * one record; don't complain about identical records
 *
 * @author GertG
 * @ingroup Join
 *
 * @sa Join_Execute.cpp, Join_IResultSetOperator.hpp
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
#include "Join/Join_ResultSetOneResult.hpp"

class SQLMan_Context;

//! implements interface Join_IResultSetOperator which contains only one record; don't complain about identical records
class Join_ResultSetDistinctOneResult : public Join_ResultSetOneResult {
public:
    //! @name constructor / destructor
    //@{
    //! constructor
    /*! 
     * @param[in] acv global context
     * @param[in] rowno_pos positions of ROWNO columns
     * @param[in] rec_producer Operator which delivers records
     */
    Join_ResultSetDistinctOneResult( 
            SQLMan_Context&     acv,
            Join_RownoPositions&    rowno_pos,
            const IOperator&    rec_producer ) :
    Join_ResultSetOneResult( acv, rowno_pos, rec_producer )
    {
        SAPDBTRACE_METHOD_DEBUG( "Join_ResultSetDistinctOneResult::ctor", Join_Trace, 1 );
    }
    
    //! destructor
    virtual ~Join_ResultSetDistinctOneResult() {}
    //@}

    //! @name manipulation of result set
    //@{
    //! add record to result set
    virtual tgg00_BasisError Add( tgg00_Rec& record );
    //@}
};

#endif // JOIN_RESULTSETDISTINCTONERESULT_HPP
