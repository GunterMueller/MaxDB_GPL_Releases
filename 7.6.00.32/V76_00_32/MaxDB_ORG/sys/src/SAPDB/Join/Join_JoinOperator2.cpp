/*!
 * @file
 * @brief implements Join_JoinOperator2
 *
 * @author GertG
 * @ingroup Join
 *
 * @sa Join_JoinOperator2.hpp
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
#include "gsp00.h"
#include "ggg00.h"
#include "ggg07.h"
#include "gkb07.h"
#include "RunTime/RTE_CurrentStack.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Hex.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_HexBuf.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "Join/Join_JoinOperator2.hpp"
#include "Join/Join_TableAccessOperator.hpp"
#include "Join/Join_InvAccessOperator.hpp"
#include "Join/Join_InvAccessOperatorEx.hpp"
#include "Join/Join_InvSelectIterator.hpp"
#include "Join/Join_HashAccessAllocator.hpp"
#include "Join/Join_HashAccessOperator.hpp"
#include "Join/Join_NopAccessOperator.hpp"
#include "Join/Join_SortOperator.hpp"
#include "Join/Join_LegacyAccessOperator.hpp"
#include "hak06.h"
#include "hak682.h"
#include "hak683.h"
#include "hak101.h"
#include "hak70.h"
#include "hkb74.h"
#include "hsp30.h"
#include "hgg01.h"
#include "hgg02.h"
#include "hgg20.h"
#include "hta01.h"
#include "hta01_3.h"
#include "hsp49.h"
#include "hkb71.h"
#include "hgg04.h"
#include "hbd01.h"
#include "hbd01_1.h"
#include "hbd07.h"
#include "hbd73.h"
#include "hbd998.h"
#include "hkb720.h"

extern "C"
void vdebug_break( SAPDB_Int4 );

/* ******************** PUBLIC MEMBERS ********************* */
/*! 
 * @param[in] acv global context
 * @param[in] dmli statement context
 * @param[in] parsk parskey to be used
 * @param[in] jvrec join view record
 * @param[in] use_old_rescnt delete 'CreateFile' flag of result file?
 * @param[in] del_parsinfos delete parse infos?
 * @param[in] sequence pointer to join table sequence array
 * @param[in] righttabidx table index of right table (start with idx 1)
 */
Join_JoinOperator2::Join_JoinOperator2(
        SQLMan_Context              &acv,
        tak_dml_info                &dmli,
        const tak_parskey           &parsk,
        const tak68_joinview_rec    &jvrec,
        const pasbool               use_old_rescnt,
        const pasbool               del_parsinfos,
        const tak68_sequence        &sequence,
        const SAPDB_Int4            righttabidx ):
    IOperator(acv), 
    m_LeftOp(acv.GetAllocator()), m_RightOp(acv.GetAllocator()), 
    m_JoinRec(acv.GetAllocator()), 
    m_NullRec(acv.GetAllocator()), 
    m_RightAuxRec(acv.GetAllocator()),
    m_Records(0), 
    m_Filter(acv.GetAllocator()), 
    m_OutputInfo(acv.GetAllocator()), 
    m_JoinRecBufferSize(0), 
    m_AuxRightOuterKeyLen(0),
    m_RightRecConvArrCnt(0), 
    m_RightAccessInvLen(sequence[righttabidx-TAB_OFFS_GG07].jos_invlen),
    m_LastRightDefBytePos(0), 
    m_OutputInfoSize(0),
    m_RightTableIdx(righttabidx),
    m_JoinSequence(sequence),
    // m_RightRecConvArr
    // m_JoinInfo
    // m_FilterDesc
    // m_SelFields
    m_AuxRightOuterJoinFile(b01niltree_id), 
    // m_AuxRightOuterJoinFilePos, 
    // m_AuxRightOuterJoinKey
    m_d_joins(dmli.d_joins), 
    m_RightAccessStrat(sequence[righttabidx-TAB_OFFS_GG07].jos_joinstrat),
    m_TableForRightOuterJoinScanStat(never_scanned), 
    m_IsTopJoinOp(righttabidx==dmli.d_cntfromtab), 
    m_GetLeftTupel(true), 
    m_LeftRecChanged(false), 
    m_LeftRecJoined(false),
    m_IsDescendingIndex(false),
    m_AppendRightOuterJoinRecords(false),
    m_OutputFilter(false)
{
    // precondition: acv.GetMessBlock() contains messblock for right table access op.
    SAPDBERR_ASSERT_STATE( m_RightTableIdx > 1 );

    if ( ! m_acv.IsOk() ) return;

    SAPDBTRACE_METHOD_DEBUG( "Join_JoinOperator2::ctor", Join_Trace, 1 );
    
    SAPDBTRACE_WRITELN( Join_Trace, 3, "right table idx : " << m_RightTableIdx 
            << "\taccess : " << sequence[m_RightTableIdx-TAB_OFFS_GG07].jos_joinstrat );
    
    SAPDBTRACE_IF( Join_Trace, 2, a683_output( ak_join, m_d_joins ) );
    tgg00_BasisError _b_err;

    _b_err = this->operator_init();

    // monitor
    b73cmd_count(ijoin_op_improved);
    
    if ( e_ok == _b_err )
    {
        SAPDBTRACE_WRITELN( Join_Trace, 3, "create right AccessOp" );
        // create new right AccessOperator with actual m_acv.GetMessBlock()
        if (! m_RightOp.SetPtr( this->AccessOperatorFactory(
            acv,
            sequence[m_RightTableIdx-TAB_OFFS_GG07].jos_table_buffer,
            sequence[m_RightTableIdx-TAB_OFFS_GG07].jos_expected_table_rec_reads,
            sequence[m_RightTableIdx-TAB_OFFS_GG07].jos_parallel_server,
            sequence[m_RightTableIdx-TAB_OFFS_GG07].jos_source)))
            _b_err = e_no_more_memory;
        SAPDBTRACE_WRITELN( Join_Trace, 3, "right AccessOp created!" );
        
        if ( acv.IsOk() && e_ok == _b_err )
        {
            a06drop_pars_fieldlist_refs (m_acv.GetMessBlock().mb_fieldlists());
            tgg00_FieldLists _aux_fieldlists = m_acv.GetMessBlock().mb_fieldlists();
            // get next m_acv.GetMessBlock()
            a682j_one_record( acv, dmli, m_RightTableIdx-1, 
                    sequence[m_RightTableIdx-1-TAB_OFFS_GG07].jos_source, parsk, 
                    jvrec, use_old_rescnt, del_parsinfos );
            a06drop_ex_fieldlist_refs (_aux_fieldlists);
            if ( acv.IsOk() )
            {
                if ( 2 == m_RightTableIdx )
                {
                    SAPDBTRACE_WRITELN( Join_Trace, 3, "create left AccessOp" );
                    // last join operator, therefore create left AccessOperator
                    if (! m_LeftOp.SetPtr( this->AccessOperatorFactory(
                        acv,
                        sequence[m_RightTableIdx-1-TAB_OFFS_GG07].jos_table_buffer,
                        sequence[m_RightTableIdx-1-TAB_OFFS_GG07].jos_expected_table_rec_reads,
                        sequence[m_RightTableIdx-1-TAB_OFFS_GG07].jos_parallel_server,
                        sequence[m_RightTableIdx-1-TAB_OFFS_GG07].jos_source)))
                        _b_err = e_no_more_memory;
                }
                else
                {
                    SAPDBTRACE_WRITELN( Join_Trace, 3, "create left JoinOp" );
                    // create new left JoinOperator
                    if (! m_LeftOp.SetPtr( new(m_acv.GetAllocator()) 
                                Join_JoinOperator2( acv, dmli, 
                                    parsk, jvrec, use_old_rescnt, 
                                    del_parsinfos, 
                                    sequence, m_RightTableIdx-1 )))
                        _b_err = e_no_more_memory;
                }
                SAPDBTRACE_WRITELN( Join_Trace, 3, "left Operator created!" );
            }
        }
    }

    if ( m_JoinInfo.jn_rightoj )
    {
        SAPDBERR_ASSERT_STATE( !a70glob_join_strats.includes( m_RightAccessStrat ) );

        if (! m_RightAuxRec.SetPtr( (tgg00_Rec*) 
                    m_acv.GetAllocator().Allocate( m_RightOp->GetRecordLength() )))
            _b_err = e_no_more_memory;
    }

    // clean up
    if ( _b_err != e_ok ) acv.ThrowError( _b_err );
}

/*************************************************************************/
/*!
 *
 */
Join_JoinOperator2::~Join_JoinOperator2()
{
    SAPDBTRACE_METHOD_DEBUG( "Join_JoinOperator2::dtor", Join_Trace, 1 );
    if ( this->IsOpened() ) this->Close();

    // release space for stack entries
    SAPDBTRACE_WRITELN( Join_Trace, 8, "deallocate m_Records @ " << (void*)m_Records );
    if ( m_Records ) m_acv.GetAllocator().Deallocate( m_Records );
}

/*************************************************************************/
/*!
 * @return error code [ ... ]
 */
tgg00_BasisError Join_JoinOperator2::Open()
{
    SAPDBTRACE_METHOD_DEBUG( "Join_JoinOperator2::Open", Join_Trace, 1 );
    tgg00_BasisError _e;
    // open outer stream
    _e = m_LeftOp->Open();
    if ( e_ok == _e ) _e = this->reset();
    if ( e_ok == _e ) _e = IOperator::Open();
    return _e;
}

/*************************************************************************/
/*! 
 * @param[in] startkeys restrict record stream to records with key greater/equal than startkey
 * @param[in] stopkeys restrict record stream to records with key lower/equal than stopkey
 * @return error code [ ... ]
 */
tgg00_BasisError Join_JoinOperator2::Open(
        const Join_TwoKeys &startkeys, 
        const Join_TwoKeys &stopkeys )
{
    SAPDBTRACE_METHOD_DEBUG( "Join_JoinOperator2::Open(startkeys,stopkeys)", Join_Trace, 1 );
    tgg00_BasisError _e;
    // open outer stream
    _e = m_LeftOp->Open( startkeys, stopkeys );
    if ( e_ok == _e ) _e = this->reset();
    if ( e_ok == _e ) _e = IOperator::Open( startkeys, stopkeys );
    return _e;
}


/*************************************************************************/
/*! 
 * @param[out] recptr pointer to memory should be filled with record
 * @return information code [e_ok, e_no_next_record] / error code [ ... ]
 */
tgg00_BasisError Join_JoinOperator2::Next( tgg00_Rec* &recptr )
{
    SAPDBTRACE_METHOD_DEBUG( "Join_JoinOperator2::Next", Join_Trace, 1 );
    tgg00_BasisError _b_err;
    SAPDB_Bool _qualified;

    do 
    {
        if ( e_ok == ( _b_err = this->next( m_Records ) ))
            this->create_output( m_JoinRec, m_Records );
        if ( m_Filter.IsAssigned() && e_ok == _b_err )
            _qualified = this->filter( m_JoinRec, _b_err );
        else
            _qualified = true;
    } 
    while ( e_ok == _b_err && !_qualified && m_acv.IsOk() );
    
    recptr = m_JoinRec.GetPtr();
    
    return _b_err;
}

