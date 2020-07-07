
/* some common prolog for every module
 */

#ifndef GSP00_H
#define GSP00_H


/******************************************
 *            I N C L U D E S             *
 ******************************************/


#ifndef GSP00_2_H
#include "gsp00_2.h"
#endif

/******************************************
 *           C O N S T A N T S            *
 ******************************************/

#define csp_comlint                     "\xCA\x21\x47\x48\x36\x48\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
#define csp_maxlint                     "\xCA\x21\x47\x48\x36\x47\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
#define csp_minlint                     "\x36\x78\x52\x51\x63\x52\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
#define csp_comsint                     "\xC5\x32\x76\x80\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
#define csp_maxsint                     "\xC5\x32\x76\x70\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
#define csp_minsint                     "\x3B\x67\x23\x20\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
#define csp_comluns                     "\xCA\x42\x94\x96\x72\x96\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
#define csp_maxluns                     "\xCA\x42\x94\x96\x72\x95\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
#define csp_minluns                     "\x80\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
#define csp_comsuns                     "\xC5\x65\x53\x60\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
#define csp_maxsuns                     "\xC5\x65\x53\x50\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
#define csp_minsuns                     "\x80\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
#define csp_maxsrel                     "\xE7\x34\x02\x82\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
#define csp_minsrel                     "\x9B\x11\x75\x49\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
#define csp_null_number                 "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
#define csp_min_number                  "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
#define csp_max_number                  "\xFF\x99\x99\x99\x99\x99\x99\x99\x99\x99\x99\x99\x99\x99\x99\x99\x99\x99\x99\x99"
#define csp_rescnt_unknown              "\x00\x3F\x90\x00\x00\x00\x00"
#define csp_rescnt_one                  "\x00\xC1\x10\x00\x00\x00\x00"
#define csp_rescnt_zero                 "\x00\x80\x00\x00\x00\x00\x00"
#define csp_zero_exponent              0x80
#define csp_zero_exp_value                    128
#define csp_value_pi                    "\xC1\x31\x41\x59\x26\x53\x58\x97\x93\x23\x84\x62\x64\x33\x83\x27\x95\x02\x88\x41"
#define csp_pw_undef                   0x00000000
    /* constants for VSP51 */
#define csp_one_number                  "\xC1\x10\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
#define csp_epsilon_number              "\xB1\x10\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
#define csp_a_half_number               "\xC0\x50\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
#define csp_two_number                  "\xC1\x20\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    /* end VSP51 */
    /* constants for VKB78 */
#define csp_number256                   "\xC3\x25\x60\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    /* end VKB78 */
    /**/
    /* About Box ID's */
#define csp_dlg_aboutbox                      100
#define csp_dlgabout_sysname                  100
#define csp_dlgabout_cname                    101
#define csp_dlgabout_relno                    102
#define csp_dlgabout_dbasename                103
#define csp_dlgabout_username                 104
#define csp_switch_lang_changed               200
    /* Common Dialog Boxes */
#define csp_dlg_fileopenbox                   200
#define csp_dlg_filesavebox                   210
#define csp_dlg_printbox                      220
#define csp_dlg_printersetupbox               230
#define csp_dlg_editfindbox                   240
#define csp_dlg_editreplacebox                250
    /* Error Boxes */
#define csp_dlg_fatal                         999
#define csp_dlg_error                         998
#define csp_dlg_warning                       997
#define csp_dlg_info                          996
    /**/
#define csp_star1                      '\x1F'    /*  in like '*', '%'  */
#define csp_any1                       '\x1E'    /*  in like '?', '_'  */
#define csp_cclass                     '\x1D'    /*  in like '(,)'     */
#define csp_crange                     '\x1C'    /*  in like '-'       */
#define csp_cnot                       '\x19'    /*  in like 'not'     */
#define csp_ascii_zero                 '0'
#define csp_ascii_blank                ' '
#define csp_ascii_star                 '*'
#define csp_ascii_question_mark        '?'
#define csp_ascii_underline            '_'
#define csp_ascii_percent              '%'
#define csp_ascii_open_bracket         '('
#define csp_ascii_close_bracket        ')'
#define csp_ascii_quote                '\''
#define csp_ascii_double_quote         '\"'
#define csp_ascii_not                  0xac
#define csp_ascii_tilde                '~'
#define csp_ascii_hyphen               '-'
#define csp_ebcdic_zero                0xf0
#define csp_ebcdic_blank               '@'
#define csp_ebcdic_star                '\\'
#define csp_ebcdic_question_mark       'o'
#define csp_ebcdic_underline           'm'
#define csp_ebcdic_percent             'l'
#define csp_ebcdic_open_bracket        'M'
#define csp_ebcdic_close_bracket       ']'
#define csp_ebcdic_quote               '}'
#define csp_ebcdic_double_quote        0x7f
#define csp_ebcdic_not                 '_'
#define csp_ebcdic_tilde               0xb0
#define csp_ebcdic_hyphen              '`'
#define csp_defined_byte               '\x00'
#define csp_unicode_blank               "\x00\x20"
#define csp_unicode_blank_desc          "\xFF\xDF"
#define csp_unicode_def_byte           '\x01'
#define csp_unicode_mark               '\x00'
#define csp_undef_byte                 0xff
#define csp_oflw_byte                  0xfe
#define csp_default_byte               0xfd
#define csp_long_desc_def_byte         0xfc
    /**/
#define csp_info_fixed                 '\x00'
#define csp_info_float                 '\x01'
#define csp_info_char                  '\x02'
#define csp_info_byte                  '\x03'
#define csp_info_date                  '\x04'
#define csp_info_time                  '\x05'
#define csp_info_string                '\x06'
#define csp_info_expression            '\x07'
#define csp_info_timestamp             '\x08'
#define csp_info_dbyte_ascii           '\x09'
#define csp_info_dbyte_ebcdic          '\n'
#define csp_info_c_oldlong_char        '\x0B'
#define csp_info_b_oldlong_byte        '\x0C'
#define csp_info_a_oldlong_ascii_dbyte '\r'
#define csp_info_e_oldlong_ebcdic_dbyte '\x0E'
#define csp_info_c_long_char           '\x13'
#define csp_info_b_long_byte           '\x14'
#define csp_info_a_long_ascii_dbyte    '\x15'
#define csp_info_e_long_ebcdic_dbyte   '\x16'
#define csp_info_smallint              '\x0F'
#define csp_info_integer               '\x10'
#define csp_info_varchar               '\x11'
#define csp_info_escape_char           '\x12'
#define csp_info_boolean               '\x17'
#define csp_info_otherwise             '\x18'
#define csp_info_mandatory             '\x00'
#define csp_info_optional              '\x01'
#define csp_info_default               '\x02'
#define csp_info_input                 '\x00'
#define csp_info_output                '\x01'
#define csp_info_inout                 '\x02'
    /**/
#define csp_kanji_out1                 '\x0E'
#define csp_kanji_out2                 '('
#define csp_kanji_out3                 '8'
#define csp_kanji_in1                  '\x0F'
#define csp_kanji_in2                  ')'
    /**/
#define csp_ascii                               0
#define csp_ebcdic                              1
#define csp_codeneutral                         2
#define csp_unicode_swap                       19
#define csp_unicode                            20
#define csp_unicode_native                     21
#define csp_utf8                               22
#define csp_instance_code                     128    /* PTS 1117216 E.Z. */
    /**/
#define bsp_c1                         ' '
#define bsp_c2                          "  "
#define bsp_c5                          "     "
#define bsp_c6                          "      "
#define bsp_c8                          "        "
#define bsp_c10                         "          "
#define bsp_c11                         "           "
#define bsp_c12                         "            "
#define bsp_c14                         "              "
#define bsp_c15                         "               "
#define bsp_c16                         "                "
#define bsp_c18                         "                  "
#define bsp_c20                         "                    "
#define bsp_c24                         "                        "
#define bsp_c30                         "                              "
#define bsp_c32                         "                                "
#define bsp_c34                         "                                  "
#define bsp_c40                         "                                        "
#define bsp_c42                         "                                          "
#define bsp_c48                         "                                                "
#define bsp_c64                         "                                                                "
#define bsp_identifier                 bsp_c18
#define bsp_knl_identifier             bsp_c64
#define bsp_date                       bsp_c8
#define bsp_dbname                     bsp_c18
#define bsp_devname                    bsp_c40
#define bsp_errtext                    bsp_c40
#define bsp_lname                      bsp_c32
#define bsp_name                       bsp_c18
#define bsp_nodeid                     bsp_c64
#define bsp_opmsg                      bsp_c64
#define bsp_sname                      bsp_c12
#define bsp_termid                     bsp_c18
#define bsp_time                       bsp_c8
    /*--------------------------------------------------------------*/
#define UNDEF_SP00                     (- 1)
#define MIN_UINT1_SP00                          0
#define MAX_UINT1_SP00                        255
#define MIN_UINT2_SP00                          0    /* +++ do not use in PASCAL */
#define MAX_UINT2_SP00                      65535    /* +++ do not use in PASCAL */
#define MIN_INT2_SP00                  (- 32768)
#define MAX_INT2_SP00                       32767
#define MAX_INT4_SP00                  2147483647
    /**/
#define csp_max_fields                       1024    /* ak - constant */
#define csp_max_indices                        16    /* ak - constant */
#define csp_maxvariables                     2000    /* shall be 3000, but */
    /* because cache can not handle buffer with more than 32KB   */
    /* (and maxvariables+25) * 16 + something has to fit...      */
    /**/
#define csp_cntcachebuf                        11
#define csp_cryptname                           6    /* tsp00_CryptName */
#define csp_fixed                              38
#define csp_float_frac                 (- 1)
#define csp_numstack                           16
#define csp_old_fileversion            (- 8)
#define csp_pool_buffers                       16
#define csp_resnum_deflen                      10
#define csp_sfield_info                      1028
#define csp_splitted_commands          (- 10)
#define csp_stringlength                     8084
#define csp_use_new_pars_info          (- 9)

#if defined (NMP)

    /**/
#define TASKS_SP00                           1024

#else

#define TASKS_SP00                            256

#endif


#if defined (BIT64)

    /**/
#define ADDR_MXSP00                             8

#else

#define ADDR_MXSP00                             4

#endif

#define ANY_PACKED_CHAR_MXSP00              65535

#if defined (vers80)

#define BUF_MXSP00                          32768    /* PTS 1116801 E.Z. */

#else

#define BUF_MXSP00                           8192

#endif

