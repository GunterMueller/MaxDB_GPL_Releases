#ifndef JOIN_SORTOPERATOR_HPP
#define JOIN_SORTOPERATOR_HPP
/*!
 * @file
 * @brief sort tuple stream
 *
 * @author GertG
 * @ingroup Join
 *
 * @sa Join_IOperator.hpp
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
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "Join/Join_Types.hpp"
#include "Join/Join_Trace.hpp"
#include "Join/Join_AccessOperator.hpp"
#include "ggg00.h"
#include "hbd07.h"

class SQLMan_Context;

//! generic interface for record stream operator
class Join_SortOperator : public Join_AccessOperator
{
public:
    //! @name constructor / destructor
    //@{
    //! constructor
    Join_SortOperator(
        SQLMan_Context          &acv,
        const SAPDB_UInt2       tabno,
        SAPDB_Byte              *bufferPtr = 0,
        const SAPDB_UInt4       &buffersize = BUF8K_MXSP00);

    //! destructor
    virtual ~Join_SortOperator();
    //@}

    //! open operator with keys given by strategy information
    virtual tgg00_BasisError Open();

    //! open stream operator and limit stream with start/stop key
    virtual tgg00_BasisError Open(
            const Join_TwoKeys& startkeys,
            const Join_TwoKeys& stopkeys );

    //! @name record stream properties
    //@{
    virtual tgg00_BasisError GetNullRecord( tgg00_Rec*& recptr )
    { return m_AccessOp->GetNullRecord( recptr ); }
    //@}
protected:
    virtual tgg00_BasisError reset();
    virtual void SelectRecords();

private:
    virtual SAPDB_Int4& result_cnt() { return m_SelectDesc.bd_rec_cnt; }
    virtual SAPDB_Int4 result_cnt() const { return m_SelectDesc.bd_rec_cnt; }
    virtual SAPDB_Int4 rec_offset() const { return m_RecLength; }
    
    tgg00_BasisError sort();
    
    // member variables
    Join_IAccessOperator *m_AccessOp;
    tgg00_FileId m_SortFile;
    tgg00_FilePos m_TreePos;
    tgg00_BdSetResultRecord m_SelectDesc;
    SAPDB_Int4 m_RecLength;
    bool m_TableSorted;
};

#endif
