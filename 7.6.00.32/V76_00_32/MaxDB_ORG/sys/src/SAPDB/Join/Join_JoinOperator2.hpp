#ifndef JOIN_JOINOPERATOR2_HPP
#define JOIN_JOINOPERATOR2_HPP
/*!
 * @file
 * @brief realizes binary left linear join operator
 *
 * @author GertG
 * @ingroup Join
 *
 * @sa Join_Execute.cpp
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
#include "gsp00.h"
#include "ggg00.h"
#include "ggg07.h"
#include "gkb07.h"
#include "vak001.h"
#include "gak68.h"
#include "Join/Join_Types.hpp"
#include "Join/Join_IOperator.hpp"

class SQLMan_Context;

//! realizes binary left linear join operator
class Join_JoinOperator2: public IOperator 
{
public:
    //! @name constructor / destructor
    //@{
    //! constructor
    Join_JoinOperator2(
        SQLMan_Context              &acv,
        tak_dml_info                &dmli,
        const tak_parskey           &parsk,
        const tak68_joinview_rec    &jvrec,
        const pasbool               use_old_rescnt,
        const pasbool               del_parsinfos,
        const tak68_sequence        &sequence,
        const SAPDB_Int4            act_join );

    //! destructor
    virtual ~Join_JoinOperator2();
    //@}
    
    //! @name record stream manipulation functions
    //@{
    //! open operator with keys given by strategy information
    virtual tgg00_BasisError Open();

    //! open stream operator and limit stream with start/stop key
    virtual tgg00_BasisError Open(
            const Join_TwoKeys& startkeys, 
            const Join_TwoKeys& stopkeys );

    //! get next record from stream
    virtual tgg00_BasisError Next( tgg00_Rec* &recptr );

    //! close operator
    virtual void Close();
    //@}

    //! @name operator/record properties
    //@{
    //! return key length of result records in stream
    SAPDB_UInt2 GetKeyLength() const { return m_JoinInfo.jn_res_key_len; }

    //! return length of result records in stream
    SAPDB_UInt2 GetRecordLength() const { return m_JoinInfo.jn_res_rec_len; }

    //! get result NULL record 
    virtual tgg00_BasisError GetNullRecord( tgg00_Rec* &recptr );
    //@}

protected:
    virtual tgg00_BasisError next( tgg00_Rec* *records );
    virtual tgg00_BasisError get_null_record( tgg00_Rec* *records );
    
private:

    // member types
    enum mt_scan_stat { never_scanned, while_scanning, already_scanned };

    // member methods
    //! return key length of joined records
    SAPDB_UInt2 join_keylen() const { return m_JoinInfo.jn_key_len; }

    //! return length of joined records
    SAPDB_UInt2 join_reclen() const { return m_JoinInfo.jn_rec_len; }
    
    tgg00_BasisError operator_init();
    tgg00_BasisError create_filter( SAPDB_Bool );
    SAPDB_Bool join( tgg00_Rec* *records, tgg00_BasisError& );
    tgg00_BasisError prepare_right_key( tgg00_Rec* *records );
    tgg00_BasisError build_right_startkey( tgg00_Rec* *records, tgg00_Lkey &key, SAPDB_Int2 &last_field_len );
    void create_output( const SAPDB_AutoPtr<tgg00_Rec>& target, tgg00_Rec* *records );
    void convert_key( tgg00_Lkey& );
    SAPDB_Bool filter( const SAPDB_AutoPtr<tgg00_Rec>&, tgg00_BasisError& );
    tgg00_BasisError reset();
    SAPDB_UInt4 get_AuxRightOuterKeyLen();

    IOperator* AccessOperatorFactory(
        SQLMan_Context     &acv,
        const SAPDB_UInt4  &buffersize,
        const SAPDB_UInt4  &expectedRecordCount,
        const SAPDB_Int4   &maxServerTasks,
        const SAPDB_UInt2  tabno);

    // member variables
    SAPDB_AutoPtr<IOperator> m_LeftOp, m_RightOp;
    SAPDB_AutoPtr<tgg00_Rec> m_JoinRec, m_NullRec, m_RightAuxRec; 
    tgg00_Rec*          *m_Records;
    SAPDB_AutoPtr<tgg00_StackList>  m_Filter;
    SAPDB_AutoPtr<tak68_outcol>     m_OutputInfo;
    SAPDB_UInt4         m_JoinRecBufferSize;
    SAPDB_UInt4         m_AuxRightOuterKeyLen;
    SAPDB_Int4          m_RightRecConvArrCnt;
    const SAPDB_Int4    m_RightAccessInvLen;
    SAPDB_Int2          m_LastRightDefBytePos;
    SAPDB_Int2          m_OutputInfoSize;
    const SAPDB_UInt2   m_RightTableIdx;
    const tak68_sequence &m_JoinSequence;
    tkb07_conv_arr      m_RightRecConvArr;              // 1536 byte
    tak68_join          m_JoinInfo;
    tgg00_StackDesc     m_FilterDesc;                   // 72 byte
    tgg00_SelectFieldsParam m_SelFields;                // 2268 byte
    tgg00_FileId        m_AuxRightOuterJoinFile;        // 40 byte
    tgg00_FilePos       m_AuxRightOuterJoinFilePos;     // 8 byte
    tgg00_Lkey          m_AuxRightOuterJoinKey;         // 1032 byte
    tak_joinrec         m_d_joins;
    const tgg07_StratEnum m_RightAccessStrat;
    mt_scan_stat        m_TableForRightOuterJoinScanStat;
    const SAPDB_Bool    m_IsTopJoinOp;
    SAPDB_Bool          m_GetLeftTupel;
    SAPDB_Bool          m_LeftRecChanged;
    SAPDB_Bool          m_LeftRecJoined;
    pasbool             m_IsDescendingIndex;
    SAPDB_Bool          m_AppendRightOuterJoinRecords;
    SAPDB_Bool          m_OutputFilter;
};

#endif
