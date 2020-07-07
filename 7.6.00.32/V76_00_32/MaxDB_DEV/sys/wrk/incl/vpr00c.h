/*PRETTY*/
#ifndef __VPR00C_H__
#define __VPR00C_H__
#ifdef OSF1
/* ADIS 1001245 */
#pragma  message disable trailcomma
#endif

/* next move to gsp00 */
#define MIN_SINT1_SP00     (-128)
#define MAX_SINT1_SP00     127

#ifndef TRUE
#define TRUE (1)
#endif
#ifndef FALSE
#define FALSE (0)
#endif

#define  CPR_SQLDBCOUNT_ONE  "\C1\10\00\00\00\00\00\00\00\00"
#define  CPR_PARSIDNULL  "\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00"
#define  CPR_NULLCHAR  '\00'
#define  CPR_HT_CODE  '\09'     /* tabulator          */
#define  CPR_LEFTIND_PAR  '['   /* eckige klammer auf */
#define  CPR_RIGHTIND_PAR  ']'  /* eckige klammer zu  */
#define  CPR_BACKSLASH  '\\'
#define  CPR_PC_ENDSIGN  '!'
#define  CPR_RECORDCHR  '!'
#define  CPR_RECORDCHR2  '~'    /* tilde */
#define  CPR_RECORDCHR3  '|'    /* senkrechter strich */
#define  CPR_MACROCHR  '%'
#define  CPR_PARAMCHR  ':'
#define  CPR_PREPARECHR  '?'    /* ?? */
#define  CPR_WARNINGCHR  'W'
#define  CPR_DQUOSYM  '\"'
#define  CPR_QUOTSYM  '\''
#define  CPR_ASTERISK  '*'
#define  CPR_CBEGIN_PAR  '{'    /* geschweifte klammer auf */
#define  CPR_CEND_PAR  '}'      /* geschweifte klammer zu  */
#define  CPR_NUMBLEN_INT2  5     /* int2 : fixed len    */
#define  CPR_NUMBLEN_INT4  10    /* int4 : fixed len    */
#define  CPR_NUMBLEN_INT8  18    /* int8 : fixed len    */
#define  CPR_NUMBLEN_R4  6    /* real4: float len    */
#define  CPR_NUMBLEN_R8  15   /* real8: float len    */

#define   MXPR_STRING   8138
#define   MXPR_SQLGA        8
#define   MXPR_PARTBUF  16000     /* connect kommando buffer */
#define   CPR_PC_ENDSIGN    '!'
#define   CPR_ERR_CMD_NOT_AVAILABLE  -101

enum tpr_lzu_type {
      CPR_LZU_CONNECT   = 1,
      CPR_LZU_REQUEST,
      CPR_LZU_RECEIVE,
      CPR_LZU_RELEASE,
      CPR_LZU_SYSPROFILE,
      CPR_LZU_CANCEL };

enum tpr_dt_type {
      CPR_DT_EMPTY   = 0,
      CPR_DT_NORMAL,
      CPR_DT_ISO,
      CPR_DT_USA,
      CPR_DT_EUR,
      CPR_DT_JIS,
      CPR_DT_ANSI };

enum tpr_lo_type {
      CPR_LO_ISO_DEFAULT    = -1,
      CPR_LO_EMPTY          = -1,
      CPR_LO_ISOLEV0        = 0,
      CPR_LO_ISOLEV1        = 1,
      CPR_LO_ISOLEV2        = 2,
      CPR_LO_ISOLEV3        = 3,
      CPR_LO_ISOLEV4        = 4,
      CPR_LO_ISOLEV9        = 9,
      CPR_LO_ISOLEV10       = 10,
      CPR_LO_ISOLEV15       = 15,
      CPR_LO_ISOLEV20       = 20,
      CPR_LO_ISOLEV30       = 30 };

