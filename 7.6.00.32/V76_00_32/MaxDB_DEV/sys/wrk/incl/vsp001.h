
/* some common prolog for every module
 */

#ifndef VSP001_H
#define VSP001_H


/******************************************
 *            I N C L U D E S             *
 ******************************************/


#ifndef GSP00_2_H
#include "gsp00_2.h"
#endif

#ifndef GSP00_H
#include "gsp00.h"
#endif

/******************************************
 *           C O N S T A N T S            *
 ******************************************/

    /* function codes as in ORACLE Call Interface */
#define csp1_nil_fc                             0
#define csp1_create_table_fc                    1
#define csp1_set_role_fc                        2    /* unused */
#define csp1_insert_fc                          3
#define csp1_select_fc                          4
#define csp1_update_fc                          5
#define csp1_drop_role_fc                       6    /* unused */
#define csp1_drop_view_fc                       7
#define csp1_drop_table_fc                      8
#define csp1_delete_fc                          9
#define csp1_create_view_fc                    10
#define csp1_drop_user_fc                      11
#define csp1_create_role_fc                    12    /* unused */
#define csp1_create_sequence_fc                13
#define csp1_alter_sequence_fc                 14    /* unused */
#define csp1_unused1_fc                        15    /* unused */
#define csp1_drop_sequence_fc                  16
#define csp1_create_schema_fc                  17
#define csp1_create_cluster_fc                 18    /* unused */
#define csp1_create_user_fc                    19
#define csp1_create_index_fc                   20
#define csp1_drop_index_fc                     21
#define csp1_drop_cluster_fc                   22    /* unused */
#define csp1_validate_index_fc                 23    /* unused */
#define csp1_create_procedure_fc               24
#define csp1_alter_procedure_fc                25    /* unused */
#define csp1_alter_table_fc                    26
#define csp1_explain_fc                        27
#define csp1_grant_fc                          28
#define csp1_revoke_fc                         29
#define csp1_create_synonym_fc                 30
#define csp1_drop_synonym_fc                   31
#define csp1_alter_system_swith_log_fc         32    /* unused */
#define csp1_set_transaction_fc                33    /* unused */
#define csp1_pl_sql_execute_fc                 34
#define csp1_lock_table_fc                     35
#define csp1_drop_schema_fc                    36
#define csp1_rename_fc                         37
#define csp1_comment_fc                        38
#define csp1_audit_fc                          39    /* unused */
#define csp1_noaudit_fc                        40    /* unused */
#define csp1_alter_index_fc                    41    /* unused */
#define csp1_create_external_database_fc         42    /* unused */
#define csp1_drop_external_database_fc         43    /* unused */
#define csp1_create_database_fc                44    /* unused */
#define csp1_alter_database_fc                 45    /* unused */
#define csp1_create_rollback_segment_fc         46    /* unused */
#define csp1_alter_rollback_segment_fc         47    /* unused */
#define csp1_drop_rollback_segment_fc          48    /* unused */
#define csp1_create_ts_tablespace_fc           49    /* unused */
#define csp1_alter_ts_tablespace_fc            50    /* unused */
#define csp1_drop_ts_tablespace_fc             51    /* unused */
#define csp1_alter_session_fc                  52    /* unused */
#define csp1_alter_user_fc                     53
#define csp1_commit_fc                         54
#define csp1_rollback_fc                       55
#define csp1_savepoint_fc                      56
#define csp1_create_control_file_fc            57    /* unused */
#define csp1_alter_tracing_fc                  58    /* unused */
#define csp1_create_trigger_fc                 59
#define csp1_alter_trigger_fc                  60    /* unused */
#define csp1_drop_trigger_fc                   61
#define csp1_analyze_table_fc                  62
#define csp1_analyze_index_fc                  63    /* unused */
#define csp1_analyze_cluster_fc                64    /* unused */
#define csp1_create_profile_fc                 65    /* unused */
#define csp1_drop_profile_fc                   66    /* unused */
#define csp1_alter_profile_fc                  67    /* unused */
#define csp1_drop_procedure_fc                 68
#define csp1_unused3_fc                        69    /* unused */
#define csp1_alter_resource_cost_fc            70    /* unused */
#define csp1_create_sl_snapshot_log_fc         71
#define csp1_alter_sl_snapshot_log_fc          72    /* unused */
#define csp1_drop_sl_snapshot_log_fc           73
#define csp1_create_snapshot_fc                74
#define csp1_alter_snapshot_fc                 75    /* unused */
#define csp1_drop_snapshot_fc                  76    /* unused */
    /* */
#define csp1_max_oci_fc                        76    /* boundary check */
    /* */
#define csp1_select_direct_fc                 201    /* DB-special */
#define csp1_select_first_fc                  202    /* DB-special */
#define csp1_select_last_fc                   203    /* DB-special */
#define csp1_select_next_fc                   204    /* DB-special */
#define csp1_select_prev_fc                   205    /* DB-special */
#define csp1_fetch_first_fc                   206    /* DB-special */
#define csp1_fetch_last_fc                    207    /* DB-special */
#define csp1_fetch_next_fc                    208    /* DB-special */
#define csp1_fetch_prev_fc                    209    /* DB-special */
#define csp1_fetch_pos_fc                     210    /* DB-special */
#define csp1_fetch_same_fc                    211    /* DB-special */
#define csp1_string_command_fc                212    /* DB-special */
#define csp1_connect_fc                       213    /* DB-special */
#define csp1_drop_parseid_fc                  214    /* DB-special */
#define csp1_close_fc                         215    /* DB-special */
#define csp1_show_73_fc                       216    /* do not remove version < 74 need this */
#define csp1_usage_fc                         217    /* DB-special */
#define csp1_serverdb_fc                      218    /* DB-special */
#define csp1_monitor_fc                       219    /* DB-special */
#define csp1_set_fc                           220    /* DB-special */
#define csp1_link_fc                          221    /* DB-special */
#define csp1_create_domain_fc                 222    /* DB-special */
#define csp1_drop_domain_fc                   223    /* DB-special */
#define csp1_describe_fc                      224    /* DB-special */
#define csp1_alter_password_fc                225    /* DB-special */
    /*                                226 - 232 was low level */
#define csp1_putval_fc                        233    /* DB-special */
#define csp1_getval_fc                        234    /* DB-special */
#define csp1_diagnose_fc                      235    /* DB-special */
#define csp1_unlock_fc                        236    /* DB-special */
#define csp1_refresh_fc                       237    /* DB-special */
#define csp1_clear_snapshot_log_fc            238    /* DB-special */
#define csp1_next_stamp_fc                    239    /* DB-special */
#define csp1_exists_table_fc                  240    /* DB-special */
#define csp1_commit_release_fc                241    /* DB-special */
#define csp1_rollback_release_fc              242    /* DB-special */
#define csp1_drda_native_ddl_fc               243    /* DB-special */
#define csp1_select_into_fc                   244    /* DB-special */
#define csp1_fetch_relative_fc                247    /* DB-special */
#define csp1_cursor_pl_sql_execute_fc         248    /* DB-special */
#define csp1_administrator_fc                 249    /* DB-special */
#define csp1_reset_fc                         250    /* DB-special */
    /* */
