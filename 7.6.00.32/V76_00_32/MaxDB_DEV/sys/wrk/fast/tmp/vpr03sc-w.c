
#line 1 "/home/gunter/SANDBOX/MaxDB/V76_00_32/MaxDB_DEV/sys/wrk/incl//geo00.h"

/*!
  @file           geo00.h
   @author         DanielD
   @special area   RTE | Basic Declarations
   @brief          Basic Declarations
   @see            http://pwww.bea.sap-ag.de/Entwicklung/cpp/C-Style/commonDeclarations.html

\if EMIT_LICENCE

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


//
//   FreeBSD portions added by Kai Mosebach,
//   For more informations see : http://www.komadev.de/sapdb
//

\endif
 */



#ifndef GEO00_H
#define GEO00_H

#include <string.h>
#include <stdarg.h>

#ifndef SAPDB_USE_REUSESDK
#include "SAPDBCommon/SAPDB_Types.h"
#else
#include "SAPDBCommon/SAPDB_Types-sdk.h" /* nocheck */
#endif

/*
 * compiler specific stuff
 */
#if defined(WIN32)
#define NEEDED_WIN32_WINNT 0x0500

#if !defined(_WIN32_WINNT)
# define _WIN32_WINNT  NEEDED_WIN32_WINNT
#else
# if _WIN32_WINNT < NEEDED_WIN32_WINNT
#  undef _WIN32_WINNT
#  define _WIN32_WINNT  NEEDED_WIN32_WINNT
# endif
#endif


# include <malloc.h>
#if defined (RTE_USE_SOCK)
# include <winsock2.h>
#endif
# include <windows.h>
#else
# if defined(AIX)
#   include <malloc.h>
#   include <stdlib.h>
# else
#if !defined(NMP) && !defined(FREEBSD)
#   include <alloca.h>
#endif
# endif
#endif

#ifdef FREEBSD
#include <stdbool.h>
#endif

#ifndef RTE_COMPILERFEATURES_H
/* compatibility with old geo00.h */
#undef externC
#ifndef SAPDB_USE_REUSESDK
#include "RunTime/RTE_CompilerFeatures.h"
#else
#include "RunTime/RTE_CompilerFeatures-sdk.h"  /* nocheck */
#endif
#endif

/* end compiler specific stuff */

#define undef_ceo00 (-1)

#if defined (__cplusplus)

#define global          extern "C"
#if !defined(AIX) || __IBMCPP__ >= 500
/* WIN32, SUN, HPUX, DEC/OSF1, SNI/NMP, LINUX, FREEBSD */
#define TYPENAME_MEO00  typename
#define EXPLICIT_MEO00  explicit
#else
#define TYPENAME_MEO00
#define EXPLICIT_MEO00
#endif
#define REFCAST_MEO00(type_) *(type_ *)

#if !defined(HPUX) && !defined(AIX) && !defined(LINUX) && !defined(FREEBSD)
#define PLACEMENT_DELETE_MEO00
#endif

#if !defined(HPUX)
#define TEMPLATE_CLASS_MEO00
#define TEMPLATE_TYPE_MEO00
#else
#define TEMPLATE_CLASS_MEO00    class
#define TEMPLATE_TYPE_MEO00 typename
#endif

inline int feo00_MemCmp (const void * p1, const void * p2, int len)
{
    return memcmp (p1, p2, len);   /* this depends on the c library */
                                   /* currently, all platforms      */
                                   /* compare as unsigned           */
}

#else

#define global
#endif

#define MEMCMP_EO00(p1, p2, len)  memcmp (p1, p2, len)
#define MIN_EO00(v1, v2) (((v1) < (v2)) ? (v1) : (v2))
#define MAX_EO00(v1, v2) (((v1) > (v2)) ? (v1) : (v2))

#define ALIGN_EO00(v1, alignment) ((((v1) + (alignment) - 1)/(alignment))*(alignment))
#define ALIGN_2BYTE_EO00(v1)      ( ( ( (v1) + 1 ) / 2 ) * 2 )
#define ALIGN_4BYTE_EO00(v1)      ( ( ( (v1) + 3 ) / 4 ) * 4 )
#define ALIGN_8BYTE_EO00(v1)      ( ( ( (v1) + 7 ) / 8 ) * 8 )

#define C_STRING_ZERO_BYTE_LEN_EO00 1


/* portable integer types */
typedef unsigned char             teo00_Bool;
typedef unsigned char             teo00_Byte;
typedef signed   char             teo00_Int1;
typedef unsigned char             teo00_Uint1;
typedef signed   short            teo00_Int2;
typedef unsigned short            teo00_Uint2;
typedef signed   int              teo00_Int4;
typedef unsigned int              teo00_Uint4;


#if defined (_WIN32) || defined (_WIN64)
# if (defined (_INTEGRAL_MAX_BITS) && _INTEGRAL_MAX_BITS >= 64) || defined (_WIN64)
   typedef signed __int64         teo00_Int8;
   typedef unsigned __int64       teo00_Uint8;
# else
#  error __int64 type not supported
# endif
#else
# if defined (BIT64)
   typedef signed long            teo00_Int8;
   typedef unsigned long          teo00_Uint8;
# else
   typedef signed long long       teo00_Int8;
   typedef unsigned long long     teo00_Uint8;
# endif
#endif


#if defined (_WIN64) || defined (_WIN32)
   typedef signed   int           teo00_Int;
   typedef unsigned int           teo00_Uint;
#else
   typedef signed   long          teo00_Int;
   typedef unsigned long          teo00_Uint;
#endif

#if defined (_WIN64)
   typedef signed   __int64		  teo00_Longint;
   typedef unsigned __int64       teo00_Longuint;
#else
   typedef signed   long          teo00_Longint;
   typedef unsigned long          teo00_Longuint;
#endif

#if defined (_WIN32)
#if defined (RTE_USE_SOCK)
   typedef SOCKET            	  teo00_Socket;
#endif
#else
   typedef int               	  teo00_Socket;
#endif

#endif

#line 1 "/home/gunter/SANDBOX/MaxDB/V76_00_32/MaxDB_DEV/sys/wrk/incl//gpr00.h"
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
#line 1 "/home/gunter/SANDBOX/MaxDB/V76_00_32/MaxDB_DEV/sys/wrk/incl//gpr04.h"
/*!================================================================
 module:    gpr04.h

 responsible: BurkhardD

 special area:   CPCDrv | Client | Converting Data

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

#ifndef __GPR04_H__
#define __GPR04_H__

/*==================================================================*
 *  INCLUDES                                                        *
 *==================================================================*/
#ifndef sql_oracle
#define sql_oracle
#endif

#ifndef sql_db2
#define sql_db2
#endif

#include "gpr00.h"
#include "gpr01.h"

/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/

/* Appends the oracle hostvartypes. */
#define SQL_ORA_STREAM_HANDLE 301

/*
  Oracle conform data types for using in Oracle SQLDA
*/

enum SQLORATypes {
        SQL_ORA_VARCHAR2             =  1,   /* char[n] */
        SQL_ORA_NUMBER               =  2,
        SQL_ORA_FIXEDPOINTINTEGER    =  3,
        SQL_ORA_FLOATPOINT           =  4,
        SQL_ORA_NULLTERMINATEDSTRING =  5,
        SQL_ORA_VARNUM               =  6,
        SQL_ORA_PACKEDDECIMAL        =  7,
        SQL_ORA_LONG                 =  8,
        SQL_ORA_VARCHAR              =  9,
        SQL_ORA_ROWID                = 11,
        SQL_ORA_INTERNDATE           = 12,
	SQL_ORA_INTERNJULIAN         = 14,
        SQL_ORA_VARRAW               = 15,
	SQL_ORA_RAW                  = 23,
        SQL_ORA_LONGRAW              = 24,
        SQL_ORA_VFILE                = 39,   /* datafile Cobol */
        SQL_ORA_VFILEC               = 40,   /* datafile C */
	SQL_ORA_UNSIGNED             = 68,
	SQL_ORA_DISPLAY              = 91,
	SQL_ORA_LONGVARCHAR          = 94,
        SQL_ORA_LONGVARRAW           = 95,
        SQL_ORA_CHAR                 = 96,
        SQL_ORA_CHARZ                = 97,
	SQL_ORA_MLSLABEL             =106,
	SQL_ORA_LONG_UNICODE         =201,
        SQL_ORA_ABAP_HANDLE          =SQL_ORA_STREAM_HANDLE, /* 301 */
        SQL_ORA_VARCHAR_WITHOUT_LEN  =302,
	SQL_ORA_LONG_DESC            =401,  /* descriptor for long column */
        SQL_ORA_TCHAR                =410,  /* TCHAR if unicode null terminated UCS2 else null terminated char*/
        SQL_ORA_TVARCHAR             =411,  /* TCHAR if unicode -> UCS2[n+2] else -> ora_varchar[n+2]*/
        SQL_ORA_TVARCHAR4            =412,  /* TCHAR if unicode -> UCS2[n+4] else -> ora_varchar4[n+4]*/
	SQL_ORA_UCS2                 =420,  /* null terminated UCS2  string */
	SQL_ORA_UTF16                =421,  /* null terminated UTF16 string */
	SQL_ORA_VARUCS2_2            =422   /* UCS2 string without null termination and length info */
    }; /* ORACLE types */


