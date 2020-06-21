/*!
 * @file
 * @brief Realizes pascal stub a686_create_join_result().
 *
 *  Realizes pascal/C++ connector and creates join result by means
 * of Join_JoinOperator.
 *
 * @author GertG
 * @ingroup Join
 *
 */
/*!
 * @addtogroup SQLManager
*/
/*!
 * @defgroup Join Join Execution
 * @ingroup SQLManager
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

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "RunTime/RTE_Crash.hpp"
#include "SAPDBCommon/SAPDB_AutoPtr.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "Join/Join_Types.hpp"
#include "Join/Join_JoinOperator2.hpp"
#include "Join/Join_IResultSetOperator.hpp"
#include "Join/Join_ResultSetInserter.hpp"
#include "Join/Join_ResultSetHashInserter.hpp"
#include "Join/Join_ResultSetAppenderEx.hpp"
#include "Join/Join_ResultSetAppender.hpp"
#include "Join/Join_ResultSetOneResult.hpp"
#include "Join/Join_ResultSetDistinctOneResult.hpp"
#include "Join/Join_ResultSetDistinct.hpp"
#include "Join/Join_ResultSetAggregator.hpp"
#include "Join/Join_ResultSetAggregatorOneGroup.hpp"
#include "Join/Join_ResultSetAggregator2.hpp"
#include "Join/Join_ResultSetDummy.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "SQLManager/SQLMan_MessBlock.hpp"
#include "SQLManager/SQLMan_Strategy.hpp"
#include "RunTime/RTE_Message.hpp"
#include "SAPDBCommon/Messages/SDBMsg_Join.h"
#include "Table/Table_TempHashTable.hpp"
#include "gsp03.h"
#include "gsp03_1.h"
#include "ggg00.h"
#include "ggg04.h"
#include "vak001.h"
#include "gak68.h"
#include "hak06.h"
#include "hak101.h"
#include "hak682.h"
#include "hak686.h"
#include "hta01.h"
#include "heo51.h" // vdebug_break()
#include "hkb720.h"
#include "hkb05.h"
#include "hak071.h"
#include "hsp41.h"
#include "hak680.h"
#include "hak502.h"

//! \if false
namespace {
    
inline    
Join_IResultSetOperator* 
    CreateResultSetOperator( 
            SQLMan_Context&         acv,
            tak_dml_info&           dmli,
            Join_ResultDesc&        result_desc,
            const SAPDB_Int4&       maxresult, 
            IOperator&              top_operator );

inline
void
    GetResultDesc(
            SQLMan_Context&         acv,
            SQLMan_MessBlock& mblock,
            Join_ResultDesc& result_desc );

} // anonymous namespace
//! \endif


//! routine which generates result of a join
/*!
 * @param[in] acv global context
 * @param[in] dmli statement context
 * @param[in] sequence pointer to join table sequence array
 * @param[in] parsk parskey to be used
 * @param[in] jvrec join view record
 * @param[in] use_old_rescnt delete 'CreateFile' flag of result file?
 * @param[in] seqsearch_for_exec delete created mblock while sequence search
 * @param[out] b_err information code [e_ok, e_no_next_record] / error code [ ... ]
 */
