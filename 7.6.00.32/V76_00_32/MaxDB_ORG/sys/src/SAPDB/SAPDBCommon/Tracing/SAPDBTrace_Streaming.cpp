/*!
   @file SAPDBTrace_Streaming.cpp

   @brief C++ tracing (streaming) of types in SAPDB (implementation)
\if EMIT_LICENCE


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


\endif
*/

#include "SAPDBCommon/Tracing/SAPDBTrace_Streaming.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Hex.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_HexBuf.hpp"
#include "SQLManager/SQLMan_Types.hpp"
#include "SQLManager/Catalog/Catalog_Types.hpp"
#include "ggg01.h" // OMS errors
#ifdef KERNEL
#include "hgg01_3.h" /* &variant +kernel */
#endif

// void *
SAPDB_OStream & operator<<( SAPDB_OStream & os, void * value )
{
    os << (const char *)SAPDB_ToString( value, _T_p );
    return os;
}

namespace SAPDBTrace {
/***********************************************************************
 class BasisError
 **********************************************************************/
SAPDB_OStream& BasisError::ToStream( SAPDB_OStream& os ) const
{
    switch ( m_BasisError )
    {
        case e_ok:
            os << "e_ok";
            break;
        case e_cachedirectory_full:
            os << "e_cachedirectory_full";
            break;
        case e_append_not_allowed:
            os << "e_append_not_allowed";
            break;
        case e_bad_datapage:
            os << "e_bad_datapage";
            break;
        case e_bad_file:
            os << "e_bad_file";
            break;
        case e_bad_invfile:
            os << "e_bad_invfile";
            break;
        case e_buffer_limit:
            os << "e_buffer_limit";
            break;
        case e_cancelled:
            os << "e_cancelled";
            break;
        case e_do_not_access_fdir:
            os << "e_do_not_access_fdir";
            break;
        case e_do_not_drop_fdir:
            os << "e_do_not_drop_fdir";
            break;
        case e_duplicate_filename:
            os << "e_duplicate_filename";
            break;
        case e_duplicate_key:
            os << "e_duplicate_key";
            break;
        case e_duplicate_sysinfo:
            os << "e_duplicate_sysinfo";
            break;
        case e_duplicate_record:
            os << "e_duplicate_record";
            break;
        case e_file_empty:
            os << "e_file_empty";
            break;
        case e_file_limit:
            os << "e_file_limit";
            break;
        case e_file_must_be_read_only:
            os << "e_file_must_be_read_only";
            break;
        case e_file_not_accessible:
            os << "e_file_not_accessible";
            break;
        case e_file_not_found:
            os << "e_file_not_found";
            break;
        case e_file_read_only:
            os << "e_file_read_only";
            break;
        case e_file_unloaded:
            os << "e_file_unloaded";
            break;
        case e_illegal_filename:
            os << "e_illegal_filename";
            break;
        case e_illegal_key:
            os << "e_illegal_key";
            break;
        case e_illegal_record:
            os << "e_illegal_record";
            break;
        case e_incompatible_savefile:
            os << "e_incompatible_savefile";
            break;
        case e_init_missing:
            os << "e_init_missing";
            break;
        case e_inv_list_not_found:
            os << "e_inv_list_not_found";
            break;
        case e_inv_not_found:
            os << "e_inv_not_found";
            break;
        case e_invalid_invlistpos:
            os << "e_invalid_invlistpos";
            break;
        case e_invalid_mix_dml_ddl:
            os << "e_invalid_mix_dml_ddl";
            break;
        case e_invalid_root:
            os << "e_invalid_root";
            break;
        case e_key_not_found:
            os << "e_key_not_found";
            break;
        case e_primkey_not_found:
            os << "e_primkey_not_found";
            break;
        case e_no_more_temp_space:
            os << "e_no_more_temp_space";
            break;
        case e_no_more_perm_space:
            os << "e_no_more_temp_space";
            break;
        case e_no_more_memory:
            os << "e_no_more_memory";
            break;
        case e_no_next_invkey:
            os << "e_no_next_invkey";
            break;
        case e_no_next_record:
            os << "e_no_next_record";
            break;
        case e_no_prev_invkey:
            os << "e_no_prev_invkey";
            break;
        case e_no_prev_record:
            os << "e_no_prev_record";
            break;
        case e_old_fileversion:
            os << "e_old_fileversion";
            break;
        case e_old_leafnodes_number:
            os << "e_old_leafnodes_number";
            break;
        case e_old_parsid:
            os << "e_old_parsid";
            break;
        case e_shutdown:
            os << "e_shutdown";
            break;
        case e_sysbuf_storage_exceeded:
            os << "e_sysbuf_storage_exceeded";
            break;
        case e_sequence_overflow:
            os << "e_sequence_overflow";
            break;
        case e_currval_without_nextval:
            os << "e_currval_without_nextval";
            break;
        case e_sysbuffer_overflow:
            os << "e_sysbuffer_overflow";
            break;
        case e_sysinfo_not_found:
            os << "e_sysinfo_not_found";
            break;
        case e_unknown_strategy:
            os << "e_unknown_strategy";
            break;
        case e_enough_resultsets:
            os << "e_enough_resultsets";
            break;
        case e_invalid_date:
            os << "e_invalid_date";
            break;
        case e_end_of_month_adjustment:
            os << "e_end_of_month_adjustment";
            break;
        case e_time_value_too_long:
            os << "e_time_value_too_long";
            break;
        case e_invalid_time:
            os << "e_invalid_time";
            break;
        case e_invalid_timestamp:
            os << "e_invalid_timestamp";
            break;
        case e_invalid_day_of_week:
            os << "e_invalid_day_of_week";
            break;
        case e_invalid_pattern:
            os << "e_invalid_pattern";
            break;
        case e_request_timeout:
            os << "e_request_timeout";
            break;
        case e_too_many_resultsets:
            os << "e_too_many_resultsets";
            break;
        case e_too_many_serials:
            os << "e_too_many_serials";
            break;
        case e_too_many_updatesets:
            os << "e_too_many_updatesets";
            break;
        case e_too_many_differences:
            os << "e_too_many_differences";
            break;
        case e_too_many_mb_stackentries:
            os << "e_too_many_mb_stackentries";
            break;
        case e_too_many_mb_data:
            os << "e_too_many_mb_data";
            break;
        case e_too_small_mb_qual_part:
            os << "e_too_small_mb_qual_part";
            break;
        case e_too_small_packet_size:
            os << "e_too_small_packet_size";
            break;
        case e_too_complicated_constr:
            os << "e_too_complicated_constr";
            break;
        case e_too_long_viewtext:
            os << "e_too_long_viewtext";
            break;
        case e_too_many_named_subtrans:
            os << "e_too_many_named_subtrans";
            break;
        case e_too_many_intern_cmd:
            os << "e_too_many_intern_cmd";
            break;
        case e_too_many_intern_parsinfos:
            os << "e_too_many_intern_parsinfos";
            break;
        case e_too_many_nested_functions:
            os << "e_too_many_nested_functions";
            break;
        case e_too_long_corelated_row:
            os << "e_too_long_corelated_row";
            break;
        case e_too_many_internal_columns:
            os << "e_too_many_internal_columns";
            break;
        case e_too_long_trigger_params:
            os << "e_too_long_trigger_params";
            break;
        case e_too_complex_trigger_cond:
            os << "e_too_complex_trigger_cond";
            break;
        case e_too_many_statements_in_trigger:
            os << "e_too_many_statements_in_trigger";
            break;
        case e_too_many_mb_strat:
            os << "e_too_many_mb_strat";
            break;
        case e_bad_hostfile_page:
            os << "e_bad_hostfile_page";
            break;
        case e_cmd_timeout_invalid:
            os << "e_cmd_timeout_invalid";
            break;
        case e_column_trunc:
            os << "e_column_trunc";
            break;
        case e_date_time_format_invalid:
            os << "e_date_time_format_invalid";
            break;
        case e_devname_invalid:
            os << "e_devname_invalid";
            break;
        case e_devsize_invalid:
            os << "e_devsize_invalid";
            break;
        case e_devsize_too_small:
            os << "e_devsize_too_small";
            break;
        case e_duplicate_invkey:
            os << "e_duplicate_invkey";
            break;
        case e_not_implemented:
            os << "e_not_implemented";
            break;
        case e_hex0_column_tail:
            os << "e_hex0_column_tail";
            break;
        case e_hostfile_error:
            os << "e_hostfile_error";
            break;
        case e_incomplete_logsegm:
            os << "e_incomplete_logsegm";
            break;
        case e_invalid_blocksize:
            os << "e_invalid_blocksize";
            break;
        case e_incompatible_log:
            os << "e_incompatible_log";
            break;
        case e_inconsistent_nodetype:
            os << "e_inconsistent_nodetype";
            break;
        case e_invalid:
            os << "e_invalid";
            break;
        case e_key_update:
            os << "e_key_update";
            break;
        case e_conf_parameter_error:
            os << "e_conf_parameter_error";
            break;
        case e_logpage_not_found:
            os << "e_logpage_not_found";
            break;
        case e_net_line_down:
            os << "e_net_line_down";
            break;
        case e_new_hostfile_required:
            os << "e_new_hostfile_required";
            break;
        case e_end_of_backup:
            os << "e_end_of_backup";
            break;
        case e_no_data_on_tape:
            os << "e_no_data_on_tape";
            break;
        case e_no_servers:
            os << "e_no_servers";
            break;
        case e_save_skipped:
            os << "e_save_skipped";
            break;
        case e_null_value_illegal:
            os << "e_null_value_illegal";
            break;
        case e_num_invalid:
            os << "e_num_invalid";
            break;
        case e_num_overflow:
            os << "e_num_overflow";
            break;
        case e_program_stack_overflow:
            os << "e_program_stack_overflow";
            break;
        case e_qual_violation:
            os << "e_qual_violation";
            break;
        case e_range_violation:
            os << "e_range_violation";
            break;
        case e_rec_not_locked:
            os << "e_rec_not_locked";
            break;
        case e_release_caused_by_shutdown:
            os << "e_release_caused_by_shutdown";
            break;
        case e_response_timeout:
            os << "e_response_timeout";
            break;
        case e_rollback_wanted:
            os << "e_rollback_wanted";
            break;
        case e_skip_upd:
            os << "e_skip_upd";
            break;
        case e_skip_key_upd:
            os << "e_skip_key_upd";
            break;
        case e_special_null:
            os << "e_special_null";
            break;
        case e_stack_op_illegal:
            os << "e_stack_op_illegal";
            break;
        case e_stack_overflow:
            os << "e_stack_overflow";
            break;
        case e_stack_type_illegal:
            os << "e_stack_type_illegal";
            break;
        case e_string_column_empty:
            os << "e_string_column_empty";
            break;
        case e_work_rolled_back:
            os << "e_work_rolled_back";
            break;
        case e_rollb_log_full:
            os << "e_rollb_log_full";
            break;
        case e_too_many_devspaces:
            os << "e_too_many_devspaces";
            break;
        case e_too_many_lockunits_specified:
            os << "e_too_many_lockunits_specified";
            break;
        case e_too_many_lock_requests:
            os << "e_too_many_lock_requests";
            break;
        case e_lock_collision:
            os << "e_lock_collision";
            break;
        case e_lock_collision_with_inserted_row:
            os << "e_lock_collision_with_inserted_row";
            break;
        case e_lock_dirty:
            os << "e_lock_dirty";
            break;
        case e_upd_view_violation:
            os << "e_upd_view_violation";
            break;
        case e_view_violation:
            os << "e_view_violation";
            break;
        case e_wait_for_lock_release:
            os << "e_wait_for_lock_release";
            break;
        case e_wrong_hostfile:
            os << "e_wrong_hostfile";
            break;
        case e_no_log_to_save:
            os << "e_no_log_to_save";
            break;
        case e_backup_is_running:
            os << "e_backup_is_running";
            break;
        case e_backup_not_running:
            os << "e_backup_not_running";
            break;
        case e_wrong_password:
            os << "e_wrong_password";
            break;
        case e_dcom_hresult:
            os << "e_dcom_hresult";
            break;
        case e_invalid_label:
            os << "e_invalid_label";
            break;
        case e_invalid_configuration:
            os << "e_invalid_configuration";
            break;
        case e_write_task_crash:
            os << "e_write_task_crash";
            break;
        case e_queue_mismatch:
            os << "e_queue_mismatch";
            break;
        case e_bad_pagetype:
            os << "e_bad_pagetype";
            break;
        case e_invalid_columnname:
            os << "e_invalid_columnname";
            break;
        case e_invalid_end_of_command:
            os << "e_invalid_end_of_command";
            break;
        case e_invalid_datatype:
            os << "e_invalid_datatype";
            break;
        case e_invalid_filetype:
            os << "e_invalid_filetype";
            break;
        case e_identifier_too_long:
            os << "e_identifier_too_long";
            break;
        case e_invalid_indexorder:
            os << "e_invalid_indexorder";
            break;
        case e_invalid_keyword:
            os << "e_invalid_keyword";
            break;
        case e_expr_in_insert_value_list:
            os << "e_expr_in_insert_value_list";
            break;
        case e_invalid_mix_functions_columns:
            os << "e_invalid_mix_functions_columns";
            break;
        case e_invalid_tablename:
            os << "e_invalid_tablename";
            break;
        case e_invalid_username:
            os << "e_invalid_username";
            break;
        case e_invalid_messagetype:
            os << "e_invalid_messagetype";
            break;
        case e_in_view_not_allowed:
            os << "e_in_view_not_allowed";
            break;
        case e_parameter_not_allowed:
            os << "e_parameter_not_allowed";
            break;
        case e_missing_constant:
            os << "e_missing_constant";
            break;
        case e_missing_identifier:
            os << "e_missing_identifier";
            break;
        case e_missing_integer:
            os << "e_missing_integer";
            break;
        case e_missing_keyword:
            os << "e_missing_keyword";
            break;
        case e_wanted_keyword:
            os << "e_wanted_keyword";
            break;
        case e_missing_number:
            os << "e_missing_number";
            break;
        case e_missing_password:
            os << "e_missing_password";
            break;
        case e_missing_delimiter:
            os << "e_missing_delimiter";
            break;
        case e_missing_string_literal:
            os << "e_missing_string_literal";
            break;
        case e_missing_value_spec:
            os << "e_missing_value_spec";
            break;
        case e_reserved_identifier:
            os << "e_reserved_identifier";
            break;
        case e_subquery_not_allowed:
            os << "e_subquery_not_allowed";
            break;
        case e_query_not_allowed:
            os << "e_query_not_allowed";
            break;
        case e_too_many_monadic_operators:
            os << "e_too_many_monadic_operators";
            break;
        case e_rowno_not_allowed:
            os << "e_rowno_not_allowed";
            break;
        case e_invalid_unsign_integer:
            os << "e_invalid_unsign_integer";
            break;
        case e_update_not_allowed:
            os << "e_update_not_allowed";
            break;
        case e_synonym_not_allowed:
            os << "e_synonym_not_allowed";
            break;
        case e_alter_column_not_allowed:
            os << "e_alter_column_not_allowed";
            break;
        case e_alter_drop_user_not_allowed:
            os << "e_alter_drop_user_not_allowed";
            break;
        case e_check_option_not_allowed:
            os << "e_check_option_not_allowed";
            break;
        case e_column_indexed:
            os << "e_column_indexed";
            break;
        case e_column_must_be_keycolumn:
            os << "e_column_must_be_keycolumn";
            break;
        case e_col_must_be_no_keycolumn:
            os << "e_col_must_be_no_keycolumn";
            break;
        case e_column_must_have_default:
            os << "e_column_must_have_default";
            break;
        case e_column_not_indexed:
            os << "e_column_not_indexed";
            break;
        case e_command_for_string_not_allow:
            os << "e_command_for_string_not_allow";
            break;
        case e_command_timeout:
            os << "e_command_timeout";
            break;
        case e_const_incompatible_with_typ:
            os << "e_const_incompatible_with_typ";
            break;
        case e_corelated_subquery_not_allowe:
            os << "e_corelated_subquery_not_allowe";
            break;
        case e_costlimit_overflow:
            os << "e_costlimit_overflow";
            break;
        case e_costwarning_overflow:
            os << "e_costwarning_overflow";
            break;
        case e_current_of_needs_fetch:
            os << "e_current_of_needs_fetch";
            break;
        case e_default_not_allowed:
            os << "e_default_not_allowed";
            break;
        case e_default_not_in_range:
            os << "e_default_not_in_range";
            break;
        case e_default_spec_not_allowed:
            os << "e_default_spec_not_allowed";
            break;
        case e_duplicate_columnname:
            os << "e_duplicate_columnname";
            break;
        case e_duplicate_constraint:
            os << "e_duplicate_constraint";
            break;
        case e_duplicate_default:
            os << "e_duplicate_default";
            break;
        case e_duplicate_indexname:
            os << "e_duplicate_indexname";
            break;
        case e_duplicate_name:
            os << "e_duplicate_name";
            break;
        case e_duplicate_privilege:
            os << "e_duplicate_privilege";
            break;
        case e_duplicate_referencename:
            os << "e_duplicate_referencename";
            break;
        case e_duplicate_resultname:
            os << "e_duplicate_resultname";
            break;
        case e_duplicate_tablename:
            os << "e_duplicate_tablename";
            break;
        case e_duplicate_trigger_def:
            os << "e_duplicate_trigger_def";
            break;
        case e_duplicate_value:
            os << "e_duplicate_value";
            break;
        case e_foreign_key_must_exist:
            os << "e_foreign_key_must_exist";
            break;
        case e_foreign_key_violation:
            os << "e_foreign_key_violation";
            break;
        case e_from_select_not_allowed:
            os << "e_from_select_not_allowed";
            break;
        case e_function_not_allowed:
            os << "e_function_not_allowed";
            break;
        case e_group_user_not_allowed:
            os << "e_group_user_not_allowed";
            break;
        case e_incompatible_datatypes:
            os << "e_incompatible_datatypes";
            break;
        case e_indexed_table_not_allowed:
            os << "e_indexed_table_not_allowed";
            break;
        case e_indexname_must_be_unique:
            os << "e_indexname_must_be_unique";
            break;
        case e_invalid_cachelimit_size:
            os << "e_invalid_cachelimit_size";
            break;
        case e_invalid_char_position:
            os << "e_invalid_char_position";
            break;
        case e_invalid_command:
            os << "e_invalid_command";
            break;
        case e_invalid_costwarning:
            os << "e_invalid_costwarning";
            break;
        case e_invalid_datalength:
            os << "e_invalid_datalength";
            break;
        case e_invalid_length_of_datatyp:
            os << "e_invalid_length_of_datatyp";
            break;
        case e_invalid_messcodetype:
            os << "e_invalid_messcodetype";
            break;
        case e_invalid_number_variable:
            os << "e_invalid_number_variable";
            break;
        case e_invalid_parameter:
            os << "e_invalid_parameter";
            break;
        case e_invalid_range:
            os << "e_invalid_range";
            break;
        case e_invalid_sequence:
            os << "e_invalid_sequence";
            break;
        case e_invalid_subtrans_structure:
            os << "e_invalid_subtrans_structure";
            break;
        case e_invalid_table_content:
            os << "e_invalid_table_content";
            break;
        case e_invalid_view_column:
            os << "e_invalid_view_column";
            break;
        case e_join_fields_too_long:
            os << "e_join_fields_too_long";
            break;
        case e_key_does_not_exist:
            os << "e_key_does_not_exist";
            break;
        case e_key_not_allowed:
            os << "e_key_not_allowed";
            break;
        case e_link_column_not_allowed:
            os << "e_link_column_not_allowed";
            break;
        case e_link_cycle_not_allowed:
            os << "e_link_cycle_not_allowed";
            break;
        case e_link_rule_not_allowed:
            os << "e_link_rule_not_allowed";
            break;
        case e_link_rule_violation:
            os << "e_link_rule_violation";
            break;
        case e_lock_row_not_allowed:
            os << "e_lock_row_not_allowed";
            break;
        case e_role_cycle:
            os << "e_role_cycle";
            break;
        case e_mandatory_field_is_null:
            os << "e_mandatory_field_is_null";
            break;
        case e_missing_basetable:
            os << "e_missing_basetable";
            break;
        case e_missing_column_definite:
            os << "e_missing_column_definite";
            break;
        case e_missing_tablename_definite:
            os << "e_missing_tablename_definite";
            break;
        case e_missing_keyfield:
            os << "e_missing_keyfield";
            break;
        case e_missing_mandatory_recordfield:
            os << "e_missing_mandatory_recordfield";
            break;
        case e_missing_privilege:
            os << "e_missing_privilege";
            break;
        case e_missing_reuseable:
            os << "e_missing_reuseable";
            break;
        case e_missing_update_column:
            os << "e_missing_update_column";
            break;
        case e_missing_viewtable:
            os << "e_missing_viewtable";
            break;
        case e_new_session_required:
            os << "e_new_session_required";
            break;
        case e_no_more_user_temp_space:
            os << "e_no_more_user_temp_space";
            break;
        case e_no_open_subtrans:
            os << "e_no_open_subtrans";
            break;
        case e_no_resulttable_allowed:
            os << "e_no_resulttable_allowed";
            break;
        case e_not_allowed_in_temp_table:
            os << "e_not_allowed_in_temp_table";
            break;
        case e_not_null_not_allowed:
            os << "e_not_null_not_allowed";
            break;
        case e_null_not_allowed:
            os << "e_null_not_allowed";
            break;
        case e_num_truncated:
            os << "e_num_truncated";
            break;
        case e_one_output_field_too_long:
            os << "e_one_output_field_too_long";
            break;
        case e_only_one_recordtype_allowed:
            os << "e_only_one_recordtype_allowed";
            break;
        case e_order_col_must_be_number:
            os << "e_order_col_must_be_number";
            break;
        case e_outer_join_def_error:
            os << "e_outer_join_def_error";
            break;
        case e_outer_join_not_allowed:
            os << "e_outer_join_not_allowed";
            break;
        case e_output_columns_too_long:
            os << "e_output_columns_too_long";
            break;
        case e_output_field_must_be_group:
            os << "e_output_field_must_be_group";
            break;
        case e_owner_must_be_specified:
            os << "e_owner_must_be_specified";
            break;
        case e_procedure_must_be_recompiled:
            os << "e_procedure_must_be_recompiled";
            break;
        case e_referenced_index_not_allowed:
            os << "e_referenced_index_not_allowed";
            break;
        case e_remote_load_not_allowed:
            os << "e_remote_load_not_allowed";
            break;
        case e_restore_catalog_error:
            os << "e_restore_catalog_error";
            break;
        case e_bd_page_collision:
            os << "e_bd_page_collision";
            break;
        case e_row_not_found:
            os << "e_row_not_found";
            break;
        case e_select_col_list_too_long:
            os << "e_select_col_list_too_long";
            break;
        case e_st_col_not_open:
            os << "e_st_col_not_open";
            break;
        case e_st_col_open_read_only:
            os << "e_st_col_open_read_only";
            break;
        case e_st_invalid_destpos:
            os << "e_st_invalid_destpos";
            break;
        case e_st_invalid_length:
            os << "e_st_invalid_length";
            break;
        case e_st_invalid_pos:
            os << "e_st_invalid_pos";
            break;
        case e_st_invalid_startpos:
            os << "e_st_invalid_startpos";
            break;
        case e_system_error:
            os << "e_system_error";
            break;
        case e_table_is_unloaded:
            os << "e_table_is_unloaded";
            break;
        case e_table_must_be_temp:
            os << "e_table_must_be_temp";
            break;
        case e_table_not_in_from_list:
            os << "e_table_not_in_from_list";
            break;
        case e_temp_table_not_allowed:
            os << "e_temp_table_not_allowed";
            break;
        case e_too_few_columns:
            os << "e_too_few_columns";
            break;
        case e_too_few_values:
            os << "e_too_few_values";
            break;
        case e_too_long_key:
            os << "e_too_long_key";
            break;
        case e_too_long_record:
            os << "e_too_long_record";
            break;
        case e_too_many_cols_dropped:
            os << "e_too_many_cols_dropped";
            break;
        case e_too_many_columns:
            os << "e_too_many_columns";
            break;
        case e_too_many_constraints:
            os << "e_too_many_constraints";
            break;
        case e_too_many_corr_tables:
            os << "e_too_many_corr_tables";
            break;
        case e_too_many_correlation_columns:
            os << "e_too_many_correlation_columns";
            break;
        case e_too_many_indices:
            os << "e_too_many_indices";
            break;
        case e_too_many_joins:
            os << "e_too_many_joins";
            break;
        case e_too_many_order_columns:
            os << "e_too_many_order_columns";
            break;
        case e_too_many_sec_links:
            os << "e_too_many_sec_links";
            break;
        case e_too_many_sourcetables:
            os << "e_too_many_sourcetables";
            break;
        case e_too_many_subqueries:
            os << "e_too_many_subqueries";
            break;
        case e_too_many_subtrans:
            os << "e_too_many_subtrans";
            break;
        case e_too_many_values:
            os << "e_too_many_values";
            break;
        case e_too_many_variables:
            os << "e_too_many_variables";
            break;
        case e_too_old_tool:
            os << "e_too_old_tool";
            break;
        case e_too_short_datapart:
            os << "e_too_short_datapart";
            break;
        case e_invalid_segment_construction:
            os << "e_invalid_segment_construction";
            break;
        case e_union_cols_mustbe_compatible:
            os << "e_union_cols_mustbe_compatible";
            break;
        case e_unknown_name:
            os << "e_unknown_name";
            break;
        case e_unknown_columnname:
            os << "e_unknown_columnname";
            break;
        case e_unknown_constraint:
            os << "e_unknown_constraint";
            break;
        case e_unknown_default:
            os << "e_unknown_default";
            break;
        case e_unknown_domainname:
            os << "e_unknown_domainname";
            break;
        case e_unknown_functionname:
            os << "e_unknown_functionname";
            break;
        case e_unknown_groupname:
            os << "e_unknown_groupname";
            break;
        case e_unknown_indexname:
            os << "e_unknown_indexname";
            break;
        case e_unknown_linkname:
            os << "e_unknown_linkname";
            break;
        case e_unknown_mapset:
            os << "e_unknown_mapset";
            break;
        case e_unknown_ordercolumn:
            os << "e_unknown_ordercolumn";
            break;
        case e_unknown_parseid:
            os << "e_unknown_parseid";
            break;
        case e_unknown_password:
            os << "e_unknown_password";
            break;
        case e_unknown_procedure_name:
            os << "e_unknown_procedure_name";
            break;
        case e_unknown_resultname:
            os << "e_unknown_resultname";
            break;
        case e_unknown_sequencename:
            os << "e_unknown_sequencename";
            break;
        case e_unknown_name_of_serverdb:
            os << "e_unknown_name_of_serverdb";
            break;
        case e_unknown_session:
            os << "e_unknown_session";
            break;
        case e_unknown_synonymname:
            os << "e_unknown_synonymname";
            break;
        case e_unknown_tablename:
            os << "e_unknown_tablename";
            break;
        case e_unknown_multibyte_set:
            os << "e_unknown_multibyte_set";
            break;
        case e_unknown_trigger:
            os << "e_unknown_trigger";
            break;
        case e_unknown_user:
            os << "e_unknown_user";
            break;
        case e_unknown_user_password_pair:
            os << "e_unknown_user_password_pair";
            break;
        case e_use_cascade:
            os << "e_use_cascade";
            break;
        case e_use_fetch_rest:
            os << "e_use_fetch_rest";
            break;
        case e_user_already_connected:
            os << "e_user_already_connected";
            break;
        case e_user_connected_to_same_task:
            os << "e_user_connected_to_same_task";
            break;
        case e_user_not_allowed:
            os << "e_user_not_allowed";
            break;
        case e_user_not_connected:
            os << "e_user_not_connected";
            break;
        case e_value_expr_must_be_param_name:
            os << "e_value_expr_must_be_param_name";
            break;
        case e_varchar_not_allowed:
            os << "e_varchar_not_allowed";
            break;
        case e_variable_not_allowed:
            os << "e_variable_not_allowed";
            break;
        case e_view_col_list_too_long:
            os << "e_view_col_list_too_long";
            break;
        case e_view_def_contradicts_table_de:
            os << "e_view_def_contradicts_table_de";
            break;
        case e_without_datatypes:
            os << "e_without_datatypes";
            break;
        case e_wrong_database_mode:
            os << "e_wrong_database_mode";
            break;
        case e_wrong_database_instance:
            os << "e_wrong_database_instance";
            break;
        case e_wrong_application:
            os << "e_wrong_application";
            break;
        case e_dbm_command_not_possible:
            os << "e_dbm_command_not_possible";
            break;
        case e_illegal_escape_sequence:
            os << "e_illegal_escape_sequence";
            break;
        case e_invalid_escape_char:
            os << "e_invalid_escape_char";
            break;
        case e_unitrans_src_too_short:
            os << "e_unitrans_src_too_short";
            break;
        case e_unitrans_dest_too_short:
            os << "e_unitrans_dest_too_short";
            break;
        case e_not_translatable:
            os << "e_not_translatable";
            break;
        case e_not_trans_to_uni:
            os << "e_not_trans_to_uni";
            break;
        case e_not_trans_from_uni:
            os << "e_not_trans_from_uni";
            break;
        case e_autosave_running:
            os << "e_autosave_running";
            break;
        case e_to_date_duplicate_format:
            os << "e_to_date_duplicate_format";
            break;
        case e_no_to_date_format:
            os << "e_no_to_date_format";
            break;
        case e_date_format_not_recognized:
            os << "e_date_format_not_recognized";
            break;
        case e_to_date_too_long_input:
            os << "e_to_date_too_long_input";
            break;
        case e_to_date_inconsistent:
            os << "e_to_date_inconsistent";
            break;
        case e_to_date_invalid_day:
            os << "e_to_date_invalid_day";
            break;
        case e_to_date_too_short_input:
            os << "e_to_date_too_short_input";
            break;
        case e_to_date_out_of_range:
            os << "e_to_date_out_of_range";
            break;
        case e_unknown_month:
            os << "e_unknown_month";
            break;
        case e_unknown_day:
            os << "e_unknown_day";
            break;
        case e_unknown_meridian:
            os << "e_unknown_meridian";
            break;
        case e_invalid_timezone:
            os << "e_invalid_timezone";
            break;
        case e_to_date_no_number:
            os << "e_to_date_no_number";
            break;
        case e_to_date_no_letter:
            os << "e_to_date_no_letter";
            break;
        case e_number_format_not_recogniz:
            os << "e_number_format_not_recogniz";
            break;
        case e_too_many_precision_specs:
            os << "e_too_many_precision_specs";
            break;
        case e_bad_precision_spec:
            os << "e_bad_precision_spec";
            break;
        case e_new_disk_not_accessible:
            os << "e_new_disk_not_accessible";
            break;
        case e_standby_info_not_ok:
            os << "e_standby_info_not_ok";
            break;
        case e_root_check:
            os << "e_root_check";
            break;
        case e_migration_error:
            os << "e_migration_error";
            break;
        case e_not_prepared_for_migration:
            os << "e_not_prepared_for_migration";
            break;
        case e_no_converter_entry:
            os << "e_no_converter_entry";
            break;
        case e_prev_data_recovery_was_incomplete: // PTS 1124994 UH 2004-01-26
            os << "e_prev_data_recovery_was_incomplete";
            break;
        case e_bad_fdir:
            os << "e_bad_fdir";
            break;
        case e_bad_logpage:
            os << "e_bad_logpage";
            break;
        case e_bad_conv_page:
            os << "e_bad_conv_page";
            break;
		case e_bad_conv_bitmap_page:
			os << "bad_convbitmappage";
			break;
        case e_bad_bitmap_page:
            os << "e_bad_bitmap_page";
            break;
        case e_disk_not_accessible:
            os << "e_disk_not_accessible";
            break;
        case e_hotstandby_cmd_send_failed: // added UH 2003-09-05
            os << "e_hotstandby_cmd_send_failed";
            break;
        case e_hotstandby_wrong_role: // added UH 2003-09-05
            os << "e_hotstandby_wrong_role";
            break;
        case e_hotstandby_establish_mirror_failed: // added UH 2003-09-05
            os << "e_hotstandby_establish_mirror_failed";
            break;
        case e_hotstandby_add_node_failed: // added UH 2003-09-05
            os << "e_hotstandby_add_node_failed";
            break;
        case e_hotstandby_del_node_failed: // added UH 2003-09-05
            os << "e_hotstandby_del_node_failed";
            break;
        case e_hotstandby_free_memory_failed: // added UH 2003-09-05
            os << "e_hotstandby_free_memory_failed";
            break;
        case e_illegal_branchlength:
            os << "e_illegal_branchlength";
            break;
        case e_illegal_entrylength:
            os << "e_illegal_entrylength";
            break;
        case e_illegal_entrypos:
            os << "e_illegal_entrypos";
            break;
        case e_illegal_keylength:
            os << "e_illegal_keylength";
            break;
        case e_illegal_page_no:
            os << "e_illegal_page_no";
            break;
        case e_move_error:
            os << "e_move_error";
            break;
        case e_invalid_entrypos:
            os << "e_invalid_entrypos";
            break;
        case e_invalid_index_structure:
            os << "e_invalid_index_structure";
            break;
        case e_invalid_leaves_structure:
            os << "e_invalid_leaves_structure";
            break;
        case e_no_more_space:
            os << "e_no_more_space";
            break;
        case e_page_in_wrong_tree:
            os << "e_page_in_wrong_tree";
            break;
        case e_data_page_corrupted:
            os << "e_data_page_corrupted";
            break;
        case e_corrupted_datapage_from_io:
            os << "e_corrupted_datapage_from_io";
            break;
        case e_invalid_fbm_mark:
            os << "e_invalid_fbm_mark";
            break;
        case e_log_error:
            os << "e_log_error";
            break;
        case e_log_full:
            os << "e_log_full";
            break;
        case e_start_server_task_error:
            os << "e_start_server_task_error";
            break;
        case e_no_more_jobs_to_wait_for:
            os << "e_no_more_jobs_to_wait_for";
            break;
        case e_no_server_task_available:
            os << "e_no_server_task_available";
            break;
        case e_nil_transindex:
            os << "e_nil_transindex";
            break;
        case e_long_column_missing:
            os << "e_long_column_missing";
            break;
        case e_bd_leaf_unlocked:
            os << "e_bd_leaf_unlocked";
            break;
        case e_too_long_objects:
            os << "e_too_long_objects";
            break;
        case e_unknown_error:
            os << "e_unknown_error";
            break;
        case e_inp_null_not_allowed:
            os << "e_inp_null_not_allowed";
            break;
        case e_inp_number_invalid:
            os << "e_inp_number_invalid";
            break;
        case e_inp_number_overflow:
            os << "e_inp_number_overflow";
            break;
        case e_param_list_too_long:
            os << "e_param_list_too_long";
            break;
        case e_param_list_too_short:
            os << "e_param_list_too_short";
            break;
        case e_incompatible_datatype:
            os << "e_incompatible_datatype";
            break;
        case e_too_many_hostvar:
            os << "e_too_many_hostvar";
            break;
        case e_missing_variable_addr:
            os << "e_missing_variable_addr";
            break;
        case e_cmd_too_long:
            os << "e_cmd_too_long";
            break;
        case e_copyop_not_allowed:
            os << "e_copyop_not_allowed";
            break;
        case e_inp_string_truncated:
            os << "e_inp_string_truncated";
            break;
        case e_use_callstm:
            os << "e_use_callstm";
            break;
        case e_cmd_not_available:
            os << "e_cmd_not_available";
            break;
        case e_uni_to_ascii_conv_failed:
            os << "e_uni_to_ascii_conv_failed";
            break;
        case e_new_failed:
            os << "e_new_failed";
            break;
        case e_nil_pointer:
            os << "e_nil_pointer";
            break;
        case e_invalid_oid:
            os << "e_invalid_oid";
            break;
        case e_unknown_guid:
            os << "e_unknown_guid";
            break;
        case e_duplicate_object:
            os << "e_duplicate_object";
            break;
        case e_object_not_locked:
            os << "e_object_not_locked";
            break;
        case e_incompatible_oid:
            os << "e_incompatible_oid";
            break;
        case e_oms_istream_error:
            os << "e_oms_istream_error";
            break;
        case e_oms_ostream_error:
            os << "e_oms_ostream_error";
            break;
        case e_com_object_crashed:
            os << "e_com_object_crashed";
            break;
        case e_no_coclass_obj_found:
            os << "e_no_coclass_obj_found";
            break;
        case e_unsupported_param_type:
            os << "e_unsupported_param_type";
            break;
        case e_progid_not_found:
            os << "e_progid_not_found";
            break;
        case e_inprocserver_not_found:
            os << "e_inprocserver_not_found";
            break;
        case e_localserver_not_found:
            os << "e_localserver_not_found";
            break;
        case e_method_not_found:
            os << "e_method_not_found";
            break;
        case e_unknown_version:
            os << "e_unknown_version";
            break;
        case e_version_already_open:
            os << "e_version_already_open";
            break;
        case e_transaction_end_required:
            os << "e_transaction_end_required";
            break;
        case e_already_in_critical_section:
            os << "e_already_in_critical_section";
            break;
        case e_not_in_critical_section:
            os << "e_not_in_critical_section";
            break;
        case e_already_in_version:
            os << "e_already_in_version";
            break;
        case e_missing_before_image:
            os << "e_missing_before_image";
            break;
        case e_unknown_schema:
            os << "e_unknown_schema";
            break;
        case e_duplicate_schema:
            os << "e_duplicate_schema";
            break;
        case e_proc_address_not_found:
            os << "e_proc_address_not_found";
            break;
        case e_directory_not_found:
            os << "e_directory_not_found";
            break;
        case e_lib_not_released:
            os << "e_lib_not_released";
            break;
        case e_buffer_too_small:
            os << "e_buffer_too_small";
            break;
        case e_terminate:
            os << "e_terminate";
            break;
        case e_oms_read_only:
            os << "e_oms_read_only";
            break;
        case e_version_bound_by_trans:
            os << "e_version_bound_by_trans";
            break;
        case e_packed_out_of_range:
            os << "e_packed_out_of_range";
            break;
        case e_wrong_class_id:
            os << "e_wrong_class_id";
            break;
        case e_wrong_object_version:
            os << "e_wrong_object_version";
            break;
        case e_wrong_object_state:
            os << "e_wrong_object_state";
            break;
        case e_too_old_oid:
            os << "e_too_old_oid";
            break;
        case e_nil_page_pointer:
            os << "e_nil_page_pointer";
            break;
        case e_no_next_object:
            os << "e_no_next_object";
            break;
        case e_hash_not_found:
            os << "e_hash_not_found";
            break;
        case e_invalid_obj_col_desc:
            os << "e_invalid_obj_col_desc";
            break;
        case e_duplicate_hash_key:
            os << "e_duplicate_hash_key";
            break;
        case e_hash_key_not_found:
            os << "e_hash_key_not_found";
            break;
        case e_invalid_iterator:
            os << "e_invalid_iterator";
            break;
        case e_unknown_consist_vers:
            os << "e_unknown_consist_vers";
            break;
        case e_object_not_found:
            os << "e_object_not_found";
            break;
        case e_obj_history_not_found:
            os << "e_obj_history_not_found";
            break;
        case e_hash_memory_exceeded:
            os << "e_hash_memory_exceeded";
            break;
        case e_illegal_object_length:
            os << "e_illegal_object_length";
            break;
        case e_illegal_object_pos:
            os << "e_illegal_object_pos";
            break;
        case e_object_dirty:
            os << "e_object_dirty";
            break;
        case e_wrong_obj_file_id:
            os << "e_wrong_obj_file_id";
            break;
        case e_page_no_in_use:
            os << "e_page_no_in_use";
            break;
        case e_wrong_all_page_count:
            os << "e_wrong_all_page_count";
            break;
        case e_inconsistent_drop_info:
            os << "e_inconsistent_drop_info";
            break;
        case e_illegal_cont_obj:
            os << "e_illegal_cont_obj";
            break;
        case e_wrong_obj_count:
            os << "e_wrong_obj_count";
            break;
        case e_case_not_found:
            os << "e_case_not_found";
            break;
        case e_too_many_catch_levels:
            os << "e_too_many_catch_levels";
            break;
        case e_missing_object_flush:
            os << "e_missing_object_flush";
            break;
        case e_requested_dbperror:
            os << "e_requested_dbperror";
            break;
        case e_undo_log_write_error:
            os << "e_undo_log_write_error";
            break;
        case e_undo_log_execute_error:
            os << "e_undo_log_execute_error";
            break;
        case e_redo_log_execute_error:
            os << "e_redo_log_execute_error";
            break;
        case e_history_write_error:
            os << "e_history_write_error";
            break;
        case e_history_read_error:
            os << "e_history_read_error";
            break;
        case e_history_remove_error:
            os << "e_history_remove_error";
            break;
        case e_redo_log_read_error:
            os << "e_redo_log_read_error";
            break;
        case e_redo_log_write_error:
            os << "e_redo_log_write_error";
            break;
        default :
            os << "?";
    }
    return os;
}
}