/*==================================================================*
 *  FUNCTION PROTOTYPES                                             *
 *==================================================================*/

#ifdef __cplusplus
extern "C" {
#endif
void p04OraTypes2Internal (int T, tsp00_Int4 L, tsp00_Int2 * vt, tpr01_Precompiledfor PreFor);
void p04oradin (sqlcatype *sqlca, sqlxatype *sqlxa, sqlgaentry *gae,
		sqlcuentry *cu, void *sqlda, struct tpr_sqlloop *lp);
void p04oradout (sqlcatype *sqlca, sqlxatype *sqlxa, sqlgaentry *gae, void *sqlda, struct tpr_sqlloop *lp);
tsp00_Int2 p04oradsfi (sqlcatype *sqlca, sqlxatype *sqlxa, sqlorentry *ore, void *sqlda, char fetchdesc, tpr00_DescribeKindEnum DescribeKind);
void p04din (sqlcatype *sqlca, sqlxatype *sqlxa, sqlgaentry *gae,
sqlcuentry *cu, struct tpr_sqlloop *lp);
void p04dout (sqlcatype *sqlca, sqlxatype *sqlxa, sqlgaentry *gae, struct tpr_sqlloop *lp);
void p04sfi (sqlcatype *sqlca, sqlxatype *sqlxa, tpr00_DescribeKindEnum DescribeKind);
void
p04orat (long L, short T, const char *S, short C, char *name,
    tsp00_Int2 * vt, tsp00_Int4 * vl, tsp00_Int2 * vf, char *err, sqlxatype* sqlxa) ;
void
p04sftoora (struct SQLROW *row, struct SQLCOL *col,
    const tsp1_param_info *sfi, tsp00_Int4 *l, short *t) ;
void p04oracolinit (sqlcatype * sqlca, sqlxatype * sqlxa, void *sqlda);
void p04trline(sqlratype * sqlrap, char*szStr);
#ifdef P04TRENTRIES
void p04trentries(sqlratype * sqlrap, sqlcatype *cap, sqlxatype *xap, char *szStr);
#endif
void p04trint2(sqlratype * sqlrap, char *szStr, tsp00_Int2 cbValue);
void p04trint4(sqlratype * sqlrap, char *szStr, tsp00_Int4 cbValue);

void p04db2din (sqlcatype *sqlca, sqlxatype *sqlxa, sqlgaentry *gae, void *sqlda);
void p04db2dout (sqlcatype *sqlca, sqlgaentry *gae, void *sqlda);
tsp00_Int2 p04db2dsfi (sqlcatype *sqlca, sqlxatype *sqlxa, sqlorentry *ore, void *sqlda, char fetchdesc, char again);
typedef struct sqlvar sqlvar;
void p04sftodb2 (sqlcatype *sqlca, sqlxatype *sqlxa, struct sqlvar *var,
		 struct SQLROW *row, const tsp1_param_info *sfi, char init);
void p04trvinit (const char *vname, int ix, char *line, tsp00_Int2 *ll,
                int inp);
void p04trloop (sqlratype *sqlrap, int lpind);
void p04ptrvwrite (sqlratype *sqlrap,
                const struct SQLCOL *col, char *buf, tsp00_Int4 pos);
tsp00_Int2 *p04swap2 (tsp00_Int2 *dbuf, tsp00_Int2 *sbuf, tsp00_Int4 len);
void p04init (struct SQLROW *row);
tsp00_Int2 p04pdecode (const char *buf, tsp00_Int2 pos,
                        struct SQLROW *row, struct SQLCOL *col);
tsp00_Int2 p04decode (const tsp1_param_info *sfi,
                        struct SQLROW *row, struct SQLCOL *col);

void pr04ColNames2ore(sqlcatype *sqlca, sqlxatype *sqlxa,sqlkaentry *kae, sqlorentry *ore);
void p04GetSFInfo(sqlcatype *sqlca, sqlxatype *sqlxa, sqlkaentry *kae, sqlorentry *ore);
void p04SFInfo2oreRes(sqlcatype *sqlca, sqlxatype *sqlxa, sqlkaentry *kae, sqlorentry *ore);


void p04mode (sqlcatype *sqlca, sqlgaentry *gae);
void p04ocimode (sqlratype *sqlrap, sqlgatype *sqlgap, sqlgaentry *gae);
void p04coltobuf (sqlratype *sqlrap, char *databuf, tsp00_Int4 *buflen,
                struct SQLCOL *col, const char *vn, const void *va,
                tsp00_Int2 vt, tsp00_Int4 vl, tsp00_Int2 vd, tsp00_Int2 vf,
		tsp00_Int4 ival, tsp00_Int4 offs, int pno,
                struct SQLERROR *sqlemp);
void p04colfrombuf (sqlratype *sqlrap, char *databuf, tsp00_Int4 *buflen,
                struct SQLCOL *col, const char *vn, void *va,
                tsp00_Int2 vt, tsp00_Int4 vl, tsp00_Int2 vd, tsp00_Int2 vf,
		tsp00_Int4 *ival, int offs, int pno,
                struct SQLERROR *sqlemp);
void p04coltoindi (const sqlcatype *sqlca, sqlxatype *sqlxa,
                struct SQLCOL *col, sqlv1pointer va1, char *base,
		tsp00_Int4 ival, char *err);
int p04col1toindi (struct SQLCOL *col, tsp00_Int4 ival, void *va,
                        tsp00_Int2 vt, tsp00_Int2 vl, tsp00_Int2 vf);
void p04colfromindi (const sqlcatype *sqlca, sqlxatype *sqlxa,
                struct SQLCOL *col, sqlv1pointer va1, char *base,
		tsp00_Int4 *ival, char *err);
void p04err (sqlratype *sqlrap, struct SQLERROR *sqlemp, char err);
void p04invaliderr (sqlratype * sqlrap, const char *buf, struct SQLCOL *col,
	tsp00_Int2 vt, tsp00_Int4 vl, tsp00_Int2 vf, tsp00_Int2 inout);
tsp00_Int4 p04rescount (tsp1_segment *seg, tsp00_Int4 datalen, int *countlen);
void p04input (sqlcatype *sqlca, sqlxatype *sqlxa, sqlgaentry *gae, struct tpr_sqlloop *lp);
void p04output (sqlcatype *sqlca, sqlxatype *sqlxa, sqlgaentry *gae, struct tpr_sqlloop *lp);
void p04sqldin (sqlcatype *sqlca, sqlxatype *sqlxa, sqlgaentry *gae, void *sqlda,
                struct tpr_sqlloop *lp);
void p04sqldout (sqlcatype *sqlca, sqlxatype *sqlxa, sqlgaentry *gae, void *sqlda, struct tpr_sqlloop *lp);
tsp00_Int2 p04sqldsfi (sqlcatype *sqlca, sqlxatype *sqlxa, sqlorentry *ore, void *sqlda, char fetchdesc, char again, tpr00_DescribeKindEnum DescribeKind);
tsp00_Int2 p04or2sqlda (sqlcatype * sqlca, sqlxatype * sqlxa, sqlorentry *ore, tpr00_DescribeKindEnum DescribeKind);
void p04or2da (sqlcatype * sqlca, sqlxatype * sqlxa, sqlorentry *ore, tpr00_DescribeKindEnum DescribeKind);
void p04ofltchr (char *va, tsp00_Int4 vl, tsp00_Int4 *cl,
                 char *rb, tsp00_Int4 rbl, char *res);
#ifdef sql_sqldb
void p04sftosql (sqlvartype *var, struct SQLROW *row,
    const tsp1_param_info *sfi, char init);
#endif

void p04traceda (sqlratype * sqlrap, char *daIn, struct SQLCOL  *col, tsp00_Int2 sqldbmode);

BOOLEAN p04IsEmptyString(char *va, tsp00_Int2 coltype, tsp00_Int2 vt, tsp00_Int4 vl);
BOOLEAN p04IsBinaryType(tsp00_Int2 coltype);
tsp00_Int4 pr04GetLoopCnt(sqlcatype *sqlca, sqlxatype *sqlxa, struct tpr_sqlloop *lp);
#ifdef __cplusplus
}
#endif
#endif
#line 1 "/home/gunter/SANDBOX/MaxDB/V76_00_32/MaxDB_DEV/sys/wrk/incl//vpr07c.h"
#ifndef RC_INVOKED
#include "vpr07Macro.h"
typedef struct
{
  char *szVersion;
  int  cbBuild;
} tpr_pcVersion;

#ifdef __cplusplus
#define externC extern "C"
#else
#define externC
#endif

externC void p07_copyright (void);
externC void p07senderid (tsp00_Int2 * xalang, tsp00_C8 senderid, tsp00_Int2 * cmpkind);
externC void p07version (tsp00_C40 * vers);
externC char *pr07RelNr();
externC char *pr07MinRelNr();

#endif

#define P07_PRODNAME   PROD_NAME_SP100
#if CORRECTION_LEVEL_SP100 < 10
#define P07_RELSTR       MAJOR_VERSION_NO_STR_SP100 "." MINOR_VERSION_NO_STR_SP100 ".0" CORRECTION_LEVEL_STR_SP100
#else
#define P07_RELSTR       MAJOR_VERSION_NO_STR_SP100 "." MINOR_VERSION_NO_STR_SP100 "." CORRECTION_LEVEL_STR_SP100
#endif
#define P07_FILEVERSION   MAJOR_VERSION_NO_SP100,MINOR_VERSION_NO_SP100,CORRECTION_LEVEL_SP100,BUILD_PREFIX_SP100
#define P07_FILEVERSION_STR   MAJOR_VERSION_NO_STR_SP100"."MINOR_VERSION_NO_STR_SP100"."CORRECTION_LEVEL_STR_SP100"."BUILD_PREFIX_STR_SP100

