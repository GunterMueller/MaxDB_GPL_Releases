/*!
 * @file  Join_TableAccessOperator.cpp
 * @brief implements Join_TableAccessOperator
 *
 * @author GertG
 * @ingroup Join
 *
 * @sa Join_TableAccessOperator.hpp
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
#include "Trace/Trace_Entry.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "Join/Join_TableAccessOperator.hpp"
#include "Join/Join_AccessOperator.hpp"
#include "ggg00.h"
#include "hgg04.h"
#include "ggg07.h"
#include "hbd02.h"
#include "hbd21.h"
#include "hta01.h"
#include "hta01_3.h"
#include "hak70.h"
#include "hak01.h"
#include "hkb720.h"
#include "hak101.h"
#include "hbd998.h"
#include "hak01.h"

class SQLMan_Context;

/* ******************** PUBLIC MEMBERS ********************* */
/*!
 * @param[in] acv global context
 * @param[in] tabno number of table
 * @param[in] compressed_buf_usage overlap records with cgg_rec_key_offset
 * @param[in] bufferPtr pointer to predefined buffer
 * @param[in] buffersize size of table buffer
 */
Join_TableAccessOperator::Join_TableAccessOperator(
        SQLMan_Context     &acv,
        const SAPDB_UInt2  tabno,
        const bool         compressed_buf_usage,
        SAPDB_Byte         *bufferPtr,
        const SAPDB_UInt4  &buffersize)
    :
    Join_AccessOperator(acv, tabno, compressed_buf_usage, bufferPtr, buffersize)
{
    SAPDBTRACE_METHOD_DEBUG( "Join_TableAccessOperator::ctor", Join_Trace, 1 );
    SAPDBERR_ASSERT_STATE( strat_more_than_one != m_AccessDesc.Strategy()->str_strategy 
            && strat_catalog != m_AccessDesc.Strategy()->str_strategy && 
            ! a70glob_inv_strats.includes( m_AccessDesc.Strategy()->str_strategy ) &&
            ! a70glob_join_inv_strats.includes( m_AccessDesc.Strategy()->str_strategy ));
}

/* ******************* PROTECTED MEMBERS ******************* */

