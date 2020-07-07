
      tsp4_argline         = tsp00_C132;
      tsp4_xuserkey        = tsp00_C18;
      tsp4_sqlmode_name    = tsp00_C8;
      (* ******* SQLARGS ******* *)
      tsp4_info_result     = (
            sp4inf_ok,
            sp4inf_illegal_user,
            sp4inf_illegal_component);
      tsp4_component       = (
            sp4co_sql_precompiler,
            sp4co_sql_pc_runtime,
            sp4co_sql_easy,
            sp4co_sql_query,
            sp4co_sql_dialog,
            sp4co_sql_appl,
            sp4co_sql_load,
            sp4co_sql_util,
            sp4co_sql_audit,
            sp4co_sql_userx,
            sp4co_sql_param);
      tsp4_xuser           = (
            sp4xu_sql_userdefault,
            sp4xu_sql_usermask,
            sp4xu_sql_userkey,
            sp4xu_sql_userparms,
            sp4xu_sql_servernode,
            sp4xu_sql_serverdb,
            sp4xu_sql_mode,
            sp4xu_sql_cachelimit,
            sp4xu_sql_timeout,
            sp4xu_sql_isolation,
            sp4xu_sql_params,
            sp4xu_sql_dblang);
      tsp4_xuserset        = PACKED SET OF tsp4_xuser;
      tsp4_command_mode    = (
            sp4cm_sql_none,
            sp4cm_sql_run,
            sp4cm_sql_batch,
            sp4cm_sql_import,
            sp4cm_sql_export,
            sp4cm_sql_run_object,
            sp4cm_sql_batch_object,
            sp4cm_sql_comp_vers,
            sp4cm_sql_list,
            sp4cm_sql_list_command,
            sp4cm_sql_select);
 
      tsp4_args_options    = RECORD
            opt_filler1    : tsp00_Int2;
            opt_filler2    : boolean;
            opt_component  : tsp4_component;
            CASE tsp4_component OF
                sp4co_sql_precompiler :
                    (opt_modulefn     : tsp00_VFilename;
                    opt_tracefile     : tsp00_VFilename;
                    opt_prognamel     : tsp00_Int2;
                    opt_progname      : tsp00_KnlIdentifier;
                    opt_modulenamel   : tsp00_Int2;
                    opt_modulename    : tsp00_KnlIdentifier;
                    opt_tpmonidl      : tsp00_Int2;
                    opt_tpmonid       : tsp00_Name;
                    opt_begmar        : tsp00_Int2;
                    opt_endmar        : tsp00_Int2;
                    opt_trace         : tsp00_Int2;
                    opt_fnind         : tsp00_Int2;
                    opt_datetime      : tsp00_Int2;
                    opt_dyn           : tsp00_Int2;
                    opt_tpmon         : tsp00_Int2;
                    opt_mode          : tsp00_Int2;
                    opt_extern        : tsp00_Int2;
                    opt_cansi         : tsp00_Int2;
                    opt_comp          : tsp00_Int2;
                    opt_prof          : tsp00_Int2;
                    opt_packetsize    : tsp00_Int4;
                    opt_list          : boolean;
                    opt_precom        : boolean;
                    opt_comment       : boolean;
                    opt_traceswitch   : boolean;
                    opt_silent        : boolean;
                    opt_nowarn        : boolean;
                    opt_lineno        : boolean;
                    opt_lib           : boolean;
                    opt_quote         : boolean;
                    opt_decpoint      : boolean;
                    opt_tabformat     : boolean;
                    opt_version       : boolean;
                    opt_help          : boolean;
                    opt_unicode       : boolean;
                    opt_quo           : tsp00_C1;
                    opt_point         : tsp00_C1;
                    opt_puser_args    : tsp4_argline);
                sp4co_sql_pc_runtime:
                    (opt_rtracefile   : tsp00_VFilename;
                    opt_rtrace        : tsp00_Int2;
                    opt_rprof         : tsp00_Int2;
                    opt_rmfetch       : tsp00_Int2;
                    opt_rtime_limit   : tsp00_Int2;
                    opt_rstmt_cnt     : tsp00_Int2;
                    opt_rfiller       : tsp00_Int2;
                    opt_rnotracetime  : boolean;
                    opt_rtraceswitch  : boolean;
                    opt_rnosdf        : boolean;
                    opt_rfiller1      : boolean;
                    opt_ruser_args    : tsp4_argline);
                sp4co_sql_easy,
                sp4co_sql_query,
                sp4co_sql_dialog,
                sp4co_sql_appl,
                sp4co_sql_load,
                sp4co_sql_util,
                sp4co_sql_param:
                    (opt_runfile      : tsp00_VFilename;
                    opt_object_owner  : tsp00_Name;
                    opt_object_name1  : tsp00_Name;
                    opt_object_name2  : tsp00_Name;
                    opt_parameter     : tsp00_Line;
                    opt_append        : boolean;
                    opt_prompt        : boolean;
                    opt_codeonly      : boolean;
                    opt_utility_session : boolean;
                    opt_comm_mode     : tsp4_command_mode;
                    opt_code          : tsp00_CodeType);
                sp4co_sql_audit:
                    (opt_aud_control  : tsp00_VFilename;
                    opt_aud_logfile   : tsp00_VFilename;
                    opt_aud_log_blocksize    : tsp00_Int4 ;
                    opt_aud_output    : tsp00_VFilename;
                    opt_aud_comm_mode : tsp4_command_mode);
                sp4co_sql_userx:
                    (opt_ux_runfile   : tsp00_VFilename;
                    opt_ux_comm_mode  : tsp4_command_mode );
                END;
            (*ENDCASE*) 
 
 
      tsp4_xuser_record    = RECORD
            xu_key         : tsp4_xuserkey;
            xu_fill        : tsp00_Int2;
            xu_servernode  : tsp00_NodeId;
            xu_serverdb    : tsp00_DbName;
            xu_user_61     : tsp00_C18;
            xu_password    : tsp00_CryptPw;
            xu_sqlmode     : tsp4_sqlmode_name;
            xu_cachelimit  : tsp00_Int4;
            xu_timeout     : tsp00_Int2;
            xu_isolation   : tsp00_Int2;
            xu_dblang_61   : tsp00_C18;
            xu_user        : tsp00_KnlIdentifier;
            xu_dblang      : tsp00_KnlIdentifier;
            xu_userUCS2    : tsp00_KnlIdentifier;
            xu_passwordUCS2: tsp00_CryptPw;
      END;
 