#define P07_BUILD         32

#define P07_FORMATVERSION(szRelNr) sprintf(szRelNr, "%d%02d%02d", MAJOR_VERSION_NO_SP100, MINOR_VERSION_NO_SP100, CORRECTION_LEVEL_SP100)

#define P07_PCR_VERSION COMP_NAME_C_PREC_SP100 " " P07_RELSTR "    Build %s"

#define P07_COMPANY         COMPANY_NAME_SP100
#define P07_PC_RUNTIME      COMP_NAME_C_PREC_SP100" Runtime"
#define P07_PC_PRECOMPILER  "C-PreComp"
#define P07_PC_COPYR_RESERV COPYRIGHT_SP100

#define P07_PC_COPYR_VERSION P07_PC_RUNTIME" "P07_FILEVERSION_STR" "DATE_SP100

#define P07_CMPCALL   "CAL"
#define P07_CMPCPC    "CPC"
#define P07_CMPCOB    "COB"
#define P07_CMPODBC   "ODB"
#define P07_CMPOCI    "OCI"

#line 1 "/home/gunter/SANDBOX/MaxDB/V76_00_32/MaxDB_DEV/sys/wrk/incl//vpr01SQL.h"
/****************************************************************************

 module:    vpr01SQL.h

 -------------------------------------------------------------------------

 responsible: BurkhardD

 special area:   CPCDrv | Client | SQLStatment Verwaltung
 description:    SQL Verwaltung

 last changed: 2000-08-07  17:17 Marco Paskamp  Unicode Erweiterungen
 see also    :

 -------------------------------------------------------------------------





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




*****************************************************************************/

#ifndef __VPR01_SQL_H__
#define __VPR01_SQL_H__

/*==================================================================*
 *  INCLUDES                                                        *
 *==================================================================*/

#include "gpr00.h"
#include "gpr01.h"
#include "vpr09DynaDesc.h"

/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/
typedef struct tpr01_SQLAttribute {
  BOOLEAN massopen;
  BOOLEAN parspre;
} tpr01_SQLAttribute;

typedef struct tpr01_SQLDesc {
  tpr01_DescTypeEnum      DescType;
  struct tpr01_SQLContainer *SQL;
  struct tpr01_StmtNameDesc *StmtNameDesc;
  struct tpr01_CursorDesc   *CursorDesc;
  struct tpr01_ErrorDesc    *ErrorDesc;
  struct tpr01_ConDesc      *ConDesc;
  tpr00_ComTypeEnum ComType;
  tsp1_cmd_mess_type  MessType;
  tpr05_StringEncoding PacketEncoding;
  sqlprentry *pr;
  sqlkaentry *ka;
  sqlorentry *ore;
  sqlcuentry *cu;
  sqlxatype  *sqlxa;
  tpr01_SQLAttribute Attribute;
} tpr01_SQLDesc;


typedef struct tpr01_SQLContainer {
  struct tpr01_EnvDesc *EnvDesc;
  tpr09DynamicDescriptor *Desc;
  void                    (*InitCont)  (struct tpr01_SQLContainer *Cont, struct tpr01_EnvDesc *EnvDesc);
  tsp00_Int4              (*OpenCont)  (struct tpr01_SQLContainer *Cont);
  void                    (*CloseCont) (struct tpr01_SQLContainer *Cont);
  tpr01_SQLDesc *(*AddDesc)   (struct tpr01_SQLContainer *Cont);
  void           (*DeleteDesc) (tpr01_SQLDesc *SQLDesc);
  tsp00_Int4              (*GetCount)  (struct tpr01_SQLContainer *Cont);
  tpr01_SQLDesc *(*EnumDesc)  (struct tpr01_SQLContainer *Cont);
  tsp00_Int4              (*SetIndex)  (struct tpr01_SQLContainer *Cont, tsp00_Int4);
  tpr01_SQLStateBinary (*State ) (struct tpr01_SQLDesc *Desc);

  void        (*Prepare)  (struct tpr01_SQLDesc *Desc);
  boolean     (*TryExecute) (struct tpr01_SQLDesc *Desc);
  boolean     (*Execute)  (struct tpr01_SQLDesc *Desc);
  void        (*Open)     (struct tpr01_SQLDesc *Desc);
  void        (*Describe) (struct tpr01_SQLDesc *Desc);
  void        (*Close)    (struct tpr01_SQLDesc *Desc);
  void        (*Release)  (struct tpr01_SQLDesc *Desc);

  tsp00_Int4  (*SetError) (struct tpr01_SQLDesc *Desc, tpr_runtime_errors_Enum ErrorEnum);

  sqlcatype * (*GetSqlca) (struct tpr01_SQLDesc *Desc);
  sqlxatype * (*GetSqlxa) (struct tpr01_SQLDesc *Desc);
  int         (*PutSQL)   (struct tpr01_SQLDesc *Desc, tpr05_String *pSQLStatement, tpr01_SQLStatementOptionEnum fOption);

  int         (*GetSQL)   (struct tpr01_SQLDesc *Desc, tpr05_String **pSQLStatement, tpr01_SQLStatementOptionEnum fOption);

  void        (*InitDesc) (struct tpr01_SQLDesc *SQLDesc, sqlxatype *sqlxa);

} tpr01_SQLContainer;


/*==================================================================*
 *  PROTOTYPES                                                      *
 *==================================================================*/

externC tpr01_SQLContainer *pr01SQLNewCont(struct tpr01_EnvDesc *EnvDesc);

externC void pr01SQLDeleteCont(tpr01_SQLContainer *Cont);

externC void pr01SQLgetAllAttributes(struct tpr01_SQLDesc *SQLDesc);

#endif
#line 1 "/home/gunter/SANDBOX/MaxDB/V76_00_32/MaxDB_DEV/sys/wrk/incl//vpr07Macro.h"
/*!================================================================
 module:    vpr07Macro.h

 responsible: BurkhardD

 special area:   Usefull Preprozessor Macros

 description:

 see:

  -------------------------------------------------------------------------





    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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

/*==================================================================*
 *  MACROS                                                          *
 *==================================================================*/

#define PR07MAX(a, b) ((a > b) ? a : b)
#define PR07MIN(a, b) ((a < b) ? a : b)
#define PR07NUMELEM(a) (sizeof(a)/sizeof(a[0]))
/*Ascii Test*/
#define PR07CIS_ASCII(buf, entype) ((entype) == sp77encodingAscii ? 1 : ((buf) == '\0'))

#define PR07UNREFPARAM(a) a;
/* stolen from vos12uc */
#define PR07PUTBIT(b,i)             \
        { b [ ( unsigned ) i / 8 ] |=  ( 1 << (( unsigned ) i % 8 )); }
#define PR07CLRBIT(b,i)             \
        { b [ ( unsigned ) i / 8 ] &= ~( 1 << (( unsigned ) i % 8 )); }
#define PR07TSTBIT(b,i)             \
        ( b [ ( unsigned ) i / 8 ] &   ( 1 << (( unsigned ) i % 8 ))  )

/*==================================================================*
 *  FUNCTION PROTOTYPES                                             *
 *==================================================================*/

#ifndef externC
#if defined(__cplusplus)
#define externC extern "C"
#else
#define externC
#endif
#endif

externC void pr07C2P(void *pout, void *cin, int len);
externC void pr07P2C(void *cout, void *pin, int len);
externC void pr07Put2Up(char *cinout);
externC void pr07StripChar(char *cin, char *stripc);
#line 1 "/home/gunter/SANDBOX/MaxDB/V76_00_32/MaxDB_DEV/sys/wrk/incl//vpr04Param.h"
/*!================================================================
 module:    vpr04Param.h

 responsible: BurkhardD

 special area:   CPCDrv | Client | Parameterverwaltung

 description: Verwaltung von Hostvariablen

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

#ifndef __VPR04_PARAM_H__
#define __VPR04_PARAM_H__

/*==================================================================*
 *  INCLUDES                                                        *
 *==================================================================*/

#include "gpr00.h"
#include "vpr09DynaDesc.h"

/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/

typedef struct tpr04_HostVarDesc {
  sqlv1pointer  va1;
  sqlv1pointer  va1mn;
  sqlv1pointer  va1mx;
} tpr04_HostVarDesc;

typedef struct tpr04_ParamDesc {
  sqlparpointer     pParam;
  sqlv1pointer      sqlv1p;
  sqlv2pointer      sqlv2p;
  tpr04_HostVarDesc HostVarDesc;
} tpr04_ParamDesc;

typedef struct tpr04_ParamContainer tpr04_ParamContainer;