#define BUF8K_MXSP00                         8192
#define COMPNAME_MXSP00                        64
#define CP_COMMANDPRE_VARS_MXSP00            1740
#define CRYPTPW_MXSP00                         24
#define CTABLE_MXSP00                         256
#define DATE_MXSP00                             8
#define DBNAME_MXSP00                          18
#define PATH_MXSP00                           256
#define DECIMAL_MXSP00                         20
#define DEVNAME_MXSP00                         40
#define DP_CODE_CACHE_MXSP00                32760    /* >= tdg_code_cache */
#define DP_DIALOG_GLOBAL_MXSP00              5500    /* >= tdg_dialog_global */
#define DP_DIAPRE_VARS_MXSP00                5000    /* >= tdg_diapre_variable_infos*/
#define DP_I_GLOBAL_MXSP00                  24000    /* >= tdg_i_global */
#define DP_PRECOMP_DCA_MXSP00                 600    /* >= tdg_dca */
#define DP_VALUE_CACHE_MXSP00               32760    /* >= tdg_value_cache */
#define ERRTEXT_MXSP00                         40
#define EXEC_ARGLINE_MXSP00                   512
#define EXEC_LINE_MXSP00                      512
#define EXEC_PROG_MXSP00                       64
#define EXT_DATE_MXSP00                        10
#define EXT_TIME_MXSP00                         8
#define EXT_TIMESTAMP_MXSP00                   26
#define INT2_MXSP00                             2
#define INT4_MXSP00                             4
#define PERM_KEY_MXSP00                      1024
#define KEY_MXSP00                           4020
#define LINE_MXSP00                           132
#define LINELENGTH_MXSP00                   32767
#define LNAME_MXSP00                           32
#define LONG_DESC_MXSP00                       40
#define LONG_DESC_BLOCK_MXSP00                 56
#define MICROSEC_MXSP00                         6
#define MOVEINT2_MXSP00                   4096000    /* tsp00_MoveObj / 2 */
#define MOVEOBJ_MXSP00                    8192000    /* tsp00_MoveObj     */
#define NAME_MXSP00                            18
#define NODEID_MXSP00                          64
#define NUMBER_MXSP00                          20
#define NUMSTR_MXSP00                          64
#define OPMSG_MXSP00                           64
#define PAGE_MXSP00                          8192
#define PRTLINE_MXSP00                        256
#define PRTNAME_MXSP00                         64
#define PW_MXSP00                              18
#define REGION_MXSP00                           8
#define RESNUM_MXSP00                           7    /* ResnumDeflen+1)div2+2*/
#define RP_REPORTPRE_VARS_MXSP00             5708
#define RTE_COMM_MXSP00                        12
#define SCREEN_BUF_MXSP00                    8488
#define SCREEN_CHARS_MXSP00                 32767
#define SFIELD_INFO_MXSP00                      8
#define SCREEN_LINES_MXSP00                 32767
#define SNAME_MXSP00                           12
#define SQLSTATE_MXSP00                         5
#define TERMID_MXSP00                          18
#define TIME_MXSP00                             8
#define TIMESTAMP_MXSP00                       20
#define TRACE_MXSP00                           16
#define UINT2_MXSP00                            2
#define UNLOAD_MXSP00                         264
#define VERSION_MXSP00                         40
#define VFILENAME_MXSP00                      256
#define VTLABEL_MXSP00                          8
#define VTMSG_MXSP00                           80
#define XUSERKEY_MXSP00                        18
#define ZONED_MXSP00                           20
    /* ******* virtual terminal ******* */
#define csp_attr_byte                           1    /*attr.-byte in front of each col*/
#define csp_vt_eof                              0    /* end_of_field                  */
#define csp_vt_soncf                            1    /* start non-continuation field  */
#define csp_vt_socf                             2    /* start of continuation field   */
#define csp_clear_before               true    /* clear screen                  */
#define csp_without_clear              false    /* overwrite screen              */
#define csp_lslabel                             8
#define csp_ls_number                           9
#define csp_sk_number                           9
#define csp_sk_lines                            2
#define csp_pc_screen_cols                     80
    /* graphic mode constants */
#define csp_vertical_bar               '\x08'
#define csp_l_right_corner             '\x0B'
#define csp_u_right_corner             '\x0C'
#define csp_l_left_corner              '\x10'
#define csp_u_left_corner              '\x11'
#define csp_left_t                     '\x12'
#define csp_right_t                    '\x13'
#define csp_horizontal_bar             '\x14'
#define csp_bottom_t                   '\x1C'
#define csp_top_t                      '\x1D'
#define csp_crossing_lines             '\x1E'
    /* Senderid-constants of the separate Components */
#define csp_comp_cpc                    "CPC"
#define csp_comp_cpp                    "CPP"    /* SQLClass PTS 1112481 FF */
#define csp_comp_db_manager             "DBM"
#define csp_cpmp_dbg                    "DBG"    /* dbproc debugger, PTS 1122997 */
#define csp_comp_gpc                    "GPC"    /* Generic Programming Client: Python ... */
#define csp_comp_jdbc                   "JDB"
#define csp_comp_kernel                 "KNL"
#define csp_comp_load                   "LOA"
#define csp_comp_odbc                   "ODB"
#define csp_comp_util                   "UTI"    /* Repair-Tool */
#define csp_comp_xci                    "XCI"
    /* max amount of parameters of a DCOM procedure */
#define cak260max_params                      256
#define cak260max_fparams                       8    /* max Float parameter */
    /**/
    /* ----- OLD CONSTANTS ----- */
#define csp_minint1                    MIN_UINT1_SP00
#define csp_maxint1                    MAX_UINT1_SP00
#define csp_minint2                    MIN_INT2_SP00
#define csp_maxint2                    MAX_INT2_SP00
#define csp_maxint4                    MAX_INT4_SP00
    /**/
#define mxsp_c1                                 1
#define mxsp_c2                                 2
#define mxsp_c3                                 3
#define mxsp_c4                                 4
#define mxsp_c5                                 5
#define mxsp_c6                                 6
#define mxsp_c8                                 8
#define mxsp_c10                               10
#define mxsp_c12                               12
#define mxsp_c14                               14
#define mxsp_c15                               15
#define mxsp_c16                               16
#define mxsp_c18                               18
#define mxsp_c20                               20
#define mxsp_c24                               24
#define mxsp_c30                               30
#define mxsp_c40                               40
#define mxsp_c50                               50
#define mxsp_c60                               60
#define mxsp_c64                               64
#define mxsp_c70                               70
#define mxsp_c80                               80
#define mxsp_c256                             256
    /**/
#define mxsp_buf                       BUF_MXSP00
#define mxsp_date                      DATE_MXSP00
#define mxsp_extdate                   EXT_DATE_MXSP00
#define mxsp_exttime                   EXT_TIME_MXSP00
#define mxsp_exttimestamp              EXT_TIMESTAMP_MXSP00
#define mxsp_long_desc                 LONG_DESC_MXSP00
#define mxsp_name                      NAME_MXSP00
#define mxsp_number                    NUMBER_MXSP00
#define mxsp_resnum                    RESNUM_MXSP00
#define mxsp_prtline                   PRTLINE_MXSP00
#define mxsp_termid                    TERMID_MXSP00
#define mxsp_time                      TIME_MXSP00
#define mxsp_timestamp                 TIMESTAMP_MXSP00

/******************************************
 *               T Y P E S                *
 ******************************************/

     typedef teo00_Bool     tsp00_Bool;
     typedef teo00_Byte     tsp00_Byte;
    /* don't declare tsp00_Int1:                                 */
    /* tsp_int1 must be unsigned because of Pascal compatibility */
    /*    declaring tsp00_Int1 as unsigned is misleading!        */
    /* typedeff teo00_Int1     tsp00_Int1;                       */
    /* the typedef above is just a comment and should never be enabled */
     typedef teo00_Uint1    tsp00_Uint1;
     typedef teo00_Int2     tsp00_Int2;
     typedef teo00_Uint2    tsp00_Uint2;
     typedef teo00_Int4     tsp00_Int4;
     typedef teo00_Uint4    tsp00_Uint4;
     typedef teo00_Int      tsp00_Int;
     typedef teo00_Uint     tsp00_Uint;
     typedef teo00_Longint  tsp00_Longint;
     typedef teo00_Longuint tsp00_Longuint;
    /* Caution: 8 byte integers might be VERY slow on 32 bit machines */
     typedef teo00_Int8     tsp00_Int8;
     typedef teo00_Uint8    tsp00_Uint8;
typedef char                       tsp00_Char;
/* tsp00_Uint1 ignored */
/* tsp00_Int2 ignored */
/* tsp00_Uint2 ignored */
/* tsp00_Int4 ignored */
/* tsp00_Uint4 ignored */

#if defined (BIT64)

/* tsp00_Longint ignored */

#else

/* tsp00_Longint ignored */

#endif

typedef real                       tsp00_Shortreal;
typedef double                     tsp00_Longreal;
typedef char                      *tsp00_Addr;
typedef char                      *tsp35proc_ptr;
    /*                 will be substituted by ptoc to tsp35proc_ptr () */
/* tsp00_Byte ignored */
typedef tsp00_Byte                *tsp00_BytePtr;

#if defined (BIT64)

    /**/
    /* 8 byte counter: integer on 64 bit machine double elsewhere */
     typedef teo00_Longint  tsp00_8ByteCounter;
/* tsp00_8ByteCounter ignored */

#else

typedef double                     tsp00_8ByteCounter;

#endif

    /**/

typedef enum tsp00_PascalComponentID_Enum
{
    cidKernel_esp00,
    cidQuickKnl_esp00,
    cidSlowKnl_esp00,
    cidQuery_esp00,
    cidLoad_esp00,
    cidDiagnose_esp00
} tsp00_PascalComponentID_Enum;
typedef tsp00_PascalComponentID_Enum tsp00_PascalComponentID_Param;
typedef ENUM_TYPE_MSP00 (          tsp00_PascalComponentID, tsp00_PascalComponentID_Enum, cidDiagnose_esp00, unsigned char);
    /**/

typedef enum tsp00_ColMode_Enum
{
    mod_syskey,
    mod_key,
    mod_mandatory,
    mod_optional,
    mod_dropped
} tsp00_ColMode_Enum;
typedef tsp00_ColMode_Enum tsp00_ColMode_Param;
typedef ENUM_TYPE_MSP00 (          tsp00_ColMode, tsp00_ColMode_Enum, mod_dropped, unsigned char);
    /**/

typedef enum tsp00_ColumnType_Enum
{
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
    gc_longchar_ebcdic
} tsp00_ColumnType_Enum;
typedef tsp00_ColumnType_Enum tsp00_ColumnType_Param;
typedef ENUM_TYPE_MSP00 (          tsp00_ColumnType, tsp00_ColumnType_Enum, gc_longchar_ebcdic, unsigned char);
    /**/

typedef enum tsp00_DatabaseState_Enum
{
    dbCold_esp00,
    dbWarm_esp00
} tsp00_DatabaseState_Enum;
typedef tsp00_DatabaseState_Enum tsp00_DatabaseState_Param;
typedef ENUM_TYPE_MSP00 (          tsp00_DatabaseState, tsp00_DatabaseState_Enum, dbWarm_esp00, unsigned char);
    /**/

typedef enum tsp00_DataType_Enum
{
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
    dunknown,
    dnumber,
    dnonumber,
    dduration,
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
    dwyde
} tsp00_DataType_Enum;
typedef tsp00_DataType_Enum tsp00_DataType_Param;
typedef ENUM_TYPE_MSP00 (          tsp00_DataType, tsp00_DataType_Enum, dwyde, unsigned char);
    /**/

typedef enum tsp00_Dbcs_Enum
{
    no_dbcs,
    dbcs_with_shifts,
    dbcs_no_shifts
} tsp00_Dbcs_Enum;
typedef tsp00_Dbcs_Enum tsp00_Dbcs_Param;
typedef ENUM_TYPE_MSP00 (          tsp00_Dbcs, tsp00_Dbcs_Enum, dbcs_no_shifts, unsigned char);
    /**/

typedef enum tsp00_DbObjectType_Enum
{
    dbo_method,
    dbo_dbproc,
    dbo_dbfunc,
    dbo_package,
    dbo_system_trigger
} tsp00_DbObjectType_Enum;
typedef tsp00_DbObjectType_Enum tsp00_DbObjectType_Param;
typedef ENUM_TYPE_MSP00 (          tsp00_DbObjectType, tsp00_DbObjectType_Enum, dbo_system_trigger, unsigned char);
    /**/

