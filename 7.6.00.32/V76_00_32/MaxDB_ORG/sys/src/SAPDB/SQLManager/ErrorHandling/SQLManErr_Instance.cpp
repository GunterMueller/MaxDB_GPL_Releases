/*!***************************************************************************

  module      : SQLManErr_Instance.cpp

  -------------------------------------------------------------------------

  responsible : ElkeZ

  special area: SQLManagerErrorHandling
  description : Error handling

  last changed: 2003-10-17  07:26
  see also    :
  first created:2003-10-17  07:26

  -------------------------------------------------------------------------


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

*****************************************************************************/

#include "SQLManager/ErrorHandling/SQLManErr_Instance.hpp"
#include "SQLManager/ErrorHandling/SQLManErr_BaseToReturnCode.hpp"
#include "Container/Container_SortableVector.hpp"
#include "ggg00.h"
#include "ggg01.h"
#include "hak101.h"

SQLManErr_Instance::SQLManErrorObject SQLManErr_Instance::errors [] =
{
    {e_requested_dbperror,false,sqlm_internal,-28999,"required DBPError","requested_dbperror"},
    {e_invalid_deallocate,false,sqlm_internal,-28951,"System error : invalid deallocation","invalid_deallocate"},
    {e_missing_object_flush,false,sqlm_internal,-28950,"System error : missing object flush during commit","missing_object_flush"},
    {e_stop_0,false,sqlm_internal,-28910,"STOP(0) not allowed","stop_0_not_allowed"},
    {e_function_not_deterministic,false,sqlm_internal,-28908,"function must be deterministic","function_not_deterministic"},
    {e_dbfunc_stop_msg,false,sqlm_internal,-28907,"function stopped due to error","dbfunc_stop_msg"},
    {e_dbfunc_code_not_found,false,sqlm_internal,-28906,"DBFunction code not found","dbfunc_code_not_found"},
    {e_dbfunc_index_error,false,sqlm_internal,-28905,"DBFunction called for indexing failed","dbfunc_index_error"},
    {e_dbfunc_with_sql_not_allowed,false,sqlm_internal,-28904,"DBFunction containing sql statements not allowed","dbfunc_with_sql_not_allowed"},
    {e_missing_return,false,sqlm_internal,-28903,"DBFunction $1 must return a value","missing_return"},
    {e_too_many_catch_levels,false,sqlm_internal,-28902,"Too many nested try/catch statements","too_many_catch_levels"},
    {e_case_not_found,false,sqlm_internal,-28901,"No case match found","case_not_found"},
    {e_recreate_index_failed,false,sqlm_internal,-28836,"Recreation of index for OmsKeyedContainer failed","index_recreation_failed"},
    {e_key_still_visible,false,sqlm_internal,-28835,"Key of new object is still visible within consistent version","key_still_visible"},
    {e_consview_cancelled,false,sqlm_internal,-28833,"Consistent view has been cancelled","consistentview_cancelled"}, /* PTS 1134029 FF */
    {e_container_dropped,false,sqlm_internal,-28832,"Tried to access already dropped container","container_dropped"},
    {e_wrong_obj_count,false,sqlm_internal,-28831,"Wrong page count (internal error)","wrong_obj_count"},
    {e_inconsistent_drop_info,false,sqlm_internal,-28828,"Inconsistent information concerning 'is container dropped'","inconsistent_drop_info"},
    {e_wrong_all_page_count,false,sqlm_internal,-28826,"Inconsistent pagecounter","wrong_all_page_count"},
    {e_page_no_in_use,false,sqlm_internal,-28823,"Page already in use","page_no_in_use"},
    {e_wrong_obj_file_id,false,sqlm_internal,-28822,"Invalid object file ID (internal error)","wrong_obj_file_id"},
    {e_hash_key_in_use,false,sqlm_internal,-28820,"An (uncommitted) object with the given key exists already","hash_key_in_use"},
    {e_object_dirty,false,sqlm_internal,-28819,"Object has been updated by other transactions","object_dirty"},
    {e_illegal_object_pos,false,sqlm_internal,-28818,"Wrong object position","illegal_object_pos"},
    {e_illegal_object_length,false,sqlm_internal,-28817,"Wrong object length","illegal_object_length"},
    {e_hash_memory_exceeded,false,sqlm_internal,-28816,"Hash memory exceeded","hash_memory_exceeded"},
    {e_obj_history_not_found,false,sqlm_internal,-28815,"Object history not found","obj_history_not_found"},
    {e_object_not_found,false,sqlm_internal,-28814,"Object not found","object_not_found"},
    {e_wrong_object_state,false,sqlm_internal,-28814,"Object not found","wrong_object_state"},
    {e_unknown_consist_vers,false,sqlm_internal,-28813,"Unknown consistent version","unknown_consist_vers"},
    {e_invalid_iterator,false,sqlm_internal,-28812,"Invalid iterator","invalid_iterator"},
    {e_hash_key_not_found,false,sqlm_internal,-28811,"Hash key not found","hash_key_not_found"},
    {e_duplicate_hash_key,false,sqlm_internal,-28810,"Duplicate hash key","duplicate_hash_key"},
    {e_invalid_obj_col_desc,false,sqlm_internal,-28809,"Invalid object column description","invalid_obj_col_desc"},
    {e_hash_not_found,false,sqlm_internal,-28808,"Hash not found","hash_not_found"},
    {e_no_next_object,false,sqlm_internal,-28807,"No next object","no_next_object"},
    {e_nil_page_pointer,false,sqlm_internal,-28806,"Nil page pointer","nil_page_pointer"},
    {e_too_old_oid,false,sqlm_internal,-28804,"Too old OID","too_old_oid"},
    {e_wrong_object_version,false,sqlm_internal,-28802,"Wrong object version","wrong_object_version"},
    {e_wrong_class_id,false,sqlm_internal,-28801,"Wrong class id","wrong_class_id"},
    {e_dbproc_proc_not_found,false,sqlm_internal,-28609,"Stored procedure not found","dbproc_proc_not_found"},
    {e_dbproc_invalid_reply,false,sqlm_internal,-28608,"Invalid reply for stored procedure","dbproc_invalid_reply"},
    {e_dbproc_packet_missing,false,sqlm_internal,-28607,"Missing object data packet","dbproc_packet_missing"},
    {e_dbproc_comm_error,false,sqlm_internal,-28606,"Communication error","dbproc_comm_error"},
    {e_dbproc_invalid_operation,false,sqlm_internal,-28605," Invalid operation in UDE Server-Kernel communication","dbproc_invalid_operation"},
    {e_dbproc_too_many_packets,false,sqlm_internal,-28604,"Trying to queue too many data packets when sending object data","dbproc_too_many_packets"},
    {e_dbproc_packet_too_big,false,sqlm_internal,-28603,"Requested packet size to transfer an object is too big","dbproc_packet_too_big"},
    {e_dbproc_packet_size,false,sqlm_internal,-28602,"Packet size does not match object size","dbproc_packet_size"},
    {e_dbproc_no_packet,false,sqlm_internal,-28601,"Object data packet for read missing","dbproc_no_packet"},
    {e_dbproc_invalid_packet,false,sqlm_internal,-28600,"Invalid request code in packet","dbproc_invalid_packet"},
    {e_internal_obj_link_corruption,false,sqlm_internal,-28548,"Internal object link corruption","internal_obj_link_corruption"},
    {e_wrong_class_entry_ptr,false,sqlm_internal,-28547,"Wrong class entry pointer","wrong_class_entry_ptr"},
    {e_internal_stream_error,false,sqlm_internal,-28546,"Internal stream error","internal_stream_error"}, /* PTS 1135058, PG */
    {e_container_already_exists,false,sqlm_internal,-28545,"Container exists already","container_already_exists"},
    {e_reset_ostream_not_supported,false,sqlm_internal,-28544,"Resetting output streams not supported by client","reset_ostream_not_supported"}, /* PTS 1131114, PG */
    {e_reset_istream_not_supported,false,sqlm_internal,-28543,"Resetting input streams not supported by client","reset_istream_not_supported"},  /* PTS 1131114, PG */
    {e_param_check,false,sqlm_internal,-28541,"DB procedure parameters invalid","param_check"},
    {e_invalid_csid,false,sqlm_internal,-28540,"Invalid OmsCriticalSection ID","invalid_csid"},
    {e_invalid_rowsize,false,sqlm_internal,-28539,"Invalid ABAP table/stream rowsize","invalid_rowsize"},
    {e_wrong_key,false,sqlm_internal,-28538,"Inheritance-error: Object has wrong key","wrong_key"},    /* PTS 1124070 */
    {e_no_keyed_object,false,sqlm_internal,-28537,"Inheritance_error: Object has no key","no_keyed_object"}, /* PTS 1124070 */
    {e_longpacked_out_of_range,false,sqlm_internal,-28535,"High or low integer out of range for long packed","longpacked_out_of_range"},
    {e_package_out_of_date,false,sqlm_internal,-28534,"Package must be recreated","package_out_of_date"},
    {e_packed_out_of_range,false,sqlm_internal,-28533,"Integer out of packed range","packed_out_of_range"},
    {e_version_bound_by_trans,false,sqlm_internal,-28532,"Version bound by transaction","version_bound_by_trans"},
    {e_oms_read_only,false,sqlm_internal,-28531,"OMS read only","oms_read_only"},
    {e_terminate,false,sqlm_internal,-28530,"omsTerminate called","terminate"},
    {e_buffer_too_small,false,sqlm_internal,-28529,"Too small name buffer","buffer_too_small"},
    {e_lib_not_released,false,sqlm_internal,-28528,"Library not released","lib_not_released"},
    {e_directory_not_found,false,sqlm_internal,-28527,"Directory not found","directory_not_found"},
    {e_proc_address_not_found,false,sqlm_internal,-28525,"Procedure Address not found","proc_address_not_found"},
    {e_duplicate_schema,false,sqlm_internal,-28524,"Duplicate schema","duplicate_schema"},
    {e_unknown_schema,false,sqlm_internal,-28523,"Unknown schema","unknown_schema"},
    {e_missing_before_image,false,sqlm_internal,-28522,"Missing before image","missing_before_image"},
    {e_already_in_version,false,sqlm_internal,-28521,"Already in version","already_in_version"},
    {e_not_in_critical_section,false,sqlm_internal,-28518,"Not in critical section","not_in_critical_section"},
    {e_already_in_critical_section,false,sqlm_internal,-28517,"Already in critical section","already_in_critical_section"},
    {e_transaction_end_required,false,sqlm_internal,-28516,"Transaction end required","transaction_end_required"},
    {e_version_already_open,false,sqlm_internal,-28515,"Version already open","version_already_open"},
    {e_unknown_version,false,sqlm_internal,-28514,"Unknown version","unknown_version"},
    {e_method_not_found,false,sqlm_internal,-28513,"Method not found","method_not_found"},
    {e_localserver_not_found,false,sqlm_internal,-28512,"LocalServer not found","localserver_not_found"},
    {e_inprocserver_not_found,false,sqlm_internal,-28511,"InProcServer not found","inprocserver_not_found"},
    {e_progid_not_found,false,sqlm_internal,-28509,"Progid not found","progid_not_found"},
    {e_unsupported_param_type,false,sqlm_internal,-28508,"Unsupported parameter type","unsupported_param_type"},
    {e_no_coclass_obj_found,false,sqlm_internal,-28505,"No Component Object found","no_coclass_obj_found"},
    {e_com_object_crashed,false,sqlm_internal,-28503,"Crash in DB-Procedure","com_object_crashed"},
    {e_oms_ostream_error,false,sqlm_internal,-28502,"Error writing OMS output stream","oms_ostream_error"},
    {e_oms_istream_error,false,sqlm_internal,-28501,"Error reading OMS input stream","oms_istream_error"},
    {e_var_obj_overwritten,false,sqlm_internal,-28007,"Var-object has been overwritten","var_obj_overwritten"},
    {e_incompatible_oid,false,sqlm_internal,-28007,"OID not compatible with given class","incompatible_oid"},
    {e_object_not_locked,false,sqlm_internal,-28006,"Object not locked","object_not_locked"},
    {e_duplicate_object,false,sqlm_internal,-28005,"Duplicate object","duplicate_object"},
    {e_unknown_guid,false,sqlm_internal,-28003,"Unknown GUID","unknown_guid"},
    {e_invalid_oid,false,sqlm_internal,-28002,"Invalid OID","invalid_oid"},
    {e_nil_pointer,false,sqlm_internal,-28001,"Access via NIL OID","nil_pointer"},
    {e_new_failed,false,sqlm_internal,-28000,"Operator NEW failed","new_failed"},
    {e_no_sqldbc_connection,false,sqlm_internal,-27020,"SQLDBC connection not established", "no_sqldbc_connection"},
    {e_uni_to_ascii_conv_failed,false,sqlm_internal,-27019,"Conversion UNICODE to ASCII not possible","uni_to_ascii_conv_failed"},
    {e_cmd_not_available,false,sqlm_internal,-27013,"Command not available in this mode","cmd_not_available"},
    {e_use_callstm,false,sqlm_internal,-27012,"PREPARE not permitted for this statement","use_callstm"},
    {e_inp_string_truncated,false,sqlm_internal,-27011,"Input variable has been truncated","inp_string_truncated"},
    {e_copyop_not_allowed,false,sqlm_internal,-27010,"Invalid use of SQL::COPY operator","copyop_not_allowed"},
    {e_cmd_too_long,false,sqlm_internal,-27009,"SQL statement too long (buffer overflow)","cmd_too_long"},
    {e_missing_variable_addr,false,sqlm_internal,-27008,"Missing variable address in SQLDA","missing_variable_addr"},
    {e_too_many_hostvar,false,sqlm_internal,-27007,"Too many parameters, SQLDA too short","too_many_hostvar"},
    {e_incompatible_datatype,false,sqlm_internal,-27006,"Incompatible data types","incompatible_datatype"},
    {e_param_list_too_short,false,sqlm_internal,-27005,"Parameter list too short","param_list_too_short"},
    {e_param_list_too_long,false,sqlm_internal,-27004,"Parameter list too long","param_list_too_long"},
    {e_inp_number_overflow,false,sqlm_internal,-27003,"Numeric input parameter overflow","inp_number_overflow"},
    {e_inp_number_invalid,false,sqlm_internal,-27002,"Invalid numeric input parameter value","inp_number_invalid"},
    {e_inp_null_not_allowed,false,sqlm_internal,-27001,"Input parameter cannot be NULL value","inp_null_not_allowed"},
    {e_unknown_error,false,sqlm_internal,-27000,"Unknown error","unknown_error"},
    {-9999,false,sqlm_internal,-9999,"System error: Otherwise unknown errorcode","unknown_errorcode"},
    {e_standby_info_not_ok,false,sqlm_internal,-9500,"System error: RT Info standby not found","standby_info_not_ok"},
    {e_conf_parameter_error,false,sqlm_internal,-9414,"System error: Configuration parameter error","conf_parameter_error"},
    {e_too_small_mb_qual_part,false,sqlm_internal,-9412,"System error: MB Qual too short","too_small_mb_qual_part"},
    {e_too_many_intern_parsinfos,false,sqlm_internal,-9411,"System error: AK Too many intern parsinfos","too_many_intern_parsinfos"},
    {e_invalid_segment_construction,false,sqlm_internal,-9409,"System error: AK Invalid segment construction","invalid_segment_construction"},
    {e_unitrans_src_too_short,false,sqlm_internal,-9408,"System error: AK Unitrans SRC too short","unitrans_src_too_short"},
    {e_system_error,false,sqlm_internal,-9407,"System error: unexpected error","system_error"},
    {-9406,false,sqlm_internal,-9406,"System error: DDL trigger failed"},
    {e_too_short_datapart,false,sqlm_internal,-9405,"System error: AK Datapart too short","too_short_datapart"},
    {e_ak_system_error,false,sqlm_internal,-9404,"System error: AK System error","system_error"},
    {e_internal_error,false,sqlm_internal,-9403,"System error: unexpected error","internal_error"},
    {e_unknown_strategy,false,sqlm_internal,-9402,"System error: AK Unexpected strategy information","unknown_strategy"},
    {e_cachedirectory_full,false,sqlm_internal,-9400,"System error: AK Cachedirectory full","cachedirectory_full"},
    {e_nil_transindex,false,sqlm_internal,-9232,"System error: KB Nil transindex","nil_transindex"},
    {e_queue_mismatch,false,sqlm_internal,-9228,"System error: KB Queue mismatch","queue_mismatch"},
    {e_skip_key_upd,false,sqlm_internal,-9226,"System error: KB Skip key update","skip_key_upd"},
    {e_no_more_jobs_to_wait_for,false,sqlm_internal,-9225,"System error: no parallel job to wait for","no_more_jobs_to_wait_for"},
    {e_start_server_task_error,false,sqlm_internal,-9224,"System error: server task could not be started","start_server_task_error"},
    {e_response_timeout,false,sqlm_internal,-9221,"System error: KB Net response timeout","response_timeout"},
    {e_stack_type_illegal,false,sqlm_internal,-9211,"System error: KB Stack type illegal","stack_type_illegal"},
    {e_stack_op_illegal,false,sqlm_internal,-9210,"System error: KB Stack op illegal","stack_op_illegal"},
    {e_log_error,false,sqlm_internal,-9209,"System error: KB Log error","log_error"},
    {e_redo_log_read_error,false,sqlm_internal,-9209,"System error: KB Log error","redo_log_read_error"},
    {e_redo_log_write_error,false,sqlm_internal,-9209,"System error: KB Log error","redo_log_write_error"},
    {e_undo_log_write_error,false,sqlm_internal,-9209,"System error: KB Log error","undo_log_write_error"},
    {e_undo_log_execute_error,false,sqlm_internal,-9209,"System error: KB Log error","undo_log_execute_error"},
    {e_redo_log_execute_error,false,sqlm_internal,-9209,"System error: KB Log error","redo_log_execute_error"},
    {e_history_write_error,false,sqlm_internal,-9209,"System error: KB Log error","history_write_error"},
    {e_history_read_error,false,sqlm_internal,-9209,"System error: KB Log error","history_read_error"},
    {e_history_remove_error,false,sqlm_internal,-9209,"System error: KB Log error","history_remove_error"},
    {e_logpage_not_found,false,sqlm_internal,-9209,"System error: KB Log error","logpage_not_found"},
    {e_duplicate_sysinfo,false,sqlm_internal,-9206,"System error: AK Duplicate catalog information","duplicate_sysinfo"},
    {e_sysinfo_not_found,false,sqlm_internal,-9205,"System error: AK Catalog information not found","sysinfo_not_found"},
    {e_skip_upd,false,sqlm_internal,-9203,"System error: KB Skip upd","skip_upd"},
    {e_enough_resultsets,false,sqlm_internal,-9202,"System error: KB Enough resultsets","enough_resultsets"},
    {e_hotstandby_free_memory_failed,false,sqlm_internal,-9195,"System Error: HotStandby: Free Memory failed (Prepare Standby)","hotstandby_free_memory_failed"},
    {e_hotstandby_del_node_failed,false,sqlm_internal,-9194,"System Error: HotStandby: Delete node from internal node list failed","hotstandby_del_node_failed"},
    {e_hotstandby_add_node_failed,false,sqlm_internal,-9193,"System Error: HotStandby: Add node to internal node list failed","hotstandby_add_node_failed"},
    {e_hotstandby_establish_mirror_failed,false,sqlm_internal,-9192,"System Error: HotStandby: Establish mirror failed","hotstandby_establish_mirror_failed"},
    {e_hotstandby_wrong_role,false,sqlm_internal,-9191,"System Error: HotStandby: Illegal command in this standby role","hotstandby_wrong_role"},
    {e_hotstandby_cmd_send_failed,false,sqlm_internal,-9190,"System Error: HotStandby: Sending command to other standby node failed","hotstandby_cmd_send_failed"},
    {e_move_error,false,sqlm_internal,-9111,"System error: Move error","move_error"},
    {e_long_column_missing,false,sqlm_internal,-9055,"System error: BD Long column missing","long_column_missing"},
    {e_data_page_corrupted,false,sqlm_internal,-9053,"System error: BD Data page corrupted","data_page_corrupted"},
    {e_invalid_fbm_mark,false,sqlm_internal,-9051,"System error: BD Invalid FBM mark","invalid_fbm_mark"},
    {e_primkey_not_found,false,sqlm_internal,-9049,"System error: BD Primkey from inv in primtree not found","primkey_not_found"},
    {e_invalid_filetype,false,sqlm_internal,-9047,"System error: BD Invalid filetype","invalid_filetype"},
    {e_no_converter_entry,false,sqlm_internal,-9046,"System error: BD No converter entry","no_converter_entry"},
    {e_root_check,false,sqlm_internal,-9045,"System error: BD Root check","root_check"},
    {e_inconsistent_nodetype,false,sqlm_internal,-9044,"System error: BD Inconsistent nodetype","inconsistent_nodetype"},
    {e_do_not_access_fdir,false,sqlm_internal,-9043,"System error: BD Fdir access not allowed","do_not_access_fdir"},
    {e_append_not_allowed,false,sqlm_internal,-9042,"System error: BD Append not allowed","append_not_allowed"},
    {e_file_not_accessible,false,sqlm_internal,-9041,"System error: BD Index not accessible","index_not_accessible"},
    {e_do_not_drop_fdir,false,sqlm_internal,-9040,"System error: BD Dropping of fdir not allowed","do_not_drop_fdir"},
    {e_bad_conv_bitmap_page,false,sqlm_internal,-9033,"System error: Bad converter bitmap page","bad_conv_bitmap_page"},
    {e_bad_conv_page,false,sqlm_internal,-9032,"System error: BD Bad converterpage","bad_conv_page"},
    {e_bad_logpage,false,sqlm_internal,-9030,"System error: BD Bad logpage","bad_logpage"},
    {e_bad_invfile,false,sqlm_internal,-9029,"System error: BD Bad invfile","bad_invfile"},
    {e_bad_file,false,sqlm_internal,-9028,"System error: BD Bad file","bad_file"},
    {e_bad_fdir,false,sqlm_internal,-9027,"System error: BD Bad fdir","bad_fdir"},
    {e_bad_datapage,false,sqlm_internal,-9026,"System error: BD Bad datapage","bad_datapage"},
    {e_invalid_entrypos,false,sqlm_internal,-9024,"System error: BD Invalid entrypos","invalid_entrypos"},
    {e_illegal_entrypos,false,sqlm_internal,-9023,"System error: BD Illegal entrypos","illegal_entrypos"},
    {e_corrupted_datapage_from_io,false,sqlm_internal,-9022,"System error: BD Corrupted datapage","corrupted_datapage"},
    {e_init_missing,false,sqlm_internal,-9020,"System error: BD Init missing","init_missing"},
    {e_buffer_limit,false,sqlm_internal,-9019,"System error: BD Buffer limit","buffer_limit"},
    {e_page_in_wrong_tree,false,sqlm_internal,-9018,"System error: BD Page in wrong tree","page_in_wrong_tree"},
    {e_no_prev_invkey,false,sqlm_internal,-9016,"System error: BD No prev invkey","no_prev_invkey"},
    {e_no_next_invkey,false,sqlm_internal,-9015,"System error: BD No next invey","no_next_invkey"},
    {e_invalid_leaves_structure,false,sqlm_internal,-9014,"System error: BD Invalid leaves structure","invalid_leaves_structure"},
    {e_invalid_index_structure,false,sqlm_internal,-9013,"System error: BD Invalid index structure","invalid_index_structure"},
    {e_inv_not_found,false,sqlm_internal,-9012,"System error: BD Inv not found","inv_not_found"},
    {e_inv_list_not_found,false,sqlm_internal,-9011,"System error: BD Inv list not found","inv_list_not_found"},
    {e_invalid_invlistpos,false,sqlm_internal,-9010,"System error: BD Invalid invlistpos","invalid_invlistpos"},
    {e_illegal_record,false,sqlm_internal,-9008,"System error: BD Illegal record","illegal_record"},
    {e_illegal_page_no,false,sqlm_internal,-9007,"System error: BD Illegal page no","illegal_page_no"},
    {e_illegal_keylength,false,sqlm_internal,-9006,"System error: BD Illegal keylength","illegal_keylength"},
    {e_illegal_key,false,sqlm_internal,-9005,"System error: BD Illegal key","illegal_key"},
    {e_illegal_filename,false,sqlm_internal,-9004,"System error: BD Illegal file name","illegal_filename"},
    {e_illegal_entrylength,false,sqlm_internal,-9003,"System error: BD Illegal entrylength","illegal_entrylength"},
    {e_illegal_branchlength,false,sqlm_internal,-9002,"System error: BD Illegal branchlength","illegal_branchlength"},
    {e_invalid_root,false,sqlm_internal,-9001,"System error: BD Invalid root","invalid_root"},
    {e_not_implemented,false,sqlm_internal,-9000,"System error: Not yet implemented","not_implemented"},
    {-8055, false,sqlm_internal,-8055,"Trigger failed"},
    {e_software_too_old,false,sqlm_internal,-8053,"Software version is too old for version of persistent data","software_too_old"}, // UH 2005-01-20
    /*
     * -8052 used for error message returned by substitute triggers
     * that were created for broken triggers during the unicode
     * migration; message is 'trigger dropped during migration; check
     * table postupgradechecklistunicode'
    */
    {e_no_snapshot_available,false,sqlm_internal,-8051,"No snapshot available","no_snapshot_available"},
    {e_migration_error,false,sqlm_internal,-8050,"Migration error","migration_error"},
    {e_not_prepared_for_migration,false,sqlm_internal,-8049,"Prepare migration required","not_prepared_for_migration"},
    {e_too_old_tool,false,sqlm_internal,-8048,"Version of component must be compatible with kernel version","too_old_tool"},
    {e_missing_password,false,sqlm_internal,-8047,"Password required","missing_password"},
    {e_not_trans_from_uni,false,sqlm_internal,-8046,"Conversion from UNICODE impossible","not_trans_from_uni"},
    {e_not_trans_to_uni,false,sqlm_internal,-8045,"Conversion to UNICODE impossible","not_trans_to_uni"},
    {e_not_translatable,false,sqlm_internal,-8044,"Conversion for character set value impossible","not_translatable"},
    {e_bd_leaf_unlocked,false,sqlm_internal,-8043,"Unlock of primary data page leaf", "leaf_unlocked"},
    {e_rollback_wanted,false,sqlm_internal,-8042,"Rollback wanted", "rollback_wanted"},
    {e_key_does_not_exist,false,sqlm_internal,-8040,"Key does not exist","key_does_not_exist"},
    {e_procedure_must_be_recompiled,false,sqlm_internal,-8037,"DB procedure must be recompiled","procedure_must_be_recompiled"},
    {e_foreign_key_must_exist,false,sqlm_internal,-8035,"Foreign key must exist","foreign_key_must_exist"},
    {e_order_col_must_be_number,false,sqlm_internal,-8034,"Order column must be number","order_col_must_be_number"},
    {e_incomplete_logsegm,false,sqlm_internal,-8033,"Incomplete log segment","incomplete_logsegm"},
    {e_owner_must_be_specified,false,sqlm_internal,-8031,"Schema must be specified","owner_must_be_specified"},
    {e_column_not_indexed,false,sqlm_internal,-8030,"Column must be indexed","column_not_indexed"},
    {e_no_servers,false,sqlm_internal,-8029,"No servers installed","no_servers"},
    {e_union_cols_mustbe_compatible,false,sqlm_internal,-8028,"UNION columns must be compatible","union_cols_mustbe_compatible"},
    {e_connection_disabled,false,sqlm_internal,-8026,"Connect disabled for this user","connect_disabled"},
    {e_table_must_be_temp,false,sqlm_internal,-8025,"Table must be temp table","table_must_be_temp"},
    {e_restore_catalog_error,false,sqlm_internal,-8024,"RESTORE CATALOG failed","restore_catalog_error"},
    {e_invalid_costwarning,false,sqlm_internal,-8023,"COSTLIMIT must be greater than COSTWARNING","invalid_costwarning"},
    {e_user_already_connected,false,sqlm_internal,-8022,"User already connected","user_already_connected"},
    {e_column_must_have_default,false,sqlm_internal,-8021,"Column must have DEFAULT specification","column_must_have_default"},
    {e_new_hostfile_required,false,sqlm_internal,-8020,"Next volume required","new_hostfile_required"},
    {e_indexname_must_be_unique,false,sqlm_internal,-8018,"Index name must be unique","indexname_must_be_unique"},
    {e_output_field_must_be_group,false,sqlm_internal,-8017,"Column must be group column","output_field_must_be_group"},
    {e_missing_viewtable,false,sqlm_internal,-8016,"Table must be a view table","missing_viewtable"},
    {e_missing_basetable,false,sqlm_internal,-8015,"Table must be a base table","missing_basetable"},
    {e_missing_reuseable,false,sqlm_internal,-8014,"Result table must be FOR REUSE","missing_reuseable"},
    {e_unknown_ordercolumn,false,sqlm_internal,-8013,"Order column must be output column","unknown_ordercolumn"},
    {e_table_not_in_from_list,false,sqlm_internal,-8010,"Table name must be in from list","table_not_in_from_list"},
    {e_default_not_in_range,false,sqlm_internal,-8009,"DEFAULT value must be in constraint specification","default_not_in_range"},
    {e_column_must_be_keycolumn,false,sqlm_internal,-8007,"Column must be key column","column_must_be_keycolumn"},
    {e_incompatible_datatypes,false,sqlm_internal,-8006,"Data types must be compatible","incompatible_datatypes"},
    {e_value_expr_must_be_param_name,false,sqlm_internal,-8005,"Value expression must be parameter name","value_expr_must_be_param_name"},
    {e_const_incompatible_with_typ,false,sqlm_internal,-8004,"Constant must be compatible with column type and length","const_incompatible_with_typ"},
    {e_incompatible_log,false,sqlm_internal,-8003,"Log and data must be compatible","incompatible_log"},
    {e_user_connected_to_same_task,false,sqlm_internal,-8002,"User already connected to this user task","user_connected_to_same_task"},
    {e_user_not_connected,false,sqlm_internal,-8001,"User must be connected","user_not_connected"},
    {e_shutdown,false,sqlm_internal,-8000,"SERVERDB must be restarted","shutdown"},
    {e_bad_pagetype,false,sqlm_internal,-7900,"Different block sizes","bad_pagetype"},
    {e_dcom_hresult,false,sqlm_internal,-7500,"DCOM HRESULT","dcom_hresult"},
    {e_st_col_not_open,false,sqlm_internal,-7201,"Long column is not open","st_col_not_open"},
    {e_st_col_open_read_only,false,sqlm_internal,-7200,"Long column was opened READ ONLY","st_col_open_read_only"},
    {e_schema_not_allowed,false,sqlm_internal,-7086,"Schema not allowed","schema_not_allowed"},
    {e_scrolling_fetch_not_allowed,false,sqlm_internal,-7085,"Function not allowed for this cursor","scrolling_fetch_not_allowed"},
    {e_parameter_change_not_allowed,false,sqlm_internal,-7084,"Parameter change not allowed","parameter_change_not_allowed"},
    {e_replicated_table_not_allowed,false,sqlm_internal,-7083,"Replicated table not allowed","replicated_table_not_allowed"},
    {e_invalid_tabletype,false,sqlm_internal,-7082,"SQL statement not allowed for this table type","invalid_tabletype"},
    {e_no_standby_configuration,false,sqlm_internal,-7081,"Hot Standby not configured","no_standby_configuration"},
    {e_query_not_allowed,false,sqlm_internal,-7080,"Query in $1 statement not allowed","query_not_allowed"},
    {e_backup_is_running,false,sqlm_internal,-7079,"A backup is already running","backup_is_running"},
    {e_backup_not_running,false,sqlm_internal,-7078,"No backup is running","backup_not_running"},
    {e_synonym_not_allowed,false,sqlm_internal,-7077,"Synonym not allowed","synonym_not_allowed"},
    {e_no_data_on_tape,false,sqlm_internal,-7076,"No more data to read from tape","no_data_on_tape"},
    {e_save_skipped,false,sqlm_internal,-7075,"Current save skipped, next is ready to take on this tape","save_skipped"},
    {e_role_cycle,false,sqlm_internal,-7074,"Cycle in granted roles not allowed","role_cycle"},
    {e_internal_recreate_failed, false,sqlm_internal,-7073,"Internal recreate failed with $1, $2","recreate_failed"},
    {e_too_many_serials,false,sqlm_internal,-7072,"More than one column with default SERIAL not allowed","too_many_serials"},
    {e_autosave_running,false,sqlm_internal,-7071,"AUTOSAVE LOG must be stopped","autosave_running"},
    {e_outer_join_not_allowed,false,sqlm_internal,-7068,"Outer join not allowed in this context","outer_join_not_allowed"},
    {e_lock_dirty,false,sqlm_internal,-7067,"Update without repeated read not allowed","lock_dirty"},
    {e_referenced_index_not_allowed,false,sqlm_internal,-7066,"Unique index referenced by foreign key not allowed","referenced_index_not_allowed"},
    {e_no_open_subtrans,false,sqlm_internal,-7065,"SUBTRANS COMMIT/ROLLBACK not allowed without SUBTRANS BEGIN","no_open_subtrans"},
    {e_not_allowed_in_temp_table,false,sqlm_internal,-7064,"Feature not allowed in temp table definition","not_allowed_in_temp_table"},
    {e_temp_table_not_allowed,false,sqlm_internal,-7063,"Temp table not allowed","temp_table_not_allowed"},
    {e_lock_row_not_allowed,false,sqlm_internal,-7061,"LOCK ROW for this table not allowed","lock_row_not_allowed"},
    {e_remote_load_not_allowed,false,sqlm_internal,-7060,"Remote TABLE LOAD/UNLOAD not allowed","remote_load_not_allowed"},
    {e_from_select_not_allowed,false,sqlm_internal,-7058,"Correlated subquery with select in from clause not allowed","from_select_not_allowed"},
    {e_view_def_contradicts_table_de,false,sqlm_internal,-7057,"View definition contradicts new table definition","view_def_contradicts_table_de"},
    {e_subquery_not_allowed,false,sqlm_internal,-7056,"Subquery not allowed","subquery_not_allowed"},
    {e_column_indexed,false,sqlm_internal,-7055,"Column(s) already indexed","column_indexed"},
    {e_check_option_not_allowed,false,sqlm_internal,-7051,"Read only view WITH CHECK OPTION not allowed","check_option_not_allowed"},
    {e_indexed_table_not_allowed,false,sqlm_internal,-7050,"Fastload with indexed table not allowed","indexed_table_not_allowed"},
    {e_alter_drop_user_not_allowed,false,sqlm_internal,-7048,"DROP/ALTER USER not allowed while user is connected","alter_drop_user_not_allowed"},
    {e_varchar_not_allowed,false,sqlm_internal,-7046,"Column with length greater than 254 not allowed","varchar_not_allowed"},
    {e_parameter_not_allowed,false,sqlm_internal,-7045,"Parameter spec not allowed","parameter_not_allowed"},
    {e_group_user_not_allowed,false,sqlm_internal,-7043,"Group member not allowed","group_user_not_allowed"},
    {e_key_update,false,sqlm_internal,-7042,"Key update not allowed","key_update"},
    {e_no_resulttable_allowed,false,sqlm_internal,-7041,"Result table name not allowed","no_resulttable_allowed"},
    {e_link_rule_not_allowed,false,sqlm_internal,-7040,"Referential constraint not allowed","link_rule_not_allowed"},
    {e_link_cycle_not_allowed,false,sqlm_internal,-7039,"Cycle in referential constraint definition not allowed","link_cycle_not_allowed"},
    {e_link_column_not_allowed,false,sqlm_internal,-7038,"Foreign key column not allowed","link_column_not_allowed"},
    {e_null_not_allowed,false,sqlm_internal,-7037,"NULL value for NOT NULL column not allowed","null_not_allowed"},
    {e_rowno_not_allowed,false,sqlm_internal,-7036,"ROWNO specification not allowed in this context","rowno_not_allowed"},
    {e_default_spec_not_allowed,false,sqlm_internal,-7035,"DEFAULT specification not allowed for this column","default_spec_not_allowed"},
    {e_command_for_string_not_allow,false,sqlm_internal,-7032,"SQL statement not allowed for column of data type LONG","command_for_string_not_allow"},
    {e_function_not_allowed,false,sqlm_internal,-7029,"Function not allowed for this data type","function_not_allowed"},
    {e_current_of_needs_fetch,false,sqlm_internal,-7026,"SQL statement not allowed without previous FETCH","current_of_needs_fetch"},
    {e_alter_column_not_allowed,false,sqlm_internal,-7024,"Alter column not allowed, constraint violation","alter_column_not_allowed"},
    {e_too_many_cols_dropped,false,sqlm_internal,-7023,"Dropping of all columns not allowed","too_many_cols_dropped"},
    {e_only_one_recordtype_allowed,false,sqlm_internal,-7022,"Columns from more than one table not allowed","only_one_recordtype_allowed"},
    {e_invalid_view_column,false,sqlm_internal,-7021,"View column derived from function not allowed","invalid_view_column"},
    {e_in_view_not_allowed,false,sqlm_internal,-7019,"Feature not allowed in view definition","in_view_not_allowed"},
    {e_variable_not_allowed,false,sqlm_internal,-7018,"Variable not allowed in view definition","variable_not_allowed"},
    {e_user_not_allowed,false,sqlm_internal,-7017,"User not allowed","user_not_allowed"},
    {e_without_datatypes,false,sqlm_internal,-7016,"Parameter spec not allowed in this context","without_datatypes"},
    {e_too_many_resultsets,false,sqlm_internal,-7015,"More than one result row not allowed","too_many_resultsets"},
    {e_too_many_updatesets,false,sqlm_internal,-7012,"More than one update row not allowed","too_many_updatesets"},
    {e_corelated_subquery_not_allowe,false,sqlm_internal,-7011,"Correlated subquery not allowed","corelated_subquery_not_allowe"},
    {e_null_value_illegal,false,sqlm_internal,-7009,"NULL value in key column not allowed","null_value_illegal"},
    {e_update_not_allowed,false,sqlm_internal,-7008,"Updates of this table not allowed","update_not_allowed"},
    {e_col_must_be_no_keycolumn,false,sqlm_internal,-7007,"Key column not allowed","col_must_be_no_keycolumn"},
    {e_reserved_identifier,false,sqlm_internal,-7006,"Reserved identifier not allowed","reserved_identifier"},
    {e_invalid_range,false,sqlm_internal,-7005,"Lower bound greater than upper bound not allowed","invalid_range"},
    {e_default_not_allowed,false,sqlm_internal,-7004,"DEFAULT not allowed","default_not_allowed"},
    {e_not_null_not_allowed,false,sqlm_internal,-7003,"NOT NULL not allowed","not_null_not_allowed"},
    {e_key_not_allowed,false,sqlm_internal,-7002,"KEY not allowed","key_not_allowed"},
    {e_duplicate_replication_user,false,sqlm_internal,-6017,"Duplicate replication user definition","duplicate_replication_user"},
    {e_to_date_duplicate_format,false,sqlm_internal,-6016,"Duplicate format code","to_date_duplicate_format"},
    {e_duplicate_value,false,sqlm_internal,-6014,"Duplicate value","duplicate_value"},
    {e_duplicate_trigger_def,false,sqlm_internal,-6013,"Duplicate trigger definition","duplicate_trigger_def"},
    {e_duplicate_default,false,sqlm_internal,-6012,"Duplicate default","duplicate_default"},
    {e_duplicate_constraint,false,sqlm_internal,-6011,"Duplicate constraint","duplicate_constraint"},
    {e_duplicate_referencename,false,sqlm_internal,-6009,"Duplicate reference name","duplicate_referencename"},
    {e_duplicate_indexname,true,sqlm_ansi,-6008,"Duplicate index name",""},
    {e_duplicate_indexname,true,sqlm_db2,-6008,"Duplicate index name",""},
    {e_duplicate_indexname,true,sqlm_internal,-6008,"Duplicate index name","duplicate_indexname"},
    {e_duplicate_name,true,sqlm_ansi,-6006,"Duplicate name",""},
    {e_duplicate_name,true,sqlm_db2,-6006,"Duplicate name",""},
    {e_duplicate_name,true,sqlm_internal,-6006,"Duplicate name","duplicate_name"},
    {e_duplicate_resultname,false,sqlm_internal,-6003,"Duplicate result table name","duplicate_resultname"},
    {e_duplicate_privilege,false,sqlm_internal,-6002,"Duplicate privilege","duplicate_privilege"},
    {e_duplicate_columnname,false,sqlm_internal,-6001,"Duplicate column name","duplicate_columnname"},
    {e_duplicate_filename,true,sqlm_ansi,-6000,"Duplicate table name",""},
    {e_duplicate_filename,true,sqlm_db2,-6000,"Duplicate table name",""},
    {e_duplicate_filename,true,sqlm_internal,-6000,"Duplicate table name","duplicate_filename"},
    {e_duplicate_tablename,true,sqlm_ansi,-6000,"Duplicate table name",""},
    {e_duplicate_tablename,true,sqlm_db2,-6000,"Duplicate table name",""},
    {e_duplicate_tablename,true,sqlm_internal,-6000,"Duplicate table name","duplicate_tablename"},
    {e_missing_delimiter,false,sqlm_internal,-5016,"Missing delimiter","missing_delimiter"},
    {e_wanted_keyword,false,sqlm_internal,-5015,"Missing keyword","wanted_keyword"},
    {e_missing_column_definite,false,sqlm_internal,-5014,"Missing definite column name","missing_column_definite"},
    {e_missing_update_column,false,sqlm_internal,-5013,"Missing update column","missing_update_column"},
    {e_missing_tablename_definite,false,sqlm_internal,-5012,"Missing definite table name","missing_tablename_definite"},
    {e_missing_value_spec,false,sqlm_internal,-5010,"Missing value specification","missing_value_spec"},
    {e_missing_integer,false,sqlm_internal,-5009,"Missing integer","missing_integer"},
    {e_missing_string_literal,false,sqlm_internal,-5008,"Missing string constant","missing_string_literal"},
    {e_missing_number,false,sqlm_internal,-5007,"Missing numeric constant","missing_number"},
    {e_missing_identifier,false,sqlm_internal,-5006,"Missing identifier","missing_identifier"},
    {e_mandatory_field_is_null,false,sqlm_internal,-5005,"Missing non-NULL value","mandatory_field_is_null"},
    {e_missing_mandatory_recordfield,false,sqlm_internal,-5005,"Missing non-NULL value","missing_mandatory_recordfield"},
    {e_missing_constant,false,sqlm_internal,-5004,"Missing constant","missing_constant"},
    {e_missing_keyfield,false,sqlm_internal,-5003,"Missing key specification","missing_keyfield"},
    {e_rec_not_locked,false,sqlm_internal,-5002,"Missing exclusive lock","rec_not_locked"},
    {e_missing_privilege,false,sqlm_internal,-5001,"Missing privilege","missing_privilege"},
    {e_unknown_sql_schema,false,sqlm_internal,-4030,"Unknown schema","unknown_sql_schema"},
    {e_no_basic_authentication,false,sqlm_internal,-4029,"Authentication method 'BASIC' disabled"},
    {e_no_matching_authentication,false,sqlm_internal,-4028,"Unknown authentication method"},
    {e_unknown_session,false,sqlm_internal,-4027,"Unknown session","unknown_session"},
    {e_unknown_multibyte_set,false,sqlm_internal,-4025,"Unknown or not loaded character set","unknown_multibyte_set"},
    {e_unknown_name,false,sqlm_internal,-4024,"Unknown name","unknown_name"},
    {e_unknown_functionname,true,sqlm_db2,-4023,"Unknown function name",""},
    {e_unknown_functionname,true,sqlm_oracle,-4023,"Unknown function name",""},
    {e_unknown_functionname,true,sqlm_internal,-4023,"Unknown function name","unknown_functionname"},
    {e_unknown_sequencename,false,sqlm_internal,-4020,"Unknown sequence name","unknown_sequencename"},
    {e_unknown_trigger,false,sqlm_internal,-4019,"Unknown trigger name","unknown_trigger"},
    {e_unknown_default,false,sqlm_internal,-4018,"Unknown default","unknown_default"},
    {e_unknown_constraint,false,sqlm_internal,-4017,"Unknown constraint","unknown_constraint"},
    {e_unknown_procedure_name,false,sqlm_internal,-4016,"Unknown procedure name","unknown_procedure_name"},
    {e_unknown_name_of_serverdb,false,sqlm_internal,-4014,"Unknown SERVERDB name","unknown_name_of_serverdb"},
    {e_unknown_groupname,false,sqlm_internal,-4013,"Unknown usergroup name","unknown_groupname"},
    {e_unknown_parseid,false,sqlm_internal,-4012,"Unknown parseid","unknown_parseid"},
    {e_unknown_indexname,false,sqlm_internal,-4011,"Unknown index name","unknown_indexname"},
    {e_unknown_mapset,false,sqlm_internal,-4010,"Unknown MAPCHAR SET name","unknown_mapset"},
    {e_unknown_linkname,false,sqlm_internal,-4009,"Unknown referential constraint name","unknown_linkname"},
    {e_unknown_user_password_pair,false,sqlm_internal,-4008,"Unknown user name/password combination","unknown_user_password_pair"},
    {e_unknown_synonymname,false,sqlm_internal,-4007,"Unknown synonym name","unknown_synonymname"},
    {e_unknown_domainname,false,sqlm_internal,-4006,"Unknown domain name","unknown_domainname"},
    {e_unknown_columnname,false,sqlm_internal,-4005,"Unknown column name","unknown_columnname"},
    {e_unknown_tablename,true,sqlm_ansi,-4004,"Unknown table name",""},
    {e_unknown_tablename,true,sqlm_db2,-4004,"Unknown table name",""},
    {e_unknown_tablename,true,sqlm_internal,-4004,"Unknown table name","unknown_tablename"},
    {e_unknown_user,false,sqlm_internal,-4003,"Unknown user name","unknown_user"},
    {e_unknown_password,false,sqlm_internal,-4002,"Unknown password","unknown_password"},
    {e_unknown_resultname,false,sqlm_internal,-4000,"Unknown result table","unknown_resultname"},
    {e_invalid,false,sqlm_internal,-3999,"Invalid parameter","invalid"},
    {e_invalid_parameter,false,sqlm_internal,-3999,"Invalid parameter","invalid_parameter"},
    {e_wrong_password,false,sqlm_internal,-3998,"Wrong password","wrong_password"},
    {e_invalid_pattern,false,sqlm_internal,-3995,"Invalid pattern","invalid_pattern"},
    {e_st_invalid_destpos,false,sqlm_internal,-3993,"Invalid long column destpos","st_invalid_destpos"},
    {e_st_invalid_startpos,false,sqlm_internal,-3990,"Invalid long column startpos","st_invalid_startpos"},
    {e_st_invalid_length,false,sqlm_internal,-3989,"Invalid long column length","st_invalid_length"},
    {e_st_invalid_pos,false,sqlm_internal,-3988,"Invalid long column position","st_invalid_pos"},
    {e_st_invalid_longhandling,false,sqlm_internal,-3987,"Invalid long column handling","st_invalid_longhandling"},
    {e_invalid_sequence,false,sqlm_internal,-3103,"Invalid sequence definition","invalid_sequence"},
    {e_invalid_subtrans_structure,false,sqlm_internal,-3102,"Invalid subtrans structure","invalid_subtrans_structure"},
    {e_prev_data_recovery_was_incomplete,false,sqlm_internal,-3070,"A previous data recovery was incomplete and must be repeated","prev_data_recovery_was_incomplete"}, // PTS 1124994 UH 2004-01-26
    {e_bad_bitmap_page,false,sqlm_internal,-3069,"Bad xparam page in backup","bad_bitmap_page"},
    {e_wrong_configuration,false,sqlm_internal,-3068,"Mismatch of parameter configuration between backup and instance","wrong_configuration"},
    {e_invalid_timezone,false,sqlm_internal,-3067,"Invalid timezone","invalid_timezone"},
    {e_to_date_inconsistent,false,sqlm_internal,-3066,"Date input value inconsistent","to_date_inconsistent"},
    {e_to_date_no_letter,false,sqlm_internal,-3065,"Invalid date input value","to_date_no_letter"},
    {e_to_date_no_number,false,sqlm_internal,-3065,"Invalid date input value","to_date_no_number"},
    {e_to_date_too_long_input,false,sqlm_internal,-3065,"Invalid date input value","to_date_too_long_input"},
    {e_to_date_too_short_input,false,sqlm_internal,-3065,"Invalid date input value","to_date_too_short_input"},
    {e_unknown_meridian,false,sqlm_internal,-3065,"Invalid date input value","unknown_meridian"},
    {e_to_date_invalid_day,false,sqlm_internal,-3064,"Date input value out of range","to_date_invalid_day"},
    {e_to_date_out_of_range,false,sqlm_internal,-3064,"Date input value out of range","to_date_out_of_range"},
    {e_bad_precision_spec,false,sqlm_internal,-3063,"Invalid format code","bad_precision_spec"},
    {e_date_format_not_recognized,false,sqlm_internal,-3063,"Invalid format code","date_format_not_recognized"},
    {e_no_to_date_format,false,sqlm_internal,-3063,"Invalid format code","no_to_date_format"},
    {e_number_format_not_recogniz,false,sqlm_internal,-3063,"Invalid format code","number_format_not_recogniz"},
    {e_too_many_precision_specs,false,sqlm_internal,-3063,"Invalid format code","too_many_precision_specs"},
    {e_unknown_month,false,sqlm_internal,-3062,"Invalid month","unknown_month"},
    {e_invalid_escape_char,false,sqlm_internal,-3061,"Invalid escape value","invalid_escape_char"},
    {e_illegal_escape_sequence,false,sqlm_internal,-3060,"Invalid escape sequence","illegal_escape_sequence"},
    {e_invalid_day_of_week,false,sqlm_internal,-3059,"Invalid day of week","invalid_day_of_week"},
    {e_unknown_day,false,sqlm_internal,-3059,"Invalid day of week","unknown_day"},
    {e_invalid_blocksize,false,sqlm_internal,-3054,"Invalid block size","invalid_blocksize"},
    {e_invalid_cachelimit_size,false,sqlm_internal,-3051,"Invalid cachelimit value","invalid_cachelimit_size"},
    {e_invalid_timestamp,false,sqlm_internal,-3050,"Invalid timestamp format","invalid_timestamp"},
    {e_invalid_time,false,sqlm_internal,-3049,"Invalid time format","invalid_time"},
    {e_invalid_date,false,sqlm_internal,-3048,"Invalid date format","invalid_date"},
    {e_invalid_table_content,false,sqlm_internal,-3047,"Invalid table for RESTORE CATALOG","invalid_table_content"},
    {e_invalid_char_position,false,sqlm_internal,-3046,"Invalid char position in string function","invalid_char_position"},
    {e_file_limit,false,sqlm_internal,-3041,"Invalid position","file_limit"},
    {e_devsize_invalid,false,sqlm_internal,-3038,"Invalid volume size","devsize_invalid"},
    {e_devname_invalid,false,sqlm_internal,-3037,"Invalid volume name","devname_invalid"},
    {e_date_time_format_invalid,false,sqlm_internal,-3035,"Invalid date time format","date_time_format_invalid"},
    {e_cmd_timeout_invalid,false,sqlm_internal,-3030,"Invalid SESSION TIMEOUT","cmd_timeout_invalid"},
    {e_special_null,false,sqlm_internal,-3024,"Invalid numeric expression","special_null"},
    {e_outer_join_def_error,false,sqlm_internal,-3022,"Invalid number of tables for outer join","outer_join_def_error"},
    {e_invalid_mix_functions_columns,false,sqlm_internal,-3021,"Invalid mixture of functions and columns","invalid_mix_functions_columns"},
    {e_invalid_datalength,false,sqlm_internal,-3020,"Invalid length for this data type","invalid_datalength"},
    {e_invalid_length_of_datatyp,false,sqlm_internal,-3020,"Invalid length for this data type","invalid_length_of_datatyp"},
    {e_num_overflow,false,sqlm_internal,-3019,"Invalid exponent","num_overflow"},
    {e_invalid_number_variable,false,sqlm_internal,-3018,"Invalid numeric parameter","invalid_number_variable"},
    {e_invalid_unsign_integer,false,sqlm_internal,-3017,"Invalid unsigned integer","invalid_unsign_integer"},
    {e_num_invalid,false,sqlm_internal,-3016,"Invalid numeric constant","num_invalid"},
    {e_invalid_end_of_command,false,sqlm_internal,-3014,"Invalid end of SQL statement","invalid_end_of_command"},
    {e_invalid_columnname,false,sqlm_internal,-3011,"Invalid column name","invalid_columnname"},
    {e_hex0_column_tail,false,sqlm_internal,-3010,"Invalid character value","hex0_column_tail"},
    {e_invalid_tablename,false,sqlm_internal,-3009,"Invalid table name","invalid_tablename"},
    {e_invalid_keyword,false,sqlm_internal,-3008,"Invalid keyword or missing delimiter","invalid_keyword"},
    {e_missing_keyword,false,sqlm_internal,-3008,"Invalid keyword or missing delimiter","missing_keyword"},
    {e_unknown_functionname,true,sqlm_ansi,-3008,"Invalid keyword or missing delimiter",""},
    {e_invalid_mix_dml_ddl,false,sqlm_internal,-3007,"Invalid sequence of DML and DDL statements","invalid_mix_dml_ddl"},
    {e_invalid_command,false,sqlm_internal,-3005,"Invalid SQL statement","invalid_command"},
    {e_wrong_hostfile,false,sqlm_internal,-3004,"Invalid host file sequence","wrong_hostfile"},
    {e_invalid_username,false,sqlm_internal,-3003,"Invalid user name","invalid_username"},
    {e_invalid_datatype,false,sqlm_internal,-3002,"Invalid datatype","invalid_datatype"},
    {e_bad_hostfile_page,false,sqlm_internal,-3000,"Invalid host file content","bad_hostfile_page"},
    {e_currval_without_nextval,false,sqlm_internal,-2029,"CURRVAL is not yet defined in this session","currval_without_nextval"},
    {e_sequence_overflow,false,sqlm_internal,-2028,"Sequence $1 exhausted","sequence_overflow"},
    {e_unitrans_dest_too_short,false,sqlm_internal,-2027,"Space for converted value too small","unitrans_dest_too_short"},
    {e_invalid_label,false,sqlm_internal,-2026,"Invalid label","invalid_label"},
    {e_invalid_configuration,false,sqlm_internal,-2025,"Invalid number of backup devices","invalid_configuration"},
    {e_sysbuffer_overflow,false,sqlm_internal,-2020,"Data buffer too small","sysbuffer_overflow"},
    {e_identifier_too_long,false,sqlm_internal,-2014,"Identifier too long","identifier_too_long"},
    {e_select_col_list_too_long,false,sqlm_internal,-2013,"Select column list too long","select_col_list_too_long"},
    {e_view_col_list_too_long,false,sqlm_internal,-2012,"View column list too long","view_col_list_too_long"},
    {e_column_trunc,false,sqlm_internal,-2010,"Assignment impossible, char value too long","column_trunc"},
    {e_join_fields_too_long,false,sqlm_internal,-2009,"Join columns too long","join_fields_too_long"},
    {e_devsize_too_small,false,sqlm_internal,-2008,"Data area too small","devsize_too_small"},
    {e_num_truncated,false,sqlm_internal,-2004,"Numeric constant too long","num_truncated"},
    {e_output_columns_too_long,false,sqlm_internal,-2003,"Output columns too long","output_columns_too_long"},
    {e_one_output_field_too_long,false,sqlm_internal,-2002,"One output column too long","one_output_field_too_long"},
    {e_too_long_key,false,sqlm_internal,-2001,"Key too long","too_long_key"},
    {e_too_long_record,false,sqlm_internal,-2000,"Row too long","too_long_record"},
    {e_use_cascade,false,sqlm_internal,-1755,"SQL statement will drop further objects (use CASCADE)","use_cascade"},
    {e_no_more_perm_space,true,sqlm_oracle,-1556,"Data space full",""},
    {e_no_more_space,true,sqlm_oracle,-1556,"Data space full",""},
    {-1452,true,sqlm_oracle,-1452,"Duplicate secondary key",""},
    {e_range_violation,true,sqlm_oracle,-1402,"Integrity violation",""},
    {e_upd_view_violation,true,sqlm_oracle,-1402,"Integrity violation",""},
    {e_too_few_columns,false,sqlm_internal,-1201,"Too few columns","too_few_columns"},
    {e_too_few_values,false,sqlm_internal,-1200,"Too few values","too_few_values"},
    {e_too_many_statements_in_trigger,false,sqlm_internal,-1120,"Too many statements in definition","too_many_statements_in_trigger"},
    {e_too_many_constraints,false,sqlm_internal,-1119,"Too many constraints for this table","too_many_constraints"},
    {e_too_complex_trigger_cond,false,sqlm_internal,-1117,"Too complicated trigger qualification","too_complex_trigger_cond"},
    {e_too_long_trigger_params,false,sqlm_internal,-1116,"Too long trigger parameters","too_long_trigger_params"},
    {e_too_many_internal_columns,false,sqlm_internal,-1115,"Too many intern columns","too_many_internal_columns"},
    {e_too_small_packet_size,false,sqlm_internal,-1114,"Communication packet too small","too_small_packet_size"},
    {e_too_many_nested_functions,false,sqlm_internal,-1113,"Too many nested function calls","too_many_nested_functions"},
    {e_too_complicated_constr,false,sqlm_internal,-1112,"Constraint too complicated","too_complicated_constr"},
    {e_too_many_named_subtrans,false,sqlm_internal,-1111,"Too many savepoints","too_many_named_subtrans"},
    {e_too_long_viewtext,false,sqlm_internal,-1110,"View definition too long","too_long_viewtext"},
    {e_too_long_corelated_row,false,sqlm_internal,-1108,"Too complicated SQL statement (correlated row too long)","too_long_corelated_row"},
    {e_too_many_intern_cmd,false,sqlm_internal,-1107,"Too complicated SQL statement (too many internal commands)","too_many_intern_cmd"},
    {e_too_many_mb_strat,false,sqlm_internal,-1106,"Too complicated SQL statement (too much strategy data)","too_many_mb_strat"},
    {e_too_many_mb_data,false,sqlm_internal,-1105,"Too complicated SQL statement (too much data)","too_many_mb_data"},
    {e_too_many_mb_stackentries,false,sqlm_internal,-1104,"Too complicated SQL statement (KB-stack overflow)","too_many_mb_stackentries"},
    {e_stack_overflow,false,sqlm_internal,-1103,"Too complicated SQL statement","stack_overflow"},
    {e_too_many_subqueries,false,sqlm_internal,-1102,"Too complicated SQL statement (too many subqueries)","too_many_subqueries"},
    {e_too_many_differences,false,sqlm_internal,-1026,"Too many differences between old and new view definition","too_many_differences"},
    {e_no_server_task_available,false,sqlm_internal,-1025,"Too many parallel tasks requests","no_server_task_available"},
    {e_too_many_subtrans,false,sqlm_internal,-1023,"Too many nested subtransactions","too_many_subtrans"},
    {e_too_many_joins,false,sqlm_internal,-1017,"Too many joins","too_many_joins"},
    {e_too_many_values,false,sqlm_internal,-1015,"Too many values","too_many_values"},
    {e_too_many_sourcetables,false,sqlm_internal,-1014,"Too many source tables","too_many_sourcetables"},
    {e_too_many_order_columns,false,sqlm_internal,-1013,"Too many order columns","too_many_order_columns"},
    {e_too_many_sec_links,false,sqlm_internal,-1012,"Too many referential constraint definitions for this table","too_many_sec_links"},
    {e_too_many_corr_tables,false,sqlm_internal,-1011,"Too many correlated tables","too_many_corr_tables"},
    {e_too_many_devspaces,false,sqlm_internal,-1010,"Too many volumes","too_many_devspaces"},
    {e_too_many_indices,false,sqlm_internal,-1009,"Too many named indexes for this table","too_many_indices"},
    {e_too_many_variables,false,sqlm_internal,-1006,"Too many variables","too_many_variables"},
    {e_too_many_monadic_operators,false,sqlm_internal,-1005,"Too many monadic operators","too_many_monadic_operators"},
    {e_too_many_correlation_columns,false,sqlm_internal,-1004,"Too many correlated columns","too_many_correlation_columns"},
    {e_sysbuf_storage_exceeded,false,sqlm_internal,-1003,"Too many buffers requested","sysbuf_storage_exceeded"},
    {e_too_many_columns,false,sqlm_internal,-1002,"Too many columns","too_many_columns"},
    {e_too_many_lockunits_specified,false,sqlm_internal,-1001,"Too many lock objects specified","too_many_lockunits_specified"},
    {e_too_many_lock_requests,false,sqlm_internal,-1000,"Too many lock requests","too_many_lock_requests"},
    {e_duplicate_filename,true,sqlm_oracle,-955,"Duplicate name",""},
    {e_duplicate_indexname,true,sqlm_oracle,-955,"Duplicate name",""},
    {e_duplicate_name,true,sqlm_oracle,-955,"Duplicate name",""},
    {e_duplicate_tablename,true,sqlm_oracle,-955,"Duplicate name",""},
    {e_unknown_tablename,true,sqlm_oracle,-942,"Unknown table name",""},
    {e_net_line_down,false,sqlm_internal,-925,"Communication failure","net_line_down"},
    {e_program_stack_overflow,false,sqlm_internal,-918,"Stack overflow","program_stack_overflow"},
    {e_write_task_crash,false,sqlm_internal,-917,"Save/restore write process crash - I/O error","write_task_crash"},
    {e_no_more_memory,false,sqlm_internal,-915,"No more memory","no_more_memory"},
    {e_new_disk_not_accessible,false,sqlm_internal,-914,"New volume not accessible","new_disk_not_accessible"},
    {e_request_timeout,true,sqlm_db2,-913,"Lock request timeout",""},
    {e_wait_for_lock_release,true,sqlm_db2,-913,"Lock request timeout",""},
    {e_work_rolled_back,true,sqlm_db2,-911,"Work rolled back",""},
    {e_costlimit_overflow,false,sqlm_internal,-910,"COSTLIMIT for this user exhausted","costlimit_overflow"},
    {e_no_more_user_temp_space,false,sqlm_internal,-907,"Space for user session exhausted","no_more_user_temp_space"},
    {e_no_more_temp_space,false,sqlm_internal,-904,"Space for result tables exhausted","no_more_temp_space"},
    {e_hostfile_error,false,sqlm_internal,-903,"Host file I/O error","hostfile_error"},
    {e_disk_not_accessible,false,sqlm_internal,-902,"I/O error","disk_not_accessible"},
    {e_log_full,false,sqlm_internal,-901,"Log full","log_full"},
    {e_no_more_perm_space,true,sqlm_ansi,-900,"Data area full",""},
    {e_no_more_perm_space,true,sqlm_db2,-900,"Data area full",""},
    {e_no_more_perm_space,true,sqlm_internal,-900,"Data area full","no_more_perm_space"},
    {e_no_more_space,true,sqlm_ansi,-900,"Data area full",""},
    {e_no_more_space,true,sqlm_db2,-900,"Data area full",""},
    {e_no_more_space,true,sqlm_internal,-900,"Data area full","no_more_space"},
    {-602,true,sqlm_oracle,-602,"System error",""},
    {e_link_rule_violation,true,sqlm_db2,-530,"Referential integrity violated",""},
    {e_log_autooverwrite_on,false,sqlm_internal,-128,"Log backup is not possible because the log overwrite mode has been enabled","log_autooverwrite_on"},
    {e_log_writing_disabled,false,sqlm_internal,-127,"Log backup is not possible because log writing has been switched off","log_writing_disabled"},
    {e_backup_history_lost,false,sqlm_internal,-126,"Databackup missing (Backuphistory lost)","backup_history_lost"},
    {e_no_log_to_save,false,sqlm_internal,-123,"No more log to save","no_log_to_save"},
    {e_file_read_only,false,sqlm_internal,-115,"Table is read only","file_read_only"},
    {e_incompatible_savefile,false,sqlm_internal,-111,"Incompatible incremental backup","incompatible_savefile"},
    {e_lob_not_found,false,sqlm_internal,-109,"Lob file dropped","lob_not_found"},
    {e_file_not_found,false,sqlm_internal,-108,"Table or index dropped or content deleted","file_not_found"},
    {e_wrong_database_instance,false,sqlm_internal,-107,"SQL statement not available for this instance","wrong_database_instance"},
    {e_file_unloaded,false,sqlm_internal,-106,"Table unloaded or content deleted","file_unloaded"},
    {e_table_is_unloaded,false,sqlm_internal,-106,"Table unloaded or content deleted","table_is_unloaded"},
    {e_dbm_command_not_possible,false,sqlm_internal,-104,"DBM command impossible at this time","dbm_command_not_possible"},
    {e_wrong_application,false,sqlm_internal,-103,"SQL statement not available for this application","wrong_application"},
    {e_cancelled,false,sqlm_internal,-102,"SQL statement cancelled","cancelled"},
    {e_wrong_database_mode,false,sqlm_internal,-101,"SQL statement not available in this DB mode","wrong_database_mode"},
    {e_new_session_required,true,sqlm_ansi,-75,"Too many SQL statements (work rolled back)",""},
    {e_new_session_required,true,sqlm_db2,-75,"Too many SQL statements (work rolled back)",""},
    {e_new_session_required,true,sqlm_oracle,-75,"Too many SQL statements (work rolled back)",""},
    {e_release_caused_by_shutdown,true,sqlm_ansi,-71,"Session terminated by shutdown (work rolled back)",""},
    {e_release_caused_by_shutdown,true,sqlm_db2,-71,"Session terminated by shutdown (work rolled back)",""},
    {e_release_caused_by_shutdown,true,sqlm_oracle,-71,"Session terminated by shutdown (work rolled back)",""},
    {e_command_timeout,true,sqlm_ansi,-70,"Session inactivity timeout (work rolled back)",""},
    {e_command_timeout,true,sqlm_db2,-70,"Session inactivity timeout (work rolled back)",""},
    {e_command_timeout,true,sqlm_oracle,-70,"Session inactivity timeout (work rolled back)",""},
    {e_rollb_log_full,true,sqlm_ansi,-65,"Work rolled back",""},
    {e_rollb_log_full,true,sqlm_db2,-65,"Work rolled back",""},
    {e_rollb_log_full,true,sqlm_oracle,-65,"Work rolled back",""},
    {e_work_rolled_back,true,sqlm_ansi,-60,"Work rolled back",""},
    {e_work_rolled_back,true,sqlm_oracle,-60,"Work rolled back",""},
    {e_lock_collision,true,sqlm_oracle,-54,"Lock collision",""},
    {e_request_timeout,true,sqlm_oracle,-51,"Lock request timeout",""},
    {e_wait_for_lock_release,true,sqlm_oracle,-51,"Lock request timeout",""},
    {e_request_timeout,true,sqlm_ansi,-50,"Lock request timeout",""},
    {e_wait_for_lock_release,true,sqlm_ansi,-50,"Lock request timeout",""},
    {e_lock_collision,true,sqlm_ansi,-40,"Lock collision",""},
    {e_lock_collision,true,sqlm_db2,-40,"Lock collision",""},
    {e_foreign_key_violation,true,sqlm_ansi,-36,"Foreign key integrity violation",""},
    {e_foreign_key_violation,true,sqlm_db2,-36,"Foreign key integrity violation",""},
    {e_foreign_key_violation,true,sqlm_oracle,-36,"Foreign key integrity violation",""},
    {e_link_rule_violation,true,sqlm_ansi,-35,"Referential integrity violated",""},
    {e_link_rule_violation,true,sqlm_oracle,-35,"Referential integrity violated",""},
    {e_upd_view_violation,true,sqlm_ansi,-32,"View violation",""},
    {e_upd_view_violation,true,sqlm_db2,-32,"View violation",""},
    {e_range_violation,true,sqlm_ansi,-30,"Integrity violation",""},
    {e_range_violation,true,sqlm_db2,-30,"Integrity violation",""},
    {e_duplicate_invkey,true,sqlm_ansi,-25,"Duplicate secondary key",""},
    {e_duplicate_key,true,sqlm_ansi,-20,"Duplicate key",""},
    {e_duplicate_key,true,sqlm_db2,-20,"Duplicate key",""},
    {e_old_parsid,false,sqlm_internal,-12,"Execution failed, parse again, monitor started","old_parsid"},
    {e_string_column_empty,true,sqlm_ansi,-11,"Long column is undefined (empty)",""},
    {e_string_column_empty,true,sqlm_db2,-11,"Long column is undefined (empty)",""},
    {e_string_column_empty,true,sqlm_oracle,-11,"Long column is undefined (empty)",""},
    {-9,false,sqlm_internal,-9,"Use new parsinfo","use_new_parsinfo"},
    {e_old_fileversion,false,sqlm_internal,-8,"Execution failed, parse again","old_fileversion"},
    {e_old_leafnodes_number,false,sqlm_internal,-8,"Execution failed, parse again","old_leafnodes_number"},
    {e_invalid_messcodetype,false,sqlm_internal,-7,"Wrong messcodetype","invalid_messcodetype"},
    {e_invalid_messagetype,false,sqlm_internal,-5,"Wrong messagetype ( use ADBS )","invalid_messagetype"},
    {e_costwarning_overflow,false,sqlm_internal,-4,"COSTWARNING exhausted","costwarning_overflow"},
    {e_duplicate_invkey,true,sqlm_oracle,-1,"Duplicate key in index",""},
    {e_duplicate_key,true,sqlm_oracle,-1,"Duplicate key in index",""},
    {e_ok,false,sqlm_internal,0,"No error","ok"},
    {e_file_empty,false,sqlm_internal,100,"Row not found","file_empty"},
    {e_key_not_found,false,sqlm_internal,100,"Row not found","key_not_found"},
    {e_no_next_record,false,sqlm_internal,100,"Row not found","no_next_record"},
    {e_no_prev_record,false,sqlm_internal,100,"Row not found","no_prev_record"},
    {e_qual_violation,false,sqlm_internal,100,"Row not found","qual_violation"},
    {e_row_not_found,false,sqlm_internal,100,"Row not found","row_not_found"},
    {e_view_violation,false,sqlm_internal,100,"Row not found","view_violation"},
    {e_string_column_empty,true,sqlm_internal,110,"LONG column is undefined (empty)","string_column_empty"},
    {e_duplicate_key,true,sqlm_internal,200,"Duplicate key","duplicate_key"},
    {e_duplicate_invkey,true,sqlm_db2,250,"Duplicate secondary key",""},
    {e_duplicate_invkey,true,sqlm_internal,250,"Duplicate secondary key","duplicate_invkey"},
    {e_range_violation,true,sqlm_internal,300,"Integrity violation","range_violation"},
    {e_upd_view_violation,true,sqlm_internal,320,"View violation","upd_view_violation"},
    {e_link_rule_violation,true,sqlm_internal,350,"Referential integrity violated","link_rule_violation"},
    {e_foreign_key_violation,true,sqlm_internal,360,"Foreign key integrity violation","foreign_key_violation"},
    {e_lock_collision,true,sqlm_internal,400,"Lock collision","lock_collision"},
    {e_request_timeout,true,sqlm_internal,500,"Lock request timeout","request_timeout"},
    {e_wait_for_lock_release,true,sqlm_internal,500,"Lock request timeout","wait_for_lock_release"},
    {e_work_rolled_back,true,sqlm_internal,600,"Work rolled back","work_rolled_back"},
    {e_rollb_log_full,true,sqlm_internal,650,"Work rolled back","rollb_log_full"},
    {e_command_timeout,true,sqlm_internal,700,"Session inactivity timeout (work rolled back)","command_timeout"},
    {e_release_caused_by_shutdown,true,sqlm_internal,710,"Session terminated by shutdown (work rolled back)","release_caused_by_shutdown"},
    {e_new_session_required,true,sqlm_internal,750,"Too many SQL statements (work rolled back)","new_session_required"},
    {17401,false,sqlm_internal,17401,"JanFebMarAprMayJunJulAugSepOctNovDec",""},
    {17600,false,sqlm_internal,17600,"2MON3TUE4WED5THU6FRI7SAT1SUN",""},
    {17610,false,sqlm_internal,17610,"1 0004Zero        03One         03Two         05Three       04Four        ",""},
    {17611,false,sqlm_internal,17611,"1 0504Five        03Six         05Seven       05Eight       04Nine        ",""},
    {17612,false,sqlm_internal,17612,"1 1003Ten         06Eleven      06Twelve      08Thirteen    08Fourteen    ",""},
    {17613,false,sqlm_internal,17613,"1 1507Fifteen     07Sixteen     09Seventeen   08Eighteen    08Nineteen    ",""},
    {17620,false,sqlm_internal,17620,"2 0206Twenty      06Thirty      06Fourty      05Fifty       05Sixty       ",""},
    {17621,false,sqlm_internal,17621,"2 0707Seventy     06Eighty      06Ninety                                  ",""},
    {17630,false,sqlm_internal,17630,"1*0007Zeroeth     05First       06Second      05Third       06Fourth      ",""},
    {17631,false,sqlm_internal,17631,"1*0505Fifth       05Sixth       07Seventh     06Eighth      05Ninth       ",""},
    {17632,false,sqlm_internal,17632,"1*1005Tenth       08Eleventh    07Twelfth     10Thirteenth  10Fourteenth  ",""},
    {17633,false,sqlm_internal,17633,"1*1509Fifteenth   09Sixteenth   11Seventeenth 10Eighteenth  10Nineteenth  ",""},
    {17640,false,sqlm_internal,17640,"2*0209Twentieth   09Thirtieth   09Fourtieth   08Fiftieth    08Sixtieth    ",""},
    {17641,false,sqlm_internal,17641,"2*0710Seventieth  09Eightieth   09Ninetieth                               ",""},
    {17650,false,sqlm_internal,17650,"3 0101-           09 Hundred    10 Thousand   09 Million    10 Millions   ",""},
    {17651,false,sqlm_internal,17651,"3*0101-           11 Hundredth  12 Thousandth 11 Millionth  11 Millionth  ",""},
    {17660,false,sqlm_internal,17660,"4 0107January     08February    05March       05April       03May         ",""},
    {17661,false,sqlm_internal,17661,"4 0604June        04July        06August      09September   07October     ",""},
    {17662,false,sqlm_internal,17662,"4 1108November    08December                                              ",""},
    {17670,false,sqlm_internal,17670,"5 0106Monday      07Tuesday     09Wednesday   08Thursday    06Friday      ",""},
    {17671,false,sqlm_internal,17671,"5 0608Saturday    06Sunday                                                ",""},
    {17680,false,sqlm_internal,17680,"6 0101I           02Ii          03Iii         02Iv          01V           ",""},
    {17681,false,sqlm_internal,17681,"6 0602Vi          03Vii         04Viii        02Ix          01X           ",""},
    {17682,false,sqlm_internal,17682,"6 1102Xi          03Xii                                                   ",""},
    {17690,false,sqlm_internal,17690,"7 3204A.m.        04P.m.        02Am          02Pm                        ",""},
    {17691,false,sqlm_internal,17691,"7 3604A.d.        04B.c.        02Ad          02Bc                        ",""},
    {17700,false,sqlm_internal,17700,"4 0107January     08February    05March       05April       03May         ",""},
    {17701,false,sqlm_internal,17701,"4 0604June        04July        06August      09September   07October     ",""},
    {17702,false,sqlm_internal,17702,"4 1108November    08December                                              ",""},
    {17710,false,sqlm_internal,17710,"5 0106Monday      07Tuesday     09Wednesday   08Thursday    06Friday      ",""},
    {17711,false,sqlm_internal,17711,"5 0608Saturday    06Sunday                                                ",""},
    {e_expr_in_insert_value_list,false,sqlm_internal,21003,"internal error, contact support","expr_in_insert_value_list"}
};