/*************************************************************************/
/*!
 *
 */
void Join_JoinOperator2::Close()
{
    SAPDBTRACE_METHOD_DEBUG( "Join_JoinOperator2::Close", Join_Trace, 1 );
    // close outer stream
    if ( m_LeftOp->IsOpened() )
        m_LeftOp->Close();

    // close inner stream
    if ( m_RightOp->IsOpened() )
        m_RightOp->Close();
    
    if ( m_JoinInfo.jn_rightoj && 
         NIL_PAGE_NO_GG00 != m_AuxRightOuterJoinFile.fileRoot_gg00() )
    {
        b01destroy_file( m_acv.TransContext(), m_AuxRightOuterJoinFile );
        m_AuxRightOuterJoinFile.fileRoot_gg00() = NIL_PAGE_NO_GG00;
    }
    IOperator::Close();
}

/*************************************************************************/
/*!
 * @param[out] recptr record consists of NULL values
 * @return error code [ ... ]
 */
tgg00_BasisError Join_JoinOperator2::GetNullRecord( tgg00_Rec* &recptr ) 
{
    SAPDBTRACE_METHOD_DEBUG( "Join_JoinOperator2::GetNullRecord", Join_Trace, 1 );
    tgg00_BasisError _b_err = e_ok;

    if ( ! m_NullRec.IsAssigned() )
    {
        SAPDBTRACE_WRITELN( Join_Trace, 7, "create NULL record" );

        // reserve extra space (failure tolerance)
        if (! m_NullRec.SetPtr( (tgg00_Rec*) 
                    m_acv.GetAllocator().Allocate( m_JoinRecBufferSize )))
            return e_no_more_memory;
        SAPDBTRACE_WRITELN( Join_Trace, 3, "allocate m_NullRec @ " << 
                (void*)m_NullRec.GetPtr() << " with length of " << m_JoinRecBufferSize );
        SAPDBTRACE_IF( Join_Trace, 3, 
                SAPDB_MemFillNoCheck( m_NullRec.GetPtr(), ':', m_JoinRecBufferSize ) );

        tgg00_Rec** _tmprecs;
        if ( ! ( _tmprecs = 
               (tgg00_Rec**) m_acv.GetAllocator().
               Allocate( m_RightTableIdx * sizeof(tgg00_Rec*))) )
            return e_no_more_memory;

        if ( e_ok != ( _b_err = m_LeftOp->get_null_record( _tmprecs ) ))
            return _b_err;
        if ( e_ok != ( _b_err = m_RightOp->get_null_record( _tmprecs ) )) 
            return _b_err;
        this->create_output( m_NullRec, _tmprecs );

        if ( m_Filter.IsAssigned() && m_OutputFilter ) 
            (void) this->filter( m_NullRec, _b_err );
        
        m_acv.GetAllocator().Deallocate( _tmprecs );
    }
    recptr = m_NullRec.GetPtr();
    SAPDBTRACE_IF( Join_Trace, 5, 
        t01buf( td_always, recptr, 1, recptr->recLen_gg00() <= 300 ? 
            recptr->recLen_gg00() : 300 ));
    return _b_err;
}

/* ******************* PROTECTED MEMBERS ******************* */

/*! 
 * @param[in,out] records vector of record pointers to be filled with involved record pointers 
 * @return information code [e_ok, e_no_next_record] / error code [ ... ]
 *
 */
tgg00_BasisError Join_JoinOperator2::next( tgg00_Rec* *records )
{
    SAPDBTRACE_METHOD_DEBUG( "Join_JoinOperator2::next(<vector>)", Join_Trace, 1 );
    SAPDBTRACE_STREAMVAR( _TraceStream, Join_Trace );

    tgg00_BasisError _left_err  = e_ok;
    tgg00_BasisError _right_err = e_ok;
    tgg00_BasisError _b_err;
    
    const SAPDB_UInt4 _c_MinStackUsage = 1024;
    SAPDB_UInt4 _FreeStack = RTE_CurrentStackFree(m_acv.TransContext().trTaskId_gg00);
    if ( _FreeStack < _c_MinStackUsage  ) 
    {
        SAPDBTRACE_WRITELN( Join_Trace, 1, "Join_JoinOperator2::next free stack is to small: " << _FreeStack );
        return e_program_stack_overflow;
    }
                
    
    do
    {
        _b_err = e_ok;
        if ( ! m_AppendRightOuterJoinRecords )
        {
            if ( m_GetLeftTupel )
            {
                m_GetLeftTupel    = false;
                m_LeftRecChanged  = false;
                m_LeftRecJoined   = false;
                SAPDBTRACE_STREAMWRITELN( _TraceStream, Join_Trace, 3, "<< get next left record from " << m_RightTableIdx - 1 << ". tab" );
                _left_err = m_LeftOp->next( records );
                SAPDBTRACE_STREAMWRITELN( _TraceStream, Join_Trace, 5, "left operator state " << SAPDBTrace::BasisError(_left_err) );
                if ( e_no_next_record == _left_err )
                {
                    if ( m_JoinInfo.jn_rightoj )
                    {
                        m_AppendRightOuterJoinRecords = true;
                    }
                    else
                    {
                        // don't close left file to ensure repeatable next() operation
                        //m_LeftOp->Close();

                        // inner join and no left record 
                        SAPDBTRACE_STREAMWRITELN( _TraceStream, Join_Trace, 5, "end of next @ " << (void *)this );
                        SAPDBTRACE_STREAMWRITELN( _TraceStream, Join_Trace, 5, "because of left e_no_next_record" );
                        return e_no_next_record;
                    }
                }
                else if ( e_ok == _left_err )
                {
                    SAPDBTRACE_STREAMWRITELN( _TraceStream, Join_Trace, 3, "left record (len=" << m_LeftOp->GetRecordLength() << ") @ " << (void *) this );
                    if ( a70glob_join_strats.includes( m_RightAccessStrat ) )
                        // set m_Startkeys, m_Stopkeys
                        _right_err = prepare_right_key( records );
                    else
                        _right_err = e_ok;
                }
                else 
                    return _left_err;
            }

            if ( e_ok == _left_err )
            {
                SAPDB_Bool _joining;
                
                // open right operator
                if ( ! m_RightOp->IsOpened() )
                {
                    if ( a70glob_join_strats.includes( m_RightAccessStrat ) )
                    {
                        if ( m_JoinInfo.jn_rightoj && never_scanned == m_TableForRightOuterJoinScanStat )
                        {
                            SAPDBTRACE_STREAMWRITELN( _TraceStream, Join_Trace, 5, "scan for RIGHT OUTER JOIN" );
                            _right_err = m_RightOp->Open();
                        }
                        else
                        {
                            tgg00_BasisError _err = m_RightOp->Open( m_Startkeys, m_Stopkeys );
                            if  ( e_ok == _right_err ) 
                                _right_err = _err;
                            else
                                ; // prepare_right_key() could given 'e_no_next_record'
                        }
                    }
                    else
                        // use one table strategy
                        _right_err = m_RightOp->Open();
                    SAPDBTRACE_STREAMWRITELN( _TraceStream, Join_Trace, 5, "right operator state " << SAPDBTrace::BasisError(_right_err) );
                }

                do 
                {
                    SAPDBTRACE_STREAMWRITELN( _TraceStream, Join_Trace, 3, ">> get next right record from " << m_RightTableIdx << ". tab (" << m_RightAccessStrat << ")");
                    if ( e_ok == _right_err ) 
                        _right_err = m_RightOp->next( records );
                    if ( e_ok == _right_err )
                    {
                        if ( never_scanned == m_TableForRightOuterJoinScanStat )
                            m_TableForRightOuterJoinScanStat = while_scanning;

                        if ( _joining = this->join( records, _b_err ) ) 
                            m_LeftRecJoined = true;
                            
                        if ( m_JoinInfo.jn_rightoj )
                        {
                            if ( !_joining && while_scanning == m_TableForRightOuterJoinScanStat )
                            {
                                /* !!! RIGHT OUTER JOIN !!! */
                                // add record with no join to auxiliary file
                                SAPDBTRACE_STREAMWRITELN( _TraceStream, Join_Trace, 3, "add to ROJ aux file" << " m_AuxRightOuterKeyLen: " << m_AuxRightOuterKeyLen );

                                (*(records+(m_RightTableIdx-TAB_OFFS_GG07)))->recLen_gg00()    = m_RightOp->GetRecordLength();
                                (*(records+(m_RightTableIdx-TAB_OFFS_GG07)))->recKeyLen_gg00() = m_AuxRightOuterKeyLen;
                                (*(records+(m_RightTableIdx-TAB_OFFS_GG07)))->recVarcolOffset_gg00() = 0;
                                (*(records+(m_RightTableIdx-TAB_OFFS_GG07)))->recVarcolCnt_gg00()    = 0;
                                
                                SAPDBTRACE_IF( Join_Trace, 4, 
                                t01buf( td_always, (*(records+(m_RightTableIdx-TAB_OFFS_GG07))), 1, (*(records+(m_RightTableIdx-TAB_OFFS_GG07)))->recLen_gg00() <= 300 ? (*(records+(m_RightTableIdx-TAB_OFFS_GG07)))->recLen_gg00() : 300 ));
                                b07cadd_record( m_acv.TransContext(), m_AuxRightOuterJoinFile, *(*(records+(m_RightTableIdx-TAB_OFFS_GG07))) );
                                _right_err = m_acv.TransContext().trError_gg00;
                            }

                            if ( _joining && already_scanned == m_TableForRightOuterJoinScanStat )
                            {
                                SAPDBTRACE_STREAMWRITELN( _TraceStream, Join_Trace, 3, "delete from ROJ aux file" );
                                SAPDBTRACE_IF( Join_Trace, 4, 
                                t01buf( td_always, &((*(records+(m_RightTableIdx-TAB_OFFS_GG07)))->recBuf_gg00()), 1, m_RightOp->GetRecordLength() <= 300 ? m_RightOp->GetRecordLength() : 300 ));
                                
                                SAPDB_Int2 _keylen = (*(records+(m_RightTableIdx-TAB_OFFS_GG07)))->recKey_gg00().keyLen_gg00();
                                (*(records+(m_RightTableIdx-TAB_OFFS_GG07)))->recKey_gg00().keyLen_gg00() = m_AuxRightOuterKeyLen;
                                b07cdel_record( m_acv.TransContext(), m_AuxRightOuterJoinFile, (*(records+(m_RightTableIdx-TAB_OFFS_GG07)))->recKey_gg00() );
                                // ignore already joined records
                                if ( e_key_not_found == m_acv.TransContext().trError_gg00 )
                                    m_acv.TransContext().trError_gg00 = e_ok;
                                _right_err = m_acv.TransContext().trError_gg00;
                                (*(records+(m_RightTableIdx-TAB_OFFS_GG07)))->recKey_gg00().keyLen_gg00() = _keylen;
                            }
                        }
                    }

                    // test for command canceling
                    if ( m_acv.TransContext().trRteCommPtr_gg00->to_cancel )
                        _right_err = e_cancelled;
                }
                while ( e_ok == _right_err && !_joining );

                if ( e_no_next_record == _right_err )
                {
                    // no more right records -> switch inner state
                    m_GetLeftTupel = true;
                    if ( while_scanning == m_TableForRightOuterJoinScanStat )
                        m_TableForRightOuterJoinScanStat = already_scanned;
                    if ( !m_LeftRecJoined && m_JoinInfo.jn_leftoj )
                    {
                        SAPDBTRACE_STREAMWRITELN( _TraceStream, Join_Trace, 5, "append LEFT OUTER record " );
                        /* !!! LEFT OUTER JOIN !!! */
                        _right_err = m_RightOp->get_null_record( records );
                    }
                    m_RightOp->Close();
                }
            }
        }

        if ( m_AppendRightOuterJoinRecords )
        {
            SAPDBTRACE_STREAMWRITELN( _TraceStream, Join_Trace, 5, "append RIGHT OUTER record " );
            
            // tie NULL record on left side
            _left_err = m_LeftOp->get_null_record( records );
            if  ( e_ok != _left_err ) return _left_err;

            if ( never_scanned == m_TableForRightOuterJoinScanStat )
            {
                /* !!! RIGHT OUTER JOIN for empty left table !!! */
                // left table delivers no records, therfore m_AuxRightOuterJoinFile
                // is empty but we have to select whole right table
                
                if ( ! m_RightOp->IsOpened() ) _right_err = m_RightOp->Open();

                if ( e_ok == _right_err )
                    _right_err = m_RightOp->next( records );

                if  ( e_no_next_record == _right_err)
                {
                    m_RightOp->Close();
                    m_GetLeftTupel = true;
                }
            }
            else
            {
                tgg00_BdSetResultRecord _set_result;
                _set_result.bd_key_check_len = 0;
                _set_result.bd_max_rec_cnt   = 1;
                _set_result.bd_max_fill_len  = m_RightOp->GetRecordLength();
                _set_result.bd_next          = true;
                _set_result.bd_drop_page     = false;
                
                // fill m_RightAuxRec with unmatched right record 
                b07cnext_record( m_acv.TransContext(), 
                        m_AuxRightOuterJoinFile, 
                        m_AuxRightOuterJoinKey, _set_result, 
                        m_AuxRightOuterJoinFilePos, 
                        m_RightAuxRec.GetPtr() );
                if ( e_key_not_found == m_acv.TransContext().trError_gg00 )
                    m_acv.TransContext().trError_gg00 = e_ok;
                if ( e_ok == m_acv.TransContext().trError_gg00 )
                {
                    SAPDBTRACE_IF( Join_Trace, 3, 
                    t01buf( td_always, m_RightAuxRec.GetPtr(), 1, 
                        m_RightAuxRec->recLen_gg00() <= 300 ? 
                        m_RightAuxRec->recLen_gg00() : 300 ));

                    *(records+(m_RightTableIdx-TAB_OFFS_GG07)) = m_RightAuxRec.GetPtr();
                }
                _right_err = m_acv.TransContext().trError_gg00;
            }

            if ( e_no_next_record == _right_err )
            {
                // don't close left file to ensure repeatable next() operation
                // m_LeftOp->Close();

                // RIGHT OUTER JOIN and no more right records
                SAPDBTRACE_STREAMWRITELN( _TraceStream, Join_Trace, 5, 
                        "end of next @ " << (void *)this );
                SAPDBTRACE_STREAMWRITELN( _TraceStream, Join_Trace, 5, 
                        "because of ROJ e_no_next_record" );
                return e_no_next_record;
            }
        } // m_AppendRightOuterJoinRecords
    }
    while ( e_no_next_record == _right_err && e_ok == _b_err && m_acv.IsOk() );

    // reach this if _right_err == _b_err == e_ok or sudden error situation
    
    SAPDBTRACE_STREAMWRITELN( _TraceStream, Join_Trace, 5, "end of next @ " << (void *)this );
#ifdef SAPDB_SLOW
    if ( e_ok != _b_err )
    {
        SAPDBTRACE_STREAMWRITELN( _TraceStream, Join_Trace, 5, "return "  << SAPDBTrace::BasisError(_b_err) );
    }
    else
    {
        SAPDBTRACE_STREAMWRITELN( _TraceStream, Join_Trace, 5, "return "  << SAPDBTrace::BasisError(_right_err) );
    }
#endif
    return ( e_ok != _b_err) ? _b_err : _right_err;
}