void Join_TableAccessOperator::SelectRecords()
{
    SAPDBTRACE_METHOD_DEBUG(
        "Join_TableAccessOperator::SelectRecords", Join_Trace, 3 );

    SAPDBTRACE_IF(
        Join_Trace, 5,
        t01surrogate( td_always, "sel TABID   ",
                      m_AccessDesc.FileIDs().file_id.fileTabId_gg00()));

    m_SelFields.sfp_result_length() = 0;
    m_SelFields.sfp_m_result_len()  = 0;
    m_SelFields.sfp_m_result_size() = m_RecBufSize;
    m_SelFields.sfp_m_result_addr() = reinterpret_cast<tsp00_MoveObjPtr>( m_RecBuf );
    m_SelFields.sfp_m_result_cnt()  = m_maxRowRequest; // set buffer count
    
    const SAPDB_Int4 _recordsWanted = m_SelFields.sfp_m_result_cnt();
    SAPDB_Int4       _recordsRead   = 0;
    tgg00_BasisError _bdState;

    // loop for virtual file (virtual file comprises several physical files [see recursive select])
    do {
        pasbool           _unqualified;
        tgg00_LockReqMode _granted_lock;

        SAPDBTRACE_WRITELN(
            Join_Trace, 5,
            "records demanded: " << m_SelFields.sfp_m_result_cnt() << NewLine
            << "\treclen: " << m_SelFields.sfp_resrec_maxlen() );

        SAPDBTRACE_WRITELN(
            Join_Trace, 3,
            "keyLen: " << m_Actualkeys.reckey.len() << NewLine
            << "mess2_type: " << m_SelFields.sfp_bd_mess2_type() );
#ifdef SAPDB_SLOW
        SAPDBTRACE_IF(Join_Trace, 5, t01buf( td_always, &m_Actualkeys.reckey.k(), 1, m_Actualkeys.reckey.len()));
#endif
        m_SelFields.sfp_rows_read() = 0;
        m_SelFields.sfp_rows_qual() = 0;
            
        b02kb_select_rec(
            m_acv.TransContext(),
            m_AccessDesc.FileIDs().file_id,
            (tsp00_KeyPtr) &m_Actualkeys.reckey.k(),
            m_Actualkeys.reckey.len(),
            (tsp00_KeyPtr) &m_Stopkeys.reckey.k(),
            m_Stopkeys.reckey.len(),
            0 /*recsize*/, 0 /*recptr*/,
            false /* ignore_vwait*/,
            m_SelFields,
            m_AccessDesc.MBlock().mb_qual()->mstack_desc(),
            _unqualified,
            _granted_lock );
        _bdState = m_acv.TransContext().trError_gg00;

        if  ( a01diag_monitor_on || a01diag_analyze_on )  
        {
            if ( (GetOpenCnt() == 1 ) && (NIL_PAGE_NO_GG00 != m_AccessDesc.FileIDs().file_id.fileRoot_gg00()) )
            {
                b21mp_root_put (m_AccessDesc.MBlock().mb_trns()->trBdTcachePtr_gg00,
                   m_AccessDesc.FileIDs().file_id.fileRoot_gg00());
                
                k720diagmon_strat_put (*m_AccessDesc.MBlock().mb_trns(), *m_AccessDesc.GetStrategyPtr());
            }
            b21mp_rows_read_put (m_AccessDesc.MBlock().mb_trns()->trBdTcachePtr_gg00, m_SelFields.sfp_rows_read());
            b21mp_rows_qual_put (m_AccessDesc.MBlock().mb_trns()->trBdTcachePtr_gg00, m_SelFields.sfp_rows_qual());
        }
        
        _recordsRead += m_SelFields.sfp_m_result_cnt();

        if ( (e_ok != _bdState) && (e_no_next_record != _bdState) )
            // error has occured
            break;

        if ( (_recordsWanted != m_SelFields.sfp_m_result_cnt())
             && ( e_ok == _bdState ) )
            // buffer full, possibly more records available
            break;

        if ( ( 1 == m_maxRowRequest ) && ( e_ok == _bdState ) )
        {
            // found one record, that is all we wanted
            _bdState = m_acv.TransContext().trError_gg00 = e_no_next_record;
            break;
        }

        if ( e_no_next_record == _bdState )
        {
            // got all records from this file
            if ( m_AccessDesc.IsVirtualFile() )
            {
                SAPDBTRACE_WRITELN(
                    Join_Trace, 3, "switch to next recursive file" );
                m_AccessDesc.SetNextFileID();

                SAPDBTRACE_IF(
                    Join_Trace, 5,
                    t01treeid( td_always, "next treeid ",
                               m_AccessDesc.FileIDs().file_id));
                m_SelFields.sfp_m_result_len()  = 0;
                m_SelFields.sfp_result_length() = 0;
                m_Actualkeys = m_Startkeys;
                m_SelFields.sfp_bd_mess2_type().becomes( mm_first );
                m_SelFields.sfp_m_result_cnt() = m_maxRowRequest;

                _bdState = e_ok;
            }
            else
            {
                if  (( strat_key_in   == m_AccessDesc.Strategy()->str_strategy  ) ||
                     ( strat_key_subq == m_AccessDesc.Strategy()->str_strategy  ))
                {
                    // try to switch next stream segment
                    m_acv.TransContext().trError_gg00 = m_AccessDesc.CreateKeys( m_Startkeys, m_Stopkeys );
                    if  ( e_ok == m_acv.TransContext().trError_gg00 )
                    {
                        SAPDBTRACE_WRITELN( Join_Trace, 5, "previous records from BD: " << m_SelFields.sfp_m_result_cnt() );
                        m_SelFields.sfp_m_result_len()  = 0;
                        m_SelFields.sfp_result_length() = 0;
                        m_Actualkeys = m_Startkeys;
                        m_SelFields.sfp_bd_mess2_type().becomes( mm_first );
                        m_SelFields.sfp_m_result_cnt() = m_maxRowRequest;

                        _bdState = e_ok;
                    }
                }
                else
                {
                    SAPDBTRACE_WRITELN( Join_Trace, 3, "file empty!" );
                }
            }
        }
    } while ( e_no_next_record != _bdState );

    SAPDBTRACE_WRITELN( Join_Trace, 5, "BD call: " << SAPDBTrace::BasisError(m_acv.TransContext().trError_gg00) );

    m_SelFields.sfp_m_result_cnt() = _recordsRead;

    if ( ( e_buffer_limit == _bdState ) && ( 0 < m_SelFields.sfp_m_result_cnt() ) ) 
    {
        SAPDBTRACE_WRITELN( Join_Trace, 5, "buffer limit! " );
        // a new virtual file/stream segment was opened but no record fit in the
        // buffer any more
        m_acv.TransContext().trError_gg00 = e_ok;
        m_SelFields.sfp_bd_mess2_type().becomes( mm_first );
    }
}

