
/* some common prolog for every module
 */

#ifndef GGG00_H
#define GGG00_H


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

     #if defined __cplusplus
     #include "ggg91.h"
     #endif
#define MAX_AUX_BUFFER_COUNT_GG00              16
#define MAX_CODE_TABLES_GG00                    8
#define MAX_COL_PER_INDEX_GG00                 16
#define MAX_COL_PER_TAB_GG00                 1024
#define MAX_COL_SEQUENCE_GG00                  16    /* cnt cols with link, inv, part */
#define MAX_HASH_PRIME_GG00                  1024    /* possible maximum 1416         */
#define MAX_INV_PER_TAB_GG00                  255    /* PTS 1124452 E.Z. */
#define MAX_QUALBUF_LOCKS_GG00                240

#if defined (vers80)


#if defined (rec32KB)

    /* PTS 1116801 E.Z. */
#define MAX_RECLEN_GG00                (BUF_MXSP00 - ( 4 * 96 ))    /* 96 : see below */

#else

#define MAX_RECLEN_GG00                (8192 - 96)    /* 96 : see below */

#endif


#else

#define MAX_RECLEN_GG00                (BUF_MXSP00 - 96)    /* 96 : see below */

#endif

    /*                       96 :    NodeHeader                         */
    /*                               + PageCheckTrailer + BdPointerSize */
    /* END PTS 1116801 E.Z. */
#define MAX_LONGRESULTBUF_GG00         (MAX_RECLEN_GG00 + MAX_COL_PER_TAB_GG00)
#define MAX_RECPARTS_GG00                     255
#define MAX_ROOTLIST_GG00                       6
#define MAX_STACK_GG00                         40
#define MAX_TAPES_GG00                         32
    /**/
#define DOUBLE_BUF_MXGG00              (2 * BUF_MXSP00)
#define FILE_ID_MXGG00                         40
#define FILE_POS_MXGG00                         8    /* FilePos used bytes */
#define FN_MXGG00                              16

#if defined (__cplusplus)

#define NIL_PAGE_NO_GG00               NIL_PAGE_NO_GG91

#else

#define NIL_PAGE_NO_GG00               MAX_INT4_SP00

#endif

#define cgg_rec_key_offset                      8
#define PAGE_HEADER_MXGG00                      8
#define PAGE_TRAILER_MXGG00            PAGE_HEADER_MXGG00
#define PAGE_CHECK_TRAILER_MXGG00      (INT4_MXSP00 + PAGE_TRAILER_MXGG00)

#if defined (vers80)

    /* PTS 1116801 E.Z. */
#define RECBODY_MXGG00                 (MAX_RECLEN_GG00 - cgg_rec_key_offset)

#if defined (rec32KB)

#define RECPART_MXGG00                 (BUF_MXSP00 / MAX_RECPARTS_GG00)

#else

#define RECPART_MXGG00                 (8192 / MAX_RECPARTS_GG00)

#endif


#else

#define RECBODY_MXGG00                 (BUF_MXSP00 - cgg_rec_key_offset)
#define RECPART_MXGG00                 (BUF_MXSP00 / MAX_RECPARTS_GG00)

#endif

#define SURROGATE_MXGG00                        8
#define RSN_RECHEAD_MXGG00             (cgg_rec_key_offset + 1 + SURROGATE_MXGG00 + 1)
#define MAX_COLUMNOFFSETS              (MAX_COL_PER_TAB_GG00 + 1)
#define RECCOLUMNS_MXGG00              (RECBODY_MXGG00 + cgg_rec_key_offset - RSN_RECHEAD_MXGG00)
    /* END PTS 1116801 E.Z. */
#define SFP_INIT_PART_MXGG00                 2248
#define STACK_ENTRY_MXGG00                      8
#define STRING_FD_MXGG00                       44    /* string file desc     */
#define SYSKEY_MXGG00                         140
#define TEMP_PREFIX_MXGG00                      8
    /**/
#define FILE_ID_HEAD_MXGG00            (FILE_ID_MXGG00 - FN_MXGG00)
#define LOGPAGE_BODY_MXGG00            (PAGE_MXSP00 - PAGE_HEADER_MXGG00 - INT4_MXSP00 - PAGE_CHECK_TRAILER_MXGG00)
#define MB_PART1_HEAD_MXGG00           (FILE_ID_MXGG00 + 2 * 32 + 8)
#define MB_PART1_RETURN_MXGG00         (FILE_ID_MXGG00 + 2 * RESNUM_MXSP00 + 2 + 4 + 2 * NUMBER_MXSP00)

#if defined (BIT64)

#define ALIGNMENT_GG00                          8

#else

#define ALIGNMENT_GG00                          4

#endif

    /**/
#define MAX_SEQINFOS_GG00                      32    /* PTS 1110096 E.Z. */
    /*****  begin offset constants  *****/
    /**/
#define cgg_col_fd_size                         9
#define cgg_eo_bytestr                          0    /* bytestringhandling */
#define cgg_max_timeout                     86400    /* seconds = 1 day */
#define cgg_nil_child                           0
#define cgg_nil_leafnodes              (- 1)    /* -> treeleafnodes */
#define cgg_nil_pid                             0
#define cgg_nil_transindex                      0
#define cgg_nil_varcol_cnt_off         (- 1)
#define cgg_treeid_fn_offset           (FILE_ID_MXGG00 - FN_MXGG00)
#define cgg_whole_bytestr              (- 1)    /* bytestringhandling */
#define NIL_STR_VERSION_GG00           NIL_PAGE_NO_GG00
    /**/
#define ROOT_CHECK_GG00                (- 1554236731)    /* 0xC53A5CA3 */
    /**/
#define cgg_literal_valuefieldlist     (- 4)
#define cgg_param_valuefieldlist       (- 3)
#define cgg_pars_result_valuefieldlist (- 2)
#define cgg_ex_result_valuefieldlist   (- 1)
#define cgg_valuelistoffset                     4
#define cgg_idx_literal_valuefieldlist          0
#define cgg_idx_param_valuefieldlist            1
#define cgg_idx_pars_result_valuefieldlist          2
#define cgg_idx_ex_result_valuefieldlist          3
#define cgg_idx_max_valuefieldlist              3
    /**/
#define cgg_dummy_file_version          "\xFF\xFF"
#define cgg_special_file_version        "\xFF\xFE"
    /**/
    /* PTS 1111576 E.Z. */
    /**/
#define cgg_max_session                 "\xFF\x00\x00\x00"
#define cgg_nil_session                 "\xFF\xFF\xFF\xFF"
#define cgg_nil_trans                   "\xFF\xFF\xFF\xFF\xFF\xFF"
    /**/
#define cgg_zero_c2                     "\x00\x00"
#define cgg_zero_c4                     "\x00\x00\x00\x00"
#define cgg_zero_id                     "\x00\x00\x00\x00\x00\x00\x00\x00"
#define cgg_zero_session                "\x00\x00\x00\x00"
#define cgg_zero_subtrans                       0
#define cgg_zero_trans                  "\x00\x00\x00\x00\x00\x00"
#define cgg_zero_userid                 "\x00\x00\x00\x00"
    /**/
#define cgg_zero_fn                     "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
#define cgg_errortext_fn                "\x1A\x00\xFF\xFF\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    /**/
#define cgg_sys1_catalog_id             "\x00\x00\x00\x00\x00\x00\x00\x01"
#define cgg_sys2_catalog_id             "\x00\x00\x00\x00\x00\x00\x00\x02"
#define cgg_sys3_commandcache_id        "\x00\x00\x00\x00\x00\x00\x00\x03"    /* PTS 1120445 D.T. */
#define CONSIST_TRANS_PREFIX_GG00       "\xFF\xFC\x00\x00\x00\x00\x00\x00"
#define OPEN_TRANS_PREFIX_GG00          "\xFF\xFB\x00\x00\x00\x00\x00\x00"
#define NIL_SYSKEY_GG00                 "\xFF\xFE\x00\x00\x00\x00\x00\x00"
#define NIL_UNIQUE_PRIM_ID_GG00         "\x00\x00\x00\x00"
#define MAX_UNIQUE_PRIM_ID_GG00         "\xFF\xFF\xFF\xFF"
    /**/
#define cgg_obj_key_file_id             "\xFF\xFF\xFF\xFF"
    /**/
    /*****  tgg00_BasisError constants  *****/
#define e_ok                                    0
#define e_cachedirectory_full                 100
#define e_append_not_allowed                  210    /*for internal use only*/
#define e_bad_datapage                        300
#define e_bad_file                            320
#define e_bad_invfile                         330
#define e_buffer_limit                        350
#define e_cancelled                           400
#define e_do_not_access_fdir                  504
#define e_do_not_drop_fdir                    505
#define e_duplicate_filename                  510
#define e_duplicate_key                       530
#define e_duplicate_sysinfo                   540
#define e_duplicate_record                    550    /*internal for joinview-check*/
#define e_file_empty                          700
#define e_file_limit                          710
#define e_file_must_be_read_only              712    /*for internal use only*/
#define e_file_not_accessible                 715
#define e_file_not_found                      720
#define e_lob_not_found                       725
#define e_file_read_only                      730
#define e_file_unloaded                       740
#define e_illegal_filename                   1000
#define e_illegal_key                        1020
#define e_illegal_record                     1030
#define e_incompatible_savefile              1040
#define e_init_missing                       1050
#define e_inv_list_not_found                 1060
#define e_inv_not_found                      1070
#define e_invalid_invlistpos                 1080
#define e_invalid_mix_dml_ddl                1090
#define e_invalid_root                       1100
#define e_key_not_found                      1200
#define e_primkey_not_found                  1210
#define e_no_more_temp_space                 1400
#define e_no_more_perm_space                 1402
#define e_no_more_memory                     1405
#define e_no_next_invkey                     1410
#define e_no_next_record                     1420
#define e_no_prev_invkey                     1430
#define e_no_prev_record                     1440
#define e_old_fileversion                    1500
#define e_old_leafnodes_number               1510
#define e_old_parsid                         1520
#define e_shutdown                           1900
#define e_sysbuf_storage_exceeded            1910
#define e_sequence_overflow                  1915
#define e_currval_without_nextval            1916    /* PTS 1110096 E.Z. */
#define e_sysbuffer_overflow                 1920
#define e_sysinfo_not_found                  1930
#define e_unknown_strategy                   2050
#define e_enough_resultsets                  2400
#define e_invalid_date                       2500
#define e_end_of_month_adjustment            2510    /*internal==> warn6 */
#define e_time_value_too_long                2520    /*internal==> warn11*/
#define e_invalid_time                       2530
#define e_invalid_timestamp                  2540
#define e_invalid_day_of_week                2545
#define e_invalid_pattern                    2550
#define e_request_timeout                    2600
#define e_too_many_resultsets                2700
#define e_too_many_serials                   2705
#define e_too_many_updatesets                2710
#define e_too_many_differences               2720
#define e_internal_recreate_failed           2730
#define e_too_many_mb_stackentries           2800
#define e_too_many_mb_data                   2801
#define e_too_small_mb_qual_part             2802
#define e_too_small_packet_size              2805
#define e_too_complicated_constr             2806
#define e_too_long_viewtext                  2807
#define e_too_many_named_subtrans            2808
#define e_too_many_intern_cmd                2809
#define e_too_many_intern_parsinfos          2810
#define e_too_many_nested_functions          2811
#define e_too_long_corelated_row             2812
#define e_too_many_internal_columns          2813
#define e_too_long_trigger_params            2814
#define e_too_complex_trigger_cond           2815
#define e_too_many_statements_in_trigger       2817
#define e_too_many_mb_strat                  2818
#define e_bad_hostfile_page                  3100
#define e_cmd_timeout_invalid                3150
#define e_column_trunc                       3200
#define e_date_time_format_invalid           3301
#define e_devname_invalid                    3304
#define e_devsize_invalid                    3308
#define e_devsize_too_small                  3310
#define e_duplicate_invkey                   3320
#define e_not_implemented                    3400
#define e_hex0_column_tail                   3600
#define e_hostfile_error                     3700
#define e_incomplete_logsegm                 3705
#define e_invalid_blocksize                  3707
#define e_incompatible_log                   3800
#define e_inconsistent_nodetype              3816
#define e_invalid                            3820
#define e_key_update                         4000
#define e_logpage_not_found                  4050
#define e_conf_parameter_error               4107
#define e_hotstandby_cmd_send_failed         4150    /* added UH 2003-09-05 */
#define e_hotstandby_wrong_role              4151    /* added UH 2003-09-05 */
#define e_hotstandby_establish_mirror_failed       4152    /* added UH 2003-09-05 */
#define e_hotstandby_add_node_failed         4153    /* added UH 2003-09-05 */
#define e_hotstandby_del_node_failed         4154    /* added UH 2003-09-05 */
#define e_hotstandby_free_memory_failed       4155    /* added UH 2003-09-05 */
#define e_net_line_down                      4210
#define e_new_hostfile_required              4300
#define e_end_of_backup                      4301    /*for internal use only*/
#define e_no_data_on_tape                    4304
#define e_no_servers                         4305
#define e_save_skipped                       4306
#define e_null_value_illegal                 4310
#define e_num_invalid                        4320
#define e_num_overflow                       4330
#define e_program_stack_overflow             4500
#define e_qual_violation                     4600
#define e_range_violation                    4700
#define e_rec_not_locked                     4710
#define e_release_caused_by_shutdown         4721
#define e_response_timeout                   4728
#define e_rollback_wanted                    4740    /*for internal use only*/
#define e_skip_upd                           4800
#define e_skip_key_upd                       4801
#define e_special_null                       4810
#define e_stack_op_illegal                   4820
#define e_stack_overflow                     4830
#define e_stack_type_illegal                 4840
#define e_string_column_empty                4850
#define e_work_rolled_back                   4900
#define e_rollb_log_full                     4906
#define e_too_many_devspaces                 4920
#define e_too_many_lockunits_specified       4930
#define e_too_many_lock_requests             4940
#define e_no_server_task_available           4946
#define e_lock_collision                     5000
#define e_lock_collision_with_inserted_row       5001
#define e_lock_dirty                         5005
#define e_upd_view_violation                 5010
#define e_view_violation                     5100
    /*e_wait_for_savepoint           =  5200;*/
#define e_wait_for_lock_release              5210
#define e_wrong_hostfile                     5220
#define e_wrong_configuration                5224    /* PTS 1121221 UH 2003-03-20 */
#define e_no_log_to_save                     5225
#define e_backup_is_running                  5226    /* PTS 1108479 UH 2000-11-27 */
#define e_backup_not_running                 5227    /* PTS 1108479 UH 2000-11-27 */
#define e_no_standby_configuration           5228
#define e_backup_history_lost                5229
#define e_log_autooverwrite_on               5230    /* PTS 1128223 MB 2004-03-04 */
#define e_log_writing_disabled               5231    /* PTS 1128223 MB 2004-03-04 */
#define e_wrong_password                     5240
#define e_dcom_hresult                       5300
#define e_invalid_label                      5550
#define e_invalid_configuration              5600
#define e_write_task_crash                   5750
#define e_queue_mismatch                     5900
#define e_bad_pagetype                       5950
#define e_invalid_columnname                 6000
#define e_invalid_end_of_command             6005
#define e_invalid_datatype                   6010
#define e_invalid_filetype                   6012
#define e_invalid_tabletype                  6014
#define e_identifier_too_long                6015
#define e_invalid_indexorder                 6016    /*only internal error*/
#define e_invalid_keyword                    6020
#define e_expr_in_insert_value_list          6025    /*only internal error*/
#define e_invalid_mix_functions_columns       6035
#define e_invalid_tablename                  6040
#define e_invalid_username                   6045
#define e_invalid_messagetype                6050
#define e_in_view_not_allowed                6055
#define e_replicated_table_not_allowed       6057
#define e_parameter_not_allowed              6060
#define e_missing_constant                   6065
#define e_missing_identifier                 6070
#define e_missing_integer                    6075
#define e_missing_keyword                    6080
#define e_wanted_keyword                     6083
#define e_missing_number                     6085
#define e_missing_password                   6087
#define e_missing_delimiter                  6088
#define e_missing_string_literal             6090
#define e_missing_value_spec                 6095
#define e_reserved_identifier                6105
#define e_subquery_not_allowed               6110
#define e_query_not_allowed                  6113
#define e_too_many_monadic_operators         6115
#define e_rowno_not_allowed                  6120
#define e_invalid_unsign_integer             6125
#define e_update_not_allowed                 6130
#define e_synonym_not_allowed                6150
#define e_alter_column_not_allowed           6200
#define e_alter_drop_user_not_allowed        6202
#define e_check_option_not_allowed           6203
#define e_column_indexed                     6204
#define e_column_must_be_keycolumn           6206
#define e_col_must_be_no_keycolumn           6208
#define e_column_must_have_default           6210
#define e_column_not_indexed                 6212
#define e_command_for_string_not_allow       6218
#define e_command_timeout                    6222
#define e_connection_disabled                6223
#define e_const_incompatible_with_typ        6224
#define e_corelated_subquery_not_allowe       6226
#define e_costlimit_overflow                 6228
#define e_costwarning_overflow               6230
#define e_current_of_needs_fetch             6232
#define e_default_not_allowed                6234
#define e_default_not_in_range               6236
#define e_default_spec_not_allowed           6238
#define e_duplicate_replication_user         6240
    /* PTS 1111576 E.Z. */
#define e_duplicate_columnname               6242
#define e_duplicate_constraint               6244
#define e_duplicate_default                  6246
#define e_duplicate_indexname                6248
#define e_duplicate_name                     6250
#define e_duplicate_privilege                6254
#define e_duplicate_referencename            6256
#define e_duplicate_resultname               6258
#define e_duplicate_tablename                6262
#define e_duplicate_trigger_def              6264
#define e_duplicate_value                    6266
#define e_scrolling_fetch_not_allowed        6267
#define e_foreign_key_must_exist             6268
#define e_foreign_key_violation              6270
#define e_from_select_not_allowed            6274
#define e_function_not_allowed               6276
#define e_group_user_not_allowed             6278
#define e_incompatible_datatypes             6282
#define e_indexed_table_not_allowed          6284
#define e_indexname_must_be_unique           6286
#define e_invalid_cachelimit_size            6288
#define e_invalid_char_position              6290
#define e_invalid_command                    6292
#define e_invalid_costwarning                6294
#define e_invalid_datalength                 6296
#define e_invalid_length_of_datatyp          6298
#define e_invalid_messcodetype               6300
#define e_invalid_number_variable            6302
#define e_invalid_parameter                  6304
#define e_invalid_range                      6308
#define e_invalid_sequence                   6310
#define e_invalid_subtrans_structure         6312
#define e_invalid_table_content              6314
#define e_invalid_view_column                6318
#define e_join_fields_too_long               6320
#define e_key_does_not_exist                 6322
    /* PTS 1111576 E.Z. */
#define e_key_not_allowed                    6324
#define e_link_column_not_allowed            6326
#define e_link_cycle_not_allowed             6328
#define e_link_rule_not_allowed              6330
#define e_link_rule_violation                6332
#define e_lock_row_not_allowed               6334
#define e_role_cycle                         6335
#define e_mandatory_field_is_null            6336
#define e_missing_basetable                  6340
#define e_missing_column_definite            6341
#define e_missing_tablename_definite         6342
#define e_missing_keyfield                   6346
#define e_missing_mandatory_recordfield       6348
#define e_missing_privilege                  6350
#define e_missing_reuseable                  6352
#define e_missing_update_column              6354
    /* PTS 1111576 E.Z. */
#define e_missing_viewtable                  6356
#define e_new_session_required               6358
#define e_no_more_user_temp_space            6360
#define e_no_open_subtrans                   6362
#define e_no_resulttable_allowed             6364
#define e_not_allowed_in_temp_table          6366
#define e_not_null_not_allowed               6368
#define e_null_not_allowed                   6370
#define e_num_truncated                      6372
#define e_one_output_field_too_long          6374
#define e_only_one_recordtype_allowed        6376
#define e_order_col_must_be_number           6377
#define e_outer_join_def_error               6378
#define e_outer_join_not_allowed             6379
#define e_output_columns_too_long            6380
#define e_output_field_must_be_group         6382
#define e_owner_must_be_specified            6384
#define e_parameter_change_not_allowed       6386
#define e_procedure_must_be_recompiled       6392
#define e_referenced_index_not_allowed       6400
#define e_remote_load_not_allowed            6402
#define e_restore_catalog_error              6404
#define e_bd_page_collision                  6405    /*for internal use only*/
#define e_row_not_found                      6408
#define e_schema_not_allowed                 6409
#define e_select_col_list_too_long           6410
    /* PTS 1120287 E.Z. */
    /* PTS 1111576 E.Z. */
#define e_st_col_not_open                    6420
#define e_st_col_open_read_only              6422
#define e_st_invalid_destpos                 6424
#define e_st_invalid_length                  6426
#define e_st_invalid_pos                     6428
#define e_st_invalid_startpos                6430
#define e_st_invalid_longhandling            6431
#define e_ak_system_error                    6432
#define e_system_error                       6433
#define e_internal_error                     6434
#define e_table_is_unloaded                  6436
#define e_table_must_be_temp                 6438
#define e_table_not_in_from_list             6440
#define e_temp_table_not_allowed             6442
#define e_too_few_columns                    6444
#define e_too_few_values                     6446
#define e_too_long_key                       6448
#define e_too_long_record                    6450
#define e_too_many_cols_dropped              6452
#define e_too_many_columns                   6454
#define e_too_many_constraints               6455
#define e_too_many_corr_tables               6456
#define e_too_many_correlation_columns       6458
#define e_too_many_indices                   6460
#define e_too_many_joins                     6462
#define e_too_many_order_columns             6466
#define e_too_many_sec_links                 6470
#define e_too_many_sourcetables              6474
#define e_too_many_subqueries                6476
#define e_too_many_subtrans                  6478
#define e_too_many_values                    6484
#define e_too_many_variables                 6486
#define e_too_old_tool                       6487
#define e_too_short_datapart                 6488
#define e_invalid_segment_construction       6489
#define e_union_cols_mustbe_compatible       6490
#define e_unknown_name                       6491
#define e_unknown_columnname                 6492
#define e_unknown_constraint                 6494
#define e_unknown_default                    6496
#define e_unknown_domainname                 6498
#define e_unknown_functionname               6499
#define e_unknown_groupname                  6500
#define e_unknown_indexname                  6502
#define e_unknown_linkname                   6504
#define e_unknown_mapset                     6506
#define e_unknown_ordercolumn                6508
#define e_unknown_parseid                    6510
#define e_unknown_password                   6512
#define e_unknown_procedure_name             6514
#define e_unknown_resultname                 6516
#define e_unknown_sequencename               6518
#define e_unknown_name_of_serverdb           6520
#define e_unknown_session                    6521
#define e_unknown_synonymname                6522
#define e_unknown_sql_schema                 6523
#define e_unknown_tablename                  6524
#define e_unknown_multibyte_set              6527
#define e_unknown_trigger                    6528
#define e_unknown_user                       6530
#define e_unknown_user_password_pair         6532
#define e_use_cascade                        6533
#define e_use_fetch_rest                     6536
#define e_user_already_connected             6538
#define e_user_connected_to_same_task        6540
#define e_user_not_allowed                   6542
#define e_user_not_connected                 6544
#define e_value_expr_must_be_param_name       6546
#define e_varchar_not_allowed                6548
#define e_variable_not_allowed               6550
#define e_view_col_list_too_long             6552
#define e_view_def_contradicts_table_de       6554
#define e_without_datatypes                  6556
#define e_wrong_database_mode                6558
#define e_wrong_database_instance            6559
#define e_wrong_application                  6560
#define e_dbm_command_not_possible           6561    /* UH 2003-02-12 */
#define e_illegal_escape_sequence            6562
#define e_invalid_escape_char                6564
#define e_unitrans_src_too_short             6568
#define e_unitrans_dest_too_short            6570
#define e_not_translatable                   6572
#define e_not_trans_to_uni                   6574
#define e_not_trans_from_uni                 6576
#define e_autosave_running                   6578
#define e_no_matching_authentication         6580
#define e_no_basic_authentication            6581
    /* Errors for oracle like timestamp functions: 67xx */
#define e_to_date_duplicate_format           6710
#define e_no_to_date_format                  6720
#define e_date_format_not_recognized         6721
#define e_to_date_too_long_input             6730
#define e_to_date_inconsistent               6731
#define e_to_date_invalid_day                6739
#define e_to_date_too_short_input            6740
#define e_to_date_out_of_range               6741
#define e_unknown_month                      6743
#define e_unknown_day                        6746
#define e_unknown_meridian                   6755
#define e_invalid_timezone                   6757
#define e_to_date_no_number                  6758
#define e_to_date_no_letter                  6759
#define e_number_format_not_recogniz         6781
#define e_too_many_precision_specs           6798
#define e_bad_precision_spec                 6799
    /* The error numbers above ressemble the corresponding */
    /* oracle returncodes, which are in the form -18xx.    */
#define e_new_disk_not_accessible            8000
#define e_standby_info_not_ok                8004    /* PTS 1111229 E.Z. */
#define e_root_check                         8010
#define e_migration_error                    8048    /* PTS 1117498 M.Ki. */
#define e_not_prepared_for_migration         8049    /* PTS 1117050 TS 2002-08-09 */
#define e_no_converter_entry                 8050
#define e_no_snapshot_available              8051    /* PTS 1121318 TS 2003-07-09 */
#define e_prev_data_recovery_was_incomplete       8052    /* PTS 1124994 UH 2004-01-26 */
#define e_bad_fdir                           9001
#define e_bad_logpage                        9010
#define e_bad_conv_page                      9030
#define e_bad_conv_bitmap_page               9031
#define e_bad_bitmap_page                    9045
#define e_disk_not_accessible                9050
#define e_illegal_branchlength               9060
#define e_illegal_entrylength                9070
#define e_illegal_entrypos                   9080
#define e_illegal_keylength                  9090
#define e_illegal_page_no                    9100
#define e_move_error                   (- 9111)
#define e_invalid_entrypos                   9120
#define e_invalid_index_structure            9130
#define e_invalid_leaves_structure           9140
#define e_no_more_space                      9150    /*for internal use only*/
#define e_page_in_wrong_tree                 9160
#define e_data_page_corrupted                9161
#define e_corrupted_datapage_from_io         9163    /*for internal use only*/
#define e_invalid_fbm_mark                   9180
#define e_log_error                          9200
#define e_undo_log_write_error               9201
#define e_undo_log_execute_error             9202
#define e_redo_log_execute_error             9203
#define e_history_write_error                9204
#define e_history_read_error                 9205
#define e_history_remove_error               9206
#define e_redo_log_read_error                9207    /* UH 2004-09-27 */
#define e_redo_log_write_error               9208    /* UH 2004-09-27 */
#define e_log_full                           9210
#define e_start_server_task_error            9221
#define e_no_more_jobs_to_wait_for           9222
#define e_software_too_old                   9230    /* UH 2004-09-27 */
#define e_nil_transindex                     9232
#define e_long_column_missing                9240    /* PTS 1103382 TS 1999-07-16 */
#define e_bd_leaf_unlocked                   9400    /*for internal use only*/
#define e_too_long_objects                   9500    /*for internal and APO use only*/
    /* basis error from -27000 to -28999 are located in ggg01 */
    /**/

/******************************************
 *               T Y P E S                *
 ******************************************/


#if defined (__cplusplus)

     #define ci2_gg00    asPasStr ()
     #define ci4_gg00    asPasStr ()
     #define ci6_gg00    asPasStr ()
     #define ci8_gg00    asPasStr ()
     typedef void *tgg00_VoidPtr;
typedef integer                    tgg00_stopper;

#else

typedef integer                    tgg00_stopper;
    /* gg90 substitution used by C and PASCAL  */
    /**/
typedef tsp00_Addr                 tgg00_VoidPtr;

typedef struct                     tgg90_Cint2
{
    tsp00_C2            ci2_gg00;
} tgg90_Cint2;

typedef struct                     tgg90_Cint4
{
    tsp00_C4            ci4_gg00;
} tgg90_Cint4;

typedef struct                     tgg90_Cint6
{
    tsp00_C6            ci6_gg00;
} tgg90_Cint6;

typedef struct                     tgg90_Cint8
{
    tsp00_C8            ci8_gg00;
} tgg90_Cint8;
    /* gg91 substitution used by C and PASCAL  */
typedef tgg90_Cint2                tgg91_FileVersion;
typedef tsp00_C1                   tgg91_ObjAnyBody;
typedef tgg90_Cint6                tgg91_PageRef;
typedef tgg90_Cint4                tgg91_SessionNo;
typedef tgg90_Cint6                tgg91_TransNo;

#endif

    /* */
typedef tsp00_Uint4                tgg00_SubtransNo;
    /* */
typedef tsp00_Int4                 tgg00_ConverterVersion;
    /* */
typedef tsp00_C4                   tgg00_UniquePrimId;
    /* */

typedef enum tgg00_AggSet__Enum
{
    use_result_id,
    prim_keys
} tgg00_AggSet__Enum;
typedef tgg00_AggSet__Enum tgg00_AggSet__Param;
typedef SMALLSET_TYPE_MSP00 (      tgg00_AggSet, tgg00_AggSet__Enum, prim_keys);
    /* */

typedef enum tgg00_BdInvRange_Enum
{
    secondary_stop,
    primary_start,
    primary_stop,
    incl_first
} tgg00_BdInvRange_Enum;
typedef tgg00_BdInvRange_Enum tgg00_BdInvRange_Param;
typedef ENUM_TYPE_MSP00 (          tgg00_BdInvRange, tgg00_BdInvRange_Enum, incl_first, unsigned char);
    /* */

typedef enum tgg00_BdUseSet__Enum
{
    bd_release_acc,
    bd_write_acc,
    bd_load_nolog,
    bd_IgnoreSetToBad,
    bd_fill4,
    bd_fill5,
    bd_fill6,
    bd_fill7
} tgg00_BdUseSet__Enum;
typedef tgg00_BdUseSet__Enum tgg00_BdUseSet__Param;
typedef SMALLSET_TYPE_MSP00 (      tgg00_BdUseSet, tgg00_BdUseSet__Enum, bd_fill7);
    /* */

typedef enum tgg00_DateTimeFormat_Enum
{
    dtf_none,
    dtf_normal,
    dtf_iso,
    dtf_usa,
    dtf_eur,
    dtf_jis,
    dtf_oracle_date,
    dtf_was_ansi_now_is_same_as_iso,
    dtf_ts_eur,
    dtf_future
} tgg00_DateTimeFormat_Enum;
typedef tgg00_DateTimeFormat_Enum tgg00_DateTimeFormat_Param;
typedef ENUM_TYPE_MSP00 (          tgg00_DateTimeFormat, tgg00_DateTimeFormat_Enum, dtf_future, unsigned char);
    /*                        dtf_ variants because of vtcontext */

typedef enum tgg00_Debug_Enum
{
    ak_cach,
    ak_isolev,
    ak_join,
    ak_messbuf,
    ak_receive_error,
    ak_sem,
    ak_sqlpacket,
    ak_strat,
    ak_syn,
    bd,
    bd_buf,
    bd_byte,
    bd_index,
    bd_inv,
    bd_io,
    bd_keym,
    bd_lock,
    bd_oflw,
    bd_psm,
    bi,
    ed,
    fs_ak,
    fs_kb,
    gg,
    gg_time,
    kb,
    kb_dist,
    kb_funct,
    kb_lock,
    kb_lockentry,
    kb_log,
    kb_net,
    kb_qual,
    kb_sel,
    sproc,
    sproc_call,
    sproc_sql,
    st_stack,
    ta,
    td_always,
    test_ak,
    test_bd,
    test_kb,
    ut,
    vs,
    vtr_ak,
    bd_tbuf,
    drda,
    bd_ioqueue,
    ak_packet_head,
    ak_packet_segm,
    ak_packet_part,
    kb_save,
    unicode,
    ak_oldpacket,
    bd_idx_create
} tgg00_Debug_Enum;
typedef tgg00_Debug_Enum tgg00_Debug_Param;
typedef ENUM_TYPE_MSP00 (          tgg00_Debug, tgg00_Debug_Enum, bd_idx_create, unsigned char);
    /* */

typedef enum tgg00_DiagType_Enum
{
    diagNil_egg00,
    diagFDir1_egg00,
    diagFDir2_egg00,
    diagDev_egg00,
    diagInvRoot_egg00,
    diagColRoot_egg00,
    diagLoginfoPage_egg00,
    diagPermPages_egg00,
    diagStaticPages_egg00,
    diagRestart_egg00,
    diagRestore_egg00,
    diagFile_egg00,
    diagLogDev_egg00,
    diagMirrLogDev_egg00,
    diagIsStateAdmin_egg00
} tgg00_DiagType_Enum;
typedef tgg00_DiagType_Enum tgg00_DiagType_Param;
typedef ENUM_TYPE_MSP00 (          tgg00_DiagType, tgg00_DiagType_Enum, diagIsStateAdmin_egg00, unsigned char);
    /* */
    /* PTS 1003033 E.Z. */

typedef enum tgg00_DirectoryMonitorOp_Enum
{
    dir_hit,
    dir_miss,
    func_count
} tgg00_DirectoryMonitorOp_Enum;
typedef tgg00_DirectoryMonitorOp_Enum tgg00_DirectoryMonitorOp_Param;
typedef ENUM_TYPE_MSP00 (          tgg00_DirectoryMonitorOp, tgg00_DirectoryMonitorOp_Enum, func_count, unsigned char);
    /* */

typedef enum tgg00_DumpStateSet__Enum
{
    dumpAll_egg00,
    dumpA51dump_egg00,
    dumpBdLocklist_egg00,
    dumpConfiguration_egg00,
    dumpConverter_egg00,
    dumpConverterCache_egg00,
    dumpDataCache_egg00,
    dumpPagerWriter_egg00,
    dumpFbm_egg00,
    dumpBackup_egg00,
    dumpKbLocklist_egg00,
    dumpLogWriter_egg00,
    dumpLogCache_egg00,
    dumpNetServer_egg00,
    dumpRestartRec_egg00,
    dumpRte_egg00,
    dumpTransformation_egg00,
    dumpUtility_egg00,
    dumpGarbcoll_egg00,
    dumpFileDir_egg00
} tgg00_DumpStateSet__Enum;
typedef tgg00_DumpStateSet__Enum tgg00_DumpStateSet__Param;
typedef SET_TYPE_MSP00 (           tgg00_DumpStateSet, tgg00_DumpStateSet__Enum, dumpFileDir_egg00, 4);
    /* */

typedef enum tgg00_FiletypeSet__Enum
{
    ftsTemp_egg00,
    ftsPerm_egg00,
    ftsByteStr_egg00,
    ftsConcurrent_egg00,
    ftsShared_egg00,
    ftsDynamic_egg00,
    ftsObject_egg00,
    ftsArchive_egg00
} tgg00_FiletypeSet__Enum;
typedef tgg00_FiletypeSet__Enum tgg00_FiletypeSet__Param;
typedef SMALLSET_TYPE_MSP00 (      tgg00_FiletypeSet, tgg00_FiletypeSet__Enum, ftsArchive_egg00);
    /* */

typedef enum tgg00_ExtendedFiletypeSet__Enum
{
    eftsClustered_egg00,
    eftsRecordsPacked_egg00
} tgg00_ExtendedFiletypeSet__Enum;
typedef tgg00_ExtendedFiletypeSet__Enum tgg00_ExtendedFiletypeSet__Param;
typedef SMALLSET_TYPE_MSP00 (      tgg00_ExtendedFiletypeSet, tgg00_ExtendedFiletypeSet__Enum, eftsRecordsPacked_egg00);
    /* */

typedef enum tgg00_HandlingSet__Enum
{
    hsWithoutLock_egg00,
    hsConsistentLock_egg00,
    hsCollisionTest_egg00,
    hsIntentExcl_egg00,
    hsNoWait_egg00,
    hsNoLog_egg00,
    hsTempLock_egg00,
    hsPermLock_egg00,
    hsOptimisticLock_egg00,
    hsCreateFile_egg00,
    hsNoTreeBalance_egg00,
    hsNoWaitForInserts_egg00,
    hsDropFile_egg00,
    hsIgnoreLock_egg00,
    hsFill14_egg00,
    hsFill15_egg00
} tgg00_HandlingSet__Enum;
typedef tgg00_HandlingSet__Enum tgg00_HandlingSet__Param;
typedef SET_TYPE_MSP00 (           tgg00_HandlingSet, tgg00_HandlingSet__Enum, hsFill15_egg00, 2);
    /* */

typedef enum tgg00_IndexMode_Enum
{
    create_index,
    restart_index,
    test_index
} tgg00_IndexMode_Enum;
typedef tgg00_IndexMode_Enum tgg00_IndexMode_Param;
typedef ENUM_TYPE_MSP00 (          tgg00_IndexMode, tgg00_IndexMode_Enum, test_index, unsigned char);
    /* */

typedef enum tgg00_InvListKind_Enum
{
    ilkArray_egg00,
    ilkSubTree_egg00
} tgg00_InvListKind_Enum;
typedef tgg00_InvListKind_Enum tgg00_InvListKind_Param;
typedef ENUM_TYPE_MSP00 (          tgg00_InvListKind, tgg00_InvListKind_Enum, ilkSubTree_egg00, unsigned char);
    /* */

typedef enum tgg00_Linkset__Enum
{
    is_primary_table,
    is_secondary_table,
    del_trigger,
    ins_trigger,
    upd_trigger,
    unique_pk_table,
    internal_trigger
} tgg00_Linkset__Enum;
typedef tgg00_Linkset__Enum tgg00_Linkset__Param;
typedef SMALLSET_TYPE_MSP00 (      tgg00_Linkset, tgg00_Linkset__Enum, internal_trigger);
    /* */

typedef enum tgg00_LockReqMode_Enum
{
    lckFree_egg00,
    lckSysShare_egg00,
    lckSysExcl_egg00,
    lckTabShare_egg00,
    lckTabExcl_egg00,
    lckRowShare_egg00,
    lckRowExcl_egg00,
    lckRowOptimistic_egg00
} tgg00_LockReqMode_Enum;
typedef tgg00_LockReqMode_Enum tgg00_LockReqMode_Param;
typedef ENUM_TYPE_MSP00 (          tgg00_LockReqMode, tgg00_LockReqMode_Enum, lckRowOptimistic_egg00, unsigned char);
    /* */

typedef enum tgg00_LockReqState__Enum
{
    lrsConsistent_egg00,
    lrsEotExcl_egg00,
    lrsTemp_egg00,
    lrsForInsert_egg00,
    lrsIgnoreInsert_egg00
} tgg00_LockReqState__Enum;
typedef tgg00_LockReqState__Enum tgg00_LockReqState__Param;
typedef SMALLSET_TYPE_MSP00 (      tgg00_LockReqState, tgg00_LockReqState__Enum, lrsIgnoreInsert_egg00);
    /* */

typedef enum tgg00_MessBufStruct_Enum
{
    mbs_nil,
    mbs_buf,
    mbs_double_buf,
    mbs_tree_stat_info,
    mbs_save_restore,
    mbs_link,
    mbs_lock,
    mbs_result,
    mbs_col_stat_info,
    mbs_stack_addr,
    mbs_stat_info,
    mbs_tree_id,
    mbs_util,
    mbs_long,
    mbs_copy_long,
    mbs_verify
} tgg00_MessBufStruct_Enum;
typedef tgg00_MessBufStruct_Enum tgg00_MessBufStruct_Param;
typedef ENUM_TYPE_MSP00 (          tgg00_MessBufStruct, tgg00_MessBufStruct_Enum, mbs_verify, unsigned char);
    /* */

typedef enum tgg00_MessType_Enum
{
    m_nil,
    m_begin,
    m_cancel,
    m_change,
    m_check_database,
    m_clear_log,
    m_close,
    m_column,
    m_column_statistic,
    m_commit,
    m_complex,
    m_connect,
    m_invalid,
    m_unused_was_copy_tempfile,
    m_create_file,
    m_create_index,
    m_unused_was_create_serverdb,
    m_create_table,
    m_delete,
    m_unused_was_delete_trans,
    m_describe,
    m_destroy_temp,
    m_diagnose,
    m_down,
    m_drop,
    m_unused_was_drop_serverdb,
    m_end,
    m_eval,
    m_fetch,
    m_flush,
    m_get,
    m_usage,
    m_index_statistic,
    m_init_config,
    m_unused_was_inquire,
    m_insert,
    m_insert_select,
    m_unused_was_insert_trans,
    m_unused_was_integrate_node,
    m_key,
    m_lock,
    m_mend,
    m_mfetch,
    m_move,
    m_unused_was_notify,
    m_unused_was_logpos_list,
    m_outcopy,
    m_unused_was_outcopy_catalog,
    m_partial_rollback,
    m_unused_was_notusedanymore,
    m_release,
    m_reload,
    m_restart,
    m_restore,
    m_return_error,
    m_return_result,
    m_rollback,
    m_save,
    m_unused_was_save_overwrite,
    m_savepoint,
    m_select,
    m_select_row,
    m_set,
    m_show,
    m_shutdown,
    m_shutdown_filesystem,
    m_set_replication,
    m_stamp,
    m_succ_file_version,
    m_table_statistic,
    m_union,
    m_unload,
    m_unlock,
    m_unused_was_unused_log,
    m_up,
    m_unused_was_upd_statistic,
    m_update,
    m_update_rec,
    m_verify,
    m_majority,
    m_unused_was_format,
    m_unused_was_repli_savepoint,
    m_nextval,
    m_history,
    m_sharedSQLParseid,
    m_procedure,
    m_new,
    m_unused_was_create_obj_hash,
    m_get_object,
    m_debug,
    m_save_parallel,
    m_restore_parallel,
    m_autosave,
    m_unused_was_open_trans,
    m_create_index_parallel,
    m_migrate_obj_files,
    m_afterCommit,
    m_afterRollback,
    m_innerTransBegin,
    m_innerTransCommit,
    m_innerTransRollback
} tgg00_MessType_Enum;
typedef tgg00_MessType_Enum tgg00_MessType_Param;
typedef ENUM_TYPE_MSP00 (          tgg00_MessType, tgg00_MessType_Enum, m_innerTransRollback, unsigned char);
    /* */

typedef enum tgg00_MessType2_Enum
{
    mm_nil,
    mm_clear,
    mm_close,
    mm_consistent,
    mm_compare_record,
    mm_config,
    mm_coordinator,
    mm_provoked_error,
    mm_copy_log,
    mm_database,
    mm_unused_was_database_clear_log,
    mm_database_noformat,
    mm_unused_was_dbname,
    mm_destroy_temp,
    mm_device,
    mm_device_restore,
    mm_direct,
    mm_distributed,
    mm_dump,
    mm_empty,
    mm_end_read_only,
    mm_expand,
    mm_file,
    mm_first,
    mm_first_join_select,
    mm_free3,
    mm_fread,
    mm_running_cmds,
    mm_index,
    mm_join_with_last_tab,
    mm_keep_lock,
    mm_key,
    mm_last,
    mm_operator_join,
    mm_lock,
    mm_last_operator_join,
    mm_lock_timeout,
    mm_log,
    mm_log_cold,
    mm_log_demo,
    mm_object_id,
    mm_next,
    mm_nokey,
    mm_ok_not_allowed,
    mm_first_operator_join,
    mm_optimize,
    mm_outcopy,
    mm_pages,
    mm_specialkey,
    mm_parse,
    mm_participant,
    mm_down,
    mm_prefix,
    mm_prev,
    mm_qual,
    mm_standby,
    mm_read,
    mm_rollback,
    mm_same,
    mm_search,
    mm_string,
    mm_subtrans,
    mm_table,
    mm_test,
    mm_trans,
    mm_trunc,
    mm_user,
    mm_with_functions,
    mm_with_join,
    mm_write,
    mm_write_off,
    mm_write_on,
    mm_unused_was_write_reject,
    mm_trigger,
    mm_ignore_duplicates,
    mm_update_duplicates,
    mm_unused_was_fetch_request,
    mm_unused_was_allocate,
    mm_unused_was_bind_req,
    mm_unused_was_exc_req,
    mm_unused_was_deallocate,
    mm_unused_was_reply_cont,
    mm_unused_was_reply_end,
    mm_unused_was_not_used_was_first_distributed,
    mm_unused_was_next_reply,
    mm_acknowledge,
    mm_newtape,
    mm_ignore,
    mm_abort,
    mm_object,
    mm_free4,
    mm_new_write,
    mm_new_read,
    mm_parallel,
    mm_checkpoint,
    mm_unused_was_optimize_longvar,
    mm_last_rowno
} tgg00_MessType2_Enum;
typedef tgg00_MessType2_Enum tgg00_MessType2_Param;
typedef ENUM_TYPE_MSP00 (          tgg00_MessType2, tgg00_MessType2_Enum, mm_last_rowno, unsigned char);
    /* */

typedef enum tgg00_MiscSet__Enum
{
    mi_upd_cols,
    mi_dt_iso,
    mi_dt_usa,
    mi_dt_eur,
    mi_dt_jis,
    mi_dt_oracle_date,
    mi_internal
} tgg00_MiscSet__Enum;
typedef tgg00_MiscSet__Enum tgg00_MiscSet__Param;
typedef SMALLSET_TYPE_MSP00 (      tgg00_MiscSet, tgg00_MiscSet__Enum, mi_internal);
    /* */

typedef enum tgg00_ObjState_Enum
{
    obsFree_egg00,
    obsFreeAfterEot_egg00,
    obsReserved_egg00,
    obsKeyReserved_egg00,
    obsOccupied_egg00,
    obsDelBeforeImage_egg00,
    obsUpdBeforeImage_egg00
} tgg00_ObjState_Enum;
typedef tgg00_ObjState_Enum tgg00_ObjState_Param;
typedef ENUM_TYPE_MSP00 (          tgg00_ObjState, tgg00_ObjState_Enum, obsUpdBeforeImage_egg00, unsigned char);
    /* */

typedef enum tgg00_MonitorPart_Enum
{
    mpAll_egg00,
    mpCaches_egg00,
    mpLoad_egg00,
    mpLog_egg00,
    mpLong_egg00,
    mpPages_egg00,
    mpRow_egg00,
    mpTransaction_egg00
} tgg00_MonitorPart_Enum;
typedef tgg00_MonitorPart_Enum tgg00_MonitorPart_Param;
typedef ENUM_TYPE_MSP00 (          tgg00_MonitorPart, tgg00_MonitorPart_Enum, mpTransaction_egg00, unsigned char);
    /* */

typedef enum tgg00_PageCheckType_Enum
{
    chckNil_egg00,
    chckWriteCnt_egg00,
    chckChecksumData_egg00,
    chckBottomLog_egg00,
    chckChecksumLog_egg00,
    chckChecksumLogInfo_egg00
} tgg00_PageCheckType_Enum;
typedef tgg00_PageCheckType_Enum tgg00_PageCheckType_Param;
typedef ENUM_TYPE_MSP00 (          tgg00_PageCheckType, tgg00_PageCheckType_Enum, chckChecksumLogInfo_egg00, unsigned char);
    /* */

typedef enum tgg00_PageMode__Enum
{
    pmTemp_egg00,
    pmCatalog_egg00,
    pmFdir_egg00,
    pmArchive_egg00,
    pmHasHistory_egg00,
    pmHasHistoryOnPage_egg00,
    pmFill6_egg00,
    pmFill7_egg00
} tgg00_PageMode__Enum;
typedef tgg00_PageMode__Enum tgg00_PageMode__Param;
typedef SMALLSET_TYPE_MSP00 (      tgg00_PageMode, tgg00_PageMode__Enum, pmFill7_egg00);
    /* */

typedef enum tgg00_PageType_Enum
{
    ptNil_egg00,
    ptData_egg00,
    ptDump_egg00,
    ptFbm_egg00,
    ptLog_egg00,
    ptSave_egg00,
    ptSys_egg00,
    ptVtrace_egg00,
    ptConverter_egg00,
    ptConverterBitMap_egg00,
    ptLogInfo_egg00,
    ptIOManInfo_egg00,
    ptFileDir_egg00,
    ptRestart_egg00,
    ptBasic_egg00,
    ptVarSpace_egg00,
    ptPrimaryFile_egg00,
    ptPrimaryFileOverflow_egg00,
    ptPrimaryFileBlob_egg00,
    ptPrimaryFileLocks_egg00,
    ptLastPageType_egg00
} tgg00_PageType_Enum;
typedef tgg00_PageType_Enum tgg00_PageType_Param;
typedef ENUM_TYPE_MSP00 (          tgg00_PageType, tgg00_PageType_Enum, ptLastPageType_egg00, unsigned char);
    /* */

typedef enum tgg00_PageType2_Enum
{
    pt2Nil_egg00,
    pt2BitMap_egg00,
    pt2Checkpt_egg00,
    pt2Config_egg00,
    pt2Converter_egg00,
    pt2EndSaveInfo_egg00,
    pt2EndVolumeInfo_egg00,
    pt2Inv_egg00,
    pt2InvSubtree_egg00,
    pt2Log_egg00,
    pt2LogCleared_egg00,
    pt2LogInfo_egg00,
    pt2String_egg00,
    pt2Tab_egg00,
    pt2Usm_egg00,
    pt2VolumeFirstInfo_egg00,
    pt2VolumeLabelInfo_egg00,
    pt2Xparam_egg00,
    pt2Object_egg00,
    pt2Hash_egg00,
    pt2VarObject_egg00,
    pt2ContObject_egg00,
    pt2UndoLog_egg00,
    pt2RedoLog_egg00,
    pt2History_egg00,
    pt2HistoryDirectory_egg00,
    pt2OpenTrans_egg00
} tgg00_PageType2_Enum;
typedef tgg00_PageType2_Enum tgg00_PageType2_Param;
typedef ENUM_TYPE_MSP00 (          tgg00_PageType2, tgg00_PageType2_Enum, pt2OpenTrans_egg00, unsigned char);
    /* */

typedef enum tgg00_QualKind_Enum
{
    primary_only,
    inv_only,
    inv_and_primary
} tgg00_QualKind_Enum;
typedef tgg00_QualKind_Enum tgg00_QualKind_Param;
typedef ENUM_TYPE_MSP00 (          tgg00_QualKind, tgg00_QualKind_Enum, inv_and_primary, unsigned char);
    /* */

typedef enum tgg00_RecoveryMode_Enum
{
    rmNone_egg00,
    rmTemp_egg00,
    rmPerm_egg00,
    rmStatic_egg00
} tgg00_RecoveryMode_Enum;
typedef tgg00_RecoveryMode_Enum tgg00_RecoveryMode_Param;
typedef ENUM_TYPE_MSP00 (          tgg00_RecoveryMode, tgg00_RecoveryMode_Enum, rmStatic_egg00, unsigned char);
    /* */
    /* PTS 1003033 E.Z. */
    /* PTS 1111134 E.Z. */

typedef enum tgg00_RefInfoIndex_Enum
{
    idata_cache_total,
    idata_cache_hit,
    idata_cache_miss,
    idata_cache_percent,
    iak_cache_total,
    iak_cache_hit,
    iak_cache_miss,
    iak_cache_percent,
    isseq_cache_total,
    isseq_cache_hit,
    isseq_cache_miss,
    isseq_cache_percent,
    idata_sql_cache_total,
    idata_sql_cache_hit,
    idata_sql_cache_miss,
    idata_sql_cache_percent,
    idata_obj_cache_total,
    idata_obj_cache_hit,
    idata_obj_cache_miss,
    idata_obj_cache_percent,
    idata_obj_log_cache_total,
    idata_obj_log_cache_hit,
    idata_obj_log_cache_miss,
    idata_obj_log_cache_percent,
    idata_history_cache_total,
    idata_history_cache_hit,
    idata_history_cache_miss,
    idata_history_cache_percent,
    isql_commands,
    iprepares,
    iprepares_again,
    iexecutes,
    icommits,
    irollbacks,
    ilocks,
    ibegin_subtrans,
    iend_subtrans,
    irollback_subtrans,
    icreates,
    ialters,
    idrops,
    iselects,
    iselects_rows_read,
    iselects_rows_qual,
    iinserts,
    iinserts_rows,
    iupdates,
    iupdates_rows_read,
    iupdates_rows_upd,
    ideletes,
    ideletes_rows_read,
    ideletes_rows_del,
    iinternal_dbproc_calls,
    iexternal_dbproc_calls,
    iinternal_trigger_calls,
    iexternal_trigger_calls,
    iprim_key,
    iprim_key_in,
    iprim_key_subq,
    iprim_key_read,
    iprim_key_qual,
    iprim_key_range,
    iprim_read_key_range,
    iprim_qual_key_range,
    iindex,
    iindex_in,
    iindex_subq,
    iindex_scan,
    iindex_read,
    iindex_qual,
    iindex_range,
    iindex_range_read,
    iindex_range_qual,
    iis_index,
    iis_index_in,
    iis_index_subq,
    iis_index_read,
    iis_index_qual,
    iis_index_range,
    iis_index_read_range,
    iis_index_qual_range,
    itable_scan,
    itable_scan_read,
    itable_scan_qual,
    iis_index_scan,
    iis_index_scan_read,
    iis_index_scan_qual,
    imem_sorts,
    imem_sorts_rows_read,
    iins_sorts,
    iins_sorts_rows_ins,
    ijoin_hash,
    ijoin_parallel,
    ijoin_legacy,
    ijoin_op_normal,
    ijoin_op_improved,
    iavg_lock_used,
    imax_lock_used,
    icollisions,
    iescalations,
    inew_rows,
    inew_tabs,
    ideadlocks,
    irequest_timeouts,
    iread_log,
    iwrite_log,
    ilog_queue_size,
    imaxqueue_used,
    icount_insert,
    icount_request,
    icount_group,
    icount_wait,
    imaxwait_logpage,
    iavg_sum_pagewaits,
    iread_string,
    iwrite_string,
    ir_v_acc,
    iw_v_acc,
    ir_p_acc,
    iw_p_acc,
    icatalog_r_v,
    icatalog_w_v,
    icatalog_r_p,
    icatalog_w_p,
    iperm_r_v,
    iperm_w_v,
    iperm_r_p,
    iperm_w_p,
    itemp_r_v,
    itemp_w_v,
    itemp_r_p,
    itemp_w_p,
    ibstr_r_v,
    ibstr_w_v,
    ibstr_r_p,
    ibstr_w_p,
    ilevel0_r_v,
    ilevel0_w_v,
    ilevel0_r_p,
    ilevel0_w_p,
    ilevel1_r_v,
    ilevel1_w_v,
    ilevel1_r_p,
    ilevel1_w_p,
    ilevel2_r_v,
    ilevel2_w_v,
    ilevel2_r_p,
    ilevel2_w_p,
    ilevel3_r_v,
    ilevel3_w_v,
    ilevel3_r_p,
    ilevel3_w_p,
    ilevel4_r_v,
    ilevel4_w_v,
    ilevel4_r_p,
    ilevel4_w_p,
    ilevel5_r_v,
    ilevel5_w_v,
    ilevel5_r_p,
    ilevel5_w_p,
    ipsm_r_v,
    ipsm_w_v,
    ipsm_r_p,
    ipsm_w_p,
    iadd_rec,
    icadd_rec,
    irepl_rec,
    icrepl_rec,
    idel_rec,
    icdel_rec,
    iget_rec,
    icget_rec,
    inext_rec,
    icnext_rec,
    iprev_rec,
    icprev_rec,
    isel_direc,
    isel_next,
    isel_prev,
    iadd_inv,
    icadd_inv,
    idel_inv,
    icdel_inv,
    iget_inv,
    iget_inv_parallel,
    icget_inv,
    ifunccount,
    itranscount,
    ikb_calls,
    iunused_but_needed
} tgg00_RefInfoIndex_Enum;
typedef tgg00_RefInfoIndex_Enum tgg00_RefInfoIndex_Param;
typedef ENUM_TYPE_MSP00 (          tgg00_RefInfoIndex, tgg00_RefInfoIndex_Enum, iunused_but_needed, unsigned char);
    /* */

typedef enum tgg00_RegionType_Enum
{
    rgnBackup_egg00,
    rgnBreak_egg00,
    rgnConfig_egg00,
    rgnConvert_egg00,
    rgnDatacache_egg00,
    rgnDiagcache_egg00,
    rgnFbm_egg00,
    rgnIntoQueue_egg00,
    rgnLock_egg00,
    rgnLog_egg00,
    rgnLogwriter_egg00,
    rgnNet_egg00,
    rgnNetdown_egg00,
    rgnNetsend_egg00,
    rgnOutQueue_egg00,
    rgnPermfdir_egg00,
    rgnSproc_egg00,
    rgnSurrogate_egg00,
    rgnTempfdir_egg00,
    rgnTrace_egg00,
    rgnData_egg00,
    rgnRow_egg00,
    rgnSave_egg00,
    rgnTab_egg00,
    rgnTrans_egg00,
    rgnOMS_egg00
} tgg00_RegionType_Enum;
typedef tgg00_RegionType_Enum tgg00_RegionType_Param;
typedef ENUM_TYPE_MSP00 (          tgg00_RegionType, tgg00_RegionType_Enum, rgnOMS_egg00, unsigned char);
    /* */

typedef enum tgg00_ServerdbType_Enum
{
    sdb_notused_gateway,
    sdb_db2_gateway,
    sdb_oracle_gateway,
    sdb_internal,
    sdb_remote,
    sdb_odbc_gateway,
    sdb_nil
} tgg00_ServerdbType_Enum;
typedef tgg00_ServerdbType_Enum tgg00_ServerdbType_Param;
typedef ENUM_TYPE_MSP00 (          tgg00_ServerdbType, tgg00_ServerdbType_Enum, sdb_nil, unsigned char);
    /* */

typedef enum tgg00_ShowKind_Enum
{
    sh_no_kind,
    sh_bad_index,
    sh_column,
    sh_connect,
    sh_connect_param,
    sh_constraint,
    sh_cons_columns,
    sh_domain_data,
    sh_snapshot,
    sh_dbm_state,
    sh_explain,
    sh_monitor_oms,
    sh_index,
    sh_link,
    sh_lock,
    sh_mapset,
    sh_instance,
    sh_p_prog,
    sh_param_dbproc,
    sh_param_trigger,
    sh_priv_granted_to,
    sh_hotstandbycomponent,
    sh_sequence,
    sh_hotstandbygroup,
    sh_procedure,
    sh_pseudo_long,
    sh_filedirectories,
    sh_ref_column,
    sh_serverdb,
    sh_servertasks,
    sh_st_config,
    sh_st_database,
    sh_loginformation,
    sh_st_index,
    sh_readerwriterlockinformation,
    sh_kernelmessages,
    sh_st_monitor,
    sh_st_state,
    sh_st_table,
    sh_readerwriterlockwaitingtasks,
    sh_st_user,
    sh_synonym,
    show_fill11,
    show_fill12,
    sh_table,
    sh_taskloadbalancinginformation,
    sh_taskloadbalancingtaskgroupstates,
    sh_taskloadbalancingtaskmoves,
    sh_table_stat,
    show_fill14a,
    show_fill14c,
    sh_trigger,
    show_fill15,
    sh_u_domain,
    sh_multiVersionReadStats,
    show_fill16,
    sh_version_old,
    sh_view,
    sh_all_column,
    sh_all_constraint,
    sh_all_cons_columns,
    sh_all_foreign_key,
    sh_all_index,
    sh_all_priv_granted,
    sf_fill1,
    sh_all_procedures,
    sh_all_sequences,
    sh_all_synonyms,
    sh_all_table,
    sh_all_triggers,
    sh_all_user,
    sh_all_views,
    sh_all_ref_columns,
    sh_info_gc_stats,
    sh_sysmon_connection,
    sh_dbfunctions,
    sh_view_uses_table,
    sh_trans,
    sh_all_view_uses_table,
    sh_new_privileges,
    sh_all_new_privileges,
    sh_domain_constraints,
    sh_dbfunc_parameters,
    sh_iobuffercaches,
    sh_st_lock,
    sh_backtrace,
    sh_odbc_st_index,
    sh_odbc_st_table,
    sh_logqueuestatistics,
    sh_logqueuestatisticsreset,
    show_fill4,
    sh_object,
    sh_objectlocks,
    sh_all_object,
    show_fill2,
    sh_pages,
    sh_roots,
    sh_session_roles,
    sh_userroles,
    sh_all_roles,
    sh_odbc_columns,
    sh_show_view,
    sh_methods,
    show_fill2a,
    sh_oms_versions,
    sh_ctrl_configuration,
    sh_ctrl_devspaces,
    sh_ctrl_io_statistics,
    sh_ctrl_log,
    show_fill17,
    sh_oms_containers,
    sh_ctrl_locks,
    sh_indexpages,
    show_fill3a,
    sh_files_old,
    sh_running_cmds,
    sh_schema,
    sh_sysmon_region,
    sh_sysmon_storage,
    sh_sysmon_ioaccess,
    sh_sysmon_backupioaccess,
    sh_sysmon_specthread,
    sh_sysmon_ukthread,
    sh_sysmon_deviothread,
    sh_sysmon_task,
    sh_sysmon_active_task,
    sh_sysmon_runnable,
    sh_sysmon_us,
    sh_sysmon_us_active,
    sh_sysmon_us_runnable,
    sh_sysmon_sv,
    sh_sysmon_sv_active,
    sh_sysmon_sv_runnable,
    sh_sysmon_dw,
    sh_sysmon_dw_active,
    sh_sysmon_dw_runnable,
    sh_sysmon_totalcount,
    sh_sysmon_task_detail,
    sh_consistent_view,
    sh_history_info,
    sh_oms_memory,
    sh_parsinfos,
    sh_internal_state,
    sh_fill21,
    sh_spinlock,
    sh_oms_lock,
    sh_heap_content,
    sh_odbc_index,
    sh_container_roots,
    sh_container_chains,
    sh_container_keys,
    sh_activeconfiguration,
    sh_backupinformation,
    sh_backupthreads,
    sh_cachestatistics,
    sh_cachestatisticsreset,
    sh_classcontainers,
    sh_classcontainerstatistics,
    sh_classcontainerstatisticsreset,
    sh_datacache,
    sh_datastatistics,
    sh_datastatisticsreset,
    sh_datavolumes,
    sh_catalogcachestatistics,
    sh_fill2,
    sh_files,
    sh_historycleanupstatistics,
    sh_historycleanupstatisticsreset,
    sh_indexstoragedetails,
    sh_iothreadstatistics,
    sh_iothreadstatisticsreset,
    sh_locks,
    sh_lockstatistics,
    sh_lockstatisticsreset,
    sh_logstatistics,
    sh_logstatisticsreset,
    sh_logvolumes,
    sh_memoryallocatorstatistics,
    sh_memoryallocatorstatisticsreset,
    sh_memoryholders,
    sh_miscellaneous,
    sh_omslocks,
    sh_omsprocedurestatistics,
    sh_omsprocedurestatisticsreset,
    sh_omsversions,
    sh_optimizerinformation,
    sh_pagelocks,
    sh_regioninformation,
    sh_regionstatistics,
    sh_regionstatisticsreset,
    sh_restartinformation,
    sh_schemas,
    sh_sessions,
    sh_spinlockstatistics,
    sh_spinlockstatisticsreset,
    sh_tablestoragedetails,
    sh_taskgroupstatistics,
    sh_taskgroupstatisticsreset,
    sh_taskstatistics,
    sh_taskstatisticsreset,
    sh_transactions,
    sh_transactionhistory,
    sh_version,
    sh_commandcachestatistics,
    sh_commandcachestatisticsreset,
    sh_commandstatistics,
    sh_commandstatisticsreset,
    sh_machineconfiguration,
    sh_machineutilization,
    sh_spinlockpoolstatistics,
    sh_spinlockpoolstatisticsreset,
    sh_readerwriterlockstatistics,
    sh_readerwriterlockstatisticsreset,
    sh_omsdiagnose,
    sh_variables,
    sh_unloadedstatements,
    sh_fill3
} tgg00_ShowKind_Enum;
typedef tgg00_ShowKind_Enum tgg00_ShowKind_Param;
typedef ENUM_TYPE_MSP00 (          tgg00_ShowKind, tgg00_ShowKind_Enum, sh_fill3, unsigned char);
    /* */

typedef enum tgg00_SrVtrace2Type_Enum
{
    kb38run_save,
    kb38end_save,
    kb38blocksize,
    kb38que_state,
    kb38rte_errtext,
    kb38hm_wait
} tgg00_SrVtrace2Type_Enum;
typedef tgg00_SrVtrace2Type_Enum tgg00_SrVtrace2Type_Param;
typedef ENUM_TYPE_MSP00 (          tgg00_SrVtrace2Type, tgg00_SrVtrace2Type_Enum, kb38hm_wait, unsigned char);
    /* */

typedef enum tgg00_StackEntryType_Enum
{
    st_fixkey,
    st_varkey,
    st_fixcol,
    st_varcol,
    st_fixinv,
    st_varinv,
    st_fixprimkey,
    st_varprimkey,
    st_old_fixkey,
    st_old_varkey,
    st_old_fixcol,
    st_old_varcol,
    st_old_varlongchar,
    st_varlongchar,
    st_column,
    st_object_key,
    st_param,
    st_dbproc_param,
    st_filler3,
    st_filler4,
    st_get_subquery,
    st_bool,
    st_value,
    st_parseid,
    st_stop,
    st_object_col,
    st_op,
    st_filler5,
    st_output,
    st_result,
    st_rec_output,
    st_jump_true,
    st_jump_false,
    st_jump_output,
    st_jump_absolute,
    st_date,
    st_time,
    st_user,
    st_strat,
    st_dummy,
    st_noround,
    st_truth,
    st_output_join,
    st_filler6,
    st_filler7,
    st_func,
    st_rowno,
    st_datetime_arith,
    st_not_needed,
    st_usergroup,
    st_timestamp,
    st_localsysdba,
    st_sysdba,
    st_output_build_rec,
    st_output_catalog,
    st_build_in_func,
    st_subquery,
    st_mass_compare,
    st_uid,
    st_language,
    st_format,
    st_transaction,
    st_assign,
    st_output_param,
    st_continue_execute,
    st_utcdiff,
    st_case,
    st_utcdate,
    st_dyn_sql,
    st_surrogate,
    st_timezone,
    st_sum_length,
    st_current_schema
} tgg00_StackEntryType_Enum;
typedef tgg00_StackEntryType_Enum tgg00_StackEntryType_Param;
typedef ENUM_TYPE_MSP00 (          tgg00_StackEntryType, tgg00_StackEntryType_Enum, st_current_schema, unsigned char);
    /* */

typedef enum tgg00_StackOpBuildIn_Enum
{
    op_b_none,
    op_b_check_format,
    op_b_checklen,
    op_b_chr,
    op_b_concat,
    op_b_datetime,
    op_b_dbyte_concat,
    op_b_dbyte_substr,
    op_b_expand,
    op_b_format_change,
    op_b_index,
    op_b_lfill,
    op_b_lpad,
    op_b_ltrim,
    op_b_rfill,
    op_b_rpad,
    op_b_rtrim,
    op_b_substr,
    op_b_chr_ora,
    op_b_trim,
    op_b_greatest,
    op_b_least,
    op_b_value,
    op_b_decode,
    op_b_to_char,
    op_b_to_24_char,
    op_b_length_ora,
    op_b_ascii_ora,
    op_b_next_day,
    op_b_user_def_func,
    op_b_currval,
    op_b_exp,
    op_b_ln,
    op_b_log,
    op_b_sin,
    op_b_cos,
    op_b_tan,
    op_b_sinh,
    op_b_cosh,
    op_b_tanh,
    op_b_to_date,
    op_b_new_time,
    op_b_ts_trunc,
    op_b_ts_round,
    op_b_acos,
    op_b_asin,
    op_b_atan,
    op_b_atan2,
    op_b_cot,
    op_b_log10,
    op_b_pi,
    op_b_degrees,
    op_b_radians,
    op_b_left,
    op_b_right,
    op_b_space,
    op_b_dayofmonth,
    op_b_namefromdate,
    op_b_toidentifier,
    op_b_dbyte_lfill,
    op_b_dbyte_rfill,
    op_b_dbyte_lpad,
    op_b_dbyte_rpad,
    op_b_dbyte_trim,
    op_b_dbyte_rtrim,
    op_b_dbyte_ltrim,
    op_b_uni_trans,
    op_b_float,
    op_b_case_start,
    op_b_case_stop,
    op_b_long_substr
} tgg00_StackOpBuildIn_Enum;
typedef tgg00_StackOpBuildIn_Enum tgg00_StackOpBuildIn_Param;
typedef ENUM_TYPE_MSP00 (          tgg00_StackOpBuildIn, tgg00_StackOpBuildIn_Enum, op_b_long_substr, unsigned char);
    /* */

typedef enum tgg00_StackOpFunc_Enum
{
    op_f_none,
    op_f_all_count,
    op_f_avg,
    op_f_check_null,
    op_f_count,
    op_f_count_kb,
    op_f_dis_avg,
    op_f_dis_count,
    op_f_dis_sum,
    op_f_max,
    op_f_min,
    op_f_sum,
    op_f_stddev,
    op_f_dis_stddev,
    op_f_variance,
    op_f_dis_variance
} tgg00_StackOpFunc_Enum;
typedef tgg00_StackOpFunc_Enum tgg00_StackOpFunc_Param;
typedef ENUM_TYPE_MSP00 (          tgg00_StackOpFunc, tgg00_StackOpFunc_Enum, op_f_dis_variance, unsigned char);
    /* */

typedef enum tgg00_StackOpOutput_Enum
{
    op_o_none,
    op_o_output,
    op_o_output_fixcol,
    op_o_output_fixkey,
    op_o_output_hold,
    op_o_output_key,
    op_o_output_later,
    op_o_output_longvar,
    op_o_output_no_oflw,
    op_o_output_oflw,
    op_o_output_order,
    op_o_output_var,
    op_o_output_not_fill,
    op_o_output_outer_join,
    op_o_output_column,
    op_o_output_cluster_key
} tgg00_StackOpOutput_Enum;
typedef tgg00_StackOpOutput_Enum tgg00_StackOpOutput_Param;
typedef ENUM_TYPE_MSP00 (          tgg00_StackOpOutput, tgg00_StackOpOutput_Enum, op_o_output_cluster_key, unsigned char);
    /* */

typedef enum tgg00_StackOpType_Enum
{
    op_none,
    op_mapchar,
    op_ascii,
    op_dbyte_length,
    op_desc_expr_upd,
    op_desc_unique_expr_upd,
    op_digits,
    op_ebcdic,
    op_expr_upd,
    op_fixed,
    op_hex,
    op_in,
    op_initcap,
    op_jmp_ins_upd,
    op_length,
    op_lowcase,
    op_not_in,
    op_num,
    op_odd,
    op_order_asc,
    op_order_desc,
    op_order_desc_ascii,
    op_order_desc_ebcdic,
    op_outer_join,
    op_remove,
    op_replace,
    op_soundex,
    op_stamp,
    op_unique,
    op_unique_desc,
    op_unique_desc_ascii,
    op_unique_desc_ebcdic,
    op_unique_expr_upd,
    op_upcase,
    op_get_sub_value,
    op_translate,
    op_hextoraw,
    op_join_key,
    op_late_asc_unique_check,
    op_late_desc_unique_check,
    op_nextval,
    op_scol_upd,
    op_dbyte_mapchar,
    op_updated,
    op__filler3,
    op__filler4,
    op_longcol_update,
    op_adddate,
    op_addmonth,
    op_addtime,
    op_date,
    op_date_from_timestamp,
    op_datediff,
    op_dayofweek,
    op_dayofyear,
    op_days,
    op_last_day,
    op_makedate,
    op_maketime,
    op_months_between,
    op_subdate,
    op_submonth,
    op_subtime,
    op_time,
    op_timediff,
    op_timestamp,
    op_weekofyear,
    op_swap,
    op_copy,
    op_filler8,
    op_abs,
    op_ceil,
    op_floor,
    op_fracround,
    op_fractrunc,
    op_power,
    op_round,
    op_sign,
    op_sqrt,
    op_trunc,
    op_filler9,
    op_filler10,
    op_filler11,
    op_div,
    op_intdiv,
    op_minus,
    op_minus_monad,
    op_mod,
    op_mod_func,
    op_mult,
    op_plus,
    op_filler12,
    op_filler13,
    op_is_integer,
    op_between,
    op_not_between,
    op_not,
    op_upd_view_and,
    op_and,
    op_or,
    op_like,
    op_not_like,
    op_not_null,
    op_not_sounds,
    op_null,
    op_sounds,
    op_true,
    op_false,
    op_undef_to_false,
    op_eq,
    op_eq_all,
    op_ge,
    op_gt,
    op_le,
    op_lt,
    op_ne,
    op_dbyte_upper,
    op_dbyte_lower,
    op_dbyte_initcap,
    op_dbyte_replace,
    op_dbyte_remove,
    op_dbyte_translate,
    op_serial,
    op_test_zero
} tgg00_StackOpType_Enum;
typedef tgg00_StackOpType_Enum tgg00_StackOpType_Param;
typedef ENUM_TYPE_MSP00 (          tgg00_StackOpType, tgg00_StackOpType_Enum, op_test_zero, unsigned char);
    /* */

typedef enum tgg00_TableKind_Enum
{
    tempty,
    twithkey,
    twithoutkey,
    tbytestring,
    tpartition,
    tonebase,
    tdummy,
    tview,
    tsynonym,
    tresult,
    tcomplexview,
    tdb2view,
    tunused1,
    tunused2,
    tshow_view,
    tcatalog_table,
    tunsed3,
    tunsed4
} tgg00_TableKind_Enum;
typedef tgg00_TableKind_Enum tgg00_TableKind_Param;
typedef ENUM_TYPE_MSP00 (          tgg00_TableKind, tgg00_TableKind_Enum, tunsed4, unsigned char);
    /* */

typedef enum tgg00_Tfn_Enum
{
    tfnSys_egg00,
    tfnColumn_egg00,
    tfnAux_egg00,
    tfnOmsInv_egg00,
    tfn__FurtherUse04_egg00,
    tfn__FurtherUse05_egg00,
    tfn__FurtherUse06_egg00,
    tfnMulti_egg00,
    tfn__FurtherUse08_egg00,
    tfn__FurtherUse09_egg00,
    tfnInvSubtree_egg00,
    tfnTempMulti_egg00,
    tfnNil_egg00,
    tfnTable_egg00,
    tfnUndoLog_egg00,
    tfnRedoLog_egg00,
    tfnHistory_egg00,
    tfnOpenTrans_egg00,
    tfnShortScol_egg00,
    tfnLog_egg00,
    tfnHistoryDirectory_egg00,
    tfn__FurtherUse21_egg00,
    tfnObj_egg00,
    tfnObjBefore_egg00,
    tfnContObj_egg00,
    tfn__FurtherUse25_egg00,
    tfnTemp_egg00,
    tfnTempAux_egg00,
    tfn__FurtherUse28_egg00,
    tfnTempOms_egg00
} tgg00_Tfn_Enum;
typedef tgg00_Tfn_Enum tgg00_Tfn_Param;
typedef ENUM_TYPE_MSP00 (          tgg00_Tfn, tgg00_Tfn_Enum, tfnTempOms_egg00, unsigned char);
    /* */

typedef enum tgg00_TfnLog_Enum
{
    tfnlNone_egg00,
    tfnlSavepoint_egg00,
    tfnlCreateIndex_egg00,
    tfnlRollback_egg00
} tgg00_TfnLog_Enum;
typedef tgg00_TfnLog_Enum tgg00_TfnLog_Param;
typedef ENUM_TYPE_MSP00 (          tgg00_TfnLog, tgg00_TfnLog_Enum, tfnlRollback_egg00, unsigned char);
    /* */

typedef enum tgg00_TfnTemp_Enum
{
    ttfnNone_egg00,
    ttfnComplexSelect_egg00,
    ttfnComplexView_egg00,
    ttfnFromSelect_egg00,
    ttfnUserResult_egg00,
    ttfnTempTable_egg00,
    ttfnPars_egg00,
    ttfnParsTrigger_egg00,
    ttfnInto_egg00,
    ttfnKeyUpdate_egg00,
    ttfnUsage_egg00,
    ttfnJoinResult_egg00,
    ttfnSubquery_egg00,
    ttfnCorrelation_egg00,
    ttfnLink_egg00,
    ttfnColumn_egg00,
    ttfnDistinct_egg00,
    ttfnSaveScheme_egg00,
    ttfnErrorText_egg00,
    ttfnUnion_egg00,
    ttfnUnionIndex_egg00,
    ttfnInternResult_egg00,
    ttfnTempInv_egg00,
    ttfnlongsurrogates_egg00,
    ttfnOuterJoinKeys_egg00,
    ttfnOuterJoin_egg00,
    ttfnUnionIntersect_egg00,
    ttfnTrigger_egg00,
    ttfnFiller28_egg00,
    ttfnPendingLink_egg00,
    ttfnLateUniqueCheck_egg00,
    ttfnCacheRollback_egg00,
    ttfnLogPosList_egg00,
    ttfnTempLog_egg00,
    ttfnFiller34_egg00,
    ttfnOnlinePno_egg00,
    ttfnGatewayResult_egg00,
    ttfnRecursive_egg00,
    ttfnOmsVersion_egg00,
    ttfnOmsVersionIndex_egg00
} tgg00_TfnTemp_Enum;
typedef tgg00_TfnTemp_Enum tgg00_TfnTemp_Param;
typedef ENUM_TYPE_MSP00 (          tgg00_TfnTemp, tgg00_TfnTemp_Enum, ttfnOmsVersionIndex_egg00, unsigned char);
    /* */

typedef enum tgg00_TransState__Enum
{
    tsLocked_egg00,
    tsTempUpdated_egg00,
    tsSurrogateUpd_egg00,
    tsBdLocked_egg00,
    tsDbFullNowait_egg00,
    tsRollback_egg00,
    tsIgnoreDBSecureLimit_egg00,
    tsFill07_egg00,
    tsFill08_egg00,
    tsFunctRollbSet_egg00,
    tsUpdated_egg00,
    tsAccessedInSession_egg00,
    tsErrTextExist_egg00,
    tsSubUpdated_egg00,
    tsFill14_egg00,
    tsFill15_egg00
} tgg00_TransState__Enum;
typedef tgg00_TransState__Enum tgg00_TransState__Param;
typedef SET_TYPE_MSP00 (           tgg00_TransState, tgg00_TransState__Enum, tsFill15_egg00, 2);
    /* */

typedef enum tgg00_VtraceType_Enum
{
    td_none,
    ak_send,
    ak_receive,
    ak_reject,
    b02add,
    b02del,
    b02direct_qual,
    b02first_qual,
    b02get,
    b02kb_del,
    b02kb_repl,
    b02last_qual,
    b02logadd,
    b02next,
    b02next_qual,
    b02prev,
    b02prev_qual,
    b02repl,
    b03add,
    b03create,
    b03del,
    b03get,
    b03first_qual,
    b03last_qual,
    b03next_list,
    b03next_qual,
    b03prev_list,
    b03prev_qual,
    b03test,
    b05copy,
    b05expand,
    b05read,
    b05write,
    b07cadd,
    b07cdel,
    b07cget,
    b07cnext,
    b07cprev,
    b07crepl,
    b15read_page,
    b15write_page,
    b_await_r,
    b_await_w,
    vttKbFunctions_egg00,
    vttKbColdFunctions_egg00,
    vttKbClearLockList_egg00,
    vttKbRequestTimeout_egg00,
    vttKbEmergencyShutdown_egg00,
    vttKbQuickShutdown_egg00,
    vttKbShutdown_egg00,
    vttKbLockCollision_egg00,
    vttKbLockWait_egg00,
    vttKbLockWaitResume_egg00,
    vttKbRedoLogError_egg00,
    vttKbServer_egg00,
    b02exists,
    b07cappend,
    b07cget_leaf,
    b03pcreate,
    b13share_wait,
    b13exclusive_wait,
    gg_opmsg,
    ak_join_strat,
    ak_strat_with_int,
    bd_timer,
    b07crel_leaf,
    b01b_create,
    b01create,
    b01destroy,
    b01empty,
    b01p_destroy,
    b01rename,
    b01t_create,
    b01t_reset,
    b05find,
    b05getset,
    b05length,
    b05trunc,
    b01verify,
    vttSlowTrace_egg00,
    b03aggr,
    b03eval,
    kb38save_parallel,
    kb38restore_parallel,
    b03check,
    b03empty,
    b03intersect,
    b03merge,
    b03imerge,
    b09append,
    b09next,
    b09prev,
    b04create,
    b04drop,
    b04del,
    b04DelAll,
    b04get_consistent,
    b04GetObjWithKey,
    b04lock,
    b04info,
    b04IsSelfLocked,
    b04new,
    b95redo_upd,
    b04ResetLock,
    b04rupd,
    b04unlock,
    b04upd,
    b04next_consistent,
    b95redo_del,
    b95redo_new,
    b91garbcol,
    kb04oms,
    b04newhash_consistent,
    b04newhash,
    b22rem_oid,
    b92verify,
    b92check_hash,
    b930DropFile,
    b930Garbcol,
    b930RemovePage,
    b04IsHistUsed,
    vttBdLockUpd_egg00,
    b95removeobj,
    b95delkey,
    b04CreateKeyIter,
    b04DestroyIter,
    b04NextKey,
    b91repair,
    ak_returncode,
    vttOperatorJoin_egg00,
    vttAccessOperator_egg00,
    ak_strat_with_bool
} tgg00_VtraceType_Enum;
typedef tgg00_VtraceType_Enum tgg00_VtraceType_Param;
typedef ENUM_TYPE_MSP00 (          tgg00_VtraceType, tgg00_VtraceType_Enum, ak_strat_with_bool, unsigned char);
    /* */

typedef enum tgg00_JoinSearchLevel_Enum
{
    jsLevel0_egg00,
    jsLevel1_egg00,
    jsLevel2_egg00,
    jsLevel3_egg00,
    jsLevel4_egg00,
    jsLevel5_egg00,
    jsLevel6_egg00,
    jsLevel7_egg00,
    jsLevel8_egg00,
    jsLevel9_egg00
} tgg00_JoinSearchLevel_Enum;
typedef tgg00_JoinSearchLevel_Enum tgg00_JoinSearchLevel_Param;
typedef ENUM_TYPE_MSP00 (          tgg00_JoinSearchLevel, tgg00_JoinSearchLevel_Enum, jsLevel9_egg00, unsigned char);
    /* */
typedef ARRAY_TYPE_MSP00 (         tgg00_AuxBuffers, tsp00_Page, MAX_AUX_BUFFER_COUNT_GG00);
typedef tgg00_AuxBuffers          *tgg00_AuxBuffersPtr;
typedef tsp00_Int2                 tgg00_BasisError;
typedef SMALLSET_TYPE_MSP00 (      tgg00_BdInvSet, tgg00_BdInvRange_Enum, incl_first);
typedef OLDPASCALSTRING_TYPE_MSP00 (  tgg00_DoubleBuf, DOUBLE_BUF_MXGG00);
typedef CSTRING_TYPE_MSP00      (  tgg00_DoubleBufc, DOUBLE_BUF_MXGG00);
typedef tsp00_C16                  tgg00_Filename;
typedef ARRAY_TYPE_MSP00 (         tgg00_Filetypes, tsp00_VfType, MAX_TAPES_GG00);
typedef ARRAY_TYPE_MSP00 (         tgg00_HashPrimeList, tsp00_Int4, MAX_HASH_PRIME_GG00);
typedef tsp00_Int2                 tgg00_KeyLen;
typedef tsp00_Buf                  tgg00_KeylistBuf;
typedef OLDPASCALSTRING_TYPE_MSP00 (  tgg00_LogBody, LOGPAGE_BODY_MXGG00);
typedef CSTRING_TYPE_MSP00      (  tgg00_LogBodyc, LOGPAGE_BODY_MXGG00);
typedef ARRAY_TYPE_MSP00 (         tgg00_MaxstackArr, tsp00_Int4, MAX_STACK_GG00);
typedef tsp00_C64                  tgg00_MediaName;
typedef tsp00_Uint4                tgg00_ObjFileNo;
typedef tsp00_Uint1                tgg00_ObjFrameVers;
typedef tsp00_Uint2                tgg00_ObjPagePos;
typedef OLDPASCALSTRING_TYPE_MSP00 (  tgg00_PageHeadStr, PAGE_HEADER_MXGG00);
typedef CSTRING_TYPE_MSP00      (  tgg00_PageHeadStrc, PAGE_HEADER_MXGG00);
typedef tsp00_Int4                 tgg00_PagePos;
typedef OLDPASCALSTRING_TYPE_MSP00 (  tgg00_RecBody, RECBODY_MXGG00);
typedef CSTRING_TYPE_MSP00      (  tgg00_RecBodyc, RECBODY_MXGG00);
typedef OLDPASCALSTRING_TYPE_MSP00 (  tgg00_RecPart, RECPART_MXGG00);
typedef CSTRING_TYPE_MSP00      (  tgg00_RecPartc, RECPART_MXGG00);
typedef ARRAY_TYPE_MSP00 (         tgg00_RootList, tsp00_PageNo, MAX_ROOTLIST_GG00);
typedef tgg00_RootList            *tgg00_RootListPtr;
typedef tsp00_C2                   tgg00_ServerdbNo;
typedef OLDPASCALSTRING_TYPE_MSP00 (  tgg00_SfpInitPart, SFP_INIT_PART_MXGG00);
typedef CSTRING_TYPE_MSP00      (  tgg00_SfpInitPartc, SFP_INIT_PART_MXGG00);
typedef SET_TYPE_MSP00 (           tgg00_StackOpFuncSet, tgg00_StackOpFunc_Enum, op_f_dis_variance, 2);
typedef tsp00_C8                   tgg00_Surrogate;
typedef OLDPASCALSTRING_TYPE_MSP00 (  tgg00_Syskey, SYSKEY_MXGG00);
typedef CSTRING_TYPE_MSP00      (  tgg00_Syskeyc, SYSKEY_MXGG00);
typedef ARRAY_TYPE_MSP00 (         tgg00_TapeCount, tsp00_Int4, MAX_TAPES_GG00);
typedef ARRAY_TYPE_MSP00 (         tgg00_TapeNames, tsp00_VFilename, MAX_TAPES_GG00);
typedef tsp00_Int2                 tgg00_TransIndex;
typedef OLDPASCALSTRING_TYPE_MSP00 (  tgg00_FileIdHead, FILE_ID_HEAD_MXGG00);
typedef CSTRING_TYPE_MSP00      (  tgg00_FileIdHeadc, FILE_ID_HEAD_MXGG00);
typedef tsp00_C4                   tgg00_UserId;
typedef tgg91_PageRef              tgg00_UserRef;
typedef tsp00_C60                  tgg00_VtraceKey;

typedef struct                     tgg00_UtilCmdId
{
    tsp00_C12           utidId_gg00;
    tsp00_Int4          utidLineNo_gg00;
} tgg00_UtilCmdId;

typedef struct                     tgg00_OidSpace
{
    union
    {
    /* 1 */
        struct tgg00_OidSpace_1
        {
            tsp00_Int4          oidFirstI4_gg00_F;
            tsp00_Int4          oidSecondI4_gg00_F;
        } C_1;
    /* 2 */
        struct tgg00_OidSpace_2
        {
            tsp00_C8            oidC8_gg00_F;
        } C_2;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tsp00_Int4         &oidFirstI4_gg00        ()  { return this->variant.C_1.oidFirstI4_gg00_F; };
    tsp00_Int4         &oidSecondI4_gg00       ()  { return this->variant.C_1.oidSecondI4_gg00_F; };
    tsp00_C8           &oidC8_gg00             ()  { return this->variant.C_2.oidC8_gg00_F; };
    /* const accessor functions */
    const tsp00_Int4   &oidFirstI4_gg00        () const  { return this->variant.C_1.oidFirstI4_gg00_F; };
    const tsp00_Int4   &oidSecondI4_gg00       () const  { return this->variant.C_1.oidSecondI4_gg00_F; };
    const tsp00_C8     &oidC8_gg00             () const  { return this->variant.C_2.oidC8_gg00_F; };
#endif    /* defined (__cplusplus) */
} tgg00_OidSpace;

typedef struct                     tgg00_MonitorRec
{
    tsp00_C12           mon_parseid;
    tsp00_C12           mon_masterparseid;
    tgg00_Surrogate     mon_cmdid;
    tsp00_C40           mon_strat;
    tsp00_C3            mon_copy_result;
    pasbool             mon_is_select;
    tsp00_Int4          mon_rows_read;
    tsp00_Int4          mon_rows_qual;
    tsp00_Int4          mon_rows_fetched;
    tsp00_Int4          mon_fetch_calls;
    tsp00_Int4          mon_virt_reads;
    tsp00_Int4          mon_kb_calls;
    tsp00_Int4          mon_waits;
    tsp00_Int4          mon_suspends;
    tsp00_Int4          mon_phys_ios;
    tsp00_Int4          mon_sec;
    tsp00_Int4          mon_microsec;
    tsp00_Int2          mon_strat_len;
    tsp00_Int2          mon_rootlist_elems;
    tgg00_RootList      mon_rootlist;
} tgg00_MonitorRec;

typedef struct                     tgg00_UserMonitorInfo
{
    tgg00_MonitorRec    umi_curr_monitor;
    tgg00_MonitorRec    umi_sum_monitor;
    tsp00_KnlIdentifier umi_username;
    tsp00_TermId        umi_termid;
    tsp00_C3            umi_parse_again;
    pasbool             umi_control_session;
    tsp00_Int4          umi_obj_hist_count;
    tsp00_Int4          umi_values_len;
    tsp00_Int4          umi_values_size;
    tsp00_MoveObjPtr    umi_values;
} tgg00_UserMonitorInfo;
typedef tgg00_UserMonitorInfo     *tgg00_UserMonitorInfoPtr;

typedef struct                     tgg00_TempDataCache
{
    tgg00_UserMonitorInfoPtr tdc_monitor_info;
} tgg00_TempDataCache;
typedef tgg00_TempDataCache       *tgg00_TempDataCachePtr;

typedef struct                     tgg00_SaveRestoreInputParam
{
    tsp00_Int2          sripBlocksize_gg00;
    tsp00_Int2          sripHostTapeNum_gg00;
    tsp00_Int4          sripFileVersion_gg00;
    pasbool             sripIsAutoLoad_gg00;
    pasbool             sripWithCheckpoint_gg00;
    pasbool             sripIsRestoreConfig_gg00;
    pasbool             sripOnlyCompleteSegments_gg00;
    tgg00_TapeNames     sripHostTapenames_gg00;
    tgg00_Filetypes     sripHostFiletypes_gg00;
    tgg00_TapeCount     sripHostTapecount_gg00;
    tgg00_MediaName     sripMedianame_gg00;
    tsp00_Date          sripUntilDate_gg00;
    tsp00_Time          sripUntilTime_gg00;
    tsp00_Uint4         sripUntilIOSequence_gg00;
    tgg00_UtilCmdId     sripUtilCmdId_gg00;
    tsp00_DbName        sripConfigDbName_gg00;
    tsp00_C14           sripTapeLabel_gg00;
    pasbool             sripNoReleaseLog_gg00;
    pasbool             sripWithFormat_gg00;
} tgg00_SaveRestoreInputParam;

typedef struct                     tgg00_PageHeader
{
    union
    {
    /* 1 */
        struct tgg00_PageHeader_1
        {
            tsp00_PageNo        pageId_gg00_F;
            tgg00_PageType      pageType_gg00_F;
            tgg00_PageType2     pageType2_gg00_F;
            tgg00_PageCheckType pageCheck_gg00_F;
            tgg00_PageMode      pageMode_gg00_F;
        } C_1;
    /* 2 */
        struct tgg00_PageHeader_2
        {
            tgg00_PageHeadStr   pageHeaderStr_gg00_F;
        } C_2;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tsp00_PageNo       &pageId_gg00            ()  { return this->variant.C_1.pageId_gg00_F; };
    tgg00_PageType     &pageType_gg00          ()  { return this->variant.C_1.pageType_gg00_F; };
    tgg00_PageType2    &pageType2_gg00         ()  { return this->variant.C_1.pageType2_gg00_F; };
    tgg00_PageCheckType &pageCheck_gg00         ()  { return this->variant.C_1.pageCheck_gg00_F; };
    tgg00_PageMode     &pageMode_gg00          ()  { return this->variant.C_1.pageMode_gg00_F; };
    tgg00_PageHeadStr  &pageHeaderStr_gg00     ()  { return this->variant.C_2.pageHeaderStr_gg00_F; };
    /* const accessor functions */
    const tsp00_PageNo &pageId_gg00            () const  { return this->variant.C_1.pageId_gg00_F; };
    const tgg00_PageType &pageType_gg00          () const  { return this->variant.C_1.pageType_gg00_F; };
    const tgg00_PageType2 &pageType2_gg00         () const  { return this->variant.C_1.pageType2_gg00_F; };
    const tgg00_PageCheckType &pageCheck_gg00         () const  { return this->variant.C_1.pageCheck_gg00_F; };
    const tgg00_PageMode &pageMode_gg00          () const  { return this->variant.C_1.pageMode_gg00_F; };
    const tgg00_PageHeadStr &pageHeaderStr_gg00     () const  { return this->variant.C_2.pageHeaderStr_gg00_F; };
#endif    /* defined (__cplusplus) */
} tgg00_PageHeader;
typedef tgg00_PageHeader           tgg00_PageTrailer;

typedef struct                     tgg00_PageCheckTrailer
{
    tsp00_Int4          trlChecksum_gg00;
    tgg00_PageTrailer   trlTrailer_gg00;
} tgg00_PageCheckTrailer;

typedef struct                     tgg00_TimeoutGlobals
{
    tsp00_Int4          timeCmd_gg00;
    tsp00_Int4          timeReq_gg00;
} tgg00_TimeoutGlobals;

typedef struct                     tgg00_Lkey
{
    union
    {
    /* 1 */
        struct tgg00_Lkey_1
        {
            tsp00_Int2          keyRecLenSpace_gg00_F;
            tsp00_Int2          keyLen_gg00_F;
            tsp00_Int2          keyVarOffSpace_gg00_F;
            tsp00_Int2          keyVarCntSpace_gg00_F;
            tsp00_Key           keyVal_gg00_F;
        } C_1;
    /* 11 */
        struct tgg00_Lkey_11
        {
            tsp00_Int2          kfill_F;
            tsp00_Int2          len_F;
            tsp00_Int2          kfill1_F;
            tsp00_Int2          kfill2_F;
            tsp00_Key           k_F;
        } C_11;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tsp00_Int2         &keyRecLenSpace_gg00    ()  { return this->variant.C_1.keyRecLenSpace_gg00_F; };
    tsp00_Int2         &keyLen_gg00            ()  { return this->variant.C_1.keyLen_gg00_F; };
    tsp00_Int2         &keyVarOffSpace_gg00    ()  { return this->variant.C_1.keyVarOffSpace_gg00_F; };
    tsp00_Int2         &keyVarCntSpace_gg00    ()  { return this->variant.C_1.keyVarCntSpace_gg00_F; };
    tsp00_Key          &keyVal_gg00            ()  { return this->variant.C_1.keyVal_gg00_F; };
    tsp00_Int2         &kfill                  ()  { return this->variant.C_11.kfill_F; };
    tsp00_Int2         &len                    ()  { return this->variant.C_11.len_F; };
    tsp00_Int2         &kfill1                 ()  { return this->variant.C_11.kfill1_F; };
    tsp00_Int2         &kfill2                 ()  { return this->variant.C_11.kfill2_F; };
    tsp00_Key          &k                      ()  { return this->variant.C_11.k_F; };
    /* const accessor functions */
    const tsp00_Int2   &keyRecLenSpace_gg00    () const  { return this->variant.C_1.keyRecLenSpace_gg00_F; };
    const tsp00_Int2   &keyLen_gg00            () const  { return this->variant.C_1.keyLen_gg00_F; };
    const tsp00_Int2   &keyVarOffSpace_gg00    () const  { return this->variant.C_1.keyVarOffSpace_gg00_F; };
    const tsp00_Int2   &keyVarCntSpace_gg00    () const  { return this->variant.C_1.keyVarCntSpace_gg00_F; };
    const tsp00_Key    &keyVal_gg00            () const  { return this->variant.C_1.keyVal_gg00_F; };
    const tsp00_Int2   &kfill                  () const  { return this->variant.C_11.kfill_F; };
    const tsp00_Int2   &len                    () const  { return this->variant.C_11.len_F; };
    const tsp00_Int2   &kfill1                 () const  { return this->variant.C_11.kfill1_F; };
    const tsp00_Int2   &kfill2                 () const  { return this->variant.C_11.kfill2_F; };
    const tsp00_Key    &k                      () const  { return this->variant.C_11.k_F; };
#endif    /* defined (__cplusplus) */
} tgg00_Lkey;
typedef tgg00_Lkey                *tgg00_LkeyPtr;
    /* UH 2003-02-03 */
typedef struct tgg00_WaitContext tgg00_WaitContext;
typedef tgg00_WaitContext         *tgg00_WaitContextPtr;

 struct                     tgg00_WaitContext
{
    tgg00_WaitContextPtr wcNextWaiting_gg00;
    tsp00_TaskId        wcWaitingTaskId_gg00;

#if defined (BIT64)

    tsp00_Int4          wcFiller_gg00;

#endif

} ;
    /* PTS 1110096 E.Z. */

typedef struct                     tgg00_OneCurrSeqInfo
{
    tgg00_Surrogate     ocs_seq_surrogate;
    tsp00_Number        ocs_seq_value;
    tsp00_Int4          ocs_seq_filler;
} tgg00_OneCurrSeqInfo;

typedef struct                     tgg00_CurrSeqInfo
{
    tsp00_Int2          csi_cnt;
    tsp00_Int2          csi_fill1;
    tsp00_Int4          csi_fill2;
    ARRAY_VAR_MSP00 (         csi_values, tgg00_OneCurrSeqInfo, MAX_SEQINFOS_GG00);
} tgg00_CurrSeqInfo;
typedef tgg00_CurrSeqInfo         *tgg00_CurrSeqInfoPtr;

typedef enum tgg00_ConsistentReadInfo_Enum
{
    CnsReadNone,
    CnsReadStatement,
    CnsReadTransaction
} tgg00_ConsistentReadInfo_Enum;
typedef tgg00_ConsistentReadInfo_Enum tgg00_ConsistentReadInfo_Param;
typedef ENUM_TYPE_MSP00 (          tgg00_ConsistentReadInfo, tgg00_ConsistentReadInfo_Enum, CnsReadTransaction, unsigned char);

typedef struct                     tgg00_TransContext
{
    tgg91_SessionNo     trSessionId_gg00;
    tgg91_TransNo       trTransId_gg00;
    tgg91_TransNo       trWriteTransId_gg00;
    tgg00_SubtransNo    trSubtransId_gg00;
    tsp00_Int4          trChild_gg00;
    tgg91_TransNo       trConsistView_gg00;
    tgg00_ConsistentReadInfo trConsistReadInfo_gg00;
    pasbool             trFiller1_gg00;
    tgg00_TransState    trState_gg00;
    tgg00_BasisError    trError_gg00;
    tsp00_WarningSet    trWarning_gg00;
    tsp00_Uint2         trObjReqTimeout_gg00;
    tsp00_Int4          trSessionAuxCounter_gg00;
    tsp00_Int4          trTempCount_gg00;
    tsp00_8ByteCounter  trIoCount_gg00;
    tsp00_TaskId        trTaskId_gg00;
    tgg00_TransIndex    trIndex_gg00;
    tsp00_Int2          trBdExclFileLocks_gg00;
    tsp00_8ByteCounter  trRowLocksPerSqlCmd_gg00;
    tsp00_RteCommAddr   trRteCommPtr_gg00;
    tgg00_TempDataCachePtr trBdTcachePtr_gg00;
    tgg00_VoidPtr       trAllocator_gg00;
    tgg00_VoidPtr       trLogContext_gg00;
    tgg00_CurrSeqInfoPtr trSeqInfoPtr_gg00;
    tgg00_VoidPtr       trAcvPtr_gg00;
    tgg00_VoidPtr       trLocalFileDirPtr_gg00;
    tgg00_VoidPtr       trErrorList_gg00;
    tgg00_VoidPtr       trFreePagesForIns_gg00;
    tgg00_VoidPtr       trSchemaShrLockHash_gg00;
    tgg00_VoidPtr       trClusterIOPtr_gg00;
    tsp00_Int4          trClusterIOSize_gg00;
    pasbool             trSyncParallelServers_gg00;
} tgg00_TransContext;
typedef tgg00_TransContext        *tgg00_TransContextPtr;

typedef struct                     tgg00_UnivTrans
{
    union
    {
    /* 1 */
        struct tgg00_UnivTrans_1
        {
            tgg00_TransContext  utrTrans_F;
        } C_1;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tgg00_TransContext &utrTrans               ()  { return this->variant.C_1.utrTrans_F; };
    /* const accessor functions */
    const tgg00_TransContext &utrTrans               () const  { return this->variant.C_1.utrTrans_F; };
#endif    /* defined (__cplusplus) */
} tgg00_UnivTrans;
typedef tgg00_UnivTrans           *tgg00_UnivTransPtr;

typedef struct                     tgg00_ObjTransInfo
{
    tgg91_PageRef       otrBeforeRef_gg00;
    tgg00_TransIndex    otrLockTransIndex_gg00;
    tgg91_TransNo       otrLockTransId_gg00;
    tgg91_TransNo       otrUpdTransId_gg00;
} tgg00_ObjTransInfo;
typedef tgg00_ObjTransInfo        *tgg00_ObjTransInfoPtr;

typedef struct                     tgg00_FilePos
{
    tsp00_PageNo        tpsPno_gg00;
    tsp00_Int4          tpsIndex_gg00;
} tgg00_FilePos;
    /* */

typedef enum tgg00_ObjFileType_Enum
{
    oftUnknownType_egg00,
    oftFixLenObjFile_egg00,
    oftVarLenObjFile_egg00
} tgg00_ObjFileType_Enum;
typedef tgg00_ObjFileType_Enum tgg00_ObjFileType_Param;
typedef ENUM_TYPE_MSP00 (          tgg00_ObjFileType, tgg00_ObjFileType_Enum, oftVarLenObjFile_egg00, unsigned char);

typedef struct                     tgg00_FileId
{
    union
    {
    /* 1 */
        struct tgg00_FileId_1
        {
            tsp00_Int4          fileRootCheck_gg00_F;
            tsp00_Int4          fileLeafNodes_gg00_F;
            tgg00_UserRef       fileUserRef_gg00_F;
            tgg91_FileVersion   fileVersion_gg00_F;
            tsp00_PageNo        fileRoot_gg00_F;
            tgg00_HandlingSet   fileHandling_gg00_F;
            tgg00_BdUseSet      fileBdUse_gg00_F;
            tgg00_FiletypeSet   fileType_gg00_F;
            tgg00_Filename      fileName_gg00_F;
        } C_1;
    /* 2 */
        struct tgg00_FileId_2
        {
            tsp00_C8            fileRootNodes_2_gg00_F;
            tsp00_C4            fileTriggerKey_gg00_F;
            tgg00_ServerdbNo    fileResultSite_gg00_F;
        } C_2;
    /* 3 */
        struct tgg00_FileId_3
        {
            tsp00_C8            fileRootNodes_3_gg00_F;
            tsp00_Int4          fileConstId_gg00_F;
        } C_3;
    /* 4 */
        struct tgg00_FileId_4
        {
            tgg00_FileIdHead    fileHeader__4_gg00_F;
            tgg00_Tfn           fileTfn_gg00_F;
            tsp00_C1            fileTfnNo_gg00_F;
            tgg00_Surrogate     fileTabId_gg00_F;
        } C_4;
    /* 5 */
        struct tgg00_FileId_5
        {
            tgg00_FileIdHead    fileHeader__5_gg00_F;
            tgg00_Tfn           fileTfn______5_gg00_F;
            tgg00_Tfn           fileTfnNo____5_gg00_F;
            tsp00_Uint1         fileContObjFileNo_gg00_F;
            tgg00_ObjFileType   fileObjFileType_gg00_F;
            tsp00_Uint1         fileNumObjKeyFiles_gg00_F;
            tsp00_C3            fileFiller3_gg00_F;
            tgg00_ObjFileNo     fileObjFileNo_gg00_F;
            tsp00_C4            fileFiller4_gg00_F;
        } C_5;
    /* 6 */
        struct tgg00_FileId_6
        {
            tgg00_FileIdHead    fileHeader__6_gg00_F;
            tgg00_Tfn           fileTfn______6_gg00_F;
            tgg00_TfnLog        fileTfnLog_gg00_F;
            tgg91_TransNo       fileTransId_gg00_F;
        } C_6;
    /* 7 */
        struct tgg00_FileId_7
        {
            tgg00_FileIdHead    fileHeader__7_gg00_F;
            tgg00_Tfn           fileTfn______7_gg00_F;
            tsp00_C1            fileTfnNo____7_gg00_F;
            tgg91_TransNo       fileTransId__7_gg00_F;
            tsp00_Int4          fileAuxCnt_gg00_F;
            tgg00_Tfn           fileOldTfn_gg00_F;
            tsp00_C1            fileFiller1_gg00_F;
            tsp00_C2            fileFiller2_gg00_F;
        } C_7;
    /* 8 */
        struct tgg00_FileId_8
        {
            tgg00_FileIdHead    fileHeader__8_gg00_F;
            tgg00_Tfn           fileTfn______8_gg00_F;
            tsp00_C1            fileTfnNo____8_gg00_F;
            tsp00_Int2          fileZeroSite_gg00_F;
            tgg91_SessionNo     fileSession_gg00_F;
            tgg00_TfnTemp       fileTfnTemp_gg00_F;
            tsp00_C1            fileFsNode_gg00_F;
            tsp00_C2            fileLevel_gg00_F;
            tsp00_Int4          fileTempCnt_gg00_F;
        } C_8;
    /* 9 */
        struct tgg00_FileId_9
        {
            tgg00_FileIdHead    fileHeader__9_gg00_F;
            tgg00_Tfn           fileTfn______9_gg00_F;
            tsp00_C1            fileTfnNo____9_gg00_F;
            tsp00_Int2          fileZeroSite_9_gg00_F;
            tgg91_SessionNo     fileSession__9_gg00_F;
            tgg00_TfnTemp       fileTfnTemp__9_gg00_F;
            tsp00_C1            fileFsNode___9_gg00_F;
            tgg91_TransNo       fileTempTrans_gg00_F;
        } C_9;
    /* 10 */
        struct tgg00_FileId_10
        {
            tgg00_FileIdHead    fileHeader_10_gg00_F;
            tgg00_Tfn           fileTfn_____10_gg00_F;
            tsp00_PageNo        fileQueueInvRoot_gg00_F;
        } C_10;
    /* 11 */
        struct tgg00_FileId_11
        {
            tgg00_FileIdHead    fileHeader_11_gg00_F;
            tgg00_Tfn           fileTfn_____11_gg00_F;
            tsp00_C1            fileTfnNo___11_gg00_F;
            tsp00_Uint1         fileObjKeyFileIndex_gg00_F;
            tsp00_C1            fileFiller5_gg00_F;
            tsp00_C8            fileFiller6_gg00_F;
            tsp00_C4            fileObjKeyFileId_gg00_F;
        } C_11;
    /* 12 */
        struct tgg00_FileId_12
        {
            tgg00_FileIdHead    fileHeader__12_gg00_F;
            tgg00_Tfn           fileTfn______12_gg00_F;
            tgg00_Tfn           fileTfnNo____12_gg00_F;
            tsp00_Uint1         fileContObjFileNo_12_gg00_F;
            tgg00_ObjFileType   fileObjFileType_12_gg00_F;
            tsp00_Uint1         fileNumObjKeyFiles_12_gg00_F;
            tsp00_C3            fileFiller3_12_gg00_F;
            tsp00_C8            fileDirFileId_gg00_F;
        } C_12;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tsp00_Int4         &fileRootCheck_gg00     ()  { return this->variant.C_1.fileRootCheck_gg00_F; };
    tsp00_Int4         &fileLeafNodes_gg00     ()  { return this->variant.C_1.fileLeafNodes_gg00_F; };
    tgg00_UserRef      &fileUserRef_gg00       ()  { return this->variant.C_1.fileUserRef_gg00_F; };
    tgg91_FileVersion  &fileVersion_gg00       ()  { return this->variant.C_1.fileVersion_gg00_F; };
    tsp00_PageNo       &fileRoot_gg00          ()  { return this->variant.C_1.fileRoot_gg00_F; };
    tgg00_HandlingSet  &fileHandling_gg00      ()  { return this->variant.C_1.fileHandling_gg00_F; };
    tgg00_BdUseSet     &fileBdUse_gg00         ()  { return this->variant.C_1.fileBdUse_gg00_F; };
    tgg00_FiletypeSet  &fileType_gg00          ()  { return this->variant.C_1.fileType_gg00_F; };
    tgg00_Filename     &fileName_gg00          ()  { return this->variant.C_1.fileName_gg00_F; };
    tsp00_C8           &fileRootNodes_2_gg00   ()  { return this->variant.C_2.fileRootNodes_2_gg00_F; };
    tsp00_C4           &fileTriggerKey_gg00    ()  { return this->variant.C_2.fileTriggerKey_gg00_F; };
    tgg00_ServerdbNo   &fileResultSite_gg00    ()  { return this->variant.C_2.fileResultSite_gg00_F; };
    tsp00_C8           &fileRootNodes_3_gg00   ()  { return this->variant.C_3.fileRootNodes_3_gg00_F; };
    tsp00_Int4         &fileConstId_gg00       ()  { return this->variant.C_3.fileConstId_gg00_F; };
    tgg00_FileIdHead   &fileHeader__4_gg00     ()  { return this->variant.C_4.fileHeader__4_gg00_F; };
    tgg00_Tfn          &fileTfn_gg00           ()  { return this->variant.C_4.fileTfn_gg00_F; };
    tsp00_C1           &fileTfnNo_gg00         ()  { return this->variant.C_4.fileTfnNo_gg00_F; };
    tgg00_Surrogate    &fileTabId_gg00         ()  { return this->variant.C_4.fileTabId_gg00_F; };
    tgg00_FileIdHead   &fileHeader__5_gg00     ()  { return this->variant.C_5.fileHeader__5_gg00_F; };
    tgg00_Tfn          &fileTfn______5_gg00    ()  { return this->variant.C_5.fileTfn______5_gg00_F; };
    tgg00_Tfn          &fileTfnNo____5_gg00    ()  { return this->variant.C_5.fileTfnNo____5_gg00_F; };
    tsp00_Uint1        &fileContObjFileNo_gg00 ()  { return this->variant.C_5.fileContObjFileNo_gg00_F; };
    tgg00_ObjFileType  &fileObjFileType_gg00   ()  { return this->variant.C_5.fileObjFileType_gg00_F; };
    tsp00_Uint1        &fileNumObjKeyFiles_gg00 ()  { return this->variant.C_5.fileNumObjKeyFiles_gg00_F; };
    tsp00_C3           &fileFiller3_gg00       ()  { return this->variant.C_5.fileFiller3_gg00_F; };
    tgg00_ObjFileNo    &fileObjFileNo_gg00     ()  { return this->variant.C_5.fileObjFileNo_gg00_F; };
    tsp00_C4           &fileFiller4_gg00       ()  { return this->variant.C_5.fileFiller4_gg00_F; };
    tgg00_FileIdHead   &fileHeader__6_gg00     ()  { return this->variant.C_6.fileHeader__6_gg00_F; };
    tgg00_Tfn          &fileTfn______6_gg00    ()  { return this->variant.C_6.fileTfn______6_gg00_F; };
    tgg00_TfnLog       &fileTfnLog_gg00        ()  { return this->variant.C_6.fileTfnLog_gg00_F; };
    tgg91_TransNo      &fileTransId_gg00       ()  { return this->variant.C_6.fileTransId_gg00_F; };
    tgg00_FileIdHead   &fileHeader__7_gg00     ()  { return this->variant.C_7.fileHeader__7_gg00_F; };
    tgg00_Tfn          &fileTfn______7_gg00    ()  { return this->variant.C_7.fileTfn______7_gg00_F; };
    tsp00_C1           &fileTfnNo____7_gg00    ()  { return this->variant.C_7.fileTfnNo____7_gg00_F; };
    tgg91_TransNo      &fileTransId__7_gg00    ()  { return this->variant.C_7.fileTransId__7_gg00_F; };
    tsp00_Int4         &fileAuxCnt_gg00        ()  { return this->variant.C_7.fileAuxCnt_gg00_F; };
    tgg00_Tfn          &fileOldTfn_gg00        ()  { return this->variant.C_7.fileOldTfn_gg00_F; };
    tsp00_C1           &fileFiller1_gg00       ()  { return this->variant.C_7.fileFiller1_gg00_F; };
    tsp00_C2           &fileFiller2_gg00       ()  { return this->variant.C_7.fileFiller2_gg00_F; };
    tgg00_FileIdHead   &fileHeader__8_gg00     ()  { return this->variant.C_8.fileHeader__8_gg00_F; };
    tgg00_Tfn          &fileTfn______8_gg00    ()  { return this->variant.C_8.fileTfn______8_gg00_F; };
    tsp00_C1           &fileTfnNo____8_gg00    ()  { return this->variant.C_8.fileTfnNo____8_gg00_F; };
    tsp00_Int2         &fileZeroSite_gg00      ()  { return this->variant.C_8.fileZeroSite_gg00_F; };
    tgg91_SessionNo    &fileSession_gg00       ()  { return this->variant.C_8.fileSession_gg00_F; };
    tgg00_TfnTemp      &fileTfnTemp_gg00       ()  { return this->variant.C_8.fileTfnTemp_gg00_F; };
    tsp00_C1           &fileFsNode_gg00        ()  { return this->variant.C_8.fileFsNode_gg00_F; };
    tsp00_C2           &fileLevel_gg00         ()  { return this->variant.C_8.fileLevel_gg00_F; };
    tsp00_Int4         &fileTempCnt_gg00       ()  { return this->variant.C_8.fileTempCnt_gg00_F; };
    tgg00_FileIdHead   &fileHeader__9_gg00     ()  { return this->variant.C_9.fileHeader__9_gg00_F; };
    tgg00_Tfn          &fileTfn______9_gg00    ()  { return this->variant.C_9.fileTfn______9_gg00_F; };
    tsp00_C1           &fileTfnNo____9_gg00    ()  { return this->variant.C_9.fileTfnNo____9_gg00_F; };
    tsp00_Int2         &fileZeroSite_9_gg00    ()  { return this->variant.C_9.fileZeroSite_9_gg00_F; };
    tgg91_SessionNo    &fileSession__9_gg00    ()  { return this->variant.C_9.fileSession__9_gg00_F; };
    tgg00_TfnTemp      &fileTfnTemp__9_gg00    ()  { return this->variant.C_9.fileTfnTemp__9_gg00_F; };
    tsp00_C1           &fileFsNode___9_gg00    ()  { return this->variant.C_9.fileFsNode___9_gg00_F; };
    tgg91_TransNo      &fileTempTrans_gg00     ()  { return this->variant.C_9.fileTempTrans_gg00_F; };
    tgg00_FileIdHead   &fileHeader_10_gg00     ()  { return this->variant.C_10.fileHeader_10_gg00_F; };
    tgg00_Tfn          &fileTfn_____10_gg00    ()  { return this->variant.C_10.fileTfn_____10_gg00_F; };
    tsp00_PageNo       &fileQueueInvRoot_gg00  ()  { return this->variant.C_10.fileQueueInvRoot_gg00_F; };
    tgg00_FileIdHead   &fileHeader_11_gg00     ()  { return this->variant.C_11.fileHeader_11_gg00_F; };
    tgg00_Tfn          &fileTfn_____11_gg00    ()  { return this->variant.C_11.fileTfn_____11_gg00_F; };
    tsp00_C1           &fileTfnNo___11_gg00    ()  { return this->variant.C_11.fileTfnNo___11_gg00_F; };
    tsp00_Uint1        &fileObjKeyFileIndex_gg00 ()  { return this->variant.C_11.fileObjKeyFileIndex_gg00_F; };
    tsp00_C1           &fileFiller5_gg00       ()  { return this->variant.C_11.fileFiller5_gg00_F; };
    tsp00_C8           &fileFiller6_gg00       ()  { return this->variant.C_11.fileFiller6_gg00_F; };
    tsp00_C4           &fileObjKeyFileId_gg00  ()  { return this->variant.C_11.fileObjKeyFileId_gg00_F; };
    tgg00_FileIdHead   &fileHeader__12_gg00    ()  { return this->variant.C_12.fileHeader__12_gg00_F; };
    tgg00_Tfn          &fileTfn______12_gg00   ()  { return this->variant.C_12.fileTfn______12_gg00_F; };
    tgg00_Tfn          &fileTfnNo____12_gg00   ()  { return this->variant.C_12.fileTfnNo____12_gg00_F; };
    tsp00_Uint1        &fileContObjFileNo_12_gg00 ()  { return this->variant.C_12.fileContObjFileNo_12_gg00_F; };
    tgg00_ObjFileType  &fileObjFileType_12_gg00 ()  { return this->variant.C_12.fileObjFileType_12_gg00_F; };
    tsp00_Uint1        &fileNumObjKeyFiles_12_gg00 ()  { return this->variant.C_12.fileNumObjKeyFiles_12_gg00_F; };
    tsp00_C3           &fileFiller3_12_gg00    ()  { return this->variant.C_12.fileFiller3_12_gg00_F; };
    tsp00_C8           &fileDirFileId_gg00     ()  { return this->variant.C_12.fileDirFileId_gg00_F; };
    /* const accessor functions */
    const tsp00_Int4   &fileRootCheck_gg00     () const  { return this->variant.C_1.fileRootCheck_gg00_F; };
    const tsp00_Int4   &fileLeafNodes_gg00     () const  { return this->variant.C_1.fileLeafNodes_gg00_F; };
    const tgg00_UserRef &fileUserRef_gg00       () const  { return this->variant.C_1.fileUserRef_gg00_F; };
    const tgg91_FileVersion &fileVersion_gg00       () const  { return this->variant.C_1.fileVersion_gg00_F; };
    const tsp00_PageNo &fileRoot_gg00          () const  { return this->variant.C_1.fileRoot_gg00_F; };
    const tgg00_HandlingSet &fileHandling_gg00      () const  { return this->variant.C_1.fileHandling_gg00_F; };
    const tgg00_BdUseSet &fileBdUse_gg00         () const  { return this->variant.C_1.fileBdUse_gg00_F; };
    const tgg00_FiletypeSet &fileType_gg00          () const  { return this->variant.C_1.fileType_gg00_F; };
    const tgg00_Filename &fileName_gg00          () const  { return this->variant.C_1.fileName_gg00_F; };
    const tsp00_C8     &fileRootNodes_2_gg00   () const  { return this->variant.C_2.fileRootNodes_2_gg00_F; };
    const tsp00_C4     &fileTriggerKey_gg00    () const  { return this->variant.C_2.fileTriggerKey_gg00_F; };
    const tgg00_ServerdbNo &fileResultSite_gg00    () const  { return this->variant.C_2.fileResultSite_gg00_F; };
    const tsp00_C8     &fileRootNodes_3_gg00   () const  { return this->variant.C_3.fileRootNodes_3_gg00_F; };
    const tsp00_Int4   &fileConstId_gg00       () const  { return this->variant.C_3.fileConstId_gg00_F; };
    const tgg00_FileIdHead &fileHeader__4_gg00     () const  { return this->variant.C_4.fileHeader__4_gg00_F; };
    const tgg00_Tfn    &fileTfn_gg00           () const  { return this->variant.C_4.fileTfn_gg00_F; };
    const tsp00_C1     &fileTfnNo_gg00         () const  { return this->variant.C_4.fileTfnNo_gg00_F; };
    const tgg00_Surrogate &fileTabId_gg00         () const  { return this->variant.C_4.fileTabId_gg00_F; };
    const tgg00_FileIdHead &fileHeader__5_gg00     () const  { return this->variant.C_5.fileHeader__5_gg00_F; };
    const tgg00_Tfn    &fileTfn______5_gg00    () const  { return this->variant.C_5.fileTfn______5_gg00_F; };
    const tgg00_Tfn    &fileTfnNo____5_gg00    () const  { return this->variant.C_5.fileTfnNo____5_gg00_F; };
    const tsp00_Uint1  &fileContObjFileNo_gg00 () const  { return this->variant.C_5.fileContObjFileNo_gg00_F; };
    const tgg00_ObjFileType &fileObjFileType_gg00   () const  { return this->variant.C_5.fileObjFileType_gg00_F; };
    const tsp00_Uint1  &fileNumObjKeyFiles_gg00 () const  { return this->variant.C_5.fileNumObjKeyFiles_gg00_F; };
    const tsp00_C3     &fileFiller3_gg00       () const  { return this->variant.C_5.fileFiller3_gg00_F; };
    const tgg00_ObjFileNo &fileObjFileNo_gg00     () const  { return this->variant.C_5.fileObjFileNo_gg00_F; };
    const tsp00_C4     &fileFiller4_gg00       () const  { return this->variant.C_5.fileFiller4_gg00_F; };
    const tgg00_FileIdHead &fileHeader__6_gg00     () const  { return this->variant.C_6.fileHeader__6_gg00_F; };
    const tgg00_Tfn    &fileTfn______6_gg00    () const  { return this->variant.C_6.fileTfn______6_gg00_F; };
    const tgg00_TfnLog &fileTfnLog_gg00        () const  { return this->variant.C_6.fileTfnLog_gg00_F; };
    const tgg91_TransNo &fileTransId_gg00       () const  { return this->variant.C_6.fileTransId_gg00_F; };
    const tgg00_FileIdHead &fileHeader__7_gg00     () const  { return this->variant.C_7.fileHeader__7_gg00_F; };
    const tgg00_Tfn    &fileTfn______7_gg00    () const  { return this->variant.C_7.fileTfn______7_gg00_F; };
    const tsp00_C1     &fileTfnNo____7_gg00    () const  { return this->variant.C_7.fileTfnNo____7_gg00_F; };
    const tgg91_TransNo &fileTransId__7_gg00    () const  { return this->variant.C_7.fileTransId__7_gg00_F; };
    const tsp00_Int4   &fileAuxCnt_gg00        () const  { return this->variant.C_7.fileAuxCnt_gg00_F; };
    const tgg00_Tfn    &fileOldTfn_gg00        () const  { return this->variant.C_7.fileOldTfn_gg00_F; };
    const tsp00_C1     &fileFiller1_gg00       () const  { return this->variant.C_7.fileFiller1_gg00_F; };
    const tsp00_C2     &fileFiller2_gg00       () const  { return this->variant.C_7.fileFiller2_gg00_F; };
    const tgg00_FileIdHead &fileHeader__8_gg00     () const  { return this->variant.C_8.fileHeader__8_gg00_F; };
    const tgg00_Tfn    &fileTfn______8_gg00    () const  { return this->variant.C_8.fileTfn______8_gg00_F; };
    const tsp00_C1     &fileTfnNo____8_gg00    () const  { return this->variant.C_8.fileTfnNo____8_gg00_F; };
    const tsp00_Int2   &fileZeroSite_gg00      () const  { return this->variant.C_8.fileZeroSite_gg00_F; };
    const tgg91_SessionNo &fileSession_gg00       () const  { return this->variant.C_8.fileSession_gg00_F; };
    const tgg00_TfnTemp &fileTfnTemp_gg00       () const  { return this->variant.C_8.fileTfnTemp_gg00_F; };
    const tsp00_C1     &fileFsNode_gg00        () const  { return this->variant.C_8.fileFsNode_gg00_F; };
    const tsp00_C2     &fileLevel_gg00         () const  { return this->variant.C_8.fileLevel_gg00_F; };
    const tsp00_Int4   &fileTempCnt_gg00       () const  { return this->variant.C_8.fileTempCnt_gg00_F; };
    const tgg00_FileIdHead &fileHeader__9_gg00     () const  { return this->variant.C_9.fileHeader__9_gg00_F; };
    const tgg00_Tfn    &fileTfn______9_gg00    () const  { return this->variant.C_9.fileTfn______9_gg00_F; };
    const tsp00_C1     &fileTfnNo____9_gg00    () const  { return this->variant.C_9.fileTfnNo____9_gg00_F; };
    const tsp00_Int2   &fileZeroSite_9_gg00    () const  { return this->variant.C_9.fileZeroSite_9_gg00_F; };
    const tgg91_SessionNo &fileSession__9_gg00    () const  { return this->variant.C_9.fileSession__9_gg00_F; };
    const tgg00_TfnTemp &fileTfnTemp__9_gg00    () const  { return this->variant.C_9.fileTfnTemp__9_gg00_F; };
    const tsp00_C1     &fileFsNode___9_gg00    () const  { return this->variant.C_9.fileFsNode___9_gg00_F; };
    const tgg91_TransNo &fileTempTrans_gg00     () const  { return this->variant.C_9.fileTempTrans_gg00_F; };
    const tgg00_FileIdHead &fileHeader_10_gg00     () const  { return this->variant.C_10.fileHeader_10_gg00_F; };
    const tgg00_Tfn    &fileTfn_____10_gg00    () const  { return this->variant.C_10.fileTfn_____10_gg00_F; };
    const tsp00_PageNo &fileQueueInvRoot_gg00  () const  { return this->variant.C_10.fileQueueInvRoot_gg00_F; };
    const tgg00_FileIdHead &fileHeader_11_gg00     () const  { return this->variant.C_11.fileHeader_11_gg00_F; };
    const tgg00_Tfn    &fileTfn_____11_gg00    () const  { return this->variant.C_11.fileTfn_____11_gg00_F; };
    const tsp00_C1     &fileTfnNo___11_gg00    () const  { return this->variant.C_11.fileTfnNo___11_gg00_F; };
    const tsp00_Uint1  &fileObjKeyFileIndex_gg00 () const  { return this->variant.C_11.fileObjKeyFileIndex_gg00_F; };
    const tsp00_C1     &fileFiller5_gg00       () const  { return this->variant.C_11.fileFiller5_gg00_F; };
    const tsp00_C8     &fileFiller6_gg00       () const  { return this->variant.C_11.fileFiller6_gg00_F; };
    const tsp00_C4     &fileObjKeyFileId_gg00  () const  { return this->variant.C_11.fileObjKeyFileId_gg00_F; };
    const tgg00_FileIdHead &fileHeader__12_gg00    () const  { return this->variant.C_12.fileHeader__12_gg00_F; };
    const tgg00_Tfn    &fileTfn______12_gg00   () const  { return this->variant.C_12.fileTfn______12_gg00_F; };
    const tgg00_Tfn    &fileTfnNo____12_gg00   () const  { return this->variant.C_12.fileTfnNo____12_gg00_F; };
    const tsp00_Uint1  &fileContObjFileNo_12_gg00 () const  { return this->variant.C_12.fileContObjFileNo_12_gg00_F; };
    const tgg00_ObjFileType &fileObjFileType_12_gg00 () const  { return this->variant.C_12.fileObjFileType_12_gg00_F; };
    const tsp00_Uint1  &fileNumObjKeyFiles_12_gg00 () const  { return this->variant.C_12.fileNumObjKeyFiles_12_gg00_F; };
    const tsp00_C3     &fileFiller3_12_gg00    () const  { return this->variant.C_12.fileFiller3_12_gg00_F; };
    const tsp00_C8     &fileDirFileId_gg00     () const  { return this->variant.C_12.fileDirFileId_gg00_F; };
#endif    /* defined (__cplusplus) */
} tgg00_FileId;
typedef tgg00_FileId              *tgg00_FileIdPtr;

typedef struct                     tgg00_Lock
{
    tgg00_Surrogate     lockTabId_gg00;
    tgg00_ServerdbNo    lockSite_gg00;
    tgg00_LockReqMode   lockMode_gg00;
    tgg00_LockReqState  lockState_gg00;
    tsp00_Int2          lockKeyLen_gg00;
    tsp00_Int2          lockKeyPos_gg00;
} tgg00_Lock;

typedef struct                     tgg00_TwoFileIds
{
    tgg00_FileId        file_id;
    tgg00_FileId        inv_id;
} tgg00_TwoFileIds;

typedef struct                     tgg00_TwoKeys
{
    tgg00_Lkey          listkey;
    tgg00_Lkey          reckey;
} tgg00_TwoKeys;
typedef tgg00_TwoKeys             *tgg00_TwoKeysPtr;

typedef struct                     tgg00_SysInfoKey
{
    tsp00_Int2          sreclen;
    tsp00_Int2          skeylen;
    tsp00_Int2          svarcoloffset;
    tsp00_Int2          svarcolcnt;
    union
    {
    /* 1 */
        struct tgg00_SysInfoKey_1
        {
            tsp00_C12           sstandardkey_F;
        } C_1;
    /* 2 */
        struct tgg00_SysInfoKey_2
        {
            tgg00_Surrogate     stableid_F;
            tsp00_C2            sentrytyp_F;
            tsp00_C2            slinkage_F;
            tgg00_Surrogate     suserid_F;
            tgg00_Surrogate     sgrantuser_F;
        } C_2;
    /* 3 */
        struct tgg00_SysInfoKey_3
        {
            tgg00_Surrogate     sauthid_F;
            tsp00_C2            sentrytyp1_F;
            tsp00_C2            slinkage1_F;
            tsp00_Identifier    stablen_F;
        } C_3;
    /* 4 */
        struct tgg00_SysInfoKey_4
        {
            tsp00_Int4          stempid_F;
            tsp00_Int4          stemprest_F;
            tsp00_C2            sentrytyp2_F;
            tsp00_Int2          sintlinkage_F;
        } C_4;
    /* 5 */
        struct tgg00_SysInfoKey_5
        {
            tgg00_Surrogate     sauthid1_F;
            tsp00_C2            sentrytyp3_F;
            tsp00_Int2          slinkage3_F;
            tsp00_KnlIdentifier sappl_F;
            tsp00_KnlIdentifier sproc_F;
        } C_5;
    /* 6 */
        struct tgg00_SysInfoKey_6
        {
            tgg00_ServerdbNo    ssite_F;
            tsp00_Int2          sfil_F;
            tsp00_Int4          stemp_F;
        } C_6;
    /* 7 */
        struct tgg00_SysInfoKey_7
        {
            tgg00_ServerdbNo    sserverno_F;
        } C_7;
    /* 8 */
        struct tgg00_SysInfoKey_8
        {
            tgg00_Syskey        all_F;
        } C_8;
    /* 9 */
        struct tgg00_SysInfoKey_9
        {
            tgg00_Surrogate     sresnidfill_F;
            tsp00_C2            sresentrytype_F;
            tsp00_KnlIdentifier srestablen_F;
            tsp00_KnlIdentifier sresmodul_name_F;
        } C_9;
    /* 10 */
        struct tgg00_SysInfoKey_10
        {
            tgg00_Surrogate     szeroid_F;
            tsp00_C2            sentrytype4_F;
            tsp00_C2            slinkage4_F;
            tsp00_Identifier    ssetname_F;
        } C_10;
    /* 11 */
        struct tgg00_SysInfoKey_11
        {
            tsp00_C2            sshowid_F;
            tgg00_ShowKind      sshowkind_F;
        } C_11;
    /* 12 */
        struct tgg00_SysInfoKey_12
        {
            tgg00_Surrogate     szeroid1_F;
            tsp00_C2            sentrytype5_F;
            tsp00_C2            slinkage5_F;
            tsp00_DbName        sdbname_F;
        } C_12;
    /* 13 */
        struct tgg00_SysInfoKey_13
        {
            tgg00_Surrogate     sauthid6_F;
            tsp00_C2            sentrytyp6_F;
            tsp00_C2            slinkage6_F;
            tsp00_KnlIdentifier sidentifier_F;
        } C_13;
    /* 14 */
        struct tgg00_SysInfoKey_14
        {
            tgg00_Surrogate     sauthid7_F;
            tsp00_C2            sentrytyp7_F;
            tsp00_C2            slinkage7_F;
            tsp00_KnlIdentifier sgwowner_F;
            tsp00_KnlIdentifier sgwtablen_F;
        } C_14;
    /* 15 */
        struct tgg00_SysInfoKey_15
        {
            tsp00_C2            sclassflag_F;
            tsp00_C4            sclassid_F;
        } C_15;
    /* 16 */
        struct tgg00_SysInfoKey_16
        {
            tgg00_Surrogate     sschema_id_F;
            tsp00_C2            sentrytype8_F;
            tsp00_C2            slinkage8_F;
            tsp00_C16           sguid_F;
        } C_16;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tsp00_C12          &sstandardkey           ()  { return this->variant.C_1.sstandardkey_F; };
    tgg00_Surrogate    &stableid               ()  { return this->variant.C_2.stableid_F; };
    tsp00_C2           &sentrytyp              ()  { return this->variant.C_2.sentrytyp_F; };
    tsp00_C2           &slinkage               ()  { return this->variant.C_2.slinkage_F; };
    tgg00_Surrogate    &suserid                ()  { return this->variant.C_2.suserid_F; };
    tgg00_Surrogate    &sgrantuser             ()  { return this->variant.C_2.sgrantuser_F; };
    tgg00_Surrogate    &sauthid                ()  { return this->variant.C_3.sauthid_F; };
    tsp00_C2           &sentrytyp1             ()  { return this->variant.C_3.sentrytyp1_F; };
    tsp00_C2           &slinkage1              ()  { return this->variant.C_3.slinkage1_F; };
    tsp00_Identifier   &stablen                ()  { return this->variant.C_3.stablen_F; };
    tsp00_Int4         &stempid                ()  { return this->variant.C_4.stempid_F; };
    tsp00_Int4         &stemprest              ()  { return this->variant.C_4.stemprest_F; };
    tsp00_C2           &sentrytyp2             ()  { return this->variant.C_4.sentrytyp2_F; };
    tsp00_Int2         &sintlinkage            ()  { return this->variant.C_4.sintlinkage_F; };
    tgg00_Surrogate    &sauthid1               ()  { return this->variant.C_5.sauthid1_F; };
    tsp00_C2           &sentrytyp3             ()  { return this->variant.C_5.sentrytyp3_F; };
    tsp00_Int2         &slinkage3              ()  { return this->variant.C_5.slinkage3_F; };
    tsp00_KnlIdentifier &sappl                  ()  { return this->variant.C_5.sappl_F; };
    tsp00_KnlIdentifier &sproc                  ()  { return this->variant.C_5.sproc_F; };
    tgg00_ServerdbNo   &ssite                  ()  { return this->variant.C_6.ssite_F; };
    tsp00_Int2         &sfil                   ()  { return this->variant.C_6.sfil_F; };
    tsp00_Int4         &stemp                  ()  { return this->variant.C_6.stemp_F; };
    tgg00_ServerdbNo   &sserverno              ()  { return this->variant.C_7.sserverno_F; };
    tgg00_Syskey       &all                    ()  { return this->variant.C_8.all_F; };
    tgg00_Surrogate    &sresnidfill            ()  { return this->variant.C_9.sresnidfill_F; };
    tsp00_C2           &sresentrytype          ()  { return this->variant.C_9.sresentrytype_F; };
    tsp00_KnlIdentifier &srestablen             ()  { return this->variant.C_9.srestablen_F; };
    tsp00_KnlIdentifier &sresmodul_name         ()  { return this->variant.C_9.sresmodul_name_F; };
    tgg00_Surrogate    &szeroid                ()  { return this->variant.C_10.szeroid_F; };
    tsp00_C2           &sentrytype4            ()  { return this->variant.C_10.sentrytype4_F; };
    tsp00_C2           &slinkage4              ()  { return this->variant.C_10.slinkage4_F; };
    tsp00_Identifier   &ssetname               ()  { return this->variant.C_10.ssetname_F; };
    tsp00_C2           &sshowid                ()  { return this->variant.C_11.sshowid_F; };
    tgg00_ShowKind     &sshowkind              ()  { return this->variant.C_11.sshowkind_F; };
    tgg00_Surrogate    &szeroid1               ()  { return this->variant.C_12.szeroid1_F; };
    tsp00_C2           &sentrytype5            ()  { return this->variant.C_12.sentrytype5_F; };
    tsp00_C2           &slinkage5              ()  { return this->variant.C_12.slinkage5_F; };
    tsp00_DbName       &sdbname                ()  { return this->variant.C_12.sdbname_F; };
    tgg00_Surrogate    &sauthid6               ()  { return this->variant.C_13.sauthid6_F; };
    tsp00_C2           &sentrytyp6             ()  { return this->variant.C_13.sentrytyp6_F; };
    tsp00_C2           &slinkage6              ()  { return this->variant.C_13.slinkage6_F; };
    tsp00_KnlIdentifier &sidentifier            ()  { return this->variant.C_13.sidentifier_F; };
    tgg00_Surrogate    &sauthid7               ()  { return this->variant.C_14.sauthid7_F; };
    tsp00_C2           &sentrytyp7             ()  { return this->variant.C_14.sentrytyp7_F; };
    tsp00_C2           &slinkage7              ()  { return this->variant.C_14.slinkage7_F; };
    tsp00_KnlIdentifier &sgwowner               ()  { return this->variant.C_14.sgwowner_F; };
    tsp00_KnlIdentifier &sgwtablen              ()  { return this->variant.C_14.sgwtablen_F; };
    tsp00_C2           &sclassflag             ()  { return this->variant.C_15.sclassflag_F; };
    tsp00_C4           &sclassid               ()  { return this->variant.C_15.sclassid_F; };
    tgg00_Surrogate    &sschema_id             ()  { return this->variant.C_16.sschema_id_F; };
    tsp00_C2           &sentrytype8            ()  { return this->variant.C_16.sentrytype8_F; };
    tsp00_C2           &slinkage8              ()  { return this->variant.C_16.slinkage8_F; };
    tsp00_C16          &sguid                  ()  { return this->variant.C_16.sguid_F; };
    /* const accessor functions */
    const tsp00_C12    &sstandardkey           () const  { return this->variant.C_1.sstandardkey_F; };
    const tgg00_Surrogate &stableid               () const  { return this->variant.C_2.stableid_F; };
    const tsp00_C2     &sentrytyp              () const  { return this->variant.C_2.sentrytyp_F; };
    const tsp00_C2     &slinkage               () const  { return this->variant.C_2.slinkage_F; };
    const tgg00_Surrogate &suserid                () const  { return this->variant.C_2.suserid_F; };
    const tgg00_Surrogate &sgrantuser             () const  { return this->variant.C_2.sgrantuser_F; };
    const tgg00_Surrogate &sauthid                () const  { return this->variant.C_3.sauthid_F; };
    const tsp00_C2     &sentrytyp1             () const  { return this->variant.C_3.sentrytyp1_F; };
    const tsp00_C2     &slinkage1              () const  { return this->variant.C_3.slinkage1_F; };
    const tsp00_Identifier &stablen                () const  { return this->variant.C_3.stablen_F; };
    const tsp00_Int4   &stempid                () const  { return this->variant.C_4.stempid_F; };
    const tsp00_Int4   &stemprest              () const  { return this->variant.C_4.stemprest_F; };
    const tsp00_C2     &sentrytyp2             () const  { return this->variant.C_4.sentrytyp2_F; };
    const tsp00_Int2   &sintlinkage            () const  { return this->variant.C_4.sintlinkage_F; };
    const tgg00_Surrogate &sauthid1               () const  { return this->variant.C_5.sauthid1_F; };
    const tsp00_C2     &sentrytyp3             () const  { return this->variant.C_5.sentrytyp3_F; };
    const tsp00_Int2   &slinkage3              () const  { return this->variant.C_5.slinkage3_F; };
    const tsp00_KnlIdentifier &sappl                  () const  { return this->variant.C_5.sappl_F; };
    const tsp00_KnlIdentifier &sproc                  () const  { return this->variant.C_5.sproc_F; };
    const tgg00_ServerdbNo &ssite                  () const  { return this->variant.C_6.ssite_F; };
    const tsp00_Int2   &sfil                   () const  { return this->variant.C_6.sfil_F; };
    const tsp00_Int4   &stemp                  () const  { return this->variant.C_6.stemp_F; };
    const tgg00_ServerdbNo &sserverno              () const  { return this->variant.C_7.sserverno_F; };
    const tgg00_Syskey &all                    () const  { return this->variant.C_8.all_F; };
    const tgg00_Surrogate &sresnidfill            () const  { return this->variant.C_9.sresnidfill_F; };
    const tsp00_C2     &sresentrytype          () const  { return this->variant.C_9.sresentrytype_F; };
    const tsp00_KnlIdentifier &srestablen             () const  { return this->variant.C_9.srestablen_F; };
    const tsp00_KnlIdentifier &sresmodul_name         () const  { return this->variant.C_9.sresmodul_name_F; };
    const tgg00_Surrogate &szeroid                () const  { return this->variant.C_10.szeroid_F; };
    const tsp00_C2     &sentrytype4            () const  { return this->variant.C_10.sentrytype4_F; };
    const tsp00_C2     &slinkage4              () const  { return this->variant.C_10.slinkage4_F; };
    const tsp00_Identifier &ssetname               () const  { return this->variant.C_10.ssetname_F; };
    const tsp00_C2     &sshowid                () const  { return this->variant.C_11.sshowid_F; };
    const tgg00_ShowKind &sshowkind              () const  { return this->variant.C_11.sshowkind_F; };
    const tgg00_Surrogate &szeroid1               () const  { return this->variant.C_12.szeroid1_F; };
    const tsp00_C2     &sentrytype5            () const  { return this->variant.C_12.sentrytype5_F; };
    const tsp00_C2     &slinkage5              () const  { return this->variant.C_12.slinkage5_F; };
    const tsp00_DbName &sdbname                () const  { return this->variant.C_12.sdbname_F; };
    const tgg00_Surrogate &sauthid6               () const  { return this->variant.C_13.sauthid6_F; };
    const tsp00_C2     &sentrytyp6             () const  { return this->variant.C_13.sentrytyp6_F; };
    const tsp00_C2     &slinkage6              () const  { return this->variant.C_13.slinkage6_F; };
    const tsp00_KnlIdentifier &sidentifier            () const  { return this->variant.C_13.sidentifier_F; };
    const tgg00_Surrogate &sauthid7               () const  { return this->variant.C_14.sauthid7_F; };
    const tsp00_C2     &sentrytyp7             () const  { return this->variant.C_14.sentrytyp7_F; };
    const tsp00_C2     &slinkage7              () const  { return this->variant.C_14.slinkage7_F; };
    const tsp00_KnlIdentifier &sgwowner               () const  { return this->variant.C_14.sgwowner_F; };
    const tsp00_KnlIdentifier &sgwtablen              () const  { return this->variant.C_14.sgwtablen_F; };
    const tsp00_C2     &sclassflag             () const  { return this->variant.C_15.sclassflag_F; };
    const tsp00_C4     &sclassid               () const  { return this->variant.C_15.sclassid_F; };
    const tgg00_Surrogate &sschema_id             () const  { return this->variant.C_16.sschema_id_F; };
    const tsp00_C2     &sentrytype8            () const  { return this->variant.C_16.sentrytype8_F; };
    const tsp00_C2     &slinkage8              () const  { return this->variant.C_16.slinkage8_F; };
    const tsp00_C16    &sguid                  () const  { return this->variant.C_16.sguid_F; };
#endif    /* defined (__cplusplus) */
} tgg00_SysInfoKey;

typedef struct                     tgg00_SeqInfo
{
    tsp00_C20           seq_increment;
    tsp00_C20           seq_minvalue;
    tsp00_C20           seq_maxvalue;
    tsp00_C20           seq_cachevalue;
    tgg00_ServerdbNo    seq_site;
    pasbool             seq_cycle;
    pasbool             seq_first;
    tsp00_Int4          seq_filler;
} tgg00_SeqInfo;

typedef struct                     tgg00_Info
{
    union
    {
    /* 1 */
        struct tgg00_Info_1
        {
            tgg00_SysInfoKey    recSysKey_gg00_F;
        } C_1;
    /* 2 */
        struct tgg00_Info_2
        {
            tsp00_C8            recSpace1_gg00_F;
            tsp00_C1            recInsSelInfo_gg00_F;
            pasbool             recIntoTemp_gg00_F;
            tgg00_Surrogate     recTempKey_gg00_F;
        } C_2;
    /* 11 */
        struct tgg00_Info_11
        {
            tgg00_SysInfoKey    basesyskey_F;
        } C_11;
    /* 21 */
        struct tgg00_Info_21
        {
            tsp00_Int2          rb_len_F;
            tsp00_Int2          rb_keylen_F;
            tsp00_Int2          rb_varcol_offset_F;
            tsp00_Int2          rb_varcol_cnt_F;
            tsp00_C1            rb_ins_sel_info_F;
            pasbool             rb_into_temp_F;
            tgg00_Surrogate     rb_tempkey_F;
        } C_21;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tgg00_SysInfoKey   &recSysKey_gg00         ()  { return this->variant.C_1.recSysKey_gg00_F; };
    tsp00_C8           &recSpace1_gg00         ()  { return this->variant.C_2.recSpace1_gg00_F; };
    tsp00_C1           &recInsSelInfo_gg00     ()  { return this->variant.C_2.recInsSelInfo_gg00_F; };
    pasbool            &recIntoTemp_gg00       ()  { return this->variant.C_2.recIntoTemp_gg00_F; };
    tgg00_Surrogate    &recTempKey_gg00        ()  { return this->variant.C_2.recTempKey_gg00_F; };
    tgg00_SysInfoKey   &basesyskey             ()  { return this->variant.C_11.basesyskey_F; };
    tsp00_Int2         &rb_len                 ()  { return this->variant.C_21.rb_len_F; };
    tsp00_Int2         &rb_keylen              ()  { return this->variant.C_21.rb_keylen_F; };
    tsp00_Int2         &rb_varcol_offset       ()  { return this->variant.C_21.rb_varcol_offset_F; };
    tsp00_Int2         &rb_varcol_cnt          ()  { return this->variant.C_21.rb_varcol_cnt_F; };
    tsp00_C1           &rb_ins_sel_info        ()  { return this->variant.C_21.rb_ins_sel_info_F; };
    pasbool            &rb_into_temp           ()  { return this->variant.C_21.rb_into_temp_F; };
    tgg00_Surrogate    &rb_tempkey             ()  { return this->variant.C_21.rb_tempkey_F; };
    /* const accessor functions */
    const tgg00_SysInfoKey &recSysKey_gg00         () const  { return this->variant.C_1.recSysKey_gg00_F; };
    const tsp00_C8     &recSpace1_gg00         () const  { return this->variant.C_2.recSpace1_gg00_F; };
    const tsp00_C1     &recInsSelInfo_gg00     () const  { return this->variant.C_2.recInsSelInfo_gg00_F; };
    const pasbool      &recIntoTemp_gg00       () const  { return this->variant.C_2.recIntoTemp_gg00_F; };
    const tgg00_Surrogate &recTempKey_gg00        () const  { return this->variant.C_2.recTempKey_gg00_F; };
    const tgg00_SysInfoKey &basesyskey             () const  { return this->variant.C_11.basesyskey_F; };
    const tsp00_Int2   &rb_len                 () const  { return this->variant.C_21.rb_len_F; };
    const tsp00_Int2   &rb_keylen              () const  { return this->variant.C_21.rb_keylen_F; };
    const tsp00_Int2   &rb_varcol_offset       () const  { return this->variant.C_21.rb_varcol_offset_F; };
    const tsp00_Int2   &rb_varcol_cnt          () const  { return this->variant.C_21.rb_varcol_cnt_F; };
    const tsp00_C1     &rb_ins_sel_info        () const  { return this->variant.C_21.rb_ins_sel_info_F; };
    const pasbool      &rb_into_temp           () const  { return this->variant.C_21.rb_into_temp_F; };
    const tgg00_Surrogate &rb_tempkey             () const  { return this->variant.C_21.rb_tempkey_F; };
#endif    /* defined (__cplusplus) */
} tgg00_Info;

typedef struct                     tgg00_HeaderRec
{
    union
    {
    /* 1 */
        struct tgg00_HeaderRec_1
        {
            tsp00_Int2          hrecLen_gg00_F;
            tsp00_Int2          hrecKeyLen_gg00_F;
            tsp00_Int2          hrecVarcolOffset_gg00_F;
            tsp00_Int2          hrecVarcolCnt_gg00_F;
        } C_1;
    /* 2 */
        struct tgg00_HeaderRec_2
        {
            tsp00_C2            hrecLenC2_gg00_F;
            tsp00_C2            hrecKeyLenC2_gg00_F;
            tsp00_C2            hrecVarcolOffC2_gg00_F;
            tsp00_C2            hrecVarcolCntC2_gg00_F;
        } C_2;
    /* 3 */
        struct tgg00_HeaderRec_3
        {
            tsp00_C2            hrecLenSpace_gg00_F;
            tsp00_C2            hrecKeyLenSpace_gg00_F;
            tgg90_Cint4         hrecPno_gg00_F;
        } C_3;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tsp00_Int2         &hrecLen_gg00           ()  { return this->variant.C_1.hrecLen_gg00_F; };
    tsp00_Int2         &hrecKeyLen_gg00        ()  { return this->variant.C_1.hrecKeyLen_gg00_F; };
    tsp00_Int2         &hrecVarcolOffset_gg00  ()  { return this->variant.C_1.hrecVarcolOffset_gg00_F; };
    tsp00_Int2         &hrecVarcolCnt_gg00     ()  { return this->variant.C_1.hrecVarcolCnt_gg00_F; };
    tsp00_C2           &hrecLenC2_gg00         ()  { return this->variant.C_2.hrecLenC2_gg00_F; };
    tsp00_C2           &hrecKeyLenC2_gg00      ()  { return this->variant.C_2.hrecKeyLenC2_gg00_F; };
    tsp00_C2           &hrecVarcolOffC2_gg00   ()  { return this->variant.C_2.hrecVarcolOffC2_gg00_F; };
    tsp00_C2           &hrecVarcolCntC2_gg00   ()  { return this->variant.C_2.hrecVarcolCntC2_gg00_F; };
    tsp00_C2           &hrecLenSpace_gg00      ()  { return this->variant.C_3.hrecLenSpace_gg00_F; };
    tsp00_C2           &hrecKeyLenSpace_gg00   ()  { return this->variant.C_3.hrecKeyLenSpace_gg00_F; };
    tgg90_Cint4        &hrecPno_gg00           ()  { return this->variant.C_3.hrecPno_gg00_F; };
    /* const accessor functions */
    const tsp00_Int2   &hrecLen_gg00           () const  { return this->variant.C_1.hrecLen_gg00_F; };
    const tsp00_Int2   &hrecKeyLen_gg00        () const  { return this->variant.C_1.hrecKeyLen_gg00_F; };
    const tsp00_Int2   &hrecVarcolOffset_gg00  () const  { return this->variant.C_1.hrecVarcolOffset_gg00_F; };
    const tsp00_Int2   &hrecVarcolCnt_gg00     () const  { return this->variant.C_1.hrecVarcolCnt_gg00_F; };
    const tsp00_C2     &hrecLenC2_gg00         () const  { return this->variant.C_2.hrecLenC2_gg00_F; };
    const tsp00_C2     &hrecKeyLenC2_gg00      () const  { return this->variant.C_2.hrecKeyLenC2_gg00_F; };
    const tsp00_C2     &hrecVarcolOffC2_gg00   () const  { return this->variant.C_2.hrecVarcolOffC2_gg00_F; };
    const tsp00_C2     &hrecVarcolCntC2_gg00   () const  { return this->variant.C_2.hrecVarcolCntC2_gg00_F; };
    const tsp00_C2     &hrecLenSpace_gg00      () const  { return this->variant.C_3.hrecLenSpace_gg00_F; };
    const tsp00_C2     &hrecKeyLenSpace_gg00   () const  { return this->variant.C_3.hrecKeyLenSpace_gg00_F; };
    const tgg90_Cint4  &hrecPno_gg00           () const  { return this->variant.C_3.hrecPno_gg00_F; };
#endif    /* defined (__cplusplus) */
} tgg00_HeaderRec;

typedef struct                     tgg00_Rec
{
    union
    {
    /* 1 */
        struct tgg00_Rec_1
        {
            tsp00_Buf           recBuf_gg00_F;
        } C_1;
    /* 2 */
        struct tgg00_Rec_2
        {
            tsp00_Int2          recLen_gg00_F;
            tsp00_Int2          recKeyLen_gg00_F;
            tsp00_Int2          recVarcolOffset_gg00_F;
            tsp00_Int2          recVarcolCnt_gg00_F;
            tgg00_RecBody       recBody_gg00_F;
        } C_2;
    /* 3 */
        struct tgg00_Rec_3
        {
            tgg00_Lkey          recKey_gg00_F;
        } C_3;
    /* 4 */
        struct tgg00_Rec_4
        {
            ARRAY_VAR_MSP00 (         recPart_gg00_F, tgg00_RecPart, MAX_RECPARTS_GG00 - 0 + 1);
        } C_4;
    /* 5 */
        struct tgg00_Rec_5
        {
            tgg00_HeaderRec     recHead_gg00_F;
        } C_5;
    /* 6 */
        struct tgg00_Rec_6
        {
            tsp00_C2            recLenC2_gg00_F;
            tsp00_C2            recKeyLenC2_gg00_F;
            tsp00_C2            recVarcolOffC2_gg00_F;
            tsp00_C2            recVarcolCntC2_gg00_F;
        } C_6;
    /* 7 */
        struct tgg00_Rec_7
        {
            tsp00_C2            recLenSpace1_gg00_F;
            tsp00_C2            recKeyLenSpace1_gg00_F;
            tgg90_Cint4         recPno_gg00_F;
        } C_7;
    /* 8 */
        struct tgg00_Rec_8
        {
            tsp00_C2            recLenSpace2_gg00_F;
            tsp00_C2            recKeyLenSpace2_gg00_F;
            tgg00_InvListKind   recInvListKind_gg00_F;
            tsp00_Uint1         recAlignment_gg00_F;
            tsp00_Uint2         recNumPrimKeys_gg00_F;
        } C_8;
    /* 11 */
        struct tgg00_Rec_11
        {
            tsp00_Buf           buf_F;
        } C_11;
    /* 12 */
        struct tgg00_Rec_12
        {
            tsp00_Int2          len_F;
            tsp00_Int2          keylen_F;
            tsp00_Int2          space_var_offset_F;
            tsp00_Int2          space_var_cnt_F;
            tgg00_RecBody       info_F;
        } C_12;
    /* 13 */
        struct tgg00_Rec_13
        {
            tgg00_Lkey          mkey_F;
        } C_13;
    /* 14 */
        struct tgg00_Rec_14
        {
            ARRAY_VAR_MSP00 (         recpart_F, tgg00_RecPart, MAX_RECPARTS_GG00 - 0 + 1);
        } C_14;
    /* 15 */
        struct tgg00_Rec_15
        {
            OLDPASCALSTRING_VAR_MSP00 (  recHeader_gg00_F, RSN_RECHEAD_MXGG00);
            ARRAY_VAR_MSP00 (         columnoffset_gg00_F, tsp00_Int2, MAX_COLUMNOFFSETS);
        } C_15;
    /* 16 */
        struct tgg00_Rec_16
        {
            OLDPASCALSTRING_VAR_MSP00 (  recHeader2_gg00_F, RSN_RECHEAD_MXGG00);
            OLDPASCALSTRING_VAR_MSP00 (  columnbuf_gg00_F, RECCOLUMNS_MXGG00);
        } C_16;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tsp00_Buf          &recBuf_gg00            ()  { return this->variant.C_1.recBuf_gg00_F; };
    tsp00_Int2         &recLen_gg00            ()  { return this->variant.C_2.recLen_gg00_F; };
    tsp00_Int2         &recKeyLen_gg00         ()  { return this->variant.C_2.recKeyLen_gg00_F; };
    tsp00_Int2         &recVarcolOffset_gg00   ()  { return this->variant.C_2.recVarcolOffset_gg00_F; };
    tsp00_Int2         &recVarcolCnt_gg00      ()  { return this->variant.C_2.recVarcolCnt_gg00_F; };
    tgg00_RecBody      &recBody_gg00           ()  { return this->variant.C_2.recBody_gg00_F; };
    tgg00_Lkey         &recKey_gg00            ()  { return this->variant.C_3.recKey_gg00_F; };
    ARRAY_TYPESTRING_MSP00(tgg00_RecPart, MAX_RECPARTS_GG00 - 0 + 1) &recPart_gg00           ()  { return this->variant.C_4.recPart_gg00_F; };
    tgg00_HeaderRec    &recHead_gg00           ()  { return this->variant.C_5.recHead_gg00_F; };
    tsp00_C2           &recLenC2_gg00          ()  { return this->variant.C_6.recLenC2_gg00_F; };
    tsp00_C2           &recKeyLenC2_gg00       ()  { return this->variant.C_6.recKeyLenC2_gg00_F; };
    tsp00_C2           &recVarcolOffC2_gg00    ()  { return this->variant.C_6.recVarcolOffC2_gg00_F; };
    tsp00_C2           &recVarcolCntC2_gg00    ()  { return this->variant.C_6.recVarcolCntC2_gg00_F; };
    tsp00_C2           &recLenSpace1_gg00      ()  { return this->variant.C_7.recLenSpace1_gg00_F; };
    tsp00_C2           &recKeyLenSpace1_gg00   ()  { return this->variant.C_7.recKeyLenSpace1_gg00_F; };
    tgg90_Cint4        &recPno_gg00            ()  { return this->variant.C_7.recPno_gg00_F; };
    tsp00_C2           &recLenSpace2_gg00      ()  { return this->variant.C_8.recLenSpace2_gg00_F; };
    tsp00_C2           &recKeyLenSpace2_gg00   ()  { return this->variant.C_8.recKeyLenSpace2_gg00_F; };
    tgg00_InvListKind  &recInvListKind_gg00    ()  { return this->variant.C_8.recInvListKind_gg00_F; };
    tsp00_Uint1        &recAlignment_gg00      ()  { return this->variant.C_8.recAlignment_gg00_F; };
    tsp00_Uint2        &recNumPrimKeys_gg00    ()  { return this->variant.C_8.recNumPrimKeys_gg00_F; };
    tsp00_Buf          &buf                    ()  { return this->variant.C_11.buf_F; };
    tsp00_Int2         &len                    ()  { return this->variant.C_12.len_F; };
    tsp00_Int2         &keylen                 ()  { return this->variant.C_12.keylen_F; };
    tsp00_Int2         &space_var_offset       ()  { return this->variant.C_12.space_var_offset_F; };
    tsp00_Int2         &space_var_cnt          ()  { return this->variant.C_12.space_var_cnt_F; };
    tgg00_RecBody      &info                   ()  { return this->variant.C_12.info_F; };
    tgg00_Lkey         &mkey                   ()  { return this->variant.C_13.mkey_F; };
    ARRAY_TYPESTRING_MSP00(tgg00_RecPart, MAX_RECPARTS_GG00 - 0 + 1) &recpart                ()  { return this->variant.C_14.recpart_F; };
    tsp00_OldPascalString<RSN_RECHEAD_MXGG00> &recHeader_gg00         ()  { return this->variant.C_15.recHeader_gg00_F; };
    ARRAY_TYPESTRING_MSP00(tsp00_Int2, MAX_COLUMNOFFSETS) &columnoffset_gg00      ()  { return this->variant.C_15.columnoffset_gg00_F; };
    tsp00_OldPascalString<RSN_RECHEAD_MXGG00> &recHeader2_gg00        ()  { return this->variant.C_16.recHeader2_gg00_F; };
    tsp00_OldPascalString<RECCOLUMNS_MXGG00> &columnbuf_gg00         ()  { return this->variant.C_16.columnbuf_gg00_F; };
    /* const accessor functions */
    const tsp00_Buf    &recBuf_gg00            () const  { return this->variant.C_1.recBuf_gg00_F; };
    const tsp00_Int2   &recLen_gg00            () const  { return this->variant.C_2.recLen_gg00_F; };
    const tsp00_Int2   &recKeyLen_gg00         () const  { return this->variant.C_2.recKeyLen_gg00_F; };
    const tsp00_Int2   &recVarcolOffset_gg00   () const  { return this->variant.C_2.recVarcolOffset_gg00_F; };
    const tsp00_Int2   &recVarcolCnt_gg00      () const  { return this->variant.C_2.recVarcolCnt_gg00_F; };
    const tgg00_RecBody &recBody_gg00           () const  { return this->variant.C_2.recBody_gg00_F; };
    const tgg00_Lkey   &recKey_gg00            () const  { return this->variant.C_3.recKey_gg00_F; };
    const ARRAY_TYPESTRING_MSP00(tgg00_RecPart, MAX_RECPARTS_GG00 - 0 + 1) &recPart_gg00           () const  { return this->variant.C_4.recPart_gg00_F; };
    const tgg00_HeaderRec &recHead_gg00           () const  { return this->variant.C_5.recHead_gg00_F; };
    const tsp00_C2     &recLenC2_gg00          () const  { return this->variant.C_6.recLenC2_gg00_F; };
    const tsp00_C2     &recKeyLenC2_gg00       () const  { return this->variant.C_6.recKeyLenC2_gg00_F; };
    const tsp00_C2     &recVarcolOffC2_gg00    () const  { return this->variant.C_6.recVarcolOffC2_gg00_F; };
    const tsp00_C2     &recVarcolCntC2_gg00    () const  { return this->variant.C_6.recVarcolCntC2_gg00_F; };
    const tsp00_C2     &recLenSpace1_gg00      () const  { return this->variant.C_7.recLenSpace1_gg00_F; };
    const tsp00_C2     &recKeyLenSpace1_gg00   () const  { return this->variant.C_7.recKeyLenSpace1_gg00_F; };
    const tgg90_Cint4  &recPno_gg00            () const  { return this->variant.C_7.recPno_gg00_F; };
    const tsp00_C2     &recLenSpace2_gg00      () const  { return this->variant.C_8.recLenSpace2_gg00_F; };
    const tsp00_C2     &recKeyLenSpace2_gg00   () const  { return this->variant.C_8.recKeyLenSpace2_gg00_F; };
    const tgg00_InvListKind &recInvListKind_gg00    () const  { return this->variant.C_8.recInvListKind_gg00_F; };
    const tsp00_Uint1  &recAlignment_gg00      () const  { return this->variant.C_8.recAlignment_gg00_F; };
    const tsp00_Uint2  &recNumPrimKeys_gg00    () const  { return this->variant.C_8.recNumPrimKeys_gg00_F; };
    const tsp00_Buf    &buf                    () const  { return this->variant.C_11.buf_F; };
    const tsp00_Int2   &len                    () const  { return this->variant.C_12.len_F; };
    const tsp00_Int2   &keylen                 () const  { return this->variant.C_12.keylen_F; };
    const tsp00_Int2   &space_var_offset       () const  { return this->variant.C_12.space_var_offset_F; };
    const tsp00_Int2   &space_var_cnt          () const  { return this->variant.C_12.space_var_cnt_F; };
    const tgg00_RecBody &info                   () const  { return this->variant.C_12.info_F; };
    const tgg00_Lkey   &mkey                   () const  { return this->variant.C_13.mkey_F; };
    const ARRAY_TYPESTRING_MSP00(tgg00_RecPart, MAX_RECPARTS_GG00 - 0 + 1) &recpart                () const  { return this->variant.C_14.recpart_F; };
    const tsp00_OldPascalString<RSN_RECHEAD_MXGG00> &recHeader_gg00         () const  { return this->variant.C_15.recHeader_gg00_F; };
    const ARRAY_TYPESTRING_MSP00(tsp00_Int2, MAX_COLUMNOFFSETS) &columnoffset_gg00      () const  { return this->variant.C_15.columnoffset_gg00_F; };
    const tsp00_OldPascalString<RSN_RECHEAD_MXGG00> &recHeader2_gg00        () const  { return this->variant.C_16.recHeader2_gg00_F; };
    const tsp00_OldPascalString<RECCOLUMNS_MXGG00> &columnbuf_gg00         () const  { return this->variant.C_16.columnbuf_gg00_F; };
#endif    /* defined (__cplusplus) */
} tgg00_Rec;
typedef tgg00_Rec                 *tgg00_RecPtr;

typedef struct                     tgg00_BufLenInfo
{
    union
    {
    /* true */
        struct tgg00_BufLenInfo_true
        {
            tgg00_Rec           rb_F;
        } C_true;
    /* false */
        struct tgg00_BufLenInfo_false
        {
            tgg00_Info          ri_F;
        } C_false;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tgg00_Rec          &rb                     ()  { return this->variant.C_true.rb_F; };
    tgg00_Info         &ri                     ()  { return this->variant.C_false.ri_F; };
    /* const accessor functions */
    const tgg00_Rec    &rb                     () const  { return this->variant.C_true.rb_F; };
    const tgg00_Info   &ri                     () const  { return this->variant.C_false.ri_F; };
#endif    /* defined (__cplusplus) */
} tgg00_BufLenInfo;
typedef SET_TYPE_MSP00 (           tgg00_ColumnSet, int, MAX_COL_PER_TAB_GG00, 128);
typedef ARRAY_TYPE_MSP00 (         tgg00_ColumnDistinctValues, tsp00_Int4, MAX_COL_PER_TAB_GG00);

typedef struct                     tgg00_TreeStatInfo
{
    tgg00_FileId        TreeStatTreeId_gg00;
    pasbool             TreeStatCalculate_gg00;
    pasbool             TreeStatFill1_gg00;
    tsp00_Int2          TreeStatFill2_gg00;
    tsp00_Int4          TreeStatNoSampleLeaves_gg00;
    tsp00_Int4          TreeStatMaxNumberOfLeaves_gg00;
    tsp00_Int4          TreeStatNumberOfLeaves_gg00;
    tsp00_Int4          TreeStatNumberOfRecords_gg00;
} tgg00_TreeStatInfo;
typedef tgg00_TreeStatInfo        *tgg00_TreeStatInfoPtr;

typedef struct                     tgg00_ColStatInfo
{
    tgg00_FileId        ColStatTreeId_gg00;
    pasbool             ColStatCalculate_gg00;
    pasbool             ColStatFill1_gg00;
    tsp00_Int2          ColStatFill2_gg00;
    tsp00_Int4          ColStatNoSampleLeaves_gg00;
    tsp00_Int4          ColStatNumberOfLeaves_gg00;
    tsp00_Int4          ColStatNumberOfRecords_gg00;
    tgg00_ColumnDistinctValues ColStatDistValues_gg00;
} tgg00_ColStatInfo;
typedef tgg00_ColStatInfo         *tgg00_ColStatInfoPtr;

typedef struct                     tgg00_TableStatInfo
{
    tsp00_Int4          TabStatNodes_gg00;
    tsp00_Int4          TabStatIndexNodes_gg00;
    tsp00_Int4          TabStatLeafnodes_gg00;
    tsp00_Int4          TabStatTreeCov_gg00;
    tsp00_Int4          TabStatRootCov_gg00;
    tsp00_Int4          TabStatIndexCov_gg00;
    tsp00_Int4          TabStatMinIndexCov_gg00;
    tsp00_Int4          TabStatMaxIndexCov_gg00;
    tsp00_Int4          TabStatLeafCov_gg00;
    tsp00_Int4          TabStatMinLeafCov_gg00;
    tsp00_Int4          TabStatMaxLeafCov_gg00;
    tsp00_Int4          TabStatRecords_gg00;
    tsp00_Int4          TabStatRecsPerPage_gg00;
    tsp00_Int4          TabStatMinRecsPerPage_gg00;
    tsp00_Int4          TabStatMaxRecsPerPage_gg00;
    tsp00_Int4          TabStatAveRecLength_gg00;
    tsp00_Int4          TabStatMinRecLength_gg00;
    tsp00_Int4          TabStatMaxRecLength_gg00;
    tsp00_Int4          TabStatDefinedStringCols_gg00;
    tsp00_Int4          TabStatAveLenStringCol_gg00;
    tsp00_Int4          TabStatMinLenStringCol_gg00;
    tsp00_Int4          TabStatMaxLenStringCol_gg00;
    tsp00_Int4          TabStatAllStringColPages_gg00;
    tsp00_Int4          TabStatAveStringColPages_gg00;
    tsp00_Int4          TabStatMinStringColPages_gg00;
    tsp00_Int4          TabStatMaxStringColPages_gg00;
    tsp00_Int4          TabStatClusterCount_gg00;
    tsp00_Int2          TabStatRootLevel_gg00;
    tsp00_Int2          TabStatAveKeyLength_gg00;
    tsp00_Int2          TabStatMinKeyLength_gg00;
    tsp00_Int2          TabStatMaxKeyLength_gg00;
    tsp00_Int2          TabStatAveSepLength_gg00;
    tsp00_Int2          TabStatMinSepLength_gg00;
    tsp00_Int2          TabStatMaxSepLength_gg00;
} tgg00_TableStatInfo;
typedef tgg00_TableStatInfo       *tgg00_TableStatInfoPtr;

typedef struct                     tgg00_SampleInfo
{
    tgg00_FileId        sam_tree_id;
    tsp00_Int4          sam_rows;
    tsp00_Int4          sam_percent;
    pasbool             sam_with_long;
    pasbool             sam_fill1;
    tsp00_Int2          sam_fill2;
    tsp00_Int4          leafnodes;
    tsp00_Int4          sec_key_cnt;
    tsp00_Int4          prim_key_cnt;
    tsp00_Int4          avg_prim_per_list;
    tsp00_Int4          records;
    tsp00_Int4          all_stringcol_pages;
    tsp00_Int4          nodes;
    tsp00_Int4          null_value_cnt;
    tsp00_Int4          min_sec_key_len;
    tsp00_Int4          max_sec_key_len;
    tsp00_Int4          avg_sec_key_len;
    tsp00_Int4          treecov;
    tsp00_Int4          rootcov;
    tsp00_Int4          indexcov;
    tsp00_Int4          min_indexcov;
    tsp00_Int4          max_indexcov;
    tsp00_Int4          leafcov;
    tsp00_Int4          min_leafcov;
    tsp00_Int4          max_leafcov;
    tsp00_Int4          rec_per_page;
    tsp00_Int4          ave_rec_length;
    tsp00_Int4          min_rec_length;
    tsp00_Int4          max_rec_length;
    tsp00_Int4          ave_key_length;
    tsp00_Int4          indexnodes;
    tsp00_Int4          ave_sep_length;
    tsp00_Int2          min_key_length;
    tsp00_Int2          max_key_length;
    tsp00_Int2          min_sep_length;
    tsp00_Int2          max_sep_length;
    tsp00_Int2          min_rec_per_page;
    tsp00_Int2          max_rec_per_page;
    tsp00_PageNo        rootsegm;
    tgg00_FiletypeSet   smplFileType_gg00;
    pasbool             pcs_dummy;
    tsp00_Int2          hight;
    tsp00_Int4          defined_stringcols;
    tsp00_Int4          min_len_stringcol;
    tsp00_Int4          max_len_stringcol;
    tsp00_Int4          ave_len_stringcol;
    tsp00_Int4          min_stringcol_pages;
    tsp00_Int4          max_stringcol_pages;
    tsp00_Int4          ave_stringcol_pages;
    tsp00_Int4          invsel_1;
    tsp00_Int4          invsel_5;
    tsp00_Int4          invsel_10;
    tsp00_Int4          invsel_25;
    tsp00_Int4          invsel_notsel;
    tsp00_Int4          min_prim_per_list;
    tsp00_Int4          max_prim_per_list;
    tsp00_Int4          sam_clustercount;
    ARRAY_VAR_MSP00 (         dist_values, tsp00_Int4, MAX_COL_PER_TAB_GG00);
} tgg00_SampleInfo;
typedef tgg00_SampleInfo          *tgg00_SampleInfoPtr;

typedef struct                     tgg00_IndexCounter
{
    tsp00_Int4          idc_prim_leafnodes;
    tsp00_Int4          idc_prim_indexnodes;
    tsp00_Int4          idc_sec_leafnodes;
    tsp00_Int4          idc_prim_keycount;
    tsp00_Int4          idc_sec_keycount;
    tsp00_Int4          idc_nullcount;
    tsp00_Int4          idc_prim_mismatch;
} tgg00_IndexCounter;

typedef struct                     tgg00_DataPart
{
    union
    {
    /* 1 */
        struct tgg00_DataPart_1
        {
            tsp00_MoveObj       mbp_buf_F;
        } C_1;
    /* 2 */
        struct tgg00_DataPart_2
        {
            tsp00_Buf           mbp_4kbuf_F;
        } C_2;
    /* 3 */
        struct tgg00_DataPart_3
        {
            tgg00_Rec           mbp_rec_F;
        } C_3;
    /* 4 */
        struct tgg00_DataPart_4
        {
            tsp00_Int2          mbp_reclen_F;
            tsp00_Int2          mbp_keylen_F;
            tsp00_Int2          mbp_varcol_offset_F;
            tsp00_Int2          mbp_varcol_cnt_F;
        } C_4;
    /* 5 */
        struct tgg00_DataPart_5
        {
            tgg00_Lkey          mbp_key_F;
            tgg00_Lkey          mbp_key2_F;
        } C_5;
    /* 6 */
        struct tgg00_DataPart_6
        {
            tsp00_Int2          mbp_space1_F;
            tsp00_Int2          mbp_space2_F;
            tsp00_Int2          mbp_space3_F;
            tsp00_Int2          mbp_space4_F;
            tgg00_RecBody       mbp_recinfo_F;
        } C_6;
    /* 7 */
        struct tgg00_DataPart_7
        {
            tgg00_SampleInfo    mbp_sample_F;
        } C_7;
    /* 8 */
        struct tgg00_DataPart_8
        {
            tgg00_IndexCounter  mbp_index_cnt_F;
        } C_8;
    /* 9 */
        struct tgg00_DataPart_9
        {
            tgg00_Info          mbp_info_F;
        } C_9;
    /* 10 */
        struct tgg00_DataPart_10
        {
            tgg00_ColStatInfo   mbp_ColStat_F;
        } C_10;
    /* 11 */
        struct tgg00_DataPart_11
        {
            tgg00_TreeStatInfo  mbp_TreeStat_F;
        } C_11;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tsp00_MoveObj      &mbp_buf                ()  { return this->variant.C_1.mbp_buf_F; };
    tsp00_Buf          &mbp_4kbuf              ()  { return this->variant.C_2.mbp_4kbuf_F; };
    tgg00_Rec          &mbp_rec                ()  { return this->variant.C_3.mbp_rec_F; };
    tsp00_Int2         &mbp_reclen             ()  { return this->variant.C_4.mbp_reclen_F; };
    tsp00_Int2         &mbp_keylen             ()  { return this->variant.C_4.mbp_keylen_F; };
    tsp00_Int2         &mbp_varcol_offset      ()  { return this->variant.C_4.mbp_varcol_offset_F; };
    tsp00_Int2         &mbp_varcol_cnt         ()  { return this->variant.C_4.mbp_varcol_cnt_F; };
    tgg00_Lkey         &mbp_key                ()  { return this->variant.C_5.mbp_key_F; };
    tgg00_Lkey         &mbp_key2               ()  { return this->variant.C_5.mbp_key2_F; };
    tsp00_Int2         &mbp_space1             ()  { return this->variant.C_6.mbp_space1_F; };
    tsp00_Int2         &mbp_space2             ()  { return this->variant.C_6.mbp_space2_F; };
    tsp00_Int2         &mbp_space3             ()  { return this->variant.C_6.mbp_space3_F; };
    tsp00_Int2         &mbp_space4             ()  { return this->variant.C_6.mbp_space4_F; };
    tgg00_RecBody      &mbp_recinfo            ()  { return this->variant.C_6.mbp_recinfo_F; };
    tgg00_SampleInfo   &mbp_sample             ()  { return this->variant.C_7.mbp_sample_F; };
    tgg00_IndexCounter &mbp_index_cnt          ()  { return this->variant.C_8.mbp_index_cnt_F; };
    tgg00_Info         &mbp_info               ()  { return this->variant.C_9.mbp_info_F; };
    tgg00_ColStatInfo  &mbp_ColStat            ()  { return this->variant.C_10.mbp_ColStat_F; };
    tgg00_TreeStatInfo &mbp_TreeStat           ()  { return this->variant.C_11.mbp_TreeStat_F; };
    /* const accessor functions */
    const tsp00_MoveObj &mbp_buf                () const  { return this->variant.C_1.mbp_buf_F; };
    const tsp00_Buf    &mbp_4kbuf              () const  { return this->variant.C_2.mbp_4kbuf_F; };
    const tgg00_Rec    &mbp_rec                () const  { return this->variant.C_3.mbp_rec_F; };
    const tsp00_Int2   &mbp_reclen             () const  { return this->variant.C_4.mbp_reclen_F; };
    const tsp00_Int2   &mbp_keylen             () const  { return this->variant.C_4.mbp_keylen_F; };
    const tsp00_Int2   &mbp_varcol_offset      () const  { return this->variant.C_4.mbp_varcol_offset_F; };
    const tsp00_Int2   &mbp_varcol_cnt         () const  { return this->variant.C_4.mbp_varcol_cnt_F; };
    const tgg00_Lkey   &mbp_key                () const  { return this->variant.C_5.mbp_key_F; };
    const tgg00_Lkey   &mbp_key2               () const  { return this->variant.C_5.mbp_key2_F; };
    const tsp00_Int2   &mbp_space1             () const  { return this->variant.C_6.mbp_space1_F; };
    const tsp00_Int2   &mbp_space2             () const  { return this->variant.C_6.mbp_space2_F; };
    const tsp00_Int2   &mbp_space3             () const  { return this->variant.C_6.mbp_space3_F; };
    const tsp00_Int2   &mbp_space4             () const  { return this->variant.C_6.mbp_space4_F; };
    const tgg00_RecBody &mbp_recinfo            () const  { return this->variant.C_6.mbp_recinfo_F; };
    const tgg00_SampleInfo &mbp_sample             () const  { return this->variant.C_7.mbp_sample_F; };
    const tgg00_IndexCounter &mbp_index_cnt          () const  { return this->variant.C_8.mbp_index_cnt_F; };
    const tgg00_Info   &mbp_info               () const  { return this->variant.C_9.mbp_info_F; };
    const tgg00_ColStatInfo &mbp_ColStat            () const  { return this->variant.C_10.mbp_ColStat_F; };
    const tgg00_TreeStatInfo &mbp_TreeStat           () const  { return this->variant.C_11.mbp_TreeStat_F; };
#endif    /* defined (__cplusplus) */
} tgg00_DataPart;
typedef tgg00_DataPart            *tgg00_DataPartPtr;
typedef OLDPASCALSTRING_TYPE_MSP00 (  tgg00_LongresultBuf, MAX_LONGRESULTBUF_GG00);
typedef CSTRING_TYPE_MSP00      (  tgg00_LongresultBufc, MAX_LONGRESULTBUF_GG00);

typedef struct                     tgg00_SelectBuffer
{
    tgg00_Rec           buf_rec;
    tsp00_Buf           rbuf;
    tsp00_Buf           lbuf;
    tgg00_Rec           inv_buf;
    union
    {
    /* true */
        struct tgg00_SelectBuffer_true
        {
            tgg00_Rec           result_F;
        } C_true;
    /* false */
        struct tgg00_SelectBuffer_false
        {
            tgg00_LongresultBuf longresult_F;
        } C_false;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tgg00_Rec          &result                 ()  { return this->variant.C_true.result_F; };
    tgg00_LongresultBuf &longresult             ()  { return this->variant.C_false.longresult_F; };
    /* const accessor functions */
    const tgg00_Rec    &result                 () const  { return this->variant.C_true.result_F; };
    const tgg00_LongresultBuf &longresult             () const  { return this->variant.C_false.longresult_F; };
#endif    /* defined (__cplusplus) */
} tgg00_SelectBuffer;

typedef struct                     tgg00_StackEntry
{
    union
    {
    /* 1 */
        struct tgg00_StackEntry_1
        {
            tgg00_StackEntryType etype_F;
            tgg00_StackOpType   eop_F;
            tsp00_Int2          epos_F;
            tsp00_Int2          elen_var_F;
            tsp00_C2            ecol_tab_F;
        } C_1;
    /* 2 */
        struct tgg00_StackEntry_2
        {
            tgg00_StackEntryType efill2_type_F;
            tgg00_StackOpFunc   eop_func_F;
            tsp00_C2            epos_c2_F;
            tsp00_C2            var_elen_F;
            tsp00_Int2          ecol_pos_F;
        } C_2;
    /* 3 */
        struct tgg00_StackEntry_3
        {
            tgg00_StackEntryType efill3_type_F;
            tgg00_StackOpOutput eop_out_F;
            tsp00_Int2          ecolno_F;
            tsp00_Int4          eval4_var_F;
        } C_3;
    /* 4 */
        struct tgg00_StackEntry_4
        {
            tgg00_StackEntryType efill4_type_F;
            tgg00_StackOpBuildIn eop_build_in_F;
            tgg00_DateTimeFormat eformat_F;
            tsp00_DataType      edatatype_F;
            tsp00_C3            elanguage_F;
            tsp00_Uint1         elength_F;
        } C_4;
    /* 5 */
        struct tgg00_StackEntry_5
        {
            tgg00_StackEntryType efill5_type_F;
            pasbool             eop_new_rec_F;
        } C_5;
    /* 6 */
        struct tgg00_StackEntry_6
        {
            tgg00_StackEntryType efill6_type_F;
            pasbool             eop_fill_F;
            tsp00_Int2          efill7_pos_F;
            tsp00_Int2          efill8_len_F;
            tsp00_Int2          eindex_F;
        } C_6;
    /* 7 */
        struct tgg00_StackEntry_7
        {
            tgg00_StackEntryType efill7_type_F;
            pasbool             eop7_fill_F;
            tsp00_DataType      eparamdatatype_F;
            tsp00_Uint1         efraction_F;
        } C_7;
    /* 8 */
        struct tgg00_StackEntry_8
        {
            tgg00_Surrogate     esurrogate_F;
        } C_8;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tgg00_StackEntryType &etype                  ()  { return this->variant.C_1.etype_F; };
    tgg00_StackOpType  &eop                    ()  { return this->variant.C_1.eop_F; };
    tsp00_Int2         &epos                   ()  { return this->variant.C_1.epos_F; };
    tsp00_Int2         &elen_var               ()  { return this->variant.C_1.elen_var_F; };
    tsp00_C2           &ecol_tab               ()  { return this->variant.C_1.ecol_tab_F; };
    tgg00_StackEntryType &efill2_type            ()  { return this->variant.C_2.efill2_type_F; };
    tgg00_StackOpFunc  &eop_func               ()  { return this->variant.C_2.eop_func_F; };
    tsp00_C2           &epos_c2                ()  { return this->variant.C_2.epos_c2_F; };
    tsp00_C2           &var_elen               ()  { return this->variant.C_2.var_elen_F; };
    tsp00_Int2         &ecol_pos               ()  { return this->variant.C_2.ecol_pos_F; };
    tgg00_StackEntryType &efill3_type            ()  { return this->variant.C_3.efill3_type_F; };
    tgg00_StackOpOutput &eop_out                ()  { return this->variant.C_3.eop_out_F; };
    tsp00_Int2         &ecolno                 ()  { return this->variant.C_3.ecolno_F; };
    tsp00_Int4         &eval4_var              ()  { return this->variant.C_3.eval4_var_F; };
    tgg00_StackEntryType &efill4_type            ()  { return this->variant.C_4.efill4_type_F; };
    tgg00_StackOpBuildIn &eop_build_in           ()  { return this->variant.C_4.eop_build_in_F; };
    tgg00_DateTimeFormat &eformat                ()  { return this->variant.C_4.eformat_F; };
    tsp00_DataType     &edatatype              ()  { return this->variant.C_4.edatatype_F; };
    tsp00_C3           &elanguage              ()  { return this->variant.C_4.elanguage_F; };
    tsp00_Uint1        &elength                ()  { return this->variant.C_4.elength_F; };
    tgg00_StackEntryType &efill5_type            ()  { return this->variant.C_5.efill5_type_F; };
    pasbool            &eop_new_rec            ()  { return this->variant.C_5.eop_new_rec_F; };
    tgg00_StackEntryType &efill6_type            ()  { return this->variant.C_6.efill6_type_F; };
    pasbool            &eop_fill               ()  { return this->variant.C_6.eop_fill_F; };
    tsp00_Int2         &efill7_pos             ()  { return this->variant.C_6.efill7_pos_F; };
    tsp00_Int2         &efill8_len             ()  { return this->variant.C_6.efill8_len_F; };
    tsp00_Int2         &eindex                 ()  { return this->variant.C_6.eindex_F; };
    tgg00_StackEntryType &efill7_type            ()  { return this->variant.C_7.efill7_type_F; };
    pasbool            &eop7_fill              ()  { return this->variant.C_7.eop7_fill_F; };
    tsp00_DataType     &eparamdatatype         ()  { return this->variant.C_7.eparamdatatype_F; };
    tsp00_Uint1        &efraction              ()  { return this->variant.C_7.efraction_F; };
    tgg00_Surrogate    &esurrogate             ()  { return this->variant.C_8.esurrogate_F; };
    /* const accessor functions */
    const tgg00_StackEntryType &etype                  () const  { return this->variant.C_1.etype_F; };
    const tgg00_StackOpType &eop                    () const  { return this->variant.C_1.eop_F; };
    const tsp00_Int2   &epos                   () const  { return this->variant.C_1.epos_F; };
    const tsp00_Int2   &elen_var               () const  { return this->variant.C_1.elen_var_F; };
    const tsp00_C2     &ecol_tab               () const  { return this->variant.C_1.ecol_tab_F; };
    const tgg00_StackEntryType &efill2_type            () const  { return this->variant.C_2.efill2_type_F; };
    const tgg00_StackOpFunc &eop_func               () const  { return this->variant.C_2.eop_func_F; };
    const tsp00_C2     &epos_c2                () const  { return this->variant.C_2.epos_c2_F; };
    const tsp00_C2     &var_elen               () const  { return this->variant.C_2.var_elen_F; };
    const tsp00_Int2   &ecol_pos               () const  { return this->variant.C_2.ecol_pos_F; };
    const tgg00_StackEntryType &efill3_type            () const  { return this->variant.C_3.efill3_type_F; };
    const tgg00_StackOpOutput &eop_out                () const  { return this->variant.C_3.eop_out_F; };
    const tsp00_Int2   &ecolno                 () const  { return this->variant.C_3.ecolno_F; };
    const tsp00_Int4   &eval4_var              () const  { return this->variant.C_3.eval4_var_F; };
    const tgg00_StackEntryType &efill4_type            () const  { return this->variant.C_4.efill4_type_F; };
    const tgg00_StackOpBuildIn &eop_build_in           () const  { return this->variant.C_4.eop_build_in_F; };
    const tgg00_DateTimeFormat &eformat                () const  { return this->variant.C_4.eformat_F; };
    const tsp00_DataType &edatatype              () const  { return this->variant.C_4.edatatype_F; };
    const tsp00_C3     &elanguage              () const  { return this->variant.C_4.elanguage_F; };
    const tsp00_Uint1  &elength                () const  { return this->variant.C_4.elength_F; };
    const tgg00_StackEntryType &efill5_type            () const  { return this->variant.C_5.efill5_type_F; };
    const pasbool      &eop_new_rec            () const  { return this->variant.C_5.eop_new_rec_F; };
    const tgg00_StackEntryType &efill6_type            () const  { return this->variant.C_6.efill6_type_F; };
    const pasbool      &eop_fill               () const  { return this->variant.C_6.eop_fill_F; };
    const tsp00_Int2   &efill7_pos             () const  { return this->variant.C_6.efill7_pos_F; };
    const tsp00_Int2   &efill8_len             () const  { return this->variant.C_6.efill8_len_F; };
    const tsp00_Int2   &eindex                 () const  { return this->variant.C_6.eindex_F; };
    const tgg00_StackEntryType &efill7_type            () const  { return this->variant.C_7.efill7_type_F; };
    const pasbool      &eop7_fill              () const  { return this->variant.C_7.eop7_fill_F; };
    const tsp00_DataType &eparamdatatype         () const  { return this->variant.C_7.eparamdatatype_F; };
    const tsp00_Uint1  &efraction              () const  { return this->variant.C_7.efraction_F; };
    const tgg00_Surrogate &esurrogate             () const  { return this->variant.C_8.esurrogate_F; };
#endif    /* defined (__cplusplus) */
} tgg00_StackEntry;
typedef tgg00_StackEntry          *tgg00_StEntryAddr;
typedef ARRAY_TYPE_MSP00 (         tgg00_StackList, tgg00_StackEntry, MAX_INT2_SP00);
typedef tgg00_StackList           *tgg00_StackListPtr;

typedef struct                     tgg00_ObjColDesc
{
    tsp00_Int2          colHashCount_gg00;
    tsp00_Int2          colInvCount_gg00;
    tgg00_StackListPtr  colHashList_gg00;
    tgg00_StackListPtr  colInvList_gg00;
} tgg00_ObjColDesc;

typedef enum tgg00_StackState__Enum
{
    ssSubquery_egg00,
    ssConstParamExpr_egg00,
    ssCopyRow_egg00,
    ssUnused4_egg00,
    ssUnused5_egg00,
    ssUnused6_egg00,
    ssUnused7_egg00,
    ssUnused8_egg00
} tgg00_StackState__Enum;
typedef tgg00_StackState__Enum tgg00_StackState__Param;
typedef SMALLSET_TYPE_MSP00 (      tgg00_StackState, tgg00_StackState__Enum, ssUnused8_egg00);

typedef struct                     tgg00_StackDesc
{
    tsp00_Int2          mcol_pos;
    tsp00_Int2          mcol_cnt;
    tsp00_Int2          mmult_pos;
    tsp00_Int2          mmult_cnt;
    tsp00_Int2          mqual_pos;
    tsp00_Int2          mqual_cnt;
    tsp00_Int2          mview_pos;
    tsp00_Int2          mview_cnt;
    tsp00_Int2          mupd_pos;
    tsp00_Int2          mupd_cnt;
    tsp00_Int2          mlink_pos;
    tsp00_Int2          mlink_cnt;
    tsp00_Int2          mstrat_pos;
    tsp00_Int2          mstrat_cnt;
    tsp00_Int2          mstring_pos;
    tsp00_Int2          mstring_cnt;
    tsp00_Int2          mtrigger_pos;
    tsp00_Int2          mtrigger_cnt;
    tsp00_Int2          minvqual_pos;
    tsp00_Int2          minvqual_cnt;
    union
    {
    /* 1 */
        struct tgg00_StackDesc_1
        {
            tsp00_Int2          mresqual_pos_F;
            tsp00_Int2          mresqual_cnt_F;
            tsp00_Int2          mspecial_F;
            tgg00_StackState    mstack_state_F;
            pasbool             mbool_F;
            tsp00_SqlMode       msqlmode_F;
            tgg00_DateTimeFormat mdatetimeformat_F;
            tsp00_Int2          mcachetrigger_F;
            tsp00_Int4          malignfill3_F;
            tsp00_Int2          mst_optimize_pos_F;
            tsp00_Int2          mst_work_depth_F;
            tsp00_Int2          mfirst_free_F;
            tsp00_Int2          mst_max_F;
            tgg00_StackListPtr  mst_addr_F;
        } C_1;
    /* 2 */
        struct tgg00_StackDesc_2
        {
            tsp00_Int2          mfill1_F;
            tsp00_Int2          mfill2_F;
            tsp00_Int2          mfixedcol_len_F;
            tsp00_C2            mfill3_F;
            tsp00_C8            mfill4_F;
            tsp00_C4            mfill5_F;
            tsp00_C4            mfill6_F;
            tsp00_C8            maddr_space_F;
        } C_2;
    /* 3 */
        struct tgg00_StackDesc_3
        {
            tsp00_Int2          mfill7_F;
            tsp00_Int2          mfill8_F;
            pasbool             mswap_F;
            tsp00_SwapKind      mswap_kind_F;
        } C_3;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tsp00_Int2         &mresqual_pos           ()  { return this->variant.C_1.mresqual_pos_F; };
    tsp00_Int2         &mresqual_cnt           ()  { return this->variant.C_1.mresqual_cnt_F; };
    tsp00_Int2         &mspecial               ()  { return this->variant.C_1.mspecial_F; };
    tgg00_StackState   &mstack_state           ()  { return this->variant.C_1.mstack_state_F; };
    pasbool            &mbool                  ()  { return this->variant.C_1.mbool_F; };
    tsp00_SqlMode      &msqlmode               ()  { return this->variant.C_1.msqlmode_F; };
    tgg00_DateTimeFormat &mdatetimeformat        ()  { return this->variant.C_1.mdatetimeformat_F; };
    tsp00_Int2         &mcachetrigger          ()  { return this->variant.C_1.mcachetrigger_F; };
    tsp00_Int4         &malignfill3            ()  { return this->variant.C_1.malignfill3_F; };
    tsp00_Int2         &mst_optimize_pos       ()  { return this->variant.C_1.mst_optimize_pos_F; };
    tsp00_Int2         &mst_work_depth         ()  { return this->variant.C_1.mst_work_depth_F; };
    tsp00_Int2         &mfirst_free            ()  { return this->variant.C_1.mfirst_free_F; };
    tsp00_Int2         &mst_max                ()  { return this->variant.C_1.mst_max_F; };
    tgg00_StackListPtr &mst_addr               ()  { return this->variant.C_1.mst_addr_F; };
    tsp00_Int2         &mfill1                 ()  { return this->variant.C_2.mfill1_F; };
    tsp00_Int2         &mfill2                 ()  { return this->variant.C_2.mfill2_F; };
    tsp00_Int2         &mfixedcol_len          ()  { return this->variant.C_2.mfixedcol_len_F; };
    tsp00_C2           &mfill3                 ()  { return this->variant.C_2.mfill3_F; };
    tsp00_C8           &mfill4                 ()  { return this->variant.C_2.mfill4_F; };
    tsp00_C4           &mfill5                 ()  { return this->variant.C_2.mfill5_F; };
    tsp00_C4           &mfill6                 ()  { return this->variant.C_2.mfill6_F; };
    tsp00_C8           &maddr_space            ()  { return this->variant.C_2.maddr_space_F; };
    tsp00_Int2         &mfill7                 ()  { return this->variant.C_3.mfill7_F; };
    tsp00_Int2         &mfill8                 ()  { return this->variant.C_3.mfill8_F; };
    pasbool            &mswap                  ()  { return this->variant.C_3.mswap_F; };
    tsp00_SwapKind     &mswap_kind             ()  { return this->variant.C_3.mswap_kind_F; };
    /* const accessor functions */
    const tsp00_Int2   &mresqual_pos           () const  { return this->variant.C_1.mresqual_pos_F; };
    const tsp00_Int2   &mresqual_cnt           () const  { return this->variant.C_1.mresqual_cnt_F; };
    const tsp00_Int2   &mspecial               () const  { return this->variant.C_1.mspecial_F; };
    const tgg00_StackState &mstack_state           () const  { return this->variant.C_1.mstack_state_F; };
    const pasbool      &mbool                  () const  { return this->variant.C_1.mbool_F; };
    const tsp00_SqlMode &msqlmode               () const  { return this->variant.C_1.msqlmode_F; };
    const tgg00_DateTimeFormat &mdatetimeformat        () const  { return this->variant.C_1.mdatetimeformat_F; };
    const tsp00_Int2   &mcachetrigger          () const  { return this->variant.C_1.mcachetrigger_F; };
    const tsp00_Int4   &malignfill3            () const  { return this->variant.C_1.malignfill3_F; };
    const tsp00_Int2   &mst_optimize_pos       () const  { return this->variant.C_1.mst_optimize_pos_F; };
    const tsp00_Int2   &mst_work_depth         () const  { return this->variant.C_1.mst_work_depth_F; };
    const tsp00_Int2   &mfirst_free            () const  { return this->variant.C_1.mfirst_free_F; };
    const tsp00_Int2   &mst_max                () const  { return this->variant.C_1.mst_max_F; };
    const tgg00_StackListPtr &mst_addr               () const  { return this->variant.C_1.mst_addr_F; };
    const tsp00_Int2   &mfill1                 () const  { return this->variant.C_2.mfill1_F; };
    const tsp00_Int2   &mfill2                 () const  { return this->variant.C_2.mfill2_F; };
    const tsp00_Int2   &mfixedcol_len          () const  { return this->variant.C_2.mfixedcol_len_F; };
    const tsp00_C2     &mfill3                 () const  { return this->variant.C_2.mfill3_F; };
    const tsp00_C8     &mfill4                 () const  { return this->variant.C_2.mfill4_F; };
    const tsp00_C4     &mfill5                 () const  { return this->variant.C_2.mfill5_F; };
    const tsp00_C4     &mfill6                 () const  { return this->variant.C_2.mfill6_F; };
    const tsp00_C8     &maddr_space            () const  { return this->variant.C_2.maddr_space_F; };
    const tsp00_Int2   &mfill7                 () const  { return this->variant.C_3.mfill7_F; };
    const tsp00_Int2   &mfill8                 () const  { return this->variant.C_3.mfill8_F; };
    const pasbool      &mswap                  () const  { return this->variant.C_3.mswap_F; };
    const tsp00_SwapKind &mswap_kind             () const  { return this->variant.C_3.mswap_kind_F; };
#endif    /* defined (__cplusplus) */
} tgg00_StackDesc;

typedef struct                     tgg00_LongQual
{
    tgg00_Surrogate     lq_lock_tabid;
    tsp00_Int4          lq_pos;
    tsp00_Int4          lq_len;
    tsp00_Int4          lq_long_size;
    pasbool             lq_long_in_file;
    tsp00_CodeType      lq_code;
    pasbool             lq_first_insert;
    pasbool             lq_last_part_of_usercmd;
    pasbool             lq_prefetch;
    tsp00_C1            lq_unused_filler1;
    union
    {
    /* false */
        struct tgg00_LongQual_false
        {
            tsp00_C1            lq_expand_char_F;
            pasbool             lq_trunc_req_F;
            tsp00_Int4          lq_data_offset_F;
        } C_false;
    /* true */
        struct tgg00_LongQual_true
        {
            pasbool             lq_try_unlock_F;
            pasbool             lq_is_pattern_F;
        } C_true;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tsp00_C1           &lq_expand_char         ()  { return this->variant.C_false.lq_expand_char_F; };
    pasbool            &lq_trunc_req           ()  { return this->variant.C_false.lq_trunc_req_F; };
    tsp00_Int4         &lq_data_offset         ()  { return this->variant.C_false.lq_data_offset_F; };
    pasbool            &lq_try_unlock          ()  { return this->variant.C_true.lq_try_unlock_F; };
    pasbool            &lq_is_pattern          ()  { return this->variant.C_true.lq_is_pattern_F; };
    /* const accessor functions */
    const tsp00_C1     &lq_expand_char         () const  { return this->variant.C_false.lq_expand_char_F; };
    const pasbool      &lq_trunc_req           () const  { return this->variant.C_false.lq_trunc_req_F; };
    const tsp00_Int4   &lq_data_offset         () const  { return this->variant.C_false.lq_data_offset_F; };
    const pasbool      &lq_try_unlock          () const  { return this->variant.C_true.lq_try_unlock_F; };
    const pasbool      &lq_is_pattern          () const  { return this->variant.C_true.lq_is_pattern_F; };
#endif    /* defined (__cplusplus) */
} tgg00_LongQual;

typedef enum tgg00_FetchState_Enum
{
    fs_reverse_fetch,
    fs_use_inv,
    fs_use_stop,
    fs_use_recs,
    fs_single,
    fs_use_cluster
} tgg00_FetchState_Enum;
typedef tgg00_FetchState_Enum tgg00_FetchState_Param;
typedef ENUM_TYPE_MSP00 (          tgg00_FetchState, tgg00_FetchState_Enum, fs_use_cluster, unsigned char);

typedef struct                     tgg00_FetchDesc
{
    tgg00_FileId        m_fill1;
    tgg00_StackDesc     m_fill2;
    tsp00_ResNum        m_resnum;
    pasbool             m_res_build;
    tgg00_FileId        m_restree;
    tsp00_ResNum        m_keycnt;
    tsp00_C1            m_fill3;
    tsp00_Int4          m_searched_pages;
    tgg00_FileId        m_fns1;
    tgg00_FileId        m_fns2;
    tsp00_Int4          m_rescnt;
    tsp00_Int4          m_rowno;
    tsp00_Int4          m_cnt;
    SMALLSET_VAR_MSP00 (      m_fetch_state, tgg00_FetchState_Enum, fs_use_cluster);
    tsp00_C3            m_fill4;
    tsp00_Int2          m_keylen;
    tsp00_Int2          m_leng;
    tsp00_Int4          m_exp_idx_pages;
    tgg00_TwoKeys       m_startkeys;
    tgg00_TwoKeys       m_stopkeys;
    tgg00_TwoKeys       m_firstkeys;
    tgg00_Lkey          m_start;
    tsp00_Int2          m_dist_optim;
    tgg00_RefInfoIndex  m_strat_info;
    tgg00_QualKind      m_qual_kind;
    tgg00_DataPartPtr   m_data;
} tgg00_FetchDesc;

typedef struct                     tgg00_LinkChainInfo
{
    tgg00_FileId        mlp_fill1;
    tgg00_StackDesc     mlp_stack_desc;
    tgg00_FileId        mlp_linktree;
    tsp00_Int2          mlp_use_fileid;
    tgg00_ServerdbNo    mlp_use_site;
    tsp00_Int2          mlp_sa_level;
    tsp00_Int2          mlp_lo_level;
    tgg00_ServerdbNo    mlp_sendsite;
    tsp00_KnlIdentifier mlp_owner;
    tsp00_KnlIdentifier mlp_tablen;
    tsp00_KnlIdentifier mlp_ref_name;
} tgg00_LinkChainInfo;

typedef struct                     tgg00_TriggerInfo
{
    tsp00_Int2          tg_param_pos;
    tsp00_Int2          tg_param_cnt;
    tsp00_Int2          tg_qual_pos;
    tsp00_Int2          tg_qual_cnt;
    tgg00_MiscSet       tg_misc;
    tgg00_MessType      tg_messType;
    tsp00_Int2          tg_updated_set_size;
    tgg00_ColumnSet     tg_upd_set;
    tgg00_ColumnSet     tg_updated_set;
} tgg00_TriggerInfo;

typedef struct                     tgg00_TriggerInfoQualBuf
{
    tgg00_LinkChainInfo tiqb_link_filler;
    tgg00_FileId        tiqb_tree_id;
    tsp00_Int4          tiqb_trigger_count;
    tsp00_C4            tiqb_key;
    tsp00_C4            tiqb_init_key;
    ARRAY_VAR_MSP00 (         tiqb_trigger_info, tgg00_TriggerInfo, 2);
} tgg00_TriggerInfoQualBuf;

typedef struct                     tgg00_QualBuf
{
    union
    {
    /* 1 */
        struct tgg00_QualBuf_1
        {
            tsp00_Buf           buf_F;
        } C_1;
    /* 2 */
        struct tgg00_QualBuf_2
        {
            tgg00_FileId        mtree_F;
            tsp00_Int2          mcol_pos_F;
            tsp00_Int2          mcol_cnt_F;
            tsp00_Int2          mmult_pos_F;
            tsp00_Int2          mmult_cnt_F;
            tsp00_Int2          mqual_pos_F;
            tsp00_Int2          mqual_cnt_F;
            tsp00_Int2          mview_pos_F;
            tsp00_Int2          mview_cnt_F;
            tsp00_Int2          mupd_pos_F;
            tsp00_Int2          mupd_cnt_F;
            tsp00_Int2          mlink_pos_F;
            tsp00_Int2          mlink_cnt_F;
            tsp00_Int2          mstrat_pos_F;
            tsp00_Int2          mstrat_cnt_F;
            tsp00_Int2          mstring_pos_F;
            tsp00_Int2          mstring_cnt_F;
            tsp00_Int2          mtrigger_pos_F;
            tsp00_Int2          mtrigger_cnt_F;
            tsp00_Int2          minvqual_pos_F;
            tsp00_Int2          minvqual_cnt_F;
            tsp00_Int2          mresqual_pos_F;
            tsp00_Int2          mresqual_cnt_F;
            tsp00_Int2          mspecial_F;
            tgg00_StackState    mstack_state_F;
            pasbool             mbool_F;
            tsp00_SqlMode       msqlmode_F;
            tgg00_DateTimeFormat mdatetimeformat_F;
            tsp00_Int2          mcachetrigger_F;
            tsp00_Int4          malignfill3_F;
            tsp00_Int2          mst_optimize_pos_F;
            tsp00_Int2          mst_work_depth_F;
            tsp00_Int2          mfirst_free_F;
            tsp00_Int2          mst_max_F;
            tgg00_StackListPtr  mst_addr_F;
        } C_2;
    /* 3 */
        struct tgg00_QualBuf_3
        {
            tgg00_FileId        mst_fill_F;
            tgg00_StackDesc     mstack_desc_F;
            tgg00_FileId        mlinktree_F;
        } C_3;
    /* 4 */
        struct tgg00_QualBuf_4
        {
            tgg00_FileId        mr_fill1_F;
            tgg00_StackDesc     mr_fill2_F;
            tsp00_ResNum        mr_resnum_F;
            pasbool             mr_res_build_F;
            tgg00_FileId        mr_restree_F;
            tsp00_ResNum        mr_keycnt_F;
            pasbool             mr_fill3_F;
            tsp00_Int4          mr_pagecnt_F;
            tsp00_Number        mr_first_serial_F;
            tsp00_Number        mr_last_serial_F;
        } C_4;
    /* 5 */
        struct tgg00_QualBuf_5
        {
            tgg00_FileId        mtable_tree_F;
            tsp00_Int2          mold_long_cnt_F;
            tsp00_Int2          mnew_long_cnt_F;
        } C_5;
    /* 6 */
        struct tgg00_QualBuf_6
        {
            tgg00_FileId        mut_fill1_F;
            tgg00_DiagType      mut_diag_type_F;
            pasbool             mut_config_F;
            tsp00_Int2          mut_pool_size_F;
            tsp00_Int2          mut_index_no_F;
            tsp00_Int4          mut_pno_F;
            tsp00_Int4          mut_pno2_F;
            tsp00_Int4          mut_count_F;
            tgg00_DumpStateSet  mut_dump_state_F;
            tgg00_Surrogate     mut_surrogate_F;
            tsp00_KnlIdentifier mut_authname_F;
            tsp00_KnlIdentifier mut_tabname_F;
            tsp00_VFilename     mut_dev_F;
            tsp00_VFilename     mut_dev2_F;
            tsp00_VFilename     mut_hostfn_F;
        } C_6;
    /* 7 */
        struct tgg00_QualBuf_7
        {
            tsp00_Int2          mlock_cnt_F;
            tsp00_Int2          mlock_fill1_F;
            ARRAY_VAR_MSP00 (         mlock_F, tgg00_Lock, MAX_QUALBUF_LOCKS_GG00);
        } C_7;
    /* 8 */
        struct tgg00_QualBuf_8
        {
            tgg00_SaveRestoreInputParam msave_restore_F;
        } C_8;
    /* 10 */
        struct tgg00_QualBuf_10
        {
            tgg00_FetchDesc     mf_desc_F;
        } C_10;
    /* 11 */
        struct tgg00_QualBuf_11
        {
            tgg00_FileId        ml_tree_F;
            tgg00_StackDesc     ml_stack_desc_F;
            tgg00_LongQual      ml_long_qual_F;
        } C_11;
    /* 13 */
        struct tgg00_QualBuf_13
        {
            tgg00_FileId        mcfc_tree_F;
            tsp00_PageNo        mcfc_pno_F;
            tsp00_Int4          mcfc_fill_len_F;
        } C_13;
    /* 14 */
        struct tgg00_QualBuf_14
        {
            tgg00_LinkChainInfo mlc_info_F;
        } C_14;
    /* 15 */
        struct tgg00_QualBuf_15
        {
            tgg00_FileId        mv_tree_F;
            tsp00_Int2          mv_longcnt_F;
            tsp00_Int2          mv_newlongcnt_F;
            pasbool             mv_extended_F;
            pasbool             mv_delete_longfile_F;
            tsp00_Int2          mv_filler2_F;
        } C_15;
    /* 16 */
        struct tgg00_QualBuf_16
        {
            tgg00_TriggerInfoQualBuf mtrigger_info_F;
        } C_16;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tsp00_Buf          &buf                    ()  { return this->variant.C_1.buf_F; };
    tgg00_FileId       &mtree                  ()  { return this->variant.C_2.mtree_F; };
    tsp00_Int2         &mcol_pos               ()  { return this->variant.C_2.mcol_pos_F; };
    tsp00_Int2         &mcol_cnt               ()  { return this->variant.C_2.mcol_cnt_F; };
    tsp00_Int2         &mmult_pos              ()  { return this->variant.C_2.mmult_pos_F; };
    tsp00_Int2         &mmult_cnt              ()  { return this->variant.C_2.mmult_cnt_F; };
    tsp00_Int2         &mqual_pos              ()  { return this->variant.C_2.mqual_pos_F; };
    tsp00_Int2         &mqual_cnt              ()  { return this->variant.C_2.mqual_cnt_F; };
    tsp00_Int2         &mview_pos              ()  { return this->variant.C_2.mview_pos_F; };
    tsp00_Int2         &mview_cnt              ()  { return this->variant.C_2.mview_cnt_F; };
    tsp00_Int2         &mupd_pos               ()  { return this->variant.C_2.mupd_pos_F; };
    tsp00_Int2         &mupd_cnt               ()  { return this->variant.C_2.mupd_cnt_F; };
    tsp00_Int2         &mlink_pos              ()  { return this->variant.C_2.mlink_pos_F; };
    tsp00_Int2         &mlink_cnt              ()  { return this->variant.C_2.mlink_cnt_F; };
    tsp00_Int2         &mstrat_pos             ()  { return this->variant.C_2.mstrat_pos_F; };
    tsp00_Int2         &mstrat_cnt             ()  { return this->variant.C_2.mstrat_cnt_F; };
    tsp00_Int2         &mstring_pos            ()  { return this->variant.C_2.mstring_pos_F; };
    tsp00_Int2         &mstring_cnt            ()  { return this->variant.C_2.mstring_cnt_F; };
    tsp00_Int2         &mtrigger_pos           ()  { return this->variant.C_2.mtrigger_pos_F; };
    tsp00_Int2         &mtrigger_cnt           ()  { return this->variant.C_2.mtrigger_cnt_F; };
    tsp00_Int2         &minvqual_pos           ()  { return this->variant.C_2.minvqual_pos_F; };
    tsp00_Int2         &minvqual_cnt           ()  { return this->variant.C_2.minvqual_cnt_F; };
    tsp00_Int2         &mresqual_pos           ()  { return this->variant.C_2.mresqual_pos_F; };
    tsp00_Int2         &mresqual_cnt           ()  { return this->variant.C_2.mresqual_cnt_F; };
    tsp00_Int2         &mspecial               ()  { return this->variant.C_2.mspecial_F; };
    tgg00_StackState   &mstack_state           ()  { return this->variant.C_2.mstack_state_F; };
    pasbool            &mbool                  ()  { return this->variant.C_2.mbool_F; };
    tsp00_SqlMode      &msqlmode               ()  { return this->variant.C_2.msqlmode_F; };
    tgg00_DateTimeFormat &mdatetimeformat        ()  { return this->variant.C_2.mdatetimeformat_F; };
    tsp00_Int2         &mcachetrigger          ()  { return this->variant.C_2.mcachetrigger_F; };
    tsp00_Int4         &malignfill3            ()  { return this->variant.C_2.malignfill3_F; };
    tsp00_Int2         &mst_optimize_pos       ()  { return this->variant.C_2.mst_optimize_pos_F; };
    tsp00_Int2         &mst_work_depth         ()  { return this->variant.C_2.mst_work_depth_F; };
    tsp00_Int2         &mfirst_free            ()  { return this->variant.C_2.mfirst_free_F; };
    tsp00_Int2         &mst_max                ()  { return this->variant.C_2.mst_max_F; };
    tgg00_StackListPtr &mst_addr               ()  { return this->variant.C_2.mst_addr_F; };
    tgg00_FileId       &mst_fill               ()  { return this->variant.C_3.mst_fill_F; };
    tgg00_StackDesc    &mstack_desc            ()  { return this->variant.C_3.mstack_desc_F; };
    tgg00_FileId       &mlinktree              ()  { return this->variant.C_3.mlinktree_F; };
    tgg00_FileId       &mr_fill1               ()  { return this->variant.C_4.mr_fill1_F; };
    tgg00_StackDesc    &mr_fill2               ()  { return this->variant.C_4.mr_fill2_F; };
    tsp00_ResNum       &mr_resnum              ()  { return this->variant.C_4.mr_resnum_F; };
    pasbool            &mr_res_build           ()  { return this->variant.C_4.mr_res_build_F; };
    tgg00_FileId       &mr_restree             ()  { return this->variant.C_4.mr_restree_F; };
    tsp00_ResNum       &mr_keycnt              ()  { return this->variant.C_4.mr_keycnt_F; };
    pasbool            &mr_fill3               ()  { return this->variant.C_4.mr_fill3_F; };
    tsp00_Int4         &mr_pagecnt             ()  { return this->variant.C_4.mr_pagecnt_F; };
    tsp00_Number       &mr_first_serial        ()  { return this->variant.C_4.mr_first_serial_F; };
    tsp00_Number       &mr_last_serial         ()  { return this->variant.C_4.mr_last_serial_F; };
    tgg00_FileId       &mtable_tree            ()  { return this->variant.C_5.mtable_tree_F; };
    tsp00_Int2         &mold_long_cnt          ()  { return this->variant.C_5.mold_long_cnt_F; };
    tsp00_Int2         &mnew_long_cnt          ()  { return this->variant.C_5.mnew_long_cnt_F; };
    tgg00_FileId       &mut_fill1              ()  { return this->variant.C_6.mut_fill1_F; };
    tgg00_DiagType     &mut_diag_type          ()  { return this->variant.C_6.mut_diag_type_F; };
    pasbool            &mut_config             ()  { return this->variant.C_6.mut_config_F; };
    tsp00_Int2         &mut_pool_size          ()  { return this->variant.C_6.mut_pool_size_F; };
    tsp00_Int2         &mut_index_no           ()  { return this->variant.C_6.mut_index_no_F; };
    tsp00_Int4         &mut_pno                ()  { return this->variant.C_6.mut_pno_F; };
    tsp00_Int4         &mut_pno2               ()  { return this->variant.C_6.mut_pno2_F; };
    tsp00_Int4         &mut_count              ()  { return this->variant.C_6.mut_count_F; };
    tgg00_DumpStateSet &mut_dump_state         ()  { return this->variant.C_6.mut_dump_state_F; };
    tgg00_Surrogate    &mut_surrogate          ()  { return this->variant.C_6.mut_surrogate_F; };
    tsp00_KnlIdentifier &mut_authname           ()  { return this->variant.C_6.mut_authname_F; };
    tsp00_KnlIdentifier &mut_tabname            ()  { return this->variant.C_6.mut_tabname_F; };
    tsp00_VFilename    &mut_dev                ()  { return this->variant.C_6.mut_dev_F; };
    tsp00_VFilename    &mut_dev2               ()  { return this->variant.C_6.mut_dev2_F; };
    tsp00_VFilename    &mut_hostfn             ()  { return this->variant.C_6.mut_hostfn_F; };
    tsp00_Int2         &mlock_cnt              ()  { return this->variant.C_7.mlock_cnt_F; };
    tsp00_Int2         &mlock_fill1            ()  { return this->variant.C_7.mlock_fill1_F; };
    ARRAY_TYPESTRING_MSP00(tgg00_Lock, MAX_QUALBUF_LOCKS_GG00) &mlock                  ()  { return this->variant.C_7.mlock_F; };
    tgg00_SaveRestoreInputParam &msave_restore          ()  { return this->variant.C_8.msave_restore_F; };
    tgg00_FetchDesc    &mf_desc                ()  { return this->variant.C_10.mf_desc_F; };
    tgg00_FileId       &ml_tree                ()  { return this->variant.C_11.ml_tree_F; };
    tgg00_StackDesc    &ml_stack_desc          ()  { return this->variant.C_11.ml_stack_desc_F; };
    tgg00_LongQual     &ml_long_qual           ()  { return this->variant.C_11.ml_long_qual_F; };
    tgg00_FileId       &mcfc_tree              ()  { return this->variant.C_13.mcfc_tree_F; };
    tsp00_PageNo       &mcfc_pno               ()  { return this->variant.C_13.mcfc_pno_F; };
    tsp00_Int4         &mcfc_fill_len          ()  { return this->variant.C_13.mcfc_fill_len_F; };
    tgg00_LinkChainInfo &mlc_info               ()  { return this->variant.C_14.mlc_info_F; };
    tgg00_FileId       &mv_tree                ()  { return this->variant.C_15.mv_tree_F; };
    tsp00_Int2         &mv_longcnt             ()  { return this->variant.C_15.mv_longcnt_F; };
    tsp00_Int2         &mv_newlongcnt          ()  { return this->variant.C_15.mv_newlongcnt_F; };
    pasbool            &mv_extended            ()  { return this->variant.C_15.mv_extended_F; };
    pasbool            &mv_delete_longfile     ()  { return this->variant.C_15.mv_delete_longfile_F; };
    tsp00_Int2         &mv_filler2             ()  { return this->variant.C_15.mv_filler2_F; };
    tgg00_TriggerInfoQualBuf &mtrigger_info          ()  { return this->variant.C_16.mtrigger_info_F; };
    /* const accessor functions */
    const tsp00_Buf    &buf                    () const  { return this->variant.C_1.buf_F; };
    const tgg00_FileId &mtree                  () const  { return this->variant.C_2.mtree_F; };
    const tsp00_Int2   &mcol_pos               () const  { return this->variant.C_2.mcol_pos_F; };
    const tsp00_Int2   &mcol_cnt               () const  { return this->variant.C_2.mcol_cnt_F; };
    const tsp00_Int2   &mmult_pos              () const  { return this->variant.C_2.mmult_pos_F; };
    const tsp00_Int2   &mmult_cnt              () const  { return this->variant.C_2.mmult_cnt_F; };
    const tsp00_Int2   &mqual_pos              () const  { return this->variant.C_2.mqual_pos_F; };
    const tsp00_Int2   &mqual_cnt              () const  { return this->variant.C_2.mqual_cnt_F; };
    const tsp00_Int2   &mview_pos              () const  { return this->variant.C_2.mview_pos_F; };
    const tsp00_Int2   &mview_cnt              () const  { return this->variant.C_2.mview_cnt_F; };
    const tsp00_Int2   &mupd_pos               () const  { return this->variant.C_2.mupd_pos_F; };
    const tsp00_Int2   &mupd_cnt               () const  { return this->variant.C_2.mupd_cnt_F; };
    const tsp00_Int2   &mlink_pos              () const  { return this->variant.C_2.mlink_pos_F; };
    const tsp00_Int2   &mlink_cnt              () const  { return this->variant.C_2.mlink_cnt_F; };
    const tsp00_Int2   &mstrat_pos             () const  { return this->variant.C_2.mstrat_pos_F; };
    const tsp00_Int2   &mstrat_cnt             () const  { return this->variant.C_2.mstrat_cnt_F; };
    const tsp00_Int2   &mstring_pos            () const  { return this->variant.C_2.mstring_pos_F; };
    const tsp00_Int2   &mstring_cnt            () const  { return this->variant.C_2.mstring_cnt_F; };
    const tsp00_Int2   &mtrigger_pos           () const  { return this->variant.C_2.mtrigger_pos_F; };
    const tsp00_Int2   &mtrigger_cnt           () const  { return this->variant.C_2.mtrigger_cnt_F; };
    const tsp00_Int2   &minvqual_pos           () const  { return this->variant.C_2.minvqual_pos_F; };
    const tsp00_Int2   &minvqual_cnt           () const  { return this->variant.C_2.minvqual_cnt_F; };
    const tsp00_Int2   &mresqual_pos           () const  { return this->variant.C_2.mresqual_pos_F; };
    const tsp00_Int2   &mresqual_cnt           () const  { return this->variant.C_2.mresqual_cnt_F; };
    const tsp00_Int2   &mspecial               () const  { return this->variant.C_2.mspecial_F; };
    const tgg00_StackState &mstack_state           () const  { return this->variant.C_2.mstack_state_F; };
    const pasbool      &mbool                  () const  { return this->variant.C_2.mbool_F; };
    const tsp00_SqlMode &msqlmode               () const  { return this->variant.C_2.msqlmode_F; };
    const tgg00_DateTimeFormat &mdatetimeformat        () const  { return this->variant.C_2.mdatetimeformat_F; };
    const tsp00_Int2   &mcachetrigger          () const  { return this->variant.C_2.mcachetrigger_F; };
    const tsp00_Int4   &malignfill3            () const  { return this->variant.C_2.malignfill3_F; };
    const tsp00_Int2   &mst_optimize_pos       () const  { return this->variant.C_2.mst_optimize_pos_F; };
    const tsp00_Int2   &mst_work_depth         () const  { return this->variant.C_2.mst_work_depth_F; };
    const tsp00_Int2   &mfirst_free            () const  { return this->variant.C_2.mfirst_free_F; };
    const tsp00_Int2   &mst_max                () const  { return this->variant.C_2.mst_max_F; };
    const tgg00_StackListPtr &mst_addr               () const  { return this->variant.C_2.mst_addr_F; };
    const tgg00_FileId &mst_fill               () const  { return this->variant.C_3.mst_fill_F; };
    const tgg00_StackDesc &mstack_desc            () const  { return this->variant.C_3.mstack_desc_F; };
    const tgg00_FileId &mlinktree              () const  { return this->variant.C_3.mlinktree_F; };
    const tgg00_FileId &mr_fill1               () const  { return this->variant.C_4.mr_fill1_F; };
    const tgg00_StackDesc &mr_fill2               () const  { return this->variant.C_4.mr_fill2_F; };
    const tsp00_ResNum &mr_resnum              () const  { return this->variant.C_4.mr_resnum_F; };
    const pasbool      &mr_res_build           () const  { return this->variant.C_4.mr_res_build_F; };
    const tgg00_FileId &mr_restree             () const  { return this->variant.C_4.mr_restree_F; };
    const tsp00_ResNum &mr_keycnt              () const  { return this->variant.C_4.mr_keycnt_F; };
    const pasbool      &mr_fill3               () const  { return this->variant.C_4.mr_fill3_F; };
    const tsp00_Int4   &mr_pagecnt             () const  { return this->variant.C_4.mr_pagecnt_F; };
    const tsp00_Number &mr_first_serial        () const  { return this->variant.C_4.mr_first_serial_F; };
    const tsp00_Number &mr_last_serial         () const  { return this->variant.C_4.mr_last_serial_F; };
    const tgg00_FileId &mtable_tree            () const  { return this->variant.C_5.mtable_tree_F; };
    const tsp00_Int2   &mold_long_cnt          () const  { return this->variant.C_5.mold_long_cnt_F; };
    const tsp00_Int2   &mnew_long_cnt          () const  { return this->variant.C_5.mnew_long_cnt_F; };
    const tgg00_FileId &mut_fill1              () const  { return this->variant.C_6.mut_fill1_F; };
    const tgg00_DiagType &mut_diag_type          () const  { return this->variant.C_6.mut_diag_type_F; };
    const pasbool      &mut_config             () const  { return this->variant.C_6.mut_config_F; };
    const tsp00_Int2   &mut_pool_size          () const  { return this->variant.C_6.mut_pool_size_F; };
    const tsp00_Int2   &mut_index_no           () const  { return this->variant.C_6.mut_index_no_F; };
    const tsp00_Int4   &mut_pno                () const  { return this->variant.C_6.mut_pno_F; };
    const tsp00_Int4   &mut_pno2               () const  { return this->variant.C_6.mut_pno2_F; };
    const tsp00_Int4   &mut_count              () const  { return this->variant.C_6.mut_count_F; };
    const tgg00_DumpStateSet &mut_dump_state         () const  { return this->variant.C_6.mut_dump_state_F; };
    const tgg00_Surrogate &mut_surrogate          () const  { return this->variant.C_6.mut_surrogate_F; };
    const tsp00_KnlIdentifier &mut_authname           () const  { return this->variant.C_6.mut_authname_F; };
    const tsp00_KnlIdentifier &mut_tabname            () const  { return this->variant.C_6.mut_tabname_F; };
    const tsp00_VFilename &mut_dev                () const  { return this->variant.C_6.mut_dev_F; };
    const tsp00_VFilename &mut_dev2               () const  { return this->variant.C_6.mut_dev2_F; };
    const tsp00_VFilename &mut_hostfn             () const  { return this->variant.C_6.mut_hostfn_F; };
    const tsp00_Int2   &mlock_cnt              () const  { return this->variant.C_7.mlock_cnt_F; };
    const tsp00_Int2   &mlock_fill1            () const  { return this->variant.C_7.mlock_fill1_F; };
    const ARRAY_TYPESTRING_MSP00(tgg00_Lock, MAX_QUALBUF_LOCKS_GG00) &mlock                  () const  { return this->variant.C_7.mlock_F; };
    const tgg00_SaveRestoreInputParam &msave_restore          () const  { return this->variant.C_8.msave_restore_F; };
    const tgg00_FetchDesc &mf_desc                () const  { return this->variant.C_10.mf_desc_F; };
    const tgg00_FileId &ml_tree                () const  { return this->variant.C_11.ml_tree_F; };
    const tgg00_StackDesc &ml_stack_desc          () const  { return this->variant.C_11.ml_stack_desc_F; };
    const tgg00_LongQual &ml_long_qual           () const  { return this->variant.C_11.ml_long_qual_F; };
    const tgg00_FileId &mcfc_tree              () const  { return this->variant.C_13.mcfc_tree_F; };
    const tsp00_PageNo &mcfc_pno               () const  { return this->variant.C_13.mcfc_pno_F; };
    const tsp00_Int4   &mcfc_fill_len          () const  { return this->variant.C_13.mcfc_fill_len_F; };
    const tgg00_LinkChainInfo &mlc_info               () const  { return this->variant.C_14.mlc_info_F; };
    const tgg00_FileId &mv_tree                () const  { return this->variant.C_15.mv_tree_F; };
    const tsp00_Int2   &mv_longcnt             () const  { return this->variant.C_15.mv_longcnt_F; };
    const tsp00_Int2   &mv_newlongcnt          () const  { return this->variant.C_15.mv_newlongcnt_F; };
    const pasbool      &mv_extended            () const  { return this->variant.C_15.mv_extended_F; };
    const pasbool      &mv_delete_longfile     () const  { return this->variant.C_15.mv_delete_longfile_F; };
    const tsp00_Int2   &mv_filler2             () const  { return this->variant.C_15.mv_filler2_F; };
    const tgg00_TriggerInfoQualBuf &mtrigger_info          () const  { return this->variant.C_16.mtrigger_info_F; };
#endif    /* defined (__cplusplus) */
} tgg00_QualBuf;
typedef tgg00_QualBuf             *tgg00_QualBufPtr;
typedef ARRAY_TYPE_MSP00 (         tgg00_FieldLists, tsp00_MoveObjPtr, cgg_idx_max_valuefieldlist - 0 + 1);
typedef tgg00_FieldLists          *tgg00_FieldListsPtr;

typedef struct                     tgg00_MessBufHeader
{
    tsp00_Int4          mb_qual_len;
    tsp00_Int4          mb_data_len;
    tsp00_Int4          mb_strat_len;
    tsp00_Int4          mb_fill2;
    tgg00_ServerdbNo    mb_src_site;
    pasbool             mb_reply;
    pasbool             mb_fill3;
    tgg00_MessType      mb_type;
    tgg00_MessType2     mb_type2;
    tgg00_MessBufStruct mb_struct;
    pasbool             mb_fill1;
} tgg00_MessBufHeader;
typedef struct tgg00_MessBlock tgg00_MessBlock;
typedef tgg00_MessBlock           *tgg00_MessBlockPtr;

typedef struct                     tgg00_MessBufTrailer
{
    union
    {
    /* 2 */
        struct tgg00_MessBufTrailer_2
        {
            tsp00_C8            mb_ptr_space1_F;
            tsp00_C8            mb_ptr_space2_F;
            tsp00_C8            mb_ptr_space3_F;
            tsp00_C8            mb_ptr_space4_F;
            tsp00_C8            mb_ptr_space5_F;
            tsp00_C8            mb_ptr_space6_F;
            tsp00_C8            mb_ptr_space7_F;
            tsp00_C8            mb_ptr_space8_F;
            tsp00_Int4          mb_data_size_F;
            tsp00_Int4          mb_qual_size_F;
            tsp00_Int4          mb_strat_size_F;
            tsp00_Int4          mb_st_size_F;
            tsp00_Int2          mb_st_max_F;
            tsp00_Int2          mb_work_st_max_F;
            tsp00_Int4          mb_workbuf_size_F;
            tgg00_FieldLists    mb_fieldlists_F;
        } C_2;
    /* 3 */
        struct tgg00_MessBufTrailer_3
        {
            tgg00_DataPartPtr   mb_data_F;
        } C_3;
    /* 4 */
        struct tgg00_MessBufTrailer_4
        {
            tsp00_C8            mb_pt_space4_1_F;
            tgg00_QualBufPtr    mb_qual_F;
        } C_4;
    /* 5 */
        struct tgg00_MessBufTrailer_5
        {
            tsp00_C8            mb_pt_space5_1_F;
            tsp00_C8            mb_ptr_space5_2_F;
            tgg00_StackListPtr  mb_st_F;
        } C_5;
    /* 6 */
        struct tgg00_MessBufTrailer_6
        {
            tsp00_C8            mb_pt_space6_1_F;
            tsp00_C8            mb_ptr_space6_2_F;
            tsp00_C8            mb_ptr_space6_3_F;
            tgg00_StackListPtr  mb_work_st_F;
        } C_6;
    /* 7 */
        struct tgg00_MessBufTrailer_7
        {
            tsp00_C8            mb_pt_space7_1_F;
            tsp00_C8            mb_ptr_space7_2_F;
            tsp00_C8            mb_ptr_space7_3_F;
            tsp00_C8            mb_ptr_space7_4_F;
            tsp00_MoveObjPtr    mb_workbuf_F;
        } C_7;
    /* 8 */
        struct tgg00_MessBufTrailer_8
        {
            tsp00_C8            mb_pt_space8_1_F;
            tsp00_C8            mb_ptr_space8_2_F;
            tsp00_C8            mb_ptr_space8_3_F;
            tsp00_C8            mb_ptr_space8_4_F;
            tsp00_C8            mb_ptr_space8_5_F;
            tgg00_MessBlockPtr  mb_next_mblock_F;
        } C_8;
    /* 9 */
        struct tgg00_MessBufTrailer_9
        {
            tsp00_C8            mb_pt_space9_1_F;
            tsp00_C8            mb_ptr_space9_2_F;
            tsp00_C8            mb_ptr_space9_3_F;
            tsp00_C8            mb_ptr_space9_4_F;
            tsp00_C8            mb_ptr_space9_5_F;
            tsp00_C8            mb_ptr_space9_6_F;
            tgg00_TransContextPtr mb_trns_F;
        } C_9;
    /* 10 */
        struct tgg00_MessBufTrailer_10
        {
            tsp00_C8            mb_pt_space10_1_F;
            tsp00_C8            mb_ptr_space10_2_F;
            tsp00_C8            mb_ptr_space10_3_F;
            tsp00_C8            mb_ptr_space10_4_F;
            tsp00_C8            mb_ptr_space10_5_F;
            tsp00_C8            mb_ptr_space10_6_F;
            tsp00_C8            mb_ptr_space10_7_F;
            tsp00_MoveObjPtr    mb_strat_F;
        } C_10;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tsp00_C8           &mb_ptr_space1          ()  { return this->variant.C_2.mb_ptr_space1_F; };
    tsp00_C8           &mb_ptr_space2          ()  { return this->variant.C_2.mb_ptr_space2_F; };
    tsp00_C8           &mb_ptr_space3          ()  { return this->variant.C_2.mb_ptr_space3_F; };
    tsp00_C8           &mb_ptr_space4          ()  { return this->variant.C_2.mb_ptr_space4_F; };
    tsp00_C8           &mb_ptr_space5          ()  { return this->variant.C_2.mb_ptr_space5_F; };
    tsp00_C8           &mb_ptr_space6          ()  { return this->variant.C_2.mb_ptr_space6_F; };
    tsp00_C8           &mb_ptr_space7          ()  { return this->variant.C_2.mb_ptr_space7_F; };
    tsp00_C8           &mb_ptr_space8          ()  { return this->variant.C_2.mb_ptr_space8_F; };
    tsp00_Int4         &mb_data_size           ()  { return this->variant.C_2.mb_data_size_F; };
    tsp00_Int4         &mb_qual_size           ()  { return this->variant.C_2.mb_qual_size_F; };
    tsp00_Int4         &mb_strat_size          ()  { return this->variant.C_2.mb_strat_size_F; };
    tsp00_Int4         &mb_st_size             ()  { return this->variant.C_2.mb_st_size_F; };
    tsp00_Int2         &mb_st_max              ()  { return this->variant.C_2.mb_st_max_F; };
    tsp00_Int2         &mb_work_st_max         ()  { return this->variant.C_2.mb_work_st_max_F; };
    tsp00_Int4         &mb_workbuf_size        ()  { return this->variant.C_2.mb_workbuf_size_F; };
    tgg00_FieldLists   &mb_fieldlists          ()  { return this->variant.C_2.mb_fieldlists_F; };
    tgg00_DataPartPtr  &mb_data                ()  { return this->variant.C_3.mb_data_F; };
    tsp00_C8           &mb_pt_space4_1         ()  { return this->variant.C_4.mb_pt_space4_1_F; };
    tgg00_QualBufPtr   &mb_qual                ()  { return this->variant.C_4.mb_qual_F; };
    tsp00_C8           &mb_pt_space5_1         ()  { return this->variant.C_5.mb_pt_space5_1_F; };
    tsp00_C8           &mb_ptr_space5_2        ()  { return this->variant.C_5.mb_ptr_space5_2_F; };
    tgg00_StackListPtr &mb_st                  ()  { return this->variant.C_5.mb_st_F; };
    tsp00_C8           &mb_pt_space6_1         ()  { return this->variant.C_6.mb_pt_space6_1_F; };
    tsp00_C8           &mb_ptr_space6_2        ()  { return this->variant.C_6.mb_ptr_space6_2_F; };
    tsp00_C8           &mb_ptr_space6_3        ()  { return this->variant.C_6.mb_ptr_space6_3_F; };
    tgg00_StackListPtr &mb_work_st             ()  { return this->variant.C_6.mb_work_st_F; };
    tsp00_C8           &mb_pt_space7_1         ()  { return this->variant.C_7.mb_pt_space7_1_F; };
    tsp00_C8           &mb_ptr_space7_2        ()  { return this->variant.C_7.mb_ptr_space7_2_F; };
    tsp00_C8           &mb_ptr_space7_3        ()  { return this->variant.C_7.mb_ptr_space7_3_F; };
    tsp00_C8           &mb_ptr_space7_4        ()  { return this->variant.C_7.mb_ptr_space7_4_F; };
    tsp00_MoveObjPtr   &mb_workbuf             ()  { return this->variant.C_7.mb_workbuf_F; };
    tsp00_C8           &mb_pt_space8_1         ()  { return this->variant.C_8.mb_pt_space8_1_F; };
    tsp00_C8           &mb_ptr_space8_2        ()  { return this->variant.C_8.mb_ptr_space8_2_F; };
    tsp00_C8           &mb_ptr_space8_3        ()  { return this->variant.C_8.mb_ptr_space8_3_F; };
    tsp00_C8           &mb_ptr_space8_4        ()  { return this->variant.C_8.mb_ptr_space8_4_F; };
    tsp00_C8           &mb_ptr_space8_5        ()  { return this->variant.C_8.mb_ptr_space8_5_F; };
    tgg00_MessBlockPtr &mb_next_mblock         ()  { return this->variant.C_8.mb_next_mblock_F; };
    tsp00_C8           &mb_pt_space9_1         ()  { return this->variant.C_9.mb_pt_space9_1_F; };
    tsp00_C8           &mb_ptr_space9_2        ()  { return this->variant.C_9.mb_ptr_space9_2_F; };
    tsp00_C8           &mb_ptr_space9_3        ()  { return this->variant.C_9.mb_ptr_space9_3_F; };
    tsp00_C8           &mb_ptr_space9_4        ()  { return this->variant.C_9.mb_ptr_space9_4_F; };
    tsp00_C8           &mb_ptr_space9_5        ()  { return this->variant.C_9.mb_ptr_space9_5_F; };
    tsp00_C8           &mb_ptr_space9_6        ()  { return this->variant.C_9.mb_ptr_space9_6_F; };
    tgg00_TransContextPtr &mb_trns                ()  { return this->variant.C_9.mb_trns_F; };
    tsp00_C8           &mb_pt_space10_1        ()  { return this->variant.C_10.mb_pt_space10_1_F; };
    tsp00_C8           &mb_ptr_space10_2       ()  { return this->variant.C_10.mb_ptr_space10_2_F; };
    tsp00_C8           &mb_ptr_space10_3       ()  { return this->variant.C_10.mb_ptr_space10_3_F; };
    tsp00_C8           &mb_ptr_space10_4       ()  { return this->variant.C_10.mb_ptr_space10_4_F; };
    tsp00_C8           &mb_ptr_space10_5       ()  { return this->variant.C_10.mb_ptr_space10_5_F; };
    tsp00_C8           &mb_ptr_space10_6       ()  { return this->variant.C_10.mb_ptr_space10_6_F; };
    tsp00_C8           &mb_ptr_space10_7       ()  { return this->variant.C_10.mb_ptr_space10_7_F; };
    tsp00_MoveObjPtr   &mb_strat               ()  { return this->variant.C_10.mb_strat_F; };
    /* const accessor functions */
    const tsp00_C8     &mb_ptr_space1          () const  { return this->variant.C_2.mb_ptr_space1_F; };
    const tsp00_C8     &mb_ptr_space2          () const  { return this->variant.C_2.mb_ptr_space2_F; };
    const tsp00_C8     &mb_ptr_space3          () const  { return this->variant.C_2.mb_ptr_space3_F; };
    const tsp00_C8     &mb_ptr_space4          () const  { return this->variant.C_2.mb_ptr_space4_F; };
    const tsp00_C8     &mb_ptr_space5          () const  { return this->variant.C_2.mb_ptr_space5_F; };
    const tsp00_C8     &mb_ptr_space6          () const  { return this->variant.C_2.mb_ptr_space6_F; };
    const tsp00_C8     &mb_ptr_space7          () const  { return this->variant.C_2.mb_ptr_space7_F; };
    const tsp00_C8     &mb_ptr_space8          () const  { return this->variant.C_2.mb_ptr_space8_F; };
    const tsp00_Int4   &mb_data_size           () const  { return this->variant.C_2.mb_data_size_F; };
    const tsp00_Int4   &mb_qual_size           () const  { return this->variant.C_2.mb_qual_size_F; };
    const tsp00_Int4   &mb_strat_size          () const  { return this->variant.C_2.mb_strat_size_F; };
    const tsp00_Int4   &mb_st_size             () const  { return this->variant.C_2.mb_st_size_F; };
    const tsp00_Int2   &mb_st_max              () const  { return this->variant.C_2.mb_st_max_F; };
    const tsp00_Int2   &mb_work_st_max         () const  { return this->variant.C_2.mb_work_st_max_F; };
    const tsp00_Int4   &mb_workbuf_size        () const  { return this->variant.C_2.mb_workbuf_size_F; };
    const tgg00_FieldLists &mb_fieldlists          () const  { return this->variant.C_2.mb_fieldlists_F; };
    const tgg00_DataPartPtr &mb_data                () const  { return this->variant.C_3.mb_data_F; };
    const tsp00_C8     &mb_pt_space4_1         () const  { return this->variant.C_4.mb_pt_space4_1_F; };
    const tgg00_QualBufPtr &mb_qual                () const  { return this->variant.C_4.mb_qual_F; };
    const tsp00_C8     &mb_pt_space5_1         () const  { return this->variant.C_5.mb_pt_space5_1_F; };
    const tsp00_C8     &mb_ptr_space5_2        () const  { return this->variant.C_5.mb_ptr_space5_2_F; };
    const tgg00_StackListPtr &mb_st                  () const  { return this->variant.C_5.mb_st_F; };
    const tsp00_C8     &mb_pt_space6_1         () const  { return this->variant.C_6.mb_pt_space6_1_F; };
    const tsp00_C8     &mb_ptr_space6_2        () const  { return this->variant.C_6.mb_ptr_space6_2_F; };
    const tsp00_C8     &mb_ptr_space6_3        () const  { return this->variant.C_6.mb_ptr_space6_3_F; };
    const tgg00_StackListPtr &mb_work_st             () const  { return this->variant.C_6.mb_work_st_F; };
    const tsp00_C8     &mb_pt_space7_1         () const  { return this->variant.C_7.mb_pt_space7_1_F; };
    const tsp00_C8     &mb_ptr_space7_2        () const  { return this->variant.C_7.mb_ptr_space7_2_F; };
    const tsp00_C8     &mb_ptr_space7_3        () const  { return this->variant.C_7.mb_ptr_space7_3_F; };
    const tsp00_C8     &mb_ptr_space7_4        () const  { return this->variant.C_7.mb_ptr_space7_4_F; };
    const tsp00_MoveObjPtr &mb_workbuf             () const  { return this->variant.C_7.mb_workbuf_F; };
    const tsp00_C8     &mb_pt_space8_1         () const  { return this->variant.C_8.mb_pt_space8_1_F; };
    const tsp00_C8     &mb_ptr_space8_2        () const  { return this->variant.C_8.mb_ptr_space8_2_F; };
    const tsp00_C8     &mb_ptr_space8_3        () const  { return this->variant.C_8.mb_ptr_space8_3_F; };
    const tsp00_C8     &mb_ptr_space8_4        () const  { return this->variant.C_8.mb_ptr_space8_4_F; };
    const tsp00_C8     &mb_ptr_space8_5        () const  { return this->variant.C_8.mb_ptr_space8_5_F; };
    const tgg00_MessBlockPtr &mb_next_mblock         () const  { return this->variant.C_8.mb_next_mblock_F; };
    const tsp00_C8     &mb_pt_space9_1         () const  { return this->variant.C_9.mb_pt_space9_1_F; };
    const tsp00_C8     &mb_ptr_space9_2        () const  { return this->variant.C_9.mb_ptr_space9_2_F; };
    const tsp00_C8     &mb_ptr_space9_3        () const  { return this->variant.C_9.mb_ptr_space9_3_F; };
    const tsp00_C8     &mb_ptr_space9_4        () const  { return this->variant.C_9.mb_ptr_space9_4_F; };
    const tsp00_C8     &mb_ptr_space9_5        () const  { return this->variant.C_9.mb_ptr_space9_5_F; };
    const tsp00_C8     &mb_ptr_space9_6        () const  { return this->variant.C_9.mb_ptr_space9_6_F; };
    const tgg00_TransContextPtr &mb_trns                () const  { return this->variant.C_9.mb_trns_F; };
    const tsp00_C8     &mb_pt_space10_1        () const  { return this->variant.C_10.mb_pt_space10_1_F; };
    const tsp00_C8     &mb_ptr_space10_2       () const  { return this->variant.C_10.mb_ptr_space10_2_F; };
    const tsp00_C8     &mb_ptr_space10_3       () const  { return this->variant.C_10.mb_ptr_space10_3_F; };
    const tsp00_C8     &mb_ptr_space10_4       () const  { return this->variant.C_10.mb_ptr_space10_4_F; };
    const tsp00_C8     &mb_ptr_space10_5       () const  { return this->variant.C_10.mb_ptr_space10_5_F; };
    const tsp00_C8     &mb_ptr_space10_6       () const  { return this->variant.C_10.mb_ptr_space10_6_F; };
    const tsp00_C8     &mb_ptr_space10_7       () const  { return this->variant.C_10.mb_ptr_space10_7_F; };
    const tsp00_MoveObjPtr &mb_strat               () const  { return this->variant.C_10.mb_strat_F; };
#endif    /* defined (__cplusplus) */
} tgg00_MessBufTrailer;

 struct                     tgg00_MessBlock
{
    union
    {
    /* 1 */
        struct tgg00_MessBlock_1
        {
            tgg00_MessBufHeader mb_header_F;
            tgg00_MessBufTrailer mb_trailer_F;
        } C_1;
    /* 2 */
        struct tgg00_MessBlock_2
        {
            tsp00_Int4          mb_qual_len_F;
            tsp00_Int4          mb_data_len_F;
            tsp00_Int4          mb_strat_len_F;
            tsp00_Int4          mb_fill2_F;
            tgg00_ServerdbNo    mb_src_site_F;
            pasbool             mb_reply_F;
            pasbool             mb_fill3_F;
            tgg00_MessType      mb_type_F;
            tgg00_MessType2     mb_type2_F;
            tgg00_MessBufStruct mb_struct_F;
            pasbool             mb_fill1_F;
            tsp00_C8            mb_ptr_space1_F;
            tsp00_C8            mb_ptr_space2_F;
            tsp00_C8            mb_ptr_space3_F;
            tsp00_C8            mb_ptr_space4_F;
            tsp00_C8            mb_ptr_space5_F;
            tsp00_C8            mb_ptr_space6_F;
            tsp00_C8            mb_ptr_space7_F;
            tsp00_C8            mb_ptr_space8_F;
            tsp00_Int4          mb_data_size_F;
            tsp00_Int4          mb_qual_size_F;
            tsp00_Int4          mb_strat_size_F;
            tsp00_Int4          mb_st_size_F;
            tsp00_Int2          mb_st_max_F;
            tsp00_Int2          mb_work_st_max_F;
            tsp00_Int4          mb_workbuf_size_F;
            tgg00_FieldLists    mb_fieldlists_F;
        } C_2;
    /* 3 */
        struct tgg00_MessBlock_3
        {
            tgg00_MessBufHeader mb_space3_F;
            tgg00_DataPartPtr   mb_data_F;
        } C_3;
    /* 4 */
        struct tgg00_MessBlock_4
        {
            tgg00_MessBufHeader mb_space4_F;
            tsp00_C8            mb_ptr_space4_1_F;
            tgg00_QualBufPtr    mb_qual_F;
        } C_4;
    /* 5 */
        struct tgg00_MessBlock_5
        {
            tgg00_MessBufHeader mb_space5_F;
            tsp00_C8            mb_ptr_space5_1_F;
            tsp00_C8            mb_ptr_space5_2_F;
            tgg00_StackListPtr  mb_st_F;
        } C_5;
    /* 6 */
        struct tgg00_MessBlock_6
        {
            tgg00_MessBufHeader mb_space6_F;
            tsp00_C8            mb_ptr_space6_1_F;
            tsp00_C8            mb_ptr_space6_2_F;
            tsp00_C8            mb_ptr_space6_3_F;
            tgg00_StackListPtr  mb_work_st_F;
        } C_6;
    /* 7 */
        struct tgg00_MessBlock_7
        {
            tgg00_MessBufHeader mb_space7_F;
            tsp00_C8            mb_ptr_space7_1_F;
            tsp00_C8            mb_ptr_space7_2_F;
            tsp00_C8            mb_ptr_space7_3_F;
            tsp00_C8            mb_ptr_space7_4_F;
            tsp00_MoveObjPtr    mb_workbuf_F;
        } C_7;
    /* 8 */
        struct tgg00_MessBlock_8
        {
            tgg00_MessBufHeader mb_space8_F;
            tsp00_C8            mb_ptr_space8_1_F;
            tsp00_C8            mb_ptr_space8_2_F;
            tsp00_C8            mb_ptr_space8_3_F;
            tsp00_C8            mb_ptr_space8_4_F;
            tsp00_C8            mb_ptr_space8_5_F;
            tgg00_MessBlockPtr  mb_next_mblock_F;
        } C_8;
    /* 9 */
        struct tgg00_MessBlock_9
        {
            tgg00_MessBufHeader mb_space9_F;
            tsp00_C8            mb_ptr_space9_1_F;
            tsp00_C8            mb_ptr_space9_2_F;
            tsp00_C8            mb_ptr_space9_3_F;
            tsp00_C8            mb_ptr_space9_4_F;
            tsp00_C8            mb_ptr_space9_5_F;
            tsp00_C8            mb_ptr_space9_6_F;
            tgg00_TransContextPtr mb_trns_F;
        } C_9;
    /* 10 */
        struct tgg00_MessBlock_10
        {
            tgg00_MessBufHeader mb_space10_F;
            tsp00_C8            mb_ptr_space10_1_F;
            tsp00_C8            mb_ptr_space10_2_F;
            tsp00_C8            mb_ptr_space10_3_F;
            tsp00_C8            mb_ptr_space10_4_F;
            tsp00_C8            mb_ptr_space10_5_F;
            tsp00_C8            mb_ptr_space10_6_F;
            tsp00_C8            mb_ptr_space10_7_F;
            tsp00_MoveObjPtr    mb_strat_F;
        } C_10;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tgg00_MessBufHeader &mb_header              ()  { return this->variant.C_1.mb_header_F; };
    tgg00_MessBufTrailer &mb_trailer             ()  { return this->variant.C_1.mb_trailer_F; };
    tsp00_Int4         &mb_qual_len            ()  { return this->variant.C_2.mb_qual_len_F; };
    tsp00_Int4         &mb_data_len            ()  { return this->variant.C_2.mb_data_len_F; };
    tsp00_Int4         &mb_strat_len           ()  { return this->variant.C_2.mb_strat_len_F; };
    tsp00_Int4         &mb_fill2               ()  { return this->variant.C_2.mb_fill2_F; };
    tgg00_ServerdbNo   &mb_src_site            ()  { return this->variant.C_2.mb_src_site_F; };
    pasbool            &mb_reply               ()  { return this->variant.C_2.mb_reply_F; };
    pasbool            &mb_fill3               ()  { return this->variant.C_2.mb_fill3_F; };
    tgg00_MessType     &mb_type                ()  { return this->variant.C_2.mb_type_F; };
    tgg00_MessType2    &mb_type2               ()  { return this->variant.C_2.mb_type2_F; };
    tgg00_MessBufStruct &mb_struct              ()  { return this->variant.C_2.mb_struct_F; };
    pasbool            &mb_fill1               ()  { return this->variant.C_2.mb_fill1_F; };
    tsp00_C8           &mb_ptr_space1          ()  { return this->variant.C_2.mb_ptr_space1_F; };
    tsp00_C8           &mb_ptr_space2          ()  { return this->variant.C_2.mb_ptr_space2_F; };
    tsp00_C8           &mb_ptr_space3          ()  { return this->variant.C_2.mb_ptr_space3_F; };
    tsp00_C8           &mb_ptr_space4          ()  { return this->variant.C_2.mb_ptr_space4_F; };
    tsp00_C8           &mb_ptr_space5          ()  { return this->variant.C_2.mb_ptr_space5_F; };
    tsp00_C8           &mb_ptr_space6          ()  { return this->variant.C_2.mb_ptr_space6_F; };
    tsp00_C8           &mb_ptr_space7          ()  { return this->variant.C_2.mb_ptr_space7_F; };
    tsp00_C8           &mb_ptr_space8          ()  { return this->variant.C_2.mb_ptr_space8_F; };
    tsp00_Int4         &mb_data_size           ()  { return this->variant.C_2.mb_data_size_F; };
    tsp00_Int4         &mb_qual_size           ()  { return this->variant.C_2.mb_qual_size_F; };
    tsp00_Int4         &mb_strat_size          ()  { return this->variant.C_2.mb_strat_size_F; };
    tsp00_Int4         &mb_st_size             ()  { return this->variant.C_2.mb_st_size_F; };
    tsp00_Int2         &mb_st_max              ()  { return this->variant.C_2.mb_st_max_F; };
    tsp00_Int2         &mb_work_st_max         ()  { return this->variant.C_2.mb_work_st_max_F; };
    tsp00_Int4         &mb_workbuf_size        ()  { return this->variant.C_2.mb_workbuf_size_F; };
    tgg00_FieldLists   &mb_fieldlists          ()  { return this->variant.C_2.mb_fieldlists_F; };
    tgg00_MessBufHeader &mb_space3              ()  { return this->variant.C_3.mb_space3_F; };
    tgg00_DataPartPtr  &mb_data                ()  { return this->variant.C_3.mb_data_F; };
    tgg00_MessBufHeader &mb_space4              ()  { return this->variant.C_4.mb_space4_F; };
    tsp00_C8           &mb_ptr_space4_1        ()  { return this->variant.C_4.mb_ptr_space4_1_F; };
    tgg00_QualBufPtr   &mb_qual                ()  { return this->variant.C_4.mb_qual_F; };
    tgg00_MessBufHeader &mb_space5              ()  { return this->variant.C_5.mb_space5_F; };
    tsp00_C8           &mb_ptr_space5_1        ()  { return this->variant.C_5.mb_ptr_space5_1_F; };
    tsp00_C8           &mb_ptr_space5_2        ()  { return this->variant.C_5.mb_ptr_space5_2_F; };
    tgg00_StackListPtr &mb_st                  ()  { return this->variant.C_5.mb_st_F; };
    tgg00_MessBufHeader &mb_space6              ()  { return this->variant.C_6.mb_space6_F; };
    tsp00_C8           &mb_ptr_space6_1        ()  { return this->variant.C_6.mb_ptr_space6_1_F; };
    tsp00_C8           &mb_ptr_space6_2        ()  { return this->variant.C_6.mb_ptr_space6_2_F; };
    tsp00_C8           &mb_ptr_space6_3        ()  { return this->variant.C_6.mb_ptr_space6_3_F; };
    tgg00_StackListPtr &mb_work_st             ()  { return this->variant.C_6.mb_work_st_F; };
    tgg00_MessBufHeader &mb_space7              ()  { return this->variant.C_7.mb_space7_F; };
    tsp00_C8           &mb_ptr_space7_1        ()  { return this->variant.C_7.mb_ptr_space7_1_F; };
    tsp00_C8           &mb_ptr_space7_2        ()  { return this->variant.C_7.mb_ptr_space7_2_F; };
    tsp00_C8           &mb_ptr_space7_3        ()  { return this->variant.C_7.mb_ptr_space7_3_F; };
    tsp00_C8           &mb_ptr_space7_4        ()  { return this->variant.C_7.mb_ptr_space7_4_F; };
    tsp00_MoveObjPtr   &mb_workbuf             ()  { return this->variant.C_7.mb_workbuf_F; };
    tgg00_MessBufHeader &mb_space8              ()  { return this->variant.C_8.mb_space8_F; };
    tsp00_C8           &mb_ptr_space8_1        ()  { return this->variant.C_8.mb_ptr_space8_1_F; };
    tsp00_C8           &mb_ptr_space8_2        ()  { return this->variant.C_8.mb_ptr_space8_2_F; };
    tsp00_C8           &mb_ptr_space8_3        ()  { return this->variant.C_8.mb_ptr_space8_3_F; };
    tsp00_C8           &mb_ptr_space8_4        ()  { return this->variant.C_8.mb_ptr_space8_4_F; };
    tsp00_C8           &mb_ptr_space8_5        ()  { return this->variant.C_8.mb_ptr_space8_5_F; };
    tgg00_MessBlockPtr &mb_next_mblock         ()  { return this->variant.C_8.mb_next_mblock_F; };
    tgg00_MessBufHeader &mb_space9              ()  { return this->variant.C_9.mb_space9_F; };
    tsp00_C8           &mb_ptr_space9_1        ()  { return this->variant.C_9.mb_ptr_space9_1_F; };
    tsp00_C8           &mb_ptr_space9_2        ()  { return this->variant.C_9.mb_ptr_space9_2_F; };
    tsp00_C8           &mb_ptr_space9_3        ()  { return this->variant.C_9.mb_ptr_space9_3_F; };
    tsp00_C8           &mb_ptr_space9_4        ()  { return this->variant.C_9.mb_ptr_space9_4_F; };
    tsp00_C8           &mb_ptr_space9_5        ()  { return this->variant.C_9.mb_ptr_space9_5_F; };
    tsp00_C8           &mb_ptr_space9_6        ()  { return this->variant.C_9.mb_ptr_space9_6_F; };
    tgg00_TransContextPtr &mb_trns                ()  { return this->variant.C_9.mb_trns_F; };
    tgg00_MessBufHeader &mb_space10             ()  { return this->variant.C_10.mb_space10_F; };
    tsp00_C8           &mb_ptr_space10_1       ()  { return this->variant.C_10.mb_ptr_space10_1_F; };
    tsp00_C8           &mb_ptr_space10_2       ()  { return this->variant.C_10.mb_ptr_space10_2_F; };
    tsp00_C8           &mb_ptr_space10_3       ()  { return this->variant.C_10.mb_ptr_space10_3_F; };
    tsp00_C8           &mb_ptr_space10_4       ()  { return this->variant.C_10.mb_ptr_space10_4_F; };
    tsp00_C8           &mb_ptr_space10_5       ()  { return this->variant.C_10.mb_ptr_space10_5_F; };
    tsp00_C8           &mb_ptr_space10_6       ()  { return this->variant.C_10.mb_ptr_space10_6_F; };
    tsp00_C8           &mb_ptr_space10_7       ()  { return this->variant.C_10.mb_ptr_space10_7_F; };
    tsp00_MoveObjPtr   &mb_strat               ()  { return this->variant.C_10.mb_strat_F; };
    /* const accessor functions */
    const tgg00_MessBufHeader &mb_header              () const  { return this->variant.C_1.mb_header_F; };
    const tgg00_MessBufTrailer &mb_trailer             () const  { return this->variant.C_1.mb_trailer_F; };
    const tsp00_Int4   &mb_qual_len            () const  { return this->variant.C_2.mb_qual_len_F; };
    const tsp00_Int4   &mb_data_len            () const  { return this->variant.C_2.mb_data_len_F; };
    const tsp00_Int4   &mb_strat_len           () const  { return this->variant.C_2.mb_strat_len_F; };
    const tsp00_Int4   &mb_fill2               () const  { return this->variant.C_2.mb_fill2_F; };
    const tgg00_ServerdbNo &mb_src_site            () const  { return this->variant.C_2.mb_src_site_F; };
    const pasbool      &mb_reply               () const  { return this->variant.C_2.mb_reply_F; };
    const pasbool      &mb_fill3               () const  { return this->variant.C_2.mb_fill3_F; };
    const tgg00_MessType &mb_type                () const  { return this->variant.C_2.mb_type_F; };
    const tgg00_MessType2 &mb_type2               () const  { return this->variant.C_2.mb_type2_F; };
    const tgg00_MessBufStruct &mb_struct              () const  { return this->variant.C_2.mb_struct_F; };
    const pasbool      &mb_fill1               () const  { return this->variant.C_2.mb_fill1_F; };
    const tsp00_C8     &mb_ptr_space1          () const  { return this->variant.C_2.mb_ptr_space1_F; };
    const tsp00_C8     &mb_ptr_space2          () const  { return this->variant.C_2.mb_ptr_space2_F; };
    const tsp00_C8     &mb_ptr_space3          () const  { return this->variant.C_2.mb_ptr_space3_F; };
    const tsp00_C8     &mb_ptr_space4          () const  { return this->variant.C_2.mb_ptr_space4_F; };
    const tsp00_C8     &mb_ptr_space5          () const  { return this->variant.C_2.mb_ptr_space5_F; };
    const tsp00_C8     &mb_ptr_space6          () const  { return this->variant.C_2.mb_ptr_space6_F; };
    const tsp00_C8     &mb_ptr_space7          () const  { return this->variant.C_2.mb_ptr_space7_F; };
    const tsp00_C8     &mb_ptr_space8          () const  { return this->variant.C_2.mb_ptr_space8_F; };
    const tsp00_Int4   &mb_data_size           () const  { return this->variant.C_2.mb_data_size_F; };
    const tsp00_Int4   &mb_qual_size           () const  { return this->variant.C_2.mb_qual_size_F; };
    const tsp00_Int4   &mb_strat_size          () const  { return this->variant.C_2.mb_strat_size_F; };
    const tsp00_Int4   &mb_st_size             () const  { return this->variant.C_2.mb_st_size_F; };
    const tsp00_Int2   &mb_st_max              () const  { return this->variant.C_2.mb_st_max_F; };
    const tsp00_Int2   &mb_work_st_max         () const  { return this->variant.C_2.mb_work_st_max_F; };
    const tsp00_Int4   &mb_workbuf_size        () const  { return this->variant.C_2.mb_workbuf_size_F; };
    const tgg00_FieldLists &mb_fieldlists          () const  { return this->variant.C_2.mb_fieldlists_F; };
    const tgg00_MessBufHeader &mb_space3              () const  { return this->variant.C_3.mb_space3_F; };
    const tgg00_DataPartPtr &mb_data                () const  { return this->variant.C_3.mb_data_F; };
    const tgg00_MessBufHeader &mb_space4              () const  { return this->variant.C_4.mb_space4_F; };
    const tsp00_C8     &mb_ptr_space4_1        () const  { return this->variant.C_4.mb_ptr_space4_1_F; };
    const tgg00_QualBufPtr &mb_qual                () const  { return this->variant.C_4.mb_qual_F; };
    const tgg00_MessBufHeader &mb_space5              () const  { return this->variant.C_5.mb_space5_F; };
    const tsp00_C8     &mb_ptr_space5_1        () const  { return this->variant.C_5.mb_ptr_space5_1_F; };
    const tsp00_C8     &mb_ptr_space5_2        () const  { return this->variant.C_5.mb_ptr_space5_2_F; };
    const tgg00_StackListPtr &mb_st                  () const  { return this->variant.C_5.mb_st_F; };
    const tgg00_MessBufHeader &mb_space6              () const  { return this->variant.C_6.mb_space6_F; };
    const tsp00_C8     &mb_ptr_space6_1        () const  { return this->variant.C_6.mb_ptr_space6_1_F; };
    const tsp00_C8     &mb_ptr_space6_2        () const  { return this->variant.C_6.mb_ptr_space6_2_F; };
    const tsp00_C8     &mb_ptr_space6_3        () const  { return this->variant.C_6.mb_ptr_space6_3_F; };
    const tgg00_StackListPtr &mb_work_st             () const  { return this->variant.C_6.mb_work_st_F; };
    const tgg00_MessBufHeader &mb_space7              () const  { return this->variant.C_7.mb_space7_F; };
    const tsp00_C8     &mb_ptr_space7_1        () const  { return this->variant.C_7.mb_ptr_space7_1_F; };
    const tsp00_C8     &mb_ptr_space7_2        () const  { return this->variant.C_7.mb_ptr_space7_2_F; };
    const tsp00_C8     &mb_ptr_space7_3        () const  { return this->variant.C_7.mb_ptr_space7_3_F; };
    const tsp00_C8     &mb_ptr_space7_4        () const  { return this->variant.C_7.mb_ptr_space7_4_F; };
    const tsp00_MoveObjPtr &mb_workbuf             () const  { return this->variant.C_7.mb_workbuf_F; };
    const tgg00_MessBufHeader &mb_space8              () const  { return this->variant.C_8.mb_space8_F; };
    const tsp00_C8     &mb_ptr_space8_1        () const  { return this->variant.C_8.mb_ptr_space8_1_F; };
    const tsp00_C8     &mb_ptr_space8_2        () const  { return this->variant.C_8.mb_ptr_space8_2_F; };
    const tsp00_C8     &mb_ptr_space8_3        () const  { return this->variant.C_8.mb_ptr_space8_3_F; };
    const tsp00_C8     &mb_ptr_space8_4        () const  { return this->variant.C_8.mb_ptr_space8_4_F; };
    const tsp00_C8     &mb_ptr_space8_5        () const  { return this->variant.C_8.mb_ptr_space8_5_F; };
    const tgg00_MessBlockPtr &mb_next_mblock         () const  { return this->variant.C_8.mb_next_mblock_F; };
    const tgg00_MessBufHeader &mb_space9              () const  { return this->variant.C_9.mb_space9_F; };
    const tsp00_C8     &mb_ptr_space9_1        () const  { return this->variant.C_9.mb_ptr_space9_1_F; };
    const tsp00_C8     &mb_ptr_space9_2        () const  { return this->variant.C_9.mb_ptr_space9_2_F; };
    const tsp00_C8     &mb_ptr_space9_3        () const  { return this->variant.C_9.mb_ptr_space9_3_F; };
    const tsp00_C8     &mb_ptr_space9_4        () const  { return this->variant.C_9.mb_ptr_space9_4_F; };
    const tsp00_C8     &mb_ptr_space9_5        () const  { return this->variant.C_9.mb_ptr_space9_5_F; };
    const tsp00_C8     &mb_ptr_space9_6        () const  { return this->variant.C_9.mb_ptr_space9_6_F; };
    const tgg00_TransContextPtr &mb_trns                () const  { return this->variant.C_9.mb_trns_F; };
    const tgg00_MessBufHeader &mb_space10             () const  { return this->variant.C_10.mb_space10_F; };
    const tsp00_C8     &mb_ptr_space10_1       () const  { return this->variant.C_10.mb_ptr_space10_1_F; };
    const tsp00_C8     &mb_ptr_space10_2       () const  { return this->variant.C_10.mb_ptr_space10_2_F; };
    const tsp00_C8     &mb_ptr_space10_3       () const  { return this->variant.C_10.mb_ptr_space10_3_F; };
    const tsp00_C8     &mb_ptr_space10_4       () const  { return this->variant.C_10.mb_ptr_space10_4_F; };
    const tsp00_C8     &mb_ptr_space10_5       () const  { return this->variant.C_10.mb_ptr_space10_5_F; };
    const tsp00_C8     &mb_ptr_space10_6       () const  { return this->variant.C_10.mb_ptr_space10_6_F; };
    const tsp00_C8     &mb_ptr_space10_7       () const  { return this->variant.C_10.mb_ptr_space10_7_F; };
    const tsp00_MoveObjPtr &mb_strat               () const  { return this->variant.C_10.mb_strat_F; };
#endif    /* defined (__cplusplus) */
} ;

typedef struct                     tgg00_VarColPosList
{
    tsp00_Int2          vpl_last;
    tsp00_Int2          vpl_last_fill1;
    tsp00_Int4          vpl_last_fill2;
    ARRAY_VAR_MSP00 (         vpl_pos_list, tsp00_Int2, MAX_COL_PER_TAB_GG00);
} tgg00_VarColPosList;

typedef struct                     tgg00_ClusteredIOParam
{
    pasbool             clust_bDoClusteredRead_gg00;
    pasbool             clust_Filler1_gg00;
    tsp00_Int2          clust_Filler2_gg00;
    tsp00_Int4          clust_Filler3_gg00;
    tsp00_PageNo        clust_LastUsedIndexPage_gg00;
    tsp00_Int4          clust_BufferSize_gg00;
    tsp00_Addr          clust_pIOBuffer_gg00;
} tgg00_ClusteredIOParam;

typedef struct                     tgg00_SelectFieldsParam
{
    union
    {
    /* 1 */
        struct tgg00_SelectFieldsParam_1
        {
            tsp00_C48           sfp_addr_fill1_F;
            tsp00_C32           sfp_addr_fill2_F;
            tsp00_Int4          sfp_data_size_F;
            tsp00_Int4          sfp_work_st_size_F;
            tsp00_Int4          sfp_workbuf_size_F;
            tsp00_Int4          sfp_workbuf_len_F;
            tsp00_Int4          sfp_act_cntresult_F;
            tsp00_Int4          sfp_ExpIdxPages_F;
            tsp00_Int4          sfp_rows_qual_F;
            tsp00_Int4          sfp_rows_read_F;
            tsp00_Int4          sfp_m_result_size_F;
            tsp00_Int4          sfp_m_result_len_F;
            tsp00_Int4          sfp_m_result_cnt_F;
            tsp00_Int2          sfp_rec_len_F;
            tsp00_Int2          sfp_rec_key_len_F;
            tsp00_Int2          sfp_oldkey_len_F;
            tsp00_Int2          sfp_oldrec_len_F;
            tsp00_Int2          sfp_oldrec_pos_F;
            tsp00_Int2          sfp_result_length_F;
            tsp00_Int2          sfp_output_offset_F;
            tsp00_Int2          sfp_work_st_max_F;
            tsp00_Int2          sfp_resrec_maxlen_F;
            tgg00_MessType      sfp_bd_mess_type_F;
            tgg00_MessType2     sfp_bd_mess2_type_F;
            pasbool             sfp_filler_F;
            pasbool             sfp_check_for_result_F;
            tgg00_QualKind      sfp_bd_inv_only_F;
            pasbool             sfp_bd_return_knf_F;
            pasbool             sfp_bd_use_stopkey_F;
            pasbool             sfp_first_qual_F;
            pasbool             sfp_filler2_F;
            pasbool             sfp_result_wanted_F;
            tgg00_VarColPosList sfp_varcol_pos_F;
            tsp00_Number        sfp_first_serial_F;
            tsp00_Number        sfp_last_serial_F;
            tgg00_StEntryAddr   sfp_work_st_top_F;
            tgg00_StEntryAddr   sfp_work_st_bottom_F;
            tgg00_StEntryAddr   sfp_work_st_last_F;
            tgg00_StEntryAddr   sfp_work_st_frame_F;
            tsp00_Int4          sfp_workbuf_top_F;
            pasbool             sfp_check_new_rec_F;
            pasbool             sfp_optimized_F;
            tsp00_SqlMode       sfp_sqlmode_F;
            tgg00_DateTimeFormat sfp_dateTimeFormat_F;
            tsp00_Addr          sfp_acv_addr_F;
            tsp00_MoveObjPtr    sfp_param_buf_addr_F;
            tgg00_ClusteredIOParam sfp_cluster_info_F;
        } C_1;
    /* 2 */
        struct tgg00_SelectFieldsParam_2
        {
            tsp00_MoveObjPtr    sfp_data_addr_F;
            tsp00_BufAddr       sfp_rec_addr_F;
            tsp00_KeyAddr       sfp_primkey_addr_F;
            tgg00_StackListPtr  sfp_work_st_addr_F;
            tsp00_MoveObjPtr    sfp_workbuf_addr_F;
            tsp00_MoveObjPtr    sfp_oldrec_addr_F;
            tsp00_BufAddr       sfp_pl_ptr_F;
            tsp00_MoveObjPtr    sfp_m_result_addr_F;
            tgg00_TwoKeysPtr    sfp_m_firstkeys_addr_F;
            tgg00_FieldListsPtr sfp_fieldlistptr_F;
        } C_2;
    /* 3 */
        struct tgg00_SelectFieldsParam_3
        {
            tgg00_SfpInitPart   sfp_init_part_F;
        } C_3;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tsp00_C48          &sfp_addr_fill1         ()  { return this->variant.C_1.sfp_addr_fill1_F; };
    tsp00_C32          &sfp_addr_fill2         ()  { return this->variant.C_1.sfp_addr_fill2_F; };
    tsp00_Int4         &sfp_data_size          ()  { return this->variant.C_1.sfp_data_size_F; };
    tsp00_Int4         &sfp_work_st_size       ()  { return this->variant.C_1.sfp_work_st_size_F; };
    tsp00_Int4         &sfp_workbuf_size       ()  { return this->variant.C_1.sfp_workbuf_size_F; };
    tsp00_Int4         &sfp_workbuf_len        ()  { return this->variant.C_1.sfp_workbuf_len_F; };
    tsp00_Int4         &sfp_act_cntresult      ()  { return this->variant.C_1.sfp_act_cntresult_F; };
    tsp00_Int4         &sfp_ExpIdxPages        ()  { return this->variant.C_1.sfp_ExpIdxPages_F; };
    tsp00_Int4         &sfp_rows_qual          ()  { return this->variant.C_1.sfp_rows_qual_F; };
    tsp00_Int4         &sfp_rows_read          ()  { return this->variant.C_1.sfp_rows_read_F; };
    tsp00_Int4         &sfp_m_result_size      ()  { return this->variant.C_1.sfp_m_result_size_F; };
    tsp00_Int4         &sfp_m_result_len       ()  { return this->variant.C_1.sfp_m_result_len_F; };
    tsp00_Int4         &sfp_m_result_cnt       ()  { return this->variant.C_1.sfp_m_result_cnt_F; };
    tsp00_Int2         &sfp_rec_len            ()  { return this->variant.C_1.sfp_rec_len_F; };
    tsp00_Int2         &sfp_rec_key_len        ()  { return this->variant.C_1.sfp_rec_key_len_F; };
    tsp00_Int2         &sfp_oldkey_len         ()  { return this->variant.C_1.sfp_oldkey_len_F; };
    tsp00_Int2         &sfp_oldrec_len         ()  { return this->variant.C_1.sfp_oldrec_len_F; };
    tsp00_Int2         &sfp_oldrec_pos         ()  { return this->variant.C_1.sfp_oldrec_pos_F; };
    tsp00_Int2         &sfp_result_length      ()  { return this->variant.C_1.sfp_result_length_F; };
    tsp00_Int2         &sfp_output_offset      ()  { return this->variant.C_1.sfp_output_offset_F; };
    tsp00_Int2         &sfp_work_st_max        ()  { return this->variant.C_1.sfp_work_st_max_F; };
    tsp00_Int2         &sfp_resrec_maxlen      ()  { return this->variant.C_1.sfp_resrec_maxlen_F; };
    tgg00_MessType     &sfp_bd_mess_type       ()  { return this->variant.C_1.sfp_bd_mess_type_F; };
    tgg00_MessType2    &sfp_bd_mess2_type      ()  { return this->variant.C_1.sfp_bd_mess2_type_F; };
    pasbool            &sfp_filler             ()  { return this->variant.C_1.sfp_filler_F; };
    pasbool            &sfp_check_for_result   ()  { return this->variant.C_1.sfp_check_for_result_F; };
    tgg00_QualKind     &sfp_bd_inv_only        ()  { return this->variant.C_1.sfp_bd_inv_only_F; };
    pasbool            &sfp_bd_return_knf      ()  { return this->variant.C_1.sfp_bd_return_knf_F; };
    pasbool            &sfp_bd_use_stopkey     ()  { return this->variant.C_1.sfp_bd_use_stopkey_F; };
    pasbool            &sfp_first_qual         ()  { return this->variant.C_1.sfp_first_qual_F; };
    pasbool            &sfp_filler2            ()  { return this->variant.C_1.sfp_filler2_F; };
    pasbool            &sfp_result_wanted      ()  { return this->variant.C_1.sfp_result_wanted_F; };
    tgg00_VarColPosList &sfp_varcol_pos         ()  { return this->variant.C_1.sfp_varcol_pos_F; };
    tsp00_Number       &sfp_first_serial       ()  { return this->variant.C_1.sfp_first_serial_F; };
    tsp00_Number       &sfp_last_serial        ()  { return this->variant.C_1.sfp_last_serial_F; };
    tgg00_StEntryAddr  &sfp_work_st_top        ()  { return this->variant.C_1.sfp_work_st_top_F; };
    tgg00_StEntryAddr  &sfp_work_st_bottom     ()  { return this->variant.C_1.sfp_work_st_bottom_F; };
    tgg00_StEntryAddr  &sfp_work_st_last       ()  { return this->variant.C_1.sfp_work_st_last_F; };
    tgg00_StEntryAddr  &sfp_work_st_frame      ()  { return this->variant.C_1.sfp_work_st_frame_F; };
    tsp00_Int4         &sfp_workbuf_top        ()  { return this->variant.C_1.sfp_workbuf_top_F; };
    pasbool            &sfp_check_new_rec      ()  { return this->variant.C_1.sfp_check_new_rec_F; };
    pasbool            &sfp_optimized          ()  { return this->variant.C_1.sfp_optimized_F; };
    tsp00_SqlMode      &sfp_sqlmode            ()  { return this->variant.C_1.sfp_sqlmode_F; };
    tgg00_DateTimeFormat &sfp_dateTimeFormat     ()  { return this->variant.C_1.sfp_dateTimeFormat_F; };
    tsp00_Addr         &sfp_acv_addr           ()  { return this->variant.C_1.sfp_acv_addr_F; };
    tsp00_MoveObjPtr   &sfp_param_buf_addr     ()  { return this->variant.C_1.sfp_param_buf_addr_F; };
    tgg00_ClusteredIOParam &sfp_cluster_info       ()  { return this->variant.C_1.sfp_cluster_info_F; };
    tsp00_MoveObjPtr   &sfp_data_addr          ()  { return this->variant.C_2.sfp_data_addr_F; };
    tsp00_BufAddr      &sfp_rec_addr           ()  { return this->variant.C_2.sfp_rec_addr_F; };
    tsp00_KeyAddr      &sfp_primkey_addr       ()  { return this->variant.C_2.sfp_primkey_addr_F; };
    tgg00_StackListPtr &sfp_work_st_addr       ()  { return this->variant.C_2.sfp_work_st_addr_F; };
    tsp00_MoveObjPtr   &sfp_workbuf_addr       ()  { return this->variant.C_2.sfp_workbuf_addr_F; };
    tsp00_MoveObjPtr   &sfp_oldrec_addr        ()  { return this->variant.C_2.sfp_oldrec_addr_F; };
    tsp00_BufAddr      &sfp_pl_ptr             ()  { return this->variant.C_2.sfp_pl_ptr_F; };
    tsp00_MoveObjPtr   &sfp_m_result_addr      ()  { return this->variant.C_2.sfp_m_result_addr_F; };
    tgg00_TwoKeysPtr   &sfp_m_firstkeys_addr   ()  { return this->variant.C_2.sfp_m_firstkeys_addr_F; };
    tgg00_FieldListsPtr &sfp_fieldlistptr       ()  { return this->variant.C_2.sfp_fieldlistptr_F; };
    tgg00_SfpInitPart  &sfp_init_part          ()  { return this->variant.C_3.sfp_init_part_F; };
    /* const accessor functions */
    const tsp00_C48    &sfp_addr_fill1         () const  { return this->variant.C_1.sfp_addr_fill1_F; };
    const tsp00_C32    &sfp_addr_fill2         () const  { return this->variant.C_1.sfp_addr_fill2_F; };
    const tsp00_Int4   &sfp_data_size          () const  { return this->variant.C_1.sfp_data_size_F; };
    const tsp00_Int4   &sfp_work_st_size       () const  { return this->variant.C_1.sfp_work_st_size_F; };
    const tsp00_Int4   &sfp_workbuf_size       () const  { return this->variant.C_1.sfp_workbuf_size_F; };
    const tsp00_Int4   &sfp_workbuf_len        () const  { return this->variant.C_1.sfp_workbuf_len_F; };
    const tsp00_Int4   &sfp_act_cntresult      () const  { return this->variant.C_1.sfp_act_cntresult_F; };
    const tsp00_Int4   &sfp_ExpIdxPages        () const  { return this->variant.C_1.sfp_ExpIdxPages_F; };
    const tsp00_Int4   &sfp_rows_qual          () const  { return this->variant.C_1.sfp_rows_qual_F; };
    const tsp00_Int4   &sfp_rows_read          () const  { return this->variant.C_1.sfp_rows_read_F; };
    const tsp00_Int4   &sfp_m_result_size      () const  { return this->variant.C_1.sfp_m_result_size_F; };
    const tsp00_Int4   &sfp_m_result_len       () const  { return this->variant.C_1.sfp_m_result_len_F; };
    const tsp00_Int4   &sfp_m_result_cnt       () const  { return this->variant.C_1.sfp_m_result_cnt_F; };
    const tsp00_Int2   &sfp_rec_len            () const  { return this->variant.C_1.sfp_rec_len_F; };
    const tsp00_Int2   &sfp_rec_key_len        () const  { return this->variant.C_1.sfp_rec_key_len_F; };
    const tsp00_Int2   &sfp_oldkey_len         () const  { return this->variant.C_1.sfp_oldkey_len_F; };
    const tsp00_Int2   &sfp_oldrec_len         () const  { return this->variant.C_1.sfp_oldrec_len_F; };
    const tsp00_Int2   &sfp_oldrec_pos         () const  { return this->variant.C_1.sfp_oldrec_pos_F; };
    const tsp00_Int2   &sfp_result_length      () const  { return this->variant.C_1.sfp_result_length_F; };
    const tsp00_Int2   &sfp_output_offset      () const  { return this->variant.C_1.sfp_output_offset_F; };
    const tsp00_Int2   &sfp_work_st_max        () const  { return this->variant.C_1.sfp_work_st_max_F; };
    const tsp00_Int2   &sfp_resrec_maxlen      () const  { return this->variant.C_1.sfp_resrec_maxlen_F; };
    const tgg00_MessType &sfp_bd_mess_type       () const  { return this->variant.C_1.sfp_bd_mess_type_F; };
    const tgg00_MessType2 &sfp_bd_mess2_type      () const  { return this->variant.C_1.sfp_bd_mess2_type_F; };
    const pasbool      &sfp_filler             () const  { return this->variant.C_1.sfp_filler_F; };
    const pasbool      &sfp_check_for_result   () const  { return this->variant.C_1.sfp_check_for_result_F; };
    const tgg00_QualKind &sfp_bd_inv_only        () const  { return this->variant.C_1.sfp_bd_inv_only_F; };
    const pasbool      &sfp_bd_return_knf      () const  { return this->variant.C_1.sfp_bd_return_knf_F; };
    const pasbool      &sfp_bd_use_stopkey     () const  { return this->variant.C_1.sfp_bd_use_stopkey_F; };
    const pasbool      &sfp_first_qual         () const  { return this->variant.C_1.sfp_first_qual_F; };
    const pasbool      &sfp_filler2            () const  { return this->variant.C_1.sfp_filler2_F; };
    const pasbool      &sfp_result_wanted      () const  { return this->variant.C_1.sfp_result_wanted_F; };
    const tgg00_VarColPosList &sfp_varcol_pos         () const  { return this->variant.C_1.sfp_varcol_pos_F; };
    const tsp00_Number &sfp_first_serial       () const  { return this->variant.C_1.sfp_first_serial_F; };
    const tsp00_Number &sfp_last_serial        () const  { return this->variant.C_1.sfp_last_serial_F; };
    const tgg00_StEntryAddr &sfp_work_st_top        () const  { return this->variant.C_1.sfp_work_st_top_F; };
    const tgg00_StEntryAddr &sfp_work_st_bottom     () const  { return this->variant.C_1.sfp_work_st_bottom_F; };
    const tgg00_StEntryAddr &sfp_work_st_last       () const  { return this->variant.C_1.sfp_work_st_last_F; };
    const tgg00_StEntryAddr &sfp_work_st_frame      () const  { return this->variant.C_1.sfp_work_st_frame_F; };
    const tsp00_Int4   &sfp_workbuf_top        () const  { return this->variant.C_1.sfp_workbuf_top_F; };
    const pasbool      &sfp_check_new_rec      () const  { return this->variant.C_1.sfp_check_new_rec_F; };
    const pasbool      &sfp_optimized          () const  { return this->variant.C_1.sfp_optimized_F; };
    const tsp00_SqlMode &sfp_sqlmode            () const  { return this->variant.C_1.sfp_sqlmode_F; };
    const tgg00_DateTimeFormat &sfp_dateTimeFormat     () const  { return this->variant.C_1.sfp_dateTimeFormat_F; };
    const tsp00_Addr   &sfp_acv_addr           () const  { return this->variant.C_1.sfp_acv_addr_F; };
    const tsp00_MoveObjPtr &sfp_param_buf_addr     () const  { return this->variant.C_1.sfp_param_buf_addr_F; };
    const tgg00_ClusteredIOParam &sfp_cluster_info       () const  { return this->variant.C_1.sfp_cluster_info_F; };
    const tsp00_MoveObjPtr &sfp_data_addr          () const  { return this->variant.C_2.sfp_data_addr_F; };
    const tsp00_BufAddr &sfp_rec_addr           () const  { return this->variant.C_2.sfp_rec_addr_F; };
    const tsp00_KeyAddr &sfp_primkey_addr       () const  { return this->variant.C_2.sfp_primkey_addr_F; };
    const tgg00_StackListPtr &sfp_work_st_addr       () const  { return this->variant.C_2.sfp_work_st_addr_F; };
    const tsp00_MoveObjPtr &sfp_workbuf_addr       () const  { return this->variant.C_2.sfp_workbuf_addr_F; };
    const tsp00_MoveObjPtr &sfp_oldrec_addr        () const  { return this->variant.C_2.sfp_oldrec_addr_F; };
    const tsp00_BufAddr &sfp_pl_ptr             () const  { return this->variant.C_2.sfp_pl_ptr_F; };
    const tsp00_MoveObjPtr &sfp_m_result_addr      () const  { return this->variant.C_2.sfp_m_result_addr_F; };
    const tgg00_TwoKeysPtr &sfp_m_firstkeys_addr   () const  { return this->variant.C_2.sfp_m_firstkeys_addr_F; };
    const tgg00_FieldListsPtr &sfp_fieldlistptr       () const  { return this->variant.C_2.sfp_fieldlistptr_F; };
    const tgg00_SfpInitPart &sfp_init_part          () const  { return this->variant.C_3.sfp_init_part_F; };
#endif    /* defined (__cplusplus) */
} tgg00_SelectFieldsParam;
typedef tgg00_SelectFieldsParam   *tgg00_SelectParamPtr;

typedef struct                     tgg00_DbSpaceInfo
{
    tsp00_Int4          db_total_pages;
    tsp00_Int4          db_used_pages;
    tsp00_Int4          db_perm_pages;
    tsp00_Int4          db_temp_pages;
    tsp00_Int2          db_perm_percent;
    tsp00_Int2          db_temp_percent;
    tsp00_Int4          db_real_perm_used;
    tsp00_Int4          db_updated_pages;
    tsp00_Int4          db_max_perm_used;
    tsp00_Int4          db_used_blocks;
} tgg00_DbSpaceInfo;
    /* PTS 1003033 E.Z. */
    /* PTS 1111134 E.Z. */

typedef struct                     tgg00_ReferenceInfo
{
    union
    {
    /* true */
        struct tgg00_ReferenceInfo_true
        {
            tsp00_8ByteCounter  data_cache_total_F;
            tsp00_8ByteCounter  data_cache_hit_F;
            tsp00_8ByteCounter  data_cache_miss_F;
            tsp00_8ByteCounter  data_cache_percent_F;
            tsp00_8ByteCounter  ak_cache_total_F;
            tsp00_8ByteCounter  ak_cache_hit_F;
            tsp00_8ByteCounter  ak_cache_miss_F;
            tsp00_8ByteCounter  ak_cache_percent_F;
            tsp00_8ByteCounter  tseq_cache_total_F;
            tsp00_8ByteCounter  tseq_cache_hit_F;
            tsp00_8ByteCounter  tseq_cache_miss_F;
            tsp00_8ByteCounter  tseq_cache_percent_F;
            tsp00_8ByteCounter  data_sql_cache_total_F;
            tsp00_8ByteCounter  data_sql_cache_hit_F;
            tsp00_8ByteCounter  data_sql_cache_miss_F;
            tsp00_8ByteCounter  data_sql_cache_percent_F;
            tsp00_8ByteCounter  data_obj_cache_total_F;
            tsp00_8ByteCounter  data_obj_cache_hit_F;
            tsp00_8ByteCounter  data_obj_cache_miss_F;
            tsp00_8ByteCounter  data_obj_cache_percent_F;
            tsp00_8ByteCounter  data_obj_log_cache_total_F;
            tsp00_8ByteCounter  data_obj_log_cache_hit_F;
            tsp00_8ByteCounter  data_obj_log_cache_miss_F;
            tsp00_8ByteCounter  data_obj_log_cache_percent_F;
            tsp00_8ByteCounter  data_history_cache_total_F;
            tsp00_8ByteCounter  data_history_cache_hit_F;
            tsp00_8ByteCounter  data_history_cache_miss_F;
            tsp00_8ByteCounter  data_history_cache_percent_F;
            tsp00_8ByteCounter  sql_commands_F;
            tsp00_8ByteCounter  prepares_F;
            tsp00_8ByteCounter  prepares_again_F;
            tsp00_8ByteCounter  executes_F;
            tsp00_8ByteCounter  commits_F;
            tsp00_8ByteCounter  rollbacks_F;
            tsp00_8ByteCounter  locks_F;
            tsp00_8ByteCounter  begin_subtrans_F;
            tsp00_8ByteCounter  end_subtrans_F;
            tsp00_8ByteCounter  rollback_subtrans_F;
            tsp00_8ByteCounter  creates_F;
            tsp00_8ByteCounter  alters_F;
            tsp00_8ByteCounter  drops_F;
            tsp00_8ByteCounter  selects_F;
            tsp00_8ByteCounter  selects_rows_read_F;
            tsp00_8ByteCounter  selects_rows_qual_F;
            tsp00_8ByteCounter  inserts_F;
            tsp00_8ByteCounter  inserts_rows_F;
            tsp00_8ByteCounter  updates_F;
            tsp00_8ByteCounter  updates_rows_read_F;
            tsp00_8ByteCounter  updates_rows_upd_F;
            tsp00_8ByteCounter  deletes_F;
            tsp00_8ByteCounter  deletes_rows_read_F;
            tsp00_8ByteCounter  deletes_rows_del_F;
            tsp00_8ByteCounter  internal_dbproc_calls_F;
            tsp00_8ByteCounter  external_dbproc_calls_F;
            tsp00_8ByteCounter  internal_trigger_calls_F;
            tsp00_8ByteCounter  external_trigger_calls_F;
            tsp00_8ByteCounter  prim_key_F;
            tsp00_8ByteCounter  prim_key_in_F;
            tsp00_8ByteCounter  prim_key_subq_F;
            tsp00_8ByteCounter  prim_key_read_F;
            tsp00_8ByteCounter  prim_key_qual_F;
            tsp00_8ByteCounter  prim_key_range_F;
            tsp00_8ByteCounter  prim_key_range_read_F;
            tsp00_8ByteCounter  prim_key_range_qual_F;
            tsp00_8ByteCounter  index_F;
            tsp00_8ByteCounter  index_in_F;
            tsp00_8ByteCounter  index_subq_F;
            tsp00_8ByteCounter  index_scan_F;
            tsp00_8ByteCounter  index_read_F;
            tsp00_8ByteCounter  index_qual_F;
            tsp00_8ByteCounter  index_range_F;
            tsp00_8ByteCounter  index_range_read_F;
            tsp00_8ByteCounter  index_range_qual_F;
            tsp00_8ByteCounter  is_index_F;
            tsp00_8ByteCounter  is_index_in_F;
            tsp00_8ByteCounter  is_index_subq_F;
            tsp00_8ByteCounter  is_index_read_F;
            tsp00_8ByteCounter  is_index_qual_F;
            tsp00_8ByteCounter  is_index_range_F;
            tsp00_8ByteCounter  is_index_range_read_F;
            tsp00_8ByteCounter  is_index_range_qual_F;
            tsp00_8ByteCounter  table_scan_F;
            tsp00_8ByteCounter  table_scan_read_F;
            tsp00_8ByteCounter  table_scan_qual_F;
            tsp00_8ByteCounter  is_index_scan_F;
            tsp00_8ByteCounter  is_index_scan_read_F;
            tsp00_8ByteCounter  is_index_scan_qual_F;
            tsp00_8ByteCounter  mem_sorts_F;
            tsp00_8ByteCounter  mem_sorts_rows_read_F;
            tsp00_8ByteCounter  ins_sorts_F;
            tsp00_8ByteCounter  ins_sorts_rows_ins_F;
            tsp00_8ByteCounter  join_hash_F;
            tsp00_8ByteCounter  join_parallel_F;
            tsp00_8ByteCounter  join_legacy_F;
            tsp00_8ByteCounter  join_op_normal_F;
            tsp00_8ByteCounter  join_op_improved_F;
            tsp00_8ByteCounter  avg_lock_used_F;
            tsp00_8ByteCounter  max_lock_used_F;
            tsp00_8ByteCounter  collisions_F;
            tsp00_8ByteCounter  escalations_F;
            tsp00_8ByteCounter  new_rows_F;
            tsp00_8ByteCounter  new_tabs_F;
            tsp00_8ByteCounter  deadlocks_F;
            tsp00_8ByteCounter  request_timeouts_F;
            tsp00_8ByteCounter  read_log_F;
            tsp00_8ByteCounter  write_log_F;
            ARRAY_VAR_MSP00 (         kb1_monitor_F, tsp00_8ByteCounter, 8);
            tsp00_8ByteCounter  read_string_F;
            tsp00_8ByteCounter  write_string_F;
            tsp00_8ByteCounter  r_v_acc_F;
            tsp00_8ByteCounter  w_v_acc_F;
            tsp00_8ByteCounter  r_p_acc_F;
            tsp00_8ByteCounter  w_p_acc_F;
            tsp00_8ByteCounter  catalog_r_v_F;
            tsp00_8ByteCounter  catalog_w_v_F;
            tsp00_8ByteCounter  catalog_r_p_F;
            tsp00_8ByteCounter  catalog_w_p_F;
            tsp00_8ByteCounter  perm_r_v_F;
            tsp00_8ByteCounter  perm_w_v_F;
            tsp00_8ByteCounter  perm_r_p_F;
            tsp00_8ByteCounter  perm_w_p_F;
            tsp00_8ByteCounter  temp_r_v_F;
            tsp00_8ByteCounter  temp_w_v_F;
            tsp00_8ByteCounter  temp_r_p_F;
            tsp00_8ByteCounter  temp_w_p_F;
            tsp00_8ByteCounter  bstr_r_v_F;
            tsp00_8ByteCounter  bstr_w_v_F;
            tsp00_8ByteCounter  bstr_r_p_F;
            tsp00_8ByteCounter  bstr_w_p_F;
            tsp00_8ByteCounter  level0_r_v_F;
            tsp00_8ByteCounter  level0_w_v_F;
            tsp00_8ByteCounter  level0_r_p_F;
            tsp00_8ByteCounter  level0_w_p_F;
            tsp00_8ByteCounter  level1_r_v_F;
            tsp00_8ByteCounter  level1_w_v_F;
            tsp00_8ByteCounter  level1_r_p_F;
            tsp00_8ByteCounter  level1_w_p_F;
            tsp00_8ByteCounter  level2_r_v_F;
            tsp00_8ByteCounter  level2_w_v_F;
            tsp00_8ByteCounter  level2_r_p_F;
            tsp00_8ByteCounter  level2_w_p_F;
            tsp00_8ByteCounter  level3_r_v_F;
            tsp00_8ByteCounter  level3_w_v_F;
            tsp00_8ByteCounter  level3_r_p_F;
            tsp00_8ByteCounter  level3_w_p_F;
            tsp00_8ByteCounter  level4_r_v_F;
            tsp00_8ByteCounter  level4_w_v_F;
            tsp00_8ByteCounter  level4_r_p_F;
            tsp00_8ByteCounter  level4_w_p_F;
            tsp00_8ByteCounter  level5_r_v_F;
            tsp00_8ByteCounter  level5_w_v_F;
            tsp00_8ByteCounter  level5_r_p_F;
            tsp00_8ByteCounter  level5_w_p_F;
            tsp00_8ByteCounter  psm_r_v_F;
            tsp00_8ByteCounter  psm_w_v_F;
            tsp00_8ByteCounter  psm_r_p_F;
            tsp00_8ByteCounter  psm_w_p_F;
            tsp00_8ByteCounter  add_rec_F;
            tsp00_8ByteCounter  cadd_rec_F;
            tsp00_8ByteCounter  repl_rec_F;
            tsp00_8ByteCounter  crepl_rec_F;
            tsp00_8ByteCounter  del_rec_F;
            tsp00_8ByteCounter  cdel_rec_F;
            tsp00_8ByteCounter  get_rec_F;
            tsp00_8ByteCounter  cget_rec_F;
            tsp00_8ByteCounter  next_rec_F;
            tsp00_8ByteCounter  cnext_rec_F;
            tsp00_8ByteCounter  prev_rec_F;
            tsp00_8ByteCounter  cprev_rec_F;
            tsp00_8ByteCounter  sel_direc_F;
            tsp00_8ByteCounter  sel_next_F;
            tsp00_8ByteCounter  sel_prev_F;
            tsp00_8ByteCounter  add_inv_F;
            tsp00_8ByteCounter  cadd_inv_F;
            tsp00_8ByteCounter  del_inv_F;
            tsp00_8ByteCounter  cdel_inv_F;
            tsp00_8ByteCounter  get_inv_F;
            tsp00_8ByteCounter  get_inv_parallel_F;
            tsp00_8ByteCounter  cget_inv_F;
            tsp00_8ByteCounter  funccount_F;
            tsp00_8ByteCounter  transcount_F;
            tsp00_8ByteCounter  kb_calls_F;
        } C_true;
    /* false */
        struct tgg00_ReferenceInfo_false
        {
            ARRAY_VAR_MSP00 (         reference_array_F, tsp00_8ByteCounter, iunused_but_needed - 0 + 1);
        } C_false;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tsp00_8ByteCounter &data_cache_total       ()  { return this->variant.C_true.data_cache_total_F; };
    tsp00_8ByteCounter &data_cache_hit         ()  { return this->variant.C_true.data_cache_hit_F; };
    tsp00_8ByteCounter &data_cache_miss        ()  { return this->variant.C_true.data_cache_miss_F; };
    tsp00_8ByteCounter &data_cache_percent     ()  { return this->variant.C_true.data_cache_percent_F; };
    tsp00_8ByteCounter &ak_cache_total         ()  { return this->variant.C_true.ak_cache_total_F; };
    tsp00_8ByteCounter &ak_cache_hit           ()  { return this->variant.C_true.ak_cache_hit_F; };
    tsp00_8ByteCounter &ak_cache_miss          ()  { return this->variant.C_true.ak_cache_miss_F; };
    tsp00_8ByteCounter &ak_cache_percent       ()  { return this->variant.C_true.ak_cache_percent_F; };
    tsp00_8ByteCounter &tseq_cache_total       ()  { return this->variant.C_true.tseq_cache_total_F; };
    tsp00_8ByteCounter &tseq_cache_hit         ()  { return this->variant.C_true.tseq_cache_hit_F; };
    tsp00_8ByteCounter &tseq_cache_miss        ()  { return this->variant.C_true.tseq_cache_miss_F; };
    tsp00_8ByteCounter &tseq_cache_percent     ()  { return this->variant.C_true.tseq_cache_percent_F; };
    tsp00_8ByteCounter &data_sql_cache_total   ()  { return this->variant.C_true.data_sql_cache_total_F; };
    tsp00_8ByteCounter &data_sql_cache_hit     ()  { return this->variant.C_true.data_sql_cache_hit_F; };
    tsp00_8ByteCounter &data_sql_cache_miss    ()  { return this->variant.C_true.data_sql_cache_miss_F; };
    tsp00_8ByteCounter &data_sql_cache_percent ()  { return this->variant.C_true.data_sql_cache_percent_F; };
    tsp00_8ByteCounter &data_obj_cache_total   ()  { return this->variant.C_true.data_obj_cache_total_F; };
    tsp00_8ByteCounter &data_obj_cache_hit     ()  { return this->variant.C_true.data_obj_cache_hit_F; };
    tsp00_8ByteCounter &data_obj_cache_miss    ()  { return this->variant.C_true.data_obj_cache_miss_F; };
    tsp00_8ByteCounter &data_obj_cache_percent ()  { return this->variant.C_true.data_obj_cache_percent_F; };
    tsp00_8ByteCounter &data_obj_log_cache_total ()  { return this->variant.C_true.data_obj_log_cache_total_F; };
    tsp00_8ByteCounter &data_obj_log_cache_hit ()  { return this->variant.C_true.data_obj_log_cache_hit_F; };
    tsp00_8ByteCounter &data_obj_log_cache_miss ()  { return this->variant.C_true.data_obj_log_cache_miss_F; };
    tsp00_8ByteCounter &data_obj_log_cache_percent ()  { return this->variant.C_true.data_obj_log_cache_percent_F; };
    tsp00_8ByteCounter &data_history_cache_total ()  { return this->variant.C_true.data_history_cache_total_F; };
    tsp00_8ByteCounter &data_history_cache_hit ()  { return this->variant.C_true.data_history_cache_hit_F; };
    tsp00_8ByteCounter &data_history_cache_miss ()  { return this->variant.C_true.data_history_cache_miss_F; };
    tsp00_8ByteCounter &data_history_cache_percent ()  { return this->variant.C_true.data_history_cache_percent_F; };
    tsp00_8ByteCounter &sql_commands           ()  { return this->variant.C_true.sql_commands_F; };
    tsp00_8ByteCounter &prepares               ()  { return this->variant.C_true.prepares_F; };
    tsp00_8ByteCounter &prepares_again         ()  { return this->variant.C_true.prepares_again_F; };
    tsp00_8ByteCounter &executes               ()  { return this->variant.C_true.executes_F; };
    tsp00_8ByteCounter &commits                ()  { return this->variant.C_true.commits_F; };
    tsp00_8ByteCounter &rollbacks              ()  { return this->variant.C_true.rollbacks_F; };
    tsp00_8ByteCounter &locks                  ()  { return this->variant.C_true.locks_F; };
    tsp00_8ByteCounter &begin_subtrans         ()  { return this->variant.C_true.begin_subtrans_F; };
    tsp00_8ByteCounter &end_subtrans           ()  { return this->variant.C_true.end_subtrans_F; };
    tsp00_8ByteCounter &rollback_subtrans      ()  { return this->variant.C_true.rollback_subtrans_F; };
    tsp00_8ByteCounter &creates                ()  { return this->variant.C_true.creates_F; };
    tsp00_8ByteCounter &alters                 ()  { return this->variant.C_true.alters_F; };
    tsp00_8ByteCounter &drops                  ()  { return this->variant.C_true.drops_F; };
    tsp00_8ByteCounter &selects                ()  { return this->variant.C_true.selects_F; };
    tsp00_8ByteCounter &selects_rows_read      ()  { return this->variant.C_true.selects_rows_read_F; };
    tsp00_8ByteCounter &selects_rows_qual      ()  { return this->variant.C_true.selects_rows_qual_F; };
    tsp00_8ByteCounter &inserts                ()  { return this->variant.C_true.inserts_F; };
    tsp00_8ByteCounter &inserts_rows           ()  { return this->variant.C_true.inserts_rows_F; };
    tsp00_8ByteCounter &updates                ()  { return this->variant.C_true.updates_F; };
    tsp00_8ByteCounter &updates_rows_read      ()  { return this->variant.C_true.updates_rows_read_F; };
    tsp00_8ByteCounter &updates_rows_upd       ()  { return this->variant.C_true.updates_rows_upd_F; };
    tsp00_8ByteCounter &deletes                ()  { return this->variant.C_true.deletes_F; };
    tsp00_8ByteCounter &deletes_rows_read      ()  { return this->variant.C_true.deletes_rows_read_F; };
    tsp00_8ByteCounter &deletes_rows_del       ()  { return this->variant.C_true.deletes_rows_del_F; };
    tsp00_8ByteCounter &internal_dbproc_calls  ()  { return this->variant.C_true.internal_dbproc_calls_F; };
    tsp00_8ByteCounter &external_dbproc_calls  ()  { return this->variant.C_true.external_dbproc_calls_F; };
    tsp00_8ByteCounter &internal_trigger_calls ()  { return this->variant.C_true.internal_trigger_calls_F; };
    tsp00_8ByteCounter &external_trigger_calls ()  { return this->variant.C_true.external_trigger_calls_F; };
    tsp00_8ByteCounter &prim_key               ()  { return this->variant.C_true.prim_key_F; };
    tsp00_8ByteCounter &prim_key_in            ()  { return this->variant.C_true.prim_key_in_F; };
    tsp00_8ByteCounter &prim_key_subq          ()  { return this->variant.C_true.prim_key_subq_F; };
    tsp00_8ByteCounter &prim_key_read          ()  { return this->variant.C_true.prim_key_read_F; };
    tsp00_8ByteCounter &prim_key_qual          ()  { return this->variant.C_true.prim_key_qual_F; };
    tsp00_8ByteCounter &prim_key_range         ()  { return this->variant.C_true.prim_key_range_F; };
    tsp00_8ByteCounter &prim_key_range_read    ()  { return this->variant.C_true.prim_key_range_read_F; };
    tsp00_8ByteCounter &prim_key_range_qual    ()  { return this->variant.C_true.prim_key_range_qual_F; };
    tsp00_8ByteCounter &index                  ()  { return this->variant.C_true.index_F; };
    tsp00_8ByteCounter &index_in               ()  { return this->variant.C_true.index_in_F; };
    tsp00_8ByteCounter &index_subq             ()  { return this->variant.C_true.index_subq_F; };
    tsp00_8ByteCounter &index_scan             ()  { return this->variant.C_true.index_scan_F; };
    tsp00_8ByteCounter &index_read             ()  { return this->variant.C_true.index_read_F; };
    tsp00_8ByteCounter &index_qual             ()  { return this->variant.C_true.index_qual_F; };
    tsp00_8ByteCounter &index_range            ()  { return this->variant.C_true.index_range_F; };
    tsp00_8ByteCounter &index_range_read       ()  { return this->variant.C_true.index_range_read_F; };
    tsp00_8ByteCounter &index_range_qual       ()  { return this->variant.C_true.index_range_qual_F; };
    tsp00_8ByteCounter &is_index               ()  { return this->variant.C_true.is_index_F; };
    tsp00_8ByteCounter &is_index_in            ()  { return this->variant.C_true.is_index_in_F; };
    tsp00_8ByteCounter &is_index_subq          ()  { return this->variant.C_true.is_index_subq_F; };
    tsp00_8ByteCounter &is_index_read          ()  { return this->variant.C_true.is_index_read_F; };
    tsp00_8ByteCounter &is_index_qual          ()  { return this->variant.C_true.is_index_qual_F; };
    tsp00_8ByteCounter &is_index_range         ()  { return this->variant.C_true.is_index_range_F; };
    tsp00_8ByteCounter &is_index_range_read    ()  { return this->variant.C_true.is_index_range_read_F; };
    tsp00_8ByteCounter &is_index_range_qual    ()  { return this->variant.C_true.is_index_range_qual_F; };
    tsp00_8ByteCounter &table_scan             ()  { return this->variant.C_true.table_scan_F; };
    tsp00_8ByteCounter &table_scan_read        ()  { return this->variant.C_true.table_scan_read_F; };
    tsp00_8ByteCounter &table_scan_qual        ()  { return this->variant.C_true.table_scan_qual_F; };
    tsp00_8ByteCounter &is_index_scan          ()  { return this->variant.C_true.is_index_scan_F; };
    tsp00_8ByteCounter &is_index_scan_read     ()  { return this->variant.C_true.is_index_scan_read_F; };
    tsp00_8ByteCounter &is_index_scan_qual     ()  { return this->variant.C_true.is_index_scan_qual_F; };
    tsp00_8ByteCounter &mem_sorts              ()  { return this->variant.C_true.mem_sorts_F; };
    tsp00_8ByteCounter &mem_sorts_rows_read    ()  { return this->variant.C_true.mem_sorts_rows_read_F; };
    tsp00_8ByteCounter &ins_sorts              ()  { return this->variant.C_true.ins_sorts_F; };
    tsp00_8ByteCounter &ins_sorts_rows_ins     ()  { return this->variant.C_true.ins_sorts_rows_ins_F; };
    tsp00_8ByteCounter &join_hash              ()  { return this->variant.C_true.join_hash_F; };
    tsp00_8ByteCounter &join_parallel          ()  { return this->variant.C_true.join_parallel_F; };
    tsp00_8ByteCounter &join_legacy            ()  { return this->variant.C_true.join_legacy_F; };
    tsp00_8ByteCounter &join_op_normal         ()  { return this->variant.C_true.join_op_normal_F; };
    tsp00_8ByteCounter &join_op_improved       ()  { return this->variant.C_true.join_op_improved_F; };
    tsp00_8ByteCounter &avg_lock_used          ()  { return this->variant.C_true.avg_lock_used_F; };
    tsp00_8ByteCounter &max_lock_used          ()  { return this->variant.C_true.max_lock_used_F; };
    tsp00_8ByteCounter &collisions             ()  { return this->variant.C_true.collisions_F; };
    tsp00_8ByteCounter &escalations            ()  { return this->variant.C_true.escalations_F; };
    tsp00_8ByteCounter &new_rows               ()  { return this->variant.C_true.new_rows_F; };
    tsp00_8ByteCounter &new_tabs               ()  { return this->variant.C_true.new_tabs_F; };
    tsp00_8ByteCounter &deadlocks              ()  { return this->variant.C_true.deadlocks_F; };
    tsp00_8ByteCounter &request_timeouts       ()  { return this->variant.C_true.request_timeouts_F; };
    tsp00_8ByteCounter &read_log               ()  { return this->variant.C_true.read_log_F; };
    tsp00_8ByteCounter &write_log              ()  { return this->variant.C_true.write_log_F; };
    ARRAY_TYPESTRING_MSP00(tsp00_8ByteCounter, 8) &kb1_monitor            ()  { return this->variant.C_true.kb1_monitor_F; };
    tsp00_8ByteCounter &read_string            ()  { return this->variant.C_true.read_string_F; };
    tsp00_8ByteCounter &write_string           ()  { return this->variant.C_true.write_string_F; };
    tsp00_8ByteCounter &r_v_acc                ()  { return this->variant.C_true.r_v_acc_F; };
    tsp00_8ByteCounter &w_v_acc                ()  { return this->variant.C_true.w_v_acc_F; };
    tsp00_8ByteCounter &r_p_acc                ()  { return this->variant.C_true.r_p_acc_F; };
    tsp00_8ByteCounter &w_p_acc                ()  { return this->variant.C_true.w_p_acc_F; };
    tsp00_8ByteCounter &catalog_r_v            ()  { return this->variant.C_true.catalog_r_v_F; };
    tsp00_8ByteCounter &catalog_w_v            ()  { return this->variant.C_true.catalog_w_v_F; };
    tsp00_8ByteCounter &catalog_r_p            ()  { return this->variant.C_true.catalog_r_p_F; };
    tsp00_8ByteCounter &catalog_w_p            ()  { return this->variant.C_true.catalog_w_p_F; };
    tsp00_8ByteCounter &perm_r_v               ()  { return this->variant.C_true.perm_r_v_F; };
    tsp00_8ByteCounter &perm_w_v               ()  { return this->variant.C_true.perm_w_v_F; };
    tsp00_8ByteCounter &perm_r_p               ()  { return this->variant.C_true.perm_r_p_F; };
    tsp00_8ByteCounter &perm_w_p               ()  { return this->variant.C_true.perm_w_p_F; };
    tsp00_8ByteCounter &temp_r_v               ()  { return this->variant.C_true.temp_r_v_F; };
    tsp00_8ByteCounter &temp_w_v               ()  { return this->variant.C_true.temp_w_v_F; };
    tsp00_8ByteCounter &temp_r_p               ()  { return this->variant.C_true.temp_r_p_F; };
    tsp00_8ByteCounter &temp_w_p               ()  { return this->variant.C_true.temp_w_p_F; };
    tsp00_8ByteCounter &bstr_r_v               ()  { return this->variant.C_true.bstr_r_v_F; };
    tsp00_8ByteCounter &bstr_w_v               ()  { return this->variant.C_true.bstr_w_v_F; };
    tsp00_8ByteCounter &bstr_r_p               ()  { return this->variant.C_true.bstr_r_p_F; };
    tsp00_8ByteCounter &bstr_w_p               ()  { return this->variant.C_true.bstr_w_p_F; };
    tsp00_8ByteCounter &level0_r_v             ()  { return this->variant.C_true.level0_r_v_F; };
    tsp00_8ByteCounter &level0_w_v             ()  { return this->variant.C_true.level0_w_v_F; };
    tsp00_8ByteCounter &level0_r_p             ()  { return this->variant.C_true.level0_r_p_F; };
    tsp00_8ByteCounter &level0_w_p             ()  { return this->variant.C_true.level0_w_p_F; };
    tsp00_8ByteCounter &level1_r_v             ()  { return this->variant.C_true.level1_r_v_F; };
    tsp00_8ByteCounter &level1_w_v             ()  { return this->variant.C_true.level1_w_v_F; };
    tsp00_8ByteCounter &level1_r_p             ()  { return this->variant.C_true.level1_r_p_F; };
    tsp00_8ByteCounter &level1_w_p             ()  { return this->variant.C_true.level1_w_p_F; };
    tsp00_8ByteCounter &level2_r_v             ()  { return this->variant.C_true.level2_r_v_F; };
    tsp00_8ByteCounter &level2_w_v             ()  { return this->variant.C_true.level2_w_v_F; };
    tsp00_8ByteCounter &level2_r_p             ()  { return this->variant.C_true.level2_r_p_F; };
    tsp00_8ByteCounter &level2_w_p             ()  { return this->variant.C_true.level2_w_p_F; };
    tsp00_8ByteCounter &level3_r_v             ()  { return this->variant.C_true.level3_r_v_F; };
    tsp00_8ByteCounter &level3_w_v             ()  { return this->variant.C_true.level3_w_v_F; };
    tsp00_8ByteCounter &level3_r_p             ()  { return this->variant.C_true.level3_r_p_F; };
    tsp00_8ByteCounter &level3_w_p             ()  { return this->variant.C_true.level3_w_p_F; };
    tsp00_8ByteCounter &level4_r_v             ()  { return this->variant.C_true.level4_r_v_F; };
    tsp00_8ByteCounter &level4_w_v             ()  { return this->variant.C_true.level4_w_v_F; };
    tsp00_8ByteCounter &level4_r_p             ()  { return this->variant.C_true.level4_r_p_F; };
    tsp00_8ByteCounter &level4_w_p             ()  { return this->variant.C_true.level4_w_p_F; };
    tsp00_8ByteCounter &level5_r_v             ()  { return this->variant.C_true.level5_r_v_F; };
    tsp00_8ByteCounter &level5_w_v             ()  { return this->variant.C_true.level5_w_v_F; };
    tsp00_8ByteCounter &level5_r_p             ()  { return this->variant.C_true.level5_r_p_F; };
    tsp00_8ByteCounter &level5_w_p             ()  { return this->variant.C_true.level5_w_p_F; };
    tsp00_8ByteCounter &psm_r_v                ()  { return this->variant.C_true.psm_r_v_F; };
    tsp00_8ByteCounter &psm_w_v                ()  { return this->variant.C_true.psm_w_v_F; };
    tsp00_8ByteCounter &psm_r_p                ()  { return this->variant.C_true.psm_r_p_F; };
    tsp00_8ByteCounter &psm_w_p                ()  { return this->variant.C_true.psm_w_p_F; };
    tsp00_8ByteCounter &add_rec                ()  { return this->variant.C_true.add_rec_F; };
    tsp00_8ByteCounter &cadd_rec               ()  { return this->variant.C_true.cadd_rec_F; };
    tsp00_8ByteCounter &repl_rec               ()  { return this->variant.C_true.repl_rec_F; };
    tsp00_8ByteCounter &crepl_rec              ()  { return this->variant.C_true.crepl_rec_F; };
    tsp00_8ByteCounter &del_rec                ()  { return this->variant.C_true.del_rec_F; };
    tsp00_8ByteCounter &cdel_rec               ()  { return this->variant.C_true.cdel_rec_F; };
    tsp00_8ByteCounter &get_rec                ()  { return this->variant.C_true.get_rec_F; };
    tsp00_8ByteCounter &cget_rec               ()  { return this->variant.C_true.cget_rec_F; };
    tsp00_8ByteCounter &next_rec               ()  { return this->variant.C_true.next_rec_F; };
    tsp00_8ByteCounter &cnext_rec              ()  { return this->variant.C_true.cnext_rec_F; };
    tsp00_8ByteCounter &prev_rec               ()  { return this->variant.C_true.prev_rec_F; };
    tsp00_8ByteCounter &cprev_rec              ()  { return this->variant.C_true.cprev_rec_F; };
    tsp00_8ByteCounter &sel_direc              ()  { return this->variant.C_true.sel_direc_F; };
    tsp00_8ByteCounter &sel_next               ()  { return this->variant.C_true.sel_next_F; };
    tsp00_8ByteCounter &sel_prev               ()  { return this->variant.C_true.sel_prev_F; };
    tsp00_8ByteCounter &add_inv                ()  { return this->variant.C_true.add_inv_F; };
    tsp00_8ByteCounter &cadd_inv               ()  { return this->variant.C_true.cadd_inv_F; };
    tsp00_8ByteCounter &del_inv                ()  { return this->variant.C_true.del_inv_F; };
    tsp00_8ByteCounter &cdel_inv               ()  { return this->variant.C_true.cdel_inv_F; };
    tsp00_8ByteCounter &get_inv                ()  { return this->variant.C_true.get_inv_F; };
    tsp00_8ByteCounter &get_inv_parallel       ()  { return this->variant.C_true.get_inv_parallel_F; };
    tsp00_8ByteCounter &cget_inv               ()  { return this->variant.C_true.cget_inv_F; };
    tsp00_8ByteCounter &funccount              ()  { return this->variant.C_true.funccount_F; };
    tsp00_8ByteCounter &transcount             ()  { return this->variant.C_true.transcount_F; };
    tsp00_8ByteCounter &kb_calls               ()  { return this->variant.C_true.kb_calls_F; };
    ARRAY_TYPESTRING_MSP00(tsp00_8ByteCounter, iunused_but_needed - 0 + 1) &reference_array        ()  { return this->variant.C_false.reference_array_F; };
    /* const accessor functions */
    const tsp00_8ByteCounter &data_cache_total       () const  { return this->variant.C_true.data_cache_total_F; };
    const tsp00_8ByteCounter &data_cache_hit         () const  { return this->variant.C_true.data_cache_hit_F; };
    const tsp00_8ByteCounter &data_cache_miss        () const  { return this->variant.C_true.data_cache_miss_F; };
    const tsp00_8ByteCounter &data_cache_percent     () const  { return this->variant.C_true.data_cache_percent_F; };
    const tsp00_8ByteCounter &ak_cache_total         () const  { return this->variant.C_true.ak_cache_total_F; };
    const tsp00_8ByteCounter &ak_cache_hit           () const  { return this->variant.C_true.ak_cache_hit_F; };
    const tsp00_8ByteCounter &ak_cache_miss          () const  { return this->variant.C_true.ak_cache_miss_F; };
    const tsp00_8ByteCounter &ak_cache_percent       () const  { return this->variant.C_true.ak_cache_percent_F; };
    const tsp00_8ByteCounter &tseq_cache_total       () const  { return this->variant.C_true.tseq_cache_total_F; };
    const tsp00_8ByteCounter &tseq_cache_hit         () const  { return this->variant.C_true.tseq_cache_hit_F; };
    const tsp00_8ByteCounter &tseq_cache_miss        () const  { return this->variant.C_true.tseq_cache_miss_F; };
    const tsp00_8ByteCounter &tseq_cache_percent     () const  { return this->variant.C_true.tseq_cache_percent_F; };
    const tsp00_8ByteCounter &data_sql_cache_total   () const  { return this->variant.C_true.data_sql_cache_total_F; };
    const tsp00_8ByteCounter &data_sql_cache_hit     () const  { return this->variant.C_true.data_sql_cache_hit_F; };
    const tsp00_8ByteCounter &data_sql_cache_miss    () const  { return this->variant.C_true.data_sql_cache_miss_F; };
    const tsp00_8ByteCounter &data_sql_cache_percent () const  { return this->variant.C_true.data_sql_cache_percent_F; };
    const tsp00_8ByteCounter &data_obj_cache_total   () const  { return this->variant.C_true.data_obj_cache_total_F; };
    const tsp00_8ByteCounter &data_obj_cache_hit     () const  { return this->variant.C_true.data_obj_cache_hit_F; };
    const tsp00_8ByteCounter &data_obj_cache_miss    () const  { return this->variant.C_true.data_obj_cache_miss_F; };
    const tsp00_8ByteCounter &data_obj_cache_percent () const  { return this->variant.C_true.data_obj_cache_percent_F; };
    const tsp00_8ByteCounter &data_obj_log_cache_total () const  { return this->variant.C_true.data_obj_log_cache_total_F; };
    const tsp00_8ByteCounter &data_obj_log_cache_hit () const  { return this->variant.C_true.data_obj_log_cache_hit_F; };
    const tsp00_8ByteCounter &data_obj_log_cache_miss () const  { return this->variant.C_true.data_obj_log_cache_miss_F; };
    const tsp00_8ByteCounter &data_obj_log_cache_percent () const  { return this->variant.C_true.data_obj_log_cache_percent_F; };
    const tsp00_8ByteCounter &data_history_cache_total () const  { return this->variant.C_true.data_history_cache_total_F; };
    const tsp00_8ByteCounter &data_history_cache_hit () const  { return this->variant.C_true.data_history_cache_hit_F; };
    const tsp00_8ByteCounter &data_history_cache_miss () const  { return this->variant.C_true.data_history_cache_miss_F; };
    const tsp00_8ByteCounter &data_history_cache_percent () const  { return this->variant.C_true.data_history_cache_percent_F; };
    const tsp00_8ByteCounter &sql_commands           () const  { return this->variant.C_true.sql_commands_F; };
    const tsp00_8ByteCounter &prepares               () const  { return this->variant.C_true.prepares_F; };
    const tsp00_8ByteCounter &prepares_again         () const  { return this->variant.C_true.prepares_again_F; };
    const tsp00_8ByteCounter &executes               () const  { return this->variant.C_true.executes_F; };
    const tsp00_8ByteCounter &commits                () const  { return this->variant.C_true.commits_F; };
    const tsp00_8ByteCounter &rollbacks              () const  { return this->variant.C_true.rollbacks_F; };
    const tsp00_8ByteCounter &locks                  () const  { return this->variant.C_true.locks_F; };
    const tsp00_8ByteCounter &begin_subtrans         () const  { return this->variant.C_true.begin_subtrans_F; };
    const tsp00_8ByteCounter &end_subtrans           () const  { return this->variant.C_true.end_subtrans_F; };
    const tsp00_8ByteCounter &rollback_subtrans      () const  { return this->variant.C_true.rollback_subtrans_F; };
    const tsp00_8ByteCounter &creates                () const  { return this->variant.C_true.creates_F; };
    const tsp00_8ByteCounter &alters                 () const  { return this->variant.C_true.alters_F; };
    const tsp00_8ByteCounter &drops                  () const  { return this->variant.C_true.drops_F; };
    const tsp00_8ByteCounter &selects                () const  { return this->variant.C_true.selects_F; };
    const tsp00_8ByteCounter &selects_rows_read      () const  { return this->variant.C_true.selects_rows_read_F; };
    const tsp00_8ByteCounter &selects_rows_qual      () const  { return this->variant.C_true.selects_rows_qual_F; };
    const tsp00_8ByteCounter &inserts                () const  { return this->variant.C_true.inserts_F; };
    const tsp00_8ByteCounter &inserts_rows           () const  { return this->variant.C_true.inserts_rows_F; };
    const tsp00_8ByteCounter &updates                () const  { return this->variant.C_true.updates_F; };
    const tsp00_8ByteCounter &updates_rows_read      () const  { return this->variant.C_true.updates_rows_read_F; };
    const tsp00_8ByteCounter &updates_rows_upd       () const  { return this->variant.C_true.updates_rows_upd_F; };
    const tsp00_8ByteCounter &deletes                () const  { return this->variant.C_true.deletes_F; };
    const tsp00_8ByteCounter &deletes_rows_read      () const  { return this->variant.C_true.deletes_rows_read_F; };
    const tsp00_8ByteCounter &deletes_rows_del       () const  { return this->variant.C_true.deletes_rows_del_F; };
    const tsp00_8ByteCounter &internal_dbproc_calls  () const  { return this->variant.C_true.internal_dbproc_calls_F; };
    const tsp00_8ByteCounter &external_dbproc_calls  () const  { return this->variant.C_true.external_dbproc_calls_F; };
    const tsp00_8ByteCounter &internal_trigger_calls () const  { return this->variant.C_true.internal_trigger_calls_F; };
    const tsp00_8ByteCounter &external_trigger_calls () const  { return this->variant.C_true.external_trigger_calls_F; };
    const tsp00_8ByteCounter &prim_key               () const  { return this->variant.C_true.prim_key_F; };
    const tsp00_8ByteCounter &prim_key_in            () const  { return this->variant.C_true.prim_key_in_F; };
    const tsp00_8ByteCounter &prim_key_subq          () const  { return this->variant.C_true.prim_key_subq_F; };
    const tsp00_8ByteCounter &prim_key_read          () const  { return this->variant.C_true.prim_key_read_F; };
    const tsp00_8ByteCounter &prim_key_qual          () const  { return this->variant.C_true.prim_key_qual_F; };
    const tsp00_8ByteCounter &prim_key_range         () const  { return this->variant.C_true.prim_key_range_F; };
    const tsp00_8ByteCounter &prim_key_range_read    () const  { return this->variant.C_true.prim_key_range_read_F; };
    const tsp00_8ByteCounter &prim_key_range_qual    () const  { return this->variant.C_true.prim_key_range_qual_F; };
    const tsp00_8ByteCounter &index                  () const  { return this->variant.C_true.index_F; };
    const tsp00_8ByteCounter &index_in               () const  { return this->variant.C_true.index_in_F; };
    const tsp00_8ByteCounter &index_subq             () const  { return this->variant.C_true.index_subq_F; };
    const tsp00_8ByteCounter &index_scan             () const  { return this->variant.C_true.index_scan_F; };
    const tsp00_8ByteCounter &index_read             () const  { return this->variant.C_true.index_read_F; };
    const tsp00_8ByteCounter &index_qual             () const  { return this->variant.C_true.index_qual_F; };
    const tsp00_8ByteCounter &index_range            () const  { return this->variant.C_true.index_range_F; };
    const tsp00_8ByteCounter &index_range_read       () const  { return this->variant.C_true.index_range_read_F; };
    const tsp00_8ByteCounter &index_range_qual       () const  { return this->variant.C_true.index_range_qual_F; };
    const tsp00_8ByteCounter &is_index               () const  { return this->variant.C_true.is_index_F; };
    const tsp00_8ByteCounter &is_index_in            () const  { return this->variant.C_true.is_index_in_F; };
    const tsp00_8ByteCounter &is_index_subq          () const  { return this->variant.C_true.is_index_subq_F; };
    const tsp00_8ByteCounter &is_index_read          () const  { return this->variant.C_true.is_index_read_F; };
    const tsp00_8ByteCounter &is_index_qual          () const  { return this->variant.C_true.is_index_qual_F; };
    const tsp00_8ByteCounter &is_index_range         () const  { return this->variant.C_true.is_index_range_F; };
    const tsp00_8ByteCounter &is_index_range_read    () const  { return this->variant.C_true.is_index_range_read_F; };
    const tsp00_8ByteCounter &is_index_range_qual    () const  { return this->variant.C_true.is_index_range_qual_F; };
    const tsp00_8ByteCounter &table_scan             () const  { return this->variant.C_true.table_scan_F; };
    const tsp00_8ByteCounter &table_scan_read        () const  { return this->variant.C_true.table_scan_read_F; };
    const tsp00_8ByteCounter &table_scan_qual        () const  { return this->variant.C_true.table_scan_qual_F; };
    const tsp00_8ByteCounter &is_index_scan          () const  { return this->variant.C_true.is_index_scan_F; };
    const tsp00_8ByteCounter &is_index_scan_read     () const  { return this->variant.C_true.is_index_scan_read_F; };
    const tsp00_8ByteCounter &is_index_scan_qual     () const  { return this->variant.C_true.is_index_scan_qual_F; };
    const tsp00_8ByteCounter &mem_sorts              () const  { return this->variant.C_true.mem_sorts_F; };
    const tsp00_8ByteCounter &mem_sorts_rows_read    () const  { return this->variant.C_true.mem_sorts_rows_read_F; };
    const tsp00_8ByteCounter &ins_sorts              () const  { return this->variant.C_true.ins_sorts_F; };
    const tsp00_8ByteCounter &ins_sorts_rows_ins     () const  { return this->variant.C_true.ins_sorts_rows_ins_F; };
    const tsp00_8ByteCounter &join_hash              () const  { return this->variant.C_true.join_hash_F; };
    const tsp00_8ByteCounter &join_parallel          () const  { return this->variant.C_true.join_parallel_F; };
    const tsp00_8ByteCounter &join_legacy            () const  { return this->variant.C_true.join_legacy_F; };
    const tsp00_8ByteCounter &join_op_normal         () const  { return this->variant.C_true.join_op_normal_F; };
    const tsp00_8ByteCounter &join_op_improved       () const  { return this->variant.C_true.join_op_improved_F; };
    const tsp00_8ByteCounter &avg_lock_used          () const  { return this->variant.C_true.avg_lock_used_F; };
    const tsp00_8ByteCounter &max_lock_used          () const  { return this->variant.C_true.max_lock_used_F; };
    const tsp00_8ByteCounter &collisions             () const  { return this->variant.C_true.collisions_F; };
    const tsp00_8ByteCounter &escalations            () const  { return this->variant.C_true.escalations_F; };
    const tsp00_8ByteCounter &new_rows               () const  { return this->variant.C_true.new_rows_F; };
    const tsp00_8ByteCounter &new_tabs               () const  { return this->variant.C_true.new_tabs_F; };
    const tsp00_8ByteCounter &deadlocks              () const  { return this->variant.C_true.deadlocks_F; };
    const tsp00_8ByteCounter &request_timeouts       () const  { return this->variant.C_true.request_timeouts_F; };
    const tsp00_8ByteCounter &read_log               () const  { return this->variant.C_true.read_log_F; };
    const tsp00_8ByteCounter &write_log              () const  { return this->variant.C_true.write_log_F; };
    const ARRAY_TYPESTRING_MSP00(tsp00_8ByteCounter, 8) &kb1_monitor            () const  { return this->variant.C_true.kb1_monitor_F; };
    const tsp00_8ByteCounter &read_string            () const  { return this->variant.C_true.read_string_F; };
    const tsp00_8ByteCounter &write_string           () const  { return this->variant.C_true.write_string_F; };
    const tsp00_8ByteCounter &r_v_acc                () const  { return this->variant.C_true.r_v_acc_F; };
    const tsp00_8ByteCounter &w_v_acc                () const  { return this->variant.C_true.w_v_acc_F; };
    const tsp00_8ByteCounter &r_p_acc                () const  { return this->variant.C_true.r_p_acc_F; };
    const tsp00_8ByteCounter &w_p_acc                () const  { return this->variant.C_true.w_p_acc_F; };
    const tsp00_8ByteCounter &catalog_r_v            () const  { return this->variant.C_true.catalog_r_v_F; };
    const tsp00_8ByteCounter &catalog_w_v            () const  { return this->variant.C_true.catalog_w_v_F; };
    const tsp00_8ByteCounter &catalog_r_p            () const  { return this->variant.C_true.catalog_r_p_F; };
    const tsp00_8ByteCounter &catalog_w_p            () const  { return this->variant.C_true.catalog_w_p_F; };
    const tsp00_8ByteCounter &perm_r_v               () const  { return this->variant.C_true.perm_r_v_F; };
    const tsp00_8ByteCounter &perm_w_v               () const  { return this->variant.C_true.perm_w_v_F; };
    const tsp00_8ByteCounter &perm_r_p               () const  { return this->variant.C_true.perm_r_p_F; };
    const tsp00_8ByteCounter &perm_w_p               () const  { return this->variant.C_true.perm_w_p_F; };
    const tsp00_8ByteCounter &temp_r_v               () const  { return this->variant.C_true.temp_r_v_F; };
    const tsp00_8ByteCounter &temp_w_v               () const  { return this->variant.C_true.temp_w_v_F; };
    const tsp00_8ByteCounter &temp_r_p               () const  { return this->variant.C_true.temp_r_p_F; };
    const tsp00_8ByteCounter &temp_w_p               () const  { return this->variant.C_true.temp_w_p_F; };
    const tsp00_8ByteCounter &bstr_r_v               () const  { return this->variant.C_true.bstr_r_v_F; };
    const tsp00_8ByteCounter &bstr_w_v               () const  { return this->variant.C_true.bstr_w_v_F; };
    const tsp00_8ByteCounter &bstr_r_p               () const  { return this->variant.C_true.bstr_r_p_F; };
    const tsp00_8ByteCounter &bstr_w_p               () const  { return this->variant.C_true.bstr_w_p_F; };
    const tsp00_8ByteCounter &level0_r_v             () const  { return this->variant.C_true.level0_r_v_F; };
    const tsp00_8ByteCounter &level0_w_v             () const  { return this->variant.C_true.level0_w_v_F; };
    const tsp00_8ByteCounter &level0_r_p             () const  { return this->variant.C_true.level0_r_p_F; };
    const tsp00_8ByteCounter &level0_w_p             () const  { return this->variant.C_true.level0_w_p_F; };
    const tsp00_8ByteCounter &level1_r_v             () const  { return this->variant.C_true.level1_r_v_F; };
    const tsp00_8ByteCounter &level1_w_v             () const  { return this->variant.C_true.level1_w_v_F; };
    const tsp00_8ByteCounter &level1_r_p             () const  { return this->variant.C_true.level1_r_p_F; };
    const tsp00_8ByteCounter &level1_w_p             () const  { return this->variant.C_true.level1_w_p_F; };
    const tsp00_8ByteCounter &level2_r_v             () const  { return this->variant.C_true.level2_r_v_F; };
    const tsp00_8ByteCounter &level2_w_v             () const  { return this->variant.C_true.level2_w_v_F; };
    const tsp00_8ByteCounter &level2_r_p             () const  { return this->variant.C_true.level2_r_p_F; };
    const tsp00_8ByteCounter &level2_w_p             () const  { return this->variant.C_true.level2_w_p_F; };
    const tsp00_8ByteCounter &level3_r_v             () const  { return this->variant.C_true.level3_r_v_F; };
    const tsp00_8ByteCounter &level3_w_v             () const  { return this->variant.C_true.level3_w_v_F; };
    const tsp00_8ByteCounter &level3_r_p             () const  { return this->variant.C_true.level3_r_p_F; };
    const tsp00_8ByteCounter &level3_w_p             () const  { return this->variant.C_true.level3_w_p_F; };
    const tsp00_8ByteCounter &level4_r_v             () const  { return this->variant.C_true.level4_r_v_F; };
    const tsp00_8ByteCounter &level4_w_v             () const  { return this->variant.C_true.level4_w_v_F; };
    const tsp00_8ByteCounter &level4_r_p             () const  { return this->variant.C_true.level4_r_p_F; };
    const tsp00_8ByteCounter &level4_w_p             () const  { return this->variant.C_true.level4_w_p_F; };
    const tsp00_8ByteCounter &level5_r_v             () const  { return this->variant.C_true.level5_r_v_F; };
    const tsp00_8ByteCounter &level5_w_v             () const  { return this->variant.C_true.level5_w_v_F; };
    const tsp00_8ByteCounter &level5_r_p             () const  { return this->variant.C_true.level5_r_p_F; };
    const tsp00_8ByteCounter &level5_w_p             () const  { return this->variant.C_true.level5_w_p_F; };
    const tsp00_8ByteCounter &psm_r_v                () const  { return this->variant.C_true.psm_r_v_F; };
    const tsp00_8ByteCounter &psm_w_v                () const  { return this->variant.C_true.psm_w_v_F; };
    const tsp00_8ByteCounter &psm_r_p                () const  { return this->variant.C_true.psm_r_p_F; };
    const tsp00_8ByteCounter &psm_w_p                () const  { return this->variant.C_true.psm_w_p_F; };
    const tsp00_8ByteCounter &add_rec                () const  { return this->variant.C_true.add_rec_F; };
    const tsp00_8ByteCounter &cadd_rec               () const  { return this->variant.C_true.cadd_rec_F; };
    const tsp00_8ByteCounter &repl_rec               () const  { return this->variant.C_true.repl_rec_F; };
    const tsp00_8ByteCounter &crepl_rec              () const  { return this->variant.C_true.crepl_rec_F; };
    const tsp00_8ByteCounter &del_rec                () const  { return this->variant.C_true.del_rec_F; };
    const tsp00_8ByteCounter &cdel_rec               () const  { return this->variant.C_true.cdel_rec_F; };
    const tsp00_8ByteCounter &get_rec                () const  { return this->variant.C_true.get_rec_F; };
    const tsp00_8ByteCounter &cget_rec               () const  { return this->variant.C_true.cget_rec_F; };
    const tsp00_8ByteCounter &next_rec               () const  { return this->variant.C_true.next_rec_F; };
    const tsp00_8ByteCounter &cnext_rec              () const  { return this->variant.C_true.cnext_rec_F; };
    const tsp00_8ByteCounter &prev_rec               () const  { return this->variant.C_true.prev_rec_F; };
    const tsp00_8ByteCounter &cprev_rec              () const  { return this->variant.C_true.cprev_rec_F; };
    const tsp00_8ByteCounter &sel_direc              () const  { return this->variant.C_true.sel_direc_F; };
    const tsp00_8ByteCounter &sel_next               () const  { return this->variant.C_true.sel_next_F; };
    const tsp00_8ByteCounter &sel_prev               () const  { return this->variant.C_true.sel_prev_F; };
    const tsp00_8ByteCounter &add_inv                () const  { return this->variant.C_true.add_inv_F; };
    const tsp00_8ByteCounter &cadd_inv               () const  { return this->variant.C_true.cadd_inv_F; };
    const tsp00_8ByteCounter &del_inv                () const  { return this->variant.C_true.del_inv_F; };
    const tsp00_8ByteCounter &cdel_inv               () const  { return this->variant.C_true.cdel_inv_F; };
    const tsp00_8ByteCounter &get_inv                () const  { return this->variant.C_true.get_inv_F; };
    const tsp00_8ByteCounter &get_inv_parallel       () const  { return this->variant.C_true.get_inv_parallel_F; };
    const tsp00_8ByteCounter &cget_inv               () const  { return this->variant.C_true.cget_inv_F; };
    const tsp00_8ByteCounter &funccount              () const  { return this->variant.C_true.funccount_F; };
    const tsp00_8ByteCounter &transcount             () const  { return this->variant.C_true.transcount_F; };
    const tsp00_8ByteCounter &kb_calls               () const  { return this->variant.C_true.kb_calls_F; };
    const ARRAY_TYPESTRING_MSP00(tsp00_8ByteCounter, iunused_but_needed - 0 + 1) &reference_array        () const  { return this->variant.C_false.reference_array_F; };
#endif    /* defined (__cplusplus) */
} tgg00_ReferenceInfo;

typedef struct                     tgg00_LogPos
{
    union
    {
    /* 1 */
        struct tgg00_LogPos_1
        {
            tsp00_PageNo        logLpno_gg00_F;
            tgg00_PageType      logPageType_gg00_F;
            tgg00_PageType2     logPageType2_gg00_F;
            tsp00_C2            logFiller_gg00_F;
            tgg00_PagePos       logPpos_gg00_F;
        } C_1;
    /* 2 */
        struct tgg00_LogPos_2
        {
            tgg00_PageHeader    logHeader_gg00_F;
        } C_2;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tsp00_PageNo       &logLpno_gg00           ()  { return this->variant.C_1.logLpno_gg00_F; };
    tgg00_PageType     &logPageType_gg00       ()  { return this->variant.C_1.logPageType_gg00_F; };
    tgg00_PageType2    &logPageType2_gg00      ()  { return this->variant.C_1.logPageType2_gg00_F; };
    tsp00_C2           &logFiller_gg00         ()  { return this->variant.C_1.logFiller_gg00_F; };
    tgg00_PagePos      &logPpos_gg00           ()  { return this->variant.C_1.logPpos_gg00_F; };
    tgg00_PageHeader   &logHeader_gg00         ()  { return this->variant.C_2.logHeader_gg00_F; };
    /* const accessor functions */
    const tsp00_PageNo &logLpno_gg00           () const  { return this->variant.C_1.logLpno_gg00_F; };
    const tgg00_PageType &logPageType_gg00       () const  { return this->variant.C_1.logPageType_gg00_F; };
    const tgg00_PageType2 &logPageType2_gg00      () const  { return this->variant.C_1.logPageType2_gg00_F; };
    const tsp00_C2     &logFiller_gg00         () const  { return this->variant.C_1.logFiller_gg00_F; };
    const tgg00_PagePos &logPpos_gg00           () const  { return this->variant.C_1.logPpos_gg00_F; };
    const tgg00_PageHeader &logHeader_gg00         () const  { return this->variant.C_2.logHeader_gg00_F; };
#endif    /* defined (__cplusplus) */
} tgg00_LogPos;
    /*******************************************************/
    /******************* SELECT - TYPEN ********************/
    /*******************************************************/

typedef struct                     tgg00_FieldDescrRec
{
    tsp00_Int4          field_count;
    tsp00_Int2          field_rec_pos;
    tsp00_Int2          field_rec_len;
    tsp00_Int2          result_out_pos;
    tsp00_Int2          result_out_len;
    pasbool             field_is_descen;
    tgg00_StackOpFunc   field_function;
    pasbool             field_sum_flag;
    pasbool             field_filler;
} tgg00_FieldDescrRec;

typedef struct                     tgg00_AggrDescrRec
{
    tsp00_Int2          agg_cnt;
    pasbool             agg_filler;
    tgg00_AggSet        agg_opt_infos;
    ARRAY_VAR_MSP00 (         agg_field_descr, tgg00_FieldDescrRec, MAX_COL_SEQUENCE_GG00);
    tsp00_Int4          agg_prim_key_cnt;
    tgg00_Lkey          agg_last_sec_key;
} tgg00_AggrDescrRec;
    /*******************************************************/
    /************** SELECT - TYPEN _ ENDE ******************/
    /*******************************************************/
    /**************************************/
    /*    VGG01 global variables */
    /**************************************/

typedef enum tgg00_InstanceType_Enum
{
    itUNKNOWN_egg00,
    itOLTP_egg00,
    itLVC_egg00,
    it_old_BW_egg00,
    it_old_CS_egg00,
    it_old_EMERGE_egg00,
    itARCHIVE_egg00
} tgg00_InstanceType_Enum;
typedef tgg00_InstanceType_Enum tgg00_InstanceType_Param;
typedef ENUM_TYPE_MSP00 (          tgg00_InstanceType, tgg00_InstanceType_Enum, itARCHIVE_egg00, unsigned char);

typedef struct                     tgg00_KernelGlobals
{
    tsp00_Int4          timezone;
    tsp00_Int4          vf_bufpool;
    tgg00_Surrogate     sysuser_id;
    tsp00_ResNum        rescnt_unknown;
    pasbool             datacachecheck;
    tsp00_ResNum        rescnt_1;
    pasbool             sysmsg_may_exist;
    tsp00_ResNum        rescnt_0;
    pasbool             db_is_for_sapr3;
    tsp00_KnlIdentifier sysuser_name;
    pasbool             treecheck;
    pasbool             bd_lock_check;
    pasbool             bd_subtree;
    tgg00_InstanceType  kglInstanceType_gg00;
    pasbool             filler3;
    tsp00_C1            undef_buf;
    tsp00_Int2          filler4;
    tsp00_MoveObjPtr    undef_addr;
} tgg00_KernelGlobals;

typedef struct                     tgg00_VtraceState
{
    pasbool             vtrAk_gg00;
    pasbool             vtrAkDelete_gg00;
    pasbool             vtrAkInsert_gg00;
    pasbool             vtrAkPacket_gg00;
    pasbool             vtrAkSelect_gg00;
    pasbool             vtrAkShortPacket_gg00;
    pasbool             vtrAkUpdate_gg00;
    pasbool             vtrAll_gg00;
    pasbool             vtrAny_gg00;
    pasbool             vtrBdIndex_gg00;
    pasbool             vtrBdPrim_gg00;
    pasbool             vtrBdString_gg00;
    pasbool             vtrIoTrace_gg00;
    pasbool             vtrKbLock_gg00;
    pasbool             vtrStrategy_gg00;
    pasbool             vtrTime_gg00;
    pasbool             vtrGateway_gg00;
    pasbool             vtrBdObject_gg00;
    pasbool             vtrOmsNew_gg00;
    pasbool             vtrOmsGet_gg00;
    pasbool             vtrOmsUpd_gg00;
    pasbool             vtrOmsFree_gg00;
    tsp00_Int2          vtrFiller1_gg00;
    pasbool             vtrCheck_gg00;
    pasbool             vtrRetcodeCheck_gg00;
    tsp00_Int2          vtrStopRetcode_gg00;
    tgg91_SessionNo     vtrSession_gg00;
} tgg00_VtraceState;

typedef struct                     tgg00_VfFileref
{
    tsp00_Int4          no;
    tsp00_VfBufaddr     buf;
    tsp00_Int4          poolptr;
    tsp00_Int2          filler;
    tsp00_Int2          buf_cnt;
} tgg00_VfFileref;

typedef struct                     tgg00_BdSetResultRecord
{
    tsp00_Int4          bd_key_check_len;
    tsp00_Int4          bd_max_rec_cnt;
    tsp00_Int4          bd_rec_cnt;
    tsp00_Int4          bd_max_fill_len;
    tsp00_Int4          bd_fill_len;
    pasbool             bd_next;
    pasbool             bd_drop_page;
    tsp00_Int2          bd_filler1;
} tgg00_BdSetResultRecord;

struct tgg00_TemplateDummy {
    tgg00_PageType       dummy_tgg00_PageType;
    tgg00_MessType       dummy_tgg00_MessType;
    tgg00_InvListKind    dummy_tgg00_InvListKind;
    tgg00_PageCheckType  dummy_tgg00_PageCheckType;
    tgg00_ConsistentReadInfo dummy_tgg00_ConsistentReadInfo;
    tgg00_RegionType     dummy_tgg00_RegionType;
    tgg00_ObjFileType    dummy_tgg00_ObjFileType;
    tgg00_MessBufStruct  dummy_tgg00_MessBufStruct;
    tgg00_IndexMode      dummy_tgg00_IndexMode;
    tgg00_StackOpType    dummy_tgg00_StackOpType;
    tgg00_StackOpOutput  dummy_tgg00_StackOpOutput;
    tgg00_PageType2      dummy_tgg00_PageType2;
    tgg00_DiagType       dummy_tgg00_DiagType;
    tgg00_FetchState     dummy_tgg00_FetchState;
    tgg00_QualKind       dummy_tgg00_QualKind;
    tgg00_RecoveryMode   dummy_tgg00_RecoveryMode;
    tgg00_LockReqMode    dummy_tgg00_LockReqMode;
    tgg00_Debug          dummy_tgg00_Debug;
    tgg00_Tfn            dummy_tgg00_Tfn;
    tgg00_StackOpBuildIn dummy_tgg00_StackOpBuildIn;
    tgg00_StackEntryType dummy_tgg00_StackEntryType;
    tgg00_ObjState       dummy_tgg00_ObjState;
    tgg00_DateTimeFormat dummy_tgg00_DateTimeFormat;
    tgg00_BdInvRange     dummy_tgg00_BdInvRange;
    tgg00_TableKind      dummy_tgg00_TableKind;
    tgg00_InstanceType   dummy_tgg00_InstanceType;
    tgg00_JoinSearchLevel dummy_tgg00_JoinSearchLevel;
    tgg00_MessType2      dummy_tgg00_MessType2;
    tgg00_TfnLog         dummy_tgg00_TfnLog;
    tgg00_StackOpFunc    dummy_tgg00_StackOpFunc;
    tgg00_ServerdbType   dummy_tgg00_ServerdbType;
    tgg00_TfnTemp        dummy_tgg00_TfnTemp;
    tgg00_MonitorPart    dummy_tgg00_MonitorPart;
    tgg00_VtraceType     dummy_tgg00_VtraceType;
    tgg00_RefInfoIndex   dummy_tgg00_RefInfoIndex;
    tgg00_DirectoryMonitorOp dummy_tgg00_DirectoryMonitorOp;
    tgg00_ShowKind       dummy_tgg00_ShowKind;
    tgg00_SrVtrace2Type  dummy_tgg00_SrVtrace2Type;
    tgg00_BdInvSet       dummy_tgg00_BdInvSet;
    tgg00_DumpStateSet   dummy_tgg00_DumpStateSet;
    tgg00_MiscSet        dummy_tgg00_MiscSet;
    tgg00_PageMode       dummy_tgg00_PageMode;
    tgg00_FiletypeSet    dummy_tgg00_FiletypeSet;
    tgg00_LockReqState   dummy_tgg00_LockReqState;
    tgg00_Linkset        dummy_tgg00_Linkset;
    tgg00_HandlingSet    dummy_tgg00_HandlingSet;
    tgg00_ExtendedFiletypeSet dummy_tgg00_ExtendedFiletypeSet;
    tgg00_TransState     dummy_tgg00_TransState;
    tgg00_StackState     dummy_tgg00_StackState;
    tgg00_BdUseSet       dummy_tgg00_BdUseSet;
    tgg00_StackOpFuncSet dummy_tgg00_StackOpFuncSet;
    tgg00_AggSet         dummy_tgg00_AggSet;
    tgg00_ColumnSet      dummy_tgg00_ColumnSet;
};


#endif
