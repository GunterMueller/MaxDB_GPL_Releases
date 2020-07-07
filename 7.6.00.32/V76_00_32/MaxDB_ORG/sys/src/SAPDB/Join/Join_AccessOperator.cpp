/*!
 * @file
 * @brief implements Join_AccessOperator
 *
 * @author GertG
 * @ingroup Join
 *
 * @sa Join_AccessOperator.hpp
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
#include "ggg00.h"
#include "hgg01_3.h"
#include "ggg11.h"
#include "gsp00.h"
#include "hak101.h"
#include "hta01.h"
#include "hta01_3.h"
#include "hgg04.h"
#include "SQLManager/SQLMan_Context.hpp"
#include "Join/Join_IAccessOperator.hpp"
#include "Trace/Trace_Entry.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "Join/Join_AccessOperator.hpp"

/* ******************** PUBLIC MEMBERS ********************* */
/*!
 * @param[in] acv global context
 * @param[in] tabno number of table
 * @param[in] compressed_buf_usage overlap records with cgg_rec_key_offset
 * @param[in] bufferPtr pointer to predefined buffer
 * @param[in] buffersize size of table buffer
 */
Join_AccessOperator::Join_AccessOperator(
        SQLMan_Context        &acv,
        const SAPDB_UInt2     tabno,
        const bool            compressed_buf_usage,
        SAPDB_Byte            *bufferPtr,
        const SAPDB_UInt4&    buffersize)
    :
    Join_IAccessOperator(acv, tabno),
    m_allocator( acv.GetAllocator() ),
    // m_Actualkeys
    m_RecBuf(bufferPtr),
    m_RecBufSize(buffersize),
    m_Record(0),
    m_CompressedBufferUsage(compressed_buf_usage),
    //m_BDState
    m_AllRecsInBuf(0),
    m_foreignRecBuf(0 != bufferPtr),
    m_FirstBufferFill(true),
    m_BufferReusagePossible(false)
{
    if ( ! m_acv.IsOk() ) return;
    
    SAPDBTRACE_METHOD_DEBUG( "Join_AccessOperator::ctor", Join_Trace, 1 );

    SAPDBTRACE_WRITELN( Join_Trace, 5, "m_CompressedBufferUsage: " << m_CompressedBufferUsage << " m_foreignRecBuf: " << m_foreignRecBuf ) ;
    SAPDBTRACE_WRITELN( Join_Trace, 5, "m_RecBufSize: " << m_RecBufSize );

    tgg00_BasisError _b_err = e_ok;

    m_acv.TransContext().trRteCommPtr_gg00->file_record_cnt = 1;

    if ( ! m_foreignRecBuf ) {
        if ( m_RecBufSize < this->GetRecordLength() )
        {
            m_RecBufSize = this->GetRecordLength();
            SAPDBTRACE_WRITELN(
                Join_Trace, 3, "m_RecBufSize = record length " << m_RecBufSize );
        }

        if ( 1 == m_RecBufSize % 2 ) 
        {
            ++m_RecBufSize;
            SAPDBTRACE_WRITELN(
                Join_Trace, 3, "m_RecBufSize aligned " << m_RecBufSize );
            // this->rec_offset() <= m_RecBufSize !!!
        }

        if (!( m_RecBuf = m_acv.GetAllocator().Allocate( m_RecBufSize )))
            _b_err = e_no_more_memory;
    }

    if ( _b_err != e_ok )
        acv.ThrowError( _b_err );
}

/*!
 *
 */
Join_AccessOperator::~Join_AccessOperator()
{
    SAPDBTRACE_METHOD_DEBUG( "Join_AccessOperator::dtor", Join_Trace, 1 );
    if ( this->IsOpened() ) this->Close();

    if ( ! m_foreignRecBuf && m_RecBuf )
        m_acv.GetAllocator().Deallocate( m_RecBuf );
}

/*!
 *
 */