// tgg00_BdInvRange
SAPDB_OStream & operator<<( SAPDB_OStream & os, const tgg00_BdInvRange & value )
{
    switch ( value )
    {
        case secondary_stop:
            os << "secondary_stop";
            break;
        case primary_start:
            os << "primary_start";
            break;
        case primary_stop:
            os << "primary_stop";
            break;
        case incl_first:
            os << "incl_first";
            break;
        default: 
            os << "?";
    }
    return os;
}

// tgg07_StratEnum
SAPDB_OStream & operator<<( SAPDB_OStream & os, const tgg07_StratEnum &value )
{
    switch ( value )
    {
        case strat_key_equal:
            os << "strat_key_equal";
            break;
        case strat_key_in:
            os << "strat_key_in";
            break;
        case strat_key_subq:
            os << "strat_key_subq";
            break;
             /* */
        case  strat_key_range:
            os << "strat_key_range";
            break;
             /* */
        case strat_inv_range:
            os << "strat_inv_range";
            break;
        case strat_inv_in:
            os << "strat_inv_in";
            break;
        case strat_inv_subq:
            os << "strat_inv_subq";
            break;
             /* */
        case strat_undecided:
            os << "strat_undecided";
            break;
             /* */
        case strat_key_range_fetch:
            os << "strat_key_range_fetch";
            break;
        case strat_inv_range_fetch:
            os << "strat_inv_range_fetch";
            break;
        case strat_inv_range_merge_fetch:
            os << "strat_inv_range_merge_fetch";
            break;
             /* */
        case strat_join_key_equal:
            os << "strat_join_key_equal";
            break;
        case strat_join_key_next:
            os << "strat_join_key_next";
            break;
        case strat_join_all_keys_equal:
            os << "strat_join_all_keys_equal";
            break;
        case strat_join_key_range:
            os << "strat_join_key_range";
            break;
        case strat_join_viewkey:
            os << "strat_join_viewkey";
            break;
        case strat_join_inv:
            os << "strat_join_inv";
            break;
        case strat_join_all_inv_equal:
            os << "strat_join_all_inv_equal";
            break;
        case strat_join_inv_range:
            os << "strat_join_inv_range";
            break;
             /* */
        case strat_more_than_one:
            os << "strat_more_than_one";
            break;
             /* */
        case strat_no_result:
            os << "strat_no_result";
            break;
             /* */
        case strat_catalog:
            os << "strat_catalog";
            break;
        case strat_viewkey:
            os << "strat_viewkey";
            break;
        default: 
            os << "?";
    }
    return os;
}

