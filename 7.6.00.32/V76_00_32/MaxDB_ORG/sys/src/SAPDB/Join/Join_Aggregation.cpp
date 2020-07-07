/*!
 * \file
 * \brief implements functions for aggregations
 *
 * \author GertG
 * \ingroup Join
 *
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

#include "Join/Join_Aggregation.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "SAPDBCommon/SAPDB_RangeCode.hpp"
#include "Container/Container_Vector.hpp"
#include "Join/Join_Types.hpp"
#include "Join/Join_Trace.hpp"
#include "ggg00.h"
#include "hta01.h"
#include "hkb721.h"
#include "hbd01.h"
#include "hbd07.h"
#include "hsp41.h"
#include "hsp51.h"
#include "hsp20.h"
#include "hsp30.h"
#include "hsp40.h"
#include "hak101.h"
#include "hgg01_3.h"
#include "hbd998.h"

namespace Join_Aggregation {
/*!
 *
 */
tgg00_BasisError new_group( 
        Join_AggregationContext &context,
        tgg00_Rec& group_rec )
{
    SAPDBTRACE_ROUTINE_DEBUG( "Join_Aggregation::new_group", Join_Trace, 3 );
    SAPDBTRACE_STREAMVAR( _TraceStream, Join_Trace );
    tgg00_BasisError _b_err = e_ok;

    SAPDBTRACE_STREAMWRITELN( _TraceStream, Join_Trace, 3, "input record:" );
    SAPDBTRACE_IF( Join_Trace, 3, 
    t01buf( td_always, &group_rec, 1, group_rec.recLen_gg00() <= 300 ? group_rec.recLen_gg00() : 300 ));
    
    SAPDB_UInt4 _func_pos = context.m_RecordDesc[ 0 ].epos() - 1;
    SAPDB_UInt4 _out_pos, _check_null_pos = 0, _disfile_cnt = 0;
    bool _null_found = false;
    
    // loop over functions
    while (( _func_pos - context.m_RecordDesc[ _func_pos - 1 ].epos() > 1) 
            && ( e_ok == _b_err ))
    {
        _func_pos -= context.m_RecordDesc[ _func_pos - 1 ].epos();
        // loop over LASTFUNCTION
        while (( st_func == context.m_RecordDesc[ _func_pos - 1 ].etype() ) 
                && ( op_f_none == context.m_RecordDesc[ _func_pos - 1 ].eop_func() ))
            _func_pos -= context.m_RecordDesc[ _func_pos - 1 ].epos();
        _out_pos = k721out_entry( (tgg00_StackList*) &context.m_RecordDesc, _func_pos );
        if ( st_func == context.m_RecordDesc[ _func_pos - 1 ].etype() )
        {
            SAPDBTRACE_STREAMWRITELN( _TraceStream, Join_Trace, 3, " func " << context.m_RecordDesc[ _func_pos - 1 ].eop_func() << " on " << _func_pos );
            if ( op_f_check_null == context.m_RecordDesc[ _func_pos - 1 ].eop_func() )
            {
                _check_null_pos = context.m_RecordDesc[ _out_pos - 1 ].epos();
                SAPDBTRACE_STREAMWRITELN( _TraceStream, Join_Trace, 3, "_check_null_pos: " << _check_null_pos );
            }
            else
            {
                if ( op_f_dis_avg   == context.m_RecordDesc[ _func_pos - 1 ].eop_func() ||
                     op_f_dis_sum   == context.m_RecordDesc[ _func_pos - 1 ].eop_func() ||
                     op_f_dis_count == context.m_RecordDesc[ _func_pos - 1 ].eop_func() ||
                     op_f_stddev    == context.m_RecordDesc[ _func_pos - 1 ].eop_func() ||
                     op_f_dis_stddev == context.m_RecordDesc[ _func_pos - 1 ].eop_func() ||
                     op_f_variance  == context.m_RecordDesc[ _func_pos - 1 ].eop_func() ||
                     op_f_dis_variance == context.m_RecordDesc[ _func_pos - 1 ].eop_func() )
                {
                    ++_disfile_cnt;
                    if ( csp_undef_byte != group_rec.recBuf_gg00()[ context.m_RecordDesc[ _out_pos - 1 ].epos() - 1 ] && 
                         csp_oflw_byte  != group_rec.recBuf_gg00()[ context.m_RecordDesc[ _out_pos - 1 ].epos() - 1 ] )
                    {
                        SAPDBTRACE_STREAMWRITELN( _TraceStream, Join_Trace, 3, "create dist entries" );
                        tgg00_FileId _htree( context.m_DistinctFileId );
                        tgg00_Rec _dist_buffer;
                        SAPDBTRACE_IF( Join_Trace, 3, SAPDB_MemFillNoCheck( &_dist_buffer, ':', sizeof( _dist_buffer ) ) );
                        
                        // move key=<group value> to _dist_buffer
                        SAPDB_RangeMove( __FILE__, 1,
                                sizeof( group_rec ), sizeof(_dist_buffer),
                                &group_rec.recBody_gg00(), 1,
                                &_dist_buffer.recBody_gg00(), 1,
                                group_rec.recKeyLen_gg00(), _b_err );
                        // move value to aggregate to _dist_buffer
                        SAPDB_RangeMove( __FILE__, 2,
                                sizeof( group_rec ), sizeof( _dist_buffer ),
                                &group_rec, context.m_RecordDesc[ _out_pos - 1 ].epos(),
                                &_dist_buffer.recBuf_gg00(), 
                                group_rec.recKeyLen_gg00() - RESCNT_MXGG04 + cgg_rec_key_offset + 1,
                                context.m_RecordDesc[ _out_pos - 1 ].elen_var(), 
                                _b_err );
                        // set key = <group value, value>
                        _dist_buffer.recKeyLen_gg00() = group_rec.recKeyLen_gg00() 
                            - RESCNT_MXGG04
                            + context.m_RecordDesc[ _out_pos - 1 ].elen_var();
                        // set record length
                        _dist_buffer.recLen_gg00() = 
                            _dist_buffer.recKeyLen_gg00() + cgg_rec_key_offset;
                        _dist_buffer.recVarcolOffset_gg00() = 0;
                        _dist_buffer.recVarcolCnt_gg00() = 0;
                        
                        if ( op_f_stddev        == context.m_RecordDesc[ _func_pos - 1 ].eop_func() ||
                             op_f_dis_stddev    == context.m_RecordDesc[ _func_pos - 1 ].eop_func() ||
                             op_f_variance      == context.m_RecordDesc[ _func_pos - 1 ].eop_func() ||
                             op_f_dis_variance  == context.m_RecordDesc[ _func_pos - 1 ].eop_func() )
                        {
                            // write 1 with swapping 
                            SAPDBTRACE_STREAMWRITELN( _TraceStream, Join_Trace, 3, "write 1" );
                            s20int4_to_buf_swap( 1, g01code.kernel_swap,
                                  _dist_buffer.recBody_gg00(),
                                  _dist_buffer.recKeyLen_gg00() + 1,
                                  sw_normal);
                            _dist_buffer.recLen_gg00() += RESCNT_MXGG04;
                        }

                        a101_GetLogicalFilename (context.m_acv.TransContext(), _htree);
                        a101_SetTempFileIndex (context.m_acv, _htree, _disfile_cnt);
                        a101_GetTempFileInstance ( context.m_acv.TransContext(), _htree);

                        _htree.fileRoot_gg00() = context.m_DistinctFileRoot[ _disfile_cnt -1].m_Root;
                        _htree.fileRootCheck_gg00() = context.m_DistinctFileRoot[ _disfile_cnt -1].m_RootCheck;
                        SAPDBTRACE_STREAMWRITELN( _TraceStream, Join_Trace, 3, "add _dist_buffer" );
                        SAPDBTRACE_IF( Join_Trace, 3, 
                        t01buf( td_always, &_dist_buffer, 1, _dist_buffer.recLen_gg00() <= 300 ? _dist_buffer.recLen_gg00() : 300 ));
                        b07cadd_record( context.m_acv.TransContext(), _htree, _dist_buffer );
                        _b_err = context.m_acv.TransContext().trError_gg00;
                    }
                }
                else
                {
                    // do nothing for op_f_all_count, op_f_count, 
                    // op_f_avg. op_f_min/max, op_f_sum
                }
                if (( csp_undef_byte == group_rec.recBuf_gg00()[ context.m_RecordDesc[ _out_pos - 1 ].epos() - 1 ] && 
                     op_f_all_count != context.m_RecordDesc[ _func_pos - 1 ].eop_func() )
                    ||
                    ( csp_oflw_byte == group_rec.recBuf_gg00()[ context.m_RecordDesc[ _out_pos - 1 ].epos() - 1 ] && 
                      !( op_f_count     == context.m_RecordDesc[ _func_pos - 1 ].eop_func() || 
                         op_f_dis_count == context.m_RecordDesc[ _func_pos - 1 ].eop_func() ||
                         op_f_all_count == context.m_RecordDesc[ _func_pos - 1 ].eop_func())))
                {
                    SAPDBTRACE_STREAMWRITELN( _TraceStream, Join_Trace, 3, "NULL found: " << _out_pos );
                    _null_found = true;
                    context.m_acv.TransContext().trWarning_gg00.addElement(warn0_exist);
                    context.m_acv.TransContext().trWarning_gg00.addElement(warn2_null_in_builtin_func);
                }
            }
            if ( op_f_avg        == context.m_RecordDesc[ _func_pos - 1 ].eop_func() || 
                 op_f_dis_avg    == context.m_RecordDesc[ _func_pos - 1 ].eop_func() ||    
                 op_f_count      == context.m_RecordDesc[ _func_pos - 1 ].eop_func() ||    
                 op_f_dis_count  == context.m_RecordDesc[ _func_pos - 1 ].eop_func() ||    
                 op_f_all_count  == context.m_RecordDesc[ _func_pos - 1 ].eop_func() ||    
                 op_f_stddev     == context.m_RecordDesc[ _func_pos - 1 ].eop_func() ||    
                 op_f_dis_stddev == context.m_RecordDesc[ _func_pos - 1 ].eop_func() ||    
                 op_f_variance   == context.m_RecordDesc[ _func_pos - 1 ].eop_func() ||    
                 op_f_dis_variance == context.m_RecordDesc[ _func_pos - 1 ].eop_func() )
            {
                SAPDB_UInt4 _pos;
                tsp00_NumError _numerr;
                _numerr.becomes( num_ok );
                
                if ( op_f_avg        == context.m_RecordDesc[ _func_pos - 1 ].eop_func() || 
                     op_f_dis_avg    == context.m_RecordDesc[ _func_pos - 1 ].eop_func() ||    
                     op_f_stddev     == context.m_RecordDesc[ _func_pos - 1 ].eop_func() ||    
                     op_f_dis_stddev == context.m_RecordDesc[ _func_pos - 1 ].eop_func() ||    
                     op_f_variance   == context.m_RecordDesc[ _func_pos - 1 ].eop_func() ||    
                     op_f_dis_variance == context.m_RecordDesc[ _func_pos - 1 ].eop_func() )
                    _pos = context.m_RecordDesc[ _out_pos - 1 ].epos() +  NUMBER_MXGG04;
                else
                    // op_f_*_count
                    _pos = context.m_RecordDesc[ _out_pos - 1 ].epos();
                SAPDBTRACE_STREAMWRITELN( _TraceStream, Join_Trace, 3, "pos for VDN(0/1): " << _pos );
                if (( csp_undef_byte == group_rec.recBuf_gg00()[ context.m_RecordDesc[ _out_pos - 1 ].epos() - 1 ] && 
                     op_f_all_count != context.m_RecordDesc[ _func_pos - 1 ].eop_func() )
                    ||
                    ( csp_oflw_byte == group_rec.recBuf_gg00()[ context.m_RecordDesc[ _out_pos - 1 ].epos() - 1 ] && 
                      !( op_f_count     == context.m_RecordDesc[ _func_pos - 1 ].eop_func() || 
                         op_f_dis_count == context.m_RecordDesc[ _func_pos - 1 ].eop_func() ||
                         op_f_all_count == context.m_RecordDesc[ _func_pos - 1 ].eop_func())))
                {
                    // int(0) --> VDN number
                    SAPDBTRACE_STREAMWRITELN( _TraceStream, Join_Trace, 5, "VDN(0)" );
                    s41p4int( group_rec.recBuf_gg00(), _pos + 1, 0, _numerr );
                }
                else
                {
                    // int(1) --> VDN number
                    SAPDBTRACE_STREAMWRITELN( _TraceStream, Join_Trace, 5, "VDN(1)" );
                    s41p4int( group_rec.recBuf_gg00(), _pos + 1, 1, _numerr );
                }
                group_rec.recBuf_gg00()[ _pos - 1 ] = csp_defined_byte;
                if (( op_f_count     == context.m_RecordDesc[ _func_pos - 1 ].eop_func() || 
                      op_f_dis_count == context.m_RecordDesc[ _func_pos - 1 ].eop_func() ) 
                      &&
                      context.m_RecordDesc[ _out_pos - 1 ].elen_var() > RESNUM_MXSP00 )
                {
                    SAPDBTRACE_STREAMWRITELN( _TraceStream, Join_Trace, 5, "fill define bytes" );
                    SAPDB_RangeFill( __FILE__,   3, 
                          sizeof( group_rec ), &group_rec, _pos + RESNUM_MXSP00,
                          context.m_RecordDesc[ _out_pos - 1 ].elen_var() - RESNUM_MXSP00,
                          csp_defined_byte, _b_err );
                }
            }
            else
            {
                // op_f_min/max, op_f_[dis_]sum 
                // op_f_check_null
            }
        }
    }
    // result record columns processed
        if ( _check_null_pos > 0 )
        {
            if ( _null_found )
                group_rec.recBuf_gg00()[ _check_null_pos - 1 ] = csp_undef_byte;
            else
                group_rec.recBuf_gg00()[ _check_null_pos - 1 ] = csp_defined_byte;
        }
    SAPDBTRACE_IF( Join_Trace, 5, 
    t01buf( td_always, &group_rec, 1, group_rec.recLen_gg00() <= 300 ? group_rec.recLen_gg00() : 300 ));

    SAPDBTRACE_STREAMWRITELN( _TraceStream, Join_Trace, 3, "new_group returns: " << SAPDBTrace::BasisError(_b_err) );
    return _b_err;
}

