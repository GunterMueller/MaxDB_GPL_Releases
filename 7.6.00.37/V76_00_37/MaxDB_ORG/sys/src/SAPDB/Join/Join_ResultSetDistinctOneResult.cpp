/*!
 * @file
 * @brief implements Join_ResultSetDistinctOneResult
 *
 * @author GertG
 * @ingroup Join
 *
 * @sa Join_ResultSetDistinctOneResult.hpp
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
#include "Join/Join_ResultSetDistinctOneResult.hpp"
#include "hkb50_2.h"

/* ******************** PUBLIC MEMBERS ********************* */
/*!
 * @param[in] record record to be added
 * @return information code [e_ok, e_no_next_record] / error code [ ... ]
 */
tgg00_BasisError Join_ResultSetDistinctOneResult::Add( tgg00_Rec &record )
{
    SAPDBTRACE_METHOD_DEBUG( "Join_ResultSetDistinctOneResult::Add", Join_Trace, 1 );

    SAPDB_Int4 _pos_to_hash = record.recKeyLen_gg00() + cgg_rec_key_offset + 1;
    SAPDB_Int4 _len_to_hash = record.recLen_gg00() - record.recKeyLen_gg00() -
        cgg_rec_key_offset;

    k50hash( &record, _pos_to_hash, _len_to_hash, &record,
          cgg_rec_key_offset + record.recKeyLen_gg00() - ( RESCNT_MXGG04 +
          HASHVAL_MXGG04 ) + 1 );

    if ( 0 == m_ResultCnt )
    {
        return Join_ResultSetOneResult::Add( record );
    }
    else
    {
        if ( memcmp( &record, 
                     m_RecBuf.GetPtr(), 
                     cgg_rec_key_offset + record.recKeyLen_gg00() - RESCNT_MXGG04 ) // key
            ||  
            memcmp( (SAPDB_Byte*)&record + cgg_rec_key_offset + record.recKeyLen_gg00(), 
                    (SAPDB_Byte*)m_RecBuf.GetPtr() + cgg_rec_key_offset + record.recKeyLen_gg00(), 
                    record.recLen_gg00() - record.recKeyLen_gg00() - cgg_rec_key_offset ) ) // info part
        {
            return Join_ResultSetOneResult::Add( record );
        }
        return e_ok;
    }
}

/* ******************* PROTECTED MEMBERS ******************* */


/* ******************** PRIVATE MEMBERS ******************** */

