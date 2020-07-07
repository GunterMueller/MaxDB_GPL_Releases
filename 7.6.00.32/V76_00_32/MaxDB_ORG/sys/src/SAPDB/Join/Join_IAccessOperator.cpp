/*!
 * @file  Join_IAccessOperator.cpp
 * @brief implements Join_IAccessOperator
 *
 * @author GertG
 * @ingroup Join
 *
 * @sa Join_IAccessOperator.hpp
 */
/*

    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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


#include "Join/Join_IAccessOperator.hpp"
#include "Trace/Trace_Entry.hpp"
#include "Trace/Trace_MessBlockEntry.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp"
#include "gsp00.h"
#include "ggg00.h"
#include "hgg01_3.h"
#include "ggg11.h"
#include "hgg04.h"
#include "hkb71.h"
#include "hta01.h"

class SQLMan_Context;

/* ******************** PUBLIC MEMBERS ********************* */
/*!
 * @param[in] acv global context
 * @param[in] tabno table number
 */
Join_IAccessOperator::Join_IAccessOperator(
    SQLMan_Context    &acv,
    const SAPDB_UInt2 tabno)
    : IOperator(acv),
      m_AccessDesc(acv), m_NullRec(acv.GetAllocator()), m_TableNo(tabno),
      m_maxRowRequest(SAPDB_MAX_INT4)
{
    SAPDBTRACE_METHOD_DEBUG( "Join_IAccessOperator::ctor", Join_Trace, 1 );

    SAPDBTRACE_WRITELN(
        Join_Trace, 5,
        "\nstrat reclen: " << GetRecordLength() );

    if ( g01vtrace.vtrAll_gg00 )
    {
        // default vtrace
        tgg11_JoinTabIdTrace _trace;
        _trace.jtHead_gg11.kbMessType_gg11  = m_AccessDesc.MBlock().mb_type();
        _trace.jtHead_gg11.kbMessType2_gg11 = m_AccessDesc.MBlock().mb_type2();
        _trace.jtType_gg11.becomes( jntrCtor_gg11 );
        _trace.jtTabId_gg11 = m_AccessDesc.MBlock().mb_qual()->mtree().fileTabId_gg00();
        _trace.jtTabno_gg11 = m_TableNo;

        Trace_CommonEntry Entry(
            tgg00_Debug::fromConst(ak_join) /* Layer */,
            tgg00_VtraceType::fromConst(vttAccessOperator_egg00),
            (tsp00_BytePtr) &_trace, 
            sizeof( _trace ) );
    }

    if ( Join_Trace.TracesLevel( 1 ) )
    {

        Trace_MessBlockEntry Entry(
            m_AccessDesc.MBlock(), 
            tgg00_Debug::fromConst( ak_messbuf ) /* Layer */,
            tgg00_VtraceType::fromConst(ak_send) );
    }

    invalidate_keys();

}

/*************************************************************************/
Join_IAccessOperator::~Join_IAccessOperator()
{
    if ( g01vtrace.vtrAll_gg00 )
    {
        // default vtrace
        tgg11_JoinTabIdTrace _trace;

        _trace.jtHead_gg11.kbMessType2_gg11.becomes( mm_nil );
        _trace.jtType_gg11.becomes( jntrDtor_gg11 );
        if ( e_ok != m_acv.TransContext().trError_gg00 &&
             e_no_next_record != m_acv.TransContext().trError_gg00 )
        {
            _trace.jtHead_gg11.kbMessType_gg11.becomes( m_return_error );
            _trace.jtHead_gg11.kbError_gg11     = m_acv.TransContext().trError_gg00;
        }
        else
        {
            _trace.jtHead_gg11.kbMessType_gg11.becomes( m_return_result );
            _trace.jtHead_gg11.kbError_gg11     = e_ok;
        }
        _trace.jtTabId_gg11 = m_AccessDesc.MBlock().mb_qual()->mtree().fileTabId_gg00();
        _trace.jtTabno_gg11 = m_TableNo;

        Trace_CommonEntry Entry(
            tgg00_Debug::fromConst(ak_join) /* Layer */,
            tgg00_VtraceType::fromConst(vttAccessOperator_egg00),
            (tsp00_BytePtr) &_trace, 
            sizeof( _trace ) );

        if ( ! m_acv.IsOk() )
        {
            _trace.jtHead_gg11.kbMessType_gg11.becomes( m_return_error );
            _trace.jtHead_gg11.kbError_gg11     = m_acv.a_ak_b_err;
            Trace_CommonEntry Entry(
                tgg00_Debug::fromConst(ak_join) /* Layer */,
                tgg00_VtraceType::fromConst(vttAccessOperator_egg00),
                (tsp00_BytePtr) &_trace, 
                sizeof( _trace ) );
        }
    }
}

