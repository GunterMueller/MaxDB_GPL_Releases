/*!
 * @file
 * @brief implements Join_AccessDesc
 *
 * @author GertG
 * @ingroup Join
 *
 * @sa Join_AccessDesc.hpp
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
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "SAPDBCommon/Messages/SDBMsg_Join.h"
#include "SQLManager/SQLMan_Context.hpp"
#include "SQLManager/SQLMan_MessBlock.hpp"
#include "Join/Join_AccessDesc.hpp"
#include "Join/Join_Trace.hpp"
#include "gsp00.h"
#include "gsp03_1.h"
#include "hak06.h"
#include "hta01.h"
#include "hta01_3.h"
#include "ggg07.h"
#include "hgg04.h"
#include "hgg04_1.h"
#include "hbd01.h"
#include "hbd01_1.h"
#include "hkb720.h"
#include "hgg01_1.h"
#include "hak725.h"
#include "hbd998.h"



/* ******************** PUBLIC MEMBERS ********************* */
/*!
 * @param[in] acv global context
 *
 */
Join_AccessDesc::Join_AccessDesc(SQLMan_Context& acv):
m_acv( acv ), 
m_mblock( acv.a_mblock ), 
m_strategy(0), 
m_onetab_strategy(0), 
m_segment_ptr1(0),
m_segment_ptr2( acv.GetAllocator() )
{
    SAPDBTRACE_METHOD_DEBUG( "Join_AccessDesc::ctor", Join_Trace, 1 );
    
    tgg00_BasisError _b_err = e_ok;
    
    // initialize messblock
    _b_err = init_mblock();
    
    // initialize strategy
    if ( e_ok == _b_err )
    {
        if ( m_mblock.mb_qual()->mstrat_pos() > 0 && 
             m_mblock.mb_qual()->mstrat_cnt() > 0 )
        {
            SAPDBTRACE_WRITELN( Join_Trace, 7, "mstrat_pos : " << m_mblock.mb_qual()->mstrat_pos() );
            m_strategy = reinterpret_cast<tgg07_StrategyInfo*>(m_mblock.mb_strat() + 
                         (*m_mblock.mb_st())[m_mblock.mb_qual()->mstrat_pos()-1].epos() - 1);
            if ( 4 == m_mblock.mb_qual()->mstrat_cnt() )
            {
                m_onetab_strategy = reinterpret_cast<tgg07_StrategyInfo*>(m_mblock.mb_strat() + (*m_mblock.mb_st())[m_mblock.mb_qual()->mstrat_pos()+cgg07_OneTabStrat_Offset-1].epos() - 1);
            } else
                m_onetab_strategy = m_strategy;
            SAPDBTRACE_IF( Join_Trace, 5, 
                    a725output_gg_strat( td_always, "join strat  ", *m_strategy ));
#ifdef SAPDB_SLOW
            if ( m_onetab_strategy != m_strategy )
            {
                SAPDBTRACE_IF( Join_Trace, 5, 
                        a725output_gg_strat( td_always, "onetab strat", *m_onetab_strategy ));
            }
#endif
            if ( m_mblock.mb_qual()->mstack_state().includes( ssSubquery_egg00 ) )
            {
                SAPDB_AutoPtr<tgg00_Rec> _rec( m_acv.GetAllocator(), 
                        reinterpret_cast<tgg00_Rec*>( m_acv.GetAllocator().Allocate( BUF_MXSP00 ) ));
                if ( _rec.IsAssigned() ) 
                {
                    k720_test_subquery( m_acv.TransContext(), 
                            *m_mblock.mb_data(), m_mblock.mb_data_size(),
                            m_mblock.mb_qual()->mstack_desc(), *_rec );
                }
                else
                    _b_err = e_no_more_memory;
            }
            if ( strat_key_in == m_strategy->str_strategy || 
                 strat_inv_in == m_strategy->str_strategy )
            {
                m_segment_ptr1 = 1;
            } else if ( strat_key_subq == m_strategy->str_strategy || 
                        strat_inv_subq == m_strategy->str_strategy ) 
            {
                if ( m_segment_ptr2.SetPtr( reinterpret_cast<tgg04_subq_context*>( m_acv.GetAllocator().Allocate( sizeof(tgg04_subq_context) ))))
                {
                    if ( strat_key_subq == m_strategy->str_strategy )
                    {
                        g04init_subq_context( m_mblock, m_strategy->str_key_in_range().skir_keystart, *m_segment_ptr2, 
                                    m_strategy->str_key_in_range().skir_IN_SUBQ_stpos );
                    } 
                    else
                    {
                        g04init_subq_context( m_mblock, m_strategy->str_inv_in_range().siir_invstart, *m_segment_ptr2, 
                                    m_strategy->str_inv_in_range().siir_IN_SUBQ_stpos );
                    }
                    if ( e_no_next_record == m_segment_ptr2->sc_read_state )
                    {
                        SAPDBTRACE_WRITELN( Join_Trace, 3, "empty subquery file!" );
                        m_strategy->str_res_possible = false;
                    }
                }
                else
                    _b_err = e_no_more_memory;
            }
        } 
        else
        {
            _b_err = e_unknown_strategy;
            m_acv.GetErrorList() + 
                Msg_List( Msg_List::Error, SDBMSG_JOIN_MISSING_STRATEGY_INFORMATION );
        }
    }

    // initialize file ids
    if ( e_ok == _b_err )
    {
        if ( a101_IsExtendedTempFile( acv, m_mblock.mb_qual()->mtree() )) 
        {
            a101_GetTempFileInstance(
                m_acv.TransContext(),
                m_mblock.mb_qual()->mtree() );
            _b_err = m_acv.TransContext().trError_gg00;
        }

        m_fileids.file_id = m_mblock.mb_qual()->mtree();
        m_fileids.file_id.fileLeafNodes_gg00() = cgg_nil_leafnodes;

        if ( IsIndexAccess() ) 
        {
            g04index_tree_build( m_fileids.file_id, m_fileids.inv_id, 
                    m_strategy->str_inv_in_range().siir_indexno );
            m_fileids.inv_id.fileRoot_gg00() = 
                m_strategy->str_inv_in_range().siir_invroot;
        }
        else if ( IsJoinInvAccess() )
        {
            g04index_tree_build( m_fileids.file_id, m_fileids.inv_id, 
                    m_strategy->str_join_multfields().sjmf_index_no );
            m_fileids.inv_id.fileRoot_gg00() = 
                m_strategy->str_join_multfields().sjmf_invroot;
        }
        else
            m_fileids.inv_id = b01niltree_id;
    }

    if ( e_ok != _b_err ) m_acv.ThrowError( _b_err );
}