struct tpr04_ParamContainer {
  tpr09DynamicDescriptor *pDesc;
  tsp00_Int4 cbCount;
  sqlxatype  *sqlxap;
  void             (*InitCont) (tpr04_ParamContainer *pCont, sqlxatype *sqlxap);
  tsp00_Int4       (*OpenCont) (tpr04_ParamContainer *pCont);
  void             (*CloseCont) (tpr04_ParamContainer *pCont);
  tsp00_Int4       (*GetCount) (tpr04_ParamContainer *pCont);
  tpr04_ParamDesc *(*EnumDesc) (tpr04_ParamContainer *pCont);
  void *           (*GetHostVarAddr) (tpr04_ParamDesc *pDesc);
  tsp00_Int2       (*GetHostVarType) (tpr04_ParamDesc *pDesc);
};

/*==================================================================*
 *  PROTOTYPES                                                      *
 *==================================================================*/

tpr04_ParamContainer *pr04NewParamContainer(sqlxatype *sqlxap);

void pr04DeleteParamContainer(tpr04_ParamContainer *pCont);

#endif
#line 1 "/home/gunter/SANDBOX/MaxDB/V76_00_32/MaxDB_DEV/sys/wrk/incl//vpr03Packet.h"
/*!================================================================

  module:       vpr03Packet.h

  responsible:  BurkhardD

  special area: Auftragsinterface


  description:  Header fuer Verwaltung des Auftragsinterfaces

  see also:

  change history:

        2001-05-17 D025086 Added pr03PacketReqRec and pr03PacketHandleComError
 -------------------------------------------------------------------





    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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
#ifndef __VPR03_PACKET_H__
#define __VPR03_PACKET_H__

/*==================================================================*
 *  INCLUDES                                                        *
 *==================================================================*/

#include "gpr01.h"
#include "vpr01Con.h"
#include "gpr03.h"


/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/

#define PR03PACKET_UNKNOWN_VERSION "00000"

typedef  char tpr03_KernelVersionC[sizeof(PR03PACKET_UNKNOWN_VERSION)];

typedef enum tpr03_KernelParam_Enum {
UnicodeCatalog_epr03,
KernelVersionString_epr03,
KnlSessionID_epr03,
KernelFeatures_epr03
} tpr03_KernelParam_Enum;

/*==================================================================*
 *  PROTOTYPES                                                      *
 *==================================================================*/

externC BOOLEAN pr03PacketGetKernelParameters( tsp1_segment *segm_ptr,
					       tpr03_KernelParam_Enum fOption,
					       void *pKernelParam );

externC BOOLEAN pr03PacketSetEncoding( tsp1_packet    *pck, tpr05_StringEncoding encodingType );
externC BOOLEAN pr03PacketSetSwapKind(tsp1_packet *pck, tsp00_SwapKind_Enum swapKind);
externC tsp00_SwapKind_Enum pr03PacketGetSwapKind( tsp1_packet *pck );

externC tpr05_StringEncoding pr03PacketGetEncoding( tsp1_packet    *pck);

externC tsp1_packet* pr03PacketInit( tpr01_ConDesc *ConDesc,
                                     tsp1_cmd_mess_type MessType);

/*!
  Function:     pr03PacketNewPacket

  see also:

  Description:  Allocates and intits a pseudopacket with a cbSize Bytes size.
  This packet can't used to comunicate with the DB kernel.

  Arguments:
    cbSize   [in] Number of Bytes for the packet.

  return value: tsp1_packet* pointer to the new allocated packet.
 */

externC tsp1_packet *pr03PacketNewPacket(tsp00_Int4 cbSize);

/*!
  Function:     pr03PacketDeletePacket

  see also:

  Description:  Destroy a pseudopacket.

  Arguments:
    pPacket  [in] pointer to the pseudopacket.

  return value: none.
 */

externC void pr03PacketDeletePacket(tsp1_packet *pPacket);
externC void pr03PacketReqRec(tpr01_ConDesc *ConDesc, struct SQLERROR *sqlemp);
externC void pr03PacketHandleComError(tpr01_ConDesc *ConDesc);

#endif
#line 1 "/home/gunter/SANDBOX/MaxDB/V76_00_32/MaxDB_DEV/sys/wrk/incl//vpr00c.h"
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

#line 1 "/home/gunter/SANDBOX/MaxDB/V76_00_32/MaxDB_DEV/sys/wrk/incl//gmt90.h"
/*!================================================================
 module:    gmt90.h

 responsible: BurkhardD

 special area:   Traceoutput
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

#ifndef __GMT90_H__
#define __GMT90_H__
#if !defined M90_INC   /* Don't reinclude */
#define M90_INC
#ifndef RC_INVOKED
#if defined (__cplusplus)
extern "C"
{
#endif
extern int m90_trace_status;
#if defined (__cplusplus)
}
#endif

#define M90_TR_STRING  (1)
#define M90_TR_UCHAR   (2)
#define M90_TR_SCHAR   (3)
#define M90_TR_DWORD   (4)
#define M90_TR_SDWORD  (5)
#define M90_TR_UDWORD  (6)
#define M90_TR_WORD    (7)
#define M90_TR_SWORD   (8)
#define M90_TR_UWORD   (9)
#define M90_TR_SDOUBLE (10)
#define M90_TR_LDOUBLE (11)
#define M90_TR_SFLOAT  (12)
#define M90_TR_BOOL    M90_TR_WORD
#define M90_TR_BUFFER  (13)
#define M90_TR_HANDLE  (14)
#define M90_TR_PTR     (15)
#define M90_TR_INT     (16)
#define M90_TR_ENUM    (17)

#define M90_TR_ENTRY   (50)
#define M90_TR_EXIT    (51)
#define M90_TR_MSG     (52)

#define M90_TR_STRING_LEN (53)

#define M90_TRACERESET 	(0)
#define M90_TRACEON    	(1)
#define M90_TRACEOFF	(2)
#define M90_TRACEINIT   (3)
#define M90_TRACEDIALOG	(88)


/* special defines for M90_TR_STRING_LEN */

#define M90_NTS -3
#define M90_NULL_DATA -1

#define M90_TRACE_PREFIX "      "
#define M90_TRACE_MAXCHAR 20
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#if !defined WIN32
#ifndef SAPDB_FAST
#define M90_TRACE_OUTPUT(buffer)\
   if (mt90level <= m90maxlevel) {\
     fprintf(file_ptr,"%*c",(int) (mt90level > 20) ? 20 : mt90level,' ');\
     fprintf(file_ptr,"%s", buffer);\
     fflush(file_ptr);\
   }
#define M90_EXIT(x) exit(x)
#define M90_EXIT_MSG(msg) {printf(" %s\n", msg); exit(16);}
#else
#define M90_TRACE_OUTPUT(buffer)
#define M90_EXIT(x)
#define M90_EXIT_MSG(msg)
#endif

#define M90CRLF "\x0A"

#else

#include <windows.h>

typedef struct {
   int version;
   HWND hwnd;
   int status;
   char FileName[260];
   int TraceLevel;
} mt90TraceStatus;


#ifndef SAPDB_FAST
#define M90_TRACE_OUTPUT(xbuffer) mt90TraceOutPut(& file_handle, xbuffer)
#define M90_EXIT(x) FatalAppExit(0,"API_EXIT Called.")
#define M90_EXIT_MSG(msg) FatalAppExit(0,msg);
#else
#define M90_TRACE_OUTPUT(buffer)
#define M90_EXIT(x)
#define M90_EXIT_MSG(msg)
#endif

#define M90CRLF "\x0D\x0A"

unsigned int  m90ctropt();
BOOL PASCAL mt90ProcDebug(HWND, unsigned, WORD, LONG );
void mt90TraceSwitch(int Level);

#endif /* WIN32 */

#define M90_TR_INIFILE   "sql-db.ini"
#define M90_TR_FILENAME_ENTRY "LogFile"
#define M90_TR_LEVEL_ENTRY "TraceLevel"
#define M90_TR_BUFFERSIZE_ENTRY "BufferSize"
#define M90_TR_PROFENTRY "m90trace"
#define M90_TR_APPNAME "SAPDB Tech Trace Interface"

#define M90_MEMCPY memcpy
#define M90_MEMSET memset
#define M90_MEMCMP memcmp
#define M90_STRCPY strcpy
#define M90_STRNCPY strncpy
#define M90_STRCMP strcmp
#define M90_STRLEN strlen
#define M90_STRCAT strcat
#define M90_STRNCAT strncat
#define M90_STRCHR strchr
#define M90_STRRCHR strrchr
#define M90_SPRINTF sprintf
#define M90_NEAR(p) p


#define M90_TR_FILENAME "trace.log"

#if defined (__cplusplus)
extern "C"
{
#endif
void m90ctrac( unsigned short int type,
char * name,
void * ptr,
long int len,
long int pos
);

#ifndef SAPDB_FAST
#define M90TRACEINIT(status);\
	  {m90_trace_status = status;\
	     m90ctrac(0, 0, NULL, (long int) 0, (long int) 0);}

#define M90TRACE(type,name,ptr);\
	  {if (m90_trace_status == 1)\
	      m90ctrac(type, name, (void *) ptr, (long int) 0, (long int) 0);}
#define M90TRACELEN(type,name,ptr,len);\
	  {if (m90_trace_status == 1)\
	      m90ctrac(type, name, (void *) ptr, (long int) len, (long int) 0);}
#define M90TRACEBUF(name, ptr, pos, len);\
	  {if (m90_trace_status == 1)\
	      m90ctrac(M90_TR_BUFFER, name, (void *) ptr, (long int) len, (long int) pos);}
#define M90TRACEIF(type,name,expr,ptr);\
	  {if (m90_trace_status == 1 && expr)\
	      m90ctrac(type, name, (void *) ptr, (long int) 0, (long int) 0);}

#define M90TRACELENIF(type,name,expr, ptr,len);\
	  {if (m90_trace_status == 1 && expr)\
	      m90ctrac(type, name, (void *) ptr, (long int) len, (long int) 0);}
#else
#define M90TRACEINIT(status);
#define M90TRACE(type,name,ptr);
#define M90TRACELEN(type,name,ptr,len);
#define M90TRACEBUF(name,ptr,pos,len);
#define M90TRACEIF(type,name,expr,ptr);
#define M90TRACELENIF(type,name,expr,ptr,len);
#endif
#if defined (__cplusplus)
}
#endif
#endif