enum tpr_result_type {
        CPR_RESULT_XUSER_READ_ERR = 1,
        CPR_RESULT_SYSTEM_REQUIRED= 2,
        CPR_RESULT_TOO_MANY_USERS = 3,
        CPR_RESULT_USER_PASSW     = 4,
        CPR_RESULT_WHENEVER_STOP  = 5,
        CPR_RESULT_OPTION_ERROR   = 6,
        CPR_RESULT_TRACE_ERROR    = 7,
        CPR_RESULT_SQLXA_INVALID  = 8};

enum tpr_gsymboltypes {
        CPR_S_UNKNOWN = 0,
        CPR_S_COLON,
        CPR_S_COMMA,
        CPR_S_DIVIDE,
        CPR_S_EOF,
        CPR_S_EQUAL,
        CPR_S_FIXED_POINT_LITERAL,
        CPR_S_FLOATING_POINT_LITERAL,
        CPR_S_IDENTIFIER,
        CPR_S_LEFTINDPAR,
        CPR_S_LEFTPAR,
        CPR_S_MACRO,
        CPR_S_MINUS,
        CPR_S_PARAMETER_NAME,
        CPR_S_GREATER,
        CPR_S_PLUS,
        CPR_S_POINT,
        CPR_S_RIGHTINDPAR,
        CPR_S_RIGHTPAR,
        CPR_S_SEMICOLON,
        CPR_S_ASTERISK,
        CPR_S_STRING_LITERAL,
        CPR_S_UNSIGNED_INTEGER,
        CPR_S_BUFEOF,
        CPR_S_CSTRING_LITERAL,
        CPR_S_QUESTION_MARK};

#define  MXPR_C8   8
#define  MXPR_C80  80

enum tpr_sqldbmode {
        CPR_KIND_EMPTY = 0,
        CPR_KIND_INTERNAL,
        CPR_KIND_DB2,
        CPR_KIND_ANSI,
        CPR_KIND_ORACLE,
        CPR_KIND_SAPR3};
        /* data base mode */

enum tpr_multibyteset {
        CPR_SINGLE_BYTE = 0,
        CPR_MULTI_SJIS,
        CPR_MULTI_EUC} ;

enum tpr_sqlstr {
        MXPR_STRINGINPUTL2 = 8138}; /* max length of string column type */

static const unsigned char CPR_NUMB1 [] = /* number one */
        {0xC1, 0x10, 0x00, 0x00, 0x00, 0x00};
enum tpr_sqlnum {
        MXPR_NUMB =  6};

enum tpr_sfmode {
        CPR_INFO_MANDATORY = 0,
        CPR_INFO_OPTIONAL,
        CPR_INFO_DEFAULT,
        CPR_INFO_ESCAPE_CHAR}; /* sfmode */

enum tpr_sfio {
        CPR_INFO_INPUT = 0,
        CPR_INFO_OUTPUT,
        CPR_INFO_INOUT}; /* sfiotype */

enum tpr_long {
        CPR_NOLONG = 0,
        CPR_OLDLONG,
        CPR_NEWLONG}; /* long descriptor type */