/*************************************************************************/
/*!
 * @param[out] record vector of record to be filled
 * @return error code [ ... ]
 */
tgg00_BasisError Join_JoinOperator2::get_null_record( tgg00_Rec** records ) 
{
    SAPDBTRACE_METHOD_DEBUG( "Join_JoinOperator2::get_null_record(<vector>)", Join_Trace, 1 );
    tgg00_BasisError _b_err;

    if ( e_ok != ( _b_err = m_LeftOp->get_null_record( records )) )
        return _b_err;
    _b_err = m_RightOp->get_null_record( records );
    return _b_err;
}

/* ******************** PRIVATE MEMBERS ******************** */
/*!
 * @return error code [ ... ]
 */
tgg00_BasisError Join_JoinOperator2::operator_init()
{
    // extract join info and adjust messblock for BD calls
    SAPDBTRACE_METHOD_DEBUG( "Join_JoinOperator2::operator_init", Join_Trace, 3 );

    SAPDB_MemCopyNoCheck( &m_JoinInfo, 
            (SAPDB_Char *)m_acv.GetMessBlock().StratPtr() + 
            (m_acv.GetMessBlock().StackEntry( m_acv.GetMessBlock().StratPos()+cgg07_JoinInfo_Offset-1 ).epos() - 1 ), 
            m_acv.GetMessBlock().StackEntry( m_acv.GetMessBlock().StratPos()+cgg07_JoinInfo_Offset-1 ).elen_var() );
    SAPDBTRACE_IF( Join_Trace, 3, 
        a683trace_joininfo( td_always, m_d_joins, m_JoinInfo ));

    if ( m_IsTopJoinOp )
    {
        m_OutputInfoSize = *(tak68_size*)((SAPDB_Char *) m_acv.GetMessBlock().StratPtr() + 
            (m_acv.GetMessBlock().StackEntry( m_acv.GetMessBlock().StratPos()+cgg07_OutputDesc_Offset-1).epos() - 1 ));
        SAPDBTRACE_WRITELN( Join_Trace, 3, "m_OutputInfoSize: " << m_OutputInfoSize );
        if ( m_OutputInfo.SetPtr( (tak68_outcol*) 
                    m_acv.GetAllocator().Allocate( m_OutputInfoSize * sizeof( tak68_outcol ) )))
        {
            SAPDBTRACE_WRITELN( Join_Trace, 3, "allocate m_OutputInfo @ " << (void*)m_OutputInfo.GetPtr() );
            SAPDB_MemCopyNoCheck( m_OutputInfo.GetPtr(), (SAPDB_Char *) m_acv.GetMessBlock().StratPtr() + 
                    (m_acv.GetMessBlock().StackEntry(m_acv.GetMessBlock().StratPos()+cgg07_OutputDesc_Offset-1).epos() - 1 ) + 
                    sizeof( tak68_size ), 
                    m_OutputInfoSize * sizeof( tak68_outcol ) );
#ifdef SAPDB_SLOW
            for ( tak68_size _i = 0; _i < m_OutputInfoSize; ++_i )
            {
                SAPDBTRACE_WRITELN( Join_Trace, 3, 
                           "oc_srctab: "   << (m_OutputInfo.GetPtr()+_i)->oc_srctab 
                        << "\toc_srcpos: " << (m_OutputInfo.GetPtr()+_i)->oc_srcpos 
                        << "\toc_dstpos: " << (m_OutputInfo.GetPtr()+_i)->oc_dstpos 
                        << "\toc_len: "    << (m_OutputInfo.GetPtr()+_i)->oc_len << NewLine );
            }
#endif
        }
        else
            return e_no_more_memory;

        // reserve extra space (failure tolerance)
        if ( this->join_reclen() > this->GetRecordLength() )
            m_JoinRecBufferSize = this->join_reclen() + 20;
        else
            m_JoinRecBufferSize = this->GetRecordLength() + 20;
        if (! m_JoinRec.SetPtr( (tgg00_Rec*) m_acv.GetAllocator().Allocate( m_JoinRecBufferSize )))
            return e_no_more_memory;
        else
        {
            SAPDBTRACE_WRITELN( Join_Trace, 3, m_JoinRecBufferSize << " bytes for join buffer allocated" );
            SAPDBTRACE_WRITELN( Join_Trace, 3, "allocate m_JoinRec @ " << (void*)m_JoinRec.GetPtr() );
        }
        tgg00_BasisError _b_err = e_ok;
        if ( m_acv.GetMessBlock().ResQualPos() > 0 && m_acv.GetMessBlock().ResQualCount() > 0 )
            _b_err = this->create_filter( m_acv.GetMessBlock().HasResultOutputCols() );
        if  ( e_ok != _b_err ) return _b_err;
        
        if ( ! ( m_Records = (tgg00_Rec**) 
                m_acv.GetAllocator().Allocate( m_RightTableIdx * sizeof(tgg00_Rec*)) ) )
            return e_no_more_memory;

    }
    
    if ( a70glob_join_strats.includes( m_RightAccessStrat ) )
    {
        // get information on changing data types ascii<->ebcdic before joining
        if ( a70glob_join_key_strats.includes( m_RightAccessStrat ) )
        {
            k74get_last_field_pos( m_acv.GetMessBlock(), 
                      m_LastRightDefBytePos, m_JoinInfo.jn_jarr_path );
        }
        else
        {
            k74check_inv_conversions( m_acv.GetMessBlock(), 
                      m_RightRecConvArr, m_RightRecConvArrCnt, 
                      m_LastRightDefBytePos, m_IsDescendingIndex, 
                      m_JoinInfo.jn_jarr_path );
        }
    }
    else
        k74get_last_field_pos( m_acv.GetMessBlock(), 
                  m_LastRightDefBytePos, m_JoinInfo.jn_jarr_path );

    if ( m_JoinInfo.jn_rightoj )
    {
        // determine auxiliary RIGHT OUTER JOIN file name
        g04build_temp_tree_id( m_AuxRightOuterJoinFile, m_acv.TransContext() );
        m_AuxRightOuterJoinFile.fileTfnTemp_gg00().becomes( ttfnOuterJoin_egg00 );
        a101_SetTempFileIndex (m_acv, m_AuxRightOuterJoinFile, m_RightTableIdx);
        m_AuxRightOuterJoinFilePos.tpsPno_gg00 = NIL_PAGE_NO_GG00;
        m_AuxRightOuterJoinKey.len() = 0;
        m_AuxRightOuterKeyLen = this->get_AuxRightOuterKeyLen();
        SAPDBTRACE_WRITELN( Join_Trace, 3, "m_AuxRightOuterKeyLen : " << m_AuxRightOuterKeyLen );
        
    }
    return e_ok;
}

