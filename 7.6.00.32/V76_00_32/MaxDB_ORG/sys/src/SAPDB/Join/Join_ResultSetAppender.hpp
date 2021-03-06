#ifndef JOIN_RESULTSETAPPENDER_HPP
#define JOIN_RESULTSETAPPENDER_HPP
/*!
 * @file
 * @brief implements interface Join_IResultSetAppender
 *
 * @author GertG
 * @ingroup Join
 *
 * @sa Join_Execute.cpp, Join_IResultSetAppender.hpp
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
#include "Join/Join_IResultSetAppender.hpp"

class SQLMan_Context;

//! implements interface Join_IResultSetAppender
class Join_ResultSetAppender : public Join_IResultSetAppender {
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
    Join_ResultSetAppender( 
            SQLMan_Context&                 acv, 
            Join_RownoPositions&            rowno_pos,
            tgg00_FileId&                   result_id, 
            const SAPDB_Int4&               rescnt,
            const SAPDB_Int4&               maxrescnt ) :
    Join_IResultSetAppender( acv, rowno_pos, result_id, rescnt, maxrescnt )
    { 
        SAPDBTRACE_METHOD_DEBUG( "Join_ResultSetAppender::ctor", Join_Trace, 1 );
    }
    //@}

    //! @name manipulation of result set
    //@{
    //! add record to result set
    /*!
     * @param[in] record [in] record to be added
     * @return information code [e_ok, e_no_next_record] / error code [ ... ]
     */
    virtual tgg00_BasisError Add( tgg00_Rec& record )
    {
        SAPDBTRACE_METHOD_DEBUG( "Join_ResultSetAppender::Add", Join_Trace, 1 );
        if ( m_ResultCnt < m_MaxResultCnt )
        {
            return add( record );
        }
        return e_no_next_record;
    }
    //@}
};

#endif // JOIN_RESULTSETAPPENDER_HPP
