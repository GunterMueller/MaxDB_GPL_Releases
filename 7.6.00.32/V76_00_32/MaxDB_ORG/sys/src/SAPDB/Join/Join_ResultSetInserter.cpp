/*!
 * @file
 * @brief implements Join_ResultSetInserter
 *
 * @author GertG
 * @ingroup Join
 *
 * @sa Join_ResultSetInserter.hpp
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
#include "Join/Join_ResultSetInserter.hpp"
#include "hbd07.h"
#include "hsp41.h"

/* ******************** PUBLIC MEMBERS ********************* */
/*!
 *
 */
Join_ResultSetInserter::~Join_ResultSetInserter()
{
    SAPDBTRACE_METHOD_DEBUG( "Join_ResultSetInserter::dtor", Join_Trace, 1 );
    if ( ! m_RecordCounterPositions.IsEmpty() )
    {
        tgg00_BasisError _b_err;

        _b_err = write_rowno();
        if ( e_ok != _b_err ) m_acv.ThrowError( _b_err );
    }
}

/* ******************* PROTECTED MEMBERS ******************* */


/* ******************** PRIVATE MEMBERS ******************** */
/*!
 *
 */
tgg00_BasisError Join_ResultSetInserter::write_rowno()
{
    SAPDBTRACE_METHOD_DEBUG( "Join_ResultSetInserter::write_rowno", Join_Trace, 3 );
    // loop over m_ResultTree and put ROWNO
    tgg00_BdSetResultRecord _set_result;
    _set_result.bd_key_check_len = 0;
    _set_result.bd_max_rec_cnt   = 1;
    _set_result.bd_max_fill_len  = BUF_MXSP00;
    _set_result.bd_next          = true;
    _set_result.bd_drop_page     = false;

    tsp00_NumError _ret;
    tgg00_Lkey _hkey;
    _hkey.len() = 0;

    tgg00_FilePos _treepos;
    _treepos.tpsPno_gg00 = NIL_PAGE_NO_GG00;

    SAPDB_Int4 _rescnt = 0;
    tgg00_Rec  _record;
    Container_Vector<SAPDB_Int4>::Iterator _iter;

    do 
    {
        b07cnext_record( m_acv.TransContext(), m_ResultTree, _hkey,
              _set_result, _treepos, &_record );
        if ( m_acv.TransContext().trRteCommPtr_gg00->to_cancel )
            m_acv.TransContext().trError_gg00 = e_cancelled;
        if ( e_key_not_found == m_acv.TransContext().trError_gg00 )
            m_acv.TransContext().trError_gg00 = e_ok;
        if ( e_ok == m_acv.TransContext().trError_gg00 )
        {
            ++_rescnt;
            for ( _iter = m_RecordCounterPositions.Begin(); _iter != m_RecordCounterPositions.End(); ++_iter) 
            {
                _record.buf()[ *_iter - 1 ] = csp_defined_byte;
                s41plint( _record.buf(), *_iter + 1,
                      csp_resnum_deflen, 0, _rescnt, _ret );
            }
            b07ctrepl_record( m_acv.TransContext(), m_ResultTree, _treepos, _record );
        }
    } 
    while ( e_ok == m_acv.TransContext().trError_gg00 );
    
    if ( e_no_next_record == m_acv.TransContext().trError_gg00 )
        m_acv.TransContext().trError_gg00 = e_ok;

    b73cmds_count( iins_sorts_rows_ins, this->ResultCount() );
    return m_acv.TransContext().trError_gg00;
}


