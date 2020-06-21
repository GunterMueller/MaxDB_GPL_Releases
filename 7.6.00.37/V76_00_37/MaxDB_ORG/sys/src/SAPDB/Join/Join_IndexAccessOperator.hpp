#ifndef JOIN_INDEXACCESSOPERATOR_HPP
#define JOIN_INDEXACCESSOPERATOR_HPP
/*!
 * @file  Join_IndexAccessOperator.hpp
 * @brief realize one table access operator by means of an index
 *
 * @author GertG
 * @ingroup Join
 *
 * @sa Join_AccessDesc.hpp
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
#include "Join/Join_Types.hpp"
#include "Join/Join_AccessOperator.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "ggg00.h"
#include "hgg04.h"
#include "hbd01.h"

class SQLMan_Context;

//! operator for base table access via key or index
class Join_IndexAccessOperator : public Join_AccessOperator {
public:
    //! @name constructor / destructor
    //@{
    //! constructor
    Join_IndexAccessOperator(
        SQLMan_Context&       acv,
        const SAPDB_UInt2     tabno,
        const bool            compressed_buf_usage,
        SAPDB_Byte*           bufferPtr,
        const SAPDB_UInt4&    buffersize)
        :
        Join_AccessOperator(acv, tabno, compressed_buf_usage, 
                bufferPtr, buffersize)
    { }
    //@}

protected:

    // member methods
    virtual tgg00_BasisError reset()
    {
        SAPDBTRACE_METHOD_DEBUG( "Join_IndexAccessOperator::reset", Join_Trace, 3 );

        tgg00_BasisError _b_err = e_ok;

        (void) Join_AccessOperator::reset();

        m_RangeSet.clear();

        if ( m_Startkeys.reckey.len() != 0 )
            m_RangeSet.addElement( primary_start );

        if ( m_Stopkeys.reckey.len() != 0
             && m_Stopkeys.reckey.k()[0] != csp_undef_byte )
            m_RangeSet.addElement( primary_stop );

        // initialize select fields parameter
        g04init_select_fields( m_SelFields, 
                &m_AccessDesc.MBlock().mb_data()->mbp_buf(), 
                m_AccessDesc.MBlock().mb_data_size(), 
                m_AccessDesc.MBlock().mb_work_st(), 
                m_AccessDesc.MBlock().mb_work_st_max(), 
                m_AccessDesc.MBlock().mb_workbuf(), 
                m_AccessDesc.MBlock().mb_workbuf_size(),
                m_AccessDesc.SQLMode(),
                &m_AccessDesc.MBlock().mb_fieldlists());
        // siehe vkb720 kb720sel_first_part()
        m_SelFields.sfp_bd_mess_type().becomes( m_select );
        m_SelFields.sfp_bd_mess2_type().becomes( mm_first );
        m_SelFields.sfp_result_wanted() = true;
        m_SelFields.sfp_filler2()       = false;
        m_SelFields.sfp_bd_use_stopkey()= m_RangeSet.includes( primary_stop ); 
        m_SelFields.sfp_bd_inv_only().becomes( m_AccessDesc.Strategy()->str_qual_kind );
        m_SelFields.sfp_check_for_result() = false; // siehe k75_fetch
        m_SelFields.sfp_rows_read() = 0;
        m_SelFields.sfp_rows_qual() = 0;
        m_SelFields.sfp_act_cntresult() = 0;
        m_SelFields.sfp_m_result_cnt() = 0;
        m_SelFields.sfp_acv_addr() = reinterpret_cast<tsp00_Addr>( &m_acv );
        if ( m_CompressedBufferUsage )
        {
            // we overlap records with cgg_rec_key_offset
            // (because of sfp_resrec_maxlen = reclen without cgg_rec_key_offset)
            m_SelFields.sfp_resrec_maxlen() = GetRecordLength() - cgg_rec_key_offset;
        }
        else
            // we can write record header into buffer
            m_SelFields.sfp_resrec_maxlen() = GetRecordLength();

        if ( 1 == m_SelFields.sfp_resrec_maxlen() % 2 ) ++m_SelFields.sfp_resrec_maxlen();
        SAPDBERR_ASSERT_STATE( m_SelFields.sfp_resrec_maxlen() <= (SAPDB_Int4) m_RecBufSize );
        
        // look k75_fetch for mm_first, mm_next
        if ( m_Stopkeys.listkey.k()[0] != csp_undef_byte )
            m_RangeSet.addElement( secondary_stop );
        else if ( m_Stopkeys.listkey.len() > 1
                  && m_Stopkeys.listkey.k()[1] != csp_undef_byte )
            m_RangeSet.addElement( secondary_stop );

        b01vstate_fileversion( m_acv.TransContext(),
                               m_AccessDesc.FileIDs().file_id );
        _b_err = m_acv.TransContext().trError_gg00;

        return _b_err;
    }

    // member variables
    tgg00_SelectFieldsParam m_SelFields;
    tgg00_BdInvSet          m_RangeSet;
    
private:

    virtual SAPDB_Int4& result_cnt() { return m_SelFields.sfp_m_result_cnt(); }
    virtual SAPDB_Int4 result_cnt() const { return m_SelFields.sfp_m_result_cnt(); }
    virtual SAPDB_Int4 rec_offset() const { return m_SelFields.sfp_resrec_maxlen(); }
};

#endif