#define M90_TRACEFILE               101
#define M90_TRACELEVEL              102
#define M90_APPLICATION             103
#define IDC_STATIC                  -1
#define DEBUGBOX                    9001
#endif
#endif /* __GMT90_H__ */
#line 1 "/home/gunter/SANDBOX/MaxDB/V76_00_32/MaxDB_DEV/sys/wrk/incl//gpr03.h"
/*!================================================================
  module:    gpr03.h

  responsible:  BurkhardD

  special area:   Header for Oldstyle PASCAL and C Modules (vpr03, vpr03c)

  description:

  see:

 -------------------------------------------------------------------





    ========== licence begin  GPL
    Copyright (c) 1999-2005 SAP AG

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

#ifndef __GPR03_H__
#define __GPR03_H__

/*==================================================================*
 *  INCLUDES                                                        *
 *==================================================================*/
#include "gpr00.h"
#include "gpr03m.h"
#include "gsp05.h"
#include "vpr01Con.h"

/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/

/*==================================================================*
 *  FUNCTION PROTOTYPES                                             *
 *==================================================================*/

#ifdef __cplusplus
extern "C" {
#endif
void p03cmdtrace (sqlratype * sqlra, sqlgaentry *gaen, tsp00_Int2 nlcnt, tsp00_Int2 comkind,
	     tpr05_String *pSQLStatement);
void p03connectcmdtrace (sqlratype * sqlra, sqlgaentry * gaen);
void p03ccmdinit  (struct tpr01_SQLDesc *SQLDesc, sqlcatype * sqlca, sqlgaentry * gaentry, char  m_type);
tsp00_Int4 p03cGetPartBuffer(sqlratype * sqlra, tsp1_part_kind_Param PartKind, tsp00_Uint1 **pBuf);
void p03gaentryinitdefault(sqlgaentry * gaentry);
void p03sqlgaentryinit (sqlgaentry * gaentry, tsp00_Int2 xuindex, struct SQLERROR * sqlemp);
void p03returncodeget (sqlcatype *sqlca, sqlxatype *sqlxa);
void p03cseterror (struct SQLERROR * sqlemp,  tpr_runtime_errors_Enum error);
void p03sendconnect (struct tpr01_SQLDesc *SQLDesc, sqlratype *sqlrap, sqlgatype *sqlga, sqlgaentry *gaen, struct SQLERROR * sqlemp);
void p03initsqlem (struct SQLERROR *sqlemp);
void p03setformat (struct tpr01_SQLDesc *SQLDesc, sqlratype * sqlrap, sqlgaentry * gaen, tsp00_Int2 datetime, struct SQLERROR * sqlemp);
void p03NologSession(sqlgatype * sqlga, tsp00_Int2 ganolog);
tsp1_packet *p03cpacketinit  (struct tpr01_SQLDesc *SQLDesc, sqlratype * sqlra, sqlgaentry * gaentry, char  m_type);
void p03find_part(sqlrapointer sqlrap, tsp1_part_kind_Param part_kind, tsp1_part_ptr *part_ptr);
void p03cputpart(sqlratype * sqlrap, sqlgaentry * gaent, char part_kind, void *ptr, tsp00_Int4 len, struct SQLERROR *sqlemp);
void p03cpparsid(sqlratype * sqlrap, sqlgaentry * gaent, tpr00_ParseId VAR_ARRAY_REF parsid, struct SQLERROR *sqlemp);
void p03csqlcaareainit(sqlcatype * sqlca);
void p03dropparsid(sqlcatype * sqlca, sqlxatype *sqlxa, sqlgaentry *ga, tpr00_ParseId VAR_ARRAY_REF parsid);
void p03putsysprofbuf(sqlratype * sqlra);
void p03modulnameput  (sqlcatype * sqlca, sqlxatype *sqlxa, sqlgaentry *ga, sqlkaentry *ka);
void pr03AbapInfoPutPart(sqlcatype *sqlca, sqlratype *sqlra, sqlgaentry * gaen);
void p03reqrecpacket(sqlcatype *sqlca, sqlcatype *sqlxa, sqlgaentry *ga);
void p03freeld(	struct SQLLD   **ld);
void p03tvfopentrace (sqlratype * sqlra, tsp00_VFileOpCodes read_write, struct SQLERROR *sqlemp);
void p03tvfwritetrace (sqlratype * sqlra, struct SQLERROR *sqlemp);
void p03tvfclosetrace (sqlratype * sqlra, struct SQLERROR *sqlemp);
void p03varparttrace (sqlratype * sqlra, sqlgaentry *gaen);
void pr03cSessionInfoGet(sqlratype * sqlrap, sqlgaentry * gaen);
void p03sqlaconnect (sqlratype * sqlrap, sqlgatype * sqlga, sqlgaentry *gaen, struct SQLERROR *sqlemp);
void p03putconnect (struct tpr01_SQLDesc *SQLDesc, sqlratype * sqlrap, sqlgaentry * gaent, struct SQLERROR *sqlemp);
void p03creqrecpacket (sqlratype * sqlra, sqlgaentry * gaentry, struct SQLERROR *sqlemp);
void p03sqlrelease (sqlratype * sqlrap, sqlgatype * sqlga, sqlgaentry *gaen, struct SQLERROR *sqlemp);
void p03conninfoget (sqlgatype * sqlga, sqlgaentry *gaen, tsp00_Int2 sess);
void p03connect (sqlxatype *sqlxa, sqlratype * sqlrap, sqlgatype * sqlga, sqlgaentry *gaen, tsp00_Int2 datetime, struct SQLERROR *sqlemp);
void p03clzuerror (sqlgaentry * gaentry, int lzu_prog, struct SQLERROR * sqlemp);
void p03messcodeget (sqlgaentry * gaentry);
void p03setsegmode (sqlratype * sqlra, sqlgaentry * gaentry);
void p03crequest (sqlratype * sqlra, sqlgaentry * gaentry,
    struct SQLERROR *sqlemp);
void p03creceive (sqlratype * sqlra, sqlgaentry * gaentry,
    struct SQLERROR *sqlemp);
void p03csqlclock (sqlratype * sqlra, long cmd);
void p03partptrinit (sqlratype * sqlra);
void p03packeterror (sqlratype * sqlra, sqlgaentry * gaentry,
    struct SQLERROR *sqlemp);
void p03cresultcount(sqlratype *sqlra, tsp00_Int4 * resultcount);
void p03warningset (struct SQLERROR *sqlemp, tsp00_C2 warnset);
void p03DebugBreak();
void p03getpid   (tsp00_Int4 *pid);
void p03gparsid (sqlratype * sqlra, struct SQLERROR *sqlemp, tpr00_ParseId VAR_ARRAY_REF parsid, tpr00_SessionID *sessionid);
void p03gselparsid (sqlratype * sqlra, struct SQLERROR *sqlemp, tpr00_ParseId VAR_ARRAY_REF parsid, tpr00_SessionID *sessionid);
void p03packeterror (sqlratype * sqlra, sqlgaentry * gaentry, struct SQLERROR *sqlemp);
void *p03dynalloc (sqlmdesc * md);
void pr03cOrInit(sqlorentry *ore);
void pr03cCuInit(sqlcuentry *cue);
void p03ctraceerror (sqlratype * sqlrap, struct SQLERROR *sqlemp);
void p03traceerror (sqlratype * sqlrap, struct SQLERROR *sqlemp);
void p03csqlemptosqlca (sqlcatype * sqlcap, struct SQLERROR *sqlemp);
void p03EnvDelete( sqlcatype *sqlcar );
void p03EnvClear( sqlcatype *sqlcar );
void *p03caci (sqlcatype *sqlcar, sqlxatype *sqlxar,
               tpr01_Precompiledfor pcfor);
void p03cancel (sqlcatype* sqlca, sqlgaentry * gaentry);
void p03orfree (sqlcatype *sqlca, struct SQLCOL *orptr, tsp00_Int2 kind);
void p03csqlinit (sqlratype * sqlrap, char comp, tsp00_Int2 language);
void p03datafopen (sqlratype * sqlra, tsp00_VFilename VAR_ARRAY_REF filename, tsp00_Int4 * fileno, tsp05_RteFileMode read_write, struct SQLERROR *sqlemp);
void p03cpresname (sqlratype * sqlrap, sqlgaentry * gaent,
		   tsp00_KnlIdentifier sqlresn,
		   struct SQLERROR *sqlemp);
void p03encrypt (tsp00_Name VAR_ARRAY_REF clearname, tsp00_CryptPw VAR_ARRAY_REF crypt);
tsp1_sqlmode pr03cMapSqlMode(tsp00_Int2 rakamode, tsp00_Int2 rasqlansi);
void p03getparameteraddr (sqlcatype *sqlca, sqlxatype *sqlxa, int *paix, int *cmcnt, void **vaaddr, int *vaind, void **indaddr, int *indind);
void
pr03cPutResultName(struct tpr01_ConDesc *ConDesc, tsp00_KnlIdentifier VAR_ARRAY_REF sqlresn, struct SQLERROR *sqlemp);
  tpr05_StringEncoding pr03cGetPacketEncoding();
#ifdef __cplusplus
}
#endif