typedef enum tsp00_Default_Enum
{
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
    other_default
} tsp00_Default_Enum;
typedef tsp00_Default_Enum tsp00_Default_Param;
typedef ENUM_TYPE_MSP00 (          tsp00_Default, tsp00_Default_Enum, other_default, unsigned char);
    /**/

typedef enum tsp00_DgcallbackResult_Enum
{
    dg_ok,
    dg_cancel,
    dg_again
} tsp00_DgcallbackResult_Enum;
typedef tsp00_DgcallbackResult_Enum tsp00_DgcallbackResult_Param;
typedef ENUM_TYPE_MSP00 (          tsp00_DgcallbackResult, tsp00_DgcallbackResult_Enum, dg_again, unsigned char);
    /**/

typedef enum tsp00_DlgBoxStyle_Enum
{
    dg_modal,
    dg_modeless
} tsp00_DlgBoxStyle_Enum;
typedef tsp00_DlgBoxStyle_Enum tsp00_DlgBoxStyle_Param;
typedef ENUM_TYPE_MSP00 (          tsp00_DlgBoxStyle, tsp00_DlgBoxStyle_Enum, dg_modeless, unsigned char);
    /**/

typedef enum tsp00_ExecMode_Enum
{
    async,
    sync_new_session,
    sync_same_session
} tsp00_ExecMode_Enum;
typedef tsp00_ExecMode_Enum tsp00_ExecMode_Param;
typedef ENUM_TYPE_MSP00 (          tsp00_ExecMode, tsp00_ExecMode_Enum, sync_same_session, unsigned char);
    /**/

typedef enum tsp00_ExecReturn_Enum
{
    ex_ok,
    ex_notok,
    ex_no_exec,
    ex_no_async,
    ex_no_sync,
    ex_no_sync_new,
    ex_no_sync_same
} tsp00_ExecReturn_Enum;
typedef tsp00_ExecReturn_Enum tsp00_ExecReturn_Param;
typedef ENUM_TYPE_MSP00 (          tsp00_ExecReturn, tsp00_ExecReturn_Enum, ex_no_sync_same, unsigned char);
    /**/

typedef enum tsp00_LcompResult_Enum
{
    l_less,
    l_equal,
    l_greater,
    l_undef
} tsp00_LcompResult_Enum;
typedef tsp00_LcompResult_Enum tsp00_LcompResult_Param;
typedef ENUM_TYPE_MSP00 (          tsp00_LcompResult, tsp00_LcompResult_Enum, l_undef, unsigned char);
    /**/

typedef enum tsp00_NetError_Enum
{
    net_ok,
    net_notok,
    net_timeout,
    net_send_line_down,
    net_receive_line_down
} tsp00_NetError_Enum;
typedef tsp00_NetError_Enum tsp00_NetError_Param;
typedef ENUM_TYPE_MSP00 (          tsp00_NetError, tsp00_NetError_Enum, net_receive_line_down, unsigned char);
    /**/

typedef enum tsp00_NumError_Enum
{
    num_ok,
    num_trunc,
    num_overflow,
    num_invalid,
    num_incompatible,
    num_missing_addr
} tsp00_NumError_Enum;
typedef tsp00_NumError_Enum tsp00_NumError_Param;
typedef ENUM_TYPE_MSP00 (          tsp00_NumError, tsp00_NumError_Enum, num_missing_addr, unsigned char);
    /**/

typedef enum tsp00_Os_Enum
{
    os_unix,
    os_vms,
    os_windows,
    os_win32,
    os_os2
} tsp00_Os_Enum;
typedef tsp00_Os_Enum tsp00_Os_Param;
typedef ENUM_TYPE_MSP00 (          tsp00_Os, tsp00_Os_Enum, os_os2, unsigned char);
    /**/

typedef enum tsp00_Service_Enum
{
    sql_user,
    sql_async_user,
    sql_utility,
    sqlDistribution_esp00,
    sqlControl_esp00,
    sqlEvent_esp00,
    sqlStreamUser_esp00
} tsp00_Service_Enum;
typedef tsp00_Service_Enum tsp00_Service_Param;
typedef ENUM_TYPE_MSP00 (          tsp00_Service, tsp00_Service_Enum, sqlStreamUser_esp00, unsigned char);
    /* */

typedef enum tsp00_ShutdownMode_Enum
{
    shtShutdKill_esp00,
    shtShutdNormal_esp00
} tsp00_ShutdownMode_Enum;
typedef tsp00_ShutdownMode_Enum tsp00_ShutdownMode_Param;
typedef ENUM_TYPE_MSP00 (          tsp00_ShutdownMode, tsp00_ShutdownMode_Enum, shtShutdNormal_esp00, unsigned char);
    /**/

typedef enum tsp00_SkLabel_Enum
{
    no_sk_labels,
    short_sk_labels,
    long_sk_labels
} tsp00_SkLabel_Enum;
typedef tsp00_SkLabel_Enum tsp00_SkLabel_Param;
typedef ENUM_TYPE_MSP00 (          tsp00_SkLabel, tsp00_SkLabel_Enum, long_sk_labels, unsigned char);
    /**/

typedef enum tsp00_Sproc_Enum
{
    sppr_db_proc,
    sppr_ddl_trigger,
    sppr_insert_trigger,
    sppr_update_trigger,
    sppr_delete_trigger,
    sppr_db_func
} tsp00_Sproc_Enum;
typedef tsp00_Sproc_Enum tsp00_Sproc_Param;
typedef ENUM_TYPE_MSP00 (          tsp00_Sproc, tsp00_Sproc_Enum, sppr_db_func, unsigned char);
    /**/

typedef enum tsp00_SqlMode_Enum
{
    sqlm_ansi,
    sqlm_db2,
    sqlm_oracle,
    sqlm_internal
} tsp00_SqlMode_Enum;
typedef tsp00_SqlMode_Enum tsp00_SqlMode_Param;
typedef ENUM_TYPE_MSP00 (          tsp00_SqlMode, tsp00_SqlMode_Enum, sqlm_internal, unsigned char);
    /**/

typedef enum tsp00_SwapKind_Enum
{
    sw_dummy,
    sw_normal,
    sw_full_swapped,
    sw_part_swapped
} tsp00_SwapKind_Enum;
typedef tsp00_SwapKind_Enum tsp00_SwapKind_Param;
typedef ENUM_TYPE_MSP00 (          tsp00_SwapKind, tsp00_SwapKind_Enum, sw_part_swapped, unsigned char);
    /**/

typedef enum tsp00_TaskState_Enum
{
    tskSessionWait_esp00,
    tskCommandWait_esp00,
    tskIoWait_esp00,
    tskSystemLockWait_esp00,
    tskUserLockWait_esp00,
    tskRegionWait_esp00,
    tskRunning_esp00,
    tskSleeping_esp00
} tsp00_TaskState_Enum;
typedef tsp00_TaskState_Enum tsp00_TaskState_Param;
typedef ENUM_TYPE_MSP00 (          tsp00_TaskState, tsp00_TaskState_Enum, tskSleeping_esp00, unsigned char);
    /**/

typedef enum tsp00_ToolLayer_Enum
{
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
    na
} tsp00_ToolLayer_Enum;
typedef tsp00_ToolLayer_Enum tsp00_ToolLayer_Param;
typedef ENUM_TYPE_MSP00 (          tsp00_ToolLayer, tsp00_ToolLayer_Enum, na, unsigned char);
    /**/

typedef enum tsp00_UniError_Enum
{
    spu_ok,
    spu_not_translatable,
    spu_dest_too_short
} tsp00_UniError_Enum;
typedef tsp00_UniError_Enum tsp00_UniError_Param;
typedef ENUM_TYPE_MSP00 (          tsp00_UniError, tsp00_UniError_Enum, spu_dest_too_short, unsigned char);
    /**/

typedef enum tsp00_ValMode_Enum
{
    vm_datapart,
    vm_alldata,
    vm_lastdata,
    vm_nodata,
    vm_no_more_data,
    vm_last_putval,
    vm_data_trunc,
    vm_close,
    vm_error,
    vm_startpos_invalid
} tsp00_ValMode_Enum;
typedef tsp00_ValMode_Enum tsp00_ValMode_Param;
typedef ENUM_TYPE_MSP00 (          tsp00_ValMode, tsp00_ValMode_Enum, vm_startpos_invalid, unsigned char);
    /**/

typedef enum tsp00_VaReturn_Enum
{
    va_ok,
    va_notok,
    va_eof,
    va_wait_first
} tsp00_VaReturn_Enum;
typedef tsp00_VaReturn_Enum tsp00_VaReturn_Param;
typedef ENUM_TYPE_MSP00 (          tsp00_VaReturn, tsp00_VaReturn_Enum, va_wait_first, unsigned char);
    /**/

typedef enum tsp00_VFileOpCodes_Enum
{
    vread,
    vwrite,
    vappend,
    voverwrite,
    vread_ora
} tsp00_VFileOpCodes_Enum;
typedef tsp00_VFileOpCodes_Enum tsp00_VFileOpCodes_Param;
typedef ENUM_TYPE_MSP00 (          tsp00_VFileOpCodes, tsp00_VFileOpCodes_Enum, vread_ora, unsigned char);
    /**/

typedef enum tsp00_VfFormat_Enum
{
    vf_plaintext,
    vf_record,
    vf_stream,
    vf_unknown,
    vf_4k_block
} tsp00_VfFormat_Enum;
typedef tsp00_VfFormat_Enum tsp00_VfFormat_Param;
typedef ENUM_TYPE_MSP00 (          tsp00_VfFormat, tsp00_VfFormat_Enum, vf_4k_block, unsigned char);
    /**/

typedef enum tsp00_VfResource_Enum
{
    vf_stack,
    vf_bufferpool
} tsp00_VfResource_Enum;
typedef tsp00_VfResource_Enum tsp00_VfResource_Param;
typedef ENUM_TYPE_MSP00 (          tsp00_VfResource, tsp00_VfResource_Enum, vf_bufferpool, unsigned char);
    /**/

typedef enum tsp00_VfReturn_Enum
{
    vf_ok,
    vf_notok,
    vf_eof,
    vf_noseek
} tsp00_VfReturn_Enum;
typedef tsp00_VfReturn_Enum tsp00_VfReturn_Param;
typedef ENUM_TYPE_MSP00 (          tsp00_VfReturn, tsp00_VfReturn_Enum, vf_noseek, unsigned char);
    /**/

typedef enum tsp00_VfType_Enum
{
    vf_t_unknown,
    vf_t_file,
    vf_t_pipe,
    vf_t_raw,
    vf_t_tape_norew,
    vf_t_tape_rew,
    vf_t_dafs_file
} tsp00_VfType_Enum;
typedef tsp00_VfType_Enum tsp00_VfType_Param;
typedef ENUM_TYPE_MSP00 (          tsp00_VfType, tsp00_VfType_Enum, vf_t_dafs_file, unsigned char);
    /**/

typedef enum tsp00_VpLinefeeds_Enum
{
    vp_nolf,
    vp_onelf,
    vp_twolf,
    vp_threelf,
    vp_page
} tsp00_VpLinefeeds_Enum;
typedef tsp00_VpLinefeeds_Enum tsp00_VpLinefeeds_Param;
typedef ENUM_TYPE_MSP00 (          tsp00_VpLinefeeds, tsp00_VpLinefeeds_Enum, vp_page, unsigned char);
    /**/