enum tpr_vtype {
        CPR_VSTRUCT     = -1,
        CPR_VINT2       = 0,
        CPR_VINT4       = 1,
        CPR_VREAL4      = 2,
        CPR_VREAL8      = 3,
        CPR_VDECIMAL    = 4,
        CPR_VZONED      = 5,
        CPR_VCHAR       = 6,
        CPR_VCHARC      = 7,
        CPR_VBCHAR      = 8,
        CPR_VDBCHAR     = 9,
        CPR_VBUF        = 10,
        CPR_VSCALAR     = 11,
        CPR_VINTARR     = 12,
        CPR_VREC        = 13,
        CPR_VNONE       = 14,
        CPR_VSTRING     = 15,
        CPR_VUNS2       = 16,
        CPR_VUNS4       = 17,
        CPR_VDECREC     = 18,
        CPR_VARRAY      = 19,
        CPR_VSTRING1    = 20,
        CPR_VDBCHARC    = 21,
        CPR_VLZONE      = 22,
        CPR_VLSZON      = 23 ,
        CPR_VTSZON      = 24,
        CPR_VNUMBER     = 25,
        CPR_VBYTE       = 26,
        CPR_VDBYTE      = 27,
        CPR_VANSICHARC  = 28,
        CPR_VREAL8SUN   = 29,
        CPR_VSTRINGC    = 30,
        CPR_VANSICHAR   = 31,
        CPR_VUNUSED1    = 32,
        CPR_VINT8       = 33,
        CPR_VUNS8       = 34,
        CPR_VSTRING4    = 35,
        CPR_VIADDR      = 36,
        CPR_VRAW        = 37,
        CPR_VRAWC       = 38,
        CPR_VFILE       = 39,
        CPR_VFILEC      = 40,
        CPR_VUNICODE    = 41,
        CPR_VUNICODEC   = 42,
        CPR_VCHARZ      = 43,
        CPR_VONUMBER    = 44,
        CPR_VOVARNUM    = 45,
        CPR_VODATE      = 46,
        CPR_VABAPHANDLE = 47,
        CPR_VVARCHAR_WITHOUT_LEN = 48,
        CPR_VINT1       = 49,
        CPR_VUNS1       = 50,
        CPR_VLONGDESC   = 51,
        CPR_VUCS2       = 52,
        CPR_VUTF16      = 53,
        CPR_VSTRINGUNICODE  = 54,   /* ora-varchar  contains UCS2 */
        CPR_VSTRINGUNICODE4 = 55    /* ora-varchar4 contains UCS2 */
};
        /* host variable type */

enum tpc_vtypes {
        CPR_VINT = CPR_VINT4,
        CPR_VUNSIGNED = CPR_VUNS4,
        CPR_LINT = CPR_NUMBLEN_INT4,
        CPR_LUNSIGNED = CPR_NUMBLEN_INT4,
        CPR_VLONG    = CPR_VINT4,
        CPR_VUNSLONG = CPR_VUNS4,
        CPR_LLONG    = CPR_NUMBLEN_INT4,
        CPR_LUNSLONG = CPR_NUMBLEN_INT4,

        CPR_VSHORT   = CPR_VINT2,
        CPR_VUNSSHORT = CPR_VUNS2,
        CPR_LSHORT   = CPR_NUMBLEN_INT2,
        CPR_LUNSSHORT = CPR_NUMBLEN_INT2,
        CPR_VFLOAT   = CPR_VREAL4,
        CPR_VDOUBLE = CPR_VREAL8,
        CPR_LFLOAT   = CPR_NUMBLEN_R4,
        CPR_LDOUBLE = CPR_NUMBLEN_R8,
        CPR_VREAL = CPR_VREAL4,
        CPR_VTINY = CPR_VINT1,
        CPR_VUTINY = CPR_VUNS1
};

enum tpr_db2types {
        CPR_DB2_INT2 = 500,
        CPR_DB2_INT2N = 501,
        CPR_DB2_INT4 = 496,
        CPR_DB2_INT4N = 497,
        CPR_DB2_FLOAT = 480,
        CPR_DB2_FLOATN = 481,
        CPR_DB2_DECIMAL = 484,
        CPR_DB2_DECIMALN = 485,
        CPR_DB2_CHAR = 452,
        CPR_DB2_CHARN = 453,
        CPR_DB2_VARCHAR = 448,
        CPR_DB2_VARCHARN = 449,
        CPR_DB2_LVARCHAR = 456,
        CPR_DB2_LVARCHARN = 457,
        CPR_DB2_CHARC = 460,
        CPR_DB2_CHARCN = 461,
        CPR_DB2_DATE = 384,
        CPR_DB2_DATEN = 385,
        CPR_DB2_TIME = 388,
        CPR_DB2_TIMEN = 389,
        CPR_DB2_TIMESTAMP = 392,
        CPR_DB2_TIMESTAMPN = 393,
        CPR_DB2_GRAPHIC = 468,
        CPR_DB2_GRAPHICN = 469,
        CPR_DB2_VARGRAPHIC = 464,
        CPR_DB2_VARGRAPHICN = 465,
        CPR_DB2_LVARGRAPHIC = 472,
        CPR_DB2_LVARGRAPHICN = 473,
        CPR_DB2_FLOATLEN = 8,
        CPR_DB2_DATELEN = 10,
        CPR_DB2_TIMELEN = 8,
        CPR_DB2_TIMESTAMPLEN = 26,
        CPR_DB2_MAXSQLVARLEN = 52};
        /* CPR_KIND_DB2 types */

