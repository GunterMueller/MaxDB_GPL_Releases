

CONST

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
      csp_unicode_blank_desc       = '\FF\DF';
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
      csp_instance_code  = 128; (* PTS 1117216 E.Z. *)
      (**)
      bsp_c1            = ' ';
      bsp_c2            = '  ';
      bsp_c5            = '     ';
      bsp_c6            = '      ';
      bsp_c8            = '        ';
      bsp_c10           = '          ';
      bsp_c11           = '           ';
      bsp_c12           = '            ';
      bsp_c14           = '              ';
      bsp_c15           = '               ';
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
      PERM_KEY_MXSP00           =       1024;
      KEY_MXSP00                =       4020;
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
      VFILENAME_MXSP00          =        256;
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
      csp_cpmp_dbg        = 'DBG'; (* dbproc debugger, PTS 1122997 *)
      csp_comp_gpc        = 'GPC'; (* Generic Programming Client: Python ... *)
      csp_comp_jdbc       = 'JDB';
      csp_comp_kernel     = 'KNL';
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
      mxsp_c15                 =         15;
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
      mxsp_buf                 = BUF_MXSP00;
      mxsp_date                = DATE_MXSP00;
      mxsp_extdate             = EXT_DATE_MXSP00;
      mxsp_exttime             = EXT_TIME_MXSP00;
      mxsp_exttimestamp        = EXT_TIMESTAMP_MXSP00;
      mxsp_long_desc           = LONG_DESC_MXSP00;
      mxsp_name                = NAME_MXSP00;
      mxsp_number              = NUMBER_MXSP00;
      mxsp_resnum              = RESNUM_MXSP00;
      mxsp_prtline             = PRTLINE_MXSP00;
      mxsp_termid              = TERMID_MXSP00;
      mxsp_time                = TIME_MXSP00;
      mxsp_timestamp           = TIMESTAMP_MXSP00;


      (*inlineC #if defined __cplusplus *)
      (*inlineC #include "ggg91.h"      *)
      (*inlineC #endif                  *)
      MAX_AUX_BUFFER_COUNT_GG00 =    16;
      MAX_CODE_TABLES_GG00      =     8;
      MAX_COL_PER_INDEX_GG00    =    16;
      MAX_COL_PER_TAB_GG00      =  1024;
      MAX_COL_SEQUENCE_GG00     =    16; (* cnt cols with link, inv, part *)
      MAX_HASH_PRIME_GG00       =  1024; (* possible maximum 1416         *)
      MAX_INV_PER_TAB_GG00      =   255; (* PTS 1124452 E.Z. *)
      MAX_QUALBUF_LOCKS_GG00    =   240;
      (* PTS 1116801 E.Z. *)
      MAX_RECLEN_GG00           = BUF_MXSP00 - 96;       (* 96 : see below *)
      (*                       96 :    NodeHeader                         *)
      (*                               + PageCheckTrailer + BdPointerSize *)
      (* END PTS 1116801 E.Z. *)
      MAX_LONGRESULTBUF_GG00    = MAX_RECLEN_GG00 + MAX_COL_PER_TAB_GG00;
      MAX_RECPARTS_GG00         =   255;
      MAX_ROOTLIST_GG00         =     6;
      MAX_STACK_GG00            =    40;
      MAX_TAPES_GG00            =    32;
      (**)
      DOUBLE_BUF_MXGG00         = 2 * BUF_MXSP00;
      FILE_ID_MXGG00            =    40;
      FILE_POS_MXGG00           =     8; (* FilePos used bytes *)
      FN_MXGG00                 =    16;
      NIL_PAGE_NO_GG00          = MAX_INT4_SP00;
      cgg_rec_key_offset        =     8;
      PAGE_HEADER_MXGG00        =     8;
      PAGE_TRAILER_MXGG00       = PAGE_HEADER_MXGG00;
      PAGE_CHECK_TRAILER_MXGG00 = INT4_MXSP00 + PAGE_TRAILER_MXGG00;
      (* PTS 1116801 E.Z. *)
      RECBODY_MXGG00            = BUF_MXSP00 - cgg_rec_key_offset;
      RECPART_MXGG00            = BUF_MXSP00 DIV MAX_RECPARTS_GG00;
      SURROGATE_MXGG00          =     8;
      RSN_RECHEAD_MXGG00        = cgg_rec_key_offset + 1 + SURROGATE_MXGG00 + 1;
      MAX_COLUMNOFFSETS         = MAX_COL_PER_TAB_GG00 + 1;
      RECCOLUMNS_MXGG00         = RECBODY_MXGG00 + cgg_rec_key_offset - RSN_RECHEAD_MXGG00;
      (* END PTS 1116801 E.Z. *)
      SFP_INIT_PART_MXGG00      =  2248;
      STACK_ENTRY_MXGG00        =     8;
      STRING_FD_MXGG00          =    44; (* string file desc     *)
      SYSKEY_MXGG00             =   140;
      TEMP_PREFIX_MXGG00        =     8;
      (**)
      FILE_ID_HEAD_MXGG00       = FILE_ID_MXGG00 - FN_MXGG00;
      LOGPAGE_BODY_MXGG00       = PAGE_MXSP00 - PAGE_HEADER_MXGG00
            - INT4_MXSP00 (* kb0p_bottom *) - PAGE_CHECK_TRAILER_MXGG00;
      MB_PART1_HEAD_MXGG00      = FILE_ID_MXGG00 + 2*32 + 8;
      MB_PART1_RETURN_MXGG00    = FILE_ID_MXGG00 + 2*RESNUM_MXSP00 + 2 + 4 + 2 * NUMBER_MXSP00;
      ALIGNMENT_GG00            =     4;
      (**)
      MAX_SEQINFOS_GG00         =    32;              (* PTS 1110096 E.Z. *)
      (*****  begin offset constants  *****)
      (**)
      cgg_col_fd_size           =     9;
      cgg_eo_bytestr            =     0; (* bytestringhandling *)
      cgg_max_timeout           = 86400; (* seconds = 1 day *)
      cgg_nil_child             =     0;
      cgg_nil_leafnodes         =    -1; (* -> treeleafnodes *)
      cgg_nil_pid               =     0;
      cgg_nil_transindex        =     0;
      cgg_nil_varcol_cnt_off    =    -1;
      cgg_treeid_fn_offset      = FILE_ID_MXGG00 - FN_MXGG00;
      cgg_whole_bytestr         =    -1; (* bytestringhandling *)
      NIL_STR_VERSION_GG00      =  NIL_PAGE_NO_GG00;
      (**)
      ROOT_CHECK_GG00           =  -1554236731; (* 0xC53A5CA3 *)
      (**)
      cgg_literal_valuefieldlist        =    -4;
      cgg_param_valuefieldlist          =    -3;
      cgg_pars_result_valuefieldlist    =    -2;
      cgg_ex_result_valuefieldlist      =    -1;
      cgg_valuelistoffset               =     4;
      cgg_idx_literal_valuefieldlist    =   0;
      cgg_idx_param_valuefieldlist      =   1;
      cgg_idx_pars_result_valuefieldlist=   2;
      cgg_idx_ex_result_valuefieldlist  =   3;
      cgg_idx_max_valuefieldlist        =   3;
      (**)
      cgg_dummy_file_version    = '\FF\FF';
      cgg_special_file_version  = '\FF\FE';
      (**)
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
      cgg_sys3_commandcache_id  = '\00\00\00\00\00\00\00\03'; (* PTS 1120445 D.T. *)
      CONSIST_TRANS_PREFIX_GG00 = '\FF\FC\00\00\00\00\00\00';
      OPEN_TRANS_PREFIX_GG00    = '\FF\FB\00\00\00\00\00\00';
      NIL_SYSKEY_GG00           = '\FF\FE\00\00\00\00\00\00';
      NIL_UNIQUE_PRIM_ID_GG00   = '\00\00\00\00';
      MAX_UNIQUE_PRIM_ID_GG00   = '\FF\FF\FF\FF';
      (**)
      cgg_obj_key_file_id       = '\FF\FF\FF\FF';
      (**)
      (*****  tgg00_BasisError constants  *****)
      e_ok                           =     0;
      e_cachedirectory_full          =   100;
      e_append_not_allowed           =   210; (*for internal use only*)
      e_bad_datapage                 =   300;
      e_bad_file                     =   320;
      e_bad_invfile                  =   330;
      e_buffer_limit                 =   350;
      e_cancelled                    =   400;
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
      e_lob_not_found                =   725;
      e_file_read_only               =   730;
      e_file_unloaded                =   740;
      e_illegal_filename             =  1000;
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
      e_no_more_temp_space           =  1400;
      e_no_more_perm_space           =  1402;
      e_no_more_memory               =  1405;
      e_no_next_invkey               =  1410;
      e_no_next_record               =  1420;
      e_no_prev_invkey               =  1430;
      e_no_prev_record               =  1440;
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
      e_internal_recreate_failed     =  2730;
      e_too_many_mb_stackentries     =  2800;
      e_too_many_mb_data             =  2801;
      e_too_small_mb_qual_part       =  2802;
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
      e_too_many_statements_in_trigger= 2817;
      e_too_many_mb_strat            =  2818;
      e_bad_hostfile_page            =  3100;
      e_cmd_timeout_invalid          =  3150;
      e_column_trunc                 =  3200;
      e_date_time_format_invalid     =  3301;
      e_devname_invalid              =  3304;
      e_devsize_invalid              =  3308;
      e_devsize_too_small            =  3310;
      e_duplicate_invkey             =  3320;
      e_not_implemented              =  3400;
      e_hex0_column_tail             =  3600;
      e_hostfile_error               =  3700;
      e_incomplete_logsegm           =  3705;
      e_invalid_blocksize            =  3707;
      e_incompatible_log             =  3800;
      e_inconsistent_nodetype        =  3816;
      e_invalid                      =  3820;
      e_key_update                   =  4000;
      e_logpage_not_found            =  4050;
      e_conf_parameter_error         =  4107;
      e_hotstandby_cmd_send_failed         =  4150; (* added UH 2003-09-05 *)
      e_hotstandby_wrong_role              =  4151; (* added UH 2003-09-05 *)
      e_hotstandby_establish_mirror_failed =  4152; (* added UH 2003-09-05 *)
      e_hotstandby_add_node_failed         =  4153; (* added UH 2003-09-05 *)
      e_hotstandby_del_node_failed         =  4154; (* added UH 2003-09-05 *)
      e_hotstandby_free_memory_failed      =  4155; (* added UH 2003-09-05 *)
      e_net_line_down                =  4210;
      e_new_hostfile_required        =  4300;
      e_end_of_backup                =  4301; (*for internal use only*)
      e_no_data_on_tape              =  4304;
      e_no_servers                   =  4305;
      e_save_skipped                 =  4306;
      e_null_value_illegal           =  4310;
      e_num_invalid                  =  4320;
      e_num_overflow                 =  4330;
      e_program_stack_overflow       =  4500;
      e_qual_violation               =  4600;
      e_range_violation              =  4700;
      e_rec_not_locked               =  4710;
      e_release_caused_by_shutdown   =  4721;
      e_response_timeout             =  4728;
      e_rollback_wanted              =  4740;  (*for internal use only*)
      e_skip_upd                     =  4800;
      e_skip_key_upd                 =  4801;
      e_special_null                 =  4810;
      e_stack_op_illegal             =  4820;
      e_stack_overflow               =  4830;
      e_stack_type_illegal           =  4840;
      e_string_column_empty          =  4850;
      e_work_rolled_back             =  4900;
      e_rollb_log_full               =  4906;
      e_too_many_devspaces           =  4920;
      e_too_many_lockunits_specified =  4930;
      e_too_many_lock_requests       =  4940;
      e_no_server_task_available     =  4946;
      e_lock_collision               =  5000;
      e_lock_collision_with_inserted_row =  5001;
      e_lock_dirty                   =  5005;
      e_upd_view_violation           =  5010;
      e_view_violation               =  5100;
      (*e_wait_for_savepoint           =  5200;*)
      e_wait_for_lock_release        =  5210;
      e_wrong_hostfile               =  5220;
      e_wrong_configuration          =  5224; (* PTS 1121221 UH 2003-03-20 *)
      e_no_log_to_save               =  5225;
      e_backup_is_running            =  5226; (* PTS 1108479 UH 2000-11-27 *)
      e_backup_not_running           =  5227; (* PTS 1108479 UH 2000-11-27 *)
      e_no_standby_configuration     =  5228;
      e_backup_history_lost          =  5229;
      e_log_autooverwrite_on         =  5230; (* PTS 1128223 MB 2004-03-04 *)
      e_log_writing_disabled         =  5231; (* PTS 1128223 MB 2004-03-04 *)
      e_wrong_password               =  5240;
      e_dcom_hresult                 =  5300;
      e_invalid_label                =  5550;
      e_invalid_configuration        =  5600;
      e_write_task_crash             =  5750;
      e_queue_mismatch               =  5900;
      e_bad_pagetype                 =  5950;
      e_invalid_columnname           =  6000;
      e_invalid_end_of_command       =  6005;
      e_invalid_datatype             =  6010;
      e_invalid_filetype             =  6012;
      e_invalid_tabletype            =  6014;
      e_identifier_too_long          =  6015;
      e_invalid_indexorder           =  6016; (*only internal error*)
      e_invalid_keyword              =  6020;
      e_expr_in_insert_value_list    =  6025; (*only internal error*)
      e_invalid_mix_functions_columns=  6035;
      e_invalid_tablename            =  6040;
      e_invalid_username             =  6045;
      e_invalid_messagetype          =  6050;
      e_in_view_not_allowed          =  6055;
      e_replicated_table_not_allowed =  6057;
      e_parameter_not_allowed        =  6060;
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
      e_query_not_allowed            =  6113;
      e_too_many_monadic_operators   =  6115;
      e_rowno_not_allowed            =  6120;
      e_invalid_unsign_integer       =  6125;
      e_update_not_allowed           =  6130;
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
      e_connection_disabled          =  6223;
      e_const_incompatible_with_typ  =  6224;
      e_corelated_subquery_not_allowe=  6226;
      e_costlimit_overflow           =  6228;
      e_costwarning_overflow         =  6230;
      e_current_of_needs_fetch       =  6232;
      e_default_not_allowed          =  6234;
      e_default_not_in_range         =  6236;
      e_default_spec_not_allowed     =  6238;
      e_duplicate_replication_user   =  6240;
      (* PTS 1111576 E.Z. *)
      e_duplicate_columnname         =  6242;
      e_duplicate_constraint         =  6244;
      e_duplicate_default            =  6246;
      e_duplicate_indexname          =  6248;
      e_duplicate_name               =  6250;
      e_duplicate_privilege          =  6254;
      e_duplicate_referencename      =  6256;
      e_duplicate_resultname         =  6258;
      e_duplicate_tablename          =  6262;
      e_duplicate_trigger_def        =  6264;
      e_duplicate_value              =  6266;
      e_scrolling_fetch_not_allowed  =  6267;
      e_foreign_key_must_exist       =  6268;
      e_foreign_key_violation        =  6270;
      e_from_select_not_allowed      =  6274;
      e_function_not_allowed         =  6276;
      e_group_user_not_allowed       =  6278;
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
      e_parameter_change_not_allowed =  6386;
      e_procedure_must_be_recompiled =  6392;
      e_referenced_index_not_allowed =  6400;
      e_remote_load_not_allowed      =  6402;
      e_restore_catalog_error        =  6404;
      e_bd_page_collision            =  6405; (*for internal use only*)
      e_row_not_found                =  6408;
      e_schema_not_allowed           =  6409;
      e_select_col_list_too_long     =  6410;
      (* PTS 1120287 E.Z. *)
      (* PTS 1111576 E.Z. *)
      e_st_col_not_open              =  6420;
      e_st_col_open_read_only        =  6422;
      e_st_invalid_destpos           =  6424;
      e_st_invalid_length            =  6426;
      e_st_invalid_pos               =  6428;
      e_st_invalid_startpos          =  6430;
      e_st_invalid_longhandling      =  6431;
      e_ak_system_error              =  6432;
      e_system_error                 =  6433;
      e_internal_error               =  6434;
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
      e_too_many_sec_links           =  6470;
      e_too_many_sourcetables        =  6474;
      e_too_many_subqueries          =  6476;
      e_too_many_subtrans            =  6478;
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
      e_unknown_sql_schema           =  6523;
      e_unknown_tablename            =  6524;
      e_unknown_multibyte_set        =  6527;
      e_unknown_trigger              =  6528;
      e_unknown_user                 =  6530;
      e_unknown_user_password_pair   =  6532;
      e_use_cascade                  =  6533;
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
      e_wrong_application            =  6560;
      e_dbm_command_not_possible     =  6561; (* UH 2003-02-12 *)
      e_illegal_escape_sequence      =  6562;
      e_invalid_escape_char          =  6564;
      e_unitrans_src_too_short       =  6568;
      e_unitrans_dest_too_short      =  6570;
      e_not_translatable             =  6572;
      e_not_trans_to_uni             =  6574;
      e_not_trans_from_uni           =  6576;
      e_autosave_running             =  6578;
      e_no_matching_authentication   =  6580;
      e_no_basic_authentication      =  6581;
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
      e_new_disk_not_accessible      =  8000;
      e_standby_info_not_ok          =  8004;  (* PTS 1111229 E.Z. *)
      e_root_check                   =  8010;
      e_migration_error              =  8048; (* PTS 1117498 M.Ki. *)
      e_not_prepared_for_migration   =  8049; (* PTS 1117050 TS 2002-08-09 *)
      e_no_converter_entry           =  8050;
      e_no_snapshot_available        =  8051; (* PTS 1121318 TS 2003-07-09 *)
      e_prev_data_recovery_was_incomplete = 8052; (* PTS 1124994 UH 2004-01-26 *)
      e_bad_fdir                     =  9001;
      e_bad_logpage                  =  9010;
      e_bad_conv_page                =  9030;
      e_bad_conv_bitmap_page         =  9031;
      e_bad_bitmap_page              =  9045;
      e_disk_not_accessible          =  9050;
      e_illegal_branchlength         =  9060;
      e_illegal_entrylength          =  9070;
      e_illegal_entrypos             =  9080;
      e_illegal_keylength            =  9090;
      e_illegal_page_no              =  9100;
      e_move_error                   = -9111;
      e_invalid_entrypos             =  9120;
      e_invalid_index_structure      =  9130;
      e_invalid_leaves_structure     =  9140;
      e_no_more_space                =  9150;  (*for internal use only*)
      e_page_in_wrong_tree           =  9160;
      e_data_page_corrupted          =  9161;
      e_corrupted_datapage_from_io   =  9163;   (*for internal use only*)
      e_invalid_fbm_mark             =  9180;
      e_log_error                    =  9200;
      e_undo_log_write_error         =  9201;
      e_undo_log_execute_error       =  9202;
      e_redo_log_execute_error       =  9203;
      e_history_write_error          =  9204;
      e_history_read_error           =  9205;
      e_history_remove_error         =  9206;
      e_redo_log_read_error          =  9207; (* UH 2004-09-27 *)
      e_redo_log_write_error         =  9208; (* UH 2004-09-27 *)
      e_log_full                     =  9210;
      e_start_server_task_error      =  9221;
      e_no_more_jobs_to_wait_for     =  9222;
      e_software_too_old             =  9230; (* UH 2004-09-27 *)
      e_nil_transindex               =  9232;
      e_long_column_missing          =  9240; (* PTS 1103382 TS 1999-07-16 *)
      e_bd_leaf_unlocked             =  9400; (*for internal use only*)
      e_too_long_objects             =  9500; (*for internal and APO use only*)
      (* basis error from -27000 to -28999 are located in ggg01 *)
      (**)


      cut_start_of_field     = '\01';  (* virt. terminal only (VTA09) *)
      cut_end_of_field       = '\00';  (* virt. terminal only (VTA09) *)
      cut_protected          = '\00';
      cut_unprotected        = '\80';
      cut_bright_protected   = '\01';
      cut_bright_unprotected = '\81';
      cut_invisible          = '\82';
      cut_mask_items         =    20;
      cut_max_cmd_lines_term =    20;
      cut_max_cmds_term      =    20;
      cut_max_files_term     =    25;
      cut_max_lines_term     =  1500;
      cut_max_param_term     =    10;
      cut_max_prime_cnt      =   506;
      cut_nil_block_no       =    -1;
      cut_nil_entry_pno      =    -1;
      cut_sql_req_failed     = -8888;
      (* *)
      cut_pfkey_none         = bsp_c8;
      cut_pfkey_help         = 'help    ';
      cut_pfkey_hexint       = 'hex/int ';


      ERRTEXTREC_MXBD00          = 56;
      FDIRENTRY_MXBD00           = 44;
      NODE_HEADER_MXBD00         = 80;
      STD_FDIRENTRY_MXBD00       = 36;
      (* *)
      REC_ALIGN_BD00             = 2;
      (* *)
      (* --- bufferhandling --- *)
      (* *)
      MIN_RECORD_LEN_BD00        = cgg_rec_key_offset + 1; (* define byte *)
      (* *)
      (* --- nodehandling --- *)
      (* *)
      BODY_BEG_BD00              = NODE_HEADER_MXBD00 + 1;
      BODY_END_BD00              = PAGE_MXSP00 - PAGE_CHECK_TRAILER_MXGG00;
      MAX_BOTTOM_BD00            = BODY_END_BD00 + 1;
      FIRST_REC_INDEX_BD00       = 0;
      (* *)
      LEAF_LEVEL_BD00            = 0;
      FIRST_INDEX_LEVEL_BD00     = 1;
      SECOND_INDEX_LEVEL_BD00    = 2;
      THIRD_INDEX_LEVEL_BD00     = 3;
      MAX_LEVEL_BD00             = 5;
      MAX_BSTR_LEVEL_BD00        = 2;
      (* *)
      FULLCOVERING_BD00          = PAGE_MXSP00 - NODE_HEADER_MXBD00 - PAGE_CHECK_TRAILER_MXGG00;
      MAX_BSTR_COVERING_BD00     = FULLCOVERING_BD00;
      HALF_COVERING_BD00         = FULLCOVERING_BD00 DIV 2;
      QUATER_COVERING_BD00       = FULLCOVERING_BD00 DIV 4;
      NINETY_PERCENT_COV_BD00    = (FULLCOVERING_BD00 * 90) DIV 100;
      (* *)
      MAX_INVLISTLENGTH_BD00     = (FULLCOVERING_BD00 DIV 4) * 3;
      POINTERSIZE_BD00           = INT4_MXSP00;
      DIVISOR_POINTERSIZE_BD00   = POINTERSIZE_BD00 DIV 2; (* Shift Value For Pointersize Division *)
      MAX_POINTERINDEX_BD00      = FULLCOVERING_BD00 DIV POINTERSIZE_BD00;
      PNOS_PER_PAGE_BD00         = FULLCOVERING_BD00 DIV INT4_MXSP00;
      (* *)
      (* --- miscellaneous --- *)
      (* *)
      LOCK_TREE_EXCL_BD00        = true;
      LOCK_LEAF_EXCL_BD00        = true;
      SYNC_TREE_LOCK_BD00        = true;
      (* *)
      FILE_EXT_COR_BD00          = '.cor';
      (* *)
      MAX_NODE_SCAN_BD00         = 5;
      LWB_OF_LIST_BD00           = 1;
      UPB_OF_LIST_BD00           = 6;
      MAX_OBJ_GARB_COLL_BD00     = 64;
      MAX_VF_BUFFERS_BD00        = 128;(* see cserv.pcf *)
      MAX_FREE_QUEUE_BD00        = 5;
      cbd_cspmaxint4_mxspbuf     = MAX_INT4_SP00 - PAGE_MXSP00;
      (* cbd_cspmaxint4_mxspbuf  = 2147479551; *)
      (* *)
      NIL_RECINDEX_BD00             = -MAX_INT2_SP00;  (* must be < -1 *)
      (* *)
      MIN_LOCK_REQUEST_SEQ_NO_BD00  = 0;
      POS_OFF_DIFF_BD00             = 1; (* difference between position and offset, *)
      (*                                 i.e. pascal and c arrays                *)


      c_edit_lines        =  8;
      c_header_line_count =  1;
      c_int4_digits       = 10;
      c_maxbuf_block      = 10;
      c_maxbuf_lines      =  5;
      c_maxint4_string    = '2147483647';
      c_minbuf_block      = 10;
      c_minbuf_lines      =  2;
      mx_int4_displ       = 11;
      mx_maxbuf_byte      =  3;
      mx_maxbuf_prefix    =  4;
      mx_minbuf_byte      =  2;
      mx_minbuf_prefix    =  5;
      mx_pos_displ        =  5;
      mx_yes_no           =  2;
      of_col_type         =  6;
      of_first_col        = 25;
      of_key_pos          = 10;
      of_second_col       = 57;
      (* *)
      c_chr_prefix        = 'chr:';
      c_c2_prefix         = 'c2 :';
      c_c4_prefix         = 'c4 :';
      c_dec_prefix        = 'dec:';
      c_hex_prefix        = 'hex:';
      c_int2_prefix       = 'int2';
      c_int4_prefix       = 'int4';
      c_pos_prefix        = 'pos:';
      c_u2_prefix         = 'u2 :';
      (* *)
      c_pfkey_help        = 'help    ';
      c_pfkey_none        = bsp_c8;
      (* *)
      c_bool_indicator    = 'B';
      c_int_indicator     = 'I';
      c_minbuf_indicator  = '@';
      c_mtype_indicator   = 'M';
      c_mtype2_indicator  = 'N';
      c_set_indicator     = 'S';
      (* *)
      msg_illegal_len     = 'ILLEGAL LENGTH      ';
      msg_illegal_pos     = 'ILLEGAL POSITION    ';
      (* *)
      c_edit        = true;
      c_header_only = true;
      c_immed_displ = true;
      c_on          = true;
      c_upper       = true;
      c_warning     = true;
      c_with_zero   = true;

TYPE

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
      (**)
      tsp00_Dbcs = (
            no_dbcs,
            dbcs_with_shifts,
            dbcs_no_shifts);
      (**)
      tsp00_DbObjectType = (
            dbo_method,
            dbo_dbproc,
            dbo_dbfunc,
            dbo_package,
            dbo_system_trigger);
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
      (**)
      tsp00_DgcallbackResult = (
            dg_ok,
            dg_cancel,
            dg_again);
      (**)
      tsp00_DlgBoxStyle = (
            dg_modal,
            dg_modeless);
      (**)
      tsp00_ExecMode = (
            async,
            sync_new_session,
            sync_same_session);
      (**)
      tsp00_ExecReturn = (
            ex_ok,
            ex_notok,
            ex_no_exec,
            ex_no_async,
            ex_no_sync,
            ex_no_sync_new,
            ex_no_sync_same);
      (**)
      tsp00_LcompResult = (
            l_less,
            l_equal,
            l_greater,
            l_undef);
      (**)
      tsp00_NetError = (
            net_ok,
            net_notok,
            net_timeout,
            net_send_line_down,
            net_receive_line_down);
      (**)
      tsp00_NumError = (
            num_ok,
            num_trunc,
            num_overflow,
            num_invalid,
            num_incompatible,
            num_missing_addr);
      (**)
      tsp00_Os = (
            os_unix,
            os_vms,
            os_windows,
            os_win32,
            os_os2);
      (**)
      tsp00_Service = (
            sql_user,
            sql_async_user,
            sql_utility,
            sqlDistribution_esp00,
            sqlControl_esp00,
            sqlEvent_esp00,
            sqlStreamUser_esp00);
      (* *)
      tsp00_ShutdownMode = (
            shtShutdKill_esp00,
            shtShutdNormal_esp00);
      (**)
      tsp00_SkLabel = (
            no_sk_labels,
            short_sk_labels,
            long_sk_labels);
      (**)
      tsp00_Sproc = (
            sppr_db_proc,
            sppr_ddl_trigger,
            sppr_insert_trigger,
            sppr_update_trigger,
            sppr_delete_trigger,
            sppr_db_func);
      (**)
      tsp00_SqlMode = (
            sqlm_ansi,
            sqlm_db2,
            sqlm_oracle,
            sqlm_internal);
      (**)
      tsp00_SwapKind = (
            sw_dummy,
            sw_normal,
            sw_full_swapped,
            sw_part_swapped);
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
      (**)
      tsp00_UniError = (
            spu_ok,
            spu_not_translatable,
            spu_dest_too_short);
      (**)
      tsp00_ValMode = (
            vm_datapart,
            vm_alldata,
            vm_lastdata,
            vm_nodata,
            vm_no_more_data,
            vm_last_putval,
            vm_data_trunc,
            vm_close,
            vm_error,             (* PTS 1113250 E.Z. *)
            vm_startpos_invalid); (* PTS 1122546 D.T. *)
      (**)
      tsp00_VaReturn = (
            va_ok,
            va_notok,
            va_eof,
            va_wait_first);
      (**)
      tsp00_VFileOpCodes = (
            vread,
            vwrite,
            vappend,
            voverwrite,
            vread_ora);
      (**)
      tsp00_VfFormat = (
            vf_plaintext,
            vf_record,
            vf_stream,
            vf_unknown,
            vf_4k_block);
      (**)
      tsp00_VfResource = (
            vf_stack,
            vf_bufferpool);
      (**)
      tsp00_VfReturn = (
            vf_ok,
            vf_notok,
            vf_eof,
            vf_noseek);
      (**)
      tsp00_VfType = (
            vf_t_unknown,
            vf_t_file,
            vf_t_pipe,
            vf_t_raw,
            vf_t_tape_norew,
            vf_t_tape_rew,
            vf_t_dafs_file);
      (**)
      tsp00_VpLinefeeds = (
            vp_nolf,
            vp_onelf,
            vp_twolf,
            vp_threelf,
            vp_page);
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
      (**)
      tsp00_VtMark = (
            vt_mark_off,
            vt_mark_block,
            vt_mark_line,
            vt_mark_contiguous,
            vt_mark_matrix);
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
      (**)
      tsp00_VtScroll = (
            vt_horizontal_scroll,
            vt_vertical_scroll);
      (**)
      tsp00_VtUsageMode = (
            vt_form,
            vt_edit);
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
            warn9_stmt_optimized,
            warn10_views_substituted,
            warn11_time_value_too_long,
            warn12_rowno_used,
            warn13_optional_column_indexed,
            warn14_next_invlist,
            warn15_user_defined_code);
      (**)
      tsp00_Wlisttype = (
            wls_stream,
            wls_line,
            wls_matrix);
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
      (**)
      tsp00_XpReturnCode = (
            xp_ok,
            xp_key_not_found,
            xp_eof,
            xp_not_ok);
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
      tsp00_VFilename      = tsp00_C256;
      tsp00_VtAttrib       = PACKED SET OF tsp00_VtMode;
      tsp00_VtColors       = PACKED SET OF tsp00_VtColor;
      tsp00_VtLabel        = tsp00_C8;
      tsp00_VtKeys         = SET OF tsp00_VtKey;
      tsp00_VtMsg          = tsp00_C80;
      tsp00_WarningSet     = PACKED SET OF tsp00_Warnings;
      tsp00_XpKeyType      = tsp00_C32;
      tsp00_Zoned          = tsp00_C20;
      (**)
      tsp_trace            = tsp00_Trace; (* cccprint compatibility *)
      (**)
      tsp00_BoolAddr         = ^boolean;
      tsp00_BufAddr          = ^tsp00_Buf;
      tsp00_KeyAddr          = ^tsp00_Key;
      tsp00_KnlIdentifierPtr = ^tsp00_KnlIdentifier;
      tsp00_MoveObjPtr       = ^tsp00_MoveObj;
      tsp00_ObjAddr          = ^tsp00_AnyPackedChar;
      tsp00_PageAddr         = ^tsp00_Page;
      tsp00_ScreenBufAddr    = ^tsp00_ScreenBuf;
      (**)
      tsp00_KeyPtr         = ^tsp00_Key (*ptoc00Redefine=tsp00_BytePtr*);
      (**)
      tsp_c24  = tsp00_C24;
      tsp_c40  = tsp00_C40;
      tsp_c256 = tsp00_C256;
      (**)
      tsp_key                = tsp00_Key;
      tsp_process_id         = tsp00_TaskId;

      tsp00_TakCacheBuf = RECORD (* used by movcc declared in VSP10 *)
            CASE boolean OF
                true :
                    (word_allignment : tsp00_Int4);
                false :
                    (buf : ARRAY [  1..csp_cntcachebuf  ] OF tsp00_Buf);
                END;
            (*ENDCASE*)

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
                    (map_int : tsp00_Int2);
                FALSE :
                    (map_c2  : tsp00_C2)
                END;
            (*ENDCASE*)


      tsp00_DataLine = RECORD
            text   : tsp00_Line;
            pos    : tsp00_LinePositions;
            length : tsp00_LinePositions
      END;

      tsp00_String = RECORD
            text   : tsp00_Line;
            length : tsp00_LinePositions
      END;

      tsp00_Ccc = RECORD
            count : tsp00_Int4
      END;

      tsp_ccc = tsp00_Ccc;

      tsp00_Numberstack = RECORD (* vsp50 *)
            stackpointer : integer;
            elem         : ARRAY [ 1..csp_numstack ] OF tsp00_Number;
            elem_err     : ARRAY [ 1..csp_numstack ] OF tsp00_NumError
      END;

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

      tsp00_ModHeader = RECORD
            spmh_modn  : tsp00_KnlIdentifier;
            spmh_appln : tsp00_KnlIdentifier;
            spmh_user  : tsp00_KnlIdentifier;
            spmh_mtype : tsp00_Uint1;
            spmh_state : char; (* 'R', 'T', 'D', 'P' *)
            spmh_index : tsp00_Int2;
      END;

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

      tsp00_PlCodeInfo = RECORD
            spplc_objtype  : tsp00_Name;
            spplc_owner    : tsp00_KnlIdentifier;
            spplc_progn    : tsp00_KnlIdentifier;
            spplc_modn     : tsp00_KnlIdentifier;
            spplc_proglang : tsp00_Name;
            spplc_subtype  : tsp00_Sname;
            spplc_version  : tsp00_C4;
      END;

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
      (* ******* virtual terminal ******* *)
      tsp00_VtLonglabel     = ARRAY [1..csp_sk_lines ] OF tsp00_VtLabel;
      tsp00_VtLlabels       = ARRAY [1..csp_sk_number] OF tsp00_VtLonglabel;
      tsp00_VtLocalFunction = vt_cursor_up..vt_del_eof;
      tsp00_VtSlabels       = ARRAY [1..csp_sk_number] OF tsp00_VtLabel;
      (**)
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

      tsp00_VtField = RECORD
            text  : tsp00_VtLabel;
            lno   : tsp00_Int2;
            colnr : tsp00_Int2;
            att   : tsp00_C1;
            fill1 : boolean;
            fill2 : tsp00_Int2;
      END;

      tsp00_HifParms = RECORD
            slabels      : tsp00_VtSlabels;
            llabels      : tsp00_VtLlabels;
            sys_text     : tsp00_Line;
            sys_attr     : tsp00_Line;
            insert_label : tsp00_VtField
      END;

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

      tsp00_VtRectangle = RECORD
            top    : tsp00_Int2; (* row ts_number *)
            left   : tsp00_Int2; (* column ts_number *)
            bottom : tsp00_Int2; (* row ts_number *)
            right  : tsp00_Int2; (* column ts_number *)
      END;

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

      tsp00_VtSplitopt = RECORD
            maximized : boolean;
            icon      : boolean;
            pos       : tsp00_VtRectangle;
      END;

      tsp00_VtSplitoptaddr = ^tsp00_VtSplitopt;
      (**)
      (* ******* virtual file ******* *)
      tsp00_VfBuf          = ARRAY [ 1..csp_pool_buffers ] OF tsp00_Page;
      tsp00_VfBufaddr      = ^tsp00_VfBuf;
      (**)
      tsp_vf_bufaddr       = ^tsp00_VfBuf;
      (* ******* disk io ******* *)
      tsp00_Maxiobufaddr = ^tsp00_AnyPackedChar;
      (**)
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

      tsp00_OnlineHeader = RECORD
            id_field    : tsp00_C8;
            relno_field : tsp00_C8;
            mode_field  : tsp00_C12;
            text_field  : tsp00_C40;
      END;

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

      tsp00_ReportFunctionResult = RECORD
            rf_used_count    : tsp00_Int4;
            rf_column_id     : ARRAY [ 1..16 ] OF tsp00_Int4;
            rf_column_type   : ARRAY [ 1..16 ] OF char;
            rf_column_result : ARRAY [ 1..16 ] OF tsp00_ReportValues;
      END;

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

      tsp00_QrPrePacket =  RECORD
            qrp_buf     : tsp00_Buf;
            qrp_parms   : tsp00_ReportInterface;
            qrp_retcode : tsp00_Int2;
            qrp_mtype   : tsp00_Int2;
      END;

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
      (**)
      tsp00_LdState = SET OF (
            ld_stream,
            ld_use_conversion,
            ld_use_toascii,
            ld_use_ucs_2_swap,
            ld_short_scol,
            ld_first_insert,
            ld_copy,
            ld_first_call    (* PTS 1122546 D.T. *)
            );

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

      (**)
      tsp00_LdbChange = SET OF (
            ldb_use_termchar,
            ldb_use_conversion,
            ldb_use_toascii,
            ldb_use_ucs_2_swap);

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

      tsp00_ParamSpecList = ARRAY [ 1..csp_maxvariables ] OF
            tsp00_ParamSpec;

      tsp_dcom_init_sink_parms = PACKED RECORD
            acv_addr  : tsp00_Addr;
            sink_addr : tsp00_Addr;
            sink_size : tsp00_Int4;
      END;


      tsp_dcom_co_create_instance_parms = PACKED RECORD
            library_name    : tsp00_Addr;    (* PTS 1121178 FF 19-Mar-2003 *)
            package_id      : tsp00_C8;      (* PTS 1139907 FF 26-Jan-2006 *)
            MD5_footprint   : tsp00_C32;     (* PTS 1121178 FF 19-Mar-2003 *)
            coclsid         : tsp00_C16;
            iid             : tsp00_C16;
            ifactory        : tsp00_Addr;
            idispatch       : tsp00_Addr;
            ikernel_sink    : tsp00_Addr;
            session_context : tsp00_Addr;
            session         : tsp00_C4;
            trans           : tsp00_C4;
            ret             : tsp00_Int4;
            (*in_proc_server  : boolean;        PTS 1112663 *)
            (*debug_mode      : boolean;    PTS 1112663 *)
            (*debug_location  : tsp00_Addr; PTS 1112663 *)
            errtext         : tsp00_C64;
            use_libname     : boolean;     (* PTS 1121178 FF 19-Mar-2003 *)
            force_run_in_kernel : boolean;
            run_in_ude_server   : boolean;
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
            run_in_ude_server : boolean;
            coclsid         : tsp00_C16;
            iid             : tsp00_C16;
            library_name    : tsp00_Addr;
            MD5_footprint   : tsp00_C32;
      END;


      tsp_dcom_release_instance_parms = PACKED RECORD
            session_context : tsp00_Addr; (* +++ FF PTS 1104668  +++ *)
            session         : tsp00_C4;
            trans           : tsp00_C4;
            hresult         : tsp00_Int4;
      END;


      tsp_dcom_paraminfo_parms = PACKED RECORD
            library_name    : tsp00_Addr;    (* PTS 1121178 FF 19-Mar-2003 *)
            prog_id         : tsp00_C64;
            method          : tsp00_KnlIdentifier;
            package_id      : tsp00_C8;      (* PTS 1139907 FF 26-Jan-2006 *)
            MD5_footprint   : tsp00_C32;     (* PTS 1121178 FF 19-Mar-2003 *)
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
            use_libname     : boolean;     (* PTS 1121178 FF 19-Mar-2003 *)
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


      tbd00_DelInvReason = (
            dirSetDeletedFlag_ebd00,
            dirRemoveDeletedFlag_ebd00,
            dirGarbageCollection_ebd00,
            dirRollback_ebd00,
            dirDeletePrimKey_ebd00);
      tbd00_PageLockMode = (
            plmLock_ebd00,
            plmLockWithSVPSync_ebd00,
            plmLockWithSVPSyncNoWait_ebd00,
            plmLockWithoutReschedule_ebd00); (* PTS 1115176 TS 2002-04-09 *)
      (* *)
      tbd_node_request = (
            nr_for_read,
            nr_for_update);
      (* *)
      tbd_lru_info = (
            lru_normal,
            lru_mid,
            lru_last);
      (* *)
      tbd_file_state_set = SET OF (
            f_unused1_ebd00, (* unloaded *)
            f_bad,
            f_read_only,
            f_not_accessible,
            f_unused2_ebd00,  (* write_off *)
            f_unused3_ebd00); (* no_tree_index *)
      (* *)
      tbd_objpage_state_set = SET OF (
            objp_no_chain_to_freelist); (* "new object" optimization *)
      (* *)
      tbd_fileinfo_desc_set = SET OF (
            fid_fill0, (* fid_user_ref *)
            fid_fill1, (* fid_new_long_col *)
            fid_fill2,
            fid_fill3,
            fid_fill4,
            fid_fill5,
            fid_fill6,
            fid_fill7);
      (* *)

      tbd_fileinfo = RECORD
            fi_type            : tgg00_FiletypeSet;
            fi_state           : tbd_file_state_set;
            fi_fill1           : boolean;
            fi_descr           : tbd_fileinfo_desc_set;
            fi_root            : tsp00_PageNo;
            fi_vers            : tgg91_FileVersion;
            fi_col_cnt         : tsp00_Int2;
            fi_user_ref        : tgg00_UserRef;
            fi_var_col_cnt_pos : tsp00_Int2
      END; (*filedirectory*)


      tbd00_FDirEntry = RECORD
            CASE integer OF
                1 :
                    (fdirBuf_bd00 : PACKED ARRAY [1..FDIRENTRY_MXBD00] OF char);
                2 :
                    (fdirEntryLength_bd00   : tsp00_Int2;
                    fdirFileNameLength_bd00 : tsp00_Int2;
                    fdirFill1_bd00          : tsp00_Int2;
                    fdirFill2_bd00          : tsp00_Int2;
                    fdirFileName_bd00       : tgg00_Filename;
                    fdirFileInfo_bd00       : tbd_fileinfo);
                END;
            (*ENDCASE*)


      tbd00_FreeQueue = RECORD
            fqPnos_bd00        : ARRAY [1..MAX_FREE_QUEUE_BD00] OF tsp00_PageNo;
            fqPageVersion_bd00 : ARRAY [1..MAX_FREE_QUEUE_BD00] OF tgg00_ConverterVersion;
            fqTop_bd00         : tsp00_Int4
      END;

      tbd00_LockRequestSeqNo = tsp00_Int4;
      (* *)
      tbd_position_list = ARRAY [ 1..MAX_BSTR_LEVEL_BD00 ] OF tgg00_PagePos;
      (* *)
      tbd_node_header   = PACKED ARRAY [1..NODE_HEADER_MXBD00] OF char;
      (* *)
      tbd_node_body     = PACKED ARRAY [BODY_BEG_BD00..BODY_END_BD00] OF char;
      (* *)
      tbd_pointer_list  = ARRAY [1..MAX_POINTERINDEX_BD00] OF tgg00_PagePos;
      (* *)
      tbd00_ConsistencyInfo = SET OF (
            c_historyInPagingFile_ebd00,
            c_historyOnPage_ebd00 );

      tbd00_PageFixSizeSpace = RECORD
            filler1               : tsp00_C50;
            fixRecordLength       : tsp00_Int2;
            filler2               : tsp00_Int4;
            filler3               : tsp00_C24;
      END;


      tbd00_RedoPage = RECORD
            filler1               : tsp00_C42;
            filler2               : tsp00_Int2;
            firstEntrySequence    : tsp00_Int4;
            (* *)
            filler3               : tsp00_Int4;
            lastEntrySequence     : tsp00_Int4;
            (* *)
            filler4               : tsp00_C8;
            (* *)
            transno               : tgg91_TransNo;
            filler5               : tsp00_C10;
      END;


      tbd00_UndoPage = RECORD
            filler1               : tsp00_C50;
            filler2               : tsp00_Int2;
            lastEntrySequence     : tsp00_Int4;
            (* *)
            pagecount             : tsp00_Int4;
            relevantForGC         : boolean;
            filler3               : boolean;
            filler4               : tsp00_Int2;
            (* *)
            transno               : tgg91_TransNo;
            lastGC_offset         : tsp00_Int2;
            (* *)
            lastGC_pageno         : tsp00_PageNo;
            filler5               : tsp00_Int4;
      END;


      tbd00_OpenTransPage = RECORD
            filler1                  : tsp00_C18;
            filler2                  : boolean;
            isCreatedDuringSavepoint : boolean;
            filler5                  : tsp00_C32;
            oldestEOTSequence        : tsp00_Int4;
            filler7                  : tsp00_C16;
            savepointSequence        : tsp00_Int4;
            filler8                  : tsp00_Int4;
      END;


      tbd_node = RECORD
            CASE integer OF
                1: (* Kernel_Page72::PersistentHeader *)
                    (nd_header                 : tgg00_PageHeader);
                2: (* Data_Page *)
                    (nd_full_header            : tbd_node_header;
                    nd_body                    : tbd_node_body;
                    nd_full_trailer            : tgg00_PageCheckTrailer);
                3:
                    (nd_id                     : tsp00_PageNo;
                    nd_pt                      : tgg00_PageType;
                    nd_pt2                     : tgg00_PageType2;
                    nd_checktype               : tgg00_PageCheckType;
                    nd_pmode                   : tgg00_PageMode;
                    (* *)
                    nd_bottom                  : tgg00_PagePos; (* Data_Page *)
                    nd_record_cnt              : tsp00_Int4;
                    (* *)
                    nd_level                   : tsp00_Int2;
                    nd_file_state              : tbd_file_state_set;
                    nd_sorted                  : boolean;
                    nd_root                    : tsp00_PageNo;
                    (* *)
                    nd_right                   : tsp00_PageNo; (* Data_ChainPage      *)
                    nd_left                    : tsp00_PageNo; (* Data_PageSplitSpace *)
                    (* *)
                    nd_last                    : tsp00_PageNo; (* Data_ChainPage      *)
                    nd_filler2                 : tsp00_Int4;
                    (* *)
                    nd_conv_version            : tgg00_ConverterVersion;
                    nd_str_version             : tsp00_Int4;
                    (* *)
                    nd_file_version            : tgg91_FileVersion;
                    nd_history_info            : tbd00_ConsistencyInfo;
                    ndPageVersion_bd00         : tsp00_Uint1;
                    ndInvUsageCnt_bd00         : tsp00_Int4;    (* Log_UndoPage *)
                    (* *)
                    ndLeafCount_bd00           : tsp00_Int4;    (* Log_UndoPage *)
                    ndTreeLeaves_bd00          : tsp00_Int4;
                    (* *)
                    nd_trans_id                : tgg91_TransNo; (* Log_UndoPage *)
                    nd_filler4                 : tsp00_Int2;
                    (* *)
                    ndInvRoot_bd00             : tsp00_PageNo;  (* Log_UndoPage *)
                    (* PTS 1120695 UH 2003-10-27 *)
                    nd_write_cnt               : tsp00_Int4;
                    (* *)
                    nd_pointer_list            : tbd_pointer_list;
                    (* *)
                    nd_checksum                : tsp00_Int4;
                    nd_trailer                 : tgg00_PageTrailer);
                4:
                    (nd_header_space           : tsp00_C8; (* used by tfnObj_egg00 and tfnObjBefore_egg00 *)
                    (* *)
                    nd_max_obj_cnt             : tsp00_Uint2;
                    nd_occ_obj_cnt             : tsp00_Uint2;
                    nd_free_eot_obj_cnt        : tsp00_Uint2;
                    nd_free_obj_cnt            : tsp00_Uint2;
                    (* *)
                    nd_level_space             : tsp00_Int2;
                    nd_file_state_space        : tbd_file_state_set;
                    nd_objpage_state           : tbd_objpage_state_set;
                    nd_root_space              : tsp00_PageNo;
                    (* *)
                    nd_right_space             : tsp00_PageNo;
                    nd_left_space              : tsp00_PageNo;
                    (* *)
                    nd_filler5_2               : tsp00_Int2;
                    nd_in_free_chain           : boolean;
                    nd_num_key_files           : tsp00_Uint1;
                    (* PTS 1115760 2002-05-15 FF                                                 *)
                    (* ndLockRequestSeqNo_bd00 has been migrated to ndLockRequestSeqNoLow_bd00   *)
                    (* and ndLockRequestSeqNoHigh_bd00 because of the eliminated empty-list.     *)
                    ndLockRequestSeqNoLow_bd00 : tbd00_LockRequestSeqNo;
                    (* *)
                    nd_conv_vers_space         : tgg00_ConverterVersion;
                    nd_obj_frame_len           : tsp00_Uint2;
                    nd_obj_key_len             : tsp00_Uint2;
                    (* *)
                    nd_file_vers_space         : tgg91_FileVersion;
                    nd_first_free_obj          : tgg00_ObjPagePos;
                    nd_first_obj               : tgg00_ObjPagePos;
                    nd_filler5_4               : tsp00_Int2;
                    (* *)
                    nd_obj_file_no1            : tsp00_C4;
                    ndLockRequestSeqNoHigh_bd00: tbd00_LockRequestSeqNo; (* PTS 1115760 2002-05-15 FF *)
                    (* *)
                    nd_next_free               : tsp00_PageNo;
                    nd_prev_free               : tsp00_PageNo; (* PTS 1115760 2002-05-15 FF *)
                    (* *)
                    nd_obj_file_no2            : tsp00_C4;
                    nd_write_cnt_space         : tsp00_Int4);
                5:
                    (ndStrHeaderSpace_bd00     : tsp00_C8; (* used by tfnColumn_egg00 *)
                    (* *)
                    ndStrBottomSpace_bd00      : tgg00_PagePos;
                    ndStrFiller1_bd00          : tsp00_Int4;
                    (* *)
                    ndStrLevelSpace_bd00       : tsp00_Int2;
                    ndStrFileStateSpace_bd00   : tbd_file_state_set;
                    ndStrFiller2_bd00          : boolean;
                    ndStrRootSpace_bd00        : tsp00_PageNo;
                    (* *)
                    ndStrRightSpace_bd00       : tsp00_PageNo;
                    ndStrLeftSpace_bd00        : tsp00_PageNo;
                    (* *)
                    ndStrParentID_bd00         : tgg00_Surrogate;
                    (* *)
                    ndStrConvVersionSpace_bd00 : tgg00_ConverterVersion;
                    ndStrVersion_bd00          : tsp00_Int4;
                    (* *)
                    ndStrFileVersionSpace_bd00 : tgg91_FileVersion;
                    ndStrFiller4_bd00          : tsp00_C6;
                    (* *)
                    ndStrFileSize_bd00         : tsp00_Int4;
                    ndStrFiller5_bd00          : tsp00_Int4;
                    (* *)
                    ndStrFiller6_bd00          : tsp00_C8;
                    (* *)
                    ndStrFiller7_bd00          : tsp00_Int4;
                    ndStrWriteCntSpace_bd00    : tsp00_Int4);
                7:
                    (ndHeaderSpace_bd00        : tsp00_C8;
                    (* *)
                    ndFirstOffset_bd00         : tgg00_PagePos;
                    ndFieldCount_bd00          : tsp00_Int4;
                    (* *)
                    nd_level_space2            : tsp00_Int2;
                    nd_file_state_space2       : tbd_file_state_set;
                    nd_sorted_space2           : boolean;
                    nd_root_space2             : tsp00_PageNo;
                    (* *)
                    nd_right_space2            : tsp00_PageNo;
                    nd_left_space2             : tsp00_PageNo;
                    (* *)
                    ndFirstFreeOffset_bd00     : tgg00_PagePos;
                    ndAllFieldInQueueCount_bd00: tsp00_Int4);
                8: (* Data_PageFixSizeSpace *)
                    (ndPageFixSizeSpace_bd00   : tbd00_PageFixSizeSpace);
                9: (* Log_UndoPage *)
                    (ndUndoPage_bd00           : tbd00_UndoPage);
                10 : (* Rst_RedoPage *)
                    (ndRedoPage_bd00           : tbd00_RedoPage);
                11 : (* Log_OpenTransPage *)
                    (ndOpenTransPage_bd00      : tbd00_OpenTransPage);
                END;
            (*ENDCASE*)

      tbd_nodeptr = ^tbd_node;

      tbd_node_ptrs = RECORD
            CASE integer OF
                1:
                    (np_ptr      : tbd_nodeptr;
                    np_cbptr     : tbd_nodeptr);
                2:
                    (np_ptr_buf  : tsp00_PageAddr;
                    np_cbptr_buf : tsp00_PageAddr)
                END;
            (*ENDCASE*)

      tbd_page_list     = ARRAY [1..MAX_VF_BUFFERS_BD00] OF tbd_node;
      (* *)
      tbd_pagelistptr   = ^tbd_page_list;
      (* *)
      tbd00_NodePtrList = ARRAY [1..MAX_VF_BUFFERS_BD00] OF tbd_nodeptr;

      tbd_univ_ptr = RECORD
            CASE integer OF
                1 :
                    (nodeaddr      : tbd_nodeptr);
                2 :
                    (bufaddr       : tsp00_PageAddr);
                3 :
                    (plistaddr     : tbd_pagelistptr);
                END;
            (*ENDCASE*)


      tbd_current_tree = RECORD
            curr_trans                   : tgg00_TransContextPtr;
            currRootNptrs_bd00           : tbd_node_ptrs;
            currIndexNptrs_bd00          : tbd_node_ptrs;
            curr_lvl_1_pno               : tsp00_PageNo;
            curr_lvl_1_left              : tsp00_PageNo;
            currLeftBound_bd00           : tsp00_PageNo;
            currRightBound_bd00          : tsp00_PageNo;
            curr_tree_id                 : tgg00_FileId;
            currFreeQueue_bd00           : tbd00_FreeQueue;
            currNumLeaves_bd00           : tsp00_Int4;
            currLeafPageCountDelta_bd00  : tsp00_Int4;
            currIndexPageCountDelta_bd00 : tsp00_Int4;
            currCounter_bd00             : tsp00_Int2;
            currRootPageLockMode_bd00    : tbd00_PageLockMode;
            currIndexPageLockMode_bd00   : tbd00_PageLockMode;
            curr_action                  : tgg00_MessType;
            currUnlockAllowed_bd00       : boolean;
            currRootUpdated_bd00         : boolean;
            currIndexUpdated_bd00        : boolean;
            currExtendedFileTypeSet_bd00 : tgg00_ExtendedFiletypeSet;
            currFill1                    : boolean;
            currFill2                    : tsp00_Int2;
      END;


      tbd_neighbors = RECORD
            pre_ln : tsp00_PageNo;
            ln     : tsp00_PageNo;
            rn     : tsp00_PageNo
      END;

      tbd_searchresult = (
            thisfound,
            nextfound,
            lastfound,
            nonefound);
      (* *)
      tbd00_SepLeafCount = tgg90_Cint4;
      (* *)

      tbd00_Separator = RECORD
            CASE integer OF
                1:
                    (sepHead_bd00     : tgg00_HeaderRec);
                2:
                    (sepKey_bd00      : tgg00_Lkey;
                    sepLeafCount_bd00 : tbd00_SepLeafCount);
                END;
            (*ENDCASE*)

      tbd00_SeparatorPtr = ^tbd00_Separator;
      (* *)
      tbd00_TreeIndexOp  = (
            opAdd_ebd00,
            opDel_ebd00,
            opRepl_ebd00);

      tbd00_TreeIndexOrder = RECORD
            txoOp_bd00        : tbd00_TreeIndexOp;
            txoFill_bd00      : boolean;
            txoLevel_bd00     : tsp00_Int2;
            txoSearchKey_bd00 : tgg00_Lkey;
            txoSep_bd00       : tbd00_Separator
      END;

      tbd00_TreeIndexOrderPtr  = ^tbd00_TreeIndexOrder;

      tbd00_TreeStatistic = RECORD
            ts_IndexPages_bd00  : tsp00_Int4;
            ts_LeafPages_bd00   : tsp00_Int4;
            ts_RecordCount_bd00 : tsp00_Int4;
      END;


      tbd00_OrderList = RECORD
            olstCount_bd00 : tsp00_Int2; (* 0..6 *)
            olstFill_bd00  : tsp00_Int2;
            olstOrder_bd00 : ARRAY  [LWB_OF_LIST_BD00..UPB_OF_LIST_BD00] OF tbd00_TreeIndexOrder
      END;


      tbd_stat_aux_vars = RECORD
            sav_separators  : tsp00_Int4;
            sav_long_oflw   : boolean;
            sav_reclen_oflw : boolean
      END;



      hexbuf_line = RECORD
            CASE integer OF
                1:
                    (ln : tsp00_Line);
                2:
                    (prefix : tsp00_C4)
                END;
            (*ENDCASE*)


      int_map_c4 = RECORD
            CASE boolean OF
                TRUE:
                    (map_i4 : tsp00_Int4);
                FALSE:
                    (map_c4 : tsp00_C4)
                END;
            (*ENDCASE*)



(*------------------------------*)

FUNCTION
              gg06Int4FromUint2 (Uint2 : tsp00_Uint2): tsp00_Int4;
        EXTERNAL ;



        PROCEDURE
              gg06Int4ToUint2 (Int4 : tsp00_Int4;
                    VAR Uint2       : tsp00_Uint2);
        EXTERNAL ;



        FUNCTION
              gg06PnoGet (VAR Cint4 : tgg90_Cint4): tsp00_Int4;
        EXTERNAL ;



        PROCEDURE
              gg06PnoPut (VAR Cint4 : tgg90_Cint4; int4 : tsp00_Int4);
        EXTERNAL ;



      PROCEDURE
              g17hexto_line (c : char;
                    VAR ln_len : integer;
                    VAR ln     : tsp00_Line);
        EXTERNAL ;



        PROCEDURE
              g17int4to_line (int : tsp00_Int4;
                    with_zero : boolean;
                    int_len   : integer;
                    ln_pos    : integer;
                    VAR ln    : tsp00_Line);
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



        FUNCTION
              g17printable_char (c : char) : boolean;
        EXTERNAL ;



        PROCEDURE
              g17sname_to_line (n : tsp00_Sname;
                    VAR ln_len    : integer;
                    VAR ln        : tsp00_Line);
        EXTERNAL ;



        PROCEDURE
              g17trimint4_to_line (int : tsp00_Int4;
                    VAR ln_len : integer;
                    VAR ln     : tsp00_Line);
        EXTERNAL ;



      PROCEDURE
              t09getcursor_line (VAR t09 : tut_terminal;
                    VAR ln    : tsp00_Line;
                    VAR pfkey : tut_pfkey);
        EXTERNAL ;



        PROCEDURE
              t09get1name (VAR t09 : tut_terminal;
                    msg            : tsp00_C20;
                    msg_attr       : char;
                    in_attr        : char;
                    in_len         : integer;
                    upper_case     : boolean;
                    VAR in_name    : tsp00_Name;
                    VAR pfkey      : tut_pfkey);
        EXTERNAL ;



        PROCEDURE
              t09get2name (VAR t09 : tut_terminal;
                    msg1,        msg2        : tsp00_C20;
                    msg1_attr,   msg2_attr   : char;
                    in1_attr,    in2_attr    : char;
                    in1_len,     in2_len     : integer;
                    upper_case1, upper_case2 : boolean;
                    VAR in1_name,in2_name    : tsp00_Name;
                    VAR pfkey                : tut_pfkey);
        EXTERNAL ;



        PROCEDURE
              t09goto_bottom (VAR term : tut_terminal);
        EXTERNAL ;



        PROCEDURE
              t09holding (VAR t09 : tut_terminal;
                    VAR pfkey     : tut_pfkey);
        EXTERNAL ;



        FUNCTION
              t09is_empty_window (VAR term : tut_terminal): boolean;
        EXTERNAL ;



        FUNCTION
              t09is_window_full (VAR term : tut_terminal;
                    wanted_lines: integer): boolean;
        EXTERNAL ;



        PROCEDURE
              t09newscreen_page (VAR t09 : tut_terminal);
        EXTERNAL ;



        PROCEDURE
              t09pfkeys (VAR term : tut_terminal; on : boolean);
        EXTERNAL ;



        PROCEDURE
              t09put (VAR t09 : tut_terminal;
                    VAR text  : tsp00_Line;
                    text_attr : char);
        EXTERNAL ;



        PROCEDURE
              t09put60 (VAR term : tut_terminal;
                    ln60         : tsp00_C60;
                    text_attr    : char);
        EXTERNAL ;



        PROCEDURE
              t09putmsg (VAR t09      : tut_terminal;
                    VAR text          : tsp00_Line;
                    is_warning        : boolean;
                    immediate_display : boolean);
        EXTERNAL ;



        PROCEDURE
              t09setpf1 (VAR term : tut_terminal; pfkey_label : tsp00_C8);
        EXTERNAL ;



        FUNCTION
              t09window_cols (VAR term : tut_terminal): integer;
        EXTERNAL ;



        FUNCTION
              t09unused_window_lines (VAR term : tut_terminal): integer;
        EXTERNAL ;



        PROCEDURE
              t09uppercase_line (VAR ln : tsp00_Line;
                    lwb : integer;
                    upb : integer);
        EXTERNAL ;



      PROCEDURE
              t12write_prot (VAR fileref : tut_vf_fileref;
                    VAR ln    : tsp00_Line;
                    length    : integer;
                    VAR error : integer);
        EXTERNAL ;



      PROCEDURE
              x04afterimagehead_display (VAR term : tut_terminal;
                    VAR protfile      : tut_vf_fileref;
                    scan              : tut_diag_scan;
                    VAR buf           : tsp00_MoveObj;
                    buf_size          : tsp00_Int4;
                    entry_offset      : tsp00_Int2;
                    entry_length      : tsp00_Int4;
                    currentIOSequence : tsp00_Int4;
                    VAR pfkey         : tut_pfkey;
                    VAR ln            : tsp00_Line;
                    VAR ln_len        : integer;
                    VAR err           : integer);
        EXTERNAL ;



      PROCEDURE
              x07debug_page (VAR term : tut_terminal;
                    VAR protfile      : tut_vf_fileref;
                    header_only       : boolean;
                    VAR file_pos      : tut_file_pos;
                    VAR buf           : tsp00_MoveObj;
                    buf_size          : tsp00_Int4;
                    VAR pfkey         : tut_pfkey);
        EXTERNAL ;



        PROCEDURE
              x07edit_page (VAR term : tut_terminal;
                    VAR protfile     : tut_vf_fileref;
                    VAR file_pos     : tut_file_pos;
                    VAR buf          : tsp00_MoveObj;
                    buf_size         : tsp00_Int4;
                    VAR pfkey        : tut_pfkey);
        EXTERNAL ;



        PROCEDURE
              x07keys_display (VAR term : tut_terminal;
                    VAR protfile : tut_vf_fileref;
                    VAR file_pos : tut_file_pos;
                    VAR buf      : tsp00_MoveObj;
                    buf_size     : tsp00_Int4;
                    VAR pfkey    : tut_pfkey);
        EXTERNAL ;



      PROCEDURE
              x08beforeimage_edit_display (VAR term : tut_terminal;
                    VAR prot   : tut_vf_fileref;
                    scan       : tut_diag_scan;
                    VAR buf    : tsp00_MoveObj;
                    buf_size   : tsp00_Int4;
                    buf_offset : tsp00_Int4;
                    VAR pfkey  : tut_pfkey);
        EXTERNAL ;



        PROCEDURE
              x08fdir_rec_edit (VAR term : tut_terminal;
                    VAR prot   : tut_vf_fileref;
                    VAR buf    : tsp00_MoveObj;
                    buf_size   : tsp00_Int4;
                    buf_offset : tsp00_Int4;
                    is_edit    : boolean;
                    VAR pfkey  : tut_pfkey);
        EXTERNAL ;



        PROCEDURE
              x08obj_edit (VAR term : tut_terminal;
                    VAR prot   : tut_vf_fileref;
                    VAR buf    : tsp00_MoveObj;
                    buf_size   : tsp00_Int4;
                    buf_offset : tsp00_Int4;
                    is_edit    : boolean;
                    VAR pfkey  : tut_pfkey);
        EXTERNAL ;



        PROCEDURE
              x08rec_edit (VAR term : tut_terminal;
                    VAR prot   : tut_vf_fileref;
                    VAR buf    : tsp00_MoveObj;
                    buf_size   : tsp00_Int4;
                    buf_offset : tsp00_Int4;
                    is_edit    : boolean;
                    is_catalog : boolean;
                    page_type2 : tgg00_PageType2;
                    VAR pfkey  : tut_pfkey);
        EXTERNAL ;



        PROCEDURE
              x08tree_index_rec_edit (VAR term : tut_terminal;
                    VAR prot   : tut_vf_fileref;
                    VAR buf    : tsp00_MoveObj;
                    buf_size   : tsp00_Int4;
                    buf_offset : tsp00_Int4;
                    is_edit    : boolean;
                    VAR pfkey  : tut_pfkey);
        EXTERNAL ;



      PROCEDURE
              SAPDB_PascalForcedMove (
                    source_upb  : tsp00_Int4;
                    destin_upb  : tsp00_Int4;
                    source      : tsp00_MoveObjPtr;
                    source_pos  : tsp00_Int4;
                    destin      : tsp00_MoveObjPtr;
                    destin_pos  : tsp00_Int4;
                    length      : tsp00_Int4);
        EXTERNAL ;



        PROCEDURE
              SAPDB_PascalForcedOverlappingMove (
                    source_upb  : tsp00_Int4;
                    destin_upb  : tsp00_Int4;
                    source      : tsp00_MoveObjPtr;
                    source_pos  : tsp00_Int4;
                    destin      : tsp00_MoveObjPtr;
                    destin_pos  : tsp00_Int4;
                    length      : tsp00_Int4);
        EXTERNAL ;



        PROCEDURE
              SAPDB_PascalForcedFill (
                    size        : tsp00_Int4;
                    m           : tsp00_MoveObjPtr;
                    pos         : tsp00_Int4;
                    len         : tsp00_Int4;
                    fillchar    : char);
        EXTERNAL ;



PROCEDURE
      t14bufdimension (VAR term : tut_terminal;
            scan                : tut_diag_scan;
            VAR bytes_per_line  : integer;
            VAR line_size       : integer;
            VAR lines_per_buf   : integer);
        FORWARD;

PROCEDURE
      t14bufdisplay (VAR term : tut_terminal;
            VAR protfile      : tut_vf_fileref;
            scan              : tut_diag_scan;
            msg               : tsp00_C30;
            VAR buf           : tsp00_MoveObj;
            buf_size          : tsp00_Int4;
            startpos          : tsp00_Int4;
            endpos            : tsp00_Int4;
            start_numbering   : tsp00_Int4;
            VAR break_pos     : tsp00_Int4;
            VAR pfkey         : tut_pfkey);
        FORWARD;

PROCEDURE
      t14display_msg (VAR term : tut_terminal; msg : tsp00_C20);
        FORWARD;

PROCEDURE
      t14editbuf (VAR term : tut_terminal;
            VAR protfile   : tut_vf_fileref;
            VAR buf        : tsp00_MoveObj;
            buf_size       : tsp00_Int4;
            buf_offset     : tsp00_Int4;
            VAR file_pos   : tut_file_pos;
            edit_scan      : tut_diag_scan;
            VAR is_saved   : boolean;
            VAR pfkey      : tut_pfkey);
        FORWARD;

PROCEDURE
      t14int4from_line (VAR term : tut_terminal;
            VAR ln  : tsp00_Line;
            ln_pos  : integer;
            ln_len  : integer;
            VAR int : tsp00_Int4;
            VAR ok  : boolean);
        FORWARD;

PROCEDURE
      t14logentry_edit (VAR term : tut_terminal;
            VAR protfile : tut_vf_fileref;
            VAR node     : tsp00_MoveObj;
            VAR file_pos : tut_file_pos;
            entry_pos    : tsp00_Int4;
            entry_length : tsp00_Int4;
            VAR is_saved : boolean;
            VAR pfkey    : tut_pfkey);
        FORWARD;

PROCEDURE
      t14object_edit (VAR term : tut_terminal;
            VAR protfile : tut_vf_fileref;
            VAR node     : tsp00_MoveObj;
            VAR file_pos : tut_file_pos;
            obj_pos      : tsp00_Int4;
            VAR is_saved : boolean;
            VAR pfkey    : tut_pfkey);
        FORWARD;

PROCEDURE
      t14record_edit (VAR term : tut_terminal;
            VAR protfile : tut_vf_fileref;
            VAR node     : tsp00_MoveObj;
            node_size    : tsp00_Int4;
            VAR file_pos : tut_file_pos;
            edit_scan    : tut_diag_scan;
            VAR is_saved : boolean;
            VAR pfkey    : tut_pfkey);
        FORWARD;

PROCEDURE
      ta14display_leave_msg (VAR term : tut_terminal);
        FORWARD;

PROCEDURE
      ta14edit_bufdimension (VAR term : tut_terminal;
            buf_size           : tsp00_Int4;
            VAR char_per_byte  : integer;
            VAR bytes_per_line : integer;
            VAR line_size      : integer;
            VAR lines_per_buf  : integer);
        FORWARD;

PROCEDURE
      ta14fill (VAR term : tut_terminal;
            VAR protfile : tut_vf_fileref;
            VAR buf      : tsp00_MoveObj;
            buf_size     : tsp00_Int4;
            buf_offset   : tsp00_Int4;
            init_pos     : tsp00_Int4;
            VAR pfkey    : tut_pfkey);
        FORWARD;

PROCEDURE
      ta14help (VAR term : tut_terminal);
        FORWARD;

PROCEDURE
      ta14hex_from_line (VAR term : tut_terminal;
            VAR ln     : tsp00_Line;
            ln_pos     : integer;
            VAR result : char;
            VAR ok     : boolean);
        FORWARD;

FUNCTION
      ta14is_nil_pno (VAR ln : tsp00_Line;
            ln_offset : integer) : boolean;
        FORWARD;

PROCEDURE
      ta14move (VAR term : tut_terminal;
            VAR protfile : tut_vf_fileref;
            VAR buf      : tsp00_MoveObj;
            buf_size     : tsp00_Int4;
            buf_offset   : tsp00_Int4;
            init_pos     : tsp00_Int4;
            VAR pfkey    : tut_pfkey);
        FORWARD;

PROCEDURE
      ta14mtype_assign (VAR mtype_value : tsp00_Name;
            VAR buf : tsp00_MoveObj;
            buf_pos : tsp00_Int4);
        FORWARD;

PROCEDURE
      ta14mtype2_assign (VAR mtype_value : tsp00_Name;
            VAR buf : tsp00_MoveObj;
            buf_pos : tsp00_Int4);
        FORWARD;

PROCEDURE
      ta14set_assign (VAR ln : tsp00_Line;
            ln_offset : integer;
            VAR buf   : tsp00_MoveObj;
            buf_pos   : tsp00_Int4);
        FORWARD;

PROCEDURE
        t14bufdimension ;



BEGIN
IF  utds_minbuf in scan
THEN
    BEGIN
    lines_per_buf  := c_minbuf_lines;
    bytes_per_line :=
          (((t09window_cols (term) - mx_minbuf_prefix)
          DIV mx_minbuf_byte)
          DIV c_minbuf_block) * c_minbuf_block;
    IF  mx_minbuf_prefix
        + bytes_per_line * mx_minbuf_byte
        + ((bytes_per_line - 1) DIV c_minbuf_block) (*block delimiter*)
        > t09window_cols (term)
    THEN
        bytes_per_line := bytes_per_line - c_minbuf_block;
    (*ENDIF*)
    line_size :=
          mx_minbuf_prefix
          + bytes_per_line * mx_minbuf_byte
          + ((bytes_per_line - 1) DIV c_minbuf_block)
    END
ELSE
    BEGIN
    lines_per_buf := c_maxbuf_lines;
    IF  utds_double_numbering in scan
    THEN
        lines_per_buf := lines_per_buf + 1;
    (*ENDIF*)
    bytes_per_line :=
          (((t09window_cols (term) - mx_maxbuf_prefix)
          DIV mx_maxbuf_byte)
          DIV c_maxbuf_block) * c_maxbuf_block;
    line_size :=
          mx_maxbuf_prefix
          + bytes_per_line * mx_maxbuf_byte
    END
(*ENDIF*)
END;

(*------------------------------*)

PROCEDURE
        t14bufdisplay ;



CONST
      min_char_count = 3;

VAR
      skip_output    : boolean;
      last_skipped   : boolean;
      is_blank       : boolean;
      blank_only     : boolean;
      vt_attr        : char;
      cint4          : tgg90_Cint4;
      upb            : tsp00_Int4;
      buf_pos        : tsp00_Int4;
      numb_pos       : tsp00_Int4;
      out_pos        : tsp00_Int4;
      pos_displ      : tsp00_Int4;
      skip_numb_pos  : tsp00_Int4;
      dummy_int      : integer;
      line_size      : integer;
      lines_per_buf  : integer;
      ln_len         : integer;
      i              : integer;
      err            : integer;
      bytes_per_line : integer;
      byte_count     : tsp00_Int4;
      char_count     : tsp00_Int4;
      prev_char_cnt  : tsp00_Int4;
      i2             : tsp00_IntMapC2;
      i4             : int_map_c4;
      i4_overflow    : int_map_c4;
      chrline        : hexbuf_line;
      c2_line        : hexbuf_line;
      c4_line        : hexbuf_line;
      decline        : hexbuf_line;
      hexline        : hexbuf_line;
      int2_line      : hexbuf_line;
      int4_line      : hexbuf_line;
      posline        : hexbuf_line;
      posline_2      : hexbuf_line;
      spaceline      : tsp00_Line;
      u2_line        : hexbuf_line;
      odd_bytecount  : boolean;

BEGIN
break_pos := 0;
pfkey     := pf_none;
IF  utds_edit in scan
THEN
    BEGIN
    vt_attr := cut_unprotected;
    ta14edit_bufdimension (term, buf_size - startpos + start_numbering,
          dummy_int, bytes_per_line, line_size, lines_per_buf);
    i4_overflow.map_i4 := -MAX_INT4_SP00;
    FOR i := 1 TO 4 DO
        IF  i4_overflow.map_c4 [i] = chr(1)
        THEN
            i4_overflow.map_c4 [i] := chr(0)
        (*ENDIF*)
    (*ENDFOR*)
    END
ELSE
    BEGIN
    vt_attr := cut_protected;
    t14bufdimension (term, scan, bytes_per_line, line_size,
          lines_per_buf)
    END;
(*ENDIF*)
IF  (startpos < 1) OR (startpos > buf_size)
THEN
    BEGIN
    buf_pos := 1;
    upb     := bytes_per_line
    END
ELSE
    BEGIN
    buf_pos := startpos;
    IF  (endpos < startpos) OR (endpos > buf_size)
    THEN
        upb := buf_size
    ELSE
        upb := endpos
    (*ENDIF*)
    END;
(*ENDIF*)
numb_pos := start_numbering;
IF  utds_edit in scan
THEN
    BEGIN
    t09put (term, term.blankline, cut_protected);
    t12write_prot (protfile, term.blankline, 1, err)
    END
ELSE
    BEGIN
    chrline.ln := term.blankline;
    ln_len     := 0;
    g17sname_to_line ('BUFFER FROM ', ln_len, chrline.ln);
    ln_len := ln_len + 1;
    g17trimint4_to_line (numb_pos, ln_len, chrline.ln);
    g17sname_to_line (' TO         ', ln_len, chrline.ln);
    ln_len := ln_len + 1;
    g17trimint4_to_line (numb_pos + upb - buf_pos, ln_len, chrline.ln);
    IF  (utds_double_numbering in scan)
        AND
        NOT (utds_minbuf in scan)
    THEN
        BEGIN
        g17sname_to_line (' (FROM      ', ln_len, chrline.ln);
        ln_len := ln_len + 1;
        g17trimint4_to_line (buf_pos, ln_len, chrline.ln);
        g17sname_to_line (' TO         ', ln_len, chrline.ln);
        ln_len := ln_len + 1;
        g17trimint4_to_line (upb, ln_len, chrline.ln);
        ln_len := ln_len + 1;
        chrline.ln [ln_len] := ')'
        END;
    (*ENDIF*)
    IF  buf_pos <> startpos
    THEN
        BEGIN
        g17sname_to_line ('   *** from ', ln_len, chrline.ln);
        ln_len := ln_len + 1;
        g17trimint4_to_line (startpos, ln_len, chrline.ln)
        END;
    (*ENDIF*)
    IF  upb <> endpos
    THEN
        BEGIN
        g17sname_to_line ('   *** to   ', ln_len, chrline.ln);
        ln_len := ln_len + 1;
        g17trimint4_to_line (endpos, ln_len, chrline.ln)
        END;
    (*ENDIF*)
    IF  ((utds_minbuf in scan               ) OR
        ( upb - buf_pos + 1 < bytes_per_line))
        AND
        (msg = bsp_c30)
    THEN
        posline := chrline
    ELSE
        BEGIN
        posline.ln := term.blankline;
        IF  msg = bsp_c30
        THEN
            SAPDB_PascalForcedMove (sizeof (chrline.ln), sizeof (posline.ln),
                  @chrline.ln, 1, @posline.ln, line_size-ln_len+1, ln_len)
        ELSE
            BEGIN
            SAPDB_PascalForcedMove (sizeof (msg), sizeof (posline.ln), @msg, 1,
                  @posline.ln, 1, sizeof (msg));
            IF  upb - buf_pos >= bytes_per_line
            THEN
                SAPDB_PascalForcedMove (sizeof (chrline.ln), sizeof (posline.ln),
                      @chrline.ln, 1,
                      @posline.ln, line_size - ln_len + 1, ln_len)
            (*ENDIF*)
            END
        (*ENDIF*)
        END;
    (*ENDIF*)
    t12write_prot (protfile, posline.ln, sizeof (posline.ln), err);
    IF  utds_display in scan
    THEN
        BEGIN
        IF  msg = bsp_c30
        THEN
            t09put (term, posline.ln, cut_protected)
        ELSE
            t09put (term, posline.ln, cut_bright_protected)
        (*ENDIF*)
        END
    (*ENDIF*)
    END;
(*ENDIF*)
last_skipped  := false;
prev_char_cnt := 0;
char_count    := 0;
blank_only    := true;
REPEAT
    skip_output := false;
    IF  (buf_pos > startpos) AND (buf_pos + bytes_per_line - 1 < upb)
        AND NOT (utds_edit in scan)
    THEN
        BEGIN
        i := buf_pos;
        skip_output := true;
        REPEAT
            IF  buf [i-bytes_per_line] <> buf [i]
            THEN
                skip_output := false;
            (*ENDIF*)
            i := i + 1
        UNTIL
            NOT skip_output OR (i >= buf_pos + bytes_per_line)
        (*ENDREPEAT*)
        END;
    (*ENDIF*)
    IF  skip_output AND NOT last_skipped
    THEN
        BEGIN
        last_skipped  := true;
        skip_numb_pos := numb_pos
        END;
    (*ENDIF*)
    IF  skip_output
    THEN
        BEGIN
        buf_pos  := buf_pos  + bytes_per_line;
        numb_pos := numb_pos + bytes_per_line
        END
    ELSE
        IF  last_skipped
            OR
            (   NOT (utds_minbuf in scan)
            AND NOT (utds_edit   in scan)
            AND     (buf_pos > startpos )
            AND     (blank_only OR (char_count = 0)) )
        THEN
            (* write number of skipped bytes  OR  write blank line *)
            BEGIN
            IF  (utds_display in scan)
                AND
                t09is_window_full (term, 1 + lines_per_buf)
            THEN
                BEGIN
                t09holding (term, pfkey);
                IF  pfkey = pf_none
                THEN
                    t09newscreen_page (term);
                (*ENDIF*)
                IF  pfkey <> pf_none
                THEN
                    break_pos := buf_pos
                (*ENDIF*)
                END;
            (*ENDIF*)
            IF  last_skipped AND (pfkey = pf_none)
            THEN
                BEGIN
                last_skipped := false;
                chrline.ln   := term.blankline;
                ln_len       := 0;
                g17sname_to_line   ('     ...    ', ln_len, chrline.ln);
                ln_len := ln_len + 1;
                g17trimint4_to_line (skip_numb_pos, ln_len, chrline.ln);
                g17sname_to_line   (' TO         ', ln_len, chrline.ln);
                ln_len := ln_len + 1;
                g17trimint4_to_line (numb_pos-1,    ln_len, chrline.ln);
                IF  (utds_double_numbering in scan)
                    AND
                    NOT (utds_minbuf in scan)
                THEN
                    BEGIN
                    ln_len := ln_len + 2;
                    chrline.ln [ln_len] := '(';
                    g17trimint4_to_line (buf_pos - numb_pos
                          + skip_numb_pos, ln_len, chrline.ln);
                    g17sname_to_line (' TO         ',
                          ln_len, chrline.ln);
                    ln_len := ln_len + 1;
                    g17trimint4_to_line (buf_pos-1, ln_len, chrline.ln);
                    ln_len := ln_len + 1;
                    chrline.ln [ln_len] := ')'
                    END;
                (*ENDIF*)
                t12write_prot (protfile, chrline.ln, ln_len, err);
                IF  (pfkey = pf_none) AND (utds_display in scan)
                THEN
                    t09put (term, chrline.ln, cut_protected);
                (*ENDIF*)
                END
            ELSE
                IF  pfkey = pf_none
                THEN
                    BEGIN
                    t12write_prot (protfile, term.blankline, 1, err);
                    IF  (utds_display in scan)
                        AND NOT t09is_empty_window (term)
                    THEN
                        t09put (term, term.blankline, cut_protected)
                    (*ENDIF*)
                    END
                (*ENDIF*)
            (*ENDIF*)
            END;
        (*ENDIF*)
    (*ENDIF*)
    IF  NOT skip_output AND (pfkey = pf_none)
    THEN
        BEGIN
        hexline.ln := term.blankline;
        chrline.ln := term.blankline;
        IF  utds_minbuf in scan
        THEN
            BEGIN
            g17int4to_line (numb_pos, NOT c_with_zero,
                  mx_minbuf_prefix, 1, chrline.ln);
            out_pos := mx_minbuf_prefix + 1
            END
        ELSE
            BEGIN
            out_pos          := mx_maxbuf_prefix + 1;
            posline.ln       := term.blankline;
            posline_2.ln     := term.blankline;
            decline.ln       := term.blankline;
            posline.prefix   := c_pos_prefix;
            decline.prefix   := c_dec_prefix;
            hexline.prefix   := c_hex_prefix;
            chrline.prefix   := c_chr_prefix;
            IF  utds_edit in scan
            THEN
                BEGIN
                c2_line.ln       := term.blankline;
                c4_line.ln       := term.blankline;
                int2_line.ln     := term.blankline;
                int4_line.ln     := term.blankline;
                spaceline        := term.blankline;
                u2_line.ln       := term.blankline;
                c2_line.prefix   := c_c2_prefix;
                c4_line.prefix   := c_c4_prefix;
                int2_line.prefix := c_int2_prefix;
                int4_line.prefix := c_int4_prefix;
                u2_line.prefix   := c_u2_prefix
                END
            (*ENDIF*)
            END;
        (*ENDIF*)
        byte_count := 0;
        char_count := 0;
        blank_only := true;
        REPEAT
            byte_count := byte_count + 1;
            IF  NOT (utds_minbuf in scan)
            THEN
                BEGIN
                IF  utds_edit in scan
                THEN
                    BEGIN
                    IF  byte_count = 1
                    THEN
                        BEGIN
                        int2_line.ln [out_pos] := ':';
                        int4_line.ln [out_pos] := ':'
                        END;
                    (*ENDIF*)
                    out_pos := out_pos + 1;
                    IF  byte_count > 1
                    THEN
                        BEGIN
                        posline.ln    [out_pos] := '|';
                        posline_2.ln  [out_pos] := '|';
                        decline.ln    [out_pos] := '|';
                        hexline.ln    [out_pos] := '|';
                        chrline.ln    [out_pos] := '|';
                        spaceline     [out_pos] := '+';
                        IF  byte_count MOD 2 = 1
                        THEN
                            BEGIN
                            c2_line.ln   [out_pos] := '|';
                            u2_line.ln   [out_pos] := '|';
                            int2_line.ln [out_pos] := '|'
                            END;
                        (*ENDIF*)
                        IF  byte_count MOD 4 = 1
                        THEN
                            BEGIN
                            c4_line.ln   [out_pos] := '|';
                            int4_line.ln [out_pos] := '|'
                            END;
                        (*ENDIF*)
                        out_pos := out_pos + 1
                        END;
                    (*ENDIF*)
                    IF  numb_pos < 1000
                    THEN
                        i := 3
                    ELSE
                        i := 4;
                    (*ENDIF*)
                    IF  buf_size - startpos + start_numbering >= 10000
                    THEN
                        i := i + 1;
                    (*ENDIF*)
                    g17int4to_line (numb_pos, NOT c_with_zero, i,
                          out_pos, posline.ln);
                    IF  utds_double_numbering in scan
                    THEN
                        BEGIN
                        IF  buf_pos < 1000
                        THEN
                            i := 3
                        ELSE
                            i := 4;
                        (*ENDIF*)
                        IF  buf_size >= 10000
                        THEN
                            i := i + 1;
                        (*ENDIF*)
                        g17int4to_line (buf_pos, NOT c_with_zero, i,
                              out_pos, posline_2.ln)
                        END;
                    (*ENDIF*)
                    IF  buf_size >= 10000
                    THEN
                        BEGIN
                        spaceline [out_pos] := '-';
                        out_pos := out_pos + 1
                        END;
                    (*ENDIF*)
                    spaceline [out_pos  ] := '-';
                    spaceline [out_pos+1] := '-';
                    spaceline [out_pos+2] := '-';
                    spaceline [out_pos+3] := '-';
                    END
                ELSE
                    (* maxbuf *)
                    BEGIN
                    IF  byte_count = 1
                    THEN
                        BEGIN
                        IF  (numb_pos < 1000)
                            AND NOT (utds_double_numbering in scan)
                        THEN
                            g17int4to_line (numb_pos, NOT c_with_zero,
                                  mx_maxbuf_byte, out_pos, posline.ln)
                        ELSE
                            g17int4to_line (numb_pos, NOT c_with_zero,
                                  mx_maxbuf_prefix + mx_maxbuf_byte, 1,
                                  posline.ln)
                        (*ENDIF*)
                        END
                    ELSE
                        IF  (numb_pos < 100)
                            OR
                            (byte_count MOD 2 = 1)
                        THEN
                            BEGIN
                            IF  numb_pos < 1000
                            THEN
                                pos_displ := numb_pos
                            ELSE
                                pos_displ := numb_pos MOD 1000;
                            (*ENDIF*)
                            g17int4to_line (pos_displ,
                                  (numb_pos >= 1000),
                                  mx_maxbuf_byte, out_pos, posline.ln)
                            END;
                        (*ENDIF*)
                    (*ENDIF*)
                    IF  utds_double_numbering in scan
                    THEN
                        BEGIN
                        IF  byte_count = 1
                        THEN
                            g17int4to_line (buf_pos, NOT c_with_zero,
                                  mx_maxbuf_prefix + mx_maxbuf_byte, 1,
                                  posline_2.ln)
                        ELSE
                            IF  (buf_pos < 100)
                                OR
                                (byte_count MOD 2 = 1)
                            THEN
                                BEGIN
                                IF  buf_pos < 1000
                                THEN
                                    pos_displ := buf_pos
                                ELSE
                                    pos_displ := buf_pos MOD 1000;
                                (*ENDIF*)
                                g17int4to_line (pos_displ,
                                      (buf_pos >= 1000),
                                      mx_maxbuf_byte, out_pos,
                                      posline_2.ln)
                                END
                            (*ENDIF*)
                        (*ENDIF*)
                        END
                    (*ENDIF*)
                    END;
                (*ENDIF*)
                g17int4to_line (ord (buf [buf_pos]), NOT c_with_zero, 3,
                      out_pos, decline.ln);
                END
            ELSE
                (* minbuf without blank space *)
                out_pos := out_pos - 1;
            (*ENDIF*)
            ;
            (* edit, minbuf, maxbuf *)
            ln_len := out_pos;
            g17hexto_line (buf [buf_pos], ln_len, hexline.ln);
            IF  g17printable_char (buf [buf_pos])
            THEN
                chrline.ln [out_pos+2] := buf [buf_pos]
            ELSE
                IF  utds_minbuf in scan
                THEN
                    BEGIN
                    odd_bytecount := (byte_count MOD  2) = 1;
                    IF  odd_bytecount AND
                        (byte_count MOD 10 <> 1)
                    THEN
                        chrline.ln [out_pos+1] := '|'
                    (*ENDIF*)
                    END;
                (*ENDIF*)
            (*ENDIF*)
            IF  utds_edit in scan
            THEN
                BEGIN
                IF  byte_count MOD 2 = 0
                THEN
                    BEGIN
                    cint4.ci4_gg00 [1] := chr (0);
                    cint4.ci4_gg00 [2] := chr (0);
                    cint4.ci4_gg00 [3] := buf [buf_pos-1];
                    cint4.ci4_gg00 [4] := buf [buf_pos  ];
                    (* *)
                    i2.mapC2_sp00 [1] := buf [buf_pos-1];
                    i2.mapC2_sp00 [2] := buf [buf_pos  ];
                    (* *)
                    g17int4to_line (gg06Int4FromUint2 (i2.mapInt_sp00),
                          NOT c_with_zero, 6, out_pos-3, u2_line.ln);
                    g17int4to_line (gg06PnoGet (cint4),
                          NOT c_with_zero, 6, out_pos-3, c2_line.ln);
                    g17int4to_line (i2.mapInt_sp00,
                          NOT c_with_zero, 6, out_pos-3, int2_line.ln)
                    END;
                (*ENDIF*)
                IF  byte_count MOD 4 = 0
                THEN
                    BEGIN
                    FOR i := 1 TO 4 DO
                        i4.map_c4 [i] := buf [buf_pos-4+i];
                    (*ENDFOR*)
                    cint4.ci4_gg00 := i4.map_c4;
                    IF  i4.map_c4 = i4_overflow.map_c4
                    THEN
                        BEGIN
                        int4_line.ln [out_pos-1] := '-';
                        int4_line.ln [out_pos  ] := '.';
                        int4_line.ln [out_pos+1] := '-'
                        END
                    ELSE
                        g17int4to_line (i4.map_i4,
                              NOT c_with_zero, 12, out_pos-9, int4_line.ln);
                    (*ENDIF*)
                    IF  gg06PnoGet (cint4) >= 0
                    THEN
                        g17int4to_line (gg06PnoGet (cint4),
                              NOT c_with_zero, 12, out_pos-9, c4_line.ln)
                    (*ENDIF*)
                    END
                (*ENDIF*)
                END;
            (*ENDIF*)
            IF  utds_minbuf in scan
            THEN
                BEGIN
                out_pos := 1 + out_pos + mx_minbuf_byte;
                IF  byte_count MOD 10 = 0
                THEN
                    out_pos := out_pos + 1
                (*ENDIF*)
                END
            ELSE
                out_pos := out_pos + mx_maxbuf_byte;
            (*ENDIF*)
            buf_pos  := buf_pos  + 1;
            numb_pos := numb_pos + 1
        UNTIL
            (byte_count >= bytes_per_line) OR (buf_pos > upb);
        (*ENDREPEAT*)
        IF  (utds_display in scan)
            AND
            t09is_window_full (term, lines_per_buf)
        THEN
            BEGIN
            t09holding (term, pfkey);
            IF  pfkey = pf_none
            THEN
                t09newscreen_page (term);
            (*ENDIF*)
            IF  pfkey <> pf_none
            THEN
                break_pos := buf_pos
            (*ENDIF*)
            END;
        (*ENDIF*)
        IF  (pfkey = pf_none) AND (utds_display in scan)
        THEN
            BEGIN
            IF  NOT (utds_minbuf in scan)
            THEN
                BEGIN
                t09put (term, posline.ln, cut_protected);
                IF  utds_double_numbering in scan
                THEN
                    t09put (term, posline_2.ln, cut_protected);
                (*ENDIF*)
                IF  utds_edit in scan
                THEN
                    t09put (term, spaceline, cut_protected);
                (*ENDIF*)
                t09put (term, decline.ln, vt_attr);
                END;
            (*ENDIF*)
            t09put (term, hexline.ln, vt_attr);
            t09put (term, chrline.ln, vt_attr);
            END;
        (*ENDIF*)
        IF  pfkey = pf_none
        THEN
            BEGIN
            IF  utds_edit in scan
            THEN
                BEGIN
                t09put (term, c2_line.ln,   vt_attr);
                t09put (term, u2_line.ln,   vt_attr);
                t09put (term, int2_line.ln, vt_attr);
                t09put (term, c4_line.ln,   vt_attr);
                t09put (term, int4_line.ln, vt_attr)
                END;
            (*ENDIF*)
            IF  utds_minbuf in scan
            THEN
                BEGIN
                hexline.ln [2] := c_minbuf_indicator;
                hexline.ln [3] := c_minbuf_indicator
                END
            ELSE
                BEGIN
                t12write_prot (protfile, posline.ln, out_pos-1,err);
                IF  utds_double_numbering in scan
                THEN
                    t12write_prot (protfile, posline_2.ln,
                          out_pos-1, err);
                (*ENDIF*)
                t12write_prot (protfile, decline.ln, out_pos-1,err)
                END;
            (*ENDIF*)
            t12write_prot (protfile, hexline.ln, out_pos-1, err);
            t12write_prot (protfile, chrline.ln, out_pos-1, err);
            IF  utds_edit in scan
            THEN
                BEGIN
                t12write_prot (protfile, c2_line.ln,   out_pos-1, err);
                t12write_prot (protfile, u2_line.ln,   out_pos-1, err);
                t12write_prot (protfile, int2_line.ln, out_pos-1, err);
                t12write_prot (protfile, c4_line.ln,   out_pos-1, err);
                t12write_prot (protfile, int4_line.ln, out_pos-1, err)
                END;
            (*ENDIF*)
            END
        (*ENDIF*)
        END
    (*ENDIF*)
UNTIL
    (buf_pos > upb) OR (pfkey <> pf_none)
(*ENDREPEAT*)
END;

(*------------------------------*)

PROCEDURE
        t14display_msg ;



VAR
      ln : tsp00_Line;

BEGIN
ln := term.blankline;
SAPDB_PascalForcedMove (sizeof (msg), sizeof (ln), @msg, 1, @ln, 1, sizeof (msg));
t09putmsg (term, ln, c_warning, NOT c_immed_displ)
END;

(*------------------------------*)

PROCEDURE
        t14editbuf ;



VAR
      is_ok          : boolean;
      is_quit        : boolean;
      is_cmd         : boolean;
      is_locate_cmd  : boolean;
      pfkeys_used    : boolean;
      indicator      : char;
      scan           : tut_diag_scan;
      c              : char;
      dummy_err      : integer;
      cint4          : tgg90_Cint4;
      i              : tsp00_Int4;
      lwb            : tsp00_Int4;
      upb            : tsp00_Int4;
      type_upb       : tsp00_Int4;
      dummy_pos      : tsp00_Int4;
      dummy_size     : integer;
      bytes_per_line : integer;
      char_per_byte  : integer;
      curr_col       : integer;
      curr_col_len   : integer;
      col_type_pos   : integer;
      lines_per_buf  : integer;
      minbuf_bytes   : integer;
      minbuf_lines   : integer;
      byte_offset    : integer;
      i2             : tsp00_IntMapC2;
      i4             : int_map_c4;
      int_4          : tsp00_Int4;
      new_lwb        : tsp00_Int4;
      bool_value     : tsp00_C6;
      mtype_value    : tsp00_Name;
      in_line        : hexbuf_line;
      node_ptr       : tbd_nodeptr;
      ln             : tsp00_Line;
      ln_len         : integer;
      err            : integer;

BEGIN
lwb       := buf_offset + 1;
is_saved  := false;
is_quit   := false;
t14bufdimension (term, [utds_minbuf],
      minbuf_bytes, dummy_size, minbuf_lines);
ta14edit_bufdimension (term, buf_size, char_per_byte,
      bytes_per_line, dummy_size, lines_per_buf);
pfkeys_used := term.use_pfkeys;
t09pfkeys (term, NOT c_on);
REPEAT
    is_ok := false;
    IF  buf_size > lwb + bytes_per_line - 1
    THEN
        upb := lwb + bytes_per_line - 1
    ELSE
        upb := buf_size;
    (*ENDIF*)
    REPEAT
        is_cmd        := false;
        is_locate_cmd := false;
        t09newscreen_page (term);
        IF  edit_scan <> [ ]
        THEN
            t12write_prot (protfile, term.blankline, 1, dummy_err);
        (*ENDIF*)
        IF  utds_page_header in edit_scan
        THEN
            x07edit_page (term, protfile, file_pos, buf, buf_size, pfkey)
        ELSE
            IF  utds_rec in edit_scan
            THEN
                BEGIN
                node_ptr := @buf;
                IF  (node_ptr^.nd_pt = ptData_egg00)
                    AND
                    ((node_ptr^.nd_pt2 = pt2Tab_egg00       ) OR     (* PTS 1103799 JA 1999-08-30 *)
                    ( node_ptr^.nd_pt2 = pt2Inv_egg00       ) OR
                    ( node_ptr^.nd_pt2 = pt2InvSubtree_egg00))
                THEN
                    BEGIN
                    IF  (pmFdir_egg00 in node_ptr^.nd_pmode) AND (node_ptr^.nd_level = LEAF_LEVEL_BD00)
                    THEN
                        x08fdir_rec_edit (term, protfile, buf, buf_size, buf_offset, c_edit, pfkey)
                    ELSE
                        IF  node_ptr^.nd_level = LEAF_LEVEL_BD00
                        THEN
                            x08rec_edit (term, protfile, buf, buf_size, buf_offset, c_edit,
                                  (pmCatalog_egg00 in node_ptr^.nd_pmode), node_ptr^.nd_pt2, pfkey)
                        ELSE
                            x08tree_index_rec_edit (term, protfile, buf, buf_size, buf_offset,
                                  c_edit, pfkey)
                        (*ENDIF*)
                    (*ENDIF*)
                    END
                ELSE
                    IF  (node_ptr^.nd_pt = ptData_egg00)
                        AND
                        ((node_ptr^.nd_pt2 = pt2Object_egg00      ) OR
                        ( node_ptr^.nd_pt2 = pt2VarObject_egg00   ) OR
                        ( node_ptr^.nd_pt2 = pt2ContObject_egg00  ))
                    THEN
                        x08obj_edit (term, protfile, buf, buf_size, buf_offset, c_edit, pfkey);
                    (*ENDIF*)
                (*ENDIF*)
                IF  (node_ptr^.nd_pt = ptData_egg00)
                    AND
                    (node_ptr^.nd_pt2 = pt2UndoLog_egg00)
                THEN
                    x08beforeimage_edit_display (term, protfile, edit_scan,
                          buf, buf_size, buf_offset, pfkey);
                (*ENDIF*)
                IF  (node_ptr^.nd_pt = ptLog_egg00)
                    OR
                    ((node_ptr^.nd_pt = ptData_egg00)
                    AND
                    (node_ptr^.nd_pt2 = pt2RedoLog_egg00))
                THEN
                    x04afterimagehead_display (term, protfile, edit_scan,
                          buf, buf_size, buf_offset, buf_size-buf_offset,
                          -1, pfkey,
                          ln, ln_len, err);
                (*ENDIF*)
                END;
            (*ENDIF*)
        (*ENDIF*)
        IF  t09is_window_full (term, 1 + c_edit_lines + 1)
        THEN
            BEGIN
            t09holding (term, pfkey);
            IF  pfkey = pf_none
            THEN
                BEGIN
                t09newscreen_page (term);
                IF  utds_page_header in edit_scan
                THEN
                    x07debug_page (term, protfile, c_header_only,
                          file_pos, buf, buf_size, pfkey)
                (*ENDIF*)
                END
            (*ENDIF*)
            END;
        (*ENDIF*)
        IF  pfkey = pf_none
        THEN
            BEGIN
            scan := [utds_display, utds_edit];
            IF  buf_offset > 0
            THEN
                scan := scan + [utds_double_numbering];
            (*ENDIF*)
            t14bufdisplay (term, protfile, scan, bsp_c30,
                  buf, buf_size, lwb, upb, lwb - buf_offset,
                  dummy_pos, pfkey)
            END;
        (*ENDIF*)
        IF  pfkey = pf_none
        THEN
            BEGIN
            t09pfkeys  (term, edit_scan <> [ ]);
            t09setpf1 (term, c_pfkey_help);
            t09getcursor_line (term, in_line.ln, pfkey);
            t09setpf1 (term, c_pfkey_none)
            END;
        (*ENDIF*)
        t09pfkeys (term, NOT c_on);
        t14display_msg (term, bsp_c20);
        IF  (pfkey = pf_help)
            OR
            ((pfkey = pf_none)
            AND ((in_line.prefix  = 'HELP') OR
            (     in_line.prefix  = 'help') OR
            (     in_line.ln[ 1 ] = '?'   ) ))
        THEN
            BEGIN
            pfkey := pf_none;
            t09goto_bottom    (term);
            t09newscreen_page (term);
            ta14help   (term);
            t09holding (term, pfkey)
            END;
        (*ENDIF*)
        IF  ((in_line.prefix = 'TYPE') OR (in_line.prefix = 'type'))
            AND (pfkey = pf_none)
        THEN
            BEGIN
            is_cmd   := true;
            type_upb := lwb - 1 +
                  ((t09unused_window_lines(term) - c_header_line_count)
                  DIV minbuf_lines) * minbuf_bytes;
            IF  type_upb > buf_size
            THEN
                type_upb := buf_size;
            (*ENDIF*)
            t14bufdisplay (term, protfile,
                  [utds_minbuf, utds_display], bsp_c30,
                  buf, buf_size, lwb, type_upb, lwb - buf_offset,
                  dummy_pos, pfkey);
            IF  pfkey = pf_none
            THEN
                t09holding (term, pfkey);
            (*ENDIF*)
            END;
        (*ENDIF*)
        IF  ((in_line.prefix = 'FILL') OR (in_line.prefix = 'fill'))
            AND (pfkey = pf_none)
        THEN
            BEGIN
            is_cmd := true;
            ta14fill (term, protfile, buf, buf_size, buf_offset, lwb,
                  pfkey)
            END;
        (*ENDIF*)
        IF  ((in_line.prefix = 'MOVE') OR (in_line.prefix = 'move'))
            AND (pfkey = pf_none)
        THEN
            BEGIN
            is_cmd := true;
            ta14move (term, protfile, buf, buf_size, buf_offset, lwb,
                  pfkey)
            END;
        (*ENDIF*)
        IF  (in_line.ln = term.blankline) AND (pfkey = pf_none)
        THEN
            is_cmd := true
        (*ENDIF*)
    UNTIL
        NOT is_cmd OR (pfkey <> pf_none);
    (*ENDREPEAT*)
    IF  pfkey = pf_none
    THEN
        BEGIN
        byte_offset := mx_maxbuf_prefix;
        IF  in_line.prefix = c_dec_prefix
        THEN
            BEGIN
            i := lwb;
            REPEAT
                t14int4from_line (term, in_line.ln,
                      byte_offset + char_per_byte - 3, 3,
                      int_4, is_ok);
                IF  is_ok AND (int_4 >= 0) AND (int_4 <= 255)
                THEN
                    buf [i] := chr (int_4);
                (*ENDIF*)
                i := i + 1;
                byte_offset := byte_offset + char_per_byte
            UNTIL
                (i > upb) OR NOT is_ok;
            (*ENDREPEAT*)
            is_ok := true
            END;
        (*ENDIF*)
        IF  in_line.prefix = c_hex_prefix
        THEN
            BEGIN
            t09uppercase_line (in_line.ln, 1, sizeof (in_line.ln));
            i := lwb;
            REPEAT
                ta14hex_from_line (term, in_line.ln,
                      byte_offset + char_per_byte - 2, c, is_ok);
                IF  is_ok
                THEN
                    buf [i] := c;
                (*ENDIF*)
                i := i + 1;
                byte_offset := byte_offset + char_per_byte
            UNTIL
                (i > upb) OR NOT is_ok;
            (*ENDREPEAT*)
            is_ok := true
            END;
        (*ENDIF*)
        IF  in_line.prefix = c_chr_prefix
        THEN
            BEGIN
            i := lwb;
            REPEAT
                c := in_line.ln [byte_offset + char_per_byte - 1];
                IF  c <> ' '
                THEN
                    buf [i] := c;
                (*ENDIF*)
                i := i + 1;
                byte_offset := byte_offset + char_per_byte
            UNTIL
                i > upb;
            (*ENDREPEAT*)
            is_ok := true
            END;
        (*ENDIF*)
        IF  in_line.prefix = c_c2_prefix
        THEN
            BEGIN
            i     := lwb;
            is_ok := true;
            WHILE (i+1 <= upb) AND is_ok DO
                BEGIN
                t14int4from_line (term, in_line.ln, byte_offset + 2,
                      2 * char_per_byte - 2, int_4, is_ok);
                IF  is_ok AND
                    (int_4 >= MIN_UINT2_SP00) AND (int_4 <= MAX_UINT2_SP00)
                THEN
                    BEGIN
                    gg06PnoPut (cint4, int_4);
                    buf [i  ] := cint4.ci4_gg00 [3];
                    buf [i+1] := cint4.ci4_gg00 [4]
                    END;
                (*ENDIF*)
                i := i + 2;
                byte_offset := byte_offset + 2 * char_per_byte
                END;
            (*ENDWHILE*)
            is_ok := true
            END;
        (*ENDIF*)
        IF  in_line.prefix = c_u2_prefix
        THEN
            BEGIN
            i     := lwb;
            is_ok := true;
            WHILE (i+1 <= upb) AND is_ok DO
                BEGIN
                t14int4from_line (term, in_line.ln, byte_offset + 2,
                      2 * char_per_byte - 2, int_4, is_ok);
                IF  is_ok AND
                    (int_4 >= MIN_UINT2_SP00) AND (int_4 <= MAX_UINT2_SP00)
                THEN
                    BEGIN
                    gg06Int4ToUint2 (int_4, i2.mapInt_sp00);
                    buf [i  ] := i2.mapC2_sp00 [1];
                    buf [i+1] := i2.mapC2_sp00 [2]
                    END;
                (*ENDIF*)
                i := i + 2;
                byte_offset := byte_offset + 2 * char_per_byte
                END;
            (*ENDWHILE*)
            is_ok := true
            END;
        (*ENDIF*)
        IF  in_line.prefix = c_int2_prefix
        THEN
            BEGIN
            i     := lwb;
            is_ok := true;
            WHILE (i+1 <= upb) AND is_ok DO
                BEGIN
                t14int4from_line (term, in_line.ln, byte_offset + 2,
                      2 * char_per_byte - 2, int_4, is_ok);
                IF  is_ok AND
                    (int_4 >= -MAX_INT2_SP00) AND (int_4 <= MAX_INT2_SP00)
                THEN
                    BEGIN
                    i2.mapInt_sp00 := int_4;
                    buf [i  ]  := i2.mapC2_sp00 [1];
                    buf [i+1]  := i2.mapC2_sp00 [2]
                    END;
                (*ENDIF*)
                i := i + 2;
                byte_offset := byte_offset + 2 * char_per_byte
                END;
            (*ENDWHILE*)
            is_ok := true
            END;
        (*ENDIF*)
        IF  in_line.prefix = c_c4_prefix
        THEN
            BEGIN
            i     := lwb;
            is_ok := true;
            WHILE (i+3 <= upb) AND is_ok DO
                BEGIN
                IF  ta14is_nil_pno (in_line.ln, byte_offset+1)
                THEN
                    i4.map_i4 := NIL_PAGE_NO_GG00
                ELSE
                    t14int4from_line (term, in_line.ln, byte_offset + 2,
                          4 * char_per_byte - 2, i4.map_i4, is_ok);
                (*ENDIF*)
                IF  is_ok
                THEN
                    BEGIN
                    gg06PnoPut (cint4, int_4);
                    buf [i  ] := cint4.ci4_gg00 [1];
                    buf [i+1] := cint4.ci4_gg00 [2];
                    buf [i+2] := cint4.ci4_gg00 [3];
                    buf [i+3] := cint4.ci4_gg00 [4]
                    END;
                (*ENDIF*)
                i := i + 4;
                byte_offset := byte_offset + 4 * char_per_byte
                END;
            (*ENDWHILE*)
            is_ok := true
            END;
        (*ENDIF*)
        IF  in_line.prefix = c_int4_prefix
        THEN
            BEGIN
            i     := lwb;
            is_ok := true;
            WHILE (i+3 <= upb) AND is_ok DO
                BEGIN
                IF  ta14is_nil_pno (in_line.ln, byte_offset+1)
                THEN
                    i4.map_i4 := NIL_PAGE_NO_GG00
                ELSE
                    t14int4from_line (term, in_line.ln, byte_offset + 2,
                          4 * char_per_byte - 2, i4.map_i4, is_ok);
                (*ENDIF*)
                IF  is_ok
                THEN
                    BEGIN
                    buf [i  ] := i4.map_c4 [1];
                    buf [i+1] := i4.map_c4 [2];
                    buf [i+2] := i4.map_c4 [3];
                    buf [i+3] := i4.map_c4 [4]
                    END;
                (*ENDIF*)
                i := i + 4;
                byte_offset := byte_offset + 4 * char_per_byte
                END;
            (*ENDWHILE*)
            is_ok := true
            END;
        (*ENDIF*)
        IF  NOT is_ok
        THEN
            BEGIN
            t09uppercase_line (in_line.ln, 1, sizeof (in_line.ln));
            IF  in_line.prefix = 'SAVE'
            THEN
                is_saved := true
            ELSE
                IF  in_line.prefix = 'QUIT'
                THEN
                    BEGIN
                    is_quit := true;
                    pfkey   := pf_end
                    END
                ELSE
                    IF  in_line.prefix = 'RR  '
                    THEN
                        BEGIN
                        IF  buf_size - PAGE_TRAILER_MXGG00 + 1 > 0
                        THEN
                            lwb := buf_size - PAGE_TRAILER_MXGG00 + 1
                        (*ENDIF*)
                        END
                    ELSE
                        IF  in_line.ln [1] = 'R'
                        THEN
                            BEGIN
                            IF  lwb + (bytes_per_line DIV 4) * 4
                                <= buf_size
                            THEN
                                lwb := lwb + (bytes_per_line DIV 4) * 4
                            (*ENDIF*)
                            END
                        ELSE
                            IF  in_line.ln [1] = 'L'
                            THEN
                                BEGIN
                                IF  lwb - (bytes_per_line DIV 4) * 4
                                    >= buf_offset + 1
                                THEN
                                    lwb := lwb -(bytes_per_line DIV 4)*4
                                ELSE
                                    lwb := buf_offset + 1
                                (*ENDIF*)
                                END
                            ELSE
                                BEGIN
                                t14int4from_line (term, in_line.ln,
                                      1, of_col_type, new_lwb, is_ok);
                                IF  is_ok
                                THEN
                                    BEGIN
                                    new_lwb := new_lwb + buf_offset;
                                    IF  (new_lwb >= buf_offset + 1) AND
                                        (new_lwb <= buf_size      )
                                    THEN
                                        BEGIN
                                        lwb           := new_lwb;
                                        is_locate_cmd := true
                                        END;
                                    (*ENDIF*)
                                    END
                                (*ENDIF*)
                                END
                            (*ENDIF*)
                        (*ENDIF*)
                    (*ENDIF*)
                (*ENDIF*)
            (*ENDIF*)
            END
        (*ENDIF*)
        END;
    (*ENDIF*)
    IF  (pfkey = pf_none) AND is_locate_cmd
    THEN
        (* handle VXT08 input *)
        BEGIN
        curr_col     := 1;
        curr_col_len := 0;
        i            := lwb;
        is_ok        := true;
        REPEAT
            IF  curr_col = 1
            THEN
                BEGIN
                col_type_pos := of_col_type + 1;
                byte_offset  := of_first_col
                END
            ELSE
                BEGIN
                col_type_pos := of_col_type + 3;
                byte_offset  := of_second_col
                END;
            (*ENDIF*)
            IF  (in_line.ln [col_type_pos+1] >= '0') AND
                (in_line.ln [col_type_pos+1] <= '9')
            THEN
                curr_col_len :=
                      ord (in_line.ln [col_type_pos+1]) - ord ('0')
            ELSE
                is_ok := false;
            (*ENDIF*)
            IF  is_ok
            THEN
                BEGIN
                indicator := in_line.ln [col_type_pos];
                IF  (indicator = c_int_indicator)
                    AND
                    ((curr_col_len = 1) OR
                    ( curr_col_len = 2) OR
                    ( curr_col_len = 4))
                THEN
                    BEGIN
                    IF  ta14is_nil_pno (in_line.ln, byte_offset)
                    THEN
                        i4.map_i4 := NIL_PAGE_NO_GG00
                    ELSE
                        t14int4from_line (term, in_line.ln,
                              byte_offset + 1,
                              mx_int4_displ, i4.map_i4, is_ok);
                    (*ENDIF*)
                    IF  is_ok AND (curr_col_len = 1)
                    THEN
                        BEGIN
                        IF  (i4.map_i4 >=   0) AND
                            (i4.map_i4 <= 255)
                        THEN
                            buf [i] := chr (i4.map_i4)
                        ELSE
                            is_ok := false
                        (*ENDIF*)
                        END
                    ELSE
                        IF  is_ok AND (curr_col_len = 2)
                        THEN
                            BEGIN
                            IF  (i4.map_i4 >= MIN_INT2_SP00) AND
                                (i4.map_i4 <= MAX_INT2_SP00)
                            THEN
                                BEGIN
                                i2.mapInt_sp00 := i4.map_i4;
                                buf [i  ]  := i2.mapC2_sp00 [1];
                                buf [i+1]  := i2.mapC2_sp00 [2]
                                END
                            (*ENDIF*)
                            END
                        ELSE
                            IF  is_ok AND (curr_col_len = 4)
                            THEN
                                BEGIN
                                buf [i  ] := i4.map_c4 [1];
                                buf [i+1] := i4.map_c4 [2];
                                buf [i+2] := i4.map_c4 [3];
                                buf [i+3] := i4.map_c4 [4]
                                END;
                            (*ENDIF*)
                        (*ENDIF*)
                    (*ENDIF*)
                    is_ok := true
                    END
                ELSE
                    IF  (indicator = c_bool_indicator)
                        AND
                        (curr_col_len = 1)
                    THEN
                        BEGIN
                        SAPDB_PascalForcedMove (sizeof(in_line.ln), sizeof (bool_value),
                              @in_line.ln, byte_offset+1, @bool_value, 1,
                              sizeof (bool_value));
                        IF  bool_value = 'TRUE  '
                        THEN
                            buf [i] := chr (ord (true))
                        ELSE
                            IF  bool_value = 'FALSE '
                            THEN
                                buf [i] := chr (ord (false))
                            (*ENDIF*)
                        (*ENDIF*)
                        END
                    ELSE
                        IF  ((indicator = c_mtype_indicator ) OR
                            ( indicator = c_mtype2_indicator))
                            AND
                            (curr_col_len = 1)
                        THEN
                            BEGIN
                            SAPDB_PascalForcedMove (sizeof (in_line.ln),
                                  sizeof (mtype_value),
                                  @in_line.ln, byte_offset+1,
                                  @mtype_value, 1,
                                  sizeof (mtype_value));
                            IF  indicator = c_mtype_indicator
                            THEN
                                ta14mtype_assign  (mtype_value, buf, i)
                            ELSE
                                ta14mtype2_assign (mtype_value, buf, i)
                            (*ENDIF*)
                            END
                        ELSE
                            IF  (indicator = c_set_indicator)
                                AND
                                (curr_col     = 1) AND
                                (curr_col_len = 1)
                            THEN
                                ta14set_assign (in_line.ln,
                                      byte_offset, buf, i);
                            (*ENDIF*)
                        (*ENDIF*)
                    (*ENDIF*)
                (*ENDIF*)
                i := i + curr_col_len
                END;
            (*ENDIF*)
            curr_col := curr_col + 1
        UNTIL
            (curr_col > 2) OR NOT is_ok
        (*ENDREPEAT*)
        END;
    (*ENDIF*)
    IF  (pfkey = pf_end) AND NOT is_saved AND NOT is_quit
    THEN
        BEGIN
        pfkey := pf_none;
        ta14display_leave_msg (term);
        t09holding (term, pfkey)
        END
    (*ENDIF*)
UNTIL
    is_saved OR (pfkey <> pf_none);
(*ENDREPEAT*)
t09pfkeys (term, pfkeys_used)
END;

(*------------------------------*)

PROCEDURE
        t14int4from_line ;



VAR
      intstring : PACKED ARRAY [1..c_int4_digits] OF char;
      factor    : tsp00_Int4;
      intlength : integer;
      i, pos    : integer;
      startpos  : integer;
      msg       : tsp00_C20;

BEGIN
ok        := true;
intlength := 0;
startpos  := ln_pos;
WHILE (startpos < ln_pos + ln_len - 1) AND (ln [startpos] = ' ') DO
    startpos := startpos + 1;
(*ENDWHILE*)
IF  ln [startpos] = '-'
THEN
    pos := startpos + 1
ELSE
    pos := startpos;
(*ENDIF*)
WHILE (pos < ln_pos + ln_len) AND
      (ln [pos] >= '0') AND (ln [pos] <= '9') DO
    BEGIN
    intlength := intlength + 1;
    pos := pos + 1
    END;
(*ENDWHILE*)
IF  (intlength = 0) OR (ln [pos] <> ' ')
THEN
    BEGIN
    ok       := false;
    msg      := 'INVALID NUMBER ('' '')';
    msg [18] := ln [pos];
    t14display_msg (term, msg)
    END;
(*ENDIF*)
IF  ok AND (intlength > c_int4_digits)
THEN
    BEGIN
    ok := false;
    t14display_msg (term, 'NUMBER TOO LONG     ')
    END;
(*ENDIF*)
IF  ok AND (intlength = c_int4_digits)
THEN
    BEGIN
    FOR i := 1 TO c_int4_digits DO
        intstring [i] := ln [pos - c_int4_digits - 1 + i];
    (*ENDFOR*)
    IF  intstring > c_maxint4_string
    THEN
        BEGIN
        ok := false;
        t14display_msg (term, 'NUMBER TOO LARGE    ')
        END
    (*ENDIF*)
    END;
(*ENDIF*)
IF  ok
THEN
    BEGIN
    pos    := pos - 1;
    int    := 0;
    factor := 1;
    FOR i := 1 TO intlength DO
        BEGIN
        int := int + factor * (ord (ln [pos]) - ord ('0'));
        pos := pos - 1;
        IF  i < intlength
        THEN
            factor := factor * 10
        (*ENDIF*)
        END;
    (*ENDFOR*)
    IF  ln [startpos] = '-'
    THEN
        int := - int
    (*ENDIF*)
    END
(*ENDIF*)
END;

(*------------------------------*)

PROCEDURE
        t14logentry_edit ;



VAR
      pfkeys_used : boolean;
      node_ptr    : tbd_nodeptr;

BEGIN
is_saved    := false;
pfkey       := pf_none;
pfkeys_used := term.use_pfkeys;
node_ptr    := @node;
t14editbuf (term, protfile, node,
      entry_pos - 1 + entry_length,
      entry_pos - 1,
      file_pos, [utds_rec], is_saved, pfkey);
IF  pfkey = pf_end
THEN
    pfkey := pf_none;
(*ENDIF*)
t09pfkeys (term, pfkeys_used)
END;

(*------------------------------*)

PROCEDURE
        t14object_edit ;



VAR
      pfkeys_used : boolean;
      node_ptr    : tbd_nodeptr;

BEGIN
is_saved    := false;
pfkey       := pf_none;
pfkeys_used := term.use_pfkeys;
node_ptr    := @node;
t14editbuf (term, protfile, node,
      obj_pos - 1 + gg06Int4FromUint2 (node_ptr^.nd_obj_frame_len),
      obj_pos - 1,
      file_pos, [utds_rec], is_saved, pfkey);
IF  pfkey = pf_end
THEN
    pfkey := pf_none;
(*ENDIF*)
t09pfkeys (term, pfkeys_used)
END;

(*------------------------------*)

PROCEDURE
        t14record_edit ;



VAR
      is_ok      : boolean;
      curr_saved : boolean;
      pfkeys_used: boolean;
      dummy_err  : integer;
      rec_pos    : tsp00_Int4;
      rec_len    : tsp00_Int4;
      i2         : tsp00_IntMapC2;
      in_line    : tsp00_Line;

BEGIN
is_saved    := false;
pfkey       := pf_none;
pfkeys_used := term.use_pfkeys; (* PTS 1000215 JA 1997-11-14 *)
REPEAT
    rec_pos    := 0;
    rec_len    := 0;
    is_ok      := false;
    in_line    := term.blankline;
    curr_saved := false;
    t09newscreen_page (term);
    t12write_prot (protfile, term.blankline, 1, dummy_err);
    x07keys_display (term, protfile, file_pos, node, node_size, pfkey);
    IF  pfkey = pf_none
    THEN
        BEGIN
        IF  t09is_window_full (term, 1)
        THEN
            BEGIN
            t09holding (term, pfkey);
            IF  pfkey = pf_none
            THEN
                t09newscreen_page (term)
            (*ENDIF*)
            END
        (*ENDIF*)
        END;
    (*ENDIF*)
    IF  pfkey = pf_none
    THEN
        BEGIN
        t09pfkeys (term, c_on);
        t09getcursor_line (term, in_line, pfkey);
        t09pfkeys (term, NOT c_on);
        END;
    (*ENDIF*)
    IF  (pfkey = pf_none)
        AND
        (in_line [of_key_pos-3] = 'p') AND
        (in_line [of_key_pos-2] = 'o') AND
        (in_line [of_key_pos-1] = 's')
        AND
        (in_line [of_key_pos + mx_pos_displ + 1] = ')')
    THEN
        BEGIN
        in_line [of_key_pos + mx_pos_displ + 1] := ' ';
        t14int4from_line (term, in_line, of_key_pos + 1,
              mx_pos_displ, rec_pos, is_ok);
        IF  is_ok
        THEN
            is_ok := (rec_pos >= 1) AND (rec_pos + 3 <= node_size)
        (*ENDIF*)
        END;
    (*ENDIF*)
    IF  is_ok AND (pfkey = pf_none)
    THEN
        BEGIN
        i2.mapC2_sp00 [1] := node [rec_pos  ];
        i2.mapC2_sp00 [2] := node [rec_pos+1];
        rec_len       := i2.mapInt_sp00;
        is_ok         := rec_pos - 1 + rec_len <= node_size
        END;
    (*ENDIF*)
    IF  is_ok AND (pfkey = pf_none)
    THEN
        BEGIN
        t14editbuf (term, protfile, node, rec_pos - 1 + rec_len,
              rec_pos - 1, file_pos, edit_scan, curr_saved, pfkey);
        IF  pfkey = pf_end
        THEN
            pfkey := pf_none;
        (*ENDIF*)
        IF  curr_saved
        THEN
            is_saved := true
        (*ENDIF*)
        END
    (*ENDIF*)
UNTIL
    NOT is_ok OR (pfkey <> pf_none);
(*ENDREPEAT*)
t09pfkeys (term, pfkeys_used) (* PTS 1000215 JA 1997-11-14 *)
END;

(*------------------------------*)

PROCEDURE
        ta14display_leave_msg ;



BEGIN
t09put   (term, term.blankline, cut_protected);
t09put60 (term,
      'If you want to leave the EDIT mode, restrike the END key    ',
      cut_bright_protected)
END;

(*------------------------------*)

PROCEDURE
        ta14edit_bufdimension ;



BEGIN
IF  buf_size >= 10000
THEN
    char_per_byte := 6
ELSE
    char_per_byte := 5;
(*ENDIF*)
bytes_per_line :=
      (t09window_cols (term) - mx_maxbuf_prefix) DIV char_per_byte;
line_size     := mx_maxbuf_prefix + bytes_per_line * char_per_byte;
lines_per_buf := c_edit_lines
END;

(*------------------------------*)

PROCEDURE
        ta14fill ;



CONST
      c_margin_bytes = 5;

VAR
      is_ok        : boolean;
      fill_char    : char;
      ln_pos       : integer;
      ln_len       : integer;
      count        : integer;
      maxbuf_bytes : integer;
      dummy_int_1  : integer;
      dummy_int_2  : integer;
      i            : tsp00_Int4;
      lwb          : tsp00_Int4;
      upb          : tsp00_Int4;
      dummy_pos    : tsp00_Int4;
      fill_len     : tsp00_Int4;
      fill_pos     : tsp00_Int4;
      n            : tsp00_Name;
      n_endpos     : tsp00_Name;
      ln           : tsp00_Line;

BEGIN
count    := 0;
is_ok    := true;
fill_pos := init_pos;
t14bufdimension (term, [ ], maxbuf_bytes, dummy_int_1, dummy_int_2);
REPEAT
    count := count + 1;
    t09newscreen_page (term);
    lwb := init_pos;
    IF  lwb + maxbuf_bytes - 1 > buf_size
    THEN
        upb := buf_size
    ELSE
        upb := lwb + maxbuf_bytes - 1;
    (*ENDIF*)
    t14bufdisplay (term, protfile, [utds_display], bsp_c30,
          buf, buf_size, lwb, upb, lwb - buf_offset, dummy_pos, pfkey);
    t09put (term, term.blankline, cut_protected);
    ln     := term.blankline;
    ln_len := 0;
    g17trimint4_to_line (init_pos - buf_offset, ln_len, ln);
    n := bsp_name;
    FOR i := 1 TO sizeof (n) DO
        n [i] := ln [i];
    (*ENDFOR*)
    t09get1name (term, '                POS:', cut_bright_protected,
          cut_unprotected, mx_int4_displ, c_upper, n, pfkey);
    t09putmsg (term, term.blankline, NOT c_warning, NOT c_immed_displ);
    IF  pfkey = pf_none
    THEN
        BEGIN
        SAPDB_PascalForcedMove (sizeof (n), sizeof (ln), @n, 1, @ln, 1, sizeof (n));
        t14int4from_line (term, ln, 1, sizeof (n), fill_pos, is_ok);
        IF  is_ok
        THEN
            BEGIN
            fill_pos := fill_pos + buf_offset;
            IF  (fill_pos <= buf_offset) OR (fill_pos > buf_size)
            THEN
                BEGIN
                is_ok := false;
                t14display_msg (term, msg_illegal_pos)
                END
            (*ENDIF*)
            END
        (*ENDIF*)
        END
    (*ENDIF*)
UNTIL
    is_ok OR (pfkey <> pf_none) OR (count >= 3);
(*ENDREPEAT*)
IF  (pfkey = pf_none) AND NOT is_ok
THEN
    pfkey := pf_end;
(*ENDIF*)
IF  pfkey = pf_none
THEN
    BEGIN
    IF  fill_pos - c_margin_bytes > buf_offset
    THEN
        lwb := fill_pos - c_margin_bytes
    ELSE
        lwb := buf_offset + 1;
    (*ENDIF*)
    upb := lwb + maxbuf_bytes - 1;
    IF  upb > buf_size
    THEN
        upb := buf_size
    (*ENDIF*)
    END;
(*ENDIF*)
count := 0;
is_ok := false;
WHILE NOT is_ok AND (pfkey = pf_none) AND (count < 3) DO
    BEGIN
    count := count + 1;
    t09newscreen_page (term);
    t14bufdisplay (term, protfile, [utds_display], bsp_c30,
          buf, buf_size, lwb, upb, lwb - buf_offset, dummy_pos, pfkey);
    ln     := term.blankline;
    ln_pos := mx_maxbuf_prefix + 1 + mx_maxbuf_byte * (fill_pos - lwb);
    ln [ln_pos  ] := '|';
    ln [ln_pos+1] := '=';
    ln [ln_pos+2] := '>';
    t09put (term, ln, cut_bright_protected);
    t09put (term, term.blankline, cut_protected);
    n        := bsp_name;
    n_endpos := bsp_name;
    t09get2name (term, '        FILL LENGTH:', 'or      FILL ENDPOS:',
          cut_bright_protected, cut_bright_protected, cut_unprotected,
          cut_unprotected, mx_int4_displ, mx_int4_displ,
          c_upper, c_upper, n, n_endpos, pfkey);
    t09putmsg (term, term.blankline, NOT c_warning, NOT c_immed_displ);
    IF  pfkey = pf_none
    THEN
        BEGIN
        ln := term.blankline;
        IF  n <> bsp_name
        THEN
            SAPDB_PascalForcedMove (sizeof (n), sizeof (ln),
                  @n, 1, @ln, 1, sizeof (n))
        ELSE
            SAPDB_PascalForcedMove (sizeof (n_endpos), sizeof (ln),
                  @n_endpos, 1, @ln, 1, sizeof (n_endpos));
        (*ENDIF*)
        t14int4from_line (term, ln, 1, sizeof (n), fill_len, is_ok);
        IF  is_ok
        THEN
            BEGIN
            IF  n <> bsp_name
            THEN
                (* length specified *)
                BEGIN
                IF  (fill_len < 1) OR
                    (fill_pos + fill_len - 1 > buf_size)
                THEN
                    BEGIN
                    t14display_msg (term, msg_illegal_len);
                    is_ok := false
                    END
                (*ENDIF*)
                END
            ELSE
                (* endpos specified *)
                BEGIN
                fill_len := fill_len + buf_offset;
                IF  (fill_len < fill_pos) OR (fill_len > buf_size)
                THEN
                    BEGIN
                    t14display_msg (term, msg_illegal_pos);
                    is_ok := false
                    END
                ELSE
                    fill_len := fill_len - fill_pos + 1
                (*ENDIF*)
                END
            (*ENDIF*)
            END
        (*ENDIF*)
        END
    (*ENDIF*)
    END;
(*ENDWHILE*)
IF  (pfkey = pf_none) AND NOT is_ok
THEN
    pfkey := pf_end;
(*ENDIF*)
IF  pfkey = pf_none
THEN
    BEGIN
    t09newscreen_page (term);
    t14bufdisplay (term, protfile, [utds_display], bsp_c30,
          buf, buf_size, lwb, upb, lwb - buf_offset, dummy_pos, pfkey);
    ln := term.blankline;
    ln [ln_pos] := '|';
    IF  fill_pos + fill_len - 1 > upb
    THEN
        BEGIN
        ln_len := mx_maxbuf_prefix + mx_maxbuf_byte * (upb - lwb + 1);
        ln [ln_len] := '>'
        END
    ELSE
        BEGIN
        ln_len := mx_maxbuf_prefix
              + mx_maxbuf_byte * (fill_pos - lwb + fill_len);
        ln [ln_len] := '|'
        END;
    (*ENDIF*)
    ln_pos := ln_pos + 1;
    WHILE ln_pos < ln_len DO
        BEGIN
        ln [ln_pos] := '=';
        ln_pos      := ln_pos + 1
        END;
    (*ENDWHILE*)
    t09put (term, ln, cut_bright_protected);
    t09put (term, term.blankline, cut_protected);
    IF  fill_pos + fill_len - 1 > upb
    THEN
        BEGIN
        IF  fill_pos + fill_len - 1 > upb + maxbuf_bytes
        THEN
            BEGIN
            upb := fill_pos + fill_len - 1;
            IF  upb + c_margin_bytes <= buf_size
            THEN
                upb := upb + c_margin_bytes
            ELSE
                upb := buf_size;
            (*ENDIF*)
            lwb := upb - maxbuf_bytes + 1
            END
        ELSE
            BEGIN
            lwb := upb + 1;
            upb := upb + maxbuf_bytes;
            IF  upb > buf_size
            THEN
                upb := buf_size
            (*ENDIF*)
            END;
        (*ENDIF*)
        t14bufdisplay (term, protfile, [utds_display], bsp_c30,
              buf, buf_size, lwb, upb, lwb - buf_offset,
              dummy_pos, pfkey);
        ln     := term.blankline;
        ln_pos := mx_maxbuf_prefix + 1;
        ln_len := mx_maxbuf_prefix +
              mx_maxbuf_byte * (fill_pos - lwb + fill_len);
        ln [ln_len] := '|';
        WHILE ln_pos < ln_len DO
            BEGIN
            ln [ln_pos] := '=';
            ln_pos      := ln_pos + 1
            END;
        (*ENDWHILE*)
        t09put (term, ln, cut_bright_protected);
        t09put (term, term.blankline, cut_protected)
        END;
    (*ENDIF*)
    n := bsp_name;
    t09putmsg (term, term.blankline, NOT c_warning, NOT c_immed_displ);
    t09get1name (term, '    fill char / hex:', cut_bright_protected,
          cut_unprotected, 2, NOT c_upper, n, pfkey);
    t09putmsg (term, term.blankline, NOT c_warning, NOT c_immed_displ)
    END;
(*ENDIF*)
IF  pfkey = pf_none
THEN
    BEGIN
    IF  n [2] = ' '
    THEN
        fill_char := n [1]
    ELSE
        BEGIN
        ln := term.blankline;
        SAPDB_PascalForcedMove (sizeof (n), sizeof (ln), @n, 1, @ln, 1, sizeof (n));
        t09uppercase_line (ln, 1, 3);
        IF  ln [1] = 'X'
        THEN
            i := 2
        ELSE
            i := 1;
        (*ENDIF*)
        ta14hex_from_line (term, ln, i, fill_char, is_ok);
        IF  NOT is_ok
        THEN
            pfkey := pf_end
        (*ENDIF*)
        END
    (*ENDIF*)
    END;
(*ENDIF*)
IF  pfkey = pf_none
THEN
    BEGIN
    ln     := term.blankline;
    ln_len := 0;
    g17sname_to_line ('FILL from   ', ln_len, ln);
    ln_len := ln_len + 1;
    g17trimint4_to_line (fill_pos - buf_offset, ln_len, ln);
    g17sname_to_line (' to         ', ln_len, ln);
    ln_len := ln_len + 1;
    g17trimint4_to_line (fill_pos + fill_len - 1 - buf_offset,
          ln_len, ln);
    g17sname_to_line (' (length    ', ln_len, ln);
    ln_len := ln_len + 1;
    g17trimint4_to_line (fill_len, ln_len, ln);
    ln_len      := ln_len + 1;
    ln [ln_len] := ')';
    t09put (term, term.blankline, cut_protected);
    t09put (term, ln, cut_bright_protected);
    t09put (term, term.blankline, cut_protected);
    n := 'y                 ';
    t09get1name (term, 'EXECUTE FILL ? (y/n)', cut_bright_protected,
          cut_unprotected, mx_yes_no, c_upper, n, pfkey)
    END;
(*ENDIF*)
IF  pfkey = pf_none
THEN
    BEGIN
    IF  n = 'Y                 '
    THEN
        FOR i := fill_pos TO fill_pos + fill_len - 1 DO
            buf [i] := fill_char
        (*ENDFOR*)
    (*ENDIF*)
    END;
(*ENDIF*)
IF  pfkey = pf_end
THEN
    pfkey := pf_none
(*ENDIF*)
END;

(*------------------------------*)

PROCEDURE
        ta14help ;



BEGIN
t09put   (term, term.blankline, cut_protected);
t09put60 (term,
      '<pos>  goto the specified position                          ',
      cut_protected);
t09put60 (term,
      'l      goto left                                            ',
      cut_protected);
t09put60 (term,
      'r      goto right                                           ',
      cut_protected);
t09put60 (term,
      'rr     goto the end of the page                             ',
      cut_protected);
t09put   (term, term.blankline, cut_protected);
t09put60 (term,
      'fill                                                        ',
      cut_protected);
t09put60 (term,
      'move                                                        ',
      cut_protected);
t09put   (term, term.blankline, cut_protected);
t09put60 (term,
      'type   displays the buffer (starting at edit position)      ',
      cut_protected);
t09put   (term, term.blankline, cut_protected);
t09put60 (term,
      'quit   leaves the EDIT mode without any changes             ',
      cut_protected);
t09put60 (term,
      'save   leaves the EDIT mode                                 ',
      cut_protected)
END;

(*------------------------------*)

PROCEDURE
        ta14hex_from_line ;



VAR
      c          : char;
      i          : integer;
      int_result : integer;
      msg        : tsp00_C20;

BEGIN
ok := true;
i  := 0;
int_result := 0;
REPEAT
    c := ln [ln_pos+i];
    IF  (c >= '0') AND (c <= '9')
    THEN
        int_result := int_result + ord(c) - ord('0')
    ELSE
        IF  (c >= 'A') AND (c <= 'F')
        THEN
            int_result := int_result + ord(c) - ord('A') + 10
        ELSE
            BEGIN
            ok  := false;
            msg := 'INVALID HEX ('' '')   ';
            msg [15] := c;
            t14display_msg (term, msg)
            END;
        (*ENDIF*)
    (*ENDIF*)
    IF  i = 0
    THEN
        int_result := int_result * 16;
    (*ENDIF*)
    i := i + 1;
UNTIL
    (i > 1) OR NOT ok;
(*ENDREPEAT*)
IF  ok
THEN
    result := chr(int_result)
(*ENDIF*)
END;

(*------------------------------*)

FUNCTION
        ta14is_nil_pno ;



CONST
      c_nil_pno1 = 'nil_pno ';
      c_nil_pno2 = 'NIL_PNO ';

VAR
      is_nil_pno : boolean;
      offset     : integer;
      i          : integer;
      n          : tsp00_C8;

BEGIN
is_nil_pno := false;
offset     := ln_offset;
IF  offset + 1 < sizeof (ln)
THEN
    BEGIN
    WHILE (ln [offset+1] = ' ') AND (offset + 1 < sizeof (ln)) DO
        offset := offset + 1;
    (*ENDWHILE*)
    IF  offset + sizeof (n) <= sizeof (ln)
    THEN
        BEGIN
        FOR i := 1 TO sizeof (n) DO
            n [i] := ln [offset+i];
        (*ENDFOR*)
        is_nil_pno :=
              (n = c_nil_pno1) OR
              (n = c_nil_pno2)
        END
    (*ENDIF*)
    END;
(*ENDIF*)
ta14is_nil_pno := is_nil_pno
END;

(*------------------------------*)

PROCEDURE
        ta14move ;



CONST
      msg_destin = 'DESTIN:                       ';

VAR
      is_ok        : boolean;
      scan         : tut_diag_scan;
      source_pos   : tsp00_Int4;
      destin_pos   : tsp00_Int4;
      move_len     : tsp00_Int4;
      source_upb   : tsp00_Int4;
      destin_lwb   : tsp00_Int4;
      destin_upb   : tsp00_Int4;
      dummy_pos    : tsp00_Int4;
      count        : tsp00_Int4;
      i            : tsp00_Int4;
      maxbuf_bytes : integer;
      minbuf_bytes : integer;
      dummy_int_1  : integer;
      dummy_int_2  : integer;
      ln_len       : integer;
      offset       : integer;
      n            : tsp00_Name;
      n_endpos     : tsp00_Name;
      msg_source   : tsp00_C30;
      ln           : tsp00_Line;

BEGIN
msg_source := 'SOURCE:                       ';
count      := 0;
is_ok      := true;
t14bufdimension (term, [ ], maxbuf_bytes, dummy_int_1, dummy_int_2);
t14bufdimension (term, [utds_minbuf], minbuf_bytes,
      dummy_int_1, dummy_int_2);
REPEAT
    count := count + 1;
    t09newscreen_page (term);
    IF  buf_offset + maxbuf_bytes > buf_size
    THEN
        source_upb := buf_size
    ELSE
        source_upb := buf_offset + maxbuf_bytes;
    (*ENDIF*)
    t14bufdisplay (term, protfile, [utds_display], msg_source,
          buf, buf_size, init_pos, source_upb, init_pos - buf_offset,
          dummy_pos, pfkey);
    t09put (term, term.blankline, cut_protected);
    ln     := term.blankline;
    ln_len := 0;
    g17trimint4_to_line (init_pos - buf_offset, ln_len, ln);
    n := bsp_name;
    FOR i := 1 TO sizeof (n) DO
        n [i] := ln [i];
    (*ENDFOR*)
    t09get1name (term, '         SOURCE POS:', cut_bright_protected,
          cut_unprotected, mx_int4_displ, c_upper, n, pfkey);
    t09putmsg (term, term.blankline, NOT c_warning, NOT c_immed_displ);
    IF  pfkey = pf_none
    THEN
        BEGIN
        SAPDB_PascalForcedMove (sizeof (n), sizeof (ln), @n, 1, @ln, 1, sizeof (n));
        t14int4from_line (term, ln, 1, sizeof (n), source_pos, is_ok);
        IF  is_ok
        THEN
            BEGIN
            source_pos := source_pos + buf_offset;
            IF  (source_pos <= buf_offset) OR (source_pos > buf_size)
            THEN
                BEGIN
                is_ok := false;
                t14display_msg (term, msg_illegal_pos)
                END
            (*ENDIF*)
            END
        (*ENDIF*)
        END
    (*ENDIF*)
UNTIL
    is_ok OR (pfkey <> pf_none) OR (count >= 3);
(*ENDREPEAT*)
IF  (pfkey = pf_none) AND NOT is_ok
THEN
    pfkey := pf_end;
(*ENDIF*)
IF  pfkey = pf_none
THEN
    BEGIN
    IF  source_pos + maxbuf_bytes - 1 <= buf_size
    THEN
        source_upb := source_pos + maxbuf_bytes - 1
    ELSE
        source_upb := buf_size
    (*ENDIF*)
    END;
(*ENDIF*)
count := 0;
is_ok := false;
WHILE NOT is_ok AND (pfkey = pf_none) AND (count < 3) DO
    BEGIN
    count := count + 1;
    t09newscreen_page (term);
    t14bufdisplay (term, protfile, [utds_display], msg_source,
          buf, buf_size, source_pos, source_upb,
          source_pos - buf_offset, dummy_pos, pfkey);
    t09put (term, term.blankline, cut_protected);
    IF  buf_offset + maxbuf_bytes > buf_size
    THEN
        destin_upb := buf_size
    ELSE
        destin_upb := buf_offset + maxbuf_bytes;
    (*ENDIF*)
    t14bufdisplay (term, protfile, [utds_display], msg_destin,
          buf, buf_size, buf_offset+1, destin_upb, 1, dummy_pos, pfkey);
    t09put (term, term.blankline, cut_protected);
    n := '1                 ';
    t09get1name (term, '         DESTIN POS:', cut_bright_protected,
          cut_unprotected, mx_int4_displ, c_upper, n, pfkey);
    t09putmsg (term, term.blankline, NOT c_warning, NOT c_immed_displ);
    IF  pfkey = pf_none
    THEN
        BEGIN
        SAPDB_PascalForcedMove (sizeof (n), sizeof (ln), @n, 1, @ln, 1, sizeof (n));
        t14int4from_line (term, ln, 1, sizeof (n), destin_pos, is_ok);
        IF  is_ok
        THEN
            BEGIN
            destin_pos := destin_pos + buf_offset;
            IF  (destin_pos <= buf_offset) OR (destin_pos > buf_size)
            THEN
                BEGIN
                is_ok := false;
                t14display_msg (term, msg_illegal_pos)
                END
            (*ENDIF*)
            END
        (*ENDIF*)
        END
    (*ENDIF*)
    END;
(*ENDWHILE*)
IF  (pfkey = pf_none) AND NOT is_ok
THEN
    pfkey := pf_end;
(*ENDIF*)
IF  pfkey = pf_none
THEN
    BEGIN
    IF  destin_pos + maxbuf_bytes - 1 <= buf_size
    THEN
        destin_upb := destin_pos + maxbuf_bytes - 1
    ELSE
        destin_upb := buf_size
    (*ENDIF*)
    END;
(*ENDIF*)
count := 0;
is_ok := false;
WHILE NOT is_ok AND (pfkey = pf_none) AND (count < 3) DO
    BEGIN
    count := count + 1;
    t09newscreen_page (term);
    t14bufdisplay (term, protfile, [utds_display], msg_source,
          buf, buf_size, source_pos, source_upb,
          source_pos - buf_offset, dummy_pos, pfkey);
    t09put (term, term.blankline, cut_protected);
    t14bufdisplay (term, protfile, [utds_display], msg_destin,
          buf, buf_size, destin_pos, destin_upb,
          destin_pos - buf_offset, dummy_pos, pfkey);
    t09put (term, term.blankline, cut_protected);
    n := bsp_name;
    n_endpos := bsp_name;
    t09get2name (term, '        MOVE LENGTH:', 'or    SOURCE ENDPOS:',
          cut_bright_protected, cut_bright_protected, cut_unprotected,
          cut_unprotected, mx_int4_displ, mx_int4_displ,
          c_upper, c_upper, n, n_endpos, pfkey);
    t09putmsg (term, term.blankline, NOT c_warning, NOT c_immed_displ);
    IF  pfkey = pf_none
    THEN
        BEGIN
        IF  n <> bsp_name
        THEN
            SAPDB_PascalForcedMove (sizeof (n), sizeof (ln), @n, 1, @ln, 1, sizeof (n))
        ELSE
            SAPDB_PascalForcedMove (sizeof (n_endpos), sizeof (ln), @n_endpos, 1,
                  @ln, 1, sizeof (n_endpos));
        (*ENDIF*)
        t14int4from_line (term, ln, 1, sizeof (n), move_len, is_ok);
        IF  is_ok
        THEN
            BEGIN
            IF  n <> bsp_name
            THEN
                (* length specified *)
                BEGIN
                IF  (source_pos + move_len - 1 > buf_size) OR
                    (destin_pos + move_len - 1 > buf_size) OR
                    (move_len < 1)
                THEN
                    BEGIN
                    is_ok := false;
                    t14display_msg (term, msg_illegal_len)
                    END
                (*ENDIF*)
                END
            ELSE
                (* endpos specified *)
                BEGIN
                move_len := move_len - buf_offset;
                IF  (move_len < source_pos) OR (move_len > buf_size)
                THEN
                    BEGIN
                    is_ok := false;
                    t14display_msg (term, msg_illegal_pos)
                    END
                ELSE
                    BEGIN
                    move_len := move_len - source_pos + 1;
                    IF  destin_pos + move_len - 1 > buf_size
                    THEN
                        BEGIN
                        is_ok := false;
                        t14display_msg (term, msg_illegal_len)
                        END
                    (*ENDIF*)
                    END
                (*ENDIF*)
                END
            (*ENDIF*)
            END
        (*ENDIF*)
        END
    (*ENDIF*)
    END;
(*ENDWHILE*)
IF  (pfkey = pf_none) AND NOT is_ok
THEN
    pfkey := pf_end;
(*ENDIF*)
IF  pfkey = pf_none
THEN
    BEGIN
    t09newscreen_page (term);
    IF  move_len > 4 * minbuf_bytes
    THEN
        BEGIN
        msg_source := 'SOURCE (trunc):               ';
        source_upb := source_pos + 4 * minbuf_bytes - 1
        END
    ELSE
        source_upb := source_pos + move_len - 1;
    (*ENDIF*)
    IF  move_len > 2 * maxbuf_bytes
    THEN
        scan := [utds_minbuf, utds_display]
    ELSE
        scan := [utds_display];
    (*ENDIF*)
    t14bufdisplay (term, protfile, scan, msg_source,
          buf, buf_size, source_pos, source_upb,
          source_pos - buf_offset, dummy_pos, pfkey);
    IF  destin_pos > 5
    THEN
        destin_lwb := destin_pos - 5
    ELSE
        destin_lwb := 1;
    (*ENDIF*)
    IF  destin_upb + maxbuf_bytes - 1 <= buf_size
    THEN
        destin_upb := destin_lwb + maxbuf_bytes - 1
    ELSE
        destin_upb := buf_size;
    (*ENDIF*)
    IF  destin_upb < destin_pos + move_len - 1
    THEN
        count := destin_upb - destin_pos + 1
    ELSE
        count := move_len;
    (*ENDIF*)
    count  := count * 3;
    ln     := term.blankline;
    offset := mx_maxbuf_prefix +
          (destin_pos - destin_lwb) * mx_maxbuf_byte;
    ln [offset+2] := '|';
    FOR i := 3 TO count DO
        ln [offset+i] := '=';
    (*ENDFOR*)
    IF  destin_upb < destin_pos + move_len - 1
    THEN
        ln [offset + count+1] := '>'
    ELSE
        ln [offset + count  ] := '|';
    (*ENDIF*)
    t09put (term, term.blankline, cut_bright_protected);
    t14bufdisplay (term, protfile, [utds_display], msg_destin,
          buf, buf_size, destin_lwb, destin_upb,
          destin_lwb - buf_offset, dummy_pos, pfkey);
    t09put (term, ln, cut_bright_protected);
    t09put (term, term.blankline, cut_protected);
    ln     := term.blankline;
    ln_len := 0;
    g17sname_to_line ('MOVE from   ', ln_len, ln);
    ln_len := ln_len + 1;
    g17trimint4_to_line (source_pos - buf_offset, ln_len, ln);
    ln_len      := ln_len + 1;
    ln [ln_len] := '-';
    g17trimint4_to_line (source_pos + move_len - 1 - buf_offset,
          ln_len, ln);
    g17sname_to_line (' to         ', ln_len, ln);
    ln_len := ln_len + 1;
    g17trimint4_to_line (destin_pos - buf_offset, ln_len, ln);
    ln_len      := ln_len + 1;
    ln [ln_len] := '-';
    g17trimint4_to_line (destin_pos + move_len - 1 - buf_offset,
          ln_len, ln);
    g17sname_to_line (' (length    ', ln_len, ln);
    ln_len := ln_len + 1;
    g17trimint4_to_line (move_len, ln_len, ln);
    ln_len      := ln_len + 1;
    ln [ln_len] := ')';
    t09put (term, ln, cut_bright_protected);
    t09put (term, term.blankline, cut_protected);
    n := 'y                 ';
    t09get1name (term, 'EXECUTE MOVE ? (y/n)', cut_bright_protected,
          cut_unprotected, mx_yes_no, c_upper, n, pfkey)
    END;
(*ENDIF*)
IF  pfkey = pf_none
THEN
    BEGIN
    IF  n = 'Y                 '
    THEN
        BEGIN
        IF  source_pos < destin_pos
        THEN
            FOR i := move_len-1 DOWNTO 0 DO
                buf[ destin_pos+i ] := buf[ source_pos+i ]
            (*ENDFOR*)
        ELSE
            SAPDB_PascalForcedOverlappingMove (buf_size, buf_size, @buf, source_pos,
                  @buf, destin_pos, move_len)
        (*ENDIF*)
        END
    (*ENDIF*)
    END;
(*ENDIF*)
IF  pfkey = pf_end
THEN
    pfkey := pf_none
(*ENDIF*)
END;

(*------------------------------*)

PROCEDURE
        ta14mtype_assign ;



VAR
      found  : boolean;
      curr   : tgg00_MessType;
      ln_len : integer;

      univ_ln : RECORD
            CASE integer OF
                1:
                    (ln : tsp00_Line);
                2:
                    (n  : tsp00_Name)
                END;
            (*ENDCASE*)


BEGIN
found := false;
curr  := m_nil;
REPEAT
    univ_ln.n := bsp_name;
    ln_len    := 0;
    g17messtype_to_line (curr, ln_len, univ_ln.ln);
    t09uppercase_line (univ_ln.ln, 1, sizeof (univ_ln.n));
    IF  univ_ln.n = mtype_value
    THEN
        found := true
    ELSE
        IF  curr = m_autosave
        THEN
            curr := m_nil
        ELSE
            curr := succ (curr)
        (*ENDIF*)
    (*ENDIF*)
UNTIL
    found OR (curr = m_nil);
(*ENDREPEAT*)
IF  found
THEN
    buf [buf_pos] := chr (ord (curr))
(*ENDIF*)
END;

(*------------------------------*)

PROCEDURE
        ta14mtype2_assign ;



VAR
      found  : boolean;
      curr   : tgg00_MessType2;
      ln_len : integer;

      univ_ln : RECORD
            CASE integer OF
                1:
                    (ln : tsp00_Line);
                2:
                    (n  : tsp00_Name)
                END;
            (*ENDCASE*)


BEGIN
found := false;
curr  := mm_nil;
REPEAT
    univ_ln.n := bsp_name;
    ln_len    := 0;
    g17mess2type_to_line (curr, ln_len, univ_ln.ln);
    t09uppercase_line (univ_ln.ln, 1, sizeof (univ_ln.n));
    IF  univ_ln.n = mtype_value
    THEN
        found := true
    ELSE
        IF  curr = mm_checkpoint
        THEN
            curr := mm_nil
        ELSE
            curr := succ (curr)
        (*ENDIF*)
    (*ENDIF*)
UNTIL
    found OR (curr = mm_nil);
(*ENDREPEAT*)
IF  found
THEN
    buf [buf_pos] := chr (ord (curr))
(*ENDIF*)
END;

(*------------------------------*)

PROCEDURE
        ta14set_assign ;



CONST
      c_unused_bit_sign = '-';

VAR
      pos      : integer;
      curr_bit : integer;

      univ_set : RECORD
            CASE integer OF
                1:
                    (c : tsp00_C2);
                2:
                    (s : PACKED SET OF 0..7);
                END;
            (*ENDCASE*)


BEGIN
univ_set.s := [ ];
curr_bit   := 0;
pos        := ln_offset + 1;
REPEAT
    WHILE (ln [pos] = ' ') AND (pos < sizeof (ln)) DO
        pos := pos + 1;
    (*ENDWHILE*)
    IF  (ln [pos] <> c_unused_bit_sign) AND (ln [pos] <> ' ')
    THEN
        univ_set.s := univ_set.s + [curr_bit];
    (*ENDIF*)
    WHILE (ln [pos] <> ' ') AND (pos < sizeof (ln)) DO
        pos := pos + 1;
    (*ENDWHILE*)
    curr_bit := curr_bit + 1
UNTIL
    (curr_bit > 7) OR (pos >= sizeof (ln));
(*ENDREPEAT*)
buf [buf_pos] := univ_set.c [1]
END;


(*
static char ident_mf[] = "@(#)vta14 FAST I386 UNIX LINUX PTOC -REL30=1 -RELVER=R76 -LC_STATE=RAMP -COMPILER=UNDEF -MACHINE=UNDEF -debug=1";
*)
