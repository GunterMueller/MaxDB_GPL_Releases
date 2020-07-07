
      tut_diag_scan = SET OF (
            utds_buffer_output,
            utds_page_header,
            utds_full_info,
            utds_bd_full_info,
            utds_node,
            utds_key,
            utds_key_first_last,
            utds_key_buf,
            utds_rec,
            utds_dump_label,
            utds_fdir,
            utds_logentry_header,
            utds_logentry_full_info,
            utds_logentry_buf,
            utds_display,
            utds_minbuf,
            utds_edit,
            utds_double_numbering);
 
      tut_vf_fileref = RECORD
            utvf_handle : tsp00_Int4;
            utvf_filler : tsp00_Int4
      END;
 
      tut_fileref_ptr = ^tut_vf_fileref;
 
      tut_file_pos = RECORD
            utfp_block_no        : tsp00_Int4;
            utfp_page_no         : tsp00_Int2;
            utfp_pages_per_block : tsp00_Int2
      END;
 
      tut_dev_type = (
            utdt_nil,
            utdt_data_perm,
            utdt_data_static,
            utdt_log,
            utdt_mirr_log,
            utdt_restart_rec,
            utdt_loginfo,
            utdt_volume_name,
            utdt_magic_rec_file,
            utdt_binary_file);
 
      tut_file = RECORD
            utf_pos           : tut_file_pos;
            utf_cycle_lwb     : tut_file_pos;
            utf_cycle_upb     : tut_file_pos;
            utf_first         : tut_file_pos;
            utf_last          : tut_file_pos;
            (* *)
            utf_entry_pno     : tsp00_Int4;
            utf_entry_pos     : tsp00_Int4;
            (* *)
            utf_cycle_lwb_pno : tsp00_Int4;
            utf_cycle_upb_pno : tsp00_Int4;
            (* *)
            utf_last_pno      : tsp00_Int4;
            utf_first_offset  : tsp00_Int4;
            (* *)
            utf_page_size     : tsp00_Int4;
            utf_page_len      : tsp00_Int4;
            (* *)
            utf_block_size    : tsp00_Int4;
            utf_block_len     : tsp00_Int4;
            (* *)
            utf_fhandle       : tsp00_Int4;
            utf_filler1       : tsp00_Int2;
            utf_rewind        : boolean;      (* PTS 1103859 JA 1999-09-02 *)
            utf_sql_dev_type  : tut_dev_type;
            (* *)
            utf_sql_devno     : tsp00_Int2;
            utf_sql_retcode   : tsp00_Int2;
            utf_sql_ref       : tsp00_Int4;
            (* *)
            utf_sql_packet    : tsp00_Addr;
            utf_sql_prot      : tut_fileref_ptr;
            (* *)
            utf_block         : tsp00_MoveObjPtr;
            utf_page          : tsp00_MoveObjPtr;
            utf_fn            : tsp00_VFilename
      END;
 
      tut_c20_array = ARRAY [1..20] OF tsp00_C20;
      (* *)
      tut_debug = (
            debug_always,
            debug_ut,
            debug_ta);
      (* *)
      tut_pfkey = (
            pf_none,
            pf_cancel,
            pf_down,
            pf_end,
            pf_help,
            pf_left,
            pf_next,
            pf_prev,
            pf_print,
            pf_right,
            pf_run,
            pf_up);
 
      tut_mask_item = RECORD
            CASE integer OF
                1:
                    (in_c256    : tsp00_C256;
                    msg         : tsp00_C20;
                    msg_in_attr : tsp00_C2;
                    upper_case  : boolean;
                    code_input  : boolean;
                    in_len      : tsp00_Int2);
                2:
                    (in_name    : tsp00_Name);
                3:
                    (in_devname : tsp00_DevName);
                4:
                    (in_fn      : tsp00_VFilename);
                5:
                    (in_num1    : tsp00_C1);
                6:
                    (in_num2    : tsp00_C2);
                7:
                    (in_num5    : tsp00_C5);
                8:
                    (in_num10   : tsp00_C10);
                9:
                    (in_ident   : tsp00_KnlIdentifier);
                10:
                    (in_db      : tsp00_DbName);
                11:
                    (in_node    : tsp00_NodeId)
                END;
            (*ENDCASE*) 
 
      tut_termline_type = (
            lt_normal,
            lt_get1name,
            lt_get2name,
            lt_getc256);
 
      tut_termline_desc = RECORD
            ld_type     : tut_termline_type;
            ld_new_page : boolean;
            ld_in_code1 : boolean;
            ld_in_code2 : boolean;
            ld_attr1    : tsp00_C2;
            ld_attr2    : tsp00_C2;
            ld_in_len1  : tsp00_Int2;
            ld_in_len2  : tsp00_Int2
      END;
 
      tut_ok_array       = ARRAY [1..cut_mask_items] OF boolean;
      tut_term_mask      = ARRAY [1..cut_mask_items] OF tut_mask_item;
      (* *)
      tut_term_cmds      = ARRAY [1..cut_max_cmd_lines_term ] OF tsp00_Line;
      (* *)
      tut_term_param_pos = ARRAY [1..cut_max_param_term] OF tsp00_Int2;
      tut_param_ln       = tsp00_C512;
 
      tut_terminal = RECORD
            inputfile           : ARRAY [1..cut_max_files_term] OF tut_vf_fileref;
            inputfn             : ARRAY [1..cut_max_files_term] OF tsp00_VFilename;
            inputfilpos         : ARRAY [1..cut_max_files_term] OF tsp00_Int4;
            param_line          : ARRAY [1..cut_max_files_term] OF tut_param_ln;
            param_lwb           : ARRAY [1..cut_max_files_term] OF tut_term_param_pos;
            param_upb           : ARRAY [1..cut_max_files_term] OF tut_term_param_pos;
            count_cmd_lines     : ARRAY [1..cut_max_cmds_term ] OF tsp00_Int2;
            last_cmd_lines      : ARRAY [1..cut_max_cmds_term ] OF tut_term_cmds;
            lines               : ARRAY [1..cut_max_lines_term] OF tsp00_Line;
            ln_desc             : ARRAY [1..cut_max_lines_term] OF tut_termline_desc;
            prime_number        : ARRAY [1..cut_max_prime_cnt ] OF tsp00_Int4;
            hif                 : tsp00_HifParms;
            blankline           : tsp00_Line;
            blank_window_line   : tsp00_Line;
            screen_buf          : tsp00_ScreenBufAddr;
            screen_att          : tsp00_ScreenBufAddr;
            blankfilename       : tsp00_VFilename;
            bottom_frame_offset : tsp00_Int4;
            help_line_offset    : tsp00_Int4;
            msg_line_offset     : tsp00_Int4;
            screen_chars        : tsp00_Int4;
            term_ref            : tsp00_Int4;
            top_frame_offset    : tsp00_Int4;
            bottom              : tsp00_Int2;
            curr_inputfile      : tsp00_Int2;
            currcol             : tsp00_Int2;
            currline            : tsp00_Int2;
            first_cmd_line      : tsp00_Int2;
            header_lines        : tsp00_Int2;
            last_cmd_line       : tsp00_Int2;
            last_cmd_no         : tsp00_Int2;
            put_linecount       : tsp00_Int2;
            retcode             : tsp00_Int2;
            retcode_external    : tsp00_Int2;
            screen_cols         : tsp00_Int2;
            screen_lines        : tsp00_Int2;
            top                 : tsp00_Int2;
            window_bottom       : tsp00_Int2;
            window_cols         : tsp00_Int2;
            window_currline     : tsp00_Int2;
            window_lines        : tsp00_Int2;
            window_top          : tsp00_Int2;
            used_window_lines   : tsp00_Int2;
            has_sysline         : boolean;
            help_state          : boolean;
            is_batch            : boolean;
            is_cmd_read         : boolean;
            is_display          : boolean;
            is_frame            : boolean;
            is_holding          : boolean;
            is_inputfile        : boolean;
            is_mask             : boolean;
            is_reset_called     : boolean;
            is_virt_term        : boolean;
            screen_available    : boolean;
            screen_changed      : boolean;
            use_pfkeys          : boolean;
            io_ok               : boolean;
            use_script          : boolean;
      END;
 
