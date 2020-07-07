
/* some common prolog for every module
 */

#ifndef VPR00_H
#define VPR00_H


/******************************************
 *            I N C L U D E S             *
 ******************************************/


#ifndef GSP00_2_H
#include "gsp00_2.h"
#endif

#ifndef GSP00_H
#include "gsp00.h"
#endif

#ifndef VSP001_H
#include "vsp001.h"
#endif

#ifndef VSP004_H
#include "vsp004.h"
#endif

#ifndef VSP008_H
#include "vsp008.h"
#endif

/******************************************
 *           C O N S T A N T S            *
 ******************************************/

    /*****************************************/
    /* tokens for the lexical analysis */
    /* 1. tokens for all precompilers */
#define cpr_s_unknown                           0
#define cpr_s_colon                             1
#define cpr_s_comma                             2
#define cpr_s_divide                            3
#define cpr_s_eof                               4
#define cpr_s_equal                             5
#define cpr_s_fixed_point_literal               6
#define cpr_s_floating_point_literal            7
#define cpr_s_identifier                        8
#define cpr_s_leftindpar                        9
#define cpr_s_leftpar                          10
#define cpr_s_macro                            11
#define cpr_s_minus                            12
#define cpr_s_parameter_name                   13
#define cpr_s_greater                          14
#define cpr_s_plus                             15
#define cpr_s_point                            16
#define cpr_s_rightindpar                      17
#define cpr_s_rightpar                         18
#define cpr_s_semicolon                        19
#define cpr_s_asterisk                         20
#define cpr_s_string_literal                   21
#define cpr_s_unsigned_integer                 22
#define cpr_s_bufeof                           23
#define cpr_s_cstring_literal                  24
#define cpr_s_question_mark                    25
#define cpr_sqldbcount_one              "\xC1\x10\x00\x00\x00\x00\x00\x00\x00\x00"
#define cpr_parsidnull                  "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
#define cpr_nullchar                   '\x00'
#define cpr_ht_code                    '\x09'    /* tabulator          */
#define cpr_leftind_par                '['    /* eckige klammer auf */
#define cpr_rightind_par               ']'    /* eckige klammer zu  */
#define cpr_backslash                  '\\'
#define cpr_pc_endsign                 '!'
#define cpr_recordchr                  '!'
#define cpr_recordchr2                 '~'    /* tilde */
#define cpr_recordchr3                 '|'    /* senkrechter strich */
#define cpr_macrochr                   '%'
#define cpr_paramchr                   ':'
#define cpr_preparechr                 '?'    /* ?? */
#define cpr_warningchr                 'W'
#define cpr_dquosym                    '\"'
#define cpr_quotsym                    '\''
#define cpr_asterisk                   '*'
#define cpr_cbegin_par                 '{'    /* geschweifte klammer auf */
#define cpr_cend_par                   '}'    /* geschweifte klammer zu  */
#define cpr_numblen_int2                        5    /* int2 : fixed len    */
#define cpr_numblen_int4                       10    /* int4 : fixed len    */
#define cpr_numblen_int8                       18    /* int8 : fixed len    */
#define cpr_numblen_r4                          6    /* real4: float len    */
#define cpr_numblen_r8                         15    /* real8: float len    */
#define cpr_result_xuser_read_err               1
#define cpr_result_system_required              2
#define cpr_result_too_many_users               3
#define cpr_result_user_passw                   4
#define cpr_result_whenever_stop                5
#define cpr_result_option_error                 6
#define cpr_result_trace_error                  7
#define cpr_result_sqlxa_invalid                8
    /* ------------tpr00_TraceParseID_Enum---------------- */
#define TraceParseIDInput_epr00                 1
#define TraceParseIDOutput_epr00                2
#define TraceParseIDChange_epr00                3
#define TraceParseIDNew_epr00                   4
#define TraceParseIDSQLStmt_epr00               5
#define TraceParseIDSQLStmtBlank_epr00          6
#define TraceParseIDUseExecute_epr00            7
#define TraceParseIDFetch_epr00                 8
#define TraceParseIDBlank_epr00                 9
#define TraceParseIDAgain_epr00                10
#define TraceParseIDVersion_epr00              11
#define TraceParseIDSelect_epr00               12
#define TraceParseIDDrop_epr00                 13
#define TraceParseIDFromCache_epr00            14
#define TraceParseIDSQLStmtMass_epr00          15
#define TraceParseIDOther_epr00                99
#define cpr_minint4                    (- 2147483647)
#define cpr_caidconst                   "SQLCA   "
#define cpr_daidconst                   "SQLDA   "
#define cpr_ghatzelcheck                "Hatzel-Check"    /* Ha ha, Hahahaha */
#define cpr_lhatzelcheck                "Check_Hatzel"    /* Ho ho, Hohohoho */
#define cpr_n_parameter                 "PARAMETER                       "
    /*----------------------------------------------------------*/
    /*   constante werden im precomilelauf errechnet muss >= 1  */
    /*----------------------------------------------------------*/
#define mxpr_partbuf                        16000    /* connect kommando buffer */
#define mxpr_sqlva1                         10000    /* host-variables addr */
#define mxpr_sqlva2                          5000    /* variable datatypes */
#define mxpr_sqlva3                         10000    /* variable names */
#define mxpr_sqlpa                           5000    /* parameter */
#define mxpr_sqlsa                            500    /* sequencs  */
#define mxpr_sqlka                           3000    /* kommandos */
#define mxpr_sqlpr                            500    /* prepare   */
#define mxpr_sqlst                           1000    /* statement */
#define mxpr_sqlfa                            200    /* fetch statements */
#define mxpr_sqlsd                             20    /* select direct fast commands */
#define mxpr_sqln                             300    /* anz sqlvar*/
#define mxpr_sqlfn                             50    /* anz include-files im modul  */
#define mxpr_sqlat                            500    /* at-area, oracle at db-cmd */
#define mxpr_sqlcu                            200    /* cursor-area  */
#define mxpr_sqlsn                            100    /* snrsor-area  */
    /*------------------------------------------*/
    /* Konstanten f?ur neu parsid rel 3.0.00    */
    /*------------------------------------------*/
#define cpr_p_precom_index                     11
#define mxpr_knlparsid                         12    /* release 3.0..  */
#define mxpr_parsid                    (mxpr_knlparsid + 4)    /* release 7.1..  */
#define mxpr_errorloop                         10    /* bei error -1,anz wiederholungen */
#define mxpr_sqlline                           52    /* mod 4 */
#define mxpr_macroline                        126
#define mxpr_sqlga                              8    /* anzahl session  */
#define mxpr_sqlma                            128    /* mocros    */
#define mxpr_partkind                          36    /* vpi00c sqlmaxpartkind */
#define mxpr_numb                               6
#define mxpr_errtext                           70
#define mxpr_preline                          512    /* mod 4 */
#define mxpr_value                            254
#define mxpr_mfetch                            18
#define mxpr_mfetchbuffer                      18
#define mxpr_mfetchrecords                    250
#define mxpr_descrlvc                        2000
#define mxpr_sysprof_stmlen                  1950
#define mxpr_buffer                          8488    /* old tsp00_MoveObj */
#define cpr_undefindicator             (- 1)
#define cpr_overflowindicator          (- 2)
#define cpr_defaultparam               (- 3)
#define cpr_kind_empty                          0
#define cpr_kind_internal                       1
#define cpr_kind_db2                            2
#define cpr_kind_ansi                           3
#define cpr_kind_oracle                         4
#define cpr_kind_sapr3                          5
#define cpr_single_byte                         0
#define cpr_multi_sjis                          1
#define cpr_multi_euc                           2
    /*------------------------------------------*/
    /* Konstanten fuer sapdb_errors  und       */
    /* Konstanten fuer sapdb_messagetypen      */
    /*------------------------------------------*/
#define cpr_pointerlen                          0
#define cpr_file_or_table_droped       (- 108)
#define cpr_err_cmd_not_available      (- 101)
#define cpr_database_crash                    800
    /* messagetypen nicht im precompiler erlaubt */
    /* parsid  [5]  wert */
#define cpr_m_nolog                            36    /* Thomas A. fragen */
    /*------------------------------------------*/
    /* Konstanten f?ur enumeration typen        */
    /*------------------------------------------*/
#define cpr_tr_inp                              0    /* trace text */
#define cpr_tr_outp                             1
#define cpr_tr_loop                             2
#define cpr_tr_longvar                          3
#define cpr_tr_arrcnt                           4
#define cpr_is_empty                   (- 1)    /* tsp00_Int2 */
#define cpr_is_false                            0
#define cpr_is_true                             1    /* tsp00_Int2 */
#define cpr_is_init                             2
#define cpr_is_describe                         3
#define cpr_is_loop_cnt                         4    /* 1.entry for loop sa-entries.sastruc*/
#define cpr_is_loop                             5    /* next sa-enties.sastruc for loop */
#define cpr_is_first                            6    /* clock mess time begin           */
#define cpr_is_end                              7    /* clock mess time end             */
#define cpr_is_only_pc                          8    /* prof.time without sqldbkerneltime*/
#define cpr_is_statement                        9    /* hostvarname is statementname */
#define cpr_vstruct                    (- 1)
#define cpr_vint2                               0    /* tsp00_Int2 */
#define cpr_vint4                               1
#define cpr_vreal4                              2
#define cpr_vreal8                              3
#define cpr_vdecimal                            4
#define cpr_vzoned                              5
#define cpr_vchar                               6
#define cpr_vcharc                              7
#define cpr_vbchar                              8
#define cpr_vdbchar                             9
#define cpr_vbuf                               10
#define cpr_vscalar                            11
#define cpr_vintarr                            12
#define cpr_vrec                               13
#define cpr_vnone                              14
#define cpr_vstring                            15
#define cpr_vuns2                              16
#define cpr_vuns4                              17
#define cpr_vdecrec                            18
#define cpr_varray                             19
#define cpr_vstring1                           20
#define cpr_vdbcharc                           21
#define cpr_vlzone                             22
#define cpr_vlszon                             23
#define cpr_vtszon                             24
#define cpr_vnumber                            25    /* visual basic */
#define cpr_vbyte                              26    /* visual basic */
#define cpr_vdbyte                             27    /* visual basic */
#define cpr_vansicharc                         28    /* ansi char for C with blanks */
#define cpr_vreal8sun                          29    /* real pascal sun */
#define cpr_vstringc                           30
#define cpr_vansichar                          31    /* ansi char */
#define cpr_vr3charnull                        32    /* R3 intern datentyp */
    /* only in sqlda, char with len=0 */
#define cpr_vint8                              33
#define cpr_vuns8                              34
#define cpr_vstring4                           35
#define cpr_viaddr                             36
#define cpr_vraw                               37
#define cpr_vrawc                              38
#define cpr_vfile                              39    /* datafile Cobol */
#define cpr_vfilec                             40    /* datafile C */
#define cpr_vunicode                           41    /* unicode   */
#define cpr_vunicodec                          42    /* unicode C */
#define cpr_vcharz                             43
#define cpr_vonumber                           44
#define cpr_vovarnum                           45
#define cpr_vodate                             46
#define cpr_vabaphandle                        47
#define cpr_vvarchar_without_len               48
#define cpr_vint1                              49
#define cpr_vuns1                              50
#define cpr_vlong_desc                         51
#define cpr_vucs2                              52
#define cpr_vutf16                             53
#define cpr_vstringunicode                     54    /* ora-varchar  contains UCS2 */
#define cpr_vstringunicode4                    55    /* ora-varchar4 contains UCS2 */
    /*------------------------------------------*/
    /* Konstanten Oracle Datentypen             */
    /*     nur in vpr00c                        */
    /*------------------------------------------*/
    /*------------------------------------------*/
    /* Konstanten f?ur include Dateien describe */
    /*------------------------------------------*/
#define cpr_info_input                          0    /* colio */
#define cpr_info_output                         1
#define cpr_info_inout                          2
    /* festgelegte dataentypen des precompilers */