#define csp1_max_special_fc                   250    /* boundary check */
    /* */
#define csp1_min_masscmd_fc                  1000    /* boundary check */
    /* */
#define csp1_minsert_fc                      1003    /* DB-masscmd */
#define csp1_mselect_fc                      1004    /* DB-masscmd */
#define csp1_mupdate_fc                      1005    /* DB-masscmd */
#define csp1_mdelete_fc                      1009    /* DB-masscmd */
#define csp1_mlock_objects_fc                1035    /* DB-masscmd */
#define csp1_mfetch_first_fc                 1206    /* DB-masscmd */
#define csp1_mfetch_last_fc                  1207    /* DB-masscmd */
#define csp1_mfetch_next_fc                  1208    /* DB-masscmd */
#define csp1_mfetch_prev_fc                  1209    /* DB-masscmd */
#define csp1_mfetch_pos_fc                   1210    /* DB-masscmd */
#define csp1_mfetch_same_fc                  1211    /* DB-masscmd */
    /* */
#define csp1_mselect_into_fc                 1244    /* DB-masscmd */
#define csp1_mfetch_relative_fc              1247    /* DB-special */
#define csp1_max_masscmd_fc                  1247    /* boundary check */
    /* */
#define csp1_masscmd_fc_offset               1000
    /* ******* next to the last byte of parsid ******* */
#define csp1_p_none                             0
#define csp1_p_command_executed                 1
#define csp1_p_use_adbs                         2
#define csp1_p_release_found                   10
#define csp1_p_not_allowed_for_program         30
#define csp1_p_close_found                     40
#define csp1_p_describe_found                  41
#define csp1_p_fetch_found                     42
#define csp1_p_mfetch_found                    43
#define csp1_p_mass_select_found               44
#define csp1_p_select_for_update_found         45
#define csp1_p_reuse_mass_select_found         46
#define csp1_p_reuse_update_sel_found          47
#define csp1_p_dialog_call                     60
#define csp1_p_mass_command                    70
#define csp1_p_mselect_found                  114
#define csp1_p_for_upd_mselect_found          115
#define csp1_p_reuse_mselect_found            116
#define csp1_p_reuse_upd_mselect_found        117
#define csp1_old_varpart_size                8240
#define csp1_part_align_length                  8
#define csp1_max_packet_cnt                     2
#define csp1_min_reply_size_per_segment        224    /* PTS 1116917 E.Z. */
    /* */
#define csp1_first_sp1_version          "62000"
#define csp1_vt_empty                           0
#define csp1_vt_null                            1
#define csp1_vt_i2                              2
#define csp1_vt_i4                              3
#define csp1_vt_r4                              4
#define csp1_vt_r8                              5
#define csp1_vt_cy                              6
#define csp1_vt_date                            7
#define csp1_vt_bstr                            8
#define csp1_vt_dispatch                        9
#define csp1_vt_error                          10
#define csp1_vt_bool                           11
#define csp1_vt_variant                        12
#define csp1_vt_unknown                        13
#define csp1_vt_decimal                        14
#define csp1_vt_i1                             16
#define csp1_vt_ui1                            17
#define csp1_vt_ui2                            18
#define csp1_vt_ui4                            19
#define csp1_vt_i8                             20
#define csp1_vt_ui8                            21
#define csp1_vt_int                            22
#define csp1_vt_uint                           23
#define csp1_vt_void                           24
#define csp1_vt_hresult                        25
#define csp1_vt_ptr                            26
#define csp1_vt_safearrary                     27
#define csp1_vt_carray                         28
#define csp1_vt_userdefined                    29
#define csp1_vt_lpstr                          30
#define csp1_vt_lpwstr                         31
#define csp1_vt_filetime                       64
#define csp1_vt_blob                           65
#define csp1_vt_stream                         66
#define csp1_vt_storage                        67
#define csp1_vt_streamed_object                68
#define csp1_vt_stored_object                  69
#define csp1_vt_blob_object                    70
#define csp1_vt_cf                             71
#define csp1_vt_clsid                          72
#define csp1_vt_lc_char                       201
#define csp1_vt_lc_wchar                      202
#define csp1_vt_lc_byte                       203
#define csp1_vt_lc_bool                       204
#define csp1_vt_lc_abap_tab_handle            205
#define csp1_vt_lc_stream_handle              206
#define csp1_vt_lc_int1                       207
#define csp1_vt_lc_kb71_param_desc            208
#define csp1_vt_lc_uint1                      209
#define csp1_vt_lc_ascii7                     210
#define csp1_vt_lc_wyde                       211
#define csp1_vt_lc_numc                       212    /* PTS 1131373, PG */
#define csp1_vt_lc_utf8                       213    /*   PTS   */
#define csp1_vt_lc_userdefined_array          214    /*         */
#define csp1_vt_lc_bitfields                  215    /* 1141492 */
#define csp1_vt_lc_oid                        216    /*         */
#define csp1_vt_lc_extoid                     217    /*         */
#define csp1_vt_lc_varoid                     218    /*   PG    */
#define csp1_vt_vector                       4096
#define csp1_vt_array                        8192
#define csp1_vt_byref                       16384
    /* features of SQL sessions */
#define sp1f_nil_feature                        0
#define sp1f_multiple_drop_parseid              1
#define sp1f_space_option                       2
#define sp1f_variable_input                     3
#define sp1f_optimized_streams                  4
#define sp1f_check_scrollableoption             5
#define sp1f_ascii_in_and_output                6
#define sp1f_last_feature                       7
    /* indicators for fields with variable length */
#define csp1_fi_max_1byte_length              245
#define csp1_fi_ignored                       250
#define csp1_fi_special_null                  251
#define csp1_fi_blob_description              252
#define csp1_fi_default_value                 253
#define csp1_fi_null_value                    254
#define csp1_fi_2byte_length                  255
    /* property names used to identify fields */
#define csp1_maxpasswordlen_tag         "maxpasswordlen"

/******************************************
 *               T Y P E S                *
 ******************************************/


