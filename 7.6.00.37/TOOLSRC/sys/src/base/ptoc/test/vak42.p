

CONST

      csp_cop1 = ' *** copyright by SAP AG, 1998 - all rights     ';
      csp_cop2 = ' reserved - program property of SAP AG ***      ';
      csp_comlint           =
            '\CA\21\47\48\36\48\00\00\00\00\00\00\00\00\00\00\00\00\00\00';
      csp_maxlint           =
            '\CA\21\47\48\36\47\00\00\00\00\00\00\00\00\00\00\00\00\00\00';
      csp_minlint           =
            '\36\78\52\51\63\52\00\00\00\00\00\00\00\00\00\00\00\00\00\00';
      csp_comsint           =
            '\C5\32\76\80\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00';
      csp_maxsint           =
            '\C5\32\76\70\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00';
      csp_minsint           =
            '\3B\67\23\20\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00';
      csp_comluns           =
            '\CA\42\94\96\72\96\00\00\00\00\00\00\00\00\00\00\00\00\00\00';
      csp_maxluns           =
            '\CA\42\94\96\72\95\00\00\00\00\00\00\00\00\00\00\00\00\00\00';
      csp_minluns           =
            '\80\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00';
      csp_comsuns           =
            '\C5\65\53\60\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00';
      csp_maxsuns           =
            '\C5\65\53\50\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00';
      csp_minsuns           =
            '\80\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00';
      csp_maxsrel           =
            '\E7\34\02\82\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00';
      csp_minsrel           =
            '\9B\11\75\49\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00';
      csp_null_number       =
            '\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00';
      csp_min_number        =
            '\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00';
      csp_max_number        =
            '\FF\99\99\99\99\99\99\99\99\99\99\99\99\99\99\99\99\99\99\99';
      csp_rescnt_unknown    = '\00\3F\90\00\00\00\00';
      csp_rescnt_one        = '\00\C1\10\00\00\00\00';
      csp_rescnt_zero       = '\00\80\00\00\00\00\00';
      csp_zero_exponent     = '\80';
      csp_zero_exp_value    =   128;
      csp_value_pi          =
            '\C1\31\41\59\26\53\58\97\93\23\84\62\64\33\83\27\95\02\88\41';
      csp_pw_undef          = $00000000;
      (* constants for VSP51 *)
      csp_one_number        =
            '\C1\10\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00';
      csp_epsilon_number    =
            '\B1\10\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00';
      csp_a_half_number     =
            '\C0\50\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00';
      csp_two_number        =
            '\C1\20\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00';
      (* end VSP51 *)
      (* constants for VKB78 *)
      csp_number256         =
            '\C3\25\60\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00';
      (* end VKB78 *)
      (**)
      (* About Box ID's *)
      csp_dlg_aboutbox         = 100;
      csp_dlgabout_sysname     = 100;
      csp_dlgabout_cname       = 101;
      csp_dlgabout_relno       = 102;
      csp_dlgabout_dbasename   = 103;
      csp_dlgabout_username    = 104;
      csp_switch_lang_changed  = 200;
      (* Common Dialog Boxes *)
      csp_dlg_fileopenbox      = 200;
      csp_dlg_filesavebox      = 210;
      csp_dlg_printbox         = 220;
      csp_dlg_printersetupbox  = 230;
      csp_dlg_editfindbox      = 240;
      csp_dlg_editreplacebox   = 250;
      (* Error Boxes *)
      csp_dlg_fatal            = 999;
      csp_dlg_error            = 998;
      csp_dlg_warning          = 997;
      csp_dlg_info             = 996;
      (**)
      csp_star1                    = '\1F';  (*  in like '*', '%'  *)
      csp_any1                     = '\1E';  (*  in like '?', '_'  *)
      csp_cclass                   = '\1D';  (*  in like '(,)'     *)
      csp_crange                   = '\1C';  (*  in like '-'       *)
      csp_cnot                     = '\19';  (*  in like 'not'     *)
      csp_ascii_zero               = '\30';
      csp_ascii_blank              = '\20';
      csp_ascii_star               = '\2A';
      csp_ascii_question_mark      = '\3F';
      csp_ascii_underline          = '\5F';
      csp_ascii_percent            = '\25';
      csp_ascii_open_bracket       = '\28';
      csp_ascii_close_bracket      = '\29';
      csp_ascii_quote              = '\27';
      csp_ascii_double_quote       = '\22';
      csp_ascii_not                = '\AC';
      csp_ascii_tilde              = '\7E';
      csp_ascii_hyphen             = '\2D';
      csp_ebcdic_zero              = '\F0';
      csp_ebcdic_blank             = '\40';
      csp_ebcdic_star              = '\5C';
      csp_ebcdic_question_mark     = '\6F';
      csp_ebcdic_underline         = '\6D';
      csp_ebcdic_percent           = '\6C';
      csp_ebcdic_open_bracket      = '\4D';
      csp_ebcdic_close_bracket     = '\5D';
      csp_ebcdic_quote             = '\7D';
      csp_ebcdic_double_quote      = '\7F';
      csp_ebcdic_not               = '\5F';
      csp_ebcdic_tilde             = '\B0';
      csp_ebcdic_hyphen            = '\60';
      csp_defined_byte             = '\00';
      csp_unicode_blank            = '\00\20';
      csp_unicode_def_byte         = '\01';
      csp_unicode_mark             = '\00';
      csp_undef_byte               = '\FF';
      csp_oflw_byte                = '\FE';
      csp_default_byte             = '\FD';
      csp_long_desc_def_byte       = '\FC';
      (**)
      csp_info_fixed           = '\00';
      csp_info_float           = '\01';
      csp_info_char            = '\02';
      csp_info_byte            = '\03';
      csp_info_date            = '\04';
      csp_info_time            = '\05';
      csp_info_string          = '\06';
      csp_info_expression      = '\07';
      csp_info_timestamp       = '\08';
      csp_info_dbyte_ascii     = '\09';
      csp_info_dbyte_ebcdic    = '\0A';
      csp_info_c_oldlong_char       = '\0B';
      csp_info_b_oldlong_byte       = '\0C';
      csp_info_a_oldlong_ascii_dbyte       = '\0D';
      csp_info_e_oldlong_ebcdic_dbyte      = '\0E';
      csp_info_c_long_char         = '\13';
      csp_info_b_long_byte         = '\14';
      csp_info_a_long_ascii_dbyte  = '\15';
      csp_info_e_long_ebcdic_dbyte = '\16';
      csp_info_smallint     = '\0F';
      csp_info_integer      = '\10';
      csp_info_varchar      = '\11';
      csp_info_escape_char  = '\12';
      csp_info_boolean      = '\17';
      csp_info_otherwise    = '\18';
      csp_info_mandatory    = '\00';
      csp_info_optional     = '\01';
      csp_info_default      = '\02';
      csp_info_input        = '\00';
      csp_info_output       = '\01';
      csp_info_inout        = '\02';
      (**)
      csp_kanji_out1        = '\0E';
      csp_kanji_out2        = '\28';
      csp_kanji_out3        = '\38';
      csp_kanji_in1         = '\0F';
      csp_kanji_in2         = '\29';
      (**)
      csp_ascii         = 0;
      csp_ebcdic        = 1;
      csp_codeneutral   = 2;
      csp_unicode_swap   = 19;
      csp_unicode        = 20;
      csp_unicode_native = 21;
      csp_utf8           = 22;
      (**)
      bsp_c1            = ' ';
      bsp_c5            = '     ';
      bsp_c6            = '      ';
      bsp_c8            = '        ';
      bsp_c10           = '          ';
      bsp_c11           = '           ';
      bsp_c12           = '            ';
      bsp_c16           = '                ';
      bsp_c18           = '                  ';
      bsp_c20           = '                    ';
      bsp_c24           = '                        ';
      bsp_c30           = '                              ';
      bsp_c32           = '                                ';
      bsp_c34           = '                                  ';
      bsp_c40           = '                                        ';
      bsp_c42           =
            '                                          ';
      bsp_c48           =
            '                                                ';
      bsp_c64           =
            '                                                                ';
      bsp_identifier     = bsp_c18;
      bsp_knl_identifier = bsp_c64;
      bsp_date           = bsp_c8;
      bsp_dbname         = bsp_c18;
      bsp_devname        = bsp_c40;
      bsp_errtext        = bsp_c40;
      bsp_lname          = bsp_c32;
      bsp_name           = bsp_c18;
      bsp_nodeid         = bsp_c64;
      bsp_opmsg          = bsp_c64;
      bsp_sname          = bsp_c12;
      bsp_termid         = bsp_c18;
      bsp_time           = bsp_c8;
      bsp_vfilename      = bsp_c64;
      (*--------------------------------------------------------------*)
      UNDEF_SP00         =         -1;
      MIN_UINT1_SP00     =          0;
      MAX_UINT1_SP00     =        255;
      MIN_UINT2_SP00     =          0; (* +++ do not use in PASCAL *)
      MAX_UINT2_SP00     =      65535; (* +++ do not use in PASCAL *)
      MIN_INT2_SP00      =     -32768;
      MAX_INT2_SP00      =      32767;
      MAX_INT4_SP00      = 2147483647;
      (**)
      csp_max_fields          =       1024; (* ak - constant *)
      csp_max_indices         =         16; (* ak - constant *)
      csp_maxvariables        =       2000; (* shall be 3000, but *)
      (* because cache can not handle buffer with more than 32KB   *)
      (* (and maxvariables+25) * 16 + something has to fit...      *)
      (**)
      csp_cntcachebuf         =         11;
      csp_cryptname           =          6; (* tsp00_CryptName *)
      csp_fixed               =         38;
      csp_float_frac          =         -1;
      csp_numstack            =         16;
      csp_old_fileversion     =         -8;
      csp_pool_buffers        =         16;
      csp_resnum_deflen       =         10;
      csp_sfield_info         =       1028;
      csp_splitted_commands   =        -10;
      csp_stringlength        =       8084;
      csp_use_new_pars_info   =         -9;
      (**)
      TASKS_SP00                =        256;
      (**)
      ADDR_MXSP00               =          4;
      ANY_PACKED_CHAR_MXSP00    =      65535;
      BUF_MXSP00                =       8192;
      BUF8K_MXSP00              =       8192;
      COMPNAME_MXSP00           =         64;
      CP_COMMANDPRE_VARS_MXSP00 =       1740;
      CRYPTPW_MXSP00            =         24;
      CTABLE_MXSP00             =        256;
      DATE_MXSP00               =          8;
      DBNAME_MXSP00             =         18;
      PATH_MXSP00               =        256;
      DECIMAL_MXSP00            =         20;
      DEVNAME_MXSP00            =         40;
      DP_CODE_CACHE_MXSP00      =      32760; (* >= tdg_code_cache *)
      DP_DIALOG_GLOBAL_MXSP00   =       5500; (* >= tdg_dialog_global *)
      DP_DIAPRE_VARS_MXSP00     =       5000; (* >= tdg_diapre_variable_infos*)
      DP_I_GLOBAL_MXSP00        =      24000; (* >= tdg_i_global *)
      DP_PRECOMP_DCA_MXSP00     =        600; (* >= tdg_dca *)
      DP_VALUE_CACHE_MXSP00     =      32760; (* >= tdg_value_cache *)
      ERRTEXT_MXSP00            =         40;
      EXEC_ARGLINE_MXSP00       =        512;
      EXEC_LINE_MXSP00          =        512;
      EXEC_PROG_MXSP00          =         64;
      EXT_DATE_MXSP00           =         10;
      EXT_TIME_MXSP00           =          8;
      EXT_TIMESTAMP_MXSP00      =         26;
      INT2_MXSP00               =          2;
      INT4_MXSP00               =          4;
      KEY_MXSP00                =       1024;
      LINE_MXSP00               =        132;
      LINELENGTH_MXSP00         =      32767;
      LNAME_MXSP00              =         32;
      LONG_DESC_MXSP00          =         40;
      LONG_DESC_BLOCK_MXSP00    =         56;
      MICROSEC_MXSP00           =          6;
      MOVEINT2_MXSP00           =    4096000; (* tsp00_MoveObj / 2 *)
      MOVEOBJ_MXSP00            =    8192000; (* tsp00_MoveObj     *)
      NAME_MXSP00               =         18;
      NODEID_MXSP00             =         64;
      NUMBER_MXSP00             =         20;
      NUMSTR_MXSP00             =         64;
      OPMSG_MXSP00              =         64;
      PAGE_MXSP00               =       8192;
      PRTLINE_MXSP00            =        256;
      PRTNAME_MXSP00            =         64;
      PW_MXSP00                 =         18;
      REGION_MXSP00             =          8;
      RESNUM_MXSP00             =          7; (* ResnumDeflen+1)div2+2*)
      RP_REPORTPRE_VARS_MXSP00  =       5708;
      RTE_COMM_MXSP00           =         12;
      SCREEN_BUF_MXSP00         =       8488;
      SCREEN_CHARS_MXSP00       =      32767;
      SFIELD_INFO_MXSP00        =          8;
      SCREEN_LINES_MXSP00       =      32767;
      SNAME_MXSP00              =         12;
      SQLSTATE_MXSP00           =          5;
      TERMID_MXSP00             =         18;
      TIME_MXSP00               =          8;
      TIMESTAMP_MXSP00          =         20;
      TRACE_MXSP00              =         16;
      UINT2_MXSP00              =          2;
      UNLOAD_MXSP00             =        264;
      VERSION_MXSP00            =         40;
      VFILENAME_MXSP00          =         64;
      VTLABEL_MXSP00            =          8;
      VTMSG_MXSP00              =         80;
      XUSERKEY_MXSP00           =         18;
      ZONED_MXSP00              =         20;
      (* ******* virtual terminal ******* *)
      csp_attr_byte          =         1; (*attr.-byte in front of each col*)
      csp_vt_eof             =         0; (* end_of_field                  *)
      csp_vt_soncf           =         1; (* start non-continuation field  *)
      csp_vt_socf            =         2; (* start of continuation field   *)
      csp_clear_before       =      true; (* clear screen                  *)
      csp_without_clear      =     false; (* overwrite screen              *)
      csp_lslabel            =         8;
      csp_ls_number          =         9;
      csp_sk_number          =         9;
      csp_sk_lines           =         2;
      csp_pc_screen_cols     =        80;
      (* graphic mode constants *)
      csp_vertical_bar       =     '\08';
      csp_l_right_corner     =     '\0B';
      csp_u_right_corner     =     '\0C';
      csp_l_left_corner      =     '\10';
      csp_u_left_corner      =     '\11';
      csp_left_t             =     '\12';
      csp_right_t            =     '\13';
      csp_horizontal_bar     =     '\14';
      csp_bottom_t           =     '\1C';
      csp_top_t              =     '\1D';
      csp_crossing_lines     =     '\1E';
      (* Senderid-constants of the separate Components *)
      csp_comp_cpc        = 'CPC';
      csp_comp_cpp        = 'CPP'; (* SQLClass PTS 1112481 FF *)
      csp_comp_db_manager = 'DBM';
      csp_comp_gpc        = 'GPC'; (* Generic Programming Client: Python ... *)
      csp_comp_jdbc       = 'JDB';
      csp_comp_load       = 'LOA';
      csp_comp_odbc       = 'ODB';
      csp_comp_util       = 'UTI'; (* Repair-Tool *)
      csp_comp_xci        = 'XCI';
      (* max amount of parameters of a DCOM procedure *)
      cak260max_params       = 256;
      cak260max_fparams      = 8;           (* max Float parameter *)
      (**)
      (* ----- OLD CONSTANTS ----- *)
      csp_minint1               = MIN_UINT1_SP00;
      csp_maxint1               = MAX_UINT1_SP00;
      csp_minint2               = MIN_INT2_SP00;
      csp_maxint2               = MAX_INT2_SP00;
      csp_maxint4               = MAX_INT4_SP00;
      (**)
      mxsp_c1                  =          1;
      mxsp_c2                  =          2;
      mxsp_c3                  =          3;
      mxsp_c4                  =          4;
      mxsp_c5                  =          5;
      mxsp_c6                  =          6;
      mxsp_c8                  =          8;
      mxsp_c10                 =         10;
      mxsp_c12                 =         12;
      mxsp_c14                 =         14;
      mxsp_c16                 =         16;
      mxsp_c18                 =         18;
      mxsp_c20                 =         20;
      mxsp_c24                 =         24;
      mxsp_c30                 =         30;
      mxsp_c40                 =         40;
      mxsp_c50                 =         50;
      mxsp_c60                 =         60;
      mxsp_c64                 =         64;
      mxsp_c70                 =         70;
      mxsp_c80                 =         80;
      mxsp_c256                =        256;
      (**)
      mxsp_addr                = ADDR_MXSP00;
      mxsp_any_packed_char     = ANY_PACKED_CHAR_MXSP00;
      mxsp_buf                 = BUF_MXSP00;
      mxsp_8kbuf               = BUF8K_MXSP00;
      mxsp_compname            = COMPNAME_MXSP00;
      mxsp_cp_commandpre_vars  = CP_COMMANDPRE_VARS_MXSP00;
      mxsp_cryptpw             = CRYPTPW_MXSP00;
      mxsp_ctable              = CTABLE_MXSP00;
      mxsp_date                = DATE_MXSP00;
      mxsp_dbname              = DBNAME_MXSP00;
      mxsp_decimal             = DECIMAL_MXSP00;
      mxsp_devname             = DEVNAME_MXSP00;
      mxsp_dp_code_cache       = DP_CODE_CACHE_MXSP00;
      mxsp_dp_dialog_global    = DP_DIALOG_GLOBAL_MXSP00;
      mxsp_dp_diapre_vars      = DP_DIAPRE_VARS_MXSP00;
      mxsp_dp_i_global         = DP_I_GLOBAL_MXSP00;
      mxsp_dp_precomp_dca      = DP_PRECOMP_DCA_MXSP00;
      mxsp_dp_value_cache      = DP_VALUE_CACHE_MXSP00;
      mxsp_errtext             = ERRTEXT_MXSP00;
      mxsp_execargline         = EXEC_ARGLINE_MXSP00;
      mxsp_execline            = EXEC_LINE_MXSP00;
      mxsp_execprog            = EXEC_PROG_MXSP00;
      mxsp_extdate             = EXT_DATE_MXSP00;
      mxsp_exttime             = EXT_TIME_MXSP00;
      mxsp_exttimestamp        = EXT_TIMESTAMP_MXSP00;
      mxsp_int2                = INT2_MXSP00;
      mxsp_int4                = INT4_MXSP00;
      mxsp_key                 = KEY_MXSP00;
      mxsp_line                = LINE_MXSP00;
      mxsp_linelength          = LINELENGTH_MXSP00;
      mxsp_lname               = LNAME_MXSP00;
      mxsp_long_desc           = LONG_DESC_MXSP00;
      mxsp_long_desc_block     = LONG_DESC_BLOCK_MXSP00;
      mxsp_microsec            = MICROSEC_MXSP00;
      mxsp_moveobj             = MOVEOBJ_MXSP00;
      mxsp_moveint2            = MOVEINT2_MXSP00;
      mxsp_name                = NAME_MXSP00;
      mxsp_nodeid              = NODEID_MXSP00;
      mxsp_number              = NUMBER_MXSP00;
      mxsp_numstr              = NUMSTR_MXSP00;
      mxsp_opmsg               = OPMSG_MXSP00;
      mxsp_page                = PAGE_MXSP00;
      mxsp_prtline             = PRTLINE_MXSP00;
      mxsp_prtname             = PRTNAME_MXSP00;
      mxsp_pw                  = PW_MXSP00;
      mxsp_region              = REGION_MXSP00;
      mxsp_resnum              = RESNUM_MXSP00;
      mxsp_rp_reportpre_vars   = RP_REPORTPRE_VARS_MXSP00;
      mxsp_rte_comm            = RTE_COMM_MXSP00;
      mxsp_screenbuf           = SCREEN_BUF_MXSP00;
      mxsp_screen_chars        = SCREEN_CHARS_MXSP00;
      mxsp_screen_lines        = SCREEN_LINES_MXSP00;
      mxsp_sfield_info         = SFIELD_INFO_MXSP00;
      mxsp_sname               = SNAME_MXSP00;
      mxsp_sqlstate            = SQLSTATE_MXSP00;
      mxsp_termid              = TERMID_MXSP00;
      mxsp_time                = TIME_MXSP00;
      mxsp_timestamp           = TIMESTAMP_MXSP00;
      mxsp_trace               = TRACE_MXSP00;
      mxsp_uint2               = UINT2_MXSP00;
      mxsp_unload              = UNLOAD_MXSP00;
      mxsp_version             = VERSION_MXSP00;
      mxsp_vfilename           = VFILENAME_MXSP00;
      mxsp_vtlabel             = VTLABEL_MXSP00;
      mxsp_vtmsg               = VTMSG_MXSP00;
      mxsp_xuserkey            = XUSERKEY_MXSP00;
      mxsp_zoned               = ZONED_MXSP00;


      (* function codes as in ORACLE Call Interface *)
      csp1_nil_fc                     =   0;
      csp1_create_table_fc            =   1;
      csp1_set_role_fc                =   2;  (* unused *)
      csp1_insert_fc                  =   3;
      csp1_select_fc                  =   4;
      csp1_update_fc                  =   5;
      csp1_drop_role_fc               =   6;  (* unused *)
      csp1_drop_view_fc               =   7;
      csp1_drop_table_fc              =   8;
      csp1_delete_fc                  =   9;
      csp1_create_view_fc             =  10;
      csp1_drop_user_fc               =  11;
      csp1_create_role_fc             =  12;  (* unused *)
      csp1_create_sequence_fc         =  13;
      csp1_alter_sequence_fc          =  14;  (* unused *)
      csp1_unused1_fc                 =  15;  (* unused *)
      csp1_drop_sequence_fc           =  16;
      csp1_create_schema_fc           =  17;
      csp1_create_cluster_fc          =  18;  (* unused *)
      csp1_create_user_fc             =  19;
      csp1_create_index_fc            =  20;
      csp1_drop_index_fc              =  21;
      csp1_drop_cluster_fc            =  22;  (* unused *)
      csp1_validate_index_fc          =  23;  (* unused *)
      csp1_create_procedure_fc        =  24;
      csp1_alter_procedure_fc         =  25;  (* unused *)
      csp1_alter_table_fc             =  26;
      csp1_explain_fc                 =  27;
      csp1_grant_fc                   =  28;
      csp1_revoke_fc                  =  29;
      csp1_create_synonym_fc          =  30;
      csp1_drop_synonym_fc            =  31;
      csp1_alter_system_swith_log_fc  =  32;  (* unused *)
      csp1_set_transaction_fc         =  33;  (* unused *)
      csp1_pl_sql_execute_fc          =  34;
      csp1_lock_table_fc              =  35;
      csp1_unused2_fc                 =  36;  (* unused *)
      csp1_rename_fc                  =  37;
      csp1_comment_fc                 =  38;
      csp1_audit_fc                   =  39;  (* unused *)
      csp1_noaudit_fc                 =  40;  (* unused *)
      csp1_alter_index_fc             =  41;  (* unused *)
      csp1_create_external_database_fc=  42;  (* unused *)
      csp1_drop_external_database_fc  =  43;  (* unused *)
      csp1_create_database_fc         =  44;  (* unused *)
      csp1_alter_database_fc          =  45;  (* unused *)
      csp1_create_rollback_segment_fc =  46;  (* unused *)
      csp1_alter_rollback_segment_fc  =  47;  (* unused *)
      csp1_drop_rollback_segment_fc   =  48;  (* unused *)
      csp1_create_ts_tablespace_fc    =  49;  (* unused *)
      csp1_alter_ts_tablespace_fc     =  50;  (* unused *)
      csp1_drop_ts_tablespace_fc      =  51;  (* unused *)
      csp1_alter_session_fc           =  52;  (* unused *)
      csp1_alter_user_fc              =  53;
      csp1_commit_fc                  =  54;
      csp1_rollback_fc                =  55;
      csp1_savepoint_fc               =  56;
      csp1_create_control_file_fc     =  57;  (* unused *)
      csp1_alter_tracing_fc           =  58;  (* unused *)
      csp1_create_trigger_fc          =  59;
      csp1_alter_trigger_fc           =  60;  (* unused *)
      csp1_drop_trigger_fc            =  61;
      csp1_analyze_table_fc           =  62;
      csp1_analyze_index_fc           =  63;  (* unused *)
      csp1_analyze_cluster_fc         =  64;  (* unused *)
      csp1_create_profile_fc          =  65;  (* unused *)
      csp1_drop_profile_fc            =  66;  (* unused *)
      csp1_alter_profile_fc           =  67;  (* unused *)
      csp1_drop_procedure_fc          =  68;
      csp1_unused3_fc                 =  69;  (* unused *)
      csp1_alter_resource_cost_fc     =  70;  (* unused *)
      csp1_create_sl_snapshot_log_fc  =  71;
      csp1_alter_sl_snapshot_log_fc   =  72;  (* unused *)
      csp1_drop_sl_snapshot_log_fc    =  73;
      csp1_create_snapshot_fc         =  74;
      csp1_alter_snapshot_fc          =  75;  (* unused *)
      csp1_drop_snapshot_fc           =  76;  (* unused *)
      (* *)
      csp1_max_oci_fc                 =  76;  (* boundary check *)
      (* *)
      csp1_select_direct_fc           = 201;  (* ADABAS-special *)
      csp1_select_first_fc            = 202;  (* ADABAS-special *)
      csp1_select_last_fc             = 203;  (* ADABAS-special *)
      csp1_select_next_fc             = 204;  (* ADABAS-special *)
      csp1_select_prev_fc             = 205;  (* ADABAS-special *)
      csp1_fetch_first_fc             = 206;  (* ADABAS-special *)
      csp1_fetch_last_fc              = 207;  (* ADABAS-special *)
      csp1_fetch_next_fc              = 208;  (* ADABAS-special *)
      csp1_fetch_prev_fc              = 209;  (* ADABAS-special *)
      csp1_fetch_pos_fc               = 210;  (* ADABAS-special *)
      csp1_fetch_same_fc              = 211;  (* ADABAS-special *)
      csp1_string_command_fc          = 212;  (* ADABAS-special *)
      csp1_connect_fc                 = 213;  (* ADABAS-special *)
      csp1_drop_parseid_fc            = 214;  (* ADABAS-special *)
      csp1_close_fc                   = 215;  (* ADABAS-special *)
      csp1_usage_fc                   = 217;  (* ADABAS-special *)
      csp1_serverdb_fc                = 218;  (* ADABAS-special *)
      csp1_monitor_fc                 = 219;  (* ADABAS-special *)
      csp1_set_fc                     = 220;  (* ADABAS-special *)
      csp1_link_fc                    = 221;  (* ADABAS-special *)
      csp1_create_domain_fc           = 222;  (* ADABAS-special *)
      csp1_drop_domain_fc             = 223;  (* ADABAS-special *)
      csp1_describe_fc                = 224;  (* ADABAS-special *)
      csp1_alter_password_fc          = 225;  (* ADABAS-special *)
      (*                                226 - 232 was low level *)
      csp1_putval_fc                  = 233;  (* ADABAS-special *)
      csp1_getval_fc                  = 234;  (* ADABAS-special *)
      csp1_diagnose_fc                = 235;  (* ADABAS-special *)
      csp1_unlock_fc                  = 236;  (* ADABAS-special *)
      csp1_refresh_fc                 = 237;  (* ADABAS-special *)
      csp1_clear_snapshot_log_fc      = 238;  (* ADABAS-special *)
      csp1_next_stamp_fc              = 239;  (* ADABAS-special *)
      csp1_exists_table_fc            = 240;  (* ADABAS-special *)
      csp1_commit_release_fc          = 241;  (* ADABAS-special *)
      csp1_rollback_release_fc        = 242;  (* ADABAS-special *)
      csp1_drda_native_ddl_fc         = 243;  (* ADABAS-special *)
      csp1_select_into_fc             = 244;  (* ADABAS-special *)
      csp1_create_dl_database_link    = 245;  (* ADABAS-special *)
      csp1_drop_dl_database_link      = 246;  (* ADABAS-special *)
      csp1_fetch_relative_fc          = 247;  (* ADABAS-special *)
      csp1_cursor_pl_sql_execute_fc   = 248;  (* ADABAS-special *)
      (* *)
      csp1_max_special_adabas_fc      = 248;  (* boundary check *)
      (* *)
      csp1_min_masscmd_fc             = 1000;  (* boundary check *)
      (* *)
      csp1_minsert_fc                 = 1003; (* ADABAS-masscmd *)
      csp1_mselect_fc                 = 1004; (* ADABAS-masscmd *)
      csp1_mupdate_fc                 = 1005; (* ADABAS-masscmd *)
      csp1_mdelete_fc                 = 1009; (* ADABAS-masscmd *)
      csp1_mlock_objects_fc           = 1035; (* ADABAS-masscmd *)
      csp1_mfetch_first_fc            = 1206; (* ADABAS-masscmd *)
      csp1_mfetch_last_fc             = 1207; (* ADABAS-masscmd *)
      csp1_mfetch_next_fc             = 1208; (* ADABAS-masscmd *)
      csp1_mfetch_prev_fc             = 1209; (* ADABAS-masscmd *)
      csp1_mfetch_pos_fc              = 1210; (* ADABAS-masscmd *)
      csp1_mfetch_same_fc             = 1211; (* ADABAS-masscmd *)
      (* *)
      csp1_mselect_into_fc            = 1244;  (* ADABAS-masscmd *)
      csp1_mfetch_relative_fc         = 1247;  (* ADABAS-special *)
      csp1_max_masscmd_fc             = 1247;  (* boundary check *)
      (* *)
      csp1_masscmd_fc_offset          = 1000;
      (* ******* next to the last byte of parsid ******* *)
      csp1_p_none                     =   0;
      csp1_p_command_executed         =   1;
      csp1_p_use_adbs                 =   2;
      csp1_p_release_found            =  10;
      csp1_p_not_allowed_for_program  =  30;
      csp1_p_close_found              =  40;
      csp1_p_describe_found           =  41;
      csp1_p_fetch_found              =  42;
      csp1_p_mfetch_found             =  43;
      csp1_p_mass_select_found        =  44;
      csp1_p_select_for_update_found  =  45;
      csp1_p_reuse_mass_select_found  =  46;
      csp1_p_reuse_update_sel_found   =  47;
      csp1_p_dialog_call              =  60;
      csp1_p_mass_command             =  70;
      csp1_p_mselect_found            = 114;
      csp1_p_for_upd_mselect_found    = 115;
      csp1_p_reuse_mselect_found      = 116;
      csp1_p_reuse_upd_mselect_found  = 117;
      csp1_old_varpart_size           = 8240;
      csp1_part_align_length          =    8;
      csp1_max_packet_cnt             =    2;
      (* *)
      csp1_first_sp1_version          = '62000';
      csp1_vt_empty           = 0;
      csp1_vt_null            = 1;
      csp1_vt_i2              = 2;
      csp1_vt_i4              = 3;
      csp1_vt_r4              = 4;
      csp1_vt_r8              = 5;
      csp1_vt_cy              = 6;
      csp1_vt_date            = 7;
      csp1_vt_bstr            = 8;
      csp1_vt_dispatch        = 9;
      csp1_vt_error           = 10;
      csp1_vt_bool            = 11;
      csp1_vt_variant         = 12;
      csp1_vt_unknown         = 13;
      csp1_vt_decimal         = 14;
      csp1_vt_i1              = 16;
      csp1_vt_ui1             = 17;
      csp1_vt_ui2             = 18;
      csp1_vt_ui4             = 19;
      csp1_vt_i8              = 20;
      csp1_vt_ui8             = 21;
      csp1_vt_int             = 22;
      csp1_vt_uint            = 23;
      csp1_vt_void            = 24;
      csp1_vt_hresult         = 25;
      csp1_vt_ptr             = 26;
      csp1_vt_safearrary      = 27;
      csp1_vt_carray          = 28;
      csp1_vt_userdefined     = 29;
      csp1_vt_lpstr           = 30;
      csp1_vt_lpwstr          = 31;
      csp1_vt_filetime        = 64;
      csp1_vt_blob            = 65;
      csp1_vt_stream          = 66;
      csp1_vt_storage         = 67;
      csp1_vt_streamed_object = 68;
      csp1_vt_stored_object   = 69;
      csp1_vt_blob_object     = 70;
      csp1_vt_cf              = 71;
      csp1_vt_clsid           = 72;
      csp1_vt_lc_char         = 201;
      csp1_vt_lc_wchar        = 202;
      csp1_vt_lc_byte         = 203;
      csp1_vt_lc_bool         = 204;
      csp1_vt_lc_abap_tab_handle = 205;
      csp1_vt_lc_stream_handle   = 206;
      csp1_vt_lc_int1            = 207;
      csp1_vt_lc_kb71_param_desc = 208;
      csp1_vt_lc_uint1           = 209;
	  csp1_vt_lc_ascii7          = 210;
      csp1_vt_lc_wyde            = 211;
      csp1_vt_vector             = 4096;
      csp1_vt_array              = 8192;
      csp1_vt_byref              = 16384;


      (*inlineC #if defined __cplusplus *)
      (*inlineC #include "ggg91.h"      *)
      (*inlineC #endif                  *)
      MAX_AUX_BUFFER_COUNT_GG00 =    16;
      MAX_CODE_TABLES_GG00      =     8;
      MAX_COL_PER_INDEX_GG00    =    16;
      MAX_COL_PER_TAB_GG00      =  1024;
      MAX_COL_SEQUENCE_GG00     =    16; (* cnt cols with link, inv, part *)
      MAX_HASH_PRIME_GG00       =  1024; (* possible maximum 1416         *)
      MAX_INV_PER_TAB_GG00      =   256;
      MAX_JOINS_GG00            =   256;
      MAX_KEYLEN_GG00           =  1024;
      MAX_QUALBUF_LOCKS_GG00    =   240;
      MAX_QUALIFICATIONS_GG00   =   512; (* (old max stack list+1)%4 *)
      MAX_RECLEN_GG00           = BUF_MXSP00 - 96; (* ++++ 7.* value ++++ *)
      (*                          7.1: buf - NodeHeader                   *)
      (*                               - PageCheckTrailer - BdPointerSize *)
      MAX_LONGRESULTBUF_GG00    = MAX_RECLEN_GG00 + MAX_COL_PER_TAB_GG00;
      MAX_RECPARTS_GG00         =   255;
      MAX_ROOTLIST_GG00         =     6;
      MAX_SERVER_MBODY_GG00     =   100;
      MAX_SERVER_MDESC_GG00     = MAX_INT2_SP00;
      MAX_SERVER_TASKS_GG00     = MAX_INT2_SP00;
      MAX_SERVERDBS_GG00        = MAX_INT2_SP00;
      MAX_SHOW_COLUMNS_GG00     =    80; (* PTS 1111217 E.Z. *)
      MAX_STACK_GG00            =    40;
      MAX_STRATEGY_KEYS_GG00    =    20;
      MAX_TAPES_GG00            =    32;
      MAX_TRANS_GG00            = MAX_INT2_SP00;
      MAX_USER_TASKS_GG00       = MAX_INT2_SP00;
      (**)
      DOUBLE_BUF_MXGG00         = 2 * BUF_MXSP00;
      FILE_ID_MXGG00            =    40;
      FILE_POS_MXGG00           =     8; (* FilePos used bytes *)
      FN_MXGG00                 =    16;
      INFLEN_MXGG00             =    48; (* MInfos *)
      LOCK_MXGG00               =    16;
      LOCK_ROWID_MXGG00         =    40;
      LOWER_PNO_BOUND_GG00      =     0;
      MB_PART1_LOCKHEAD_MXGG00  =     4;
      MOD_HEADER_MXGG00         =    64;
      MULT_HEAD_MXGG00          =    16; (* header of str_mindex strat *)
      NIL_PAGE_NO_GG00          = MAX_INT4_SP00;
      NUMBER_MXGG00             = (csp_fixed + 1) DIV 2 + 2;
      ONE_8BYTE_CNT_GG00        =     1.0;             (* PTS 1103743 JA 1999-08-25 *)
      UPPER_PNO_BOUND_GG00      =     9;
      cgg_rec_key_offset        =     8;
      PAGE_HEADER_MXGG00        =     8;
      PAGE_TRAILER_MXGG00       = PAGE_HEADER_MXGG00;
      PAGE_CHECK_TRAILER_MXGG00 = INT4_MXSP00 + PAGE_TRAILER_MXGG00;
      RECBODY_MXGG00            = BUF_MXSP00 - cgg_rec_key_offset;
      RECPART_MXGG00            = BUF_MXSP00 DIV MAX_RECPARTS_GG00;
      RESCNT_MXGG00             =     4;
      SFP_INIT_PART_MXGG00      =  2200;
      SITE_MXGG00               =     2;
      STACK_ENTRY_MXGG00        =     8;
      STRING_FD_MXGG00          =    44; (* string file desc     *)
      SURROGATE_MXGG00          =     8;
      SYSKEY_MXGG00             =   140;
      TEMP_PREFIX_MXGG00        =     8;
      TG_UPD_SET_OFFSET_MXGG00  =    64; (* TriggerInfo - 32   *)
      XCONTROL_MXGG00           =   500;
      (**)
      FILE_ID_HEAD_MXGG00       = FILE_ID_MXGG00 - FN_MXGG00;
      LOGPAGE_BODY_MXGG00       = PAGE_MXSP00 - PAGE_HEADER_MXGG00
            - INT4_MXSP00 (* kb0p_bottom *) - PAGE_CHECK_TRAILER_MXGG00;
      MB_PART1_HEAD_MXGG00      = FILE_ID_MXGG00 + 2*32 + 8;
      MB_PART1_RETURN_MXGG00    = FILE_ID_MXGG00 + 2*RESNUM_MXSP00 + 2 + 4;
      MB_STRAT_SIZE_MXGG00      = ( MAX_INT2_SP00 DIV 2 ) * 2;
      ALIGNMENT_GG00            =     4;
      (**)
      MAX_SEQINFOS_GG00         =    32;              (* PTS 1110096 E.Z. *)
      (*****  begin offset constants  *****)
      (**)
      cgg_begin_termsets        =     3;
      cgg_col_fd_size           =     9;
      cgg_end_termsets          =    10;
      cgg_eo_bytestr            =     0; (* bytestringhandling *)
      cgg_languages             =     4;
      cgg_log_sys_pages         =     3; (* 2 config + restart rec *)
      cgg_min_data_pno          =   250;
      cgg_min_sysbuf_pages      =    10;
      cgg_mapset_entries        =   256;
      cgg_max_cmd_timeout       = 32400; (* seconds = 9 h   *)
      cgg_max_timeout           = 86400; (* seconds = 1 day *)
      cgg_msgtext_len           =   160; (* Length of column MSGTEXT *)
      cgg_nil_child             =     0;
      cgg_nil_leafnodes         =    -1; (* -> treeleafnodes *)
      cgg_nil_pid               =     0;
      cgg_nil_transindex        =     0;
      cgg_nil_varcol_cnt_off    =    -1;
      cgg_min_nextval_len       =    10;
      cgg_old_parsid            =   -12;
      cgg_oradate_len_to_char   =    14;
      cgg_oradate_max_to_char   =   100;
      cgg_range_entries         =    24;
      cgg_return_stack_entries  =     8;
      cgg_str_col_entry_size    =     8;
      cgg_suspend_lvl_0         =     0;
      cgg_susp_level_1          =     1;
      cgg_susp_level_2          =     2;
      cgg_termset_entries       =   128;
      cgg_timeout_interval      =    30;
      cgg_lc_timeout_interval   =     5; (* PTS 1110289 *)
      cgg_treeid_fn_offset      = FILE_ID_MXGG00 - FN_MXGG00;
      cgg_whole_bytestr         =    -1; (* bytestringhandling *)
      NIL_STR_VERSION_GG00      =  NIL_PAGE_NO_GG00;
      (**)
      cgg_m_switch              =   500;
      cgg_m_switchlimit         =   501;
      cgg_m_buflength           =   502;
      cgg_m_minbuf              =   503;
      cgg_m_maxbuf              =   504;
      (**)
      cgg_is_false              =     0;
      cgg_is_true               =     1;
      cgg_is_undef              =     2;
      cgg_is_not_false          =     3;
      cgg_is_not_true           =     4;
      cgg_is_not_undef          =     5;
      (**)
      cgg_union_byte            =     1;
      cgg_except_byte           =     2;
      cgg_intersect_byte        =     3;
      cgg_cdistinct_bytes       =     4;
      (**)
      cgg_union_select          =   100;
      cgg_union_append          =    10;
      TriggerKeyLen_cgg00       =    14;
      (**)
      cgg_monday                =     1;
      cgg_sunday                =     7;
      (**)
      (***** constants used together with g02_codetables ******)
      cgg_to_ebcdic             =     1; (* ascii_to_ebcdic *)
      cgg_to_ascii              =     2; (* ebcdic_to_ascii *)
      cgg_up_ebcdic             =     3; (* small_ebcdic_to_capital_ebcdic*)
      cgg_up_ascii              =     4; (* small_ascii_to_capital_ascii  *)
      cgg_to_up_ebcdic          =     5; (* small_ascii_to_capital_ebcdic *)
      cgg_to_up_ascii           =     6; (* small_ebcdic_to_capital_ascii *)
      cgg_low_ebcdic            =     7; (* capital_ebcdic_to_small_ebcdic*)
      cgg_low_ascii             =     8; (* capital_ascii_to_small_ascii  *)
      (**)
      cgg_default_mapchar_set   = 'DEFAULTMAP        ';
      cgg_evfile                = '\00\0E';
      ROOT_CHECK_GG00           =  -1554236731; (* 0xC53A5CA3 *)
      (* PTS 1111576 E.Z. *)
      cgg_zero_exponent         = '\80';
      (**)
      cgg_truechar              = '\01';
      cgg_falsechar             = '\00';
      cgg_ascii_colon           = '\3A';
      cgg_ascii_comma           = '\2C';
      cgg_ascii_equal           = '\3D';
      cgg_ascii_greater         = '\3E';
      cgg_ascii_less            = '\3C';
      cgg_ascii_period          = '\2E';
      cgg_ascii_plus            = '\2B';
      cgg_ascii_slash           = '\2F';
      cgg_ebcdic_colon          = '\7A';
      cgg_ebcdic_comma          = '\6B';
      cgg_ebcdic_equal          = '\7E';
      cgg_ebcdic_greater        = '\6E';
      cgg_ebcdic_less           = '\4C';
      cgg_ebcdic_period         = '\4B';
      cgg_ebcdic_plus           = '\4E';
      cgg_ebcdic_slash          = '\61';
      (**)
      cgg_dummy_file_version    = '\FF\FF';
      cgg_special_file_version  = '\FF\FE';
      (**)
      cgg_dummy_site            = '\FF\FE';
      cgg_nil_site              = '\FF\FF';
      (**)
      MAX_TRANS_COUNT_GG00      = '\FF\00\00\00';
      MAX_SESSION_COUNT_GG00    = '\FF\00\00\00';
      (**)
      cgg_del_trigger_link      = '\00\01';
      cgg_ins_trigger_link      = '\00\03';
      cgg_upd_trigger_link      = '\00\05';
      (**)
      cgg_local_segment_id      = '\00\00';
      cgg_public_segment_id     = '\00\01';
      (**)
      cgg_etermset              = '\00\4F';
      cgg_econfig               = '\00\50';
      cgg_emapset               = '\00\51';
      (* PTS 1111576 E.Z. *)
      (**)
      cgg_max_session           = '\FF\00\00\00';
      cgg_nil_session           = '\FF\FF\FF\FF';
      cgg_nil_trans             = '\FF\FF\FF\FF\FF\FF';
      (**)
      cgg_zero_c2               = '\00\00';
      cgg_zero_c4               = '\00\00\00\00';
      cgg_zero_id               = '\00\00\00\00\00\00\00\00';
      cgg_zero_session          = '\00\00\00\00';
      cgg_zero_snap_key         = '\00\00\00\00\00\00';
      cgg_zero_subtrans         = 0;
      cgg_zero_trans            = '\00\00\00\00\00\00';
      cgg_zero_userid           = '\00\00\00\00';
      (**)
      cgg_zero_fn               =
            '\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00';
      cgg_errortext_fn          =
            '\1A\00\FF\FF\00\00\00\00\00\00\00\00\00\00\00\00';
      (**)
      cgg_sys1_catalog_id       = '\00\00\00\00\00\00\00\01';
      cgg_sys2_catalog_id       = '\00\00\00\00\00\00\00\02';
      cgg_temp_tablock_id       = '\00\00\00\00\00\00\00\0A';
      cgg_redo_lock_id          = '\00\00\00\00\00\00\00\0C';
      cgg_listid                = '\00\00\00\00\00\00\00\1A';
      CONSIST_TRANS_PREFIX_GG00 = '\FF\FC\00\00\00\00\00\00';
      OPEN_TRANS_PREFIX_GG00    = '\FF\FB\00\00\00\00\00\00';
      (**)
      cgg_obj_key_file_id       = '\FF\FF\FF\FF';
      (**)
      cgg_param_in_between_expr = '\03';
      cgg_param_in_in_expr      = '\04';
      (**)
      (***** in part2.buf[ 5 ] together with m_insert_select *****)
      cgg_ins_sel_nokey         = '\01';
      cgg_ignore_dupl           = '\02';
      cgg_reject_dupl           = '\03';
      cgg_update_dupl           = '\04';
      (**)
      (*****  tgg00_BasisError constants  *****)
      e_ok                           =     0;
      e_cachedirectory_full          =   100;
      e_append_not_allowed           =   210; (*for internal use only*)
      e_bad_datapage                 =   300;
      e_bad_devspace                 =   310;
      e_bad_file                     =   320;
      e_bad_invfile                  =   330;
      e_bad_usmpage                  =   340;
      e_buffer_limit                 =   350;
      e_cancelled                    =   400;
      e_com_error                    =   450;
      e_do_not_access_fdir           =   504;
      e_do_not_drop_fdir             =   505;
      e_duplicate_filename           =   510;
      e_duplicate_key                =   530;
      e_duplicate_sysinfo            =   540;
      e_duplicate_record             =   550; (*internal for joinview-check*)
      e_file_empty                   =   700;
      e_file_limit                   =   710;
      e_file_must_be_read_only       =   712; (*for internal use only*)
      e_file_not_accessible          =   715;
      e_file_not_found               =   720;
      e_file_read_only               =   730;
      e_foreign_db_error             =   735;
      e_file_unloaded                =   740;
      e_illegal_filename             =  1000;
      e_illegal_invlistpos           =  1010;
      e_illegal_key                  =  1020;
      e_illegal_record               =  1030;
      e_incompatible_savefile        =  1040;
      e_init_missing                 =  1050;
      e_inv_list_not_found           =  1060;
      e_inv_not_found                =  1070;
      e_invalid_invlistpos           =  1080;
      e_invalid_mix_dml_ddl          =  1090;
      e_invalid_root                 =  1100;
      e_key_not_found                =  1200;
      e_primkey_not_found            =  1210;
      e_impossible_mirrored_dev      =  1300;
      e_no_more_temp_space           =  1400;
      e_no_more_memory               =  1405;
      e_no_next_invkey               =  1410;
      e_no_next_record               =  1420;
      e_no_prev_invkey               =  1430;
      e_no_prev_record               =  1440;
      e_no_statistic                 =  1450;
      e_old_fileversion              =  1500;
      e_old_leafnodes_number         =  1510;
      e_old_parsid                   =  1520;
      e_shutdown                     =  1900;
      e_sysbuf_storage_exceeded      =  1910;
      e_sequence_overflow            =  1915;
      e_currval_without_nextval      =  1916;   (* PTS 1110096 E.Z. *)
      e_sysbuffer_overflow           =  1920;
      e_sysinfo_not_found            =  1930;
      e_unknown_strategy             =  2050;
      e_sysdevname_invalid           =  2300;
      e_enough_resultsets            =  2400;
      e_invalid_date                 =  2500;
      e_end_of_month_adjustment      =  2510; (*internal==> warn6 *)
      e_time_value_too_long          =  2520; (*internal==> warn11*)
      e_invalid_time                 =  2530;
      e_invalid_timestamp            =  2540;
      e_invalid_day_of_week          =  2545;
      e_invalid_pattern              =  2550;
      e_request_timeout              =  2600;
      e_too_many_resultsets          =  2700;
      e_too_many_serials             =  2705;
      e_too_many_updatesets          =  2710;
      e_too_many_differences         =  2720;
      e_too_many_mb_stackentries     =  2800;
      e_too_many_mb_data             =  2801;
      e_too_small_mb_qual_part       =  2802;
      e_illegal_server_mb            =  2803;
      e_too_many_file_infos          =  2804;
      e_too_small_packet_size        =  2805;
      e_too_complicated_constr       =  2806;
      e_too_long_viewtext            =  2807;
      e_too_many_named_subtrans      =  2808;
      e_too_many_intern_cmd          =  2809;
      e_too_many_intern_parsinfos    =  2810;
      e_too_many_nested_functions    =  2811;
      e_too_long_corelated_row       =  2812;
      e_too_many_internal_columns    =  2813;
      e_too_long_trigger_params      =  2814;
      e_too_complex_trigger_cond     =  2815;
      e_too_complex_drda_stmt        =  2816;
      e_too_many_statements_in_trigger= 2817;
      e_too_many_mb_strat            =  2818;
      e_bad_hostfile_page            =  3100;
      e_bad_backup_page              =  3110;
      e_cmd_timeout_invalid          =  3150;
      e_column_trunc                 =  3200;
      e_serverdb_not_empty           =  3250;
      e_serverdb_read_only           =  3300;
      e_date_time_format_invalid     =  3301;
      e_default_code_invalid         =  3302;
      e_devname_invalid              =  3304;
      e_duplicate_devname            =  3306;
      e_devsize_invalid              =  3308;
      e_devsize_too_small            =  3310;
      e_duplicate_invkey             =  3320;
      e_not_implemented              =  3400;
      e_hex0_column_tail             =  3600;
      e_hostfile_error               =  3700;
      e_incomplete_logsegm           =  3705;
      e_invalid_blocksize            =  3707;
      e_serverdb_no_invalid          =  3720;
      e_incompatible_log             =  3800;
      e_inconsistent_db              =  3810;
      e_inconsistent_nodetype        =  3816;
      e_invalid                      =  3820;
      e_serverdb_name_invalid        =  3825;
      e_invalid_sponsor              =  3830;
      e_key_update                   =  4000;
      e_lock_timeout_invalid         =  4030;
      e_logpage_not_found            =  4050;
      e_end_of_log_section           =  4060;
      e_log_not_cold_saved           =  4100;
      e_logdev_not_in_xparam         =  4103;
      e_logmode_invalid              =  4106;
      e_no_of_logdev_invalid         =  4120;
      e_invalid_mirrored_dev         =  4170;
      e_net_line_down                =  4210; (* ++++++++++ *)
      e_net_disconnected             =  4215; (* ++++++++++ *)
      e_net_notok                    =  4220; (* ++++++++++ *)
      e_new_hostfile_required        =  4300;
      e_end_of_backup                =  4301;
      e_no_of_dev_invalid            =  4302;
      e_logsegm_size_invalid         =  4303;
      e_no_data_on_tape              =  4304;
      e_no_servers                   =  4305;
      e_save_skipped                 =  4306;
      e_null_value_illegal           =  4310;
      e_num_invalid                  =  4320;
      e_num_overflow                 =  4330;
      e_open_write_trans             =  4400;
      e_program_stack_overflow       =  4500;
      e_qual_violation               =  4600;
      e_range_violation              =  4700;
      e_rec_not_locked               =  4710;
      e_record_not_qualified         =  4720;
      e_release_caused_by_shutdown   =  4721;
      e_req_timeout_invalid          =  4722;
      e_request_discarded            =  4725;
      e_response_timeout             =  4728;
      e_restart_incomplete           =  4730;
      e_rollback_wanted              =  4740;
      e_skip_upd                     =  4800;
      e_skip_key_upd                 =  4801;
      e_special_null                 =  4810;
      e_stack_op_illegal             =  4820;
      e_stack_overflow               =  4830;
      e_stack_type_illegal           =  4840;
      e_string_column_empty          =  4850;
      e_work_rolled_back             =  4900;
      e_serverdb_error               =  4905;
      e_rollb_log_full               =  4906;
      e_log_reject                   =  4907;
      e_too_many_devspaces           =  4920;
      e_too_many_lockunits_specified =  4930;
      e_too_many_lock_requests       =  4940;
      e_too_many_net_requests        =  4945;
      e_trans_already_working        =  4950;
      e_trigger_failed               =  4960;
      e_function_failed              =  4970;
      e_lock_collision               =  5000;
      e_pending_lock_collision       =  5001;
      e_lock_dirty                   =  5005;
      e_upd_view_violation           =  5010;
      e_upd_in_nolog_session         =  5012;
      e_unexpected_net_request       =  5015;
      e_unexpected_drda_cp           =  5016;
      e_unknown_serverdb             =  5020;
      e_unknown_database_link        =  5025;
      e_unauthorized_serverdb        =  5030;
      e_view_violation               =  5100;
      (*e_wait_for_savepoint           =  5200;*)
      e_wait_for_lock_release        =  5210;
      e_wrong_hostfile               =  5220;
      e_no_log_to_save               =  5225;
      e_backup_is_running            =  5226; (* PTS 1108479 UH 2000-11-27 *)
      e_backup_not_running           =  5227; (* PTS 1108479 UH 2000-11-27 *)
      e_wrong_gateway_ccsid          =  5230;
      e_wrong_password               =  5240;
      e_dcom_hresult                 =  5300;
      e_missing_usertype_guid        =  5360;
      e_too_many_interfaces          =  5370;
      e_unexpected_child_request     =  5400;
      e_too_many_child_requests      =  5500;
      e_invalid_label                =  5550;
      e_invalid_configuration        =  5600;
      e_read_task_crash              =  5700;
      e_write_task_crash             =  5750;
      e_acknowledgement              =  5800;
      e_queue_mismatch               =  5900;
      e_bad_pagetype                 =  5950;
      e_invalid_columnname           =  6000;
      e_invalid_end_of_command       =  6005;
      e_invalid_datatype             =  6010;
      e_invalid_filetype             =  6012;
      e_damaged_invlist              =  6013;
      e_invalid_invseparator         =  6014;
      e_identifier_too_long          =  6015;
      e_invalid_indexorder           =  6016;
      e_invalid_keyword              =  6020;
      e_expr_in_insert_value_list    =  6025; (*only internal error*)
      e_parse_not_implemented        =  6030;
      e_invalid_mix_functions_columns=  6035;
      e_invalid_tablename            =  6040;
      e_invalid_username             =  6045;
      e_invalid_messagetype          =  6050;
      e_in_view_not_allowed          =  6055;
      e_replicated_table_not_allowed =  6057;
      e_parameter_not_allowed        =  6060;
      e_mess_type_illegal            =  6062;
      e_missing_constant             =  6065;
      e_missing_identifier           =  6070;
      e_missing_integer              =  6075;
      e_missing_keyword              =  6080;
      e_wanted_keyword               =  6083;
      e_missing_number               =  6085;
      e_missing_password             =  6087;
      e_missing_delimiter            =  6088;
      e_missing_string_literal       =  6090;
      e_missing_value_spec           =  6095;
      e_reserved_identifier          =  6105;
      e_subquery_not_allowed         =  6110;
      e_too_many_monadic_operators   =  6115;
      e_rowno_not_allowed            =  6120;
      e_invalid_unsign_integer       =  6125;
      e_update_not_allowed           =  6130;
      e_in_union_not_allowed         =  6135;
      e_synonym_not_allowed          =  6150;
      e_alter_column_not_allowed     =  6200;
      e_alter_drop_user_not_allowed  =  6202;
      e_check_option_not_allowed     =  6203;
      e_column_indexed               =  6204;
      e_column_must_be_keycolumn     =  6206;
      e_col_must_be_no_keycolumn     =  6208;
      e_column_must_have_default     =  6210;
      e_column_not_indexed           =  6212;
      e_command_for_string_not_allow =  6218;
      e_command_timeout              =  6222;
      e_const_incompatible_with_typ  =  6224;
      e_corelated_subquery_not_allowe=  6226;
      e_costlimit_overflow           =  6228;
      e_costwarning_overflow         =  6230;
      e_current_of_needs_fetch       =  6232;
      e_default_not_allowed          =  6234;
      e_default_not_in_range         =  6236;
      e_default_spec_not_allowed     =  6238;
      (* PTS 1111576 E.Z. *)
      e_duplicate_columnname         =  6242;
      e_duplicate_constraint         =  6244;
      e_duplicate_default            =  6246;
      e_duplicate_indexname          =  6248;
      e_duplicate_name               =  6250;
      e_duplicate_ordercolumn        =  6252;
      e_duplicate_privilege          =  6254;
      e_duplicate_referencename      =  6256;
      e_duplicate_resultname         =  6258;
      e_duplicate_tablename          =  6262;
      e_duplicate_trigger_def        =  6264;
      e_duplicate_value              =  6266;
      e_foreign_key_must_exist       =  6268;
      e_foreign_key_violation        =  6270;
      e_from_select_not_allowed      =  6274;
      e_function_not_allowed         =  6276;
      e_group_user_not_allowed       =  6278;
      e_incompatible_code_spec       =  6280;
      e_incompatible_datatypes       =  6282;
      e_indexed_table_not_allowed    =  6284;
      e_indexname_must_be_unique     =  6286;
      e_invalid_cachelimit_size      =  6288;
      e_invalid_char_position        =  6290;
      e_invalid_command              =  6292;
      e_invalid_costwarning          =  6294;
      e_invalid_datalength           =  6296;
      e_invalid_length_of_datatyp    =  6298;
      e_invalid_messcodetype         =  6300;
      e_invalid_number_variable      =  6302;
      e_invalid_parameter            =  6304;
      e_invalid_range                =  6308;
      e_invalid_sequence             =  6310;
      e_invalid_subtrans_structure   =  6312;
      e_invalid_table_content        =  6314;
      e_invalid_termset              =  6316;
      e_invalid_view_column          =  6318;
      e_join_fields_too_long         =  6320;
      e_key_does_not_exist           =  6322;
      (* PTS 1111576 E.Z. *)
      e_key_not_allowed              =  6324;
      e_link_column_not_allowed      =  6326;
      e_link_cycle_not_allowed       =  6328;
      e_link_rule_not_allowed        =  6330;
      e_link_rule_violation          =  6332;
      e_lock_row_not_allowed         =  6334;
      e_role_cycle                   =  6335;
      e_mandatory_field_is_null      =  6336;
      e_missing_basetable            =  6340;
      e_missing_column_definite      =  6341;
      e_missing_tablename_definite   =  6342;
      e_missing_indexvalue           =  6344;
      e_missing_keyfield             =  6346;
      e_missing_mandatory_recordfield=  6348;
      e_missing_privilege            =  6350;
      e_missing_reuseable            =  6352;
      e_missing_update_column        =  6354;
      (* PTS 1111576 E.Z. *)
      e_missing_viewtable            =  6356;
      e_new_session_required         =  6358;
      e_no_more_user_temp_space      =  6360;
      e_no_open_subtrans             =  6362;
      e_no_resulttable_allowed       =  6364;
      e_not_allowed_in_temp_table    =  6366;
      e_not_null_not_allowed         =  6368;
      e_null_not_allowed             =  6370;
      e_num_truncated                =  6372;
      e_one_output_field_too_long    =  6374;
      e_only_one_recordtype_allowed  =  6376;
      e_order_col_must_be_number     =  6377;
      e_outer_join_def_error         =  6378;
      e_outer_join_not_allowed       =  6379;
      e_output_columns_too_long      =  6380;
      e_output_field_must_be_group   =  6382;
      e_owner_must_be_specified      =  6384;
      e_par_in_expl_not_allowed      =  6386;
      e_permlimit_not_allowed        =  6390;
      e_procedure_must_be_recompiled =  6392;
      e_range_for_string_not_allowed =  6398;
      e_referenced_index_not_allowed =  6400;
      e_remote_load_not_allowed      =  6402;
      e_restore_catalog_error        =  6404;
      e_bd_page_collision            =  6405; (*for internal use only*)
      e_root_is_first_index          =  6406; (*for internal use only*)
      e_root_is_leaf                 =  6407; (*for internal use only*)
      e_row_not_found                =  6408;
      e_select_col_list_too_long     =  6410;
      e_serverdb_already_activated   =  6412;
      e_serverdb_invalid             =  6414;
      e_serverdb_must_be_activated   =  6416;
      (* PTS 1111576 E.Z. *)
      e_st_col_not_open              =  6420;
      e_st_col_open_read_only        =  6422;
      e_st_invalid_destpos           =  6424;
      e_st_invalid_length            =  6426;
      e_st_invalid_pos               =  6428;
      e_st_invalid_startpos          =  6430;
      e_system_error                 =  6432;
      e_system_inf_inconsistent      =  6434;
      e_table_is_unloaded            =  6436;
      e_table_must_be_temp           =  6438;
      e_table_not_in_from_list       =  6440;
      e_temp_table_not_allowed       =  6442;
      e_too_few_columns              =  6444;
      e_too_few_values               =  6446;
      e_too_long_key                 =  6448;
      e_too_long_record              =  6450;
      e_too_many_cols_dropped        =  6452;
      e_too_many_columns             =  6454;
      e_too_many_constraints         =  6455;
      e_too_many_corr_tables         =  6456;
      e_too_many_correlation_columns =  6458;
      e_too_many_indices             =  6460;
      e_too_many_joins               =  6462;
      e_too_many_order_columns       =  6466;
      e_too_many_long_columns        =  6468;
      e_too_many_sec_links           =  6470;
      e_too_many_serverdbs           =  6472;
      e_too_many_sourcetables        =  6474;
      e_too_many_subqueries          =  6476;
      e_too_many_subtrans            =  6478;
      e_too_many_termsets            =  6480;
      e_too_many_users_connected     =  6482;
      e_too_many_values              =  6484;
      e_too_many_variables           =  6486;
      e_too_old_tool                 =  6487;
      e_too_short_datapart           =  6488;
      e_invalid_segment_construction =  6489;
      e_union_cols_mustbe_compatible =  6490;
      e_unknown_name                 =  6491;
      e_unknown_columnname           =  6492;
      e_unknown_constraint           =  6494;
      e_unknown_default              =  6496;
      e_unknown_domainname           =  6498;
      e_unknown_functionname         =  6499;
      e_unknown_groupname            =  6500;
      e_unknown_indexname            =  6502;
      e_unknown_linkname             =  6504;
      e_unknown_mapset               =  6506;
      e_unknown_ordercolumn          =  6508;
      e_unknown_parseid              =  6510;
      e_unknown_password             =  6512;
      e_unknown_procedure_name       =  6514;
      e_unknown_resultname           =  6516;
      e_unknown_sequencename         =  6518;
      e_unknown_name_of_serverdb     =  6520;
      e_unknown_session              =  6521;
      e_unknown_synonymname          =  6522;
      e_unknown_tablename            =  6524;
      e_unknown_termset              =  6526;
      e_unknown_multibyte_set        =  6527;
      e_unknown_trigger              =  6528;
      e_unknown_user                 =  6530;
      e_unknown_user_password_pair   =  6532;
      e_use_cascade                  =  6533;
      e_use_dbsparse                 =  6534;
      e_use_fetch_rest               =  6536;
      e_user_already_connected       =  6538;
      e_user_connected_to_same_task  =  6540;
      e_user_not_allowed             =  6542;
      e_user_not_connected           =  6544;
      e_value_expr_must_be_param_name=  6546;
      e_varchar_not_allowed          =  6548;
      e_variable_not_allowed         =  6550;
      e_view_col_list_too_long       =  6552;
      e_view_def_contradicts_table_de=  6554;
      e_without_datatypes            =  6556;
      e_wrong_database_mode          =  6558;
      e_wrong_database_instance      =  6559;
      e_invalid_nodeid               =  6560;
      e_illegal_escape_sequence      =  6562;
      e_invalid_escape_char          =  6564;
      e_unitrans_src_too_short       =  6568;
      e_unitrans_dest_too_short      =  6570;
      e_not_translatable             =  6572;
      e_not_trans_to_uni             =  6574;
      e_not_trans_from_uni           =  6576;
      e_autosave_running             =  6578;
      (* Errors for oracle like timestamp functions: 67xx *)
      e_to_date_duplicate_format     =  6710;
      e_no_to_date_format            =  6720;
      e_date_format_not_recognized   =  6721;
      e_to_date_too_long_input       =  6730;
      e_to_date_inconsistent         =  6731;
      e_to_date_invalid_day          =  6739;
      e_to_date_too_short_input      =  6740;
      e_to_date_out_of_range         =  6741;
      e_unknown_month                =  6743;
      e_unknown_day                  =  6746;
      e_unknown_meridian             =  6755;
      e_invalid_timezone             =  6757;
      e_to_date_no_number            =  6758;
      e_to_date_no_letter            =  6759;
      e_number_format_not_recogniz   =  6781;
      e_too_many_precision_specs     =  6798;
      e_bad_precision_spec           =  6799;
      (* The error numbers above ressemble the corresponding *)
      (* oracle returncodes, which are in the form -18xx.    *)
      e_history_not_found            =  7010;
      e_new_disk_not_accessible      =  8000;
      e_db2_not_accessible           =  8001;
      e_allocate_failed              =  8002;
      e_vtam_communication_failed    =  8003;
      e_standby_info_not_ok          =  8004;  (* PTS 1111229 E.Z. *)
      e_root_check                   =  8010;
      e_no_converter_entry           =  8050;
      e_bad_converter_entry          =  8051;
      e_lwb_fatal_errors             =  9000;
      e_bad_fdir                     =  9001;
      e_bad_logpage                  =  9010;
      e_bad_output                   =  9020;
      e_bad_fbmpage                  =  9025;
      e_bad_convpage                 =  9030;
      e_bad_syspage                  =  9040;
      e_bad_bitmap_page              =  9045;
      e_disk_not_accessible          =  9050;
      e_illegal_branchlength         =  9060;
      e_illegal_entrylength          =  9070;
      e_illegal_entrypos             =  9080;
      e_illegal_keylength            =  9090;
      e_illegal_page_no              =  9100;
      e_illegal_root                 =  9110;
      e_move_error                   = -9111;
      e_invalid_entrypos             =  9120;
      e_invalid_index_structure      =  9130;
      e_invalid_leaves_structure     =  9140;
      e_no_more_space                =  9150;
      e_page_in_wrong_tree           =  9160;
      e_data_page_corrupted          =  9161;
      e_corrupted_datapage_from_io   =  9163;
      e_illegal_page_address         =  9170;
      e_invalid_fbm_mark             =  9180;
      e_multiple_converter_entry     =  9190;
      e_log_error                    =  9200;
      e_log_full                     =  9210;
      e_net_error                    =  9220; (* ++++++++++ *)
      e_xstart_required              =  9230;
      e_nil_transindex               =  9232;
      e_checkpoint_missing           =  9234;
      e_long_column_missing          =  9240; (* PTS 1103382 TS 1999-07-16 *)
      e_exclusive_violation          =  9300;
      e_bd_leaf_unlocked             =  9400; (*for internal use only*)
      e_too_long_objects             =  9500;
      (*- begin C++ SQL-RTE errors (basis error      = SQLCode) -*)
      (*-      Range from -27000 to -28999                 -*)
      e_unknown_error                =  -27000;
      e_inp_null_not_allowed         =  -27001;
      e_inp_number_invalid           =  -27002;
      e_inp_number_overflow          =  -27003;
      e_param_list_too_long          =  -27004;
      e_param_list_too_short         =  -27005;
      e_incompatible_datatype        =  -27006;
      e_too_many_hostvar             =  -27007;
      e_missing_variable_addr        =  -27008;
      e_cmd_too_long                 =  -27009;
      e_copyop_not_allowed           =  -27010;
      e_inp_string_truncated         =  -27011;
      e_use_callstm                  =  -27012;
      e_cmd_not_available            =  -27013;
      e_new_failed                   =  -28000;
      e_nil_pointer                  =  -28001;
      e_invalid_oid                  =  -28002;
      e_unknown_guid                 =  -28003;
      e_unknown_clsid                =  -28004;
      e_duplicate_object             =  -28005;
      e_object_not_locked            =  -28006;
      e_incompatible_oid             =  -28007;
      e_oms_istream_error            =  -28501;
      e_oms_ostream_error            =  -28502;
      e_com_object_crashed           =  -28503;
      e_coguid_not_in_typelib        =  -28504;
      e_no_coclass_obj_found         =  -28505;
      e_wrong_first_param            =  -28506;
      e_too_many_out_param           =  -28507;
      e_unsupported_param_type       =  -28508;
      e_progid_not_found             =  -28509;
      e_typelib_not_found            =  -28510;
      e_inprocserver_not_found       =  -28511;
      e_localserver_not_found        =  -28512;
      e_method_not_found             =  -28513;
      e_unknown_version              =  -28514; (* ADIS 1001807, T.A. 15.12.1998 *)
      e_version_already_open         =  -28515;
      e_transaction_end_required     =  -28516;
      e_already_in_critical_section  =  -28517; (* ADIS CR 1000225, T.A. 16.02.1998 *)
      e_not_in_critical_section      =  -28518; (*                                  *)
      e_too_many_varobj_containers   =  -28519;
      e_object_too_long              =  -28520;
      e_already_in_version           =  -28521;
      e_missing_before_image         =  -28522;
      e_unknown_schema               =  -28523;
      e_duplicate_schema             =  -28524;
      e_proc_address_not_found       =  -28525;
      e_lib_not_loaded               =  -28526;
      e_directory_not_found          =  -28527;
      e_lib_not_released             =  -28528;
      e_buffer_too_small             =  -28529;
      e_terminate                    =  -28530;
      e_oms_read_only                =  -28531; (* PTS 1112358 *)
      e_version_bound_by_trans       =  -28532; (* PTS 1115023 *)
      e_packed_out_of_range          =  -28533; (* PTS 1115372, T.A. *)
      (*      C++ object kernel errors  Range from -28800 to -28899 *)
      e_wrong_class_id               =  -28801;
      e_wrong_object_version         =  -28802;
      e_wrong_object_state           =  -28803;
      e_too_old_oid                  =  -28804;
      e_no_bd_lock                   =  -28805;
      e_nil_page_pointer             =  -28806;
      e_no_next_object               =  -28807;
      e_hash_not_found               =  -28808;
      e_invalid_obj_col_desc         =  -28809;
      e_duplicate_hash_key           =  -28810;
      e_hash_key_not_found           =  -28811;
      e_invalid_iterator             =  -28812;
      e_unknown_consist_vers         =  -28813;
      e_object_not_found             =  -28814;
      e_obj_history_not_found        =  -28815;
      e_hash_memory_exceeded         =  -28816;
      e_illegal_object_length        =  -28817;
      e_illegal_object_pos           =  -28818;
      e_object_dirty                 =  -28819;
      e_critical_section_in_use      =  -28821;
      e_wrong_obj_file_id            =  -28822;
      e_page_no_in_use               =  -28823;
      e_case_not_found               =  -28901;
      e_missing_object_flush         =  -28950;
      e_requested_dbperror           =  -28999; (* PTS 1112358 *)
      (**)
      mxgg_double_buf         = DOUBLE_BUF_MXGG00;
      mxgg_fn                 = FN_MXGG00;
      mxgg_inflen             = INFLEN_MXGG00;
      mxgg_int4               = mxsp_int4;
      mxgg_lock               = LOCK_MXGG00;
      mxgg_lock_rowid         = LOCK_ROWID_MXGG00;
      mxgg_mb_part1_lockhead  = MB_PART1_LOCKHEAD_MXGG00;
      mxgg_mb_part1_return    = MB_PART1_RETURN_MXGG00;
      mxgg_mb_part1head       = MB_PART1_HEAD_MXGG00;
      mxgg_multhead           = MULT_HEAD_MXGG00;
      mxgg_number             = NUMBER_MXGG00;
      mxgg_page_header        = PAGE_HEADER_MXGG00;
      mxgg_page_trailer       = PAGE_TRAILER_MXGG00;
      mxgg_page_check_trailer = PAGE_CHECK_TRAILER_MXGG00;
      mxgg_record             = MAX_RECLEN_GG00;
      mxgg_rescnt             = RESCNT_MXGG00;
      mxgg_sfp_init_part      = SFP_INIT_PART_MXGG00;
      mxgg_site               = SITE_MXGG00;
      mxgg_stack_entry        = STACK_ENTRY_MXGG00;
      mxgg_stringfd           = STRING_FD_MXGG00;
      mxgg_surrogate          = SURROGATE_MXGG00;
      mxgg_syskey             = SYSKEY_MXGG00;
      mxgg_tempprefix         = TEMP_PREFIX_MXGG00;
      mxgg_tg_upd_set_offset  = TG_UPD_SET_OFFSET_MXGG00;
      mxgg_treeid             = FILE_ID_MXGG00;
      mxgg_treeidhead         = FILE_ID_HEAD_MXGG00;
      mxgg_treepos            = FILE_POS_MXGG00;
      mxgg_userid             = 4;


      mxak00_ofield             =    80;  (* &tak_ofield *)
      cak00_maxordercolumns     =   128;


      cak_maxsources            =  64;
      cak_unicode_mark          = '\00';
      cak_del_trigger_linkage   = '\00\01';
      cak_ins_trigger_linkage   = '\00\03';
      cak_upd_trigger_linkage   = '\00\05';
      cak_init_fileversion      = '\00\00';
      cak_normal_codeset        =      1;
      cak_unicode               =      2;
      cak_init_cache_size       =     32;
      cak_max_align             =   1024;
      cak_default_precision     =     20;
      cak_maxconstlength        =     30;
      cak_maxvarcoliolen        =    255;
      cak_max_named_subtrans    =     50;
      cak_max_constraints       =   1024;
      cak_max_cmd_stack         =     16;
      cak_max_proc              =     64;
      cak_max_resn              =     50;
      cak_max_trigger_level     =     64;
      cak_max_mmarray           =     21;
      cak_maxlinkpos            =   MAX_COL_PER_TAB_GG00 * 2;
      mxak_set                  =   ((MAX_COL_PER_TAB_GG00 + 1 + 15) DIV 16) * 2;
      (* primary pages via secondary key = 100 / cak_maxpercentage    *)
      (* duplicates when more than one strategy is used(random insert)*)
      (* plus building up the result set                              *)
      cak_max_del_qual          =     16;
      cak_ak_float              =      0;
      cak_complex_view_indicator =     4;
      cak_is_undefined          =     -2;
      cak_check_number_length   =  10000;
      cak_max_sm_error          =     16;
      cak_minserverdbs          =      0;
      cak_maxserverdbs          =    255;
      cak_dbproccache           =      5;
      cak_max_trigger_stack     =    900;
      cak_e_parameter           =  21000;
      cak_e_isolation_join      =  21001;
      cak_e_union_in_corr       =  21002;
      cak_e_expr_in_insert      =  21003;
      cak_e_receive_error       =  21004;
      cak_e_subq_type_diff      =  21005;
      cak_e_dbproc_call         =  21007; (* PTS 1112612 *)
      cak_e_use_fetch_rest      =     -2;
      cak_e_table_unloaded      =   -106;
      cak_e_table_or_file_dropped = -108; (* return_code *)
      cak_e_inv_not_found       =  -9012;
      cak_e_cache_full          =  -9400; (* return_code *)
      cak_e_released            =  10000;
      cak_e_roll_released       =  10001;
      cak_e_kb_net_line_down    =    600;
      cak_e_kb_ansi_net_line_down =  -60;
      cak_sysdba_version        =     40;
      mxak_order_errortext      =    256;
      (* PTS 1109925 E.Z. *)
      cak_utcdiff_len           =      4;
      cak_utcdiff_frac          =      2;
      (****************************************************************)
      (* Default values for the mimikry of the Oracle NLS support     *)
      cak_nls_currency      = '$   ';
      cak_nls_date_format   =
            ' DD-MON-YY                                        ';
      cak_nls_date_language = 'ENG';
      cak_nls_numeric_chars = '.,';
      (*--------------------------------------------------------------*)
      (* BEGIN COMPUTE CONSTANTS *)
      (* INCLUDE CONSTANTS FROM XSIZEK *)
      (* INCLUDE CONSTANTS FROM VSP00 *)
      (* INCLUDE CONSTANTS FROM VGG00 *)
      mxak_cache_dir_entry      =          48;
      mxak_standard_sysk        = mxgg_surrogate + 4;
      (*                          4=sizeof(sentrytyp)+sizeof(slinkage) *)
      cak_sysbufferoffset       = cgg_rec_key_offset + mxak_standard_sysk;
      mxak_one_join             =          44;  (* &tak68_one_join     *)
      mxak_comment              =        8068;
      mxak_full_dml_init        =        1232;
      mxak_part_dml_init        =         912;
      mxak_entry_type           =           2;
      mxak_resname_rec_key      =         138; (* mxgg_surrogate +
      2 + 2 * sizeof (tsp_knl_identifier) *)
      mxak_linkparsinfo         =         212  (* &tak_linkparsinfo *);
      mxak_mapset               =         768  (* 3 * mxgg_mapset *);
      mxak_old_long_desc        =          32;
      mxak_selfref_link_info    =           8  (* &tak_selfref_link_info*);
      mxak_usage_offset         =           8  (* 4 * &tsp_int2 *);
      mxak_usertab_offset       =           8  (* 4 * &tsp_int2 *);
      mxak_usagedef             =          12  (* &tak_usagedef *);
      cak_max_usage             =
            (mxgg_record - cak_sysbufferoffset - 8 ) DIV mxak_usagedef;
      mxak_usertabdef           =          12  (* &tak_usertabdef *);
      cak_max_usertab           =
            (mxgg_record - cak_sysbufferoffset - 8) DIV mxak_usertabdef;
      cak_complete_prefix       =           3;
      mxak_complex_entry        =          12  (* &tak_complex_entry *);
      mxak_complexrecminlength  = cak_sysbufferoffset +
            4 + mxgg_treeid + mxak_complex_entry;
      cak_intern_prefix         =           4;
      mxak_columnset            =    mxak_set  (* &tak_columnset *);
      (* PTS 1111574 E.Z. *)
      cak_maxconstraintstack    = (mxgg_record -
            cak_sysbufferoffset - 8 - mxak_columnset - 22) DIV mxgg_stack_entry;
      cak_maxdefaultvalue       = (mxgg_record -
            cak_sysbufferoffset - 4); (* PTS 1108428 *)
      cak_maxpercentage         =          15;
      cak_baseindexoffset       =          12;
      (* PTS 1111576 E.Z. *)
      (* sign for dropped columns *)
      cak_cdropped              = MAX_COL_PER_TAB_GG00 + 1;
      mxak_commandhead          =          20;
      mxak_one_corcolumn        =          16  (* &tak_one_corcolumn *);
      mxak_col_uses_offset      =          32;
      mxak_domainrec_offset     =          56;
      mxak_sequence_offset      =         148;
      mxak_userrec_offset       =         152;
      mxak_synonymrec_offset    =          60;
      cak_frac_offset           =           1;
      cak_initrows              =       10000;
      cak_initpages             =         500;
      cak_initavgrowlen         =         200;
      cak_initinvlist           =          10;
      cak_into_res_fid          =           1;
      cak_intern_res_fid        =           2;
      cak_keycol_low_def        =           0; (* fp_key_low_handl *)
      cak_keycol_low_trunc      =          -1; (* fp_key_low_handl *)
      cak_keycol_low_zero       =           1; (* fp_key_low_handl *)
      cak_max_param_index       =        1024;
      mxak_link_offset          =           6(*3*&tsp_int2, in p_buf!*);
      cak_maxkeyfields          =         512;
      cak_maxcacherest          =         100;
      (* correlation_columns of all tables *)
      cak_maxcorcolumns         =          64;
      cak_maxcorlevel           =           4; (* maxno. of corr-subqueries
      *)
      cak_maxfieldlength        =         254; (* max. of n for char (n) *)
      cak_maxdb2fieldlength     =        4000; (* max. of n for char (n) *)
      cak_maxorafieldlength     =        8000; (* max. of n for char (n) *)
      cak_maxadabasfieldlength  =        8000; (* max. of n for char (n) *)
      cak_maxresulttables       =           2; (* 1 EXTERN, 1 INTERN  *)
      cak_max_role_context      =       32768;
      cak_maxlinkcolumns        =          16;
      cak_maxlinkdef            =          30;
      cak_max_litcoldesc        =         335;
      (* PTS 1111574 E.Z. *)
      mxak_longlit_buf          = mxgg_record - 48;
      (* mxak_all_locks = 1604 MAX_QUALBUF_LOCKS_GG00 * mxgg_lock + 4 *)
      mxak_param_info           =         268  (* &tak_param_info *);
      mxak_procrec_offset       = cak_sysbufferoffset + 2 * mxsp_name + 2 + 2;
      mxak_systembuf            =       32760  (* &tak_systembuffer *);
      mxak_mapset_rec           =         812  (* &tak_mapset_record *);
      mxak_domainrec            =          76  (* &tak_domainrecord *);
      (* PTS 1111574 E.Z. *)
      mxak_constraintrec        = cak_sysbufferoffset + 8 + mxak_columnset + 22 +
            cak_maxconstraintstack * mxgg_stack_entry (* &tak_constraintrecord *);
      mxak_longinfo             =          24  (* &tak_longinfo *);
      cak_maxlonginfo           =
            ( mxgg_record - cak_sysbufferoffset - 4 ) DIV mxak_longinfo;
      mxak_longinfo_licount     = 2 + cak_maxlonginfo * mxak_longinfo;
      cak_init_node             =         512; (*  pars_tree  *)
      cak_next_node             =         256; (*  pars_tree  *)
      cak_max_node              = csp_maxint2; (*  pars_tree  *)
      cak_no_level              =           0;
      cak_lo_level              =           1;
      cak_sa_level              =           2;
      cak_maxnode               =         512; (*  pars_tree  *)
      cak_maxorderlength        =        1020;
      cak_additional_parsinfos  =          25; (* for fixedpos and ...*)
      (* PTS 1111574 E.Z. *)
      cak_maxparsfields         = csp_maxvariables + cak_additional_parsinfos;
      cak_maxshortinfo          =         300  (*old_csp_maxvariables*);
      cak_maxparams             =        2700;
      (*sizeof(tak_shortinforecord/tak_parsinforecord) <= 32KB *)
      cak_maxpcount             =           8; (* needed for parsinfos*)
      (* if max no of stack_entries is 2048 *)
      (* PTS 1111574 E.Z. *)
      mxak_colnamesbuf          = mxgg_record - cak_sysbufferoffset - 4;
      mxak_useridrec            =          28  (* & tak_useridrecord *);
      mxak_sysdbarec            =          32  (* &tak_sysdbarecord *);
      mxak_res_keysbuf          =       13316;
      (* PTS 1111574 E.Z. *)
      mxak_viewtextrec          = mxgg_record  (* &tak_viewtextrecord *);
      cak_max_screen_cols       =          79;
      cak_maxtextlength         =          80;
      mxak_multindex            =         208  (* &tak_multindex *);
      cak_max_index_per_rec     =
            ( mxgg_record - cak_sysbufferoffset - 4 ) DIV mxak_multindex;
      (* PTS 1111574 E.Z. *)
      mxak_vcolumndesc          =          12  (* &tak_vcolumndescription *);
      cak_maxviewdesc           =
            ( mxgg_record - cak_sysbufferoffset - 8 ) DIV mxak_vcolumndesc ;
      (* END PTS 1111574 E.Z. *)
      cak_viewdesc_offset       = cak_sysbufferoffset + 8;
      mxak_viewtext_offset      =          12  (* 6 * &tsp_int2 *);
      mxak_viewtext             = mxgg_record -
            cak_sysbufferoffset - mxak_viewtext_offset;
      mxak_viewtabdef           =          44  (* &tak_viewtabdef *);
      mxak_one_viewjoin         =          24  (* &tak_one_viewjoin *);
      mxak_one_derived_desc     =           8  (* &tak_one_derived_desc *);
      (* view qualification parts *)
      mxak_viewqual_basis_offs  =          12; (* tsp_c2 + tsp_int2 + 4*boolean *)
      mxak_viewqual_stack_offs  =          14; (* tsp_c2 + 2*tsp_int2+2*tsp_int4 *)
      mxak_viewqual_join_offs   =           4; (* tsp_c2 + tsp_int2 *)
      mxak_viewqual_basis       = (mxgg_record - cak_sysbufferoffset -
            mxak_viewqual_basis_offs);
      mxak_viewqual_stack       = (mxgg_record - cak_sysbufferoffset -
            mxak_viewqual_stack_offs);
      mxak_viewqual_join        = (mxgg_record - cak_sysbufferoffset -
            mxak_viewqual_join_offs);
      cak_max_viewqual_tab      = mxak_viewqual_basis DIV mxak_viewtabdef;
      cak_max_viewqual_join     = mxak_viewqual_join  DIV mxak_one_viewjoin;
      cak_max_viewqual_stack    = mxak_viewqual_stack DIV mxgg_stack_entry;
      cak_max_viewqual_data     = cak_max_viewqual_stack * mxgg_stack_entry;
      cak_max_viewqual_derivedcol =
            mxak_viewqual_join DIV mxak_one_derived_desc;
      mxak_union_header         =         188  (* 7 * &tsp_int2 + &tsp_int4
            + &tsp_name + &tgg00_FileId *);
      mxak_union_entry          =          16  (* &tak_union_entry *);
      cak_max_union             = (mxgg_record -
            cak_sysbufferoffset - mxak_union_header) DIV mxak_union_entry;
      cak_maxcomplex            = (mxgg_record -
            cak_sysbufferoffset - mxgg_treeid - 4 ) DIV mxak_complex_entry;
      cak_page80percent         = (mxgg_record * 8 ) DIV 10
            (*                    44 = mxbd_node_header + mxbd_node_trailer *)
            (*                         + cbd_pointersize                    *);
      mxak_parsbuf              = mxgg_record - cak_sysbufferoffset;
      mxak_stored_cmd_buf       = mxgg_record - cak_sysbufferoffset - 8;
      mxak_pars_header          =         296;
      mxak_parsid               =          12;
      mxak_parskey              =           6  (* &tak_parskey - 2 *);
      cak_extern_pos            =           1;
      cak_intern_pos            =           2;
      cak_aux_pos               =           3;
      mxak_base_offset          =           8  (* 4 * &tsp00_Uint1
            + 2 * &tsp_int2 *);
      mxak_change_colinfo       =           4  (* &tak_change_colinfo *);
      mxak_vttabdef             =          24  (* &tak_vttabdef *);
      (* PTS 1111574 E.Z. *)
      mxak_correc               = cak_maxcorcolumns * mxak_one_corcolumn + 16*4 + 8;
      mxak_linfoarr             = mxak_longinfo * cak_maxlonginfo + 4;
      mxak_serverdbnamerec      =     132  (* &tak_serverdbnamerecord *);
      mxak_serverdbrec          =         112  (* &tak_serverdbrecord *);
      mxak_col_func_nesting     =          32;
      mxak_col_stats            =          12;
      (* PTS 1111574 E.Z. *)
      MAX_COL_STATS_AK00        = (MAX_RECLEN_GG00 - cak_sysbufferoffset -
            22 - mxak_columnset) DIV mxak_col_stats;
      (* PTS 1109291 E.Z. *)
      cak_max_pcounts           =         100;
      (* END COMPUTE CONSTANTS *)
      (*--------------------------------------------------------------*)
      cak_complex_select        =    101;
      cak_temp_user_name        =  'TEMP              ';
      cak_domain_user           =  'DOMAIN            ';
      cak_oms_user              =  'OMS               ';
      cak_sys_application       =  'SYS_PROC          ';
      cak_ln_dual               =  'DUAL              ';
      cak_ln_new                =  'NEW               ';
      cak_ln_old                =  'OLD               ';
      cak_ln_show               =  'SHOW              ';
      cak_ln_syscat1            =  'SYS%CAT1          ';
      cak_ln_syscat2            =  'SYS%CAT2          ';
      cak_ln_syserrormap        =  'SYSERRORMAP       ';
      cak_ln_sysmessages        =  'MESSAGES          ';
      cak_ln_sysparseid         =  'SYSPARSEID        ';
      cak_ln_system             =  'SYSTEM            ';
      cak_u_public              =  'PUBLIC            ';
      cak_u_superdba            =  'SUPERDBA          ';
      cak_u_sys                 =  'SYSDD             ';
      cak_u_oracle_sys          =  'SYS               ';
      cak_util_userid           =  '\00\00\00\01';
      cak_nil_group_id          =  '\00\00\00\00\00\00\00\00';
      cak_util_user_id          =  '\FF\FF\00\00\00\00\00\01';
      cak_public_id             =  '\00\00\00\00\00\00\00\02';
      cak_public_user_id        =  '\FF\FF\00\00\00\00\00\03';
      cak_temp_user_id          =  '\00\00\00\00\00\00\00\04';
      cak_sys_user_id           =  '\00\00\00\00\00\00\00\05';
      cak_zerotableid           =  cgg_zero_id;
      cak_temp_info_linkage     =  '\FF\FF';
      cak_zero_linkage          =  '\00\00';
      cak_init_linkage          =  '\00\01';
      cak_snap_viewbase_linkage =  '\00\0B';
      cak_zero_site             =  '\00\00';
      cak_fromseltab_site       =  '\FF\FE';
      cak_temp_table_site       =  '\FF\FD';
      cak_all_site              =  '\FF\FC';
      cak_show_table_site       =  '\FF\FB';
      cak_type_site             =  '\FF\F9';
      cak_class_site            =  '\FF\F8';
      cak_dbfunc_site           =  '\FF\F7';
      cak_max_site              =  '\FF\FF';
      (* PTS 1115978 E.Z. *)
      cak_ln_column             =  'COLUMN            ';
      cak_ln_columnname         =  'COLUMNNAME        ';
      cak_ln_columntype         =  'MOD               ';
      cak_ln_datatype           =  'DATATYPE          ';
      cak_ln_date               =  'DATE              ';
      cak_ln_default            =  'DEFAULT           ';
      cak_ln_description        =  'DESCRIPTION       ';
      cak_ln_domainname         =  'DOMAINNAME        ';
      cak_ln_frac               =  'DEC               ';
      cak_ln_grantor            =  'GRANTOR           ';
      cak_ln_indexname          =  'INDEXNAME         ';
      cak_ln_length             =  'LEN               ';
      cak_ln_order              =  'SORT              ';
      cak_ln_password           =  'PASSWORD          ';
      cak_ln_privileges         =  'PRIVILEGES        ';
      cak_ln_program            =  'PROGRAM           ';
      cak_ln_state              =  'STATE             ';
      cak_ln_synonymname        =  'SYNONYMNAME       ';
      cak_ln_sysdba             =  'SYSDBA            ';
      cak_ln_table              =  'TABLE             ';
      cak_ln_tablename          =  'TABLENAME         ';
      cak_ln_text               =  'TEXT              ';
      cak_ln_time               =  'TIME              ';
      cak_ln_type               =  'TYPE              ';
      cak_ln_user               =  'USER              ';
      cak_ln_username           =  'USERNAME          ';
      cak_ln_value              =  'VALUE             ';
      cak_n_tablekey            =  'SYSKEY            ';
      cak_return_req            = true;
      cak_x_cascade             =    1;
      cak_x_restrict            =    2;
      cak_x_set_null            =    3;
      cak_x_set_default         =    4;
      cak_x_date                =   45;
      cak_x_default             =   53;
      cak_x_false               =   78;
      cak_x_null                =  129;
      cak_x_stamp               =  192;
      cak_x_time                =  205;
      cak_x_timestamp           =  211;
      cak_x_transaction         =  435;
      cak_x_true                =  215;
      cak_x_user                =  219;
      cak_x_usergroup           =  220;
      cak_x_uid                 =  221;
      (* used in VAK11 and VAK17 *)
      cak_x_beginload           =   1;
      cak_x_create_table        =   3;
      cak_x_describe_table      =   7;
      cak_x_drop_table          =   9;
      cak_x_drop_view           =  10;
      cak_x_end_incopy_table    =  11;
      cak_x_end_load            =  12;
      cak_x_incopy_table        =  14;
      cak_x_like                =  15;
      cak_x_load                =  16;
      cak_x_restore             =  18;
      cak_x_save_init           =  19;
      cak_x_restore_string      =  20;
      cak_x_save_short_string   =  21;
      cak_x_unload_table        =  51;
      (* PTS 1111576 E.Z. *)
      (* used in VAK12 *)
      cak_x_create_domain       =  20;
      cak_x_drop_domain         =  21;
      cak_x_create_type         =  22;
      cak_x_drop_type           =  23;
      cak_x_create_dbproc       =  24;
      cak_x_drop_dbproc         =  25;
      cak_x_create_package      =  26;
      cak_x_create_class        =  27;
      cak_x_drop_package        =  28;
      cak_x_drop_class          =  29;
      (* used in VAK11 and VAK13 and VAK14 *)
      cak_x_alter_add           =  22;
      cak_x_alter_drop          =  23;
      cak_x_alter_alter         =  24;
      cak_x_drop_constraint     =  26;
      cak_x_add_constraint      =  27;
      cak_x_alter_constraint    =  28;
      cak_x_default_def         =  29;
      cak_x_primary_key         =  30;
      cak_x_alter_dynamic       =  31;
      cak_x_modify              =  32;
      cak_x_sample              =  33;
      cak_x_alter_fact          =  34;
      cak_x_alter_dimension     =  35;
      cak_x_alter_bwhierarchy   =  36;
      cak_x_alter_type          =  37;
      (* used in VAK16 *)
      cak_x_create_view         =  0;
      (* PTS 1111576 E.Z. *)
      cak_x_end_restore         =  32;
      cak_x_save_string         =  33;
      (* used in VAK21 *)
      cak_x_create_user         =  37;
      cak_x_user_like           =  38;
      cak_x_alter_password      =  39;
      cak_x_drop_user           =  40;
      cak_x_alter_user          =  41;
      cak_x_grant_user          =  42;
      cak_x_rename_user         =  43;
      cak_x_user_group          =  100;
      (* used in VAK22 *)
      cak_x_one_grant_spec      =   3;
      (* used in VAK23 *)
      cak_x_create_synonym      =  43;
      cak_x_drop_synonym        =  44;
      cak_x_create_sequence     =  54;
      cak_x_drop_sequence       =  55;
      cak_x_create_role         =  56;
      cak_x_drop_role           =  57;
      (* used in VAK24 *)
      cak_x_create_index        =  45;
      cak_x_create_unique_index =  46;
      cak_x_drop_index          =  47;
      (* used in VAK25 *)
      cak_x_create_link         =  49;
      cak_x_drop_link           =  50;
      cak_x_drop_foreign_key    =  51;
      (* constants for create sequence *)
      cak_x_increment           = 1;
      cak_x_start               = 2;
      cak_x_maxvalue            = 3;
      cak_x_minvalue            = 4;
      (* constants for utility syntax VAK30, VAK36, VAK37 *)
      cak_x_util_commit         =  2;
      cak_x_commit_release      =  3;
      cak_x_alter_config        =  4;
      cak_x_clear_log           =  6;
      cak_x_get_config          =  7;
      cak_x_get_dev_config      =  8;
      cak_x_init_config         =  9;
      cak_x_restart             =  10;
      cak_x_restore_db          =  11;
      cak_x_restore_pages       =  12;
      cak_x_restore_log         =  13;
      cak_x_restore_config      =  14;
      cak_x_save_log            =  15;
      cak_x_save_database       =  16;
      cak_x_save_pages          =  17;
      cak_x_save_log_cold       =  18;
      cak_x_verify              =  19;
      cak_x_rest_log_from       =  20;
      (* PTS 1115982 E.Z. *)
      cak_x_restore_parameters  =  22;
      cak_x_util_rollback       =  23;
      cak_x_init_after_load     =  37;
      cak_x_upd_statistics      =  38;
      (* PTS 1111289 E.Z. *)
      cak_x_shutdown            =  42;
      cak_x_write_on            =  45;
      cak_x_write_off           =  46;
      (* PTS 1111576 E.Z. *)
      cak_x_set_log_demo        =  51;
      cak_x_set_log_config      =  52;
      cak_x_write_halt          =  53;
      (* PTS 1115978 E.Z. *)
      cak_x_set_nolog_off       =  57;
      cak_x_add_devspace        =  58;
      cak_x_adevconfig_rest     =  59;
      cak_x_timeout_on          =  62;
      cak_x_timeout_off         =  63;
      cak_x_updstat_on          =  64;
      cak_x_updstat_off         =  65;
      cak_x_diagnose            =  80;
      cak_x_vtrace_on           =  97;
      cak_x_vtrace_off          =  98;
      cak_x_diag_index          =  101;
      cak_x_diag_dump           =  102;
      cak_x_diag_commit_wait    =  119;
      cak_x_activate            =  122;
      cak_x_upd_stat_col        =  123;
      cak_x_create_serverdb     =  128;
      cak_x_tab_on_write        =  129;
      cak_x_tab_off_write       =  130;
      cak_x_commit_trans        =  133;
      cak_x_rollback_trans      =  134;
      cak_x_alter_mapset        =  135;
      cak_x_create_mapset       =  136;
      cak_x_drop_mapset         =  137;
      cak_x_get_mapset          =  138;
      cak_x_alter_termset       =  139;
      cak_x_create_termset      =  140;
      cak_x_drop_termset        =  141;
      cak_x_get_termset         =  142;
      cak_x_on_diag_parse       =  147;
      cak_x_off_diag_parse      =  148;
      (* PTS 1115978 E.Z. *)
      cak_x_read_label          =  150;
      cak_x_write_label         =  151;
      cak_x_end                 =  152;
      cak_x_end_clear           =  153;
      cak_x_alter_serverdb      =  154;
      cak_x_autosave            =  155;
      cak_x_monitor             =  156;
      cak_x_force_checkpoint    =  157;
      cak_x_get_restart_version =  158;
      cak_x_set_event           =  159;
      cak_x_delete_event        =  160;
      cak_x_diagnose_analyze    =  161;
      cak_x_switch              =  162;
      cak_x_set_parameter       =  163;
      cak_x_delete_log_from_to  =  164; (* PTS 1108247 E.Z. *)
      cak_x_init_standby        =  165; (* PTS 1111229 E.Z. *)
      cak_x_prepare_standby     =  166; (* PTS 1111229 E.Z. *)
      cak_x_restart_standby     =  167; (* PTS 1111229 E.Z. *)
      cak_x_register_standby    =  168; (* PTS 1111229 E.Z. *)
      cak_x_insert_standby      =  169; (* PTS 1111229 E.Z. *)
      cak_x_remove_standby      =  170; (* PTS 1111229 E.Z. *)
      cak_x_takeover            =  171; (* PTS 1111229 E.Z. *)
      cak_x_synchronize_log     =  172; (* PTS 1111229 E.Z. *)
      cak_x_log_devspace_on     =  180; (* PTS 1113190 E.Z. *)
      cak_x_log_devspace_off    =  181; (* PTS 1113190 E.Z. *)
      cak_x_direct              = 1000;
      (* used together with a43 *)
      cak_x_explain             =  4;
      (* used together with a51 *)
      cak_x_connect             =  1;
      cak_x_current_user        =  2;
      cak_x_set_lang            =  3;
      cak_x_set_off_timeout     =  4;
      cak_x_set_on_timeout      =  5;
      cak_x_set_format          =  6;
      cak_x_set_degree          =  7;
      cak_x_get_special_register=  8;
      cak_x_set_isolevel        =  9;
      cak_x_nls_currency        =  10;
      cak_x_nls_date_format     =  11;
      cak_x_nls_date_language   =  12;
      cak_x_nls_numeric_chars   =  13;
      cak_x_set_session_trace   =  14;
      cak_x_set_role            =  15;
      cak_x_get_code_tables     =  16;
      (* PTS 1102256 E.Z. *)
      cak_x_wait_for_checkpoint =  17;
      (* PTS 1106574 E.Z. *)
      cak_x_enable_write_trans  =  18;
      (* PTS 1107004 T.A. *)
      cak_x_set_write_trans     =  19;
      (* used together with a52 *)
      cak_x_commit              =   1;
      cak_x_rollback            =   2;
      cak_x_release             =   3;
      cak_x_lock                =   4;
      cak_x_nowait              =   5;
      cak_x_table               =   6;
      cak_x_rows                =   7;
      cak_x_row                 =   8;
      cak_x_exclusive           =   9;
      cak_x_share               =  10;
      cak_x_current_unused      =  11; (* unused *)
      cak_x_unlock              =  12;
      cak_x_begin_subtrans      =  13;
      cak_x_end_subtrans        =  14;
      cak_x_rollback_subtrans   =  15;
      cak_x_optimistic          =  16;
      cak_x_force_savepoint     =  17;
      (* used together with a55 *)
      cak_x_search_clause       =  1;
      cak_x_keyspec_list        =  2;
      cak_x_keyspec             =  3;
      cak_x_current_of          =  4;
      cak_x_pos_of              =  5;
      (* used together with a56 *)
      cak_x_normal_insert       =  1;
      cak_x_enum_values         =  2;
      cak_x_column_list         =  3;
      cak_x_value_list          =  4;
      cak_x_ignore_dupl         =  5;
      cak_x_update_dupl         =  6;
      cak_x_reject_dupl         =  7;
      cak_x_named_values        =  8;
      cak_x_set_clause          =  9;
      cak_x_insert_select       = 10;
      cak_x_minsert             = 11;
      cak_x_pseudo_ins_select   = 12;
      (* used together with a57 *)
      cak_x_update              =  1;
      cak_x_update_set          =  2;
      cak_x_one_update_col      =  3;
      cak_x_mupdate             =  4;
      (* PTS 1111576 E.Z. *)
      cak_x_complete_refresh    =  6;
      cak_x_append              =  7;
      (* used together with a58 *)
      cak_x_delete              =  1;
      cak_x_mdelete             =  2;
      (* used together with a60 *)
      cak_x_select_list         =   1;
      cak_x_select_column       =   2;
      cak_x_single_select       =   3;
      cak_x_lock_nowait         =   4;
      cak_x_lock_exclusive      =   5;
      cak_x_lock_share          =   6;
      cak_x_lock_optimistic     =   7;
      cak_x_lock_ignore         =   8;
      (* used together with a62 *)
      cak_x_select_direct       =   1;
      cak_x_select_first        =   2;
      cak_x_select_last         =   3;
      cak_x_select_next         =   4;
      cak_x_select_prev         =   5;
      cak_x_index_spec          =   6;
      cak_x_indexvalues         =   7;
      cak_x_indexname_spec      =   8;
      cak_x_indexnamevalues     =   9;
      cak_x_indexn_valuelist    =  10;
      (* used together with a63 *)
      cak_x_mass_select         =   1;
      cak_x_distinct            =   2;
      cak_x_from_part           =   3;
      cak_x_decl_cursor         =   4;
      cak_x_order               =   5;
      cak_x_for_update          =   6;
      cak_x_group_by            =   7;
      cak_x_having              =   8;
      cak_x_sort_spec           =   9;
      cak_x_search_condition    =  10;
      cak_x_for_reuse           =  11;
      cak_x_subquery            =  12;
      cak_x_start_union         =  13;
      cak_x_union               =  14;
      cak_x_union_all           =  15;
      cak_x_except              =  16;
      cak_x_except_all          =  17;
      cak_x_intersect           =  18;
      cak_x_intersect_all       =  19;
      cak_x_res_delete          =  20;
      cak_x_res_no_delete       =  21;
      cak_x_for_fetch_only      =  23;
      cak_x_recursive_select    =  24;
      cak_x_sel_after_recursive =  25;
      cak_x_new_columnname_list =  26;
      cak_x_one_val_subquery    =  27;
      cak_x_arith_with_subquery =  28;
      (* used together with a64 *)
      cak_x_or_operator         =   1;
      cak_x_and_operator        =   2;
      cak_x_predicate           =   3;
      cak_x_add_operator        =   4;
      cak_x_mult_operator       =   5;
      cak_x_between_pred        =   6;
      cak_x_in_pred             =   7;
      cak_x_in_pred_list        =   8;
      cak_x_like_pred           =   9;
      cak_x_sounds_pred         =  10;
      cak_x_duration            =  11;
      cak_x_left_outer_join     =  12;
      cak_x_right_outer_join    =  13;
      cak_x_full_outer_join     =  14;
      cak_x_escape              =  15;
      cak_x_truth_value         =  16;
      cak_x_value_expression    =  17;
      (* used together with a66 *)
      cak_x_given_sequence      =   1;
      cak_x_table_spec          =   2;
      cak_x_select_in_from_part =   3;
      (* used together with a73 *)
      cak_x_fetch               =   1;
      cak_x_mfetch              =   2;
      cak_x_first               =   3;
      cak_x_next                =   4;
      cak_x_last                =   5;
      cak_x_prev                =   6;
      cak_x_same                =   7;
      cak_x_pos                 =   8;
      cak_x_params              =   9;
      cak_x_close               =  10;
      cak_x_describe            =  11;
      cak_x_drop                =  12;
      cak_x_stamp_get           =  13;
      (* PTS 1111576 E.Z. *)
      cak_x_describe_parsid     =  15;
      cak_x_next_long           =  16;
      cak_x_relative            =  17;
      (* used together with a80 *)
      cak_x_select_hint         =   1;
      cak_x_delete_hint         =   2;
      cak_x_insert_hint         =   3;
      cak_x_update_hint         =   4;
      cak_x_ordered_hint        =   5;
      cak_x_buildresult_hint    =   6;
      cak_x_keyaccess_hint      =   7;
      cak_x_keyscan_hint        =   8;
      cak_x_indexaccess_hint    =   9;
      cak_x_indexscan_hint      =  10;
      cak_x_enablemerge_hint    =  11;
      cak_x_disableinvonly_hint =  12;
      cak_x_trace_hint          =  13;
      cak_x_L1_strat_hint       =  14;
      cak_x_L2_strat_hint       =  15;
      cak_x_L2_more_strat_hint  =  16;
      cak_x_L2_range_strat_hint =  17;
      cak_x_L2_strat_no_optim_hint =  18;
      cak_x_in_strat_hint       =  19;
      cak_x_subq_strat_hint     =  20;
      (* fp_consts *)
      cak_fp_date               =  -1;
      cak_fp_time               =  -2;
      cak_fp_timestamp          =  -3;
      cak_fp_utcdate            =  -4;
      cak_fp_utcdiff            =  -5;
      cak_fp_timezone           =  -6;
      cak_fp_stamp              =  -7;
      cak_fp_transaction        =  -8;
      cak_fp_nextval            =  -9; (* must be last of cak_fp_..*)
      (* consts in parsid together with m_usage *)
      cak_usage_on              =   1;
      cak_usage_on_except_trigger=  2;
      cak_usage_off             =   3;
      cak_usage_add             =   4;
      (* char-constants *)
      cak_tempinfo_byte         =  csp_undef_byte;
      (* entrytyp *)
      cak_edummy                =  '\00\00';
      cak_etable                =  '\00\01';
      cak_ctable                =    '\01';
      cak_eunused               =  '\00\05';
      cak_epriv                 =  '\00\06';
      cak_cpriv                 =    '\06';
      cak_eprivuser             =  '\00\07';
      cak_cprivuser             =    '\07';
      cak_eviewtext             =  '\00\08';
      cak_cviewtext             =    '\08';
      cak_eviewdesc             =  '\00\0A';
      cak_cviewdesc             =    '\0A';
      (*   evfile                =  '\00\0E' in vgg00   *)
      cak_elinkdef              =  '\00\0F';
      cak_clinkdef              =    '\0F';
      cak_eprivproc             =  '\00\10';
      cak_cprivproc             =    '\10';
      cak_edefault              =  '\00\11';
      cak_cdefault              =    '\11';
      cak_econstraint           =  '\00\12';
      cak_cconstraint           =    '\12';
      cak_eforeignkey           =  '\00\13';
      cak_cforeignkey           =    '\13';
      cak_eprimarykey           =  '\00\14';
      cak_cprimarykey           =    '\14';
      cak_etrigger              =  '\00\15';
      cak_ctrigger              =    '\15';
      cak_econstraintname       =  '\00\16';
      cak_cconstraintname       =    '\16';
      cak_ealterdate            =  '\00\17';
      cak_calterdate            =    '\17';
      cak_epermmessblock        =  '\00\18';
      cak_cpermmessblock        =    '\18';
      cak_econtainer            =  '\00\19';
      cak_ccontainer            =    '\19';
      cak_eviewqual_basis       =  '\00\20';
      cak_cviewqual_basis       =    '\20';
      cak_eviewqual_join        =  '\00\21';
      cak_cviewqual_join        =    '\21';
      cak_eviewqual_stack       =  '\00\22';
      cak_cviewqual_stack       =    '\22';
      cak_eviewqual_expcol      =  '\00\23';
      cak_cviewqual_expcol      =    '\23';
      cak_eviewqual_derivedcol  =  '\00\24';
      cak_cviewqual_derivedcol  =    '\24';
      cak_esysfile2             =  '\00\40';
      cak_etableref             =  '\00\41';
      cak_ctableref             =    '\41';
      cak_euser                 =  '\00\42';
      cak_cuser                 =    '\42';
      cak_eserverdb             =  '\00\43';
      cak_cserverdb             =    '\43';
      cak_esiteset              =  '\00\44';
      cak_csiteset              =    '\44';
      cak_eserverdbname         =  '\00\45';
      cak_cserverdbname         =    '\45';
      cak_edomain               =  '\00\46';
      cak_cdomain               =    '\46';
      cak_eusertab              =  '\00\47';
      cak_cusertab              =    '\47';
      cak_eusage                =  '\00\48';
      cak_cusage                =    '\48';
      cak_esysdba               =  '\00\49';
      cak_csysdba               =    '\49';
      cak_eprocpriv             =  '\00\4B';
      cak_cprocpriv             =    '\4B';
      cak_euserref              =  '\00\4D';
      cak_cuserref              =    '\4D';
      cak_edomainref            =  '\00\4E';
      cak_cdomainref            =    '\4E';
      cak_etermset              = cgg_etermset;
      cak_ctermset              =    '\4F';
      cak_emapset               = cgg_emapset;
      cak_cmapset               =    '\51';
      cak_esequence             =  '\00\52';
      cak_csequence             =    '\52';
      cak_esequencepriv         =  '\00\53';
      cak_csequencepriv         =    '\53';
      cak_db2esiteset           =  '\00\54';
      cak_db2csiteset           =    '\54';
      cak_euserid               =  '\00\55';
      cak_cuserid               =    '\55';
      (* PTS 1111576 E.Z. *)
      cak_eseqfree              =  '\00\57';
      cak_cseqfree              =    '\57';
      cak_efuncref              =  '\00\58';
      cak_cfuncref              =    '\58';
      cak_efunccode             =  '\00\59';
      cak_cfunccode             =    '\59';
      cak_ecomment              =  '\00\60';
      cak_ccomment              =    '\60';
      cak_edomainusage          =  '\00\61';
      cak_cdomainusage          =    '\61';
      (* PTS 1111576 E.Z. *)
      cak_estatistics           =  '\00\66';
      cak_cstatistics           =    '\66';
      cak_emindex               =  '\00\67';
      cak_cmindex               =    '\67';
      cak_eroletab              =  '\00\68';
      cak_croletab              =    '\68';
      cak_emethod               =  '\00\69';
      cak_cmethod               =    '\69';
      cak_emethodref            =  '\00\70';
      cak_cmethodref            =    '\70';
      cak_etype_uid             =  '\00\71';
      cak_ctype_uid             =    '\71';
      cak_emethodinfo           =  '\00\72';
      cak_cmethodinfo           =    '\72';
      cak_eclassmap             =  '\00\73';
      cak_cclassmap             =    '\73';
      cak_eversion              =  '\00\74';
      cak_cversion              =    '\74';
      cak_eschemaref            =  '\00\75';
      cak_cschemaref            =    '\75';
      cak_eschema               =  '\00\76';
      cak_cschema               =    '\76';
      cak_etemp                 =  '\00\80';
      cak_ctemp                 =    '\80';
      cak_eparsinfo             =  '\00\81';
      cak_cparsinfo             =    '\81';
      cak_ejparsinfo            =  '\00\82';
      cak_cjparsinfo            =    '\82';
      cak_ecorrinfo             =  '\00\83';
      cak_ccorrinfo             =    '\83';
      cak_eshortinfo            =  '\00\84';
      cak_cshortinfo            =    '\84';
      cak_estratinfo            =  '\00\86';
      cak_cstratinfo            =    '\86';
      cak_eunionrec             =  '\00\87';
      cak_cunionrec             =    '\87';
      cak_eviewkey              =  '\00\88';
      cak_cviewkey              =    '\88';
      cak_ecomplex              =  '\00\89';
      cak_ccomplex              =    '\89';
      (* PTS 1111575 E.Z. *)
      cak_etemppriv             =  '\00\8B';
      cak_ctemppriv             =    '\8B';
      cak_eressiteinfo           = '\00\8D';
      cak_cressiteinfo           =   '\8D';
      cak_elinkparsinfo         =  '\00\94';
      cak_clinkparsinfo         =    '\94';
      cak_etriggerparsinfo      =  '\00\95';
      cak_ctriggerparsinfo      =    '\95';
      cak_esubtrans             =  '\00\96';
      cak_csubtrans             =    '\96';
      cak_estored_cmd           =  '\00\97';
      cak_cstored_cmd           =    '\97';
      cak_echangeinfos          =  '\00\98';
      cak_cchangeinfos          =    '\98';
      cak_elongliteral          =  '\00\99';
      cak_clongliteral          =    '\99';
      cak_elogged_temp          =  '\00\B0';
      cak_clogged_temp          =    '\B0';
      cak_eresult               =  '\00\B1';
      cak_cresult               =    '\B1';
      cak_ereskey               =  '\00\B2';
      cak_creskey               =    '\B2';
      cak_eplonginfo            =  '\00\B4';
      cak_cplonginfo            =    '\B4';
      cak_eresname              =  '\00\B5';
      cak_cresname              =    '\B5';
      cak_etempprocref          =  '\00\B6';
      cak_ctempprocref          =    '\B6';
      cak_etemptableref         =  '\00\B7';
      cak_ctemptableref         =    '\B7';
      cak_etempdefault          =  '\00\B8';
      cak_ctempdefault          =    '\B8';
      cak_etempconstraint       =  '\00\B9';
      cak_ctempconstraint       =    '\B9';
      cak_etempviewdesc         =  '\00\BA';
      cak_ctempviewdesc         =    '\BA';
      cak_etempscoldesc         =  '\00\BB';
      cak_ctempscoldesc         =    '\BB';
      cak_etempviewtext         =  '\00\BC';
      cak_ctempviewtext         =    '\BC';
      cak_etmpconstraintname    =  '\00\BD';
      cak_ctmpconstraintname    =    '\BD';
      cak_elinkpos              =  '\00\BE';
      cak_clinkpos              =    '\BE';
      cak_edbprocparsinfo       =  '\00\BF';
      cak_cdbprocparsinfo       =    '\BF';
      cak_ecolnames             =  '\00\C0';
      cak_ccolnames             =    '\C0';
      cak_emessblock            =  '\00\C1';
      cak_cmessblock            =    '\C1';
      cak_ecommand_info         =  '\00\C2';
      cak_ccommand_info         =    '\C2';
      cak_emax                  =  195; (* '00C3' *)
      cak_i_no_keyword          =    0;
      cak_i_abs                 =    1;
      cak_i_absolute            =    2;
      cak_i_acos                =    3;
      cak_i_action              =    4;
      cak_i_activate            =    5;
      cak_i_adabas              =    6;
      cak_i_add                 =    7;
      cak_i_adddate             =    8;
      cak_i_addtime             =    9;
      cak_i_add_months          =   10;
      cak_i_after               =   11;
      cak_i_all                 =   12;
      cak_i_alpha               =   13;
      cak_i_alter               =   14;
      cak_i_analyze             =   15;
      cak_i_and                 =   16;
      cak_i_ansi                =   17;
      cak_i_any                 =   18;
      cak_i_append              =   19;
      cak_i_as                  =   20;
      cak_i_asc                 =   21;
      cak_i_ascii               =   22;
      cak_i_asin                =   23;
      cak_i_at                  =   24;
      cak_i_atan                =   25;
      cak_i_atan2               =   26;
      cak_i_autosave            =   27;
      cak_i_avg                 =   28;
      cak_i_backup_pages        =   29;
      cak_i_before              =   30;
      cak_i_begin               =   31;
      cak_i_beginload           =   32;
      cak_i_beginproc           =   33;
      cak_i_between             =   34;
      cak_i_binary              =   35;
      cak_i_bit                 =   36;
      cak_i_blocksize           =   37;
      cak_i_boolean             =   38;
      cak_i_both                =   39;
      cak_i_break               =   40;
      cak_i_buffer              =   41;
      cak_i_bufferpool          =   42;
      cak_i_bwhierarchy         =   43;
      cak_i_by                  =   44;
      cak_i_byte                =   45;
      cak_i_cache               =   46;
      cak_i_cachelimit          =   47;
      cak_i_caches              =   48;
      cak_i_call                =   49;
      cak_i_cancel              =   50;
      cak_i_cascade             =   51;
      cak_i_case                =   52;
      cak_i_cast                =   53;
      cak_i_catalog             =   54;
      cak_i_catch               =   55;
      cak_i_cccrashhh           =   56;
      cak_i_ceil                =   57;
      cak_i_ceiling             =   58;
      cak_i_char                =   59;
      cak_i_character           =   60;
      cak_i_check               =   61;
      cak_i_checkpoint          =   62;
      cak_i_chr                 =   63;
      cak_i_clear               =   64;
      cak_i_close               =   65;
      cak_i_cluster             =   66;
      cak_i_cold                =   67;
      cak_i_collate             =   68;
      cak_i_column              =   69;
      cak_i_comment             =   70;
      cak_i_commit              =   71;
      cak_i_complete            =   72;
      cak_i_compute             =   73;
      cak_i_concat              =   74;
      cak_i_config              =   75;
      cak_i_connect             =   76;
      cak_i_constraint          =   77;
      cak_i_constraints         =   78;
      cak_i_container           =   79;
      cak_i_continue            =   80;
      cak_i_copy                =   81;
      cak_i_cos                 =   82;
      cak_i_cosh                =   83;
      cak_i_costlimit           =   84;
      cak_i_costwarning         =   85;
      cak_i_cot                 =   86;
      cak_i_count               =   87;
      cak_i_create              =   88;
      cak_i_cross               =   89;
      cak_i_curdate             =   90;
      cak_i_current             =   91;
      cak_i_current_date        =   92;
      cak_i_current_time        =   93;
      cak_i_current_timestamp   =   94;
      cak_i_currval             =   95;
      cak_i_cursor              =   96;
      cak_i_curtime             =   97;
      cak_i_cycle               =   98;
      cak_i_data                =   99;
      cak_i_database            =  100;
      cak_i_date                =  101;
      cak_i_datediff            =  102;
      cak_i_day                 =  103;
      cak_i_dayname             =  104;
      cak_i_dayofmonth          =  105;
      cak_i_dayofweek           =  106;
      cak_i_dayofyear           =  107;
      cak_i_days                =  108;
      cak_i_db2                 =  109;
      cak_i_dba                 =  110;
      cak_i_dbfunction          =  111;
      cak_i_dbproc              =  112;
      cak_i_dbprocedure         =  113;
      cak_i_dbyte               =  114;
      cak_i_db_above_limit      =  115;
      cak_i_db_below_limit      =  116;
      cak_i_dec                 =  117;
      cak_i_decimal             =  118;
      cak_i_declare             =  119;
      cak_i_decode              =  120;
      cak_i_default             =  121;
      cak_i_degree              =  122;
      cak_i_degrees             =  123;
      cak_i_delete              =  124;
      cak_i_desc                =  125;
      cak_i_describe            =  126;
      cak_i_destpos             =  127;
      cak_i_device              =  128;
      cak_i_devspace            =  129;
      cak_i_diagnose            =  130;
      cak_i_digits              =  131;
      cak_i_dimension           =  132;
      cak_i_direct              =  133;
      cak_i_disable             =  134;
      cak_i_distinct            =  135;
      cak_i_div                 =  136;
      cak_i_do                  =  137;
      cak_i_domain              =  138;
      cak_i_double              =  139;
      cak_i_drop                =  140;
      cak_i_dsetpass            =  141;
      cak_i_duplicates          =  142;
      cak_i_dynamic             =  143;
      cak_i_ebcdic              =  144;
      cak_i_editproc            =  145;
      cak_i_else                =  146;
      cak_i_enable              =  147;
      cak_i_end                 =  148;
      cak_i_endload             =  149;
      cak_i_endpos              =  150;
      cak_i_endproc             =  151;
      cak_i_error               =  152;
      cak_i_escape              =  153;
      cak_i_estimate            =  154;
      cak_i_eur                 =  155;
      cak_i_event               =  156;
      cak_i_except              =  157;
      cak_i_exclusive           =  158;
      cak_i_execute             =  159;
      cak_i_exists              =  160;
      cak_i_exp                 =  161;
      cak_i_expand              =  162;
      cak_i_explain             =  163;
      cak_i_explicit            =  164;
      cak_i_extract             =  165;
      cak_i_fact                =  166;
      cak_i_false               =  167;
      cak_i_fetch               =  168;
      cak_i_file                =  169;
      cak_i_first               =  170;
      cak_i_firstpos            =  171;
      cak_i_fixed               =  172;
      cak_i_float               =  173;
      cak_i_floor               =  174;
      cak_i_flush               =  175;
      cak_i_fnull               =  176;
      cak_i_for                 =  177;
      cak_i_force               =  178;
      cak_i_foreign             =  179;
      cak_i_format              =  180;
      cak_i_fread               =  181;
      cak_i_freepage            =  182;
      cak_i_from                =  183;
      cak_i_full                =  184;
      cak_i_fversion            =  185;
      cak_i_fwrite              =  186;
      cak_i_get                 =  187;
      cak_i_get_objectname      =  188;
      cak_i_get_owner           =  189;
      cak_i_grant               =  190;
      cak_i_graphic             =  191;
      cak_i_greatest            =  192;
      cak_i_group               =  193;
      cak_i_having              =  194;
      cak_i_hex                 =  195;
      cak_i_hextoraw            =  196;
      cak_i_high                =  197;
      cak_i_hold                =  198;
      cak_i_hour                =  199;
      cak_i_hours               =  200;
      cak_i_identified          =  201;
      cak_i_if                  =  202;
      cak_i_ifnull              =  203;
      cak_i_ignore              =  204;
      cak_i_implicit            =  205;
      cak_i_in                  =  206;
      cak_i_increment           =  207;
      cak_i_index               =  208;
      cak_i_indexname           =  209;
      cak_i_indicator           =  210;
      cak_i_info                =  211;
      cak_i_init                =  212;
      cak_i_initcap             =  213;
      cak_i_initrans            =  214;
      cak_i_inner               =  215;
      cak_i_inout               =  216;
      cak_i_inproc              =  217;
      cak_i_insert              =  218;
      cak_i_instance            =  219;
      cak_i_instr               =  220;
      cak_i_int                 =  221;
      cak_i_integer             =  222;
      cak_i_internal            =  223;
      cak_i_intersect           =  224;
      cak_i_into                =  225;
      cak_i_is                  =  226;
      cak_i_iso                 =  227;
      cak_i_isolation           =  228;
      cak_i_jis                 =  229;
      cak_i_join                =  230;
      cak_i_keep                =  231;
      cak_i_key                 =  232;
      cak_i_label               =  233;
      cak_i_language            =  234;
      cak_i_last                =  235;
      cak_i_lastpos             =  236;
      cak_i_last_day            =  237;
      cak_i_lcase               =  238;
      cak_i_leading             =  239;
      cak_i_least               =  240;
      cak_i_left                =  241;
      cak_i_length              =  242;
      cak_i_level               =  243;
      cak_i_lfill               =  244;
      cak_i_like                =  245;
      cak_i_link                =  246;
      cak_i_list                =  247;
      cak_i_lllooppp            =  248;
      cak_i_ln                  =  249;
      cak_i_load                =  250;
      cak_i_local               =  251;
      cak_i_locate              =  252;
      cak_i_lock                =  253;
      cak_i_log                 =  254;
      cak_i_log10               =  255;
      cak_i_logfull             =  256;
      cak_i_log_above_limit     =  257;
      cak_i_long                =  258;
      cak_i_longfile            =  259;
      cak_i_low                 =  260;
      cak_i_lower               =  261;
      cak_i_lpad                =  262;
      cak_i_ltrim               =  263;
      cak_i_makedate            =  264;
      cak_i_maketime            =  265;
      cak_i_mapchar             =  266;
      cak_i_max                 =  267;
      cak_i_maxtrans            =  268;
      cak_i_maxvalue            =  269;
      cak_i_mbcs                =  270;
      cak_i_medianame           =  271;
      cak_i_medium              =  272;
      cak_i_microsec            =  273;
      cak_i_microsecs           =  274;
      cak_i_min                 =  275;
      cak_i_minus               =  276;
      cak_i_minute              =  277;
      cak_i_minutes             =  278;
      cak_i_minvalue            =  279;
      cak_i_mod                 =  280;
      cak_i_mode                =  281;
      cak_i_modify              =  282;
      cak_i_monitor             =  283;
      cak_i_month               =  284;
      cak_i_monthname           =  285;
      cak_i_months              =  286;
      cak_i_months_between      =  287;
      cak_i_natural             =  288;
      cak_i_nchar               =  289;
      cak_i_new                 =  290;
      cak_i_new_time            =  291;
      cak_i_next                =  292;
      cak_i_nextval             =  293;
      cak_i_next_day            =  294;
      cak_i_nlssort             =  295;
      cak_i_nls_currency        =  296;
      cak_i_nls_date_format     =  297;
      cak_i_nls_date_language   =  298;
      cak_i_nls_language        =  299;
      cak_i_nls_numeric_chars   =  300;
      cak_i_nls_sort            =  301;
      cak_i_no                  =  302;
      cak_i_nocache             =  303;
      cak_i_nocycle             =  304;
      cak_i_nolog               =  305;
      cak_i_nomaxvalue          =  306;
      cak_i_nominvalue          =  307;
      cak_i_none                =  308;
      cak_i_noorder             =  309;
      cak_i_norewind            =  310;
      cak_i_normal              =  311;
      cak_i_noround             =  312;
      cak_i_nosort              =  313;
      cak_i_not                 =  314;
      cak_i_now                 =  315;
      cak_i_nowait              =  316;
      cak_i_null                =  317;
      cak_i_num                 =  318;
      cak_i_number              =  319;
      cak_i_numeric             =  320;
      cak_i_nvl                 =  321;
      cak_i_obid                =  322;
      cak_i_object              =  323;
      cak_i_of                  =  324;
      cak_i_off                 =  325;
      cak_i_on                  =  326;
      cak_i_only                =  327;
      cak_i_open                =  328;
      cak_i_optimistic          =  329;
      cak_i_optimize            =  330;
      cak_i_option              =  331;
      cak_i_or                  =  332;
      cak_i_oracle              =  333;
      cak_i_order               =  334;
      cak_i_out                 =  335;
      cak_i_outer               =  336;
      cak_i_overwrite           =  337;
      cak_i_package             =  338;
      cak_i_packed              =  339;
      cak_i_pages               =  340;
      cak_i_param               =  341;
      cak_i_parse               =  342;
      cak_i_parseid             =  343;
      cak_i_password            =  344;
      cak_i_pattern             =  345;
      cak_i_pctfree             =  346;
      cak_i_pctused             =  347;
      cak_i_percent             =  348;
      cak_i_pi                  =  349;
      cak_i_ping                =  350;
      cak_i_pipe                =  351;
      cak_i_pos                 =  352;
      cak_i_power               =  353;
      cak_i_precision           =  354;
      cak_i_prepare             =  355;
      cak_i_prev                =  356;
      cak_i_primary             =  357;
      cak_i_priv                =  358;
      cak_i_privileges          =  359;
      cak_i_proc                =  360;
      cak_i_procedure           =  361;
      cak_i_psm                 =  362;
      cak_i_public              =  363;
      cak_i_quick               =  364;
      cak_i_radians             =  365;
      cak_i_range               =  366;
      cak_i_raw                 =  367;
      cak_i_rawtohex            =  368;
      cak_i_read                =  369;
      cak_i_real                =  370;
      cak_i_recursive           =  371;
      cak_i_references          =  372;
      cak_i_register            =  373;
      cak_i_reject              =  374;
      cak_i_relative            =  375;
      cak_i_release             =  376;
      cak_i_remote              =  377;
      cak_i_remove              =  378;
      cak_i_rename              =  379;
      cak_i_replace             =  380;
      cak_i_resource            =  381;
      cak_i_restart             =  382;
      cak_i_restore             =  383;
      cak_i_restrict            =  384;
      cak_i_return              =  385;
      cak_i_returns             =  386;
      cak_i_reuse               =  387;
      cak_i_revoke              =  388;
      cak_i_rfill               =  389;
      cak_i_right               =  390;
      cak_i_role                =  391;
      cak_i_rollback            =  392;
      cak_i_round               =  393;
      cak_i_row                 =  394;
      cak_i_rowid               =  395;
      cak_i_rowno               =  396;
      cak_i_rownum              =  397;
      cak_i_rows                =  398;
      cak_i_rpad                =  399;
      cak_i_rtrim               =  400;
      cak_i_same                =  401;
      cak_i_sample              =  402;
      cak_i_sapr3               =  403;
      cak_i_save                =  404;
      cak_i_savepoint           =  405;
      cak_i_schema              =  406;
      cak_i_search              =  407;
      cak_i_second              =  408;
      cak_i_seconds             =  409;
      cak_i_segment             =  410;
      cak_i_select              =  411;
      cak_i_selectivity         =  412;
      cak_i_selupd              =  413;
      cak_i_sequence            =  414;
      cak_i_serial              =  415;
      cak_i_serverdb            =  416;
      cak_i_session             =  417;
      cak_i_set                 =  418;
      cak_i_share               =  419;
      cak_i_show                =  420;
      cak_i_shutdown            =  421;
      cak_i_sign                =  422;
      cak_i_sin                 =  423;
      cak_i_sinh                =  424;
      cak_i_smallint            =  425;
      cak_i_some                =  426;
      cak_i_soundex             =  427;
      cak_i_sounds              =  428;
      cak_i_sourcepos           =  429;
      cak_i_space               =  430;
      cak_i_sqlid               =  431;
      cak_i_sqlmode             =  432;
      cak_i_sqrt                =  433;
      cak_i_stamp               =  434;
      cak_i_standard            =  435;
      cak_i_standby             =  436;
      cak_i_start               =  437;
      cak_i_startpos            =  438;
      cak_i_stat                =  439;
      cak_i_state               =  440;
      cak_i_statistics          =  441;
      cak_i_stddev              =  442;
      cak_i_stop                =  443;
      cak_i_storage             =  444;
      cak_i_subdate             =  445;
      cak_i_subpages            =  446;
      cak_i_substr              =  447;
      cak_i_substring           =  448;
      cak_i_subtime             =  449;
      cak_i_subtrans            =  450;
      cak_i_sum                 =  451;
      cak_i_switch              =  452;
      cak_i_synchronize         =  453;
      cak_i_synonym             =  454;
      cak_i_sysdate             =  455;
      cak_i_sysdba              =  456;
      cak_i_tabid               =  457;
      cak_i_table               =  458;
      cak_i_tablespace          =  459;
      cak_i_takeover            =  460;
      cak_i_tan                 =  461;
      cak_i_tanh                =  462;
      cak_i_tape                =  463;
      cak_i_temp                =  464;
      cak_i_termchar            =  465;
      cak_i_then                =  466;
      cak_i_time                =  467;
      cak_i_timediff            =  468;
      cak_i_timeout             =  469;
      cak_i_timestamp           =  470;
      cak_i_timezone            =  471;
      cak_i_to                  =  472;
      cak_i_toidentifier        =  473;
      cak_i_topic               =  474;
      cak_i_to_char             =  475;
      cak_i_to_date             =  476;
      cak_i_to_number           =  477;
      cak_i_trace               =  478;
      cak_i_trailing            =  479;
      cak_i_transaction         =  480;
      cak_i_translate           =  481;
      cak_i_trigger             =  482;
      cak_i_trim                =  483;
      cak_i_true                =  484;
      cak_i_trunc               =  485;
      cak_i_truncate            =  486;
      cak_i_try                 =  487;
      cak_i_type                =  488;
      cak_i_ucase               =  489;
      cak_i_uid                 =  490;
      cak_i_unicode             =  491;
      cak_i_union               =  492;
      cak_i_unique              =  493;
      cak_i_unknown             =  494;
      cak_i_unload              =  495;
      cak_i_unlock              =  496;
      cak_i_until               =  497;
      cak_i_update              =  498;
      cak_i_upper               =  499;
      cak_i_usa                 =  500;
      cak_i_usage               =  501;
      cak_i_user                =  502;
      cak_i_usergroup           =  503;
      cak_i_userid              =  504;
      cak_i_using               =  505;
      cak_i_utcdate             =  506;
      cak_i_utcdiff             =  507;
      cak_i_validproc           =  508;
      cak_i_value               =  509;
      cak_i_values              =  510;
      cak_i_varchar             =  511;
      cak_i_varchar2            =  512;
      cak_i_vargraphic          =  513;
      cak_i_variance            =  514;
      cak_i_varying             =  515;
      cak_i_verify              =  516;
      cak_i_version             =  517;
      cak_i_view                =  518;
      cak_i_vsize               =  519;
      cak_i_vtrace              =  520;
      cak_i_wait                =  521;
      cak_i_week                =  522;
      cak_i_weekofyear          =  523;
      cak_i_when                =  524;
      cak_i_whenever            =  525;
      cak_i_where               =  526;
      cak_i_while               =  527;
      cak_i_with                =  528;
      cak_i_work                =  529;
      cak_i_write               =  530;
      cak_i_year                =  531;
      cak_i_years               =  532;
      cak_i_zoned               =  533;
      cak_maxkeyword            =  533;
      (*=======================================================*)
      (* fileserver constants                                  *)
      (*=======================================================*)
      cak_l_ascii_sq_bracket    =   '\5B';
      cak_r_ascii_sq_bracket    =   '\5D';
      cak_l_ebcdic_sq_bracket_ebc = '\DA';
      cak_r_ebcdic_sq_bracket   =   '\BD';
      mxak_dm_filler1           =      18;


      csp2_dual_log        =    1;
      csp2_single_log      =    2;
      csp2_demo_log        =    3;
      csp2_max_log_devs    =   32;
      csp2_max_data_devs   = 4096; (* power (2, 12) see cserv.pcf *)


      csp3_eo_msg                     = 50000;
      (**)
      (**)
      (* Number range 51000 to 51999 (ak layer) see gsp03_1 *)
      (**)
      (**)
      (* Number range 52000 to 52999 (kb layer) see gsp03_2 *)
      (**)
      (**)
      (* Number range 53000 to 53999 (bd layer) see gsp03_3 *)
      (**)
      csp3_msg                              = 54000;
      csp3_bad_page                         = 54001;
      csp3_sysbuf_storage_exceeded          = 54002;
      csp3_allocat                          = 54003;
      csp3_size_check                       = 54004;
      csp3_xparam_error                     = 54005;
      csp3_illegal_taskid                   = 54006;
      csp3_region_check                     = 54007;
      csp3_no_more_memory                   = 54008;
      csp3_g08x1_excl_check                 = 54009;
      csp3_g20x1_unifill                    = 54010;
      csp3_move_error                       = 54111;
      (**)
      csp3_n_akcache         = 'AK CACHE';
      csp3_n_autosave        = 'AUTOSAVE';
      csp3_n_bufwriter       = 'BUFWRITE';
      csp3_n_btree           = 'B*TREE  ';
      csp3_n_check           = 'CHECK   ';
      csp3_n_checkpoint      = 'CHKPOINT';
      csp3_n_config          = 'CONFIG  ';
      csp3_n_convcache       = 'CONVCACH';
      csp3_n_data            = 'DATA    ';
      csp3_n_datacache       = 'DATACACH';
      csp3_n_distribution    = 'DISTRIB ';
      csp3_n_dyndata         = 'dynDATA ';
      csp3_n_dynpool         = 'dynpool ';
      csp3_n_fill            = 'FILL    ';
      csp3_n_fbm             = 'FBM     ';
      csp3_n_hresult         = 'HRESULT ';
      csp3_n_history         = 'HISTORY ';
      csp3_n_io              = 'I/O     ';
      csp3_n_index           = 'INDEX   ';
      csp3_n_join            = 'JOIN    ';
      csp3_n_lock            = 'LOCK    ';
      csp3_n_log             = 'LOG     ';
      csp3_n_memory          = 'MEMORY  ';
      csp3_n_migration       = 'MIGRAT  ';
      csp3_n_move            = 'MOVE    ';
      csp3_n_net             = 'NET     ';
      csp3_n_obj             = 'OBJECT  ';
      csp3_n_objlog          = 'OBJLOG  ';
      csp3_n_order           = 'ORDER   ';
      csp3_n_param           = 'PARAM   ';
      csp3_n_region          = 'REGION  ';
      csp3_n_restart         = 'RESTART ';
      csp3_n_restore         = 'RESTORE ';
      csp3_n_server          = 'SERVER  ';
      csp3_n_size_check      = 'SIZECHEC';
      csp3_n_save            = 'SAVE    ';
      csp3_n_savepoint       = 'SAVPOINT';
      csp3_n_aksemantic      = 'SEMANTIC';
      csp3_n_show            = 'SHOW    ';
      csp3_n_shutdown        = 'SHUTDOWN';
      csp3_n_syserror        = 'SYSERROR';
      csp3_n_tempcache       = 'TEMPCACH';
      csp3_n_treelock        = 'TREELOCK';
      csp3_n_trigger         = 'TRIGGER ';
      csp3_n_unicode         = 'UNICODE ';


      csp3_ak_msg                     = 51000;
      csp3_a07_system_error           = 51080;
      csp3_a10_clean_cache            = 51101;
      csp3_a10_cmd_rollback           = 51102;
      csp3_a10_all_release            = 51103;
      csp3_a10_rm_parsinfos           = 51104;
      csp3_a10_cache_dump             = 51105;
      csp3_a10_cache_dir_full         = 51106;
      csp3_a10_get_mem                = 51107;
      csp3_a10_hit_rate               = 51108;
      csp3_a10_cache_supplement       = 51109;
      csp3_a10_cache_wrong_lru        = 51110;
      csp3_a10_cache_wrong_less       = 51111;
      csp3_a10_cache_wrong_greater    = 51112;
      csp3_a10_cache_verify           = 51113;
      csp3_a10_cache_error            = 51114;
      csp3_a260_hresult               = 51260;
      csp3_a262_invalid_part_ptr      = 51262;
      csp3_a35_crash_called           = 51351;
      csp3_a36_control_exists         = 51361;
      csp3_a36_ascii_msg_expected     = 51362;
      csp3_a361_no_direct_migration   = 51381;
      csp3_a40_move                   = 51401;
      csp3_a40_move_const             = 51402;
      csp3_a40_move_i4                = 51403;
      csp3_a663_too_few_result_sets   = 51663;
      csp3_a680_mt_join_impossible    = 51681;
      csp3_a680_best_join_invalid     = 51682;
      csp3_a680_too_small_fieldcount  = 51683;
      csp3_a680_inconsistent_lengths  = 51684;
      csp3_a682_sysinfo_kind_invalid  = 51690;
      csp3_a91_no_copyright           = 51911;
      csp3_a92_invalid_part_kind      = 51921;
      csp3_a92_incorrect_part_header  = 51922;
      (* PTS 1111537 E.Z. *)
      csp3_a92_incorrect_partbuf_len  = 51923;
      csp3_a93_unknown_part_kind      = 51931;
      csp3_a93_result_too_long        = 51932;
      csp3_a93_invalid_messtype_afile = 51933;
      csp3_a93_unknown_messtype       = 51934;
      csp3_a94_drop_serverdb_failed   = 51941;
      csp3_a94_restart_failed         = 51942;
      csp3_a94_shutdown_failed        = 51943;
      csp3_a94_release_failed         = 51944;


      STRINVINRANGE_START_MXGG07 =  28; (* header of tgg07_StrInvInRange without keys *)
      STRATEGY_START_MXGG07      =  76; (* StrategyInf - str_catalog *)
      (**)
      INV_DISTINCT_OPTIM_GG07    = -2;
      AUTO_DISTINCT_OPTIM_GG07   =  1;
      NO_DISTINCT_OPTIM_GG07     =  0;
      KEYSEQLEN_OFFSET_GG07      =  MAX_KEYLEN_GG00 + 1;
      (**)
      (* PTS 1111217 E.Z. *)
      MAX_STRATETGY_LEN_GG07     = 236;
      (**)
      MAX_COLPOSARR_IDX_GG07     = MAX_STRATEGY_KEYS_GG00 - 1;
      (**)
      NOT_SEQUENCED_GG07         = -5;
      IS_UNDEFINED_GG07          = -5;
      VARKEY_GG07                = chr(255);


      MAX_LOGPOS_LIST_KB00    =    64;
      MAX_SHORTCOL_LEN_KB00   = MAX_RECLEN_GG00 - cgg_rec_key_offset -
            SURROGATE_MXGG00;
      NIL_LOGENTRY_SEQ_KB00   = '\FF\FF\FF\FF\FF\FF';
      DUMMY_LOGENTRY_SEQ_KB00 = '\FF\FF\FF\FF\FF\FE';
      (**)
      DUMMY_LPNO_KB00         =    -1;
      INDEX_LOCK_BYTE_KB00    =     1;
      PNO_LOG_INFO_KB00       =     2;
      SKIP_COL_BYTE_KB00      =   253;


      (*inlineC #if defined __cplusplus *)
      (*inlineC #include "ggg92.h"      *)
      (*inlineC #endif                  *)
      ITEMS_PER_SUPPLY_SEGM_KB05  =     100;                         (* PTS 1105492 JA 2000-03-15 *)
      MAX_ROW_HASH_KB05           = 2500000;
      MAX_SUPPLY_BLOCK_ITEMS_KB05 =  100000;                         (* PTS 1105492 JA 2000-03-15 *)
      MAX_TAB_HASH_KB05           =   50000;
      MAX_TRANS_HASH_KB05         =    1024;
      MAX_SORTED_ROW_LOCKS_KB05   =    1000;                         (* PTS 1107056 JA 2001-03-01 *)
      NIL_REGION_KB05             =       0;
      OMS_CHECKPT_REGION_KB05     =       1;                         (* PTS 1001607 UH 16-10-1998 *)
      REGIONS_KB05                =      64;
      ROWID_MXKB05                =      56;
      ROWID_POSTFIX_MXKB05        =       8;
      SPACE_BETWEEN_REGIONS_KB05  =      16;                         (* PTS 1105492 JA 2000-03-15 *)
      TIMEOUT_EOT_EXCL_KB05       =     300; (*  5 min *)
      TRANS_PER_OPEN_ENTRY_KB05   =       3;                         (* PTS 1105492 JA 2000-03-15 *)
      (* *)
      MAX_SUPPLY_SEGMENTS_KB05    = MAX_INT4_SP00 DIV ITEMS_PER_SUPPLY_SEGM_KB05; (*JA 2000-03-15 *)
      MAX_REGIONS_KB05            = 3 * REGIONS_KB05; (* 3 = trans + tab + row *)
      MAX_TRANS_REGIONS_KB05      = REGIONS_KB05;
      ROWID_PREFIX_MXKB05         = ROWID_MXKB05 - INT4_MXSP00 - ROWID_POSTFIX_MXKB05;


      (* max length of SHOW-resultrow *)
      mxak40_shinfo               =        5400;


      c_get_all          = true;
      (* h.b. 16.08.1999 always run monitor  *)
      c_monitor_on       = true;
      (* *)
      c_100_pct          = 100;
      c_undefined        = -1;
      (* *)
      c_is_date          = true;
      c_is_null          = true;
      c_inclusive        = true;
      c_null             = true;
      c_to_undef         = true;
      c_reset_statistics = true;
      c_not_in_use       = '\00\00\00\00\00\00\00\00\00\00\00\00';
      n_yes          = 'yes         ';
      n_no           = 'no          ';
      n_on           = 'on          ';
      n_off          = 'off         ';
      n_counting     = 'counting    ';
      n_ak           = 'AK          ';
      n_ascii        = 'ASCII       ';
      n_backup       = 'BACKUP      '; (* PTS 1103957 JA 1999-09-15 *)
      n_bytestr      = 'BYTESTR     ';
      n_datacache    = 'DATACACHE   '; (* PTS 1103957 JA 1999-09-15 *)
      n_default      = 'DEFAULT     ';
      n_delete       = 'DELETE      ';
      n_disconnected = 'disconnected';
      n_ebcdic       = 'EBCDIC      ';
      n_eur          = 'EUR         ';
      n_index        = 'INDEX       ';
      n_insert       = 'INSERT      ';
      n_internal     = 'INTERNAL    ';
      n_iso          = 'ISO         ';
      n_jis          = 'JIS         ';
      n_kb_regions   = 'KB REGIONS  '; (* PTS 1103957 JA 1999-09-15 *)
      n_lock         = 'LOCK        ';
      n_lock_supply  = 'LOCK SUPPLY '; (* PTS 1103957 JA 1999-09-15 *)
      n_log          = 'LOG         '; (* PTS 1103957 JA 1999-09-15 *)
      n_log_queue    = 'LOG QUEUE   '; (* PTS 1103957 JA 1999-09-15 *)
      n_long         = 'LONG        ';
      n_obj_bd       = 'OBJECT      ';
      n_obj_free     = 'OBJECT FREE ';
      n_obj_new      = 'OBJECT ADD  ';
      n_obj_get      = 'OBJECT GET  ';
      n_obj_upd      = 'OBJECT ALTER';
      n_optimize     = 'OPTIMIZE    ';
      n_order        = 'ORDER       ';
      n_order_stndrd = 'ORDER STNDRD';
      n_pages        = 'PAGES       ';
      n_perm         = 'perm        ';
      n_select       = 'SELECT      ';
      n_single_log   = 'SINGLE      ';
      n_table        = 'TABLE       ';
      n_temp         = 'temp        ';
      n_temp_concurr = 'temp concurr';
      n_time         = 'TIME        ';
      n_translist    = 'TRANSLIST   '; (* PTS 1103957 JA 1999-09-15 *)
      n_tree         = 'TREE        '; (* PTS 1103957 JA 1999-09-15 *)
      n_tree_locks   = 'TREE LOCKS  '; (* PTS 1103957 JA 1999-09-15 *)
      n_unicode      = 'UNICODE     ';
      n_unknown      = 'unknown     ';
      n_update       = 'UPDATE      ';
      n_usa          = 'USA         ';
      (* *)
      ln_serverdb    = 'SERVERDB          ';
      ln_log         = 'LOG               ';
      (* *)
      u_dba          = 'DBA     ';
      u_resource     = 'RESOURCE';
      u_standard     = 'STANDARD';
      (* *)
      (* statistic text *)
      (* *)
      n_bad_device       = 'Bad DEVSPACE                  ';
      n_reintegration    = 'DEVSPACE in reintegration     ';
      n_cf_dev_name      = 'DATA DEVSPACE     name        ';
      n_cf_dev_size      = 'DATA DEVSPACE     size        ';
      n_cf_logdev_name   = 'LOG name                      ';
      n_cf_logdev_size   = 'LOG size                      ';
      n_cf01_defaultcode = 'Default code                  ';
      n_cf02_date_time   = 'Date/time format              ';
      n_cf04_sesstimeout = 'SESSION TIMEOUT               ';
      n_cf06_req_timeout = 'REQUEST TIMEOUT               ';
      n_cf08_no_of_logs  = 'No of logs                    ';
      n_cf09_no_of_devs  = 'No of DATA DEVSPACEs          ';
      n_cf10_mirror_dev  = 'Mirrored DEVSPACEs            ';
      n_cf11_converter_sz= 'Converter size                ';
      n_cf12_datacache_sz= 'Datacache size                ';
      n_cf13_awe_mapped  = 'AWE mapped size               ';
      n_cf14_awe_unmapped= 'AWE unmapped size             ';
      n_inv01_cnt_seckey = 'Secondary keys (index lists)  ';
      n_inv02_avg_seclen = 'Avg secondary key length      ';
      n_inv03_min_seclen = 'Min secondary key length      ';
      n_inv04_max_seclen = 'Max secondary key length      ';
      n_inv05_cnt_prim   = 'Primary keys                  ';
      n_inv06_avg_listl  = 'Avg primary keys per list     ';
      n_inv07_min_listl  = 'Min primary keys per list     ';
      n_inv08_max_listl  = 'Max primary keys per list     ';
      n_logmode          = 'Log mode                      ';
      n_log02_pages      = 'Log pages                     ';
      n_log03_used_pages = 'Used log pages                ';
      n_log04_used_pcent = 'Used log pages (%)            ';
      n_log05_notsaved   = 'Not saved log pages           ';
      n_log06_notsav_pct = 'Not saved log pages (%)       ';
      n_log07_since_bup  = 'Log pages since data backup   ';
      n_log11_cnt_savept = 'Savepoints                    ';
      n_log12_cnt_checkp = 'Checkpoints                   ';
      n_segmsize         = 'Log segment size              ';
      n_select_notsel    = 'Values with selectivity >  25%';
      n_select01         = 'Values with selectivity <=  1%';
      n_select05         = 'Values with selectivity <=  5%';
      n_select10         = 'Values with selectivity <= 10%';
      n_select25         = 'Values with selectivity <= 25%';
      (* PTS 1115978 E.Z. *)
      n_st021_monitoring = 'Monitoring                    '; (* h.b. PTS  *)
      n_st022_da         = 'Diagnose Analyze              '; (* h.b. PTS  *)
      n_st023_da_filter  = 'Diagnose Analyze Filter       '; (* h.b. PTS  *)
      n_st031_mon_read   = 'Diagnose monitor read         ';
      n_st032_mon_sel    = 'Diagnose monitor selectivity  ';
      n_st033_mon_sec    = 'Diagnose monitor time         ';
      n_st034_mon_data   = 'Monitor data collection       ';
      n_st035_mon_row    = 'Monitor rows                  ';
      n_st036_sap_db     = 'SERVERDB is for SAP           ';
      n_st04_used        = 'Catalog cache pages used      ';
      n_st05_checkpt     = 'Checkpoint wanted             ';
      n_st06_distrib     = 'Distribution state            ';
      n_st061_bad_index  = 'No of bad indexes             ';
      n_st07_vtrace      = 'Vtrace                        ';
      n_st08_vtrace_chck = 'Vtrace check                  ';
      n_st08a_check      = 'Check                         '; (* PTS 1103957 JA 1999-09-15 *)
      n_st11_vtrace_sess = 'Vtrace session                ';
      (* PTS 1110976 E.Z. *)
      n_st11a_stop_on_err= 'Stop on error     ';
      n_st12_db_full     = 'SERVERDB full                 ';
      n_st13_connect_poss= 'Connect possible              ';
      (* PTS 1107118 E.Z. *)
      n_st132_shutd_ok   = 'Last shutdown finished ok     ';
      m_st133_monitor_oms= 'Monitor OMS started           ';
      n_st18_alog_write  = 'Archive log writing           '; (* h.b. PTS 1113190 *)
      n_tab01_root       = 'Root pno                      ';
      n_tab02_filetype   = 'Filetype                      ';
      n_tab03_nodes      = 'Used  pages                   ';
      n_tab04_indexnodes = 'Index pages                   ';
      n_tab05_leafnodes  = 'Leaf  pages                   ';
      n_tab06_hight      = 'Index levels                  ';
      n_tab07_covering   = 'Space used in all   pages (%) ';
      n_tab08_croot      = 'Space used in root  page  (%) ';
      n_tab09_cindex     = 'Space used in index pages (%) ';
      n_tab10_cmin       = 'Space used in index pages (%) min       ';
      n_tab11_cmax       = 'Space used in index pages (%) max       ';
      n_tab12_cleaf      = 'Space used in leaf  pages (%) ';
      n_tab13_lmin       = 'Space used in leaf  pages (%) min       ';
      n_tab14_lmax       = 'Space used in leaf  pages (%) max       ';
      n_tab15_rows       = 'Rows                          ';
      n_tab16_rows_per   = 'Avg rows per page             ';
      n_tab17_min_rows   = 'Min rows per page             ';
      n_tab18_max_rows   = 'Max rows per page             ';
      n_tab19_avg_rowlen = 'Avg row length                ';
      n_tab20_min_rowlen = 'Min row length                ';
      n_tab21_max_rowlen = 'Max row length                ';
      n_tab22_avg_keylen = 'Avg key length                ';
      n_tab23_min_keylen = 'Min key length                ';
      n_tab24_max_keylen = 'Max key length                ';
      n_tab25_avg_seplen = 'Avg separator length          ';
      n_tab26_min_seplen = 'Min separator length          ';
      n_tab27_max_seplen = 'Max separator length          ';
      n_tab28_def_str    = 'Defined LONG columns          ';
      n_tab29_avg_strlen = 'Avg LONG   column length      ';
      n_tab30_min_strlen = 'Min LONG   column length      ';
      n_tab31_max_strlen = 'Max LONG   column length      ';
      n_tab32_str_pages  = 'LONG column pages             ';
      n_tab33_avg_spages = 'Avg pages per LONG column     ';
      n_tab34_min_spages = 'Min pages per LONG column     ';
      n_tab35_max_spages = 'Max pages per LONG column     ';
      n_termchar_set     = 'TERMCHAR SET                  ';
      (* *)
      n_sys2_cat         = 'SYS_2_CATALOG     ';
      n_obj_file         = 'OBJECT            ';

TYPE

      (*inlineC typedef teo00_Uint1    tsp_int1;      *)
      (*     reason: ptoc int1 is unsigned            *)
      (*inlineC typedef teo00_Uint1    tsp_uint1;     *)
      (*inlineC typedef teo00_Int2     tsp_int2;      *)
      (*inlineC typedef teo00_Uint2    tsp_uint2;     *)
      (*inlineC typedef teo00_Int4     tsp_int4;      *)
      (*inlineC typedef teo00_Uint4    tsp_uint4;     *)
      (*inlineC typedef teo00_Longint  tsp_longint;   *)
      (*inlineC typedef teo00_Longuint tsp_longuint;  *)
      (*inlineC typedef teo00_Bool     tsp00_Bool;    *)
      (*inlineC typedef teo00_Byte     tsp00_Byte;    *)
      (* don't declare tsp00_Int1:                                 *)
      (* tsp_int1 must be unsigned because of Pascal compatibility *)
      (*    declaring tsp00_Int1 as unsigned is misleading!        *)
      (* typedeff teo00_Int1     tsp00_Int1;                       *)
      (* the typedef above is just a comment and should never be enabled *)
      (*inlineC typedef teo00_Uint1    tsp00_Uint1;   *)
      (*inlineC typedef teo00_Int2     tsp00_Int2;    *)
      (*inlineC typedef teo00_Uint2    tsp00_Uint2;   *)
      (*inlineC typedef teo00_Int4     tsp00_Int4;    *)
      (*inlineC typedef teo00_Uint4    tsp00_Uint4;   *)
      (*inlineC typedef teo00_Int      tsp00_Int;     *)
      (*inlineC typedef teo00_Uint     tsp00_Uint;    *)
      (*inlineC typedef teo00_Longint  tsp00_Longint; *)
      (*inlineC typedef teo00_Longuint tsp00_Longuint;*)
      (* Caution: 8 byte integers might be VERY slow on 32 bit machines *)
      (*inlineC typedef teo00_Int8     tsp00_Int8;    *)
      (*inlineC typedef teo00_Uint8    tsp00_Uint8;   *)
      tsp00_Char      = char;
      tsp00_Uint1     = MIN_UINT1_SP00..MAX_UINT1_SP00 (*ptoc00Ignore*);
      tsp00_Int2      = shortint                       (*ptoc00Ignore*);
      tsp00_Uint2     = tsp00_Int2 (*ptoc00Ignore*);  (* +++ do not use in PASCAL +++ *)
      tsp00_Int4      = longint    (*ptoc00Ignore*);
      tsp00_Uint4     = tsp00_Int4 (*ptoc00Ignore*);  (* +++ do not use in PASCAL +++ *)
      tsp00_Longint   = tsp00_Int4 (*ptoc00Ignore*);
      tsp00_Shortreal = real; (* 4 Byte *)
      tsp00_Longreal  = double;
      tsp00_Addr      = ^char;  (* pointer to anything *)
      tsp35proc_ptr   = ^char;  (* pointer to a function,                *)
      (*                 will be substituted by ptoc to tsp35proc_ptr () *)
      tsp00_Byte      = char        (*ptoc00Ignore*);
      tsp00_BytePtr   = ^tsp00_Byte;
      (**)
      tsp_int1      = tsp00_Uint1   (*ptoc00Ignore*);
      tsp_uint1     = tsp00_Uint1   (*ptoc00Ignore*);
      tsp_int2      = tsp00_Int2    (*ptoc00Ignore*);
      tsp_uint2     = tsp00_Uint2   (*ptoc00Ignore*);
      tsp_int4      = tsp00_Int4    (*ptoc00Ignore*);
      tsp_longint   = tsp00_Longint (*ptoc00Ignore*);
      tsp_shortreal = tsp00_Shortreal;
      tsp_longreal  = tsp00_Longreal;
      tsp_addr      = tsp00_Addr;
      (**)
      (* 8 byte counter: integer on 64 bit machine double elsewhere *)
      tsp00_8ByteCounter  = double;
      (**)
      tsp00_PascalComponentID = (
            cidKernel_esp00,
            cidQuickKnl_esp00,
            cidSlowKnl_esp00,
            cidQuery_esp00,
            cidLoad_esp00,
            cidDiagnose_esp00);
      (**)
      tsp00_ColMode = (
            mod_syskey,
            mod_key,
            mod_mandatory,
            mod_optional,
            mod_dropped);
      tsp_col_mode = tsp00_ColMode;
      (**)
      tsp00_ColumnType = (
            gc_fixkey,
            gc_fixkey_ascii,
            gc_fixkey_ebcdic,
            gc_varkey,
            gc_varkey_ascii,
            gc_varkey_ebcdic,
            gc_fixcol,
            gc_fixcol_ascii,
            gc_fixcol_ebcdic,
            gc_varcol,
            gc_varcol_ascii,
            gc_varcol_ebcdic,
            gc_longchar,
            gc_longchar_ascii,
            gc_longchar_ebcdic);
      tsp_column_type = tsp00_ColumnType;
      (**)
      tsp00_DatabaseState = (
            dbCold_esp00,
            dbWarm_esp00);
      (**)
      tsp00_DataType = (
            dfixed,
            dfloat,
            dcha,
            dche,
            dchb,
            drowid,
            dstra,
            dstre,
            dstrb,
            dstrdb,
            ddate,
            dtime,
            dvfloat,
            dtimestamp,
            dunknown,  (* used for typecheck in dml *)
            dnumber,   (*        "      " *)
            dnonumber, (*        "      " *)
            dduration, (*        "      " *)
            ddbyteebcdic,
            dlonga,
            dlonge,
            dlongb,
            dlongdb,
            dboolean,
            dunicode,
            ddtfiller1,
            ddtfiller2,
            ddtfiller3,
            ddtfiller4,
            dsmallint,
            dinteger,
            dvarchara,
            dvarchare,
            dvarcharb,
            dstruni,
            dlonguni,
            dvarcharuni,
            dudt,
            dabaptabhandle,
			dwyde);
      tsp_data_type = tsp00_DataType;
      (**)
      tsp00_Dbcs = (
            no_dbcs,
            dbcs_with_shifts,
            dbcs_no_shifts);
      tsp_dbcs = tsp00_Dbcs;
      (**)
      tsp00_DbObjectType = (
            dbo_method,
            dbo_dbproc,
            dbo_dbfunc,
            dbo_package);
      tsp_db_obj_type = tsp00_DbObjectType;
      (**)
      tsp00_Default = (
            no_default,
            default_user,
            default_usergroup,
            default_date,
            default_time,
            default_stamp,
            default_timestamp,
            default_uid,
            default_true,
            default_false,
            other_default);
      tsp_default = tsp00_Default;
      (**)
      tsp_dev = (
            dev_perm,
            dev_temp);
      (**)
      tsp00_DgcallbackResult = (
            dg_ok,
            dg_cancel,
            dg_again);
      tsp_dgcallback_result = tsp00_DgcallbackResult;
      (**)
      tsp00_DlgBoxStyle = (
            dg_modal,
            dg_modeless);
      tsp_dlgbox_style = tsp00_DlgBoxStyle;
      (**)
      tsp00_ExecMode = (
            async,
            sync_new_session,
            sync_same_session);
      tsp_exec_mode = tsp00_ExecMode;
      (**)
      tsp00_ExecReturn = (
            ex_ok,
            ex_notok,
            ex_no_exec,
            ex_no_async,
            ex_no_sync,
            ex_no_sync_new,
            ex_no_sync_same);
      tsp_exec_return = tsp00_ExecReturn;
      (**)
      tsp00_LcompResult = (
            l_less,
            l_equal,
            l_greater,
            l_undef);
      tsp_lcomp_result = tsp00_LcompResult;
      (**)
      tsp00_NetError = (
            net_ok,
            net_notok,
            net_timeout,
            net_send_line_down,
            net_receive_line_down);
      tsp_net_error = tsp00_NetError;
      (**)
      tsp00_NumError = (
            num_ok,
            num_trunc,
            num_overflow,
            num_invalid,
            num_incompatible,
            num_missing_addr);
      tsp_num_error = tsp00_NumError;
      (**)
      tsp00_Os = (
            os_unix,
            os_vms,
            os_windows,
            os_win32,
            os_os2);
      tsp_os = tsp00_Os;
      (**)
      tsp00_Service = (
            sql_user,
            sql_async_user,
            sql_utility,
            sqlDistribution_esp00,
            sqlControl_esp00,
            sqlEvent_esp00,
            sqlStreamUser_esp00);
      tsp_service = tsp00_Service;
      (* *)
      tsp00_ShutdownMode = (
            shtShutdKill_esp00,
            shtShutdNormal_esp00);
      tsp_shutdown_mode = tsp00_ShutdownMode;
      (**)
      tsp00_SkLabel = (
            no_sk_labels,
            short_sk_labels,
            long_sk_labels);
      tsp_sk_label = tsp00_SkLabel;
      (**)
      tsp00_Sproc = (
            sppr_db_proc,
            sppr_ddl_trigger,
            sppr_insert_trigger,
            sppr_update_trigger,
            sppr_delete_trigger,
            sppr_db_func);
      tsp_sproc = tsp00_Sproc;
      (**)
      tsp_sqlmode = (
            sqlm_ansi,
            sqlm_db2,
            sqlm_oracle,
            sqlm_adabas);
      tsp00_SqlMode = tsp_sqlmode;
      (**)
      tsp00_SwapKind = (
            sw_dummy,
            sw_normal,
            sw_full_swapped,
            sw_part_swapped);
      tsp_swap_kind = tsp00_SwapKind;
      (**)
      tsp00_TaskState = (
            tskSessionWait_esp00,
            tskCommandWait_esp00,
            tskIoWait_esp00,
            tskSystemLockWait_esp00,
            tskUserLockWait_esp00,
            tskRegionWait_esp00,
            tskRunning_esp00,
            tskSleeping_esp00);
      (**)
      tsp00_ToolLayer = (
            cp,
            dg,
            d0,
            d1,
            d2,
            d3,
            d4,
            d5,
            d6,
            d7,
            d8,
            d9,
            fc,
            mt,
            pc,
            qu,
            sp,
            vdg,
            vin,
            vlo,
            vqu,
            wb,
            xx,
            ye,
            na);
      tsp_layer = tsp00_ToolLayer;
      (**)
      tsp00_UniError = (
            spu_ok,
            spu_not_translatable,
            spu_dest_too_short);
      tsp_uni_error = tsp00_UniError;
      (**)
      tsp00_ValMode = (
            vm_datapart,
            vm_alldata,
            vm_lastdata,
            vm_nodata,
            vm_no_more_data,
            vm_last_putval,
            vm_data_trunc,
            vm_close);
      tsp_valmode = tsp00_ValMode;
      (**)
      tsp00_VaReturn = (
            va_ok,
            va_notok,
            va_eof,
            va_wait_first);
      tsp_va_return = tsp00_VaReturn;
      (**)
      tsp00_VFileOpCodes = (
            vread,
            vwrite,
            vappend,
            voverwrite,
            vread_ora);
      tsp_opcodes = tsp00_VFileOpCodes;
      (**)
      tsp00_VfFormat = (
            vf_plaintext,
            vf_record,
            vf_stream,
            vf_unknown,
            vf_4k_block);
      tsp_vf_format = tsp00_VfFormat;
      (**)
      tsp00_VfResource = (
            vf_stack,
            vf_bufferpool);
      tsp_vf_resource = tsp00_VfResource;
      (**)
      tsp00_VfReturn = (
            vf_ok,
            vf_notok,
            vf_eof,
            vf_noseek);
      tsp_vf_return = tsp00_VfReturn;
      (**)
      tsp00_VfType = (
            vf_t_unknown,
            vf_t_file,
            vf_t_pipe,
            vf_t_raw,
            vf_t_tape_norew,
            vf_t_tape_rew,
            vf_t_extrenal_io);
      tsp_vf_type = tsp00_VfType;
      (**)
      tsp00_VpLinefeeds = (
            vp_nolf,
            vp_onelf,
            vp_twolf,
            vp_threelf,
            vp_page);
      tsp_vp_linefeeds = tsp00_VpLinefeeds;
      (**)
      tsp00_VtColor = (
            vt_default_color,
            vt_white,
            vt_black,
            vt_red,
            vt_green,
            vt_yellow,
            vt_blue,
            vt_pink,
            vt_light_blue,
            vt_last_color);(* enter new values before the last one *)
      tsp_vt_color = tsp00_VtColor;
      (**)
      tsp00_VtKey = (
            vt_unknown,
            vt_character_key,
            vt_enter,
            vt_pf01,
            vt_pf02,
            vt_pf03,
            vt_pf04,
            vt_pf05,
            vt_pf06,
            vt_pf07,
            vt_pf08,
            vt_pf09,
            vt_pf10,
            vt_pf11,
            vt_pf12,
            vt_clear,
            vt_do_key,
            vt_sk01,
            vt_sk02,
            vt_sk03,
            vt_sk04,
            vt_sk05,
            vt_sk06,
            vt_sk07,
            vt_sk08,
            vt_sk09,
            vt_help_key,
            vt_up_key,
            vt_down_key,
            vt_left_key,
            vt_right_key,
            vt_cmd_key,
            vt_pick_key,
            vt_put_key,
            vt_mark_key,
            vt_select_key,
            vt_move_key,
            vt_copy_key,
            vt_insert_key,
            vt_delete_key,
            vt_top_key,
            vt_bottom_key,
            vt_cancel_key,
            vt_undo_key,
            vt_end_key,
            vt_escape_key,
            vt_fill,
            vt_file_open,
            vt_file_save,
            vt_exit,
            vt_print,
            vt_find,
            vt_replace,
            vt_cursor_up,
            vt_cursor_down,
            vt_cursor_right,
            vt_cursor_left,
            vt_cursor_home,
            vt_cursor_end,
            vt_next_field,
            vt_prev_field,
            vt_insert_char,
            vt_delete_char,
            vt_rubout,
            vt_del_eof,
            vt_mouse_down,
            vt_mouse_up,
            vt_mouse_move,
            vt_mouse_dbl,
            vt_hscroll,
            vt_vscroll,
            vt_control_key,
            vt_cursor_bol,
            vt_cursor_eol,
            vt_last_key ); (* enter new values before the last one *)
      tsp_vt_key = tsp00_VtKey;
      (**)
      tsp00_VtMark = (
            vt_mark_off,
            vt_mark_block,
            vt_mark_line,
            vt_mark_contiguous,
            vt_mark_matrix);
      tsp_vt_mark = tsp00_VtMark;
      (**)
      tsp00_VtMode = (
            vt_bright,
            vt_inverse,
            vt_underline,
            vt_blink,
            vt_invisible,
            vt_mixed,
            vt_grayed,
            vt_last_mode); (* enter new values before the last one *)
      tsp_vt_mode = tsp00_VtMode;
      (**)
      tsp00_VtScroll = (
            vt_horizontal_scroll,
            vt_vertical_scroll);
      tsp_vt_scroll = tsp00_VtScroll;
      (**)
      tsp00_VtUsageMode = (
            vt_form,
            vt_edit);
      tsp_vt_usage_mode = tsp00_VtUsageMode;
      (**)
      tsp00_Warnings = (
            warn0_exist,
            warn1,
            warn2_null_in_builtin_func,
            warn3_output_not_into_columns,
            warn4_nullwhere,
            warn5,
            warn6_end_of_month_adjustment,
            warn7,
            warn8_scan_or_sort,
            warn9,
            warn10_some_serverdbs_down,
            warn11_time_value_too_long,
            warn12_rowno_used,
            warn13_optional_column_indexed,
            warn14_next_invlist,
            warn15_serverdb_not_in_majority);
      tsp_warnings = tsp00_Warnings;
      (**)
      tsp00_Wlisttype = (
            wls_stream,
            wls_line,
            wls_matrix);
      tsp_wlisttype = tsp00_Wlisttype;
      (**)
      tsp00_XpKey = (
            xp_int2_type,
            xp_int4_type,
            xp_c8_type,
            xp_c18_type,
            xp_c24_type,
            xp_c40_type,
            xp_c64_type,
            xp_crypt_type);
      tsp_xp_key = tsp00_XpKey;
      (**)
      tsp00_XpReturnCode = (
            xp_ok,
            xp_key_not_found,
            xp_eof,
            xp_not_ok);
      tsp_xp_return_code = tsp00_XpReturnCode;
      (**)
      tsp00_C1             = PACKED ARRAY [1..  1] OF char;
      tsp00_C2             = PACKED ARRAY [1..  2] OF char;
      tsp00_C3             = PACKED ARRAY [1..  3] OF char;
      tsp00_C4             = PACKED ARRAY [1..  4] OF char;
      tsp00_C5             = PACKED ARRAY [1..  5] OF char;
      tsp00_C6             = PACKED ARRAY [1..  6] OF char;
      tsp00_C8             = PACKED ARRAY [1..  8] OF char;
      tsp00_C9             = PACKED ARRAY [1..  9] OF char;
      tsp00_C10            = PACKED ARRAY [1.. 10] OF char;
      tsp00_C11            = PACKED ARRAY [1.. 11] OF char;
      tsp00_C12            = PACKED ARRAY [1.. 12] OF char;
      tsp00_C14            = PACKED ARRAY [1.. 14] OF char;
      tsp00_C15            = PACKED ARRAY [1.. 15] OF char;
      tsp00_C16            = PACKED ARRAY [1.. 16] OF char;
      tsp00_C18            = PACKED ARRAY [1.. 18] OF char;
      tsp00_C20            = PACKED ARRAY [1.. 20] OF char;
      tsp00_C24            = PACKED ARRAY [1.. 24] OF char;
      tsp00_C28            = PACKED ARRAY [1.. 28] OF char;
      tsp00_C30            = PACKED ARRAY [1.. 30] OF char;
      tsp00_C32            = PACKED ARRAY [1.. 32] OF char;
      tsp00_C34            = PACKED ARRAY [1.. 34] OF char;
      tsp00_C35            = PACKED ARRAY [1.. 35] OF char;
      tsp00_C36            = PACKED ARRAY [1.. 36] OF char;
      tsp00_C40            = PACKED ARRAY [1.. 40] OF char;
      tsp00_C42            = PACKED ARRAY [1.. 42] OF char;
      tsp00_C48            = PACKED ARRAY [1.. 48] OF char;
      tsp00_C50            = PACKED ARRAY [1.. 50] OF char;
      tsp00_C60            = PACKED ARRAY [1.. 60] OF char;
      tsp00_C64            = PACKED ARRAY [1.. 64] OF char;
      tsp00_C66            = PACKED ARRAY [1.. 66] OF char;
      tsp00_C70            = PACKED ARRAY [1.. 70] OF char;
      tsp00_C80            = PACKED ARRAY [1.. 80] OF char;
      tsp00_C120           = PACKED ARRAY [1..120] OF char;
      tsp00_C132           = PACKED ARRAY [1..132] OF char;
      tsp00_C256           = PACKED ARRAY [1..256] OF char;
      tsp00_C264           = PACKED ARRAY [1..264] OF char;
      tsp00_C512           = PACKED ARRAY [1..512] OF char;
      (**)
      (*inlineC #if defined (__cplusplus)               *)
      (*inlineC typedef tsp00_Void tsp00_AnyPackedChar; *)
      (*inlineC typedef tsp00_Void tsp00_AnyPackedCharc;*)
      (*inlineC #else                                   *)
      tsp00_AnyPackedChar  = PACKED ARRAY [1..ANY_PACKED_CHAR_MXSP00] OF char;
      (*inlineC #endif                                  *)
      tsp00_Buf            = PACKED ARRAY [1..BUF_MXSP00            ] OF char;
      tsp00_Buf8K          = PACKED ARRAY [1..BUF8K_MXSP00          ] OF char;
      tsp00_CryptPw        = tsp00_C24;
      tsp00_CodeType       = tsp00_Uint1;
      tsp00_CompName       = tsp00_C64;
      tsp00_Copyright      = tsp00_C48;
      tsp00_Ctable         = tsp00_C256;
      tsp00_CryptName      = PACKED ARRAY [1..csp_cryptname] OF tsp00_Int4;
      tsp00_Date           = tsp00_C8;
      tsp00_DbName         = tsp00_C18;
      tsp00_Path           = tsp00_C256;
      tsp00_Decimal        = tsp00_C20;
      tsp00_DevName        = tsp00_C40;
      tsp00_ErrText        = tsp00_C40;
      tsp00_ExecProg       = tsp00_C64;
      tsp00_ExecArgLine    = tsp00_C512;
      tsp00_ExecLine       = tsp00_C512;
      tsp00_ExtTimestamp   = PACKED ARRAY [1..EXT_TIMESTAMP_MXSP00] OF char;
      tsp00_FieldRange     = 1..20;
      tsp00_Identifier     = tsp00_C18;
      tsp00_Int2Buf        = PACKED ARRAY [1..BUF_MXSP00] OF tsp00_Int2;
      tsp00_IsolationLevel = tsp00_Int2;
      tsp00_KnlIdentifier  = tsp00_C64;
      tsp00_Key            = PACKED ARRAY [1..KEY_MXSP00] OF char;
      tsp00_Line           = tsp00_C132;
      tsp00_LinePositions  = tsp00_Int2;
      tsp00_Lname          = tsp00_C32;
      tsp00_MoveInt2       = PACKED ARRAY [1..MOVEINT2_MXSP00] OF tsp00_Int2;
      (*inlineC #if defined (__cplusplus)         *)
      (*inlineC typedef tsp00_Void tsp00_MoveObj; *)
      (*inlineC typedef tsp00_Void tsp00_MoveObjc;*)
      (*inlineC #else                             *)
      tsp00_MoveObj        = PACKED ARRAY [1..MOVEOBJ_MXSP00 ] OF char;
      (*inlineC #endif                            *)
      tsp00_Name           = tsp00_C18;
      tsp00_NodeId         = tsp00_C64;
      tsp00_Number         = tsp00_C20;
      tsp00_NumStr         = tsp00_C64;
      tsp00_Opmsg          = tsp00_C64;
      tsp00_Page           = PACKED ARRAY [1..PAGE_MXSP00] OF char;
      tsp00_PageNo         = tsp00_Int4;
      tsp00_PrtLine        = tsp00_C256;
      tsp00_PrtName        = tsp00_C64;
      tsp00_Pw             = tsp00_C18;
      tsp00_Region         = tsp00_C8;
      tsp00_RegionId       = tsp00_Int4;
      tsp00_ResNum         = PACKED ARRAY [1..RESNUM_MXSP00    ] OF char;
      tsp00_ScreenBuf      = PACKED ARRAY [1..SCREEN_BUF_MXSP00] OF char;
      tsp00_Sname          = tsp00_C12;
      tsp00_SqlState       = tsp00_C5;
      tsp00_TaskId         = tsp00_Int4;
      tsp00_TermId         = tsp00_C18;
      tsp00_Time           = tsp00_C8;
      tsp00_Timestamp      = tsp00_C20;
      tsp00_Trace          = tsp00_C16;
      tsp00_UnloadMemory   = tsp00_C264;
      tsp00_Version        = tsp00_C40;
      tsp00_VFilename      = tsp00_C64;
      tsp00_VtAttrib       = PACKED SET OF tsp00_VtMode;
      tsp00_VtColors       = PACKED SET OF tsp00_VtColor;
      tsp00_VtLabel        = tsp00_C8;
      tsp00_VtKeys         = SET OF tsp00_VtKey;
      tsp00_VtMsg          = tsp00_C80;
      tsp00_WarningSet     = PACKED SET OF tsp00_Warnings;
      tsp00_XpKeyType      = tsp00_C18;
      tsp00_Zoned          = tsp00_C20;
      (**)
      tsp_trace            = tsp00_Trace; (* cccprint compatibility *)
      (**)
      tsp00_BoolAddr         = ^boolean;
      tsp_bufaddr            = ^tsp00_Buf;          (* used by PTOC macro *)
      tsp00_BufAddr          = tsp_bufaddr;
      tsp00_KeyAddr          = ^tsp00_Key;
      tsp00_KnlIdentifierPtr = ^tsp00_KnlIdentifier;
      tsp_moveobj_ptr        = ^tsp00_MoveObj;      (* used by PTOC macro *)
      tsp00_MoveObjPtr       = tsp_moveobj_ptr;
      tsp00_ObjAddr          = ^tsp00_AnyPackedChar;
      tsp00_PageAddr         = ^tsp00_Page;
      tsp00_ScreenBufAddr    = ^tsp00_ScreenBuf;
      (**)
      tsp00_KeyPtr         = ^tsp00_Key (*ptoc00Redefine=tsp00_BytePtr*);
      (**)
      tsp_c1   = tsp00_C1;
      tsp_c2   = tsp00_C2;
      tsp_c3   = tsp00_C3;
      tsp_c4   = tsp00_C4;
      tsp_c5   = tsp00_C5;
      tsp_c6   = tsp00_C6;
      tsp_c8   = tsp00_C8;
      tsp_c9   = tsp00_C9;
      tsp_c10  = tsp00_C10;
      tsp_c11  = tsp00_C11;
      tsp_c12  = tsp00_C12;
      tsp_c14  = tsp00_C14;
      tsp_c15  = tsp00_C15;
      tsp_c16  = tsp00_C16;
      tsp_c18  = tsp00_C18;
      tsp_c20  = tsp00_C20;
      tsp_c24  = tsp00_C24;
      tsp_c28  = tsp00_C28;
      tsp_c30  = tsp00_C30;
      tsp_c32  = tsp00_C32;
      tsp_c34  = tsp00_C34;
      tsp_c35  = tsp00_C35;
      tsp_c36  = tsp00_C36;
      tsp_c40  = tsp00_C40;
      tsp_c42  = tsp00_C42;
      tsp_c48  = tsp00_C48;
      tsp_c50  = tsp00_C50;
      tsp_c60  = tsp00_C60;
      tsp_c64  = tsp00_C64;
      tsp_c66  = tsp00_C66;
      tsp_c70  = tsp00_C70;
      tsp_c80  = tsp00_C80;
      tsp_c120 = tsp00_C120;
      tsp_c132 = tsp00_C132;
      tsp_c256 = tsp00_C256;
      tsp_c264 = tsp00_C264;
      tsp_c512 = tsp00_C512;
      (**)
      tsp_any_packed_char    = tsp00_AnyPackedChar;
      tsp_buf                = tsp00_Buf;
      tsp_8kbuf              = tsp00_Buf8K;
      tsp_cryptpw            = tsp00_CryptPw;
      tsp_code_type          = tsp00_CodeType;
      tsp_compname           = tsp00_CompName;
      tsp_copyright          = tsp00_Copyright;
      tsp_ctable             = tsp00_Ctable;
      tsp_cryptname          = tsp00_CryptName;
      tsp_date               = tsp00_Date;
      tsp_dbname             = tsp00_DbName;
      tsp_decimal            = tsp00_Decimal;
      tsp_devname            = tsp00_DevName;
      tsp_errtext            = tsp00_ErrText;
      tsp_execprog           = tsp00_ExecProg;
      tsp_execargline        = tsp00_ExecArgLine;
      tsp_execline           = tsp00_ExecLine;
      tsp_exttimestamp       = tsp00_ExtTimestamp;
      tsp_fieldrange         = tsp00_FieldRange;
      tsp_identifier         = tsp00_Identifier;
      tsp_int2buf            = tsp00_Int2Buf;
      tsp_isolation_level    = tsp00_IsolationLevel;
      tsp_knl_identifier     = tsp00_KnlIdentifier;
      tsp_key                = tsp00_Key;
      tsp_line               = tsp00_Line;
      tsp_linepositions      = tsp00_LinePositions;
      tsp_lname              = tsp00_Lname;
      tsp_moveint2           = tsp00_MoveInt2;
      tsp_moveobj            = tsp00_MoveObj;
      tsp_name               = tsp00_Name;
      tsp_nodeid             = tsp00_NodeId;
      tsp_number             = tsp00_Number;
      tsp_numstr             = tsp00_NumStr;
      tsp_opmsg              = tsp00_Opmsg;
      tsp_page               = tsp00_Page;
      tsp_page_no            = tsp00_PageNo;
      tsp_process_id         = tsp00_TaskId;
      tsp_prtline            = tsp00_PrtLine;
      tsp_prtname            = tsp00_PrtName;
      tsp_pw                 = tsp00_Pw;
      tsp_region             = tsp00_Region;
      tsp_region_id          = tsp00_RegionId;
      tsp_resnum             = tsp00_ResNum;
      tsp_screenbuf          = tsp00_ScreenBuf;
      tsp_sname              = tsp00_Sname;
      tsp_sqlstate           = tsp00_SqlState;
      tsp_termid             = tsp00_TermId;
      tsp_time               = tsp00_Time;
      tsp_timestamp          = tsp00_Timestamp;
      tsp_unload_memory      = tsp00_UnloadMemory;
      tsp_version            = tsp00_Version;
      tsp_vfilename          = tsp00_VFilename;
      tsp_vt_attrib          = tsp00_VtAttrib;
      tsp_vt_colors          = tsp00_VtColors;
      tsp_vtlabel            = tsp00_VtLabel;
      tsp_vt_keys            = tsp00_VtKeys;
      tsp_vtmsg              = tsp00_VtMsg;
      tsp_warningset         = tsp00_WarningSet;
      tsp_xp_key_type        = tsp00_XpKeyType;
      tsp_zoned              = tsp00_Zoned;
      (**)
      tsp_booladdr           = tsp00_BoolAddr;
      tsp_knl_identifier_ptr = tsp00_KnlIdentifierPtr;
      tsp_objaddr            = tsp00_ObjAddr;
      tsp_pageaddr           = tsp00_PageAddr;
      tsp_screenbufaddr      = tsp00_ScreenBufAddr;

      tsp00_TakCacheBuf = RECORD (* used by movcc declared in VSP10 *)
            CASE boolean OF
                true :
                    (word_allignment : tsp00_Int4);
                false :
                    (buf : ARRAY [  1..csp_cntcachebuf  ] OF tsp00_Buf);
                END;
            (*ENDCASE*)

      tsp_takcachebuf = tsp00_TakCacheBuf;

      tsp00_IntMapC2 = RECORD
            CASE boolean OF
                TRUE  :
                    (mapInt_sp00 : tsp00_Int2);
                FALSE :
                    (mapC2_sp00  : tsp00_C2)
                END;
            (*ENDCASE*)


      tsp_int_map_c2 = RECORD
            CASE boolean OF
                TRUE  :
                    (map_int : tsp_int2);
                FALSE :
                    (map_c2  : tsp_c2)
                END;
            (*ENDCASE*)


      tsp00_DataLine = RECORD
            text   : tsp00_Line;
            pos    : tsp00_LinePositions;
            length : tsp00_LinePositions
      END;

      tsp_dataline = tsp00_DataLine;

      tsp00_String = RECORD
            text   : tsp00_Line;
            length : tsp00_LinePositions
      END;

      tsp_string = tsp00_String;

      tsp00_Ccc = RECORD
            count : tsp00_Int4
      END;

      tsp_ccc = tsp00_Ccc;

      tsp00_Numberstack = RECORD (* vsp50 *)
            stackpointer : integer;
            elem         : ARRAY [ 1..csp_numstack ] OF tsp00_Number;
            elem_err     : ARRAY [ 1..csp_numstack ] OF tsp00_NumError
      END;

      tsp_numberstack = tsp00_Numberstack;

      tsp00_SfieldInfo = PACKED RECORD
            mode     : char;
            io_type  : char;
            d_type   : char;
            length   : char;
            CASE boolean OF
                true :
                    (frac        : char;
                    in_out_l     : char;
                    bufpos       : tsp00_Int2);
                false :
                    (longiolen   : tsp00_Int2;
                    bufpos_1     : tsp00_Int2);
                END;
            (*ENDCASE*)
            (* records for communication between kernel and dialog *)
            (* in case of stored procedures                 *)

      tsp_sfield_info = tsp00_SfieldInfo;

      tsp00_ModHeader = RECORD
            spmh_modn  : tsp00_KnlIdentifier;
            spmh_appln : tsp00_KnlIdentifier;
            spmh_user  : tsp00_KnlIdentifier;
            spmh_mtype : tsp00_Uint1;
            spmh_state : char; (* 'R', 'T', 'D', 'P' *)
            spmh_index : tsp00_Int2;
      END;

      tsp_mod_header = tsp00_ModHeader;

      tsp00_KernelDca = RECORD
      (* fields have no name prefix as some names   *)
      (* are shared with the type tdg_workbench_dca *)
            cancelb_ptr       : tsp00_BoolAddr;
            msgno             : tsp00_Int2;
            first_call        : boolean;
            stop_msg_set      : boolean;
            curr_user         : tsp00_KnlIdentifier;
            curr_group        : tsp00_KnlIdentifier;
            owner_sysdba      : tsp00_KnlIdentifier;
            system_user       : tsp00_KnlIdentifier;
            user_system_user  : tsp00_KnlIdentifier;
            stop_msg          : tsp00_C256;
            proc_type         : tsp00_Sproc;
            kernel_dca_filler : tsp00_C1;
            mess_type         : tsp00_Int2;
            firstcall         : boolean;
            lastcall          : boolean;
            value_p           : tsp00_Addr;
            value_size        : tsp00_Int4;
            code_p            : tsp00_Addr;
            code_size         : tsp00_Int4;
      END;

      tsp_kernel_dca = tsp00_KernelDca;

      tsp00_PlCodeInfo = RECORD
            spplc_objtype  : tsp00_Name;
            spplc_owner    : tsp00_KnlIdentifier;
            spplc_progn    : tsp00_KnlIdentifier;
            spplc_modn     : tsp00_KnlIdentifier;
            spplc_proglang : tsp00_Name;
            spplc_subtype  : tsp00_Sname;
            spplc_version  : tsp00_C4;
      END;

      tsp_pl_code_info =  tsp00_PlCodeInfo;
      (* End Dialog - Kernel *)

      tsp00_RteCommRec = RECORD
            to_cancel       : boolean;
            rte_fill1       : boolean;
            virt_reads      : tsp00_Int2;
            file_root       : tsp00_Int4;
            file_record_cnt : tsp00_Int4;
      END;

      tsp00_RteCommAddr = ^tsp00_RteCommRec;
      (**)
      tsp_rte_comm_rec  = tsp00_RteCommRec;
      tsp_rte_comm_addr = tsp00_RteCommAddr;

      tsp00_RteRelocationRec = RECORD
            rte_acv_pointer     : tsp00_ObjAddr;
            rte_tak_pointer     : tsp00_ObjAddr;
            rte_relocationvalue : tsp00_Int4;
            rte_relocated       : boolean;
            rte_rel_fill        : boolean;
            rte_rel_fill1       : tsp00_Int2;
            rte_swap1_start     : tsp00_ObjAddr;
            rte_swap1_length    : tsp00_Int4;
            rte_swap2_start     : tsp00_ObjAddr;
      END;

      tsp00_RteRelocateAddr = ^tsp00_RteRelocationRec;
      (**)
      tsp_rte_relocation_rec = tsp00_RteRelocationRec;
      tsp_rte_relocate_addr  = tsp00_RteRelocateAddr;
      (**)
      (* ******* virtual terminal ******* *)
      tsp00_VtLonglabel     = ARRAY [1..csp_sk_lines ] OF tsp00_VtLabel;
      tsp00_VtLlabels       = ARRAY [1..csp_sk_number] OF tsp00_VtLonglabel;
      tsp00_VtLocalFunction = vt_cursor_up..vt_del_eof;
      tsp00_VtSlabels       = ARRAY [1..csp_sk_number] OF tsp00_VtLabel;
      (**)
      tsp_vt_longlabel      = tsp00_VtLonglabel;
      tsp_vt_llabels        = tsp00_VtLlabels;
      tsp_vt_local_function = tsp00_VtLocalFunction;
      tsp_vt_slabels        = tsp00_VtSlabels;

      tsp00_TerminalDescription = RECORD
            content_buf   : tsp00_ScreenBufAddr;
            attribute_buf : tsp00_ScreenBufAddr;
            num_of_lines  : tsp00_Int2;
            num_of_cols   : tsp00_Int2;
            colors        : tsp00_VtColors;
            has_sysline   : boolean;
            labels        : tsp00_SkLabel;
            returnkeys    : tsp00_VtKeys;
            attributes    : tsp00_VtAttrib;
            graphic_chars : boolean;
            mark          : boolean;
            windows       : boolean;
            dbcs          : tsp00_Dbcs;
            so_char       : tsp00_C1;
            si_char       : tsp00_C1;
            filler_1      : boolean;
            filler_2      : tsp00_C2;
      END;

      tsp_terminal_description = tsp00_TerminalDescription;

      tsp00_VtField = RECORD
            text  : tsp00_VtLabel;
            lno   : tsp00_Int2;
            colnr : tsp00_Int2;
            att   : tsp00_C1;
            fill1 : boolean;
            fill2 : tsp00_Int2;
      END;

      tsp_vt_field = tsp00_VtField;

      tsp00_HifParms = RECORD
            slabels      : tsp00_VtSlabels;
            llabels      : tsp00_VtLlabels;
            sys_text     : tsp00_Line;
            sys_attr     : tsp00_Line;
            insert_label : tsp00_VtField
      END;

      tsp_hif_parms = tsp00_HifParms;

      tsp00_VtKeyStroke = RECORD
            key    : tsp00_VtKey;
            fill1  : boolean;
            fill2  : tsp00_Int2;
            CASE tsp00_VtKey OF
                vt_character_key,
                vt_control_key   :
                    (ch : char);
                vt_hscroll,
                vt_vscroll       :
                    (scroll_dir : tsp00_Int2);
                vt_mouse_down,
                vt_mouse_up,
                vt_mouse_move,
                vt_mouse_dbl     :
                    (mouse_x : tsp00_Int2;
                    mouse_y  : tsp00_Int2)
                END;
            (*ENDCASE*)

      tsp_vt_keystroke = tsp00_VtKeyStroke;

      tsp00_VtOptions = RECORD
            wait_for_input        : boolean;
            usage_mode            : tsp00_VtUsageMode;
            return_on_last        : boolean;
            return_on_first       : boolean;
            returnkeys            : tsp00_VtKeys;
            reject_keys           : tsp00_VtKeys;
            bell                  : boolean;
            mark                  : tsp00_VtMark;
            first_line_changed    : tsp00_Int2;
            last_line_changed     : tsp00_Int2;
            screen2_first_changed : tsp00_Int2;
            screen2_last_changed  : tsp00_Int2;
            overtype              : boolean;
            fill1                 : boolean;
      END;

      tsp_vt_options = tsp00_VtOptions;

      tsp00_VtRectangle = RECORD
            top    : tsp00_Int2; (* row ts_number *)
            left   : tsp00_Int2; (* column ts_number *)
            bottom : tsp00_Int2; (* row ts_number *)
            right  : tsp00_Int2; (* column ts_number *)
      END;

      tsp_vt_rectangle = tsp00_VtRectangle;
      tsp_rectangle    = tsp00_VtRectangle;

      tsp00_VtResult = RECORD
            num_of_lines   : tsp00_Int2;
            num_of_cols    : tsp00_Int2;
            mark_begin     : tsp00_Int2;
            mark_end       : tsp00_Int2;
            keystroke      : tsp00_VtKeyStroke;
            screen_changed : boolean;
            vt_res_fill1   : boolean;
            vt_res_fill2   : tsp00_Int2;
      END;

      tsp_vt_result = tsp00_VtResult;

      tsp00_VtSplitopt = RECORD
            maximized : boolean;
            icon      : boolean;
            pos       : tsp00_VtRectangle;
      END;

      tsp00_VtSplitoptaddr = ^tsp00_VtSplitopt;
      (**)
      tsp_vt_splitopt      = tsp00_VtSplitopt;
      tsp_vt_splitoptaddr  = tsp00_VtSplitoptaddr;
      (* ******* virtual file ******* *)
      tsp00_VfBuf          = ARRAY [ 1..csp_pool_buffers ] OF tsp00_Page;
      tsp00_VfBufaddr      = ^tsp00_VfBuf;
      (**)
      tsp_vf_buf           = tsp00_VfBuf;
      tsp_vf_bufaddr       = ^tsp00_VfBuf;
      (* ******* disk io ******* *)
      tsp00_Maxiobufaddr = ^tsp00_AnyPackedChar;
      (**)
      tsp_maxiobufaddr   = tsp00_Maxiobufaddr;
      (* ******* VGETSORT ******* *)

      tsp00_DecimalPresentation = PACKED RECORD
            thousand_token  : char;
            zero_point      : char;
            dec_pres_filler : tsp00_Int2
      END;

      tsp_decimal_presentation = tsp00_DecimalPresentation;

      tsp00_XpValueRec = RECORD
            xp_info          : tsp00_Int2;
            xp_fill1         : boolean ;
            xp_value_t        : tsp00_XpKey;
            CASE tsp00_XpKey OF
                xp_int2_type :
                    (xp_int2 : tsp00_Int2);
                xp_int4_type :
                    (xp_int4 : tsp00_Int4);
                xp_c8_type   :
                    (xp_c8   : tsp00_C8);
                xp_c18_type  :
                    (xp_c18  : tsp00_C18);
                xp_c24_type  :
                    (xp_c24  : tsp00_C24);
                xp_c40_type  :
                    (xp_c40  : tsp00_C40);
                xp_c64_type  :
                    (xp_c64  : tsp00_C64);
                xp_crypt_type:
                    (xp_crypt: tsp00_CryptName);
                END;
            (*ENDCASE*)
            (*------------------------------------*)
            (*      QUERY/REPORT interface        *)
            (* for Precompiler, EASY and DIALOG   *)
            (*------------------------------------*)

      tsp_xp_value_rec = tsp00_XpValueRec;

      tsp00_OnlineHeader = RECORD
            id_field    : tsp00_C8;
            relno_field : tsp00_C8;
            mode_field  : tsp00_C12;
            text_field  : tsp00_C40;
      END;

      tsp_online_header = tsp00_OnlineHeader;

      tsp00_ReportValues = RECORD
            count               : tsp00_Number;
            CASE boolean OF
                true :
                    (sum        : tsp00_Number;
                    minimum     : tsp00_Number;
                    maximum     : tsp00_Number;
                    val1        : tsp00_Number;
                    val2        : tsp00_Number;
                    val3        : tsp00_Number;
                    val4        : tsp00_Number);
                false :
                    (min_string : tsp00_C40;
                    max_string  : tsp00_C40);
                END;
            (*ENDCASE*)

      tsp_report_values = tsp00_ReportValues;

      tsp00_ReportFunctionResult = RECORD
            rf_used_count    : tsp00_Int4;
            rf_column_id     : ARRAY [ 1..16 ] OF tsp00_Int4;
            rf_column_type   : ARRAY [ 1..16 ] OF char;
            rf_column_result : ARRAY [ 1..16 ] OF tsp00_ReportValues;
      END;

      tsp_report_function_result = tsp00_ReportFunctionResult;

      tsp00_ReportInterface = RECORD
            dp_frame_header : tsp00_OnlineHeader;
            dp_e_size       : tsp00_Int2;
            dp_e_length     : tsp00_Int2;
            dp_dbname       : tsp00_DbName;
            dp_res_name     : tsp00_KnlIdentifier;
            dp_resultcount  : tsp00_Int4;
            dp_rowno        : tsp00_Int4;
            dp_colno        : tsp00_Int4;
            dp_sql_rc       : tsp00_Int2;
            dp_count        : tsp00_Int2; (* only for Precompiler *)
            dp_funct_vals   : tsp00_ReportFunctionResult;
      END;

      tsp_report_interface = tsp00_ReportInterface;

      tsp00_QrPrePacket =  RECORD
            qrp_buf     : tsp00_Buf;
            qrp_parms   : tsp00_ReportInterface;
            qrp_retcode : tsp00_Int2;
            qrp_mtype   : tsp00_Int2;
      END;

      tsp_qr_pre_packet = tsp00_QrPrePacket;
      (**)
      tsp00_LdInfo = SET OF (
            ld_ex_trigger,
            ld_with_lock,
            ld_no_close,
            ld_new_rec,
            ld_is_comment,
            ld_is_catalog,
            ld_unicode,
            ld_without_shortcol);
      tsp_ld_info = tsp00_LdInfo;
      (**)
      tsp00_LdState = SET OF (
            ld_use_termchar,
            ld_use_conversion,
            ld_use_toascii,
            ld_use_ucs_2_swap,
            ld_short_scol);

      tsp00_LongDescriptor = RECORD
            CASE boolean OF
                true :
                    (ld_just_for_compile_help : boolean);
                false :
                    (ld_descriptor          : tsp00_C8;
                    ld_tabid                : tsp00_C8;
                    ld_maxlen               : tsp00_Int4;
                    ld_intern_pos           : tsp00_Int4;
                    ld_infoset              : tsp00_LdInfo;
                    ld_state                : tsp00_LdState;
                    ld_used_in_ak           : tsp00_C1;
                    ld_valmode              : tsp00_ValMode;
                    ld_valind               : tsp00_Int2;
                    ld_unused_2             : tsp00_Int2;
                    ld_valpos               : tsp00_Int4;
                    ld_vallen               : tsp00_Int4)
                END;
            (*ENDCASE*)

      tsp_long_descriptor = tsp00_LongDescriptor;
      (**)
      tsp00_LdbChange = SET OF (
            ldb_use_termchar,
            ldb_use_conversion,
            ldb_use_toascii,
            ldb_use_ucs_2_swap);
      tsp_ldb_change = tsp00_LdbChange;

      tsp00_LongDescBlock = RECORD
            ldb_descriptor  : tsp00_C8;
            ldb_serverdb_no : tsp00_C2;
            ldb_change      : tsp00_LdbChange;
            ldb_replicated  : boolean;
            ldb_curr_pageno : tsp00_Int4;
            ldb_curr_pos    : tsp00_Int2;
            ldb_colno       : tsp00_C1;
            ldb_show_kind   : tsp00_C1;
            (* above: 20 bytes not seen by application *)
            ldb_intern_pos  : tsp00_Int4;
            (* above: 4 bytes may be seen by application *)
            ldb_infoset     : SET OF (ldb_ex_trigger,
                  ldb_with_lock, ldb_no_close,
                  ldb_new_rec, ldb_is_comment, ldb_is_catalog,
                  ldb_unicode);
            ldb_filler1     : boolean;
            ldb_filler1a    : tsp00_C1;
            (* above: 3 byte only seen by the application *)
            ldb_valmode     : tsp00_ValMode;
            ldb_valind      : tsp00_Int2;
            ldb_oldvallen   : tsp00_Int2; (* unused in newlong *)
            ldb_valpos      : tsp00_Int4;
            ldb_vallen      : tsp00_Int4;
            ldb_full_len    : tsp00_Int4;
            (* above: 17 bytes seen by the application and kernel *)
            ldb_filler2     : tsp00_C4;
            (* below: 8 bytes not seen by application *)
            ldb_last_i_pos  : tsp00_Int4;
            ldb_root_check  : tsp00_Int4;
      END;

      tsp_long_desc_block = tsp00_LongDescBlock;

      tsp00_ParamSpec = PACKED RECORD
            para_type   : char;
            para_length : char;
            CASE boolean OF
                true :
                    (para_frac : char;
                    filler     : char);
                false :
                    (para_long_length : tsp00_Int2);
                END;
            (*ENDCASE*)

      tsp_param_spec = tsp00_ParamSpec;
      tsp00_ParamSpecList = ARRAY [ 1..csp_maxvariables ] OF
            tsp00_ParamSpec;
      tsp_param_spec_list = tsp00_ParamSpecList;

      tsp_dcom_init_sink_parms = PACKED RECORD
            acv_addr  : tsp00_Addr;
            sink_addr : tsp00_Addr;
            sink_size : tsp00_Int4;
      END;


      tsp_dcom_co_create_instance_parms = PACKED RECORD
            coclsid         : tsp00_C16;
            iid             : tsp00_C16;
            ifactory        : tsp00_Addr;
            idispatch       : tsp00_Addr;
            ikernel_sink    : tsp00_Addr;
            session_context : tsp00_Addr;
            session         : tsp00_C4;
            trans           : tsp00_C4;
            ret             : tsp00_Int4;
            (*in_proc_server  : boolean;	PTS 1112663 *)
            (*debug_mode      : boolean;    PTS 1112663 *)
            (*debug_location  : tsp00_Addr; PTS 1112663 *)
            errtext         : tsp00_C64;
      END;


      tsp_dcom_dispatch_parms = PACKED RECORD
            idispatch       : tsp00_Addr;
            ifactory        : tsp00_Addr;
            session_context : tsp00_Addr;
            session         : tsp00_C4;
            param_stack     : tsp00_MoveObjPtr;
            param_stack_len : tsp00_Int4;
            dispid          : tsp00_Int4;
            hresult         : tsp00_Int4;
            trans           : tsp00_C4;
            monitor_slot_id : tsp00_Int2;
            (* debug_mode      : boolean; PTS 1112663 *)
            monitoring      : boolean;
            (* debug_location  : tsp00_Addr; PTS 1112663 *)
            method_name     : tsp00_C64;
            errtext         : tsp00_C64;
      END;


      tsp_dcom_release_instance_parms = PACKED RECORD
            session_context : tsp00_Addr; (* +++ FF PTS 1104668  +++ *)
            session         : tsp00_C4;
            trans           : tsp00_C4;
            hresult         : tsp00_Int4;
      END;


      tsp_dcom_paraminfo_parms = PACKED RECORD
            prog_id         : tsp00_C64;
            method          : tsp00_KnlIdentifier;
            coclsid         : tsp00_C16;
            iid             : tsp00_C16;
            udttypeguid     : tsp00_C16;
            packet          : tsp00_MoveObjPtr;
            typeinfo_ptr    : tsp00_Addr;
            session_context : tsp00_Addr;
            dispid          : tsp00_Int4;
            const_dispid    : tsp00_Int4;
            session         : tsp00_C4;
            trans           : tsp00_C4;
            const_type      : tsp00_Int2;
            const_typeid    : tsp00_Int2;
            func_idx        : integer;
            (*in_proc_server  : boolean;     PTS 1112663 *)
            db_obj_type     : tsp00_DbObjectType;
            (* debug_mode      : boolean;    PTS 1112663 *)
            (* debug_location  : tsp00_Addr; PTS 1112663 *)
            errtext         : tsp00_C64;
      END;

      (* +++ FF Change Request 1000139  +++ *)

      tsp_dcom_monitor_parms = PACKED RECORD
            session_handle   : tsp00_Addr;
            retcode          : tsp00_Int4;
            idispatch        : tsp00_Addr;
            iid              : tsp00_C16;
            session          : tsp00_C4;
      END;

      (* --- FF Change Request 1000139  --- *)


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
            sp1m_sfill2,
            sp1m_sfill3,
            sp1m_sfill4,
            sp1m_sfill5,
            sp1m_specials_upper_bound,
            sp1m_wait_for_event);
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
            sp1iw_warn0,
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
            sp1co_not_used1,
            sp1co_not_used2,
            sp1co_not_used3,
            sp1co_not_used4,
            sp1co_not_used5,
            sp1co_not_used6,
            sp1co_not_used7);
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
            sp1pk_checkpoint_info);
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
            sp1pr_installation);
      (*                sp1pr_internal_cmd given for example by LOAD, *)
      (*                if command was not given by the user but      *)
      (*                needed by LOAD for internal use               *)
      (* *)
      tsp1_segment_kind = (
            sp1sk_nil,
            sp1sk_cmd,
            sp1sk_return);
      (* *)
      tsp1_sqlmode = (
            sp1sm_nil,
            sp1sm_session_sqlmode,
            sp1sm_adabas,
            sp1sm_ansi,
            sp1sm_db2,
            sp1sm_oracle);

      tsp1_part_header = RECORD
            sp1p_part_kind   : tsp1_part_kind;
            sp1p_attributes  : tsp1_part_attr;
            sp1p_arg_count   : tsp_int2;
            sp1p_segm_offset : tsp_int4;
            (* *)
            sp1p_buf_len     : tsp_int4;
            sp1p_buf_size    : tsp_int4
      END;


      tsp1_part = RECORD
            CASE integer OF
                1:
                    (sp1p_part_header : tsp1_part_header;
                    sp1p_buf          : tsp_moveobj);
                2:
                    (sp1p_part_kind   : tsp1_part_kind;
                    sp1p_attributes   : tsp1_part_attr;
                    sp1p_arg_count    : tsp_int2;
                    sp1p_segm_offset  : tsp_int4;
                    sp1p_buf_len      : tsp_int4;
                    sp1p_buf_size     : tsp_int4;
                    sp1p_resnum       : tsp_resnum);
                END;
            (*ENDCASE*)

      tsp1_part_ptr = ^tsp1_part;

      tsp1_segment_header = RECORD
            CASE integer OF
                1:
                    (sp1s_segm_len          : tsp_int4;
                    sp1s_segm_offset        : tsp_int4;
                    (*  *)
                    sp1s_no_of_parts        : tsp_int2;
                    sp1s_own_index          : tsp_int2;
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
                    sp1c_filler2            : tsp_c8;
                    sp1c_filler3            : tsp_c8);
                2:
                    (sp1s_space2            : tsp_c8;
                    (*  *)
                    sp1s_space3             : tsp_c4;
                    sp1s_space4             : tsp1_segment_kind;
                    sp1r_sqlstate           : tsp_sqlstate; (* 5 bytes*)
                    sp1r_returncode         : tsp_int2;
                    sp1r_errorpos           : tsp_int4;
                    (*  *)
                    sp1r_extern_warning     : tsp_warningset;
                    sp1r_intern_warning     : tsp1_warningset_intern;
                    sp1r_function_code      : tsp_int2; (* csp1_.._fc *)
                    sp1r_tracelevel         : tsp_int1;
                    sp1r_filler1            : boolean;
                    sp1r_filler2            : tsp_c8)
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
                    sp1p_buf                : tsp_moveobj);
                3:
                    (sp1s_segm_len          : tsp_int4;
                    sp1s_segm_offset        : tsp_int4;
                    sp1s_no_of_parts        : tsp_int2;
                    sp1s_own_index          : tsp_int2;
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
                    sp1c_filler2            : tsp_c8;
                    sp1c_filler3            : tsp_c8);
                4:
                    (sp1s_space2            : tsp_c8;
                    sp1s_space3             : tsp_c4;
                    sp1s_space4             : tsp1_segment_kind;
                    (* --- return segment header --- *)
                    sp1r_sqlstate           : tsp_sqlstate; (* 5 bytes*)
                    sp1r_returncode         : tsp_int2;
                    sp1r_errorpos           : tsp_int4;
                    sp1r_extern_warning     : tsp_warningset;
                    sp1r_intern_warning     : tsp1_warningset_intern;
                    sp1r_function_code      : tsp_int2; (* csp1_.._fc *)
                    sp1r_tracelevel         : tsp_int1;
                    sp1r_filler1            : boolean;
                    sp1r_filler2            : tsp_c8;
                    (* --- first part --- *)
                    sp1p_part_kind          : tsp1_part_kind;(* |     *)
                    sp1p_attributes         : tsp1_part_attr;(* |first*)
                    sp1p_arg_count          : tsp_int2;      (* |part *)
                    sp1p_segm_offset        : tsp_int4;      (* |head *)
                    sp1p_buf_len            : tsp_int4;      (* |     *)
                    sp1p_buf_size           : tsp_int4;      (* |     *)
                    sp1p_resnum             : tsp_resnum)
                END;
            (*ENDCASE*)

      tsp1_segment_ptr = ^tsp1_segment;
      (* *)

      tsp1_packet_header = RECORD
            sp1h_mess_code    : tsp_code_type; (* code of old packet *)
            sp1h_mess_swap    : tsp_swap_kind; (* swap of old packet *)
            sp1h_filler1      : tsp_int2;
            sp1h_appl_version : tsp_c5;        (* | senderid of      *)
            sp1h_application  : tsp_c3;        (* |    old packet    *)
            sp1h_varpart_size : tsp_int4;
            (*  *)
            sp1h_varpart_len  : tsp_int4;
            sp1h_filler2      : tsp_int2;
            sp1h_no_of_segm   : tsp_int2;
            sp1h_filler3      : tsp_c8
      END;


      tsp1_packet = RECORD
            sp1_header           : tsp1_packet_header;
            CASE integer OF
                1:
                    (sp1_varpart : tsp_moveobj);
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
            sp1i_data_type  : tsp_data_type;
            sp1i_frac       : tsp_int1;
            sp1i_length     : tsp_int2;
            sp1i_in_out_len : tsp_int2;
            sp1i_bufpos     : tsp_int4;
      END;

      (* die beiden naechsten Typen ersetzen die vsp00-Typen *)

      tsp1_dcom_param_info = PACKED RECORD
            sp1i_dcom_mode           : tsp1_param_opt_set;
            sp1i_dcom_io_type        : tsp1_param_io_type;
            sp1i_dcom_datatype       : tsp_int2;
            sp1i_dcom_sub_datatype   : tsp_int2;
            sp1i_dcom_length         : tsp_int2;
            sp1i_dcom_dim            : tsp_int2;
            sp1i_dcom_frac           : tsp_int1;
            sp1i_dcom_filler         : boolean;
            sp1i_dcom_cpp_offset     : tsp_int4;
            sp1i_dcom_ascii_offset   : tsp_int4;
            sp1i_dcom_unicode_offset : tsp_int4;
            sp1i_dcom_type_uid       : tsp_c16;
      END;


      tsp1_param_spec = PACKED RECORD
            para_type   : tsp_data_type;
            para_frac   : char;
            para_length : tsp_int2;
      END;

      tsp1_param_spec_list = ARRAY [ 1..csp_maxvariables ] OF
            tsp1_param_spec;


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
      (* *)
      tgg00_SubtransNo  = tsp00_Uint4;
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
            bd_fill3,
            bd_fill4,
            bd_fill5,
            bd_fill6,
            bd_fill7);
      (* *)
      tgg00_CacheType = (
            cachCatalog_egg00,
            cachConv_egg00,
            cachData_egg00,
            cachServerMbody_egg00,
            cachServerMdesc_egg00,
            cachFbm_egg00,
            cachLogQueue_egg00,
            cachProcCode_egg00,
            cachProcData_egg00,
            cachTemp_egg00,
            cachUsm_egg00,
            cachSequence_egg00);
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
            diagColId_egg00,
            diagColRoot_egg00,
            diagConfig_egg00,
            diagDev_egg00,
            diagInvId_egg00,
            diagInvName_egg00,
            diagInvTree_egg00,
            diagLoginfoPage_egg00,
            diagLogFrom_egg00,
            diagLogLast_egg00,
            diagMultInvId_egg00,
            diagMultInvName_egg00,
            diagPages_egg00,
            diagEnableDevspace_egg00,
            diagConvPage_egg00,
            diagNotUsed_egg00,
            diagRestart_egg00,
            diagRestore_egg00,
            diagTabId_egg00,
            diagTabName_egg00,
            diagTabIdGet_egg00,
            diagTabNameGet_egg00,
            diagFile_egg00,
            diagUserIdGet_egg00,
            diagUserNameGet_egg00,
            diagWrite_egg00,
            diagLogDev_egg00,
            diagMirrLogDev_egg00,
            diagSysDev_egg00,
            diagMirrSysDev_egg00);
      (* *)
      (* PTS 1003033 E.Z. *)
      tgg00_DirectoryMonitorOp = (
            dir_hit,
            dir_miss,
            func_count);
      (* *)
      tgg00_Distinct = (
            no_distinct,
            full_distinct,
            distinct_without_warning_byte);
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
            dumpObjFirDir_egg00);
      (* *)
      tgg00_ErrorText = (
            errtext_char,
            errtext_stack,
            errtext_surrogate);
      (* *)
      tgg00_FiletypeSet = SET OF (
            ftsTemp_egg00,
            ftsPerm_egg00,
            ftsByteStr_egg00,
            ftsConcurrent_egg00, (* parallel temp/perm access [locklist] *)
            ftsShared_egg00,     (* temp access for different tasks [cache]*)
            ftsDynamic_egg00,
            ftsObject_egg00,
            ftsFill7_egg00);
      (* *)
      tgg00_FsUseMode = (
            fs_read,
            fs_write,
            fs_readwrite);
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
            hsIgnoreReadOnly_egg00,
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
            unique_pk_table);
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
            lrsTemp_egg00);
      (* *)
      tgg00_LogMode = (
            logModeNil_egg00,
            logModeDual_egg00,
            logModeSingle_egg00,
            logModeDemo_egg00);
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
            mbs_copy_long);
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
            m_continued_log,
            m_copy_tempfile,
            m_create_file,
            m_create_index,
            m_create_serverdb,
            m_create_table,
            m_delete,
            m_delete_trans,
            m_describe,
            m_destroy_temp,
            m_diagnose,
            m_down,
            m_drop,
            m_drop_serverdb,
            m_end,
            m_eval,
            m_fetch,
            m_flush,
            m_get,
            m_usage,
            m_index_statistic,
            m_init_config,
            m_inquire,
            m_insert,
            m_insert_select,
            m_insert_trans,
            m_integrate_node,
            m_key,
            m_lock,
            m_mend,
            m_mfetch,
            m_move,
            m_notify,
            m_logpos_list,
            m_outcopy,
            m_outcopy_catalog,
            m_partial_rollback,
            m_notusedanymore,   (*unused *) (* PTS 1107952 E.Z. *)
            m_release,
            m_reload,
            m_restart,
            m_restore,
            m_return_error,
            m_return_result,
            m_rollback,
            m_save,
            m_save_overwrite,
            m_savepoint,
            m_select,
            m_select_row,
            m_set,
            m_show,
            m_shutdown,
            m_shutdown_filesystem,
            m_signal_commit,
            m_stamp,
            m_succ_file_version,
            m_table_statistic,
            m_union,
            m_unload,
            m_unlock,
            m_unused_log,
            m_up,
            m_upd_statistic,
            m_update,
            m_update_rec,
            m_verify,
            m_majority,
            m_format,
            m_repli_savepoint,
            m_nextval,
            m_history,
            m_drda,
            m_procedure,
            m_new,
            m_create_obj_hash,
            m_get_object,
            m_filler,                        (* PTS 1111576 E.Z. *)
            m_save_parallel,
            m_restore_parallel,
            m_autosave,
            m_open_trans,
            m_create_index_parallel,
            m_migrate_obj_files);                    (* PTS 471100 FF *)
      (* *)
      tgg00_MessType2 = (
            mm_nil,
            mm_clear,
            mm_close,
            mm_consistent,
            mm_compare_record,
            mm_config,
            mm_coordinator,
            mm_copy,
            mm_copy_log,
            mm_database,
            mm_database_clear_log,
            mm_database_noformat,
            mm_dbname,
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
            mm_fnull,
            mm_fread,
            mm_running_cmds,
            mm_index,
            mm_join_with_last_tab,
            mm_keep_lock,
            mm_key,
            mm_last,
            mm_length,
            mm_lock,
            mm_fwrite,
            mm_lock_timeout,
            mm_log,
            mm_log_cold,
            mm_log_demo,
            mm_object_id,
            mm_next,
            mm_nokey,
            mm_ok_not_allowed,
            mm_open,
            mm_optimize,
            mm_outcopy,
            mm_pages,
            mm_pages_clear_log,
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
            mm_write_reject,
            mm_trigger,
            mm_ignore_duplicates,
            mm_update_duplicates,
            mm_fetch_request,
            mm_allocate,
            mm_bind_req,
            mm_exc_req,
            mm_deallocate,
            mm_reply_cont,
            mm_reply_end,
            mm_first_distributed,
            mm_next_reply,
            mm_acknowledge,
            mm_newtape,
            mm_ignore,
            mm_abort,
            mm_object,
            mm_qual_no_opt,
            mm_new_write,
            mm_new_read,
            mm_parallel,
            mm_checkpoint,
            mm_optimize_longvar);   (* PTS 1106798 JA 2000-05-30 *)
      (* *)
      tgg00_MiscSet = SET OF (
            mi_upd_cols,
            mi_dt_iso,
            mi_dt_usa,
            mi_dt_eur,
            mi_dt_jis,
            mi_dt_oracle_date);
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
            pmFill3_egg00,
            pmFill4_egg00,
            pmFill5_egg00,
            pmFill6_egg00,
            pmFill7_egg00);
      (* *)
      tgg00_PageType = (
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
            ptLastPageType_egg00); (* see Kernel_Page.hpp *)
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
      tgg00_PrivR = (
            r_sel,
            r_upd,
            r_ins,
            r_del,
            r_owner,
            r_alter,
            r_index,
            r_link);
      (* *)
      tgg00_QualKind = (
            primary_only,
            inv_only,
            inv_and_primary);
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
            iprim_key_read,
            iprim_key_qual,
            iprim_key_range,
            iprim_read_key_range,
            iprim_qual_key_range,
            iindex,
            iindex_read,
            iindex_qual,
            iindex_range,
            iindex_range_read,
            iindex_range_qual,
            iis_index,
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
            rgnBufwrtr_egg00,
            rgnBuf2wrtr_egg00,
            rgnConfig_egg00,
            rgnConvert_egg00,
            rgnDatacache_egg00,
            rgnDiagcache_egg00,
            rgnDrda_egg00,
            rgnFbm_egg00,
            rgnIntoQueue_egg00,
            rgnKeymem_egg00,
            rgnLock_egg00,
            rgnLog_egg00,
            rgnLogwriter_egg00,
            rgnNet_egg00,
            rgnNetdown_egg00,
            rgnNetsend_egg00,
            rgnOutQueue_egg00,
            rgnPermfdir_egg00,
            rgnReqPno_egg00,
            rgnSplitt1_egg00,
            rgnSproc_egg00,
            rgnSurrogate_egg00,
            rgnTempfdir_egg00,
            rgnTrace_egg00,
            rgnTree_egg00,
            rgnData_egg00,
            rgnRow_egg00,
            rgnSave_egg00,
            rgnTab_egg00,
            rgnTrans_egg00,
            rgnOMS_egg00);
      (* *)
      tgg00_Replicated = (
            rpl_none,     (* rplNone_egg00     *)
            rpl_full,     (*rplFull_egg00      *)
            rpl_part);    (*rplPart_egg00      *)
      (* *)
      tgg00_ServerdbType = (
            sdb_adabas_c_gateway, (*  sqlm_ansi     *)
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
            sh_component_classes,
            sh_dbm_state,                 (* was sh_packages *)
            sh_explain,
            sh_monitor_oms,
            sh_index,
            sh_link,
            sh_lock,
            sh_mapset,
            sh_p_domain,
            sh_p_prog,
            sh_param_dbproc,
            sh_param_trigger,
            sh_priv_granted_to,
            sh_priv_on,
            sh_sequence,
            show_fill6,                   (* unused, was sh_priv_user *)
            sh_procedure,
            show_fill7,                   (* unused, was sh_procedurelist *)
            show_fill8,                   (* unused, was sh_procparam *)
            sh_ref_column,
            sh_serverdb,
            sh_st_column,
            sh_st_config,
            sh_st_database,
            sh_st_devspace,
            sh_st_index,
            sh_st_log,
            show_fill9,                   (* unused, was sh_st_mapset *)
            sh_st_monitor,
            sh_st_state,
            sh_st_table,
            show_fill10,                  (* unused, was sh_st_termset *)
            sh_st_user,
            sh_synonym,
            show_fill11,                  (* unused, was sh_sysspace *)
            show_fill12,                  (* unused, was sh_sysdba *)
            sh_table,
            show_fill13,                  (* unused, was sh_tabledef *)
            show_fill14,                  (* unused, was sh_tablelist *)
            sh_user_current,
            sh_table_stat,
            sh_termset,
            sh_transfile,
            sh_trigger,
            show_fill15,                  (* unused, was sh_trigger_trace *)
            sh_u_domain,
            sh_user,
            show_fill16,                  (* unused, was sh_values *)
            sh_version,
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
            sh_info_gc_stats,             (* unused PTS 1111576 E.Z. reused: PTS 1113774 F.F. *)
            sh_sysmon_connection,         (* unused PTS 1111576 E.Z. reused: PTS 1114549 E.Z. *)
            sh_dbfunctions,
            sh_view_uses_table,
            sh_trans,
            sh_all_view_uses_table,
            sh_new_privileges,
            sh_all_new_privileges,
            sh_domain_constraints,
            sh_dbfunc_parameters,
            sh_xparam,
            sh_st_lock,
            sh_filler3,                   (* unused PTS 1111576 E.Z. *)
            sh_odbc_st_index,
            sh_odbc_st_table,
            show_fill3,                   (* unused, was sh_db_link *)
            sh_fill3a,                    (* unused, was sh_multibyte *)
            show_fill4,                   (* unused, was sh_gatewayview *)
            sh_object,
            sh_fill3b,                    (* unused, was sh_index_obj *)
            sh_all_object,
            show_fill2,                   (* unused, was sh_autosave *)
            sh_pages,
            sh_roots,
            sh_session_roles,
            sh_roles,
            sh_all_roles,
            sh_odbc_columns,
            sh_show_view,
            sh_methods,
            sh_method_params,
            sh_oms_versions,
            sh_ctrl_configuration,
            sh_ctrl_devspaces,
            sh_ctrl_io_statistics,
            sh_ctrl_log,
            sh_ctrl_cache,
            sh_oms_containers,
            sh_ctrl_locks,
            sh_indexpages,
            sh_treelocks,
            sh_files,
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
            sh_allocatorstatistic, (* PTS 1108435 E.Z. *)
            sh_spinlock,        (* PTS 1109404 E.Z. *)
            sh_oms_lock,        (* PTS 1110315 *)
            sh_heap_content,
            sh_odbc_index,      (* PTS 1114119 T.A. *)
            sh_container_roots,  (* PTS 1115641 F.F *)
            sh_container_chains, (* PTS 1115641 F.F *)
            sh_container_keys,   (* PTS 1115641 F.F *)
            sh_fill3);
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
            st_filler0,
            st_object_key, (* ADIS 1001810, T.A. 15.12.1998 *)
            st_filler1,
            st_filler2,
            st_filler3,
            st_filler4,
            st_get_subquery,
            st_bool,
            st_value,
            st_parseid,
            st_stop,
            st_object_col,
            st_op,
            st_outp_cp_join,
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
            st_short_scol,
            st_long_scol,
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
            st_utcdate);
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
            op_b_float);
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
            op_o_output_outer_join);
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
            op__filler1,
            op__filler2,
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
            op_filler7,
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
            op_filler17,
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
            tadabas_c);
      (* *)
      tgg00_Tfn = (
            tfnSys_egg00,
            tfnColumn_egg00,
            tfnAux_egg00,
            tfnInv_egg00,
            tfn__FurtherUse04_egg00,
            tfn__FurtherUse05_egg00,
            tfn__FurtherUse06_egg00,
            tfnMulti_egg00,
            tfn__FurtherUse08_egg00,
            tfn__FurtherUse09_egg00,
            tfnInvSubtree_egg00,
            tfnTempInv_egg00,
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
            tfnTempLog_egg00);
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
            ttfnFiller22_egg00,
            ttfnFiller23_egg00,
            ttfnOuterJoinKeys_egg00,
            ttfnOuterJoin_egg00,
            ttfnUnionIntersect_egg00,
            ttfnTrigger_egg00,
            ttfnFiller28_egg00,
            ttfnPendingLink_egg00,
            ttfnLateUniqueCheck_egg00,
            ttfnCacheRollback_egg00,
            ttfnLogPosList_egg00,
            ttfnFiller33_egg00,
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
            tsSaveptWanted_egg00,
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
      tgg00_UseMode = (
            umUser_egg00,
            umUtility_egg00,
            umStandby_egg00);
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
            ak_returncode);
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
      tgg00_LabelData     = PACKED ARRAY [1..XCONTROL_MXGG00    ] OF char;
      tgg00_LockRowId     = PACKED ARRAY [1..LOCK_ROWID_MXGG00  ] OF char;
      tgg00_LogBody       = PACKED ARRAY [1..LOGPAGE_BODY_MXGG00] OF char;
      tgg00_MaxstackArr   = ARRAY [1..MAX_STACK_GG00] OF tsp00_Int4;
      tgg00_MediaName     = tsp00_C64;
      tgg00_ObjFileNo     = tsp00_Uint4;
      tgg00_ObjFrameVers  = tsp00_Uint1;
      tgg00_ObjPagePos    = tsp00_Uint2;
      tgg00_PageHeadStr   = PACKED ARRAY [1..PAGE_HEADER_MXGG00] OF char;
      tgg00_PagePos       = tsp00_Int4;
      tgg00_PosInt        = 0 .. MAX_INT2_SP00;
      tgg00_PrivilegeSet  = SET OF tgg00_PrivR;
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
            tdcAuxBuffers_gg00     : tgg00_AuxBuffersPtr;
            tdc_monitor_info       : tgg00_UserMonitorInfoPtr;
            tdc_fdir_root          : tsp00_PageNo;
            tdc_cache_size         : tsp00_Int4;
            tdc_cache_active       : boolean;
            tdc_fill1              : boolean;
            tdc_fill2              : tsp00_Int2;
            tdcAuxBufferCount_gg00 : tsp00_Int4;
      END;

      tgg00_TempDataCachePtr = ^tgg00_TempDataCache;

      tgg00_DataCacheStatistic = RECORD
            dcsDataPagesTotal_gg00      : tsp00_Int4;
            dcsDataPagesChanged_gg00    : tsp00_Int4;
            dcsObjDataPagesTotal_gg00   : tsp00_Int4;
            dcsObjDataPagesChanged_gg00 : tsp00_Int4;
            dcsLogPagesTotal_gg00       : tsp00_Int4;
            dcsLogPagesChanged_gg00     : tsp00_Int4;
            dcsObjLogPagesTotal_gg00    : tsp00_Int4;
            dcsObjLogPagesChanged_gg00  : tsp00_Int4
      END;


      tgg00_PnoList = RECORD
            plTop_gg00 : tsp00_Int4;
            plPno_gg00 : ARRAY [LOWER_PNO_BOUND_GG00..UPPER_PNO_BOUND_GG00] OF
                  tsp00_PageNo
      END;


      tgg00_SaveRestoreInputParam = RECORD
            sripBlocksize_gg00       : tsp00_Int2;
            sripHostTapeNum_gg00     : tsp00_Int2;
            sripFileVersion_gg00     : tsp00_Int4;
            (**)
            sripIsAutoLoad_gg00      : boolean;
            sripWithCheckpoint_gg00  : boolean;
            sripIsRestoreConfig_gg00 : boolean;
            sripFiller1_gg00         : boolean;
            (**)
            sripHostTapenames_gg00   : tgg00_TapeNames;
            (**)
            sripHostFiletypes_gg00   : tgg00_Filetypes;
            (**)
            sripHostTapecount_gg00   : tgg00_TapeCount;
            (**)
            sripMedianame_gg00       : tgg00_MediaName;
            (**)
            sripUntilDate_gg00       : tsp00_Date;
            (**)
            sripUntilTime_gg00       : tsp00_Time;
            (**)
            sripUtilCmdId_gg00       : tgg00_UtilCmdId;
            (**)
            sripConfigDbName_gg00    : tsp00_DbName;
            sripFiller2_gg00         : tsp00_Int2
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


      tgg00_Sys2catHeader = RECORD
            syh_reclen        : tsp00_Int2;
            syh_keylen        : tsp00_Int2;
            syh_varcol_offset : tsp00_Int2;
            syh_varcol_cnt    : tsp00_Int2;
            syh_surrogate     : tgg00_Surrogate;
            syh_entrytype     : tsp00_C2;
            syh_linkage       : tsp00_C2;
            syh_name          : tsp00_Name;
            syh_segmentid     : tsp00_C2
      END;


      tgg00_TermDesc = RECORD
            td_internal : tsp00_C1;
            td_external : tsp00_C1;
            td_comment  : tsp00_C8;
            td_filler   : tsp00_C2;
      END;


      tgg00_TermsetRecord = RECORD
            term_reclen        : tsp00_Int2;
            term_keylen        : tsp00_Int2;
            term_varcol_offset : tsp00_Int2;
            term_varcol_cnt    : tsp00_Int2;
            term_surrogate     : tgg00_Surrogate;
            term_entrytype     : tsp00_C2;
            term_linkage       : tsp00_C2;
            term_name          : tsp00_KnlIdentifier;
            term_segmentid     : tsp00_C2;
            term_code          : tsp00_CodeType; (* ascii, ebcdic *)
            term_valid         : boolean;
            term_filler1       : tsp00_Int2;
            term_count         : tsp00_Int2;
            term_desc          : ARRAY [1..cgg_termset_entries] OF tgg00_TermDesc;
      END;


      tgg00_TimeoutGlobals = RECORD
            timeCmd_gg00 : tsp00_Int4;
            timeReq_gg00 : tsp00_Int4;
      END;


      tgg00_CodeTables = RECORD
            tables      : ARRAY [1..MAX_CODE_TABLES_GG00 ] OF tsp00_Ctable;
            termset     : ARRAY [cgg_begin_termsets..cgg_end_termsets]
                  OF tsp00_Ctable;
            inv_termset : ARRAY [cgg_begin_termsets..cgg_end_termsets]
                  OF tsp00_Ctable
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

      tgg00_SwapInfo = RECORD
            swapMessType_gg00      : tgg00_MessType;
            swapMessType2_gg00     : tgg00_MessType2;
            swapCatalogAccess_gg00 : boolean;
            swapReceive_gg00       : boolean;
            swapSource_gg00        : tsp00_SwapKind;
            swapDest_gg00          : tsp00_SwapKind;
            swapSourceCode_gg00    : tsp00_CodeType;
            swapDestCode_gg00      : tsp00_CodeType
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

      tgg00_TransChild = RECORD
            tcdSessionId_gg00         : tgg91_SessionNo;
            tcdTransId_gg00           : tgg91_TransNo;
            tcdWriteTransId_gg00      : tgg91_TransNo;      (* PTS 1108234 JA 2000-11-17 *)
            (* *)
            tcdSubtransId_gg00        : tgg00_SubtransNo;   (* PTS 1109485 E.Z. *)
            tcdChild_gg00             : tsp00_Int4;
            tcdConsistView_gg00       : tgg91_TransNo;
            tcdFiller1_gg00           : tsp00_Uint2;        (* PTS 1110237 E.Z. *)
            (* *)
            tcdState_gg00             : tgg00_TransState;
            tcdError_gg00             : tgg00_BasisError;
            tcdWarning_gg00           : tsp00_WarningSet;
            tcdObjReqTimeout_gg00     : tsp00_Uint2;
            (* *)
            tcdSessionAuxCounter_gg00 : tsp00_Int4;         (* h.b. PTS 1107826 *)
            tcdTempCount_gg00         : tsp00_Int4;
            (* *)
            tcdIoCount_gg00           : tsp00_8ByteCounter; (* PTS 1103743 JA 1999-08-25 *)
      END;

      tgg00_TransChildPtr = ^tgg00_TransChild;

      tgg00_TransContext = RECORD
            trSessionId_gg00         : tgg91_SessionNo;    (* |          *)
            trTransId_gg00           : tgg91_TransNo;      (* |          *)
            trWriteTransId_gg00      : tgg91_TransNo;      (* |          * PTS 1108234 JA 2000-11-17 *)
            (*                                             (* |          *)
            trSubtransId_gg00        : tgg00_SubtransNo;   (* |          * PTS 1109485 E.Z. *)
            trChild_gg00             : tsp00_Int4;         (* |          *)
            trConsistView_gg00       : tgg91_TransNo;      (* |          *)
            trFiller1_gg00           : tsp00_Uint2;        (* PTS 1110237 E.Z. *)
            (*                                             (* |          *)
            trState_gg00             : tgg00_TransState;   (* |          *)
            trError_gg00             : tgg00_BasisError;   (* |          *)
            trWarning_gg00           : tsp00_WarningSet;   (* | child    *)
            trObjReqTimeout_gg00     : tsp00_Uint2;        (* | data     *)
            (*                                             (* |          *)
            trSessionAuxCounter_gg00 : tsp00_Int4;         (* |          * h.b. PTS 1107826 *)
            trTempCount_gg00         : tsp00_Int4;         (* |          *)
            (*                                             (* |          *)
            trIoCount_gg00           : tsp00_8ByteCounter; (* |          * PTS 1103743 JA 1999-08-25 *)
            (* *)
            trTaskId_gg00            : tsp00_TaskId;
            trIndex_gg00             : tgg00_TransIndex;
            trBdExclFileLocks_gg00   : tsp00_Int2;
            (* *)
            trRowLocksPerSqlCmd_gg00 : tsp00_8ByteCounter;    (* PTS 1103743 JA 1999-08-25 *)
            (* *)
            trRteCommPtr_gg00        : tsp00_RteCommAddr;
            trBdTcachePtr_gg00       : tgg00_TempDataCachePtr;
            trAllocator_gg00         : tgg00_VoidPtr;         (* PTS 1108234 JA 2000-11-08 *)
            trLogContext_gg00        : tgg00_VoidPtr; (* Logging74 UH *)
            trSeqInfoPtr_gg00        : tgg00_CurrSeqInfoPtr;  (* PTS 1110095 E.Z. *)
      END;

      tgg00_TransContextPtr = ^tgg00_TransContext;

      tgg00_UnivTrans = RECORD
            CASE integer OF
                1:
                    (utrTrans : tgg00_TransContext);
                2:
                    (utrChild : tgg00_TransChild)
                END;
            (*ENDCASE*)

      tgg00_UnivTransPtr = ^tgg00_UnivTrans;

      tgg00_ObjTransInfo = RECORD
            otrBeforeRef_gg00      : tgg91_PageRef;
            otrLockTransIndex_gg00 : tgg00_TransIndex;
            otrLockTransId_gg00    : tgg91_TransNo;
            otrUpdTransId_gg00     : tgg91_TransNo
      END;


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
                END;
            (*ENDCASE*)

      tgg00_FileIdPtr = ^tgg00_FileId;
      tgg00_ContainerId = tgg00_FileId;

      tgg00_KeyRangeRecord = RECORD
            CASE boolean OF
                true :
                    (prim_start_excl       : boolean;
                    prim_start_incl        : boolean;
                    prim_start_full        : boolean;
                    prim_start_zero        : boolean;
                    prim_first             : boolean;
                    prim_start_prefix      : boolean;
                    sec_start_prefix       : boolean;
                    use_prim_start_for_pos : boolean;
                    prim_stop_excl         : boolean;
                    prim_stop_incl         : boolean;
                    prim_stop_prefix       : boolean;
                    sec_start_excl         : boolean;
                    sec_start_full         : boolean;
                    sec_start_incl         : boolean;
                    sec_start_zero         : boolean;
                    sec_stop_excl          : boolean;
                    sec_stop_incl          : boolean;
                    sec_stop_prefix        : boolean);
                false :
                    (key_ranges : ARRAY [1..cgg_range_entries] OF boolean);
                END;
            (*ENDCASE*)


      tgg00_Lock = RECORD
            lockTabId_gg00  : tgg00_Surrogate;
            lockSite_gg00   : tgg00_ServerdbNo;
            lockMode_gg00   : tgg00_LockReqMode;
            lockState_gg00  : tgg00_LockReqState;
            lockKeyLen_gg00 : tsp00_Int2;
            lockKeyPos_gg00 : tsp00_Int2;
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

      tgg00_KeyRange = RECORD
            keystart : tgg00_Lkey;
            keystopp : tgg00_Lkey
      END;


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


      tgg00_TfileAdmin = RECORD
            tfile_type : tgg00_TfnTemp;
            tfile_id   : tgg00_FileId;
            tfile_key  : tgg00_Lkey;
            filler_x   : boolean;
            pno_count  : tsp00_Int2;
            filler_y   : tsp00_Int4;
            get_pos    : tsp00_Int4;
            rec_count  : tsp00_Int4;
      END;


      tgg00_OmsVersionContext = RECORD
            ovc_tree          : tgg00_FileId;
            ovc_index_tree    : tgg00_FileId;
            ovc_key           : tsp00_Int4;
            ovc_trans_version : tgg91_TransNo;
      END;


      tgg00_OmsLockInfo = RECORD (* PTS 1110315 *)
            oli_handle      : tsp00_C8;
            oli_lockmode    : tsp00_C12;
            oli_requestmode : tsp00_C12;
            oli_taskid      : tsp00_TaskId;
            oli_timeout     : tsp00_Int4;
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
                13:
                    (termsets          : tgg00_TermsetRecord);
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


      tgg00_ColumnDistinctValues =  ARRAY[1..MAX_COL_PER_TAB_GG00] OF tsp00_Int4;

      tgg00_TreeStatInfo = RECORD
            (* in *)
            TreeStatTreeId_gg00           : tgg00_FileId;
            TreeStatCalculate_gg00        : boolean;
            TreeStatFill1_gg00            : boolean;
            TreeStatFill2_gg00            : tsp00_Int2;
            TreeStatNoSampleLeaves_gg00   : tsp00_Int4;
            (* out *)
            TreeStatNumberOfLeaves_gg00   : tsp00_Int4;
            TreeStatNumberOfRecords_gg00  : tsp00_Int4;
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
            TabStatRootLevel_gg00           : tsp00_Int2;
            TabStatAveKeyLength_gg00        : tsp00_Int2;
            TabStatMinKeyLength_gg00        : tsp00_Int2;
            TabStatMaxKeyLength_gg00        : tsp00_Int2;
            TabStatAveSepLength_gg00        : tsp00_Int2;
            TabStatMinSepLength_gg00        : tsp00_Int2;
            TabStatMaxSepLength_gg00        : tsp00_Int2;
      END;

      tgg00_TableStatInfoPtr = ^tgg00_TableStatInfo;

      tgg00_IndexStatInfo = RECORD
            IndexStatNodes_gg00             : tsp00_Int4;
            IndexStatIndexNodes_gg00        : tsp00_Int4;
            IndexStatLeafNodes_gg00         : tsp00_Int4;
            IndexStatTreeCov_gg00           : tsp00_Int4;
            IndexStatRootCov_gg00           : tsp00_Int4;
            IndexStatIndexCov_gg00          : tsp00_Int4;
            IndexStatMinIndexCov_gg00       : tsp00_Int4;
            IndexStatMaxIndexCov_gg00       : tsp00_Int4;
            IndexStatLeafCov_gg00           : tsp00_Int4;
            IndexStatMinLeafCov_gg00        : tsp00_Int4;
            IndexStatMaxLeafCov_gg00        : tsp00_Int4;
            IndexStatSecKeyCnt_gg00         : tsp00_Int4;
            IndexStatPrimKeyCnt_gg00        : tsp00_Int4;
            IndexStatAvgPrimPerList_gg00    : tsp00_Int4;
            IndexStatMinPrimPerList_gg00    : tsp00_Int4;
            IndexStatMaxPrimPerList_gg00    : tsp00_Int4;
            IndexStatRootLevel_gg00         : tsp00_Int2;
            IndexStatAvgSecKeyLen_gg00      : tsp00_Int2;
            IndexStatMinSecKeyLen_gg00      : tsp00_Int2;
            IndexStatMaxSecKeyLen_gg00      : tsp00_Int2;
            IndexStatAveSepLength_gg00      : tsp00_Int2;
            IndexStatMinSepLength_gg00      : tsp00_Int2;
            IndexStatMaxSepLength_gg00      : tsp00_Int2;
      END;

      tgg00_IndexStatInfoPtr = ^tgg00_IndexStatInfo;

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
            dist_values         : ARRAY[1..MAX_COL_PER_TAB_GG00] OF tsp00_Int4;
      END;

      tgg00_SampleInfoPtr = ^tgg00_SampleInfo;

      tgg00_IndexCounter = RECORD
            idc_prim_leafnodes : tsp00_Int4;
            idc_sec_leafnodes  : tsp00_Int4;
            idc_prim_keycount  : tsp00_Int4;
            idc_sec_keycount   : tsp00_Int4;
            idc_nullcount      : tsp00_Int4;
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
                    (mbp_pno_list     : tgg00_PnoList);
                11:
                    (mbp_ColStat      : tgg00_ColStatInfo);
                12:
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


      tgg00_StringFileDescr = RECORD (*40*)
            str_treeid : tgg00_FileId;
            use_mode   : tgg00_FsUseMode;
            codespec   : tsp00_CodeType;
            dhomesite  : tgg00_ServerdbNo;
      END;


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
                END;
            (*ENDCASE*)

      tgg_st_entry_addr  = ^tgg00_StackEntry;
      (**)
      tgg00_StEntryAddr  = tgg_st_entry_addr;
      tgg00_StackList    = ARRAY [1..MAX_INT2_SP00] OF tgg00_StackEntry;
      tgg00_StackListPtr = ^tgg00_StackList;

      tgg00_ObjColDesc = RECORD
            colHashCount_gg00 : tsp00_Int2;
            colInvCount_gg00  : tsp00_Int2;
            colHashList_gg00  : tgg00_StackListPtr;
            colInvList_gg00   : tgg00_StackListPtr
      END;


      tgg00_TriggerInfo = RECORD
            tg_tree_id    : tgg00_FileId;
            tg_out_pos    : tsp00_Uint1;
            tg_misc       : tgg00_MiscSet;
            tg_out_cnt    : tsp00_Int2;
            tg_qual_pos   : tsp00_Int2;
            tg_qual_cnt   : tsp00_Int2;
            tg_key        : tsp00_C4;
            tg_init_key   : tsp00_C4;
            tg_fill       : tsp00_Addr;
            tg_alpha_fill : tsp00_Int4;
            tg_upd_set    : PACKED SET OF 0..MAX_COL_PER_TAB_GG00;
            tg_align_fill : tsp00_Int2;
            tg_align8byte : tsp00_Int4;
      END;

      tgg00_TrigInfoPtr = ^tgg00_TriggerInfo;

      tgg00_TriggerPtr = RECORD
            CASE boolean OF
                true :
                    (stack : tgg_st_entry_addr);
                false :
                    (tr    : tgg00_TrigInfoPtr);
                END;
            (*ENDCASE*)


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
                    msubquery       : boolean;
                    mbool           : boolean;
                    msqlmode        : tsp00_SqlMode;
                    mdatetimeformat : tgg00_DateTimeFormat; (* PTS 1115085 *)
                    malignfill2     : tsp00_Int2;
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


      tgg00_LongCopyQual= RECORD
            lcq_src_lock_tabid    : tgg00_Surrogate;
            lcq_src_pos           : tsp00_Int4;
            lcq_len               : tsp00_Int4;
            lcq_dest_pos          : tsp00_Int4;
            lcq_src_long_in_file  : boolean;
            lcq_dest_long_in_file : boolean;
            lcq_src_tabid         : tgg00_Surrogate;
            lcq_dest_lock_tabid   : tgg00_Surrogate;
      END;


      tgg00_FetchDesc = RECORD
            m_fill1          : tgg00_FileId;
            m_fill2          : tgg00_StackDesc;
            m_resnum         : tsp00_ResNum;
            m_res_build      : boolean;
            m_restree        : tgg00_FileId;
            m_keycnt         : tsp00_ResNum;
            m_fill3          : boolean;
            m_searched_pages : tsp00_Int4;
            (* bis hierher siehe tgg00_QualBuf *)
            m_fns1           : tgg00_FileId;
            m_fns2           : tgg00_FileId;
            m_rescnt         : tsp00_Int4;
            m_rowno          : tsp00_Int4;
            m_cnt            : tsp00_Int4;
            m_use_inv        : boolean;
            m_use_stop       : boolean;
            m_single         : boolean;
            m_use_recs       : boolean;
            m_keylen         : tsp00_Int2;
            m_leng           : tsp00_Int2;
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
                    msubquery        : boolean;
                    mbool            : boolean;
                    msqlmode         : tsp00_SqlMode;
                    mdatetimeformat : tgg00_DateTimeFormat; (* PTS 1115085 *)
                    malignfill2      : tsp00_Int2;
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
                    mr_pagecnt       : tsp00_Int4);
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
                12:
                    (mcl_tree        : tgg00_FileId;         (* copy long *)
                    mcl_stack_desc   : tgg00_StackDesc;
                    mcl_copy_long    : tgg00_LongCopyQual);
                13:
                    (mcfc_tree       : tgg00_FileId;         (* copy file content *)
                    mcfc_pno         : tsp00_PageNo;
                    mcfc_fill_len    : tsp00_Int4);
                14:
                    (mlc_info        : tgg00_LinkChainInfo); (* chaining link parsinfo's *)
                END;
            (*ENDCASE*)

      tgg00_QualBufPtr = ^tgg00_QualBuf;

      tgg00_MessBufHeader = RECORD
            mb_qual_len   : tsp00_Int4;
            mb_data_len   : tsp00_Int4;
            mb_strat_len  : tsp00_Int4;
            mb_fill2      : tsp00_Int4;
            (* *)
            mb_src_site   : tgg00_ServerdbNo;
            mb_reply      : boolean;
            mb_replicated : tgg00_Replicated;
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
                    (* *)
                    mb_data_size    : tsp00_Int4;
                    mb_qual_size    : tsp00_Int4;
                    mb_strat_size   : tsp00_Int4;
                    (* *)
                    mb_st_size      : tsp00_Int4;
                    mb_st_max       : tsp00_Int2;
                    mb_work_st_max  : tsp00_Int2;
                    (* *)
                    mb_workbuf_size : tsp00_Int4);
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
                    mb_ptr_space10_5 : tsp00_C8;       (* mb_trns = mb_trans_child *)
                    mb_ptr_space10_6 : tsp00_C8;
                    mb_trans_child   : tgg00_TransChildPtr);
                11:
                    (mb_pt_space11_1 : tsp00_C8;
                    mb_ptr_space11_2 : tsp00_C8;
                    mb_ptr_space11_3 : tsp00_C8;
                    mb_ptr_space11_4 : tsp00_C8;
                    mb_ptr_space11_5 : tsp00_C8;
                    mb_ptr_space11_6 : tsp00_C8;
                    mb_ptr_space11_7 : tsp00_C8;
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
                    mb_replicated   : tgg00_Replicated;
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
                    (* *)
                    mb_data_size    : tsp00_Int4;
                    mb_qual_size    : tsp00_Int4;
                    mb_strat_size   : tsp00_Int4;
                    (* *)
                    mb_st_size      : tsp00_Int4;
                    mb_st_max       : tsp00_Int2;
                    mb_work_st_max  : tsp00_Int2;
                    (* *)
                    mb_workbuf_size : tsp00_Int4);
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
                    mb_ptr_space10_5 : tsp00_C8;       (* mb_trns = mb_trans_child *)
                    mb_ptr_space10_6 : tsp00_C8;
                    mb_trans_child   : tgg00_TransChildPtr);
                11:
                    (mb_space11      : tgg00_MessBufHeader;
                    mb_ptr_space11_1 : tsp00_C8;
                    mb_ptr_space11_2 : tsp00_C8;
                    mb_ptr_space11_3 : tsp00_C8;
                    mb_ptr_space11_4 : tsp00_C8;
                    mb_ptr_space11_5 : tsp00_C8;
                    mb_ptr_space11_6 : tsp00_C8;
                    mb_ptr_space11_7 : tsp00_C8;
                    mb_strat         : tsp00_MoveObjPtr);
                END;
            (*ENDCASE*)


      tgg00_DbFuncInfo = RECORD
            dbf_codeid              : tgg00_Surrogate;
            dbf_acv_addr            : tsp00_Addr;
            dbf_site                : tgg00_ServerdbNo;
            dbf_new_code            : boolean;
            dbf_com_params          : tsp_dcom_co_create_instance_parms;
            dbf_dispid              : tsp00_Int4;
            dbf_created             : boolean;
      END;


      tgg00_VarColPosList = RECORD
            vpl_last       : tsp00_Int2;
            vpl_last_fill1 : tsp00_Int2;
            vpl_last_fill2 : tsp00_Int4;
            vpl_pos_list   : ARRAY [1..MAX_COL_PER_TAB_GG00] OF tsp00_Int2
      END;


      tgg00_SelectFieldsParam = RECORD
            CASE integer OF
                1:
                    (sfp_addr_fill1     : tsp00_C48; (*  6 * 8 bytes  *)
                    sfp_addr_fill2      : tsp00_C24; (*  3 * 8 bytes  *)
                    sfp_data_size       : tsp00_Int4;
                    sfp_work_st_size    : tsp00_Int4;
                    sfp_workbuf_size    : tsp00_Int4;
                    sfp_workbuf_len     : tsp00_Int4;
                    sfp_act_cntresult   : tsp00_Int4;
                    sfp_oj_spec         : tsp00_Int4;
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
                    sfp_replicated      : tgg00_Replicated;
                    sfp_check_for_result: boolean; (* PTS 1112079 E.Z. *)
                    sfp_bd_inv_only     : tgg00_QualKind;
                    sfp_bd_return_knf   : boolean;
                    sfp_bd_use_stopkey  : boolean;
                    sfp_first_qual      : boolean;
                    sfp_is_first_record : boolean;
                    sfp_result_wanted   : boolean;
                    sfp_varcol_pos      : tgg00_VarColPosList;
                    (* end of init part *)
                    sfp_work_st_top     : tgg_st_entry_addr;
                    sfp_work_st_bottom  : tgg_st_entry_addr;
                    sfp_work_st_last    : tgg_st_entry_addr;
                    sfp_workbuf_top     : tsp00_Int4;
                    sfp_check_new_rec   : boolean;
                    sfp_optimized       : boolean;
                    sfp_sqlmode         : tsp00_SqlMode;
                    sfp_dateTimeFormat  : tgg00_DateTimeFormat; (* PTS 1115085 *)
                    sfp_acv_addr        : tsp00_Addr;
                    );
                2:
                    (sfp_data_addr       : tsp00_MoveObjPtr;
                    sfp_rec_addr         : tsp00_BufAddr;
                    sfp_primkey_addr     : tsp00_KeyAddr;
                    sfp_work_st_addr     : tgg00_StackListPtr;
                    sfp_workbuf_addr     : tsp00_MoveObjPtr;
                    sfp_oldrec_addr      : tsp00_MoveObjPtr;
                    sfp_pl_ptr           : tsp00_BufAddr;
                    sfp_m_result_addr    : tsp00_MoveObjPtr;
                    sfp_m_firstkeys_addr : tgg00_TwoKeysPtr);
                3 :
                    (sfp_init_part       : tgg00_SfpInitPart);
                END;
            (*ENDCASE*)

      tgg00_SelectParamPtr = ^tgg00_SelectFieldsParam;

      tgg00_FuncParamInfo = RECORD
            fpi_nil_ptr : tsp00_ObjAddr;
            fpi_sel_ptr : tgg00_SelectParamPtr;
      END;


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
                    prim_key_read          : tsp00_8ByteCounter;
                    prim_key_qual          : tsp00_8ByteCounter;
                    prim_key_range         : tsp00_8ByteCounter;
                    prim_key_range_read    : tsp00_8ByteCounter;
                    prim_key_range_qual    : tsp00_8ByteCounter;
                    index                  : tsp00_8ByteCounter;
                    index_read             : tsp00_8ByteCounter;
                    index_qual             : tsp00_8ByteCounter;
                    index_range            : tsp00_8ByteCounter;
                    index_range_read       : tsp00_8ByteCounter;
                    index_range_qual       : tsp00_8ByteCounter;
                    is_index               : tsp00_8ByteCounter;
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
                    cget_inv               : tsp00_8ByteCounter;
                    (**)
                    funccount              : tsp00_8ByteCounter; (*==== show monitor transaction ====*)
                    transcount             : tsp00_8ByteCounter;
                    kb_calls               : tsp00_8ByteCounter);
                false :
                    (reference_array : ARRAY [tgg00_RefInfoIndex] OF tsp00_8ByteCounter);
                END;
            (*ENDCASE*)

      tgg00_COMMonEnum = (
            cmiRunTime,
            cmiRunTimeMin, (* PTS 1000230, T.A. 1999-07-08 *)
            cmiRunTimeMax,
            cmiOmsRead,
            cmiLCRead,
            cmiLCKeyRead,
            cmiOmsStore,
            cmiLCStore,
            cmiOmsDelete,
            cmiLCDelete,
            cmiLCLock,
            cmiLCIter,     (* PTS 1107819 T.A 2000-09-13 *)
            cmiLogHop,
            cmiLogHopIter, (* PTS 1107819 T.A 2000-09-13 *)
            cmiExceptions,
            cmiOutOfDate,
            cmiTimeout,
            cmiOutOfMemory, (* PTS 1108772 T.A. 2000-12-13 *)
            cmiSubtransRollback,
            cmiSubtransCommit,
            cmiMaxSubtransLevel,
            cmiLoadVarObj,
            cmiStoreVarObj,
            cmiSumVarObjSize,
            cmiCntVarObj,
            cmiCreateVersion, (* PTS 1000247, T.A. 1999-07-08 *)
            cmiOpenVersion,
            cmiCloseVersion,
            cmiDropVersion,
            cmiStoreInVersion,
            cmiDeleteInVersion,
            cmiLCReadInVersion,
            cmiLCKeyReadInVersion,
            cmiDeleteVarObject,
            cmiMaxWaitNewConsView,
            cmiMinWaitNewConsView,
            cmiSumWaitNewConsView,
            cmiCntWaitNewConsView,
            cmiMallocMin,
            cmiMallocMax,
            cmiMallocSum,
            cmiMallocCnt,
            cmiFreeMin,
            cmiFreeMax,
            cmiFreeSum,
            cmiFreeCnt,
            cmiDeltaMin,
            cmiDeltaMax,
            cmiDeltaAvg,
            cmiOmsTerminate, (* PTS 1112796, T.A. *)
            cmiMinReadApplCallbacks,
            cmiMaxReadApplCallbacks,
            cmiSumReadApplCallbacks,
            cmiMinStreamRowsRead,
            cmiMaxStreamRowsRead,
            cmiSumStreamRowsRead,
            cmiMinWriteApplCallbacks,
            cmiMaxWriteApplCallbacks,
            cmiSumWriteApplCallbacks,
            cmiMinStreamRowsWrite,
            cmiMaxStreamRowsWrite,
            cmiSumStreamRowsWrite,
            cmiCacheSize);

      tgg00_COMMonitorInfo = RECORD
            cmiMethodName_gg00  : tsp00_KnlIdentifier;
            cmiCallCnt_gg00     : tsp00_8ByteCounter;
            cmiCounters_gg00    : ARRAY[tgg00_COMMonEnum] OF tsp00_8ByteCounter;
      END;


      tgg00_COMMonitorTransferBuf = RECORD
            ctbMessType_gg00 : tgg00_MessType;
            ctbDispId_gg00   : tsp00_Int4;
            ctbMonInfo_gg00  : tgg00_COMMonitorInfo;
      END;


      tgg00_ConfigRestartParam = RECORD
            crReservedRedoSize_gg00  : tsp00_Int4;
            crConvRootBlockAddr_gg00 : tsp00_Uint4;
            (* *)
            crFdirRoot_gg00          : tsp00_PageNo;
            crLongDirRoot_gg00       : tsp00_PageNo;
            (* *)
            crMaxDynamicPno_gg00     : tsp00_PageNo;
            crMaxStaticPno_gg00      : tsp00_PageNo;
            (* *)
            crFiller1_gg00           : tsp00_Int4;
            crFiller2_gg00           : tsp00_Int4;
            (* *)
            crFiller3_gg00           : tsp00_Int4;
            crFiller4_gg00           : tsp00_Int4;
            (* *)
            crReleaseVers_gg00       : tsp00_Version;
      END;


      tgg00_ServerdbIdent = RECORD
            CASE integer OF
                1:
                    (svServerdb_gg00  : tsp00_DbName;
                    svServernode_gg00 : tsp00_NodeId);
                2:
                    (ggsv_serverdb    : tsp00_DbName;
                    ggsv_servernode   : tsp00_NodeId);
                END;
            (*ENDCASE*)


      tgg00_ControlUser = RECORD
            ggcu_password : tsp00_CryptPw;
            ggcu_user     : tsp00_KnlIdentifier
      END;


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


      tgg00_NextJoinFields = PACKED RECORD
            jfrom  : tsp00_Int2;
            jto    : tsp00_Int2;
            jlen   : tsp00_Int2;
            jfill1 : tsp00_Int2;
      END;

      tgg00_NJoinArr = ARRAY [1..MAX_JOINS_GG00] OF tgg00_NextJoinFields;

      tgg00_NextJoins = RECORD
            n_j_cnt : tsp00_Int2;
            n_j_pos : tsp00_Int2;
            n_j_arr : tgg00_NJoinArr
      END;


      tgg00_MInfos = RECORD
            o_tree    : tgg00_FileId;
            o_recl    : tsp00_Int2;
            o_keyl    : tsp00_Int2;
            n_rec_len : tsp00_Int2;
            n_key_len : tsp00_Int2;
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
            itBW_egg00,
            itCS_egg00,
            itEMERGE_egg00);

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


      tgg00_CodeGlobals = RECORD
            ctype        : tsp00_CodeType;
            char_default : tsp00_CodeType;
            date_time    : tgg00_DateTimeFormat;
            kernel_swap  : tsp00_SwapKind
      END;


      tgg00_OneShortDay = RECORD
            daynumber : tsp00_Int4;
            dayname   : tsp00_C3;
            filler    : boolean;
      END;


      tgg00_OneLanguageShortDaynames = RECORD
            language : tsp00_C3;
            filler   : boolean;
            days     : ARRAY [cgg_monday..cgg_sunday]
                  OF tgg00_OneShortDay;
      END;

      tgg00_ShortDaynames = ARRAY [1..cgg_languages]
            OF tgg00_OneLanguageShortDaynames;

      tgg00_TaskIdGlobals = RECORD
            taskTraceWriter_gg00  : tsp00_TaskId;
            taskLogWriter_gg00    : tsp00_TaskId;
            taskUtility_gg00      : tsp00_TaskId;
      END;


      tgg00_TabIdGlobals = RECORD
            sys1_cat        : tgg00_FileId;
            sys2_cat        : tgg00_FileId;
            (* PTS 1104393 E.Z. *)
            sys_diag_parse  : tgg00_FileId;
            sys_msg         : tgg00_FileId;
            obj_hash        : tgg00_FileId;
            sys_cmd_analyze : tgg00_FileId;
            sys_data_analyze: tgg00_FileId
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


      tgg00_Err = RECORD
            CASE integer OF
                1 :
                    (errt        : tsp00_C256);
                2 :
                    (errtableid  : tgg00_Surrogate;
                    errstack     : tgg00_StackEntry;
                    errtablesite : tgg00_ServerdbNo;
                    errstacktype : tgg00_Tfn;
                    errrest1     : boolean;
                    errrest2     : tsp00_C20);
                3 :
                    (errfill1    : tgg00_Surrogate;
                    errfill2     : tgg00_StackEntry;
                    errfill3     : tgg00_ServerdbNo;
                    errStackType_gg00 : tgg00_Tfn);
                END;
            (*ENDCASE*)

      tgg_aggr_descr_rec        = tgg00_AggrDescrRec;
      tgg_basis_error           = tgg00_BasisError;
      tgg_bd_inv_set            = tgg00_BdInvSet;
      tgg_bd_set_result_record  = tgg00_BdSetResultRecord;
      tgg_buf_len_info          = tgg00_BufLenInfo;
      tgg_cache_type            = tgg00_CacheType;
      tgg_code_globals          = tgg00_CodeGlobals;
      tgg_code_tables           = tgg00_CodeTables;
      tgg_controluser           = tgg00_ControlUser;
      tgg_datapart              = tgg00_DataPart;
      tgg_datapart_ptr          = tgg00_DataPartPtr;
      tgg_datetimeformat        = tgg00_DateTimeFormat;
      tgg_dbfunc_info           = tgg00_DbFuncInfo;
      tgg_dbspace_info          = tgg00_DbSpaceInfo;
      tgg_debug                 = tgg00_Debug;
      tgg_diag_type             = tgg00_DiagType;
      tgg_directory_monitor_op  = tgg00_DirectoryMonitorOp;
      tgg_distinct              = tgg00_Distinct;
      tgg_double_buf            = tgg00_DoubleBuf;
      tgg_err                   = tgg00_Err;
      tgg_errortext             = tgg00_ErrorText;
      tgg_fetch_desc            = tgg00_FetchDesc;
      tgg_func_param_info       = tgg00_FuncParamInfo;
      tgg_kernel_globals        = tgg00_KernelGlobals;
      tgg_key_range_record      = tgg00_KeyRangeRecord;
      tgg_keylist_buf           = tgg00_KeylistBuf;
      tgg_linkset               = tgg00_Linkset;
      tgg_lkey                  = tgg00_Lkey;
      tgg_long_qual             = tgg00_LongQual;
      tgg_m_infos               = tgg00_MInfos;
      tgg_mess_block            = tgg00_MessBlock;
      tgg_mess_block_ptr        = tgg00_MessBlockPtr;
      tgg_mess_info_buf         = tgg00_RecBody;
      tgg_message2_type         = tgg00_MessType2;
      tgg_message_type          = tgg00_MessType;
      tgg_next_joins            = tgg00_NextJoins;
      tgg_pid_globals           = tgg00_TaskIdGlobals;
      tgg_pos_int               = tgg00_PosInt;
      tgg_priv_r                = tgg00_PrivR;
      tgg_privilege_set         = tgg00_PrivilegeSet;
      tgg_qual_buf              = tgg00_QualBuf;
      tgg_qual_buf_ptr          = tgg00_QualBufPtr;
      tgg_qual_kind             = tgg00_QualKind;
      tgg_rec_buf               = tgg00_Rec;
      tgg_ref_info_index        = tgg00_RefInfoIndex;
      tgg_region_type           = tgg00_RegionType;
      tgg_replicated            = tgg00_Replicated;
      tgg_sample_info           = tgg00_SampleInfo;
      tgg_select_fields_param   = tgg00_SelectFieldsParam;
      tgg_select_param_ptr      = tgg00_SelectParamPtr;
      tgg_seq_info              = tgg00_SeqInfo;
      tgg_serverdb_ident        = tgg00_ServerdbIdent;
      tgg_serverdb_no           = tgg00_ServerdbNo;
      tgg_serverdb_type         = tgg00_ServerdbType;
      tgg_sfp_init_part         = tgg00_SfpInitPart;
      tgg_show_kind             = tgg00_ShowKind;
      tgg_stack_desc            = tgg00_StackDesc;
      tgg_stack_entry           = tgg00_StackEntry;
      tgg_stack_entry_type      = tgg00_StackEntryType;
      tgg_stack_list            = tgg00_StackList;
      tgg_stack_list_ptr        = tgg00_StackListPtr;
      tgg_stack_op_build_in     = tgg00_StackOpBuildIn;
      tgg_stack_op_func         = tgg00_StackOpFunc;
      tgg_stack_op_output       = tgg00_StackOpOutput;
      tgg_stack_op_type         = tgg00_StackOpType;
      tgg_string_filedescr      = tgg00_StringFileDescr;
      tgg_surrogate             = tgg00_Surrogate;
      tgg_sysinfokey            = tgg00_SysInfoKey;
      tgg_syskey                = tgg00_Syskey;
      tgg_tabid_globals         = tgg00_TabIdGlobals;
      tgg_tablekind             = tgg00_TableKind;
      tgg_trigger_info          = tgg00_TriggerInfo;
      tgg_trigger_ptr           = tgg00_TriggerPtr;
      tgg_two_keys              = tgg00_TwoKeys;
      tgg_two_keys_ptr          = tgg00_TwoKeysPtr;
      tgg_two_tree_ids          = tgg00_TwoFileIds;
      tgg_usemode               = tgg00_UseMode;
      tgg_userid                = tgg00_UserId;
      tgg_varcol_pos_list       = tgg00_VarColPosList;
      tgg_vf_fileref            = tgg00_VfFileref;
      tgg_vtrace_state          = tgg00_VtraceState;
      tgg_vtrace_type           = tgg00_VtraceType;


      (*---------------- basic types ---------------------*)
      tak00_ooption = SET OF (is_order_asc, is_opt, is_possible_oflw_field,
            is_val_expression, is_nls_column, is_not_definite,
            is_col_only_specified);

      tak00_ofield  =  RECORD
            ofname   : tsp_knl_identifier;
            ofno     : tsp_int2;
            ofstno   : tsp_int2;
            ofasc    : tak00_ooption;
            oftabno  : tsp00_Uint1;
            offno    : tsp_int2;
            ofapos   : tsp_int4;
            ofnode   : tsp_int2;
            offill   : tsp_int2;
      END;


      tak00_ord_fields  =  RECORD
            ofield  : ARRAY [ 1..cak00_maxordercolumns ] OF tak00_ofield;
            ocntord : tsp_int2;
            ofill1  : tsp_int2;
            ofill2  : tsp_int4;
      END;

      tak00_ord_fields_ptr = ^tak00_ord_fields;
      (* column catalog representation *)
      tak00_colprop  =  (ctkey,    ctopt,    ctdefault,   ctcomment,
            ctunused, ctmulti,     ctlink,    ctdropped,
            ctexpression, ctinvisible, ctcatalog, ctviewname,
            ctjoinviewkey, ctdomain, ctaltered, ctserial);
      tak00_colpropset  =  SET OF tak00_colprop;

      tak00_columninfo  =  PACKED RECORD   (* colinfo_l = 44 B *)
            CASE integer OF
                1 :
                    (ccolstack    : tgg_stack_entry;
                    ccolpropset   : tak00_colpropset;   (* column prop set *)
                    cextcolno     : tsp_int2;
                    creccolno     : tsp_int2;
                    ctabno        : tsp00_Uint1;
                    cdatatyp      : tsp_data_type;
                    cdatalen      : tsp_int2;
                    cinoutlen     : tsp_int2;
                    cdatafrac     : tsp00_Uint1;
                    cshiftoutchar : char;
                    cnextind      : tsp_int2;
                    cudtdatatype  : tsp_c2;
                    cfiller02     : char;
                    ccolumnn_len  : char;
                    ccolumnn      : tsp_knl_identifier);
                2 :
                    (cfiller1     : tgg_stack_entry;
                    cfiller2      : tak00_colpropset;
                    cfiller3      : tsp_int2;
                    cfiller4      : tsp_int2;
                    cfiller5      : tsp00_Uint1;
                    cfiller6      : tsp_data_type;
                    cfiller7      : tsp_int2;
                    cfiller8      : tsp_int2;
                    cfiller9      : tsp00_Uint1;
                    cbinary       : boolean);
                3:
                    (cfiller_1     : tgg_stack_entry;
                    cfiller_2      : tak00_colpropset;
                    cfiller_3      : tsp_int2;
                    cfiller_4      : tsp_int2;
                    cfiller_5      : tsp00_Uint1;
                    cfiller_6      : tsp_data_type;
                    cfiller_7      : tsp_int2;
                    cfiller_8      : tsp_int2;
                    cfiller_9      : tsp00_Uint1;
                    cvarchar       : boolean);
                END;
            (*ENDCASE*)

      tak00_colinfo_ptr  =  ^tak00_columninfo;
      tak00_duration_typ  =  ( year, month, day,
            hour, minute, second, microsecond );

      tak00_scolinf  =  RECORD
            sci_len       : tsp_int2;
            sci_iolen     : tsp_int2;
            sci_frac      : tsp_int2;
            sci_typ       : tsp_data_type;
            sci_dttyp     : tak00_duration_typ;
            sci_cprops    : tak00_colpropset;   (* column prop set *)
            sci_com_type  : tsp_int2;
            sci_udt_id    : tsp_c2;
      END;


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
            s_ebcdic,
            s_byte,
            s_dbyte,
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
            s_columnname,
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
            s_kanji_string,
            s_dbyte_ebcdic,
            s_unused,         (* unused, was s_localsysdba *)
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
            s_get_owner,          (* PTS 1111577 E.Z. *)
            s_get_name);          (* PTS 1111577 E.Z. *)
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
            ddl_analyze_procedure,
            ddl_begin_end_load,
            ddl_comment_on,
            ddl_create_domain,
            ddl_create_index,
            ddl_create_link,
            ddl_create_serverdb,
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
            ddl_drop_serverdb,
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
            ddl_incopy_table,
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
            ddl_create_db_link,
            ddl_drop_db_link,
            ddl_alter_dbproc,
            ddl_rename_user,
            ddl_create_dbfunc,
            ddl_alter_trigger);
      tak_convert_type = (conv_none,
            conv_ascii_to_ebcdic,
            conv_ebcdic_to_ascii,
            conv_ascii_to_unicode,
            conv_unicode_to_ascii,
            conv_ebcdic_to_unicode,
            conv_unicode_to_ebcdic,
            conv_ascii_to_num,
            conv_ebcdic_to_num,
            conv_unicode_to_num,
            conv_num_to_ascii,
            conv_num_to_ebcdic,
            conv_num_to_unicode);
      (* *)
      tak_table_attribute_set = SET OF (ta_fact, ta_dimension, ta_bwhierarchy);
      (* *)
      tak_two_int2       = ARRAY [1..2] OF tsp_int2;

      tak_connect_rec = RECORD
            cn_username       : tsp_knl_identifier;
            cn_timeout        : tsp_int2;
            cn_password       : tsp_cryptpw;
            cn_global_state   : tgg00_HandlingSet;
            cn_tmp_cache_size : tsp_int4;
            cn_messcode       : tsp00_Uint1;
            cn_internal       : boolean;
            cn_filler2        : tsp_int2;
      END;


      tak_sm_description  =  RECORD
            sm_error_cnt      : tsp_int2;
            sm_must_local     : boolean;
            sm_return_req     : boolean;
            sm_not_local      : boolean;
            sm_filler1        : boolean;
            sm_expected_error : tgg_basis_error;
            sm_errorlist      : ARRAY[ 1..cak_max_sm_error ] OF
                  tgg_basis_error;
      END;


      tak_user_info = RECORD
            ui_id      : tgg_surrogate;
            ui_owner   : tgg_surrogate;
            ui_is_role : boolean;
            ui_kind    : tak_usertyp;
      END;


      tak_parskey  =  RECORD
            p_count  : tsp_c3;
            p_id     : tsp_c1;     (* commandno *)
            p_kind   : tgg_message_type;
            p_no     : tsp00_Uint1;   (* max buf_no *)
            p_filler : tsp_int2;
      END;


      tak_parsid = RECORD
            pid_session : tgg91_SessionNo;
            CASE boolean OF
                true :
                    (pid_parsk : tak_parskey);
                false :
                    (pid_fill1     : tsp_c4;
                    pid_fill2      : tsp_c2;
                    pid_appl_info  : tsp_c1;
                    pid_dtime_info : tsp_c1);
                END;
            (*ENDCASE*)

      tak_special_node_info = SET OF
            (ni_complex_predicate, ni_complex_string_id,
            ni_info_node, ni_skip_node, ni_blank_skip);

      tak_ap_node  =  RECORD (* UNIX needs 16 bytes for this *)
            n_proc     : tak_procs;
            n_special  : tak_special_node_info;
            n_symb     : tak_sc_symbol;
            n_datatype : tsp_data_type;
            n_subproc  : tsp_int2;
            n_length   : tsp_int2;
            n_pos      : tsp_int4;
            n_lo_level : tsp_int2; (*see nodepointer in glob1*)
            n_sa_level : tsp_int2;
      END;

      tak_ap_max_tree   =  ARRAY [ 0..cak_max_node ] OF tak_ap_node;
      tak_ap_max_tree_ptr  = ^tak_ap_max_tree;

      tak_scanner_glob  =  RECORD      (* variable : scv *)
            sc_symb           : tak_sc_symbol;
            sc_eof_symb       : tak_sc_symbol;
            sc_sylength       : tsp_int2;
            sc_sypos          : tsp_int4;
            sc_newpos         : tsp_int4;
            sc_lastpos        : tsp_int4;
            sc_missing_kw_pos : tsp_int4;
            sc_double_quote   : tsp_int2;
            sc_reserved_check : boolean;
            sc_pack           : boolean; (*TRUE = shorten id. with '''*)
            sc_eof_kw         : tsp_int2;
      END;

      (* PTS 1111575 E.Z. *)
      tak_execution_kind  =  (only_parsing,
            only_executing,
            parsing_executing,
            pars_then_execute,
            only_syntax);
      tak_charset       =  SET OF char;
      tak_sqlm_charset  =  ARRAY[tsp_sqlmode] OF tak_charset;
      tak_columnset     =  SET OF 0..MAX_COL_PER_TAB_GG00;
      tak_symbolset     =  SET OF tak_sc_symbol;
      tak_datapart      =  PACKED ARRAY [ 1..csp_maxint2 ] OF char;
      tak_eop_arr       =  PACKED ARRAY [ 1..4096] OF tgg_stack_op_type;
      tak_colindex      =  PACKED ARRAY [ 1..MAX_COL_PER_TAB_GG00] OF tsp_int2;
      tak_colinteger    =  PACKED ARRAY [ 1..MAX_COL_PER_TAB_GG00] OF tsp_int2;
      tak_colsequence   =  PACKED ARRAY [ 1..MAX_COL_SEQUENCE_GG00 ] OF tsp_int2;
      tak_inv_colnos    =  PACKED ARRAY [ 1..32 ] OF tsp_int2;
      tak_corlevel      =  ARRAY [ 1..cak_maxcorlevel ] OF tsp00_Uint1;
      tak_keyword       =  PACKED ARRAY [ 1..18 ] OF char;
      tak_oldidentifier =  tsp_c18;
      tak_keywordtab    =  ARRAY [1..cak_maxkeyword]  OF tak_keyword;
      tak_order_errortext = PACKED ARRAY [ 1..mxak_order_errortext ] OF char;
      tak_parsbuffer    =  PACKED ARRAY [ 1..mxak_parsbuf ]    OF char;
      tak_16stackentry  =  ARRAY [ 1..MAX_COL_SEQUENCE_GG00 ] OF
            tgg_stack_entry;
      tak_upd_stat_info  = ARRAY [ 1..MAX_COL_PER_TAB_GG00 ] OF tsp_int4;
      tak_oms_version_id = PACKED ARRAY [ 1..22 ] OF char;
      tak_sysbufferoffset = PACKED ARRAY [ 1..cak_sysbufferoffset ] OF char;
      tak_eop_arr_ptr    = ^tak_eop_arr;
      (* PTS 1109291 E.Z. *)
      tak_pcountarr      = PACKED ARRAY [ 1..cak_max_pcounts ] OF tsp_c3;
      (* PTS 1114411 T.A. *)
      tak_WydeArray      = ARRAY[1..MOVEINT2_MXSP00] OF tsp00_Int2;

      tak_del_tab_qual  =  RECORD
            del_qual_cnt : tsp_int2;
            del_colno    : tsp_int2;
            del_qual     : ARRAY[ 1..cak_max_del_qual ] OF tsp_c2;
      END;


      tak_role_context = RECORD
            rctxt_cnt   : tsp_int4;
            rctxt_roles : ARRAY[1..cak_max_role_context] OF tgg_surrogate;
      END;

      tak_role_context_ptr = ^tak_role_context;
      tak_map_set = PACKED ARRAY [ 1..mxak_mapset  ] OF char;

      tak_mapset_record = RECORD
            map_reclen        : tsp_int2;
            map_keylen        : tsp_int2;
            map_varcol_offset : tsp_int2;
            map_varcol_cnt    : tsp_int2;
            map_surrogate     : tgg_surrogate;
            map_entrytype     : tsp_c2;
            map_linkage       : tsp_c2;
            map_name          : tsp_knl_identifier;
            map_segmentid     : tsp_c2;
            map_count         : tsp_int2;
            map_code          : tsp_code_type; (* ascii, ebcdic *)
            map_filler1       : boolean;
            map_filler2       : tsp_int2;
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
                    (priv_all_set      : tgg_privilege_set;
                    priv_all_grant_set : tgg_privilege_set;
                    priv_col_exist     : tak_priv_desc;
                    priv_filler        : boolean;
                    priv_sel_set       : tak_columnset;
                    priv_upd_set       : tak_columnset;
                    priv_grant_sel_set : tak_columnset;
                    priv_grant_upd_set : tak_columnset;
                    priv_ref_set       : tak_columnset;
                    priv_grant_ref_set : tak_columnset);
                2 :
                    (priv_filler1      : tsp_c4;
                    priv_col : ARRAY[ tak_priv_descriptors ] OF  tak_columnset);
                3 :
                    (priv_c132        : tsp_c132);
                4 :
                    (priv_c196 : PACKED ARRAY[1..196] OF char);
                END;
            (*ENDCASE*)


      tak_privrecord = RECORD
            pr_rec_len       : tsp_int2;
            pr_key_len       : tsp_int2;
            pr_varcol_offset : tsp_int2;
            pr_varcol_cnt    : tsp_int2;
            pr_tabid         : tgg_surrogate;
            pr_entrytype     : tsp_c2;
            pr_linkage       : tsp_c2;
            pr_userid        : tgg_surrogate;
            pr_segmentid     : tsp_c2;
            pr_filler        : tsp_c2;
            pr_priv          : tak_privilege;
      END;


      tak_privuserrecord = RECORD
            pru_rec_len       : tsp_int2;
            pru_key_len       : tsp_int2;
            pru_varcol_offset : tsp_int2;
            pru_varcol_cnt    : tsp_int2;
            pru_tabid         : tgg_surrogate;
            pru_entrytype     : tsp_c2;
            pru_linkage       : tsp_c2;
            pru_grantee       : tgg_surrogate;
            pru_grantor       : tgg_surrogate;
            pru_segmentid     : tsp_c2;
            pru_filler        : tsp_c2;
            pru_date          : tsp_int4;
            pru_time          : tsp_int4;
            pru_priv          : tak_privilege;
      END;


      tak_sprivilege  =  RECORD    (* 68 B *)
            priv_all_set       : tgg_privilege_set;
            priv_col_exist     : tak_priv_desc;
            priv_filler        : tsp_int2;
            priv_sel_set       : tak_columnset;
            priv_upd_set       : tak_columnset;
      END;


      tak_proc_seq_privilege = RECORD
            psp_reclen        : tsp_int2;
            psp_keylen        : tsp_int2;
            psp_varcol_offset : tsp_int2;
            psp_varcol_cnt    : tsp_int2;
            psp_id            : tgg_surrogate;
            psp_type          : tsp_c2;
            psp_linkage       : tsp_c2;
            psp_grantor       : tgg_surrogate;
            psp_seqmentid     : tsp_c2;
            psp_priv          : tgg_privilege_set;
            psp_filler        : boolean;
            psp_date          : tsp_int4;
            psp_time          : tsp_int4;
      END;


      tak_column_statistics  =  RECORD
            c_values       : tsp_int4;
            c_pages        : tsp_int4;
            c_avg_list_len : tsp_int4;
      END;


      tak_colsort  =  RECORD
            colbuf : tsp_int4;
            colind : tsp_int4;
            colpos : tsp_int4;
      END;

      tak_colsortarr  =  ARRAY[ 1..MAX_COL_PER_TAB_GG00 ] OF tak_colsort;
      (* domain catalog representation *)

      tak_domainref  =  RECORD
            dreclen        : tsp_int2;
            dkeylen        : tsp_int2;
            dvarcol_offset : tsp_int2;
            dvarcol_cnt    : tsp_int2;
            downer         : tgg_surrogate;
            dentrytype     : tsp_int2;
            dseqno         : tsp_int2;
            dname          : tsp_knl_identifier;
            dsegmentid     : tsp_c2;
            dfiller        : tsp_c2;
            dsurrogate     : tgg_surrogate;
      END;


      tak_domainrecord  =  RECORD
            dom_reclen        : tsp_int2;
            dom_keylen        : tsp_int2;
            dom_varcol_offset : tsp_int2;
            dom_varcol_cnt    : tsp_int2;
            dom_surrogate     : tgg_surrogate;
            dom_entrytype     : tsp_c2;
            dom_seqno         : tsp_int2;
            dom_segmentid     : tsp_c2;
            dom_filler        : tsp00_Uint1;
            dom_constraint    : boolean;
            dom_type_uid      : tsp_c16;
            dom_owner         : tgg_surrogate;
            dom_date          : tsp_int4;
            dom_time          : tsp_int4;
            dom_colinfo       : tak00_columninfo;
      END;


      tak_col_uses_dom = RECORD
            cud_reclen        : tsp_int2;
            cud_keylen        : tsp_int2;
            cud_varcol_offset : tsp_int2;
            cud_varcol_cnt    : tsp_int2;
            cud_surrogate     : tgg_surrogate;
            cud_entrytype     : tsp_c2;
            cud_linkage       : tsp_c2;
            cud_segmentid     : tsp_c2;
            cud_filler1       : tsp_c2;
            cud_owner         : tgg_surrogate;
            cud_name          : tsp_knl_identifier;
      END;


      tak_sequencerecord  =  RECORD
            seq_reclen        : tsp_int2;
            seq_keylen        : tsp_int2;
            seq_varcol_offset : tsp_int2;
            seq_varcol_cnt    : tsp_int2;
            seq_surrogate     : tgg_surrogate;
            seq_entrytype     : tsp_c2;
            seq_linkage       : tsp_c2;
            seq_segmentid     : tsp_c2;
            seq_comment       : boolean;
            seq_public        : boolean;
            seq_date          : tsp_int4;
            seq_time          : tsp_int4;
            seq_value         : tsp_number;
            seq_owner         : tgg_surrogate;
            seq_info          : tgg_seq_info;
            seq_name          : tsp_knl_identifier;
      END;


      tak_tablereference  =  RECORD
            rtablerefkey1 : tak_sysbufferoffset;
            rtablerefkey2 : tsp_knl_identifier;
            rsegmentid    : tsp_c2;
            rsystable     : boolean;
            rfiller       : boolean;
            rtablekind    : tgg_tablekind;
            rundefbyte    : boolean;
            rtableid      : tgg_surrogate;
            rsynid        : tgg_surrogate;
      END;

      (* table structure and column catalog representation *)
      tak_columnarr  = ARRAY [ 1..1536] OF tak00_colinfo_ptr;
      tak_basefiller = PACKED ARRAY [ 1..16 ] OF char;
      (* default / range catalog representation *)
      tak_align = tsp_c4;
      tak_basecolinfo_ptr = ^tak_basecolinfo;
      (* 4096 has NOTHING to do with tsp_buf or so *)

      tak_basecolinfo = RECORD
            bci_index   : tsp_int4;
            bci_rec_cnt : tsp_int2;
            bci_ref_cnt : tsp_int2;
            bci_buf_size: tsp_int2;
            bci_fill1   : tsp_int2;
            bci_fill2   : tsp_int4;
            bci_next    : tak_basecolinfo_ptr;
            CASE boolean OF
                true :
                    (bci_align : ARRAY[1..cak_max_align] OF tak_align);
                false :
                    (bci_buf : PACKED ARRAY [1..4096] OF char);
                END;
            (*ENDCASE*)


      tak_baserecord  =  RECORD   (* 3996 B *)
            breclen        : tsp_int2;
            bkeylen        : tsp_int2;
            bvarcol_offset : tsp_int2;
            bvarcol_cnt    : tsp_int2;
            bsurrogate     : tgg_surrogate;
            bentrytype     : tsp_c2;
            bseqno         : tsp_c2;
            bsegmentid     : tsp_c2;
            bfirstindex    : tsp_int2;
            blastindex     : tsp_int2;
            btablekind     : tgg_tablekind;
            blinkexist     : tgg_linkset;
            btreeid        : tgg00_FileId;
            bauthid        : tgg_surrogate;
            breccnt        : tsp_int2;    (* 77/78 *)
            bmaxcol        : tsp_int2;
            bmaxreclen     : tsp_int2;
            bkeycolcount   : tsp_int2;
            blenfixedcol   : tsp_int2;
            bvarcolcount   : tsp_int2;
            bcntdropcol    : tsp_int2;
            bstringcount   : tsp_int2;
            blongvarcolcnt : tsp00_Uint1;
            bcomment       : boolean;
            bavgrowlen     : tsp_int2;
            blastkeyind    : tsp_int2;
            bfirstcolind   : tsp_int2;           (* 100 *)
            bdatecreate    : tsp_int4;
            btimecreate    : tsp_int4;
            bupdstatdate   : tsp_int4;
            bupdstattime   : tsp_int4;           (* 116 *)
            bpages         : tsp_int4;
            brows          : tsp_int4;
            balterdate     : tsp_int4;
            baltertime     : tsp_int4;
            bsample        : tsp_int4;
            bpriv_all_set  : tgg_privilege_set;
            bpriv_col_exist: tak_priv_desc;
            bunloaded      : boolean;
            bindexexist    : boolean;
            bv_level       : tsp00_Uint1;
            bv_tabcount    : tsp00_Uint1;
            bv_tablekind   : tgg_tablekind;
            bv_checkopt    : boolean;
            bv_qualexist   : boolean;
            bv_distinct    : tgg_distinct;
            bv_viewlist    : boolean;
            bdef_stamp     : boolean;
            bcorr_view     : boolean;
            bsqlmode       : tsp_sqlmode;
            bnamed_constr  : tsp_int2;
            bshow_kind     : tgg_show_kind;
            brecreate_view : boolean;
            bshowkind      : tgg_show_kind;
            battributes    : tak_table_attribute_set;
            btablen        : tsp_knl_identifier_ptr;
            bptr           : tak_basecolinfo_ptr;
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
            dfreclen           : tsp_int2;
            dfkeylen           : tsp_int2;
            dfvarcol_offset    : tsp_int2;
            dfvarcol_cnt       : tsp_int2;
            dfsurrogate        : tgg_surrogate;
            dfentrytype        : tsp_c2;
            dfseqno            : tsp_c2;
            dfsegment_id       : tsp_c2;
            dfdefault_function : tsp_int2;
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
            tgg_stack_entry;

      tak_alterdaterecord = RECORD
            ad_rec_len       : tsp_int2;
            ad_key_len       : tsp_int2;
            ad_varcol_offset : tsp_int2;
            ad_varcol_cnt    : tsp_int2;
            ad_surrogate     : tgg_surrogate;
            ad_entrytype     : tsp_c2;
            ad_linkage       : tsp_c2;
            ad_segmentid     : tsp_c2;
            ad_filler        : tsp_c2;
            ad_createdate    : tsp_int4;
            ad_createtime    : tsp_int4;
            ad_alterdate     : tsp_int4;
            ad_altertime     : tsp_int4;
      END;

      tak_comment_text = ARRAY[1..mxak_comment] OF char;

      tak_comment_record = RECORD
            com_rec_len       : tsp_int2;
            com_key_len       : tsp_int2;
            com_varcol_offset : tsp_int2;
            com_varcol_cnt    : tsp_int2;
            com_surrogate     : tgg_surrogate;
            com_entrytype     : tsp_c2;
            com_linkage       : tsp_c2;
            com_type          : tak_comment_type;
            com_colno         : tsp_c1;
            com_segmentid     : tsp_c2;
            com_length        : tsp_int4;
            com_text          : tak_comment_text;
      END;


      tak_constraintrecord  =  RECORD
            crec_len       : tsp_int2;
            ckey_len       : tsp_int2;
            cvarcol_offset : tsp_int2;
            cvarcol_cnt    : tsp_int2;
            csurrogate     : tgg_surrogate;
            centrytype     : tsp_c2;
            cseqno         : tsp_c2;
            csegmentid     : tsp_c2;
            cstack_cnt     : tsp_int2;
            cvalue_pos     : tsp_int2;
            cfunction      : boolean;
            coldrange      : boolean;
            ccolset        : tak_columnset;
            ccname         : tak_oldidentifier;
            cstackstart    : tsp00_Uint1;
            csuffixlen     : tsp00_Uint1;
            cfiller        : tsp_int2;
            CASE boolean OF
                true :
                    (cstack     : tak_ak_constr_stack);
                false :
                    (csuffix : tsp_knl_identifier);
                END;
            (*ENDCASE*)


      tak_one_f_proc = RECORD
            ofp_procid   : tgg_surrogate;
            ofp_language : tsp_c3;
            ofp_sqlmode  : SET OF tsp_sqlmode;
      END;

      tak_funcref_restkey = ARRAY[1..56] OF char;

      tak_funcrefrecord = RECORD
            fct_reclen        : tsp_int2;
            fct_keylen        : tsp_int2;
            fct_varcol_offset : tsp_int2;
            fct_varcol_cnt    : tsp_int2;
            fct_key1          : tsp_c8;
            fct_entrytype     : tsp_c2;
            fct_linkage       : tsp_c2;
            fct_key2          : tak_funcref_restkey;
            fct_segmentid     : tsp_c2;
            fct_proc_cnt      : tsp00_Uint1;
            fct_comment       : boolean;
            fct_fill2         : tsp_int4;
            fct_procids       : ARRAY[1..cak_max_proc] OF tak_one_f_proc;
      END;


      tak_constraintnamerecord  =  RECORD
            cnreclen        : tsp_int2;
            cnkeylen        : tsp_int2;
            cnvarcol_offset : tsp_int2;
            cnvarcol_cnt    : tsp_int2;
            cnkey1          : tsp_c12;
            cnkey2          : tsp_knl_identifier;
            cnsegmentid     : tsp_c2;
            cnid            : tsp_int2;
            cnfiller        : tsp_int2;
      END;

      (* named index catalog representation *)

      tak_multindex  =  RECORD    (* 212 *)
            indexn_prefix : tak_oldidentifier;
            indexno       : tsp_int2;
            icount        : tsp00_Uint1;
            ifill         : boolean;
            icolseq       : tak_colsequence;
            iunique       : boolean;
            icomment      : boolean;
            idatecre      : tsp_int4;
            itimecre      : tsp_int4;
            irows         : tsp_int4;
            ipages        : tsp_int4;
            iinvl         : tsp_int4;
            ifiller       : boolean;     (* unused *)
            idisabled     : boolean;
            ifiller1      : boolean; (* PTS 1114312 *)
            isuffixlen    : tsp00_Uint1;
            icolstack     : tak_16stackentry;
      END;


      tak_mindexrecord  =  RECORD   (* 3396 *)
            ireclen        : tsp_int2;
            ikeylen        : tsp_int2;
            ivarcol_offset : tsp_int2;
            ivarcol_cnt    : tsp_int2;
            isurrogate     : tgg_surrogate;
            ientrytype     : tsp_c2;
            iseqno         : tsp_c2;
            isegmentid     : tsp_c2;
            indexcount     : tsp00_Uint1;
            indexnext      : boolean;
            indexdef       : ARRAY [ 1..cak_max_index_per_rec ] OF tak_multindex;
      END;

      (* link catalog representation *)

      tak_linkdef  =  RECORD   (* 216 *)
            ltableid      : tgg_surrogate;
            linkn_prefix  : tak_oldidentifier;
            lfiller1      : tsp_int2;           (* unused *)
            ldatecre      : tsp_int4;
            ltimecre      : tsp_int4;
            ldatatyp      : tsp_data_type;
            laction       : tsp00_Uint1;
            lcolcount     : tsp00_Uint1;
            lindexid      : tsp_c1;
            lcomment      : boolean;
            lsuffixlen    : tsp00_Uint1;
            lfiller2      : tsp_int2;
            lprimcolseq   : tak_colsequence;
            lseccolseq    : tak_colsequence;
            lrecpos       : tak_colsequence;
            lstack        : tak_16stackentry;
      END;


      tak_linkrecord  =  RECORD
            lreclen        : tsp_int2;
            lkeylen        : tsp_int2;
            lvarcol_offset : tsp_int2;
            lvarcol_cnt    : tsp_int2;
            lsurrogate     : tgg_surrogate;
            lentrytype     : tsp_c2;
            lseqno         : tsp_c2;
            lsegmentid     : tsp_c2;
            linkcount      : tsp_int2;
            linkdef        : ARRAY [ 1..cak_maxlinkdef ]   OF tak_linkdef;
      END;


      tak_link_def_record  =  RECORD
            lreclen        : tsp_int2;
            lkeylen        : tsp_int2;
            lvarcol_offset : tsp_int2;
            lvarcol_cnt    : tsp_int2;
            lsurrogate     : tgg_surrogate;
            lentrytype     : tsp_c2;
            lsegno         : tsp_c2;
            lkeyname       : tsp_knl_identifier;
            lsegmentid     : tsp_c2;
            ldefault_len   : tsp_int2;
            sfiller        : tsp_int2;
            ldefault_key   : tsp_key;
      END;


      tak_linkparsinfo = RECORD
            lptreeid      : tgg00_FileId;
            lpauxfile     : tgg00_FileId;
            lpfiller      : tsp_int2;          (* unused *)
            lpcolcount    : tsp00_Uint1;
            lp_one_to_one : boolean;
            lpstack       : tak_16stackentry;
      END;


      tak_linkparsrecord  =  RECORD
            lpreclen        : tsp_int2;
            lpkeylen        : tsp_int2;
            lpvarcol_offset : tsp_int2;
            lpvarcol_cnt    : tsp_int2;
            lpsurrogate     : tgg_surrogate;
            lpentrytype     : tsp_c2;
            lpseqno         : tsp_c2;
            lpsegmentid     : tsp_c2;
            lpcount         : tsp_int2;
            lpinfo          : ARRAY [ 1..cak_maxlinkdef ] OF tak_linkparsinfo;
      END;

      tak_decresstate  =  (dr_ansi,
            dr_db2,
            dr_db2_with_hold,
            dr_oracle,
            dr_sql_db);
      tak_resstate  =  (rs_result,
            rs_noresult,
            rs_intinvnoresult,
            rs_nobase,
            rs_useinv,
            rs_joinupdate,
            rs_show,
            rs_result_longcolumn); (* PTS 1113491 *)
      tak_sresstate  =  SET OF tak_resstate;
      tak_select_fetch_type = (
            sft_normal_select,
            sft_select_all_results,
            sft_select_some_results);

      tak_resname_record  =  RECORD
            resreclen         : tsp_int2;
            reskeylen         : tsp_int2;
            resvarcol_offset  : tsp_int2;
            resvarcol_cnt     : tsp_int2;
            reskey_idfill     : tgg_surrogate;
            reskey_identr     : tsp_c2;
            reskey_name       : tsp_knl_identifier;
            reskey_modul_name : tsp_knl_identifier;
            resinfobuf        : tsp00_Uint1;
            resparsinfobyte   : tsp00_Uint1;
            restreeid         : tgg00_FileId;
            resunlocktabid    : tgg_surrogate;
            restrans          : tgg91_TransNo;
            ressubfill        : tsp_int2;            (* unused *)
            ressubtrans       : tgg00_SubtransNo;   (* PTS 1109485 E.Z. *)
            reseparsk         : tak_parskey;
            resstate          : tak_sresstate;
            resmaxlinkage     : tsp00_Uint1;
            resdecresdel      : tak_decresstate;
            resdatetimeformat : tgg_datetimeformat;
            resimpl_upd_stat  : boolean;
            resexecuted       : boolean;
            resselect_fetch   : tak_select_fetch_type;
            resanalyze_cmdid  : tgg_surrogate;
      END;

      tak_resn_rec_arr = ARRAY [1..cak_max_resn] OF tak_resname_record;
      (* trigger representation *)
      tak_trigger_stack = ARRAY[1..cak_max_trigger_stack] OF tgg_stack_entry;

      tak_triggerrecord = RECORD
            tr_reclen         : tsp_int2;
            tr_keylen         : tsp_int2;
            tr_varcol_offset  : tsp_int2;
            tr_varcol_cnt     : tsp_int2;
            tr_surrogate      : tgg_surrogate;
            tr_entrytype      : tsp_c2;
            tr_linkage        : tsp_c2;
            tr_segmentid      : tsp_c2;
            tr_param_cnt      : tsp_int2;
            tr_condition_cnt  : tsp_int2;
            tr_succ_exists    : boolean;
            tr_comment        : boolean;
            tr_proc_id        : tgg_surrogate;
            tr_date           : tsp_int4;
            tr_time           : tsp_int4;
            tr_upd_set        : tak_columnset; (* 130 *)
            tr_filler1        : tsp_int2;
            tr_filler2        : tsp00_Uint1;
            tr_name_len       : tsp00_Uint1;
            CASE boolean OF
                true :
                    (tr_name  : tsp_knl_identifier);
                false :
                    (tr_stack : tak_trigger_stack);
                END;
            (*ENDCASE*)
            (* usage catalog representation *)


      tak_usagedef  =  RECORD   (* 12 B *)
            usa_tableid   : tgg_surrogate;
            usa_filler    : tsp_int2;           (* unused *)
            usa_tablekind : tgg_tablekind;
            usa_empty     : boolean;
      END;


      tak_usagerecord  =  RECORD
            usagereclen        : tsp_int2;
            usagekeylen        : tsp_int2;
            usagevarcol_offset : tsp_int2;
            usagevarcol_cnt    : tsp_int2;
            usagesurrogate     : tgg_surrogate;
            usageentrytype     : tsp_c2;
            usageseqno         : tsp_c2;
            usagesegmentid     : tsp_c2;
            usagefiller        : tsp_int2;
            usagecount         : tsp_int2;
            usagenext_exist    : boolean;
            usagefull          : boolean;
            usagedef           : ARRAY [ 1..cak_max_usage ] OF tak_usagedef;
      END;

      (* usertab catalog representation *)
      tak_usertab_descriptor  =  (ut_table,
            ut_base_table,
            ut_procedure,
            ut_sequence,
            ut_adabas_systable,
            ut_oracle_systable,
            ut_view,
            ut_filler,            (* unused  PTS 1111576 E.Z. *)
            ut_role);

      tak_usertabdef  =  RECORD   (* 12 *)
            ut_surrogate : tgg_surrogate;
            ut_empty     : boolean;
            ut_kind      : tak_usertab_descriptor;
            ut_filler    : tsp_int2;          (* unused *)
      END;


      tak_usertablerecord  =  RECORD
            usatabreclen        : tsp_int2;
            usatabkeylen        : tsp_int2;
            usatabvarcol_offset : tsp_int2;
            usatabvarcol_cnt    : tsp_int2;
            usatabsurrogate     : tgg_surrogate;
            usatabentrytype     : tsp_c2;
            usatabseqno         : tsp_c2;
            usertabsegmentid    : tsp_c2;
            usertabfiller       : tsp_int2;
            usertabnext_exist   : boolean;
            usertabfull         : boolean;
            usertabcount        : tsp_int2;
            usertabdef          : ARRAY [ 1..cak_max_usertab ] OF
                  tak_usertabdef;
      END;

      (* user catalog representation *)
      tak_userparams  =  (up_unused1, up_unused2, maxtimeout,
            costwarning, costlimit, cachelimit);
      tak_userpar_arr =  ARRAY [ tak_userparams ] OF tsp_int4;
      tak_userrectype =  SET OF (is_user_rec, is_group_rec, is_role_rec);
      tak_role_info   =  (role_all, role_none, role_except);

      tak_userrecord  =  RECORD  (* 120 *)
            ureclen        : tsp_int2;
            ukeylen        : tsp_int2;
            uvarcol_offset : tsp_int2;
            uvarcol_cnt    : tsp_int2;
            usurrogate     : tgg_surrogate;
            uentrytype     : tsp_c2;
            useqno         : tsp_c2;
            usersegmentid  : tsp_c2;
            userfiller1    : boolean;
            userroleinfo   : tak_role_info;
            urecordtyp     : tak_userrectype;
            userkind       : tak_usertyp;
            usersysdba     : boolean;
            uexclusive     : boolean;
            userfiller2    : tsp_int2;
            userfiller3    : tsp_int2;
            userpassw      : tsp_cryptpw;
            usupportpasspw : tsp_cryptpw;
            usergroup_id   : tgg_surrogate;
            userowner_id   : tgg_surrogate;
            upwcreatedate  : tsp_int4;
            upwcreatetime  : tsp_int4;
            ucreatedate    : tsp_int4;
            ucreatetime    : tsp_int4;
            uparams        : tak_userpar_arr;
            uperm_ref      : tgg00_UserRef;
            ucomment       : boolean;
            userrolecnt    : tsp00_Uint1;
            ualterdate     : tsp_int4;
            ualtertime     : tsp_int4;
            username       : tsp_knl_identifier;
      END;


      tak_useridrecord = RECORD
            us_reclen        : tsp_int2;
            us_keylen        : tsp_int2;
            us_varcol_offset : tsp_int2;
            us_varcol_cnt    : tsp_int2;
            us_surrogate     : tgg_surrogate;
            us_entrytype     : tsp_c2;
            us_linkage       : tsp_c2;
            us_segmentid     : tsp_c2;
            us_filler        : tsp_c2;
            us_userid        : tsp_c4;
      END;

      (* user reference catalog representation username  ==> user_id *)
      tak_uref_restkey = ARRAY[1..56] OF char;

      tak_userrefrecord  =  RECORD
            ru_key_offset : tak_sysbufferoffset;
            ru_rest_key   : tak_uref_restkey;
            ru_segmentid  : tsp_c2;
            ru_filler     : tsp_int2;
            ru_user_id    : tgg_surrogate;
      END;

      (* system user catalog representation *)

      tak_sysdbarecord  =  RECORD
            sysreclen        : tsp_int2;
            syskeylen        : tsp_int2;
            sysvarcol_offset : tsp_int2;
            sysvarcol_cnt    : tsp_int2;
            syssurrogate     : tgg_surrogate;
            sysentrytyp      : tsp_c2;
            syssegno         : tsp_c2;
            syssegmentid     : tsp_c2;
            sysactivated     : boolean;
            sysversion       : tsp00_Uint1;
            sysauthid        : tgg_surrogate;
      END;

      (* location catalog representation *)

      tak_serverdbrecord  =  RECORD
            sitereclen        : tsp_int2;
            sitekeylen        : tsp_int2;
            sitevarcol_offset : tsp_int2;
            sitevarcol_cnt    : tsp_int2;
            sitekey           : tsp_c12;
            sitesegmentid     : tsp_c2;
            sitemap_no        : tgg_serverdb_no;
            siteserverdbname  : tsp_dbname;
            sitetype          : tgg_serverdb_type;
            sitefiller1       : boolean;
            siteid            : tsp_nodeid;
            siterefcount      : tsp_int4;
      END;


      tak_serverdbnamerecord  =  RECORD
            snreclen        : tsp_int2;
            snkeylen        : tsp_int2;
            snvarcol_offset : tsp_int2;
            snvarcol_cnt    : tsp_int2;
            snkey1          : tsp_c12;
            snkey2          : tsp_dbname;
            snsegmentid     : tsp_c2;
            snnodeid        : tsp_nodeid;
            snsite          : tgg_serverdb_no;
            snsitetype      : tgg_serverdb_type;
            sncode          : tsp_code_type;
            sntermchar      : tak_oldidentifier;
            snremote_site   : tgg_serverdb_no;
            snlocal_site    : tgg_serverdb_no;
            snfiller2       : tsp_int2;
      END;

      (* stored procedure catalog representation *)
      tak_param_spec_ptr = ^tsp1_param_spec_list;

      tak_param_list = RECORD
            param_spec_ptr    : tak_param_spec_ptr;
            param_counter     : tsp_int2;
            param_filler1     : tsp_int2;
            param_filler2     : tsp_int4;
      END;

      tak_param_state = (param_undef, param_new, param_old);

      tak_param_info  =  RECORD
            param_datatype        : tsp_data_type;
            param_datafrac        : tsp00_Uint1;
            param_datalength      : tsp_int2;
            param_inoutlength     : tsp_int2;
            param_in_out          : tsp_c2;
            param_com_datatype    : tsp_int2;
            param_type_id         : tsp_c2;
            param_cpp_offset      : tsp_int2;
            param_ascii_offset    : tsp_int2;
            param_unicode_offset  : tsp_int2;
            param_new_old         : tak_param_state;
            param_name_len        : char;
            param_name            : tsp_c256;
      END;

      tak_param_info_ptr = ^tak_param_info;
      tak_param_array  =    ARRAY [ 1..cak_max_param_index ] OF
            tak_param_info_ptr;
      tak_proc_kind = (p_dbproc, p_trigger, p_function);
      tak_debug_mode = (dbg_inproc, dbg_local, dbg_remote);

      tak_methodrecord = RECORD
            me_reclen              : tsp_int2;
            me_keylen              : tsp_int2;
            me_varcol_offset       : tsp_int2;
            me_varcol_cnt          : tsp_int2;
            me_surrogate           : tgg_surrogate;
            me_entrytype           : tsp_c2;
            me_linkage             : tsp_c2;
            me_segmentid           : tsp_c2;
            me_param_cnt           : tsp_int2;
            me_type                : tsp_db_obj_type;
            me_debug               : tak_debug_mode;
            me_sql                 : boolean;
            me_cursor              : boolean;
            me_dispid              : tsp_int4;
            me_coclsid             : tsp_c16;
            me_iid                 : tsp_c16;
            me_package_id          : tgg_surrogate;
            me_param_list          : tak_param_array;
      END;


      tak_method_inforecord = RECORD
            mei_reclen         : tsp_int2;
            mei_keylen         : tsp_int2;
            mei_varcol_offset  : tsp_int2;
            mei_varcol_cnt     : tsp_int2;
            mei_surrogate      : tgg_surrogate;
            mei_entrytype      : tsp_c2;
            mei_linkage        : tsp_c2;
            mei_segmentid      : tsp_c2;
            mei_comment        : boolean;
            mei_filler         : boolean;
            mei_owner          : tgg_surrogate;
            mei_date           : tsp_int4;
            mei_time           : tsp_int4;
            mei_prog_id        : tsp_c64;
            mei_name           : tsp_knl_identifier;
            mei_debug_location : tsp_c132;
      END;


      tak_methodrefrecord = RECORD
            mrf_reclen        : tsp_int2;
            mrf_keylen        : tsp_int2;
            mrf_varcol_offset : tsp_int2;
            mrf_varcol_cnt    : tsp_int2;
            mrf_surrogate     : tgg_surrogate;
            mrf_entrytype     : tsp_c2;
            mrf_linkage       : tsp_c2;
            mrf_methodname    : tsp_knl_identifier;
            mrf_segmentid     : tsp_c2;
            mrf_type          : tsp_db_obj_type;
            mrf_filler        : boolean;
            mrf_method_id     : tgg_surrogate;
      END;


      tak_type_uid_map_record = RECORD
            tum_reclen        : tsp_int2;
            tum_keylen        : tsp_int2;
            tum_varcol_offset : tsp_int2;
            tum_varcol_cnt    : tsp_int2;
            tum_uid1          : tsp_c8;
            tum_entrytype     : tsp_c2;
            tum_linkage       : tsp_c2;
            tum_uid2          : tsp_c8;
            tum_segmentid     : tsp_c2;
            tum_id            : tsp_c2;
      END;


      tak_class_map_guid_record = RECORD
            cmr_reclen        : tsp_int2;
            cmr_keylen        : tsp_int2;
            cmr_varcol_offset : tsp_int2;
            cmr_varcol_cnt    : tsp_int2;
            cmr_surrogate     : tgg_surrogate;
            cmr_entrytype     : tsp_c2;
            cmr_linkage       : tsp_c2;
            cmr_segmentid     : tsp_c2;
            cmr_container_no  : tsp_int4;
            cmr_schema        : tsp00_Int4;
            cmr_guid          : tsp_c16;
      END;


      tak_class_record = RECORD
            freclen        : tsp_int2;
            fkeylen        : tsp_int2;
            fvarcol_offset : tsp_int2;
            fvarcol_cnt    : tsp_int2;
            fschema        : tgg00_Surrogate;
            fentrytype     : tsp_c2;
            flinkage       : tsp_c2;
            fguid          : tsp_c16;
            fsegmentid     : tsp_c2;
            findexexist    : boolean;
            ffiller        : boolean;
            fsize          : tsp_int4;
            ftree          : tgg00_FileId;
            fdate          : tsp_int4;
            ftime          : tsp_int4;
            fclassname     : tsp_knl_identifier;
      END;


      tak_schema_ref_record = RECORD
            schr_reclen       : tsp_int2;
            schr_keylen       : tsp_int2;
            schrvarcol_offset : tsp_int2;
            schrvarcol_cnt    : tsp_int2;
            schr_surrogate    : tgg00_Surrogate;
            schr_entrytype    : tsp_c2;
            schr_linkage      : tsp_c2;
            schr_name         : tsp00_KnlIdentifier;
            schr_segmentid    : tsp_c2;
            schr_filler       : tsp00_Int4;
            schr_schema_id    : tsp00_Int4;
      END;


      tak_schema_record = RECORD
            sch_reclen        : tsp_int2;
            sch_keylen        : tsp_int2;
            sch_varcol_offset : tsp_int2;
            sch_varcol_cnt    : tsp_int2;
            sch_surrogate     : tgg00_Surrogate;
            sch_entrytype     : tsp_c2;
            sch_linkage       : tsp_c2;
            sch_segmentid     : tsp_c2;
            sch_filler        : tsp_c2;
            sch_date          : tsp_int4;
            sch_time          : tsp_int4;
            sch_name          : tsp_knl_identifier;
      END;


      tak_version_record = RECORD
            vreclen        : tsp_int2;
            vkeylen        : tsp_int2;
            vvarcol_offset : tsp_int2;
            vvarcol_cnt    : tsp_int2;
            vsurrogate     : tgg_surrogate;
            ventrytype     : tsp_c2;
            vlinkage       : tsp_c2;
            vversion_id    : tak_oms_version_id;
            vsegmentid     : tsp_c2;
            vconsistview   : tgg91_TransNo;
            vmarked        : boolean;
            vfiller        : boolean;
            vdate          : tsp_int4;
            vtime          : tsp_int4;
            vupdtree       : tgg00_FileId;
      END;


      tak_rolerecord = RECORD
            role_reclen        : tsp_int2;
            role_keylen        : tsp_int2;
            role_varcol_offset : tsp_int2;
            role_varcol_cnt    : tsp_int2;
            role_key1          : tgg_surrogate;
            role_entrytype     : tsp_c2;
            role_linkage       : tsp_c2;
            role_rest_key      : tak_uref_restkey;
            role_segmentid     : tsp_c2;
            role_filler        : tsp_c2;
            role_owner         : tgg_surrogate;
            role_id            : tgg_surrogate;
            role_date          : tsp_int4;
            role_time          : tsp_int4;
            role_password      : tsp_cryptpw;
      END;

      (* synonym catalog representation *)

      tak_synonymrecord  =  RECORD   (* 128 *)
            synreclen        : tsp_int2;
            synkeylen        : tsp_int2;
            synvarcol_offset : tsp_int2;
            synvarcol_cnt    : tsp_int2;
            synsurrogate     : tgg_surrogate;
            synentrytype     : tsp_c2;
            synseqno         : tsp_c2;
            synsegmentid     : tsp_c2;
            syn_maxcol       : tsp_int2;
            syn_internal     : boolean; (* PTS 1109242 *)
            synfiller0       : boolean;
            syn_tablekind    : tgg_tablekind;
            syncomment       : boolean;
            syn_tableid      : tgg_surrogate;
            syn_time         : tsp_int4;
            syn_date         : tsp_int4;
            syn_owner        : tgg_surrogate;
            syn_authid       : tgg_surrogate;
            syn_tablen       : tsp_knl_identifier;
      END;

      (* cammand info catalog representation *)

      tak_commandinforecord = RECORD
            cireclen        : tsp_int2;
            cikeylen        : tsp_int2;
            civarcol_offset : tsp_int2;
            civarcol_cnt    : tsp_int2;
            cisurrogate     : tgg_surrogate;
            cientrytype     : tsp_c2;
            cilinkage       : tsp_c2;
            cicommand_id    : tsp00_C8;
      END;

      tak_jcolprop  =  (jtkey, jtfirstkey, jtonlykey, jtsingle,
            jtmulti, jtunique, jtascii, jtebcdic);
      tak_jcolpropset  =  SET OF tak_jcolprop;
      tak_joinset      =  PACKED SET OF 1..cak_maxsources;

      tak_one_joinpart  =  PACKED RECORD
            jop_multiplier   : tsp_int4;        (* ~ distinctvalue *)
            jop_fieldno      : tsp_int2;        (* record column position *)
            jop_startstack   : tsp_int2;        (* stack code position *)
            jop_cntstack     : tsp_int2;        (* stackentry count for join description *)
            jop_tableno      : tsp00_Uint1;     (* index in tak_all_from_tables *)
            jop_datatyp      : tsp_data_type;   (* from tak_colinfo *)
            jop_datalen      : tsp_int2;        (* from tak_colinfo *)
            jop_inoutlen     : tsp_int2;        (* from tak_colinfo *)
            jop_propset      : tak_jcolpropset;
            jop_outer_join   : boolean;
            jop_outpos       : tsp_int2;
      END;


      tak_one_join  =  PACKED RECORD
            jo_recs          : ARRAY [ 1..2 ] OF tak_one_joinpart;
            jo_op            : tgg_stack_op_type;
            jo_no_join       : boolean;
            jo_partno        : tsp00_Uint1;
            jo_col_upd_stat  : tsp00_Uint1;
      END;

      tak_joinarr  =  ARRAY [ 0..MAX_JOINS_GG00 ] OF tak_one_join;

      tak_joinrec  =  RECORD
            jrc_joinarr : tak_joinarr;
            jrc_cnt     : tsp_int2;
            jrc_col_upd : boolean;
            jrc_filler  : tsp00_Uint1;
      END;

      (*======== view catalog representation ========*)
      (* view text catalog representation *)

      tak_vttabdef  =  RECORD   (* 24 B *)
            vtttableid      : tgg_surrogate;
            vttreftabid     : tgg_surrogate;
            vttfiller       : tsp_int2;           (* unused *)
            vttrefname      : boolean;
            vtttabcount     : tsp00_Uint1;
            vttsynonym      : boolean;
            vttcomplex_view : boolean;
            vtttab_n_pos    : tsp_int2;
      END;

      tak_viewtext  =  PACKED ARRAY [ 1..mxak_viewtext  ] OF char;

      tak_viewtextrecord  =  RECORD    (* 3992 *)
            vtreclen        : tsp_int2;
            vtkeylen        : tsp_int2;
            vtvarcol_offset : tsp_int2;
            vtvarcol_cnt    : tsp_int2;
            vtsurrogate     : tgg_surrogate;
            vtentrytype     : tsp_c2;
            vtsegno         : tsp_c2;
            vtsegmentid     : tsp_c2;
            vtfromtabcnt    : tsp_int2;
            vttabcount      : tsp_int2;
            vttextlength    : tsp_int2;
            vtnextexist     : boolean;
            vtcontext       : tsp00_Uint1;
            vtselect_pos    : tsp_int2;
            CASE boolean OF
                true :
                    (vttab : ARRAY [ 1..cak_maxsources ] OF tak_vttabdef);
                false :
                    (vttbuf : tak_viewtext);
                END;
            (*ENDCASE*)
            (* view column desription catalog representation *)


      tak_vcolumndescription  =  PACKED RECORD
            vfromtabno    : tsp_int2;
            vn_pos        : tsp_int2;
            vfromextcolno : tsp_int2;
            vextcolno     : tsp_int2;
            vdatatype     : tsp_data_type;
            vfiller       : boolean;
            vfiller1      : tsp_int2;
      END;


      tak_viewdescrecord  =  RECORD
            vdreclen        : tsp_int2;
            vdkeylen        : tsp_int2;
            vdvarcol_offset : tsp_int2;
            vdvarcol_cnt    : tsp_int2;
            vdsurrogate     : tgg_surrogate;
            vdentrytype     : tsp_c2;
            vdsegno         : tsp_int2;
            vsegmentid      : tsp_c2;
            vdesc_cnt       : tsp_int2;
            vdesc_next      : boolean;
            vdesc_fill1     : boolean;
            vdesc_fill2     : tsp_int2;
            vdescription    : ARRAY[ 1..cak_maxviewdesc ] OF
                  tak_vcolumndescription;
      END;

      (* view qualification catalog representation *)

      tak_viewtabdef  =  RECORD   (* 44 Bytes *)
            vttableid        : tgg_surrogate;
            vtfiller         : tsp_int2;      (* unused *)
            vtins_for_upd    : boolean;
            vtone_to_one     : boolean;
            vtnot_used_links : tak_charset;
      END;

      tak_viewtabarr  =  ARRAY[ 1..cak_max_viewqual_tab ] OF tak_viewtabdef;

      tak_one_viewjoin  =  PACKED RECORD
            j1fieldno    : tsp_int2;
            j1startstack : tsp_int2;
            j1tableno    : tsp00_Uint1;
            j1cntstack   : tsp00_Uint1;
            j1propset    : tak_jcolpropset;
            j1fill       : char;
            j1inoutlen   : tsp_int2;
            j2fieldno    : tsp_int2;
            j2tableno    : tsp00_Uint1;
            j2cntstack   : tsp00_Uint1;
            j2propset    : tak_jcolpropset;
            j2fill       : char;
            j2inoutlen   : tsp_int2;
            j12operator  : tgg_stack_op_type;
            j12fill1     : char;
            j12fill2     : tsp_int4;
      END;

      tak_viewjoins  =  ARRAY [ 1..cak_max_viewqual_join ] OF
            tak_one_viewjoin;
      tak_view_stack_entries   =   ARRAY [ 1..cak_max_viewqual_stack ] OF
            tgg_stack_entry;
      tak_viewbuf  =  PACKED ARRAY [ 1..cak_max_viewqual_data  ] OF char;

      tak_one_derived_desc  =  PACKED RECORD
            drecno    : tsp_int2;
            dextno    : tsp_int2;
            dfiller   : tsp_int2;
            dexttabno : tsp00_Uint1;
            dtabno    : tsp00_Uint1;
      END;

      tak_derived_col_info  =  ARRAY[ 1..cak_max_viewqual_derivedcol ] OF
            tak_one_derived_desc;

      tak_viewqual_basis_record = RECORD
            vreclen         : tsp_int2;
            vkeylen         : tsp_int2;
            vvarcol_offset  : tsp_int2;
            vvarcol_cnt     : tsp_int2;
            vsurrogate      : tgg_surrogate;
            ventrytype      : tsp_c2;
            vlinkage        : tsp_c2;
            vsegmentid      : tsp_c2;
            vfiller         : boolean;
            vjoin_exists    : boolean;
            vstack_exists   : boolean;
            vderived_exists : boolean;
            vbasetabcnt     : tsp_int2;
            vtable          : tak_viewtabarr;
      END;


      tak_viewqual_join_record = RECORD
            vreclen         : tsp_int2;
            vkeylen         : tsp_int2;
            vvarcol_offset  : tsp_int2;
            vvarcol_cnt     : tsp_int2;
            vsurrogate      : tgg_surrogate;
            ventrytype      : tsp_c2;
            vlinkage        : tsp_c2;
            vsegmentid      : tsp_c2;
            vjoincount      : tsp_int2;
            vjoin           : tak_viewjoins;
      END;


      tak_viewqual_stack_record = RECORD
            vreclen         : tsp_int2;
            vkeylen         : tsp_int2;
            vvarcol_offset  : tsp_int2;
            vvarcol_cnt     : tsp_int2;
            vsurrogate      : tgg_surrogate;
            ventrytype      : tsp_c2;
            vlinkage        : tsp_c2;
            vsegmentid      : tsp_c2;
            vfiller         : tsp_c2;
            vview_offs      : tsp_int2;
            vstcount        : tsp_int2;
            vdatapos        : tsp_int4;
            vdatalen        : tsp_int4;
            CASE boolean OF
                true:
                    (vstack : tak_view_stack_entries);
                false:
                    (vdata  : tak_viewbuf);
                END;
            (*ENDCASE*)


      tak_viewqual_derivedcol_record = RECORD
            vreclen         : tsp_int2;
            vkeylen         : tsp_int2;
            vvarcol_offset  : tsp_int2;
            vvarcol_cnt     : tsp_int2;
            vsurrogate      : tgg_surrogate;
            ventrytype      : tsp_c2;
            vlinkage        : tsp_c2;
            vsegmentid      : tsp_c2;
            vderivedcnt     : tsp_int2;
            vderived_cols   : tak_derived_col_info;
      END;


      tak_viewkeyrecord  =  RECORD
            vkreclen        : tsp_int2;
            vkkeylen        : tsp_int2;
            vkvarcol_offset : tsp_int2;
            vkvarcol_cnt    : tsp_int2;
            vkkey           : tsp_c12;
            vksegmentid     : tsp_c2;
            vkjvkeylen      : tsp_int2;
            vkjvkey         : tsp_key;
      END;


      tak_col_stats = RECORD
            cst_dist_values : tsp_int4;
            cst_pages       : tsp_int4;
            cst_avg_lst_len : tsp_int4;
      END;


      tak_statisticsinfo = RECORD
            sti_reclen        : tsp_int2;
            sti_keylen        : tsp_int2;
            sti_varcol_offset : tsp_int2;
            sti_varcol_cnt    : tsp_int2;
            sti_tabid         : tgg_surrogate;
            sti_entrytype     : tsp_c2;
            sti_linkage       : tsp_c2;
            sti_segmentid     : tsp_c2;
            sti_col_cnt       : tsp_int2;
            sti_rows          : tsp_int4;
            sti_pages         : tsp_int4;
            sti_date          : tsp_int4;
            sti_time          : tsp_int4;
            sti_filler        : tsp_int2;
            sti_index         : tak_columnset;  (* 130 Bytes *)
            sti_col_stats     : ARRAY[1..MAX_COL_STATS_AK00] OF tak_col_stats;
      END;

      (* result structure catalog representation *)

      tak_resultrecord  =  RECORD   (* 3996 B *)
            breclen        : tsp_int2;
            bkeylen        : tsp_int2;
            bvarcol_offset : tsp_int2;
            bvarcol_cnt    : tsp_int2;
            bkey           : tsp_c12;
            bsegmentid     : tsp_c2;
            bfirstindex    : tsp_int2;
            blastindex     : tsp_int2;
            btablekind     : tgg_tablekind;
            blinkexist     : tgg_linkset;
            btreeid        : tgg00_FileId;
            bauthid        : tgg_surrogate;
            breccnt        : tsp_int2;    (* 77/78 *)
            bmaxcol        : tsp_int2;
            bmaxreclen     : tsp_int2;
            bkeycolcount   : tsp_int2;
            blenfixedcol   : tsp_int2;
            bvarcolcount   : tsp_int2;
            bcntdropcol    : tsp_int2;
            bstringcount   : tsp_int2;
            blongvarcolcnt : tsp00_Uint1;
            bcomment       : boolean;
            bavgrowlen     : tsp_int2;
            blastkeyind    : tsp_int2;
            bfirstcolind   : tsp_int2;           (* 100 *)
            bdatecreate    : tsp_int4;
            btimecreate    : tsp_int4;
            bupdstatdate   : tsp_int4;
            bupdstattime   : tsp_int4;           (* 116 *)
            bpages         : tsp_int4;
            brows          : tsp_int4;
            balterdate     : tsp_int4;
            baltertime     : tsp_int4;
            bsample        : tsp_int4;
            boutcolno      : tsp_int2;
            bunloaded      : boolean;
            bindexexist    : boolean;
            bv_level       : tsp00_Uint1;
            bv_tabcount    : tsp00_Uint1;
            bv_tablekind   : tgg_tablekind;
            bv_checkopt    : boolean;
            bv_qualexist   : boolean;
            bv_distinct    : tgg_distinct;
            bv_viewlist    : boolean;
            bdef_stamp     : boolean;
            bcorr_view     : boolean;
            bsqlmode       : tsp_sqlmode;
            bnamed_constr  : tsp_int2;
            bshow_kind     : tgg_show_kind;
            brecreate_view : boolean;
            bshowkind      : tgg_show_kind;
            battributes    : tak_table_attribute_set;
            btablen        : tsp_knl_identifier_ptr;
            bptr           : tak_basecolinfo_ptr;
            CASE integer OF
                1 :
                    (bextcolindex : tak_colindex);
                2 :
                    (bcolumn : tak_columnarr);
                END;
            (*ENDCASE*)


      tak_named_subtrans_info = RECORD
            str_name : tsp_knl_identifier;
            str_no   : tgg00_SubtransNo;
      END;


      tak_subtransrecord = RECORD
            str_reclen        : tsp_int2;
            str_keylen        : tsp_int2;
            str_varcol_offset : tsp_int2;
            str_varcol_cnt    : tsp_int2;
            str_key           : tgg_surrogate;
            str_etype         : tsp_c2;
            str_linkage       : tsp_c2;
            str_count         : tsp_int2;
            str_filler        : tsp_int2;
            str_subtrans      : ARRAY[ 1..cak_max_named_subtrans ] OF
                  tak_named_subtrans_info;
      END;


      tak_scoldescrecord  =  RECORD
            scd_reclen        : tsp_int2;
            scd_keylen        : tsp_int2;
            scd_varcol_offset : tsp_int2;
            scd_varcol_cnt    : tsp_int2;
            scd_colkey        : tgg_surrogate;
            scd_etype         : tsp_c2;
            scd_filler        : tsp_c2;   (* always 0 *)
            scd_stringfd      : tgg_string_filedescr;
            scd_lock          : tgg00_Lock;
            scd_key           : tsp_key;
      END;

      tak_changetype = (ch_to_ascii,
            ch_to_ebcdic,
            ch_date,
            ch_time_or_timestamp,
            ch_to_multibyte_set,
            ch_to_termchar_set,
            ch_to_longsupport,
            ch_to_warn2);

      tak_change_colinfo  =  RECORD
            ch_startpos : tsp_int2;
            ch_length   : char;
            ch_type     : SET OF tak_changetype;
      END;

      tak_column_change_arr = ARRAY [ 1..MAX_COL_PER_TAB_GG00 ] OF
            tak_change_colinfo;

      tak_changerecord  =  RECORD
            cr_colcount : tsp_int2;
            cr_was_opt  : boolean;
            cr_filler   : boolean;
            cr_columns  : tak_column_change_arr;
      END;


      tak_changeinfos = RECORD
            ci_rec_len       : tsp_int2;
            ci_key_len       : tsp_int2;
            ci_varcol_offset : tsp_int2;
            ci_varcol_cnt    : tsp_int2;
            ci_surrogate     : tgg_surrogate;
            ci_entrytype     : tsp_c2;
            ci_segno         : tsp_int2;
            ci_fullen        : tsp_int4;
            ci_colcount      : tsp_int2;
            ci_filler        : tsp_int2;
            ci_ch_columns    : tak_column_change_arr;
      END;


      tak_keyspec = RECORD
            ks_pos : tsp_int2;
            ks_len : tsp_int2
      END;


      tak_two_keyspecs = RECORD
            reckeyspec  : tak_keyspec;
            listkeyspec : tak_keyspec
      END;

      tak_res_keysbuf = PACKED ARRAY [ 1..mxak_res_keysbuf ] OF char;

      tak_resultkeyrecord  =  RECORD
            res_rec_len        : tsp_int2;
            res_key_len        : tsp_int2;
            res_varcol_offset  : tsp_int2;
            res_varcol_cnt     : tsp_int2;
            res_surrogate      : tgg_surrogate;
            res_entrytype      : tsp_c2;
            res_segno          : tsp_int2;
            res_fullen         : tsp_int4;
            res_startkeys      : tak_two_keyspecs;
            res_stopkeys       : tak_two_keyspecs;
            res_nextkeys       : tak_two_keyspecs;(* key > fetch next*)
            res_nextpos        : tsp_int4;
            res_prevkeys       : tak_two_keyspecs;(* key > fetch prev*)
            res_prevpos        : tsp_int4;
            res_updkey         : tak_keyspec;
            res_treeids        : tgg_two_tree_ids;
            res_restree        : tgg00_FileId;
            res_resultname     : tsp_knl_identifier;
            res_rowno          : tsp_int2;
            res_order          : boolean;
            res_build          : boolean;
            res_actres         : tsp_int4;  (* no. of results *)
            (*===*)
            res_reclen         : tsp_int2;  (* reclen incl. Update-Keys*)
            res_length         : tsp_int2;  (* length of output *)
            res_keylen         : tsp_int2;
            res_outcolno       : tsp_int2;
            res_resstate       : tak_sresstate;
            res_decresdel      : tak_decresstate;
            res_first_fetch    : boolean;
            res_eof            : boolean;
            res_useupdk        : tsp00_Uint1;
            res_updchar        : tsp_c1;  (* truncate Blank/h'00' *)
            res_for_update     : boolean;
            (*===*)
            res_updtabid       : tgg_surrogate;
            res_unlocktab      : tgg_surrogate;
            res_primkeylen     : tsp_int2;
            res_minkeylen      : tsp_int2;  (* for keys with > 1 column *)
            res_inv_only       : tsp_int2;
            res_strat_info     : tgg_ref_info_index;
            res_parsinfob      : tsp00_Uint1;
            (*===*)
            res_pars_dt_format : tgg_datetimeformat;
            res_pars_sqlmode   : tsp_sqlmode;
            res_upd_tabcnt     : tsp00_Uint1;
            res_qual_kind      : tgg_qual_kind;
            res_known_pages    : tsp_int4;
            res_searched_pages : tsp_int4;
            res_filler3        : tsp_int4;
            res_analyze_cmdid  : tgg_surrogate;
            CASE boolean OF
                true  :
                    (res_change  : tak_changerecord);
                false  :
                    (res_keysbuf : tak_res_keysbuf);
                END;
            (*ENDCASE*)


      tak_longinfo  =  RECORD   (* 24 *)
            CASE boolean OF
                true :
                    (licolname    : tak_oldidentifier;  (* long info columnname *)
                    lityplen_data : tsp_c2;    (* data type and length *)
                    lifrac_inoutl : tsp_c2;    (* data frac and in/outlength *)
                    libufpos      : tsp_int2); (* position in output buffer  *)
                false :
                    (lifiller1    : tsp_c20;
                    lilonglength  :     tsp_int2;
                    lifiller2     :     tsp_int2)
                END;
            (*ENDCASE*)
            (* length  =  mxak_linfoarr *)

      tak_all_column_infos  =  ARRAY [ 1..MAX_COL_PER_TAB_GG00 ] OF tak_longinfo;
      tak_longinfoarr  =  ARRAY [ 1..cak_maxlonginfo ] OF tak_longinfo;

      tak_longinforecord  =  RECORD       (* 4012 *)
            lireclen        : tsp_int2;
            likeylen        : tsp_int2;
            livarcol_offset : tsp_int2;
            livarcol_cnt    : tsp_int2;
            lisurrogate     : tgg_surrogate;
            lientrytype     : tsp_c2;
            lisegno         : tsp_int2;
            lisl            : tsp_int2;
            licount         : tsp_int2;
            liinfo          : tak_longinfoarr
      END;


      tak_shortinfo  =  RECORD   (* 8 *)
            CASE boolean OF
                true :
                    (simodeiotype : tsp_c2;    (* mode and in/out-type *)
                    sityplen_data : tsp_c2;    (* data type and length *)
                    sifrac_inoutl : tsp_c2;    (* data frac and in/outlength *)
                    sibufpos      : tsp_int2); (* position in output buffer  *)
                false :
                    (sifiller1    : tsp_int4;
                    silonglength  : tsp_int2;
                    sifiller2     : tsp_int2)
                END;
            (*ENDCASE*)

      tak_shortinfoarr  =  ARRAY [ 1..cak_maxshortinfo ] OF tak_shortinfo;
      tak_paraminfoarr  =  ARRAY [ 1..cak_maxparams    ] OF tsp1_param_info;
      tak_parinfarr_ptr = ^tak_paraminfoarr;

      tak_shortinforecord  =  RECORD   (* 32KB *)
            sireclen        : tsp_int2;
            sikeylen        : tsp_int2;
            sivarcol_offset : tsp_int2;
            sivarcol_cnt    : tsp_int2;
            sisurrogate     : tgg_surrogate;
            sientrytype     : tsp_c2;
            sisegno         : tsp_int2;
            sifullen        : tsp_int4;
            sisl            : tsp_int2;
            sicount         : tsp_int2;
            siinfo          : tak_paraminfoarr
      END;

      tak_sysbufferaddress  = ^tak_systembuffer;
      tak_colnamesbuf = PACKED ARRAY [ 1 .. mxak_colnamesbuf ] OF char;

      tak_columnnamesrecord = RECORD
            cnreclen        : tsp_int2;
            cnkeylen        : tsp_int2;
            cnvarcol_offset : tsp_int2;
            cnvarcol_cnt    : tsp_int2;
            cnsurrogate     : tgg_surrogate;
            cnentrytype     : tsp_c2;
            cnsegno         : tsp_int2;
            cnbit64fill     : tsp_int4;
            CASE boolean OF
                true :
                    (cnfullen  : tsp_int4;
                    cncount   : tsp_int2);
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
            fp_syskey,
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
            fp_val_from_res_buffer);
      (**)
      tak_fp_kind_type_ptr = ^tak_fp_kind_type;
      (**)
      tak_fcolprop  =  (fp_opt, fp_like, fp_like1, fp_like2,
            fp_serial, fp_default, fp_escape);
      tak_fcolpropset  =  SET OF tak_fcolprop;

      tak_len_and_frac = RECORD
            CASE boolean OF
                true :
                    (lf_datalen : tsp_int2);
                false :
                    (lf_fixed_datalen : tsp00_Uint1;
                    lf_frac           : tsp00_Uint1)
                END;
            (*ENDCASE*)


      tak_field_pars  =  PACKED RECORD
            fp_kind         : tak_fp_kind_type;
            fp_etype        : tgg_stack_entry_type;
            fp_datatyp      : tsp_data_type;
            fp_colset       : tak_fcolpropset;
            CASE tak_fp_kind_type OF
                fp_catalog_column,
                fp_val_varcol_with_len,
                fp_val_all_with_len :
                    (fp_frompos_v1    : tsp_int4;
                    fp_movebefore_v1  : tsp_int2;
                    fp_rangeextcolno  : tsp_int2;
                    fp_len_and_frac_v1: tak_len_and_frac;
                    fp_inoutlen_v1    : tsp_int2);
                fp_val_all_without_l,
                fp_selord_val_no_len :
                    (fp_frompos_v2    : tsp_int4;
                    fp_movebefore_v2  : tsp_int2;
                    fp_stackpos       : tsp_int2;
                    fp_len_and_frac_v2: tak_len_and_frac;
                    fp_inoutlen_v2    : tsp_int2);
                fp_lock_val :
                    (fp_frompos_v3    : tsp_int4;
                    fp_movebefore_v3  : tsp_int2;
                    fp_fill_31        : tsp_int2;
                    fp_len_and_frac_v3: tak_len_and_frac;
                    fp_inoutlen_v3    : tsp_int2);
                fp_last_lock,
                fp_reclen,
                fp_syskey,
                fp_temp_table_key :
                    (fp_fill_41       : tsp_int4;
                    fp_movebefore_v4  : tsp_int2;
                    fp_fill_42        : tsp_int2;
                    fp_fill_43        : tsp_int4);
                fp_current_of_key,
                fp_viewkey_to_mess_buf :
                    (fp_curr_keylen   : tsp_int2;
                    fp_startpos       : tsp_int2;
                    fp_movebefore_v5  : tsp_int2;
                    fp_fill_51        : tsp_int2;
                    fp_pos_no         : tsp_int4);
                fp_val_from_res_buffer :
                    (fp_res_buf_index : tsp_int2;
                    fp_glob_fp_kind   : tak_fp_kind_type;
                    fp_fill_61        : boolean;
                    fp_movebefore_v6  : tsp_int2;
                    fp_rangeextcolno_v6: tsp_int2;
                    fp_len_and_frac_v6: tak_len_and_frac;
                    fp_inoutlen_v6    : tsp_int2);
                fp_fill_part2_to_len,
                fp_use_old_values_up_to_len :
                    (fp_fill_71       : tsp_int4;
                    fp_fill_72        : tsp_int4;
                    fp_fill_73        : tsp_int2;
                    fp_len            : tsp_int2);
                fp_ordered_select_key :
                    (fp_fill_81       : tsp_int4;
                    fp_movebefore_v8  : tsp_int2;
                    fp_keystack       : tsp_int2;
                    fp_to_stack       : tsp_int2;
                    fp_key_low_handl  : tsp_int2);
                fp_escape_val :
                    (fp_fill_101       : tsp_int4;
                    fp_fill_102        : tsp_int4;
                    fp_fill_103        : tsp_int2;
                    fp_escape_char    : tsp_c2);
                fp_pseudo_ins_select,
                fp_verify_constraint,
                fp_verify_view :
                    (fp_fill_111       : tsp_int4;
                    fp_movebeforev11   : tsp_int2;
                    fp_fill_112        : tsp_int2;
                    fp_reccolno        : tsp_int2;
                    fp_fill_113        : tsp_int2);
                fp_result_name :
                    (fp_fromposv14     : tsp_int4;
                    fp_fill_141        : tsp_int4;
                    fp_datalenv14      : tsp_int2;
                    fp_dataiolenv14    : tsp_int2);
                fp_verify_not_null :
                    (fp_stackentry     : tgg_stack_entry;
                    fp_fill_151        : tsp_int4);
                (* PTS 1111577 E.Z. *)
                fp_get_name_value :
                    (fp_frompos_v15    : tsp_int4;
                    fp_movebefore_v15  : tsp_int2;
                    fp_sc_symbol       : tak_sc_symbol;
                    fp_objecttype      : tsp00_C1;
                    fp_len_and_frac_v15: tak_len_and_frac;
                    fp_inoutlen_v15    : tsp_int2);
                END;
            (*ENDCASE*)


      tak_ak_all_locks  =  RECORD
            all_lock_count : tsp_int2;
            all_lock_fill  : tsp_int2;
            all_locks      : ARRAY [ 1..MAX_QUALBUF_LOCKS_GG00 ] OF tgg00_Lock;
      END;

      tak_parser_fields  =  ARRAY [ 1..cak_maxparsfields ] OF
            tak_field_pars;

      tak_selfref_link_info = RECORD
            sf_index  : tsp_int2;
            sf_parsno : tsp_int2;
            sf_use_id : tsp_int2;
            sf_fill   : tsp_int2;    (* unused *)
      END;


      tak_parsinforecord  =  RECORD    (* 3996 *)
            CASE integer OF
                1 :
                    (p_reclen       : tsp_int2;
                    p_keylen        : tsp_int2;
                    p_varcol_offset : tsp_int2;
                    p_varcol_cnt    : tsp_int2;
                    p_surrogate     : tgg_surrogate;
                    p_entrytype     : tsp_c2;
                    p_segno         : tsp_int2;
                    p_fullen        : tsp_int4;
                    p_filler        : tsp_int4;
                    p_buf           : tak_parsbuffer);
                2 :
                    (p1reclen          : tsp_int2;
                    p1keylen           : tsp_int2;
                    p1varcol_offset    : tsp_int2;
                    p1varcol_cnt       : tsp_int2;
                    p1surrogate        : tgg_surrogate;
                    p1entrytype        : tsp_c2;
                    p1segno            : tsp_int2;
                    p1_fullen          : tsp_int4;
                    p1_filler          : tsp_int4;
                    p_treeid           : tgg00_FileId;
                    p_pagecnt          : tsp_int4;
                    p_resn             : tsp_knl_identifier;
                    p_modul_name       : tsp_knl_identifier;
                    p_searchname       : tsp_knl_identifier;
                    p_mtyp             : tgg_message_type;
                    p_mtyp2            : tgg_message2_type;
                    p_select           : boolean;
                    p_single           : boolean;  (* 192 *)
                    p_tabid            : tgg_surrogate;
                    p_int_res_cnt      : tsp_int2;
                    p_joinviewtabcnt   : tsp_int2;
                    p_p_no             : tsp_int2;
                    p_subcntlevel      : tsp_int2;
                    p_posno            : tsp_int4;
                    p_statement_kind   : tgg_ref_info_index;
                    p_changeinfos_exist: boolean;
                    p_const_param_expr : boolean;
                    p_precomp_info_byte: tsp00_Uint1;
                    (*====*)
                    p_reuse            : boolean;
                    p_prepare          : boolean;
                    p_function_code    : tsp_int2;
                    p_sqlmode          : tsp_sqlmode;
                    p_filler0          : boolean;
                    p_handling_set     : tgg00_HandlingSet;
                    p_session_isolevel : tsp00_Uint1;
                    p_use_sess_isolevel: boolean;
                    p_resreclen        : tsp_int2;
                    p_resinfolen       : tsp_int2;
                    p_j_v_keylen       : tsp_int2;
                    p_longliteral      : boolean;
                    p_parsing_again    : boolean; (* pts 1104055 *)
                    p_filler1          : tsp_c2;
                    p_max_infos        : tsp_int2;
                    p_cnt_infos        : tsp_int2;  (* mxak_pars_header
                          + cak_sysbufferoffset *)
                    p_pars_infos       : tak_parser_fields);
                3 :
                    (p2reclen        : tsp_int2;
                    p2keylen         : tsp_int2;
                    p2varcol_offset  : tsp_int2;
                    p2varcol_cnt     : tsp_int2;
                    p2surrogate      : tgg_surrogate;
                    p2entrytype      : tsp_c2;
                    p2segno          : tsp_int2;
                    p2fullen         : tsp_int4;
                    p2filler         : tsp_int4;
                    p_use_fileid     : tsp_int2;
                    p_sa_level       : tsp_int2;
                    p_lo_level       : tsp_int2);
                4 :
                    (p3reclen       : tsp_int2;
                    p3keylen        : tsp_int2;
                    p3varcol_offset : tsp_int2;
                    p3varcol_cnt    : tsp_int2;
                    p3surrogate     : tgg_surrogate;
                    p3entrytype     : tsp_c2;
                    p3segno         : tsp_int2;
                    p_tl_filler1    : PACKED ARRAY
                          [ 1..mxak_pars_header ] OF char;
                    p_temp_lock_atch : tsp_int4;
                    p_temp_lock      : tak_ak_all_locks);
                5 :
                    (p_sysbufferoffset : ARRAY[1..cak_sysbufferoffset] OF char;
                    p_pars_header      : ARRAY[1..mxak_pars_header] OF char;
                    p_short_info       : tak_paraminfoarr);
                7 :
                    (p7reclen          : tsp_int2;
                    p7keylen           : tsp_int2;
                    p7varcol_offset    : tsp_int2;
                    p7varcol_cnt       : tsp_int2;
                    p7surrogate        : tgg_surrogate;
                    p7entrytype        : tsp_c2;
                    p7segno            : tsp_int2;
                    p_tl_filler7  : PACKED ARRAY
                          [ 1..mxak_pars_header ] OF char;
                    p_handling         : tgg00_HandlingSet;
                    p_fetch_object     : boolean;
                    p7fill             : boolean);
                END;
            (*ENDCASE*)

      tak_stored_cmd_buf = PACKED ARRAY [ 1..mxak_stored_cmd_buf ] OF char;

      tak_stored_cmd_record = RECORD
            scr_reclen         : tsp_int2;
            scr_keylen         : tsp_int2;
            scr_varcol_offset  : tsp_int2;
            scr_varcol_cnt     : tsp_int2;
            scr_surrogate      : tgg_surrogate;
            scr_entrytype      : tsp_c2;
            scr_segno          : tsp_int2;
            scr_fullen         : tsp_int4;
            scr_movelen        : tsp_int2;
            scr_ddl            : tak_ddl_descriptor;
            scr_sql_mode       : tsp_sqlmode;
            scr_buf            : tak_stored_cmd_buf;
      END;

      tak_union_kind  =  (uis_none, uis_union, uis_except, uis_intersect);

      tak_union_entry  =  RECORD (* 16 *)
            uparsid     : tak_parskey;
            ufirstsel   : tsp_int2;
            usecondsel  : tsp_int2;
            ufiller     : tsp_int2;
            uis_kind    : tak_union_kind;
            uis_all     : boolean;
      END;

      tak_union_array  =  ARRAY [ 1..cak_max_union ] OF tak_union_entry;

      tak_unionrecord  =  RECORD  (* 4020 *)
            usyskreclen        : tsp_int2;
            usyskkeylen        : tsp_int2;
            usyskvarcol_offset : tsp_int2;
            usyskvarcol_cnt    : tsp_int2;
            usysksurrogate     : tgg_surrogate;
            usyskentrytype     : tsp_c2;
            usysksegno         : tsp_int2;
            uselect_cnt        : tsp_int2;
            ucolpos            : tsp_int2;
            ures_cnt           : tsp_int4;
            ures_tree          : tgg00_FileId;
            ures_name          : tsp_knl_identifier;
            umodul_name        : tsp_knl_identifier;
            ukeylen            : tsp_int2;
            urowno             : tsp_int2;
            usubcount          : tsp_int2;
            ucntpar            : tsp_int2;
            uinsertselect      : boolean;
            uappend_all        : boolean;
            ufiller            : tsp_int2;
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
            cnextpars    : tsp_int2;
            centryinfo   : tak_complex_infoset;
            cqual_jv_upd : tak_jv_upd_description;
      END;

      tak_complex_array  =  ARRAY [ 1..cak_maxcomplex ] OF tak_complex_entry;

      tak_complexrecord  =  RECORD
            compreclen        : tsp_int2;
            compkeylen        : tsp_int2;
            compvarcol_offset : tsp_int2;
            compvarcol_cnt    : tsp_int2;
            compsurrogate     : tgg_surrogate;
            compentrytype     : tsp_c2;
            compsegno         : tsp_int2;
            compcnt           : tsp_int2;
            compmax_sel_cnt   : tsp_int2;
            comptree          : tgg00_FileId;
            comparr           : tak_complex_array;
      END;

      tak_one_table_specialname =  SET OF
            (oresfile,
            oissynonym,
            oisreference,
            oisjoinview,
            oispartjoinview,
            oisshowview,
            oisreplicated);

      tak_messblock_record = RECORD
            mbr_reclen        : tsp_int2;
            mbr_keylen        : tsp_int2;
            mbr_varcol_offset : tsp_int2;
            mbr_varcol_cnt    : tsp_int2;
            mbr_surrogate     : tgg_surrogate;
            mbr_entrytype     : tsp_c2;
            mbr_linkage       : tsp_c2;
            mbr_segment_id    : tsp_c2;
            mbr_stack_len     : tsp_int4;
            mbr_filler1       : tsp_c2;
            mbr_mess_block    : tgg_mess_block;
      END;


      tak_soc_record  =  RECORD
            socoldtabno    : tsp00_Uint1;
            socselecttabno : tsp00_Uint1;
            socfields      : tsp00_Uint1;
            socfill        : tsp00_Uint1
      END;

      tak_cmd_part_type = (cpt_unknown,
            cpt_in_select_list,
            cpt_in_where_clause,
            cpt_in_having_clause);

      tak_one_corcolumn  =  RECORD (* 12 Bytes, mxak_one_corcolumn *)
            cfieldno : tsp_int2;
            cnode    : tsp_int2;
            clevel   : tak_corlevel;
            csetnode : tsp_int2;
            ccntcol  : tsp00_Uint1;
            ctabno   : tsp00_Uint1;
            ccmd_part: tak_cmd_part_type;
            cfiller1 : tsp00_Uint1;
            cfiller2 : tsp_int2;
      END;

      tak_all_corcolumns  =  ARRAY [ 1..cak_maxcorcolumns ] OF
            tak_one_corcolumn;

      tak_correcord  =  RECORD
            tc_reclen        : tsp_int2;
            tc_keylen        : tsp_int2;
            tc_varcol_offset : tsp_int2;
            tc_varcol_cnt    : tsp_int2;
            tc_surrogate     : tgg_surrogate;
            tc_entrytype     : tsp_c2;
            tc_segno         : tsp_int2;
            tc_no_of_tables  : tsp_int2;
            tc_no_of_columns : tsp_int2;
            tc_starttable    : tsp_int2;
            tc_oldstarttable : tsp_int2;
            tc_tables        : ARRAY [ 1..16 ] OF tak_soc_record;
            tc_columns       : tak_all_corcolumns
      END;

      (* PTS 1111575 E.Z. *)

      tak_linkposlen  = RECORD
            lpos : tsp_int2;
            llen : tsp_int2;
      END;

      tak_linkposinfo  =  ARRAY[ 1..cak_maxlinkpos ] OF tak_linkposlen;

      tak_linkinforecord  =  RECORD
            lreclen        : tsp_int2;
            lvarcol_offset : tsp_int2;
            lvarcol_cnt    : tsp_int2;
            lkeylen        : tsp_int2;
            lsurrogate     : tgg_surrogate;
            lentrytype     : tsp_c2;
            lseqno         : tsp_int2;
            lfullen        : tsp_int4;
            lmess_type     : tgg_message_type;
            lprim_check    : boolean;
            lunique_cnt    : tsp_int2;
            lupd_col_info  : tak_columnset;
            lnot_used      : tak_charset;
            lpos_info      : tak_linkposinfo;
      END;

      (*========= cache data types =============*)
      tak_directory_state  =  (d_fix, d_release);
      tak_sysdir_state  =  SET OF (dfix, dlocked, dversrec,
            dadd_rec, dwritten, dmodified,ddisplace (* PTS 1110796 *),dno_meaning);
      tak_cache_dir_ptr     = ^tak_cache_dir_entry;
      tak_moveobj_ptr       = ^tsp_moveobj;
      tak_cache_dir_e_ptr   = ^tak_cache_dir_entry;

      tak_cache_dir_pointer = RECORD
            CASE integer OF
                1 :
                    (cdir_ptr    : tak_cache_dir_e_ptr);
                2 :
                    (buf_ptr     : tsp_bufaddr);
                3 :
                    (moveobj_ptr : tak_moveobj_ptr);
                4 :
                    (sys_ptr     : tak_sysbufferaddress);
                5 :
                    (i           : tsp_int4);
                END;
            (*ENDCASE*)


      tak_sysdirectory  =  RECORD   (* 1220 *)
            ci_cache_dir_header  : tak_cache_dir_pointer;
            ci_cache_dir_trailor : tak_cache_dir_pointer;
            ci_cache_p           : tsp_bufaddr;
            ci_tmp_read_cnt      : tsp_int4;
            ci_displaceCnt       : tsp_int4; (* PTS 1110796 *)
            ci_tmp_dislodged     : boolean;
            ci_filler1           : boolean;
            ci_filler2           : tsp_int2;
            ci_tmp_read_found    : tsp_int4;
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
            psynid    : tgg_surrogate;
            psynfound : boolean;
            psystable : boolean;
            pcount    : tsp_int2;
      END;


      tak_alterdropcol  =  RECORD
            dextno  : tsp_int2;
            dcolptr : tak00_colinfo_ptr;
      END;


      tak_dropcol  =  RECORD
            dcount : tsp_int4;
            dtabno : tsp_int2;
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
            v_drop_serverdb,
            v_rename_user);

      tak_drop_serverdb_record  =  RECORD
            ds_dbname              : tsp_dbname;
            ds_site                : tgg_serverdb_no;
            ds_hard_drop           : boolean;
            ds_filler1             : boolean;
            ds_filler2             : tsp_int2;
            ds_node_id             : tsp_nodeid;
            ds_local_user_file     : tgg00_FileId;
            ds_remote_user_file    : tgg00_FileId;
            ds_remote_local_tables : tgg00_FileId;
            ds_dropped_tables      : tgg00_FileId;
            ds_views               : tgg00_FileId;
            ds_analyzed_views      : tgg00_FileId;
            ds_global_tables       : tgg00_FileId
      END;

      tak_drop_db_ptr  = ^tak_drop_serverdb_record;

      tak_viewscan_par  =  RECORD
            vsc_baseauthname  : tsp_knl_identifier;
            vsc_basetablen    : tsp_knl_identifier;
            vsc_filevers      : tgg91_FileVersion;
            vsc_rp_rt_handling: boolean;
            vsc_init_type     : tak_viewscantype;
            vsc_base_tabid    : tgg_surrogate;
            vsc_key           : tgg_sysinfokey;
            vsc_edi           : boolean;
            vsc_restrict      : boolean;
            vsc_ti            : tsp_int2;
            vsc_tabno         : tsp_int2;
            vsc_type          : tak_viewscantype;
            vsc_delete_view   : boolean;
            vsc_mult_index    : boolean;
            vsc_modified      : boolean;
            vsc_from_tableid  : tgg_surrogate;
            vsc_col_list      : boolean;
            vsc_col_dropped   : boolean;
            vsc_usagedef      : tak_usagedef;
            vsc_start_tabid   : tgg_surrogate;
            vsc_vdesc_modified: ARRAY[ 1..8 ] OF boolean;
            vsc_rows          : tsp_int4;
            vsc_tabno_set     : tak_joinset;
            vsc_vdesc_cnt     : tsp_int2;
            (* if header above is changed, don't forget *)
            (* tak_save_viewscan_par !!                 *)
            CASE tak_viewscantype OF
                v_drop_serverdb :
                    (vsc_ds_rec : tak_drop_serverdb_record);
                v_statistics    :
                    (vsc_base_page_cnt : tsp_int4;
                    vsc_base_rec_cnt   : tsp_int4;
                    vsc_st_filler1     : tsp_int4;
                    vsc_st_filler2     : tsp_int4;
                    vsc_page_cnt_info  : tak_upd_stat_info);
                v_alter_drop, v_revoke_priv, v_index :
                    (vsc_drop_set       : tak_columnset;
                    vsc_valid_colset    : tak_columnset;
                    vsc_dr_col          : tak_dropcol;
                    vsc_valid_cols      : tak_colindex;
                    vsc_valid_cnt       : tsp_int4;
                    vsc_extcol_set      : tak_columnset;
                    vsc_rev_tableid     : tgg_surrogate;
                    vsc_start_revoke    : boolean;
                    vsc_userkind        : tak_usertyp;
                    vsc_page_cnt        : tsp_int4;
                    vsc_rec_cnt         : tsp_int4;
                    vsc_prim_cnt        : tsp_int4;
                    vsc_null_cnt        : tsp_int4;
                    vsc_single_index    : boolean;
                    vsc_mode            : tak_index_cmd_mode;
                    vsc_index_col_count : tsp_int2;
                    vsc_col_info        : tak_index_col_info;
                    vsc_indexname       : tsp_knl_identifier;
                    vsc_unique          : boolean;
                    vsc_ignore_error    : boolean;
                    vsc_keylen          : tsp_int2;
                    vsc_col_ptr         : tak00_colinfo_ptr;
                    vsc_op_order        : tgg_stack_op_type;
                    vsc_filler1         : boolean;
                    vsc_filler2         : boolean;
                    vsc_dropped_treeid  : tgg00_FileId);
                v_alter_column :
                    (vsc_drop_col: tak_dropcol;
                    vsc_colinfo  : tak00_columninfo;
                    vsc_coladd   : boolean);
                v_rename_column, v_rename_table :
                    (vsc_rename_set  : tak_columnset;
                    vsc_info_cnt     : tsp_int4;
                    vsc_pos_info     : tak_colinteger;
                    vsc_newcolname   : tsp_knl_identifier;
                    vsc_newcol_len   : tsp_int2;
                    vsc_oldcolname   : tsp_knl_identifier;
                    vsc_col_extcolno : tsp_int2;
                    vsc_colptr       : tak00_colinfo_ptr;
                    vsc_renamed_cols : tsp_int4;
                    vsc_renamed      : boolean;
                    vsc_col_in_qual  : boolean;
                    vsc_quotes       : boolean;
                    vsc_rename_filler: boolean;
                    vsc_new_authname : tsp_knl_identifier;
                    vsc_old_len      : tsp_int2;
                    vsc_new_tablen   : tsp_knl_identifier;
                    vsc_new_len      : tsp_int2;
                    vsc_old_tablen   : tsp_knl_identifier;
                    vsc_ren_tabid    : tgg_surrogate);
                v_rename_user :
                    (vsc_old_user_name : tsp_knl_identifier;
                    vsc_new_user_name : tsp_knl_identifier);
                v_save_scheme, v_intern_save_scheme :
                    (vsc_tree_id      : tgg00_FileId;
                    vsc_buf           : tak_sysbufferaddress;
                    vsc_into_authname : tsp_knl_identifier;
                    vsc_into_tablen   : tsp_knl_identifier;
                    vsc_save_into     : boolean;
                    vsc_tablekind     : tgg_tablekind;
                    vsc_into_treeid   : tgg00_FileId;
                    vsc_cmd_cnt       : tsp_int4;
                    vsc_first_save    : boolean;
                    vsc_last_save     : boolean;
                    vsc_user_save     : boolean;
                    vsc_all_save      : boolean);
                END;
            (*ENDCASE*)


      tak_save_viewscan_par  =  RECORD
            vsc_baseauthname  : tsp_knl_identifier;
            vsc_basetablen    : tsp_knl_identifier;
            vsc_filevers      : tgg91_FileVersion;
            vsc_rp_rt_handling: boolean;
            vsc_init_type     : tak_viewscantype;
            vsc_base_tabid    : tgg_surrogate;
            vsc_key           : tgg_sysinfokey;
            vsc_edi           : boolean;
            vsc_free          : boolean;     (* free *)
            vsc_ti            : tsp_int2;
            vsc_tabno         : tsp_int2;
            vsc_type          : tak_viewscantype;
            vsc_delete_view   : boolean;
            vsc_mult_index    : boolean;
            vsc_modified      : boolean;
            vsc_from_tableid  : tgg_surrogate;
            vsc_col_list      : boolean;
            vsc_col_dropped   : boolean;
            vsc_usagedef      : tak_usagedef;
            vsc_start_tabid   : tgg_surrogate;
            vsc_vdesc_modified: ARRAY[ 1..8 ] OF boolean;
            vsc_rows          : tsp_int4;
            vsc_tabno_set     : tak_joinset;
            vsc_vdesc_cnt     : tsp_int2;
            (* the part above must agree with the prefix of *)
            (* tak_viewscan_parameter !!                    *)
            vsc_tree_id       : tgg00_FileId;
            vsc_buf           : tak_sysbufferaddress;
            vsc_into_authname : tsp_knl_identifier;
            vsc_into_tablen   : tsp_knl_identifier;
            vsc_save_into     : boolean;
            vsc_tablekind     : tgg_tablekind;
            vsc_into_treeid   : tgg00_FileId;
            vsc_cmd_cnt       : tsp_int4;
            vsc_first_save    : boolean;
            vsc_last_save     : boolean;
            vsc_user_save     : boolean;
            vsc_all_save      : boolean;
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
            tri_fill1        : tsp00_Uint1;
            tri_replicated   : tgg_replicated;
            tri_obj_seq      : tsp_c6;
            tri_fill2        : tsp_int2;
      END;

      tak_rollback_info = PACKED SET OF (trans_rollback, new_trans);

      tak_dbproc_entry = RECORD
            dbp_sysdba  : tsp_knl_identifier;
            dbp_tree    : tgg00_FileId;
      END;

      tak_dbproc_array = ARRAY[ 1..cak_dbproccache ] OF tak_dbproc_entry;

      tak_dbproc_cache = RECORD
            dbc_cnt   : tsp_int2;
            dbc_fil   : tsp_int2;
            dbc_ptr   : tak_sysbufferaddress;
            dbc_cache : tak_dbproc_array;
      END;

      tak_query_type = (type_n, type_j, type_an,
            type_aj, type_else, type_no);

      tak_updstat_rec = RECORD
            us_tabid      : tgg_surrogate;
            us_date       : tsp_int4;
            us_time       : tsp_int4;
            us_known_pages: tsp_int4;
            us_found_pages: tsp_int4;
      END;


      tak_index_scan_record = RECORD
            isr_buf        : tak_sysbufferaddress;
            isr_index      : integer;
            isr_sysk       : tgg_sysinfokey;
      END;


      tak_optimize_info = RECORD
            o_do_optimize : boolean;
            o_query_type  : tak_query_type;
            o_mess_type   : tsp_int2;
            o_ex_kind     : tak_execution_kind;
            o_optimized   : boolean;
            o_filler1     : tsp_int2;
            o_cmd_tree    : tgg00_FileId;
            o_filler3     : tsp_c8;
      END;


      tak_nls_param_rec = RECORD
            currency       : tsp_c4;
            date_format    : tsp_c50;
            date_language  : tsp_c3;
            numeric_chars  : tsp_c2;
            df_length      : tsp_int2;
            dl_found       : boolean;
            dl_is_german   : boolean;
            dl_language_no : tsp00_Uint1;
            nls_binary     : boolean;
            nls_sort       : tsp_knl_identifier;
      END;

      tak_size_ptr = ^tak_size_info;

      tak_size_info = RECORD
            csz_next       : tak_size_ptr;
            csz_session    : tgg91_SessionNo;
            csz_ak_cache   : tsp_int4;
            (* PTS 1115978 E.Z. *)
            csz_fill1      : boolean;
            csz_tracelevel : tsp00_Uint1;
            csz_fill2      : tsp_int2;
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
            at_xci);
      tak_explain_kind = (ex_default, ex_join, ex_sequence);

      tak_cmd_stack = RECORD
            cst_top : tsp_int4;
            cst_stack : ARRAY[1..cak_max_cmd_stack] OF tsp_moveobj_ptr;
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
                    (d : tsp_longreal);
                false :
                    (* (c : tsp_c32);  1106969 FF *)
                    (c : tsp_c64);  (* 1106969 FF *)
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
            ai_alloc_sys         : tak_AllocatorMonitorInfo;
            ai_alloc_usr         : tak_AllocatorMonitorInfo;
      END;


      tak_all_command_glob  =  RECORD   (* acv  : variable, ca 23000*)
            a_in_packet             : tsp1_packet_ptr;
            a_out_packet            : tsp1_packet_ptr;
            a_cmd_segm              : tsp1_segment_ptr;
            a_cmd_part              : tsp1_part_ptr;
            a_data_part             : tsp1_part_ptr;
            a_abap_part             : tsp1_part_ptr;
            a_rescount_part         : tsp1_part_ptr;
            a_resname_part          : tsp1_part_ptr;
            a_data_ptr              : tsp_moveobj_ptr;
            a_return_segm           : tsp1_segment_ptr;
            a_curr_retpart          : tsp1_part_ptr;
            a_ap_tree               : tak_ap_max_tree_ptr;
            a_drop_serverdb_ptr     : tak_drop_db_ptr;
            a_dbp_packet_ptr        : tsp1_packet_ptr;
            a_input_data_pos        : tsp_int4;
            a_output_data_pos       : tsp_int4;
            a_mb_data_addr          : tgg_datapart_ptr;
            a_mb_strat_addr         : tsp00_MoveObjPtr;
            a_cmd_globstate         : tgg00_HandlingSet;
            a_code_to_compare       : tsp00_Uint1;
            a_info_output           : boolean;
            a_ex_kind               : tak_execution_kind;
            a_precomp_info_byte     : tsp00_Uint1;
            a_scv_index             : tsp_int2;
            a_scv                   : tak_scanner_glob;
            a_in_internal_subtr     : boolean;
            a_init_ddl              : tak_ddl_descriptor;
            a_count_variab          : tsp_int2;
            a_command_kind          : tak_commandkind;
            a_is_ddl                : tak_ddl_descriptor;
            a_stored_proc_call      : boolean;
            a_wait_for_checkpoint   : boolean; (* PTS 1107004 *)
            a_outer_join            : boolean;
            a_intern_explain        : boolean;
            a_pars_explain          : boolean;
            a_internal_sql          : tak_internal_sql_kind;
            a_from_select           : boolean;
            a_user_defined_error    : boolean;
            a_allow_functions       : tak_function;
            a_rowno_allowed         : boolean;
            a_rowno_found           : boolean;
            a_special_expr          : boolean;
            a_oneval_subq_allowed   : boolean;
            a_column_factor         : boolean;
            a_join_predicate        : boolean;
            a_try_again             : boolean;
            a_select_node           : tsp_int2;
            a_curr_res_id           : tsp_int4;
            a_complex_key           : tgg_surrogate;
            a_max_intern_select     : tsp_int2;
            a_intern_select_cnt     : tsp_int2;
            a_shortinfo_key         : tgg_surrogate;
            a_union_cnt             : tsp_int4;
            a_union_key             : tak_parskey;
            a_union_kind            : tsp00_Uint1;
            a_shared_result         : boolean;
            a_no_local_call         : boolean;
            a_costcheck             : boolean;
            a_costsum               : tsp_int4;
            a_isolation_info        : tak_isolation_info;
            a_corr_select           : boolean;
            a_error_tableid         : tgg_surrogate;
            a_errpos_offset         : tsp_int4; (* joinview *)
            a_parameter_offset      : tsp_int2;
            a_qualified_jv_upd      : tak_jv_upd_description;
            a_rest_equal_second     : boolean; (* execute correlation*)
            a_part_rollback         : boolean;
            a_rollback_info         : tak_rollback_info;
            a_timestamp             : tsp_timestamp;
            a_show_last_key         : tsp_int4;
            a_first_parskey         : char;
            a_first_parsid          : char;
            a_leftpar_cnt           : tsp_int2;
            a_rightpar_cnt          : tsp_int2;
            a_tempinfo_key          : tgg_surrogate;
            a_viewname              : tsp_knl_identifier;
            a_fromsel_n             : tsp_int2;
            a_may_be_direct_sel     : boolean;
            a_date_time_used        : boolean;
            a_compact_varpart       : boolean;
            a_max_codewidth         : tsp00_Uint1;
            a_session_max_codewidth : tsp00_Uint1;
            a_ak_b_err              : tgg_basis_error;
            a_result_name           : tsp_knl_identifier;
            a_mess_code             : tsp_code_type;
            a_basic_code            : tsp_code_type;
            a_session_mess_code     : tsp_code_type;
            a_corr_key              : tak_parskey;
            a_curr_ex_parskey       : tak_parskey;
            a_init_ex_kind          : tak_execution_kind;
            a_in_ddl_trigger        : boolean;
            a_iso_level             : tsp00_Uint1;
            a_for_fetch_only        : boolean;
            a_in_select_list        : boolean;
            a_in_installation       : boolean;
            a_parsing_again         : boolean;
            a_statement_kind        : tgg_ref_info_index;
            a_modul_name            : tsp_knl_identifier;
            a_parameter_spec        : tak_param_list;
            a_mb_data_size          : tsp_int4;
            a_mb_strat_size         : tsp_int4;
            a_ap_max_node           : tsp_int2;
            a_visual_basic_area     : tsp_bufaddr;
            a_dynamic_sql           : boolean;
            (*----------------------------------------------------*)
            a_comp_type             : tak_appl_type;
            a_explain_view          : boolean;
            a_explain_kind          : tak_explain_kind;
            a_comp_vers             : tsp_c5;
            a_degree                : tsp_c3;
            a_cpart_type            : tak_cmd_part_type;
            (* new with /u2/611 *)
            a_retpart_locked        : boolean;
            a_no_of_likes           : tsp_int2;
            a_maxnode               : tsp_int4;
            a_data_length           : tsp_int4;
            a_resultnum             : tsp_resnum;
            a_trunc_identifier      : boolean;
            a_mb_qual_addr          : tgg_qual_buf_ptr;
            a_mb_qual_size          : tsp_int4;
            a_stack_size            : tsp_int4;
            a_work_st_addr          : tgg_stack_list_ptr;
            a_work_buf_addr         : tsp_moveobj_ptr;
            a_work_buf_size         : tsp_int4;
            a_work_st_max           : tsp_int2;
            a_not_swap_f3           : PACKED ARRAY [1..10] OF char;
            a_mblock                : tgg_mess_block;
            (*----------------------------------------------------*)
            a_restarted             : boolean;
            a_is_connected          : boolean;
            a_in_session            : boolean;
            a_init_user_id          : tgg_surrogate;
            a_connected_name        : tsp_knl_identifier;
            a_curr_user_name        : tsp_knl_identifier;
            a_curr_user_id          : tgg_surrogate;
            a_current_user_kind     : tak_usertyp;
            a_is_exclusive          : boolean;
            a_user_perm_ref         : tgg00_UserRef;
            a_acc_user_id           : tgg_surrogate;
            a_acc_user              : tsp_knl_identifier;
            a_acc_groupname         : tsp_knl_identifier;
            a_acc_dba_id            : tgg_surrogate;
            a_acc_dbaname           : tsp_knl_identifier;
            a_command_count         : tsp_int4;
            a_costwarn_value        : tsp_int4;
            a_costlimit_value       : tsp_int4;
            a_user_timeout          : tsp_int2;
            a_use_timeout           : boolean;
            a_check_lock            : boolean;
            a_ak_language           : tsp_c3;
            a_errormsg_loading      : boolean;
            a_trigger_tree          : tgg00_FileId;
            a_dialog_area           : tsp_bufaddr;
            a_sample_pages          : tsp_int4;
            a_optimize_info         : tak_optimize_info;
            a_proc_user             : tsp_knl_identifier;
            a_progusage_add         : boolean;
            a_proc_compile          : boolean;
            a_proc_id               : tgg_surrogate;
            a_show_data_cnt         : tsp_int4;
            a_dca                   : tsp_kernel_dca;
            a_states                : tak_site_state_ptr;
            a_proc_userid           : tgg_surrogate;
            a_resname_addr          :
                  ARRAY [ cak_extern_pos..cak_aux_pos ] OF
                  tak_sysbufferaddress;
            a_union_res_id          : tsp_int4;
            a_first_union           : boolean;
            a_expl_resname          : tsp_knl_identifier;
            a_sub_found             : boolean;
            a_send_messbuf          : boolean;
            a_union_append          : boolean;
            a_next_upos             : tsp_int2;
            a_dbproc_cache          : tak_dbproc_cache;
            a_max_res_id            : tsp_int4;
            a_show_curr             : tgg00_FileId;
            a_pars_curr             : tgg00_FileId;
            a_pars_file             : tgg00_FileId;
            a_usage_curr            : tgg00_FileId;
            a_into_tree             : tgg00_FileId;
            a_intern_tree           : tgg00_FileId;
            a_intern_res_tree       : tgg00_FileId;
            a_intern_cmd_tree       : tgg00_FileId;
            a_sysdir                : tak_sysdirectory;
            a_pars_last_key         : tak_parskey;
            a_dt_format             : tgg_datetimeformat;
            a_fk_check              : boolean;
            a_fetch_rest            : boolean;
            a_insert_select         : boolean;
            a_transinf              : tak_transinfo; (* 92 Bytes *)
            a_named_subtrans        : boolean;
            a_link_pending          : boolean;
            a_rqscrr                : tsp_int2;
            a_long_desc_pos         : tsp_int2;
            a_result_cnt            : tsp_int2;
            a_proc_appl             : tsp_knl_identifier;
            a_proc_routine          : tsp_knl_identifier;
            a_temp_tablekey         : tgg_surrogate;
            a_constraint_cnt        : tsp_int2;
            a_fk_check_cnt          : tsp_int2;
            a_execute_trigger       : boolean;
            a_trigger_level         : tsp00_Uint1;
            a_trigger_trace         : boolean;
            a_sqlmode               : tsp_sqlmode;
            a_trigger_trace_key     : tsp_c4;
            a_restore_with_string   : tsp_int2;
            a_outer_n               : tsp_int2;
            a_session_sqlmode       : tsp_sqlmode;
            a_dbproc_level          : tsp00_Uint1;
            a_acc_termid            : tsp_termid;
            a_col_file_count        : tsp_int_map_c2;
            a_collection_id         : tsp_c18;
            a_row_not_found_text    : tak_order_errortext;
            a_len_row_not_found_text: tsp_int2;
            a_col_func_count        : tsp_int2;
            a_size_ptr              : tak_size_ptr;
            a_func_id               : tgg_surrogate;
            a_stack_addr            : tgg_stack_list_ptr;
            a_unused_n              : boolean;               (* unused *)
            a_is_term_driver        : boolean;
            a_sys_dbproc            : boolean;
            a_new_interpreter       : boolean;
            a_p_arr1      : tak_syspointerarr;
            a_p_arr2       : tak_syspointerarr;
            a_p_arr3       : tak_syspointerarr;
            a_p_arr4       : tak_syspointerarr;
            a_p_arr5       : tak_syspointerarr;
            a_ptr1         : tak_sysbufferaddress;
            a_ptr2         : tak_sysbufferaddress;
            a_ptr3         : tak_sysbufferaddress;
            a_ptr4         : tak_sysbufferaddress;
            a_ptr5         : tak_sysbufferaddress;
            a_ptr6         : tak_sysbufferaddress;
            a_ptr7         : tak_sysbufferaddress;
            a_ptr8         : tak_sysbufferaddress;
            a_ptr9         : tak_sysbufferaddress;
            a_ptr10        : tak_sysbufferaddress;
            a_cmd_stack    : tak_cmd_stack;
            a_drda_info_ptr: tak_sysbufferaddress;
            a_old_packet_p : tsp1_packet_ptr;
            a_error_key_ptr: tak_sysbufferaddress;
            a_unionrec_ptr : tak_sysbufferaddress;
            a_nls_params   : tak_nls_param_rec;
            a_opt_info_ptr : tak_sysbufferaddress;
            a_err_parm_cnt : tsp_int2;
            a_error_params : tsp_c80;
            a_uni_cmd_part : tsp1_part_ptr;
            a_role_ptr     : tak_role_context_ptr;
            a_role_info    : tak_role_info;
            a_recursive_no          : tsp_int2;
            a_recursive_state       : tak_recursive_state;
            a_outstanding_packet    : boolean;
            a_no_of_com_objects     : tsp_int4;
            a_kernel_sink           : tak_kernel_sink;
            a_mass_all_rescount     : tsp_int4;   (* h.b. CR 1000072 *)
            a_subq_datatype_problem : boolean;    (* PTS 1001162 E.Z. *)
            a_release_instances     : boolean;
            a_init_info_output      : boolean;
            a_abap_tab_comm_ok      : boolean; (* PTS 1107485 E.Z. *)
            a_first_hint_node       : tsp_int2;
            a_free_pcounts          : tak_pcountarr;  (* PTS 1109291 E.Z. *)
            a_first_free_pcount     : tsp_int2;       (* PTS 1114071 E.Z. *)
            a_last_free_pcount      : tsp_int2;       (* PTS 1114071 E.Z. *)
            a_max_used_pcount       : tsp_c3;         (* PTS 1109291 E.Z. *)
            a_isInCppDBProcedure    : boolean;        (* PTS 1110148 T.A. *)
            a_allocatorInfo         : tak_allocator_info; (* PTS 1110148 T.A. *)
            (*   ============= serverdb alle raus ============ *)
            a_current_auth_site     : tgg_serverdb_no;
      END;

      tak_acv_address  = ^tak_all_command_glob;

      tak_a11_like_record  =  RECORD
            a1l_p_arr      : tak_syspointerarr;
            a1l_privset    : tak_privilege;
            a1l_likecolset : tak_columnset;
            a1l_rec_to_ext : tak_colindex;
      END;


      tak_constraint_info = RECORD
            tree_index : tsp_int2;
            extcolno   : tsp_int2;
      END;


      tak_a11_glob  =  RECORD  (* a11v *)
            a1authid            : tgg_surrogate;
            a1tableid           : tgg_surrogate;
            a1likeid            : tgg_surrogate;
            a1authname          : tsp_knl_identifier;
            a1likename          : tsp_knl_identifier;
            a1tablen            : tsp_knl_identifier;
            a1coln              : tsp_knl_identifier;
            a1liketablen        : tsp_knl_identifier;
            a1sysk              : tgg_sysinfokey;
            a1valkey            : tgg_sysinfokey;
            a1p_arr             : tak_syspointerarr;
            a1constraint_info   : ARRAY[ 0..cak_max_constraints ]
                  OF tak_constraint_info;
            a1colsysp           : tak_sysbufferaddress;
            a1sysbuf            : tak_sysbufferaddress;
            a1valsysp           : tak_sysbufferaddress;
            a1domainbuf         : tak_sysbufferaddress;
            a1colptr            : tak00_colinfo_ptr;
            a1ti                : tsp_int4;
            a1ci                : tsp_int4;
            a1reclen            : tsp_int4;
            a1longcharcnt       : tsp_int4;
            a1temp_id           : tsp_int4;
            a1foreign_key_node  : tsp_int2;
            a1extcolno          : tsp_int2;
            a1keypos            : tsp_int2;
            a1strcolcount       : tsp_int2;
            a1part2_pos         : tsp_int2;
            a1key_node          : tsp_int2;
            a1unique_node       : tsp_int2;
            a1default_datatype  : tsp_data_type;
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
            a1constraint_cnt    : tsp_int2;
            a1init_colcount     : tsp_int2;
            a1constraint_index  : tsp_int2;
            a1serial_node       : tsp_int2;
            a1constraint_name   : tsp_knl_identifier;
            a1long_col_cnt      : tsp_int2;
      END;


      tak_a21_glob   = RECORD
            a2user_name    : tsp_knl_identifier;
            a2like_name    : tsp_knl_identifier;
            a2groupname    : tsp_knl_identifier;
            a2password     : tsp_name;
            a2user_id      : tgg_surrogate;
            a2like_id      : tgg_surrogate;
            a2group_id     : tgg_surrogate;
            a2sysk         : tgg_sysinfokey;
            a2params       : tak_userpar_arr;
            a2sysbuf       : tak_sysbufferaddress;
            a2errorpos     : tsp_int4;
            a2ti           : tsp_int2;
            a2userkind     : tsp_int2;
            a2like         : boolean;
            a2modified     : boolean;
            a2exclusive    : boolean;
            a2ex_modified  : boolean;
            a2usergroup    : boolean;
            a2cascade      : boolean;
            a2role         : boolean;
      END;


      tak_a25_glob  =  RECORD
            a2ti          : tsp_int4;
            a2key_col_ptr : tak00_colinfo_ptr;
            a2linkname    : tsp_knl_identifier;
            a2keycount    : tsp_int2;
            a2primlink    : tak_sysbufferaddress;
            a2seclink     : tak_sysbufferaddress;
            a2resfile     : tgg00_FileId;
            a2columnn     : tsp_knl_identifier;
            a2refcol_ti   : tsp_int2;
            a2default     : tgg_lkey;
            a2syskey      : ARRAY[ 1..2 ] OF tgg_sysinfokey;
            a2base_p      : ARRAY[ 1..2 ] OF tak_sysbufferaddress;
            a2version     : ARRAY[ 1..2 ] OF tsp_c2;
            a2authname    : ARRAY[ 1..2 ] OF tsp_knl_identifier;
            a2tablen      : ARRAY[ 1..2 ] OF tsp_knl_identifier;
            a2fieldpos    : ARRAY[ 1..MAX_STRATEGY_KEYS_GG00 ] OF tsp_int2;
            a2create_tab  : boolean;
            a2fillchar    : tsp_c1;
            a2delact      : tsp_int2;
      END;


      tak_a30_utility_glob  =  RECORD
            a3authname     : tsp_knl_identifier;
            a3tablen       : tsp_knl_identifier;
            a3coln         : tsp_knl_identifier;
            a3ti           : tsp_int2;
            a3passw        : tsp_name;
            a3p_arr        : tak_syspointerarr;
            a3treeid       : tgg00_FileId;
            a3hostfn       : tsp_vfilename;
      END;


      tak_long_descriptor = RECORD
            CASE integer OF
                1 :
                    (desc : tsp_long_descriptor);
                2 :
                    (lds_descriptor  : tgg_surrogate;
                    lds_tabid        : tgg_surrogate;
                    lds_maxlen       : tsp_int4;
                    lds_intern_pos   : tsp_int4;
                    lds_infoset      : tsp_ld_info;
                    lds_state        : tsp00_LdState;
                    lds_show_kind    : tgg_show_kind;
                    lds_valmode      : tsp_valmode;
                    lds_valind       : tsp_int2;
                    lds_colno        : tsp_int2;
                    lds_valpos       : tsp_int4;
                    lds_vallen       : tsp_int4);
                3 :
                    (ldc_descriptor  : tgg_surrogate;
                    ldc_tabid        : tgg_surrogate;
                    ldc_maxlen       : tsp_int4;
                    ldc_intern_pos   : tsp_int4;
                    ldc_infoset      : tsp_ld_info;
                    ldc_state        : tsp00_LdState;
                    ldc_comment_type : tak_comment_type);
                END;
            (*ENDCASE*)


      tak_long_desc_block = RECORD
            CASE integer OF
                1 :
                    (ldb_desc : PACKED ARRAY [ 1..mxsp_long_desc_block ] OF char);
                2 :
                    (ldb_descriptor : tgg_surrogate;
                    ldb_filler2     : tsp_int2;
                    ldb_change      : tsp_ldb_change;
                    ldb_filler3     : boolean;
                    ldb_curr_pageno : tsp_int4;
                    ldb_curr_pos    : tsp_int2;
                    ldb_filler0     : tsp_c1;
                    ldb_show_kind   : tgg_show_kind;
                    (* above: 20 bytes not seen by application *)
                    ldb_intern_pos  : tsp_int4;
                    (* above: 4 bytes may be seen by application *)
                    ldb_infoset     : tsp_ld_info;
                    ldb_filler1     : boolean;
                    (* above: 2 bytes not seen by application *)
                    ldb_valind      : tsp_c1;
                    (* above: 1 byte only seen by the application *)
                    ldb_valmode     : tsp_valmode;
                    ldb_oldvalpos   : tsp_int2;
                    ldb_oldvallen   : tsp_int2;
                    (* above: 5 bytes seen by the application and knl *)
                    ldb_valpos      : tsp_int4;
                    ldb_vallen      : tsp_int4;
                    (* above: additional 8 bytes for new packet >= 6.1.2 *)
                    ldb_full_len    : tsp_int4;
                    ldb_filler4     : tsp_int4;
                    (* below: 8 bytes not seen by the application *)
                    ldb_last_i_pos  : tsp_int4;
                    ldb_root_check  : tsp_c4);
                3 :
                    (ldb1_descriptor  : tgg_surrogate;
                    ldb1_filler2      : tsp_int2;
                    ldb1_change       : SET OF (ldb1_use_termchar,
                          ldb1_use_conversion);
                    ldb1_filler3      : boolean;
                    ldb_colno         : tsp_int4;
                    ldb1_curr_pos     : tsp_int2;
                    ldb1_colno        : tsp_c1;
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
            cused_fields : tsp_int2;
            ccfill       : tsp_int2
      END;

      tak_needed_spec = (
            ons_with_username,
            ons_only_tablename);

      tak_one_table  =  RECORD        (* 588 *)
            ouser         : tsp_knl_identifier;
            otable        : tsp_knl_identifier;
            oreference    : tsp_knl_identifier;
            ofromtableid  : tgg_surrogate;
            otreeid       : tgg00_FileId;
            opages        : tsp_int4;
            oall_priv     : boolean;
            ospecialname  : tak_one_table_specialname;
            oview         : boolean;
            oviewqual     : boolean;
            oviewcheck    : boolean;
            ocomplex_view : boolean;
            otab_node     : tsp_int2;
            ospecs_needed : tak_needed_spec;
            ocounttabs    : tsp00_Uint1;
            oattributes   : tak_table_attribute_set;
            ounused0      : tsp_int1;       (* free *)
            ounused1      : tsp_int4;       (* free *)
            oprivset      : tak_columnset;
            osetallpriv   : tak_columnset;
      END;


      tak_dml_onefield  =  RECORD   (* a55-a58 *)
            dml_col_ptr           : tak00_colinfo_ptr;
            dml_index             : tsp_int2;
            dml_colno_in_subquery : tsp_int2;
            dml_node              : tsp_int2;
            dml_res_index         : tsp00_Uint1;
            dml_fill              : tsp00_Uint1;
      END;

      tak_tab_allow_state  =  (only_one_tab,
            may_more_tabs,
            are_more_tabs,
            tabs_with_arith);
      tak_all_from_tables  =  ARRAY [ 1..cak_maxsources ] OF tak_one_table;
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
            d_resbuf_addr        : tsp_bufaddr;
            (* sparr used for resultset *)
            d_alpha_fill1        : PACKED ARRAY [ 1..68 ] OF char;
            d_user               : tsp_knl_identifier;
            d_table              : tsp_knl_identifier;
            d_column             : tsp_knl_identifier;
            d_refname            : tsp_knl_identifier; (* for NAME =  ... *)
            (* --- *)
            d_upd_set            : tak_columnset;
            d_foundset           : tak_columnset;
            d_param_st_index     : tsp_int2;
            d_param_st_begin     : tsp_int2;
            (* --- *)
            d_first_tab          : tsp_int2;
            d_udt_datatype       : tsp_c2;
            d_pargeslen          : tsp_int2;
            d_movebefore         : tsp_int2;
            (* --- *)
            d_index              : tsp_int2;
            d_inoutpos           : tsp_int2;
            d_vppos              : tsp_int4;
            (* --- *)
            d_exprno             : tsp_int2; (*no used for expressions*)
            d_cntfromtab         : tsp_int2;
            d_acttabindex        : tsp_int2;
            d_fieldno            : tsp_int2;
            (* --- *)
            d_keylen             : tsp_int2;
            d_outcolno           : tsp_int2;
            d_maxlen             : tsp_int4;
            (* --- *)
            d_pos_in_parsbuf     : tsp_int2;
            d_parnum             : tsp_int2;
            d_checkview          : boolean;
            d_join_dml           : boolean;
            d_colindex           : tsp_int2;
            (* --- *)
            d_reclen             : tsp_int2;
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
            d_datatype           : tsp_data_type;
            (* --- *)
            d_allowed            : tak_tab_allow_state;
            d_pars_kind          : tak_fp_kind_type;
            d_maxcounttabs       : tsp_int2;
            d_basetabno          : tsp_int2;
            d_corr               : tak_corr_type;
            d_lowpars            : tsp00_Uint1;
            (* --- *)
            d_level              : tak_corlevel;
            d_index_strat_poss   : tak_inv_only;
            d_oldlowpars         : tsp00_Uint1;
            d_prep_corr_cols     : boolean;
            d_like_expression    : boolean;
            (* --- *)
            d_n_pos              : tsp_int4;
            d_filled_bytes       : tsp_int2;
            d_escape_char        : tsp_c2;
            (* --- *)
            d_expand             : tsp_int2;
            d_concat             : boolean;
            d_hex                : boolean;
            d_first_union        : boolean;
            d_change_date_time   : boolean;
            d_one_join_phase     : boolean;
            d_check_null_byte    : boolean;
            (* --- *)
            d_const_value_expr   : boolean;
            d_is_parameter_only  : boolean;
            d_strat_info         : tgg_ref_info_index;
            d_truncate           : boolean;
            d_topos              : tsp_int2;
            d_in_method_param    : boolean;
            d_escape             : boolean;
            (* --- *)
            d_mselect_rescnt     : tsp_int4;
            d_execute_unlock     : boolean;
            d_pseudo_ins_select  : boolean;
            d_viewdesc_linkage   : tsp_c2;
            (* --- *)
            d_inv_colno          : tak_inv_colnos;
            d_resname            : tsp_knl_identifier;
            d_oj_tables          : tak_joinset;
            (* --- *)
            d_all_count          : boolean;
            d_repl_reskey        : boolean;
            d_mselect_loop_cnt   : tsp_int2;
            d_inv_cnt            : tsp00_Uint1;
            d_align_fill         : boolean;   (* unused *)
            d_single_expr        : boolean;
            d_only_table_scan    : boolean;
            (* --- *)
            (* length is mxak_part_dml_init                  *)
            (* variables above are always initialized        *)
            (* --- *)
            d_sparr              : tak_syspointerarr;
            d_linkbuf            : tak_sysbufferaddress;
            d_viewtextbuf        : tak_sysbufferaddress;
            (* --- *)
            d_literalrec         : tak_sysbufferaddress;
            (* --- *)
            d_alpha_fill2        : PACKED ARRAY [ 1..68 ] OF char;
            (* --- *)
            d_tableid            : tgg_surrogate;
            (* --- *)
            d_act_node           : tsp_int2;
            d_cntpar             : tsp_int2;
            d_globstate          : tgg00_HandlingSet;
            d_unchanged_globstate: tgg00_HandlingSet;
            (* --- *)
            d_pos_result         : tsp_int2;
            d_rowno              : tsp_int2;
            d_resname_found      : boolean; (* resultset name *)
            d_use_sub            : boolean;
            d_distinct           : tgg_distinct;
            d_view               : boolean;
            (* --- *)
            d_viewusername       : tsp_knl_identifier;
            d_viewtablen         : tsp_knl_identifier;
            (* --- *)
            d_subquery           : boolean;
            d_sublevel           : tsp00_Uint1;  (* only for executing *)
            d_subcount           : tsp_int2;
            d_phase_cnt          : tsp_int2;
            d_only_sem_check     : boolean;
            d_union              : boolean;
            (* --- *)
            d_union_order_n      : tsp_int2;
            d_union_insert       : boolean;
            d_outer_join         : boolean;
            d_repl_nokey_ins_sel : boolean;
            d_is_function_column : boolean;
            d_global_pos_result  : tsp_int2;
            (* --- *)
            d_longdesc_found     : boolean;
            d_unused1            : boolean;  (* unused *)
            d_dist_optim         : tsp_int2;
            d_qual_kind          : tgg_qual_kind;
            d_view_col_list      : boolean;
            d_view_col_node      : tsp_int2;
            (* length is mxak_full_dml_init                           *)
            (* variables above are only initialized when NOT in_union *)
            (* --- *)
            (* following variables will be not or partly initialized  *)
            d_tabarr             : tak_all_from_tables;
            d_joins              : tak_joinrec;
            d_change             : tak_changerecord;
            d_change_align       : tsp_int4;
            (* d_change also used for insert..select *)
            (* --- *)
            d_order_cols         : tak00_ord_fields;
            d_group_cols         : tak00_ord_fields;
            d_order_or_group_cols: tak00_ord_fields_ptr;
            d_alpha_fill3        : PACKED ARRAY [ 1..4 ] OF char;
            (* --- *)
            d_ch_datatype        : tsp_data_type;
            d_like_optimize      : boolean;
            d_like_part          : tsp00_Uint1;
            d_string_column      : boolean;
            (* PTS 1111575 E.Z. *)
            d_unused11           : boolean;      (* unused *)
            d_with_lock_clause   : boolean;
            d_first_longdesc_pos : tsp_int2;
            (* --- *)
            d_cnt_longdesc       : tsp_int2;
            d_subquery_node      : tsp_int2;
            d_last_filler        : tsp_int4;     (* unused *)
            d_ins_sel_tabid      : tgg_surrogate;
            (* --- *)
      END;

      tak_part_dml_info = PACKED ARRAY [ 1..mxak_full_dml_init ] OF char;
      tak_dfarr  =  ARRAY [ 1..MAX_COL_PER_TAB_GG00 ] OF tak_dml_onefield;

      tak_explain_rec  =  RECORD
            exp_user         : tsp_knl_identifier;
            exp_table        : tsp_knl_identifier;
            exp_column       : tsp_knl_identifier;
            exp_only_index   : tsp_c1;
            exp_distinct_opt : tsp_c1;
            exp_temp_inv     : tsp_c1;
            exp_more_qual    : tsp_c1;
            exp_strat        : tsp_c40;
            exp_pagecount    : tsp_int4
      END;


      tak_select_record  =  RECORD
            sel_res_tree          : tgg00_FileId;
            sel_res_name          : tsp_knl_identifier;
            sel_output_n          : tsp_int2;
            sel_into_n            : tsp_int2;
            sel_from_n            : tsp_int2;
            sel_group_n           : tsp_int2;
            sel_having_n          : tsp_int2;
            sel_order_n           : tsp_int2;
            sel_res_b             : tsp_int2;
            sel_parinf_b          : tsp_int2;
            sel_is_not_corr_search: boolean;
            sel_new_atdistinct    : tgg_distinct;
            sel_last_pars_part    : boolean;
            sel_where_done        : boolean;
            sel_view_done         : boolean;
            sel_has_subquery      : boolean;
      END;

      tak_pageno_array   =  ARRAY [ 1..MAX_COL_PER_TAB_GG00 ] OF tsp_int4;
      tak_fs_value_qual  =  (c_startpos, c_endpos, c_length4, c_length2);
      tak_fields_to_use  =  (no_fields, keyfields, corr_fields);

      tak_corr_infos  =  RECORD
            co_corr_current    : tgg00_FileId;
            co_corr_buf        : tsp_bufaddr;
            co_use_cinfo       : boolean;
            co_use_fields      : tak_fields_to_use;
            co_all_recs_len    : tsp_int2;
            co_one_rec_len     : tsp_int2;
            co_one_key_len     : tsp_int2;
            co_lcomp_len       : tsp_int2;
            co_startkeyfields  : tsp_int2;   (* new mess_buf *)
            co_act_pos         : tsp_int2;
            co_sel_keypos      : tsp_int2;
            co_upd_keylen      : tsp_int2;
            co_minkeylen       : tsp_int2;
            co_varkeyvalstack  : tsp_int2;
            co_fill            : tsp_int2;
      END;


      tak_fill_rec  =  RECORD
            fr_f_no          : tsp_int2;
            fr_last_fno      : tsp_int2;
            fr_total_leng    : tsp_int2;
            fr_leng          : tsp_int2;
            fr_uni_fill_c2   : tsp_c2;
            fr_filler0       : tsp_int2;
      END;


      tak_exec_rec  =  RECORD
            ex_act_treeid    : tgg00_FileId;
            ex_all_rescnt    : tsp_int4;
            ex_plus          : tsp_int2;
            ex_maxbufs       : tsp_int2;
            ex_with_link     : boolean;
            ex_output_wanted : boolean;
            ex_command_needed: boolean;
            ex_fill          : boolean;
            ex_mb_data       : tgg00_DataPartPtr; (* PTS 1112612 *)
            ex_updjv_ptr     : tsp_moveobj_ptr;
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
            llclongid       : tgg_surrogate;
            llcdatapos      : tsp_int4;
            llcdatalen      : tsp_int4;
            llwith_shortcol : boolean;
            llfiller1       : boolean;
            llfiller2       : tsp_int2;
            llfiller3       : tsp_int4;
      END;

      tak_litcoldesc_arr    =  ARRAY[ 1..cak_max_litcoldesc ] OF
            tak_litcoldesc;
      tak_longlit_buf    =  PACKED ARRAY[ 1..mxak_longlit_buf ] OF char;

      tak_longliteral_rec  =  RECORD
            litreclen        : tsp_int2;
            litkeylen        : tsp_int2;
            litvarcol_offset : tsp_int2;
            litvarcol_cnt    : tsp_int2;
            litsurrogate     : tgg_surrogate;
            litentrytype     : tsp_c2;
            litlinkage       : tsp_c2;
            littabid         : tgg_surrogate;
            litcolcount      : tsp_int2;
            litfiller1       : tsp_int2;
            litlen           : tsp_int4;
            litsize          : tsp_int4;
            litdataptr       : tsp_moveobj_ptr;
            CASE boolean OF
                true :
                    (litcoldesc     : tak_litcoldesc_arr);
                false :
                    (litdatabuf     : tak_longlit_buf);
                END;
            (*ENDCASE*)


      tak_systembuffer  =  RECORD
            CASE integer OF
                0 :
                    (b_sl           : tsp_int2;
                    b_kl            : tsp_int2;
                    b_varcol_offset : tsp_int2;
                    b_varcol_cnt    : tsp_int2);
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
                21 :
                    (slonginfo    : tak_longinforecord);
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
                (*     *)
                (* 27 is unused       *)
                (*     *)
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
                37 :
                    (stermset     : tgg00_TermsetRecord);
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
                    (syskey       : tgg_sysinfokey);
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
                64 :
                    (srole : tak_rolerecord);
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

      tak_cache_dir_entry = RECORD
            CASE integer OF
                1 :
                    (cd_pos          : tsp_int4;
                    cd_check         : tsp_int4;
                    cd_less          : tak_cache_dir_pointer;
                    cd_greater       : tak_cache_dir_pointer;
                    cd_fix_prev      : tak_cache_dir_pointer;
                    cd_fix_next      : tak_cache_dir_pointer;
                    cd_lru_prev      : tak_cache_dir_pointer;
                    cd_lru_next      : tak_cache_dir_pointer;
                    cd_cmd_no        : tsp_int4;
                    cd_create_cmd_no : tsp_int4;
                    cd_state         : tak_sysdir_state;
                    cd_balance       : tak_balance_info;
                    cd_buf_no        : tsp_int2;
                    cd_syslen        : tsp_int4;
                    cd_syskey        : tgg_sysinfokey);
                2 :
                    (cd_block_pos  : tsp_int4;
                    cd_block_check : tsp_int4;
                    cd_bl_less     : tak_cache_dir_pointer;
                    cd_next_free   : tak_cache_dir_pointer;
                    cd_bl_fix_prev : tak_cache_dir_pointer;
                    cd_bl_fix_next : tak_cache_dir_pointer;
                    cd_bl_lru_prev : tak_cache_dir_pointer;
                    cd_bl_lru_next : tak_cache_dir_pointer;
                    cd_free_fill   : tsp_int4;
                    cd_block_len   : tsp_int4;
                    cd_bl_state    : tak_sysdir_state;
                    cd_mem_type    : tak_mem_type;
                    cd_bl_buf_no   : tsp_int2;
                    cd_bl_syslen   : tsp_int4;
                    cd_sysinfo     : tak_systembuffer);
                END;
            (*ENDCASE*)

      tak_odbc_partype = ( no_odbc,
            normal_odbc,
            extended_odbc );
      tak_odbc_functiontype = ( odbc_nothing, odbc_call, odbc_fn,
            odbc_oj, odbc_d, odbc_t, odbc_ts, odbc_esc);

      tak_dbproc_stack_param = RECORD
            CASE integer OF
                1 :
                    (i1  : tsp_c1);
                2 :
                    (c2  : tsp_c2);
                3 :
                    (i2  : tsp_int2);
                4 :
                    (i4  : tsp_int4);
                5 :
                    (r4  : tsp_shortreal);
                6 :
                    (r8  : tsp_longreal);
                7 :
                    (ptr : tsp_moveobj_ptr);
                END;
            (*ENDCASE*)


      tak_dbproc_stack_param_debug = RECORD
            dbg_value    : tak_dbproc_stack_param;
            dbg_vt       : tsp_int2;
            dbg_inout    : tsp1_param_io_type;
            dbg_is_ptr   : boolean;
            dbg_length   : tsp_int4;
      END;


      tak_fromsel_tabid = RECORD
            CASE integer OF
                1 :
                    (fromsiteno : tgg_serverdb_no;
                    session     : tgg91_SessionNo;
                    ft          : tgg00_TfnTemp;
                    fs_no       : tsp00_Uint1);
                2 :
                    (fparschar  : tsp_c1;
                    fcmd_count  : tsp_c3;
                    ffill       : tsp_int2;
                    ft1         : tgg00_TfnTemp;
                    fs_no1      : tsp00_Uint1);
                3 :
                    (tabid      : tgg_surrogate);
                END;


      tsp2_devname      = tsp00_VFilename;
      (**)
      tsp2_dev_type     = (
            sp2dt_sys,
            sp2dt_mirr_sys,
            sp2dt_log,
            sp2dt_mirr_log,
            sp2dt_filler,
            sp2dt_data,
            sp2dt_mirr_data,
            sp2dt_trace);
      (**)
      tsp2_dev_type_set = SET OF tsp2_dev_type;
      (**)
      tsp2_io_op_code   = (
            sp2ioc_read,
            sp2ioc_write);
      (**)
      tsp2_v2_return    = (
            sp2v2r_ok,
            sp2v2r_notok,
            sp2v2r_nosupport);
      (**)
      tsp2_v2_devno     = (
            sp2v2d_dev,
            sp2v2d_mirr_dev,
            sp2v2d_all);
      (**)
      tsp2_process_type    = (
            sp2pt_rte,
            sp2pt_utility,
            sp2pt_timeout,
            sp2pt_user,
            sp2pt_filler,
            sp2pt_log_writer,
            sp2pt_trace_writer,
            sp2pt_sender,
            sp2pt_receiver,
            sp2pt_server,
            sp2pt_data_writer,
            sp2pt_conv_scanner,
            sp2pt_event,
            sp2pt_backup,
            sp2pt_garbage_collector,
            sp2pt_objlog_writer);


      tsp3_priority = (
            sp3p_nil,
            sp3p_syscon,
            sp3p_console,
            sp3p_knldiag);
      (**)
      tsp3_msg_type = (
            sp3m_nil,
            sp3m_error,
            sp3m_warning,
            sp3m_info);


      tgg07_ColPosArr     = ARRAY [0..MAX_COLPOSARR_IDX_GG07] OF tsp00_Int2;
      (**)
      (**)
      tgg07_SubqKind = ( no_subquery, query_in_subq, normal_subquery);
      (**)
      (**)
      tgg07_StrRaw = ARRAY[1..MAX_STRATETGY_LEN_GG07] OF tsp_c1;
      (**)
      (**)

      tgg07_StrKeyInRange = RECORD
            skir_keystart      : tgg07_ColPosArr;
            skir_keystop       : tgg07_ColPosArr;
            skir_IN_SUBQ_stpos : tsp00_Int2;
            skir_inelemcnt     : tsp00_Int2;
      END;

      tgg07_InvColSet = PACKED SET OF 1..MAX_COL_SEQUENCE_GG00;
      (**)
      (**)
      tgg07_invstrat_props = (
            isp_exact_match,
            isp_exact_IN_SUBQ_match,
            isp_unique_idx,
            isp_inv_only,
            isp_order_support,
            isp_listmerge_allowed);

      tgg07_StrInvInRange = RECORD
            siir_keystart      : tgg07_ColPosArr;
            siir_keystop       : tgg07_ColPosArr;
            siir_invstart      : tgg07_ColPosArr;
            siir_invstop       : tgg07_ColPosArr;
            siir_startcnt      : tsp00_Int2;
            siir_stopcnt       : tsp00_Int2;
            siir_invroot       : tsp00_PageNo;
            siir_indexno       : tsp00_Int2;
            siir_icount        : tsp00_Int2;
            siir_invcoldesc    : tgg07_InvColSet;
            siir_invlen        : tsp00_Int2;
            siir_inelemcnt     : tsp00_Int2;
            siir_subq_idx      : tsp00_Int2;
            siir_out_invseqlen : tsp00_Int2;
            siir_IN_SUBQ_stpos : tsp00_Int2;
            siir_strat_props   : SET OF tgg07_invstrat_props;
            siir_filler        : tsp_c3;
      END;


      tgg07_StrJoinInv = RECORD
            sji_keystart : tgg07_ColPosArr;
            sji_keystop  : tgg07_ColPosArr;
            sji_invroot  : tsp00_PageNo;
            sji_invindex : tsp00_Int2;
            sji_jfiller  : tsp00_Int2;
      END;


      tgg07_StrJoinMultFields = RECORD (* multiple keys or indices *)
            sjmf_keystart  : tgg07_ColPosArr;
            sjmf_keystop   : tgg07_ColPosArr;
            sjmf_invroot   : tsp00_PageNo;
            sjmf_index_no  : tsp00_Int2;
            sjmf_cntfields : tsp00_Int2;
      END;


      tgg07_StrJoinViewkey = RECORD
            sjv_lastkeylen : tsp00_Int2;
            sjv_minkeylen  : tsp00_Int2;
      END;


      tgg07_StrCatalog = RECORD
            scat_show_kind  : tgg00_ShowKind;
            scat_upd_strat  : boolean;
            scat_authname   : tsp00_Int2;
            scat_tablename  : tsp00_Int2;
            scat_username   : tsp00_Int2;
            scat_columnname : tsp00_Int2;
            scat_fill2      : tsp00_Int2;
            scat_col_needed : ARRAY[1..MAX_SHOW_COLUMNS_GG00] OF boolean;
            scat_qual_cols  : ARRAY[1..MAX_SHOW_COLUMNS_GG00] OF boolean;
            scat_dblink     : tsp00_KnlIdentifier;
      END;

      tgg07_StratEnum = (
            strat_key_equal,
            strat_key_equal_sub,
            strat_key_in,
            (***)
            strat_key_range,
            strat_key_range_sub,
            (***)
            strat_inv_range,
            strat_inv_range_sub,
            strat_inv_in,
            (***)
            strat_undecided,
            (***)
            strat_key_range_fetch,
            strat_inv_range_fetch,
            strat_inv_range_merge_fetch,
            (***)
            strat_join_key_equal,
            strat_join_key_next,
            strat_join_all_keys_equal,
            strat_join_key_range,
            strat_join_viewkey,
            strat_join_inv,
            strat_join_all_inv_equal,
            strat_join_inv_range,
            (***)
            strat_more_than_one,
            (***)
            strat_no_result,
            (***)
            strat_catalog);
      (**)
      tgg07_StratEnumSet = SET OF tgg07_StratEnum;

      tgg07_StrategyInfo = RECORD
            str_result_id          : tgg00_FileId;
            str_strategy           : tgg07_StratEnum;
            str_build_result       : boolean;
            str_distinc            : tgg00_Distinct;
            str_ordering           : boolean;
            str_use_rowno          : boolean;
            str_use_subquery       : tgg07_SubqKind;
            str_corr_single        : boolean;
            str_union_kind         : tsp00_Uint1;
            str_res_possible       : boolean;
            str_use_order          : boolean;
            str_out_keyseqlen      : tsp00_Int2;
            str_qual_kind          : tgg00_QualKind;
            str_cnt_strat          : tsp00_Uint1;
            str_maxresults         : tsp00_Int2;
            str_foundresults       : tsp00_Int4;
            str_key_len            : tsp00_Int2;
            str_rec_len            : tsp00_Int2;
            str_union_key_cnt_offs : tsp00_Int4;
            str_stack_output_offs  : tsp00_Int2;
            str_search_first       : boolean;
            str_all_files          : boolean;
            str_key_order_support  : boolean;
            str_filler             : tsp_c3;
            CASE tgg07_StratEnum OF
                strat_key_equal,
                strat_key_equal_sub,
                strat_key_in,
                strat_key_range_fetch,
                strat_key_range,
                strat_key_range_sub:
                    (str_key_in_range    : tgg07_StrKeyInRange)
                          ;
                strat_inv_range,
                strat_inv_in,
                strat_inv_range_sub,
                strat_inv_range_fetch,
                strat_inv_range_merge_fetch:
                    (str_inv_in_range    : tgg07_StrInvInRange)
                          ;
                strat_join_inv:
                    (str_join_inv        : tgg07_StrJoinInv)
                          ;
                strat_join_all_keys_equal,
                strat_join_key_range,
                strat_join_all_inv_equal,
                strat_join_inv_range:
                    (str_join_multfields : tgg07_StrJoinMultFields)
                          ;
                strat_join_viewkey:
                    (str_join_viewkey    : tgg07_StrJoinViewkey)
                          ;
                strat_catalog:
                    (str_catalog         : tgg07_StrCatalog);
                strat_undecided:
                    (str_raw             : tgg07_StrRaw);
                END;
            (*ENDCASE*)


      tgg07_selectstrat_rec = RECORD
            ssr_old_strategynum : tgg07_StratEnum;
            ssr_new_strategynum : tgg07_StratEnum;
            ssr_invlen          : tsp_int2;
            ssr_filler          : tsp_int2;
      END;


      tgg07_K720selectVars = RECORD
            sv_one_result        : boolean;
            sv_trunc_part2       : boolean;
            sv_no_oj_destroy_f   : boolean;
            sv_file_to_drop      : boolean;
            sv_call_do_avg       : boolean;
            sv_call_later_output : boolean;
            sv_finding_possible  : boolean;
            sv_type              : tgg00_MessType;
            sv_type2             : tgg00_MessType2;
            sv_global_strat      : tgg07_StratEnum;
            sc_filler            : tsp00_Int2;
            sv_keylen            : tsp00_Int2;
            sv_dis_file_cnt      : tsp00_Int2;
            sv_tree              : tgg00_FileId;
      END;

      tgg07_OJoinFieldSet = PACKED SET OF 0..MAX_JOINS_GG00;

      tgg07_KbOneJoinInfo = PACKED RECORD
            kboj_recpos  : tsp00_Int2;
            kboj_len     : tsp00_Int2;
            kboj_op      : tgg00_StackOpType;
            kboj_fill1   : boolean;
            kboj_fill2   : tsp00_Int2;
      END;


      tgg07_KbJoinInfo = RECORD
            kbji_parts : ARRAY [1..2] OF tgg07_KbOneJoinInfo;
      END;


      tgg07_KbJoinRec = RECORD
            kbjr_cnt        : tsp00_Uint1;
            kbjr_linkcnt    : tsp00_Uint1;
            kbjr_left_oj    : boolean;
            kbjr_right_oj   : boolean;
            kbjr_lof        : tgg07_OJoinFieldSet;
            kbjr_rof        : tgg07_OJoinFieldSet;
            kbjr_jarr       : ARRAY [1..MAX_JOINS_GG00] OF tgg07_KbJoinInfo;
      END;


      tkb00_ConfigPhase = (
            cfpNone_ekb00,
            cfpStartConfig_ekb00,
            cfpConfigured_ekb00,
            cfpActivateStart_ekb00,
            cfpFiller1_ekb00,
            cfpFiller2_ekb00,
            cfpRestartIncomplete_ekb00);
      (* *)

      tkb00_Check = RECORD                    (* PTS 1103957 JA 1999-09-15 *)
            chkBackup_kb00     : boolean;
            chkLock_kb00       : boolean;
            chkLockSupply_kb00 : boolean;
            chkLog_kb00        : boolean;
            chkLogQueue_kb00   : boolean;
            chkRegion_kb00     : boolean;
            chkTrans_kb00      : boolean;
            chkFiller1_kb00    : boolean;
      END;

      tkb00_LogEntrySeq = tgg90_Cint6;
      tkb00_LogLpnoList = ARRAY [1..MAX_LOGPOS_LIST_KB00] OF tsp00_PageNo;
      tkb00_LogPposList = ARRAY [1..MAX_LOGPOS_LIST_KB00] OF tgg00_PagePos;

      tkb00_SaveptParam = RECORD
            svpId_kb00               : tsp00_Uint4;    (* SAPDB_UInt4 *)
            svpReason_kb00           : tsp00_Uint1;    (* Log_SavepointReason *)
            svpFiller1_kb00          : boolean;
            svpOpenTransCnt_kb00     : tsp00_Uint2;    (* SAPDB_UInt2 *)
            (* *)
            svpIOsequence_kb00       : tsp00_Uint4;    (* Log_IOSequenceNo *)
            svpStartOffset_kb00      : tsp00_Uint4;    (* Log_RawDeviceOffset *)
            (* *)
            svpStartEntryOffset_kb00 : tsp00_Uint2;    (* Data_EntryOffset *)
            svpStartEntryType_kb00   : tsp00_Uint1;    (* Log_ActionType   *)
            svpFiller2_kb00          : boolean;
            svpFiller3_kb00          : tsp00_Int4;
            (* *)
            svpOpenTransRoot_kb00    : tsp00_PageNo;   (* Data_PageNo *)
            svpHistoryRoot_kb00      : tsp00_PageNo;   (* Data_PageNo *)
            (* *)
            svpDate_kb00             : tsp00_Uint4;    (* SAPDB_UInt4 *)
            svpTime_kb00             : tsp00_Uint4;    (* SAPDB_UInt4 *)
            (* *)
            svpStartEntryDate_kb00   : tsp00_Uint4;    (* Kernel_Date *)
            svpStartEntryTime_kb00   : tsp00_Uint4;    (* Kernel_Time *)
            (* *)
            svpFiller4_kb00          : tsp00_C48;
            (* *)
            svpFiller5_kb00          : tsp00_Uint4;
            svpFiller6_kb00          : tsp00_Uint4;
      END;(*104*)


      tkb00_SurrogateParam = RECORD
            srgSession_kb00    : tgg91_SessionNo;
            srgFiller1_kb00    : tsp00_Int4;
            (* *)
            srgSurrogate_kb00  : tgg00_Surrogate;
            (* *)
            srgSyskey_kb00     : tgg00_Surrogate;
            (* *)
            srgWriteTrans_kb00 : tgg91_TransNo;                      (* PTS 1108234 JA 2000-12-13 *)
            srgFiller2_kb00    : tsp00_Int2
      END;


      tkb00_LogPageHeader = RECORD
            logHeader_kb00   : tgg00_PageHeader;
            logPposFree_kb00 : tgg00_PagePos;
      END;


      tkb00_Page = RECORD
            CASE integer OF
                1:
                    (pageLpno_kb00            : tsp00_PageNo;
                    pageType_kb00             : tgg00_PageType;
                    pageType2_kb00            : tgg00_PageType2;
                    pageCheck_kb00            : tgg00_PageCheckType;
                    pageMode_kb00             : tgg00_PageMode);
                2:
                    (pageHeader_kb00          : tgg00_PageHeader;
                    pagePposFree_kb00         : tgg00_PagePos;
                    pageLogBody_kb00          : tgg00_LogBody;
                    pageChecksum_kb00         : tsp00_Int4;
                    pageTrailer_kb00          : tgg00_PageTrailer);
                3:
                    (pageLogHeader_kb00       : tkb00_LogPageHeader;
                    pageFiller1_kb00          : tgg00_LogBody;
                    pageCheckTrailer_kb00     : tgg00_PageCheckTrailer);
                4:
                    (pageBuf_kb00             : tsp00_Page);
                5:
                    (pageLogPos_kb00               : tgg00_LogPos;
                    rstFiller1_kb00                : tsp00_Int4;
                    (* *)
                    rstFiller2_kb00                : boolean;
                    rstConfigPhase_kb00            : tkb00_ConfigPhase;
                    rstSetEndReadOnly_kb00         : boolean;
                    rstLastSaveDataSuccessful_kb00 : boolean;
                    rstFiller3_kb00                : tsp00_Int4;
                    (* *)
                    rstConverterVersion_kb00  : tsp00_Int4;
                    rstPrevConverterVers_kb00 : tsp00_Int4;
                    (* *)
                    rstCurrBackupVersion_kb00 : tsp00_Int4;
                    rstPrevBackupVersion_kb00 : tsp00_Int4;
                    (* *)
                    rstFiller4_kb00           : tsp00_Int4;
                    rstFiller5_kb00           : tsp00_Int4;
                    (* *)
                    rstConfigParam_kb00       : tgg00_ConfigRestartParam;
                    rstFurtherUse2a_kb00      : tsp00_C70;
                    rstFurtherUse2b_kb00      : tsp00_C2;
                    (* *)
                    rstLastSavept_kb00        : tkb00_SaveptParam;
                    rstFurtherUse2_kb00       : tsp00_C64;
                    (* *)
                    rstLastSurrogate_kb00     : tkb00_SurrogateParam;
                    rstFurtherUse3_kb00       : tsp00_C70;
                    rstFurtherUse4_kb00       : tsp00_C2;
                    (* *)
                    rstFurtherUse5_kb00       : tsp00_C70;
                    rstFurtherUse6_kb00       : tsp00_C70;
                    rstFiller6_kb00           : tsp00_Int4;
                    (* *)
                    rstDataBackupCnt_kb00     : tsp00_Int4;
                    rstLastDataBackup_kb00    : tsp00_PageNo;
                    (* *)
                    rstReleaseVersion_kb00    : tsp00_Version;
                    rstDbIdent_kb00           : tsp00_Line)
                END;
            (*ENDCASE*)

      tkb00_PagePtr = ^tkb00_Page;

      tkb00_UnivType = RECORD
            CASE integer OF
                1:
                    (c    : tsp00_C1);
                2:
                    (mess : tgg00_MessType);
                3:
                    (swap : tsp00_SwapKind)
                END;
            (*ENDCASE*)


      tkb00_LogFileRef = RECORD
            lfrFileId_kb00        : tgg00_FileId;
            lfrFileLastPos_kb00   : tgg00_LogPos;
            lfrRightmostPage_kb00 : tsp00_PageNo
      END;

      tkb00_LogFileRefPtr = ^tkb00_LogFileRef;

      tkb00_LogQueueInfo = RECORD
            lqiCountGroup_kb00      : tsp00_8ByteCounter;        (* PTS 1103743 JA 1999-08-25 *)
            lqiCountInsert_kb00     : tsp00_8ByteCounter;
            lqiCountRequest_kb00    : tsp00_8ByteCounter;
            lqiCountWait_kb00       : tsp00_8ByteCounter;
            lqiAvgSumPagewaits_kb00 : tsp00_8ByteCounter;
            (* *)
            lqiLogQueueSize_kb00    : tsp00_Int4;
            lqiMaxQueueUsed_kb00    : tsp00_Int4;
            (* *)
            lqiMaxWaitLogpage_kb00  : tsp00_Int4;
            lqiFiller_kb00          : tsp00_Int4
      END;


      tkb00_LogDeviceInfo = RECORD
            ldiPagesUsed_kb00     : tsp00_Int4;
            ldiPagesNotSaved_kb00 : tsp00_Int4;
            (* *)
            ldiPagesSinceBup_kb00 : tsp00_Int4;
            ldiSaveptCount_kb00   : tsp00_Int4;
      END;


      tkb05_KernelOid = tgg00_OidSpace;
      (* *)
      tkb05_RegionNo     = tsp00_Int2;                               (* PTS 1105492 JA 2000-03-15 *)
      (* *)
      tkb05_RowId        = PACKED ARRAY [1..ROWID_MXKB05        ] OF char;
      tkb05_RowIdPrefix  = PACKED ARRAY [1..ROWID_PREFIX_MXKB05 ] OF char;
      tkb05_RowIdPostfix = PACKED ARRAY [1..ROWID_POSTFIX_MXKB05] OF char;
      (* *)
      tkb05_ShortKey = tsp00_C10;
      (* *)
      tkb05_Handling = SET OF (
            hdCheckCollision_ekb05,
            hdInsLock_ekb05,
            hdInsReq_ekb05,
            hdCheckOwnReq_ekb05,
            hdFiller4_ekb05,
            hdFiller5_ekb05,
            hdFiller6_ekb05,
            hdFiller7_ekb05);

      tkb05_WantedLock = RECORD
            wlTabId_kb05       : tgg00_Surrogate;
            (* *)
            wlMode_kb05        : tgg00_LockReqMode;
            wlState_kb05       : tgg00_LockReqState;
            wlKeyLen_kb05      : tsp00_Int2;
            wlRowWanted_kb05   : boolean;
            wlHandling_kb05    : tkb05_Handling;
            wlGrantedMode_kb05 : tgg00_LockReqMode;
            wlFiller1_kb05     : boolean;
            CASE integer OF
                1:
                    (wlRowId_kb05       : tkb05_RowId);
                2:
                    (wlShortKey_kb05    : tkb05_ShortKey);
                3:
                    (wlRowIdPrefix_kb05 : tkb05_RowIdPrefix;
                    wlRowIdPostfix_kb05 : tkb05_RowIdPostfix;
                    wlRowIdHash_kb05    : tsp00_Int4);
                END;
            (*ENDCASE*)

      tkb05_ItemKind = (
            ikNone_ekb05,
            ikFree_ekb05,
            ikRequest_ekb05,
            ikRowLock_ekb05,
            ikRowTrans_ekb05,
            ikRowId_ekb05,
            ikTabLock_ekb05,
            ikTabTrans_ekb05,
            ikConsistView_ekb05,
            ikOpenTrans_ekb05,
            ikHashOpenTrans_ekb05);
      (**)
      tkb05_LocklistState = (
            lsOpen_ekb05,
            lsClosed_ekb05,
            lsClosedWaitForCheckpt_ekb05);
      (**)
      tkb05_Shutdown = (
            shShutdownNone_ekb05,
            shfiller1_ekb05,
            shShutdownDb_ekb05);
      (**)
      tkb05_TransInfo = SET OF (
            tiAuxPermExists_ekb05,
            tiAuxTempExists_ekb05,
            tiCollision_ekb05,
            tiDropEntry_ekb05,
            tiExclusiveUser_ekb05,
            tiLogFull_ekb05,
            tiNologSession_ekb05,
            tiUserTask_ekb05,
            tiPriority_ekb05,
            tiCancelled_ekb05,
            tiFiller10_ekb05,
            tiFiller11_ekb05,
            tiFiller12_ekb05,
            tiFiller13_ekb05,
            tiFiller14_ekb05,
            tiFiller15_ekb05);
      (**)
      tkb05_TransLockMode = (
            tlmFree_ekb05,
            tlmShare_ekb05,
            tlmExcl_ekb05,
            tlmEotExcl_ekb05);
      (**)
      tkb05_ReqPtr = ^tkb05_Req;

      tkb05_Req = RECORD
            rqItemKind_kb05      : tkb05_ItemKind;
            rqExtentItem_kb05    : boolean;                          (* PTS 1105492 JA 2000-03-15 *)
            rqChkOwnRegion_kb05  : tsp00_Uint1;
            rqChkTransIndex_kb05 : tsp00_Uint1;
            rqChkTaskId_kb05     : tsp00_TaskId;
            (* *)
            rqNextReq_kb05       : tkb05_ReqPtr;
            rqPrevReq_kb05       : tkb05_ReqPtr;
            (* *)
            rqLastReq_kb05       : tkb05_ReqPtr;
            rqFillerAddr_kb05    : tsp00_Addr;                       (* PTS 1105492 JA 2000-03-15 *)
            (* *)
            rqTransIndex_kb05    : tgg00_TransIndex;
            rqEotWanted_kb05     : boolean;
            rqReqMode_kb05       : tgg00_LockReqMode;
            rqLockMode_kb05      : tgg00_LockReqMode;
            rqTransLockMode_kb05 : tkb05_TransLockMode;
            rqFiller1_kb05       : tsp00_Int2;
            (* *)
            rqRowShareLocks_kb05 : tsp00_Int4;
            rqRowExclLocks_kb05  : tsp00_Int4
      END;


      tkb05_RowIdItem = RECORD
            riItemKind_kb05      : tkb05_ItemKind;
            riExtentItem_kb05    : boolean;                          (* PTS 1105492 JA 2000-03-15 *)
            riChkOwnRegion_kb05  : tsp00_Uint1;
            riChkTransIndex_kb05 : tsp00_Uint1;
            riChkTaskId_kb05     : tsp00_TaskId;
            (* *)
            riRowId_kb05         : tkb05_RowId
      END;

      tkb05_RowIdItemPtr = ^tkb05_RowIdItem;
      (**)
      tkb05_RowLockPtr   = ^tkb05_RowLock;

      tkb05_RowLock = RECORD
            rlItemKind_kb05         : tkb05_ItemKind;
            rlExtentItem_kb05       : boolean;                       (* PTS 1105492 JA 2000-03-15 *)
            rlChkOwnRegion_kb05     : tsp00_Uint1;
            rlChkTransIndex_kb05    : tsp00_Uint1;
            rlChkTaskId_kb05        : tsp00_TaskId;
            (* *)
            rlTabId_kb05            : tgg00_Surrogate;
            (* *)
            rlNextHashRow_kb05      : tkb05_RowLockPtr;
            rlRowIdItem_kb05        : tkb05_RowIdItemPtr;
            (* *)
            rlFirstReqRow_kb05      : tkb05_ReqPtr;
            rlFillerAddr_kb05       : tsp00_Addr;                    (* PTS 1105492 JA 2000-03-15 *)
            (* *)
            rlShortKey_kb05         : tkb05_ShortKey;
            rlKeyLen_kb05           : tsp00_Int2;
            rlExclTransIndex_kb05   : tgg00_TransIndex;
            rlLockState_kb05        : tgg00_LockReqState;  (* PTS 1115302 UH 2002-05-02 *)
            rlFiller_kb05           : boolean;             (* PTS 1115302 UH 2002-05-02 *)
            (*                                 16 bytes *)
            rlRowShareLocks_kb05    : tsp00_Int4;
            rlRowOptimCnt_kb05      : tsp00_Int4;
            (* *)
            rlRowOptimVers_kb05     : tsp00_Int4;
            rlShare1TransIndex_kb05 : tgg00_TransIndex;
            rlShare2TransIndex_kb05 : tgg00_TransIndex
      END;

      tkb05_RowHash = ARRAY [1..MAX_ROW_HASH_KB05] OF tkb05_RowLockPtr;
      (**)
      tkb05_RowHashPtr = ^tkb05_RowHash;
      (**)
      tkb05_TabLockPtr = ^tkb05_TabLock;

      tkb05_TabLock = RECORD
            tlItemKind_kb05         : tkb05_ItemKind;
            tlExtentItem_kb05       : boolean;                       (* PTS 1105492 JA 2000-03-15 *)
            tlChkOwnRegion_kb05     : tsp00_Uint1;
            tlChkTransIndex_kb05    : tsp00_Uint1;
            tlChkTaskId_kb05        : tsp00_TaskId;
            (* *)
            tlTabId_kb05            : tgg00_Surrogate;  (* hashing like: HashOpenTrans, ConsistView*)
            (* *)
            tlNextHashTab_kb05      : tkb05_TabLockPtr; (* next hash:    HashOpenTrans, ConsistView*)
            tlFirstReqTab_kb05      : tkb05_ReqPtr;
            (* *)
            tlFirstReqRow_kb05      : tkb05_ReqPtr;
            tlFillerAddr_kb05       : tsp00_Addr;                    (* PTS 1105492 JA 2000-03-15 *)
            (* *)
            tlIsSysExcl_kb05        : boolean;
            tlFiller1_kb05          : boolean;
            tlExclTransIndex_kb05   : tgg00_TransIndex;
            tlShare1TransIndex_kb05 : tgg00_TransIndex;
            tlShare2TransIndex_kb05 : tgg00_TransIndex;
            (* *)
            tlSysShareLocks_kb05    : tsp00_Int4;
            tlTabShareLocks_kb05    : tsp00_Int4;
            (* *)
            tlTabOptimVers_kb05     : tsp00_Int4;
            tlRowShareLocks_kb05    : tsp00_Int4;
            (* *)
            tlRowOptimCnt_kb05      : tsp00_Int4;
            tlRowExclLocks_kb05     : tsp00_Int4
      END;

      tkb05_TabHash     = ARRAY [1..MAX_TAB_HASH_KB05] OF tkb05_TabLockPtr;
      (**)
      tkb05_TabHashPtr  = ^tkb05_TabHash;
      (**)
      tkb05_RowTransPtr = ^tkb05_RowTrans;

      tkb05_RowTrans = RECORD
            rtItemKind_kb05      : tkb05_ItemKind;
            rtExtentItem_kb05    : boolean;                          (* PTS 1105492 JA 2000-03-15 *)
            rtChkOwnRegion_kb05  : tsp00_Uint1;
            rtChkTransIndex_kb05 : tsp00_Uint1;
            rtChkTaskId_kb05     : tsp00_TaskId;
            (* *)
            rtRow_kb05           : tkb05_RowLockPtr;
            rtNextRow_kb05       : tkb05_RowTransPtr;
            (* *)
            rtPrevRow_kb05       : tkb05_RowTransPtr;
            rtFillerAddr_kb05     : tsp00_Addr;                      (* PTS 1105492 JA 2000-03-15 *)
            (* *)
            rtShortKey_kb05      : tkb05_ShortKey;
            rtKeyLen_kb05        : tsp00_Int2;
            rtLockMode_kb05      : tgg00_LockReqMode;
            rtLockState_kb05     : tgg00_LockReqState;
            rtReqMode_kb05       : tgg00_LockReqMode;
            rtReqState_kb05      : tgg00_LockReqState;
            (*                                        16 bytes *)
            rtRowRegion_kb05     : tkb05_RegionNo;
            rtSharedLocked_kb05  : boolean;                          (* PTS 1107452 JA 2000-08-08 *)
            rtOptimSet_kb05      : boolean;                          (* PTS 1107452 JA 2000-08-08 *)
            rtRowHash_kb05       : tsp00_Int4;
            (* *)
            rtRowOptimVers_kb05  : tsp00_Int4;
            rtTabOptimVers_kb05  : tsp00_Int4
      END;

      tkb05_TabTransPtr = ^tkb05_TabTrans;

      tkb05_TabTrans = RECORD
            ttItemKind_kb05      : tkb05_ItemKind;
            ttExtentItem_kb05    : boolean;                          (* PTS 1105492 JA 2000-03-15 *)
            ttChkOwnRegion_kb05  : tsp00_Uint1;
            ttChkTransIndex_kb05 : tsp00_Uint1;
            ttChkTaskId_kb05     : tsp00_TaskId;
            (* *)
            ttTabId_kb05         : tgg00_Surrogate;
            (* *)
            ttTab_kb05           : tkb05_TabLockPtr;
            ttFirstRow_kb05      : tkb05_RowTransPtr;
            (* *)
            ttConsistentRow_kb05 : tkb05_RowTransPtr;
            ttNextTab_kb05       : tkb05_TabTransPtr;
            (* *)
            ttLockMode_kb05      : tgg00_LockReqMode;
            ttLockState_kb05     : tgg00_LockReqState;
            ttReqMode_kb05       : tgg00_LockReqMode;
            ttReqState_kb05      : tgg00_LockReqState;
            ttKeySort_kb05       : boolean;                          (* PTS 1107056 JA 2001-03-01 *)
            ttFiller1_kb05       : boolean;                          (* PTS 1107056 JA 2001-03-01 *)
            ttTabRegion_kb05     : tkb05_RegionNo;
            (* *)
            ttTabHash_kb05       : tsp00_Int4;
            ttRowShareLocks_kb05 : tsp00_Int4;
            (* *)
            ttRowOptimCnt_kb05   : tsp00_Int4;
            ttRowExclLocks_kb05  : tsp00_Int4
      END;


      tkb05_SurrogateTransId = RECORD
            CASE integer OF
                1:
                    (surrId_kb05     : tgg00_Surrogate);
                2:
                    (surrSpace_kb05  : tsp00_Int2;
                    surrTransId_kb05 : tgg91_TransNo)
                END;
            (*ENDCASE*)

      tkb05_HashOpenTransPtr = ^tkb05_HashOpenTrans;

      tkb05_HashOpenTrans = RECORD
            hshItemKind_kb05      : tkb05_ItemKind;
            hshExtentItem_kb05    : boolean;                         (* PTS 1105492 JA 2000-03-15 *)
            hshChkOwnRegion_kb05  : tsp00_Uint1;
            hshChkTransIndex_kb05 : tsp00_Uint1;
            hshChkTaskId_kb05     : tsp00_TaskId;
            (* *)
            hshTransId_kb05       : tkb05_SurrogateTransId;  (* hashing like: TabLock, ConsistView *)
            (* *)
            hshNextHashTab_kb05   : tkb05_TabLockPtr;        (* next hash:    TabLock, ConsistView *)
            hshFillerAddr_kb05    : tsp00_Addr;                      (* PTS 1105492 JA 2000-03-15 *)
            (* *)
            hshUsedCnt_kb05       : tsp00_Int4;
            hshOwnHashValue_kb05  : tsp00_Int4                       (* PTS 1105492 JA 2000-03-15 *)
      END;

      tkb05_OpenTransPtr = ^tkb05_OpenTrans;

      tkb05_OpenTrans = RECORD                                       (* PTS 1105492 JA 2000-03-15 *)
            openItemKind_kb05      : tkb05_ItemKind;
            openExtentItem_kb05    : boolean;
            openChkOwnRegion_kb05  : tsp00_Uint1;
            openChkTransIndex_kb05 : tsp00_Uint1;
            openChkTaskId_kb05     : tsp00_TaskId;
            (* *)
            openNext_kb05          : tkb05_OpenTransPtr;
            openOwnHashTrans_kb05  : ARRAY [1..TRANS_PER_OPEN_ENTRY_KB05] OF tkb05_HashOpenTransPtr;
            (*                                                                        16/32 bytes *)
            openTransId_kb05       : ARRAY [1..TRANS_PER_OPEN_ENTRY_KB05] OF tgg91_TransNo;
            openHashRegion_kb05    : ARRAY [1..TRANS_PER_OPEN_ENTRY_KB05] OF tkb05_RegionNo;
            (*                                                                           24 bytes *)
            openCount_kb05         : tsp00_Int4; (* 1-3 open trans *)
            openFiller1_kb05       : tsp00_Int4
      END;

      tkb05_ConsistViewPtr = ^tkb05_ConsistView;

      tkb05_ConsistView = RECORD
            cstItemKind_kb05        : tkb05_ItemKind;
            cstExtentItem_kb05      : boolean;                       (* PTS 1105492 JA 2000-03-15 *)
            cstChkOwnRegion_kb05    : tsp00_Uint1;
            cstChkTransIndex_kb05   : tsp00_Uint1;
            cstChkTaskId_kb05       : tsp00_TaskId;
            (* *)
            cstConsistViewNo_kb05   : tkb05_SurrogateTransId;(*hashing like: TabLock, HashOpenTrans*)
            (* *)
            cstNextHashTab_kb05     : tkb05_TabLockPtr;      (*next hash:    TabLock, HashOpenTrans*)
            cstNextConsistView_kb05 : tkb05_ConsistViewPtr;
            (* *)
            cstPrevConsistView_kb05 : tkb05_ConsistViewPtr;
            cstFirstOpenTrans_kb05  : tkb05_OpenTransPtr;
            (* *)
            cstStartDate_kb05       : tsp00_Int4;                    (* PTS 1105492 JA 2000-03-15 *)
            cstStartTime_kb05       : tsp00_Int4;
            (* *)
            cstTimeLastUsed_kb05    : tsp00_Int4;                    (* PTS 1105492 JA 2000-03-15 *)
            cstFiller1_kb05         : tsp00_Int4;                    (* PTS 1105492 JA 2000-03-15 *)
            (* *)
            cstMinOpenTransId_kb05  : tgg91_TransNo;
            cstCurrTransIndex_kb05  : tgg00_TransIndex;
      END;

      tkb05_SupplyItemPtr = ^tkb05_SupplyItem;

      tkb05_SupplyItem = RECORD                                      (* PTS 1105492 JA 2000-03-15 *)
            CASE integer OF
                1:
                    (siItemKind_kb05      : tkb05_ItemKind;
                    siExtentItem_kb05     : boolean;              (* to ensure correct item size *)
                    siChkOwnRegion_kb05   : tsp00_Uint1;          (* SupplyItem contains         *)
                    siChkTransIndex_kb05  : tsp00_Uint1;          (* all kinds of items          *)
                    siChkTaskId_kb05      : tsp00_TaskId;
                    siNext_kb05           : tkb05_SupplyItemPtr);
                2:
                    (siConsistView_kb05   : tkb05_ConsistView);
                3:
                    (siHashOpenTrans_kb05 : tkb05_HashOpenTrans);
                4:
                    (siOpenTrans_kb05     : tkb05_OpenTrans);
                5:
                    (siReq_kb05           : tkb05_Req);
                6:
                    (siRowIdItem_kb05     : tkb05_RowIdItem);
                7:
                    (siRowLock_kb05       : tkb05_RowLock);
                8:
                    (siRowTrans_kb05      : tkb05_RowTrans);
                9:
                    (siTabLock_kb05       : tkb05_TabLock);
                10:
                    (siTabTrans_kb05      : tkb05_TabTrans);
                END;
            (*ENDCASE*)

      tkb05_SupplyBlock    = ARRAY [1..MAX_SUPPLY_BLOCK_ITEMS_KB05] OF tkb05_SupplyItem;
      (**)
      tkb05_SupplyBlockPtr = ^tkb05_SupplyBlock;                     (* PTS 1105492 JA 2000-03-15 *)
      (**)
      tkb05_SupplySegmPtr  = ^tkb05_SupplySegment;                   (* PTS 1105492 JA 2000-03-15 *)

      tkb05_SupplySegment = RECORD                                   (* PTS 1105492 JA 2000-03-15 *)
            sgmNext_kb05       : tkb05_SupplySegmPtr;
            sgmFillerAddr_kb05 : tsp00_Addr;
            sgmItemList_kb05   : ARRAY [1..ITEMS_PER_SUPPLY_SEGM_KB05] OF tkb05_SupplyItem;
      END;

      tkb05_SupplySegmPtrList    = ARRAY [1..MAX_SUPPLY_SEGMENTS_KB05] OF tkb05_SupplySegmPtr;
      (**)
      tkb05_SupplySegmPtrListPtr = ^tkb05_SupplySegmPtrList;         (* PTS 1105492 JA 2000-03-15 *)

      tkb05_SupplyPoolGlob = RECORD                                  (* PTS 1105492 JA 2000-03-15 *)
            poolSupplySegmList_kb05   : tkb05_SupplySegmPtrListPtr;
            poolFirstFreeSegm_kb05    : tkb05_SupplySegmPtr;
            (* *)
            poolFreeSegments_kb05     : tsp00_Int4;         (* all elements: lockpool region *)
            poolMinFreeSegms_kb05     : tsp00_Int4;
            (* *)
            poolFiller1_kb05          : tsp00_Int4;
            poolMinOpenTransId_kb05   : tgg91_TransNo;
            poolMinConsistViewNo_kb05 : tgg91_TransNo;
            (*                                         16 bytes *)
            poolSegmsUsedAmount_kb05  : tsp00_8ByteCounter;          (* PTS 1103743 JA 1999-08-25*)
            (* *)
            poolItemsUsedAmount_kb05  : tsp00_8ByteCounter;          (* PTS 1103743 JA 1999-08-25*)
            (* *)
            poolAmountPoints_kb05     : tsp00_8ByteCounter;          (* PTS 1103743 JA 1999-08-25*)
            (* *)
            poolEmptyWarning_kb05     : boolean;            (* read outside region *)
            poolFiller2_kb05          : boolean;
            poolFiller3_kb05          : tsp00_Int2;
            poolOwnRegionId_kb05      : tsp00_RegionId
      END;

      tkb05_SupplyPoolGlobPtr = ^tkb05_SupplyPoolGlob;               (* PTS 1105492 JA 2000-03-15 *)

      tkb05_UnivItemPtr = RECORD                                     (* PTS 1105492 JA 2000-03-15 *)
            CASE integer OF
                1:
                    (uiConsistView_kb05   : tkb05_ConsistViewPtr);
                2:
                    (uiHashOpenTrans_kb05 : tkb05_HashOpenTransPtr);
                3:
                    (uiOpenTrans_kb05     : tkb05_OpenTransPtr);
                4:
                    (uiReq_kb05           : tkb05_ReqPtr);
                5:
                    (uiRowIdItem_kb05     : tkb05_RowIdItemPtr);
                6:
                    (uiRowLock_kb05       : tkb05_RowLockPtr);
                7:
                    (uiRowTrans_kb05      : tkb05_RowTransPtr);
                8:
                    (uiSupplyItem_kb05    : tkb05_SupplyItemPtr);
                9:
                    (uiTabLock_kb05       : tkb05_TabLockPtr);
                10:
                    (uiTabTrans_kb05      : tkb05_TabTransPtr);
                END;
            (*ENDCASE*)


      tkb05_TransLogging = RECORD                                    (* PTS 1105492 JA 2000-03-15 *)
            tlogAuxFnNo_kb05          : tsp00_Int4;        (* outside region *)
            tlogTempFunctRollb_kb05   : tsp00_Int4;        (* outside region *)
            (* *)
            tlogTempEntryCnt_kb05     : tsp00_Int4;        (* outside region *)
            tlogTempFileRoot_kb05     : tsp00_PageNo;      (* outside region *)
            (* *)
            tlogTransPtr_kb05         : tsp00_Addr;        (* outside region - Logging74 UH *)
            tFillerAddr_kb05          : tsp00_Addr;        (* outside region - Logging74 UH *)
      END;

      tkb05_TransEntryPtr = ^tkb05_TransEntry;

      tkb05_TransEntry = RECORD
            teOwnAddr_kb05         : tkb05_TransEntryPtr;
            teNextHash_kb05        : tkb05_TransEntryPtr;
            (* *)
            tePrevHash_kb05        : tkb05_TransEntryPtr;
            teNextTrans_kb05       : tkb05_TransEntryPtr;
            (* *)
            tePrevTrans_kb05       : tkb05_TransEntryPtr;
            teFirstTab_kb05        : tkb05_TabTransPtr;
            (* *)
            teTabRecentlyUsed_kb05 : tkb05_TabTransPtr;
            teRowRecentlyUsed_kb05 : tkb05_RowTransPtr;
            (* *)
            teReqTab_kb05          : tkb05_TabTransPtr;
            teReqRow_kb05          : tkb05_RowTransPtr;
            (* *)
            teConsistView_kb05     : tkb05_ConsistViewPtr;
            teCancelAddr_kb05      : tsp00_RteCommAddr;
            (* *)
            teOwnIndex_kb05        : tgg00_TransIndex;    (* static *)
            teOwnRegion_kb05       : tkb05_RegionNo;      (* static *)
            teSessionId_kb05       : tgg91_SessionNo;
            (* *)
            teSelectTransId_kb05   : tgg91_TransNo;                  (* PTS 1108234 JA 2001-01-19 *)
            teWriteTransId_kb05    : tgg91_TransNo;                  (* PTS 1108234 JA 2000-12-13 *)
            teSubtransId_kb05      : tgg00_SubtransNo;    (* PTS 1109485 E.Z. *)
            (* *)
            teUserId_kb05          : tgg00_UserId;
            teTermId_kb05          : tsp00_TermId;
            teTimeoutEnabled_kb05  : boolean;
            teFiller2_kb05         : boolean;
            (*                                             24 bytes *)
            teTaskId_kb05          : tsp00_TaskId;
            teInfo_kb05            : tkb05_TransInfo;
            teLockMode_kb05        : tkb05_TransLockMode;
            teReqMode_kb05         : tkb05_TransLockMode;
            (* *)
            teReqTimeout_kb05      : tsp00_Int4;
            teTimeLastWrite_kb05   : tsp00_Int4;
            (* *)
            teState_kb05           : tgg00_MessType;
            teObjLockMode_kb05     : tkb05_TransLockMode;
            teFiller3_kb05         : tsp00_Int2;                     (* PTS 1105435 JA 2000-02-01 *)
            teFiller4_kb05         : tsp00_Int4;                     (* PTS 1105435 JA 2000-02-01 *)
            (* *)
            teStartDate_kb05       : tsp00_Int4;
            teStartTime_kb05       : tsp00_Int4;
            (* *)
            teFirstCollLock_kb05   : tgg00_TransIndex;
            teObjReq_kb05          : tgg00_TransIndex;
            teObjReqTimeout_kb05   : tsp00_Int4;
            (* *)
            teWaiting_kb05         : boolean;            (* lockwait region *)
            teFiller5_kb05         : boolean;            (* lockwait region *)
            teFiller6_kb05         : tsp00_Int2;         (* lockwait region *)
            teFiller7_kb05         : tsp00_Int4;         (* lockwait region *)
            (* *)
            teLog_kb05             : tkb05_TransLogging;             (* PTS 1105492 JA 2000-03-15 *)
      END;

      tkb05_TransList       = ARRAY [1..MAX_TRANS_GG00] OF tkb05_TransEntry;
      (**)
      tkb05_TransPtrList    = ARRAY [1..MAX_TRANS_GG00] OF tkb05_TransEntryPtr;  (* JA 2000-03-15 *)
      (**)
      tkb05_TransListPtr    = ^tkb05_TransList;
      (**)
      tkb05_TransPtrListPtr = ^tkb05_TransPtrList;                   (* PTS 1105492 JA 2000-03-15 *)

      tkb05_TransGlob = RECORD
            trFirst_kb05           : tkb05_TransEntryPtr;
            trFirstFree_kb05       : tkb05_TransEntryPtr;
            (* *)
            trTransHash_kb05       : tkb05_TransPtrListPtr;
            trFillerAddr_kb05      : tsp00_Addr;                     (* PTS 1105492 JA 2000-03-15 *)
            trFreeEntries_kb05     : tsp00_Int2;
            trConnectOnlyCnt_kb05  : tsp00_Int2;
            trLockTransCnt_kb05    : tsp00_Int2;
            trReqTransCnt_kb05     : tsp00_Int2;
            (* *)
            trObjLockTransCnt_kb05 : tsp00_Int2;
            trOwnRegion_kb05       : tkb05_RegionNo;                 (* PTS 1105492 JA 2000-03-15 *)
            trRedoWaitTaskId_kb05  : tsp00_TaskId;
            (* *)
            trRowEntryAmount_kb05  : tsp00_8ByteCounter;             (* PTS 1103743 JA 1999-08-25 *)
            (* *)
            trTabEntryAmount_kb05  : tsp00_8ByteCounter;             (* PTS 1103743 JA 1999-08-25 *)
            (* *)
            trSqlReqTimeouts_kb05  : tsp00_8ByteCounter;             (* PTS 1103743 JA 1999-08-25 *)
            (* *)
            trObjReqTimeouts_kb05  : tsp00_8ByteCounter;             (* PTS 1103743 JA 1999-08-25 *)
            (* *)
            trDeadlocks_kb05       : tsp00_8ByteCounter;             (* PTS 1103743 JA 1999-08-25 *)
            (* *)
            trEscalations_kb05     : tsp00_8ByteCounter;             (* PTS 1103743 JA 1999-08-25 *)
      END;

      tkb05_TransGlobPtr  = ^tkb05_TransGlob;                        (* PTS 1105492 JA 2000-03-15 *)

      tkb05_RegionGlob = RECORD                                      (* PTS 1105492 JA 2000-03-15 *)
            rglTransGlob_kb05        : tkb05_TransGlobPtr;
            rglSupplyBlock_kb05      : tkb05_SupplyBlockPtr;
            (* *)
            rglRowHash_kb05          : tkb05_RowHashPtr;
            rglTabHash_kb05          : tkb05_TabHashPtr;
            (* *)
            rglFirstConsistView_kb05 : tkb05_ConsistViewPtr;
            rglFirstFree_kb05        : tkb05_SupplyItemPtr;
            (* *)
            rglExtentFirstSegm_kb05  : tkb05_SupplySegmPtr;
            rglExtentFirstFree_kb05  : tkb05_SupplyItemPtr;
            (* *)
            rglOwnRegion_kb05        : tkb05_RegionNo;
            rglLocklistState_kb05    : tkb05_LocklistState;
            rglFiller1_kb05          : boolean;
            rglFreeItems_kb05        : tsp00_Int4;
            (* *)
            rglExtentItems_kb05      : tsp00_Int4;
            rglExtentFreeItems_kb05  : tsp00_Int4;
            (* *)
            rglConsistViewCnt_kb05   : tsp00_Int4;
            rglOpenTransCnt_kb05     : tsp00_Int4;
            (* *)
            rglMinOpenTransId_kb05   : tgg91_TransNo;
            rglMinConsistViewNo_kb05 : tgg91_TransNo;
            rglFiller2_kb05          : tsp00_Int4;
            (*                                       16 bytes *)
            rglCollisions_kb05       : tsp00_8ByteCounter;           (* PTS 1103743 JA 1999-08-25 *)
      END;

      tkb05_RegionGlobPtr     = ^tkb05_RegionGlob;                   (* PTS 1105492 JA 2000-03-15 *)
      (**)
      tkb05_RgnGlobPtrList    = ARRAY [1..MAX_REGIONS_KB05] OF tkb05_RegionGlobPtr; (* 2000-03-15 *)
      (**)
      tkb05_RgnGlobPtrListPtr = ^tkb05_RgnGlobPtrList;               (* PTS 1105492 JA 2000-03-15 *)

      tkb05_LockStateGlob = RECORD                                   (* PTS 1105492 JA 2000-03-15 *)
            lglTimeLastClear_kb05       : tsp00_Int4;
            lglShutdownWanted_kb05      : tkb05_Shutdown;  (* all elements: lockinit region *)
            lglFiller1_kb05             : boolean;
            lglFiller2_kb05             : tsp00_Int2;
            (* *)
            lglOwnRegionId_kb05         : tsp00_RegionId;
            lglShutdownWaitTask_kb05    : tsp00_TaskId;              (* PTS 1110974 UH 2001-10-08 *)
            (* *)
            lglShutdownWaitTimeout_kb05 : tsp00_Int4;                (* PTS 1110974 UH 2001-10-08 *)
            lglFiller3_kb05             : tsp00_Int4;                (* PTS 1110974 UH 2001-10-08 *)
            (* *)
            lglShutdownCancelByte_kb05  : tsp00_RteCommAddr;         (* PTS 1110974 UH 2001-10-08 *)
            trFiller4_kb05              : tsp00_Addr;                (* PTS 1110974 UH 2001-10-08 *)
      END;

      tkb05_LockStateGlobPtr = ^tkb05_LockStateGlob;                 (* PTS 1105492 JA 2000-03-15 *)

      tkb05_ObjCollisionEntry = RECORD                               (* PTS 1105492 JA 2000-03-15 *)
            objcOwnIndex_kb05       : tgg00_TransIndex;
            objcFirstReq_kb05       : tgg00_TransIndex;
            objcNextCollLock_kb05   : tgg00_TransIndex;
            objcPrevCollLock_kb05   : tgg00_TransIndex;    (* all elements: first row region *)
            (* *)
            objcLockTransId_kb05    : tgg91_TransNo;
            objcLockTransIndex_kb05 : tgg00_TransIndex;
            (* *)
            objcOid_kb05            : tkb05_KernelOid;
            (* *)
            objcObjFileNo_kb05      : tgg00_ObjFileNo;
            objcFiller2_kb05        : tsp00_Int4
      END;

      tkb05_ObjCollEntryPtr = ^tkb05_ObjCollisionEntry;
      (**)
      tkb05_ObjCollList     = ARRAY [1..MAX_TRANS_GG00] OF tkb05_ObjCollisionEntry;
      (**)
      tkb05_ObjCollListPtr  = ^tkb05_ObjCollList;

      tkb05_ObjReqEntry = RECORD                                     (* PTS 1105492 JA 2000-03-15 *)
            objrReqTransId_kb05     : tgg91_TransNo;
            objrOwnIndex_kb05       : tgg00_TransIndex;
            (* *)
            objrCollEntryIndex_kb05 : tgg00_TransIndex;      (* all elements: first row region *)
            objrNextReq_kb05        : tgg00_TransIndex;
            objrWaitTaskId_kb05     : tsp00_TaskId
      END;

      tkb05_ObjReqEntryPtr = ^tkb05_ObjReqEntry;
      (**)
      tkb05_ObjReqList     = ARRAY [1..MAX_TRANS_GG00] OF tkb05_ObjReqEntry;     (* JA 2000-03-15 *)
      (**)
      tkb05_ObjReqListPtr  = ^tkb05_ObjReqList;                      (* PTS 1105492 JA 2000-03-15 *)

      tkb05_ObjCollGlob = RECORD                                     (* PTS 1105492 JA 2000-03-15 *)
            oglObjCollList_kb05      : tkb05_ObjCollListPtr;
            oglObjReqList_kb05       : tkb05_ObjReqListPtr;
            (* *)
            oglObjCollFirstFree_kb05 : tgg00_TransIndex;
            oglObjCollFreeCnt_kb05   : tsp00_Int2;
            oglObjReqCnt_kb05        : tsp00_Int2;
            oglFiller1_kb05          : tsp00_Int2;
            (* *)
            oglFiller2_kb05          : tsp00_Int4;
            oglOwnRegionId_kb05      : tsp00_RegionId;
            (* *)
            oglCollisions_kb05       : tsp00_8ByteCounter
      END;

      tkb05_ObjCollGlobPtr = ^tkb05_ObjCollGlob;                     (* PTS 1105492 JA 2000-03-15 *)

      tkb05_Glob = RECORD                                            (* PTS 1105492 JA 2000-03-15 *)
            glRegionGlob_kb05                : tkb05_RgnGlobPtrListPtr;
            glSupplyPoolGlob_kb05            : tkb05_SupplyPoolGlobPtr;
            (* *)
            glLockStateGlob_kb05             : tkb05_LockStateGlobPtr;
            glObjCollGlob_kb05               : tkb05_ObjCollGlobPtr;
            (* *)
            glTransList_kb05                 : tkb05_TransPtrListPtr;
            glFillerAddr_kb05                : tsp00_Addr;
            (* *)
            glGenerated_kb05                 : boolean;
            glFiller1_kb05                   : boolean;
            glRegions_kb05                   : tsp00_Int2;
            glRowRegions_kb05                : tsp00_Int2;
            glTabRegions_kb05                : tsp00_Int2;
            (* *)
            glTransRegions_kb05              : tsp00_Int2;
            glTransEntries_kb05              : tsp00_Int2;
            glTransEntriesPerRegion_kb05     : tsp00_Int2;
            glTransHashEntriesPerRegion_kb05 : tsp00_Int2;
            (* *)
            glFiller2_kb05                   : tsp00_Int4;
            glDeadlockDetection_kb05         : tsp00_Int4;
            (* *)
            glItems_kb05                     : tsp00_Int4;
            glMaxRowsPerTab_kb05             : tsp00_Int4;
            (* *)
            glSupplyItemsPerRegion_kb05      : tsp00_Int4;
            glMaxItemsPerRegion_kb05         : tsp00_Int4;
            (* *)
            glSupplyPoolSegments_kb05        : tsp00_Int4;
            glSupplyPoolItems_kb05           : tsp00_Int4;
            (* *)
            glRowHashEntriesPerRegion_kb05   : tsp00_Int4;
            glTabHashEntriesPerRegion_kb05   : tsp00_Int4;
            (* *)
            glSelectTransGeneration_kb05     : tgg91_TransNo;        (* PTS 1108234 JA 2000-12-13 *)
            glFiller3_kb05                   : tsp00_Int2;           (* PTS 1108234 JA 2000-12-13 *)
            (* *)
            glNilWantedLock_kb05             : tkb05_WantedLock;
            glNilOpenTrans_kb05              : tkb05_OpenTrans;
            glNilReq_kb05                    : tkb05_Req;
            glNilRowLock_kb05                : tkb05_RowLock;
            glNilRowTrans_kb05               : tkb05_RowTrans;
            glNilTabLock_kb05                : tkb05_TabLock;
            glNilTabTrans_kb05               : tkb05_TabTrans;
            glNilTransEntry_kb05             : tkb05_TransEntry
      END;


      tkb05_UnivPtr = RECORD                                         (* PTS 1105492 JA 2000-03-15 *)
            CASE integer OF
                1:
                    (uvAddr_kb05              : tsp00_ObjAddr);
                2:
                    (uvLockStateGlob_kb05     : tkb05_LockStateGlobPtr);
                3:
                    (uvObjCollGlob_kb05       : tkb05_ObjCollGlobPtr);
                4:
                    (uvObjCollList_kb05       : tkb05_ObjCollListPtr);
                5:
                    (uvObjReqList_kb05        : tkb05_ObjReqListPtr);
                6:
                    (uvRegionGlob_kb05        : tkb05_RegionGlobPtr);
                7:
                    (uvRgnGlobPtrList_kb05    : tkb05_RgnGlobPtrListPtr);
                8:
                    (uvRowHash_kb05           : tkb05_RowHashPtr);
                9:
                    (uvSupplyBlock_kb05       : tkb05_SupplyBlockPtr);
                10:
                    (uvSupplyPoolGlob_kb05    : tkb05_SupplyPoolGlobPtr);
                11:
                    (uvSupplySegm_kb05        : tkb05_SupplySegmPtr);
                12:
                    (uvSupplySegmPtrList_kb05 : tkb05_SupplySegmPtrListPtr);
                13:
                    (uvTabHash_kb05           : tkb05_TabHashPtr);
                14:
                    (uvTransGlob_kb05         : tkb05_TransGlobPtr);
                15:
                    (uvTransList_kb05         : tkb05_TransListPtr);
                16:
                    (uvTransPtrList_kb05      : tkb05_TransPtrListPtr)
                END;
            (*ENDCASE*)


      tkb05_ConsistViewShow = RECORD                                 (* PTS 1105492 JA 2000-03-15 *)
            cshRecLen_kb05       : tsp00_Int2;
            cshKeyLen_kb05       : tsp00_Int2;
            cshVarcolOffset_kb05 : tsp00_Int2;
            cshVarcolCnt_kb05    : tsp00_Int2;
            cshKey_kb05          : tsp00_Int4;
            (* *)
            cshConsistView_kb05  : tgg91_TransNo;
            cshOpenTrans_kb05    : tgg91_TransNo;
            cshStartDate_kb05    : tsp00_Int4;
            cshStartTime_kb05    : tsp00_Int4;
            cshTimeLastUsed_kb05 : tsp00_Int4;
      END;


      tkb05_TransShow = RECORD
            tshRecLen_kb05       : tsp00_Int2;
            tshKeyLen_kb05       : tsp00_Int2;
            tshVarcolOffset_kb05 : tsp00_Int2;
            tshVarcolCnt_kb05    : tsp00_Int2;
            tshKey_kb05          : tsp00_Int4;
            (* *)
            tshTaskId_kb05       : tsp00_TaskId;
            tshSession_kb05      : tgg91_SessionNo;
            (* *)
            tshWriteTransId_kb05 : tgg91_TransNo;
            tsh_filler1_kb05     : tsp00_Int2;            (* unused *)
            tshSubtrans_kb05     : tgg00_SubtransNo;      (* PTS 1109485 E.Z. *)
            tsh_filler2_kb05     : tsp00_Int4;            (* unused *)
            (* *)
            tshStartDate_kb05    : tsp00_Int4;
            tshStartTime_kb05    : tsp00_Int4;
            (* *)
            tshSelTransId_kb05   : tgg91_TransNo;                    (* PTS 1108234 JA 2001-01-19 *)
            tshTermId_kb05       : tsp00_TermId;
            (*                                          24 bytes *)
            tshUserId_kb05       : tgg00_UserId;
            tshReqTimeout_kb05   : tsp00_Int4;
            (* *)
            tshTimeWrite_kb05    : tsp00_Int4;
            tshTrLockMode_kb05   : tkb05_TransLockMode;
            tshTrReqMode_kb05    : tkb05_TransLockMode;
            tshLockMode_kb05     : tgg00_LockReqMode;
            tshReqMode_kb05      : tgg00_LockReqMode;
            (* *)
            tshLockState_kb05    : tgg00_LockReqState;
            tshReqState_kb05     : tgg00_LockReqState;
            tshConsistView_kb05  : tgg91_TransNo                     (* PTS 1105492 JA 2000-03-15 *)
      END;


      tkb05_LockShow = RECORD
            lshTransShow_kb05   : tkb05_TransShow;
            lshTabId_kb05       : tgg00_Surrogate;
            lshRowId_kb05       : tkb05_RowId;
            lshRowIdLen_kb05    : tsp00_Int2;
            lshUndefRowId_kb05  : boolean;
            lshFiller1_kb05     : boolean
      END;


      tkb05_ShowRec = RECORD
            CASE integer OF
                1:
                    (shrConsist_kb05 : tkb05_ConsistViewShow);       (* PTS 1105492 JA 2000-03-15 *)
                2:
                    (shrLock_kb05    : tkb05_LockShow);
                3:
                    (shrTrans_kb05   : tkb05_TransShow);
                4:
                    (shrRec_kb05     : tgg00_Rec)
                END;
            (*ENDCASE*)

      tkb05_ShtHeader      = tsp00_C12;
      tkb05_ShtInfo        = PACKED ARRAY [1..256] OF char;
      tkb05_ShtDesc        = tsp00_C40;
      tkb05_ShtDescUnicode = tsp00_C80;
      tkb05_ShtVal         = tsp00_C12;
      tkb05_ShtValUnicode  = tsp00_C24;

      tkb05_ShowStatRec = RECORD
            CASE integer OF
                1:
                    (shtRecLen_kb05         : tsp00_Int2;
                    shtKeyLen_kb05          : tsp00_Int2;
                    shtVarcolOffset_kb05    : tsp00_Int2;
                    shtVarcolCnt_kb05       : tsp00_Int2;
                    shtKey_kb05             : tsp00_C4;
                    shtDefByte_kb05         : tsp00_C1;
                    shtInfo_kb05            : tkb05_ShtInfo);
                2:
                    (shtHeader_kb05         : tkb05_ShtHeader;
                    shtSpace1_kb05          : tsp00_C1;
                    shtDesc_kb05            : tkb05_ShtDesc;
                    shtDefByte2_kb05        : tsp00_C1;
                    shtVal_kb05             : tkb05_ShtVal);
                3:
                    (shtSpace2_kb05         : tkb05_ShtHeader;
                    shtSpace3_kb05          : tsp00_C1;
                    shtDescUnicode_kb05     : tkb05_ShtDescUnicode;
                    shtDefByte2Unicode_kb05 : tsp00_C1;
                    shtValUnicode_kb05      : tkb05_ShtValUnicode)
                END;
            (*ENDCASE*)


      tkb05_LockInfo = RECORD
            infSqlCollisions_kb05  : tsp00_8ByteCounter;         (* PTS 1103743 JA 1999-08-25 *)
            infOmsCollisions_kb05  : tsp00_8ByteCounter;         (* PTS 1105492 JA 2000-03-15 *)
            infEscalations_kb05    : tsp00_8ByteCounter;
            infDeadlocks_kb05      : tsp00_8ByteCounter;
            infSqlReqTimeouts_kb05 : tsp00_8ByteCounter;
            infObjReqTimeouts_kb05 : tsp00_8ByteCounter;
            infRowEntryAmount_kb05 : tsp00_8ByteCounter;
            infTabEntryAmount_kb05 : tsp00_8ByteCounter;
            infKbCalls_kb05        : tsp00_8ByteCounter;
            (* *)
            infLockTrans_kb05      : tsp00_Int4;
            infReqTrans_kb05       : tsp00_Int4;
            (* *)
            infEscalValue_kb05     : tsp00_Int4;
            infObjLockTrans_kb05   : tsp00_Int4;
            (* *)
            infMaxEntries_kb05     : tsp00_Int4;
            infEntriesUsed_kb05    : tsp00_Int4;
            (* *)
            infAvgUsed_kb05        : tsp00_Int4;
            infMaxUsed_kb05        : tsp00_Int4;
            (* *)
            infConsistViewCnt_kb05 : tsp00_Int4;
            infOpenTransCnt_kb05   : tsp00_Int4;
      END;


      tak40_sel_param = PACKED ARRAY[1..3520]  OF char;
      tak40_get_param = PACKED ARRAY[1..16524] OF char;
      tak40_sel_param_ptr = ^tak40_sel_param;
      tak40_get_param_ptr = ^tak40_get_param;
      tak40_shinfo  =  PACKED ARRAY [ 1..mxak40_shinfo ] OF char;

      tak40_show_record  =  RECORD
            shsl            : tsp_int2;
            shkl            : tsp_int2;
            shvarcol_offset : tsp_int2;
            shvarcol_cnt    : tsp_int2;
            shkey           : tsp_c4;
            shinfo          : tak40_shinfo;
      END;


      tak40_show_glob  =  RECORD
            a4sysbuf       : tak_sysbufferaddress;
            a4col_ptr      : tak00_colinfo_ptr;
            a4dombuf       : tak_sysbufferaddress;
            a4ti           : tsp_int4;
            a4ci           : tsp_int4;
            a4pos          : tsp_int4;
            a4index        : tsp_int4;
            a4id_len       : tsp_int4;
            a4sh_key       : tsp_c4;
			a4LongKey      : tsp_int4;  (* PTS 1113491 *)
            a4p_arr        : tak_syspointerarr;
            a4authname     : tsp_knl_identifier;
            a4tablen       : tsp_knl_identifier;
            a4user         : tsp_knl_identifier;
            a4coln         : tsp_knl_identifier;
            a4sysk         : tgg_sysinfokey;
            a4showfile_name: tsp_knl_identifier;
            a4sh_kind      : tgg_show_kind;
            a4priv_check   : boolean;
            a4priv         : tgg_privilege_set;
            a4is_synonym   : boolean;
            a4del_basetab  : boolean;    (* PTS 1106617 E.Z. *)
            a4tab_count    : tsp_int2;
            a4treeid       : tgg00_FileId;
			a4LongContainer: tgg00_FileId; (* PTS 1113491 *)
            a4sh_buf       : tak40_show_record;
            a4col_len      : tsp_int2;
            a4tab_len      : tsp_int2;
            a4error_pos    : tsp_int4;
            a4synname      : tsp_knl_identifier;
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
            a4usertab_pos  : tsp_int2;
            a4usertab_cnt  : tsp_int2;
            a4mfetch_pos   : tsp_int2;
            a4mfetch_cnt   : tsp_int2;
            a4tree_pos     : tgg00_FilePos;
            a4usertab_buf  : tak_sysbufferaddress;
            a4next_key     : tgg_sysinfokey;
            a4b_err        : tgg_basis_error;
            a4next_owner   : tsp_knl_identifier;
            a4next_tablen  : tsp_knl_identifier;
            a4next_user    : tgg_surrogate;
            a4owner_id     : tgg_surrogate;
            a4data         : tsp_buf;
            a4mblock       : tgg_mess_block;
            a4strategy     : tgg07_StrategyInfo;
            a4sel          : tgg_select_param_ptr;
            a4sel_param    : tak40_sel_param_ptr;
            a4get_param    : tak40_get_param_ptr;
            a4select_vars  : tgg07_K720selectVars;
            a4long_desc_len: tsp_int2;
            a4res_buf_ptr  : tgg_datapart_ptr;
            a4mess_code    : tsp_code_type;
            a4row_ptr      : ^tak40_show_record;
            a4acv_ptr      : tak_acv_address;
            a4all_objects  : boolean;
            a4roles        : boolean;
            a4role_cnt     : tsp_int2;
            a4role_ids     : ARRAY[1..8] OF tgg_surrogate;
      END;

      tak40_show_glob_ptr = ^tak40_show_glob;     (* PTS 1108435 JA 2000-11-27 *)



      tak42_oms_monitor_rec = RECORD
            CASE integer OF
                1 :
                    (align : tsp00_Addr);
                2 :
                    (rec : tgg00_Rec);
                3 :
                    (offset : ARRAY[1..cgg_rec_key_offset] OF char;
                    iid     : tsp00_C16;
                    dispid  : tsp00_Int4;
                    filler  : tsp00_Int4;
                    info    : tgg00_COMMonitorInfo);
                END;
            (*ENDCASE*)


      tak42_monitor_rec_key = RECORD
            CASE boolean OF
                true :
                    (lkey : tgg00_Lkey);
                false :
                    (offset  : ARRAY[1..cgg_rec_key_offset] OF char;
                    iid      : tsp00_C16;
                    dispid   : tsp00_Int4);
                END;
            (*ENDCASE*)


VAR

              a01defaultkey         : tgg_sysinfokey;
              a01diag_monitor_on    : boolean;
              a01sm_collect_data    : boolean;
              a01diag_analyze_on    : boolean;
              a01diag_ana_coll_data : boolean;
              a01char_size          : integer;
              a01sm_reads           : tsp_int4;
              a01sm_selectivity     : tsp_int4;
              a01sm_milliseconds    : tsp_int4;
              a01controluser        : tsp_knl_identifier;
              a01_i_current         : tsp_knl_identifier;
              a01_i_domain          : tsp_knl_identifier;
              a01_il_b_identifier   : tsp_knl_identifier;
              a01_zero_res_name     : tsp_knl_identifier;


              a36xparam_tree : tgg00_FileId;


              a40yes_no : ARRAY[boolean] OF tsp_c3;


              b01niltree_id     : tgg00_FileId;
              b01blankfilename  : tsp00_VFilename;


              g01code               : tgg_code_globals;
              g01glob               : tgg_kernel_globals;
              g01stand_alone        : boolean;
              g01autosave_distance  : tsp00_Int4;
              g01page_size          : tsp_int4;
              g01tabid              : tgg_tabid_globals;
              g01timeout            : tgg00_TimeoutGlobals;
              g01unicode            : boolean;
              g01vtrace             : tgg_vtrace_state;


              a42check_time : boolean;


      ak42refresh_cnt      : tsp00_Int4;
      ak42fast_refresh_cnt : tsp00_Int4;
      a42monitor_arr       : ARRAY [ tgg00_RefInfoIndex ] OF tsp00_C40;
      a42parseid_list      : tsp_moveobj_ptr;


(*------------------------------*)

PROCEDURE
              g02gettermsetdesc (
                    VAR termsetname : tsp_knl_identifier;
                    VAR messcode : tsp_code_type;
                    VAR code     : tsp_code_type;
                    VAR found    : boolean);
        EXTERNAL ;



      PROCEDURE
              bd91InfoGCReset;
        EXTERNAL ;



        PROCEDURE
              bd91InfoGCStats ( idxGC         : tsp00_Int4;
                    VAR taskId                : tsp00_TaskId;
                    VAR active                : boolean;
                    VAR activateCount         : tsp00_8ByteCounter;
                    VAR deleteObj             : tsp00_8ByteCounter;
                    VAR relHistEntryNormal    : tsp00_8ByteCounter;
                    VAR relHistEntrySensitive : tsp00_8ByteCounter;
                    VAR relHistEntryTruncate  : tsp00_8ByteCounter;
                    VAR relHistCreateObjFile  : tsp00_8ByteCounter;
                    VAR relHistDeleteObj      : tsp00_8ByteCounter;
                    VAR relHistDropObjFile    : tsp00_8ByteCounter;
                    VAR relHistLockObj        : tsp00_8ByteCounter;
                    VAR relHistInsertObj      : tsp00_8ByteCounter;
                    VAR relHistNewObj         : tsp00_8ByteCounter;
                    VAR relHistUpdateObj      : tsp00_8ByteCounter;
                    VAR relPageEmpty          : tsp00_8ByteCounter;
                    VAR relPageDropFile       : tsp00_8ByteCounter;
                    VAR relDropFile           : tsp00_8ByteCounter);
        EXTERNAL ;



        PROCEDURE   (* PTS 1115641 FF *)
              bd91InfoContainerRoots ( VAR pObjFileIter  : tsp00_Addr;
                    VAR pContObjFileIter  : tsp00_Addr;
                    pStackarea            : tsp00_Addr;
                    StackareaLen          : tsp00_Int2;
                    VAR ClassId           : tsp00_Int4;
                    VAR Root              : tsp00_Int4;
                    VAR ContainerSequelNo : tsp00_Int4;
                    VAR NumKeyPartitions  : tsp00_Int2;
                    VAR VarContainer      : boolean;
                    VAR DeleteFlag        : boolean;
                    VAR MaxObjBodySize    : tsp00_Int4;
                    VAR MaxObjPerPage     : tsp00_Int4;
                    VAR MaxChains         : tsp00_Int4);
        EXTERNAL ;



        PROCEDURE   (* PTS 1115641 FF *)
              bd91InfoContainerChains ( VAR pObjFileIter  : tsp00_Addr;
                    VAR pContObjFileIter  : tsp00_Addr;
                    pStackarea            : tsp00_Addr;
                    StackareaLen          : tsp00_Int2;
                    VAR ClassId           : tsp00_Int4;
                    VAR ContainerSequelNo : tsp00_Int4;
                    VAR ChainId           : tsp00_Int4;
                    VAR Subroot           : tsp00_Int4;
                    VAR ObjCount          : tsp00_Int4;
                    VAR PageCount         : tsp00_Int4;
                    VAR ActiveGC          : tsp00_Int4;
                    VAR RelEmptyPages     : boolean);
        EXTERNAL ;



        PROCEDURE   (* PTS 1115641 FF  *)
              bd91InfoContainerKeys ( VAR pObjFileIter  : tsp00_Addr;
                    pStackarea            : tsp00_Addr;
                    StackareaLen          : tsp00_Int2;
                    VAR KeyIdx            : tsp00_Int2;
                    VAR ClassId           : tsp00_Int4;
                    VAR KeyRoot           : tsp00_Int4);
        EXTERNAL ;



      PROCEDURE
              gg06Cint6ToLine (VAR Cint6 : tgg90_Cint6;
                    VAR LineLen          : integer;
                    VAR Line             : tsp00_Line);
        EXTERNAL ;



        FUNCTION
              gg06IsNilTrans (VAR TransNo : tgg91_TransNo): boolean;
        EXTERNAL ;



        FUNCTION
              gg06SessionGetInt4 (VAR SessionNo : tgg91_SessionNo): tsp00_Int4;
        EXTERNAL ;



        PROCEDURE
              gg06SessionToLine (
                    VAR SessionNo : tgg91_SessionNo;
                    VAR LineLen : integer;
                    VAR Line    : tsp00_Line);
        EXTERNAL ;



        PROCEDURE
              gg06TransToLine (
                    VAR TransNo : tgg91_TransNo;
                    VAR LineLen : integer;
                    VAR Line    : tsp00_Line);
        EXTERNAL ;



      PROCEDURE
              g10mv   (
                    mod_id         : tsp_c6;
                    mod_intern_num : tsp_int4;
                    source_upb     : tsp_int4;
                    destin_upb     : tsp_int4;
                    VAR source     : tsp00_Buf;
                    source_pos     : tsp_int4;
                    VAR destin     : tsp00_KnlIdentifier;
                    destin_pos     : tsp_int4;
                    length         : tsp_int4;
                    VAR e          : tgg_basis_error);
        EXTERNAL ;



        PROCEDURE
              g10mv1  (
                    mod_id         : tsp_c6;
                    mod_intern_num : tsp_int4;
                    source_upb     : tsp_int4;
                    destin_upb     : tsp_int4;
                    VAR source     : tsp00_C120;
                    source_pos     : tsp_int4;
                    VAR destin     : tsp00_C256;
                    destin_pos     : tsp_int4;
                    length         : tsp_int4;
                    VAR e          : tgg_basis_error);
        EXTERNAL ;



        PROCEDURE
              g10mv2  (
                    mod_id         : tsp_c6;
                    mod_intern_num : tsp_int4;
                    source_upb     : tsp_int4;
                    destin_upb     : tsp_int4;
                    VAR source     : tgg00_RecBody;
                    source_pos     : tsp_int4;
                    VAR destin     : tsp00_Key;
                    destin_pos     : tsp_int4;
                    length         : tsp_int4;
                    VAR e          : tgg_basis_error);
        EXTERNAL ;



        PROCEDURE
              g10mv3  (
                    mod_id         : tsp_c6;
                    mod_intern_num : tsp_int4;
                    source_upb     : tsp_int4;
                    destin_upb     : tsp_int4;
                    VAR source     : tsp_moveobj;
                    source_pos     : tsp_int4;
                    VAR destin     : tsp00_C256;
                    destin_pos     : tsp_int4;
                    length         : tsp_int4;
                    VAR e          : tgg_basis_error);
        EXTERNAL ;



        PROCEDURE
              g10mv4  (
                    mod_id         : tsp_c6;
                    mod_intern_num : tsp_int4;
                    source_upb     : tsp_int4;
                    destin_upb     : tsp_int4;
                    VAR source     : tsp00_Buf;
                    source_pos     : tsp_int4;
                    VAR destin     : tkb05_LockShow;
                    destin_pos     : tsp_int4;
                    length         : tsp_int4;
                    VAR e          : tgg_basis_error);
        EXTERNAL ;



        PROCEDURE
              g10mv5  (
                    mod_id         : tsp_c6;
                    mod_intern_num : tsp_int4;
                    source_upb     : tsp_int4;
                    destin_upb     : tsp_int4;
                    VAR source     : tsp00_Line;
                    source_pos     : tsp_int4;
                    VAR destin     : tsp00_C20;
                    destin_pos     : tsp_int4;
                    length         : tsp_int4;
                    VAR e          : tgg_basis_error);
        EXTERNAL ;



        PROCEDURE
              g10mv6  (
                    mod_id         : tsp_c6;
                    mod_intern_num : tsp_int4;
                    source_upb     : tsp_int4;
                    destin_upb     : tsp_int4;
                    VAR source     : tgg00_RecBody;
                    source_pos     : tsp_int4;
                    VAR destin     : tsp00_C18;
                    destin_pos     : tsp_int4;
                    length         : tsp_int4;
                    VAR e          : tgg_basis_error);
        EXTERNAL ;



        PROCEDURE
              g10mv7  (
                    mod_id         : tsp_c6;
                    mod_intern_num : tsp_int4;
                    source_upb     : tsp_int4;
                    destin_upb     : tsp_int4;
                    VAR source     : tsp_c12;
                    source_pos     : tsp_int4;
                    VAR destin     : tsp_moveobj;
                    destin_pos     : tsp_int4;
                    length         : tsp_int4;
                    VAR e          : tgg_basis_error);
        EXTERNAL ;



        PROCEDURE
              g10mv8  (
                    mod_id         : tsp_c6;
                    mod_intern_num : tsp_int4;
                    source_upb     : tsp_int4;
                    destin_upb     : tsp_int4;
                    VAR source     : tsp_moveobj;
                    source_pos     : tsp_int4;
                    VAR destin     : tsp_c12;
                    destin_pos     : tsp_int4;
                    length         : tsp_int4;
                    VAR e          : tgg_basis_error);
        EXTERNAL ;



      PROCEDURE
              g17date_to_line (date : tsp00_Date;
                    VAR ln_len      : integer;
                    VAR ln          : tsp00_Line);
        EXTERNAL ;



        PROCEDURE
              g17intdate_time (
                    int_date     : tsp00_Int4;
                    int_time     : tsp00_Int4;
                    VAR str_date : tsp00_Date;
                    VAR str_time : tsp00_Time);
        EXTERNAL ;



        PROCEDURE
              g17int4to_line (
                    int       : tsp00_Int4;
                    with_zero : boolean;
                    int_len   : integer;
                    ln_pos    : integer;
                    VAR ln    : tsp00_C30);
        EXTERNAL ;



        PROCEDURE
              g17messtype_to_line (mess_type : tgg00_MessType;
                    VAR ln_len : integer;
                    VAR ln     : tsp00_Line);
        EXTERNAL ;



        PROCEDURE
              g17mess2type_to_line (mess2_type : tgg00_MessType2;
                    VAR ln_len : integer;
                    VAR ln     : tsp00_Line);
        EXTERNAL ;



        PROCEDURE
              g17pagetype_to_line (pagetype : tgg00_PageType;
                    VAR ln_len : integer;
                    VAR ln     : tsp00_Line);
        EXTERNAL ;



        PROCEDURE
              g17surrogate_to_line (VAR surrogate : tgg00_Surrogate;
                    VAR ln_len : integer;
                    VAR ln     : tsp00_Line);
        EXTERNAL ;



        PROCEDURE
              g17time_to_line (time : tsp00_Time;
                    VAR ln_len      : integer;
                    VAR ln          : tsp00_Line);
        EXTERNAL ;



        PROCEDURE
              g17trimint4_to_line (intval : tsp00_Int4;
                    VAR ln_len : integer;
                    VAR ln     : tsp00_Line);
        EXTERNAL ;



      FUNCTION
              Kernel_DiagInfop (
                    requested       : integer;
                    VAR kindOut     : tak_keyword;
                    VAR topicKeyOut : tsp00_Sname;
                    VAR levelOut    : integer) : boolean;
        EXTERNAL ;



      FUNCTION
              kb03IsBackupCheck     : boolean;
        EXTERNAL ;



        FUNCTION
              kb03IsLockCheck       : boolean;
        EXTERNAL ;



        FUNCTION
              kb03IsLockSupplyCheck : boolean;
        EXTERNAL ;



        FUNCTION
              kb03IsLogCheck        : boolean;
        EXTERNAL ;



        FUNCTION
              kb03IsLogQueueCheck   : boolean;
        EXTERNAL ;



        FUNCTION
              kb03IsRegionCheck     : boolean;
        EXTERNAL ;



        FUNCTION
              kb03IsTransCheck      : boolean;
        EXTERNAL ;


              (* PTS 1107118 E.Z. *)

      PROCEDURE
              k38get_autosave_medianame (
                    TaskId         : tsp00_TaskId;
                    VAR media_name : tsp00_C64);
        EXTERNAL ;



        FUNCTION
              k38is_on_autosave (
                    VAR t     : tgg00_TransContext;
                    in_region : boolean) : boolean;
        EXTERNAL ;



      PROCEDURE
              k51info_lock (
                    TaskId         : tsp00_TaskId;
                    VAR LockInfo   : tkb05_LockInfo);
        EXTERNAL ;



        FUNCTION
              k51is_closed_locklist (pid : tsp00_TaskId) : boolean;
        EXTERNAL ;



        PROCEDURE
              k51reset_statistics (pid : tsp00_TaskId);
        EXTERNAL ;



      PROCEDURE
              kb560GetHistFileInfo (VAR Trans : tgg00_TransContext;
                    VAR HistFileNo       : tsp00_Int4;
                    VAR FirstTransId     : tgg91_TransNo;
                    VAR FirstTransIsUsed : boolean;
                    VAR LastTransId      : tgg91_TransNo;
                    VAR LastTransIsUsed  : boolean;
                    VAR PageCount        : tsp00_Int4;
                    VAR bFileIsEmpty     : boolean);
        EXTERNAL ;



        FUNCTION
              kb560GetLogDeviceSize : tsp00_Int4;
        EXTERNAL ;



        PROCEDURE
              kb560GetStatistics (taskid : tsp00_TaskId;
                    VAR LogDeviceInfo    : tkb00_LogDeviceInfo;
                    VAR LogQueueInfo     : tkb00_LogQueueInfo);
        EXTERNAL ;



        FUNCTION
              kb560IsLogDeviceEnabled : boolean;
        EXTERNAL ;



        PROCEDURE
              kb560ResetStatistics (taskid : tsp00_TaskId);
        EXTERNAL ;



        FUNCTION
              kb560LogIsFull (TaskId : tsp00_TaskId) : boolean;
        EXTERNAL ;



      PROCEDURE
              k57statistics_reset (process_id : tsp00_TaskId);
        EXTERNAL ;



        PROCEDURE
              k57info_seq_cache (
                    process_id      : tsp00_TaskId;
                    VAR cache_total : tsp00_8ByteCounter;
                    VAR cache_hit   : tsp00_8ByteCounter;
                    VAR cache_miss  : tsp00_8ByteCounter);
        EXTERNAL ;



        PROCEDURE
              k57select_restartrec (TaskId : tsp00_TaskId;
                    pAcv  : tsp00_Addr;
                    pA41v : tsp00_Addr);
        EXTERNAL ;



        PROCEDURE
              k57select_log_info (TaskId : tsp00_TaskId;
                    pAcv  : tsp00_Addr;
                    pA41v : tsp00_Addr);
        EXTERNAL ;



      PROCEDURE
              k720init_ref_statistic;
        EXTERNAL ;



        PROCEDURE
              k720show_monitor (VAR info : tgg00_ReferenceInfo);
        EXTERNAL ;



      PROCEDURE
              a11get_check_table (
                    VAR acv          : tak_all_command_glob;
                    new_table        : boolean;
                    basetable        : boolean;
                    unload_allowed   : boolean;
                    required_priv    : tgg_privilege_set;
                    any_priv         : boolean;
                    all_base_rec     : boolean;
                    d_state          : tak_directory_state;
                    VAR act_tree_ind : tsp_int4;
                    VAR authid       : tsp_knl_identifier;
                    VAR tablen       : tsp_knl_identifier;
                    VAR d_sparr      : tak_syspointerarr);
        EXTERNAL ;



      FUNCTION
              a21is_owner (
                    VAR acv     : tak_all_command_glob;
                    VAR userrec : tak_userrecord) : boolean;
        EXTERNAL ;



      FUNCTION
              a260BuildDebugStack : boolean;
        EXTERNAL ;

 (* PTS 1107879 & PTS 1112663 *)

        PROCEDURE
              a260reset_statistics;
        EXTERNAL ;



        PROCEDURE
              a260statistics (
                    VAR external_proc_cnt   : tsp00_8ByteCounter;
                    VAR internal_proc_cnt   : tsp00_8ByteCounter);
        EXTERNAL ;



      PROCEDURE
              a262reset_statistics;
        EXTERNAL ;



        PROCEDURE
              a262statistics (
                    VAR external_trigger_cnt : tsp00_8ByteCounter;
                    VAR internal_trigger_cnt : tsp00_8ByteCounter);
        EXTERNAL ;



      PROCEDURE
              a24init_index_scan (
                    VAR acv            : tak_all_command_glob;
                    VAR tabid          : tgg_surrogate;
                    VAR index_scan_rec : tak_index_scan_record);
        EXTERNAL ;



        PROCEDURE
              a24finish_index_scan (
                    VAR acv            : tak_all_command_glob;
                    VAR index_scan_rec : tak_index_scan_record);
        EXTERNAL ;



        PROCEDURE
              a24fnd_indexno (
                    VAR acv            : tak_all_command_glob;
                    VAR tabid          : tgg_surrogate;
                    indexno            : integer;
                    VAR index_scan_rec : tak_index_scan_record);
        EXTERNAL ;



        PROCEDURE
              a24find_indexname (
                    VAR acv            : tak_all_command_glob;
                    VAR tabid          : tgg_surrogate;
                    VAR indexname      : tsp_knl_identifier;
                    VAR index_scan_rec : tak_index_scan_record);
        EXTERNAL ;



        PROCEDURE
              a24get_indexname (
                    VAR acv        : tak_all_command_glob;
                    indexbuf       : tak_sysbufferaddress;
                    index          : integer;
                    VAR index_name : tsp_knl_identifier);
        EXTERNAL ;



        FUNCTION
              a24next_named_index (
                    VAR acv            : tak_all_command_glob;
                    VAR index_scan_rec : tak_index_scan_record) : boolean;
        EXTERNAL ;



      PROCEDURE
              a28sys_upd_statistics (
                    VAR acv     : tak_all_command_glob;
                    VAR tree    : tgg00_FileId;
                    mtype       : tgg_message_type;
                    m2type      : tgg_message2_type;
                    known_pages : tsp_int4;
                    found_pages : tsp_int4);
        EXTERNAL ;



      PROCEDURE
              ak341OmsHeapInfo (VAR acv : tak_all_command_glob;
                    VAR totalHeap             : tsp00_8ByteCounter;
                    VAR freeSpaceInFreeChunks : tsp00_8ByteCounter;
                    VAR reserved              : tsp00_8ByteCounter;
                    VAR mallocated            : tsp00_8ByteCounter;
                    VAR emergencySize         : tsp00_8ByteCounter;
                    VAR emergencyInUse        : tsp00_8ByteCounter;
                    VAR emergencyMaxUsed      : tsp00_8ByteCounter);
        EXTERNAL ;



        FUNCTION
              ak341GetMonitorInfo (
                    VAR acv     : tak_all_command_glob;
                    VAR handle  : tsp00_Addr;
                    VAR iid     : tsp00_C16;
                    VAR disid   : tsp00_Int4;
                    VAR moninfo : tgg00_COMMonitorInfo) : boolean;
        EXTERNAL ;



        PROCEDURE
              ak341GetObjFileInfo (
                    VAR Trans           : tgg00_TransContext;
                    VAR ObjFile         : tgg00_FileId;
                    VAR NumAllPages     : tsp00_Int4;
                    VAR NumFreePages    : tsp00_Int4;
                    VAR NumEmptyPages   : tsp00_Int4;
                    (* PTS 1107153, T.A. 17.07.2000 *)
                    VAR NumKeyLeafPages : tsp00_Int4;
                    (* change request 1000139, T.A. 14.10.1998 *)
                    VAR Occupancy       : tsp00_Int4);
        EXTERNAL ;



        PROCEDURE
              ak341MonitorHeapContent ( VAR acv     : tak_all_command_glob;
                    VAR a41v : tak40_show_glob);
        EXTERNAL ;



        FUNCTION
              ak341IsMonitorOn : boolean;
        EXTERNAL ;



        FUNCTION
              ak341NextOmsLockObjInfo(VAR pHandle : tsp00_Addr;
                    VAR lockInfo : tgg00_OmsLockInfo) : boolean;
        EXTERNAL ;



        PROCEDURE
              ak341ResetMonitor(VAR acv : tak_all_command_glob);
        EXTERNAL ;



        PROCEDURE
              ak341StartStopMonitor(doStart : boolean);
        EXTERNAL ;



      PROCEDURE
              a36get_catalog_record (
                    VAR t      : tgg00_TransContext;
                    VAR sysk   : tgg_sysinfokey;
                    VAR sysbuf : tak_systembuffer);
        EXTERNAL ;



        PROCEDURE
              a36restart_time (
                    VAR date : tsp_int4;
                    VAR time : tsp_int4);
        EXTERNAL ;



      PROCEDURE
              a37event_state (
                    VAR acv  : tak_all_command_glob;
                    VAR a41v : tak40_show_glob);
        EXTERNAL ;



      PROCEDURE
              a40get_catalog_table (
                    VAR acv  : tak_all_command_glob;
                    VAR a41v : tak40_show_glob);
        EXTERNAL ;



        PROCEDURE
              a40move (
                    VAR acv     : tak_all_command_glob;
                    VAR a41v    : tak40_show_glob;
                    moveobj_ptr : tsp_moveobj_ptr;
                    move_len    : integer);
        EXTERNAL ;



        PROCEDURE
              a40move_const (
                    VAR acv     : tak_all_command_glob;
                    VAR a41v    : tak40_show_glob;
                    moveobj_ptr : tsp_moveobj_ptr;
                    move_len    : integer);
        EXTERNAL ;



        PROCEDURE
              a40move_counter (
                    VAR acv       : tak_all_command_glob;
                    VAR a41v      : tak40_show_glob;
                    val           : tsp00_8ByteCounter;
                    is_null       : boolean;
                    frac          : integer;
                    zero_to_undef : boolean);
        EXTERNAL ;



        PROCEDURE
              a40move_guid (
                    VAR acv     : tak_all_command_glob;
                    VAR a41v    : tak40_show_glob;
                    VAR guid    : tsp_c16);
        EXTERNAL ;



        PROCEDURE
              a40move_i4 (
                    VAR acv       : tak_all_command_glob;
                    VAR a41v      : tak40_show_glob;
                    i4_val        : tsp_int4;
                    is_null       : boolean;
                    frac          : integer;
                    zero_to_undef : boolean);
        EXTERNAL ;



        PROCEDURE
              a40move_real (
                    VAR acv       : tak_all_command_glob;
                    VAR a41v      : tak40_show_glob;
                    val           : tsp_longreal;
                    is_null       : boolean;
                    frac          : integer);
        EXTERNAL ;



        PROCEDURE
              a40put_date_time (
                    VAR acv      : tak_all_command_glob;
                    VAR a41v     : tak40_show_glob;
                    VAR datetime : tsp_int4;
                    date         : boolean;
                    is_undef     : boolean);
        EXTERNAL ;



        PROCEDURE
              a40put_one_value (
                    VAR acv      : tak_all_command_glob;
                    VAR a41v     : tak40_show_glob;
                    VAR col_info : tak00_columninfo;
                    do_move      : boolean;
                    VAR valbuf   : tkb05_RowId;
                    valpos       : integer;
                    vallen       : integer;
                    VAR destbuf  : tsp00_C120;
                    VAR destlen  : integer;
                    VAR ok       : boolean);
        EXTERNAL ;



        PROCEDURE
              a40init_table_scan (
                    VAR acv          : tak_all_command_glob;
                    VAR a41v         : tak40_show_glob;
                    scan_temp        : boolean;
                    scan_private     : boolean;
                    scan_non_private : boolean;
                    scan_public      : boolean;
                    use_synonyms     : boolean;
                    all_base         : boolean);
        EXTERNAL ;



        PROCEDURE
              a40ignore_down_error (VAR acv : tak_all_command_glob);
        EXTERNAL ;



        PROCEDURE
              a40table_column_to_shbuf (
                    VAR acv   : tak_all_command_glob;
                    VAR a41v  : tak40_show_glob;
                    base_ptr  : tak_sysbufferaddress;
                    colindex  : integer);
        EXTERNAL ;



        PROCEDURE
              a40username_from_syskey (
                    VAR sysk      : tgg_sysinfokey;
                    VAR user_name : tsp_knl_identifier);
        EXTERNAL ;



        FUNCTION
              a40next_table (
                    VAR acv  : tak_all_command_glob;
                    VAR a41v : tak40_show_glob) : boolean;
        EXTERNAL ;



      PROCEDURE
              a51build_userkey (
                    VAR user_name : tsp_knl_identifier;
                    VAR userkey : tgg_sysinfokey);
        EXTERNAL ;



      PROCEDURE
              a544get_analyze_filter (VAR state : tsp00_C64);
        EXTERNAL ;



      FUNCTION
              a545sm_get_rowno : tsp_int4;
        EXTERNAL ;



      FUNCTION
              a722test_col_qual (
                    VAR acv   : tak_all_command_glob;
                    VAR a41v  : tak40_show_glob;
                    colname   : tak_oldidentifier;
                    def_byte  : char;
                    value_ptr : tsp_moveobj_ptr;
                    value_len : integer) : boolean;
        EXTERNAL ;



      FUNCTION
              a10BaseRecPersistentOffset : integer;
        EXTERNAL ;



        FUNCTION
              a10cache_used : tsp_int4;
        EXTERNAL ;



        PROCEDURE
              a10get_sysinfo (
                    VAR acv      : tak_all_command_glob;
                    VAR syskey   : tgg_sysinfokey;
                    dstate       : tak_directory_state;
                    VAR syspoint : tak_sysbufferaddress;
                    VAR b_err    : tgg_basis_error);
        EXTERNAL ;



        PROCEDURE
              a10next_sysinfo (
                    VAR acv      : tak_all_command_glob;
                    VAR syskey   : tgg_sysinfokey;
                    key_prefix   : integer;
                    dstate       : tak_directory_state;
                    rec_kind     : tsp_c2;
                    VAR syspoint : tak_sysbufferaddress;
                    VAR b_err    : tgg_basis_error);
        EXTERNAL ;



        PROCEDURE
              a10_key_del  (
                    VAR acv    : tak_all_command_glob;
                    VAR syskey : tgg_sysinfokey);
        EXTERNAL ;



      PROCEDURE
              a06check_username (
                    VAR acv    : tak_all_command_glob;
                    VAR auth   : tsp_knl_identifier;
                    VAR is_dba : boolean;
                    VAR ok     : boolean);
        EXTERNAL ;



        PROCEDURE
              a06colname_retpart_move (
                    VAR acv     : tak_all_command_glob;
                    moveobj_ptr : tsp_moveobj_ptr;
                    move_len    : tsp_int4;
                    src_codeset : tsp_int2);
        EXTERNAL ;



        PROCEDURE
              a06determine_username (
                    VAR acv       : tak_all_command_glob;
                    VAR userid    : tgg_surrogate;
                    VAR user_name : tsp_knl_identifier);
        EXTERNAL ;



        PROCEDURE
              a06get_page_cnt (
                    VAR acv          : tak_all_command_glob;
                    VAR file_id      : tgg00_FileId;
                    do_sample        : boolean;
                    VAR page_count   : tsp_int4;
                    VAR b_err        : tgg_basis_error);
        EXTERNAL ;



        PROCEDURE
              a06finish_curr_retpart (
                    VAR acv   : tak_all_command_glob;
                    part_kind : tsp1_part_kind;
                    arg_count : tsp_int2);
        EXTERNAL ;



        PROCEDURE
              a06ignore_rsend_mess_buf (
                    VAR acv              : tak_all_command_glob;
                    VAR mbuf             : tgg_mess_block;
                    VAR send_description : tak_sm_description;
                    VAR e                : tgg_basis_error);
        EXTERNAL ;



        PROCEDURE
              a06rsend_mess_buf (
                    VAR acv     : tak_all_command_glob;
                    VAR mblock  : tgg_mess_block;
                    result_req  : boolean;
                    VAR b_err   : tgg_basis_error);
        EXTERNAL ;



        PROCEDURE
              a06retpart_move (
                    VAR acv     : tak_all_command_glob;
                    moveobj_ptr : tsp_moveobj_ptr;
                    move_len    : tsp_int4);
        EXTERNAL ;



        PROCEDURE
              a06set_mtree (
                    VAR acv    : tak_all_command_glob;
                    VAR mblock : tgg_mess_block;
                    VAR tree   : tgg00_FileId);
        EXTERNAL ;



        PROCEDURE
              a06_systable_get (
                    VAR acv      : tak_all_command_glob;
                    dstate       : tak_directory_state;
                    VAR tableid  : tgg_surrogate;
                    VAR base_ptr : tak_sysbufferaddress;
                    get_all      : boolean;
                    VAR ok       : boolean);
        EXTERNAL ;



      FUNCTION
              a061exist_columnname (
                    VAR base_rec    : tak_baserecord;
                    VAR column      : tsp_knl_identifier;
                    VAR colinfo_ptr : tak00_colinfo_ptr) : boolean;
        EXTERNAL ;



      PROCEDURE
              a05_identifier_get (
                    VAR acv     : tak_all_command_glob;
                    tree_index  : integer;
                    obj_len     : integer;
                    VAR moveobj : tsp00_KnlIdentifier);
        EXTERNAL ;



      PROCEDURE
              bd01DataCacheSize (
                    VAR TotalPages : tsp00_Int4;
                    VAR UnMapPages : tsp00_Int4);
        EXTERNAL ;



        FUNCTION
              bd01ConverterSize : tsp00_Int4;
        EXTERNAL ;



        FUNCTION
              b01connect_allowed : boolean;
        EXTERNAL ;



        FUNCTION
              b01db_full( taskId : tsp00_TaskId ) : boolean;
        EXTERNAL ;



        PROCEDURE
              b01dbspace_statistic  (
                    VAR t    : tgg00_TransContext;
                    VAR info : tgg00_DbSpaceInfo);
        EXTERNAL ;



        FUNCTION
              b01devsize (
                    dev_type : tsp2_dev_type;
                    devno : tsp00_Int4) : tsp00_Int4;
        EXTERNAL ;



        PROCEDURE
              b01empty_file (
                    VAR t       : tgg00_TransContext;
                    VAR file_id : tgg00_FileId);
        EXTERNAL ;



        FUNCTION
              b01get_bad_index_cnt (pid : tsp00_TaskId) : tsp00_Int4;
        EXTERNAL ;



        PROCEDURE
              b01get_data_cache_workload (
                    VAR t                  : tgg00_TransContext;
                    VAR DataCacheSize      : tsp00_Int4;
                    VAR OmsHistoryPageCnt  : tsp00_Int4;
                    VAR OmsDataPageCnt     : tsp00_Int4;
                    VAR SqlDataPageCnt     : tsp00_Int4);
        EXTERNAL ;



        PROCEDURE
              b01get_devname (
                    dev_type    : tsp2_dev_type;
                    devno       : tsp00_Int4;
                    VAR devname : tsp2_devname);
        EXTERNAL ;



        PROCEDURE
              b01next_bad_dev (
                    pid           : tsp00_TaskId;
                    VAR dev_type  : tsp2_dev_type;
                    VAR devno     : tsp00_Int4;
                    VAR devname   : tsp2_devname;
                    VAR realy_bad : boolean);
        EXTERNAL ;



        PROCEDURE
              b01next_dev_info (
                    TaskId              : tsp00_TaskId;
                    VAR dev_no          : tsp00_Int4;
                    VAR dev_type        : tsp2_dev_type;
                    VAR dev_name        : tsp2_devname;
                    VAR dev_size        : tsp00_Int4;
                    VAR dev_used_blocks : tsp00_Int4;
                    VAR dev_online      : boolean;
                    VAR dev_state       : tsp00_C18);
        EXTERNAL ;



        PROCEDURE
              b01next_fdir_entry (
                    VAR t       : tgg00_TransContext;
                    VAR file_id : tgg00_FileId);
        EXTERNAL ;



        FUNCTION
              b01no_of_data_devs : tsp00_Int4;
        EXTERNAL ;



        FUNCTION
              b01no_of_log_devs : tsp00_Int4;
        EXTERNAL ;



        PROCEDURE
              b01show_monitor (
                    TaskId   : tsp00_TaskId;
                    VAR info : tgg00_ReferenceInfo);
        EXTERNAL ;



        PROCEDURE
              b01reset_monitor (part : tgg00_MonitorPart);
        EXTERNAL ;



        PROCEDURE
              b01destroy_file (
                    VAR t       : tgg00_TransContext;
                    VAR file_id : tgg00_FileId);
        EXTERNAL ;



      PROCEDURE
              b02next_record (
                    VAR t       : tgg00_TransContext;
                    VAR file_id : tgg00_FileId;
                    VAR rk      : tgg00_Lkey;
                    inclusive   : boolean;
                    VAR b       : tgg00_Rec);
        EXTERNAL ;



      PROCEDURE
              b07cadd_record (
                    VAR t       : tgg00_TransContext;
                    VAR file_id : tgg00_FileId;
                    VAR b       : tgg00_Rec);
        EXTERNAL ;



        PROCEDURE
              b07cget_record (
                    VAR t       : tgg00_TransContext;
                    VAR file_id : tgg00_FileId;
                    VAR rk      : tgg00_Lkey;
                    VAR b       : tgg00_Rec);
        EXTERNAL ;



        PROCEDURE
              b07cnext_record (
                    VAR t          : tgg00_TransContext;
                    VAR file_id    : tgg00_FileId;
                    VAR rk         : tgg00_Lkey;
                    VAR set_result : tgg00_BdSetResultRecord;
                    VAR tree_pos   : tgg00_FilePos;
                    VAR b          : tsp00_Buf);
        EXTERNAL ;



        PROCEDURE
              b07crepl_record (
                    VAR t       : tgg00_TransContext;
                    VAR file_id : tgg00_FileId;
                    VAR b       : tgg00_Rec);
        EXTERNAL ;



      PROCEDURE
              s10fil (
                    size     : tsp_int4;
                    VAR m    : tsp00_C120;
                    pos      : tsp_int4;
                    len      : tsp_int4;
                    fillchar : char);
        EXTERNAL ;



        PROCEDURE
              s10fil1 (
                    size     : tsp_int4;
                    VAR m    : tsp00_Buf;
                    pos      : tsp_int4;
                    len      : tsp_int4;
                    fillchar : char);
        EXTERNAL ;



        PROCEDURE
              s10fil2 (
                    size     : tsp_int4;
                    VAR m    : tsp_moveobj;
                    pos      : tsp_int4;
                    len      : tsp_int4;
                    fillchar : char);
        EXTERNAL ;



        PROCEDURE
              s10mv1  (
                    size1    : tsp_int4;   size2 : tsp_int4;
                    VAR b1   : tsp00_Line;   pos1 : tsp_int4;
                    VAR b2   : tsp00_Sname;  pos2 : tsp_int4;
                    move_len : tsp_int4);
        EXTERNAL ;



      FUNCTION
              s20or4b (
                    VAR source    : tsp_buf;
                    source_pos    : tsp_int4) : tsp_int4;
        EXTERNAL ;



      PROCEDURE
              s40gbyte (
                    VAR buf        : tsp00_C120;
                    pos            : tsp_int4;
                    len            : integer;
                    VAR dest       : tsp00_C256;
                    dpos           : tsp_int4;
                    dlen           : integer;
                    VAR truncated  : boolean);
        EXTERNAL ;



      PROCEDURE
              s41plint (
                    VAR buf : tsp00_Number;
                    pos     : tsp_int4;
                    len     : integer;
                    frac    : integer;
                    source  : tsp_int4;
                    VAR res : tsp_num_error);
        EXTERNAL ;



        PROCEDURE
              s41p4int (
                    VAR buf : tsp00_ResNum;
                    pos     : tsp_int4;
                    source  : tsp_int4;
                    VAR res : tsp_num_error);
        EXTERNAL ;



      FUNCTION
              g01cache_size (cache_type : tgg_cache_type) : tsp_int4;
        EXTERNAL ;



        PROCEDURE
              g01datapart_init (
                    VAR mblock    : tgg_mess_block;
                    datapart_ptr  : tgg_datapart_ptr;
                    datapart_size : tsp_int4);
        EXTERNAL ;



        FUNCTION
              g01is_livecache : boolean;
        EXTERNAL ;



        PROCEDURE
              g01log_mode (VAR nam  : tsp00_Sname);
        EXTERNAL ;



        FUNCTION
              g01maxcpu : tsp_int4;
        EXTERNAL ;



        FUNCTION
              g01maxlocks : tsp_int4;
        EXTERNAL ;



        FUNCTION
              g01maxobjgarbagecoll : tsp_int4;
        EXTERNAL ;



        FUNCTION
              g01maxuser : tsp_int4;
        EXTERNAL ;



        PROCEDURE
              g01mblock_init (
                    VAR source_trans : tgg00_TransContext;
                    mess_type  : tgg_message_type;
                    mess2_type : tgg_message2_type;
                    VAR mblock : tgg_mess_block);
        EXTERNAL ;



        PROCEDURE
              g01qual_init (
                    VAR mblock : tgg_mess_block;
                    qual_ptr   : tgg_qual_buf_ptr;
                    qual_size  : tsp_int4);
        EXTERNAL ;



        PROCEDURE
              g01stack_init (
                    VAR mblock : tgg_mess_block;
                    stack_ptr  : tgg_stack_list_ptr;
                    stack_size : tsp_int4;
                    qual_ptr   : tgg_qual_buf_ptr;
                    qual_size  : tsp_int4);
        EXTERNAL ;



        PROCEDURE
              g01allocate_msg (
                    msg_label  : tsp_c8;
                    msg_text   : tsp_c24;
                    alloc_size : tsp_int4);
        EXTERNAL ;



      PROCEDURE
              g04build_temp_tree_id (
                    VAR curr : tgg00_FileId;
                    VAR t : tgg00_TransContext);
        EXTERNAL ;



      PROCEDURE
              g11kernel_version  (VAR vers : tsp_version);
        EXTERNAL ;



      PROCEDURE
              g20unifill (
                    size      : tsp_int4;
                    m         : tsp_moveobj_ptr;
                    pos       : tsp_int4;
                    len       : tsp_int4;
                    filluchar : tsp_c2);
        EXTERNAL ;



      PROCEDURE
              vgetapplpid (
                    pid           : tsp_process_id;
                    VAR appl_node : tsp_nodeid;
                    VAR appl_pid  : tsp_int4;
                    VAR ok        : boolean);
        EXTERNAL ;



        PROCEDURE
              vversion (
                    VAR rdsversion : tsp_version;
                    VAR lzuversion : tsp_version);
        EXTERNAL ;



        PROCEDURE
              vdcom_dbproc (VAR cdp : tsp_dcom_dispatch_parms);
        EXTERNAL ;



        PROCEDURE  (* Change Request 1000139 T.A. 14.10.1998 *)
              vdcom_monitor_open (VAR params : tsp_dcom_monitor_parms);
        EXTERNAL ;



        PROCEDURE (* Change Request 1000139 T.A. 14.10.1998 *)
              vdcom_monitor_next (VAR params : tsp_dcom_monitor_parms);
        EXTERNAL ;



        PROCEDURE (* Change Request 1000139 T.A. 14.10.1998 *)
              vdcom_monitor_close (VAR params : tsp_dcom_monitor_parms);
        EXTERNAL ;



        PROCEDURE
              vallocat (
                    length     : tsp_int4;
                    VAR p      : tsp_moveobj_ptr;
                    VAR ok     : boolean);
        EXTERNAL ;



        PROCEDURE
              vgetfirstuserpid (VAR pid : tsp_process_id);
        EXTERNAL ;



        FUNCTION
              RTEMem_AWEAvailable : boolean;
        EXTERNAL ;



      PROCEDURE
              a07ak_system_error (
                    VAR acv  : tak_all_command_glob;
                    modul_no : integer;
                    id       : integer);
        EXTERNAL ;



        PROCEDURE
              a07_b_put_error (
                    VAR acv : tak_all_command_glob;
                    b_err   : tgg_basis_error;
                    err_code : tsp_int4);
        EXTERNAL ;



        PROCEDURE
              a07_nb_put_error (
                    VAR acv : tak_all_command_glob;
                    b_err   : tgg_basis_error;
                    err_code : tsp_int4;
                    VAR n    : tsp_knl_identifier);
        EXTERNAL ;



        FUNCTION
              a07_return_code (
                    b_err    : tgg_basis_error;
                    sqlmode  : tsp_sqlmode) : tsp_int2;
        EXTERNAL ;



      PROCEDURE
              t01int4 (
                    debug    : tgg00_Debug;
                    nam      : tsp00_Sname;
                    int      : tsp00_Int4);
        EXTERNAL ;



        PROCEDURE
              t01lidentifier (
                    debug : tgg00_Debug;
                    id : tsp00_KnlIdentifier);
        EXTERNAL ;



        PROCEDURE
              t01buf (
                    debug    : tgg00_Debug;
                    VAR buf  : tsp00_Buf;
                    startpos : integer;
                    endpos   : integer);
        EXTERNAL ;



PROCEDURE cccprint(tt:tsp_trace);EXTERNAL;

PROCEDURE
      a42execute (
            VAR acv  : tak_all_command_glob;
            VAR a41v : tak40_show_glob);
        FORWARD;

PROCEDURE
      a42get_tablename (
            VAR t      : tgg00_TransContext;
            VAR tabid  : tgg00_Surrogate;
            VAR authid : tsp00_KnlIdentifier;
            VAR tablen : tsp00_KnlIdentifier;
            VAR b_err  : tgg00_BasisError);
        FORWARD;

PROCEDURE
      a42get_username (
            VAR t        : tgg00_TransContext;
            VAR userid   : tgg00_UserId;
            VAR username : tsp00_KnlIdentifier;
            VAR b_err    : tgg00_BasisError);
        FORWARD;

PROCEDURE
      a42_get_pagecount (
            VAR acv       : tak_all_command_glob;
            base_ptr      : tak_sysbufferaddress;
            VAR pagecount : tsp00_Int4);
        FORWARD;

PROCEDURE
      a42_init_time_array;
        FORWARD;

PROCEDURE
      a42monitor_init (procid : tsp00_TaskId);
        FORWARD;

PROCEDURE
      a42_start_semantic (VAR acv : tak_all_command_glob);
        FORWARD;

PROCEDURE
      ak42ctrl_config (
            VAR acv  : tak_all_command_glob;
            VAR a41v : tak40_show_glob);
        FORWARD;

PROCEDURE
      ak42ctrl_devspaces (
            VAR acv  : tak_all_command_glob;
            VAR a41v : tak40_show_glob);
        FORWARD;

PROCEDURE
      ak42ctrl_io_statistics (
            VAR acv  : tak_all_command_glob;
            VAR a41v : tak40_show_glob);
        FORWARD;

PROCEDURE
      ak42ctrl_locks (
            VAR acv  : tak_all_command_glob;
            VAR a41v : tak40_show_glob);
        FORWARD;

PROCEDURE
      ak42ctrl_log_statistics (
            VAR acv  : tak_all_command_glob;
            VAR a41v : tak40_show_glob);
        FORWARD;

PROCEDURE
      ak42ctrl_caches_statistics (
            VAR acv  : tak_all_command_glob;
            VAR a41v : tak40_show_glob);
        FORWARD;

PROCEDURE
      ak42out_monitor (
            VAR acv   : tak_all_command_glob;
            VAR a41v  : tak40_show_glob;
            VAR info  : tgg00_ReferenceInfo;
            ind       : tgg00_RefInfoIndex);
        FORWARD;

PROCEDURE
      ak42data_cache_statistics (
            VAR acv   : tak_all_command_glob;
            VAR a41v  : tak40_show_glob);
        FORWARD;

FUNCTION
      ak42connected_users (VAR acv : tak_all_command_glob;
            VAR a41v : tak40_show_glob) : integer;
        FORWARD;

PROCEDURE
      ak42datacache_hits (VAR acv   : tak_all_command_glob;
            VAR datacache_hitrate         : tsp00_Int4;
            VAR datacache_oms_hitrate     : tsp00_Int4;
            VAR datacache_oms_log_hitrate : tsp00_Int4);
        FORWARD;

PROCEDURE
      ak42dbm_state (VAR acv   : tak_all_command_glob;
            VAR a41v  : tak40_show_glob);
        FORWARD;

PROCEDURE
      ak42HeapContent (VAR acv   : tak_all_command_glob;
            VAR a41v  : tak40_show_glob);
        FORWARD;

PROCEDURE
      ak42history_info (
            VAR acv   : tak_all_command_glob;
            VAR a41v  : tak40_show_glob);
        FORWARD;

PROCEDURE
      ak42monitor_object(
            VAR acv   : tak_all_command_glob;
            kw_index  : integer);
        FORWARD;

PROCEDURE
      ak42monitor_show (
            VAR acv   : tak_all_command_glob;
            VAR a41v  : tak40_show_glob;
            kw_index  : integer);
        FORWARD;

PROCEDURE
      a42_monitor_init (process_id : tsp00_TaskId);
        FORWARD;

PROCEDURE
      ak42monitor_oms (
            VAR acv          : tak_all_command_glob;
            VAR a41v         : tak40_show_glob;
            reset_statistics : boolean);
        FORWARD;

PROCEDURE
      ak42AggrMethodMonitorInfo (
            VAR acv     : tak_all_command_glob;
            VAR iid     : tsp00_C16;
            VAR dispid  : integer;
            VAR monInfo : tgg00_COMMonitorInfo);
        FORWARD;

PROCEDURE
      ak42MonitorOmsBuildUserResult(
            VAR acv          : tak_all_command_glob;
            VAR a41v         : tak40_show_glob);
        FORWARD;

PROCEDURE
      ak42ResetOmsMonitor(VAR acv : tak_all_command_glob);
        FORWARD;

PROCEDURE
      ak42init_running_cmd_list;
        FORWARD;

PROCEDURE
      a42put_running_cmds (
            VAR acv    : tak_all_command_glob;
            VAR parsid : tsp_c12);
        FORWARD;

FUNCTION
      a42getparseid (
            VAR t        : tgg00_TransContext;
            VAR pid      : tsp_process_id;
            VAR parsid   : tsp_c12;
            VAR b_err    : tgg00_BasisError): boolean;
        FORWARD;

PROCEDURE
      a42reset_running_cmds (VAR acv : tak_all_command_glob);
        FORWARD;

PROCEDURE
      ak42oms_memory (
            VAR acv  : tak_all_command_glob;
            VAR a41v : tak40_show_glob);
        FORWARD;

PROCEDURE (* PTS 1110315 *)
      ak42OmsLockObjInfo (VAR acv : tak_all_command_glob;
            VAR a41v : tak40_show_glob);
        FORWARD;

PROCEDURE
      ak42show_running_cmds (
            VAR acv  : tak_all_command_glob;
            VAR a41v : tak40_show_glob);
        FORWARD;

PROCEDURE
      ak42add_parseid (
            VAR acv     : tak_all_command_glob;
            VAR a41v    : tak40_show_glob;
            VAR tree    : tgg00_FileId);
        FORWARD;

PROCEDURE
      ak42consistent_view (
            VAR acv     : tak_all_command_glob;
            VAR a41v    : tak40_show_glob);
        FORWARD;

PROCEDURE
      ak42roots (
            VAR acv  : tak_all_command_glob;
            VAR a41v : tak40_show_glob);
        FORWARD;

PROCEDURE
      ak42files (
            VAR acv  : tak_all_command_glob;
            VAR a41v : tak40_show_glob);
        FORWARD;

PROCEDURE
      ak42InfoGCStats (
            VAR acv  : tak_all_command_glob;
            VAR a41v : tak40_show_glob);
        FORWARD;

PROCEDURE   (* PTS 1115641 *)
      ak42_container_roots (
            VAR acv  : tak_all_command_glob;
            VAR a41v : tak40_show_glob);
        FORWARD;

PROCEDURE   (* PTS 1115641 FF *)
      ak42_container_chains (
            VAR acv  : tak_all_command_glob;
            VAR a41v : tak40_show_glob);
        FORWARD;

PROCEDURE   (* PTS 1115641 FF *)
      ak42_container_keys (
            VAR acv  : tak_all_command_glob;
            VAR a41v : tak40_show_glob);
        FORWARD;

PROCEDURE
      ak42select_lock_trans (
            VAR acv  : tak_all_command_glob;
            VAR a41v : tak40_show_glob);
        FORWARD;

PROCEDURE
      ak42one_lock_info (
            VAR acv       : tak_all_command_glob;
            VAR a41v      : tak40_show_glob;
            VAR lock_info : tkb05_LockShow);
        FORWARD;

PROCEDURE
      ak42appl_info (
            VAR acv    : tak_all_command_glob;
            VAR a41v   : tak40_show_glob;
            process_id : tsp00_TaskId);
        FORWARD;

PROCEDURE
      ak42lock_mode (
            VAR acv   : tak_all_command_glob;
            VAR a41v  : tak40_show_glob;
            lock_mode : tkb05_TransLockMode);
        FORWARD;

PROCEDURE
      ak42lock_state (
            VAR acv   : tak_all_command_glob;
            VAR a41v  : tak40_show_glob;
            lock_mode : tkb05_TransLockMode);
        FORWARD;

PROCEDURE
      ak42lock_req_mode (
            VAR acv   : tak_all_command_glob;
            VAR a41v  : tak40_show_glob;
            lock_mode : tgg00_LockReqMode);
        FORWARD;

PROCEDURE
      ak42lock_req_state (
            VAR acv    : tak_all_command_glob;
            VAR a41v   : tak40_show_glob;
            lock_state : tgg00_LockReqState);
        FORWARD;

PROCEDURE
      ak42serverdb_statistics (
            VAR acv  : tak_all_command_glob;
            VAR a41v : tak40_show_glob);
        FORWARD;

PROCEDURE
      ak42state_statistics (
            VAR acv  : tak_all_command_glob;
            VAR a41v : tak40_show_glob);
        FORWARD;

PROCEDURE
      ak42ref_i4_statistic (
            VAR acv  : tak_all_command_glob;
            VAR a41v : tak40_show_glob;
            textpar  : tsp00_C40;    (* called with constants *)
            lint     : tsp00_Int4);
        FORWARD;

PROCEDURE
      ak42ref_i4_stat_undef (
            VAR acv    : tak_all_command_glob;
            VAR a41v   : tak40_show_glob;
            textpar    : tsp00_C40;  (* called with constants *)
            lint       : tsp00_Int4;
            zero_undef : boolean);
        FORWARD;

PROCEDURE
      ak42ref_stat_real (
            VAR acv    : tak_all_command_glob;
            VAR a41v   : tak40_show_glob;
            textpar    : tsp00_C40;  (* called with constants *)
            lreal      : tsp00_Longreal);
        FORWARD;

PROCEDURE
      ak42prefix (
            VAR acv     : tak_all_command_glob;
            VAR a41v    : tak40_show_glob;
            VAR col_cnt : integer);
        FORWARD;

PROCEDURE
      ak42monitor_type (
            VAR acv     : tak_all_command_glob;
            VAR a41v    : tak40_show_glob;
            VAR col_cnt : integer);
        FORWARD;

PROCEDURE
      ak42devspace_statistics (
            VAR acv  : tak_all_command_glob;
            VAR a41v : tak40_show_glob);
        FORWARD;

FUNCTION
      ak42evalpercent (cnt, max : tsp00_Int4) : tsp00_Int4;
        FORWARD;

FUNCTION
      ak42evalpercent_cnt (cnt, max : tsp00_8ByteCounter) : tsp00_Int4;
        FORWARD;

FUNCTION
      ak42evalpercent_real (cnt, max : tsp00_Longreal) : tsp00_Int4;
        FORWARD;

PROCEDURE
      ak42datab_statistic (
            VAR acv  : tak_all_command_glob;
            VAR a41v : tak40_show_glob;
            v30      : tsp00_C30;    (* called with constants *)
            lint     : tsp00_Int4);
        FORWARD;

PROCEDURE
      ak42datab_stat_undef (
            VAR acv    : tak_all_command_glob;
            VAR a41v   : tak40_show_glob;
            v30        : tsp00_C30;  (* called with constants *)
            lint       : tsp00_Int4;
            zero_undef : boolean);
        FORWARD;

PROCEDURE
      ak42log_statistics  (
            VAR acv  : tak_all_command_glob;
            VAR a41v : tak40_show_glob);
        FORWARD;

PROCEDURE
      ak42user_statistics  (
            VAR acv  : tak_all_command_glob;
            VAR a41v : tak40_show_glob);
        FORWARD;

PROCEDURE
      ak42all_user_st_show  (
            VAR acv  : tak_all_command_glob;
            VAR a41v : tak40_show_glob);
        FORWARD;

PROCEDURE
      ak42one_st_user_show (
            VAR acv  : tak_all_command_glob;
            VAR a41v : tak40_show_glob;
            one_show  : boolean);
        FORWARD;

PROCEDURE
      ak42show_config  (
            VAR acv  : tak_all_command_glob;
            VAR a41v : tak40_show_glob);
        FORWARD;

PROCEDURE
      ak42hex (
            alpha_char : char;
            VAR outnam : tsp00_C32;
            outpos     : integer);
        FORWARD;

PROCEDURE
      ak42show_version (
            VAR acv  : tak_all_command_glob;
            VAR a41v : tak40_show_glob);
        FORWARD;

PROCEDURE
      a42iindex_statistics   (
            VAR acv    : tak_all_command_glob;
            VAR a41v   : tak40_show_glob);
        FORWARD;

PROCEDURE
      a42table_statistics   (
            VAR acv    : tak_all_command_glob;
            VAR a41v   : tak40_show_glob;
            pages_only : boolean);
        FORWARD;

PROCEDURE
      ak42char2hex (
            input      : char;
            VAR output : tsp00_C2);
        FORWARD;

PROCEDURE
      ak42one_index_statistics (
            VAR acv         : tak_all_command_glob;
            show_kind       : tgg00_ShowKind;
            VAR baserec     : tak_baserecord;
            VAR st_entry    : tgg00_StackEntry;
            VAR mblock      : tgg00_MessBlock);
        FORWARD;

PROCEDURE
      ak42table_statistics (
            VAR acv      : tak_all_command_glob;
            VAR baserec  : tak_baserecord;
            VAR mblock   : tgg00_MessBlock);
        FORWARD;

PROCEDURE
      ak42print_stat_tab_index   (
            VAR acv       : tak_all_command_glob;
            VAR a41v      : tak40_show_glob;
            VAR mblock    : tgg00_MessBlock;
            selectivity   : boolean);
        FORWARD;

PROCEDURE
      a42_check_database (VAR acv : tak_all_command_glob);
        FORWARD;

PROCEDURE
      ak42check_db_shortinfos (VAR acv : tak_all_command_glob);
        FORWARD;

PROCEDURE
      ak42snullname_statistic (
            VAR acv  : tak_all_command_glob;
            VAR a41v : tak40_show_glob;
            VAR v30  : tsp00_C30);
        FORWARD;

PROCEDURE
      ak42sname_statistic (
            VAR acv  : tak_all_command_glob;
            VAR a41v : tak40_show_glob;
            VAR v30  : tsp00_C30;
            VAR nam  : tsp00_Sname);
        FORWARD;

PROCEDURE
      a42name_and_val_statistic (
            VAR acv  : tak_all_command_glob;
            VAR a41v : tak40_show_glob;
            VAR vkw  : tak_keyword;
            value    : tsp00_Int4;
            VAR nam  : tsp00_Sname);
        FORWARD;

PROCEDURE
      ak42hostname_statistic (
            VAR acv  : tak_all_command_glob;
            VAR a41v : tak40_show_glob;
            VAR v30  : tsp00_C30;
            VAR nam  : tsp00_VFilename);
        FORWARD;

PROCEDURE
      ak42get_refinfo (
            VAR acv  : tak_all_command_glob;
            VAR info : tgg00_ReferenceInfo);
        FORWARD;

PROCEDURE
      ak42get_tablename (
            VAR acv       : tak_all_command_glob;
            VAR a41v      : tak40_show_glob;
            required_priv : tgg00_PrivilegeSet);
        FORWARD;

PROCEDURE
      a42inc_refresh_cnt (is_fast : boolean);
        FORWARD;

PROCEDURE
      a42index_inf_to_messbuf (
            VAR acv         : tak_all_command_glob;
            VAR mblock      : tgg00_MessBlock;
            VAR a41v        : tak40_show_glob;
            VAR indexn      : tsp00_KnlIdentifier;
            VAR selectivity : boolean;
            diagnose_index  : boolean);
        FORWARD;

PROCEDURE
      ak42session_to_sname (
            session : tgg91_SessionNo;
            max_len : integer;
            VAR n   : tsp00_Sname);
        FORWARD;

PROCEDURE
      ak42xparams (
            VAR acv       : tak_all_command_glob;
            VAR a41v      : tak40_show_glob);
        FORWARD;

PROCEDURE
      ak42ascii_const_move (
            src           : tsp00_MoveObjPtr;
            srclen        : integer;
            VAR destpos   : integer;
            VAR dest      : tsp00_C256;
            VAR e         : tsp00_Int2);
        FORWARD;

PROCEDURE
      ak42move_trans_no (
            VAR acv   : tak_all_command_glob;
            VAR a41v  : tak40_show_glob;
            VAR trans : tgg91_TransNo);
        FORWARD;

PROCEDURE
      ak42internal_state (VAR acv : tak_all_command_glob;
            VAR a41v      : tak40_show_glob);
        FORWARD;

PROCEDURE
      a42bool_to_intern_state (
            acv_ptr     : tak_acv_address;
            a41v_ptr    : tak40_show_glob_ptr;
            component   : tsp00_C20;
            description : tsp00_C40;
            bool_val    : boolean);
        FORWARD;

PROCEDURE
      a42cint6_to_intern_state (
            acv_ptr     : tak_acv_address;
            a41v_ptr    : tak40_show_glob_ptr;
            component   : tsp00_C20;
            description : tsp00_C40;
            cint6       : tgg90_Cint6);
        FORWARD;

PROCEDURE
      a42counter_to_intern_state (
            acv_ptr     : tak_acv_address;
            a41v_ptr    : tak40_show_glob_ptr;
            component   : tsp00_C20;
            description : tsp00_C40;
            counter     : tsp00_8ByteCounter);
        FORWARD;

PROCEDURE
      a42date_to_intern_state (
            acv_ptr     : tak_acv_address;
            a41v_ptr    : tak40_show_glob_ptr;
            component   : tsp00_C20;
            description : tsp00_C40;
            int_date    : tsp00_Int4);
        FORWARD;

PROCEDURE
      a42int4_to_intern_state (
            acv_ptr     : tak_acv_address;
            a41v_ptr    : tak40_show_glob_ptr;
            component   : tsp00_C20;
            description : tsp00_C40;
            int_val     : tsp00_Int4);
        FORWARD;

PROCEDURE
      a42logpos_to_intern_state (
            acv_ptr     : tak_acv_address;
            a41v_ptr    : tak40_show_glob_ptr;
            component   : tsp00_C20;
            description : tsp00_C40;
            logpos      : tgg00_LogPos);
        FORWARD;

PROCEDURE
      a42mess_to_intern_state (
            acv_ptr     : tak_acv_address;
            a41v_ptr    : tak40_show_glob_ptr;
            component   : tsp00_C20;
            description : tsp00_C40;
            messtype    : tgg00_MessType);
        FORWARD;

PROCEDURE
      a42mess2_to_intern_state (
            acv_ptr     : tak_acv_address;
            a41v_ptr    : tak40_show_glob_ptr;
            component   : tsp00_C20;
            description : tsp00_C40;
            mess2type    : tgg00_MessType2);
        FORWARD;

PROCEDURE
      a42session_to_intern_state (
            acv_ptr     : tak_acv_address;
            a41v_ptr    : tak40_show_glob_ptr;
            component   : tsp00_C20;
            description : tsp00_C40;
            session       : tgg91_SessionNo);
        FORWARD;

PROCEDURE
      a42str_to_intern_state (
            acv_ptr     : tak_acv_address;
            a41v_ptr    : tak40_show_glob_ptr;
            component   : tsp00_C20;
            description : tsp00_C40;
            str_ptr     : tsp_moveobj_ptr;
            str_len     : integer);
        FORWARD;

PROCEDURE
      a42surrogate_to_intern_state (
            acv_ptr     : tak_acv_address;
            a41v_ptr    : tak40_show_glob_ptr;
            component   : tsp00_C20;
            description : tsp00_C40;
            surrogate   : tgg00_Surrogate);
        FORWARD;

PROCEDURE
      a42time_to_intern_state (
            acv_ptr     : tak_acv_address;
            a41v_ptr    : tak40_show_glob_ptr;
            component   : tsp00_C20;
            description : tsp00_C40;
            int_time    : tsp00_Int4);
        FORWARD;

PROCEDURE
      a42trans_to_intern_state (
            acv_ptr     : tak_acv_address;
            a41v_ptr    : tak40_show_glob_ptr;
            component   : tsp00_C20;
            description : tsp00_C40;
            trans       : tgg91_TransNo);
        FORWARD;

PROCEDURE
      a42uint1_to_intern_state (
            acv_ptr     : tak_acv_address;
            a41v_ptr    : tak40_show_glob_ptr;
            component   : tsp00_C20;
            description : tsp00_C40;
            int_val     : tsp00_Int2);
        FORWARD;

PROCEDURE
      a42uint2_to_intern_state (
            acv_ptr     : tak_acv_address;
            a41v_ptr    : tak40_show_glob_ptr;
            component   : tsp00_C20;
            description : tsp00_C40;
            int_val     : tsp00_Int4);
        FORWARD;

PROCEDURE
      a42uint4_to_intern_state (
            acv_ptr     : tak_acv_address;
            a41v_ptr    : tak40_show_glob_ptr;
            component   : tsp00_C20;
            description : tsp00_C40;
            int_val     : tsp00_8ByteCounter);
        FORWARD;

PROCEDURE
        a42execute ;



BEGIN
cccprint('AK42  a42execute');
WITH acv, a41v DO
    BEGIN
    CASE a4sh_kind OF
        sh_consistent_view : (* PTS 1106007 *)
            ak42consistent_view (acv, a41v);
        sh_ctrl_configuration :
            ak42ctrl_config (acv, a41v);
        sh_ctrl_devspaces :
            ak42ctrl_devspaces (acv, a41v);
        sh_ctrl_io_statistics :
            ak42ctrl_io_statistics (acv, a41v);
        sh_ctrl_locks :
            ak42ctrl_locks (acv, a41v);
        sh_ctrl_log :
            ak42ctrl_log_statistics (acv, a41v);
        sh_ctrl_cache :
            ak42ctrl_caches_statistics (acv, a41v);
        (* PTS 1115043 E.Z. *)
        sh_dbm_state :
            ak42dbm_state (acv, a41v);
        sh_roots :
            ak42roots (acv, a41v);
        (* PTS 1113491 *)
        sh_heap_content :
            ak42HeapContent (acv, a41v);
        sh_history_info :
            ak42history_info (acv, a41v);
        (* PTS 1109995 E.Z. *)
        sh_lock, sh_trans, sh_st_lock, sh_transfile :
            ak42select_lock_trans (acv, a41v);
        sh_st_config :
            ak42show_config  (acv, a41v);
        sh_st_database :
            ak42serverdb_statistics (acv, a41v);
        sh_st_devspace :
            ak42devspace_statistics (acv, a41v);
        sh_st_log :
            ak42log_statistics  (acv, a41v);
        sh_monitor_oms :
            ak42monitor_oms  (acv, a41v, NOT c_reset_statistics);
        sh_st_monitor :
            ak42monitor_show (acv, a41v, cak_i_all);
        sh_st_state :
            ak42state_statistics (acv, a41v);
        sh_st_user :
            ak42user_statistics  (acv, a41v);
        sh_version :
            ak42show_version (acv, a41v);
        sh_xparam :
            ak42xparams (acv, a41v);
        sh_running_cmds :
            ak42show_running_cmds(acv, a41v);
        sh_oms_memory :
            ak42oms_memory(acv, a41v);
        (* PTS 1107717 E.Z. *)
        sh_internal_state :
            ak42internal_state (acv, a41v);
        sh_oms_lock : (* PTS 1110315 *)
            ak42OmsLockObjInfo (acv, a41v);
        sh_files :
            ak42files (acv, a41v);
        sh_info_gc_stats :
            ak42InfoGCStats (acv, a41v);
        sh_container_roots : (* PTS 1115641 FF *)
            ak42_container_roots (acv, a41v);
        sh_container_chains : (* PTS 1115641 FF *)
            ak42_container_chains (acv, a41v);
        sh_container_keys : (* PTS 1115641 FF *)
            ak42_container_keys (acv, a41v);
        END;
    (*ENDCASE*)
    END;
(*ENDWITH*)
;cccprint('<a42execute     ');
END;

(*------------------------------*)

PROCEDURE
        a42get_tablename ;



VAR
      name_pos        : integer;
      syskey          : tgg00_SysInfoKey;
      sysbuf          : tgg00_Rec;
      sysbuf_ptr      : tak_sysbufferaddress;

BEGIN
cccprint('AK42  a42get_tab');
sysbuf_ptr      := @sysbuf;
authid          := a01_il_b_identifier;
tablen          := a01_il_b_identifier;
syskey          := a01defaultkey;
syskey.stableid := tabid;
a36get_catalog_record (t, syskey, sysbuf_ptr^);
IF  t.trError_gg00 = e_ok
THEN
    BEGIN
    IF  sysbuf_ptr^.sbase.btablekind = tsynonym
    THEN
        name_pos := sizeof(tak_synonymrecord) -
              sizeof(sysbuf_ptr^.ssynonym.syn_tablen) + 1
    ELSE
        name_pos := a10BaseRecPersistentOffset + 1; (* PTS 1105713, T.A. 24.02.2000 *)
    (*ENDIF*)
    t01int4 (ak_sem, 'name_pos    ', name_pos);
    tablen := a01_il_b_identifier;
    g10mv ('VAK42 ',   1,
          sizeof (sysbuf.buf), sizeof (tablen), sysbuf.buf, name_pos + 1,
          tablen, 1, ord (sysbuf.buf[name_pos]), t.trError_gg00);
    IF  sysbuf_ptr^.sbase.btablekind = tsynonym
    THEN
        syskey.sauthid   := sysbuf_ptr^.ssynonym.syn_authid
    ELSE
        syskey.sauthid   := sysbuf_ptr^.sbase.bauthid;
    (*ENDIF*)
    syskey.sentrytyp := cak_euser;
    IF  t.trError_gg00 = e_ok
    THEN
        a36get_catalog_record (t, syskey, sysbuf_ptr^);
    (*ENDIF*)
    IF  t.trError_gg00 = e_ok
    THEN
        authid := sysbuf_ptr^.suser.username
    (*ENDIF*)
    END;
(*ENDIF*)
t01lidentifier (bi, authid);
t01lidentifier (bi, tablen);
b_err := t.trError_gg00
;cccprint('<a42get_tablenam');
END;

(*------------------------------*)

PROCEDURE
        a42get_username ;



VAR
      i               : integer;
      syskey          : tgg00_SysInfoKey;
      userrec         : tak_userrecord;
      userrec_ptr     : tak_sysbufferaddress;

BEGIN
cccprint('AK42  a42get_use');
t.trError_gg00 := e_ok;
IF  userid = cak_util_userid
THEN
    username := a01controluser
ELSE
    BEGIN
    userrec_ptr       := @userrec;
    username          := a01_il_b_identifier;
    syskey            := a01defaultkey;
    syskey.sauthid[1] := chr(255);
    syskey.sauthid[2] := chr(255);
    FOR i := 1 TO sizeof (tgg00_UserId) DO
        syskey.sauthid[sizeof(syskey.sauthid) - sizeof (tgg00_UserId) + i] :=
              userid[i];
    (*ENDFOR*)
    syskey.sentrytyp := cak_euser;
    a36get_catalog_record (t, syskey, userrec_ptr^);
    IF  t.trError_gg00 = e_ok
    THEN
        username := userrec.username
    (*ENDIF*)
    END;
(*ENDIF*)
b_err := t.trError_gg00
;cccprint('<a42get_username');
END;

(*------------------------------*)

PROCEDURE
        a42_get_pagecount ;



VAR
      pages          : tsp00_Int4;
      index_scan_rec : tak_index_scan_record;

      data           : RECORD
            CASE boolean OF
                true :
                    (d : tsp00_Longreal);
                false :
                    (b : tgg00_DoubleBuf);
                END;
            (*ENDCASE*)

      qual           : tgg00_QualBuf;
      mblock         : tgg00_MessBlock;

BEGIN
cccprint('AK42  a42_get_pa');
g01mblock_init   (acv.a_transinf.tri_trans, m_nil, mm_nil, mblock);
g01datapart_init (mblock, @data, sizeof (data));
g01stack_init    (mblock, NIL, 0, @qual, sizeof (qual));
WITH acv DO
    BEGIN
    ak42table_statistics (acv, base_ptr^.sbase, mblock);
    WITH mblock.mb_data^.mbp_sample DO
        pages := nodes + all_stringcol_pages;
    (*ENDWITH*)
    IF  a_return_segm^.sp1r_returncode = 0
    THEN
        BEGIN
        (*=======================================*)
        (*= determine number of index leaf pages *)
        (*=======================================*)
        IF  base_ptr^.sbase.bindexexist
        THEN
            BEGIN
            a24init_index_scan (acv, base_ptr^.sbase.bsurrogate,
                  index_scan_rec);
            WITH index_scan_rec DO
                WHILE a24next_named_index (acv, index_scan_rec) DO
                    WITH isr_buf^.smindex.indexdef[isr_index] DO
                        IF  (indexno <> 0) AND
                            (a_return_segm^.sp1r_returncode = 0)
                        THEN
                            BEGIN
                            ak42one_index_statistics (acv, sh_indexpages,
                                  base_ptr^.sbase, icolstack[1],
                                  mblock);
                            pages := pages +
                                  mblock.mb_data^.mbp_sample.nodes
                            END;
                        (*ENDIF*)
                    (*ENDWITH*)
                (*ENDWHILE*)
            (*ENDWITH*)
            a24finish_index_scan (acv, index_scan_rec)
            END;
        (*=====================================*)
        (*= determine space of single indexes =*)
        (*=====================================*)
        (*ENDIF*)
        END;
    (*ENDIF*)
    IF  a_return_segm^.sp1r_returncode = 0
    THEN
        pagecount := pagecount + pages;
    (*ENDIF*)
    END;
(*ENDWITH*)
;cccprint('<a42_get_pagecou');
END;

(*------------------------------*)

PROCEDURE
        a42_init_time_array ;



BEGIN
cccprint('AK42  a42_init_t');
a42check_time := true;
;cccprint('<a42_init_time_a');
END;

(* PTS 1103033 E.Z. *)
(*------------------------------*)

PROCEDURE
        a42monitor_init ;



BEGIN
cccprint('AK42  a42monitor');
ak42refresh_cnt      := 0;
ak42fast_refresh_cnt := 0;
a260reset_statistics;
a262reset_statistics;
k51reset_statistics  (procid);
kb560ResetStatistics (procid);
k57statistics_reset  (procid);
k720init_ref_statistic;
b01reset_monitor (mpAll_egg00);
(* PTS 1103033 E.Z. *)
(* PTS 1103735 E.Z. *)
(* PTS 1111134 E.Z. *)
bd91InfoGCReset; (* PTS 1113774 F.F. *)
;cccprint('<a42monitor_init');
END;

(*------------------------------*)

PROCEDURE
        a42_start_semantic ;



VAR
      procid : tsp00_TaskId;

BEGIN
cccprint('AK42  a42_start_');
WITH acv DO
    BEGIN
    procid := a_transinf.tri_trans.trTaskId_gg00;
    IF  (a_current_user_kind <> usysdba) AND
        (a_current_user_kind <> udba)    AND
        (a_current_user_kind <> ucontroluser)
    THEN
        a07_b_put_error (acv, e_missing_privilege, 1)
    ELSE
        CASE a_ap_tree^[ a_ap_tree^[ 0 ].n_lo_level ].n_subproc  OF
            cak_i_init:
                BEGIN
                IF  a_ap_tree^[ a_ap_tree^[ 0 ].n_lo_level ].n_length <> cak_i_object
                THEN (* PTS 1108772 *)
                    a42monitor_init (procid);
                (*ENDIF*)
                ak42ResetOmsMonitor (acv);
                END;
            cak_i_on :
                BEGIN
                (* h.b. 16.08.1999 always run monitor  *)
                END;
            cak_i_off :
                BEGIN
                (* h.b. 16.08.1999 always run monitor  *)
                END;
            cak_i_object :
                ak42monitor_object(acv, a_ap_tree^[a_ap_tree^[0].n_lo_level].n_length);
            OTHERWISE :
                a07_b_put_error (acv, e_not_implemented, 1);
            END;
        (*ENDCASE*)
    (*ENDIF*)
    END;
(*ENDWITH*)
;cccprint('<a42_start_seman');
END;

(*------------------------------*)

PROCEDURE
        ak42ctrl_config ;



VAR
      autosave_on    : boolean;
      kb_factor      : tsp00_Int4;
      totalPages     : tsp00_Int4;
      unMapPages     : tsp00_Int4;
      sname          : tsp00_Sname;
      media_name     : tsp00_C64;
      log_device_info: tkb00_LogDeviceInfo;
      log_queue_info : tkb00_LogQueueInfo;

BEGIN
cccprint('AK42  ak42ctrl_c');
bd01DataCacheSize( totalPages, unMapPages ); (* TODO *)
a40get_catalog_table (acv, a41v);
IF  acv.a_return_segm^.sp1r_returncode = 0
THEN
    BEGIN
    CASE g01code.char_default OF
        csp_ascii:
            sname := n_ascii;
        csp_ebcdic:
            sname := n_ebcdic;
        csp_codeneutral:
            sname := n_internal;
        csp_unicode:
            sname := n_unicode;
        OTHERWISE
            sname := bsp_sname;
        END;
    (*ENDCASE*)
    a40move_const (acv, a41v, @sname, sizeof (sname));
    CASE g01code.date_time OF
        dtf_normal :
            sname := n_internal;
        dtf_eur, dtf_ts_eur :
            sname := n_eur;
        dtf_iso    :
            sname := n_iso;
        dtf_jis    :
            sname := n_jis;
        dtf_usa    :
            sname := n_usa;
        OTHERWISE
            sname := bsp_sname;
        END;
    (*ENDCASE*)
    a40move_const (acv, a41v, @sname, sizeof (sname));
    kb560GetStatistics (acv.a_transinf.tri_trans.trTaskId_gg00,
          log_device_info, log_queue_info);
    (* IF bd999LogIsMirrored +++++ is missing *)
    sname := n_single_log;
    a40move_const (acv, a41v, @sname, sizeof (sname));
    a40move_const (acv, a41v, @a40yes_no [false], 1);
    a40move_const (acv, a41v, @a40yes_no [b01db_full( acv.a_transinf.tri_trans.trTaskId_gg00) ], 1);
    (* PTS 1112568 E.Z. *)
    a40move_const (acv, a41v, @a40yes_no
          [kb560LogIsFull(acv.a_transinf.tri_trans.trTaskId_gg00)], 1);
    a40move_const (acv, a41v, @a40yes_no [b01connect_allowed], 1);
    a40move_const (acv, a41v, @a40yes_no [a01diag_monitor_on], 1);
    a40move_const (acv, a41v, @a40yes_no [c_monitor_on], 1);
    a40move_const (acv, a41v, @a40yes_no [g01vtrace.vtrAny_gg00], 1);
    autosave_on := k38is_on_autosave (acv.a_transinf.tri_trans, false);
    a40move_const (acv, a41v, @a40yes_no [autosave_on], 1);
    (* PTS 1000211, T.A. 1999-07-12 *)
    IF  autosave_on
    THEN
        BEGIN
        k38get_autosave_medianame (acv.a_transinf.tri_trans.trTaskId_gg00, media_name);
        a40move_const (acv, a41v, @media_name, sizeof(media_name));
        END
    ELSE
        a40move (acv, a41v, NIL, 0);
    (*ENDIF*)
    a40move_const (acv, a41v, @a40yes_no [
          k51is_closed_locklist (acv.a_transinf.tri_trans.trTaskId_gg00)], 1);
    a40move_i4    (acv, a41v, g01maxuser,           NOT c_null, 0, NOT c_to_undef);
    a40move_i4    (acv, a41v, g01maxcpu,            NOT c_null, 0, NOT c_to_undef);
    a40move_i4    (acv, a41v, b01no_of_log_devs,    NOT c_null, 0, NOT c_to_undef);
    a40move_i4    (acv, a41v, b01no_of_data_devs,   NOT c_null, 0, NOT c_to_undef);
    a40move_i4    (acv, a41v,
          b01get_bad_index_cnt(acv.a_transinf.tri_trans.trTaskId_gg00), NOT c_null, 0, NOT c_to_undef);
    kb_factor := PAGE_MXSP00 DIV 1024;
    a40move_i4    (acv, a41v,
          bd01ConverterSize * kb_factor, NOT c_null, 0, NOT c_to_undef);
    a40move_i4    (acv, a41v,
          totalPages * kb_factor, NOT c_null, 0, NOT c_to_undef);
    a40move_i4    (acv, a41v,
          g01cache_size (cachLogQueue_egg00), NOT c_null, 0, NOT c_to_undef);
    a40move_i4    (acv, a41v, g01maxlocks, NOT c_null, 0, NOT c_to_undef);
    END;
(*ENDIF*)
;cccprint('<ak42ctrl_config');
END;

(*------------------------------*)

PROCEDURE
        ak42ctrl_devspaces ;



VAR
      kb_factor       : integer;
      dev_no          : tsp_int4;
      dev_used_blocks : tsp00_Int4;
      devType         : tsp_c4;
      dev_type        : tsp2_dev_type;
      dev_name        : tsp2_devname;
      dev_size        : tsp_int4;
      dev_online      : boolean;
      dev_state       : tsp_c18;

BEGIN
cccprint('AK42  ak42ctrl_d');
a40get_catalog_table (acv, a41v);
IF  acv.a_return_segm^.sp1r_returncode = 0
THEN
    BEGIN
    dev_no    := -1;
    kb_factor := PAGE_MXSP00 DIV 1024;
    REPEAT
        b01next_dev_info (acv.a_transinf.tri_trans.trTaskId_gg00,
              dev_no, dev_type, dev_name, dev_size, dev_used_blocks,
              dev_online, dev_state);
        IF  dev_no >= 0
        THEN
            BEGIN
            CASE dev_type OF
                sp2dt_sys :
                    devType := 'SYS ';
                sp2dt_log :
                    devType := 'LOG ';
                sp2dt_data :
                    devType := 'DATA';
                OTHERWISE
                    devType := '    ';
                END;
            (*ENDCASE*)
            a40move_const (acv, a41v, @devType, sizeof (devType));
            a40move_i4    (acv, a41v, dev_no, NOT c_null, 0, NOT c_to_undef);
            a40move_const (acv, a41v, @dev_name, sizeof (dev_name));
            a40move_i4    (acv, a41v, dev_size * kb_factor, NOT c_null, 0, NOT c_to_undef);
            a40move_const (acv, a41v, @dev_state, sizeof (dev_state));
            END;
        (*ENDIF*)
    UNTIL
        dev_no = -1;
    (*ENDREPEAT*)
    END;
(*ENDIF*)
;cccprint('<ak42ctrl_devspa');
END;

(*------------------------------*)

PROCEDURE
        ak42ctrl_io_statistics ;



VAR
      rowNo  : integer;
      sum_ix : integer;
      ind    : tgg00_RefInfoIndex;
      c10    : tsp_c10;
      info   : tgg00_ReferenceInfo;
      sum    : ARRAY[1..4] OF tsp00_Longreal;

BEGIN
cccprint('AK42  ak42ctrl_i');
a40get_catalog_table (acv, a41v);
IF  acv.a_return_segm^.sp1r_returncode = 0
THEN
    BEGIN
    FOR sum_ix := 1 TO 4 DO
        sum[sum_ix] := 0;
    (*ENDFOR*)
    rowNo  := 0;
    ak42get_refinfo (acv, info);
    (* PTS 1115043 E.Z. *)
    FOR ind :=  icatalog_r_v TO ipsm_w_p DO
        BEGIN
        IF  a41v.a4pos = 1
        THEN
            BEGIN
            sum_ix := 1;
            rowNo  := rowNo + 1;
            CASE rowNo OF
                1 :
                    c10 := 'Catalog   ';
                2 :
                    c10 := 'Perm Data ';
                3 :
                    c10 := 'Temp Data ';
                (* PTS 1103033 E.Z. *)
                4 :
                    c10 := 'Long Data ';
                5 :
                    c10 := 'Leaf      ';
                6 :
                    c10 := 'Level 1   ';
                7 :
                    c10 := 'Level 2   ';
                8 :
                    c10 := 'Level 3   ';
                (* PTS 1115043 E.Z. *)
                9 :
                    c10 := 'Level 4   ';
                10 :
                    c10 := 'Level 5   ';
                11 :
                    BEGIN
                    c10 := 'Sum (Tree)'; (* PTS 1102630, T.A. 1999-05-10 *)
                    a40move_const (acv, a41v, @c10, sizeof (c10));
                    FOR sum_ix := 1 TO 4 DO
                        a40move_real (acv, a41v, sum[sum_ix], sum[sum_ix] < 0, 0);
                    (*ENDFOR*)
                    c10 := 'Converter ';
                    END
                END;
            (*ENDCASE*)
            a40move_const (acv, a41v, @c10, sizeof (c10));
            END;
        (*ENDIF*)
        a40move_counter (acv, a41v, info.reference_array[ind], NOT c_null, 0, NOT c_to_undef);
        IF  (rowNo >= 4) AND (rowNo <= 10) (* PTS 1102630, T.A. 1999-05-10 *)
        THEN
            IF  sum[sum_ix] >= 0
            THEN
                sum[sum_ix] := sum[sum_ix] + info.reference_array[ind];
            (*ENDIF*)
        (*ENDIF*)
        sum_ix := sum_ix + 1
        END;
    (*ENDFOR*)
    END;
(*ENDIF*)
;cccprint('<ak42ctrl_io_sta');
END;

(*------------------------------*)

PROCEDURE
        ak42ctrl_locks ;



VAR
      lock_info : tkb05_LockInfo; (* PTS 1103743 JA 1999-08-25 *)

BEGIN
cccprint('AK42  ak42ctrl_l');
a40get_catalog_table (acv, a41v);
IF  acv.a_return_segm^.sp1r_returncode = 0
THEN
    BEGIN
    (* PTS 1103033 E.Z.; PTS 1103743 JA 1999-08-25 *)
    k51info_lock (acv.a_transinf.tri_trans.trTaskId_gg00, lock_info);
    WITH lock_info DO
        BEGIN
        a40move_i4      (acv, a41v, infMaxEntries_kb05,      NOT c_null, 0, NOT c_to_undef);
        a40move_i4      (acv, a41v, infAvgUsed_kb05,         NOT c_null, 0, NOT c_to_undef);
        a40move_counter (acv, a41v, infSqlCollisions_kb05,   NOT c_null, 0, NOT c_to_undef);
        a40move_counter (acv, a41v, infEscalations_kb05,     NOT c_null, 0, NOT c_to_undef);
        a40move_counter (acv, a41v, infRowEntryAmount_kb05,  NOT c_null, 0, NOT c_to_undef);
        a40move_counter (acv, a41v, infTabEntryAmount_kb05,  NOT c_null, 0, NOT c_to_undef);
        a40move_i4      (acv, a41v, g01timeout.timeReq_gg00, NOT c_null, 0, NOT c_to_undef)
        END
    (*ENDWITH*)
    END
(*ENDIF*)
;cccprint('<ak42ctrl_locks ');
END;

(*------------------------------*)

PROCEDURE
        ak42ctrl_log_statistics ;



VAR
      kb_factor           : integer;
      log_percent         : tsp00_Int4;
      sname               : tsp00_Sname;
      ref_info            : tgg00_ReferenceInfo;  (* PTS 1103743 JA 1999-08-25 *)
      log_queue_info      : tkb00_LogQueueInfo;
      log_device_info     : tkb00_LogDeviceInfo;

BEGIN
cccprint('AK42  ak42ctrl_l');
a40get_catalog_table (acv, a41v);
IF  acv.a_return_segm^.sp1r_returncode = 0
THEN
    BEGIN
    kb_factor := PAGE_MXSP00 DIV 1024;
    ak42get_refinfo (acv, ref_info);
    kb560GetStatistics (acv.a_transinf.tri_trans.trTaskId_gg00,
          log_device_info, log_queue_info);
    (* IF bd999LogIsMirrored +++++ is missing *)
    sname := n_single_log;
    a40move_const (acv, a41v, @sname, sizeof (sname));
    a40move_i4    (acv, a41v, kb560GetLogDeviceSize * kb_factor, NOT c_null, 0, NOT c_to_undef);
    a40move_i4    (acv, a41v, g01autosave_distance * kb_factor, NOT c_null, 0, NOT c_to_undef);
    WITH log_device_info DO
        BEGIN
        a40move_i4 (acv, a41v, ldiPagesUsed_kb00 * kb_factor,     NOT c_null, 0, NOT c_to_undef);
        log_percent := ak42evalpercent (ldiPagesUsed_kb00, kb560GetLogDeviceSize);
        (* *)
        a40move_i4 (acv, a41v, log_percent,                       NOT c_null, 0, NOT c_to_undef);
        a40move_i4 (acv, a41v, ldiPagesNotSaved_kb00 * kb_factor, NOT c_null, 0, NOT c_to_undef);
        log_percent := ak42evalpercent (ldiPagesNotSaved_kb00, kb560GetLogDeviceSize);
        (* *)
        a40move_i4 (acv, a41v, log_percent,                       NOT c_null, 0, NOT c_to_undef);
        a40move_i4 (acv, a41v, ldiPagesSinceBup_kb00 * kb_factor, NOT c_null, 0, NOT c_to_undef);
        a40move    (acv, a41v, NIL, 0);                                           (* to do *)
        a40move_i4 (acv, a41v, ldiSaveptCount_kb00,               NOT c_null, 0, NOT c_to_undef);
        a40move_i4 (acv, a41v, 0(*ldiCheckptCount_kb00*),         NOT c_null, 0, NOT c_to_undef)
        END;
    (*ENDWITH*)
    WITH ref_info DO
        BEGIN
        a40move_counter (acv,a41v, reference_array [iread_log ], NOT c_null, 0, NOT c_to_undef);
        a40move_counter (acv,a41v, reference_array [iwrite_log], NOT c_null, 0, NOT c_to_undef)
        END;
    (*ENDWITH*)
    WITH log_queue_info DO
        BEGIN
        a40move_i4      (acv, a41v, lqiLogQueueSize_kb00 * kb_factor, NOT c_null, 0, NOT c_to_undef);
        a40move         (acv, a41v, NIL, 0); (* queue allocated ?? *)
        a40move         (acv, a41v, NIL, 0); (* queue entries   ?? *)
        a40move_counter (acv, a41v, lqiCountRequest_kb00,             NOT c_null, 0, NOT c_to_undef);
        a40move_counter (acv, a41v, lqiCountGroup_kb00,               NOT c_null, 0, NOT c_to_undef);
        a40move_counter (acv, a41v, lqiCountWait_kb00,                NOT c_null, 0, NOT c_to_undef);
        a40move_i4      (acv, a41v, lqiMaxWaitLogpage_kb00,           NOT c_null, 0, NOT c_to_undef);
        a40move_counter (acv, a41v, lqiAvgSumPagewaits_kb00,          NOT c_null, 0, NOT c_to_undef)
        END;
    (*ENDWITH*)
    (* former obj_log_cache_info: PTS 1105395 JA 2000-03-07 *)
    a40move_i4 (acv, a41v, 0 (* PagesUsed    *), NOT c_null, 0, NOT c_to_undef);
    a40move_i4 (acv, a41v, 0 (* PagesMinFree *), NOT c_null, 0, NOT c_to_undef)
    END
(*ENDIF*)
;cccprint('<ak42ctrl_log_st');
END;

(*------------------------------*)

PROCEDURE
        ak42ctrl_caches_statistics ;



VAR
      i          : tgg00_RefInfoIndex;
      ind        : tgg00_RefInfoIndex;
      kind_ix    : integer;
      total      : tsp00_Longreal;
      hit        : tsp00_Longreal;
      (* PTS 1115020 E.Z. *)
      cache_kind : ARRAY[1..3] OF tsp00_C18;
      info       : tgg00_ReferenceInfo;

BEGIN
cccprint('AK42  ak42ctrl_c');
a40get_catalog_table (acv, a41v);
IF  acv.a_return_segm^.sp1r_returncode = 0
THEN
    BEGIN
    ak42get_refinfo (acv, info);
    cache_kind[1] := 'DATA              ';
    (* PTS 1103735 E.Z. *)
    (* PTS 1111134 E.Z. *)
    (* PTS 1105315 E.Z. *)
    (* PTS 1115020 E.Z. *)
    cache_kind[2] := 'CATALOG           ';
    cache_kind[3] := 'SEQUENCE          ';
    kind_ix       := 0;
    (* PTS 1103735 E.Z. *)
    FOR ind := idata_cache_total TO isseq_cache_percent DO
        BEGIN
        IF  ind in [idata_cache_total,
            (* PTS 1103735 E.Z. *)
            (* PTS 1111134 E.Z. *)
            (* PTS 1105315 E.Z. *)
            (* PTS 1115020 E.Z. *)
            iak_cache_total, isseq_cache_total]
        THEN
            BEGIN
            kind_ix := kind_ix + 1;
            a40move_const (acv, a41v, @cache_kind[kind_ix], sizeof (cache_kind[kind_ix]));
            END
        ELSE
            IF  ind in [ idata_cache_percent,
                (* PTS 1103735 E.Z. *)
                (* PTS 1111134 E.Z. *)
                (* PTS 1105315 E.Z. *)
                (* PTS 1115020 E.Z. *)
                iak_cache_percent,
                isseq_cache_percent]
            THEN
                BEGIN (* evaluate percentage value *)
                i   := pred(pred(ind));
                hit := info.reference_array[ i ];
                i   := pred(i);
                total := info.reference_array[ i ];
                info.reference_array[ ind ] :=
                      ak42evalpercent_real (hit, total);
                END;
            (* PTS 1111134 E.Z. *)
            (*ENDIF*)
        (*ENDIF*)
        a40move_counter (acv, a41v, info.reference_array[ind], NOT c_null, 0, NOT c_to_undef);
        END;
    (*ENDFOR*)
    END
(*ENDIF*)
;cccprint('<ak42ctrl_caches');
END;

(*------------------------------*)

PROCEDURE
        ak42out_monitor ;



VAR
      hit   : tsp00_Longreal;
      total : tsp00_Longreal;
      i     : tgg00_RefInfoIndex;

BEGIN
cccprint('AK42  ak42out_mo');
(* PTS 1103735 E.Z. *)
(* PTS 1111134 E.Z. *)
IF  ind in [ idata_cache_percent,
    (* PTS 1105315 E.Z. *)
    (* PTS 1115020 E.Z. *)
    (* PTS 1115043 E.Z. *)
    iak_cache_percent,
    isseq_cache_percent,
    idata_obj_cache_percent, idata_obj_log_cache_percent,
    (* PTS 1106423 TS 2000-04-20 *)
    (* PTS 1111572 E.Z. *)
    idata_history_cache_percent,
    (* PTS 1106423 *)
    (* PTS 1111134 E.Z. *)
    idata_sql_cache_percent (* PTS 1109469 *)
    ]
THEN
    BEGIN (* evaluate percentage value *)
    i   := pred(pred(ind));
    hit := info.reference_array[ i ];
    i   := pred(i);
    total := info.reference_array[ i ];
    IF  (hit >= 0) AND (total >= 0)
    THEN
        info.reference_array[ ind ] :=
              ak42evalpercent_real (hit, total)
    ELSE
        info.reference_array[ ind ] := -1
    (*ENDIF*)
    END;
(* PTS 1103735 E.Z. *)
(* PTS 1111134 E.Z. *)
(* PTS 1111572 E.Z. *)
(*ENDIF*)
IF  (ind >= idata_cache_total) AND (ind <= idata_history_cache_percent)
THEN
    a41v.a4ti := cak_i_caches;
(*ENDIF*)
IF  (ind >= isql_commands) AND (ind <= iins_sorts_rows_ins )
THEN
    a41v.a4ti := cak_i_load;
(* PTS 1103033 E.Z. *)
(*ENDIF*)
IF  (ind  >= iavg_lock_used) AND (ind <= irequest_timeouts)
THEN
    a41v.a4ti := cak_i_lock;
(*ENDIF*)
IF  (ind >= iread_log) AND (ind <= iavg_sum_pagewaits)
THEN
    a41v.a4ti := cak_i_log;
(* PTS 1103033 E.Z. *)
(*ENDIF*)
IF  (ind >= iread_string) AND (ind <= iwrite_string)
THEN
    a41v.a4ti := cak_i_long;
(* PTS 1115043 E.Z. *)
(*ENDIF*)
IF  (ind >= ir_v_acc) AND (ind <= ipsm_w_p)
THEN
    a41v.a4ti := cak_i_pages;
(*ENDIF*)
IF  (ind >= iadd_rec) AND (ind <= icget_inv)
THEN
    a41v.a4ti := cak_i_row;
(* PTS 1103033 E.Z. *)
(*ENDIF*)
IF  (ind >= ifunccount) AND (ind <= ikb_calls)
THEN
    a41v.a4ti := cak_i_transaction;
(* PTS 1115043 E.Z. *)
(*ENDIF*)
IF  (info.reference_array[ ind ] >= 0)
THEN
    (* h.b. '* 1.0' looks silly but is necessary because
          of missing cast in pascal *)
    ak42ref_stat_real (acv, a41v, a42monitor_arr [ind], info.reference_array [ind] * 1.0);
(* PTS 1103119 E.Z. *)
(* PTS 1103247 E.Z. *)
(*ENDIF*)
IF  g01is_livecache
THEN
    (* PTS 1103735 E.Z. *)
    (* PTS 1111134 E.Z. *)
    (* PTS 1111572 E.Z. *)
    IF  ind = idata_history_cache_percent
    THEN
        ak42data_cache_statistics (acv, a41v)
    (*ENDIF*)
(*ENDIF*)
;cccprint('<ak42out_monitor');
END;

(* PTS 1111572 E.Z. *)
(*------------------------------*)

PROCEDURE
        ak42data_cache_statistics ;



VAR
      DataCacheSize      : tsp00_Int4;
      OmsHistoryPageCnt  : tsp00_Int4;
      OmsDataPageCnt     : tsp00_Int4;
      SqlDataPageCnt     : tsp00_Int4;

BEGIN
cccprint('AK42  ak42data_c');
a41v.a4ti := cak_i_caches;
(* k56iobjlog_info and obj_log_cache_info removed: PTS 1105395 JA 2000-03-07 *)
(* PTS 1111572 E.Z. *)
b01get_data_cache_workload (acv.a_transinf.tri_trans,
      DataCacheSize, OmsHistoryPageCnt, OmsDataPageCnt, SqlDataPageCnt);
ak42ref_i4_statistic (acv, a41v,
      'Data cache no of SQL data pages         ', SqlDataPageCnt);
ak42ref_i4_statistic (acv, a41v,
      'Data cache no of OMS data pages         ', OmsDataPageCnt);
(* PTS 1111572 E.Z. *)
ak42ref_i4_statistic (acv, a41v,
      'Data cache no of history/undo pages     ', OmsHistoryPageCnt);
(* PTS 1111572 E.Z. *)
;cccprint('<ak42data_cache_');
END;

(* PTS 1115043 E.Z. *)
(* following to be destroyed with PTS 1115090 *)
(*------------------------------*)

FUNCTION
        ak42connected_users ;



VAR
      b_err     : tgg_basis_error;
      aux_file  : tgg00_FileId;
      sm_desc   : tak_sm_description;
      mblock    : tgg_mess_block;
      data      : tsp_buf;
      qual      : tgg_qual_buf;
      cnt          : integer;
      set_result   : tgg_bd_set_result_record;
      tree_pos     : tgg00_FilePos;
      zerokey      : tgg_lkey;
      b            : tsp_buf;

BEGIN
cccprint('AK42  ak42connec');
WITH acv, a41v DO
    BEGIN
    g04build_temp_tree_id (aux_file, a_transinf.tri_trans);
    aux_file.fileTempCnt_gg00 := 1;
    aux_file.fileTfnTemp_gg00 := ttfnSaveScheme_egg00;
    aux_file.fileType_gg00    := aux_file.fileType_gg00 + [ ftsShared_egg00 ];
    g01mblock_init (a_transinf.tri_trans,
          m_show, mm_user, mblock);
    g01datapart_init (mblock, @data, sizeof (data));
    mblock.mb_struct           := mbs_buf;
    mblock.mb_data^.mbp_buf[1] := chr(ord(g01code.ctype));
    mblock.mb_data_len         := 1;
    g01qual_init (mblock, @qual, sizeof (qual));
    a06set_mtree (acv, mblock, aux_file);
    mblock.mb_qual_len := sizeof (aux_file);
    WITH sm_desc DO
        BEGIN (* The error kb_net_line_down will be ignored *)
        sm_error_cnt := 0;
        sm_errorlist[1] := e_net_line_down;
        sm_errorlist[2] := e_net_disconnected;
        sm_return_req   := false;
        sm_must_local   := true;
        sm_not_local := false
        END;
    (*ENDWITH*)
    a06ignore_rsend_mess_buf (acv, mblock, sm_desc, b_err);
    IF  b_err <> e_ok
    THEN
        a07_b_put_error (acv, b_err, 1)
    ELSE
        BEGIN
        WITH set_result DO
            BEGIN
            bd_key_check_len:= 0;
            bd_max_rec_cnt  := csp_maxint2;
            bd_max_fill_len := sizeof (b);
            bd_next         := false;
            END;
        (*ENDWITH*)
        zerokey.len     := 0;
        tree_pos.tpsPno_gg00  := NIL_PAGE_NO_GG00;
        cnt := 0;
        REPEAT
            b07cnext_record (a_transinf.tri_trans, aux_file,
                  zerokey, set_result, tree_pos, b);
            b_err := a_transinf.tri_trans.trError_gg00;
            IF  (b_err = e_ok) OR (b_err = e_key_not_found) OR
                (b_err = e_buffer_limit)
            THEN
                BEGIN
                cnt := cnt + set_result.bd_rec_cnt;
                IF  b_err = e_buffer_limit
                THEN
                    b_err := e_ok
                ELSE
                    b_err := e_no_next_record;
                (*ENDIF*)
                END;
            (*ENDIF*)
        UNTIL
            (b_err <> e_ok) OR (a_return_segm^.sp1r_returncode <> 0);
        (*ENDREPEAT*)
        b01destroy_file (a_transinf.tri_trans, aux_file);
        ak42connected_users := cnt;
        END;
    (*ENDIF*)
    END;
(*ENDWITH*)
;cccprint('<ak42connected_u');
END;

(*------------------------------*)

PROCEDURE
        ak42datacache_hits ;



VAR
      info    : tgg00_ReferenceInfo;
      total   : tsp00_Longreal;
      hit     : tsp00_Longreal;

BEGIN
cccprint('AK42  ak42dataca');
WITH acv, info DO
    BEGIN
    b01show_monitor  (a_transinf.tri_trans.trTaskId_gg00, info);
    total := info.reference_array[ idata_cache_total ];
    hit   := info.reference_array[ idata_cache_hit ];
    IF  (hit >= 0) AND (total >= 0)
    THEN
        datacache_hitrate := ak42evalpercent_real (hit, total)
    ELSE
        datacache_hitrate := 0;
    (*ENDIF*)
    datacache_oms_hitrate := 0;
    datacache_oms_log_hitrate := 0;
    total := info.reference_array[ idata_obj_cache_total ];
    hit   := info.reference_array[ idata_obj_cache_hit ];
    IF  (hit >= 0) AND (total >= 0)
    THEN
        datacache_oms_hitrate := ak42evalpercent_real (hit, total);
    (*ENDIF*)
    total := info.reference_array[ idata_obj_log_cache_total ];
    hit   := info.reference_array[ idata_obj_log_cache_hit ];
    IF  (hit >= 0) AND (total >= 0)
    THEN
        datacache_oms_log_hitrate := ak42evalpercent_real (hit, total);
    (*ENDIF*)
    END;
(*ENDWITH*)
;cccprint('<ak42datacache_h');
END;

(*------------------------------*)

PROCEDURE
        ak42dbm_state ;



VAR
      autosave_on               : boolean;
      really_bad                : boolean;
      datacache_hitrate         : tsp00_Int4;
      datacache_oms_hitrate     : tsp00_Int4;
      datacache_oms_log_hitrate : tsp00_Int4;
      cnt                       : tsp00_Int4;
      devno                     : tsp00_Int4;
      dev_type                  : tsp2_dev_type;
      devname                   : tsp2_devname;
      yes_no                    : ARRAY[boolean] OF tsp_c3;
      on_off                    : ARRAY[boolean] OF tsp_c3;

BEGIN
cccprint('AK42  ak42dbm_st');
a40get_catalog_table (acv, a41v);
IF  acv.a_return_segm^.sp1r_returncode = 0
THEN
    BEGIN
    yes_no[true]  := 'Yes';
    yes_no[false] := 'No ';
    on_off[true]  := 'On ';
    on_off[false] := 'Off';
    a40move_i4    (acv, a41v, g01page_size DIV 1024, NOT c_null, 0, NOT c_to_undef);
    a40move_i4    (acv, a41v, g01maxuser,            NOT c_null, 0, NOT c_to_undef);
    (* following to be changed with PTS 1115090 *)
    a40move_i4    (acv, a41v, ak42connected_users (acv, a41v),
          NOT c_null, 0, NOT c_to_undef);
    ak42datacache_hits (acv, datacache_hitrate,
          datacache_oms_hitrate, datacache_oms_log_hitrate);
    a40move_i4    (acv, a41v, datacache_hitrate, NOT c_null, 0, NOT c_to_undef);
    a40move_const (acv, a41v, @yes_no [b01db_full(acv.a_transinf.tri_trans.trTaskId_gg00)], 3);
    a40move_const (acv, a41v, @yes_no [b01connect_allowed], 3);
    a40move_const (acv, a41v, @on_off [a01diag_monitor_on], 3);
    a40move_const (acv, a41v, @on_off [c_monitor_on], 3);
    a40move_const (acv, a41v, @on_off [g01vtrace.vtrAny_gg00], 3);
    autosave_on := k38is_on_autosave (acv.a_transinf.tri_trans, false);
    a40move_const (acv, a41v, @on_off [autosave_on], 3);
    a40move_i4    (acv, a41v,
          b01get_bad_index_cnt(acv.a_transinf.tri_trans.trTaskId_gg00), NOT c_null, 0, NOT c_to_undef);
    a40move_const (acv, a41v, @yes_no
          [kb560LogIsFull(acv.a_transinf.tri_trans.trTaskId_gg00)], 3);
    cnt := 0;
    dev_type := sp2dt_log;
    devno    := 1;
    WHILE dev_type <> sp2dt_trace DO
        BEGIN
        b01next_bad_dev (acv.a_transinf.tri_trans.trTaskId_gg00, dev_type,
              devno, devname, really_bad);
        IF  devname <> b01blankfilename
        THEN
            IF  really_bad
            THEN
                cnt := succ(cnt);
            (*ENDIF*)
        (*ENDIF*)
        END;
    (*ENDWHILE*)
    a40move_i4        (acv, a41v, cnt, NOT c_null, 0, NOT c_to_undef);
    a40move_i4        (acv, a41v, datacache_oms_hitrate, NOT c_null, 0, NOT c_to_undef);
    a40move_i4        (acv, a41v, datacache_oms_log_hitrate, NOT c_null, 0, NOT c_to_undef);
    END;
(*ENDIF*)
;cccprint('<ak42dbm_state  ');
END;

(* PTS 1113491 *)
(*------------------------------*)

PROCEDURE
        ak42HeapContent ;



BEGIN
cccprint('AK42  ak42HeapCo');
a40get_catalog_table (acv, a41v);
IF  acv.a_return_segm^.sp1r_returncode = 0
THEN
    BEGIN
    ak341MonitorHeapContent(acv, a41v);
    END;
(*ENDIF*)
;cccprint('<ak42HeapContent');
END;

(*------------------------------*)

PROCEDURE
        ak42history_info ;



VAR
      HistFileNo       : tsp00_Int4;
      FirstTransId     : tgg91_TransNo;
      FirstTransIsUsed : boolean;
      LastTransId      : tgg91_TransNo;
      LastTransIsUsed  : boolean;
      PageCount        : tsp00_Int4;
      bFileIsEmpty     : boolean;

BEGIN
cccprint('AK42  ak42histor');
IF  NOT g01is_livecache
THEN
    a07_b_put_error (acv, e_wrong_database_instance, 1)
ELSE
    BEGIN
    a40get_catalog_table (acv, a41v);
    IF  acv.a_return_segm^.sp1r_returncode = 0
    THEN
        BEGIN
        HistFileNo := 0;
        REPEAT
            kb560GetHistFileInfo (acv.a_transinf.tri_trans, HistFileNo,
                  FirstTransId, FirstTransIsUsed,
                  LastTransId, LastTransIsUsed,
                  PageCount, bFileIsEmpty);
            IF  (acv.a_transinf.tri_trans.trError_gg00 = e_ok) AND (HistFileNo >= 0)
            THEN
                BEGIN
                a40move_i4        (acv, a41v, HistFileNo-1, NOT c_null, 0, NOT c_to_undef);
                (* PTS 1112272 E.Z. *)
                ak42move_trans_no (acv, a41v, FirstTransId);
                a40move_const     (acv, a41v, @a40yes_no [FirstTransIsUsed], 3);
                ak42move_trans_no (acv, a41v, LastTransId);
                a40move_const     (acv, a41v, @a40yes_no [LastTransIsUsed], 3);
                a40move_i4        (acv, a41v, PageCount, NOT c_null, 0, NOT c_to_undef);
                a40move_const     (acv, a41v, @a40yes_no [bFileIsEmpty], 3);
                END;
            (*ENDIF*)
        UNTIL
            (HistFileNo < 0) OR (acv.a_transinf.tri_trans.trError_gg00 <> e_ok);
        (*ENDREPEAT*)
        IF  acv.a_transinf.tri_trans.trError_gg00 <> e_ok
        THEN
            a07_b_put_error (acv, acv.a_transinf.tri_trans.trError_gg00, 1)
        (*ENDIF*)
        END
    (*ENDIF*)
    END
(*ENDIF*)
;cccprint('<ak42history_inf');
END;

(*------------------------------*)

PROCEDURE
        ak42monitor_object ;



BEGIN
cccprint('AK42  ak42monito');
ak341StartStopMonitor(kw_index = cak_i_on)
;cccprint('<ak42monitor_obj');
END;

(*------------------------------*)

PROCEDURE
        ak42monitor_show ;



VAR
      ind           : tgg00_RefInfoIndex;
      info          : tgg00_ReferenceInfo;

BEGIN
cccprint('AK42  ak42monito');
WITH acv, a41v, info DO
    BEGIN
    a4sh_kind := sh_st_monitor;
    a40get_catalog_table (acv, a41v);
    IF  (a_ex_kind <> only_parsing) AND
        (a_return_segm^.sp1r_returncode = 0)
    THEN
        BEGIN
        ak42get_refinfo (acv, info);
        CASE kw_index OF
            cak_i_all :
                BEGIN
                (*========== show monitor all ==================*)
                (* PTS 1103247 E.Z. *)
                (* PTS 1103735 E.Z. *)
                (* PTS 1111134 E.Z. *)
                (* PTS 1111572 E.Z. *)
                (* PTS 1115043 E.Z. *)
                IF  g01is_livecache
                THEN
                    FOR ind := idata_cache_total TO idata_obj_log_cache_percent DO
                        (* PTS 1103033 E.Z. *)
                        ak42out_monitor (acv, a41v, info, ind)
                    (*ENDFOR*)
                ELSE
                    FOR ind := idata_cache_total TO idata_sql_cache_percent DO
                        (* PTS 1103033 E.Z. *)
                        ak42out_monitor (acv, a41v, info, ind);
                    (*ENDFOR*)
                (*ENDIF*)
                FOR ind := idata_history_cache_total TO idata_history_cache_percent DO
                    ak42out_monitor (acv, a41v, info, ind);
                (*ENDFOR*)
                (* PTS 1115043 E.Z. *)
                FOR ind := isql_commands TO ikb_calls DO
                    (* PTS 1103033 E.Z. *)
                    (* PTS 1103745 E.Z. *)
                    IF  g01is_livecache
                        OR
                        (
                        (ind <> iexternal_dbproc_calls) AND
                        (ind <> iexternal_trigger_calls)
                        )
                    THEN
                        ak42out_monitor (acv, a41v, info, ind);
                    (*ENDIF*)
                (*ENDFOR*)
                END;
            cak_i_caches :
                BEGIN
                (*========== show monitor caches ===============*)
                (* PTS 1103119 E.Z. *)
                (* PTS 1103735 E.Z. *)
                (* PTS 1111134 E.Z. *)
                (* PTS 1111572 E.Z. *)
                (* PTS 1115043 E.Z. *)
                IF  g01is_livecache
                THEN
                    FOR ind := idata_cache_total TO idata_obj_log_cache_percent DO
                        (* PTS 1103033 E.Z. *)
                        ak42out_monitor (acv, a41v, info, ind)
                    (*ENDFOR*)
                ELSE
                    FOR ind := idata_cache_total TO idata_sql_cache_percent DO
                        (* PTS 1103033 E.Z. *)
                        ak42out_monitor (acv, a41v, info, ind);
                    (*ENDFOR*)
                (*ENDIF*)
                FOR ind := idata_history_cache_total TO idata_history_cache_percent DO
                    ak42out_monitor (acv, a41v, info, ind);
                (*ENDFOR*)
                END;
            cak_i_load :
                (*========== show monitor load =================*)
                FOR ind := isql_commands TO iins_sorts_rows_ins DO
                    (* PTS 1103745 E.Z. *)
                    IF  g01is_livecache
                        OR
                        (
                        (ind <> iexternal_dbproc_calls) AND
                        (ind <> iexternal_trigger_calls)
                        )
                    THEN
                        ak42out_monitor (acv, a41v, info, ind);
                    (*ENDIF*)
                (*ENDFOR*)
            cak_i_lock :
                (*========== show monitor lock =================*)
                (* PTS 1103033 E.Z. *)
                FOR ind  := iavg_lock_used TO irequest_timeouts DO
                    ak42out_monitor (acv, a41v, info, ind);
                (*ENDFOR*)
            cak_i_log :
                (*========== show monitor log ==================*)
                FOR ind := iread_log TO iavg_sum_pagewaits DO
                    ak42out_monitor (acv, a41v, info, ind);
                (*ENDFOR*)
            cak_i_long :
                (*========== show monitor long =================*)
                (* PTS 1103033 E.Z. *)
                FOR ind := iread_string TO iwrite_string DO
                    ak42out_monitor (acv, a41v, info, ind);
                (*ENDFOR*)
            cak_i_pages :
                (*========== show monitor pages ================*)
                (* PTS 1115043 E.Z. *)
                FOR ind := ir_v_acc TO ipsm_w_p DO
                    (* PTS 1103033 E.Z. *)
                    ak42out_monitor (acv, a41v, info, ind);
                (*ENDFOR*)
            cak_i_row :
                (*========== show monitor row ==================*)
                FOR ind := iadd_rec TO icget_inv DO
                    ak42out_monitor (acv, a41v, info, ind);
                (*ENDFOR*)
            (* PTS 1103033 E.Z. *)
            cak_i_transaction :
                (*========== show monitor transaction ==========*)
                FOR ind := ifunccount TO ikb_calls DO
                    ak42out_monitor (acv, a41v, info, ind);
                (*ENDFOR*)
            (* PTS 1115043 E.Z. *)
            OTHERWISE
                a07_b_put_error (acv, e_not_implemented, 1);
            END;
        (*ENDCASE*)
        END;
    (*ENDIF*)
    END;
(*ENDWITH*)
;cccprint('<ak42monitor_sho');
END;

(*------------------------------*)

PROCEDURE
        a42_monitor_init ;



BEGIN
cccprint('AK42  a42_monito');
(*========== show monitor caches ===============*)
a42monitor_arr[ idata_cache_total ] :=
      'Data cache accesses                     ';
a42monitor_arr[ idata_cache_hit ] :=
      'Data cache accesses successful          ';
a42monitor_arr[ idata_cache_miss ] :=
      'Data cache accesses unsuccessful        ';
a42monitor_arr[ idata_cache_percent ] :=
      'Data cache hit rate (%)                 ';
(* PTS 1103735 E.Z. *)
(* PTS 1111134 E.Z. *)
(* PTS 1105315 E.Z. *)
(* PTS 1115020 E.Z. *)
a42monitor_arr[ iak_cache_total ] :=
      'Catalog cache accesses                  ';
a42monitor_arr[ iak_cache_hit ] :=
      'Catalog cache accesses successful       ';
a42monitor_arr[ iak_cache_miss ] :=
      'Catalog cache accesses unsuccessful     ';
a42monitor_arr[ iak_cache_percent ] :=
      'Catalog cache hit rate (%)              ';
a42monitor_arr[ isseq_cache_total ] :=
      'Sequence cache accesses                 ';
a42monitor_arr[ isseq_cache_hit ] :=
      'Sequence cache accesses successful      ';
a42monitor_arr[isseq_cache_miss] :=
      'Sequence cache accesses unsuccessful    ';
a42monitor_arr[isseq_cache_percent] :=
      'Sequence cache hit rate (%)             ';
(* PTS 1109469 *)
a42monitor_arr[idata_sql_cache_total] :=
      'Data cache sqlpage accesses             ';
a42monitor_arr[idata_sql_cache_hit]  :=
      'Data cache sqlpage accesses successful  ';
a42monitor_arr[idata_sql_cache_miss] :=
      'Data cache sqlpage accesses unsuccessful';
a42monitor_arr[idata_sql_cache_percent] :=
      'Data cache sqlpage hit rate(%)          ';
(* END PTS 1109469 *)
(* PTS 1115043 E.Z. *)
a42monitor_arr[idata_obj_cache_total] :=
      'Data cache OMS accesses                 ';
a42monitor_arr[idata_obj_cache_hit] :=
      'Data cache OMS accesses successful      ';
a42monitor_arr[idata_obj_cache_miss] :=
      'Data cache OMS accesses unsuccessful    ';
a42monitor_arr[idata_obj_cache_percent] :=
      'Data cache OMS hit rate (%)             ';
a42monitor_arr[idata_obj_log_cache_total] :=
      'Data cache OMS log accesses             ';
a42monitor_arr[idata_obj_log_cache_hit] :=
      'Data cache OMS log accesses successful  ';
a42monitor_arr[idata_obj_log_cache_miss] :=
      'Data cache OMS log accesses unsuccessful';
a42monitor_arr[idata_obj_log_cache_percent] :=
      'Data cache OMS log hit rate (%)         ';
(* PTS 1106423 TS 2000-04-20 *)
(* PTS 1111572 E.Z. *)
a42monitor_arr[idata_history_cache_total] :=
      'Data history/undo accesses              ';
a42monitor_arr[idata_history_cache_hit] :=
      'Data history/undo accesses successful   ';
a42monitor_arr[idata_history_cache_miss] :=
      'Data history/undo accesses unsuccessful ';
a42monitor_arr[idata_history_cache_percent] :=
      'Data history/undo hit rate (%)          ';
(* PTS 1106423 *)
(* PTS 1103735 E.Z. *)
(* PTS 1111134 E.Z. *)
(*========== show monitor load =================*)
a42monitor_arr[ isql_commands ] :=
      'SQL commands                            ';
a42monitor_arr[ iprepares ] :=
      'PREPAREs                                ';
(* PTS 1105779 E.Z. *)
a42monitor_arr[ iprepares_again ] :=
      'PREPAREs repeated                       ';
a42monitor_arr[ iexecutes ] :=
      'EXECUTEs                                ';
a42monitor_arr[ icommits ] :=
      'COMMITs                                 ';
a42monitor_arr[ irollbacks ] :=
      'ROLLBACKs                               ';
a42monitor_arr[ ilocks ] :=
      'LOCKs and UNLOCKs                       ';
a42monitor_arr[ ibegin_subtrans ] :=
      'SUBTRANS BEGINs                         ';
a42monitor_arr[ iend_subtrans ] :=
      'SUBTRANS ENDs                           ';
a42monitor_arr[ irollback_subtrans ] :=
      'SUBTRANS ROLLBACKs                      ';
a42monitor_arr[ icreates ] :=
      'CREATEs                                 ';
a42monitor_arr[ ialters ] :=
      'ALTERs                                  ';
a42monitor_arr[ idrops ] :=
      'DROPs                                   ';
a42monitor_arr[ iselects ] :=
      'SELECTs and FETCHes                     ';
a42monitor_arr[ iselects_rows_read ] :=
      'SELECTs and FETCHes, rows read          ';
a42monitor_arr[ iselects_rows_qual ] :=
      'SELECTs and FETCHes, rows qual          ';
a42monitor_arr[ iinserts ] :=
      'INSERTs                                 ';
a42monitor_arr[ iinserts_rows ] :=
      'INSERTs, rows inserted                  ';
a42monitor_arr[ iupdates ] :=
      'UPDATEs                                 ';
a42monitor_arr[ iupdates_rows_read ] :=
      'UPDATEs, rows read                      ';
a42monitor_arr[ iupdates_rows_upd ] :=
      'UPDATEs, rows updated                   ';
a42monitor_arr[ ideletes ] :=
      'DELETEs                                 ';
a42monitor_arr[ ideletes_rows_read ] :=
      'DELETEs, rows read                      ';
a42monitor_arr[ ideletes_rows_del ] :=
      'DELETEs, rows deleted                   ';
(* PTS 1103033 E.Z. *)
a42monitor_arr[ iinternal_dbproc_calls ] :=
      'Internal DBPROC calls                   ';
a42monitor_arr[ iexternal_dbproc_calls ] :=
      'External DBPROC calls                   ';
a42monitor_arr[ iinternal_trigger_calls ] :=
      'Internal trigger calls                  ';
a42monitor_arr[ iexternal_trigger_calls ] :=
      'External trigger calls                  ';
a42monitor_arr[ iprim_key ] :=
      'Primary key accesses                    ';
a42monitor_arr[ iprim_key_read ] :=
      'Primary key accesses, rows read         ';
a42monitor_arr[ iprim_key_qual ] :=
      'Primary key accesses, rows qual         ';
a42monitor_arr[ iprim_key_range ] :=
      'Primary key range accesses              ';
a42monitor_arr[ iprim_read_key_range ] :=
      'Primary key range accesses, rows read   ';
a42monitor_arr[ iprim_qual_key_range ] :=
      'Primary key range accesses, rows qual   ';
a42monitor_arr[ iindex ] :=
      'Index accesses                          ';
a42monitor_arr[ iindex_read ] :=
      'Index accesses, rows read               ';
a42monitor_arr[ iindex_qual ] :=
      'Index accesses, rows qual               ';
a42monitor_arr[ iindex_range ] :=
      'Index range accesses                    ';
a42monitor_arr[ iindex_range_read ] :=
      'Index range accesses, rows read         ';
a42monitor_arr[ iindex_range_qual ] :=
      'Index range accesses, rows qual         ';
a42monitor_arr[ iis_index ] :=
      'Isolated index accesses                 ';
a42monitor_arr[ iis_index_read ] :=
      'Isolated index accesses, rows read      ';
a42monitor_arr[ iis_index_qual ] :=
      'Isolated index accesses, rows qual      ';
a42monitor_arr[ iis_index_range ] :=
      'Isolated index range accesses           ';
a42monitor_arr[ iis_index_read_range ] :=
      'Isolated index range accesses, rows read';
a42monitor_arr[ iis_index_qual_range ] :=
      'Isolated index range accesses, rows qual';
a42monitor_arr[ itable_scan ] :=
      'Table scans                             ';
a42monitor_arr[ itable_scan_read ] :=
      'Table scans, rows read                  ';
a42monitor_arr[ itable_scan_qual ] :=
      'Table scans, rows qual                  ';
a42monitor_arr[ iis_index_scan ] :=
      'Isolated index scans                    ';
a42monitor_arr[ iis_index_scan_read ] :=
      'Isolated index scans, rows read         ';
a42monitor_arr[ iis_index_scan_qual ] :=
      'Isolated index scans, rows qual         ';
a42monitor_arr[ imem_sorts ] :=
      'Memory sorts / sort&merge               ';
a42monitor_arr[ imem_sorts_rows_read ] :=
      'Memory sorts / sort&merge, rows read    ';
a42monitor_arr[ iins_sorts ] :=
      'Sorts by insertion                      ';
a42monitor_arr[ iins_sorts_rows_ins ] :=
      'Sorts by insertion, rows inserted       ';
(*========== show monitor lock =================*)
a42monitor_arr[ iavg_lock_used ] :=
      'Lock list avg used entries              ';
a42monitor_arr[ imax_lock_used ] :=
      'Lock list max used entries              ';
a42monitor_arr[ icollisions ] :=
      'Lock list collisions                    ';
a42monitor_arr[ iescalations ] :=
      'Lock list escalations                   ';
a42monitor_arr[ inew_rows ] :=
      'Lock list inserted row   entries        ';
a42monitor_arr[ inew_tabs ] :=
      'Lock list inserted table entries        ';
(* PTS 1103033 E.Z. *)
a42monitor_arr[ ideadlocks ] :=
      'Detected deadlocks                      ';
a42monitor_arr[ irequest_timeouts ] :=
      'Request timeouts                        ';
(*========== show monitor log ==================*)
a42monitor_arr[ iread_log ] :=
      'Log page physical reads                 ';
a42monitor_arr[ iwrite_log ] :=
      'Log page physical writes                ';
a42monitor_arr[ ilog_queue_size ] :=
      'Log queue pages                         ';
a42monitor_arr[ imaxqueue_used ] :=
      'Log queue max used pages                ';
a42monitor_arr[ icount_insert ] :=
      'Log queue inserts                       ';
a42monitor_arr[ icount_request ] :=
      'Log queue overflows                     ';
a42monitor_arr[ icount_group ] :=
      'Log queue group commits                 ';
a42monitor_arr[ icount_wait ] :=
      'Log queue waits for log page write      ';
a42monitor_arr[ imaxwait_logpage ] :=
      'Log queue max waits per log page        ';
a42monitor_arr[ iavg_sum_pagewaits ] :=
      'Log queue avg waits per log page        ';
(*========== show monitor long =================*)
a42monitor_arr[ iread_string ] :=
      'BD read   LONG                          ';
a42monitor_arr[ iwrite_string ] :=
      'BD write  LONG                          ';
(* PTS 1103033 E.Z. *)
(*========== show monitor pages ================*)
a42monitor_arr[ ir_v_acc ] :=
      'Virtual  reads                          ';
a42monitor_arr[ iw_v_acc ] :=
      'Virtual  writes                         ';
a42monitor_arr[ ir_p_acc ] :=
      'Physical reads                          ';
a42monitor_arr[ iw_p_acc ] :=
      'Physical writes                         ';
a42monitor_arr[ icatalog_r_v ] :=
      'Catalog        virtual  reads           ';
a42monitor_arr[ icatalog_w_v ] :=
      'Catalog        virtual  writes          ';
a42monitor_arr[ icatalog_r_p ] :=
      'Catalog        physical reads           ';
a42monitor_arr[ icatalog_w_p ] :=
      'Catalog        physical writes          ';
(* PTS 1115043 E.Z. *)
a42monitor_arr[ iperm_r_v ] :=
      'Perm page      virtual  reads           ';
a42monitor_arr[ iperm_w_v ] :=
      'Perm page      virtual  writes          ';
a42monitor_arr[ iperm_r_p ] :=
      'Perm page      physical reads           ';
a42monitor_arr[ iperm_w_p ] :=
      'Perm page      physical writes          ';
a42monitor_arr[ itemp_r_v ] :=
      'Temp page      virtual  reads           ';
a42monitor_arr[ itemp_w_v ] :=
      'Temp page      virtual  writes          ';
a42monitor_arr[ itemp_r_p ] :=
      'Temp page      physical reads           ';
a42monitor_arr[ itemp_w_p ] :=
      'Temp page      physical writes          ';
(* PTS 1103033 E.Z. *)
a42monitor_arr[ ibstr_r_v ] :=
      'LONG page      virtual  reads           ';
a42monitor_arr[ ibstr_w_v ] :=
      'LONG page      virtual  writes          ';
a42monitor_arr[ ibstr_r_p ] :=
      'LONG page      physical reads           ';
a42monitor_arr[ ibstr_w_p ] :=
      'LONG page      physical writes          ';
a42monitor_arr[ ilevel0_r_v ] :=
      'Leaf page      virtual  reads           ';
a42monitor_arr[ ilevel0_w_v ] :=
      'Leaf page      virtual  writes          ';
a42monitor_arr[ ilevel0_r_p ] :=
      'Leaf page      physical reads           ';
a42monitor_arr[ ilevel0_w_p ] :=
      'Leaf page      physical writes          ';
a42monitor_arr[ ilevel1_r_v ] :=
      'Level1 page    virtual  reads           ';
a42monitor_arr[ ilevel1_w_v ] :=
      'Level1 page    virtual  writes          ';
a42monitor_arr[ ilevel1_r_p ] :=
      'Level1 page    physical reads           ';
a42monitor_arr[ ilevel1_w_p ] :=
      'Level1 page    physical writes          ';
a42monitor_arr[ ilevel2_r_v ] :=
      'Level2 page    virtual  reads           ';
a42monitor_arr[ ilevel2_w_v ] :=
      'Level2 page    virtual  writes          ';
a42monitor_arr[ ilevel2_r_p ] :=
      'Level2 page    physical reads           ';
a42monitor_arr[ ilevel2_w_p ] :=
      'Level2 page    physical writes          ';
a42monitor_arr[ ilevel3_r_v ] :=
      'Level3 page    virtual  reads           ';
a42monitor_arr[ ilevel3_w_v ] :=
      'Level3 page    virtual  writes          ';
a42monitor_arr[ ilevel3_r_p ] :=
      'Level3 page    physical reads           ';
a42monitor_arr[ ilevel3_w_p ] :=
      'Level3 page    physical writes          ';
a42monitor_arr[ ilevel4_r_v ] :=
      'Level4 page    virtual  reads           ';
a42monitor_arr[ ilevel4_w_v ] :=
      'Level4 page    virtual  writes          ';
a42monitor_arr[ ilevel4_r_p ] :=
      'Level4 page    physical reads           ';
a42monitor_arr[ ilevel4_w_p ] :=
      'Level4 page    physical writes          ';
a42monitor_arr[ ilevel5_r_v ] :=
      'Level5 page    virtual  reads           ';
a42monitor_arr[ ilevel5_w_v ] :=
      'Level5 page    virtual  writes          ';
a42monitor_arr[ ilevel5_r_p ] :=
      'Level5 page    physical reads           ';
a42monitor_arr[ ilevel5_w_p ] :=
      'Level5 page    physical writes          ';
a42monitor_arr[ ipsm_r_v ] :=
      'Converter page virtual  reads           ';
a42monitor_arr[ ipsm_w_v ] :=
      'Converter page virtual  writes          ';
a42monitor_arr[ ipsm_r_p ] :=
      'Converter page physical reads           ';
a42monitor_arr[ ipsm_w_p ] :=
      'Converter page physical writes          ';
(*========== show monitor row ==================*)
a42monitor_arr[ iadd_rec ] :=
      'BD add  record perm                     ';
a42monitor_arr[ icadd_rec ] :=
      'BD add  record temp                     ';
a42monitor_arr[ irepl_rec ] :=
      'BD repl record perm                     ';
a42monitor_arr[ icrepl_rec ] :=
      'BD repl record temp                     ';
a42monitor_arr[ idel_rec ] :=
      'BD del  record perm                     ';
a42monitor_arr[ icdel_rec ] :=
      'BD del  record temp                     ';
a42monitor_arr[ iget_rec ] :=
      'BD get  record perm                     ';
a42monitor_arr[ icget_rec ] :=
      'BD get  record temp                     ';
a42monitor_arr[ inext_rec ] :=
      'BD next record perm                     ';
a42monitor_arr[ icnext_rec ] :=
      'BD next record temp                     ';
a42monitor_arr[ iprev_rec ] :=
      'BD prev record perm                     ';
a42monitor_arr[ icprev_rec ] :=
      'BD prev record temp                     ';
a42monitor_arr[ isel_direc ] :=
      'BD select direct record                 ';
a42monitor_arr[ isel_next ] :=
      'BD select next   record                 ';
a42monitor_arr[ isel_prev ] :=
      'BD select prev   record                 ';
a42monitor_arr[ iadd_inv ] :=
      'BD add to   index list perm             ';
a42monitor_arr[ icadd_inv ] :=
      'BD add to   index list temp             ';
a42monitor_arr[ idel_inv ] :=
      'BD del from index list perm             ';
a42monitor_arr[ icdel_inv ] :=
      'BD del from index list temp             ';
a42monitor_arr[ iget_inv ] :=
      'BD get      index list perm             ';
a42monitor_arr[ icget_inv ] :=
      'BD get      index list temp             ';
(* PTS 1103033 E.Z. *)
(*========== show monitor transaction ==========*)
a42monitor_arr[ ifunccount ] :=
      'SQL commands                            ';
a42monitor_arr[ itranscount ] :=
      'Write transactions                      ';
a42monitor_arr[ ikb_calls ] :=
      'KB calls                                ';
(* PTS 1115043 E.Z. *)
ak42refresh_cnt      := 0;
ak42fast_refresh_cnt := 0;
ak42init_running_cmd_list;
(* PTS 1103033 E.Z. *)
a42monitor_init (process_id);
;cccprint('<a42_monitor_ini');
END;

(*------------------------------*)

PROCEDURE
        ak42monitor_oms ;



CONST
      c_sys_dispid = 5;
      c_debug      = true;

TYPE

      I386Stack = RECORD (* PTS 1107879 *)
            plong     : ^tsp_int4;
            pBuf      : ^tgg00_COMMonitorTransferBuf;
      END;


VAR
      handle     : tsp00_Addr;
      iid        : tsp00_C16;
      dispid     : tsp00_Int4;
      dp         : tsp_dcom_dispatch_parms;
      bufLen     : tsp_int4;
      buf        : tgg00_COMMonitorTransferBuf;
      stack      : ARRAY[1..2] OF tak_dbproc_stack_param_debug;
      pStack     : ^I386Stack;
      params     : tsp_dcom_monitor_parms;

BEGIN
cccprint('AK42  ak42monito');
(* PTS 1105440 *)
(* PTS 1107243 *)
(* &if $MACH not in [ NMP ] *)
IF  NOT reset_statistics
THEN
    BEGIN
    a40get_catalog_table (acv, a41v);
    IF  acv.a_return_segm^.sp1r_returncode = 0
    THEN
        BEGIN
        handle := NIL;
        WHILE ak341GetMonitorInfo (acv, handle, iid, dispid, buf.ctbMonInfo_gg00) DO
            (* must be called until false is returned. Otherwise resources *)
            (* in liboms are not released !!                               *)
            ak42AggrMethodMonitorInfo (acv, iid, dispid, buf.ctbMonInfo_gg00);
        (*ENDWHILE*)
        END;
    (*ENDIF*)
    END;
(*ENDIF*)
IF  acv.a_return_segm^.sp1r_returncode = 0
THEN
    BEGIN
    dp.ifactory          := NIL;
    dp.session_context   := @acv;
    dp.dispid            := c_sys_dispid;
    dp.hresult           := 0;
    dp.param_stack       := @stack;
    dp.param_stack_len   := sizeof(stack);
    (*dp.debug_mode        := false; PTS 1112663 *)
    (*dp.debug_location    := NIL;   PTS 1112663 *)
    dp.errtext           := bsp_c64;
    dp.monitoring        := false;
    buf.ctbMessType_gg00 := m_diagnose;
    IF  a260BuildDebugStack(*(NOT c_debug)PTS 1112663 *) (* PTS 1107879 *)
    THEN
        BEGIN
        stack[1].dbg_value.ptr := @bufLen;
        stack[1].dbg_vt        := csp1_vt_i4;
        stack[1].dbg_inout     := sp1io_inout;
        stack[1].dbg_is_ptr    := true;
        stack[1].dbg_length    := sizeof (bufLen);
        stack[2].dbg_value.ptr := @buf;
        stack[2].dbg_vt        := csp1_vt_userdefined;
        stack[2].dbg_inout     := sp1io_inout;
        stack[2].dbg_is_ptr    := true;
        stack[2].dbg_length    := sizeof (buf);
        END
    ELSE
        BEGIN
        pStack        := @stack;
        pStack^.plong := @bufLen;
        pStack^.pBuf  := @buf;
        END;
    (*ENDIF*)
    vdcom_monitor_open (params);
    REPEAT
        IF  params.retcode = 0
        THEN
            BEGIN
            dp.idispatch  := params.idispatch;
            IF  reset_statistics
            THEN
                buf.ctbDispId_gg00 := -1
            ELSE
                buf.ctbDispId_gg00 := 0;
            (*ENDIF*)
            REPEAT
                bufLen := sizeof(buf) - sizeof (buf.ctbMonInfo_gg00);
                vdcom_dbproc (dp);
                IF  NOT reset_statistics AND (buf.ctbDispId_gg00 >= 0) AND (bufLen > 0)
                THEN
                    ak42AggrMethodMonitorInfo (acv, params.iid, buf.ctbDispId_gg00, buf.ctbMonInfo_gg00);
                (*ENDIF*)
                IF  buf.ctbDispId_gg00 >= 0
                THEN
                    buf.ctbDispId_gg00 := buf.ctbDispId_gg00 + 1
                (*ENDIF*)
            UNTIL
                (buf.ctbDispId_gg00 < 0) OR reset_statistics OR (dp.hresult <> 0);
            (*ENDREPEAT*)
            vdcom_monitor_next (params);
            IF  dp.hresult <> 0
            THEN
                a07ak_system_error (acv, 42, 1);
            (*ENDIF*)
            END;
        (*ENDIF*)
    UNTIL
        params.retcode <> 0;
    (*ENDREPEAT*)
    vdcom_monitor_close (params);
    IF  reset_statistics
    THEN
        ak341ResetMonitor(acv)
    ELSE
        BEGIN
        ak42MonitorOmsBuildUserResult(acv, a41v);
        b01empty_file (acv.a_transinf.tri_trans, acv.a_into_tree)
        END;
    (*ENDIF*)
    END
(* PTS 1105440 *)
(* PTS 1107243 *)
(* &endif*)
(*ENDIF*)
;cccprint('<ak42monitor_oms');
END;

(*------------------------------*)

PROCEDURE
        ak42AggrMethodMonitorInfo ;



VAR
      exitLoop    : boolean;
      monEnum     : tgg00_COMMonEnum;
      key         : tak42_monitor_rec_key;
      monitor_rec : tak42_oms_monitor_rec;

BEGIN
cccprint('AK42  ak42AggrMe');
key.iid              := iid;
key.dispid           := dispid;
key.lkey.keyLen_gg00 := sizeof(key.iid) + sizeof(key.dispid);
b07cget_record (acv.a_transinf.tri_trans, acv.a_into_tree,
      key.lkey, monitor_rec.rec);
IF  acv.a_transinf.tri_trans.trError_gg00 = e_key_not_found
THEN
    BEGIN
    monitor_rec.rec.recLen_gg00    :=
          sizeof(monitor_rec.offset) +
          sizeof(monitor_rec.iid)    +
          sizeof(monitor_rec.dispid) +
          sizeof(monitor_rec.filler) +
          sizeof(monitor_rec.info);
    monitor_rec.rec.recKeyLen_gg00 := sizeof(monitor_rec.iid) +
          sizeof(monitor_rec.dispid);
    monitor_rec.rec.recVarcolOffset_gg00 := 0;
    monitor_rec.rec.recVarcolCnt_gg00    := 0;
    monitor_rec.iid    := iid;
    monitor_rec.dispid := dispid;
    monitor_rec.info   := monInfo;
    b07cadd_record (acv.a_transinf.tri_trans, acv.a_into_tree, monitor_rec.rec);
    END
ELSE
    BEGIN
    monitor_rec.info.cmiCallCnt_gg00 := monitor_rec.info.cmiCallCnt_gg00 + monInfo.cmiCallCnt_gg00;
    monEnum  := cmiRunTime;
    exitLoop := false;
    WHILE NOT exitLoop DO
        BEGIN
        CASE monEnum OF
            cmiRunTimeMin, cmiMinWaitNewConsView, cmiMallocMin, cmiFreeMin, cmiDeltaMin,
            cmiMinReadApplCallbacks, cmiMinStreamRowsRead, cmiMinWriteApplCallbacks,
            cmiMinStreamRowsWrite :
                BEGIN
                IF  monInfo.cmiCounters_gg00[monEnum] <
                    monitor_rec.info.cmiCounters_gg00[monEnum]
                THEN
                    monitor_rec.info.cmiCounters_gg00[monEnum] :=
                          monInfo.cmiCounters_gg00[monEnum]
                (*ENDIF*)
                END;
            cmiRunTimeMax, cmiMaxSubtransLevel,cmiMaxWaitNewConsView,
            cmiMallocMax,  cmiFreeMax, cmiCacheSize, cmiDeltaMax,
            cmiMaxReadApplCallbacks, cmiMaxStreamRowsRead, cmiMaxWriteApplCallbacks,
            cmiMaxStreamRowsWrite :
                BEGIN
                IF  monInfo.cmiCounters_gg00[monEnum] >
                    monitor_rec.info.cmiCounters_gg00[monEnum]
                THEN
                    monitor_rec.info.cmiCounters_gg00[monEnum] :=
                          monInfo.cmiCounters_gg00[monEnum]
                (*ENDIF*)
                END;
            cmiDeltaAvg :
                BEGIN
                monitor_rec.info.cmiCounters_gg00[cmiDeltaAvg] :=
                      (monitor_rec.info.cmiCounters_gg00[cmiDeltaAvg] *
                      (monitor_rec.info.cmiCallCnt_gg00 - monInfo.cmiCallCnt_gg00) +
                      monInfo.cmiCounters_gg00[cmiDeltaAvg]) / monitor_rec.info.cmiCallCnt_gg00;
                END;
            OTHERWISE
                monitor_rec.info.cmiCounters_gg00[monEnum] :=
                      monitor_rec.info.cmiCounters_gg00[monEnum] +
                      monInfo.cmiCounters_gg00[monEnum];
            END;
        (*ENDCASE*)
        IF  monEnum < cmiCacheSize
        THEN (* PTS 1107850 *)
            monEnum := succ(monEnum)
        ELSE
            exitLoop := true;
        (*ENDIF*)
        END;
    (*ENDWHILE*)
    b07crepl_record (acv.a_transinf.tri_trans, acv.a_into_tree, monitor_rec.rec);
    END;
(*ENDIF*)
IF  acv.a_transinf.tri_trans.trError_gg00 <> e_ok
THEN
    a07_b_put_error (acv, acv.a_transinf.tri_trans.trError_gg00, 1);
(*ENDIF*)
;cccprint('<ak42AggrMethodM');
END;

(*------------------------------*)

PROCEDURE
        ak42MonitorOmsBuildUserResult ;



VAR
      monEnum     : tgg00_COMMonEnum;
      b_err       : tgg00_BasisError;
      avg         : tsp00_8ByteCounter;
      monitor_rec : tak42_oms_monitor_rec;
      set_result  : tgg_bd_set_result_record;
      tree_pos    : tgg00_FilePos;
      zerokey     : ^tgg_lkey;

BEGIN
cccprint('AK42  ak42Monito');
IF  acv.a_return_segm^.sp1r_returncode = 0
THEN
    BEGIN
    WITH set_result DO
        BEGIN
        bd_key_check_len:= 0;
        bd_max_rec_cnt  := 1;
        bd_max_fill_len := sizeof (monitor_rec);
        bd_next         := true;
        END;
    (*ENDWITH*)
    zerokey              := @monitor_rec;
    zerokey^.len         := 0;
    tree_pos.tpsPno_gg00 := NIL_PAGE_NO_GG00;
    REPEAT
        b07cnext_record (acv.a_transinf.tri_trans,  acv.a_into_tree,
              zerokey^, set_result, tree_pos, monitor_rec.rec.recBuf_gg00);
        b_err := acv.a_transinf.tri_trans.trError_gg00;
        IF  (b_err = e_ok) OR (b_err = e_key_not_found)
        THEN
            BEGIN
            b_err := e_ok;
            a40move_guid (acv, a41v, monitor_rec.iid);
            a40move_i4    (acv, a41v,
                  monitor_rec.dispid, NOT c_null, 0, NOT c_to_undef);
            a40move       (acv, a41v, @monitor_rec.info.cmiMethodName_gg00,
                  sizeof (monitor_rec.info.cmiMethodName_gg00));
            a40move_counter (acv, a41v,
                  monitor_rec.info.cmiCallCnt_gg00, NOT c_null, 0, NOT c_to_undef);
            monEnum := cmiRunTime;
            WHILE monEnum < cmiCacheSize DO
                BEGIN
                IF  monEnum in [cmiSumVarObjSize, cmiSumWaitNewConsView]
                THEN
                    BEGIN
                    IF  monitor_rec.info.cmiCounters_gg00[succ(monEnum)] > 0
                    THEN
                        BEGIN
                        avg := trunc(monitor_rec.info.cmiCounters_gg00[monEnum] /
                              monitor_rec.info.cmiCounters_gg00[succ(monEnum)]);
                        a40move_counter (acv, a41v, avg, NOT c_null, 0, NOT c_to_undef);
                        END
                    ELSE
                        a40move (acv, a41v, NIL, 0);
                    (*ENDIF*)
                    monEnum := succ(monEnum)
                    END
                ELSE
                    IF  monEnum in [cmiSumReadApplCallbacks, cmiSumStreamRowsRead,
                        cmiSumWriteApplCallbacks, cmiSumStreamRowsWrite]
                    THEN
                        BEGIN
                        IF  monitor_rec.info.cmiCallCnt_gg00 > 0
                        THEN
                            BEGIN
                            avg := trunc(monitor_rec.info.cmiCounters_gg00[monEnum] /
                                  monitor_rec.info.cmiCallCnt_gg00);
                            a40move_counter (acv, a41v, avg, NOT c_null, 0, NOT c_to_undef);
                            END
                        ELSE
                            a40move (acv, a41v, NIL, 0);
                        (*ENDIF*)
                        END
                    ELSE
                        a40move_counter (acv, a41v,
                              monitor_rec.info.cmiCounters_gg00[monEnum],
                              monitor_rec.info.cmiCounters_gg00[monEnum] = csp_maxint4, 0, NOT c_to_undef);
                    (*ENDIF*)
                (*ENDIF*)
                monEnum := succ(monEnum)
                END;
            (*ENDWHILE*)
            a40move_counter (acv, a41v,
                  monitor_rec.info.cmiCounters_gg00[cmiCacheSize],
                  monitor_rec.info.cmiCounters_gg00[cmiCacheSize] < 1, 0, NOT c_to_undef);
            END;
        (*ENDIF*)
    UNTIL
        (b_err <> e_ok) OR (acv.a_return_segm^.sp1r_returncode <> 0);
    (*ENDREPEAT*)
    END;
(*ENDIF*)
;cccprint('<ak42MonitorOmsB');
END;

(*------------------------------*)

PROCEDURE
        ak42ResetOmsMonitor ;



VAR
      a41v : tak40_show_glob;

BEGIN
cccprint('AK42  ak42ResetO');
ak42monitor_oms (acv, a41v, c_reset_statistics)
;cccprint('<ak42ResetOmsMon');
END;

(*------------------------------*)

PROCEDURE
        ak42init_running_cmd_list ;



VAR
      ok         : boolean;
      alloc_sum  : tsp00_Int4;

BEGIN
cccprint('AK42  ak42init_r');
alloc_sum := g01maxuser * sizeof (tak_parsid);
g01allocate_msg (csp3_n_dynpool, 'DYNP_A42_PARSEID       :',
      alloc_sum);
vallocat (alloc_sum, a42parseid_list, ok);
IF  ok
THEN
    s10fil2 (alloc_sum, a42parseid_list^, 1,
          alloc_sum, csp_defined_byte);
(*ENDIF*)
;cccprint('<ak42init_runnin');
END;

(*------------------------------*)

PROCEDURE
        a42put_running_cmds ;



VAR
      pidpos          : integer;
      first_user_pid  : integer;

BEGIN
cccprint('AK42  a42put_run');
WITH acv DO
    BEGIN
    vgetfirstuserpid(first_user_pid);
    (* h.b. PTS 1104483 *)
    IF  (a_transinf.tri_trans.trTaskId_gg00 >=  first_user_pid) AND
        (a_transinf.tri_trans.trTaskId_gg00 <=
        (first_user_pid + g01maxuser))
    THEN
        BEGIN
        pidpos := ((a_transinf.tri_trans.trTaskId_gg00 - first_user_pid)
              * sizeof (parsid)) + 1;
        g10mv7 ('VAK42 ',   2,
              sizeof (parsid), g01maxuser * sizeof (parsid),
              parsid, 1, a42parseid_list^, pidpos, sizeof(parsid),
              a_return_segm^.sp1r_returncode);
        END;
    (*ENDIF*)
    END;
(*ENDWITH*)
;cccprint('<a42put_running_');
END;

(*------------------------------*)

FUNCTION
        a42getparseid ;



VAR
      pidpos          : integer;
      first_user_pid  : integer;

BEGIN
cccprint('AK42  a42getpars');
a42getparseid := false;
vgetfirstuserpid(first_user_pid);
IF  (pid >=  first_user_pid) AND
    (pid <= (first_user_pid + g01maxuser))
THEN
    BEGIN
    pidpos := ((pid - first_user_pid) * sizeof (parsid)) + 1;
    g10mv8 ('VAK42 ',   3,
          g01maxuser * sizeof (parsid), sizeof (parsid),
          a42parseid_list^, pidpos, parsid, 1, sizeof(parsid),
          b_err);
    IF  parsid <> c_not_in_use
    THEN
        BEGIN
        parsid[11]    := chr(0);
        (* because pid_appl_info is chr(0) in SYSPARSEID *)
        a42getparseid := true;
        END;
    (*ENDIF*)
    END;
(*ENDIF*)
;cccprint('<a42getparseid  ');
END;

(*------------------------------*)

PROCEDURE
        a42reset_running_cmds ;



VAR
      pidpos          : integer;
      parsid          : tsp_c12;
      first_user_pid  : integer;

BEGIN
cccprint('AK42  a42reset_r');
WITH acv DO
    BEGIN
    parsid  := c_not_in_use;
    vgetfirstuserpid(first_user_pid);
    (* h.b. PTS 1104483 *)
    IF  (a_transinf.tri_trans.trTaskId_gg00 >=  first_user_pid) AND
        (a_transinf.tri_trans.trTaskId_gg00 <=
        (first_user_pid + g01maxuser))
    THEN
        BEGIN
        pidpos  := ((a_transinf.tri_trans.trTaskId_gg00 - first_user_pid)
              * sizeof (parsid)) + 1;
        g10mv7 ('VAK42 ',   4,
              sizeof (parsid), g01maxuser * sizeof (parsid),
              parsid, 1, a42parseid_list^, pidpos, sizeof(parsid),
              a_return_segm^.sp1r_returncode);
        END;
    (*ENDIF*)
    END;
(*ENDWITH*)
;cccprint('<a42reset_runnin');
END;

(*------------------------------*)

PROCEDURE
        ak42oms_memory ;



VAR
      totalHeap         : tsp00_8ByteCounter;
      spaceInFreeChunks : tsp00_8ByteCounter;
      reserved          : tsp00_8ByteCounter;
      mallocated        : tsp00_8ByteCounter;
      emergencySize     : tsp00_8ByteCounter;
      emergencyInUse    : tsp00_8ByteCounter;
      emergencyMaxUsed  : tsp00_8ByteCounter;

BEGIN
cccprint('AK42  ak42oms_me');
a40get_catalog_table (acv, a41v);
IF  (acv.a_return_segm^.sp1r_returncode = 0) AND
    (acv.a_ex_kind <> only_parsing)          AND
    g01is_livecache
THEN
    BEGIN
    ak341OmsHeapInfo(acv, totalHeap, spaceInFreeChunks, reserved, mallocated,
          emergencySize, emergencyInUse, emergencyMaxUsed);
    a40move_counter (acv, a41v, totalHeap, NOT c_null, 0, NOT c_to_undef);
    a40move_counter (acv, a41v, spaceInFreeChunks, NOT c_null, 0, NOT c_to_undef);
    a40move_counter (acv, a41v, reserved, NOT c_null, 0, NOT c_to_undef);
    a40move_counter (acv, a41v, mallocated, NOT c_null, 0, NOT c_to_undef);
    a40move_counter (acv, a41v, emergencySize, NOT c_null, 0, NOT c_to_undef);
    a40move_counter (acv, a41v, emergencyInUse, NOT c_null, 0, NOT c_to_undef);
    a40move_counter (acv, a41v, emergencyMaxUsed, NOT c_null, 0, NOT c_to_undef);
    END;
(*ENDIF*)
;cccprint('<ak42oms_memory ');
END;

(*------------------------------*)

PROCEDURE
        ak42OmsLockObjInfo ;



VAR
      exitLoop : boolean;
      handle   : tsp00_Addr;
      lockInfo : tgg00_OmsLockInfo;

BEGIN
cccprint('AK42  ak42OmsLoc');
a40get_catalog_table (acv, a41v);
IF  (acv.a_return_segm^.sp1r_returncode = 0) AND
    (acv.a_ex_kind <> only_parsing)
THEN
    BEGIN
    handle   := NIL;
    exitLoop := false;
    WHILE NOT exitLoop AND ak341NextOmsLockObjInfo(handle, lockInfo) DO
        BEGIN
        a40move       (acv, a41v, @lockInfo.oli_handle, sizeof(lockInfo.oli_handle));
        a40move_i4    (acv, a41v, lockInfo.oli_taskid, NOT c_null, 0, NOT c_to_undef);
        a40move_const (acv, a41v, @lockInfo.oli_lockmode, sizeof (lockInfo.oli_lockmode));
        a40move_const (acv, a41v, @lockInfo.oli_requestmode, sizeof (lockInfo.oli_requestmode));
        a40move_i4    (acv, a41v, lockInfo.oli_timeout, lockInfo.oli_timeout < 0, 0, NOT c_to_undef);
        IF  handle = NIL
        THEN
            exitLoop := true;
        (*ENDIF*)
        END;
    (*ENDWHILE*)
    END;
(*ENDIF*)
;cccprint('<ak42OmsLockObjI');
END;

(*------------------------------*)

PROCEDURE
        ak42show_running_cmds ;



VAR
      b_err     : tgg00_BasisError;
      aux_file  : tgg00_FileId;
      mblock    : tgg00_MessBlock;
      data      : tsp_buf;
      qual      : tgg_qual_buf;

BEGIN
cccprint('AK42  ak42show_r');
WITH acv, a41v DO
    BEGIN
    a40get_catalog_table (acv, a41v);
    IF  (a_return_segm^.sp1r_returncode = 0) AND
        (a_ex_kind <> only_parsing)
    THEN
        BEGIN
        g04build_temp_tree_id (aux_file, a_transinf.tri_trans);
        aux_file.fileTempCnt_gg00 := 1;
        aux_file.fileTfnTemp_gg00 := ttfnSaveScheme_egg00;
        aux_file.fileType_gg00    := aux_file.fileType_gg00 + [ ftsShared_egg00 ];
        g01mblock_init (acv.a_transinf.tri_trans, m_show, mm_running_cmds, mblock);
        g01datapart_init (mblock, @data, sizeof (data));
        g01stack_init    (mblock, NIL, 0, @qual, sizeof (qual));
        mblock.mb_qual^.mtree := aux_file;
        a06rsend_mess_buf (acv, mblock, cak_return_req, b_err);
        IF  b_err <> e_ok
        THEN
            a07_b_put_error (acv, b_err, 1)
        ELSE
            ak42add_parseid (acv, a41v, aux_file);
        (*ENDIF*)
        b01destroy_file (a_transinf.tri_trans, aux_file);
        END;
    (*ENDIF*)
    END;
(*ENDWITH*)
;cccprint('<ak42show_runnin');
END;

(*------------------------------*)

PROCEDURE
        ak42add_parseid ;



VAR
      b_err        : tgg_basis_error;
      rec_len      : tsp_int_map_c2;
      key_len      : tsp_int_map_c2;
      pos          : integer;
      set_result   : tgg_bd_set_result_record;
      tree_pos     : tgg00_FilePos;
      zerokey      : tgg_lkey;
      b            : tsp_buf;

BEGIN
cccprint('AK42  ak42add_pa');
WITH acv, a41v DO
    BEGIN
    WITH set_result DO
        BEGIN
        bd_key_check_len:= 0;
        bd_max_rec_cnt  := csp_maxint2;
        bd_max_fill_len := sizeof (b);
        bd_next         := false;
        END;
    (*ENDWITH*)
    zerokey.len          := 0;
    tree_pos.tpsPno_gg00 := NIL_PAGE_NO_GG00;
    REPEAT
        b07cnext_record (a_transinf.tri_trans, tree,
              zerokey, set_result, tree_pos, b);
        b_err := a_transinf.tri_trans.trError_gg00;
        IF  (b_err = e_ok) OR (b_err = e_key_not_found) OR
            (b_err = e_buffer_limit)
        THEN
            BEGIN
            pos    := 1;
            WHILE pos < set_result.bd_fill_len DO
                BEGIN
                rec_len.map_c2[1] := b[pos  ];
                rec_len.map_c2[2] := b[pos+1];
                key_len.map_c2[1] := b[pos+2];
                key_len.map_c2[2] := b[pos+3];
                t01buf (ak_sem, b, 1, rec_len.map_int);
                a40move_i4 (acv, a41v, s20or4b (b, pos + cgg_rec_key_offset),
                      NOT c_null, 0, NOT c_to_undef);
                a40move (acv, a41v,
                      @b[pos+cgg_rec_key_offset+key_len.map_int], mxsp_c12);
                pos := pos + rec_len.map_int
                END;
            (*ENDWHILE*)
            IF  b_err = e_buffer_limit
            THEN
                b_err := e_ok
            ELSE
                b_err := e_no_next_record;
            (*ENDIF*)
            END;
        (*ENDIF*)
    UNTIL
        (b_err <> e_ok) OR (a_return_segm^.sp1r_returncode <> 0);
    (*ENDREPEAT*)
    END;
(*ENDWITH*)
;cccprint('<ak42add_parseid');
END;

(* PTS 1106007 *)
(*------------------------------*)

PROCEDURE
        ak42consistent_view ;



VAR
      b_err        : tgg_basis_error;
      rec_len      : tsp_int_map_c2;
      key_len      : tsp_int_map_c2;
      pos          : integer;
      set_result   : tgg_bd_set_result_record;
      tree_pos     : tgg00_FilePos;
      zerokey      : tgg_lkey;
      b            : tsp_buf;
      data         : tgg00_BufLenInfo;
      qual         : tgg00_QualBuf;
      mblock       : tgg00_MessBlock;
      pShow        : ^tkb05_ConsistViewShow;

BEGIN
cccprint('AK42  ak42consis');
WITH acv, a41v DO
    BEGIN
    a40get_catalog_table (acv, a41v);
    IF  a_return_segm^.sp1r_returncode = 0
    THEN
        BEGIN
        g01mblock_init (acv.a_transinf.tri_trans,
              m_show, mm_object, mblock);
        g01datapart_init (mblock, @data, sizeof (data));
        g01stack_init    (mblock, NIL, 0, @qual, sizeof (qual));
        mblock.mb_qual^.mtree := a_into_tree;
        a06rsend_mess_buf (acv, mblock,
              cak_return_req, b_err);
        IF  b_err = e_ok
        THEN
            BEGIN
            WITH set_result DO
                BEGIN
                bd_key_check_len:= 0;
                bd_max_rec_cnt  := csp_maxint2;
                bd_max_fill_len := sizeof (b);
                bd_next         := false;
                END;
            (*ENDWITH*)
            zerokey.len          := 0;
            tree_pos.tpsPno_gg00 := NIL_PAGE_NO_GG00;
            REPEAT
                b07cnext_record (a_transinf.tri_trans,  a_into_tree,
                      zerokey, set_result, tree_pos, b);
                b_err := a_transinf.tri_trans.trError_gg00;
                IF  (b_err = e_ok) OR (b_err = e_key_not_found) OR
                    (b_err = e_buffer_limit)
                THEN
                    BEGIN
                    pos  := 1;
                    WHILE pos < set_result.bd_fill_len DO
                        BEGIN
                        rec_len.map_c2[1] := b[pos  ];
                        rec_len.map_c2[2] := b[pos+1];
                        key_len.map_c2[1] := b[pos+2];
                        key_len.map_c2[2] := b[pos+3];
                        t01buf (ak_sem, b, 1, rec_len.map_int);
                        pShow := @b[pos];
                        IF  gg06IsNilTrans (pShow^.cshConsistView_kb05) (* PTS 1108234 JA 2001-01-19 *)
                        THEN
                            a40move (acv, a41v, NIL, 0)  (* undef *)
                        ELSE
                            a40move (acv, a41v, @pShow^.cshConsistView_kb05,
                                  sizeof(pShow^.cshConsistView_kb05));
                        (*ENDIF*)
                        IF  gg06IsNilTrans (pShow^.cshOpenTrans_kb05)   (* PTS 1108234 JA 2001-01-19 *)
                        THEN
                            a40move (acv, a41v, NIL, 0)  (* undef *)
                        ELSE
                            a40move (acv, a41v, @pShow^.cshOpenTrans_kb05,
                                  sizeof(pShow^.cshOpenTrans_kb05));
                        (*ENDIF*)
                        a40put_date_time (acv, a41v, pShow^.cshStartDate_kb05,
                              c_is_date, pShow^.cshStartDate_kb05 = 0);
                        a40put_date_time (acv, a41v, pShow^.cshStartTime_kb05,
                              NOT c_is_date, pShow^.cshStartTime_kb05 = 0);
                        a40move_i4 (acv, a41v, pShow^.cshTimeLastUsed_kb05,
                              NOT c_null, 0, NOT c_to_undef);
                        pos := pos + rec_len.map_int
                        END;
                    (*ENDWHILE*)
                    IF  b_err = e_buffer_limit
                    THEN
                        b_err := e_ok
                    ELSE
                        b_err := e_no_next_record;
                    (*ENDIF*)
                    END;
                (*ENDIF*)
            UNTIL
                (b_err <> e_ok) OR (a_return_segm^.sp1r_returncode <> 0);
            (*ENDREPEAT*)
            END;
        (*ENDIF*)
        b01empty_file (a_transinf.tri_trans, a_into_tree)
        END;
    (*ENDIF*)
    END;
(*ENDWITH*)
;cccprint('<ak42consistent_');
END;

(*------------------------------*)

PROCEDURE
        ak42roots ;



VAR
      use_tree       : boolean;
      table_found    : boolean;
      res            : tsp00_NumError;
      tree           : tgg00_FileId;
      owner          : tsp00_KnlIdentifier;
      tablen         : tsp00_KnlIdentifier;
      owner_ptr      : tsp00_MoveObjPtr;
      tablen_ptr     : tsp00_MoveObjPtr;
      indexname      : tsp00_KnlIdentifier;
      indexn_ptr     : tsp00_MoveObjPtr;
      indexn_len     : integer;
      f_type         : tsp00_C18;
      base_ptr       : tak_sysbufferaddress;
      col_ptr        : tak00_colinfo_ptr;
      number         : tsp00_ResNum;
      index_scan_rec : tak_index_scan_record;

BEGIN
cccprint('AK42  ak42roots ');
tree := b01niltree_id;
a40get_catalog_table (acv, a41v);
REPEAT
    use_tree := false;
    b01next_fdir_entry (acv.a_transinf.tri_trans, tree);
    IF  acv.a_transinf.tri_trans.trError_gg00 = e_ok
    THEN
        BEGIN
        s41p4int (number, 1, tree.fileRoot_gg00, res);
        IF  res = num_ok
        THEN
            use_tree := a722test_col_qual (acv,
                  a41v, 'ROOT              ',
                  csp_defined_byte, @number, sizeof (number))
        ELSE
            use_tree := true;
        (*ENDIF*)
        acv.a_transinf.tri_trans.trError_gg00 := e_ok
        END;
    (*ENDIF*)
    IF  use_tree
    THEN
        BEGIN
        owner_ptr  := NIL;
        tablen_ptr := NIL;
        indexn_ptr := NIL;
        IF  tree.fileTfn_gg00 in [tfnMulti_egg00,
            tfnTable_egg00, tfnShortScol_egg00]
        THEN
            BEGIN
            a06_systable_get (acv, d_release, tree.fileTabId_gg00,
                  base_ptr, NOT c_get_all, table_found);
            IF  table_found
            THEN
                BEGIN
                owner     := a01_il_b_identifier;
                indexname := a01_il_b_identifier;
                a06determine_username (acv,
                      base_ptr^.sbase.bauthid, owner);
                tablen := base_ptr^.sbase.btablen^;
                CASE tree.fileTfn_gg00 OF
                    tfnMulti_egg00 :
                        BEGIN
                        a24fnd_indexno (acv, tree.fileTabId_gg00,
                              ord (tree.fileTfnNo_gg00[1]),
                              index_scan_rec);
                        IF  acv.a_return_segm^.sp1r_returncode <> 0
                        THEN (* could not read index catalog info *)
                            acv.a_return_segm^.sp1r_returncode := 0
                        ELSE
                            WITH index_scan_rec DO
                                IF  isr_buf <> NIL
                                THEN
                                    BEGIN
                                    a24get_indexname (acv, isr_buf, isr_index,
                                          indexname);
                                    indexn_ptr := @indexname;
                                    indexn_len := sizeof (indexname)
                                    END;
                                (*ENDIF*)
                            (*ENDWITH*)
                        (*ENDIF*)
                        f_type := 'NAMED INDEX       ';
                        END;
                    tfnTable_egg00 :
                        f_type := 'TABLE             ';
                    tfnShortScol_egg00 :
                        f_type := 'SHORT STRING FILE ';
                    END;
                (*ENDCASE*)
                owner_ptr  := @owner;
                tablen_ptr := @tablen;
                END;
            (*ENDIF*)
            END
        ELSE
            BEGIN
            acv.a_transinf.tri_trans.trError_gg00 := e_ok;
            CASE tree.fileTfn_gg00 OF
                tfnSys_egg00 :
                    f_type := 'SYS               ';
                tfnTempInv_egg00 :
                    f_type := 'TEMP INV          ';
                tfnTemp_egg00 :
                    f_type := 'TEMP              ';
                tfnColumn_egg00 :
                    f_type := 'LONG COLUMN       ';
                OTHERWISE
                    f_type := 'UNKNOWN           ';
                END;
            (*ENDCASE*)
            END;
        (*ENDIF*)
        a40move       (acv, a41v, @tree.fileTabId_gg00, sizeof (tree.fileTabId_gg00));
        a40move       (acv, a41v, owner_ptr,   sizeof (owner));
        a40move       (acv, a41v, tablen_ptr,  sizeof (tablen));
        a40move       (acv, a41v, indexn_ptr,  indexn_len);
        a40move_const (acv, a41v, @f_type,     sizeof (f_type));
        a40move_i4    (acv, a41v,
              tree.fileRoot_gg00, NOT c_null, 0, NOT c_to_undef);
        END;
    (*ENDIF*)
UNTIL
    acv.a_transinf.tri_trans.trError_gg00 <> e_ok;
(*ENDREPEAT*)
;cccprint('<ak42roots      ');
END;

(*------------------------------*)

PROCEDURE
        ak42files ;



CONST
      c_do_estimate = true;

VAR
      table_found    : boolean;
      e              : tgg00_BasisError;
      next_e         : tgg00_BasisError;
      pageCount      : tsp00_Int4;
      NumAllPages    : tsp00_Int4;
      NumFreePages   : tsp00_Int4;
      NumEmptyPages  : tsp00_Int4;
      NumKeyLeafPages: tsp00_Int4;
      Occupancy      : tsp00_Int4;
      tree           : tgg00_FileId;
      owner          : tsp00_KnlIdentifier;
      tablen         : tsp00_KnlIdentifier;
      owner_ptr      : tsp00_MoveObjPtr;
      tablen_ptr     : tsp00_MoveObjPtr;
      indexname      : tsp00_KnlIdentifier;
      indexn_ptr     : tsp00_MoveObjPtr;
      indexn_len     : integer;
      f_type         : tsp00_C18;
      base_ptr       : tak_sysbufferaddress;
      col_ptr        : tak00_colinfo_ptr;
      number         : tsp00_ResNum;
      index_scan_rec : tak_index_scan_record;

BEGIN
cccprint('AK42  ak42files ');
tree := b01niltree_id;
a40get_catalog_table (acv, a41v);
REPEAT
    b01next_fdir_entry (acv.a_transinf.tri_trans, tree);
    next_e := acv.a_transinf.tri_trans.trError_gg00;
    IF  (next_e = e_ok)
    THEN
        BEGIN
        owner_ptr  := NIL;
        tablen_ptr := NIL;
        indexn_ptr := NIL;
        IF  tree.fileTfn_gg00 in [tfnMulti_egg00,
            tfnTable_egg00, tfnShortScol_egg00]
        THEN
            BEGIN
            a06_systable_get (acv, d_release, tree.fileTabId_gg00,
                  base_ptr, NOT c_get_all, table_found);
            IF  table_found
            THEN
                BEGIN
                owner     := a01_il_b_identifier;
                indexname := a01_il_b_identifier;
                a06determine_username (acv,
                      base_ptr^.sbase.bauthid, owner);
                tablen := base_ptr^.sbase.btablen^;
                CASE tree.fileTfn_gg00 OF
                    tfnMulti_egg00 :
                        BEGIN
                        a24fnd_indexno (acv, tree.fileTabId_gg00,
                              ord (tree.fileTfnNo_gg00[1]),
                              index_scan_rec);
                        IF  acv.a_return_segm^.sp1r_returncode <> 0
                        THEN (* could not read index catalog info *)
                            acv.a_return_segm^.sp1r_returncode := 0
                        ELSE
                            WITH index_scan_rec DO
                                IF  isr_buf <> NIL
                                THEN
                                    BEGIN
                                    a24get_indexname (acv, isr_buf, isr_index,
                                          indexname);
                                    indexn_ptr := @indexname;
                                    indexn_len := sizeof (indexname)
                                    END;
                                (*ENDIF*)
                            (*ENDWITH*)
                        (*ENDIF*)
                        f_type := 'NAMED INDEX       ';
                        END;
                    tfnTable_egg00 :
                        f_type := 'TABLE             ';
                    tfnShortScol_egg00 :
                        f_type := 'SHORT STRING FILE ';
                    END;
                (*ENDCASE*)
                owner_ptr  := @owner;
                tablen_ptr := @tablen;
                END;
            (*ENDIF*)
            END
        ELSE
            BEGIN
            acv.a_transinf.tri_trans.trError_gg00 := e_ok;
            CASE tree.fileTfn_gg00 OF
                tfnSys_egg00 :
                    f_type := 'SYS               ';
                tfnTempInv_egg00 :
                    f_type := 'TEMP INV          ';
                tfnTemp_egg00 :
                    f_type := 'TEMP              ';
                tfnColumn_egg00 :
                    f_type := 'LONG COLUMN       ';
                tfnObj_egg00 :
                    f_type := 'OBJECT            ';
                OTHERWISE
                    f_type := 'UNKNOWN           ';
                END;
            (*ENDCASE*)
            END;
        (*ENDIF*)
        IF  tree.fileTfn_gg00 = tfnObj_egg00
        THEN
            BEGIN
            ak341GetObjFileInfo (acv.a_transinf.tri_trans, tree,
                  NumAllPages, NumFreePages, NumEmptyPages, NumKeyLeafPages, Occupancy);
            pageCount := NumAllPages + NumKeyLeafPages
            END
        ELSE
            BEGIN
            a06get_page_cnt (acv, tree, NOT c_do_estimate, pageCount, e);
            IF  e <> e_ok
            THEN
                pageCount := -1
            (*ENDIF*)
            END;
        (*ENDIF*)
        a40move       (acv, a41v, @tree, sizeof (tree));
        a40move       (acv, a41v, @tree.fileTabId_gg00, sizeof (tree.fileTabId_gg00));
        a40move       (acv, a41v, owner_ptr,   sizeof (owner));
        a40move       (acv, a41v, tablen_ptr,  sizeof (tablen));
        a40move       (acv, a41v, indexn_ptr,  indexn_len);
        a40move_const (acv, a41v, @f_type,     sizeof (f_type));
        a40move_i4    (acv, a41v,
              tree.fileRoot_gg00, NOT c_null, 0, NOT c_to_undef);
        a40move_i4    (acv, a41v,
              pageCount, pageCount < 0, 0, NOT c_to_undef);
        END;
    (*ENDIF*)
UNTIL
    next_e <> e_ok;
(*ENDREPEAT*)
;cccprint('<ak42files      ');
END;

(*------------------------------*)

PROCEDURE
        ak42InfoGCStats ;



CONST
      c_null          = true;
      c_zero_to_undef = true;

VAR
      taskId                : tsp00_TaskId;
      active                : boolean;
      activateCount         : tsp00_8ByteCounter;
      deleteObj             : tsp00_8ByteCounter;
      relHistCreateObjFile  : tsp00_8ByteCounter;
      relHistDeleteObj      : tsp00_8ByteCounter;
      relHistDropObjFile    : tsp00_8ByteCounter;
      relHistLockObj        : tsp00_8ByteCounter;
      relHistInsertObj      : tsp00_8ByteCounter;
      relHistNewObj         : tsp00_8ByteCounter;
      relHistUpdateObj      : tsp00_8ByteCounter;
      relHistEntryNormal    : tsp00_8ByteCounter;
      relHistEntrySensitive : tsp00_8ByteCounter;
      relHistEntryTruncate  : tsp00_8ByteCounter;
      relPageEmpty                  : tsp00_8ByteCounter;
      relPageDropFile       : tsp00_8ByteCounter;
      relDropFile           : tsp00_8ByteCounter;
      ix                    : tsp00_Int4;

BEGIN
cccprint('AK42  ak42InfoGC');
a40get_catalog_table (acv, a41v);
FOR ix := 1 TO g01maxobjgarbagecoll DO
    BEGIN
    IF  acv.a_return_segm^.sp1r_returncode = 0
    THEN
        BEGIN
        bd91InfoGCStats ( ix, taskId, active, activateCount, deleteObj,
              relHistEntryNormal, relHistEntrySensitive, relHistEntryTruncate,
              relHistCreateObjFile, relHistDeleteObj, relHistDropObjFile , relHistLockObj,
              relHistInsertObj, relHistNewObj, relHistUpdateObj,
              relPageEmpty, relPageDropFile, relDropFile );
        a40move_i4 (acv, a41v, taskId, NOT c_null, 0, NOT c_zero_to_undef);
        a40move_const (acv, a41v, @a40yes_no [active], 3);
        a40move_counter (acv, a41v, activateCount, NOT c_null, 0, NOT c_zero_to_undef);
        a40move_counter (acv, a41v, deleteObj, NOT c_null, 0, NOT c_zero_to_undef);
        a40move_counter (acv, a41v, relHistEntryNormal, NOT c_null, 0, NOT c_zero_to_undef);
        a40move_counter (acv, a41v, relHistEntrySensitive, NOT c_null, 0, NOT c_zero_to_undef);
        a40move_counter (acv, a41v, relHistEntryTruncate, NOT c_null, 0, NOT c_zero_to_undef);
        a40move_counter (acv, a41v, relHistCreateObjFile, NOT c_null, 0, NOT c_zero_to_undef);
        a40move_counter (acv, a41v, relHistDeleteObj, NOT c_null, 0, NOT c_zero_to_undef);
        a40move_counter (acv, a41v, relHistDropObjFile, NOT c_null, 0, NOT c_zero_to_undef);
        a40move_counter (acv, a41v, relHistLockObj, NOT c_null, 0, NOT c_zero_to_undef);
        a40move_counter (acv, a41v, relHistInsertObj, NOT c_null, 0, NOT c_zero_to_undef);
        a40move_counter (acv, a41v, relHistNewObj, NOT c_null, 0, NOT c_zero_to_undef);
        a40move_counter (acv, a41v, relHistUpdateObj, NOT c_null, 0, NOT c_zero_to_undef);
        a40move_counter (acv, a41v, relPageEmpty, NOT c_null, 0, NOT c_zero_to_undef);
        a40move_counter (acv, a41v, relPageDropFile, NOT c_null, 0, NOT c_zero_to_undef);
        a40move_counter (acv, a41v, relDropFile, NOT c_null, 0, NOT c_zero_to_undef);
        END;
    (*ENDIF*)
    END;
(*ENDFOR*)
;cccprint('<ak42InfoGCStats');
END;

(*------------------------------*)

PROCEDURE
        ak42_container_roots ;



CONST
      c_null          = true;
      c_zero_to_undef = true;

VAR
      pObjFileIter          : tsp00_Addr;
      pContObjFileIter      : tsp00_Addr;

      Stackarea  : RECORD       (* PTS 1116125 FF 2002-06-10 *)
            CASE boolean OF
                true :
                    (p    : tsp00_C64);
                false :
                    (align : tsp00_Addr);
                END;
            (*ENDCASE*)

      ClassId               : tsp00_Int4;
      Root                  : tsp00_Int4;
      ContainerSequelNo     : tsp00_Int4;
      NumKeyPartitions      : tsp00_Int2;
      MaxObjBodySize        : tsp00_Int4;
      MaxObjPerPage         : tsp00_Int4;
      MaxChains       : tsp00_Int4;
      VarContainer          : boolean;
      DeleteFlag            : boolean;

BEGIN
cccprint('AK42  ak42_conta');
a40get_catalog_table (acv, a41v);
ClassId          := 0;
pObjFileIter     := NIL;
pContObjFileIter := NIL;
WHILE (ClassId >= 0) AND (acv.a_return_segm^.sp1r_returncode = 0) DO
    BEGIN
    bd91InfoContainerRoots (pObjFileIter, pContObjFileIter,
          @Stackarea.p[1], sizeof(Stackarea), ClassId, Root, ContainerSequelNo,
          NumKeyPartitions, VarContainer, DeleteFlag,
          MaxObjBodySize, MaxObjPerPage, MaxChains);
    IF  ClassId >= 0
    THEN
        BEGIN
        a40move_i4 (acv, a41v, ClassId, NOT c_null, 0, NOT c_zero_to_undef);
        a40move_i4 (acv, a41v, ContainerSequelNo, NOT c_null, 0, NOT c_zero_to_undef);
        a40move_i4 (acv, a41v, Root, NOT c_null, 0, NOT c_zero_to_undef);
        a40move_i4 (acv, a41v, NumKeyPartitions, NOT c_null, 0, NOT c_zero_to_undef);
        a40move_const (acv, a41v, @a40yes_no [VarContainer], 3);
        a40move_const (acv, a41v, @a40yes_no [DeleteFlag], 3);
        a40move_i4 (acv, a41v, MaxObjBodySize, NOT c_null, 0, NOT c_zero_to_undef);
        a40move_i4 (acv, a41v, MaxObjPerPage, NOT c_null, 0, NOT c_zero_to_undef);
        a40move_i4 (acv, a41v, MaxChains, NOT c_null, 0, NOT c_zero_to_undef);
        END;
    (*ENDIF*)
    END;
(*ENDWHILE*)
;cccprint('<ak42_container_');
END;

(*------------------------------*)

PROCEDURE
        ak42_container_chains ;



CONST
      c_null          = true;
      c_zero_to_undef = true;

VAR
      pObjFileIter      : tsp00_Addr;
      pContObjFileIter  : tsp00_Addr;

      Stackarea  : RECORD       (* PTS 1116125 FF 2002-06-10 *)
            CASE boolean OF
                true :
                    (p    : tsp00_C64);
                false :
                    (align : tsp00_Addr);
                END;
            (*ENDCASE*)

      ClassId           : tsp00_Int4;
      ContainerSequelNo : tsp00_Int4;
      ChainId           : tsp00_Int4;
      Subroot           : tsp00_Int4;
      ObjCount          : tsp00_Int4;
      PageCount         : tsp00_Int4;
      ActiveGC          : tsp00_Int4;
      RelEmptyPages     : boolean;

BEGIN
cccprint('AK42  ak42_conta');
a40get_catalog_table (acv, a41v);
ClassId          := 0;
pObjFileIter     := NIL;
pContObjFileIter := NIL;
WHILE (ClassId >= 0) AND (acv.a_return_segm^.sp1r_returncode = 0) DO
    BEGIN
    bd91InfoContainerChains (pObjFileIter, pContObjFileIter,
          @Stackarea.p[1], sizeof(Stackarea), ClassId, ContainerSequelNo, ChainId,
          Subroot, ObjCount, PageCount, ActiveGC, RelEmptyPages );
    IF  ClassId >= 0
    THEN
        BEGIN
        a40move_i4 (acv, a41v, ClassId, NOT c_null, 0, NOT c_zero_to_undef);
        a40move_i4 (acv, a41v, ContainerSequelNo, NOT c_null, 0, NOT c_zero_to_undef);
        a40move_i4 (acv, a41v, ChainId, NOT c_null, 0, NOT c_zero_to_undef);
        a40move_i4 (acv, a41v, Subroot, NOT c_null, 0, NOT c_zero_to_undef);
        a40move_i4 (acv, a41v, ObjCount, NOT c_null, 0, NOT c_zero_to_undef);
        a40move_i4 (acv, a41v, PageCount, NOT c_null, 0, NOT c_zero_to_undef);
        a40move_i4 (acv, a41v, ActiveGC, NOT c_null, 0, NOT c_zero_to_undef);
        a40move_const (acv, a41v, @a40yes_no [RelEmptyPages], 3);
        END;
    (*ENDIF*)
    END;
(*ENDWHILE*)
;cccprint('<ak42_container_');
END;

(*------------------------------*)

PROCEDURE
        ak42_container_keys ;



CONST
      c_null          = true;
      c_zero_to_undef = true;

VAR
      pObjFileIter      : tsp00_Addr;

      Stackarea  : RECORD       (* PTS 1116125 FF 2002-06-10 *)
            CASE boolean OF
                true :
                    (p    : tsp00_C64);
                false :
                    (align : tsp00_Addr);
                END;
            (*ENDCASE*)

      ClassId           : tsp00_Int4;
      KeyRoot           : tsp00_Int4;
      KeyIdx            : tsp00_Int2;

BEGIN
cccprint('AK42  ak42_conta');
a40get_catalog_table (acv, a41v);
ClassId          := 0;
pObjFileIter     := NIL;
WHILE (ClassId >= 0) AND (acv.a_return_segm^.sp1r_returncode = 0) DO
    BEGIN
    bd91InfoContainerKeys (pObjFileIter, @Stackarea.p[1], sizeof(Stackarea),
          KeyIdx, ClassId, KeyRoot);
    IF  ClassId >= 0
    THEN
        BEGIN
        a40move_i4 (acv, a41v, ClassId, NOT c_null, 0, NOT c_zero_to_undef);
        a40move_i4 (acv, a41v, KeyIdx, NOT c_null, 0, NOT c_zero_to_undef);
        a40move_i4 (acv, a41v, KeyRoot, NOT c_null, 0, NOT c_zero_to_undef);
        END;
    (*ENDIF*)
    END;
(*ENDWHILE*)
;cccprint('<ak42_container_');
END;

(*------------------------------*)

PROCEDURE
        ak42select_lock_trans ;



CONST
      c_desc_len  = 40;
      c_value_len = 12;

VAR
      b_err     : tgg00_BasisError;
      rec_len   : tsp00_IntMapC2;
      key_len   : tsp00_IntMapC2;
      rec_pos   : integer;
      offset    : integer;
      ix        : tsp00_Int4;
      set_result: tgg00_BdSetResultRecord;
      tree_pos  : tgg00_FilePos;
      zerokey   : tgg00_Lkey;
      lock_info : tkb05_LockShow;
      mm_type   : tgg00_MessType2;
      data      : tgg00_BufLenInfo;
      qual      : tgg00_QualBuf;
      mblock    : tgg00_MessBlock;

BEGIN
cccprint('AK42  ak42select');
WITH acv, a41v DO
    BEGIN
    a40get_catalog_table (acv, a41v);
    IF  a_return_segm^.sp1r_returncode = 0
    THEN
        BEGIN
        CASE a4sh_kind OF
            sh_lock :
                mm_type := mm_lock;
            sh_trans :
                mm_type := mm_subtrans;
            sh_st_lock :
                mm_type := mm_config;
            (* PTS 1109995 E.Z. *)
            sh_transfile :
                mm_type := mm_database;
            END;
        (*ENDCASE*)
        g01mblock_init (acv.a_transinf.tri_trans,
              m_show, mm_type, mblock);
        g01datapart_init (mblock, @data, sizeof (data));
        g01stack_init    (mblock, NIL, 0, @qual, sizeof (qual));
        mblock.mb_qual^.mtree := a_into_tree;
        a06rsend_mess_buf (acv, mblock,
              cak_return_req, b_err);
        IF  b_err = e_ok
        THEN
            BEGIN
            WITH set_result DO
                BEGIN
                bd_key_check_len:= 0;
                bd_max_rec_cnt  := MAX_INT2_SP00;
                bd_max_fill_len := sizeof(data);
                bd_next         := false;
                END;
            (*ENDWITH*)
            zerokey.len     := 0;
            tree_pos.tpsPno_gg00  := NIL_PAGE_NO_GG00;
            REPEAT
                b07cnext_record (a_transinf.tri_trans, a_into_tree,
                      zerokey, set_result, tree_pos, data.rb.buf);
                b_err := a_transinf.tri_trans.trError_gg00;
                IF  (b_err = e_ok) OR (b_err = e_key_not_found) OR
                    (b_err = e_buffer_limit)
                THEN
                    BEGIN
                    rec_pos := 1;
                    WHILE rec_pos < set_result.bd_fill_len DO
                        BEGIN
                        rec_len.mapC2_sp00[1] := data.rb.buf[rec_pos  ];
                        rec_len.mapC2_sp00[2] := data.rb.buf[rec_pos+1];
                        CASE a4sh_kind OF
                            sh_st_lock :
                                BEGIN
                                key_len.mapC2_sp00[1] := data.rb.buf[rec_pos+2];
                                key_len.mapC2_sp00[2] := data.rb.buf[rec_pos+3];
                                offset := cgg_rec_key_offset +
                                      key_len.mapInt_sp00;
                                (* h.b. PTS 1001082 *)
                                a40move (acv, a41v,
                                      @data.rb.buf[rec_pos + offset + 1],
                                      rec_len.mapInt_sp00 - offset -
                                      c_value_len * a01char_size - 1 - 1);
                                (* h.b. PTS 1001082 *)
                                a40move (acv, a41v,
                                      @data.rb.buf[rec_pos + offset + 1 +
                                      c_desc_len * a01char_size + 1],
                                      c_value_len * a01char_size)
                                END;
                            (* PTS 1109995 E.Z. *)
                            sh_transfile :
                                BEGIN
                                (* PTS 1106563 TS 2000-05-09 *)
                                key_len.mapC2_sp00[1] := data.rb.buf[rec_pos+2];
                                key_len.mapC2_sp00[2] := data.rb.buf[rec_pos+3];
                                offset := cgg_rec_key_offset +  key_len.mapInt_sp00;
                                (* offset := offset + 1; PTS 1107127 TS 2000-07-17 *)
                                FOR ix := 1 TO 4 DO
                                    BEGIN
                                    a40move_i4 (acv, a41v,
                                          s20or4b (data.rb.buf, rec_pos + offset + 1), (* PTS 1107127 TS 2000-07-17 *)
                                          NOT c_null, 0, NOT c_to_undef);
                                    offset := offset + sizeof (tsp00_Int4) + 1;
                                    END;
                                (*ENDFOR*)
                                (* h.b. PTS 1001082 *)
                                a40move (acv, a41v,
                                      @data.rb.buf[rec_pos + offset + 1], mxsp_c12); (* PTS 1107127 TS 2000-07-17 *)
                                offset := offset + mxsp_c12 + 1;
                                (* h.b. PTS 1001082 *)
                                a40move (acv, a41v,
                                      @data.rb.buf[rec_pos + offset + 1], mxsp_c5); (* PTS 1107127 TS 2000-07-17 *)
                                (* PTS 1106563  *)
                                END;
                            OTHERWISE
                                BEGIN
                                g10mv4 ('VAK42 ',   5,
                                      sizeof (data.rb.buf),
                                      sizeof(lock_info), data.rb.buf,
                                      rec_pos, lock_info, 1,
                                      rec_len.mapInt_sp00, b_err);
                                IF  b_err <> e_move_error
                                THEN
                                    ak42one_lock_info (acv, a41v, lock_info)
                                (*ENDIF*)
                                END;
                            END;
                        (*ENDCASE*)
                        rec_pos := rec_pos + rec_len.mapInt_sp00
                        END;
                    (*ENDWHILE*)
                    IF  b_err = e_buffer_limit
                    THEN
                        b_err := e_ok
                    ELSE
                        b_err := e_no_next_record;
                    (*ENDIF*)
                    END;
                (*ENDIF*)
            UNTIL
                (b_err <> e_ok) OR
                (a_return_segm^.sp1r_returncode <> 0);
            (*ENDREPEAT*)
            END
        ELSE
            a07_b_put_error (acv, b_err, 1);
        (*ENDIF*)
        b01empty_file (a_transinf.tri_trans, a_into_tree)
        END;
    (*ENDIF*)
    END;
(*ENDWITH*)
;cccprint('<ak42select_lock');
END;

(*------------------------------*)

PROCEDURE
        ak42one_lock_info ;



CONST
      c_do_move = true;

VAR
      ok               : boolean;
      table_found      : boolean;
      is_obj_lock      : boolean;
      obj_class_char   : char;
      c3               : tsp00_C3;
      p                : integer;
      c_index          : integer;
      rowid_pos        : integer;
      length           : integer;
      val_length       : integer;
      max_length       : integer;
      curr_move_len    : integer;
      help_tabname_len : integer;
      tabname_pos      : integer;

      userid : RECORD
            CASE boolean OF
                true :
                    (surrogate : tgg00_Surrogate);
                false :
                    (fill : tsp00_C4;
                    id   : tgg00_UserId);
                END;
            (*ENDCASE*)

      sysk         : tgg00_SysInfoKey;
      user         : tsp00_KnlIdentifier;
      owner        : tsp00_KnlIdentifier;
      tablename    : tsp00_KnlIdentifier;
      aux_line     : tsp00_Line;
      c20_1        : tsp00_C20;
      c120         : tsp00_C120;
      key_info     : tsp00_C256;
      is_trunc     : boolean;
      sysbuf       : tgg00_Rec;

BEGIN
cccprint('AK42  ak42one_lo');
WITH acv, a41v DO
    BEGIN
    WITH lock_info.lshTransShow_kb05 DO
        BEGIN
        (*
              ------------ session ==> shinfo -----------*)
        a40move_i4 (acv, a41v, gg06SessionGetInt4 (tshSession_kb05),
              NOT c_null, 0, NOT c_to_undef);
        (*
              ------------ select trans  ==> shinfo ------------*)
        ak42move_trans_no (acv, a41v, tshSelTransId_kb05);           (* PTS 1108234 JA 2001-01-19 *)
        (*
              ------------ sub trans  ==> shinfo ------------*)
        a40move_i4 (acv, a41v, tshSubtrans_kb05,                     (* PTS 1108234 JA 2001-01-19 *)
              NOT c_null, 0, NOT c_to_undef);
        (*
              ------------ write trans  ==> shinfo ------------*)
        IF  gg06IsNilTrans (tshWriteTransId_kb05)                    (* PTS 1108234 JA 2001-01-19 *)
        THEN
            a40move (acv, a41v, NIL, 0)  (* undef *)
        ELSE
            a40move (acv, a41v, @tshWriteTransId_kb05, sizeof(tshWriteTransId_kb05));
        (*ENDIF*)
        END;
    (*ENDWITH*)
    WITH lock_info.lshTransShow_kb05 DO
        BEGIN
        a40move_i4 (acv, a41v, tshTaskId_kb05,
              NOT c_null, 0, NOT c_to_undef);
        (*------------ user name ==> shinfo ------------*)
        userid.surrogate := a_curr_user_id;
        userid.id        := tshUserId_kb05;
        a06determine_username (acv, userid.surrogate, user);
        a40move (acv, a41v, @user, sizeof (user));
        (*------------ date ==> shinfo ------------*)
        a40put_date_time (acv, a41v, tshStartDate_kb05,
              c_is_date, tshStartDate_kb05 = 0);
        (*------------ time ==> shinfo ------------*)
        a40put_date_time (acv, a41v, tshStartTime_kb05,
              NOT c_is_date, tshStartTime_kb05 = 0);
        (*------------ termid ==> shinfo --------------*)
        a40move_const (acv, a41v, @tshTermId_kb05,
              sizeof (tshTermId_kb05));
        (*------------ lock timeout dummy -------------*)
        a40move (acv, a41v, NIL, 0);
        (*------------ req timeout ==> shinfo --------------*)
        IF  tshTrReqMode_kb05 <> tlmFree_ekb05
        THEN
            a40move_i4 (acv, a41v, tshReqTimeout_kb05,
                  NOT c_null, 0, NOT c_to_undef)
        ELSE
            a40move (acv, a41v, NIL, 0);
        (*ENDIF*)
        (*------------ time last write ==> shinfo --------------*)
        IF  tshTrLockMode_kb05 = tlmEotExcl_ekb05
        THEN
            a40move_i4 (acv, a41v, tshTimeWrite_kb05,
                  NOT c_null, 0, NOT c_to_undef)
        ELSE
            a40move (acv, a41v, NIL, 0)
        (*ENDIF*)
        END;
    (*ENDWITH*)
    IF  a4sh_kind = sh_lock
    THEN
        WITH lock_info.lshTransShow_kb05 DO                          (* PTS 1108234 JA 2001-01-19 *)
            BEGIN
            (*---------- lock mode/state ==> shinfo ------------*)
            ak42lock_req_mode  (acv, a41v, tshLockMode_kb05);
            ak42lock_req_state (acv, a41v, tshLockState_kb05);
            (*---------- lock request mode/state ==> shinfo ----*)
            ak42lock_req_mode  (acv, a41v, tshReqMode_kb05);
            ak42lock_req_state (acv, a41v, tshReqState_kb05);
            END
        (*ENDWITH*)
    ELSE
        WITH lock_info.lshTransShow_kb05 DO                          (* PTS 1108234 JA 2001-01-19 *)
            BEGIN
            (*---------- trans lock mode/state ==> shinfo ------------*)
            ak42lock_mode  (acv, a41v, tshTrLockMode_kb05);
            ak42lock_state (acv, a41v, tshTrLockMode_kb05);
            (*---------- trans lock request mode/state ==> shinfo ----*)
            ak42lock_mode  (acv, a41v, tshTrReqMode_kb05);
            ak42lock_state (acv, a41v, tshTrReqMode_kb05);
            END;
        (*ENDWITH*)
    (*ENDIF*)
    WITH lock_info.lshTransShow_kb05 DO                              (* PTS 1108234 JA 2001-01-19 *)
        BEGIN
        IF  gg06IsNilTrans (tshConsistView_kb05)                     (* PTS 1108234 JA 2001-01-19 *)
        THEN
            a40move (acv, a41v, NIL, 0)  (* undef *)
        ELSE
            (* PTS 1106007 *)
            a40move (acv, a41v, @tshConsistView_kb05, sizeof(tshConsistView_kb05));
        (*ENDIF*)
        ak42appl_info (acv, a41v, tshTaskId_kb05)
        END;
    (*ENDWITH*)
    IF  a4sh_kind = sh_lock
    THEN
        BEGIN
        is_obj_lock := false;
        (*---------- owner, table name ==> shinfo ----*)
        a06_systable_get (acv, d_release, lock_info.lshTabId_kb05,
              a4p_arr.pbasep, c_get_all, table_found);
        IF  acv.a_return_segm^.sp1r_returncode =
            a07_return_code (e_old_fileversion, acv.a_sqlmode)
        THEN
            acv.a_return_segm^.sp1r_returncode := 0; (* PTS 1113099 *)
        (*ENDIF*)
        IF  table_found
        THEN
            a40table_column_to_shbuf (acv, a41v, a4p_arr.pbasep, -1)
        ELSE
            IF  (a_return_segm^.sp1r_returncode =
                a07_return_code (e_lock_collision, acv.a_sqlmode))
            THEN
                BEGIN
                a_return_segm^.sp1r_returncode := 0;
                sysk           := a01defaultkey;
                sysk.stableid  := lock_info.lshTabId_kb05;
                a4p_arr.pbasep := @sysbuf;
                a36get_catalog_record (a_transinf.tri_trans,
                      sysk, a4p_arr.pbasep^);
                IF  a_transinf.tri_trans.trError_gg00 = e_ok
                THEN
                    BEGIN
                    table_found := true;
                    (* ADIS 1001989, T.A. 24.01.1999 *)
                    a06determine_username (acv,
                          a4p_arr.pbasep^.sbase.bauthid, owner);
                    a40move (acv, a41v, @owner, sizeof(owner));
                    tablename   := a01_il_b_identifier;
                    tabname_pos :=
                          sizeof (tak_baserecord) -
                          sizeof (a4p_arr.pbasep^.sbase.bcolumn) -
                          sizeof (a4p_arr.pbasep^.sbase.bptr) -
                          sizeof (a4p_arr.pbasep^.sbase.btablen) + 1;
                    g10mv ('VAK42 ',   6,
                          sizeof (sysbuf.buf), sizeof (tablename), sysbuf.buf,
                          tabname_pos + 1, tablename, 1, ord (sysbuf.buf[tabname_pos]),
                          a_return_segm^.sp1r_returncode);
                    a40move (acv, a41v, @tablename, sizeof(tablename));
                    a4p_arr.pbasep := NIL
                    END;
                (*ENDIF*)
                END;
            (*ENDIF*)
        (*ENDIF*)
        IF  NOT table_found
        THEN
            BEGIN
            IF  g01unicode
            THEN
                help_tabname_len := mxsp_c18 DIV 2
            ELSE
                help_tabname_len := mxsp_c18;
            (*ENDIF*)
            a40move (acv, a41v, NIL, 0);
            IF  lock_info.lshTabId_kb05 = g01tabid.sys2_cat.fileTabId_gg00
            THEN
                a40move_const (acv, a41v, @n_sys2_cat, help_tabname_len)
            ELSE
                BEGIN
                obj_class_char := lock_info.lshTabId_kb05 [1];
                lock_info.lshTabId_kb05 [1] := chr (0);
                IF  (lock_info.lshTabId_kb05 = cgg_zero_id)
                    AND
                    (ord (obj_class_char) > 0)
                THEN
                    BEGIN
                    a40move_const (acv, a41v, @n_obj_file, help_tabname_len);
                    is_obj_lock := true
                    END
                ELSE
                    (*---- table has been dropped in the meantime *)
                    a40move_const (acv, a41v, NIL, 0)
                (*ENDIF*)
                END
            (*ENDIF*)
            END;
        (*---------- tabid ==> shinfo -----------*)
        (*ENDIF*)
        a40move (acv, a41v, @lock_info.lshTabId_kb05, SURROGATE_MXGG00);
        (*---------- rowid len ==> shinfo -----------*)
        a40move_i4 (acv, a41v, lock_info.lshRowIdLen_kb05,
              NOT c_null, 0, c_to_undef);
        (*---------- rowid ==> shinfo ---------------*)
        IF  lock_info.lshUndefRowId_kb05
        THEN
            a40move (acv, a41v, NIL, 0)
        ELSE
            BEGIN
            (*------------- rowid hex ==> shinfo ------------*)
            IF  lock_info.lshRowIdLen_kb05
                <= sizeof (lock_info.lshRowId_kb05)
            THEN
                length := lock_info.lshRowIdLen_kb05
            ELSE
                length := sizeof (lock_info.lshRowId_kb05);
            (*ENDIF*)
            a40move (acv, a41v, @lock_info.lshRowId_kb05, length)
            END;
        (*ENDIF*)
        IF  lock_info.lshUndefRowId_kb05
        THEN
            a40move (acv, a41v, NIL, 0)
        ELSE
            BEGIN
            (*------------- rowid text => shinfo ------------*)
            IF  NOT table_found OR (a4p_arr.pbasep = NIL) (* PTS 1113367 *)
            THEN
                BEGIN
                IF  NOT is_obj_lock
                THEN
                    a40move (acv, a41v, NIL, 0)
                ELSE
                    BEGIN
                    IF  lock_info.lshRowIdLen_kb05 <= sizeof (lock_info.lshRowId_kb05)
                    THEN
                        length := lock_info.lshRowIdLen_kb05
                    ELSE
                        length := sizeof (lock_info.lshRowId_kb05);
                    (*ENDIF*)
                    a40move_const (acv, a41v, @lock_info.lshRowId_kb05, length)
                    END
                (*ENDIF*)
                END
            ELSE
                BEGIN
                IF  lock_info.lshRowIdLen_kb05 <= sizeof(tkb05_RowIdPrefix)
                THEN
                    max_length := lock_info.lshRowIdLen_kb05
                ELSE
                    max_length := sizeof(tkb05_RowIdPrefix);
                (*ENDIF*)
                c_index := a4p_arr.pbasep^.sbase.bfirstcolind;
                rowid_pos  := 1;
                p          := 1;
                ok         := true;
                WHILE (c_index <> 0) AND ok DO
                    BEGIN
                    WITH a4p_arr.pbasep^.sbase, bcolumn[c_index]^ DO
                        IF  NOT (ctkey in ccolpropset)
                            OR
                            (rowid_pos > max_length)
                        THEN
                            c_index := 0
                        ELSE
                            BEGIN
                            IF  rowid_pos + cinoutlen - 1 > max_length
                            THEN
                                IF  cdatatyp in [dfixed, dfloat]
                                THEN
                                    length := 0
                                ELSE
                                    BEGIN
                                    length := max_length - rowid_pos +1;
                                    IF  (cdatatyp = dunicode)
                                        AND (((length - 1) MOD 2) <> 0)
                                    THEN
                                        length := pred (length);
                                    (*ENDIF*)
                                    END
                                (*ENDIF*)
                            ELSE
                                length := cinoutlen;
                            (*ENDIF*)
                            IF  length > 0
                            THEN
                                BEGIN
                                IF  cdatatyp = dunicode
                                THEN
                                    g20unifill (sizeof (c120), @c120,
                                          1, sizeof (c120),
                                          csp_unicode_blank)
                                ELSE
                                    s10fil (sizeof (c120), c120,
                                          1, sizeof (c120), bsp_c1);
                                (*ENDIF*)
                                a40put_one_value (acv, a41v,
                                      bcolumn[c_index]^, NOT c_do_move,
                                      lock_info.lshRowId_kb05, rowid_pos,
                                      length, c120, val_length, ok);
                                IF  rowid_pos > 1
                                THEN
                                    ak42ascii_const_move ( @'.', 1,
                                          p, key_info,
                                          a_return_segm^.sp1r_returncode);
                                (*ENDIF*)
                                ;
                                CASE cdatatyp OF
                                    dche, dcha, dunicode,
                                    ddate, dtime, dtimestamp :
                                        ak42ascii_const_move ( @'''', 1,
                                              p, key_info,
                                              a_return_segm^.sp1r_returncode);
                                    dchb :
                                        BEGIN
                                        ak42ascii_const_move ( @'x', 1,
                                              p, key_info,
                                              a_return_segm^.sp1r_returncode);
                                        ak42ascii_const_move ( @'''', 1,
                                              p, key_info,
                                              a_return_segm^.sp1r_returncode)
                                        END;
                                    OTHERWISE:
                                        BEGIN
                                        END;
                                    END;
                                (*ENDCASE*)
                                IF  ( g01unicode
                                    AND (cdatatyp = dunicode) )
                                    OR
                                    ( NOT g01unicode
                                    AND (cdatatyp <> dunicode) )
                                THEN
                                    BEGIN
                                    g10mv1 ('VAK42 ',   7,
                                          sizeof (c120),
                                          sizeof(key_info),
                                          c120, 1, key_info, p,
                                          val_length,
                                          a_return_segm^.sp1r_returncode);
                                    p := p + val_length;
                                    END
                                ELSE
                                    IF  g01unicode
                                        AND (cdatatyp <> dunicode)
                                    THEN
                                        BEGIN
                                        ak42ascii_const_move ( @c120,
                                              val_length, p, key_info,
                                              a_return_segm^.sp1r_returncode)
                                        END
                                    ELSE
                                        BEGIN
                                        s40gbyte (c120, 1, val_length,
                                              key_info, p,
                                              val_length * 2, is_trunc);
                                        p := p + val_length * 2;
                                        END;
                                    (*ENDIF*)
                                (*ENDIF*)
                                IF  cdatatyp in [ dcha, dchb, dche,
                                    dunicode, ddate, dtime, dtimestamp ]
                                THEN
                                    ak42ascii_const_move ( @'''', 1,
                                          p, key_info,
                                          a_return_segm^.sp1r_returncode);
                                (*ENDIF*)
                                END;
                            (*ENDIF*)
                            rowid_pos := rowid_pos + cinoutlen;
                            c_index := cnextind
                            END;
                        (*ENDIF*)
                    (*ENDWITH*)
                    END;
                (*ENDWHILE*)
                IF  ok
                THEN
                    BEGIN
                    IF  max_length < lock_info.lshRowIdLen_kb05
                    THEN
                        BEGIN
                        (* key is truncated *)
                        c3 := '...';
                        ak42ascii_const_move ( @c3, 3, p, key_info,
                              a_return_segm^.sp1r_returncode);
                        curr_move_len := p - 1;
                        END
                    ELSE
                        curr_move_len := p - 1;
                    (*ENDIF*)
                    END
                ELSE
                    BEGIN
                    p := 1;
                    ak42ascii_const_move ( @' ', 1, p, key_info,
                          a_return_segm^.sp1r_returncode);
                    curr_move_len := p - 1;
                    END;
                (*ENDIF*)
                a40move (acv, a41v, @key_info, curr_move_len)
                END;
            (*ENDIF*)
            END;
        (*ENDIF*)
        END
    (*ENDIF*)
    END;
(*ENDWITH*)
;cccprint('<ak42one_lock_in');
END;

(*------------------------------*)

PROCEDURE
        ak42appl_info ;



VAR
      ok        : boolean;
      appl_pid  : tsp00_Int4;
      appl_node : tsp00_NodeId;

BEGIN
cccprint('AK42  ak42appl_i');
(* process id and node_id of application ==> shinfo *)
vgetapplpid (process_id, appl_node, appl_pid, ok);
a40move_i4 (acv, a41v, appl_pid, NOT ok, 0, NOT c_to_undef);
IF  ok
THEN
    a40move_const (acv, a41v, @appl_node, sizeof (appl_node))
ELSE
    a40move (acv, a41v, NIL, 0)
(*ENDIF*)
;cccprint('<ak42appl_info  ');
END;

(*------------------------------*)

PROCEDURE
        ak42lock_mode ;



VAR
      do_move : boolean;
      id      : tsp00_C14;

BEGIN
cccprint('AK42  ak42lock_m');
do_move := true;
CASE lock_mode OF
    tlmFree_ekb05  :
        BEGIN
        do_move := false;
        a40move (acv, a41v, NIL, 0)
        END;
    tlmShare_ekb05 :
        id := 'share         ';
    tlmExcl_ekb05     :
        id := 'exclusive     ';
    tlmEotExcl_ekb05 :
        id := 'exclusive     ';
    END;
(*ENDCASE*)
IF  do_move
THEN
    a40move_const (acv, a41v, @id, sizeof (id))
(*ENDIF*)
;cccprint('<ak42lock_mode  ');
END;

(*------------------------------*)

PROCEDURE
        ak42lock_state ;



VAR
      id : tsp00_C10;

BEGIN
cccprint('AK42  ak42lock_s');
CASE lock_mode OF
    tlmExcl_ekb05:
        BEGIN
        id := 'write     ';
        a40move_const (acv, a41v, @id, sizeof (id))
        END;
    OTHERWISE
        a40move (acv, a41v, NIL, 0)
    END
(*ENDCASE*)
;cccprint('<ak42lock_state ');
END;

(*------------------------------*)

PROCEDURE
        ak42lock_req_mode ;



VAR
      do_move : boolean;
      id      : tsp00_C14;

BEGIN
cccprint('AK42  ak42lock_r');
do_move := true;
CASE lock_mode OF
    lckFree_egg00  :
        BEGIN
        do_move := false;
        a40move (acv, a41v, NIL, 0)
        END;
    lckSysShare_egg00 :
        id := 'sys_share     ';
    lckSysExcl_egg00     :
        id := 'sys_exclusive ';
    lckTabShare_egg00 :
        id := 'tab_share     ';
    lckTabExcl_egg00 :
        id := 'tab_exclusive ';
    lckRowShare_egg00 :
        id := 'row_share     ';
    lckRowExcl_egg00 :
        id := 'row_exclusive ';
    lckRowOptimistic_egg00 :
        id := 'row_optimistic';
    END;
(*ENDCASE*)
IF  do_move
THEN
    a40move_const (acv, a41v, @id, sizeof (id))
(*ENDIF*)
;cccprint('<ak42lock_req_mo');
END;

(*------------------------------*)

PROCEDURE
        ak42lock_req_state ;



VAR
      do_move : boolean;
      id      : tsp00_C10;

BEGIN
cccprint('AK42  ak42lock_r');
do_move := true;
IF  lrsEotExcl_egg00 in lock_state
THEN
    id := 'write     '
ELSE
    IF  lrsConsistent_egg00 in lock_state
    THEN
        id := 'consistent'
    ELSE
        IF  lrsTemp_egg00 in lock_state
        THEN
            id := 'temp      '
        ELSE
            BEGIN
            do_move := false;
            a40move (acv, a41v, NIL, 0)
            END;
        (*ENDIF*)
    (*ENDIF*)
(*ENDIF*)
IF  do_move
THEN
    a40move_const (acv, a41v, @id, sizeof (id))
(*ENDIF*)
;cccprint('<ak42lock_req_st');
END;

(*------------------------------*)

PROCEDURE
        ak42serverdb_statistics ;



VAR
      db_space_info  : tgg00_DbSpaceInfo;
      log_percent    : tsp00_Int4;
      restart_date   : tsp00_Int4;
      restart_time   : tsp00_Int4;
      unused_pages   : tsp00_Int4;
      log_device_info: tkb00_LogDeviceInfo;
      log_queue_info : tkb00_LogQueueInfo;

BEGIN
cccprint('AK42  ak42server');
a40get_catalog_table (acv, a41v);
WITH acv, a41v DO
    IF  a_return_segm^.sp1r_returncode = 0
    THEN
        BEGIN
        b01dbspace_statistic (a_transinf.tri_trans, db_space_info);
        IF  a_transinf.tri_trans.trError_gg00 <> e_ok
        THEN
            a07_b_put_error (acv, a_transinf.tri_trans.trError_gg00, 1)
        ELSE
            BEGIN
            WITH db_space_info DO
                BEGIN
                a40move_i4 (acv, a41v, db_total_pages,    NOT c_null, 0, NOT c_to_undef);
                (* 12.04.2001 h.b. is not known anymore *)
                a40move_i4 (acv, a41v, NIL_PAGE_NO_GG00,  c_null, 0, NOT c_to_undef);
                a40move_i4 (acv, a41v, db_perm_pages,     NOT c_null, 0, NOT c_to_undef);
                (* CR 1113689 *)
                a40move_i4 (acv, a41v, db_max_perm_used,  NOT c_null, 0, NOT c_to_undef);
                a40move_i4 (acv, a41v, db_real_perm_used, NOT c_null, 0, NOT c_to_undef);
                a40move_i4 (acv, a41v, db_perm_percent,   NOT c_null, 0, NOT c_to_undef);
                a40move_i4 (acv, a41v, db_temp_pages,     NOT c_null, 0, NOT c_to_undef);
                a40move_i4 (acv, a41v, db_temp_percent,   NOT c_null, 0, NOT c_to_undef);
                unused_pages := db_total_pages - db_used_pages;
                a40move_i4 (acv, a41v, unused_pages,      NOT c_null, 0, NOT c_to_undef);
                a40move_i4 (acv, a41v,
                      ak42evalpercent (unused_pages, db_total_pages), NOT c_null, 0, NOT c_to_undef);
                a40move_i4 (acv, a41v, db_updated_pages,  NOT c_null, 0, NOT c_to_undef);
                a40move_i4 (acv, a41v, db_used_blocks,    NOT c_null, 0, NOT c_to_undef);
                a40move_const (acv, a41v, @a40yes_no [b01db_full(acv.a_transinf.tri_trans.trTaskId_gg00)], 3)
                END;
            (*ENDWITH*)
            (* *)
            kb560GetStatistics (a_transinf.tri_trans.trTaskId_gg00, log_device_info, log_queue_info);
            (* *)
            WITH log_device_info DO
                BEGIN
                (* *)
                a40move_i4 (acv, a41v, kb560GetLogDeviceSize, NOT c_null, 0, NOT c_to_undef);
                a40move_i4 (acv, a41v, ldiPagesUsed_kb00,     NOT c_null, 0, NOT c_to_undef);
                log_percent := ak42evalpercent (ldiPagesUsed_kb00, kb560GetLogDeviceSize);
                (* *)
                a40move_i4 (acv, a41v, log_percent,                  NOT c_null, 0, NOT c_to_undef);
                a40move_i4 (acv, a41v, ldiPagesNotSaved_kb00,        NOT c_null, 0, NOT c_to_undef);
                log_percent := ak42evalpercent(ldiPagesNotSaved_kb00, kb560GetLogDeviceSize);
                (* *)
                a40move_i4 (acv, a41v, log_percent,                  NOT c_null, 0, NOT c_to_undef);
                a40move_i4 (acv, a41v, ldiPagesSinceBup_kb00,        NOT c_null, 0, NOT c_to_undef);
                a40move_i4 (acv, a41v, g01autosave_distance,         NOT c_null, 0, NOT c_to_undef);
                a40move_i4 (acv, a41v, ldiSaveptCount_kb00,          NOT c_null, 0, NOT c_to_undef);
                a40move_i4 (acv, a41v, 0(*ldiCheckptCount_kb00*),    NOT c_null, 0, NOT c_to_undef)
                END;
            (*ENDWITH*)
            (*------------ restart date/time ==> shinfo ------------*)
            a36restart_time  (restart_date, restart_time);
            a40put_date_time (acv, a41v, restart_date, c_is_date,     NOT c_is_null);
            a40put_date_time (acv, a41v, restart_time, NOT c_is_date, NOT c_is_null);
            IF  g01is_livecache
            THEN
                BEGIN
                (* k56iobjlog_info removed: PTS 1105395 JA 2000-03-07 *)
                (* former obj_log_cache_info:                         *)
                a40move_i4 (acv, a41v, 0 (* PagesUsed    *), NOT c_null, 0, NOT c_to_undef);
                a40move_i4 (acv, a41v, 0 (* PagesMinFree *), NOT c_null, 0, NOT c_to_undef)
                END
            ELSE
                BEGIN
                a40move (acv, a41v, NIL, 0);
                a40move (acv, a41v, NIL, 0)
                END
            (*ENDIF*)
            END;
        (*ENDIF*)
        END;
    (*ENDIF*)
(*ENDWITH*)
;cccprint('<ak42serverdb_st');
END;

(*------------------------------*)

PROCEDURE
        ak42state_statistics ;



VAR
      is_all         : boolean;
      found          : boolean;
      realy_bad      : boolean;
      basic_code     : tsp00_CodeType;
      messcode       : tsp00_CodeType;
      dev_type       : tsp2_dev_type;
      i              : integer;
      requested      : integer;
      devno          : tsp00_Int4;
      nam            : tsp00_Sname;
      off_on         : ARRAY[boolean] OF tsp00_Sname;
      v30            : tsp00_C30;
      (* h.b. PTS 1105522 *)

      analyze_filter : RECORD
            CASE boolean OF
                true :
                    (devn : tsp00_VFilename);
                false :
                    (filter : tsp00_C64);
                END;
            (*ENDCASE*)


      termsetinfo  : RECORD
            CASE boolean OF
                true :
                    (devn : tsp00_VFilename);
                false :
                    (ts_name : tsp00_KnlIdentifier;
                    ts_code : tsp00_C8);
                END;
            (*ENDCASE*)

      devname      : tsp2_devname;
      keyword      : tak_keyword;
      kindOut      : tak_keyword;
      topicKeyOut  : tsp00_Sname;
      levelOut     : integer;

BEGIN
cccprint('AK42  ak42state_');
WITH acv, a41v DO
    BEGIN
    is_all    := (a_current_user_kind in [usysdba, ucontroluser]);
    a4sh_kind := sh_st_state;
    a40get_catalog_table (acv, a41v);
    IF  (a_ex_kind <> only_parsing) AND
        (a_return_segm^.sp1r_returncode = 0)
    THEN
        BEGIN
        off_on[false] := n_off;
        off_on[true ] := n_on;
        (* PTS 1115978 E.Z. *)
        (* PTS 1112386*)
        v30 := n_st021_monitoring;
        ak42sname_statistic (acv, a41v, v30, off_on[c_monitor_on]);
        (* PTS 1113190 *)
        v30 := n_st18_alog_write;
        ak42sname_statistic (acv, a41v, v30, off_on[kb560IsLogDeviceEnabled]);
        IF  a01diag_monitor_on
        THEN
            BEGIN
            IF  a01sm_reads < MAX_INT4_SP00
            THEN
                ak42datab_statistic (acv, a41v, n_st031_mon_read, a01sm_reads);
            (*ENDIF*)
            IF  a01sm_selectivity <> cak_is_undefined
            THEN
                ak42datab_statistic (acv, a41v, n_st032_mon_sel, a01sm_selectivity);
            (*ENDIF*)
            IF  a01sm_milliseconds < MAX_INT4_SP00
            THEN
                ak42datab_statistic (acv, a41v, n_st033_mon_sec, a01sm_milliseconds);
            (*ENDIF*)
            ak42datab_statistic (acv, a41v, n_st035_mon_row, a545sm_get_rowno);
            v30 := n_st034_mon_data;
            ak42sname_statistic (acv, a41v, v30, off_on[a01sm_collect_data])
            END;
        (*ENDIF*)
        ;
        IF  a01diag_analyze_on
        THEN
            BEGIN
            v30 := n_st022_da;
            IF  a01diag_ana_coll_data
            THEN
                nam := n_counting
            ELSE
                nam := n_on;
            (*ENDIF*)
            ak42sname_statistic (acv, a41v, v30, nam);
            (* h.b. PTS 1105522 *)
            v30 := n_st023_da_filter;
            analyze_filter.devn := b01blankfilename;
            a544get_analyze_filter (analyze_filter.filter);
            ak42hostname_statistic (acv, a41v, v30, analyze_filter.devn)
            END;
        (*ENDIF*)
        ;
        v30 := n_st036_sap_db;
        IF  g01glob.db_is_for_sapr3
        THEN
            nam := n_yes
        ELSE
            nam := n_no;
        (*ENDIF*)
        ak42sname_statistic (acv, a41v, v30, nam);
        v30 := n_st12_db_full;
        IF  b01db_full( acv.a_transinf.tri_trans.trTaskId_gg00)
        THEN
            nam := n_yes
        ELSE
            nam := n_no;
        (*ENDIF*)
        ak42sname_statistic (acv, a41v, v30, nam);
        v30 := n_st13_connect_poss;
        IF  b01connect_allowed
        THEN
            nam := n_yes
        ELSE
            nam := n_no;
        (*ENDIF*)
        ak42sname_statistic (acv, a41v, v30, nam);
        IF  g01is_livecache
        THEN
            BEGIN
            (* PTS 1107118 E.Z. *)
            v30 := n_st132_shutd_ok;
            (* PTS 1113134 E.Z. *)
            ak42snullname_statistic (acv, a41v, v30);
            (* END PTS 1107118 E.Z. *)
            v30 := m_st133_monitor_oms;
            IF  ak341IsMonitorOn
            THEN
                nam := n_yes
            ELSE
                nam := n_no;
            (*ENDIF*)
            ak42sname_statistic (acv, a41v, v30, nam);
            END;
        (*ENDIF*)
        IF  is_all
        THEN
            ak42datab_statistic (acv, a41v, n_st04_used, a10cache_used);
        (*ENDIF*)
        v30 := n_st05_checkpt;
        IF  k51is_closed_locklist (a_transinf.tri_trans.trTaskId_gg00)
        THEN
            nam := n_yes
        ELSE
            nam := n_no;
        (*ENDIF*)
        ak42sname_statistic (acv, a41v, v30, nam);
        IF  NOT g01stand_alone
        THEN
            BEGIN
            nam := n_disconnected;
            v30 := n_st06_distrib;
            ak42sname_statistic (acv, a41v, v30, nam)
            END;
        (*ENDIF*)
        ak42datab_statistic (acv, a41v, n_st061_bad_index,
              b01get_bad_index_cnt (a_transinf.tri_trans.trTaskId_gg00));
        v30 := n_st07_vtrace;
        IF  g01vtrace.vtrAk_gg00
        THEN
            BEGIN
            nam := n_ak;
            ak42sname_statistic (acv, a41v, v30, nam)
            END;
        (*ENDIF*)
        IF  g01vtrace.vtrAll_gg00
        THEN
            BEGIN
            nam := n_default;
            ak42sname_statistic (acv, a41v, v30, nam)
            END;
        (*ENDIF*)
        IF  g01vtrace.vtrAkDelete_gg00
        THEN
            BEGIN
            nam := n_delete;
            ak42sname_statistic (acv, a41v, v30, nam)
            END;
        (*ENDIF*)
        IF  g01vtrace.vtrBdIndex_gg00
        THEN
            BEGIN
            nam := n_index;
            ak42sname_statistic (acv, a41v, v30, nam)
            END;
        (*ENDIF*)
        IF  g01vtrace.vtrAkInsert_gg00
        THEN
            BEGIN
            nam := n_insert;
            ak42sname_statistic (acv, a41v, v30, nam)
            END;
        (*ENDIF*)
        IF  g01vtrace.vtrKbLock_gg00
        THEN
            BEGIN
            nam := n_lock;
            ak42sname_statistic (acv, a41v, v30, nam)
            END;
        (*ENDIF*)
        IF  g01vtrace.vtrBdString_gg00
        THEN
            BEGIN
            nam := n_long;
            ak42sname_statistic (acv, a41v, v30, nam)
            END;
        (*ENDIF*)
        IF  g01vtrace.vtrStrategy_gg00
        THEN
            BEGIN
            nam := n_optimize;
            ak42sname_statistic (acv, a41v, v30, nam)
            END;
        (*ENDIF*)
        IF  g01vtrace.vtrAkPacket_gg00
        THEN
            BEGIN
            nam := n_order;
            ak42sname_statistic (acv, a41v, v30, nam)
            END;
        (*ENDIF*)
        IF  g01vtrace.vtrAkShortPacket_gg00
        THEN
            BEGIN
            nam := n_order_stndrd;
            ak42sname_statistic (acv, a41v, v30, nam)
            END;
        (*ENDIF*)
        IF  g01vtrace.vtrIoTrace_gg00
        THEN
            BEGIN
            nam := n_pages;
            ak42sname_statistic (acv, a41v, v30, nam)
            END;
        (*ENDIF*)
        IF  g01vtrace.vtrAkSelect_gg00
        THEN
            BEGIN
            nam := n_select;
            ak42sname_statistic (acv, a41v, v30, nam)
            END;
        (*ENDIF*)
        IF  g01vtrace.vtrBdPrim_gg00
        THEN
            BEGIN
            nam := n_table;
            ak42sname_statistic (acv, a41v, v30, nam)
            END;
        (*ENDIF*)
        IF  g01vtrace.vtrTime_gg00
        THEN
            BEGIN
            nam := n_time;
            ak42sname_statistic (acv, a41v, v30, nam)
            END;
        (*ENDIF*)
        IF  g01vtrace.vtrAkUpdate_gg00
        THEN
            BEGIN
            nam := n_update;
            ak42sname_statistic (acv, a41v, v30, nam)
            END;
        (*ENDIF*)
        IF  g01vtrace.vtrBdObject_gg00
        THEN
            BEGIN
            nam := n_obj_bd;
            ak42sname_statistic (acv, a41v, v30, nam)
            END;
        (*ENDIF*)
        IF  g01vtrace.vtrOmsNew_gg00
        THEN
            BEGIN
            nam := n_obj_new;
            ak42sname_statistic (acv, a41v, v30, nam)
            END;
        (*ENDIF*)
        IF  g01vtrace.vtrOmsGet_gg00
        THEN
            BEGIN
            nam := n_obj_get;
            ak42sname_statistic (acv, a41v, v30, nam)
            END;
        (*ENDIF*)
        IF  g01vtrace.vtrOmsUpd_gg00
        THEN
            BEGIN
            nam := n_obj_upd;
            ak42sname_statistic (acv, a41v, v30, nam)
            END;
        (*ENDIF*)
        IF  g01vtrace.vtrOmsFree_gg00
        THEN
            BEGIN
            nam := n_obj_free;
            ak42sname_statistic (acv, a41v, v30, nam)
            END;
        (* PTS 1110976 E.Z. *)
        (*ENDIF*)
        requested := 0;
        WHILE Kernel_DiagInfop (requested, kindOut, topicKeyOut, levelOut) DO
            BEGIN
            IF  levelOut > 0
            THEN
                a42name_and_val_statistic (acv, a41v, kindOut, levelOut,
                      topicKeyOut);
            (*ENDIF*)
            requested := requested + 1;
            END;
        (*ENDWHILE*)
        (* END PTS 1110976 E.Z. *)
        IF  g01vtrace.vtrCheck_gg00
        THEN
            BEGIN
            v30 := n_st08_vtrace_chck;
            nam := n_on;
            ak42sname_statistic (acv, a41v, v30, nam)
            END;
        (*ENDIF*)
        v30 := n_st08a_check;  (* PTS 1103957 JA 1999-09-15 *)
        IF  kb03IsBackupCheck
        THEN
            (* PTS 1103957 JA 1999-09-15 *)
            BEGIN
            nam := n_backup;
            ak42sname_statistic (acv, a41v, v30, nam)
            END;
        (*ENDIF*)
        IF  kb03IsLockCheck
        THEN
            (* PTS 1103957 JA 1999-09-15 *)
            BEGIN
            nam := n_lock;
            ak42sname_statistic (acv, a41v, v30, nam)
            END;
        (*ENDIF*)
        IF  kb03IsLockSupplyCheck
        THEN
            (* PTS 1103957 JA 1999-09-15 *)
            BEGIN
            nam := n_lock_supply;
            ak42sname_statistic (acv, a41v, v30, nam)
            END;
        (*ENDIF*)
        IF  kb03IsLogCheck
        THEN
            (* PTS 1103957 JA 1999-09-15 *)
            BEGIN
            nam := n_log;
            ak42sname_statistic (acv, a41v, v30, nam)
            END;
        (*ENDIF*)
        IF  kb03IsLogQueueCheck
        THEN
            (* PTS 1103957 JA 1999-09-15 *)
            BEGIN
            nam := n_log_queue;
            ak42sname_statistic (acv, a41v, v30, nam)
            END;
        (*ENDIF*)
        IF  kb03IsRegionCheck
        THEN
            (* PTS 1103957 JA 1999-09-15 *)
            BEGIN
            nam := n_kb_regions;
            ak42sname_statistic (acv, a41v, v30, nam)
            END;
        (*ENDIF*)
        IF  kb03IsTransCheck
        THEN
            (* PTS 1103957 JA 1999-09-15 *)
            BEGIN
            nam := n_translist;
            ak42sname_statistic (acv, a41v, v30, nam)
            END;
        (*ENDIF*)
        IF  g01glob.datacachecheck
        THEN
            (* PTS 1103957 JA 1999-09-15 *)
            BEGIN
            nam := n_datacache;
            ak42sname_statistic (acv, a41v, v30, nam)
            END;
        (*ENDIF*)
        IF  g01glob.bd_lock_check
        THEN
            (* PTS 1103957 JA 1999-09-15 *)
            BEGIN
            nam := n_tree_locks;
            ak42sname_statistic (acv, a41v, v30, nam)
            END;
        (*ENDIF*)
        IF  g01glob.treecheck
        THEN
            (* PTS 1103957 JA 1999-09-15 *)
            BEGIN
            nam := n_tree;
            ak42sname_statistic (acv, a41v, v30, nam)
            END;
        (* PTS 1107617 E.Z. *)
        (*ENDIF*)
        ;
        (* vtrSite removed: PTS 1103799 JA 1999-08-30 *)
        IF  g01vtrace.vtrSession_gg00.ci4_gg00 <> cgg_nil_session
        THEN
            BEGIN
            v30 := n_st11_vtrace_sess;
            ak42session_to_sname (g01vtrace.vtrSession_gg00, sizeof(nam), nam);
            ak42sname_statistic (acv, a41v, v30, nam)
            END;
        (* PTS 1110976 E.Z. *)
        (*ENDIF*)
        IF  g01vtrace.vtrRetcodeCheck_gg00
        THEN
            BEGIN
            keyword := n_st11a_stop_on_err;
            nam := n_on;
            a42name_and_val_statistic (acv, a41v, keyword,
                  g01vtrace.vtrStopRetcode_gg00, nam);
            END;
        (* END PTS 1110976 E.Z. *)
        (*ENDIF*)
        a37event_state (acv, a41v);
        dev_type := sp2dt_log;
        devno    := 1;
        WHILE dev_type <> sp2dt_trace DO
            BEGIN
            b01next_bad_dev (a_transinf.tri_trans.trTaskId_gg00, dev_type,
                  devno, devname, realy_bad);
            IF  devname <> b01blankfilename
            THEN
                BEGIN
                IF  realy_bad
                THEN
                    v30 := n_bad_device
                ELSE
                    v30 := n_reintegration;
                (*ENDIF*)
                ak42hostname_statistic (acv, a41v, v30, devname)
                END;
            (*ENDIF*)
            END;
        (*ENDWHILE*)
        v30 := n_termchar_set;
        FOR i := cgg_begin_termsets TO cgg_end_termsets DO
            BEGIN
            termsetinfo.devn := b01blankfilename;
            messcode         := i;
            g02gettermsetdesc (termsetinfo.ts_name, messcode,
                  basic_code, found);
            IF  found
            THEN
                BEGIN
                IF  basic_code = csp_ascii
                THEN
                    termsetinfo.ts_code := ' ASCII  '
                ELSE
                    IF  basic_code = csp_ebcdic
                    THEN
                        termsetinfo.ts_code := ' EBCDIC '
                    ELSE
                        termsetinfo.ts_code := ' error  ';
                    (*ENDIF*)
                (*ENDIF*)
                ak42hostname_statistic (acv,
                      a41v, v30, termsetinfo.devn)
                END;
            (*ENDIF*)
            END;
        (*ENDFOR*)
        END;
    (*ENDIF*)
    END;
(*ENDWITH*)
;cccprint('<ak42state_stati');
END;

(*------------------------------*)

PROCEDURE
        ak42ref_i4_statistic ;



BEGIN
cccprint('AK42  ak42ref_i4');
ak42ref_i4_stat_undef (acv, a41v, textpar, lint, false);
;cccprint('<ak42ref_i4_stat');
END;

(*------------------------------*)

PROCEDURE
        ak42ref_i4_stat_undef ;



VAR
      col_cnt : integer;

BEGIN
cccprint('AK42  ak42ref_i4');
WITH acv, a41v  DO
    IF  a_return_segm^.sp1r_returncode = 0
    THEN
        BEGIN
        ak42prefix    (acv, a41v, col_cnt);
        a40move_const (acv, a41v, @textpar, sizeof (textpar));
        a40move_i4    (acv, a41v, lint, NOT c_null, 0, zero_undef);
        col_cnt := col_cnt + 2;
        WHILE col_cnt < a4p_arr.pinfop^.sbase.bmaxcol - 1 DO
            BEGIN
            col_cnt := col_cnt + 1;
            a40move (acv, a41v, NIL, 0)
            END;
        (*ENDWHILE*)
        END;
    (*ENDIF*)
(*ENDWITH*)
;cccprint('<ak42ref_i4_stat');
END;

(*------------------------------*)

PROCEDURE
        ak42ref_stat_real ;



VAR
      col_cnt : integer;

BEGIN
cccprint('AK42  ak42ref_st');
WITH acv, a41v  DO
    IF  a_return_segm^.sp1r_returncode = 0
    THEN
        BEGIN
        ak42prefix    (acv, a41v, col_cnt);
        a40move_const (acv, a41v, @textpar, sizeof (textpar));
        a40move_real  (acv, a41v, lreal, NOT c_null, 0);
        col_cnt := col_cnt + 2;
        WHILE col_cnt < a4p_arr.pinfop^.sbase.bmaxcol - 1 DO
            BEGIN
            col_cnt := col_cnt + 1;
            a40move (acv, a41v, NIL, 0)
            END;
        (*ENDWHILE*)
        END;
    (*ENDIF*)
(*ENDWITH*)
;cccprint('<ak42ref_stat_re');
END;

(*------------------------------*)

PROCEDURE
        ak42prefix ;



BEGIN
cccprint('AK42  ak42prefix');
WITH a41v DO
    BEGIN
    col_cnt := 0;
    CASE a4sh_kind OF
        sh_st_index, sh_odbc_st_index, sh_indexpages :
            BEGIN
            a40table_column_to_shbuf (acv, a41v, a4p_arr.pbasep, -1);
            IF  a4col_ptr = NIL
            THEN
                a40move (acv, a41v, @a4coln, sizeof (a4coln))
            ELSE
                a40move (acv, a41v, NIL, 0);
            (*ENDIF*)
            IF  a4col_ptr <> NIL
            THEN
                a40move (acv, a41v, @a4col_ptr^.ccolumnn,
                      ord (a4col_ptr^.ccolumnn_len))
            ELSE
                a40move (acv, a41v, NIL, 0);
            (*ENDIF*)
            col_cnt := 4
            END;
        sh_st_monitor :
            ak42monitor_type (acv, a41v, col_cnt);
        sh_st_table, sh_odbc_st_table,
        sh_pages :
            BEGIN
            a40table_column_to_shbuf (acv, a41v, a4p_arr.pbasep, -1);
            col_cnt := 2
            END;
        OTHERWISE ;
        END;
    (*ENDCASE*)
    END
(*ENDWITH*)
;cccprint('<ak42prefix     ');
END;

(*------------------------------*)

PROCEDURE
        ak42monitor_type ;



VAR
      c8 : tsp00_C8;

BEGIN
cccprint('AK42  ak42monito');
CASE a41v.a4ti OF
    cak_i_caches :
        c8 := 'CACHES  ';
    cak_i_load :
        c8 := 'LOAD    ';
    cak_i_lock :
        c8 := 'LOCK    ';
    cak_i_log :
        c8 := 'LOG     ';
    cak_i_long :
        c8 := 'LONG    ';
    cak_i_pages :
        c8 := 'PAGES   ';
    cak_i_row :
        c8 := 'ROW     ';
    cak_i_serverdb :
        c8 := 'SERVERDB';
    cak_i_transaction :
        c8 := 'TRANS   ';
    END;
(*ENDCASE*)
a40move_const (acv, a41v, @c8, sizeof (c8));
col_cnt := 1
;cccprint('<ak42monitor_typ');
END;

(*------------------------------*)

PROCEDURE
        ak42devspace_statistics ;



VAR
      dev_no          : tsp_int4;
      dev_used_blocks : tsp00_Int4;
      free_pages      : tsp00_Int4;
      devType         : tsp_c4;
      dev_type        : tsp2_dev_type;
      dev_name        : tsp2_devname;
      dev_size        : tsp_int4;
      dev_online      : boolean;
      dev_state       : tsp_c18;

BEGIN
cccprint('AK42  ak42devspa');
WITH acv, a41v DO
    BEGIN
    a4sh_kind := sh_st_devspace;
    a40get_catalog_table (acv, a41v);
    IF  acv.a_return_segm^.sp1r_returncode = 0
    THEN
        BEGIN
        dev_no    := -1;
        REPEAT
            b01next_dev_info (acv.a_transinf.tri_trans.trTaskId_gg00,
                  dev_no, dev_type, dev_name, dev_size, dev_used_blocks,
                  dev_online, dev_state);
            IF  (dev_no >= 0) AND (dev_type = sp2dt_data)
            THEN
                BEGIN
                a40move_const (acv, a41v, @dev_name, sizeof(dev_name));
                a40move_i4 (acv, a41v, dev_size, NOT c_null, 0, NOT c_to_undef);
                a40move_i4 (acv, a41v, dev_used_blocks, NOT c_null, 0, NOT c_to_undef);
                a40move_i4 (acv, a41v,
                      ak42evalpercent (dev_used_blocks, dev_size),
                      NOT c_null, 0, NOT c_to_undef);
                free_pages := dev_size - dev_used_blocks;
                a40move_i4 (acv, a41v, free_pages, NOT c_null, 0, NOT c_to_undef);
                a40move_i4 (acv, a41v,
                      ak42evalpercent (free_pages, dev_size),
                      NOT c_null, 0, NOT c_to_undef)
                END;
            (*ENDIF*)
        UNTIL
            dev_no = -1;
        (*ENDREPEAT*)
        END;
    (*ENDIF*)
    END;
(*ENDWITH*)
;cccprint('<ak42devspace_st');
END;

(*------------------------------*)

FUNCTION
        ak42evalpercent ;



BEGIN
cccprint('AK42  ak42evalpe');
IF  (cnt > 0) AND (max > 0)
THEN
    BEGIN
    IF  max >= (MAX_INT4_SP00 DIV 1000)
    THEN
        BEGIN
        max := max DIV 1000;
        cnt := cnt DIV 1000
        END;
    (*ENDIF*)
    cnt             := cnt * 1000;
    ak42evalpercent := ((cnt DIV max) + 5) DIV 10
    END
ELSE
    ak42evalpercent := 0
(*ENDIF*)
;cccprint('<ak42evalpercent');
END;

(*------------------------------*)

FUNCTION
        ak42evalpercent_cnt ;



BEGIN
cccprint('AK42  ak42evalpe');
(* PTS 1103743 JA 1999-08-25 *)
IF  (cnt > 0) AND (max > 0)
THEN
    BEGIN
    ak42evalpercent_cnt := round (cnt * 100 / max);
    END
ELSE
    ak42evalpercent_cnt := 0
(*ENDIF*)
;cccprint('<ak42evalpercent');
END;

(*------------------------------*)

FUNCTION
        ak42evalpercent_real ;



BEGIN
cccprint('AK42  ak42evalpe');
IF  (cnt > 0) AND (max > 0)
THEN
    BEGIN
    ak42evalpercent_real := round (cnt * 100 / max);
    END
ELSE
    ak42evalpercent_real := 0
(*ENDIF*)
;cccprint('<ak42evalpercent');
END;

(*------------------------------*)

PROCEDURE
        ak42datab_statistic ;



BEGIN
cccprint('AK42  ak42datab_');
ak42datab_stat_undef (acv, a41v, v30, lint, false);
;cccprint('<ak42datab_stati');
END;

(*------------------------------*)

PROCEDURE
        ak42datab_stat_undef ;



VAR
      col_cnt  : integer;
      frac     : integer;

BEGIN
cccprint('AK42  ak42datab_');
WITH acv, a41v  DO
    IF  a_return_segm^.sp1r_returncode = 0
    THEN
        BEGIN
        ak42prefix (acv, a41v, col_cnt);
        a40move_const (acv, a41v, @v30, sizeof (v30));
        a40move_i4 (acv, a41v, lint,
              NOT c_null, 0, zero_undef);
        a40move    (acv, a41v, NIL, 0)
        END;
    (*ENDIF*)
(*ENDWITH*)
;cccprint('<ak42datab_stat_');
END;

(*------------------------------*)

PROCEDURE
        ak42log_statistics ;



VAR
      log_percent    : tsp00_Int4;
      nam            : tsp00_Sname;
      v30            : tsp00_C30;
      log_device_info: tkb00_LogDeviceInfo;
      log_queue_info : tkb00_LogQueueInfo;

BEGIN
cccprint('AK42  ak42log_st');
WITH acv, a41v DO
    BEGIN
    a4sh_kind := sh_st_log;
    a40get_catalog_table (acv, a41v);
    IF  (a_ex_kind <> only_parsing) AND
        (a_return_segm^.sp1r_returncode = 0 )
    THEN
        BEGIN
        kb560GetStatistics (a_transinf.tri_trans.trTaskId_gg00, log_device_info, log_queue_info);
        nam := n_single_log;
        v30 := n_logmode;
        ak42sname_statistic (acv, a41v, v30, nam);
        WITH log_device_info DO
            BEGIN
            ak42datab_statistic (acv, a41v, n_log02_pages,      kb560GetLogDeviceSize);
            ak42datab_statistic (acv, a41v, n_log03_used_pages, ldiPagesUsed_kb00);
            log_percent := ak42evalpercent (ldiPagesUsed_kb00, kb560GetLogDeviceSize);
            (* *)
            ak42datab_statistic (acv, a41v, n_log04_used_pcent, log_percent);
            ak42datab_statistic (acv, a41v, n_log05_notsaved,   ldiPagesNotSaved_kb00);
            log_percent := ak42evalpercent (ldiPagesNotSaved_kb00, kb560GetLogDeviceSize);
            (* *)
            ak42datab_statistic (acv, a41v, n_log06_notsav_pct, log_percent);
            ak42datab_statistic (acv, a41v, n_log07_since_bup,  ldiPagesSinceBup_kb00);
            ak42datab_statistic (acv, a41v, n_segmsize, g01autosave_distance);
            ak42datab_statistic (acv, a41v, n_log11_cnt_savept, ldiSaveptCount_kb00);
            ak42datab_statistic (acv, a41v, n_log12_cnt_checkp, 0(*ldiCheckptCount_kb00*))
            END
        (*ENDWITH*)
        END
    (*ENDIF*)
    END
(*ENDWITH*)
;cccprint('<ak42log_statist');
END;

(*------------------------------*)

PROCEDURE
        ak42user_statistics ;



VAR
      ti : integer;

BEGIN
cccprint('AK42  ak42user_s');
WITH acv, a41v DO
    BEGIN
    a4sh_kind := sh_st_user;
    a40get_catalog_table (acv, a41v);
    IF  a_return_segm^.sp1r_returncode = 0
    THEN
        BEGIN
        IF  a4authname = a01_il_b_identifier
        THEN
            IF  (a_current_user_kind = usysdba) OR
                (a_current_user_kind = udba)    OR
                (a_current_user_kind = ucontroluser)
            THEN
                ak42all_user_st_show (acv, a41v)
            ELSE
                BEGIN
                a4authname := a_curr_user_name;
                ak42one_st_user_show (acv, a41v, true)
                END
            (*ENDIF*)
        ELSE
            BEGIN
            IF  a4authname = a01_i_current
            THEN
                a4authname := a_curr_user_name;
            (*ENDIF*)
            ak42one_st_user_show (acv, a41v, true);
            END;
        (*ENDIF*)
        END;
    (*ENDIF*)
    END;
(*ENDWITH*)
;cccprint('<ak42user_statis');
END;

(*------------------------------*)

PROCEDURE
        ak42all_user_st_show ;



VAR
      b_err : tgg00_BasisError;
      sysk  : tgg00_SysInfoKey;

BEGIN
cccprint('AK42  ak42all_us');
WITH acv, a41v   DO
    IF  (a_return_segm^.sp1r_returncode = 0) AND
        (a_ex_kind <> only_parsing)
    THEN
        BEGIN
        sysk           := a01defaultkey;
        sysk.sauthid   := cgg_zero_id;
        sysk.sentrytyp := cak_euserref;
        REPEAT (* scan all user catalog records *)
            a10next_sysinfo (acv, sysk, 0, d_release,
                  cak_euserref, a4sysbuf, b_err);
            IF  b_err = e_ok
            THEN
                BEGIN
                a40username_from_syskey (sysk, a4authname);
                ak42one_st_user_show (acv, a41v, false);
                a40ignore_down_error (acv)
                END;
            (*ENDIF*)
        UNTIL
            (b_err <> e_ok) OR (a_return_segm^.sp1r_returncode <> 0);
        (*ENDREPEAT*)
        IF  (b_err <> e_no_next_record) AND (b_err <> e_ok)
        THEN
            a07_b_put_error ( acv, b_err, 1);
        (*ENDIF*)
        END;
    (*ENDIF*)
(*ENDWITH*)
;cccprint('<ak42all_user_st');
END;

(*------------------------------*)

PROCEDURE
        ak42one_st_user_show ;



VAR
      ok             : boolean;
      counting       : boolean;
      is_dba         : boolean;
      b_err          : tgg00_BasisError;
      priv_count     : tsp00_Int4;
      init_username  : tsp00_KnlIdentifier;
      init_userid    : tgg00_Surrogate;
      ukind          : tsp00_C8;
      userkey        : tgg00_SysInfoKey;

BEGIN
cccprint('AK42  ak42one_st');
WITH acv, a41v DO
    BEGIN
    a06check_username (acv, a4authname, is_dba, ok);
    counting   := (a_ex_kind <> only_parsing);
    priv_count := 0;
    IF  NOT (ok)
    THEN
        a07_b_put_error (acv, e_unknown_user, a_ap_tree^[ a4ti ].n_pos)
    ELSE
        BEGIN (* privilege check *)
        userkey := a01defaultkey;
        a51build_userkey (a4authname, a4sysk);
        a10get_sysinfo (acv, a4sysk, d_release, a4sysbuf, b_err);
        IF  b_err = e_ok
        THEN
            userkey.sauthid := a4sysbuf^.suserref.ru_user_id
        ELSE
            a07_b_put_error (acv, e_unknown_user, 1)
        (*ENDIF*)
        END;
    (*ENDIF*)
    IF  a_return_segm^.sp1r_returncode = 0
    THEN
        BEGIN
        userkey.sentrytyp := cak_euser;
        a10get_sysinfo (acv, userkey, d_fix, a4sysbuf, b_err);
        END;
    (*ENDIF*)
    IF  a_return_segm^.sp1r_returncode = 0
    THEN
        WITH a4sysbuf^.suser DO
            IF  NOT (one_show)
            THEN
                BEGIN
                IF  a4authname <> a_curr_user_name
                THEN
                    IF  NOT a21is_owner (acv, a4sysbuf^.suser)
                    THEN
                        counting := false;
                    (*ENDIF*)
                (*ENDIF*)
                END
            ELSE
                IF  NOT (is_group_rec in urecordtyp)
                THEN
                    a07_b_put_error (acv,
                          e_group_user_not_allowed,
                          a_ap_tree^[ a4ti ].n_pos)
                ELSE
                    IF  (a4authname        <> a_curr_user_name) AND
                        (userkey.sauthid <> usergroup_id)
                    THEN
                        IF  NOT a21is_owner (acv, a4sysbuf^.suser)
                        THEN
                            a07_b_put_error (acv, e_unknown_user,
                                  a_ap_tree^[ a4ti ].n_pos);
                        (*ENDIF*)
                    (*ENDIF*)
                (*ENDIF*)
            (*ENDIF*)
        (*ENDWITH*)
    (*ENDIF*)
    IF  (a_return_segm^.sp1r_returncode = 0) AND (counting)
    THEN
        BEGIN (* determine permanent user space *)
        init_username       := a_curr_user_name;
        init_userid         := a_curr_user_id;
        a_curr_user_name    := a4authname;
        a_curr_user_id      := a4sysbuf^.syskey.sauthid;
        a40init_table_scan (acv, a41v,
              false, true, false, false, false, false);
        WHILE a40next_table (acv, a41v) DO
            WITH a4p_arr.pbasep^.sbase DO
                IF  ((btablekind = twithkey)
                    OR
                    (btablekind = twithoutkey))
                    AND
                    (btreeid.fileTfn_gg00 <> tfnTemp_egg00)
                THEN
                    a42_get_pagecount (acv,
                          a4p_arr.pbasep, priv_count);
                (*ENDIF*)
            (*ENDWITH*)
        (*ENDWHILE*)
        a_curr_user_name    := init_username;
        a_curr_user_id      := init_userid;
        END;
    (*ENDIF*)
    IF  (a_return_segm^.sp1r_returncode = 0)  AND  (counting)
    THEN
        BEGIN
        (*============================================*)
        (* return results, send them to bd            *)
        (*============================================*)
        WITH a4sysbuf^.suser DO
            BEGIN
            a40move (acv, a41v, @a4authname, sizeof (a4authname));
            CASE userkind OF
                udba      :
                    ukind := u_dba;
                uprivate  :
                    ukind := u_resource;
                OTHERWISE
                    ukind := u_standard;
                END;
            (*ENDCASE*)
            a40move_const (acv, a41v, @ukind, sizeof (ukind));
            a40move_i4 (acv, a41v, priv_count,
                  (userkind = unoprivate), 0, NOT c_to_undef);
            IF  a4authname <> a_curr_user_name
            THEN
                a40move (acv, a41v, NIL, 0)
            ELSE
                a40move_i4 (acv, a41v,
                      a_transinf.tri_trans.trTempCount_gg00,
                      NOT c_null, 0, NOT c_to_undef);
            (*ENDIF*)
            END;
        (*ENDWITH*)
        END;
    (*ENDIF*)
    IF  a_return_segm^.sp1r_returncode = 0
    THEN
        a10_key_del (acv, a4sysbuf^.syskey);
    (*ENDIF*)
    END;
(*ENDWITH*)
;cccprint('<ak42one_st_user');
END;

(*------------------------------*)

PROCEDURE
        ak42show_config ;



CONST
      c_with_zero = true;

VAR
      kb_factor       : integer;
      i               : tsp00_Int4;
      no_of_data_devs : tsp00_Int4;
      no_of_log_devs  : tsp00_Int4;
      totalPages      : tsp00_Int4;
      unMapPages      : tsp00_Int4;
      nam             : tsp00_Sname;
      v30             : tsp00_C30;
      devname         : tsp2_devname;

BEGIN
cccprint('AK42  ak42show_c');
bd01DataCacheSize( totalPages, unMapPages );
WITH acv, a41v DO
    BEGIN
    a4sh_kind := sh_st_config;
    a40get_catalog_table (acv, a41v);
    IF  (a_ex_kind <> only_parsing) AND
        (a_return_segm^.sp1r_returncode = 0)
    THEN
        BEGIN
        v30 := n_cf01_defaultcode;
        CASE g01code.char_default OF
            csp_ascii:
                nam := n_ascii;
            csp_ebcdic:
                nam := n_ebcdic;
            csp_codeneutral:
                nam := n_internal;
            csp_unicode:
                nam := n_unicode;
            OTHERWISE
                nam := bsp_sname;
            END;
        (*ENDCASE*)
        ak42sname_statistic (acv, a41v, v30, nam);
        v30 := n_cf02_date_time;
        CASE g01code.date_time OF
            dtf_normal :
                nam := n_internal;
            dtf_eur, dtf_ts_eur :
                nam := n_eur;
            dtf_iso    :
                nam := n_iso;
            dtf_jis    :
                nam := n_jis;
            dtf_usa    :
                nam := n_usa;
            OTHERWISE
                nam := bsp_sname;
            END;
        (*ENDCASE*)
        ak42sname_statistic (acv, a41v, v30, nam);
        ak42datab_statistic (acv, a41v, n_cf04_sesstimeout, g01timeout.timeCmd_gg00);
        ak42datab_statistic (acv, a41v, n_cf06_req_timeout, g01timeout.timeReq_gg00);
        v30 := n_logmode;
        g01log_mode (nam);
        ak42sname_statistic (acv, a41v, v30, nam);
        ak42datab_statistic (acv, a41v, n_segmsize, g01autosave_distance);
        no_of_log_devs := b01no_of_log_devs;
        ak42datab_statistic (acv, a41v, n_cf08_no_of_logs, no_of_log_devs);
        no_of_data_devs := b01no_of_data_devs;
        ak42datab_statistic (acv, a41v, n_cf09_no_of_devs, no_of_data_devs);
        v30 := n_cf10_mirror_dev;
        nam := n_no;
        ak42sname_statistic (acv, a41v, v30, nam);
        FOR i := 1 TO no_of_log_devs DO
            BEGIN
            v30 := n_cf_logdev_size;
            g17int4to_line (i, NOT c_with_zero, 2, 12, v30);
            ak42datab_statistic (acv, a41v, v30, b01devsize (sp2dt_log, i))
            END;
        (*ENDFOR*)
        FOR i := 1 TO no_of_log_devs DO
            BEGIN
            v30 := n_cf_logdev_name;
            g17int4to_line (i, NOT c_with_zero, 2, 12, v30);
            b01get_devname (sp2dt_log, i, devname);
            ak42hostname_statistic (acv, a41v, v30, devname);
            g01log_mode (nam);
            IF  nam = 'DUAL        '
            THEN
                BEGIN
                v30 [14] := '''';
                b01get_devname (sp2dt_mirr_log, i, devname);
                ak42hostname_statistic (acv, a41v, v30, devname)
                END
            (*ENDIF*)
            END;
        (*ENDFOR*)
        FOR i := 1 TO no_of_data_devs DO
            BEGIN
            v30 := n_cf_dev_size;
            g17int4to_line (i, NOT c_with_zero, 3, 14, v30);
            ak42datab_statistic (acv, a41v, v30, b01devsize (sp2dt_data, i))
            END;
        (*ENDFOR*)
        FOR i := 1 TO no_of_data_devs DO
            BEGIN
            v30 := n_cf_dev_name;
            g17int4to_line (i, NOT c_with_zero, 3, 14, v30);
            b01get_devname (sp2dt_data, i, devname);
            ak42hostname_statistic (acv, a41v, v30, devname);
            END;
        (*ENDFOR*)
        (* PTS 1112851 *)
        kb_factor := PAGE_MXSP00 DIV 1024;
        ak42datab_statistic (acv, a41v, n_cf11_converter_sz, bd01ConverterSize * kb_factor);
        ak42datab_statistic (acv, a41v, n_cf12_datacache_sz, totalPages * kb_factor);
        IF  RTEMem_AWEAvailable (* PTS 1114026 *)
        THEN
            BEGIN
            ak42datab_statistic (acv, a41v,
                  n_cf13_awe_mapped, (totalPages - unMapPages) * kb_factor);
            ak42datab_statistic (acv, a41v,
                  n_cf14_awe_unmapped, unMapPages * kb_factor);
            END;
        (*ENDIF*)
        END
    (*ENDIF*)
    END
(*ENDWITH*)
;cccprint('<ak42show_config');
END;

(*------------------------------*)

PROCEDURE
        ak42hex ;



VAR
      i    : integer;
      dec  : integer;

BEGIN
cccprint('AK42  ak42hex   ');
dec := ord (alpha_char);
outnam [ outpos   ] := chr(dec DIV 16);
outnam [ outpos+1 ] := chr(dec MOD 16);
FOR i := outpos TO outpos + 1 DO
    BEGIN
    IF  ord(outnam[ i ]) > 9
    THEN
        outnam[ i ] := chr(ord('A')-10 + ord(outnam[ i ]))
    ELSE
        outnam[ i ] := chr (ord('0') + ord(outnam[ i ]));
    (*ENDIF*)
    END
(*ENDFOR*)
;cccprint('<ak42hex        ');
END;

(*------------------------------*)

PROCEDURE
        ak42show_version ;



VAR
      rdsvers  : tsp00_Version;
      lzuvers  : tsp00_Version;

BEGIN
cccprint('AK42  ak42show_v');
WITH acv, a41v  DO
    BEGIN
    a4sh_kind := sh_version;
    a40get_catalog_table (acv, a41v);
    IF  (a_ex_kind <> only_parsing) AND
        (a_return_segm^.sp1r_returncode = 0)
    THEN
        BEGIN
        g11kernel_version (rdsvers);
        vversion (rdsvers, lzuvers);
        a40move_const (acv, a41v, @rdsvers, sizeof (rdsvers));
        a40move_const (acv, a41v, @lzuvers, sizeof (lzuvers));
        END;
    (*ENDIF*)
    END;
(*ENDWITH*)
;cccprint('<ak42show_versio');
END;

(*------------------------------*)

PROCEDURE
        a42iindex_statistics ;



CONST
      c_single_index = true;
      c_exit_loop    = MAX_INT2_SP00;

VAR
      jx             : integer;
      colindex       : integer;
      null_value     : tsp00_KnlIdentifier;
      col_ptr        : tak00_colinfo_ptr;
      indexname      : tsp00_KnlIdentifier;
      index_scan_rec : tak_index_scan_record;
      data           : tgg00_SampleInfo;
      qual           : tgg00_QualBuf;
      mblock         : tgg00_MessBlock;

BEGIN
cccprint('AK42  a42iindex_');
g01mblock_init (acv.a_transinf.tri_trans, m_nil, mm_nil, mblock);
g01stack_init    (mblock, NIL, 0, @qual, sizeof (qual));
g01datapart_init (mblock, @data, sizeof (data));
WITH acv, a41v DO
    BEGIN
    IF  a4p_arr.pbasep^.sbase.bindexexist
    THEN
        BEGIN
        a24init_index_scan (acv, a4p_arr.pbasep^.sbase.bsurrogate,
              index_scan_rec);
        WITH index_scan_rec DO
            WHILE a24next_named_index (acv, index_scan_rec) DO
                WITH isr_buf^.smindex.indexdef[isr_index] DO
                    IF  indexno <> 0
                    THEN
                        WITH a4p_arr.pbasep^.sbase DO
                            BEGIN
                            a24get_indexname (acv, isr_buf, isr_index,
                                  indexname);
                            colindex := bextcolindex[icolseq[1]] + bfirstindex;
                            IF  (
                                a722test_col_qual (acv,
                                a41v, cak_ln_indexname, ' ', @indexname,
                                sizeof(indexname))
                                AND
                                a722test_col_qual (acv, a41v,
                                cak_ln_columnname, csp_undef_byte,
                                @null_value, 1)
                                )
                            THEN
                                BEGIN
                                (* PTS 1114312, T.A. removed code for unnamed indexes *)
                                a4col_ptr := NIL;
                                a4coln    := indexname;
                                ak42one_index_statistics (acv, a41v.a4sh_kind,
                                      a4p_arr.pbasep^.sbase,icolstack[1],
                                      mblock);
                                IF  a_return_segm^.sp1r_returncode = 0
                                THEN
                                    ak42print_stat_tab_index (acv,
                                          a41v, mblock, true);
                                (*ENDIF*)
                                END;
                            (*ENDIF*)
                            END;
                        (*ENDWITH*)
                    (*ENDIF*)
                (*ENDWITH*)
            (*ENDWHILE*)
        (*ENDWITH*)
        a24finish_index_scan (acv, index_scan_rec)
        END;
    (*ENDIF*)
    END;
(*ENDWITH*)
;cccprint('<a42iindex_stati');
END;

(*------------------------------*)

PROCEDURE
        a42table_statistics ;



CONST
      c_estimate_pages = 6;
      c_shortcol_pages = 7;

VAR
      estimate_pages : boolean;
      b_err          : tgg00_BasisError;
      page_cnt       : tsp_int4;
      val1           : tsp_int4;
      val2           : tsp_int4;
      treeid         : tgg00_FileId;
      qual           : tgg00_QualBuf;
      data           : tgg00_SampleInfo;
      mblock         : tgg00_MessBlock;

BEGIN
cccprint('AK42  a42table_s');
WITH acv, a41v DO
    BEGIN
    t01int4 (ak_sem, 'a4sh_kind   ', ord (a4sh_kind));
    IF  (a_return_segm^.sp1r_returncode = 0) AND
        (a_ex_kind <> only_parsing)
    THEN
        IF  pages_only
        THEN
            BEGIN
            page_cnt       := MAX_INT4_SP00;
            treeid         := a4p_arr.pbasep^.sbase.btreeid;
            treeid.fileLeafNodes_gg00 := cgg_nil_leafnodes;
            estimate_pages := a4strategy.str_catalog.scat_col_needed[c_estimate_pages];
            a06get_page_cnt (acv, treeid, estimate_pages, page_cnt, b_err);
            IF  b_err = e_ok
            THEN
                BEGIN
                IF  estimate_pages
                THEN
                    BEGIN
                    val1 := 0;
                    val2 := page_cnt
                    END
                ELSE
                    BEGIN
                    val1 := page_cnt;
                    val2 := 0
                    END;
                (*ENDIF*)
                ak42datab_statistic (acv, a41v, n_tab05_leafnodes, val1);
                a40move_i4 (acv, a41v, val2, NOT c_null, 0, c_to_undef);
                (* h.b. PTS 1107063 *)
                val1 := 0;
                IF  (a4p_arr.pbasep^.sbase.bstringcount > 0) AND
                    (a4strategy.str_catalog.scat_col_needed[c_shortcol_pages])
                THEN
                    BEGIN
                    treeid.fileType_gg00  := [ftsPerm_egg00, ftsConcurrent_egg00];
                    treeid.fileTfn_gg00   := tfnShortScol_egg00;
                    treeid.fileRoot_gg00  := NIL_PAGE_NO_GG00;
                    treeid.fileVersion_gg00.ci2_gg00
                          := cgg_dummy_file_version;
                    a06get_page_cnt (acv, treeid,
                          estimate_pages, page_cnt, b_err);
                    IF  b_err = e_ok
                    THEN
                        val1 := page_cnt
                    (*ENDIF*)
                    END;
                (*ENDIF*)
                a40move_i4 (acv, a41v, val1, NOT c_null, 0, c_to_undef);
                END
            ELSE
                a07_b_put_error (acv, b_err, 1)
            (*ENDIF*)
            END
        ELSE
            BEGIN
            g01mblock_init (a_transinf.tri_trans,
                  m_nil, mm_nil, mblock);
            g01stack_init    (mblock, NIL, 0, @qual, sizeof (qual));
            g01datapart_init (mblock, @data, sizeof (data));
            ak42table_statistics (acv, a4p_arr.pbasep^.sbase, mblock);
            IF  a_return_segm^.sp1r_returncode = 0
            THEN
                BEGIN
                ak42print_stat_tab_index (acv, a41v, mblock, false);
                (*== test if implicit update statistics is usefull =*)
                WITH a4p_arr.pbasep^.sbase DO
                    a28sys_upd_statistics (acv, btreeid, m_select_row, mm_next,
                          bpages, mblock.mb_data^.mbp_sample.nodes);
                (*ENDWITH*)
                END;
            (*ENDIF*)
            END;
        (*ENDIF*)
    (*ENDIF*)
    END;
(*ENDWITH*)
;cccprint('<a42table_statis');
END;

(*------------------------------*)

PROCEDURE
        ak42char2hex ;



VAR
      i        : integer;
      dec      : integer;
      hex_byte : ARRAY [ 1..2 ] OF integer;

BEGIN
cccprint('AK42  ak42char2h');
dec           := ord (input);
hex_byte[ 1 ] := dec DIV 16;
hex_byte[ 2 ] := dec MOD 16;
FOR i := 1 TO 2 DO
    IF  hex_byte[ i ] > 9
    THEN
        output [ i ] := chr(ord('A') - 10 + hex_byte[ i ])
    ELSE
        output [ i ] := chr(ord('0') + hex_byte[ i ]);
    (*ENDIF*)
(*ENDFOR*)
;cccprint('<ak42char2hex   ');
END;

(*------------------------------*)

PROCEDURE
        ak42one_index_statistics ;



CONST
      c_do_estimate = true;

VAR
      b_err    : tgg00_BasisError;
      tree     : tgg00_FileId;

BEGIN
cccprint('AK42  ak42one_in');
IF  show_kind = sh_indexpages
THEN
    BEGIN
    tree := baserec.btreeid;
    tree.fileTfn_gg00              := tfnMulti_egg00;
    tree.fileTfnNo_gg00[1]         := st_entry.ecol_tab[1];
    tree.fileRoot_gg00             := NIL_PAGE_NO_GG00;
    tree.fileVersion_gg00.ci2_gg00 := cgg_dummy_file_version;
    a06get_page_cnt (acv, tree, NOT c_do_estimate,
          mblock.mb_data^.mbp_sample.nodes, b_err);
    IF  b_err = e_file_not_accessible
    THEN
        BEGIN
        mblock.mb_data^.mbp_sample.nodes := 0;
        b_err                            := e_ok
        END;
    (*ENDIF*)
    END
ELSE
    BEGIN
    mblock.mb_type           := m_index_statistic;
    mblock.mb_type2          := mm_test;
    g01stack_init (mblock, @st_entry, sizeof (st_entry),
          mblock.mb_qual, mblock.mb_qual_size);
    a06set_mtree (acv, mblock, baserec.btreeid);
    (* h.b. PTS 1104683 *)
    mblock.mb_data^.mbp_sample.sam_rows    := c_undefined;
    mblock.mb_data^.mbp_sample.sam_percent := c_100_pct;
    (* h.b. PTS 1115096 *)
    mblock.mb_data^.mbp_sample.sam_with_long := true;
    mblock.mb_data^.mbp_sample.sam_fill1     := false;
    mblock.mb_data^.mbp_sample.sam_fill2     := 0;
    WITH mblock.mb_qual^ DO
        BEGIN
        mmult_pos   := 1;
        mmult_cnt   := 1;
        mfirst_free := 2
        END;
    (*ENDWITH*)
    a06rsend_mess_buf (acv, mblock, cak_return_req, b_err);
    IF  b_err = e_file_not_accessible
    THEN
        BEGIN
        s10fil1 (mblock.mb_qual_size, mblock.mb_qual^.buf, 1,
              mblock.mb_qual_size, chr(0));
        b_err := e_ok;
        END;
    (*ENDIF*)
    END;
(*ENDIF*)
IF  b_err <> e_ok
THEN
    a07_b_put_error (acv, b_err, 1)
(*ENDIF*)
;cccprint('<ak42one_index_s');
END;

(*------------------------------*)

PROCEDURE
        ak42table_statistics ;



VAR
      b_err   : tgg00_BasisError;
      colindex: integer;

BEGIN
cccprint('AK42  ak42table_');
mblock.mb_type  := m_table_statistic;
mblock.mb_type2 := mm_nil;
IF  baserec.bstringcount > 0
THEN
    BEGIN
    mblock.mb_type2 := mm_string;
    WITH baserec DO
        FOR colindex := bfirstindex TO blastindex DO
            WITH bcolumn[ colindex ]^ DO
                IF  NOT (ctdropped in ccolpropset) AND
                    (cdatatyp in [ dstra, dstre, dstrb, dstrdb, dstruni ])
                THEN
                    mblock.mb_type2 := mm_nil;
                (*ENDIF*)
            (*ENDWITH*)
        (*ENDFOR*)
    (*ENDWITH*)
    END;
(*ENDIF*)
mblock.mb_qual^.mtree                    := baserec.btreeid;
mblock.mb_qual^.mtree.fileLeafNodes_gg00 := cgg_nil_leafnodes;
mblock.mb_qual^.mtree.fileHandling_gg00  := [hsWithoutLock_egg00];
mblock.mb_data^.mbp_sample.sam_rows      := c_undefined;
mblock.mb_data^.mbp_sample.sam_percent   := c_100_pct;
(* h.b. PTS 1115096 *)
mblock.mb_data^.mbp_sample.sam_with_long := true;
mblock.mb_data^.mbp_sample.sam_fill1     := false;
mblock.mb_data^.mbp_sample.sam_fill2     := 0;
a06rsend_mess_buf (acv, mblock, cak_return_req, b_err);
IF  b_err <> e_ok
THEN
    a07_b_put_error (acv, b_err, 1)
(*ENDIF*)
;cccprint('<ak42table_stati');
END;

(*------------------------------*)

PROCEDURE
        ak42print_stat_tab_index ;



VAR
      col_cnt : integer;
      v30     : tsp00_C30;
      nam     : tsp00_Sname;

BEGIN
cccprint('AK42  ak42print_');
WITH acv, a41v , mblock.mb_data^.mbp_sample  DO
    CASE  a4sh_kind OF
        sh_odbc_st_index, sh_odbc_st_table :
            BEGIN
            ak42prefix (acv, a41v, col_cnt);
            IF  a4sh_kind = sh_odbc_st_index
            THEN
                a40move_i4 (acv, a41v, sec_key_cnt,
                      NOT c_null, 0, NOT c_to_undef)
            ELSE
                a40move_i4 (acv, a41v, records,
                      NOT c_null, 0, NOT c_to_undef);
            (*ENDIF*)
            a40move_i4 (acv, a41v, nodes, NOT c_null, 0, NOT c_to_undef)
            END;
        sh_indexpages :
            ak42datab_statistic (acv, a41v, n_tab05_leafnodes, nodes);
        OTHERWISE
            BEGIN
            ak42datab_statistic (acv, a41v, n_tab01_root, rootsegm);
            IF  ftsTemp_egg00 in smplFileType_gg00
            THEN
                BEGIN
                IF  ftsConcurrent_egg00 in smplFileType_gg00
                THEN
                    nam := n_temp_concurr
                ELSE
                    nam := n_temp
                (*ENDIF*)
                END
            ELSE
                IF  ftsPerm_egg00 in smplFileType_gg00
                THEN
                    nam := n_perm
                ELSE
                    IF  ftsByteStr_egg00 in smplFileType_gg00
                    THEN
                        nam := n_bytestr
                    ELSE
                        nam := n_unknown;
                    (*ENDIF*)
                (*ENDIF*)
            (*ENDIF*)
            v30 := n_tab02_filetype;
            ak42sname_statistic  (acv, a41v, v30, nam);
            ak42datab_statistic  (acv, a41v, n_tab03_nodes,      nodes);
            ak42datab_statistic  (acv, a41v, n_tab04_indexnodes, indexnodes);
            ak42datab_statistic  (acv, a41v, n_tab05_leafnodes,  leafnodes);
            ak42datab_statistic  (acv, a41v, n_tab06_hight,      hight);
            ak42datab_statistic  (acv, a41v, n_tab07_covering,   treecov);
            ak42datab_statistic  (acv, a41v, n_tab08_croot,      rootcov);
            ak42datab_statistic  (acv, a41v, n_tab09_cindex,     indexcov);
            ak42ref_i4_statistic (acv, a41v, n_tab10_cmin,       min_indexcov);
            ak42ref_i4_statistic (acv, a41v, n_tab11_cmax,       max_indexcov);
            ak42datab_statistic  (acv, a41v, n_tab12_cleaf,      leafcov);
            ak42ref_i4_statistic (acv, a41v, n_tab13_lmin,       min_leafcov);
            ak42ref_i4_statistic (acv, a41v, n_tab14_lmax,       max_leafcov);
            IF  a4sh_kind in [sh_st_table]
            THEN
                BEGIN
                ak42datab_statistic  (acv, a41v, n_tab15_rows, records);
                ak42datab_stat_undef (acv, a41v, n_tab16_rows_per, rec_per_page, (records <> 0));
                ak42datab_statistic  (acv, a41v, n_tab17_min_rows, min_rec_per_page);
                ak42datab_statistic  (acv, a41v, n_tab18_max_rows, max_rec_per_page);
                ak42datab_stat_undef (acv, a41v, n_tab19_avg_rowlen,
                      ave_rec_length, (max_rec_length = 0));
                ak42datab_stat_undef (acv, a41v, n_tab20_min_rowlen,
                      min_rec_length, (max_rec_length = 0));
                ak42datab_stat_undef (acv, a41v, n_tab21_max_rowlen, max_rec_length, true);
                ak42datab_stat_undef (acv, a41v, n_tab22_avg_keylen, ave_key_length, (max_key_length = 0));
                ak42datab_stat_undef (acv, a41v, n_tab23_min_keylen, min_key_length, (max_key_length = 0));
                ak42datab_stat_undef (acv, a41v, n_tab24_max_keylen, max_key_length, true);
                ak42datab_stat_undef (acv, a41v, n_tab25_avg_seplen, ave_sep_length, (max_sep_length = 0));
                ak42datab_stat_undef (acv, a41v, n_tab26_min_seplen, min_sep_length, (max_sep_length = 0));
                ak42datab_stat_undef (acv, a41v, n_tab27_max_seplen, max_sep_length, true);
                ak42datab_statistic  (acv, a41v, n_tab28_def_str,    defined_stringcols);
                ak42datab_stat_undef (acv, a41v, n_tab29_avg_strlen,
                      ave_len_stringcol, defined_stringcols = 0);
                ak42datab_stat_undef (acv, a41v, n_tab30_min_strlen,
                      min_len_stringcol, defined_stringcols = 0);
                ak42datab_stat_undef (acv, a41v, n_tab31_max_strlen,
                      max_len_stringcol, defined_stringcols = 0);
                ak42datab_statistic (acv, a41v,  n_tab32_str_pages, all_stringcol_pages);
                ak42datab_stat_undef (acv, a41v, n_tab33_avg_spages,
                      ave_stringcol_pages, all_stringcol_pages = 0);
                ak42datab_stat_undef (acv, a41v, n_tab34_min_spages,
                      min_stringcol_pages, all_stringcol_pages = 0);
                ak42datab_stat_undef (acv, a41v, n_tab35_max_spages,
                      max_stringcol_pages, all_stringcol_pages = 0);
                END
            ELSE
                BEGIN
                ak42datab_statistic  (acv, a41v, n_inv01_cnt_seckey, sec_key_cnt);
                ak42datab_stat_undef (acv, a41v, n_inv02_avg_seclen, avg_sec_key_len, max_sec_key_len = 0);
                ak42datab_statistic  (acv, a41v, n_inv03_min_seclen, min_sec_key_len);
                ak42datab_statistic  (acv, a41v, n_inv04_max_seclen, max_sec_key_len);
                ak42datab_stat_undef (acv, a41v, n_tab25_avg_seplen, ave_sep_length, (max_sep_length = 0));
                ak42datab_stat_undef (acv, a41v, n_tab26_min_seplen, min_sep_length, (max_sep_length = 0));
                ak42datab_stat_undef (acv, a41v, n_tab27_max_seplen, max_sep_length, true);
                ak42datab_statistic  (acv, a41v, n_inv05_cnt_prim,   prim_key_cnt);
                ak42datab_stat_undef (acv, a41v, n_inv06_avg_listl,
                      avg_prim_per_list DIV 10, prim_key_cnt=0);
                ak42datab_statistic  (acv, a41v, n_inv07_min_listl,  min_prim_per_list);
                ak42datab_statistic  (acv, a41v, n_inv08_max_listl,  max_prim_per_list);
                IF  selectivity
                THEN
                    BEGIN
                    ak42datab_stat_undef (acv, a41v, n_select01,      invsel_1,      false);
                    ak42datab_stat_undef (acv, a41v, n_select05,      invsel_5,      false);
                    ak42datab_stat_undef (acv, a41v, n_select10,      invsel_10,     false);
                    ak42datab_stat_undef (acv, a41v, n_select25,      invsel_25,     false);
                    ak42datab_stat_undef (acv, a41v, n_select_notsel, invsel_notsel, false);
                    END;
                (*ENDIF*)
                END;
            (*ENDIF*)
            END;
        END;
    (*ENDCASE*)
(*ENDWITH*)
;cccprint('<ak42print_stat_');
END;

(*------------------------------*)

PROCEDURE
        a42_check_database ;



VAR
      res            : tsp00_NumError;
      number         : tsp00_Number;
      i              : integer;
      sizes          : ARRAY [1..2] OF tsp00_Int4;
      parsid         : tak_parsid;
      db_space_info  : tgg00_DbSpaceInfo;
      log_device_info: tkb00_LogDeviceInfo;
      log_queue_info : tkb00_LogQueueInfo;
      name           : tak_oldidentifier;

BEGIN
cccprint('AK42  a42_check_');
WITH acv DO
    IF  a_return_segm^.sp1r_returncode = 0
    THEN
        BEGIN
        IF  a_ex_kind = only_parsing
        THEN
            BEGIN
            (*=============================*)
            (* Shortinfo ==> return part   *)
            (*=============================*)
            ak42check_db_shortinfos (acv);
            (*=============================*)
            (* Parsid    ==> return part   *)
            (*=============================*)
            parsid.pid_session       := a_transinf.tri_trans.trSessionId_gg00;
            parsid.pid_parsk         := a_pars_last_key;
            parsid.pid_parsk.p_kind  := m_check_database;
            parsid.pid_appl_info [1] := chr(ord(a_precomp_info_byte));
            parsid.pid_dtime_info[1] := chr(ord(dtf_none));
            a06retpart_move        (acv, @parsid, sizeof (parsid));
            a06finish_curr_retpart (acv, sp1pk_parsid, 1)
            END
        ELSE
            BEGIN
            b01dbspace_statistic (a_transinf.tri_trans, db_space_info);
            IF  a_transinf.tri_trans.trError_gg00 <> e_ok
            THEN
                a07_b_put_error (acv, a_transinf.tri_trans.trError_gg00, 1)
            ELSE
                BEGIN
                kb560GetStatistics (a_transinf.tri_trans.trTaskId_gg00, log_device_info, log_queue_info);
                sizes[1] := ak42evalpercent (db_space_info.db_used_pages, db_space_info.db_total_pages);
                sizes[2] := ak42evalpercent (log_device_info.ldiPagesUsed_kb00, kb560GetLogDeviceSize);
                (* colnames into ret part *)
                (* PTS 1113268 E.Z. *)
                name := ln_serverdb;
                a06colname_retpart_move (acv, @name, 8, csp_ascii);
                name := ln_log;
                a06colname_retpart_move (acv, @name, 3, csp_ascii);
                a06finish_curr_retpart (acv, sp1pk_columnnames, 2);
                (* short infos into retpart *)
                ak42check_db_shortinfos (acv);
                (* values into retpart *)
                FOR i := 1 TO 2 DO
                    BEGIN
                    s41plint (number, 2, 2, 0, sizes[ i ], res);
                    IF  res <> num_ok
                    THEN
                        number[1] := csp_undef_byte
                    ELSE
                        number[1] := csp_defined_byte;
                    (*ENDIF*)
                    a06retpart_move (acv, @number[1], sizeof (tsp00_C3))
                    END;
                (*ENDFOR*)
                a06finish_curr_retpart (acv, sp1pk_data, 2)
                END;
            (*ENDIF*)
            END;
        (*ENDIF*)
        IF  (a_comp_type = at_unknown) OR
            (a_comp_vers < csp1_first_sp1_version)
        THEN
            a_result_name := a01_il_b_identifier
        ELSE
            a_result_name := a01_zero_res_name
        (*ENDIF*)
        END;
    (*ENDIF*)
(*ENDWITH*)
;cccprint('<a42_check_datab');
END;

(*------------------------------*)

PROCEDURE
        ak42check_db_shortinfos ;



VAR
      param_info  : tsp1_param_info;

BEGIN
cccprint('AK42  ak42check_');
WITH param_info DO
    BEGIN
    sp1i_mode     := [sp1ot_mandatory];
    sp1i_io_type   := sp1io_output;
    sp1i_data_type := dfixed;
    sp1i_frac     := 0;
    sp1i_length   := 2;
    sp1i_in_out_len := 3;
    sp1i_bufpos   := 1;
    a06retpart_move (acv,
          @param_info, sizeof (param_info));
    sp1i_bufpos := sp1i_bufpos + sp1i_in_out_len;
    a06retpart_move (acv,
          @param_info, sizeof (param_info));
    END;
(*ENDWITH*)
a06finish_curr_retpart (acv, sp1pk_shortinfo, 2)
;cccprint('<ak42check_db_sh');
END;

(* PTS 1113134 E.Z. *)
(*------------------------------*)

PROCEDURE
        ak42snullname_statistic ;



VAR
      col_cnt : integer;

BEGIN
cccprint('AK42  ak42snulln');
WITH acv, a41v  DO
    IF  a_return_segm^.sp1r_returncode = 0
    THEN
        BEGIN
        ak42prefix    (acv, a41v, col_cnt);
        a40move_const (acv, a41v, @v30, sizeof (v30));
        a40move (acv, a41v, NIL, 0);
        a40move_const (acv, a41v, NIL, 0)
        END;
    (*ENDIF*)
(*ENDWITH*)
;cccprint('<ak42snullname_s');
END;

(*------------------------------*)

PROCEDURE
        ak42sname_statistic ;



VAR
      col_cnt : integer;

BEGIN
cccprint('AK42  ak42sname_');
WITH acv, a41v  DO
    IF  a_return_segm^.sp1r_returncode = 0
    THEN
        BEGIN
        ak42prefix    (acv, a41v, col_cnt);
        a40move_const (acv, a41v, @v30, sizeof (v30));
        a40move (acv, a41v, NIL, 0);
        a40move_const (acv, a41v, @nam, sizeof (nam))
        END;
    (*ENDIF*)
(*ENDWITH*)
;cccprint('<ak42sname_stati');
END;

(*------------------------------*)

PROCEDURE
        a42name_and_val_statistic ;



VAR
      col_cnt : integer;

BEGIN
cccprint('AK42  a42name_an');
WITH acv, a41v  DO
    IF  a_return_segm^.sp1r_returncode = 0
    THEN
        BEGIN
        ak42prefix    (acv, a41v, col_cnt);
        a40move_const (acv, a41v, @vkw, sizeof (vkw));
        IF  value = MAX_INT4_SP00
        THEN
            a40move (acv, a41v, NIL, 0)
        ELSE
            a40move_i4 (acv, a41v, value,
                  NOT c_null, 0, NOT c_to_undef);
        (*ENDIF*)
        a40move_const (acv, a41v, @nam, sizeof (nam))
        END;
    (*ENDIF*)
(*ENDWITH*)
;cccprint('<a42name_and_val');
END;

(*------------------------------*)

PROCEDURE
        ak42hostname_statistic ;



BEGIN
cccprint('AK42  ak42hostna');
WITH acv  DO
    IF  a_return_segm^.sp1r_returncode = 0
    THEN
        BEGIN
        a40move_const (acv, a41v, @v30, sizeof (v30));
        a40move (acv, a41v, NIL, 0);
        a40move_const (acv, a41v, @nam, sizeof (nam))
        END;
    (*ENDIF*)
(*ENDWITH*)
;cccprint('<ak42hostname_st');
END;

(*------------------------------*)

PROCEDURE
        ak42get_refinfo ;



VAR
      procid            : tsp00_TaskId;
      (**)
      _sseq_cache_total : tsp00_8ByteCounter;           (* PTS 1103743 JA 1999-08-25 *)
      _sseq_cache_hit   : tsp00_8ByteCounter;
      _sseq_cache_miss  : tsp00_8ByteCounter;
      (**)
      lock_info         : tkb05_LockInfo;
      log_queue_info    : tkb00_LogQueueInfo;
      log_device_info   : tkb00_LogDeviceInfo;

BEGIN
cccprint('AK42  ak42get_re');
(* PTS 1103033 E.Z.; PTS 1103743 JA 1999-08-25 *)
WITH acv, info DO
    BEGIN
    b01show_monitor  (a_transinf.tri_trans.trTaskId_gg00, info);
    (* PTS 1103735 E.Z. *)
    (* PTS 1111134 E.Z. *)
    k720show_monitor (info);
    procid := a_transinf.tri_trans.trTaskId_gg00;
    (* *)
    kb560GetStatistics (procid, log_device_info, log_queue_info);
    WITH log_queue_info DO
        BEGIN
        reference_array [ ilog_queue_size    ] := lqiLogQueueSize_kb00;
        reference_array [ icount_group       ] := lqiCountGroup_kb00;
        reference_array [ icount_insert      ] := lqiCountInsert_kb00;
        reference_array [ icount_request     ] := lqiCountRequest_kb00;
        reference_array [ icount_wait        ] := lqiCountWait_kb00;
        reference_array [ imaxqueue_used     ] := lqiMaxQueueUsed_kb00;
        reference_array [ imaxwait_logpage   ] := lqiMaxWaitLogpage_kb00;
        reference_array [ iavg_sum_pagewaits ] := lqiAvgSumPagewaits_kb00
        END;
    (*ENDWITH*)
    k51info_lock (procid, lock_info);
    WITH lock_info DO
        BEGIN
        reference_array [ iavg_lock_used    ] := infAvgUsed_kb05;
        reference_array [ imax_lock_used    ] := infMaxUsed_kb05;
        reference_array [ icollisions       ] := infSqlCollisions_kb05; (* PTS 1105492 JA 2000-03-15 *)
        reference_array [ iescalations      ] := infEscalations_kb05;
        reference_array [ ideadlocks        ] := infDeadlocks_kb05;
        reference_array [ irequest_timeouts ] := infSqlReqTimeouts_kb05 + infObjReqTimeouts_kb05;
        reference_array [ inew_rows         ] := infRowEntryAmount_kb05;
        reference_array [ inew_tabs         ] := infTabEntryAmount_kb05;
        reference_array [ ikb_calls         ] := infKbCalls_kb05
        END;
    (*ENDWITH*)
    reference_array [ itranscount       ] := 0(*ldiWriteTransCnt_kb00*);
    (* *)
    k57info_seq_cache (procid, _sseq_cache_total, _sseq_cache_hit, _sseq_cache_miss);
    reference_array [ isseq_cache_total ] := _sseq_cache_total;
    reference_array [ isseq_cache_hit   ] := _sseq_cache_hit;
    reference_array [ isseq_cache_miss  ] := _sseq_cache_miss;
    (* PTS 1103033 E.Z. *)
    a260statistics (external_dbproc_calls,  internal_dbproc_calls);
    a262statistics (external_trigger_calls, internal_trigger_calls);
    (* PTS 1109469, T.A. 2001-03-22 *)
    reference_array [idata_sql_cache_total] := reference_array [idata_cache_total];
    reference_array [idata_sql_cache_hit  ] := reference_array [idata_cache_hit];
    reference_array [idata_sql_cache_miss ] := reference_array [idata_cache_miss];
    (* PTS 1115043 E.Z. *)
    IF  g01is_livecache
    THEN
        BEGIN
        (* PTS 1111572 E.Z. *)
        reference_array [idata_cache_total] := reference_array [idata_cache_total]
              + reference_array [idata_obj_cache_total]
              + reference_array[idata_obj_log_cache_total]
              + reference_array[idata_history_cache_total];
        reference_array [idata_cache_hit] := reference_array [idata_cache_hit]
              + reference_array [idata_obj_cache_hit]
              + reference_array[idata_obj_log_cache_hit]
              + reference_array[idata_history_cache_hit];
        reference_array [idata_cache_miss] := reference_array [idata_cache_miss]
              + reference_array [idata_obj_cache_miss]
              + reference_array[idata_obj_log_cache_miss]
              + reference_array[idata_history_cache_miss];
        END
    (*ENDIF*)
    END;
(*ENDWITH*)
;cccprint('<ak42get_refinfo');
END;

(*------------------------------*)

PROCEDURE
        ak42get_tablename ;



BEGIN
cccprint('AK42  ak42get_ta');
WITH acv, a41v DO
    BEGIN
    a4ti := a_ap_tree^[ a4ti ].n_lo_level;
    a11get_check_table (acv, false, false, false, required_priv,
          (a_is_ddl = ddl_update_statistics) AND (required_priv <> []),
          true, d_fix, a4ti, a4authname, a4tablen, a4p_arr);
    IF  a_return_segm^.sp1r_returncode = 0
    THEN
        IF  NOT (a4p_arr.pbasep^.sbase.btablekind
            in [ twithkey, twithoutkey ])
        THEN
            a07_nb_put_error (acv, e_missing_basetable,
                  a_ap_tree^[ a4ti ].n_pos, a4tablen);
        (*ENDIF*)
    (*ENDIF*)
    IF  a_return_segm^.sp1r_returncode = 0
    THEN
        BEGIN
        a4ti := a_ap_tree^[ a4ti ].n_sa_level;
        IF  (a4ti <> 0) AND (a_ap_tree^[ a4ti ].n_proc <> a42)
        THEN
            BEGIN
            a05_identifier_get (acv, a4ti, sizeof (a4coln), a4coln);
            a4ti := a_ap_tree^[ a4ti ].n_sa_level
            END;
        (*ENDIF*)
        END;
    (*ENDIF*)
    END;
(*ENDWITH*)
;cccprint('<ak42get_tablena');
END;

(*------------------------------*)

PROCEDURE
        a42inc_refresh_cnt ;



BEGIN
cccprint('AK42  a42inc_ref');
ak42refresh_cnt := ak42refresh_cnt + 1;
IF  is_fast
THEN
    ak42fast_refresh_cnt := ak42fast_refresh_cnt + 1
(*ENDIF*)
;cccprint('<a42inc_refresh_');
END;

(*------------------------------*)

PROCEDURE
        a42index_inf_to_messbuf ;



CONST
      c_max_st_index   = MAX_COL_PER_INDEX_GG00;

VAR
      found          : boolean;
      send_messbuf   : boolean;
      required_priv  : tgg00_PrivilegeSet;
      b_err          : tgg00_BasisError;
      a4save         : integer;
      j              : integer;
      mindex_no      : tsp_int2;
      stack          : ARRAY[1..c_max_st_index] OF tgg00_StackEntry;
      index_scan_rec : tak_index_scan_record;

BEGIN
cccprint('AK42  a42index_i');
WITH acv, a41v DO
    BEGIN
    IF  a_return_segm^.sp1r_returncode = 0
    THEN
        BEGIN
        b_err        := e_ok;
        send_messbuf := NOT selectivity;
        a4save       := a4ti;
        a4ti         := a_ap_tree^ [ a4ti ].n_lo_level;
        a4priv_check := true;
        indexn       := a01_il_b_identifier;
        WITH a_ap_tree^[ a4ti ] DO
            IF  n_symb = s_identifier
            THEN
                BEGIN
                (* indexname *)
                a05_identifier_get (acv, a4ti, sizeof (indexn), indexn)
                END
            ELSE
                a4ti := a4save;
            (*ENDIF*)
        (*ENDWITH*)
        IF  (diagnose_index AND
            (a_current_user_kind in [usysdba, ucontroluser]))
            OR (* PTS 1109358, allow update stat column for domain user *)
            ((a_is_ddl = ddl_update_statistics) AND
            ( acv.a_curr_user_name = a01_i_domain))
        THEN
            required_priv := []
        ELSE
            required_priv := [r_owner];
        (*ENDIF*)
        ak42get_tablename (acv, a41v, required_priv);
        END;
    (*ENDIF*)
    IF  (a_return_segm^.sp1r_returncode = 0) AND
        ((a_ex_kind <> only_parsing) OR (a4sh_kind = sh_no_kind))
    THEN
        BEGIN
        selectivity := NOT selectivity AND (a4ti <> 0);
        mblock.mb_struct := mbs_stack_addr;
        IF  diagnose_index
        THEN
            BEGIN
            mblock.mb_type  := m_diagnose;
            mblock.mb_type2 := mm_index
            END
        ELSE
            BEGIN
            mblock.mb_type  := m_index_statistic;
            IF  selectivity
            THEN
                mblock.mb_type2 := mm_test
            ELSE
                mblock.mb_type2 := mm_nil
            (*ENDIF*)
            END;
        (*ENDIF*)
        found := false;
        IF  ( indexn = a01_il_b_identifier )
        THEN
            BEGIN
            IF  NOT a061exist_columnname (a4p_arr.pbasep^.sbase,
                a4coln, a4col_ptr)
            THEN
                a07_b_put_error (acv, e_unknown_columnname, 1)
            ELSE
                BEGIN
                IF  ( a4p_arr.pbasep^.sbase.bindexexist )
                THEN
                    BEGIN
                    a24init_index_scan (acv, a4p_arr.pbasep^.sbase.btreeid.fileTabId_gg00,
                          index_scan_rec);
                    WHILE NOT found AND
                          a24next_named_index (acv, index_scan_rec)
                          DO
                        WITH index_scan_rec, isr_buf^.smindex.indexdef[isr_index] DO
                            IF  ( icount = 1 ) AND
                                ( icolseq[ 1 ] = a4col_ptr^.creccolno )
                            THEN
                                BEGIN
                                found := true;
                                a24get_indexname( acv, isr_buf, isr_index,
                                      indexn );
                                END;
                            (*ENDIF*)
                        (*ENDWITH*)
                    (*ENDWHILE*)
                    END;
                (*ENDIF*)
                IF  ( NOT found )
                THEN
                    a07_b_put_error (acv, e_column_not_indexed, 1);
                (*ENDIF*)
                END;
            (*ENDIF*)
            END;
        (*ENDIF*)
        IF  (a_return_segm^.sp1r_returncode = 0) AND
            (indexn <> a01_il_b_identifier)
        THEN
            WITH index_scan_rec DO
                BEGIN
                (*========== multiple index ==========*)
                IF  ( NOT found )
                THEN
                    BEGIN
                    IF  ( a4p_arr.pbasep^.sbase.bindexexist )
                    THEN
                        BEGIN
                        a24find_indexname (acv,
                              a4p_arr.pbasep^.sbase.bsurrogate,
                              indexn, index_scan_rec);
                        found := isr_buf <> NIL;
                        END;
                    (*ENDIF*)
                    END;
                (*ENDIF*)
                IF  ( found )
                THEN
                    BEGIN
                    mblock.mb_qual^.mtree := a4p_arr.pbasep^.sbase.btreeid;
                    mblock.mb_st          := @stack[1];
                    mblock.mb_qual^.mst_addr := mblock.mb_st;
                    mblock.mb_st_size :=
                          c_max_st_index * sizeof (tgg00_StackEntry);
                    mblock.mb_st_max := c_max_st_index;
                    WITH isr_buf^.smindex.indexdef[isr_index],
                         mblock, mb_qual^ DO
                        BEGIN
                        mmult_pos  := 1;
                        mmult_cnt  := 1;
                        mb_st^ [1] := icolstack[ 1 ];
                        mfirst_free := 2;
                        IF  diagnose_index
                        THEN
                            FOR j := 2 TO icount DO
                                BEGIN
                                mb_st^ [j]  := icolstack[ j ];
                                mmult_cnt   := succ(mmult_cnt);
                                mfirst_free := succ(mfirst_free);
                                END;
                            (*ENDFOR*)
                        (* h.b. PTS 1104683 *)
                        (*ENDIF*)
                        mblock.mb_data^.mbp_sample.sam_rows    := c_undefined;
                        mblock.mb_data^.mbp_sample.sam_percent := c_100_pct;
                        (* h.b. PTS 1115096 *)
                        mblock.mb_data^.mbp_sample.sam_with_long := true;
                        mblock.mb_data^.mbp_sample.sam_fill1     := false;
                        mblock.mb_data^.mbp_sample.sam_fill2     := 0;
                        IF  send_messbuf
                        THEN
                            a06rsend_mess_buf (acv, mblock,
                                  cak_return_req, b_err);
                        (*ENDIF*)
                        END
                    (*ENDWITH*)
                    END
                ELSE
                    a07_nb_put_error (acv, e_unknown_indexname,
                          1, indexn)
                (*ENDIF*)
                END;
            (*ENDWITH*)
        (*ENDIF*)
        IF  b_err <> e_ok
        THEN
            a07_b_put_error (acv, b_err, 1);
        (*ENDIF*)
        END;
    (*ENDIF*)
    END;
(*ENDWITH*)
;cccprint('<a42index_inf_to');
END;

(*------------------------------*)

PROCEDURE
        ak42session_to_sname ;



VAR
      pos    : integer;
      ln_len : integer;
      ln     : tsp00_Line;

BEGIN
cccprint('AK42  ak42sessio');
FOR ln_len := 1 TO sizeof (n) DO
    ln [ln_len] := ' ';
(*ENDFOR*)
ln_len := 0;
gg06SessionToLine (session, ln_len, ln);
IF  ln_len > sizeof (n)
THEN
    ln_len := sizeof (n);
(*ENDIF*)
IF  ln_len > max_len
THEN
    ln_len := max_len;
(*ENDIF*)
IF  (ln [1] in ['0'..'9']) AND (max_len <= sizeof (n))
THEN
    pos := max_len - ln_len + 1
ELSE
    pos := 1;
(*ENDIF*)
n := bsp_sname;
s10mv1 (sizeof (ln), sizeof (n), ln, 1, n, pos, ln_len)
;cccprint('<ak42session_to_');
END;

(*------------------------------*)

PROCEDURE
        ak42xparams ;



VAR
      b_err     : tgg00_BasisError;
      curr_key  : tgg00_Lkey;
      data      : tgg00_Rec;
      valname   : tsp00_C18;

BEGIN
cccprint('AK42  ak42xparam');
(* creates a result table containing the main entries *)
(* of the xparam record                               *)
a40get_catalog_table (acv, a41v);
IF  (acv.a_return_segm^.sp1r_returncode = 0) AND
    (a36xparam_tree.fileRoot_gg00 <> NIL_PAGE_NO_GG00)
THEN
    WITH acv DO
        BEGIN
        curr_key.len := 0;
        REPEAT
            (* do not use b07cnext_record for concurrent temp file *)
            b02next_record (a_transinf.tri_trans, a36xparam_tree,
                  curr_key, NOT c_inclusive, data);
            b_err := a_transinf.tri_trans.trError_gg00;
            IF  (b_err = e_ok) OR (b_err = e_key_not_found)
            THEN
                BEGIN
                b_err        := e_ok;
                curr_key.len := data.keylen;
                g10mv2 ('VAK42 ',   8,
                      sizeof (data.info), sizeof (curr_key.k),
                      data.info, 1, curr_key.k, 1, data.keylen, b_err);
                g10mv6 ('VAK42 ',   9,
                      sizeof (data.info), sizeof (valname),
                      data.info, 1, valname, 1, sizeof (valname), b_err);
                IF  valname <> 'CONTROLPASSWORD   '
                THEN
                    BEGIN
                    a40move_const (acv, a41v,
                          @data.info[1], mxsp_c18);
                    a40move_const(acv, a41v,
                          @data.info[ mxsp_c18 + 1],
                          data.len - cgg_rec_key_offset - mxsp_c18);
                    END
                (*ENDIF*)
                END;
            (*ENDIF*)
        UNTIL
            (b_err <> e_ok) OR
            (a_return_segm^.sp1r_returncode <> 0);
        (*ENDREPEAT*)
        END;
    (*ENDWITH*)
(*ENDIF*)
;cccprint('<ak42xparams    ');
END;

(*------------------------------*)

PROCEDURE
        ak42ascii_const_move ;



VAR
      ix  : integer;

BEGIN
cccprint('AK42  ak42ascii_');
IF  g01unicode
THEN
    FOR ix := 1 TO srclen DO
        BEGIN
        dest [ destpos ]     := csp_unicode_mark;
        dest [ destpos + 1 ] := src^ [ ix ];
        destpos              := destpos + 2;
        END
    (*ENDFOR*)
ELSE
    BEGIN
    g10mv3 ('VAK42 ',  10,
          srclen, sizeof (dest), src^, 1,
          dest, destpos, srclen, e);
    destpos := destpos + srclen;
    END;
(*ENDIF*)
;cccprint('<ak42ascii_const');
END;

(*------------------------------*)

PROCEDURE
        ak42move_trans_no ;



VAR
      length   : integer;
      c20      : tsp00_C20;
      aux_line : tsp00_Line;

BEGIN
cccprint('AK42  ak42move_t');
(* PTS 1108234 JA 2001-01-19 *)
length := 0;
IF  gg06IsNilTrans (trans)
THEN
    a40move (acv, a41v, NIL, 0)  (* undef *)
ELSE
    BEGIN
    gg06TransToLine (trans, length, aux_line);
    IF  length > sizeof (c20)
    THEN
        length := sizeof (c20);
    (*ENDIF*)
    c20 := bsp_c20;
    g10mv5 ('VAK42 ',  11,
          sizeof (aux_line), sizeof(c20),
          aux_line, 1, c20, sizeof(c20)+1-length,
          length, acv.a_return_segm^.sp1r_returncode);
    a40move_const (acv, a41v, @c20, sizeof(c20))
    END
(*ENDIF*)
;cccprint('<ak42move_trans_');
END;

(* PTS 1108435 E.Z. *)
(*------------------------------*)

PROCEDURE
        ak42internal_state ;



VAR
      component     : tsp00_C20;
      description   : tsp00_C40;
      value         : tsp00_C12;

BEGIN
cccprint('AK42  ak42intern');
WITH acv, a41v DO
    BEGIN
    a40get_catalog_table (acv, a41v);
    IF  (a_return_segm^.sp1r_returncode = 0) AND
        (a_ex_kind <> only_parsing)
    THEN
        BEGIN
        k57select_restartrec (acv.a_transinf.tri_trans.trTaskId_gg00, @acv, @a41v);
        k57select_log_info (acv.a_transinf.tri_trans.trTaskId_gg00, @acv, @a41v);
        END
    (*ENDIF*)
    END
(*ENDWITH*)
;cccprint('<ak42internal_st');
END;

(*------------------------------*)

PROCEDURE
        a42bool_to_intern_state ;



VAR
      n : tsp00_Sname;

BEGIN
cccprint('AK42  a42bool_to');
(* PTS 1108435 JA 2000-11-30 *)
(* inserts a result record with a bool value into INTERNAL_STATE *)
IF  bool_val
THEN
    n := n_yes
ELSE
    n := n_no;
(*ENDIF*)
a40move_const (acv_ptr^, a41v_ptr^, @component,   sizeof (component));
a40move_const (acv_ptr^, a41v_ptr^, @description, sizeof (description));
a40move_const (acv_ptr^, a41v_ptr^, @n,           sizeof (n))
;cccprint('<a42bool_to_inte');
END;

(*------------------------------*)

PROCEDURE
        a42cint6_to_intern_state ;



VAR
      ln_len : integer;
      ln     : tsp00_Line;

BEGIN
cccprint('AK42  a42cint6_t');
(* PTS 1108435 JA 2000-11-30 *)
(* inserts a result record with a cint6 into INTERNAL_STATE *)
ln_len := 0;
gg06Cint6ToLine (cint6, ln_len, ln);
a40move_const   (acv_ptr^, a41v_ptr^, @component,   sizeof (component));
a40move_const   (acv_ptr^, a41v_ptr^, @description, sizeof (description));
a40move_const   (acv_ptr^, a41v_ptr^, @ln,          ln_len)
;cccprint('<a42cint6_to_int');
END;

(*------------------------------*)

PROCEDURE
        a42counter_to_intern_state ;



BEGIN
cccprint('AK42  a42counter');
(* PTS 1108435 JA 2000-11-30 *)
(* inserts a result record with a counter into INTERNAL_STATE *)
a40move_const   (acv_ptr^, a41v_ptr^, @component,   sizeof (component));
a40move_const   (acv_ptr^, a41v_ptr^, @description, sizeof (description));
a40move_counter (acv_ptr^, a41v_ptr^, counter, NOT c_null, 0, NOT c_to_undef)
;cccprint('<a42counter_to_i');
END;

(*------------------------------*)

PROCEDURE
        a42date_to_intern_state ;



VAR
      ln_len     : integer;
      str_date   : tsp00_Date;
      dummy_time : tsp00_Time;
      ln         : tsp00_Line;

BEGIN
cccprint('AK42  a42date_to');
(* PTS 1108435 JA 2000-11-30 *)
(* inserts a result record with a date value into INTERNAL_STATE *)
ln_len := 0;
g17intdate_time (int_date, 0, str_date, dummy_time);
g17date_to_line (str_date, ln_len, ln);
a40move_const   (acv_ptr^, a41v_ptr^, @component,   sizeof (component));
a40move_const   (acv_ptr^, a41v_ptr^, @description, sizeof (description));
a40move_const   (acv_ptr^, a41v_ptr^, @ln,          ln_len)
;cccprint('<a42date_to_inte');
END;

(*------------------------------*)

PROCEDURE
        a42int4_to_intern_state ;



BEGIN
cccprint('AK42  a42int4_to');
(* PTS 1108435 JA 2000-11-30 *)
(* inserts a result record with an int4 into INTERNAL_STATE *)
a40move_const (acv_ptr^, a41v_ptr^, @component,   sizeof (component));
a40move_const (acv_ptr^, a41v_ptr^, @description, sizeof (description));
a40move_i4    (acv_ptr^, a41v_ptr^, int_val, NOT c_null, 0, NOT c_to_undef);
;cccprint('<a42int4_to_inte');
END;

(*------------------------------*)

PROCEDURE
        a42logpos_to_intern_state ;



VAR
      ln_len : integer;
      ln     : tsp00_Line;

BEGIN
cccprint('AK42  a42logpos_');
(* PTS 1108435 JA 2000-11-30 *)
(* inserts a result record with a logpos into INTERNAL_STATE *)
ln_len := 0;
g17trimint4_to_line (logpos.logLpno_gg00, ln_len, ln);
ln_len      := ln_len + 1;
ln [ln_len] := '.';
g17trimint4_to_line (logpos.logPpos_gg00, ln_len, ln);
ln_len      := ln_len + 1;
ln [ln_len] := ' ';
g17pagetype_to_line (logpos.logPageType_gg00, ln_len, ln);
a40move_const (acv_ptr^, a41v_ptr^, @component,   sizeof (component));
a40move_const (acv_ptr^, a41v_ptr^, @description, sizeof (description));
a40move_const (acv_ptr^, a41v_ptr^, @ln,          ln_len)
;cccprint('<a42logpos_to_in');
END;

(*------------------------------*)

PROCEDURE
        a42mess_to_intern_state ;



VAR
      ln_len : integer;
      ln     : tsp00_Line;

BEGIN
cccprint('AK42  a42mess_to');
(* PTS 1108435 JA 2000-11-30 *)
(* inserts a result record with a message type into INTERNAL_STATE *)
ln_len := 0;
g17messtype_to_line (messtype, ln_len, ln);
a40move_const (acv_ptr^, a41v_ptr^, @component,   sizeof (component));
a40move_const (acv_ptr^, a41v_ptr^, @description, sizeof (description));
a40move_const (acv_ptr^, a41v_ptr^, @ln,          ln_len)
;cccprint('<a42mess_to_inte');
END;

(*------------------------------*)

PROCEDURE
        a42mess2_to_intern_state ;



VAR
      ln_len : integer;
      ln     : tsp00_Line;

BEGIN
cccprint('AK42  a42mess2_t');
(* PTS 1108435 JA 2000-11-30 *)
(* inserts a result record with a message2 type into INTERNAL_STATE *)
ln_len := 0;
g17mess2type_to_line (mess2type, ln_len, ln);
a40move_const (acv_ptr^, a41v_ptr^, @component,   sizeof (component));
a40move_const (acv_ptr^, a41v_ptr^, @description, sizeof (description));
a40move_const (acv_ptr^, a41v_ptr^, @ln,          ln_len)
;cccprint('<a42mess2_to_int');
END;

(*------------------------------*)

PROCEDURE
        a42session_to_intern_state ;



VAR
      ln_len : integer;
      ln     : tsp00_Line;

BEGIN
cccprint('AK42  a42session');
(* PTS 1108435 JA 2000-11-30 *)
(* inserts a result record with a session no into INTERNAL_STATE *)
ln_len := 0;
gg06SessionToLine (session, ln_len, ln);
a40move_const     (acv_ptr^, a41v_ptr^, @component,   sizeof (component));
a40move_const     (acv_ptr^, a41v_ptr^, @description, sizeof (description));
a40move_const     (acv_ptr^, a41v_ptr^, @ln,          ln_len)
;cccprint('<a42session_to_i');
END;

(*------------------------------*)

PROCEDURE
        a42str_to_intern_state ;



VAR
      aux_len : integer;

BEGIN
cccprint('AK42  a42str_to_');
(* PTS 1108435 JA 2000-11-30 *)
(* inserts a result record with a string into INTERNAL_STATE *)
IF  str_len > mxsp_c40
THEN
    aux_len := mxsp_c40
ELSE
    aux_len := str_len;
(*ENDIF*)
a40move_const (acv_ptr^, a41v_ptr^, @component,   sizeof (component));
a40move_const (acv_ptr^, a41v_ptr^, @description, sizeof (description));
a40move_const (acv_ptr^, a41v_ptr^, str_ptr,      aux_len)
;cccprint('<a42str_to_inter');
END;

(*------------------------------*)

PROCEDURE
        a42surrogate_to_intern_state ;



VAR
      ln_len : integer;
      ln     : tsp00_Line;

BEGIN
cccprint('AK42  a42surroga');
(* PTS 1108435 JA 2000-11-30 *)
(* inserts a result record with a surrogate into INTERNAL_STATE *)
ln_len := 0;
g17surrogate_to_line (surrogate, ln_len, ln);
a40move_const (acv_ptr^, a41v_ptr^, @component,   sizeof (component));
a40move_const (acv_ptr^, a41v_ptr^, @description, sizeof (description));
a40move_const (acv_ptr^, a41v_ptr^, @ln,          ln_len)
;cccprint('<a42surrogate_to');
END;

(*------------------------------*)

PROCEDURE
        a42time_to_intern_state ;



VAR
      ln_len     : integer;
      dummy_date : tsp00_Date;
      str_time   : tsp00_Time;
      ln         : tsp00_Line;

BEGIN
cccprint('AK42  a42time_to');
(* PTS 1108435 JA 2000-11-30 *)
(* inserts a result record with a time value into INTERNAL_STATE *)
ln_len := 0;
g17intdate_time (0, int_time, dummy_date, str_time);
g17time_to_line (str_time, ln_len, ln);
a40move_const   (acv_ptr^, a41v_ptr^, @component,   sizeof (component));
a40move_const   (acv_ptr^, a41v_ptr^, @description, sizeof (description));
a40move_const   (acv_ptr^, a41v_ptr^, @ln,          ln_len)
;cccprint('<a42time_to_inte');
END;

(*------------------------------*)

PROCEDURE
        a42trans_to_intern_state ;



VAR
      ln_len : integer;
      ln     : tsp00_Line;

BEGIN
cccprint('AK42  a42trans_t');
(* PTS 1108435 JA 2000-11-30 *)
(* inserts a result record with a transaction no into INTERNAL_STATE *)
ln_len := 0;
gg06TransToLine (trans, ln_len, ln);
a40move_const   (acv_ptr^, a41v_ptr^, @component,   sizeof (component));
a40move_const   (acv_ptr^, a41v_ptr^, @description, sizeof (description));
a40move_const   (acv_ptr^, a41v_ptr^, @ln,          ln_len)
;cccprint('<a42trans_to_int');
END;

(* PTS 1110418 E.Z. *)
(*------------------------------*)

PROCEDURE
        a42uint1_to_intern_state ;



BEGIN
cccprint('AK42  a42uint1_t');
(*
      the type of the last parameter is correct.
      It is one factor too high.
      We do not want to output those integer with negative sign.
      Therefore we need the next integer available in Pascal
      able to hold the correct value.
      *)
(* inserts a result record with an UInt1/Int2 into INTERNAL_STATE *)
a40move_const (acv_ptr^, a41v_ptr^, @component,   sizeof (component));
a40move_const (acv_ptr^, a41v_ptr^, @description, sizeof (description));
a40move_i4    (acv_ptr^, a41v_ptr^, int_val, NOT c_null, 0, NOT c_to_undef);
;cccprint('<a42uint1_to_int');
END;

(*------------------------------*)

PROCEDURE
        a42uint2_to_intern_state ;



BEGIN
cccprint('AK42  a42uint2_t');
(*
      the type of the last parameter is correct.
      It is one factor too high.
      We do not want to output those integer with negative sign.
      Therefore we need the next integer available in Pascal
      able to hold the correct value.
      *)
(* inserts a result record with an UInt2/Int4 into INTERNAL_STATE *)
a40move_const (acv_ptr^, a41v_ptr^, @component,   sizeof (component));
a40move_const (acv_ptr^, a41v_ptr^, @description, sizeof (description));
a40move_i4    (acv_ptr^, a41v_ptr^, int_val, NOT c_null, 0, NOT c_to_undef);
;cccprint('<a42uint2_to_int');
END;

(*------------------------------*)

PROCEDURE
        a42uint4_to_intern_state ;



BEGIN
cccprint('AK42  a42uint4_t');
(*
      the type of the last parameter is correct.
      It is one factor too high.
      We do not want to output those integer with negative sign.
      Therefore we need the next integer available in Pascal
      able to hold the correct value.
      *)
(* inserts a result record with an UInt4/8ByteCounter into INTERNAL_STATE *)
a40move_const   (acv_ptr^, a41v_ptr^, @component,   sizeof (component));
a40move_const   (acv_ptr^, a41v_ptr^, @description, sizeof (description));
a40move_counter (acv_ptr^, a41v_ptr^, int_val, NOT c_null, 0, NOT c_to_undef)
;cccprint('<a42uint4_to_int');
END;

(* END PTS 1110418 E.Z. *)


(*
static char ident_mf[] = "@(#)vak42 SLOW I386 UNIX LINUX PTOC -REL30=1 -RELVER=R74 -DIAGNOSE=1 -BETA=1";
*)