/*************************************************************************/
/*!
 * @param[in] is_outputfilter filter contains output values
 * @return error code [ ... ]
 */
tgg00_BasisError Join_JoinOperator2::create_filter( SAPDB_Bool is_outputfilter )
{
    SAPDBTRACE_METHOD_DEBUG( "Join_JoinOperator2::create_filter", Join_Trace, 3 );
    
    tgg00_BasisError _b_err = e_ok;
    m_FilterDesc = m_acv.a_mblock.mb_qual()->mstack_desc();
    if ( m_Filter.SetPtr( (tgg00_StackList*) 
                    m_acv.GetAllocator().Allocate( m_acv.GetMessBlock().ResQualCount() * sizeof( tgg00_StackEntry ) ) ) )
    {
        SAPDBTRACE_WRITELN( Join_Trace, 3, "allocate m_Filter @ " << (void*)m_Filter.GetPtr() );
        SAPDB_MemCopyNoCheck( m_Filter.GetPtr(), (tgg00_StackEntry *)m_acv.a_mblock.mb_st() + ( m_acv.GetMessBlock().ResQualPos() - 1 ), 
                m_acv.GetMessBlock().ResQualCount() * sizeof( tgg00_StackEntry ) );
    }
    else
        _b_err = e_no_more_memory;

    m_FilterDesc.mst_addr()         = m_Filter.GetPtr();
    m_FilterDesc.mqual_cnt = m_acv.GetMessBlock().ResQualCount();
    m_FilterDesc.mqual_pos = 1;
    m_FilterDesc.mst_max() = m_FilterDesc.mqual_cnt;
    m_FilterDesc.mfirst_free() = m_FilterDesc.mqual_cnt + 1;
    m_FilterDesc.mst_optimize_pos() = 0;
    m_FilterDesc.mstrat_pos         =  m_FilterDesc.mstrat_cnt     = 0;
    m_FilterDesc.mresqual_pos()     =  m_FilterDesc.mresqual_cnt() = 0;
    m_FilterDesc.mcol_pos           =  m_FilterDesc.mcol_cnt       = 0;
    m_FilterDesc.mmult_pos          =  m_FilterDesc.mmult_cnt      = 0;
    m_FilterDesc.mview_pos          =  m_FilterDesc.mview_cnt      = 0;
    m_FilterDesc.mupd_pos           =  m_FilterDesc.mupd_cnt       = 0;
    m_FilterDesc.mlink_pos          =  m_FilterDesc.mlink_cnt      = 0;
    m_FilterDesc.mstring_pos        =  m_FilterDesc.mstring_cnt    = 0;
    m_FilterDesc.mtrigger_pos       =  m_FilterDesc.mtrigger_cnt   = 0;
    m_FilterDesc.minvqual_pos       =  m_FilterDesc.minvqual_cnt   = 0;

    SAPDBTRACE_IF( Join_Trace, 3, 
    t01stackdesc( td_always, "copied rqual", m_FilterDesc.mst_addr(), m_FilterDesc ));
    SAPDBTRACE_WRITELN( Join_Trace, 3, "dataptr @ " << (void*)m_acv.GetMessBlock().DataPtr() );
    SAPDBTRACE_IF( Join_Trace, 5, t01buf( td_always, m_acv.GetMessBlock().DataPtr(), 1, m_acv.GetMessBlock().DataLength() ));

    g04init_select_fields( m_SelFields, 
            m_acv.GetMessBlock().DataPtr(),  
            m_acv.GetMessBlock().DataLength(), 
            m_acv.a_work_st_addr, 
            m_acv.a_work_st_max, 
            m_acv.a_work_buf_addr, 
            m_acv.a_work_buf_size,
            m_acv.a_sqlmode,
            &m_acv.a_mblock.mb_fieldlists() );
    m_SelFields.sfp_bd_mess_type().becomes( m_select );
    m_SelFields.sfp_result_wanted() = true;
    m_SelFields.sfp_m_result_size() = m_JoinRecBufferSize;
    m_SelFields.sfp_first_qual()      = true;
    m_SelFields.sfp_filler2()         = false;
    m_SelFields.sfp_rows_read()       = 0;
    m_SelFields.sfp_rows_qual()       = 0;
    m_SelFields.sfp_acv_addr()        = reinterpret_cast<tsp00_Addr>( &m_acv );
    m_SelFields.sfp_oldrec_addr()     = 0;
    m_SelFields.sfp_primkey_addr()    = 0;
    //SAPDBTRACE_IF( Join_Trace, 3, t01selfields( td_always, "m_SelFields ", m_SelFields ));
    SAPDBTRACE_WRITELN( Join_Trace, 3, "is output filter " << is_outputfilter );
    m_OutputFilter = is_outputfilter;

    /* remove result qualification from message block */
    m_acv.GetMessBlock().FirstFree()  = m_acv.GetMessBlock().ResQualPos();
    m_acv.GetMessBlock().ResQualPos() = m_acv.GetMessBlock().ResQualCount() = 0;

    if ( m_acv.a_mblock.mb_qual()->mstack_state().includes( ssSubquery_egg00 ) )
    {
        tgg00_Rec* _rec = reinterpret_cast<tgg00_Rec*>( m_acv.GetAllocator().Allocate( sizeof(tgg00_Rec) ) );;
        if ( _rec )
        {
            k720_test_subquery( m_acv.TransContext(), 
                *reinterpret_cast<tgg00_DataPartPtr>(m_acv.GetMessBlock().DataPtr()), 
                m_acv.GetMessBlock().DataLength(),
                m_FilterDesc, *_rec );
            _b_err = m_acv.TransContext().trError_gg00;
            m_acv.GetAllocator().Deallocate( _rec );
        }
        else
            _b_err = e_no_more_memory;
    }
    return _b_err;
}

/*************************************************************************/
/*!
 * @param[in] records vector of records involved in join
 * @return error code [ ... ]
 */
tgg00_BasisError Join_JoinOperator2::prepare_right_key( tgg00_Rec* *records )
{
    SAPDBTRACE_METHOD_DEBUG( "Join_JoinOperator2::prepare_right_key", Join_Trace, 3 );
    SAPDBERR_ASSERT_STATE( a70glob_join_strats.includes( m_RightAccessStrat ) );
    SAPDBERR_ASSERT_STATE( m_JoinInfo.jn_jarr_path > 0 );
        
    SAPDBTRACE_WRITELN( Join_Trace, 5, "prepare for access: " << m_RightAccessStrat << " (" << m_JoinInfo.jn_jarr_path << ")");
    
    // TODO: if we have non-equi joins we have to adjust start/stop key according used join operator

    SAPDB_Int2 _last_field_len; 
    tgg00_BasisError _b_err = e_ok;

    if ( a70glob_join_key_strats.includes( m_RightAccessStrat ) )
    {
        // set primary startkey
        _b_err = this->build_right_startkey( records, m_Startkeys.reckey, _last_field_len );

        // conversions not neccessary
        switch ( m_RightAccessStrat )
        {
            // complete key access
            case strat_join_viewkey:
            case strat_join_key_equal:
            case strat_join_all_keys_equal:
                // last key is cutted off
                SAPDB_Char _defbyte; 
                _defbyte = m_Startkeys.reckey.keyVal_gg00()[ (m_Startkeys.reckey.keyLen_gg00()-_last_field_len+1) - 1 ];
                SAPDBTRACE_WRITELN( Join_Trace, 5, "last defbyte: " << SAPDBTrace_Hex(&_defbyte, 1) << ", act key len: " << m_Startkeys.reckey.keyLen_gg00() );
                m_Startkeys.reckey.keyLen_gg00() -= 
                    // last inoutlen
                    _last_field_len -
                    ( s30lnr_defbyte( &m_Startkeys.reckey.keyVal_gg00(), _defbyte, 
                        (m_Startkeys.reckey.keyLen_gg00()-_last_field_len+1) + 1,
                        _last_field_len - 1 ) 
                    + 1 /* defbyte lenght */);
                SAPDBTRACE_WRITELN( Join_Trace, 5, "key len: " << m_Startkeys.reckey.keyLen_gg00() );
                // set primary stopkey
                m_Stopkeys.reckey.keyLen_gg00() = m_Startkeys.reckey.keyLen_gg00();
                SAPDB_MemCopyNoCheck( &m_Stopkeys.reckey.keyVal_gg00(), 
                        &m_Startkeys.reckey.keyVal_gg00(), 
                        m_Stopkeys.reckey.keyLen_gg00() );
                break;
            // key part access
            case strat_join_key_next:
            case strat_join_key_range:
                // set primary startkey, already done
                // set primary stopkey
                m_Stopkeys.reckey.keyLen_gg00() = 
                       sizeof( m_Stopkeys.reckey.keyVal_gg00() );
                SAPDB_MemCopyNoCheck( &m_Stopkeys.reckey.keyVal_gg00(), 
                        &m_Startkeys.reckey.keyVal_gg00(), 
                        m_Startkeys.reckey.keyLen_gg00() );
                // fill stopkey with 0xFF
                SAPDB_MemFillNoCheck( ((SAPDB_Byte*)&m_Stopkeys.reckey.keyVal_gg00()) + 
                        m_Startkeys.reckey.keyLen_gg00(), 0xFF, 
                        sizeof(m_Stopkeys.reckey.keyVal_gg00()) - 
                        m_Startkeys.reckey.keyLen_gg00() );
                break;
            default:
                SAPDBTRACE_WRITELN( Join_Trace, 1, "unknown strategy: " << m_RightAccessStrat << NewLine << __FILE__ << " line " << __LINE__ );
                return e_unknown_strategy;
        }
    }
    else
    {
        // set index startkey
        _b_err = this->build_right_startkey( records, m_Startkeys.listkey, _last_field_len );

        switch ( m_RightAccessStrat )
        {
            // complete index access
            case strat_join_inv:
            case strat_join_all_inv_equal:
                // last key is cutted off
                SAPDB_Char _defbyte; 
                _defbyte = m_Startkeys.listkey.keyVal_gg00()[ (m_Startkeys.listkey.keyLen_gg00()-_last_field_len+1) - 1 ];
                SAPDBTRACE_WRITELN( Join_Trace, 5, "last defbyte: " << SAPDBTrace_Hex(&_defbyte, 1) << ", act key len: " << m_Startkeys.listkey.keyLen_gg00() );
                m_Startkeys.listkey.keyLen_gg00() -= 
                    // last inoutlen
                    _last_field_len -
                    ( s30lnr_defbyte( &m_Startkeys.listkey.keyVal_gg00(), _defbyte, 
                        (m_Startkeys.listkey.keyLen_gg00()-_last_field_len+1) + 1,
                        _last_field_len - 1 ) 
                    + 1 /* defbyte lenght */);
                SAPDBTRACE_WRITELN( Join_Trace, 5, "build inv len: " << m_Startkeys.listkey.len()
                        << "\tis descending: " << m_IsDescendingIndex 
                        << "\nindex length: " << m_RightAccessInvLen );
                // ascii/ebcdic, descending conversion
                this->convert_key( m_Startkeys.listkey );
                if ( m_IsDescendingIndex && 
                     m_RightAccessInvLen > m_Startkeys.listkey.len() )
                {
                    // we have to fill invkey for descending indexes with define 
                    // bytes because descending key are blown up to full length
                    if ( (0xFF - csp_unicode_def_byte) == m_Startkeys.listkey.k()[ m_LastRightDefBytePos - 1 - cgg_rec_key_offset ] )
                    {
                        g20unifill( sizeof(m_Startkeys.listkey.k()), 
                                &m_Startkeys.listkey.k(), 
                                m_Startkeys.listkey.len()+1,
                                m_RightAccessInvLen - m_Startkeys.listkey.len(),
                                csp_unicode_blank_desc );
                    }
                    else
                    {
                        SAPDB_MemFillNoCheck( (SAPDB_Byte*)&m_Startkeys.listkey.k() + m_Startkeys.listkey.len(), 
                                m_Startkeys.listkey.k()[ m_LastRightDefBytePos - cgg_rec_key_offset - 1 ], 
                                m_RightAccessInvLen - m_Startkeys.listkey.len() );
                    }
                    m_Startkeys.listkey.len() = m_RightAccessInvLen;
                    SAPDBTRACE_WRITELN( Join_Trace, 5, "set listkey.len(): " << m_Startkeys.listkey.len() );
                }
                // set index stopkey
                m_Stopkeys.listkey.len() = m_Startkeys.listkey.len();
                SAPDB_MemCopyNoCheck( &m_Stopkeys.listkey.k(), 
                        &m_Startkeys.listkey.k(), m_Startkeys.listkey.len() );
                break;
            // index part access
            case strat_join_inv_range:
                // set index startkey, already done
                // ascii/ebcdic, descending conversion
                this->convert_key( m_Startkeys.listkey );
                // set index stopkey
                m_Stopkeys.listkey.len() = sizeof(m_Stopkeys.listkey.k());
                SAPDB_MemCopyNoCheck( &m_Stopkeys.listkey.k(), &m_Startkeys.listkey.k(), 
                        m_Startkeys.listkey.len() );
                SAPDB_MemFillNoCheck( ((SAPDB_Byte*)&m_Stopkeys.listkey.keyVal_gg00()) + 
                        m_Startkeys.listkey.keyLen_gg00(), 0xFF, 
                        sizeof(m_Stopkeys.listkey.keyVal_gg00()) - 
                        m_Startkeys.listkey.keyLen_gg00() );
                break;
            default:
                SAPDBTRACE_WRITELN( Join_Trace, 1, "unknown strategy: " << m_RightAccessStrat << NewLine << __FILE__ << " line " << __LINE__ );
                return e_unknown_strategy;
        }

        // set primary start/stopkey
        m_Startkeys.reckey.len() = 0;
        m_Stopkeys.reckey.len() = 0;
    }

    // too long key could return e_no_next_record
    return _b_err;
}