tgg00_BasisError Join_AccessOperator::Open()
{
    SAPDBTRACE_METHOD_DEBUG( "Join_AccessOperator::Open", Join_Trace, 1 );
    
    if ( g01vtrace.vtrAll_gg00 )
    {
        // default vtrace
        tgg11_JoinTabIdTrace _trace;
        _trace.jtHead_gg11.kbMessType_gg11.becomes( m_nil );
        _trace.jtHead_gg11.kbMessType2_gg11.becomes( mm_nil );
        _trace.jtType_gg11.becomes( jntrOpen_gg11 );
        _trace.jtTabId_gg11 = m_AccessDesc.MBlock().mb_qual()->mtree().fileTabId_gg00();
        _trace.jtTabno_gg11 = m_TableNo;

        Trace_CommonEntry Entry(
            tgg00_Debug::fromConst(ak_join) /* Layer */,
            tgg00_VtraceType::fromConst(vttAccessOperator_egg00),
            (tsp00_BytePtr) &_trace, 
            sizeof( _trace ) );
    }
    
    tgg00_BasisError _b_err;
    Join_TwoKeys _old_startkeys(m_Startkeys), _old_stopkeys(m_Stopkeys);

    _b_err = IOperator::Open();

    if ( e_ok == _b_err ) 
        _b_err = m_AccessDesc.CreateKeys( m_Startkeys, m_Stopkeys );

    if ( e_ok == _b_err ) 
    {
        m_FirstBufferFill = true;
        _b_err = reset();
    }

    if ( e_ok == _b_err ) 
        check_buffer_reusage( _old_startkeys, _old_stopkeys );

    return _b_err;
}

/*!
 * @param[in] startkeys restrict record stream to records with key greater/equal than startkey
 * @param[in] stopkeys restrict record stream to records with key lower/equal than stopkey
 */
tgg00_BasisError Join_AccessOperator::Open(
            const Join_TwoKeys& startkeys,
            const Join_TwoKeys& stopkeys )
{
    SAPDBTRACE_METHOD_DEBUG( "Join_AccessOperator::Open(startkeys,stopkeys)", Join_Trace, 1 );

#ifdef SAPDB_SLOW
    if ( Join_Trace.TracesLevel( 3 ) )
    {
        t01key( td_always, "start.rec   ", const_cast<tgg00_Lkey&>(startkeys.reckey ));
        t01key( td_always, "stop.rec    ", const_cast<tgg00_Lkey&>(stopkeys.reckey ));
        t01key( td_always, "start.inv   ", const_cast<tgg00_Lkey&>(startkeys.listkey ));
        t01key( td_always, "stop.inv    ", const_cast<tgg00_Lkey&>(stopkeys.listkey ));
    }
#endif
    if ( g01vtrace.vtrAll_gg00 )
    {
        // default vtrace
        tgg11_JoinTabIdTrace _trace;
        _trace.jtHead_gg11.kbMessType_gg11.becomes( m_nil );
        _trace.jtHead_gg11.kbMessType2_gg11.becomes( mm_nil );
        _trace.jtType_gg11.becomes( jntrOpen1_gg11 );
        _trace.jtTabId_gg11 = m_AccessDesc.MBlock().mb_qual()->mtree().fileTabId_gg00();
        _trace.jtTabno_gg11 = m_TableNo;

        Trace_CommonEntry Entry(
            tgg00_Debug::fromConst(ak_join) /* Layer */,
            tgg00_VtraceType::fromConst(vttAccessOperator_egg00),
            (tsp00_BytePtr) &_trace, 
            sizeof( _trace ) );
    }
    
    tgg00_BasisError _b_err;

    Join_TwoKeys _old_startkeys(m_Startkeys), _old_stopkeys(m_Stopkeys);

    _b_err = IOperator::Open( startkeys, stopkeys );
    m_FirstBufferFill = true;
    
    if ( e_ok == _b_err ) _b_err = reset();

    if ( e_ok == _b_err ) 
        check_buffer_reusage( _old_startkeys, _old_stopkeys );

    return _b_err;
}

/*!
 * @param[in,out] recptr pointer to memory should be filled with record
 * @return information code [e_ok, e_no_next_record] / error code [ ... ]
 */