#define cpr_vshort                     cpr_vint2    /* c */
#define cpr_vunsshort                  cpr_vuns2    /* c */
#define cpr_vlong                      cpr_vint4    /* c */
#define cpr_vunslong                   cpr_vuns4    /* c */
#define cpr_llong                      cpr_numblen_int4    /* c */
#define cpr_lunslong                   cpr_numblen_int4    /* c */
#define cpr_vint                       cpr_vint4    /* c */
#define cpr_vunsigned                  cpr_vuns4    /* c */
#define cpr_lint                       cpr_numblen_int4    /* c */
#define cpr_lunsigned                  cpr_numblen_int4    /* c */
#define cpr_vinteger                   cpr_vint4    /* pascal */
#define cpr_linteger                   cpr_numblen_int4    /* pascal */
#define cpr_vfloat                     cpr_vreal4    /* c */
#define cpr_vdouble                    cpr_vreal8    /* c */
#define cpr_vreal                      cpr_vreal4    /* f77 */
#define cpr_lshort                     cpr_numblen_int2    /* c */
#define cpr_lunsshort                  cpr_numblen_int2    /* c */
#define cpr_lfloat                     cpr_numblen_r4    /* c */
#define cpr_ldouble                    cpr_numblen_r8    /* c */
#define cpr_trace_empty                         0    /* tsp00_Int2 */
#define cpr_trace_off                           1
#define cpr_trace_formal                        2
#define cpr_trace_long                          3
#define cpr_trace_modformal                     4
#define cpr_trace_modlong                       5
#define cpr_com_empty                           0    /* command type */
#define cpr_com_sql                             1
#define cpr_com_sql_conn                        2    /* sql connect statement */
#define cpr_com_cancel                          3    /* sql cancel  statement */
#define cpr_com_option                          4
#define cpr_com_sql_close                       5
#define cpr_com_crea_ind                        6
#define cpr_com_sql_getval                      7    /* exex sql getval command */
#define cpr_com_sql_open                        8    /* c-i exec sql open statement */
#define cpr_com_commit_release                  9    /* call interface */
#define cpr_com_describe                       10    /* call interface */
#define cpr_com_fetch_describe                 11    /* call interface */
#define cpr_com_mfetch                         12    /* call interface */
#define cpr_com_mfetch_describe                13    /* call interface */
#define cpr_com_exec                           14    /* intermediate code (of pass1) */
#define cpr_com_cics                           15    /* cics command found           */
#define cpr_com_command                        16    /* exec command                 */
#define cpr_com_unused                         17
#define cpr_com_proc                           18    /* exec proc                    */
#define cpr_com_opt                            19    /* command line options */
#define cpr_com_sql_putval                     20    /* exec sql putval command */
#define cpr_com_set_format                     21    /* set format rel3.0  */
#define cpr_com_declare_cursor                 22    /* declare cursor for ?? as param*/
#define cpr_com_sql_fetch                      23    /* exec sql fetch command        */
#define cpr_com_ora_descr_sel                  24    /* oracle desc sel.list */
#define cpr_com_oracle                         25    /* exec oracle          */
#define cpr_com_trace                          26
#define cpr_com_sql_open_keep                  27    /* exec sql open stmt. with keep opt. */
#define cpr_com_commit                         28
#define cpr_com_rollback                       29
#define cpr_com_rollback_release               30
#define cpr_com_abort_session                  31
#define cpr_in_sqlva                            0    /* tsp00_Int2  */
#define cpr_in_sqlst                            1
#define cpr_state_empty                         0    /* commandostate */
#define cpr_state_parse                         1
#define cpr_state_execute                       2
#define cpr_state_command                       3
#define cpr_state_prepare                       4
#define cpr_state_macro                         5
#define cpr_state_trace                         6
#define cpr_state_sync_exec                     7
#define cpr_state_newsync_exec                  8
#define cpr_state_async_exec                    9
#define cpr_state_decl_statem                  10
#define cpr_state_decl_with_hold               11
#define cpr_state_execute_at_fetch             12
#define cpr_ci_implicit                         0    /* connectype    */
#define cpr_ci_program                          1
#define cpr_ci_connect                          2
#define cpr_ci_ora_connect                      3
#define cpr_ci_explicit                         4
#define cpr_co_ok                               0    /* tsp00_Int2 */
#define cpr_co_dbms_start_required              1
#define cpr_co_too_many_users                   2
#define cpr_co_restart_required                 3
#define cpr_co_logon_required                   4
#define cpr_co_user_or_pswd_illegal             5
#define cpr_co_crash                            6
#define cpr_co_timeout                          7
#define cpr_co_implicit                         8
#define cpr_co_user_op                          9
#define cpr_se_empty                            0    /* tsp00_Int2 */
#define cpr_se_primary                          1
#define cpr_se_second                           2
#define CPR_CMD_STATE_INIT                      0
#define CPR_CMD_STATE_START                     1
#define CPR_CMD_STATE_READY                     2
#define CPR_CMD_STATE_INFLIGHT                  3
#define CPR_CMD_STATE_END                       4
#define cpr_fe_empty                            0    /* no fetch stm */
#define cpr_fe_fetch                            1    /* normal fetch */
#define cpr_fe_first                            2    /* fetch first  */
#define cpr_fe_next                             3    /* fetch next   */
#define cpr_fe_last                             4    /* fetch last   */
#define cpr_fe_prev                             5    /* fetch prev   */
#define cpr_fe_same                             6    /* fetch same   */
#define cpr_fe_posc                             7    /* fetch pos const*/
#define cpr_fe_posv                             8    /* fetch pos variable */
#define cpr_fe_relc                             9    /* fetch relative const*/
#define cpr_fe_relv                            10    /* fetch relative variable */
#define cpr_lo_iso_default             (- 1)    /* option default */
#define cpr_lo_empty                   (- 1)    /* tsp00_Int2 empty */
#define cpr_lo_isolev0                          0    /* isolation level */
#define cpr_lo_isolev1                          1
#define cpr_lo_isolev2                          2
#define cpr_lo_isolev3                          3    /* ansi default */
#define cpr_lo_isolev4                          4    /* alt  */
#define cpr_lo_isolev9                          9
#define cpr_lo_isolev10                        10    /* normal, sql-db default */
#define cpr_lo_isolev15                        15    /* implicit */
#define cpr_lo_isolev20                        20    /* explicit */
#define cpr_lo_isolev30                        30
#define cpr_dt_empty                            0    /* sqldb date time format */
#define cpr_dt_normal                           1
#define cpr_dt_iso                              2
#define cpr_dt_usa                              3
#define cpr_dt_eur                              4
#define cpr_dt_jis                              5
#define cpr_dt_ansi                             6
#define cpr_lzu_connect                         1    /* fehler analyse von sql_result */
#define cpr_lzu_request                         2
#define cpr_lzu_receive                         3
#define cpr_lzu_release                         4
#define cpr_lzu_sysprofile                      5
#define cpr_lzu_cancel                          6
    /*----------------------------------------------------------*/
    /*  errors f?ur call_interface                              */
    /*----------------------------------------------------------*/
#define cpr_c_ok                                0
#define cpr_c_incompatible                      1
#define cpr_c_overflow                          2
#define cpr_c_truncated                         3
#define cpr_c_varnowrong                        4
#define cpr_c_notimplemented                    5
#define cpr_la_all                              0    /* all pascal runtime */
#define cpr_la_call                             1
#define cpr_la_c                                3
#define cpr_la_cobol                            4
#define cpr_la_odbc                             5
#define cpr_la_oci                              6
#define cpr_la_cobmic                           7
#define cpr_la_cob8860                          8
#define cpr_la_pascalvax                        9
#define cpr_la_pascal                          10
#define cpr_la_pascallpi                       11
#define cpr_la_pascal31                        12
    /* Konstanten fuer CPC-Code-Erzeugung */
    /* sqlva1-area -entry's */
#define sqlvasc                                 0    /* scalar */
#define sqlvacm                                 1    /* componente */
#define sqlvast                                 2    /* struct */
#define sqlvapt                                 3    /* pointer */
    /* sqlpar-area -entry's */
#define sqlparlo                                0    /* loop */
#define sqlparst                                1    /* struct */
#define sqlparel                                2    /* entry */
#define CPR_ERROR_ABAP_DESC_NOT_SET    (- 861)
#define CPR_ERROR_ABAP_EXCEPTION       (- 862)
#define CPR_ERROR_CONNECTION_BUSY      (- 810)
#define cpr_error_session_not_connected (- 821)
#define cpr_error_invalid_option       (- 863)
#define Version61_cpr00                     60100
#define Version62_cpr00                     60200
#define Version71_cpr00                     70100
#define Version714cpr00                     70104
#define Version72_cpr00                     70200
#define Version721_cpr00                    70201
#define Version722_cpr00                    70202
#define Version724_cpr00                    70204
#define Version725_cpr00                    70205
#define Version730_cpr00                    70300
#define Version731_cpr00                    70301
#define Version740_cpr00                    70400
#define Version741_cpr00                    70401
#define Version742_cpr00                    70402
    /* Kernel Versions */
#define ActualKernelVersion_cpr00      Version742_cpr00
#define MinKernelVersion_cpr00         Version62_cpr00
#define SelDirFast_KernelVersion_cpr00 Version72_cpr00
#define MaxLenOnBlob_KernelVersion_cpr00 Version722_cpr00
    /* CPC Runtime Versions */
#define Actual_CPCVersion_cpr00        Version742_cpr00
#define Min_CPCVersion_cpr00           Version731_cpr00
#define BlobSupport_CPCVersion_cpr00   Version722_cpr00
    /* tpr00_DescribeKindEnum */
#define Describe_Columns_epr00                  1
#define Describe_Params_epr00                   2
#define Describe_Both_epr00                     3

/******************************************
 *               T Y P E S                *
 ******************************************/

typedef tsp00_Int4                 tpr_intaddr;
typedef tsp00_Int4                 tpr_longint;
typedef integer                    tpr_cmd_state_Enum;
typedef OLDPASCALSTRING_TYPE_MSP00 (  tpr_parsid, mxpr_parsid);
typedef CSTRING_TYPE_MSP00      (  tpr_parsidc, mxpr_parsid);
typedef tpr_parsid                 tpr00_ParseId;
typedef tpr_parsidc                tpr00_ParseIdc;
typedef tsp00_Int2                 tpr00_sp1rFunctionCode;

typedef struct                     tpr00_ParseInfo
{
    tpr00_ParseId       ParseId;
    tpr00_sp1rFunctionCode sp1rFunctionCode;
} tpr00_ParseInfo;
typedef OLDPASCALSTRING_TYPE_MSP00 (  tpr00_KnlSessionID, 4);
typedef CSTRING_TYPE_MSP00      (  tpr00_KnlSessionIDc, 4);

typedef struct                     tpr00_SessionID
{
    tpr00_KnlSessionID  KnlSession;
    tsp00_Int4          ClientSession;
} tpr00_SessionID;
typedef tsp00_Name                 tpr_password;
typedef OLDPASCALSTRING_TYPE_MSP00 (  tpr_sqlline, mxpr_sqlline);
typedef CSTRING_TYPE_MSP00      (  tpr_sqllinec, mxpr_sqlline);
typedef OLDPASCALSTRING_TYPE_MSP00 (  tpr_macroline, mxpr_macroline);
typedef CSTRING_TYPE_MSP00      (  tpr_macrolinec, mxpr_macroline);
typedef tsp00_C64                  tpr_hostname;
    /**   tpr_buffer     = PACKED ARRAY [ 1..mxpr_buffer] OF char; **/
typedef tsp00_MoveObj              tpr_buffer;
typedef tsp00_MoveObj             *tpr_stm_bufferp;
typedef OLDPASCALSTRING_TYPE_MSP00 (  tpr_vvalue, mxpr_value);
typedef CSTRING_TYPE_MSP00      (  tpr_vvaluec, mxpr_value);
typedef tsp00_Int2                 tpr_symbol;

typedef struct                     tpr_scannertype
{
    tsp00_Int4          syposacc;
    tsp00_Int4          sypos;
    tsp00_Int4          sylen;
    tpr_symbol          symb;
    tsp00_Int2          syfill;
} tpr_scannertype;