/*************************************************************************/
/*!
 * @param[in] records vector of records involved in join
 * @param[out] key built key consists of values from param 'records'
 * @return error code [ ... ]
 */
tgg00_BasisError Join_JoinOperator2::build_right_startkey( 
        tgg00_Rec* *records, 
        tgg00_Lkey &key,
        SAPDB_Int2 &last_field_len )
{
    SAPDBTRACE_METHOD_DEBUG( "Join_JoinOperator2::build_right_startkey", Join_Trace, 4 );
    SAPDBTRACE_IF( Join_Trace, 6, 
            SAPDB_MemFillNoCheck( &key, ':', sizeof( key ) ) );
    tak_one_join _joinpred;
    SAPDB_Int2 _src_tabno, _src_side; 
    tgg00_BasisError _b_err = e_ok;

    last_field_len = key.keyLen_gg00() = 0;
    for ( SAPDB_UInt2 _i = 1; _i <= m_JoinInfo.jn_jarr_path; ++_i )
    {
        // copy key parts from different tables
        SAPDBTRACE_IF( Join_Trace, 4, 
        a683_one_join_entry( td_always, m_d_joins, m_JoinInfo.jn_jarr[ _i - 1 ], false ));
        _joinpred = (*m_d_joins.jrc_joinarr)[ m_JoinInfo.jn_jarr[ _i - 1 ] ];
        if ( 0 == (_joinpred.jo_partno-1) /* _dst_side */ ) 
            _src_side = 1; 
        else 
            _src_side = 0;
        SAPDBTRACE_WRITELN( Join_Trace, 6, "_src_side: " << _src_side << " _dst_side: " << _joinpred.jo_partno - 1 );
        
        if ( cak68_join_value == _joinpred.jo_recs[ _src_side ].jop_tableno 
            ||
             cak68_join_value == _joinpred.jo_recs[ _joinpred.jo_partno-1 ].jop_tableno )
        {
            _src_tabno = m_JoinSequence[m_RightTableIdx-1-TAB_OFFS_GG07].jos_source;
            SAPDBTRACE_WRITELN( Join_Trace, 5, "get value from " << _src_tabno );
        }
        else
        {
            _src_tabno = _joinpred.jo_recs[ _src_side ].jop_tableno;
            SAPDBTRACE_WRITELN( Join_Trace, 6, "_src_tabno: " << _src_tabno );
        }
        SAPDBERR_ASSERT_STATE( cak68_join_value != _src_tabno );
            
        SAPDB_MemCopyNoCheck( 
                (SAPDB_Byte*)(&key.keyVal_gg00()) + 
                key.keyLen_gg00(), 
                ((SAPDB_Byte*)(*(records+(_src_tabno-1)))) + 
                _joinpred.jo_recs[ _src_side ].jop_outpos - 1, 
                _joinpred.jo_recs[ _src_side ].jop_inoutlen );
        SAPDBTRACE_WRITELN( Join_Trace, 7, 
                SAPDBTrace_HexBuf((SAPDB_Byte*)(&key.keyVal_gg00()) + key.keyLen_gg00(), _joinpred.jo_recs[ _src_side ].jop_inoutlen ));

        SAPDB_Char _defbyte = key.keyVal_gg00()[ (key.keyLen_gg00()+1) - 1 ];
        SAPDBTRACE_WRITELN( Join_Trace, 7, "col defbyte: " << SAPDBTrace_Hex(&_defbyte, 1) );

        if  ( _i < m_JoinInfo.jn_jarr_path && 
              _joinpred.jo_recs[ _src_side ].jop_inoutlen < 
              _joinpred.jo_recs[ (_joinpred.jo_partno-1) ].jop_inoutlen  )
        {
            if ( csp_unicode_def_byte == _defbyte || (0xFF - csp_unicode_def_byte) == _defbyte )
            {
#ifdef SAPDB_SLOW
                if ( csp_unicode_def_byte == _defbyte )
                {
                    SAPDBTRACE_WRITELN( Join_Trace, 7, "fill up value with unicode blank" );
                
                } else {
                    SAPDBTRACE_WRITELN( Join_Trace, 7, "fill up value with unicode blank desc" );
                }
#endif
                g20unifill( sizeof(key.keyVal_gg00()), 
                        &key.keyVal_gg00(), 
                        key.keyLen_gg00() + _joinpred.jo_recs[ _src_side ].jop_inoutlen + 1,
                        _joinpred.jo_recs[ (_joinpred.jo_partno-1) ].jop_inoutlen - 
                        _joinpred.jo_recs[ _src_side ].jop_inoutlen,
                        (csp_unicode_def_byte == _defbyte) ? csp_unicode_blank : csp_unicode_blank_desc );
            }
            else
            {
                SAPDBTRACE_WRITELN( Join_Trace, 7, "fill up value (" << SAPDBTrace_Hex(&_defbyte, 1) << ")");
                SAPDB_MemFillNoCheck( ((SAPDB_Byte*)&key.keyVal_gg00()) + 
                        key.keyLen_gg00() +
                        _joinpred.jo_recs[ _src_side ].jop_inoutlen, 
                        _defbyte, 
                        _joinpred.jo_recs[ (_joinpred.jo_partno-1) ].jop_inoutlen - 
                        _joinpred.jo_recs[ _src_side ].jop_inoutlen );
            }
            SAPDBTRACE_WRITELN( Join_Trace, 7, 
                    SAPDBTrace_HexBuf((SAPDB_Byte*)(&key.keyVal_gg00()) + key.keyLen_gg00(), _joinpred.jo_recs[ (_joinpred.jo_partno-1) ].jop_inoutlen ));
            last_field_len = _joinpred.jo_recs[ (_joinpred.jo_partno-1) ].jop_inoutlen;
            key.keyLen_gg00() += 
                _joinpred.jo_recs[ (_joinpred.jo_partno-1) ].jop_inoutlen;
        }
        else
        {

            if ( _joinpred.jo_recs[ _src_side ].jop_inoutlen > 
                 _joinpred.jo_recs[ (_joinpred.jo_partno-1) ].jop_inoutlen  )
            {
                last_field_len = _joinpred.jo_recs[ (_joinpred.jo_partno-1) ].jop_inoutlen;
                SAPDB_Int4 _fieldlen = s30lnr_defbyte( &key.keyVal_gg00(), 
                        _defbyte, key.keyLen_gg00()+1, _joinpred.jo_recs[ _src_side ].jop_inoutlen );
                SAPDBTRACE_WRITELN( Join_Trace, 7, "src_def > dest_def ( src_real_len = " << _fieldlen << ", defbyte: " << 
                        SAPDBTrace_Hex( &_defbyte, 1) << " )");
                key.keyLen_gg00() += 
                    _joinpred.jo_recs[ (_joinpred.jo_partno-1) ].jop_inoutlen;

                if ( _fieldlen > _joinpred.jo_recs[ (_joinpred.jo_partno-1) ].jop_inoutlen )
                {
                    SAPDBTRACE_WRITELN( Join_Trace, 6, "too long key part!" );
                    _b_err = e_no_next_record;
                    break;
                }
            }
            else
            {
                last_field_len = _joinpred.jo_recs[ _src_side ].jop_inoutlen;
                key.keyLen_gg00() += 
                    _joinpred.jo_recs[ _src_side ].jop_inoutlen;
            }
        }
    }

    return _b_err;
}