#define CPR_DEFINED_BYTE (char) 0x00
#define CPR_UNDEF_BYTE (char) 0xff
#define CPR_DEFAULT_BYTE (char) 0xfd
#define CPR_LONG_DESC_DEF_BYTE (char) 0xfC
#define CPR_OFLW_BYTE (char) 0xfe
#define CPR_COUNT_DEF_BYTE (char) 0x01
        /* defined byte */

#define CPR_ERROR_ABAP_DESC_NOT_SET  (-861)
#define CPR_ERROR_ABAP_EXCEPTION     (-862)
#define CPR_ERROR_CONNECTION_BUSY    (-810)
#define cpr_error_session_not_connected  (-821)
#define cpr_error_invalid_option         (-863)

enum tpr_num_error {
        CPR_NUM_OK = 0,
        CPR_NUM_TRUNC,
        CPR_NUM_OVERFLOW,
        CPR_NUM_INVALID,
        CPR_NUM_INCOMPATIBLE,
        CPR_NUM_MISSING_ADDR,
        CPR_NUM_INVESC,
        CPR_NUM_UNTERM,
        CPR_NUM_TRUNCCHAR = CPR_NUM_TRUNC};
        /* conversion function return codes */

enum tpr_tracetypes {
        CPR_TRACE_EMPTY = 0,
        CPR_TRACE_OFF,
        CPR_TRACE_FORMAL,
        CPR_TRACE_LONG,
        CPR_TRACE_MODFORMAL,
        CPR_TRACE_MODLONG}; /* trace type */

enum tpr_trtexttypes {
        CPR_TR_INP = 0,
        CPR_TR_OUTP,
        CPR_TR_LOOP,
        CPR_TR_LONGVAR,
        CPR_TR_ARRCNT}; /* trace text */

enum tpr_latypes {
        CPR_LA_ALL = 0,
        CPR_LA_CALL,
        CPR_LA_C = 3,
        CPR_LA_COBOL,
        CPR_LA_ODBC,
        CPR_LA_OCI,
        CPR_LA_COBMIC,
        CPR_LA_COB8860,
        CPR_LA_PASCALVAX,

        CPR_LA_PASCAL,
         CPR_LA_PASCALLPI,
        CPR_LA_PASCAL31  }; /* language */

enum tpr_pars_idix {
        CPR_PARS_IDIX = 10}; /* parse id index for special return codes */

enum tpr_statetypes {
        CPR_STATE_EMPTY = 0,
        CPR_STATE_PARSE,
        CPR_STATE_EXECUTE,
        CPR_STATE_COMMAND,
        CPR_STATE_PREPARE,
        CPR_STATE_MACRO,
        CPR_STATE_TRACE,
        CPR_STATE_SYNC_EXEC,
        CPR_STATE_NEWSYNC_EXEC,
        CPR_STATE_ASYNC_EXEC,
        CPR_STATE_DECL_STATEM,
        CPR_STATE_DECL_WITH_HOLD,
        CPR_STATE_EXECUTE_AT_FETCH};