//-------------------------------------------------------------------------------------------------

SQLManErr_Interface& SQLManErr_Interface::GetInstance()
{
    return SQLManErr_Instance::Instance();
}

//-------------------------------------------------------------------------------------------------

SQLManErr_Instance::SQLManErr_Instance()
:baseReturns (RTEMem_RawAllocator::Instance())
{
}

//-------------------------------------------------------------------------------------------------

void SQLManErr_Instance::Create()
{
    int numErrors = sizeof (errors) / sizeof (SQLManErrorObject);

    if (baseReturns.Reserve (numErrors))
    {
        for (int index = 0; index < numErrors; index++)
        {
            baseReturns.InsertEnd (SQLManErr_BaseToReturnCode(
                errors[index].baseError,
                errors[index].returnCode,
                errors[index].sqlmodeDependent,
                errors[index].sqlmode,
				index));
        }

        baseReturns.QuickSort();
    }

    return;
}

//-------------------------------------------------------------------------------------------------

SAPDB_Int2 SQLManErr_Instance::GetReturnCode(SAPDB_Int2 baseError,
        tsp00_SqlMode_Param sqlmode)

{
    int elem = BaseToReturnBinarySearch (baseError, sqlmode);

if (-1 != elem)
    {
        return (baseReturns[elem].GetReturncode());
    }
    else
    {
        return (-9999);
    }
}

