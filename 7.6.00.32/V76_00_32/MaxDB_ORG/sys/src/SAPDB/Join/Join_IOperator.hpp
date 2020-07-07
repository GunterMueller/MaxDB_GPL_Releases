#ifndef JOIN_IOPERATOR_HPP
#define JOIN_IOPERATOR_HPP
/*!
 * @file
 * @brief generic interface for record stream operator
 *
 * @author GertG
 * @ingroup Join
 *
 * @sa Join_AccessOperator.hpp, Join_JoinOperator.hpp
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
#include "ggg00.h"

class SQLMan_Context;

//! generic interface for record stream operator
class IOperator 
{
public:
    //! @name constructor / destructor
    //@{
    //! constructor
    /*! 
     * @param[in] acv [in] global context
     */
    IOperator(SQLMan_Context& acv): m_acv(acv), m_opened(false), m_opencnt(0) 
    {
        m_Startkeys.reckey.len() = m_Startkeys.listkey.len() = 0;
        m_Stopkeys.reckey.len()  = m_Stopkeys.listkey.len()  = 0;
    }
    //! destructor
    virtual ~IOperator() {};
    //@}

    //! @name record stream manipulation functions
    //@{
    //! open operator with keys given by strategy information
    virtual tgg00_BasisError Open() { m_opened = true; m_opencnt++; return e_ok; }

    //! open stream operator and limit stream with start/stop key
    /*! 
     * @param[in] startkeys restrict record stream to records with key greater/equal than startkey
     * @param[in] stopkeys restrict record stream to records with key lower/equal than stopkey
     */
    virtual tgg00_BasisError Open(
            const Join_TwoKeys& startkeys, 
            const Join_TwoKeys& stopkeys )
    { 
        m_opened = true; 
        m_opencnt++; 
        m_Startkeys = startkeys; m_Stopkeys  = stopkeys;
        return e_ok;
    }

    //! get next record from stream
    /*! 
     * @param[in,out] record pointer to memory should be filled with record
     * @return information code [e_ok, e_no_next_record] / error code [ ... ]
     */
    virtual tgg00_BasisError Next( tgg00_Rec*& recptr )   =0;

    //! close operator
    virtual void Close() { m_opened = false; }
    //@}

    //! @name operator/record properties
    //@{
    //! determine if operator is open
    SAPDB_Bool IsOpened() const { return m_opened; }
    //! determine if operator is open
    SAPDB_UInt4 GetOpenCnt() const { return m_opencnt; }
    //! return key length of records in stream
    virtual SAPDB_UInt2 GetKeyLength() const =0;
    //! return length of records in stream
    virtual SAPDB_UInt2 GetRecordLength() const =0;

    virtual tgg00_BasisError GetNullRecord( tgg00_Rec*& recptr ) =0;
    //@}

protected:
    friend class Join_JoinOperator2;
    
    //! get next record into record array
    virtual tgg00_BasisError next( tgg00_Rec* *records )  =0;
    //! get NULL record into record array
    virtual tgg00_BasisError get_null_record( tgg00_Rec** records ) =0;

    //! reference to global context
    SQLMan_Context&        m_acv;
    //! startkeys which restrict record stream to records with key greater/equal than startkey
    Join_TwoKeys           m_Startkeys;
    //! stopkeys which restrict record stream to records with key lower/equal than stopkey
    Join_TwoKeys           m_Stopkeys;

private:
    SAPDB_Bool              m_opened;
    SAPDB_UInt4             m_opencnt; //!< amount of Open() for object, parallel opens not possible!
};

#endif
