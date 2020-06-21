/*!
 * @file
 * @brief implements Join_IResultSetAggregator
 *
 * @author GertG
 * @ingroup Join
 *
 * @sa Join_IResultSetAggregator.hpp
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
#include "Join/Join_ResultSetAggregator2.hpp"
#include "Join/Join_Aggregation.hpp"
#include "SAPDBCommon/SAPDB_RangeCode.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "hak101.h"
#include "hbd07.h"
#include "hkb721.h"
#include "hgg01_3.h"
#include "hta01.h"
#include "hsp51.h"
#include "hsp41.h"
#include "hbd998.h"

/* ******************** PUBLIC MEMBERS ********************* */

tgg00_BasisError Join_ResultSetAggregator2::Add( tgg00_Rec& record )
{
    SAPDBTRACE_METHOD_DEBUG( "Join_ResultSetAggregator2::Add", Join_Trace, 3 );

    if ( m_ResultCnt < m_MaxResultCnt )
    {
        tgg00_BasisError _b_err = e_ok;

        set_result_counter( record, 1 );

        if ( ! m_TempHashTable->InsertOrReplace( 
                reinterpret_cast<SAPDB_Byte*>(&record),
                m_acv.GetErrorList() ) )
        {
            // what to return ???
            return e_system_error;
        }

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

        SAPDBTRACE_WRITELN( Join_Trace, 3, "accepted recs: " << m_ResultCnt << "\tresult cnt: " << m_GroupCnt );
        SAPDBTRACE_WRITELN( Join_Trace, 3, "return: " << SAPDBTrace::BasisError(_b_err) );
        return _b_err;
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

/* ******************* PROTECTED MEMBERS ******************* */

/* ******************** PRIVATE MEMBERS ******************** */
/*!
 *
 */
void Join_ResultSetAggregator2::post_processing()
{
    SAPDBTRACE_METHOD_DEBUG( "Join_ResultSetAggregator2::post_processing", Join_Trace, 3 );

    m_acv.TransContext().trError_gg00 = e_ok;

    if ( m_TempHashTable->IsEmpty() )
        return;

    tgg00_BasisError _b_err = e_ok;

    if ( ! m_TempHashTable->First( m_acv.GetErrorList() ) )
    {
        m_acv.ThrowError( e_system_error );
        return;
    }
    SAPDBTRACE_WRITELN( Join_Trace, 3, "m_RecordCounterPositions.IsEmpty(): " << m_RecordCounterPositions.IsEmpty() << 
            ", m_PostProcessing: " << (int) m_PostProcessing << 
            ", m_DistinctFileRoot.GetSize(): " << m_DistinctFileRoot.GetSize() );
    
    if  ( sfh_none == m_PostProcessing  && m_RecordCounterPositions.IsEmpty() )
    {
        do {
            // create result file
            tgg00_Rec &_buffer = 
                *reinterpret_cast<tgg00_Rec*>(m_TempHashTable->Get() );
            SAPDBTRACE_IF( Join_Trace, 9, 
            t01buf( td_always, &_buffer, 1, _buffer.recLen_gg00() <= 300 ? _buffer.recLen_gg00() : 300 ));
            b07cappend_record( m_acv.TransContext(), 
                    m_ResultTree, m_TreePos, _buffer );
            _b_err = m_acv.TransContext().trError_gg00;
            ++m_GroupCnt;
            if ( m_acv.TransContext().trRteCommPtr_gg00->to_cancel )
                _b_err = e_cancelled;
        } while ( e_ok == _b_err && m_TempHashTable->Next() );
    }
    else
    {
        SAPDB_UInt4 _res_cnt = 0;
        Join_AggregationContext _context = 
            Join_AggregationContext( m_acv, *m_AggregationDesc, m_DistinctFileId, m_DistinctFileRoot );

        do {
            tgg00_Rec &_buffer = 
                *reinterpret_cast<tgg00_Rec*>(m_TempHashTable->Get() );
            SAPDBTRACE_IF( Join_Trace, 9, 
            t01buf( td_always, &_buffer, 1, _buffer.recLen_gg00() <= 300 ? _buffer.recLen_gg00() : 300 ));
            
            if ( m_acv.TransContext().trRteCommPtr_gg00->to_cancel )
                _b_err = e_cancelled;
            else
            {
                _context.m_grouprec_changed = false;
                ++_res_cnt;
                
                _b_err = Join_Aggregation::post_processing( _context, _buffer, 
                        _res_cnt, m_LaterOutputPos, m_RecordCounterPositions, m_PostProcessing );

                // write into file
                if ( e_ok == _b_err )
                {
                    b07cappend_record( m_acv.TransContext(), 
                            m_ResultTree, m_TreePos, _buffer );
                    _b_err = m_acv.TransContext().trError_gg00;
                    ++m_GroupCnt;
                }
            }
        } while ( e_ok == _b_err && m_TempHashTable->Next() );
    }
    if ( e_ok != _b_err ) m_acv.ThrowError( _b_err );
}

/* ********************************************************* */
/*!
 *
 */
void Join_ResultSetAggregator2::write_NULL_record()
{
    SAPDBTRACE_METHOD_DEBUG( "Join_ResultSetAggregator2::write_NULL_record", Join_Trace, 3 );
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
        _out_pos = k721out_entry( (tgg00_StackList*) m_AggregationDesc, _func_pos );
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
    SAPDBTRACE_IF( Join_Trace, 3, 
    t01buf( td_always, _record, 1, _record->recLen_gg00() <= 300 ? _record->recLen_gg00() : 300 ));

    b07cadd_record( m_acv.TransContext(), m_ResultTree, *_record );
    if ( e_ok != m_acv.TransContext().trError_gg00 ) 
        m_acv.ThrowError( m_acv.TransContext().trError_gg00 );
}
    
