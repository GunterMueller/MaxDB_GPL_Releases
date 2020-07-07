#ifndef JOIN_IRESULTSETAPPENDER_HPP
#define JOIN_IRESULTSETAPPENDER_HPP
/*!
 * @file
 * @brief interface to result set operator which appends record
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
#include "Join/Join_IResultSetOperator.hpp"
#include "Container/Container_Vector.hpp"
#include "hbd07.h"
#include "hsp41.h"

class SQLMan_Context;

//! interface to result set operator which appends record
class Join_IResultSetAppender : public Join_IResultSetOperator {
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
    Join_IResultSetAppender( 
            SQLMan_Context&                acv, 
            Join_RownoPositions&           rowno_pos,
            tgg00_FileId&                  result_id, 
            const SAPDB_Int4&              rescnt,
            const SAPDB_Int4&              maxrescnt ) :
    Join_IResultSetOperator( acv, rowno_pos, rescnt, maxrescnt ), m_ResultTree( result_id )
    {
        SAPDBTRACE_IF( Join_Trace, 3, t01treeid( td_always, "m_ResultTree", m_ResultTree ));
        m_TreePos.tpsPno_gg00 = NIL_PAGE_NO_GG00; 
    }
    //! destructor
    virtual ~Join_IResultSetAppender(){};
    //@}
    
    //! @name manipulation of result set
    //@{
    //! add record to result set
    /*!
     * @param[in] record record to be added
     * @return information code [e_ok, e_no_next_record] / error code [ ... ]
     */
    virtual tgg00_BasisError Add( tgg00_Rec& record) =0;
    //@}

protected:
    //! common code to add a record to result set
    /*!
     * @param[in] record record to be added
     * @return information code [e_ok, e_no_next_record] / error code [ ... ]
     */
    tgg00_BasisError add( tgg00_Rec& record )
    {
        SAPDBTRACE_METHOD_DEBUG( "Join_IResultSetAppender::add", Join_Trace, 3 );

        tgg00_BasisError _b_err = e_ok;

        set_result_counter( record, m_ResultCnt + 1 );

        write_rowno( record );

        SAPDBTRACE_IF( Join_Trace, 3, 
        t01buf( td_always, &record, 1, record.recLen_gg00() <= 300 ? record.recLen_gg00() : 300 ));
        // record.recKeyLen_gg00() <= RESCNT_MXGG04
        // insert in orderd sequence (improof insert performance)
        b07cappend_record( m_acv.TransContext(), 
                m_ResultTree, m_TreePos, record );
        _b_err = m_acv.TransContext().trError_gg00;

        if ( e_ok == _b_err ) 
        {
            if ( m_acv.TransContext().trRteCommPtr_gg00->to_cancel )
                _b_err = e_cancelled;
            if ( ++m_ResultCnt == m_MaxResultCnt ) 
            {
                _b_err = e_no_next_record;
                if ( m_SetWarning )
                {
                    m_acv.TransContext().trWarning_gg00.addElement(warn0_exist);
                    m_acv.TransContext().trWarning_gg00.addElement(warn12_rowno_used);
                }
            }
        }

        SAPDBTRACE_WRITELN( Join_Trace, 3, "result cnt: " << m_ResultCnt );
        SAPDBTRACE_WRITELN( Join_Trace, 3, "return: " << SAPDBTrace::BasisError(_b_err) );
        return _b_err;
    }

    //! common code to write ROWNO to record
    /*!
     * @param[in] record record on which to written ROWNO
     */
    void write_rowno( tgg00_Rec& record )
    {
        tsp00_NumError   _ret;
        // put ROWNO
        Container_Vector<SAPDB_Int4>::Iterator _iter;
        for ( _iter = m_RecordCounterPositions.Begin(); _iter != m_RecordCounterPositions.End(); ++_iter) 
        {
            record.buf()[ *_iter - 1 ] = csp_defined_byte;
            s41plint( record.buf(), *_iter + 1,
                  csp_resnum_deflen, 0, m_ResultCnt + 1, _ret );
        }
    }

private:
    // member variables
    tgg00_FilePos                m_TreePos;
    tgg00_FileId                 m_ResultTree;
};

#endif // JOIN_IRESULTSETAPPENDER_HPP
