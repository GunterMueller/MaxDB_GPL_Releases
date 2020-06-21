#ifndef JOIN_RESULTSETAGGREGATOR2_HPP
#define JOIN_RESULTSETAGGREGATOR2_HPP
/*!
 * @file
 * @brief 
 *
 * @author GertG
 * @ingroup Join
 *
 * @sa Join_Aggregator.hpp
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
#include "SAPDBCommon/SAPDB_AutoPtr.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "SQLManager/SQLMan_MessBlock.hpp"
#include "Container/Container_Vector.hpp"
#include "Join/Join_IOperator.hpp"
#include "Join/Join_IResultSetOperator.hpp"
#include "Table/Table_TempHashTable.hpp"
#include "Join/Join_Aggregation.hpp"
#include "Join/Join_Aggregator.hpp"
#include "hgg04.h"
#include "gsp00.h"
#include "hbd73.h"
#include "hbd01.h"
#include "hbd07.h"
#include "hak101.h"

//! operator which aggregates record corresponding to key
class Join_ResultSetAggregator2 : public Join_IResultSetOperator {
public:
    //! @name constructor / destructor
    //@{
    //! constructor
    /*! 
     * @param[in] acv global context
     * @param[in] rowno_pos positions of ROWNO columns
     * @param[in] aggr_desc autoptr pointer to result record description (takes ownership)
     * @param[in] result_id name of result file
     * @param[in] rescnt amount of records already in result set
     * @param[in] max_recs_accepted maximal amount of records accepted for result set
     * @param[in] is_select_into is the statement a "SELECT ... INTO ... " 
     * @param[in] one_group is statement without "GROUP BY" clause
     * @param[in] rec_producer operator which delivers records to this operator
     */
    Join_ResultSetAggregator2(
            SQLMan_Context      &acv, 
            Join_RownoPositions &rowno_pos,
            const tgg00_StackList*  aggr_desc,
            tgg00_FileId        &result_id, 
            const SAPDB_Int4    &rescnt,
            const SAPDB_Int4    &max_recs_accepted,
            const pasbool       is_select_into,
            const pasbool       one_group,
            Table_FixSizeEntry_FieldDescription   &field_desc,
            Table_FixSizeEntry_KeyNoList          &key_desc,
            IOperator                             &rec_producer ) :
    Join_IResultSetOperator(acv, rowno_pos, rescnt, max_recs_accepted), 
    m_SelectInto( is_select_into ), 
    m_OneGroup( one_group ),
    m_PostProcessing(sfh_none), 
    m_GroupCnt(0), 
    m_LaterOutputPos(0), 
    // m_TreePos
    m_ResultTree( result_id ), 
    // m_DistinctFileId
    m_DistinctFileRoot( acv.GetAllocator() ), 
    m_AggregationDesc( aggr_desc ),
    m_RecordProducer(rec_producer),
    m_Aggregator( acv.GetAllocator() ),
    m_TempHashTable( acv.GetAllocator() )
    {
        SAPDBTRACE_METHOD_DEBUG( "Join_ResultSetAggregator2::ctor", Join_Trace, 3 );
        g04build_temp_tree_id( m_DistinctFileId, acv.TransContext() );
        m_DistinctFileId.fileTfnTemp_gg00().becomes( ttfnDistinct_egg00 );
        m_DistinctFileRoot.Reserve(2);
        m_TreePos.tpsPno_gg00 = NIL_PAGE_NO_GG00; 

        tgg00_BasisError _b_err = 
            Join_Aggregation::initialize( m_acv, *m_AggregationDesc, m_DistinctFileId,
                    m_DistinctFileRoot, m_ResultTree, m_LaterOutputPos, m_PostProcessing );

        if ( e_ok != _b_err )
        {
            m_acv.ThrowError( _b_err );
            return;
        }

        // construct Aggregator
        if ( ! m_Aggregator.SetPtr( new ( acv.GetAllocator() ) 
                    Join_Aggregator( m_acv, 
                        *m_AggregationDesc, 
                        m_DistinctFileId, m_DistinctFileRoot )) )
        {
            m_acv.ThrowError( e_no_more_memory );
            return;
        }

        // construct m_TempHashTable
        if ( m_OneGroup )
        {
            m_TempHashTable.SetPtr( new ( acv.GetAllocator() )
                    Table_FixSizeTempSingleRecordAggregate(
                        acv.TransContext(),
                        *m_Aggregator,
                        m_RecordProducer.GetRecordLength(),
                        acv.GetErrorList() ));
        }
        else
        {
            m_TempHashTable.SetPtr( new ( acv.GetAllocator() )
                    Table_FixSizeTempHashTableAggregate(
                        acv.TransContext(),
                        *m_Aggregator,
                        m_RecordProducer.GetRecordLength(),
                        field_desc,
                        key_desc,
                        m_MaxResultCnt /*expected recs*/,
                        acv.GetErrorList()) );
        }

        if ( ! ( m_TempHashTable.IsAssigned() && 
                 m_TempHashTable->CompletelyConstructed() ))
            m_acv.ThrowError( e_no_more_memory );
        else
            b73cmd_count( imem_sorts );
    }

    //! destructor
    virtual ~Join_ResultSetAggregator2()
    {
        SAPDBTRACE_METHOD_DEBUG( "Join_ResultSetAggregator2::dtor", Join_Trace, 3 );

        if ( ! m_DistinctFileRoot.IsEmpty() )
        {
            // destroy distinct files
            a101_DestroyGroupedTempFiles (
                m_acv.TransContext(), ttfnDistinct_egg00, -1, -1,
                m_DistinctFileId.fileName_gg00());
        }
    }

    //! finalize operator
    virtual void Close()
    {
        SAPDBTRACE_METHOD_DEBUG( "Join_ResultSetAggregator2::Close", Join_Trace, 3 );
        if ( this->IsOpened() )
        {
            // build result file
            post_processing();

            // this->ResultCount() valid !!
            if  ( m_OneGroup && 0 == this->ResultCount() )
                this->write_NULL_record();

            if ( m_SelectInto && m_OneGroup )
            {
                // move record into data part of messblock
                // normal result record or NULL record 
                tgg00_BdSetResultRecord _set_result;
                tgg00_Lkey              _key;
                _set_result.bd_key_check_len = 0;
                _set_result.bd_max_rec_cnt   = 1;
                _set_result.bd_max_fill_len  = sizeof(m_acv.a_mblock.mb_data()->mbp_rec());
                _set_result.bd_next          = true;
                _set_result.bd_drop_page     = false;
                m_TreePos.tpsPno_gg00 = NIL_PAGE_NO_GG00; 
                _key.keyLen_gg00() = 0;
                b07cnext_record( m_acv.TransContext(), m_ResultTree, _key, 
                        _set_result, m_TreePos, &m_acv.a_mblock.mb_data()->mbp_rec());
                m_acv.GetMessBlock().DataLength() = m_acv.a_mblock.mb_data()->mbp_rec().recLen_gg00();
                b01empty_file( m_acv.TransContext(), m_ResultTree );
            }
            b73cmds_count( imem_sorts_rows_read, m_ResultCnt );
        }
    }
    //@}

    //! @name properties
    //@{
    //! return amount of records in result set, only valid after Close()
    virtual SAPDB_Int4 ResultCount() const { return m_GroupCnt; }
    //@}


    //! common code to add a record to result set
    /*!
     * @param[in] record record to be added
     * @return information code [e_ok, e_no_next_record] / error code [ ... ]
     */
    tgg00_BasisError Add( tgg00_Rec& record );

private:
        
    //! calculate functions and check SUM overflow
    void post_processing();
    void write_NULL_record();
    
    // member variables
    bool            m_SelectInto, m_OneGroup;
    tgg07_setfunc_handling_Enum m_PostProcessing;
    SAPDB_Int4      m_GroupCnt;             //!< result count == group count
    SAPDB_UInt4     m_LaterOutputPos;
    tgg00_FilePos   m_TreePos;
    tgg00_FileId    m_ResultTree;           //!< name of result file
    tgg00_FileId    m_DistinctFileId;       //!< template of distinct file names
    Container_Vector<Join_RootPair>  m_DistinctFileRoot;
    const tgg00_StackList*           m_AggregationDesc; //!< result record description
    IOperator       &m_RecordProducer;
    SAPDB_AutoPtr<Join_Aggregator> m_Aggregator;
    SAPDB_AutoPtr<Table_FixSizeTempAggregate> m_TempHashTable;
};

#endif // JOIN_IRESULTSETAGGREGATOR_HPP
