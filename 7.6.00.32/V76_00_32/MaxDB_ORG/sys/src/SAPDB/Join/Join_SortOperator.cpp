/*!
 * \file
 * \brief sort tuple stream
 *
 * \author GertG
 * \ingroup Join
 *
 * \sa Join_SortOperator.hpp
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

#include "SQLManager/SQLMan_Context.hpp"
#include "SQLManager/SQLMan_MessBlock.hpp"
#include "Join/Join_SortOperator.hpp"
#include "Join/Join_InvAccessOperator.hpp"
#include "Join/Join_TableAccessOperator.hpp"
#include "Trace/Trace_Entry.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_HexBuf.hpp"
#include "hbd07.h"
#include "hgg04.h"
#include "heo51.h" //vdebug_break()
#include "hgg01_3.h"
#include "ggg11.h"

/* ******************** PUBLIC MEMBERS ********************* */

Join_SortOperator::Join_SortOperator(
        SQLMan_Context &acv,
        const SAPDB_UInt2 tabno,
        SAPDB_Byte *bufferPtr,
        const SAPDB_UInt4 &buffersize) : 
    Join_AccessOperator(acv, tabno, false, bufferPtr, buffersize), 
    m_AccessOp(0), m_TableSorted(false)
{
    SAPDBTRACE_METHOD_DEBUG( "Join_SortOperator::ctor", Join_Trace, 1 );

    tgg00_BasisError _b_err = e_ok;

    // adjust output description
    // m_AccessDesc.RemoveKey();

    // create temp file
    g04build_temp_tree_id( m_SortFile, m_acv.TransContext() );
    m_SortFile.fileTfnTemp_gg00().becomes( ttfnTempInv_egg00 );
    b01tcreate_file( m_acv.TransContext(), m_SortFile );

    // point to one table strategy to create right access operators
    acv.GetMessBlock().StratPos() = acv.GetMessBlock().StratPos() + 3;
    acv.GetMessBlock().StratCnt() = 1;

    // create AccessOperator
    if ( m_AccessDesc.IsOneTableIndexAccess() )
        m_AccessOp = new (m_acv.GetAllocator())
            Join_InvAccessOperator(
                acv,
                tabno,
                false,
                (SAPDB_Byte*) m_RecBuf,
                m_RecBufSize );
    else
        m_AccessOp = new (m_acv.GetAllocator())
            Join_TableAccessOperator(
                acv,
                tabno,
                false,
                (SAPDB_Byte*) m_RecBuf,
                m_RecBufSize );

    if ( ! m_AccessOp ) 
    {
        _b_err = e_no_more_memory;
        if ( Join_Trace.TracesLevel( 1 ) )
        {
            SAPDBTrace_Stream traceStream( &Join_Trace.GetTracePrinter() );
            traceStream << "Join_SortOperator::Join_SortOperator: access operator creation failed: no more memory";
        }
    }
        
    m_RecLength = m_AccessOp->GetRecordLength();
    if ( 1 == m_RecLength % 2 ) ++m_RecLength; // to allow pointing into buffer

    if ( e_ok != _b_err ) m_acv.ThrowError( _b_err );
}

/* ********************************************************* */
Join_SortOperator::~Join_SortOperator()
{
    // destroy access operator
    destroy( m_AccessOp, m_acv.GetAllocator() );

    // destroy temp file
    if ( NIL_PAGE_NO_GG00 != m_SortFile.fileRoot_gg00() )
    {
        b01destroy_file( m_acv.TransContext(), m_SortFile );
        m_SortFile.fileRoot_gg00() = NIL_PAGE_NO_GG00;
    }
}

/* ********************************************************* */
/*!
 *
 */
tgg00_BasisError Join_SortOperator::Open()
{
    SAPDBTRACE_METHOD_DEBUG( "Join_SortOperator::Open", Join_Trace, 3 );
    tgg00_BasisError _e;
    _e = Join_AccessOperator::Open();
    if ( e_ok == _e && ! m_TableSorted ) 
        _e = this->sort();
    return _e;
}

/* ********************************************************* */
/*!
 * @param[in] startkeys restrict record stream to records with key greater/equal than startkey
 * @param[in] stopkeys restrict record stream to records with key lower/equal than stopkey
 */
tgg00_BasisError Join_SortOperator::Open(
            const Join_TwoKeys& startkeys,
            const Join_TwoKeys& stopkeys )
{
    SAPDBTRACE_METHOD_DEBUG( "Join_SortOperator::Open(startkeys,stopkeys)", Join_Trace, 1 );
    tgg00_BasisError _e;
    _e = Join_AccessOperator::Open( startkeys, stopkeys );
    if ( e_ok == _e && ! m_TableSorted ) 
        _e = this->sort();
    return _e;
}

