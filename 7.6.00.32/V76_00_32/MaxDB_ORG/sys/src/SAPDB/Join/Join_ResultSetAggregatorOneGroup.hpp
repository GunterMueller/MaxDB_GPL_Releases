#ifndef JOIN_RESULTSETAGGREGATORONEGROUP_HPP
#define JOIN_RESULTSETAGGREGATORONEGROUP_HPP
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
#include "Join/Join_IResultSetAggregator.hpp"
#include "SAPDBCommon/SAPDB_RangeCode.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "hgg01_3.h"
#include "hsp41.h"

class SQLMan_Context;

//! interface to result set operator which inserts record and sort them corresponding to key
class Join_ResultSetAggregatorOneGroup : public Join_IResultSetAggregator {
public:
    //! @name constructor / destructor
    //@{
    //! constructor
    /*! 
     * @param[in] acv global context
     * @param[in] rowno_pos positions of ROWNO columns
     * @param[in] aggr_desc pointer to result record description
     * @param[in] result_id name of result file
     * @param[in] maxrescnt maximal amount of records in result set
     * @param[in] rec_producer operator delivers records
     * @param[in] is_select_into we deal with "SELECT INTO"?
     */
    Join_ResultSetAggregatorOneGroup( 
            SQLMan_Context&                acv, 
            Join_RownoPositions&           rowno_pos,
            Join_StackListPtr&             aggr_desc,
            tgg00_FileId&                  result_id, 
            const SAPDB_Int4&              maxrescnt,
            IOperator&                     rec_producer,
            const pasbool                  is_select_into ) :
    Join_IResultSetAggregator( acv, rowno_pos, aggr_desc, result_id, 0, maxrescnt, rec_producer ),
    m_ResultFetched( false ), m_SelectInto( is_select_into )
    {
        SAPDBTRACE_METHOD_DEBUG( "Join_ResultSetAggregatorOneGroup::ctor", Join_Trace, 1 );
    }

    virtual void Close()
    {
        SAPDBTRACE_METHOD_DEBUG( "Join_ResultSetAggregatorOneGroup::Close", Join_Trace, 1 );
        if ( this->IsOpened() )
        {
            if ( m_GroupCnt == 0 )
                write_NULL_record();
            Join_IResultSetAggregator::Close();

            if ( m_SelectInto )
            {
                if ( 0 == this->ResultCount() )
                {
                    m_acv.GetMessBlock().DataLength() = 0;
                    if ( e_ok == m_acv.TransContext().trError_gg00 )
                    {
                        m_acv.TransContext().trError_gg00 = e_no_next_record;
                    }
                }
                else
                {
                    // move record into data part of messblock
                    SAPDB_MemCopyNoCheck( &m_acv.a_mblock.mb_data()->mbp_4kbuf(), &m_GroupRec, m_GroupRec.recLen_gg00() );
                    m_acv.GetMessBlock().DataLength() = m_GroupRec.recLen_gg00();
                }
            }
            else
            {
                if ( this->ResultCount() > 0 )
                    b07cadd_record( m_acv.TransContext(), m_ResultTree, m_GroupRec );
            }
        } 
    };
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
        SAPDBTRACE_METHOD_DEBUG( "Join_ResultSetAggregatorOneGroup::Add", Join_Trace, 1 );

        if ( m_ResultCnt < m_MaxResultCnt )
        {
            SAPDBTRACE_IF( Join_Trace, 5, 
            t01buf( td_always, &record, 1, record.recLen_gg00() <= 300 ? record.recLen_gg00() : 300 ));
            return add( record );
        }
        else
        {
            if ( m_SelectInto )
            {
                if ( sqlm_oracle == m_acv.a_sqlmode )
                {
                    SAPDBTRACE_WRITELN( Join_Trace, 3, "accept more results" );
                    m_acv.TransContext().trWarning_gg00.addElement(warn0_exist);
                    m_acv.TransContext().trWarning_gg00.addElement(warn12_rowno_used);
                    return e_no_next_record;
                }
                else
                {
                    SAPDBTRACE_WRITELN( Join_Trace, 3, "too many results" );
                    return e_too_many_resultsets;
                }
            }
            return e_no_next_record;
        }
    }
    //@}