typedef enum tsp1_cmd_mess_type_Enum
{
    sp1m_nil,
    sp1m_cmd_lower_bound,
    sp1m_dbs,
    sp1m_parse,
    sp1m_cfill0,
    sp1m_syntax,
    sp1m_cfill1,
    sp1m_cfill2,
    sp1m_cfill3,
    sp1m_cfill4,
    sp1m_cfill5,
    sp1m_cmd_upper_bound,
    sp1m_no_cmd_lower_bound,
    sp1m_execute,
    sp1m_ncfill0,
    sp1m_putval,
    sp1m_getval,
    sp1m_load,
    sp1m_unload,
    sp1m_ncfill1,
    sp1m_ncfill2,
    sp1m_ncfill3,
    sp1m_ncfill4,
    sp1m_ncfill5,
    sp1m_no_cmd_upper_bound,
    sp1m_hello,
    sp1m_util_lower_bound,
    sp1m_utility,
    sp1m_incopy,
    sp1m_ufill1,
    sp1m_outcopy,
    sp1m_diag_outcopy,
    sp1m_ufill3,
    sp1m_ufill4,
    sp1m_ufill5,
    sp1m_ufill6,
    sp1m_ufill7,
    sp1m_util_upper_bound,
    sp1m_specials_lower_bound,
    sp1m_switch,
    sp1m_switchlimit,
    sp1m_buflength,
    sp1m_minbuf,
    sp1m_maxbuf,
    sp1m_state_utility,
    sp1m_get_challenge,
    sp1m_sfill3,
    sp1m_sfill4,
    sp1m_sfill5,
    sp1m_specials_upper_bound,
    sp1m_wait_for_event,
    sp1m_procserv_lower_bound,
    sp1m_procserv_call,
    sp1m_procserv_reply,
    sp1m_procserv_fill1,
    sp1m_procserv_fill2,
    sp1m_procserv_fill3,
    sp1m_procserv_fill4,
    sp1m_procserv_fill5,
    sp1m_procserv_upper_bound,
    sp1m_last_cmd_mess_type
} tsp1_cmd_mess_type_Enum;
typedef tsp1_cmd_mess_type_Enum tsp1_cmd_mess_type_Param;
typedef ENUM_TYPE_MSP00 (          tsp1_cmd_mess_type, tsp1_cmd_mess_type_Enum, sp1m_last_cmd_mess_type, unsigned char);
    /* */

typedef enum tsp1_comm_error_Enum
{
    sp1ce_ok,
    sp1ce_notok,
    sp1ce_tasklimit,
    sp1ce_timeout,
    sp1ce_crash,
    sp1ce_start_required,
    sp1ce_shutdown,
    sp1ce_send_line_down,
    sp1ce_receive_line_down,
    sp1ce_packet_limit,
    sp1ce_released
} tsp1_comm_error_Enum;
typedef tsp1_comm_error_Enum tsp1_comm_error_Param;
typedef ENUM_TYPE_MSP00 (          tsp1_comm_error, tsp1_comm_error_Enum, sp1ce_released, unsigned char);
    /* */

typedef enum tsp1_utility_startup_Enum
{
    sp1us_connect,
    sp1us_restart,
    sp1us_shutdown
} tsp1_utility_startup_Enum;
typedef tsp1_utility_startup_Enum tsp1_utility_startup_Param;
typedef ENUM_TYPE_MSP00 (          tsp1_utility_startup, tsp1_utility_startup_Enum, sp1us_shutdown, unsigned char);
    /* */

typedef enum tsp1_intern_warning_Enum
{
    sp1iw_warn0_resultset_closed,
    sp1iw_warn1,
    sp1iw_warn2,
    sp1iw_warn3,
    sp1iw_warn4,
    sp1iw_warn5,
    sp1iw_warn6,
    sp1iw_warn7,
    sp1iw_warn8,
    sp1iw_warn9,
    sp1iw_warn10,
    sp1iw_warn11,
    sp1iw_warn12,
    sp1iw_warn13,
    sp1iw_warn14,
    sp1iw_warn15
} tsp1_intern_warning_Enum;
typedef tsp1_intern_warning_Enum tsp1_intern_warning_Param;
typedef ENUM_TYPE_MSP00 (          tsp1_intern_warning, tsp1_intern_warning_Enum, sp1iw_warn15, unsigned char);
    /* */
typedef SET_TYPE_MSP00 (           tsp1_warningset_intern, tsp1_intern_warning_Enum, sp1iw_warn15, 2);
    /* */

typedef enum tsp1_command_option_Enum
{
    sp1co_selfetch_off,
    sp1co_scrollable_cursor_on,
    sp1co_no_resultset_close_needed,
    sp1co_not_used2,
    sp1co_not_used3,
    sp1co_not_used4,
    sp1co_not_used5,
    sp1co_not_used6
} tsp1_command_option_Enum;
typedef tsp1_command_option_Enum tsp1_command_option_Param;
typedef ENUM_TYPE_MSP00 (          tsp1_command_option, tsp1_command_option_Enum, sp1co_not_used6, unsigned char);
    /* */
typedef SMALLSET_TYPE_MSP00 (      tsp1_command_options, tsp1_command_option_Enum, sp1co_not_used6);
    /* */

typedef enum tsp1_part_kind_Enum
{
    sp1pk_nil,
    sp1pk_appl_parameter_description,
    sp1pk_columnnames,
    sp1pk_command,
    sp1pk_conv_tables_returned,
    sp1pk_data,
    sp1pk_errortext,
    sp1pk_getinfo,
    sp1pk_modulname,
    sp1pk_page,
    sp1pk_parsid,
    sp1pk_parsid_of_select,
    sp1pk_resultcount,
    sp1pk_resulttablename,
    sp1pk_shortinfo,
    sp1pk_user_info_returned,
    sp1pk_surrogate,
    sp1pk_bdinfo,
    sp1pk_longdata,
    sp1pk_tablename,
    sp1pk_session_info_returned,
    sp1pk_output_cols_no_parameter,
    sp1pk_key,
    sp1pk_serial,
    sp1pk_relative_pos,
    sp1pk_abap_istream,
    sp1pk_abap_ostream,
    sp1pk_abap_info,
    sp1pk_checkpoint_info,
    sp1pk_procid,
    sp1pk_long_demand,
    sp1pk_message_list,
    sp1pk_vardata_shortinfo,
    sp1pk_vardata,
    sp1pk_feature,
    sp1pk_clientid,
    sp1pk_last_part_kind
} tsp1_part_kind_Enum;
typedef tsp1_part_kind_Enum tsp1_part_kind_Param;
typedef ENUM_TYPE_MSP00 (          tsp1_part_kind, tsp1_part_kind_Enum, sp1pk_last_part_kind, unsigned char);
    /* */

typedef enum tsp1_part_attributes_Enum
{
    sp1pa_last_packet,
    sp1pa_next_packet,
    sp1pa_first_packet,
    sp1pa_fill3,
    sp1pa_fill4,
    sp1pa_fill5,
    sp1pa_fill6,
    sp1pa_fill7
} tsp1_part_attributes_Enum;
typedef tsp1_part_attributes_Enum tsp1_part_attributes_Param;
typedef ENUM_TYPE_MSP00 (          tsp1_part_attributes, tsp1_part_attributes_Enum, sp1pa_fill7, unsigned char);
    /* */
typedef SMALLSET_TYPE_MSP00 (      tsp1_part_attr, tsp1_part_attributes_Enum, sp1pa_fill7);
    /* */