enum tpr_intypes {
      CPR_IN_SQLVA          = 0,
      CPR_IN_SQLST          = 1};

enum tpr_co_type{
      CPR_CO_OK                     = 0,
      CPR_CO_DBMS_START_REQUIRED,
      CPR_CO_TOO_MANY_USERS,
      CPR_CO_RESTART_REQUIRED,
      CPR_CO_LOGON_REQUIRED,
      CPR_CO_USER_OR_PSWD_ILLEGAL,
      CPR_CO_CRASH,
      CPR_CO_TIMEOUT,
      CPR_CO_IMPLICIT,
      CPR_CO_USER_OP};

enum tpr_session_type{
      CPR_SE_EMPTY          = 0,
      CPR_SE_PRIMARY,
      CPR_SE_SECOND};

typedef enum tpr_cmd_state_Enum {
      CPR_CMD_STATE_INIT = 0,
      CPR_CMD_STATE_START,
      CPR_CMD_STATE_READY,
      CPR_CMD_STATE_INFLIGHT,
      CPR_CMD_STATE_END} tpr_cmd_state_Enum;

typedef tsp00_C64 tpr_hostname;


typedef struct
{
        tsp00_Int4        syposacc;
        tsp00_Int4        sypos;
        tsp00_Int4        sylen;
        tsp00_Int2        symb;
        tsp00_Int2        syfill;
}
 tpr_scannertype;


struct tpr_sqlloop
{

        tsp00_Int4 lpmax;
        tsp00_Int4 lpcnt;
        tsp00_Int4 lperrd3;
        tsp00_Int2 lpnext;
        tsp00_Int2 lpfiller;
        tsp00_Int4 lperrd3i;    /* only for input sqlerrd[2] */
        tsp00_Int4 lprescount;  /* only for select into array */
        tsp00_Int4 lpindi;
        tsp00_Int4 lpindo;
        tsp00_Int2 lpfirsti;
        tsp00_Int2 lpfirsto;
        tsp00_Int4 lprecli;
        tsp00_Int4 lpreclo;
        char  lperri;
        char  lperro;
};

typedef char tpr00_KnlSessionID[4];
typedef tsp00_Int4 tpr00_ClientSession;

typedef struct tpr00_SessionID {
  tpr00_KnlSessionID KnlSession;      /* Session no from Kernel */
  tpr00_ClientSession ClientSession;  /* unique connect id on client */
} tpr00_SessionID;

#define ParseIdKnl_mxpr00       (12)
#define ParseId_mxpr00       (ParseIdKnl_mxpr00+sizeof(tpr00_ClientSession))
typedef char tpr00_ParseId[ParseId_mxpr00];
#define ParseIdNull_cpr00    (tpr00_ParseId *)"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

typedef tsp00_Int2 tpr00_sp1rFunctionCode;

typedef struct tpr00_ParseInfo {
  tpr00_ParseId ParseId;
  tpr00_sp1rFunctionCode sp1rFunctionCode;
} tpr00_ParseInfo;

/* Definitions from newer kernel versions */
#define sp1pk_procid             (sp1pk_checkpoint_info+1)
#define sp1pk_long_demand        (sp1pk_checkpoint_info+2)
#define sp1pk_message_list       (sp1pk_checkpoint_info+3)
#define sp1pk_vardata_shortinfo  (sp1pk_checkpoint_info+4)
#define sp1pk_vardata            (sp1pk_checkpoint_info+5)
#define sp1pk_feature            (sp1pk_checkpoint_info+6)
#define sp1pk_clientid           (sp1pk_checkpoint_info+7)
#define sp1pk_last_part_kind     (sp1pk_checkpoint_info+8)

typedef tsp1_part *tpr00_PartCache[sp1pk_last_part_kind];
#define PartCache_mxpr00 (sizeof(tpr00_PartCache)/sizeof(tpr00_PartCache*))

