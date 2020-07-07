
&     ifdef __cplusplus
      (*inlineC #define ci2_gg00    asPasStr () *)
      (*inlineC #define ci4_gg00    asPasStr () *)
      (*inlineC #define ci6_gg00    asPasStr () *)
      (*inlineC #define ci8_gg00    asPasStr () *)
      (*inlineC typedef void *tgg00_VoidPtr; *)
      tgg00_stopper = integer;
&     else
      tgg00_stopper = integer;
      (* gg90 substitution used by C and PASCAL  *)
      (**)
      tgg00_VoidPtr = tsp00_Addr;    (* PTS 1108234 JA 2000-11-08 *)
 
      tgg90_Cint2 = RECORD
            ci2_gg00 : tsp00_C2
      END;
 
 
      tgg90_Cint4 = RECORD
            ci4_gg00 : tsp00_C4
      END;
 
 
      tgg90_Cint6 = RECORD
            ci6_gg00 : tsp00_C6
      END;
 
 
      tgg90_Cint8 = RECORD
            ci8_gg00 : tsp00_C8
      END;
 
      (* gg91 substitution used by C and PASCAL  *)
      tgg91_FileVersion = tgg90_Cint2;
      tgg91_ObjAnyBody  = tsp00_C1;
      tgg91_PageRef     = tgg90_Cint6;
      tgg91_SessionNo   = tgg90_Cint4;
      tgg91_TransNo     = tgg90_Cint6;
&     endif
      (* *)
      tgg00_SubtransNo  = tsp00_Uint4;
      (* *)
      tgg00_ConverterVersion = tsp00_Int4;
      (* *)
      tgg00_UniquePrimId = tsp00_C4;
      (* *)
      tgg00_AggSet = SET OF (
            use_result_id,
            prim_keys);
      (* *)
      tgg00_BdInvRange = (
            secondary_stop,
            primary_start,
            primary_stop,
            incl_first);
      (* *)
      tgg00_BdUseSet = SET OF (
            bd_release_acc,
            bd_write_acc,
            bd_load_nolog,
            bd_IgnoreSetToBad,
            bd_fill4,
            bd_fill5,
            bd_fill6,
            bd_fill7);
      (* *)
      tgg00_DateTimeFormat = (
            dtf_none,
            dtf_normal,
            dtf_iso,
            dtf_usa,
            dtf_eur,
            dtf_jis,
            dtf_oracle_date,
            dtf_was_ansi_now_is_same_as_iso,   (* PTS 1112472 E.Z. *)
            dtf_ts_eur,      (* for debis *)
            dtf_future);     (* for future use. Do NOT allow more  *)
      (*                        dtf_ variants because of vtcontext *)
      tgg00_Debug = (
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
            bd_idx_create);
      (* *)
      tgg00_DiagType = (
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
            diagIsStateAdmin_egg00);
      (* *)
      (* PTS 1003033 E.Z. *)
      tgg00_DirectoryMonitorOp = (
            dir_hit,
            dir_miss,
            func_count);
      (* *)
      tgg00_DumpStateSet = SET OF (
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
            dumpFileDir_egg00);
      (* *)
      tgg00_FiletypeSet = SET OF (
            ftsTemp_egg00,
            ftsPerm_egg00,
            ftsByteStr_egg00,
            ftsConcurrent_egg00, (* parallel temp/perm access [locklist] *)
            ftsShared_egg00,     (* temp access for different tasks [cache]*)
            ftsDynamic_egg00,
            ftsObject_egg00,
            ftsArchive_egg00);   (* PTS 1117381 E.Z. *)
      (* *)
      tgg00_ExtendedFiletypeSet = SET OF (
            eftsClustered_egg00,
            eftsRecordsPacked_egg00);
      (* *)
      tgg00_HandlingSet = PACKED SET OF (
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
            hsIgnoreLock_egg00, (* PTS 1108174 *)
            hsFill14_egg00,
            hsFill15_egg00);
      (* *)
      tgg00_IndexMode  = (
            create_index,
            restart_index,
            test_index);
      (* *)
      tgg00_InvListKind = (
            ilkArray_egg00,
            ilkSubTree_egg00);
      (* *)
      tgg00_Linkset = SET OF (
            is_primary_table,
            is_secondary_table,
            del_trigger,
            ins_trigger,
            upd_trigger,
            unique_pk_table,
            internal_trigger);
      (* *)
      tgg00_LockReqMode = (
            lckFree_egg00,
            lckSysShare_egg00,
            lckSysExcl_egg00,
            lckTabShare_egg00,
            lckTabExcl_egg00,
            lckRowShare_egg00,
            lckRowExcl_egg00,
            lckRowOptimistic_egg00);
      (* *)
      tgg00_LockReqState = SET OF (
            lrsConsistent_egg00,
            lrsEotExcl_egg00,
            lrsTemp_egg00,
            lrsForInsert_egg00,
            lrsIgnoreInsert_egg00);
      (* *)
      tgg00_MessBufStruct = (
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
            mbs_verify);
      (* *)
      tgg00_MessType = (
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
            m_unused_was_copy_tempfile, (* PTS 1114833 UJ *)
            m_create_file,
            m_create_index,
            m_unused_was_create_serverdb, (* PTS 1114833 UJ *)
            m_create_table,
            m_delete,
            m_unused_was_delete_trans, (* PTS 1114833 UJ *)
            m_describe,
            m_destroy_temp,
            m_diagnose,
            m_down,
            m_drop,
            m_unused_was_drop_serverdb, (* PTS 1114833 UJ *)
            m_end,
            m_eval,
            m_fetch,
            m_flush,
            m_get,
            m_usage,
            m_index_statistic,
            m_init_config,
            m_unused_was_inquire, (* PTS 1114833 UJ *)
            m_insert,
            m_insert_select,
            m_unused_was_insert_trans, (* PTS 1114833 UJ *)
            m_unused_was_integrate_node, (* PTS 1114833 UJ *)
            m_key,
            m_lock,
            m_mend,
            m_mfetch,
            m_move,
            m_unused_was_notify, (* PTS 1114833 UJ *)
            m_unused_was_logpos_list, (* PTS 1114833 UJ *)
            m_outcopy,
            m_unused_was_outcopy_catalog, (* PTS 1114833 UJ *)
            m_partial_rollback,
            m_unused_was_notusedanymore,   (*unused *) (* PTS 1107952 E.Z. *) (* PTS 1114833 UJ *)
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
            m_unused_was_unused_log, (* PTS 1114833 UJ *)
            m_up,
            m_unused_was_upd_statistic, (* PTS 1114833 UJ *)
            m_update,
            m_update_rec,
            m_verify,
            m_majority,
            m_unused_was_format, (* PTS 1114833 UJ *)
            m_unused_was_repli_savepoint, (* PTS 1114833 UJ *)
            m_nextval,
            m_history,
            m_sharedSQLParseid,
            m_procedure,
            m_new,
            m_unused_was_create_obj_hash, (* PTS 1114833 UJ*)
            m_get_object,
            m_debug,                        (* PTS 1122726 *)
            m_save_parallel,
            m_restore_parallel,
            m_autosave,
            m_unused_was_open_trans, (* PTS 1114833 UJ *)
            m_create_index_parallel,
            m_migrate_obj_files,                    (* PTS 471100 FF *)
            m_afterCommit,                          (* PTS 1131350 MS *)
            m_afterRollback,                        (* PTS 1131350 MS *)
            m_innerTransBegin,                      (* PTS 1139523 *)
            m_innerTransCommit,                     (* PTS 1139523 *)
            m_innerTransRollback);                  (* PTS 1139523 *)
      (* *)
      tgg00_MessType2 = (
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
            mm_unused_was_database_clear_log, (* PTS 1114833 UJ *)
            mm_database_noformat,
            mm_unused_was_dbname, (* PTS 1114833 UJ *)
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
            mm_free3,    (* was mm_fnull PTS 1120287 E.Z. *)
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
            mm_standby,     (* PTS 1111229 E.Z., was mm_quick *)
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
            mm_unused_was_write_reject, (* PTS 1114833 UJ *)
            mm_trigger,
            mm_ignore_duplicates,
            mm_update_duplicates,
            mm_unused_was_fetch_request, (* PTS 1114833 UJ *)
            mm_unused_was_allocate, (* PTS 1114833 UJ *)
            mm_unused_was_bind_req, (* PTS 1114833 UJ *)
            mm_unused_was_exc_req, (* PTS 1114833 UJ *)
            mm_unused_was_deallocate, (* PTS 1114833 UJ *)
            mm_unused_was_reply_cont, (* PTS 1114833 UJ *)
            mm_unused_was_reply_end, (* PTS 1114833 UJ *)
            mm_unused_was_not_used_was_first_distributed, (* PTS 1114833 UJ *)
            mm_unused_was_next_reply, (* PTS 1114833 UJ *)
            mm_acknowledge,
            mm_newtape,
            mm_ignore,
            mm_abort,
            mm_object,
            mm_free4,        (* unused, was mm_qual_no_opt *)
            mm_new_write,
            mm_new_read,
            mm_parallel,
            mm_checkpoint,
            mm_unused_was_optimize_longvar,   (* PTS 1106798 JA 2000-05-30 *) (* PTS 1114833 UJ *)
            mm_last_rowno);
      (* *)
      tgg00_MiscSet = SET OF (
            mi_upd_cols,
            mi_dt_iso,
            mi_dt_usa,
            mi_dt_eur,
            mi_dt_jis,
            mi_dt_oracle_date,
            mi_internal);
      (* *)
      tgg00_ObjState = (
            obsFree_egg00,
            obsFreeAfterEot_egg00,
            obsReserved_egg00,
            obsKeyReserved_egg00,
            obsOccupied_egg00,
            obsDelBeforeImage_egg00,
            obsUpdBeforeImage_egg00);
      (* *)
      tgg00_MonitorPart = (
            mpAll_egg00,
            mpCaches_egg00,
            mpLoad_egg00,
            mpLog_egg00,
            mpLong_egg00,
            mpPages_egg00,
            mpRow_egg00,
            mpTransaction_egg00);
      (* *)
      tgg00_PageCheckType = (
            chckNil_egg00,
            chckWriteCnt_egg00,
            chckChecksumData_egg00,
            chckBottomLog_egg00,
            chckChecksumLog_egg00,
            chckChecksumLogInfo_egg00);
      (* *)
      tgg00_PageMode = SET OF (
            pmTemp_egg00,
            pmCatalog_egg00,
            pmFdir_egg00,
            pmArchive_egg00,
            pmHasHistory_egg00,
            pmHasHistoryOnPage_egg00,
            pmFill6_egg00,
            pmFill7_egg00);
      (* *)
      tgg00_PageType = ( (* see also Data_PageBasic.hpp *)
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
            ptFileDir_egg00,             (* 25.08.03, M.Ki. *)
            ptRestart_egg00,             (* UH 2004-05-18 *)
            ptBasic_egg00,               (* UH 2004-05-18 *)
            ptVarSpace_egg00,            (* UH 2004-05-18 *)
            ptPrimaryFile_egg00,         (* UH 2004-05-18 *)
            ptPrimaryFileOverflow_egg00, (* UH 2004-05-18 *)
            ptPrimaryFileBlob_egg00,     (* UH 2004-05-18 *)
            ptPrimaryFileLocks_egg00,    (* UH 2004-05-18 *)
            ptLastPageType_egg00);
      (* *)
      tgg00_PageType2 = (
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
            pt2OpenTrans_egg00);
      (* *)
      tgg00_QualKind = (
            primary_only,
            inv_only,
            inv_and_primary);
      (* *)
      tgg00_RecoveryMode = (
            rmNone_egg00,
            rmTemp_egg00,
            rmPerm_egg00,
            rmStatic_egg00);
      (* *)
      (* PTS 1003033 E.Z. *)
      (* PTS 1111134 E.Z. *)
      tgg00_RefInfoIndex = (
            (*==== show monitor caches ====*)
            idata_cache_total,
            idata_cache_hit,
            idata_cache_miss,
            idata_cache_percent,
            (* PTS 1115020 E.Z. *)
            iak_cache_total,
            iak_cache_hit,
            iak_cache_miss,
            iak_cache_percent,
            isseq_cache_total,
            isseq_cache_hit,
            isseq_cache_miss,
            isseq_cache_percent,
            (* PTS 1109469, T.A. 2001-03-22 *)
            idata_sql_cache_total,
            idata_sql_cache_hit,
            idata_sql_cache_miss,
            idata_sql_cache_percent,
            (* PTS 1115043 E.Z. *)
            idata_obj_cache_total,
            idata_obj_cache_hit,
            idata_obj_cache_miss,
            idata_obj_cache_percent,
            idata_obj_log_cache_total,
            idata_obj_log_cache_hit,
            idata_obj_log_cache_miss,
            idata_obj_log_cache_percent,
            (* PTS 1106423 TS 2000-04-20 *)
            (* PTS 1111572 E.Z. *)
            idata_history_cache_total,
            idata_history_cache_hit,
            idata_history_cache_miss,
            idata_history_cache_percent,
            (* PTS 1106423 *)
            (*==== show monitor load ====*)
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
            (*==== show monitor lock ====*)
            iavg_lock_used,
            imax_lock_used,
            icollisions,
            iescalations,
            inew_rows,
            inew_tabs,
            ideadlocks,
            irequest_timeouts,
            (*==== show monitor log ====*)
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
            (*==== show monitor long ====*)
            iread_string,
            iwrite_string,
            (*==== show monitor pages ====*)
            ir_v_acc,
            iw_v_acc,
            ir_p_acc,
            iw_p_acc,
            icatalog_r_v,
            icatalog_w_v,
            icatalog_r_p,
            icatalog_w_p,
            (* PTS 1115020 E.Z. *)
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
            (*==== show monitor row ====*)
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
            (*==== show monitor transaction ====*)
            ifunccount,
            itranscount,
            ikb_calls,
            (*==== unused but needed because of loop in b73initref ====*)
            iunused_but_needed);
      (* *)
      tgg00_RegionType = (
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
            rgnOMS_egg00);
      (* *)
      tgg00_ServerdbType = (
            sdb_notused_gateway,  (*  sqlm_ansi     *)
            sdb_db2_gateway,      (*  sqlm_db2      *)
            sdb_oracle_gateway,   (*  sqlm_oracle   *)
            sdb_internal,         (*  sqlm_internal *)
            sdb_remote,
            sdb_odbc_gateway,
            sdb_nil);
      (* *)
      tgg00_ShowKind = (
            sh_no_kind,
            sh_bad_index,
            sh_column,
            sh_connect,
            sh_connect_param,
            sh_constraint,
            sh_cons_columns,
            sh_domain_data,
            sh_snapshot,
            sh_dbm_state,                 (* was sh_packages *)
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
            sh_filedirectories,           (* reused M.Ki *)
            sh_ref_column,
            sh_serverdb,
            sh_servertasks,               (* reused UH *)
            sh_st_config,
            sh_st_database,
            sh_loginformation,            (* reused EZ *)
            sh_st_index,
            sh_readerwriterlockinformation,(* reused RW, was sh_st_log *)
            sh_kernelmessages,
            sh_st_monitor,
            sh_st_state,
            sh_st_table,
            sh_readerwriterlockwaitingtasks,(* reused RW, was sh_st_termset *)
            sh_st_user,
            sh_synonym,
            show_fill11,                  (* unused, was sh_sysspace *)
            show_fill12,                  (* unused, was sh_sysdba *)
            sh_table,
            sh_taskloadbalancinginformation,     (* was sh_tabledef, reused: R.R. *)
            sh_taskloadbalancingtaskgroupstates, (* was sh_tablelist, reused: R.R. *)
            sh_taskloadbalancingtaskmoves,       (* was sh_user_current, reused: R.R. *)
            sh_table_stat,
            show_fill14a,                 (* unused, was sh_termset *)
            show_fill14c,                 (* unused, was sh_transfile (sysdd.treelocks) *)
            sh_trigger,
            show_fill15,                  (* unused, was sh_trigger_trace *)
            sh_u_domain,
            sh_multiVersionReadStats,     (* unused, was sh_user *)
            show_fill16,                  (* unused, was sh_values *)
            sh_version_old,
            sh_view,
            sh_all_column,
            sh_all_constraint,
            sh_all_cons_columns,
            sh_all_foreign_key,
            sh_all_index,
            sh_all_priv_granted,
            sf_fill1                      (* unused, was sh_all_priv_on *),
            sh_all_procedures,
            sh_all_sequences,
            sh_all_synonyms,
            sh_all_table,
            sh_all_triggers,
            sh_all_user,
            sh_all_views,
            sh_all_ref_columns,
            sh_info_gc_stats,             (* reused: PTS 1113774 F.F. *)
            sh_sysmon_connection,         (* reused: PTS 1114549 E.Z. *)
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
            sh_backtrace,                 (* 2004-01-26 T.A. *)
            sh_odbc_st_index,
            sh_odbc_st_table,
            sh_logqueuestatistics,            (* reused, PTS 1132161 mb *)
            sh_logqueuestatisticsreset,       (* reused, PTS 1132161 mb *)
            show_fill4,                   (* unused, was sh_gatewayview *)
            sh_object,
            sh_objectlocks,
            sh_all_object,
            show_fill2,                   (* unused, was sh_autosave *)
            sh_pages,
            sh_roots,
            sh_session_roles,
            sh_userroles,
            sh_all_roles,
            sh_odbc_columns,
            sh_show_view,
            sh_methods,
            show_fill2a,                   (* unused, was sh_method_params *)
            sh_oms_versions,
            sh_ctrl_configuration,
            sh_ctrl_devspaces,
            sh_ctrl_io_statistics,
            sh_ctrl_log,
            show_fill17,                   (* unused, was sh_ctrl_cache PTS 1120445 D.T. *)
            sh_oms_containers,
            sh_ctrl_locks,
            sh_indexpages,
            show_fill3a,                  (* unused, was sh_treelocks *)
            sh_files_old,
            sh_running_cmds,
            sh_schema,
            (* PTS 1105189 E.Z. *)
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
            (* END PTS 1105189 E.Z. *)
            sh_consistent_view,
            sh_history_info,
            sh_oms_memory,
            sh_parsinfos,
            sh_internal_state,  (* PTS 1107717 E.Z. *)
            sh_fill21,          (* unused, was sh_allocatorstatistic *)
            sh_spinlock,        (* PTS 1109404 E.Z. *)
            sh_oms_lock,        (* PTS 1110315 *)
            sh_heap_content,
            sh_odbc_index,      (* PTS 1114119 T.A. *)
            sh_container_roots,  (* PTS 1115641 F.F *)
            sh_container_chains, (* PTS 1115641 F.F *)
            sh_container_keys,   (* PTS 1115641 F.F *)
            sh_activeconfiguration,        (* BEGIN PTS 1115043 E.Z. *)
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
            sh_fill2,              (* unused, was sh_events *)
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
            sh_version,                     (* END PTS 1115043 E.Z. *)
            sh_commandcachestatistics,        (* PTS 1120445 D.T. *)
            sh_commandcachestatisticsreset,   (* PTS 1120445 D.T. *)
            sh_commandstatistics,             (* PTS 1120445 D.T. *)
            sh_commandstatisticsreset,        (* PTS 1120445 D.T. *)
            sh_machineconfiguration,
            sh_machineutilization,
            sh_spinlockpoolstatistics,
            sh_spinlockpoolstatisticsreset,
            sh_readerwriterlockstatistics,
            sh_readerwriterlockstatisticsreset,
            sh_omsdiagnose, (* PTS 1127508 *)
            sh_variables,
            sh_unloadedstatements,
            sh_fill3 (* end marker, do not insert anything behind *)
            );
      (* *)
      tgg00_SrVtrace2Type = (
            kb38run_save,
            kb38end_save,
            kb38blocksize,
            kb38que_state,
            kb38rte_errtext,
            kb38hm_wait);
      (* *)
      tgg00_StackEntryType = (
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
            st_column,     (* PTS 1116801 E.Z. *)
            st_object_key, (* ADIS 1001810, T.A. 15.12.1998 *)
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
            st_mass_compare, (* together with comparison_ops *)
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
            st_current_schema);
      (* *)
      tgg00_StackOpBuildIn = (
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
            op_b_exp,   (* The following functions don't need a    *)
            op_b_ln,    (* own stack-entry. They are here, because *)
            op_b_log,   (* tgg00_StackOpType is full.              *)
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
            op_b_dbyte_lfill, (* the following functions need *)
            op_b_dbyte_rfill, (* one stack_entry for its own needs *)
            op_b_dbyte_lpad,
            op_b_dbyte_rpad,
            op_b_dbyte_trim,
            op_b_dbyte_rtrim,
            op_b_dbyte_ltrim,
            op_b_uni_trans,
            op_b_float,
            op_b_case_start,
            op_b_case_stop,
            op_b_long_substr);
      (* *)
      tgg00_StackOpFunc = (
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
            op_f_dis_variance);
      (* *)
      tgg00_StackOpOutput = (
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
            op_o_output_column, (* PTS 1116801 E.Z. *)
            op_o_output_cluster_key);
      (* *)
      tgg00_StackOpType = (
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
            op_jmp_ins_upd,     (*st_jump_ins_upd*)
            op_length,
            op_lowcase,
            op_not_in,
            op_num,
            op_odd,
            op_order_asc,
            op_order_desc,
            op_order_desc_ascii,
            op_order_desc_ebcdic,
            op_outer_join,  (*op_outer_join --> op_o_output_outer_join*)
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
            op_hextoraw,       (* st_op *)
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
            op_adddate, (*begin date_time_op*)
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
            op_abs,     (*begin func_arith_op*)
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
            op_div,     (*begin arith_op*)
            op_intdiv,
            op_minus,
            op_minus_monad,
            op_mod,
            op_mod_func,
            op_mult,
            op_plus,
            op_filler12,
            op_filler13,
            op_is_integer, (* begin bool_op *)
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
            op_eq,      (*begin comp_op*)
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
            op_test_zero);
      (* *)
      tgg00_TableKind = (
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
            tunused1,            (* unused *)
            tunused2,            (* unused *)
            tshow_view,
            tcatalog_table,
            tunsed3,             (* unused *)
            tunsed4);            (* unused *)
      (* *)
      tgg00_Tfn = (
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
            tfnUndoLog_egg00,     (*Logging74 UH *)
            tfnRedoLog_egg00,     (*Logging74 UH *)
            tfnHistory_egg00,     (*Logging74 UH *)
            tfnOpenTrans_egg00,   (*Logging74 UH *)
            tfnShortScol_egg00,
            tfnLog_egg00,
            tfnHistoryDirectory_egg00, (*Logging74 UH *)
            tfn__FurtherUse21_egg00,
            tfnObj_egg00,
            tfnObjBefore_egg00,
            tfnContObj_egg00,
            tfn__FurtherUse25_egg00,
            tfnTemp_egg00,      (* 26 = x1A: used by cgg_errortext_fn *)
            tfnTempAux_egg00,   (* the lowest tfn to perform b01trelease_temp_cache *)
            tfn__FurtherUse28_egg00,
            tfnTempOms_egg00);
      (* *)
      tgg00_TfnLog = (
            tfnlNone_egg00,
            tfnlSavepoint_egg00,
            tfnlCreateIndex_egg00,
            tfnlRollback_egg00);
      (* *)
      tgg00_TfnTemp = (
            ttfnNone_egg00,
            ttfnComplexSelect_egg00,
            ttfnComplexView_egg00,
            ttfnFromSelect_egg00,
            ttfnUserResult_egg00,
            ttfnTempTable_egg00,
            (* the ttfn_.. above will be counted during monitoring *)
            (* ttfnTempTable_egg00 has to be the last one above        *)
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
            ttfnOmsVersionIndex_egg00);
      (* *)
      tgg00_TransState = PACKED SET OF (
            tsLocked_egg00,
            tsTempUpdated_egg00,
            tsSurrogateUpd_egg00,
            tsBdLocked_egg00,
            tsDbFullNowait_egg00,      (* PTS 1106067 JA 2000-03-21 *)
            tsRollback_egg00,
            tsIgnoreDBSecureLimit_egg00,
            tsFill07_egg00,            (* PTS 1107952 E.Z. *)
            tsFill08_egg00,
            tsFunctRollbSet_egg00,
            tsUpdated_egg00,
            tsAccessedInSession_egg00,
            tsErrTextExist_egg00,
            tsSubUpdated_egg00,
            tsFill14_egg00,
            tsFill15_egg00);
      (* *)
      tgg00_VtraceType = (
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
            b91repair, (* PTS 1111944 AK 10/10/2001    *)
            ak_returncode,
            vttOperatorJoin_egg00,
            vttAccessOperator_egg00,
            ak_strat_with_bool);
      (* *)
      tgg00_JoinSearchLevel = (
            jsLevel0_egg00,  (* automatic   *)
            jsLevel1_egg00,  (* greedy      *)
            jsLevel2_egg00,
            jsLevel3_egg00,
            jsLevel4_egg00,  (* heuristic   *)
            jsLevel5_egg00,
            jsLevel6_egg00,
            jsLevel7_egg00,
            jsLevel8_egg00,
            jsLevel9_egg00); (* permutation *)
      (* *)
      tgg00_AuxBuffers    = PACKED ARRAY [1..MAX_AUX_BUFFER_COUNT_GG00] OF tsp00_Page;
      tgg00_AuxBuffersPtr = ^tgg00_AuxBuffers;
      tgg00_BasisError    = tsp00_Int2;
      tgg00_BdInvSet      = SET OF tgg00_BdInvRange;
      tgg00_DoubleBuf     = PACKED ARRAY [1..DOUBLE_BUF_MXGG00] OF char;
      tgg00_Filename      = tsp00_C16;
      tgg00_Filetypes     = ARRAY [1..MAX_TAPES_GG00     ] OF tsp00_VfType;
      tgg00_HashPrimeList = ARRAY [1..MAX_HASH_PRIME_GG00] OF tsp00_Int4;
      tgg00_KeyLen        = tsp00_Int2;
      tgg00_KeylistBuf    = tsp00_Buf;
      tgg00_LogBody       = PACKED ARRAY [1..LOGPAGE_BODY_MXGG00] OF char;
      tgg00_MaxstackArr   = ARRAY [1..MAX_STACK_GG00] OF tsp00_Int4;
      tgg00_MediaName     = tsp00_C64;
      tgg00_ObjFileNo     = tsp00_Uint4;
      tgg00_ObjFrameVers  = tsp00_Uint1;
      tgg00_ObjPagePos    = tsp00_Uint2;
      tgg00_PageHeadStr   = PACKED ARRAY [1..PAGE_HEADER_MXGG00] OF char;
      tgg00_PagePos       = tsp00_Int4;
      tgg00_RecBody       = PACKED ARRAY [1..RECBODY_MXGG00] OF char;
      tgg00_RecPart       = PACKED ARRAY [1..RECPART_MXGG00] OF char;
      tgg00_RootList      = ARRAY [1..MAX_ROOTLIST_GG00] OF tsp00_PageNo;
      tgg00_RootListPtr   = ^tgg00_RootList;
      tgg00_ServerdbNo    = tsp00_C2;
      tgg00_SfpInitPart   = PACKED ARRAY [1..SFP_INIT_PART_MXGG00] OF char;
      tgg00_StackOpFuncSet= PACKED SET OF tgg00_StackOpFunc;
      tgg00_Surrogate     = tsp00_C8;
      tgg00_Syskey        = PACKED ARRAY [1..SYSKEY_MXGG00] OF char;
      tgg00_TapeCount     = ARRAY [1..MAX_TAPES_GG00] OF tsp00_Int4;
      tgg00_TapeNames     = ARRAY [1..MAX_TAPES_GG00] OF tsp00_VFilename;
      tgg00_TransIndex    = tsp00_Int2;
      tgg00_FileIdHead    = PACKED ARRAY [1..FILE_ID_HEAD_MXGG00] OF char;
      tgg00_UserId        = tsp00_C4;
      tgg00_UserRef       = tgg91_PageRef;
      tgg00_VtraceKey     = tsp00_C60;
 
      tgg00_UtilCmdId = RECORD       (* PTS 1108625 UH 2000-12-11 *)
            utidId_gg00     : tsp00_C12;
            utidLineNo_gg00 : tsp00_Int4;
      END;
 
 
      tgg00_OidSpace = RECORD
            CASE integer OF
                1:
                    (oidFirstI4_gg00 : tsp00_Int4;
                    oidSecondI4_gg00 : tsp00_Int4);
                2:
                    (oidC8_gg00 : tsp00_C8);
                END;
            (*ENDCASE*) 
 
 
      tgg00_MonitorRec = RECORD
            mon_parseid        : tsp00_C12;
            mon_masterparseid  : tsp00_C12;
            mon_cmdid          : tgg00_Surrogate;
            mon_strat          : tsp00_C40;
            mon_copy_result    : tsp00_C3;
            mon_is_select      : boolean;
            mon_rows_read      : tsp00_Int4;
            mon_rows_qual      : tsp00_Int4;
            mon_rows_fetched   : tsp00_Int4;
            mon_fetch_calls    : tsp00_Int4;
            mon_virt_reads     : tsp00_Int4;
            mon_kb_calls       : tsp00_Int4;
            mon_waits          : tsp00_Int4;
            mon_suspends       : tsp00_Int4;
            mon_phys_ios       : tsp00_Int4;
            mon_sec            : tsp00_Int4;
            mon_microsec       : tsp00_Int4;
            mon_strat_len      : tsp00_Int2;
            mon_rootlist_elems : tsp00_Int2;
            mon_rootlist       : tgg00_RootList;
      END;
 
 
      tgg00_UserMonitorInfo = RECORD
            umi_curr_monitor    : tgg00_MonitorRec;
            umi_sum_monitor     : tgg00_MonitorRec;
            umi_username        : tsp00_KnlIdentifier;
            umi_termid          : tsp00_TermId;
            umi_parse_again     : tsp00_C3;
            umi_control_session : boolean;
            umi_obj_hist_count  : tsp00_Int4;
            umi_values_len      : tsp00_Int4;
            umi_values_size     : tsp00_Int4;
            umi_values          : tsp00_MoveObjPtr;
      END;
 
      tgg00_UserMonitorInfoPtr = ^tgg00_UserMonitorInfo;
 
      tgg00_TempDataCache = RECORD
            tdc_monitor_info : tgg00_UserMonitorInfoPtr;
      END;
 
      tgg00_TempDataCachePtr = ^tgg00_TempDataCache;
 
      tgg00_SaveRestoreInputParam = RECORD
            sripBlocksize_gg00            : tsp00_Int2;
            sripHostTapeNum_gg00          : tsp00_Int2;
            sripFileVersion_gg00          : tsp00_Int4;
            (**)
            sripIsAutoLoad_gg00           : boolean;
            sripWithCheckpoint_gg00       : boolean;
            sripIsRestoreConfig_gg00      : boolean;
            sripOnlyCompleteSegments_gg00 : boolean;
            (**)
            sripHostTapenames_gg00        : tgg00_TapeNames;
            (**)
            sripHostFiletypes_gg00        : tgg00_Filetypes;
            (**)
            sripHostTapecount_gg00        : tgg00_TapeCount;
            (**)
            sripMedianame_gg00            : tgg00_MediaName;
            (**)
            sripUntilDate_gg00            : tsp00_Date;
            (**)
            sripUntilTime_gg00            : tsp00_Time;
            (**)
            sripUntilIOSequence_gg00      : tsp00_Uint4;
            (**)
            sripUtilCmdId_gg00            : tgg00_UtilCmdId;
            (**)
            sripConfigDbName_gg00         : tsp00_DbName;
            sripTapeLabel_gg00            : tsp00_C14; (* PTS 1128100 mb 2004-03-01 *)
            sripNoReleaseLog_gg00         : boolean;   (* PTS 1128703 mb 2004-05-05 *)
            sripWithFormat_gg00           : boolean;   (* PTS 1139725 UH 2005-01-13 *)
      END;
 
 
      tgg00_PageHeader = RECORD
            CASE integer OF
                1:
                    (pageId_gg00        : tsp00_PageNo;
                    pageType_gg00       : tgg00_PageType;
                    pageType2_gg00      : tgg00_PageType2;
                    pageCheck_gg00      : tgg00_PageCheckType;
                    pageMode_gg00       : tgg00_PageMode);
                2:
                    (pageHeaderStr_gg00 : tgg00_PageHeadStr);
                END;
            (*ENDCASE*) 
 
      tgg00_PageTrailer = tgg00_PageHeader;
 
      tgg00_PageCheckTrailer = RECORD
            trlChecksum_gg00 : tsp00_Int4;
            trlTrailer_gg00  : tgg00_PageTrailer
      END;
 
 
      tgg00_TimeoutGlobals = RECORD
            timeCmd_gg00 : tsp00_Int4;
            timeReq_gg00 : tsp00_Int4;
      END;
 
 
      tgg00_Lkey = RECORD
            CASE integer OF
                1:
                    (keyRecLenSpace_gg00 : tsp00_Int2;
                    keyLen_gg00          : tsp00_Int2;
                    keyVarOffSpace_gg00  : tsp00_Int2;
                    keyVarCntSpace_gg00  : tsp00_Int2;
                    keyVal_gg00          : tsp00_Key);
                11:
                    (kfill               : tsp00_Int2;
                    len                  : tsp00_Int2;
                    kfill1               : tsp00_Int2;
                    kfill2               : tsp00_Int2;
                    k                    : tsp00_Key);
                END;
            (*ENDCASE*) 
 
      tgg00_LkeyPtr = ^tgg00_Lkey;
      (* UH 2003-02-03 *)
      tgg00_WaitContextPtr = ^tgg00_WaitContext;
 
      tgg00_WaitContext = RECORD
            wcNextWaiting_gg00   : tgg00_WaitContextPtr;
            wcWaitingTaskId_gg00 : tsp00_TaskId;
&           ifdef BIT64
            wcFiller_gg00        : tsp00_Int4;
&           endif
      END;
 
      (* PTS 1110096 E.Z. *)
 
      tgg00_OneCurrSeqInfo = RECORD
            ocs_seq_surrogate : tgg00_Surrogate;
            ocs_seq_value     : tsp00_Number;
            ocs_seq_filler    : tsp00_Int4;
      END;
 
 
      tgg00_CurrSeqInfo = RECORD
            csi_cnt    : tsp00_Int2;
            csi_fill1  : tsp00_Int2;
            csi_fill2  : tsp00_Int4;
            csi_values : PACKED ARRAY [1..MAX_SEQINFOS_GG00] OF tgg00_OneCurrSeqInfo;
      END;
 
      tgg00_CurrSeqInfoPtr = ^tgg00_CurrSeqInfo;
      tgg00_ConsistentReadInfo = ( CnsReadNone, CnsReadStatement, CnsReadTransaction );
 
      tgg00_TransContext = RECORD
            trSessionId_gg00            : tgg91_SessionNo;    (* |          *)
            trTransId_gg00              : tgg91_TransNo;      (* |          *)
            trWriteTransId_gg00         : tgg91_TransNo;      (* |          * PTS 1108234 JA 2000-11-17 *)
            (*                                             (* |          *)
            trSubtransId_gg00           : tgg00_SubtransNo;   (* |          * PTS 1109485 E.Z. *)
            trChild_gg00                : tsp00_Int4;         (* |          *)
            trConsistView_gg00          : tgg91_TransNo;      (* |          *)
            trConsistReadInfo_gg00      : tgg00_ConsistentReadInfo;
            trFiller1_gg00              : boolean;         (* PTS 1110237 E.Z. *)
            (*                                             (* |          *)
            trState_gg00                : tgg00_TransState;   (* |          *)
            trError_gg00                : tgg00_BasisError;   (* |          *)
            trWarning_gg00              : tsp00_WarningSet;   (* | child    *)
            trObjReqTimeout_gg00        : tsp00_Uint2;        (* | data     *)
            (*                                             (* |          *)
            trSessionAuxCounter_gg00    : tsp00_Int4;         (* |          * h.b. PTS 1107826 *)
            trTempCount_gg00            : tsp00_Int4;         (* |          *)
            (*                                             (* |          *)
            trIoCount_gg00              : tsp00_8ByteCounter; (* |          * PTS 1103743 JA 1999-08-25 *)
            (* *)
            trTaskId_gg00               : tsp00_TaskId;
            trIndex_gg00                : tgg00_TransIndex;
            trBdExclFileLocks_gg00      : tsp00_Int2;
            (* *)
            trRowLocksPerSqlCmd_gg00    : tsp00_8ByteCounter;    (* PTS 1103743 JA 1999-08-25 *)
            (* *)
            trRteCommPtr_gg00           : tsp00_RteCommAddr;
            trBdTcachePtr_gg00          : tgg00_TempDataCachePtr;
            trAllocator_gg00            : tgg00_VoidPtr;         (* PTS 1108234 JA 2000-11-08 *)
            trLogContext_gg00           : tgg00_VoidPtr;
            trSeqInfoPtr_gg00           : tgg00_CurrSeqInfoPtr;  (* PTS 1110095 E.Z.  *)
            trAcvPtr_gg00               : tgg00_VoidPtr;
            trLocalFileDirPtr_gg00      : tgg00_VoidPtr;         (* new file dir M.Ki *)
            trErrorList_gg00            : tgg00_VoidPtr;         (* UH 2004-09-27     *)
            trFreePagesForIns_gg00      : tgg00_VoidPtr;         (* PTS 1136263 FF Object Layer - vbd90 *)
            trSchemaShrLockHash_gg00    : tgg00_VoidPtr;         (* PTS 1139346 FF Object Layer - vbd90 *)
            trClusterIOPtr_gg00         : tgg00_VoidPtr;         (* PTS 1139789 HB BW Feature Pack *)
            trClusterIOSize_gg00        : tsp00_Int4;            (* PTS 1139789 HB BW Feature Pack *)
            trSyncParallelServers_gg00  : boolean;               (* PTS 1126829 T.A.  *)
      END;
 
      tgg00_TransContextPtr = ^tgg00_TransContext;
 
      tgg00_UnivTrans = RECORD
            CASE integer OF
                1:
                    (utrTrans : tgg00_TransContext);
                END;
            (*ENDCASE*) 
 
      tgg00_UnivTransPtr = ^tgg00_UnivTrans;
 
      tgg00_ObjTransInfo = RECORD
            otrBeforeRef_gg00      : tgg91_PageRef;
            otrLockTransIndex_gg00 : tgg00_TransIndex;
            otrLockTransId_gg00    : tgg91_TransNo;
            otrUpdTransId_gg00     : tgg91_TransNo
      END;
 
      tgg00_ObjTransInfoPtr = ^tgg00_ObjTransInfo;
 
      tgg00_FilePos = RECORD
            tpsPno_gg00   : tsp00_PageNo;
            tpsIndex_gg00 : tsp00_Int4
      END;
 
      (* *)
      tgg00_ObjFileType = (
            oftUnknownType_egg00,
            oftFixLenObjFile_egg00,
            oftVarLenObjFile_egg00);
 
      tgg00_FileId = RECORD
            CASE integer OF
                1:
                    (fileRootCheck_gg00   : tsp00_Int4;
                    fileLeafNodes_gg00    : tsp00_Int4;
                    fileUserRef_gg00      : tgg00_UserRef;
                    fileVersion_gg00      : tgg91_FileVersion;
                    fileRoot_gg00         : tsp00_PageNo;
                    fileHandling_gg00     : tgg00_HandlingSet;
                    fileBdUse_gg00        : tgg00_BdUseSet;
                    fileType_gg00         : tgg00_FiletypeSet;
                    fileName_gg00         : tgg00_Filename);
                2:
                    (fileRootNodes_2_gg00 : tsp00_C8;
                    fileTriggerKey_gg00   : tsp00_C4;
                    fileResultSite_gg00   : tgg00_ServerdbNo);
                3:
                    (fileRootNodes_3_gg00 : tsp00_C8;
                    fileConstId_gg00      : tsp00_Int4);
                4:
                    (fileHeader__4_gg00   : tgg00_FileIdHead;
                    fileTfn_gg00          : tgg00_Tfn;
                    fileTfnNo_gg00        : tsp00_C1;
                    fileTabId_gg00        : tgg00_Surrogate);
                5:
                    (fileHeader__5_gg00     : tgg00_FileIdHead;
                    fileTfn______5_gg00     : tgg00_Tfn;         (* BD object container *)
                    fileTfnNo____5_gg00     : tgg00_Tfn;
                    fileContObjFileNo_gg00  : tsp00_Uint1;
                    fileObjFileType_gg00    : tgg00_ObjFileType;
                    fileNumObjKeyFiles_gg00 : tsp00_Uint1;        (* CRS 1109486 AK 27/02/2000 *)
                    fileFiller3_gg00        : tsp00_C3;
                    fileObjFileNo_gg00      : tgg00_ObjFileNo;
                    fileFiller4_gg00        : tsp00_C4);
                6:
                    (fileHeader__6_gg00   : tgg00_FileIdHead;
                    fileTfn______6_gg00   : tgg00_Tfn;         (* log file *)
                    fileTfnLog_gg00       : tgg00_TfnLog;
                    fileTransId_gg00      : tgg91_TransNo);
                7:
                    (fileHeader__7_gg00   : tgg00_FileIdHead;
                    fileTfn______7_gg00   : tgg00_Tfn;         (* KB auxiliary file *)
                    fileTfnNo____7_gg00   : tsp00_C1;
                    fileTransId__7_gg00   : tgg91_TransNo;
                    fileAuxCnt_gg00       : tsp00_Int4;
                    fileOldTfn_gg00       : tgg00_Tfn;
                    fileFiller1_gg00      : tsp00_C1;
                    fileFiller2_gg00      : tsp00_C2);
                8:
                    (fileHeader__8_gg00   : tgg00_FileIdHead;
                    fileTfn______8_gg00   : tgg00_Tfn;         (* AK temp file *)
                    fileTfnNo____8_gg00   : tsp00_C1;
                    fileZeroSite_gg00     : tsp00_Int2;        (* old PrefixDestroyFile format *)
                    fileSession_gg00      : tgg91_SessionNo;
                    fileTfnTemp_gg00      : tgg00_TfnTemp;
                    fileFsNode_gg00       : tsp00_C1;
                    fileLevel_gg00        : tsp00_C2;
                    fileTempCnt_gg00      : tsp00_Int4);
                9:
                    (fileHeader__9_gg00  : tgg00_FileIdHead;
                    fileTfn______9_gg00  : tgg00_Tfn;         (* KB temp file *)
                    fileTfnNo____9_gg00  : tsp00_C1;
                    fileZeroSite_9_gg00  : tsp00_Int2;        (* old PrefixDestroyFile format *)
                    fileSession__9_gg00  : tgg91_SessionNo;
                    fileTfnTemp__9_gg00  : tgg00_TfnTemp;
                    fileFsNode___9_gg00  : tsp00_C1;
                    fileTempTrans_gg00   : tgg91_TransNo);
                10:
                    (fileHeader_10_gg00  : tgg00_FileIdHead;  (* PTS 1107091 AK 30/06/2000 *)
                    fileTfn_____10_gg00  : tgg00_Tfn;         (* BD temp inv file *)
                    fileQueueInvRoot_gg00 : tsp00_PageNo);
                11:
                    (fileHeader_11_gg00    : tgg00_FileIdHead;
                    fileTfn_____11_gg00    : tgg00_Tfn;         (* BD object key files *)
                    fileTfnNo___11_gg00    : tsp00_C1;
                    fileObjKeyFileIndex_gg00 : tsp00_Uint1;       (* CRS 1109486 AK 27/02/2000 *)
                    fileFiller5_gg00       : tsp00_C1;
                    fileFiller6_gg00       : tsp00_C8;
                    fileObjKeyFileId_gg00  : tsp00_C4);
                12: (* Variant used by object layer with new FileDir FF 17-Sep-2003 *)
                    (fileHeader__12_gg00       : tgg00_FileIdHead;
                    fileTfn______12_gg00       : tgg00_Tfn;
                    fileTfnNo____12_gg00       : tgg00_Tfn;
                    fileContObjFileNo_12_gg00  : tsp00_Uint1;
                    fileObjFileType_12_gg00    : tgg00_ObjFileType;
                    fileNumObjKeyFiles_12_gg00 : tsp00_Uint1;
                    fileFiller3_12_gg00        : tsp00_C3;
                    fileDirFileId_gg00         : tsp00_C8);
                END;
            (*ENDCASE*) 
 
      tgg00_FileIdPtr = ^tgg00_FileId;
 
      tgg00_Lock = RECORD
            lockTabId_gg00  :
                  tgg00_Surrogate;
            lockSite_gg00   :
                  tgg00_ServerdbNo;
            lockMode_gg00   :
                  tgg00_LockReqMode;
            lockState_gg00  :
                  tgg00_LockReqState;
            lockKeyLen_gg00 :
                  tsp00_Int2;
            lockKeyPos_gg00 :
                  tsp00_Int2;
      END;
 
 
      tgg00_TwoFileIds = RECORD
            file_id : tgg00_FileId;
            inv_id  : tgg00_FileId
      END;
 
 
      tgg00_TwoKeys = RECORD
            listkey : tgg00_Lkey;
            reckey  : tgg00_Lkey
      END;
 
      tgg00_TwoKeysPtr = ^tgg00_TwoKeys;
 
      tgg00_SysInfoKey = RECORD
            sreclen               : tsp00_Int2;
            skeylen               : tsp00_Int2;
            svarcoloffset         : tsp00_Int2;
            svarcolcnt            : tsp00_Int2;
            CASE integer OF
                1 :
                    (sstandardkey : tsp00_C12);
                2 :
                    (stableid     : tgg00_Surrogate;
                    sentrytyp     : tsp00_C2;
                    slinkage      : tsp00_C2;
                    suserid       : tgg00_Surrogate;
                    sgrantuser    : tgg00_Surrogate);
                3 :
                    (sauthid      : tgg00_Surrogate;
                    sentrytyp1    : tsp00_C2;
                    slinkage1     : tsp00_C2;
                    stablen       : tsp00_Identifier);
                4 :
                    (stempid      : tsp00_Int4;
                    stemprest     : tsp00_Int4;
                    sentrytyp2    : tsp00_C2;
                    sintlinkage   : tsp00_Int2);
                5 :
                    (sauthid1     : tgg00_Surrogate;
                    sentrytyp3    : tsp00_C2;
                    slinkage3     : tsp00_Int2;
                    sappl         : tsp00_KnlIdentifier;
                    sproc         : tsp00_KnlIdentifier);
                6 :
                    (ssite        : tgg00_ServerdbNo;
                    sfil          : tsp00_Int2;
                    stemp         : tsp00_Int4);
                7 :
                    (sserverno    : tgg00_ServerdbNo);
                8 :
                    (all          : tgg00_Syskey);
                9 :
                    (sresnidfill  : tgg00_Surrogate;
                    sresentrytype : tsp00_C2;
                    srestablen    : tsp00_KnlIdentifier;
                    sresmodul_name: tsp00_KnlIdentifier);
                10 :
                    (szeroid      : tgg00_Surrogate;
                    sentrytype4   : tsp00_C2;
                    slinkage4     : tsp00_C2;
                    ssetname      : tsp00_Identifier);
                11 :
                    (sshowid      : tsp00_C2;
                    sshowkind     : tgg00_ShowKind);
                12 :
                    (szeroid1     : tgg00_Surrogate;
                    sentrytype5   : tsp00_C2;
                    slinkage5     : tsp00_C2;
                    sdbname       : tsp00_DbName);
                13 :
                    (sauthid6     : tgg00_Surrogate;
                    sentrytyp6    : tsp00_C2;
                    slinkage6     : tsp00_C2;
                    sidentifier   : tsp00_KnlIdentifier);
                14 :
                    (sauthid7     : tgg00_Surrogate;
                    sentrytyp7    : tsp00_C2;
                    slinkage7     : tsp00_C2;
                    sgwowner      : tsp00_KnlIdentifier;
                    sgwtablen     : tsp00_KnlIdentifier);
                15 :
                    (sclassflag   : tsp00_C2;
                    sclassid      : tsp00_C4);
                16 :
                    (sschema_id   : tgg00_Surrogate;
                    sentrytype8   : tsp00_C2;
                    slinkage8     : tsp00_C2;
                    sguid         : tsp00_C16);
                END;
            (*ENDCASE*) 
 
 
      tgg00_SeqInfo = RECORD
            seq_increment  : tsp00_C20;
            seq_minvalue   : tsp00_C20;
            seq_maxvalue   : tsp00_C20;
            seq_cachevalue : tsp00_C20;
            seq_site       : tgg00_ServerdbNo;
            seq_cycle      : boolean;
            seq_first      : boolean;
            seq_filler     : tsp00_Int4;
      END;
 
 
      tgg00_Info = RECORD
            CASE integer OF
                1:
                    (recSysKey_gg00    : tgg00_SysInfoKey);
                2:
                    (recSpace1_gg00    : tsp00_C8; (* Len, KeyLen,... *)
                    recInsSelInfo_gg00 : tsp00_C1;
                    recIntoTemp_gg00   : boolean;
                    recTempKey_gg00    : tgg00_Surrogate);
                11:
                    (basesyskey        : tgg00_SysInfoKey);
                21:
                    (rb_len           : tsp00_Int2;
                    rb_keylen         : tsp00_Int2;
                    rb_varcol_offset  : tsp00_Int2;
                    rb_varcol_cnt     : tsp00_Int2;
                    rb_ins_sel_info   : tsp00_C1;
                    rb_into_temp      : boolean;
                    rb_tempkey        : tgg00_Surrogate);
                END;
            (*ENDCASE*) 
 
 
      tgg00_HeaderRec = RECORD
            CASE integer OF
                1:
                    (hrecLen_gg00         : tsp00_Int2;
                    hrecKeyLen_gg00       : tsp00_Int2;
                    hrecVarcolOffset_gg00 : tsp00_Int2;
                    hrecVarcolCnt_gg00    : tsp00_Int2);
                2:
                    (hrecLenC2_gg00       : tsp00_C2;
                    hrecKeyLenC2_gg00     : tsp00_C2;
                    hrecVarcolOffC2_gg00  : tsp00_C2;
                    hrecVarcolCntC2_gg00  : tsp00_C2);
                3:
                    (hrecLenSpace_gg00    : tsp00_C2;
                    hrecKeyLenSpace_gg00  : tsp00_C2;
                    hrecPno_gg00          : tgg90_Cint4)
                END;
            (*ENDCASE*) 
 
 
      tgg00_Rec = RECORD
            CASE integer OF
                1:
                    (recBuf_gg00         : tsp00_Buf);
                2:
                    (recLen_gg00         : tsp00_Int2;
                    recKeyLen_gg00       : tsp00_Int2;
                    recVarcolOffset_gg00 : tsp00_Int2;
                    recVarcolCnt_gg00    : tsp00_Int2;
                    recBody_gg00         : tgg00_RecBody);
                3:
                    (recKey_gg00         : tgg00_Lkey);
                4:
                    (recPart_gg00        : ARRAY [0..MAX_RECPARTS_GG00] OF tgg00_RecPart);
                5:
                    (recHead_gg00        : tgg00_HeaderRec);
                6:
                    (recLenC2_gg00       : tsp00_C2;
                    recKeyLenC2_gg00     : tsp00_C2;
                    recVarcolOffC2_gg00  : tsp00_C2;
                    recVarcolCntC2_gg00  : tsp00_C2);
                7:
                    (recLenSpace1_gg00   : tsp00_C2;
                    recKeyLenSpace1_gg00 : tsp00_C2;
                    recPno_gg00          : tgg90_Cint4);
                8:
                    (recLenSpace2_gg00   : tsp00_C2;
                    recKeyLenSpace2_gg00 : tsp00_C2;
                    recInvListKind_gg00  : tgg00_InvListKind;
                    recAlignment_gg00    : tsp00_Uint1;
                    recNumPrimKeys_gg00  : tsp00_Uint2);
                11:
                    (buf                 : tsp00_Buf);
                12:
                    (len                 : tsp00_Int2;
                    keylen               : tsp00_Int2;
                    space_var_offset     : tsp00_Int2;
                    space_var_cnt        : tsp00_Int2;
                    info                 : tgg00_RecBody);
                13:
                    (mkey                : tgg00_Lkey);
                14:
                    (recpart             : ARRAY [0..MAX_RECPARTS_GG00] OF tgg00_RecPart);
                15:
                    (recHeader_gg00      : PACKED ARRAY [1..RSN_RECHEAD_MXGG00] OF char;
                    columnoffset_gg00    : PACKED ARRAY [1..MAX_COLUMNOFFSETS] OF tsp00_Int2);
                16:
                    (recHeader2_gg00     : PACKED ARRAY [1..RSN_RECHEAD_MXGG00] OF char;
                    columnbuf_gg00       : PACKED ARRAY [1..RECCOLUMNS_MXGG00] OF char);
                END;
            (*ENDCASE*) 
 
      tgg00_RecPtr = ^tgg00_Rec;
 
      tgg00_BufLenInfo = RECORD
            CASE boolean OF
                true:
                    (rb : tgg00_Rec);
                false:
                    (ri : tgg00_Info);
                END;
            (*ENDCASE*) 
 
      tgg00_ColumnSet = SET OF 1 .. MAX_COL_PER_TAB_GG00;
      tgg00_ColumnDistinctValues =  ARRAY[1..MAX_COL_PER_TAB_GG00] OF tsp00_Int4;
 
      tgg00_TreeStatInfo = RECORD
      (* in *)
            TreeStatTreeId_gg00            : tgg00_FileId;
            TreeStatCalculate_gg00         : boolean;
            TreeStatFill1_gg00             : boolean;
            TreeStatFill2_gg00             : tsp00_Int2;
            TreeStatNoSampleLeaves_gg00    : tsp00_Int4;
            TreeStatMaxNumberOfLeaves_gg00 : tsp00_Int4;
            (* out *)
            TreeStatNumberOfLeaves_gg00    : tsp00_Int4;
            TreeStatNumberOfRecords_gg00   : tsp00_Int4;
      END;
 
      tgg00_TreeStatInfoPtr = ^tgg00_TreeStatInfo;
 
      tgg00_ColStatInfo = RECORD
      (* in *)
            ColStatTreeId_gg00           : tgg00_FileId;
            ColStatCalculate_gg00        : boolean;
            ColStatFill1_gg00            : boolean;
            ColStatFill2_gg00            : tsp00_Int2;
            ColStatNoSampleLeaves_gg00   : tsp00_Int4;
            (* out *)
            ColStatNumberOfLeaves_gg00   : tsp00_Int4;
            ColStatNumberOfRecords_gg00  : tsp00_Int4;
            ColStatDistValues_gg00       : tgg00_ColumnDistinctValues;
      END;
 
      tgg00_ColStatInfoPtr = ^tgg00_ColStatInfo;
 
      tgg00_TableStatInfo = RECORD
            TabStatNodes_gg00               : tsp00_Int4;
            TabStatIndexNodes_gg00          : tsp00_Int4;
            TabStatLeafnodes_gg00           : tsp00_Int4;
            TabStatTreeCov_gg00             : tsp00_Int4;
            TabStatRootCov_gg00             : tsp00_Int4;
            TabStatIndexCov_gg00            : tsp00_Int4;
            TabStatMinIndexCov_gg00         : tsp00_Int4;
            TabStatMaxIndexCov_gg00         : tsp00_Int4;
            TabStatLeafCov_gg00             : tsp00_Int4;
            TabStatMinLeafCov_gg00          : tsp00_Int4;
            TabStatMaxLeafCov_gg00          : tsp00_Int4;
            TabStatRecords_gg00             : tsp00_Int4;
            TabStatRecsPerPage_gg00         : tsp00_Int4;
            TabStatMinRecsPerPage_gg00      : tsp00_Int4;
            TabStatMaxRecsPerPage_gg00      : tsp00_Int4;
            TabStatAveRecLength_gg00        : tsp00_Int4;
            TabStatMinRecLength_gg00        : tsp00_Int4;
            TabStatMaxRecLength_gg00        : tsp00_Int4;
            TabStatDefinedStringCols_gg00   : tsp00_Int4;
            TabStatAveLenStringCol_gg00     : tsp00_Int4;
            TabStatMinLenStringCol_gg00     : tsp00_Int4;
            TabStatMaxLenStringCol_gg00     : tsp00_Int4;
            TabStatAllStringColPages_gg00   : tsp00_Int4;
            TabStatAveStringColPages_gg00   : tsp00_Int4;
            TabStatMinStringColPages_gg00   : tsp00_Int4;
            TabStatMaxStringColPages_gg00   : tsp00_Int4;
            TabStatClusterCount_gg00        : tsp00_Int4;
            TabStatRootLevel_gg00           : tsp00_Int2;
            TabStatAveKeyLength_gg00        : tsp00_Int2;
            TabStatMinKeyLength_gg00        : tsp00_Int2;
            TabStatMaxKeyLength_gg00        : tsp00_Int2;
            TabStatAveSepLength_gg00        : tsp00_Int2;
            TabStatMinSepLength_gg00        : tsp00_Int2;
            TabStatMaxSepLength_gg00        : tsp00_Int2;
      END;
 
      tgg00_TableStatInfoPtr = ^tgg00_TableStatInfo;
 
      tgg00_SampleInfo = RECORD
            sam_tree_id         : tgg00_FileId;
            sam_rows            : tsp00_Int4;
            sam_percent         : tsp00_Int4;
            sam_with_long       : boolean; (* PTS 1115096 *)
            sam_fill1           : boolean;
            sam_fill2           : tsp00_Int2;
            leafnodes           : tsp00_Int4;
            sec_key_cnt         : tsp00_Int4;
            prim_key_cnt        : tsp00_Int4;
            avg_prim_per_list   : tsp00_Int4;
            records             : tsp00_Int4;
            all_stringcol_pages : tsp00_Int4;
            nodes               : tsp00_Int4;
            null_value_cnt      : tsp00_Int4;
            min_sec_key_len     : tsp00_Int4;
            max_sec_key_len     : tsp00_Int4;
            avg_sec_key_len     : tsp00_Int4;
            (*                          for ak42print_tab_index only: *)
            treecov             : tsp00_Int4;
            rootcov             : tsp00_Int4;
            indexcov            : tsp00_Int4;
            min_indexcov        : tsp00_Int4;
            max_indexcov        : tsp00_Int4;
            leafcov             : tsp00_Int4;
            min_leafcov         : tsp00_Int4;
            max_leafcov         : tsp00_Int4;
            rec_per_page        : tsp00_Int4;
            ave_rec_length      : tsp00_Int4;
            min_rec_length      : tsp00_Int4;
            max_rec_length      : tsp00_Int4;
            ave_key_length      : tsp00_Int4;
            indexnodes          : tsp00_Int4;
            ave_sep_length      : tsp00_Int4;
            min_key_length      : tsp00_Int2;
            max_key_length      : tsp00_Int2;
            min_sep_length      : tsp00_Int2;
            max_sep_length      : tsp00_Int2;
            min_rec_per_page    : tsp00_Int2;
            max_rec_per_page    : tsp00_Int2;
            rootsegm            : tsp00_PageNo;
            smplFileType_gg00   : tgg00_FiletypeSet;
            pcs_dummy           : boolean;
            hight               : tsp00_Int2;
            defined_stringcols  : tsp00_Int4;
            min_len_stringcol   : tsp00_Int4;
            max_len_stringcol   : tsp00_Int4;
            ave_len_stringcol   : tsp00_Int4;
            min_stringcol_pages : tsp00_Int4;
            max_stringcol_pages : tsp00_Int4;
            ave_stringcol_pages : tsp00_Int4;
            invsel_1            : tsp00_Int4;
            invsel_5            : tsp00_Int4;
            invsel_10           : tsp00_Int4;
            invsel_25           : tsp00_Int4;
            invsel_notsel       : tsp00_Int4;
            min_prim_per_list   : tsp00_Int4;
            max_prim_per_list   : tsp00_Int4;
            sam_clustercount    : tsp00_Int4;
            dist_values         : ARRAY[1..MAX_COL_PER_TAB_GG00] OF tsp00_Int4;
      END;
 
      tgg00_SampleInfoPtr = ^tgg00_SampleInfo;
 
      tgg00_IndexCounter = RECORD
            idc_prim_leafnodes  : tsp00_Int4;
            idc_prim_indexnodes : tsp00_Int4;
            idc_sec_leafnodes   : tsp00_Int4;
            idc_prim_keycount   : tsp00_Int4;
            idc_sec_keycount    : tsp00_Int4;
            idc_nullcount       : tsp00_Int4;
            idc_prim_mismatch   : tsp00_Int4;
      END;
 
 
      tgg00_DataPart = RECORD
            CASE integer OF
                1:
                    (mbp_buf          : tsp00_MoveObj);
                2:
                    (mbp_4kbuf        : tsp00_Buf);
                3:
                    (mbp_rec          : tgg00_Rec);
                4:
                    (mbp_reclen       : tsp00_Int2;
                    mbp_keylen        : tsp00_Int2;
                    mbp_varcol_offset : tsp00_Int2;
                    mbp_varcol_cnt    : tsp00_Int2);
                5:
                    (mbp_key          : tgg00_Lkey;
                    mbp_key2          : tgg00_Lkey);
                6:
                    (mbp_space1       : tsp00_Int2;
                    mbp_space2        : tsp00_Int2;
                    mbp_space3        : tsp00_Int2;
                    mbp_space4        : tsp00_Int2;
                    mbp_recinfo       : tgg00_RecBody);
                7:
                    (mbp_sample       : tgg00_SampleInfo);
                8:
                    (mbp_index_cnt    : tgg00_IndexCounter);
                9:
                    (mbp_info         : tgg00_Info);
                10:
                    (mbp_ColStat      : tgg00_ColStatInfo);
                11:
                    (mbp_TreeStat     : tgg00_TreeStatInfo);
                END;
            (*ENDCASE*) 
 
      tgg00_DataPartPtr = ^tgg00_DataPart;
      tgg00_LongresultBuf = PACKED ARRAY [ 1..MAX_LONGRESULTBUF_GG00] OF char;
 
      tgg00_SelectBuffer = RECORD
            buf_rec : tgg00_Rec;
            rbuf    : tsp00_Buf;
            lbuf    : tsp00_Buf;
            inv_buf : tgg00_Rec;
            CASE boolean OF
                true :
                    (result  : tgg00_Rec);
                false :
                    (longresult : tgg00_LongresultBuf);
                END;
            (*ENDCASE*) 
 
 
      tgg00_StackEntry = RECORD
            CASE integer OF
                1:
                    (etype       : tgg00_StackEntryType;
                    eop          : tgg00_StackOpType;
                    epos         : tsp00_Int2;
                    elen_var     : tsp00_Int2;
                    ecol_tab     : tsp00_C2);
                2:
                    (efill2_type : tgg00_StackEntryType;
                    eop_func     : tgg00_StackOpFunc;
                    epos_c2      : tsp00_C2;
                    var_elen     : tsp00_C2;
                    ecol_pos     : tsp00_Int2);
                3:
                    (efill3_type : tgg00_StackEntryType;
                    eop_out      : tgg00_StackOpOutput;
                    ecolno       : tsp00_Int2;
                    eval4_var    : tsp00_Int4);
                4:
                    (efill4_type : tgg00_StackEntryType;
                    eop_build_in : tgg00_StackOpBuildIn;
                    eformat      : tgg00_DateTimeFormat;
                    edatatype    : tsp00_DataType;
                    elanguage    : tsp00_C3;
                    elength      : tsp00_Uint1);
                5:
                    (efill5_type : tgg00_StackEntryType;
                    eop_new_rec  : boolean);
                6:
                    (efill6_type : tgg00_StackEntryType;
                    eop_fill     : boolean;
                    efill7_pos   : tsp00_Int2;
                    efill8_len   : tsp00_Int2;
                    eindex       : tsp00_Int2);
                7:
                    (efill7_type    : tgg00_StackEntryType;
                    eop7_fill      : boolean;
                    eparamdatatype : tsp00_DataType;
                    efraction      : tsp00_Uint1);
                8:
                    (esurrogate    : tgg00_Surrogate);
                END;
            (*ENDCASE*) 
 
      tgg00_StEntryAddr  = ^tgg00_StackEntry;
      tgg00_StackList    = ARRAY [1..MAX_INT2_SP00] OF tgg00_StackEntry;
      tgg00_StackListPtr = ^tgg00_StackList;
 
      tgg00_ObjColDesc = RECORD
            colHashCount_gg00 : tsp00_Int2;
            colInvCount_gg00  : tsp00_Int2;
            colHashList_gg00  : tgg00_StackListPtr;
            colInvList_gg00   : tgg00_StackListPtr
      END;
 
      tgg00_StackState = PACKED SET OF (
            ssSubquery_egg00,
            ssConstParamExpr_egg00,
            ssCopyRow_egg00,
            ssUnused4_egg00,
            ssUnused5_egg00,
            ssUnused6_egg00,
            ssUnused7_egg00,
            ssUnused8_egg00);
 
      tgg00_StackDesc = RECORD
            mcol_pos     : tsp00_Int2;
            mcol_cnt     : tsp00_Int2;
            mmult_pos    : tsp00_Int2;
            mmult_cnt    : tsp00_Int2;
            (* *)
            mqual_pos    : tsp00_Int2;
            mqual_cnt    : tsp00_Int2;
            mview_pos    : tsp00_Int2;
            mview_cnt    : tsp00_Int2;
            (* *)
            mupd_pos     : tsp00_Int2;
            mupd_cnt     : tsp00_Int2;
            mlink_pos    : tsp00_Int2;
            mlink_cnt    : tsp00_Int2;
            (* *)
            mstrat_pos   : tsp00_Int2;
            mstrat_cnt   : tsp00_Int2;
            mstring_pos  : tsp00_Int2;
            mstring_cnt  : tsp00_Int2;
            (* *)
            mtrigger_pos : tsp00_Int2;
            mtrigger_cnt : tsp00_Int2;
            minvqual_pos : tsp00_Int2;
            minvqual_cnt : tsp00_Int2;
            (* *)
            CASE integer OF
                1:
                    (mresqual_pos   : tsp00_Int2;
                    mresqual_cnt    : tsp00_Int2;
                    mspecial        : tsp00_Int2;
                    mstack_state    : tgg00_StackState;
                    mbool           : boolean;
                    msqlmode        : tsp00_SqlMode;
                    mdatetimeformat : tgg00_DateTimeFormat; (* PTS 1115085 *)
                    mcachetrigger   : tsp00_Int2;
                    malignfill3     : tsp00_Int4;
                    (* *)
                    mst_optimize_pos: tsp00_Int2;
                    mst_work_depth  : tsp00_Int2;
                    mfirst_free     : tsp00_Int2;
                    mst_max         : tsp00_Int2;
                    mst_addr        : tgg00_StackListPtr);
                2:
                    (mfill1         : tsp00_Int2;
                    mfill2          : tsp00_Int2;
                    mfixedcol_len   : tsp00_Int2;
                    mfill3          : tsp00_C2;
                    mfill4          : tsp00_C8;
                    (* *)
                    mfill5          : tsp00_C4;
                    mfill6          : tsp00_C4;
                    maddr_space     : tsp00_C8);
                3:
                    (mfill7         : tsp00_Int2;
                    mfill8          : tsp00_Int2;
                    mswap           : boolean;
                    mswap_kind      : tsp00_SwapKind);
                END;
            (*ENDCASE*) 
 
 
      tgg00_LongQual = RECORD
            lq_lock_tabid           : tgg00_Surrogate;
            lq_pos                  : tsp00_Int4;
            lq_len                  : tsp00_Int4;
            lq_long_size            : tsp00_Int4;
            lq_long_in_file         : boolean;
            lq_code                 : tsp00_CodeType;
            lq_first_insert         : boolean;
            lq_last_part_of_usercmd : boolean;
            lq_prefetch             : boolean; (* PTS 1122546 D.T. *)
            lq_unused_filler1       : tsp00_C1;
            CASE boolean OF
                false :
                    (lq_expand_char : tsp00_C1;
                    lq_trunc_req    : boolean;
                    lq_data_offset  : tsp00_Int4);
                true  :
                    (lq_try_unlock  : boolean;
                    lq_is_pattern   : boolean);
                END;
            (*ENDCASE*) 
 
      tgg00_FetchState = (
            fs_reverse_fetch,
            fs_use_inv,
            fs_use_stop,
            fs_use_recs,
            fs_single,
            fs_use_cluster
            );
 
      tgg00_FetchDesc = RECORD
            m_fill1          : tgg00_FileId;
            m_fill2          : tgg00_StackDesc;
            m_resnum         : tsp00_ResNum;
            m_res_build      : boolean;
            m_restree        : tgg00_FileId;
            m_keycnt         : tsp00_ResNum;
            m_fill3          : tsp00_C1;
            m_searched_pages : tsp00_Int4;
            (* bis hierher siehe tgg00_QualBuf *)
            m_fns1           : tgg00_FileId;
            m_fns2           : tgg00_FileId;
            m_rescnt         : tsp00_Int4;
            m_rowno          : tsp00_Int4;
            m_cnt            : tsp00_Int4;
            m_fetch_state    : SET OF tgg00_FetchState;
            m_fill4          : tsp00_C3;
            m_keylen         : tsp00_Int2;
            m_leng           : tsp00_Int2;
            m_exp_idx_pages  : tsp00_Int4;
            m_startkeys      : tgg00_TwoKeys; (*pos 269 *)
            m_stopkeys       : tgg00_TwoKeys;
            m_firstkeys      : tgg00_TwoKeys;
            m_start          : tgg00_Lkey;
            m_dist_optim     : tsp00_Int2;
            m_strat_info     : tgg00_RefInfoIndex;
            m_qual_kind      : tgg00_QualKind;
            m_data           : tgg00_DataPartPtr;
      END;
 
 
      tgg00_LinkChainInfo = RECORD
            mlp_fill1      : tgg00_FileId;
            mlp_stack_desc : tgg00_StackDesc;
            mlp_linktree   : tgg00_FileId;
            mlp_use_fileid : tsp00_Int2;
            mlp_use_site   : tgg00_ServerdbNo;
            mlp_sa_level   : tsp00_Int2;
            mlp_lo_level   : tsp00_Int2;
            mlp_sendsite   : tgg00_ServerdbNo;
            mlp_owner      : tsp00_KnlIdentifier;
            mlp_tablen     : tsp00_KnlIdentifier;
            mlp_ref_name   : tsp00_KnlIdentifier;
      END;
 
 
      tgg00_TriggerInfo = RECORD
            tg_param_pos        : tsp00_Int2;
            tg_param_cnt        : tsp00_Int2;
            tg_qual_pos         : tsp00_Int2;
            tg_qual_cnt         : tsp00_Int2;
            tg_misc             : tgg00_MiscSet;
            tg_messType         : tgg00_MessType;
            tg_updated_set_size : tsp00_Int2;
            tg_upd_set          : tgg00_ColumnSet;
            tg_updated_set      : tgg00_ColumnSet;
      END;
 
 
      tgg00_TriggerInfoQualBuf = RECORD
            tiqb_link_filler    : tgg00_LinkChainInfo;
            tiqb_tree_id        : tgg00_FileId;
            tiqb_trigger_count  : tsp00_Int4;
            tiqb_key            : tsp00_C4;
            tiqb_init_key       : tsp00_C4;
            tiqb_trigger_info   : ARRAY[1..2] OF tgg00_TriggerInfo;
      END;
 
 
      tgg00_QualBuf = RECORD
            CASE integer OF
                1:
                    (buf             : tsp00_Buf);
                2:
                    (mtree           : tgg00_FileId;
                    mcol_pos         : tsp00_Int2;
                    mcol_cnt         : tsp00_Int2;
                    mmult_pos        : tsp00_Int2;
                    mmult_cnt        : tsp00_Int2;
                    mqual_pos        : tsp00_Int2;
                    mqual_cnt        : tsp00_Int2;
                    mview_pos        : tsp00_Int2;
                    mview_cnt        : tsp00_Int2;
                    mupd_pos         : tsp00_Int2;
                    mupd_cnt         : tsp00_Int2;
                    mlink_pos        : tsp00_Int2;
                    mlink_cnt        : tsp00_Int2;
                    mstrat_pos       : tsp00_Int2;
                    mstrat_cnt       : tsp00_Int2;
                    mstring_pos      : tsp00_Int2;
                    mstring_cnt      : tsp00_Int2;
                    mtrigger_pos     : tsp00_Int2;
                    mtrigger_cnt     : tsp00_Int2;
                    minvqual_pos     : tsp00_Int2;
                    minvqual_cnt     : tsp00_Int2;
                    mresqual_pos     : tsp00_Int2;
                    mresqual_cnt     : tsp00_Int2;
                    mspecial         : tsp00_Int2;
                    mstack_state     : tgg00_StackState;
                    mbool            : boolean;
                    msqlmode         : tsp00_SqlMode;
                    mdatetimeformat  : tgg00_DateTimeFormat; (* PTS 1115085 *)
                    mcachetrigger   : tsp00_Int2;
                    malignfill3      : tsp00_Int4;
                    mst_optimize_pos : tsp00_Int2;
                    mst_work_depth   : tsp00_Int2;
                    mfirst_free      : tsp00_Int2;
                    mst_max          : tsp00_Int2;
                    mst_addr         : tgg00_StackListPtr);
                3:
                    (mst_fill        : tgg00_FileId;
                    mstack_desc      : tgg00_StackDesc;
                    mlinktree        : tgg00_FileId);
                4:
                    (mr_fill1        : tgg00_FileId;         (* ReturnStackEntries *)
                    mr_fill2         : tgg00_StackDesc;
                    mr_resnum        : tsp00_ResNum;
                    mr_res_build     : boolean;
                    mr_restree       : tgg00_FileId;
                    mr_keycnt        : tsp00_ResNum;
                    mr_fill3         : boolean;
                    mr_pagecnt       : tsp00_Int4;
                    mr_first_serial  : tsp00_Number;
                    mr_last_serial   : tsp00_Number);
                5:
                    (mtable_tree     : tgg00_FileId;         (* succ file version *)
                    mold_long_cnt    : tsp00_Int2;
                    mnew_long_cnt    : tsp00_Int2);
                6:
                    (mut_fill1       : tgg00_FileId;
                    mut_diag_type    : tgg00_DiagType;
                    mut_config       : boolean;
                    mut_pool_size    : tsp00_Int2;
                    mut_index_no     : tsp00_Int2;
                    mut_pno          : tsp00_Int4;
                    mut_pno2         : tsp00_Int4;
                    mut_count        : tsp00_Int4;
                    mut_dump_state   : tgg00_DumpStateSet;
                    mut_surrogate    : tgg00_Surrogate;
                    mut_authname     : tsp00_KnlIdentifier;
                    mut_tabname      : tsp00_KnlIdentifier;
                    mut_dev          : tsp00_VFilename;
                    mut_dev2         : tsp00_VFilename;
                    mut_hostfn       : tsp00_VFilename);
                7:
                    (mlock_cnt       : tsp00_Int2;
                    mlock_fill1      : tsp00_Int2;
                    mlock            : ARRAY [1..MAX_QUALBUF_LOCKS_GG00] OF tgg00_Lock);
                8:
                    (msave_restore   : tgg00_SaveRestoreInputParam);
                (* PTS 1111576 E.Z. *)
                10:
                    (mf_desc         : tgg00_FetchDesc);
                11:
                    (ml_tree         : tgg00_FileId;         (* long col access *)
                    ml_stack_desc    : tgg00_StackDesc;
                    ml_long_qual     : tgg00_LongQual);
                13:
                    (mcfc_tree       : tgg00_FileId;         (* copy file content *)
                    mcfc_pno         : tsp00_PageNo;
                    mcfc_fill_len    : tsp00_Int4);
                14:
                    (mlc_info        : tgg00_LinkChainInfo); (* chaining link parsinfo's *)
                15: (* PTS 1125096 E.Z. *)
                    (mv_tree           : tgg00_FileId;         (* m_verify *)
                    mv_longcnt         : tsp00_Int2;
                    mv_newlongcnt      : tsp00_Int2;
                    mv_extended        : boolean;
                    mv_delete_longfile : boolean;
                    mv_filler2         : tsp00_Int2);
                16 :
                    (mtrigger_info : tgg00_TriggerInfoQualBuf);
                END;
            (*ENDCASE*) 
 
      tgg00_QualBufPtr = ^tgg00_QualBuf;
      tgg00_FieldLists = ARRAY [0..cgg_idx_max_valuefieldlist] OF tsp00_MoveObjPtr;
      tgg00_FieldListsPtr = ^tgg00_FieldLists;
 
      tgg00_MessBufHeader = RECORD
            mb_qual_len   : tsp00_Int4;
            mb_data_len   : tsp00_Int4;
            mb_strat_len  : tsp00_Int4;
            mb_fill2      : tsp00_Int4;
            (* *)
            mb_src_site   : tgg00_ServerdbNo;
            mb_reply      : boolean;
            mb_fill3      : boolean;
            mb_type       : tgg00_MessType;
            mb_type2      : tgg00_MessType2;
            mb_struct     : tgg00_MessBufStruct;
            mb_fill1      : boolean;
      END;
 
      tgg00_MessBlockPtr = ^tgg00_MessBlock;
 
      tgg00_MessBufTrailer = RECORD
            CASE integer OF
                2:
                    (mb_ptr_space1  : tsp00_C8; (* mb_data        *)
                    mb_ptr_space2   : tsp00_C8; (* mb_qual        *)
                    mb_ptr_space3   : tsp00_C8; (* mb_st          *)
                    mb_ptr_space4   : tsp00_C8; (* mb_work_st     *)
                    mb_ptr_space5   : tsp00_C8; (* mb_workbuf     *)
                    mb_ptr_space6   : tsp00_C8; (* mb_next_mblock *)
                    mb_ptr_space7   : tsp00_C8; (* mb_trns        *)
                    mb_ptr_space8   : tsp00_C8; (* mb_strat       *)
                    mb_data_size    : tsp00_Int4;
                    mb_qual_size    : tsp00_Int4;
                    mb_strat_size   : tsp00_Int4;
                    mb_st_size      : tsp00_Int4;
                    mb_st_max       : tsp00_Int2;
                    mb_work_st_max  : tsp00_Int2;
                    mb_workbuf_size : tsp00_Int4;
                    mb_fieldlists   : tgg00_FieldLists);
                3:
                    (mb_data        : tgg00_DataPartPtr);
                4:
                    (mb_pt_space4_1 : tsp00_C8;
                    mb_qual         : tgg00_QualBufPtr);
                5:
                    (mb_pt_space5_1 : tsp00_C8;
                    mb_ptr_space5_2 : tsp00_C8;
                    mb_st           : tgg00_StackListPtr);
                6:
                    (mb_pt_space6_1 : tsp00_C8;
                    mb_ptr_space6_2 : tsp00_C8;
                    mb_ptr_space6_3 : tsp00_C8;
                    mb_work_st      : tgg00_StackListPtr);
                7:
                    (mb_pt_space7_1 : tsp00_C8;
                    mb_ptr_space7_2 : tsp00_C8;
                    mb_ptr_space7_3 : tsp00_C8;
                    mb_ptr_space7_4 : tsp00_C8;
                    mb_workbuf      : tsp00_MoveObjPtr);
                8:
                    (mb_pt_space8_1 : tsp00_C8;
                    mb_ptr_space8_2 : tsp00_C8;
                    mb_ptr_space8_3 : tsp00_C8;
                    mb_ptr_space8_4 : tsp00_C8;
                    mb_ptr_space8_5 : tsp00_C8;
                    mb_next_mblock  : tgg00_MessBlockPtr);
                9:
                    (mb_pt_space9_1 : tsp00_C8;
                    mb_ptr_space9_2 : tsp00_C8;
                    mb_ptr_space9_3 : tsp00_C8;
                    mb_ptr_space9_4 : tsp00_C8;
                    mb_ptr_space9_5 : tsp00_C8;        (* mb_trns = mb_trans_child *)
                    mb_ptr_space9_6 : tsp00_C8;
                    mb_trns         : tgg00_TransContextPtr);
                10:
                    (mb_pt_space10_1 : tsp00_C8;
                    mb_ptr_space10_2 : tsp00_C8;
                    mb_ptr_space10_3 : tsp00_C8;
                    mb_ptr_space10_4 : tsp00_C8;
                    mb_ptr_space10_5 : tsp00_C8;
                    mb_ptr_space10_6 : tsp00_C8;
                    mb_ptr_space10_7 : tsp00_C8;
                    mb_strat         : tsp00_MoveObjPtr);
                END;
            (*ENDCASE*) 
 
 
      tgg00_MessBlock = RECORD
            CASE integer OF
                1:
                    (mb_header      : tgg00_MessBufHeader;
                    mb_trailer      : tgg00_MessBufTrailer);
                2:
                    (mb_qual_len    : tsp00_Int4;
                    mb_data_len     : tsp00_Int4;
                    mb_strat_len    : tsp00_Int4;
                    mb_fill2        : tsp00_Int4;
                    (* *)
                    mb_src_site     : tgg00_ServerdbNo;
                    mb_reply        : boolean;
                    mb_fill3        : boolean;
                    mb_type         : tgg00_MessType;
                    mb_type2        : tgg00_MessType2;
                    mb_struct       : tgg00_MessBufStruct;
                    mb_fill1        : boolean;
                    (* *)
                    mb_ptr_space1   : tsp00_C8; (* mb_data        *)
                    mb_ptr_space2   : tsp00_C8; (* mb_qual        *)
                    mb_ptr_space3   : tsp00_C8; (* mb_st          *)
                    mb_ptr_space4   : tsp00_C8; (* mb_work_st     *)
                    mb_ptr_space5   : tsp00_C8; (* mb_workbuf     *)
                    mb_ptr_space6   : tsp00_C8; (* mb_next_mblock *)
                    mb_ptr_space7   : tsp00_C8; (* mb_trns        *)
                    mb_ptr_space8   : tsp00_C8; (* mb_strat       *)
                    mb_data_size    : tsp00_Int4;
                    mb_qual_size    : tsp00_Int4;
                    mb_strat_size   : tsp00_Int4;
                    mb_st_size      : tsp00_Int4;
                    mb_st_max       : tsp00_Int2;
                    mb_work_st_max  : tsp00_Int2;
                    mb_workbuf_size : tsp00_Int4;
                    mb_fieldlists   : tgg00_FieldLists);
                3 :
                    (mb_space3      : tgg00_MessBufHeader;
                    mb_data         : tgg00_DataPartPtr);
                4 :
                    (mb_space4      : tgg00_MessBufHeader;
                    mb_ptr_space4_1 : tsp00_C8;
                    mb_qual         : tgg00_QualBufPtr);
                5 :
                    (mb_space5      : tgg00_MessBufHeader;
                    mb_ptr_space5_1 : tsp00_C8;
                    mb_ptr_space5_2 : tsp00_C8;
                    mb_st           : tgg00_StackListPtr);
                6 :
                    (mb_space6      : tgg00_MessBufHeader;
                    mb_ptr_space6_1 : tsp00_C8;
                    mb_ptr_space6_2 : tsp00_C8;
                    mb_ptr_space6_3 : tsp00_C8;
                    mb_work_st      : tgg00_StackListPtr);
                7 :
                    (mb_space7      : tgg00_MessBufHeader;
                    mb_ptr_space7_1 : tsp00_C8;
                    mb_ptr_space7_2 : tsp00_C8;
                    mb_ptr_space7_3 : tsp00_C8;
                    mb_ptr_space7_4 : tsp00_C8;
                    mb_workbuf      : tsp00_MoveObjPtr);
                8 :
                    (mb_space8      : tgg00_MessBufHeader;
                    mb_ptr_space8_1 : tsp00_C8;
                    mb_ptr_space8_2 : tsp00_C8;
                    mb_ptr_space8_3 : tsp00_C8;
                    mb_ptr_space8_4 : tsp00_C8;
                    mb_ptr_space8_5 : tsp00_C8;
                    mb_next_mblock  : tgg00_MessBlockPtr);
                9 :
                    (mb_space9      : tgg00_MessBufHeader;
                    mb_ptr_space9_1 : tsp00_C8;
                    mb_ptr_space9_2 : tsp00_C8;
                    mb_ptr_space9_3 : tsp00_C8;
                    mb_ptr_space9_4 : tsp00_C8;
                    mb_ptr_space9_5 : tsp00_C8;        (* mb_trns = mb_trans_child *)
                    mb_ptr_space9_6 : tsp00_C8;
                    mb_trns         : tgg00_TransContextPtr);
                10:
                    (mb_space10      : tgg00_MessBufHeader;
                    mb_ptr_space10_1 : tsp00_C8;
                    mb_ptr_space10_2 : tsp00_C8;
                    mb_ptr_space10_3 : tsp00_C8;
                    mb_ptr_space10_4 : tsp00_C8;
                    mb_ptr_space10_5 : tsp00_C8;
                    mb_ptr_space10_6 : tsp00_C8;
                    mb_ptr_space10_7 : tsp00_C8;
                    mb_strat         : tsp00_MoveObjPtr);
                END;
            (*ENDCASE*) 
 
 
      tgg00_VarColPosList = RECORD
            vpl_last       : tsp00_Int2;
            vpl_last_fill1 : tsp00_Int2;
            vpl_last_fill2 : tsp00_Int4;
            vpl_pos_list   : ARRAY [1..MAX_COL_PER_TAB_GG00] OF tsp00_Int2
      END;
 
 
      tgg00_ClusteredIOParam = RECORD
            clust_bDoClusteredRead_gg00  : boolean;
            clust_Filler1_gg00           : boolean;
            clust_Filler2_gg00           : tsp00_Int2;
            clust_Filler3_gg00           : tsp00_Int4;
            clust_LastUsedIndexPage_gg00 : tsp00_PageNo;
            clust_BufferSize_gg00        : tsp00_Int4;
            clust_pIOBuffer_gg00         : tsp00_Addr;
      END;
 
 
      tgg00_SelectFieldsParam = RECORD
            CASE integer OF
                1:
                    (sfp_addr_fill1     : tsp00_C48; (*  6 * 8 bytes  *)
                    sfp_addr_fill2      : tsp00_C32; (*  4 * 8 bytes  *)
                    sfp_data_size       : tsp00_Int4;
                    sfp_work_st_size    : tsp00_Int4;
                    sfp_workbuf_size    : tsp00_Int4;
                    sfp_workbuf_len     : tsp00_Int4;
                    sfp_act_cntresult   : tsp00_Int4;
                    sfp_ExpIdxPages     : tsp00_Int4;
                    sfp_rows_qual       : tsp00_Int4;
                    sfp_rows_read       : tsp00_Int4;
                    sfp_m_result_size   : tsp00_Int4;
                    sfp_m_result_len    : tsp00_Int4;
                    sfp_m_result_cnt    : tsp00_Int4;
                    sfp_rec_len         : tsp00_Int2;
                    sfp_rec_key_len     : tsp00_Int2;
                    sfp_oldkey_len      : tsp00_Int2;
                    sfp_oldrec_len      : tsp00_Int2;
                    sfp_oldrec_pos      : tsp00_Int2;
                    sfp_result_length   : tsp00_Int2;
                    sfp_output_offset   : tsp00_Int2;
                    sfp_work_st_max     : tsp00_Int2;
                    sfp_resrec_maxlen   : tsp00_Int2;
                    sfp_bd_mess_type    : tgg00_MessType;
                    sfp_bd_mess2_type   : tgg00_MessType2;
                    sfp_filler          : boolean;
                    sfp_check_for_result: boolean; (* PTS 1112079 E.Z. *)
                    sfp_bd_inv_only     : tgg00_QualKind;
                    sfp_bd_return_knf   : boolean;
                    sfp_bd_use_stopkey  : boolean;
                    sfp_first_qual      : boolean;
                    sfp_filler2         : boolean;
                    sfp_result_wanted   : boolean;
                    sfp_varcol_pos      : tgg00_VarColPosList;
                    sfp_first_serial    : tsp00_Number;
                    sfp_last_serial     : tsp00_Number;
                    (* end of init part *)
                    sfp_work_st_top     : tgg00_StEntryAddr;
                    sfp_work_st_bottom  : tgg00_StEntryAddr;
                    sfp_work_st_last    : tgg00_StEntryAddr;
                    sfp_work_st_frame   : tgg00_StEntryAddr;
                    sfp_workbuf_top     : tsp00_Int4;
                    sfp_check_new_rec   : boolean;
                    sfp_optimized       : boolean;
                    sfp_sqlmode         : tsp00_SqlMode;
                    sfp_dateTimeFormat  : tgg00_DateTimeFormat; (* PTS 1115085 *)
                    sfp_acv_addr        : tsp00_Addr;
                    sfp_param_buf_addr  : tsp00_MoveObjPtr;
                    sfp_cluster_info    : tgg00_ClusteredIOParam);
                2:
                    (sfp_data_addr       : tsp00_MoveObjPtr;
                    sfp_rec_addr         : tsp00_BufAddr;
                    sfp_primkey_addr     : tsp00_KeyAddr;
                    sfp_work_st_addr     : tgg00_StackListPtr;
                    sfp_workbuf_addr     : tsp00_MoveObjPtr;
                    sfp_oldrec_addr      : tsp00_MoveObjPtr;
                    sfp_pl_ptr           : tsp00_BufAddr;
                    sfp_m_result_addr    : tsp00_MoveObjPtr;
                    sfp_m_firstkeys_addr : tgg00_TwoKeysPtr;
                    sfp_fieldlistptr     : tgg00_FieldListsPtr);
                3 :
                    (sfp_init_part       : tgg00_SfpInitPart);
                END;
            (*ENDCASE*) 
 
      tgg00_SelectParamPtr = ^tgg00_SelectFieldsParam;
 
      tgg00_DbSpaceInfo = RECORD
            db_total_pages    : tsp00_Int4;
            db_used_pages     : tsp00_Int4;
            db_perm_pages     : tsp00_Int4;
            db_temp_pages     : tsp00_Int4;
            db_perm_percent   : tsp00_Int2;
            db_temp_percent   : tsp00_Int2;
            db_real_perm_used : tsp00_Int4;
            db_updated_pages  : tsp00_Int4;
            db_max_perm_used  : tsp00_Int4;
            db_used_blocks    : tsp00_Int4;
      END;
 
      (* PTS 1003033 E.Z. *)
      (* PTS 1111134 E.Z. *)
 
      tgg00_ReferenceInfo = RECORD
            CASE boolean OF
                true :
                    (data_cache_total          : tsp00_8ByteCounter; (*==== show monitor caches ====*)
                    data_cache_hit             : tsp00_8ByteCounter;
                    data_cache_miss            : tsp00_8ByteCounter;
                    data_cache_percent         : tsp00_8ByteCounter;
                    (* PTS 1115020 E.Z. *)
                    ak_cache_total             : tsp00_8ByteCounter;
                    ak_cache_hit               : tsp00_8ByteCounter;
                    ak_cache_miss              : tsp00_8ByteCounter;
                    ak_cache_percent           : tsp00_8ByteCounter;
                    tseq_cache_total           : tsp00_8ByteCounter;
                    tseq_cache_hit             : tsp00_8ByteCounter;
                    tseq_cache_miss            : tsp00_8ByteCounter;
                    tseq_cache_percent         : tsp00_8ByteCounter;
                    (* PTS 1109469, T.A. 2001-03-22 *)
                    data_sql_cache_total       : tsp00_8ByteCounter;
                    data_sql_cache_hit         : tsp00_8ByteCounter;
                    data_sql_cache_miss        : tsp00_8ByteCounter;
                    data_sql_cache_percent     : tsp00_8ByteCounter;
                    (* PTS 1115043 E.Z. *)
                    data_obj_cache_total       : tsp00_8ByteCounter;
                    data_obj_cache_hit         : tsp00_8ByteCounter;
                    data_obj_cache_miss        : tsp00_8ByteCounter;
                    data_obj_cache_percent     : tsp00_8ByteCounter;
                    data_obj_log_cache_total   : tsp00_8ByteCounter;
                    data_obj_log_cache_hit     : tsp00_8ByteCounter;
                    data_obj_log_cache_miss    : tsp00_8ByteCounter;
                    data_obj_log_cache_percent : tsp00_8ByteCounter;
                    (* PTS 1106423 TS 2000-04-20 *)
                    (* PTS 1111572 E.Z. *)
                    data_history_cache_total    : tsp00_8ByteCounter;
                    data_history_cache_hit      : tsp00_8ByteCounter;
                    data_history_cache_miss     : tsp00_8ByteCounter;
                    data_history_cache_percent  : tsp00_8ByteCounter;
                    (* PTS 1106423 *)
                    (**)
                    sql_commands           : tsp00_8ByteCounter; (*==== show monitor load ====*)
                    prepares               : tsp00_8ByteCounter;
                    prepares_again         : tsp00_8ByteCounter;
                    executes               : tsp00_8ByteCounter;
                    commits                : tsp00_8ByteCounter;
                    rollbacks              : tsp00_8ByteCounter;
                    locks                  : tsp00_8ByteCounter;
                    begin_subtrans         : tsp00_8ByteCounter;
                    end_subtrans           : tsp00_8ByteCounter;
                    rollback_subtrans      : tsp00_8ByteCounter;
                    creates                : tsp00_8ByteCounter;
                    alters                 : tsp00_8ByteCounter;
                    drops                  : tsp00_8ByteCounter;
                    selects                : tsp00_8ByteCounter;
                    selects_rows_read      : tsp00_8ByteCounter;
                    selects_rows_qual      : tsp00_8ByteCounter;
                    inserts                : tsp00_8ByteCounter;
                    inserts_rows           : tsp00_8ByteCounter;
                    updates                : tsp00_8ByteCounter;
                    updates_rows_read      : tsp00_8ByteCounter;
                    updates_rows_upd       : tsp00_8ByteCounter;
                    deletes                : tsp00_8ByteCounter;
                    deletes_rows_read      : tsp00_8ByteCounter;
                    deletes_rows_del       : tsp00_8ByteCounter;
                    internal_dbproc_calls  : tsp00_8ByteCounter;
                    external_dbproc_calls  : tsp00_8ByteCounter;
                    internal_trigger_calls : tsp00_8ByteCounter;
                    external_trigger_calls : tsp00_8ByteCounter;
                    prim_key               : tsp00_8ByteCounter;
                    prim_key_in            : tsp00_8ByteCounter;
                    prim_key_subq          : tsp00_8ByteCounter;
                    prim_key_read          : tsp00_8ByteCounter;
                    prim_key_qual          : tsp00_8ByteCounter;
                    prim_key_range         : tsp00_8ByteCounter;
                    prim_key_range_read    : tsp00_8ByteCounter;
                    prim_key_range_qual    : tsp00_8ByteCounter;
                    index                  : tsp00_8ByteCounter;
                    index_in               : tsp00_8ByteCounter;
                    index_subq             : tsp00_8ByteCounter;
                    index_scan             : tsp00_8ByteCounter;
                    index_read             : tsp00_8ByteCounter;
                    index_qual             : tsp00_8ByteCounter;
                    index_range            : tsp00_8ByteCounter;
                    index_range_read       : tsp00_8ByteCounter;
                    index_range_qual       : tsp00_8ByteCounter;
                    is_index               : tsp00_8ByteCounter;
                    is_index_in            : tsp00_8ByteCounter;
                    is_index_subq          : tsp00_8ByteCounter;
                    is_index_read          : tsp00_8ByteCounter;
                    is_index_qual          : tsp00_8ByteCounter;
                    is_index_range         : tsp00_8ByteCounter;
                    is_index_range_read    : tsp00_8ByteCounter;
                    is_index_range_qual    : tsp00_8ByteCounter;
                    table_scan             : tsp00_8ByteCounter;
                    table_scan_read        : tsp00_8ByteCounter;
                    table_scan_qual        : tsp00_8ByteCounter;
                    is_index_scan          : tsp00_8ByteCounter;
                    is_index_scan_read     : tsp00_8ByteCounter;
                    is_index_scan_qual     : tsp00_8ByteCounter;
                    mem_sorts              : tsp00_8ByteCounter;
                    mem_sorts_rows_read    : tsp00_8ByteCounter;
                    ins_sorts              : tsp00_8ByteCounter;
                    ins_sorts_rows_ins     : tsp00_8ByteCounter;
                    join_hash              : tsp00_8ByteCounter;
                    join_parallel          : tsp00_8ByteCounter;
                    join_legacy            : tsp00_8ByteCounter;
                    join_op_normal         : tsp00_8ByteCounter;
                    join_op_improved       : tsp00_8ByteCounter;
                    (**)
                    avg_lock_used          : tsp00_8ByteCounter; (*==== show monitor lock ====*)
                    max_lock_used          : tsp00_8ByteCounter;
                    collisions             : tsp00_8ByteCounter;
                    escalations            : tsp00_8ByteCounter;
                    new_rows               : tsp00_8ByteCounter;
                    new_tabs               : tsp00_8ByteCounter;
                    deadlocks              : tsp00_8ByteCounter;
                    request_timeouts       : tsp00_8ByteCounter;
                    (**)
                    read_log               : tsp00_8ByteCounter; (*==== show monitor log ====*)
                    write_log              : tsp00_8ByteCounter;
                    kb1_monitor            : ARRAY [1..8] OF tsp00_8ByteCounter;
                    (**)
                    read_string            : tsp00_8ByteCounter; (*==== show monitor long ====*)
                    write_string           : tsp00_8ByteCounter;
                    (**)
                    r_v_acc                : tsp00_8ByteCounter; (*==== show monitor pages ====*)
                    w_v_acc                : tsp00_8ByteCounter;
                    r_p_acc                : tsp00_8ByteCounter;
                    w_p_acc                : tsp00_8ByteCounter;
                    catalog_r_v            : tsp00_8ByteCounter;
                    catalog_w_v            : tsp00_8ByteCounter;
                    catalog_r_p            : tsp00_8ByteCounter;
                    catalog_w_p            : tsp00_8ByteCounter;
                    (* PTS 1115020 E.Z. *)
                    perm_r_v               : tsp00_8ByteCounter;
                    perm_w_v               : tsp00_8ByteCounter;
                    perm_r_p               : tsp00_8ByteCounter;
                    perm_w_p               : tsp00_8ByteCounter;
                    temp_r_v               : tsp00_8ByteCounter;
                    temp_w_v               : tsp00_8ByteCounter;
                    temp_r_p               : tsp00_8ByteCounter;
                    temp_w_p               : tsp00_8ByteCounter;
                    bstr_r_v               : tsp00_8ByteCounter;
                    bstr_w_v               : tsp00_8ByteCounter;
                    bstr_r_p               : tsp00_8ByteCounter;
                    bstr_w_p               : tsp00_8ByteCounter;
                    level0_r_v             : tsp00_8ByteCounter;
                    level0_w_v             : tsp00_8ByteCounter;
                    level0_r_p             : tsp00_8ByteCounter;
                    level0_w_p             : tsp00_8ByteCounter;
                    level1_r_v             : tsp00_8ByteCounter;
                    level1_w_v             : tsp00_8ByteCounter;
                    level1_r_p             : tsp00_8ByteCounter;
                    level1_w_p             : tsp00_8ByteCounter;
                    level2_r_v             : tsp00_8ByteCounter;
                    level2_w_v             : tsp00_8ByteCounter;
                    level2_r_p             : tsp00_8ByteCounter;
                    level2_w_p             : tsp00_8ByteCounter;
                    level3_r_v             : tsp00_8ByteCounter;
                    level3_w_v             : tsp00_8ByteCounter;
                    level3_r_p             : tsp00_8ByteCounter;
                    level3_w_p             : tsp00_8ByteCounter;
                    level4_r_v             : tsp00_8ByteCounter;
                    level4_w_v             : tsp00_8ByteCounter;
                    level4_r_p             : tsp00_8ByteCounter;
                    level4_w_p             : tsp00_8ByteCounter;
                    level5_r_v             : tsp00_8ByteCounter;
                    level5_w_v             : tsp00_8ByteCounter;
                    level5_r_p             : tsp00_8ByteCounter;
                    level5_w_p             : tsp00_8ByteCounter;
                    psm_r_v                : tsp00_8ByteCounter;
                    psm_w_v                : tsp00_8ByteCounter;
                    psm_r_p                : tsp00_8ByteCounter;
                    psm_w_p                : tsp00_8ByteCounter;
                    (**)
                    add_rec                : tsp00_8ByteCounter; (*==== show monitor row ====*)
                    cadd_rec               : tsp00_8ByteCounter;
                    repl_rec               : tsp00_8ByteCounter;
                    crepl_rec              : tsp00_8ByteCounter;
                    del_rec                : tsp00_8ByteCounter;
                    cdel_rec               : tsp00_8ByteCounter;
                    get_rec                : tsp00_8ByteCounter;
                    cget_rec               : tsp00_8ByteCounter;
                    next_rec               : tsp00_8ByteCounter;
                    cnext_rec              : tsp00_8ByteCounter;
                    prev_rec               : tsp00_8ByteCounter;
                    cprev_rec              : tsp00_8ByteCounter;
                    sel_direc              : tsp00_8ByteCounter;
                    sel_next               : tsp00_8ByteCounter;
                    sel_prev               : tsp00_8ByteCounter;
                    add_inv                : tsp00_8ByteCounter;
                    cadd_inv               : tsp00_8ByteCounter;
                    del_inv                : tsp00_8ByteCounter;
                    cdel_inv               : tsp00_8ByteCounter;
                    get_inv                : tsp00_8ByteCounter;
                    get_inv_parallel       : tsp00_8ByteCounter;
                    cget_inv               : tsp00_8ByteCounter;
                    (**)
                    funccount              : tsp00_8ByteCounter; (*==== show monitor transaction ====*)
                    transcount             : tsp00_8ByteCounter;
                    kb_calls               : tsp00_8ByteCounter);
                false :
                    (reference_array : ARRAY [tgg00_RefInfoIndex] OF tsp00_8ByteCounter);
                END;
            (*ENDCASE*) 
 
 
      tgg00_LogPos = RECORD
            CASE integer OF
                1:
                    (logLpno_gg00     : tsp00_PageNo;
                    logPageType_gg00  : tgg00_PageType;
                    logPageType2_gg00 : tgg00_PageType2;
                    logFiller_gg00    : tsp00_C2;
                    logPpos_gg00      : tgg00_PagePos);
                2:
                    (logHeader_gg00   : tgg00_PageHeader)
                END;
            (*ENDCASE*) 
            (*******************************************************)
            (******************* SELECT - TYPEN ********************)
            (*******************************************************)
 
 
      tgg00_FieldDescrRec = RECORD
            field_count     : tsp00_Int4;
            field_rec_pos   : tsp00_Int2;
            field_rec_len   : tsp00_Int2;
            result_out_pos  : tsp00_Int2;
            result_out_len  : tsp00_Int2;
            field_is_descen : boolean;
            field_function  : tgg00_StackOpFunc;
            field_sum_flag  : boolean;
            field_filler    : boolean;
      END;
 
 
      tgg00_AggrDescrRec = RECORD
            agg_cnt          : tsp00_Int2;
            agg_filler       : boolean;
            agg_opt_infos    : tgg00_AggSet;
            agg_field_descr  : ARRAY [1..MAX_COL_SEQUENCE_GG00] OF
                  tgg00_FieldDescrRec;
            agg_prim_key_cnt : tsp00_Int4;
            agg_last_sec_key : tgg00_Lkey;
      END;
 
      (*******************************************************)
      (************** SELECT - TYPEN _ ENDE ******************)
      (*******************************************************)
      (**************************************)
      (*    VGG01 global variables *)
      (**************************************)
      tgg00_InstanceType =
            (itUNKNOWN_egg00,
            itOLTP_egg00,
            itLVC_egg00,
            it_old_BW_egg00,
            it_old_CS_egg00,
            it_old_EMERGE_egg00,
            itARCHIVE_egg00); (* PTS 1117376 E.Z. *)
 
      tgg00_KernelGlobals = RECORD
            timezone                      : tsp00_Int4;
            vf_bufpool                    : tsp00_Int4;
            (* *)
            sysuser_id                    : tgg00_Surrogate;
            (* *)
            rescnt_unknown                : tsp00_ResNum;
            datacachecheck                : boolean;
            (* *)
            rescnt_1                      : tsp00_ResNum;
            sysmsg_may_exist              : boolean;
            (* *)
            rescnt_0                      : tsp00_ResNum;
            db_is_for_sapr3               : boolean;
            (* *)
            sysuser_name                  : tsp00_KnlIdentifier;
            (* *)
            treecheck                     : boolean;
            bd_lock_check                 : boolean;
            bd_subtree                    : boolean;
            kglInstanceType_gg00          : tgg00_InstanceType;
            filler3                       : boolean;
            undef_buf                     : tsp00_C1;
            filler4                       : tsp00_Int2;
            (* *)
            undef_addr                    : tsp00_MoveObjPtr;
      END;
 
 
      tgg00_VtraceState = RECORD
            vtrAk_gg00            : boolean;
            vtrAkDelete_gg00      : boolean;
            vtrAkInsert_gg00      : boolean;
            vtrAkPacket_gg00      : boolean;
            vtrAkSelect_gg00      : boolean;
            vtrAkShortPacket_gg00 : boolean;
            vtrAkUpdate_gg00      : boolean;
            vtrAll_gg00           : boolean;
            (* *)
            vtrAny_gg00           : boolean;
            vtrBdIndex_gg00       : boolean;
            vtrBdPrim_gg00        : boolean;
            vtrBdString_gg00      : boolean; (* PTS 1107617 E.Z. *)
            vtrIoTrace_gg00       : boolean;
            vtrKbLock_gg00        : boolean;
            vtrStrategy_gg00      : boolean;
            vtrTime_gg00          : boolean;
            (* *)
            vtrGateway_gg00       : boolean;
            vtrBdObject_gg00      : boolean;                (* PTS 1103799 JA 1999-08-30 *)
            vtrOmsNew_gg00        : boolean;
            vtrOmsGet_gg00        : boolean;
            vtrOmsUpd_gg00        : boolean;
            vtrOmsFree_gg00       : boolean;
            vtrFiller1_gg00       : tsp00_Int2;  (* PTS 1107617 E.Z. *)
            (* *)
            vtrCheck_gg00         : boolean;
            vtrRetcodeCheck_gg00  : boolean;
            vtrStopRetcode_gg00   : tsp00_Int2;
            vtrSession_gg00       : tgg91_SessionNo;
      END;
 
 
      tgg00_VfFileref = RECORD
            no      : tsp00_Int4;
            buf     : tsp00_VfBufaddr;
            poolptr : tsp00_Int4;
            filler  : tsp00_Int2;
            buf_cnt : tsp00_Int2
      END;
 
 
      tgg00_BdSetResultRecord = RECORD
            bd_key_check_len : tsp00_Int4;
            bd_max_rec_cnt   : tsp00_Int4;
            bd_rec_cnt       : tsp00_Int4;
            bd_max_fill_len  : tsp00_Int4;
            bd_fill_len      : tsp00_Int4;
            bd_next          : boolean;
            bd_drop_page     : boolean;
            bd_filler1       : tsp00_Int2
      END;
 
