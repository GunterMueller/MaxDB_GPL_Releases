
      tkb3_all_states = (
            sy_allocated,        (* dynpool and dyndata are allocated *)
            sy_bup_working,      (* label-I/O without reset glob data *)
            sy_configured,       (* config pages are copied to BD     *)
            sy_error,            (* an error is occurred              *)
            sy_initialized,      (* global data are initialized       *)
            sy_knockoffwork,     (* starting end phase                *)
            sy_filler6,
            sy_filler7);
      (* *)
      tkb3_system_state = PACKED SET OF tkb3_all_states;
      (* *)
      tkb3_answer_id = (
            ans_okay,
            ans_task_ready,
            ans_read_finish,
            ans_error,
            ans_tape_full);
      (* *)
      tkb3_savetasks = (
            task_nil,
            task_drt,
            task_twt,
            task_dwt,
            task_trt);
      (* *)
      tkb3_task_state   = (
            ts_none,          (* initialisation                       *)
            ts_started,       (* task started in HM (utility)         *)
            ts_running,       (* before entry in loop (child)         *)
            ts_stopped,       (* after abandonment from loop (child)  *)
            ts_waiting);      (* trt is in read wait                  *)
      (* *)
      tkb3_sre_state = (
            sres_free,
            sres_contains_pages,
            sres_reading_writing);
      (* *)
      tkb3_info_page_body =  PACKED ARRAY [1..mxkb3_info_page_body] OF char;
      (* *)
      tkb3_xparam_page_body = PACKED ARRAY [1..mxkb3_xparam_page_body] OF char;
 
      tkb3_info_stuff = RECORD
            inf_pno           : tsp00_PageNo;
            inf_pt            : tgg00_PageType;
            inf_pt2           : tgg00_PageType2;
            inf_check         : tgg00_PageCheckType;
            inf_filler1       : boolean;
            (* *)
            inf_mess_type     : tgg00_MessType;
            inf_mess2_type    : tgg00_MessType2;
            inf_volume_no     : tsp00_Int2;
            inf_swap_check    : tsp00_Int4;
            (* *)
            inf_file_version  : tsp00_Int4;
            inf_save_blocksize: tsp00_Int2;
            inf_max_volume_no : tsp00_Int2;
            (* *)
            inf_page_count    : tsp00_Int4;
            inf_bd_page_count : tsp00_Int4;
            (* *)
            inf_vol_page_cnt  : tsp00_Int4;
            inf_tapes_used    : tsp00_Int4;
            (* *)
            inf_curr_knlvers  : tsp00_Version;
            inf_knlvers       : tsp00_Version;
            inf_serverdb      : tsp00_DbName;
            inf_servernode    : tsp00_NodeId;
            inf_is_consistent : boolean;
            inf_is_clustered  : boolean;
            (* *)
            inf_first_iosequence : tsp00_Uint4; (* PTS 1120199 UH 2003-01-29 *)
            inf_last_iosequence  : tsp00_Uint4; (* PTS 1120199 UH 2003-01-29 *)
            (* *)
            inf_start_date    : tsp00_Date;
            inf_start_time    : tsp00_Time;
            inf_end_date      : tsp00_Date;
            inf_end_time      : tsp00_Time;
            (* *)
            inf_dbstamp1_date : tsp00_Date;
            inf_dbstamp1_time : tsp00_Time;
            inf_dbstamp2_date : tsp00_Date;
            inf_dbstamp2_time : tsp00_Time;
            (* *)
            inf_label         : tsp00_C14; (* PTS 1130971 mb 2004--08-21 *)
            inf_filler3       : tsp00_C2;  (* PTS 1130971 mb 2004--08-21 *)
            (* *)
            inf_db_ident      : tsp00_Line;  (* PTS 1000449 UH 19980909 *)
            (* *)
            inf_max_used_data_pno : tsp00_Int4; (* PTS 1105071 UH 17-01-2000 *)
            inf_conv_page_count   : tsp00_Int4;
      END;
 
 
      tkb3_info_page = RECORD
            CASE integer OF
                1:
                    (the_info       : tkb3_info_stuff);
                2:
                    (inf_header     : tgg00_PageHeader;
                    inf_body        : tkb3_info_page_body;
                    inf_checksum    : tsp00_Int4;
                    inf_trailer     : tgg00_PageTrailer);
                END;
            (*ENDCASE*) 
 
      tkb3_info_ptr = ^tkb3_info_page ;
 
      tkb3_xparam_page = RECORD
            CASE integer OF
                1:
                    (xp_pno         : tsp00_PageNo;
                    xp_pt           : tgg00_PageType;
                    xp_pt2          : tgg00_PageType2;
                    xp_check        : tgg00_PageCheckType;
                    xp_filler1      : boolean);
                2:
                    (xp_header      : tgg00_PageHeader;
                    (* *)
                    xp_length       : tsp00_Int4;
                    xp_total_length : tsp00_Int4;
                    (* *)
                    xp_data         : tkb3_xparam_page_body;
                    (* *)
                    xp_checksum     : tsp00_Int4;
                    xp_trailer      : tgg00_PageTrailer);
                END;
            (*ENDCASE*) 
 
 
      tkb3_task_desc = RECORD
            tsd_task_pid    : tsp00_TaskId ;
            tsd_filler2     : tsp00_Int2 ;
            tsd_tape_index  : tsp00_Int2 ;
            tsd_state       : tkb3_task_state ;
            tsd_task_kind   : tkb3_savetasks ;
            tsd_filler      : tsp00_C6 ;
      END ;
 
      tkb3_task_list     = ARRAY [1..ckb3_max_tasks] OF tkb3_task_desc;
      (* *)
      tkb3_task_list_ptr = ^tkb3_task_list;
 
      tkb3_tape_desc = RECORD
            tpd_is_open           : boolean;
            tpd_is_full           : boolean;
            tpd_volume_no         : tsp00_Int2;
            tpd_fno               : tsp00_Int4;
            (* *)
            tpd_max_pages         : tsp00_Int4;
            tpd_cnt_pages         : tsp00_Int4;
            (* *)
            tpd_xp_size           : tsp00_Int4;          (* [bytes] *)
            tpd_xp_read           : tsp00_Int4;          (* [bytes] *)
            (* *)
            tpd_xp_pages          : tsp00_Int4;
            tpd_type              : tsp00_VfType;
            tpd_check_destructive : boolean;     (* PTS 1000360 UH *)
            tpd_mirror_index      : tsp00_Int2;
            (* *)
            tpd_total_cnt_pages   : tsp00_Int4;          (* norewind   *)
            tpd_err               : tgg00_BasisError;    (* autos show *)
            tpd_is_replaced       : boolean;
            tpd_is_clustered      : boolean;
            (* *)
            tpd_name              : tsp00_VFilename;
            (* *)
            tpd_errtext           : tsp00_ErrText;
      END;
 
      tkb3_tape_list     = ARRAY [1..MAX_TAPES_GG00] OF tkb3_tape_desc;
      (* *)
      tkb3_tape_list_ptr = ^tkb3_tape_list;
 
      (* volume description: array of used volumes without deconfigured volumes
         dvd_volno references real volume number *)
      tkb3_devsp_desc = RECORD
            dvd_is_open   : boolean;
            dvd_filler1   : boolean;
            dvd_volno     : tsp00_Int2; 
            dvd_fno       : tsp00_Int4;
      END;
 
      tkb3_devsp_list = ARRAY [1..ckb3_max_data_volumes]
            OF tkb3_devsp_desc;

      (* *)
      tkb3_devsp_list_ptr  = ^tkb3_devsp_list;

      (* mapping from real volume number to volume index of tkb3_devsp_list *)
      tkb3_volume_index = ARRAY [1..ckb3_max_data_volumes]
            OF tsp00_Int2;

      (* *)
      tkb3_volume_index_ptr = ^tkb3_volume_index;

      (* *)
      tkb3_bool_list       = ARRAY [1..ckb3_max_bool_list] OF boolean;
      (* *)
      tkb3_into_queue_list = ARRAY [1..MAX_TAPES_GG00] OF tsp00_Int4;
 
      tkb3_page = RECORD
            CASE integer OF
                1:
                    (the_page  : tsp00_Page);
                2:
                    (the_pno   : tsp00_PageNo;
                    the_pt     : tgg00_PageType;
                    the_pt2    : tgg00_PageType2;
                    the_check  : tgg00_PageCheckType);
                3:
                    (info_page : tkb3_info_page);
                4:
                    (xp_page   : tkb3_xparam_page);
                5:
                    (univ_page : tkb00_Page);
                END;
            (*ENDCASE*) 
 
      tkb3_page_ptr  = ^tkb3_page;
      tkb3_block     = ARRAY [1..1000] OF tkb3_page;
      (* *)
      tkb3_block_ptr = ^tkb3_block;
 
      tkb3_srr_elem = RECORD             (* One element of srr queue *)
            sre_state           : tkb3_sre_state;
            sre_is_clustered    : boolean;
            sre_pages_per_block : tsp00_Int2;
            sre_next            : tsp00_Int2;
            sre_prev            : tsp00_Int2;
            (* *)
            sre_task_no_into    : tsp00_Int2;
            sre_filler2         : tsp00_Int2;
            sre_filler3         : tsp00_Int4;
            (* *)
            CASE integer OF
                1:
                    (sre_block      : tkb3_block_ptr);
                2:
                    (sre_addr_space : tsp00_C8);
                END;
            (*ENDCASE*) 
 
      tkb3_srr_array = ARRAY [0..1000] OF tkb3_srr_elem;
      (*   element '0' is for writing info, config and restart pages *)
      (* *)
      tkb3_srr_ptr   = ^tkb3_srr_array;
 
      tkb3_remote_channel = RECORD
            kb3rc_size    : tsp00_Int4;
            kb3rc_length  : tsp00_Int4;
            (* *)
            kb3rc_pid     : tsp00_TaskId;
            kb3rc_filler1 : tsp00_TaskId;
            (* *)
            CASE boolean OF
                true:
                    (kb3rc_buffer  : tsp00_MoveObjPtr);
                false:
                    (kb3rc_filler2 : tsp00_C8);
                END;
            (*ENDCASE*) 
 
      tkb3_remote_channel_ptr = ^tkb3_remote_channel;
      tkb3_remote_io = ARRAY [1..MAX_TAPES_GG00] OF tkb3_remote_channel;
 
      tkb3_save_static = RECORD
            sta_pages_transferred  : tsp00_Int4;
            sta_into_count         : tsp00_Int4; (* sum read blocks     *)
            (* *)
            sta_out_count          : tsp00_Int4; (* sum write blocks    *)
            sta_queue_into_count   : tsp00_Int4; (* sum read waits      *)
            (* *)
            sta_queue_out_count    : tsp00_Int4; (* sum write waits     *)
            sta_blocksize          : tsp00_Int2; (* pages per block     *)
            sta_max_tasks          : tsp00_Int2; (* task  list size     *)
            (* *)
            sta_num_devsp          : tsp00_Int2; (* no of used devspaces*)
            sta_devsp_ready_cnt    : tsp00_Int2; (* no of ready devsp   *)
            sta_num_tapes          : tsp00_Int2; (* no of tape devices  *)
            sta_tapes_ready_cnt    : tsp00_Int2; (* restore only        *)
            (* *)
            sta_tapes_start_cnt    : tsp00_Int2; (* restore only        *)
            sta_conttape           : tsp00_Int2; (* continue tape index *)
            sta_task_for_conttape  : tsp00_Int2; (* continue task index *)
            sta_queue_size         : tsp00_Int2; (* no of queue elements*)
            (* *)
            sta_queue_first_free   : tsp00_Int2;
            sta_queue_first_read   : tsp00_Int2;
            sta_queue_last_read    : tsp00_Int2;
            sta_read_tasks_waiting : tsp00_Int2;
            (* *)
            sta_write_tasks_waiting: tsp00_Int2;
            sta_system_state       : tkb3_system_state;
            sta_is_auto_load       : boolean;
            sta_is_cold            : boolean;  (* save data/pages cold *)
            sta_msgtype            : tgg00_MessType;   (* save/restore *)
            sta_msg2type           : tgg00_MessType2;
            sta_vtr_direction      : tgg00_VtraceType; (*sav/res vtrace*)
            (* *)
            sta_until_date         : tsp00_Date;       (*restore until *)
            sta_until_time         : tsp00_Time;       (*restore until *)
            (* *)
            sta_until_iosequence   : tsp00_Uint4;     (*restore until *)
            sta_region             : tsp00_Int4;
            (* *)
            sta_wait_pid           : tsp00_TaskId;     (* autosave     *)
            sta_try_again          : boolean;          (* autosave     *)
            sta_check_save         : boolean;
            sta_write_bitmap_pages : boolean;          (* save-pages   *)
            sta_write_conv_pages   : boolean;          (* savedata/archive  *)
            (* *)
            sta_file_version       : tsp00_Int4;       (* autosave     *)
            sta_locked_queue_index : tsp00_Int2;       (* restore log  *)
            sta_is_remote          : boolean;
            sta_lwt_perform_redo   : boolean;          (* restore log  *)
            (* *)
            sta_last_lpno_wanted   : tsp00_PageNo;     (* restore log  *)
            sta_trt_eof_wait_pid   : tsp00_TaskId;     (* restore log  *)
            (* *)
            sta_media_name         : tgg00_MediaName;
            (* *)
            sta_tape_label         : tsp00_C14;        (* PTS 1128100 mb 2004-03-01 *)
            sta_no_release_log     : boolean;          (* PTS 1128703 mb 2004-05-05 *)
            sta_filler1            : boolean;
            (* *)
            sta_coordinator_taskid : tsp00_TaskId;     (* PTS 1134753 UH 2005-04-07 *)
            sta_filler2            : tsp00_Int4;       (* PTS 1134753 UH 2005-04-07 *)
            (* *)
            sta_utilcmd_id         : tgg00_UtilCmdId; (* PTS 1108623 UH 2000-12-11 *)
            (* *)
            sta_volume_started     : tkb3_bool_list;
            (* *)
            sta_trt_into_queue     : tkb3_into_queue_list;
            (* *)
            sta_remote             : tkb3_remote_io;
            (* *)
            sta_info               : tkb3_info_stuff;
            (* *)
            (* sta_task            : tkb3_task_list_ptr    = 8 bytes *)
            (* sta_tape            : tkb3_tape_list_ptr    = 8 bytes *)
            (* sta_devsp           : tkb3_devsp_list_ptr   = 8 bytes *)
            (* sta_vol_idx         : tkb3_volume_index_ptr = 8 bytes *)
            (* sta_data_rst_rec    : tkb00_PagePtr         = 8 bytes *)
            (* sta_queue           : tkb3_srr_ptr          = 8 bytes *)
            (* sta_to_cancel       : ^boolean              = 8 bytes *)
            (* sta_backup_server   : tsp00_Addr            = 8 bytes *)
            CASE integer OF
                1:
                    (sta_task       : tkb3_task_list_ptr);
                2:
                    (sta_adr_space1 : tsp00_C8;
                    sta_tape        : tkb3_tape_list_ptr);
                3:
                    (sta_adr_space2 : tsp00_C8;
                    sta_addr_space3 : tsp00_C8;
                    sta_devsp       : tkb3_devsp_list_ptr);
                4:
                    (sta_adr_space4 : tsp00_C8;
                    sta_addr_space5 : tsp00_C8;
                    sta_addr_space6 : tsp00_C8;
                    sta_data_rst_rec: tkb00_PagePtr);
                5:
                    (sta_adr_space7 : tsp00_C8;
                    sta_addr_space8 : tsp00_C8;
                    sta_addr_space9 : tsp00_C8;
                    sta_addr_spac10 : tsp00_C8;
                    sta_queue       : tkb3_srr_ptr);
                6:
                    (sta_adr_spac11 : tsp00_C8;
                    sta_addr_spac12 : tsp00_C8;
                    sta_addr_spac13 : tsp00_C8;
                    sta_addr_spac14 : tsp00_C8;
                    sta_addr_spac15 : tsp00_C8;
                    sta_to_cancel   : tsp00_BoolAddr);
                7:
                    (sta_adr_spac16   : tsp00_C8;
                    sta_addr_spac17   : tsp00_C8;
                    sta_addr_spac18   : tsp00_C8;
                    sta_addr_spac19   : tsp00_C8;
                    sta_addr_spac20   : tsp00_C8;
                    sta_addr_spac21   : tsp00_C8;
                    sta_backup_server : tsp00_Addr);
                8:
                    (sta_adr_spac22 : tsp00_C8;
                    sta_addr_spac23 : tsp00_C8;
                    sta_addr_spac24 : tsp00_C8;
                    sta_addr_spac26 : tsp00_C8;
                    sta_addr_spac27 : tsp00_C8;
                    sta_addr_spac28 : tsp00_C8;
                    sta_addr_spac29 : tsp00_C8;
                    sta_vol_idx     : tkb3_volume_index_ptr);
                9:
                    (sta_adr_spac30 : tsp00_C8;
                    sta_addr_spac31 : tsp00_C8;
                    sta_addr_spac32 : tsp00_C8;
                    sta_addr_spac33 : tsp00_C8;
                    sta_addr_spac34 : tsp00_C8;
                    sta_addr_spac35 : tsp00_C8;
                    sta_addr_spac36 : tsp00_C8;
                    sta_addr_spac37 : tsp00_C8);

                END;
            (*ENDCASE*) 
 
      tkb3_static_array = ARRAY [1..ckb3_max_statics] OF tkb3_save_static;
      tkb3_save_static_ptr = ^tkb3_save_static;
 
      tkb3_backup_result = RECORD
            bdef_date              : tsp00_C1;
            bcol_date              : tsp00_Date;
            bcol_extdate_filler1   : tsp00_C2;
            bdef_time              : tsp00_C1;
            bcol_time              : tsp00_Time;
            bdef_serverdb          : tsp00_C1;
            bcol_serverdb          : tsp00_DbName;
            bdef_servernode        : tsp00_C1;
            bcol_servernode        : tsp00_NodeId;
            bdef_kernel_version    : tsp00_C1;
            bcol_kernel_version    : tsp00_Version;
            bcol_pages_transferred : tsp00_ResNum; (* incl. def-byte *)
            bcol_pages_left        : tsp00_ResNum; (* incl. def-byte *)
            bcol_no_of_volumes     : tsp00_ResNum; (* incl. def-byte *)
            bdef_media_name        : tsp00_C1;
            bcol_media_name        : tsp00_C64;
            bdef_tape_name         : tsp00_C1;
            bcol_tape_name         : tsp00_VFilename;
            bdef_tape_errortext    : tsp00_C1;
            bcol_tape_errortext    : tsp00_ErrText;
            bdef_tape_label        : tsp00_C1;
            bcol_tape_label        : tsp00_C14;
            bdef_is_consistent     : tsp00_C1;
            bcol_is_consistent     : char;
            bcol_first_iosequence  : tsp00_ResNum; (* incl. def-byte *)
            bcol_last_iosequence   : tsp00_ResNum; (* incl. def-byte *)
            bdef_dbstamp1_date     : tsp00_C1;
            bcol_dbstamp1_date     : tsp00_Date;
            bcol_extdate_filler2   : tsp00_C2;
            bdef_dbstamp1_time     : tsp00_C1;
            bcol_dbstamp1_time     : tsp00_Time;
            bdef_dbstamp2_date     : tsp00_C1;
            bcol_dbstamp2_date     : tsp00_Date;
            bcol_extdate_filler3   : tsp00_C2;
            bdef_dbstamp2_time     : tsp00_C1;
            bcol_dbstamp2_time     : tsp00_Time;
            bcol_bd_page_count     : tsp00_ResNum; (* incl. def-byte *)
            bcol_tapes_used        : tsp00_ResNum; (* incl. def-byte *)
            bdef_db_ident          : tsp00_C1;       (* PTS 1000449 UH 19980909 *)
            bcol_db_ident          : tsp00_Line;     (* PTS 1000449 UH 19980909 *)
            bcol_max_used_data_pno : tsp00_ResNum; (* PTS 1105071 UH 17-01-2000 *)
            bcol_conv_page_count   : tsp00_ResNum; (* PTS 1129689 TS 2004-05-18 *)
      END;
 
 
      tkb3_univ_addr = RECORD
            CASE integer OF
                1:
                    (obj   : tsp00_ObjAddr      );
                2:
                    (task  : tkb3_task_list_ptr );
                3:
                    (tape  : tkb3_tape_list_ptr );
                4:
                    (devsp : tkb3_devsp_list_ptr);
                5:
                    (volidx: tkb3_volume_index_ptr);
                6:
                    (srr   : tkb3_srr_ptr       );
                7:
                    (buf   : tsp00_PageAddr     );
                8:
                    (block : tkb3_block_ptr     );
                9:
                    (log   : tkb00_PagePtr      );
                END;
 