/* End of definitions from newer kernel versions */

#if defined ( VSP00_H ) || defined ( GSP00_H )
#define sp1p_buf            variant.C_1.sp1p_buf_F
#define tsp1_segment_header_02 tsp1_segment_header
#define tsp1_segment_header_01 tsp1_segment_header
#define sp1p_part_header    variant.C_1.sp1p_part_header_F
#define sp1s_segm_header_01 variant.C_1.sp1s_segm_header_F
#define sp1s_segm_header    variant.C_1.sp1s_segm_header_F
#define sp1c_prepare        variant.C_1.sp1c_prepare_F
#define sp1c_mass_cmd       variant.C_1.sp1c_mass_cmd_F
#define sp1c_mess_type      variant.C_1.sp1c_mess_type_F
#define sp1c_sqlmode        variant.C_1.sp1c_sqlmode_F
#define sp1c_parsing_again  variant.C_1.sp1c_parsing_again_F
#define sp1s_segm_len       variant.C_1.sp1s_segm_len_F
#define sp1_segm            variant.C_2.sp1_segm_F
#define sp1r_returncode     variant.C_2.sp1r_returncode_F
#define sp1r_errorpos       variant.C_2.sp1r_errorpos_F
#define sp1r_sqlstate       variant.C_2.sp1r_sqlstate_F
#define sp1r_extern_warning variant.C_2.sp1r_extern_warning_F
#define sp1r_function_code  variant.C_2.sp1r_function_code_F
#else

#define tsp1_segment_header_01 tsp1_segment_header
#define sp1s_segm_header_01 segh_01
#define sp1c_prepare        segh_01.sp1c_prepare
#define sp1c_mass_cmd       segh_01.sp1c_mass_cmd
#define sp1c_mess_type      segh_01.sp1c_mess_type
#define sp1c_parsing_again  segh_01.sp1c_parsing_again
#define sp1c_sqlmode        segh_01.sp1c_sqlmode
#define sp1c_commit_immediately segh_01.sp1c_commit_immediately
#define sp1c_with_info      segh_01.sp1c_with_info
#define sp1s_segm_len       segh_01.sp1s_segm_len
#define sp1r_function_code  segh_02.sp1r_function_code

#define mxsp_c2         MXSP_C2
#define mxsp_c8         MXSP_C8
#define mxsp_c18        MXSP_C18
#define mxsp_c24        MXSP_C24
#define mxsp_c64        MXSP_C64
#define mxsp_c70        MXSP_C70
#define mxsp_name       MXSP_NAME
#define mxsp_termid     MXSP_TERMID
#define NODEID_MXSP00     MX_NODEID
#define CRYPTPW_MXSP00    MAXCRYPTPWLENGTH
#define mxsp_buf        MAXBUFFERLENGTH

#define bsp_c1          BSP_C1
#define bsp_c18         BSP_C18
#define bsp_c64         BSP_C64
#define bsp_nodeid      BSP_NODEID

#define sp1sm_internal        SP1SM_INTERNAL
#define sp1sm_ansi            SP1SM_ANSI
#define sp1sm_db2             SP1SM_DB2
#define sp1sm_oracle          SP1SM_ORACLE

#define sp1pk_command         SP1PK_COMMAND
#define sp1pk_data            SP1PK_DATA
#define sp1pk_resultcount     SP1PK_RESULTCOUNT
#define sp1pk_errortext       SP1PK_ERRORTEXT
#define sp1pk_resulttablename SP1PK_RESULTTABLENAME
#define sp1pk_parsid          SP1PK_PARSID
#define sp1pk_shortinfo       SP1PK_SHORTINFO
#define sp1pk_columnnames     SP1PK_COLUMNNAMES
#define sp1pk_tablename       SP1PK_TABLENAME
#define sp1pk_session_info_returned SP1PK_SESSION_INFO_RETURNED

