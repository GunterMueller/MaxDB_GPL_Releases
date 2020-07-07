#ifndef JOIN_RESULTSETAGGREGATOR_HPP
#define JOIN_RESULTSETAGGREGATOR_HPP
/*!
 * @file
 * @brief interface to result set operator which inserts record and 
 * sort them corresponding to key
 *
 * @author GertG
 * @ingroup Join
 *
 * @sa Join_Execute.cpp, Join_IResultSetAggregator.hpp
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
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "Join/Join_IResultSetAggregator.hpp"
#include "Join/Join_ResultSetInserter.hpp"
#include "hbd07.h"

class SQLMan_Context;

//! interface to result set operator which inserts record and sort them corresponding to key
class Join_ResultSetAggregator : public Join_IResultSetAggregator {
public:
    //! @name constructor / destructor
    //@{
    //! constructor
    /*! 
     * @param[in] acv global context
     * @param[in] rowno_pos positions of ROWNO columns
     * @param[in] aggr_desc pointer to result record description
     * @param[in] result_id name of result file
     * @param[in] rescnt amount of records already in result set
     * @param[in] maxrescnt maximal amount of records in result set
     * @param[in] rec_producer operator delivers records
     */
    Join_ResultSetAggregator(
            SQLMan_Context&                acv, 
            Join_RownoPositions&           rowno_pos,
            Join_StackListPtr&             aggr_desc,
            tgg00_FileId&                  result_id, 
            const SAPDB_Int4&              rescnt,
            const SAPDB_Int4&              maxrescnt,
            IOperator&                     rec_producer ) :
    Join_IResultSetAggregator( acv, rowno_pos, aggr_desc, result_id, rescnt, maxrescnt, rec_producer )
    {
        SAPDBTRACE_METHOD_DEBUG( "Join_ResultSetAggregator::ctor", Join_Trace, 1 );
        m_acv.TransContext().trWarning_gg00.addElement(warn0_exist);
        m_acv.TransContext().trWarning_gg00.addElement(warn8_scan_or_sort);
        m_TreePos.tpsPno_gg00 = NIL_PAGE_NO_GG00;
    }

    //! destructor
    virtual ~Join_ResultSetAggregator() {};
    //@}
    
    //! @name manipulation of result set
    //@{
    //! add record to result set
    /*!
     * @param[in] record record to be added
     * @return information code [e_ok, e_no_next_record] / error code [ ... ]
     */
    virtual tgg00_BasisError Add( tgg00_Rec& record )
    {
        SAPDBTRACE_METHOD_DEBUG( "Join_ResultSetAggregator::Add", Join_Trace, 1 );

        if ( m_ResultCnt < m_MaxResultCnt )
        {
            SAPDBTRACE_IF( Join_Trace, 5, 
            t01buf( td_always, &record, 1, record.recLen_gg00() <= 300 ? record.recLen_gg00() : 300 ));
            return add( record );
        }
        return e_no_next_record;
    }
    //@}

protected:

    virtual tgg00_BasisError get_group_record( tgg00_Rec& record )
    {
        SAPDBTRACE_METHOD_DEBUG( "Join_ResultSetAggregator::get_group_record", Join_Trace, 3 );
        SAPDBTRACE_IF( Join_Trace, 5, SAPDB_MemFillNoCheck( &m_GroupRec, ':', sizeof( m_GroupRec ) ) );

        build_group_key( record );

        m_TreePos.tpsPno_gg00 = NIL_PAGE_NO_GG00; 
        b07ctget_record( m_acv.TransContext(), m_ResultTree, m_TreePos, m_GroupKey, m_GroupRec );
        return m_acv.TransContext().trError_gg00;
    }

    virtual tgg00_BasisError update_group_record()
    {
        SAPDBTRACE_METHOD_DEBUG( "Join_ResultSetAggregator::update_group_record", Join_Trace, 3 );
        b07ctrepl_record( m_acv.TransContext(), m_ResultTree, m_TreePos, m_GroupRec );
        return m_acv.TransContext().trError_gg00;
    }
    
    virtual tgg00_BasisError add_group_record( tgg00_Rec& record )
    {
        SAPDBTRACE_METHOD_DEBUG( "Join_ResultSetAggregator::add_group_record", Join_Trace, 3 );
        b07cadd_record( m_acv.TransContext(), m_ResultTree, record );
        if ( e_ok == m_acv.TransContext().trError_gg00 ) ++m_GroupCnt;
        return m_acv.TransContext().trError_gg00;
    }
    
    virtual tgg00_BasisError is_new_group_allowed() { return e_ok; }

    virtual void init_result_scan() 
    { 
        m_ResultPos.tpsPno_gg00 = NIL_PAGE_NO_GG00; 
        m_SetResult.bd_key_check_len = 0;
        m_SetResult.bd_max_rec_cnt = 1;
        m_SetResult.bd_max_fill_len = mxsp_buf;
        m_SetResult.bd_next = true;
        m_SetResult.bd_drop_page = false;
    }
    
    virtual tgg00_BasisError fetch_next_result( tgg00_Rec& record ) 
    { 
        tgg00_Lkey _hkey;
        _hkey.len() = 0;

        b07cnext_record( m_acv.TransContext(), m_ResultTree, 
                _hkey, m_SetResult, m_ResultPos, &record );
        if ( e_key_not_found == m_acv.TransContext().trError_gg00 )
            m_acv.TransContext().trError_gg00 = e_ok;
        return m_acv.TransContext().trError_gg00;
    }
    
    virtual tgg00_BasisError update_result( tgg00_Rec& record ) 
    { 
        SAPDBTRACE_IF( Join_Trace, 3, 
        t01buf( td_always, &record, 1, record.recLen_gg00() <= 300 ? record.recLen_gg00() : 300 ));
        b07crepl_record( m_acv.TransContext(), m_ResultTree, record );
        return m_acv.TransContext().trError_gg00;
    }

    virtual void write_NULL_record() { /* do nothing */ }
    
private:
    tgg00_FilePos m_ResultPos;
    tgg00_BdSetResultRecord m_SetResult;
};

#endif // JOIN_RESULTSETAGGREGATOR_HPP