/* ********************************************************* */
/*!
 *
 */
tgg00_BasisError update_group( 
        Join_AggregationContext &context,
        tgg00_Rec &record, tgg00_Rec &group_rec, SAPDB_Bool SourceIsPartialAggregate )
{
    SAPDBTRACE_ROUTINE_DEBUG( "Join_Aggregation::update_group", Join_Trace, 3 );
    SAPDBTRACE_STREAMVAR( _TraceStream, Join_Trace );
    tgg00_BasisError _b_err = e_ok;
    SAPDB_UInt4 _func_pos = context.m_RecordDesc[ 0 ].epos() - 1;
    SAPDB_UInt4 _out_pos, _check_null_pos = 0, _disfile_cnt = 0;
    bool _null_found = false;
    tgg00_StackOpFunc _operator;
    context.m_grouprec_changed = false;
    SAPDBTRACE_STREAMWRITELN( _TraceStream, Join_Trace, 3, "merge of aggregate: " << SourceIsPartialAggregate );
    SAPDBTRACE_STREAMWRITELN( _TraceStream, Join_Trace, 3, "input record:" );
    SAPDBTRACE_IF( Join_Trace, 3, 
    t01buf( td_always, &record, 1, record.recLen_gg00() <= 300 ? record.recLen_gg00() : 300 ));
    SAPDBTRACE_STREAMWRITELN( _TraceStream, Join_Trace, 3, "group record " );
    SAPDBTRACE_IF( Join_Trace, 3, 
    t01buf( td_always, &group_rec, 1, group_rec.recLen_gg00() <= 300 ? group_rec.recLen_gg00() : 300 ));

    // loop over functions
    while (( _func_pos - context.m_RecordDesc[ _func_pos - 1 ].epos() > 1) 
            && ( e_ok == _b_err ))
    {
        _func_pos -= context.m_RecordDesc[ _func_pos - 1 ].epos();
        // loop over LASTFUNCTION
        while (( st_func == context.m_RecordDesc[ _func_pos - 1 ].etype() ) 
                && ( op_f_none == context.m_RecordDesc[ _func_pos - 1 ].eop_func() ))
            _func_pos -= context.m_RecordDesc[ _func_pos - 1 ].epos();
        _out_pos = k721out_entry( (tgg00_StackList*) &context.m_RecordDesc, _func_pos );
        if ( st_func == context.m_RecordDesc[ _func_pos - 1 ].etype() )
        {
            _operator = context.m_RecordDesc[ _func_pos - 1 ].eop_func();
            SAPDBTRACE_STREAMWRITELN( _TraceStream, Join_Trace, 3, " func " << _operator << " on  " << _func_pos );
            if ( op_f_check_null == _operator )
            {
                _check_null_pos = context.m_RecordDesc[ _out_pos - 1 ].epos();
                SAPDBTRACE_STREAMWRITELN( _TraceStream, Join_Trace, 3, "_check_null_pos: " << _check_null_pos );
            }
            else
            {
                if ( op_f_dis_avg   == _operator ||
                     op_f_dis_sum   == _operator ||
                     op_f_dis_count == _operator ||
                     op_f_stddev    == _operator ||
                     op_f_dis_stddev == _operator ||
                     op_f_variance  == _operator ||
                     op_f_dis_variance == _operator )
                    ++_disfile_cnt;

                if ( csp_undef_byte != record.recBuf_gg00()[ context.m_RecordDesc[ _out_pos - 1 ].epos() - 1 ]  || 
                     op_f_all_count == _operator )
                {
                    bool _get_act_value = false;
    
                    if ( csp_oflw_byte == group_rec.recBuf_gg00()[ context.m_RecordDesc[ _out_pos - 1 ].epos() - 1 ] &&
                         op_f_all_count != _operator )
                    {
                        // overflow byte in group record
                        SAPDBTRACE_STREAMWRITELN( _TraceStream, Join_Trace, 5, "overflow byte" );
                        context.m_grouprec_changed = false;
                        // do nothing more
                    }
                    else
                    {
                        // no overflow byte in group record || op_f_all_count
                        if ( csp_oflw_byte == record.recBuf_gg00()[ context.m_RecordDesc[ _out_pos - 1 ].epos() - 1 ] &&
                             !( op_f_all_count == _operator ||
                                op_f_count     == _operator ||
                                op_f_dis_count == _operator ) ) 
                        {
                            // overflow byte w/o COUNT operator
                            context.m_grouprec_changed = true;
                            SAPDBTRACE_STREAMWRITELN( _TraceStream, Join_Trace, 5, "write oflw on: " << context.m_RecordDesc[ _out_pos - 1 ].epos() );
                            group_rec.recBuf_gg00()[ context.m_RecordDesc[ _out_pos - 1 ].epos() - 1 ] = csp_oflw_byte;
                            // do nothing more
                        }
                        else
                        {
                            // no null/overflow byte || op_f_*_count
                            if ( op_f_min == _operator || op_f_max == _operator )
                            {
                                if ( csp_undef_byte == group_rec.recBuf_gg00()[ context.m_RecordDesc[ _out_pos - 1 ].epos() - 1 ])
                                    _get_act_value = true;
                                else
                                {
                                    tsp00_LcompResult _lres;
                                    s30cmp( &record,  
                                            context.m_RecordDesc[ _out_pos - 1 ].epos() + 1, 
                                            context.m_RecordDesc[ _out_pos - 1 ].elen_var() - 1,
                                            &group_rec,
                                            context.m_RecordDesc[ _out_pos - 1 ].epos() + 1, 
                                            context.m_RecordDesc[ _out_pos - 1 ].elen_var() - 1,
                                            _lres );
                                    SAPDBTRACE_IF( Join_Trace, 3, 
                                    t01buf( td_always, &record, context.m_RecordDesc[ _out_pos - 1 ].epos(),  context.m_RecordDesc[ _out_pos - 1 ].epos() + context.m_RecordDesc[ _out_pos - 1 ].elen_var() - 1 ));
                                    SAPDBTRACE_STREAMWRITELN( _TraceStream, Join_Trace, 3, "comp res: " << _lres );
                                    SAPDBTRACE_IF( Join_Trace, 3, 
                                    t01buf( td_always, &group_rec, context.m_RecordDesc[ _out_pos - 1 ].epos(),  context.m_RecordDesc[ _out_pos - 1 ].epos() + context.m_RecordDesc[ _out_pos - 1 ].elen_var() - 1 ));
                                    if (( op_f_min == _operator && l_less == _lres ) ||
                                       ( op_f_max == _operator && l_greater == _lres ))
                                        _get_act_value = true;
                                }
                            }
                            else
                            {
                                if ( op_f_dis_avg   == _operator ||
                                     op_f_dis_sum   == _operator ||
                                     op_f_dis_count == _operator ||
                                     op_f_stddev    == _operator ||
                                     op_f_dis_stddev == _operator ||
                                     op_f_variance  == _operator ||
                                     op_f_dis_variance == _operator )
                                {
                                    tgg00_Rec     _dist_buffer;
                                    SAPDBTRACE_IF( Join_Trace, 3, SAPDB_MemFillNoCheck( &_dist_buffer, ':', sizeof( _dist_buffer ) ) );
                                    tgg00_FileId  _htree( context.m_DistinctFileId );
    
                                    // move key=<group value> to _dist_buffer
                                    SAPDB_RangeMove( __FILE__, 4,
                                            sizeof( group_rec ), sizeof(_dist_buffer),
                                            &group_rec.recBody_gg00(), 1,
                                            &_dist_buffer.recBody_gg00(), 1,
                                            group_rec.recKeyLen_gg00(), _b_err );
                                    // move value to aggregate to _dist_buffer
                                    SAPDB_RangeMove( __FILE__, 5,
                                            sizeof( record ), sizeof( _dist_buffer ),
                                            &record, context.m_RecordDesc[ _out_pos - 1 ].epos(),
                                            &_dist_buffer.recBuf_gg00(), 
                                            record.recKeyLen_gg00() - RESCNT_MXGG04 + cgg_rec_key_offset + 1,
                                            context.m_RecordDesc[ _out_pos - 1 ].elen_var(), 
                                            _b_err );
                                    // set key = <group value, value>
                                    _dist_buffer.recKeyLen_gg00() = group_rec.recKeyLen_gg00() 
                                        - RESCNT_MXGG04 
                                        + context.m_RecordDesc[ _out_pos - 1 ].elen_var();
                                    // set record length
                                    _dist_buffer.recLen_gg00() = 
                                        _dist_buffer.recKeyLen_gg00() + cgg_rec_key_offset;
                                    _dist_buffer.recVarcolOffset_gg00() = 0;
                                    _dist_buffer.recVarcolCnt_gg00() = 0;
                                    
                                    if ( op_f_stddev        == context.m_RecordDesc[ _func_pos - 1 ].eop_func() ||
                                         op_f_dis_stddev    == context.m_RecordDesc[ _func_pos - 1 ].eop_func() ||
                                         op_f_variance      == context.m_RecordDesc[ _func_pos - 1 ].eop_func() ||
                                         op_f_dis_variance  == context.m_RecordDesc[ _func_pos - 1 ].eop_func() )
                                    {
                                        // write 1 with swapping 
                                        SAPDBTRACE_STREAMWRITELN( _TraceStream, Join_Trace, 3, "write 1" );
                                        s20int4_to_buf_swap( 1, g01code.kernel_swap,
                                              _dist_buffer.recBody_gg00(),
                                              _dist_buffer.recKeyLen_gg00() + 1,
                                              sw_normal);
                                        _dist_buffer.recLen_gg00() += RESCNT_MXGG04;
                                    }
    
                                    a101_GetLogicalFilename ( context.m_acv.TransContext(), _htree);
                                    a101_SetTempFileIndex (context.m_acv, _htree, _disfile_cnt);
                                    a101_GetTempFileInstance ( context.m_acv.TransContext(), _htree);
    
                                    _htree.fileRoot_gg00() = context.m_DistinctFileRoot[ _disfile_cnt -1].m_Root;
                                    _htree.fileRootCheck_gg00() = context.m_DistinctFileRoot[ _disfile_cnt -1].m_RootCheck;
                                    if ( csp_oflw_byte != record.recBuf_gg00()[ context.m_RecordDesc[ _out_pos - 1 ].epos() - 1 ] )
                                    {
                                        if ( e_ok == _b_err )
                                        {
                                            SAPDBTRACE_STREAMWRITELN( _TraceStream, Join_Trace, 3, "add _dist_buffer" );
                                            SAPDBTRACE_IF( Join_Trace, 3, 
                                            t01buf( td_always, &_dist_buffer, 1, _dist_buffer.recLen_gg00() <= 300 ? _dist_buffer.recLen_gg00() : 300 ));
                                            b07cadd_record( context.m_acv.TransContext(), _htree, _dist_buffer );
                                            _b_err = context.m_acv.TransContext().trError_gg00;
                                            if ( e_duplicate_key == context.m_acv.TransContext().trError_gg00 )
                                                context.m_acv.TransContext().trError_gg00 = e_ok;
                                        }
    
                                        if ( e_duplicate_key == _b_err && 
                                           ( op_f_stddev   == _operator || 
                                             op_f_variance == _operator ))
                                        {
                                            tgg00_Lkey    _stddev_key;
                                            tgg00_FilePos _htreepos;
                                            _htreepos.tpsPno_gg00 = NIL_PAGE_NO_GG00; 
                                            _b_err = context.m_acv.TransContext().trError_gg00 = e_ok;
                                            _stddev_key.len() = _dist_buffer.recKeyLen_gg00();
                                            
                                            SAPDB_RangeMove( __FILE__, 6,
                                                    sizeof( _dist_buffer ), sizeof( _stddev_key.k() ),
                                                    &_dist_buffer, cgg_rec_key_offset + 1,
                                                    &_stddev_key.k(), 1,
                                                    _dist_buffer.recKeyLen_gg00(), 
                                                    _b_err );
                                            if ( e_ok == _b_err )
                                                b07ctget_record( context.m_acv.TransContext(), _htree, _htreepos, _stddev_key, _dist_buffer );
                                            // update counter
                                            s20int4_to_buf_swap( 1 +
                                                  s20buf_to_int4_swap( &_dist_buffer,
                                                  _dist_buffer.recKeyLen_gg00() + cgg_rec_key_offset + 1,
                                                  sw_normal, g01code.kernel_swap ),
                                                  g01code.kernel_swap,
                                                  _dist_buffer.recBody_gg00(),
                                                  _dist_buffer.recKeyLen_gg00() + 1, sw_normal);
                                            if ( e_ok == _b_err )
                                            {
                                                SAPDBTRACE_STREAMWRITELN( _TraceStream, Join_Trace, 3, "add _dist_buffer" );
                                                SAPDBTRACE_IF( Join_Trace, 3, 
                                                t01buf( td_always, &_dist_buffer, 1, _dist_buffer.recLen_gg00() <= 300 ? _dist_buffer.recLen_gg00() : 300 ));
                                                b07ctrepl_record( context.m_acv.TransContext(), _htree, _htreepos, _dist_buffer );
                                            }
                                        }
                                    }
                                    else
                                        // overflow byte found
                                        _b_err = e_duplicate_key;
                                    if ( e_duplicate_key == _b_err )
                                    {
                                        _b_err = context.m_acv.TransContext().trError_gg00 = e_ok;
                                    }
                                    else
                                    {
                                        if ( e_ok == _b_err )
                                        {
                                            switch ( _operator )
                                            {
                                                case op_f_dis_avg:
                                                case op_f_stddev:
                                                case op_f_dis_stddev:
                                                case op_f_variance:
                                                case op_f_dis_variance:
                                                           _operator.becomes(  op_f_avg ); break;
                                                case op_f_dis_sum:
                                                           _operator.becomes( op_f_sum ); break;
                                                case op_f_dis_count:
                                                           _operator.becomes( op_f_count ); break;
                                            }
                                        }
                                    }
                                }
                                else
                                {
                                    // op_f_avg, op_f_sum, op_f_count
                                    // op_f_all_count, op_f_check_null
                                }
                            }
                            tsp00_NumError _numerr;
                            _numerr.becomes( num_ok );
    
                            if (( op_f_avg == _operator || 
                                  op_f_sum == _operator ) 
                                  && e_ok == _b_err )
                            {
                                if ( csp_undef_byte == group_rec.recBuf_gg00()[ context.m_RecordDesc[ _out_pos - 1 ].epos() - 1 ] )
                                {
                                    // value for group is null
                                    _get_act_value = true;
                                }
                                else
                                {
                                    tsp00_NumError _numerr;
                                    SAPDB_Int4 _length;
                                    
                                    context.m_grouprec_changed = true;
                                    //if ( _out_pos + 1 != k721out_entry( (tgg00_StackList*) &context.m_RecordDesc, _out_pos ) )
                                    if ( st_result == context.m_RecordDesc[ _out_pos - 1 + 1 ].etype() )
                                    {
                                        s51add ( &record,
                                            /* new value to aggregate */
                                            context.m_RecordDesc[ _out_pos - 1 ].epos() + 1,
                                            context.m_RecordDesc[ _out_pos - 1 ].elen_var() - 1,
                                            /* old aggregate value */
                                            &group_rec,
                                            context.m_RecordDesc[ _out_pos - 1 ].epos() + 1,
                                            context.m_RecordDesc[ _out_pos - 1 ].elen_var() - 1,
                                            /* new aggregate value */
                                            &group_rec,
                                            context.m_RecordDesc[ _out_pos - 1 ].epos() + 1,
                                            context.m_RecordDesc[ _out_pos - 1 + 1 ].epos(),
                                            context.m_RecordDesc[ _out_pos - 1 + 1 ].elen_var(),
                                            _length, _numerr );
                                    }
                                    else
                                    {
                                        if (st_sum_length == context.m_RecordDesc[ _func_pos -2 ].etype())
                                        {
                                            s51add ( &record,
                                                /* new value to aggregate */
                                                context.m_RecordDesc[ _out_pos - 1 ].epos() + 1,
                                                context.m_RecordDesc[ _out_pos - 1 ].elen_var() - 1,
                                                /* old aggregate value */
                                                &group_rec,
                                                context.m_RecordDesc[ _out_pos - 1 ].epos() + 1,
                                                context.m_RecordDesc[ _out_pos - 1 ].elen_var() - 1,
                                                /* new aggregate value */
                                                &group_rec,
                                                context.m_RecordDesc[ _out_pos - 1 ].epos() + 1,
                                                context.m_RecordDesc[ _func_pos- 2 ].epos(),
                                                context.m_RecordDesc[ _func_pos- 2 ].elen_var(),
                                                _length, _numerr );
                                        }
                                        else
                                        {
                                            s51add ( &record,
                                                /* new value to aggregate */
                                                context.m_RecordDesc[ _out_pos - 1 ].epos() + 1,
                                                context.m_RecordDesc[ _out_pos - 1 ].elen_var() - 1,
                                                /* old aggregate value */
                                                &group_rec,
                                                context.m_RecordDesc[ _out_pos - 1 ].epos() + 1,
                                                context.m_RecordDesc[ _out_pos - 1 ].elen_var() - 1,
                                                /* new aggregate value */
                                                &group_rec,
                                                context.m_RecordDesc[ _out_pos - 1 ].epos() + 1,
                                                csp_fixed /* whole amount of numbers*/,
                                                csp_float_frac /* amount of fraction numbers of above */,
                                                _length, _numerr );
                                        }
                                    }
                                    if ( _numerr != num_ok )
                                    {
                                        // not all overflowes become num_overflow !!! 
                                        // i.e.:
                                        // 9.9999999999999999999999999999999999999E+62 
                                        // + 11 will fail !!! 
                                        // it produces pattern shown further down
                                        // try to sum up to the end 
                                        // don't stop other aggregation functions
                                        group_rec.recBuf_gg00()[ context.m_RecordDesc[ _out_pos - 1 ].epos() - 1     ] = csp_oflw_byte;
                                        group_rec.recBuf_gg00()[ context.m_RecordDesc[ _out_pos - 1 ].epos() - 1 + 1 ] = 0xFF;
                                        for ( SAPDB_Int1 _i = 1; _i <= context.m_RecordDesc[ _out_pos - 1 ].elen_var() - 2; ++_i )
                                            group_rec.recBuf_gg00()[ context.m_RecordDesc[ _out_pos - 1 ].epos() - 1 + 1 + _i ] = 0x99;
                                    }
                                }
                            }
                            if (( op_f_avg      == _operator || 
                                  op_f_count    == _operator || 
                                  op_f_all_count == _operator ) 
                               && e_ok == _b_err )
                            {
                                SAPDB_UInt4 _pos;
                                SAPDB_Int4  _cnt; 
                                SAPDB_Int4  _offset = 1; 
                                
                                context.m_grouprec_changed = true;
                                if ( op_f_avg == _operator )
                                    _pos = context.m_RecordDesc[ _out_pos - 1 ].epos() + NUMBER_MXGG04 + 1;
                                else
                                    _pos = context.m_RecordDesc[ _out_pos - 1 ].epos() + 1;
                                // get old aggregate value
                                s40g4int( &group_rec, _pos, _cnt, _numerr );
#ifdef                          SAPDB_SLOW
                                if ( num_ok != _numerr )
                                    RTE_Crash( SAPDBErr_Exception( __CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED, "(VDN conversion failed)"));
#endif
                                if ( SourceIsPartialAggregate )
                                {
                                    s40g4int( &record, _pos, _offset, _numerr );
                                    SAPDBTRACE_STREAMWRITELN( _TraceStream, Join_Trace, 3, "old aggregate: " << _offset );
#ifdef                              SAPDB_SLOW
                                    if ( num_ok != _numerr )
                                        RTE_Crash( SAPDBErr_Exception( __CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED, "(VDN conversion failed)"));
#endif
                                }
                                SAPDBTRACE_STREAMWRITELN( _TraceStream, Join_Trace, 3, "pos for VDN(): " << _pos << " old value: " << _cnt );
                                if ( num_ok == _numerr )
                                    s41p4int( &group_rec, _pos, _cnt + _offset, _numerr );
                            }
                            if ( _get_act_value )
                            {
                                SAPDBTRACE_STREAMWRITELN( _TraceStream, Join_Trace, 5, "copy act value" );
                                SAPDB_RangeMove( __FILE__, 7,
                                        sizeof( record ), sizeof( group_rec ),
                                        &record, 
                                        context.m_RecordDesc[ _out_pos - 1 ].epos(),
                                        &group_rec, 
                                        context.m_RecordDesc[ _out_pos - 1 ].epos(),
                                        context.m_RecordDesc[ _out_pos - 1 ].elen_var(), 
                                        _b_err );
                                context.m_grouprec_changed = true;
                            }
                        }
                    }
                }
                else
                {
                    // null value found && op != op_f_all_count
                   SAPDBTRACE_WRITELN( Join_Trace, 3, "NULL found: " << _out_pos );
                    _null_found = true;
                    context.m_acv.TransContext().trWarning_gg00.addElement(warn0_exist);
                    context.m_acv.TransContext().trWarning_gg00.addElement(warn2_null_in_builtin_func);
                }
            }
        }
    }
    // result record processed
    if ( _null_found && _check_null_pos > 0 )
    {
        if ( csp_undef_byte != group_rec.recBuf_gg00()[ _check_null_pos - 1 ] )
        {
            // previous NULL not found
            group_rec.recBuf_gg00()[ _check_null_pos - 1 ] = csp_undef_byte;
            context.m_grouprec_changed = true;
        }
    }
    SAPDBTRACE_IF( Join_Trace, 5, 
    t01buf( td_always, &group_rec, 1, group_rec.recLen_gg00() <= 300 ? group_rec.recLen_gg00() : 300 ));

    SAPDBTRACE_WRITELN( Join_Trace, 3, "update_group returns: " << SAPDBTrace::BasisError(_b_err) << "\t group changed: " << context.m_grouprec_changed );
    return _b_err;
}