externPascal void a686_create_join_result (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak_dml_info          VAR_VALUE_REF  dmli,
    const tak68_sequence VAR_VALUE_REF   sequence,
    const tak_parskey     VAR_VALUE_REF  parsk,
    const tak68_joinview_rec  VAR_VALUE_REF  jvrec,
    const pasbool                        use_old_rescnt,
    const pasbool                        seqsearch_for_exec,
    tgg00_BasisError      VAR_VALUE_REF  b_err )
{
    SQLMan_Context &_rContext = SQLMan_Context::AcvToContext(acv);

    if  ( !_rContext.IsErrorListEmpty() )
    {
        RTE_Message( _rContext.GetErrorList() );
        _rContext.ResetErrorList();
    }

    SAPDBTRACE_ROUTINE_DEBUG( "a686_create_join_result", Join_Trace, 1 );
#ifdef SAPDB_SLOW
    SAPDB_ULong _usedbytes1, _maxusedbytes, _ctrlbyted1, _usedbytes2, _ctrlbyted2;
    _rContext.GetAllocator().CalcStatistics(_usedbytes1, _maxusedbytes, _ctrlbyted1);
    vdebug_break( 77777 );
#endif

    _rContext.TransContext().trError_gg00 = e_ok;
    
    b_err = e_ok;
    
    SAPDBTRACE_IF( Join_Trace, 5, 
            t01messblock( td_always, "orig mblock ", _rContext.a_mblock ));

    //get max result
    SAPDB_Int4 _maxresult;
    k720_maxresult_get( dmli.d_rowno, _maxresult );
    if  ( _maxresult <= 1 ) 
    {   
        if (( cgg04_subquery_one_record == dmli.d_rowno || 
             cgg04_at_least_one_record == dmli.d_rowno ) &&
            ! dmli.d_pseudo_ins_select /* insert value = <select>*/ )
            // EXISTS clause, don't warn about exceeding ROWNO record count
            _maxresult = -1;
        else
            _maxresult = 1;
        SAPDBTRACE_WRITELN( Join_Trace, 3, "special maxresult : " << _maxresult );
    }
    if  ( e_ok != b_err ) _rContext.ThrowError( b_err );
    
    //get messblock of last sequenced table into context
    a682j_one_record( _rContext, dmli, dmli.d_cntfromtab, 
            sequence[dmli.d_cntfromtab-1].jos_source, 
            parsk, jvrec, use_old_rescnt, seqsearch_for_exec );

    Join_ResultDesc  _ResultDesc( _rContext );
    GetResultDesc( _rContext, _rContext.GetMessBlock(), _ResultDesc );

    SAPDB_Bool _restree_created = false; 

    if ( _rContext.IsOk() )
    {
        // create result file
        if ( ! dmli.d_single && _ResultDesc.m_ResultID.fileHandling_gg00().includes( hsCreateFile_egg00 ) )
        {
            a101_CreateGroupedTempFile (
                _rContext.TransContext(), _ResultDesc.m_ResultID,
                a101_GetExtendedTempFileType (_rContext, _ResultDesc.m_ResultID) );
            if ( e_ok != _rContext.TransContext().trError_gg00 )
                _rContext.ThrowError( _rContext.TransContext().trError_gg00 );
            else
                _restree_created = true;
        }
        SAPDB_AutoPtr<IOperator> _TopJoinOp( _rContext.GetAllocator() );
        //initialize top join operator
        if ( ! _TopJoinOp.SetPtr( new( _rContext.GetAllocator() ) 
                Join_JoinOperator2( _rContext, dmli, parsk, jvrec, 
                use_old_rescnt, seqsearch_for_exec, sequence, 
                dmli.d_cntfromtab ) ))
            _rContext.ThrowError( e_no_more_memory );

        if ( _rContext.IsOk() )
        {
            tgg00_Rec* _record;
            SAPDB_AutoPtr<Join_IResultSetOperator> 
                _ResultInserter( _rContext.GetAllocator() );

            if ( _ResultInserter.SetPtr( 
                    CreateResultSetOperator(_rContext, dmli, 
                        _ResultDesc, _maxresult, *_TopJoinOp)) &&
                 _rContext.IsOk() )
            {
                _ResultInserter->Open();
                if ( _rContext.IsOk() )
                {
                    b_err = _TopJoinOp->Open();
                    while ( e_ok == b_err &&
                          e_ok == ( b_err = _TopJoinOp->Next( _record ) ) && 
                            _rContext.IsOk() )
                    {
                        b_err = _ResultInserter->Add( *_record );
                    }
                    _TopJoinOp->Close();

                }
                _ResultInserter->Close();
                _ResultDesc.m_RecordsInResult = _ResultInserter->ResultCount();
                SAPDBTRACE_WRITELN( Join_Trace, 3, "ResultCount(): " << _ResultDesc.m_RecordsInResult );
            }
            else
                _rContext.ThrowError( e_no_more_memory );
        }
    }
    SAPDBTRACE_WRITELN( Join_Trace, 3, "b_err: " << SAPDBTrace::BasisError(b_err) );

    if ( _rContext.IsOk() && ( e_ok == b_err || e_no_next_record == b_err ))
    {
        if  ( 0 == _ResultDesc.m_RecordsInResult && _restree_created )
        {
            // what about first part of UNION , could we delete this file ?    
            a101_DestroyGroupedTempFile( _rContext.TransContext(), _ResultDesc.m_ResultID );
            a502empty_result( _rContext, dmli, _ResultDesc.m_ResultID );
        }
        _rContext.GetMessBlock().MessStructure().becomes( mbs_result );
        _rContext.GetMessBlock().MessType().becomes( m_return_result );
        _rContext.GetMessBlock().MessType2().becomes( mm_nil );
        _rContext.GetMessBlock().mb_qual()->mr_restree()   = _ResultDesc.m_ResultID;
        _rContext.GetMessBlock().mb_qual()->mr_res_build() = true;
        // set result count into messblock
        tsp00_NumError _res_err;
        s41p4int( _rContext.GetMessBlock().mb_qual()->mr_resnum(), 2, _ResultDesc.m_RecordsInResult, _res_err );
        _rContext.GetMessBlock().mb_qual()->mr_resnum()[ 0 ] = csp_defined_byte;
        _rContext.GetMessBlock().mb_qual()->mr_pagecnt() =
              _rContext.TransContext().trRteCommPtr_gg00->file_record_cnt;
        
        if  ( 0 == _ResultDesc.m_RecordsInResult || !dmli.d_single )
            _rContext.GetMessBlock().DataLength()  = 0;
        
        if ( dmli.d_single && 0 == _ResultDesc.m_RecordsInResult )
            _rContext.ThrowError( e_no_next_record );
        
        if ( dmli.d_pseudo_ins_select /* insert values comes from a <select> clause */
             && _rContext.TransContext().trWarning_gg00.includes(warn12_rowno_used) )
            _rContext.ThrowError( e_too_many_resultsets );
    }
    else
    {
        kb05ReturnErrorHandling( _rContext.TransContext(), false, m_select );
        _rContext.GetMessBlock().MessType().becomes( m_return_error );
        _rContext.GetMessBlock().MessType2().becomes( mm_nil );
        _rContext.GetMessBlock().MessStructure().becomes( mbs_nil );
        _rContext.GetMessBlock().QualLength() = 0;
        _rContext.GetMessBlock().DataLength() = 0;
        if  ( e_rollback_wanted == b_err || 
            acv.a_returncode == a071_return_code( e_rollback_wanted, _rContext.a_sqlmode))
        {
            _rContext.a_rollback_info.clear();
            _rContext.a_rollback_info.addElement( trans_rollback );
            _rContext.a_rollback_info.addElement( new_trans );
            _rContext.TransContext().trError_gg00 = e_work_rolled_back;
            _rContext.ThrowError( e_work_rolled_back );
        }
        else
        {
            _rContext.ThrowError( b_err );
        }
        if  ( seqsearch_for_exec )
        {
            // delete parsinfo's that have not been
            // deleted by ak682_tmpbuf_to_mbuf()
            a680rollback_temp_jinfo( _rContext, dmli, parsk, jvrec.jv_tabid,
                  dmli.d_cntfromtab, true );
        }
        if  ( _restree_created )
            a101_DestroyGroupedTempFile( _rContext.TransContext(), _ResultDesc.m_ResultID );
    }
    a06drop_fieldlist_references (_rContext.GetMessBlock().mb_fieldlists());

#ifdef SAPDB_SLOW
    _rContext.GetAllocator().CalcStatistics(_usedbytes2, _maxusedbytes, _ctrlbyted2);
    if ( ! _usedbytes1 == _usedbytes2 )
        RTE_Crash( SAPDBErr_Exception( __CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED, "(join exec memory leek)"));
#endif
}