typedef enum tsp1_producer_Enum
{
    sp1pr_nil,
    sp1pr_user_cmd,
    sp1pr_internal_cmd,
    sp1pr_kernel,
    sp1pr_installation,
    sp1pr_view_optimizer,
    sp1pr_complex_view_handling,
    sp1pr_queryrewrite
} tsp1_producer_Enum;
typedef tsp1_producer_Enum tsp1_producer_Param;
typedef ENUM_TYPE_MSP00 (          tsp1_producer, tsp1_producer_Enum, sp1pr_queryrewrite, unsigned char);
    /*                sp1pr_internal_cmd given for example by LOAD, */
    /*                if command was not given by the user but      */
    /*                needed by LOAD for internal use               */
    /* */

typedef enum tsp1_segment_kind_Enum
{
    sp1sk_nil,
    sp1sk_cmd,
    sp1sk_return,
    sp1sk_proccall,
    sp1sk_procreply,
    sp1sk_last_segment_kind
} tsp1_segment_kind_Enum;
typedef tsp1_segment_kind_Enum tsp1_segment_kind_Param;
typedef ENUM_TYPE_MSP00 (          tsp1_segment_kind, tsp1_segment_kind_Enum, sp1sk_last_segment_kind, unsigned char);
    /* */

typedef enum tsp1_sqlmode_Enum
{
    sp1sm_nil,
    sp1sm_session_sqlmode,
    sp1sm_internal,
    sp1sm_ansi,
    sp1sm_db2,
    sp1sm_oracle
} tsp1_sqlmode_Enum;
typedef tsp1_sqlmode_Enum tsp1_sqlmode_Param;
typedef ENUM_TYPE_MSP00 (          tsp1_sqlmode, tsp1_sqlmode_Enum, sp1sm_oracle, unsigned char);

typedef struct                     tsp1_part_header
{
    tsp1_part_kind      sp1p_part_kind;
    tsp1_part_attr      sp1p_attributes;
    tsp00_Int2          sp1p_arg_count;
    tsp00_Int4          sp1p_segm_offset;
    tsp00_Int4          sp1p_buf_len;
    tsp00_Int4          sp1p_buf_size;
} tsp1_part_header;

