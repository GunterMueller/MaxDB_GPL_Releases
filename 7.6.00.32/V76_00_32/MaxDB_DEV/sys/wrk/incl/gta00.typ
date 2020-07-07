
      tts_param_array = ARRAY [1..cta_max_res_fields] OF tsp1_param_info;
      tts_ident_array = ARRAY [1..cta_max_res_fields] OF tsp00_KnlIdentifier;
 
      tta_fields = RECORD
            cnt_res_fields : tsp00_Int2;
            res_fieldparam : tts_param_array;
            res_fieldname  : tts_ident_array;
      END;
 
 
      tta_sresults = RECORD
            cnt_sres_fields : tsp00_Int2;
            n_sres_filler   : tsp00_Int2;
            sres_fields     : ARRAY [1..cta_max_res_fields] OF tsp00_SfieldInfo;
      END;
 
      tta_input_params = ARRAY [1..cta_max_input_fields] OF tsp1_param_info;
 
      tta_ex_in = RECORD
            cnt_input_fields : tsp00_Int2;
            act_cnt_fields   : tsp00_Int2;
            input_params     : tta_input_params;
      END;
 
      tta_execution_kind = (
            tek_only_parsing,
            tek_only_executing,
            tek_parsing_executing,
            tek_pars_then_execute,
            tek_only_syntax,
            tek_dialog_call,
            tek_dbsload
            );
      tta_simulation = (
            tasim_none,
            tasim_ascii,
            tasim_unicode);
 
      tta_col_id = RECORD
            descr     : tsp00_C8;
            shortinfo : tsp1_param_info;
            free      : boolean;
      END;
 
      tta_col_descr_list = ARRAY [1..5] OF tta_col_id;
 
      tta_prot = RECORD
            name    : tsp00_C20;
            text    : tsp00_C40;
            vfn     : tsp00_VFilename;
            refer   : tut_vf_fileref;
            refsafe : tut_vf_fileref;
            ln      : tsp00_Line;
            errt    : tsp00_ErrText;
            err     : tsp00_Int2;
            common  : boolean;
            no_prot : boolean;
            session : boolean;
      END;
      
      tta_dbprocLongData = RECORD
            dbp_count  : integer;
            dbp_length : array[1..5] of integer;
            dbp_pos    : array[1..5] of integer;
            dbp_addr   : array[1..5] of tsp00_MoveObjPtr;
      END;
 
 
      tta_user_proc_globals = RECORD
            procid           : tsp00_TaskId;
            username         : tsp00_KnlIdentifier;
            param_pw         : tsp00_Pw;
            termid           : tsp00_TermId;
            serverdb         : tsp00_DbName;
            servernode       : tsp00_NodeId;
            utilserverdb     : tsp00_DbName;
            utilservernode   : tsp00_NodeId;
            password         : tsp00_CryptPw;
            left_header      : tsp00_C20;
            right_header     : tsp00_C20;
            msg_header       : tsp00_C30;
            host_swap        : tsp00_SwapKind;
            swap             : tsp00_SwapKind;
            host_code_type   : tsp00_CodeType;
            term_code_type   : tsp00_CodeType;
            inlength         : tsp00_Int4;
            user_no          : tsp00_Int2;
            p_count          : tsp00_Int2;
            cnt              : tsp00_Int4;
            reference        : tsp00_Int4;
            roldtime         : tsp00_Int4;
            virt             : tsp00_Int4;
            term_simulation  : tta_simulation;
            to_break         : boolean;
            byte_swap        : boolean;
            cancel           : boolean;
            handler_order    : boolean;
            info             : boolean;
            is_batch         : boolean;
            is_connected     : boolean;
            is_data_input    : boolean;
            is_ok            : boolean;
            is_virt_term     : boolean;
            is_prompting     : boolean;
            long_errmsg      : boolean;
            rtime            : boolean;
            stop             : boolean;
            sum              : boolean;
            time             : boolean;
            xusertype        : tsp4_xuser;
            code_type        : tsp00_CodeType;
            sqlmode          : tsp00_SqlMode;
            prot             : tta_prot;
            open_cols        : tta_col_descr_list;
            execution        : tta_execution_kind;
            dbprocCall       : boolean;
            parsing          : ARRAY [1..cta_max_parsing] OF tsp00_C12;
            execute_input    : ARRAY [1..cta_max_parsing] OF tta_ex_in;
            (* PTS 1105196 E.Z. *)
            execute_output   : ARRAY [1..cta_max_parsing] OF tta_ex_in;
            dbprocLongData   : tta_dbprocLongData;
            asc_to_ebc       : tsp00_Ctable;
            ebc_to_asc       : tsp00_Ctable;
            digiset          : SET OF char;
            first_token      : tsp00_Sname;
            second_token     : tsp00_Sname;
            bigcmd_offset    : tsp00_Int4;
            term             : ^tut_terminal;
            pfkey            : tut_pfkey;
            appl_version     : tsp00_C5;
            application      : tsp00_C3;
            infparams        : tta_fields;
            extern_enc       : tsp00_Int2;
            curr_segm        : tsp1_segment_ptr;
            curr_ret_segm    : tsp1_segment_ptr;
            more_segments    : tsp00_Int2;
            appl_param_infos : boolean;
            mass_cmd         : boolean;
            prepare          : boolean;
            command_options  : tsp1_command_options;
            with_abap_part   : boolean;
            file_info        : boolean;
            longdesccnt      : tsp00_Int2;
            longdescs        : tsp00_Line;
            shared_sql_drop  : boolean;
            shared_drop_trans: boolean;
            longdem_cnt      : tsp00_Int4;
            longdem_pos      : tsp00_Int4;
            longdem_length   : tsp00_Int4;
            longdem_atonce   : boolean;
            variable_input   : boolean;
            check_scrollopt  : boolean;
            autocommit       : boolean;
            ascii_in_output  : boolean;
            with_module_part : tsp00_Int4;
      END;
 
      tta_user_proc_ptr = ^tta_user_proc_globals;
      tta_terminal_ptr  = ^tut_terminal;
      tta_muli_waits    = PACKED ARRAY [1..cta_max_muli_procs] OF tsp00_Int2;
 
      tta_muli_user = RECORD
            procid    : tsp00_TaskId;
            upg_ptr   : tta_user_proc_ptr;
            term_ptr  : tta_terminal_ptr;
            next_wait : tsp00_Int2;
            my_buf    : tsp00_Int2;
            rc        : tsp00_Int2;
            busy      : boolean;
            us_fill1  : boolean;
            wait      : tta_muli_waits
      END;
 
      tta_all_muli_users = PACKED ARRAY [0..cta_max_muli_procs]
            OF tta_muli_user;
      tta_buf_state = (
            free_buf,
            dirty_buf,
            for_user,
            for_prot,
            for_handler);
 
      tta_muli_buf = RECORD
            stamp  : tsp00_Int4;
            txt    : tsp00_Line;
            buflen : tsp00_Int2;
            state  : tta_buf_state;
            filler : boolean;
      END;
 
      tta_all_muli_buf = ARRAY [1..cta_max_muli_buf] OF tta_muli_buf;
      
      (* PTS 1111575 E.Z. *)
       
