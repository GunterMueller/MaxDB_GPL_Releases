#ifndef JOIN_NOPACCESSOPERATOR_HPP
#define JOIN_NOPACCESSOPERATOR_HPP
/*!
 * @file  Join_NopAccessOperator.hpp
 * @brief realize an access operator, which always delivers e_no_next_record
 *
 * @author GertG
 * @ingroup Join
 *
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

class SQLMan_Context;

//! operator for strategy strat_no_result
class Join_NopAccessOperator: public Join_AccessOperator {
public:
    //! @name constructor / destructor
    //@{
    //! constructor
    Join_NopAccessOperator(
            SQLMan_Context&   acv,
            const SAPDB_UInt2 tabno,
            const bool        compressed_buf_usage = true,
            SAPDB_Byte*       bufferPtr      = 0,
            const SAPDB_UInt4& buffersize    = BUF8K_MXSP00) :
    Join_AccessOperator(acv, tabno, compressed_buf_usage, &m_BufFake, sizeof(m_BufFake)),
    m_ResCnt(0)
{
    SAPDBTRACE_METHOD_DEBUG( "Join_NopAccessOperator::ctor", Join_Trace, 1 );
}
    //@}

protected:

    virtual void SelectRecords() { m_acv.TransContext().trError_gg00 = e_no_next_record; }

private:

    virtual SAPDB_Int4& result_cnt()      { return m_ResCnt; }
    virtual SAPDB_Int4 result_cnt() const { return 0; }
    virtual SAPDB_Int4 rec_offset() const { return 0; }

    // member variables
    SAPDB_Byte m_BufFake;
    SAPDB_Int4 m_ResCnt;
};

#endif