/*************************************************************************/

void Join_IAccessOperator::SetMaximumRowsReadAtATime(
    const SAPDB_Int4 maxRowRequest)
{
    m_maxRowRequest = maxRowRequest;
}

/*************************************************************************/

SAPDB_Int4 Join_IAccessOperator::GetMaximumRowsReadAtATime() const
{
    return m_maxRowRequest;
}

/*************************************************************************/

tgg00_BasisError Join_IAccessOperator::GetNullRecord( tgg00_Rec*& recptr )
{
    SAPDBTRACE_METHOD_DEBUG( "Join_IAccessOperator::GetNullRecord", Join_Trace, 1 );

    if ( !m_NullRec.IsAssigned() && m_AccessDesc.HasOutputColumns() )
    {
        SAPDBTRACE_WRITELN( Join_Trace, 7, "create NULL record" );
        
        if (! m_NullRec.SetPtr( (tgg00_Rec*) 
                    m_acv.GetAllocator().Allocate( this->GetRecordLength() )))
            return e_no_more_memory;

        SAPDBTRACE_IF( Join_Trace, 3, SAPDB_MemFillNoCheck(m_NullRec.GetPtr(), ':', this->GetRecordLength()) );
        tgg00_StackDesc _output_desc = m_AccessDesc.MBlock().mb_qual()->mstack_desc();
        Join_StackListPtr _output( m_acv.GetAllocator(),
                (tgg00_StackList*) m_acv.GetAllocator().Allocate( 
                ((*m_AccessDesc.MBlock().mb_st())[m_AccessDesc.MBlock().
                 mb_qual()->mqual_pos()-1].epos() - 2) * 
                sizeof( tgg00_StackEntry ))); 
        
        if ( _output.IsAssigned() )
        {
            SAPDBTRACE_WRITELN( Join_Trace, 0, "allocate _output @ " << (void*)_output.GetPtr() );
            SAPDB_MemCopyNoCheck( _output.GetPtr(), (tgg00_StackEntry *)m_AccessDesc.MBlock().mb_st() + ( m_AccessDesc.MBlock().mb_qual()->mqual_pos() - 1 + 1 ), 
                     ((*m_AccessDesc.MBlock().mb_st())[m_AccessDesc.MBlock().mb_qual()->mqual_pos()-1].epos() - 2) * sizeof( tgg00_StackEntry ) );
            _output_desc.mst_addr()         = _output.GetPtr();
            _output_desc.mqual_cnt          = 
                    ((*m_AccessDesc.MBlock().mb_st())[m_AccessDesc.MBlock().mb_qual()->mqual_pos()-1].epos() - 2);
            _output_desc.mqual_pos = 1;
            _output_desc.mst_max()          = _output_desc.mqual_cnt;
            _output_desc.mfirst_free()      = _output_desc.mqual_cnt + 1;
            _output_desc.mst_optimize_pos() = 0;
            _output_desc.mstrat_pos         =  _output_desc.mstrat_cnt     = 0;
            _output_desc.mresqual_pos()     =  _output_desc.mresqual_cnt() = 0;
            _output_desc.mcol_pos           =  _output_desc.mcol_cnt       = 0;
            _output_desc.mmult_pos          =  _output_desc.mmult_cnt      = 0;
            _output_desc.mview_pos          =  _output_desc.mview_cnt      = 0;
            _output_desc.mupd_pos           =  _output_desc.mupd_cnt       = 0;
            _output_desc.mlink_pos          =  _output_desc.mlink_cnt      = 0;
            _output_desc.mstring_pos        =  _output_desc.mstring_cnt    = 0;
            _output_desc.mtrigger_pos       =  _output_desc.mtrigger_cnt   = 0;
            _output_desc.minvqual_pos       =  _output_desc.minvqual_cnt   = 0;
            
            // adjust column stack entries 
            for ( int _i = 0; _i < _output_desc.mqual_cnt; ++_i )
            {
                switch ( (*_output)[ _i ].etype() )
                {
                    case st_fixkey: 
                    case st_fixcol: 
                    case st_varkey: 
                    case st_varcol: 
                    case st_fixinv: 
                    case st_varinv: 
                    case st_varlongchar: 
                    case st_column: 
                        (*_output)[ _i ].etype().becomes( st_value );
                        // NULL value asured thhrough a680search_sequence()
                        (*_output)[ _i ].epos()  = m_AccessDesc.MBlock().mb_data_len();
                        (*_output)[ _i ].elen_var() = 1;
                        (*_output)[ _i ].ecol_tab()[0] = 0;
                        (*_output)[ _i ].ecol_tab()[1] = 0;
                        break;
                }
            }
            
            SAPDBTRACE_IF( Join_Trace, 3, 
            t01stackdesc( td_always, "copied outpu", _output_desc.mst_addr(), _output_desc ));
            
            tgg00_StackEntry *_err_st_entry;
            pasbool _unqualified = false;

            tgg00_SelectFieldsParam _selfields;
            g04init_select_fields( _selfields, 
                    reinterpret_cast<tsp00_MoveObjPtr>(m_AccessDesc.MBlock().mb_data()),
                    m_AccessDesc.MBlock().mb_data_size(), 
                    m_acv.a_work_st_addr, 
                    m_acv.a_work_st_max, 
                    m_acv.a_work_buf_addr, 
                    m_acv.a_work_buf_size,
                    m_acv.a_sqlmode,
                    &m_AccessDesc.MBlock().mb_fieldlists());
            _selfields.sfp_bd_mess_type().becomes( m_select );
            _selfields.sfp_result_wanted() = true;
            _selfields.sfp_first_qual()    = true;
            _selfields.sfp_filler2()       = false;
            _selfields.sfp_rows_read()     = 0;
            _selfields.sfp_rows_qual()     = 0;
            _selfields.sfp_acv_addr()      = reinterpret_cast<tsp00_Addr>( &m_acv );
            _selfields.sfp_oldrec_addr()   = 0;
            _selfields.sfp_primkey_addr()  = 0;
            _selfields.sfp_result_length() = 0; 
            _selfields.sfp_m_result_len()  = 0;
            _selfields.sfp_m_result_cnt()  = 1;
            _selfields.sfp_m_result_size() = this->GetRecordLength();
            _selfields.sfp_m_result_addr() = reinterpret_cast<tsp00_MoveObjPtr>(m_NullRec.GetPtr());
            _selfields.sfp_rec_addr()    = reinterpret_cast<tsp00_BufAddr>(m_NullRec.GetPtr());
            _selfields.sfp_rec_len()     = m_NullRec->recLen_gg00()    = this->GetRecordLength(); 
            _selfields.sfp_rec_key_len() = m_NullRec->recKeyLen_gg00() = this->GetKeyLength();
            m_acv.TransContext().trError_gg00 = e_ok;
            
            k71qual_handling( m_acv.TransContext(), _selfields, 
                    false, false, _output_desc,
                    _err_st_entry, _unqualified );
            SAPDBTRACE_IF( Join_Trace, 3, t01buf( td_always, m_NullRec.GetPtr(), 1, this->GetRecordLength() ) );
            
        }
        else
            return e_no_more_memory;
        
    }
    recptr = m_NullRec.GetPtr();
    return e_ok;
}