// bool
SAPDB_OStream & operator<< (SAPDB_OStream & os, const bool value)
{
    switch ( value )
    {
        case true:
            os << "true";
            break;
        case false:
            os << "false";
            break;
    }
    return os;
}

//tgg04_Distinct
SAPDB_OStream & operator<< (SAPDB_OStream &os, const tgg04_Distinct & value)
{
    switch ( value )
    {
        case no_distinct:
            os << "no_distinct";
            break;
        case full_distinct:
            os << "full_distinct";
            break;
        case distinct_without_warning_byte:
            os << "distinct_without_warning_byte";
            break;
        default:
            os << "?";
    }
    return os;
}

// tgg04_CacheType
SAPDB_OStream & operator<< (SAPDB_OStream &os, const tgg04_CacheType & value)
{
    switch ( value )
    {
        case cachCatalog_egg04:
            os << "cachCatalog_egg04";
            break;
        case cachConv_egg04:
            os << "cachConv_egg04";
            break;
        case cachData_egg04:
            os << "cachData_egg04";
            break;
        case cachFbm_egg04:
            os << "cachFbm_egg04";
            break;
        case cachProcCode_egg04:
            os << "cachProcCode_egg04";
            break;
        case cachProcData_egg04:
            os << "cachProcData_egg04";
            break;
        case cachTemp_egg04:
            os << "cachTemp_egg04";
            break;
        case cachUsm_egg04:
            os << "cachUsm_egg04";
            break;
        case cachSequence_egg04:
            os << "cachSequence_egg04";
            break;
        default:
            os << "?";
    }
    return os;
}

