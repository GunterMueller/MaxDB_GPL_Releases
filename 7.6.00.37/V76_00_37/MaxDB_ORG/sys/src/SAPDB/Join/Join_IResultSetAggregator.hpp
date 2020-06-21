#ifndef JOIN_IRESULTSETAGGREGATOR_HPP
#define JOIN_IRESULTSETAGGREGATOR_HPP
/*!
 * @file
 * @brief interface to result set operator which inserts record and 
 * sort them corresponding to key
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
#include "Container/Container_Vector.hpp"
#include "Join/Join_IOperator.hpp"
#include "Join/Join_IResultSetOperator.hpp"
#include "Join/Join_Aggregation.hpp"
#include "hgg04.h"
#include "gsp00.h"
#include "hsp30.h"
#include "hsp40.h"
#include "hsp41.h"
#include "hsp51.h"
#include "hta01.h"
#include "hbd73.h"

namespace Join_Aggregation {
tgg00_BasisError new_group( Join_AggregationContext&, tgg00_Rec&);
tgg00_BasisError update_group( Join_AggregationContext&, tgg00_Rec&, tgg00_Rec&);
}

//! interface to result set operator which aggregates record corresponding to key
class Join_IResultSetAggregator : public Join_IResultSetOperator {
public:
    //! @name constructor / destructor
    //@{
    //! constructor
    /*! 
     * @param[in] acv global context
     * @param[in] rowno_pos positions of ROWNO columns
     * @param[in] result_id name of result file
     * @param[in] rescnt amount of records already in result set
     * @param[in] max_recs_accepted maximal amount of records accepted for result set
     * @param[in] rec_producer operator delivers records
     */

    
    Join_IResultSetAggregator(
            SQLMan_Context&     acv, 
            Join_RownoPositions& rowno_pos,
            const tgg00_StackList* aggr_desc,
            tgg00_FileId&       result_id, 
            const SAPDB_Int4&   rescnt,
            const SAPDB_Int4&   max_recs_accepted,
            IOperator&          rec_producer ) :
    Join_IResultSetOperator(acv, rowno_pos, rescnt, max_recs_accepted), 
    m_GroupCnt(0), 
    // m_GroupRec
    // m_TreePos
    // m_GroupKey
    m_LaterOutputPos(0), 
    m_ResultTree( result_id ),
    m_AggregationDesc( aggr_desc ),
    m_RecordProducer(rec_producer),
    m_CallPostProcessing(sfh_none), 
    // m_DistinctFileId
    m_DistinctFileRoot( acv.GetAllocator() ),
    m_AggrContext( m_acv, *m_AggregationDesc, m_DistinctFileId, m_DistinctFileRoot )
    {
        SAPDBTRACE_METHOD_DEBUG( "Join_IResultSetAggregator::ctor", Join_Trace, 5 );
        g04build_temp_tree_id( m_DistinctFileId, acv.TransContext() );
        m_DistinctFileId.fileTfnTemp_gg00().becomes( ttfnDistinct_egg00 );
        m_DistinctFileRoot.Reserve(2);
        b73cmd_count( iins_sorts );
        SAPDBTRACE_IF( Join_Trace, 3, SAPDB_MemFillNoCheck(&m_GroupRec, ':', sizeof(m_GroupRec)) );
        SAPDBTRACE_IF( Join_Trace, 3, SAPDB_MemFillNoCheck(&m_GroupKey, ':', sizeof(m_GroupKey)) );

        tgg00_BasisError _b_err = 
            Join_Aggregation::initialize( m_acv, *m_AggregationDesc, m_DistinctFileId,
                    m_DistinctFileRoot, m_ResultTree, m_LaterOutputPos, m_CallPostProcessing );

        if ( e_ok != _b_err )
        {
            m_acv.ThrowError( _b_err );
            return;
        }
    }

    //! destructor
    virtual ~Join_IResultSetAggregator();

    //! finalize operator
    virtual void Close()
    {
        SAPDBTRACE_METHOD_DEBUG( "Join_IResultSetAggregator::Close", Join_Trace, 3 );
        if ( this->IsOpened() )
        {
            if ( this->ResultCount() > 0 )
            {
                if ( (sfh_none != m_CallPostProcessing) || !m_RecordCounterPositions.IsEmpty() )
                    post_processing();
            }
            b73cmds_count( iins_sorts_rows_ins, this->ResultCount() );
        }
    }
    //@}

    //! @name properties
    //@{
    //! return amount of records in result set
    virtual SAPDB_Int4 ResultCount() const { return m_GroupCnt; }
    //@}

