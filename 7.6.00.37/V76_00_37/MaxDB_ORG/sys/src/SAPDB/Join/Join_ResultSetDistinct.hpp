#ifndef JOIN_RESULTSETDISTINCT_HPP
#define JOIN_RESULTSETDISTINCT_HPP
/*!
 * @file
 * @brief implements interface Join_IResultSetInserter; 
 * guarantee distinctness of records
 *
 * @author GertG
 * @ingroup Join
 *
 * @sa Join_Execute.cpp, Join_IResultSetInserter.hpp
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
#include "Join/Join_IResultSetInserter.hpp"

class SQLMan_Context;

//! implements interface Join_IResultSetInserter; guarantee distinctness of records
class Join_ResultSetDistinct : public Join_IResultSetInserter {
public:
    //! @name constructor / destructor
    //@{
    //! constructor
    /*! 
     * @param[in] acv global context
     * @param[in] rowno_pos positions of ROWNO columns
     * @param[in] result_id name of result file
     * @param[in] rescnt amount of records already in result set
     * @param[in] maxrescnt maximal amount of records in result set
     */
    Join_ResultSetDistinct( 
            SQLMan_Context&                 acv, 
            Join_RownoPositions&            rowno_pos,
            tgg00_FileId&                   result_id, 
            const SAPDB_Int4&               rescnt,
            const SAPDB_Int4&               maxrescnt) :
    Join_IResultSetInserter( acv, rowno_pos, result_id, rescnt, maxrescnt )
    {
        SAPDBTRACE_METHOD_DEBUG( "Join_ResultSetDistinct::ctor", Join_Trace, 1 );
    }
    
    //! destructor
    virtual ~Join_ResultSetDistinct() {}

    //! @name manipulation of result set
    //@{
    //! add record to result set
    virtual tgg00_BasisError Add( tgg00_Rec& record );
    //@}
};

#endif // JOIN_RESULTSETDISTINCT_HPP