/*************************************************************************/

tgg00_BasisError Join_TableAccessOperator::reset()
{
    SAPDBTRACE_METHOD_DEBUG( "Join_TableAccessOperator::reset", Join_Trace, 5 );

    (void) Join_AccessOperator::reset();

    m_RangeSet.clear();

    if ( m_Startkeys.reckey.len() != 0 )
        m_RangeSet.addElement( primary_start );

    if ( m_Stopkeys.reckey.len() != 0
         && m_Stopkeys.reckey.k()[0] != csp_undef_byte )
        m_RangeSet.addElement( primary_stop );

    // initialize select fields parameter
    g04init_select_fields( m_SelFields, 
            &m_AccessDesc.MBlock().mb_data()->mbp_buf(), 
            m_AccessDesc.MBlock().mb_data_size(), 
            m_AccessDesc.MBlock().mb_work_st(), 
            m_AccessDesc.MBlock().mb_work_st_max(), 
            m_AccessDesc.MBlock().mb_workbuf(), 
            m_AccessDesc.MBlock().mb_workbuf_size(),
            m_AccessDesc.SQLMode(),
            &m_AccessDesc.MBlock().mb_fieldlists() );

    // siehe vkb720 kb720sel_first_part()
    m_SelFields.sfp_bd_mess_type().becomes( m_select );
    m_SelFields.sfp_bd_mess2_type().becomes( mm_first );
    m_SelFields.sfp_result_wanted() = true;
    m_SelFields.sfp_filler2()       = false;
    m_SelFields.sfp_bd_use_stopkey()= m_RangeSet.includes( primary_stop ); 
    m_SelFields.sfp_bd_inv_only().becomes( m_AccessDesc.Strategy()->str_qual_kind );

    if  (( a01join_clust_read || m_AccessDesc.Strategy()->str_use_clusterIO ) &&
         ( m_acv.TransContext().trClusterIOPtr_gg00  != NULL ) &&
         ( ! a101_IsTempFile(m_acv, m_AccessDesc.MBlock().mb_qual()->mtree()) ) &&
         ( bd998ArePagesClustered( m_acv.TransContext(), m_AccessDesc.MBlock().mb_qual()->mtree().fileTabId_gg00() ) ) )
    {
        m_SelFields.sfp_cluster_info().clust_bDoClusteredRead_gg00 = true;
        m_SelFields.sfp_cluster_info().clust_BufferSize_gg00       = m_acv.TransContext().trClusterIOSize_gg00;
        m_SelFields.sfp_cluster_info().clust_pIOBuffer_gg00        = (tsp00_Addr ) m_acv.TransContext().trClusterIOPtr_gg00;
    }

    m_SelFields.sfp_check_for_result() = false; // siehe k75_fetch
    m_SelFields.sfp_rows_read() = 0;
    m_SelFields.sfp_rows_qual() = 0;
    m_SelFields.sfp_act_cntresult() = 0;
    m_SelFields.sfp_m_result_cnt() = 0;
    m_SelFields.sfp_acv_addr() = reinterpret_cast<tsp00_Addr>( &m_acv );
    if ( m_CompressedBufferUsage )
    {
        // we overlap records with cgg_rec_key_offset
        // (because of sfp_resrec_maxlen = reclen without cgg_rec_key_offset)
        m_SelFields.sfp_resrec_maxlen() = GetRecordLength() - cgg_rec_key_offset;
    }
    else
        // we can write record header into buffer
        m_SelFields.sfp_resrec_maxlen() = GetRecordLength();

    if ( 1 == m_SelFields.sfp_resrec_maxlen() % 2 ) ++m_SelFields.sfp_resrec_maxlen();
    SAPDBTRACE_WRITELN( Join_Trace, 3, "m_RecBufSize aligned " << m_RecBufSize );
    SAPDBTRACE_WRITELN( Join_Trace, 3, "sfp_resrec_maxlen aligned " << m_SelFields.sfp_resrec_maxlen() );
    SAPDBERR_ASSERT_STATE( m_SelFields.sfp_resrec_maxlen() <= (SAPDB_Int4) m_RecBufSize );

    return e_ok;
}


/* ******************** PRIVATE MEMBERS ******************** */