// tgg00_MessType
SAPDB_OStream & operator<< (SAPDB_OStream &os, const tgg00_MessType &value)
{
    switch ( value )
    {
        case m_nil :
            os << "m_nil";
            break;
        case m_begin :
            os << "m_begin";
            break;
        case m_cancel :
            os << "m_cancel";
            break;
        case m_change :
            os << "m_change";
            break;
        case m_check_database :
            os << "m_check_database";
            break;
        case m_clear_log :
            os << "m_clear_log";
            break;
        case m_close :
            os << "m_close";
            break;
        case m_column :
            os << "m_column";
            break;
        case m_column_statistic :
            os << "m_column_statistic";
            break;
        case m_commit :
            os << "m_commit";
            break;
        case m_complex :
            os << "m_complex";
            break;
        case m_connect :
            os << "m_connect";
            break;
        case m_invalid :
            os << "m_invalid";
            break;
        case m_unused_was_copy_tempfile :
            os << "m_unused_was_copy_tempfile";
            break;
        case m_create_file :
            os << "m_create_file";
            break;
        case m_create_index :
            os << "m_create_index";
            break;
        case m_unused_was_create_serverdb :
            os << "m_unused_was_create_serverdb";
            break;
        case m_create_table :
            os << "m_create_table";
            break;
        case m_delete :
            os << "m_delete";
            break;
        case m_unused_was_delete_trans :
            os << "m_unused_was_delete_trans";
            break;
        case m_describe :
            os << "m_describe";
            break;
        case m_destroy_temp :
            os << "m_destroy_temp";
            break;
        case m_diagnose :
            os << "m_diagnose";
            break;
        case m_down :
            os << "m_down";
            break;
        case m_drop :
            os << "m_drop";
            break;
        case m_unused_was_drop_serverdb :
            os << "m_unused_was_drop_serverdb";
            break;
        case m_end :
            os << "m_end";
            break;
        case m_eval :
            os << "m_eval";
            break;
        case m_fetch :
            os << "m_fetch";
            break;
        case m_flush :
            os << "m_flush";
            break;
        case m_get :
            os << "m_get";
            break;
        case m_usage :
            os << "m_usage";
            break;
        case m_index_statistic :
            os << "m_index_statistic";
            break;
        case m_init_config :
            os << "m_init_config";
            break;
        case m_unused_was_inquire :
            os << "m_unused_was_inquire";
            break;
        case m_insert :
            os << "m_insert";
            break;
        case m_insert_select :
            os << "m_insert_select";
            break;
        case m_unused_was_insert_trans :
            os << "m_unused_was_insert_trans";
            break;
        case m_unused_was_integrate_node :
            os << "m_unused_was_integrate_node";
            break;
        case m_key :
            os << "m_key";
            break;
        case m_lock :
            os << "m_lock";
            break;
        case m_mend :
            os << "m_mend";
            break;
        case m_mfetch :
            os << "m_mfetch";
            break;
        case m_move :
            os << "m_move";
            break;
        case m_unused_was_notify :
            os << "m_unused_was_notify";
            break;
        case m_unused_was_logpos_list :
            os << "m_unused_was_logpos_list";
            break;
        case m_outcopy :
            os << "m_outcopy";
            break;
        case m_unused_was_outcopy_catalog :
            os << "m_unused_was_outcopy_catalog";
            break;
        case m_partial_rollback :
            os << "m_partial_rollback";
            break;
        case m_unused_was_notusedanymore :
            os << "m_unused_was_notusedanymore";
            break;
        case m_release :
            os << "m_release";
            break;
        case m_reload :
            os << "m_reload";
            break;
        case m_restart :
            os << "m_restart";
            break;
        case m_restore :
            os << "m_restore";
            break;
        case m_return_error :
            os << "m_return_error";
            break;
        case m_return_result :
            os << "m_return_result";
            break;
        case m_rollback :
            os << "m_rollback";
            break;
        case m_save :
            os << "m_save";
            break;
        case m_unused_was_save_overwrite :
            os << "m_unused_was_save_overwrite";
            break;
        case m_savepoint :
            os << "m_savepoint";
            break;
        case m_select :
            os << "m_select";
            break;
        case m_select_row :
            os << "m_select_row";
            break;
        case m_set :
            os << "m_set";
            break;
        case m_show :
            os << "m_show";
            break;
        case m_shutdown :
            os << "m_shutdown";
            break;
        case m_shutdown_filesystem :
            os << "m_shutdown_filesystem";
            break;
        case m_set_replication :
            os << "m_set_replication";
            break;
        case m_stamp :
            os << "m_stamp";
            break;
        case m_succ_file_version :
            os << "m_succ_file_version";
            break;
        case m_table_statistic :
            os << "m_table_statistic";
            break;
        case m_union :
            os << "m_union";
            break;
        case m_unload :
            os << "m_unload";
            break;
        case m_unlock :
            os << "m_unlock";
            break;
        case m_unused_was_unused_log :
            os << "m_unused_was_unused_log";
            break;
        case m_up :
            os << "m_up";
            break;
        case m_unused_was_upd_statistic :
            os << "m_unused_was_upd_statistic";
            break;
        case m_update :
            os << "m_update";
            break;
        case m_update_rec :
            os << "m_update_rec";
            break;
        case m_verify :
            os << "m_verify";
            break;
        case m_majority :
            os << "m_majority";
            break;
        case m_unused_was_format :
            os << "m_unused_was_format";
            break;
        case m_unused_was_repli_savepoint :
            os << "m_unused_was_repli_savepoint";
            break;
        case m_nextval :
            os << "m_nextval";
            break;
        case m_history :
            os << "m_history";
            break;
        case m_sharedSQLParseid :
            os << "m_sharedSQLParseid";
            break;
        case m_procedure :
            os << "m_procedure";
            break;
        case m_new :
            os << "m_new";
            break;
        case m_unused_was_create_obj_hash :
            os << "m_unused_was_create_obj_hash";
            break;
        case m_get_object :
            os << "m_get_object";
            break;
        case m_debug :
            os << "m_debu";
            break;
        case m_save_parallel :
            os << "m_save_parallel";
            break;
        case m_restore_parallel :
            os << "m_restore_parallel";
            break;
        case m_autosave :
            os << "m_autosave";
            break;
        case m_unused_was_open_trans :
            os << "m_unused_was_open_trans";
            break;
        case m_create_index_parallel :
            os << "m_create_index_parallel";
            break;
        case m_migrate_obj_files :
            os << "m_migrate_obj_files";
            break;
        default :
            os << "?";
            break;
    }
    return os;
}