typedef enum tsp00_VtColor_Enum
{
    vt_default_color,
    vt_white,
    vt_black,
    vt_red,
    vt_green,
    vt_yellow,
    vt_blue,
    vt_pink,
    vt_light_blue,
    vt_last_color
} tsp00_VtColor_Enum;
typedef tsp00_VtColor_Enum tsp00_VtColor_Param;
typedef ENUM_TYPE_MSP00 (          tsp00_VtColor, tsp00_VtColor_Enum, vt_last_color, unsigned char);
    /**/

typedef enum tsp00_VtKey_Enum
{
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
    vt_last_key
} tsp00_VtKey_Enum;
typedef tsp00_VtKey_Enum tsp00_VtKey_Param;
typedef ENUM_TYPE_MSP00 (          tsp00_VtKey, tsp00_VtKey_Enum, vt_last_key, unsigned char);
    /**/

typedef enum tsp00_VtMark_Enum
{
    vt_mark_off,
    vt_mark_block,
    vt_mark_line,
    vt_mark_contiguous,
    vt_mark_matrix
} tsp00_VtMark_Enum;
typedef tsp00_VtMark_Enum tsp00_VtMark_Param;
typedef ENUM_TYPE_MSP00 (          tsp00_VtMark, tsp00_VtMark_Enum, vt_mark_matrix, unsigned char);
    /**/

typedef enum tsp00_VtMode_Enum
{
    vt_bright,
    vt_inverse,
    vt_underline,
    vt_blink,
    vt_invisible,
    vt_mixed,
    vt_grayed,
    vt_last_mode
} tsp00_VtMode_Enum;
typedef tsp00_VtMode_Enum tsp00_VtMode_Param;
typedef ENUM_TYPE_MSP00 (          tsp00_VtMode, tsp00_VtMode_Enum, vt_last_mode, unsigned char);
    /**/

typedef enum tsp00_VtScroll_Enum
{
    vt_horizontal_scroll,
    vt_vertical_scroll
} tsp00_VtScroll_Enum;
typedef tsp00_VtScroll_Enum tsp00_VtScroll_Param;
typedef ENUM_TYPE_MSP00 (          tsp00_VtScroll, tsp00_VtScroll_Enum, vt_vertical_scroll, unsigned char);
    /**/

typedef enum tsp00_VtUsageMode_Enum
{
    vt_form,
    vt_edit
} tsp00_VtUsageMode_Enum;
typedef tsp00_VtUsageMode_Enum tsp00_VtUsageMode_Param;
typedef ENUM_TYPE_MSP00 (          tsp00_VtUsageMode, tsp00_VtUsageMode_Enum, vt_edit, unsigned char);
    /**/

typedef enum tsp00_Warnings_Enum
{
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
    warn15_user_defined_code
} tsp00_Warnings_Enum;
typedef tsp00_Warnings_Enum tsp00_Warnings_Param;
typedef ENUM_TYPE_MSP00 (          tsp00_Warnings, tsp00_Warnings_Enum, warn15_user_defined_code, unsigned char);
    /**/

typedef enum tsp00_Wlisttype_Enum
{
    wls_stream,
    wls_line,
    wls_matrix
} tsp00_Wlisttype_Enum;
typedef tsp00_Wlisttype_Enum tsp00_Wlisttype_Param;
typedef ENUM_TYPE_MSP00 (          tsp00_Wlisttype, tsp00_Wlisttype_Enum, wls_matrix, unsigned char);
    /**/

typedef enum tsp00_XpKey_Enum
{
    xp_int2_type,
    xp_int4_type,
    xp_c8_type,
    xp_c18_type,
    xp_c24_type,
    xp_c40_type,
    xp_c64_type,
    xp_crypt_type
} tsp00_XpKey_Enum;
typedef tsp00_XpKey_Enum tsp00_XpKey_Param;
typedef ENUM_TYPE_MSP00 (          tsp00_XpKey, tsp00_XpKey_Enum, xp_crypt_type, unsigned char);
    /**/

typedef enum tsp00_XpReturnCode_Enum
{
    xp_ok,
    xp_key_not_found,
    xp_eof,
    xp_not_ok
} tsp00_XpReturnCode_Enum;
typedef tsp00_XpReturnCode_Enum tsp00_XpReturnCode_Param;
typedef ENUM_TYPE_MSP00 (          tsp00_XpReturnCode, tsp00_XpReturnCode_Enum, xp_not_ok, unsigned char);
    /**/
typedef OLDPASCALSTRING_TYPE_MSP00 (  tsp00_C1, 1);
typedef CSTRING_TYPE_MSP00      (  tsp00_C1c, 1);
typedef OLDPASCALSTRING_TYPE_MSP00 (  tsp00_C2, 2);
typedef CSTRING_TYPE_MSP00      (  tsp00_C2c, 2);
typedef OLDPASCALSTRING_TYPE_MSP00 (  tsp00_C3, 3);
typedef CSTRING_TYPE_MSP00      (  tsp00_C3c, 3);
typedef OLDPASCALSTRING_TYPE_MSP00 (  tsp00_C4, 4);
typedef CSTRING_TYPE_MSP00      (  tsp00_C4c, 4);
typedef OLDPASCALSTRING_TYPE_MSP00 (  tsp00_C5, 5);
typedef CSTRING_TYPE_MSP00      (  tsp00_C5c, 5);
typedef OLDPASCALSTRING_TYPE_MSP00 (  tsp00_C6, 6);
typedef CSTRING_TYPE_MSP00      (  tsp00_C6c, 6);
typedef OLDPASCALSTRING_TYPE_MSP00 (  tsp00_C8, 8);
typedef CSTRING_TYPE_MSP00      (  tsp00_C8c, 8);
typedef OLDPASCALSTRING_TYPE_MSP00 (  tsp00_C9, 9);
typedef CSTRING_TYPE_MSP00      (  tsp00_C9c, 9);
typedef OLDPASCALSTRING_TYPE_MSP00 (  tsp00_C10, 10);
typedef CSTRING_TYPE_MSP00      (  tsp00_C10c, 10);
typedef OLDPASCALSTRING_TYPE_MSP00 (  tsp00_C11, 11);
typedef CSTRING_TYPE_MSP00      (  tsp00_C11c, 11);
typedef OLDPASCALSTRING_TYPE_MSP00 (  tsp00_C12, 12);
typedef CSTRING_TYPE_MSP00      (  tsp00_C12c, 12);
typedef OLDPASCALSTRING_TYPE_MSP00 (  tsp00_C14, 14);
typedef CSTRING_TYPE_MSP00      (  tsp00_C14c, 14);
typedef OLDPASCALSTRING_TYPE_MSP00 (  tsp00_C15, 15);
typedef CSTRING_TYPE_MSP00      (  tsp00_C15c, 15);
typedef OLDPASCALSTRING_TYPE_MSP00 (  tsp00_C16, 16);
typedef CSTRING_TYPE_MSP00      (  tsp00_C16c, 16);
typedef OLDPASCALSTRING_TYPE_MSP00 (  tsp00_C18, 18);
typedef CSTRING_TYPE_MSP00      (  tsp00_C18c, 18);
typedef OLDPASCALSTRING_TYPE_MSP00 (  tsp00_C20, 20);
typedef CSTRING_TYPE_MSP00      (  tsp00_C20c, 20);
typedef OLDPASCALSTRING_TYPE_MSP00 (  tsp00_C24, 24);
typedef CSTRING_TYPE_MSP00      (  tsp00_C24c, 24);
typedef OLDPASCALSTRING_TYPE_MSP00 (  tsp00_C28, 28);
typedef CSTRING_TYPE_MSP00      (  tsp00_C28c, 28);
typedef OLDPASCALSTRING_TYPE_MSP00 (  tsp00_C30, 30);
typedef CSTRING_TYPE_MSP00      (  tsp00_C30c, 30);
typedef OLDPASCALSTRING_TYPE_MSP00 (  tsp00_C32, 32);
typedef CSTRING_TYPE_MSP00      (  tsp00_C32c, 32);
typedef OLDPASCALSTRING_TYPE_MSP00 (  tsp00_C34, 34);
typedef CSTRING_TYPE_MSP00      (  tsp00_C34c, 34);
typedef OLDPASCALSTRING_TYPE_MSP00 (  tsp00_C35, 35);
typedef CSTRING_TYPE_MSP00      (  tsp00_C35c, 35);
typedef OLDPASCALSTRING_TYPE_MSP00 (  tsp00_C36, 36);
typedef CSTRING_TYPE_MSP00      (  tsp00_C36c, 36);
typedef OLDPASCALSTRING_TYPE_MSP00 (  tsp00_C40, 40);
typedef CSTRING_TYPE_MSP00      (  tsp00_C40c, 40);
typedef OLDPASCALSTRING_TYPE_MSP00 (  tsp00_C42, 42);
typedef CSTRING_TYPE_MSP00      (  tsp00_C42c, 42);
typedef OLDPASCALSTRING_TYPE_MSP00 (  tsp00_C48, 48);
typedef CSTRING_TYPE_MSP00      (  tsp00_C48c, 48);
typedef OLDPASCALSTRING_TYPE_MSP00 (  tsp00_C50, 50);
typedef CSTRING_TYPE_MSP00      (  tsp00_C50c, 50);
typedef OLDPASCALSTRING_TYPE_MSP00 (  tsp00_C60, 60);
typedef CSTRING_TYPE_MSP00      (  tsp00_C60c, 60);
typedef OLDPASCALSTRING_TYPE_MSP00 (  tsp00_C64, 64);
typedef CSTRING_TYPE_MSP00      (  tsp00_C64c, 64);
typedef OLDPASCALSTRING_TYPE_MSP00 (  tsp00_C66, 66);
typedef CSTRING_TYPE_MSP00      (  tsp00_C66c, 66);
typedef OLDPASCALSTRING_TYPE_MSP00 (  tsp00_C70, 70);
typedef CSTRING_TYPE_MSP00      (  tsp00_C70c, 70);
typedef OLDPASCALSTRING_TYPE_MSP00 (  tsp00_C80, 80);
typedef CSTRING_TYPE_MSP00      (  tsp00_C80c, 80);
typedef OLDPASCALSTRING_TYPE_MSP00 (  tsp00_C120, 120);
typedef CSTRING_TYPE_MSP00      (  tsp00_C120c, 120);
typedef OLDPASCALSTRING_TYPE_MSP00 (  tsp00_C132, 132);
typedef CSTRING_TYPE_MSP00      (  tsp00_C132c, 132);
typedef OLDPASCALSTRING_TYPE_MSP00 (  tsp00_C256, 256);
typedef CSTRING_TYPE_MSP00      (  tsp00_C256c, 256);
typedef OLDPASCALSTRING_TYPE_MSP00 (  tsp00_C264, 264);
typedef CSTRING_TYPE_MSP00      (  tsp00_C264c, 264);
typedef OLDPASCALSTRING_TYPE_MSP00 (  tsp00_C512, 512);
typedef CSTRING_TYPE_MSP00      (  tsp00_C512c, 512);
    /**/
     #if defined (__cplusplus)
     typedef tsp00_Void tsp00_AnyPackedChar;
     typedef tsp00_Void tsp00_AnyPackedCharc;
     #else