//-------------------------------------------------------------------------------------------------

SAPDB_Int2 SQLManErr_Instance::GetBaseError(SAPDB_Int2 returnCode)
{
    int elem = ErrorObjectBinarySearch (returnCode);

    if (-1 != elem)
    {
        return errors[elem].baseError;
    }
    else
    {
        return -1;
    }
}

//-------------------------------------------------------------------------------------------------

const char* SQLManErr_Instance::GetErrorText(SAPDB_Int2 returnCode)
{
    int elem = ErrorObjectBinarySearch (returnCode);

    if (-1 != elem)
    {
        return errors[elem].errorText;
    }
    else
    {
        // -9999 unknown errorcode
        return errors[ErrorObjectBinarySearch(-9999)].errorText;
    }
}

//-------------------------------------------------------------------------------------------------

const char* SQLManErr_Instance::GetFirstText(SAPDB_Int2 *returnCode)
{
     if (0 < sizeof (errors))
     {
         *returnCode = errors[0].returnCode;
         return errors[0].errorText;
     }
     else
         return (NULL);
}

//-------------------------------------------------------------------------------------------------

const char* SQLManErr_Instance::GetNextText(SAPDB_Int2 *returnCode)
{
    int elem = ErrorObjectBinarySearch (*returnCode);

    if (-1 != elem)
    {
        int upperBound = sizeof (errors) / sizeof (SQLManErrorObject) - 1;
        while ((elem < upperBound) && (*returnCode == errors[elem+1].returnCode))
		{
			elem += 1;
		}
        if (elem < upperBound)
		{
            *returnCode = errors[elem+1].returnCode;
            return errors[elem+1].errorText;
        }
        else
            return (NULL);
    }
    else
        return (NULL);
}