// tgg00_MessType2
SAPDB_OStream & operator<< (SAPDB_OStream &os, const tgg00_MessType2 &value)
{
    switch ( value )
    {
        case mm_nil :
            os << "mm_nil";
            break;
        case mm_clear :
            os << "mm_clear";
            break;
        case mm_close :
            os << "mm_close";
            break;
        case mm_consistent :
            os << "mm_consistent";
            break;
        case mm_compare_record :
            os << "mm_compare_record";
            break;
        case mm_config :
            os << "mm_config";
            break;
        case mm_coordinator :
            os << "mm_coordinator";
            break;
        case mm_provoked_error :
            os << "mm_provoked_error";
            break;
        case mm_copy_log :
            os << "mm_copy_log";
            break;
        case mm_database :
            os << "mm_database";
            break;
        case mm_unused_was_database_clear_log :
            os << "mm_unused_was_database_clear_log";
            break;
        case mm_database_noformat :
            os << "mm_database_noformat";
            break;
        case mm_unused_was_dbname :
            os << "mm_unused_was_dbname";
            break;
        case mm_destroy_temp :
            os << "mm_destroy_temp";
            break;
        case mm_device :
            os << "mm_device";
            break;
        case mm_device_restore :
            os << "mm_device_restore";
            break;
        case mm_direct :
            os << "mm_direct";
            break;
        case mm_distributed :
            os << "mm_distributed";
            break;
        case mm_dump :
            os << "mm_dump";
            break;
        case mm_empty :
            os << "mm_empty";
            break;
        case mm_end_read_only :
            os << "mm_end_read_only";
            break;
        case mm_expand :
            os << "mm_expand";
            break;
        case mm_file :
            os << "mm_file";
            break;
        case mm_first :
            os << "mm_first";
            break;
        case mm_first_join_select :
            os << "mm_first_join_select";
            break;
        case mm_free3 :
            os << "mm_free3";
            break;
        case mm_fread :
            os << "mm_fread";
            break;
        case mm_running_cmds :
            os << "mm_running_cmds";
            break;
        case mm_index :
            os << "mm_index";
            break;
        case mm_join_with_last_tab :
            os << "mm_join_with_last_tab";
            break;
        case mm_keep_lock :
            os << "mm_keep_lock";
            break;
        case mm_key :
            os << "mm_key";
            break;
        case mm_last :
            os << "mm_last";
            break;
        case mm_operator_join :
            os << "mm_operator_join";
            break;
        case mm_lock :
            os << "mm_lock";
            break;
        case mm_first_operator_join :
            os << "mm_first_operator_join";
            break;
        case mm_lock_timeout :
            os << "mm_lock_timeout";
            break;
        case mm_log :
            os << "mm_log";
            break;
        case mm_log_cold :
            os << "mm_log_cold";
            break;
        case mm_log_demo :
            os << "mm_log_demo";
            break;
        case mm_object_id :
            os << "mm_object_id";
            break;
        case mm_next :
            os << "mm_next";
            break;
        case mm_nokey :
            os << "mm_nokey";
            break;
        case mm_ok_not_allowed :
            os << "mm_ok_not_allowed";
            break;
        case mm_last_operator_join :
            os << "mm_last_operator_join";
            break;
        case mm_optimize :
            os << "mm_optimize";
            break;
        case mm_outcopy :
            os << "mm_outcopy";
            break;
        case mm_pages :
            os << "mm_pages";
            break;
        case mm_specialkey :
            os << "mm_specialkey";
            break;
        case mm_parse :
            os << "mm_parse";
            break;
        case mm_participant :
            os << "mm_participant";
            break;
        case mm_down :
            os << "mm_down";
            break;
        case mm_prefix :
            os << "mm_prefix";
            break;
        case mm_prev :
            os << "mm_prev";
            break;
        case mm_qual :
            os << "mm_qual";
            break;
        case mm_standby :
            os << "mm_standby";
            break;
        case mm_read :
            os << "mm_read";
            break;
        case mm_rollback :
            os << "mm_rollback";
            break;
        case mm_same :
            os << "mm_same";
            break;
        case mm_search :
            os << "mm_search";
            break;
        case mm_string :
            os << "mm_string";
            break;
        case mm_subtrans :
            os << "mm_subtrans";
            break;
        case mm_table :
            os << "mm_table";
            break;
        case mm_test :
            os << "mm_test";
            break;
        case mm_trans :
            os << "mm_trans";
            break;
        case mm_trunc :
            os << "mm_trunc";
            break;
        case mm_user :
            os << "mm_user";
            break;
        case mm_with_functions :
            os << "mm_with_functions";
            break;
        case mm_with_join :
            os << "mm_with_join";
            break;
        case mm_write :
            os << "mm_write";
            break;
        case mm_write_off :
            os << "mm_write_off";
            break;
        case mm_write_on :
            os << "mm_write_on";
            break;
        case mm_unused_was_write_reject :
            os << "mm_unused_was_write_reject";
            break;
        case mm_trigger :
            os << "mm_trigger";
            break;
        case mm_ignore_duplicates :
            os << "mm_ignore_duplicates";
            break;
        case mm_update_duplicates :
            os << "mm_update_duplicates";
            break;
        case mm_unused_was_fetch_request :
            os << "mm_unused_was_fetch_request";
            break;
        case mm_unused_was_allocate :
            os << "mm_unused_was_allocate";
            break;
        case mm_unused_was_bind_req :
            os << "mm_unused_was_bind_req";
            break;
        case mm_unused_was_exc_req :
            os << "mm_unused_was_exc_req";
            break;
        case mm_unused_was_deallocate :
            os << "mm_unused_was_deallocate";
            break;
        case mm_unused_was_reply_cont :
            os << "mm_unused_was_reply_cont";
            break;
        case mm_unused_was_reply_end :
            os << "mm_unused_was_reply_end";
            break;
        case mm_unused_was_not_used_was_first_distributed :
            os << "mm_unused_was_not_used_was_first_distributed";
            break;
        case mm_unused_was_next_reply :
            os << "mm_unused_was_next_reply";
            break;
        case mm_acknowledge :
            os << "mm_acknowledge";
            break;
        case mm_newtape :
            os << "mm_newtape";
            break;
        case mm_ignore :
            os << "mm_ignore";
            break;
        case mm_abort :
            os << "mm_abort";
            break;
        case mm_object :
            os << "mm_object";
            break;
        case mm_new_write :
            os << "mm_new_write";
            break;
        case mm_new_read :
            os << "mm_new_read";
            break;
        case mm_parallel :
            os << "mm_parallel";
            break;
        case mm_checkpoint :
            os << "mm_checkpoint";
            break;
        case mm_unused_was_optimize_longvar :
            os << "mm_unused_was_optimize_longvar";
            break;
        case mm_last_rowno :
            os << "mm_last_rowno";
            break;
        default :
            os << "?";
            break;
    }
    return os;
}