/*************************************************************************/
/*!
 * @param[in,out] records vector of records involved in join
 * @param[out] b_err error code [ ... ]
 * @return state if join condition is fulfilled
 */
SAPDB_Bool Join_JoinOperator2::join( 
        tgg00_Rec*       *records, 
        tgg00_BasisError &b_err ) 
{
    SAPDBTRACE_METHOD_DEBUG( "Join_JoinOperator2::join", Join_Trace, 3 );

    SAPDB_UInt2 _joincnt = 1;
    SAPDB_Bool _fullfilled = true;
    tak_one_join _joinpred;

    while ( _joincnt <= m_JoinInfo.jn_jarr_cnt && _fullfilled && e_ok == b_err )
    {
        SAPDBTRACE_IF( Join_Trace, 3, 
        a683_one_join_entry( td_always, m_d_joins, m_JoinInfo.jn_jarr[ _joincnt - 1 ], false ));
        SAPDB_Bool _reverse_operands = false;
        _joinpred = 
            (*m_d_joins.jrc_joinarr)[ m_JoinInfo.jn_jarr[ _joincnt - 1 ] ];
        tgg00_StackOpType _stackop = _joinpred.jo_op;

        SAPDBTRACE_IF( Join_Trace, 3, 
            t01op( td_always, "_stackop    ", _stackop ));

        if  ( 1 == _joinpred.jo_partno )
        {
            switch ( _stackop )
            {
                case op_lt:
                    _stackop.becomes( op_gt );
                    break;
                case op_le:
                    _stackop.becomes( op_ge );
                    break;
                case op_gt:
                    _stackop.becomes( op_lt );
                    break;
                case op_ge:
                    _stackop.becomes( op_le );
                    break;
                default:
                    /* for op_like, op_not_like, op_sounds, op_not_sounds
                     * _reverse_operands has no effect for op_eq, op_ne   */
                    _reverse_operands = true;
            }
            SAPDBTRACE_IF( Join_Trace, 3, 
                t01op( td_always, "_stackop rev", _stackop ));
        }

        SAPDB_UInt2 _dst_side, _src_side, _src_tabno, _dst_tabno;
        if ( 0 == ( _dst_side = _joinpred.jo_partno-1 ) ) 
            _src_side = 1;
        else
            _src_side = 0;
        SAPDBTRACE_WRITELN( Join_Trace, 5, "_src_side: " << _src_side << " _dst_side: " << _dst_side );
        if ( cak68_join_value == _joinpred.jo_recs[ _src_side ].jop_tableno 
            ||
             cak68_join_value == _joinpred.jo_recs[ _joinpred.jo_partno-1 ].jop_tableno )
        {
            _src_tabno = m_JoinSequence[m_RightTableIdx-1-TAB_OFFS_GG07].jos_source;
            _dst_tabno = m_JoinSequence[m_RightTableIdx-1              ].jos_source;
            SAPDBTRACE_WRITELN( Join_Trace, 5, "get value from " << _src_tabno << " join with " << _dst_tabno );
        }
        else
        {
            _src_tabno = _joinpred.jo_recs[ _src_side ].jop_tableno;
            _dst_tabno = _joinpred.jo_recs[ _dst_side ].jop_tableno;
            SAPDBTRACE_WRITELN( Join_Trace, 5, "_src_tabno: " << _src_tabno << "\t_dst_tabno: " << _dst_tabno );
        }
        SAPDBERR_ASSERT_STATE( cak68_join_value != _src_tabno && cak68_join_value != _dst_tabno );
        
        SAPDBTRACE_WRITELN( Join_Trace, 5, 
                " join tab " << _src_tabno << 
                "(" << _joinpred.jo_recs[ _src_side ].jop_fieldno << ")" << 
                " on pos " << _joinpred.jo_recs[ _src_side ].jop_outpos <<
                " with " << _dst_tabno << 
                "(" << _joinpred.jo_recs[ _dst_side ].jop_fieldno << ")" << 
                " on pos " << _joinpred.jo_recs[ _dst_side ].jop_outpos );
        
        tsp00_LcompResult _lc_result;
        
        // compare columns
        switch ( _stackop )
        {
            case op_like:
            case op_not_like:
                {
                    tgg00_Rec *_like_pattern = 0;
                    SAPDB_Int4 _change_pos, _ix;
                    if ( _reverse_operands )
                    {
                        SAPDBTRACE_WRITELN( Join_Trace, 5, "reverse operands " << "\tleft record changed: " << m_LeftRecChanged );
                        if ( ! m_LeftRecChanged )
                        {
                            m_LeftRecChanged = true;
                            // left record
                            _like_pattern = *(records+_src_tabno-1);
                            _change_pos   = _joinpred.jo_recs[ _src_side ].jop_outpos;
                            _ix           = 0;
                        }
                    }
                    else
                    {
                        // right record
                        _like_pattern = *(records+_dst_tabno-1);
                        _change_pos   = _joinpred.jo_recs[ _dst_side ].jop_outpos;
                        _ix           = 1;
                    }
                    pasbool _ok;
                    if ( _like_pattern )
                    {
                        if ( csp_unicode_def_byte == _like_pattern->recBuf_gg00()[ _change_pos - 1 ] )
                        {
                            SAPDBTRACE_WRITELN( Join_Trace, 5, "uni build pattern" );
                            s49uni_build_pattern(
                                  _like_pattern, _change_pos + 1,
                                  _change_pos + 
                                  _joinpred.jo_recs[ _ix ].jop_inoutlen,
                                  (char *)csp_unicode_blank, false,
                                  sqlm_ansi, _ok );
                        }
                        else
                        {
                            SAPDBTRACE_WRITELN( Join_Trace, 5, "build pattern" );
                            s49build_pattern(
                                  _like_pattern,
                                  _change_pos + 1, _change_pos + 
                                  _joinpred.jo_recs[ _ix ].jop_inoutlen,
                                  bsp_c1, false, false,
                                  sqlm_ansi, _ok );
                        }
                        if ( ! _ok )
                        {
                            if ( sqlm_ansi == m_acv.GetMessBlock().SqlMode() )
                                m_acv.TransContext().trError_gg00 = e_illegal_escape_sequence;
                            else
                                m_acv.TransContext().trError_gg00 = e_invalid_pattern;
                        }
                    }
                }
                // fall through
            case op_sounds:
            case op_not_sounds:
                {
                    SAPDB_Int4 _compare_ok = 0;
                    tkb07_buffer_description _workbuf;

                    _workbuf.buffer_addr = m_acv.a_work_buf_addr;
                    _workbuf.buffer_size = m_acv.a_work_buf_size;
                    _workbuf.buffer_len  = 0;
                    if ( _reverse_operands )
                    {
                        k71join_comparison(m_SelFields, _stackop, _workbuf,
                              /* right record + pos */
                              *(records+_dst_tabno-1),
                              _joinpred.jo_recs[ _dst_side ].jop_outpos,
                              _joinpred.jo_recs[ _dst_side ].jop_inoutlen,
                              /* left record + pos */
                              *(records+_src_tabno-1),
                              _joinpred.jo_recs[ _src_side ].jop_outpos,
                              _joinpred.jo_recs[ _src_side ].jop_inoutlen,
                              _compare_ok );
                    }
                    else
                    {
                        k71join_comparison(m_SelFields, _stackop, _workbuf,
                              /* left record + pos */
                              *(records+_src_tabno-1),
                              _joinpred.jo_recs[ _src_side ].jop_outpos,
                              _joinpred.jo_recs[ _src_side ].jop_inoutlen,
                              /* right record + pos */
                              *(records+_dst_tabno-1),
                              _joinpred.jo_recs[ _dst_side ].jop_outpos,
                              _joinpred.jo_recs[ _dst_side ].jop_inoutlen,
                              _compare_ok );
                    }
                    _fullfilled = ( cgg04_is_true == _compare_ok );
                }
                break;
            default:
                s30luc( 
                        /* left record + pos */
                        *(records+_src_tabno-1),
                        _joinpred.jo_recs[ _src_side ].jop_outpos,
                        _joinpred.jo_recs[ _src_side ].jop_inoutlen,
                        /* right record + pos */
                        *(records+_dst_tabno-1),
                        _joinpred.jo_recs[ _dst_side ].jop_outpos,
                        _joinpred.jo_recs[ _dst_side ].jop_inoutlen,
                        _lc_result );
                SAPDBTRACE_IF( Join_Trace, 3, 
                    t01comp_result( td_always, "comp result ", _lc_result ));
                switch( _stackop )
                {
                    case op_eq:
                        if ( l_equal == _lc_result )
                            _fullfilled = true;
                        else
                            _fullfilled = false;
                        break;
                    case op_le:
                        if ( l_equal == _lc_result || l_less == _lc_result )
                            _fullfilled = true;
                        else
                            _fullfilled = false;
                        break;
                    case op_lt:
                        if ( l_less == _lc_result )
                            _fullfilled = true;
                        else
                            _fullfilled = false;
                        break;
                    case op_ge:
                        if ( l_equal == _lc_result || l_greater ==_lc_result )
                            _fullfilled = true;
                        else
                            _fullfilled = false;
                        break;
                    case op_gt:
                        if ( l_greater == _lc_result )
                            _fullfilled = true;
                        else
                            _fullfilled = false;
                        break;
                    case op_ne :
                        if ( l_equal != _lc_result && l_undef != _lc_result ) 
                            _fullfilled = true;
                        else
                            _fullfilled = false;
                        break;
                    default:
                        _fullfilled = false;
                        SAPDBTRACE_WRITELN( Join_Trace, 1, "operator " << _stackop << " not implemented: " << __FILE__ << " line " << __LINE__ );
                        b_err = e_not_implemented;
                }
        }
        ++_joincnt;
    }
    SAPDBTRACE_WRITELN( Join_Trace, 3, "is join: " << _fullfilled );
    return _fullfilled;
}

/*************************************************************************/
/*!
 * @param[in] key key to be converted ( descending, codetable )
 */