tgg00_BasisError Join_AccessOperator::Next( tgg00_Rec*& recptr )
{
    SAPDBTRACE_METHOD_DEBUG( "Join_AccessOperator::Next", Join_Trace, 1 );

    m_acv.TransContext().trError_gg00 = e_ok;

    if ( is_empty_buffer() )
    {
        if ( e_no_next_record == m_BDState )
        {
            SAPDBTRACE_WRITELN( Join_Trace, 3, "file empty!" );
            m_acv.TransContext().trError_gg00 = e_no_next_record;
            m_BDState = e_ok;
        }

        if ( e_ok == m_acv.TransContext().trError_gg00 )
        {
            SAPDBTRACE_WRITELN( Join_Trace, 3, "fill buffer" );
            SAPDBTRACE_IF( Join_Trace, 3, SAPDB_MemFillNoCheck(m_RecBuf, ':', m_RecBufSize) );
            m_acv.TransContext().trRteCommPtr_gg00->file_root = NIL_PAGE_NO_GG00;

            // child classes need to implement this:
            this->SelectRecords();

            // bd delivers e_buffer_limit if one record fits not at all
            // we can't get this error because auf m_RecBufSize >= BUF8L_MXSP00
            m_BDState = m_acv.TransContext().trError_gg00;

            SAPDBTRACE_WRITELN( Join_Trace, 5, "BD call: " << SAPDBTrace::BasisError(m_acv.TransContext().trError_gg00) );
            if ( ( e_no_next_record == m_BDState && this->result_cnt() > 0 ))
                m_acv.TransContext().trError_gg00 = e_ok;

            SAPDBTRACE_WRITELN( Join_Trace, 5, "records from BD: " << this->result_cnt() );

            if ( e_no_next_record == m_BDState && m_FirstBufferFill )
            {
                SAPDBTRACE_WRITELN( Join_Trace, 5, "all recs in buffer " );
                m_AllRecsInBuf = this->result_cnt();
                m_BufferReusagePossible = true;
            }
            m_FirstBufferFill = false;


            // get first record from buffer
            m_Record = reinterpret_cast<SAPDB_Byte*>( m_RecBuf );
        }
    }
    else
    {
        SAPDBTRACE_WRITELN( Join_Trace, 3, "get record from buffer" );
        SAPDBTRACE_WRITELN( Join_Trace, 5, "records in buffer: " << this->result_cnt() );
        // get next record from buffer
        m_Record += this->rec_offset();
    }
    SAPDBTRACE_WRITELN(
        Join_Trace, 3,
        "Next(): "
        << SAPDBTrace::BasisError(
            m_acv.TransContext().trError_gg00) );

#ifdef SAPDB_SLOW
    if ( e_ok == m_acv.TransContext().trError_gg00 )
    {
        SAPDBTRACE_IF( Join_Trace, 5, 
            t01surrogate(
                td_always, "sel TABID   ",
                m_AccessDesc.MBlock().mb_qual()->mtree().fileTabId_gg00()));
        SAPDBTRACE_WRITELN(
            Join_Trace, 5,
            "records (reclen w/o rec offset="
            << this->rec_offset()
            << "[" << this->GetRecordLength()-cgg_rec_key_offset << "])" );
        SAPDBTRACE_WRITELN( Join_Trace, 5, "next rec is " );
        SAPDBTRACE_IF( Join_Trace, 5, 
            t01buf( td_always, m_Record, 1,
                    this->rec_offset()
                    <= 300
                    ? (this->rec_offset() + cgg_rec_key_offset)
                    : 300 ));
        SAPDBTRACE_WRITELN( Join_Trace, 5, "rec buffer:" );
        SAPDBTRACE_IF( Join_Trace, 9, 
            t01buf( td_always, m_Record, 1,
                    this->result_cnt()
                    * this->rec_offset()
                    <= 300
                    ? (this->result_cnt()
                       * this->rec_offset() + cgg_rec_key_offset)
                    : 300 ));
    }
#endif
    --this->result_cnt(); // remaining records in buffer
    recptr = reinterpret_cast<tgg00_Rec*>( m_Record );

    return m_acv.TransContext().trError_gg00;
}

/*!
 *
 */
