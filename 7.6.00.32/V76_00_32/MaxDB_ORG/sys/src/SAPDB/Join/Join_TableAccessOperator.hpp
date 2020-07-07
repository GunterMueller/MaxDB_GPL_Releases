#ifndef JOIN_TABLEACCESSOPERATOR_HPP
#define JOIN_TABLEACCESSOPERATOR_HPP
/*!
 * @file  Join_TableAccessOperator.hpp
 * @brief realize one table access operator by means of Join_AccessDesc
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
#include "gsp00.h"

class SQLMan_Context;

//! operator for base table access via key or index
class Join_TableAccessOperator: public Join_AccessOperator {
public:
    //! @name constructor / destructor
    //@{
    //! constructor
    Join_TableAccessOperator(
            SQLMan_Context&   acv,
            const SAPDB_UInt2 tabno,
            const bool        compressed_buf_usage = true,
            SAPDB_Byte*       bufferPtr      = 0,
            const SAPDB_UInt4& buffersize    = BUF8K_MXSP00);
    //@}

protected:

    virtual tgg00_BasisError reset();
    virtual void SelectRecords();

private:

    virtual SAPDB_Int4& result_cnt() { return m_SelFields.sfp_m_result_cnt(); }
    virtual SAPDB_Int4 result_cnt() const { return m_SelFields.sfp_m_result_cnt(); }
    virtual SAPDB_Int4 rec_offset() const { return m_SelFields.sfp_resrec_maxlen(); }

    // member variables
    tgg00_SelectFieldsParam m_SelFields;
    tgg00_BdInvSet          m_RangeSet;
    
};

#endif
