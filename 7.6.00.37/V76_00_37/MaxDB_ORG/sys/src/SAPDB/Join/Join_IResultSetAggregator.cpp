/*!
 * @file
 * @brief implements Join_IResultSetAggregator
 *
 * @author GertG
 * @ingroup Join
 *
 * @sa Join_IResultSetAggregator.hpp
 */
/*

    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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
#include "Join/Join_IResultSetAggregator.hpp"
#include "SAPDBCommon/SAPDB_RangeCode.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "Join/Join_Aggregation.hpp"
#include "hak101.h"
#include "hbd07.h"
#include "hkb721.h"
#include "hbd01.h"
#include "hgg01_3.h"
#include "hta01.h"
#include "hsp51.h"
#include "hbd998.h"

/* ******************** PUBLIC MEMBERS ********************* */

/*!
 * 
 */
Join_IResultSetAggregator::~Join_IResultSetAggregator()
{
    SAPDBTRACE_METHOD_DEBUG( "Join_IResultSetAggregator::dtor", Join_Trace, 5 );

    if ( ! m_DistinctFileRoot.IsEmpty() )
    {
        // destroy distinct files
        a101_DestroyGroupedTempFiles (
            m_acv.TransContext(),
            ttfnDistinct_egg00,
            -1,
            -1,
            m_DistinctFileId.fileName_gg00());
    }
    
}


/* ******************* PROTECTED MEMBERS ******************* */

/* ******************** PRIVATE MEMBERS ******************** */
/*!
 *
 */
void Join_IResultSetAggregator::post_processing()
{
    SAPDBTRACE_METHOD_DEBUG( "Join_IResultSetAggregator::post_processing", Join_Trace, 3 );

    
    tgg00_BasisError _b_err = e_ok;
    SAPDB_UInt4 _cnt_repl = 0;
    SAPDB_UInt4 _res_cnt = 0;
    tgg00_Rec _buffer;
    
    SAPDBTRACE_WRITELN( Join_Trace, 3, "m_RecordCounterPositions.IsEmpty(): " << m_RecordCounterPositions.IsEmpty() << 
            ", m_CallPostProcessing: " << (int) m_CallPostProcessing << 
            ", m_DistinctFileRoot.GetSize(): " << m_DistinctFileRoot.GetSize() );

    Join_AggregationContext _context = 
        Join_AggregationContext( m_acv, *m_AggregationDesc, m_DistinctFileId, m_DistinctFileRoot );

    this->init_result_scan();
    do {
        SAPDBTRACE_IF( Join_Trace, 5, SAPDB_MemFillNoCheck( &_buffer, ':', sizeof( _buffer ) ) );
        _b_err = this->fetch_next_result( _buffer );
        SAPDBTRACE_IF( Join_Trace, 9, 
        t01buf( td_always, &_buffer, 1, _buffer.recLen_gg00() <= 300 ? _buffer.recLen_gg00() : 300 ));
        
        if ( m_acv.TransContext().trRteCommPtr_gg00->to_cancel )
            _b_err = e_cancelled;

        if (e_ok == _b_err) 
        {
            _context.m_grouprec_changed = false;
            ++_res_cnt;
            
            _b_err = Join_Aggregation::post_processing( _context, _buffer, 
                    _res_cnt, m_LaterOutputPos, m_RecordCounterPositions, m_CallPostProcessing );
            
            SAPDBTRACE_IF( Join_Trace, 5, 
            t01buf( td_always, &_buffer, 1, _buffer.recLen_gg00() <= 300 ? _buffer.recLen_gg00() : 300 ));

            if ( _context.m_grouprec_changed && ( e_ok == _b_err ) && 
               ( e_ok == (_b_err = update_result( _buffer)) )) 
                ++_cnt_repl;
        }
    } while ( e_ok == _b_err && e_ok == m_acv.TransContext().trError_gg00 );
    
    if ( e_no_next_record == m_acv.TransContext().trError_gg00 )
        m_acv.TransContext().trError_gg00 = e_ok;
    b73cmds_count( iins_sorts_rows_ins, _cnt_repl );
}