/*!
 *
 */
Join_AccessDesc::~Join_AccessDesc()
{
    SAPDBTRACE_METHOD_DEBUG( "Join_AccessDesc::dtor", Join_Trace, 1 );

    SAPDBTRACE_WRITELN( Join_Trace, 8, "deallocate mb_qual() @ " << (void*)m_mblock.mb_qual() );
    if (m_mblock.mb_qual()) m_acv.GetAllocator().Deallocate(m_mblock.mb_qual());
    SAPDBTRACE_WRITELN( Join_Trace, 8, "deallocate mb_st() @ " << (void*)m_mblock.mb_st() );
    if (m_mblock.mb_st()) m_acv.GetAllocator().Deallocate(m_mblock.mb_st());
    SAPDBTRACE_WRITELN( Join_Trace, 8, "deallocate mb_strat() @ " << (void*)m_mblock.mb_strat() );
    if (m_mblock.mb_strat()) m_acv.GetAllocator().Deallocate(m_mblock.mb_strat());
    a06drop_fieldlist_references (m_mblock.mb_fieldlists());

}
/*!
 *
 */
SAPDB_Bool Join_AccessDesc::IsVirtualFile() const 
{ 
    return m_strategy->str_all_files && 
        ( bd998IsExtendedTempFile(
        m_acv.TransContext(),
        m_fileids.file_id ) ) &&
        ( ttfnRecursive_egg00 == bd998GetExtendedTempFileType(
        m_acv.TransContext(),
        m_fileids.file_id) ) &&
        ( a101_GetTempFileLevel(
        m_acv, m_fileids.file_id ) > 1 );
}