void Join_JoinOperator2::convert_key( tgg00_Lkey &key )
{
    SAPDBTRACE_METHOD_DEBUG( "Join_JoinOperator2::convert_key", Join_Trace, 3 );
    SAPDBTRACE_WRITELN( Join_Trace, 3, "m_RightRecConvArrCnt " << m_RightRecConvArrCnt );
    SAPDBTRACE_IF( Join_Trace, 5, t01buf( td_always, key.k(), 1, key.len() <= 100 ? key.len() : 100 ));
    
    // take conversions from m_RightRecConvArr and adjust key 
    for ( SAPDB_UInt2 _i = 0; _i < m_RightRecConvArrCnt; ++_i )
    {
        SAPDBTRACE_WRITELN( Join_Trace, 3, "ct_epos: " << m_RightRecConvArr[ _i ].ct_epos << "\tct_elenvar: " << m_RightRecConvArr[ _i ].ct_elenvar << "\tct_codeno: " << m_RightRecConvArr[ _i ].ct_codeno );
        if ( m_RightRecConvArr[ _i ].ct_codeno != 0 )
            s30map( g02codetables.tables[ m_RightRecConvArr[ _i ].ct_codeno - 1 ],
                    key.k(), 
                    m_RightRecConvArr[ _i ].ct_epos - cgg_rec_key_offset,
                    key.k(),
                    m_RightRecConvArr[ _i ].ct_epos - cgg_rec_key_offset,
                    m_RightRecConvArr[ _i ].ct_elenvar );
        if ( m_RightRecConvArr[ _i ].ct_is_desc )
        {
            SAPDBTRACE_WRITELN( Join_Trace, 5, "convert desc" );
            // build revers index key for descending indexes
            for ( SAPDB_Int4 _jx = m_RightRecConvArr[ _i ].ct_epos - cgg_rec_key_offset; 
                  _jx <= m_RightRecConvArr[ _i ].ct_epos + m_RightRecConvArr[ _i ].ct_elenvar - cgg_rec_key_offset - 1; 
                  ++_jx )
            {
                key.k()[ _jx - 1 ] = char(0xFF - key.k()[ _jx - 1 ]);
            }
        } 
    }
    SAPDBTRACE_IF( Join_Trace, 5, t01buf( td_always, key.keyVal_gg00(), 1, key.keyLen_gg00() <= 100 ? key.keyLen_gg00() : 100 ));
}

/*************************************************************************/
/*!
 * @param[out] target output record
 * @param[in] records vector of records involved into join
 */
void Join_JoinOperator2::create_output( 
        const SAPDB_AutoPtr<tgg00_Rec>&  target,
        tgg00_Rec* *records )
{
    SAPDBTRACE_METHOD_DEBUG( "Join_JoinOperator2::create_output", Join_Trace, 3 );
    SAPDBERR_ASSERT_STATE( target.GetPtr() ); 
    SAPDBTRACE_IF( Join_Trace, 5, SAPDB_MemFillNoCheck( target.GetPtr(), ':', m_JoinRecBufferSize ) );

    tgg00_BasisError _b_err = e_ok;

    target->recLen_gg00()    = join_reclen();
    target->recKeyLen_gg00() = join_keylen();
    target->recVarcolOffset_gg00() = 0;
    target->recVarcolCnt_gg00()    = 0;
    
    for ( tsp00_Int2 _i = 0; _i < m_OutputInfoSize; ++_i )
    {
        SAPDBTRACE_WRITELN( Join_Trace, 3, 
                (m_OutputInfo.GetPtr()+_i)->oc_srctab << " : "  << 
                (m_OutputInfo.GetPtr()+_i)->oc_srcpos << " -> " <<
                (m_OutputInfo.GetPtr()+_i)->oc_dstpos << " ( "  <<
                (m_OutputInfo.GetPtr()+_i)->oc_len    << " )" );
        SAPDB_MemCopyNoCheck( 
                (SAPDB_Byte*)&(target->recBuf_gg00()) + (m_OutputInfo.GetPtr()+_i)->oc_dstpos - 1, 
                (SAPDB_Byte*)&(*(records + (m_OutputInfo.GetPtr()+_i)->oc_srctab - 1))->recBuf_gg00() +
                (m_OutputInfo.GetPtr()+_i)->oc_srcpos - 1, 
                (m_OutputInfo.GetPtr()+_i)->oc_len );
        //SAPDBTRACE_IF( Join_Trace, 7, 
        //t01buf( td_always, target.GetPtr(), 1, target->recLen_gg00() <= 300 ? target->recLen_gg00() : 300 ));
    }
    
#ifdef SAPDB_SLOW
    SAPDB_MemFillNoCheck( (SAPDB_Byte*)&target->recKey_gg00().k() + target->recKeyLen_gg00() - RESCNT_MXGG04, 0x99, RESCNT_MXGG04 );
#endif
}

/*************************************************************************/
/*!
 * @param[in,out] recptr record to be checked and changed (output filter)
 * @param[out]  b_err error code [ ... ]
 * @return true if records fulfilled filter condition
 */
SAPDB_Bool Join_JoinOperator2::filter( 
        const SAPDB_AutoPtr<tgg00_Rec>  &recptr, 
        tgg00_BasisError                &b_err ) 
{
    // filter and transform (i.e. functions) record
    SAPDBTRACE_METHOD_DEBUG( "Join_JoinOperator2::filter", Join_Trace, 3 );
    tgg00_StackEntry *_err_st_entry;
    pasbool _unqualified = false;

    m_SelFields.sfp_m_result_addr() = reinterpret_cast<tsp00_MoveObjPtr>(recptr.GetPtr());
    m_SelFields.sfp_rec_addr()      = reinterpret_cast<tsp00_BufAddr>(recptr.GetPtr());
    m_SelFields.sfp_rec_len()       = recptr->recLen_gg00();
    m_SelFields.sfp_rec_key_len()   = recptr->recKeyLen_gg00();
    m_SelFields.sfp_result_length() = 0; 
    m_SelFields.sfp_m_result_len()  = 0;
    m_SelFields.sfp_m_result_cnt()  = 1;
    
    SAPDBTRACE_WRITELN( Join_Trace, 7, "before filter: " << m_OutputFilter );
    SAPDBTRACE_IF( Join_Trace, 7, 
    t01buf( td_always, recptr.GetPtr(), 1, recptr->recLen_gg00() <= 300 ? recptr->recLen_gg00() : 300 ));
    if ( m_OutputFilter )
    {
        tsp00_Buf _resbuf;
        SAPDBTRACE_IF( Join_Trace, 5, SAPDB_MemFillNoCheck( &_resbuf, ':', sizeof(_resbuf) ));
        m_SelFields.sfp_m_result_size() = sizeof(_resbuf);
        m_SelFields.sfp_m_result_addr() = reinterpret_cast<tsp00_MoveObjPtr>(&_resbuf);
        SAPDB_Int4 _old_result_len = m_SelFields.sfp_result_length();
        m_acv.TransContext().trError_gg00 = e_ok;
        
        k71qual_handling( m_acv.TransContext(), m_SelFields, 
                m_FilterDesc.mview_cnt > 0, false, m_FilterDesc,
                _err_st_entry, _unqualified );
        // write result length
        recptr->recLen_gg00() = GetRecordLength();
        recptr->recKeyLen_gg00() = GetKeyLength();

        if ( _unqualified )
            m_SelFields.sfp_result_length() = _old_result_len;
        else
        {
            if ( -1 == (*m_Filter)[ m_FilterDesc.mqual_pos - 1 ].ecol_pos() )
            {
                SAPDBTRACE_WRITELN( Join_Trace, 7, "transfer constant expression" );
                /* we don't have complete output qualification of record */
                /* ak684transfer_const_expressions() */
                /* ak684out_filter_stack()           */
                for ( SAPDB_Int4 _ix = m_FilterDesc.mqual_pos + 1; 
                        _ix <= (*m_Filter)[ m_FilterDesc.mqual_pos - 1 ].epos() + 
                        m_FilterDesc.mqual_pos - 2; ++_ix )
                {
                    if ( st_output == (*m_Filter)[ _ix - 1 ].etype() )
                    {
                        SAPDBTRACE_WRITELN( Join_Trace, 5, "pos: " << (*m_Filter)[ _ix - 1 ].epos() << " len: " << (*m_Filter)[ _ix - 1 ].elen_var());
                        SAPDB_MemCopyNoCheck( 
                                (SAPDB_Byte*)recptr.GetPtr() + (*m_Filter)[ _ix - 1 ].epos() - 1, 
                                (SAPDB_Byte*)&_resbuf + (*m_Filter)[ _ix - 1 ].epos() - 1,
                                (*m_Filter)[ _ix - 1 ].elen_var() 
                                );
                    }
                }
            } else
            {
                SAPDB_MemCopyNoCheck( (SAPDB_Byte*)recptr.GetPtr() + cgg_rec_key_offset,
                        (SAPDB_Byte*)&_resbuf + cgg_rec_key_offset,
                        recptr->recLen_gg00() );
            }
            // write defined result counter
            SAPDBTRACE_IF( Join_Trace, 3, SAPDB_MemFillNoCheck( (SAPDB_Byte*)&recptr->recKey_gg00().k() + recptr->recKeyLen_gg00() - RESCNT_MXGG04, 0x99, RESCNT_MXGG04 ) );
        }
    } else
    {
        m_SelFields.sfp_m_result_size() = m_JoinRecBufferSize;
        m_SelFields.sfp_m_result_addr() = reinterpret_cast<tsp00_MoveObjPtr>(recptr.GetPtr());

        k71qual_handling( m_acv.TransContext(), m_SelFields, 
                m_FilterDesc.mview_cnt > 0, false, m_FilterDesc,
                _err_st_entry, _unqualified );
        // write result length
        recptr->recLen_gg00() = GetRecordLength();
        recptr->recKeyLen_gg00() = GetKeyLength();
    }
    SAPDBTRACE_WRITELN( Join_Trace, 3, "k71qual_handling: " << SAPDBTrace::BasisError(m_acv.TransContext().trError_gg00) );
    
#ifdef SAPDB_SLOW
    if ( e_ok == m_acv.TransContext().trError_gg00 )
    {
        SAPDBTRACE_WRITELN( Join_Trace, 7, "after filter: " );
        SAPDBTRACE_IF( Join_Trace, 7, 
        t01buf( td_always, recptr.GetPtr(), 1, recptr->recLen_gg00() <= 300 ? recptr->recLen_gg00() : 300 ));
    }
#endif
    
    switch ( m_acv.TransContext().trError_gg00 ) 
    {
        case e_ok:
            return true; break;
        case e_qual_violation:
            m_acv.TransContext().trError_gg00 = e_ok; 
            return false; break;
        default:
            b_err = m_acv.TransContext().trError_gg00;
            return false;
    }
}

/*************************************************************************/
/*!
* @return error code [ ... ]
*/
tgg00_BasisError Join_JoinOperator2::reset() 
{
    if ( m_JoinInfo.jn_rightoj )
        // create auxiliary RIGHT OUTER JOIN file
        b01tcreate_file( m_acv.TransContext(), m_AuxRightOuterJoinFile );
    m_TableForRightOuterJoinScanStat       = never_scanned;
    m_AuxRightOuterJoinFilePos.tpsPno_gg00 = NIL_PAGE_NO_GG00;
    m_AppendRightOuterJoinRecords = false;
    m_AuxRightOuterJoinKey.len()  = 0;
    return m_acv.TransContext().trError_gg00;
}