// tgg00_DateTimeFormat
SAPDB_OStream & operator<< (SAPDB_OStream &os, const tgg00_DateTimeFormat &value)
{
    switch ( value )
    {
        case dtf_none :
            os << "dtf_none";
            break;
        case dtf_normal :
            os << "dtf_normal";
            break;
        case dtf_iso :
            os << "dtf_iso";
            break;
        case dtf_usa :
            os << "dtf_usa";
            break;
        case dtf_eur :
            os << "dtf_eur";
            break;
        case dtf_jis :
            os << "dtf_jis";
            break;
        case dtf_oracle_date :
            os << "dtf_oracle_date";
            break;
        case dtf_was_ansi_now_is_same_as_iso :
            os << "dtf_was_ansi_now_is_same_as_iso";
            break;
        case dtf_ts_eur :
            os << "dtf_ts_eur";
            break;
        case dtf_future :
            os << "dtf_future";
            break;
        default :
            os << "?";
    }
    return os;
}

// tgg00_IndexMode
SAPDB_OStream & operator<< (SAPDB_OStream &os, const tgg00_IndexMode &value)
{
    switch ( value )
    {
        case create_index :
            os << "create_index";
            break;
        case restart_index :
            os << "restart_index";
            break;
        case test_index :
            os << "test_index";
            break;
        default :
            os << "?";
    }
    return os;
}

