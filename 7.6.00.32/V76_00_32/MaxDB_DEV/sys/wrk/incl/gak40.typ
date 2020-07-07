
      tak40_int2_arr = ARRAY [ 1..mxak40_shinfo ] OF tsp00_Int2;
      tak40_int2_arr_ptr = ^tak40_int2_arr;
      
      tak40_shinfo  =  PACKED ARRAY [ 1..mxak40_shinfo ] OF char;
 
      tak40_show_record  =  RECORD
            shsl            : tsp00_Int2;
            shkl            : tsp00_Int2;
            shvarcol_offset : tsp00_Int2;
            shvarcol_cnt    : tsp00_Int2;
            shkey           : tsp00_C4;
            shinfo          : tak40_shinfo;
      END;
 
 
      tak40_show_glob  =  RECORD
            a4sysbuf       : tak_sysbufferaddress;
            a4col_ptr      : tak00_colinfo_ptr;
            a4dombuf       : tak_sysbufferaddress;
            a4commandId    : tsp00_Int4;
            a4ti           : tsp00_Int4;
            a4ci           : tsp00_Int4;
            a4pos          : tsp00_Int4;
            a4index        : tsp00_Int4;
            a4id_len       : tsp00_Int4;
            a4sh_key       : tsp00_C4;
			a4LongKey      : tsp00_Int4;  (* PTS 1113491 *) 
            a4p_arr        : tak_syspointerarr;
            a4authname     : tsp00_KnlIdentifier;
            a4tablen       : tsp00_KnlIdentifier;
            a4user         : tsp00_KnlIdentifier;
            a4coln         : tsp00_KnlIdentifier;
            a4sysk         : tgg00_SysInfoKey;
            a4showfile_name: tsp00_KnlIdentifier;
            a4sh_kind      : tgg00_ShowKind;
            a4priv_check   : boolean;
            a4priv         : tak00_PrivilegeSet;
            a4is_synonym   : boolean;
            a4del_basetab  : boolean;    (* PTS 1106617 E.Z. *)
            a4tab_count    : tsp00_Int2;
            a4treeid       : tgg00_FileId;
			a4LongContainer: tgg00_FileId; (* PTS 1113491 *) 
            a4sh_buf       : tak40_show_record;
            a4col_len      : tsp00_Int2;
            a4tab_len      : tsp00_Int2;
            a4error_pos    : tsp00_Int4;
            a4synname      : tsp00_KnlIdentifier;
            a4systab_select: boolean;     (* false in case of explain, some ddl *)
            a4unique       : boolean;
            a4temp         : boolean;
            a4private      : boolean;
            a4nonprivate   : boolean;
            a4public       : boolean;
            a4syns         : boolean;
            a4all          : boolean;
            a4is_systable  : boolean;
            a4sys_tables   : boolean;
            a4use_base     : boolean;
            a4use_view     : boolean;
            a4usertab_pos  : tsp00_Int2;
            a4usertab_cnt  : tsp00_Int2;
            a4mfetch_pos   : tsp00_Int2;
            a4mfetch_cnt   : tsp00_Int2;
            a4tree_pos     : tgg00_FilePos;
            a4usertab_buf  : tak_sysbufferaddress;
            a4next_key     : tgg00_SysInfoKey;
            a4b_err        : tgg00_BasisError;
            a4next_owner   : tsp00_KnlIdentifier;
            a4next_tablen  : tsp00_KnlIdentifier;
            a4next_user    : tgg00_Surrogate;
            a4owner_id     : tgg00_Surrogate;
            a4data         : tsp00_Buf;
            a4mblock       : tgg00_MessBlock;
            a4strategy     : tgg07_StrategyInfo;
            a4sel          : tgg00_SelectParamPtr;
            a4sel_param    : tgg07_select_context_ptr;
            a4get_param    : tgg07_get_param_ptr;
            a4long_desc_len: tsp00_Int2;
            a4res_buf_ptr  : tgg00_DataPartPtr;
            a4mess_code    : tsp00_CodeType;
            a4row_ptr      : ^tak40_show_record;
            a4acv_ptr      : tak_acv_address;
            a4all_objects  : boolean;
            a4roles        : boolean;
            a4role_cnt     : tsp00_Int2;
            a4role_ids     : ARRAY[1..8] OF tgg00_Surrogate;
            a4aux_buf_ptr  : tak40_int2_arr_ptr;
      END;
 
      tak40_show_glob_ptr = ^tak40_show_glob;     (* PTS 1108435 JA 2000-11-27 *)
 
