#ifndef JOIN_IACCESSOPERATOR_HPP
#define JOIN_IACCESSOPERATOR_HPP
/*!
 * @file  Join_IAccessOperator.hpp
 * @brief realize one table access operator by means of Join_AccessDesc
 *
 * @author MartinKi
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
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "Join/Join_IOperator.hpp"
#include "Join/Join_AccessDesc.hpp"
#include "ggg00.h"

class SQLMan_Context;

//! operator for base table access via key or index
class Join_IAccessOperator: public IOperator {
public:
    //! @name record stream properties
    //@{
    //! return key length of records in stream
    SAPDB_UInt2 GetKeyLength() const { return m_AccessDesc.GetKeyLength(); }

    //! return length of records in stream
    SAPDB_UInt2 GetRecordLength() const { return m_AccessDesc.GetRecordLength(); }
    //@}

    /*! 
     *
     */
    virtual tgg00_BasisError Next( tgg00_Rec*& recptr ) =0;
    /*!
     * @brief Sets the maxmimum number of rows that will read during a
     * single bd-access.
     */
    virtual void SetMaximumRowsReadAtATime(const SAPDB_Int4 maxRowRequest);

    /*!
     * @brief Returns the maxmimum number of rows that will read during a
     * single bd-access.
     */
    virtual SAPDB_Int4 GetMaximumRowsReadAtATime() const;

    /*!
     * @brief Returns true if all qualified records of this table are
     * buffered in memory.
     */
    virtual SAPDB_Bool IsResultFullyBuffered() const { return false; };

    /*!
     * @brief Returns the number of records that are buffered. If not
     * all qualified records could be buffered, 0 is returned.
     */
    virtual SAPDB_UInt4 GetBufferedRecordCount() const { return 0; };

    virtual tgg00_BasisError GetNullRecord( tgg00_Rec*& recptr );

    // member methods
    //! @name constructor(is protected) / destructor
    //@{
    //! destructor
    virtual ~Join_IAccessOperator();
    //@}

protected:
    // member methods
    //! @name constructor / destructor
    //@{
    //! constructor
    Join_IAccessOperator(SQLMan_Context& acv, const SAPDB_UInt2 tabno);

    void invalidate_keys();

    virtual tgg00_BasisError next( tgg00_Rec** records )
    {
        tgg00_Rec *_recptr = 0;

        tgg00_BasisError _b_err = this->Next( _recptr );
        *(records + m_TableNo-1) = _recptr;
        return _b_err;
    }

    virtual tgg00_BasisError get_null_record( tgg00_Rec** records )
    {
        tgg00_Rec *_recptr = 0;

        tgg00_BasisError _b_err = this->GetNullRecord( _recptr );
        *(records + m_TableNo-TAB_OFFS_GG07) = _recptr;
        return _b_err;
    }

    // protected member variables
    Join_AccessDesc         m_AccessDesc;
    SAPDB_Int4              m_maxRowRequest;
    const SAPDB_UInt2       m_TableNo;

private:
    // private member variables
    SAPDB_AutoPtr<tgg00_Rec> m_NullRec; 

};

/*************************************************************************/

inline void Join_IAccessOperator::invalidate_keys()
{
    // initialize keys as invalid key: first key compare --> false
    m_Startkeys.reckey.keyLen_gg00()  = -1;
    m_Startkeys.listkey.keyLen_gg00() = -1;
    m_Stopkeys.reckey.keyLen_gg00()   = -1;
    m_Stopkeys.listkey.keyLen_gg00()  = -1;
}

#endif