/*************************************************************************/
/*!
 * @return length or "right outer key"
 */
SAPDB_UInt4 Join_JoinOperator2::get_AuxRightOuterKeyLen()
{
    SAPDBTRACE_METHOD_DEBUG( "Join_JoinOperator2::get_AuxRightOuterKeyLen", Join_Trace, 3 );
    SAPDBERR_ASSERT_STATE( m_acv.GetMessBlock().HasOutputCols() );

    SAPDB_Int4 _stop = m_acv.GetMessBlock().QualPos();
    SAPDB_Int4 _i = m_acv.GetMessBlock().StackEntry(m_acv.GetMessBlock().QualPos()-1).epos() + 
        m_acv.GetMessBlock().QualPos() - 1;
    
    // loop over output columns, look for output genertated by ak684key_columns_stack()
    while ( _i > _stop )
    {
        --_i;
        if ( ( st_varkey == m_acv.GetMessBlock().StackEntry(_i-1).etype() || 
               st_fixkey == m_acv.GetMessBlock().StackEntry(_i-1).etype()) &&
             st_output == m_acv.GetMessBlock().StackEntry((_i-1)+1).etype() &&
             op_o_output_outer_join == m_acv.GetMessBlock().StackEntry((_i-1)+1).eop_out() )
        {
            SAPDBTRACE_WRITELN( Join_Trace, 5, "VAR KEY at pos " << _i << "\tkey len " << m_acv.GetMessBlock().StackEntry(_i-1).ecol_pos() );
            return m_acv.GetMessBlock().StackEntry(_i-1).ecol_pos();
        }
    }
    m_acv.ThrowError( e_key_does_not_exist );
    SAPDBERR_ASSERT_STATE( false );
    return 0;
}

/*************************************************************************/
/*!
 * @param[in] acv context
 * @param[in] buffersize size of buffer for operator
 * @param[in] expectedRecordCount amount of records expected
 * @param[in] maxServerTasks amount of possible server tasks
 * @param[in] tabno number of table 
 * @return pointer to created base table access operator
 */
IOperator* Join_JoinOperator2::AccessOperatorFactory(
    SQLMan_Context          &acv,
    const SAPDB_UInt4       &buffersize,
    const SAPDB_UInt4       &expectedRecordCount,
    const SAPDB_Int4		&maxServerTasks,
    const SAPDB_UInt2       tabno)
{
    SAPDBTRACE_METHOD_DEBUG( "Join_JoinOperator2::AccessOperatorFactory", Join_Trace, 3 );

    Join_AccessDesc _accessDesc( acv );
    Join_IAccessOperator*     _accessOp     = 0;
    Join_HashAccessOperator*  _hashAccessOp = 0;
    SAPDB_Byte*               _recordBuffer = 0;
    SAPDB_UInt                _recBufSize   = buffersize;

    // important!!! proof for right creation of _accessDesc !!!
    if ( ! m_acv.IsOk() ) return 0;

    SAPDBTRACE_WRITELN( Join_Trace, 3, "access strategy : " << _accessDesc.Strategy()->str_strategy );

    if ( ! ( _accessDesc.IsKnownAccess() || _accessDesc.IsJoinAccess() ) )
    {
        m_acv.ThrowError( e_unknown_strategy );
        return 0;
    }

    if ( strat_inv_range_merge_fetch == _accessDesc.Strategy()->str_strategy )
    {
        m_acv.ThrowError( e_not_implemented );
        return 0;
    }

    if  ( ! _accessDesc.Strategy()->str_res_possible )
    {
        SAPDBTRACE_WRITELN( Join_Trace, 3, "create Join_NopAccessOperator" );
        _accessOp = new (m_acv.GetAllocator())
            Join_NopAccessOperator(
                acv,
                tabno,
                !m_JoinInfo.jn_rightoj,
                _recordBuffer,
                _recBufSize );
    } else if ( strat_more_than_one == _accessDesc.Strategy()->str_strategy || 
                strat_catalog       == _accessDesc.Strategy()->str_strategy ) 
    {

        SAPDBTRACE_WRITELN( Join_Trace, 3, "create Join_LegacyOperator" );
        _accessOp = new (m_acv.GetAllocator())
            Join_LegacyAccessOperator(
                acv,
                tabno,
                _recordBuffer,
                _recBufSize );

    } else if  ( _accessDesc.Strategy()->str_access_mod.includes( strmod_sorted ) )
    {
        SAPDBTRACE_WRITELN( Join_Trace, 3, "create Join_SortOperator" );
        if  ( strat_more_than_one == _accessDesc.OneTableStrategy()->str_strategy ||
              strat_catalog       == _accessDesc.OneTableStrategy()->str_strategy )
            _accessOp = new (m_acv.GetAllocator())
                Join_LegacyAccessOperator(
                    acv,
                    tabno,
                    _recordBuffer,
                    _recBufSize );
        else
            _accessOp = new (m_acv.GetAllocator())
                Join_SortOperator(
                    acv,
                    tabno,
                    _recordBuffer,
                    _recBufSize );
    } else 
    {
        if ( _accessDesc.UseHashJoin() && g01use_join_hashtable() ) 
        {
            SAPDBTRACE_WRITELN( Join_Trace, 3, "create Join_HashOperator" );
            // create hash access operator
            SAPDB_UInt4 _expectedTableSize =
                static_cast<SAPDB_UInt4>(
                    expectedRecordCount * _accessDesc.GetRecordLength() * 1.1 );
            if ( Join_Trace.TracesLevel( 1 ) )
            {
                SAPDBTrace_Stream traceStream( &Join_Trace.GetTracePrinter() );
                traceStream
                    << "Join_JoinOperator - hash access:" << NewLine
                    << "  Expected record count: " << expectedRecordCount << NewLine
                    << "  Record length        : "
                    << _accessDesc.GetRecordLength() << NewLine
                    << "  Expected table size  : " << _expectedTableSize << NewLine;
                }
            if ( _expectedTableSize < BUF8K_MXSP00 ) 
            {
                SAPDBTRACE_WRITELN(
                    Join_Trace, 1,
                    "Corrected expected table size from "
                    << _expectedTableSize << " to "
                    << BUF8K_MXSP00 << " bytes" );
                _expectedTableSize = BUF8K_MXSP00;
            }

            SAPDB_Int2 _joinTransitionKeyLength = 0;
            for ( int _i = 1; _i <= m_JoinInfo.jn_jarr_path; ++_i )
            {
                _joinTransitionKeyLength += 
                    (*m_d_joins.jrc_joinarr)[ m_JoinInfo.jn_jarr[ _i-1 ] ].jo_recs[ (*m_d_joins.jrc_joinarr)[ m_JoinInfo.jn_jarr[ _i-1 ] ].jo_partno-1 ].jop_inoutlen;
            }
                    
            SAPDB_Int2 _lastKeyColStartPos = m_LastRightDefBytePos
                - cgg_rec_key_offset - 1;
            Join_HashAccessOperator::UniqueKeys _uniqueKeys =
                (_accessDesc.Strategy()->str_strategy == strat_join_key_equal)
                || (_accessDesc.Strategy()->str_strategy == strat_join_all_keys_equal)
                || (_accessDesc.Strategy()->str_strategy == strat_join_inv)
                || (_accessDesc.Strategy()->str_strategy == strat_join_all_inv_equal)
                ? Join_HashAccessOperator::NoDuplicateKeys
                : Join_HashAccessOperator::AllowDuplicateKeys;

            Join_HashAccessOperator::KeySizes _keySizes = 
                (_accessDesc.Strategy()->str_strategy == strat_join_key_next)
                || (_accessDesc.Strategy()->str_strategy == strat_join_key_range)
                ? Join_HashAccessOperator::FixSizedKeys
                : Join_HashAccessOperator::VarSizedKeys;

            _hashAccessOp =
                new (m_acv.GetAllocator()) Join_HashAccessOperator(
                    acv,
                    tabno,
                    _uniqueKeys,
                    _keySizes,
                    _expectedTableSize,
                    _joinTransitionKeyLength,
                    _lastKeyColStartPos);
            if ( _hashAccessOp ) 
            {
                // make access operator use hash record buffer and
                // size if it could be allocated
                _recordBuffer = _hashAccessOp->GetRecordBuffer();
                if ( 0 != _recordBuffer ) {
                    _recBufSize = _expectedTableSize;
                }
            }
        }

        if ( _accessDesc.IsIndexAccess() || _accessDesc.IsJoinInvAccess() ) 
        {
            bd998IncrementIndexUsageCount(m_acv.TransContext(),
            _accessDesc.FileIDs().inv_id.fileTabId_gg00());
                
            if ( _accessDesc.UseParallelIndexAccess() && 
                 maxServerTasks > 0 )
            {
                SAPDBTRACE_WRITELN( Join_Trace, 3, "create Join_InvAccessOperatorEx" );
                _accessOp = new (m_acv.GetAllocator())
                    Join_InvAccessOperatorEx(
                        acv,
                        tabno,
                        !m_JoinInfo.jn_rightoj,
                        _recordBuffer,
                        _recBufSize,
                        maxServerTasks);
            } else 
            {
                SAPDBTRACE_WRITELN( Join_Trace, 3, "create Join_InvAccessOperator" );
                _accessOp = new (m_acv.GetAllocator())
                    Join_InvAccessOperator(
                        acv,
                        tabno,
                        !m_JoinInfo.jn_rightoj,
                        _recordBuffer,
                        _recBufSize );
            }
                
        } else 
        {
            SAPDBTRACE_WRITELN( Join_Trace, 3, "create Join_TableAccessOperator" );
            _accessOp = new (m_acv.GetAllocator())
                Join_TableAccessOperator(
                    acv,
                    tabno,
                    !m_JoinInfo.jn_rightoj,
                    _recordBuffer,
                    _recBufSize );
        }
    }

    if ( _accessOp 
         && (( strat_join_key_equal
               == _accessDesc.Strategy()->str_strategy)
             ||
             ( strat_join_all_keys_equal
               == _accessDesc.Strategy()->str_strategy)
             ||
             ( strat_join_viewkey
               == _accessDesc.Strategy()->str_strategy)) ) {
        _accessOp->SetMaximumRowsReadAtATime( 1 );
    }

    if ( _hashAccessOp && _accessOp ) 
    {
        // connect hash access operator with base access operator
        _hashAccessOp->SetAccessOperator( _accessOp );
        _accessOp = _hashAccessOp;
    }

    if ( ! _accessOp ) 
    {
        destroy( _hashAccessOp,  m_acv.GetAllocator() );
        m_acv.TransContext().trError_gg00 = e_no_more_memory;
        if ( Join_Trace.TracesLevel( 1 ) )
        {
            SAPDBTrace_Stream traceStream( &Join_Trace.GetTracePrinter() );
            traceStream
                << "Join_JoinOperator2:: access operator creation failed: no more memory";
        }
    }

    return _accessOp;
}
