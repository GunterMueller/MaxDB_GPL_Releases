#ifndef JOIN_RESULTSETHASHINSERTER_HPP
#define JOIN_RESULTSETHASHINSERTER_HPP
/*!
 * @file
 * @brief implements interface Join_IResultSetInserter; the operator inserts a
 * hash value in key to get a fingerprint of record
 *
 * @author GertG
 * @ingroup Join
 *
 * @sa Join_Execute.cpp, Join_IResultSetInserter.hpp
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
#include "hkb50_2.h"

class SQLMan_Context;

//! implements interface Join_IResultSetInserter; the operator inserts a hash value in key to get a fingerprint of record
class Join_ResultSetHashInserter : public Join_ResultSetInserter {
public:
    //! @name constructor / destructor
    //@{
    //! constructor
    /*! 
     * @param[in] acv global context
     * @param[in] rowno_pos positions of ROWNO columns
     * @param[in] result_id name of result file
     * @param[in] rescnt amount of records already in result set
     * @param[in] maxrescnt maximal amount of records in result set
     */
    Join_ResultSetHashInserter( 
            SQLMan_Context&                 acv, 
            Join_RownoPositions&            rowno_pos,
            tgg00_FileId&                   result_id, 
            const SAPDB_Int4&               rescnt,
            const SAPDB_Int4&               maxrescnt ) :
    Join_ResultSetInserter( acv, rowno_pos, result_id, rescnt, maxrescnt )
    {
        SAPDBTRACE_METHOD_DEBUG( "Join_ResultSetHashInserter::ctor", Join_Trace, 1 );
    }
    
    //! destructor
    virtual ~Join_ResultSetHashInserter(){}
    //@}

    //! @name manipulation of result set
    //@{
    //! add record to result set
    /*!
     * @param[in] record record to be added
     * @return information code [e_ok, e_no_next_record] / error code [ ... ]
     */
    virtual tgg00_BasisError Add( tgg00_Rec& record )
    {
        SAPDBTRACE_METHOD_DEBUG( "Join_ResultSetHashInserter::Add", Join_Trace, 1 );
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

            return add( record );
        }
        return e_no_next_record;
    }
    //!@}
};

#endif // JOIN_RESULTSETHASHINSERTER_HPP