/*! 
 * @param[in] startkey record stream is restricted to records with key greater/equal than startkey
 * @param[in] stopkey record stream is restricted to records with key lower/equal than stopkey
 */
tgg00_BasisError Join_AccessDesc::CreateKeys(
        Join_TwoKeys& startkey,
        Join_TwoKeys& stopkey ) 
{
    SAPDBTRACE_METHOD_DEBUG( "Join_AccessDesc::CreateKeys", Join_Trace, 1 );
    tgg00_BasisError _b_err = e_ok;
    pasbool _use_stopkeys = true; // check for fullkey

    if (  m_strategy )
    {
        // create primary keys 
        if  ( this->IsKeyAccess() )
        {
            startkey.listkey.len() = stopkey.listkey.len() = 0;

            if ( strat_viewkey == m_strategy->str_strategy )
            {
                startkey.reckey.len() = m_mblock.mb_data()->mbp_key().keyLen_gg00();
                SAPDB_MemCopyNoCheck( startkey.reckey.k(), 
                        m_mblock.mb_data()->mbp_key().keyVal_gg00(), 
                        startkey.reckey.len());
                stopkey.reckey = startkey.reckey;
            } 
            else
            {
                if (( strat_key_in == m_strategy->str_strategy ) )
                {
                    SAPDBERR_ASSERT_STATE( m_segment_ptr1 > 0 );
                    while ( st_dummy == 
                          (*m_mblock.mb_st())[  m_strategy->str_key_in_range().skir_IN_SUBQ_stpos + m_segment_ptr1 - 1 ].etype() )
                        ++m_segment_ptr1;
                    SAPDBTRACE_IF( Join_Trace, 7, 
                            t01stackentry( td_always, (*m_mblock.mb_st())[  m_strategy->str_key_in_range().skir_IN_SUBQ_stpos + m_segment_ptr1 - 1 ], 
                                 m_strategy->str_key_in_range().skir_IN_SUBQ_stpos + m_segment_ptr1 ));
                    if ( st_value == (*m_mblock.mb_st())[  m_strategy->str_key_in_range().skir_IN_SUBQ_stpos + m_segment_ptr1 - 1 ].etype())
                    {
                        SAPDBTRACE_WRITELN(
                            Join_Trace, 3, "switch to next stream segment" );
                        g04limitprimkeys( m_mblock, 
                            m_strategy->str_key_in_range().skir_keystart, 
                            startkey.reckey, 
                            m_strategy->str_key_in_range().skir_keystop,
                            stopkey.reckey, _use_stopkeys, 
                            m_strategy->str_key_in_range().skir_IN_SUBQ_stpos, 
                            m_segment_ptr1++ );
                    }
                    else
                    {
                        m_segment_ptr1 = 0;
                        _b_err = e_no_next_record;
                    }
                }
                else if (( strat_key_subq == m_strategy->str_strategy ) )
                {
                    SAPDBERR_ASSERT_STATE( m_segment_ptr2.IsAssigned() );
                    
                    pasbool    _ok;
                    SAPDB_AutoPtr<tgg00_Rec> _rec( 
                            m_acv.GetAllocator(),
                            reinterpret_cast<tgg00_Rec*>( 
                            m_acv.GetAllocator().Allocate( BUF_MXSP00 )));
                    if  ( _rec.IsAssigned() )
                    {
                        g04limitprimkeys_subq( m_mblock, *m_segment_ptr2, *_rec, 
                                m_strategy->str_key_in_range().skir_keystart, 
                                startkey.reckey,
                                m_strategy->str_key_in_range().skir_keystop,
                                stopkey.reckey, _ok, _use_stopkeys );
                        _b_err = m_segment_ptr2->sc_read_state;
                    }
                    else
                        _b_err = e_no_more_memory;
                }
                else
                {
                    g04limitprimkeys( m_mblock, 
                        m_strategy->str_key_in_range().skir_keystart, 
                        startkey.reckey, 
                        m_strategy->str_key_in_range().skir_keystop,
                        stopkey.reckey, _use_stopkeys, 
                        IS_UNDEFINED_GG07, -1 /* use an IN as range */ );
                }
            }
        }
        else if ( this->IsIndexAccess() )
        {
            g04limitprimkeys( m_mblock, 
                m_strategy->str_key_in_range().skir_keystart, 
                startkey.reckey, 
                m_strategy->str_key_in_range().skir_keystop,
                stopkey.reckey, _use_stopkeys, 
                IS_UNDEFINED_GG07, -1 /* use an IN as range */ );

                if ( m_strategy->str_inv_in_range().siir_startcnt > 0 || 
                     m_strategy->str_inv_in_range().siir_stopcnt > 0 )
                {
                    if (( strat_inv_in == m_strategy->str_strategy ) )
                    {
                        SAPDBERR_ASSERT_STATE( m_segment_ptr1 > 0 );
                        while ( st_dummy == (*m_mblock.mb_st())[ m_strategy->
                                str_inv_in_range().siir_IN_SUBQ_stpos + 
                                m_segment_ptr1 - 1 ].etype() )
                            ++m_segment_ptr1;
                        SAPDBTRACE_IF( Join_Trace, 7, 
                                t01stackentry( td_always, (*m_mblock.mb_st())[ m_strategy->str_inv_in_range().siir_IN_SUBQ_stpos + m_segment_ptr1 - 1 ], 
                                    m_strategy->str_inv_in_range().siir_IN_SUBQ_stpos + m_segment_ptr1 ));
                        if ( st_value == (*m_mblock.mb_st())[ m_strategy->
                             str_inv_in_range().siir_IN_SUBQ_stpos + 
                             m_segment_ptr1 - 1 ].etype())
                        {
                            SAPDBTRACE_WRITELN(
                                Join_Trace, 3, "switch to next stream segment" );
                            g04limitinvkeys( m_mblock, 
                                    m_strategy->str_inv_in_range(), 
                                    startkey.listkey, stopkey.listkey, 
                                    m_strategy->str_inv_in_range().siir_IN_SUBQ_stpos,
                                    m_segment_ptr1++ );
                        }
                        else
                        {
                            m_segment_ptr1 = 0;
                            _b_err = e_no_next_record;
                        }
                    } 
                    else if (( strat_inv_subq == m_strategy->str_strategy ) )
                    {
                        SAPDBERR_ASSERT_STATE( m_segment_ptr2.IsAssigned() );
                        
                        pasbool    _ok;
                        SAPDB_AutoPtr<tgg00_Rec> _rec(
                                m_acv.GetAllocator(),
                                reinterpret_cast<tgg00_Rec*>( m_acv.GetAllocator().Allocate( BUF_MXSP00 )));
                        if  ( _rec.IsAssigned() )
                        {
                            g04limitinvkeys_subq( m_mblock, *m_segment_ptr2, *_rec, 
                                    m_strategy->str_inv_in_range(),
                                    startkey.listkey, stopkey.listkey, _ok );
                            _b_err = m_segment_ptr2->sc_read_state;
                        }
                        else
                            _b_err = e_no_more_memory;
                    }
                    else 
                    {
                        g04limitinvkeys( m_mblock, 
                                m_strategy->str_inv_in_range(), 
                                startkey.listkey, stopkey.listkey, 
                                IS_UNDEFINED_GG07,
                                -1 /* use an IN as range */ );
                    }
                }
                else
                {
                    startkey.listkey.len() = 0;
                    stopkey.listkey.len() = PERM_KEY_MXSP00;
                    stopkey.listkey.k() = b01fullkey;
                }
        }
        else
        {
            // strat_no_result, strat_more_than_one, strat_catalog
            // read whole result file created
            startkey.reckey.len() = startkey.listkey.len() = 0;
            stopkey.reckey.len()  = stopkey.listkey.len()  = 0;
        }
    }
    else
        _b_err = e_unknown_strategy;
    
#ifdef SAPDB_SLOW
    SAPDBTRACE_WRITELN( Join_Trace, 7, "return: " << SAPDBTrace::BasisError(_b_err) );
    if ( Join_Trace.TracesLevel(1) && e_no_next_record != _b_err)
    {
        t01key( td_always, "start.rec   ", startkey.reckey );
        t01key( td_always, "stop.rec    ", stopkey.reckey );
        if ( IsIndexAccess() )
        {
            t01key( td_always, "start.list  ", startkey.listkey );
            t01key( td_always, "stop.list   ", stopkey.listkey );
        }
    }
#endif
    return _b_err;
}