typedef enum tpr_runtime_errors_Enum
{
    cpr_p_ok,
    cpr_area_overflow,
    cpr_cmd_not_available,
    cpr_cmd_too_long,
    cpr_could_not_load_lib,
    cpr_connect_syntax_wrong,
    cpr_conv_from_unicode_impossible,
    cpr_conv_to_unicode_impossible,
    cpr_no_long_columns_found,
    cpr_datafile_close_error,
    cpr_datafile_open_error,
    cpr_datafile_read_error,
    cpr_datafile_write_error,
    cpr_db_session_not_allowed,
    cpr_differ_sqlmode_not_allowed,
    cpr_duplicate_fetch,
    cpr_duplicate_fetch_callif,
    cpr_errorno_from_sysprofile,
    cpr_ffetch_fetch_mix_not_allowed,
    cpr_ffetch_not_allowed,
    cpr_implicit_connect_wrong,
    cpr_incompatible_datatype,
    cpr_inp_default_not_allowed,
    cpr_inp_null_not_allowed,
    cpr_inp_number_invalid,
    cpr_inp_number_overflow,
    cpr_inp_number_truncated,
    cpr_inp_string_truncated,
    cpr_inp_string_truncated_oracle,
    cpr_invalid_commandstate,
    cpr_invalid_declare_command,
    cpr_invalid_escape,
    cpr_invalid_program,
    cpr_macro_not_init,
    cpr_memory_allocation_faild,
    cpr_missing_previous_fetch,
    cpr_missing_user_passw,
    cpr_missing_variable_addr,
    cpr_multibyteset_not_allowed,
    cpr_no_session_connected,
    cpr_not_implemented,
    cpr_out_null_not_allowed,
    cpr_out_number_invalid,
    cpr_out_number_overflow,
    cpr_output_part_too_short,
    cpr_paramlist_not_allowed,
    cpr_paramlist_too_long,
    cpr_paramlist_too_short,
    cpr_paramlist_wrong,
    cpr_r_loop_init_wrong,
    cpr_r_sql_cancelled,
    cpr_r_too_many_statements,
    cpr_reflex_connect_notok,
    cpr_reflex_crash,
    cpr_reflex_notok,
    cpr_reflex_shutdown,
    cpr_reflex_start_required,
    cpr_reflex_timeout,
    cpr_request_area_overflow,
    cpr_rmissing_indicatorvariable,
    cpr_rt_no_output_variable,
    cpr_select_parsid_missing,
    cpr_session_already_connected,
    cpr_set_format_error,
    cpr_too_many_fetch_statements,
    cpr_too_many_hostvar,
    cpr_too_many_hostvar_callif,
    cpr_too_many_users,
    cpr_tracefile_close_error,
    cpr_tracefile_open_error,
    cpr_tracefile_write_error,
    cpr_trace_error,
    cpr_unknown_datatype,
    cpr_unknown_statement_name,
    cpr_unknown_sysprofile,
    cpr_unterminated_c_string,
    cpr_use_call_stm,
    cpr_valbuf_too_short,
    cpr_xuser_file_open_error,
    cpr_abap_desc_not_set,
    cpr_abap_exception,
    cpr_connection_busy,
    cpr_session_not_connected,
    cpr_invalid_option,
    cpr_unknown_cursor_name
} tpr_runtime_errors_Enum;
typedef tpr_runtime_errors_Enum tpr_runtime_errors_Param;
typedef ENUM_TYPE_MSP00 (          tpr_runtime_errors, tpr_runtime_errors_Enum, cpr_unknown_cursor_name, unsigned char);
    /*******************************************/
    /* Descriptor for dynamic allocated memory */
    /*   neu 10.5.94  descriptor zur Area-     */
    /*  Behandlung  (siehe vpr03c,p03dynalloc) */
    /*******************************************/

typedef struct                     tpr_sqlmdesc
{
    tsp00_Int4          descMaxelem;
    tsp00_Int4          descNelem;
    tsp00_Int4          descElemSize;
    tsp00_Int4          descIncr;
    tpr_intaddr         descElemPtr;
} tpr_sqlmdesc;
    /******************************************/
    /*        V A L U E R E C                 */
    /*                                        */
    /* Variant-Record, der zum Austausch der  */
    /* Parameterwerte zwischen Hostprogramm   */
    /* und der Pascal-Subroutine des Precom-  */
    /* piler-Programms dient, in der SQLDB-    */
    /* Auftraege erzeugt und abgeschickt      */
    /* werden.                                */
    /* Austausch erfolgt ueber Adresse der    */
    /* Hostvariablen                          */
    /******************************************/
typedef char                       tpr_vvvtype;