typedef OLDPASCALSTRING_TYPE_MSP00 (  tsp00_AnyPackedChar, ANY_PACKED_CHAR_MXSP00);
typedef CSTRING_TYPE_MSP00      (  tsp00_AnyPackedCharc, ANY_PACKED_CHAR_MXSP00);
     #endif
typedef OLDPASCALSTRING_TYPE_MSP00 (  tsp00_Buf, BUF_MXSP00);
typedef CSTRING_TYPE_MSP00      (  tsp00_Bufc, BUF_MXSP00);
typedef OLDPASCALSTRING_TYPE_MSP00 (  tsp00_Buf8K, BUF8K_MXSP00);
typedef CSTRING_TYPE_MSP00      (  tsp00_Buf8Kc, BUF8K_MXSP00);
typedef tsp00_C24                  tsp00_CryptPw;
typedef tsp00_C24c                 tsp00_CryptPwc;
typedef tsp00_Uint1                tsp00_CodeType;
typedef tsp00_C64                  tsp00_CompName;
typedef tsp00_C64c                 tsp00_CompNamec;
typedef tsp00_C48                  tsp00_Copyright;
typedef tsp00_C48c                 tsp00_Copyrightc;
typedef tsp00_C256                 tsp00_Ctable;
typedef tsp00_C256c                tsp00_Ctablec;
typedef ARRAY_TYPE_MSP00 (         tsp00_CryptName, tsp00_Int4, csp_cryptname);
typedef tsp00_C8                   tsp00_Date;
typedef tsp00_C8c                  tsp00_Datec;
typedef tsp00_C18                  tsp00_DbName;
typedef tsp00_C18c                 tsp00_DbNamec;
typedef tsp00_C256                 tsp00_Path;
typedef tsp00_C256c                tsp00_Pathc;
typedef tsp00_C20                  tsp00_Decimal;
typedef tsp00_C20c                 tsp00_Decimalc;
typedef tsp00_C40                  tsp00_DevName;
typedef tsp00_C40c                 tsp00_DevNamec;
typedef tsp00_C40                  tsp00_ErrText;
typedef tsp00_C40c                 tsp00_ErrTextc;
typedef tsp00_C64                  tsp00_ExecProg;
typedef tsp00_C64c                 tsp00_ExecProgc;
typedef tsp00_C512                 tsp00_ExecArgLine;
typedef tsp00_C512c                tsp00_ExecArgLinec;
typedef tsp00_C512                 tsp00_ExecLine;
typedef tsp00_C512c                tsp00_ExecLinec;
typedef OLDPASCALSTRING_TYPE_MSP00 (  tsp00_ExtTimestamp, EXT_TIMESTAMP_MXSP00);
typedef CSTRING_TYPE_MSP00      (  tsp00_ExtTimestampc, EXT_TIMESTAMP_MXSP00);
typedef char                       tsp00_FieldRange;
typedef tsp00_C18                  tsp00_Identifier;
typedef tsp00_C18c                 tsp00_Identifierc;
typedef ARRAY_TYPE_MSP00 (         tsp00_Int2Buf, tsp00_Int2, BUF_MXSP00);
typedef tsp00_Int2                 tsp00_IsolationLevel;
typedef tsp00_C64                  tsp00_KnlIdentifier;
typedef tsp00_C64c                 tsp00_KnlIdentifierc;
typedef OLDPASCALSTRING_TYPE_MSP00 (  tsp00_Key, KEY_MXSP00);
typedef CSTRING_TYPE_MSP00      (  tsp00_Keyc, KEY_MXSP00);
typedef tsp00_C132                 tsp00_Line;
typedef tsp00_C132c                tsp00_Linec;
typedef tsp00_Int2                 tsp00_LinePositions;
typedef tsp00_C32                  tsp00_Lname;
typedef tsp00_C32c                 tsp00_Lnamec;
typedef ARRAY_TYPE_MSP00 (         tsp00_MoveInt2, tsp00_Int2, MOVEINT2_MXSP00);
     #if defined (__cplusplus)
     typedef tsp00_Void tsp00_MoveObj;
     typedef tsp00_Void tsp00_MoveObjc;
     #else
typedef OLDPASCALSTRING_TYPE_MSP00 (  tsp00_MoveObj, MOVEOBJ_MXSP00);
typedef CSTRING_TYPE_MSP00      (  tsp00_MoveObjc, MOVEOBJ_MXSP00);
     #endif
typedef tsp00_C18                  tsp00_Name;
typedef tsp00_C18c                 tsp00_Namec;
typedef tsp00_C64                  tsp00_NodeId;
typedef tsp00_C64c                 tsp00_NodeIdc;
typedef tsp00_C20                  tsp00_Number;
typedef tsp00_C20c                 tsp00_Numberc;
typedef tsp00_C64                  tsp00_NumStr;
typedef tsp00_C64c                 tsp00_NumStrc;
typedef tsp00_C64                  tsp00_Opmsg;
typedef tsp00_C64c                 tsp00_Opmsgc;
typedef OLDPASCALSTRING_TYPE_MSP00 (  tsp00_Page, PAGE_MXSP00);
typedef CSTRING_TYPE_MSP00      (  tsp00_Pagec, PAGE_MXSP00);
typedef tsp00_Int4                 tsp00_PageNo;
typedef tsp00_C256                 tsp00_PrtLine;
typedef tsp00_C256c                tsp00_PrtLinec;
typedef tsp00_C64                  tsp00_PrtName;
typedef tsp00_C64c                 tsp00_PrtNamec;
typedef tsp00_C18                  tsp00_Pw;
typedef tsp00_C18c                 tsp00_Pwc;
typedef tsp00_C8                   tsp00_Region;
typedef tsp00_C8c                  tsp00_Regionc;
typedef tsp00_Int4                 tsp00_RegionId;
typedef OLDPASCALSTRING_TYPE_MSP00 (  tsp00_ResNum, RESNUM_MXSP00);
typedef CSTRING_TYPE_MSP00      (  tsp00_ResNumc, RESNUM_MXSP00);
typedef OLDPASCALSTRING_TYPE_MSP00 (  tsp00_ScreenBuf, SCREEN_BUF_MXSP00);
typedef CSTRING_TYPE_MSP00      (  tsp00_ScreenBufc, SCREEN_BUF_MXSP00);
typedef tsp00_C12                  tsp00_Sname;
typedef tsp00_C12c                 tsp00_Snamec;
typedef tsp00_C5                   tsp00_SqlState;
typedef tsp00_C5c                  tsp00_SqlStatec;
typedef tsp00_Int4                 tsp00_TaskId;
typedef tsp00_C18                  tsp00_TermId;
typedef tsp00_C18c                 tsp00_TermIdc;
typedef tsp00_C8                   tsp00_Time;
typedef tsp00_C8c                  tsp00_Timec;
typedef tsp00_C20                  tsp00_Timestamp;
typedef tsp00_C20c                 tsp00_Timestampc;
typedef tsp00_C16                  tsp00_Trace;
typedef tsp00_C16c                 tsp00_Tracec;
typedef tsp00_C264                 tsp00_UnloadMemory;
typedef tsp00_C264c                tsp00_UnloadMemoryc;
typedef tsp00_C40                  tsp00_Version;
typedef tsp00_C40c                 tsp00_Versionc;
typedef tsp00_C256                 tsp00_VFilename;
typedef tsp00_C256c                tsp00_VFilenamec;
typedef SMALLSET_TYPE_MSP00 (      tsp00_VtAttrib, tsp00_VtMode_Enum, vt_last_mode);
typedef SET_TYPE_MSP00 (           tsp00_VtColors, tsp00_VtColor_Enum, vt_last_color, 2);
typedef tsp00_C8                   tsp00_VtLabel;
typedef tsp00_C8c                  tsp00_VtLabelc;
typedef SET_TYPE_MSP00 (           tsp00_VtKeys, tsp00_VtKey_Enum, vt_last_key, 10);
typedef tsp00_C80                  tsp00_VtMsg;
typedef tsp00_C80c                 tsp00_VtMsgc;
typedef SET_TYPE_MSP00 (           tsp00_WarningSet, tsp00_Warnings_Enum, warn15_user_defined_code, 2);
typedef tsp00_C32                  tsp00_XpKeyType;
typedef tsp00_C32c                 tsp00_XpKeyTypec;
typedef tsp00_C20                  tsp00_Zoned;
typedef tsp00_C20c                 tsp00_Zonedc;
    /**/
typedef tsp00_Trace                tsp_trace;
typedef tsp00_Tracec               tsp_tracec;
    /**/
typedef pasbool                   *tsp00_BoolAddr;
typedef tsp00_Buf                 *tsp00_BufAddr;
typedef tsp00_Key                 *tsp00_KeyAddr;
typedef tsp00_KnlIdentifier       *tsp00_KnlIdentifierPtr;
typedef tsp00_MoveObj             *tsp00_MoveObjPtr;
typedef tsp00_AnyPackedChar       *tsp00_ObjAddr;
typedef tsp00_Page                *tsp00_PageAddr;
typedef tsp00_ScreenBuf           *tsp00_ScreenBufAddr;
    /**/
typedef tsp00_BytePtr              tsp00_KeyPtr;
    /**/
typedef tsp00_C24                  tsp_c24;
typedef tsp00_C24c                 tsp_c24c;
typedef tsp00_C40                  tsp_c40;
typedef tsp00_C40c                 tsp_c40c;
typedef tsp00_C256                 tsp_c256;
typedef tsp00_C256c                tsp_c256c;
    /**/
typedef tsp00_Key                  tsp_key;
typedef tsp00_Keyc                 tsp_keyc;
typedef tsp00_TaskId               tsp_process_id;