namespace {
//! Create operator which adds records to result set
/*!
 * @param[in] acv global context
 * @param[in] dmli statement context
 * @param[in] result_desc describes result
 * @param[in] maxresult maximal amount of result set
 * @param[in] top_operator join operator deliveres records
 * @return pointer to a Join_IResultSetOperator corresponding desired properties like distinctness and others
 */
inline 
Join_IResultSetOperator* 
    CreateResultSetOperator( 
            SQLMan_Context&         acv,
            tak_dml_info&           dmli,
            Join_ResultDesc&        result_desc,
            const SAPDB_Int4&       maxresult, 
            IOperator&              top_operator)
{
    SAPDBTRACE_ROUTINE_DEBUG( "CreateResultSetOperator", Join_Trace, 1 );
    SAPDBTRACE_WRITELN( Join_Trace, 3, "keylen: " << top_operator.GetKeyLength() << "\tselinto: " << dmli.d_single << "\tdistinct: " << dmli.d_distinct );
    SAPDBTRACE_WRITELN( Join_Trace, 3, "a_union_kind: " << acv.a_union_kind << "\ta_union_cnt: " << acv.a_union_cnt );
    SAPDBTRACE_WRITELN( Join_Trace, 3, "prior results from mass select: " << result_desc.m_RecordsInResult );
    SAPDBTRACE_WRITELN( Join_Trace, 3, "warning about ROWNO: " << (maxresult < 0) );

    Table_FixSizeEntry_KeyNoList _key_desc( acv.GetAllocator() );
    _key_desc.Reserve( result_desc.m_FieldDesc.GetSize() );
    bool _mem_ok = true;

    for ( Table_FixSizeEntry_FieldDescription::Iterator _i = result_desc.m_FieldDesc.Begin(); _i != result_desc.m_FieldDesc.End(); ++_i )
    {
        if  ( (*_i).Offset + 1 < cgg_rec_key_offset + top_operator.GetKeyLength() )
            _mem_ok &= _key_desc.InsertEnd( result_desc.m_FieldDesc.GetIndex(_i) );
    }
                if ( !_mem_ok )
                {
                    acv.ThrowError( e_no_more_memory );
                    return 0;
                }
    
#ifdef SAPDB_SLOW
    if ( Join_Trace.TracesLevel( 3 ) )
    {
        SAPDBTRACE_WRITELN( Join_Trace, 3, "field desc: " << result_desc.m_FieldDesc.GetSize() );
        for ( Table_FixSizeEntry_FieldDescription::Iterator _i = result_desc.m_FieldDesc.Begin(); _i != result_desc.m_FieldDesc.End(); ++_i )
        {
            SAPDBTRACE_WRITELN( Join_Trace, 3, "offs " << (*_i).Offset << " \tlen " << (*_i).Length );
        }
        SAPDBTRACE_WRITELN( Join_Trace, 3, "key desc: " << _key_desc.GetSize() );
        for ( Table_FixSizeEntry_KeyNoList::Iterator _j = _key_desc.Begin(); _j != _key_desc.End(); ++_j )
        {
            SAPDBTRACE_WRITELN( Join_Trace, 3, "field idx " << *_j );
        }
    }
#endif
    
    if ( result_desc.m_Aggregation )
    {
        Join_IResultSetOperator *_p = 0;
        if ( gg01_hashed_resultset )
        {
            _p = new( acv.GetAllocator() ) 
                Join_ResultSetAggregator2( acv, result_desc.m_RownoPositions, 
                        result_desc.m_AggregationDesc.GetPtr(), result_desc.m_ResultID, 
                        result_desc.m_RecordsInResult, maxresult, dmli.d_single, 
                        result_desc.m_AggregationOneGroup, 
                        result_desc.m_FieldDesc, _key_desc,
                        top_operator );
            if ( acv.IsOk() ) 
                return _p;
            else if ( a071_return_code( e_no_more_memory, acv.a_sqlmode ) == acv.ReturnCode())
            {
                // we can't construct Join_ResultSetAggregator2
                destroy( _p, acv.GetAllocator() );
                acv.a_returncode = 0;
                acv.a_ak_b_err = e_ok;
                RTE_Message( Msg_List( Msg_List::Info, csp3_ak_msg, 
                            "JOIN", __FILE__, __LINE__, "Join_ResultSetAggregator2 failed - fall back to Join_ResultSetAggregator") );
            } else
                return 0; // any other error
        }
    
        if ( result_desc.m_AggregationOneGroup )
        {
            _p = new( acv.GetAllocator() ) 
                Join_ResultSetAggregatorOneGroup( acv, 
                        result_desc.m_RownoPositions, 
                        result_desc.m_AggregationDesc.GetPtr(), result_desc.m_ResultID, 
                        maxresult, top_operator, dmli.d_single );
            return _p;
        } else {
            _p = new( acv.GetAllocator() ) 
                Join_ResultSetAggregator( acv, result_desc.m_RownoPositions, 
                        result_desc.m_AggregationDesc.GetPtr(), result_desc.m_ResultID, 
                        result_desc.m_RecordsInResult, maxresult, top_operator );
            return _p;
        }
    }
    else
    {
        // note precedence of result set operators

        if  ( result_desc.m_DummyResult )
        {
            return new( acv.GetAllocator() ) 
                Join_ResultSetDummy( acv, 
                        result_desc.m_RownoPositions, result_desc.m_RecordsInResult, maxresult );
        }
        
        if ( dmli.d_single )
        {
            if ( dmli.d_distinct != no_distinct )
                return new( acv.GetAllocator() ) 
                    Join_ResultSetDistinctOneResult( acv, 
                            result_desc.m_RownoPositions, top_operator );
            else
                return new( acv.GetAllocator() ) 
                    Join_ResultSetOneResult( acv, 
                            result_desc.m_RownoPositions, top_operator );
        }

            if ( acv.a_union_cnt > 0 )
            {
                if ( dmli.d_distinct != no_distinct )
                {
                    return new( acv.GetAllocator() ) 
                    Join_ResultSetDistinct( acv, result_desc.m_RownoPositions, 
                            result_desc.m_ResultID, result_desc.m_RecordsInResult, maxresult );
                }
                else
                {
                    // UNION/EXCEPT/INTERSECT without any distincness
                    return new( acv.GetAllocator() ) 
                    Join_ResultSetHashInserter( acv, 
                            result_desc.m_RownoPositions, 
                            result_desc.m_ResultID, 
                            result_desc.m_RecordsInResult, maxresult );
                }
            }

                if ( dmli.d_distinct != no_distinct )
                {
                    // SELECT DISTINCT or distinct set operation
                    if ( distinct_without_warning_byte == dmli.d_distinct )
                        acv.ThrowError( e_not_implemented );
            if ( result_desc.m_DistinctResult )
                        return new( acv.GetAllocator() ) 
                            // ROWNUM makes tuple distinct !
                    Join_ResultSetAppenderEx( acv, result_desc.m_RownoPositions, 
                            result_desc.m_ResultID, result_desc.m_RecordsInResult, maxresult );
                    else
                        return new( acv.GetAllocator() ) 
                    Join_ResultSetDistinct( acv, result_desc.m_RownoPositions, 
                            result_desc.m_ResultID, result_desc.m_RecordsInResult, maxresult );
                }

                    if ( top_operator.GetKeyLength() > RESCNT_MXGG04 )
                    {
                        return new( acv.GetAllocator() ) 
                Join_ResultSetInserter( acv, result_desc.m_RownoPositions, 
                        result_desc.m_ResultID, result_desc.m_RecordsInResult, maxresult );
                    }

                        return new( acv.GetAllocator() ) 
            Join_ResultSetAppender( acv, result_desc.m_RownoPositions, 
                    result_desc.m_ResultID, result_desc.m_RecordsInResult, maxresult );
        }
}

//! extract result facts for resut set operator
/*!
 * @param[in] acv global context
 * @param[in] mblock message block for last AccessOperator
 * @param[out] result_desc describes result
 */
inline
void
    GetResultDesc(
            SQLMan_Context&         acv,
            SQLMan_MessBlock& mblock,
            Join_ResultDesc& result_desc )
{
    SAPDBTRACE_ROUTINE_DEBUG( "GetResultDesc", Join_Trace, 1 );
    // precondition acv.Messblock() contains last join messblock
    SAPDBERR_ASSERT_STATE( mm_last_operator_join == mblock.MessType2() );

    tak68_size _OutputInfoSize = *(tak68_size*)((SAPDB_Char *) mblock.StratPtr() + 
        (mblock.StackEntry( mblock.StratPos()+cgg07_OutputDesc_Offset-1).epos() - 1 ));
    SAPDBTRACE_WRITELN( Join_Trace, 5, "_OutputInfoSize: " << _OutputInfoSize );
    SAPDB_UInt4 _OutputInfoLen = sizeof( _OutputInfoSize ) + 
        _OutputInfoSize * sizeof( tak68_outcol );

    tak68_size _RownoInfoSize = *(tak68_size*)((SAPDB_Char *) mblock.StratPtr() + 
        (mblock.StackEntry( mblock.StratPos()+cgg07_OutputDesc_Offset-1).epos() - 1 )
        + _OutputInfoLen );
    SAPDBTRACE_WRITELN( Join_Trace, 5, "_RownoInfoSize: " << _RownoInfoSize );
    SAPDB_UInt4 _RownoInfoLen = sizeof( _RownoInfoSize ) +
        _RownoInfoSize * sizeof( tak68_rowno_pos );
    if ( _RownoInfoSize > 0 )
    {
        if ( !result_desc.m_RownoPositions.Reserve( _RownoInfoSize ) )
        {
            acv.ThrowError( e_no_more_memory );
            return;
        }
    }

    tak68_size _OutputResultInfoSize = *(tak68_size*)((SAPDB_Char *) mblock.StratPtr() + 
        (mblock.StackEntry( mblock.StratPos()+cgg07_OutputDesc_Offset-1).epos() - 1 )
        + _OutputInfoLen + _RownoInfoLen );
    SAPDBTRACE_WRITELN( Join_Trace, 5, "_OutputResultInfoSize: " << _OutputResultInfoSize );
    SAPDB_UInt4 _OutputResultInfoLen = sizeof( _OutputResultInfoSize ) +
            _OutputResultInfoSize * sizeof( tak68_field_desc );
    if ( _OutputResultInfoSize > 0 )
    {
        if ( !result_desc.m_FieldDesc.Reserve( _OutputResultInfoSize ) )
        {
            acv.ThrowError( e_no_more_memory );
            return;
        }
    }

    result_desc.m_Aggregation = *(pasbool*)((SAPDB_Char *) mblock.StratPtr() + 
        (mblock.StackEntry( mblock.StratPos()+cgg07_OutputDesc_Offset-1).epos() - 1 )
        + _OutputInfoLen + _RownoInfoLen + _OutputResultInfoLen );
    SAPDBTRACE_WRITELN( Join_Trace, 5, "Aggregation: " << result_desc.m_Aggregation );
    SAPDBERR_ASSERT_STATE( result_desc.m_Aggregation && mblock.HasResultOutputCols() || !result_desc.m_Aggregation )

    result_desc.m_AggregationOneGroup = *(pasbool*)((SAPDB_Char *) mblock.StratPtr() + 
        (mblock.StackEntry( mblock.StratPos()+cgg07_OutputDesc_Offset-1).epos() - 1 )
        + _OutputInfoLen + _RownoInfoLen + _OutputResultInfoLen + 
        sizeof(result_desc.m_Aggregation));
    SAPDBTRACE_WRITELN( Join_Trace, 5, "AggregationOneGroup: " << result_desc.m_AggregationOneGroup );
    
    SAPDB_UInt2 _i;
    // read ROWNO positions into vector
    SAPDB_UInt4 _pos = _OutputInfoLen + sizeof( _RownoInfoSize );
    for ( _i = 1; _i <= _RownoInfoSize; ++_i )
    {
        result_desc.m_RownoPositions.InsertEnd( *(tak68_rowno_pos*)((SAPDB_Char *) mblock.StratPtr() + 
        (mblock.StackEntry( mblock.StratPos()+cgg07_OutputDesc_Offset-1).epos() - 1 )
        + _pos ));
        _pos += sizeof( tak68_rowno_pos );
    }
#ifdef SAPDB_SLOW
    if ( Join_Trace.TracesLevel( 5 ) )
    {
        SAPDBTRACE_WRITELN( Join_Trace, 3, "ROWNO positions: " << result_desc.m_RownoPositions.GetSize() );
        for ( Container_Vector<tak68_rowno_pos>::Iterator _i = result_desc.m_RownoPositions.Begin(); _i != result_desc.m_RownoPositions.End(); ++_i )
        {
            SAPDBTRACE_WRITELN( Join_Trace, 5, "pos: " << (*_i) );
    }
    }
#endif

    // read field descriptions
    _pos = _OutputInfoLen + _RownoInfoLen + sizeof( _OutputResultInfoSize );
    for ( _i = 1; _i <= _OutputResultInfoSize; ++_i )
    {
        tak68_field_desc _field_desc = *(tak68_field_desc*)((SAPDB_Char *) mblock.StratPtr() + 
        (mblock.StackEntry( mblock.StratPos()+cgg07_OutputDesc_Offset-1).epos() - 1 )
        + _pos );
        result_desc.m_FieldDesc.InsertEnd( 
                Table_OffsetLengthTuple( _field_desc.fd_epos-1, _field_desc.fd_elen_var ) );
        _pos += sizeof( tak68_field_desc );
    }

    // read aggregation description which should described on result qualification
    if ( result_desc.m_Aggregation && mblock.HasResultOutputCols() )
    {
        SAPDB_UInt2 _stackentry_cnt = mblock.GetResOutputDescCount();
        result_desc.m_AggregationDesc.SetPtr( (tgg00_StackList*) acv.GetAllocator().Allocate( _stackentry_cnt * sizeof( tgg00_StackEntry )) );
        if (  result_desc.m_AggregationDesc.IsAssigned() )
            SAPDB_MemCopyNoCheck( result_desc.m_AggregationDesc.GetPtr(), mblock.StackEntryPtr( mblock.ResQualPos() - 1 ), 
                    _stackentry_cnt * sizeof( tgg00_StackEntry ) );
        else
            acv.ThrowError( e_no_more_memory );
#ifdef SAPDB_SLOW
        if ( Join_Trace.TracesLevel( 5 ) && result_desc.m_AggregationDesc.IsAssigned() )
    {
        SAPDBTRACE_WRITELN( Join_Trace, 1, "result description: " );
            for ( SAPDB_Int4 _i = 1; _i < (*result_desc.m_AggregationDesc)[0].epos(); ++_i)
        {
                SAPDBTRACE_IF ( Join_Trace, 5, t01stackentry( td_always, (*result_desc.m_AggregationDesc)[_i-1], _i));
        }
    }
#endif
    }

    SQLMan_Strategy *_pStrategy = mblock.GetStrategyPtr();
    
    if ( _pStrategy )
    {
        result_desc.m_ResultID  = _pStrategy->str_result_id;
        result_desc.m_RecordsInResult = _pStrategy->str_foundresults; // results already in result
        result_desc.m_DistinctResult  = _pStrategy->str_use_rowno; // write ROWNUM
        result_desc.m_DummyResult     = 
            ( strat_join_viewkey == _pStrategy->str_strategy ) && 
            result_desc.m_FieldDesc.IsEmpty();
    } 
    else
    {
        acv.GetErrorList() + 
            Msg_List( Msg_List::Error, SDBMSG_JOIN_MISSING_STRATEGY_INFORMATION );
        acv.ThrowError( e_unknown_strategy );
    }
}

} // anonymous namespace

