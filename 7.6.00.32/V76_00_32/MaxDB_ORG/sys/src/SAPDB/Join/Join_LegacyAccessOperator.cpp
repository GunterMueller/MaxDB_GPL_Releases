/*!
 * \file
 * \brief create result set with legacy operations
 *
 * \author GertG
 * \ingroup Join
 *
 * \sa Join_LegacyAccessOperator.hpp
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
#include "Join/Join_LegacyAccessOperator.hpp"
#include "Join/Join_InvAccessOperator.hpp"
#include "Join/Join_TableAccessOperator.hpp"
#include "Trace/Trace_Entry.hpp"
#include "hbd01.h"
#include "hbd07.h"
#include "hak41.h"
#include "hgg04.h"
#include "heo51.h" //vdebug_break()
#include "hgg01_3.h"
#include "ggg11.h"
#include "hkb05.h"
#include "hak101.h"
#include "hbd998.h"

/* ******************** PUBLIC MEMBERS ********************* */

Join_LegacyAccessOperator::Join_LegacyAccessOperator(
        SQLMan_Context &acv,
        const SAPDB_UInt2 tabno,
        SAPDB_Byte *bufferPtr,
        const SAPDB_UInt4 &buffersize) : 
    Join_AccessOperator(acv, tabno, false, bufferPtr, buffersize), 
    m_ResultCreated(false)
{
    SAPDBTRACE_METHOD_DEBUG( "Join_LegacyAccessOperator::ctor", Join_Trace, 1 );

    // create temp file
    g04build_temp_tree_id( m_ResultFile, m_acv.TransContext() );
    m_ResultFile.fileTfnTemp_gg00().becomes( ttfnJoinResult_egg00 );
    b01tcreate_file( m_acv.TransContext(), m_ResultFile );
    if  ( e_ok != m_acv.TransContext().trError_gg00 )
        m_acv.ThrowError( m_acv.TransContext().trError_gg00 );
    m_AccessDesc.GetOneTableStrategyPtr()->str_result_id = m_ResultFile;
}

/* ********************************************************* */
Join_LegacyAccessOperator::~Join_LegacyAccessOperator()
{
    SAPDBTRACE_METHOD_DEBUG( "Join_LegacyAccessOperator::dtor", Join_Trace, 1 );

    // destroy result file file
    b01destroy_file( m_acv.TransContext(), m_ResultFile );
}

/* ********************************************************* */
/*!
 *
 */
tgg00_BasisError Join_LegacyAccessOperator::Open()
{
    SAPDBTRACE_METHOD_DEBUG( "Join_LegacyAccessOperator::Open", Join_Trace, 3 );
    tgg00_BasisError _e;
    _e = Join_AccessOperator::Open();
    if ( e_ok == _e && ! m_ResultCreated ) 
        _e = this->legacy_access();
    return _e;
}

/* ********************************************************* */
/*!
 * @param[in] startkeys restrict record stream to records with key greater/equal than startkey
 * @param[in] stopkeys restrict record stream to records with key lower/equal than stopkey
 */
tgg00_BasisError Join_LegacyAccessOperator::Open(
            const Join_TwoKeys& startkeys,
            const Join_TwoKeys& stopkeys )
{
    SAPDBTRACE_METHOD_DEBUG( "Join_LegacyAccessOperator::Open(startkeys,stopkeys)", Join_Trace, 1 );
    // called if legacy access operator do also sorting of result
    tgg00_BasisError _e;
    _e = Join_AccessOperator::Open( startkeys, stopkeys );
    if ( e_ok == _e && ! m_ResultCreated ) 
        _e = this->legacy_access();
    return _e;
}

/* ******************* PROTECTED MEMBERS ******************* */
void Join_LegacyAccessOperator::SelectRecords()
{
    SAPDBTRACE_METHOD_DEBUG( "Join_LegacyAccessOperator::SelectRecords", Join_Trace, 1 );

    m_SelectDesc.bd_rec_cnt = m_SelectDesc.bd_fill_len = 0;
    m_SelectDesc.bd_key_check_len = m_Startkeys.reckey.len();

    b07cnext_record( m_acv.TransContext(), m_ResultFile,
            m_Actualkeys.reckey, m_SelectDesc, m_TreePos, m_RecBuf );
    SAPDBTRACE_WRITELN( Join_Trace, 5, "BD call: " << SAPDBTrace::BasisError(m_acv.TransContext().trError_gg00) );
    if ( e_key_not_found == m_acv.TransContext().trError_gg00 )
        m_acv.TransContext().trError_gg00 = e_no_next_record;
    if ( e_buffer_limit  == m_acv.TransContext().trError_gg00 )
        m_acv.TransContext().trError_gg00 = e_ok;
}

/* ********************************************************* */
tgg00_BasisError Join_LegacyAccessOperator::reset()
{
    SAPDBTRACE_METHOD_DEBUG( "Join_LegacyAccessOperator::reset", Join_Trace, 3 );
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

tgg00_BasisError Join_LegacyAccessOperator::legacy_access()
{
    SAPDBTRACE_METHOD_DEBUG( "Join_LegacyAccessOperator::legacy_access", Join_Trace, 3 );

    if ( g01vtrace.vtrAll_gg00 )
    {
        // default vtrace
        tgg11_JoinTabIdTrace _trace;
        _trace.jtHead_gg11.kbMessType_gg11.becomes( m_nil );
        _trace.jtHead_gg11.kbMessType2_gg11.becomes( mm_nil );
        _trace.jtType_gg11.becomes( jntrLegacy_gg11 );
        _trace.jtTabId_gg11 = m_AccessDesc.MBlock().mb_qual()->mtree().fileTabId_gg00();
        _trace.jtTabno_gg11 = m_TableNo;

        Trace_CommonEntry Entry(
            tgg00_Debug::fromConst(ak_join) /* Layer */,
            tgg00_VtraceType::fromConst(vttAccessOperator_egg00),
            (tsp00_BytePtr) &_trace, 
            sizeof( _trace ) );
    }

    tgg00_MessType2 _tmp = m_AccessDesc.MBlock().mb_type2();
    m_AccessDesc.MBlock().mb_type2().becomes( mm_nil );
    

    // prepare legacy access
    m_AccessDesc.PrepareLegacyAccess();
    // do this action after PrepareLegacyAccess() is called
    m_RecOffset = m_AccessDesc.OneTableStrategy()->str_rec_len;
    
    tgg00_BasisError _b_err = e_ok;

    if ( strat_catalog == m_AccessDesc.OneTableStrategy()->str_strategy )
    {
        // transfer m_AccessDesc.MBlock() to acv
        tgg00_MessBlock _a_mblock = m_acv.a_mblock;
        m_acv.a_mblock = m_AccessDesc.MBlock();
        // for correctness we have to invalidate m_AccessDesc.MBlock()
        // but who cares
        // a41execute_show() manipulate mblock.mb_data_len, but we need this in
        // right outer join handling (NULL value)!!
        tgg00_MessBlock _tmp = m_AccessDesc.MBlock();
        a41execute_show( m_acv, _tmp );

        m_acv.a_mblock = _a_mblock;
    }
    else
    {
        k05functions( m_AccessDesc.MBlock() );
    }
    _b_err = m_acv.TransContext().trError_gg00;

    m_AccessDesc.MBlock().mb_type2().becomes( _tmp );
   
    // we want to read on file so we need the physical name
    a101_GetTempFileInstance( m_acv.TransContext(), m_ResultFile );
    
    m_ResultCreated = true;
    return _b_err;
}