typedef struct                     tsp00_TakCacheBuf
{
    union
    {
    /* true */
        struct tsp00_TakCacheBuf_true
        {
            tsp00_Int4          word_allignment_F;
        } C_true;
    /* false */
        struct tsp00_TakCacheBuf_false
        {
            ARRAY_VAR_MSP00 (         buf_F, tsp00_Buf, csp_cntcachebuf);
        } C_false;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tsp00_Int4         &word_allignment        ()  { return this->variant.C_true.word_allignment_F; };
    ARRAY_TYPESTRING_MSP00(tsp00_Buf, csp_cntcachebuf) &buf                    ()  { return this->variant.C_false.buf_F; };
    /* const accessor functions */
    const tsp00_Int4   &word_allignment        () const  { return this->variant.C_true.word_allignment_F; };
    const ARRAY_TYPESTRING_MSP00(tsp00_Buf, csp_cntcachebuf) &buf                    () const  { return this->variant.C_false.buf_F; };
#endif    /* defined (__cplusplus) */
} tsp00_TakCacheBuf;

typedef struct                     tsp00_IntMapC2
{
    union
    {
    /* TRUE */
        struct tsp00_IntMapC2_TRUE
        {
            tsp00_Int2          mapInt_sp00_F;
        } C_TRUE;
    /* FALSE */
        struct tsp00_IntMapC2_FALSE
        {
            tsp00_C2            mapC2_sp00_F;
        } C_FALSE;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tsp00_Int2         &mapInt_sp00            ()  { return this->variant.C_TRUE.mapInt_sp00_F; };
    tsp00_C2           &mapC2_sp00             ()  { return this->variant.C_FALSE.mapC2_sp00_F; };
    /* const accessor functions */
    const tsp00_Int2   &mapInt_sp00            () const  { return this->variant.C_TRUE.mapInt_sp00_F; };
    const tsp00_C2     &mapC2_sp00             () const  { return this->variant.C_FALSE.mapC2_sp00_F; };
#endif    /* defined (__cplusplus) */
} tsp00_IntMapC2;

typedef struct                     tsp_int_map_c2
{
    union
    {
    /* TRUE */
        struct tsp_int_map_c2_TRUE
        {
            tsp00_Int2          map_int_F;
        } C_TRUE;
    /* FALSE */
        struct tsp_int_map_c2_FALSE
        {
            tsp00_C2            map_c2_F;
        } C_FALSE;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tsp00_Int2         &map_int                ()  { return this->variant.C_TRUE.map_int_F; };
    tsp00_C2           &map_c2                 ()  { return this->variant.C_FALSE.map_c2_F; };
    /* const accessor functions */
    const tsp00_Int2   &map_int                () const  { return this->variant.C_TRUE.map_int_F; };
    const tsp00_C2     &map_c2                 () const  { return this->variant.C_FALSE.map_c2_F; };
#endif    /* defined (__cplusplus) */
} tsp_int_map_c2;

typedef struct                     tsp00_DataLine
{
    tsp00_Line          text;
    tsp00_LinePositions pos;
    tsp00_LinePositions length;
} tsp00_DataLine;

typedef struct                     tsp00_String
{
    tsp00_Line          text;
    tsp00_LinePositions length;
} tsp00_String;

typedef struct                     tsp00_Ccc
{
    tsp00_Int4          count;
} tsp00_Ccc;
typedef tsp00_Ccc                  tsp_ccc;

typedef struct                     tsp00_Numberstack
{
    integer             stackpointer;
    ARRAY_VAR_MSP00 (         elem, tsp00_Number, csp_numstack);
    ARRAY_VAR_MSP00 (         elem_err, tsp00_NumError, csp_numstack);
} tsp00_Numberstack;

typedef struct                     tsp00_SfieldInfo
{
    char                mode;
    char                io_type;
    char                d_type;
    char                length;
    union
    {
    /* true */
        struct tsp00_SfieldInfo_true
        {
            char                frac_F;
            char                in_out_l_F;
            tsp00_Int2          bufpos_F;
        } C_true;
    /* false */
        struct tsp00_SfieldInfo_false
        {
            tsp00_Int2          longiolen_F;
            tsp00_Int2          bufpos_1_F;
        } C_false;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    char               &frac                   ()  { return this->variant.C_true.frac_F; };
    char               &in_out_l               ()  { return this->variant.C_true.in_out_l_F; };
    tsp00_Int2         &bufpos                 ()  { return this->variant.C_true.bufpos_F; };
    tsp00_Int2         &longiolen              ()  { return this->variant.C_false.longiolen_F; };
    tsp00_Int2         &bufpos_1               ()  { return this->variant.C_false.bufpos_1_F; };
    /* const accessor functions */
    const char         &frac                   () const  { return this->variant.C_true.frac_F; };
    const char         &in_out_l               () const  { return this->variant.C_true.in_out_l_F; };
    const tsp00_Int2   &bufpos                 () const  { return this->variant.C_true.bufpos_F; };
    const tsp00_Int2   &longiolen              () const  { return this->variant.C_false.longiolen_F; };
    const tsp00_Int2   &bufpos_1               () const  { return this->variant.C_false.bufpos_1_F; };
#endif    /* defined (__cplusplus) */
} tsp00_SfieldInfo;
    /* records for communication between kernel and dialog */
    /* in case of stored procedures                 */

typedef struct                     tsp00_ModHeader
{
    tsp00_KnlIdentifier spmh_modn;
    tsp00_KnlIdentifier spmh_appln;
    tsp00_KnlIdentifier spmh_user;
    tsp00_Uint1         spmh_mtype;
    char                spmh_state;
    tsp00_Int2          spmh_index;
} tsp00_ModHeader;

typedef struct                     tsp00_KernelDca
{
    tsp00_BoolAddr      cancelb_ptr;
    tsp00_Int2          msgno;
    pasbool             first_call;
    pasbool             stop_msg_set;
    tsp00_KnlIdentifier curr_user;
    tsp00_KnlIdentifier curr_group;
    tsp00_KnlIdentifier owner_sysdba;
    tsp00_KnlIdentifier system_user;
    tsp00_KnlIdentifier user_system_user;
    tsp00_C256          stop_msg;
    tsp00_Sproc         proc_type;
    tsp00_C1            kernel_dca_filler;
    tsp00_Int2          mess_type;
    pasbool             firstcall;
    pasbool             lastcall;
    tsp00_Addr          value_p;
    tsp00_Int4          value_size;
    tsp00_Addr          code_p;
    tsp00_Int4          code_size;
} tsp00_KernelDca;

typedef struct                     tsp00_PlCodeInfo
{
    tsp00_Name          spplc_objtype;
    tsp00_KnlIdentifier spplc_owner;
    tsp00_KnlIdentifier spplc_progn;
    tsp00_KnlIdentifier spplc_modn;
    tsp00_Name          spplc_proglang;
    tsp00_Sname         spplc_subtype;
    tsp00_C4            spplc_version;
} tsp00_PlCodeInfo;
    /* End Dialog - Kernel */

typedef struct                     tsp00_RteCommRec
{
    pasbool             to_cancel;
    pasbool             rte_fill1;
    tsp00_Int2          virt_reads;
    tsp00_Int4          file_root;
    tsp00_Int4          file_record_cnt;
} tsp00_RteCommRec;
typedef tsp00_RteCommRec          *tsp00_RteCommAddr;
    /**/

typedef struct                     tsp00_RteRelocationRec
{
    tsp00_ObjAddr       rte_acv_pointer;
    tsp00_ObjAddr       rte_tak_pointer;
    tsp00_Int4          rte_relocationvalue;
    pasbool             rte_relocated;
    pasbool             rte_rel_fill;
    tsp00_Int2          rte_rel_fill1;
    tsp00_ObjAddr       rte_swap1_start;
    tsp00_Int4          rte_swap1_length;
    tsp00_ObjAddr       rte_swap2_start;
} tsp00_RteRelocationRec;
typedef tsp00_RteRelocationRec    *tsp00_RteRelocateAddr;
    /**/
    /* ******* virtual terminal ******* */
typedef ARRAY_TYPE_MSP00 (         tsp00_VtLonglabel, tsp00_VtLabel, csp_sk_lines);
typedef ARRAY_TYPE_MSP00 (         tsp00_VtLlabels, tsp00_VtLonglabel, csp_sk_number);
typedef char                       tsp00_VtLocalFunction;
typedef ARRAY_TYPE_MSP00 (         tsp00_VtSlabels, tsp00_VtLabel, csp_sk_number);
    /**/

typedef struct                     tsp00_TerminalDescription
{
    tsp00_ScreenBufAddr content_buf;
    tsp00_ScreenBufAddr attribute_buf;
    tsp00_Int2          num_of_lines;
    tsp00_Int2          num_of_cols;
    tsp00_VtColors      colors;
    pasbool             has_sysline;
    tsp00_SkLabel       labels;
    tsp00_VtKeys        returnkeys;
    tsp00_VtAttrib      attributes;
    pasbool             graphic_chars;
    pasbool             mark;
    pasbool             windows;
    tsp00_Dbcs          dbcs;
    tsp00_C1            so_char;
    tsp00_C1            si_char;
    pasbool             filler_1;
    tsp00_C2            filler_2;
} tsp00_TerminalDescription;

typedef struct                     tsp00_VtField
{
    tsp00_VtLabel       text;
    tsp00_Int2          lno;
    tsp00_Int2          colnr;
    tsp00_C1            att;
    pasbool             fill1;
    tsp00_Int2          fill2;
} tsp00_VtField;

typedef struct                     tsp00_HifParms
{
    tsp00_VtSlabels     slabels;
    tsp00_VtLlabels     llabels;
    tsp00_Line          sys_text;
    tsp00_Line          sys_attr;
    tsp00_VtField       insert_label;
} tsp00_HifParms;

typedef struct                     tsp00_VtKeyStroke
{
    tsp00_VtKey         key;
    pasbool             fill1;
    tsp00_Int2          fill2;
    union
    {
    /* vt_character_key */
        struct tsp00_VtKeyStroke_vt_character_key
        {
            char                ch_F;
        } C_vt_character_key;
    /* vt_hscroll */
        struct tsp00_VtKeyStroke_vt_hscroll
        {
            tsp00_Int2          scroll_dir_F;
        } C_vt_hscroll;
    /* vt_mouse_down */
        struct tsp00_VtKeyStroke_vt_mouse_down
        {
            tsp00_Int2          mouse_x_F;
            tsp00_Int2          mouse_y_F;
        } C_vt_mouse_down;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    char               &ch                     ()  { return this->variant.C_vt_character_key.ch_F; };
    tsp00_Int2         &scroll_dir             ()  { return this->variant.C_vt_hscroll.scroll_dir_F; };
    tsp00_Int2         &mouse_x                ()  { return this->variant.C_vt_mouse_down.mouse_x_F; };
    tsp00_Int2         &mouse_y                ()  { return this->variant.C_vt_mouse_down.mouse_y_F; };
    /* const accessor functions */
    const char         &ch                     () const  { return this->variant.C_vt_character_key.ch_F; };
    const tsp00_Int2   &scroll_dir             () const  { return this->variant.C_vt_hscroll.scroll_dir_F; };
    const tsp00_Int2   &mouse_x                () const  { return this->variant.C_vt_mouse_down.mouse_x_F; };
    const tsp00_Int2   &mouse_y                () const  { return this->variant.C_vt_mouse_down.mouse_y_F; };
#endif    /* defined (__cplusplus) */
} tsp00_VtKeyStroke;

typedef struct                     tsp00_VtOptions
{
    pasbool             wait_for_input;
    tsp00_VtUsageMode   usage_mode;
    pasbool             return_on_last;
    pasbool             return_on_first;
    tsp00_VtKeys        returnkeys;
    tsp00_VtKeys        reject_keys;
    pasbool             bell;
    tsp00_VtMark        mark;
    tsp00_Int2          first_line_changed;
    tsp00_Int2          last_line_changed;
    tsp00_Int2          screen2_first_changed;
    tsp00_Int2          screen2_last_changed;
    pasbool             overtype;
    pasbool             fill1;
} tsp00_VtOptions;

typedef struct                     tsp00_VtRectangle
{
    tsp00_Int2          top;
    tsp00_Int2          left;
    tsp00_Int2          bottom;
    tsp00_Int2          right;
} tsp00_VtRectangle;

typedef struct                     tsp00_VtResult
{
    tsp00_Int2          num_of_lines;
    tsp00_Int2          num_of_cols;
    tsp00_Int2          mark_begin;
    tsp00_Int2          mark_end;
    tsp00_VtKeyStroke   keystroke;
    pasbool             screen_changed;
    pasbool             vt_res_fill1;
    tsp00_Int2          vt_res_fill2;
} tsp00_VtResult;

typedef struct                     tsp00_VtSplitopt
{
    pasbool             maximized;
    pasbool             icon;
    tsp00_VtRectangle   pos;
} tsp00_VtSplitopt;
typedef tsp00_VtSplitopt          *tsp00_VtSplitoptaddr;
    /**/
    /* ******* virtual file ******* */
typedef ARRAY_TYPE_MSP00 (         tsp00_VfBuf, tsp00_Page, csp_pool_buffers);
typedef tsp00_VfBuf               *tsp00_VfBufaddr;
    /**/
typedef tsp00_VfBuf               *tsp_vf_bufaddr;
    /* ******* disk io ******* */
typedef tsp00_AnyPackedChar       *tsp00_Maxiobufaddr;
    /**/
    /* ******* VGETSORT ******* */

typedef struct                     tsp00_DecimalPresentation
{
    char                thousand_token;
    char                zero_point;
    tsp00_Int2          dec_pres_filler;
} tsp00_DecimalPresentation;
typedef tsp00_DecimalPresentation  tsp_decimal_presentation;

typedef struct                     tsp00_XpValueRec
{
    tsp00_Int2          xp_info;
    pasbool             xp_fill1;
    tsp00_XpKey         xp_value_t;
    union
    {
    /* xp_int2_type */
        struct tsp00_XpValueRec_xp_int2_type
        {
            tsp00_Int2          xp_int2_F;
        } C_xp_int2_type;
    /* xp_int4_type */
        struct tsp00_XpValueRec_xp_int4_type
        {
            tsp00_Int4          xp_int4_F;
        } C_xp_int4_type;
    /* xp_c8_type */
        struct tsp00_XpValueRec_xp_c8_type
        {
            tsp00_C8            xp_c8_F;
        } C_xp_c8_type;
    /* xp_c18_type */
        struct tsp00_XpValueRec_xp_c18_type
        {
            tsp00_C18           xp_c18_F;
        } C_xp_c18_type;
    /* xp_c24_type */
        struct tsp00_XpValueRec_xp_c24_type
        {
            tsp00_C24           xp_c24_F;
        } C_xp_c24_type;
    /* xp_c40_type */
        struct tsp00_XpValueRec_xp_c40_type
        {
            tsp00_C40           xp_c40_F;
        } C_xp_c40_type;
    /* xp_c64_type */
        struct tsp00_XpValueRec_xp_c64_type
        {
            tsp00_C64           xp_c64_F;
        } C_xp_c64_type;
    /* xp_crypt_type */
        struct tsp00_XpValueRec_xp_crypt_type
        {
            tsp00_CryptName     xp_crypt_F;
        } C_xp_crypt_type;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tsp00_Int2         &xp_int2                ()  { return this->variant.C_xp_int2_type.xp_int2_F; };
    tsp00_Int4         &xp_int4                ()  { return this->variant.C_xp_int4_type.xp_int4_F; };
    tsp00_C8           &xp_c8                  ()  { return this->variant.C_xp_c8_type.xp_c8_F; };
    tsp00_C18          &xp_c18                 ()  { return this->variant.C_xp_c18_type.xp_c18_F; };
    tsp00_C24          &xp_c24                 ()  { return this->variant.C_xp_c24_type.xp_c24_F; };
    tsp00_C40          &xp_c40                 ()  { return this->variant.C_xp_c40_type.xp_c40_F; };
    tsp00_C64          &xp_c64                 ()  { return this->variant.C_xp_c64_type.xp_c64_F; };
    tsp00_CryptName    &xp_crypt               ()  { return this->variant.C_xp_crypt_type.xp_crypt_F; };
    /* const accessor functions */
    const tsp00_Int2   &xp_int2                () const  { return this->variant.C_xp_int2_type.xp_int2_F; };
    const tsp00_Int4   &xp_int4                () const  { return this->variant.C_xp_int4_type.xp_int4_F; };
    const tsp00_C8     &xp_c8                  () const  { return this->variant.C_xp_c8_type.xp_c8_F; };
    const tsp00_C18    &xp_c18                 () const  { return this->variant.C_xp_c18_type.xp_c18_F; };
    const tsp00_C24    &xp_c24                 () const  { return this->variant.C_xp_c24_type.xp_c24_F; };
    const tsp00_C40    &xp_c40                 () const  { return this->variant.C_xp_c40_type.xp_c40_F; };
    const tsp00_C64    &xp_c64                 () const  { return this->variant.C_xp_c64_type.xp_c64_F; };
    const tsp00_CryptName &xp_crypt               () const  { return this->variant.C_xp_crypt_type.xp_crypt_F; };
#endif    /* defined (__cplusplus) */
} tsp00_XpValueRec;
    /*------------------------------------*/
    /*      QUERY/REPORT interface        */
    /* for Precompiler, EASY and DIALOG   */
    /*------------------------------------*/

typedef struct                     tsp00_OnlineHeader
{
    tsp00_C8            id_field;
    tsp00_C8            relno_field;
    tsp00_C12           mode_field;
    tsp00_C40           text_field;
} tsp00_OnlineHeader;

typedef struct                     tsp00_ReportValues
{
    tsp00_Number        count;
    union
    {
    /* true */
        struct tsp00_ReportValues_true
        {
            tsp00_Number        sum_F;
            tsp00_Number        minimum_F;
            tsp00_Number        maximum_F;
            tsp00_Number        val1_F;
            tsp00_Number        val2_F;
            tsp00_Number        val3_F;
            tsp00_Number        val4_F;
        } C_true;
    /* false */
        struct tsp00_ReportValues_false
        {
            tsp00_C40           min_string_F;
            tsp00_C40           max_string_F;
        } C_false;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tsp00_Number       &sum                    ()  { return this->variant.C_true.sum_F; };
    tsp00_Number       &minimum                ()  { return this->variant.C_true.minimum_F; };
    tsp00_Number       &maximum                ()  { return this->variant.C_true.maximum_F; };
    tsp00_Number       &val1                   ()  { return this->variant.C_true.val1_F; };
    tsp00_Number       &val2                   ()  { return this->variant.C_true.val2_F; };
    tsp00_Number       &val3                   ()  { return this->variant.C_true.val3_F; };
    tsp00_Number       &val4                   ()  { return this->variant.C_true.val4_F; };
    tsp00_C40          &min_string             ()  { return this->variant.C_false.min_string_F; };
    tsp00_C40          &max_string             ()  { return this->variant.C_false.max_string_F; };
    /* const accessor functions */
    const tsp00_Number &sum                    () const  { return this->variant.C_true.sum_F; };
    const tsp00_Number &minimum                () const  { return this->variant.C_true.minimum_F; };
    const tsp00_Number &maximum                () const  { return this->variant.C_true.maximum_F; };
    const tsp00_Number &val1                   () const  { return this->variant.C_true.val1_F; };
    const tsp00_Number &val2                   () const  { return this->variant.C_true.val2_F; };
    const tsp00_Number &val3                   () const  { return this->variant.C_true.val3_F; };
    const tsp00_Number &val4                   () const  { return this->variant.C_true.val4_F; };
    const tsp00_C40    &min_string             () const  { return this->variant.C_false.min_string_F; };
    const tsp00_C40    &max_string             () const  { return this->variant.C_false.max_string_F; };
#endif    /* defined (__cplusplus) */
} tsp00_ReportValues;

typedef struct                     tsp00_ReportFunctionResult
{
    tsp00_Int4          rf_used_count;
    ARRAY_VAR_MSP00 (         rf_column_id, tsp00_Int4, 16);
    OLDPASCALSTRING_VAR_MSP00 (  rf_column_type, 16);
    ARRAY_VAR_MSP00 (         rf_column_result, tsp00_ReportValues, 16);
} tsp00_ReportFunctionResult;

typedef struct                     tsp00_ReportInterface
{
    tsp00_OnlineHeader  dp_frame_header;
    tsp00_Int2          dp_e_size;
    tsp00_Int2          dp_e_length;
    tsp00_DbName        dp_dbname;
    tsp00_KnlIdentifier dp_res_name;
    tsp00_Int4          dp_resultcount;
    tsp00_Int4          dp_rowno;
    tsp00_Int4          dp_colno;
    tsp00_Int2          dp_sql_rc;
    tsp00_Int2          dp_count;
    tsp00_ReportFunctionResult dp_funct_vals;
} tsp00_ReportInterface;

typedef struct                     tsp00_QrPrePacket
{
    tsp00_Buf           qrp_buf;
    tsp00_ReportInterface qrp_parms;
    tsp00_Int2          qrp_retcode;
    tsp00_Int2          qrp_mtype;
} tsp00_QrPrePacket;
    /**/

typedef enum tsp00_LdInfo__Enum
{
    ld_ex_trigger,
    ld_with_lock,
    ld_no_close,
    ld_new_rec,
    ld_is_comment,
    ld_is_catalog,
    ld_unicode,
    ld_without_shortcol
} tsp00_LdInfo__Enum;
typedef tsp00_LdInfo__Enum tsp00_LdInfo__Param;
typedef SMALLSET_TYPE_MSP00 (      tsp00_LdInfo, tsp00_LdInfo__Enum, ld_without_shortcol);
    /**/

typedef enum tsp00_LdState__Enum
{
    ld_stream,
    ld_use_conversion,
    ld_use_toascii,
    ld_use_ucs_2_swap,
    ld_short_scol,
    ld_first_insert,
    ld_copy,
    ld_first_call
} tsp00_LdState__Enum;
typedef tsp00_LdState__Enum tsp00_LdState__Param;
typedef SMALLSET_TYPE_MSP00 (      tsp00_LdState, tsp00_LdState__Enum, ld_first_call);

typedef struct                     tsp00_LongDescriptor
{
    union
    {
    /* true */
        struct tsp00_LongDescriptor_true
        {
            pasbool             ld_just_for_compile_help_F;
        } C_true;
    /* false */
        struct tsp00_LongDescriptor_false
        {
            tsp00_C8            ld_descriptor_F;
            tsp00_C8            ld_tabid_F;
            tsp00_Int4          ld_maxlen_F;
            tsp00_Int4          ld_intern_pos_F;
            tsp00_LdInfo        ld_infoset_F;
            tsp00_LdState       ld_state_F;
            tsp00_C1            ld_used_in_ak_F;
            tsp00_ValMode       ld_valmode_F;
            tsp00_Int2          ld_valind_F;
            tsp00_Int2          ld_unused_2_F;
            tsp00_Int4          ld_valpos_F;
            tsp00_Int4          ld_vallen_F;
        } C_false;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    pasbool            &ld_just_for_compile_help ()  { return this->variant.C_true.ld_just_for_compile_help_F; };
    tsp00_C8           &ld_descriptor          ()  { return this->variant.C_false.ld_descriptor_F; };
    tsp00_C8           &ld_tabid               ()  { return this->variant.C_false.ld_tabid_F; };
    tsp00_Int4         &ld_maxlen              ()  { return this->variant.C_false.ld_maxlen_F; };
    tsp00_Int4         &ld_intern_pos          ()  { return this->variant.C_false.ld_intern_pos_F; };
    tsp00_LdInfo       &ld_infoset             ()  { return this->variant.C_false.ld_infoset_F; };
    tsp00_LdState      &ld_state               ()  { return this->variant.C_false.ld_state_F; };
    tsp00_C1           &ld_used_in_ak          ()  { return this->variant.C_false.ld_used_in_ak_F; };
    tsp00_ValMode      &ld_valmode             ()  { return this->variant.C_false.ld_valmode_F; };
    tsp00_Int2         &ld_valind              ()  { return this->variant.C_false.ld_valind_F; };
    tsp00_Int2         &ld_unused_2            ()  { return this->variant.C_false.ld_unused_2_F; };
    tsp00_Int4         &ld_valpos              ()  { return this->variant.C_false.ld_valpos_F; };
    tsp00_Int4         &ld_vallen              ()  { return this->variant.C_false.ld_vallen_F; };
    /* const accessor functions */
    const pasbool      &ld_just_for_compile_help () const  { return this->variant.C_true.ld_just_for_compile_help_F; };
    const tsp00_C8     &ld_descriptor          () const  { return this->variant.C_false.ld_descriptor_F; };
    const tsp00_C8     &ld_tabid               () const  { return this->variant.C_false.ld_tabid_F; };
    const tsp00_Int4   &ld_maxlen              () const  { return this->variant.C_false.ld_maxlen_F; };
    const tsp00_Int4   &ld_intern_pos          () const  { return this->variant.C_false.ld_intern_pos_F; };
    const tsp00_LdInfo &ld_infoset             () const  { return this->variant.C_false.ld_infoset_F; };
    const tsp00_LdState &ld_state               () const  { return this->variant.C_false.ld_state_F; };
    const tsp00_C1     &ld_used_in_ak          () const  { return this->variant.C_false.ld_used_in_ak_F; };
    const tsp00_ValMode &ld_valmode             () const  { return this->variant.C_false.ld_valmode_F; };
    const tsp00_Int2   &ld_valind              () const  { return this->variant.C_false.ld_valind_F; };
    const tsp00_Int2   &ld_unused_2            () const  { return this->variant.C_false.ld_unused_2_F; };
    const tsp00_Int4   &ld_valpos              () const  { return this->variant.C_false.ld_valpos_F; };
    const tsp00_Int4   &ld_vallen              () const  { return this->variant.C_false.ld_vallen_F; };
#endif    /* defined (__cplusplus) */
} tsp00_LongDescriptor;
    /**/

typedef enum tsp00_LdbChange__Enum
{
    ldb_use_termchar,
    ldb_use_conversion,
    ldb_use_toascii,
    ldb_use_ucs_2_swap
} tsp00_LdbChange__Enum;
typedef tsp00_LdbChange__Enum tsp00_LdbChange__Param;
typedef SMALLSET_TYPE_MSP00 (      tsp00_LdbChange, tsp00_LdbChange__Enum, ldb_use_ucs_2_swap);

typedef enum tsp00_ldb_infoset__Enum
{
    ldb_ex_trigger,
    ldb_with_lock,
    ldb_no_close,
    ldb_new_rec,
    ldb_is_comment,
    ldb_is_catalog,
    ldb_unicode
} tsp00_ldb_infoset__Enum;
typedef tsp00_ldb_infoset__Enum tsp00_ldb_infoset__Param;

typedef struct                     tsp00_LongDescBlock
{
    tsp00_C8            ldb_descriptor;
    tsp00_C2            ldb_serverdb_no;
    tsp00_LdbChange     ldb_change;
    pasbool             ldb_replicated;
    tsp00_Int4          ldb_curr_pageno;
    tsp00_Int2          ldb_curr_pos;
    tsp00_C1            ldb_colno;
    tsp00_C1            ldb_show_kind;
    tsp00_Int4          ldb_intern_pos;
    SMALLSET_VAR_MSP00 (      ldb_infoset, tsp00_ldb_infoset__Enum, ldb_unicode);
    pasbool             ldb_filler1;
    tsp00_C1            ldb_filler1a;
    tsp00_ValMode       ldb_valmode;
    tsp00_Int2          ldb_valind;
    tsp00_Int2          ldb_oldvallen;
    tsp00_Int4          ldb_valpos;
    tsp00_Int4          ldb_vallen;
    tsp00_Int4          ldb_full_len;
    tsp00_C4            ldb_filler2;
    tsp00_Int4          ldb_last_i_pos;
    tsp00_Int4          ldb_root_check;
} tsp00_LongDescBlock;

typedef struct                     tsp00_ParamSpec
{
    char                para_type;
    char                para_length;
    union
    {
    /* true */
        struct tsp00_ParamSpec_true
        {
            char                para_frac_F;
            char                filler_F;
        } C_true;
    /* false */
        struct tsp00_ParamSpec_false
        {
            tsp00_Int2          para_long_length_F;
        } C_false;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    char               &para_frac              ()  { return this->variant.C_true.para_frac_F; };
    char               &filler                 ()  { return this->variant.C_true.filler_F; };
    tsp00_Int2         &para_long_length       ()  { return this->variant.C_false.para_long_length_F; };
    /* const accessor functions */
    const char         &para_frac              () const  { return this->variant.C_true.para_frac_F; };
    const char         &filler                 () const  { return this->variant.C_true.filler_F; };
    const tsp00_Int2   &para_long_length       () const  { return this->variant.C_false.para_long_length_F; };
#endif    /* defined (__cplusplus) */
} tsp00_ParamSpec;
typedef ARRAY_TYPE_MSP00 (         tsp00_ParamSpecList, tsp00_ParamSpec, csp_maxvariables);

typedef struct                     tsp_dcom_init_sink_parms
{
    tsp00_Addr          acv_addr;
    tsp00_Addr          sink_addr;
    tsp00_Int4          sink_size;
} tsp_dcom_init_sink_parms;

typedef struct                     tsp_dcom_co_create_instance_parms
{
    tsp00_Addr          library_name;
    tsp00_C8            package_id;
    tsp00_C32           MD5_footprint;
    tsp00_C16           coclsid;
    tsp00_C16           iid;
    tsp00_Addr          ifactory;
    tsp00_Addr          idispatch;
    tsp00_Addr          ikernel_sink;
    tsp00_Addr          session_context;
    tsp00_C4            session;
    tsp00_C4            trans;
    tsp00_Int4          ret;
    tsp00_C64           errtext;
    pasbool             use_libname;
    pasbool             force_run_in_kernel;
    pasbool             run_in_ude_server;
} tsp_dcom_co_create_instance_parms;

typedef struct                     tsp_dcom_dispatch_parms
{
    tsp00_Addr          idispatch;
    tsp00_Addr          ifactory;
    tsp00_Addr          session_context;
    tsp00_C4            session;
    tsp00_MoveObjPtr    param_stack;
    tsp00_Int4          param_stack_len;
    tsp00_Int4          dispid;
    tsp00_Int4          hresult;
    tsp00_C4            trans;
    tsp00_Int2          monitor_slot_id;
    pasbool             monitoring;
    tsp00_C64           method_name;
    tsp00_C64           errtext;
    pasbool             run_in_ude_server;
    tsp00_C16           coclsid;
    tsp00_C16           iid;
    tsp00_Addr          library_name;
    tsp00_C32           MD5_footprint;
} tsp_dcom_dispatch_parms;

typedef struct                     tsp_dcom_release_instance_parms
{
    tsp00_Addr          session_context;
    tsp00_C4            session;
    tsp00_C4            trans;
    tsp00_Int4          hresult;
} tsp_dcom_release_instance_parms;

typedef struct                     tsp_dcom_paraminfo_parms
{
    tsp00_Addr          library_name;
    tsp00_C64           prog_id;
    tsp00_KnlIdentifier method;
    tsp00_C8            package_id;
    tsp00_C32           MD5_footprint;
    tsp00_C16           coclsid;
    tsp00_C16           iid;
    tsp00_C16           udttypeguid;
    tsp00_MoveObjPtr    packet;
    tsp00_Addr          typeinfo_ptr;
    tsp00_Addr          session_context;
    tsp00_Int4          dispid;
    tsp00_Int4          const_dispid;
    tsp00_C4            session;
    tsp00_C4            trans;
    tsp00_Int2          const_type;
    tsp00_Int2          const_typeid;
    integer             func_idx;
    pasbool             use_libname;
    tsp00_DbObjectType  db_obj_type;
    tsp00_C64           errtext;
} tsp_dcom_paraminfo_parms;
    /* +++ FF Change Request 1000139  +++ */

typedef struct                     tsp_dcom_monitor_parms
{
    tsp00_Addr          session_handle;
    tsp00_Int4          retcode;
    tsp00_Addr          idispatch;
    tsp00_C16           iid;
    tsp00_C4            session;
} tsp_dcom_monitor_parms;

struct tsp00_TemplateDummy {
    tsp00_XpReturnCode   dummy_tsp00_XpReturnCode;
    tsp00_UniError       dummy_tsp00_UniError;
    tsp00_VfType         dummy_tsp00_VfType;
    tsp00_ExecMode       dummy_tsp00_ExecMode;
    tsp00_Default        dummy_tsp00_Default;
    tsp00_PascalComponentID dummy_tsp00_PascalComponentID;
    tsp00_VtKey          dummy_tsp00_VtKey;
    tsp00_VfResource     dummy_tsp00_VfResource;
    tsp00_ToolLayer      dummy_tsp00_ToolLayer;
    tsp00_XpKey          dummy_tsp00_XpKey;
    tsp00_Warnings       dummy_tsp00_Warnings;
    tsp00_VpLinefeeds    dummy_tsp00_VpLinefeeds;
    tsp00_VtScroll       dummy_tsp00_VtScroll;
    tsp00_Wlisttype      dummy_tsp00_Wlisttype;
    tsp00_DbObjectType   dummy_tsp00_DbObjectType;
    tsp00_LcompResult    dummy_tsp00_LcompResult;
    tsp00_VfReturn       dummy_tsp00_VfReturn;
    tsp00_VtUsageMode    dummy_tsp00_VtUsageMode;
    tsp00_NumError       dummy_tsp00_NumError;
    tsp00_SwapKind       dummy_tsp00_SwapKind;
    tsp00_Sproc          dummy_tsp00_Sproc;
    tsp00_SqlMode        dummy_tsp00_SqlMode;
    tsp00_VtColor        dummy_tsp00_VtColor;
    tsp00_VfFormat       dummy_tsp00_VfFormat;
    tsp00_DlgBoxStyle    dummy_tsp00_DlgBoxStyle;
    tsp00_VaReturn       dummy_tsp00_VaReturn;
    tsp00_ExecReturn     dummy_tsp00_ExecReturn;
    tsp00_Dbcs           dummy_tsp00_Dbcs;
    tsp00_VtMark         dummy_tsp00_VtMark;
    tsp00_DatabaseState  dummy_tsp00_DatabaseState;
    tsp00_VFileOpCodes   dummy_tsp00_VFileOpCodes;
    tsp00_DataType       dummy_tsp00_DataType;
    tsp00_VtMode         dummy_tsp00_VtMode;
    tsp00_ValMode        dummy_tsp00_ValMode;
    tsp00_Os             dummy_tsp00_Os;
    tsp00_SkLabel        dummy_tsp00_SkLabel;
    tsp00_ColumnType     dummy_tsp00_ColumnType;
    tsp00_NetError       dummy_tsp00_NetError;
    tsp00_ColMode        dummy_tsp00_ColMode;
    tsp00_ShutdownMode   dummy_tsp00_ShutdownMode;
    tsp00_Service        dummy_tsp00_Service;
    tsp00_TaskState      dummy_tsp00_TaskState;
    tsp00_DgcallbackResult dummy_tsp00_DgcallbackResult;
    tsp00_VtKeys         dummy_tsp00_VtKeys;
    tsp00_VtColors       dummy_tsp00_VtColors;
    tsp00_LdbChange      dummy_tsp00_LdbChange;
    tsp00_LdState        dummy_tsp00_LdState;
    tsp00_WarningSet     dummy_tsp00_WarningSet;
    tsp00_VtAttrib       dummy_tsp00_VtAttrib;
    tsp00_LdInfo         dummy_tsp00_LdInfo;
};


#endif
