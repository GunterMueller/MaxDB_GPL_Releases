/*!
 * @file  Join_InvAccessOperatorEx.cpp
 * @brief implements Join_IndexAccessOperator
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
#include "Join/Join_InvAccessOperatorEx.hpp"
#include "Join/Join_IndexAccessOperator.hpp"
#include "Join/Join_InvSelectIterator.hpp"
#include "Trace/Trace_Entry.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"

#include "ggg00.h"
#include "ggg07.h"
#include "hta01.h"

class SQLMan_Context;

/* ******************** PUBLIC MEMBERS ********************* */
/*!
 * @param[in] acv global context
 * @param[in] tabno number of table
 * @param[in] compressed_buf_usage overlap records with cgg_rec_key_offset
 * @param[in] bufferPtr pointer to predefined buffer
 * @param[in] buffersize size of table buffer
 * @param[in] maxServerTasks max amount of free server tasks
 */
Join_InvAccessOperatorEx::Join_InvAccessOperatorEx(
        SQLMan_Context&    acv,
        const SAPDB_UInt2  tabno,
        const bool         compressed_buf_usage,
        SAPDB_Byte*        bufferPtr,
        const SAPDB_UInt4& buffersize,
        const SAPDB_Int4&  maxServerTasks)
    :
    Join_IndexAccessOperator(acv, tabno, compressed_buf_usage, 
            bufferPtr, buffersize),
    m_pInvIterator(0)
{
    if ( ! m_acv.IsOk() ) return;

    SAPDBTRACE_METHOD_DEBUG( "Join_InvAccessOperatorEx::ctor", Join_Trace, 1 );
    SAPDBERR_ASSERT_STATE( m_AccessDesc.IsJoinInvAccess() );

    // try to create an inv list iterator, which is able to handle
    // the inv list in parallel
    m_pInvIterator = new(m_allocator)
        Join_InvSelectIterator (
            m_acv.TransContext(),
            *reinterpret_cast<SQLMan_MessBlock*>(&m_AccessDesc.MBlock()),
            m_AccessDesc.FileIDs().inv_id,
            m_SelFields, m_AccessDesc.MBlock().mb_qual()->mstack_desc(),
            maxServerTasks);

    if ( 0 == m_pInvIterator )
        acv.ThrowError( e_no_more_memory );
    
    if ( e_ok != m_acv.TransContext().trError_gg00 )
        acv.ThrowError( m_acv.TransContext().trError_gg00 );
}

Join_InvAccessOperatorEx::~Join_InvAccessOperatorEx()
{
    SAPDBTRACE_METHOD_DEBUG( "Join_InvAccessOperatorEx::~Join_InvAccessOperatorEx", Join_Trace, 1 );

    destroy( m_pInvIterator, m_allocator );
}

tgg00_BasisError Join_InvAccessOperatorEx::Open()
{
    SAPDBERR_ASSERT_STATE( 0 == m_pInvIterator );
    return Join_IndexAccessOperator::Open();
}

/*!
 * @param[in] startkeys restrict record stream to records with key greater/equal than startkey
 * @param[in] stopkeys restrict record stream to records with key lower/equal than stopkey
 */
tgg00_BasisError Join_InvAccessOperatorEx::Open(
            const Join_TwoKeys& startkeys,
            const Join_TwoKeys& stopkeys )
{
    SAPDBTRACE_METHOD_DEBUG( "Join_InvAccessOperatorEx::Open(startkeys,stopkeys)", Join_Trace, 1 );

    tgg00_BasisError  _e;
    _e = Join_IndexAccessOperator::Open( startkeys, stopkeys );

    if ( e_ok == _e )
        _e = m_pInvIterator->Open(
            m_Startkeys.listkey,
            m_Startkeys.reckey,
            m_Stopkeys.reckey,
            m_RangeSet );
    return _e;
}

/*!
 *
 */
void Join_InvAccessOperatorEx::Close()
{
    SAPDBTRACE_METHOD_DEBUG( "Join_InvAccessOperatorEx::Close", Join_Trace, 1 );

    Join_IndexAccessOperator::Close();

    m_pInvIterator->Close();
}

/* ******************* PROTECTED MEMBERS ******************* */

/*!
 * @param[in,out] record pointer to memory should be filled with record
 * @return information code [e_ok, e_no_next_record] / error code [ ... ]
 */
void Join_InvAccessOperatorEx::SelectRecords()
{
    SAPDBTRACE_METHOD_DEBUG( "Join_InvAccessOperatorEx::SelectRecords", Join_Trace, 1 );

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

    m_acv.TransContext().trError_gg00 = m_pInvIterator->NextRow(0);

    if (e_ok == m_acv.TransContext().trError_gg00)
    {
        m_SelFields.sfp_m_result_cnt() = 1;
    }
    else
    {
        if (e_inv_list_not_found == m_acv.TransContext().trError_gg00)
        {
            m_acv.TransContext().trError_gg00 = e_no_next_record;
        }
        m_SelFields.sfp_m_result_cnt() = 0;
    }
}


/* ******************** PRIVATE MEMBERS ******************** */

