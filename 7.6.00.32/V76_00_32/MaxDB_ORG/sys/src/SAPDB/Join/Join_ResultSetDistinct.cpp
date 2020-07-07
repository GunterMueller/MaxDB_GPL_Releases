/*!
 * @file
 * @brief implements Join_ResultSetDistinct
 *
 * @author GertG
 * @ingroup Join
 *
 * @sa Join_ResultSetDistinct.hpp
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
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "Join/Join_ResultSetDistinct.hpp"
#include "hbd07.h"
#include "hkb50_2.h"
#include "hsp30.h"

/* ******************** PUBLIC MEMBERS ********************* */
/*!
 * @param[in] record record to be added
 * @return information code [e_ok, e_no_next_record] / error code [ ... ]
 */
tgg00_BasisError Join_ResultSetDistinct::Add( tgg00_Rec &record )
{
    SAPDBTRACE_METHOD_DEBUG( "Join_ResultSetDistinct::Add", Join_Trace, 1 );

    if ( m_ResultCnt < m_MaxResultCnt )
    {
        // original but don't understandable
        //SAPDB_Int4 _pos_to_hash = record.recKeyLen_gg00() + cgg_rec_key_offset + 2;
        SAPDB_Int4 _pos_to_hash = record.recKeyLen_gg00() + cgg_rec_key_offset + 1;
        SAPDB_Int4 _len_to_hash = record.recLen_gg00() - record.recKeyLen_gg00() -
            cgg_rec_key_offset;
        k50hash( &record, _pos_to_hash, _len_to_hash, &record,
              cgg_rec_key_offset + record.recKeyLen_gg00() - ( RESCNT_MXGG04 +
              HASHVAL_MXGG04 ) + 1 );
        
        tgg00_Lkey _search_key;

        _search_key.len() = record.recKeyLen_gg00() - RESCNT_MXGG04;
        SAPDB_MemCopyNoCheck( &_search_key.k(), 
                (SAPDB_Byte *)&record.buf() + cgg_rec_key_offset,
                _search_key.len() );

        tgg00_BdSetResultRecord _set_result;
        tgg00_Rec               _buffer;
        tgg00_FilePos           _treepos;
        _set_result.bd_key_check_len = _search_key.len();
        _set_result.bd_max_rec_cnt   = MAX_INT2_SP00;
        _set_result.bd_max_fill_len  = sizeof( _buffer );
        _set_result.bd_next          = false;
        _set_result.bd_drop_page     = false;
        _treepos.tpsPno_gg00         = NIL_PAGE_NO_GG00;
        m_acv.TransContext().trError_gg00 = e_buffer_limit;
        bool _rec_found = false;

        while ( !_rec_found && e_buffer_limit == m_acv.TransContext().trError_gg00 )
        {
            b07cnext_record( m_acv.TransContext(), m_ResultTree, 
                    _search_key, _set_result, _treepos, &_buffer );
            if ( m_acv.TransContext().trRteCommPtr_gg00->to_cancel )
                m_acv.TransContext().trError_gg00 = e_cancelled;
            if ( e_key_not_found == m_acv.TransContext().trError_gg00 )
                m_acv.TransContext().trError_gg00 = e_ok;
            if ( e_ok == m_acv.TransContext().trError_gg00 || 
                 e_buffer_limit == m_acv.TransContext().trError_gg00 )
            {
                SAPDB_Int4 _recpos = 0;
                while ( !_rec_found && _recpos < _set_result.bd_fill_len )
                {
                    SAPDB_Int4 _pos = record.recKeyLen_gg00() + 
                        cgg_rec_key_offset + 1;
                    SAPDB_Int4 _comp_len = record.recLen_gg00() - _pos + 1;
                    tsp00_LcompResult _lc;

                    s30cmp( &record, _pos, _comp_len, 
                            &_buffer, _recpos + _pos, _comp_len, _lc );
                    _rec_found = ( l_equal == _lc );
                    _recpos += record.recLen_gg00();
                }
            }
        }
        if ( e_no_next_record == m_acv.TransContext().trError_gg00 || 
             e_buffer_limit == m_acv.TransContext().trError_gg00 )
            m_acv.TransContext().trError_gg00 = e_ok;
        
        if ( ! _rec_found )
            return add( record );
        else
            return e_ok;
    }
    return e_no_next_record;
}

/* ******************* PROTECTED MEMBERS ******************* */


/* ******************** PRIVATE MEMBERS ******************** */