#endif
#line 1 "/home/gunter/SANDBOX/MaxDB/V76_00_32/MaxDB_DEV/sys/wrk/incl//vpr03Part.h"
/*!================================================================

  module:       vpr03Part.h

  responsible:  BurkhardD

  special area: Functions to modify a Part


  description:  Header fuer Verwaltung des Auftragsinterfaces

  see also:

 -------------------------------------------------------------------





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
#ifndef __VPR03_PART_H__
#define __VPR03_PART_H__

/*==================================================================*
 *  INCLUDES                                                        *
 *==================================================================*/

#include "gpr00.h"

/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/

/*==================================================================*
 *  PROTOTYPES                                                      *
 *==================================================================*/

externC tsp1_part *pr03PartFind(sqlratype *sqlra, tsp1_part_kind_Param part_kind);
externC tsp1_part *pr03PartFindDirect(sqlratype *sqlra, tsp1_part_kind_Param part_kind);
externC void pr03PartInitCache(sqlratype *sqlra);
externC tsp00_Int4 pr03PartPut(tsp1_part *partPtr, tsp00_Int4 Offset, void *pBuf, tsp00_Int4 cbLen);
externC tsp00_Int4 pr03PartCopy(tsp1_part *partPtr, void *pBuf, tsp00_Int4 cbLen);
externC tsp00_Int4 pr03PartAppend(tsp1_part *partPtr, void *pBuf, tsp00_Int4 cbLen);
externC tsp00_Int4 pr03PartGetPartLength(tsp1_part *partPtr);
externC void pr03PartSetPartLength(tsp1_part *partPtr, tsp00_Int4 cbLen);
externC tsp00_Int4 pr03PartGetFreePartSpace(tsp1_part *partPtr);
externC tpr_runtime_errors_Enum pr03PartConverttoPart(tsp1_part *partPtr, tsp00_Int4 *Offset,tsp00_Int4 *cbPartLen, tpr05_StringEncoding encodingPart, void *pBuf, tsp00_Int4 cbBufLen, tpr05_StringEncoding EncodingBuf);
externC char *pr03PartGetRawPtr(tsp1_part *partPtr);
#endif
#line 1 "/home/gunter/SANDBOX/MaxDB/V76_00_32/MaxDB_DEV/sys/wrk/incl//vpr03sc.h"

#define CPR_INFO_UDT         (CPR_INFO_STRING+1)
#define CPR_INFO_ABAP_HANDLE (CPR_INFO_STRING+2)
#define CPR_VABAPHANDLE      (CPR_VODATE+1)

#ifdef __cplusplus
extern "C" {
#endif
void p03sABAPError(struct SQLERROR *sqlemp, char *szStr, char cbErrorCode);
#ifdef __cplusplus
}
#endif

#line 121 "vpr03sc-w.c"
/*PRETTY */

#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <time.h>
#include <ctype.h>
#include "livecachetypes.h" /* no check */
#line 130 "vpr03sc-w.c"
#include <unistd.h>
#line 134 "vpr03sc-w.c"

#ifndef DEBUG

#endif

/********  vsp26  proceduren  **********/
extern s26new_part_init (tsp1_packet *packet_ptr, tsp1_segment *segm_ptr, tsp1_part **part_ptr);
extern s26finish_part (tsp1_packet *packet_ptr, tsp1_part *part_ptr);
extern int s26nextpart (tsp1_part** part_ptr);
extern void s26find_part (tsp1_segment *segm_ptr, char part_kind, tsp1_part **part_ptr);
extern tsp00_Int4 s26size_new_part (tsp1_packet_ptr packet_ptr, tsp1_segment* segm);

extern void sqlnul (short *, short *, int *);

int
pr03WriteProc(SQLABAPWriteProc *WriteProc,
	      SQL_LC_ABAPTabParm *StreamParam,
	      unsigned char *pOutputStream,
	      int cbRowCount,
              int cbBufSize,
              void* pMask,
              boolean old);

int
pr03ReadProc(SQLABAPReadProc *ReadProc,
	      SQL_LC_ABAPTabParm *StreamParam,
	      unsigned char *pInputStream,
	      int cbPartLenMax,
	      int *cbBufSize,
	      int *cbRowCount,
	      void* pMask,
             boolean old);

void p03sABAPError(struct SQLERROR *sqlemp, char *szStr, char cbErrorCode)
{
  if (szStr) {
    size_t len = strlen(szStr);
    sqlemp->elzu = sp1ce_notok;
    memset(sqlemp->etext, ' ', sizeof(sqlemp->elzu));
    if (len > 0)
      memcpy((char*)sqlemp->etext, szStr, len);
  }
  else {
    sqlemp->elzu = sp1ce_ok;
  }
  p03cseterror (sqlemp, cbErrorCode);
}

SQLStreamDesc *p03sGetABAPDescriptor(void *sqlda, sqlint2 sqldbmode, OmsTypeInt4 ABAPTabId)
{
  SQLStreamDesc *descp=NULL;
  switch (sqldbmode) {
  case CPR_KIND_ORACLE:{}
  case CPR_KIND_SAPR3:{
    SQLDA *da=sqlda;
    int i;
    int null;
    for (i=0;i<da->F;i++) {
      short T;
      sqlnul ((short*)&da->T[i], &T, &null);
      if (T == SQL_ORA_ABAP_HANDLE) {
	if (ABAPTabId) {
	  if (((SQLStreamDesc *)(da->V[i]))->StreamParam->C_1.ABAPTab.hABAPTab.ABAPTabId != ABAPTabId)
	    continue;
	}
	descp = (SQLStreamDesc *)da->V[i];
	break;
      }
    }
    break;
  }
  case CPR_KIND_INTERNAL: {
    sqldatype *da = (sqldatype *) sqlda;
    int i;
    for (i=0;i<da->sqln;i++) {
      if (da->sqlvar[i].hostvartype == CPR_VABAPHANDLE) {
	if (ABAPTabId) {
	  if (((SQLStreamDesc *)(da->sqlvar[i].hostvaraddr))->StreamParam->C_1.ABAPTab.hABAPTab.ABAPTabId != ABAPTabId)
	    continue;
	}
	descp = (SQLStreamDesc *)da->sqlvar[i].hostvaraddr;
	break;
      }
    }
  }
  }
  return(descp);
}

SQLStreamDesc *pr03sGetHostVarABAPDescriptor(tpr04_ParamContainer *Param, sqlxatype *sqlxa, OmsTypeInt4 ABAPTabId)
{
  SQLStreamDesc *StreamDesc = NULL;
  if (Param) {
    tpr04_ParamDesc *ParamDesc;
    Param->OpenCont(Param);
    while(ParamDesc = Param->EnumDesc(Param)) {
      tsp00_Int2 fType = Param->GetHostVarType(ParamDesc);
      SQLStreamDesc *Desc = (SQLStreamDesc *)Param->GetHostVarAddr(ParamDesc);
      if (fType == CPR_VABAPHANDLE) {
	if (Desc && Desc->StreamParam->C_1.ABAPTab.hABAPTab.ABAPTabId == ABAPTabId) {
	  StreamDesc = Desc;
	  break;
	}
      }
    }
    Param->CloseCont(Param);
  }
  return StreamDesc;
}

void p03sSendABAPReturnPacket(struct tpr01_SQLDesc *SQLDesc, sqlratype * sqlra, sqlgaentry * gaentry, char *ptr, tsp00_Int4 cbLen, struct SQLERROR *sqlemp)
{
  tsp1_packet   *pck=NULL;
  tsp1_part     *part_ptr=NULL;
  pck = p03cpacketinit (SQLDesc, sqlra, gaentry, (char)sp1m_dbs);
  if (pck) {
    s26new_part_init (pck, sqlra->rasegptr, &part_ptr);
    if (part_ptr) {
      part_ptr->sp1p_part_header.sp1p_part_kind = (char)sp1pk_data;
      memcpy(part_ptr->sp1p_buf, ptr, cbLen);
      part_ptr->sp1p_part_header.sp1p_buf_len = cbLen;
      s26finish_part (pck, part_ptr);
      p03initsqlem (sqlemp);
      pr03PacketReqRec(SQLDesc->ConDesc, sqlemp);
    }
  }
}