protected:

    //! common code to add a record to result set
    /*!
     * @param[in] record record to be added
     * @return information code [e_ok, e_no_next_record] / error code [ ... ]
     */
    tgg00_BasisError add( tgg00_Rec& record )
    {
        SAPDBTRACE_METHOD_DEBUG( "Join_IResultSetAggregator::add", Join_Trace, 3 );

        tgg00_BasisError _b_err = e_ok;
        
        set_result_counter( record, 1 );
        
        if ( e_key_not_found == this->get_group_record( record ))
        {
            if  ( e_ok == ( _b_err = this->is_new_group_allowed()))
            {
                if ( e_ok == (_b_err = Join_Aggregation::new_group( m_AggrContext, record )))
                        _b_err = this->add_group_record( record );
            }
        }
        else
        {
            if ( e_ok == ( _b_err = Join_Aggregation::update_group( m_AggrContext, record, m_GroupRec, false )))
            {
                if  ( m_AggrContext.m_grouprec_changed )
                    _b_err = this->update_group_record();
            }
        }

        if ( e_ok == _b_err )
        {
            if ( m_acv.TransContext().trRteCommPtr_gg00->to_cancel )
                _b_err = e_cancelled;

            if ( ++m_ResultCnt == m_MaxResultCnt ) 
            {
                _b_err = e_no_next_record;
                if ( m_SetWarning )
                {
                    m_acv.TransContext().trWarning_gg00.addElement(warn0_exist);
                    m_acv.TransContext().trWarning_gg00.addElement(warn12_rowno_used);
                }
            }
        }
        SAPDBTRACE_WRITELN( Join_Trace, 3, "accepted recs: " << m_ResultCnt << "\tresult cnt: " << m_GroupCnt );
        SAPDBTRACE_WRITELN( Join_Trace, 3, "return: " << SAPDBTrace::BasisError(_b_err) );
        return _b_err;
    }

    void build_group_key( tgg00_Rec& record )
    {
        SAPDBTRACE_IF( Join_Trace, 5, SAPDB_MemFillNoCheck( &m_GroupKey, ':', sizeof( m_GroupKey ) ) );
        m_GroupKey.len() = record.recKeyLen_gg00();
        SAPDB_MemCopyNoCheck( &m_GroupKey.k(), (SAPDB_Byte *)&record.recBody_gg00(), record.recKeyLen_gg00() );
        SAPDBTRACE_IF( Join_Trace, 5, t01key( td_always, "m_GroupKey  ", m_GroupKey ) );
    }
    
    //! get group record for a given record into buffer behind m_GroupRec
    /*!
     * @param[in] record record for a group
     */
    virtual tgg00_BasisError get_group_record( tgg00_Rec& record )=0;

    //! update group record for a given record
    virtual tgg00_BasisError update_group_record()=0;

    //! insert group record for a given record
    /*!
     * @param[in] record record for a group
     */
    virtual tgg00_BasisError add_group_record( tgg00_Rec& record )=0;

    //! build an updated group record in m_GroupRec
    /*!
     * @param[in] record record for group update
     */
    virtual tgg00_BasisError is_new_group_allowed() =0 ;
    
    virtual void init_result_scan()=0;
    
    virtual tgg00_BasisError fetch_next_result( tgg00_Rec& record )=0;

    virtual tgg00_BasisError update_result( tgg00_Rec& record )=0;
        
    // member variables
    SAPDB_Int4 m_GroupCnt;            //!< result count == group count
    tgg00_Rec  m_GroupRec;            //!< aux. buffer for group record
    tgg00_FilePos  m_TreePos; //!< treepos of group record
    tgg00_Lkey     m_GroupKey;        //!< group key
    SAPDB_UInt4    m_LaterOutputPos;
    tgg00_FileId   m_ResultTree;      //!< name of result file
    const tgg00_StackList*  m_AggregationDesc; //!< result record description
    
    IOperator&     m_RecordProducer;

private:

    //! calculate functions and check SUM overflow
    void post_processing();
    
    // member variables
    tgg07_setfunc_handling_Enum m_CallPostProcessing;

    tgg00_FileId    m_DistinctFileId; //!< template name of distinct files
    Container_Vector<Join_RootPair>  m_DistinctFileRoot; // ORDER DEPENDENCY
    Join_AggregationContext          m_AggrContext; // ORDER DEPENDENCY
};

#endif // JOIN_IRESULTSETAGGREGATOR_HPP
