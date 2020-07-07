
      tsp1_cmd_mess_type = (
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
            sp1m_last_cmd_mess_type);
      (* *)
      tsp1_comm_error = (
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
            sp1ce_released);
      (* *)
      tsp1_utility_startup = (
            sp1us_connect,
            sp1us_restart,
            sp1us_shutdown);
      (* *)
      tsp1_intern_warning = (
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
            sp1iw_warn15);
      (* *)
      tsp1_warningset_intern = PACKED SET OF tsp1_intern_warning;
      (* *)
      tsp1_command_option = (
            sp1co_selfetch_off,
            sp1co_scrollable_cursor_on,
            sp1co_no_resultset_close_needed,
            sp1co_not_used2,
            sp1co_not_used3,
            sp1co_not_used4,
            sp1co_not_used5,
            sp1co_not_used6);
      (* *)
      tsp1_command_options = SET OF tsp1_command_option;
      (* *)
      tsp1_part_kind = (
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
            sp1pk_long_demand, (* PTS 1122546 D.T. *)
            sp1pk_message_list,
            sp1pk_vardata_shortinfo,
            sp1pk_vardata,
            sp1pk_feature,
            sp1pk_clientid,
            sp1pk_last_part_kind);
      (* *)
      tsp1_part_attributes = (
            sp1pa_last_packet,
            sp1pa_next_packet,   (* h.b. CR 1000072 *)
            sp1pa_first_packet,
            sp1pa_fill3,
            sp1pa_fill4,
            sp1pa_fill5,
            sp1pa_fill6,
            sp1pa_fill7);
      (* *)
      tsp1_part_attr = SET OF tsp1_part_attributes;
      (* *)
      tsp1_producer = (
            sp1pr_nil,
            sp1pr_user_cmd,
            sp1pr_internal_cmd,
            sp1pr_kernel,
            sp1pr_installation,
            sp1pr_view_optimizer,
            sp1pr_complex_view_handling,
            sp1pr_queryrewrite);
      (*                sp1pr_internal_cmd given for example by LOAD, *)
      (*                if command was not given by the user but      *)
      (*                needed by LOAD for internal use               *)
      (* *)
      tsp1_segment_kind = (
            sp1sk_nil,
            sp1sk_cmd,
            sp1sk_return,
            sp1sk_proccall,
            sp1sk_procreply,
            sp1sk_last_segment_kind);
      (* *)
      tsp1_sqlmode = (
            sp1sm_nil,
            sp1sm_session_sqlmode,
            sp1sm_internal,
            sp1sm_ansi,
            sp1sm_db2,
            sp1sm_oracle);
 
      tsp1_part_header = RECORD
            sp1p_part_kind   : tsp1_part_kind;
            sp1p_attributes  : tsp1_part_attr;
            sp1p_arg_count   : tsp00_Int2;
            sp1p_segm_offset : tsp00_Int4;
            (* *)
            sp1p_buf_len     : tsp00_Int4;
            sp1p_buf_size    : tsp00_Int4
      END;
 
 
      tsp1_part = RECORD
            CASE integer OF
                1:
                    (sp1p_part_header : tsp1_part_header;
                    sp1p_buf          : tsp00_MoveObj);
                2:
                    (sp1p_part_kind   : tsp1_part_kind;
                    sp1p_attributes   : tsp1_part_attr;
                    sp1p_arg_count    : tsp00_Int2;
                    sp1p_segm_offset  : tsp00_Int4;
                    sp1p_buf_len      : tsp00_Int4;
                    sp1p_buf_size     : tsp00_Int4;
                    sp1p_resnum       : tsp00_ResNum);
                END;
            (*ENDCASE*) 
 
      tsp1_part_ptr = ^tsp1_part;
 
      tsp1_segment_header = RECORD
            CASE integer OF
                1:
                    (sp1s_segm_len          : tsp00_Int4;
                    sp1s_segm_offset        : tsp00_Int4;
                    (*  *)
                    sp1s_no_of_parts        : tsp00_Int2;
                    sp1s_own_index          : tsp00_Int2;
                    sp1s_segm_kind          : tsp1_segment_kind;
                    sp1c_mess_type          : tsp1_cmd_mess_type;
                    sp1c_sqlmode            : tsp1_sqlmode;
                    sp1c_producer           : tsp1_producer;
                    (*  *)
                    sp1c_commit_immediately : boolean;
                    sp1c_ignore_costwarning : boolean;
                    sp1c_prepare            : boolean;
                    sp1c_with_info          : boolean;
                    sp1c_mass_cmd           : boolean;
                    sp1c_parsing_again      : boolean;
                    sp1c_command_options    : tsp1_command_options;
                    sp1c_filler1            : boolean;
                    (*  *)
                    sp1c_filler2            : tsp00_C8;
                    sp1c_filler3            : tsp00_C8);
                2:
                    (sp1s_space2            : tsp00_C8;
                    (*  *)
                    sp1s_space3             : tsp00_C4;
                    sp1s_space4             : tsp1_segment_kind;
                    sp1r_sqlstate           : tsp00_SqlState; (* 5 bytes*)
                    sp1r_returncode         : tsp00_Int2;
                    sp1r_errorpos           : tsp00_Int4;
                    (*  *)
                    sp1r_extern_warning     : tsp00_WarningSet;
                    sp1r_intern_warning     : tsp1_warningset_intern;
                    sp1r_function_code      : tsp00_Int2; (* csp1_.._fc *)
                    sp1r_tracelevel         : tsp00_Uint1;
                    sp1r_filler1            : boolean;
                    sp1r_filler2            : tsp00_C8)
                END;
            (*ENDCASE*) 
 
 
      tsp1_segment = RECORD
            CASE integer OF
                1:
                    (sp1s_segm_header       : tsp1_segment_header;
                    sp1p_part               : tsp1_part);
                2:
                    (sp1s_space1            : tsp1_segment_header;
                    sp1p_part_header        : tsp1_part_header;
                    sp1p_buf                : tsp00_MoveObj);
                3:
                    (sp1s_segm_len          : tsp00_Int4;
                    sp1s_segm_offset        : tsp00_Int4;
                    sp1s_no_of_parts        : tsp00_Int2;
                    sp1s_own_index          : tsp00_Int2;
                    sp1s_segm_kind          : tsp1_segment_kind;
                    (* --- cmd segment header --- *)
                    sp1c_mess_type          : tsp1_cmd_mess_type;
                    sp1c_sqlmode            : tsp1_sqlmode;
                    sp1c_producer           : tsp1_producer;
                    sp1c_commit_immediately : boolean;
                    sp1c_ignore_costwarning : boolean;
                    sp1c_prepare            : boolean;
                    sp1c_with_info          : boolean;
                    sp1c_mass_cmd           : boolean;
                    sp1c_parsing_again      : boolean;
                    sp1c_command_options    : tsp1_command_options;
                    sp1c_filler1            : boolean;
                    sp1c_filler2            : tsp00_C8;
                    sp1c_filler3            : tsp00_C8);
                4:
                    (sp1s_space2            : tsp00_C8;
                    sp1s_space3             : tsp00_C4;
                    sp1s_space4             : tsp1_segment_kind;
                    (* --- return segment header --- *)
                    sp1r_sqlstate           : tsp00_SqlState; (* 5 bytes*)
                    sp1r_returncode         : tsp00_Int2;
                    sp1r_errorpos           : tsp00_Int4;
                    sp1r_extern_warning     : tsp00_WarningSet;
                    sp1r_intern_warning     : tsp1_warningset_intern;
                    sp1r_function_code      : tsp00_Int2; (* csp1_.._fc *)
                    sp1r_tracelevel         : tsp00_Uint1;
                    sp1r_filler1            : boolean;
                    sp1r_filler2            : tsp00_C8;
                    (* --- first part --- *)
                    sp1p_part_kind          : tsp1_part_kind;(* |     *)
                    sp1p_attributes         : tsp1_part_attr;(* |first*)
                    sp1p_arg_count          : tsp00_Int2;      (* |part *)
                    sp1p_segm_offset        : tsp00_Int4;      (* |head *)
                    sp1p_buf_len            : tsp00_Int4;      (* |     *)
                    sp1p_buf_size           : tsp00_Int4;      (* |     *)
                    sp1p_resnum             : tsp00_ResNum)
                END;
            (*ENDCASE*) 
 
      tsp1_segment_ptr = ^tsp1_segment;
      (* *)
 
      tsp1_packet_header = RECORD
            sp1h_mess_code    : tsp00_CodeType; (* code of old packet *)
            sp1h_mess_swap    : tsp00_SwapKind; (* swap of old packet *)
            sp1h_filler1      : tsp00_Int2;
            sp1h_appl_version : tsp00_C5;        (* | senderid of      *)
            sp1h_application  : tsp00_C3;        (* |    old packet    *)
            sp1h_varpart_size : tsp00_Int4;
            (*  *)
            sp1h_varpart_len  : tsp00_Int4;
            sp1h_filler2      : tsp00_Int2;
            sp1h_no_of_segm   : tsp00_Int2;
            sp1h_filler3      : tsp00_C8
      END;
 
 
      tsp1_packet = RECORD
            sp1_header           : tsp1_packet_header;
            CASE integer OF
                1:
                    (sp1_varpart : tsp00_MoveObj);
                2:
                    (sp1_segm    : tsp1_segment);
                END;
            (*ENDCASE*) 
 
      tsp1_packet_ptr  = ^tsp1_packet;
      (* *)
      tsp1_packet_list =  ARRAY [1..csp1_max_packet_cnt] OF tsp1_packet_ptr;
      (* *)
      tsp1_param_opt_type = (
            sp1ot_mandatory,
            sp1ot_optional,
            sp1ot_default,
            sp1ot_escape_char);
      (* *)
      tsp1_param_opt_set = SET OF tsp1_param_opt_type;
      (* *)
      tsp1_param_io_type = (
            sp1io_input,
            sp1io_output,
            sp1io_inout);
 
      tsp1_param_info = PACKED RECORD
            sp1i_mode      : tsp1_param_opt_set;
            sp1i_io_type    : tsp1_param_io_type;
            sp1i_data_type  : tsp00_DataType;
            sp1i_frac       : tsp00_Uint1;
            sp1i_length     : tsp00_Int2;
            sp1i_in_out_len : tsp00_Int2;
            CASE boolean OF
                true :
                    (sp1i_bufpos : tsp00_Int4);
                false :
                    (sp1i_param_no : tsp00_Int2;
                    sp1i_read_only : boolean;
                    sp1i_serial    : boolean);
                END;
            (*ENDCASE*) 
            (* die beiden naechsten Typen ersetzen die vsp00-Typen *)
 
 
      tsp1_dcom_param_info = PACKED RECORD
            sp1i_dcom_mode           : tsp1_param_opt_set;
            sp1i_dcom_io_type        : tsp1_param_io_type;
            sp1i_dcom_datatype       : tsp00_Int2;
            sp1i_dcom_sub_datatype   : tsp00_Int2;
            sp1i_dcom_length         : tsp00_Int2;
            sp1i_dcom_dim            : tsp00_Int2;
            sp1i_dcom_frac           : tsp00_Uint1;
            sp1i_dcom_filler         : boolean;
            sp1i_dcom_cpp_offset     : tsp00_Int4;
            sp1i_dcom_ascii_offset   : tsp00_Int4;
            sp1i_dcom_unicode_offset : tsp00_Int4;
            sp1i_dcom_type_uid       : tsp00_C16;
      END;
 
 
      tsp1_param_spec = PACKED RECORD
            para_type   : tsp00_DataType;
            para_frac   : char;
            para_length : tsp00_Int2;
      END;
 
      tsp1_param_spec_list = ARRAY [ 1..csp_maxvariables ] OF
            tsp1_param_spec;
      (* *)
      tsp1_feature =  tsp00_Uint1;
 
      tsp1_one_feature = PACKED RECORD
            of_feature  : tsp1_feature;
            of_value    : tsp00_Uint1;
      END;
 
      tsp1_features = PACKED ARRAY [sp1f_nil_feature..sp1f_last_feature] OF tsp1_one_feature;
 