protected:

    /*!
     *
     */
    virtual tgg00_BasisError get_group_record( tgg00_Rec& record )
    {
        SAPDBTRACE_METHOD_DEBUG( "Join_ResultSetAggregatorOneGroup::get_group_record", Join_Trace, 3 );
        
        build_group_key( record );
        
        if ( 0 == this->ResultCount() ) return e_key_not_found;
        // group record already in m_GroupRec
        return e_ok;
    }
    
    /*!
     *
     */
    virtual tgg00_BasisError update_group_record()
    {
        SAPDBTRACE_METHOD_DEBUG( "Join_ResultSetAggregatorOneGroup::update_group_record", Join_Trace, 3 );
        // m_GroupRec already updated
        return e_ok;
    }

    /*!
     *
     */
    virtual tgg00_BasisError add_group_record( tgg00_Rec& record )
    {
        SAPDBTRACE_METHOD_DEBUG( "Join_ResultSetAggregatorOneGroup::add_group_record", Join_Trace, 3 );
        
        ++m_GroupCnt;
        SAPDB_MemCopyNoCheck( &m_GroupRec, &record, record.recLen_gg00() );
        return e_ok;
    }
    
    /*!
     *
     */
    virtual tgg00_BasisError is_new_group_allowed() 
    { 
        if ( m_GroupCnt == 0 ) 
            return e_ok;
        else
        {
            if ( sqlm_oracle == m_acv.a_sqlmode )
            {
                SAPDBTRACE_WRITELN( Join_Trace, 3, "accept more results" );
                m_acv.TransContext().trWarning_gg00.addElement(warn0_exist);
                m_acv.TransContext().trWarning_gg00.addElement(warn12_rowno_used);
                return e_no_next_record;
            }
            else
            {
                SAPDBTRACE_WRITELN( Join_Trace, 3, "too many results" );
                return e_too_many_resultsets;
            }
        }
    }
    
    /*!
     *
     */
    virtual void init_result_scan() { m_ResultFetched = false; }
    
    /*!
     *
     */
    virtual tgg00_BasisError fetch_next_result( tgg00_Rec& record ) 
    { 
        SAPDBTRACE_METHOD_DEBUG( "Join_ResultSetAggregatorOneGroup::fetch_next_result", Join_Trace, 5 );
        if ( ! m_ResultFetched )
        {
            tgg00_BasisError _b_err = e_ok;
            m_ResultFetched = true;

            SAPDB_RangeMove( __FILE__, 1,
                    sizeof( m_GroupRec ), sizeof( record ),
                    &m_GroupRec, 1, &record, 1, 
                    m_GroupRec.recLen_gg00(), _b_err );
            SAPDBTRACE_WRITELN( Join_Trace, 7, "return: " << SAPDBTrace::BasisError(_b_err) );
            return _b_err;
        }
        return e_no_next_record; 
    }
    
    /*!
     *
     */
    virtual tgg00_BasisError update_result( tgg00_Rec& record ) 
    { 
        SAPDBTRACE_METHOD_DEBUG( "Join_ResultSetAggregatorOneGroup::update_result", Join_Trace, 5 );
        tgg00_BasisError _b_err = e_ok;

        SAPDB_RangeMove( __FILE__, 2,
                sizeof( record ), sizeof( m_GroupRec ), 
                &record, 1, &m_GroupRec, 1, 
                record.recLen_gg00(), _b_err );
        SAPDBTRACE_WRITELN( Join_Trace, 7, "return: " << SAPDBTrace::BasisError(_b_err) );
        return _b_err;
    }

    /*!
     *
     */
    void write_NULL_record()
    {
        SAPDBTRACE_METHOD_DEBUG( "Join_ResultSetAggregatorOneGroup::write_NULL_record", Join_Trace, 5 );
        tgg00_BasisError _b_err = e_ok;
        tgg00_Rec* _record;
        SAPDB_UInt4 _func_pos = (*m_AggregationDesc)[ 0 ].epos() - 1;
        SAPDB_UInt2 _out_pos;

        m_RecordProducer.GetNullRecord( _record );

        while (( _func_pos - (*m_AggregationDesc)[ _func_pos - 1 ].epos() > 1 ) 
                && ( e_ok == m_acv.TransContext().trError_gg00 ))
        {
            _func_pos -= (*m_AggregationDesc)[ _func_pos - 1 ].epos();
            // loop over LASTFUNCTION
            while (( st_func == (*m_AggregationDesc)[ _func_pos - 1 ].etype() ) 
                    && ( op_f_none == (*m_AggregationDesc)[ _func_pos - 1 ].eop_func() ))
                _func_pos -= (*m_AggregationDesc)[ _func_pos - 1 ].epos();
            _out_pos = k721out_entry( (tgg00_StackList*) m_AggregationDesc.GetPtr(), _func_pos );
            if ( st_func == (*m_AggregationDesc)[ _func_pos - 1 ].etype() )
            {
                if ( op_f_count      == (*m_AggregationDesc)[ _func_pos - 1 ].eop_func() || 
                     op_f_dis_count  == (*m_AggregationDesc)[ _func_pos - 1 ].eop_func() ||    
                     op_f_all_count  == (*m_AggregationDesc)[ _func_pos - 1 ].eop_func() )
                {
                    // write record count 0
                    _record->recBuf_gg00()[ (*m_AggregationDesc)[ _out_pos - 1 ].epos() - 1 ] = csp_defined_byte;
                    SAPDB_RangeMove( __FILE__, 3,
                            sizeof( g01glob.rescnt_0 ), _record->recLen_gg00(), 
                            &g01glob.rescnt_0, 2, _record, 
                            (*m_AggregationDesc)[ _out_pos - 1 ].epos() + 1, 
                            sizeof( tsp00_ResNum ) - 1, 
                            m_acv.TransContext().trError_gg00 );
                }
                else
                {
                    if ( op_f_check_null == (*m_AggregationDesc)[ _func_pos - 1 ].eop_func() )
                    {
                        // set defined byte
                        _record->recBuf_gg00()[ (*m_AggregationDesc)[ _out_pos - 1 ].epos() - 1 ] = csp_defined_byte;
                    }
                    else
                    {
                        // write NULL value
                        _record->recBuf_gg00()[ (*m_AggregationDesc)[ _out_pos - 1 ].epos() - 1 ] = csp_undef_byte;
                        SAPDB_RangeFill( __FILE__,  4,    
                              _record->recLen_gg00(),
                              _record,
                              (*m_AggregationDesc)[ _out_pos - 1 ].epos() + 1,
                              (*m_AggregationDesc)[ _out_pos - 1 ].elen_var() - 1, 
                              csp_defined_byte,
                              m_acv.TransContext().trError_gg00 );
                    }
                }
            }
        }
        if ( m_LaterOutputPos > 0 )
        {
            SAPDBTRACE_WRITELN( Join_Trace, 3, "put 'later out': " 
                    << (*m_AggregationDesc)[ m_LaterOutputPos - 1 - 1 ].epos() 
                    << "[" << (*m_AggregationDesc)[ m_LaterOutputPos - 1 ].elen_var() << "]" 
                    << " --> " << (*m_AggregationDesc)[ m_LaterOutputPos - 1 ].epos() );
            SAPDB_RangeMove( __FILE__, 5,
                    _record->recLen_gg00(), _record->recLen_gg00(),
                    _record, 
                    (*m_AggregationDesc)[ m_LaterOutputPos - 1 - 1 ].epos(),
                    _record, 
                    (*m_AggregationDesc)[ m_LaterOutputPos - 1 ].epos(),
                    (*m_AggregationDesc)[ m_LaterOutputPos - 1 ].elen_var(), 
                    m_acv.TransContext().trError_gg00 );
        }

        // put ROWNO
        tsp00_NumError _ret;
        Container_Vector<SAPDB_Int4>::Iterator _iter;
        for ( _iter = m_RecordCounterPositions.Begin(); _iter != m_RecordCounterPositions.End(); ++_iter) 
        {
            _record->buf()[ *_iter - 1 ] = csp_defined_byte;
            s41plint( &_record->buf(), *_iter + 1,
                  csp_resnum_deflen, 0, 1, _ret );
        }

        set_result_counter( *_record, ++m_GroupCnt );

        SAPDB_MemCopyNoCheck( &m_GroupRec, _record, _record->recLen_gg00());
        SAPDBTRACE_IF( Join_Trace, 3, 
        t01buf( td_always, &m_GroupRec, 1, m_GroupRec.recLen_gg00() <= 300 ? m_GroupRec.recLen_gg00() : 300 ));
    }
    
private:

    const pasbool m_SelectInto;  //!< SELECT INTO wanted?
    bool m_ResultFetched; //!< is result fetched?
};

#endif // JOIN_RESULTSETAGGREGATORONEGROUP_HPP