void p03sSendABAPErrorPacket(struct tpr01_SQLDesc *SQLDesc, sqlratype * sqlra, sqlgaentry * gaentry,
char *szErrorText, char error, struct SQLERROR *sqlemp)
{
  tsp1_packet *pck=NULL;
  tsp1_part   *part_ptr=NULL;
  tsp00_Int4    cbLen;
  struct SQLERROR em;     /* localer Error um ueberschreiben zu verhindern*/
  if (sqlemp->ereturncode == 0) {
    p03sABAPError(sqlemp, szErrorText, error);
  }
  pck = p03cpacketinit (SQLDesc, sqlra, gaentry, (char)sp1m_dbs);
  if (pck) {
    tsp1_segment_header *seghp = (tsp1_segment_header *)&pck->sp1_segm;
    s26new_part_init (pck, sqlra->rasegptr, &part_ptr);
    if (part_ptr) {
      seghp->sp1r_returncode = sqlemp->ereturncode;
      part_ptr->sp1p_part_header.sp1p_part_kind = sp1pk_errortext;
      cbLen = PR07MAX(part_ptr->sp1p_part_header.sp1p_buf_len, sqlemp->etextlen);
      memcpy(part_ptr->sp1p_buf, sqlemp->etext, cbLen);
      part_ptr->sp1p_part_header.sp1p_buf_len = cbLen;
      s26finish_part (pck, part_ptr);
      p03initsqlem (&em);
      pr03PacketReqRec(SQLDesc->ConDesc, &em);
    }
  }
  if (em.ereturncode != 0) {
    if (sqlemp->ereturncode == 0) {    /* alten aber nicht ueberschreiben */
      memcpy(sqlemp, &em, sizeof(em));
    }
  }
}

void p03sPutStream(struct tpr01_SQLDesc *SQLDesc, sqlratype * sqlra, sqlgaentry * gaentry,
SQL_LC_ABAPTabParm *StreamParam, tsp00_Int4 cbRowCount,
SQLABAPReadProc *ReadProc, struct SQLERROR *sqlemp, boolean old, boolean first, boolean *last, void* pHiddenMask)
{
  tsp1_packet   *pck=NULL;
  tsp1_part     *part_ptr=NULL;
  int retcode = SQL_ABAP_OK;
  char szStr[30];

  tsp00_Int4 cbPartLenMax;
  unsigned char *pInputStream;
  if (!old) {
    p04trint4(sqlra, "PUTSTREAM EXT", cbRowCount);
  } else {
    p04trint4(sqlra, "PUTSTREAM", cbRowCount);
  }
  if (first)
    pck = p03cpacketinit (SQLDesc, sqlra, gaentry, (char)sp1m_dbs);
  else
    pck = SQLDesc->ConDesc->SegmDesc->packetPtr;
  if (pck) {
    s26new_part_init (pck, sqlra->rasegptr, &part_ptr);
    if (part_ptr) {
      part_ptr->sp1p_part_header.sp1p_part_kind = (char)sp1pk_data;
      part_ptr->sp1p_part_header.sp1p_arg_count = 0;
      cbPartLenMax = part_ptr->sp1p_part_header.sp1p_buf_size;
      pInputStream = part_ptr->sp1p_buf;
    }
  }
  else {
    p03sSendABAPErrorPacket(SQLDesc, sqlra, gaentry, "", cpr_memory_allocation_faild, sqlemp);
    return;
  }
  if (part_ptr) {
    tsp00_Int4 cbBufSize = 0;
    p04trint4(sqlra, "PartLenMax", cbPartLenMax);
    retcode = pr03ReadProc(ReadProc, StreamParam, pInputStream, cbPartLenMax, &cbBufSize, &cbRowCount, pHiddenMask, old);
    if (retcode == SQL_ABAP_EXCEPTION) {
      p03sABAPError(sqlemp, "", cpr_abap_exception);
      p03sSendABAPErrorPacket(SQLDesc, sqlra, gaentry, "", cpr_abap_exception, sqlemp);
      return;
    }
    if (SQL_ABAP_NO_MORE_DATA == retcode) {
      part_ptr->sp1p_part_header.sp1p_attributes[0] = 1 << sp1pa_last_packet;
      retcode = SQL_ABAP_OK;
    }
    else {
      *last = 1;
    }
    if (retcode == SQL_ABAP_OK) {
      part_ptr->sp1p_part_header.sp1p_arg_count= cbRowCount;
      part_ptr->sp1p_part_header.sp1p_buf_len  = cbBufSize;
      p04trint4(sqlra, "RowCount", part_ptr->sp1p_part_header.sp1p_arg_count);
      p04trint4(sqlra, "BufSize", part_ptr->sp1p_part_header.sp1p_buf_len);
      s26finish_part (pck, part_ptr);
      if (s26size_new_part (pck, sqlra->rasegptr) <= 0)
      {
        /* no next part possible */
        *last = true;
      }
      if (*last) {
        p03initsqlem (sqlemp);
        pr03PacketReqRec(SQLDesc->ConDesc, sqlemp);
      }
    }
    else {
      sprintf(szStr, "ReadProc failed (%d)", retcode);
      p03sSendABAPErrorPacket(SQLDesc, sqlra, gaentry, szStr, cpr_abap_desc_not_set, sqlemp);
      return;
    }
  }
  else {
    p03sSendABAPErrorPacket(SQLDesc, sqlra, gaentry, "", cpr_memory_allocation_faild, sqlemp);
  }
}

void p03sGetStream(struct tpr01_SQLDesc *SQLDesc, sqlratype * sqlra, sqlgaentry * gaentry,
SQL_LC_ABAPTabParm *StreamParam, SQLABAPWriteProc *WriteProc,
struct SQLERROR *sqlemp, boolean old,  tsp1_part **curr_part_ptr, boolean last)
{
  tsp1_packet   *pck=NULL;
  tsp1_part     *part_ptr=NULL;
  int retcode = SQL_ABAP_OK;

  unsigned char *pOutputStream;
  tsp00_Int4 cbRowCount=0;
  tsp00_Int4 cbBufSize=0;
  void*      pHiddenMask = 0;
  /*part_ptr = pr03PartFind(sqlra, sp1pk_data);*/
  part_ptr = *curr_part_ptr;
  if (part_ptr->sp1p_part_header.sp1p_buf_len > sizeof(tsp00_Int4))
  {
      pHiddenMask = &part_ptr->sp1p_buf[sizeof(tsp00_Int4)];
  }
  s26nextpart(&part_ptr);
  *curr_part_ptr = part_ptr;
  if (part_ptr) {
    pOutputStream = part_ptr->sp1p_buf;
    cbRowCount = part_ptr->sp1p_part_header.sp1p_arg_count;
    cbBufSize = part_ptr->sp1p_part_header.sp1p_buf_len;
    if (!old) {
      p04trint4(sqlra, "GETSTREAM EXT", cbRowCount);
    } else {
      p04trint4(sqlra, "GETSTREAM", cbRowCount);
    }
    p04trint4(sqlra, "BufSize", cbBufSize);
    if (cbRowCount <= 0 ) {
      retcode = SQL_ABAP_NO_MORE_DATA;
      p03sSendABAPReturnPacket(SQLDesc, sqlra, gaentry, (char*)&retcode, sizeof(retcode), sqlemp);
    }
    else {
      retcode = pr03WriteProc(WriteProc, StreamParam, pOutputStream, cbRowCount, cbBufSize, pHiddenMask, old);
      if (retcode == SQL_ABAP_EXCEPTION) {
	p03sABAPError(sqlemp, "", cpr_abap_exception);
	p03sSendABAPErrorPacket(SQLDesc, sqlra, gaentry, "", cpr_abap_exception, sqlemp);
	return;
      }
      if (retcode == SQL_ABAP_OK) {
      if (last)
	p03sSendABAPReturnPacket(SQLDesc, sqlra, gaentry, (char*)&retcode, sizeof(retcode), sqlemp);
      }
      else {
	char szStr[30];
	sprintf(szStr, "WriteProc failed (%d)", retcode);
	p03sSendABAPErrorPacket(SQLDesc, sqlra, gaentry, szStr, cpr_abap_desc_not_set, sqlemp);
      }
    }
  }
  else {
    p03sSendABAPErrorPacket(SQLDesc, sqlra, gaentry, "", cpr_memory_allocation_faild, sqlemp);
    return;
  }
}

typedef struct tpr03_SaveValues {
  tsp00_Int2 xakano;
  tsp00_Int2 raactsession;
} tpr03_SaveValues;

void pr03sBeforeCallBack(sqlcatype *sqlca, sqlxatype *sqlxa, tpr03_SaveValues *pSaves)
{
  pSaves->xakano = sqlxa->xakano;
  pSaves->raactsession = sqlca->sqlrap->raactsession;
}

void pr03sAfterCallBack(sqlcatype *sqlca, sqlxatype *sqlxa, tpr03_SaveValues *pSaves)
{
  sqlxa->xakano = pSaves->xakano;
  sqlca->sqlrap->raactsession = pSaves->raactsession;
}