void Join_AccessOperator::Close()
{
    SAPDBTRACE_METHOD_DEBUG( "Join_AccessOperator::Close", Join_Trace, 1 );

    if ( g01vtrace.vtrAll_gg00 )
    {
        // default vtrace
        tgg11_JoinTabIdTrace _trace;
        _trace.jtHead_gg11.kbMessType_gg11.becomes( m_nil );
        _trace.jtHead_gg11.kbMessType2_gg11.becomes( mm_nil );
        _trace.jtType_gg11.becomes( jntrClose_gg11 );
        _trace.jtTabId_gg11 = m_AccessDesc.MBlock().mb_qual()->mtree().fileTabId_gg00();
        _trace.jtTabno_gg11 = m_TableNo;

        Trace_CommonEntry Entry(
            tgg00_Debug::fromConst(ak_join) /* Layer */,
            tgg00_VtraceType::fromConst(vttAccessOperator_egg00),
            (tsp00_BytePtr) &_trace, 
            sizeof( _trace ) );
    }

    m_AccessDesc.ResetFileID();
    IOperator::Close();
}

SAPDB_Bool Join_AccessOperator::IsResultFullyBuffered() const
{
    return m_BufferReusagePossible;
}


SAPDB_UInt4 Join_AccessOperator::GetBufferedRecordCount() const
{
    return m_AllRecsInBuf;
}

/* ******************* PROTECTED MEMBERS ******************* */


/* ******************** PRIVATE MEMBERS ******************** */
/*!
 *
 */
tgg00_BasisError Join_AccessOperator::reset()
{
    SAPDBTRACE_METHOD_DEBUG( "Join_AccessOperator::reset", Join_Trace, 3 );

    m_Actualkeys = m_Startkeys;
    m_BDState = e_ok;
    return e_ok;
}

/* ******************** PRIVATE MEMBERS ******************** */
/*!
 *
 */
inline
bool Join_AccessOperator::is_same_key(
        const Join_TwoKeys& old_key,
        const Join_TwoKeys& new_key) const
{
    if ( old_key.reckey.keyLen_gg00() == new_key.reckey.keyLen_gg00() )
    {
        short _diff = memcmp(&old_key.reckey.keyVal_gg00(), &new_key.reckey.keyVal_gg00(), new_key.reckey.keyLen_gg00());
#ifdef SAPDB_SLOW
        if ( _diff )
            SAPDBTRACE_WRITELN( Join_Trace, 7, "different key content" );
#endif
        if ( (!_diff) && m_AccessDesc.IsIndexAccess() || m_AccessDesc.IsJoinInvAccess() )
        {
            if ( old_key.listkey.keyLen_gg00() == new_key.listkey.keyLen_gg00() )
            {
                _diff += memcmp(&old_key.listkey.keyVal_gg00(), &new_key.listkey.keyVal_gg00(), new_key.listkey.keyLen_gg00());
#ifdef SAPDB_SLOW
                if ( _diff )
                    SAPDBTRACE_WRITELN( Join_Trace, 7, "different index content" );
#endif
            }
            else
                // diff length of secondary keys
                return false;
        }
        return !_diff;
    }
    // diff length of primary keys
    return false;
}

/*!
 *
 */
void Join_AccessOperator::check_buffer_reusage(
        const Join_TwoKeys& old_startkeys,
        const Join_TwoKeys& old_stopkeys )
{
    SAPDBTRACE_METHOD_DEBUG( "Join_AccessOperator::check_buffer_reusage", Join_Trace, 5 );
    if ( m_BufferReusagePossible )
    {
        if ( this->is_same_key( old_startkeys, m_Startkeys ) &&
             this->is_same_key( old_stopkeys , m_Stopkeys ) )
        {
            SAPDBTRACE_WRITELN( Join_Trace, 5, "reuse previous buffer content" );
            // reset content of buffer
            this->result_cnt() = this->GetBufferedRecordCount();
            m_BDState = e_no_next_record;
            // step virtually one record befor first record
            m_Record = reinterpret_cast<SAPDB_Byte*>( m_RecBuf ) -
                this->rec_offset();
        }
        else
            m_BufferReusagePossible = false;
    }
}
