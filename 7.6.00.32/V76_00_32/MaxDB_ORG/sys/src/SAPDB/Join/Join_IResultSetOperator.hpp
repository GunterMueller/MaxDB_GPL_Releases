#ifndef JOIN_IRESULTSETOPERATOR_HPP
#define JOIN_IRESULTSETOPERATOR_HPP
/*!
 * @file
 * @brief generic interface for join result handling
 *
 * @author GertG
 * @ingroup Join
 *
 * @sa Join_Execute.cpp
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
#include "SQLManager/SQLMan_Context.hpp"
#include "Join/Join_Trace.hpp"
#include "Join/Join_Types.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "Container/Container_Vector.hpp"
#include "ggg00.h"
#include "hgg01_3.h"
#include "hsp20.h"
#include "hta01.h"
#include "hkb721.h"

//! generic interface for join result handling
class Join_IResultSetOperator
{
public:
    //! @name constructor / destructor
    //@{
    //! constructor
    /*! 
     * @param[in] acv global context
     * @param[in] rowno_pos positions of ROWNO columns
     * @param[in] rescnt amount of records already in result set
     * @param[in] maxrescnt maximal amount of records in result set
     */
    Join_IResultSetOperator( 
            SQLMan_Context&         acv, 
            Join_RownoPositions&    rowno_pos,
            const SAPDB_Int4&       rescnt,
            const SAPDB_Int4&       maxrescnt) : 
        m_acv(acv), m_ResultCnt(rescnt), m_MaxResultCnt(abs(maxrescnt)), 
        m_opened(false),
        m_RecordCounterPositions( acv.GetAllocator() )
        {
            SAPDBTRACE_METHOD_DEBUG( "Join_IResultSetOperator::ctor", Join_Trace, 5 );
            SAPDBTRACE_WRITELN( Join_Trace, 3, "m_ResultCnt: " << m_ResultCnt << "\tm_MaxResultCnt: " << m_MaxResultCnt );
            if ( !m_RecordCounterPositions.Initialize( rowno_pos ))
            {
                m_acv.ThrowError( e_no_more_memory );
            }
            m_SetWarning = !( maxrescnt < 0 );
#ifdef SAPDB_SLOW
            if ( !m_RecordCounterPositions.IsEmpty() )
            {
                SAPDBTRACE_WRITELN( Join_Trace, 3, "ROWNUM positions:" );
                Join_RownoPositions::Iterator _iter;
                for ( _iter = m_RecordCounterPositions.Begin(); _iter != m_RecordCounterPositions.End(); ++_iter) 
                {
                    SAPDBTRACE_WRITELN( Join_Trace, 3, *_iter );
                }
        }
#endif
        }
        
    //! destructor
    virtual ~Join_IResultSetOperator() 
    {
        SAPDBTRACE_METHOD_DEBUG( "Join_IResultSetOperator::dtor", Join_Trace, 5 );
    }
    //@}

    //! initializes operator
    /*!
      * default implementation analyses result record and
      * detect ROWNO/ROWNUM positions
      */
    virtual void Open() { m_opened = true; }

    //! finalize operator
    virtual void Close() { m_opened = false; }
    //! determine if operator is open
    SAPDB_Bool IsOpened() const { return m_opened; }

    //! @name manipulation of result set
    //@{
    //! add record to result set
    /*!
     * @param[in] record record to be added
     * @return information code [e_ok, e_no_next_record] / error code [ ... ]
     */
    virtual tgg00_BasisError Add( tgg00_Rec& record ) =0;
    //@}

    //! @name properties
    //@{
    //! return amount of records in result set
    virtual SAPDB_Int4 ResultCount() const { return m_ResultCnt; }
    //@}
    
protected:
    //! write result counter into record
    /*!
     * @param[in] record record on which to write result counter
     * @param[in] count result counter to write
     */
    void set_result_counter( tgg00_Rec &record, const SAPDB_Int4 count ) const
    {
        // add result counter to record
        s20int4_to_buf_swap( count, g01code.kernel_swap, 
                record.recBody_gg00(), record.recKeyLen_gg00() - 3, 
                sw_normal );
    }


    // member variables
    SQLMan_Context&           m_acv; //!< global context
    SAPDB_Int4                      m_ResultCnt; //!< ammount of result rows in result set
    SAPDB_Int4                      m_MaxResultCnt; //!< max. amount of result rows
    SAPDB_Bool                      m_SetWarning; //!< set warning if result count exceed ROWNO amount
    Join_RownoPositions     m_RecordCounterPositions;
private:
    SAPDB_Bool              m_opened;
};

#endif // JOIN_IRESULTSETOPERATOR_HPP