//-------------------------------------------------------------------------------------------------

const char* SQLManErr_Instance::GetBaseErrorText(SAPDB_Int2 baseError)
{
    int elem = BaseToReturnBinarySearch (baseError, sqlm_internal);

    if (-1 != elem)
    {
        return (errors[baseReturns[elem].GetErrorIndex()].baseErrorText);
    }
    else
    {
        return (NULL);
    }
}

//-------------------------------------------------------------------------------------------------

int SQLManErr_Instance::BaseToReturnBinarySearch(SAPDB_Int2 baseError,
        tsp00_SqlMode_Param sqlmode)
{
    int lowerBound = 0;
    int upperBound;
    int searchPos;

    upperBound = baseReturns.GetSize() - 1;
    // calculate initial search position.
    searchPos = ( lowerBound + upperBound) / 2;

    while(
        (
        (baseReturns[searchPos].GetBaseError() != baseError) ||
        (baseReturns[searchPos].GetSqlmode() != sqlm_internal)
        )
        &&
        (lowerBound <= upperBound)
        )
    {
        if (baseReturns[searchPos].GetBaseError() > baseError)
        {
            upperBound = searchPos - 1;
        }
        else
        {
            lowerBound = searchPos + 1;
        }
        searchPos = (lowerBound + upperBound) / 2;
    }

    if (lowerBound <= upperBound)
    {
        if (
           (baseReturns[searchPos].GetSqlmodeDependant())
           &&
           (sqlm_internal != sqlmode)
           )
        {
            searchPos -= sqlm_internal - sqlmode;
        }
		return (searchPos);
	}
	else
	{
		return (-1);
	}
}

//-------------------------------------------------------------------------------------------------

int SQLManErr_Instance::ErrorObjectBinarySearch(int searchedReturnCode)
{
    int lowerBound = 0;
    int upperBound;
    int searchPos;

    upperBound = sizeof (errors) / sizeof (SQLManErrorObject) - 1;
    // calculate initial search position.
    searchPos = ( lowerBound + upperBound) / 2;

    while((errors[searchPos].returnCode != searchedReturnCode) && (lowerBound <= upperBound))
    {
        if (errors[searchPos].returnCode > searchedReturnCode)
        {
            upperBound = searchPos - 1;
        }
        else
        {
            lowerBound = searchPos + 1;
        }
        searchPos = (lowerBound + upperBound) / 2;
    }

    if (lowerBound <= upperBound)
        return (searchPos);
    else
        return (-1);

}

//-------------------------------------------------------------------------------------------------

void a101_CreateErrorHandler ()
{
    SQLManErr_Instance::Instance().Create();
}

//-------------------------------------------------------------------------------------------------