#define os_os2                OS_OS2
#define os_win32              OS_WIN32

#define sp1ce_ok              SP1CE_OK
#define sp1ce_notok           SP1CE_NOTOK
#define sp1ce_tasklimit       SP1CE_TASKLIMIT
#define sp1ce_timeout         SP1CE_TIMEOUT
#define sp1ce_crash           SP1CE_CRASH
#define sp1ce_start_required  SP1CE_START_REQUIRED
#define sp1ce_shutdown        SP1CE_SHUTDOWN

#define sp1m_dbs              SP1M_DBS


#define tsp00_TaskId  PROCESS_ID
typedef unsigned short  tsp00_Uint2;
typedef unsigned int    tsp00_Uint4;

#define vf_ok           VF_OK
#define vf_eof          VF_EOF
#define vread           VREAD
#define vappend         VAPPEND
#define voverwrite      VOVERWRITE
#define vm_nodata       VM_NODATA

#define sql_user        SQL_USER
#define sql_utility     SQL_UTILITY

#define csp_unicode      CSP_UNICODE
#define csp_unicode_swap CSP_UNICODE_SWAP

#define uni_fillup_field uni_fill_char
#define uni_del_trailing_blanks uni_tr_space

#define csp1_p_none                     CSP1_P_NONE
#define csp1_p_mass_command             CSP1_P_MASS_COMMAND
#define csp1_p_mselect_found            CSP1_P_MSELECT_FOUND
#define csp1_p_reuse_mselect_found      CSP1_P_REUSE_MSELECT_FOUND
#define csp1_p_for_upd_mselect_found    CSP1_P_FOR_UPD_MSELECT_FOUND
#define csp1_p_reuse_upd_mselect_found  CSP1_P_REUSE_UPD_MSELECT_FOUND
#define csp1_p_mass_command             CSP1_P_MASS_COMMAND
#define csp1_p_mfetch_found             CSP1_P_MFETCH_FOUND
#define csp1_p_fetch_found              CSP1_P_FETCH_FOUND
#define csp1_p_select_for_update_found  CSP1_P_SELECT_FOR_UPDATE_FOUND
#define csp1_p_reuse_update_sel_found   CSP1_P_REUSE_UPDATE_SEL_FOUND

#define csp1_nil_fc                     CSP1_NIL_FC
#define csp1_select_fc                  CSP1_SELECT_FC
#define csp1_select_direct_fc           CSP1_SELECT_DIRECT_FC
#define csp1_mselect_into_fc            CSP1_MSELECT_INTO_FC
#define csp1_mfetch_first_fc            CSP1_MFETCH_FIRST_FC
#define csp1_mfetch_same_fc             CSP1_MFETCH_SAME_FC
#define csp1_string_command_fc          CSP1_STRING_COMMAND_FC

#define sp1pa_last_packet        SP1PA_LAST_PACKET

typedef enum tsp8_uni_option {
  uni_change_to_upper,
  uni_fillup_field,
  uni_del_trailing_blanks
} tsp8_uni_option;

typedef enum  tsp8_uni_error {
            uni_ok = 0,
            uni_no_such_encoding,
            uni_not_translatable,
            uni_dest_too_short,
            uni_src_too_short,
            uni_is_singlebyte,
            uni_is_multibyte,
            uni_is_incomplete
} tsp8_uni_error;

typedef enum tsp8_uni_load_error_Enum
{
    uni_load_ok,
    uni_encoding_not_found,
    uni_no_dblang_found,
    uni_cannot_open_infofile,
    uni_cannot_open_localefile,
    uni_cannot_open_longcolfile,
    uni_file_error_localefile,
    uni_file_error_longcolfile,
    uni_memory_alloc_failed,
    uni_encoding_already_loaded,
    uni_bad_localefile,
    uni_bad_infofile
} tsp8_uni_load_error_Enum;

#endif

/* __VPR00C_H__ */
#endif