typedef struct                     tpr_valuerec
{
    union
    {
    /* cpr_vint2 */
        struct tpr_valuerec_cpr_vint2
        {
            tsp00_Int2          i2_F;
        } C_cpr_vint2;
    /* cpr_vint4 */
        struct tpr_valuerec_cpr_vint4
        {
            tsp00_Int4          i4_F;
        } C_cpr_vint4;
    /* cpr_vreal4 */
        struct tpr_valuerec_cpr_vreal4
        {
            tsp00_Shortreal     r4_F;
        } C_cpr_vreal4;
    /* cpr_vreal8 */
        struct tpr_valuerec_cpr_vreal8
        {
            tsp00_Longreal      r8_F;
        } C_cpr_vreal8;
    /* cpr_vreal8sun */
        struct tpr_valuerec_cpr_vreal8sun
        {
            tsp00_Longreal      r8s_F;
        } C_cpr_vreal8sun;
    /* cpr_vdecimal */
        struct tpr_valuerec_cpr_vdecimal
        {
            tsp00_Decimal       dec_F;
        } C_cpr_vdecimal;
    /* cpr_vzoned */
        struct tpr_valuerec_cpr_vzoned
        {
            tsp00_Zoned         zon_F;
        } C_cpr_vzoned;
    /* cpr_vchar */
        struct tpr_valuerec_cpr_vchar
        {
            tpr_buffer          ch_F;
        } C_cpr_vchar;
    /* cpr_vbuf */
        struct tpr_valuerec_cpr_vbuf
        {
            tpr_buffer          buf_F;
        } C_cpr_vbuf;
    /* cpr_viaddr */
        struct tpr_valuerec_cpr_viaddr
        {
            tpr_intaddr         iaddr_F;
        } C_cpr_viaddr;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tsp00_Int2         &i2                     ()  { return this->variant.C_cpr_vint2.i2_F; };
    tsp00_Int4         &i4                     ()  { return this->variant.C_cpr_vint4.i4_F; };
    tsp00_Shortreal    &r4                     ()  { return this->variant.C_cpr_vreal4.r4_F; };
    tsp00_Longreal     &r8                     ()  { return this->variant.C_cpr_vreal8.r8_F; };
    tsp00_Longreal     &r8s                    ()  { return this->variant.C_cpr_vreal8sun.r8s_F; };
    tsp00_Decimal      &dec                    ()  { return this->variant.C_cpr_vdecimal.dec_F; };
    tsp00_Zoned        &zon                    ()  { return this->variant.C_cpr_vzoned.zon_F; };
    tpr_buffer         &ch                     ()  { return this->variant.C_cpr_vchar.ch_F; };
    tpr_buffer         &buf                    ()  { return this->variant.C_cpr_vbuf.buf_F; };
    tpr_intaddr        &iaddr                  ()  { return this->variant.C_cpr_viaddr.iaddr_F; };
    /* const accessor functions */
    const tsp00_Int2   &i2                     () const  { return this->variant.C_cpr_vint2.i2_F; };
    const tsp00_Int4   &i4                     () const  { return this->variant.C_cpr_vint4.i4_F; };
    const tsp00_Shortreal &r4                     () const  { return this->variant.C_cpr_vreal4.r4_F; };
    const tsp00_Longreal &r8                     () const  { return this->variant.C_cpr_vreal8.r8_F; };
    const tsp00_Longreal &r8s                    () const  { return this->variant.C_cpr_vreal8sun.r8s_F; };
    const tsp00_Decimal &dec                    () const  { return this->variant.C_cpr_vdecimal.dec_F; };
    const tsp00_Zoned  &zon                    () const  { return this->variant.C_cpr_vzoned.zon_F; };
    const tpr_buffer   &ch                     () const  { return this->variant.C_cpr_vchar.ch_F; };
    const tpr_buffer   &buf                    () const  { return this->variant.C_cpr_vbuf.buf_F; };
    const tpr_intaddr  &iaddr                  () const  { return this->variant.C_cpr_viaddr.iaddr_F; };
#endif    /* defined (__cplusplus) */
} tpr_valuerec;
typedef tpr_valuerec              *tpr_ptr_to_value;

typedef struct                     tpr_vtypepointer
{
    union
    {
    /* true */
        struct tpr_vtypepointer_true
        {
            tpr_intaddr         intaddr_F;
        } C_true;
    /* false */
        struct tpr_vtypepointer_false
        {
            tpr_ptr_to_value    vtypep_F;
        } C_false;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tpr_intaddr        &intaddr                ()  { return this->variant.C_true.intaddr_F; };
    tpr_ptr_to_value   &vtypep                 ()  { return this->variant.C_false.vtypep_F; };
    /* const accessor functions */
    const tpr_intaddr  &intaddr                () const  { return this->variant.C_true.intaddr_F; };
    const tpr_ptr_to_value &vtypep                 () const  { return this->variant.C_false.vtypep_F; };
#endif    /* defined (__cplusplus) */
} tpr_vtypepointer;
    /**************************************/
    /*        S Q L D A                   */
    /* Descriptor    Area                 */
    /* Datentype f?ur describe Kommandos   */
    /* Variablen ?uber diesen type werden  */
    /* declariert und dynamisch belegt.   */
    /**************************************/

typedef struct                     tpr_sqlrow
{
    tsp00_Int4          ireclen;
    tsp00_Int4          oreclen;
} tpr_sqlrow;

typedef struct                     tpr_sqlcol
{
    tsp00_DataType      coltype;
    tsp1_param_opt_set  colmode;
    tsp1_param_io_type  colio;
    tsp00_Uint1         colfrac;
    tsp00_Int4          collen;
    tsp00_Int4          colpos;
    tsp00_Int2          coliolen;
    pasbool             colisnum;
    pasbool             colisuni;
    pasbool             colislong;
    tsp00_Int2          colvartype;
    tsp00_Int2          colindtype;
    tpr_intaddr         coltobuf;
    tpr_intaddr         colfrombuf;
    tpr_intaddr         coltoindi;
    tpr_intaddr         colfromindi;
} tpr_sqlcol;

typedef struct                     sqlvartype
{
    tsp00_KnlIdentifier colname;
    tsp00_Int2          colio;
    tsp1_param_opt_set  colmode;
    tsp00_C3            colfill1;
    tsp00_Int2          coltype;
    tsp00_Int4          collength;
    tsp00_Int2          colfrac;
    tsp00_Int2          colfill2;
    tsp00_Int2          hostvartype;
    tsp00_Int2          hostcolsize;
    tsp00_Int4          hostindicator;
    tpr_vtypepointer    hostvaraddr;
    tpr_vtypepointer    hostindaddr;
    tpr_sqlcol          sqlcoldb;
} sqlvartype;

typedef struct                     sqldatype
{
    OLDPASCALSTRING_VAR_MSP00 (  sqldaid, 8);
    tsp00_Int4          sqlmax;
    tsp00_Int2          sqln;
    tsp00_Int2          sqld;
    tsp00_Int4          sqlloop;
    tsp00_Int4          sqloffset;
    tsp00_Int2          sqlkano;
    tsp00_Int2          sqlprno;
    tsp00_Int2          sqlkamode;
    tsp00_Int2          sqlfiller;
    tpr_sqlrow          sqlrow;
    ARRAY_VAR_MSP00 (         sqlvar, sqlvartype, mxpr_sqln);
} sqldatype;
typedef sqldatype                 *sqldapointer;
    /**************************************/
    /*        S Q L D A  for  D B 2 /S A A*/
    /* Descriptor    Area                 */
    /* Datentype f?ur describe Kommandos   */
    /* Variablen ?uber diesen type werden  */
    /* declariert und dynamisch belegt.   */
    /**************************************/

typedef struct                     sqlvardb2type
{
    tsp00_Int2          sqltype;
    tsp00_Int2          sqllen;
    tpr_vtypepointer    sqldata;
    tpr_vtypepointer    sqlind;
    tsp00_Int2          sqlnamel;
    tsp00_C30           sqlname;
    tpr_sqlcol          sqlcoldb;
} sqlvardb2type;

typedef struct                     sqldb2type
{
    OLDPASCALSTRING_VAR_MSP00 (  sqldaid, 8);
    tsp00_Int4          sqldabc;
    tsp00_Int2          sqln;
    tsp00_Int2          sqld;
    tsp00_Int4          sqlloop;
    tsp00_Int4          sqloffset;
    tsp00_Int2          sqlkano;
    tsp00_Int2          sqlprno;
    tsp00_Int2          sqlkamode;
    tsp00_Int2          sqlfiller;
    tpr_sqlrow          sqlrow;
    ARRAY_VAR_MSP00 (         sqlvardb2, sqlvardb2type, mxpr_sqln);
} sqldb2type;
typedef sqldb2type                *sqldb2pointer;
    /**************************************/
    /*        S Q L D A                   */
    /* Descriptor    Area   O R A C L E   */
    /* Datentype f?ur describe Kommandos   */
    /* Variablen ?uber diesen type werden  */
    /* declariert und dynamisch belegt.   */
    /**************************************/
typedef ARRAY_TYPE_MSP00 (         tpr_arr_int2, tsp00_Int2, mxpr_sqln);
typedef ARRAY_TYPE_MSP00 (         tpr_arr_int4, tsp00_Int4, mxpr_sqln);
typedef ARRAY_TYPE_MSP00 (         tpr_arr_ptrv, tpr_vtypepointer, mxpr_sqln);
typedef ARRAY_TYPE_MSP00 (         tpr_arr_dbcolinfo, tpr_sqlcol, 1);
typedef tpr_arr_ptrv              *tpr_arr_ptrvp;
typedef tpr_arr_int2              *tpr_arr_int2p;
typedef tpr_arr_int4              *tpr_arr_int4p;
typedef tpr_arr_dbcolinfo         *tpr_arr_sqlcolp;

typedef struct                     tpr_sqlcolpointer
{
    union
    {
    /* true */
        struct tpr_sqlcolpointer_true
        {
            tpr_intaddr         intaddr_F;
        } C_true;
    /* false */
        struct tpr_sqlcolpointer_false
        {
            tpr_arr_sqlcolp     sqlcolp_F;
        } C_false;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tpr_intaddr        &intaddr                ()  { return this->variant.C_true.intaddr_F; };
    tpr_arr_sqlcolp    &sqlcolp                ()  { return this->variant.C_false.sqlcolp_F; };
    /* const accessor functions */
    const tpr_intaddr  &intaddr                () const  { return this->variant.C_true.intaddr_F; };
    const tpr_arr_sqlcolp &sqlcolp                () const  { return this->variant.C_false.sqlcolp_F; };
#endif    /* defined (__cplusplus) */
} tpr_sqlcolpointer;

typedef struct                     sqloratype
{
    OLDPASCALSTRING_VAR_MSP00 (  sqldaid, 8);
    tsp00_Int4          sqldabc;
    tsp00_Int4          sqlfill;
    tsp00_Int4          sqlloop;
    tsp00_Int4          sqloffset;
    tsp00_Int2          sqlkano;
    tsp00_Int2          sqlprno;
    tsp00_Int2          sqlkamode;
    tsp00_Int2          sqlfiller;
    tpr_sqlrow          sqlrow;
    tpr_sqlcolpointer   sqlcoldb;
    tsp00_Int4          ora_n;
    tpr_arr_ptrvp       ora_v;
    tpr_arr_int4p       ora_l;
    tpr_arr_int2p       ora_t;
    tpr_arr_ptrvp       ora_i;
    tsp00_Int4          ora_f;
    tpr_arr_ptrvp       ora_s;
    tpr_arr_int2p       ora_m;
    tpr_arr_int2p       ora_c;
    tpr_arr_ptrvp       ora_x;
    tpr_arr_int2p       ora_y;
    tpr_arr_int2p       ora_z;
    tpr_arr_ptrvp       ora_fm;
    tpr_arr_int2p       ora_fml;
    tpr_arr_int2p       ora_fclp;
    tpr_arr_int2p       ora_fcrc;
} sqloratype;
typedef sqloratype                *sqlorapointer;
    /*********************************************/
    /*        S Q L D A V R E C    and           */
    /*        S Q L V A R V R E C                */
    /*  Varient record for sqlda in              */
    /*    call interface                         */
    /*********************************************/

typedef struct                     sqlvarvrec
{
    union
    {
    /* true */
        struct sqlvarvrec_true
        {
            sqlvartype          sqldbv_F;
        } C_true;
    /* false */
        struct sqlvarvrec_false
        {
            sqlvardb2type       db2v_F;
        } C_false;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    sqlvartype         &sqldbv                 ()  { return this->variant.C_true.sqldbv_F; };
    sqlvardb2type      &db2v                   ()  { return this->variant.C_false.db2v_F; };
    /* const accessor functions */
    const sqlvartype   &sqldbv                 () const  { return this->variant.C_true.sqldbv_F; };
    const sqlvardb2type &db2v                   () const  { return this->variant.C_false.db2v_F; };
#endif    /* defined (__cplusplus) */
} sqlvarvrec;
typedef char                       tpr_kind;

typedef struct                     sqldaptrvrec
{
    union
    {
    /* cpr_kind_internal */
        struct sqldaptrvrec_cpr_kind_internal
        {
            sqldapointer        sqldaptr_F;
        } C_cpr_kind_internal;
    /* cpr_kind_db2 */
        struct sqldaptrvrec_cpr_kind_db2
        {
            sqldb2pointer       db2daptr_F;
        } C_cpr_kind_db2;
    /* cpr_kind_oracle */
        struct sqldaptrvrec_cpr_kind_oracle
        {
            sqlorapointer       oradaptr_F;
        } C_cpr_kind_oracle;
    /* cpr_kind_empty */
        struct sqldaptrvrec_cpr_kind_empty
        {
            tpr_intaddr         intaddr_F;
        } C_cpr_kind_empty;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    sqldapointer       &sqldaptr               ()  { return this->variant.C_cpr_kind_internal.sqldaptr_F; };
    sqldb2pointer      &db2daptr               ()  { return this->variant.C_cpr_kind_db2.db2daptr_F; };
    sqlorapointer      &oradaptr               ()  { return this->variant.C_cpr_kind_oracle.oradaptr_F; };
    tpr_intaddr        &intaddr                ()  { return this->variant.C_cpr_kind_empty.intaddr_F; };
    /* const accessor functions */
    const sqldapointer &sqldaptr               () const  { return this->variant.C_cpr_kind_internal.sqldaptr_F; };
    const sqldb2pointer &db2daptr               () const  { return this->variant.C_cpr_kind_db2.db2daptr_F; };
    const sqlorapointer &oradaptr               () const  { return this->variant.C_cpr_kind_oracle.oradaptr_F; };
    const tpr_intaddr  &intaddr                () const  { return this->variant.C_cpr_kind_empty.intaddr_F; };
#endif    /* defined (__cplusplus) */
} sqldaptrvrec;

typedef struct                     sqldavrec
{
    union
    {
    /* cpr_kind_internal */
        struct sqldavrec_cpr_kind_internal
        {
            sqldatype           sqldb_F;
        } C_cpr_kind_internal;
    /* cpr_kind_db2 */
        struct sqldavrec_cpr_kind_db2
        {
            sqldb2type          db2_F;
        } C_cpr_kind_db2;
    /* cpr_kind_oracle */
        struct sqldavrec_cpr_kind_oracle
        {
            sqloratype          orada_F;
        } C_cpr_kind_oracle;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    sqldatype          &sqldb                  ()  { return this->variant.C_cpr_kind_internal.sqldb_F; };
    sqldb2type         &db2                    ()  { return this->variant.C_cpr_kind_db2.db2_F; };
    sqloratype         &orada                  ()  { return this->variant.C_cpr_kind_oracle.orada_F; };
    /* const accessor functions */
    const sqldatype    &sqldb                  () const  { return this->variant.C_cpr_kind_internal.sqldb_F; };
    const sqldb2type   &db2                    () const  { return this->variant.C_cpr_kind_db2.db2_F; };
    const sqloratype   &orada                  () const  { return this->variant.C_cpr_kind_oracle.orada_F; };
#endif    /* defined (__cplusplus) */
} sqldavrec;
    /**************************************/
    /*        S Q L T R A C E  TYPE  neu  */
    /*    trace output infos              */
    /*    aus alten sqlca ?ubernommen    */
    /**************************************/

typedef struct                     sqltatype
{
    tsp00_VFilename     tatracefn;
    tsp00_Int4          tatracepprt;
    tsp00_Int4          tatraceno;
    tsp00_Int2          tatracety;
    tsp00_Int2          tamaxsecoutp;
    tsp00_VFilename     tamaxsecfn;
    tsp00_Int4          tamaxsecno;
    tsp00_Int4          tamaxsecpptr;
    tsp00_C8            tastartdate;
    tsp00_C8            taenddate;
    tsp00_C8            tastarttime;
    tsp00_C8            taendtime;
    tsp00_Int2          tacnttrstm;
    tsp00_Int2          tatrout;
    tsp00_Int2          tafiller;
    tsp00_Int2          tastr80l;
    tsp00_C256          tastr80;
} sqltatype;
typedef sqltatype                 *sqltapointer;
    /**************************************/
    /*    O P T I O N - A R E A   neu     */
    /*    runtime optionen                */
    /*    aus alten sqlga ?ubernommen      */
    /**************************************/

typedef struct                     sqloatype
{
    tsp00_VFilename     oamodtracefn;
    tsp00_Int2          oamodtrace;
    tsp00_Int2          oamodtracety;
    tsp00_Int2          oamodsettrty;
    tsp00_Int2          oamaxtrstm;
    tsp00_Int2          oanosdf;
    tsp00_Int2          oamaxsec;
    tsp00_Int2          oapidflag;
    tsp00_Int4          oapid;
} sqloatype;
typedef sqloatype                 *sqloapointer;
    /*********************************************/
    /*        LONGVARCHAR                        */
    /*  For sql commands with longvarchars       */
    /* Structur to send a hostvariable >= 4k     */
    /* to the  kernel                            */
    /*********************************************/

typedef struct                     tpr_longvarcharrec
{
    tsp00_Int2          lvccolno;
    tsp00_Int2          lvcdescno;
    tsp00_Int4          lvcfreepos;
    tsp00_Int2          lvcaoffset;
    pasbool             lvcnext;
    tsp00_ValMode       lvcvalmode;
    tsp00_Int2          lvcoldpos;
    tsp00_Int2          lvcoldlen;
    tsp00_Int4          lvcvalpos;
    tsp00_Int4          lvcvallen;
    tsp00_Int4          lvchostpos;
    tsp00_Int4          lvchostlen;
    tsp00_Int4          lvcindicator;
    tsp00_Int4          lvcvalmlen;
    tpr_runtime_errors  lvcerror;
    pasbool             lvcpiecewise;
} tpr_longvarcharrec;
    /*********************************************/
    /*        LONGVARCHAR DESCRIPTOINS INFOS     */
    /*  For sql commands with longvarchars       */
    /*  globale variable in g_init allocated     */
    /* pointer is set in sqlra-area (dlvcp)      */
    /*   will be set in procedure p04pinputvalue */
    /*   will be init in p01adbsexecute          */
    /*   will be used only for one command       */
    /*********************************************/

typedef struct                     tpr_ptr_packet
{
    union
    {
    /* true */
        struct tpr_ptr_packet_true
        {
            tpr_intaddr         intaddr_F;
        } C_true;
    /* false */
        struct tpr_ptr_packet_false
        {
            tsp1_packet_ptr     vtypep_F;
        } C_false;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tpr_intaddr        &intaddr                ()  { return this->variant.C_true.intaddr_F; };
    tsp1_packet_ptr    &vtypep                 ()  { return this->variant.C_false.vtypep_F; };
    /* const accessor functions */
    const tpr_intaddr  &intaddr                () const  { return this->variant.C_true.intaddr_F; };
    const tsp1_packet_ptr &vtypep                 () const  { return this->variant.C_false.vtypep_F; };
#endif    /* defined (__cplusplus) */
} tpr_ptr_packet;

typedef struct                     tpr_ptr_moveobj
{
    union
    {
    /* true */
        struct tpr_ptr_moveobj_true
        {
            tpr_intaddr         intaddr_F;
        } C_true;
    /* false */
        struct tpr_ptr_moveobj_false
        {
            tsp00_MoveObjPtr    vtypep_F;
        } C_false;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tpr_intaddr        &intaddr                ()  { return this->variant.C_true.intaddr_F; };
    tsp00_MoveObjPtr   &vtypep                 ()  { return this->variant.C_false.vtypep_F; };
    /* const accessor functions */
    const tpr_intaddr  &intaddr                () const  { return this->variant.C_true.intaddr_F; };
    const tsp00_MoveObjPtr &vtypep                 () const  { return this->variant.C_false.vtypep_F; };
#endif    /* defined (__cplusplus) */
} tpr_ptr_moveobj;

typedef struct                     tpr_restbuflong
{
    tsp00_Int2          rbcolno;
    tsp00_Int2          rbfiller1;
    tsp00_Int4          rbfiller2;
    tsp00_Int4          rbrestlen;
    tsp00_Int4          rbhostlensum;
    tsp00_Int4          rblongcolmax;
    tsp00_Int4          rbbufsize;
    tpr_ptr_moveobj     rbbufpart;
} tpr_restbuflong;

typedef struct                     tpr_ptr_restbuflong
{
    union
    {
    /* true */
        struct tpr_ptr_restbuflong_true
        {
            tpr_intaddr         intaddr_F;
        } C_true;
    /* false */
        struct tpr_ptr_restbuflong_false
        {
            tpr_restbuflong    *vtypep_F;
        } C_false;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tpr_intaddr        &intaddr                ()  { return this->variant.C_true.intaddr_F; };
    tpr_restbuflong *  &vtypep                 ()  { return this->variant.C_false.vtypep_F; };
    /* const accessor functions */
    const tpr_intaddr  &intaddr                () const  { return this->variant.C_true.intaddr_F; };
    tpr_restbuflong * const  &vtypep                 () const  { return this->variant.C_false.vtypep_F; };
#endif    /* defined (__cplusplus) */
} tpr_ptr_restbuflong;

typedef struct                     tpr_colpointer
{
    union
    {
    /* true */
        struct tpr_colpointer_true
        {
            tpr_sqlcol         *addr_F;
        } C_true;
    /* false */
        struct tpr_colpointer_false
        {
            tpr_intaddr         intaddr_F;
        } C_false;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tpr_sqlcol *       &addr                   ()  { return this->variant.C_true.addr_F; };
    tpr_intaddr        &intaddr                ()  { return this->variant.C_false.intaddr_F; };
    /* const accessor functions */
    tpr_sqlcol * const  &addr                   () const  { return this->variant.C_true.addr_F; };
    const tpr_intaddr  &intaddr                () const  { return this->variant.C_false.intaddr_F; };
#endif    /* defined (__cplusplus) */
} tpr_colpointer;

typedef struct                     tpr_sqlspld
{
    union
    {
    /* false */
        struct tpr_sqlspld_false
        {
            tsp00_Int2          ldkind_o_F;
            tsp00_Int2          ldlength_o_F;
            tsp00_LongDescriptor descold_F;
        } C_false;
    /* true */
        struct tpr_sqlspld_true
        {
            tsp00_Int2          ldkind_n_F;
            tsp00_Int2          ldlength_n_F;
            tsp00_LongDescBlock descnew_F;
        } C_true;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tsp00_Int2         &ldkind_o               ()  { return this->variant.C_false.ldkind_o_F; };
    tsp00_Int2         &ldlength_o             ()  { return this->variant.C_false.ldlength_o_F; };
    tsp00_LongDescriptor &descold                ()  { return this->variant.C_false.descold_F; };
    tsp00_Int2         &ldkind_n               ()  { return this->variant.C_true.ldkind_n_F; };
    tsp00_Int2         &ldlength_n             ()  { return this->variant.C_true.ldlength_n_F; };
    tsp00_LongDescBlock &descnew                ()  { return this->variant.C_true.descnew_F; };
    /* const accessor functions */
    const tsp00_Int2   &ldkind_o               () const  { return this->variant.C_false.ldkind_o_F; };
    const tsp00_Int2   &ldlength_o             () const  { return this->variant.C_false.ldlength_o_F; };
    const tsp00_LongDescriptor &descold                () const  { return this->variant.C_false.descold_F; };
    const tsp00_Int2   &ldkind_n               () const  { return this->variant.C_true.ldkind_n_F; };
    const tsp00_Int2   &ldlength_n             () const  { return this->variant.C_true.ldlength_n_F; };
    const tsp00_LongDescBlock &descnew                () const  { return this->variant.C_true.descnew_F; };
#endif    /* defined (__cplusplus) */
} tpr_sqlspld;

typedef struct                     tpr_sqlpcld
{
    tpr_vtypepointer    ldhostaddr;
    tpr_vtypepointer    ldindkaddr;
    tpr_colpointer      ldcoladdr;
    tsp00_Int4          ldhostlen;
    tsp00_Int2          ldhosttype;
    tsp00_Int2          ldindktype;
    tsp00_Int2          ldindklen;
    tsp00_Int2          ldindkfrac;
    tsp00_Int2          ldrowno;
    tsp00_Int2          ldcolno;
    tsp00_C1            ldsfiotype;
    tsp00_DataType      ldsfdtype;
    tsp00_Int2          ldfill;
    tsp00_Int4          ldfileno;
    tsp00_Int4          ldsfbufpos;
    tsp00_Int4          ldhostlensum;
    tpr_ptr_restbuflong ldrestdata;
} tpr_sqlpcld;
typedef ARRAY_TYPE_MSP00 (         tpr_sqlspldarr, tpr_sqlspld, mxpr_descrlvc);
typedef ARRAY_TYPE_MSP00 (         tpr_sqlpcldarr, tpr_sqlpcld, mxpr_descrlvc);
typedef tpr_sqlspldarr            *tpr_sqlspldptr;
typedef tpr_sqlpcldarr            *tpr_sqlpcldptr;
typedef struct sqlorentry sqlorentry;
typedef sqlorentry                *sqlorentry_ptr;

typedef struct                     tpr_sqlld
{
    tsp00_Int2          ldmaxi;
    tsp00_Int2          ldmaxo;
    tsp00_Int2          ldindpc;
    tsp00_Int2          ldindsp;
    tsp00_Int2          ldcolkind;
    tsp00_Int2          ldcount;
    pasbool             ldputvalsend;
    pasbool             ldallsend;
    tsp00_Int2          ldretmaxi;
    tsp00_Int2          ldretmaxo;
    tsp00_Int2          ldretindpc;
    tsp00_Int2          ldretindsp;
    tpr_ptr_restbuflong ldrestbufp;
    tsp00_Int4          lderrd3;
    tpr_sqlpcldptr      ldpc;
    tpr_sqlspldptr      ldsp;
    tpr_sqlmdesc        ldpc_mdesc;
    tpr_sqlmdesc        ldsp_mdesc;
    tpr_intaddr         ldpcarr;
    tpr_intaddr         ldsparr;
    tpr_sqlmdesc        ldpcarr_mdesc;
    tpr_sqlmdesc        ldsparr_mdesc;
    sqlorentry_ptr      ldorelastcmd;
} tpr_sqlld;

typedef struct                     tpr_ptr_sqlld
{
    union
    {
    /* true */
        struct tpr_ptr_sqlld_true
        {
            tpr_intaddr         intaddr_F;
        } C_true;
    /* false */
        struct tpr_ptr_sqlld_false
        {
            tpr_sqlld          *vtypep_F;
        } C_false;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tpr_intaddr        &intaddr                ()  { return this->variant.C_true.intaddr_F; };
    tpr_sqlld *        &vtypep                 ()  { return this->variant.C_false.vtypep_F; };
    /* const accessor functions */
    const tpr_intaddr  &intaddr                () const  { return this->variant.C_true.intaddr_F; };
    tpr_sqlld * const  &vtypep                 () const  { return this->variant.C_false.vtypep_F; };
#endif    /* defined (__cplusplus) */
} tpr_ptr_sqlld;
    /**************************************/
    /*        S Q L G A   neu              */
    /*  Global Area                       */
    /* Globaldaten, die zusaetzlich       */
    /* benoetigt werden                   */
    /**************************************/
typedef ARRAY_TYPE_MSP00 (         tpr_part_kind_ptr_record, tsp1_part_ptr, mxpr_partkind);

typedef struct                     sqlgaentry
{
    tsp00_TaskId        gareference;
    tsp00_C2            gamesscode;
    tsp00_Int2          gastatus;
    tsp00_Int2          gavaindex;
    tsp00_Int2          ganodeindex;
    tsp00_Int2          gareconnect;
    tsp00_Int2          ganolog;
    tsp00_Int2          gaprofinit;
    tsp00_Int2          gadbcodeset;
    tsp00_KnlIdentifier gaatdbname;
    tsp00_Int2          gafiller2;
    tpr_intaddr         gapatchptr;
    tsp1_packet_ptr     gareqptr;
    tsp1_packet_ptr     garecptr;
    tsp1_packet_list    gapacketlist;
    tsp4_xuser_record   gaxuserrec;
    tsp00_Int4          gaKernelVersion;
    tpr_intaddr         gaServerNode;
    pasbool             gaIsCanceled;
    tpr00_SessionID     gaKnlSessionID;
} sqlgaentry;
typedef sqlgaentry                *sqlgaentrypointer;

typedef struct                     sqlgatype
{
    tsp4_xuser_record   gaopxuserrec;
    tsp00_Int2          gauseropset;
    tsp00_Int2          gamodisolation;
    tsp00_Int2          gasqlconntyp;
    tsp00_Int2          gaencodingcnt;
    tpr_intaddr         gaencodingptr;
    tsp00_Int2          gaasciicodeset;
    tsp00_Int2          gaunicodeset;
    tsp00_Int2          gacmpkind;
    tpr_cmd_state_Enum  gaCmdState;
    tsp00_Addr          gaConnection;
    ARRAY_VAR_MSP00 (         gaentry, sqlgaentrypointer, 8);
} sqlgatype;
typedef sqlgatype                 *sqlgapointer;
    /**************************************/
    /*        S Q L R A                   */
    /*    Runtime Area                    */
    /* SQLDB-Auftragssegment               */
    /* f?ur  SQL und Dialog Aufrufe       */
    /* Precompilerprogramm                */
    /**************************************/

typedef struct                     tpr_error_warnset
{
    union
    {
    /* true */
        struct tpr_error_warnset_true
        {
            tsp00_Int2          int2_F;
        } C_true;
    /* false */
        struct tpr_error_warnset_false
        {
            tsp00_WarningSet    warn_F;
        } C_false;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tsp00_Int2         &int2                   ()  { return this->variant.C_true.int2_F; };
    tsp00_WarningSet   &warn                   ()  { return this->variant.C_false.warn_F; };
    /* const accessor functions */
    const tsp00_Int2   &int2                   () const  { return this->variant.C_true.int2_F; };
    const tsp00_WarningSet &warn                   () const  { return this->variant.C_false.warn_F; };
#endif    /* defined (__cplusplus) */
} tpr_error_warnset;
    /**************************************/
    /*        S Q L R A  neu              */
    /* Runtime Area SQLDB-Auftragssegment */
    /* f?ur  SQL und Dialog Aufrufe       */
    /* Precompilerprogramm                */
    /**************************************/

typedef struct                     tpr_sqlerror
{
    tsp00_Int2          ereturncode;
    tsp00_Int2          etextlen;
    tsp00_Int4          eerrorpos;
    tsp00_C16           ewarning;
    tsp00_C6            esqlstate;
    tsp1_comm_error     elzu;
    tpr_runtime_errors  eprerr;
    tsp00_C70           etext;
} tpr_sqlerror;
typedef tpr_sqlerror              *sqlempointer;

typedef struct                     sqlratype
{
    tsp00_C8            ratermid;
    tsp00_TaskId        raprocessid;
    tsp00_C3            racomponent;
    tsp00_Os            rasqlos;
    tsp00_Int2          ralang;
    tsp00_Int2          rasqlinit;
    tsp00_Int2          raactsession;
    tsp00_Int2          radbmode;
    tsp00_Int2          rasqlansi;
    tsp00_Int2          rakamode;
    tsp00_Int2          ramodeswitch;
    tsp00_Int2          ramesstype;
    tsp00_Int4          ranumrows;
    tsp00_Int2          ranotrtimeoutp;
    tsp00_Int2          raopprof;
    tsp00_Int4          raprofsec;
    pasbool             raparse_again;
    tsp00_C3            rafiller;
    tsp00_Int4          ra2filler;
    tsp1_segment_ptr    rasegptr;
    tpr_part_kind_ptr_record rasegmpartptr;
    tpr_intaddr         raprofstmtp;
    tpr_ptr_sqlld       rasqlldp;
    sqltapointer        rasqltap;
} sqlratype;
typedef sqlratype                 *sqlrapointer;
    /*********************************************/
    /*        S Q L S N                          */
    /* Statementname Area for statementname or   */
    /* cursorname from hostvar                   */
    /*********************************************/

typedef struct                     tpr_sqlsnentry
{
    tsp00_KnlIdentifier snname;
    tsp00_Int2          sntype;
    tsp00_Int2          snindex;
    tsp00_Int2          sncount;
    tsp00_Int2          snfiller1;
    tpr_vtypepointer    snstmtp;
    tsp00_Int4          snbuflen;
    tsp00_Int4          snstmtlen;
    tsp00_Int2          snsession;
    tsp00_Int2          snfiller2;
    tpr00_ParseInfo     snParseInfo;
} tpr_sqlsnentry;
typedef ARRAY_TYPE_MSP00 (         tpr_sqlsnentries, tpr_sqlsnentry, mxpr_sqlsn);

typedef struct                     tpr_ptr_sqlsne
{
    union
    {
    /* true */
        struct tpr_ptr_sqlsne_true
        {
            tpr_intaddr         intaddr_F;
        } C_true;
    /* false */
        struct tpr_ptr_sqlsne_false
        {
            tpr_sqlsnentries   *vtypep_F;
        } C_false;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tpr_intaddr        &intaddr                ()  { return this->variant.C_true.intaddr_F; };
    tpr_sqlsnentries *  &vtypep                 ()  { return this->variant.C_false.vtypep_F; };
    /* const accessor functions */
    const tpr_intaddr  &intaddr                () const  { return this->variant.C_true.intaddr_F; };
    tpr_sqlsnentries * const  &vtypep                 () const  { return this->variant.C_false.vtypep_F; };
#endif    /* defined (__cplusplus) */
} tpr_ptr_sqlsne;

typedef struct                     tpr_sqlsn
{
    tsp00_Int4          snentcnt;
    tsp00_Int4          snentact;
    tpr_ptr_sqlsne      snentptr;
} tpr_sqlsn;
    /*********************************************/
    /*        S Q L M A                          */
    /*  Macro Area                              */
    /* Liste fester Laenge                       */
    /* Eintraege enthalten Werte der Macro-      */
    /* parameter, die in SQL-Kommandos des       */
    /* Hostprogramms vorkommen                   */
    /*********************************************/

typedef struct                     tpr_mavalue
{
    tsp00_Int2          malen;
    tpr_macroline       maval;
} tpr_mavalue;
typedef ARRAY_TYPE_MSP00 (         sqlmaentries, tpr_mavalue, mxpr_sqlma);

typedef struct                     sqlmatype
{
    tsp00_Int4          maversion;
    sqlmaentries        maentry;
} sqlmatype;
typedef sqlmatype                 *sqlmapointer;
typedef tsp00_Addr                 tpr05_StringEncoding;
    /**************************************/
    /*        S Q L M F                   */
    /* Mfetch Area                        */
    /* wird zur Umschaltung von fetch auf */
    /* mfetch gegraucht                   */
    /**************************************/
    /* fetch optimierungs typen  */

typedef struct                     tpr_mfetchentry
{
    tsp00_Int2          mfInit;
    tsp00_Int2          mfSelectno;
    tsp00_Int2          mfBufinit;
    tsp00_Int2          mfReclen;
    tsp00_Int4          mfBuflen;
    tsp00_Int4          mfRecpos;
    tsp00_Int2          mfPrevfaspec;
    tsp00_Int2          mfLastfaspec;
    tsp00_Int2          mfRownotfound;
    tsp00_Int2          mfFetchget;
    tsp00_Int4          mfRecposbeg;
    tsp00_Int4          mfMaxselrec;
    tsp00_Int4          mfLastrecno;
    tsp00_Int4          mfNextLoopCnt;
    tsp00_Int4          mfAllRecordsRead;
    tsp00_Int4          mfActualPos;
    tsp00_Int4          mfBufbegno;
    tsp00_Int4          mfBufendno;
    tsp00_Int4          mfLongcolpos;
    tpr_ptr_packet      mfBufpointer;
    tpr_ptr_moveobj     mfrecpointer;
    tpr_part_kind_ptr_record mfsegmpartptr;
    tsp00_Int4          mfrecptrlen;
    tsp00_Int2          mforindex;
    tsp00_Int2          mffetchkind;
    tsp00_Int4          mfsumanzrec;
    tsp00_Int2          mfopendata;
    tsp00_Int2          mfPrecomindex;
    tpr_parsid          mfSelparsid;
    tsp00_KnlIdentifier mfresultname;
    tpr05_StringEncoding PacketEncoding;
} tpr_mfetchentry;
typedef ARRAY_TYPE_MSP00 (         sqlmfentarr, tpr_mfetchentry, csp_maxint2);
typedef sqlmfentarr               *sqlmfentptr;

typedef struct                     sqlmftype
{
    tsp00_Int2          mfselcnt;
    tsp00_Int2          mfmfetch;
    tsp00_Int4          mffiller;
    tpr_sqlmdesc        mfdesc;
    sqlmfentptr         mfentry;
} sqlmftype;
typedef sqlmftype                 *sqlmfpointer;
typedef char                       tpr_sqlarea_mode;

typedef struct                     sqlareapointer
{
    union
    {
    /* 0 */
        struct sqlareapointer_0
        {
            tpr_intaddr         intaddr_F;
        } C_0;
    /* 1 */
        struct sqlareapointer_1
        {
            sqlgapointer        sqlgaptr_F;
        } C_1;
    /* 2 */
        struct sqlareapointer_2
        {
            sqlrapointer        sqlraptr_F;
        } C_2;
    /* 3 */
        struct sqlareapointer_3
        {
            sqloapointer        sqloaptr_F;
        } C_3;
    /* 4 */
        struct sqlareapointer_4
        {
            sqlmapointer        sqlmaptr_F;
        } C_4;
    /* 5 */
        struct sqlareapointer_5
        {
            sqlmfpointer        sqlmfptr_F;
        } C_5;
    /* 6 */
        struct sqlareapointer_6
        {
            tpr_ptr_sqlld       sqllaptr_F;
        } C_6;
    /* 7 */
        struct sqlareapointer_7
        {
            sqltapointer        sqltaptr_F;
        } C_7;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tpr_intaddr        &intaddr                ()  { return this->variant.C_0.intaddr_F; };
    sqlgapointer       &sqlgaptr               ()  { return this->variant.C_1.sqlgaptr_F; };
    sqlrapointer       &sqlraptr               ()  { return this->variant.C_2.sqlraptr_F; };
    sqloapointer       &sqloaptr               ()  { return this->variant.C_3.sqloaptr_F; };
    sqlmapointer       &sqlmaptr               ()  { return this->variant.C_4.sqlmaptr_F; };
    sqlmfpointer       &sqlmfptr               ()  { return this->variant.C_5.sqlmfptr_F; };
    tpr_ptr_sqlld      &sqllaptr               ()  { return this->variant.C_6.sqllaptr_F; };
    sqltapointer       &sqltaptr               ()  { return this->variant.C_7.sqltaptr_F; };
    /* const accessor functions */
    const tpr_intaddr  &intaddr                () const  { return this->variant.C_0.intaddr_F; };
    const sqlgapointer &sqlgaptr               () const  { return this->variant.C_1.sqlgaptr_F; };
    const sqlrapointer &sqlraptr               () const  { return this->variant.C_2.sqlraptr_F; };
    const sqloapointer &sqloaptr               () const  { return this->variant.C_3.sqloaptr_F; };
    const sqlmapointer &sqlmaptr               () const  { return this->variant.C_4.sqlmaptr_F; };
    const sqlmfpointer &sqlmfptr               () const  { return this->variant.C_5.sqlmfptr_F; };
    const tpr_ptr_sqlld &sqllaptr               () const  { return this->variant.C_6.sqllaptr_F; };
    const sqltapointer &sqltaptr               () const  { return this->variant.C_7.sqltaptr_F; };
#endif    /* defined (__cplusplus) */
} sqlareapointer;
    /*********************************************/
    /*        S Q L V A 1, 2, 3, EX - A R E A S  */
    /*  Variable Area                            */
    /* sqlva1 addressen der hostvariblen + index */
    /*        to  sqlva2, sqlva3                 */
    /* sqlva2 contains the datatyps              */
    /* sqlva3 contains the variable names        */
    /* sqlnd   contains the declare for expanding*/
    /*         the sqlva1 table  only used       */
    /*         during precompiling (vpc00)       */
    /*********************************************/

typedef struct                     sqlva1entry
{
    union
    {
    /* sqlvasc */
        struct sqlva1entry_sqlvasc
        {
            tsp00_Int2          va1indi_sc_F;
            tsp00_Int2          va1indva2_sc_F;
            tsp00_Int2          va1indva3_sc_F;
            tsp00_Int2          va1fill_F;
            tpr_vtypepointer    va1addr_sc_F;
            tpr_intaddr         va1fill1_F;
        } C_sqlvasc;
    /* sqlvacm */
        struct sqlva1entry_sqlvacm
        {
            tsp00_Int2          va1indi_cm_F;
            tsp00_Int2          va1indva2_cm_F;
            tsp00_Int2          va1indva3_cm_F;
            tsp00_Int2          va1fill_cm_F;
            tsp00_Int4          va1offs_cm_F;
            tsp00_Int4          va1fill2_F;
        } C_sqlvacm;
    /* sqlvast */
        struct sqlva1entry_sqlvast
        {
            tsp00_Int2          va1indi_st_F;
            tsp00_Int2          va1cmpcnt_st_F;
            tsp00_Int4          va1size_st_F;
            tpr_vtypepointer    va1addr_st_F;
            tsp00_Int4          va1ix_st_F;
        } C_sqlvast;
    /* sqlvapt */
        struct sqlva1entry_sqlvapt
        {
            tsp00_Int2          va1indi_pt_F;
            tsp00_Int2          va1fill3_F;
            tsp00_Int4          va1ix_pt_F;
            tpr_vtypepointer    va1addr_pt_F;
            tpr_intaddr         va1fill5_F;
        } C_sqlvapt;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tsp00_Int2         &va1indi_sc             ()  { return this->variant.C_sqlvasc.va1indi_sc_F; };
    tsp00_Int2         &va1indva2_sc           ()  { return this->variant.C_sqlvasc.va1indva2_sc_F; };
    tsp00_Int2         &va1indva3_sc           ()  { return this->variant.C_sqlvasc.va1indva3_sc_F; };
    tsp00_Int2         &va1fill                ()  { return this->variant.C_sqlvasc.va1fill_F; };
    tpr_vtypepointer   &va1addr_sc             ()  { return this->variant.C_sqlvasc.va1addr_sc_F; };
    tpr_intaddr        &va1fill1               ()  { return this->variant.C_sqlvasc.va1fill1_F; };
    tsp00_Int2         &va1indi_cm             ()  { return this->variant.C_sqlvacm.va1indi_cm_F; };
    tsp00_Int2         &va1indva2_cm           ()  { return this->variant.C_sqlvacm.va1indva2_cm_F; };
    tsp00_Int2         &va1indva3_cm           ()  { return this->variant.C_sqlvacm.va1indva3_cm_F; };
    tsp00_Int2         &va1fill_cm             ()  { return this->variant.C_sqlvacm.va1fill_cm_F; };
    tsp00_Int4         &va1offs_cm             ()  { return this->variant.C_sqlvacm.va1offs_cm_F; };
    tsp00_Int4         &va1fill2               ()  { return this->variant.C_sqlvacm.va1fill2_F; };
    tsp00_Int2         &va1indi_st             ()  { return this->variant.C_sqlvast.va1indi_st_F; };
    tsp00_Int2         &va1cmpcnt_st           ()  { return this->variant.C_sqlvast.va1cmpcnt_st_F; };
    tsp00_Int4         &va1size_st             ()  { return this->variant.C_sqlvast.va1size_st_F; };
    tpr_vtypepointer   &va1addr_st             ()  { return this->variant.C_sqlvast.va1addr_st_F; };
    tsp00_Int4         &va1ix_st               ()  { return this->variant.C_sqlvast.va1ix_st_F; };
    tsp00_Int2         &va1indi_pt             ()  { return this->variant.C_sqlvapt.va1indi_pt_F; };
    tsp00_Int2         &va1fill3               ()  { return this->variant.C_sqlvapt.va1fill3_F; };
    tsp00_Int4         &va1ix_pt               ()  { return this->variant.C_sqlvapt.va1ix_pt_F; };
    tpr_vtypepointer   &va1addr_pt             ()  { return this->variant.C_sqlvapt.va1addr_pt_F; };
    tpr_intaddr        &va1fill5               ()  { return this->variant.C_sqlvapt.va1fill5_F; };
    /* const accessor functions */
    const tsp00_Int2   &va1indi_sc             () const  { return this->variant.C_sqlvasc.va1indi_sc_F; };
    const tsp00_Int2   &va1indva2_sc           () const  { return this->variant.C_sqlvasc.va1indva2_sc_F; };
    const tsp00_Int2   &va1indva3_sc           () const  { return this->variant.C_sqlvasc.va1indva3_sc_F; };
    const tsp00_Int2   &va1fill                () const  { return this->variant.C_sqlvasc.va1fill_F; };
    const tpr_vtypepointer &va1addr_sc             () const  { return this->variant.C_sqlvasc.va1addr_sc_F; };
    const tpr_intaddr  &va1fill1               () const  { return this->variant.C_sqlvasc.va1fill1_F; };
    const tsp00_Int2   &va1indi_cm             () const  { return this->variant.C_sqlvacm.va1indi_cm_F; };
    const tsp00_Int2   &va1indva2_cm           () const  { return this->variant.C_sqlvacm.va1indva2_cm_F; };
    const tsp00_Int2   &va1indva3_cm           () const  { return this->variant.C_sqlvacm.va1indva3_cm_F; };
    const tsp00_Int2   &va1fill_cm             () const  { return this->variant.C_sqlvacm.va1fill_cm_F; };
    const tsp00_Int4   &va1offs_cm             () const  { return this->variant.C_sqlvacm.va1offs_cm_F; };
    const tsp00_Int4   &va1fill2               () const  { return this->variant.C_sqlvacm.va1fill2_F; };
    const tsp00_Int2   &va1indi_st             () const  { return this->variant.C_sqlvast.va1indi_st_F; };
    const tsp00_Int2   &va1cmpcnt_st           () const  { return this->variant.C_sqlvast.va1cmpcnt_st_F; };
    const tsp00_Int4   &va1size_st             () const  { return this->variant.C_sqlvast.va1size_st_F; };
    const tpr_vtypepointer &va1addr_st             () const  { return this->variant.C_sqlvast.va1addr_st_F; };
    const tsp00_Int4   &va1ix_st               () const  { return this->variant.C_sqlvast.va1ix_st_F; };
    const tsp00_Int2   &va1indi_pt             () const  { return this->variant.C_sqlvapt.va1indi_pt_F; };
    const tsp00_Int2   &va1fill3               () const  { return this->variant.C_sqlvapt.va1fill3_F; };
    const tsp00_Int4   &va1ix_pt               () const  { return this->variant.C_sqlvapt.va1ix_pt_F; };
    const tpr_vtypepointer &va1addr_pt             () const  { return this->variant.C_sqlvapt.va1addr_pt_F; };
    const tpr_intaddr  &va1fill5               () const  { return this->variant.C_sqlvapt.va1fill5_F; };
#endif    /* defined (__cplusplus) */
} sqlva1entry;

typedef struct                     sqlva2entry
{
    tsp00_Int2          va2typ;
    tsp00_Int2          va2digit;
    tsp00_Int4          va2size;
    tsp00_Int2          va2frac;
    tsp00_Int2          va2const;
} sqlva2entry;

typedef struct                     sqlva3entry
{
    tsp00_Lname         va3name;
    tsp00_Int2          va3naml;
    tsp00_Int2          va3fill;
} sqlva3entry;
typedef ARRAY_TYPE_MSP00 (         sqlva1type, sqlva1entry, mxpr_sqlva1);
typedef sqlva1type                *sqlva1p;
typedef ARRAY_TYPE_MSP00 (         sqlva2type, sqlva2entry, mxpr_sqlva2);
typedef sqlva2type                *sqlva2p;
typedef ARRAY_TYPE_MSP00 (         sqlva3type, sqlva3entry, mxpr_sqlva3);
typedef sqlva3type                *sqlva3p;
    /*********************************************/
    /*        S Q L P A R  neu                   */
    /* Parameter Area                            */
    /* loopcount-entry, struct-enty und          */
    /* Liste, soviele Eintraege wie Summe aller  */
    /* Parameter in allen SQL-Kommandos des      */
    /* Hostprogramms                             */
    /* Eintraege enthalten Verweise auf Host-    */
    /* variable und Indikatorvariable und Be-    */
    /* schreibung - fuer Parameter-Uebergabe     */
    /* zwischen Hostprogramm und SQLDB - der      */
    /* Hostvariablen                             */
    /*********************************************/

typedef struct                     sqlparentry
{
    union
    {
    /* sqlparlo */
        struct sqlparentry_sqlparlo
        {
            tsp00_Int2          pakindlo_F;
            tsp00_Int2          pava1index_F;
            tsp00_Int4          paloopcnt_F;
            tsp00_Int4          paloopmin_F;
        } C_sqlparlo;
    /* sqlparst */
        struct sqlparentry_sqlparst
        {
            tsp00_Int2          pakindst_F;
            tsp00_Int2          pavarst_F;
            tsp00_Int2          paindst_F;
            tsp00_Int2          paelcnt_F;
            tpr_intaddr         pafill4_F;
        } C_sqlparst;
    /* sqlparel */
        struct sqlparentry_sqlparel
        {
            tsp00_Int2          pakindel_F;
            tsp00_Int2          pavarno_F;
            tsp00_Int2          paindno_F;
        } C_sqlparel;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tsp00_Int2         &pakindlo               ()  { return this->variant.C_sqlparlo.pakindlo_F; };
    tsp00_Int2         &pava1index             ()  { return this->variant.C_sqlparlo.pava1index_F; };
    tsp00_Int4         &paloopcnt              ()  { return this->variant.C_sqlparlo.paloopcnt_F; };
    tsp00_Int4         &paloopmin              ()  { return this->variant.C_sqlparlo.paloopmin_F; };
    tsp00_Int2         &pakindst               ()  { return this->variant.C_sqlparst.pakindst_F; };
    tsp00_Int2         &pavarst                ()  { return this->variant.C_sqlparst.pavarst_F; };
    tsp00_Int2         &paindst                ()  { return this->variant.C_sqlparst.paindst_F; };
    tsp00_Int2         &paelcnt                ()  { return this->variant.C_sqlparst.paelcnt_F; };
    tpr_intaddr        &pafill4                ()  { return this->variant.C_sqlparst.pafill4_F; };
    tsp00_Int2         &pakindel               ()  { return this->variant.C_sqlparel.pakindel_F; };
    tsp00_Int2         &pavarno                ()  { return this->variant.C_sqlparel.pavarno_F; };
    tsp00_Int2         &paindno                ()  { return this->variant.C_sqlparel.paindno_F; };
    /* const accessor functions */
    const tsp00_Int2   &pakindlo               () const  { return this->variant.C_sqlparlo.pakindlo_F; };
    const tsp00_Int2   &pava1index             () const  { return this->variant.C_sqlparlo.pava1index_F; };
    const tsp00_Int4   &paloopcnt              () const  { return this->variant.C_sqlparlo.paloopcnt_F; };
    const tsp00_Int4   &paloopmin              () const  { return this->variant.C_sqlparlo.paloopmin_F; };
    const tsp00_Int2   &pakindst               () const  { return this->variant.C_sqlparst.pakindst_F; };
    const tsp00_Int2   &pavarst                () const  { return this->variant.C_sqlparst.pavarst_F; };
    const tsp00_Int2   &paindst                () const  { return this->variant.C_sqlparst.paindst_F; };
    const tsp00_Int2   &paelcnt                () const  { return this->variant.C_sqlparst.paelcnt_F; };
    const tpr_intaddr  &pafill4                () const  { return this->variant.C_sqlparst.pafill4_F; };
    const tsp00_Int2   &pakindel               () const  { return this->variant.C_sqlparel.pakindel_F; };
    const tsp00_Int2   &pavarno                () const  { return this->variant.C_sqlparel.pavarno_F; };
    const tsp00_Int2   &paindno                () const  { return this->variant.C_sqlparel.paindno_F; };
#endif    /* defined (__cplusplus) */
} sqlparentry;
typedef ARRAY_TYPE_MSP00 (         sqlpartype, sqlparentry, mxpr_sqlpa);
typedef sqlpartype                *sqlparpointer;
    /*********************************************/
    /*        S Q L F A                          */
    /*  Fetch Area                               */
    /* Eintraege enthalten zur mfetch Abarbeitung*/
    /*     interne Werte                         */
    /*********************************************/

typedef struct                     sqlfaentry
{
    tsp00_Int2          faselectno;
    tsp00_Int2          famfindex;
    tsp00_Int4          fapos;
    tsp00_Int2          faspec;
    tsp00_Int2          fafetch;
    tsp00_Int4          fareclen;
} sqlfaentry;
typedef ARRAY_TYPE_MSP00 (         sqlfatype, sqlfaentry, mxpr_sqlfa);
typedef sqlfatype                 *sqlfapointer;
    /*********************************************/
    /*        S Q L K A                          */
    /*  Kommando Area                            */
    /* Struktur aus Initialisierungs-Flag fuer   */
    /* Hatzel-Check und einer Liste mit sovielen */
    /* Eintraegen wie SQL-Kommandos im Hostpro-  */
    /* gramm eingebettet sind.                   */
    /* Eintraege enthalten Kommandobeschreibung  */
    /* und Verweis auf Parameterbeschreibungen   */
    /*********************************************/

typedef struct                     sqlkaentry
{
    tsp00_Int2          katyp;
    tsp00_Int2          kastate;
    tsp00_Int2          kapacount;
    tsp00_Int2          kapaindex;
    tsp00_Int2          kaStcount;
    tsp00_Int2          kaStindex;
    tsp00_Int2          kamacro;
    tsp00_Int2          kaprindex;
    tsp00_Int2          kadiindex;
    tsp00_Int2          kaparserrc;
    tsp00_Int4          kaversion;
    tpr00_ParseInfo     kaParseInfo;
    tsp00_Int2          kaopendata;
    tsp00_Int2          kaparamcnt;
    tsp00_Int2          kalineno;
    tsp00_Int2          kafaindex;
    tsp00_Int2          kaatindex;
    tsp00_Int2          kacuindex;
    tsp00_Int2          kamode;
    tpr_sqlrow          kasqlrow;
    sqlfaentry         *kafae;
} sqlkaentry;
typedef sqlkaentry                *sqlkaentry_ptr;
typedef ARRAY_TYPE_MSP00 (         sqlkatype, sqlkaentry, mxpr_sqlka);
typedef sqlkatype                 *sqlkapointer;
    /*********************************************/
    /*        S Q L O R                          */
    /*  Oracle   Area                            */
    /* Eintraege enthalten zu jedem Kommando     */
    /* von Prepare, Open, Fetch  Kommandos       */
    /* Pointer auf die shortfield infos,         */
    /* ist open neu geparsed, und beim fetch den */
    /* kaindex zum zugehoerigen open             */
    /*********************************************/

 struct                     sqlorentry
{
    tsp00_Int4          orcolcnt;
    tsp00_Int4          orcollen;
    tpr_sqlrow          orsqlrow;
    tpr_sqlcolpointer   orcolptr;
    tsp00_Int4          orcolcntacc;
    tsp00_Int2          oropennew;
    tsp00_Int4          orrescnt;
    tsp00_Int4          orreslen;
    tpr_sqlrow          orressqlrow;
    tpr_sqlcolpointer   orresptr;
    tsp00_Int4          orrescntacc;
    tsp00_Int4          orresnamlen;
    tpr_vtypepointer    orresnamptr;
    tsp00_Int4          orresnamacc;
    tpr_vtypepointer    orresnamencoding;
} ;
    /* sqlorentry_ptr = ^sqlorentry; */
    /* declared above before tpr_sqlld */
typedef ARRAY_TYPE_MSP00 (         sqlortype, sqlorentry, mxpr_sqlka);
typedef sqlortype                 *sqlorpointer;
    /*********************************************/
    /*        S Q L P R                          */
    /*  Prepare  Area                            */
    /* Eintraege enthalten Kommandobeschreibung  */
    /* von Prepare Kommandos  und                */
    /* von Report  Kommandos                     */
    /* und Verweis auf Parameterbeschreibungen   */
    /*********************************************/

typedef struct                     sqlprentry
{
    tsp00_Int2          prStmtNameIndex;
    tsp00_Int2          prCursorNameIndex;
    tsp00_Int2          prstate;
    tsp00_KnlIdentifier prStmtName;
    tsp00_Int2          prkaindex;
    tsp00_Int2          prarea;
    tsp00_Int2          prDescribe;
    tsp00_KnlIdentifier prCursorName;
    tsp00_Int2          prcuindex;
    tsp00_Int2          prstcount;
    tsp00_Int2          prstindex;
} sqlprentry;
typedef ARRAY_TYPE_MSP00 (         sqlprtype, sqlprentry, mxpr_sqlpr);
typedef sqlprtype                 *sqlprpointer;
    /*********************************************/
    /*        S Q L S T                          */
    /*  Statement  Area                          */
    /* Eintraege enthalten Stringkonstanten      */
    /* von Prepare  Kommandos                    */
    /*********************************************/

typedef struct                     sqlstentry
{
    tsp00_Int4          stkano;
    tsp00_Int4          stllen;
    tpr_sqlline         stline;
} sqlstentry;
typedef ARRAY_TYPE_MSP00 (         sqlsttype, sqlstentry, mxpr_sqlst);
typedef sqlsttype                 *sqlstpointer;
    /*********************************************/
    /*        S Q L A T                          */
    /*  AT <dbname> for  ORACLE Area             */
    /* Eintraege enthalten DBnamen-beschreibung  */
    /* von Oracle  Kommandos  und                */
    /* und Verweise auf die gaarea               */
    /*********************************************/

typedef struct                     sqlatentry
{
    tsp00_KnlIdentifier atdbname;
    tsp00_Int2          atdbindex;
    tsp00_NodeId        atdbstring;
    tsp00_Int2          atgaindex;
    tsp00_Int2          atdbnamix;
} sqlatentry;
typedef ARRAY_TYPE_MSP00 (         sqlattype, sqlatentry, mxpr_sqlat);
typedef sqlattype                 *sqlatpointer;
    /*********************************************/
    /*        S Q L C U                          */
    /*  Cursor and Statementname   Area  Oracle  */
    /* Eintraege enthalten Ergn u Stmn  for      */
    /* multi db   Kommandos                      */
    /*********************************************/

typedef struct                     sqlcuentry
{
    tsp00_KnlIdentifier cuname;
    tsp00_Int2          cufill;
    tsp00_Int2          cuatindex;
    tsp00_Int4          cuerrd3;
    tsp00_Int2          custate;
    tsp00_Int2          cukind;
    tsp00_Int2          cukaopenind;
    tsp00_Int4          culoopin;
    tsp00_Int4          culoopout;
    sqldaptrvrec        cusqlindap;
    sqldaptrvrec        cusqloutdap;
} sqlcuentry;
typedef sqlcuentry                *sqlcuentry_ptr;
typedef ARRAY_TYPE_MSP00 (         sqlcutype, sqlcuentry, mxpr_sqlcu);
typedef sqlcutype                 *sqlcupointer;
    /*********************************************/
    /*        S Q L F N                          */
    /*  Filename include Area                    */
    /* Eintraege enthalten kanomin bis kanomax   */
    /* und filename des include files            */
    /*********************************************/

typedef struct                     sqlfnentry
{
    tsp00_Int2          fnkanomin;
    tsp00_Int2          fnkanomax;
    tsp00_KnlIdentifier fnfilen;
    tsp00_Int2          fnincllno;
} sqlfnentry;
typedef ARRAY_TYPE_MSP00 (         sqlfntype, sqlfnentry, mxpr_sqlfn);
typedef sqlfntype                 *sqlfnpointer;

typedef struct                     tpr00_OldAreas
{
    sqlparpointer       sqlpap;
    sqlkapointer        sqlkap;
    sqlprpointer        sqlprp;
    sqlstpointer        sqlstp;
    sqlfapointer        sqlfap;
    sqlatpointer        sqlatp;
    sqlva1p             sqlv1p;
    sqlva2p             sqlv2p;
    sqlva3p             sqlv3p;
    sqlcupointer        sqlcup;
    sqlorpointer        sqlorp;
    sqlfnpointer        sqlfnp;
} tpr00_OldAreas;
typedef tsp00_Int4                 tpr01_DescTypeEnum;
typedef tsp00_Int4                 tpr01_SQLStateBinary;
typedef tsp00_Int2                 tpr01_SQLAttribute;
typedef tsp00_Int4                 tpr00_ComTypeEnum;
typedef tsp00_Int4                 tpr00_DescribeKindEnum;
typedef tsp00_Int4                 tpr00_TraceParseID_Enum;
typedef tsp00_Int4                 tpr01_KeywordTypeEnum;
typedef tsp00_Int4                 tpr_gsymboltypes;
typedef tsp00_Addr                 tpr01_ConDesc;

typedef struct                     tpr01_SQLDesc
{
    tpr01_DescTypeEnum  DescType;
    tsp00_Addr          SQL;
    tsp00_Addr          StmtNameDesc;
    tsp00_Addr          CursorDesc;
    tsp00_Addr          ErrorDesc;
    tsp00_Addr          ConDesc;
    tpr00_ComTypeEnum   ComType;
    tsp1_cmd_mess_type  MessType;
    tpr05_StringEncoding PacketEncoding;
    sqlprentry         *pr;
    sqlkaentry         *ka;
    sqlorentry         *ore;
    sqlcuentry         *cu;
    tsp00_Addr          sqlxap;
    tpr01_SQLStateBinary State;
    tpr01_SQLAttribute  Attribute;
} tpr01_SQLDesc;
    /*********************************************/
    /*        S Q L C X A  - A R E A             */
    /*  cxa-area   interne werte call-interf.    */
    /*********************************************/

typedef struct                     sqlcxatype
{
    tsp00_Int2          xainit;
    tsp00_Int2          xacmdinit;
    tsp00_Int2          xakano;
    tsp00_KnlIdentifier xaprogn;
    tsp00_Int2          xaprogc;
    tsp00_KnlIdentifier xamodn;
    tsp00_Int2          xamodc;
    tsp00_Int2          xalang;
    tsp00_Addr          sqlModDesc;
    tpr01_SQLDesc      *xaSQLDesc;
    tsp00_Int4          xacfetposc;
    tsp00_Int2          xaprno;
    tsp00_Sname         xalcheck;
    tsp00_KnlIdentifier xaprname;
    tsp00_Int2          xafiller1;
    sqldaptrvrec        xasqldap;
    tsp00_Int2          xacfetspec;
    tsp00_C6            xacfiller;
} sqlcxatype;
typedef sqlcxatype                *sqlcxapointer;
typedef OLDPASCALSTRING_TYPE_MSP00 (  sqlstatetype, 6);
typedef CSTRING_TYPE_MSP00      (  sqlstatetypec, 6);
    /**************************************/
    /*        S Q L C A  neu              */
    /* Communication Area                 */
    /* Fehlermeldungen von SQLDB an das    */
    /* Hostprogramm                       */
    /**************************************/

typedef struct                     sqlcatype
{
    tsp00_Int4          sqlenv;
    OLDPASCALSTRING_VAR_MSP00 (  sqlcaid, 8);
    tsp00_Int4          sqlcabc;
    tsp00_Int4          sqlcode;
    tsp00_Int2          sqlerrml;
    tsp00_C70           sqlerrmc;
    tsp00_C8            sqlerrp;
    ARRAY_VAR_MSP00 (         sqlerrd, tsp00_Int4, 6);
    char                sqlwarn0;
    char                sqlwarn1;
    char                sqlwarn2;
    char                sqlwarn3;
    char                sqlwarn4;
    char                sqlwarn5;
    char                sqlwarn6;
    char                sqlwarn7;
    char                sqlwarn8;
    char                sqlwarn9;
    char                sqlwarna;
    char                sqlwarnb;
    char                sqlwarnc;
    char                sqlwarnd;
    char                sqlwarne;
    char                sqlwarnf;
    OLDPASCALSTRING_VAR_MSP00 (  sqlext, 12);
    tsp00_KnlIdentifier sqlresn;
    tsp00_Int2          sqlfill1;
    tsp00_Int2          sqlmfetch;
    tsp00_Int4          sqltermref;
    tsp00_Int2          sqlfill5;
    tsp00_Int2          sqldbmode;
    tsp00_Int2          sqldatetime;
    sqlstatetype        sqlstate;
    tsp4_argline        sqlargl;
    sqlgapointer        sqlgap;
    sqlrapointer        sqlrap;
    sqloapointer        sqloap;
    sqlmapointer        sqlmap;
    sqlmfpointer        sqlmfp;
    tpr_intaddr         sqlplp;
    sqlempointer        sqlemp;
    tpr_sqlmdesc        sqlxadesc;
    sqlcxapointer       sqlcxap;
    tsp00_Addr          sqlEnvDesc;
} sqlcatype;
    /*********************************************/
    /*        S Q L X A                          */
    /*  Extent   Area                            */
    /* Struktur aus Initialisierungs-Flag fuer   */
    /* Hatzel-Check und modul globale Angaben    */
    /* Pointer auf alle SQL-Areas                */
    /*********************************************/

typedef struct                     sqlxatype
{
    tsp00_Int2          xainit;
    tsp00_Int2          xacmdinit;
    tsp00_Int2          xakano;
    tsp00_KnlIdentifier xaprogn;
    tsp00_Int2          xaprogc;
    tsp00_KnlIdentifier xamodn;
    tsp00_Int2          xamodc;
    tsp00_Int2          xalang;
    tsp00_Addr          xaModDesc;
    tpr01_SQLDesc      *xaSQLDesc;
    tsp00_Int4          xacfetposc;
    tsp00_Int2          xaprno;
    tsp00_Sname         xalcheck;
    tsp00_KnlIdentifier xaprname;
    tsp00_Int2          xastopcnt;
    tsp00_Int2          xallen;
    tpr_sqlline         xaline;
    tsp00_Int2          xafiller2;
    tsp00_Int4          xatime;
    tsp00_Int2          xadescpos;
    tsp00_Int2          xadesclen;
    tsp00_Int2          xaatcount;
    tsp00_Int2          xaatmax;
    tsp00_Int4          xafiller3;
    sqlparpointer       sqlpap;
    sqlparpointer       sqlpae;
    sqlkapointer        sqlkap;
    sqlkapointer        sqlkae;
    sqlprpointer        sqlprp;
    sqlprpointer        sqlpre;
    sqlstpointer        sqlstp;
    sqlstpointer        sqlste;
    sqlfapointer        sqlfap;
    sqlfapointer        sqlfae;
    sqlatpointer        sqlatp;
    sqlatpointer        sqlate;
    sqlva1p             sqlv1p;
    sqlva1p             sqlv1e;
    sqlva2p             sqlv2p;
    sqlva2p             sqlv2e;
    sqlva3p             sqlv3p;
    sqlva3p             sqlv3e;
    tpr_intaddr         sqlfill1;
    tpr_intaddr         sqlfill2;
    tpr_intaddr         sqlfill3;
    tpr_intaddr         sqlfill4;
    sqlcupointer        sqlcup;
    sqlcupointer        sqlcue;
    sqlorpointer        sqlorp;
    sqlorpointer        sqlore;
    sqlfnpointer        sqlfnp;
    sqlfnpointer        sqlfne;
    tpr_sqlsn           sqlsnp;
    tpr_sqlmdesc        sqlsndesc;
} sqlxatype;
    /*********************************************/
    /*        LOOPRECORD                         */
    /*  For sql array commands                   */
    /* Structur to send several commands         */
    /* to then kernel                            */
    /*********************************************/

typedef struct                     tpr_sqlloop
{
    tsp00_Int4          lpmax;
    tsp00_Int4          lpcnt;
    tsp00_Int4          lperrd3;
    tsp00_Int2          lpnext;
    tsp00_Int2          lpfiller;
    tsp00_Int4          lperrd3i;
    tsp00_Int4          lprescount;
    tsp00_Int4          lpindi;
    tsp00_Int4          lpindo;
    tsp00_Int2          lpfirsti;
    tsp00_Int2          lpfirsto;
    tsp00_Int4          lprecli;
    tsp00_Int4          lpreclo;
    tpr_runtime_errors  lperri;
    tpr_runtime_errors  lperro;
} tpr_sqlloop;
    /************************************************/
    /*   T Y P E N  des C A L L - I N T E R F A C E */
    /*     beginnen mit sqc                         */
    /************************************************/
typedef char                       tpr_char_array;
typedef tpr_char_array            *tpr_char_arrayp;
typedef OLDPASCALSTRING_TYPE_MSP00 (  tpr_string, 2);
typedef CSTRING_TYPE_MSP00      (  tpr_stringc, 2);
typedef char                       tpr05_CharBuffer;

typedef struct                     tpr05_String
{
    tpr05_CharBuffer   *rawString;
    tsp00_Addr         *encodingType;
    tsp00_Int4          cbLen;
    tsp00_Int4          cbAllocLen;
} tpr05_String;
typedef tpr05_String              *tpr05_StringPointer;

typedef struct                     tpr05_StmtAnalysisDesc
{
    tpr01_KeywordTypeEnum StmtType;
    tpr01_KeywordTypeEnum StmtOption;
    tsp00_Int4          CursorPos;
    tsp00_Int4          CursorLength;
    tpr_gsymboltypes    CursorType;
    tpr_gsymboltypes    FetchPosType;
    tsp00_Int4          UsingPos;
} tpr05_StmtAnalysisDesc;

struct tpr00_TemplateDummy {
    tpr_runtime_errors   dummy_tpr_runtime_errors;
};


#endif