/* ******************* PROTECTED MEMBERS ******************* */
void Join_SortOperator::SelectRecords()
{
    SAPDBTRACE_METHOD_DEBUG( "Join_SortOperator::SelectRecords", Join_Trace, 1 );

    m_SelectDesc.bd_rec_cnt = m_SelectDesc.bd_fill_len = 0;
    m_SelectDesc.bd_key_check_len = m_Startkeys.reckey.len();

    SAPDBTRACE_WRITELN( Join_Trace, 5, SAPDBTrace_HexBuf(&m_Actualkeys.reckey, cgg_rec_key_offset, m_Actualkeys.reckey.len()) );
    b07cnext_record( m_acv.TransContext(), m_SortFile, 
            m_Actualkeys.reckey, m_SelectDesc, m_TreePos, m_RecBuf );
    SAPDBTRACE_WRITELN( Join_Trace, 5, "BD call: " << SAPDBTrace::BasisError(m_acv.TransContext().trError_gg00) );
    if ( e_key_not_found == m_acv.TransContext().trError_gg00 )
        m_acv.TransContext().trError_gg00 = e_no_next_record;
    if ( e_buffer_limit  == m_acv.TransContext().trError_gg00 )
        m_acv.TransContext().trError_gg00 = e_ok;
}

/* ********************************************************* */
tgg00_BasisError Join_SortOperator::reset()
{
    SAPDBTRACE_METHOD_DEBUG( "Join_SortOperator::reset", Join_Trace, 3 );
    (void) Join_AccessOperator::reset();
    
    m_SelectDesc.bd_key_check_len = 0;
    m_SelectDesc.bd_max_rec_cnt = m_maxRowRequest;
    m_SelectDesc.bd_max_fill_len = m_RecBufSize;
    m_SelectDesc.bd_rec_cnt = 0;
    m_SelectDesc.bd_drop_page = false;
    m_SelectDesc.bd_next = false;
    m_TreePos.tpsPno_gg00 = NIL_PAGE_NO_GG00;
    return e_ok;
}

/* ******************** PRIVATE MEMBERS ******************** */

tgg00_BasisError Join_SortOperator::sort()
{
    SAPDBTRACE_METHOD_DEBUG( "Join_SortOperator::sort", Join_Trace, 3 );

    if ( g01vtrace.vtrAll_gg00 )
    {
        // default vtrace
        tgg11_JoinTabIdTrace _trace;
        _trace.jtHead_gg11.kbMessType_gg11.becomes( m_nil );
        _trace.jtHead_gg11.kbMessType2_gg11.becomes( mm_nil );
        _trace.jtType_gg11.becomes( jntrSort_gg11 );
        _trace.jtTabId_gg11 = m_AccessDesc.MBlock().mb_qual()->mtree().fileTabId_gg00();
        _trace.jtTabno_gg11 = m_TableNo;

        Trace_CommonEntry Entry(
            tgg00_Debug::fromConst(ak_join) /* Layer */,
            tgg00_VtraceType::fromConst(vttAccessOperator_egg00),
            (tsp00_BytePtr) &_trace, 
            sizeof( _trace ) );
    }

    // TODO insert trace output (fast)
    // sort result set
    tgg00_BasisError _b_err = e_ok;
    tgg00_RecPtr _recptr = 0;
    
    SAPDBTRACE_WRITELN( Join_Trace, 5, "m_RecLength : " << m_RecLength << " (" << m_AccessOp->GetRecordLength() << ") " 
            << "\tkeylen : " << m_AccessOp->GetKeyLength() );
    _b_err = m_AccessOp->Open();
    while ( e_ok == _b_err && m_acv.IsOk() &&
            e_ok == ( _b_err = m_AccessOp->Next( _recptr )))
    {
        _recptr->recLen_gg00() = m_RecLength;
        _recptr->recKeyLen_gg00() = m_AccessOp->GetKeyLength();
        _recptr->recVarcolOffset_gg00() = 0;
        _recptr->recVarcolCnt_gg00() = 0;
        
        SAPDBTRACE_IF( Join_Trace, 9, 
        t01buf( td_always, _recptr, 1, 
            _recptr->recLen_gg00() <= 300 ? 
            _recptr->recLen_gg00() : 300 ));
        b07cadd_record( m_acv.TransContext(), m_SortFile, *_recptr );
        _b_err = m_acv.TransContext().trError_gg00;
    }
    m_AccessOp->Close();
   
    if ( e_ok != _b_err && e_no_next_record != _b_err ) 
        return _b_err;

    m_TableSorted = true;
    return e_ok;
}