// tgg00_InvListKind
SAPDB_OStream & operator<< (SAPDB_OStream &os, const tgg00_InvListKind &value)
{
    switch ( value )
    {
        case ilkArray_egg00 :
            os << "ilkArray_egg00";
            break;
        case ilkSubTree_egg00 :
            os << "ilkSubTree_egg00";
            break;
        default :
            os << "?";
    }
    return os;
}

// tgg00_LockReqMode
SAPDB_OStream & operator<< (SAPDB_OStream &os, const tgg00_LockReqMode &value)
{
    switch ( value )
    {
        case lckFree_egg00 :
            os << "lckFree_egg00";
            break;
        case lckSysShare_egg00 :
            os << "lckSysShare_egg00";
            break;
        case lckSysExcl_egg00 :
            os << "lckSysExcl_egg00";
            break;
        case lckTabShare_egg00 :
            os << "lckTabShare_egg00";
            break;
        case lckTabExcl_egg00 :
            os << "lckTabExcl_egg00";
            break;
        case lckRowShare_egg00 :
            os << "lckRowShare_egg00";
            break;
        case lckRowExcl_egg00 :
            os << "lckRowExcl_egg00";
            break;
        case lckRowOptimistic_egg00 :
            os << "lckRowOptimistic_egg00";
            break;
        default :
            os << "?";
    }
    return os;
}