typedef struct                     tsp1_part
{
    union
    {
    /* 1 */
        struct tsp1_part_1
        {
            tsp1_part_header    sp1p_part_header_F;
            tsp00_MoveObj       sp1p_buf_F;
        } C_1;
    /* 2 */
        struct tsp1_part_2
        {
            tsp1_part_kind      sp1p_part_kind_F;
            tsp1_part_attr      sp1p_attributes_F;
            tsp00_Int2          sp1p_arg_count_F;
            tsp00_Int4          sp1p_segm_offset_F;
            tsp00_Int4          sp1p_buf_len_F;
            tsp00_Int4          sp1p_buf_size_F;
            tsp00_ResNum        sp1p_resnum_F;
        } C_2;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tsp1_part_header   &sp1p_part_header       ()  { return this->variant.C_1.sp1p_part_header_F; };
    tsp00_MoveObj      &sp1p_buf               ()  { return this->variant.C_1.sp1p_buf_F; };
    tsp1_part_kind     &sp1p_part_kind         ()  { return this->variant.C_2.sp1p_part_kind_F; };
    tsp1_part_attr     &sp1p_attributes        ()  { return this->variant.C_2.sp1p_attributes_F; };
    tsp00_Int2         &sp1p_arg_count         ()  { return this->variant.C_2.sp1p_arg_count_F; };
    tsp00_Int4         &sp1p_segm_offset       ()  { return this->variant.C_2.sp1p_segm_offset_F; };
    tsp00_Int4         &sp1p_buf_len           ()  { return this->variant.C_2.sp1p_buf_len_F; };
    tsp00_Int4         &sp1p_buf_size          ()  { return this->variant.C_2.sp1p_buf_size_F; };
    tsp00_ResNum       &sp1p_resnum            ()  { return this->variant.C_2.sp1p_resnum_F; };
    /* const accessor functions */
    const tsp1_part_header &sp1p_part_header       () const  { return this->variant.C_1.sp1p_part_header_F; };
    const tsp00_MoveObj &sp1p_buf               () const  { return this->variant.C_1.sp1p_buf_F; };
    const tsp1_part_kind &sp1p_part_kind         () const  { return this->variant.C_2.sp1p_part_kind_F; };
    const tsp1_part_attr &sp1p_attributes        () const  { return this->variant.C_2.sp1p_attributes_F; };
    const tsp00_Int2   &sp1p_arg_count         () const  { return this->variant.C_2.sp1p_arg_count_F; };
    const tsp00_Int4   &sp1p_segm_offset       () const  { return this->variant.C_2.sp1p_segm_offset_F; };
    const tsp00_Int4   &sp1p_buf_len           () const  { return this->variant.C_2.sp1p_buf_len_F; };
    const tsp00_Int4   &sp1p_buf_size          () const  { return this->variant.C_2.sp1p_buf_size_F; };
    const tsp00_ResNum &sp1p_resnum            () const  { return this->variant.C_2.sp1p_resnum_F; };
#endif    /* defined (__cplusplus) */
} tsp1_part;
typedef tsp1_part                 *tsp1_part_ptr;

typedef struct                     tsp1_segment_header
{
    union
    {
    /* 1 */
        struct tsp1_segment_header_1
        {
            tsp00_Int4          sp1s_segm_len_F;
            tsp00_Int4          sp1s_segm_offset_F;
            tsp00_Int2          sp1s_no_of_parts_F;
            tsp00_Int2          sp1s_own_index_F;
            tsp1_segment_kind   sp1s_segm_kind_F;
            tsp1_cmd_mess_type  sp1c_mess_type_F;
            tsp1_sqlmode        sp1c_sqlmode_F;
            tsp1_producer       sp1c_producer_F;
            pasbool             sp1c_commit_immediately_F;
            pasbool             sp1c_ignore_costwarning_F;
            pasbool             sp1c_prepare_F;
            pasbool             sp1c_with_info_F;
            pasbool             sp1c_mass_cmd_F;
            pasbool             sp1c_parsing_again_F;
            tsp1_command_options sp1c_command_options_F;
            pasbool             sp1c_filler1_F;
            tsp00_C8            sp1c_filler2_F;
            tsp00_C8            sp1c_filler3_F;
        } C_1;
    /* 2 */
        struct tsp1_segment_header_2
        {
            tsp00_C8            sp1s_space2_F;
            tsp00_C4            sp1s_space3_F;
            tsp1_segment_kind   sp1s_space4_F;
            tsp00_SqlState      sp1r_sqlstate_F;
            tsp00_Int2          sp1r_returncode_F;
            tsp00_Int4          sp1r_errorpos_F;
            tsp00_WarningSet    sp1r_extern_warning_F;
            tsp1_warningset_intern sp1r_intern_warning_F;
            tsp00_Int2          sp1r_function_code_F;
            tsp00_Uint1         sp1r_tracelevel_F;
            pasbool             sp1r_filler1_F;
            tsp00_C8            sp1r_filler2_F;
        } C_2;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tsp00_Int4         &sp1s_segm_len          ()  { return this->variant.C_1.sp1s_segm_len_F; };
    tsp00_Int4         &sp1s_segm_offset       ()  { return this->variant.C_1.sp1s_segm_offset_F; };
    tsp00_Int2         &sp1s_no_of_parts       ()  { return this->variant.C_1.sp1s_no_of_parts_F; };
    tsp00_Int2         &sp1s_own_index         ()  { return this->variant.C_1.sp1s_own_index_F; };
    tsp1_segment_kind  &sp1s_segm_kind         ()  { return this->variant.C_1.sp1s_segm_kind_F; };
    tsp1_cmd_mess_type &sp1c_mess_type         ()  { return this->variant.C_1.sp1c_mess_type_F; };
    tsp1_sqlmode       &sp1c_sqlmode           ()  { return this->variant.C_1.sp1c_sqlmode_F; };
    tsp1_producer      &sp1c_producer          ()  { return this->variant.C_1.sp1c_producer_F; };
    pasbool            &sp1c_commit_immediately ()  { return this->variant.C_1.sp1c_commit_immediately_F; };
    pasbool            &sp1c_ignore_costwarning ()  { return this->variant.C_1.sp1c_ignore_costwarning_F; };
    pasbool            &sp1c_prepare           ()  { return this->variant.C_1.sp1c_prepare_F; };
    pasbool            &sp1c_with_info         ()  { return this->variant.C_1.sp1c_with_info_F; };
    pasbool            &sp1c_mass_cmd          ()  { return this->variant.C_1.sp1c_mass_cmd_F; };
    pasbool            &sp1c_parsing_again     ()  { return this->variant.C_1.sp1c_parsing_again_F; };
    tsp1_command_options &sp1c_command_options   ()  { return this->variant.C_1.sp1c_command_options_F; };
    pasbool            &sp1c_filler1           ()  { return this->variant.C_1.sp1c_filler1_F; };
    tsp00_C8           &sp1c_filler2           ()  { return this->variant.C_1.sp1c_filler2_F; };
    tsp00_C8           &sp1c_filler3           ()  { return this->variant.C_1.sp1c_filler3_F; };
    tsp00_C8           &sp1s_space2            ()  { return this->variant.C_2.sp1s_space2_F; };
    tsp00_C4           &sp1s_space3            ()  { return this->variant.C_2.sp1s_space3_F; };
    tsp1_segment_kind  &sp1s_space4            ()  { return this->variant.C_2.sp1s_space4_F; };
    tsp00_SqlState     &sp1r_sqlstate          ()  { return this->variant.C_2.sp1r_sqlstate_F; };
    tsp00_Int2         &sp1r_returncode        ()  { return this->variant.C_2.sp1r_returncode_F; };
    tsp00_Int4         &sp1r_errorpos          ()  { return this->variant.C_2.sp1r_errorpos_F; };
    tsp00_WarningSet   &sp1r_extern_warning    ()  { return this->variant.C_2.sp1r_extern_warning_F; };
    tsp1_warningset_intern &sp1r_intern_warning    ()  { return this->variant.C_2.sp1r_intern_warning_F; };
    tsp00_Int2         &sp1r_function_code     ()  { return this->variant.C_2.sp1r_function_code_F; };
    tsp00_Uint1        &sp1r_tracelevel        ()  { return this->variant.C_2.sp1r_tracelevel_F; };
    pasbool            &sp1r_filler1           ()  { return this->variant.C_2.sp1r_filler1_F; };
    tsp00_C8           &sp1r_filler2           ()  { return this->variant.C_2.sp1r_filler2_F; };
    /* const accessor functions */
    const tsp00_Int4   &sp1s_segm_len          () const  { return this->variant.C_1.sp1s_segm_len_F; };
    const tsp00_Int4   &sp1s_segm_offset       () const  { return this->variant.C_1.sp1s_segm_offset_F; };
    const tsp00_Int2   &sp1s_no_of_parts       () const  { return this->variant.C_1.sp1s_no_of_parts_F; };
    const tsp00_Int2   &sp1s_own_index         () const  { return this->variant.C_1.sp1s_own_index_F; };
    const tsp1_segment_kind &sp1s_segm_kind         () const  { return this->variant.C_1.sp1s_segm_kind_F; };
    const tsp1_cmd_mess_type &sp1c_mess_type         () const  { return this->variant.C_1.sp1c_mess_type_F; };
    const tsp1_sqlmode &sp1c_sqlmode           () const  { return this->variant.C_1.sp1c_sqlmode_F; };
    const tsp1_producer &sp1c_producer          () const  { return this->variant.C_1.sp1c_producer_F; };
    const pasbool      &sp1c_commit_immediately () const  { return this->variant.C_1.sp1c_commit_immediately_F; };
    const pasbool      &sp1c_ignore_costwarning () const  { return this->variant.C_1.sp1c_ignore_costwarning_F; };
    const pasbool      &sp1c_prepare           () const  { return this->variant.C_1.sp1c_prepare_F; };
    const pasbool      &sp1c_with_info         () const  { return this->variant.C_1.sp1c_with_info_F; };
    const pasbool      &sp1c_mass_cmd          () const  { return this->variant.C_1.sp1c_mass_cmd_F; };
    const pasbool      &sp1c_parsing_again     () const  { return this->variant.C_1.sp1c_parsing_again_F; };
    const tsp1_command_options &sp1c_command_options   () const  { return this->variant.C_1.sp1c_command_options_F; };
    const pasbool      &sp1c_filler1           () const  { return this->variant.C_1.sp1c_filler1_F; };
    const tsp00_C8     &sp1c_filler2           () const  { return this->variant.C_1.sp1c_filler2_F; };
    const tsp00_C8     &sp1c_filler3           () const  { return this->variant.C_1.sp1c_filler3_F; };
    const tsp00_C8     &sp1s_space2            () const  { return this->variant.C_2.sp1s_space2_F; };
    const tsp00_C4     &sp1s_space3            () const  { return this->variant.C_2.sp1s_space3_F; };
    const tsp1_segment_kind &sp1s_space4            () const  { return this->variant.C_2.sp1s_space4_F; };
    const tsp00_SqlState &sp1r_sqlstate          () const  { return this->variant.C_2.sp1r_sqlstate_F; };
    const tsp00_Int2   &sp1r_returncode        () const  { return this->variant.C_2.sp1r_returncode_F; };
    const tsp00_Int4   &sp1r_errorpos          () const  { return this->variant.C_2.sp1r_errorpos_F; };
    const tsp00_WarningSet &sp1r_extern_warning    () const  { return this->variant.C_2.sp1r_extern_warning_F; };
    const tsp1_warningset_intern &sp1r_intern_warning    () const  { return this->variant.C_2.sp1r_intern_warning_F; };
    const tsp00_Int2   &sp1r_function_code     () const  { return this->variant.C_2.sp1r_function_code_F; };
    const tsp00_Uint1  &sp1r_tracelevel        () const  { return this->variant.C_2.sp1r_tracelevel_F; };
    const pasbool      &sp1r_filler1           () const  { return this->variant.C_2.sp1r_filler1_F; };
    const tsp00_C8     &sp1r_filler2           () const  { return this->variant.C_2.sp1r_filler2_F; };
#endif    /* defined (__cplusplus) */
} tsp1_segment_header;

typedef struct                     tsp1_segment
{
    union
    {
    /* 1 */
        struct tsp1_segment_1
        {
            tsp1_segment_header sp1s_segm_header_F;
            tsp1_part           sp1p_part_F;
        } C_1;
    /* 2 */
        struct tsp1_segment_2
        {
            tsp1_segment_header sp1s_space1_F;
            tsp1_part_header    sp1p_part_header_F;
            tsp00_MoveObj       sp1p_buf_F;
        } C_2;
    /* 3 */
        struct tsp1_segment_3
        {
            tsp00_Int4          sp1s_segm_len_F;
            tsp00_Int4          sp1s_segm_offset_F;
            tsp00_Int2          sp1s_no_of_parts_F;
            tsp00_Int2          sp1s_own_index_F;
            tsp1_segment_kind   sp1s_segm_kind_F;
            tsp1_cmd_mess_type  sp1c_mess_type_F;
            tsp1_sqlmode        sp1c_sqlmode_F;
            tsp1_producer       sp1c_producer_F;
            pasbool             sp1c_commit_immediately_F;
            pasbool             sp1c_ignore_costwarning_F;
            pasbool             sp1c_prepare_F;
            pasbool             sp1c_with_info_F;
            pasbool             sp1c_mass_cmd_F;
            pasbool             sp1c_parsing_again_F;
            tsp1_command_options sp1c_command_options_F;
            pasbool             sp1c_filler1_F;
            tsp00_C8            sp1c_filler2_F;
            tsp00_C8            sp1c_filler3_F;
        } C_3;
    /* 4 */
        struct tsp1_segment_4
        {
            tsp00_C8            sp1s_space2_F;
            tsp00_C4            sp1s_space3_F;
            tsp1_segment_kind   sp1s_space4_F;
            tsp00_SqlState      sp1r_sqlstate_F;
            tsp00_Int2          sp1r_returncode_F;
            tsp00_Int4          sp1r_errorpos_F;
            tsp00_WarningSet    sp1r_extern_warning_F;
            tsp1_warningset_intern sp1r_intern_warning_F;
            tsp00_Int2          sp1r_function_code_F;
            tsp00_Uint1         sp1r_tracelevel_F;
            pasbool             sp1r_filler1_F;
            tsp00_C8            sp1r_filler2_F;
            tsp1_part_kind      sp1p_part_kind_F;
            tsp1_part_attr      sp1p_attributes_F;
            tsp00_Int2          sp1p_arg_count_F;
            tsp00_Int4          sp1p_segm_offset_F;
            tsp00_Int4          sp1p_buf_len_F;
            tsp00_Int4          sp1p_buf_size_F;
            tsp00_ResNum        sp1p_resnum_F;
        } C_4;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tsp1_segment_header &sp1s_segm_header       ()  { return this->variant.C_1.sp1s_segm_header_F; };
    tsp1_part          &sp1p_part              ()  { return this->variant.C_1.sp1p_part_F; };
    tsp1_segment_header &sp1s_space1            ()  { return this->variant.C_2.sp1s_space1_F; };
    tsp1_part_header   &sp1p_part_header       ()  { return this->variant.C_2.sp1p_part_header_F; };
    tsp00_MoveObj      &sp1p_buf               ()  { return this->variant.C_2.sp1p_buf_F; };
    tsp00_Int4         &sp1s_segm_len          ()  { return this->variant.C_3.sp1s_segm_len_F; };
    tsp00_Int4         &sp1s_segm_offset       ()  { return this->variant.C_3.sp1s_segm_offset_F; };
    tsp00_Int2         &sp1s_no_of_parts       ()  { return this->variant.C_3.sp1s_no_of_parts_F; };
    tsp00_Int2         &sp1s_own_index         ()  { return this->variant.C_3.sp1s_own_index_F; };
    tsp1_segment_kind  &sp1s_segm_kind         ()  { return this->variant.C_3.sp1s_segm_kind_F; };
    tsp1_cmd_mess_type &sp1c_mess_type         ()  { return this->variant.C_3.sp1c_mess_type_F; };
    tsp1_sqlmode       &sp1c_sqlmode           ()  { return this->variant.C_3.sp1c_sqlmode_F; };
    tsp1_producer      &sp1c_producer          ()  { return this->variant.C_3.sp1c_producer_F; };
    pasbool            &sp1c_commit_immediately ()  { return this->variant.C_3.sp1c_commit_immediately_F; };
    pasbool            &sp1c_ignore_costwarning ()  { return this->variant.C_3.sp1c_ignore_costwarning_F; };
    pasbool            &sp1c_prepare           ()  { return this->variant.C_3.sp1c_prepare_F; };
    pasbool            &sp1c_with_info         ()  { return this->variant.C_3.sp1c_with_info_F; };
    pasbool            &sp1c_mass_cmd          ()  { return this->variant.C_3.sp1c_mass_cmd_F; };
    pasbool            &sp1c_parsing_again     ()  { return this->variant.C_3.sp1c_parsing_again_F; };
    tsp1_command_options &sp1c_command_options   ()  { return this->variant.C_3.sp1c_command_options_F; };
    pasbool            &sp1c_filler1           ()  { return this->variant.C_3.sp1c_filler1_F; };
    tsp00_C8           &sp1c_filler2           ()  { return this->variant.C_3.sp1c_filler2_F; };
    tsp00_C8           &sp1c_filler3           ()  { return this->variant.C_3.sp1c_filler3_F; };
    tsp00_C8           &sp1s_space2            ()  { return this->variant.C_4.sp1s_space2_F; };
    tsp00_C4           &sp1s_space3            ()  { return this->variant.C_4.sp1s_space3_F; };
    tsp1_segment_kind  &sp1s_space4            ()  { return this->variant.C_4.sp1s_space4_F; };
    tsp00_SqlState     &sp1r_sqlstate          ()  { return this->variant.C_4.sp1r_sqlstate_F; };
    tsp00_Int2         &sp1r_returncode        ()  { return this->variant.C_4.sp1r_returncode_F; };
    tsp00_Int4         &sp1r_errorpos          ()  { return this->variant.C_4.sp1r_errorpos_F; };
    tsp00_WarningSet   &sp1r_extern_warning    ()  { return this->variant.C_4.sp1r_extern_warning_F; };
    tsp1_warningset_intern &sp1r_intern_warning    ()  { return this->variant.C_4.sp1r_intern_warning_F; };
    tsp00_Int2         &sp1r_function_code     ()  { return this->variant.C_4.sp1r_function_code_F; };
    tsp00_Uint1        &sp1r_tracelevel        ()  { return this->variant.C_4.sp1r_tracelevel_F; };
    pasbool            &sp1r_filler1           ()  { return this->variant.C_4.sp1r_filler1_F; };
    tsp00_C8           &sp1r_filler2           ()  { return this->variant.C_4.sp1r_filler2_F; };
    tsp1_part_kind     &sp1p_part_kind         ()  { return this->variant.C_4.sp1p_part_kind_F; };
    tsp1_part_attr     &sp1p_attributes        ()  { return this->variant.C_4.sp1p_attributes_F; };
    tsp00_Int2         &sp1p_arg_count         ()  { return this->variant.C_4.sp1p_arg_count_F; };
    tsp00_Int4         &sp1p_segm_offset       ()  { return this->variant.C_4.sp1p_segm_offset_F; };
    tsp00_Int4         &sp1p_buf_len           ()  { return this->variant.C_4.sp1p_buf_len_F; };
    tsp00_Int4         &sp1p_buf_size          ()  { return this->variant.C_4.sp1p_buf_size_F; };
    tsp00_ResNum       &sp1p_resnum            ()  { return this->variant.C_4.sp1p_resnum_F; };
    /* const accessor functions */
    const tsp1_segment_header &sp1s_segm_header       () const  { return this->variant.C_1.sp1s_segm_header_F; };
    const tsp1_part    &sp1p_part              () const  { return this->variant.C_1.sp1p_part_F; };
    const tsp1_segment_header &sp1s_space1            () const  { return this->variant.C_2.sp1s_space1_F; };
    const tsp1_part_header &sp1p_part_header       () const  { return this->variant.C_2.sp1p_part_header_F; };
    const tsp00_MoveObj &sp1p_buf               () const  { return this->variant.C_2.sp1p_buf_F; };
    const tsp00_Int4   &sp1s_segm_len          () const  { return this->variant.C_3.sp1s_segm_len_F; };
    const tsp00_Int4   &sp1s_segm_offset       () const  { return this->variant.C_3.sp1s_segm_offset_F; };
    const tsp00_Int2   &sp1s_no_of_parts       () const  { return this->variant.C_3.sp1s_no_of_parts_F; };
    const tsp00_Int2   &sp1s_own_index         () const  { return this->variant.C_3.sp1s_own_index_F; };
    const tsp1_segment_kind &sp1s_segm_kind         () const  { return this->variant.C_3.sp1s_segm_kind_F; };
    const tsp1_cmd_mess_type &sp1c_mess_type         () const  { return this->variant.C_3.sp1c_mess_type_F; };
    const tsp1_sqlmode &sp1c_sqlmode           () const  { return this->variant.C_3.sp1c_sqlmode_F; };
    const tsp1_producer &sp1c_producer          () const  { return this->variant.C_3.sp1c_producer_F; };
    const pasbool      &sp1c_commit_immediately () const  { return this->variant.C_3.sp1c_commit_immediately_F; };
    const pasbool      &sp1c_ignore_costwarning () const  { return this->variant.C_3.sp1c_ignore_costwarning_F; };
    const pasbool      &sp1c_prepare           () const  { return this->variant.C_3.sp1c_prepare_F; };
    const pasbool      &sp1c_with_info         () const  { return this->variant.C_3.sp1c_with_info_F; };
    const pasbool      &sp1c_mass_cmd          () const  { return this->variant.C_3.sp1c_mass_cmd_F; };
    const pasbool      &sp1c_parsing_again     () const  { return this->variant.C_3.sp1c_parsing_again_F; };
    const tsp1_command_options &sp1c_command_options   () const  { return this->variant.C_3.sp1c_command_options_F; };
    const pasbool      &sp1c_filler1           () const  { return this->variant.C_3.sp1c_filler1_F; };
    const tsp00_C8     &sp1c_filler2           () const  { return this->variant.C_3.sp1c_filler2_F; };
    const tsp00_C8     &sp1c_filler3           () const  { return this->variant.C_3.sp1c_filler3_F; };
    const tsp00_C8     &sp1s_space2            () const  { return this->variant.C_4.sp1s_space2_F; };
    const tsp00_C4     &sp1s_space3            () const  { return this->variant.C_4.sp1s_space3_F; };
    const tsp1_segment_kind &sp1s_space4            () const  { return this->variant.C_4.sp1s_space4_F; };
    const tsp00_SqlState &sp1r_sqlstate          () const  { return this->variant.C_4.sp1r_sqlstate_F; };
    const tsp00_Int2   &sp1r_returncode        () const  { return this->variant.C_4.sp1r_returncode_F; };
    const tsp00_Int4   &sp1r_errorpos          () const  { return this->variant.C_4.sp1r_errorpos_F; };
    const tsp00_WarningSet &sp1r_extern_warning    () const  { return this->variant.C_4.sp1r_extern_warning_F; };
    const tsp1_warningset_intern &sp1r_intern_warning    () const  { return this->variant.C_4.sp1r_intern_warning_F; };
    const tsp00_Int2   &sp1r_function_code     () const  { return this->variant.C_4.sp1r_function_code_F; };
    const tsp00_Uint1  &sp1r_tracelevel        () const  { return this->variant.C_4.sp1r_tracelevel_F; };
    const pasbool      &sp1r_filler1           () const  { return this->variant.C_4.sp1r_filler1_F; };
    const tsp00_C8     &sp1r_filler2           () const  { return this->variant.C_4.sp1r_filler2_F; };
    const tsp1_part_kind &sp1p_part_kind         () const  { return this->variant.C_4.sp1p_part_kind_F; };
    const tsp1_part_attr &sp1p_attributes        () const  { return this->variant.C_4.sp1p_attributes_F; };
    const tsp00_Int2   &sp1p_arg_count         () const  { return this->variant.C_4.sp1p_arg_count_F; };
    const tsp00_Int4   &sp1p_segm_offset       () const  { return this->variant.C_4.sp1p_segm_offset_F; };
    const tsp00_Int4   &sp1p_buf_len           () const  { return this->variant.C_4.sp1p_buf_len_F; };
    const tsp00_Int4   &sp1p_buf_size          () const  { return this->variant.C_4.sp1p_buf_size_F; };
    const tsp00_ResNum &sp1p_resnum            () const  { return this->variant.C_4.sp1p_resnum_F; };
#endif    /* defined (__cplusplus) */
} tsp1_segment;
typedef tsp1_segment              *tsp1_segment_ptr;
    /* */

typedef struct                     tsp1_packet_header
{
    tsp00_CodeType      sp1h_mess_code;
    tsp00_SwapKind      sp1h_mess_swap;
    tsp00_Int2          sp1h_filler1;
    tsp00_C5            sp1h_appl_version;
    tsp00_C3            sp1h_application;
    tsp00_Int4          sp1h_varpart_size;
    tsp00_Int4          sp1h_varpart_len;
    tsp00_Int2          sp1h_filler2;
    tsp00_Int2          sp1h_no_of_segm;
    tsp00_C8            sp1h_filler3;
} tsp1_packet_header;

typedef struct                     tsp1_packet
{
    tsp1_packet_header  sp1_header;
    union
    {
    /* 1 */
        struct tsp1_packet_1
        {
            tsp00_MoveObj       sp1_varpart_F;
        } C_1;
    /* 2 */
        struct tsp1_packet_2
        {
            tsp1_segment        sp1_segm_F;
        } C_2;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tsp00_MoveObj      &sp1_varpart            ()  { return this->variant.C_1.sp1_varpart_F; };
    tsp1_segment       &sp1_segm               ()  { return this->variant.C_2.sp1_segm_F; };
    /* const accessor functions */
    const tsp00_MoveObj &sp1_varpart            () const  { return this->variant.C_1.sp1_varpart_F; };
    const tsp1_segment &sp1_segm               () const  { return this->variant.C_2.sp1_segm_F; };
#endif    /* defined (__cplusplus) */
} tsp1_packet;
typedef tsp1_packet               *tsp1_packet_ptr;
    /* */
typedef ARRAY_TYPE_MSP00 (         tsp1_packet_list, tsp1_packet_ptr, csp1_max_packet_cnt);
    /* */

typedef enum tsp1_param_opt_type_Enum
{
    sp1ot_mandatory,
    sp1ot_optional,
    sp1ot_default,
    sp1ot_escape_char
} tsp1_param_opt_type_Enum;
typedef tsp1_param_opt_type_Enum tsp1_param_opt_type_Param;
typedef ENUM_TYPE_MSP00 (          tsp1_param_opt_type, tsp1_param_opt_type_Enum, sp1ot_escape_char, unsigned char);
    /* */
typedef SMALLSET_TYPE_MSP00 (      tsp1_param_opt_set, tsp1_param_opt_type_Enum, sp1ot_escape_char);
    /* */

typedef enum tsp1_param_io_type_Enum
{
    sp1io_input,
    sp1io_output,
    sp1io_inout
} tsp1_param_io_type_Enum;
typedef tsp1_param_io_type_Enum tsp1_param_io_type_Param;
typedef ENUM_TYPE_MSP00 (          tsp1_param_io_type, tsp1_param_io_type_Enum, sp1io_inout, unsigned char);

typedef struct                     tsp1_param_info
{
    tsp1_param_opt_set  sp1i_mode;
    tsp1_param_io_type  sp1i_io_type;
    tsp00_DataType      sp1i_data_type;
    tsp00_Uint1         sp1i_frac;
    tsp00_Int2          sp1i_length;
    tsp00_Int2          sp1i_in_out_len;
    union
    {
    /* true */
        struct tsp1_param_info_true
        {
            tsp00_Int4          sp1i_bufpos_F;
        } C_true;
    /* false */
        struct tsp1_param_info_false
        {
            tsp00_Int2          sp1i_param_no_F;
            pasbool             sp1i_read_only_F;
            pasbool             sp1i_serial_F;
        } C_false;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tsp00_Int4         &sp1i_bufpos            ()  { return this->variant.C_true.sp1i_bufpos_F; };
    tsp00_Int2         &sp1i_param_no          ()  { return this->variant.C_false.sp1i_param_no_F; };
    pasbool            &sp1i_read_only         ()  { return this->variant.C_false.sp1i_read_only_F; };
    pasbool            &sp1i_serial            ()  { return this->variant.C_false.sp1i_serial_F; };
    /* const accessor functions */
    const tsp00_Int4   &sp1i_bufpos            () const  { return this->variant.C_true.sp1i_bufpos_F; };
    const tsp00_Int2   &sp1i_param_no          () const  { return this->variant.C_false.sp1i_param_no_F; };
    const pasbool      &sp1i_read_only         () const  { return this->variant.C_false.sp1i_read_only_F; };
    const pasbool      &sp1i_serial            () const  { return this->variant.C_false.sp1i_serial_F; };
#endif    /* defined (__cplusplus) */
} tsp1_param_info;
    /* die beiden naechsten Typen ersetzen die vsp00-Typen */

typedef struct                     tsp1_dcom_param_info
{
    tsp1_param_opt_set  sp1i_dcom_mode;
    tsp1_param_io_type  sp1i_dcom_io_type;
    tsp00_Int2          sp1i_dcom_datatype;
    tsp00_Int2          sp1i_dcom_sub_datatype;
    tsp00_Int2          sp1i_dcom_length;
    tsp00_Int2          sp1i_dcom_dim;
    tsp00_Uint1         sp1i_dcom_frac;
    pasbool             sp1i_dcom_filler;
    tsp00_Int4          sp1i_dcom_cpp_offset;
    tsp00_Int4          sp1i_dcom_ascii_offset;
    tsp00_Int4          sp1i_dcom_unicode_offset;
    tsp00_C16           sp1i_dcom_type_uid;
} tsp1_dcom_param_info;

typedef struct                     tsp1_param_spec
{
    tsp00_DataType      para_type;
    char                para_frac;
    tsp00_Int2          para_length;
} tsp1_param_spec;
typedef ARRAY_TYPE_MSP00 (         tsp1_param_spec_list, tsp1_param_spec, csp_maxvariables);
    /* */
typedef tsp00_Uint1                tsp1_feature;

typedef struct                     tsp1_one_feature
{
    tsp1_feature        of_feature;
    tsp00_Uint1         of_value;
} tsp1_one_feature;
typedef ARRAY_TYPE_MSP00 (         tsp1_features, tsp1_one_feature, sp1f_last_feature - sp1f_nil_feature + 1);

struct tsp001_TemplateDummy {
    tsp1_part_attributes dummy_tsp1_part_attributes;
    tsp1_sqlmode         dummy_tsp1_sqlmode;
    tsp1_producer        dummy_tsp1_producer;
    tsp1_cmd_mess_type   dummy_tsp1_cmd_mess_type;
    tsp1_param_io_type   dummy_tsp1_param_io_type;
    tsp1_comm_error      dummy_tsp1_comm_error;
    tsp1_param_opt_type  dummy_tsp1_param_opt_type;
    tsp1_intern_warning  dummy_tsp1_intern_warning;
    tsp1_segment_kind    dummy_tsp1_segment_kind;
    tsp1_command_option  dummy_tsp1_command_option;
    tsp1_utility_startup dummy_tsp1_utility_startup;
    tsp1_part_kind       dummy_tsp1_part_kind;
    tsp1_param_opt_set   dummy_tsp1_param_opt_set;
    tsp1_warningset_intern dummy_tsp1_warningset_intern;
    tsp1_part_attr       dummy_tsp1_part_attr;
    tsp1_command_options dummy_tsp1_command_options;
};


#endif