extern tpr_pcVersion *p07pcversion;
void p03sABAPStream(sqlcatype *sqlca, sqlxatype *sqlxa, sqlgaentry *gaentry)
{
  tsp1_part *part_ptr;
  sqlratype *sqlrap = sqlca->sqlrap;
  sqlgatype *sqlgap = sqlca->sqlgap;
  sqlkaentry *ka = sqlxa->sqlkap+sqlxa->xakano-1;
  SQLStreamDesc *descp=NULL;
  tsp00_Int4 cbParamCount=0;
  tpr04_ParamContainer *Param=NULL;
  int tabCount=0;
  int maxFree;
  boolean isInStream;
#line 471 "vpr03sc-w.c"
  boolean old_streams = false;
#line 473 "vpr03sc-w.c"
  M90TRACE (M90_TR_ENTRY, "p03sABAPStream", 0);

  if (sqlca->sqlemp->ereturncode == 0 && sqlrap) {
    struct SQLERROR *sqlemp = sqlca->sqlemp;
    do {
      OmsTypeInt4 ABAPTabIds[16];
      int         requiredRows[16];
      void*       hiddenMask[16];
      int part_kind = sp1pk_abap_istream;
      int ix;
      boolean last = false;
      tsp1_segment   *segp= (sqlrap) ? (tsp1_segment*)sqlrap->rasegptr : NULL;
      tabCount      = 0;
      maxFree       = -1;
      if (!(part_ptr = pr03PartFindDirect(sqlrap, part_kind))) {
        isInStream = false;
        part_kind = sp1pk_abap_ostream;
        part_ptr = pr03PartFindDirect(sqlrap, part_kind);
        if (part_ptr)
        {
            tabCount = segp->variant.C_1.sp1s_segm_header_F.variant.C_1.sp1s_no_of_parts_F / 2;
            if (tabCount > 1) {
                part_ptr = pr03PartFindDirect(sqlrap, part_kind);
                if (!part_ptr) {
                  p03sSendABAPErrorPacket(sqlxa->xaSQLDesc, sqlrap, gaentry, "Missing output stream part",
                                          cpr_invalid_commandstate, sqlemp);
                  return;
                }
            }
            memcpy(&ABAPTabIds[0], part_ptr->sp1p_buf, sizeof(ABAPTabIds[0]));
            requiredRows[0] = part_ptr->sp1p_part_header.sp1p_arg_count;
         }
      }
      else
      {
          int partCount = segp->variant.C_1.sp1s_segm_header_F.variant.C_1.sp1s_no_of_parts_F;
          isInStream    = true;
          part_ptr = pr03PartFindDirect(sqlrap, part_kind);
          if (part_ptr
              && part_ptr->sp1p_part_header.sp1p_part_kind == sp1pk_abap_istream
              && part_ptr->sp1p_part_header.sp1p_buf_len >= sizeof(ABAPTabIds[0])) {
            int maskLen = part_ptr->sp1p_part_header.sp1p_buf_len - sizeof(ABAPTabIds[tabCount-1]);
            memcpy(&ABAPTabIds[0], part_ptr->sp1p_buf, sizeof(ABAPTabIds[0]));
            requiredRows[0] = part_ptr->sp1p_part_header.sp1p_arg_count;
            if (maskLen > 0)
            {
                hiddenMask[0] = malloc(maskLen);
                if (hiddenMask[0])
                {
                    memcpy (hiddenMask[0], &part_ptr->sp1p_buf[sizeof(ABAPTabIds[0])], maskLen);
                    maxFree = 0;
                }
                else
                {
                    /* TODO error handling */
                }
            }
            else
            {
                hiddenMask[0] = 0;
            }
            tabCount = 1;
          }
          else {
            p03sSendABAPErrorPacket(sqlxa->xaSQLDesc, sqlrap, gaentry, "Missing input stream part",
                                   cpr_invalid_commandstate, sqlemp);
            return;
          }
          for (ix = 1; ix < partCount; ++ix)
          {
              s26nextpart(&part_ptr);
              if (part_ptr
                  && part_ptr->sp1p_part_header.sp1p_part_kind == sp1pk_abap_istream
                  && part_ptr->sp1p_part_header.sp1p_buf_len >= sizeof(ABAPTabIds[tabCount-1])) {
                  int maskLen = part_ptr->sp1p_part_header.sp1p_buf_len - sizeof(ABAPTabIds[tabCount-1]);
                  memcpy(&ABAPTabIds[tabCount], part_ptr->sp1p_buf, sizeof(ABAPTabIds[tabCount]));
                  requiredRows[tabCount] = part_ptr->sp1p_part_header.sp1p_arg_count;
                  if (maskLen > 0)
                  {
                      hiddenMask[tabCount] = malloc(maskLen);
                      if (hiddenMask[tabCount])
                      {
                          memcpy (hiddenMask[tabCount], &part_ptr->sp1p_buf[sizeof(ABAPTabIds[tabCount])], maskLen);
                          maxFree = tabCount;
                      }
                      else
                      {
                          /* TODO error handling */
                      }
                  }
                  else
                  {
                      hiddenMask[tabCount] = 0;
                  }
                  ++tabCount;
                  if (16 == tabCount) break;
              }
              else {
                p03sSendABAPErrorPacket(sqlxa->xaSQLDesc, sqlrap, gaentry, "Missing input stream part",
                                        cpr_invalid_commandstate, sqlemp);
                return;
              }
          }
      }
      for (ix = 0; ix < tabCount; ++ ix)
      {
          p04trint4(sqlrap, "TabId", ABAPTabIds[ix]);
#line 588 "vpr03sc-w.c"
            descp = p03sGetABAPDescriptor(sqlca->sqlcxap->xasqldap,
                                          sqlca->sqldbmode, ABAPTabIds[ix]);
          if (!descp) {
            char szTmp[100];
            sprintf(szTmp, "Descriptor for TabID(%d) undefined", ABAPTabIds[ix]);
            p03sSendABAPErrorPacket(sqlxa->xaSQLDesc, sqlrap, gaentry, szTmp, cpr_abap_desc_not_set, sqlemp);
          }
          if (isInStream)
          {
            if (descp->ReadProc) {
#line 602 "vpr03sc-w.c"
                last = (ix == tabCount - 1);
                p03sPutStream(sqlxa->xaSQLDesc, sqlrap, gaentry, descp->StreamParam, requiredRows[ix],
                   descp->ReadProc, sqlemp, old_streams, 0 == ix, &last, hiddenMask[ix]);
#line 608 "vpr03sc-w.c"
            }
            else {
              p03sSendABAPErrorPacket(sqlxa->xaSQLDesc, sqlrap, gaentry, "ReadProc undefined", cpr_abap_desc_not_set, sqlemp);
            }
          }
          else {
            if (descp->WriteProc) {
#line 619 "vpr03sc-w.c"
            last = (ix + 1 == tabCount);
            p03sGetStream(sqlxa->xaSQLDesc, sqlrap, gaentry, descp->StreamParam,
               descp->WriteProc, sqlemp, old_streams, &part_ptr, last);
#line 626 "vpr03sc-w.c"
            if (sqlemp->ereturncode != 0) break;
            if (!last) {
                s26nextpart(&part_ptr);
                if (part_ptr
                    && part_ptr->sp1p_part_header.sp1p_part_kind == sp1pk_abap_ostream
                    && part_ptr->sp1p_part_header.sp1p_buf_len >= sizeof(ABAPTabIds[ix+1])) {
                  memcpy(&ABAPTabIds[ix+1], part_ptr->sp1p_buf, sizeof(ABAPTabIds[ix+1]));
                }
                else {
                  p03sSendABAPErrorPacket(sqlxa->xaSQLDesc, sqlrap, gaentry, "Missing output stream part",
                                          cpr_invalid_commandstate, sqlemp);
                }
            }
            }
            else {
              p03sSendABAPErrorPacket(sqlxa->xaSQLDesc, sqlrap, gaentry, "WriteProc undefined",
                                      cpr_abap_desc_not_set, sqlemp);
            }
          }
          if (Param) {
             pr04DeleteParamContainer(Param);
             Param = 0;
          }
          if (last) break;
      } /* end for */
      for (ix = 0; ix <= maxFree; ++ix)
      {
          free(hiddenMask[ix]);
      }
    } while (sqlemp->ereturncode == 0 && part_ptr);
    p03csqlemptosqlca (sqlca, sqlemp);
  }
  M90TRACE (M90_TR_EXIT, "p03sABAPStream", 0);
}


static char ident_sr[] = "@(#)vpr03sc-w dated 2006-07-17 05:56:31 used 2008-05-09 01:05:37";
extern char *vpr03sc_w_force_data () { return( ident_sr ); }
static char ident_mf[] = "@(#)vpr03sc-w FAST I386 UNIX LINUX PTOC -REL30=1 -RELVER=R76 -LC_STATE=RAMP -COMPILER=UNDEF -MACHINE=UNDEF -SQLODBC=1";
static char ident_cc[] = "@(#)vpr03sc-w cc -DREL30 -DLINUX -DI386 -DSAG -DSDB_RELEASE_070600 -I/home/gunter/SANDBOX/MaxDB/V76_00_32/MaxDB_DEV/usr/incl -I/home/gunter/SANDBOX/MaxDB/V76_00_32/MaxDB_DEV/sys/wrk/incl -I/home/gunter/SANDBOX/MaxDB/V76_00_32/MaxDB_DEV/sys/wrk/incl/SAPDB -DSYSV -D_SVID -I/usr/include/ncurses -w -Wall -D_FILE_OFFSET_BITS=64 -fPIC -DDYNAHASH -DHAS_NPTL -DSQLODBC -Dsql_oracle -Dsql_sqldb -fno-strict-aliasing -DSAPDB_FAST -D_REENTRANT -O3 -march=pentium -mcpu=pentiumpro -I/home/gunter/SANDBOX/MaxDB/DevTool/incl ";
extern char *vpr03sc_w_force_whatinfo () { return( ident_cc ); }