// tgg00_StackOpType
SAPDB_OStream & operator<< (SAPDB_OStream &os, const tgg00_StackOpType &value)
{
    switch ( value )
    {
        case op_none :
            os << "op_none";
            break;
        case op_mapchar :
            os << "op_mapchar";
            break;
        case op_ascii :
            os << "op_ascii";
            break;
        case op_dbyte_length :
            os << "op_dbyte_length";
            break;
        case op_desc_expr_upd :
            os << "op_desc_expr_upd";
            break;
        case op_desc_unique_expr_upd :
            os << "op_desc_unique_expr_upd";
            break;
        case op_digits :
            os << "op_digits";
            break;
        case op_ebcdic :
            os << "op_ebcdic";
            break;
        case op_expr_upd :
            os << "op_expr_upd";
            break;
        case op_fixed :
            os << "op_fixed";
            break;
        case op_hex :
            os << "op_hex";
            break;
        case op_in :
            os << "op_in";
            break;
        case op_initcap :
            os << "op_initcap";
            break;
        case op_jmp_ins_upd :
            os << "op_jmp_ins_upd";
            break;
        case op_length :
            os << "op_length";
            break;
        case op_lowcase :
            os << "op_lowcase";
            break;
        case op_not_in :
            os << "op_not_in";
            break;
        case op_num :
            os << "op_num";
            break;
        case op_odd :
            os << "op_odd";
            break;
        case op_order_asc :
            os << "op_order_asc";
            break;
        case op_order_desc :
            os << "op_order_desc";
            break;
        case op_order_desc_ascii :
            os << "op_order_desc_ascii";
            break;
        case op_order_desc_ebcdic :
            os << "op_order_desc_ebcdic";
            break;
        case op_outer_join :
            os << "op_outer_join";
            break;
        case op_remove :
            os << "op_remove";
            break;
        case op_replace :
            os << "op_replace";
            break;
        case op_soundex :
            os << "op_soundex";
            break;
        case op_stamp :
            os << "op_stamp";
            break;
        case op_unique :
            os << "op_unique";
            break;
        case op_unique_desc :
            os << "op_unique_desc";
            break;
        case op_unique_desc_ascii :
            os << "op_unique_desc_ascii";
            break;
        case op_unique_desc_ebcdic :
            os << "op_unique_desc_ebcdic";
            break;
        case op_unique_expr_upd :
            os << "op_unique_expr_upd";
            break;
        case op_upcase :
            os << "op_upcase";
            break;
        case op_get_sub_value :
            os << "op_get_sub_value";
            break;
        case op_translate :
            os << "op_translate";
            break;
        case op_hextoraw :
            os << "op_hextoraw";
            break;
        case op_join_key :
            os << "op_join_key";
            break;
        case op_late_asc_unique_check :
            os << "op_late_asc_unique_check";
            break;
        case op_late_desc_unique_check :
            os << "op_late_desc_unique_check";
            break;
        case op_nextval :
            os << "op_nextval";
            break;
        case op_scol_upd :
            os << "op_scol_upd";
            break;
        case op_dbyte_mapchar :
            os << "op_dbyte_mapchar";
            break;
        case op_updated :
            os << "updated";
            break;
        case op__filler3 :
            os << "op__filler3";
            break;
        case op__filler4 :
            os << "op__filler4";
            break;
        case op_longcol_update :
            os << "op_longcol_update";
            break;
        case op_adddate :
            os << "op_adddate";
            break;
        case op_addmonth :
            os << "op_addmonth";
            break;
        case op_addtime :
            os << "op_addtime";
            break;
        case op_date :
            os << "op_date";
            break;
        case op_date_from_timestamp :
            os << "op_date_from_timestamp";
            break;
        case op_datediff :
            os << "op_datediff";
            break;
        case op_dayofweek :
            os << "op_dayofweek";
            break;
        case op_dayofyear :
            os << "op_dayofyear";
            break;
        case op_days :
            os << "op_days";
            break;
        case op_last_day :
            os << "op_last_day";
            break;
        case op_makedate :
            os << "op_makedate";
            break;
        case op_maketime :
            os << "op_maketime";
            break;
        case op_months_between :
            os << "op_months_between";
            break;
        case op_subdate :
            os << "op_subdate";
            break;
        case op_submonth :
            os << "op_submonth";
            break;
        case op_subtime :
            os << "op_subtime";
            break;
        case op_time :
            os << "op_time";
            break;
        case op_timediff :
            os << "op_timediff";
            break;
        case op_timestamp :
            os << "op_timestamp";
            break;
        case op_weekofyear :
            os << "op_weekofyear";
            break;
        case op_swap :
            os << "op_swap";
            break;
        case op_copy :
            os << "op_copy";
            break;
        case op_filler8 :
            os << "op_filler8";
            break;
        case op_abs :
            os << "op_abs";
            break;
        case op_ceil :
            os << "op_ceil";
            break;
        case op_floor :
            os << "op_floor";
            break;
        case op_fracround :
            os << "op_fracround";
            break;
        case op_fractrunc :
            os << "op_fractrunc";
            break;
        case op_power :
            os << "op_power";
            break;
        case op_round :
            os << "op_round";
            break;
        case op_sign :
            os << "op_sign";
            break;
        case op_sqrt :
            os << "op_sqrt";
            break;
        case op_trunc :
            os << "op_trunc";
            break;
        case op_filler9 :
            os << "op_filler9";
            break;
        case op_filler10 :
            os << "op_filler10";
            break;
        case op_filler11 :
            os << "op_filler11";
            break;
        case op_div :
            os << "op_div";
            break;
        case op_intdiv :
            os << "op_intdiv";
            break;
        case op_minus :
            os << "op_minus";
            break;
        case op_minus_monad :
            os << "op_minus_monad";
            break;
        case op_mod :
            os << "op_mod";
            break;
        case op_mod_func :
            os << "op_mod_func";
            break;
        case op_mult :
            os << "op_mult";
            break;
        case op_plus :
            os << "op_plus";
            break;
        case op_filler12 :
            os << "op_filler12";
            break;
        case op_filler13 :
            os << "op_filler13";
            break;
        case op_is_integer :
            os << "op_is_integer";
            break;
        case op_between :
            os << "op_between";
            break;
        case op_not_between :
            os << "op_not_between";
            break;
        case op_not :
            os << "op_not";
            break;
        case op_upd_view_and :
            os << "op_upd_view_and";
            break;
        case op_and :
            os << "op_and";
            break;
        case op_or :
            os << "op_or";
            break;
        case op_like :
            os << "op_like";
            break;
        case op_not_like :
            os << "op_not_like";
            break;
        case op_not_null :
            os << "op_not_null";
            break;
        case op_not_sounds :
            os << "op_not_sounds";
            break;
        case op_null :
            os << "op_null";
            break;
        case op_sounds :
            os << "op_sounds";
            break;
        case op_true :
            os << "op_true";
            break;
        case op_false :
            os << "op_false";
            break;
        case op_undef_to_false :
            os << "op_undef_to_false";
            break;
        case op_eq :
            os << "op_eq";
            break;
        case op_eq_all :
            os << "op_eq_all";
            break;
        case op_ge :
            os << "op_ge";
            break;
        case op_gt :
            os << "op_gt";
            break;
        case op_le :
            os << "op_le";
            break;
        case op_lt :
            os << "op_lt";
            break;
        case op_ne :
            os << "op_ne";
            break;
        case op_dbyte_upper :
            os << "op_dbyte_upper";
            break;
        case op_dbyte_lower :
            os << "op_dbyte_lower";
            break;
        case op_dbyte_initcap :
            os << "op_dbyte_initcap";
            break;
        case op_dbyte_replace :
            os << "op_dbyte_replace";
            break;
        case op_dbyte_remove :
            os << "op_dbyte_remove";
            break;
        case op_dbyte_translate :
            os << "op_dbyte_translate";
            break;
        case op_serial :
            os << "op_serial";
            break;
        case op_test_zero :
            os << "op_test_zero";
            break;
        default :
            os << "?";
    }
    return os;
}

// tgg00_StackOpFunc
SAPDB_OStream & operator<< (SAPDB_OStream &os, const tgg00_StackOpFunc &value)
{
    switch ( value )
    {
        case op_f_none :
            os << "LASTFUNCTION";
            break;
        case op_f_all_count :
            os << "ALL COUNT";
            break;
        case op_f_avg :
            os << "AVG";
            break;
        case op_f_check_null :
            os << "CHECK NULL";
            break;
        case op_f_count :
            os << "COUNT";
            break;
        case op_f_count_kb :
            os << "COUNT KB";
            break;
        case op_f_dis_avg :
            os << "DIS AVG";
            break;
        case op_f_dis_count :
            os << "DIS COUNT";
            break;
        case op_f_dis_sum :
            os << "DIS SUM";
            break;
        case op_f_max :
            os << "MAX";
            break;
        case op_f_min :
            os << "MIN";
            break;
        case op_f_sum :
            os << "SUM";
            break;
        case op_f_stddev :
            os << "STDDEV";
            break;
        case op_f_dis_stddev :
            os << "DIS STDDEV";
            break;
        case op_f_variance :
            os << "VARIANCE";
            break;
        case op_f_dis_variance :
            os << "DIS VARIANCE";
            break;
        default :
            os << "?";
    }
    return os;
}

// tsp00_LcompResult
SAPDB_OStream & operator<< (SAPDB_OStream &os, const tsp00_LcompResult &value)
{
    switch ( value )
    {
        case l_less :
            os << "l_less";
            break;
        case l_equal :
            os << "l_equal";
            break;
        case l_greater :
            os << "l_greater";
            break;
        case l_undef :
            os << "l_undef";
            break;
    }
    return os;
}

// SQLMan_Identifier
SAPDB_OStream & operator<< (SAPDB_OStream &os , const SQLMan_Identifier & id)
{
#ifdef KERNEL
    if (g01unicode)
    {
        int ix = 0;
        char  aux[sizeof(id) / 2 + 1];
        char* pAux = &aux[0];
        while ((ix < sizeof(id)) && (0 == id[ix]))
        {
            ++ix;
            if (id[ix] > 127)
            {
                break;
            }
            *pAux++ = id[ix];
            ++ix;
        }
        if (ix > sizeof(id))
        {
            *pAux = 0;
            os << &aux[0];
        }
        else
        {
            os << NewLine; 
            os << SAPDBTrace_HexBuf (&id, sizeof(id));
        }
    }
    else
#endif
    {
        int idx = sizeof(id) - 1;
        while ((idx > 0) && (' ' == id[idx]))
        {
            --idx;
        }
        os.PutString((char*) &id[0], idx + 1);
        
    }
    return os;
}

// Catalog_ObjectId
SAPDB_OStream & operator<< (SAPDB_OStream &os, const Catalog_ObjectId & OID)
{
    os << NewLine; 
    os << SAPDBTrace_Hex (&OID, sizeof(OID));
    return os;
}
