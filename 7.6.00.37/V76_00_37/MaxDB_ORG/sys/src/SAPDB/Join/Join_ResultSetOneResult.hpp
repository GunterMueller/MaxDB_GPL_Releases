#ifndef JOIN_RESULTSETONERESULT_HPP
#define JOIN_RESULTSETONERESULT_HPP
/*!
 * @file
 * @brief implements interface Join_IResultSetOperator which contains only 
 * one record
 *
 * @author GertG
 * @ingroup Join
 *
 * @sa Join_Execute.cpp, Join_IResultSetOperator.hpp
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
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "SQLManager/SQLMan_MessBlock.hpp"
#include "Join/Join_IResultSetOperator.hpp"
#include "Container/Container_Vector.hpp"
#include "Join/Join_IOperator.hpp"
#include "hsp41.h"


//! implements interface Join_IResultSetOperator which contains only one record
class Join_ResultSetOneResult : public Join_IResultSetOperator {
public:
    //! @name constructor / destructor
    //@{
    //! constructor
    /*! 
     * @param[in] acv global context
     * @param[in] rowno_pos positions of ROWNO columns
     * @param[in] rec_producer Operator which delivers records
     */
    Join_ResultSetOneResult( 
            SQLMan_Context&     acv,
            Join_RownoPositions&    rowno_pos,
            const IOperator&    rec_producer ) :
    Join_IResultSetOperator( acv, rowno_pos, 0, 1 ),
    m_RecBuf( acv.GetAllocator() )
    {
        SAPDBTRACE_METHOD_DEBUG( "Join_ResultSetOneResult::ctor", Join_Trace, 1 );
        if ( m_RecBuf.SetPtr( (tgg00_Rec*) acv.GetAllocator().
                    Allocate( rec_producer.GetRecordLength() + 10 /* fault tolerance */ )))
        {
#if defined(SAPDB_SLOW) ||defined(SAPDB_QUICK)
            m_BufSize = rec_producer.GetRecordLength();
#endif
            SAPDBTRACE_WRITELN( Join_Trace, 3, "allocated " <<  rec_producer.GetRecordLength() << "bytes on @ " << (void*)m_RecBuf.GetPtr());
        }
        else
            acv.ThrowError( e_no_more_memory );
    }
    
    //! finalize operator
    virtual void Close()
    {
        SAPDBTRACE_METHOD_DEBUG( "Join_ResultSetOneResult::Close", Join_Trace, 1 );
        if ( this->IsOpened() )
        {
            if ( 0 == this->ResultCount() )
            {
                m_acv.GetMessBlock().DataLength() = 0;
                if ( e_ok == m_acv.TransContext().trError_gg00 )
                {
                    m_acv.TransContext().trError_gg00 = e_no_next_record;
                }
            }
            else
            {
                // copy result into data part
                SAPDBTRACE_IF( Join_Trace, 2, 
                t01buf( td_always, m_RecBuf.GetPtr(), 1, m_RecBuf->recLen_gg00() <= 300 ? m_RecBuf->recLen_gg00() : 300 ));
                SAPDB_MemCopyNoCheck( &m_acv.a_mblock.mb_data()->mbp_4kbuf(), m_RecBuf.GetPtr(), m_RecBuf->recLen_gg00() );
                m_acv.GetMessBlock().DataLength() = m_RecBuf->recLen_gg00();
            }
        }
    }
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
        SAPDBTRACE_METHOD_DEBUG( "Join_ResultSetOneResult::Add", Join_Trace, 1 );

        if ( m_ResultCnt < m_MaxResultCnt )
        {
            return add( record );
        }
        else
        {
            if ( sqlm_oracle == m_acv.a_sqlmode )
            {
                SAPDBTRACE_WRITELN( Join_Trace, 3, "accept more results" );
                m_acv.TransContext().trWarning_gg00.addElement(warn0_exist);
                m_acv.TransContext().trWarning_gg00.addElement(warn12_rowno_used);
                return e_no_next_record;
            }
            else
            {
                SAPDBTRACE_WRITELN( Join_Trace, 3, "too many results" );
                return e_too_many_resultsets;
            }
        }
    }
    //@}

protected:
    virtual tgg00_BasisError add( tgg00_Rec& record )
    {
        SAPDBTRACE_METHOD_DEBUG( "Join_ResultSetOneResult::add", Join_Trace, 3 );
        
        tsp00_NumError   _ret;

        set_result_counter( record, m_ResultCnt + 1 );

        // put ROWNO
        Container_Vector<SAPDB_Int4>::Iterator _iter;
        for ( _iter = m_RecordCounterPositions.Begin(); _iter != m_RecordCounterPositions.End(); ++_iter) 
        {
            record.buf()[ *_iter - 1 ] = csp_defined_byte;
            s41plint( record.buf(), *_iter + 1,
                  csp_resnum_deflen, 0, m_ResultCnt + 1, _ret );
        }

        SAPDBERR_ASSERT_STATE( m_BufSize >= record.recLen_gg00() );
        SAPDBTRACE_IF( Join_Trace, 2, 
        t01buf( td_always, &record, 1, record.recLen_gg00() <= 300 ? record.recLen_gg00() : 300 ));
        // move record into data part of messblock
        SAPDB_MemCopyNoCheck( m_RecBuf.GetPtr(), &record, record.recLen_gg00() );
        ++m_ResultCnt;
        return e_ok;
    }

    // data mambers
    SAPDB_AutoPtr<tgg00_Rec> m_RecBuf;
#if defined(SAPDB_SLOW) || defined(SAPDB_QUICK)
    SAPDB_Int4 m_BufSize;
#endif
};

#endif // JOIN_RESULTSETONERESULT_HPP