/* ********************************************************* */
/*!
 *
 */
tgg00_BasisError post_processing( 
        Join_AggregationContext &context,
        tgg00_Rec &record, 
        const SAPDB_UInt4 result_cnt,
        const SAPDB_UInt4 later_output_pos,
        const Container_Vector<SAPDB_Int4> &rec_counter_pos,
        const tgg07_setfunc_handling_Enum post_processing_state )
{
    SAPDBTRACE_ROUTINE_DEBUG( "Join_Aggregation::post_processing", Join_Trace, 3 );

    context.m_grouprec_changed = false;
    SAPDB_UInt4 _disfile_cnt = 0;
    tgg00_BasisError b_err = e_ok;
    
    //++result_cnt;

    if ( !rec_counter_pos.IsEmpty() )
    {
        tsp00_NumError _ret;
        Container_Vector<SAPDB_Int4>::ConstIterator _iter;

        // put ROWNO
        for ( _iter = rec_counter_pos.Begin(); _iter != rec_counter_pos.End(); ++_iter) 
        {
            record.buf()[ *_iter - 1 ] = csp_defined_byte;
            s41plint( record.buf(), *_iter + 1,
                  csp_resnum_deflen, 0, result_cnt, _ret );
        }
        context.m_grouprec_changed = true;
    }

    SAPDBTRACE_IF( Join_Trace, 9, 
    t01buf( td_always, &record, 1, record.recLen_gg00() <= 300 ? record.recLen_gg00() : 300 ));
    SAPDB_UInt4 _func_pos = context.m_RecordDesc[ 0 ].epos() - 1;

    while ( _func_pos - context.m_RecordDesc[ _func_pos - 1 ].epos() > 1) 
    {
        _func_pos -= context.m_RecordDesc[ _func_pos - 1 ].epos();
        // loop over LASTFUNCTION
        while (( st_func == context.m_RecordDesc[ _func_pos - 1 ].etype() ) 
                && ( op_f_none == context.m_RecordDesc[ _func_pos - 1 ].eop_func() ))
            _func_pos -= context.m_RecordDesc[ _func_pos - 1 ].epos();
        SAPDB_UInt4 _out_pos = k721out_entry( (tgg00_StackList*) &context.m_RecordDesc, _func_pos );
        if ( st_func == context.m_RecordDesc[ _func_pos - 1 ].etype() )
        {
            SAPDBTRACE_WRITELN( Join_Trace, 3, " func " << context.m_RecordDesc[ _func_pos - 1 ].eop_func() << " on " << _func_pos );
            if  (
                (
                (op_f_avg       == context.m_RecordDesc[ _func_pos - 1 ].eop_func() || 
                op_f_dis_avg    == context.m_RecordDesc[ _func_pos - 1 ].eop_func() ||    
                op_f_sum        == context.m_RecordDesc[ _func_pos - 1 ].eop_func() ||    
                op_f_dis_sum    == context.m_RecordDesc[ _func_pos - 1 ].eop_func() )
                &&
                (sfh_all == post_processing_state)) 
                ||
                op_f_stddev     == context.m_RecordDesc[ _func_pos - 1 ].eop_func() ||    
                op_f_dis_stddev == context.m_RecordDesc[ _func_pos - 1 ].eop_func() ||    
                op_f_variance   == context.m_RecordDesc[ _func_pos - 1 ].eop_func() ||    
                op_f_dis_variance == context.m_RecordDesc[ _func_pos - 1 ].eop_func() 
                )
            {
                if  ( !( op_f_sum      == context.m_RecordDesc[ _func_pos - 1 ].eop_func() || 
                         op_f_dis_sum  == context.m_RecordDesc[ _func_pos - 1 ].eop_func() ))
                {
                    if ( csp_undef_byte != record.recBuf_gg00()[ context.m_RecordDesc[ _out_pos - 1 ].epos() - 1 ] && 
                         csp_oflw_byte  != record.recBuf_gg00()[ context.m_RecordDesc[ _out_pos - 1 ].epos() - 1 ] )
                    {
                        tsp00_NumError _numerr;
                        SAPDB_Int4 _length;
                        SAPDBTRACE_WRITELN( Join_Trace, 3, "s51div " );
#ifdef SAPDB_SLOW
                        SAPDBTRACE_IF ( Join_Trace, 5, t01buf( td_always, &record, 
                                context.m_RecordDesc[ _out_pos - 1 ].epos() + 1, 
                                context.m_RecordDesc[ _out_pos - 1 ].epos() + 1 + NUMBER_MXGG04 - 1 ));
                        SAPDBTRACE_IF ( Join_Trace, 5, t01buf( td_always, &record, 
                                context.m_RecordDesc[ _out_pos - 1 ].epos() + 1 + NUMBER_MXGG04, 
                                context.m_RecordDesc[ _out_pos - 1 ].epos() + 1 + NUMBER_MXGG04 + RESNUM_MXSP00 - 1 ));
#endif
                        s51div( &record,
                              context.m_RecordDesc[ _out_pos - 1 ].epos() + 1, 
                              NUMBER_MXGG04 - 1,
                              &record, 
                              context.m_RecordDesc[ _out_pos - 1 ].epos() + NUMBER_MXGG04 + 1,
                              RESNUM_MXSP00 - 1,
                              &record, 
                              context.m_RecordDesc[ _out_pos - 1 ].epos() + 1,
                              csp_fixed, csp_float_frac, _length, _numerr );
                        context.m_grouprec_changed = true;
                        if ( op_f_stddev     == context.m_RecordDesc[ _func_pos - 1 ].eop_func() ||    
                             op_f_dis_stddev == context.m_RecordDesc[ _func_pos - 1 ].eop_func() ||    
                             op_f_variance   == context.m_RecordDesc[ _func_pos - 1 ].eop_func() ||    
                             op_f_dis_variance == context.m_RecordDesc[ _func_pos - 1 ].eop_func() )
                        {
                            tgg00_FileId _htree( context.m_DistinctFileId );
                            tgg00_Rec _buf1, _buf2;
                            SAPDBTRACE_IF( Join_Trace, 5, SAPDB_MemFillNoCheck( &_buf1, ':', sizeof( _buf1 ) ) );
                            SAPDBTRACE_IF( Join_Trace, 5, SAPDB_MemFillNoCheck( &_buf2, ':', sizeof( _buf2 ) ) );
                            a101_GetLogicalFilename ( context.m_acv.TransContext(), _htree);
                            a101_SetTempFileIndex (context.m_acv, _htree, _disfile_cnt + 1);
                            a101_GetTempFileInstance ( context.m_acv.TransContext(), _htree);
                            _htree.fileRoot_gg00() = context.m_DistinctFileRoot[ _disfile_cnt ].m_Root;
                            _htree.fileRootCheck_gg00() = context.m_DistinctFileRoot[ _disfile_cnt ].m_RootCheck;
                            ++_disfile_cnt;
                            
                            k721stddev( &context.m_acv.TransContext(), _htree, 
                                    record, _buf1, &_buf2, 
                                    context.m_RecordDesc[ _func_pos - 1 ].eop_func(),
                                    context.m_RecordDesc[ _out_pos - 1 ].epos() );
                        }
                    }
                }
            }
        }
    }
    // call later output
    if ( later_output_pos > 0 )
    {
        SAPDBTRACE_WRITELN( Join_Trace, 3, "put 'later out': " 
                << context.m_RecordDesc[ later_output_pos - 1 - 1 ].epos() 
                << "[" << context.m_RecordDesc[ later_output_pos - 1 ].elen_var() << "]" 
                << " --> " << context.m_RecordDesc[ later_output_pos - 1 ].epos() );
        SAPDB_RangeMove( __FILE__, 8,
                sizeof( record ), sizeof( record ),
                &record, 
                context.m_RecordDesc[ later_output_pos - 1 - 1 ].epos(),
                &record, 
                context.m_RecordDesc[ later_output_pos - 1 ].epos(),
                context.m_RecordDesc[ later_output_pos - 1 ].elen_var(), 
                context.m_acv.TransContext().trError_gg00 );
        b_err = context.m_acv.TransContext().trError_gg00;
    }

    SAPDBTRACE_IF( Join_Trace, 5, 
    t01buf( td_always, &record, 1, record.recLen_gg00() <= 300 ? record.recLen_gg00() : 300 ));

    return b_err;
}

