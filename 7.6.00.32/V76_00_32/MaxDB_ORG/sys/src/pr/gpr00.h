/*!================================================================
 module:    gpr00.h

 responsible: BurkhardD
 
 special area:   General CPC Driver Header
 
 description: 
 
 see:  
 



    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
    ========== licence end




 ===================================================================*/

#ifndef __GPR00_H__
#define __GPR00_H__

/*==================================================================*
 *  INCLUDES                                                        *
 *==================================================================*/
#include "gsp00.h"
#include "vsp001.h"
#include "vpr05IfCom_String.h"  /*general string type definition*/


#if !defined(__cplusplus)
#include "heo102.h"
#else
#include "heo02.h"
#endif
#include "gmt90.h"
#include "vpr00c.h"
#include "vpi00c.h"
#include "vpr07Check.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#ifdef __cplusplus
extern "C" {                         /* Assume C declarations for C++ */
#endif  /* __cplusplus */

#define CPR_NULL_DATA                   (-1)
#define CPR_DATA_AT_EXEC                (-2)
#define CPR_NTS                         (-3)
#define CPR_NO_TOTAL                    (-4)
#define CPR_DEFAULT_PARAM               (-5)
#define CPR_IGNORE                      (-6)
#define CPR_BY_REF                      (-7)
#define CPR_BY_VALUE                    (-8)
#define CPR_LEN_DATA_AT_EXEC_OFFSET     (-100)
#define CPR_LEN_DATA_AT_EXEC(length)    (-length+CPR_LEN_DATA_AT_EXEC_OFFSET)

#define cpr_pointerlen (0)
/* Byteposition in the ParseID for Statemttype info. See vsp00 csp_p_* for more info */
#define cpr_p_precom_index (10)

/* Stopcount after an error is returned to the application */
#define ErrorLoop_mxpr00  (10)

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

typedef enum tpr00_IsWhatTypeEnum {
  cpr_is_empty          = -1,  
  cpr_is_false          = 0,
  cpr_is_true           = 1,  
  cpr_is_init           = 2,
  cpr_is_describe       = 3,
  cpr_is_loop_cnt       = 4,  
  cpr_is_loop           = 5,  
  cpr_is_first          = 6,  
  cpr_is_end            = 7,  
  cpr_is_only_pc        = 8,  
  cpr_is_statement      = 9
} tpr00_IsWhatTypeEnum;

typedef enum tpr00_InAreaTypeEnum {
  cpr_in_sqlva = 0,
  cpr_in_sqlst = 1
} tpr00_InAreaTypeEnum;


typedef enum tpr00_SQLModeEnum {
  cpr_kind_empty         =   0,
  cpr_kind_internal      =   1,
  cpr_kind_db2           =   2,
  cpr_kind_ansi          =   3,
  cpr_kind_oracle        =   4,
  cpr_kind_sapr3         =   5
} tpr00_SQLModeEnum;
 
 
enum tpr_hostvar_types_Enum
{
  cpr_vint2=0, 
  cpr_vint4, cpr_vreal4, cpr_vreal8, cpr_vdecimal, cpr_vzoned=5, 
  cpr_vchar, cpr_vcharc, cpr_vbchar, cpr_vdbchar, cpr_vbuf=10, 
  cpr_vscalar, cpr_vintarr, cpr_vrec, cpr_vnone, cpr_vstring=15, 
  cpr_vuns2, cpr_vuns4, cpr_vdecrec, cpr_varray, cpr_vstring1=20,
  cpr_vdbcharc, cpr_vlzone, cpr_vlszon, cpr_vtszon, cpr_vnumber=25,
  cpr_vbyte, cpr_vdbyte, cpr_vansicharc, cpr_vreal8sun, cpr_vstringc=30,
  cpr_vansichar, cpr_vunused1, cpr_vint8, cpr_vuns8, cpr_vstring4=35,
  cpr_viaddr, cpr_vraw, cpr_vrawc, cpr_vfile, cpr_vfilec=40,
  cpr_vunicode, cpr_vunicodec, cpr_vcharz, cpr_vonumber, cpr_vovarnum=45,
  cpr_vodate, cpr_vabaphandle, cpr_vvarchar_without_len, cpr_vint1, cpr_vuns1=50,
  cpr_vlong_desc, cpr_vucs2, cpr_vutf16, cpr_vstringunicode, cpr_vstringunicode4=55
};

typedef enum tpr00_DescribeKindEnum {
  Describe_Columns_epr00 = 1,
  Describe_Params_epr00 = 2,
  Describe_Both_epr00 = 3
} tpr00_DescribeKindEnum;

typedef enum tpr00_FetchSpecEnum {
        CPR_FE_EMPTY = 0,
        CPR_FE_FETCH,
        CPR_FE_FIRST,
        CPR_FE_NEXT,
        CPR_FE_LAST,
        CPR_FE_PREV,
        CPR_FE_SAME,
        CPR_FE_POSC,
        CPR_FE_POSV,
        CPR_FE_RELC,
        CPR_FE_RELV
} tpr00_FetchSpecEnum; /* fetch specification */
 

typedef enum tpr00_ErrorKeyEnum {
  ErrNoError,
  ErrRowNotFound,
  ErrDescNotFound,
  ErrSqlCancelled,
  ErrInpDefaultNotAllowed,
  ErrInpNullNotAllowed,
  ErrInpNumberInvalid,
  ErrInpNumberOverflow,
  ErrInvalidCommandstate,
  ErrInvalidProgram,
  ErrDBSessionNotAllowed,
  ErrNoSessionConnected,
  ErrNotImplemented,
  ErrOutNullNotAllowed,
  ErrOutNumberInvalid,
  ErrOutNumberOverflow,
  ErrParamListTooLong,
  ErrParamListTooShort,
  ErrParamListWrong,
  ErrMissingIndicatorvariable,
  ErrReflexStartRequired,
  ErrReflexTimeout,
  ErrReflexCrash,
  ErrRequestAreaOverflow,
  ErrTraceFileClose,
  ErrTraceFileOpen,
  ErrTraceFileWrite,
  ErrIncompatibleDatatype,
  ErrUnknownDatatype,
  ErrUnknownStatementName,
  ErrUnknownCursorName,
  ErrAreaOverflow,
  ErrMemoryAllocationFaild,
  ErrTooManyHostvar,
  ErrTooManyHostvarCall,
  ErrMacroNotInit,
  ErrParamlistNotAllowed,
  ErrInvalidDeclareCommand,
  ErrLoopInitWrong,
  ErrMissingVariableAddr,
  ErrDuplicateFetch,
  ErrDuplicateFetchCall,
  ErrTooManyFetchStatements,
  ErrCmdTooLong,
  ErrConnectSyntaxWrong,
  ErrReflexShutdown,
  ErrReflexConnectNotOk,
  ErrSelectParsidMissing,
  ErrImplicitConnectWrong,
  ErrMissingUserPassw,
  ErrUnknownSysprofile,
  ErrErrorNoFromSysprofile,
  ErrInpStringTruncated,
  ErrUnterminatedCString,
  ErrInvalidEscape,
  ErrMultiByteSetNotAllowed,
  ErrDataFileOpen,
  ErrDataFileWrite,
  ErrDataFileRead,
  ErrDataFileClose,
  ErrConvFromUnicodeImpossible,
  ErrConvToUnicodeImpossible,
  ErrNoLongColumnsFound,
  ErrUseCallStm,
  ErrValbufTooShort,
  ErrFfetchNotAllowed,
  ErrFetchMixNotAllowed,
  ErrSessionAlreadyConnected,
  ErrNoOutputVariable,
  ErrDifferSqlmodeNotAllowed,
  ErrMissingPreviousFetch,
  ErrTooManyStatements,
  ErrTooManyUsers,
  ErrSetFormatError,
  ErrXuserFileOpen,
  ErrCmdNotAvailable,
  ErrAbapDescNotSet,
  ErrAbapException,
  ErrConnectionBusy,
  ErrorUnknown
} tpr00_ErrorKeyEnum;

typedef enum tpr00_ComTypeEnum { 
      cpr_com_empty         = 0,     /* command type */
      cpr_com_sql           = 1,
      cpr_com_sql_conn      = 2,     /* sql connect statement */
      cpr_com_cancel        = 3,     /* sql cancel  statement */
      cpr_com_option        = 4,
      cpr_com_sql_close     = 5,
      cpr_com_crea_ind      = 6,
      cpr_com_sql_getval    = 7,     /* exec sql getval command */
      cpr_com_sql_open      = 8,     /* c-i exec sql open statement */
      cpr_com_commit_release = 9,     /* call interface */
      cpr_com_describe      = 10,    /* call interface */
      cpr_com_fetch_describe= 11,    /* call interface */
      cpr_com_mfetch        = 12,    /* call interface */
      cpr_com_mfetch_describe=13,    /* call interface */
      cpr_com_exec          = 14,    /* intermediate code (of pass1) */
      cpr_com_cics          = 15,    /* cics command found           */
      cpr_com_command       = 16,    /* exec command                 */
      cpr_com_unused        = 17,
      cpr_com_proc          = 18,    /* exec proc                    */
      cpr_com_opt           = 19,    /* command line options */
      cpr_com_sql_putval    = 20,    /* exec sql putval command */
      cpr_com_set_format    = 21,    /* set format rel3.0  */
      cpr_com_declare_cursor= 22,    /* declare cursor for ?? as param*/
      cpr_com_sql_fetch     = 23,    /* exec sql fetch command        */
      cpr_com_ora_descr_sel = 24,    /* oracle desc sel.list */
      cpr_com_oracle        = 25,    /* exec oracle          */
      cpr_com_trace         = 26,
      cpr_com_sql_open_keep = 27,    /* exec sql open stmt. with keep opt. */
      cpr_com_commit        = 28,
      cpr_com_rollback      = 29,
      cpr_com_rollback_release = 30,
      cpr_com_abort_session = 31,
      cpr_com_undef = -1
} tpr00_ComTypeEnum;

typedef enum tpr00_StateTypeEnum {
        cpr_state_empty = 0,
        cpr_state_parse,
        cpr_state_execute,
        cpr_state_command,
        cpr_state_prepare,
        cpr_state_macro,
	cpr_state_trace,
        cpr_state_sync_exec,
        cpr_state_newsync_exec,
        cpr_state_async_exec,
        cpr_state_decl_statem,
	cpr_state_decl_with_hold,
	cpr_state_execute_at_fetch
} tpr00_StateTypeEnum;

typedef enum tpr00_ResultTypeEnum {
  cpr_result_xuser_read_err = 1,
  cpr_result_system_required= 2,
  cpr_result_too_many_users = 3,
  cpr_result_user_passw     = 4,
  cpr_result_whenever_stop  = 5,
  cpr_result_option_error   = 6,
  cpr_result_trace_error    = 7,
  cpr_result_sqlxa_invalid  = 8
} tpr00_ResultTypeEnum;

typedef enum tpr00_ConnectionTypeEnum {
      cpr_co_ok                     = 0,
      cpr_co_dbms_start_required,
      cpr_co_too_many_users,
      cpr_co_restart_required,
      cpr_co_logon_required,
      cpr_co_user_or_pswd_illegal,
      cpr_co_crash,
      cpr_co_timeout,
      cpr_co_implicit,
      cpr_co_user_op
} tpr00_ConnectionTypeEnum;

typedef enum tpr00_kernel_errors_Enum {
  cpr_file_or_table_droped  = -108,
  cpr_err_cmd_not_available = -101,
  cpr_database_crash        =  800
} tpr00_kernel_errors_Enum;

#define Errorloop_mxpr00  (3)

typedef enum tpr_runtime_errors_Enum
{
  cpr_p_ok,
  cpr_area_overflow,
  cpr_cmd_not_available,
  cpr_cmd_too_long,
  cpr_could_not_load_lib,           /* -888 */
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
  cpr_abap_desc_not_set,            /* -861 */
  cpr_abap_exception,               /* -862 */
  cpr_connection_busy,              /* -810 */
  cpr_session_not_connected,        /* -821 */
  cpr_invalid_option,               /* -863 */
  cpr_unknown_cursor_name           /* -874 */
} tpr_runtime_errors_Enum;

typedef enum tpr_runtime_errors_Enum tpr_runtime_errors_epr00;

typedef enum tpr00_TraceParseID_Enum
{
  TraceParseIDInput_epr00        = 1,
  TraceParseIDOutput_epr00       = 2,
  TraceParseIDChange_epr00       = 3,
  TraceParseIDNew_epr00          = 4,
  TraceParseIDSQLStmt_epr00      = 5,
  TraceParseIDSQLStmtBlank_epr00 = 6,
  TraceParseIDUseExecute_epr00   = 7,
  TraceParseIDFetch_epr00        = 8,
  TraceParseIDBlank_epr00        = 9,
  TraceParseIDAgain_epr00        = 10,
  TraceParseIDVersion_epr00      = 11,
  TraceParseIDSelect_epr00       = 12,
  TraceParseIDDrop_epr00         = 13,
  TraceParseIDFromCache_epr00    = 14,
  TraceParseIDSQLStmtMass_epr00  = 15,
  TraceParseIDOther_epr00        = 99  
} tpr00_TraceParseID_Enum;

typedef struct                     tpr_longvarcharrec
{
    tsp00_Int2            lvccolno;
    tsp00_Int2            lvcdescno;
    tsp00_Int4            lvcfreepos;
    tsp00_Int2            lvcaoffset;
    pasbool             lvcnext;
    tsp00_ValMode         lvcvalmode;
    tsp00_Int2            lvcoldpos;
    tsp00_Int2            lvcoldlen;
    tsp00_Int4            lvcvalpos;
    tsp00_Int4            lvcvallen;
    tsp00_Int4            lvchostpos;
    tsp00_Int4            lvchostlen;
    tsp00_Int4            lvcindicator;
    tsp00_Int4            lvcvalmlen;
    tsp00_Uint1            lvcerror;
    pasbool             lvcpiecewise;
} tpr_longvarcharrec;

typedef struct tpr00_ParentDesc tpr00_ParentDesc;
typedef struct tpr00_ParentContainer tpr00_ParentContainer;

typedef enum tpr00_SessionInfoEnum {
  cpr_se_empty, cpr_se_primary, cpr_se_second
} tpr00_SessionInfoEnum;

typedef enum tpr00_ConnectTypeEnum {
  cpr_ci_implicit, cpr_ci_program, cpr_ci_connect, cpr_ci_ora_connect, cpr_ci_explicit
} tpr00_ConnectTypeEnum;
 

typedef enum tpr00_TraceTypeEnum {
      cpr_trace_empty, cpr_trace_off, cpr_trace_formal, cpr_trace_long, cpr_trace_modformal, cpr_trace_modlong
} tpr00_TraceTypeEnum;

typedef enum trp00_TraceTextEnum {
  cpr_tr_inp, cpr_tr_outp, cpr_tr_loop, cpr_tr_longvar, cpr_tr_arrcnt
} tpr00_TraceTextEnum;

#define ActualSession_cpr00 (1)

typedef char tpr00_IdentifierRawString [3*sizeof(tsp00_KnlIdentifierc)]; /*factor 3, because the max posible size (worst case) of a UTF8 character is 3 bytes*/

/*!
  Declaration: tpr00_CursorNameDesc
  Description: type for encoding independend cursor name
 */
typedef struct tpr00_CursorName 
{
  tpr05_String CursorName;
  tpr00_IdentifierRawString private_StrBuf; /*placeholder for constant stringbuffer - only for internal use!!!*/
}tpr00_CursorNameDesc;

/*!
  Declaration: tpr00_StmtNameStruct
  Description: type for encoding independend statement name
 */
typedef struct tpr00_StmtName 
{
  tpr05_String stStmtName;
  tpr00_IdentifierRawString private_StrBuf; /*placeholder for constant stringbuffer - only for internal use!!!*/
}tpr00_StmtNameStruct;


typedef enum tpr00_Version_Enum {
  VersionUnknown_cpr00 = 00000,
  Version61_cpr00 = 60100,
  Version62_cpr00 = 60200,
  Version71_cpr00 = 70100,
  Version72_cpr00 = 70200,
  Version721_cpr00 = 70201,
  Version722_cpr00 = 70202,
  Version730_cpr00 = 70300,
  Version731_cpr00 = 70301,
  Version740_cpr00 = 70400,
  Version742_cpr00 = 70402
} tpr00_Version_Enum;

/* Kernel Versions */
#define Actual_KernelVersion_cpr00       Version742_cpr00
#define Min_KernelVersion_cpr00          Version62_cpr00

#define SelDirFast_KernelVersion_cpr00   Version72_cpr00
#define MaxLenOnBlob_KernelVersion_cpr00 Version722_cpr00

/* CPC Runtime Versions */
#define Actual_CPCVersion_cpr00          Version742_cpr00
#define Min_CPCVersion_cpr00             Version731_cpr00

#define BlobSupport_CPCVersion_cpr00     Version722_cpr00


#define FeatureSet743_cpr00 { \
                              {sp1f_nil_feature,           0}, \
                              {sp1f_multiple_drop_parseid, 0}, \
                              {sp1f_space_option,          0}, \
                              {sp1f_variable_input,        0}, \
                              {sp1f_optimized_streams,     1}\
                            }

#define mxpr_macroline   (126)

typedef tsp00_KnlIdentifierc tpr00_DatabaseName;
typedef tsp00_NodeId        tpr00_ServerNode;
typedef tsp00_DbName        tpr00_ServerName;
typedef tsp00_KnlIdentifierc tpr00_UID;
typedef tsp00_CryptPw       tpr00_CryptPWD;
typedef tsp00_Int2          tpr00_Symbol;
typedef char                tpr_macroline[mxpr_macroline];
/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/
extern tsp00_SwapKind_Enum sqlLocalMachineSwapKind; /*normal, fullswap, halfswap depends on swap kind of local machine*/

#ifdef __cplusplus
}                                    /* End of extern "C" { */
#endif  /* __cplusplus */
#endif

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
