
      tak_complex_call_reason  =  (c_increment, c_set_last_key,
            c_set_last_pars, c_set_p_no, c_set_recursive);
      tak_object_type = (obj_domain, obj_table, obj_temp_table,
            obj_trigger);
      tak_allow_type = (
            at_has_to_be_there,
            at_may_be_there,
            at_not_allowed);
      tak_progusagetyp  =  (p_notyp,
            p_column,
            p_file,
            p_index,
            p_synonym,
            p_table,
            p_user,
            p_view,
            p_domain,
            p_procedure,
            p_foreign_key);
      tak_sc_symbol  = (s_unknown,
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
            s_updated, (* PTS 1127075 *)
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
            s_utcdate,            (* PTS 1116174 E.Z. *)
            s_get_schema,         (* PTS 1111577 E.Z. *)
            s_get_name,           (* PTS 1111577 E.Z. *)
            s_case,               (* PTS 1117523 E.Z. *)
            s_now,                (* PTS 1125159 E.Z. *)
            s_current_schema);    (* PTS 1132105 E.Z. *)
      tak_procs  = (no_proc,
            a10, a11, a12, a13, a14, a15, a16, a17, a18, a19,
            a20, a21, a22, a23, a24, a25, a26, a27, a28, a29,
            a30, a31, a32, a33, a34, a35, a36, a37, a38, a39,
            a40, a41, a42, a43, a44, a45, a46, a47, a48, a49,
            a50, a51, a52, a53, a54, a55, a56, a57, a58, a59,
            a60, a61, a62, a63, a64, a65, a66, a67, a68, a69,
            a70, a71, a72, a73, a74, a75, a76, a77, a78, a79,
            a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90,
            a63query_spec, a92fromsel, a260, a261, a262, a264, a641, a999ptr_node);
      tak_usertyp  =  (usuperdba, udba, uprivate, unoprivate, usysdba,
            ucontroluser);
      tak_ddl_descriptor  =  (no_ddl,
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
            ddl_create_filler2,   (* unused, was ddl_create_serverdb *)
            ddl_create_package,
            ddl_create_procedure,
            ddl_create_role,
            ddl_create_filler1,   (* unused  PTS 1111576 E.Z. *)
            ddl_create_synonym,
            ddl_create_table,
            ddl_create_user,
            ddl_create_view,
            ddl_db2,
            ddl_drop_domain,
            ddl_drop_index,
            ddl_drop_link,
            ddl_drop_filler2,      (* unused, was ddl_drop_serverdb *)
            ddl_drop_package,
            ddl_drop_procedure,
            ddl_drop_role,
            ddl_drop_filler1,      (* unused  PTS 1111576 E.Z. *)
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
            ddl_rename_index,      (* PTS 1111229 E.Z. *)
            ddl_unused2,                    (* unused *)
            ddl_unused3,                    (* unused *)
            ddl_unused4,                    (* unused *)
            ddl_truncate,
            ddl_unused5,                    (* unused  PTS 1111576 E.Z. *)
            ddl_unused6,                    (* unused  PTS 1111576 E.Z. *)
            ddl_create_schema,
            ddl_alter_table,
            ddl_drop_schema,
            ddl_unused8,
            ddl_alter_dbproc,
            ddl_rename_user,
            ddl_create_dbfunc,
            ddl_alter_trigger);
      tak_proc_compile_type = (
            pct_none,
            pct_dbproc,
            pct_function,
            pct_insert_trigger,
            pct_update_trigger,
            pct_delete_trigger,
            pct_multi_trigger);
      tak_convert_type = (conv_none,
            conv_ascii_to_unicode,
            conv_unicode_to_ascii,
            conv_ascii_to_num,
            conv_unicode_to_num,
            conv_num_to_ascii,
            conv_num_to_unicode);
      (* *)
      tak_table_attribute_set = SET OF (
            ta_fact,
            ta_dimension,
            ta_bwhierarchy,
            ta_no_references_view,
            ta_no_fixed_length_column,
            ta_cluster_key);
      (* *)
      tak_two_int2       = ARRAY [1..2] OF tsp00_Int2;
 
      tak_connect_rec = RECORD
            cn_username       : tsp00_KnlIdentifier;
            cn_timeout        : tsp00_Int2;
            cn_password       : tsp00_CryptPw;
            cn_global_state   : tgg00_HandlingSet;
            cn_tmp_cache_size : tsp00_Int4;
            cn_receivedPw     : tsp00_C8;
            cn_messcode       : tsp00_Uint1;
            cn_internal       : boolean;
            cn_mysql_connect  : boolean;
            cn_oldVersion     : boolean;
      END;
 
 
      tak_sm_description  =  RECORD
            sm_error_cnt      : tsp00_Int2;
            sm_must_local     : boolean;
            sm_return_req     : boolean;
            sm_not_local      : boolean;
            sm_filler1        : boolean;
            sm_expected_error : tgg00_BasisError;
            sm_errorlist      : ARRAY[ 1..cak_max_sm_error ] OF
                  tgg00_BasisError;
      END;
 
 
      tak_user_info = RECORD
            ui_id      : tgg00_Surrogate;
            ui_owner   : tgg00_Surrogate;
            ui_is_role : boolean;
            ui_kind    : tak_usertyp;
      END;
 
 
      tak_parskey  =  RECORD
            p_count  : tsp00_C3;
            p_id     : tsp00_C1;     (* commandno *)
            p_kind   : tgg00_MessType;
            p_no     : tsp00_Uint1;   (* max buf_no *)
            p_filler : tsp00_Int2;
      END;
 
 
      tak_parsid = RECORD
            pid_session : tgg91_SessionNo;
            CASE boolean OF
                true :
                    (pid_parsk : tak_parskey);
                false :
                    (pid_fill1     : tsp00_C4;
                    pid_fill2      : tsp00_C2;
                    pid_appl_info  : tsp00_C1;
                    pid_dtime_info : tsp00_C1);
                END;
            (*ENDCASE*) 
 
      tak_queryrewrite_exec = SET OF (
            qrwMergeF, qrwMergeE,
            qrwConvertExist, qrwConvertToExist,
            qrwDistinctPullUp, qrwDistinctPushDownTo, qrwDistinctPushDownFrom, qrwDistinctForSubqueries,
            qrwSimplifyPredicates, qrwEliminateGroupByOrDistinct, qrwEliminateOrderBy,
            qrwPushDownPredicates, qrwPushDownProjection, qrwPushDownQuantifier,
            qrwOptimizeSubquery
            );
      tak_queryrewrite_mode = (
            No_Rewrite, Statement_Rewrite, Operator_Rewrite
            );
      tak_special_node_info = SET OF
            (ni_complex_predicate, ni_complex_string_id,
            ni_info_node, ni_skip_node, ni_blank_skip,
            ni_correlated_column);                   (* ni_correlated_column used with n_symb=s_columnid only *)
      tak_querycheck_node = SET OF
            (
            qcn_FixedPos,               (* n_pos does not describe some position with the statement     *)
            qcn_SubQInfo,               (* subquery type for complex view replacement in queryrewrite   *)
            qcn_RightsChecked,          (* indicate that privileges were aleady checked by queryrewrite *)
            qcn_SpecialJoinFromSelect,  (* indicate a special join fromselect                           *)
            qcn_ContainsSJFromSelect,   (* indicates that there is a special from select below          *)
            qcn_Filler1,
            qcn_Filler2,
            qcn_Filler3
            );
 
      tak_ap_node  =  RECORD (* UNIX needs 16+8 bytes for this *)
            n_proc     : tak_procs;
            n_special  : tak_special_node_info;
            n_symb     : tak_sc_symbol;
            n_datatype : tsp00_DataType;
            n_lo_level : tsp00_Int2; (*see nodepointer in glob1*)
            n_sa_level : tsp00_Int2;
            (* PTS 1128197 D.T. *)
            n_refback   : tsp00_Int2;            (* 'pointer' to father node in tree *)
            n_querycheck: tak_querycheck_node;
            n_dt_format : tgg00_DateTimeFormat;
            (* *)
            CASE integer OF
                1 :                                  (* used in all cases except those descriped in 2 *)
                    (n_subproc  : tsp00_Int2;        (* the order of these three MUST not be changed  *)
                    n_length   : tsp00_Int2;         (* as a01node_get_ptr needs that *)
                    n_pos      : tsp00_Int4);
                2 :                                  (* used with n_symb=s_columnid and s_tableid *)
                    (n_fromtabnode : tsp00_Int2;
                    n_columnindex  : tsp00_Int2;     (* will be filled with cextcolno *)
                    n_filler  : tsp00_Int4);
                END;
            (*ENDCASE*) 
 
      tak_ap_max_tree   =  ARRAY [ 0..cak_max_node ] OF tak_ap_node;
      tak_ap_max_tree_ptr  = ^tak_ap_max_tree;
      tak_scanner_states = SET OF
            (scs_reserved_check,
            scs_pack (*TRUE = shorten id. with '''*),
            scs_hint_allowed);
 
      tak_scanner_glob  =  RECORD      (* variable : scv *)
            sc_symb           : tak_sc_symbol;
            sc_eof_symb       : tak_sc_symbol;
            sc_sylength       : tsp00_Int2;
            sc_sypos          : tsp00_Int4;
            sc_newpos         : tsp00_Int4;
            sc_lastpos        : tsp00_Int4;
            sc_missing_kw_pos : tsp00_Int4;
            sc_double_quote   : tsp00_Int2;
            sc_states         : tak_scanner_states;
            sc_filler         : boolean;
            sc_eof_kw         : tsp00_Int2;
      END;
 
      (* PTS 1111575 E.Z. *)
      tak_execution_kind  =  (only_parsing,
            only_executing,
            parsing_executing,
            pars_then_execute,
            only_syntax);
      tak_charset       =  SET OF char;
      tak_sqlm_charset  =  ARRAY[tsp00_SqlMode] OF tak_charset;
      tak_columnset     =  SET OF 0..MAX_COL_PER_TAB_GG00;
      tak_symbolset     =  SET OF tak_sc_symbol;
      tak_datapart      =  PACKED ARRAY [ 1..csp_maxint2 ] OF char;
      tak_eop_arr       =  PACKED ARRAY [ 1..4096] OF tgg00_StackOpType;
      tak_colindex      =  PACKED ARRAY [ 1..MAX_COL_PER_TAB_GG00] OF tsp00_Int2;
      tak_colinteger    =  PACKED ARRAY [ 1..MAX_COL_PER_TAB_GG00] OF tsp00_Int2;
      tak_colsequence   =  PACKED ARRAY [ 1..MAX_COL_SEQUENCE_GG00 ] OF tsp00_Int2;
      tak_inv_colnos    =  PACKED ARRAY [ 1..32 ] OF tsp00_Int2;
      tak_corlevel      =  ARRAY [ 1..cak_maxcorlevel ] OF tsp00_Uint1;
      tak_keyword       =  PACKED ARRAY [ 1..18 ] OF char;
      tak_oldidentifier =  tsp00_C18;
      tak_keywordtab    =  ARRAY [1..cak_maxkeyword]  OF tak_keyword;
      tak_order_errortext = PACKED ARRAY [ 1..mxak_order_errortext ] OF char;
      tak_parsbuffer    =  PACKED ARRAY [ 1..mxak_parsbuf ]    OF char;
      tak_16stackentry  =  ARRAY [ 1..MAX_COL_SEQUENCE_GG00 ] OF
            tgg00_StackEntry;
      tak_upd_stat_info  = ARRAY [ 1..MAX_COL_PER_TAB_GG00 ] OF tsp00_Int4;
      tak_oms_version_id = PACKED ARRAY [ 1..22 ] OF char;
      tak_oms_version_desc = PACKED ARRAY [ 1..512 ] OF char; (* PTS 1117690 *)
      tak_sysbufferoffset = PACKED ARRAY [ 1..cak_sysbufferoffset ] OF char;
      tak_eop_arr_ptr    = ^tak_eop_arr;
      (* PTS 1109291 E.Z. *)
      tak_pcountarr      = PACKED ARRAY [ 1..cak_max_pcounts ] OF tsp00_C3;
      (* PTS 1114411 T.A. *)
      tak_WydeArray      = ARRAY[1..MOVEINT2_MXSP00] OF tsp00_Int2;
 
      tak_del_tab_qual  =  RECORD
            del_qual_cnt : tsp00_Int2;
            del_colno    : tsp00_Int2;
            del_qual     : ARRAY[ 1..cak_max_del_qual ] OF tsp00_C2;
      END;
 
 
      tak_role_context = RECORD
            rctxt_cnt   : tsp00_Int4;
            rctxt_roles : ARRAY[1..cak_max_role_context] OF tgg00_Surrogate;
      END;
 
      tak_role_context_ptr = ^tak_role_context;
      tak_map_set = PACKED ARRAY [ 1..mxak_mapset  ] OF char;
 
      tak_mapset_record = RECORD
            map_reclen        : tsp00_Int2;
            map_keylen        : tsp00_Int2;
            map_varcol_offset : tsp00_Int2;
            map_varcol_cnt    : tsp00_Int2;
            map_surrogate     : tgg00_Surrogate;
            map_entrytype     : tsp00_C2;
            map_linkage       : tsp00_C2;
            map_name          : tsp00_KnlIdentifier;
            map_segmentid     : tsp00_C2;
            map_count         : tsp00_Int2;
            map_code          : tsp00_CodeType; (* ascii, unicode *)
            map_filler1       : boolean;
            map_filler2       : tsp00_Int2;
            map_set           : tak_map_set;
      END;
 
      (* privilege catalog representation *)
      tak_priv_descriptors  =  (priv_col_sel, priv_col_upd,
            priv_col_sel_grant, priv_col_upd_grant,
            priv_col_ref, priv_col_ref_grant);
      tak_priv_desc  =  SET OF tak_priv_descriptors;
 
      tak_privilege  =  RECORD    (* 132 B *)
            CASE integer OF
                1 :
                    (priv_all_set      : tak00_PrivilegeSet;
                    priv_all_grant_set : tak00_PrivilegeSet;
                    priv_col_exist     : tak_priv_desc;
                    priv_filler        : boolean;
                    priv_sel_set       : tak_columnset;
                    priv_upd_set       : tak_columnset;
                    priv_grant_sel_set : tak_columnset;
                    priv_grant_upd_set : tak_columnset;
                    priv_ref_set       : tak_columnset;
                    priv_grant_ref_set : tak_columnset);
                2 :
                    (priv_filler1      : tsp00_C4;
                    priv_col : ARRAY[ tak_priv_descriptors ] OF  tak_columnset);
                3 :
                    (priv_c132        : tsp00_C132);
                4 :
                    (priv_c196 : PACKED ARRAY[1..196] OF char);
                END;
            (*ENDCASE*) 
 
 
      tak_privrecord = RECORD
            pr_rec_len       : tsp00_Int2;
            pr_key_len       : tsp00_Int2;
            pr_varcol_offset : tsp00_Int2;
            pr_varcol_cnt    : tsp00_Int2;
            pr_tabid         : tgg00_Surrogate;
            pr_entrytype     : tsp00_C2;
            pr_linkage       : tsp00_C2;
            pr_userid        : tgg00_Surrogate;
            pr_segmentid     : tsp00_C2;
            pr_filler        : tsp00_C2;
            pr_priv          : tak_privilege;
      END;
 
 
      tak_privuserrecord = RECORD
            pru_rec_len       : tsp00_Int2;
            pru_key_len       : tsp00_Int2;
            pru_varcol_offset : tsp00_Int2;
            pru_varcol_cnt    : tsp00_Int2;
            pru_tabid         : tgg00_Surrogate;
            pru_entrytype     : tsp00_C2;
            pru_linkage       : tsp00_C2;
            pru_grantee       : tgg00_Surrogate;
            pru_grantor       : tgg00_Surrogate;
            pru_segmentid     : tsp00_C2;
            pru_filler        : tsp00_C2;
            pru_date          : tsp00_Int4;
            pru_time          : tsp00_Int4;
            pru_priv          : tak_privilege;
      END;
 
 
      tak_sprivilege  =  RECORD    (* 68 B *)
            priv_all_set       : tak00_PrivilegeSet;
            priv_col_exist     : tak_priv_desc;
            priv_filler        : tsp00_Int2;
            priv_sel_set       : tak_columnset;
            priv_upd_set       : tak_columnset;
      END;
 
 
      tak_proc_seq_privilege = RECORD
            psp_reclen        : tsp00_Int2;
            psp_keylen        : tsp00_Int2;
            psp_varcol_offset : tsp00_Int2;
            psp_varcol_cnt    : tsp00_Int2;
            psp_id            : tgg00_Surrogate;
            psp_type          : tsp00_C2;
            psp_linkage       : tsp00_C2;
            psp_grantor       : tgg00_Surrogate;
            psp_seqmentid     : tsp00_C2;
            psp_priv          : tak00_PrivilegeSet;
            psp_grantOption   : boolean;
            psp_date          : tsp00_Int4;
            psp_time          : tsp00_Int4;
      END;
 
 
      tak_column_statistics  =  RECORD
            c_values       : tsp00_Int4;
            c_pages        : tsp00_Int4;
            c_avg_list_len : tsp00_Int4;
      END;
 
 
      tak_colsort  =  RECORD
            colbuf : tsp00_Int4;
            colind : tsp00_Int4;
            colpos : tsp00_Int4;
      END;
 
      tak_colsortarr  =  ARRAY[ 1..MAX_COL_PER_TAB_GG00 ] OF tak_colsort;
      (* domain catalog representation *)
 
      tak_domainref  =  RECORD
            dreclen        : tsp00_Int2;
            dkeylen        : tsp00_Int2;
            dvarcol_offset : tsp00_Int2;
            dvarcol_cnt    : tsp00_Int2;
            downer         : tgg00_Surrogate;
            dentrytype     : tsp00_Int2;
            dseqno         : tsp00_Int2;
            dname          : tsp00_KnlIdentifier;
            dsegmentid     : tsp00_C2;
            dfiller        : tsp00_C2;
            dsurrogate     : tgg00_Surrogate;
      END;
 
 
      tak_domainrecord  =  RECORD
            dom_reclen        : tsp00_Int2;
            dom_keylen        : tsp00_Int2;
            dom_varcol_offset : tsp00_Int2;
            dom_varcol_cnt    : tsp00_Int2;
            dom_surrogate     : tgg00_Surrogate;
            dom_entrytype     : tsp00_C2;
            dom_seqno         : tsp00_Int2;
            dom_segmentid     : tsp00_C2;
            dom_filler        : tsp00_Uint1;
            dom_constraint    : boolean;
            dom_type_uid      : tsp00_C16;
            dom_owner         : tgg00_Surrogate;
            dom_date          : tsp00_Int4;
            dom_time          : tsp00_Int4;
            dom_colinfo       : tak00_columninfo;
      END;
 
 
      tak_col_uses_dom = RECORD
            cud_reclen        : tsp00_Int2;
            cud_keylen        : tsp00_Int2;
            cud_varcol_offset : tsp00_Int2;
            cud_varcol_cnt    : tsp00_Int2;
            cud_surrogate     : tgg00_Surrogate;
            cud_entrytype     : tsp00_C2;
            cud_linkage       : tsp00_C2;
            cud_segmentid     : tsp00_C2;
            cud_filler1       : tsp00_C2;
            cud_owner         : tgg00_Surrogate;
            cud_name          : tsp00_KnlIdentifier;
      END;
 
 
      tak_sequencerecord  =  RECORD
            seq_reclen        : tsp00_Int2;
            seq_keylen        : tsp00_Int2;
            seq_varcol_offset : tsp00_Int2;
            seq_varcol_cnt    : tsp00_Int2;
            seq_surrogate     : tgg00_Surrogate;
            seq_entrytype     : tsp00_C2;
            seq_linkage       : tsp00_C2;
            seq_segmentid     : tsp00_C2;
            seq_comment       : boolean;
            seq_public        : boolean;
            seq_date          : tsp00_Int4;
            seq_time          : tsp00_Int4;
            seq_value         : tsp00_Number;
            seq_owner         : tgg00_Surrogate;
            seq_schema        : tgg00_Surrogate;
            seq_info          : tgg00_SeqInfo;
            seq_name          : tsp00_KnlIdentifier;
      END;
 
 
      tak_tablereference  =  RECORD
            rtablerefkey1 : tak_sysbufferoffset;
            rtablerefkey2 : tsp00_KnlIdentifier;
            rsegmentid    : tsp00_C2;
            rsystable     : boolean;
            rfiller       : boolean;
            rtablekind    : tgg00_TableKind;
            rundefbyte    : boolean;
            rtableid      : tgg00_Surrogate;
            rsynid        : tgg00_Surrogate;
      END;
 
      (* table structure and column catalog representation *)
&     ifdef bit64
      tak_columnarr  = ARRAY [ 1..1280] OF tak00_colinfo_ptr;
&     else
      tak_columnarr  = ARRAY [ 1..1536] OF tak00_colinfo_ptr;
&     endif
      tak_basefiller = PACKED ARRAY [ 1..16 ] OF char;
      (* default / range catalog representation *)
&     ifdef bit64
      tak_align = tsp00_C8;
&     else
      tak_align = tsp00_C4;
&     endif
      tak_basecolinfo_ptr = ^tak_basecolinfo;
      (* 4096 has NOTHING to do with tsp00_Buf or so *)
 
      tak_basecolinfo = RECORD
            bci_index   : tsp00_Int4;
            bci_rec_cnt : tsp00_Int2;
            bci_ref_cnt : tsp00_Int2;
            bci_buf_size: tsp00_Int2;
            bci_fill1   : tsp00_Int2;
            bci_fill2   : tsp00_Int4;
            bci_next    : tak_basecolinfo_ptr;
            CASE boolean OF
                true :
                    (bci_align : ARRAY[1..cak_max_align] OF tak_align);
                false :
                    (bci_buf : PACKED ARRAY [1..4096] OF char);
                END;
            (*ENDCASE*) 
 
      tak_view_attributes = SET OF (
            va_correlation,
            va_date_time_used,
            va_unused1,
            va_unused2,
            va_unused3,
            va_unused4,
            va_unused5,
            va_unused6);
 
      tak_baserecord  =  RECORD   (* 3996 B *)
            breclen          : tsp00_Int2;
            bkeylen          : tsp00_Int2;
            bvarcol_offset   : tsp00_Int2;
            bvarcol_cnt      : tsp00_Int2;
            bsurrogate       : tgg00_Surrogate;
            bentrytype       : tsp00_C2;
            bseqno           : tsp00_C2;
            bsegmentid       : tsp00_C2;
            bfirstindex      : tsp00_Int2;
            blastindex       : tsp00_Int2;
            btablekind       : tgg00_TableKind;
            blinkexist       : tgg00_Linkset;
            btreeid          : tgg00_FileId;
            bauthid          : tgg00_Surrogate;
            bschema          : tgg00_Surrogate;
            bschemacontext   : tgg00_Surrogate;
            breccnt          : tsp00_Int2;    (* 77/78 *)
            bmaxcol          : tsp00_Int2;
            bmaxreclen       : tsp00_Int2;
            bkeycolcount     : tsp00_Int2;
            blenfixedcol     : tsp00_Int2;
            bvarcolcount     : tsp00_Int2;
            bcntdropcol      : tsp00_Int2;
            bstringcount     : tsp00_Int2;
            blongvarcolcnt   : tsp00_Uint1;
            bcomment         : boolean;
            bavgrowlen       : tsp00_Int2;
            blastkeyind      : tsp00_Int2;
            bfirstcolind     : tsp00_Int2;           (* 100 *)
            bdatecreate      : tsp00_Int4;
            btimecreate      : tsp00_Int4;
            bupdstatdate     : tsp00_Int4;
            bupdstattime     : tsp00_Int4;           (* 116 *)
            bpages           : tsp00_Int4;
            brows            : tsp00_Int4;
            balterdate       : tsp00_Int4;
            baltertime       : tsp00_Int4;
            bsample          : tsp00_Int4;
            bpriv_all_set    : tak00_PrivilegeSet;
            bpriv_col_exist  : tak_priv_desc;
            bunloaded        : boolean;
            bindexexist      : boolean;
            bv_level         : tsp00_Uint1;
            bv_tabcount      : tsp00_Uint1;
            bv_tablekind     : tgg00_TableKind;
            bv_checkopt      : boolean;
            bv_qualexist     : boolean;
            bv_distinct      : tgg04_Distinct;
            bv_viewlist      : boolean;
            bdef_stamp       : boolean;
            bview_attributes : tak_view_attributes;
            bsqlmode         : tsp00_SqlMode;
            bnamed_constr    : tsp00_Int2;
            bshow_kind       : tgg00_ShowKind;
            brecreate_view   : boolean;
            bshowkind        : tgg00_ShowKind;
            battributes      : tak_table_attribute_set;
            btablen          : tsp00_KnlIdentifierPtr;
            bptr             : tak_basecolinfo_ptr;
            bcolumnExtension : ^tak_columnarr;
            bcolumnSplitIdx  : tsp00_Int2;
            bupdstatinserted : boolean;
            CASE integer OF
                1 :
                    (bextcolindex : tak_colindex);
                2 :
                    (bcolumn : tak_columnarr);
                END;
            (*ENDCASE*) 
            (* default / range catalog representation *)
 
      tak_default_value = ARRAY[1..cak_maxdefaultvalue] OF char; (* PTS 1108428 *)
 
      tak_defaultrecord  =  RECORD
            dfreclen           : tsp00_Int2;
            dfkeylen           : tsp00_Int2;
            dfvarcol_offset    : tsp00_Int2;
            dfvarcol_cnt       : tsp00_Int2;
            dfsurrogate        : tgg00_Surrogate;
            dfentrytype        : tsp00_C2;
            dfseqno            : tsp00_C2;
            dfsegment_id       : tsp00_C2;
            dfdefault_function : tsp00_Int2;
            dfvalues           : tak_default_value; (* PTS 1108428 *)
      END;
 
      tak_comment_type = (
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
            cm_sequence);
      tak_ak_constr_stack  =  ARRAY[ 1..cak_maxconstraintstack ] OF
            tgg00_StackEntry;
 
      tak_alterdaterecord = RECORD
            ad_rec_len       : tsp00_Int2;
            ad_key_len       : tsp00_Int2;
            ad_varcol_offset : tsp00_Int2;
            ad_varcol_cnt    : tsp00_Int2;
            ad_surrogate     : tgg00_Surrogate;
            ad_entrytype     : tsp00_C2;
            ad_linkage       : tsp00_C2;
            ad_segmentid     : tsp00_C2;
            ad_filler        : tsp00_C2;
            ad_createdate    : tsp00_Int4;
            ad_createtime    : tsp00_Int4;
            ad_alterdate     : tsp00_Int4;
            ad_altertime     : tsp00_Int4;
      END;
 
      tak_comment_text = ARRAY[1..mxak_comment] OF char;
 
      tak_comment_record = RECORD
            com_rec_len       : tsp00_Int2;
            com_key_len       : tsp00_Int2;
            com_varcol_offset : tsp00_Int2;
            com_varcol_cnt    : tsp00_Int2;
            com_surrogate     : tgg00_Surrogate;
            com_entrytype     : tsp00_C2;
            com_linkage       : tsp00_C2;
            com_type          : tak_comment_type;
            com_colno         : tsp00_C1;
            com_segmentid     : tsp00_C2;
            com_length        : tsp00_Int4;
            com_text          : tak_comment_text;
      END;
 
 
      tak_constraintrecord  =  RECORD
            crec_len       : tsp00_Int2;
            ckey_len       : tsp00_Int2;
            cvarcol_offset : tsp00_Int2;
            cvarcol_cnt    : tsp00_Int2;
            csurrogate     : tgg00_Surrogate;
            centrytype     : tsp00_C2;
            cseqno         : tsp00_C2;
            csegmentid     : tsp00_C2;
            cstack_cnt     : tsp00_Int2;
            cvalue_pos     : tsp00_Int2;
            cfunction      : boolean;
            coldrange      : boolean;
            ccolset        : tak_columnset;
            ccname         : tak_oldidentifier;
            cstackstart    : tsp00_Uint1;
            csuffixlen     : tsp00_Uint1;
            cfiller        : tsp00_Int2;
            CASE boolean OF
                true :
                    (cstack     : tak_ak_constr_stack);
                false :
                    (csuffix : tsp00_KnlIdentifier);
                END;
            (*ENDCASE*) 
 
 
      tak_one_f_proc = RECORD
            ofp_procid   : tgg00_Surrogate;
            ofp_language : tsp00_C3;
            ofp_sqlmode  : SET OF tsp00_SqlMode;
      END;
 
      tak_funcref_restkey = ARRAY[1..56] OF char;
 
      tak_funcrefrecord = RECORD
            fct_reclen        : tsp00_Int2;
            fct_keylen        : tsp00_Int2;
            fct_varcol_offset : tsp00_Int2;
            fct_varcol_cnt    : tsp00_Int2;
            fct_key1          : tsp00_C8;
            fct_entrytype     : tsp00_C2;
            fct_linkage       : tsp00_C2;
            fct_key2          : tak_funcref_restkey;
            fct_segmentid     : tsp00_C2;
            fct_proc_cnt      : tsp00_Uint1;
            fct_comment       : boolean;
            fct_fill2         : tsp00_Int4;
            fct_procids       : ARRAY[1..cak_max_proc] OF tak_one_f_proc;
      END;
 
 
      tak_constraintnamerecord  =  RECORD
            cnreclen        : tsp00_Int2;
            cnkeylen        : tsp00_Int2;
            cnvarcol_offset : tsp00_Int2;
            cnvarcol_cnt    : tsp00_Int2;
            cnkey1          : tsp00_C12;
            cnkey2          : tsp00_KnlIdentifier;
            cnsegmentid     : tsp00_C2;
            cnid            : tsp00_Int2;
            cnfiller        : tsp00_Int2;
      END;
 
      (* named index catalog representation *)
 
      tak_multindex  =  RECORD    (* 212 *)
            indexn_prefix : tak_oldidentifier;
            indexno       : tsp00_Int2;
            icount        : tsp00_Uint1;
            ifill         : boolean;
            icolseq       : tak_colsequence;
            iunique       : boolean;
            icomment      : boolean;
            idatecre      : tsp00_Int4;
            itimecre      : tsp00_Int4;
            iinitdate     : tsp00_Int4;
            ifiller2      : tsp00_Int4; (* was: ipages *)
            iinittime     : tsp00_Int4;
            ifiller       : boolean;     (* unused *)
            idisabled     : boolean;
            ifiller1      : boolean; (* PTS 1114312 *)
            isuffixlen    : tsp00_Uint1;
            icolstack     : tak_16stackentry;
      END;
 
 
      tak_mindexrecord  =  RECORD   (* 3396 *)
            ireclen        : tsp00_Int2;
            ikeylen        : tsp00_Int2;
            ivarcol_offset : tsp00_Int2;
            ivarcol_cnt    : tsp00_Int2;
            isurrogate     : tgg00_Surrogate;
            ientrytype     : tsp00_C2;
            iseqno         : tsp00_C2;
            isegmentid     : tsp00_C2;
            indexcount     : tsp00_Uint1;
            indexnext      : boolean;
            indexdef       : ARRAY [ 1..cak_max_index_per_rec ] OF tak_multindex;
      END;
 
      (* link catalog representation *)
 
      tak_linkdef  =  RECORD   (* 216 *)
            ltableid      : tgg00_Surrogate;
            linkn_prefix  : tak_oldidentifier;
            lfiller1      : tsp00_Int2;           (* unused *)
            ldatecre      : tsp00_Int4;
            ltimecre      : tsp00_Int4;
            ldatatyp      : tsp00_DataType;
            laction       : tsp00_Uint1;
            lcolcount     : tsp00_Uint1;
            lindexid      : tsp00_C1;
            lcomment      : boolean;
            lsuffixlen    : tsp00_Uint1;
            lfiller2      : tsp00_Int2;
            lprimcolseq   : tak_colsequence;
            lseccolseq    : tak_colsequence;
            lrecpos       : tak_colsequence;
            lstack        : tak_16stackentry;
      END;
 
 
      tak_linkrecord  =  RECORD
            lreclen        : tsp00_Int2;
            lkeylen        : tsp00_Int2;
            lvarcol_offset : tsp00_Int2;
            lvarcol_cnt    : tsp00_Int2;
            lsurrogate     : tgg00_Surrogate;
            lentrytype     : tsp00_C2;
            lseqno         : tsp00_C2;
            lsegmentid     : tsp00_C2;
            linkcount      : tsp00_Int2;
            linkdef        : ARRAY [ 1..cak_maxlinkdef ]   OF tak_linkdef;
      END;
 
 
      tak_link_def_record  =  RECORD
            lreclen        : tsp00_Int2;
            lkeylen        : tsp00_Int2;
            lvarcol_offset : tsp00_Int2;
            lvarcol_cnt    : tsp00_Int2;
            lsurrogate     : tgg00_Surrogate;
            lentrytype     : tsp00_C2;
            lsegno         : tsp00_C2;
            lkeyname       : tsp00_KnlIdentifier;
            lsegmentid     : tsp00_C2;
            ldefault_len   : tsp00_Int2;
            sfiller        : tsp00_Int2;
            ldefault_key   : tsp00_Key;
      END;
 
 
      tak_linkparsinfo = RECORD
            lptreeid      : tgg00_FileId;
            lpauxfile     : tgg00_FileId;
            lpfiller      : tsp00_Int2;          (* unused *)
            lpcolcount    : tsp00_Uint1;
            lp_one_to_one : boolean;
            lpstack       : tak_16stackentry;
      END;
 
 
      tak_linkparsrecord  =  RECORD
            lpreclen        : tsp00_Int2;
            lpkeylen        : tsp00_Int2;
            lpvarcol_offset : tsp00_Int2;
            lpvarcol_cnt    : tsp00_Int2;
            lpsurrogate     : tgg00_Surrogate;
            lpentrytype     : tsp00_C2;
            lpseqno         : tsp00_C2;
            lpsegmentid     : tsp00_C2;
            lpcount         : tsp00_Int2;
            lpinfo          : ARRAY [ 1..cak_maxlinkdef ] OF tak_linkparsinfo;
      END;
 
      tak_decresstate  =  (
            dr_ansi,
            dr_db2,
            dr_db2_with_hold,
            dr_oracle,
            dr_sql_db);
      tak_resstate  =  (
            rs_result,
            rs_noresult,
            rs_intinvnoresult,
            rs_nobase,
            rs_useinv,
            rs_joinupdate,
            rs_show,
            rs_reuseable
            );
      tak_sresstate  =  SET OF tak_resstate;
      tak_select_fetch_type = (
            sft_normal_select,
            sft_select_all_results,
            sft_select_some_results);
 
      tak_resname_record  =  RECORD
            resreclen         : tsp00_Int2;
            reskeylen         : tsp00_Int2;
            resvarcol_offset  : tsp00_Int2;
            resvarcol_cnt     : tsp00_Int2;
            reskey_idfill     : tgg00_Surrogate;
            reskey_identr     : tsp00_C2;
            reskey_name       : tsp00_KnlIdentifier;
            reskey_modul_name : tsp00_KnlIdentifier;
            resinfobuf        : tsp00_Uint1;
            resparsinfobyte   : tsp00_Uint1;
            restreeid         : tgg00_FileId;
            resLongFileId     : tsp00_Int4;
            resunlocktabid    : tgg00_Surrogate;
            restrans          : tgg91_TransNo;
            resscrollable     : boolean;
            ressubfill        : boolean;            (* unused *)
            ressubtrans       : tgg00_SubtransNo;   (* PTS 1109485 E.Z. *)
            reseparsk         : tak_parskey;
            resstate          : tak_sresstate;
            resmaxlinkage     : tsp00_Uint1;
            resdecresdel      : tak_decresstate;
            resdatetimeformat : tgg00_DateTimeFormat;
            resimpl_upd_stat  : boolean;
            resexecuted       : boolean;
            resselect_fetch   : tak_select_fetch_type;
            resanalyze_cmdid  : tgg00_Surrogate;
            resconsistentview : tgg91_TransNo;
            resfiller         : tsp00_Int2;
      END;
 
      tak_resn_rec_arr = ARRAY [1..cak_max_resn] OF tak_resname_record;
      (* trigger representation *)
      tak_proc_language = (
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
            lang_other);
      tak_trigger_stack = ARRAY[1..cak_max_trigger_stack] OF tgg00_StackEntry;
 
      tak_triggerrecord = RECORD
            tr_reclen         : tsp00_Int2;
            tr_keylen         : tsp00_Int2;
            tr_varcol_offset  : tsp00_Int2;
            tr_varcol_cnt     : tsp00_Int2;
            tr_surrogate      : tgg00_Surrogate;
            tr_entrytype      : tsp00_C2;
            tr_linkage        : tsp00_C2;
            tr_segmentid      : tsp00_C2;
            tr_param_cnt      : tsp00_Int2;
            tr_condition_cnt  : tsp00_Int2;
            tr_succ_exists    : boolean;
            tr_comment        : boolean;
            tr_proc_id        : tgg00_Surrogate;
            tr_date           : tsp00_Int4;
            tr_time           : tsp00_Int4;
            tr_upd_set        : tak_columnset; (* 130 *)
            tr_maxUpdatedColno: tsp00_Int2;
            tr_language       : tak_proc_language;
            tr_name_len       : tsp00_Uint1;
            CASE boolean OF
                true :
                    (tr_name  : tsp00_KnlIdentifier);
                false :
                    (tr_stack : tak_trigger_stack);
                END;
            (*ENDCASE*) 
            (* usage catalog representation *)
 
 
      tak_usagedef  =  RECORD   (* 12 B *)
            usa_tableid   : tgg00_Surrogate;
            usa_filler    : tsp00_Int2;           (* unused *)
            usa_tablekind : tgg00_TableKind;
            usa_empty     : boolean;
      END;
 
 
      tak_usagerecord  =  RECORD
            usagereclen        : tsp00_Int2;
            usagekeylen        : tsp00_Int2;
            usagevarcol_offset : tsp00_Int2;
            usagevarcol_cnt    : tsp00_Int2;
            usagesurrogate     : tgg00_Surrogate;
            usageentrytype     : tsp00_C2;
            usageseqno         : tsp00_C2;
            usagesegmentid     : tsp00_C2;
            usagefiller        : tsp00_Int2;
            usagecount         : tsp00_Int2;
            usagenext_exist    : boolean;
            usagefull          : boolean;
            usagedef           : ARRAY [ 1..cak_max_usage ] OF tak_usagedef;
      END;
 
      (* usertab catalog representation *)
      tak_usertab_descriptor  =  (ut_table,
            ut_base_table,
            ut_procedure,
            ut_sequence,
            ut_internal_systable,
            ut_oracle_systable,
            ut_view,
            ut_filler,            (* unused  PTS 1111576 E.Z. *)
            ut_role);
 
      tak_usertabdef  =  RECORD   (* 12 *)
            ut_surrogate : tgg00_Surrogate;
            ut_empty     : boolean;
            ut_kind      : tak_usertab_descriptor;
            ut_filler    : tsp00_Int2;          (* unused *)
      END;
 
 
      tak_usertablerecord  =  RECORD
            usatabreclen        : tsp00_Int2;
            usatabkeylen        : tsp00_Int2;
            usatabvarcol_offset : tsp00_Int2;
            usatabvarcol_cnt    : tsp00_Int2;
            usatabsurrogate     : tgg00_Surrogate;
            usatabentrytype     : tsp00_C2;
            usatabseqno         : tsp00_C2;
            usertabsegmentid    : tsp00_C2;
            usertabfiller       : tsp00_Int2;
            usertabnext_exist   : boolean;
            usertabfull         : boolean;
            usertabcount        : tsp00_Int2;
            usertabdef          : ARRAY [ 1..cak_max_usertab ] OF
                  tak_usertabdef;
      END;
 
      (* user catalog representation *)
      tak_userparams  =  (schemaid, up_unused2, maxtimeout,
            costwarning, costlimit, cachelimit);
      tak_userpar_arr =  ARRAY [ tak_userparams ] OF tsp00_Int4;
      tak_userrectype =  SET OF (is_user_rec, is_group_rec, is_role_rec, is_schema_rec);
      tak_role_info   =  (role_all, role_none, role_except);
 
      tak_userrecord  =  RECORD  (* 120 *)
            ureclen        : tsp00_Int2;
            ukeylen        : tsp00_Int2;
            uvarcol_offset : tsp00_Int2;
            uvarcol_cnt    : tsp00_Int2;
            usurrogate     : tgg00_Surrogate;
            uentrytype     : tsp00_C2;
            useqno         : tsp00_C2;
            usersegmentid  : tsp00_C2;
            userchardefcode: tsp00_CodeType; (* PTS 1117216 E.Z. *)
            userroleinfo   : tak_role_info;
            urecordtyp     : tak_userrectype;
            userkind       : tak_usertyp;
            usersysdba     : boolean;
            uexclusive     : boolean;
            ureplication   : boolean;
            userconnect    : boolean;
            userfiller3    : tsp00_Int2;
            userpassw      : tsp00_CryptPw;
            usupportpasspw : tsp00_CryptPw;
            usergroup_id   : tgg00_Surrogate;
            userowner_id   : tgg00_Surrogate;
            upwcreatedate  : tsp00_Int4;
            upwcreatetime  : tsp00_Int4;
            ucreatedate    : tsp00_Int4;
            ucreatetime    : tsp00_Int4;
            uparams        : tak_userpar_arr;
            uperm_ref      : tgg00_UserRef;
            ucomment       : boolean;
            userrolecnt    : tsp00_Uint1;
            ualterdate     : tsp00_Int4;
            ualtertime     : tsp00_Int4;
            username       : tsp00_KnlIdentifier;
      END;
 
 
      tak_useridrecord = RECORD
            us_reclen        : tsp00_Int2;
            us_keylen        : tsp00_Int2;
            us_varcol_offset : tsp00_Int2;
            us_varcol_cnt    : tsp00_Int2;
            us_surrogate     : tgg00_Surrogate;
            us_entrytype     : tsp00_C2;
            us_linkage       : tsp00_C2;
            us_segmentid     : tsp00_C2;
            us_filler        : tsp00_C2;
            us_userid        : tsp00_C4;
      END;
 
      (* user reference catalog representation username  ==> user_id *)
      tak_uref_restkey = ARRAY[1..56] OF char;
 
      tak_userrefrecord  =  RECORD
            ru_key_offset : tak_sysbufferoffset;
            ru_rest_key   : tak_uref_restkey;
            ru_segmentid  : tsp00_C2;
            ru_filler     : tsp00_Int2;
            ru_user_id    : tgg00_Surrogate;
      END;
 
      (* system user catalog representation *)
 
      tak_sysdbarecord  =  RECORD
            sysreclen        : tsp00_Int2;
            syskeylen        : tsp00_Int2;
            sysvarcol_offset : tsp00_Int2;
            sysvarcol_cnt    : tsp00_Int2;
            syssurrogate     : tgg00_Surrogate;
            sysentrytyp      : tsp00_C2;
            syssegno         : tsp00_C2;
            syssegmentid     : tsp00_C2;
            sysactivated     : boolean;
            sysversion       : tsp00_Uint1;
            sysauthid        : tgg00_Surrogate;
            sysKnlVersion    : tsp00_Version;
      END;
 
      (* location catalog representation *)
 
      tak_serverdbrecord  =  RECORD
            sitereclen        : tsp00_Int2;
            sitekeylen        : tsp00_Int2;
            sitevarcol_offset : tsp00_Int2;
            sitevarcol_cnt    : tsp00_Int2;
            sitekey           : tsp00_C12;
            sitesegmentid     : tsp00_C2;
            sitemap_no        : tgg00_ServerdbNo;
            siteserverdbname  : tsp00_DbName;
            sitetype          : tgg00_ServerdbType;
            sitefiller1       : boolean;
            siteid            : tsp00_NodeId;
            siterefcount      : tsp00_Int4;
      END;
 
 
      tak_serverdbnamerecord  =  RECORD
            snreclen        : tsp00_Int2;
            snkeylen        : tsp00_Int2;
            snvarcol_offset : tsp00_Int2;
            snvarcol_cnt    : tsp00_Int2;
            snkey1          : tsp00_C12;
            snkey2          : tsp00_DbName;
            snsegmentid     : tsp00_C2;
            snnodeid        : tsp00_NodeId;
            snsite          : tgg00_ServerdbNo;
            snsitetype      : tgg00_ServerdbType;
            sncode          : tsp00_CodeType;
            sntermchar      : tak_oldidentifier;
            snremote_site   : tgg00_ServerdbNo;
            snlocal_site    : tgg00_ServerdbNo;
            snfiller2       : tsp00_Int2;
      END;
 
      (* stored procedure catalog representation *)
      tak_param_spec_ptr = ^tsp1_param_spec_list;
 
      tak_param_list = RECORD
            param_spec_ptr    : tak_param_spec_ptr;
            param_counter     : tsp00_Int2;
            param_filler1     : tsp00_Int2;
            param_filler2     : tsp00_Int4;
      END;
 
      tak_param_state = (param_undef, param_new, param_old, param_const);
 
      tak_param_info  =  RECORD
            param_datatype        : tsp00_DataType;
            param_datafrac        : tsp00_Uint1;
            param_datalength      : tsp00_Int2;
            param_inoutlength     : tsp00_Int2;
            param_in_out          : tsp00_C2;
            param_com_datatype    : tsp00_Int2;
            param_type_id         : tsp00_C2;
            param_cpp_offset      : tsp00_Int2;
            param_ascii_offset    : tsp00_Int2;
            param_unicode_offset  : tsp00_Int2;
            param_state           : tak_param_state;
            param_name_len        : char;
            param_name            : tsp00_C256;
      END;
 
      tak_param_info_ptr = ^tak_param_info;
      tak_param_array  =    ARRAY [ 1..cak_max_param_index ] OF
            tak_param_info_ptr;
      tak_proc_kind = (p_dbproc, p_trigger, p_function);
      tak_debug_mode = (dbg_inproc, dbg_local);
 
      tak_methodrecord = RECORD
            me_reclen              : tsp00_Int2;
            me_keylen              : tsp00_Int2;
            me_varcol_offset       : tsp00_Int2;
            me_varcol_cnt          : tsp00_Int2;
            me_surrogate           : tgg00_Surrogate;
            me_entrytype           : tsp00_C2;
            me_linkage             : tsp00_C2;
            me_segmentid           : tsp00_C2;
            me_param_cnt           : tsp00_Int2;
            CASE boolean OF
                true :
                    (me_type       : tsp00_DbObjectType;
                    me_debug       : tak_debug_mode;
                    me_sql         : boolean;
                    me_cursor      : boolean;
                    me_dispid      : tsp00_Int4;
                    me_coclsid     : tsp00_C16;
                    me_iid         : tsp00_C16;
                    me_package_id  : tgg00_Surrogate;
                    me_param_list  : tak_param_array);
                false :
                    (me_typefill   : tsp00_DbObjectType;
                    me_debugfill   : tak_debug_mode;
                    me_sqlfill     : boolean;
                    me_cursorfill  : boolean;
                    (* caution : actually 2. variant starts here,        *)
                    (*           has been done this way due to alignment *)
                    me_language      : tak_proc_language;
                    me_deterministic : boolean;
                    me_filler2       : tsp00_Int2;
                    me_return_pos    : tsp00_Int4;
                    me_return_len    : tsp00_Int4;
                    me_owner         : tgg00_Surrogate);
                END;
            (*ENDCASE*) 
 
 
      tak_method_inforecord = RECORD
            mei_reclen         : tsp00_Int2;
            mei_keylen         : tsp00_Int2;
            mei_varcol_offset  : tsp00_Int2;
            mei_varcol_cnt     : tsp00_Int2;
            mei_surrogate      : tgg00_Surrogate;
            mei_entrytype      : tsp00_C2;
            mei_linkage        : tsp00_C2;
            mei_segmentid      : tsp00_C2;
            mei_comment        : boolean;
            mei_filler         : boolean;
            mei_owner          : tgg00_Surrogate;
            mei_schema         : tgg00_Surrogate;
            mei_date           : tsp00_Int4;
            mei_time           : tsp00_Int4;
            mei_prog_id        : tsp00_C64;
            mei_name           : tsp00_KnlIdentifier;
            mei_language       : tsp00_KnlIdentifier;
      END;
 
 
      tak_debug_info = RECORD
            dbg_stack_entry_index : tsp00_Int2;
            dbg_code_length       : tsp00_Int2;
            dbg_code_pos          : tsp00_Int4;
      END;
 
 
      tak_debug_mapping_info = RECORD
            dmi_extcolno  : tsp00_Int2;
            dmi_rec_pos   : tsp00_Int2;
      END;
 
 
      tak_method_debuginfo = RECORD
            med_reclen         : tsp00_Int2;
            med_keylen         : tsp00_Int2;
            med_varcol_offset  : tsp00_Int2;
            med_varcol_cnt     : tsp00_Int2;
            med_surrogate      : tgg00_Surrogate;
            med_entrytype      : tsp00_C2;
            med_linkage        : tsp00_C2;
            med_segmentid      : tsp00_C2;
            med_count          : tsp00_Int2;
            CASE boolean OF
                true :
                    (med_info    : ARRAY[1..cak_max_debug_info] OF tak_debug_info);
                false :
                    (med_mapping : ARRAY[1..cak_max_debug_mapping] OF tak_debug_mapping_info);
                END;
            (*ENDCASE*) 
 
      tak_method_filename = ARRAY[1..1025] OF char;
 
      tak_method_fileinforecord = RECORD
            mef_reclen         : tsp00_Int2;
            mef_keylen         : tsp00_Int2;
            mef_varcol_offset  : tsp00_Int2;
            mef_varcol_cnt     : tsp00_Int2;
            mef_surrogate      : tgg00_Surrogate;
            mef_entrytype      : tsp00_C2;
            mef_linkage        : tsp00_C2;
            mef_segmentid      : tsp00_C2;
            mef_filler         : tsp00_C2;
            mef_md5_footprint  : tsp00_C32;
            mef_filename       : tak_method_filename;
      END;
 
 
      tak_methodrefrecord = RECORD
            mrf_reclen        : tsp00_Int2;
            mrf_keylen        : tsp00_Int2;
            mrf_varcol_offset : tsp00_Int2;
            mrf_varcol_cnt    : tsp00_Int2;
            mrf_surrogate     : tgg00_Surrogate;
            mrf_entrytype     : tsp00_C2;
            mrf_linkage       : tsp00_C2;
            mrf_methodname    : tsp00_KnlIdentifier;
            mrf_segmentid     : tsp00_C2;
            mrf_type          : tsp00_DbObjectType;
            mrf_filler        : boolean;
            mrf_method_id     : tgg00_Surrogate;
      END;
 
 
      tak_type_uid_map_record = RECORD
            tum_reclen        : tsp00_Int2;
            tum_keylen        : tsp00_Int2;
            tum_varcol_offset : tsp00_Int2;
            tum_varcol_cnt    : tsp00_Int2;
            tum_uid1          : tsp00_C8;
            tum_entrytype     : tsp00_C2;
            tum_linkage       : tsp00_C2;
            tum_uid2          : tsp00_C8;
            tum_segmentid     : tsp00_C2;
            tum_id            : tsp00_C2;
      END;
 
 
      tak_class_map_guid_record = RECORD
            cmr_reclen        : tsp00_Int2;
            cmr_keylen        : tsp00_Int2;
            cmr_varcol_offset : tsp00_Int2;
            cmr_varcol_cnt    : tsp00_Int2;
            cmr_surrogate     : tgg00_Surrogate;
            cmr_entrytype     : tsp00_C2;
            cmr_linkage       : tsp00_C2;
            cmr_segmentid     : tsp00_C2;
            cmr_container_no  : tsp00_Int4;
            cmr_schema        : tsp00_Int4;
            cmr_guid          : tsp00_C16;
      END;
 
 
      tak_class_record = RECORD
            freclen        : tsp00_Int2;
            fkeylen        : tsp00_Int2;
            fvarcol_offset : tsp00_Int2;
            fvarcol_cnt    : tsp00_Int2;
            fschema        : tgg00_Surrogate;
            fentrytype     : tsp00_C2;
            flinkage       : tsp00_C2;
            fguid          : tsp00_C16;
            fsegmentid     : tsp00_C2;
            findexexist    : boolean;
            ffiller        : boolean;
            fsize          : tsp00_Int4;
            ftree          : tgg00_FileId;
            fdate          : tsp00_Int4;
            ftime          : tsp00_Int4;
            fclassname     : tsp00_KnlIdentifier;
      END;
 
 
      tak_schema_ref_record = RECORD
            schr_reclen       : tsp00_Int2;
            schr_keylen       : tsp00_Int2;
            schrvarcol_offset : tsp00_Int2;
            schrvarcol_cnt    : tsp00_Int2;
            schr_surrogate    : tgg00_Surrogate;
            schr_entrytype    : tsp00_C2;
            schr_linkage      : tsp00_C2;
            schr_name         : tsp00_KnlIdentifier;
            schr_segmentid    : tsp00_C2;
            schr_filler       : tsp00_Int4;
            schr_schema_id    : tsp00_Int4;
      END;
 
 
      tak_schema_record = RECORD
            sch_reclen        : tsp00_Int2;
            sch_keylen        : tsp00_Int2;
            sch_varcol_offset : tsp00_Int2;
            sch_varcol_cnt    : tsp00_Int2;
            sch_surrogate     : tgg00_Surrogate;
            sch_entrytype     : tsp00_C2;
            sch_linkage       : tsp00_C2;
            sch_segmentid     : tsp00_C2;
            sch_filler        : tsp00_C2;
            sch_date          : tsp00_Int4;
            sch_time          : tsp00_Int4;
            sch_name          : tsp00_KnlIdentifier;
      END;
 
 
      tak_version_record = RECORD
            vreclen        : tsp00_Int2;
            vkeylen        : tsp00_Int2;
            vvarcol_offset : tsp00_Int2;
            vvarcol_cnt    : tsp00_Int2;
            vsurrogate     : tgg00_Surrogate;
            ventrytype     : tsp00_C2;
            vlinkage       : tsp00_C2;
            vversion_id    : tak_oms_version_id;
            vsegmentid     : tsp00_C2;
            vconsistview   : tgg91_TransNo;
            vmarked        : boolean;
            vfiller        : boolean;
            vdate          : tsp00_Int4;
            vtime          : tsp00_Int4;
            vupdtree       : tgg00_FileId;
      END;
 
      (* synonym catalog representation *)
 
      tak_synonymrecord  =  RECORD   (* 128 *)
            synreclen        : tsp00_Int2;
            synkeylen        : tsp00_Int2;
            synvarcol_offset : tsp00_Int2;
            synvarcol_cnt    : tsp00_Int2;
            synsurrogate     : tgg00_Surrogate;
            synentrytype     : tsp00_C2;
            synseqno         : tsp00_C2;
            synsegmentid     : tsp00_C2;
            syn_maxcol       : tsp00_Int2;
            syn_internal     : boolean; (* PTS 1109242 *)
            synfiller0       : boolean;
            syn_tablekind    : tgg00_TableKind;
            syncomment       : boolean;
            syn_tableid      : tgg00_Surrogate;
            syn_time         : tsp00_Int4;
            syn_date         : tsp00_Int4;
            syn_owner        : tgg00_Surrogate;
            syn_authid       : tgg00_Surrogate;
            syn_tablen       : tsp00_KnlIdentifier;
      END;
 
      (* cammand info catalog representation *)
 
      tak_commandinforecord = RECORD
            cireclen        : tsp00_Int2;
            cikeylen        : tsp00_Int2;
            civarcol_offset : tsp00_Int2;
            civarcol_cnt    : tsp00_Int2;
            cisurrogate     : tgg00_Surrogate;
            cientrytype     : tsp00_C2;
            cilinkage       : tsp00_C2;
            cicommand_id    : tsp00_C8;
      END;
 
      (*! don't remove/regroup any of following enum types because this !*)
      (*! type will be made persitent on view catalog records           !*)
      tak_jcolprop  =  (jtkey, jtfirstkey, jtonlykey, jtfiller1,
            jtmulti, jtfiller2, jtascii);
      tak_jcolpropset  =  SET OF tak_jcolprop;
      tak_joinset      =  PACKED SET OF 1..cak00_maxsources;
 
      tak_one_joinpart  =  PACKED RECORD
            jop_multiplier   : tsp00_Int4;     (* ~ distinctvalue *)
            jop_fieldno      : tsp00_Int2;     (* record column position *)
            jop_startstack   : tsp00_Int2;     (* stack code position *)
            jop_cntstack     : tsp00_Int2;     (* stackentry count for join description *)
            jop_tableno      : tsp00_Int2;     (* index in tak_all_from_tables *)
            jop_filler0      : tsp00_Int2;
            jop_inoutlen     : tsp00_Int2;     (* from tak_colinfo *)
            jop_outpos       : tsp00_Int2;
            jop_datatyp      : tsp00_DataType; (* from tak_colinfo *)
            jop_propset      : tak_jcolpropset;
            jop_outer_join   : boolean;
            jop_filler       : tsp00_C3;
      END;
 
 
      tak_one_join  =  PACKED RECORD
            jo_recs          : ARRAY [ 1..2 ] OF tak_one_joinpart;
            jo_op            : tgg00_StackOpType;
            jo_no_join       : boolean;
            jo_partno        : tsp00_Uint1;
            jo_col_upd_stat  : tsp00_Uint1;
      END;
 
      tak_joinarr  =  ARRAY [ 0..MAX_JOINARR_ELEMS_AK00 ] OF tak_one_join;
      tak_joinarr_ptr = ^tak_joinarr;
 
      tak_joinrec  =  RECORD
            jrc_joinarr : tak_joinarr_ptr;
&           ifdef BIT64
&           else
            jrc_alpha_fill : PACKED ARRAY [ 1..4 ] OF char;
&           endif
            jrc_cnt         : tsp00_Int2;
            jrc_capacity    : tsp00_Int2;
            jrc_col_upd     : boolean;
            jrc_filler      : tsp00_C3;
      END;
 
      (*======== view catalog representation ========*)
      (* view text catalog representation *)
      tak_from_table_info = ( fti_none, fti_synonym, fti_withoutSchema );
 
      tak_vttabdef  =  RECORD   (* 24 B *)
            vtttableid       : tgg00_Surrogate;
            vttreftabid      : tgg00_Surrogate;
            vtttabcount      : tsp00_Int2;
            vttrefname       : boolean;
            vttfiller        : tsp00_Uint1;          (* unused *)
            vttfromTableInfo : tak_from_table_info;
            vttcomplex_view  : boolean;
            vtttab_n_pos     : tsp00_Int2;
      END;
 
      tak_viewtext  =  PACKED ARRAY [ 1..mxak_viewtext  ] OF char;
 
      tak_viewtextrecord  =  RECORD    (* 3992 *)
            vtreclen        : tsp00_Int2;
            vtkeylen        : tsp00_Int2;
            vtvarcol_offset : tsp00_Int2;
            vtvarcol_cnt    : tsp00_Int2;
            vtsurrogate     : tgg00_Surrogate;
            vtentrytype     : tsp00_C2;
            vtsegno         : tsp00_C2;
            vtsegmentid     : tsp00_C2;
            vtfromtabcnt    : tsp00_Int2;
            vttabcount      : tsp00_Int2;
            vttextlength    : tsp00_Int2;
            vtnextexist     : boolean;
            vtcontext       : tsp00_Uint1;
            vtselect_pos    : tsp00_Int2;
            CASE boolean OF
                true :
                    (vttab : ARRAY [ 1..cak00_maxsources ] OF tak_vttabdef);
                false :
                    (vttbuf : tak_viewtext);
                END;
            (*ENDCASE*) 
            (* view column desription catalog representation *)
 
 
      tak_vcolumndescription  =  PACKED RECORD
            vfromtabno    : tsp00_Int2;
            vfromextcolno : tsp00_Int2;
            vn_pos        : tsp00_Int4;
            vextcolno     : tsp00_Int2;
            vdatatype     : tsp00_DataType;
            vfiller       : boolean;
      END;
 
 
      tak_viewdescrecord  =  RECORD
            vdreclen        : tsp00_Int2;
            vdkeylen        : tsp00_Int2;
            vdvarcol_offset : tsp00_Int2;
            vdvarcol_cnt    : tsp00_Int2;
            vdsurrogate     : tgg00_Surrogate;
            vdentrytype     : tsp00_C2;
            vdsegno         : tsp00_Int2;
            vsegmentid      : tsp00_C2;
            vdesc_cnt       : tsp00_Int2;
            vdesc_next      : boolean;
            vdesc_fill1     : boolean;
            vdesc_fill2     : tsp00_Int2;
            vdescription    : ARRAY[ 1..cak_maxviewdesc ] OF
                  tak_vcolumndescription;
      END;
 
      (* view qualification catalog representation *)
 
      tak_viewtabdef  =  RECORD   (* 44 Bytes *)
            vttableid        : tgg00_Surrogate;
            vtfiller         : tsp00_Int2;      (* unused *)
            vtins_for_upd    : boolean;
            vtone_to_one     : boolean;
            vtnot_used_links : tak_charset;
      END;
 
      tak_viewtabarr  =  ARRAY[ 1..cak_max_viewqual_tab ] OF tak_viewtabdef;
 
      tak_one_viewjoin  =  PACKED RECORD
            j1fieldno    : tsp00_Int2;
            j1tableno    : tsp00_Int2;
            j1cntstack   : tsp00_Int2;
            j1inoutlen   : tsp00_Int2;
            j1outpos     : tsp00_Int2;
            j1startstack : tsp00_Int2;
            j1propset    : tak_jcolpropset;
            j12partno    : tsp00_Uint1;
            j2fieldno    : tsp00_Int2;
            j2tableno    : tsp00_Int2;
            j2cntstack   : tsp00_Int2;
            j2inoutlen   : tsp00_Int2;
            j2outpos     : tsp00_Int2;
            j2propset    : tak_jcolpropset;
            j12operator  : tgg00_StackOpType;
            j1datatype   : tsp00_DataType;
            j2datatype   : tsp00_DataType;
      END;
 
      tak_viewjoins  =  ARRAY [ 1..cak_max_viewqual_join ] OF
            tak_one_viewjoin;
      tak_view_stack_entries   =   ARRAY [ 1..cak_max_viewqual_stack ] OF
            tgg00_StackEntry;
      tak_viewbuf  =  PACKED ARRAY [ 1..cak_max_viewqual_data  ] OF char;
 
      tak_one_derived_desc  =  PACKED RECORD
            drecno    : tsp00_Int2;
            dextno    : tsp00_Int2;
            dexttabno : tsp00_Int2;
            dtabno    : tsp00_Int2;
      END;
 
      tak_derived_col_info  =  ARRAY[ 1..cak_max_viewqual_derivedcol ] OF
            tak_one_derived_desc;
 
      tak_viewqual_basis_record = RECORD
            vreclen         : tsp00_Int2;
            vkeylen         : tsp00_Int2;
            vvarcol_offset  : tsp00_Int2;
            vvarcol_cnt     : tsp00_Int2;
            vsurrogate      : tgg00_Surrogate;
            ventrytype      : tsp00_C2;
            vlinkage        : tsp00_C2;
            vsegmentid      : tsp00_C2;
            vfiller         : boolean;
            vjoin_exists    : boolean;
            vstack_exists   : boolean;
            vderived_exists : boolean;
            vbasetabcnt     : tsp00_Int2;
            vtable          : tak_viewtabarr;
      END;
 
 
      tak_viewqual_join_record = RECORD
            vreclen         : tsp00_Int2;
            vkeylen         : tsp00_Int2;
            vvarcol_offset  : tsp00_Int2;
            vvarcol_cnt     : tsp00_Int2;
            vsurrogate      : tgg00_Surrogate;
            ventrytype      : tsp00_C2;
            vlinkage        : tsp00_C2;
            vsegmentid      : tsp00_C2;
            vjoincount      : tsp00_Int2;
            vjoin           : tak_viewjoins;
      END;
 
 
      tak_viewqual_stack_record = RECORD
            vreclen         : tsp00_Int2;
            vkeylen         : tsp00_Int2;
            vvarcol_offset  : tsp00_Int2;
            vvarcol_cnt     : tsp00_Int2;
            vsurrogate      : tgg00_Surrogate;
            ventrytype      : tsp00_C2;
            vlinkage        : tsp00_C2;
            vsegmentid      : tsp00_C2;
            vfiller         : tsp00_C2;
            vview_offs      : tsp00_Int2;
            vstcount        : tsp00_Int2;
            vdatapos        : tsp00_Int4;
            vdatalen        : tsp00_Int4;
            CASE boolean OF
                true:
                    (vstack : tak_view_stack_entries);
                false:
                    (vdata  : tak_viewbuf);
                END;
            (*ENDCASE*) 
 
 
      tak_viewqual_derivedcol_record = RECORD
            vreclen         : tsp00_Int2;
            vkeylen         : tsp00_Int2;
            vvarcol_offset  : tsp00_Int2;
            vvarcol_cnt     : tsp00_Int2;
            vsurrogate      : tgg00_Surrogate;
            ventrytype      : tsp00_C2;
            vlinkage        : tsp00_C2;
            vsegmentid      : tsp00_C2;
            vderivedcnt     : tsp00_Int2;
            vderived_cols   : tak_derived_col_info;
      END;
 
 
      tak_viewkeyrecord  =  RECORD
            vkreclen        : tsp00_Int2;
            vkkeylen        : tsp00_Int2;
            vkvarcol_offset : tsp00_Int2;
            vkvarcol_cnt    : tsp00_Int2;
            vkkey           : tsp00_C12;
            vksegmentid     : tsp00_C2;
            vkjvkeylen      : tsp00_Int2;
            vkjvkey         : tsp00_Key;
      END;
 
 
      tak_col_stats = RECORD
            cst_dist_values : tsp00_Int4;
            cst_pages       : tsp00_Int4;
            cst_avg_lst_len : tsp00_Int4;
      END;
 
 
      tak_statisticsinfo = RECORD
            sti_reclen        : tsp00_Int2;
            sti_keylen        : tsp00_Int2;
            sti_varcol_offset : tsp00_Int2;
            sti_varcol_cnt    : tsp00_Int2;
            sti_tabid         : tgg00_Surrogate;
            sti_entrytype     : tsp00_C2;
            sti_linkage       : tsp00_C2;
            sti_segmentid     : tsp00_C2;
            sti_col_cnt       : tsp00_Int2;
            sti_rows          : tsp00_Int4;
            sti_pages         : tsp00_Int4;
            sti_date          : tsp00_Int4;
            sti_time          : tsp00_Int4;
            sti_filler        : tsp00_Int2;
            sti_index         : tak_columnset;  (* 130 Bytes *)
            sti_col_stats     : ARRAY[1..MAX_COL_STATS_AK00] OF tak_col_stats;
      END;
 
      (* result structure catalog representation *)
 
      tak_resultrecord  =  RECORD   (* 3996 B *)
            breclen        : tsp00_Int2;
            bkeylen        : tsp00_Int2;
            bvarcol_offset : tsp00_Int2;
            bvarcol_cnt    : tsp00_Int2;
            bkey           : tsp00_C12;
            bsegmentid     : tsp00_C2;
            bfirstindex    : tsp00_Int2;
            blastindex     : tsp00_Int2;
            btablekind     : tgg00_TableKind;
            blinkexist     : tgg00_Linkset;
            btreeid        : tgg00_FileId;
            bauthid        : tgg00_Surrogate;
            bschema        : tgg00_Surrogate;
            bschemacontext : tgg00_Surrogate;
            breccnt        : tsp00_Int2;    (* 77/78 *)
            bmaxcol        : tsp00_Int2;
            bmaxreclen     : tsp00_Int2;
            bkeycolcount   : tsp00_Int2;
            blenfixedcol   : tsp00_Int2;
            bvarcolcount   : tsp00_Int2;
            bcntdropcol    : tsp00_Int2;
            bstringcount   : tsp00_Int2;
            blongvarcolcnt : tsp00_Uint1;
            bcomment       : boolean;
            bavgrowlen     : tsp00_Int2;
            blastkeyind    : tsp00_Int2;
            bfirstcolind   : tsp00_Int2;           (* 100 *)
            bdatecreate    : tsp00_Int4;
            btimecreate    : tsp00_Int4;
            bupdstatdate   : tsp00_Int4;
            bupdstattime   : tsp00_Int4;           (* 116 *)
            bpages         : tsp00_Int4;
            brows          : tsp00_Int4;
            balterdate     : tsp00_Int4;
            baltertime     : tsp00_Int4;
            bsample        : tsp00_Int4;
            boutcolno      : tsp00_Int2;
            bunloaded      : boolean;
            bindexexist    : boolean;
            bv_level       : tsp00_Uint1;
            bv_tabcount    : tsp00_Uint1;
            bv_tablekind   : tgg00_TableKind;
            bv_checkopt    : boolean;
            bv_qualexist   : boolean;
            bv_distinct    : tgg04_Distinct;
            bv_viewlist    : boolean;
            bdef_stamp     : boolean;
            bview_attributes : tak_view_attributes;
            bsqlmode       : tsp00_SqlMode;
            bnamed_constr  : tsp00_Int2;
            bshow_kind     : tgg00_ShowKind;
            brecreate_view : boolean;
            bshowkind      : tgg00_ShowKind;
            battributes    : tak_table_attribute_set;
            btablen        : tsp00_KnlIdentifierPtr;
            bptr           : tak_basecolinfo_ptr;
            bcolumnExtension : ^tak_columnarr;
            bcolumnSplitIdx  : tsp00_Int2;
            bupdstatinserted : boolean;
            CASE integer OF
                1 :
                    (bextcolindex : tak_colindex);
                2 :
                    (bcolumn : tak_columnarr);
                END;
            (*ENDCASE*) 
 
 
      tak_named_subtrans_info = RECORD
            str_name : tsp00_KnlIdentifier;
            str_no   : tgg00_SubtransNo;
      END;
 
 
      tak_subtransrecord = RECORD
            str_reclen        : tsp00_Int2;
            str_keylen        : tsp00_Int2;
            str_varcol_offset : tsp00_Int2;
            str_varcol_cnt    : tsp00_Int2;
            str_key           : tgg00_Surrogate;
            str_etype         : tsp00_C2;
            str_linkage       : tsp00_C2;
            str_count         : tsp00_Int2;
            str_filler        : tsp00_Int2;
            str_subtrans      : ARRAY[ 1..cak_max_named_subtrans ] OF
                  tak_named_subtrans_info;
      END;
 
 
      tak_scoldescrecord  =  RECORD
            scd_reclen        : tsp00_Int2;
            scd_keylen        : tsp00_Int2;
            scd_varcol_offset : tsp00_Int2;
            scd_varcol_cnt    : tsp00_Int2;
            scd_colkey        : tgg00_Surrogate;
            scd_etype         : tsp00_C2;
            scd_filler        : tsp00_C2;   (* always 0 *)
            scd_stringfd      : tgg04_StringFileDescr;
            scd_lock          : tgg00_Lock;
            scd_key           : tsp00_Key;
      END;
 
      tak_changetype = (ch_to_ascii,
            ch_number_change_in_next_entry,
            ch_date,
            ch_time_or_timestamp,
            ch_to_multibyte_set,
            ch_to_longsupport,
            ch_to_warn2);
      tak_number_changetype = (
            nch_nativeint8,
            nch_div_by_count);
      tak_number_changetype_set = SET OF tak_number_changetype;
 
      tak_change_colinfo  =  RECORD
            ch_startpos : tsp00_Int2;
            ch_length   : char;
            CASE boolean OF
                true :
                    (ch_type : SET OF tak_changetype);
                false :
                    (ch_number_type : tak_number_changetype_set);
                END;
            (*ENDCASE*) 
 
      tak_column_change_arr = ARRAY [ 1..MAX_COL_PER_TAB_GG00 ] OF
            tak_change_colinfo;
 
      tak_changerecord  =  RECORD
            cr_colcount : tsp00_Int2;
            cr_was_opt  : boolean;
            cr_filler   : boolean;
            cr_columns  : tak_column_change_arr;
      END;
 
 
      tak_changeinfos = RECORD
            ci_rec_len       : tsp00_Int2;
            ci_key_len       : tsp00_Int2;
            ci_varcol_offset : tsp00_Int2;
            ci_varcol_cnt    : tsp00_Int2;
            ci_surrogate     : tgg00_Surrogate;
            ci_entrytype     : tsp00_C2;
            ci_segno         : tsp00_Int2;
            ci_fullen        : tsp00_Int4;
            ci_colcount      : tsp00_Int2;
            ci_filler        : tsp00_Int2;
            ci_ch_columns    : tak_column_change_arr;
      END;
 
 
      tak_keyspec = RECORD
            ks_pos : tsp00_Int2;
            ks_len : tsp00_Int2
      END;
 
 
      tak_two_keyspecs = RECORD
            reckeyspec  : tak_keyspec;
            listkeyspec : tak_keyspec
      END;
 
      tak_res_keysbuf = PACKED ARRAY [ 1..mxak_res_keysbuf ] OF char;
 
      tak_resultkeyrecord  =  RECORD
            res_rec_len        : tsp00_Int2;
            res_key_len        : tsp00_Int2;
            res_varcol_offset  : tsp00_Int2;
            res_varcol_cnt     : tsp00_Int2;
            res_surrogate      : tgg00_Surrogate;
            res_entrytype      : tsp00_C2;
            res_segno          : tsp00_Int2;
            res_fullen         : tsp00_Int4;
            res_startkeys      : tak_two_keyspecs;
            res_stopkeys       : tak_two_keyspecs;
            res_nextkeys       : tak_two_keyspecs;(* key > fetch next*)
            res_nextpos        : tsp00_Int4;
            res_prevkeys       : tak_two_keyspecs;(* key > fetch prev*)
            res_prevpos        : tsp00_Int4;
            res_updkey         : tak_keyspec;
            res_treeids        : tgg00_TwoFileIds;
            res_restree        : tgg00_FileId;
            res_resultname     : tsp00_KnlIdentifier;
            res_rowno          : tsp00_Int4;
            res_actres         : tsp00_Int4;  (* no. of results *)
            (*===*)
            res_reclen         : tsp00_Int2;  (* reclen incl. Update-Keys*)
            res_length         : tsp00_Int2;  (* length of output *)
            res_keylen         : tsp00_Int2;
            res_outcolno       : tsp00_Int2;
            res_useupdk        : tsp00_Int2;
            res_resstate       : tak_sresstate;
            res_decresdel      : tak_decresstate;
            res_bof            : boolean;
            res_eof            : boolean;
            res_updchar        : tsp00_C1;  (* truncate Blank/h'00' *)
            res_for_update     : boolean;
            (*===*)
            res_updtabid       : tgg00_Surrogate;
            res_unlocktab      : tgg00_Surrogate;
            res_primkeylen     : tsp00_Int2;
            res_minkeylen      : tsp00_Int2;  (* for keys with > 1 column *)
            res_dist_optim     : tsp00_Int2;
            res_strat_info     : tgg00_RefInfoIndex;
            res_parsinfob      : tsp00_Uint1;
            (*===*)
            res_pars_dt_format : tgg00_DateTimeFormat;
            res_pars_sqlmode   : tsp00_SqlMode;
            res_reverse_access : boolean;
            res_qual_kind      : tgg00_QualKind;
            res_known_pages    : tsp00_Int4;
            res_searched_pages : tsp00_Int4;
            res_upd_tabcnt     : tsp00_Int2;
            res_order          : boolean;
            res_build          : boolean;
            res_limit_offset   : tsp00_Int4;
            res_upper_limit    : tsp00_Int4;
            res_analyze_cmdid  : tgg00_Surrogate;
            CASE boolean OF
                true  :
                    (res_change  : tak_changerecord);
                false  :
                    (res_keysbuf : tak_res_keysbuf);
                END;
            (*ENDCASE*) 
 
 
      tak_shortinfo  =  RECORD   (* 8 *)
            CASE boolean OF
                true :
                    (simodeiotype : tsp00_C2;    (* mode and in/out-type *)
                    sityplen_data : tsp00_C2;    (* data type and length *)
                    sifrac_inoutl : tsp00_C2;    (* data frac and in/outlength *)
                    sibufpos      : tsp00_Int2); (* position in output buffer  *)
                false :
                    (sifiller1    : tsp00_Int4;
                    silonglength  : tsp00_Int2;
                    sifiller2     : tsp00_Int2)
                END;
            (*ENDCASE*) 
 
      tak_shortinfoarr  =  ARRAY [ 1..cak_maxshortinfo ] OF tak_shortinfo;
      tak_paraminfoarr  =  ARRAY [ 1..cak_maxparams    ] OF tsp1_param_info;
      tak_parinfarr_ptr = ^tak_paraminfoarr;
 
      tak_shortinforecord  =  RECORD   (* 32KB *)
            sireclen        : tsp00_Int2;
            sikeylen        : tsp00_Int2;
            sivarcol_offset : tsp00_Int2;
            sivarcol_cnt    : tsp00_Int2;
            sisurrogate     : tgg00_Surrogate;
            sientrytype     : tsp00_C2;
            sisegno         : tsp00_Int2;
            sifullen        : tsp00_Int4;
            sisl            : tsp00_Int2;
            sicount         : tsp00_Int2;
            siinfo          : tak_paraminfoarr
      END;
 
      tak_sysbufferaddress  = ^tak_systembuffer;
      tak_colnamesbuf = PACKED ARRAY [ 1 .. mxak_colnamesbuf ] OF char;
 
      tak_columnnamesrecord = RECORD
            cnreclen        : tsp00_Int2;
            cnkeylen        : tsp00_Int2;
            cnvarcol_offset : tsp00_Int2;
            cnvarcol_cnt    : tsp00_Int2;
            cnsurrogate     : tgg00_Surrogate;
            cnentrytype     : tsp00_C2;
            cnsegno         : tsp00_Int2;
            cnbit64fill     : tsp00_Int4;
            CASE boolean OF
                true :
                    (cnfullen  : tsp00_Int4;
                    cncount   : tsp00_Int2);
                false :
                    (cnbuf     : tak_colnamesbuf;
                    cnnext     : tak_sysbufferaddress;
                    cnlast     : tak_sysbufferaddress);
                END;
            (*ENDCASE*) 
 
      tak_fp_kind_type  = (fp_catalog_column,
            fp_unused1,                 (* unused *)
            fp_result_name,
            fp_current_of_key,
            fp_unused2,                 (* unused *)
            fp_escape_val,
            fp_fill_part2_to_len,
            fp_get_name_value,          (* PTS 1111577 E.Z. *)
            fp_ordered_select_key,
            fp_pseudo_ins_select,
            fp_reclen,
            fp_unused3,                 (* unused, PTS 1141257, T.A. *)
            fp_temp_table_key,
            fp_use_old_values_up_to_len,
            fp_verify_constraint,
            fp_verify_view,
            fp_verify_not_null,
            fp_viewkey_to_mess_buf, (* last of case in vak506 *)
            fp_val_varcol_with_len,
            fp_val_all_with_len,
            fp_val_all_without_l,
            fp_lock_val,
            fp_last_lock,
            fp_selord_val_no_len,
            fp_val_from_res_buffer,
            fp_fieldlist,          (* PTS 1116801 E.Z. *)
            fp_rowno,
            fp_limit_offset,
            fp_upper_limit);
      (**)
      tak_fp_kind_type_ptr = ^tak_fp_kind_type;
      (**)
      tak_fcolprop  =  (fp_opt, fp_like, fp_like1, fp_like2,
            fp_serial, fp_default, fp_escape);
      tak_fcolpropset  =  SET OF tak_fcolprop;
 
      tak_len_and_frac = RECORD
            CASE boolean OF
                true :
                    (lf_datalen : tsp00_Int2);
                false :
                    (lf_fixed_datalen : tsp00_Uint1;
                    lf_frac           : tsp00_Uint1)
                END;
            (*ENDCASE*) 
 
 
      tak_field_pars  =  PACKED RECORD
            fp_kind         : tak_fp_kind_type;
            fp_etype        : tgg00_StackEntryType;
            fp_datatyp      : tsp00_DataType;
            fp_colset       : tak_fcolpropset;
            CASE tak_fp_kind_type OF
                fp_catalog_column,
                fp_val_varcol_with_len,
                fp_val_all_with_len :
                    (fp_frompos_v1    : tsp00_Int4;
                    fp_movebefore_v1  : tsp00_Int2;
                    fp_rangeextcolno  : tsp00_Int2;
                    fp_len_and_frac_v1: tak_len_and_frac;
                    fp_inoutlen_v1    : tsp00_Int2);
                fp_val_all_without_l,
                fp_selord_val_no_len,
                fp_rowno,
                fp_limit_offset,
                fp_upper_limit :
                    (fp_frompos_v2    : tsp00_Int4;
                    fp_movebefore_v2  : tsp00_Int2;
                    fp_stackpos       : tsp00_Int2;
                    fp_len_and_frac_v2: tak_len_and_frac;
                    fp_inoutlen_v2    : tsp00_Int2);
                fp_lock_val :
                    (fp_frompos_v3    : tsp00_Int4;
                    fp_movebefore_v3  : tsp00_Int2;
                    fp_fill_31        : tsp00_Int2;
                    fp_len_and_frac_v3: tak_len_and_frac;
                    fp_inoutlen_v3    : tsp00_Int2);
                fp_last_lock,
                fp_reclen,
                fp_temp_table_key :
                    (fp_fill_41       : tsp00_Int4;
                    fp_movebefore_v4  : tsp00_Int2;
                    fp_fill_42        : tsp00_Int2;
                    fp_fill_43        : tsp00_Int4);
                fp_current_of_key,
                fp_viewkey_to_mess_buf :
                    (fp_curr_keylen   : tsp00_Int2;
                    fp_startpos       : tsp00_Int2;
                    fp_movebefore_v5  : tsp00_Int2;
                    fp_fill_51        : tsp00_Int2;
                    fp_pos_no         : tsp00_Int4);
                fp_val_from_res_buffer :
                    (fp_res_buf_index : tsp00_Int2;
                    fp_glob_fp_kind   : tak_fp_kind_type;
                    fp_fill_61        : boolean;
                    fp_movebefore_v6  : tsp00_Int2;
                    fp_rangeextcolno_v6: tsp00_Int2;
                    fp_len_and_frac_v6: tak_len_and_frac;
                    fp_inoutlen_v6    : tsp00_Int2);
                fp_fill_part2_to_len,
                fp_use_old_values_up_to_len :
                    (fp_fill_71       : tsp00_Int4;
                    fp_fill_72        : tsp00_Int2;
                    fp_fill_73        : tsp00_Int2;
                    fp_len            : tsp00_Int4);
                fp_ordered_select_key :
                    (fp_fill_81       : tsp00_Int4;
                    fp_movebefore_v8  : tsp00_Int2;
                    fp_keystack       : tsp00_Int2;
                    fp_to_stack       : tsp00_Int2;
                    fp_key_low_handl  : tsp00_Int2);
                fp_escape_val :
                    (fp_fill_101       : tsp00_Int4;
                    fp_fill_102        : tsp00_Int4;
                    fp_fill_103        : tsp00_Int2;
                    fp_escape_char    : tsp00_C2);
                fp_pseudo_ins_select,
                fp_verify_constraint,
                fp_verify_view :
                    (fp_fill_111       : tsp00_Int4;
                    fp_movebeforev11   : tsp00_Int2;
                    fp_fill_112        : tsp00_Int2;
                    fp_reccolno        : tsp00_Int2;
                    fp_fill_113        : tsp00_Int2);
                fp_result_name :
                    (fp_fromposv14     : tsp00_Int4;
                    fp_fill_141        : tsp00_Int4;
                    fp_datalenv14      : tsp00_Int2;
                    fp_dataiolenv14    : tsp00_Int2);
                fp_verify_not_null :
                    (fp_stackentry     : tgg00_StackEntry;
                    fp_fill_151        : tsp00_Int4);
                (* PTS 1111577 E.Z. *)
                fp_get_name_value :
                    (fp_frompos_v15    : tsp00_Int4;
                    fp_movebefore_v15  : tsp00_Int2;
                    fp_sc_symbol       : tak_sc_symbol;
                    fp_objecttype      : tsp00_C1;
                    fp_len_and_frac_v15: tak_len_and_frac;
                    fp_inoutlen_v15    : tsp00_Int2);
                (* PTS 1116801 E.Z. *)
                fp_fieldlist :
                    (fp_frompos_v16      : tsp00_Int4;
                    fp_interncolno       : tsp00_Int2;
                    fp_rangeextcolno_v16 : tsp00_Int2;
                    fp_len_and_frac_v16  : tak_len_and_frac;
                    fp_inoutlen_v16      : tsp00_Int2);
                END;
            (*ENDCASE*) 
 
 
      tak_ak_all_locks  =  RECORD
            all_lock_count : tsp00_Int2;
            all_lock_fill  : tsp00_Int2;
            all_locks      : ARRAY [ 1..MAX_QUALBUF_LOCKS_GG00 ] OF tgg00_Lock;
      END;
 
      tak_parser_fields  =  ARRAY [ 1..cak_maxparsfields ] OF
            tak_field_pars;
 
      tak_selfref_link_info = RECORD
            sf_index  : tsp00_Int2;
            sf_parsno : tsp00_Int2;
            sf_use_id : tsp00_Int2;
            sf_fill   : tsp00_Int2;    (* unused *)
      END;
 
      tak_parsinfo_states = PACKED SET OF (
            pi_changeinfos_exist,
            pi_hint_exist,
            pu_unused3,
            pi_unused4,
            pi_unused5,
            pi_unused6,
            pi_unused7,
            pi_unused8 );
 
      tak_parsinforecord  =  RECORD    (* 3996 *)
            CASE integer OF
                1 :
                    (p_reclen       : tsp00_Int2;
                    p_keylen        : tsp00_Int2;
                    p_varcol_offset : tsp00_Int2;
                    p_varcol_cnt    : tsp00_Int2;
                    p_surrogate     : tgg00_Surrogate;
                    p_entrytype     : tsp00_C2;
                    p_segno         : tsp00_Int2;
                    p_fullen        : tsp00_Int4;
                    p_filler        : tsp00_Int4;
                    p_buf           : tak_parsbuffer);
                2 :
                    (p1reclen          : tsp00_Int2;
                    p1keylen           : tsp00_Int2;
                    p1varcol_offset    : tsp00_Int2;
                    p1varcol_cnt       : tsp00_Int2;
                    p1surrogate        : tgg00_Surrogate;
                    p1entrytype        : tsp00_C2;
                    p1segno            : tsp00_Int2;
                    p1_fullen          : tsp00_Int4;
                    p1_filler          : tsp00_Int4;
                    p_treeid           : tgg00_FileId;
                    p_pagecnt          : tsp00_Int4;
                    p_resn             : tsp00_KnlIdentifier;
                    p_modul_name       : tsp00_KnlIdentifier;
                    p_searchname       : tsp00_KnlIdentifier;
                    p_mtyp             : tgg00_MessType;
                    p_mtyp2            : tgg00_MessType2;
                    p_select           : boolean;
                    p_single           : boolean;  (* 192 *)
                    p_tabid            : tgg00_Surrogate;
                    p_int_res_cnt      : tsp00_Int2;
                    p_joinviewtabcnt   : tsp00_Int2;
                    p_p_no             : tsp00_Int2;
                    p_subcntlevel      : tsp00_Int2;
                    p_posno            : tsp00_Int4;
                    p_statement_kind   : tgg00_RefInfoIndex;
                    p_bool_states      : tak_parsinfo_states;
                    p_filler1          : boolean;
                    p_precomp_info_byte: tsp00_Uint1;
                    (*====*)
                    p_reuse            : boolean;
                    p_prepare          : boolean;
                    p_function_code    : tsp00_Int2;
                    p_sqlmode          : tsp00_SqlMode;
                    p_own_trans        : boolean;
                    p_handling_set     : tgg00_HandlingSet;
                    p_session_isolevel : tsp00_Uint1;
                    p_use_sess_isolevel: boolean;
                    p_resreclen        : tsp00_Int2;
                    p_resinfolen       : tsp00_Int2;
                    p_j_v_keylen       : tsp00_Int2;
                    p_longliteral      : boolean;
                    p_parsing_again    : boolean; (* pts 1104055 *)
                    p_cmd_id           : tsp00_Int2;
                    p_max_infos        : tsp00_Int2;
                    p_cnt_infos        : tsp00_Int2;  (* mxak_pars_header
                          + cak_sysbufferoffset *)
                    p_pars_infos       : tak_parser_fields);
                4 :
                    (p3reclen       : tsp00_Int2;
                    p3keylen        : tsp00_Int2;
                    p3varcol_offset : tsp00_Int2;
                    p3varcol_cnt    : tsp00_Int2;
                    p3surrogate     : tgg00_Surrogate;
                    p3entrytype     : tsp00_C2;
                    p3segno         : tsp00_Int2;
                    p_tl_filler1    : PACKED ARRAY
                          [ 1..mxak_pars_header ] OF char;
                    p_temp_lock_atch : tsp00_Int4;
                    p_temp_lock      : tak_ak_all_locks);
                5 :
                    (p_sysbufferoffset : ARRAY[1..cak_sysbufferoffset] OF char;
                    p_pars_header      : ARRAY[1..mxak_pars_header] OF char;
                    p_short_info       : tak_paraminfoarr);
                7 :
                    (p7reclen          : tsp00_Int2;
                    p7keylen           : tsp00_Int2;
                    p7varcol_offset    : tsp00_Int2;
                    p7varcol_cnt       : tsp00_Int2;
                    p7surrogate        : tgg00_Surrogate;
                    p7entrytype        : tsp00_C2;
                    p7segno            : tsp00_Int2;
                    p_tl_filler7  : PACKED ARRAY
                          [ 1..mxak_pars_header ] OF char;
                    p_handling         : tgg00_HandlingSet;
                    p_fetch_object     : boolean;
                    p7fill             : boolean);
                END;
            (*ENDCASE*) 
 
      tak_stored_cmd_buf = PACKED ARRAY [ 1..mxak_stored_cmd_buf ] OF char;
 
      tak_stored_cmd_record = RECORD
            scr_reclen         : tsp00_Int2;
            scr_keylen         : tsp00_Int2;
            scr_varcol_offset  : tsp00_Int2;
            scr_varcol_cnt     : tsp00_Int2;
            scr_surrogate      : tgg00_Surrogate;
            scr_entrytype      : tsp00_C2;
            scr_segno          : tsp00_Int2;
            scr_fullen         : tsp00_Int4;
            scr_movelen        : tsp00_Int2;
            scr_ddl            : tak_ddl_descriptor;
            scr_sql_mode       : tsp00_SqlMode;
            scr_buf            : tak_stored_cmd_buf;
      END;
 
      tak_union_kind  =  (uis_none, uis_union, uis_except, uis_intersect);
 
      tak_union_entry  =  RECORD (* 16 *)
            uparsid     : tak_parskey;
            ufirstsel   : tsp00_Int2;
            usecondsel  : tsp00_Int2;
            ufiller     : tsp00_Int2;
            uis_kind    : tak_union_kind;
            uis_all     : boolean;
      END;
 
      tak_union_array  =  ARRAY [ 1..cak_max_union ] OF tak_union_entry;
 
      tak_unionrecord  =  RECORD  (* 4020 *)
            usyskreclen        : tsp00_Int2;
            usyskkeylen        : tsp00_Int2;
            usyskvarcol_offset : tsp00_Int2;
            usyskvarcol_cnt    : tsp00_Int2;
            usysksurrogate     : tgg00_Surrogate;
            usyskentrytype     : tsp00_C2;
            usysksegno         : tsp00_Int2;
            uselect_cnt        : tsp00_Int2;
            ucolpos            : tsp00_Int2;
            ures_tree          : tgg00_FileId;
            ures_name          : tsp00_KnlIdentifier;
            umodul_name        : tsp00_KnlIdentifier;
            urowno             : tsp00_Int4;
            ukeylen            : tsp00_Int2;
            usubcount          : tsp00_Int2;
            ucntpar            : tsp00_Int2;
            uinsertselect      : boolean;
            ufiller1           : boolean;
            uselect_arr        : tak_union_array;
      END;
 
      tak_jv_upd_description  =  (no_jv_upd,
            jv_upd_release_locks,
            jv_upd_end_release);
      tak_complex_info = (
            ci_lastpars,
            ci_unlock,
            ci_recursive);
      tak_complex_infoset = SET OF tak_complex_info;
 
      tak_complex_entry  =  RECORD (* 12 *)
            cparsk       : tak_parskey;
            cnextpars    : tsp00_Int2;
            centryinfo   : tak_complex_infoset;
            cqual_jv_upd : tak_jv_upd_description;
      END;
 
      tak_complex_array  =  ARRAY [ 1..cak_maxcomplex ] OF tak_complex_entry;
 
      tak_complexrecord  =  RECORD
            compreclen        : tsp00_Int2;
            compkeylen        : tsp00_Int2;
            compvarcol_offset : tsp00_Int2;
            compvarcol_cnt    : tsp00_Int2;
            compsurrogate     : tgg00_Surrogate;
            compentrytype     : tsp00_C2;
            compsegno         : tsp00_Int2;
            compcnt           : tsp00_Int2;
            compmax_sel_cnt   : tsp00_Int2;
            compcreateaseltree: tgg00_FileId;
            comparr           : tak_complex_array;
      END;
 
      tak_one_table_specialname =  SET OF
            (oresfile,
            oissynonym,
            oisreference,
            oisjoinview,
            oispartjoinview,
            oisshowview,
            oifiller1);
      tak_messbockFieldSequenceInfo =
            (mfs_literals,
            mfs_parameters,
            mfs_pars_result,
            mfs_ex_result);
 
      tak_messblock_record = RECORD
            mbr_reclen            : tsp00_Int2;
            mbr_keylen            : tsp00_Int2;
            mbr_varcol_offset     : tsp00_Int2;
            mbr_varcol_cnt        : tsp00_Int2;
            mbr_surrogate         : tgg00_Surrogate;
            mbr_entrytype         : tsp00_C2;
            mbr_linkage           : tsp00_C2;
            mbr_segment_id        : tsp00_C2;
            mbr_stack_len         : tsp00_Int4;
            mbr_fieldSequenceInfo : SET OF tak_messbockFieldSequenceInfo;
            mbr_recCount          : char; (* caution : this is a transient information used in the AK cache *)
            mbr_mess_block        : tgg00_MessBlock;
      END;
 
 
      tak_soc_record  =  RECORD
            socoldtabno    : tsp00_Int2;
            socselecttabno : tsp00_Int2;
            socfields      : tsp00_Uint1;
            socfill0       : tsp00_Uint1;
            socfill1       : tsp00_Int2;
      END;
 
      tak_cmd_part_type = (cpt_unknown,
            cpt_in_where_clause,
            cpt_in_having_clause,
            cpt_in_select_list);
 
      tak_one_corcolumn  =  RECORD (* 16 Bytes, mxak_one_corcolumn *)
            cfieldno : tsp00_Int2;
            cnode    : tsp00_Int2;
            clevel   : tak_corlevel;
            csetnode : tsp00_Int2;
            ccntcol  : tsp00_Uint1;
            cfiller2 : tsp00_Uint1;
            ccmd_part: tak_cmd_part_type;
            cfiller1 : tsp00_Uint1;
            ctabno   : tsp00_Int2;
      END;
 
      tak_all_corcolumns  =  ARRAY [ 1..cak_maxcorcolumns ] OF
            tak_one_corcolumn;
 
      tak_correcord  =  RECORD
            tc_reclen        : tsp00_Int2;
            tc_keylen        : tsp00_Int2;
            tc_varcol_offset : tsp00_Int2;
            tc_varcol_cnt    : tsp00_Int2;
            tc_surrogate     : tgg00_Surrogate;
            tc_entrytype     : tsp00_C2;
            tc_segno         : tsp00_Int2;
            tc_no_of_tables  : tsp00_Int2;
            tc_no_of_columns : tsp00_Int2;
            tc_starttable    : tsp00_Int2;
            tc_oldstarttable : tsp00_Int2;
            tc_tables        : ARRAY [ 1..16 ] OF tak_soc_record;
            tc_columns       : tak_all_corcolumns
      END;
 
      (* PTS 1111575 E.Z. *)
 
      tak_linkposlen  = RECORD
            lpos : tsp00_Int2;
            llen : tsp00_Int2;
      END;
 
      tak_linkposinfo  =  ARRAY[ 1..cak_maxlinkpos ] OF tak_linkposlen;
 
      tak_linkinforecord  =  RECORD
            lreclen        : tsp00_Int2;
            lvarcol_offset : tsp00_Int2;
            lvarcol_cnt    : tsp00_Int2;
            lkeylen        : tsp00_Int2;
            lsurrogate     : tgg00_Surrogate;
            lentrytype     : tsp00_C2;
            lseqno         : tsp00_Int2;
            lfullen        : tsp00_Int4;
            lmess_type     : tgg00_MessType;
            lprim_check    : boolean;
            lunique_cnt    : tsp00_Int2;
            lupd_col_info  : tak_columnset;
            lnot_used      : tak_charset;
            lpos_info      : tak_linkposinfo;
      END;
 
      (*========= cache data types =============*)
      tak_directory_state  =  (d_fix, d_release);
      tak_sysdir_state  =  SET OF (dfix, dlocked, dreference,
            dadd_rec, dwritten, dmodified,ddisplace (* PTS 1110796 *),dno_meaning);
      tak_cache_dir_ptr     = ^tak_cache_dir_entry;
      tak_moveobj_ptr       = ^tsp00_MoveObj;
      tak_cache_dir_e_ptr   = ^tak_cache_dir_entry;
 
      tak_cache_dir_pointer = RECORD
            CASE integer OF
                1 :
                    (cdir_ptr    : tak_cache_dir_e_ptr);
                2 :
                    (buf_ptr     : tsp00_BufAddr);
                3 :
                    (moveobj_ptr : tak_moveobj_ptr);
                4 :
                    (sys_ptr     : tak_sysbufferaddress);
                5 :
                    (i           : tsp00_Int4);
                END;
            (*ENDCASE*) 
 
 
      tak_sysdirectory  =  RECORD   (* 1220 *)
            ci_cache_dir_header  : tak_cache_dir_pointer;
            ci_cache_dir_trailor : tak_cache_dir_pointer;
            ci_cache_p           : tsp00_BufAddr;
            ci_tmp_read_cnt      : tsp00_Int4;
            ci_displaceCnt       : tsp00_Int4; (* PTS 1110796 *)
            ci_tmp_dislodged     : boolean;
            ci_inIterator        : boolean;
            ci_rollbackFileExists: boolean;
            ci_filler2           : boolean;
            ci_tmp_read_found    : tsp00_Int4;
            ci_cmd_rollback_tree : tgg00_FileId;
            ci_dir : ARRAY [ 1..cak_emax ] OF tak_cache_dir_pointer;
      END;
 
      (* PTS 1111575 E.Z. *)
 
      tak_syspointerarr  =  RECORD    (* 64 / 120 on ALPHA *)
            px        : ARRAY [ 1..cak_maxpcount ] OF tak_sysbufferaddress;
            presnp    : tak_sysbufferaddress;
            pinfop    : tak_sysbufferaddress;
            pparsp    : tak_sysbufferaddress;
            pbasep    : tak_sysbufferaddress;
            pcolnamep : tak_sysbufferaddress;
            psynid    : tgg00_Surrogate;
            psynfound : boolean;
            psystable : boolean;
            pcount    : tsp00_Int2;
&           ifdef BIT64
            pfiller2  : tsp00_Int4; (* 8 byte alignment *)
&           endif
      END;
 
 
      tak_alterdropcol  =  RECORD
            dextno  : tsp00_Int2;
            dcolptr : tak00_colinfo_ptr;
      END;
 
 
      tak_dropcol  =  RECORD
            dcount : tsp00_Int4;
            dtabno : tsp00_Int2;
            ddel   : boolean;
            dfiller: boolean;
            dcol   : ARRAY [ 1..MAX_COL_PER_TAB_GG00 ] OF tak_alterdropcol;
      END;
 
      tak_index_cmd_mode  =  (cmd_create_index, cmd_drop_index);
      tak_index_col_info  =  ARRAY[1..MAX_COL_SEQUENCE_GG00] OF tak00_colinfo_ptr;
      tak_viewscantype  =  (v_version,
            v_statistics,
            v_intern_save_scheme,
            v_alter_drop,
            v_index,
            v_alter_column,
            v_rename_column,
            v_rename_table,
            v_save_scheme,
            v_revoke_priv,
            v_rename_user);
 
      tak_viewscan_par  =  RECORD
            vsc_baseauthname  : tsp00_KnlIdentifier;
            vsc_basetablen    : tsp00_KnlIdentifier;
            vsc_filevers      : tgg91_FileVersion;
            vsc_rp_rt_handling: boolean;
            vsc_init_type     : tak_viewscantype;
            vsc_base_tabid    : tgg00_Surrogate;
            vsc_key           : tgg00_SysInfoKey;
            vsc_edi           : boolean;
            vsc_restrict      : boolean;
            vsc_ti            : tsp00_Int2;
            vsc_tabno         : tsp00_Int2;
            vsc_type          : tak_viewscantype;
            vsc_delete_view   : boolean;
            vsc_mult_index    : boolean;
            vsc_modified      : boolean;
            vsc_from_tableid  : tgg00_Surrogate;
            vsc_col_list      : boolean;
            vsc_col_dropped   : boolean;
            vsc_usagedef      : tak_usagedef;
            vsc_start_tabid   : tgg00_Surrogate;
            vsc_vdesc_modified: ARRAY[ 1..8 ] OF boolean;
            vsc_rows          : tsp00_Int4;
            vsc_tabno_set     : tak_joinset;
            vsc_vdesc_cnt     : tsp00_Int2;
            (* if header above is changed, don't forget *)
            (* tak_save_viewscan_par !!                 *)
            CASE tak_viewscantype OF
                v_statistics    :
                    (vsc_base_page_cnt : tsp00_Int4;
                    vsc_base_rec_cnt   : tsp00_Int4;
                    vsc_st_filler1     : tsp00_Int4;
                    vsc_st_filler2     : tsp00_Int4;
                    vsc_page_cnt_info  : tak_upd_stat_info);
                v_alter_drop, v_revoke_priv, v_index :
                    (vsc_drop_set       : tak_columnset;
                    vsc_valid_colset    : tak_columnset;
                    vsc_dr_col          : tak_dropcol;
                    vsc_valid_cols      : tak_colindex;
                    vsc_valid_cnt       : tsp00_Int4;
                    vsc_extcol_set      : tak_columnset;
                    vsc_rev_tableid     : tgg00_Surrogate;
                    vsc_start_revoke    : boolean;
                    vsc_userkind        : tak_usertyp;
                    vsc_page_cnt        : tsp00_Int4;
                    vsc_rec_cnt         : tsp00_Int4;
                    vsc_prim_cnt        : tsp00_Int4;
                    vsc_null_cnt        : tsp00_Int4;
                    vsc_single_index    : boolean;
                    vsc_mode            : tak_index_cmd_mode;
                    vsc_index_col_count : tsp00_Int2;
                    vsc_col_info        : tak_index_col_info;
                    vsc_indexname       : tsp00_KnlIdentifier;
                    vsc_unique          : boolean;
                    vsc_ignore_error    : boolean;
                    vsc_keylen          : tsp00_Int2;
                    vsc_col_ptr         : tak00_colinfo_ptr;
                    vsc_op_order        : tgg00_StackOpType;
                    vsc_filler1         : boolean;
                    vsc_filler2         : boolean;
                    vsc_dropped_treeid  : tgg00_FileId);
                v_alter_column :
                    (vsc_drop_col: tak_dropcol;
                    vsc_colinfo  : tak00_columninfo;
                    vsc_coladd   : boolean);
                v_rename_column, v_rename_table :
                    (vsc_rename_set  : tak_columnset;
                    vsc_info_cnt     : tsp00_Int4;
                    vsc_pos_info     : tak_colinteger;
                    vsc_newcolname   : tsp00_KnlIdentifier;
                    vsc_newcol_len   : tsp00_Int2;
                    vsc_oldcolname   : tsp00_KnlIdentifier;
                    vsc_col_extcolno : tsp00_Int2;
                    vsc_colptr       : tak00_colinfo_ptr;
                    vsc_renamed_cols : tsp00_Int4;
                    vsc_renamed      : boolean;
                    vsc_col_in_qual  : boolean;
                    vsc_quotes       : boolean;
                    vsc_rename_filler: boolean;
                    vsc_new_authname : tsp00_KnlIdentifier;
                    vsc_old_len      : tsp00_Int2;
                    vsc_new_tablen   : tsp00_KnlIdentifier;
                    vsc_new_len      : tsp00_Int2;
                    vsc_old_tablen   : tsp00_KnlIdentifier;
                    vsc_ren_tabid    : tgg00_Surrogate);
                v_rename_user :
                    (vsc_old_user_name : tsp00_KnlIdentifier;
                    vsc_new_user_name : tsp00_KnlIdentifier);
                v_save_scheme, v_intern_save_scheme :
                    (vsc_tree_id      : tgg00_FileId;
                    vsc_buf           : tak_sysbufferaddress;
                    vsc_into_authname : tsp00_KnlIdentifier;
                    vsc_into_tablen   : tsp00_KnlIdentifier;
                    vsc_save_into     : boolean;
                    vsc_tablekind     : tgg00_TableKind;
                    vsc_into_treeid   : tgg00_FileId;
                    vsc_cmd_cnt       : tsp00_Int4;
                    vsc_first_save    : boolean;
                    vsc_last_save     : boolean;
                    vsc_user_save     : boolean;
                    vsc_all_save      : boolean;
                    vsc_schema_save   : boolean);
                END;
            (*ENDCASE*) 
 
 
      tak_save_viewscan_par  =  RECORD
            vsc_baseauthname  : tsp00_KnlIdentifier;
            vsc_basetablen    : tsp00_KnlIdentifier;
            vsc_filevers      : tgg91_FileVersion;
            vsc_rp_rt_handling: boolean;
            vsc_init_type     : tak_viewscantype;
            vsc_base_tabid    : tgg00_Surrogate;
            vsc_key           : tgg00_SysInfoKey;
            vsc_edi           : boolean;
            vsc_free          : boolean;     (* free *)
            vsc_ti            : tsp00_Int2;
            vsc_tabno         : tsp00_Int2;
            vsc_type          : tak_viewscantype;
            vsc_delete_view   : boolean;
            vsc_mult_index    : boolean;
            vsc_modified      : boolean;
            vsc_from_tableid  : tgg00_Surrogate;
            vsc_col_list      : boolean;
            vsc_col_dropped   : boolean;
            vsc_usagedef      : tak_usagedef;
            vsc_start_tabid   : tgg00_Surrogate;
            vsc_vdesc_modified: ARRAY[ 1..8 ] OF boolean;
            vsc_rows          : tsp00_Int4;
            vsc_tabno_set     : tak_joinset;
            vsc_vdesc_cnt     : tsp00_Int2;
            (* the part above must agree with the prefix of *)
            (* tak_viewscan_parameter !!                    *)
            vsc_tree_id       : tgg00_FileId;
            vsc_buf           : tak_sysbufferaddress;
            vsc_into_authname : tsp00_KnlIdentifier;
            vsc_into_tablen   : tsp00_KnlIdentifier;
            vsc_save_into     : boolean;
            vsc_tablekind     : tgg00_TableKind;
            vsc_into_treeid   : tgg00_FileId;
            vsc_cmd_cnt       : tsp00_Int4;
            vsc_first_save    : boolean;
            vsc_last_save     : boolean;
            vsc_user_save     : boolean;
            vsc_all_save      : boolean;
            vsc_schema_save   : boolean;
      END;
 
      tak_function  =  (tf_unknown, tf_no_func, tf_func, tf_func_arith);
      tak_internal_sql_kind  =  (no_internal_sql,
            sql_restore_schema,
            sql_alter_table,
            sql_internal_proc_call,
            sql_create_table_as_select,
            sql_parse_cursor);    (* PTS 1111576 E.Z. *)
      tak_commandkind  = (
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
            corr_in_union_command);
      tak_site_states  =  ARRAY[ 0..7, 0..255 ] OF
            tgg00_TransState;
      tak_site_state_ptr   = ^tak_site_states;
      tak_isolation_info  =  (temp_lock_rec_not_needed,
            temp_lock_rec_needed,
            temp_lock_rec_get,
            temp_lock_rec_finished);
 
      tak_transinfo  =  RECORD
            tri_trans        : tgg00_TransContext;
            tri_global_state : tgg00_HandlingSet;
            tri_fill1        : tsp00_Uint2;
            tri_obj_seq      : tsp00_C6;
            tri_fill2        : tsp00_Int2;
      END;
 
      tak_rollback_info = PACKED SET OF (trans_rollback, new_trans);
      tak_query_type = (type_n, type_j, type_an,
            type_aj, type_else, type_no);
 
      tak_updstat_rec = RECORD
            us_tabid      : tgg00_Surrogate;
            us_date       : tsp00_Int4;
            us_time       : tsp00_Int4;
            us_known_pages: tsp00_Int4;
            us_found_pages: tsp00_Int4;
      END;
 
 
      tak_index_scan_record = RECORD
            isr_buf        : tak_sysbufferaddress;
            isr_index      : integer;
            isr_sysk       : tgg00_SysInfoKey;
      END;
 
 
      tak_optimize_info = RECORD
            o_do_optimize : boolean;
            o_query_type  : tak_query_type;
            o_mess_type   : tsp00_Int2;
            o_ex_kind     : tak_execution_kind;
            o_optimized   : boolean;
            o_filler1     : tsp00_Int2;
            o_cmd_tree    : tgg00_FileId;
            o_filler3     : tsp00_C8;
      END;
 
 
      tak_nls_param_rec = RECORD
            currency       : tsp00_C4;
            date_format    : tsp00_C50;
            date_language  : tsp00_C3;
            numeric_chars  : tsp00_C2;
            df_length      : tsp00_Int2;
            dl_found       : boolean;
            dl_is_german   : boolean;
            dl_language_no : tsp00_Uint1;
            nls_binary     : boolean;
            nls_sort       : tsp00_KnlIdentifier;
      END;
 
      tak_appl_type = (
            at_unknown,
            at_cpc,
            at_cpp, (* PTS 1112481 at_cpp added FF *)
            at_db_manager,
            at_gpc, (* Python, ... *)
            at_jdbc,
            at_load,
            at_odbc,
            at_util, (* REPAIR *)
            at_xci,
            at_kernel,
            at_debugger);
      tak_explain_kind = (ex_default, ex_join, ex_sequence);
 
      tak_cmd_stack = RECORD
            cst_top : tsp00_Int4;
            cst_stack : ARRAY[1..cak_max_cmd_stack] OF tsp00_MoveObjPtr;
      END;
 
      tak_recursive_state = (
            rs_no_recursive_select,
            rs_check_select_list,
            rs_first_select,
            rs_recursive_select,
            rs_last_select);
 
      tak_kernel_sink = RECORD
            CASE boolean OF
                true :
                    (d : tsp00_Longreal);
                false :
                    (c : tsp00_C132); (* 1124533 MS  1106969 FF // 2002-09-10 tsp00_C80 sqlHeap ptr *)
                END;
            (*ENDCASE*) 
 
 
      tak_AllocatorMonitorInfo = RECORD
            ami_MallocMin : tsp00_8ByteCounter;
            ami_MallocMax : tsp00_8ByteCounter;
            ami_MallocSum : tsp00_8ByteCounter;
            ami_MallocCnt : tsp00_8ByteCounter;
            ami_FreeMin   : tsp00_8ByteCounter;
            ami_FreeMax   : tsp00_8ByteCounter;
            ami_FreeSum   : tsp00_8ByteCounter;
            ami_FreeCnt   : tsp00_8ByteCounter;
            ami_Allocated : tsp00_8ByteCounter;
      END;
 
 
      tak_allocator_info = RECORD
            ai_uktId             : tsp00_Int4;
            ai_badAllocCountDown : tsp00_Int4; (* meaning only in omststknl, PTS 1112358 *)
            ai_badAllocCount     : tsp00_Int4; (* PTS 1126797 *)
            ai_HeapUsageCurrent  : tsp00_8ByteCounter;
            ai_HeapUsageMax      : tsp00_8ByteCounter;
            ai_alloc_sys         : tak_AllocatorMonitorInfo;
            ai_alloc_usr         : tak_AllocatorMonitorInfo;
      END;
 
 
      tak_dbproc_call_info = RECORD
            dbc_conversionError : boolean;
            dbc_errorTableId    : tsp00_Int4;
      END;
 
 
      tak_used_corr_infos = RECORD
            uci_oldlowpars       : tsp00_Uint1;
            uci_lowpars          : tsp00_Uint1;
            uci_pid              : tsp00_C1;
      END;
 
      tak_acv_address  = ^tak_all_command_glob;
      tak_size_ptr = ^tak_size_info;
      (**)
      tak_command_states = PACKED SET OF (
            csa_subq_datatype_problem (* PTS 1001162 E.Z. *),
            csa_hint_exist,
            csa_short_sum_values,
            csa_unused4,
            csa_unused5,
            csa_unused6,
            csa_unused7,
            csa_unused8 );
 
      tak_all_command_glob  =  RECORD   (* acv  : variable, ca 23000*)
            a_in_packet             : tsp1_packet_ptr;
            a_out_packet            : tsp1_packet_ptr;
            a_cmd_segm              : tsp1_segment_ptr;
            a_cmd_part              : tsp1_part_ptr;
            a_data_part             : tsp1_part_ptr;
            a_abap_part             : tsp1_part_ptr;
            a_rescount_part         : tsp1_part_ptr;
            a_resname_part          : tsp1_part_ptr;
            a_modulename_part       : tsp1_part_ptr;
            a_appl_param_part       : tsp1_part_ptr;
            a_feature_part          : tsp1_part_ptr;
            a_clientid_part         : tsp1_part_ptr;
            a_data_ptr              : tsp00_MoveObjPtr;
            a_return_segm           : tsp1_segment_ptr;
            a_ap_tree               : tak_ap_max_tree_ptr;
            a_curr_retpart          : tsp1_part_ptr;
            a_input_data_pos        : tsp00_Int4;
            a_output_data_pos       : tsp00_Int4;
            a_cmd_globstate         : tgg00_HandlingSet;
            a_info_output           : boolean;
            a_ex_kind               : tak_execution_kind;
            a_precomp_info_byte     : tsp00_Uint1;
            a_scv_index             : tsp00_Int2;
            a_scv                   : tak_scanner_glob;
            a_in_internal_subtr     : boolean;
            a_init_ddl              : tak_ddl_descriptor;
            a_count_variab          : tsp00_Int2;
            a_count_literals        : tsp00_Int2;
            a_count_output_params   : tsp00_Int2;
            a_first_output_param    : tsp00_Int2;
            a_command_kind          : tak_commandkind;
            a_is_ddl                : tak_ddl_descriptor;
            a_outer_join            : boolean;
            a_intern_explain        : boolean;
            a_pars_explain          : boolean;
            a_internal_sql          : tak_internal_sql_kind;
            a_from_select           : boolean;
            a_allow_functions       : tak_function;
            a_rowno_allowed         : boolean;
            a_rowno_found           : boolean;
            a_special_expr          : boolean;
            a_oneval_subq_allowed   : boolean;
            a_column_factor         : boolean;
            a_join_predicate        : boolean;
            a_try_again             : boolean;
            a_select_node           : tsp00_Int2;
            a_complex_key           : tgg00_Surrogate;
            a_max_intern_select     : tsp00_Int2;
            a_intern_select_cnt     : tsp00_Int2;
            a_shortinfo_key         : tgg00_Surrogate;
            a_union_cnt             : tsp00_Int4;
            a_union_key             : tak_parskey;
            a_union_kind            : tsp00_Uint1;
            a_costcheck             : boolean;
            a_costsum               : tsp00_Int4;
            a_isolation_info        : tak_isolation_info;
            a_corr_select           : boolean;
            a_error_tableid         : tgg00_Surrogate;
            a_errpos_offset         : tsp00_Int4; (* joinview *)
            a_parameter_offset      : tsp00_Int2;
            a_qualified_jv_upd      : tak_jv_upd_description;
            a_rest_equal_second     : boolean; (* execute correlation*)
            a_part_rollback         : boolean;
            a_rollback_info         : tak_rollback_info;
            a_timestamp             : tsp00_Timestamp;
            a_show_last_key         : tsp00_Int4;
            a_first_parskey         : char;
            a_first_parsid          : char;
            a_leftpar_cnt           : tsp00_Int2;
            a_rightpar_cnt          : tsp00_Int2;
            a_specialpar_cnt        : tsp00_Int2;
            a_tempinfo_key          : tgg00_Surrogate;
            a_viewSchemaId          : tgg00_Surrogate;
            a_viewname              : tsp00_KnlIdentifier;
            a_fromsel_n             : tsp00_Int2;
            a_may_be_direct_sel     : boolean;
            a_date_time_used        : boolean;
            a_compact_varpart       : boolean;
            a_max_codewidth         : tsp00_Uint1;
            a_result_name           : tsp00_KnlIdentifier;
            a_mess_code             : tsp00_CodeType;
            a_corr_key              : tak_parskey;
            a_curr_ex_parskey       : tak_parskey;
            a_init_ex_kind          : tak_execution_kind;
            a_in_ddl_trigger        : boolean;
            a_iso_level             : tsp00_Uint1;
            a_for_fetch_only        : boolean;
            a_in_select_list        : boolean;
            a_in_installation       : boolean;
            a_parsing_again         : boolean;
            a_statement_kind        : tgg00_RefInfoIndex;
            a_modul_name            : tsp00_KnlIdentifier;
            a_parameter_spec        : tak_param_list;
            a_ap_max_node           : tsp00_Int2;
            a_dynamic_sql           : boolean;
            a_intern_warnings       : tsp1_warningset_intern;
            a_explain_kind          : tak_explain_kind;
            a_cpart_type            : tak_cmd_part_type;
            a_retpart_locked        : boolean;
            a_parsingForSharedSql   : boolean;
            a_createSharedSQL       : boolean;
            a_no_of_likes           : tsp00_Int2;
            a_maxnode               : tsp00_Int4;
            a_data_length           : tsp00_Int4;
            a_resultnum             : tsp00_ResNum;
            a_mblock                : tgg00_MessBlock;
            a_sample_pages          : tsp00_Int4;
            a_optimize_info         : tak_optimize_info; (* init *)
            a_progusage_add         : boolean;
            a_proc_compile          : tak_proc_compile_type;
            a_proc_id               : tgg00_Surrogate;
            a_show_data_cnt         : tsp00_Int4;
            a_dca                   : tsp00_KernelDca; (* ??? *)
            a_union_res_id          : tsp00_Int4;
            a_sql_cmd_state         : tak_command_states;
            a_sub_found             : boolean;
            a_expl_resname          : tsp00_KnlIdentifier;
            a_next_upos             : tsp00_Int2;
            a_insert_select         : boolean;
            a_named_subtrans        : boolean;
            a_link_pending          : boolean;
            a_variable_input        : boolean;
            a_long_desc_pos         : tsp00_Int2;
            a_execute_trigger       : boolean;
            a_trigger_level         : tsp00_Uint1;
            a_sqlmode               : tsp00_SqlMode;
            a_outer_n               : tsp00_Int2;
            a_dbproc_level          : tsp00_Uint1;
            a_col_func_count        : tsp00_Int2;
            a_cmd_stack             : tak_cmd_stack;
            a_unionrec_ptr          : tak_sysbufferaddress;
            a_opt_info_ptr          : tak_sysbufferaddress;
            a_err_parm_cnt          : tsp00_Int2;
            a_error_params          : tsp00_C80;
            a_uni_cmd_part          : tsp1_part_ptr;
            a_recursive_no          : tsp00_Int2;
            a_recursive_state       : tak_recursive_state;
            a_outstanding_packet    : boolean;
            a_mass_all_rescount     : tsp00_Int4;   (* h.b. CR 1000072 *)
            a_init_info_output      : boolean;
            a_first_hint_node       : tsp00_Int2;
            a_isInCppDBProcedure    : boolean;        (* PTS 1110148 T.A. *)
            a_where_corr_info       : tak_used_corr_infos;
            a_stored_proc_call      : boolean;
            a_user_defined_error    : boolean;
            a_col_file_count        : tsp00_IntMapC2;
            a_ak_b_err              : tgg00_BasisError;
            a_resname_addr          :
                  ARRAY [ cak_extern_pos..cak_aux_pos ] OF
                  tak_sysbufferaddress;
            a_curr_res_id           : tsp00_Int4;
            a_p_arr1                : tak_syspointerarr;
            a_p_arr2                : tak_syspointerarr;
            a_p_arr3                : tak_syspointerarr;
            a_p_arr5                : tak_syspointerarr;
            a_ptr1                  : tak_sysbufferaddress;
            a_ptr2                  : tak_sysbufferaddress;
            a_ptr3                  : tak_sysbufferaddress;
            a_ptr6                  : tak_sysbufferaddress;
            a_ptr8                  : tak_sysbufferaddress;
            a_ptr10                 : tak_sysbufferaddress;
            a_saved_packet_ptr      : tsp1_packet_ptr;
            a_saved_reply_length    : tsp00_Int4;
            a_planHandle            : tgg00_VoidPtr;
            a_prepareHandle         : tgg00_VoidPtr;
            a_longdemand_data       : tgg00_VoidPtr;
            a_invalidateList        : tgg00_VoidPtr;
            (* PTS 1119397 E.Z. *)
            a_update_table_set      : boolean;
            a_update_tableowner     : tsp00_KnlIdentifier;
            a_update_tablename      : tsp00_KnlIdentifier;
            a_initial_packet_header : tsp1_packet_header;
            a_initial_segment_header: tsp1_segment_header;
            a_cmd_packet_header     : tsp1_packet_header;
            a_cmd_segment_header    : tsp1_segment_header;
            a_main_returncode       : tsp00_Int2;
            a_returncode            : tsp00_Int2;
            a_main_errorpos         : tsp00_Int4;
            a_errorpos              : tsp00_Int4;
            a_command_id            : tsp00_Int4;
            a_cmd_start_id          : tsp00_Int4;
            a_resultCurrLongFileId  : tsp00_Int4;
            a_resultCurrLongFileKey : tsp00_Int4;
            a_resultCurrLongFile    : tgg00_FileId;
            a_first_serial          : tsp00_Number;
            a_last_serial           : tsp00_Number;
            a_complex_tree          : tgg00_FileId;
            a_statement_parsid      : tak_parsid;
            a_pars_last_key         : tak_parskey;
            a_replication_check_id  : tsp00_C8;
            a_queryrewrite_mode     : tak_queryrewrite_mode;
            a_queryrewrite_done     : tak_queryrewrite_mode;
            a_queryrewrite_exec     : tak_queryrewrite_exec;
            a_queryrewrite_modified : boolean;
            a_original_stmt_lenght  : tsp00_Int4; (* statementsize before queryrewrite *)
            a_provoked_exception    : boolean;
            a_systemTriggerFailed   : boolean;
            a_limit_allowed         : boolean;
            a_limit_found           : boolean;
            a_limit_node            : tsp00_Int2;
            a_end_of_statement_part : boolean;
            (*********************************** end of statement context ********************************)
            (* start of session context *)
            a_session_ap_tree       : tak_ap_max_tree_ptr;
            a_input_data_map        : tsp00_MoveObjPtr;
            a_client_out_packet     : tsp1_packet_ptr;
            a_into_tree             : tgg00_FileId;
            a_intern_tree           : tgg00_FileId;
            a_intern_res_tree       : tgg00_FileId;
            a_intern_cmd_tree       : tgg00_FileId;
            a_trigger_tree          : tgg00_FileId;
            a_mb_qual_addr          : tgg00_QualBufPtr;
            a_mb_qual_size          : tsp00_Int4;
            a_stack_size            : tsp00_Int4;
            a_work_st_addr          : tgg00_StackListPtr;
            a_work_buf_addr         : tsp00_MoveObjPtr;
            a_work_buf_size         : tsp00_Int4;
            a_work_st_max           : tsp00_Int2;
            a_defaultcode           : tsp00_CodeType;
            a_session_max_codewidth : tsp00_Uint1;
            a_session_mess_code     : tsp00_CodeType;
            a_comp_type             : tak_appl_type;
            a_comp_vers             : tsp00_C5;
            a_degree                : tsp00_C3;
            a_is_connected          : boolean;
            a_in_session            : boolean;
            a_db_full_connection    : boolean;
            a_curr_schema_set       : boolean;
            a_curr_dbprocedure      : tgg00_Surrogate;
            a_init_user_id          : tgg00_Surrogate;
            a_connected_name        : tsp00_KnlIdentifier;
            a_curr_user_name        : tsp00_KnlIdentifier;
            a_curr_schema           : tsp00_KnlIdentifier;
            a_curr_user_id          : tgg00_Surrogate;
            a_curr_schema_id        : tgg00_Surrogate;
            a_current_user_kind     : tak_usertyp;
            a_is_exclusive          : boolean;
            a_user_perm_ref         : tgg00_UserRef;
            a_acc_user_id           : tgg00_Surrogate;
            a_acc_user              : tsp00_KnlIdentifier;
            a_acc_groupname         : tsp00_KnlIdentifier;
            a_acc_dba_id            : tgg00_Surrogate;
            a_acc_dbaname           : tsp00_KnlIdentifier;
            a_command_sequence      : tsp00_Int4;
            a_costwarn_value        : tsp00_Int4;
            a_costlimit_value       : tsp00_Int4;
            a_user_timeout          : tsp00_Int2;
            a_use_timeout           : boolean;
            a_check_lock            : boolean;
            a_ak_language           : tsp00_C3;
            a_errormsg_loading      : boolean;
            a_proc_userid           : tgg00_Surrogate;
            a_pseudoUnicodeClient   : boolean;
            a_dt_format             : tgg00_DateTimeFormat;
            a_max_res_id            : tsp00_Int4;
            a_show_curr             : tgg00_FileId;
            a_pars_curr             : tgg00_FileId;
            a_usage_curr            : tgg00_FileId;
            a_sysdir                : tak_sysdirectory;
            a_fk_check              : boolean;
            a_transinf              : tak_transinfo; (* 92 Bytes *)
            a_result_cnt            : tsp00_Int2;
            a_temp_tablekey         : tgg00_Surrogate;
            a_constraint_cnt        : tsp00_Int2;
            a_fk_check_cnt          : tsp00_Int2;
            a_restore_with_string   : tsp00_Int2;
            a_session_sqlmode       : tsp00_SqlMode;
            a_acc_termid            : tsp00_TermId;
            a_row_not_found_text    : tak_order_errortext;
            a_len_row_not_found_text: tsp00_Int2;
            a_stack_addr            : tgg00_StackListPtr;
            a_error_key_ptr         : tak_sysbufferaddress;
            a_nls_params            : tak_nls_param_rec;
            a_role_ptr              : tak_role_context_ptr;
            a_role_info             : tak_role_info;
            a_no_of_com_objects     : tsp00_Int4;
            a_kernel_sink           : tak_kernel_sink;
            a_release_instances     : boolean;
            a_abap_tab_comm_ok      : boolean; (* PTS 1107485 E.Z. *)
            a_free_pcounts          : tak_pcountarr;  (* PTS 1109291 E.Z. *)
            a_first_free_pcount     : tsp00_Int2;       (* PTS 1114071 E.Z. *)
            a_last_free_pcount      : tsp00_Int2;       (* PTS 1114071 E.Z. *)
            a_max_used_pcount       : tsp00_C3;         (* PTS 1109291 E.Z. *)
            a_allocatorInfo         : tak_allocator_info; (* PTS 1110148 T.A. *)
            a_dbproc_call_info      : tak_dbproc_call_info; (* PTS 1116664 T.A. *)
            a_dbp_packet_ptr        : tsp1_packet_ptr;
            a_mb_data_addr          : tgg00_DataPartPtr;
            a_mb_strat_addr         : tsp00_MoveObjPtr;
            a_size_ptr              : tak_size_ptr;
            a_cpp_session_members   : tgg00_VoidPtr;
            a_mb_data_size          : tsp00_Int4;
            a_mb_strat_size         : tsp00_Int4;
            a_reply_packet_size     : tsp00_Int4;
            a_space_option          : boolean;          (* PTS 1121403 E.Z. *)
            a_isReplicationUser     : boolean;
            a_isReplicationSession  : boolean;
            a_withInternalTriggers  : boolean;
            a_packetVtrace          : boolean;
            a_optimizedStreamSupport: boolean;
            a_check_scrollableoption: boolean;
            a_localCacheUsageOnly   : boolean;
            a_modifyBootStrapCatalog: boolean;
            a_ascii_in_and_output   : boolean;
            a_challenge             : tsp00_C8;
            a_cmd_id                : tsp00_Int2;
            a_authentication        : tsp00_MoveObjPtr;
            a_in_inner_trans        : boolean;
            a_outer_trans           : tgg00_UnivTrans;
            (* end of session context *)
      END;
 
 
      tak_size_info = RECORD
            csz_next       : tak_size_ptr;
            csz_acv        : tak_acv_address;
            csz_session    : tgg91_SessionNo;
            csz_ak_cache   : tsp00_Int4;
            (* PTS 1115978 E.Z. *)
            csz_fill1      : boolean;
            csz_tracelevel : tsp00_Uint1;
            csz_fill2      : tsp00_Int2;
            csz_alpha_fill : tsp00_Int4;
      END;
 
 
      tak_a11_like_record  =  RECORD
            a1l_p_arr      : tak_syspointerarr;
            a1l_privset    : tak_privilege;
            a1l_likecolset : tak_columnset;
            a1l_rec_to_ext : tak_colindex;
      END;
 
 
      tak_constraint_info = RECORD
            tree_index : tsp00_Int2;
            extcolno   : tsp00_Int2;
      END;
 
 
      tak_a11_glob  =  RECORD  (* a11v *)
            a1authid            : tgg00_Surrogate;
            a1tableid           : tgg00_Surrogate;
            a1likeid            : tgg00_Surrogate;
            a1authname          : tsp00_KnlIdentifier;
            a1likename          : tsp00_KnlIdentifier;
            a1tablen            : tsp00_KnlIdentifier;
            a1coln              : tsp00_KnlIdentifier;
            a1liketablen        : tsp00_KnlIdentifier;
            a1sysk              : tgg00_SysInfoKey;
            a1valkey            : tgg00_SysInfoKey;
            a1p_arr             : tak_syspointerarr;
            a1constraint_info   : ARRAY[ 0..cak_max_constraints ]
                  OF tak_constraint_info;
            a1colsysp           : tak_sysbufferaddress;
            a1sysbuf            : tak_sysbufferaddress;
            a1valsysp           : tak_sysbufferaddress;
            a1domainbuf         : tak_sysbufferaddress;
            a1colptr            : tak00_colinfo_ptr;
            a1ti                : tsp00_Int4;
            a1ci                : tsp00_Int4;
            a1reclen            : tsp00_Int4;
            a1longcharcnt       : tsp00_Int4;
            a1temp_id           : tsp00_Int4;
            a1foreign_key_node  : tsp00_Int2;
            a1extcolno          : tsp00_Int2;
            a1keypos            : tsp00_Int2;
            a1strcolcount       : tsp00_Int2;
            a1part2_pos         : tsp00_Int2;
            a1key_node          : tsp00_Int2;
            a1unique_node       : tsp00_Int2;
            a1default_datatype  : tsp00_DataType;
            a1inpkey            : boolean;
            a1valbufexist       : boolean;
            a1createtab         : boolean;
            a1systable          : boolean;
            a1liketable         : boolean;
            a1sort              : boolean;
            a1dynamic           : boolean;
            a1default_stamp     : boolean;
            a1global            : boolean;
            a1temp_table        : boolean;
            a1like_temp_table   : boolean;
            a1nolog             : boolean;
            a1add_rec           : boolean;
            a1for_archive       : boolean;   (* PTS 1117381 E.Z. *)
            a1ispacked          : boolean;
            a1clustered         : boolean;
            a1constraint_cnt    : tsp00_Int2;
            a1init_colcount     : tsp00_Int2;
            a1constraint_index  : tsp00_Int2;
            a1serial_node       : tsp00_Int2;
            a1constraint_name   : tsp00_KnlIdentifier;
            a1long_col_cnt      : tsp00_Int2;
      END;
 
 
      tak_a21_glob   = RECORD
            a2user_name        : tsp00_KnlIdentifier;
            a2like_name        : tsp00_KnlIdentifier;
            a2groupname        : tsp00_KnlIdentifier;
            a2password         : tsp00_Name;
            a2authorizationId  : tgg00_Surrogate;
            a2user_id          : tgg00_Surrogate;
            a2like_id          : tgg00_Surrogate;
            a2group_id         : tgg00_Surrogate;
            a2sysk             : tgg00_SysInfoKey;
            a2params           : tak_userpar_arr;
            a2sysbuf           : tak_sysbufferaddress;
            a2errorpos         : tsp00_Int4;
            a2ti               : tsp00_Int2;
            a2userkind         : tsp00_Int2;
            a2like             : boolean;
            a2modified         : boolean;
            a2connect          : boolean;
            a2connect_modified : boolean;
            a2exclusive        : boolean;
            a2ex_modified      : boolean;
            a2usergroup        : boolean;
            a2cascade          : boolean;
            a2role             : boolean;
            a2defc_modified    : boolean;
            a2replication      : boolean;
            a2schema           : boolean;
            a2OmsSchema        : boolean;
            a2GroupUserAllowed : boolean;
            a2defaultcode      : tsp00_CodeType;
      END;
 
 
      tak_a25_glob  =  RECORD
            a2ti          : tsp00_Int4;
            a2key_col_ptr : tak00_colinfo_ptr;
            a2linkname    : tsp00_KnlIdentifier;
            a2keycount    : tsp00_Int2;
            a2primlink    : tak_sysbufferaddress;
            a2seclink     : tak_sysbufferaddress;
            a2resfile     : tgg00_FileId;
            a2columnn     : tsp00_KnlIdentifier;
            a2refcol_ti   : tsp00_Int2;
            a2default     : tgg00_Lkey;
            a2syskey      : ARRAY[ 1..2 ] OF tgg00_SysInfoKey;
            a2base_p      : ARRAY[ 1..2 ] OF tak_sysbufferaddress;
            a2version     : ARRAY[ 1..2 ] OF tsp00_C2;
            a2authname    : ARRAY[ 1..2 ] OF tsp00_KnlIdentifier;
            a2tablen      : ARRAY[ 1..2 ] OF tsp00_KnlIdentifier;
            a2fieldpos    : ARRAY[ 1..MAX_STRATEGY_KEYS_GG04 ] OF tsp00_Int2;
            a2create_tab  : boolean;
            a2fillchar    : tsp00_C1;
            a2delact      : tsp00_Int2;
      END;
 
 
      tak_a30_utility_glob  =  RECORD
            a3authname     : tsp00_KnlIdentifier;
            a3tablen       : tsp00_KnlIdentifier;
            a3coln         : tsp00_KnlIdentifier;
            a3ti           : tsp00_Int2;
            a3passw        : tsp00_Name;
            a3p_arr        : tak_syspointerarr;
            a3treeid       : tgg00_FileId;
            a3hostfn       : tsp00_VFilename;
      END;
 
 
      tak_long_descriptor = RECORD
            CASE integer OF
                1 :
                    (desc : tsp00_LongDescriptor);
                2 :
                    (lds_descriptor  : tgg00_Surrogate;
                    lds_tabid        : tgg00_Surrogate;
                    lds_maxlen       : tsp00_Int4;
                    lds_intern_pos   : tsp00_Int4;
                    lds_infoset      : tsp00_LdInfo;
                    lds_state        : tsp00_LdState;
                    lds_show_kind    : tgg00_ShowKind;
                    lds_valmode      : tsp00_ValMode;
                    lds_valind       : tsp00_Int2;
                    lds_colno        : tsp00_Int2;
                    lds_valpos       : tsp00_Int4;
                    lds_vallen       : tsp00_Int4);
                3 :
                    (ldc_descriptor  : tgg00_Surrogate;
                    ldc_tabid        : tgg00_Surrogate;
                    ldc_maxlen       : tsp00_Int4;
                    ldc_intern_pos   : tsp00_Int4;
                    ldc_infoset      : tsp00_LdInfo;
                    ldc_state        : tsp00_LdState;
                    ldc_comment_type : tak_comment_type);
                4 :
                    (lds_descriptor1  : tgg00_Surrogate;
                    lds_tabid1        : tgg00_Surrogate;
                    lds_copyTabid     : tgg00_Surrogate;
                    lds_infoset1      : tsp00_LdInfo;
                    lds_state1        : tsp00_LdState;
                    lds_show_kind1    : tgg00_ShowKind;
                    lds_valmode1      : tsp00_ValMode;
                    lds_valind1       : tsp00_Int2;
                    lds_colno1        : tsp00_Int2;
                    lds_copyLongId    : tgg00_Surrogate);
                END;
            (*ENDCASE*) 
 
 
      tak_long_desc_block = RECORD
            CASE integer OF
                1 :
                    (ldb_desc : PACKED ARRAY [ 1..LONG_DESC_BLOCK_MXSP00 ] OF char);
                2 :
                    (ldb_descriptor : tgg00_Surrogate;
                    ldb_filler2     : tsp00_Int2;
                    ldb_change      : tsp00_LdbChange;
                    ldb_filler3     : boolean;
                    ldb_curr_pageno : tsp00_Int4;
                    ldb_curr_pos    : tsp00_Int2;
                    ldb_filler0     : tsp00_C1;
                    ldb_show_kind   : tgg00_ShowKind;
                    (* above: 20 bytes not seen by application *)
                    ldb_intern_pos  : tsp00_Int4;
                    (* above: 4 bytes may be seen by application *)
                    ldb_infoset     : tsp00_LdInfo;
                    ldb_filler1     : boolean;
                    (* above: 2 bytes not seen by application *)
                    ldb_valind      : tsp00_C1;
                    (* above: 1 byte only seen by the application *)
                    ldb_valmode     : tsp00_ValMode;
                    ldb_filler5     : tsp00_Int2;
                    ldb_filler6     : tsp00_Int2;
                    ldb_valpos      : tsp00_Int4;
                    ldb_vallen      : tsp00_Int4;
                    (* above: 13 bytes seen by the application and knl *)
                    ldb_full_len    : tsp00_Int4;
                    ldb_filler4     : tsp00_Int4;
                    (* below: 8 bytes not seen by the application *)
                    ldb_last_i_pos  : tsp00_Int4;
                    ldb_root_check  : tsp00_C4);
                3 :
                    (ldb1_descriptor  : tgg00_Surrogate;
                    ldb1_filler2      : tsp00_Int2;
                    ldb1_change       : SET OF (ldb1_unused_was_use_termchar,
                          ldb1_use_conversion);
                    ldb1_filler3      : boolean;
                    ldb_colno         : tsp00_Int4;
                    ldb1_curr_pos     : tsp00_Int2;
                    ldb1_colno        : tsp00_C1;
                    ldb1_comment_type : tak_comment_type);
                END;
            (*ENDCASE*) 
            (**************************************************************)
            (*                       DML_PART                             *)
            (**************************************************************)
 
      tak_lockenum  = (no_lock,
            read_lock,
            write_lock,
            no_lock_string,
            optimistic_lock);
 
      tak_corfields  =  RECORD
            cfields      : tak_all_corcolumns;
            cused_fields : tsp00_Int2;
            ccfill       : tsp00_Int2
      END;
 
      tak_needed_spec = (
            ons_with_username,
            ons_only_tablename);
 
      tak_one_table  =  RECORD        (* 588 *)
            ouser         : tsp00_KnlIdentifier;
            otable        : tsp00_KnlIdentifier;
            oreference    : tsp00_KnlIdentifier;
            ofromtableid  : tgg00_Surrogate;
            otreeid       : tgg00_FileId;
            opages        : tsp00_Int4;
            oall_priv     : boolean;
            ospecialname  : tak_one_table_specialname;
            oview         : boolean;
            oviewqual     : boolean;
            oviewcheck    : boolean;
            ocomplex_view : boolean;
            otab_node     : tsp00_Int2;
            ospecs_needed : tak_needed_spec;
            ounused2      : tsp00_Uint1;
            oattributes   : tak_table_attribute_set;
            ounused0      : tsp00_Uint1;       (* free *)
            ocounttabs    : tsp00_Int2;
            ounused1      : tsp00_Int2;       (* free *)
            oprivset      : tak_columnset;
            osetallpriv   : tak_columnset;
      END;
 
 
      tak_dml_onefield  =  RECORD   (* a55-a58 *)
            dml_col_ptr           : tak00_colinfo_ptr;
            dml_index             : tsp00_Int2;
            dml_colno_in_subquery : tsp00_Int2;
            dml_node              : tsp00_Int2;
            dml_res_index         : tsp00_Uint1;
            dml_fill              : tsp00_Uint1;
      END;
 
      tak_tab_allow_state  =  (only_one_tab,
            may_more_tabs,
            are_more_tabs,
            tabs_with_arith);
      tak_all_from_tables  =  ARRAY [ 1..cak00_maxsources ] OF tak_one_table;
      tak_all_from_tables_ptr = ^tak_all_from_tables;
      tak_corr_type  =  (no_correlation,
            correlation,
            first_correlation,
            lowest_of_correlation,
            having_correlation);
      tak_inv_only  =  (ind_init, ind_no_inv, ind_inv );
 
      tak_dml_info  =  RECORD               (* !! a54_dml_init !! *)
            d_colptr             : tak00_colinfo_ptr;
            d_colbuf             : tak00_colinfo_ptr;
            d_esparr             : tak_syspointerarr;
            d_resbuf_addr        : tsp00_BufAddr;
            (* sparr used for resultset *)
&           ifdef BIT64
&           else
            d_alpha_fill1        : PACKED ARRAY [ 1..68 ] OF char;
&           endif
            d_user               : tsp00_KnlIdentifier;
            d_table              : tsp00_KnlIdentifier;
            d_column             : tsp00_KnlIdentifier;
            d_refname            : tsp00_KnlIdentifier; (* for NAME =  ... *)
            (* --- *)
            d_upd_set            : tak_columnset;
            d_foundset           : tak_columnset;
            d_param_st_index     : tsp00_Int2;
            d_param_st_begin     : tsp00_Int2;
            (* --- *)
            d_first_tab          : tsp00_Int2;
            d_udt_datatype       : tsp00_C2;
            d_pargeslen          : tsp00_Int2;
            d_movebefore         : tsp00_Int2;
            (* --- *)
            d_index              : tsp00_Int2;
            d_p_filler1          : tsp00_Int2;
            d_vppos              : tsp00_Int4;
            (* --- *)
            d_exprno             : tsp00_Int2; (*no used for expressions*)
            d_cntfromtab         : tsp00_Int2;
            d_acttabindex        : tsp00_Int2;
            d_fieldno            : tsp00_Int2;
            (* --- *)
            d_keylen             : tsp00_Int2;
            d_outcolno           : tsp00_Int2;
            d_maxlen             : tsp00_Int4;
            (* --- *)
            d_pos_in_parsbuf     : tsp00_Int4;
            d_inoutpos           : tsp00_Int4;
            d_parameter_count    : tsp00_Int2;
            d_parnum             : tsp00_Int2;
            d_checkview          : boolean;
            d_join_dml           : boolean;
            d_colindex           : tsp00_Int2;
            (* --- *)
            d_reclen             : tsp00_Int2;
            d_standard           : boolean; (*sequence of jointables is given*)
            d_wherepart          : boolean;
            d_range              : boolean;
            d_nullkey            : boolean;
            d_like               : boolean;
            d_key                : boolean;
            (* --- *)
            d_single             : boolean; (* true during *)
            (* select direct/ordered/into *)
            d_use_order          : boolean;
            d_arith_where        : boolean;
            d_join               : boolean;
            d_reuse              : boolean;
            d_group              : boolean;
            d_having             : boolean;
            d_datatype           : tsp00_DataType;
            (* --- *)
            d_allowed            : tak_tab_allow_state;
            d_pars_kind          : tak_fp_kind_type;
            d_maxcounttabs       : tsp00_Int2;
            d_basetabno          : tsp00_Int2;
            d_corr               : tak_corr_type;
            d_lowpars            : tsp00_Uint1;
            (* --- *)
            d_level              : tak_corlevel;
            d_align_fill1        : boolean;
            d_oldlowpars         : tsp00_Uint1;
            d_prep_corr_cols     : boolean;
            d_like_expression    : boolean;
            (* --- *)
            d_n_pos              : tsp00_Int4;
            d_filled_bytes       : tsp00_Int2;
            d_escape_char        : tsp00_C2;
            (* --- *)
            d_expand             : tsp00_Int2;
            d_concat             : boolean;
            d_hex                : boolean;
            d_first_union        : boolean;
            d_change_date_time   : boolean;
            d_one_join_phase     : boolean;
            d_check_null_byte    : boolean;
            (* --- *)
            d_const_value_expr   : boolean;
            d_is_parameter_only  : boolean;
            d_strat_info         : tgg00_RefInfoIndex;
            d_truncate           : boolean;
            d_topos              : tsp00_Int2;
            d_in_method_param    : boolean;
            d_escape             : boolean;
            (* --- *)
            d_mselect_rescnt     : tsp00_Int4;
            d_execute_unlock     : boolean;
            d_pseudo_ins_select  : boolean;
            d_viewdesc_linkage   : tsp00_C2;
            (* --- *)
            d_inv_colno          : tak_inv_colnos;
            d_resname            : tsp00_KnlIdentifier;
            d_oj_tables          : tak_joinset;
            (* --- *)
            d_all_count          : boolean;
            d_repl_reskey        : boolean;
            d_mselect_loop_cnt   : tsp00_Int2;
            d_inv_cnt            : tsp00_Uint1;
            d_single_expr        : boolean;
            d_only_table_scan    : boolean;
            d_in_case_function   : boolean;
            (* --- *)
            d_where_corr           : boolean;
            d_where_subquery       : boolean;
            d_where_corr_subquery  : boolean;
            d_having_subquery      : boolean;
            d_having_corr_subquery : boolean;
            d_type_long_allowed    : boolean;
            d_optimize_setfunction : boolean;
            d_specialjoinfromselect    : boolean;   (* PTS 1138343 D.T. *)
            (* --- *)
            (* length is mxak_part_dml_init                  *)
            (* variables above are always initialized        *)
            (* --- *)
            d_sparr              : tak_syspointerarr;
            d_linkbuf            : tak_sysbufferaddress;
            d_viewtextbuf        : tak_sysbufferaddress;
            d_viewdescbuf        : tak_sysbufferaddress;
            (* --- *)
            d_literalrec         : tak_sysbufferaddress;
            (* --- *)
&           ifdef BIT64
&           else
            d_alpha_fill2        : PACKED ARRAY [ 1..72 ] OF char;
&           endif
            (* --- *)
            d_tableid            : tgg00_Surrogate;
            d_table_as_id        : tgg00_Surrogate;
            (* --- *)
            d_act_node           : tsp00_Int2;
            d_cntpar             : tsp00_Int2;
            d_globstate          : tgg00_HandlingSet;
            d_unchanged_globstate: tgg00_HandlingSet;
            (* --- *)
            d_limit_offset       : tsp00_Int4;
            d_upper_limit        : tsp00_Int4;
            (* --- *)
            d_rowno              : tsp00_Int4;
            d_pos_result         : tsp00_Int2;
            d_resname_found      : boolean; (* resultset name *)
            d_use_sub            : boolean;
            (* --- *)
            d_viewusername       : tsp00_KnlIdentifier;
            d_viewtablen         : tsp00_KnlIdentifier;
            (* --- *)
            d_subquery           : boolean;
            d_sublevel           : tsp00_Uint1;  (* only for executing *)
            d_subcount           : tsp00_Int2;
            d_phase_cnt          : tsp00_Int2;
            d_only_sem_check     : boolean;
            d_union              : boolean;
            (* --- *)
            d_union_order_n      : tsp00_Int2;
            d_union_insert       : boolean;
            d_outer_join         : boolean;
            d_repl_nokey_ins_sel : boolean;
            d_is_function_column : boolean;
            d_global_pos_result  : tsp00_Int2;
            (* --- *)
            d_longdesc_found     : boolean;
            d_unused1            : boolean;  (* unused *)
            d_dist_optim         : tsp00_Int2;
            d_qual_kind          : tgg00_QualKind;
            d_view_col_list      : boolean;
            d_view_col_node      : tsp00_Int2;
            (* --- *)
            (* PTS 1119397 E.Z. *)
            d_massdata_ptr       : tsp00_MoveObjPtr;
&           ifdef BIT64
&           else
            d_alpha_fill4        : PACKED ARRAY [ 1..4 ] OF char;
&           endif
            d_local_massdata_part: boolean;
            d_further_fill1      : boolean;        (* unused *)
            d_longlitcount       : tsp00_Int2;
            d_distinct           : tgg04_Distinct;
            d_view               : boolean;
            d_union_limit_n      : tsp00_Int2;
            (* --- *)
            d_initial_ex_parskey : tak_parskey;
            (* --- *)
            (* length is mxak_full_dml_init                           *)
            (* variables above are only initialized when NOT in_union *)
            (* --- *)
            (* following variables will be not or partly initialized  *)
            d_tabarr             : tak_all_from_tables_ptr;
&           ifdef BIT64
&           else
            d_alpha_fill5        : PACKED ARRAY [ 1..4 ] OF char;
&           endif
            d_tabarr_capacity    : tsp00_Int4;
            d_fill5              : tsp00_Int4;
            d_joins              : tak_joinrec;
            d_change             : tak_changerecord;
            d_change_align       : tsp00_Int4;
            (* d_change also used for insert..select *)
            (* --- *)
            d_order_cols         : tak00_ord_fields;
            d_group_cols         : tak00_ord_fields;
            d_order_or_group_cols: tak00_ord_fields_ptr;
&           ifdef BIT64
&           else
            d_alpha_fill3        : PACKED ARRAY [ 1..4 ] OF char;
&           endif
            (* --- *)
            d_ch_datatype        : tsp00_DataType;
            d_like_optimize      : boolean;
            d_like_part          : tsp00_Uint1;
            d_string_column      : boolean;
            (* PTS 1111575 E.Z. *)
            d_unused11           : boolean;      (* unused *)
            d_with_lock_clause   : boolean;
            d_first_longdesc_pos : tsp00_Int2;
            (* --- *)
            d_cnt_longdesc       : tsp00_Int2;
            d_subquery_node      : tsp00_Int2;
            d_fromtabnode        : tsp00_Int2; (* PTS 1128197 D.T. *)
            d_columnindex        : tsp00_Int2; (* will be filled with cextcolno *)
            d_ins_sel_tabid      : tgg00_Surrogate;
            (* --- *)
            d_init_tabarr        : ARRAY[1..cak_init_join_tab_size] OF tak_one_table;
            d_jrc_initial_arr    : tak_one_join;
            d_align_fill3        : tsp00_Int4;
      END;
 
      tak_part_dml_info = PACKED ARRAY [ 1..mxak_full_dml_init ] OF char;
      tak_dfarr  =  ARRAY [ 1..MAX_COL_PER_TAB_GG00 ] OF tak_dml_onefield;
 
      tak_select_record  =  RECORD
            sel_res_tree          : tgg00_FileId;
            sel_res_name          : tsp00_KnlIdentifier;
            sel_output_n          : tsp00_Int2;
            sel_into_n            : tsp00_Int2;
            sel_from_n            : tsp00_Int2;
            sel_group_n           : tsp00_Int2;
            sel_having_n          : tsp00_Int2;
            sel_order_n           : tsp00_Int2;
            sel_limit_n           : tsp00_Int2;
            sel_res_b             : tsp00_Int2;
            sel_parinf_b          : tsp00_Int2;
            sel_is_not_corr_search: boolean;
            sel_new_atdistinct    : tgg04_Distinct;
            sel_last_pars_part    : boolean;
            sel_where_done        : boolean;
            sel_view_done         : boolean;
            sel_has_subquery      : boolean;
      END;
 
      tak_pageno_array   =  ARRAY [ 1..MAX_COL_PER_TAB_GG00 ] OF tsp00_Int4;
      tak_fs_value_qual  =  (c_startpos, c_endpos, c_length4, c_length2);
      tak_fields_to_use  =  (no_fields, keyfields, corr_fields);
 
      tak_corr_infos  =  RECORD
            co_corr_current    : tgg00_FileId;
            co_corr_buf        : tsp00_BufAddr;
            co_use_cinfo       : boolean;
            co_use_fields      : tak_fields_to_use;
            co_all_recs_len    : tsp00_Int2;
            co_one_rec_len     : tsp00_Int2;
            co_one_key_len     : tsp00_Int2;
            co_lcomp_len       : tsp00_Int2;
            co_startkeyfields  : tsp00_Int2;   (* new mess_buf *)
            co_act_pos         : tsp00_Int2;
            co_sel_keypos      : tsp00_Int2;
            co_upd_keylen      : tsp00_Int2;
            co_minkeylen       : tsp00_Int2;
            co_varkeyvalstack  : tsp00_Int2;
            co_fill            : tsp00_Int2;
      END;
 
 
      tak_fill_rec  =  RECORD
            fr_f_no          : tsp00_Int2;
            fr_last_fno      : tsp00_Int2;
            fr_total_leng    : tsp00_Int2;
            fr_leng          : tsp00_Int2;
            fr_uni_fill_c2   : tsp00_C2;
            fr_filler0       : tsp00_Int2;
      END;
 
 
      tak_exec_rec  =  RECORD
            ex_act_treeid          : tgg00_FileId;
            ex_all_rescnt          : tsp00_Int4;
            ex_plus                : tsp00_Int2;
            ex_maxbufs             : tsp00_Int2;
            ex_with_link           : boolean;
            ex_output_wanted       : boolean;
            ex_command_needed      : boolean;
            ex_fill                : boolean;
            ex_mb_data             : tgg00_DataPartPtr; (* PTS 1112612 *)
            ex_updjv_ptr           : tsp00_MoveObjPtr;
            ex_parameter_fieldlist : tsp00_MoveObjPtr;
            ex_result_fieldlist    : tsp00_MoveObjPtr;
      END;
 
 
      tak_factorrec  =  RECORD
            fr_minus_n     : integer;
            fr_jpos        : integer;
            fr_error       : boolean;
            fr_describ     : boolean;
            fr_minus_symb  : boolean;
            fr_getnumb     : boolean;
      END;
 
 
      tak_litcoldesc  =  RECORD
            llclongid       : tgg00_Surrogate;
            llcdatapos      : tsp00_Int4;
            llcdatalen      : tsp00_Int4;
            llwith_shortcol : boolean;
            llfiller1       : boolean;
            llfiller2       : tsp00_Int2;
            llfiller3       : tsp00_Int4;
      END;
 
      tak_litcoldesc_arr    =  ARRAY[ 1..cak_max_litcoldesc ] OF
            tak_litcoldesc;
      tak_longlit_buf    =  PACKED ARRAY[ 1..mxak_longlit_buf ] OF char;
 
      tak_longliteral_rec  =  RECORD
            litreclen        : tsp00_Int2;
            litkeylen        : tsp00_Int2;
            litvarcol_offset : tsp00_Int2;
            litvarcol_cnt    : tsp00_Int2;
            litsurrogate     : tgg00_Surrogate;
            litentrytype     : tsp00_C2;
            litlinkage       : tsp00_C2;
            litfullen        : tsp00_Int4;
            litfiller        : tsp00_Int4;
            littabid         : tgg00_Surrogate;
            litcolcount      : tsp00_Int2;
            litfiller1       : tsp00_Int2;
            litlen           : tsp00_Int4;
            litsize          : tsp00_Int4;
            litdataptr       : tsp00_MoveObjPtr;
&           ifdef BIT64
&           else
            libit64fill      : tsp00_Int4;
&           endif
            CASE boolean OF
                true :
                    (litcoldesc     : tak_litcoldesc_arr);
                false :
                    (litdatabuf     : tak_longlit_buf);
                END;
            (*ENDCASE*) 
 
 
      tak_longdemand_rec = RECORD
            ldem_position      : tsp00_Int4;
            ldem_length        : tsp00_Int4;
            ldem_getatonce     : boolean;
      END; (* PTS 1122546 D.T. *)
 
      tak_longdemand_ptr = ^tak_longdemand_rec; (* PTS 1122546 D.T. *)
 
      tak_hintrecord  =  RECORD
            hint_reclen        : tsp00_Int2;
            hint_keylen        : tsp00_Int2;
            hint_varcol_offset : tsp00_Int2;
            hint_varcol_cnt    : tsp00_Int2;
            hint_surrogate     : tgg00_Surrogate;
            hint_entrytype     : tsp00_C2;
            hint_linkage       : tsp00_C2;
            hint_filler        : tsp00_Int2;
            hint_joincfg_cnt   : tsp00_Int2;
            hint_common_config : tak00_access_configuration;
            hint_join_config   : tak00_join_configuration_arr;
      END;
 
 
      tak_systembuffer  =  RECORD
            CASE integer OF
                0 :
                    (b_sl           : tsp00_Int2;
                    b_kl            : tsp00_Int2;
                    b_varcol_offset : tsp00_Int2;
                    b_varcol_cnt    : tsp00_Int2);
                1 :
                    (sbase        : tak_baserecord);
                2 :
                    (scomment     : tak_comment_record);
                3  :
                    (scomplexrec  : tak_complexrecord);
                4  :
                    (sconstraint  : tak_constraintrecord);
                5  :
                    (sconstrname  : tak_constraintnamerecord);
                6  :
                    (scorr        : tak_correcord);
                8  :
                    (sdefault     : tak_defaultrecord);
                9   :
                    (sdomain      : tak_domainrecord);
                10  :
                    (sdomainref   : tak_domainref);
                (* PTS 1111575 E.Z. *)
                14 :
                    (slink        : tak_linkrecord);
                15 :
                    (slinkdefault : tak_link_def_record);
                16 :
                    (slinkparsinfo : tak_linkparsrecord);
                17 :
                    (slinkposinfo : tak_linkinforecord);
                18 :
                    (sserverdb    : tak_serverdbrecord);
                19 :
                    (sserverdbname : tak_serverdbnamerecord);
                22 :
                    (smindex      : tak_mindexrecord);
                23 :
                    (sparsinfo    : tak_parsinforecord);
                24 :
                    (spriv        : tak_privrecord);
                25 :
                    (sprivuser    : tak_privuserrecord);
                26 :
                    (sreskey      : tak_resultkeyrecord);
                27 :
                    (shint        : tak_hintrecord);
                28 :
                    (sresult      : tak_resultrecord);
                29 :
                    (sscoldesc    : tak_scoldescrecord);
                30 :
                    (ssequence    : tak_sequencerecord);
                31 :
                    (sshortinfo   : tak_shortinforecord);
                33 :
                    (ssubtrans    : tak_subtransrecord);
                34 :
                    (ssynonym     : tak_synonymrecord);
                35 :
                    (ssysdba      : tak_sysdbarecord);
                36 :
                    (stableref    : tak_tablereference);
                38 :
                    (strigger     : tak_triggerrecord);
                40 :
                    (sunionrec    : tak_unionrecord);
                41 :
                    (susage       : tak_usagerecord);
                42 :
                    (suser        : tak_userrecord);
                43 :
                    (suserid      : tak_useridrecord);
                44 :
                    (suserref     : tak_userrefrecord);
                45 :
                    (susertab     : tak_usertablerecord);
                46 :
                    (sviewdesc    : tak_viewdescrecord);
                47 :
                    (sviewkey     : tak_viewkeyrecord);
                (*     *)
                (* 48 is unused       *)
                (*     *)
                49 :
                    (sviewtext    : tak_viewtextrecord);
                50 :
                    (syskey       : tgg00_SysInfoKey);
                51 :
                    (ssysbuf: PACKED ARRAY[ 1..mxak_systembuf ] OF char);
                52 :
                    (sresname     : tak_resname_record);
                53 :
                    (smapset      : tak_mapset_record);
                54 :
                    (sfuncref     : tak_funcrefrecord);
                55 :
                    (sprocseqpriv : tak_proc_seq_privilege);
                56 :
                    (scol_uses_dom : tak_col_uses_dom);
                57 :
                    (salterdate    : tak_alterdaterecord);
                59 :
                    (scolnames    : tak_columnnamesrecord);
                60 :
                    (sstored_cmd  : tak_stored_cmd_record);
                61 :
                    (schangeinfos : tak_changeinfos);
                62 :
                    (smessblock : tak_messblock_record);
                63 :
                    (sstatistics : tak_statisticsinfo);
                (*     *)
                (* 64 is unused       *)
                (*     *)
                65 :
                    (slongliteral : tak_longliteral_rec);
                66 :
                    (smethod      : tak_methodrecord);
                67 :
                    (smethodref   : tak_methodrefrecord);
                68 :
                    (stypeuidmap  : tak_type_uid_map_record);
                69 :
                    (smethodinfo  : tak_method_inforecord);
                70 :
                    (sclassmap    : tak_class_map_guid_record);
                71 :
                    (sclass       : tak_class_record);
                72 :
                    (sversion     : tak_version_record);
                73 :
                    (scommand_info: tak_commandinforecord);
                74 :
                    (sviewqual_basis : tak_viewqual_basis_record);
                75 :
                    (sviewqual_join : tak_viewqual_join_record);
                76 :
                    (sviewqual_stack : tak_viewqual_stack_record);
                77 :
                    (sviewqual_derivedcol : tak_viewqual_derivedcol_record);
                78 :
                    (sschema : tak_schema_record);
                79 :
                    (sschemaref : tak_schema_ref_record);
                80 :
                    (smethodfileinfo : tak_method_fileinforecord);
                81 :
                    (smethoddebuginfo : tak_method_debuginfo);
                END;
            (*ENDCASE*) 
 
      tak_mem_type = (
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
            dyn_syntax_tree);
      tak_balance_info = (left_weighted, balanced, right_weighted);
 
      tak_globalCacheReference = RECORD
            cd_pos               : tsp00_Int4;
            cd_check             : tsp00_Int2;
            cd_refCount          : tsp00_Int2;
            cd_less              : tak_cache_dir_pointer;
            cd_greater           : tak_cache_dir_pointer;
            cd_fix_prev          : tak_cache_dir_pointer;
            cd_fix_next          : tak_cache_dir_pointer;
            cd_lru_prev          : tak_cache_dir_pointer;
            cd_lru_next          :  tak_cache_dir_pointer;
            cd_cmd_no            : tsp00_Int4;
            cd_create_cmd_no     : tsp00_Int4;
            cd_state             : tak_sysdir_state;
            cd_balance           : tak_balance_info;
            cd_buf_no            : tsp00_Int2;
            cd_syslen            : tsp00_Int4;
            cd_globalCacheHandle : tgg00_VoidPtr;
            cd_globalSysinfoPtr  : tak_sysbufferaddress;
            cd_prevRef           : tak_cache_dir_pointer;
            cd_nextRef           : tak_cache_dir_pointer;
      END;
 
 
      tak_cache_dir_entry = RECORD
            CASE integer OF
                1 :
                    (cd_pos          : tsp00_Int4;
                    cd_check         : tsp00_Int2;
                    cd_refCount      : tsp00_Int2;
                    cd_less          : tak_cache_dir_pointer;
                    cd_greater       : tak_cache_dir_pointer;
                    cd_fix_prev      : tak_cache_dir_pointer;
                    cd_fix_next      : tak_cache_dir_pointer;
                    cd_lru_prev      : tak_cache_dir_pointer;
                    cd_lru_next      : tak_cache_dir_pointer;
                    cd_cmd_no        : tsp00_Int4;
                    cd_create_cmd_no : tsp00_Int4;
                    cd_state         : tak_sysdir_state;
                    cd_balance       : tak_balance_info;
                    cd_buf_no        : tsp00_Int2;
                    cd_syslen        : tsp00_Int4;
                    cd_syskey        : tgg00_SysInfoKey);
                2 :
                    (cd_block_pos    : tsp00_Int4;
                    cd_block_check   : tsp00_Int2;
                    cd_blockRefCount : tsp00_Int2;
                    cd_bl_less       : tak_cache_dir_pointer;
                    cd_next_free     : tak_cache_dir_pointer;
                    cd_bl_fix_prev   : tak_cache_dir_pointer;
                    cd_bl_fix_next   : tak_cache_dir_pointer;
                    cd_bl_lru_prev   : tak_cache_dir_pointer;
                    cd_bl_lru_next   : tak_cache_dir_pointer;
                    cd_free_fill     : tsp00_Int4;
                    cd_block_len     : tsp00_Int4;
                    cd_bl_state      : tak_sysdir_state;
                    cd_mem_type      : tak_mem_type;
                    cd_bl_buf_no     : tsp00_Int2;
                    cd_bl_syslen     : tsp00_Int4;
                    cd_sysinfo       : tak_systembuffer);
                3 :
                    (cd_ref        : tak_globalCacheReference);
                END;
            (*ENDCASE*) 
 
      tak_odbc_partype = ( no_odbc,
            normal_odbc,
            extended_odbc );
      tak_odbc_functiontype = ( odbc_nothing, odbc_call, odbc_fn,
            odbc_oj, odbc_d, odbc_t, odbc_ts, odbc_esc, odbc_dbfunc);
 
      tak_dbproc_stack_param = RECORD
            CASE integer OF
                1 :
                    (i1  : tsp00_C1);
                2 :
                    (c2  : tsp00_C2);
                3 :
                    (i2  : tsp00_Int2);
                4 :
                    (i4  : tsp00_Int4);
                5 :
                    (r4  : tsp00_Shortreal);
                6 :
                    (r8  : tsp00_Longreal);
                7 :
                    (i8  : tsp00_8ByteCounter);
                8 :
                    (ptr : tsp00_MoveObjPtr);
                END;
            (*ENDCASE*) 
 
 
      tak_dbproc_stack_param_debug = RECORD
            dbg_value    : tak_dbproc_stack_param;
            dbg_vt       : tsp00_Int2;
            dbg_inout    : tsp1_param_io_type;
            dbg_is_ptr   : boolean;
            dbg_length   : tsp00_Int4;
      END;
 
 
      tak_fromsel_tabid = RECORD
            CASE integer OF
                1 :
                    (fromsiteno : tgg00_ServerdbNo;
                    session     : tgg91_SessionNo;
                    ft          : tgg00_TfnTemp;
                    fs_no       : tsp00_Uint1);
                2 :
                    (fparschar  : tsp00_C1;
                    fcmd_count  : tsp00_C3;
                    ffill       : tsp00_Int2;
                    ft1         : tgg00_TfnTemp;
                    fs_no1      : tsp00_Uint1);
                3 :
                    (tabid      : tgg00_Surrogate);
                END;
 
