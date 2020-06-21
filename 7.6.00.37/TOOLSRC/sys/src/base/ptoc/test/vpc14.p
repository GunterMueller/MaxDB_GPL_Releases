

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


      (*****************************************)
      (* tokens for the lexical analysis *)
      (* 1. tokens for all precompilers *)
      cpr_s_unknown                =   0;
      cpr_s_colon                  =   1;
      cpr_s_comma                  =   2;
      cpr_s_divide                 =   3;
      cpr_s_eof                    =   4;
      cpr_s_equal                  =   5;
      cpr_s_fixed_point_literal    =   6;
      cpr_s_floating_point_literal =   7;
      cpr_s_identifier             =   8;
      cpr_s_leftindpar             =   9;
      cpr_s_leftpar                =  10;
      cpr_s_macro                  =  11;
      cpr_s_minus                  =  12;
      cpr_s_parameter_name         =  13;
      cpr_s_greater                =  14;
      cpr_s_plus                   =  15;
      cpr_s_point                  =  16;
      cpr_s_rightindpar            =  17;
      cpr_s_rightpar               =  18;
      cpr_s_semicolon              =  19;
      cpr_s_asterisk               =  20;
      cpr_s_string_literal         =  21;
      cpr_s_unsigned_integer       =  22;
      cpr_s_bufeof                 =  23;
      cpr_s_cstring_literal        =  24;
      cpr_s_question_mark          =  25;
      cpr_sqldbcount_one = '\C1\10\00\00\00\00\00\00\00\00';
      cpr_parsidnull     = '\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00';
      cpr_nullchar       = '\00';
      cpr_ht_code        = '\09';    (* tabulator          *)
      cpr_leftind_par    = '[';    (* eckige klammer auf *)
      cpr_rightind_par   = ']';    (* eckige klammer zu  *)
      cpr_backslash      = '\\';
      cpr_pc_endsign     = '!';
      cpr_recordchr      = '!';
      cpr_recordchr2     = '~';      (* tilde *)
      cpr_recordchr3     = '|';      (* senkrechter strich *)
      cpr_macrochr          = '%';
      cpr_paramchr          = ':';
      cpr_preparechr        = '?';      (* ?? *)
      cpr_warningchr        = 'W';
      cpr_dquosym           = '"';
      cpr_quotsym           = '''';
      cpr_asterisk          = '*';
      cpr_cbegin_par        = '{';    (* geschweifte klammer auf *)
      cpr_cend_par          = '}';    (* geschweifte klammer zu  *)
      cpr_numblen_int2       =  5;       (* int2 : fixed len    *)
      cpr_numblen_int4       = 10;       (* int4 : fixed len    *)
      cpr_numblen_int8       = 18;       (* int8 : fixed len    *)
      cpr_numblen_r4      =  6;       (* real4: float len    *)
      cpr_numblen_r8      = 15;       (* real8: float len    *)
      cpr_result_xuser_read_err = 1;
      cpr_result_system_required= 2;
      cpr_result_too_many_users = 3;
      cpr_result_user_passw     = 4;
      cpr_result_whenever_stop  = 5;
      cpr_result_option_error   = 6;
      cpr_result_trace_error    = 7;
      cpr_result_sqlxa_invalid  = 8;
      (* ------------tpr00_TraceParseID_Enum---------------- *)
      TraceParseIDInput_epr00        = 1;
      TraceParseIDOutput_epr00      = 2;
      TraceParseIDChange_epr00       = 3;
      TraceParseIDNew_epr00          = 4;
      TraceParseIDSQLStmt_epr00      = 5;
      TraceParseIDSQLStmtBlank_epr00 = 6;
      TraceParseIDUseExecute_epr00   = 7;
      TraceParseIDFetch_epr00        = 8;
      TraceParseIDBlank_epr00        = 9;
      TraceParseIDAgain_epr00        = 10;
      TraceParseIDVersion_epr00      = 11;
      TraceParseIDSelect_epr00       = 12;
      TraceParseIDDrop_epr00         = 13;
      TraceParseIDFromCache_epr00    = 14;
      TraceParseIDSQLStmtMass_epr00  = 15;
      TraceParseIDOther_epr00        = 99;
      cpr_minint4         = - 2147483647;
      cpr_caidconst      = 'SQLCA   ';
      cpr_daidconst      = 'SQLDA   ';
      cpr_ghatzelcheck   = 'Hatzel-Check'; (* Ha ha, Hahahaha *)
      cpr_lhatzelcheck   = 'Check_Hatzel'; (* Ho ho, Hohohoho *)
      cpr_n_parameter    = 'PARAMETER                       ';
      (*----------------------------------------------------------*)
      (*   constante werden im precomilelauf errechnet muss >= 1  *)
      (*----------------------------------------------------------*)
      mxpr_partbuf        = 16000;     (* connect kommando buffer *)
      mxpr_sqlva1         = 10000;     (* host-variables addr *)
      mxpr_sqlva2         =  5000;     (* variable datatypes *)
      mxpr_sqlva3         = 10000;     (* variable names *)
      mxpr_sqlpa          =  5000;     (* parameter *)
      mxpr_sqlsa          =   500;     (* sequencs  *)
      mxpr_sqlka          =  3000;     (* kommandos *)
      mxpr_sqlpr          =   500;     (* prepare   *)
      mxpr_sqlst          =  1000;     (* statement *)
      mxpr_sqlfa          =   200;     (* fetch statements *)
      mxpr_sqlsd          =    20;     (* select direct fast commands *)
      mxpr_sqln           =   300;     (* anz sqlvar*)
      mxpr_sqlfn          =    50;     (* anz include-files im modul  *)
      mxpr_sqlat          =  500;     (* at-area, oracle at db-cmd *)
      mxpr_sqlcu          =  200;     (* cursor-area  *)
      mxpr_sqlsn          =  100;     (* snrsor-area  *)
      (*------------------------------------------*)
      (* Konstanten f?ur neu parsid rel 3.0.00    *)
      (*------------------------------------------*)
      cpr_p_precom_index     = 11;
      mxpr_knlparsid         =  12;   (* release 3.0..  *)
      mxpr_parsid            =  mxpr_knlparsid + 4;   (* release 7.1..  *)
      mxpr_errorloop   =    3; (* bei error -1,anz wiederholungen *)
      mxpr_sqlline     =   52;          (* mod 4 *)
      mxpr_macroline   =  126;
      mxpr_sqlga       =    8;  (* anzahl adabas session  *)
      mxpr_sqlma       =  128;     (* mocros    *)
      mxpr_partkind    =   28;    (* vpi00c sqlmaxpartkind *)
      mxpr_numb           =    6;
      mxpr_errtext        =   70;
      mxpr_preline        =  512;          (* mod 4 *)
      mxpr_value          =  254;
      mxpr_mfetch         =   18;
      mxpr_mfetchbuffer   =   18;
      mxpr_mfetchrecords  =  250;
      mxpr_descrlvc       = 2000;
      mxpr_sysprof_stmlen = 1950;
      mxpr_buffer         = 8488;   (* old tsp_moveobj *)
      cpr_undefindicator    = -1;
      cpr_overflowindicator = -2;
      cpr_kind_empty         =   0;
      cpr_kind_internal      =   1;
      cpr_kind_db2           =   2;
      cpr_kind_ansi          =   3;
      cpr_kind_oracle        =   4;
      cpr_kind_sapr3         =   5;
      cpr_single_byte        =   0;
      cpr_multi_sjis         =   1;
      cpr_multi_euc          =   2;
      (*------------------------------------------*)
      (* Konstanten fuer adabas_errors  und       *)
      (* Konstanten fuer adabas_messagetypen      *)
      (*------------------------------------------*)
      cpr_pointerlen            =    0;
      cpr_file_or_table_droped  = -108;
      cpr_err_cmd_not_available = -101;
      cpr_database_crash        =  800;
      (* messagetypen nicht im precompiler erlaubt *)
      (* parsid  [5]  wert *)
      cpr_m_nolog           = 36;  (* Thomas A. fragen *)
      (*------------------------------------------*)
      (* Konstanten f?ur enumeration typen        *)
      (*------------------------------------------*)
      cpr_tr_inp            = 0;    (* trace text *)
      cpr_tr_outp           = 1;
      cpr_tr_loop           = 2;
      cpr_tr_longvar        = 3;
      cpr_tr_arrcnt         = 4;
      cpr_is_empty          = -1;   (* tsp_int2 *)
      cpr_is_false          = 0;
      cpr_is_true           = 1;    (* tsp_int2 *)
      cpr_is_init           = 2;
      cpr_is_describe       = 3;
      cpr_is_loop_cnt       = 4;    (* 1.entry for loop sa-entries.sastruc*)
      cpr_is_loop           = 5;    (* next sa-enties.sastruc for loop *)
      cpr_is_first          = 6;    (* clock mess time begin           *)
      cpr_is_end            = 7;    (* clock mess time end             *)
      cpr_is_only_pc        = 8;   (* prof.time without sqldbkerneltime*)
      cpr_is_statement      = 9;   (* hostvarname is statementname *)
      cpr_vstruct           = -1;
      cpr_vint2             = 0;     (* tsp_int2 *)
      cpr_vint4             = 1;
      cpr_vreal4            = 2;
      cpr_vreal8            = 3;
      cpr_vdecimal          = 4;
      cpr_vzoned            = 5;
      cpr_vchar             = 6;
      cpr_vcharc            = 7;
      cpr_vbchar            = 8;
      cpr_vdbchar           = 9;
      cpr_vbuf              = 10;
      cpr_vscalar           = 11;
      cpr_vintarr           = 12;
      cpr_vrec              = 13;
      cpr_vnone             = 14;
      cpr_vstring           = 15;
      cpr_vuns2             = 16;
      cpr_vuns4             = 17;
      cpr_vdecrec           = 18;
      cpr_varray            = 19;
      cpr_vstring1          = 20;
      cpr_vdbcharc          = 21;
      cpr_vlzone            = 22;
      cpr_vlszon            = 23;
      cpr_vtszon            = 24;
      cpr_vnumber           = 25;   (* visual basic *)
      cpr_vbyte             = 26;   (* visual basic *)
      cpr_vdbyte            = 27;   (* visual basic *)
      cpr_vansicharc        = 28;   (* ansi char for C with blanks *)
      cpr_vreal8sun         = 29;   (* real pascal sun *)
      cpr_vstringc          = 30;
      cpr_vansichar         = 31;   (* ansi char *)
      cpr_vr3charnull       = 32;   (* R3 intern datentyp *)
      (* only in sqlda, char with len=0 *)
      cpr_vint8         = 33;
      cpr_vuns8         = 34;
      cpr_vstring4      = 35;
      cpr_viaddr        = 36;
      cpr_vraw          = 37;
      cpr_vrawc         = 38;
      cpr_vfile         = 39;   (* datafile Cobol *)
      cpr_vfilec        = 40;   (* datafile C *)
      cpr_vunicode      = 41;   (* unicode   *)
      cpr_vunicodec     = 42;   (* unicode C *)
      cpr_vcharz        = 43;
      cpr_vonumber      = 44;
      cpr_vovarnum      = 45;
      cpr_vodate        = 46;
      cpr_vabaphandle   = 47;
      cpr_vvarchar_without_len   = 48;
      cpr_vint1         = 49;
      cpr_vuns1         = 50;
      cpr_vlong_desc    = 51;
      cpr_vucs2         = 52;
      cpr_vutf16        = 53;
      cpr_vstringunicode   = 54;    (* ora-varchar  contains UCS2 *)
      cpr_vstringunicode4  = 55;    (* ora-varchar4 contains UCS2 *)
      (*------------------------------------------*)
      (* Konstanten Oracle Datentypen             *)
      (*     nur in vpr00c                        *)
      (*------------------------------------------*)
      (*------------------------------------------*)
      (* Konstanten f?ur include Dateien describe *)
      (*------------------------------------------*)
      cpr_info_input    = 0;    (* colio *)
      cpr_info_output   = 1;
      cpr_info_inout    = 2;
      (* festgelegte dataentypen des precompilers *)
      cpr_vshort   =  cpr_vint2;       (* c *)
      cpr_vunsshort=  cpr_vuns2;       (* c *)
      cpr_vlong    =  cpr_vint4;       (* c *)
      cpr_vunslong =  cpr_vuns4;       (* c *)
      cpr_llong    =  cpr_numblen_int4; (* c *)
      cpr_lunslong =  cpr_numblen_int4; (* c *)
      cpr_vint     =  cpr_vint4;       (* c *)
      cpr_vunsigned=  cpr_vuns4;       (* c *)
      cpr_lint     =  cpr_numblen_int4; (* c *)
      cpr_lunsigned=  cpr_numblen_int4; (* c *)
      cpr_vinteger =  cpr_vint4;       (* pascal *)
      cpr_linteger =  cpr_numblen_int4; (* pascal *)
      cpr_vfloat   =  cpr_vreal4;      (* c *)
      cpr_vdouble  =  cpr_vreal8;      (* c *)
      cpr_vreal    =  cpr_vreal4;      (* f77 *)
      cpr_lshort   =  cpr_numblen_int2; (* c *)
      cpr_lunsshort=  cpr_numblen_int2; (* c *)
      cpr_lfloat   =  cpr_numblen_r4;(* c *)
      cpr_ldouble  =  cpr_numblen_r8;(* c *)
      cpr_trace_empty       = 0;    (* tsp_int2 *)
      cpr_trace_off         = 1;
      cpr_trace_formal      = 2;
      cpr_trace_long        = 3;
      cpr_trace_modformal   = 4;
      cpr_trace_modlong     = 5;
      cpr_com_empty         = 0;     (* command type *)
      cpr_com_sql           = 1;
      cpr_com_sql_conn      = 2;     (* sql connect statement *)
      cpr_com_cancel        = 3;     (* sql cancel  statement *)
      cpr_com_option        = 4;
      cpr_com_sql_close     = 5;
      cpr_com_crea_ind      = 6;
      cpr_com_sql_getval    = 7;     (* exex sql getval command *)
      cpr_com_sql_open      = 8;     (* c-i exec sql open statement *)
      cpr_com_commit_release = 9;    (* call interface *)
      cpr_com_describe      = 10;    (* call interface *)
      cpr_com_fetch_describe= 11;    (* call interface *)
      cpr_com_mfetch        = 12;    (* call interface *)
      cpr_com_mfetch_describe=13;    (* call interface *)
      cpr_com_exec          = 14;    (* intermediate code (of pass1) *)
      cpr_com_cics          = 15;    (* cics command found           *)
      cpr_com_command       = 16;    (* exec command                 *)
      cpr_com_unused        = 17;
      cpr_com_proc          = 18;    (* exec proc                    *)
      cpr_com_opt           = 19;    (* command line options *)
      cpr_com_sql_putval    = 20;    (* exec sql putval command *)
      cpr_com_set_format    = 21;    (* set format rel3.0  *)
      cpr_com_declare_cursor= 22;    (* declare cursor for ?? as param*)
      cpr_com_sql_fetch     = 23;    (* exec sql fetch command        *)
      cpr_com_ora_descr_sel = 24;    (* oracle desc sel.list *)
      cpr_com_oracle        = 25;    (* exec oracle          *)
      cpr_com_trace         = 26;
      cpr_com_sql_open_keep = 27;    (* exec sql open stmt. with keep opt. *)
      cpr_com_commit        = 28;
      cpr_com_rollback      = 29;
      cpr_com_rollback_release = 30;
      cpr_com_abort_session = 31;
      cpr_in_sqlva          = 0;     (* tsp_int2  *)
      cpr_in_sqlst          = 1;
      cpr_state_empty       = 0;     (* commandostate *)
      cpr_state_parse       = 1;
      cpr_state_execute     = 2;
      cpr_state_command     = 3;
      cpr_state_prepare     = 4;
      cpr_state_macro       = 5;
      cpr_state_trace       = 6;
      cpr_state_sync_exec   = 7;
      cpr_state_newsync_exec= 8;
      cpr_state_async_exec  = 9;
      cpr_state_decl_statem =10;
      cpr_state_decl_with_hold =11;
      cpr_state_execute_at_fetch  = 12;
      cpr_ci_implicit       = 0;     (* connectype    *)
      cpr_ci_program        = 1;
      cpr_ci_connect        = 2;
      cpr_ci_ora_connect    = 3;
      cpr_ci_explicit       = 4;
      cpr_co_ok                     = 0;    (* tsp_int2 *)
      cpr_co_dbms_start_required    = 1;
      cpr_co_too_many_users         = 2;
      cpr_co_restart_required       = 3;
      cpr_co_logon_required         = 4;
      cpr_co_user_or_pswd_illegal   = 5;
      cpr_co_crash                  = 6;
      cpr_co_timeout                = 7;
      cpr_co_implicit               = 8;
      cpr_co_user_op                = 9;

      cpr_se_empty          = 0;    (* tsp_int2 *)
      cpr_se_primary        = 1;
      cpr_se_second         = 2;
      CPR_CMD_STATE_INIT     = 0;
      CPR_CMD_STATE_START    = 1;
      CPR_CMD_STATE_READY    = 2;
      CPR_CMD_STATE_INFLIGHT = 3;
      CPR_CMD_STATE_END      = 4;
      cpr_fe_empty          = 0;    (* no fetch stm *)
      cpr_fe_fetch          = 1;    (* normal fetch *)
      cpr_fe_first          = 2;    (* fetch first  *)
      cpr_fe_next           = 3;    (* fetch next   *)
      cpr_fe_last           = 4;    (* fetch last   *)
      cpr_fe_prev           = 5;    (* fetch prev   *)
      cpr_fe_same           = 6;    (* fetch same   *)
      cpr_fe_posc           = 7;    (* fetch pos const*)
      cpr_fe_posv           = 8;    (* fetch pos variable *)
      cpr_fe_relc           = 9;    (* fetch relative const*)
      cpr_fe_relv           = 10;   (* fetch relative variable *)
      cpr_lo_iso_default    = -1;   (* option default *)
      cpr_lo_empty          = -1;   (* tsp_int2 empty *)
      cpr_lo_isolev0        = 0;    (* isolation level *)
      cpr_lo_isolev1        = 1;
      cpr_lo_isolev2        = 2;
      cpr_lo_isolev3        = 3;        (* ansi default *)
      cpr_lo_isolev4        = 4;        (* alt  *)
      cpr_lo_isolev9        = 9;
      cpr_lo_isolev10       = 10;       (* normal, sql-db default *)
      cpr_lo_isolev15       = 15;       (* implicit *)
      cpr_lo_isolev20       = 20;       (* explicit *)
      cpr_lo_isolev30       = 30;
      cpr_dt_empty          = 0;  (* sqldb date time format *)
      cpr_dt_normal         = 1;
      cpr_dt_iso            = 2;
      cpr_dt_usa            = 3;
      cpr_dt_eur            = 4;
      cpr_dt_jis            = 5;
      cpr_dt_ansi           = 6;
      cpr_lzu_connect   = 1;     (* fehler analyse von sql_result *)
      cpr_lzu_request   = 2;
      cpr_lzu_receive   = 3;
      cpr_lzu_release   = 4;
      cpr_lzu_sysprofile = 5;
      cpr_lzu_cancel     = 6;
      (*----------------------------------------------------------*)
      (*  errors f?ur call_interface                              *)
      (*----------------------------------------------------------*)
      cpr_c_ok           = 0;
      cpr_c_incompatible = 1;
      cpr_c_overflow     = 2;
      cpr_c_truncated    = 3;
      cpr_c_varnowrong   = 4;
      cpr_c_notimplemented=5;
      cpr_la_all     = 0;  (* all pascal runtime *)
      cpr_la_call    = 1;
      cpr_la_c       = 3;
      cpr_la_cobol   = 4;
      cpr_la_odbc    = 5;
      cpr_la_oci     = 6;
      cpr_la_cobmic  = 7;
      cpr_la_cob8860   = 8;
      cpr_la_pascalvax = 9;
      cpr_la_pascal    = 10;
      cpr_la_pascallpi = 11;
      cpr_la_pascal31  = 12;
      (* Konstanten fuer CPC-Code-Erzeugung *)
      (* sqlva1-area -entry's *)
      sqlvasc = 0;    (* scalar *)
      sqlvacm = 1;    (* componente *)
      sqlvast = 2;    (* struct *)
      sqlvapt = 3;    (* pointer *)
      (* sqlpar-area -entry's *)
      sqlparlo   = 0;   (* loop *)
      sqlparst   = 1;   (* struct *)
      sqlparel   = 2;   (* entry *)
      CPR_ERROR_ABAP_DESC_NOT_SET = -861;
      CPR_ERROR_ABAP_EXCEPTION    = -862;
      CPR_ERROR_CONNECTION_BUSY   = -810;
      cpr_error_session_not_connected   = -821;
      cpr_error_invalid_option    =-863;
      Version61_cpr00 = 60100;
      Version62_cpr00 = 60200;
      Version71_cpr00 = 70100;
      Version714cpr00 = 70104;
      Version72_cpr00 = 70200;
      Version721_cpr00 = 70201;
      Version722_cpr00 = 70202;
      Version724_cpr00 = 70204;
      Version725_cpr00 = 70205;
      Version730_cpr00 = 70300;
      Version731_cpr00 = 70301;
      Version740_cpr00 = 70400;
      Version741_cpr00 = 70401;
      Version742_cpr00 = 70402;
      (* Kernel Versions *)
      ActualKernelVersion_cpr00 = Version742_cpr00;
      MinKernelVersion_cpr00    = Version62_cpr00;
      SelDirFast_KernelVersion_cpr00 = Version72_cpr00;
      MaxLenOnBlob_KernelVersion_cpr00 = Version722_cpr00;
      (* CPC Runtime Versions *)
      Actual_CPCVersion_cpr00  = Version742_cpr00;
      Min_CPCVersion_cpr00     = Version731_cpr00;
      BlobSupport_CPCVersion_cpr00 = Version722_cpr00;
      (* tpr00_DescribeKindEnum *)
      Describe_Columns_epr00 = 1;
      Describe_Params_epr00 = 2;
      Describe_Both_epr00 = 3;


      cpc_i_abort       =    153  ;
      cpc_i_adabas      =    1    ;
      cpc_i_alter       =    2    ;
      cpc_i_any         =    3    ;
      cpc_i_array       =    4    ;
      cpc_i_as          =    5    ;
      cpc_i_async       =    6    ;
      cpc_i_at          =    7    ;
      cpc_i_begin       =    8    ;
      cpc_i_bind        =    9    ;
      cpc_i_bnddsc      =   10    ;
      cpc_i_call        =   11    ;
      cpc_i_cancel      =   89    ;
      cpc_i_char        =   12    ;
      cpc_i_character   =   13    ;
      cpc_i_charz       =   14    ;
      cpc_i_cics        =   15    ;
      cpc_i_close       =   16    ;
      cpc_i_column      =   17    ;
      cpc_i_command     =   18    ;
      cpc_i_connect     =   19    ;
      cpc_i_const       =   20    ;
      cpc_i_continue    =   21    ;
      cpc_i_conversation=   22    ;
      cpc_i_create      =   23    ;
      cpc_i_current     =  124    ;
      cpc_i_cursor      =   24    ;
      cpc_i_database    =   25    ;
      cpc_i_datafile    =   26    ;
      cpc_i_date        =   27    ;
      cpc_i_dbname      =   28    ;
      cpc_i_dbproc      =   29    ;
      cpc_i_decimal     =   30    ;
      cpc_i_declare     =   31    ;
      cpc_i_describe    =   32    ;
      cpc_i_descriptor  =   33    ;
      cpc_i_display     =   34    ;
      cpc_i_do          =   35    ;
      cpc_i_double      =   36    ;
      cpc_i_end         =   37    ;
      cpc_i_exec        =   38    ;
      cpc_i_execute     =   39    ;
      cpc_i_false       =   40    ;
      cpc_i_fetch       =   41    ;
      cpc_i_first       =   42    ;
      cpc_i_float       =   43    ;
      cpc_i_for         =   44    ;
      cpc_i_found       =   46    ;
      cpc_i_from        =   47    ;
      cpc_i_get         =  152    ;
      cpc_i_getval      =  147    ;
      cpc_i_go          =   48    ;
      cpc_i_goto        =   49    ;
      cpc_i_hold        =   84    ;
      cpc_i_immediate   =   50    ;
      cpc_i_include     =   51    ;
      cpc_i_ind         =   52    ;
      cpc_i_index       =   53    ;
      cpc_i_indicator   =   54    ;
      cpc_i_integer     =   55    ;
      cpc_i_internal    =   142   ;
      cpc_i_into        =   56    ;
      cpc_i_is          =   57    ;
      cpc_i_keep        =  149    ;
      cpc_i_keeputm     =   58    ;
      cpc_i_labels      =   59    ;
      cpc_i_last        =   145   ;
      cpc_i_line        =   60    ;
      cpc_i_link        =   61    ;
      cpc_i_list        =   62    ;
      cpc_i_long        =   63    ;
      cpc_i_longint     =   64    ;
      cpc_i_macro       =   65    ;
      cpc_i_mlslabel    =   66    ;
      cpc_i_names       =   67    ;
      cpc_i_newsync     =   68    ;
      cpc_i_next        =   143   ;
      cpc_i_nolog       =   69    ;
      cpc_i_not         =   70    ;
      cpc_i_notfound    =   71    ;
      cpc_i_number      =   72    ;
      cpc_i_of          =   73    ;
      cpc_i_off         =   74    ;
      cpc_i_on          =   75    ;
      cpc_i_open        =   76    ;
      cpc_i_option      =   146   ;
      cpc_i_oraca       =   77    ;
      cpc_i_oracle      =   78    ;
      cpc_i_packed      =   79    ;
      cpc_i_perform     =   80    ;
      cpc_i_pos         =   81    ;
      cpc_i_prepare     =   82    ;
      cpc_i_prev        =   144   ;
      cpc_i_printer     =   83    ;
      cpc_i_putval      =   148   ;
      cpc_i_raw         =   85    ;
      cpc_i_real        =   86    ;
      cpc_i_reconnect   =   87    ;
      cpc_i_record      =   88    ;
      cpc_i_result      =   90    ;
      cpc_i_return      =   91    ;
      cpc_i_rowid       =   92    ;
      cpc_i_section     =   93    ;
      cpc_i_seldsc      =   94    ;
      cpc_i_select      =   95    ;
      cpc_i_serverdb    =   96    ;
      cpc_i_session     =   97    ;
      cpc_i_set         =   98    ;
      cpc_i_shortint    =   99    ;
      cpc_i_shortreal   =  100    ;
      cpc_i_sql         =  101    ;
      cpc_i_sqlbegin    =  102    ;
      cpc_i_sqlca       =  103    ;
      cpc_i_sqlconst    =  104    ;
      cpc_i_sqlda       =  105    ;
      cpc_i_sqldb       =  106    ;
      cpc_i_sqldec      =  107    ;
      cpc_i_sqlerror    =  108    ;
      cpc_i_sqlend      =  109    ;
      cpc_i_sqlexception=  110    ;
      cpc_i_sqlproc     =  112    ;
      cpc_i_sqltype     =  113    ;
      cpc_i_sqlucs2     =  150    ;
      cpc_i_sqlutf16    =  151    ;
      cpc_i_sqlwarning  =  114    ;
      cpc_i_sql_trace   =  115    ;
      cpc_i_start       =  116    ;
      cpc_i_statement   =  117    ;
      cpc_i_stop        =  118    ;
      cpc_i_string      =  119    ;
      cpc_i_struct      =  120    ;
      cpc_i_sync        =  121    ;
      cpc_i_table       =  122    ;
      cpc_i_to          =  123    ;
      cpc_i_tptst       =  125    ;
      cpc_i_trace       =  126    ;
      cpc_i_transid     =  127    ;
      cpc_i_true        =  128    ;
      cpc_i_type        =  129    ;
      cpc_i_unique      =  130    ;
      cpc_i_unsigned    =  131    ;
      cpc_i_using       =  132    ;
      cpc_i_var         =  133    ;
      cpc_i_varchar     =  134    ;
      cpc_i_varchar2    =  135    ;
      cpc_i_variables   =  136    ;
      cpc_i_varnum      =  137    ;
      cpc_i_varraw      =  138    ;
      cpc_i_version     =  111    ;
      cpc_i_when        =  139    ;
      cpc_i_whenever    =  140    ;
      cpc_i_with        =   45    ;
      cpc_i_xctl        =  141    ;
      mxpc_keyword    = 153;



     (*****************************************)
     (* from vpc00c   cpcCconst  *)
        cpc_cconst = 8;

 (* 2. C  tokens*)

        cpc_cs_leftbrc               =  25;
        cpc_cs_rightbrc              =  26;
        cpc_cs_prepcom               =  27;
        cpc_cs_unknown               = cpr_s_unknown;
        cpc_cs_identifier            = cpr_s_identifier;
        cpc_cs_parameter             = cpr_s_parameter_name;
        cpc_cs_unsigned              = cpr_s_unsigned_integer;
        cpc_cs_char                  = cpr_s_string_literal;
        cpc_cs_string                = cpr_s_cstring_literal;
        cpc_cs_comma                 = cpr_s_comma;
        cpc_cs_period                = cpr_s_point;
        cpc_cs_equal                 = cpr_s_equal;
        cpc_cs_leftpar               = cpr_s_leftpar;
        cpc_cs_rightpar              = cpr_s_rightpar;
        cpc_cs_leftbrk               = cpr_s_leftindpar;
        cpc_cs_rightbrk              = cpr_s_rightindpar;
        cpc_cs_asterisk              = cpr_s_asterisk;
        cpc_cs_colon                 = cpr_s_colon;
        cpc_cs_macro                 = cpr_s_macro;
        cpc_cs_minus                 = cpr_s_minus;
        cpc_cs_divide                = cpr_s_divide;
        cpc_cs_greater               = cpr_s_greater;
        cpc_cs_eof                   = cpr_s_eof;
        cpc_cs_bufeof                = cpr_s_bufeof;

 (* COBOL tokens*)

        cpc_cbs_percent              = cpr_s_macro;
        cpc_cbs_unknown              = cpr_s_unknown;
        cpc_cbs_identifier           = cpr_s_identifier;
        cpc_cbs_parameter            = cpr_s_parameter_name;
        cpc_cbs_unsigned             = cpr_s_unsigned_integer;
        cpc_cbs_string               = cpr_s_string_literal;
        cpc_cbs_comma                = cpr_s_comma;
        cpc_cbs_period               = cpr_s_point;
        cpc_cbs_semicolon            = cpr_s_semicolon;
        cpc_cbs_equal                = cpr_s_equal;
        cpc_cbs_plus                 = cpr_s_plus;
        cpc_cbs_minus                = cpr_s_minus;
        cpc_cbs_leftpar              = cpr_s_leftpar;
        cpc_cbs_rightpar             = cpr_s_rightpar;
        cpc_cbs_leftbrk              = cpr_s_leftindpar;
        cpc_cbs_rightbrk             = cpr_s_rightindpar;
        cpc_cbs_colon                = cpr_s_colon;
        cpc_cbs_divide               = cpr_s_divide;
        cpc_cbs_eof                  = cpr_s_eof;
        cpc_cbs_bufeof               = cpr_s_bufeof;

 (* p11getchar input status *)

        cpc_ch_ok  =  0;
        cpc_ch_eof = -1; (* logical end of file reached,*)
                         (* no more characters read *)
        cpc_ch_eol = -2; (* end of line reached,*)
                     (* a new line is input and a new character read *)
        cpc_ch_eob = -3; (* during repeated scan of the same buffer *)
                      (* buflen is reached (the 2nd, 3rd, ... time) *)

    (* ----------------------------------------------- *)

              (* werte fuer den typen tpc_cob_section *)
              cpc_cob_none = 0;  (* initialzustand                   *)
              cpc_cob_file = 1;  (* FILE SECTION gefunden            *)
              cpc_cob_work = 2;  (* WORKING-STORAGE SECTION gefunden *)
              cpc_cob_link = 3;  (* LINKAGE SECTION gefunden         *)
              cpc_cob_proc = 4;  (* PRODEDURE DIVISION gefunden      *)
              cpc_cob_usin = 5;  (* innerhalb USING der PROCEDURE DIV*)
              cpc_cob_decl = 6;  (* DECLARATIVES gefunden            *)
              cpc_cob_decen= 7;  (* DECLRATIVES Ende                 *)
              cpc_cob_init = 8;  (* SQLINIT SECTION einfuegen        *)
              cpc_cob_scre = 9;  (* SCREEN SECTION gefunden          *)
              cpc_cob_p_id =10;  (* PROGRAM-ID gefunden              *)
              cpc_f_goback =11;  (* GOBACK     found                 *)
              cpc_cob_insp =12;  (* INSPECT found (runtime args)     *)
              cpc_cob_argl =13;  (* MOVE TO SQLARGL found            *)


      (*------------------------------------------*)
      (* Konstanten f?ur enumeration typen        *)
      (*------------------------------------------*)

      cpc_as_var            = 0;     (* symbolic values for asclause in *)
      cpc_as_type           = 1;     (* record dclgen *)
      cpc_as_struct         = 2;
      cpc_as_record         = 3;     (* nur fortran *)
      cpc_as_empty          =99;

      cpc_oc_empty          = 0;    (* c++ optoin *)
      cpc_oc_cansi          = 1;
      cpc_oc_cplus          = 2;

      cpc_imm_no            = 0;    (* immediate  *)
      cpc_imm_variable      = 1;
      cpc_imm_string        = 2;

      cpc_notp                    = 0;  (* Werte fuer optpmon *)
      cpc_pseudoconvers           = 1;
      cpc_convers                 = 2;
      cpc_aim                     = 3;
      cpc_utm                     = 4;
      cpc_utmconvers              = 5;

      cpc_static_link             = 0;  (* Werte fuer opdyn *)
      cpc_dynamic_link            = 1;

      (*----------------------------------------------------------*)
      (*  constanten der precompiler                              *)
      (*----------------------------------------------------------*)
      mxpc_const_sec     = 100;
      mxpc_vffiles       = 8;

      mxpc_varent        = 1000;
      mxpc_typent        = 1000;
      mxpc_ndarray       =    4;     (* indices pro hostvariable *)
      mxpc_sqlnd         =  100;
      mxpc_sqlva1        = mxpr_sqlva1;     (* host-variables addr *)
      mxpc_sqlva2        = mxpr_sqlva2;     (* variable datatypes *)
      mxpc_sqlva3        = mxpr_sqlva3;     (* variable names *)

      cpc_pclistfile      = 1;
      cpc_inputfile       = 2;
      cpc_work1file       = 3;
      cpc_work2file       = 4;
      cpc_work3file       = 5;
      cpc_lnoinput        = 6;
      cpc_lnooutput       = 7;
      cpc_includefile     = 8;

      (* =================================== *)

      cpc_se_off     = 0;       (* tsp_int2*)
      cpc_se_const   = 1;
      cpc_se_type    = 2;
      cpc_se_var     = 3;
      cpc_se_record  = 4;
      cpc_se_declare = 5;
      cpc_se_structure = 6;
      (* =================================== *)
      cpc_ch_no      = 0;       (* tsp_int2    *)
      cpc_ch_syntax  = 1;
      cpc_ch_all     = 2;
      (* =================================== *)
      cpc_usage_empty = 0;       (* usage add    *)
      cpc_usage_comm  = 1;
      cpc_usage_prep  = 2;
      cpc_usage_cancel= 3;
      (* =================================== *)


 (*  constants for runtime interface code generation *)

  cpc_c_sqllatab          =  1;
  cpc_c_prtscall          =  1;
  cpc_c_fircon            =  1;
  cpc_c_conv              =  1;
  cpc_c_tptstcode         =  1;
  cpc_c_loadl             =  1;
  cpc_c_savel             =  1;
  cpc_c_keeputm           =  1;
  cpc_c_dosal             =  1;
  cpc_c_deletl            =  1;
  cpc_c_allocca           =  1;
  cpc_c_rlsrc             =  1;
  cpc_c_ca_end            =  1;

 cpc_c_g_sqllatab        =  20;
 cpc_c_g_sqlcheck        =  4;
 cpc_c_g_sqladext        =  4;
 cpc_c_g_sqladpa         =  4;
 cpc_c_g_sql2adpa        =  4;
 cpc_c_g_sqladkast       =  14;
 cpc_c_g_perfutm         =  4;
 cpc_c_g_sqlprts         =  4;
 cpc_c_g_tptst           =  4;
 cpc_c_g_inheader        =  10;
 cpc_c_g_inlowval        =  15;
 cpc_c_g_incaenv         =  4;
 cpc_c_g_invarinit       =  10;
 cpc_c_g_intail          =  10;
 cpc_c_g_loadl           =  4;
 cpc_c_g_endloadl        =  4;
 cpc_c_g_savel           =  4;
 cpc_c_g_dosal           =  4;
 cpc_c_g_rlsrc           =  4;
 cpc_c_g_ca_end          =  4;
 cpc_c_g_ca_do           =  4;
 cpc_c_g_deletl          =  4;
 cpc_c_g_ltheader        =  10;
 cpc_c_g_ltsqlext        =  10;
 cpc_c_g_ltsqlpa         =  10;
 cpc_c_g_lt2sqlpa        =  10;
 cpc_c_g_ltkastate       =  15;
 cpc_c_g_lttail          =  10;
 cpc_c_g_hpgt            =  24;


 (*-------------------------------------------- *)

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


      tsp4_argline         = tsp_c132;
      tsp4_xuserkey        = tsp_c18;
      tsp4_sqlmode_name    = tsp_c8;
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
            opt_filler1    : tsp_int2;
            opt_filler2    : boolean;
            opt_component  : tsp4_component;
            CASE tsp4_component OF
                sp4co_sql_precompiler :
                    (opt_modulefn     : tsp_vfilename;
                    opt_tracefile     : tsp_vfilename;
                    opt_prognamel     : tsp_int2;
                    opt_progname      : tsp_knl_identifier;
                    opt_modulenamel   : tsp_int2;
                    opt_modulename    : tsp_knl_identifier;
                    opt_tpmonidl      : tsp_int2;
                    opt_tpmonid       : tsp_name;
                    opt_begmar        : tsp_int2;
                    opt_endmar        : tsp_int2;
                    opt_trace         : tsp_int2;
                    opt_fnind         : tsp_int2;
                    opt_datetime      : tsp_int2;
                    opt_dyn           : tsp_int2;
                    opt_tpmon         : tsp_int2;
                    opt_mode          : tsp_int2;
                    opt_extern        : tsp_int2;
                    opt_cansi         : tsp_int2;
                    opt_comp          : tsp_int2;
                    opt_prof          : tsp_int2;
                    opt_packetsize    : tsp_int4;
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
                    opt_quo           : tsp_c1;
                    opt_point         : tsp_c1;
                    opt_puser_args    : tsp4_argline);
                sp4co_sql_pc_runtime:
                    (opt_rtracefile   : tsp_vfilename;
                    opt_rtrace        : tsp_int2;
                    opt_rprof         : tsp_int2;
                    opt_rmfetch       : tsp_int2;
                    opt_rtime_limit   : tsp_int2;
                    opt_rstmt_cnt     : tsp_int2;
                    opt_rfiller       : tsp_int2;
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
                    (opt_runfile      : tsp_vfilename;
                    opt_object_owner  : tsp_name;
                    opt_object_name1  : tsp_name;
                    opt_object_name2  : tsp_name;
                    opt_parameter     : tsp_line;
                    opt_append        : boolean;
                    opt_prompt        : boolean;
                    opt_codeonly      : boolean;
                    opt_utility_session : boolean;
                    opt_comm_mode     : tsp4_command_mode;
                    opt_code          : tsp_code_type);
                sp4co_sql_audit:
                    (opt_aud_control  : tsp_vfilename;
                    opt_aud_logfile   : tsp_vfilename;
                    opt_aud_log_blocksize    : tsp_int4 ;
                    opt_aud_output    : tsp_vfilename;
                    opt_aud_comm_mode : tsp4_command_mode);
                sp4co_sql_userx:
                    (opt_ux_runfile   : tsp_vfilename;
                    opt_ux_comm_mode  : tsp4_command_mode );
                END;
            (*ENDCASE*)


      tsp4_xuser_record    = RECORD
            xu_key         : tsp4_xuserkey;
            xu_fill        : tsp_int2;
            xu_servernode  : tsp_nodeid;
            xu_serverdb    : tsp_dbname;
            xu_user_61     : tsp_c18;
            xu_password    : tsp_cryptpw;
            xu_sqlmode     : tsp4_sqlmode_name;
            xu_cachelimit  : tsp_int4;
            xu_timeout     : tsp_int2;
            xu_isolation   : tsp_int2;
            xu_dblang_61   : tsp_c18;
            xu_user        : tsp_knl_identifier;
            xu_dblang      : tsp_knl_identifier;
            xu_userUCS2    : tsp_knl_identifier;
            xu_passwordUCS2: tsp_cryptpw;
      END;


      tsp5_rte_filewarning = (
            sp5vfw_no_warning,
            sp5vfw_no_eol_found,
            sp5vfw_no_next_tape);

      tsp5_rte_fileerror = RECORD
            sp5fe_result       : tsp_vf_return;
            sp5fe_warning      : tsp5_rte_filewarning;
            filler             : tsp_c2;
            sp5fe_text         : tsp_c40;
      END;

      tsp5_rte_datakind = (
            sp5vf_text,
            sp5vf_binary);
      tsp5_rte_filemode = (
            sp5vf_read,
            sp5vf_write,
            sp5vf_readwrite,
            sp5vf_append);
      tsp5_rte_buffering_kind = (
            sp5bk_buffered,
            sp5bk_unbuffered,
            sp5bk_disksynced);
      tsp5_rte_closeoption = (
            sp5vf_close_normal,
            sp5vf_close_rewind,
            sp5vf_close_next_tape);
      tsp5_rte_seekkind = (
            sp5vf_seek_begin,
            sp5vf_seek_cur,
            sp5vf_seek_end);

      tsp5_rte_fileinfo = RECORD
            sp5fi_exists        : boolean;
            sp5fi_readable      : boolean;
            sp5fi_writeable     : boolean;
            sp5fi_is_link       : boolean;
            sp5fi_size          : tsp_longint;
            sp5fi_size_x        : tsp_longint;
            sp5fi_date_modified : tsp_date;
            sp5fi_time_modified : tsp_time;
            sp5fi_media_kind    : tsp_vf_type;
      END;


      tpr_intaddr = tsp_int4;
      tpr_longint = tsp_int4;
      tpr_cmd_state_Enum = integer;
      tpr_parsid      = PACKED ARRAY [ 1..mxpr_parsid ]  OF char;
      tpr00_ParseId = tpr_parsid;
      tpr00_sp1rFunctionCode = tsp00_Int2;

      tpr00_ParseInfo = RECORD
            ParseId          : tpr00_ParseId;
            sp1rFunctionCode : tpr00_sp1rFunctionCode;
      END;

      tpr00_KnlSessionID = PACKED ARRAY [ 1..4 ]  OF char;

      tpr00_SessionID = RECORD
            KnlSession : tpr00_KnlSessionID; (* Session no from Kernel *)
            ClientSession : tsp00_Int4;      (* unique connect id on client *)
      END;

      tpr_password   = tsp_name;
      tpr_sqlline    = PACKED ARRAY [ 1..mxpr_sqlline ]  OF char;
      tpr_macroline  = PACKED ARRAY [ 1..mxpr_macroline] OF char;
      tpr_hostname   = tsp_c64;
      (**   tpr_buffer     = PACKED ARRAY [ 1..mxpr_buffer] OF char; **)
      tpr_buffer     = tsp_moveobj;
      tpr_stm_bufferp= ^tsp_moveobj;
      tpr_vvalue  = PACKED ARRAY [ 1..mxpr_value ] OF char;
      tpr_symbol  =    tsp_int2;

      tpr_scannertype = RECORD    (* 16 B *)
            syposacc : tsp_int4;  (* aktuelle pos ab der analys. wird *)
            sypos    : tsp_int4;  (* tpr_symbol position *)
            sylen    : tsp_int4;  (* tpr_symbol l?ange    *)
            symb     : tpr_symbol; (* erkanntes tpr_symbol *)
            syfill   : tsp_int2;    (* :true analyse ist innerhalb einer *)
            (* nur pascal stringkonstanten *)
      END;

      tpr_runtime_errors = (
            cpr_p_ok,
            cpr_area_overflow ,               (* -761 *)
            cpr_cmd_not_available,            (* -101 *)
            cpr_cmd_too_long,                 (* -706 *)
            cpr_could_not_load_lib,           (* -888 *)
            cpr_connect_syntax_wrong,         (* -707 *)
            cpr_conv_from_unicode_impossible, (* -777 *)
            cpr_conv_to_unicode_impossible,   (* -778 *)
            cpr_no_long_columns_found,        (* -779 *)
            cpr_datafile_close_error,         (* -776 *)
            cpr_datafile_open_error,          (* -773 *)
            cpr_datafile_read_error,          (* -775 *)
            cpr_datafile_write_error,         (* -774 *)
            cpr_db_session_not_allowed,       (* -806 *)
            cpr_differ_sqlmode_not_allowed,   (* -756 *)
            cpr_duplicate_fetch,              (* -703 *)
            cpr_duplicate_fetch_callif,       (* -703 *)
            cpr_errorno_from_sysprofile,      (* -720 *)
            cpr_ffetch_fetch_mix_not_allowed, (* -750 *)
            cpr_ffetch_not_allowed,           (* -749 *)
            cpr_implicit_connect_wrong,       (* -711 *)
            cpr_incompatible_datatype,        (* -817 *)
            cpr_inp_null_not_allowed,         (* -801 *)
            cpr_inp_number_invalid,           (* -802 *)
            cpr_inp_number_overflow,          (* -751 *)
            cpr_inp_number_truncated,         (* w1   *)
            cpr_inp_string_truncated,         (* -743 *)
            cpr_inp_string_truncated_oracle,  (* w1   *)
            cpr_invalid_commandstate,         (*-9801 *)
            cpr_invalid_declare_command,      (* -820 *)
            cpr_invalid_escape,               (* -768 *)
            cpr_invalid_program,              (* -805 *)
            cpr_macro_not_init,               (* -818 *)
            cpr_memory_allocation_faild,      (* -760 *)
            cpr_missing_previous_fetch,       (*-7026 *)
            cpr_missing_user_passw,           (* -715 *)
            cpr_missing_variable_addr,        (* -702 *)
            cpr_multibyteset_not_allowed,     (* -772 *)
            cpr_no_session_connected,         (* -807 *)
            cpr_not_implemented,              (* -808 *)
            cpr_out_null_not_allowed,         (* -809 *)
            cpr_out_number_invalid,           (*-9806 *)
            cpr_out_number_overflow,          (* -811 *)
            cpr_output_part_too_short,        (*  w3  *)
            cpr_paramlist_not_allowed,        (* -819 *)
            cpr_paramlist_too_long,           (* -804 *)
            cpr_paramlist_too_short,          (* -812 *)
            cpr_paramlist_wrong,              (* -883 *)
            cpr_r_loop_init_wrong,            (* -856 *)
            cpr_r_sql_cancelled,              (* -102 *)
            cpr_r_too_many_statements,        (* -868 *)
            cpr_reflex_connect_notok,         (* -709 *)
            cpr_reflex_crash,                 (* +800 *)
            cpr_reflex_notok,                 (* -807 *)
            cpr_reflex_shutdown,              (* -708 *)
            cpr_reflex_start_required,        (* -813 *)
            cpr_reflex_timeout,               (* +700 *)
            cpr_request_area_overflow,        (* -706 *)
            cpr_rmissing_indicatorvariable,   (* -884 *)
            cpr_rt_no_output_variable,        (* -753 *)
            cpr_select_parsid_missing,        (* -710 *)
            cpr_session_already_connected,    (* -752 *)
            cpr_set_format_error,             (*-9808 *)
            cpr_too_many_fetch_statements,    (* -705 *)
            cpr_too_many_hostvar,             (* -893 *)
            cpr_too_many_hostvar_callif,      (* -712 *)
            cpr_too_many_users,               (*-9807 *)
            cpr_tracefile_close_error,        (* -814 *)
            cpr_tracefile_open_error,         (* -815 *)
            cpr_tracefile_write_error,        (* -816 *)
            cpr_trace_error,                  (* -803 *)
            cpr_unknown_datatype,             (*-9803 *)
            cpr_unknown_statement_name,       (* -880 *)
            cpr_unknown_sysprofile,           (* -719 *)
            cpr_unterminated_c_string,        (* -767 *)
            cpr_use_call_stm,                 (* -744 *)
            cpr_valbuf_too_short,             (* -747 *)
            cpr_xuser_file_open_error,        (* -757 *)
            cpr_abap_desc_not_set,            (* -861 *)
            cpr_abap_exception,               (* -862 *)
            cpr_connection_busy,              (* -810 *)
            cpr_session_not_connected,        (* -821 *)
            cpr_invalid_option,               (* -863 *)
            cpr_unknown_cursor_name           (* -874 *)
            );
      (*******************************************)
      (* Descriptor for dynamic allocated memory *)
      (*   neu 10.5.94  descriptor zur Area-     *)
      (*  Behandlung  (siehe vpr03c,p03dynalloc) *)
      (*******************************************)

      tpr_sqlmdesc  = RECORD        (* 20 B *)
            descMaxelem : tsp_int4;      (* number of allocated elements *)
            descNelem   : tsp_int4;      (* number of used elements *)
            descElemSize: tsp_int4;      (* element size in bytes *)
            descIncr    : tsp_int4;      (* no. of elem. alloc. in one step *)
            descElemPtr : tpr_intaddr;   (* pointer to first element *)
      END;

      (******************************************)
      (*        V A L U E R E C                 *)
      (*                                        *)
      (* Variant-Record, der zum Austausch der  *)
      (* Parameterwerte zwischen Hostprogramm   *)
      (* und der Pascal-Subroutine des Precom-  *)
      (* piler-Programms dient, in der SQLDB-    *)
      (* Auftraege erzeugt und abgeschickt      *)
      (* werden.                                *)
      (* Austausch erfolgt ueber Adresse der    *)
      (* Hostvariablen                          *)
      (******************************************)
      tpr_vvvtype     =  cpr_vint2 .. cpr_viaddr;

      tpr_valuerec    = RECORD   (* 8488 B *)
            CASE tpr_vvvtype OF
                cpr_vint2 :
                    (i2 : tsp_int2);
                cpr_vint4 :
                    (i4 : tsp_int4);
                cpr_vreal4 :
                    (r4 : tsp_shortreal);
                cpr_vreal8 :
                    (r8 : tsp_longreal);
                cpr_vreal8sun :
                    (r8s : tsp_longreal);
                cpr_vdecimal :
                    (dec : tsp_decimal);
                cpr_vzoned, cpr_vlzone, cpr_vlszon, cpr_vtszon :
                    (zon : tsp_zoned);
                cpr_vchar, cpr_vbchar, cpr_vdbchar, cpr_vcharc :
                    (ch : tpr_buffer);
                cpr_vbuf  :
                    (buf : tpr_buffer);
                cpr_viaddr  :
                    (iaddr : tpr_intaddr);
                END;
            (*ENDCASE*)

      tpr_ptr_to_value = ^tpr_valuerec;

      tpr_vtypepointer = RECORD
            CASE boolean OF
                true :
                    (intaddr : tpr_intaddr);
                false :
                    (vtypep  : tpr_ptr_to_value);
                END;
            (*ENDCASE*)
            (**************************************)
            (*        S Q L D A                   *)
            (* Descriptor    Area                 *)
            (* Datentype f?ur describe Kommandos   *)
            (* Variablen ?uber diesen type werden  *)
            (* declariert und dynamisch belegt.   *)
            (**************************************)


      tpr_sqlrow = RECORD     (* 8 B *)
            ireclen : tsp_int4;
            oreclen : tsp_int4;
      END;


      tpr_sqlcol = RECORD     (* 36 B *)
            coltype  : tsp_data_type;
            colmode  : tsp1_param_opt_set;
            colio    : tsp1_param_io_type;
            colfrac  : tsp_int1;
            collen   : tsp_int4;
            colpos   : tsp_int4;
            coliolen : tsp_int2;
            colisnum : boolean;
            colisuni : boolean;
            colislong: boolean;
            colvartype  : tsp_int2;
            colindtype  : tsp_int2;
            coltobuf   : tpr_intaddr;
            colfrombuf : tpr_intaddr;
            coltoindi  : tpr_intaddr;
            colfromindi: tpr_intaddr;
      END;


      sqlvartype  =   RECORD   (* 84 B *)
            colname  : tsp_knl_identifier;          (* selectlist columnnames  *)
            colio    : tsp_int2;          (* 0:inp ;  1:outp         *)
            colmode  : tsp1_param_opt_set;
            colfill1 : tsp_c3;
            coltype  : tsp_int2;          (* 0:number; 2: char        *)
            collength: tsp_int4;          (* anzahl stellen gesamt    *)
            colfrac  : tsp_int2;          (* anzahl stel. nach komma  *)
            colfill2 : tsp_int2;          (* anzahl stel. nach komma  *)
            hostvartype   : tsp_int2;     (* host variable type       *)
            hostcolsize   : tsp_int2;     (* hostvar size for array's *)
            hostindicator : tsp_int4;     (* host indicator variable  *)
            hostvaraddr : tpr_vtypepointer;(* hostvariablen address    *)
            hostindaddr : tpr_vtypepointer;(* host indicator addresse  *)
            sqlcoldb   : tpr_sqlcol;  (* interne tpr_sqlcol infos sqldb *)
      END;


      sqldatype =   RECORD
            sqldaid : PACKED ARRAY [ 1..8 ] OF char; (* sqlda_const *)
            sqlmax  : tsp_int4; (* max anzahl der SQLDA.sqlvar = 300*)
            sqln    : tsp_int2; (* anzahl sqlvar elemente      *)
            sqld    : tsp_int2; (* erg. anzahl sqlvar elemente *)
            sqlloop   : tsp_int4; (* array statem loop value,default=1 *)
            sqloffset : tsp_int4; (* offset of loop index  1,2.. *)
            sqlkano   : tsp_int2; (* leztes describe kommando    *)
            sqlprno   : tsp_int2; (* letztes describe prkommando *)
            sqlkamode : tsp_int2; (* call_interface gakamode     *)
            sqlfiller : tsp_int2; (* offset of loop index  1,2.. *)
            sqlrow    : tpr_sqlrow; (* sqlrow 8 B *)
            sqlvar  : ARRAY [ 1..mxpr_sqln ] OF sqlvartype;
      END;

      sqldapointer = ^sqldatype;
      (**************************************)
      (*        S Q L D A  for  D B 2 /S A A*)
      (* Descriptor    Area                 *)
      (* Datentype f?ur describe Kommandos   *)
      (* Variablen ?uber diesen type werden  *)
      (* declariert und dynamisch belegt.   *)
      (**************************************)

      sqlvardb2type  =   RECORD   (* 80 B *)
            sqltype    : tsp_int2;        (*   data type db2 or saa   *)
            sqllen     : tsp_int2;        (* data external length     *)
            sqldata    : tpr_vtypepointer;(* hostvariablen address    *)
            sqlind     : tpr_vtypepointer;(* indicator     address    *)
            sqlnamel      : tsp_int2;     (* sqlname length           *)
            sqlname       : tsp_c30;      (* selectlist columnnames   *)
            sqlcoldb      : tpr_sqlcol;   (* interne tpr_sqlcol infos *)
      END;


      sqldb2type =   RECORD
            sqldaid : PACKED ARRAY [ 1..8 ] OF char; (* sqlda_const *)
            sqldabc : tsp_int4;     (* max length of SQLDA in bytes *)
            sqln    : tsp_int2;     (* anzahl sqlvar elemente      *)
            sqld    : tsp_int2;     (* erg. anzahl sqlvar elemente *)
            sqlloop   : tsp_int4; (* array statem loop value,default=1 *)
            sqloffset : tsp_int4; (* offset of loop index  1,2.. *)
            sqlkano   : tsp_int2; (* call-int.  mfindex bei ffetch *)
            sqlprno   : tsp_int2; (* call-int.fetchspec bei ffetch *)
            sqlkamode : tsp_int2; (* call_interface gakamode     *)
            sqlfiller : tsp_int2; (* offset of loop index  1,2.. *)
            sqlrow    : tpr_sqlrow; (* sqlrow 8 B *)
            sqlvardb2 : ARRAY [ 1..mxpr_sqln ] OF sqlvardb2type;
      END;

      sqldb2pointer = ^sqldb2type;
      (**************************************)
      (*        S Q L D A                   *)
      (* Descriptor    Area   O R A C L E   *)
      (* Datentype f?ur describe Kommandos   *)
      (* Variablen ?uber diesen type werden  *)
      (* declariert und dynamisch belegt.   *)
      (**************************************)
      tpr_arr_int2 = ARRAY [1..mxpr_sqln] OF tsp_int2;
      tpr_arr_int4 = ARRAY [1..mxpr_sqln] OF tsp_int4;
      tpr_arr_ptrv = ARRAY [1..mxpr_sqln] OF tpr_vtypepointer;
      tpr_arr_dbcolinfo =  ARRAY [1..1] OF tpr_sqlcol;
      tpr_arr_ptrvp   = ^tpr_arr_ptrv;
      tpr_arr_int2p   = ^tpr_arr_int2;
      tpr_arr_int4p   = ^tpr_arr_int4;
      tpr_arr_sqlcolp = ^tpr_arr_dbcolinfo;

      tpr_sqlcolpointer = RECORD
            CASE boolean OF
                true :
                    (intaddr : tpr_intaddr);
                false :
                    (sqlcolp : tpr_arr_sqlcolp);
                END;
            (*ENDCASE*)


      sqloratype =   RECORD
            sqldaid : PACKED ARRAY [ 1..8 ] OF char; (* sqlda_const *)
            sqldabc : tsp_int4;   (* max length of SQLDA in bytes *)
            sqlfill : tsp_int4;   (* dummy *)
            sqlloop : tsp_int4;   (* sqlda array statement :loop *)
            sqloffset : tsp_int4; (* offset of loop index  1,2.. *)
            sqlkano   : tsp_int2; (* call-int.  mfindex bei ffetch *)
            sqlprno   : tsp_int2; (* call-int.fetchspec bei ffetch *)
            sqlkamode : tsp_int2; (* call_interface gakamode     *)
            sqlfiller : tsp_int2; (* offset of loop index  1,2.. *)
            sqlrow    : tpr_sqlrow; (* sqlrow 8 B *)
            sqlcoldb  : tpr_sqlcolpointer; (* pointer to array of sqlcol's *)
            ora_n   : tsp_int4;   (* anzahl sqlvar elemente      *)
            ora_v   : tpr_arr_ptrvp; (** ptr arr of addr of SLI or BV values *)
            ora_l   : tpr_arr_int4p; (*  ptr arr of length of SLI or BV value*)
            ora_t   : tpr_arr_int2p; (*  ptr arr of datatyp.of SLI or BV val.*)
            ora_i   : tpr_arr_ptrvp; (** ptr arr of addr of indicator values *)
            ora_f   : tsp_int4;   (*  actuelle anzahl sqlvar elemente     *)
            ora_s   : tpr_arr_ptrvp; (** ptr arr of addr of SLI or P names   *)
            ora_m   : tpr_arr_int2p; (*  ptr arr of max-leng of SLI or P name*)
            ora_c   : tpr_arr_int2p; (*  ptr arr of cur-leng of SLI or P name*)
            ora_x   : tpr_arr_ptrvp; (** ptr arr of addr of indicator names  *)
            ora_y   : tpr_arr_int2p; (*  ptr arr of max-leng of indica.names *)
            ora_z   : tpr_arr_int2p; (*  ptr arr of cur-leng of indica.names *)
            (* for cobol only *)
            ora_fm  : tpr_arr_ptrvp; (** ptr arr of addr of hostvar decimal  *)
            ora_fml : tpr_arr_int2p; (*  ptr arr of max-leng of hostvar decim*)
            ora_fclp: tpr_arr_int2p; (*  ptr set to null for future use    *)
            ora_fcrc: tpr_arr_int2p; (*  ptr set to null for future use    *)
      END;

      sqlorapointer = ^sqloratype;
      (*********************************************)
      (*        S Q L D A V R E C    and           *)
      (*        S Q L V A R V R E C                *)
      (*  Varient record for sqlda in              *)
      (*    call interface                         *)
      (*********************************************)

      sqlvarvrec  = RECORD
            CASE boolean OF
                true :
                    (sqldbv  : sqlvartype);
                false :
                    (db2v   : sqlvardb2type);
                END;
            (*ENDCASE*)

      tpr_kind      = cpr_kind_empty .. cpr_kind_oracle;

      sqldaptrvrec  = RECORD
            CASE tpr_kind  OF
                cpr_kind_internal :
                    (sqldaptr  : sqldapointer);
                cpr_kind_db2 :
                    (db2daptr  : sqldb2pointer);
                cpr_kind_oracle :
                    (oradaptr  : sqlorapointer);
                cpr_kind_empty  :
                    (intaddr   : tpr_intaddr);
                END;
            (*ENDCASE*)


      sqldavrec  = RECORD
            CASE tpr_kind  OF
                cpr_kind_internal :
                    (sqldb  : sqldatype);
                cpr_kind_db2 :
                    (db2   : sqldb2type);
                cpr_kind_oracle :
                    (orada   : sqloratype);
                END;
            (*ENDCASE*)
            (**************************************)
            (*        S Q L T R A C E  TYPE  neu  *)
            (*    trace output infos              *)
            (*    aus alten sqlca ?ubernommen    *)
            (**************************************)


      sqltatype    = RECORD
            tatracefn    : tsp00_VFilename; (* options name oder actueller *)
            tatracepprt  : tsp_int4;     (* lzu 3.0 vf  poolprt *)
            tatraceno    : tsp_int4;     (* fileno nach vfopen *)
            tatracety    : tsp_int2;     (* option type oder actueller  *)
            tamaxsecoutp : tsp_int2;     (*is_true komm. wird ausgegeben*)
            tamaxsecfn   : tsp00_VFilename; (* sqltracefn max sec *)
            tamaxsecno   : tsp_int4;     (* file number*)
            tamaxsecpptr : tsp_int4;     (* pool-pointer*)
            tastartdate  : tsp_c8;
            taenddate    : tsp_c8;
            tastarttime  : tsp_c8;
            taendtime    : tsp_c8;
            tacnttrstm   : tsp_int2;  (* count statem.s in trace zyklisch*)
            tatrout      : tsp_int2;  (* trace output typefr?uher xatrace *)
            tafiller     : tsp_int2;
            tastr80l     : tsp_int2;
            tastr80      : tsp_c256;   (* output string z.Z parameter *)
      END;

      sqltapointer =  ^sqltatype;
      (**************************************)
      (*    O P T I O N - A R E A   neu     *)
      (*    runtime optionen                *)
      (*    aus alten sqlga ?ubernommen      *)
      (**************************************)

      sqloatype  = RECORD   (* 82 B *)
            oamodtracefn    : tsp00_VFilename;  (* modul options *)
            oamodtrace      : tsp_int2;     (* modul option=true*)
            oamodtracety    : tsp_int2;     (* modul options *)
            oamodsettrty    : tsp_int2;     (* set trace options *)
            oamaxtrstm      : tsp_int2;  (* max statem.s in trace zyklisch*)
            oanosdf         : tsp_int2;  (* r.op noseldirfast  cpr_is_true*)
            oamaxsec        : tsp_int2;  (* trace max sec command option  *)
            oapidflag       : tsp_int2;  (* tracefilename :: processid    *)
            oapid           : tsp_int4;
      END;

      sqloapointer =  ^sqloatype;
      (*********************************************)
      (*        LONGVARCHAR                        *)
      (*  For sql commands with longvarchars       *)
      (* Structur to send a hostvariable >= 4k     *)
      (* to the  kernel                            *)
      (*********************************************)

      tpr_longvarcharrec   = RECORD       (* 44 B *)
            lvccolno   : tsp_int2;  (* last colno           *)
            lvcdescno  : tsp_int2;  (* last descno          *)
            lvcfreepos : tsp_int4;  (* varpart inp  freepos *)
            lvcaoffset : tsp_int2;  (* last hostaddr offset *)
            lvcnext    : boolean;   (* next longvar column  *)
            lvcvalmode : tsp_valmode;  (* last desc valmode *)
            lvcoldpos  : tsp_int2;     (* dummy     *)
            lvcoldlen  : tsp_int2;     (* dummy     *)
            lvcvalpos  : tsp_int4;     (* last desc valpos  *)
            lvcvallen  : tsp_int4;     (* last desc vallen  *)
            lvchostpos : tsp_int4;  (* last hostvar pos     *)
            lvchostlen : tsp_int4;  (* last hostvarlen      *)
            lvcindicator:tsp_int4;  (* indicator inhalt     *)
            lvcvalmlen : tsp_int4;  (* max len an kern gesendet *)
            lvcerror   : tpr_runtime_errors; (*        error    *)
            lvcpiecewise : boolean;
      END;

      (*********************************************)
      (*        LONGVARCHAR DESCRIPTOINS INFOS     *)
      (*  For sql commands with longvarchars       *)
      (*  globale variable in g_init allocated     *)
      (* pointer is set in sqlra-area (dlvcp)      *)
      (*   will be set in procedure p04pinputvalue *)
      (*   will be init in p01adbsexecute          *)
      (*   will be used only for one command       *)
      (*********************************************)

      tpr_ptr_packet = RECORD
            CASE boolean OF
                true :
                    (intaddr : tpr_intaddr);
                false :
                    (vtypep  : tsp1_packet_ptr);
                END;
            (*ENDCASE*)


      tpr_ptr_moveobj = RECORD
            CASE boolean OF
                true :
                    (intaddr : tpr_intaddr);
                false :
                    (vtypep  : tsp_moveobj_ptr);
                END;
            (*ENDCASE*)


      tpr_restbuflong = RECORD     (* 28 B *)
            rbcolno      : tsp_int2;  (* last bearbeitete colno *)
            rbfiller1    : tsp_int2;  (* last bearbeitete colno *)
            rbfiller2    : tsp_int4;  (* last bearbeitete colno *)
            rbrestlen    : tsp_int4;  (* anz bytes im puffer    *)
            rbhostlensum : tsp_int4;  (* akumulierte hostlength *)
            rblongcolmax : tsp_int4;  (* maximale longcollength *)
            rbbufsize    : tsp_int4;  (* allocate leng from rbvarpart *)
            rbbufpart    : tpr_ptr_moveobj;
      END;


      tpr_ptr_restbuflong = RECORD
            CASE boolean OF
                true :
                    (intaddr : tpr_intaddr);
                false :
                    (vtypep  : ^tpr_restbuflong);
                END;
            (*ENDCASE*)


      tpr_colpointer = RECORD
            CASE boolean OF
                true:
                    (addr: ^tpr_sqlcol);
                false:
                    (intaddr: tpr_intaddr);
                END;
            (*ENDCASE*)


      tpr_sqlspld   = RECORD   (* 60 B *)
            CASE boolean OF
                false:
                    (
                          ldkind_o   :  tsp_int2; (* cpc_if_false : old *)
                    ldlength_o :  tsp_int2; (* sizeof (tsp_long_descriptor) *)
                descold :
                    tsp_long_descriptor
                          );
                true :
                    (
                          ldkind_n   :  tsp_int2; (*  cpr_is_true : new *)
                    ldlength_n :  tsp_int2; (* sizeof (tsp_long_desc_block) *)
                descnew :
                    tsp_long_desc_block
                          );
                END;
            (*ENDCASE*)


      tpr_sqlpcld   = RECORD    (* 44 B *)
            ldhostaddr : tpr_vtypepointer; (* last hostvaraddr  *)
            ldindkaddr : tpr_vtypepointer; (* last indikatoraddr*)
            ldcoladdr  : tpr_colpointer;   (* sqlcol record addr*)
            ldhostlen  : tsp_int4;  (* last hostvarlen      *)
            ldhosttype : tsp_int2;  (* last hostvartype     *)
            ldindktype : tsp_int2;  (* last indikatortype   *)
            ldindklen  : tsp_int2;  (* last indikatortypelen*)
            ldindkfrac : tsp_int2;  (* last indikatortypefrac*)
            ldrowno    : tsp_int2;  (* rowno for m_commands *)
            ldcolno    : tsp_int2;  (* colno im record      *)
            ldsfiotype : tsp_c1;    (* index 1 = sfiotype  *)
            ldsfdtype  : tsp_data_type; (* index 2 = sfdtype   *)
            ldfill     : tsp_int2;
            ldfileno   : tsp_int4;  (* fileno if long spalte datei *)
            ldsfbufpos : tsp_int4;  (* first inp varpart pos*)
            ldhostlensum : tsp_int4;(* hostvarlen aufaddiert  *)
            ldrestdata : tpr_ptr_restbuflong;  (* pointer to restdata in varpart *)
      END;

      tpr_sqlspldarr = ARRAY [ 1..mxpr_descrlvc ] OF tpr_sqlspld;
      tpr_sqlpcldarr = ARRAY [ 1..mxpr_descrlvc ] OF tpr_sqlpcld;
      tpr_sqlspldptr = ^tpr_sqlspldarr;
      tpr_sqlpcldptr = ^tpr_sqlpcldarr;

      sqlorentry_ptr = ^sqlorentry;

      tpr_sqlld   = RECORD          (* 36 B *)
            ldmaxi       : tsp_int2; (* maximale lvc-col inp *)
            ldmaxo       : tsp_int2; (* maximale lvc-col outp*)
            ldindpc      : tsp_int2; (* index to pc_descr    *)
            ldindsp      : tsp_int2; (* index to ldsp*)
            ldcolkind    : tsp_int2; (* cpr_is_true lvc-col,cpr_is_false*)
            ldcount      : tsp_int2; (* anz act. descr in buf*)
            ldputvalsend : boolean;  (* init false, true putval send *)
            ldallsend    : boolean;  (* init false, true all record send*)
            ldretmaxi    : tsp_int2; (* maximale lvc-col inp *)
            ldretmaxo    : tsp_int2; (* maximale lvc-col outp*)
            ldretindpc   : tsp_int2; (* index to pc_descr    *)
            ldretindsp   : tsp_int2; (* index to ldsp*)
            ldrestbufp   : tpr_ptr_restbuflong;
            lderrd3      : tsp_int4; (* anz fetched rows *)
            ldpc         : tpr_sqlpcldptr;
            ldsp         : tpr_sqlspldptr;
            ldpc_mdesc   : tpr_sqlmdesc; (* PTS 1102339 *)
            ldsp_mdesc   : tpr_sqlmdesc; (* PTS 1102339 *)
            ldpcarr      : tpr_intaddr;
            ldsparr      : tpr_intaddr;
            ldpcarr_mdesc : tpr_sqlmdesc;
            ldsparr_mdesc : tpr_sqlmdesc;
	    ldorelastcmd : sqlorentry_ptr; (* ref. to sqlor area of cmd before putval/getval *)
      END;


      tpr_ptr_sqlld = RECORD
            CASE boolean OF
                true :
                    (intaddr : tpr_intaddr);
                false :
                    (vtypep  : ^tpr_sqlld);
                END;
            (*ENDCASE*)
            (**************************************)
            (*        S Q L G A   neu              *)
            (*  Global Area                       *)
            (* Globaldaten, die zusaetzlich       *)
            (* benoetigt werden                   *)
            (**************************************)

      tpr_part_kind_ptr_record = ARRAY [ 1..mxpr_partkind ] OF tsp1_part_ptr;

      sqlgaentry =  RECORD     (* 220 B *)
      (* sqlga *)
            gareference : tsp_process_id; (* adabas pid 4B   *)
            gamesscode  : tsp_c2;         (*code_type, swap_kind*)
            gastatus    : tsp_int2;
            gavaindex   : tsp_int2; (*0=tpr_hostname, >0 :hostvar.*)
            ganodeindex : tsp_int2;
            gareconnect : tsp_int2;  (* reconnect set is_true, *)
            ganolog     : tsp_int2;
            gaprofinit  : tsp_int2;        (* profile init   *)
            gadbcodeset : tsp_int2;        (* connect codeset *)
            gaatdbname  : tsp_knl_identifier;      (* db-name  18B    *)
            gafiller2   : tsp_int2;
            gapatchptr  : tpr_intaddr;   (*  patch area *)
            gareqptr    : tsp1_packet_ptr;
            garecptr    : tsp1_packet_ptr;
            gapacketlist: tsp1_packet_list;
            gaxuserrec : tsp4_xuser_record;   (* 180 B *)
            gaKernelVersion : tsp00_Int4;
            gaServerNode : tpr_intaddr;
            gaIsCanceled : boolean;
            gaKnlSessionID : tpr00_SessionID;
      END;

      sqlgaentrypointer   = ^sqlgaentry;

      sqlgatype =  RECORD     (* 232 B *)
      (* sqlga *)
            gaopxuserrec  : tsp4_xuser_record;   (* 180 B *)
            gauseropset   : tsp_int2;  (* cpr_is_true, if useroptions *)
            gamodisolation: tsp_int2;  (* modul options *)
            gasqlconntyp  : tsp_int2;
            gaencodingcnt : tsp_int2;     (* from s81uni_fload         *)
            gaencodingptr : tpr_intaddr;  (* pointer from s81uni_fload *)
            gaasciicodeset: tsp_int2;     (* src_  from s81uni_fload   *)
            gaunicodeset  : tsp_int2;     (* dest_ from s81uni_fload   *)
            gacmpkind     : tsp_int2;     (* component, oci, odbc, c, cob *)
            gaCmdState    : tpr_cmd_state_Enum ;
            gaConnection  : tsp_addr;
            gaentry       : PACKED ARRAY  [1..8] OF sqlgaentrypointer;
      END;

      sqlgapointer =  ^sqlgatype;
      (**************************************)
      (*        S Q L R A                   *)
      (*    Runtime Area                    *)
      (* SQLDB-Auftragssegment               *)
      (* f?ur  SQL und Dialog Aufrufe       *)
      (* Precompilerprogramm                *)
      (**************************************)

      tpr_error_warnset =  RECORD
            CASE boolean OF
                true :
                    (int2 : tsp_int2);
                false :
                    (warn : tsp_warningset);
                END;
            (*ENDCASE*)
            (**************************************)
            (*        S Q L R A  neu              *)
            (* Runtime Area SQLDB-Auftragssegment *)
            (* f?ur  SQL und Dialog Aufrufe       *)
            (* Precompilerprogramm                *)
            (**************************************)


      tpr_sqlerror  =  RECORD    (* 104 B *)
            ereturncode   :  tsp_int2;
            etextlen      :  tsp_int2;
            eerrorpos     :  tsp_int4;
            ewarning      :  tsp_c16;
            esqlstate     :  tsp_c6;
            elzu          :  tsp1_comm_error;
            eprerr        :  tpr_runtime_errors;
            etext         :  tsp_c70;
      END;

      sqlempointer = ^tpr_sqlerror;

      sqlratype  = RECORD    (* 68 B *)
      (*  sqlra *)
            ratermid      : tsp_c8;
            raprocessid   : tsp_process_id;  (*4B anwender pid  *)
            racomponent   : tsp_c3;
            rasqlos       : tsp_os;    (* 1 B char betriebssystem *)
            ralang        : tsp_int2;  (* language of runtime syst*)
            rasqlinit     : tsp_int2;  (* sqlinit = 3 cpr_is_init  *)
            raactsession  : tsp_int2;
            radbmode      : tsp_int2;  (* programm sqldbmode      *)
            rasqlansi     : tsp_int2;  (*z cpr_kind_empty or kind_ansi *)
            rakamode      : tsp_int2;  (* actual statem mode      *)
            ramodeswitch  : tsp_int2;  (* switch fuer mess_type   *)
            ramesstype    : tsp_int2;  (* last request mess_type  *)
            ranumrows     : tsp_int4;  (* number of rows errd(3)  *)
            ranotrtimeoutp: tsp_int2;  (* no trace time output    *)
            raopprof      : tsp_int2;  (* profile option          *)
            raprofsec     : tsp_int4;  (* profile sec pro command *)
            raparse_again : boolean ;  (* true, wenn -8 gesetzt   *)
            rafiller      : tsp_c3  ;
            ra2filler     : tsp_int4;
            rasegptr      : tsp1_segment_ptr;
            rasegmpartptr : tpr_part_kind_ptr_record;
            raprofstmtp   : tpr_intaddr;  (* profile stmt-pointer  *)
            rasqlldp      : tpr_ptr_sqlld;    (* dlvc pointer      *)
            rasqltap      : sqltapointer;    (* trace area        *)
      END;

      sqlrapointer = ^sqlratype;
      (*********************************************)
      (*        S Q L S N                          *)
      (* Statementname Area for statementname or   *)
      (* cursorname from hostvar                   *)
      (*********************************************)

      tpr_sqlsnentry =  RECORD           (* 52 B *)
            snname    : tsp_knl_identifier;     (* statement or cursorname *)
            sntype    : tsp_int2;     (* Type of name cpr_is_statement or *)
            (* cpr_is_cursor *)
            snindex   : tsp_int2;     (* index into st-area or sn-area *)
            (* in case of sntype *)
            sncount   : tsp_int2;     (* count of st-area *)
            snfiller1  : tsp_int2;
            snstmtp   : tpr_vtypepointer; (* pointer to statement if the *)
            (* comes from hostvar snstind = 0 *)
            snbuflen  : tsp_int4;     (* len of allcate buffer  *)
            snstmtlen : tsp_int4;     (* len of statement *)
            (** neu 8.7.96 ****)
            snsession : tsp_int2;     (* prepare stmname raactsessionr *)
            snfiller2  : tsp_int2;
            snParseInfo : tpr00_ParseInfo;  (* parsid von prepare  *)
      END;

      tpr_sqlsnentries = ARRAY [ 1..mxpr_sqlsn ] OF tpr_sqlsnentry;

      tpr_ptr_sqlsne = RECORD
            CASE boolean OF
                true :
                    (intaddr : tpr_intaddr);
                false :
                    (vtypep  : ^tpr_sqlsnentries);
                END;
            (*ENDCASE*)


      tpr_sqlsn = RECORD
            snentcnt : tsp_int4;
            snentact : tsp_int4;    (* actueller snentry index **)
            snentptr : tpr_ptr_sqlsne;
      END;

      (*********************************************)
      (*        S Q L M A                          *)
      (*  Macro Area                              *)
      (* Liste fester Laenge                       *)
      (* Eintraege enthalten Werte der Macro-      *)
      (* parameter, die in SQL-Kommandos des       *)
      (* Hostprogramms vorkommen                   *)
      (*********************************************)

      tpr_mavalue  =  RECORD       (* 32 B *)
            malen      : tsp_int2;    (* macrolength  *)
            maval      : tpr_macroline;
      END;

      sqlmaentries = ARRAY [ 1..mxpr_sqlma ] OF tpr_mavalue;

      sqlmatype = RECORD       (* 4100 B *)
      (* sqlma *)
            maversion : tsp_int4;
            maentry   : sqlmaentries;
      END;

      sqlmapointer =  ^sqlmatype;

      tpr05_StringEncoding = tsp_addr;

      (**************************************)
      (*        S Q L M F                   *)
      (* Mfetch Area                        *)
      (* wird zur Umschaltung von fetch auf *)
      (* mfetch gegraucht                   *)
      (**************************************)
      (* fetch optimierungs typen  *)

      tpr_mfetchentry  = RECORD      (* 92 B *)
            mfInit       : tsp_int2; (* cpr_is_true record belegt *)
            mfSelectno   : tsp_int2; (* zugeh. select number  *)
            mfBufinit    : tsp_int2; (* 0 = kein new alloc,<>0 buf exist*)
            mfReclen     : tsp_int2; (* acctuelle record length im buf *)
            mfBuflen     : tsp_int4; (* zugeh. auftrags puffer length*)
            mfRecpos     : tsp_int4; (* acctuelle record position im buf*)
            mfPrevfaspec : tsp_int2; (* faspec from command before      *)
            mfLastfaspec : tsp_int2; (* faspec cpr_fe_last if it was called *)
            mfRownotfound: tsp_int2; (* error : cpr_is_true,                *)
            mfFetchget   : tsp_int2; (* init = 0, if one mfetch = cpr_is_init*)
            mfRecposbeg  : tsp_int4; (* first record position in buf    *)
            mfMaxselrec  : tsp_int4; (* max records in result:sqlerrd(3)*)
            mfLastrecno  : tsp_int4; (* last fetch rec no to user    *)
            mfNextLoopCnt: tsp_int4; (* Number of remaining records in mfBuf *)
            mfAllRecordsRead: tsp_int4;
            mfActualPos: tsp_int4;   (* Position of the actual record to read *)
            mfBufbegno   : tsp_int4; (* record begin no  in buffer   *)
            mfBufendno   : tsp_int4; (* record end no  in buffer     *)
            mfLongcolpos : tsp_int4; (* first longvarchar pos in buf    *)
            mfBufpointer : tpr_ptr_packet;  (* pointer eines packets.   *)
            mfrecpointer : tpr_ptr_moveobj; (* pointer auf 1.record save*)
            mfsegmpartptr: tpr_part_kind_ptr_record;
            mfrecptrlen  : tsp_int4;    (* length of rec pointer buffer *)
            mforindex    : tsp_int2;    (* index in or_area von open    *)
            mffetchkind  : tsp_int2;    (*init:is_true, fetch:is_false, *)
            (*  mfetch: is_loop             *)
            mfsumanzrec  : tsp_int4;    (* anz rec gesammt bei mfetch is_loop*)
            mfopendata   : tsp_int2;    (*init:is_false, opendata:is_true*)
            mfPrecomindex: tsp_int2;    (* empty    *)
            mfSelparsid  : tpr_parsid;  (* select parsid des mfetch     *)
            mfresultname : tsp_knl_identifier;    (* select resultname            *)
            PacketEncoding : tpr05_StringEncoding;
      END;

      sqlmfentarr = ARRAY [ 1..csp_maxint2 ] OF tpr_mfetchentry;
      sqlmfentptr = ^sqlmfentarr;

      sqlmftype   =  RECORD    (* 1516 B *)
            mfselcnt   :  tsp_int2;
            mfmfetch   :  tsp_int2; (* change next com to mfetch *)
            mffiller   :  tsp_int4; (* 8 alignment                *)
            mfdesc     :  tpr_sqlmdesc;
            mfentry    :  sqlmfentptr;
      END;

      sqlmfpointer =  ^sqlmftype;
      tpr_sqlarea_mode  = 0.. 10 ;

      sqlareapointer = RECORD
            CASE tpr_sqlarea_mode  OF
                0  :
                    (intaddr   : tpr_intaddr);
                1 :
                    (sqlgaptr  : sqlgapointer);
                2 :
                    (sqlraptr  : sqlrapointer);
                3 :
                    (sqloaptr  : sqloapointer);
                4 :
                    (sqlmaptr  : sqlmapointer);
                5 :
                    (sqlmfptr  : sqlmfpointer);
                6 :
                    (sqllaptr  : tpr_ptr_sqlld);
                7 :
                    (sqltaptr  : sqltapointer);
                END;
            (*ENDCASE*)
            (*********************************************)
            (*        S Q L V A 1, 2, 3, EX - A R E A S  *)
            (*  Variable Area                            *)
            (* sqlva1 addressen der hostvariblen + index *)
            (*        to  sqlva2, sqlva3                 *)
            (* sqlva2 contains the datatyps              *)
            (* sqlva3 contains the variable names        *)
            (* sqlnd   contains the declare for expanding*)
            (*         the sqlva1 table  only used       *)
            (*         during precompiling (vpc00)       *)
            (*********************************************)


      sqlva1entry = RECORD    (* 16 B *)
            CASE tsp_int2 OF
                sqlvasc:
                    (
                          va1indi_sc : tsp_int2;
                    va1indva2_sc : tsp_int2;
                    va1indva3_sc : tsp_int2;
                    va1fill   : tsp_int2;
                    va1addr_sc : tpr_vtypepointer;
                    va1fill1  : tpr_intaddr;
                    );
                sqlvacm:
                    (
                          va1indi_cm  : tsp_int2;
                    va1indva2_cm: tsp_int2;
                    va1indva3_cm: tsp_int2;
                    va1fill_cm  : tsp_int2;
                    va1offs_cm    : tsp_int4;
                    va1fill2   : tsp_int4;
                    );
                    sqlvast:
                          (
                          va1indi_st : tsp_int2;
                    va1cmpcnt_st : tsp_int2;
                    va1size_st   : tsp_int4;
                    va1addr_st : tpr_vtypepointer;
                    va1ix_st     : tsp_int4;
                    );
                    sqlvapt:
                          (
                          va1indi_pt : tsp_int2;
                    va1fill3  : tsp_int2;
                    va1ix_pt   : tsp_int4;
                    va1addr_pt : tpr_vtypepointer;
                    va1fill5  : tpr_intaddr;
                    );
                END;
            (*ENDCASE*)


      sqlva2entry = RECORD      (* 12 B *)
            va2typ   : tsp_int2;
            va2digit : tsp_int2;
            va2size  : tsp_int4;
            va2frac  : tsp_int2;
            va2const : tsp_int2; (* const flag; if va2const <> 0 then va1addr
            can only be used as input parameter *)
      END;


      sqlva3entry = RECORD        (* 36 B *)
            va3name  : tsp_lname;
            va3naml  : tsp_int2;        (* vaname length *)
            va3fill: tsp_int2;
      END;

      sqlva1type = ARRAY [ 1..mxpr_sqlva1 ] OF sqlva1entry;
      sqlva1p    = ^sqlva1type;
      sqlva2type = ARRAY [ 1..mxpr_sqlva2 ] OF sqlva2entry;
      sqlva2p    = ^sqlva2type;
      sqlva3type = ARRAY [ 1..mxpr_sqlva3 ] OF sqlva3entry;
      sqlva3p    = ^sqlva3type;
      (*********************************************)
      (*        S Q L P A R  neu                   *)
      (* Parameter Area                            *)
      (* loopcount-entry, struct-enty und          *)
      (* Liste, soviele Eintraege wie Summe aller  *)
      (* Parameter in allen SQL-Kommandos des      *)
      (* Hostprogramms                             *)
      (* Eintraege enthalten Verweise auf Host-    *)
      (* variable und Indikatorvariable und Be-    *)
      (* schreibung - fuer Parameter-Uebergabe     *)
      (* zwischen Hostprogramm und SQLDB - der      *)
      (* Hostvariablen                             *)
      (*********************************************)

      sqlparentry = RECORD            (* 12 B *)
            CASE tsp_int2 OF  (* parType, loop, struc, element, sqlpl, funcpl *)
                sqlparlo :
                    (  pakindlo   : tsp_int2;
                    pava1index : tsp_int2;      (* hostvar index :loop *)
                    paloopcnt  : tsp_int4;      (* const of loop count *)
                    paloopmin  : tsp_int4;  (* minimum of var-arrays cnt*)
                    );
                    sqlparst :
                          (  pakindst    : tsp_int2;
                    pavarst     : tsp_int2;         (* sqlva1 stindex *)
                    paindst     : tsp_int2;         (* sqlva1 stindex *)
                    paelcnt     : tsp_int2;
                    pafill4     : tpr_intaddr;
                    );
                sqlparel :
                    (  pakindel : tsp_int2;
                    pavarno  : tsp_int2;         (* sqlva1 index *)
                    paindno  : tsp_int2;         (* sqlva1 index *)
                    );
                END;
            (*ENDCASE*)

      sqlpartype = ARRAY [ 1..mxpr_sqlpa ] OF sqlparentry;
      sqlparpointer = ^sqlpartype;
      (*********************************************)
      (*        S Q L F A                          *)
      (*  Fetch Area                               *)
      (* Eintraege enthalten zur mfetch Abarbeitung*)
      (*     interne Werte                         *)
      (*********************************************)

      sqlfaentry =  RECORD        (* 16 B *)
            faselectno : tsp_int2;   (* selectno of mfarea *)
            famfindex  : tsp_int2;   (* index into mfarea  *)
            fapos      : tsp_int4;   (* pos value of fetch *)
            faspec     : tsp_int2;   (* specification fetch*)
            fafetch    : tsp_int2;   (* cpr_is_true for fetch stm *)
            fareclen   : tsp_int4;   (* output record length  *)
      END;

      sqlfatype = ARRAY [ 1..mxpr_sqlfa ] OF sqlfaentry;
      sqlfapointer  = ^sqlfatype;
      (*********************************************)
      (*        S Q L K A                          *)
      (*  Kommando Area                            *)
      (* Struktur aus Initialisierungs-Flag fuer   *)
      (* Hatzel-Check und einer Liste mit sovielen *)
      (* Eintraegen wie SQL-Kommandos im Hostpro-  *)
      (* gramm eingebettet sind.                   *)
      (* Eintraege enthalten Kommandobeschreibung  *)
      (* und Verweis auf Parameterbeschreibungen   *)
      (*********************************************)

      sqlkaentry =  RECORD       (* 56 B *)
            katyp      : tsp_int2;
            kastate    : tsp_int2;
            kapacount : tsp_int2;      (* sqlpar anzahl   *)
            kapaindex : tsp_int2;      (* sqlpar 1. index *)
            kaStcount : tsp_int2;      (* sqlst anzahl   *)
            kaStindex : tsp_int2;      (* sqlst 1. index *)
            kamacro    : tsp_int2;          (* true : enth?altmacros *)
            kaprindex  : tsp_int2;      (* prarea-index (prepare)*)
            kadiindex  : tsp_int2;      (* dialog-index (dialog) *)
            kaparserrc : tsp_int2;      (* pars  error_code      *)
            kaversion  : tsp_int4;      (* macro version  *)
            kaParseInfo : tpr00_ParseInfo;
            kaopendata : tsp_int2;      (* fetch results from open command *)
            kaparamcnt : tsp_int2;      (* interner kasa z?ahler *)
            kalineno   : tsp_int2;      (* lineno for profiling  *)
            kafaindex  : tsp_int2;      (* faarea index (fetch ) *)
            kaatindex  : tsp_int2;      (* atarea index (ora-db) *)
            kacuindex  : tsp_int2;      (* cuarea index (cursor, statement) *)
            kamode     : tsp_int2;      (* sqldbmode of the current command *)
            kasqlrow   : tpr_sqlrow;    (* reclen info's after parsen *)
            kafae      : ^sqlfaentry;
      END;

      sqlkaentry_ptr = ^sqlkaentry;
      sqlkatype = ARRAY [ 1..mxpr_sqlka ] OF sqlkaentry;
      sqlkapointer  = ^sqlkatype;
      (*********************************************)
      (*        S Q L O R                          *)
      (*  Oracle   Area                            *)
      (* Eintraege enthalten zu jedem Kommando     *)
      (* von Prepare, Open, Fetch  Kommandos       *)
      (* Pointer auf die shortfield infos,         *)
      (* ist open neu geparsed, und beim fetch den *)
      (* kaindex zum zugehoerigen open             *)
      (*********************************************)

      sqlorentry =  RECORD      (* 28 B *)
            orcolcnt   : tsp_int4;
            orcollen   : tsp_int4;
            orsqlrow   : tpr_sqlrow;    (* reclen info's after parsen *)
            orcolptr   : tpr_sqlcolpointer; (* sqlcol-info pointer  *)
            orcolcntacc: tsp_int4;(* acctuelle colcnt belegung      *)
            oropennew  : tsp_int2;(*nach:open cpr_is_true,fetch cpr_is_false*)
            (**)
            orrescnt   : tsp_int4;    (* max anz result columns *)
            (**)
            orreslen   : tsp_int4;
            (**)
            orressqlrow: tpr_sqlrow;  (* res-reclen info's after parsen *)
            (**)
            orresptr   : tpr_sqlcolpointer;  (* rescol-info pointer  *)
            (**)
            orrescntacc: tsp_int4; (* acctuelle rescolcnt belegung   *)
            (**)
            orresnamlen: tsp_int4; (* max  resnamelen alloc <1Blen,name> *)
            (**)
            orresnamptr: tpr_vtypepointer; (* rescol-names pointer  *)
            (**)
            orresnamacc: tsp_int4; (* acctuelle resnamelen belegung   *)
            orresnamencoding: tpr_vtypepointer; (* acctuelle resnamelen belegung   *)
      END;

      (* sqlorentry_ptr = ^sqlorentry; *) (* declared above before tpr_sqlld *)
      sqlortype = ARRAY [ 1..mxpr_sqlka ] OF sqlorentry;
      sqlorpointer  = ^sqlortype;
      (*********************************************)
      (*        S Q L P R                          *)
      (*  Prepare  Area                            *)
      (* Eintraege enthalten Kommandobeschreibung  *)
      (* von Prepare Kommandos  und                *)
      (* von Report  Kommandos                     *)
      (* und Verweis auf Parameterbeschreibungen   *)
      (*********************************************)

      sqlprentry =  RECORD
            prStmtNameIndex   : tsp_int2;  (* > 0 statementname in va-area *)
            prCursorNameIndex : tsp_int2;  (* > 0 cursorname in va-area *)
            prstate           : tsp_int2;
            prStmtName        : tsp_knl_identifier; (*statementname*)
            prkaindex         : tsp_int2;  (*sqlka index  *)
            prarea            : tsp_int2;  (*sqlva od. sqlst*)
            prDescribe        : tsp00_Int2;
            prCursorName      : tsp_knl_identifier;  (* cursorname *)
            prcuindex         : tsp_int2;  (* > 0 cursorname from hostvar *)
            prstcount         : tsp_int2;  (* anz sqlst entries *)
            prstindex         : tsp_int2;  (* st_count = 0 index into va *)
      END;

      sqlprtype = ARRAY [ 1..mxpr_sqlpr ] OF sqlprentry;
      sqlprpointer  = ^sqlprtype;
      (*********************************************)
      (*        S Q L S T                          *)
      (*  Statement  Area                          *)
      (* Eintraege enthalten Stringkonstanten      *)
      (* von Prepare  Kommandos                    *)
      (*********************************************)

      sqlstentry =  RECORD        (* 56 B *)
            stkano     : tsp_int4;
            stllen     : tsp_int4;
            stline     : tpr_sqlline;
      END;

      sqlsttype = ARRAY [ 1..mxpr_sqlst ] OF sqlstentry;
      sqlstpointer  = ^sqlsttype;
      (*********************************************)
      (*        S Q L A T                          *)
      (*  AT <dbname> for  ORACLE Area             *)
      (* Eintraege enthalten DBnamen-beschreibung  *)
      (* von Oracle  Kommandos  und                *)
      (* und Verweise auf die gaarea               *)
      (*********************************************)

      sqlatentry =  RECORD      (* 88 B *)
            atdbname   : tsp_knl_identifier;      (* db-name  18B    *)
            atdbindex  : tsp_int2;      (* index in sqlva1 *)
            atdbstring : tsp_nodeid;    (* servernode  64B  *)
            atgaindex  : tsp_int2;      (* actt. index into ga-area *)
            atdbnamix  : tsp_int2;      (* dbname index in sqlpa    *)
      END;

      sqlattype = ARRAY [ 1..mxpr_sqlat ] OF sqlatentry;
      sqlatpointer  = ^sqlattype;
      (*********************************************)
      (*        S Q L C U                          *)
      (*  Cursor and Statementname   Area  Oracle  *)
      (* Eintraege enthalten Ergn u Stmn  for      *)
      (* multi db   Kommandos                      *)
      (*********************************************)

      sqlcuentry =  RECORD        (* 28 B *)
            cuname     : tsp_knl_identifier;     (* erg or stm-name 18B *)
            cufill     : tsp_int2;
            cuatindex  : tsp_int2;     (* index into at-area  *)
            cuerrd3    : tsp_int4;     (* accumulierter erd3 des cursors *)
            custate    : tsp_int2;     (* Status des cursors *)
            cukind     : tsp_int2;     (* cursor = 0; statement = 1 *)
            cukaopenind : tsp_int2;
            culoopin   : tsp_int4;
            culoopout  : tsp_int4;
            cusqlindap : sqldaptrvrec;
            cusqloutdap: sqldaptrvrec;
      END;

      sqlcuentry_ptr = ^sqlcuentry;
      sqlcutype = ARRAY [ 1..mxpr_sqlcu ] OF sqlcuentry;
      sqlcupointer  = ^sqlcutype;

      (*********************************************)
      (*        S Q L F N                          *)
      (*  Filename include Area                    *)
      (* Eintraege enthalten kanomin bis kanomax   *)
      (* und filename des include files            *)
      (*********************************************)

      sqlfnentry =  RECORD        (* 24 B *)
            fnkanomin  : tsp_int2;  (* min kano in inclfile *)
            fnkanomax  : tsp_int2;  (* max kano in inclfile *)
            fnfilen    : tsp_knl_identifier;  (* file name   *)
            fnincllno  : tsp_int2;  (* incl lineno *)
      END;

      sqlfntype = ARRAY [ 1..mxpr_sqlfn ] OF sqlfnentry;
      sqlfnpointer  = ^sqlfntype;

      tpr00_OldAreas  =  RECORD
            sqlpap     : sqlparpointer;
            sqlkap     : sqlkapointer;
            sqlprp     : sqlprpointer;
            sqlstp     : sqlstpointer;
            sqlfap     : sqlfapointer;
            sqlatp     : sqlatpointer;
            sqlv1p    : sqlva1p;
            sqlv2p    : sqlva2p;
            sqlv3p    : sqlva3p;
            sqlcup    : sqlcupointer;
            sqlorp    : sqlorpointer;
            sqlfnp    : sqlfnpointer;
      END;

      tpr01_DescTypeEnum     = tsp00_Int4;
      tpr01_SQLStateBinary   = tsp00_Int4;
      tpr01_SQLAttribute     = tsp00_Int2;
      tpr00_ComTypeEnum      = tsp00_Int4;
      tpr00_DescribeKindEnum = tsp00_Int4;
      tpr00_TraceParseID_Enum = tsp00_Int4;
      tpr01_KeywordTypeEnum  = tsp00_Int4;
      tpr_gsymboltypes = tsp00_Int4;

      tpr01_ConDesc =  tsp_addr;

      tpr01_SQLDesc =  RECORD
            DescType : tpr01_DescTypeEnum;
            SQL       : tsp_addr;
            StmtNameDesc : tsp_addr;
            CursorDesc : tsp_addr;
            ErrorDesc  : tsp_addr;
            ConDesc    : tsp_addr;
            ComType : tpr00_ComTypeEnum;
            MessType : tsp1_cmd_mess_type;
            PacketEncoding : tpr05_StringEncoding;
            pr : ^sqlprentry;
            ka : ^sqlkaentry;
            ore : ^sqlorentry;
            cu : ^sqlcuentry;
            sqlxap : tsp_addr;
            State : tpr01_SQLStateBinary;
            Attribute : tpr01_SQLAttribute;
      END;

      (*********************************************)
      (*        S Q L C X A  - A R E A             *)
      (*  cxa-area   interne werte call-interf.    *)
      (*********************************************)

      sqlcxatype   =  RECORD   (* 92 B *)
      (* sqlcxa-anf *)
            xainit     : tsp_int2;    (* Modulinitalisen = true *)
            xacmdinit  : tsp_int2;
            xakano     : tsp_int2;    (* actueller sqlka index *)
            (* call-interf commandotype *)
            xaprogn    : tsp_knl_identifier;    (* programm-name *)
            xaprogc    : tsp_int2;    (* progcheck   *)
            xamodn     : tsp_knl_identifier;    (* modul-name  *)
            xamodc     : tsp_int2;    (* mod-check   *)
            xalang     : tsp_int2;    (* nach ra language    *)
            sqlModDesc : tsp_addr;
            xaSQLDesc : ^tpr01_SQLDesc;
            xacfetposc : tsp_int4;    (* fetch pos  const-value *)
            xaprno     : tsp_int2;    (* actuelle prno von kano*)
            xalcheck   : tsp_sname;   (* local hatzel_check    *)
            xaprname   : tsp_knl_identifier;    (* statementname *)
            (* call-interf. resultname *)
            xafiller1  : tsp_int2;
            xasqldap   : sqldaptrvrec;(* sqlda-area all_kind *)
            xacfetspec : tsp_int2;    (* search fetch specification  *)
            xacfiller  : tsp_c6;
      END;

      sqlcxapointer   = ^sqlcxatype;
      sqlstatetype     = PACKED ARRAY [ 1..6 ]  OF char;
      (**************************************)
      (*        S Q L C A  neu              *)
      (* Communication Area                 *)
      (* Fehlermeldungen von SQLDB an das    *)
      (* Hostprogramm                       *)
      (**************************************)

      sqlcatype =   RECORD         (* 448 B *)
            sqlenv  : tsp_int4;      (*  cobol init = 0  *)
            sqlcaid : PACKED ARRAY [1..8] OF char; (* bei Dumpanalyse *)
            sqlcabc  : tsp_int4;  (* Laenge der SQLCA, konstant ?? *)
            sqlcode  : tsp_int4;  (* SQLDB-Returncode        *)
            sqlerrml : tsp_int2;  (* Laenge der Fehlermeldung *)
            sqlerrmc : tsp_c70;   (* Meldungstext             *)
            sqlerrp  : tsp_c8;    (* connect_lockmode         *)
            sqlerrd  : ARRAY [ 1..6 ]  OF tsp_int4; (* ResCnt(3),Cost *)
            sqlwarn0 : char; (* Anzeige, ob Warnings ges., ggf. 'W' *)
            sqlwarn1 : char; (* alphanumerische Werte abgeschnitten *)
            sqlwarn2 : char; (* Bei Aggregierung NULL-Werte ignor.  *)
            sqlwarn3 : char; (* SELECT-Spal.zahl ungleich Paramzahl *)
            sqlwarn4 : char; (* UPDATE o. DELETE ?uber ganze Tabelle *)
            sqlwarn5 : char;
            sqlwarn6 : char;
            sqlwarn7 : char;
            sqlwarn8 : char;
            sqlwarn9 : char;
            sqlwarna : char;  (* row not found bei verteil. node down*)
            sqlwarnb : char;
            sqlwarnc : char;  (* rowno begrenzung *)
            sqlwarnd : char;
            sqlwarne : char;  (*select next,prev sec.key ?anderung *)
            sqlwarnf : char;  (* create table,index mit ibm syntax *)
            sqlext   : PACKED ARRAY [1..12] OF char; (* hatzel_check *)
            sqlresn : tsp_knl_identifier;    (* resultname after select-comm *)
            sqlfill1   : tsp_int2; (* empty                        *)
            sqlmfetch  : tsp_int2; (* 0:kein, 1:implicit, 18:option *)
            sqltermref : tsp_int4; (* Reference to virtual termial  *)
            sqlfill5   : tsp_int2; (* empty                        *)
            sqldbmode  : tsp_int2; (* SQLDB=1,ansi=3,saa/db2=2,orac=4*)
            sqldatetime: tsp_int2; (* SQLDB-datetime format :0 mormal*)
            sqlstate   : sqlstatetype; (*   6 B    *)
            sqlargl    : tsp4_argline;  (* option runtime argline 132 B*)
            sqlgap   : sqlgapointer;    (* global connect  *)
            sqlrap   : sqlrapointer;    (* runtime auftrag,sql or dia*)
            sqloap   : sqloapointer;    (* runtime optionen-area *)
            sqlmap   : sqlmapointer;    (* macro, varlang  *)
            sqlmfp   : sqlmfpointer;    (* mfetch-area     *)
            sqlplp   : tpr_intaddr;     (* empty           *)
            sqlemp   : sqlempointer;    (* tpr_sqlerror pointer *)
            sqlxadesc: tpr_sqlmdesc;
            sqlcxap   : sqlcxapointer;   (* pointer to sqlcxa *)
            sqlEnvDesc : tsp_addr;
      END;

      (*********************************************)
      (*        S Q L X A                          *)
      (*  Extent   Area                            *)
      (* Struktur aus Initialisierungs-Flag fuer   *)
      (* Hatzel-Check und modul globale Angaben    *)
      (* Pointer auf alle SQL-Areas                *)
      (*********************************************)

      sqlxatype = RECORD     (* 236 B *)
      (* sqlxa *)
            xainit     : tsp_int2;        (* Modulinitalisen = true *)
            xacmdinit  : tsp_int2;
            xakano     : tsp_int2;        (* actueller sqlka index *)
            (* call-interf commandotype *)
            xaprogn    : tsp_knl_identifier;        (* programm-name *)
            xaprogc    : tsp_int2;        (* progcheck   *)
            xamodn     : tsp_knl_identifier;        (* modul-name  *)
            xamodc     : tsp_int2;        (* mod-check   *)
            xalang     : tsp_int2;        (* nach ra language    *)
            xaModDesc : tsp_addr;
            xaSQLDesc : ^tpr01_SQLDesc;
            xaprno     : tsp_int2;         (* actuelle prno von kano*)
            xalcheck   : tsp_sname;        (* local hatzel_check    *)
            xaprname   : tsp_knl_identifier;         (* statementname *)
            (* call-interf. resultn  *)
            xastopcnt  : tsp_int2;         (* anz loop bei error -1 *)
            (* anz klammern precomp. *)
            xallen     : tsp_int2;         (* line length *)
            xaline     : tpr_sqlline;      (* line  52 B  *)
            xafiller2  : tsp_int2;
            xatime     : tsp_int4;         (* pc ret_syposacc*)
            xadescpos  : tsp_int2;         (* descrip.pos in varpart *)
            xadesclen  : tsp_int2;         (* descrip.len in varpart *)
            xaatcount : tsp_int2;          (* nur precompiler *)
            xaatmax   : tsp_int2;          (* max sqlatentrys, runtime *)
            xafiller3  : tsp_int4;
            sqlpap     : sqlparpointer;    (* parameter *)
            sqlpae     : sqlparpointer;    (* parameter *)
            sqlkap     : sqlkapointer;     (* kommando  *)
            sqlkae     : sqlkapointer;     (* kommando  *)
            sqlprp     : sqlprpointer;     (* prepare   *)
            sqlpre     : sqlprpointer;     (* prepare   *)
            sqlstp     : sqlstpointer;     (* statement *)
            sqlste     : sqlstpointer;     (* statement *)
            sqlfap     : sqlfapointer;     (* fetch     *)
            sqlfae     : sqlfapointer;     (* fetch     *)
            sqlatp     : sqlatpointer;     (* sqlat-area for oracle *)
            sqlate     : sqlatpointer;     (* sqlat-area for oracle *)
            sqlv1p    : sqlva1p;           (* pointer to sqlva1-area *)
            sqlv1e    : sqlva1p;           (* pointer to sqlva1-area *)
            sqlv2p    : sqlva2p;           (* pointer to sqlva2-area *)
            sqlv2e    : sqlva2p;           (* pointer to sqlva2-area *)
            sqlv3p    : sqlva3p;           (* pointer to sqlva3-area *)
            sqlv3e    : sqlva3p;           (* pointer to sqlva3-area *)
            sqlfill1  : tpr_intaddr;       (* unused *)
            sqlfill2  : tpr_intaddr;       (* unused *)
            sqlfill3  : tpr_intaddr;       (* unused *)
            sqlfill4  : tpr_intaddr;       (* unused *)
            sqlcup    : sqlcupointer;      (* sqlcuarea for oracle *)
            sqlcue    : sqlcupointer;      (* sqlcuarea for oracle *)
            sqlorp    : sqlorpointer;      (* sqlorarea for oracle *)
            sqlore    : sqlorpointer;      (* sqlorarea for oracle *)
            sqlfnp    : sqlfnpointer;      (* pointer to sqlfn-area *)
            sqlfne    : sqlfnpointer;      (* pointer to sqlfn-area *)
            sqlsnp    : tpr_sqlsn;     (* pointer to sqlsnarea *)
            sqlsndesc : tpr_sqlmdesc;      (* sqlsn-descriptor pointer *)
      END;

      (*********************************************)
      (*        LOOPRECORD                         *)
      (*  For sql array commands                   *)
      (* Structur to send several commands         *)
      (* to then kernel                            *)
      (*********************************************)

      tpr_sqlloop   = RECORD       (* 40 B *)
            lpmax   : tsp_int4;  (* maximale array count *)
            lpcnt   : tsp_int4;  (* max.actuell arr.count*)
            lperrd3 : tsp_int4;  (* max. sqlerrd 3 count *)
            lpnext  : tsp_int2;  (* cpr_is_true send com.to kern*)
            lpfiller: tsp_int2;  (*   empty              *)
            lperrd3i  : tsp_int4;  (* input acumulierte sqlerrd[3] *)
            lprescount: tsp_int4; (* only select into array  *)
            lpindi  : tsp_int4;  (* actuell input  index *)
            lpindo  : tsp_int4;  (* actuell output index *)
            lpfirsti: tsp_int2;  (* first input  call    *)
            lpfirsto: tsp_int2;  (* first output call    *)
            lprecli : tsp_int4;  (* recordlength input   *)
            lpreclo : tsp_int4;  (* recordlength output  *)
            lperri  : tpr_runtime_errors; (* error input     *)
            lperro  : tpr_runtime_errors; (* error output    *)
      END;

      (************************************************)
      (*   T Y P E N  des C A L L - I N T E R F A C E *)
      (*     beginnen mit sqc                         *)
      (************************************************)
      tpr_char_array = char;      (* 1.Byte eines char-arrays *)
      tpr_char_arrayp = ^tpr_char_array;
      tpr_string = PACKED ARRAY [ 1..2 ]  OF char;
      tpr05_CharBuffer = char;

      tpr05_String = RECORD
            rawString    : ^tpr05_CharBuffer;  (* contains the SQL-Statement *)
            encodingType : ^tsp_addr;          (* encoding type (ASCII, UCS2...) of szSQLStatement*)
            cbLen        : tsp00_Int4;         (* length of szSQLStatement in Byte *)
            cbAllocLen   : tsp00_Int4;         (* allocated length for szSQLStatement in Byte *)
      END;

      tpr05_StringPointer = ^tpr05_String;

      tpr05_StmtAnalysisDesc = RECORD
           StmtType   : tpr01_KeywordTypeEnum; (*first keyword of SQL-Statement (SELECT, UPDATE ...)*)
           StmtOption : tpr01_KeywordTypeEnum; (*option for a SQL-Statement (NEXT, FIRST ...)*)
           CursorPos  : tsp00_Int4;            (*position of hostvariable which contains cursorname*)
           CursorLength : tsp00_Int4;          (*length of hostvariable which contains cursorname*)
           CursorType : tpr_gsymboltypes;      (*TYPE of symbol where cursor come from (paramter, identifier...)*)
           FetchPosType : tpr_gsymboltypes;    (*TYPE of symbol (paramter, identifier...) which contains parameter for fetch [pos|relative|absolute...]*)
           UsingPos : tsp00_Int4;               (*position of using-clause in SQL-Statement*)
      END;

      tpc_keywords     = ARRAY [ 1..mxpc_keyword ]  OF tsp_sname;
      tpc_keywordindex = ARRAY [ 0..mxpc_keyword ]  OF tsp_int2;
      tpc_ptoc_template_dummy  = (
            dummy
            );



      tpc_prelinetype    = PACKED ARRAY [ 1..mxpr_preline ]  OF char;

      tpc_local_areas    = (
      a_sqlat,
      a_sqlcu,
      a_sqlda,
      a_sqldi,
      a_sqlfa,
      a_sqlfn,
      a_sqlka,
      a_sqlna,
      a_sqlop,
      a_sqlpa,
      a_sqlpr,
      a_sqlsa,
      a_sqlsd,
      a_sqlsd1,
      a_sqlst,
      a_sqlva,
      a_sqlva1,
      a_sqlva2,
      a_sqlva3,
      a_sqlxa);



      (*----------------------------------------------------------*)
      (*  typen      der precompiler                              *)
      (*----------------------------------------------------------*)



    tpc_pre_errors = (
                  cpc_pre_ok,
                  cpc_buffer_overflow,            (* -706 *)
                  cpc_connect_no_syntax_check,    (* -735 *)
                  cpc_const_declare_wrong,        (* -822 *)
                  cpc_const_too_large,            (* -832 *)
                  cpc_datatyp_not_compatible,     (* -817 *)
                  cpc_del_oracle_statement,       (* -762 *)
                  cpc_duplicate_fielddef,         (* -825 *)
                  cpc_duplicate_statementname,    (* -826 *)
                  cpc_duplicate_substructure_name,(* -724 *)
                  cpc_duplicate_typedef,          (* -827 *)
                  cpc_duplicate_vardef,           (* -828 *)
                  cpc_end_of_lineno_input,        (* -741 *)
                  cpc_extra_block_closure,        (* -738 *)
                  cpc_file_close_error,           (* -829 *)
                  cpc_file_open_error,            (* -830 *)
                  cpc_file_read_error,            (* -831 *)
                  cpc_file_write_error,           (* -832 *)
                  cpc_forloop_not_allowed,        (* -824 *)
                  cpc_include_area_on_wrong_position, (* -833 *)
                  cpc_include_file_exist,         (* -758 *)
                  cpc_include_file_produced,      (* -759 *)
                  cpc_includefile_allready_open,  (* -834 *)
                  cpc_includefile_open_error,     (* -835 *)
                  cpc_init_keyword_wrong,         (* -899 *)
                  cpc_invalid_cprep_command,      (* -836 *)
                  cpc_invalid_datatype,           (* -837 *)
                  cpc_invalid_declarator,         (* -838 *)
                  cpc_invalid_listname_expansion, (* -731 *)
                  cpc_invalid_macro_string,       (* -839 *)
                  cpc_invalid_macronumber,        (* -840 *)
                  cpc_invalid_option,             (* -841 *)
                  cpc_invalid_param_indicator,    (* -842 *)
                  cpc_invalid_parametername,      (* -843 *)
                  cpc_invalid_recordname,         (* -844 *)
                  cpc_invalid_rlistname_range,    (* -732 *)
                  cpc_invalid_section_begin_or_end,(* -886 *)
                  cpc_invalid_sql_command,        (* -845 *)
                  cpc_invalid_stmt_in_section,    (* -846 *)
                  cpc_invalid_type_definition,    (* -847 *)
                  cpc_invalid_unsignedinteger,    (* -848 *)
                  cpc_invalid_var_definition,     (* -849 *)
                  cpc_long_column_exist_in_table, (* -771 *)
                  cpc_loop_error_count,           (* -740 *)
                  cpc_macro_in_fetch,             (* -770 *)
                  cpc_missing_array,              (* -855 *)
                  cpc_missing_connect_user,       (* -882 *)
                  cpc_missing_dbsname,            (* -857 *)
                  cpc_missing_endsign,            (* -850 *)
                  cpc_missing_indicatorvariable,  (* -884 *)
                  cpc_missing_modulname,          (* -852 *)
                  cpc_nested_comment_not_closed,  (* -730 *)
                  cpc_no_macro_init,              (* -721 *)
                  cpc_no_output_variable,         (* -753 *)
                  cpc_no_pointer_allowed,         (* -734 *)
                  cpc_no_sql_statement_check,     (* -853 *)
                  cpc_only_question_marks_allowed,(* -854 *)
                  cpc_p_loop_init_wrong,          (* -856 *)
                  cpc_pointer_variable,           (* -733 *)
                  cpc_pre_cmd_not_available,      (* -101 *)
                  cpc_pre_not_implemented,        (* -808 *)
                  cpc_pre_reflex_start_required,
                  cpc_pre_request_area_overflow,  (* -706 *)
                  cpc_ptr_comp_not_allowed,       (* -769 *)
                  cpc_session_not_allowed,        (* -806 *)
                  cpc_source_code_truncated,      (* -729 *)
                  cpc_storage_class_not_allowed,  (* -858 *)
                  cpc_too_long_paramlist,         (* -859 *)
                  cpc_too_long_readline,          (* -889 *)
                  cpc_too_long_wheneverstring,    (* -887 *)
                  cpc_too_many_const_declared,    (* -860 *)
                  cpc_too_many_type_declared,     (* -870 *)
                  cpc_too_many_var_declared,      (* -871 *)
                  cpc_too_short_marend,           (* -890 *)
                  cpc_too_short_paramlist,        (* -872 *)
                  cpc_trace_param_no_struc_allowed,(* -894 *)
                  cpc_unknown_constname,           (* -873 *)
                  cpc_unknown_cursorname,          (* -874 *)
                  cpc_unknown_macronumber,         (* -876 *)
                  cpc_unknown_parametername,       (* -877 *)
                  cpc_unknown_recordname,          (* -878 *)
                  cpc_unknown_sql_command,         (* -879 *)
                  cpc_unknown_statementname,       (* -880 *)
                  cpc_unknown_typename,            (* -881 *)
                  cpc_usage_add_error,             (*-9809 *)
                  cpc_usage_on_off_error,          (*-9805 *)
                  cpc_variable_may_overflow,       (* -892 *)
                  cpc_variable_not_allowed_as_array,(* -763 *)
                  cpc_variable_will_be_truncated,   (* -885 *)
                  cpc_whole_table_del_upd,          (* -895 *)
                  cpc_macros_not_allowed            (* -780 *)
                  );

      tpc_cob_section = tsp_int2;

      tpc_vffileentry  = RECORD           (* 92 B *)
            vffileno  : tsp_int4;
            vffilen   : tsp00_VFilename;
            vffilemode : tsp5_rte_filemode;
            vfbinary  : boolean;
            vffeof    : boolean;
            vfreclen2 : tsp_int2;
            vfcnt     : tsp_int2;      (* line z?ahler *)
        (**** 3.0.00 lzu  neu *****)
            vfform    : tsp_vf_format;
            vfbufcount: tsp_int2;
            vffiller  : tsp_int2;
            vfblock   : tpr_intaddr;
            vfreclen4 : tsp_int4;
      END;

      tpc_vfentries   = ARRAY [ 1..mxpc_vffiles ] OF tpc_vffileentry;

      tpc_vffiletype = RECORD             (* 744 B *)
            vfacc     : tsp_int2;    (* actueller writefile index *)
            vfpoolprt : tsp_int4;
            vffn      : tpc_vfentries;
      END;



      tpc_tprelinerec  = RECORD
          llen    : tsp_int2;    (* gesamte l?ange *)
          lbegpos : tsp_int2;    (* zu untersuchende anf position *)
          lendpos : tsp_int2;    (* zuuntersuchende end position  *)
          laccpos : tsp_int2;    (* actuelle pos bei outpline     *)
          lline   : tpc_prelinetype; (* zeile *)
      END;

      tpc_analysetype  = RECORD      (* analyse informationen*)
          ancomtyp   : tsp_int2;     (* sql, dia, report *)
          ansection  : tsp_int2; (* actuelle section *)
          anbegin    : boolean;
          ansuccstatem : boolean;      (* folgestatement inpline *)
          ancheck    : tsp_int2;       (* opcheck angabe        *)
          anusage    : array [ 1..mxpr_sqlga ] of boolean;
                                        (* true : usage on sess 1 *)
      END;

      tpc_wheneverentry  = RECORD      (* whenever informationen*)
            whlen   : tsp_int2;    (* string length    *)
            whact   : tsp_int2;    (* sqlaction art stop, go, call  *)
            whstr   : tsp_c50;     (* sqlaction string    *)
            whfiller: tsp_int2;
      END;

      (*** 1:warning,  2:error,  3:excep,  4:not found ****)
      (*** 5:sqlbegin, 6:sqlend,                       ****)
      tpc_whenevertype = ARRAY [ 1..6 ]  of tpc_wheneverentry;
     (*********************************)
     (*  const sections angaben       *)
     (*********************************)

      tpc_endsentry   = RECORD      (* search ende type   *)
            ename : tsp_sname;
            enaml : tsp_int2;
      END;

      tpc_endstype   = RECORD      (* search end array  *)
            ecnt    : tsp_int4;  (* count            *)
            eend    : ARRAY  [ 1..5 ] of tpc_endsentry;
      END;


     (*********************************)
     (*  const sections angaben       *)
     (*********************************)

      tpc_const_entry   = RECORD      (* const section array  *)
            cname : tsp_lname;
            cwert : tsp_int2;
      END;

      tpc_csectionp = ARRAY [1..mxpc_const_sec] OF tpc_const_entry;

      tpc_const_sec  = RECORD      (* const section array  *)
            ccnt    : tsp_int2;    (* count            *)
            cbot    : tsp_int2;    (* first index      *)
            csec    : ^tpc_csectionp;
      END;

     (***********************************)
     (*  lno lineno auf source quelle   *)
     (***********************************)

     tpc_lnorecord  =  RECORD
         lnoinp  : tsp_int2;     (* cpr_is_true : input datei existiert *)
         lnoout  : tsp_int2;     (* cpr_is_true : output auf ur-quelle  *)
         lnoind  : tsp_int2; (* workfile  output actuell        *)
         lnolast : tsp_int2; (* workfile  output last           *)
         lnosrc  : tsp_int2; (* source line-no                  *)
         lnoexp  : tsp_int2; (* expandierte line-no             *)
         lnochr  : tsp_c12;  (* lnosrc, lnoexp als char       *)
      end;

 (* types for runtime interface code generation *)

 tpc_sqllatab          = record
 srccode : array [1..cpc_c_sqllatab         ] of tsp_c66;
 srclen  : array [1..cpc_c_sqllatab         ] of tsp_int4;
 end;
 tpc_prtscall          = record
 srccode : array [1..cpc_c_prtscall         ] of tsp_c66;
 srclen  : array [1..cpc_c_prtscall         ] of tsp_int4;
 end;
 tpc_fircon            = record
 srccode : array [1..cpc_c_fircon           ] of tsp_c66;
 srclen  : array [1..cpc_c_fircon           ] of tsp_int4;
 end;
 tpc_conv              = record
 srccode : array [1..cpc_c_conv             ] of tsp_c66;
 srclen  : array [1..cpc_c_conv             ] of tsp_int4;
 end;
 tpc_tptstcode         = record
 srccode : array [1..cpc_c_tptstcode        ] of tsp_c66;
 srclen  : array [1..cpc_c_tptstcode        ] of tsp_int4;
 end;
 tpc_loadl             = record
 srccode : array [1..cpc_c_loadl            ] of tsp_c66;
 srclen  : array [1..cpc_c_loadl            ] of tsp_int4;
 end;
 tpc_savel             = record
 srccode : array [1..cpc_c_savel            ] of tsp_c66;
 srclen  : array [1..cpc_c_savel            ] of tsp_int4;
 end;
 tpc_keeputm           = record
 srccode : array [1..cpc_c_keeputm          ] of tsp_c66;
 srclen  : array [1..cpc_c_keeputm          ] of tsp_int4;
 end;
 tpc_dosal             = record
 srccode : array [1..cpc_c_dosal            ] of tsp_c66;
 srclen  : array [1..cpc_c_dosal            ] of tsp_int4;
 end;
 tpc_deletl            = record
 srccode : array [1..cpc_c_deletl           ] of tsp_c66;
 srclen  : array [1..cpc_c_deletl           ] of tsp_int4;
 end;
 tpc_allocca           = record
 srccode : array [1..cpc_c_allocca          ] of tsp_c66;
 srclen  : array [1..cpc_c_allocca          ] of tsp_int4;
 end;
 tpc_rlsrc             = record
 srccode : array [1..cpc_c_rlsrc            ] of tsp_c66;
 srclen  : array [1..cpc_c_rlsrc            ] of tsp_int4;
 end;
 tpc_ca_end            = record
 srccode : array [1..cpc_c_ca_end           ] of tsp_c66;
 srclen  : array [1..cpc_c_ca_end           ] of tsp_int4;
 end;

 tpc_g_sqllatab        = record
 srccode : array [1..cpc_c_g_sqllatab       ] of tsp_c66;
 srclen  : array [1..cpc_c_g_sqllatab       ] of tsp_int4;
 srcflag : array [1..cpc_c_g_sqllatab       ] of char ;
 end;
 tpc_g_sqlcheck        = record
 srccode : array [1..cpc_c_g_sqlcheck       ] of tsp_c66;
 srclen  : array [1..cpc_c_g_sqlcheck       ] of tsp_int4;
 end;
 tpc_g_sqladext        = record
 srccode : array [1..cpc_c_g_sqladext       ] of tsp_c66;
 srclen  : array [1..cpc_c_g_sqladext       ] of tsp_int4;
 end;
 tpc_g_sqladpa         = record
 srccode : array [1..cpc_c_g_sqladpa        ] of tsp_c66;
 srclen  : array [1..cpc_c_g_sqladpa        ] of tsp_int4;
 end;
 tpc_g_sql2adpa         = record
 srccode : array [1..cpc_c_g_sql2adpa        ] of tsp_c66;
 srclen  : array [1..cpc_c_g_sql2adpa        ] of tsp_int4;
 srcflag : array [1..cpc_c_g_sql2adpa        ] of char;
 end;
 tpc_g_sqladkast       = record
 srccode : array [1..cpc_c_g_sqladkast      ] of tsp_c66;
 srclen  : array [1..cpc_c_g_sqladkast      ] of tsp_int4;
 end;
 tpc_g_perfutm         = record
 srccode : array [1..cpc_c_g_perfutm        ] of tsp_c66;
 srclen  : array [1..cpc_c_g_perfutm        ] of tsp_int4;
 end;
 tpc_g_sqlprts         = record
 srccode : array [1..cpc_c_g_sqlprts        ] of tsp_c66;
 srclen  : array [1..cpc_c_g_sqlprts        ] of tsp_int4;
 end;
 tpc_inlowval        = record
 srccode : array [1..cpc_c_g_inlowval       ] of tsp_c66;
 srclen  : array [1..cpc_c_g_inlowval       ] of tsp_int4;
 end;
 tpc_incaenv         = record
 srccode : array [1..cpc_c_g_incaenv        ] of tsp_c66;
 srclen  : array [1..cpc_c_g_incaenv        ] of tsp_int4;
 end;
 tpc_inheader        = record
 srccode : array [1..cpc_c_g_inheader       ] of tsp_c66;
 srclen  : array [1..cpc_c_g_inheader       ] of tsp_int4;
 srcflag : array [1..cpc_c_g_inheader       ] of char;
 end;
 tpc_invarinit       = record
 srccode : array [1..cpc_c_g_invarinit      ] of tsp_c66;
 srclen  : array [1..cpc_c_g_invarinit      ] of tsp_int4;
 srcflag : array [1..cpc_c_g_invarinit      ] of char;
 end;
 tpc_intail          = record
 srccode : array [1..cpc_c_g_intail         ] of tsp_c66;
 srclen  : array [1..cpc_c_g_intail         ] of tsp_int4;
 srcflag : array [1..cpc_c_g_intail         ] of char;
 end;
 tpc_g_appinit         = record
                       inheader   : tpc_inheader    ;
                       inlowval   : tpc_inlowval    ;
                       incaenv    : tpc_incaenv     ;
                       invarinit  : tpc_invarinit   ;
                       intail     : tpc_intail      ;
 end;
 tpc_g_tptst           = record
 srccode : array [1..cpc_c_g_tptst          ] of tsp_c66;
 srclen  : array [1..cpc_c_g_tptst          ] of tsp_int4;
 end;
 tpc_g_endloadl        = record
 srccode : array [1..cpc_c_g_endloadl       ] of tsp_c66;
 srclen  : array [1..cpc_c_g_endloadl       ] of tsp_int4;
 end;
 tpc_g_loadl           = record
 srccode : array [1..cpc_c_g_loadl          ] of tsp_c66;
 srclen  : array [1..cpc_c_g_loadl          ] of tsp_int4;
 end;
 tpc_g_savel           = record
 srccode : array [1..cpc_c_g_savel          ] of tsp_c66;
 srclen  : array [1..cpc_c_g_savel          ] of tsp_int4;
 end;
 tpc_g_dosal           = record
 srccode : array [1..cpc_c_g_dosal          ] of tsp_c66;
 srclen  : array [1..cpc_c_g_dosal          ] of tsp_int4;
 end;
 tpc_g_deletl          = record
 srccode : array [1..cpc_c_g_deletl         ] of tsp_c66;
 srclen  : array [1..cpc_c_g_deletl         ] of tsp_int4;
 end;
 tpc_ltheader          = record
 srccode : array [1..cpc_c_g_ltheader       ] of tsp_c66;
 srclen  : array [1..cpc_c_g_ltheader       ] of tsp_int4;
 srcflag : array [1..cpc_c_g_ltheader       ] of char;
 end;
 tpc_ltsqlext          = record
 srccode : array [1..cpc_c_g_ltsqlext       ] of tsp_c66;
 srclen  : array [1..cpc_c_g_ltsqlext       ] of tsp_int4;
 srcflag : array [1..cpc_c_g_ltsqlext       ] of char;
 end;
 tpc_ltsqlpa           = record
 srccode : array [1..cpc_c_g_ltsqlpa        ] of tsp_c66;
 srclen  : array [1..cpc_c_g_ltsqlpa        ] of tsp_int4;
 srcflag : array [1..cpc_c_g_ltsqlpa        ] of char;
 end;
 tpc_lt2sqlpa          = record
 srccode : array [1..cpc_c_g_lt2sqlpa       ] of tsp_c66;
 srclen  : array [1..cpc_c_g_lt2sqlpa       ] of tsp_int4;
 srcflag : array [1..cpc_c_g_lt2sqlpa       ] of char;
 end;
 tpc_ltkastate         = record
 srccode : array [1..cpc_c_g_ltkastate      ] of tsp_c66;
 srclen  : array [1..cpc_c_g_ltkastate      ] of tsp_int4;
 srcflag : array [1..cpc_c_g_ltkastate      ] of char;
 end;
 tpc_lttail            = record
 srccode : array [1..cpc_c_g_lttail         ] of tsp_c66;
 srclen  : array [1..cpc_c_g_lttail         ] of tsp_int4;
 srcflag : array [1..cpc_c_g_lttail         ] of char;
 end;
 tpc_g_latabind        = record
                       ltheader   : tpc_ltheader   ;
                       ltsqlext   : tpc_ltsqlext   ;
                       ltsqlpa    : tpc_ltsqlpa    ;
                       lt2sqlpa   : tpc_lt2sqlpa   ;
                       ltkastate  : tpc_ltkastate  ;
                       lttail     : tpc_lttail     ;
 end;
 tpc_g_rlsrc           = record
 srccode : array [1..cpc_c_g_rlsrc          ] of tsp_c66;
 srclen  : array [1..cpc_c_g_rlsrc          ] of tsp_int4;
 end;
 tpc_g_ca_end          = record
 srccode : array [1..cpc_c_g_ca_end         ] of tsp_c66;
 srclen  : array [1..cpc_c_g_ca_end         ] of tsp_int4;
 end;
 tpc_g_ca_do           = record
 srccode : array [1..cpc_c_g_ca_do          ] of tsp_c66;
 srclen  : array [1..cpc_c_g_ca_do          ] of tsp_int4;
 end;
 tpc_g_hpgt            = record
 srccode : array [1..cpc_c_g_hpgt           ] of tsp_c66;
 srclen  : array [1..cpc_c_g_hpgt           ] of tsp_int4;
 end;

 tpc_pass1codestruc = record
                  sqllatab  : tpc_sqllatab  ;
                  prtscall  : tpc_prtscall  ;
                  fircon    : tpc_fircon    ;
                  conv      : tpc_conv      ;
                  tptstcode : tpc_tptstcode ;
                  loadl     : tpc_loadl     ;
                  savel     : tpc_savel     ;
                  dosal     : tpc_dosal     ;
                  deletl    : tpc_deletl    ;
                  rlsrc     : tpc_rlsrc     ;
                  ca_end    : tpc_ca_end    ;
                  allocca   : tpc_allocca   ;
                  keeputm   : tpc_keeputm   ;
                  end;

 tpc_tpcodestruc =    record
                  stmt_sep  : char          ;
                  str_delim : char          ;
                  sqllatab  : tpc_g_sqllatab  ;
                  sqlcheck  : tpc_g_sqlcheck  ;
                  sqladext  : tpc_g_sqladext  ;
                  sqladpa   : tpc_g_sqladpa   ;
                  sql2adpa  : tpc_g_sql2adpa  ;
                  sqladkast : tpc_g_sqladkast ;
                  perfutm   : tpc_g_perfutm   ;
                  sqlprts   : tpc_g_sqlprts   ;
                  appinit   : tpc_g_appinit   ;
                  tptst     : tpc_g_tptst     ;
                  endloadl  : tpc_g_endloadl  ;
                  loadl     : tpc_g_loadl     ;
                  savel     : tpc_g_savel     ;
                  dosal     : tpc_g_dosal     ;
                  deletl    : tpc_g_deletl    ;
                  latabind  : tpc_g_latabind  ;
                  rlsrc     : tpc_g_rlsrc     ;
                  ca_end    : tpc_g_ca_end    ;
                  ca_do     : tpc_g_ca_do     ;
                  hpgt      : tpc_g_hpgt      ;
                  end;

 tpc_varent = record     (* Eintrag in Variablentabelle,  48 B *)
      vaname : tsp_lname;(* Name der Variablen *)
      vanaml : tsp_int2; (* Laenge des Namens  *)
      vacnt  : tsp_int2; (*Gesamtzahl der skal.Komp. einer Variablen *)
      vastrix: tsp_int2; (*Index in typtable: Vater-Struktur Komp.,0 *)
      vatypix: tsp_int2; (*Index in typtable: Typ-Beschr.der Variabln*)
      vasqlix: tsp_int2; (*Index in sqlva1  : sqlva beschr.der Variab*)
      vaglobl: tsp_int2; (* cpc_cconst= statische Var., 0 sonst*)
      vablockid: tsp_int2;   (* Block index der Variablen  *)
      vafiller : tsp_int2;   (*    empty                   *)
 end;

 tpc_typent = record (* Eintrag in Typ-Tabelle *)     (* 16 B *)
         tyref    : tsp_int2; (* Reference-Indikator: 1/0 falls *)
                   (* Variablen-Deklaration existiert oder nicht *)
         typtr    : tsp_int2; (* Pointer Kennung *)
         tydigit  : tsp_int2;
    case
    tyindi: tsp_int2 of  (* Typ-Indikator *)
      cpr_vstruct :           (* Struktur *)
           (
           tycmcnt: tsp_int2; (*Anzahl der Komponenten der Struktur *)
           tycmpix: tsp_int2; (*Ind.des ersten Komp-Index in cmpindex*)
           tyvacnt: tsp_int2; (*Gesamtzahl der skalaren Komponenten *)
           tyfill1: tsp_int2;
           );
      cpr_varray:  (* Array - Deskriptor fuer jede Dimension *)
           (
           tydim  : tsp_int4; (* Anzahl der Komponenten *)
           tyfix  : tsp_int2; (* Anfangsindex *)
           tytypix: tsp_int2; (*Index in typtable:Typ-Beschr.der Komp*)
           );
      cpr_vint2, cpr_vint4, cpr_vreal4, cpr_vreal8, cpr_vdecimal,
      cpr_vzoned, cpr_vuns2, cpr_vuns4,
      cpr_vchar, cpr_vcharc, cpr_vstring, cpr_vstring1, cpr_vlzone,
      cpr_vlszon, cpr_vtszon, cpr_vreal8sun, cpr_vucs2, cpr_vutf16,
      cpr_vunicode, cpr_vunicodec :
          (
           tysize : tsp_int4;(*Laenge des DB-Typs wie altes tlen *)
           tyfrac : tsp_int2;(*Skalier.des DB-Typs wie altest tfrac *)
           tyfill3: tsp_int2;
           );
 end;

 tpc_vartable = array [1..mxpc_varent] (* Variablentabelle *)
      of tpc_varent;    (* Eintrag von "echten" Variab.und Struk-komp. *)

 tpc_typtable = array [1..mxpc_typent] (* Typ-Tabelle *)
      of tpc_typent;    (*Eintrag der skalaren DB-Typen und Strukturen *)

 tpc_cmpindex = array [1..mxpc_varent]
                      (* Indizes der Strukturkomp.in vartab *)
      of tsp_int2;    (* wird nur ueber typtable referenziert *)

 tpc_vartablep = ^tpc_vartable;
 tpc_typtablep = ^tpc_typtable;
 tpc_cmpindexp = ^tpc_cmpindex;

 tpc_symtab = record (* Allgemeine Symboltabelle *)
         varcnt: tsp_int2; (* Aktuelle Belegungen *)
         varsav: tsp_int2; (* .. vor der letzten declare section *)
         varbot: tsp_int2; (* ..einer Variablen-Stack-Schicht (VPC32)*)
         typcnt: tsp_int2;
         cmpcnt: tsp_int2;
         pcfiller : tsp_int2;
         vartablep: tpc_vartablep; (* Zeiger zu Tablellen *)
         typtablep: tpc_typtablep;
         cmpindexp: tpc_cmpindexp;
 end;
     (***************************)
     (**    sqlnd entry        **)
     (**  NameDescriptor entry **)
     (**  neu hostvar analyse  **)
     (** alte ex_table erweitert*)
     (***************************)

   tpc_nd_entry  = RECORD
        ndvarmainix : tsp_int2; (* vartable index main structure *)
        ndvararrix  : tsp_int2; (* vartable index in array structure *)
        ndvarentix  : tsp_int2; (* vartable index sub structure  *)
        ndtypentix  : tsp_int2; (* typtable index of last struct *)
        ndmaintyindi: tsp_int2; (* typindi des varmain entry's   *)
        ndcompexpan : tsp_int2; (* componenten expansion is_true *)
        ndarraycnt  : tsp_int2; (*numb.of array indices in arrayix *)
        ndarrayix   : packed array [1..mxpc_ndarray] of tsp_int2;
        ndsqlva1st  : tsp_int4; (* sqlva1 index to struct  *)
        ndsqlva1ix  : tsp_int4; (* sqlva1 index of first element *)
        ndsqlva1cnt : tsp_int4; (* numb.of sqlva1 entries needed *)
        ndBlockId  : tsp_int2;
        ndkano     : tsp_int2;
        ndLoopPar  : tsp_int2;
        ndExpanlen : tsp_int2;  (* l?ange f?uer expansion *)
        ndCompIx   : tsp_int2;
        ndNamelen  : tsp_int2;
        ndNamePtr  : tpr_intaddr;
    END;

     tpc_arrndtype =  ARRAY [ 1..mxpc_sqlnd ] OF tpc_nd_entry;

     tpc_nd_entryp  =  ^tpc_arrndtype;


 tpc_pc_sqlva = RECORD    (* allgemeine sqlva_areas tables *)
         va1cnt   : tsp_int2; (* maximale belegung sqlva1 *)
         va2cnt   : tsp_int2; (* maximale belegung sqlva2 *)
         va3cnt   : tsp_int2; (* maximale belegung sqlva3 *)
         ndmax    : tsp_int2; (* maximale belegung sqlnd*)
         ndcnt    : tsp_int2; (* letzter nd entry f?ur vararbeitung *)
         ndfiller : tsp_int2;
         ndtabp   : tpc_nd_entryp;  (* pointer to tpc_nd_entry array *)
  END;

 tpc_dclclause = record
        dclattr : tsp_c132;
        dclattrl: tsp_int2;
        dclfnlen: tsp_int2;  (* file name length in [1] *)
 end;

 tpc_dclgen = record
        tabclause : tsp_int2;  (* cpc_i_table::table, i_dbproc::dbproc*)
        asclause : tsp_int2;
        indclause: tsp_int2;
        dcluser   : tsp_knl_identifier;
        dcltab    : tsp_knl_identifier;
        dclmodul  : tsp_knl_identifier;
        dcltabl   : tsp_int2;
        clause    : array [1..2] of tpc_dclclause;
 end;

 tpc_a_dclen   = array [1..2] of tsp_int2;

 tpc_pc_cics = RECORD          (* contains all variables needed in 1. *)
         progid   : tsp_name;(*an 2.pass.name of an extern.proced. *)
         partptst : tsp_name;(*name for a special test in cobol. *)
         cicscount : tsp_int4;
         iscics    : boolean;
         pass1     : boolean;
  END;

    tpc_pc_usage = RECORD        (* contains usage add infos *)
         uscom    : tsp_int2;     (* command nr us_empty = 0 *)
         usdobjn1 : tsp_knl_identifier;     (* defobjectname1  *)
         usfiller : tsp_int2;     (* empty           *)
         usdobjn2 : tsp_knl_identifier;     (* defobjectname2  *)
      END;


    tpc_partbuffer =  ARRAY [ 1..mxpr_partbuf ] OF char;
    tpc_partbufptr =  ^tpc_partbuffer;

    tpc_cmd_part = RECORD        (* record analyse kommando  *)
         part1len   : tsp_int4;     (* part1len  cmd len *)
         part2len   : tsp_int4;     (* part2len  zwischen len *)
         partmaxlen : tsp_int4;
         partsizlen : tsp_int4;
         partbufp   : tpc_partbufptr;  (* pointer auf partmaxlen buffer *)
      END;


   tpc_globals = RECORD    (* apc  19980 B *)
         pcerror   : tpc_pre_errors;      (* error *)
         pcerrcnt  : tsp_int2;        (* anzahl error z?ahler     *)
         pcwrncnt  : tsp_int2;        (* anzahl warnings z?ahler  *)
         pcxuserset: tsp4_xuserset;    (* 2B sqlarg3 xuserset for*.pcl*)
         pcendsymlen: tsp_int2;       (* ende adabas_kommando    *)
         pcendsym  : tsp_sname;       (* ende adabas_kommando    *)
         pcendcmd  : tsp_c2;          (* ende einer codezeile in 1*)
                                      (* gefundenes endzeich. in 2*)
         pcendse   :tpc_endstype;     (* 76B, end_search f?ur sqlstt*)
         pclineperpage: tsp_int4;     (* ausgabe listing         *)
         pccharperline: tsp_int4;     (* ausgabe listing         *)
         pcfilen   : tsp_knl_identifier;        (* parameter filename      *)
         pcfilet   : tsp_name;        (* parameter filetype      *)
         pcfilem   : tsp_name;        (* parameter filemodus     *)
         pcargs    : tsp4_argline;     (* 132B, args parameter inpline  *)
         pcexecsql : tsp_int2;         (* = 0 kein execsql command*)
         pcstmax    : tsp_int2;        (* maximal sqlst -area  *)
         pcststm    : tsp_int2;        (* max st-area of statem *)
         pcpaloop   : tsp_int4;        (* max-index for loop   *)
         pckaatind  : tsp_int2;        (* at index bevor ka found *)
         pckacuind  : tsp_int2;        (* cu index bevor ka found *)
         pcat       : tsp_int2;        (* sqlat-index          *)
         pccu       : tsp_int2;        (* sqlcu-index          *)
         pcatgacnt  : tsp_int2;        (* number of possible ga_entrys*)
         pcpa       : tsp_int2;        (* sqlpa-index  max     *)
         pcsa       : tsp_int2;        (* sqlsa-index  max     *)
         pcka       : tsp_int2;        (* sqlka-index  max     *)
         pcpr       : tsp_int2;        (* sqlpr-index  max     *)
         pcst       : tsp_int2;        (* sqlst-index  max     *)
         pcfa       : tsp_int2;        (* sqlfa-index  max     *)
         pcfn       : tsp_int2;        (* sqlfn-index  max     *)
         pcdi       : tsp_int2;        (* sqldi-index  max     *)
         pcna       : tsp_int2;        (* sqlna-index  max     *)
         pcop       : tsp_int2;        (* sqlop-index  max     *)
         pcda       : tsp_int2;        (* sqlda-index  max     *)
         pcbnddsc   : tsp_int2;        (* bnddsc-index max     *)
         pcseldsc   : tsp_int2;        (* seldsc-index max     *)
         pcopts    : tsp4_args_options; (* precompiler optionen *)
         pcvf      : tpc_vffiletype;   (* 744B, interne  files   *)
         pccodeind : tsp_int2;         (* actueller codefileindex *)
         pcinpind  : tsp_int2;         (* actueller inputfileindex*)
         pcinpline : tpc_tprelinerec;  (* 522B, eine programmzeile   *)
         pcoutline : tpc_tprelinerec;  (* 522B,eine code outputline *)
         pcinpeof  : boolean;          (* inpmodule eof        *)
         pcsqlcom  : boolean;          (* true read reflex_com *)
         pccomment : boolean;          (* true comment in nextline *)
         pcscan    : tpr_scannertype;  (* 14B, syntaxerkennung      *)
         pcan      : tpc_analysetype;  (* 16B, analyse informationen*)
         pccobsect : tpc_cob_section;  (* 2B, Info, ueber die Sektion *)
                                       (* eines COBOL-Programms *)
         pcwhe     : tpc_whenevertype; (* 336B, whenever informationen*)
         pcwheset  : tsp_int2;         (* whenever true : set   *)
         pcwheno   : tsp_int2;         (* whenever nummer       *)
         pccse     : tpc_const_sec;    (* 8B, const section array   *)
         pccmcnt   : tsp_int2;         (* nested comment count  *)
         pclno     : tpc_lnorecord;    (* lno output record    *)
         sqlca     : sqlcatype;        (* 448B *)
         sqlxa     : sqlxatype;        (* 236B *)
         pcsymtab  : tpc_symtab;       (* 24B, general tpr_symbol table *)
         pcsqlva   : tpc_pc_sqlva;     (* 16B, general sqlva_areas tables *)
         pcdclgen  : tpc_dclgen;       (* 230B, param.struc. for incl.table *)
         pccics    : tpc_pc_cics ;     (* 44B, comnicat.between 1.and 2.pass*)
         pcusage   : tpc_pc_usage;     (* 40B, usage add record  *)
         pccmdpart : tpc_cmd_part;     (* 32776B, for analyse kommando *)
   END;


PROCEDURE
              m90c30 (
                    layer : tsp_layer;
                    str30 : tsp_c30);
        EXTERNAL ;



        PROCEDURE
              m90int (layer : tsp_layer;
                    nam : tsp_sname;
                    int : integer);
        EXTERNAL ;



        PROCEDURE
              m90int4 (layer : tsp_layer;
                    nam : tsp_sname;
                    int  : tsp_int4);
        EXTERNAL ;



        PROCEDURE
              m90int2 (layer : tsp_layer;
                    nam : tsp_sname;
                    int  : tsp_int2);
        EXTERNAL ;



        PROCEDURE
              m90name (layer : tsp_layer;
                    nam : tsp_name);
        EXTERNAL ;



        PROCEDURE
              m90lname (layer : tsp_layer;
                    nam : tsp_lname);
        EXTERNAL ;



        PROCEDURE
              m90identifier  (
                    layer : tsp_layer;
                    nam   : tsp_knl_identifier);
        EXTERNAL ;



        PROCEDURE
              m90buf (layer : tsp_layer;
                    VAR buf : char;
                    pos_anf : integer;
                    pos_end : integer);
        EXTERNAL ;



      PROCEDURE
              p14batdatabase (VAR apc : tpc_globals;
                    VAR dbname : tsp_knl_identifier);
        EXTERNAL ;



        PROCEDURE
              p14bcudeclare ( VAR apc : tpc_globals;
                    VAR cursorn : tsp_knl_identifier);
        EXTERNAL ;



        PROCEDURE
              p14bcusearch (VAR apc : tpc_globals;
                    VAR buf : tpc_partbuffer;
                    blen : integer);
        EXTERNAL ;



      FUNCTION
              p14symb (
                    PROCEDURE next_symbol (VAR p1: tpc_globals;
                    VAR p2: tpc_partbuffer; p3: integer);
                    VAR apc : tpc_globals) : tpr_symbol;
        EXTERNAL ;



        PROCEDURE
              p14binitsqlvan (VAR apc : tpc_globals);
        EXTERNAL ;



        PROCEDURE
              p14ansqlstatement (
                    PROCEDURE next_symbol (VAR p1: tpc_globals;
                    VAR p2: tpc_partbuffer; p3: integer);
                    VAR apc : tpc_globals);
        EXTERNAL ;



        PROCEDURE
              p14an1sqlstatement (
                    PROCEDURE next_symbol (VAR p1: tpc_globals;
                    VAR p2: tpc_partbuffer; p3: integer);
                    VAR apc : tpc_globals);
        EXTERNAL ;



        PROCEDURE
              p14anaparameter (
                    PROCEDURE next_symbol (VAR p1: tpc_globals;
                    VAR p2: tpc_partbuffer; p3: integer);
                    VAR apc : tpc_globals;
                    VAR pos : tsp_int4;
                    VAR nextsa : boolean);
        EXTERNAL ;



        PROCEDURE
              p14btypentget (
                    PROCEDURE next_symbol (VAR p1: tpc_globals;
                    VAR p2: tpc_partbuffer; p3: integer);
                    VAR apc : tpc_globals;
                    VAR typentix : tsp_int2;
                    VAR vatyp    : tsp_int2;
                    VAR lastvatyp: tsp_int2;
                    resolve      : tsp_int2);
        EXTERNAL ;



        PROCEDURE
              p14putsqlstarea   (VAR apc : tpc_globals);
        EXTERNAL ;



      PROCEDURE
              p16tabledclgen   (VAR apc : tpc_globals;
                    VAR dclgen : boolean);
        EXTERNAL ;



      PROCEDURE
              p19sqlkaallocate (VAR index : tsp_int2);
        EXTERNAL ;



        PROCEDURE
              p19sqlprallocate (VAR index : tsp_int2);
        EXTERNAL ;



        PROCEDURE
              p19sqlfaallocate (VAR index : tsp_int2);
        EXTERNAL ;



        PROCEDURE
              p19sqlstallocate (VAR index : tsp_int2);
        EXTERNAL ;



        PROCEDURE
              p19sqlfnallocate (VAR index : tsp_int2);
        EXTERNAL ;



        PROCEDURE
              p19setprsqlindex (VAR index : tsp_int2);
        EXTERNAL ;



        PROCEDURE
              p19setsqlstindex (VAR index : tsp_int2);
        EXTERNAL ;



        PROCEDURE
              p19cparentry  (VAR apc : tpc_globals;
                    kindentry: tsp_int2;
                    parm2 : tsp_int4;
                    parm3 : tsp_int4;
                    parm4 : tsp_int4);
        EXTERNAL ;



      PROCEDURE
              p19getname (VAR apc : tpc_globals;
                    VAR  nam  : tsp_knl_identifier;
                    VAR  len  : integer);
        EXTERNAL ;



        PROCEDURE
              p19sprepareprname (VAR apc : tpc_globals;
                    VAR  nam  : tsp_knl_identifier;
                    VAR  ind  : integer);
        EXTERNAL ;



        PROCEDURE
              p19sstatementprname (VAR apc : tpc_globals;
                    VAR  nam  : tsp_knl_identifier;
                    VAR  ind  : integer);
        EXTERNAL ;



        PROCEDURE
              p19scursorprname (VAR apc : tpc_globals;
                    VAR  nam  : tsp_knl_identifier;
                    VAR  ind  : integer);
        EXTERNAL ;



        PROCEDURE
              p19snextsubchar (VAR apc : tpc_globals;
                    anf : integer;
                    VAR pos : integer;
                    VAR subchr : char);
        EXTERNAL ;



        PROCEDURE
              p19gmacronumber  (VAR apc : tpc_globals;
                    mpos  : integer;
                    VAR mind : integer;
                    VAR nextpos : integer);
        EXTERNAL ;



      PROCEDURE
              p10getkeyword (VAR apc : tpc_globals;
                    VAR buf : tpc_partbuffer;
                    VAR index : integer);
        EXTERNAL ;



        PROCEDURE
              p10stringliteralget (VAR apc : tpc_globals;
                    VAR buf : tpc_partbuffer);
        EXTERNAL ;



        PROCEDURE
              p10int2unsignedget (VAR apc : tpc_globals;
                    VAR buf : tpc_partbuffer;
                    VAR int : tsp_int2);
        EXTERNAL ;



        PROCEDURE
              p10rmspace (VAR buf : tpc_partbuffer;
                    cpr_quotsym, escsym : char;
                    ind, inplen : tsp_int4;
                    VAR  maxbuf, outlen : tsp_int4);
        EXTERNAL ;



      PROCEDURE
              p11vfopen  (VAR apc : tpc_globals;
                    ind  :  integer);
        EXTERNAL ;



        PROCEDURE
              p11precomerror (VAR apc : tpc_globals;
                    error : tpc_pre_errors);
        EXTERNAL ;



        PROCEDURE
              p11wrerrorlisting  (VAR apc : tpc_globals;
                    error : tpc_pre_errors);
        EXTERNAL ;



        PROCEDURE
              p11nerrorlisting  (VAR apc : tpc_globals;
                    VAR nam : tsp_lname;
                    naml  : tsp_int2;
                    index : tsp_int2);
        EXTERNAL ;



      PROCEDURE
              pr01eInitFileName(VAR filename : tsp00_VFilename);
        EXTERNAL ;



        PROCEDURE
              p14putval (
                    PROCEDURE next_symbol (VAR p1: tpc_globals;
                    VAR p2: tpc_partbuffer; p3: integer);
                    VAR apc : tpc_globals;
                    VAR keyind : integer;
                    VAR pos : tsp_int4);
        EXTERNAL ;



        PROCEDURE
              p14getval (
                    PROCEDURE next_symbol (VAR p1: tpc_globals;
                    VAR p2: tpc_partbuffer; p3: integer);
                    VAR apc : tpc_globals;
                    VAR keyind : integer;
                    VAR pos : tsp_int4);
        EXTERNAL ;



      PROCEDURE
              p05up1casebuf (VAR buf : tsp_c132;
                    lwb : tsp_int4;
                    upb : tsp_int4);
        EXTERNAL ;



        PROCEDURE
              p05up2casebuf (VAR buf : tsp_knl_identifier;
                    lwb : tsp_int4;
                    upb : tsp_int4);
        EXTERNAL ;



        PROCEDURE
              p05searchword  (VAR buf : tpc_partbuffer;
                    lwb : integer;
                    upb : integer;
                    word : tsp_sname;
                    lw   : integer;
                    VAR pos : integer);
        EXTERNAL ;



        PROCEDURE
              p05inttochr12 (int : integer;
                    VAR chr12 : tsp_c12);
        EXTERNAL ;



      PROCEDURE
              s10fil (size : tsp_int4;
                    VAR m : tpc_partbuffer;
                    pos : tsp_int4;
                    len : tsp_int4;
                    fillchar : char);
        EXTERNAL ;



        PROCEDURE
              s10fil1 (size : tsp_int4;
                    VAR m : tpr_macroline;
                    pos : tsp_int4;
                    len : tsp_int4;
                    fillchar : char);
        EXTERNAL ;



        PROCEDURE
              s10fil2 (size : tsp_int4;
                    VAR m : tsp_c132;
                    pos : tsp_int4;
                    len : tsp_int4;
                    fillchar : char);
        EXTERNAL ;



        PROCEDURE
              s10mv1 (size1 : tsp_int4; size2 : tsp_int4;
                    VAR val1 : tpc_partbuffer; p1 : tsp_int4;
                    VAR val2 : tpr_sqlline; p2  :tsp_int4;
                    anz: tsp_int4);
        EXTERNAL ;



        PROCEDURE
              s10mv2 (size1 : tsp_int4; size2 : tsp_int4;
                    VAR val1 : tsp_c12; p1 : tsp_int4;
                    VAR val2 : tsp_name; p2  :tsp_int4;
                    anz: tsp_int4);
        EXTERNAL ;



        PROCEDURE
              s10mv3 (size1 : tsp_int4; size2 : tsp_int4;
                    VAR val1 : tpc_partbuffer; p1  :tsp_int4;
                    VAR val2 : tsp_knl_identifier; p2 : tsp_int4;
                    anz: tsp_int4);
        EXTERNAL ;



        PROCEDURE
              s10mv4 (size1 : tsp_int4; size2 : tsp_int4;
                    VAR val1 : tpc_partbuffer; p1 : tsp_int4;
                    VAR val2 : tsp_c50; p2  :tsp_int4;
                    anz: tsp_int4);
        EXTERNAL ;



        PROCEDURE
              s10mv5 (size1 : tsp_int4; size2 : tsp_int4;
                    VAR val1 : tpc_partbuffer; p1 : tsp_int4;
                    VAR val2 : tsp00_VFilename; p2  :tsp_int4;
                    anz: tsp_int4);
        EXTERNAL ;



        PROCEDURE
              s10mv6 (size1 : tsp_int4; size2 : tsp_int4;
                    VAR val1 : tpc_partbuffer; p1 : tsp_int4;
                    VAR val2 : tpr_macroline; p2  :tsp_int4;
                    anz: tsp_int4);
        EXTERNAL ;



        PROCEDURE
              s10mv7 (size1 : tsp_int4; size2 : tsp_int4;
                    VAR val1 : tsp_lname; p1 : tsp_int4;
                    VAR val2 : tsp_name; p2  :tsp_int4;
                    anz: tsp_int4);
        EXTERNAL ;



        PROCEDURE
              s10mv8 (size1 : tsp_int4; size2 : tsp_int4;
                    VAR val1 : tsp_knl_identifier; p1 : tsp_int4;
                    VAR val2 : tsp_knl_identifier; p2  :tsp_int4;
                    anz: tsp_int4);
        EXTERNAL ;



        PROCEDURE
              s10mv9 (size1 : tsp_int4; size2 : tsp_int4;
                    VAR val1 : tpc_partbuffer; p1 : tsp_int4;
                    VAR val2 : tsp_c132; p2  :tsp_int4;
                    anz: tsp_int4);
        EXTERNAL ;



        PROCEDURE
              s10mv10 (size1 : tsp_int4; size2 : tsp_int4;
                    VAR val1 : tsp_knl_identifier; p1 : tsp_int4;
                    VAR val2 : tsp_c132; p2  :tsp_int4;
                    anz: tsp_int4);
        EXTERNAL ;



        PROCEDURE
              s10mv11 (size1 : tsp_int4; size2 : tsp_int4;
                    VAR val1 : tsp_sname; p1 : tsp_int4;
                    VAR val2 : tpc_partbuffer; p2  :tsp_int4;
                    anz: tsp_int4);
        EXTERNAL ;



        PROCEDURE
              s10mv12 (size1 : tsp_int4; size2 : tsp_int4;
                    VAR val1 : tsp00_VFilename; p1 : tsp_int4;
                    VAR val2 : tpc_partbuffer; p2  :tsp_int4;
                    anz: tsp_int4);
        EXTERNAL ;



        PROCEDURE
              s10mv13 (size1 : tsp_int4; size2 : tsp_int4;
                    VAR val1 : tsp_knl_identifier; p1 : tsp_int4;
                    VAR val2 : tpc_partbuffer; p2  :tsp_int4;
                    anz: tsp_int4);
        EXTERNAL ;



        PROCEDURE
              s10mv14 (size1 : tsp_int4; size2 : tsp_int4;
                    VAR val1 : tsp_c132; p1 : tsp_int4;
                    VAR val2 : tpc_partbuffer; p2  :tsp_int4;
                    anz: tsp_int4);
        EXTERNAL ;



        PROCEDURE
              s10mv15 (size1 : tsp_int4; size2 : tsp_int4;
                    VAR val1 : tpc_partbuffer; p1 : tsp_int4;
                    VAR val2 : tsp_dbname; p2  :tsp_int4;
                    anz: tsp_int4);
        EXTERNAL ;



        PROCEDURE
              s10mv16 (size1 : tsp_int4; size2 : tsp_int4;
                    VAR val1 : tpc_partbuffer; p1 : tsp_int4;
                    VAR val2 : tsp_nodeid; p2  :tsp_int4;
                    anz: tsp_int4);
        EXTERNAL ;



        PROCEDURE
              s10mv17 (size1 : tsp_int4; size2 : tsp_int4;
                    VAR val1 : tsp00_VFilename; p1 : tsp_int4;
                    VAR val2 : tsp_knl_identifier; p2  :tsp_int4;
                    anz: tsp_int4);
        EXTERNAL ;



        PROCEDURE
              s10mv18 (size1 : tsp_int4; size2 : tsp_int4;
                    VAR val1 : tsp_c12;  p1  :tsp_int4;
                    VAR val2 : tsp_knl_identifier; p2 : tsp_int4;
                    anz: tsp_int4);
        EXTERNAL ;



        PROCEDURE
              s10mvr  (size1 : tsp_int4; size2 : tsp_int4;
                    VAR val1 : tpc_partbuffer; p1 : tsp_int4;
                    VAR val2 : tpc_partbuffer; p2  :tsp_int4;
                    anz: tsp_int4);
        EXTERNAL ;



      FUNCTION
              s30eq (VAR a : tsp_sname;
                    VAR b : tpc_partbuffer;
                    bi,cnt : tsp_int4) : boolean;
        EXTERNAL ;



        FUNCTION
              s30len (VAR str : tsp00_VFilename;
                    val : char; cnt : tsp_int4) : tsp_int4;
        EXTERNAL ;



        FUNCTION
              s30len1 (VAR str : tsp_knl_identifier;
                    val : char; cnt : tsp_int4) : tsp_int4;
        EXTERNAL ;



        FUNCTION
              s30lnr (VAR str : tsp_c132;
                    val : char;
                    start : tsp_int4;
                    cnt : tsp_int4) : tsp_int4;
        EXTERNAL ;



PROCEDURE m90print(tt:tsp_trace);EXTERNAL;

PROCEDURE
      p14end (
            PROCEDURE next_symbol (VAR p1: tpc_globals;
            VAR p2: tpc_partbuffer; p3: integer);
            VAR apc : tpc_globals);
        FORWARD;

PROCEDURE
      p14sqlfn_areaput (VAR apc : tpc_globals);
        FORWARD;

PROCEDURE
      p14oracle  (
            PROCEDURE next_symbol (VAR p1: tpc_globals;
            VAR p2: tpc_partbuffer; p3: integer);
            VAR apc : tpc_globals);
        FORWARD;

PROCEDURE
      p14_put_dclgen   (VAR apc : tpc_globals);
        FORWARD;

PROCEDURE
      p14open  (
            PROCEDURE next_symbol (VAR p1: tpc_globals;
            VAR p2: tpc_partbuffer; p3: integer);
            VAR apc : tpc_globals;
            VAR openno : integer);
        FORWARD;

PROCEDURE
      p14include   (
            PROCEDURE next_symbol (VAR p1: tpc_globals;
            VAR p2: tpc_partbuffer; p3: integer);
            VAR apc : tpc_globals;
            VAR keyind: integer;
            VAR dclgen : boolean);
        FORWARD;

PROCEDURE
      p14begin (
            PROCEDURE next_symbol (VAR p1: tpc_globals;
            VAR p2: tpc_partbuffer; p3: integer);
            VAR apc : tpc_globals);
        FORWARD;

PROCEDURE
      p14command  (
            PROCEDURE next_symbol (VAR p1: tpc_globals;
            VAR p2: tpc_partbuffer; p3: integer);
            VAR apc : tpc_globals);
        FORWARD;

PROCEDURE
      p14cancel  (
            PROCEDURE next_symbol (VAR p1: tpc_globals;
            VAR p2: tpc_partbuffer; p3: integer);
            VAR apc : tpc_globals);
        FORWARD;

PROCEDURE
      p14abortsession (
            PROCEDURE next_symbol (VAR p1: tpc_globals;
            VAR p2: tpc_partbuffer; p3: integer);
            VAR apc : tpc_globals);
        FORWARD;

PROCEDURE
      p14version  (
            PROCEDURE next_symbol (VAR p1: tpc_globals;
            VAR p2: tpc_partbuffer; p3: integer);
            VAR apc : tpc_globals);
        FORWARD;

PROCEDURE
      p14_hostv_get  (
            PROCEDURE next_symbol (VAR p1: tpc_globals;
            VAR p2: tpc_partbuffer; p3: integer);
            VAR apc : tpc_globals;
            VAR paix : tsp_int2;
            VAR va1ix : tsp_int2);
        FORWARD;

PROCEDURE
      p14_prind_get  (
            PROCEDURE next_symbol (VAR p1: tpc_globals;
            VAR p2: tpc_partbuffer; p3: integer);
            VAR apc : tpc_globals;
            VAR prind : integer);
        FORWARD;

PROCEDURE
      p14_add_prind  (
            VAR apc : tpc_globals;
            VAR prind : integer;
            VAR paix  : tsp_int2);
        FORWARD;

PROCEDURE
      p14prepare  (
            PROCEDURE next_symbol (VAR p1: tpc_globals;
            VAR p2: tpc_partbuffer; p3: integer);
            VAR apc : tpc_globals);
        FORWARD;

PROCEDURE
      p14execute  (
            PROCEDURE next_symbol (VAR p1: tpc_globals;
            VAR p2: tpc_partbuffer; p3: integer);
            VAR apc : tpc_globals;
            VAR statemno : integer;
            VAR immediate: integer);
        FORWARD;

PROCEDURE
      p14declare  (
            PROCEDURE next_symbol (VAR p1: tpc_globals;
            VAR p2: tpc_partbuffer; p3: integer);
            VAR apc : tpc_globals;
            VAR keyindex : integer;
            VAR statemno : integer);
        FORWARD;

PROCEDURE
      p14describe (
            PROCEDURE next_symbol (VAR p1: tpc_globals;
            VAR p2: tpc_partbuffer; p3: integer);
            VAR apc : tpc_globals);
        FORWARD;

PROCEDURE
      p14close  (
            PROCEDURE next_symbol (VAR p1: tpc_globals;
            VAR p2: tpc_partbuffer; p3: integer);
            VAR apc : tpc_globals);
        FORWARD;

PROCEDURE
      p14forloop  (
            PROCEDURE next_symbol (VAR p1: tpc_globals;
            VAR p2: tpc_partbuffer; p3: integer);
            VAR apc : tpc_globals);
        FORWARD;

PROCEDURE
      p14fetch  (
            PROCEDURE next_symbol (VAR p1: tpc_globals;
            VAR p2: tpc_partbuffer; p3: integer);
            VAR apc : tpc_globals;
            VAR keyind : integer;
            VAR pos : tsp_int4);
        FORWARD;

PROCEDURE
      p14whenever  (
            PROCEDURE next_symbol (VAR p1: tpc_globals;
            VAR p2: tpc_partbuffer; p3: integer);
            VAR apc : tpc_globals);
        FORWARD;

PROCEDURE
      p14_get_action (
            PROCEDURE next_symbol (VAR p1: tpc_globals;
            VAR p2: tpc_partbuffer; p3: integer);
            VAR apc : tpc_globals;
            ind : integer);
        FORWARD;

PROCEDURE
      p14gmacrono (
            PROCEDURE next_symbol (VAR p1: tpc_globals;
            VAR p2: tpc_partbuffer; p3: integer);
            VAR apc : tpc_globals;
            VAR  macnr : tsp_int2);
        FORWARD;

PROCEDURE
      p14nolog (
            PROCEDURE next_symbol (VAR p1: tpc_globals;
            VAR p2: tpc_partbuffer; p3: integer);
            VAR apc : tpc_globals);
        FORWARD;

PROCEDURE
      p14option  (
            PROCEDURE next_symbol (VAR p1: tpc_globals;
            VAR p2: tpc_partbuffer; p3: integer);
            VAR apc : tpc_globals;
            VAR statemno : integer);
        FORWARD;

PROCEDURE
      p14gdbsname (
            PROCEDURE next_symbol (VAR p1: tpc_globals;
            VAR p2: tpc_partbuffer; p3: integer);
            VAR apc : tpc_globals;
            VAR ind : integer;
            VAR dbpos : integer;
            VAR dblen : integer;
            VAR dbnodepos : integer;
            VAR dbnodelen : integer);
        FORWARD;

PROCEDURE
      p14gtrace (
            PROCEDURE next_symbol (VAR p1: tpc_globals;
            VAR p2: tpc_partbuffer; p3: integer);
            VAR apc : tpc_globals;
            keyind: integer);
        FORWARD;

PROCEDURE
      p14traceline (
            PROCEDURE next_symbol (VAR p1: tpc_globals;
            VAR p2: tpc_partbuffer; p3: integer);
            VAR apc : tpc_globals;
            VAR statemno : integer);
        FORWARD;

FUNCTION
      p14ifstbind   (VAR apc : tpc_globals;
            kano : integer) : boolean;
        FORWARD;

PROCEDURE
      p14_dclgen   (
            PROCEDURE next_symbol (VAR p1: tpc_globals;
            VAR p2: tpc_partbuffer; p3: integer);
            VAR apc : tpc_globals;
            VAR dclgen : boolean;
            VAR fnlen  : integer);
        FORWARD;

PROCEDURE
      p14getname (
            PROCEDURE next_symbol (VAR p1: tpc_globals;
            VAR p2: tpc_partbuffer; p3: integer);
            VAR apc : tpc_globals;
            VAR  nam  : tsp_knl_identifier;
            VAR  len  : integer;
            VAR  macro: tsp_int2
            ;
            VAR  param: tsp_int2
            );
        FORWARD;

PROCEDURE
      p14oradescribe (
            PROCEDURE next_symbol (VAR p1: tpc_globals;
            VAR p2: tpc_partbuffer; p3: integer);
            VAR apc : tpc_globals;
            VAR orakatyp : tsp_int2);
        FORWARD;

PROCEDURE
      p14kaentryinit (VAR apc : tpc_globals);
        FORWARD;

PROCEDURE
        p14end ;



VAR
      keyind : integer;
      i      : integer;

BEGIN
m90print('PC14  p14end    ');
WITH apc, pcscan, pccmdpart DO
    BEGIN
    IF   pcerror = cpc_pre_ok
    THEN
        BEGIN
        next_symbol (apc, partbufp^, part1len);
        p10getkeyword (apc, partbufp^, keyind);
        IF   keyind = cpc_i_declare
        THEN
            BEGIN
            next_symbol (apc, partbufp^, part1len);
            p10getkeyword (apc, partbufp^, keyind);
            CASE keyind OF
                cpc_i_const :
                    BEGIN
                    IF   pcan.ansection <> cpc_se_const
                    THEN
                        p11wrerrorlisting (apc, cpc_invalid_section_begin_or_end);
                    (*ENDIF*)
                    pcan.ansection := cpc_se_off;
                    next_symbol (apc, partbufp^, part1len);
                    p10getkeyword (apc, partbufp^, keyind);
                    IF   keyind <> cpc_i_section
                    THEN
                        p11wrerrorlisting (apc,cpc_unknown_sql_command);
                    (*ENDIF*)
                    FOR i := 1 TO pccse.ccnt DO
                        WITH pccse, csec^ [i]  DO
                            BEGIN
                            m90lname (pc, cname);
                            m90int4 (pc, 'cwert       ', cwert);
                            END;
                        (*ENDWITH*)
                    (*ENDFOR*)
                    END;
                cpc_i_type  :
                    BEGIN
                    IF   pcan.ansection <> cpc_se_type
                    THEN
                        p11wrerrorlisting (apc, cpc_invalid_section_begin_or_end);
                    (*ENDIF*)
                    pcan.ansection := cpc_se_off;
                    next_symbol (apc, partbufp^, part1len);
                    p10getkeyword (apc, partbufp^, keyind);
                    IF   keyind <> cpc_i_section
                    THEN
                        p11wrerrorlisting (apc,cpc_unknown_sql_command);
                    (*ENDIF*)
                    END;
                cpc_i_record   :
                    BEGIN
                    IF   pcan.ansection <> cpc_se_record
                    THEN
                        p11wrerrorlisting (apc, cpc_invalid_section_begin_or_end);
                    (*ENDIF*)
                    pcan.ansection := cpc_se_declare;
                    END;
                cpc_i_section  :
                    BEGIN
                    IF   pcan.ansection <> cpc_se_var
                    THEN
                        p11wrerrorlisting (apc, cpc_invalid_section_begin_or_end);
                    (*ENDIF*)
                    pcan.ansection := cpc_se_off;
                    END;
                OTHERWISE:
                    p11wrerrorlisting (apc, cpc_unknown_sql_command);
                END;
            (*ENDCASE*)
            IF   pcerror = cpc_pre_ok
            THEN
                BEGIN
                next_symbol (apc, partbufp^, part1len);
                IF   (symb <> cpr_s_eof) AND (symb <> cpr_s_bufeof)
                THEN
                    p11wrerrorlisting (apc, cpc_invalid_sql_command);
                (*ENDIF*)
                END;
            (*ENDIF*)
            END
        ELSE
            p11wrerrorlisting (apc, cpc_unknown_sql_command);
        (*ENDIF*)
        END;
    (*ENDIF*)
    WHILE  (symb <> cpr_s_eof) AND (symb <> cpr_s_bufeof) DO
        next_symbol (apc, partbufp^, part1len);
    (*ENDWHILE*)
    END;
(*ENDWITH*)
m90name(xx, 'END p14end        ');
END;

(*------------------------------*)

PROCEDURE
        p14sqlfn_areaput ;



VAR
      i   : integer;
      len : integer;
      pos : integer;

BEGIN
m90print('PC14  p14sqlfn_a');
WITH apc, sqlxa, pcvf.vffn [cpc_includefile ] DO
    BEGIN
    p19sqlfnallocate (pcfn);
    m90int2 (pc, 'pcfn        ', pcfn );
    WITH sqlfnp^ [pcfn] DO
        BEGIN
        fnkanomin := pcka + 1;
        fnkanomax := csp_maxint2;
        fnfilen   := bsp_knl_identifier;
        fnincllno := pclno.lnosrc;
        len     := s30len (vffilen, bsp_c1, mxsp_vfilename);
        pos     := 1;
        m90int2 (pc, 'fnkanomin   ', fnkanomin);
        m90int2 (pc, 'fnkanomax   ', fnkanomax);
        m90int2 (pc, 'fnincllno   ', fnincllno);
        m90int2 (pc, 'len         ', len      );
        m90int2 (pc, 'pos         ', pos      );
        FOR i := 1 TO len DO
            IF  vffilen [i] = '/'
            THEN
                pos := i + 1;
            (*ENDIF*)
        (*ENDFOR*)
        IF  len > mxsp_name
        THEN
            len := mxsp_name;
        (*ENDIF*)
        m90int2 (pc, 'len  n      ', len      );
        m90int2 (pc, 'pos  n      ', pos      );
        s10mv17 (mxsp_vfilename, sizeof(fnfilen), vffilen, pos,
              fnfilen, 1, len);
        END;
    (*ENDWITH*)
    END;
(*ENDWITH*)
END;

(*------------------------------*)

PROCEDURE
        p14oracle ;



VAR
      retpos    : tsp_int2;

BEGIN
m90print('PC14  p14oracle ');
WITH apc, pcscan, sqlca, sqlrap^, pccmdpart, sqlxa   DO
    BEGIN
    retpos := sypos;
    IF  ralang in [cpr_la_c, cpr_la_cobol]
    THEN
        (* if f?ur umstellung next_symbol *)
        WHILE  (symb <>cpr_s_eof) AND (symb <> cpr_s_bufeof) DO
            next_symbol (apc, partbufp^, part1len);
        (*ENDWHILE*)
    (*ENDIF*)
    IF  ralang = cpr_la_cobol
    THEN
        BEGIN
        part1len := part1len + 1;
        partbufp^ [part1len] := cpr_nullchar ;
        END;
    (*ENDIF*)
    syposacc := retpos;
    symb     := cpr_s_unknown;
    p11wrerrorlisting (apc, cpc_del_oracle_statement);
    WHILE  (symb <> cpr_s_eof) AND (symb <> cpr_s_bufeof) DO
        next_symbol (apc, partbufp^, part1len);
    (*ENDWHILE*)
    END;
(*ENDWITH*)
m90name(xx, 'END p14oracle     ');
END;

(*------------------------------*)

PROCEDURE
        p14_put_dclgen ;



VAR
      sn  : tsp_sname;
      len  : integer;

BEGIN
m90print('PC14  p14_put_dc');
WITH apc, pccmdpart, pcdclgen DO
    BEGIN
    part1len := 0;
    IF   pcerror = cpc_pre_ok
    THEN
        BEGIN
        s10fil (partsizlen, partbufp^, 1, 300, bsp_c1);
        sn := '   INCLUDE  ';
        s10mv11 (mxsp_sname, partsizlen,
              sn, 1 , partbufp^, part1len + 1,
              mxsp_sname);
        part1len := part1len + mxsp_sname + 1;
        partbufp^ [part1len] := pcopts.opt_quo [1] ;
        len := clause [1].dclfnlen;
        s10mv12 (mxsp_vfilename, partsizlen,
              pcvf.vffn [cpc_includefile].vffilen, 1,
              partbufp^, part1len + 1, len);
        part1len := part1len + len  + 1;
        partbufp^ [part1len] := pcopts.opt_quo [1] ;
        IF  tabclause = cpc_i_table
        THEN
            sn := ' TABLE      '
        ELSE
            sn := ' DBPROC     ';
        (*ENDIF*)
        s10mv11 (mxsp_sname, partsizlen,
              sn, 1 , partbufp^, part1len + 2, 9);
        part1len := part1len + 10;
        IF  dcluser <> bsp_knl_identifier
        THEN
            BEGIN
            len     := s30len1 (dcluser, bsp_c1, sizeof(dcluser));
            s10mv13 (sizeof(dcluser), partsizlen,
                  dcluser, 1 , partbufp^, part1len + 1,
                  len );
            part1len := part1len + len + 2;
            partbufp^ [part1len] := '.';
            END;
        (*ENDIF*)
        s10mv13 (sizeof(dcltab), partsizlen,
              dcltab, 1 , partbufp^, part1len + 1,
              dcltabl );
        part1len := part1len + dcltabl + 1;
        IF  dclmodul <> bsp_knl_identifier
        THEN
            BEGIN
            partbufp^ [part1len+1] := '.';
            len     := s30len1 (dclmodul, bsp_c1, sizeof(dclmodul));
            s10mv13 (sizeof(dclmodul), partsizlen,
                  dclmodul, 1 , partbufp^, part1len + 2,
                  len );
            part1len := part1len + len + 2;
            END;
        (*ENDIF*)
        CASE asclause OF
            cpc_as_record :
                sn := ' AS RECORD  ';
            cpc_as_type :
                sn := ' AS TYPE    ';
            cpc_as_struct :
                sn := ' AS STRUCT  ';
            OTHERWISE:
                sn := ' AS VAR     ';
            END;
        (*ENDCASE*)
        s10mv11 (mxsp_sname, partsizlen,
              sn, 1 , partbufp^, part1len + 1,
              mxsp_sname);
        part1len := part1len + mxsp_sname + 1;
        s10mv14 (sizeof(clause[1].dclattr), partsizlen,
              clause [1]. dclattr, 1 , partbufp^, part1len + 1,
              clause [1]. dclattrl);
        part1len := part1len +  clause [1]. dclattrl + 1;
        IF  indclause = cpr_is_true
        THEN
            BEGIN
            sn := 'IND         ';
            s10mv11 (mxsp_sname, partsizlen,
                  sn, 1 , partbufp^, part1len + 1, 5);
            part1len := part1len + 5 + 1;
            s10mv14 (sizeof(clause[1].dclattr), partsizlen,
                  clause [2]. dclattr, 1 , partbufp^, part1len + 1,
                  clause [2]. dclattrl);
            part1len := part1len +  clause [2]. dclattrl + 1;
            END;
        (*ENDIF*)
        END;
    (*ENDIF*)
    END;
(*ENDWITH*)
m90name(xx, 'END p14_put_dclgen');
END;

(*------------------------------*)

PROCEDURE
        p14open ;



VAR
      keyind : integer;
      ind    : integer;
      openind: integer;
      found  : boolean;
      err    : tpc_pre_errors;
      cursorn: tsp_knl_identifier;
      cursorl: integer;
      kaind     : tsp_int2;
      macro     : tsp_int2;
      param     : tsp_int2;
      va1ix     : tsp_int2;
      ch12      : tsp_c12;
      no        : integer;
      CursorNameIndex  : tsp_int2;
      rscan  : tpr_scannertype;
      ndcuindex  : integer;
      lineno    : tsp_int2;
      kaentry   : sqlkaentry;
      cudiind   : tsp_int2;
      dummy : tsp00_Int2;

BEGIN
m90print('PC14  p14open   ');
WITH apc, pcscan, sqlca, sqlrap^, pccmdpart, sqlxa DO
    BEGIN
    xadescpos := 0;
    xadesclen := 0;
    IF   pcerror = cpc_pre_ok
    THEN
        BEGIN
        err := cpc_pre_ok;
        openno := 0;
        next_symbol (apc, partbufp^, part1len);
        p10getkeyword (apc, partbufp^, keyind);
        IF  (keyind = cpc_i_column)
        THEN
            BEGIN
            pcan.ancomtyp := cpr_com_sql
            END
        ELSE
            pcan.ancomtyp := cpr_com_sql_open;
        (*ENDIF*)
        p14getname (next_symbol, apc, cursorn, cursorl, macro
              , param
              );
        IF  (param = cpr_is_true)
        THEN
            BEGIN
            dummy := 0;
            p14_add_prind(apc, ind, dummy);
            ind := pcpr;
            rscan := pcscan;
            p14_hostv_get (next_symbol, apc, CursorNameIndex, va1ix);
            no := va1ix;
            p05inttochr12 (no, ch12);
            cursorn := bsp_knl_identifier;
            cursorn[1] := ':';
            s10mv18 (mxsp_c12, sizeof(cursorn), ch12, 2,
                  cursorn, 2, 4);
            pcscan := rscan;
            (* PTS 1104482 *)
            ndcuindex := pcsqlva.ndcnt;
            END
        ELSE
            BEGIN
            dummy := 0;
            p14_add_prind(apc, ind, dummy);
            ind := pcpr;
            CursorNameIndex := 0;
            (* PTS 1104482 *)
            ndcuindex := 0;
            END;
        (*ENDIF*)
        IF   (ind = 0)
        THEN
            BEGIN
            next_symbol (apc, partbufp^, part1len);
            p10getkeyword (apc, partbufp^, keyind);
            IF  (symb = cpr_s_eof) OR (keyind = cpc_i_into) OR
                (keyind = cpc_i_using)
            THEN
                (* open ohne declare cursor *)
                err := cpc_unknown_cursorname
            ELSE
                p14ansqlstatement (next_symbol, apc);
            (*ENDIF*)
            END
        ELSE
            (* cursorname gefunden *)
            WITH sqlxa, sqlprp^ [ind ] DO
                BEGIN
                cudiind := 0;
                IF  prcuindex > 0
                THEN
                    cudiind := sqlprp^ [prcuindex]. prkaindex;
                (*ENDIF*)
                m90int2 (pc, 'cudiind ****', cudiind);
                m90int2 (pc, 'prcuindex   ', prcuindex);
                m90int2 (pc, 'prkaindex   ', prkaindex);
                m90int2 (pc, 'prStmtNameIn', prStmtNameIndex);
                m90int2 (pc, 'prarea      ', prarea   );
                m90int2 (pc, 'prstate     ', prstate  );
                m90int2 (pc, 'prstcount   ', prstcount);
                m90int2 (pc, 'prstindex   ', prstindex);
                IF  pcinpind = cpc_inputfile
                THEN
                    lineno   := pclno.lnosrc
                ELSE
                    lineno   := - pclno.lnoexp;
                (*ENDIF*)
                next_symbol (apc, partbufp^, part1len);
                IF   symb = cpr_s_eof
                THEN
                    (* cursorname ohne using oder into *)
                    (* es wird kein sa eintrag generiert *)
                    BEGIN
                    (* ka_eintrag generieren *)
                    p14kaentryinit (apc);
                    prCursorName := cursorn;
                    prCursorNameIndex := CursorNameIndex;
                    prkaindex := pcka;
                    WITH sqlkap^ [pcka ] DO
                        BEGIN
                        kaprindex := ind;
                        kamacro   := macro;
                        kacuindex := pckacuind;
                        kalineno  := lineno;
                        openno    := pcka;
                        END;
                    (*ENDWITH*)
                    IF   prkaindex = 0
                    THEN
                        BEGIN
                        END
                    ELSE
                        BEGIN
                        (** open ohne prepare korr 1.11.95 *)
                        openind := prkaindex;
                        found := false;
                        REPEAT
                            m90int2 (pc, 'prkaindex   ', prkaindex);
                            m90int2 (pc, 'openind     ', openind  );
                            IF  (sqlkap^[openind].katyp <> pcan.ancomtyp)
                                OR  (sqlkap^[openind].kaprindex <> ind)
                            THEN
                                openind := sqlkap^[openind].kadiindex
                            ELSE
                                found := true;
                            (*ENDIF*)
                        UNTIL
                            (openind = 0) OR (found);
                        (*ENDREPEAT*)
                        IF  openind = 0
                        THEN
                            openind := prkaindex;
                        (*ENDIF*)
                        sqlkap^[pcka] := sqlkap^ [openind];
                        sqlkap^ [pcka] .katyp    := pcan.ancomtyp;
                        sqlkap^ [pcka] .kalineno := lineno;
                        sqlkap^[openind].kadiindex := pcka;
                        openno := pcka;
                        m90int2 (pc, 'kaprindex   ',
                              sqlkap^ [prkaindex] .kaprindex );
                        m90int2 (pc, 'prkaindex   ', prkaindex);
                        m90int2 (pc, 'openind     ', openind  );
                        m90int2 (pc, 'pcpr        ', pcpr     );
                        m90int2 (pc, 'pcka        ', pcka     );
                        END;
                    (*ENDIF*)
                    m90int2 (pc, 'openno      ', openno  );
                    pcan.ancheck := pcopts.opt_mode;
                    sqlxa.xakano := openno;
                    (* PTS 1104482 *)
                    IF  ndcuindex > 0
                    THEN
                        pcsqlva.ndtabp^ [ndcuindex].ndkano := pcka;
                    (*ENDIF*)
                    END
                ELSE
                    BEGIN
                    (* using oder descriptor *)
                    p10getkeyword (apc, partbufp^, keyind);
                    CASE keyind OF
                        cpc_i_into, cpc_i_using :
                            BEGIN
                            IF   prkaindex = 0
                            THEN
                                kaind := pcka
                            ELSE
                                kaind := prkaindex;
                            (*ENDIF*)
                            next_symbol (apc, partbufp^, part1len);
                            p10getkeyword (apc, partbufp^, keyind);
                            IF   keyind = cpc_i_descriptor
                            THEN
                                BEGIN
                                (* ka_eintrag generieren *)
                                p14kaentryinit (apc);
                                (** open entry doppeln *)
                                sqlkap^[pcka] := sqlkap^[kaind];
                                kaind := pcka;
                                WITH sqlkap^ [kaind ] DO
                                BEGIN
                                kadiindex := prkaindex;
                                prkaindex := pcka;
                                prCursorName := cursorn;
                                (* PTS 1104482 *)
                                prCursorNameIndex := CursorNameIndex;
                                IF  ndcuindex > 0
                                THEN
                                    pcsqlva.ndtabp^ [ndcuindex].ndkano := pcka;
                                (*ENDIF*)
                                END;
                                next_symbol (apc, partbufp^, part1len);
                                IF   symb <> cpr_s_eof
                                THEN
                                    BEGIN
                                    IF  (rakamode <> cpr_kind_internal)
                                    THEN
                                        BEGIN
                                        IF  partbufp^ [sypos] = cpr_paramchr
                                        THEN
                                            xadescpos := sypos + 1
                                        ELSE
                                            xadescpos := sypos;
                                        (*ENDIF*)
                                        (* xadescpos := sypos; *)
                                        xadesclen := syposacc - xadescpos;
                                        next_symbol (apc, partbufp^, part1len);
                                        END
                                    ELSE
                                        BEGIN
                                        IF  partbufp^ [sypos] = cpr_paramchr
                                        THEN
                                            xadescpos := sypos + 1
                                        ELSE
                                            BEGIN
                                            p10getkeyword (apc, partbufp^, keyind);
                                            IF  keyind <> cpc_i_keep
                                            THEN
                                                BEGIN
                                                xadescpos := sypos;
                                                xadesclen := syposacc - xadescpos;
                                                next_symbol (apc, partbufp^, part1len);
                                                END
                                            ELSE
                                                BEGIN
                                                xadescpos := -1;
                                                END
                                            (*ENDIF*)
                                            END
                                        (*ENDIF*)
                                        END;
                                    (*ENDIF*)
                                    p10getkeyword (apc, partbufp^, keyind);
                                    IF  keyind = cpc_i_keep
                                    THEN
                                        BEGIN
                                        pcan.ancomtyp := cpr_com_sql_open_keep;
                                        END;
                                    (*ENDIF*)
                                    WHILE  (symb <> cpr_s_eof) AND
                                          (symb <> cpr_s_bufeof) DO
                                        BEGIN
                                        next_symbol (apc, partbufp^,
                                              part1len);
                                        END;
                                    (*ENDWHILE*)
                                    END
                                ELSE
                                    BEGIN
                                    IF  (rakamode = cpr_kind_internal)
                                    THEN
                                        (* default sqldb sqlda expansion *)
                                        xadescpos := -1
                                    ELSE
                                        err := cpc_invalid_sql_command;
                                    (*ENDIF*)
                                    END;
                                (*ENDIF*)
                                sqlxa.sqlprp^ [ind] .prDescribe :=
                                      cpr_is_true;
                                sqlxa.sqlkap^[kaind] .kaprindex := ind;
                                sqlxa.sqlkap^[kaind] .kapacount:= -1;
                                sqlxa.sqlkap^[kaind] .katyp := pcan.ancomtyp;
                                sqlxa.sqlkap^[kaind] .kalineno := lineno;
                                openno := kaind;
                                END
                            ELSE
                                IF  symb <> cpr_s_parameter_name
                                THEN
                                    err := cpc_invalid_sql_command
                                ELSE
                                    BEGIN
                                    kaentry := sqlkap^[kaind];  (*bf*)
                                    p14an1sqlstatement (next_symbol, apc);
                                    m90int2 (pc, 'ind         ', ind);
                                    m90int2 (pc, 'kaind       ', kaind);
                                    m90int2 (pc, 'pcka        ', pcka);
                                    m90int2 (pc, 'prkaindex   ', prkaindex);
                                    m90int2 (pc, 'kalineno    ', lineno   );
                                    (******bf*******)
                                    WITH kaentry  DO
                                        BEGIN
                                        kaprindex := ind;
                                        katyp := pcan.ancomtyp;
                                        kapacount := sqlxa.sqlkap^ [pcka]
                                              .kapacount;
                                        kapaindex := sqlxa.sqlkap^ [pcka]
                                              .kapaindex;
                                        kalineno := lineno;
                                        IF  cudiind <> 0
                                        THEN
                                            BEGIN
                                            kadiindex := cudiind;
                                            prkaindex := pcka;
                                            sqlprp^[prcuindex].prkaindex
                                                  := pcka;
                                            END
                                        ELSE
                                            BEGIN
                                            kadiindex := prkaindex;
                                            prkaindex := pcka;
                                            END;
                                        (*ENDIF*)
                                        prCursorName := cursorn;
                                        prCursorNameIndex := CursorNameIndex;
                                        sqlxa.sqlkap^[pcka] := kaentry;
                                        END;
                                    (*ENDWITH*)
                                    xakano := pcka;
                                    openno := pcka;
                                    END;
                                (*ENDIF*)
                            (*ENDIF*)
                            END;
                        OTHERWISE:
                            err := cpc_invalid_sql_command;
                        END;
                    (*ENDCASE*)
                    sqlxa.xakano := openno;
                    END;
                (*ENDIF*)
                END;
            (*ENDWITH*)
        (*ENDIF*)
        IF   err <> cpc_pre_ok
        THEN
            p11precomerror (apc, err);
        (*ENDIF*)
        END;
    (*ENDIF*)
    WHILE  (symb <> cpr_s_eof) AND (symb <> cpr_s_bufeof) DO
        next_symbol (apc, partbufp^, part1len);
    (*ENDWHILE*)
    m90int2 (pc, 'xakano      ', xakano   );
    m90int2 (pc, 'xadescpos   ', xadescpos);
    m90int2 (pc, 'xadesclen   ', xadesclen);
    END;
(*ENDWITH*)
m90name(xx, 'END p14open       ');
END;

(*------------------------------*)

PROCEDURE
        p14include ;



VAR
      len    : integer;
      pos    : integer;
      rscan  : tpr_scannertype;
      inclfile : boolean;

BEGIN
m90print('PC14  p14include');
WITH apc, pcscan, sqlca, sqlrap^, pccmdpart, pcdclgen DO
    BEGIN
    keyind:= 0;
    dclgen   := false;
    inclfile := false;
    asclause := cpc_as_empty;
    IF   pcerror = cpc_pre_ok
    THEN
        BEGIN
        next_symbol (apc, partbufp^, part1len);
        IF   (symb = cpr_s_identifier)
            OR ((ralang = cpr_la_c)
            AND (symb <> cpr_s_string_literal)
            AND  (symb <> cpr_s_cstring_literal))
        THEN
            BEGIN
            p10getkeyword (apc, partbufp^, keyind);
            CASE ralang OF
                cpr_la_pascallpi,
                cpr_la_pascal, cpr_la_pascal31, cpr_la_pascalvax :
                    CASE keyind OF
                        cpc_i_sqlconst :
                            BEGIN
                            IF   (pccodeind > cpc_work1file)
                            THEN
                                p11precomerror (apc,
                                      cpc_include_area_on_wrong_position);
                            (*ENDIF*)
                            pccodeind := cpc_work2file;
                            END;
                        cpc_i_sqlproc  :
                            BEGIN
                            IF   (pccodeind > cpc_work2file)
                            THEN
                                p11precomerror (apc,
                                      cpc_include_area_on_wrong_position);
                            (*ENDIF*)
                            pccodeind := cpc_work3file;
                            END;
                        cpc_i_sqltype, cpc_i_sqlca  :
                            BEGIN
                            IF   (pccodeind <> cpc_work2file)
                            THEN
                                p11precomerror (apc,
                                      cpc_include_area_on_wrong_position);
                            (*ENDIF*)
                            END;
                        OTHERWISE:
                            p11wrerrorlisting (apc, cpc_unknown_sql_command);
                        END;
                    (*ENDCASE*)
                cpr_la_cobol, cpr_la_cobmic :
                    BEGIN
                    END;
                cpr_la_c :
                    IF   (keyind in [cpc_i_sqlca, cpc_i_sqlda, cpc_i_oraca] )
                    THEN
                        BEGIN
                        WHILE (symb<> cpr_s_eof) AND (symb <> cpr_s_bufeof) DO
                            BEGIN
                            next_symbol (apc, partbufp^, part1len);
                            END;
                        (*ENDWHILE*)
                        END
                    ELSE
                        IF  (rakamode <> cpr_kind_oracle)
                            AND  ( rakamode <> cpr_kind_sapr3 )
                        THEN
                            p11wrerrorlisting(apc,cpc_unknown_sql_command)
                        ELSE
                            BEGIN
                            (* umschalten auf include file *)
                            IF   pcinpind = cpc_includefile
                            THEN
                                p11wrerrorlisting (apc,
                                      cpc_includefile_allready_open)
                            ELSE
                                WITH pcvf.vffn [cpc_includefile ] DO
                                    BEGIN
                                    inclfile := true;
                                    vffileno := 0;
                                    vffilemode  := sp5vf_read;
                                    vfbinary := false;
                                    vfreclen2:= 0;
                                    vfreclen4:= 0;
                                    pr01eInitFileName(vffilen);
                                    vfcnt    := 0;
                                    pos       := 1;
                                    WHILE (symb <> cpr_s_eof) AND
                                          (symb <> cpr_s_bufeof) DO
                                        BEGIN
                                        IF   sylen + pos > mxsp_vfilename
                                        THEN
                                            len := mxsp_vfilename - pos
                                        ELSE
                                            len := sylen;
                                        (*ENDIF*)
                                        s10mv5 (partsizlen, mxsp_vfilename,
                                              partbufp^, sypos,
                                              vffilen, pos, len);
                                        pos := pos + len;
                                        next_symbol (apc, partbufp^,
                                              part1len);
                                        END;
                                    (*ENDWHILE*)
                                    END;
                                (*ENDWITH*)
                            (*ENDIF*)
                            END;
                        (*ENDIF*)
                    (*ENDIF*)
                OTHERWISE:
                    p11wrerrorlisting(apc,cpc_pre_not_implemented);
                END;
            (*ENDCASE*)
            IF  (pcerror = cpc_pre_ok)
                AND (symb <> cpr_s_eof) AND (symb <> cpr_s_bufeof)
                AND (ralang <> cpr_la_cobol)
            THEN
                BEGIN
                rscan := pcscan;
                next_symbol (apc, partbufp^, part1len);
                IF   (symb <> cpr_s_eof) AND (symb <> cpr_s_bufeof)
                THEN
                    p11wrerrorlisting (apc, cpc_invalid_sql_command);
                (*ENDIF*)
                IF  NOT (ralang in [cpr_la_c, cpr_la_cobol])
                THEN
                    pcscan := rscan;
                (*ENDIF*)
                END;
            (*ENDIF*)
            END
        ELSE
            IF  NOT (  (symb = cpr_s_string_literal)
                OR (symb = cpr_s_cstring_literal))
            THEN
                p11wrerrorlisting (apc, cpc_invalid_sql_command);
            (*ENDIF*)
        (*ENDIF*)
        IF   (symb = cpr_s_string_literal)
            OR (symb = cpr_s_cstring_literal)
        THEN
            BEGIN
            (* umschalten auf include file *)
            p10stringliteralget (apc, partbufp^);
            IF   pcinpind = cpc_includefile
            THEN
                p11wrerrorlisting (apc, cpc_includefile_allready_open)
            ELSE
                WITH pcvf.vffn [cpc_includefile ] DO
                    BEGIN
                    inclfile := true;
                    vffileno := 0;
                    vffilemode  := sp5vf_read;
                    vfbinary := false;
                    vfreclen2:= 0;
                    vfreclen4:= 0;
                    pr01eInitFileName(vffilen);
                    vfcnt    := 0;
                    IF   sylen > mxsp_vfilename
                    THEN
                        len := mxsp_vfilename
                    ELSE
                        len := sylen;
                    (*ENDIF*)
                    s10mv5 (partsizlen, mxsp_vfilename, partbufp^, sypos,
                          vffilen, 1, len);
                    (*  get include syntax into pcdclgen *)
                    p14_dclgen (next_symbol, apc, dclgen, len);
                    END;
                (*ENDWITH*)
            (*ENDIF*)
            END;
        (*ENDIF*)
        IF  inclfile
        THEN
            WITH pcvf.vffn [cpc_includefile ] DO
                BEGIN
                IF   pcan.ansection <> cpc_se_off
                THEN
                    pcsqlcom := true;
                (*ENDIF*)
                p11vfopen (apc, cpc_includefile);
                IF   pcerror = cpc_pre_ok
                THEN
                    BEGIN
                    pcinpind := cpc_includefile;
                    (* sqlfn-area belegen 3.11.93 *)
                    p14sqlfn_areaput (apc);
                    IF  dclgen
                    THEN
                        BEGIN
                        p11wrerrorlisting (apc, cpc_include_file_exist);
                        sqlca.sqlcode := 0;
                        pcerror := cpc_pre_ok ;
                        END;
                    (*ENDIF*)
                    dclgen := false;
                    END
                ELSE
                    BEGIN
                    IF  (dclgen) AND (pcopts.opt_mode <> cpc_ch_no)
                    THEN
                        BEGIN
                        p16tabledclgen (apc, dclgen);
                        IF  (dclgen) AND (sqlcode = 0)
                        THEN
                            BEGIN
                            pcerror  := cpc_pre_ok;
                            vffilemode  := sp5vf_write;
                            p11vfopen (apc, cpc_includefile);
                            p14_put_dclgen (apc);
                            END
                        (*ENDIF*)
                        END;
                    (*ENDIF*)
                    IF  (NOT dclgen) OR (pcerror <> cpc_pre_ok)
                    THEN
                        BEGIN
                        dclgen := false;
                        p11wrerrorlisting (apc, cpc_includefile_open_error);
                        END;
                    (*ENDIF*)
                    END;
                (*ENDIF*)
                END;
            (*ENDWITH*)
        (*ENDIF*)
        END;
    (*ENDIF*)
    END;
(*ENDWITH*)
m90name(xx, 'END p14include    ');
END;

(*------------------------------*)

PROCEDURE
        p14begin ;



VAR
      keyind : integer;

BEGIN
m90print('PC14  p14begin  ');
WITH apc, pcscan, sqlca, sqlrap^, pccmdpart DO
    BEGIN
    IF   pcerror = cpc_pre_ok
    THEN
        BEGIN
        next_symbol (apc, partbufp^, part1len);
        p10getkeyword (apc, partbufp^, keyind);
        IF   keyind = cpc_i_declare
        THEN
            BEGIN
            next_symbol (apc, partbufp^, part1len);
            p10getkeyword (apc, partbufp^, keyind);
            CASE ralang OF
                cpr_la_pascallpi,
                cpr_la_pascal, cpr_la_pascal31, cpr_la_pascalvax :
                    IF   pcan.ansection <> cpc_se_off
                    THEN
                        p11wrerrorlisting (apc, cpc_invalid_section_begin_or_end)
                    ELSE
                        CASE keyind OF
                            cpc_i_const :
                                BEGIN
                                pcan.ansection := cpc_se_const;
                                next_symbol (apc, partbufp^, part1len);
                                p10getkeyword (apc, partbufp^, keyind);
                                IF   keyind <> cpc_i_section
                                THEN
                                    p11wrerrorlisting (apc,cpc_unknown_sql_command);
                                (*ENDIF*)
                                END;
                            cpc_i_type  :
                                BEGIN
                                pcan.ansection := cpc_se_type;
                                next_symbol (apc, partbufp^, part1len);
                                p10getkeyword (apc, partbufp^, keyind);
                                IF   keyind <> cpc_i_section
                                THEN
                                    p11wrerrorlisting (apc,cpc_unknown_sql_command);
                                (*ENDIF*)
                                END;
                            cpc_i_section  :
                                pcan.ansection := cpc_se_var;
                            OTHERWISE:
                                p11wrerrorlisting (apc, cpc_unknown_sql_command);
                            END;
                        (*ENDCASE*)
                    (*ENDIF*)
                cpr_la_c, cpr_la_cobol, cpr_la_cobmic, cpr_la_cob8860 :
                    IF   keyind = cpc_i_section
                    THEN
                        pcan.ansection := cpc_se_var
                    ELSE
                        p11wrerrorlisting (apc,cpc_unknown_sql_command);
                    (*ENDIF*)
                OTHERWISE:
                    p11wrerrorlisting (apc, cpc_pre_not_implemented);
                END;
            (*ENDCASE*)
            IF   (pcerror = cpc_pre_ok)
            THEN
                BEGIN
                next_symbol (apc, partbufp^, part1len);
                IF   (symb <> cpr_s_eof) AND (symb <> cpr_s_bufeof)
                THEN
                    p11wrerrorlisting (apc, cpc_invalid_sql_command);
                (*ENDIF*)
                END;
            (*ENDIF*)
            END
        ELSE
            p11wrerrorlisting (apc, cpc_unknown_sql_command);
        (*ENDIF*)
        END;
    (*ENDIF*)
    IF  pcan.ansection <> cpc_se_record
    THEN
        WHILE  (symb <> cpr_s_eof) AND (symb <> cpr_s_bufeof) DO
            next_symbol (apc, partbufp^, part1len);
        (*ENDWHILE*)
    (*ENDIF*)
    END;
(*ENDWITH*)
m90name(xx, 'END p14begin      ');
END;

(*------------------------------*)

PROCEDURE
        p14command ;



VAR
      err   : tpc_pre_errors;
      ptypentix : tsp_int2;
      pvatyp    : tsp_int2;
      keyind    : integer;
      pos       : integer;
      len       : integer;
      retpos    : tsp_int2;
      lastvatyp : tsp_int2;
      vix       : integer;

BEGIN
m90print('PC14  p14command');
WITH apc, sqlxa, pcscan, sqlca, sqlrap^, pccmdpart, pcsqlva DO
    BEGIN
    retpos := sypos;
    pcan.ancheck := pcopts.opt_mode;
    pcusage.uscom    := cpc_usage_comm;
    pcusage.usdobjn1 := bsp_knl_identifier;
    pcusage.usdobjn2 := bsp_knl_identifier;
    IF  ralang in [cpr_la_c, cpr_la_cobol]
    THEN
        WHILE  (symb <> cpr_s_eof) AND (symb <> cpr_s_bufeof) DO
            next_symbol (apc, partbufp^, part1len);
        (*ENDWHILE*)
    (*ENDIF*)
    IF  ralang = cpr_la_cobol
    THEN
        BEGIN
        part1len := part1len + 1;
        partbufp^ [part1len] := cpr_nullchar ;
        END;
    (*ENDIF*)
    syposacc := retpos;
    symb     := cpr_s_unknown;
    err  := cpc_pre_ok;
    p14kaentryinit (apc);
    WITH sqlkap^ [pcka ] DO
        BEGIN
        p19sqlprallocate (pcpr);
        WITH sqlxa, sqlprp^ [pcpr ] DO
            BEGIN
            kaprindex := pcpr;
            prstcount := 0;
            prstindex := 0;
            prcuindex := 0;
            prstate:= cpr_state_sync_exec;
            prarea := cpr_in_sqlva;
            prkaindex := 0;
            prDescribe := cpr_is_false;
            prStmtName   := bsp_knl_identifier;
            prStmtNameIndex := 0;
            prCursorNameIndex := 0;
            prCursorName := bsp_knl_identifier;
            next_symbol (apc, partbufp^, part1len);
            IF  symb = cpr_s_identifier
            THEN
                BEGIN
                p10getkeyword (apc, partbufp^, keyind);
                CASE  keyind OF
                    cpc_i_sync :
                        prstate:= cpr_state_sync_exec;
                    cpc_i_newsync :
                        prstate:= cpr_state_newsync_exec;
                    cpc_i_async :
                        prstate:= cpr_state_async_exec;
                    OTHERWISE:
                        p11precomerror (apc, cpc_invalid_sql_command);
                    END;
                (*ENDCASE*)
                next_symbol (apc, partbufp^, part1len);
                END;
            (*ENDIF*)
            IF  symb = cpr_s_parameter_name
            THEN
                BEGIN
                syposacc := sypos + 1;
                ptypentix := 0;
                p14btypentget (next_symbol, apc, ptypentix,
                      pvatyp, lastvatyp, cpr_is_true);
                IF  ptypentix = 0
                THEN
                    p11precomerror (apc, cpc_unknown_parametername)
                ELSE
                    BEGIN
                    kapacount := 0;
                    kapaindex := pcpa + 1;
                    syposacc   := sypos;
                    WITH ndtabp^ [ndcnt] DO
                        BEGIN
                        IF  ndsqlva1st <> ndsqlva1ix
                        THEN
                            BEGIN
                            p19cparentry (apc, sqlparst, ndsqlva1st,
                                  0 , ndsqlva1cnt);
                            kapacount := 1;
                            END;
                        (*ENDIF*)
                        p19cparentry (apc, sqlparel,
                              ndsqlva1ix, 0, 0);
                        kapacount := kapacount + 1;
                        END;
                    (*ENDWITH*)
                    END;
                (*ENDIF*)
                END
            ELSE
                IF  (symb = cpr_s_string_literal)
                    OR (symb = cpr_s_cstring_literal)
                THEN
                    BEGIN
                    (* command = stringconst  *)
                    p10stringliteralget (apc, partbufp^);
                    kaStindex := pcst + 1;
                    prarea     := cpr_in_sqlst;
                    pos := sypos;
                    WHILE pos <  sypos + sylen  DO
                        BEGIN
                        IF   pos + mxpr_sqlline
                            > sypos + sylen
                        THEN
                            len := sypos + sylen - pos
                        ELSE
                            len := mxpr_sqlline;
                        (*ENDIF*)
                        p19sqlstallocate (pcst);
                        IF  pcst > pcstmax
                        THEN
                            pcstmax := pcst;
                        (*ENDIF*)
                        kaStcount := kaStcount + 1;
                        s10mv1 (partsizlen,mxpr_sqlline,
                              partbufp^, pos,
                              sqlstp^ [pcst].stline, 1, len);
                        sqlstp^ [pcst].stllen  := len;
                        sqlstp^ [pcst].stkano := pcka;
                        pos := pos + len;
                        END;
                    (*ENDWHILE*)
                    END
                ELSE
                    p11precomerror (apc, cpc_invalid_sql_command);
                (*ENDIF*)
            (*ENDIF*)
            IF   (symb <> cpr_s_eof) AND  (symb <> cpr_s_bufeof)
            THEN
                next_symbol (apc, partbufp^, part1len);
            (*ENDIF*)
            IF  prstate <> cpr_state_async_exec
            THEN
                BEGIN
                (* result parameter *)
                p10getkeyword (apc, partbufp^, keyind);
                IF  keyind <> cpc_i_result
                THEN
                    p11precomerror (apc, cpc_invalid_sql_command)
                ELSE
                    BEGIN
                    next_symbol (apc, partbufp^, part1len);
                    IF  symb <> cpr_s_parameter_name
                    THEN
                        p11precomerror (apc, cpc_invalid_sql_command)
                    ELSE
                        BEGIN
                        syposacc := sypos + 1;
                        ptypentix := 0;
                        p14btypentget (next_symbol, apc, ptypentix,
                              pvatyp, lastvatyp, cpr_is_true);
                        IF  ptypentix = 0
                        THEN
                            p11precomerror (apc, cpc_unknown_parametername)
                        ELSE
                            BEGIN
                            prkaindex := pcpa + 1;
                            WITH ndtabp^ [ndcnt] DO
                                BEGIN
                                IF  ndsqlva1st <> ndsqlva1ix
                                THEN
                                    BEGIN
                                    p19cparentry (apc, sqlparst,
                                          ndsqlva1st, 0, ndsqlva1cnt);
                                    END;
                                (*ENDIF*)
                                p19cparentry (apc, sqlparel,
                                      ndsqlva1ix, 0, 0);
                                vix := ndsqlva1ix;
                                IF  sqlv1p^[vix].va1indi_sc = sqlvapt
                                THEN
                                    vix :=  sqlv1p^[vix].va1ix_pt;
                                (*ENDIF*)
                                END;
                            (*ENDWITH*)
                            WITH  sqlv1p^[vix],sqlv2p^[va1indva2_sc] DO
                                CASE  va2typ  OF
                                    cpr_vint2, cpr_vint4 :
                                        BEGIN
                                        END;
                                    OTHERWISE:
                                        BEGIN
                                        p11precomerror (apc,
                                              cpc_datatyp_not_compatible);
                                        END;
                                    END;
                                (*ENDCASE*)
                            (*ENDWITH*)
                            END;
                        (*ENDIF*)
                        IF   (symb <> cpr_s_eof) AND
                            (symb <> cpr_s_bufeof)
                        THEN
                            next_symbol (apc, partbufp^, part1len);
                        (*ENDIF*)
                        END;
                    (*ENDIF*)
                    END;
                (*ENDIF*)
                END;
            (*ENDIF*)
            IF  (symb <> cpr_s_eof)
                AND (symb <> cpr_s_bufeof)
            THEN
                p11precomerror (apc, cpc_invalid_sql_command);
            (*ENDIF*)
            END;
        (*ENDWITH*)
        END;
    (*ENDWITH*)
    IF   err <> cpc_pre_ok
    THEN
        p11precomerror (apc, err);
    (*ENDIF*)
    symb := cpr_s_eof;
    END;
(*ENDWITH*)
m90name(xx, 'END p14command    ');
END;

(*------------------------------*)

PROCEDURE
        p14cancel ;



VAR
      err   : tpc_pre_errors;
      ptypentix : tsp_int2;
      pvatyp    : tsp_int2;
      keyind    : integer;
      retpos    : tsp_int2;
      lastvatyp : tsp_int2;
      sessno    : tsp_int2;

BEGIN
m90print('PC14  p14cancel ');
WITH apc, sqlxa, pcscan, sqlca, sqlrap^, pccmdpart, pcsqlva DO
    BEGIN
    retpos := syposacc;
    pcan.ancheck := pcopts.opt_mode;
    pcan.ancomtyp := cpr_com_cancel;
    pcusage.uscom    := cpc_usage_cancel;
    pcusage.usdobjn1 := bsp_knl_identifier;
    pcusage.usdobjn2 := bsp_knl_identifier;
    IF  ralang in [cpr_la_c, cpr_la_cobol]
    THEN
        WHILE  (symb <> cpr_s_eof) AND (symb <> cpr_s_bufeof) DO
            next_symbol (apc, partbufp^, part1len);
        (*ENDWHILE*)
    (*ENDIF*)
    IF  ralang = cpr_la_cobol
    THEN
        BEGIN
        part1len := part1len + 1;
        partbufp^ [part1len] := cpr_nullchar ;
        END;
    (*ENDIF*)
    syposacc := retpos;
    symb     := cpr_s_unknown;
    err  := cpc_pre_ok;
    p14kaentryinit (apc);
    WITH sqlkap^ [pcka ] DO
        BEGIN
        next_symbol (apc, partbufp^, part1len);
        CASE symb OF
            cpr_s_identifier :
                BEGIN
                p10getkeyword (apc, partbufp^, keyind);
                IF  keyind = cpc_i_current
                THEN
                    (* kapaindex = null *)
                    kapacount := 1
                ELSE
                    p11wrerrorlisting (apc, cpc_unknown_sql_command);
                (*ENDIF*)
                next_symbol (apc, partbufp^, part1len);
                END;
            cpr_s_parameter_name :
                WITH  pcsqlva, pcsymtab DO
                    BEGIN
                    (* statement in variabler *)
                    syposacc := sypos+1;
                    ptypentix  := 0;
                    p14btypentget (next_symbol, apc, ptypentix,
                          pvatyp, lastvatyp, cpr_is_true);
                    IF   ptypentix = 0
                    THEN
                        BEGIN
                        IF   (pcerror = cpc_pre_ok)
                        THEN
                            BEGIN
                            p11precomerror (apc, cpc_unknown_parametername);
                            p11nerrorlisting ( apc,
                                  vartablep^ [ndtabp^[ndcnt].ndvarentix].vaname,
                                  vartablep^[ndtabp^[ndcnt].ndvarentix].vanaml,0);
                            END
                        (*ENDIF*)
                        END
                    ELSE
                        IF  NOT (pvatyp in [cpr_vint2, cpr_vint4,
                            cpr_vuns2, cpr_vuns4])
                        THEN
                            BEGIN
                            p11precomerror (apc, cpc_datatyp_not_compatible);
                            p11nerrorlisting ( apc,
                                  vartablep^ [ndtabp^[ndcnt].ndvarentix].vaname,
                                  vartablep^[ndtabp^[ndcnt].ndvarentix].vanaml,0);
                            END
                        ELSE
                            BEGIN
                            (* kapaindex positiv *)
                            kapaindex := pcpa + 1;
                            WITH ndtabp^ [ndcnt] DO
                                BEGIN
                                IF  ndsqlva1st <> ndsqlva1ix
                                THEN
                                    BEGIN
                                    p19cparentry (apc, sqlparst, ndsqlva1st,
                                          0 , ndsqlva1cnt);
                                    kapacount := 1;
                                    END;
                                (*ENDIF*)
                                p19cparentry (apc, sqlparel,
                                      ndsqlva1ix, 0, 0);
                                kapacount := kapacount + 1;
                                END;
                            (*ENDWITH*)
                            END;
                        (*ENDIF*)
                    (*ENDIF*)
                    END;
                (*ENDWITH*)
            cpr_s_unsigned_integer :
                BEGIN
                p10int2unsignedget (apc, partbufp^, sessno);
                IF  (sessno < 1)
                    OR (sessno > 8)
                THEN
                    p11wrerrorlisting (apc, cpc_session_not_allowed)
                ELSE
                    BEGIN
                    (* kapaindex negativ *)
                    kapacount := 1;
                    kapaindex := - sessno;
                    END;
                (*ENDIF*)
                next_symbol (apc, partbufp^, part1len);
                END;
            OTHERWISE
                p11wrerrorlisting (apc, cpc_unknown_sql_command);
            END;
        (*ENDCASE*)
        IF   (symb <> cpr_s_eof) AND
            (symb <> cpr_s_bufeof)
        THEN
            p11wrerrorlisting (apc, cpc_unknown_sql_command);
        (*ENDIF*)
        END;
    (*ENDWITH*)
    symb := cpr_s_eof;
    END;
(*ENDWITH*)
END;

(*------------------------------*)

PROCEDURE
        p14abortsession ;



VAR
      nextsa : boolean;
      err    : tpc_pre_errors;
      pos    : integer;
      len    : integer;
      retpos : integer;
      keyind    : integer;

BEGIN
m90print('PC14  p14abortse');
WITH apc, pcscan, sqlca, sqlrap^, pccmdpart  DO
    BEGIN
    retpos := syposacc;
    IF  ralang in [cpr_la_c]
    THEN
        WHILE  (symb <> cpr_s_eof) AND (symb <> cpr_s_bufeof) DO
            next_symbol (apc, partbufp^, part1len);
        (*ENDWHILE*)
    (*ENDIF*)
    syposacc := retpos;
    symb     := cpr_s_unknown;
    err  := cpc_pre_ok;
    next_symbol (apc, partbufp^, part1len);
    p10getkeyword (apc, partbufp^, keyind);
    IF   keyind = cpc_i_session
    THEN
        BEGIN
        pcan.ancomtyp := cpr_com_abort_session;
        p14kaentryinit (apc);
        END
    ELSE
        p11wrerrorlisting (apc, cpc_unknown_sql_command);
    (*ENDIF*)
    END;
(*ENDWITH*)
m90name(xx, 'END p14abortsessio');
END;

(*------------------------------*)

PROCEDURE
        p14version ;



VAR
      i     : integer;
      err   : tpc_pre_errors;
      ptypentix : tsp_int2;
      pvatyp    : tsp_int2;
      retpos    : tsp_int2;
      lastvatyp : tsp_int2;

BEGIN
m90print('PC14  p14version');
WITH apc, sqlxa, pcscan, sqlca, sqlrap^, pccmdpart, pcsqlva DO
    BEGIN
    retpos := syposacc;
    pcan.ancheck := pcopts.opt_mode;
    pcan.ancomtyp := cpr_com_cancel;
    pcusage.uscom    := cpc_usage_cancel;
    pcusage.usdobjn1 := bsp_knl_identifier;
    pcusage.usdobjn2 := bsp_knl_identifier;
    IF  ralang in [cpr_la_c, cpr_la_cobol]
    THEN
        WHILE  (symb <> cpr_s_eof) AND (symb <> cpr_s_bufeof) DO
            next_symbol (apc, partbufp^, part1len);
        (*ENDWHILE*)
    (*ENDIF*)
    IF  ralang = cpr_la_cobol
    THEN
        BEGIN
        part1len := part1len + 1;
        partbufp^ [part1len] := cpr_nullchar ;
        END;
    (*ENDIF*)
    syposacc := retpos;
    symb     := cpr_s_unknown;
    err  := cpc_pre_ok;
    p14kaentryinit (apc);
    WITH sqlkap^ [pcka ] DO
        BEGIN
        kapaindex := pcpa + 1;
        FOR i := 1 TO 2 DO
            BEGIN
            next_symbol (apc, partbufp^, part1len);
            CASE symb OF
                cpr_s_parameter_name :
                    WITH  pcsqlva, pcsymtab DO
                        BEGIN
                        (* 1.lzu  2.pcr version variable *)
                        syposacc := sypos+1;
                        ptypentix  := 0;
                        p14btypentget (next_symbol, apc, ptypentix,
                              pvatyp, lastvatyp, cpr_is_true);
                        IF   ptypentix = 0
                        THEN
                            BEGIN
                            IF   (pcerror = cpc_pre_ok)
                            THEN
                                BEGIN
                                p11precomerror(apc,cpc_unknown_parametername);
                                p11nerrorlisting ( apc,
                                      vartablep^ [ndtabp^[ndcnt].ndvarentix]
                                      .vaname,
                                      vartablep^[ndtabp^[ndcnt].ndvarentix]
                                      .vanaml,0);
                                END
                            (*ENDIF*)
                            END
                        ELSE
                            IF  NOT (pvatyp in [cpr_vchar, cpr_vcharc,
                                cpr_vbuf, cpr_vstring, cpr_vstring4])
                            THEN
                                BEGIN
                                p11precomerror (apc,
                                      cpc_datatyp_not_compatible);
                                p11nerrorlisting ( apc,
                                      vartablep^ [ndtabp^[ndcnt].ndvarentix]
                                      .vaname,
                                      vartablep^[ndtabp^[ndcnt].ndvarentix]
                                      .vanaml,0);
                                END
                            ELSE
                                BEGIN
                                (* kapaindex pa-area-index *)
                                WITH ndtabp^ [ndcnt] DO
                                    BEGIN
                                    IF  ndsqlva1st <> ndsqlva1ix
                                    THEN
                                        BEGIN
                                        p19cparentry (apc, sqlparst, ndsqlva1st,
                                              0 , ndsqlva1cnt);
                                        kapacount := kapacount + 1;
                                        END;
                                    (*ENDIF*)
                                    p19cparentry (apc, sqlparel,
                                          ndsqlva1ix, 0, 0);
                                    kapacount := kapacount + 1;
                                    END;
                                (*ENDWITH*)
                                END;
                            (*ENDIF*)
                        (*ENDIF*)
                        END;
                    (*ENDWITH*)
                OTHERWISE
                    p11wrerrorlisting (apc, cpc_unknown_sql_command);
                END;
            (*ENDCASE*)
            END;
        (*ENDFOR*)
        IF   (symb <> cpr_s_eof) AND
            (symb <> cpr_s_bufeof)
        THEN
            p11wrerrorlisting (apc, cpc_unknown_sql_command);
        (*ENDIF*)
        END;
    (*ENDWITH*)
    symb := cpr_s_eof;
    END;
(*ENDWITH*)
END;

(*------------------------------*)

PROCEDURE
        p14_hostv_get ;



VAR
      ptypentix : tsp_int2;
      pvatyp    : tsp_int2;
      lastvatyp : tsp_int2;

BEGIN
m90print('PC14  p14_hostv_');
WITH apc, sqlxa, pcscan, sqlca DO
    BEGIN
    m90int(pc, 'pcpr        ', pcpr);
    WITH  sqlprp^ [pcpr ],  sqlkap^ [pcka ],
         pcsqlva, pcsymtab DO
        BEGIN
        (* statementname in hostvar *)
        va1ix    := 0;
        syposacc := sypos+1;
        ptypentix  := 0;
        p14btypentget (next_symbol, apc, ptypentix,
              pvatyp, lastvatyp, cpr_is_true);
        IF   ptypentix = 0
        THEN
            BEGIN
            IF   (pcerror = cpc_pre_ok)
            THEN
                BEGIN
                p11precomerror (apc, cpc_unknown_parametername);
                p11nerrorlisting ( apc,
                      vartablep^ [ndtabp^[ndcnt].ndvarentix].vaname,
                      vartablep^ [ndtabp^[ndcnt].ndvarentix].vanaml, 0);
                END
            (*ENDIF*)
            END
        ELSE
            IF  NOT (pvatyp in [cpr_vchar..cpr_vbuf,
                cpr_vstring, cpr_vstring1, cpr_vucs2, cpr_vutf16, cpr_vunicode, cpr_vunicodec])
            THEN
                BEGIN
                p11precomerror (apc, cpc_datatyp_not_compatible);
                p11nerrorlisting ( apc,
                      vartablep^ [ndtabp^[ndcnt].ndvarentix].vaname,
                      vartablep^ [ndtabp^[ndcnt].ndvarentix].vanaml, 0);
                END;
            (*ENDIF*)
        (*ENDIF*)
        IF   (pcerror = cpc_pre_ok)
        THEN
            BEGIN
            paix  := pcpa + 1;
            WITH ndtabp^ [ndcnt] DO
                BEGIN
                IF  ndsqlva1st <> ndsqlva1ix
                THEN
                    BEGIN
                    p19cparentry (apc, sqlparst, ndsqlva1st,
                          0 , ndsqlva1cnt);
                    END;
                (*ENDIF*)
                p19cparentry (apc, sqlparel,
                      ndsqlva1ix, 0, 0);
                va1ix := ndsqlva1ix;
                END;
            (*ENDWITH*)
            END;
        (*ENDIF*)
        END;
    (*ENDWITH*)
    END;
(*ENDWITH*)
m90int2(pc, 'paix        ', paix);
m90name(xx, 'END p14_hostv_get ');
END;

(*------------------------------*)

PROCEDURE
        p14_prind_get ;



VAR
      ptypentix : tsp_int2;
      pvatyp    : tsp_int2;
      lastvatyp : tsp_int2;
      statemn   : tsp_knl_identifier;
      paix      : tsp_int2;
      ch12      : tsp_c12;
      no        : integer;

BEGIN
m90print('PC14  p14_prind_');
WITH apc, sqlxa, pcscan, sqlca, pccmdpart DO
    BEGIN
    m90int(pc, 'pcpr        ', pcpr);
    WITH  pcsqlva, pcsymtab DO
        BEGIN
        (* statementname in hostvar *)
        (* eliminate parametermarker *)
        paix    := 0;
        statemn := bsp_knl_identifier;
        statemn[1] := ':';
        (*****
              IF  sylen >  sizeof(statemn)
              THEN
              stateml :=  sizeof(statemn)
              ELSE
              stateml := sylen;
              s10mv3 (sizeof(partbufp^), sizeof(statemn), partbufp^, sypos,
              statemn, 1, stateml);
              *******)
        partbufp^ [sypos] := bsp_c1;
        syposacc := sypos+1;
        ptypentix  := 0;
        p14btypentget (next_symbol, apc, ptypentix,
              pvatyp, lastvatyp, cpr_is_true);
        IF   ptypentix = 0
        THEN
            BEGIN
            IF   (pcerror = cpc_pre_ok)
            THEN
                BEGIN
                p11precomerror (apc, cpc_unknown_parametername);
                p11nerrorlisting ( apc,
                      vartablep^ [ndtabp^[ndcnt].ndvarentix].vaname,
                      vartablep^ [ndtabp^[ndcnt].ndvarentix].vanaml, 0);
                END
            (*ENDIF*)
            END
        ELSE
            IF  NOT (pvatyp in [cpr_vchar..cpr_vbuf,
                cpr_vstring, cpr_vstring1, cpr_vucs2, cpr_vutf16, cpr_vunicode, cpr_vunicodec])
            THEN
                BEGIN
                p11precomerror (apc, cpc_datatyp_not_compatible);
                p11nerrorlisting ( apc,
                      vartablep^ [ndtabp^[ndcnt].ndvarentix].vaname,
                      vartablep^ [ndtabp^[ndcnt].ndvarentix].vanaml, 0);
                END;
            (*ENDIF*)
        (*ENDIF*)
        IF   (pcerror = cpc_pre_ok)
        THEN
            BEGIN
            paix  := pcpa + 1;
            WITH ndtabp^ [ndcnt] DO
                BEGIN
                IF  ndsqlva1st <> ndsqlva1ix
                THEN
                    BEGIN
                    p19cparentry (apc, sqlparst, ndsqlva1st,
                          0 , ndsqlva1cnt);
                    END;
                (*ENDIF*)
                p19cparentry (apc, sqlparel,
                      ndsqlva1ix, 0, 0);
                no := ndsqlva1ix;
                p05inttochr12 (no, ch12);
                s10mv18 (mxsp_c12, sizeof(statemn), ch12, 2,
                      statemn, 2, 4);
                END;
            (*ENDWITH*)
            END;
        (**    p14_add_prind(apc, prind, paix);   ***)
        (**    sqlprp^[prind].prStmtName := statemn;  ***)
        p14_add_prind(apc, prind, paix);
        sqlprp^[prind].prStmtName := statemn;
        m90int (pc, 'prind       ', prind);
        m90int2(pc, 'paix        ', paix);
        m90int2(pc, 'pcpr        ', pcpr);
        m90name(xx, 'END p14_prind_get ');
        END;
    (*ENDWITH*)
    END;
(*ENDWITH*)
END;

(*------------------------------*)

PROCEDURE
        p14_add_prind ;



BEGIN
m90print('PC14  p14_add_pr');
WITH apc, sqlxa, sqlca DO
    BEGIN
    m90int (pc, 'pcpr        ', pcpr);
    m90int2(pc, 'paix        ', paix);
    p19sqlprallocate (pcpr);
    WITH sqlprp^[pcpr] DO
        BEGIN
        prstcount := 0;
        prstindex := 0;
        prcuindex := 0;
        prstate := cpr_state_empty;
        prStmtName   := bsp_knl_identifier;
        prStmtNameIndex := paix;
        prCursorName := bsp_knl_identifier;
        prCursorNameIndex := 0;
        prkaindex := pcka;
        prarea := cpr_in_sqlva;   (** alt st ***)
        prind := pcpr;
        END;
    (*ENDWITH*)
    END;
(*ENDWITH*)
m90name(xx, 'END p14_add_prind ');
END;

(*------------------------------*)

PROCEDURE
        p14prepare ;



VAR
      statemn : tsp_knl_identifier;
      stateml : integer;
      pos     : integer;
      len     : integer;
      ind     : integer;
      posab   : integer;
      posch   : integer;
      subch   : char;
      err     : tpc_pre_errors;
      ptypentix : tsp_int2;
      pvatyp    : tsp_int2;
      retpos    : integer;
      keyind    : integer;
      lastvatyp : tsp_int2;
      paix      : tsp_int2;
      va1ix     : tsp_int2;
      kaind     : tsp_int2;
      kaindlast : tsp_int2;
      ch12      : tsp_c12;
      no        : integer;

BEGIN
m90print('PC14  p14prepare');
WITH apc, sqlxa, pcscan, sqlca, sqlrap^, pccmdpart DO
    BEGIN
    xadescpos := 0;
    xadesclen := 0;
    pcan.ancheck := pcopts.opt_mode;
    pcusage.uscom    := cpc_usage_prep;
    pcusage.usdobjn1 := bsp_knl_identifier;
    pcusage.usdobjn2 := bsp_knl_identifier;
    retpos    := syposacc;
    IF  ralang in [cpr_la_c, cpr_la_cobol]
    THEN
        (* if f?ur umstellung next_symbol *)
        WHILE  (symb <> cpr_s_eof) AND (symb <> cpr_s_bufeof) DO
            next_symbol (apc, partbufp^, part1len);
        (*ENDWHILE*)
    (*ENDIF*)
    IF  ralang = cpr_la_cobol
    THEN
        BEGIN
        part1len := part1len + 1;
        partbufp^ [part1len] := cpr_nullchar ;
        END;
    (*ENDIF*)
    syposacc := retpos;
    symb     := cpr_s_unknown;
    IF   pcerror = cpc_pre_ok
    THEN
        BEGIN
        err := cpc_pre_ok;
        m90int (pc, 'pcpr   v    ', pcpr);
        m90int2(pc, 'pcka        ', pcka);
        m90int2(pc, 'ind         ', ind );
        p14kaentryinit (apc);
        p19sqlprallocate (pcpr);
        m90int (pc, 'pcpr   n    ', pcpr);
        m90int2(pc, 'pcka        ', pcka);
        m90int2(pc, 'ind         ', ind );
        WITH sqlkap^ [pcka ] DO
            BEGIN
            kaprindex  := pcpr;
            paix       := 0;
            (** sqlprp^ [pcpr].prStmtNameIndex := 0;***)
            next_symbol (apc, partbufp^, part1len);
            IF   (symb = cpr_s_parameter_name)
                AND (err = cpc_pre_ok)
            THEN
                BEGIN
                m90c30(pc, 'in slow version expand hostvar');
                ind := pcpr;
                p14_hostv_get (next_symbol, apc, paix, va1ix);
                no := va1ix;
                p05inttochr12 (no, ch12);
                statemn := bsp_knl_identifier;
                statemn[1] := ':';
                s10mv18 (mxsp_c12, sizeof(statemn), ch12, 2,
                      statemn, 2, 4);
                m90int (pc, 'pcpr        ', pcpr);
                m90int2(pc, 'pcka        ', pcka);
                m90int2(pc, 'paix        ', paix );
                m90int2(pc, 'va1ix       ', va1ix );
                END
            ELSE
                BEGIN
                p19getname (apc, statemn, stateml);
                IF  partbufp^ [sypos-1] <> '"'
                THEN
                    p05up2casebuf (statemn, 1, stateml);
                (*ENDIF*)
                END;
            (*ENDIF*)
            p19sprepareprname (apc, statemn, ind);
            m90int (pc, 'pcpr        ', pcpr);
            m90int2(pc, 'pcka        ', pcka);
            m90int2(pc, 'ind         ', ind );
            ind := pcpr;
            sqlprp^ [ind ].prkaindex := 0;
            next_symbol (apc, partbufp^, part1len);
            IF  err = cpc_pre_ok
            THEN
                WITH sqlxa, sqlprp^ [ind ] DO
                    BEGIN
                    prstate:= cpr_state_prepare;
                    prarea := cpr_in_sqlva;
                    prStmtNameIndex := paix;
                    (**** 29.07.96****)
                    IF  prkaindex = 0
                    THEN
                        prkaindex := pcka;
                    (*ENDIF*)
                    prDescribe := cpr_is_false;
                    prStmtName     := statemn;
                    prcuindex  := 0;
                    (* into or from *)
                    p10getkeyword (apc, partbufp^, keyind);
                    IF   keyind = cpc_i_into
                    THEN
                        BEGIN
                        xakano := pcka;
                        next_symbol (apc, partbufp^, part1len);
                        xadescpos := sypos;
                        WHILE  (keyind  <> cpc_i_using) AND
                              (keyind  <> cpc_i_from) AND
                              (symb <> cpr_s_eof) AND (symb <> cpr_s_bufeof) DO
                            BEGIN
                            xadesclen := syposacc - xadescpos;
                            next_symbol (apc, partbufp^, part1len);
                            p10getkeyword (apc, partbufp^, keyind);
                            END;
                        (*ENDWHILE*)
                        IF   keyind = cpc_i_using
                        THEN
                            BEGIN
                            next_symbol (apc, partbufp^, part1len);
                            p10getkeyword (apc, partbufp^, keyind);
                            IF   (keyind <>  cpc_i_names)
                                AND (keyind <>  cpc_i_labels)
                                AND (keyind <>  cpc_i_any)
                            THEN
                                p11precomerror (apc, cpc_pre_not_implemented);
                            (*ENDIF*)
                            next_symbol (apc, partbufp^, part1len);
                            END;
                        (*ENDIF*)
                        p10getkeyword (apc, partbufp^, keyind);
                        END;
                    (*ENDIF*)
                    IF   keyind = cpc_i_from
                    THEN
                        next_symbol (apc, partbufp^, part1len);
                    (*ENDIF*)
                    END;
                (*ENDWITH*)
            (*ENDIF*)
            END;
        (*ENDWITH*)
        WITH  sqlprp^ [ind ],  sqlkap^ [pcka ],
             pcsqlva, pcsymtab DO
            BEGIN
            (** statement **)
            IF  ((symb = cpr_s_parameter_name)
                OR   (symb = cpr_s_identifier)) {Horst,29.10.96, PTS 2606 }
                AND (err = cpc_pre_ok)
            THEN
                BEGIN
                (* statement in variabler *)
                IF  (symb = cpr_s_parameter_name)
                THEN
                    syposacc := sypos+1
                ELSE                       {Horst,29.10.96, PTS 2606 }
                    syposacc := sypos;
                (*ENDIF*)
                ptypentix  := 0;
                p14btypentget (next_symbol, apc, ptypentix,
                      pvatyp, lastvatyp, cpr_is_true);
                IF   ptypentix = 0
                THEN
                    BEGIN
                    IF   (pcerror = cpc_pre_ok)
                    THEN
                        BEGIN
                        p11precomerror (apc, cpc_unknown_parametername);
                        p11nerrorlisting ( apc,
                              vartablep^ [ndtabp^[ndcnt].ndvarentix].vaname,
                              vartablep^[ndtabp^[ndcnt].ndvarentix].vanaml,0);
                        END
                    (*ENDIF*)
                    END
                ELSE
                    IF  NOT (pvatyp in [cpr_vchar..cpr_vbuf,
                        cpr_vstring, cpr_vstring1, cpr_vunicode ,cpr_vunicodec,
                        cpr_vucs2, cpr_vutf16])
                    THEN
                        BEGIN
                        p11precomerror (apc, cpc_datatyp_not_compatible);
                        p11nerrorlisting ( apc,
                              vartablep^ [ndtabp^[ndcnt].ndvarentix].vaname,
                              vartablep^[ndtabp^[ndcnt].ndvarentix].vanaml,0);
                        END;
                    (*ENDIF*)
                (*ENDIF*)
                kaStcount := 0;
                kaStindex := pcpa + 1;
                prstcount := 0;
                prstindex := kaStindex;
                prcuindex := 0;
                prCursorName := bsp_knl_identifier;  (*BF*)
                prCursorNameIndex := 0;
                prStmtName   := statemn;
                WITH ndtabp^ [ndcnt] DO
                    BEGIN
                    IF  ndsqlva1st <> ndsqlva1ix
                    THEN
                        BEGIN
                        p19cparentry (apc, sqlparst, ndsqlva1st,
                              0 , ndsqlva1cnt);
                        kaStcount := 1;
                        END;
                    (*ENDIF*)
                    p19cparentry (apc, sqlparel,
                          ndsqlva1ix, 0, 0);
                    kaStcount := kaStcount + 1;
                    END;
                (*ENDWITH*)
                END
            ELSE
                IF   NOT  (((symb = cpr_s_string_literal)  OR
                    (symb = cpr_s_cstring_literal))
                    AND (err = cpc_pre_ok))
                THEN
                    BEGIN
                    IF   err = cpc_pre_ok
                    THEN
                        err := cpc_invalid_parametername
                    (*ENDIF*)
                    END
                ELSE
                    BEGIN
                    (* prepare statement als string *)
                    p10stringliteralget (apc, partbufp^);
                    posab := sypos;
                    posch := 1;
                    WHILE (posch <> 0) DO
                        BEGIN
                        p19snextsubchar (apc, posab, posch, subch);
                        IF   (posch <> 0) AND (subch <> cpr_preparechr)
                        THEN
                            err := cpc_only_question_marks_allowed;
                        (*ENDIF*)
                        posab := posch + 1;
                        END;
                    (*ENDWHILE*)
                    IF   err = cpc_pre_ok
                    THEN
                        BEGIN
                        kaStindex := pcst + 1;
                        prstindex := kaStindex;
                        prarea     := cpr_in_sqlst;
                        prStmtName   := statemn;
                        prCursorName := bsp_knl_identifier;  (*BF*)
                        prCursorNameIndex := 0;  (*BF*)
                        prcuindex  := 0;
                        pos := sypos;
                        WHILE pos <  sypos + sylen  DO
                            BEGIN
                            IF   pos + mxpr_sqlline
                                > sypos + sylen
                            THEN
                                len := sypos + sylen - pos
                            ELSE
                                len := mxpr_sqlline;
                            (*ENDIF*)
                            p19sqlstallocate (pcst);
                            IF  pcst > pcstmax
                            THEN
                                pcstmax := pcst;
                            (*ENDIF*)
                            kaStcount := kaStcount + 1;
                            prstcount := kaStcount;
                            s10mv1 (partsizlen,mxpr_sqlline,
                                  partbufp^, pos,
                                  sqlstp^ [pcst].stline, 1, len);
                            sqlstp^ [pcst].stllen  := len;
                            sqlstp^ [pcst].stkano := pcka;
                            pos := pos + len;
                            END;
                        (*ENDWHILE*)
                        END;
                    (*ENDIF*)
                    END;
                (*ENDIF*)
            (*ENDIF*)
            END;
        (*ENDWITH*)
        IF   err <> cpc_pre_ok
        THEN
            p11precomerror (apc, err);
        (*ENDIF*)
        END;
    (*ENDIF*)
    symb := cpr_s_eof;
    m90int2 (pc, 'xakano      ', xakano   );
    m90int2 (pc, 'xadescpos   ', xadescpos);
    m90int2 (pc, 'xadesclen   ', xadesclen);
    END;
(*ENDWITH*)
m90name(xx, 'END   p14prepa    ');
END;

(*------------------------------*)

PROCEDURE
        p14execute ;



VAR
      statemn : tsp_knl_identifier;
      stateml : integer;
      keyind  : integer;
      prind   : integer;
      posab   : integer;
      pos     : integer;
      len     : integer;
      posch   : integer;
      mnr     : integer;
      subch   : char;
      err     : tpc_pre_errors;
      ptypentix : tsp_int2;
      pvatyp    : tsp_int2;
      kaind     : tsp_int2;
      retpos    : tsp_int2;
      lastvatyp : tsp_int2;
      ndindex   : integer;

BEGIN
m90print('PC14  p14execute');
WITH apc, pcscan, sqlca, sqlrap^, pccmdpart, sqlxa, pcsqlva  DO
    BEGIN
    retpos    := syposacc;
    xadescpos := 0;
    xadesclen := 0;
    IF  ralang in [cpr_la_c, cpr_la_cobol]
    THEN
        (* if f?ur umstellung next_symbol *)
        WHILE  (symb <> cpr_s_eof) AND (symb <> cpr_s_bufeof) DO
            next_symbol (apc, partbufp^, part1len);
        (*ENDWHILE*)
    (*ENDIF*)
    IF  ralang = cpr_la_cobol
    THEN
        BEGIN
        part1len := part1len + 1;
        partbufp^ [part1len] := cpr_nullchar ;
        END;
    (*ENDIF*)
    syposacc := retpos;
    symb     := cpr_s_unknown;
    IF   pcerror = cpc_pre_ok
    THEN
        BEGIN
        err := cpc_pre_ok;
        IF  err = cpc_pre_ok
        THEN
            BEGIN
            immediate := cpc_imm_no;
            next_symbol (apc, partbufp^, part1len);
            p10getkeyword (apc, partbufp^, keyind);
            IF   keyind = cpc_i_immediate
            THEN
                BEGIN
                p14kaentryinit (apc);
                p19sqlprallocate (pcpr);
                p19sqlfaallocate (pcfa);
                statemno := pcka;
                WITH sqlxa.sqlprp^ [pcpr] ,
                     sqlxa.sqlkap^ [pcka ], sqlxa, pcsqlva, pcsymtab DO
                    BEGIN
                    prstcount := 0;
                    prstindex := 0;
                    prcuindex := 0;
                    prstate:= cpr_state_prepare;
                    prStmtName   := bsp_knl_identifier;
                    prStmtNameIndex := 0;
                    prCursorName := bsp_knl_identifier;
                    prCursorNameIndex := 0;
                    prkaindex := pcka;
                    prarea     := cpr_in_sqlva;
                    prDescribe := cpr_is_false;
                    kaprindex  := pcpr;
                    kafaindex  := pcfa;
                    next_symbol (apc, partbufp^, part1len);
                    IF   symb = cpr_s_parameter_name
                    THEN
                        BEGIN
                        (* variable *)
                        immediate := cpc_imm_variable;
                        syposacc := sypos + 1;
                        ptypentix := 0;
                        p14btypentget (next_symbol, apc, ptypentix,
                              pvatyp, lastvatyp, cpr_is_true);
                        IF   ptypentix = 0
                        THEN
                            BEGIN
                            IF   (pcerror = cpc_pre_ok)
                            THEN
                                BEGIN
                                p11precomerror(apc,cpc_unknown_parametername);
                                p11nerrorlisting ( apc,
                                      vartablep^ [ndtabp^[ndcnt]
                                      .ndvarentix].vaname,
                                      vartablep^ [ndtabp^[ndcnt]
                                      .ndvarentix].vanaml, 0);
                                END
                            (*ENDIF*)
                            END
                        ELSE
                            BEGIN
                            WITH ndtabp^ [ndcnt] DO
                                BEGIN
                                kaStindex := pcpa + 1;
                                IF  ndsqlva1st <> ndsqlva1ix
                                THEN
                                    p19cparentry(apc,sqlparst, ndsqlva1st,
                                          0 , ndsqlva1cnt);
                                (*ENDIF*)
                                p19cparentry (apc, sqlparel,
                                      ndsqlva1ix, 0, 0);
                                END;
                            (*ENDWITH*)
                            END;
                        (*ENDIF*)
                        END
                    ELSE
                        BEGIN
                        (* stringconstante *)
                        p10stringliteralget (apc, partbufp^); (*PETER*)
                        immediate := cpc_imm_string;
                        s10fil (partsizlen, partbufp^, 1,
                              sypos-1, bsp_c1);
                        posab := sypos;
                        posch := 1;
                        WHILE (posch <> 0) DO
                            BEGIN
                            p19snextsubchar (apc, posab, posch, subch);
                            IF   subch = cpr_macrochr
                            THEN
                                BEGIN
                                kamacro := cpr_is_true;
                                p19gmacronumber (apc, posch, mnr, posch);
                                err := cpc_macros_not_allowed;
                                END;
                            (*ENDIF*)
                            posab := posch + 1;
                            END;
                        (*ENDWHILE*)
                        prarea     := cpr_in_sqlst;
                        kaStcount := 0;
                        kaStindex := pcst + 1;
                        pos := sypos;
                        WHILE pos <  sypos + sylen  DO
                            BEGIN
                            IF   pos + mxpr_sqlline
                                > sypos + sylen
                            THEN
                                len := sypos + sylen - pos
                            ELSE
                                len := mxpr_sqlline;
                            (*ENDIF*)
                            p19sqlstallocate (pcst);
                            IF  pcst > pcstmax
                            THEN
                                pcstmax := pcst;
                            (*ENDIF*)
                            kaStcount := kaStcount + 1;
                            s10mv1 (partsizlen,mxpr_sqlline,
                                  partbufp^, pos,
                                  sqlstp^ [pcst] .stline, 1, len);
                            sqlstp^ [pcst].stllen  := len;
                            sqlstp^ [pcst].stkano := pcka;
                            pos := pos + len;
                            END;
                        (*ENDWHILE*)
                        syposacc := sypos;
                        next_symbol (apc, partbufp^, part1len);
                        p10getkeyword (apc, partbufp^, keyind);
                        IF   keyind = cpc_i_connect
                        THEN
                            pcan.ancomtyp := cpr_com_sql_conn;
                        (*ENDIF*)
                        END;
                    (*ENDIF*)
                    IF  err = cpc_pre_ok
                    THEN
                        pcan.ancheck := pcopts.opt_mode;
                    (*ENDIF*)
                    sqlxa.xakano := pcka;
                    END
                (*ENDWITH*)
                END
            ELSE
                BEGIN
                (* execute using *)
                ndindex := 0;
                IF   (symb = cpr_s_parameter_name)
                    AND (err = cpc_pre_ok)
                THEN
                    BEGIN
                    m90buf (pc, partbufp^ [1], 1 , part1len );
                    m90int2 (pc, 'syposacc    ', syposacc);
                    m90int2 (pc, 'sypos       ', sypos);
                    p14_prind_get (next_symbol, apc, prind);
                    ndindex := ndcnt;
                    END
                ELSE
                    BEGIN
                    p19getname (apc, statemn, stateml);
                    IF  partbufp^ [sypos-1] <> '"'
                    THEN
                        p05up2casebuf (statemn, 1, stateml);
                    (*ENDIF*)
                    p19sstatementprname (apc, statemn, prind);
                    m90int2 (pc, 'prind       ', prind    );
                    next_symbol (apc, partbufp^, part1len);
                    END;
                (*ENDIF*)
                IF   prind = 0
                THEN
                    err := cpc_unknown_statementname
                ELSE
                    BEGIN
                    m90int2 (pc, 'prind       ', prind    );
                    m90int2 (pc, 'pcka        ', pcka );
                    m90int2 (pc, 'prStmtNameIn', sqlprp^[prind].prStmtNameIndex );
                    IF  sqlprp^[prind].prStmtNameIndex > 0
                    THEN
                        BEGIN
                        p14kaentryinit (apc);
                        m90int2 (pc, 'pcka        ', pcka );
                        WITH sqlkap^[pcka] DO
                            BEGIN
                            kaind := pcka;
                            kaprindex  := prind;
                            END;
                        (*ENDWITH*)
                        END
                    ELSE
                        kaind := sqlprp^ [prind] .prkaindex;
                    (*ENDIF*)
                    m90int2 (pc, 'kaind       ', kaind    );
                    IF   symb = cpr_s_eof
                    THEN
                        BEGIN
                        (* without parameter *)
                        p14kaentryinit (apc);
                        p19sqlfaallocate (pcfa);
                        statemno     := pcka;
                        sqlkap^ [pcka ] :=  sqlkap^ [kaind ];
                        WITH sqlxa.sqlkap^ [pcka ] DO
                            BEGIN
                            katyp  := pcan.ancomtyp;
                            kapaindex := 0;
                            IF  pcinpind = cpc_inputfile
                            THEN
                                kalineno   := pclno.lnosrc
                            ELSE
                                kalineno   := - pclno.lnoexp;
                            (*ENDIF*)
                            kamode    := rakamode;
                            kaatindex := pckaatind;
                            kacuindex := 0;
                            pcan.ancheck := pcopts.opt_mode;
                            pcda  := mxpr_sqln;
                            sqlxa.sqlprp^ [prind] .prDescribe :=
                                  cpr_is_true;
                            (* verweiss auf same prepare stm *)
                            sqlxa.sqlprp^ [prind] .prkaindex := pcka;
                            kadiindex := kaind;
                            kafaindex  := pcfa;
                            END;
                        (*ENDWITH*)
                        END
                    ELSE
                        BEGIN
                        (* using *)
                        p10getkeyword (apc, partbufp^, keyind);
                        IF   keyind = cpc_i_using
                        THEN
                            BEGIN
                            next_symbol (apc, partbufp^, part1len);
                            p10getkeyword (apc, partbufp^, keyind);
                            IF   keyind = cpc_i_descriptor
                            THEN
                                BEGIN
                                p14kaentryinit (apc);
                                p19sqlfaallocate (pcfa);
                                statemno     := pcka;
                                sqlkap^ [pcka ] :=  sqlkap^ [kaind ];
                                WITH sqlxa.sqlkap^ [pcka ] DO
                                    BEGIN
                                    katyp  := pcan.ancomtyp;
                                    kapacount := -1;
                                    kapaindex := 0;
                                    IF  pcinpind = cpc_inputfile
                                    THEN
                                        kalineno   := pclno.lnosrc
                                    ELSE
                                        kalineno   := - pclno.lnoexp;
                                    (*ENDIF*)
                                    kamode    := rakamode;
                                    kaatindex := pckaatind;
                                    kacuindex := 0;
                                    pcan.ancheck := pcopts.opt_mode;
                                    pcda  := mxpr_sqln;
                                    sqlxa.sqlprp^ [prind] .prDescribe :=
                                          cpr_is_true;
                                    (* verweiss auf same prepare stm *)
                                    sqlxa.sqlprp^ [prind] .prkaindex := pcka;
                                    kadiindex := kaind;
                                    kafaindex  := pcfa;
                                    END;
                                (*ENDWITH*)
                                next_symbol (apc, partbufp^, part1len);
                                IF   symb <> cpr_s_eof
                                THEN
                                    BEGIN
                                    IF  partbufp^ [sypos] = cpr_paramchr
                                    THEN
                                        xadescpos := sypos + 1
                                    ELSE
                                        xadescpos := sypos;
                                    (*ENDIF*)
                                    WHILE  (symb <> cpr_s_eof) AND
                                          (symb <> cpr_s_bufeof) DO
                                        BEGIN
                                        xadesclen := syposacc - xadescpos;
                                        next_symbol (apc, partbufp^,
                                              part1len);
                                        END;
                                    (*ENDWHILE*)
                                    END
                                ELSE
                                    BEGIN
                                    IF  (rakamode = cpr_kind_internal)
                                    THEN
                                        (* default sqldb sqlda expansion *)
                                        xadescpos := -1
                                    ELSE
                                        err := cpc_invalid_sql_command;
                                    (*ENDIF*)
                                    END;
                                (*ENDIF*)
                                END
                            ELSE
                                IF   symb <> cpr_s_parameter_name
                                THEN
                                    err := cpc_invalid_parametername
                                ELSE
                                    BEGIN
                                    p14an1sqlstatement (next_symbol, apc);
                                    statemno     := pcka;
                                    sqlxa.xakano := pcka;
                                    sqlkap^ [pcka] .kaprindex := prind;
                                    sqlkap^ [pcka] .kaStcount:=
                                          sqlkap^ [kaind] .kaStcount;
                                    sqlkap^ [pcka] .kaStindex:=
                                          sqlkap^ [kaind] .kaStindex;
                                    (* verweiss auf same prepare stm *)
                                    sqlprp^ [prind] .prkaindex := pcka;
                                    sqlkap^ [pcka] .kadiindex := kaind;
                                    p19sqlfaallocate (pcfa);
                                    sqlkap^ [pcka] .kafaindex  := pcfa;
                                    IF  ndindex > 0
                                    THEN
                                        ndtabp^ [ndindex].ndkano := pcka;
                                    (*ENDIF*)
                                    END
                                (*ENDIF*)
                            (*ENDIF*)
                            END
                        ELSE
                            err := cpc_invalid_sql_command;
                        (*ENDIF*)
                        END;
                    (*ENDIF*)
                    END;
                (*ENDIF*)
                END;
            (*ENDIF*)
            END;
        (*ENDIF*)
        IF   err <> cpc_pre_ok
        THEN
            p11precomerror (apc, err);
        (*ENDIF*)
        END;
    (*ENDIF*)
    m90int2 (pc, 'xakano      ', xakano   );
    m90int2 (pc, 'xadescpos   ', xadescpos);
    m90int2 (pc, 'xadesclen   ', xadesclen);
    symb := cpr_s_eof;
    END;
(*ENDWITH*)
m90name(xx, 'END p14execute    ');
END;

(*------------------------------*)

PROCEDURE
        p14declare ;



VAR
      statemn : tsp_knl_identifier;
      stateml : integer;
      cursorn : tsp_knl_identifier;
      cursorl : integer;
      keyind  : integer;
      ind     : integer;
      kaind   : integer;
      macro  : tsp_int2;
      param  : tsp_int2;
      va1ix     : tsp_int2;
      ch12      : tsp_c12;
      no        : integer;
      rscan  : tpr_scannertype;
      StmtNameIndex  : tsp_int2;
      retcom : tsp_int2;
      withhold : boolean;
      ndindex  : integer;
      ndcuindex  : integer;
      CursorNameIndex  : tsp_int2;
      retStmtNameIndex  : tsp_int2;

BEGIN
m90print('PC14  p14declare');
WITH apc, pcscan, sqlca, sqlrap^, pccmdpart, pcsqlva DO
    BEGIN
    ndindex  := 0;
    keyindex := 0;
    withhold := false;
    IF   pcerror = cpc_pre_ok
    THEN
        BEGIN
        p19sqlprallocate (pcpr);
        next_symbol (apc, partbufp^, part1len);
        p10getkeyword (apc, partbufp^, keyind);
        keyindex := keyind;
        IF  keyind <> cpc_i_table
        THEN
            BEGIN
            p14getname (next_symbol, apc, cursorn, cursorl, macro
                  , param
                  );
            WITH sqlxa, sqlprp^ [pcpr ] DO
                BEGIN
                prstcount := 0;
                prstindex := 0;
                prcuindex := 0;
                prCursorName := bsp_knl_identifier;  (*BF*)
                prCursorNameIndex := 0;
                prStmtName   := bsp_knl_identifier;    (*BD*)
                prStmtNameIndex := 0;
                prstate    := cpr_state_command;
                prarea     := cpr_in_sqlva;
                prkaindex := 0;
                prDescribe := cpr_is_false;
                IF   (param = cpr_is_true)
                THEN
                    BEGIN
                    ind := pcpr;
                    rscan := pcscan;
                    p14_hostv_get (next_symbol, apc, CursorNameIndex, va1ix);
                    no := va1ix;
                    p05inttochr12 (no, ch12);
                    cursorn := bsp_knl_identifier;
                    cursorn[1] := ':';
                    s10mv18 (mxsp_c12, sizeof(cursorn), ch12, 2,
                          cursorn, 2, 4);
                    pcscan := rscan;
                    (* PTS 1104482 *)
                    ndcuindex := ndcnt;
                    END
                ELSE
                    BEGIN
                    CursorNameIndex := 0;
                    ndcuindex := 0;
                    END;
                (*ENDIF*)
                next_symbol (apc, partbufp^, part1len);
                p10getkeyword (apc, partbufp^, keyind);
                keyindex := keyind;
                IF  ( keyind = cpc_i_database )  AND
                    ( ( rakamode = cpr_kind_oracle ) OR
                    ( rakamode = cpr_kind_sapr3 ) )
                THEN
                    BEGIN
                    pcpr := pcpr - 1;
                    p19setprsqlindex (pcpr);
                    p14batdatabase (apc, cursorn);
                    END
                ELSE
                    IF   keyind =   cpc_i_statement
                    THEN
                        BEGIN
                        prstate  := cpr_state_decl_statem;
                        prStmtName   := cursorn;
                        END
                    ELSE
                        BEGIN
                        IF   keyind <>  cpc_i_cursor
                        THEN
                            BEGIN
                            pcpr := pcpr - 1;
                            p19setprsqlindex (pcpr);
                            p11precomerror (apc, cpc_invalid_sql_command);
                            END
                        ELSE
                            BEGIN
                            (* for or with hold *)
                            next_symbol (apc, partbufp^, part1len);
                            p10getkeyword (apc, partbufp^, keyind);
                            IF  (keyind <> cpc_i_for)
                            THEN
                                BEGIN
                                p10getkeyword (apc, partbufp^, keyind);
                                IF  (keyind <> cpc_i_with)
                                THEN
                                    p11precomerror (apc,
                                          cpc_invalid_sql_command);
                                (*  hold  *)
                                (*ENDIF*)
                                next_symbol (apc, partbufp^, part1len);
                                p10getkeyword (apc, partbufp^, keyind);
                                IF  (keyind <> cpc_i_hold)
                                THEN
                                    p11precomerror (apc,
                                          cpc_invalid_sql_command);
                                (* for  *)
                                (*ENDIF*)
                                next_symbol (apc, partbufp^, part1len);
                                p10getkeyword (apc, partbufp^, keyind);
                                IF  (keyind <> cpc_i_for)
                                THEN
                                    p11precomerror (apc,
                                          cpc_invalid_sql_command);
                                (*ENDIF*)
                                withhold := true;
                                IF  rakamode <> cpr_kind_db2
                                THEN
                                    p11precomerror (apc,
                                          cpc_invalid_sql_command);
                                (*ENDIF*)
                                END;
                            (*ENDIF*)
                            next_symbol (apc, partbufp^, part1len);
                            p10getkeyword (apc, partbufp^, keyind);
                            IF  (keyind = cpc_i_select)
                                OR (keyind = cpc_i_with)
                                OR  (symb = cpr_s_leftpar)
                                OR  (symb = cpr_s_leftindpar)
                            THEN
                                BEGIN
                                (* select statement als string *)
                                pcan.ancheck := cpc_ch_no;
                                retcom := pcan.ancomtyp;
                                pcan.ancomtyp := cpr_com_declare_cursor;
                                p14an1sqlstatement (next_symbol, apc);
                                pcan.ancomtyp := retcom;
                                pcan.ancheck := pcopts.opt_mode;
                                statemno := pcka;
                                sqlkap^ [pcka] .katyp     := retcom;
                                sqlkap^ [pcka] .kaprindex := pcpr;
                                sqlkap^ [pcka] .kamacro   := macro;
                                prkaindex := pcka;
                                prarea     := cpr_in_sqlst;
                                prStmtNameIndex := 0;
                                prCursorName  := cursorn;
                                prCursorNameIndex := CursorNameIndex;
                                p14putsqlstarea (apc);
                                END
                            ELSE
                                BEGIN
                                (* statement name *)
                                IF   (symb = cpr_s_parameter_name)
                                THEN
                                    BEGIN
                                    rscan := pcscan;
                                    p14_hostv_get (next_symbol, apc, StmtNameIndex, va1ix);
                                    no := va1ix;
                                    p05inttochr12 (no, ch12);
                                    statemn := bsp_knl_identifier;
                                    statemn[1] := ':';
                                    s10mv18 (mxsp_c12, sizeof(statemn), ch12, 2,
                                          statemn, 2, 4);
                                    pcscan := rscan;
                                    ndindex := ndcnt;
                                    END
                                ELSE
                                    BEGIN
                                    ndindex := 0;
                                    p19getname (apc, statemn, stateml);
                                    IF  partbufp^ [sypos-1] <> '"'
                                    THEN
                                        p05up2casebuf (statemn,1,stateml);
                                    (*ENDIF*)
                                    p19sprepareprname (apc, statemn, ind);
                                    StmtNameIndex := 0;
                                    END;
                                (*ENDIF*)
                                IF   ind = 0
                                THEN
                                    p11precomerror (apc,
                                          cpc_unknown_statementname)
                                ELSE
                                    BEGIN
                                    p14kaentryinit (apc);
                                    statemno := pcka;
                                    prCursorName := cursorn;
                                    prCursorNameIndex := CursorNameIndex;
                                    prStmtName := statemn;
                                    prStmtNameIndex := StmtNameIndex;
                                    IF  withhold
                                    THEN
                                        prstate:= cpr_state_decl_with_hold
                                    ELSE
                                        prstate := cpr_state_command;
                                    (*ENDIF*)
                                    prkaindex :=  pcka;
                                    sqlkap^ [pcka] .kaprindex := pcpr;
                                    sqlkap^ [pcka] .kamacro   := macro;
                                    sqlkap^ [pcka] .kapacount:= 0;
                                    IF  pcinpind = cpc_inputfile
                                    THEN
                                        sqlkap^ [pcka] .kalineno
                                              := pclno.lnosrc
                                    ELSE
                                        sqlkap^ [pcka] .kalineno
                                              := - pclno.lnoexp;
                                    (*ENDIF*)
                                    sqlprp^ [pcpr] .prcuindex := ind;
                                    sqlkap^ [pcka] .kafaindex := 0;
                                    (* PTS 1104482 *)
                                    IF  ndcuindex > 0
                                    THEN
                                        ndtabp^ [ndcuindex].ndkano := pcka;
                                    (*ENDIF*)
                                    IF  ndindex > 0
                                    THEN
                                        ndtabp^ [ndindex].ndkano := pcka;
                                    (*ENDIF*)
                                    END;
                                (*ENDIF*)
                                END;
                            (*ENDIF*)
                            p14bcudeclare ( apc, cursorn );
                            END;
                        (*ENDIF*)
                        END;
                    (*ENDIF*)
                (*ENDIF*)
                END;
            (*ENDWITH*)
            END;
        (*ENDIF*)
        END;
    (*ENDIF*)
    WHILE  (symb <> cpr_s_eof) AND (symb <> cpr_s_bufeof) DO
        next_symbol (apc, partbufp^, part1len);
    (*ENDWHILE*)
    END;
(*ENDWITH*)
m90name(xx, 'END   p14decla    ');
END;

(*------------------------------*)

PROCEDURE
        p14describe ;



VAR
      keyind : integer;
      ind    : integer;
      staten : tsp_knl_identifier;
      statel : integer;
      orakatyp : tsp_int2;
      ndindex  : integer;

BEGIN
m90print('PC14  p14describ');
WITH apc, pcscan, sqlca, sqlrap^, pccmdpart, sqlxa, pcsqlva DO
    BEGIN
    xadescpos := 0;
    xadesclen := 0;
    IF   pcerror = cpc_pre_ok
    THEN
        BEGIN
        next_symbol (apc, partbufp^, part1len);
        orakatyp := cpr_com_empty;
        IF  (rakamode = cpr_kind_oracle)
            OR  (rakamode = cpr_kind_sapr3 )
        THEN
            p14oradescribe (next_symbol, apc, orakatyp);
        (*ENDIF*)
        IF   (symb = cpr_s_parameter_name)
            AND (pcerror = cpc_pre_ok)
        THEN
            BEGIN
            p14_prind_get (next_symbol, apc, ind);
            ndindex := ndcnt;
            END
        ELSE
            BEGIN
            ndindex := 0;
            p19getname (apc, staten, statel);
            IF  partbufp^ [sypos-1] <> '"'
            THEN
                p05up2casebuf (staten, 1, statel);
            (*ENDIF*)
            p19sstatementprname (apc, staten, ind);
            next_symbol (apc, partbufp^, part1len);
            END;
        (*ENDIF*)
        IF   ind = 0
        THEN
            p19scursorprname (apc, staten, ind);
        (*ENDIF*)
        IF   ind = 0
        THEN
            p11precomerror (apc, cpc_unknown_statementname)
        ELSE
            WITH sqlxa, sqlprp^ [ind ] DO
                BEGIN
                p14kaentryinit (apc);
                pcda    := mxpr_sqln;
                IF   prkaindex = 0
                THEN
                    BEGIN
                    (* ka_eintrag generieren *)
                    prkaindex := pcka;
                    (* init sqlka-entry *)
                    WITH sqlkap^ [prkaindex ] DO
                        BEGIN
                        IF  orakatyp = cpr_com_empty
                        THEN
                            katyp   := pcan.ancomtyp
                        ELSE
                            katyp   := orakatyp;
                        (*ENDIF*)
                        kaprindex  := ind;
                        kapacount := -3;
                        END;
                    (*ENDWITH*)
                    END
                ELSE
                    BEGIN
                    sqlkap^ [pcka] := sqlkap^ [prkaindex];
                    IF  orakatyp = cpr_com_empty
                    THEN
                        sqlkap^ [pcka].katyp   := pcan.ancomtyp
                    ELSE
                        sqlkap^ [pcka].katyp   := orakatyp;
                    (*ENDIF*)
                    sqlkap^ [pcka].kapacount := -3;
                    sqlkap^ [pcka].kapaindex :=  0;
                    IF  pcinpind = cpc_inputfile
                    THEN
                        sqlkap^ [pcka] .kalineno   := pclno.lnosrc
                    ELSE
                        sqlkap^ [pcka] .kalineno   := - pclno.lnoexp;
                    (*ENDIF*)
                    sqlkap^ [pcka].kamode     := rakamode;
                    sqlkap^ [pcka]. kaatindex := pckaatind;
                    sqlkap^ [pcka]. kacuindex := 0;
                    (* verweiss auf same prepare stment *)
                    sqlkap^ [pcka].kadiindex  := prkaindex;
                    sqlkap^ [pcka].kafaindex  := 0;
                    sqlprp^ [ind]. prkaindex := pcka;
                    sqlkap^ [pcka].kaprindex  := ind;
                    END;
                (*ENDIF*)
                pcan.ancheck := pcopts.opt_mode;
                sqlxa.xakano := prkaindex;
                IF  ndindex > 0
                THEN
                    ndtabp^ [ndindex].ndkano := prkaindex;
                (*ENDIF*)
                p10getkeyword (apc, partbufp^, keyind);
                IF   (symb = cpr_s_eof) OR (symb = cpr_s_bufeof)
                THEN
                    BEGIN
                    IF  (rakamode = cpr_kind_internal)
                    THEN
                        (* default sqldb sqlda expansion *)
                        xadescpos := -1
                    ELSE
                        p11precomerror (apc, cpc_invalid_sql_command);
                    (*ENDIF*)
                    END
                ELSE
                    BEGIN
                    IF  (keyind = cpc_i_into)
                    THEN
                        BEGIN
                        (* descriptor name *)
                        next_symbol (apc, partbufp^, part1len);
                        IF  partbufp^ [sypos] = cpr_paramchr
                        THEN
                            xadescpos := sypos + 1
                        ELSE
                            xadescpos := sypos;
                        (*ENDIF*)
                        WHILE  (symb <> cpr_s_eof) AND (symb <> cpr_s_bufeof)
                              AND (keyind <> cpc_i_using) DO
                            BEGIN
                            xadesclen := syposacc - xadescpos;
                            next_symbol (apc, partbufp^, part1len);
                            p10getkeyword (apc, partbufp^, keyind);
                            END;
                        (*ENDWHILE*)
                        END
                    ELSE
                        p11precomerror (apc, cpc_invalid_sql_command);
                    (*ENDIF*)
                    IF  (keyind = cpc_i_using)
                    THEN
                        BEGIN
                        next_symbol (apc, partbufp^, part1len);
                        p10getkeyword (apc, partbufp^, keyind);
                        IF   (keyind <>  cpc_i_names)
                            AND (keyind <>  cpc_i_labels)
                            AND (keyind <>  cpc_i_any)
                        THEN
                            p11precomerror (apc, cpc_pre_not_implemented);
                        (*ENDIF*)
                        END;
                    (*ENDIF*)
                    END;
                (*ENDIF*)
                END;
            (*ENDWITH*)
        (*ENDIF*)
        END;
    (*ENDIF*)
    WHILE  (symb <> cpr_s_eof) AND (symb <> cpr_s_bufeof) DO
        next_symbol (apc, partbufp^, part1len);
    (*ENDWHILE*)
    m90int2 (pc, 'xakano      ', xakano   );
    m90int2 (pc, 'xadescpos   ', xadescpos);
    m90int2 (pc, 'xadesclen   ', xadesclen);
    END;
(*ENDWITH*)
m90name(xx, 'END p14describe   ');
END;

(*------------------------------*)

PROCEDURE
        p14close ;



VAR
      retpos    : tsp_int2;
      macro  : tsp_int2;
      param  : tsp_int2;
      cursorn : tsp_knl_identifier;
      cursorl : integer;
      rscan  : tpr_scannertype;
      va1ix     : tsp_int2;
      ch12      : tsp_c12;
      no        : integer;
      CursorNameIndex  : tsp_int2;
      prind : integer;
      paix  : tsp_int2;
      ndcuindex  : integer;

BEGIN
m90print('PC14  p14close  ');
WITH apc, pcscan, sqlca, sqlrap^, pccmdpart, sqlxa   DO
    BEGIN
    retpos := syposacc;
    (* if f?ur umstellung next_symbol *)
    WHILE  (symb <> cpr_s_eof) AND (symb <> cpr_s_bufeof) DO
        next_symbol (apc, partbufp^, part1len);
    (*ENDWHILE*)
    syposacc := retpos;
    symb     := cpr_s_unknown;
    (* close *)
    next_symbol (apc, partbufp^, part1len);
    p14getname (next_symbol, apc, cursorn, cursorl, macro, param );
    WITH sqlxa, sqlprp^ [pcpr ] DO
        BEGIN
        paix := 0;
        p14_add_prind  (apc, prind, paix);
        pcpr := prind;
        WITH sqlxa, sqlprp^ [pcpr ] DO
            BEGIN
            sqlprp^[pcpr].prarea := cpr_in_sqlst;
            sqlprp^[pcpr].prDescribe := cpr_is_false;
            END;
        (*ENDWITH*)
        pcan.ancomtyp    := cpr_com_sql_close;
        pcusage.uscom    := cpc_usage_prep;
        IF  (param = cpr_is_true)
        THEN
            BEGIN
            pcan.ancheck := cpc_ch_no;
            rscan := pcscan;
            p14_hostv_get (next_symbol, apc, CursorNameIndex, va1ix);
            no := va1ix;
            p05inttochr12 (no, ch12);
            cursorn := bsp_knl_identifier;
            cursorn[1] := ':';
            s10mv18 (mxsp_c12, sizeof(cursorn), ch12, 2,
                  cursorn, 2, 4);
            pcscan := rscan;
            (* PTS 1104482 *)
            ndcuindex := pcsqlva.ndcnt;
            sqlprp^[pcpr].prCursorName := cursorn;
            sqlprp^[pcpr].prCursorNameIndex := CursorNameIndex;
            END
        ELSE
            BEGIN
            sqlxa.sqlprp^[pcpr].prCursorName := cursorn;
            (* PTS 1104482 *)
            ndcuindex := 0;
            END;
        (*ENDIF*)
        IF  ndcuindex > 0
        THEN
            pcsqlva.ndtabp^ [ndcuindex].ndkano := pcka+1;
        (*ENDIF*)
        END;
    (*ENDWITH*)
    END;
(*ENDWITH*)
m90name(xx, 'END p14close      ');
END;

(*------------------------------*)

PROCEDURE
        p14forloop ;



VAR
      retposfor : integer;
      retpos : integer;
      itypentix : tsp_int2;
      ivatyp    : tsp_int2;
      iind      : tsp_int2;
      loopcnt   : tsp_int2;
      keyind : integer;
      i      : integer;
      lastvatyp : tsp_int2;
      statemno : integer;
      immediate: integer;
      loopcnterr : tpc_pre_errors;

BEGIN
m90print('PC14  p14forloop');
WITH apc, pcscan, sqlca, sqlrap^, pccmdpart, pcsqlva, sqlxa   DO
    BEGIN
    statemno  := 0;
    retposfor := sypos;
    retpos    := syposacc;
    loopcnterr:= cpc_pre_ok;
    IF  ralang in [cpr_la_c, cpr_la_cobol]
    THEN
        (* if f?ur umstellung next_symbol *)
        WHILE  (symb <> cpr_s_eof) AND (symb <> cpr_s_bufeof) DO
            next_symbol (apc, partbufp^, part1len);
        (*ENDWHILE*)
    (*ENDIF*)
    IF  ralang = cpr_la_cobol
    THEN
        BEGIN
        part1len := part1len + 1;
        partbufp^ [part1len] := cpr_nullchar ;
        END;
    (*ENDIF*)
    symb     := cpr_s_unknown;
    syposacc := retpos;
    m90buf (pc, partbufp^ [1], 1 , part1len );
    IF   pcerror = cpc_pre_ok
    THEN
        BEGIN
        next_symbol (apc, partbufp^, part1len);
        IF  symb = cpr_s_parameter_name
        THEN
            BEGIN
            syposacc := sypos + 1;
            itypentix := 0;
            p14btypentget (next_symbol, apc, itypentix, ivatyp,
                  lastvatyp, cpr_is_true );
            IF  ((ivatyp = cpr_vstruct) OR
                (ivatyp = cpr_varray))
            THEN
                p11wrerrorlisting (apc, cpc_invalid_sql_command)
            ELSE
                BEGIN
                IF  itypentix <> 0
                THEN
                    BEGIN
                    IF  pcpaloop = 0
                    THEN
                        BEGIN
                        (* first entry for sqlsa sastruc = cpr_is_loop_cnt *)
                        iind := ndtabp^ [ndcnt].ndsqlva1ix;
                        p19cparentry (apc, sqlparlo,
                              -iind, 0, 0);
                        pcpaloop := -pcpa;
                        ndtabp^ [ndcnt] .ndkano  := pcka + 1;
                        FOR i := retposfor TO sypos-1 DO
                            partbufp^ [i] := bsp_c1;
                        (*ENDFOR*)
                        (* massen comand *)
                        END;
                    (*ENDIF*)
                    END
                (*ENDIF*)
                END
            (*ENDIF*)
            END
        ELSE
            IF  symb = cpr_s_unsigned_integer
            THEN
                BEGIN
                p10int2unsignedget (apc, partbufp^, loopcnt);
                IF  loopcnt < 1
                THEN
                    BEGIN
                    p11wrerrorlisting (apc, cpc_p_loop_init_wrong);
                    loopcnterr :=  cpc_p_loop_init_wrong;
                    END;
                (*ENDIF*)
                p19cparentry (apc,  sqlparlo,
                      loopcnt, loopcnt, 0);
                pcpaloop := -pcpa;
                FOR i := retposfor TO syposacc-1 DO
                    partbufp^ [i] := bsp_c1;
                (*ENDFOR*)
                next_symbol (apc, partbufp^, part1len);
                END
            ELSE
                p11wrerrorlisting (apc, cpc_invalid_sql_command);
            (*ENDIF*)
        (*ENDIF*)
        END;
    (*ENDIF*)
    IF  pcopts.opt_comp = 0
    THEN
        p11wrerrorlisting (apc, cpc_forloop_not_allowed);
    (*ENDIF*)
    retpos := syposacc;
    (* untersuche ob:  for :loop  execute into .... *)
    (* next_symbol (apc, partbufp^, part1len);   *)
    p10getkeyword (apc, partbufp^, keyind);
    CASE  keyind OF
        cpc_i_open :
            BEGIN
            p14open (next_symbol, apc, statemno);
            IF  statemno > 0
            THEN
                IF  sqlxa.sqlkap^ [statemno].kapacount < 0
                THEN
                    sqlxa.sqlkap^ [statemno].kapacount :=
                          sqlxa.sqlkap^ [statemno].kapacount - 1;   (* +1 *)
                (*ENDIF*)
            (*ENDIF*)
            keyind := cpc_i_open;
            xakano := statemno;
            END;
        cpc_i_execute :
            BEGIN
            p14execute (next_symbol, apc, statemno, immediate);
            IF  statemno > 0
            THEN
                IF  sqlxa.sqlkap^ [statemno].kapacount < 0
                THEN
                    sqlxa.sqlkap^ [statemno].kapacount :=
                          sqlxa.sqlkap^ [statemno].kapacount - 1;   (* +1 *)
                (*ENDIF*)
            (*ENDIF*)
            keyind := cpc_i_execute;
            xakano := statemno;
            END;
        cpc_i_fetch :
            BEGIN
            p14fetch (next_symbol, apc, keyind, retpos);
            IF   keyind = cpc_i_descriptor
            THEN
                BEGIN
                syposacc := retpos;
                p14ansqlstatement (next_symbol, apc);
                sqlxa.sqlkap^ [pcka] .kapacount := -3; (* +1 *)
                pcda := mxpr_sqln;
                END
            ELSE
                BEGIN
                syposacc := retpos;
                p14ansqlstatement (next_symbol, apc);
                END;
            (*ENDIF*)
            END;
        cpc_i_putval :
            BEGIN
            p14putval (next_symbol, apc, keyind, retpos);
            p14ansqlstatement (next_symbol, apc);
            IF  keyind = cpc_i_descriptor
            THEN
                BEGIN
                sqlxa.sqlkap^ [pcka] .kapacount := -2;
                pcda := mxpr_sqln;
                END
            (*ENDIF*)
            END;
        cpc_i_getval :
            BEGIN
            p14getval (next_symbol, apc, keyind, retpos);
            IF   keyind = cpc_i_descriptor
            THEN
                BEGIN
                syposacc := retpos;
                p14ansqlstatement (next_symbol, apc);
                sqlxa.sqlkap^ [pcka] .kapacount := -2; (* +1 *)
                pcda := mxpr_sqln;
                END
            ELSE
                BEGIN
                syposacc := retpos;
                p14ansqlstatement (next_symbol, apc);
                END;
            (*ENDIF*)
            END;
        OTHERWISE:
            BEGIN
            syposacc := retpos;
            p14ansqlstatement (next_symbol, apc);
            END;
        END;
    (*ENDCASE*)
    IF  xakano > 0
    THEN
        BEGIN
        sqlkap^ [xakano].kapaindex := pcpaloop;
        sqlkap^ [xakano].kapacount := sqlkap^ [xakano].kapacount +1;
        m90int2 (pc, 'pcka        ', pcka     );
        m90int2 (pc, 'xakano      ', xakano   );
        m90int2 (pc, 'pcpaloop    ', pcpaloop );
        m90int2 (pc, 'kapacount   ', sqlkap^ [xakano].kapacount );
        END;
    (*ENDIF*)
    IF  pcerror = cpc_pre_ok
    THEN
        pcerror := loopcnterr;
    (*ENDIF*)
    symb := cpr_s_eof;
    END;
(*ENDWITH*)
m90name(xx, 'END p14forloop    ');
END;

(*------------------------------*)

PROCEDURE
        p14fetch ;



VAR
      n       : integer;
      f_pos   : integer;
      stmlen : tsp_int4;
      macro  : tsp_int2;
      param  : tsp_int2;
      cursorn : tsp_knl_identifier;
      cursorl : integer;
      rscan  : tpr_scannertype;
      va1ix     : tsp_int2;
      ch12      : tsp_c12;
      no        : integer;
      CursorNameIndex  : tsp_int2;
      prind : integer;
      paix  : tsp_int2;
      ndcuindex : integer;

BEGIN
m90print('PC14  p14fetch  ');
WITH apc, pcscan, sqlca, sqlrap^, pccmdpart, sqlxa   DO
    BEGIN
    IF  ralang in [cpr_la_c, cpr_la_cobol]
    THEN
        (* if f?ur umstellung next_symbol *)
        WHILE  (symb <> cpr_s_eof) AND (symb <> cpr_s_bufeof) DO
            next_symbol (apc, partbufp^, part1len);
        (*ENDWHILE*)
    (*ENDIF*)
    symb     := cpr_s_unknown;
    xadescpos := 0;
    xadesclen := 0;
    keyind := 0;
    f_pos  := 0;
    pcan.ancomtyp := cpr_com_sql_fetch;
    p10rmspace (partbufp^, cpr_quotsym , cpr_quotsym,
          1, part1len-1, part1len, stmlen);
    IF  ralang <> cpr_la_cobol
    THEN
        partbufp^ [part1len] := chr (0);
    (*ENDIF*)
    m90buf (pc, partbufp^ [1], 1 , part1len );
    (* fetch command *)
    syposacc := 1;
    next_symbol (apc, partbufp^, part1len);
    p19sqlfaallocate (pcfa);
    (* fetch spec oder ergname  *)
    next_symbol (apc, partbufp^, part1len);
    p10getkeyword (apc, partbufp^, keyind);
    IF   keyind = cpc_i_pos
    THEN
        BEGIN
        n := 0;
        REPEAT
            next_symbol (apc, partbufp^, part1len);
            IF  symb = cpr_s_leftpar
            THEN
                n := n + 1;
            (*ENDIF*)
            IF  symb = cpr_s_parameter_name
            THEN
                f_pos := cpc_i_pos;
            (*ENDIF*)
            IF  symb = cpr_s_rightpar
            THEN
                n := n - 1;
            (*ENDIF*)
        UNTIL
            (n = 0) OR (symb = cpr_s_eof) OR (symb = cpr_s_bufeof);
        (*ENDREPEAT*)
        END;
    (*ENDIF*)
    IF  (keyind <> cpc_i_using)
        AND (keyind <> cpc_i_into)
    THEN
        BEGIN
        (* ergname oder fetchspec *)
        IF    (keyind <> cpc_i_next)
            AND     (keyind <> cpc_i_prev)
            AND     (keyind <> cpc_i_last)
            AND     (keyind <> cpc_i_first)
            AND     (keyind <> cpc_i_pos)
        THEN
            BEGIN
            p14getname (next_symbol, apc, cursorn, cursorl, macro
                  , param
                  );
            paix := 0;
            p14_add_prind  (apc, prind, paix);
            pcpr := prind;
            WITH sqlxa, sqlprp^ [pcpr ] DO
                BEGIN
                sqlprp^[pcpr].prarea := cpr_in_sqlst;
                sqlprp^[pcpr].prDescribe := cpr_is_false;
                END;
            (*ENDWITH*)
            IF  (param = cpr_is_true)
            THEN
                BEGIN
                pcan.ancomtyp := cpr_com_sql_fetch;
                pcan.ancheck := cpc_ch_no;
                pcusage.uscom := cpc_usage_empty;
                rscan := pcscan;
                p14_hostv_get (next_symbol, apc, CursorNameIndex, va1ix);
                no := va1ix;
                p05inttochr12 (no, ch12);
                cursorn := bsp_knl_identifier;
                cursorn[1] := ':';
                s10mv18 (mxsp_c12, sizeof(cursorn), ch12, 2,
                      cursorn, 2, 4);
                (* PTS 1104482 *)
                ndcuindex := pcsqlva.ndcnt;
                IF  ndcuindex > 0
                THEN
                    pcsqlva.ndtabp^ [ndcuindex].ndkano := pcka+1;
                (*ENDIF*)
                pcscan := rscan;
                sqlprp^[pcpr].prCursorName := cursorn;
                sqlprp^[pcpr].prCursorNameIndex := CursorNameIndex;
                END
            ELSE
                BEGIN
                sqlxa.sqlprp^[pcpr].prCursorName := cursorn;
                END;
            (*ENDIF*)
            next_symbol (apc, partbufp^, part1len);
            p10getkeyword (apc, partbufp^, keyind);
            END;
        (*ENDIF*)
        END
    ELSE
        BEGIN
        paix := 0;
        p14_add_prind  (apc, prind, paix);
        pcpr := prind;
        WITH sqlxa, sqlprp^ [pcpr ] DO
            BEGIN
            sqlprp^[pcpr].prarea := cpr_in_sqlst;
            sqlprp^[pcpr].prDescribe := cpr_is_false;
            END;
        (*ENDWITH*)
        END;
    (*ENDIF*)
    pos := syposacc;
    IF  (keyind <> cpc_i_using)
        AND (keyind <> cpc_i_into)
    THEN
        BEGIN
        (*  using/ into    *)
        next_symbol (apc, partbufp^, part1len);
        p10getkeyword (apc, partbufp^, keyind);
        END;
    (*ENDIF*)
    IF   keyind <>  cpc_i_descriptor
    THEN
        BEGIN
        next_symbol (apc, partbufp^, part1len);
        p10getkeyword (apc, partbufp^, keyind);
        END;
    (*ENDIF*)
    IF   keyind = cpc_i_descriptor
    THEN
        BEGIN
        (*        sqlprp^[pcpr].prDescribe := cpr_is_true;*)
        IF  f_pos = cpc_i_pos
        THEN
            p11wrerrorlisting (apc, cpc_invalid_sql_command)
        ELSE
            BEGIN
            (* default :descriptor, delete ':' *)
            n := syposacc;
            WHILE (partbufp^ [n] = bsp_c1) DO
                n := n + 1;
            (*ENDWHILE*)
            IF  partbufp^ [n] = cpr_paramchr
            THEN
                partbufp^ [n] := bsp_c1;
            (*ENDIF*)
            xadescpos := n;
            xadesclen := part1len - xadescpos;
            END;
        (*ENDIF*)
        IF  (rakamode = cpr_kind_internal) AND (part1len - n = 0)
        THEN
            BEGIN
            xadescpos := -1
            END
        (*ENDIF*)
        END;
    (*ENDIF*)
    symb     := cpr_s_eof;
    syposacc := 1;
    END;
(*ENDWITH*)
m90name(xx, 'END p14fetch      ');
END;

(*------------------------------*)

PROCEDURE
        p14whenever ;



VAR
      keyind : integer;

BEGIN
m90print('PC14  p14wheneve');
WITH apc, pcscan, pccmdpart DO
    BEGIN
    IF   pcerror = cpc_pre_ok
    THEN
        BEGIN
        pcwheset := cpr_is_true;
        next_symbol (apc, partbufp^, part1len);
        p10getkeyword (apc, partbufp^, keyind);
        CASE keyind OF
            cpc_i_sqlwarning :
                p14_get_action (next_symbol, apc, 1);
            cpc_i_sqlerror :
                p14_get_action (next_symbol, apc, 2);
            cpc_i_sqlexception :
                p14_get_action (next_symbol, apc, 3);
            cpc_i_notfound  :
                p14_get_action (next_symbol, apc, 4);
            cpc_i_not  :
                BEGIN
                next_symbol (apc, partbufp^, part1len);
                p10getkeyword (apc, partbufp^, keyind);
                IF   keyind = cpc_i_found
                THEN
                    p14_get_action (next_symbol, apc, 4)
                ELSE
                    p11wrerrorlisting (apc, cpc_unknown_sql_command);
                (*ENDIF*)
                END;
            cpc_i_sqlbegin :
                p14_get_action (next_symbol, apc, 5);
            cpc_i_sqlend   :
                p14_get_action (next_symbol, apc, 6);
            OTHERWISE:
                p11wrerrorlisting (apc, cpc_unknown_sql_command);
            END;
        (*ENDCASE*)
        END;
    (*ENDIF*)
    WHILE  (symb <> cpr_s_eof) AND (symb <> cpr_s_bufeof) DO
        next_symbol (apc, partbufp^, part1len);
    (*ENDWHILE*)
    END;
(*ENDWITH*)
m90name(xx, 'END p14whenever   ');
END;

(*------------------------------*)

PROCEDURE
        p14_get_action ;



VAR
      i           : integer;
      pos, keyind : integer;

BEGIN
m90print('PC14  p14_get_ac');
WITH apc, pcscan, sqlca, sqlrap^, pccmdpart, pcwhe [ind ] DO
    IF   pcerror = cpc_pre_ok
    THEN
        BEGIN
        pos := syposacc;
        IF  ralang in [cpr_la_c, cpr_la_cobol]
        THEN
            (* if f?ur umstellung next_symbol *)
            WHILE  (symb <> cpr_s_eof) AND (symb <> cpr_s_bufeof) DO
                next_symbol (apc, partbufp^, part1len);
            (*ENDWHILE*)
        (*ENDIF*)
        IF  ralang = cpr_la_cobol
        THEN
            BEGIN
            part1len := part1len + 1;
            partbufp^ [part1len] := cpr_nullchar ;
            END;
        (*ENDIF*)
        syposacc := pos;
        symb := cpr_s_unknown;
        next_symbol (apc, partbufp^, part1len);
        p10getkeyword (apc, partbufp^, keyind);
        CASE keyind OF
            cpc_i_stop :
                BEGIN
                whlen := sylen;
                whact := cpc_i_stop;
                s10mv4 (partsizlen, mxsp_c50, partbufp^, sypos,
                      whstr, 1, sylen);
                END;
            cpc_i_continue :
                BEGIN
                whlen := 0;
                whact := cpc_i_continue;
                END;
            cpc_i_goto, cpc_i_go :
                IF  (ind = 5) OR (ind = 6)
                THEN
                    p11wrerrorlisting (apc, cpc_unknown_sql_command)
                ELSE
                    BEGIN
                    IF   ralang in [cpr_la_cobol, cpr_la_cobmic,
                        cpr_la_cob8860 ]
                    THEN
                        BEGIN
                        IF   keyind = cpc_i_go
                        THEN
                            BEGIN
                            next_symbol (apc, partbufp^, part1len);
                            p10getkeyword (apc, partbufp^, keyind);
                            IF   keyind <> cpc_i_to
                            THEN
                                p11wrerrorlisting (apc, cpc_unknown_sql_command);
                            (*ENDIF*)
                            END;
                        (*ENDIF*)
                        pos := syposacc;
                        END
                    ELSE
                        pos := sypos;
                    (*ENDIF*)
                    IF   ralang =  cpr_la_c
                    THEN
                        BEGIN
                        IF   keyind = cpc_i_go
                        THEN
                            BEGIN
                            next_symbol (apc, partbufp^, part1len);
                            p10getkeyword (apc, partbufp^, keyind);
                            IF   keyind <> cpc_i_to
                            THEN
                                p11wrerrorlisting (apc, cpc_unknown_sql_command);
                            (*ENDIF*)
                            END;
                        (*ENDIF*)
                        partbufp^ [pos]   := 'g';
                        partbufp^ [pos+1] := 'o';
                        partbufp^ [pos+2] := 't';
                        partbufp^ [pos+3] := 'o';
                        FOR i := pos+4 TO syposacc DO
                            partbufp^ [i] := bsp_c1;
                        (*ENDFOR*)
                        END;
                    (*ENDIF*)
                    whlen := part1len - pos;
                    whact := cpc_i_goto;
                    IF   whlen > mxsp_c50
                    THEN
                        BEGIN
                        whlen := mxsp_c50;
                        p11wrerrorlisting (apc, cpc_too_long_wheneverstring)
                        END;
                    (*ENDIF*)
                    i := syposacc;
                    WHILE ((partbufp^ [i] = bsp_c1) AND (i < part1len)) DO
                        i := i + 1;
                    (*ENDWHILE*)
                    IF  partbufp^ [i] = cpr_paramchr
                    THEN
                        partbufp^ [i] := bsp_c1;
                    (*ENDIF*)
                    s10mv4 (partsizlen, mxsp_c50, partbufp^, pos,
                          whstr, 1, whlen);
                    END;
                (*ENDIF*)
            cpc_i_do :
                BEGIN
                IF  (ralang = cpr_la_cobol)
                    OR (ralang = cpr_la_cobmic)
                THEN
                    BEGIN
                    next_symbol (apc, partbufp^, part1len);
                    p10getkeyword (apc, partbufp^, keyind);
                    IF   keyind <> cpc_i_perform
                    THEN
                        p11wrerrorlisting (apc, cpc_unknown_sql_command);
                    (*ENDIF*)
                    END;
                (*ENDIF*)
                pos := syposacc;
                whlen := part1len - pos;
                whact := cpc_i_call;
                IF   whlen > mxsp_c50
                THEN
                    BEGIN
                    whlen := mxsp_c50;
                    p11wrerrorlisting (apc, cpc_too_long_wheneverstring)
                    END;
                (*ENDIF*)
                s10mv4 (partsizlen, mxsp_c50, partbufp^, pos,
                      whstr, 1, whlen);
                END;
            cpc_i_call :
                BEGIN
                pos := syposacc;
                whlen := part1len - pos;
                whact := cpc_i_call;
                IF   whlen > mxsp_c50
                THEN
                    BEGIN
                    whlen := mxsp_c50;
                    p11wrerrorlisting (apc, cpc_too_long_wheneverstring)
                    END;
                (*ENDIF*)
                s10mv4 (partsizlen, mxsp_c50, partbufp^, pos,
                      whstr, 1, whlen);
                END;
            OTHERWISE:
                p11wrerrorlisting (apc, cpc_unknown_sql_command);
            END;
        (*ENDCASE*)
        symb := cpr_s_eof;
        END;
    (*ENDIF*)
(*ENDWITH*)
m90name(xx, 'END p14_get_action');
END;

(*------------------------------*)

PROCEDURE
        p14gmacrono ;



VAR
      err : tpc_pre_errors;
      len : tsp_int4;
      vnam : tsp_lname;
      vnaml: integer;
      ptypentix  : tsp_int2;
      pvatyp     : tsp_int2;
      retpos     : tsp_int2;
      lastvatyp  : tsp_int2;
      setupper   : boolean;
      i          : integer;
      pos        : integer;
      mlen       : integer;

BEGIN
m90print('PC14  p14gmacron');
WITH apc, pcscan, sqlca, sqlrap^, pccmdpart,
     sqlxa, pcsqlva, pcsymtab DO
    BEGIN
    retpos := syposacc;
    IF  ralang in [cpr_la_c]
    THEN
        WHILE  (symb <> cpr_s_eof) AND (symb <> cpr_s_bufeof) DO
            next_symbol (apc, partbufp^, part1len);
        (*ENDWHILE*)
    (*ENDIF*)
    syposacc := retpos;
    symb     := cpr_s_unknown;
    IF   pcerror = cpc_pre_ok
    THEN
        BEGIN
        err := cpc_pre_ok;
        next_symbol (apc, partbufp^, part1len);
        IF   symb <> cpr_s_macro
        THEN
            err := cpc_unknown_sql_command
        ELSE
            BEGIN
            p14kaentryinit (apc);
            next_symbol (apc, partbufp^, part1len);
            IF   symb <> cpr_s_unsigned_integer
            THEN
                err := cpc_invalid_macro_string
            ELSE
                BEGIN
                p10int2unsignedget (apc, partbufp^, macnr);
                next_symbol (apc, partbufp^, part1len);
                IF   (macnr <=0) OR (macnr > 128)
                THEN
                    BEGIN
                    macnr := 1;
                    err := cpc_unknown_macronumber;
                    END
                ELSE
                    IF   symb = cpr_s_equal
                    THEN
                        BEGIN
                        next_symbol (apc, partbufp^, part1len);
                        IF   symb = cpr_s_identifier
                        THEN
                            BEGIN
                            IF  ralang = cpr_la_cobol
                            THEN
                                BEGIN
                                retpos := sypos ;
                                WHILE  (symb <> cpr_s_eof)
                                      AND (symb <> cpr_s_bufeof) DO
                                    next_symbol (apc, partbufp^, part1len);
                                (*ENDWHILE*)
                                sypos := retpos ;
                                END;
                            (**24.11.94 **)
                            (*ENDIF*)
                            IF  partbufp^ [sypos-1] = '"'
                            THEN
                                BEGIN
                                setupper := false;
                                p10rmspace (partbufp^,cpr_quotsym, cpr_quotsym,
                                      sypos-1, part1len-1,
                                      part1len, len);
                                len := len -2;
                                END
                            ELSE
                                BEGIN
                                setupper := true;
                                p10rmspace (partbufp^,cpr_quotsym, cpr_quotsym,
                                      sypos, part1len-1,
                                      part1len, len);
                                END;  (*ENDIF*)
                            (*ENDIF*)
                            IF   len > mxpr_macroline
                            THEN
                                err := cpc_invalid_macro_string
                            ELSE
                                BEGIN
                                m90int2 (pc, 'len         ', len      );
                                s10mv6 (partsizlen, mxpr_macroline,
                                      partbufp^, sypos,
                                      sqlmap^.maentry [macnr] .maval, 1,
                                      len);
                                mlen := 0;
                                pos := sypos;
                                REPEAT
                                    IF  (partbufp^ [pos] = '"')
                                        AND  (partbufp^ [pos+1] = '"')
                                    THEN
                                        pos := pos + 1;
                                    (*ENDIF*)
                                    mlen := mlen + 1;
                                    sqlmap^.maentry [macnr].maval[mlen]
                                          := partbufp^ [pos];
                                    pos := pos + 1;
                                UNTIL
                                    (mlen >= mxpr_macroline)
                                    OR (pos >= sypos + len);
                                (*ENDREPEAT*)
                                sqlmap^.maentry [macnr] .malen :=  mlen;
                                (***24.11.94 ****)
                                IF  (setupper)
                                THEN
                                    WITH  sqlmap^.maentry [macnr] DO
                                        FOR i := 1 TO len DO
                                            IF   (maval [i]  in [ 'a'..'i',
                                                'j'..'r',  's'..'z'] )
                                            THEN
                                                maval[i] := chr(ord(maval[i])
                                                      + ord('A') - ord('a'));
                                            (*ENDIF*)
                                        (*ENDFOR*)
                                    (*ENDWITH*)
                                (*ENDIF*)
                                END;
                            (*ENDIF*)
                            END
                        ELSE
                            IF   symb = cpr_s_parameter_name
                            THEN
                                BEGIN
                                syposacc := sypos + 1;
                                ptypentix := 0;
                                retpos := sypos;
                                p14btypentget (next_symbol, apc, ptypentix,
                                      pvatyp, lastvatyp, cpr_is_true);
                                syposacc := sypos;
                                IF   ptypentix = 0
                                THEN
                                    BEGIN
                                    p11precomerror(apc,cpc_unknown_parametername);
                                    vnam:= 'MACRO_PARAMETER                 ';
                                    vnaml:= 16;
                                    p11nerrorlisting ( apc, vnam, vnaml, 0);
                                    END
                                ELSE
                                    BEGIN
                                    sqlmap^.maentry [macnr] .malen :=
                                          - pcpa - 1;
                                    WITH ndtabp^ [ndcnt] DO
                                        BEGIN
                                        IF  ndsqlva1st <> ndsqlva1ix
                                        THEN
                                            BEGIN
                                            p19cparentry (apc, sqlparst,
                                                  ndsqlva1st,
                                                  0 , ndsqlva1cnt);
                                            END;
                                        (*ENDIF*)
                                        p19cparentry (apc, sqlparel,
                                              ndsqlva1ix, 0, 0);
                                        END;
                                    (*ENDWITH*)
                                    s10fil1 (mxpr_macroline,
                                          sqlmap^.maentry [macnr] .maval, 1,
                                          mxpr_macroline, bsp_c1);
                                    IF  NOT (pvatyp in [cpr_vchar..cpr_vbuf,
                                        cpr_vstring, cpr_vstring1])
                                    THEN
                                        err := cpc_datatyp_not_compatible
                                    (*ENDIF*)
                                    END;
                                (*ENDIF*)
                                IF  ralang = cpr_la_cobol
                                THEN
                                    WHILE  (symb <> cpr_s_eof) AND
                                          (symb <> cpr_s_bufeof) DO
                                        next_symbol (apc, partbufp^,
                                              part1len);
                                    (*ENDWHILE*)
                                (*ENDIF*)
                                IF  ( symb <> cpr_s_eof ) AND
                                    ( symb <> cpr_s_bufeof )
                                THEN
                                    BEGIN
                                    err := cpc_missing_endsign ;
                                    sypos := retpos;
                                    END
                                (*ENDIF*)
                                END
                            ELSE
                                err := cpc_invalid_macro_string
                            (*ENDIF*)
                        (*ENDIF*)
                        END
                    ELSE
                        err := cpc_invalid_macro_string
                    (*ENDIF*)
                (*ENDIF*)
                END
            (*ENDIF*)
            END
        (*ENDIF*)
        END;
    (*ENDIF*)
    symb := cpr_s_eof;
    END;
(*ENDWITH*)
IF   err <> cpc_pre_ok
THEN
    p11wrerrorlisting (apc, err);
(*ENDIF*)
m90name(xx, 'END p14gmacrono   ');
END;

(*------------------------------*)

PROCEDURE
        p14nolog ;



VAR
      err : tpc_pre_errors;
      retpos     : tsp_int2;

BEGIN
m90print('PC14  p14nolog  ');
WITH apc, pcscan, sqlca, sqlrap^, pccmdpart,
     sqlxa, pcsqlva, pcsymtab DO
    BEGIN
    retpos := syposacc;
    IF  ralang in [cpr_la_c]
    THEN
        WHILE  (symb <> cpr_s_eof) AND (symb <> cpr_s_bufeof) DO
            next_symbol (apc, partbufp^, part1len);
        (*ENDWHILE*)
    (*ENDIF*)
    syposacc := retpos;
    symb     := cpr_s_unknown;
    IF   pcerror = cpc_pre_ok
    THEN
        BEGIN
        err := cpc_pre_ok;
        next_symbol (apc, partbufp^, part1len);
        IF   (symb = cpr_s_eof) OR   (symb = cpr_s_bufeof)
        THEN
            BEGIN
            (* set nolog flag in gaentry *)
            sqlgap^.gaentry [raactsession]^.ganolog := cpr_is_true;
            END
        ELSE
            p11wrerrorlisting (apc, cpc_invalid_sql_command);
        (*ENDIF*)
        END;
    (*ENDIF*)
    END;
(*ENDWITH*)
END;

(*------------------------------*)

PROCEDURE
        p14option ;



VAR
      statemn : tsp_knl_identifier;
      stateml : integer;
      keyind  : integer;
      prind   : integer;
      posab   : integer;
      pos     : integer;
      len     : integer;
      posch   : integer;
      mnr     : integer;
      subch   : char;
      err     : tpc_pre_errors;
      ptypentix : tsp_int2;
      pvatyp    : tsp_int2;
      kaind     : tsp_int2;
      retpos    : tsp_int2;
      lastvatyp : tsp_int2;
      ndindex   : integer;

BEGIN
m90print('PC14  p14option ');
WITH apc, pcscan, sqlca, sqlrap^, pccmdpart, sqlxa, pcsqlva  DO
    BEGIN
    retpos    := syposacc;
    xadescpos := 0;
    xadesclen := 0;
    (* if f?ur umstellung next_symbol *)
    WHILE  (symb <> cpr_s_eof) AND (symb <> cpr_s_bufeof) DO
        next_symbol (apc, partbufp^, part1len);
    (*ENDWHILE*)
    syposacc := retpos;
    symb     := cpr_s_unknown;
    IF   pcerror = cpc_pre_ok
    THEN
        BEGIN
        err := cpc_pre_ok;
        IF  err = cpc_pre_ok
        THEN
            BEGIN
            (*next_symbol (apc, partbufp^, part1len);*)
            p10getkeyword (apc, partbufp^, keyind);
            p14kaentryinit (apc);
            p19sqlprallocate (pcpr);
            p19sqlfaallocate (pcfa);
            statemno := pcka;
            WITH sqlxa.sqlprp^ [pcpr] ,
                 sqlxa.sqlkap^ [pcka ], sqlxa, pcsqlva, pcsymtab DO
                BEGIN
                prstcount := 0;
                prstindex := 0;
                prcuindex := 0;
                prstate:= cpr_state_prepare;
                prStmtName   := bsp_knl_identifier;
                prStmtNameIndex := 0;
                prCursorName := bsp_knl_identifier;
                prCursorNameIndex := 0;
                prkaindex := pcka;
                prarea     := cpr_in_sqlva;
                prDescribe := cpr_is_false;
                kaprindex  := pcpr;
                kafaindex  := pcfa;
                next_symbol (apc, partbufp^, part1len);
                IF   symb = cpr_s_parameter_name
                THEN
                    BEGIN
                    (* variable *)
                    syposacc := sypos + 1;
                    ptypentix := 0;
                    p14btypentget (next_symbol, apc, ptypentix,
                          pvatyp, lastvatyp, cpr_is_true);
                    IF   ptypentix = 0
                    THEN
                        BEGIN
                        IF   (pcerror = cpc_pre_ok)
                        THEN
                            BEGIN
                            p11precomerror(apc,cpc_unknown_parametername);
                            p11nerrorlisting ( apc,
                                  vartablep^ [ndtabp^[ndcnt]
                                  .ndvarentix].vaname,
                                  vartablep^ [ndtabp^[ndcnt]
                                  .ndvarentix].vanaml, 0);
                            END
                        (*ENDIF*)
                        END
                    ELSE
                        BEGIN
                        WITH ndtabp^ [ndcnt] DO
                            BEGIN
                            kaStindex := pcpa + 1;
                            IF  ndsqlva1st <> ndsqlva1ix
                            THEN
                                p19cparentry(apc,sqlparst, ndsqlva1st,
                                      0 , ndsqlva1cnt);
                            (*ENDIF*)
                            p19cparentry (apc, sqlparel,
                                  ndsqlva1ix, 0, 0);
                            END;
                        (*ENDWITH*)
                        END;
                    (*ENDIF*)
                    END
                ELSE
                    BEGIN
                    (* stringconstante *)
                    pcan.ancheck := cpc_ch_no;
                    p10stringliteralget (apc, partbufp^);
                    s10fil (partsizlen, partbufp^, 1,
                          sypos-1, bsp_c1);
                    posab := sypos;
                    posch := 1;
                    WHILE (posch <> 0) DO
                        BEGIN
                        p19snextsubchar (apc, posab, posch, subch);
                        IF   subch = cpr_macrochr
                        THEN
                            BEGIN
                            kamacro := cpr_is_true;
                            p19gmacronumber (apc, posch, mnr, posch);
                            IF   (mnr <= 0) OR (mnr > 128)
                            THEN
                                err := cpc_invalid_macronumber;
                            (*ENDIF*)
                            END;
                        (*ENDIF*)
                        posab := posch + 1;
                        END;
                    (*ENDWHILE*)
                    prarea     := cpr_in_sqlst;
                    kaStcount := 0;
                    kaStindex := pcst + 1;
                    pos := sypos;
                    WHILE pos <  sypos + sylen  DO
                        BEGIN
                        IF   pos + mxpr_sqlline
                            > sypos + sylen
                        THEN
                            len := sypos + sylen - pos
                        ELSE
                            len := mxpr_sqlline;
                        (*ENDIF*)
                        p19sqlstallocate (pcst);
                        IF  pcst > pcstmax
                        THEN
                            pcstmax := pcst;
                        (*ENDIF*)
                        kaStcount := kaStcount + 1;
                        s10mv1 (partsizlen,mxpr_sqlline,
                              partbufp^, pos,
                              sqlstp^ [pcst] .stline, 1, len);
                        sqlstp^ [pcst].stllen  := len;
                        sqlstp^ [pcst].stkano := pcka;
                        pos := pos + len;
                        END;
                    (*ENDWHILE*)
                    syposacc := sypos;
                    END;
                (*ENDIF*)
                sqlxa.xakano := pcka;
                END;
            (*ENDWITH*)
            END;
        (*ENDIF*)
        IF   err <> cpc_pre_ok
        THEN
            p11precomerror (apc, err);
        (*ENDIF*)
        END;
    (*ENDIF*)
    m90int2 (pc, 'xakano      ', xakano   );
    m90int2 (pc, 'xadescpos   ', xadescpos);
    m90int2 (pc, 'xadesclen   ', xadesclen);
    symb := cpr_s_eof;
    END;
(*ENDWITH*)
END;

(*------------------------------*)

PROCEDURE
        p14gdbsname ;



VAR
      keyind : integer;
      dbsname: tsp_dbname;
      dbnode : tsp_nodeid;
      vnam   : tsp_lname;
      pcnt   : integer;
      vnaml  : integer;
      len    : integer;
      lastvatyp : tsp_int2;
      ptypentix : tsp_int2;
      pvatyp    : tsp_int2;
      vix       : tsp_int2;
      pvalen    : tsp_int2;
      retpos    : integer;

BEGIN
m90print('PC14  p14gdbsnam');
WITH apc, pcscan, sqlca, sqlrap^, pccmdpart, sqlxa,
     pcsqlva, pcsymtab  DO
    BEGIN
    pcnt      := 0;
    dbpos     := 1;
    dblen     := 0;
    dbnodepos := 1;
    dbnodelen := 0;
    retpos := syposacc;
    IF  ralang in [cpr_la_c, cpr_la_cobol]
    THEN
        (* if f?ur umstellung next_symbol *)
        WHILE  (symb <> cpr_s_eof) AND (symb <> cpr_s_bufeof) DO
            next_symbol (apc, partbufp^, part1len);
        (*ENDWHILE*)
    (*ENDIF*)
    IF  ralang = cpr_la_cobol
    THEN
        BEGIN
        part1len := part1len + 1;
        partbufp^ [part1len] := cpr_nullchar ;
        END;
    (*ENDIF*)
    syposacc := retpos;
    symb     := cpr_s_unknown;
    IF   pcerror = cpc_pre_ok
    THEN
        BEGIN
        m90buf (pc, partbufp^ [1], 1 , part1len );
        ind   := raactsession;
        next_symbol (apc, partbufp^, part1len);
        (* dbname analyse *)
        p14kaentryinit (apc);
        IF   (symb = cpr_s_string_literal)
            OR  (symb = cpr_s_cstring_literal)
        THEN
            BEGIN
            p10stringliteralget (apc, partbufp^);
            IF   sylen > mxsp_dbname
            THEN
                p11wrerrorlisting (apc, cpc_invalid_sql_command)
            ELSE
                BEGIN
                dbsname := bsp_dbname;
                sqlgap^.gaentry [ind]^ .gavaindex := 0;
                s10mv15 (partsizlen, mxsp_dbname, partbufp^, sypos,
                      dbsname, 1, sylen);
                dbpos := sypos;
                dblen := sylen;
                next_symbol (apc, partbufp^, part1len);
                IF   sqlgap^.gaentry [ind]^.gaxuserrec.xu_serverdb
                    = bsp_dbname
                THEN
                    sqlgap^.gaentry [ind]^.gaxuserrec.xu_serverdb
                          := dbsname;
                (*ENDIF*)
                END
            (*ENDIF*)
            END
        ELSE
            IF   symb = cpr_s_parameter_name
            THEN
                BEGIN
                pcnt  := pcnt + 1;
                dbpos := sypos + 1;
                syposacc := sypos + 1;
                ptypentix := 0;
                p14btypentget (next_symbol, apc, ptypentix,
                      pvatyp, lastvatyp, cpr_is_true);
                dblen := sypos  - dbpos;
                WHILE partbufp^ [dbpos+dblen-1] in [bsp_c1, cpr_pc_endsign] DO
                    dblen := dblen - 1;
                (*ENDWHILE*)
                IF   ptypentix = 0
                THEN
                    BEGIN
                    p11precomerror (apc, cpc_unknown_parametername);
                    vnam := 'SERVERDB_PARAMETER              ';
                    vnaml:= 20;
                    p11nerrorlisting ( apc, vnam, vnaml, pcnt);
                    END
                ELSE
                    BEGIN
                    sqlgap^.gaentry [ind]^ .gavaindex :=  pcpa + 1;
                    WITH ndtabp^ [ndcnt] DO
                        BEGIN
                        IF  ndsqlva1st <> ndsqlva1ix
                        THEN
                            BEGIN
                            p19cparentry (apc, sqlparst, ndsqlva1st,
                                  0 , ndsqlva1cnt);
                            END;
                        (*ENDIF*)
                        p19cparentry (apc, sqlparel,
                              ndsqlva1ix, 0, 0);
                        vix    := ndtabp^ [ndcnt].ndsqlva1ix;
                        IF  sqlv1p^[vix].va1indi_sc = sqlvapt
                        THEN
                            vix :=  sqlv1p^[vix].va1ix_pt;
                        (*ENDIF*)
                        pvalen := sqlv2p^[sqlv1p^[vix].va1indva2_sc].va2size;
                        END;
                    (*ENDWITH*)
                    IF  NOT (pvatyp in [cpr_vchar..cpr_vbuf,
                        cpr_vstring,  cpr_vstring1])
                    THEN
                        p11wrerrorlisting (apc, cpc_datatyp_not_compatible)
                    ELSE
                        IF  pvalen > 0
                        THEN
                            BEGIN
                            len := mxsp_dbname;
                            IF   pvatyp = cpr_vcharc
                            THEN
                                len := len + 1;
                            (*ENDIF*)
                            IF   len < pvalen
                            THEN
                                BEGIN
                                p11wrerrorlisting (apc,
                                      cpc_variable_will_be_truncated);
                                vnam := 'SERVERDB_PARAMETER              ';
                                vnaml:= 20;
                                p11nerrorlisting ( apc, vnam, vnaml, pcnt);
                                pcerror := cpc_pre_ok;
                                END;
                            (*ENDIF*)
                            END;
                        (*ENDIF*)
                    (*ENDIF*)
                    END
                (*ENDIF*)
                END
            ELSE
                p11wrerrorlisting (apc, cpc_invalid_sql_command);
            (*ENDIF*)
        (*ENDIF*)
        END;
    (*ENDIF*)
    IF   (symb = cpr_s_eof) OR   (symb = cpr_s_bufeof)
    THEN
        BEGIN
        (* servernode  = blank *)
        dbnodepos := 1;
        dbnodelen := 0;
        sqlgap^.gaentry [ind]^.gaxuserrec.xu_servernode := bsp_nodeid;
        sqlgap^.gaentry [ind]^ .ganodeindex := 0;
        END
    ELSE
        BEGIN
        p10getkeyword (apc, partbufp^, keyind);
        IF  keyind <> cpc_i_on
        THEN
            p11wrerrorlisting (apc, cpc_invalid_sql_command)
        ELSE
            BEGIN
            next_symbol (apc, partbufp^, part1len);
            (* servernode  analyse *)
            IF   (symb = cpr_s_string_literal)
                OR  (symb = cpr_s_cstring_literal)
            THEN
                BEGIN
                p10stringliteralget (apc, partbufp^);
                IF   sylen > mxsp_nodeid
                THEN
                    p11wrerrorlisting (apc, cpc_invalid_sql_command)
                ELSE
                    BEGIN
                    dbnode := bsp_nodeid;
                    sqlgap^.gaentry [ind]^ .ganodeindex := 0;
                    s10mv16 (partsizlen, mxsp_nodeid, partbufp^, sypos,
                          dbnode, 1, sylen);
                    dbnodepos := sypos;
                    dbnodelen := sylen;
                    next_symbol (apc, partbufp^, part1len);
                    IF   sqlgap^.gaentry [ind]^ .gaxuserrec.xu_servernode
                        = bsp_nodeid
                    THEN
                        sqlgap^.gaentry [ind]^ .gaxuserrec.xu_servernode
                              := dbnode;
                    (*ENDIF*)
                    END
                (*ENDIF*)
                END
            ELSE
                IF   symb = cpr_s_parameter_name
                THEN
                    BEGIN
                    pcnt  := pcnt + 1;
                    dbnodepos := sypos + 1;
                    syposacc := sypos + 1;
                    ptypentix := 0;
                    p14btypentget (next_symbol, apc, ptypentix,
                          pvatyp, lastvatyp, cpr_is_true);
                    dbnodelen := sypos  - dbnodepos;
                    WHILE partbufp^ [dbnodepos+dbnodelen-1]
                          in [bsp_c1, cpr_pc_endsign] DO
                        dbnodelen := dbnodelen - 1;
                    (*ENDWHILE*)
                    IF   ptypentix = 0
                    THEN
                        BEGIN
                        p11precomerror (apc, cpc_unknown_parametername);
                        vnam := 'SERVERNODE_PARAMETER            ';
                        vnaml:= 22;
                        p11nerrorlisting ( apc, vnam, vnaml, pcnt);
                        END
                    ELSE
                        BEGIN
                        IF   (symb <> cpr_s_eof) AND  (symb <> cpr_s_bufeof)
                        THEN
                            next_symbol (apc, partbufp^, part1len);
                        (*ENDIF*)
                        IF   (symb = cpr_s_eof) OR  (symb = cpr_s_bufeof)
                        THEN
                            BEGIN
                            sqlgap^.gaentry [ind]^ .ganodeindex := pcpa + 1;
                            WITH ndtabp^ [ndcnt] DO
                                BEGIN
                                IF  ndsqlva1st <> ndsqlva1ix
                                THEN
                                    BEGIN
                                    p19cparentry (apc, sqlparst, ndsqlva1st,
                                          0 , ndsqlva1cnt);
                                    END;
                                (*ENDIF*)
                                p19cparentry (apc, sqlparel,
                                      ndsqlva1ix, 0, 0);
                                vix    := ndtabp^ [ndcnt].ndsqlva1ix;
                                IF  sqlv1p^[vix].va1indi_sc = sqlvapt
                                THEN
                                    vix :=  sqlv1p^[vix].va1ix_pt;
                                (*ENDIF*)
                                pvalen := sqlv2p^[sqlv1p^[vix].va1indva2_sc]
                                      .va2size;
                                END;
                            (*ENDWITH*)
                            END
                        ELSE
                            p11wrerrorlisting (apc, cpc_invalid_sql_command);
                        (*ENDIF*)
                        IF  NOT (pvatyp in [cpr_vchar..cpr_vbuf,
                            cpr_vstring,  cpr_vstring1])
                        THEN
                            p11wrerrorlisting (apc, cpc_datatyp_not_compatible)
                        ELSE
                            IF  pvalen > 0
                            THEN
                                BEGIN
                                len := mxsp_nodeid;
                                IF   pvatyp = cpr_vcharc
                                THEN
                                    len := len + 1;
                                (*ENDIF*)
                                IF   len < pvalen
                                THEN
                                    BEGIN
                                    p11wrerrorlisting (apc,
                                          cpc_variable_will_be_truncated);
                                    vnam:= 'SERVERNODE_PARAMETER            ';
                                    vnaml:= 22;
                                    p11nerrorlisting (apc, vnam, vnaml, pcnt);
                                    pcerror := cpc_pre_ok;
                                    END;
                                (*ENDIF*)
                                END;
                            (*ENDIF*)
                        (*ENDIF*)
                        END
                    (*ENDIF*)
                    END
                ELSE
                    p11wrerrorlisting (apc, cpc_invalid_sql_command);
                (*ENDIF*)
            (*ENDIF*)
            END;
        (*ENDIF*)
        END;
    (*ENDIF*)
    symb := cpr_s_eof;
    END;
(*ENDWITH*)
m90name(xx, 'END p14gdbsname   ');
END;

(*------------------------------*)

PROCEDURE
        p14gtrace ;



BEGIN
m90print('PC14  p14gtrace ');
WITH apc, pcscan, sqlca, pccmdpart, sqlgap^, sqloap^ DO
    BEGIN
    CASE keyind OF
        cpc_i_off  :
            oamodsettrty := cpr_trace_off;
        cpc_i_on   :
            oamodsettrty := cpr_trace_formal;
        cpc_i_long :
            oamodsettrty := cpr_trace_long;
        OTHERWISE:
            p11precomerror (apc, cpc_unknown_sql_command);
        END;
    (*ENDCASE*)
    IF  pcerror = cpc_pre_ok
    THEN
        BEGIN
        next_symbol (apc, partbufp^, part1len);
        IF  ( symb <> cpr_s_eof ) AND ( symb <> cpr_s_bufeof )
        THEN
            p11precomerror ( apc, cpc_missing_endsign );
        (*ENDIF*)
        END;
    (*ENDIF*)
    WHILE NOT (symb in [cpr_s_eof, cpr_s_bufeof]) DO
        next_symbol (apc, partbufp^, part1len);
    (*ENDWHILE*)
    END;
(*ENDWITH*)
m90name(xx, 'END p14gtrace     ');
END;

(*------------------------------*)

PROCEDURE
        p14traceline ;



BEGIN
m90print('PC14  p14traceli');
p14option (next_symbol, apc, statemno);
m90name(xx, 'END p14traceline  ');
END;

(*------------------------------*)

FUNCTION
        p14ifstbind ;



VAR
      stbind  : boolean;
      pcstset : boolean;

BEGIN
m90print('PC14  p14ifstbin');
WITH apc, sqlxa.sqlkap^ [kano] DO
    BEGIN
    stbind  := true;
    pcstset := true;
    kaversion  := 0;
    IF  kaprindex <> 0
    THEN
        IF  sqlxa.sqlprp^[kaprindex].prarea <> cpr_in_sqlst
        THEN
            stbind := false
        ELSE
            pcstset := false;
        (*ENDIF*)
    (*ENDIF*)
    m90int2 (pc, 'pcst vor    ', pcst );
    m90int2 (pc, 'kaStindex   ', kaStindex);
    IF  (stbind) AND (pcstset)
    THEN
        IF  kaStindex + kaStcount - 1 = pcst
        THEN
            BEGIN
            pcst :=  kaStindex - 1;
            p19setsqlstindex (pcst);
            END;
        (*ENDIF*)
    (*ENDIF*)
    m90int2 (pc, 'pcst nach   ', pcst );
    m90int2 (pc, 'kaStindex   ', kaStindex);
    IF   kaStcount  > pcststm
    THEN
        pcststm :=  kaStcount;
    (*ENDIF*)
    p14ifstbind := stbind;
    END;
(*ENDWITH*)
END;

(*------------------------------*)

PROCEDURE
        p14_dclgen ;



VAR
      keyind : integer;
      len, i : integer;
      attrpos  : integer;
      pos      : integer;

BEGIN
m90print('PC14  p14_dclgen');
WITH apc, pcscan, sqlca, pccmdpart, pcdclgen DO
    BEGIN
    next_symbol (apc, partbufp^, part1len);
    IF   (symb = cpr_s_eof) OR (symb = cpr_s_bufeof)
    THEN
        BEGIN
        dclgen  := false;
        sqlcxap^.xasqldap.sqldaptr^.sqln := 0;
        sqlcxap^.xasqldap.sqldaptr^.sqld := 0;
        END
    ELSE
        BEGIN
        (* pcdclgen default init *)
        dclgen     := true;
        asclause  := cpc_as_var;
        indclause := cpr_is_false;
        dcluser    := bsp_knl_identifier;
        dcltab     := bsp_knl_identifier;
        dcltabl    := 0;
        (* virt.filename length = fnlen *)
        clause [1].dclfnlen := fnlen;
        FOR i := 1 TO 2 DO
            WITH clause [i] DO
                BEGIN
                s10fil2 (sizeof(dclattr), dclattr, 1, sizeof(dclattr), bsp_c1);
                dclattrl:= 0;
                END;
            (*ENDWITH*)
        (*ENDFOR*)
        p10getkeyword (apc, partbufp^, keyind);
        tabclause := 0;
        IF  keyind = cpc_i_table
        THEN
            tabclause := cpc_i_table
        ELSE
            IF  keyind =  cpc_i_dbproc
            THEN
                (* dbproc clause *)
                tabclause := cpc_i_dbproc
            ELSE
                p11wrerrorlisting (apc, cpc_invalid_sql_command);
            (*ENDIF*)
        (*ENDIF*)
        IF  tabclause  <>  0
        THEN
            BEGIN
            next_symbol (apc, partbufp^, part1len);
            p10getkeyword (apc, partbufp^, keyind);
            p19getname (apc, dcltab, len);
            dcltabl  := len;
            dclmodul := bsp_knl_identifier;
            next_symbol (apc, partbufp^, part1len);
            symb := p14symb (next_symbol, apc);
            IF  symb = cpr_s_point
            THEN
                BEGIN
                s10mv8 (mxsp_name, sizeof(dcluser), dcltab, 1,
                      dcluser, 1, sizeof(dcluser));
                dcltab     := bsp_knl_identifier;
                next_symbol (apc, partbufp^, part1len);
                p19getname (apc, dcltab, len);
                dcltabl := len;
                next_symbol (apc, partbufp^, part1len);
                END;
            (*ENDIF*)
            symb := p14symb (next_symbol, apc);
            IF  symb = cpr_s_point
            THEN
                BEGIN
                next_symbol (apc, partbufp^, part1len);
                p19getname (apc, dclmodul, len);
                next_symbol (apc, partbufp^, part1len);
                END;
            (*ENDIF*)
            p10getkeyword (apc, partbufp^, keyind);
            IF  keyind = cpc_i_as
            THEN
                BEGIN
                next_symbol (apc, partbufp^, part1len);
                p10getkeyword (apc, partbufp^, keyind);
                CASE keyind  OF
                    cpc_i_record :
                        asclause := cpc_as_record;
                    cpc_i_type :
                        asclause := cpc_as_type;
                    cpc_i_struct :
                        asclause := cpc_as_struct;
                    OTHERWISE:
                        asclause := cpc_as_var;
                    END;
                (*ENDCASE*)
                next_symbol (apc, partbufp^, part1len);
                attrpos  := sypos;
                p10getkeyword (apc, partbufp^, keyind);
                IF  (keyind <> cpc_i_ind)
                    AND (symb <> cpr_s_eof) AND (symb <> cpr_s_bufeof)
                THEN
                    BEGIN
                    REPEAT
                        next_symbol (apc, partbufp^, part1len);
                        p10getkeyword (apc, partbufp^, keyind);
                    UNTIL
                        (keyind = cpc_i_ind) OR (symb = cpr_s_eof)
                        OR (symb = cpr_s_bufeof);
                    (*ENDREPEAT*)
                    len := sypos - attrpos;
                    IF  len > mxsp_c80
                    THEN
                        len :=  mxsp_c80;
                    (*ENDIF*)
                    s10mv9 (partsizlen, sizeof(clause[1].dclattr), partbufp^, attrpos,
                          clause[1].dclattr, 1, len);
                    clause[1].dclattrl := len;
                    END;
                (*ENDIF*)
                END;
            (*ENDIF*)
            attrpos  := syposacc;
            IF  keyind = cpc_i_ind
            THEN
                BEGIN
                indclause := cpr_is_true;
                REPEAT
                    next_symbol (apc, partbufp^, part1len);
                    p10getkeyword (apc, partbufp^, keyind);
                UNTIL
                    (symb = cpr_s_eof) OR (symb = cpr_s_bufeof);
                (*ENDREPEAT*)
                pos := attrpos;
                WHILE (pos < sypos) DO
                    BEGIN
                    pos := pos + 1;
                    IF  partbufp^ [attrpos] = bsp_c1
                    THEN
                        attrpos := attrpos + 1
                    ELSE
                        pos := sypos;
                    (*ENDIF*)
                    END;
                (*ENDWHILE*)
                len := sypos - attrpos;
                IF  len > mxsp_c80
                THEN
                    len :=  mxsp_c80;
                (*ENDIF*)
                s10mv9 (partsizlen, sizeof(clause[2].dclattr), partbufp^, attrpos,
                      clause[2].dclattr, 1, len);
                clause[2].dclattrl := len;
                END;
            (*ENDIF*)
            IF  (symb = cpr_s_eof) OR (symb = cpr_s_bufeof)
            THEN
                FOR i := 1 TO 2 DO
                    WITH clause [i] DO
                        BEGIN
                        len := s30lnr (dclattr, bsp_c1, 1, dclattrl);
                        IF  len = 0
                        THEN
                            BEGIN
                            dclattrl := dcltabl;
                            IF  i = 1
                            THEN
                                s10mv10 (sizeof(dcltab), sizeof(dclattr), dcltab, 1,
                                      dclattr, 1, dclattrl)
                            ELSE
                                BEGIN
                                s10mv10 (sizeof(dcltab), sizeof(dclattr), dcltab, 1,
                                      dclattr, 2, dclattrl);
                                dclattrl := dclattrl + 1;
                                dclattr [1] := 'I';
                                END;
                            (*ENDIF*)
                            p05up1casebuf (dclattr, 1, dclattrl);
                            END;
                        (*ENDIF*)
                        END;
                    (*ENDWITH*)
                (*ENDFOR*)
            (*ENDIF*)
            END;
        (*ENDIF*)
        END;
    (*ENDIF*)
    END;
(*ENDWITH*)
m90name(xx, 'END p14_dclgen    ');
END;

(*------------------------------*)

PROCEDURE
        p14getname ;



VAR
      mlen : integer;
      mpos : integer;
      pos  : integer;

BEGIN
m90print('PC14  p14getname');
WITH apc, pccmdpart, pcscan DO
    BEGIN
    IF  sylen = 0
    THEN
        next_symbol (apc, partbufp^, part1len);
    (*ENDIF*)
    mpos := sypos;
    mlen := sylen;
    IF  (symb = cpr_s_macro)
        OR (symb = cpr_s_parameter_name)
    THEN
        BEGIN
        IF  symb = cpr_s_parameter_name
        THEN
            BEGIN
            param := cpr_is_true;
            macro := cpr_is_false;
            END
        ELSE
            BEGIN
            param := cpr_is_false;
            macro := cpr_is_true;
            END;
        (*ENDIF*)
        mlen := sylen+1;
        END
    ELSE
        BEGIN
        macro := cpr_is_false;
        param := cpr_is_false;
        IF  (symb <> cpr_s_identifier)
            AND  (symb <> cpr_s_unknown)  (*kanji*)
        THEN
            IF  (symb =  cpr_s_string_literal)
                OR (symb =  cpr_s_cstring_literal)
            THEN
                BEGIN
                m90int2 (pc, 'len         ', len  );
                m90int2 (pc, 'mpos        ', mpos   );
                m90int2 (pc, 'symb        ', symb   );
                m90int2 (pc, 'sypos       ', sypos  );
                m90int2 (pc, 'sylen       ', sylen  );
                m90buf (pc, partbufp^ [1], 1 , part1len );
                IF  partbufp^ [mpos] = '"'
                THEN
                    BEGIN
                    mpos := mpos + 1;
                    mlen := mlen - 2;
                    END;
                (*ENDIF*)
                END
            ELSE
                p11precomerror (apc, cpc_invalid_sql_command);
            (*ENDIF*)
        (*ENDIF*)
        END;
    (*ENDIF*)
    nam := bsp_knl_identifier;
    len := 0;
    pos := mpos;
    REPEAT
        IF  (partbufp^ [pos] = '"')
            AND  (partbufp^ [pos+1] = '"')
        THEN
            pos := pos + 1;
        (*ENDIF*)
        len := len + 1;
        nam [len] := partbufp^ [pos];
        pos := pos + 1;
    UNTIL
        (len >= mxsp_c64) OR (pos >= mpos + mlen);
    (*ENDREPEAT*)
    IF  partbufp^ [mpos-1] <> '"'
    THEN
        p05up2casebuf (nam, 1, len);
    (*ENDIF*)
    END;
(*ENDWITH*)
m90identifier (pc, nam);
m90name(xx, 'END p14getname    ');
END;

(*------------------------------*)

PROCEDURE
        p14oradescribe ;



VAR
      keyind : integer;

BEGIN
m90print('PC14  p14oradesc');
WITH apc, pcscan, pccmdpart, sqlxa DO
    BEGIN
    orakatyp := cpr_com_ora_descr_sel;
    p10getkeyword (apc, partbufp^, keyind);
    IF  keyind = cpc_i_select
    THEN
        BEGIN
        next_symbol (apc, partbufp^, part1len);
        p10getkeyword (apc, partbufp^, keyind);
        IF  keyind <> cpc_i_list
        THEN
            p11precomerror (apc, cpc_invalid_sql_command)
        ELSE
            BEGIN
            next_symbol (apc, partbufp^, part1len);
            p10getkeyword (apc, partbufp^, keyind);
            IF  keyind <> cpc_i_for
            THEN
                p11precomerror (apc, cpc_invalid_sql_command)
            ELSE
                next_symbol (apc, partbufp^, part1len);
            (*ENDIF*)
            END
        (*ENDIF*)
        END
    ELSE
        IF  keyind = cpc_i_bind
        THEN
            BEGIN
            orakatyp := cpr_com_describe;
            next_symbol (apc, partbufp^, part1len);
            p10getkeyword (apc, partbufp^, keyind);
            IF  keyind <> cpc_i_variables
            THEN
                p11precomerror (apc, cpc_invalid_sql_command)
            ELSE
                BEGIN
                next_symbol (apc, partbufp^, part1len);
                p10getkeyword (apc, partbufp^, keyind);
                IF  keyind <> cpc_i_for
                THEN
                    p11precomerror (apc, cpc_invalid_sql_command)
                ELSE
                    next_symbol (apc, partbufp^, part1len);
                (*ENDIF*)
                END
            (*ENDIF*)
            END;
        (*ENDIF*)
    (*ENDIF*)
    END;
(*ENDWITH*)
m90name(xx, 'END p14oradescribe');
END;

(*------------------------------*)

PROCEDURE
        p14kaentryinit ;



BEGIN
m90print('PC14  p14kaentry');
WITH apc, sqlxa DO
    BEGIN
    p19sqlkaallocate (pcka);
    WITH sqlxa, sqlkap^ [pcka ], sqlca, sqlrap^ DO
        BEGIN
        katyp  := pcan.ancomtyp;
        kastate:= cpr_state_empty;
        kapacount := 0;
        kapaindex := 0;
        kaStcount := 0;
        kaStindex := 0;
        kaprindex  := 0;
        kaparamcnt := 0;
        kaversion  := -1;
        kadiindex  := 0;
        kafaindex  := 0;
        kamacro    := cpr_is_false;
        IF  pcinpind = cpc_inputfile
        THEN
            kalineno   := pclno.lnosrc
        ELSE
            kalineno   := - pclno.lnoexp;
        (*ENDIF*)
        kamode    := rakamode;
        kaatindex := pckaatind;
        kacuindex := 0;
        xakano    := pcka;
        END;
    (*ENDWITH*)
    END;
(*ENDWITH*)
END;


(*
static char ident_mf[] = "@(#)vpc14 SLOW I386 UNIX LINUX PTOC -REL30=1 -RELVER=R74 -DIAGNOSE=1 -BETA=1";
*)