/* ******************* PROTECTED MEMBERS ******************* */


/* ******************** PRIVATE MEMBERS ******************** */
/*!
 *
 */
tgg00_BasisError Join_AccessDesc::init_mblock()
{
    SAPDBTRACE_METHOD_DEBUG( "Join_AccessDesc::init_mblock", Join_Trace, 3 );
    m_mblock.mb_qual()->msqlmode() = m_acv.a_mblock.mb_qual()->msqlmode();
    m_mblock.mb_qual()->mst_optimize_pos() = 0; // to avoid copying of mb_data
    
    m_mblock.mb_qual() = 0;
    SAPDB_Int4 _quallen = 
        m_mblock.mb_qual_len() < MB_PART1_HEAD_MXGG00 + MB_PART1_RETURN_MXGG00 ? 
        MB_PART1_HEAD_MXGG00 + MB_PART1_RETURN_MXGG00 : m_mblock.mb_qual_len();
    SAPDBTRACE_WRITELN( Join_Trace, 8, "allocate mb_qual: " << m_mblock.mb_qual_len() );
    if ( m_mblock.mb_qual() = reinterpret_cast<tgg00_QualBufPtr>( m_acv.GetAllocator().Allocate( _quallen )))
    {
        SAPDBTRACE_WRITELN( Join_Trace, 8, "allocate mb_qual @ " << (void*)m_mblock.mb_qual() );
        m_mblock.mb_qual_size() = _quallen;

        m_mblock.mb_st() = 0;
        m_mblock.mb_st_max() = m_acv.a_mblock.mb_qual()->mfirst_free()-1;
        m_mblock.mb_st_size() = m_mblock.mb_st_max() * sizeof(tgg00_StackEntry);
        SAPDBTRACE_WRITELN( Join_Trace, 8, "allocate mb_st: " << m_mblock.mb_st_size() );
        if ( m_mblock.mb_st() = reinterpret_cast<tgg00_StackListPtr>( m_acv.GetAllocator().Allocate( m_mblock.mb_st_size() )))
        {
            SAPDBTRACE_WRITELN( Join_Trace, 8, "allocate mb_st @ " << (void*)m_mblock.mb_st() );
        }
    }
    m_mblock.mb_qual()->mst_max() = m_mblock.mb_st_max();
    m_mblock.mb_work_st() = m_acv.a_work_st_addr;
    m_mblock.mb_work_st_max() = m_acv.a_work_st_max;
    m_mblock.mb_workbuf() = m_acv.a_work_buf_addr;
    m_mblock.mb_workbuf_size() = m_acv.a_work_buf_size;

    m_mblock.mb_strat() = 0;
    SAPDBTRACE_WRITELN( Join_Trace, 8, "allocate mb_strat: " << m_mblock.mb_strat_len() );
    if ( m_mblock.mb_strat() = reinterpret_cast<tsp00_MoveObjPtr>( m_acv.GetAllocator().Allocate( m_mblock.mb_strat_len() ) ) )
    {
        SAPDBTRACE_WRITELN( Join_Trace, 8, " allocate mb_strat @ " << (void*)m_mblock.mb_strat() );
        m_mblock.mb_strat_size() = m_acv.a_mblock.mb_strat_len();
    }

    if ( !(m_mblock.mb_qual() && m_mblock.mb_st() && m_mblock.mb_strat() ))
        return  e_no_more_memory;
    else
        {
            SQLMan_MessBlock* pSource      = reinterpret_cast<SQLMan_MessBlock*>(&m_acv.a_mblock);
            SQLMan_MessBlock* pDestination = reinterpret_cast<SQLMan_MessBlock*>(&m_mblock);
            return (pSource->Copy (SQLMan_Context::AcvToContext(m_acv), *pDestination, true /*w/o data*/, NULL));
        }
}