/* ********************************************************* */
/*!
 * analyses result record and detect ROWNO/ROWNUM positions
 */
tgg00_BasisError initialize( 
        SQLMan_Context &m_acv,
        tgg00_StackList &record_desc,
        tgg00_FileId &distinct_fileid,
        Container_Vector<Join_RootPair>  &distinct_fileroot,
        const tgg00_FileId &result_tree,
        SAPDB_UInt4 &later_output_pos,
        tgg07_setfunc_handling_Enum &post_processing_state )
{
    SAPDBTRACE_ROUTINE_DEBUG( "Join_Aggregation::initialize", Join_Trace, 3 );

    tgg00_BasisError _b_err = e_ok;
    SAPDB_UInt4 _i = 1;
    SAPDB_UInt4 _func_pos = record_desc[ 0 ].epos() - 1;
    SAPDB_UInt2 _func_cnt = 0, _out_pos;
    SAPDB_UInt4 _disfile_cnt = 0;
   
    // check for LASTFUNCTION
    if ( !( st_func == record_desc[ _func_pos - 1 ].etype() && 
          op_f_none == record_desc[ _func_pos - 1 ].eop_func() ))
        _b_err = e_stack_type_illegal;
    
    while (( _func_pos - record_desc[ _func_pos - 1 ].epos() > 1) 
            && ( e_ok == _b_err ))
    {
        _func_pos -= record_desc[ _func_pos - 1 ].epos();
        // loop over LASTFUNCTION
        while (( st_func == record_desc[ _func_pos - 1 ].etype() ) 
                && ( op_f_none == record_desc[ _func_pos - 1 ].eop_func() ))
            _func_pos -= record_desc[ _func_pos - 1 ].epos();
        if ( st_func == record_desc[ _func_pos - 1 ].etype() )
        {
            SAPDBTRACE_WRITELN( Join_Trace, 3, " func " << record_desc[ _func_pos - 1 ].eop_func() << " on  " << _func_pos );
            if ( op_f_check_null != record_desc[ _func_pos - 1 ].eop_func() )
            {
                ++_func_cnt; 
                if  (
                    (op_f_avg         == record_desc[ _func_pos - 1 ].eop_func() ||
                    op_f_dis_avg      == record_desc[ _func_pos - 1 ].eop_func() ||
                    op_f_stddev       == record_desc[ _func_pos - 1 ].eop_func() ||
                    op_f_dis_stddev   == record_desc[ _func_pos - 1 ].eop_func() ||
                    op_f_variance     == record_desc[ _func_pos - 1 ].eop_func() ||
                    op_f_dis_variance == record_desc[ _func_pos - 1 ].eop_func() )
                    &&
                    (
                    (1 == record_desc[ _func_pos - 1 ].ecol_tab()[0]) ||
                    /* selrec.selr_union OR       check for union has to be included */
                    ((ttfnUserResult_egg00 != bd998GetExtendedTempFileType(
                          m_acv.TransContext(),
                          result_tree)) &&
                     ( ttfnInto_egg00       != bd998GetExtendedTempFileType(
                         m_acv.TransContext(),
                         result_tree)))
                    )
                    )
                {
                    post_processing_state = sfh_all;
                }
                else
                {
                    if (op_f_stddev       == record_desc[ _func_pos - 1 ].eop_func() ||
                        op_f_dis_stddev   == record_desc[ _func_pos - 1 ].eop_func() ||
                        op_f_variance     == record_desc[ _func_pos - 1 ].eop_func() ||
                        op_f_dis_variance == record_desc[ _func_pos - 1 ].eop_func() )
                    { 
                        post_processing_state = sfh_stddev_variance;
                    }
                }
                if ( op_f_dis_avg     == record_desc[ _func_pos - 1 ].eop_func() ||
                    op_f_dis_sum      == record_desc[ _func_pos - 1 ].eop_func() ||
                    op_f_dis_count    == record_desc[ _func_pos - 1 ].eop_func() ||
                    op_f_stddev       == record_desc[ _func_pos - 1 ].eop_func() ||
                    op_f_dis_stddev   == record_desc[ _func_pos - 1 ].eop_func() ||
                    op_f_variance     == record_desc[ _func_pos - 1 ].eop_func() ||
                    op_f_dis_variance == record_desc[ _func_pos - 1 ].eop_func() )
                {
                    tgg00_FileId _htree( distinct_fileid );
                    
                    a101_GetLogicalFilename (m_acv.TransContext(), _htree);
                    a101_SetTempFileIndex (m_acv, _htree, ++_disfile_cnt);
                    _htree.fileRootCheck_gg00() = ROOT_CHECK_GG00;
                    a101_CreateGroupedTempFile (m_acv.TransContext(),
                        _htree,
                        a101_GetExtendedTempFileType (m_acv, _htree));
                    _b_err = m_acv.TransContext().trError_gg00;
                    if ( distinct_fileroot.IsFull() ) 
                        distinct_fileroot.Reserve( 2*distinct_fileroot.GetSize() );
                    if ( !distinct_fileroot.InsertEnd( Join_RootPair(_htree.fileRoot_gg00(), _htree.fileRootCheck_gg00()) ) )
                        return e_no_more_memory;
                }
                _out_pos = k721out_entry( (tgg00_StackList*) &record_desc, _func_pos );
                if ( op_o_output_hold == record_desc[ _out_pos - 1 ].eop_out() &&
                    op_o_output_later == record_desc[ _out_pos - 1 + 1 ].eop_out() )
                    later_output_pos = _out_pos + 1;
           }
        }
        else
            _b_err = e_stack_type_illegal;
    }
    SAPDBTRACE_WRITELN( Join_Trace, 3, "later_output_pos: " << later_output_pos );

    return _b_err;
}


} // namespace

