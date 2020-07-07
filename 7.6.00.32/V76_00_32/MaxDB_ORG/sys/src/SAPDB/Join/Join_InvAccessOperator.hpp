#ifndef JOIN_INVACCESSOPERATOR_HPP
#define JOIN_INVACCESSOPERATOR_HPP
/*!
 * @file  Join_InvAccessOperator.hpp
 * @brief realize one table access operator by means of b03select_invrec()
 *
 * @author GertG
 * @ingroup Join
 *
 * @sa Join_IndexAccessOperator.hpp
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
#include "Join/Join_IndexAccessOperator.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "ggg00.h"
#include "hak01.h"
#include "hbd03.h"
#include "hbd21.h"
#include "hta01.h"
#include "hkb720.h"

class SQLMan_Context;

//! operator for base table access via key or index
class Join_InvAccessOperator : public Join_IndexAccessOperator {
public:
    //! @name constructor / destructor
    //@{
    //! constructor
    /*!
     * @param[in] acv global context
     * @param[in] tabno number of table
     * @param[in] compressed_buf_usage overlap records with cgg_rec_key_offset
     * @param[in] bufferPtr pointer to predefined buffer
     * @param[in] buffersize size of table buffer
     */
    Join_InvAccessOperator(
        SQLMan_Context&       acv,
        const SAPDB_UInt2     tabno, 
        const bool            compressed_buf_usage = true,
        SAPDB_Byte*           bufferPtr     = 0,
        const SAPDB_UInt4&    buffersize    = BUF8K_MXSP00)
    :
    Join_IndexAccessOperator(acv, tabno, compressed_buf_usage, 
            bufferPtr, buffersize)
    {
        SAPDBTRACE_METHOD_DEBUG( "Join_InvAccessOperator::ctor", Join_Trace, 1 );

        if ( e_ok != m_acv.TransContext().trError_gg00 )
            acv.ThrowError( m_acv.TransContext().trError_gg00 );
    }
    //@}

protected:

    // member methods
    /*!
     * @param[in,out] record pointer to memory should be filled with record
     * @return information code [e_ok, e_no_next_record] / error code [ ... ]
     */
    virtual void SelectRecords()
    {
        SAPDBTRACE_METHOD_DEBUG(
            "Join_InvAccessOperator::SelectRecords", Join_Trace, 1 );

        SAPDBTRACE_IF(
            Join_Trace, 5,
            t01surrogate( td_always, "sel INVID   ",
                          m_AccessDesc.FileIDs().inv_id.fileTabId_gg00()));
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

        do {
            tgg00_LockReqMode _granted_lock;

            SAPDBTRACE_WRITELN(
                Join_Trace, 5,
                "records demanded: " << m_SelFields.sfp_m_result_cnt() << NewLine
                << "\treclen: " << m_SelFields.sfp_resrec_maxlen() );

            SAPDBTRACE_WRITELN(
                Join_Trace, 3,
                "keyLen: " << m_Actualkeys.listkey.len() << NewLine
                << "mess2_type: " << m_SelFields.sfp_bd_mess2_type() );
#ifdef SAPDB_SLOW
            t01buf( td_always, &m_Actualkeys.listkey.k(), 1, m_Actualkeys.listkey.len());
#endif
            m_SelFields.sfp_rows_read() = 0;
            m_SelFields.sfp_rows_qual() = 0;

            b03select_invrec(
                m_acv.TransContext(),
                m_AccessDesc.FileIDs(),
                m_Actualkeys,
                m_Stopkeys,
                m_Startkeys.reckey,
                m_RangeSet,
                0 /*recsize*/, 0 /*recptr*/,
                m_SelFields,
                m_AccessDesc.MBlock().mb_qual()->mstack_desc(),
                _granted_lock);
            _bdState = m_acv.TransContext().trError_gg00;

            _recordsRead += m_SelFields.sfp_m_result_cnt();

            if  ( a01diag_monitor_on || a01diag_analyze_on )  
            {
                if ( (GetOpenCnt() == 1 ) && (NIL_PAGE_NO_GG00 != m_AccessDesc.FileIDs().inv_id.fileRoot_gg00()) )
                {
                    b21mp_root_put (m_AccessDesc.MBlock().mb_trns()->trBdTcachePtr_gg00,
                       m_AccessDesc.FileIDs().inv_id.fileRoot_gg00());

                    k720diagmon_strat_put (*m_AccessDesc.MBlock().mb_trns(), *m_AccessDesc.GetStrategyPtr());
                }
                b21mp_rows_read_put (m_AccessDesc.MBlock().mb_trns()->trBdTcachePtr_gg00, m_SelFields.sfp_rows_read());
                b21mp_rows_qual_put (m_AccessDesc.MBlock().mb_trns()->trBdTcachePtr_gg00, m_SelFields.sfp_rows_qual());
            }

            if ( (e_ok != _bdState) && (e_no_next_record != _bdState) )
                // error has occured
                break;

            if ( (_recordsWanted != m_SelFields.sfp_m_result_cnt())
                 && ( e_ok == _bdState ) )
                // buffer full, possibly more records available
                break;

            if ( (1 == m_maxRowRequest) && (e_ok == _bdState) )
            {
                // found one record, that is all we wanted
                _bdState = m_acv.TransContext().trError_gg00 = e_no_next_record;
                break;
            }
            
            if ( e_no_next_record == _bdState )
            {
                if  (( strat_inv_in == m_AccessDesc.Strategy()->str_strategy  ) ||
                     ( strat_inv_subq == m_AccessDesc.Strategy()->str_strategy ))
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
        } while ( e_no_next_record != _bdState );

        SAPDBTRACE_WRITELN( Join_Trace, 5, "BD call: " << SAPDBTrace::BasisError(m_acv.TransContext().trError_gg00) );

        m_SelFields.sfp_m_result_cnt() = _recordsRead;

        if ( (e_buffer_limit == _bdState)
             && (0 < m_SelFields.sfp_m_result_cnt()) ) 
        {
            SAPDBTRACE_WRITELN( Join_Trace, 5, "buffer limit! " );
            // a new stream segment was opened but no record fit in the
            // buffer any more
            m_acv.TransContext().trError_gg00 = e_ok;
            m_SelFields.sfp_bd_mess2_type().becomes( mm_first );
        }
    }
};

#endif
