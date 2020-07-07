
/* some common prolog for every module
 */

#ifndef VAK001_H
#define VAK001_H


/******************************************
 *            I N C L U D E S             *
 ******************************************/


#ifndef GSP00_2_H
#include "gsp00_2.h"
#endif

#ifndef GSP00_H
#include "gsp00.h"
#endif

#ifndef VSP001_H
#include "vsp001.h"
#endif

#ifndef GGG00_H
#include "ggg00.h"
#endif

#ifndef GGG04_H
#include "ggg04.h"
#endif

#ifndef GAK00_H
#include "gak00.h"
#endif

/******************************************
 *           C O N S T A N T S            *
 ******************************************/

#define cak_unicode_mark               '\x00'
#define cak_del_trigger_linkage         "\x00\x01"
#define cak_ins_trigger_linkage         "\x00\x03"
#define cak_upd_trigger_linkage         "\x00\x05"
#define cak_init_fileversion            "\x00\x00"
#define cak_normal_codeset                      1
#define cak_unicode                             2
#define cak_init_cache_size                    32
#define cak_cache_chunk_flag           (- 2)

#if defined (bit64)

#define cak_max_align                         512

#else

#define cak_max_align                        1024

#endif

#define cak_default_precision                  20
#define cak_init_join_tab_size                  4
#define cak_max_debug_info                   1000
#define cak_max_debug_mapping                2018
#define cak_maxconstlength                     30
#define cak_maxvarcoliolen                    255
#define cak_max_named_subtrans                 50
#define cak_max_constraints                  1024
#define cak_max_cmd_stack                      16
#define cak_max_proc                           64
#define cak_max_resn                           50
#define cak_max_trigger_level                  64
#define cak_max_mmarray                        21
#define cak_maxlinkpos                 (MAX_COL_PER_TAB_GG00 * 2)
#define mxak_set                       (( ( MAX_COL_PER_TAB_GG00 + 1 + 15 ) / 16 ) * 2)
    /* primary pages via secondary key = 100 / cak_maxpercentage    */
    /* duplicates when more than one strategy is used(random insert)*/
    /* plus building up the result set                              */
#define cak_max_del_qual                       16
#define cak_ak_float                            0
#define cak_complex_view_indicator              4
#define cak_is_undefined               (- 2)
#define cak_check_number_length             10000
#define cak_max_sm_error                       16
#define cak_dbproccache                         5
#define cak_max_trigger_stack                 900
#define cak_e_parameter                     21000
#define cak_e_isolation_join                21001
#define cak_e_union_in_corr                 21002
#define cak_e_expr_in_insert                21003
#define cak_e_receive_error                 21004
#define cak_e_subq_type_diff                21005
#define cak_e_dbproc_call                   21007    /* PTS 1112612 */
#define cak_e_schema_mapped                 21008
#define cak_e_use_fetch_rest           (- 2)
#define cak_e_table_unloaded           (- 106)
#define cak_e_table_or_file_dropped    (- 108)    /* return_code */
#define cak_e_corelated_subquery_not_allowed (- 7011)    /* return_code */
#define cak_e_inv_not_found            (- 9012)
#define cak_e_cache_full               (- 9400)    /* return_code */
#define cak_e_released                      10000
#define cak_e_roll_released                 10001
#define cak_e_kb_net_line_down                600
#define cak_e_kb_ansi_net_line_down    (- 60)
#define cak_sysdba_version                     45
#define mxak_order_errortext                  256
    /* PTS 1109925 E.Z. */
#define cak_utcdiff_len                         4
#define cak_utcdiff_frac                        2
    /* D.T. QueryRewrite */
#define cak_qrewrite_ref_init          (- 1)
#define cak_qrewrite_ref_star          (- 2)
#define cak_qrewrite_ref_multistar     (- 4)
#define cak_qrewrite_ref_function      (- 8)
#define cak_qrewrite_ref_aggregate     (- 16)
    /* */
    /****************************************************************/
    /* Default values for the mimikry of the Oracle NLS support     */
#define cak_nls_currency                "$   "
#define cak_nls_date_format             " DD-MON-YY                                        "
#define cak_nls_date_language           "ENG"
#define cak_nls_numeric_chars           ".,"

#if defined (BIT64)

    /*--------------------------------------------------------------*/
    /* BEGIN COMPUTE CONSTANTS */
    /* INCLUDE CONSTANTS FROM XSIZEK */
    /* INCLUDE CONSTANTS FROM VSP00 */
    /* INCLUDE CONSTANTS FROM VGG00 */
#define mxak_cache_dir_entry                   72

#else

#define mxak_cache_dir_entry                   48

#endif

#define mxak_standard_sysk             (SURROGATE_MXGG00 + 4)
    /*                          4=sizeof(sentrytyp)+sizeof(slinkage) */
#define cak_sysbufferoffset            (cgg_rec_key_offset + mxak_standard_sysk)
#define mxak_comment                         8068
#define mxak_full_dml_init                   1320
#define mxak_part_dml_init                    952
#define mxak_entry_type                         2
#define mxak_resname_rec_key                  138    /* SURROGATE_MXGG00 +
      2 + 2 * sizeof (tsp00_KnlIdentifier) */
#define mxak_linkparsinfo                     212    /* &tak_linkparsinfo */
#define mxak_mapset                          1536    /* 3 * 2 * cgg04_mapset_entries */
#define mxak_selfref_link_info                  8    /* &tak_selfref_link_info*/
#define mxak_usage_offset                       8    /* 4 * &tsp00_Int2 */
#define mxak_usertab_offset                     8    /* 4 * &tsp00_Int2 */
#define mxak_usagedef                          12    /* &tak_usagedef */
#define cak_max_usage                  (( MAX_RECLEN_GG00 - cak_sysbufferoffset - 8 ) / mxak_usagedef)
#define mxak_usertabdef                        12    /* &tak_usertabdef */
#define cak_max_usertab                (( MAX_RECLEN_GG00 - cak_sysbufferoffset - 8 ) / mxak_usertabdef)
#define cak_complete_prefix                     3
#define mxak_complex_entry                     12    /* &tak_complex_entry */
#define mxak_complexrecminlength       (cak_sysbufferoffset + 4 + FILE_ID_MXGG00 + mxak_complex_entry)
#define cak_intern_prefix                       4
#define mxak_columnset                 mxak_set    /* &tak_columnset */
    /* PTS 1111574 E.Z. */
#define cak_maxconstraintstack         (( MAX_RECLEN_GG00 - cak_sysbufferoffset - 8 - mxak_columnset - 22 ) / STACK_ENTRY_MXGG00)
#define cak_maxdefaultvalue            (( MAX_RECLEN_GG00 - cak_sysbufferoffset - 4 ))    /* PTS 1108428 */
#define cak_maxpercentage                      15
#define cak_baseindexoffset                    12
    /* PTS 1111576 E.Z. */
    /* sign for dropped columns */
#define cak_cdropped                   (MAX_COL_PER_TAB_GG00 + 1)
#define mxak_commandhead                       20
#define mxak_one_corcolumn                     16    /* &tak_one_corcolumn */
#define mxak_col_uses_offset                   32
#define mxak_domainrec_offset                  56
#define mxak_sequence_offset                  156
#define mxak_userrec_offset                   152
#define mxak_synonymrec_offset                 60
#define cak_frac_offset                         1
#define cak_initrows                            0
#define cak_initpages                           1
#define cak_initavgrowlen                     200
#define cak_initinvlist                        10
#define cak_into_res_fid                        1
#define cak_intern_res_fid                      2
#define cak_keycol_low_def                      0    /* fp_key_low_handl */
#define cak_keycol_low_trunc           (- 1)    /* fp_key_low_handl */
#define cak_keycol_low_zero                     1    /* fp_key_low_handl */
#define cak_max_param_index                  1024
#define cak_maxkeyfields                      512
#define cak_maxcacherest                      100
    /* correlation_columns of all tables */
#define cak_maxcorcolumns                      64
#define cak_maxcorlevel                         4    /* maxno. of corr-subqueries
      */
#define cak_maxfieldlength                    254    /* max. of n for char (n) */
#define cak_maxdb2fieldlength                4000    /* max. of n for char (n) */
#define cak_maxorafieldlength                8000    /* max. of n for char (n) */
#define cak_maxdeffieldlength                8000    /* max. of n for char (n) */
#define cak_maxresulttables                     2    /* 1 EXTERN, 1 INTERN  */
#define cak_max_role_context                32768
#define cak_maxlinkcolumns                     16
#define cak_maxlinkdef                         30
#define cak_max_litcoldesc                    335
#define cak_maxsubcnt_per_level               256    /* PTS 1130126 E.Z. */
    /* PTS 1111574 E.Z. */
#define mxak_longlit_buf               (MAX_INT2_SP00 - 56)
    /* mxak_all_locks = 1604 MAX_QUALBUF_LOCKS_GG00 * LOCK_MXGG04 + 4 */
#define mxak_param_info                       268    /* &tak_param_info */
#define mxak_procrec_offset            (cak_sysbufferoffset + 2 * mxsp_name + 2 + 2)
#define mxak_systembuf                      32760    /* &tak_systembuffer */
#define mxak_mapset_rec                      1580    /* &tak_mapset_record */
#define mxak_domainrec                         76    /* &tak_domainrecord */
    /* PTS 1111574 E.Z. */
#define mxak_constraintrec             (cak_sysbufferoffset + 8 + mxak_columnset + 22 + cak_maxconstraintstack * STACK_ENTRY_MXGG00)    /* &tak_constraintrecord */
#define cak_init_node                         512    /*  pars_tree  */
#define cak_next_node                         256    /*  pars_tree  */
#define cak_max_node                   csp_maxint2    /*  pars_tree  */
#define cak_no_level                            0
#define cak_lo_level                            1
#define cak_sa_level                            2
#define cak_maxnode                           512    /*  pars_tree  */
#define cak_maxorderlength                   4016
#define cak_additional_parsinfos               25    /* for fixedpos and ...*/
    /* PTS 1111574 E.Z. */
#define cak_maxparsfields              (csp_maxvariables + cak_additional_parsinfos)
#define cak_maxshortinfo                      300    /*old_csp_maxvariables*/
#define cak_maxparams                        2700
    /*sizeof(tak_shortinforecord/tak_parsinforecord) <= 32KB */
#define cak_maxpcount                           8    /* needed for parsinfos*/
    /* if max no of stack_entries is 2048 */
    /* PTS 1111574 E.Z. */
#define mxak_colnamesbuf               (MAX_RECLEN_GG00 - cak_sysbufferoffset - 4)
#define mxak_useridrec                         28    /* & tak_useridrecord */
#define mxak_res_keysbuf                    40280
#define mxak_res_keysbuf_offset               332
    /* sizeof (tak_resultkeyrecord) - mxak_res_keysbuf */
    /* PTS 1111574 E.Z. */
#define mxak_viewtextrec               MAX_RECLEN_GG00    /* &tak_viewtextrecord */
#define cak_max_screen_cols                    79
#define cak_maxtextlength                      80
#define mxak_multindex                        208    /* &tak_multindex */
#define cak_max_index_per_rec          (( MAX_RECLEN_GG00 - cak_sysbufferoffset - 4 ) / mxak_multindex)
    /* PTS 1111574 E.Z. */
#define mxak_vcolumndesc                       12    /* &tak_vcolumndescription */
#define cak_maxviewdesc                (( MAX_RECLEN_GG00 - cak_sysbufferoffset - 8 ) / mxak_vcolumndesc)
    /* END PTS 1111574 E.Z. */
#define cak_viewdesc_offset            (cak_sysbufferoffset + 8)
#define mxak_viewtext_offset                   12    /* 6 * &tsp00_Int2 */
#define mxak_viewtext                  (MAX_RECLEN_GG00 - cak_sysbufferoffset - mxak_viewtext_offset)
#define mxak_viewtabdef                        44    /* &tak_viewtabdef */
#define mxak_one_viewjoin                      28    /* &tak_one_viewjoin */
#define mxak_one_derived_desc                   8    /* &tak_one_derived_desc */
    /* view qualification parts */
#define mxak_viewqual_basis_offs               12    /* tsp00_C2 + tsp00_Int2 + 4*boolean */
#define mxak_viewqual_stack_offs               14    /* tsp00_C2 + 2*tsp00_Int2+2*tsp00_Int4 */
#define mxak_viewqual_join_offs                 4    /* tsp00_C2 + tsp00_Int2 */
#define mxak_viewqual_basis            (( MAX_RECLEN_GG00 - cak_sysbufferoffset - mxak_viewqual_basis_offs ))
#define mxak_viewqual_stack            (( MAX_RECLEN_GG00 - cak_sysbufferoffset - mxak_viewqual_stack_offs ))
#define mxak_viewqual_join             (( MAX_RECLEN_GG00 - cak_sysbufferoffset - mxak_viewqual_join_offs ))
#define cak_max_viewqual_tab           (mxak_viewqual_basis / mxak_viewtabdef)
#define cak_max_viewqual_join          (mxak_viewqual_join / mxak_one_viewjoin)
#define cak_max_viewqual_stack         (mxak_viewqual_stack / STACK_ENTRY_MXGG00)
#define cak_max_viewqual_data          (cak_max_viewqual_stack * STACK_ENTRY_MXGG00)
#define cak_max_viewqual_derivedcol    (mxak_viewqual_join / mxak_one_derived_desc)
#define mxak_union_header                     184    /* 7 * &tsp00_Int2
            + &tsp00_Name + &tgg00_FileId */
#define mxak_union_entry                       16    /* &tak_union_entry */
#define cak_max_union                  (( MAX_RECLEN_GG00 - cak_sysbufferoffset - mxak_union_header ) / mxak_union_entry)
#define cak_maxcomplex                 (( MAX_RECLEN_GG00 - cak_sysbufferoffset - FILE_ID_MXGG00 - 4 ) / mxak_complex_entry)
#define cak_page80percent              (( MAX_RECLEN_GG00 * 8 ) / 10)
#define mxak_parsbuf                   (MAX_RECLEN_GG00 - cak_sysbufferoffset)
#define mxak_stored_cmd_buf            (MAX_RECLEN_GG00 - cak_sysbufferoffset - 8)
#define mxak_pars_header                      296
#define mxak_parsid                            12
#define mxak_parskey                            6    /* &tak_parskey - 2 */
#define cak_extern_pos                          1
#define cak_intern_pos                          2
#define cak_aux_pos                             3
#define mxak_base_offset                        8    /* 4 * &tsp00_Uint1
            + 2 * &tsp00_Int2 */
#define mxak_change_colinfo                     4    /* &tak_change_colinfo */
#define mxak_vttabdef                          24    /* &tak_vttabdef */
    /* PTS 1111574 E.Z. */
#define mxak_correc                    (cak_maxcorcolumns * mxak_one_corcolumn + 16 * 8 + 8)
#define mxak_serverdbrec                      112    /* &tak_serverdbrecord */
#define mxak_col_func_nesting                  32
#define mxak_col_stats                         12
    /* PTS 1111574 E.Z. */
#define MAX_COL_STATS_AK00             (( MAX_RECLEN_GG00 - cak_sysbufferoffset - 22 - mxak_columnset ) / mxak_col_stats)
    /* PTS 1109291 E.Z. */
#define cak_max_pcounts                       100
    /* END COMPUTE CONSTANTS */
    /*--------------------------------------------------------------*/
#define cak_complex_select                    101
#define cak_iso_level_stmt_consistent          50
#define cak_iso_level_trans_consistent         60
#define cak_temp_user_name              "TEMP              "
#define cak_domain_user                 "DOMAIN            "
#define cak_oms_user                    "OMS               "
#define cak_sys_application             "SYS_PROC          "
#define cak_ln_dual                     "DUAL              "
#define cak_ln_new                      "NEW               "
#define cak_ln_old                      "OLD               "
#define cak_ln_show                     "SHOW              "
#define cak_ln_syscat1                  "SYS%CAT1          "
#define cak_ln_syscat2                  "SYS%CAT2          "
#define cak_ln_syserrormap              "SYSERRORMAP       "
#define cak_ln_sysmessages              "MESSAGES          "
#define cak_ln_sysparseid               "SYSPARSEID        "
#define cak_ln_system                   "SYSTEM            "
#define cak_u_public                    "PUBLIC            "
#define cak_u_superdba                  "SUPERDBA          "
#define cak_u_sys                       "SYSDD             "
#define cak_u_oracle_sys                "SYS               "
#define cak_util_userid                 "\x00\x00\x00\x01"
#define cak_nil_group_id                "\x00\x00\x00\x00\x00\x00\x00\x00"
#define cak_util_user_id                "\xFF\xFF\x00\x00\x00\x00\x00\x01"
#define cak_public_id                   "\x00\x00\x00\x00\x00\x00\x00\x02"
#define cak_public_user_id              "\xFF\xFF\x00\x00\x00\x00\x00\x03"
#define cak_temp_user_id                "\x00\x00\x00\x00\x00\x00\x00\x04"
#define cak_sys_user_id                 "\x00\x00\x00\x00\x00\x00\x00\x05"
#define cak_replication_user_id         "\xFF\xFF\x00\x00\x00\x00\x00\x06"
#define cak_replication_usertab_id      "\xFF\xFF\x00\x00\x00\x00\x00\x07"
#define cak_zerotableid                cgg_zero_id
#define cak_temp_info_linkage           "\xFF\xFF"
#define cak_zero_linkage                "\x00\x00"
#define cak_init_linkage                "\x00\x01"
#define cak_snap_viewbase_linkage       "\x00\x0B"
#define cak_zero_site                   "\x00\x00"
#define cak_fromseltab_site             "\xFF\xFE"
#define cak_temp_table_site             "\xFF\xFD"
#define cak_all_site                    "\xFF\xFC"
#define cak_show_table_site             "\xFF\xFB"
#define cak_type_site                   "\xFF\xF9"
#define cak_class_site                  "\xFF\xF8"
#define cak_dbfunc_site                 "\xFF\xF7"
#define cak_catalog_id                  "\xFF\xF6"
#define cak_max_site                    "\xFF\xFF"
    /* PTS 1115978 E.Z. */
#define cak_ln_column                   "COLUMN            "
#define cak_ln_columnname               "COLUMNNAME        "
#define cak_ln_columntype               "MOD               "
#define cak_ln_datatype                 "DATATYPE          "
#define cak_ln_date                     "DATE              "
#define cak_ln_default                  "DEFAULT           "
#define cak_ln_description              "DESCRIPTION       "
#define cak_ln_domainname               "DOMAINNAME        "
#define cak_ln_frac                     "DEC               "
#define cak_ln_grantor                  "GRANTOR           "
#define cak_ln_indexname                "INDEXNAME         "
#define cak_ln_length                   "LEN               "
#define cak_ln_order                    "SORT              "
#define cak_ln_password                 "PASSWORD          "
#define cak_ln_privileges               "PRIVILEGES        "
#define cak_ln_program                  "PROGRAM           "
#define cak_ln_state                    "STATE             "
#define cak_ln_synonymname              "SYNONYMNAME       "
#define cak_ln_sysdba                   "SYSDBA            "
#define cak_ln_table                    "TABLE             "
#define cak_ln_tablename                "TABLENAME         "
#define cak_ln_text                     "TEXT              "
#define cak_ln_time                     "TIME              "
#define cak_ln_type                     "TYPE              "
#define cak_ln_user                     "USER              "
#define cak_ln_username                 "USERNAME          "
#define cak_ln_value                    "VALUE             "
#define cak_n_tablekey                  "SYSKEY            "
#define cak_return_req                 true
#define cak_x_cascade                           1
#define cak_x_restrict                          2
#define cak_x_set_null                          3
#define cak_x_set_default                       4
#define cak_x_date                             45
#define cak_x_default                          53
#define cak_x_false                            78
#define cak_x_null                            129
#define cak_x_stamp                           192
#define cak_x_sysdba                          445
#define cak_x_time                            205
#define cak_x_timestamp                       211
#define cak_x_timezone                        459
#define cak_x_transaction                     435
#define cak_x_true                            215
#define cak_x_user                            219
#define cak_x_usergroup                       220
#define cak_x_uid                             221
#define cak_x_utcdate                         494
#define cak_x_utcdiff                         495
    /* used in VAK11 and VAK17 */
#define cak_x_beginload                         1
#define cak_x_create_table                      3
#define cak_x_describe_table                    7
#define cak_x_drop_table                        9
#define cak_x_drop_view                        10
#define cak_x_end_load                         12
#define cak_x_like                             15
#define cak_x_load                             16
#define cak_x_restore                          18
#define cak_x_save_init                        19
#define cak_x_restore_string                   20
#define cak_x_save_short_string                21
    /* PTS 1111576 E.Z. */
    /* used in VAK12 */
#define cak_x_create_domain                    20
#define cak_x_drop_domain                      21
#define cak_x_create_type                      22
#define cak_x_create_dbproc                    24
#define cak_x_create_package                   26
#define cak_x_drop_package                     28
#define cak_x_create_system_trigger            29
#define cak_x_drop_system_trigger              30
    /* used in VAK11 and VAK13 and VAK14 */
#define cak_x_alter_add                        22
#define cak_x_alter_drop                       23
#define cak_x_alter_alter                      24
#define cak_x_drop_constraint                  26
#define cak_x_add_constraint                   27
#define cak_x_alter_constraint                 28
#define cak_x_default_def                      29
#define cak_x_primary_key                      30
#define cak_x_alter_dynamic                    31
#define cak_x_modify                           32
#define cak_x_sample                           33
#define cak_x_alter_fact                       34
#define cak_x_alter_dimension                  35
#define cak_x_alter_bwhierarchy                36
#define cak_x_alter_type                       37
#define cak_x_alter_bad                        38
#define cak_x_cluster                          39
#define cak_x_alter_cluster                    40
    /* PTS 1111576 E.Z. */
#define cak_x_end_restore                      32
#define cak_x_save_string                      33
    /* used in VAK21 */
#define cak_x_create_user                      37
#define cak_x_user_like                        38
#define cak_x_alter_password                   39
#define cak_x_drop_user                        40
#define cak_x_alter_user                       41
#define cak_x_grant_user                       42
#define cak_x_rename_user                      43
#define cak_x_create_schema                    44
#define cak_x_drop_schema                      45
#define cak_x_user_group                      100
    /* used in VAK22 */
#define cak_x_one_grant_spec                    3
    /* used in VAK23 */
#define cak_x_create_synonym                   43
#define cak_x_drop_synonym                     44
#define cak_x_create_sequence                  54
#define cak_x_drop_sequence                    55
#define cak_x_create_role                      56
#define cak_x_drop_role                        57
    /* used in VAK24 */
#define cak_x_create_index                     45
#define cak_x_create_unique_index              46
#define cak_x_drop_index                       47
#define cak_x_recreate_bad_index               48
    /* used in VAK25 */
#define cak_x_create_link                      49
#define cak_x_drop_link                        50
#define cak_x_drop_foreign_key                 51
    /* constants for create sequence */
#define cak_x_start                             2
    /* constants for utility syntax VAK30, VAK36, VAK37 */
#define cak_x_util_commit                       2
#define cak_x_commit_release                    3
#define cak_x_clear_log                         6
#define cak_x_get_config                        7
#define cak_x_get_dev_config                    8
#define cak_x_init_config                       9
#define cak_x_restart                          10
#define cak_x_restore_db                       11
#define cak_x_restore_pages                    12
#define cak_x_restore_log                      13
#define cak_x_restore_config                   14
#define cak_x_save_log                         15
#define cak_x_save_database                    16
#define cak_x_save_pages                       17
#define cak_x_save_log_cold                    18
#define cak_x_verify                           19
#define cak_x_rest_log_from                    20
#define cak_x_extended_check                   24
    /* PTS 1115982 E.Z. */
#define cak_x_init_after_load                  37
#define cak_x_upd_statistics                   38
    /* PTS 1111289 E.Z. */
#define cak_x_shutdown                         42
#define cak_x_suspend                          43
#define cak_x_resume                           44
#define cak_x_write_on                         45
#define cak_x_write_off                        46
    /* PTS 1111576 E.Z. */
    /* PTS 1120104 E.Z. */
    /* PTS 1115978 E.Z. */
#define cak_x_set_nolog_off                    57
#define cak_x_add_devspace                     58
#define cak_x_timeout_on                       62
#define cak_x_timeout_off                      63
#define cak_x_diagnose                         80
#define cak_x_reset_all                        81
#define cak_x_reset                            82
#define cak_x_vtrace_on                        97
#define cak_x_vtrace_off                       98
#define cak_x_diag_index                      101
#define cak_x_diag_dump                       102
#define cak_x_activate                        122
#define cak_x_upd_stat_col                    123
#define cak_x_upd_stat_system_table           124
#define cak_x_tab_on_write                    129
#define cak_x_tab_off_write                   130
#define cak_x_alter_mapset                    135
#define cak_x_create_mapset                   136
#define cak_x_drop_mapset                     137
#define cak_x_get_mapset                      138
#define cak_x_on_diag_parse                   147
#define cak_x_off_diag_parse                  148
    /* PTS 1115978 E.Z. */
#define cak_x_read_label                      150
#define cak_x_end                             152
#define cak_x_autosave                        155
#define cak_x_monitor                         156
#define cak_x_force_checkpoint                157
#define cak_x_get_restart_version             158
#define cak_x_diagnose_analyze                161
#define cak_x_switch                          162
#define cak_x_set_parameter                   163
#define cak_x_delete_log_from_to              164    /* PTS 1108247 E.Z. */
#define cak_x_init_standby                    165    /* PTS 1111229 E.Z. */
#define cak_x_prepare_standby                 166    /* PTS 1111229 E.Z. */
#define cak_x_restart_standby                 167    /* PTS 1111229 E.Z. */
#define cak_x_register_standby                168    /* PTS 1111229 E.Z. */
#define cak_x_insert_standby                  169    /* PTS 1111229 E.Z. */
#define cak_x_remove_standby                  170    /* PTS 1111229 E.Z. */
#define cak_x_takeover                        171    /* PTS 1111229 E.Z. */
#define cak_x_synchronize_log                 172    /* PTS 1111229 E.Z. */
#define cak_x_write_backuphistory             173    /* PTS 1129960 mb   */
#define cak_x_write_backupmedium              174    /* PTS 1129960 mb   */
#define cak_x_log_devspace_on                 180    /* PTS 1113190 E.Z. */
#define cak_x_log_devspace_off                181    /* PTS 1113190 E.Z. */
#define cak_x_migrate_database                190    /* PTS 1117050 E.Z. */
#define cak_x_migrate_to_unicode              191    /* PTS 1117050 E.Z. */
#define cak_x_migrate_volumes                 192    /* PTS 1124973 E.Z. */
#define cak_x_get_bad_log_volume              200    /* PTS 1117472 E.Z. */
#define cak_x_restore_bad_log                 201    /* PTS 1117472 E.Z. */
#define cak_x_auto_overwrite_on               210    /* PTS 1120104 E.Z. */
#define cak_x_auto_overwrite_off              211    /* PTS 1120104 E.Z. */
#define cak_x_get_block                       212    /* PTS 1119691 E.Z. */
#define cak_x_set_block                       213    /* PTS 1119691 E.Z. */
#define cak_x_delete_block                    214    /* PTS 1119691 E.Z. */
#define cak_x_create_snapshot                 215
#define cak_x_drop_snapshot                   216
#define cak_x_restore_snapshot                217
#define cak_x_drop_data_volume                218
#define cak_x_recreate_index                  219    /* PTS 1134856 FF */
#define cak_x_check_index                     220    /* PTS 1134856 FF */
#define cak_x_check_table_system_table        221
    /* used together with a43 */
#define cak_x_explain                           4
    /* used together with a51 */
#define cak_x_connect                           1
#define cak_x_current_user                      2
#define cak_x_set_lang                          3
#define cak_x_set_format                        6
#define cak_x_set_degree                        7
#define cak_x_get_special_register              8
#define cak_x_set_isolevel                      9
#define cak_x_nls_date_format                  11
#define cak_x_set_session_trace                14
#define cak_x_set_role                         15
#define cak_x_authentication_init              16
    /* PTS 1118840 T.A. */
#define cak_x_set_session_unicode              20
#define cak_x_set_session_replication          21
    /* used together with a52 */
#define cak_x_commit                            1
#define cak_x_rollback                          2
#define cak_x_release                           3
#define cak_x_lock                              4
#define cak_x_nowait                            5
#define cak_x_table                             6
#define cak_x_rows                              7
#define cak_x_row                               8
#define cak_x_exclusive                         9
#define cak_x_share                            10
#define cak_x_unlock                           12
#define cak_x_begin_subtrans                   13
#define cak_x_end_subtrans                     14
#define cak_x_rollback_subtrans                15
#define cak_x_optimistic                       16
#define cak_x_force_savepoint                  17
#define cak_x_inner_trans_begin                18
#define cak_x_inner_trans_commit               19
#define cak_x_inner_trans_rollback             20
    /* used together with a55 */
#define cak_x_search_clause                     1
#define cak_x_keyspec_list                      2
#define cak_x_keyspec                           3
#define cak_x_current_of                        4
#define cak_x_pos_of                            5
    /* used together with a56 */
#define cak_x_normal_insert                     1
#define cak_x_enum_values                       2
#define cak_x_column_list                       3
#define cak_x_value_list                        4
#define cak_x_ignore_dupl                       5
#define cak_x_update_dupl                       6
#define cak_x_reject_dupl                       7
#define cak_x_named_values                      8
#define cak_x_set_clause                        9
#define cak_x_insert_select                    10
#define cak_x_minsert                          11
#define cak_x_pseudo_ins_select                12
    /* used together with a57 */
#define cak_x_update                            1
#define cak_x_update_set                        2
#define cak_x_one_update_col                    3
#define cak_x_mupdate                           4
    /* PTS 1111576 E.Z. */
#define cak_x_append                            7
    /* used together with a58 */
#define cak_x_delete                            1
#define cak_x_mdelete                           2
    /* used together with a60 */
#define cak_x_select_list                       1
#define cak_x_select_column                     2
#define cak_x_single_select                     3
#define cak_x_lock_nowait                       4
#define cak_x_lock_exclusive                    5
#define cak_x_lock_share                        6
#define cak_x_lock_optimistic                   7
#define cak_x_lock_ignore                       8
#define cak_x_limit                             9
#define cak_x_top_percent                      10
#define cak_x_top_ties                         11
    /* used together with a62 */
#define cak_x_select_direct                     1
    /* used together with a63 */
#define cak_x_mass_select                       1
#define cak_x_distinct                          2
#define cak_x_from_part                         3
#define cak_x_decl_cursor                       4
#define cak_x_order                             5
#define cak_x_for_update                        6
#define cak_x_group_by                          7
#define cak_x_having                            8
#define cak_x_sort_spec                         9
#define cak_x_search_condition                 10
#define cak_x_for_reuse                        11
#define cak_x_subquery                         12
#define cak_x_start_union                      13
#define cak_x_union                            14
#define cak_x_union_all                        15
#define cak_x_except                           16
#define cak_x_except_all                       17
#define cak_x_intersect                        18
#define cak_x_intersect_all                    19
#define cak_x_res_delete                       20
#define cak_x_res_no_delete                    21
#define cak_x_for_fetch_only                   23
#define cak_x_recursive_select                 24
#define cak_x_new_columnname_list              26
#define cak_x_one_val_subquery                 27
#define cak_x_arith_with_subquery              28
#define cak_x_corr_subquery                    29
#define cak_x_one_val_corr_sub                 30
    /* used together with a64 */
#define cak_x_or_operator                       1
#define cak_x_and_operator                      2
#define cak_x_predicate                         3
#define cak_x_add_operator                      4
#define cak_x_mult_operator                     5
#define cak_x_between_pred                      6
#define cak_x_in_pred                           7
#define cak_x_in_pred_list                      8
#define cak_x_like_pred                         9
#define cak_x_sounds_pred                      10
#define cak_x_duration                         11
#define cak_x_left_outer_join                  12
#define cak_x_right_outer_join                 13
#define cak_x_full_outer_join                  14
#define cak_x_escape                           15
#define cak_x_truth_value                      16
#define cak_x_value_expression                 17
    /* used together with a66 */
#define cak_x_given_sequence                    1
#define cak_x_table_spec                        2
#define cak_x_select_in_from_part               3
    /* used together with a73 */
#define cak_x_fetch                             1
#define cak_x_mfetch                            2
#define cak_x_first                             3
#define cak_x_next                              4
#define cak_x_last                              5
#define cak_x_prev                              6
#define cak_x_same                              7
#define cak_x_pos                               8
#define cak_x_params                            9
#define cak_x_close                            10
#define cak_x_describe                         11
#define cak_x_drop                             12
#define cak_x_stamp_get                        13
    /* PTS 1111576 E.Z. */
#define cak_x_describe_parsid                  15
#define cak_x_relative                         17
    /* used together with a80 */
#define cak_x_select_hint                       1
#define cak_x_delete_hint                       2
#define cak_x_update_hint                       4
#define cak_x_join_hint                         5
#define cak_x_buildresult_hint                  6
#define cak_x_keyaccess_hint                    7
#define cak_x_keyscan_hint                      8
#define cak_x_indexaccess_hint                  9
#define cak_x_indexscan_hint                   10
#define cak_x_enablemerge_hint                 11
#define cak_x_disableinvonly_hint              12
#define cak_x_trace_hint                       13
#define cak_x_L1_strat_hint                    14
#define cak_x_L2_strat_hint                    15
#define cak_x_L2_more_strat_hint               16
#define cak_x_L2_range_strat_hint              17
#define cak_x_L2_strat_no_optim_hint           18
#define cak_x_in_strat_hint                    19
#define cak_x_subq_strat_hint                  20
#define cak_x_fetchresult_hint                 21
#define cak_x_nopath_hint                      22
#define cak_x_ordered_hint                     23
#define cak_x_access_hint                      24
#define cak_x_keyrange_hint                    25
#define cak_x_indexrange_hint                  26
#define cak_x_coordinator_join_hint            27
#define cak_x_operator_join_hint               28
#define cak_x_buffersize_hint                  29
#define cak_x_pretend_param_hint               30
#define cak_x_parallel_serv_hint               31
#define cak_x_insert_hint                      32
#define cak_x_hashratio_hint                   33
    /* fp_consts */
#define cak_fp_date                    (- 1)
#define cak_fp_time                    (- 2)
#define cak_fp_timestamp               (- 3)
#define cak_fp_utcdate                 (- 4)
#define cak_fp_utcdiff                 (- 5)
#define cak_fp_timezone                (- 6)
#define cak_fp_stamp                   (- 7)
#define cak_fp_transaction             (- 8)
#define cak_fp_current_schema          (- 9)
#define cak_fp_nextval                 (- 10)    /* must be last of cak_fp_..*/
#define cak_time_durationlen                    6
    /* consts in parsid together with m_usage */
#define cak_usage_on                            1
#define cak_usage_on_except_trigger             2
#define cak_usage_off                           3
#define cak_usage_add                           4
    /* char-constants */
#define cak_tempinfo_byte              csp_undef_byte
    /* entrytyp */
#define cak_edummy                      "\x00\x00"
#define cak_etable                      "\x00\x01"
#define cak_ctable                     '\x01'
#define cak_eunused                     "\x00\x05"
#define cak_epriv                       "\x00\x06"
#define cak_cpriv                      '\x06'
#define cak_eprivuser                   "\x00\x07"
#define cak_cprivuser                  '\x07'
#define cak_eviewtext                   "\x00\x08"
#define cak_cviewtext                  '\x08'
#define cak_eviewdesc                   "\x00\x0A"
#define cak_cviewdesc                  '\n'
    /*   evfile                =  '\00\0E' in vgg00   */
#define cak_elinkdef                    "\x00\x0F"
#define cak_clinkdef                   '\x0F'
#define cak_eprivproc                   "\x00\x10"
#define cak_cprivproc                  '\x10'
#define cak_edefault                    "\x00\x11"
#define cak_cdefault                   '\x11'
#define cak_econstraint                 "\x00\x12"
#define cak_cconstraint                '\x12'
#define cak_eforeignkey                 "\x00\x13"
#define cak_cforeignkey                '\x13'
#define cak_eprimarykey                 "\x00\x14"
#define cak_cprimarykey                '\x14'
#define cak_etrigger                    "\x00\x15"
#define cak_ctrigger                   '\x15'
#define cak_econstraintname             "\x00\x16"
#define cak_cconstraintname            '\x16'
#define cak_ealterdate                  "\x00\x17"
#define cak_calterdate                 '\x17'
#define cak_epermmessblock              "\x00\x18"
#define cak_cpermmessblock             '\x18'
#define cak_econtainer                  "\x00\x19"
#define cak_ccontainer                 '\x19'
#define cak_eviewqual_basis             "\x00\x20"
#define cak_cviewqual_basis            ' '
#define cak_eviewqual_join              "\x00\x21"
#define cak_cviewqual_join             '!'
#define cak_eviewqual_stack             "\x00\x22"
#define cak_cviewqual_stack            '\"'
#define cak_eviewqual_expcol            "\x00\x23"
#define cak_cviewqual_expcol           '#'
#define cak_eviewqual_derivedcol        "\x00\x24"
#define cak_cviewqual_derivedcol       '$'
#define cak_eviewqual_join_check        "\x00\x25"
#define cak_cviewqual_join_check       '%'
#define cak_esysfile2                   "\x00\x40"
#define cak_etableref                   "\x00\x41"
#define cak_ctableref                  'A'
#define cak_euser                       "\x00\x42"
#define cak_cuser                      'B'
#define cak_eserverdb                   "\x00\x43"
#define cak_cserverdb                  'C'
#define cak_esiteset                    "\x00\x44"
#define cak_csiteset                   'D'
#define cak_eserverdbname               "\x00\x45"
#define cak_cserverdbname              'E'
#define cak_edomain                     "\x00\x46"
#define cak_cdomain                    'F'
#define cak_eusertab                    "\x00\x47"
#define cak_cusertab                   'G'
#define cak_eusage                      "\x00\x48"
#define cak_cusage                     'H'
#define cak_esysdba                     "\x00\x49"
#define cak_csysdba                    'I'
#define cak_eprocpriv                   "\x00\x4B"
#define cak_cprocpriv                  'K'
#define cak_euserref                    "\x00\x4D"
#define cak_cuserref                   'M'
#define cak_edomainref                  "\x00\x4E"
#define cak_cdomainref                 'N'
#define cak_emapset                     "\x00\x51"
#define cak_cmapset                    'Q'
#define cak_esequence                   "\x00\x52"
#define cak_csequence                  'R'
#define cak_esequencepriv               "\x00\x53"
#define cak_csequencepriv              'S'
#define cak_db2esiteset                 "\x00\x54"
#define cak_db2csiteset                'T'
#define cak_euserid                     "\x00\x55"
#define cak_cuserid                    'U'
    /* PTS 1111576 E.Z. */
#define cak_eseqfree                    "\x00\x57"
#define cak_cseqfree                   'W'
#define cak_efuncref                    "\x00\x58"
#define cak_cfuncref                   'X'
#define cak_efunccode                   "\x00\x59"
#define cak_cfunccode                  'Y'
#define cak_ecomment                    "\x00\x60"
#define cak_ccomment                   '`'
#define cak_edomainusage                "\x00\x61"
#define cak_cdomainusage               'a'
    /* PTS 1111576 E.Z. */
#define cak_estatistics                 "\x00\x66"
#define cak_cstatistics                'f'
#define cak_emindex                     "\x00\x67"
#define cak_cmindex                    'g'
#define cak_eroletab                    "\x00\x68"
#define cak_croletab                   'h'
#define cak_emethod                     "\x00\x69"
#define cak_cmethod                    'i'
#define cak_emethodref                  "\x00\x70"
#define cak_cmethodref                 'p'
#define cak_etype_uid                   "\x00\x71"
#define cak_ctype_uid                  'q'
#define cak_emethodinfo                 "\x00\x72"
#define cak_cmethodinfo                'r'
#define cak_eclassmap                   "\x00\x73"
#define cak_cclassmap                  's'
#define cak_eversion                    "\x00\x74"
#define cak_cversion                   't'
#define cak_eschemaref                  "\x00\x75"
#define cak_cschemaref                 'u'
#define cak_eschema                     "\x00\x76"
#define cak_cschema                    'v'
#define cak_emethodfileinfo             "\x00\x77"
#define cak_cmethodfileinfo            'w'
#define cak_emethoddebuginfo            "\x00\x78"
#define cak_cmethoddebuginfo           'x'
#define cak_etemp                       "\x00\x80"
#define cak_ctemp                      0x80
#define cak_eparsinfo                   "\x00\x81"
#define cak_cparsinfo                  0x81
#define cak_ejparsinfo                  "\x00\x82"
#define cak_cjparsinfo                 0x82
#define cak_ecorrinfo                   "\x00\x83"
#define cak_ccorrinfo                  0x83
#define cak_eshortinfo                  "\x00\x84"
#define cak_cshortinfo                 0x84
#define cak_ehint                       "\x00\x85"
#define cak_chint                      0x85
#define cak_estratinfo                  "\x00\x86"
#define cak_cstratinfo                 0x86
#define cak_eunionrec                   "\x00\x87"
#define cak_cunionrec                  0x87
#define cak_eviewkey                    "\x00\x88"
#define cak_cviewkey                   0x88
#define cak_ecomplex                    "\x00\x89"
#define cak_ccomplex                   0x89
    /* PTS 1111575 E.Z. */
#define cak_etemppriv                   "\x00\x8B"
#define cak_ctemppriv                  0x8b
#define cak_eressiteinfo                "\x00\x8D"
#define cak_cressiteinfo               0x8d
#define cak_elinkparsinfo               "\x00\x94"
#define cak_clinkparsinfo              0x94
#define cak_etriggerparsinfo            "\x00\x95"
#define cak_ctriggerparsinfo           0x95
#define cak_esubtrans                   "\x00\x96"
#define cak_csubtrans                  0x96
#define cak_estored_cmd                 "\x00\x97"
#define cak_cstored_cmd                0x97
#define cak_echangeinfos                "\x00\x98"
#define cak_cchangeinfos               0x98
#define cak_elongliteral                "\x00\x99"
#define cak_clongliteral               0x99
#define cak_ejparsinfo_joinarr          "\x00\xA0"
#define cak_cjparsinfo_joinarr         0xa0
#define cak_ejparsinfo_eqarr            "\x00\xA1"
#define cak_cjparsinfo_eqarr           0xa1
#define cak_ejparsinfo_table            "\x00\xA2"
#define cak_cjparsinfo_table           0xa2
#define cak_elogged_temp                "\x00\xB0"
#define cak_clogged_temp               0xb0
#define cak_eresult                     "\x00\xB1"
#define cak_cresult                    0xb1
#define cak_ereskey                     "\x00\xB2"
#define cak_creskey                    0xb2
#define cak_eresname                    "\x00\xB5"
#define cak_cresname                   0xb5
#define cak_etempprocref                "\x00\xB6"
#define cak_ctempprocref               0xb6
#define cak_etemptableref               "\x00\xB7"
#define cak_ctemptableref              0xb7
#define cak_etempdefault                "\x00\xB8"
#define cak_ctempdefault               0xb8
#define cak_etempconstraint             "\x00\xB9"
#define cak_ctempconstraint            0xb9
#define cak_etempviewdesc               "\x00\xBA"
#define cak_ctempviewdesc              0xba
#define cak_etempscoldesc               "\x00\xBB"
#define cak_ctempscoldesc              0xbb
#define cak_etempviewtext               "\x00\xBC"
#define cak_ctempviewtext              0xbc
#define cak_etmpconstraintname          "\x00\xBD"
#define cak_ctmpconstraintname         0xbd
#define cak_elinkpos                    "\x00\xBE"
#define cak_clinkpos                   0xbe
#define cak_edbprocparsinfo             "\x00\xBF"
#define cak_cdbprocparsinfo            0xbf
#define cak_ecolnames                   "\x00\xC0"
#define cak_ccolnames                  0xc0
#define cak_emessblock                  "\x00\xC1"
#define cak_cmessblock                 0xc1
#define cak_ecommand_info               "\x00\xC2"
#define cak_ccommand_info              0xc2
#define cak_esharedSQL_info             "\x00\xC3"
#define cak_csharedSQL_info            0xc3
#define cak_emax                              196    /* '00C4' */
#define cak_i_no_keyword                        0
#define cak_i_abs                               1
#define cak_i_absolute                          2
#define cak_i_acos                              3
#define cak_i_add                               4
#define cak_i_adddate                           5
#define cak_i_address                           6
#define cak_i_addtime                           7
#define cak_i_add_months                        8
#define cak_i_after                             9
#define cak_i_all                              10
#define cak_i_alpha                            11
#define cak_i_alter                            12
#define cak_i_alterin                          13
#define cak_i_analyze                          14
#define cak_i_and                              15
#define cak_i_ansi                             16
#define cak_i_any                              17
#define cak_i_append                           18
#define cak_i_archive                          19
#define cak_i_as                               20
#define cak_i_asc                              21
#define cak_i_ascii                            22
#define cak_i_asin                             23
#define cak_i_at                               24
#define cak_i_atan                             25
#define cak_i_atan2                            26
#define cak_i_authentication                   27
#define cak_i_authorization                    28
#define cak_i_auto                             29
#define cak_i_autosave                         30
#define cak_i_avg                              31
#define cak_i_bad                              32
#define cak_i_before                           33
#define cak_i_begin                            34
#define cak_i_beginload                        35
#define cak_i_beginproc                        36
#define cak_i_between                          37
#define cak_i_binary                           38
#define cak_i_bit                              39
#define cak_i_block                            40
#define cak_i_blocksize                        41
#define cak_i_boolean                          42
#define cak_i_both                             43
#define cak_i_break                            44
#define cak_i_buffer                           45
#define cak_i_bufferpool                       46
#define cak_i_bwhierarchy                      47
#define cak_i_by                               48
#define cak_i_byte                             49
#define cak_i_cache                            50
#define cak_i_cachelimit                       51
#define cak_i_caches                           52
#define cak_i_call                             53
#define cak_i_cancel                           54
#define cak_i_cascade                          55
#define cak_i_case                             56
#define cak_i_catalog                          57
#define cak_i_catch                            58
#define cak_i_ceil                             59
#define cak_i_ceiling                          60
#define cak_i_char                             61
#define cak_i_character                        62
#define cak_i_check                            63
#define cak_i_checkpoint                       64
#define cak_i_chr                              65
#define cak_i_clear                            66
#define cak_i_close                            67
#define cak_i_cluster                          68
#define cak_i_column                           69
#define cak_i_comment                          70
#define cak_i_commit                           71
#define cak_i_compute                          72
#define cak_i_concat                           73
#define cak_i_config                           74
#define cak_i_connect                          75
#define cak_i_constraint                       76
#define cak_i_constraints                      77
#define cak_i_container                        78
#define cak_i_continue                         79
#define cak_i_cos                              80
#define cak_i_cosh                             81
#define cak_i_costlimit                        82
#define cak_i_costwarning                      83
#define cak_i_cot                              84
#define cak_i_count                            85
#define cak_i_create                           86
#define cak_i_createin                         87
#define cak_i_cross                            88
#define cak_i_curdate                          89
#define cak_i_current                          90
#define cak_i_current_date                     91
#define cak_i_current_schema                   92
#define cak_i_current_time                     93
#define cak_i_current_timestamp                94
#define cak_i_currval                          95
#define cak_i_cursor                           96
#define cak_i_curtime                          97
#define cak_i_cycle                            98
#define cak_i_dafs                             99
#define cak_i_data                            100
#define cak_i_database                        101
#define cak_i_date                            102
#define cak_i_datediff                        103
#define cak_i_day                             104
#define cak_i_dayname                         105
#define cak_i_dayofmonth                      106
#define cak_i_dayofweek                       107
#define cak_i_dayofyear                       108
#define cak_i_days                            109
#define cak_i_db2                             110
#define cak_i_dba                             111
#define cak_i_dbproc                          112
#define cak_i_dbprocedure                     113
#define cak_i_debug                           114
#define cak_i_dec                             115
#define cak_i_decimal                         116
#define cak_i_declare                         117
#define cak_i_decode                          118
#define cak_i_default                         119
#define cak_i_defaultcode                     120
#define cak_i_degree                          121
#define cak_i_degrees                         122
#define cak_i_delete                          123
#define cak_i_desc                            124
#define cak_i_describe                        125
#define cak_i_deterministic                   126
#define cak_i_device                          127
#define cak_i_diagnose                        128
#define cak_i_digits                          129
#define cak_i_dimension                       130
#define cak_i_disable                         131
#define cak_i_distinct                        132
#define cak_i_div                             133
#define cak_i_do                              134
#define cak_i_domain                          135
#define cak_i_double                          136
#define cak_i_drop                            137
#define cak_i_dropin                          138
#define cak_i_dsetpass                        139
#define cak_i_duplicates                      140
#define cak_i_dynamic                         141
#define cak_i_editproc                        142
#define cak_i_else                            143
#define cak_i_enable                          144
#define cak_i_end                             145
#define cak_i_endload                         146
#define cak_i_endproc                         147
#define cak_i_error                           148
#define cak_i_escape                          149
#define cak_i_estimate                        150
#define cak_i_eur                             151
#define cak_i_except                          152
#define cak_i_exclusive                       153
#define cak_i_execute                         154
#define cak_i_exists                          155
#define cak_i_exp                             156
#define cak_i_expand                          157
#define cak_i_explain                         158
#define cak_i_explicit                        159
#define cak_i_extended                        160
#define cak_i_fact                            161
#define cak_i_false                           162
#define cak_i_fetch                           163
#define cak_i_file                            164
#define cak_i_first                           165
#define cak_i_fixed                           166
#define cak_i_float                           167
#define cak_i_floor                           168
#define cak_i_flush                           169
#define cak_i_for                             170
#define cak_i_force                           171
#define cak_i_foreign                         172
#define cak_i_format                          173
#define cak_i_freepage                        174
#define cak_i_from                            175
#define cak_i_full                            176
#define cak_i_function                        177
#define cak_i_fversion                        178
#define cak_i_get                             179
#define cak_i_get_objectname                  180
#define cak_i_get_schema                      181
#define cak_i_grant                           182
#define cak_i_graphic                         183
#define cak_i_greatest                        184
#define cak_i_group                           185
#define cak_i_having                          186
#define cak_i_hex                             187
#define cak_i_hextoraw                        188
#define cak_i_high                            189
#define cak_i_history                         190
#define cak_i_hold                            191
#define cak_i_hour                            192
#define cak_i_hours                           193
#define cak_i_identified                      194
#define cak_i_if                              195
#define cak_i_ifnull                          196
#define cak_i_ignore                          197
#define cak_i_implicit                        198
#define cak_i_in                              199
#define cak_i_increment                       200
#define cak_i_index                           201
#define cak_i_indexname                       202
#define cak_i_indicator                       203
#define cak_i_info                            204
#define cak_i_init                            205
#define cak_i_initcap                         206
#define cak_i_initrans                        207
#define cak_i_inner                           208
#define cak_i_inout                           209
#define cak_i_inproc                          210
#define cak_i_insert                          211
#define cak_i_instance                        212
#define cak_i_instr                           213
#define cak_i_int                             214
#define cak_i_integer                         215
#define cak_i_internal                        216
#define cak_i_intersect                       217
#define cak_i_into                            218
#define cak_i_is                              219
#define cak_i_iso                             220
#define cak_i_isolation                       221
#define cak_i_isupdatedcolumn                 222
#define cak_i_java                            223
#define cak_i_jis                             224
#define cak_i_join                            225
#define cak_i_key                             226
#define cak_i_label                           227
#define cak_i_language                        228
#define cak_i_last                            229
#define cak_i_last_day                        230
#define cak_i_lcase                           231
#define cak_i_least                           232
#define cak_i_left                            233
#define cak_i_length                          234
#define cak_i_level                           235
#define cak_i_lfill                           236
#define cak_i_like                            237
#define cak_i_limit                           238
#define cak_i_list                            239
#define cak_i_ln                              240
#define cak_i_load                            241
#define cak_i_local                           242
#define cak_i_locate                          243
#define cak_i_lock                            244
#define cak_i_log                             245
#define cak_i_log10                           246
#define cak_i_logfull                         247
#define cak_i_logwriter                       248
#define cak_i_long                            249
#define cak_i_longfile                        250
#define cak_i_low                             251
#define cak_i_lower                           252
#define cak_i_lpad                            253
#define cak_i_ltrim                           254
#define cak_i_makedate                        255
#define cak_i_maketime                        256
#define cak_i_mapchar                         257
#define cak_i_max                             258
#define cak_i_maxtrans                        259
#define cak_i_maxvalue                        260
#define cak_i_mbcs                            261
#define cak_i_medianame                       262
#define cak_i_medium                          263
#define cak_i_microsec                        264
#define cak_i_microsecs                       265
#define cak_i_migrate                         266
#define cak_i_min                             267
#define cak_i_minus                           268
#define cak_i_minute                          269
#define cak_i_minutes                         270
#define cak_i_minvalue                        271
#define cak_i_mod                             272
#define cak_i_mode                            273
#define cak_i_modify                          274
#define cak_i_monitor                         275
#define cak_i_month                           276
#define cak_i_monthname                       277
#define cak_i_months                          278
#define cak_i_months_between                  279
#define cak_i_name                            280
#define cak_i_natural                         281
#define cak_i_nchar                           282
#define cak_i_new                             283
#define cak_i_new_time                        284
#define cak_i_next                            285
#define cak_i_nextval                         286
#define cak_i_next_day                        287
#define cak_i_nlssort                         288
#define cak_i_nls_currency                    289
#define cak_i_nls_date_format                 290
#define cak_i_nls_date_language               291
#define cak_i_nls_language                    292
#define cak_i_nls_numeric_chars               293
#define cak_i_nls_sort                        294
#define cak_i_no                              295
#define cak_i_nocache                         296
#define cak_i_nocycle                         297
#define cak_i_nolog                           298
#define cak_i_nomaxvalue                      299
#define cak_i_nominvalue                      300
#define cak_i_none                            301
#define cak_i_noorder                         302
#define cak_i_norewind                        303
#define cak_i_normal                          304
#define cak_i_noround                         305
#define cak_i_nosort                          306
#define cak_i_not                             307
#define cak_i_now                             308
#define cak_i_nowait                          309
#define cak_i_null                            310
#define cak_i_num                             311
#define cak_i_number                          312
#define cak_i_numeric                         313
#define cak_i_nvl                             314
#define cak_i_obid                            315
#define cak_i_object                          316
#define cak_i_of                              317
#define cak_i_off                             318
#define cak_i_on                              319
#define cak_i_only                            320
#define cak_i_open                            321
#define cak_i_optimistic                      322
#define cak_i_optimize                        323
#define cak_i_option                          324
#define cak_i_or                              325
#define cak_i_oracle                          326
#define cak_i_order                           327
#define cak_i_out                             328
#define cak_i_outer                           329
#define cak_i_overwrite                       330
#define cak_i_package                         331
#define cak_i_packed                          332
#define cak_i_page                            333
#define cak_i_pages                           334
#define cak_i_param                           335
#define cak_i_parameter                       336
#define cak_i_parse                           337
#define cak_i_parseid                         338
#define cak_i_password                        339
#define cak_i_pctfree                         340
#define cak_i_pctused                         341
#define cak_i_per                             342
#define cak_i_percent                         343
#define cak_i_permanent                       344
#define cak_i_pi                              345
#define cak_i_ping                            346
#define cak_i_pipe                            347
#define cak_i_pos                             348
#define cak_i_power                           349
#define cak_i_precision                       350
#define cak_i_prepare                         351
#define cak_i_prev                            352
#define cak_i_primary                         353
#define cak_i_priv                            354
#define cak_i_privileges                      355
#define cak_i_proc                            356
#define cak_i_procedure                       357
#define cak_i_psm                             358
#define cak_i_public                          359
#define cak_i_quick                           360
#define cak_i_radians                         361
#define cak_i_range                           362
#define cak_i_raw                             363
#define cak_i_rawtohex                        364
#define cak_i_read                            365
#define cak_i_real                            366
#define cak_i_recursive                       367
#define cak_i_references                      368
#define cak_i_register                        369
#define cak_i_reject                          370
#define cak_i_relative                        371
#define cak_i_release                         372
#define cak_i_remote                          373
#define cak_i_remove                          374
#define cak_i_rename                          375
#define cak_i_replace                         376
#define cak_i_replication                     377
#define cak_i_reset                           378
#define cak_i_resource                        379
#define cak_i_restart                         380
#define cak_i_restore                         381
#define cak_i_restrict                        382
#define cak_i_resume                          383
#define cak_i_return                          384
#define cak_i_returns                         385
#define cak_i_reuse                           386
#define cak_i_revoke                          387
#define cak_i_rfill                           388
#define cak_i_right                           389
#define cak_i_role                            390
#define cak_i_rollback                        391
#define cak_i_round                           392
#define cak_i_row                             393
#define cak_i_rowid                           394
#define cak_i_rowno                           395
#define cak_i_rownum                          396
#define cak_i_rows                            397
#define cak_i_rpad                            398
#define cak_i_rtrim                           399
#define cak_i_same                            400
#define cak_i_sample                          401
#define cak_i_sapr3                           402
#define cak_i_save                            403
#define cak_i_savepoint                       404
#define cak_i_schema                          405
#define cak_i_second                          406
#define cak_i_seconds                         407
#define cak_i_select                          408
#define cak_i_selectivity                     409
#define cak_i_selupd                          410
#define cak_i_sequence                        411
#define cak_i_serial                          412
#define cak_i_serverdb                        413
#define cak_i_session                         414
#define cak_i_set                             415
#define cak_i_share                           416
#define cak_i_show                            417
#define cak_i_shutdown                        418
#define cak_i_sign                            419
#define cak_i_sin                             420
#define cak_i_sinh                            421
#define cak_i_smallint                        422
#define cak_i_snapshot                        423
#define cak_i_some                            424
#define cak_i_soundex                         425
#define cak_i_sounds                          426
#define cak_i_space                           427
#define cak_i_sqlid                           428
#define cak_i_sqlmode                         429
#define cak_i_sqrt                            430
#define cak_i_stamp                           431
#define cak_i_standard                        432
#define cak_i_standby                         433
#define cak_i_start                           434
#define cak_i_startpos                        435
#define cak_i_stat                            436
#define cak_i_state                           437
#define cak_i_static                          438
#define cak_i_statistics                      439
#define cak_i_stddev                          440
#define cak_i_stop                            441
#define cak_i_storage                         442
#define cak_i_subdate                         443
#define cak_i_subpages                        444
#define cak_i_substr                          445
#define cak_i_substring                       446
#define cak_i_subtime                         447
#define cak_i_subtrans                        448
#define cak_i_sum                             449
#define cak_i_suspend                         450
#define cak_i_switch                          451
#define cak_i_synchronize                     452
#define cak_i_synonym                         453
#define cak_i_sysdate                         454
#define cak_i_sysdba                          455
#define cak_i_system                          456
#define cak_i_table                           457
#define cak_i_tablespace                      458
#define cak_i_takeover                        459
#define cak_i_tan                             460
#define cak_i_tanh                            461
#define cak_i_tape                            462
#define cak_i_temp                            463
#define cak_i_then                            464
#define cak_i_time                            465
#define cak_i_timediff                        466
#define cak_i_timeout                         467
#define cak_i_timestamp                       468
#define cak_i_timezone                        469
#define cak_i_to                              470
#define cak_i_toidentifier                    471
#define cak_i_top                             472
#define cak_i_to_char                         473
#define cak_i_to_date                         474
#define cak_i_to_number                       475
#define cak_i_trace                           476
#define cak_i_transaction                     477
#define cak_i_translate                       478
#define cak_i_trigger                         479
#define cak_i_trim                            480
#define cak_i_true                            481
#define cak_i_trunc                           482
#define cak_i_truncate                        483
#define cak_i_try                             484
#define cak_i_type                            485
#define cak_i_ucase                           486
#define cak_i_uid                             487
#define cak_i_unicode                         488
#define cak_i_union                           489
#define cak_i_unique                          490
#define cak_i_unknown                         491
#define cak_i_unload                          492
#define cak_i_unlock                          493
#define cak_i_until                           494
#define cak_i_unused                          495
#define cak_i_update                          496
#define cak_i_upper                           497
#define cak_i_usa                             498
#define cak_i_usage                           499
#define cak_i_user                            500
#define cak_i_usergroup                       501
#define cak_i_userid                          502
#define cak_i_using                           503
#define cak_i_utcdate                         504
#define cak_i_utcdiff                         505
#define cak_i_validproc                       506
#define cak_i_value                           507
#define cak_i_values                          508
#define cak_i_varchar                         509
#define cak_i_varchar2                        510
#define cak_i_vargraphic                      511
#define cak_i_variance                        512
#define cak_i_verify                          513
#define cak_i_version                         514
#define cak_i_view                            515
#define cak_i_volume                          516
#define cak_i_vsize                           517
#define cak_i_vtrace                          518
#define cak_i_wait                            519
#define cak_i_week                            520
#define cak_i_weekofyear                      521
#define cak_i_when                            522
#define cak_i_whenever                        523
#define cak_i_where                           524
#define cak_i_while                           525
#define cak_i_with                            526
#define cak_i_work                            527
#define cak_i_write                           528
#define cak_i_writer                          529
#define cak_i_year                            530
#define cak_i_years                           531
#define cak_i_zoned                           532
#define cak_maxkeyword                        532
    /*=======================================================*/
    /* fileserver constants                                  */
    /*=======================================================*/
#define cak_l_ascii_sq_bracket         '['
#define cak_r_ascii_sq_bracket         ']'
#define mxak_dm_filler1                        18
#define MAX_JOINARR_ELEMS_AK00         MAX_INT2_SP00    /* <= MAX_INT2_SP00 */

/******************************************
 *               T Y P E S                *
 ******************************************/


typedef enum tak_complex_call_reason_Enum
{
    c_increment,
    c_set_last_key,
    c_set_last_pars,
    c_set_p_no,
    c_set_recursive
} tak_complex_call_reason_Enum;
typedef tak_complex_call_reason_Enum tak_complex_call_reason_Param;
typedef ENUM_TYPE_MSP00 (          tak_complex_call_reason, tak_complex_call_reason_Enum, c_set_recursive, unsigned char);

typedef enum tak_object_type_Enum
{
    obj_domain,
    obj_table,
    obj_temp_table,
    obj_trigger
} tak_object_type_Enum;
typedef tak_object_type_Enum tak_object_type_Param;
typedef ENUM_TYPE_MSP00 (          tak_object_type, tak_object_type_Enum, obj_trigger, unsigned char);

typedef enum tak_allow_type_Enum
{
    at_has_to_be_there,
    at_may_be_there,
    at_not_allowed
} tak_allow_type_Enum;
typedef tak_allow_type_Enum tak_allow_type_Param;
typedef ENUM_TYPE_MSP00 (          tak_allow_type, tak_allow_type_Enum, at_not_allowed, unsigned char);

typedef enum tak_progusagetyp_Enum
{
    p_notyp,
    p_column,
    p_file,
    p_index,
    p_synonym,
    p_table,
    p_user,
    p_view,
    p_domain,
    p_procedure,
    p_foreign_key
} tak_progusagetyp_Enum;
typedef tak_progusagetyp_Enum tak_progusagetyp_Param;
typedef ENUM_TYPE_MSP00 (          tak_progusagetyp, tak_progusagetyp_Enum, p_foreign_key, unsigned char);

typedef enum tak_sc_symbol_Enum
{
    s_unknown,
    s_comma,
    s_divide,
    s_eof,
    s_equal,
    s_greater,
    s_greater_or_eq,
    s_leftpar,
    s_less,
    s_less_or_eq,
    s_minus,
    s_plus,
    s_point,
    s_rightpar,
    s_semicolon,
    s_asterisk,
    s_unequal,
    s_key,
    s_not_null,
    s_ascii,
    s_updated,
    s_byte,
    s_byte_string,
    s_date,
    s_time,
    s_user,
    s_notin,
    s_in,
    s_notbetween,
    s_between,
    s_default,
    s_character,
    s_fixed,
    s_float,
    s_file,
    s_smallint,
    s_varchar,
    s_longvarchar,
    s_integer,
    s_fixed_point_literal,
    s_floating_point_literal,
    s_identifier,
    s_authid,
    s_tablename,
    s_tableid,
    s_columnname,
    s_columnid,
    s_parameter_name,
    s_special_identifier,
    s_string_literal,
    s_unsigned_integer,
    s_null,
    s_desc,
    s_or,
    s_not,
    s_and,
    s_is_not_null,
    s_is_null,
    s_count,
    s_avg,
    s_min,
    s_max,
    s_sum,
    s_abs,
    s_trunc,
    s_div,
    s_mod,
    s_concat,
    s_substr,
    s_trim,
    s_length,
    s_vsize,
    s_upper,
    s_lower,
    s_lpad,
    s_rpad,
    s_num,
    s_chr,
    s_all,
    s_any,
    s_reference_name,
    s_asc,
    s_all_priv,
    s_owner,
    s_insert,
    s_delete,
    s_update,
    s_select,
    s_selupd,
    s_index,
    s_rfill,
    s_lfill,
    s_dis_count,
    s_dis_avg,
    s_dis_sum,
    s_vs_decimal,
    s_vs_zoned,
    s_all_count,
    s_adddate,
    s_subdate,
    s_datediff,
    s_dayofweek,
    s_weekofyear,
    s_dayofyear,
    s_addtime,
    s_subtime,
    s_timediff,
    s_next_day,
    s_rowno,
    s_maketime,
    s_round,
    s_mapchar,
    s_usergroup,
    s_packed,
    s_binary,
    s_makedate,
    s_exists,
    s_expand,
    s_stamp,
    s_mult,
    s_like,
    s_notlike,
    s_sounds,
    s_notsounds,
    s_value,
    s_func_char,
    s_day,
    s_days,
    s_digits,
    s_hex,
    s_hour,
    s_microsecond,
    s_minute,
    s_month,
    s_second,
    s_timestamp,
    s_year,
    s_timezone,
    s_alter,
    s_outer_join,
    s_soundex,
    s_double_precision,
    s_link,
    s_number,
    s_sysdba,
    s_func_date,
    s_func_time,
    s_func_timestamp,
    s_colon,
    s_hostfilename,
    s_midnight,
    s_graphic,
    s_vargraphic,
    s_long_varchar,
    s_long_vargraphic,
    s_noround,
    s_is_true,
    s_is_not_true,
    s_is_false,
    s_is_not_false,
    s_to_number,
    s_ltrim,
    s_rtrim,
    s_initcap,
    s_translate,
    s_replace,
    s_ceil,
    s_floor,
    s_sign,
    s_mod_func,
    s_sqrt,
    s_power,
    s_exp,
    s_ln,
    s_log,
    s_sin,
    s_cos,
    s_tan,
    s_sinh,
    s_cosh,
    s_tanh,
    s_greatest,
    s_least,
    s_decode,
    s_stddev,
    s_dis_stddev,
    s_variance,
    s_dis_variance,
    s_chr_ora,
    s_hextoraw,
    s_add_months,
    s_last_day,
    s_months_between,
    s_dba,
    s_nextval,
    s_sequence,
    s_to_24_char,
    s_uid,
    s_rowid,
    s_user_func,
    s_currval,
    s_real,
    s_new_time,
    s_to_date,
    s_to_char,
    s_old_long,
    s_acos,
    s_asin,
    s_atan,
    s_atan2,
    s_cot,
    s_log10,
    s_pi,
    s_radians,
    s_degrees,
    s_left,
    s_right,
    s_space,
    s_dayofmonth,
    s_monthname,
    s_boolean,
    s_true,
    s_false,
    s_toidentifier,
    s_degree,
    s_cast,
    s_database,
    s_where,
    s_having,
    s_dayname,
    s_unicode,
    s_mbcs,
    s_transaction,
    s_serial,
    s_method_call,
    s_hint,
    s_utcdiff,
    s_utcdate,
    s_get_schema,
    s_get_name,
    s_case,
    s_now,
    s_current_schema
} tak_sc_symbol_Enum;
typedef tak_sc_symbol_Enum tak_sc_symbol_Param;
typedef ENUM_TYPE_MSP00 (          tak_sc_symbol, tak_sc_symbol_Enum, s_current_schema, unsigned char);

typedef enum tak_procs_Enum
{
    no_proc,
    a10,
    a11,
    a12,
    a13,
    a14,
    a15,
    a16,
    a17,
    a18,
    a19,
    a20,
    a21,
    a22,
    a23,
    a24,
    a25,
    a26,
    a27,
    a28,
    a29,
    a30,
    a31,
    a32,
    a33,
    a34,
    a35,
    a36,
    a37,
    a38,
    a39,
    a40,
    a41,
    a42,
    a43,
    a44,
    a45,
    a46,
    a47,
    a48,
    a49,
    a50,
    a51,
    a52,
    a53,
    a54,
    a55,
    a56,
    a57,
    a58,
    a59,
    a60,
    a61,
    a62,
    a63,
    a64,
    a65,
    a66,
    a67,
    a68,
    a69,
    a70,
    a71,
    a72,
    a73,
    a74,
    a75,
    a76,
    a77,
    a78,
    a79,
    a80,
    a81,
    a82,
    a83,
    a84,
    a85,
    a86,
    a87,
    a88,
    a89,
    a90,
    a63query_spec,
    a92fromsel,
    a260,
    a261,
    a262,
    a264,
    a641,
    a999ptr_node
} tak_procs_Enum;
typedef tak_procs_Enum tak_procs_Param;
typedef ENUM_TYPE_MSP00 (          tak_procs, tak_procs_Enum, a999ptr_node, unsigned char);

typedef enum tak_usertyp_Enum
{
    usuperdba,
    udba,
    uprivate,
    unoprivate,
    usysdba,
    ucontroluser
} tak_usertyp_Enum;
typedef tak_usertyp_Enum tak_usertyp_Param;
typedef ENUM_TYPE_MSP00 (          tak_usertyp, tak_usertyp_Enum, ucontroluser, unsigned char);

typedef enum tak_ddl_descriptor_Enum
{
    no_ddl,
    ddl_alter_password,
    ddl_alter_tab_add,
    ddl_alter_tab_alter,
    ddl_alter_tab_drop,
    ddl_alter_user,
    ddl_unused9,
    ddl_begin_end_load,
    ddl_comment_on,
    ddl_create_domain,
    ddl_create_index,
    ddl_create_link,
    ddl_create_filler2,
    ddl_create_package,
    ddl_create_procedure,
    ddl_create_role,
    ddl_create_filler1,
    ddl_create_synonym,
    ddl_create_table,
    ddl_create_user,
    ddl_create_view,
    ddl_db2,
    ddl_drop_domain,
    ddl_drop_index,
    ddl_drop_link,
    ddl_drop_filler2,
    ddl_drop_package,
    ddl_drop_procedure,
    ddl_drop_role,
    ddl_drop_filler1,
    ddl_drop_synonym,
    ddl_drop_table,
    ddl_drop_user,
    ddl_drop_view,
    ddl_grant_execute,
    ddl_grant_privilege,
    ddl_grant_user,
    ddl_unused10,
    ddl_rename_column,
    ddl_rename_synonym,
    ddl_rename_table,
    ddl_rename_view,
    ddl_restore_schema,
    ddl_revoke_execute,
    ddl_revoke_privilege,
    ddl_save_restore_cmd,
    ddl_store_procedure,
    ddl_usage_off,
    ddl_update_statistics,
    ddl_create_trigger,
    ddl_create_as_select,
    ddl_drop_trigger,
    ddl_add_constraint,
    ddl_drop_constraint,
    ddl_alter_constraint,
    ddl_create_sequence,
    ddl_drop_sequence,
    ddl_alter_key,
    ddl_rename_index,
    ddl_unused2,
    ddl_unused3,
    ddl_unused4,
    ddl_truncate,
    ddl_unused5,
    ddl_unused6,
    ddl_create_schema,
    ddl_alter_table,
    ddl_drop_schema,
    ddl_unused8,
    ddl_alter_dbproc,
    ddl_rename_user,
    ddl_create_dbfunc,
    ddl_alter_trigger
} tak_ddl_descriptor_Enum;
typedef tak_ddl_descriptor_Enum tak_ddl_descriptor_Param;
typedef ENUM_TYPE_MSP00 (          tak_ddl_descriptor, tak_ddl_descriptor_Enum, ddl_alter_trigger, unsigned char);

typedef enum tak_proc_compile_type_Enum
{
    pct_none,
    pct_dbproc,
    pct_function,
    pct_insert_trigger,
    pct_update_trigger,
    pct_delete_trigger,
    pct_multi_trigger
} tak_proc_compile_type_Enum;
typedef tak_proc_compile_type_Enum tak_proc_compile_type_Param;
typedef ENUM_TYPE_MSP00 (          tak_proc_compile_type, tak_proc_compile_type_Enum, pct_multi_trigger, unsigned char);

typedef enum tak_convert_type_Enum
{
    conv_none,
    conv_ascii_to_unicode,
    conv_unicode_to_ascii,
    conv_ascii_to_num,
    conv_unicode_to_num,
    conv_num_to_ascii,
    conv_num_to_unicode
} tak_convert_type_Enum;
typedef tak_convert_type_Enum tak_convert_type_Param;
typedef ENUM_TYPE_MSP00 (          tak_convert_type, tak_convert_type_Enum, conv_num_to_unicode, unsigned char);
    /* */

typedef enum tak_table_attribute_set__Enum
{
    ta_fact,
    ta_dimension,
    ta_bwhierarchy,
    ta_no_references_view,
    ta_no_fixed_length_column,
    ta_cluster_key
} tak_table_attribute_set__Enum;
typedef tak_table_attribute_set__Enum tak_table_attribute_set__Param;
typedef SMALLSET_TYPE_MSP00 (      tak_table_attribute_set, tak_table_attribute_set__Enum, ta_cluster_key);
    /* */
typedef ARRAY_TYPE_MSP00 (         tak_two_int2, tsp00_Int2, 2);

typedef struct                     tak_connect_rec
{
    tsp00_KnlIdentifier cn_username;
    tsp00_Int2          cn_timeout;
    tsp00_CryptPw       cn_password;
    tgg00_HandlingSet   cn_global_state;
    tsp00_Int4          cn_tmp_cache_size;
    tsp00_C8            cn_receivedPw;
    tsp00_Uint1         cn_messcode;
    pasbool             cn_internal;
    pasbool             cn_mysql_connect;
    pasbool             cn_oldVersion;
} tak_connect_rec;

typedef struct                     tak_sm_description
{
    tsp00_Int2          sm_error_cnt;
    pasbool             sm_must_local;
    pasbool             sm_return_req;
    pasbool             sm_not_local;
    pasbool             sm_filler1;
    tgg00_BasisError    sm_expected_error;
    ARRAY_VAR_MSP00 (         sm_errorlist, tgg00_BasisError, cak_max_sm_error);
} tak_sm_description;

typedef struct                     tak_user_info
{
    tgg00_Surrogate     ui_id;
    tgg00_Surrogate     ui_owner;
    pasbool             ui_is_role;
    tak_usertyp         ui_kind;
} tak_user_info;

typedef struct                     tak_parskey
{
    tsp00_C3            p_count;
    tsp00_C1            p_id;
    tgg00_MessType      p_kind;
    tsp00_Uint1         p_no;
    tsp00_Int2          p_filler;
} tak_parskey;

typedef struct                     tak_parsid
{
    tgg91_SessionNo     pid_session;
    union
    {
    /* true */
        struct tak_parsid_true
        {
            tak_parskey         pid_parsk_F;
        } C_true;
    /* false */
        struct tak_parsid_false
        {
            tsp00_C4            pid_fill1_F;
            tsp00_C2            pid_fill2_F;
            tsp00_C1            pid_appl_info_F;
            tsp00_C1            pid_dtime_info_F;
        } C_false;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tak_parskey        &pid_parsk              ()  { return this->variant.C_true.pid_parsk_F; };
    tsp00_C4           &pid_fill1              ()  { return this->variant.C_false.pid_fill1_F; };
    tsp00_C2           &pid_fill2              ()  { return this->variant.C_false.pid_fill2_F; };
    tsp00_C1           &pid_appl_info          ()  { return this->variant.C_false.pid_appl_info_F; };
    tsp00_C1           &pid_dtime_info         ()  { return this->variant.C_false.pid_dtime_info_F; };
    /* const accessor functions */
    const tak_parskey  &pid_parsk              () const  { return this->variant.C_true.pid_parsk_F; };
    const tsp00_C4     &pid_fill1              () const  { return this->variant.C_false.pid_fill1_F; };
    const tsp00_C2     &pid_fill2              () const  { return this->variant.C_false.pid_fill2_F; };
    const tsp00_C1     &pid_appl_info          () const  { return this->variant.C_false.pid_appl_info_F; };
    const tsp00_C1     &pid_dtime_info         () const  { return this->variant.C_false.pid_dtime_info_F; };
#endif    /* defined (__cplusplus) */
} tak_parsid;

typedef enum tak_queryrewrite_exec__Enum
{
    qrwMergeF,
    qrwMergeE,
    qrwConvertExist,
    qrwConvertToExist,
    qrwDistinctPullUp,
    qrwDistinctPushDownTo,
    qrwDistinctPushDownFrom,
    qrwDistinctForSubqueries,
    qrwSimplifyPredicates,
    qrwEliminateGroupByOrDistinct,
    qrwEliminateOrderBy,
    qrwPushDownPredicates,
    qrwPushDownProjection,
    qrwPushDownQuantifier,
    qrwOptimizeSubquery
} tak_queryrewrite_exec__Enum;
typedef tak_queryrewrite_exec__Enum tak_queryrewrite_exec__Param;
typedef SET_TYPE_MSP00 (           tak_queryrewrite_exec, tak_queryrewrite_exec__Enum, qrwOptimizeSubquery, 2);

typedef enum tak_queryrewrite_mode_Enum
{
    No_Rewrite,
    Statement_Rewrite,
    Operator_Rewrite
} tak_queryrewrite_mode_Enum;
typedef tak_queryrewrite_mode_Enum tak_queryrewrite_mode_Param;
typedef ENUM_TYPE_MSP00 (          tak_queryrewrite_mode, tak_queryrewrite_mode_Enum, Operator_Rewrite, unsigned char);

typedef enum tak_special_node_info__Enum
{
    ni_complex_predicate,
    ni_complex_string_id,
    ni_info_node,
    ni_skip_node,
    ni_blank_skip,
    ni_correlated_column
} tak_special_node_info__Enum;
typedef tak_special_node_info__Enum tak_special_node_info__Param;
typedef SMALLSET_TYPE_MSP00 (      tak_special_node_info, tak_special_node_info__Enum, ni_correlated_column);

typedef enum tak_querycheck_node__Enum
{
    qcn_FixedPos,
    qcn_SubQInfo,
    qcn_RightsChecked,
    qcn_SpecialJoinFromSelect,
    qcn_ContainsSJFromSelect,
    qcn_Filler1,
    qcn_Filler2,
    qcn_Filler3
} tak_querycheck_node__Enum;
typedef tak_querycheck_node__Enum tak_querycheck_node__Param;
typedef SMALLSET_TYPE_MSP00 (      tak_querycheck_node, tak_querycheck_node__Enum, qcn_Filler3);

typedef struct                     tak_ap_node
{
    tak_procs           n_proc;
    tak_special_node_info n_special;
    tak_sc_symbol       n_symb;
    tsp00_DataType      n_datatype;
    tsp00_Int2          n_lo_level;
    tsp00_Int2          n_sa_level;
    tsp00_Int2          n_refback;
    tak_querycheck_node n_querycheck;
    tgg00_DateTimeFormat n_dt_format;
    union
    {
    /* 1 */
        struct tak_ap_node_1
        {
            tsp00_Int2          n_subproc_F;
            tsp00_Int2          n_length_F;
            tsp00_Int4          n_pos_F;
        } C_1;
    /* 2 */
        struct tak_ap_node_2
        {
            tsp00_Int2          n_fromtabnode_F;
            tsp00_Int2          n_columnindex_F;
            tsp00_Int4          n_filler_F;
        } C_2;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tsp00_Int2         &n_subproc              ()  { return this->variant.C_1.n_subproc_F; };
    tsp00_Int2         &n_length               ()  { return this->variant.C_1.n_length_F; };
    tsp00_Int4         &n_pos                  ()  { return this->variant.C_1.n_pos_F; };
    tsp00_Int2         &n_fromtabnode          ()  { return this->variant.C_2.n_fromtabnode_F; };
    tsp00_Int2         &n_columnindex          ()  { return this->variant.C_2.n_columnindex_F; };
    tsp00_Int4         &n_filler               ()  { return this->variant.C_2.n_filler_F; };
    /* const accessor functions */
    const tsp00_Int2   &n_subproc              () const  { return this->variant.C_1.n_subproc_F; };
    const tsp00_Int2   &n_length               () const  { return this->variant.C_1.n_length_F; };
    const tsp00_Int4   &n_pos                  () const  { return this->variant.C_1.n_pos_F; };
    const tsp00_Int2   &n_fromtabnode          () const  { return this->variant.C_2.n_fromtabnode_F; };
    const tsp00_Int2   &n_columnindex          () const  { return this->variant.C_2.n_columnindex_F; };
    const tsp00_Int4   &n_filler               () const  { return this->variant.C_2.n_filler_F; };
#endif    /* defined (__cplusplus) */
} tak_ap_node;
typedef ARRAY_TYPE_MSP00 (         tak_ap_max_tree, tak_ap_node, cak_max_node - 0 + 1);
typedef tak_ap_max_tree           *tak_ap_max_tree_ptr;

typedef enum tak_scanner_states__Enum
{
    scs_reserved_check,
    scs_pack,
    scs_hint_allowed
} tak_scanner_states__Enum;
typedef tak_scanner_states__Enum tak_scanner_states__Param;
typedef SMALLSET_TYPE_MSP00 (      tak_scanner_states, tak_scanner_states__Enum, scs_hint_allowed);

typedef struct                     tak_scanner_glob
{
    tak_sc_symbol       sc_symb;
    tak_sc_symbol       sc_eof_symb;
    tsp00_Int2          sc_sylength;
    tsp00_Int4          sc_sypos;
    tsp00_Int4          sc_newpos;
    tsp00_Int4          sc_lastpos;
    tsp00_Int4          sc_missing_kw_pos;
    tsp00_Int2          sc_double_quote;
    tak_scanner_states  sc_states;
    pasbool             sc_filler;
    tsp00_Int2          sc_eof_kw;
} tak_scanner_glob;
    /* PTS 1111575 E.Z. */

typedef enum tak_execution_kind_Enum
{
    only_parsing,
    only_executing,
    parsing_executing,
    pars_then_execute,
    only_syntax
} tak_execution_kind_Enum;
typedef tak_execution_kind_Enum tak_execution_kind_Param;
typedef ENUM_TYPE_MSP00 (          tak_execution_kind, tak_execution_kind_Enum, only_syntax, unsigned char);
typedef SET_TYPE_MSP00 (           tak_charset, unsigned char, 255, 32);
typedef ARRAY_TYPE_MSP00 (         tak_sqlm_charset, tak_charset, sqlm_internal - 0 + 1);
typedef SET_TYPE_MSP00 (           tak_columnset, int, MAX_COL_PER_TAB_GG00, 130);
typedef SET_TYPE_MSP00 (           tak_symbolset, tak_sc_symbol_Enum, s_current_schema, 32);
typedef OLDPASCALSTRING_TYPE_MSP00 (  tak_datapart, csp_maxint2);
typedef CSTRING_TYPE_MSP00      (  tak_datapartc, csp_maxint2);
typedef ARRAY_TYPE_MSP00 (         tak_eop_arr, tgg00_StackOpType, 4096);
typedef ARRAY_TYPE_MSP00 (         tak_colindex, tsp00_Int2, MAX_COL_PER_TAB_GG00);
typedef ARRAY_TYPE_MSP00 (         tak_colinteger, tsp00_Int2, MAX_COL_PER_TAB_GG00);
typedef ARRAY_TYPE_MSP00 (         tak_colsequence, tsp00_Int2, MAX_COL_SEQUENCE_GG00);
typedef ARRAY_TYPE_MSP00 (         tak_inv_colnos, tsp00_Int2, 32);
typedef ARRAY_TYPE_MSP00 (         tak_corlevel, tsp00_Uint1, cak_maxcorlevel);
typedef OLDPASCALSTRING_TYPE_MSP00 (  tak_keyword, 18);
typedef CSTRING_TYPE_MSP00      (  tak_keywordc, 18);
typedef tsp00_C18                  tak_oldidentifier;
typedef ARRAY_TYPE_MSP00 (         tak_keywordtab, tak_keyword, cak_maxkeyword);
typedef OLDPASCALSTRING_TYPE_MSP00 (  tak_order_errortext, mxak_order_errortext);
typedef CSTRING_TYPE_MSP00      (  tak_order_errortextc, mxak_order_errortext);
typedef OLDPASCALSTRING_TYPE_MSP00 (  tak_parsbuffer, mxak_parsbuf);
typedef CSTRING_TYPE_MSP00      (  tak_parsbufferc, mxak_parsbuf);
typedef ARRAY_TYPE_MSP00 (         tak_16stackentry, tgg00_StackEntry, MAX_COL_SEQUENCE_GG00);
typedef ARRAY_TYPE_MSP00 (         tak_upd_stat_info, tsp00_Int4, MAX_COL_PER_TAB_GG00);
typedef OLDPASCALSTRING_TYPE_MSP00 (  tak_oms_version_id, 22);
typedef CSTRING_TYPE_MSP00      (  tak_oms_version_idc, 22);
typedef OLDPASCALSTRING_TYPE_MSP00 (  tak_oms_version_desc, 512);
typedef CSTRING_TYPE_MSP00      (  tak_oms_version_descc, 512);
typedef OLDPASCALSTRING_TYPE_MSP00 (  tak_sysbufferoffset, cak_sysbufferoffset);
typedef CSTRING_TYPE_MSP00      (  tak_sysbufferoffsetc, cak_sysbufferoffset);
typedef tak_eop_arr               *tak_eop_arr_ptr;
    /* PTS 1109291 E.Z. */
typedef ARRAY_TYPE_MSP00 (         tak_pcountarr, tsp00_C3, cak_max_pcounts);
    /* PTS 1114411 T.A. */
typedef ARRAY_TYPE_MSP00 (         tak_WydeArray, tsp00_Int2, MOVEINT2_MXSP00);

typedef struct                     tak_del_tab_qual
{
    tsp00_Int2          del_qual_cnt;
    tsp00_Int2          del_colno;
    ARRAY_VAR_MSP00 (         del_qual, tsp00_C2, cak_max_del_qual);
} tak_del_tab_qual;

typedef struct                     tak_role_context
{
    tsp00_Int4          rctxt_cnt;
    ARRAY_VAR_MSP00 (         rctxt_roles, tgg00_Surrogate, cak_max_role_context);
} tak_role_context;
typedef tak_role_context          *tak_role_context_ptr;
typedef OLDPASCALSTRING_TYPE_MSP00 (  tak_map_set, mxak_mapset);
typedef CSTRING_TYPE_MSP00      (  tak_map_setc, mxak_mapset);

typedef struct                     tak_mapset_record
{
    tsp00_Int2          map_reclen;
    tsp00_Int2          map_keylen;
    tsp00_Int2          map_varcol_offset;
    tsp00_Int2          map_varcol_cnt;
    tgg00_Surrogate     map_surrogate;
    tsp00_C2            map_entrytype;
    tsp00_C2            map_linkage;
    tsp00_KnlIdentifier map_name;
    tsp00_C2            map_segmentid;
    tsp00_Int2          map_count;
    tsp00_CodeType      map_code;
    pasbool             map_filler1;
    tsp00_Int2          map_filler2;
    tak_map_set         map_set;
} tak_mapset_record;
    /* privilege catalog representation */

typedef enum tak_priv_descriptors_Enum
{
    priv_col_sel,
    priv_col_upd,
    priv_col_sel_grant,
    priv_col_upd_grant,
    priv_col_ref,
    priv_col_ref_grant
} tak_priv_descriptors_Enum;
typedef tak_priv_descriptors_Enum tak_priv_descriptors_Param;
typedef ENUM_TYPE_MSP00 (          tak_priv_descriptors, tak_priv_descriptors_Enum, priv_col_ref_grant, unsigned char);
typedef SMALLSET_TYPE_MSP00 (      tak_priv_desc, tak_priv_descriptors_Enum, priv_col_ref_grant);

typedef struct                     tak_privilege
{
    union
    {
    /* 1 */
        struct tak_privilege_1
        {
            tak00_PrivilegeSet  priv_all_set_F;
            tak00_PrivilegeSet  priv_all_grant_set_F;
            tak_priv_desc       priv_col_exist_F;
            pasbool             priv_filler_F;
            tak_columnset       priv_sel_set_F;
            tak_columnset       priv_upd_set_F;
            tak_columnset       priv_grant_sel_set_F;
            tak_columnset       priv_grant_upd_set_F;
            tak_columnset       priv_ref_set_F;
            tak_columnset       priv_grant_ref_set_F;
        } C_1;
    /* 2 */
        struct tak_privilege_2
        {
            tsp00_C4            priv_filler1_F;
            ARRAY_VAR_MSP00 (         priv_col_F, tak_columnset, priv_col_ref_grant - 0 + 1);
        } C_2;
    /* 3 */
        struct tak_privilege_3
        {
            tsp00_C132          priv_c132_F;
        } C_3;
    /* 4 */
        struct tak_privilege_4
        {
            OLDPASCALSTRING_VAR_MSP00 (  priv_c196_F, 196);
        } C_4;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tak00_PrivilegeSet &priv_all_set           ()  { return this->variant.C_1.priv_all_set_F; };
    tak00_PrivilegeSet &priv_all_grant_set     ()  { return this->variant.C_1.priv_all_grant_set_F; };
    tak_priv_desc      &priv_col_exist         ()  { return this->variant.C_1.priv_col_exist_F; };
    pasbool            &priv_filler            ()  { return this->variant.C_1.priv_filler_F; };
    tak_columnset      &priv_sel_set           ()  { return this->variant.C_1.priv_sel_set_F; };
    tak_columnset      &priv_upd_set           ()  { return this->variant.C_1.priv_upd_set_F; };
    tak_columnset      &priv_grant_sel_set     ()  { return this->variant.C_1.priv_grant_sel_set_F; };
    tak_columnset      &priv_grant_upd_set     ()  { return this->variant.C_1.priv_grant_upd_set_F; };
    tak_columnset      &priv_ref_set           ()  { return this->variant.C_1.priv_ref_set_F; };
    tak_columnset      &priv_grant_ref_set     ()  { return this->variant.C_1.priv_grant_ref_set_F; };
    tsp00_C4           &priv_filler1           ()  { return this->variant.C_2.priv_filler1_F; };
    ARRAY_TYPESTRING_MSP00(tak_columnset, priv_col_ref_grant - 0 + 1) &priv_col               ()  { return this->variant.C_2.priv_col_F; };
    tsp00_C132         &priv_c132              ()  { return this->variant.C_3.priv_c132_F; };
    tsp00_OldPascalString<196> &priv_c196              ()  { return this->variant.C_4.priv_c196_F; };
    /* const accessor functions */
    const tak00_PrivilegeSet &priv_all_set           () const  { return this->variant.C_1.priv_all_set_F; };
    const tak00_PrivilegeSet &priv_all_grant_set     () const  { return this->variant.C_1.priv_all_grant_set_F; };
    const tak_priv_desc &priv_col_exist         () const  { return this->variant.C_1.priv_col_exist_F; };
    const pasbool      &priv_filler            () const  { return this->variant.C_1.priv_filler_F; };
    const tak_columnset &priv_sel_set           () const  { return this->variant.C_1.priv_sel_set_F; };
    const tak_columnset &priv_upd_set           () const  { return this->variant.C_1.priv_upd_set_F; };
    const tak_columnset &priv_grant_sel_set     () const  { return this->variant.C_1.priv_grant_sel_set_F; };
    const tak_columnset &priv_grant_upd_set     () const  { return this->variant.C_1.priv_grant_upd_set_F; };
    const tak_columnset &priv_ref_set           () const  { return this->variant.C_1.priv_ref_set_F; };
    const tak_columnset &priv_grant_ref_set     () const  { return this->variant.C_1.priv_grant_ref_set_F; };
    const tsp00_C4     &priv_filler1           () const  { return this->variant.C_2.priv_filler1_F; };
    const ARRAY_TYPESTRING_MSP00(tak_columnset, priv_col_ref_grant - 0 + 1) &priv_col               () const  { return this->variant.C_2.priv_col_F; };
    const tsp00_C132   &priv_c132              () const  { return this->variant.C_3.priv_c132_F; };
    const tsp00_OldPascalString<196> &priv_c196              () const  { return this->variant.C_4.priv_c196_F; };
#endif    /* defined (__cplusplus) */
} tak_privilege;

typedef struct                     tak_privrecord
{
    tsp00_Int2          pr_rec_len;
    tsp00_Int2          pr_key_len;
    tsp00_Int2          pr_varcol_offset;
    tsp00_Int2          pr_varcol_cnt;
    tgg00_Surrogate     pr_tabid;
    tsp00_C2            pr_entrytype;
    tsp00_C2            pr_linkage;
    tgg00_Surrogate     pr_userid;
    tsp00_C2            pr_segmentid;
    tsp00_C2            pr_filler;
    tak_privilege       pr_priv;
} tak_privrecord;

typedef struct                     tak_privuserrecord
{
    tsp00_Int2          pru_rec_len;
    tsp00_Int2          pru_key_len;
    tsp00_Int2          pru_varcol_offset;
    tsp00_Int2          pru_varcol_cnt;
    tgg00_Surrogate     pru_tabid;
    tsp00_C2            pru_entrytype;
    tsp00_C2            pru_linkage;
    tgg00_Surrogate     pru_grantee;
    tgg00_Surrogate     pru_grantor;
    tsp00_C2            pru_segmentid;
    tsp00_C2            pru_filler;
    tsp00_Int4          pru_date;
    tsp00_Int4          pru_time;
    tak_privilege       pru_priv;
} tak_privuserrecord;

typedef struct                     tak_sprivilege
{
    tak00_PrivilegeSet  priv_all_set;
    tak_priv_desc       priv_col_exist;
    tsp00_Int2          priv_filler;
    tak_columnset       priv_sel_set;
    tak_columnset       priv_upd_set;
} tak_sprivilege;

typedef struct                     tak_proc_seq_privilege
{
    tsp00_Int2          psp_reclen;
    tsp00_Int2          psp_keylen;
    tsp00_Int2          psp_varcol_offset;
    tsp00_Int2          psp_varcol_cnt;
    tgg00_Surrogate     psp_id;
    tsp00_C2            psp_type;
    tsp00_C2            psp_linkage;
    tgg00_Surrogate     psp_grantor;
    tsp00_C2            psp_seqmentid;
    tak00_PrivilegeSet  psp_priv;
    pasbool             psp_grantOption;
    tsp00_Int4          psp_date;
    tsp00_Int4          psp_time;
} tak_proc_seq_privilege;

typedef struct                     tak_column_statistics
{
    tsp00_Int4          c_values;
    tsp00_Int4          c_pages;
    tsp00_Int4          c_avg_list_len;
} tak_column_statistics;

typedef struct                     tak_colsort
{
    tsp00_Int4          colbuf;
    tsp00_Int4          colind;
    tsp00_Int4          colpos;
} tak_colsort;
typedef ARRAY_TYPE_MSP00 (         tak_colsortarr, tak_colsort, MAX_COL_PER_TAB_GG00);
    /* domain catalog representation */

typedef struct                     tak_domainref
{
    tsp00_Int2          dreclen;
    tsp00_Int2          dkeylen;
    tsp00_Int2          dvarcol_offset;
    tsp00_Int2          dvarcol_cnt;
    tgg00_Surrogate     downer;
    tsp00_Int2          dentrytype;
    tsp00_Int2          dseqno;
    tsp00_KnlIdentifier dname;
    tsp00_C2            dsegmentid;
    tsp00_C2            dfiller;
    tgg00_Surrogate     dsurrogate;
} tak_domainref;

typedef struct                     tak_domainrecord
{
    tsp00_Int2          dom_reclen;
    tsp00_Int2          dom_keylen;
    tsp00_Int2          dom_varcol_offset;
    tsp00_Int2          dom_varcol_cnt;
    tgg00_Surrogate     dom_surrogate;
    tsp00_C2            dom_entrytype;
    tsp00_Int2          dom_seqno;
    tsp00_C2            dom_segmentid;
    tsp00_Uint1         dom_filler;
    pasbool             dom_constraint;
    tsp00_C16           dom_type_uid;
    tgg00_Surrogate     dom_owner;
    tsp00_Int4          dom_date;
    tsp00_Int4          dom_time;
    tak00_columninfo    dom_colinfo;
} tak_domainrecord;

typedef struct                     tak_col_uses_dom
{
    tsp00_Int2          cud_reclen;
    tsp00_Int2          cud_keylen;
    tsp00_Int2          cud_varcol_offset;
    tsp00_Int2          cud_varcol_cnt;
    tgg00_Surrogate     cud_surrogate;
    tsp00_C2            cud_entrytype;
    tsp00_C2            cud_linkage;
    tsp00_C2            cud_segmentid;
    tsp00_C2            cud_filler1;
    tgg00_Surrogate     cud_owner;
    tsp00_KnlIdentifier cud_name;
} tak_col_uses_dom;

typedef struct                     tak_sequencerecord
{
    tsp00_Int2          seq_reclen;
    tsp00_Int2          seq_keylen;
    tsp00_Int2          seq_varcol_offset;
    tsp00_Int2          seq_varcol_cnt;
    tgg00_Surrogate     seq_surrogate;
    tsp00_C2            seq_entrytype;
    tsp00_C2            seq_linkage;
    tsp00_C2            seq_segmentid;
    pasbool             seq_comment;
    pasbool             seq_public;
    tsp00_Int4          seq_date;
    tsp00_Int4          seq_time;
    tsp00_Number        seq_value;
    tgg00_Surrogate     seq_owner;
    tgg00_Surrogate     seq_schema;
    tgg00_SeqInfo       seq_info;
    tsp00_KnlIdentifier seq_name;
} tak_sequencerecord;

typedef struct                     tak_tablereference
{
    tak_sysbufferoffset rtablerefkey1;
    tsp00_KnlIdentifier rtablerefkey2;
    tsp00_C2            rsegmentid;
    pasbool             rsystable;
    pasbool             rfiller;
    tgg00_TableKind     rtablekind;
    pasbool             rundefbyte;
    tgg00_Surrogate     rtableid;
    tgg00_Surrogate     rsynid;
} tak_tablereference;

#if defined (bit64)

    /* table structure and column catalog representation */
typedef ARRAY_TYPE_MSP00 (         tak_columnarr, tak00_colinfo_ptr, 1280);

#else

typedef ARRAY_TYPE_MSP00 (         tak_columnarr, tak00_colinfo_ptr, 1536);

#endif

typedef OLDPASCALSTRING_TYPE_MSP00 (  tak_basefiller, 16);
typedef CSTRING_TYPE_MSP00      (  tak_basefillerc, 16);

#if defined (bit64)

    /* default / range catalog representation */
typedef tsp00_C8                   tak_align;

#else

typedef tsp00_C4                   tak_align;

#endif

typedef struct tak_basecolinfo tak_basecolinfo;
typedef tak_basecolinfo           *tak_basecolinfo_ptr;
    /* 4096 has NOTHING to do with tsp00_Buf or so */

 struct                     tak_basecolinfo
{
    tsp00_Int4          bci_index;
    tsp00_Int2          bci_rec_cnt;
    tsp00_Int2          bci_ref_cnt;
    tsp00_Int2          bci_buf_size;
    tsp00_Int2          bci_fill1;
    tsp00_Int4          bci_fill2;
    tak_basecolinfo_ptr bci_next;
    union
    {
    /* true */
        struct tak_basecolinfo_true
        {
            ARRAY_VAR_MSP00 (         bci_align_F, tak_align, cak_max_align);
        } C_true;
    /* false */
        struct tak_basecolinfo_false
        {
            OLDPASCALSTRING_VAR_MSP00 (  bci_buf_F, 4096);
        } C_false;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    ARRAY_TYPESTRING_MSP00(tak_align, cak_max_align) &bci_align              ()  { return this->variant.C_true.bci_align_F; };
    tsp00_OldPascalString<4096> &bci_buf                ()  { return this->variant.C_false.bci_buf_F; };
    /* const accessor functions */
    const ARRAY_TYPESTRING_MSP00(tak_align, cak_max_align) &bci_align              () const  { return this->variant.C_true.bci_align_F; };
    const tsp00_OldPascalString<4096> &bci_buf                () const  { return this->variant.C_false.bci_buf_F; };
#endif    /* defined (__cplusplus) */
} ;

typedef enum tak_view_attributes__Enum
{
    va_correlation,
    va_date_time_used,
    va_unused1,
    va_unused2,
    va_unused3,
    va_unused4,
    va_unused5,
    va_unused6
} tak_view_attributes__Enum;
typedef tak_view_attributes__Enum tak_view_attributes__Param;
typedef SMALLSET_TYPE_MSP00 (      tak_view_attributes, tak_view_attributes__Enum, va_unused6);

typedef struct                     tak_baserecord
{
    tsp00_Int2          breclen;
    tsp00_Int2          bkeylen;
    tsp00_Int2          bvarcol_offset;
    tsp00_Int2          bvarcol_cnt;
    tgg00_Surrogate     bsurrogate;
    tsp00_C2            bentrytype;
    tsp00_C2            bseqno;
    tsp00_C2            bsegmentid;
    tsp00_Int2          bfirstindex;
    tsp00_Int2          blastindex;
    tgg00_TableKind     btablekind;
    tgg00_Linkset       blinkexist;
    tgg00_FileId        btreeid;
    tgg00_Surrogate     bauthid;
    tgg00_Surrogate     bschema;
    tgg00_Surrogate     bschemacontext;
    tsp00_Int2          breccnt;
    tsp00_Int2          bmaxcol;
    tsp00_Int2          bmaxreclen;
    tsp00_Int2          bkeycolcount;
    tsp00_Int2          blenfixedcol;
    tsp00_Int2          bvarcolcount;
    tsp00_Int2          bcntdropcol;
    tsp00_Int2          bstringcount;
    tsp00_Uint1         blongvarcolcnt;
    pasbool             bcomment;
    tsp00_Int2          bavgrowlen;
    tsp00_Int2          blastkeyind;
    tsp00_Int2          bfirstcolind;
    tsp00_Int4          bdatecreate;
    tsp00_Int4          btimecreate;
    tsp00_Int4          bupdstatdate;
    tsp00_Int4          bupdstattime;
    tsp00_Int4          bpages;
    tsp00_Int4          brows;
    tsp00_Int4          balterdate;
    tsp00_Int4          baltertime;
    tsp00_Int4          bsample;
    tak00_PrivilegeSet  bpriv_all_set;
    tak_priv_desc       bpriv_col_exist;
    pasbool             bunloaded;
    pasbool             bindexexist;
    tsp00_Uint1         bv_level;
    tsp00_Uint1         bv_tabcount;
    tgg00_TableKind     bv_tablekind;
    pasbool             bv_checkopt;
    pasbool             bv_qualexist;
    tgg04_Distinct      bv_distinct;
    pasbool             bv_viewlist;
    pasbool             bdef_stamp;
    tak_view_attributes bview_attributes;
    tsp00_SqlMode       bsqlmode;
    tsp00_Int2          bnamed_constr;
    tgg00_ShowKind      bshow_kind;
    pasbool             brecreate_view;
    tgg00_ShowKind      bshowkind;
    tak_table_attribute_set battributes;
    tsp00_KnlIdentifierPtr btablen;
    tak_basecolinfo_ptr bptr;
    tak_columnarr      *bcolumnExtension;
    tsp00_Int2          bcolumnSplitIdx;
    pasbool             bupdstatinserted;
    union
    {
    /* 1 */
        struct tak_baserecord_1
        {
            tak_colindex        bextcolindex_F;
        } C_1;
    /* 2 */
        struct tak_baserecord_2
        {
            tak_columnarr       bcolumn_F;
        } C_2;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tak_colindex       &bextcolindex           ()  { return this->variant.C_1.bextcolindex_F; };
    tak_columnarr      &bcolumn                ()  { return this->variant.C_2.bcolumn_F; };
    /* const accessor functions */
    const tak_colindex &bextcolindex           () const  { return this->variant.C_1.bextcolindex_F; };
    const tak_columnarr &bcolumn                () const  { return this->variant.C_2.bcolumn_F; };
#endif    /* defined (__cplusplus) */
} tak_baserecord;
    /* default / range catalog representation */
typedef OLDPASCALSTRING_TYPE_MSP00 (  tak_default_value, cak_maxdefaultvalue);
typedef CSTRING_TYPE_MSP00      (  tak_default_valuec, cak_maxdefaultvalue);

typedef struct                     tak_defaultrecord
{
    tsp00_Int2          dfreclen;
    tsp00_Int2          dfkeylen;
    tsp00_Int2          dfvarcol_offset;
    tsp00_Int2          dfvarcol_cnt;
    tgg00_Surrogate     dfsurrogate;
    tsp00_C2            dfentrytype;
    tsp00_C2            dfseqno;
    tsp00_C2            dfsegment_id;
    tsp00_Int2          dfdefault_function;
    tak_default_value   dfvalues;
} tak_defaultrecord;

typedef enum tak_comment_type_Enum
{
    cm_nil,
    cm_column,
    cm_dbproc,
    cm_domain,
    cm_foreign_key,
    cm_filler,
    cm_index,
    cm_priv_column,
    cm_priv_dbproc,
    cm_priv_table,
    cm_synonym,
    cm_table,
    cm_trigger,
    cm_user,
    cm_sequence
} tak_comment_type_Enum;
typedef tak_comment_type_Enum tak_comment_type_Param;
typedef ENUM_TYPE_MSP00 (          tak_comment_type, tak_comment_type_Enum, cm_sequence, unsigned char);
typedef ARRAY_TYPE_MSP00 (         tak_ak_constr_stack, tgg00_StackEntry, cak_maxconstraintstack);

typedef struct                     tak_alterdaterecord
{
    tsp00_Int2          ad_rec_len;
    tsp00_Int2          ad_key_len;
    tsp00_Int2          ad_varcol_offset;
    tsp00_Int2          ad_varcol_cnt;
    tgg00_Surrogate     ad_surrogate;
    tsp00_C2            ad_entrytype;
    tsp00_C2            ad_linkage;
    tsp00_C2            ad_segmentid;
    tsp00_C2            ad_filler;
    tsp00_Int4          ad_createdate;
    tsp00_Int4          ad_createtime;
    tsp00_Int4          ad_alterdate;
    tsp00_Int4          ad_altertime;
} tak_alterdaterecord;
typedef OLDPASCALSTRING_TYPE_MSP00 (  tak_comment_text, mxak_comment);
typedef CSTRING_TYPE_MSP00      (  tak_comment_textc, mxak_comment);

typedef struct                     tak_comment_record
{
    tsp00_Int2          com_rec_len;
    tsp00_Int2          com_key_len;
    tsp00_Int2          com_varcol_offset;
    tsp00_Int2          com_varcol_cnt;
    tgg00_Surrogate     com_surrogate;
    tsp00_C2            com_entrytype;
    tsp00_C2            com_linkage;
    tak_comment_type    com_type;
    tsp00_C1            com_colno;
    tsp00_C2            com_segmentid;
    tsp00_Int4          com_length;
    tak_comment_text    com_text;
} tak_comment_record;

typedef struct                     tak_constraintrecord
{
    tsp00_Int2          crec_len;
    tsp00_Int2          ckey_len;
    tsp00_Int2          cvarcol_offset;
    tsp00_Int2          cvarcol_cnt;
    tgg00_Surrogate     csurrogate;
    tsp00_C2            centrytype;
    tsp00_C2            cseqno;
    tsp00_C2            csegmentid;
    tsp00_Int2          cstack_cnt;
    tsp00_Int2          cvalue_pos;
    pasbool             cfunction;
    pasbool             coldrange;
    tak_columnset       ccolset;
    tak_oldidentifier   ccname;
    tsp00_Uint1         cstackstart;
    tsp00_Uint1         csuffixlen;
    tsp00_Int2          cfiller;
    union
    {
    /* true */
        struct tak_constraintrecord_true
        {
            tak_ak_constr_stack cstack_F;
        } C_true;
    /* false */
        struct tak_constraintrecord_false
        {
            tsp00_KnlIdentifier csuffix_F;
        } C_false;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tak_ak_constr_stack &cstack                 ()  { return this->variant.C_true.cstack_F; };
    tsp00_KnlIdentifier &csuffix                ()  { return this->variant.C_false.csuffix_F; };
    /* const accessor functions */
    const tak_ak_constr_stack &cstack                 () const  { return this->variant.C_true.cstack_F; };
    const tsp00_KnlIdentifier &csuffix                () const  { return this->variant.C_false.csuffix_F; };
#endif    /* defined (__cplusplus) */
} tak_constraintrecord;

typedef struct                     tak_one_f_proc
{
    tgg00_Surrogate     ofp_procid;
    tsp00_C3            ofp_language;
    SMALLSET_VAR_MSP00 (      ofp_sqlmode, tsp00_SqlMode_Enum, sqlm_internal);
} tak_one_f_proc;
typedef OLDPASCALSTRING_TYPE_MSP00 (  tak_funcref_restkey, 56);
typedef CSTRING_TYPE_MSP00      (  tak_funcref_restkeyc, 56);

typedef struct                     tak_funcrefrecord
{
    tsp00_Int2          fct_reclen;
    tsp00_Int2          fct_keylen;
    tsp00_Int2          fct_varcol_offset;
    tsp00_Int2          fct_varcol_cnt;
    tsp00_C8            fct_key1;
    tsp00_C2            fct_entrytype;
    tsp00_C2            fct_linkage;
    tak_funcref_restkey fct_key2;
    tsp00_C2            fct_segmentid;
    tsp00_Uint1         fct_proc_cnt;
    pasbool             fct_comment;
    tsp00_Int4          fct_fill2;
    ARRAY_VAR_MSP00 (         fct_procids, tak_one_f_proc, cak_max_proc);
} tak_funcrefrecord;

typedef struct                     tak_constraintnamerecord
{
    tsp00_Int2          cnreclen;
    tsp00_Int2          cnkeylen;
    tsp00_Int2          cnvarcol_offset;
    tsp00_Int2          cnvarcol_cnt;
    tsp00_C12           cnkey1;
    tsp00_KnlIdentifier cnkey2;
    tsp00_C2            cnsegmentid;
    tsp00_Int2          cnid;
    tsp00_Int2          cnfiller;
} tak_constraintnamerecord;
    /* named index catalog representation */

typedef struct                     tak_multindex
{
    tak_oldidentifier   indexn_prefix;
    tsp00_Int2          indexno;
    tsp00_Uint1         icount;
    pasbool             ifill;
    tak_colsequence     icolseq;
    pasbool             iunique;
    pasbool             icomment;
    tsp00_Int4          idatecre;
    tsp00_Int4          itimecre;
    tsp00_Int4          iinitdate;
    tsp00_Int4          ifiller2;
    tsp00_Int4          iinittime;
    pasbool             ifiller;
    pasbool             idisabled;
    pasbool             ifiller1;
    tsp00_Uint1         isuffixlen;
    tak_16stackentry    icolstack;
} tak_multindex;

typedef struct                     tak_mindexrecord
{
    tsp00_Int2          ireclen;
    tsp00_Int2          ikeylen;
    tsp00_Int2          ivarcol_offset;
    tsp00_Int2          ivarcol_cnt;
    tgg00_Surrogate     isurrogate;
    tsp00_C2            ientrytype;
    tsp00_C2            iseqno;
    tsp00_C2            isegmentid;
    tsp00_Uint1         indexcount;
    pasbool             indexnext;
    ARRAY_VAR_MSP00 (         indexdef, tak_multindex, cak_max_index_per_rec);
} tak_mindexrecord;
    /* link catalog representation */

typedef struct                     tak_linkdef
{
    tgg00_Surrogate     ltableid;
    tak_oldidentifier   linkn_prefix;
    tsp00_Int2          lfiller1;
    tsp00_Int4          ldatecre;
    tsp00_Int4          ltimecre;
    tsp00_DataType      ldatatyp;
    tsp00_Uint1         laction;
    tsp00_Uint1         lcolcount;
    tsp00_C1            lindexid;
    pasbool             lcomment;
    tsp00_Uint1         lsuffixlen;
    tsp00_Int2          lfiller2;
    tak_colsequence     lprimcolseq;
    tak_colsequence     lseccolseq;
    tak_colsequence     lrecpos;
    tak_16stackentry    lstack;
} tak_linkdef;

typedef struct                     tak_linkrecord
{
    tsp00_Int2          lreclen;
    tsp00_Int2          lkeylen;
    tsp00_Int2          lvarcol_offset;
    tsp00_Int2          lvarcol_cnt;
    tgg00_Surrogate     lsurrogate;
    tsp00_C2            lentrytype;
    tsp00_C2            lseqno;
    tsp00_C2            lsegmentid;
    tsp00_Int2          linkcount;
    ARRAY_VAR_MSP00 (         linkdef, tak_linkdef, cak_maxlinkdef);
} tak_linkrecord;

typedef struct                     tak_link_def_record
{
    tsp00_Int2          lreclen;
    tsp00_Int2          lkeylen;
    tsp00_Int2          lvarcol_offset;
    tsp00_Int2          lvarcol_cnt;
    tgg00_Surrogate     lsurrogate;
    tsp00_C2            lentrytype;
    tsp00_C2            lsegno;
    tsp00_KnlIdentifier lkeyname;
    tsp00_C2            lsegmentid;
    tsp00_Int2          ldefault_len;
    tsp00_Int2          sfiller;
    tsp00_Key           ldefault_key;
} tak_link_def_record;

typedef struct                     tak_linkparsinfo
{
    tgg00_FileId        lptreeid;
    tgg00_FileId        lpauxfile;
    tsp00_Int2          lpfiller;
    tsp00_Uint1         lpcolcount;
    pasbool             lp_one_to_one;
    tak_16stackentry    lpstack;
} tak_linkparsinfo;

typedef struct                     tak_linkparsrecord
{
    tsp00_Int2          lpreclen;
    tsp00_Int2          lpkeylen;
    tsp00_Int2          lpvarcol_offset;
    tsp00_Int2          lpvarcol_cnt;
    tgg00_Surrogate     lpsurrogate;
    tsp00_C2            lpentrytype;
    tsp00_C2            lpseqno;
    tsp00_C2            lpsegmentid;
    tsp00_Int2          lpcount;
    ARRAY_VAR_MSP00 (         lpinfo, tak_linkparsinfo, cak_maxlinkdef);
} tak_linkparsrecord;

typedef enum tak_decresstate_Enum
{
    dr_ansi,
    dr_db2,
    dr_db2_with_hold,
    dr_oracle,
    dr_sql_db
} tak_decresstate_Enum;
typedef tak_decresstate_Enum tak_decresstate_Param;
typedef ENUM_TYPE_MSP00 (          tak_decresstate, tak_decresstate_Enum, dr_sql_db, unsigned char);

typedef enum tak_resstate_Enum
{
    rs_result,
    rs_noresult,
    rs_intinvnoresult,
    rs_nobase,
    rs_useinv,
    rs_joinupdate,
    rs_show,
    rs_reuseable
} tak_resstate_Enum;
typedef tak_resstate_Enum tak_resstate_Param;
typedef ENUM_TYPE_MSP00 (          tak_resstate, tak_resstate_Enum, rs_reuseable, unsigned char);
typedef SMALLSET_TYPE_MSP00 (      tak_sresstate, tak_resstate_Enum, rs_reuseable);

typedef enum tak_select_fetch_type_Enum
{
    sft_normal_select,
    sft_select_all_results,
    sft_select_some_results
} tak_select_fetch_type_Enum;
typedef tak_select_fetch_type_Enum tak_select_fetch_type_Param;
typedef ENUM_TYPE_MSP00 (          tak_select_fetch_type, tak_select_fetch_type_Enum, sft_select_some_results, unsigned char);

typedef struct                     tak_resname_record
{
    tsp00_Int2          resreclen;
    tsp00_Int2          reskeylen;
    tsp00_Int2          resvarcol_offset;
    tsp00_Int2          resvarcol_cnt;
    tgg00_Surrogate     reskey_idfill;
    tsp00_C2            reskey_identr;
    tsp00_KnlIdentifier reskey_name;
    tsp00_KnlIdentifier reskey_modul_name;
    tsp00_Uint1         resinfobuf;
    tsp00_Uint1         resparsinfobyte;
    tgg00_FileId        restreeid;
    tsp00_Int4          resLongFileId;
    tgg00_Surrogate     resunlocktabid;
    tgg91_TransNo       restrans;
    pasbool             resscrollable;
    pasbool             ressubfill;
    tgg00_SubtransNo    ressubtrans;
    tak_parskey         reseparsk;
    tak_sresstate       resstate;
    tsp00_Uint1         resmaxlinkage;
    tak_decresstate     resdecresdel;
    tgg00_DateTimeFormat resdatetimeformat;
    pasbool             resimpl_upd_stat;
    pasbool             resexecuted;
    tak_select_fetch_type resselect_fetch;
    tgg00_Surrogate     resanalyze_cmdid;
    tgg91_TransNo       resconsistentview;
    tsp00_Int2          resfiller;
} tak_resname_record;
typedef ARRAY_TYPE_MSP00 (         tak_resn_rec_arr, tak_resname_record, cak_max_resn);
    /* trigger representation */

typedef enum tak_proc_language_Enum
{
    lang_pl,
    lang_cpp,
    lang_java,
    lang_system,
    lang_future_use_01,
    lang_future_use_02,
    lang_future_use_03,
    lang_future_use_04,
    lang_future_use_05,
    lang_future_use_06,
    lang_future_use_07,
    lang_future_use_08,
    lang_future_use_09,
    lang_future_use_10,
    lang_future_use_11,
    lang_future_use_12,
    lang_future_use_13,
    lang_future_use_14,
    lang_future_use_15,
    lang_future_use_16,
    lang_future_use_17,
    lang_future_use_18,
    lang_future_use_19,
    lang_future_use_20,
    lang_other
} tak_proc_language_Enum;
typedef tak_proc_language_Enum tak_proc_language_Param;
typedef ENUM_TYPE_MSP00 (          tak_proc_language, tak_proc_language_Enum, lang_other, unsigned char);
typedef ARRAY_TYPE_MSP00 (         tak_trigger_stack, tgg00_StackEntry, cak_max_trigger_stack);

typedef struct                     tak_triggerrecord
{
    tsp00_Int2          tr_reclen;
    tsp00_Int2          tr_keylen;
    tsp00_Int2          tr_varcol_offset;
    tsp00_Int2          tr_varcol_cnt;
    tgg00_Surrogate     tr_surrogate;
    tsp00_C2            tr_entrytype;
    tsp00_C2            tr_linkage;
    tsp00_C2            tr_segmentid;
    tsp00_Int2          tr_param_cnt;
    tsp00_Int2          tr_condition_cnt;
    pasbool             tr_succ_exists;
    pasbool             tr_comment;
    tgg00_Surrogate     tr_proc_id;
    tsp00_Int4          tr_date;
    tsp00_Int4          tr_time;
    tak_columnset       tr_upd_set;
    tsp00_Int2          tr_maxUpdatedColno;
    tak_proc_language   tr_language;
    tsp00_Uint1         tr_name_len;
    union
    {
    /* true */
        struct tak_triggerrecord_true
        {
            tsp00_KnlIdentifier tr_name_F;
        } C_true;
    /* false */
        struct tak_triggerrecord_false
        {
            tak_trigger_stack   tr_stack_F;
        } C_false;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tsp00_KnlIdentifier &tr_name                ()  { return this->variant.C_true.tr_name_F; };
    tak_trigger_stack  &tr_stack               ()  { return this->variant.C_false.tr_stack_F; };
    /* const accessor functions */
    const tsp00_KnlIdentifier &tr_name                () const  { return this->variant.C_true.tr_name_F; };
    const tak_trigger_stack &tr_stack               () const  { return this->variant.C_false.tr_stack_F; };
#endif    /* defined (__cplusplus) */
} tak_triggerrecord;
    /* usage catalog representation */

typedef struct                     tak_usagedef
{
    tgg00_Surrogate     usa_tableid;
    tsp00_Int2          usa_filler;
    tgg00_TableKind     usa_tablekind;
    pasbool             usa_empty;
} tak_usagedef;

typedef struct                     tak_usagerecord
{
    tsp00_Int2          usagereclen;
    tsp00_Int2          usagekeylen;
    tsp00_Int2          usagevarcol_offset;
    tsp00_Int2          usagevarcol_cnt;
    tgg00_Surrogate     usagesurrogate;
    tsp00_C2            usageentrytype;
    tsp00_C2            usageseqno;
    tsp00_C2            usagesegmentid;
    tsp00_Int2          usagefiller;
    tsp00_Int2          usagecount;
    pasbool             usagenext_exist;
    pasbool             usagefull;
    ARRAY_VAR_MSP00 (         usagedef, tak_usagedef, cak_max_usage);
} tak_usagerecord;
    /* usertab catalog representation */

typedef enum tak_usertab_descriptor_Enum
{
    ut_table,
    ut_base_table,
    ut_procedure,
    ut_sequence,
    ut_internal_systable,
    ut_oracle_systable,
    ut_view,
    ut_filler,
    ut_role
} tak_usertab_descriptor_Enum;
typedef tak_usertab_descriptor_Enum tak_usertab_descriptor_Param;
typedef ENUM_TYPE_MSP00 (          tak_usertab_descriptor, tak_usertab_descriptor_Enum, ut_role, unsigned char);

typedef struct                     tak_usertabdef
{
    tgg00_Surrogate     ut_surrogate;
    pasbool             ut_empty;
    tak_usertab_descriptor ut_kind;
    tsp00_Int2          ut_filler;
} tak_usertabdef;

typedef struct                     tak_usertablerecord
{
    tsp00_Int2          usatabreclen;
    tsp00_Int2          usatabkeylen;
    tsp00_Int2          usatabvarcol_offset;
    tsp00_Int2          usatabvarcol_cnt;
    tgg00_Surrogate     usatabsurrogate;
    tsp00_C2            usatabentrytype;
    tsp00_C2            usatabseqno;
    tsp00_C2            usertabsegmentid;
    tsp00_Int2          usertabfiller;
    pasbool             usertabnext_exist;
    pasbool             usertabfull;
    tsp00_Int2          usertabcount;
    ARRAY_VAR_MSP00 (         usertabdef, tak_usertabdef, cak_max_usertab);
} tak_usertablerecord;
    /* user catalog representation */

typedef enum tak_userparams_Enum
{
    schemaid,
    up_unused2,
    maxtimeout,
    costwarning,
    costlimit,
    cachelimit
} tak_userparams_Enum;
typedef tak_userparams_Enum tak_userparams_Param;
typedef ENUM_TYPE_MSP00 (          tak_userparams, tak_userparams_Enum, cachelimit, unsigned char);
typedef ARRAY_TYPE_MSP00 (         tak_userpar_arr, tsp00_Int4, cachelimit - 0 + 1);

typedef enum tak_userrectype__Enum
{
    is_user_rec,
    is_group_rec,
    is_role_rec,
    is_schema_rec
} tak_userrectype__Enum;
typedef tak_userrectype__Enum tak_userrectype__Param;
typedef SMALLSET_TYPE_MSP00 (      tak_userrectype, tak_userrectype__Enum, is_schema_rec);

typedef enum tak_role_info_Enum
{
    role_all,
    role_none,
    role_except
} tak_role_info_Enum;
typedef tak_role_info_Enum tak_role_info_Param;
typedef ENUM_TYPE_MSP00 (          tak_role_info, tak_role_info_Enum, role_except, unsigned char);

typedef struct                     tak_userrecord
{
    tsp00_Int2          ureclen;
    tsp00_Int2          ukeylen;
    tsp00_Int2          uvarcol_offset;
    tsp00_Int2          uvarcol_cnt;
    tgg00_Surrogate     usurrogate;
    tsp00_C2            uentrytype;
    tsp00_C2            useqno;
    tsp00_C2            usersegmentid;
    tsp00_CodeType      userchardefcode;
    tak_role_info       userroleinfo;
    tak_userrectype     urecordtyp;
    tak_usertyp         userkind;
    pasbool             usersysdba;
    pasbool             uexclusive;
    pasbool             ureplication;
    pasbool             userconnect;
    tsp00_Int2          userfiller3;
    tsp00_CryptPw       userpassw;
    tsp00_CryptPw       usupportpasspw;
    tgg00_Surrogate     usergroup_id;
    tgg00_Surrogate     userowner_id;
    tsp00_Int4          upwcreatedate;
    tsp00_Int4          upwcreatetime;
    tsp00_Int4          ucreatedate;
    tsp00_Int4          ucreatetime;
    tak_userpar_arr     uparams;
    tgg00_UserRef       uperm_ref;
    pasbool             ucomment;
    tsp00_Uint1         userrolecnt;
    tsp00_Int4          ualterdate;
    tsp00_Int4          ualtertime;
    tsp00_KnlIdentifier username;
} tak_userrecord;

typedef struct                     tak_useridrecord
{
    tsp00_Int2          us_reclen;
    tsp00_Int2          us_keylen;
    tsp00_Int2          us_varcol_offset;
    tsp00_Int2          us_varcol_cnt;
    tgg00_Surrogate     us_surrogate;
    tsp00_C2            us_entrytype;
    tsp00_C2            us_linkage;
    tsp00_C2            us_segmentid;
    tsp00_C2            us_filler;
    tsp00_C4            us_userid;
} tak_useridrecord;
    /* user reference catalog representation username  ==> user_id */
typedef OLDPASCALSTRING_TYPE_MSP00 (  tak_uref_restkey, 56);
typedef CSTRING_TYPE_MSP00      (  tak_uref_restkeyc, 56);

typedef struct                     tak_userrefrecord
{
    tak_sysbufferoffset ru_key_offset;
    tak_uref_restkey    ru_rest_key;
    tsp00_C2            ru_segmentid;
    tsp00_Int2          ru_filler;
    tgg00_Surrogate     ru_user_id;
} tak_userrefrecord;
    /* system user catalog representation */

typedef struct                     tak_sysdbarecord
{
    tsp00_Int2          sysreclen;
    tsp00_Int2          syskeylen;
    tsp00_Int2          sysvarcol_offset;
    tsp00_Int2          sysvarcol_cnt;
    tgg00_Surrogate     syssurrogate;
    tsp00_C2            sysentrytyp;
    tsp00_C2            syssegno;
    tsp00_C2            syssegmentid;
    pasbool             sysactivated;
    tsp00_Uint1         sysversion;
    tgg00_Surrogate     sysauthid;
    tsp00_Version       sysKnlVersion;
} tak_sysdbarecord;
    /* location catalog representation */

typedef struct                     tak_serverdbrecord
{
    tsp00_Int2          sitereclen;
    tsp00_Int2          sitekeylen;
    tsp00_Int2          sitevarcol_offset;
    tsp00_Int2          sitevarcol_cnt;
    tsp00_C12           sitekey;
    tsp00_C2            sitesegmentid;
    tgg00_ServerdbNo    sitemap_no;
    tsp00_DbName        siteserverdbname;
    tgg00_ServerdbType  sitetype;
    pasbool             sitefiller1;
    tsp00_NodeId        siteid;
    tsp00_Int4          siterefcount;
} tak_serverdbrecord;

typedef struct                     tak_serverdbnamerecord
{
    tsp00_Int2          snreclen;
    tsp00_Int2          snkeylen;
    tsp00_Int2          snvarcol_offset;
    tsp00_Int2          snvarcol_cnt;
    tsp00_C12           snkey1;
    tsp00_DbName        snkey2;
    tsp00_C2            snsegmentid;
    tsp00_NodeId        snnodeid;
    tgg00_ServerdbNo    snsite;
    tgg00_ServerdbType  snsitetype;
    tsp00_CodeType      sncode;
    tak_oldidentifier   sntermchar;
    tgg00_ServerdbNo    snremote_site;
    tgg00_ServerdbNo    snlocal_site;
    tsp00_Int2          snfiller2;
} tak_serverdbnamerecord;
    /* stored procedure catalog representation */
typedef tsp1_param_spec_list      *tak_param_spec_ptr;

typedef struct                     tak_param_list
{
    tak_param_spec_ptr  param_spec_ptr;
    tsp00_Int2          param_counter;
    tsp00_Int2          param_filler1;
    tsp00_Int4          param_filler2;
} tak_param_list;

typedef enum tak_param_state_Enum
{
    param_undef,
    param_new,
    param_old,
    param_const
} tak_param_state_Enum;
typedef tak_param_state_Enum tak_param_state_Param;
typedef ENUM_TYPE_MSP00 (          tak_param_state, tak_param_state_Enum, param_const, unsigned char);

typedef struct                     tak_param_info
{
    tsp00_DataType      param_datatype;
    tsp00_Uint1         param_datafrac;
    tsp00_Int2          param_datalength;
    tsp00_Int2          param_inoutlength;
    tsp00_C2            param_in_out;
    tsp00_Int2          param_com_datatype;
    tsp00_C2            param_type_id;
    tsp00_Int2          param_cpp_offset;
    tsp00_Int2          param_ascii_offset;
    tsp00_Int2          param_unicode_offset;
    tak_param_state     param_state;
    char                param_name_len;
    tsp00_C256          param_name;
} tak_param_info;
typedef tak_param_info            *tak_param_info_ptr;
typedef ARRAY_TYPE_MSP00 (         tak_param_array, tak_param_info_ptr, cak_max_param_index);

typedef enum tak_proc_kind_Enum
{
    p_dbproc,
    p_trigger,
    p_function
} tak_proc_kind_Enum;
typedef tak_proc_kind_Enum tak_proc_kind_Param;
typedef ENUM_TYPE_MSP00 (          tak_proc_kind, tak_proc_kind_Enum, p_function, unsigned char);

typedef enum tak_debug_mode_Enum
{
    dbg_inproc,
    dbg_local
} tak_debug_mode_Enum;
typedef tak_debug_mode_Enum tak_debug_mode_Param;
typedef ENUM_TYPE_MSP00 (          tak_debug_mode, tak_debug_mode_Enum, dbg_local, unsigned char);

typedef struct                     tak_methodrecord
{
    tsp00_Int2          me_reclen;
    tsp00_Int2          me_keylen;
    tsp00_Int2          me_varcol_offset;
    tsp00_Int2          me_varcol_cnt;
    tgg00_Surrogate     me_surrogate;
    tsp00_C2            me_entrytype;
    tsp00_C2            me_linkage;
    tsp00_C2            me_segmentid;
    tsp00_Int2          me_param_cnt;
    union
    {
    /* true */
        struct tak_methodrecord_true
        {
            tsp00_DbObjectType  me_type_F;
            tak_debug_mode      me_debug_F;
            pasbool             me_sql_F;
            pasbool             me_cursor_F;
            tsp00_Int4          me_dispid_F;
            tsp00_C16           me_coclsid_F;
            tsp00_C16           me_iid_F;
            tgg00_Surrogate     me_package_id_F;
            tak_param_array     me_param_list_F;
        } C_true;
    /* false */
        struct tak_methodrecord_false
        {
            tsp00_DbObjectType  me_typefill_F;
            tak_debug_mode      me_debugfill_F;
            pasbool             me_sqlfill_F;
            pasbool             me_cursorfill_F;
            tak_proc_language   me_language_F;
            pasbool             me_deterministic_F;
            tsp00_Int2          me_filler2_F;
            tsp00_Int4          me_return_pos_F;
            tsp00_Int4          me_return_len_F;
            tgg00_Surrogate     me_owner_F;
        } C_false;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tsp00_DbObjectType &me_type                ()  { return this->variant.C_true.me_type_F; };
    tak_debug_mode     &me_debug               ()  { return this->variant.C_true.me_debug_F; };
    pasbool            &me_sql                 ()  { return this->variant.C_true.me_sql_F; };
    pasbool            &me_cursor              ()  { return this->variant.C_true.me_cursor_F; };
    tsp00_Int4         &me_dispid              ()  { return this->variant.C_true.me_dispid_F; };
    tsp00_C16          &me_coclsid             ()  { return this->variant.C_true.me_coclsid_F; };
    tsp00_C16          &me_iid                 ()  { return this->variant.C_true.me_iid_F; };
    tgg00_Surrogate    &me_package_id          ()  { return this->variant.C_true.me_package_id_F; };
    tak_param_array    &me_param_list          ()  { return this->variant.C_true.me_param_list_F; };
    tsp00_DbObjectType &me_typefill            ()  { return this->variant.C_false.me_typefill_F; };
    tak_debug_mode     &me_debugfill           ()  { return this->variant.C_false.me_debugfill_F; };
    pasbool            &me_sqlfill             ()  { return this->variant.C_false.me_sqlfill_F; };
    pasbool            &me_cursorfill          ()  { return this->variant.C_false.me_cursorfill_F; };
    tak_proc_language  &me_language            ()  { return this->variant.C_false.me_language_F; };
    pasbool            &me_deterministic       ()  { return this->variant.C_false.me_deterministic_F; };
    tsp00_Int2         &me_filler2             ()  { return this->variant.C_false.me_filler2_F; };
    tsp00_Int4         &me_return_pos          ()  { return this->variant.C_false.me_return_pos_F; };
    tsp00_Int4         &me_return_len          ()  { return this->variant.C_false.me_return_len_F; };
    tgg00_Surrogate    &me_owner               ()  { return this->variant.C_false.me_owner_F; };
    /* const accessor functions */
    const tsp00_DbObjectType &me_type                () const  { return this->variant.C_true.me_type_F; };
    const tak_debug_mode &me_debug               () const  { return this->variant.C_true.me_debug_F; };
    const pasbool      &me_sql                 () const  { return this->variant.C_true.me_sql_F; };
    const pasbool      &me_cursor              () const  { return this->variant.C_true.me_cursor_F; };
    const tsp00_Int4   &me_dispid              () const  { return this->variant.C_true.me_dispid_F; };
    const tsp00_C16    &me_coclsid             () const  { return this->variant.C_true.me_coclsid_F; };
    const tsp00_C16    &me_iid                 () const  { return this->variant.C_true.me_iid_F; };
    const tgg00_Surrogate &me_package_id          () const  { return this->variant.C_true.me_package_id_F; };
    const tak_param_array &me_param_list          () const  { return this->variant.C_true.me_param_list_F; };
    const tsp00_DbObjectType &me_typefill            () const  { return this->variant.C_false.me_typefill_F; };
    const tak_debug_mode &me_debugfill           () const  { return this->variant.C_false.me_debugfill_F; };
    const pasbool      &me_sqlfill             () const  { return this->variant.C_false.me_sqlfill_F; };
    const pasbool      &me_cursorfill          () const  { return this->variant.C_false.me_cursorfill_F; };
    const tak_proc_language &me_language            () const  { return this->variant.C_false.me_language_F; };
    const pasbool      &me_deterministic       () const  { return this->variant.C_false.me_deterministic_F; };
    const tsp00_Int2   &me_filler2             () const  { return this->variant.C_false.me_filler2_F; };
    const tsp00_Int4   &me_return_pos          () const  { return this->variant.C_false.me_return_pos_F; };
    const tsp00_Int4   &me_return_len          () const  { return this->variant.C_false.me_return_len_F; };
    const tgg00_Surrogate &me_owner               () const  { return this->variant.C_false.me_owner_F; };
#endif    /* defined (__cplusplus) */
} tak_methodrecord;

typedef struct                     tak_method_inforecord
{
    tsp00_Int2          mei_reclen;
    tsp00_Int2          mei_keylen;
    tsp00_Int2          mei_varcol_offset;
    tsp00_Int2          mei_varcol_cnt;
    tgg00_Surrogate     mei_surrogate;
    tsp00_C2            mei_entrytype;
    tsp00_C2            mei_linkage;
    tsp00_C2            mei_segmentid;
    pasbool             mei_comment;
    pasbool             mei_filler;
    tgg00_Surrogate     mei_owner;
    tgg00_Surrogate     mei_schema;
    tsp00_Int4          mei_date;
    tsp00_Int4          mei_time;
    tsp00_C64           mei_prog_id;
    tsp00_KnlIdentifier mei_name;
    tsp00_KnlIdentifier mei_language;
} tak_method_inforecord;

typedef struct                     tak_debug_info
{
    tsp00_Int2          dbg_stack_entry_index;
    tsp00_Int2          dbg_code_length;
    tsp00_Int4          dbg_code_pos;
} tak_debug_info;

typedef struct                     tak_debug_mapping_info
{
    tsp00_Int2          dmi_extcolno;
    tsp00_Int2          dmi_rec_pos;
} tak_debug_mapping_info;

typedef struct                     tak_method_debuginfo
{
    tsp00_Int2          med_reclen;
    tsp00_Int2          med_keylen;
    tsp00_Int2          med_varcol_offset;
    tsp00_Int2          med_varcol_cnt;
    tgg00_Surrogate     med_surrogate;
    tsp00_C2            med_entrytype;
    tsp00_C2            med_linkage;
    tsp00_C2            med_segmentid;
    tsp00_Int2          med_count;
    union
    {
    /* true */
        struct tak_method_debuginfo_true
        {
            ARRAY_VAR_MSP00 (         med_info_F, tak_debug_info, cak_max_debug_info);
        } C_true;
    /* false */
        struct tak_method_debuginfo_false
        {
            ARRAY_VAR_MSP00 (         med_mapping_F, tak_debug_mapping_info, cak_max_debug_mapping);
        } C_false;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    ARRAY_TYPESTRING_MSP00(tak_debug_info, cak_max_debug_info) &med_info               ()  { return this->variant.C_true.med_info_F; };
    ARRAY_TYPESTRING_MSP00(tak_debug_mapping_info, cak_max_debug_mapping) &med_mapping            ()  { return this->variant.C_false.med_mapping_F; };
    /* const accessor functions */
    const ARRAY_TYPESTRING_MSP00(tak_debug_info, cak_max_debug_info) &med_info               () const  { return this->variant.C_true.med_info_F; };
    const ARRAY_TYPESTRING_MSP00(tak_debug_mapping_info, cak_max_debug_mapping) &med_mapping            () const  { return this->variant.C_false.med_mapping_F; };
#endif    /* defined (__cplusplus) */
} tak_method_debuginfo;
typedef OLDPASCALSTRING_TYPE_MSP00 (  tak_method_filename, 1025);
typedef CSTRING_TYPE_MSP00      (  tak_method_filenamec, 1025);

typedef struct                     tak_method_fileinforecord
{
    tsp00_Int2          mef_reclen;
    tsp00_Int2          mef_keylen;
    tsp00_Int2          mef_varcol_offset;
    tsp00_Int2          mef_varcol_cnt;
    tgg00_Surrogate     mef_surrogate;
    tsp00_C2            mef_entrytype;
    tsp00_C2            mef_linkage;
    tsp00_C2            mef_segmentid;
    tsp00_C2            mef_filler;
    tsp00_C32           mef_md5_footprint;
    tak_method_filename mef_filename;
} tak_method_fileinforecord;

typedef struct                     tak_methodrefrecord
{
    tsp00_Int2          mrf_reclen;
    tsp00_Int2          mrf_keylen;
    tsp00_Int2          mrf_varcol_offset;
    tsp00_Int2          mrf_varcol_cnt;
    tgg00_Surrogate     mrf_surrogate;
    tsp00_C2            mrf_entrytype;
    tsp00_C2            mrf_linkage;
    tsp00_KnlIdentifier mrf_methodname;
    tsp00_C2            mrf_segmentid;
    tsp00_DbObjectType  mrf_type;
    pasbool             mrf_filler;
    tgg00_Surrogate     mrf_method_id;
} tak_methodrefrecord;

typedef struct                     tak_type_uid_map_record
{
    tsp00_Int2          tum_reclen;
    tsp00_Int2          tum_keylen;
    tsp00_Int2          tum_varcol_offset;
    tsp00_Int2          tum_varcol_cnt;
    tsp00_C8            tum_uid1;
    tsp00_C2            tum_entrytype;
    tsp00_C2            tum_linkage;
    tsp00_C8            tum_uid2;
    tsp00_C2            tum_segmentid;
    tsp00_C2            tum_id;
} tak_type_uid_map_record;

typedef struct                     tak_class_map_guid_record
{
    tsp00_Int2          cmr_reclen;
    tsp00_Int2          cmr_keylen;
    tsp00_Int2          cmr_varcol_offset;
    tsp00_Int2          cmr_varcol_cnt;
    tgg00_Surrogate     cmr_surrogate;
    tsp00_C2            cmr_entrytype;
    tsp00_C2            cmr_linkage;
    tsp00_C2            cmr_segmentid;
    tsp00_Int4          cmr_container_no;
    tsp00_Int4          cmr_schema;
    tsp00_C16           cmr_guid;
} tak_class_map_guid_record;

typedef struct                     tak_class_record
{
    tsp00_Int2          freclen;
    tsp00_Int2          fkeylen;
    tsp00_Int2          fvarcol_offset;
    tsp00_Int2          fvarcol_cnt;
    tgg00_Surrogate     fschema;
    tsp00_C2            fentrytype;
    tsp00_C2            flinkage;
    tsp00_C16           fguid;
    tsp00_C2            fsegmentid;
    pasbool             findexexist;
    pasbool             ffiller;
    tsp00_Int4          fsize;
    tgg00_FileId        ftree;
    tsp00_Int4          fdate;
    tsp00_Int4          ftime;
    tsp00_KnlIdentifier fclassname;
} tak_class_record;

typedef struct                     tak_schema_ref_record
{
    tsp00_Int2          schr_reclen;
    tsp00_Int2          schr_keylen;
    tsp00_Int2          schrvarcol_offset;
    tsp00_Int2          schrvarcol_cnt;
    tgg00_Surrogate     schr_surrogate;
    tsp00_C2            schr_entrytype;
    tsp00_C2            schr_linkage;
    tsp00_KnlIdentifier schr_name;
    tsp00_C2            schr_segmentid;
    tsp00_Int4          schr_filler;
    tsp00_Int4          schr_schema_id;
} tak_schema_ref_record;

typedef struct                     tak_schema_record
{
    tsp00_Int2          sch_reclen;
    tsp00_Int2          sch_keylen;
    tsp00_Int2          sch_varcol_offset;
    tsp00_Int2          sch_varcol_cnt;
    tgg00_Surrogate     sch_surrogate;
    tsp00_C2            sch_entrytype;
    tsp00_C2            sch_linkage;
    tsp00_C2            sch_segmentid;
    tsp00_C2            sch_filler;
    tsp00_Int4          sch_date;
    tsp00_Int4          sch_time;
    tsp00_KnlIdentifier sch_name;
} tak_schema_record;

typedef struct                     tak_version_record
{
    tsp00_Int2          vreclen;
    tsp00_Int2          vkeylen;
    tsp00_Int2          vvarcol_offset;
    tsp00_Int2          vvarcol_cnt;
    tgg00_Surrogate     vsurrogate;
    tsp00_C2            ventrytype;
    tsp00_C2            vlinkage;
    tak_oms_version_id  vversion_id;
    tsp00_C2            vsegmentid;
    tgg91_TransNo       vconsistview;
    pasbool             vmarked;
    pasbool             vfiller;
    tsp00_Int4          vdate;
    tsp00_Int4          vtime;
    tgg00_FileId        vupdtree;
} tak_version_record;
    /* synonym catalog representation */

typedef struct                     tak_synonymrecord
{
    tsp00_Int2          synreclen;
    tsp00_Int2          synkeylen;
    tsp00_Int2          synvarcol_offset;
    tsp00_Int2          synvarcol_cnt;
    tgg00_Surrogate     synsurrogate;
    tsp00_C2            synentrytype;
    tsp00_C2            synseqno;
    tsp00_C2            synsegmentid;
    tsp00_Int2          syn_maxcol;
    pasbool             syn_internal;
    pasbool             synfiller0;
    tgg00_TableKind     syn_tablekind;
    pasbool             syncomment;
    tgg00_Surrogate     syn_tableid;
    tsp00_Int4          syn_time;
    tsp00_Int4          syn_date;
    tgg00_Surrogate     syn_owner;
    tgg00_Surrogate     syn_authid;
    tsp00_KnlIdentifier syn_tablen;
} tak_synonymrecord;
    /* cammand info catalog representation */

typedef struct                     tak_commandinforecord
{
    tsp00_Int2          cireclen;
    tsp00_Int2          cikeylen;
    tsp00_Int2          civarcol_offset;
    tsp00_Int2          civarcol_cnt;
    tgg00_Surrogate     cisurrogate;
    tsp00_C2            cientrytype;
    tsp00_C2            cilinkage;
    tsp00_C8            cicommand_id;
} tak_commandinforecord;
    /*! don't remove/regroup any of following enum types because this !*/
    /*! type will be made persitent on view catalog records           !*/

typedef enum tak_jcolprop_Enum
{
    jtkey,
    jtfirstkey,
    jtonlykey,
    jtfiller1,
    jtmulti,
    jtfiller2,
    jtascii
} tak_jcolprop_Enum;
typedef tak_jcolprop_Enum tak_jcolprop_Param;
typedef ENUM_TYPE_MSP00 (          tak_jcolprop, tak_jcolprop_Enum, jtascii, unsigned char);
typedef SMALLSET_TYPE_MSP00 (      tak_jcolpropset, tak_jcolprop_Enum, jtascii);
typedef SET_TYPE_MSP00 (           tak_joinset, int, cak00_maxsources, 32);

typedef struct                     tak_one_joinpart
{
    tsp00_Int4          jop_multiplier;
    tsp00_Int2          jop_fieldno;
    tsp00_Int2          jop_startstack;
    tsp00_Int2          jop_cntstack;
    tsp00_Int2          jop_tableno;
    tsp00_Int2          jop_filler0;
    tsp00_Int2          jop_inoutlen;
    tsp00_Int2          jop_outpos;
    tsp00_DataType      jop_datatyp;
    tak_jcolpropset     jop_propset;
    pasbool             jop_outer_join;
    tsp00_C3            jop_filler;
} tak_one_joinpart;

typedef struct                     tak_one_join
{
    ARRAY_VAR_MSP00 (         jo_recs, tak_one_joinpart, 2);
    tgg00_StackOpType   jo_op;
    pasbool             jo_no_join;
    tsp00_Uint1         jo_partno;
    tsp00_Uint1         jo_col_upd_stat;
} tak_one_join;
typedef ARRAY_TYPE_MSP00 (         tak_joinarr, tak_one_join, MAX_JOINARR_ELEMS_AK00 - 0 + 1);
typedef tak_joinarr               *tak_joinarr_ptr;

typedef struct                     tak_joinrec
{
    tak_joinarr_ptr     jrc_joinarr;

#if defined (BIT64)


#else

    OLDPASCALSTRING_VAR_MSP00 (  jrc_alpha_fill, 4);

#endif

    tsp00_Int2          jrc_cnt;
    tsp00_Int2          jrc_capacity;
    pasbool             jrc_col_upd;
    tsp00_C3            jrc_filler;
} tak_joinrec;
    /*======== view catalog representation ========*/
    /* view text catalog representation */

typedef enum tak_from_table_info_Enum
{
    fti_none,
    fti_synonym,
    fti_withoutSchema
} tak_from_table_info_Enum;
typedef tak_from_table_info_Enum tak_from_table_info_Param;
typedef ENUM_TYPE_MSP00 (          tak_from_table_info, tak_from_table_info_Enum, fti_withoutSchema, unsigned char);

typedef struct                     tak_vttabdef
{
    tgg00_Surrogate     vtttableid;
    tgg00_Surrogate     vttreftabid;
    tsp00_Int2          vtttabcount;
    pasbool             vttrefname;
    tsp00_Uint1         vttfiller;
    tak_from_table_info vttfromTableInfo;
    pasbool             vttcomplex_view;
    tsp00_Int2          vtttab_n_pos;
} tak_vttabdef;
typedef OLDPASCALSTRING_TYPE_MSP00 (  tak_viewtext, mxak_viewtext);
typedef CSTRING_TYPE_MSP00      (  tak_viewtextc, mxak_viewtext);

typedef struct                     tak_viewtextrecord
{
    tsp00_Int2          vtreclen;
    tsp00_Int2          vtkeylen;
    tsp00_Int2          vtvarcol_offset;
    tsp00_Int2          vtvarcol_cnt;
    tgg00_Surrogate     vtsurrogate;
    tsp00_C2            vtentrytype;
    tsp00_C2            vtsegno;
    tsp00_C2            vtsegmentid;
    tsp00_Int2          vtfromtabcnt;
    tsp00_Int2          vttabcount;
    tsp00_Int2          vttextlength;
    pasbool             vtnextexist;
    tsp00_Uint1         vtcontext;
    tsp00_Int2          vtselect_pos;
    union
    {
    /* true */
        struct tak_viewtextrecord_true
        {
            ARRAY_VAR_MSP00 (         vttab_F, tak_vttabdef, cak00_maxsources);
        } C_true;
    /* false */
        struct tak_viewtextrecord_false
        {
            tak_viewtext        vttbuf_F;
        } C_false;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    ARRAY_TYPESTRING_MSP00(tak_vttabdef, cak00_maxsources) &vttab                  ()  { return this->variant.C_true.vttab_F; };
    tak_viewtext       &vttbuf                 ()  { return this->variant.C_false.vttbuf_F; };
    /* const accessor functions */
    const ARRAY_TYPESTRING_MSP00(tak_vttabdef, cak00_maxsources) &vttab                  () const  { return this->variant.C_true.vttab_F; };
    const tak_viewtext &vttbuf                 () const  { return this->variant.C_false.vttbuf_F; };
#endif    /* defined (__cplusplus) */
} tak_viewtextrecord;
    /* view column desription catalog representation */

typedef struct                     tak_vcolumndescription
{
    tsp00_Int2          vfromtabno;
    tsp00_Int2          vfromextcolno;
    tsp00_Int4          vn_pos;
    tsp00_Int2          vextcolno;
    tsp00_DataType      vdatatype;
    pasbool             vfiller;
} tak_vcolumndescription;

typedef struct                     tak_viewdescrecord
{
    tsp00_Int2          vdreclen;
    tsp00_Int2          vdkeylen;
    tsp00_Int2          vdvarcol_offset;
    tsp00_Int2          vdvarcol_cnt;
    tgg00_Surrogate     vdsurrogate;
    tsp00_C2            vdentrytype;
    tsp00_Int2          vdsegno;
    tsp00_C2            vsegmentid;
    tsp00_Int2          vdesc_cnt;
    pasbool             vdesc_next;
    pasbool             vdesc_fill1;
    tsp00_Int2          vdesc_fill2;
    ARRAY_VAR_MSP00 (         vdescription, tak_vcolumndescription, cak_maxviewdesc);
} tak_viewdescrecord;
    /* view qualification catalog representation */

typedef struct                     tak_viewtabdef
{
    tgg00_Surrogate     vttableid;
    tsp00_Int2          vtfiller;
    pasbool             vtins_for_upd;
    pasbool             vtone_to_one;
    tak_charset         vtnot_used_links;
} tak_viewtabdef;
typedef ARRAY_TYPE_MSP00 (         tak_viewtabarr, tak_viewtabdef, cak_max_viewqual_tab);

typedef struct                     tak_one_viewjoin
{
    tsp00_Int2          j1fieldno;
    tsp00_Int2          j1tableno;
    tsp00_Int2          j1cntstack;
    tsp00_Int2          j1inoutlen;
    tsp00_Int2          j1outpos;
    tsp00_Int2          j1startstack;
    tak_jcolpropset     j1propset;
    tsp00_Uint1         j12partno;
    tsp00_Int2          j2fieldno;
    tsp00_Int2          j2tableno;
    tsp00_Int2          j2cntstack;
    tsp00_Int2          j2inoutlen;
    tsp00_Int2          j2outpos;
    tak_jcolpropset     j2propset;
    tgg00_StackOpType   j12operator;
    tsp00_DataType      j1datatype;
    tsp00_DataType      j2datatype;
} tak_one_viewjoin;
typedef ARRAY_TYPE_MSP00 (         tak_viewjoins, tak_one_viewjoin, cak_max_viewqual_join);
typedef ARRAY_TYPE_MSP00 (         tak_view_stack_entries, tgg00_StackEntry, cak_max_viewqual_stack);
typedef OLDPASCALSTRING_TYPE_MSP00 (  tak_viewbuf, cak_max_viewqual_data);
typedef CSTRING_TYPE_MSP00      (  tak_viewbufc, cak_max_viewqual_data);

typedef struct                     tak_one_derived_desc
{
    tsp00_Int2          drecno;
    tsp00_Int2          dextno;
    tsp00_Int2          dexttabno;
    tsp00_Int2          dtabno;
} tak_one_derived_desc;
typedef ARRAY_TYPE_MSP00 (         tak_derived_col_info, tak_one_derived_desc, cak_max_viewqual_derivedcol);

typedef struct                     tak_viewqual_basis_record
{
    tsp00_Int2          vreclen;
    tsp00_Int2          vkeylen;
    tsp00_Int2          vvarcol_offset;
    tsp00_Int2          vvarcol_cnt;
    tgg00_Surrogate     vsurrogate;
    tsp00_C2            ventrytype;
    tsp00_C2            vlinkage;
    tsp00_C2            vsegmentid;
    pasbool             vfiller;
    pasbool             vjoin_exists;
    pasbool             vstack_exists;
    pasbool             vderived_exists;
    tsp00_Int2          vbasetabcnt;
    tak_viewtabarr      vtable;
} tak_viewqual_basis_record;

typedef struct                     tak_viewqual_join_record
{
    tsp00_Int2          vreclen;
    tsp00_Int2          vkeylen;
    tsp00_Int2          vvarcol_offset;
    tsp00_Int2          vvarcol_cnt;
    tgg00_Surrogate     vsurrogate;
    tsp00_C2            ventrytype;
    tsp00_C2            vlinkage;
    tsp00_C2            vsegmentid;
    tsp00_Int2          vjoincount;
    tak_viewjoins       vjoin;
} tak_viewqual_join_record;

typedef struct                     tak_viewqual_stack_record
{
    tsp00_Int2          vreclen;
    tsp00_Int2          vkeylen;
    tsp00_Int2          vvarcol_offset;
    tsp00_Int2          vvarcol_cnt;
    tgg00_Surrogate     vsurrogate;
    tsp00_C2            ventrytype;
    tsp00_C2            vlinkage;
    tsp00_C2            vsegmentid;
    tsp00_C2            vfiller;
    tsp00_Int2          vview_offs;
    tsp00_Int2          vstcount;
    tsp00_Int4          vdatapos;
    tsp00_Int4          vdatalen;
    union
    {
    /* true */
        struct tak_viewqual_stack_record_true
        {
            tak_view_stack_entries vstack_F;
        } C_true;
    /* false */
        struct tak_viewqual_stack_record_false
        {
            tak_viewbuf         vdata_F;
        } C_false;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tak_view_stack_entries &vstack                 ()  { return this->variant.C_true.vstack_F; };
    tak_viewbuf        &vdata                  ()  { return this->variant.C_false.vdata_F; };
    /* const accessor functions */
    const tak_view_stack_entries &vstack                 () const  { return this->variant.C_true.vstack_F; };
    const tak_viewbuf  &vdata                  () const  { return this->variant.C_false.vdata_F; };
#endif    /* defined (__cplusplus) */
} tak_viewqual_stack_record;

typedef struct                     tak_viewqual_derivedcol_record
{
    tsp00_Int2          vreclen;
    tsp00_Int2          vkeylen;
    tsp00_Int2          vvarcol_offset;
    tsp00_Int2          vvarcol_cnt;
    tgg00_Surrogate     vsurrogate;
    tsp00_C2            ventrytype;
    tsp00_C2            vlinkage;
    tsp00_C2            vsegmentid;
    tsp00_Int2          vderivedcnt;
    tak_derived_col_info vderived_cols;
} tak_viewqual_derivedcol_record;

typedef struct                     tak_viewkeyrecord
{
    tsp00_Int2          vkreclen;
    tsp00_Int2          vkkeylen;
    tsp00_Int2          vkvarcol_offset;
    tsp00_Int2          vkvarcol_cnt;
    tsp00_C12           vkkey;
    tsp00_C2            vksegmentid;
    tsp00_Int2          vkjvkeylen;
    tsp00_Key           vkjvkey;
} tak_viewkeyrecord;

typedef struct                     tak_col_stats
{
    tsp00_Int4          cst_dist_values;
    tsp00_Int4          cst_pages;
    tsp00_Int4          cst_avg_lst_len;
} tak_col_stats;

typedef struct                     tak_statisticsinfo
{
    tsp00_Int2          sti_reclen;
    tsp00_Int2          sti_keylen;
    tsp00_Int2          sti_varcol_offset;
    tsp00_Int2          sti_varcol_cnt;
    tgg00_Surrogate     sti_tabid;
    tsp00_C2            sti_entrytype;
    tsp00_C2            sti_linkage;
    tsp00_C2            sti_segmentid;
    tsp00_Int2          sti_col_cnt;
    tsp00_Int4          sti_rows;
    tsp00_Int4          sti_pages;
    tsp00_Int4          sti_date;
    tsp00_Int4          sti_time;
    tsp00_Int2          sti_filler;
    tak_columnset       sti_index;
    ARRAY_VAR_MSP00 (         sti_col_stats, tak_col_stats, MAX_COL_STATS_AK00);
} tak_statisticsinfo;
    /* result structure catalog representation */

typedef struct                     tak_resultrecord
{
    tsp00_Int2          breclen;
    tsp00_Int2          bkeylen;
    tsp00_Int2          bvarcol_offset;
    tsp00_Int2          bvarcol_cnt;
    tsp00_C12           bkey;
    tsp00_C2            bsegmentid;
    tsp00_Int2          bfirstindex;
    tsp00_Int2          blastindex;
    tgg00_TableKind     btablekind;
    tgg00_Linkset       blinkexist;
    tgg00_FileId        btreeid;
    tgg00_Surrogate     bauthid;
    tgg00_Surrogate     bschema;
    tgg00_Surrogate     bschemacontext;
    tsp00_Int2          breccnt;
    tsp00_Int2          bmaxcol;
    tsp00_Int2          bmaxreclen;
    tsp00_Int2          bkeycolcount;
    tsp00_Int2          blenfixedcol;
    tsp00_Int2          bvarcolcount;
    tsp00_Int2          bcntdropcol;
    tsp00_Int2          bstringcount;
    tsp00_Uint1         blongvarcolcnt;
    pasbool             bcomment;
    tsp00_Int2          bavgrowlen;
    tsp00_Int2          blastkeyind;
    tsp00_Int2          bfirstcolind;
    tsp00_Int4          bdatecreate;
    tsp00_Int4          btimecreate;
    tsp00_Int4          bupdstatdate;
    tsp00_Int4          bupdstattime;
    tsp00_Int4          bpages;
    tsp00_Int4          brows;
    tsp00_Int4          balterdate;
    tsp00_Int4          baltertime;
    tsp00_Int4          bsample;
    tsp00_Int2          boutcolno;
    pasbool             bunloaded;
    pasbool             bindexexist;
    tsp00_Uint1         bv_level;
    tsp00_Uint1         bv_tabcount;
    tgg00_TableKind     bv_tablekind;
    pasbool             bv_checkopt;
    pasbool             bv_qualexist;
    tgg04_Distinct      bv_distinct;
    pasbool             bv_viewlist;
    pasbool             bdef_stamp;
    tak_view_attributes bview_attributes;
    tsp00_SqlMode       bsqlmode;
    tsp00_Int2          bnamed_constr;
    tgg00_ShowKind      bshow_kind;
    pasbool             brecreate_view;
    tgg00_ShowKind      bshowkind;
    tak_table_attribute_set battributes;
    tsp00_KnlIdentifierPtr btablen;
    tak_basecolinfo_ptr bptr;
    tak_columnarr      *bcolumnExtension;
    tsp00_Int2          bcolumnSplitIdx;
    pasbool             bupdstatinserted;
    union
    {
    /* 1 */
        struct tak_resultrecord_1
        {
            tak_colindex        bextcolindex_F;
        } C_1;
    /* 2 */
        struct tak_resultrecord_2
        {
            tak_columnarr       bcolumn_F;
        } C_2;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tak_colindex       &bextcolindex           ()  { return this->variant.C_1.bextcolindex_F; };
    tak_columnarr      &bcolumn                ()  { return this->variant.C_2.bcolumn_F; };
    /* const accessor functions */
    const tak_colindex &bextcolindex           () const  { return this->variant.C_1.bextcolindex_F; };
    const tak_columnarr &bcolumn                () const  { return this->variant.C_2.bcolumn_F; };
#endif    /* defined (__cplusplus) */
} tak_resultrecord;

typedef struct                     tak_named_subtrans_info
{
    tsp00_KnlIdentifier str_name;
    tgg00_SubtransNo    str_no;
} tak_named_subtrans_info;

typedef struct                     tak_subtransrecord
{
    tsp00_Int2          str_reclen;
    tsp00_Int2          str_keylen;
    tsp00_Int2          str_varcol_offset;
    tsp00_Int2          str_varcol_cnt;
    tgg00_Surrogate     str_key;
    tsp00_C2            str_etype;
    tsp00_C2            str_linkage;
    tsp00_Int2          str_count;
    tsp00_Int2          str_filler;
    ARRAY_VAR_MSP00 (         str_subtrans, tak_named_subtrans_info, cak_max_named_subtrans);
} tak_subtransrecord;

typedef struct                     tak_scoldescrecord
{
    tsp00_Int2          scd_reclen;
    tsp00_Int2          scd_keylen;
    tsp00_Int2          scd_varcol_offset;
    tsp00_Int2          scd_varcol_cnt;
    tgg00_Surrogate     scd_colkey;
    tsp00_C2            scd_etype;
    tsp00_C2            scd_filler;
    tgg04_StringFileDescr scd_stringfd;
    tgg00_Lock          scd_lock;
    tsp00_Key           scd_key;
} tak_scoldescrecord;

typedef enum tak_changetype_Enum
{
    ch_to_ascii,
    ch_number_change_in_next_entry,
    ch_date,
    ch_time_or_timestamp,
    ch_to_multibyte_set,
    ch_to_longsupport,
    ch_to_warn2
} tak_changetype_Enum;
typedef tak_changetype_Enum tak_changetype_Param;
typedef ENUM_TYPE_MSP00 (          tak_changetype, tak_changetype_Enum, ch_to_warn2, unsigned char);

typedef enum tak_number_changetype_Enum
{
    nch_nativeint8,
    nch_div_by_count
} tak_number_changetype_Enum;
typedef tak_number_changetype_Enum tak_number_changetype_Param;
typedef ENUM_TYPE_MSP00 (          tak_number_changetype, tak_number_changetype_Enum, nch_div_by_count, unsigned char);
typedef SMALLSET_TYPE_MSP00 (      tak_number_changetype_set, tak_number_changetype_Enum, nch_div_by_count);

typedef struct                     tak_change_colinfo
{
    tsp00_Int2          ch_startpos;
    char                ch_length;
    union
    {
    /* true */
        struct tak_change_colinfo_true
        {
            SMALLSET_VAR_MSP00 (      ch_type_F, tak_changetype_Enum, ch_to_warn2);
        } C_true;
    /* false */
        struct tak_change_colinfo_false
        {
            tak_number_changetype_set ch_number_type_F;
        } C_false;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tsp00_SmallSet<tak_changetype_Enum, ch_to_warn2> &ch_type                ()  { return this->variant.C_true.ch_type_F; };
    tak_number_changetype_set &ch_number_type         ()  { return this->variant.C_false.ch_number_type_F; };
    /* const accessor functions */
    const tsp00_SmallSet<tak_changetype_Enum, ch_to_warn2> &ch_type                () const  { return this->variant.C_true.ch_type_F; };
    const tak_number_changetype_set &ch_number_type         () const  { return this->variant.C_false.ch_number_type_F; };
#endif    /* defined (__cplusplus) */
} tak_change_colinfo;
typedef ARRAY_TYPE_MSP00 (         tak_column_change_arr, tak_change_colinfo, MAX_COL_PER_TAB_GG00);

typedef struct                     tak_changerecord
{
    tsp00_Int2          cr_colcount;
    pasbool             cr_was_opt;
    pasbool             cr_filler;
    tak_column_change_arr cr_columns;
} tak_changerecord;

typedef struct                     tak_changeinfos
{
    tsp00_Int2          ci_rec_len;
    tsp00_Int2          ci_key_len;
    tsp00_Int2          ci_varcol_offset;
    tsp00_Int2          ci_varcol_cnt;
    tgg00_Surrogate     ci_surrogate;
    tsp00_C2            ci_entrytype;
    tsp00_Int2          ci_segno;
    tsp00_Int4          ci_fullen;
    tsp00_Int2          ci_colcount;
    tsp00_Int2          ci_filler;
    tak_column_change_arr ci_ch_columns;
} tak_changeinfos;

typedef struct                     tak_keyspec
{
    tsp00_Int2          ks_pos;
    tsp00_Int2          ks_len;
} tak_keyspec;

typedef struct                     tak_two_keyspecs
{
    tak_keyspec         reckeyspec;
    tak_keyspec         listkeyspec;
} tak_two_keyspecs;
typedef OLDPASCALSTRING_TYPE_MSP00 (  tak_res_keysbuf, mxak_res_keysbuf);
typedef CSTRING_TYPE_MSP00      (  tak_res_keysbufc, mxak_res_keysbuf);

typedef struct                     tak_resultkeyrecord
{
    tsp00_Int2          res_rec_len;
    tsp00_Int2          res_key_len;
    tsp00_Int2          res_varcol_offset;
    tsp00_Int2          res_varcol_cnt;
    tgg00_Surrogate     res_surrogate;
    tsp00_C2            res_entrytype;
    tsp00_Int2          res_segno;
    tsp00_Int4          res_fullen;
    tak_two_keyspecs    res_startkeys;
    tak_two_keyspecs    res_stopkeys;
    tak_two_keyspecs    res_nextkeys;
    tsp00_Int4          res_nextpos;
    tak_two_keyspecs    res_prevkeys;
    tsp00_Int4          res_prevpos;
    tak_keyspec         res_updkey;
    tgg00_TwoFileIds    res_treeids;
    tgg00_FileId        res_restree;
    tsp00_KnlIdentifier res_resultname;
    tsp00_Int4          res_rowno;
    tsp00_Int4          res_actres;
    tsp00_Int2          res_reclen;
    tsp00_Int2          res_length;
    tsp00_Int2          res_keylen;
    tsp00_Int2          res_outcolno;
    tsp00_Int2          res_useupdk;
    tak_sresstate       res_resstate;
    tak_decresstate     res_decresdel;
    pasbool             res_bof;
    pasbool             res_eof;
    tsp00_C1            res_updchar;
    pasbool             res_for_update;
    tgg00_Surrogate     res_updtabid;
    tgg00_Surrogate     res_unlocktab;
    tsp00_Int2          res_primkeylen;
    tsp00_Int2          res_minkeylen;
    tsp00_Int2          res_dist_optim;
    tgg00_RefInfoIndex  res_strat_info;
    tsp00_Uint1         res_parsinfob;
    tgg00_DateTimeFormat res_pars_dt_format;
    tsp00_SqlMode       res_pars_sqlmode;
    pasbool             res_reverse_access;
    tgg00_QualKind      res_qual_kind;
    tsp00_Int4          res_known_pages;
    tsp00_Int4          res_searched_pages;
    tsp00_Int2          res_upd_tabcnt;
    pasbool             res_order;
    pasbool             res_build;
    tsp00_Int4          res_limit_offset;
    tsp00_Int4          res_upper_limit;
    tgg00_Surrogate     res_analyze_cmdid;
    union
    {
    /* true */
        struct tak_resultkeyrecord_true
        {
            tak_changerecord    res_change_F;
        } C_true;
    /* false */
        struct tak_resultkeyrecord_false
        {
            tak_res_keysbuf     res_keysbuf_F;
        } C_false;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tak_changerecord   &res_change             ()  { return this->variant.C_true.res_change_F; };
    tak_res_keysbuf    &res_keysbuf            ()  { return this->variant.C_false.res_keysbuf_F; };
    /* const accessor functions */
    const tak_changerecord &res_change             () const  { return this->variant.C_true.res_change_F; };
    const tak_res_keysbuf &res_keysbuf            () const  { return this->variant.C_false.res_keysbuf_F; };
#endif    /* defined (__cplusplus) */
} tak_resultkeyrecord;

typedef struct                     tak_shortinfo
{
    union
    {
    /* true */
        struct tak_shortinfo_true
        {
            tsp00_C2            simodeiotype_F;
            tsp00_C2            sityplen_data_F;
            tsp00_C2            sifrac_inoutl_F;
            tsp00_Int2          sibufpos_F;
        } C_true;
    /* false */
        struct tak_shortinfo_false
        {
            tsp00_Int4          sifiller1_F;
            tsp00_Int2          silonglength_F;
            tsp00_Int2          sifiller2_F;
        } C_false;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tsp00_C2           &simodeiotype           ()  { return this->variant.C_true.simodeiotype_F; };
    tsp00_C2           &sityplen_data          ()  { return this->variant.C_true.sityplen_data_F; };
    tsp00_C2           &sifrac_inoutl          ()  { return this->variant.C_true.sifrac_inoutl_F; };
    tsp00_Int2         &sibufpos               ()  { return this->variant.C_true.sibufpos_F; };
    tsp00_Int4         &sifiller1              ()  { return this->variant.C_false.sifiller1_F; };
    tsp00_Int2         &silonglength           ()  { return this->variant.C_false.silonglength_F; };
    tsp00_Int2         &sifiller2              ()  { return this->variant.C_false.sifiller2_F; };
    /* const accessor functions */
    const tsp00_C2     &simodeiotype           () const  { return this->variant.C_true.simodeiotype_F; };
    const tsp00_C2     &sityplen_data          () const  { return this->variant.C_true.sityplen_data_F; };
    const tsp00_C2     &sifrac_inoutl          () const  { return this->variant.C_true.sifrac_inoutl_F; };
    const tsp00_Int2   &sibufpos               () const  { return this->variant.C_true.sibufpos_F; };
    const tsp00_Int4   &sifiller1              () const  { return this->variant.C_false.sifiller1_F; };
    const tsp00_Int2   &silonglength           () const  { return this->variant.C_false.silonglength_F; };
    const tsp00_Int2   &sifiller2              () const  { return this->variant.C_false.sifiller2_F; };
#endif    /* defined (__cplusplus) */
} tak_shortinfo;
typedef ARRAY_TYPE_MSP00 (         tak_shortinfoarr, tak_shortinfo, cak_maxshortinfo);
typedef ARRAY_TYPE_MSP00 (         tak_paraminfoarr, tsp1_param_info, cak_maxparams);
typedef tak_paraminfoarr          *tak_parinfarr_ptr;

typedef struct                     tak_shortinforecord
{
    tsp00_Int2          sireclen;
    tsp00_Int2          sikeylen;
    tsp00_Int2          sivarcol_offset;
    tsp00_Int2          sivarcol_cnt;
    tgg00_Surrogate     sisurrogate;
    tsp00_C2            sientrytype;
    tsp00_Int2          sisegno;
    tsp00_Int4          sifullen;
    tsp00_Int2          sisl;
    tsp00_Int2          sicount;
    tak_paraminfoarr    siinfo;
} tak_shortinforecord;
typedef struct tak_systembuffer tak_systembuffer;
typedef tak_systembuffer          *tak_sysbufferaddress;
typedef OLDPASCALSTRING_TYPE_MSP00 (  tak_colnamesbuf, mxak_colnamesbuf);
typedef CSTRING_TYPE_MSP00      (  tak_colnamesbufc, mxak_colnamesbuf);

typedef struct                     tak_columnnamesrecord
{
    tsp00_Int2          cnreclen;
    tsp00_Int2          cnkeylen;
    tsp00_Int2          cnvarcol_offset;
    tsp00_Int2          cnvarcol_cnt;
    tgg00_Surrogate     cnsurrogate;
    tsp00_C2            cnentrytype;
    tsp00_Int2          cnsegno;
    tsp00_Int4          cnbit64fill;
    union
    {
    /* true */
        struct tak_columnnamesrecord_true
        {
            tsp00_Int4          cnfullen_F;
            tsp00_Int2          cncount_F;
        } C_true;
    /* false */
        struct tak_columnnamesrecord_false
        {
            tak_colnamesbuf     cnbuf_F;
            tak_sysbufferaddress cnnext_F;
            tak_sysbufferaddress cnlast_F;
        } C_false;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tsp00_Int4         &cnfullen               ()  { return this->variant.C_true.cnfullen_F; };
    tsp00_Int2         &cncount                ()  { return this->variant.C_true.cncount_F; };
    tak_colnamesbuf    &cnbuf                  ()  { return this->variant.C_false.cnbuf_F; };
    tak_sysbufferaddress &cnnext                 ()  { return this->variant.C_false.cnnext_F; };
    tak_sysbufferaddress &cnlast                 ()  { return this->variant.C_false.cnlast_F; };
    /* const accessor functions */
    const tsp00_Int4   &cnfullen               () const  { return this->variant.C_true.cnfullen_F; };
    const tsp00_Int2   &cncount                () const  { return this->variant.C_true.cncount_F; };
    const tak_colnamesbuf &cnbuf                  () const  { return this->variant.C_false.cnbuf_F; };
    const tak_sysbufferaddress &cnnext                 () const  { return this->variant.C_false.cnnext_F; };
    const tak_sysbufferaddress &cnlast                 () const  { return this->variant.C_false.cnlast_F; };
#endif    /* defined (__cplusplus) */
} tak_columnnamesrecord;

typedef enum tak_fp_kind_type_Enum
{
    fp_catalog_column,
    fp_unused1,
    fp_result_name,
    fp_current_of_key,
    fp_unused2,
    fp_escape_val,
    fp_fill_part2_to_len,
    fp_get_name_value,
    fp_ordered_select_key,
    fp_pseudo_ins_select,
    fp_reclen,
    fp_unused3,
    fp_temp_table_key,
    fp_use_old_values_up_to_len,
    fp_verify_constraint,
    fp_verify_view,
    fp_verify_not_null,
    fp_viewkey_to_mess_buf,
    fp_val_varcol_with_len,
    fp_val_all_with_len,
    fp_val_all_without_l,
    fp_lock_val,
    fp_last_lock,
    fp_selord_val_no_len,
    fp_val_from_res_buffer,
    fp_fieldlist,
    fp_rowno,
    fp_limit_offset,
    fp_upper_limit
} tak_fp_kind_type_Enum;
typedef tak_fp_kind_type_Enum tak_fp_kind_type_Param;
typedef ENUM_TYPE_MSP00 (          tak_fp_kind_type, tak_fp_kind_type_Enum, fp_upper_limit, unsigned char);
    /**/
typedef tak_fp_kind_type          *tak_fp_kind_type_ptr;
    /**/

typedef enum tak_fcolprop_Enum
{
    fp_opt,
    fp_like,
    fp_like1,
    fp_like2,
    fp_serial,
    fp_default,
    fp_escape
} tak_fcolprop_Enum;
typedef tak_fcolprop_Enum tak_fcolprop_Param;
typedef ENUM_TYPE_MSP00 (          tak_fcolprop, tak_fcolprop_Enum, fp_escape, unsigned char);
typedef SMALLSET_TYPE_MSP00 (      tak_fcolpropset, tak_fcolprop_Enum, fp_escape);

typedef struct                     tak_len_and_frac
{
    union
    {
    /* true */
        struct tak_len_and_frac_true
        {
            tsp00_Int2          lf_datalen_F;
        } C_true;
    /* false */
        struct tak_len_and_frac_false
        {
            tsp00_Uint1         lf_fixed_datalen_F;
            tsp00_Uint1         lf_frac_F;
        } C_false;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tsp00_Int2         &lf_datalen             ()  { return this->variant.C_true.lf_datalen_F; };
    tsp00_Uint1        &lf_fixed_datalen       ()  { return this->variant.C_false.lf_fixed_datalen_F; };
    tsp00_Uint1        &lf_frac                ()  { return this->variant.C_false.lf_frac_F; };
    /* const accessor functions */
    const tsp00_Int2   &lf_datalen             () const  { return this->variant.C_true.lf_datalen_F; };
    const tsp00_Uint1  &lf_fixed_datalen       () const  { return this->variant.C_false.lf_fixed_datalen_F; };
    const tsp00_Uint1  &lf_frac                () const  { return this->variant.C_false.lf_frac_F; };
#endif    /* defined (__cplusplus) */
} tak_len_and_frac;

typedef struct                     tak_field_pars
{
    tak_fp_kind_type    fp_kind;
    tgg00_StackEntryType fp_etype;
    tsp00_DataType      fp_datatyp;
    tak_fcolpropset     fp_colset;
    union
    {
    /* fp_catalog_column */
        struct tak_field_pars_fp_catalog_column
        {
            tsp00_Int4          fp_frompos_v1_F;
            tsp00_Int2          fp_movebefore_v1_F;
            tsp00_Int2          fp_rangeextcolno_F;
            tak_len_and_frac    fp_len_and_frac_v1_F;
            tsp00_Int2          fp_inoutlen_v1_F;
        } C_fp_catalog_column;
    /* fp_val_all_without_l */
        struct tak_field_pars_fp_val_all_without_l
        {
            tsp00_Int4          fp_frompos_v2_F;
            tsp00_Int2          fp_movebefore_v2_F;
            tsp00_Int2          fp_stackpos_F;
            tak_len_and_frac    fp_len_and_frac_v2_F;
            tsp00_Int2          fp_inoutlen_v2_F;
        } C_fp_val_all_without_l;
    /* fp_lock_val */
        struct tak_field_pars_fp_lock_val
        {
            tsp00_Int4          fp_frompos_v3_F;
            tsp00_Int2          fp_movebefore_v3_F;
            tsp00_Int2          fp_fill_31_F;
            tak_len_and_frac    fp_len_and_frac_v3_F;
            tsp00_Int2          fp_inoutlen_v3_F;
        } C_fp_lock_val;
    /* fp_last_lock */
        struct tak_field_pars_fp_last_lock
        {
            tsp00_Int4          fp_fill_41_F;
            tsp00_Int2          fp_movebefore_v4_F;
            tsp00_Int2          fp_fill_42_F;
            tsp00_Int4          fp_fill_43_F;
        } C_fp_last_lock;
    /* fp_current_of_key */
        struct tak_field_pars_fp_current_of_key
        {
            tsp00_Int2          fp_curr_keylen_F;
            tsp00_Int2          fp_startpos_F;
            tsp00_Int2          fp_movebefore_v5_F;
            tsp00_Int2          fp_fill_51_F;
            tsp00_Int4          fp_pos_no_F;
        } C_fp_current_of_key;
    /* fp_val_from_res_buffer */
        struct tak_field_pars_fp_val_from_res_buffer
        {
            tsp00_Int2          fp_res_buf_index_F;
            tak_fp_kind_type    fp_glob_fp_kind_F;
            pasbool             fp_fill_61_F;
            tsp00_Int2          fp_movebefore_v6_F;
            tsp00_Int2          fp_rangeextcolno_v6_F;
            tak_len_and_frac    fp_len_and_frac_v6_F;
            tsp00_Int2          fp_inoutlen_v6_F;
        } C_fp_val_from_res_buffer;
    /* fp_fill_part2_to_len */
        struct tak_field_pars_fp_fill_part2_to_len
        {
            tsp00_Int4          fp_fill_71_F;
            tsp00_Int2          fp_fill_72_F;
            tsp00_Int2          fp_fill_73_F;
            tsp00_Int4          fp_len_F;
        } C_fp_fill_part2_to_len;
    /* fp_ordered_select_key */
        struct tak_field_pars_fp_ordered_select_key
        {
            tsp00_Int4          fp_fill_81_F;
            tsp00_Int2          fp_movebefore_v8_F;
            tsp00_Int2          fp_keystack_F;
            tsp00_Int2          fp_to_stack_F;
            tsp00_Int2          fp_key_low_handl_F;
        } C_fp_ordered_select_key;
    /* fp_escape_val */
        struct tak_field_pars_fp_escape_val
        {
            tsp00_Int4          fp_fill_101_F;
            tsp00_Int4          fp_fill_102_F;
            tsp00_Int2          fp_fill_103_F;
            tsp00_C2            fp_escape_char_F;
        } C_fp_escape_val;
    /* fp_pseudo_ins_select */
        struct tak_field_pars_fp_pseudo_ins_select
        {
            tsp00_Int4          fp_fill_111_F;
            tsp00_Int2          fp_movebeforev11_F;
            tsp00_Int2          fp_fill_112_F;
            tsp00_Int2          fp_reccolno_F;
            tsp00_Int2          fp_fill_113_F;
        } C_fp_pseudo_ins_select;
    /* fp_result_name */
        struct tak_field_pars_fp_result_name
        {
            tsp00_Int4          fp_fromposv14_F;
            tsp00_Int4          fp_fill_141_F;
            tsp00_Int2          fp_datalenv14_F;
            tsp00_Int2          fp_dataiolenv14_F;
        } C_fp_result_name;
    /* fp_verify_not_null */
        struct tak_field_pars_fp_verify_not_null
        {
            tgg00_StackEntry    fp_stackentry_F;
            tsp00_Int4          fp_fill_151_F;
        } C_fp_verify_not_null;
    /* fp_get_name_value */
        struct tak_field_pars_fp_get_name_value
        {
            tsp00_Int4          fp_frompos_v15_F;
            tsp00_Int2          fp_movebefore_v15_F;
            tak_sc_symbol       fp_sc_symbol_F;
            tsp00_C1            fp_objecttype_F;
            tak_len_and_frac    fp_len_and_frac_v15_F;
            tsp00_Int2          fp_inoutlen_v15_F;
        } C_fp_get_name_value;
    /* fp_fieldlist */
        struct tak_field_pars_fp_fieldlist
        {
            tsp00_Int4          fp_frompos_v16_F;
            tsp00_Int2          fp_interncolno_F;
            tsp00_Int2          fp_rangeextcolno_v16_F;
            tak_len_and_frac    fp_len_and_frac_v16_F;
            tsp00_Int2          fp_inoutlen_v16_F;
        } C_fp_fieldlist;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tsp00_Int4         &fp_frompos_v1          ()  { return this->variant.C_fp_catalog_column.fp_frompos_v1_F; };
    tsp00_Int2         &fp_movebefore_v1       ()  { return this->variant.C_fp_catalog_column.fp_movebefore_v1_F; };
    tsp00_Int2         &fp_rangeextcolno       ()  { return this->variant.C_fp_catalog_column.fp_rangeextcolno_F; };
    tak_len_and_frac   &fp_len_and_frac_v1     ()  { return this->variant.C_fp_catalog_column.fp_len_and_frac_v1_F; };
    tsp00_Int2         &fp_inoutlen_v1         ()  { return this->variant.C_fp_catalog_column.fp_inoutlen_v1_F; };
    tsp00_Int4         &fp_frompos_v2          ()  { return this->variant.C_fp_val_all_without_l.fp_frompos_v2_F; };
    tsp00_Int2         &fp_movebefore_v2       ()  { return this->variant.C_fp_val_all_without_l.fp_movebefore_v2_F; };
    tsp00_Int2         &fp_stackpos            ()  { return this->variant.C_fp_val_all_without_l.fp_stackpos_F; };
    tak_len_and_frac   &fp_len_and_frac_v2     ()  { return this->variant.C_fp_val_all_without_l.fp_len_and_frac_v2_F; };
    tsp00_Int2         &fp_inoutlen_v2         ()  { return this->variant.C_fp_val_all_without_l.fp_inoutlen_v2_F; };
    tsp00_Int4         &fp_frompos_v3          ()  { return this->variant.C_fp_lock_val.fp_frompos_v3_F; };
    tsp00_Int2         &fp_movebefore_v3       ()  { return this->variant.C_fp_lock_val.fp_movebefore_v3_F; };
    tsp00_Int2         &fp_fill_31             ()  { return this->variant.C_fp_lock_val.fp_fill_31_F; };
    tak_len_and_frac   &fp_len_and_frac_v3     ()  { return this->variant.C_fp_lock_val.fp_len_and_frac_v3_F; };
    tsp00_Int2         &fp_inoutlen_v3         ()  { return this->variant.C_fp_lock_val.fp_inoutlen_v3_F; };
    tsp00_Int4         &fp_fill_41             ()  { return this->variant.C_fp_last_lock.fp_fill_41_F; };
    tsp00_Int2         &fp_movebefore_v4       ()  { return this->variant.C_fp_last_lock.fp_movebefore_v4_F; };
    tsp00_Int2         &fp_fill_42             ()  { return this->variant.C_fp_last_lock.fp_fill_42_F; };
    tsp00_Int4         &fp_fill_43             ()  { return this->variant.C_fp_last_lock.fp_fill_43_F; };
    tsp00_Int2         &fp_curr_keylen         ()  { return this->variant.C_fp_current_of_key.fp_curr_keylen_F; };
    tsp00_Int2         &fp_startpos            ()  { return this->variant.C_fp_current_of_key.fp_startpos_F; };
    tsp00_Int2         &fp_movebefore_v5       ()  { return this->variant.C_fp_current_of_key.fp_movebefore_v5_F; };
    tsp00_Int2         &fp_fill_51             ()  { return this->variant.C_fp_current_of_key.fp_fill_51_F; };
    tsp00_Int4         &fp_pos_no              ()  { return this->variant.C_fp_current_of_key.fp_pos_no_F; };
    tsp00_Int2         &fp_res_buf_index       ()  { return this->variant.C_fp_val_from_res_buffer.fp_res_buf_index_F; };
    tak_fp_kind_type   &fp_glob_fp_kind        ()  { return this->variant.C_fp_val_from_res_buffer.fp_glob_fp_kind_F; };
    pasbool            &fp_fill_61             ()  { return this->variant.C_fp_val_from_res_buffer.fp_fill_61_F; };
    tsp00_Int2         &fp_movebefore_v6       ()  { return this->variant.C_fp_val_from_res_buffer.fp_movebefore_v6_F; };
    tsp00_Int2         &fp_rangeextcolno_v6    ()  { return this->variant.C_fp_val_from_res_buffer.fp_rangeextcolno_v6_F; };
    tak_len_and_frac   &fp_len_and_frac_v6     ()  { return this->variant.C_fp_val_from_res_buffer.fp_len_and_frac_v6_F; };
    tsp00_Int2         &fp_inoutlen_v6         ()  { return this->variant.C_fp_val_from_res_buffer.fp_inoutlen_v6_F; };
    tsp00_Int4         &fp_fill_71             ()  { return this->variant.C_fp_fill_part2_to_len.fp_fill_71_F; };
    tsp00_Int2         &fp_fill_72             ()  { return this->variant.C_fp_fill_part2_to_len.fp_fill_72_F; };
    tsp00_Int2         &fp_fill_73             ()  { return this->variant.C_fp_fill_part2_to_len.fp_fill_73_F; };
    tsp00_Int4         &fp_len                 ()  { return this->variant.C_fp_fill_part2_to_len.fp_len_F; };
    tsp00_Int4         &fp_fill_81             ()  { return this->variant.C_fp_ordered_select_key.fp_fill_81_F; };
    tsp00_Int2         &fp_movebefore_v8       ()  { return this->variant.C_fp_ordered_select_key.fp_movebefore_v8_F; };
    tsp00_Int2         &fp_keystack            ()  { return this->variant.C_fp_ordered_select_key.fp_keystack_F; };
    tsp00_Int2         &fp_to_stack            ()  { return this->variant.C_fp_ordered_select_key.fp_to_stack_F; };
    tsp00_Int2         &fp_key_low_handl       ()  { return this->variant.C_fp_ordered_select_key.fp_key_low_handl_F; };
    tsp00_Int4         &fp_fill_101            ()  { return this->variant.C_fp_escape_val.fp_fill_101_F; };
    tsp00_Int4         &fp_fill_102            ()  { return this->variant.C_fp_escape_val.fp_fill_102_F; };
    tsp00_Int2         &fp_fill_103            ()  { return this->variant.C_fp_escape_val.fp_fill_103_F; };
    tsp00_C2           &fp_escape_char         ()  { return this->variant.C_fp_escape_val.fp_escape_char_F; };
    tsp00_Int4         &fp_fill_111            ()  { return this->variant.C_fp_pseudo_ins_select.fp_fill_111_F; };
    tsp00_Int2         &fp_movebeforev11       ()  { return this->variant.C_fp_pseudo_ins_select.fp_movebeforev11_F; };
    tsp00_Int2         &fp_fill_112            ()  { return this->variant.C_fp_pseudo_ins_select.fp_fill_112_F; };
    tsp00_Int2         &fp_reccolno            ()  { return this->variant.C_fp_pseudo_ins_select.fp_reccolno_F; };
    tsp00_Int2         &fp_fill_113            ()  { return this->variant.C_fp_pseudo_ins_select.fp_fill_113_F; };
    tsp00_Int4         &fp_fromposv14          ()  { return this->variant.C_fp_result_name.fp_fromposv14_F; };
    tsp00_Int4         &fp_fill_141            ()  { return this->variant.C_fp_result_name.fp_fill_141_F; };
    tsp00_Int2         &fp_datalenv14          ()  { return this->variant.C_fp_result_name.fp_datalenv14_F; };
    tsp00_Int2         &fp_dataiolenv14        ()  { return this->variant.C_fp_result_name.fp_dataiolenv14_F; };
    tgg00_StackEntry   &fp_stackentry          ()  { return this->variant.C_fp_verify_not_null.fp_stackentry_F; };
    tsp00_Int4         &fp_fill_151            ()  { return this->variant.C_fp_verify_not_null.fp_fill_151_F; };
    tsp00_Int4         &fp_frompos_v15         ()  { return this->variant.C_fp_get_name_value.fp_frompos_v15_F; };
    tsp00_Int2         &fp_movebefore_v15      ()  { return this->variant.C_fp_get_name_value.fp_movebefore_v15_F; };
    tak_sc_symbol      &fp_sc_symbol           ()  { return this->variant.C_fp_get_name_value.fp_sc_symbol_F; };
    tsp00_C1           &fp_objecttype          ()  { return this->variant.C_fp_get_name_value.fp_objecttype_F; };
    tak_len_and_frac   &fp_len_and_frac_v15    ()  { return this->variant.C_fp_get_name_value.fp_len_and_frac_v15_F; };
    tsp00_Int2         &fp_inoutlen_v15        ()  { return this->variant.C_fp_get_name_value.fp_inoutlen_v15_F; };
    tsp00_Int4         &fp_frompos_v16         ()  { return this->variant.C_fp_fieldlist.fp_frompos_v16_F; };
    tsp00_Int2         &fp_interncolno         ()  { return this->variant.C_fp_fieldlist.fp_interncolno_F; };
    tsp00_Int2         &fp_rangeextcolno_v16   ()  { return this->variant.C_fp_fieldlist.fp_rangeextcolno_v16_F; };
    tak_len_and_frac   &fp_len_and_frac_v16    ()  { return this->variant.C_fp_fieldlist.fp_len_and_frac_v16_F; };
    tsp00_Int2         &fp_inoutlen_v16        ()  { return this->variant.C_fp_fieldlist.fp_inoutlen_v16_F; };
    /* const accessor functions */
    const tsp00_Int4   &fp_frompos_v1          () const  { return this->variant.C_fp_catalog_column.fp_frompos_v1_F; };
    const tsp00_Int2   &fp_movebefore_v1       () const  { return this->variant.C_fp_catalog_column.fp_movebefore_v1_F; };
    const tsp00_Int2   &fp_rangeextcolno       () const  { return this->variant.C_fp_catalog_column.fp_rangeextcolno_F; };
    const tak_len_and_frac &fp_len_and_frac_v1     () const  { return this->variant.C_fp_catalog_column.fp_len_and_frac_v1_F; };
    const tsp00_Int2   &fp_inoutlen_v1         () const  { return this->variant.C_fp_catalog_column.fp_inoutlen_v1_F; };
    const tsp00_Int4   &fp_frompos_v2          () const  { return this->variant.C_fp_val_all_without_l.fp_frompos_v2_F; };
    const tsp00_Int2   &fp_movebefore_v2       () const  { return this->variant.C_fp_val_all_without_l.fp_movebefore_v2_F; };
    const tsp00_Int2   &fp_stackpos            () const  { return this->variant.C_fp_val_all_without_l.fp_stackpos_F; };
    const tak_len_and_frac &fp_len_and_frac_v2     () const  { return this->variant.C_fp_val_all_without_l.fp_len_and_frac_v2_F; };
    const tsp00_Int2   &fp_inoutlen_v2         () const  { return this->variant.C_fp_val_all_without_l.fp_inoutlen_v2_F; };
    const tsp00_Int4   &fp_frompos_v3          () const  { return this->variant.C_fp_lock_val.fp_frompos_v3_F; };
    const tsp00_Int2   &fp_movebefore_v3       () const  { return this->variant.C_fp_lock_val.fp_movebefore_v3_F; };
    const tsp00_Int2   &fp_fill_31             () const  { return this->variant.C_fp_lock_val.fp_fill_31_F; };
    const tak_len_and_frac &fp_len_and_frac_v3     () const  { return this->variant.C_fp_lock_val.fp_len_and_frac_v3_F; };
    const tsp00_Int2   &fp_inoutlen_v3         () const  { return this->variant.C_fp_lock_val.fp_inoutlen_v3_F; };
    const tsp00_Int4   &fp_fill_41             () const  { return this->variant.C_fp_last_lock.fp_fill_41_F; };
    const tsp00_Int2   &fp_movebefore_v4       () const  { return this->variant.C_fp_last_lock.fp_movebefore_v4_F; };
    const tsp00_Int2   &fp_fill_42             () const  { return this->variant.C_fp_last_lock.fp_fill_42_F; };
    const tsp00_Int4   &fp_fill_43             () const  { return this->variant.C_fp_last_lock.fp_fill_43_F; };
    const tsp00_Int2   &fp_curr_keylen         () const  { return this->variant.C_fp_current_of_key.fp_curr_keylen_F; };
    const tsp00_Int2   &fp_startpos            () const  { return this->variant.C_fp_current_of_key.fp_startpos_F; };
    const tsp00_Int2   &fp_movebefore_v5       () const  { return this->variant.C_fp_current_of_key.fp_movebefore_v5_F; };
    const tsp00_Int2   &fp_fill_51             () const  { return this->variant.C_fp_current_of_key.fp_fill_51_F; };
    const tsp00_Int4   &fp_pos_no              () const  { return this->variant.C_fp_current_of_key.fp_pos_no_F; };
    const tsp00_Int2   &fp_res_buf_index       () const  { return this->variant.C_fp_val_from_res_buffer.fp_res_buf_index_F; };
    const tak_fp_kind_type &fp_glob_fp_kind        () const  { return this->variant.C_fp_val_from_res_buffer.fp_glob_fp_kind_F; };
    const pasbool      &fp_fill_61             () const  { return this->variant.C_fp_val_from_res_buffer.fp_fill_61_F; };
    const tsp00_Int2   &fp_movebefore_v6       () const  { return this->variant.C_fp_val_from_res_buffer.fp_movebefore_v6_F; };
    const tsp00_Int2   &fp_rangeextcolno_v6    () const  { return this->variant.C_fp_val_from_res_buffer.fp_rangeextcolno_v6_F; };
    const tak_len_and_frac &fp_len_and_frac_v6     () const  { return this->variant.C_fp_val_from_res_buffer.fp_len_and_frac_v6_F; };
    const tsp00_Int2   &fp_inoutlen_v6         () const  { return this->variant.C_fp_val_from_res_buffer.fp_inoutlen_v6_F; };
    const tsp00_Int4   &fp_fill_71             () const  { return this->variant.C_fp_fill_part2_to_len.fp_fill_71_F; };
    const tsp00_Int2   &fp_fill_72             () const  { return this->variant.C_fp_fill_part2_to_len.fp_fill_72_F; };
    const tsp00_Int2   &fp_fill_73             () const  { return this->variant.C_fp_fill_part2_to_len.fp_fill_73_F; };
    const tsp00_Int4   &fp_len                 () const  { return this->variant.C_fp_fill_part2_to_len.fp_len_F; };
    const tsp00_Int4   &fp_fill_81             () const  { return this->variant.C_fp_ordered_select_key.fp_fill_81_F; };
    const tsp00_Int2   &fp_movebefore_v8       () const  { return this->variant.C_fp_ordered_select_key.fp_movebefore_v8_F; };
    const tsp00_Int2   &fp_keystack            () const  { return this->variant.C_fp_ordered_select_key.fp_keystack_F; };
    const tsp00_Int2   &fp_to_stack            () const  { return this->variant.C_fp_ordered_select_key.fp_to_stack_F; };
    const tsp00_Int2   &fp_key_low_handl       () const  { return this->variant.C_fp_ordered_select_key.fp_key_low_handl_F; };
    const tsp00_Int4   &fp_fill_101            () const  { return this->variant.C_fp_escape_val.fp_fill_101_F; };
    const tsp00_Int4   &fp_fill_102            () const  { return this->variant.C_fp_escape_val.fp_fill_102_F; };
    const tsp00_Int2   &fp_fill_103            () const  { return this->variant.C_fp_escape_val.fp_fill_103_F; };
    const tsp00_C2     &fp_escape_char         () const  { return this->variant.C_fp_escape_val.fp_escape_char_F; };
    const tsp00_Int4   &fp_fill_111            () const  { return this->variant.C_fp_pseudo_ins_select.fp_fill_111_F; };
    const tsp00_Int2   &fp_movebeforev11       () const  { return this->variant.C_fp_pseudo_ins_select.fp_movebeforev11_F; };
    const tsp00_Int2   &fp_fill_112            () const  { return this->variant.C_fp_pseudo_ins_select.fp_fill_112_F; };
    const tsp00_Int2   &fp_reccolno            () const  { return this->variant.C_fp_pseudo_ins_select.fp_reccolno_F; };
    const tsp00_Int2   &fp_fill_113            () const  { return this->variant.C_fp_pseudo_ins_select.fp_fill_113_F; };
    const tsp00_Int4   &fp_fromposv14          () const  { return this->variant.C_fp_result_name.fp_fromposv14_F; };
    const tsp00_Int4   &fp_fill_141            () const  { return this->variant.C_fp_result_name.fp_fill_141_F; };
    const tsp00_Int2   &fp_datalenv14          () const  { return this->variant.C_fp_result_name.fp_datalenv14_F; };
    const tsp00_Int2   &fp_dataiolenv14        () const  { return this->variant.C_fp_result_name.fp_dataiolenv14_F; };
    const tgg00_StackEntry &fp_stackentry          () const  { return this->variant.C_fp_verify_not_null.fp_stackentry_F; };
    const tsp00_Int4   &fp_fill_151            () const  { return this->variant.C_fp_verify_not_null.fp_fill_151_F; };
    const tsp00_Int4   &fp_frompos_v15         () const  { return this->variant.C_fp_get_name_value.fp_frompos_v15_F; };
    const tsp00_Int2   &fp_movebefore_v15      () const  { return this->variant.C_fp_get_name_value.fp_movebefore_v15_F; };
    const tak_sc_symbol &fp_sc_symbol           () const  { return this->variant.C_fp_get_name_value.fp_sc_symbol_F; };
    const tsp00_C1     &fp_objecttype          () const  { return this->variant.C_fp_get_name_value.fp_objecttype_F; };
    const tak_len_and_frac &fp_len_and_frac_v15    () const  { return this->variant.C_fp_get_name_value.fp_len_and_frac_v15_F; };
    const tsp00_Int2   &fp_inoutlen_v15        () const  { return this->variant.C_fp_get_name_value.fp_inoutlen_v15_F; };
    const tsp00_Int4   &fp_frompos_v16         () const  { return this->variant.C_fp_fieldlist.fp_frompos_v16_F; };
    const tsp00_Int2   &fp_interncolno         () const  { return this->variant.C_fp_fieldlist.fp_interncolno_F; };
    const tsp00_Int2   &fp_rangeextcolno_v16   () const  { return this->variant.C_fp_fieldlist.fp_rangeextcolno_v16_F; };
    const tak_len_and_frac &fp_len_and_frac_v16    () const  { return this->variant.C_fp_fieldlist.fp_len_and_frac_v16_F; };
    const tsp00_Int2   &fp_inoutlen_v16        () const  { return this->variant.C_fp_fieldlist.fp_inoutlen_v16_F; };
#endif    /* defined (__cplusplus) */
} tak_field_pars;

typedef struct                     tak_ak_all_locks
{
    tsp00_Int2          all_lock_count;
    tsp00_Int2          all_lock_fill;
    ARRAY_VAR_MSP00 (         all_locks, tgg00_Lock, MAX_QUALBUF_LOCKS_GG00);
} tak_ak_all_locks;
typedef ARRAY_TYPE_MSP00 (         tak_parser_fields, tak_field_pars, cak_maxparsfields);

typedef struct                     tak_selfref_link_info
{
    tsp00_Int2          sf_index;
    tsp00_Int2          sf_parsno;
    tsp00_Int2          sf_use_id;
    tsp00_Int2          sf_fill;
} tak_selfref_link_info;

typedef enum tak_parsinfo_states__Enum
{
    pi_changeinfos_exist,
    pi_hint_exist,
    pu_unused3,
    pi_unused4,
    pi_unused5,
    pi_unused6,
    pi_unused7,
    pi_unused8
} tak_parsinfo_states__Enum;
typedef tak_parsinfo_states__Enum tak_parsinfo_states__Param;
typedef SMALLSET_TYPE_MSP00 (      tak_parsinfo_states, tak_parsinfo_states__Enum, pi_unused8);

typedef struct                     tak_parsinforecord
{
    union
    {
    /* 1 */
        struct tak_parsinforecord_1
        {
            tsp00_Int2          p_reclen_F;
            tsp00_Int2          p_keylen_F;
            tsp00_Int2          p_varcol_offset_F;
            tsp00_Int2          p_varcol_cnt_F;
            tgg00_Surrogate     p_surrogate_F;
            tsp00_C2            p_entrytype_F;
            tsp00_Int2          p_segno_F;
            tsp00_Int4          p_fullen_F;
            tsp00_Int4          p_filler_F;
            tak_parsbuffer      p_buf_F;
        } C_1;
    /* 2 */
        struct tak_parsinforecord_2
        {
            tsp00_Int2          p1reclen_F;
            tsp00_Int2          p1keylen_F;
            tsp00_Int2          p1varcol_offset_F;
            tsp00_Int2          p1varcol_cnt_F;
            tgg00_Surrogate     p1surrogate_F;
            tsp00_C2            p1entrytype_F;
            tsp00_Int2          p1segno_F;
            tsp00_Int4          p1_fullen_F;
            tsp00_Int4          p1_filler_F;
            tgg00_FileId        p_treeid_F;
            tsp00_Int4          p_pagecnt_F;
            tsp00_KnlIdentifier p_resn_F;
            tsp00_KnlIdentifier p_modul_name_F;
            tsp00_KnlIdentifier p_searchname_F;
            tgg00_MessType      p_mtyp_F;
            tgg00_MessType2     p_mtyp2_F;
            pasbool             p_select_F;
            pasbool             p_single_F;
            tgg00_Surrogate     p_tabid_F;
            tsp00_Int2          p_int_res_cnt_F;
            tsp00_Int2          p_joinviewtabcnt_F;
            tsp00_Int2          p_p_no_F;
            tsp00_Int2          p_subcntlevel_F;
            tsp00_Int4          p_posno_F;
            tgg00_RefInfoIndex  p_statement_kind_F;
            tak_parsinfo_states p_bool_states_F;
            pasbool             p_filler1_F;
            tsp00_Uint1         p_precomp_info_byte_F;
            pasbool             p_reuse_F;
            pasbool             p_prepare_F;
            tsp00_Int2          p_function_code_F;
            tsp00_SqlMode       p_sqlmode_F;
            pasbool             p_own_trans_F;
            tgg00_HandlingSet   p_handling_set_F;
            tsp00_Uint1         p_session_isolevel_F;
            pasbool             p_use_sess_isolevel_F;
            tsp00_Int2          p_resreclen_F;
            tsp00_Int2          p_resinfolen_F;
            tsp00_Int2          p_j_v_keylen_F;
            pasbool             p_longliteral_F;
            pasbool             p_parsing_again_F;
            tsp00_Int2          p_cmd_id_F;
            tsp00_Int2          p_max_infos_F;
            tsp00_Int2          p_cnt_infos_F;
            tak_parser_fields   p_pars_infos_F;
        } C_2;
    /* 4 */
        struct tak_parsinforecord_4
        {
            tsp00_Int2          p3reclen_F;
            tsp00_Int2          p3keylen_F;
            tsp00_Int2          p3varcol_offset_F;
            tsp00_Int2          p3varcol_cnt_F;
            tgg00_Surrogate     p3surrogate_F;
            tsp00_C2            p3entrytype_F;
            tsp00_Int2          p3segno_F;
            OLDPASCALSTRING_VAR_MSP00 (  p_tl_filler1_F, mxak_pars_header);
            tsp00_Int4          p_temp_lock_atch_F;
            tak_ak_all_locks    p_temp_lock_F;
        } C_4;
    /* 5 */
        struct tak_parsinforecord_5
        {
            OLDPASCALSTRING_VAR_MSP00 (  p_sysbufferoffset_F, cak_sysbufferoffset);
            OLDPASCALSTRING_VAR_MSP00 (  p_pars_header_F, mxak_pars_header);
            tak_paraminfoarr    p_short_info_F;
        } C_5;
    /* 7 */
        struct tak_parsinforecord_7
        {
            tsp00_Int2          p7reclen_F;
            tsp00_Int2          p7keylen_F;
            tsp00_Int2          p7varcol_offset_F;
            tsp00_Int2          p7varcol_cnt_F;
            tgg00_Surrogate     p7surrogate_F;
            tsp00_C2            p7entrytype_F;
            tsp00_Int2          p7segno_F;
            OLDPASCALSTRING_VAR_MSP00 (  p_tl_filler7_F, mxak_pars_header);
            tgg00_HandlingSet   p_handling_F;
            pasbool             p_fetch_object_F;
            pasbool             p7fill_F;
        } C_7;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tsp00_Int2         &p_reclen               ()  { return this->variant.C_1.p_reclen_F; };
    tsp00_Int2         &p_keylen               ()  { return this->variant.C_1.p_keylen_F; };
    tsp00_Int2         &p_varcol_offset        ()  { return this->variant.C_1.p_varcol_offset_F; };
    tsp00_Int2         &p_varcol_cnt           ()  { return this->variant.C_1.p_varcol_cnt_F; };
    tgg00_Surrogate    &p_surrogate            ()  { return this->variant.C_1.p_surrogate_F; };
    tsp00_C2           &p_entrytype            ()  { return this->variant.C_1.p_entrytype_F; };
    tsp00_Int2         &p_segno                ()  { return this->variant.C_1.p_segno_F; };
    tsp00_Int4         &p_fullen               ()  { return this->variant.C_1.p_fullen_F; };
    tsp00_Int4         &p_filler               ()  { return this->variant.C_1.p_filler_F; };
    tak_parsbuffer     &p_buf                  ()  { return this->variant.C_1.p_buf_F; };
    tsp00_Int2         &p1reclen               ()  { return this->variant.C_2.p1reclen_F; };
    tsp00_Int2         &p1keylen               ()  { return this->variant.C_2.p1keylen_F; };
    tsp00_Int2         &p1varcol_offset        ()  { return this->variant.C_2.p1varcol_offset_F; };
    tsp00_Int2         &p1varcol_cnt           ()  { return this->variant.C_2.p1varcol_cnt_F; };
    tgg00_Surrogate    &p1surrogate            ()  { return this->variant.C_2.p1surrogate_F; };
    tsp00_C2           &p1entrytype            ()  { return this->variant.C_2.p1entrytype_F; };
    tsp00_Int2         &p1segno                ()  { return this->variant.C_2.p1segno_F; };
    tsp00_Int4         &p1_fullen              ()  { return this->variant.C_2.p1_fullen_F; };
    tsp00_Int4         &p1_filler              ()  { return this->variant.C_2.p1_filler_F; };
    tgg00_FileId       &p_treeid               ()  { return this->variant.C_2.p_treeid_F; };
    tsp00_Int4         &p_pagecnt              ()  { return this->variant.C_2.p_pagecnt_F; };
    tsp00_KnlIdentifier &p_resn                 ()  { return this->variant.C_2.p_resn_F; };
    tsp00_KnlIdentifier &p_modul_name           ()  { return this->variant.C_2.p_modul_name_F; };
    tsp00_KnlIdentifier &p_searchname           ()  { return this->variant.C_2.p_searchname_F; };
    tgg00_MessType     &p_mtyp                 ()  { return this->variant.C_2.p_mtyp_F; };
    tgg00_MessType2    &p_mtyp2                ()  { return this->variant.C_2.p_mtyp2_F; };
    pasbool            &p_select               ()  { return this->variant.C_2.p_select_F; };
    pasbool            &p_single               ()  { return this->variant.C_2.p_single_F; };
    tgg00_Surrogate    &p_tabid                ()  { return this->variant.C_2.p_tabid_F; };
    tsp00_Int2         &p_int_res_cnt          ()  { return this->variant.C_2.p_int_res_cnt_F; };
    tsp00_Int2         &p_joinviewtabcnt       ()  { return this->variant.C_2.p_joinviewtabcnt_F; };
    tsp00_Int2         &p_p_no                 ()  { return this->variant.C_2.p_p_no_F; };
    tsp00_Int2         &p_subcntlevel          ()  { return this->variant.C_2.p_subcntlevel_F; };
    tsp00_Int4         &p_posno                ()  { return this->variant.C_2.p_posno_F; };
    tgg00_RefInfoIndex &p_statement_kind       ()  { return this->variant.C_2.p_statement_kind_F; };
    tak_parsinfo_states &p_bool_states          ()  { return this->variant.C_2.p_bool_states_F; };
    pasbool            &p_filler1              ()  { return this->variant.C_2.p_filler1_F; };
    tsp00_Uint1        &p_precomp_info_byte    ()  { return this->variant.C_2.p_precomp_info_byte_F; };
    pasbool            &p_reuse                ()  { return this->variant.C_2.p_reuse_F; };
    pasbool            &p_prepare              ()  { return this->variant.C_2.p_prepare_F; };
    tsp00_Int2         &p_function_code        ()  { return this->variant.C_2.p_function_code_F; };
    tsp00_SqlMode      &p_sqlmode              ()  { return this->variant.C_2.p_sqlmode_F; };
    pasbool            &p_own_trans            ()  { return this->variant.C_2.p_own_trans_F; };
    tgg00_HandlingSet  &p_handling_set         ()  { return this->variant.C_2.p_handling_set_F; };
    tsp00_Uint1        &p_session_isolevel     ()  { return this->variant.C_2.p_session_isolevel_F; };
    pasbool            &p_use_sess_isolevel    ()  { return this->variant.C_2.p_use_sess_isolevel_F; };
    tsp00_Int2         &p_resreclen            ()  { return this->variant.C_2.p_resreclen_F; };
    tsp00_Int2         &p_resinfolen           ()  { return this->variant.C_2.p_resinfolen_F; };
    tsp00_Int2         &p_j_v_keylen           ()  { return this->variant.C_2.p_j_v_keylen_F; };
    pasbool            &p_longliteral          ()  { return this->variant.C_2.p_longliteral_F; };
    pasbool            &p_parsing_again        ()  { return this->variant.C_2.p_parsing_again_F; };
    tsp00_Int2         &p_cmd_id               ()  { return this->variant.C_2.p_cmd_id_F; };
    tsp00_Int2         &p_max_infos            ()  { return this->variant.C_2.p_max_infos_F; };
    tsp00_Int2         &p_cnt_infos            ()  { return this->variant.C_2.p_cnt_infos_F; };
    tak_parser_fields  &p_pars_infos           ()  { return this->variant.C_2.p_pars_infos_F; };
    tsp00_Int2         &p3reclen               ()  { return this->variant.C_4.p3reclen_F; };
    tsp00_Int2         &p3keylen               ()  { return this->variant.C_4.p3keylen_F; };
    tsp00_Int2         &p3varcol_offset        ()  { return this->variant.C_4.p3varcol_offset_F; };
    tsp00_Int2         &p3varcol_cnt           ()  { return this->variant.C_4.p3varcol_cnt_F; };
    tgg00_Surrogate    &p3surrogate            ()  { return this->variant.C_4.p3surrogate_F; };
    tsp00_C2           &p3entrytype            ()  { return this->variant.C_4.p3entrytype_F; };
    tsp00_Int2         &p3segno                ()  { return this->variant.C_4.p3segno_F; };
    tsp00_OldPascalString<mxak_pars_header> &p_tl_filler1           ()  { return this->variant.C_4.p_tl_filler1_F; };
    tsp00_Int4         &p_temp_lock_atch       ()  { return this->variant.C_4.p_temp_lock_atch_F; };
    tak_ak_all_locks   &p_temp_lock            ()  { return this->variant.C_4.p_temp_lock_F; };
    tsp00_OldPascalString<cak_sysbufferoffset> &p_sysbufferoffset      ()  { return this->variant.C_5.p_sysbufferoffset_F; };
    tsp00_OldPascalString<mxak_pars_header> &p_pars_header          ()  { return this->variant.C_5.p_pars_header_F; };
    tak_paraminfoarr   &p_short_info           ()  { return this->variant.C_5.p_short_info_F; };
    tsp00_Int2         &p7reclen               ()  { return this->variant.C_7.p7reclen_F; };
    tsp00_Int2         &p7keylen               ()  { return this->variant.C_7.p7keylen_F; };
    tsp00_Int2         &p7varcol_offset        ()  { return this->variant.C_7.p7varcol_offset_F; };
    tsp00_Int2         &p7varcol_cnt           ()  { return this->variant.C_7.p7varcol_cnt_F; };
    tgg00_Surrogate    &p7surrogate            ()  { return this->variant.C_7.p7surrogate_F; };
    tsp00_C2           &p7entrytype            ()  { return this->variant.C_7.p7entrytype_F; };
    tsp00_Int2         &p7segno                ()  { return this->variant.C_7.p7segno_F; };
    tsp00_OldPascalString<mxak_pars_header> &p_tl_filler7           ()  { return this->variant.C_7.p_tl_filler7_F; };
    tgg00_HandlingSet  &p_handling             ()  { return this->variant.C_7.p_handling_F; };
    pasbool            &p_fetch_object         ()  { return this->variant.C_7.p_fetch_object_F; };
    pasbool            &p7fill                 ()  { return this->variant.C_7.p7fill_F; };
    /* const accessor functions */
    const tsp00_Int2   &p_reclen               () const  { return this->variant.C_1.p_reclen_F; };
    const tsp00_Int2   &p_keylen               () const  { return this->variant.C_1.p_keylen_F; };
    const tsp00_Int2   &p_varcol_offset        () const  { return this->variant.C_1.p_varcol_offset_F; };
    const tsp00_Int2   &p_varcol_cnt           () const  { return this->variant.C_1.p_varcol_cnt_F; };
    const tgg00_Surrogate &p_surrogate            () const  { return this->variant.C_1.p_surrogate_F; };
    const tsp00_C2     &p_entrytype            () const  { return this->variant.C_1.p_entrytype_F; };
    const tsp00_Int2   &p_segno                () const  { return this->variant.C_1.p_segno_F; };
    const tsp00_Int4   &p_fullen               () const  { return this->variant.C_1.p_fullen_F; };
    const tsp00_Int4   &p_filler               () const  { return this->variant.C_1.p_filler_F; };
    const tak_parsbuffer &p_buf                  () const  { return this->variant.C_1.p_buf_F; };
    const tsp00_Int2   &p1reclen               () const  { return this->variant.C_2.p1reclen_F; };
    const tsp00_Int2   &p1keylen               () const  { return this->variant.C_2.p1keylen_F; };
    const tsp00_Int2   &p1varcol_offset        () const  { return this->variant.C_2.p1varcol_offset_F; };
    const tsp00_Int2   &p1varcol_cnt           () const  { return this->variant.C_2.p1varcol_cnt_F; };
    const tgg00_Surrogate &p1surrogate            () const  { return this->variant.C_2.p1surrogate_F; };
    const tsp00_C2     &p1entrytype            () const  { return this->variant.C_2.p1entrytype_F; };
    const tsp00_Int2   &p1segno                () const  { return this->variant.C_2.p1segno_F; };
    const tsp00_Int4   &p1_fullen              () const  { return this->variant.C_2.p1_fullen_F; };
    const tsp00_Int4   &p1_filler              () const  { return this->variant.C_2.p1_filler_F; };
    const tgg00_FileId &p_treeid               () const  { return this->variant.C_2.p_treeid_F; };
    const tsp00_Int4   &p_pagecnt              () const  { return this->variant.C_2.p_pagecnt_F; };
    const tsp00_KnlIdentifier &p_resn                 () const  { return this->variant.C_2.p_resn_F; };
    const tsp00_KnlIdentifier &p_modul_name           () const  { return this->variant.C_2.p_modul_name_F; };
    const tsp00_KnlIdentifier &p_searchname           () const  { return this->variant.C_2.p_searchname_F; };
    const tgg00_MessType &p_mtyp                 () const  { return this->variant.C_2.p_mtyp_F; };
    const tgg00_MessType2 &p_mtyp2                () const  { return this->variant.C_2.p_mtyp2_F; };
    const pasbool      &p_select               () const  { return this->variant.C_2.p_select_F; };
    const pasbool      &p_single               () const  { return this->variant.C_2.p_single_F; };
    const tgg00_Surrogate &p_tabid                () const  { return this->variant.C_2.p_tabid_F; };
    const tsp00_Int2   &p_int_res_cnt          () const  { return this->variant.C_2.p_int_res_cnt_F; };
    const tsp00_Int2   &p_joinviewtabcnt       () const  { return this->variant.C_2.p_joinviewtabcnt_F; };
    const tsp00_Int2   &p_p_no                 () const  { return this->variant.C_2.p_p_no_F; };
    const tsp00_Int2   &p_subcntlevel          () const  { return this->variant.C_2.p_subcntlevel_F; };
    const tsp00_Int4   &p_posno                () const  { return this->variant.C_2.p_posno_F; };
    const tgg00_RefInfoIndex &p_statement_kind       () const  { return this->variant.C_2.p_statement_kind_F; };
    const tak_parsinfo_states &p_bool_states          () const  { return this->variant.C_2.p_bool_states_F; };
    const pasbool      &p_filler1              () const  { return this->variant.C_2.p_filler1_F; };
    const tsp00_Uint1  &p_precomp_info_byte    () const  { return this->variant.C_2.p_precomp_info_byte_F; };
    const pasbool      &p_reuse                () const  { return this->variant.C_2.p_reuse_F; };
    const pasbool      &p_prepare              () const  { return this->variant.C_2.p_prepare_F; };
    const tsp00_Int2   &p_function_code        () const  { return this->variant.C_2.p_function_code_F; };
    const tsp00_SqlMode &p_sqlmode              () const  { return this->variant.C_2.p_sqlmode_F; };
    const pasbool      &p_own_trans            () const  { return this->variant.C_2.p_own_trans_F; };
    const tgg00_HandlingSet &p_handling_set         () const  { return this->variant.C_2.p_handling_set_F; };
    const tsp00_Uint1  &p_session_isolevel     () const  { return this->variant.C_2.p_session_isolevel_F; };
    const pasbool      &p_use_sess_isolevel    () const  { return this->variant.C_2.p_use_sess_isolevel_F; };
    const tsp00_Int2   &p_resreclen            () const  { return this->variant.C_2.p_resreclen_F; };
    const tsp00_Int2   &p_resinfolen           () const  { return this->variant.C_2.p_resinfolen_F; };
    const tsp00_Int2   &p_j_v_keylen           () const  { return this->variant.C_2.p_j_v_keylen_F; };
    const pasbool      &p_longliteral          () const  { return this->variant.C_2.p_longliteral_F; };
    const pasbool      &p_parsing_again        () const  { return this->variant.C_2.p_parsing_again_F; };
    const tsp00_Int2   &p_cmd_id               () const  { return this->variant.C_2.p_cmd_id_F; };
    const tsp00_Int2   &p_max_infos            () const  { return this->variant.C_2.p_max_infos_F; };
    const tsp00_Int2   &p_cnt_infos            () const  { return this->variant.C_2.p_cnt_infos_F; };
    const tak_parser_fields &p_pars_infos           () const  { return this->variant.C_2.p_pars_infos_F; };
    const tsp00_Int2   &p3reclen               () const  { return this->variant.C_4.p3reclen_F; };
    const tsp00_Int2   &p3keylen               () const  { return this->variant.C_4.p3keylen_F; };
    const tsp00_Int2   &p3varcol_offset        () const  { return this->variant.C_4.p3varcol_offset_F; };
    const tsp00_Int2   &p3varcol_cnt           () const  { return this->variant.C_4.p3varcol_cnt_F; };
    const tgg00_Surrogate &p3surrogate            () const  { return this->variant.C_4.p3surrogate_F; };
    const tsp00_C2     &p3entrytype            () const  { return this->variant.C_4.p3entrytype_F; };
    const tsp00_Int2   &p3segno                () const  { return this->variant.C_4.p3segno_F; };
    const tsp00_OldPascalString<mxak_pars_header> &p_tl_filler1           () const  { return this->variant.C_4.p_tl_filler1_F; };
    const tsp00_Int4   &p_temp_lock_atch       () const  { return this->variant.C_4.p_temp_lock_atch_F; };
    const tak_ak_all_locks &p_temp_lock            () const  { return this->variant.C_4.p_temp_lock_F; };
    const tsp00_OldPascalString<cak_sysbufferoffset> &p_sysbufferoffset      () const  { return this->variant.C_5.p_sysbufferoffset_F; };
    const tsp00_OldPascalString<mxak_pars_header> &p_pars_header          () const  { return this->variant.C_5.p_pars_header_F; };
    const tak_paraminfoarr &p_short_info           () const  { return this->variant.C_5.p_short_info_F; };
    const tsp00_Int2   &p7reclen               () const  { return this->variant.C_7.p7reclen_F; };
    const tsp00_Int2   &p7keylen               () const  { return this->variant.C_7.p7keylen_F; };
    const tsp00_Int2   &p7varcol_offset        () const  { return this->variant.C_7.p7varcol_offset_F; };
    const tsp00_Int2   &p7varcol_cnt           () const  { return this->variant.C_7.p7varcol_cnt_F; };
    const tgg00_Surrogate &p7surrogate            () const  { return this->variant.C_7.p7surrogate_F; };
    const tsp00_C2     &p7entrytype            () const  { return this->variant.C_7.p7entrytype_F; };
    const tsp00_Int2   &p7segno                () const  { return this->variant.C_7.p7segno_F; };
    const tsp00_OldPascalString<mxak_pars_header> &p_tl_filler7           () const  { return this->variant.C_7.p_tl_filler7_F; };
    const tgg00_HandlingSet &p_handling             () const  { return this->variant.C_7.p_handling_F; };
    const pasbool      &p_fetch_object         () const  { return this->variant.C_7.p_fetch_object_F; };
    const pasbool      &p7fill                 () const  { return this->variant.C_7.p7fill_F; };
#endif    /* defined (__cplusplus) */
} tak_parsinforecord;
typedef OLDPASCALSTRING_TYPE_MSP00 (  tak_stored_cmd_buf, mxak_stored_cmd_buf);
typedef CSTRING_TYPE_MSP00      (  tak_stored_cmd_bufc, mxak_stored_cmd_buf);

typedef struct                     tak_stored_cmd_record
{
    tsp00_Int2          scr_reclen;
    tsp00_Int2          scr_keylen;
    tsp00_Int2          scr_varcol_offset;
    tsp00_Int2          scr_varcol_cnt;
    tgg00_Surrogate     scr_surrogate;
    tsp00_C2            scr_entrytype;
    tsp00_Int2          scr_segno;
    tsp00_Int4          scr_fullen;
    tsp00_Int2          scr_movelen;
    tak_ddl_descriptor  scr_ddl;
    tsp00_SqlMode       scr_sql_mode;
    tak_stored_cmd_buf  scr_buf;
} tak_stored_cmd_record;

typedef enum tak_union_kind_Enum
{
    uis_none,
    uis_union,
    uis_except,
    uis_intersect
} tak_union_kind_Enum;
typedef tak_union_kind_Enum tak_union_kind_Param;
typedef ENUM_TYPE_MSP00 (          tak_union_kind, tak_union_kind_Enum, uis_intersect, unsigned char);

typedef struct                     tak_union_entry
{
    tak_parskey         uparsid;
    tsp00_Int2          ufirstsel;
    tsp00_Int2          usecondsel;
    tsp00_Int2          ufiller;
    tak_union_kind      uis_kind;
    pasbool             uis_all;
} tak_union_entry;
typedef ARRAY_TYPE_MSP00 (         tak_union_array, tak_union_entry, cak_max_union);

typedef struct                     tak_unionrecord
{
    tsp00_Int2          usyskreclen;
    tsp00_Int2          usyskkeylen;
    tsp00_Int2          usyskvarcol_offset;
    tsp00_Int2          usyskvarcol_cnt;
    tgg00_Surrogate     usysksurrogate;
    tsp00_C2            usyskentrytype;
    tsp00_Int2          usysksegno;
    tsp00_Int2          uselect_cnt;
    tsp00_Int2          ucolpos;
    tgg00_FileId        ures_tree;
    tsp00_KnlIdentifier ures_name;
    tsp00_KnlIdentifier umodul_name;
    tsp00_Int4          urowno;
    tsp00_Int2          ukeylen;
    tsp00_Int2          usubcount;
    tsp00_Int2          ucntpar;
    pasbool             uinsertselect;
    pasbool             ufiller1;
    tak_union_array     uselect_arr;
} tak_unionrecord;

typedef enum tak_jv_upd_description_Enum
{
    no_jv_upd,
    jv_upd_release_locks,
    jv_upd_end_release
} tak_jv_upd_description_Enum;
typedef tak_jv_upd_description_Enum tak_jv_upd_description_Param;
typedef ENUM_TYPE_MSP00 (          tak_jv_upd_description, tak_jv_upd_description_Enum, jv_upd_end_release, unsigned char);

typedef enum tak_complex_info_Enum
{
    ci_lastpars,
    ci_unlock,
    ci_recursive
} tak_complex_info_Enum;
typedef tak_complex_info_Enum tak_complex_info_Param;
typedef ENUM_TYPE_MSP00 (          tak_complex_info, tak_complex_info_Enum, ci_recursive, unsigned char);
typedef SMALLSET_TYPE_MSP00 (      tak_complex_infoset, tak_complex_info_Enum, ci_recursive);

typedef struct                     tak_complex_entry
{
    tak_parskey         cparsk;
    tsp00_Int2          cnextpars;
    tak_complex_infoset centryinfo;
    tak_jv_upd_description cqual_jv_upd;
} tak_complex_entry;
typedef ARRAY_TYPE_MSP00 (         tak_complex_array, tak_complex_entry, cak_maxcomplex);

typedef struct                     tak_complexrecord
{
    tsp00_Int2          compreclen;
    tsp00_Int2          compkeylen;
    tsp00_Int2          compvarcol_offset;
    tsp00_Int2          compvarcol_cnt;
    tgg00_Surrogate     compsurrogate;
    tsp00_C2            compentrytype;
    tsp00_Int2          compsegno;
    tsp00_Int2          compcnt;
    tsp00_Int2          compmax_sel_cnt;
    tgg00_FileId        compcreateaseltree;
    tak_complex_array   comparr;
} tak_complexrecord;

typedef enum tak_one_table_specialname__Enum
{
    oresfile,
    oissynonym,
    oisreference,
    oisjoinview,
    oispartjoinview,
    oisshowview,
    oifiller1
} tak_one_table_specialname__Enum;
typedef tak_one_table_specialname__Enum tak_one_table_specialname__Param;
typedef SMALLSET_TYPE_MSP00 (      tak_one_table_specialname, tak_one_table_specialname__Enum, oifiller1);

typedef enum tak_messbockFieldSequenceInfo_Enum
{
    mfs_literals,
    mfs_parameters,
    mfs_pars_result,
    mfs_ex_result
} tak_messbockFieldSequenceInfo_Enum;
typedef tak_messbockFieldSequenceInfo_Enum tak_messbockFieldSequenceInfo_Param;
typedef ENUM_TYPE_MSP00 (          tak_messbockFieldSequenceInfo, tak_messbockFieldSequenceInfo_Enum, mfs_ex_result, unsigned char);

typedef struct                     tak_messblock_record
{
    tsp00_Int2          mbr_reclen;
    tsp00_Int2          mbr_keylen;
    tsp00_Int2          mbr_varcol_offset;
    tsp00_Int2          mbr_varcol_cnt;
    tgg00_Surrogate     mbr_surrogate;
    tsp00_C2            mbr_entrytype;
    tsp00_C2            mbr_linkage;
    tsp00_C2            mbr_segment_id;
    tsp00_Int4          mbr_stack_len;
    SMALLSET_VAR_MSP00 (      mbr_fieldSequenceInfo, tak_messbockFieldSequenceInfo_Enum, mfs_ex_result);
    char                mbr_recCount;
    tgg00_MessBlock     mbr_mess_block;
} tak_messblock_record;

typedef struct                     tak_soc_record
{
    tsp00_Int2          socoldtabno;
    tsp00_Int2          socselecttabno;
    tsp00_Uint1         socfields;
    tsp00_Uint1         socfill0;
    tsp00_Int2          socfill1;
} tak_soc_record;

typedef enum tak_cmd_part_type_Enum
{
    cpt_unknown,
    cpt_in_where_clause,
    cpt_in_having_clause,
    cpt_in_select_list
} tak_cmd_part_type_Enum;
typedef tak_cmd_part_type_Enum tak_cmd_part_type_Param;
typedef ENUM_TYPE_MSP00 (          tak_cmd_part_type, tak_cmd_part_type_Enum, cpt_in_select_list, unsigned char);

typedef struct                     tak_one_corcolumn
{
    tsp00_Int2          cfieldno;
    tsp00_Int2          cnode;
    tak_corlevel        clevel;
    tsp00_Int2          csetnode;
    tsp00_Uint1         ccntcol;
    tsp00_Uint1         cfiller2;
    tak_cmd_part_type   ccmd_part;
    tsp00_Uint1         cfiller1;
    tsp00_Int2          ctabno;
} tak_one_corcolumn;
typedef ARRAY_TYPE_MSP00 (         tak_all_corcolumns, tak_one_corcolumn, cak_maxcorcolumns);

typedef struct                     tak_correcord
{
    tsp00_Int2          tc_reclen;
    tsp00_Int2          tc_keylen;
    tsp00_Int2          tc_varcol_offset;
    tsp00_Int2          tc_varcol_cnt;
    tgg00_Surrogate     tc_surrogate;
    tsp00_C2            tc_entrytype;
    tsp00_Int2          tc_segno;
    tsp00_Int2          tc_no_of_tables;
    tsp00_Int2          tc_no_of_columns;
    tsp00_Int2          tc_starttable;
    tsp00_Int2          tc_oldstarttable;
    ARRAY_VAR_MSP00 (         tc_tables, tak_soc_record, 16);
    tak_all_corcolumns  tc_columns;
} tak_correcord;
    /* PTS 1111575 E.Z. */

typedef struct                     tak_linkposlen
{
    tsp00_Int2          lpos;
    tsp00_Int2          llen;
} tak_linkposlen;
typedef ARRAY_TYPE_MSP00 (         tak_linkposinfo, tak_linkposlen, cak_maxlinkpos);

typedef struct                     tak_linkinforecord
{
    tsp00_Int2          lreclen;
    tsp00_Int2          lvarcol_offset;
    tsp00_Int2          lvarcol_cnt;
    tsp00_Int2          lkeylen;
    tgg00_Surrogate     lsurrogate;
    tsp00_C2            lentrytype;
    tsp00_Int2          lseqno;
    tsp00_Int4          lfullen;
    tgg00_MessType      lmess_type;
    pasbool             lprim_check;
    tsp00_Int2          lunique_cnt;
    tak_columnset       lupd_col_info;
    tak_charset         lnot_used;
    tak_linkposinfo     lpos_info;
} tak_linkinforecord;
    /*========= cache data types =============*/

typedef enum tak_directory_state_Enum
{
    d_fix,
    d_release
} tak_directory_state_Enum;
typedef tak_directory_state_Enum tak_directory_state_Param;
typedef ENUM_TYPE_MSP00 (          tak_directory_state, tak_directory_state_Enum, d_release, unsigned char);

typedef enum tak_sysdir_state__Enum
{
    dfix,
    dlocked,
    dreference,
    dadd_rec,
    dwritten,
    dmodified,
    ddisplace,
    dno_meaning
} tak_sysdir_state__Enum;
typedef tak_sysdir_state__Enum tak_sysdir_state__Param;
typedef SMALLSET_TYPE_MSP00 (      tak_sysdir_state, tak_sysdir_state__Enum, dno_meaning);
typedef struct tak_cache_dir_entry tak_cache_dir_entry;
typedef tak_cache_dir_entry       *tak_cache_dir_ptr;
typedef tsp00_MoveObj             *tak_moveobj_ptr;
typedef tak_cache_dir_entry       *tak_cache_dir_e_ptr;

typedef struct                     tak_cache_dir_pointer
{
    union
    {
    /* 1 */
        struct tak_cache_dir_pointer_1
        {
            tak_cache_dir_e_ptr cdir_ptr_F;
        } C_1;
    /* 2 */
        struct tak_cache_dir_pointer_2
        {
            tsp00_BufAddr       buf_ptr_F;
        } C_2;
    /* 3 */
        struct tak_cache_dir_pointer_3
        {
            tak_moveobj_ptr     moveobj_ptr_F;
        } C_3;
    /* 4 */
        struct tak_cache_dir_pointer_4
        {
            tak_sysbufferaddress sys_ptr_F;
        } C_4;
    /* 5 */
        struct tak_cache_dir_pointer_5
        {
            tsp00_Int4          i_F;
        } C_5;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tak_cache_dir_e_ptr &cdir_ptr               ()  { return this->variant.C_1.cdir_ptr_F; };
    tsp00_BufAddr      &buf_ptr                ()  { return this->variant.C_2.buf_ptr_F; };
    tak_moveobj_ptr    &moveobj_ptr            ()  { return this->variant.C_3.moveobj_ptr_F; };
    tak_sysbufferaddress &sys_ptr                ()  { return this->variant.C_4.sys_ptr_F; };
    tsp00_Int4         &i                      ()  { return this->variant.C_5.i_F; };
    /* const accessor functions */
    const tak_cache_dir_e_ptr &cdir_ptr               () const  { return this->variant.C_1.cdir_ptr_F; };
    const tsp00_BufAddr &buf_ptr                () const  { return this->variant.C_2.buf_ptr_F; };
    const tak_moveobj_ptr &moveobj_ptr            () const  { return this->variant.C_3.moveobj_ptr_F; };
    const tak_sysbufferaddress &sys_ptr                () const  { return this->variant.C_4.sys_ptr_F; };
    const tsp00_Int4   &i                      () const  { return this->variant.C_5.i_F; };
#endif    /* defined (__cplusplus) */
} tak_cache_dir_pointer;

typedef struct                     tak_sysdirectory
{
    tak_cache_dir_pointer ci_cache_dir_header;
    tak_cache_dir_pointer ci_cache_dir_trailor;
    tsp00_BufAddr       ci_cache_p;
    tsp00_Int4          ci_tmp_read_cnt;
    tsp00_Int4          ci_displaceCnt;
    pasbool             ci_tmp_dislodged;
    pasbool             ci_inIterator;
    pasbool             ci_rollbackFileExists;
    pasbool             ci_filler2;
    tsp00_Int4          ci_tmp_read_found;
    tgg00_FileId        ci_cmd_rollback_tree;
    ARRAY_VAR_MSP00 (         ci_dir, tak_cache_dir_pointer, cak_emax);
} tak_sysdirectory;
    /* PTS 1111575 E.Z. */

typedef struct                     tak_syspointerarr
{
    ARRAY_VAR_MSP00 (         px, tak_sysbufferaddress, cak_maxpcount);
    tak_sysbufferaddress presnp;
    tak_sysbufferaddress pinfop;
    tak_sysbufferaddress pparsp;
    tak_sysbufferaddress pbasep;
    tak_sysbufferaddress pcolnamep;
    tgg00_Surrogate     psynid;
    pasbool             psynfound;
    pasbool             psystable;
    tsp00_Int2          pcount;

#if defined (BIT64)

    tsp00_Int4          pfiller2;

#endif

} tak_syspointerarr;

typedef struct                     tak_alterdropcol
{
    tsp00_Int2          dextno;
    tak00_colinfo_ptr   dcolptr;
} tak_alterdropcol;

typedef struct                     tak_dropcol
{
    tsp00_Int4          dcount;
    tsp00_Int2          dtabno;
    pasbool             ddel;
    pasbool             dfiller;
    ARRAY_VAR_MSP00 (         dcol, tak_alterdropcol, MAX_COL_PER_TAB_GG00);
} tak_dropcol;

typedef enum tak_index_cmd_mode_Enum
{
    cmd_create_index,
    cmd_drop_index
} tak_index_cmd_mode_Enum;
typedef tak_index_cmd_mode_Enum tak_index_cmd_mode_Param;
typedef ENUM_TYPE_MSP00 (          tak_index_cmd_mode, tak_index_cmd_mode_Enum, cmd_drop_index, unsigned char);
typedef ARRAY_TYPE_MSP00 (         tak_index_col_info, tak00_colinfo_ptr, MAX_COL_SEQUENCE_GG00);

typedef enum tak_viewscantype_Enum
{
    v_version,
    v_statistics,
    v_intern_save_scheme,
    v_alter_drop,
    v_index,
    v_alter_column,
    v_rename_column,
    v_rename_table,
    v_save_scheme,
    v_revoke_priv,
    v_rename_user
} tak_viewscantype_Enum;
typedef tak_viewscantype_Enum tak_viewscantype_Param;
typedef ENUM_TYPE_MSP00 (          tak_viewscantype, tak_viewscantype_Enum, v_rename_user, unsigned char);

typedef struct                     tak_viewscan_par
{
    tsp00_KnlIdentifier vsc_baseauthname;
    tsp00_KnlIdentifier vsc_basetablen;
    tgg91_FileVersion   vsc_filevers;
    pasbool             vsc_rp_rt_handling;
    tak_viewscantype    vsc_init_type;
    tgg00_Surrogate     vsc_base_tabid;
    tgg00_SysInfoKey    vsc_key;
    pasbool             vsc_edi;
    pasbool             vsc_restrict;
    tsp00_Int2          vsc_ti;
    tsp00_Int2          vsc_tabno;
    tak_viewscantype    vsc_type;
    pasbool             vsc_delete_view;
    pasbool             vsc_mult_index;
    pasbool             vsc_modified;
    tgg00_Surrogate     vsc_from_tableid;
    pasbool             vsc_col_list;
    pasbool             vsc_col_dropped;
    tak_usagedef        vsc_usagedef;
    tgg00_Surrogate     vsc_start_tabid;
    ARRAY_VAR_MSP00 (         vsc_vdesc_modified, pasbool, 8);
    tsp00_Int4          vsc_rows;
    tak_joinset         vsc_tabno_set;
    tsp00_Int2          vsc_vdesc_cnt;
    union
    {
    /* v_statistics */
        struct tak_viewscan_par_v_statistics
        {
            tsp00_Int4          vsc_base_page_cnt_F;
            tsp00_Int4          vsc_base_rec_cnt_F;
            tsp00_Int4          vsc_st_filler1_F;
            tsp00_Int4          vsc_st_filler2_F;
            tak_upd_stat_info   vsc_page_cnt_info_F;
        } C_v_statistics;
    /* v_alter_drop */
        struct tak_viewscan_par_v_alter_drop
        {
            tak_columnset       vsc_drop_set_F;
            tak_columnset       vsc_valid_colset_F;
            tak_dropcol         vsc_dr_col_F;
            tak_colindex        vsc_valid_cols_F;
            tsp00_Int4          vsc_valid_cnt_F;
            tak_columnset       vsc_extcol_set_F;
            tgg00_Surrogate     vsc_rev_tableid_F;
            pasbool             vsc_start_revoke_F;
            tak_usertyp         vsc_userkind_F;
            tsp00_Int4          vsc_page_cnt_F;
            tsp00_Int4          vsc_rec_cnt_F;
            tsp00_Int4          vsc_prim_cnt_F;
            tsp00_Int4          vsc_null_cnt_F;
            pasbool             vsc_single_index_F;
            tak_index_cmd_mode  vsc_mode_F;
            tsp00_Int2          vsc_index_col_count_F;
            tak_index_col_info  vsc_col_info_F;
            tsp00_KnlIdentifier vsc_indexname_F;
            pasbool             vsc_unique_F;
            pasbool             vsc_ignore_error_F;
            tsp00_Int2          vsc_keylen_F;
            tak00_colinfo_ptr   vsc_col_ptr_F;
            tgg00_StackOpType   vsc_op_order_F;
            pasbool             vsc_filler1_F;
            pasbool             vsc_filler2_F;
            tgg00_FileId        vsc_dropped_treeid_F;
        } C_v_alter_drop;
    /* v_alter_column */
        struct tak_viewscan_par_v_alter_column
        {
            tak_dropcol         vsc_drop_col_F;
            tak00_columninfo    vsc_colinfo_F;
            pasbool             vsc_coladd_F;
        } C_v_alter_column;
    /* v_rename_column */
        struct tak_viewscan_par_v_rename_column
        {
            tak_columnset       vsc_rename_set_F;
            tsp00_Int4          vsc_info_cnt_F;
            tak_colinteger      vsc_pos_info_F;
            tsp00_KnlIdentifier vsc_newcolname_F;
            tsp00_Int2          vsc_newcol_len_F;
            tsp00_KnlIdentifier vsc_oldcolname_F;
            tsp00_Int2          vsc_col_extcolno_F;
            tak00_colinfo_ptr   vsc_colptr_F;
            tsp00_Int4          vsc_renamed_cols_F;
            pasbool             vsc_renamed_F;
            pasbool             vsc_col_in_qual_F;
            pasbool             vsc_quotes_F;
            pasbool             vsc_rename_filler_F;
            tsp00_KnlIdentifier vsc_new_authname_F;
            tsp00_Int2          vsc_old_len_F;
            tsp00_KnlIdentifier vsc_new_tablen_F;
            tsp00_Int2          vsc_new_len_F;
            tsp00_KnlIdentifier vsc_old_tablen_F;
            tgg00_Surrogate     vsc_ren_tabid_F;
        } C_v_rename_column;
    /* v_rename_user */
        struct tak_viewscan_par_v_rename_user
        {
            tsp00_KnlIdentifier vsc_old_user_name_F;
            tsp00_KnlIdentifier vsc_new_user_name_F;
        } C_v_rename_user;
    /* v_save_scheme */
        struct tak_viewscan_par_v_save_scheme
        {
            tgg00_FileId        vsc_tree_id_F;
            tak_sysbufferaddress vsc_buf_F;
            tsp00_KnlIdentifier vsc_into_authname_F;
            tsp00_KnlIdentifier vsc_into_tablen_F;
            pasbool             vsc_save_into_F;
            tgg00_TableKind     vsc_tablekind_F;
            tgg00_FileId        vsc_into_treeid_F;
            tsp00_Int4          vsc_cmd_cnt_F;
            pasbool             vsc_first_save_F;
            pasbool             vsc_last_save_F;
            pasbool             vsc_user_save_F;
            pasbool             vsc_all_save_F;
            pasbool             vsc_schema_save_F;
        } C_v_save_scheme;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tsp00_Int4         &vsc_base_page_cnt      ()  { return this->variant.C_v_statistics.vsc_base_page_cnt_F; };
    tsp00_Int4         &vsc_base_rec_cnt       ()  { return this->variant.C_v_statistics.vsc_base_rec_cnt_F; };
    tsp00_Int4         &vsc_st_filler1         ()  { return this->variant.C_v_statistics.vsc_st_filler1_F; };
    tsp00_Int4         &vsc_st_filler2         ()  { return this->variant.C_v_statistics.vsc_st_filler2_F; };
    tak_upd_stat_info  &vsc_page_cnt_info      ()  { return this->variant.C_v_statistics.vsc_page_cnt_info_F; };
    tak_columnset      &vsc_drop_set           ()  { return this->variant.C_v_alter_drop.vsc_drop_set_F; };
    tak_columnset      &vsc_valid_colset       ()  { return this->variant.C_v_alter_drop.vsc_valid_colset_F; };
    tak_dropcol        &vsc_dr_col             ()  { return this->variant.C_v_alter_drop.vsc_dr_col_F; };
    tak_colindex       &vsc_valid_cols         ()  { return this->variant.C_v_alter_drop.vsc_valid_cols_F; };
    tsp00_Int4         &vsc_valid_cnt          ()  { return this->variant.C_v_alter_drop.vsc_valid_cnt_F; };
    tak_columnset      &vsc_extcol_set         ()  { return this->variant.C_v_alter_drop.vsc_extcol_set_F; };
    tgg00_Surrogate    &vsc_rev_tableid        ()  { return this->variant.C_v_alter_drop.vsc_rev_tableid_F; };
    pasbool            &vsc_start_revoke       ()  { return this->variant.C_v_alter_drop.vsc_start_revoke_F; };
    tak_usertyp        &vsc_userkind           ()  { return this->variant.C_v_alter_drop.vsc_userkind_F; };
    tsp00_Int4         &vsc_page_cnt           ()  { return this->variant.C_v_alter_drop.vsc_page_cnt_F; };
    tsp00_Int4         &vsc_rec_cnt            ()  { return this->variant.C_v_alter_drop.vsc_rec_cnt_F; };
    tsp00_Int4         &vsc_prim_cnt           ()  { return this->variant.C_v_alter_drop.vsc_prim_cnt_F; };
    tsp00_Int4         &vsc_null_cnt           ()  { return this->variant.C_v_alter_drop.vsc_null_cnt_F; };
    pasbool            &vsc_single_index       ()  { return this->variant.C_v_alter_drop.vsc_single_index_F; };
    tak_index_cmd_mode &vsc_mode               ()  { return this->variant.C_v_alter_drop.vsc_mode_F; };
    tsp00_Int2         &vsc_index_col_count    ()  { return this->variant.C_v_alter_drop.vsc_index_col_count_F; };
    tak_index_col_info &vsc_col_info           ()  { return this->variant.C_v_alter_drop.vsc_col_info_F; };
    tsp00_KnlIdentifier &vsc_indexname          ()  { return this->variant.C_v_alter_drop.vsc_indexname_F; };
    pasbool            &vsc_unique             ()  { return this->variant.C_v_alter_drop.vsc_unique_F; };
    pasbool            &vsc_ignore_error       ()  { return this->variant.C_v_alter_drop.vsc_ignore_error_F; };
    tsp00_Int2         &vsc_keylen             ()  { return this->variant.C_v_alter_drop.vsc_keylen_F; };
    tak00_colinfo_ptr  &vsc_col_ptr            ()  { return this->variant.C_v_alter_drop.vsc_col_ptr_F; };
    tgg00_StackOpType  &vsc_op_order           ()  { return this->variant.C_v_alter_drop.vsc_op_order_F; };
    pasbool            &vsc_filler1            ()  { return this->variant.C_v_alter_drop.vsc_filler1_F; };
    pasbool            &vsc_filler2            ()  { return this->variant.C_v_alter_drop.vsc_filler2_F; };
    tgg00_FileId       &vsc_dropped_treeid     ()  { return this->variant.C_v_alter_drop.vsc_dropped_treeid_F; };
    tak_dropcol        &vsc_drop_col           ()  { return this->variant.C_v_alter_column.vsc_drop_col_F; };
    tak00_columninfo   &vsc_colinfo            ()  { return this->variant.C_v_alter_column.vsc_colinfo_F; };
    pasbool            &vsc_coladd             ()  { return this->variant.C_v_alter_column.vsc_coladd_F; };
    tak_columnset      &vsc_rename_set         ()  { return this->variant.C_v_rename_column.vsc_rename_set_F; };
    tsp00_Int4         &vsc_info_cnt           ()  { return this->variant.C_v_rename_column.vsc_info_cnt_F; };
    tak_colinteger     &vsc_pos_info           ()  { return this->variant.C_v_rename_column.vsc_pos_info_F; };
    tsp00_KnlIdentifier &vsc_newcolname         ()  { return this->variant.C_v_rename_column.vsc_newcolname_F; };
    tsp00_Int2         &vsc_newcol_len         ()  { return this->variant.C_v_rename_column.vsc_newcol_len_F; };
    tsp00_KnlIdentifier &vsc_oldcolname         ()  { return this->variant.C_v_rename_column.vsc_oldcolname_F; };
    tsp00_Int2         &vsc_col_extcolno       ()  { return this->variant.C_v_rename_column.vsc_col_extcolno_F; };
    tak00_colinfo_ptr  &vsc_colptr             ()  { return this->variant.C_v_rename_column.vsc_colptr_F; };
    tsp00_Int4         &vsc_renamed_cols       ()  { return this->variant.C_v_rename_column.vsc_renamed_cols_F; };
    pasbool            &vsc_renamed            ()  { return this->variant.C_v_rename_column.vsc_renamed_F; };
    pasbool            &vsc_col_in_qual        ()  { return this->variant.C_v_rename_column.vsc_col_in_qual_F; };
    pasbool            &vsc_quotes             ()  { return this->variant.C_v_rename_column.vsc_quotes_F; };
    pasbool            &vsc_rename_filler      ()  { return this->variant.C_v_rename_column.vsc_rename_filler_F; };
    tsp00_KnlIdentifier &vsc_new_authname       ()  { return this->variant.C_v_rename_column.vsc_new_authname_F; };
    tsp00_Int2         &vsc_old_len            ()  { return this->variant.C_v_rename_column.vsc_old_len_F; };
    tsp00_KnlIdentifier &vsc_new_tablen         ()  { return this->variant.C_v_rename_column.vsc_new_tablen_F; };
    tsp00_Int2         &vsc_new_len            ()  { return this->variant.C_v_rename_column.vsc_new_len_F; };
    tsp00_KnlIdentifier &vsc_old_tablen         ()  { return this->variant.C_v_rename_column.vsc_old_tablen_F; };
    tgg00_Surrogate    &vsc_ren_tabid          ()  { return this->variant.C_v_rename_column.vsc_ren_tabid_F; };
    tsp00_KnlIdentifier &vsc_old_user_name      ()  { return this->variant.C_v_rename_user.vsc_old_user_name_F; };
    tsp00_KnlIdentifier &vsc_new_user_name      ()  { return this->variant.C_v_rename_user.vsc_new_user_name_F; };
    tgg00_FileId       &vsc_tree_id            ()  { return this->variant.C_v_save_scheme.vsc_tree_id_F; };
    tak_sysbufferaddress &vsc_buf                ()  { return this->variant.C_v_save_scheme.vsc_buf_F; };
    tsp00_KnlIdentifier &vsc_into_authname      ()  { return this->variant.C_v_save_scheme.vsc_into_authname_F; };
    tsp00_KnlIdentifier &vsc_into_tablen        ()  { return this->variant.C_v_save_scheme.vsc_into_tablen_F; };
    pasbool            &vsc_save_into          ()  { return this->variant.C_v_save_scheme.vsc_save_into_F; };
    tgg00_TableKind    &vsc_tablekind          ()  { return this->variant.C_v_save_scheme.vsc_tablekind_F; };
    tgg00_FileId       &vsc_into_treeid        ()  { return this->variant.C_v_save_scheme.vsc_into_treeid_F; };
    tsp00_Int4         &vsc_cmd_cnt            ()  { return this->variant.C_v_save_scheme.vsc_cmd_cnt_F; };
    pasbool            &vsc_first_save         ()  { return this->variant.C_v_save_scheme.vsc_first_save_F; };
    pasbool            &vsc_last_save          ()  { return this->variant.C_v_save_scheme.vsc_last_save_F; };
    pasbool            &vsc_user_save          ()  { return this->variant.C_v_save_scheme.vsc_user_save_F; };
    pasbool            &vsc_all_save           ()  { return this->variant.C_v_save_scheme.vsc_all_save_F; };
    pasbool            &vsc_schema_save        ()  { return this->variant.C_v_save_scheme.vsc_schema_save_F; };
    /* const accessor functions */
    const tsp00_Int4   &vsc_base_page_cnt      () const  { return this->variant.C_v_statistics.vsc_base_page_cnt_F; };
    const tsp00_Int4   &vsc_base_rec_cnt       () const  { return this->variant.C_v_statistics.vsc_base_rec_cnt_F; };
    const tsp00_Int4   &vsc_st_filler1         () const  { return this->variant.C_v_statistics.vsc_st_filler1_F; };
    const tsp00_Int4   &vsc_st_filler2         () const  { return this->variant.C_v_statistics.vsc_st_filler2_F; };
    const tak_upd_stat_info &vsc_page_cnt_info      () const  { return this->variant.C_v_statistics.vsc_page_cnt_info_F; };
    const tak_columnset &vsc_drop_set           () const  { return this->variant.C_v_alter_drop.vsc_drop_set_F; };
    const tak_columnset &vsc_valid_colset       () const  { return this->variant.C_v_alter_drop.vsc_valid_colset_F; };
    const tak_dropcol  &vsc_dr_col             () const  { return this->variant.C_v_alter_drop.vsc_dr_col_F; };
    const tak_colindex &vsc_valid_cols         () const  { return this->variant.C_v_alter_drop.vsc_valid_cols_F; };
    const tsp00_Int4   &vsc_valid_cnt          () const  { return this->variant.C_v_alter_drop.vsc_valid_cnt_F; };
    const tak_columnset &vsc_extcol_set         () const  { return this->variant.C_v_alter_drop.vsc_extcol_set_F; };
    const tgg00_Surrogate &vsc_rev_tableid        () const  { return this->variant.C_v_alter_drop.vsc_rev_tableid_F; };
    const pasbool      &vsc_start_revoke       () const  { return this->variant.C_v_alter_drop.vsc_start_revoke_F; };
    const tak_usertyp  &vsc_userkind           () const  { return this->variant.C_v_alter_drop.vsc_userkind_F; };
    const tsp00_Int4   &vsc_page_cnt           () const  { return this->variant.C_v_alter_drop.vsc_page_cnt_F; };
    const tsp00_Int4   &vsc_rec_cnt            () const  { return this->variant.C_v_alter_drop.vsc_rec_cnt_F; };
    const tsp00_Int4   &vsc_prim_cnt           () const  { return this->variant.C_v_alter_drop.vsc_prim_cnt_F; };
    const tsp00_Int4   &vsc_null_cnt           () const  { return this->variant.C_v_alter_drop.vsc_null_cnt_F; };
    const pasbool      &vsc_single_index       () const  { return this->variant.C_v_alter_drop.vsc_single_index_F; };
    const tak_index_cmd_mode &vsc_mode               () const  { return this->variant.C_v_alter_drop.vsc_mode_F; };
    const tsp00_Int2   &vsc_index_col_count    () const  { return this->variant.C_v_alter_drop.vsc_index_col_count_F; };
    const tak_index_col_info &vsc_col_info           () const  { return this->variant.C_v_alter_drop.vsc_col_info_F; };
    const tsp00_KnlIdentifier &vsc_indexname          () const  { return this->variant.C_v_alter_drop.vsc_indexname_F; };
    const pasbool      &vsc_unique             () const  { return this->variant.C_v_alter_drop.vsc_unique_F; };
    const pasbool      &vsc_ignore_error       () const  { return this->variant.C_v_alter_drop.vsc_ignore_error_F; };
    const tsp00_Int2   &vsc_keylen             () const  { return this->variant.C_v_alter_drop.vsc_keylen_F; };
    const tak00_colinfo_ptr &vsc_col_ptr            () const  { return this->variant.C_v_alter_drop.vsc_col_ptr_F; };
    const tgg00_StackOpType &vsc_op_order           () const  { return this->variant.C_v_alter_drop.vsc_op_order_F; };
    const pasbool      &vsc_filler1            () const  { return this->variant.C_v_alter_drop.vsc_filler1_F; };
    const pasbool      &vsc_filler2            () const  { return this->variant.C_v_alter_drop.vsc_filler2_F; };
    const tgg00_FileId &vsc_dropped_treeid     () const  { return this->variant.C_v_alter_drop.vsc_dropped_treeid_F; };
    const tak_dropcol  &vsc_drop_col           () const  { return this->variant.C_v_alter_column.vsc_drop_col_F; };
    const tak00_columninfo &vsc_colinfo            () const  { return this->variant.C_v_alter_column.vsc_colinfo_F; };
    const pasbool      &vsc_coladd             () const  { return this->variant.C_v_alter_column.vsc_coladd_F; };
    const tak_columnset &vsc_rename_set         () const  { return this->variant.C_v_rename_column.vsc_rename_set_F; };
    const tsp00_Int4   &vsc_info_cnt           () const  { return this->variant.C_v_rename_column.vsc_info_cnt_F; };
    const tak_colinteger &vsc_pos_info           () const  { return this->variant.C_v_rename_column.vsc_pos_info_F; };
    const tsp00_KnlIdentifier &vsc_newcolname         () const  { return this->variant.C_v_rename_column.vsc_newcolname_F; };
    const tsp00_Int2   &vsc_newcol_len         () const  { return this->variant.C_v_rename_column.vsc_newcol_len_F; };
    const tsp00_KnlIdentifier &vsc_oldcolname         () const  { return this->variant.C_v_rename_column.vsc_oldcolname_F; };
    const tsp00_Int2   &vsc_col_extcolno       () const  { return this->variant.C_v_rename_column.vsc_col_extcolno_F; };
    const tak00_colinfo_ptr &vsc_colptr             () const  { return this->variant.C_v_rename_column.vsc_colptr_F; };
    const tsp00_Int4   &vsc_renamed_cols       () const  { return this->variant.C_v_rename_column.vsc_renamed_cols_F; };
    const pasbool      &vsc_renamed            () const  { return this->variant.C_v_rename_column.vsc_renamed_F; };
    const pasbool      &vsc_col_in_qual        () const  { return this->variant.C_v_rename_column.vsc_col_in_qual_F; };
    const pasbool      &vsc_quotes             () const  { return this->variant.C_v_rename_column.vsc_quotes_F; };
    const pasbool      &vsc_rename_filler      () const  { return this->variant.C_v_rename_column.vsc_rename_filler_F; };
    const tsp00_KnlIdentifier &vsc_new_authname       () const  { return this->variant.C_v_rename_column.vsc_new_authname_F; };
    const tsp00_Int2   &vsc_old_len            () const  { return this->variant.C_v_rename_column.vsc_old_len_F; };
    const tsp00_KnlIdentifier &vsc_new_tablen         () const  { return this->variant.C_v_rename_column.vsc_new_tablen_F; };
    const tsp00_Int2   &vsc_new_len            () const  { return this->variant.C_v_rename_column.vsc_new_len_F; };
    const tsp00_KnlIdentifier &vsc_old_tablen         () const  { return this->variant.C_v_rename_column.vsc_old_tablen_F; };
    const tgg00_Surrogate &vsc_ren_tabid          () const  { return this->variant.C_v_rename_column.vsc_ren_tabid_F; };
    const tsp00_KnlIdentifier &vsc_old_user_name      () const  { return this->variant.C_v_rename_user.vsc_old_user_name_F; };
    const tsp00_KnlIdentifier &vsc_new_user_name      () const  { return this->variant.C_v_rename_user.vsc_new_user_name_F; };
    const tgg00_FileId &vsc_tree_id            () const  { return this->variant.C_v_save_scheme.vsc_tree_id_F; };
    const tak_sysbufferaddress &vsc_buf                () const  { return this->variant.C_v_save_scheme.vsc_buf_F; };
    const tsp00_KnlIdentifier &vsc_into_authname      () const  { return this->variant.C_v_save_scheme.vsc_into_authname_F; };
    const tsp00_KnlIdentifier &vsc_into_tablen        () const  { return this->variant.C_v_save_scheme.vsc_into_tablen_F; };
    const pasbool      &vsc_save_into          () const  { return this->variant.C_v_save_scheme.vsc_save_into_F; };
    const tgg00_TableKind &vsc_tablekind          () const  { return this->variant.C_v_save_scheme.vsc_tablekind_F; };
    const tgg00_FileId &vsc_into_treeid        () const  { return this->variant.C_v_save_scheme.vsc_into_treeid_F; };
    const tsp00_Int4   &vsc_cmd_cnt            () const  { return this->variant.C_v_save_scheme.vsc_cmd_cnt_F; };
    const pasbool      &vsc_first_save         () const  { return this->variant.C_v_save_scheme.vsc_first_save_F; };
    const pasbool      &vsc_last_save          () const  { return this->variant.C_v_save_scheme.vsc_last_save_F; };
    const pasbool      &vsc_user_save          () const  { return this->variant.C_v_save_scheme.vsc_user_save_F; };
    const pasbool      &vsc_all_save           () const  { return this->variant.C_v_save_scheme.vsc_all_save_F; };
    const pasbool      &vsc_schema_save        () const  { return this->variant.C_v_save_scheme.vsc_schema_save_F; };
#endif    /* defined (__cplusplus) */
} tak_viewscan_par;

typedef struct                     tak_save_viewscan_par
{
    tsp00_KnlIdentifier vsc_baseauthname;
    tsp00_KnlIdentifier vsc_basetablen;
    tgg91_FileVersion   vsc_filevers;
    pasbool             vsc_rp_rt_handling;
    tak_viewscantype    vsc_init_type;
    tgg00_Surrogate     vsc_base_tabid;
    tgg00_SysInfoKey    vsc_key;
    pasbool             vsc_edi;
    pasbool             vsc_free;
    tsp00_Int2          vsc_ti;
    tsp00_Int2          vsc_tabno;
    tak_viewscantype    vsc_type;
    pasbool             vsc_delete_view;
    pasbool             vsc_mult_index;
    pasbool             vsc_modified;
    tgg00_Surrogate     vsc_from_tableid;
    pasbool             vsc_col_list;
    pasbool             vsc_col_dropped;
    tak_usagedef        vsc_usagedef;
    tgg00_Surrogate     vsc_start_tabid;
    ARRAY_VAR_MSP00 (         vsc_vdesc_modified, pasbool, 8);
    tsp00_Int4          vsc_rows;
    tak_joinset         vsc_tabno_set;
    tsp00_Int2          vsc_vdesc_cnt;
    tgg00_FileId        vsc_tree_id;
    tak_sysbufferaddress vsc_buf;
    tsp00_KnlIdentifier vsc_into_authname;
    tsp00_KnlIdentifier vsc_into_tablen;
    pasbool             vsc_save_into;
    tgg00_TableKind     vsc_tablekind;
    tgg00_FileId        vsc_into_treeid;
    tsp00_Int4          vsc_cmd_cnt;
    pasbool             vsc_first_save;
    pasbool             vsc_last_save;
    pasbool             vsc_user_save;
    pasbool             vsc_all_save;
    pasbool             vsc_schema_save;
} tak_save_viewscan_par;

typedef enum tak_function_Enum
{
    tf_unknown,
    tf_no_func,
    tf_func,
    tf_func_arith
} tak_function_Enum;
typedef tak_function_Enum tak_function_Param;
typedef ENUM_TYPE_MSP00 (          tak_function, tak_function_Enum, tf_func_arith, unsigned char);

typedef enum tak_internal_sql_kind_Enum
{
    no_internal_sql,
    sql_restore_schema,
    sql_alter_table,
    sql_internal_proc_call,
    sql_create_table_as_select,
    sql_parse_cursor
} tak_internal_sql_kind_Enum;
typedef tak_internal_sql_kind_Enum tak_internal_sql_kind_Param;
typedef ENUM_TYPE_MSP00 (          tak_internal_sql_kind, tak_internal_sql_kind_Enum, sql_parse_cursor, unsigned char);

typedef enum tak_commandkind_Enum
{
    single_command,
    complex_view_command,
    union_command,
    subquery_command,
    sub_in_complex_command,
    sub_in_union_command,
    last_command,
    show_command,
    union_in_sub_command,
    link_command,
    internal_create_tab_command,
    corr_in_union_command
} tak_commandkind_Enum;
typedef tak_commandkind_Enum tak_commandkind_Param;
typedef ENUM_TYPE_MSP00 (          tak_commandkind, tak_commandkind_Enum, corr_in_union_command, unsigned char);
typedef ARRAY2_TYPE_MSP00 (        tak_site_states, tgg00_TransState, 7 - 0 + 1, 255 - 0 + 1);
typedef tak_site_states           *tak_site_state_ptr;

typedef enum tak_isolation_info_Enum
{
    temp_lock_rec_not_needed,
    temp_lock_rec_needed,
    temp_lock_rec_get,
    temp_lock_rec_finished
} tak_isolation_info_Enum;
typedef tak_isolation_info_Enum tak_isolation_info_Param;
typedef ENUM_TYPE_MSP00 (          tak_isolation_info, tak_isolation_info_Enum, temp_lock_rec_finished, unsigned char);

typedef struct                     tak_transinfo
{
    tgg00_TransContext  tri_trans;
    tgg00_HandlingSet   tri_global_state;
    tsp00_Uint2         tri_fill1;
    tsp00_C6            tri_obj_seq;
    tsp00_Int2          tri_fill2;
} tak_transinfo;

typedef enum tak_rollback_info__Enum
{
    trans_rollback,
    new_trans
} tak_rollback_info__Enum;
typedef tak_rollback_info__Enum tak_rollback_info__Param;
typedef SMALLSET_TYPE_MSP00 (      tak_rollback_info, tak_rollback_info__Enum, new_trans);

typedef enum tak_query_type_Enum
{
    type_n,
    type_j,
    type_an,
    type_aj,
    type_else,
    type_no
} tak_query_type_Enum;
typedef tak_query_type_Enum tak_query_type_Param;
typedef ENUM_TYPE_MSP00 (          tak_query_type, tak_query_type_Enum, type_no, unsigned char);

typedef struct                     tak_updstat_rec
{
    tgg00_Surrogate     us_tabid;
    tsp00_Int4          us_date;
    tsp00_Int4          us_time;
    tsp00_Int4          us_known_pages;
    tsp00_Int4          us_found_pages;
} tak_updstat_rec;

typedef struct                     tak_index_scan_record
{
    tak_sysbufferaddress isr_buf;
    integer             isr_index;
    tgg00_SysInfoKey    isr_sysk;
} tak_index_scan_record;

typedef struct                     tak_optimize_info
{
    pasbool             o_do_optimize;
    tak_query_type      o_query_type;
    tsp00_Int2          o_mess_type;
    tak_execution_kind  o_ex_kind;
    pasbool             o_optimized;
    tsp00_Int2          o_filler1;
    tgg00_FileId        o_cmd_tree;
    tsp00_C8            o_filler3;
} tak_optimize_info;

typedef struct                     tak_nls_param_rec
{
    tsp00_C4            currency;
    tsp00_C50           date_format;
    tsp00_C3            date_language;
    tsp00_C2            numeric_chars;
    tsp00_Int2          df_length;
    pasbool             dl_found;
    pasbool             dl_is_german;
    tsp00_Uint1         dl_language_no;
    pasbool             nls_binary;
    tsp00_KnlIdentifier nls_sort;
} tak_nls_param_rec;

typedef enum tak_appl_type_Enum
{
    at_unknown,
    at_cpc,
    at_cpp,
    at_db_manager,
    at_gpc,
    at_jdbc,
    at_load,
    at_odbc,
    at_util,
    at_xci,
    at_kernel,
    at_debugger
} tak_appl_type_Enum;
typedef tak_appl_type_Enum tak_appl_type_Param;
typedef ENUM_TYPE_MSP00 (          tak_appl_type, tak_appl_type_Enum, at_debugger, unsigned char);

typedef enum tak_explain_kind_Enum
{
    ex_default,
    ex_join,
    ex_sequence
} tak_explain_kind_Enum;
typedef tak_explain_kind_Enum tak_explain_kind_Param;
typedef ENUM_TYPE_MSP00 (          tak_explain_kind, tak_explain_kind_Enum, ex_sequence, unsigned char);

typedef struct                     tak_cmd_stack
{
    tsp00_Int4          cst_top;
    ARRAY_VAR_MSP00 (         cst_stack, tsp00_MoveObjPtr, cak_max_cmd_stack);
} tak_cmd_stack;

typedef enum tak_recursive_state_Enum
{
    rs_no_recursive_select,
    rs_check_select_list,
    rs_first_select,
    rs_recursive_select,
    rs_last_select
} tak_recursive_state_Enum;
typedef tak_recursive_state_Enum tak_recursive_state_Param;
typedef ENUM_TYPE_MSP00 (          tak_recursive_state, tak_recursive_state_Enum, rs_last_select, unsigned char);

typedef struct                     tak_kernel_sink
{
    union
    {
    /* true */
        struct tak_kernel_sink_true
        {
            tsp00_Longreal      d_F;
        } C_true;
    /* false */
        struct tak_kernel_sink_false
        {
            tsp00_C132          c_F;
        } C_false;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tsp00_Longreal     &d                      ()  { return this->variant.C_true.d_F; };
    tsp00_C132         &c                      ()  { return this->variant.C_false.c_F; };
    /* const accessor functions */
    const tsp00_Longreal &d                      () const  { return this->variant.C_true.d_F; };
    const tsp00_C132   &c                      () const  { return this->variant.C_false.c_F; };
#endif    /* defined (__cplusplus) */
} tak_kernel_sink;

typedef struct                     tak_AllocatorMonitorInfo
{
    tsp00_8ByteCounter  ami_MallocMin;
    tsp00_8ByteCounter  ami_MallocMax;
    tsp00_8ByteCounter  ami_MallocSum;
    tsp00_8ByteCounter  ami_MallocCnt;
    tsp00_8ByteCounter  ami_FreeMin;
    tsp00_8ByteCounter  ami_FreeMax;
    tsp00_8ByteCounter  ami_FreeSum;
    tsp00_8ByteCounter  ami_FreeCnt;
    tsp00_8ByteCounter  ami_Allocated;
} tak_AllocatorMonitorInfo;

typedef struct                     tak_allocator_info
{
    tsp00_Int4          ai_uktId;
    tsp00_Int4          ai_badAllocCountDown;
    tsp00_Int4          ai_badAllocCount;
    tsp00_8ByteCounter  ai_HeapUsageCurrent;
    tsp00_8ByteCounter  ai_HeapUsageMax;
    tak_AllocatorMonitorInfo ai_alloc_sys;
    tak_AllocatorMonitorInfo ai_alloc_usr;
} tak_allocator_info;

typedef struct                     tak_dbproc_call_info
{
    pasbool             dbc_conversionError;
    tsp00_Int4          dbc_errorTableId;
} tak_dbproc_call_info;

typedef struct                     tak_used_corr_infos
{
    tsp00_Uint1         uci_oldlowpars;
    tsp00_Uint1         uci_lowpars;
    tsp00_C1            uci_pid;
} tak_used_corr_infos;
typedef struct tak_all_command_glob tak_all_command_glob;
typedef tak_all_command_glob      *tak_acv_address;
typedef struct tak_size_info tak_size_info;
typedef tak_size_info             *tak_size_ptr;
    /**/

typedef enum tak_command_states__Enum
{
    csa_subq_datatype_problem,
    csa_hint_exist,
    csa_short_sum_values,
    csa_unused4,
    csa_unused5,
    csa_unused6,
    csa_unused7,
    csa_unused8
} tak_command_states__Enum;
typedef tak_command_states__Enum tak_command_states__Param;
typedef SMALLSET_TYPE_MSP00 (      tak_command_states, tak_command_states__Enum, csa_unused8);

 struct                     tak_all_command_glob
{
    tsp1_packet_ptr     a_in_packet;
    tsp1_packet_ptr     a_out_packet;
    tsp1_segment_ptr    a_cmd_segm;
    tsp1_part_ptr       a_cmd_part;
    tsp1_part_ptr       a_data_part;
    tsp1_part_ptr       a_abap_part;
    tsp1_part_ptr       a_rescount_part;
    tsp1_part_ptr       a_resname_part;
    tsp1_part_ptr       a_modulename_part;
    tsp1_part_ptr       a_appl_param_part;
    tsp1_part_ptr       a_feature_part;
    tsp1_part_ptr       a_clientid_part;
    tsp00_MoveObjPtr    a_data_ptr;
    tsp1_segment_ptr    a_return_segm;
    tak_ap_max_tree_ptr a_ap_tree;
    tsp1_part_ptr       a_curr_retpart;
    tsp00_Int4          a_input_data_pos;
    tsp00_Int4          a_output_data_pos;
    tgg00_HandlingSet   a_cmd_globstate;
    pasbool             a_info_output;
    tak_execution_kind  a_ex_kind;
    tsp00_Uint1         a_precomp_info_byte;
    tsp00_Int2          a_scv_index;
    tak_scanner_glob    a_scv;
    pasbool             a_in_internal_subtr;
    tak_ddl_descriptor  a_init_ddl;
    tsp00_Int2          a_count_variab;
    tsp00_Int2          a_count_literals;
    tsp00_Int2          a_count_output_params;
    tsp00_Int2          a_first_output_param;
    tak_commandkind     a_command_kind;
    tak_ddl_descriptor  a_is_ddl;
    pasbool             a_outer_join;
    pasbool             a_intern_explain;
    pasbool             a_pars_explain;
    tak_internal_sql_kind a_internal_sql;
    pasbool             a_from_select;
    tak_function        a_allow_functions;
    pasbool             a_rowno_allowed;
    pasbool             a_rowno_found;
    pasbool             a_special_expr;
    pasbool             a_oneval_subq_allowed;
    pasbool             a_column_factor;
    pasbool             a_join_predicate;
    pasbool             a_try_again;
    tsp00_Int2          a_select_node;
    tgg00_Surrogate     a_complex_key;
    tsp00_Int2          a_max_intern_select;
    tsp00_Int2          a_intern_select_cnt;
    tgg00_Surrogate     a_shortinfo_key;
    tsp00_Int4          a_union_cnt;
    tak_parskey         a_union_key;
    tsp00_Uint1         a_union_kind;
    pasbool             a_costcheck;
    tsp00_Int4          a_costsum;
    tak_isolation_info  a_isolation_info;
    pasbool             a_corr_select;
    tgg00_Surrogate     a_error_tableid;
    tsp00_Int4          a_errpos_offset;
    tsp00_Int2          a_parameter_offset;
    tak_jv_upd_description a_qualified_jv_upd;
    pasbool             a_rest_equal_second;
    pasbool             a_part_rollback;
    tak_rollback_info   a_rollback_info;
    tsp00_Timestamp     a_timestamp;
    tsp00_Int4          a_show_last_key;
    char                a_first_parskey;
    char                a_first_parsid;
    tsp00_Int2          a_leftpar_cnt;
    tsp00_Int2          a_rightpar_cnt;
    tsp00_Int2          a_specialpar_cnt;
    tgg00_Surrogate     a_tempinfo_key;
    tgg00_Surrogate     a_viewSchemaId;
    tsp00_KnlIdentifier a_viewname;
    tsp00_Int2          a_fromsel_n;
    pasbool             a_may_be_direct_sel;
    pasbool             a_date_time_used;
    pasbool             a_compact_varpart;
    tsp00_Uint1         a_max_codewidth;
    tsp00_KnlIdentifier a_result_name;
    tsp00_CodeType      a_mess_code;
    tak_parskey         a_corr_key;
    tak_parskey         a_curr_ex_parskey;
    tak_execution_kind  a_init_ex_kind;
    pasbool             a_in_ddl_trigger;
    tsp00_Uint1         a_iso_level;
    pasbool             a_for_fetch_only;
    pasbool             a_in_select_list;
    pasbool             a_in_installation;
    pasbool             a_parsing_again;
    tgg00_RefInfoIndex  a_statement_kind;
    tsp00_KnlIdentifier a_modul_name;
    tak_param_list      a_parameter_spec;
    tsp00_Int2          a_ap_max_node;
    pasbool             a_dynamic_sql;
    tsp1_warningset_intern a_intern_warnings;
    tak_explain_kind    a_explain_kind;
    tak_cmd_part_type   a_cpart_type;
    pasbool             a_retpart_locked;
    pasbool             a_parsingForSharedSql;
    pasbool             a_createSharedSQL;
    tsp00_Int2          a_no_of_likes;
    tsp00_Int4          a_maxnode;
    tsp00_Int4          a_data_length;
    tsp00_ResNum        a_resultnum;
    tgg00_MessBlock     a_mblock;
    tsp00_Int4          a_sample_pages;
    tak_optimize_info   a_optimize_info;
    pasbool             a_progusage_add;
    tak_proc_compile_type a_proc_compile;
    tgg00_Surrogate     a_proc_id;
    tsp00_Int4          a_show_data_cnt;
    tsp00_KernelDca     a_dca;
    tsp00_Int4          a_union_res_id;
    tak_command_states  a_sql_cmd_state;
    pasbool             a_sub_found;
    tsp00_KnlIdentifier a_expl_resname;
    tsp00_Int2          a_next_upos;
    pasbool             a_insert_select;
    pasbool             a_named_subtrans;
    pasbool             a_link_pending;
    pasbool             a_variable_input;
    tsp00_Int2          a_long_desc_pos;
    pasbool             a_execute_trigger;
    tsp00_Uint1         a_trigger_level;
    tsp00_SqlMode       a_sqlmode;
    tsp00_Int2          a_outer_n;
    tsp00_Uint1         a_dbproc_level;
    tsp00_Int2          a_col_func_count;
    tak_cmd_stack       a_cmd_stack;
    tak_sysbufferaddress a_unionrec_ptr;
    tak_sysbufferaddress a_opt_info_ptr;
    tsp00_Int2          a_err_parm_cnt;
    tsp00_C80           a_error_params;
    tsp1_part_ptr       a_uni_cmd_part;
    tsp00_Int2          a_recursive_no;
    tak_recursive_state a_recursive_state;
    pasbool             a_outstanding_packet;
    tsp00_Int4          a_mass_all_rescount;
    pasbool             a_init_info_output;
    tsp00_Int2          a_first_hint_node;
    pasbool             a_isInCppDBProcedure;
    tak_used_corr_infos a_where_corr_info;
    pasbool             a_stored_proc_call;
    pasbool             a_user_defined_error;
    tsp00_IntMapC2      a_col_file_count;
    tgg00_BasisError    a_ak_b_err;
    ARRAY_VAR_MSP00 (         a_resname_addr, tak_sysbufferaddress, cak_aux_pos - cak_extern_pos + 1);
    tsp00_Int4          a_curr_res_id;
    tak_syspointerarr   a_p_arr1;
    tak_syspointerarr   a_p_arr2;
    tak_syspointerarr   a_p_arr3;
    tak_syspointerarr   a_p_arr5;
    tak_sysbufferaddress a_ptr1;
    tak_sysbufferaddress a_ptr2;
    tak_sysbufferaddress a_ptr3;
    tak_sysbufferaddress a_ptr6;
    tak_sysbufferaddress a_ptr8;
    tak_sysbufferaddress a_ptr10;
    tsp1_packet_ptr     a_saved_packet_ptr;
    tsp00_Int4          a_saved_reply_length;
    tgg00_VoidPtr       a_planHandle;
    tgg00_VoidPtr       a_prepareHandle;
    tgg00_VoidPtr       a_longdemand_data;
    tgg00_VoidPtr       a_invalidateList;
    pasbool             a_update_table_set;
    tsp00_KnlIdentifier a_update_tableowner;
    tsp00_KnlIdentifier a_update_tablename;
    tsp1_packet_header  a_initial_packet_header;
    tsp1_segment_header a_initial_segment_header;
    tsp1_packet_header  a_cmd_packet_header;
    tsp1_segment_header a_cmd_segment_header;
    tsp00_Int2          a_main_returncode;
    tsp00_Int2          a_returncode;
    tsp00_Int4          a_main_errorpos;
    tsp00_Int4          a_errorpos;
    tsp00_Int4          a_command_id;
    tsp00_Int4          a_cmd_start_id;
    tsp00_Int4          a_resultCurrLongFileId;
    tsp00_Int4          a_resultCurrLongFileKey;
    tgg00_FileId        a_resultCurrLongFile;
    tsp00_Number        a_first_serial;
    tsp00_Number        a_last_serial;
    tgg00_FileId        a_complex_tree;
    tak_parsid          a_statement_parsid;
    tak_parskey         a_pars_last_key;
    tsp00_C8            a_replication_check_id;
    tak_queryrewrite_mode a_queryrewrite_mode;
    tak_queryrewrite_mode a_queryrewrite_done;
    tak_queryrewrite_exec a_queryrewrite_exec;
    pasbool             a_queryrewrite_modified;
    tsp00_Int4          a_original_stmt_lenght;
    pasbool             a_provoked_exception;
    pasbool             a_systemTriggerFailed;
    pasbool             a_limit_allowed;
    pasbool             a_limit_found;
    tsp00_Int2          a_limit_node;
    pasbool             a_end_of_statement_part;
    tak_ap_max_tree_ptr a_session_ap_tree;
    tsp00_MoveObjPtr    a_input_data_map;
    tsp1_packet_ptr     a_client_out_packet;
    tgg00_FileId        a_into_tree;
    tgg00_FileId        a_intern_tree;
    tgg00_FileId        a_intern_res_tree;
    tgg00_FileId        a_intern_cmd_tree;
    tgg00_FileId        a_trigger_tree;
    tgg00_QualBufPtr    a_mb_qual_addr;
    tsp00_Int4          a_mb_qual_size;
    tsp00_Int4          a_stack_size;
    tgg00_StackListPtr  a_work_st_addr;
    tsp00_MoveObjPtr    a_work_buf_addr;
    tsp00_Int4          a_work_buf_size;
    tsp00_Int2          a_work_st_max;
    tsp00_CodeType      a_defaultcode;
    tsp00_Uint1         a_session_max_codewidth;
    tsp00_CodeType      a_session_mess_code;
    tak_appl_type       a_comp_type;
    tsp00_C5            a_comp_vers;
    tsp00_C3            a_degree;
    pasbool             a_is_connected;
    pasbool             a_in_session;
    pasbool             a_db_full_connection;
    pasbool             a_curr_schema_set;
    tgg00_Surrogate     a_curr_dbprocedure;
    tgg00_Surrogate     a_init_user_id;
    tsp00_KnlIdentifier a_connected_name;
    tsp00_KnlIdentifier a_curr_user_name;
    tsp00_KnlIdentifier a_curr_schema;
    tgg00_Surrogate     a_curr_user_id;
    tgg00_Surrogate     a_curr_schema_id;
    tak_usertyp         a_current_user_kind;
    pasbool             a_is_exclusive;
    tgg00_UserRef       a_user_perm_ref;
    tgg00_Surrogate     a_acc_user_id;
    tsp00_KnlIdentifier a_acc_user;
    tsp00_KnlIdentifier a_acc_groupname;
    tgg00_Surrogate     a_acc_dba_id;
    tsp00_KnlIdentifier a_acc_dbaname;
    tsp00_Int4          a_command_sequence;
    tsp00_Int4          a_costwarn_value;
    tsp00_Int4          a_costlimit_value;
    tsp00_Int2          a_user_timeout;
    pasbool             a_use_timeout;
    pasbool             a_check_lock;
    tsp00_C3            a_ak_language;
    pasbool             a_errormsg_loading;
    tgg00_Surrogate     a_proc_userid;
    pasbool             a_pseudoUnicodeClient;
    tgg00_DateTimeFormat a_dt_format;
    tsp00_Int4          a_max_res_id;
    tgg00_FileId        a_show_curr;
    tgg00_FileId        a_pars_curr;
    tgg00_FileId        a_usage_curr;
    tak_sysdirectory    a_sysdir;
    pasbool             a_fk_check;
    tak_transinfo       a_transinf;
    tsp00_Int2          a_result_cnt;
    tgg00_Surrogate     a_temp_tablekey;
    tsp00_Int2          a_constraint_cnt;
    tsp00_Int2          a_fk_check_cnt;
    tsp00_Int2          a_restore_with_string;
    tsp00_SqlMode       a_session_sqlmode;
    tsp00_TermId        a_acc_termid;
    tak_order_errortext a_row_not_found_text;
    tsp00_Int2          a_len_row_not_found_text;
    tgg00_StackListPtr  a_stack_addr;
    tak_sysbufferaddress a_error_key_ptr;
    tak_nls_param_rec   a_nls_params;
    tak_role_context_ptr a_role_ptr;
    tak_role_info       a_role_info;
    tsp00_Int4          a_no_of_com_objects;
    tak_kernel_sink     a_kernel_sink;
    pasbool             a_release_instances;
    pasbool             a_abap_tab_comm_ok;
    tak_pcountarr       a_free_pcounts;
    tsp00_Int2          a_first_free_pcount;
    tsp00_Int2          a_last_free_pcount;
    tsp00_C3            a_max_used_pcount;
    tak_allocator_info  a_allocatorInfo;
    tak_dbproc_call_info a_dbproc_call_info;
    tsp1_packet_ptr     a_dbp_packet_ptr;
    tgg00_DataPartPtr   a_mb_data_addr;
    tsp00_MoveObjPtr    a_mb_strat_addr;
    tak_size_ptr        a_size_ptr;
    tgg00_VoidPtr       a_cpp_session_members;
    tsp00_Int4          a_mb_data_size;
    tsp00_Int4          a_mb_strat_size;
    tsp00_Int4          a_reply_packet_size;
    pasbool             a_space_option;
    pasbool             a_isReplicationUser;
    pasbool             a_isReplicationSession;
    pasbool             a_withInternalTriggers;
    pasbool             a_packetVtrace;
    pasbool             a_optimizedStreamSupport;
    pasbool             a_check_scrollableoption;
    pasbool             a_localCacheUsageOnly;
    pasbool             a_modifyBootStrapCatalog;
    pasbool             a_ascii_in_and_output;
    tsp00_C8            a_challenge;
    tsp00_Int2          a_cmd_id;
    tsp00_MoveObjPtr    a_authentication;
    pasbool             a_in_inner_trans;
    tgg00_UnivTrans     a_outer_trans;
} ;

 struct                     tak_size_info
{
    tak_size_ptr        csz_next;
    tak_acv_address     csz_acv;
    tgg91_SessionNo     csz_session;
    tsp00_Int4          csz_ak_cache;
    pasbool             csz_fill1;
    tsp00_Uint1         csz_tracelevel;
    tsp00_Int2          csz_fill2;
    tsp00_Int4          csz_alpha_fill;
} ;

typedef struct                     tak_a11_like_record
{
    tak_syspointerarr   a1l_p_arr;
    tak_privilege       a1l_privset;
    tak_columnset       a1l_likecolset;
    tak_colindex        a1l_rec_to_ext;
} tak_a11_like_record;

typedef struct                     tak_constraint_info
{
    tsp00_Int2          tree_index;
    tsp00_Int2          extcolno;
} tak_constraint_info;

typedef struct                     tak_a11_glob
{
    tgg00_Surrogate     a1authid;
    tgg00_Surrogate     a1tableid;
    tgg00_Surrogate     a1likeid;
    tsp00_KnlIdentifier a1authname;
    tsp00_KnlIdentifier a1likename;
    tsp00_KnlIdentifier a1tablen;
    tsp00_KnlIdentifier a1coln;
    tsp00_KnlIdentifier a1liketablen;
    tgg00_SysInfoKey    a1sysk;
    tgg00_SysInfoKey    a1valkey;
    tak_syspointerarr   a1p_arr;
    ARRAY_VAR_MSP00 (         a1constraint_info, tak_constraint_info, cak_max_constraints - 0 + 1);
    tak_sysbufferaddress a1colsysp;
    tak_sysbufferaddress a1sysbuf;
    tak_sysbufferaddress a1valsysp;
    tak_sysbufferaddress a1domainbuf;
    tak00_colinfo_ptr   a1colptr;
    tsp00_Int4          a1ti;
    tsp00_Int4          a1ci;
    tsp00_Int4          a1reclen;
    tsp00_Int4          a1longcharcnt;
    tsp00_Int4          a1temp_id;
    tsp00_Int2          a1foreign_key_node;
    tsp00_Int2          a1extcolno;
    tsp00_Int2          a1keypos;
    tsp00_Int2          a1strcolcount;
    tsp00_Int2          a1part2_pos;
    tsp00_Int2          a1key_node;
    tsp00_Int2          a1unique_node;
    tsp00_DataType      a1default_datatype;
    pasbool             a1inpkey;
    pasbool             a1valbufexist;
    pasbool             a1createtab;
    pasbool             a1systable;
    pasbool             a1liketable;
    pasbool             a1sort;
    pasbool             a1dynamic;
    pasbool             a1default_stamp;
    pasbool             a1global;
    pasbool             a1temp_table;
    pasbool             a1like_temp_table;
    pasbool             a1nolog;
    pasbool             a1add_rec;
    pasbool             a1for_archive;
    pasbool             a1ispacked;
    pasbool             a1clustered;
    tsp00_Int2          a1constraint_cnt;
    tsp00_Int2          a1init_colcount;
    tsp00_Int2          a1constraint_index;
    tsp00_Int2          a1serial_node;
    tsp00_KnlIdentifier a1constraint_name;
    tsp00_Int2          a1long_col_cnt;
} tak_a11_glob;

typedef struct                     tak_a21_glob
{
    tsp00_KnlIdentifier a2user_name;
    tsp00_KnlIdentifier a2like_name;
    tsp00_KnlIdentifier a2groupname;
    tsp00_Name          a2password;
    tgg00_Surrogate     a2authorizationId;
    tgg00_Surrogate     a2user_id;
    tgg00_Surrogate     a2like_id;
    tgg00_Surrogate     a2group_id;
    tgg00_SysInfoKey    a2sysk;
    tak_userpar_arr     a2params;
    tak_sysbufferaddress a2sysbuf;
    tsp00_Int4          a2errorpos;
    tsp00_Int2          a2ti;
    tsp00_Int2          a2userkind;
    pasbool             a2like;
    pasbool             a2modified;
    pasbool             a2connect;
    pasbool             a2connect_modified;
    pasbool             a2exclusive;
    pasbool             a2ex_modified;
    pasbool             a2usergroup;
    pasbool             a2cascade;
    pasbool             a2role;
    pasbool             a2defc_modified;
    pasbool             a2replication;
    pasbool             a2schema;
    pasbool             a2OmsSchema;
    pasbool             a2GroupUserAllowed;
    tsp00_CodeType      a2defaultcode;
} tak_a21_glob;

typedef struct                     tak_a25_glob
{
    tsp00_Int4          a2ti;
    tak00_colinfo_ptr   a2key_col_ptr;
    tsp00_KnlIdentifier a2linkname;
    tsp00_Int2          a2keycount;
    tak_sysbufferaddress a2primlink;
    tak_sysbufferaddress a2seclink;
    tgg00_FileId        a2resfile;
    tsp00_KnlIdentifier a2columnn;
    tsp00_Int2          a2refcol_ti;
    tgg00_Lkey          a2default;
    ARRAY_VAR_MSP00 (         a2syskey, tgg00_SysInfoKey, 2);
    ARRAY_VAR_MSP00 (         a2base_p, tak_sysbufferaddress, 2);
    ARRAY_VAR_MSP00 (         a2version, tsp00_C2, 2);
    ARRAY_VAR_MSP00 (         a2authname, tsp00_KnlIdentifier, 2);
    ARRAY_VAR_MSP00 (         a2tablen, tsp00_KnlIdentifier, 2);
    ARRAY_VAR_MSP00 (         a2fieldpos, tsp00_Int2, MAX_STRATEGY_KEYS_GG04);
    pasbool             a2create_tab;
    tsp00_C1            a2fillchar;
    tsp00_Int2          a2delact;
} tak_a25_glob;

typedef struct                     tak_a30_utility_glob
{
    tsp00_KnlIdentifier a3authname;
    tsp00_KnlIdentifier a3tablen;
    tsp00_KnlIdentifier a3coln;
    tsp00_Int2          a3ti;
    tsp00_Name          a3passw;
    tak_syspointerarr   a3p_arr;
    tgg00_FileId        a3treeid;
    tsp00_VFilename     a3hostfn;
} tak_a30_utility_glob;

typedef struct                     tak_long_descriptor
{
    union
    {
    /* 1 */
        struct tak_long_descriptor_1
        {
            tsp00_LongDescriptor desc_F;
        } C_1;
    /* 2 */
        struct tak_long_descriptor_2
        {
            tgg00_Surrogate     lds_descriptor_F;
            tgg00_Surrogate     lds_tabid_F;
            tsp00_Int4          lds_maxlen_F;
            tsp00_Int4          lds_intern_pos_F;
            tsp00_LdInfo        lds_infoset_F;
            tsp00_LdState       lds_state_F;
            tgg00_ShowKind      lds_show_kind_F;
            tsp00_ValMode       lds_valmode_F;
            tsp00_Int2          lds_valind_F;
            tsp00_Int2          lds_colno_F;
            tsp00_Int4          lds_valpos_F;
            tsp00_Int4          lds_vallen_F;
        } C_2;
    /* 3 */
        struct tak_long_descriptor_3
        {
            tgg00_Surrogate     ldc_descriptor_F;
            tgg00_Surrogate     ldc_tabid_F;
            tsp00_Int4          ldc_maxlen_F;
            tsp00_Int4          ldc_intern_pos_F;
            tsp00_LdInfo        ldc_infoset_F;
            tsp00_LdState       ldc_state_F;
            tak_comment_type    ldc_comment_type_F;
        } C_3;
    /* 4 */
        struct tak_long_descriptor_4
        {
            tgg00_Surrogate     lds_descriptor1_F;
            tgg00_Surrogate     lds_tabid1_F;
            tgg00_Surrogate     lds_copyTabid_F;
            tsp00_LdInfo        lds_infoset1_F;
            tsp00_LdState       lds_state1_F;
            tgg00_ShowKind      lds_show_kind1_F;
            tsp00_ValMode       lds_valmode1_F;
            tsp00_Int2          lds_valind1_F;
            tsp00_Int2          lds_colno1_F;
            tgg00_Surrogate     lds_copyLongId_F;
        } C_4;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tsp00_LongDescriptor &desc                   ()  { return this->variant.C_1.desc_F; };
    tgg00_Surrogate    &lds_descriptor         ()  { return this->variant.C_2.lds_descriptor_F; };
    tgg00_Surrogate    &lds_tabid              ()  { return this->variant.C_2.lds_tabid_F; };
    tsp00_Int4         &lds_maxlen             ()  { return this->variant.C_2.lds_maxlen_F; };
    tsp00_Int4         &lds_intern_pos         ()  { return this->variant.C_2.lds_intern_pos_F; };
    tsp00_LdInfo       &lds_infoset            ()  { return this->variant.C_2.lds_infoset_F; };
    tsp00_LdState      &lds_state              ()  { return this->variant.C_2.lds_state_F; };
    tgg00_ShowKind     &lds_show_kind          ()  { return this->variant.C_2.lds_show_kind_F; };
    tsp00_ValMode      &lds_valmode            ()  { return this->variant.C_2.lds_valmode_F; };
    tsp00_Int2         &lds_valind             ()  { return this->variant.C_2.lds_valind_F; };
    tsp00_Int2         &lds_colno              ()  { return this->variant.C_2.lds_colno_F; };
    tsp00_Int4         &lds_valpos             ()  { return this->variant.C_2.lds_valpos_F; };
    tsp00_Int4         &lds_vallen             ()  { return this->variant.C_2.lds_vallen_F; };
    tgg00_Surrogate    &ldc_descriptor         ()  { return this->variant.C_3.ldc_descriptor_F; };
    tgg00_Surrogate    &ldc_tabid              ()  { return this->variant.C_3.ldc_tabid_F; };
    tsp00_Int4         &ldc_maxlen             ()  { return this->variant.C_3.ldc_maxlen_F; };
    tsp00_Int4         &ldc_intern_pos         ()  { return this->variant.C_3.ldc_intern_pos_F; };
    tsp00_LdInfo       &ldc_infoset            ()  { return this->variant.C_3.ldc_infoset_F; };
    tsp00_LdState      &ldc_state              ()  { return this->variant.C_3.ldc_state_F; };
    tak_comment_type   &ldc_comment_type       ()  { return this->variant.C_3.ldc_comment_type_F; };
    tgg00_Surrogate    &lds_descriptor1        ()  { return this->variant.C_4.lds_descriptor1_F; };
    tgg00_Surrogate    &lds_tabid1             ()  { return this->variant.C_4.lds_tabid1_F; };
    tgg00_Surrogate    &lds_copyTabid          ()  { return this->variant.C_4.lds_copyTabid_F; };
    tsp00_LdInfo       &lds_infoset1           ()  { return this->variant.C_4.lds_infoset1_F; };
    tsp00_LdState      &lds_state1             ()  { return this->variant.C_4.lds_state1_F; };
    tgg00_ShowKind     &lds_show_kind1         ()  { return this->variant.C_4.lds_show_kind1_F; };
    tsp00_ValMode      &lds_valmode1           ()  { return this->variant.C_4.lds_valmode1_F; };
    tsp00_Int2         &lds_valind1            ()  { return this->variant.C_4.lds_valind1_F; };
    tsp00_Int2         &lds_colno1             ()  { return this->variant.C_4.lds_colno1_F; };
    tgg00_Surrogate    &lds_copyLongId         ()  { return this->variant.C_4.lds_copyLongId_F; };
    /* const accessor functions */
    const tsp00_LongDescriptor &desc                   () const  { return this->variant.C_1.desc_F; };
    const tgg00_Surrogate &lds_descriptor         () const  { return this->variant.C_2.lds_descriptor_F; };
    const tgg00_Surrogate &lds_tabid              () const  { return this->variant.C_2.lds_tabid_F; };
    const tsp00_Int4   &lds_maxlen             () const  { return this->variant.C_2.lds_maxlen_F; };
    const tsp00_Int4   &lds_intern_pos         () const  { return this->variant.C_2.lds_intern_pos_F; };
    const tsp00_LdInfo &lds_infoset            () const  { return this->variant.C_2.lds_infoset_F; };
    const tsp00_LdState &lds_state              () const  { return this->variant.C_2.lds_state_F; };
    const tgg00_ShowKind &lds_show_kind          () const  { return this->variant.C_2.lds_show_kind_F; };
    const tsp00_ValMode &lds_valmode            () const  { return this->variant.C_2.lds_valmode_F; };
    const tsp00_Int2   &lds_valind             () const  { return this->variant.C_2.lds_valind_F; };
    const tsp00_Int2   &lds_colno              () const  { return this->variant.C_2.lds_colno_F; };
    const tsp00_Int4   &lds_valpos             () const  { return this->variant.C_2.lds_valpos_F; };
    const tsp00_Int4   &lds_vallen             () const  { return this->variant.C_2.lds_vallen_F; };
    const tgg00_Surrogate &ldc_descriptor         () const  { return this->variant.C_3.ldc_descriptor_F; };
    const tgg00_Surrogate &ldc_tabid              () const  { return this->variant.C_3.ldc_tabid_F; };
    const tsp00_Int4   &ldc_maxlen             () const  { return this->variant.C_3.ldc_maxlen_F; };
    const tsp00_Int4   &ldc_intern_pos         () const  { return this->variant.C_3.ldc_intern_pos_F; };
    const tsp00_LdInfo &ldc_infoset            () const  { return this->variant.C_3.ldc_infoset_F; };
    const tsp00_LdState &ldc_state              () const  { return this->variant.C_3.ldc_state_F; };
    const tak_comment_type &ldc_comment_type       () const  { return this->variant.C_3.ldc_comment_type_F; };
    const tgg00_Surrogate &lds_descriptor1        () const  { return this->variant.C_4.lds_descriptor1_F; };
    const tgg00_Surrogate &lds_tabid1             () const  { return this->variant.C_4.lds_tabid1_F; };
    const tgg00_Surrogate &lds_copyTabid          () const  { return this->variant.C_4.lds_copyTabid_F; };
    const tsp00_LdInfo &lds_infoset1           () const  { return this->variant.C_4.lds_infoset1_F; };
    const tsp00_LdState &lds_state1             () const  { return this->variant.C_4.lds_state1_F; };
    const tgg00_ShowKind &lds_show_kind1         () const  { return this->variant.C_4.lds_show_kind1_F; };
    const tsp00_ValMode &lds_valmode1           () const  { return this->variant.C_4.lds_valmode1_F; };
    const tsp00_Int2   &lds_valind1            () const  { return this->variant.C_4.lds_valind1_F; };
    const tsp00_Int2   &lds_colno1             () const  { return this->variant.C_4.lds_colno1_F; };
    const tgg00_Surrogate &lds_copyLongId         () const  { return this->variant.C_4.lds_copyLongId_F; };
#endif    /* defined (__cplusplus) */
} tak_long_descriptor;

typedef enum tak001_ldb1_change__Enum
{
    ldb1_unused_was_use_termchar,
    ldb1_use_conversion
} tak001_ldb1_change__Enum;
typedef tak001_ldb1_change__Enum tak001_ldb1_change__Param;

typedef struct                     tak_long_desc_block
{
    union
    {
    /* 1 */
        struct tak_long_desc_block_1
        {
            OLDPASCALSTRING_VAR_MSP00 (  ldb_desc_F, LONG_DESC_BLOCK_MXSP00);
        } C_1;
    /* 2 */
        struct tak_long_desc_block_2
        {
            tgg00_Surrogate     ldb_descriptor_F;
            tsp00_Int2          ldb_filler2_F;
            tsp00_LdbChange     ldb_change_F;
            pasbool             ldb_filler3_F;
            tsp00_Int4          ldb_curr_pageno_F;
            tsp00_Int2          ldb_curr_pos_F;
            tsp00_C1            ldb_filler0_F;
            tgg00_ShowKind      ldb_show_kind_F;
            tsp00_Int4          ldb_intern_pos_F;
            tsp00_LdInfo        ldb_infoset_F;
            pasbool             ldb_filler1_F;
            tsp00_C1            ldb_valind_F;
            tsp00_ValMode       ldb_valmode_F;
            tsp00_Int2          ldb_filler5_F;
            tsp00_Int2          ldb_filler6_F;
            tsp00_Int4          ldb_valpos_F;
            tsp00_Int4          ldb_vallen_F;
            tsp00_Int4          ldb_full_len_F;
            tsp00_Int4          ldb_filler4_F;
            tsp00_Int4          ldb_last_i_pos_F;
            tsp00_C4            ldb_root_check_F;
        } C_2;
    /* 3 */
        struct tak_long_desc_block_3
        {
            tgg00_Surrogate     ldb1_descriptor_F;
            tsp00_Int2          ldb1_filler2_F;
            SMALLSET_VAR_MSP00 (      ldb1_change_F, tak001_ldb1_change__Enum, ldb1_use_conversion);
            pasbool             ldb1_filler3_F;
            tsp00_Int4          ldb_colno_F;
            tsp00_Int2          ldb1_curr_pos_F;
            tsp00_C1            ldb1_colno_F;
            tak_comment_type    ldb1_comment_type_F;
        } C_3;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tsp00_OldPascalString<LONG_DESC_BLOCK_MXSP00> &ldb_desc               ()  { return this->variant.C_1.ldb_desc_F; };
    tgg00_Surrogate    &ldb_descriptor         ()  { return this->variant.C_2.ldb_descriptor_F; };
    tsp00_Int2         &ldb_filler2            ()  { return this->variant.C_2.ldb_filler2_F; };
    tsp00_LdbChange    &ldb_change             ()  { return this->variant.C_2.ldb_change_F; };
    pasbool            &ldb_filler3            ()  { return this->variant.C_2.ldb_filler3_F; };
    tsp00_Int4         &ldb_curr_pageno        ()  { return this->variant.C_2.ldb_curr_pageno_F; };
    tsp00_Int2         &ldb_curr_pos           ()  { return this->variant.C_2.ldb_curr_pos_F; };
    tsp00_C1           &ldb_filler0            ()  { return this->variant.C_2.ldb_filler0_F; };
    tgg00_ShowKind     &ldb_show_kind          ()  { return this->variant.C_2.ldb_show_kind_F; };
    tsp00_Int4         &ldb_intern_pos         ()  { return this->variant.C_2.ldb_intern_pos_F; };
    tsp00_LdInfo       &ldb_infoset            ()  { return this->variant.C_2.ldb_infoset_F; };
    pasbool            &ldb_filler1            ()  { return this->variant.C_2.ldb_filler1_F; };
    tsp00_C1           &ldb_valind             ()  { return this->variant.C_2.ldb_valind_F; };
    tsp00_ValMode      &ldb_valmode            ()  { return this->variant.C_2.ldb_valmode_F; };
    tsp00_Int2         &ldb_filler5            ()  { return this->variant.C_2.ldb_filler5_F; };
    tsp00_Int2         &ldb_filler6            ()  { return this->variant.C_2.ldb_filler6_F; };
    tsp00_Int4         &ldb_valpos             ()  { return this->variant.C_2.ldb_valpos_F; };
    tsp00_Int4         &ldb_vallen             ()  { return this->variant.C_2.ldb_vallen_F; };
    tsp00_Int4         &ldb_full_len           ()  { return this->variant.C_2.ldb_full_len_F; };
    tsp00_Int4         &ldb_filler4            ()  { return this->variant.C_2.ldb_filler4_F; };
    tsp00_Int4         &ldb_last_i_pos         ()  { return this->variant.C_2.ldb_last_i_pos_F; };
    tsp00_C4           &ldb_root_check         ()  { return this->variant.C_2.ldb_root_check_F; };
    tgg00_Surrogate    &ldb1_descriptor        ()  { return this->variant.C_3.ldb1_descriptor_F; };
    tsp00_Int2         &ldb1_filler2           ()  { return this->variant.C_3.ldb1_filler2_F; };
    tsp00_SmallSet<tak001_ldb1_change__Enum, ldb1_use_conversion> &ldb1_change            ()  { return this->variant.C_3.ldb1_change_F; };
    pasbool            &ldb1_filler3           ()  { return this->variant.C_3.ldb1_filler3_F; };
    tsp00_Int4         &ldb_colno              ()  { return this->variant.C_3.ldb_colno_F; };
    tsp00_Int2         &ldb1_curr_pos          ()  { return this->variant.C_3.ldb1_curr_pos_F; };
    tsp00_C1           &ldb1_colno             ()  { return this->variant.C_3.ldb1_colno_F; };
    tak_comment_type   &ldb1_comment_type      ()  { return this->variant.C_3.ldb1_comment_type_F; };
    /* const accessor functions */
    const tsp00_OldPascalString<LONG_DESC_BLOCK_MXSP00> &ldb_desc               () const  { return this->variant.C_1.ldb_desc_F; };
    const tgg00_Surrogate &ldb_descriptor         () const  { return this->variant.C_2.ldb_descriptor_F; };
    const tsp00_Int2   &ldb_filler2            () const  { return this->variant.C_2.ldb_filler2_F; };
    const tsp00_LdbChange &ldb_change             () const  { return this->variant.C_2.ldb_change_F; };
    const pasbool      &ldb_filler3            () const  { return this->variant.C_2.ldb_filler3_F; };
    const tsp00_Int4   &ldb_curr_pageno        () const  { return this->variant.C_2.ldb_curr_pageno_F; };
    const tsp00_Int2   &ldb_curr_pos           () const  { return this->variant.C_2.ldb_curr_pos_F; };
    const tsp00_C1     &ldb_filler0            () const  { return this->variant.C_2.ldb_filler0_F; };
    const tgg00_ShowKind &ldb_show_kind          () const  { return this->variant.C_2.ldb_show_kind_F; };
    const tsp00_Int4   &ldb_intern_pos         () const  { return this->variant.C_2.ldb_intern_pos_F; };
    const tsp00_LdInfo &ldb_infoset            () const  { return this->variant.C_2.ldb_infoset_F; };
    const pasbool      &ldb_filler1            () const  { return this->variant.C_2.ldb_filler1_F; };
    const tsp00_C1     &ldb_valind             () const  { return this->variant.C_2.ldb_valind_F; };
    const tsp00_ValMode &ldb_valmode            () const  { return this->variant.C_2.ldb_valmode_F; };
    const tsp00_Int2   &ldb_filler5            () const  { return this->variant.C_2.ldb_filler5_F; };
    const tsp00_Int2   &ldb_filler6            () const  { return this->variant.C_2.ldb_filler6_F; };
    const tsp00_Int4   &ldb_valpos             () const  { return this->variant.C_2.ldb_valpos_F; };
    const tsp00_Int4   &ldb_vallen             () const  { return this->variant.C_2.ldb_vallen_F; };
    const tsp00_Int4   &ldb_full_len           () const  { return this->variant.C_2.ldb_full_len_F; };
    const tsp00_Int4   &ldb_filler4            () const  { return this->variant.C_2.ldb_filler4_F; };
    const tsp00_Int4   &ldb_last_i_pos         () const  { return this->variant.C_2.ldb_last_i_pos_F; };
    const tsp00_C4     &ldb_root_check         () const  { return this->variant.C_2.ldb_root_check_F; };
    const tgg00_Surrogate &ldb1_descriptor        () const  { return this->variant.C_3.ldb1_descriptor_F; };
    const tsp00_Int2   &ldb1_filler2           () const  { return this->variant.C_3.ldb1_filler2_F; };
    const tsp00_SmallSet<tak001_ldb1_change__Enum, ldb1_use_conversion> &ldb1_change            () const  { return this->variant.C_3.ldb1_change_F; };
    const pasbool      &ldb1_filler3           () const  { return this->variant.C_3.ldb1_filler3_F; };
    const tsp00_Int4   &ldb_colno              () const  { return this->variant.C_3.ldb_colno_F; };
    const tsp00_Int2   &ldb1_curr_pos          () const  { return this->variant.C_3.ldb1_curr_pos_F; };
    const tsp00_C1     &ldb1_colno             () const  { return this->variant.C_3.ldb1_colno_F; };
    const tak_comment_type &ldb1_comment_type      () const  { return this->variant.C_3.ldb1_comment_type_F; };
#endif    /* defined (__cplusplus) */
} tak_long_desc_block;
    /**************************************************************/
    /*                       DML_PART                             */
    /**************************************************************/

typedef enum tak_lockenum_Enum
{
    no_lock,
    read_lock,
    write_lock,
    no_lock_string,
    optimistic_lock
} tak_lockenum_Enum;
typedef tak_lockenum_Enum tak_lockenum_Param;
typedef ENUM_TYPE_MSP00 (          tak_lockenum, tak_lockenum_Enum, optimistic_lock, unsigned char);

typedef struct                     tak_corfields
{
    tak_all_corcolumns  cfields;
    tsp00_Int2          cused_fields;
    tsp00_Int2          ccfill;
} tak_corfields;

typedef enum tak_needed_spec_Enum
{
    ons_with_username,
    ons_only_tablename
} tak_needed_spec_Enum;
typedef tak_needed_spec_Enum tak_needed_spec_Param;
typedef ENUM_TYPE_MSP00 (          tak_needed_spec, tak_needed_spec_Enum, ons_only_tablename, unsigned char);

typedef struct                     tak_one_table
{
    tsp00_KnlIdentifier ouser;
    tsp00_KnlIdentifier otable;
    tsp00_KnlIdentifier oreference;
    tgg00_Surrogate     ofromtableid;
    tgg00_FileId        otreeid;
    tsp00_Int4          opages;
    pasbool             oall_priv;
    tak_one_table_specialname ospecialname;
    pasbool             oview;
    pasbool             oviewqual;
    pasbool             oviewcheck;
    pasbool             ocomplex_view;
    tsp00_Int2          otab_node;
    tak_needed_spec     ospecs_needed;
    tsp00_Uint1         ounused2;
    tak_table_attribute_set oattributes;
    tsp00_Uint1         ounused0;
    tsp00_Int2          ocounttabs;
    tsp00_Int2          ounused1;
    tak_columnset       oprivset;
    tak_columnset       osetallpriv;
} tak_one_table;

typedef struct                     tak_dml_onefield
{
    tak00_colinfo_ptr   dml_col_ptr;
    tsp00_Int2          dml_index;
    tsp00_Int2          dml_colno_in_subquery;
    tsp00_Int2          dml_node;
    tsp00_Uint1         dml_res_index;
    tsp00_Uint1         dml_fill;
} tak_dml_onefield;

typedef enum tak_tab_allow_state_Enum
{
    only_one_tab,
    may_more_tabs,
    are_more_tabs,
    tabs_with_arith
} tak_tab_allow_state_Enum;
typedef tak_tab_allow_state_Enum tak_tab_allow_state_Param;
typedef ENUM_TYPE_MSP00 (          tak_tab_allow_state, tak_tab_allow_state_Enum, tabs_with_arith, unsigned char);
typedef ARRAY_TYPE_MSP00 (         tak_all_from_tables, tak_one_table, cak00_maxsources);
typedef tak_all_from_tables       *tak_all_from_tables_ptr;

typedef enum tak_corr_type_Enum
{
    no_correlation,
    correlation,
    first_correlation,
    lowest_of_correlation,
    having_correlation
} tak_corr_type_Enum;
typedef tak_corr_type_Enum tak_corr_type_Param;
typedef ENUM_TYPE_MSP00 (          tak_corr_type, tak_corr_type_Enum, having_correlation, unsigned char);

typedef enum tak_inv_only_Enum
{
    ind_init,
    ind_no_inv,
    ind_inv
} tak_inv_only_Enum;
typedef tak_inv_only_Enum tak_inv_only_Param;
typedef ENUM_TYPE_MSP00 (          tak_inv_only, tak_inv_only_Enum, ind_inv, unsigned char);

typedef struct                     tak_dml_info
{
    tak00_colinfo_ptr   d_colptr;
    tak00_colinfo_ptr   d_colbuf;
    tak_syspointerarr   d_esparr;
    tsp00_BufAddr       d_resbuf_addr;

#if defined (BIT64)


#else

    OLDPASCALSTRING_VAR_MSP00 (  d_alpha_fill1, 68);

#endif

    tsp00_KnlIdentifier d_user;
    tsp00_KnlIdentifier d_table;
    tsp00_KnlIdentifier d_column;
    tsp00_KnlIdentifier d_refname;
    tak_columnset       d_upd_set;
    tak_columnset       d_foundset;
    tsp00_Int2          d_param_st_index;
    tsp00_Int2          d_param_st_begin;
    tsp00_Int2          d_first_tab;
    tsp00_C2            d_udt_datatype;
    tsp00_Int2          d_pargeslen;
    tsp00_Int2          d_movebefore;
    tsp00_Int2          d_index;
    tsp00_Int2          d_p_filler1;
    tsp00_Int4          d_vppos;
    tsp00_Int2          d_exprno;
    tsp00_Int2          d_cntfromtab;
    tsp00_Int2          d_acttabindex;
    tsp00_Int2          d_fieldno;
    tsp00_Int2          d_keylen;
    tsp00_Int2          d_outcolno;
    tsp00_Int4          d_maxlen;
    tsp00_Int4          d_pos_in_parsbuf;
    tsp00_Int4          d_inoutpos;
    tsp00_Int2          d_parameter_count;
    tsp00_Int2          d_parnum;
    pasbool             d_checkview;
    pasbool             d_join_dml;
    tsp00_Int2          d_colindex;
    tsp00_Int2          d_reclen;
    pasbool             d_standard;
    pasbool             d_wherepart;
    pasbool             d_range;
    pasbool             d_nullkey;
    pasbool             d_like;
    pasbool             d_key;
    pasbool             d_single;
    pasbool             d_use_order;
    pasbool             d_arith_where;
    pasbool             d_join;
    pasbool             d_reuse;
    pasbool             d_group;
    pasbool             d_having;
    tsp00_DataType      d_datatype;
    tak_tab_allow_state d_allowed;
    tak_fp_kind_type    d_pars_kind;
    tsp00_Int2          d_maxcounttabs;
    tsp00_Int2          d_basetabno;
    tak_corr_type       d_corr;
    tsp00_Uint1         d_lowpars;
    tak_corlevel        d_level;
    pasbool             d_align_fill1;
    tsp00_Uint1         d_oldlowpars;
    pasbool             d_prep_corr_cols;
    pasbool             d_like_expression;
    tsp00_Int4          d_n_pos;
    tsp00_Int2          d_filled_bytes;
    tsp00_C2            d_escape_char;
    tsp00_Int2          d_expand;
    pasbool             d_concat;
    pasbool             d_hex;
    pasbool             d_first_union;
    pasbool             d_change_date_time;
    pasbool             d_one_join_phase;
    pasbool             d_check_null_byte;
    pasbool             d_const_value_expr;
    pasbool             d_is_parameter_only;
    tgg00_RefInfoIndex  d_strat_info;
    pasbool             d_truncate;
    tsp00_Int2          d_topos;
    pasbool             d_in_method_param;
    pasbool             d_escape;
    tsp00_Int4          d_mselect_rescnt;
    pasbool             d_execute_unlock;
    pasbool             d_pseudo_ins_select;
    tsp00_C2            d_viewdesc_linkage;
    tak_inv_colnos      d_inv_colno;
    tsp00_KnlIdentifier d_resname;
    tak_joinset         d_oj_tables;
    pasbool             d_all_count;
    pasbool             d_repl_reskey;
    tsp00_Int2          d_mselect_loop_cnt;
    tsp00_Uint1         d_inv_cnt;
    pasbool             d_single_expr;
    pasbool             d_only_table_scan;
    pasbool             d_in_case_function;
    pasbool             d_where_corr;
    pasbool             d_where_subquery;
    pasbool             d_where_corr_subquery;
    pasbool             d_having_subquery;
    pasbool             d_having_corr_subquery;
    pasbool             d_type_long_allowed;
    pasbool             d_optimize_setfunction;
    pasbool             d_specialjoinfromselect;
    tak_syspointerarr   d_sparr;
    tak_sysbufferaddress d_linkbuf;
    tak_sysbufferaddress d_viewtextbuf;
    tak_sysbufferaddress d_viewdescbuf;
    tak_sysbufferaddress d_literalrec;

#if defined (BIT64)


#else

    OLDPASCALSTRING_VAR_MSP00 (  d_alpha_fill2, 72);

#endif

    tgg00_Surrogate     d_tableid;
    tgg00_Surrogate     d_table_as_id;
    tsp00_Int2          d_act_node;
    tsp00_Int2          d_cntpar;
    tgg00_HandlingSet   d_globstate;
    tgg00_HandlingSet   d_unchanged_globstate;
    tsp00_Int4          d_limit_offset;
    tsp00_Int4          d_upper_limit;
    tsp00_Int4          d_rowno;
    tsp00_Int2          d_pos_result;
    pasbool             d_resname_found;
    pasbool             d_use_sub;
    tsp00_KnlIdentifier d_viewusername;
    tsp00_KnlIdentifier d_viewtablen;
    pasbool             d_subquery;
    tsp00_Uint1         d_sublevel;
    tsp00_Int2          d_subcount;
    tsp00_Int2          d_phase_cnt;
    pasbool             d_only_sem_check;
    pasbool             d_union;
    tsp00_Int2          d_union_order_n;
    pasbool             d_union_insert;
    pasbool             d_outer_join;
    pasbool             d_repl_nokey_ins_sel;
    pasbool             d_is_function_column;
    tsp00_Int2          d_global_pos_result;
    pasbool             d_longdesc_found;
    pasbool             d_unused1;
    tsp00_Int2          d_dist_optim;
    tgg00_QualKind      d_qual_kind;
    pasbool             d_view_col_list;
    tsp00_Int2          d_view_col_node;
    tsp00_MoveObjPtr    d_massdata_ptr;

#if defined (BIT64)


#else

    OLDPASCALSTRING_VAR_MSP00 (  d_alpha_fill4, 4);

#endif

    pasbool             d_local_massdata_part;
    pasbool             d_further_fill1;
    tsp00_Int2          d_longlitcount;
    tgg04_Distinct      d_distinct;
    pasbool             d_view;
    tsp00_Int2          d_union_limit_n;
    tak_parskey         d_initial_ex_parskey;
    tak_all_from_tables_ptr d_tabarr;

#if defined (BIT64)


#else

    OLDPASCALSTRING_VAR_MSP00 (  d_alpha_fill5, 4);

#endif

    tsp00_Int4          d_tabarr_capacity;
    tsp00_Int4          d_fill5;
    tak_joinrec         d_joins;
    tak_changerecord    d_change;
    tsp00_Int4          d_change_align;
    tak00_ord_fields    d_order_cols;
    tak00_ord_fields    d_group_cols;
    tak00_ord_fields_ptr d_order_or_group_cols;

#if defined (BIT64)


#else

    OLDPASCALSTRING_VAR_MSP00 (  d_alpha_fill3, 4);

#endif

    tsp00_DataType      d_ch_datatype;
    pasbool             d_like_optimize;
    tsp00_Uint1         d_like_part;
    pasbool             d_string_column;
    pasbool             d_unused11;
    pasbool             d_with_lock_clause;
    tsp00_Int2          d_first_longdesc_pos;
    tsp00_Int2          d_cnt_longdesc;
    tsp00_Int2          d_subquery_node;
    tsp00_Int2          d_fromtabnode;
    tsp00_Int2          d_columnindex;
    tgg00_Surrogate     d_ins_sel_tabid;
    ARRAY_VAR_MSP00 (         d_init_tabarr, tak_one_table, cak_init_join_tab_size);
    tak_one_join        d_jrc_initial_arr;
    tsp00_Int4          d_align_fill3;
} tak_dml_info;
typedef OLDPASCALSTRING_TYPE_MSP00 (  tak_part_dml_info, mxak_full_dml_init);
typedef CSTRING_TYPE_MSP00      (  tak_part_dml_infoc, mxak_full_dml_init);
typedef ARRAY_TYPE_MSP00 (         tak_dfarr, tak_dml_onefield, MAX_COL_PER_TAB_GG00);

typedef struct                     tak_select_record
{
    tgg00_FileId        sel_res_tree;
    tsp00_KnlIdentifier sel_res_name;
    tsp00_Int2          sel_output_n;
    tsp00_Int2          sel_into_n;
    tsp00_Int2          sel_from_n;
    tsp00_Int2          sel_group_n;
    tsp00_Int2          sel_having_n;
    tsp00_Int2          sel_order_n;
    tsp00_Int2          sel_limit_n;
    tsp00_Int2          sel_res_b;
    tsp00_Int2          sel_parinf_b;
    pasbool             sel_is_not_corr_search;
    tgg04_Distinct      sel_new_atdistinct;
    pasbool             sel_last_pars_part;
    pasbool             sel_where_done;
    pasbool             sel_view_done;
    pasbool             sel_has_subquery;
} tak_select_record;
typedef ARRAY_TYPE_MSP00 (         tak_pageno_array, tsp00_Int4, MAX_COL_PER_TAB_GG00);

typedef enum tak_fs_value_qual_Enum
{
    c_startpos,
    c_endpos,
    c_length4,
    c_length2
} tak_fs_value_qual_Enum;
typedef tak_fs_value_qual_Enum tak_fs_value_qual_Param;
typedef ENUM_TYPE_MSP00 (          tak_fs_value_qual, tak_fs_value_qual_Enum, c_length2, unsigned char);

typedef enum tak_fields_to_use_Enum
{
    no_fields,
    keyfields,
    corr_fields
} tak_fields_to_use_Enum;
typedef tak_fields_to_use_Enum tak_fields_to_use_Param;
typedef ENUM_TYPE_MSP00 (          tak_fields_to_use, tak_fields_to_use_Enum, corr_fields, unsigned char);

typedef struct                     tak_corr_infos
{
    tgg00_FileId        co_corr_current;
    tsp00_BufAddr       co_corr_buf;
    pasbool             co_use_cinfo;
    tak_fields_to_use   co_use_fields;
    tsp00_Int2          co_all_recs_len;
    tsp00_Int2          co_one_rec_len;
    tsp00_Int2          co_one_key_len;
    tsp00_Int2          co_lcomp_len;
    tsp00_Int2          co_startkeyfields;
    tsp00_Int2          co_act_pos;
    tsp00_Int2          co_sel_keypos;
    tsp00_Int2          co_upd_keylen;
    tsp00_Int2          co_minkeylen;
    tsp00_Int2          co_varkeyvalstack;
    tsp00_Int2          co_fill;
} tak_corr_infos;

typedef struct                     tak_fill_rec
{
    tsp00_Int2          fr_f_no;
    tsp00_Int2          fr_last_fno;
    tsp00_Int2          fr_total_leng;
    tsp00_Int2          fr_leng;
    tsp00_C2            fr_uni_fill_c2;
    tsp00_Int2          fr_filler0;
} tak_fill_rec;

typedef struct                     tak_exec_rec
{
    tgg00_FileId        ex_act_treeid;
    tsp00_Int4          ex_all_rescnt;
    tsp00_Int2          ex_plus;
    tsp00_Int2          ex_maxbufs;
    pasbool             ex_with_link;
    pasbool             ex_output_wanted;
    pasbool             ex_command_needed;
    pasbool             ex_fill;
    tgg00_DataPartPtr   ex_mb_data;
    tsp00_MoveObjPtr    ex_updjv_ptr;
    tsp00_MoveObjPtr    ex_parameter_fieldlist;
    tsp00_MoveObjPtr    ex_result_fieldlist;
} tak_exec_rec;

typedef struct                     tak_factorrec
{
    integer             fr_minus_n;
    integer             fr_jpos;
    pasbool             fr_error;
    pasbool             fr_describ;
    pasbool             fr_minus_symb;
    pasbool             fr_getnumb;
} tak_factorrec;

typedef struct                     tak_litcoldesc
{
    tgg00_Surrogate     llclongid;
    tsp00_Int4          llcdatapos;
    tsp00_Int4          llcdatalen;
    pasbool             llwith_shortcol;
    pasbool             llfiller1;
    tsp00_Int2          llfiller2;
    tsp00_Int4          llfiller3;
} tak_litcoldesc;
typedef ARRAY_TYPE_MSP00 (         tak_litcoldesc_arr, tak_litcoldesc, cak_max_litcoldesc);
typedef OLDPASCALSTRING_TYPE_MSP00 (  tak_longlit_buf, mxak_longlit_buf);
typedef CSTRING_TYPE_MSP00      (  tak_longlit_bufc, mxak_longlit_buf);

typedef struct                     tak_longliteral_rec
{
    tsp00_Int2          litreclen;
    tsp00_Int2          litkeylen;
    tsp00_Int2          litvarcol_offset;
    tsp00_Int2          litvarcol_cnt;
    tgg00_Surrogate     litsurrogate;
    tsp00_C2            litentrytype;
    tsp00_C2            litlinkage;
    tsp00_Int4          litfullen;
    tsp00_Int4          litfiller;
    tgg00_Surrogate     littabid;
    tsp00_Int2          litcolcount;
    tsp00_Int2          litfiller1;
    tsp00_Int4          litlen;
    tsp00_Int4          litsize;
    tsp00_MoveObjPtr    litdataptr;

#if defined (BIT64)


#else

    tsp00_Int4          libit64fill;

#endif

    union
    {
    /* true */
        struct tak_longliteral_rec_true
        {
            tak_litcoldesc_arr  litcoldesc_F;
        } C_true;
    /* false */
        struct tak_longliteral_rec_false
        {
            tak_longlit_buf     litdatabuf_F;
        } C_false;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tak_litcoldesc_arr &litcoldesc             ()  { return this->variant.C_true.litcoldesc_F; };
    tak_longlit_buf    &litdatabuf             ()  { return this->variant.C_false.litdatabuf_F; };
    /* const accessor functions */
    const tak_litcoldesc_arr &litcoldesc             () const  { return this->variant.C_true.litcoldesc_F; };
    const tak_longlit_buf &litdatabuf             () const  { return this->variant.C_false.litdatabuf_F; };
#endif    /* defined (__cplusplus) */
} tak_longliteral_rec;

typedef struct                     tak_longdemand_rec
{
    tsp00_Int4          ldem_position;
    tsp00_Int4          ldem_length;
    pasbool             ldem_getatonce;
} tak_longdemand_rec;
typedef tak_longdemand_rec        *tak_longdemand_ptr;

typedef struct                     tak_hintrecord
{
    tsp00_Int2          hint_reclen;
    tsp00_Int2          hint_keylen;
    tsp00_Int2          hint_varcol_offset;
    tsp00_Int2          hint_varcol_cnt;
    tgg00_Surrogate     hint_surrogate;
    tsp00_C2            hint_entrytype;
    tsp00_C2            hint_linkage;
    tsp00_Int2          hint_filler;
    tsp00_Int2          hint_joincfg_cnt;
    tak00_access_configuration hint_common_config;
    tak00_join_configuration_arr hint_join_config;
} tak_hintrecord;

 struct                     tak_systembuffer
{
    union
    {
    /* 0 */
        struct tak_systembuffer_0
        {
            tsp00_Int2          b_sl_F;
            tsp00_Int2          b_kl_F;
            tsp00_Int2          b_varcol_offset_F;
            tsp00_Int2          b_varcol_cnt_F;
        } C_0;
    /* 1 */
        struct tak_systembuffer_1
        {
            tak_baserecord      sbase_F;
        } C_1;
    /* 2 */
        struct tak_systembuffer_2
        {
            tak_comment_record  scomment_F;
        } C_2;
    /* 3 */
        struct tak_systembuffer_3
        {
            tak_complexrecord   scomplexrec_F;
        } C_3;
    /* 4 */
        struct tak_systembuffer_4
        {
            tak_constraintrecord sconstraint_F;
        } C_4;
    /* 5 */
        struct tak_systembuffer_5
        {
            tak_constraintnamerecord sconstrname_F;
        } C_5;
    /* 6 */
        struct tak_systembuffer_6
        {
            tak_correcord       scorr_F;
        } C_6;
    /* 8 */
        struct tak_systembuffer_8
        {
            tak_defaultrecord   sdefault_F;
        } C_8;
    /* 9 */
        struct tak_systembuffer_9
        {
            tak_domainrecord    sdomain_F;
        } C_9;
    /* 10 */
        struct tak_systembuffer_10
        {
            tak_domainref       sdomainref_F;
        } C_10;
    /* 14 */
        struct tak_systembuffer_14
        {
            tak_linkrecord      slink_F;
        } C_14;
    /* 15 */
        struct tak_systembuffer_15
        {
            tak_link_def_record slinkdefault_F;
        } C_15;
    /* 16 */
        struct tak_systembuffer_16
        {
            tak_linkparsrecord  slinkparsinfo_F;
        } C_16;
    /* 17 */
        struct tak_systembuffer_17
        {
            tak_linkinforecord  slinkposinfo_F;
        } C_17;
    /* 18 */
        struct tak_systembuffer_18
        {
            tak_serverdbrecord  sserverdb_F;
        } C_18;
    /* 19 */
        struct tak_systembuffer_19
        {
            tak_serverdbnamerecord sserverdbname_F;
        } C_19;
    /* 22 */
        struct tak_systembuffer_22
        {
            tak_mindexrecord    smindex_F;
        } C_22;
    /* 23 */
        struct tak_systembuffer_23
        {
            tak_parsinforecord  sparsinfo_F;
        } C_23;
    /* 24 */
        struct tak_systembuffer_24
        {
            tak_privrecord      spriv_F;
        } C_24;
    /* 25 */
        struct tak_systembuffer_25
        {
            tak_privuserrecord  sprivuser_F;
        } C_25;
    /* 26 */
        struct tak_systembuffer_26
        {
            tak_resultkeyrecord sreskey_F;
        } C_26;
    /* 27 */
        struct tak_systembuffer_27
        {
            tak_hintrecord      shint_F;
        } C_27;
    /* 28 */
        struct tak_systembuffer_28
        {
            tak_resultrecord    sresult_F;
        } C_28;
    /* 29 */
        struct tak_systembuffer_29
        {
            tak_scoldescrecord  sscoldesc_F;
        } C_29;
    /* 30 */
        struct tak_systembuffer_30
        {
            tak_sequencerecord  ssequence_F;
        } C_30;
    /* 31 */
        struct tak_systembuffer_31
        {
            tak_shortinforecord sshortinfo_F;
        } C_31;
    /* 33 */
        struct tak_systembuffer_33
        {
            tak_subtransrecord  ssubtrans_F;
        } C_33;
    /* 34 */
        struct tak_systembuffer_34
        {
            tak_synonymrecord   ssynonym_F;
        } C_34;
    /* 35 */
        struct tak_systembuffer_35
        {
            tak_sysdbarecord    ssysdba_F;
        } C_35;
    /* 36 */
        struct tak_systembuffer_36
        {
            tak_tablereference  stableref_F;
        } C_36;
    /* 38 */
        struct tak_systembuffer_38
        {
            tak_triggerrecord   strigger_F;
        } C_38;
    /* 40 */
        struct tak_systembuffer_40
        {
            tak_unionrecord     sunionrec_F;
        } C_40;
    /* 41 */
        struct tak_systembuffer_41
        {
            tak_usagerecord     susage_F;
        } C_41;
    /* 42 */
        struct tak_systembuffer_42
        {
            tak_userrecord      suser_F;
        } C_42;
    /* 43 */
        struct tak_systembuffer_43
        {
            tak_useridrecord    suserid_F;
        } C_43;
    /* 44 */
        struct tak_systembuffer_44
        {
            tak_userrefrecord   suserref_F;
        } C_44;
    /* 45 */
        struct tak_systembuffer_45
        {
            tak_usertablerecord susertab_F;
        } C_45;
    /* 46 */
        struct tak_systembuffer_46
        {
            tak_viewdescrecord  sviewdesc_F;
        } C_46;
    /* 47 */
        struct tak_systembuffer_47
        {
            tak_viewkeyrecord   sviewkey_F;
        } C_47;
    /* 49 */
        struct tak_systembuffer_49
        {
            tak_viewtextrecord  sviewtext_F;
        } C_49;
    /* 50 */
        struct tak_systembuffer_50
        {
            tgg00_SysInfoKey    syskey_F;
        } C_50;
    /* 51 */
        struct tak_systembuffer_51
        {
            OLDPASCALSTRING_VAR_MSP00 (  ssysbuf_F, mxak_systembuf);
        } C_51;
    /* 52 */
        struct tak_systembuffer_52
        {
            tak_resname_record  sresname_F;
        } C_52;
    /* 53 */
        struct tak_systembuffer_53
        {
            tak_mapset_record   smapset_F;
        } C_53;
    /* 54 */
        struct tak_systembuffer_54
        {
            tak_funcrefrecord   sfuncref_F;
        } C_54;
    /* 55 */
        struct tak_systembuffer_55
        {
            tak_proc_seq_privilege sprocseqpriv_F;
        } C_55;
    /* 56 */
        struct tak_systembuffer_56
        {
            tak_col_uses_dom    scol_uses_dom_F;
        } C_56;
    /* 57 */
        struct tak_systembuffer_57
        {
            tak_alterdaterecord salterdate_F;
        } C_57;
    /* 59 */
        struct tak_systembuffer_59
        {
            tak_columnnamesrecord scolnames_F;
        } C_59;
    /* 60 */
        struct tak_systembuffer_60
        {
            tak_stored_cmd_record sstored_cmd_F;
        } C_60;
    /* 61 */
        struct tak_systembuffer_61
        {
            tak_changeinfos     schangeinfos_F;
        } C_61;
    /* 62 */
        struct tak_systembuffer_62
        {
            tak_messblock_record smessblock_F;
        } C_62;
    /* 63 */
        struct tak_systembuffer_63
        {
            tak_statisticsinfo  sstatistics_F;
        } C_63;
    /* 65 */
        struct tak_systembuffer_65
        {
            tak_longliteral_rec slongliteral_F;
        } C_65;
    /* 66 */
        struct tak_systembuffer_66
        {
            tak_methodrecord    smethod_F;
        } C_66;
    /* 67 */
        struct tak_systembuffer_67
        {
            tak_methodrefrecord smethodref_F;
        } C_67;
    /* 68 */
        struct tak_systembuffer_68
        {
            tak_type_uid_map_record stypeuidmap_F;
        } C_68;
    /* 69 */
        struct tak_systembuffer_69
        {
            tak_method_inforecord smethodinfo_F;
        } C_69;
    /* 70 */
        struct tak_systembuffer_70
        {
            tak_class_map_guid_record sclassmap_F;
        } C_70;
    /* 71 */
        struct tak_systembuffer_71
        {
            tak_class_record    sclass_F;
        } C_71;
    /* 72 */
        struct tak_systembuffer_72
        {
            tak_version_record  sversion_F;
        } C_72;
    /* 73 */
        struct tak_systembuffer_73
        {
            tak_commandinforecord scommand_info_F;
        } C_73;
    /* 74 */
        struct tak_systembuffer_74
        {
            tak_viewqual_basis_record sviewqual_basis_F;
        } C_74;
    /* 75 */
        struct tak_systembuffer_75
        {
            tak_viewqual_join_record sviewqual_join_F;
        } C_75;
    /* 76 */
        struct tak_systembuffer_76
        {
            tak_viewqual_stack_record sviewqual_stack_F;
        } C_76;
    /* 77 */
        struct tak_systembuffer_77
        {
            tak_viewqual_derivedcol_record sviewqual_derivedcol_F;
        } C_77;
    /* 78 */
        struct tak_systembuffer_78
        {
            tak_schema_record   sschema_F;
        } C_78;
    /* 79 */
        struct tak_systembuffer_79
        {
            tak_schema_ref_record sschemaref_F;
        } C_79;
    /* 80 */
        struct tak_systembuffer_80
        {
            tak_method_fileinforecord smethodfileinfo_F;
        } C_80;
    /* 81 */
        struct tak_systembuffer_81
        {
            tak_method_debuginfo smethoddebuginfo_F;
        } C_81;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tsp00_Int2         &b_sl                   ()  { return this->variant.C_0.b_sl_F; };
    tsp00_Int2         &b_kl                   ()  { return this->variant.C_0.b_kl_F; };
    tsp00_Int2         &b_varcol_offset        ()  { return this->variant.C_0.b_varcol_offset_F; };
    tsp00_Int2         &b_varcol_cnt           ()  { return this->variant.C_0.b_varcol_cnt_F; };
    tak_baserecord     &sbase                  ()  { return this->variant.C_1.sbase_F; };
    tak_comment_record &scomment               ()  { return this->variant.C_2.scomment_F; };
    tak_complexrecord  &scomplexrec            ()  { return this->variant.C_3.scomplexrec_F; };
    tak_constraintrecord &sconstraint            ()  { return this->variant.C_4.sconstraint_F; };
    tak_constraintnamerecord &sconstrname            ()  { return this->variant.C_5.sconstrname_F; };
    tak_correcord      &scorr                  ()  { return this->variant.C_6.scorr_F; };
    tak_defaultrecord  &sdefault               ()  { return this->variant.C_8.sdefault_F; };
    tak_domainrecord   &sdomain                ()  { return this->variant.C_9.sdomain_F; };
    tak_domainref      &sdomainref             ()  { return this->variant.C_10.sdomainref_F; };
    tak_linkrecord     &slink                  ()  { return this->variant.C_14.slink_F; };
    tak_link_def_record &slinkdefault           ()  { return this->variant.C_15.slinkdefault_F; };
    tak_linkparsrecord &slinkparsinfo          ()  { return this->variant.C_16.slinkparsinfo_F; };
    tak_linkinforecord &slinkposinfo           ()  { return this->variant.C_17.slinkposinfo_F; };
    tak_serverdbrecord &sserverdb              ()  { return this->variant.C_18.sserverdb_F; };
    tak_serverdbnamerecord &sserverdbname          ()  { return this->variant.C_19.sserverdbname_F; };
    tak_mindexrecord   &smindex                ()  { return this->variant.C_22.smindex_F; };
    tak_parsinforecord &sparsinfo              ()  { return this->variant.C_23.sparsinfo_F; };
    tak_privrecord     &spriv                  ()  { return this->variant.C_24.spriv_F; };
    tak_privuserrecord &sprivuser              ()  { return this->variant.C_25.sprivuser_F; };
    tak_resultkeyrecord &sreskey                ()  { return this->variant.C_26.sreskey_F; };
    tak_hintrecord     &shint                  ()  { return this->variant.C_27.shint_F; };
    tak_resultrecord   &sresult                ()  { return this->variant.C_28.sresult_F; };
    tak_scoldescrecord &sscoldesc              ()  { return this->variant.C_29.sscoldesc_F; };
    tak_sequencerecord &ssequence              ()  { return this->variant.C_30.ssequence_F; };
    tak_shortinforecord &sshortinfo             ()  { return this->variant.C_31.sshortinfo_F; };
    tak_subtransrecord &ssubtrans              ()  { return this->variant.C_33.ssubtrans_F; };
    tak_synonymrecord  &ssynonym               ()  { return this->variant.C_34.ssynonym_F; };
    tak_sysdbarecord   &ssysdba                ()  { return this->variant.C_35.ssysdba_F; };
    tak_tablereference &stableref              ()  { return this->variant.C_36.stableref_F; };
    tak_triggerrecord  &strigger               ()  { return this->variant.C_38.strigger_F; };
    tak_unionrecord    &sunionrec              ()  { return this->variant.C_40.sunionrec_F; };
    tak_usagerecord    &susage                 ()  { return this->variant.C_41.susage_F; };
    tak_userrecord     &suser                  ()  { return this->variant.C_42.suser_F; };
    tak_useridrecord   &suserid                ()  { return this->variant.C_43.suserid_F; };
    tak_userrefrecord  &suserref               ()  { return this->variant.C_44.suserref_F; };
    tak_usertablerecord &susertab               ()  { return this->variant.C_45.susertab_F; };
    tak_viewdescrecord &sviewdesc              ()  { return this->variant.C_46.sviewdesc_F; };
    tak_viewkeyrecord  &sviewkey               ()  { return this->variant.C_47.sviewkey_F; };
    tak_viewtextrecord &sviewtext              ()  { return this->variant.C_49.sviewtext_F; };
    tgg00_SysInfoKey   &syskey                 ()  { return this->variant.C_50.syskey_F; };
    tsp00_OldPascalString<mxak_systembuf> &ssysbuf                ()  { return this->variant.C_51.ssysbuf_F; };
    tak_resname_record &sresname               ()  { return this->variant.C_52.sresname_F; };
    tak_mapset_record  &smapset                ()  { return this->variant.C_53.smapset_F; };
    tak_funcrefrecord  &sfuncref               ()  { return this->variant.C_54.sfuncref_F; };
    tak_proc_seq_privilege &sprocseqpriv           ()  { return this->variant.C_55.sprocseqpriv_F; };
    tak_col_uses_dom   &scol_uses_dom          ()  { return this->variant.C_56.scol_uses_dom_F; };
    tak_alterdaterecord &salterdate             ()  { return this->variant.C_57.salterdate_F; };
    tak_columnnamesrecord &scolnames              ()  { return this->variant.C_59.scolnames_F; };
    tak_stored_cmd_record &sstored_cmd            ()  { return this->variant.C_60.sstored_cmd_F; };
    tak_changeinfos    &schangeinfos           ()  { return this->variant.C_61.schangeinfos_F; };
    tak_messblock_record &smessblock             ()  { return this->variant.C_62.smessblock_F; };
    tak_statisticsinfo &sstatistics            ()  { return this->variant.C_63.sstatistics_F; };
    tak_longliteral_rec &slongliteral           ()  { return this->variant.C_65.slongliteral_F; };
    tak_methodrecord   &smethod                ()  { return this->variant.C_66.smethod_F; };
    tak_methodrefrecord &smethodref             ()  { return this->variant.C_67.smethodref_F; };
    tak_type_uid_map_record &stypeuidmap            ()  { return this->variant.C_68.stypeuidmap_F; };
    tak_method_inforecord &smethodinfo            ()  { return this->variant.C_69.smethodinfo_F; };
    tak_class_map_guid_record &sclassmap              ()  { return this->variant.C_70.sclassmap_F; };
    tak_class_record   &sclass                 ()  { return this->variant.C_71.sclass_F; };
    tak_version_record &sversion               ()  { return this->variant.C_72.sversion_F; };
    tak_commandinforecord &scommand_info          ()  { return this->variant.C_73.scommand_info_F; };
    tak_viewqual_basis_record &sviewqual_basis        ()  { return this->variant.C_74.sviewqual_basis_F; };
    tak_viewqual_join_record &sviewqual_join         ()  { return this->variant.C_75.sviewqual_join_F; };
    tak_viewqual_stack_record &sviewqual_stack        ()  { return this->variant.C_76.sviewqual_stack_F; };
    tak_viewqual_derivedcol_record &sviewqual_derivedcol   ()  { return this->variant.C_77.sviewqual_derivedcol_F; };
    tak_schema_record  &sschema                ()  { return this->variant.C_78.sschema_F; };
    tak_schema_ref_record &sschemaref             ()  { return this->variant.C_79.sschemaref_F; };
    tak_method_fileinforecord &smethodfileinfo        ()  { return this->variant.C_80.smethodfileinfo_F; };
    tak_method_debuginfo &smethoddebuginfo       ()  { return this->variant.C_81.smethoddebuginfo_F; };
    /* const accessor functions */
    const tsp00_Int2   &b_sl                   () const  { return this->variant.C_0.b_sl_F; };
    const tsp00_Int2   &b_kl                   () const  { return this->variant.C_0.b_kl_F; };
    const tsp00_Int2   &b_varcol_offset        () const  { return this->variant.C_0.b_varcol_offset_F; };
    const tsp00_Int2   &b_varcol_cnt           () const  { return this->variant.C_0.b_varcol_cnt_F; };
    const tak_baserecord &sbase                  () const  { return this->variant.C_1.sbase_F; };
    const tak_comment_record &scomment               () const  { return this->variant.C_2.scomment_F; };
    const tak_complexrecord &scomplexrec            () const  { return this->variant.C_3.scomplexrec_F; };
    const tak_constraintrecord &sconstraint            () const  { return this->variant.C_4.sconstraint_F; };
    const tak_constraintnamerecord &sconstrname            () const  { return this->variant.C_5.sconstrname_F; };
    const tak_correcord &scorr                  () const  { return this->variant.C_6.scorr_F; };
    const tak_defaultrecord &sdefault               () const  { return this->variant.C_8.sdefault_F; };
    const tak_domainrecord &sdomain                () const  { return this->variant.C_9.sdomain_F; };
    const tak_domainref &sdomainref             () const  { return this->variant.C_10.sdomainref_F; };
    const tak_linkrecord &slink                  () const  { return this->variant.C_14.slink_F; };
    const tak_link_def_record &slinkdefault           () const  { return this->variant.C_15.slinkdefault_F; };
    const tak_linkparsrecord &slinkparsinfo          () const  { return this->variant.C_16.slinkparsinfo_F; };
    const tak_linkinforecord &slinkposinfo           () const  { return this->variant.C_17.slinkposinfo_F; };
    const tak_serverdbrecord &sserverdb              () const  { return this->variant.C_18.sserverdb_F; };
    const tak_serverdbnamerecord &sserverdbname          () const  { return this->variant.C_19.sserverdbname_F; };
    const tak_mindexrecord &smindex                () const  { return this->variant.C_22.smindex_F; };
    const tak_parsinforecord &sparsinfo              () const  { return this->variant.C_23.sparsinfo_F; };
    const tak_privrecord &spriv                  () const  { return this->variant.C_24.spriv_F; };
    const tak_privuserrecord &sprivuser              () const  { return this->variant.C_25.sprivuser_F; };
    const tak_resultkeyrecord &sreskey                () const  { return this->variant.C_26.sreskey_F; };
    const tak_hintrecord &shint                  () const  { return this->variant.C_27.shint_F; };
    const tak_resultrecord &sresult                () const  { return this->variant.C_28.sresult_F; };
    const tak_scoldescrecord &sscoldesc              () const  { return this->variant.C_29.sscoldesc_F; };
    const tak_sequencerecord &ssequence              () const  { return this->variant.C_30.ssequence_F; };
    const tak_shortinforecord &sshortinfo             () const  { return this->variant.C_31.sshortinfo_F; };
    const tak_subtransrecord &ssubtrans              () const  { return this->variant.C_33.ssubtrans_F; };
    const tak_synonymrecord &ssynonym               () const  { return this->variant.C_34.ssynonym_F; };
    const tak_sysdbarecord &ssysdba                () const  { return this->variant.C_35.ssysdba_F; };
    const tak_tablereference &stableref              () const  { return this->variant.C_36.stableref_F; };
    const tak_triggerrecord &strigger               () const  { return this->variant.C_38.strigger_F; };
    const tak_unionrecord &sunionrec              () const  { return this->variant.C_40.sunionrec_F; };
    const tak_usagerecord &susage                 () const  { return this->variant.C_41.susage_F; };
    const tak_userrecord &suser                  () const  { return this->variant.C_42.suser_F; };
    const tak_useridrecord &suserid                () const  { return this->variant.C_43.suserid_F; };
    const tak_userrefrecord &suserref               () const  { return this->variant.C_44.suserref_F; };
    const tak_usertablerecord &susertab               () const  { return this->variant.C_45.susertab_F; };
    const tak_viewdescrecord &sviewdesc              () const  { return this->variant.C_46.sviewdesc_F; };
    const tak_viewkeyrecord &sviewkey               () const  { return this->variant.C_47.sviewkey_F; };
    const tak_viewtextrecord &sviewtext              () const  { return this->variant.C_49.sviewtext_F; };
    const tgg00_SysInfoKey &syskey                 () const  { return this->variant.C_50.syskey_F; };
    const tsp00_OldPascalString<mxak_systembuf> &ssysbuf                () const  { return this->variant.C_51.ssysbuf_F; };
    const tak_resname_record &sresname               () const  { return this->variant.C_52.sresname_F; };
    const tak_mapset_record &smapset                () const  { return this->variant.C_53.smapset_F; };
    const tak_funcrefrecord &sfuncref               () const  { return this->variant.C_54.sfuncref_F; };
    const tak_proc_seq_privilege &sprocseqpriv           () const  { return this->variant.C_55.sprocseqpriv_F; };
    const tak_col_uses_dom &scol_uses_dom          () const  { return this->variant.C_56.scol_uses_dom_F; };
    const tak_alterdaterecord &salterdate             () const  { return this->variant.C_57.salterdate_F; };
    const tak_columnnamesrecord &scolnames              () const  { return this->variant.C_59.scolnames_F; };
    const tak_stored_cmd_record &sstored_cmd            () const  { return this->variant.C_60.sstored_cmd_F; };
    const tak_changeinfos &schangeinfos           () const  { return this->variant.C_61.schangeinfos_F; };
    const tak_messblock_record &smessblock             () const  { return this->variant.C_62.smessblock_F; };
    const tak_statisticsinfo &sstatistics            () const  { return this->variant.C_63.sstatistics_F; };
    const tak_longliteral_rec &slongliteral           () const  { return this->variant.C_65.slongliteral_F; };
    const tak_methodrecord &smethod                () const  { return this->variant.C_66.smethod_F; };
    const tak_methodrefrecord &smethodref             () const  { return this->variant.C_67.smethodref_F; };
    const tak_type_uid_map_record &stypeuidmap            () const  { return this->variant.C_68.stypeuidmap_F; };
    const tak_method_inforecord &smethodinfo            () const  { return this->variant.C_69.smethodinfo_F; };
    const tak_class_map_guid_record &sclassmap              () const  { return this->variant.C_70.sclassmap_F; };
    const tak_class_record &sclass                 () const  { return this->variant.C_71.sclass_F; };
    const tak_version_record &sversion               () const  { return this->variant.C_72.sversion_F; };
    const tak_commandinforecord &scommand_info          () const  { return this->variant.C_73.scommand_info_F; };
    const tak_viewqual_basis_record &sviewqual_basis        () const  { return this->variant.C_74.sviewqual_basis_F; };
    const tak_viewqual_join_record &sviewqual_join         () const  { return this->variant.C_75.sviewqual_join_F; };
    const tak_viewqual_stack_record &sviewqual_stack        () const  { return this->variant.C_76.sviewqual_stack_F; };
    const tak_viewqual_derivedcol_record &sviewqual_derivedcol   () const  { return this->variant.C_77.sviewqual_derivedcol_F; };
    const tak_schema_record &sschema                () const  { return this->variant.C_78.sschema_F; };
    const tak_schema_ref_record &sschemaref             () const  { return this->variant.C_79.sschemaref_F; };
    const tak_method_fileinforecord &smethodfileinfo        () const  { return this->variant.C_80.smethodfileinfo_F; };
    const tak_method_debuginfo &smethoddebuginfo       () const  { return this->variant.C_81.smethoddebuginfo_F; };
#endif    /* defined (__cplusplus) */
} ;

typedef enum tak_mem_type_Enum
{
    dyn_fill1,
    dyn_fill2,
    dyn_fill3,
    dyn_fill4,
    dyn_fill5,
    dyn_fill6,
    dyn_fill7,
    dyn_fill8,
    dyn_fill9,
    dyn_fill10,
    dyn_free,
    dyn_normal,
    dyn_packet,
    dyn_mess_block,
    dyn_mess_block_part,
    dyn_syntax_tree
} tak_mem_type_Enum;
typedef tak_mem_type_Enum tak_mem_type_Param;
typedef ENUM_TYPE_MSP00 (          tak_mem_type, tak_mem_type_Enum, dyn_syntax_tree, unsigned char);

typedef enum tak_balance_info_Enum
{
    left_weighted,
    balanced,
    right_weighted
} tak_balance_info_Enum;
typedef tak_balance_info_Enum tak_balance_info_Param;
typedef ENUM_TYPE_MSP00 (          tak_balance_info, tak_balance_info_Enum, right_weighted, unsigned char);

typedef struct                     tak_globalCacheReference
{
    tsp00_Int4          cd_pos;
    tsp00_Int2          cd_check;
    tsp00_Int2          cd_refCount;
    tak_cache_dir_pointer cd_less;
    tak_cache_dir_pointer cd_greater;
    tak_cache_dir_pointer cd_fix_prev;
    tak_cache_dir_pointer cd_fix_next;
    tak_cache_dir_pointer cd_lru_prev;
    tak_cache_dir_pointer cd_lru_next;
    tsp00_Int4          cd_cmd_no;
    tsp00_Int4          cd_create_cmd_no;
    tak_sysdir_state    cd_state;
    tak_balance_info    cd_balance;
    tsp00_Int2          cd_buf_no;
    tsp00_Int4          cd_syslen;
    tgg00_VoidPtr       cd_globalCacheHandle;
    tak_sysbufferaddress cd_globalSysinfoPtr;
    tak_cache_dir_pointer cd_prevRef;
    tak_cache_dir_pointer cd_nextRef;
} tak_globalCacheReference;

 struct                     tak_cache_dir_entry
{
    union
    {
    /* 1 */
        struct tak_cache_dir_entry_1
        {
            tsp00_Int4          cd_pos_F;
            tsp00_Int2          cd_check_F;
            tsp00_Int2          cd_refCount_F;
            tak_cache_dir_pointer cd_less_F;
            tak_cache_dir_pointer cd_greater_F;
            tak_cache_dir_pointer cd_fix_prev_F;
            tak_cache_dir_pointer cd_fix_next_F;
            tak_cache_dir_pointer cd_lru_prev_F;
            tak_cache_dir_pointer cd_lru_next_F;
            tsp00_Int4          cd_cmd_no_F;
            tsp00_Int4          cd_create_cmd_no_F;
            tak_sysdir_state    cd_state_F;
            tak_balance_info    cd_balance_F;
            tsp00_Int2          cd_buf_no_F;
            tsp00_Int4          cd_syslen_F;
            tgg00_SysInfoKey    cd_syskey_F;
        } C_1;
    /* 2 */
        struct tak_cache_dir_entry_2
        {
            tsp00_Int4          cd_block_pos_F;
            tsp00_Int2          cd_block_check_F;
            tsp00_Int2          cd_blockRefCount_F;
            tak_cache_dir_pointer cd_bl_less_F;
            tak_cache_dir_pointer cd_next_free_F;
            tak_cache_dir_pointer cd_bl_fix_prev_F;
            tak_cache_dir_pointer cd_bl_fix_next_F;
            tak_cache_dir_pointer cd_bl_lru_prev_F;
            tak_cache_dir_pointer cd_bl_lru_next_F;
            tsp00_Int4          cd_free_fill_F;
            tsp00_Int4          cd_block_len_F;
            tak_sysdir_state    cd_bl_state_F;
            tak_mem_type        cd_mem_type_F;
            tsp00_Int2          cd_bl_buf_no_F;
            tsp00_Int4          cd_bl_syslen_F;
            tak_systembuffer    cd_sysinfo_F;
        } C_2;
    /* 3 */
        struct tak_cache_dir_entry_3
        {
            tak_globalCacheReference cd_ref_F;
        } C_3;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tsp00_Int4         &cd_pos                 ()  { return this->variant.C_1.cd_pos_F; };
    tsp00_Int2         &cd_check               ()  { return this->variant.C_1.cd_check_F; };
    tsp00_Int2         &cd_refCount            ()  { return this->variant.C_1.cd_refCount_F; };
    tak_cache_dir_pointer &cd_less                ()  { return this->variant.C_1.cd_less_F; };
    tak_cache_dir_pointer &cd_greater             ()  { return this->variant.C_1.cd_greater_F; };
    tak_cache_dir_pointer &cd_fix_prev            ()  { return this->variant.C_1.cd_fix_prev_F; };
    tak_cache_dir_pointer &cd_fix_next            ()  { return this->variant.C_1.cd_fix_next_F; };
    tak_cache_dir_pointer &cd_lru_prev            ()  { return this->variant.C_1.cd_lru_prev_F; };
    tak_cache_dir_pointer &cd_lru_next            ()  { return this->variant.C_1.cd_lru_next_F; };
    tsp00_Int4         &cd_cmd_no              ()  { return this->variant.C_1.cd_cmd_no_F; };
    tsp00_Int4         &cd_create_cmd_no       ()  { return this->variant.C_1.cd_create_cmd_no_F; };
    tak_sysdir_state   &cd_state               ()  { return this->variant.C_1.cd_state_F; };
    tak_balance_info   &cd_balance             ()  { return this->variant.C_1.cd_balance_F; };
    tsp00_Int2         &cd_buf_no              ()  { return this->variant.C_1.cd_buf_no_F; };
    tsp00_Int4         &cd_syslen              ()  { return this->variant.C_1.cd_syslen_F; };
    tgg00_SysInfoKey   &cd_syskey              ()  { return this->variant.C_1.cd_syskey_F; };
    tsp00_Int4         &cd_block_pos           ()  { return this->variant.C_2.cd_block_pos_F; };
    tsp00_Int2         &cd_block_check         ()  { return this->variant.C_2.cd_block_check_F; };
    tsp00_Int2         &cd_blockRefCount       ()  { return this->variant.C_2.cd_blockRefCount_F; };
    tak_cache_dir_pointer &cd_bl_less             ()  { return this->variant.C_2.cd_bl_less_F; };
    tak_cache_dir_pointer &cd_next_free           ()  { return this->variant.C_2.cd_next_free_F; };
    tak_cache_dir_pointer &cd_bl_fix_prev         ()  { return this->variant.C_2.cd_bl_fix_prev_F; };
    tak_cache_dir_pointer &cd_bl_fix_next         ()  { return this->variant.C_2.cd_bl_fix_next_F; };
    tak_cache_dir_pointer &cd_bl_lru_prev         ()  { return this->variant.C_2.cd_bl_lru_prev_F; };
    tak_cache_dir_pointer &cd_bl_lru_next         ()  { return this->variant.C_2.cd_bl_lru_next_F; };
    tsp00_Int4         &cd_free_fill           ()  { return this->variant.C_2.cd_free_fill_F; };
    tsp00_Int4         &cd_block_len           ()  { return this->variant.C_2.cd_block_len_F; };
    tak_sysdir_state   &cd_bl_state            ()  { return this->variant.C_2.cd_bl_state_F; };
    tak_mem_type       &cd_mem_type            ()  { return this->variant.C_2.cd_mem_type_F; };
    tsp00_Int2         &cd_bl_buf_no           ()  { return this->variant.C_2.cd_bl_buf_no_F; };
    tsp00_Int4         &cd_bl_syslen           ()  { return this->variant.C_2.cd_bl_syslen_F; };
    tak_systembuffer   &cd_sysinfo             ()  { return this->variant.C_2.cd_sysinfo_F; };
    tak_globalCacheReference &cd_ref                 ()  { return this->variant.C_3.cd_ref_F; };
    /* const accessor functions */
    const tsp00_Int4   &cd_pos                 () const  { return this->variant.C_1.cd_pos_F; };
    const tsp00_Int2   &cd_check               () const  { return this->variant.C_1.cd_check_F; };
    const tsp00_Int2   &cd_refCount            () const  { return this->variant.C_1.cd_refCount_F; };
    const tak_cache_dir_pointer &cd_less                () const  { return this->variant.C_1.cd_less_F; };
    const tak_cache_dir_pointer &cd_greater             () const  { return this->variant.C_1.cd_greater_F; };
    const tak_cache_dir_pointer &cd_fix_prev            () const  { return this->variant.C_1.cd_fix_prev_F; };
    const tak_cache_dir_pointer &cd_fix_next            () const  { return this->variant.C_1.cd_fix_next_F; };
    const tak_cache_dir_pointer &cd_lru_prev            () const  { return this->variant.C_1.cd_lru_prev_F; };
    const tak_cache_dir_pointer &cd_lru_next            () const  { return this->variant.C_1.cd_lru_next_F; };
    const tsp00_Int4   &cd_cmd_no              () const  { return this->variant.C_1.cd_cmd_no_F; };
    const tsp00_Int4   &cd_create_cmd_no       () const  { return this->variant.C_1.cd_create_cmd_no_F; };
    const tak_sysdir_state &cd_state               () const  { return this->variant.C_1.cd_state_F; };
    const tak_balance_info &cd_balance             () const  { return this->variant.C_1.cd_balance_F; };
    const tsp00_Int2   &cd_buf_no              () const  { return this->variant.C_1.cd_buf_no_F; };
    const tsp00_Int4   &cd_syslen              () const  { return this->variant.C_1.cd_syslen_F; };
    const tgg00_SysInfoKey &cd_syskey              () const  { return this->variant.C_1.cd_syskey_F; };
    const tsp00_Int4   &cd_block_pos           () const  { return this->variant.C_2.cd_block_pos_F; };
    const tsp00_Int2   &cd_block_check         () const  { return this->variant.C_2.cd_block_check_F; };
    const tsp00_Int2   &cd_blockRefCount       () const  { return this->variant.C_2.cd_blockRefCount_F; };
    const tak_cache_dir_pointer &cd_bl_less             () const  { return this->variant.C_2.cd_bl_less_F; };
    const tak_cache_dir_pointer &cd_next_free           () const  { return this->variant.C_2.cd_next_free_F; };
    const tak_cache_dir_pointer &cd_bl_fix_prev         () const  { return this->variant.C_2.cd_bl_fix_prev_F; };
    const tak_cache_dir_pointer &cd_bl_fix_next         () const  { return this->variant.C_2.cd_bl_fix_next_F; };
    const tak_cache_dir_pointer &cd_bl_lru_prev         () const  { return this->variant.C_2.cd_bl_lru_prev_F; };
    const tak_cache_dir_pointer &cd_bl_lru_next         () const  { return this->variant.C_2.cd_bl_lru_next_F; };
    const tsp00_Int4   &cd_free_fill           () const  { return this->variant.C_2.cd_free_fill_F; };
    const tsp00_Int4   &cd_block_len           () const  { return this->variant.C_2.cd_block_len_F; };
    const tak_sysdir_state &cd_bl_state            () const  { return this->variant.C_2.cd_bl_state_F; };
    const tak_mem_type &cd_mem_type            () const  { return this->variant.C_2.cd_mem_type_F; };
    const tsp00_Int2   &cd_bl_buf_no           () const  { return this->variant.C_2.cd_bl_buf_no_F; };
    const tsp00_Int4   &cd_bl_syslen           () const  { return this->variant.C_2.cd_bl_syslen_F; };
    const tak_systembuffer &cd_sysinfo             () const  { return this->variant.C_2.cd_sysinfo_F; };
    const tak_globalCacheReference &cd_ref                 () const  { return this->variant.C_3.cd_ref_F; };
#endif    /* defined (__cplusplus) */
} ;

typedef enum tak_odbc_partype_Enum
{
    no_odbc,
    normal_odbc,
    extended_odbc
} tak_odbc_partype_Enum;
typedef tak_odbc_partype_Enum tak_odbc_partype_Param;
typedef ENUM_TYPE_MSP00 (          tak_odbc_partype, tak_odbc_partype_Enum, extended_odbc, unsigned char);

typedef enum tak_odbc_functiontype_Enum
{
    odbc_nothing,
    odbc_call,
    odbc_fn,
    odbc_oj,
    odbc_d,
    odbc_t,
    odbc_ts,
    odbc_esc,
    odbc_dbfunc
} tak_odbc_functiontype_Enum;
typedef tak_odbc_functiontype_Enum tak_odbc_functiontype_Param;
typedef ENUM_TYPE_MSP00 (          tak_odbc_functiontype, tak_odbc_functiontype_Enum, odbc_dbfunc, unsigned char);

typedef struct                     tak_dbproc_stack_param
{
    union
    {
    /* 1 */
        struct tak_dbproc_stack_param_1
        {
            tsp00_C1            i1_F;
        } C_1;
    /* 2 */
        struct tak_dbproc_stack_param_2
        {
            tsp00_C2            c2_F;
        } C_2;
    /* 3 */
        struct tak_dbproc_stack_param_3
        {
            tsp00_Int2          i2_F;
        } C_3;
    /* 4 */
        struct tak_dbproc_stack_param_4
        {
            tsp00_Int4          i4_F;
        } C_4;
    /* 5 */
        struct tak_dbproc_stack_param_5
        {
            tsp00_Shortreal     r4_F;
        } C_5;
    /* 6 */
        struct tak_dbproc_stack_param_6
        {
            tsp00_Longreal      r8_F;
        } C_6;
    /* 7 */
        struct tak_dbproc_stack_param_7
        {
            tsp00_8ByteCounter  i8_F;
        } C_7;
    /* 8 */
        struct tak_dbproc_stack_param_8
        {
            tsp00_MoveObjPtr    ptr_F;
        } C_8;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tsp00_C1           &i1                     ()  { return this->variant.C_1.i1_F; };
    tsp00_C2           &c2                     ()  { return this->variant.C_2.c2_F; };
    tsp00_Int2         &i2                     ()  { return this->variant.C_3.i2_F; };
    tsp00_Int4         &i4                     ()  { return this->variant.C_4.i4_F; };
    tsp00_Shortreal    &r4                     ()  { return this->variant.C_5.r4_F; };
    tsp00_Longreal     &r8                     ()  { return this->variant.C_6.r8_F; };
    tsp00_8ByteCounter &i8                     ()  { return this->variant.C_7.i8_F; };
    tsp00_MoveObjPtr   &ptr                    ()  { return this->variant.C_8.ptr_F; };
    /* const accessor functions */
    const tsp00_C1     &i1                     () const  { return this->variant.C_1.i1_F; };
    const tsp00_C2     &c2                     () const  { return this->variant.C_2.c2_F; };
    const tsp00_Int2   &i2                     () const  { return this->variant.C_3.i2_F; };
    const tsp00_Int4   &i4                     () const  { return this->variant.C_4.i4_F; };
    const tsp00_Shortreal &r4                     () const  { return this->variant.C_5.r4_F; };
    const tsp00_Longreal &r8                     () const  { return this->variant.C_6.r8_F; };
    const tsp00_8ByteCounter &i8                     () const  { return this->variant.C_7.i8_F; };
    const tsp00_MoveObjPtr &ptr                    () const  { return this->variant.C_8.ptr_F; };
#endif    /* defined (__cplusplus) */
} tak_dbproc_stack_param;

typedef struct                     tak_dbproc_stack_param_debug
{
    tak_dbproc_stack_param dbg_value;
    tsp00_Int2          dbg_vt;
    tsp1_param_io_type  dbg_inout;
    pasbool             dbg_is_ptr;
    tsp00_Int4          dbg_length;
} tak_dbproc_stack_param_debug;

typedef struct                     tak_fromsel_tabid
{
    union
    {
    /* 1 */
        struct tak_fromsel_tabid_1
        {
            tgg00_ServerdbNo    fromsiteno_F;
            tgg91_SessionNo     session_F;
            tgg00_TfnTemp       ft_F;
            tsp00_Uint1         fs_no_F;
        } C_1;
    /* 2 */
        struct tak_fromsel_tabid_2
        {
            tsp00_C1            fparschar_F;
            tsp00_C3            fcmd_count_F;
            tsp00_Int2          ffill_F;
            tgg00_TfnTemp       ft1_F;
            tsp00_Uint1         fs_no1_F;
        } C_2;
    /* 3 */
        struct tak_fromsel_tabid_3
        {
            tgg00_Surrogate     tabid_F;
        } C_3;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tgg00_ServerdbNo   &fromsiteno             ()  { return this->variant.C_1.fromsiteno_F; };
    tgg91_SessionNo    &session                ()  { return this->variant.C_1.session_F; };
    tgg00_TfnTemp      &ft                     ()  { return this->variant.C_1.ft_F; };
    tsp00_Uint1        &fs_no                  ()  { return this->variant.C_1.fs_no_F; };
    tsp00_C1           &fparschar              ()  { return this->variant.C_2.fparschar_F; };
    tsp00_C3           &fcmd_count             ()  { return this->variant.C_2.fcmd_count_F; };
    tsp00_Int2         &ffill                  ()  { return this->variant.C_2.ffill_F; };
    tgg00_TfnTemp      &ft1                    ()  { return this->variant.C_2.ft1_F; };
    tsp00_Uint1        &fs_no1                 ()  { return this->variant.C_2.fs_no1_F; };
    tgg00_Surrogate    &tabid                  ()  { return this->variant.C_3.tabid_F; };
    /* const accessor functions */
    const tgg00_ServerdbNo &fromsiteno             () const  { return this->variant.C_1.fromsiteno_F; };
    const tgg91_SessionNo &session                () const  { return this->variant.C_1.session_F; };
    const tgg00_TfnTemp &ft                     () const  { return this->variant.C_1.ft_F; };
    const tsp00_Uint1  &fs_no                  () const  { return this->variant.C_1.fs_no_F; };
    const tsp00_C1     &fparschar              () const  { return this->variant.C_2.fparschar_F; };
    const tsp00_C3     &fcmd_count             () const  { return this->variant.C_2.fcmd_count_F; };
    const tsp00_Int2   &ffill                  () const  { return this->variant.C_2.ffill_F; };
    const tgg00_TfnTemp &ft1                    () const  { return this->variant.C_2.ft1_F; };
    const tsp00_Uint1  &fs_no1                 () const  { return this->variant.C_2.fs_no1_F; };
    const tgg00_Surrogate &tabid                  () const  { return this->variant.C_3.tabid_F; };
#endif    /* defined (__cplusplus) */
} tak_fromsel_tabid;

struct tak001_TemplateDummy {
    tak_complex_info     dummy_tak_complex_info;
    tak_corr_type        dummy_tak_corr_type;
    tak_from_table_info  dummy_tak_from_table_info;
    tak_lockenum         dummy_tak_lockenum;
    tak_fs_value_qual    dummy_tak_fs_value_qual;
    tak_index_cmd_mode   dummy_tak_index_cmd_mode;
    tak_usertyp          dummy_tak_usertyp;
    tak_procs            dummy_tak_procs;
    tak_commandkind      dummy_tak_commandkind;
    tak_userparams       dummy_tak_userparams;
    tak_comment_type     dummy_tak_comment_type;
    tak_isolation_info   dummy_tak_isolation_info;
    tak_internal_sql_kind dummy_tak_internal_sql_kind;
    tak_tab_allow_state  dummy_tak_tab_allow_state;
    tak_proc_language    dummy_tak_proc_language;
    tak_sc_symbol        dummy_tak_sc_symbol;
    tak_progusagetyp     dummy_tak_progusagetyp;
    tak_recursive_state  dummy_tak_recursive_state;
    tak_balance_info     dummy_tak_balance_info;
    tak_execution_kind   dummy_tak_execution_kind;
    tak_allow_type       dummy_tak_allow_type;
    tak_fp_kind_type     dummy_tak_fp_kind_type;
    tak_queryrewrite_mode dummy_tak_queryrewrite_mode;
    tak_fcolprop         dummy_tak_fcolprop;
    tak_union_kind       dummy_tak_union_kind;
    tak_debug_mode       dummy_tak_debug_mode;
    tak_inv_only         dummy_tak_inv_only;
    tak_complex_call_reason dummy_tak_complex_call_reason;
    tak_ddl_descriptor   dummy_tak_ddl_descriptor;
    tak_jcolprop         dummy_tak_jcolprop;
    tak_object_type      dummy_tak_object_type;
    tak_viewscantype     dummy_tak_viewscantype;
    tak_proc_compile_type dummy_tak_proc_compile_type;
    tak_odbc_functiontype dummy_tak_odbc_functiontype;
    tak_convert_type     dummy_tak_convert_type;
    tak_select_fetch_type dummy_tak_select_fetch_type;
    tak_resstate         dummy_tak_resstate;
    tak_decresstate      dummy_tak_decresstate;
    tak_directory_state  dummy_tak_directory_state;
    tak_appl_type        dummy_tak_appl_type;
    tak_role_info        dummy_tak_role_info;
    tak_mem_type         dummy_tak_mem_type;
    tak_cmd_part_type    dummy_tak_cmd_part_type;
    tak_priv_descriptors dummy_tak_priv_descriptors;
    tak_function         dummy_tak_function;
    tak_jv_upd_description dummy_tak_jv_upd_description;
    tak_proc_kind        dummy_tak_proc_kind;
    tak_number_changetype dummy_tak_number_changetype;
    tak_needed_spec      dummy_tak_needed_spec;
    tak_explain_kind     dummy_tak_explain_kind;
    tak_param_state      dummy_tak_param_state;
    tak_query_type       dummy_tak_query_type;
    tak_odbc_partype     dummy_tak_odbc_partype;
    tak_fields_to_use    dummy_tak_fields_to_use;
    tak_changetype       dummy_tak_changetype;
    tak_messbockFieldSequenceInfo dummy_tak_messbockFieldSequenceInfo;
    tak_usertab_descriptor dummy_tak_usertab_descriptor;
    tak_sresstate        dummy_tak_sresstate;
    tak_rollback_info    dummy_tak_rollback_info;
    tak_userrectype      dummy_tak_userrectype;
    tak_jcolpropset      dummy_tak_jcolpropset;
    tak_priv_desc        dummy_tak_priv_desc;
    tak_columnset        dummy_tak_columnset;
    tak_complex_infoset  dummy_tak_complex_infoset;
    tak_command_states   dummy_tak_command_states;
    tak_scanner_states   dummy_tak_scanner_states;
    tak_one_table_specialname dummy_tak_one_table_specialname;
    tak_fcolpropset      dummy_tak_fcolpropset;
    tak_number_changetype_set dummy_tak_number_changetype_set;
    tak_symbolset        dummy_tak_symbolset;
    tak_table_attribute_set dummy_tak_table_attribute_set;
    tak_view_attributes  dummy_tak_view_attributes;
    tak_joinset          dummy_tak_joinset;
    tak_sysdir_state     dummy_tak_sysdir_state;
    tak_querycheck_node  dummy_tak_querycheck_node;
    tak_parsinfo_states  dummy_tak_parsinfo_states;
    tak_charset          dummy_tak_charset;
    tak_queryrewrite_exec dummy_tak_queryrewrite_exec;
    tak_special_node_info dummy_tak_special_node_info;
};


#endif
