#ifndef JOIN_INVACCESSOPERATOREX_HPP
#define JOIN_INVACCESSOPERATOREX_HPP
/*!
 * @file  Join_InvAccessOperatorEx.hpp
 * @brief realize one table access operator by means of Join_InvSelectIterator
 *
 * @author GertG
 * @ingroup Join
 *
 * @sa Join_IndexAccessOperator.hpp
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
#include "Join/Join_IndexAccessOperator.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "ggg00.h"
#include "hgg01_3.h"

class Join_InvSelectIterator; //<! forward declaration
class SQLMan_Context;

//! operator for base table access via key or index
class Join_InvAccessOperatorEx : public Join_IndexAccessOperator {
public:
    //! @name constructor / destructor
    //@{
    //! constructor
    Join_InvAccessOperatorEx(
        SQLMan_Context&       acv,
        const SAPDB_UInt2     tabno,
        const bool            compressed_buf_usage = true,
        SAPDB_Byte*           bufferPtr     = 0,
        const SAPDB_UInt4&    buffersize    = BUF8K_MXSP00,
        const SAPDB_Int4&     maxServerTasks = gg01optimize_parallel_server);

    //! destructor
    virtual ~Join_InvAccessOperatorEx();
    //@}

    //! open operator with keys given by strategy information
    virtual tgg00_BasisError Open();

    //! open stream operator and limit stream with start/stop key
    virtual tgg00_BasisError Open(
        const Join_TwoKeys& startkeys,
        const Join_TwoKeys& stopkeys );

    //! close operator
    virtual void Close();

protected:

    // member methods
    virtual void SelectRecords();

    // member variables
    Join_InvSelectIterator* m_pInvIterator;
};

#endif
