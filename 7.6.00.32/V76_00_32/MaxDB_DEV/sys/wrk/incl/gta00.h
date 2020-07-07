
/* some common prolog for every module
 */

#ifndef GTA00_H
#define GTA00_H


/******************************************
 *            I N C L U D E S             *
 ******************************************/


#ifndef GSP00_2_H
#include "gsp00_2.h"
#endif

#ifndef GSP00_H
#include "gsp00.h"
#endif

#ifndef GGG00_H
#include "ggg00.h"
#endif

#ifndef GUT00_H
#include "gut00.h"
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

#define cta_max_input_fields                 2000
#define cta_max_muli_buf                        5
#define cta_max_muli_procs                     50
#define cta_max_parsing                        10
#define cta_max_res_fields                   1024
#define cta_n_buflimit                  "BUFLIMIT    "
#define cta_n_config                    "CONFIG      "
#define cta_n_connect                   "CONNECT     "
#define cta_n_data_input                "DATA        "
#define cta_n_declare                   "DECLARE     "
#define cta_n_endsum                    "ENDSUM      "
#define cta_n_executing                 "EXECUTING   "
#define cta_n_fetch                     "FETCH       "
#define cta_n_file                      "FILE        "
#define cta_n_getval                    "GETVAL      "
#define cta_n_load                      "LOAD        "
#define cta_n_masscmd                   "MASSCMD     "
#define cta_n_maxbuf                    "MAXBUF      "
#define cta_n_minbuf                    "MINBUF      "
#define cta_n_no_data_input             "NODATA      "
#define cta_n_no_masscmd                "NO_MASSCMD  "
#define cta_n_no_parameter              "NO_PARAMETER"
#define cta_n_noreal                    "NOREALTIME  "
#define cta_n_notime                    "NOTIME      "
#define cta_n_online                    "ONLINE      "
#define cta_n_parameter                 "PARAMETER   "
#define cta_n_pars_execute              "PARS_EXECUTE"
#define cta_n_pars_then_ex              "PARS_THEN_EX"
#define cta_n_parsing                   "PARSING     "
#define cta_n_putval                    "PUTVAL      "
#define cta_n_real                      "REALTIME    "
#define cta_n_run                       "RUN         "
#define cta_n_select                    "SELECT      "
#define cta_n_show                      "SHOW        "
#define cta_n_sum                       "SUM         "
#define cta_n_switch                    "SWITCH      "
#define cta_n_switch_limited            "SWITCHLIMIT "
#define cta_n_syntax                    "SYNTAX      "
#define cta_n_tbuflimit                 "TBUFLIMIT   "
#define cta_n_time                      "TIME        "
#define cta_n_tmaxbuf                   "TMAXBUF     "
#define cta_n_tminbuf                   "TMINBUF     "
#define cta_n_tswitch                   "TSWITCH     "
#define cta_n_tswitch_limited           "TSWITCHLIMIT"
#define mxta_old_long_desc                     32    /*version < csp1_first_sp1_version*/

/******************************************
 *               T Y P E S                *
 ******************************************/

typedef ARRAY_TYPE_MSP00 (         tts_param_array, tsp1_param_info, cta_max_res_fields);
typedef ARRAY_TYPE_MSP00 (         tts_ident_array, tsp00_KnlIdentifier, cta_max_res_fields);

typedef struct                     tta_fields
{
    tsp00_Int2          cnt_res_fields;
    tts_param_array     res_fieldparam;
    tts_ident_array     res_fieldname;
} tta_fields;

typedef struct                     tta_sresults
{
    tsp00_Int2          cnt_sres_fields;
    tsp00_Int2          n_sres_filler;
    ARRAY_VAR_MSP00 (         sres_fields, tsp00_SfieldInfo, cta_max_res_fields);
} tta_sresults;
typedef ARRAY_TYPE_MSP00 (         tta_input_params, tsp1_param_info, cta_max_input_fields);

typedef struct                     tta_ex_in
{
    tsp00_Int2          cnt_input_fields;
    tsp00_Int2          act_cnt_fields;
    tta_input_params    input_params;
} tta_ex_in;

typedef enum tta_execution_kind_Enum
{
    tek_only_parsing,
    tek_only_executing,
    tek_parsing_executing,
    tek_pars_then_execute,
    tek_only_syntax,
    tek_dialog_call,
    tek_dbsload
} tta_execution_kind_Enum;
typedef tta_execution_kind_Enum tta_execution_kind_Param;
typedef ENUM_TYPE_MSP00 (          tta_execution_kind, tta_execution_kind_Enum, tek_dbsload, unsigned char);

typedef enum tta_simulation_Enum
{
    tasim_none,
    tasim_ascii,
    tasim_unicode
} tta_simulation_Enum;
typedef tta_simulation_Enum tta_simulation_Param;
typedef ENUM_TYPE_MSP00 (          tta_simulation, tta_simulation_Enum, tasim_unicode, unsigned char);

typedef struct                     tta_col_id
{
    tsp00_C8            descr;
    tsp1_param_info     shortinfo;
    pasbool             free;
} tta_col_id;
typedef ARRAY_TYPE_MSP00 (         tta_col_descr_list, tta_col_id, 5);

typedef struct                     tta_prot
{
    tsp00_C20           name;
    tsp00_C40           text;
    tsp00_VFilename     vfn;
    tut_vf_fileref      refer;
    tut_vf_fileref      refsafe;
    tsp00_Line          ln;
    tsp00_ErrText       errt;
    tsp00_Int2          err;
    pasbool             common;
    pasbool             no_prot;
    pasbool             session;
} tta_prot;

typedef struct                     tta_dbprocLongData
{
    integer             dbp_count;
    ARRAY_VAR_MSP00 (         dbp_length, integer, 5);
    ARRAY_VAR_MSP00 (         dbp_pos, integer, 5);
    ARRAY_VAR_MSP00 (         dbp_addr, tsp00_MoveObjPtr, 5);
} tta_dbprocLongData;

typedef struct                     tta_user_proc_globals
{
    tsp00_TaskId        procid;
    tsp00_KnlIdentifier username;
    tsp00_Pw            param_pw;
    tsp00_TermId        termid;
    tsp00_DbName        serverdb;
    tsp00_NodeId        servernode;
    tsp00_DbName        utilserverdb;
    tsp00_NodeId        utilservernode;
    tsp00_CryptPw       password;
    tsp00_C20           left_header;
    tsp00_C20           right_header;
    tsp00_C30           msg_header;
    tsp00_SwapKind      host_swap;
    tsp00_SwapKind      swap;
    tsp00_CodeType      host_code_type;
    tsp00_CodeType      term_code_type;
    tsp00_Int4          inlength;
    tsp00_Int2          user_no;
    tsp00_Int2          p_count;
    tsp00_Int4          cnt;
    tsp00_Int4          reference;
    tsp00_Int4          roldtime;
    tsp00_Int4          virt;
    tta_simulation      term_simulation;
    pasbool             to_break;
    pasbool             byte_swap;
    pasbool             cancel;
    pasbool             handler_order;
    pasbool             info;
    pasbool             is_batch;
    pasbool             is_connected;
    pasbool             is_data_input;
    pasbool             is_ok;
    pasbool             is_virt_term;
    pasbool             is_prompting;
    pasbool             long_errmsg;
    pasbool             rtime;
    pasbool             stop;
    pasbool             sum;
    pasbool             time;
    tsp4_xuser          xusertype;
    tsp00_CodeType      code_type;
    tsp00_SqlMode       sqlmode;
    tta_prot            prot;
    tta_col_descr_list  open_cols;
    tta_execution_kind  execution;
    pasbool             dbprocCall;
    ARRAY_VAR_MSP00 (         parsing, tsp00_C12, cta_max_parsing);
    ARRAY_VAR_MSP00 (         execute_input, tta_ex_in, cta_max_parsing);
    ARRAY_VAR_MSP00 (         execute_output, tta_ex_in, cta_max_parsing);
    tta_dbprocLongData  dbprocLongData;
    tsp00_Ctable        asc_to_ebc;
    tsp00_Ctable        ebc_to_asc;
    SET_VAR_MSP00 (           digiset, unsigned char, 255, 32);
    tsp00_Sname         first_token;
    tsp00_Sname         second_token;
    tsp00_Int4          bigcmd_offset;
    tut_terminal       *term;
    tut_pfkey           pfkey;
    tsp00_C5            appl_version;
    tsp00_C3            application;
    tta_fields          infparams;
    tsp00_Int2          extern_enc;
    tsp1_segment_ptr    curr_segm;
    tsp1_segment_ptr    curr_ret_segm;
    tsp00_Int2          more_segments;
    pasbool             appl_param_infos;
    pasbool             mass_cmd;
    pasbool             prepare;
    tsp1_command_options command_options;
    pasbool             with_abap_part;
    pasbool             file_info;
    tsp00_Int2          longdesccnt;
    tsp00_Line          longdescs;
    pasbool             shared_sql_drop;
    pasbool             shared_drop_trans;
    tsp00_Int4          longdem_cnt;
    tsp00_Int4          longdem_pos;
    tsp00_Int4          longdem_length;
    pasbool             longdem_atonce;
    pasbool             variable_input;
    pasbool             check_scrollopt;
    pasbool             autocommit;
    pasbool             ascii_in_output;
    tsp00_Int4          with_module_part;
} tta_user_proc_globals;
typedef tta_user_proc_globals     *tta_user_proc_ptr;
typedef tut_terminal              *tta_terminal_ptr;
typedef ARRAY_TYPE_MSP00 (         tta_muli_waits, tsp00_Int2, cta_max_muli_procs);

typedef struct                     tta_muli_user
{
    tsp00_TaskId        procid;
    tta_user_proc_ptr   upg_ptr;
    tta_terminal_ptr    term_ptr;
    tsp00_Int2          next_wait;
    tsp00_Int2          my_buf;
    tsp00_Int2          rc;
    pasbool             busy;
    pasbool             us_fill1;
    tta_muli_waits      wait;
} tta_muli_user;
typedef ARRAY_TYPE_MSP00 (         tta_all_muli_users, tta_muli_user, cta_max_muli_procs - 0 + 1);

typedef enum tta_buf_state_Enum
{
    free_buf,
    dirty_buf,
    for_user,
    for_prot,
    for_handler
} tta_buf_state_Enum;
typedef tta_buf_state_Enum tta_buf_state_Param;
typedef ENUM_TYPE_MSP00 (          tta_buf_state, tta_buf_state_Enum, for_handler, unsigned char);

typedef struct                     tta_muli_buf
{
    tsp00_Int4          stamp;
    tsp00_Line          txt;
    tsp00_Int2          buflen;
    tta_buf_state       state;
    pasbool             filler;
} tta_muli_buf;
typedef ARRAY_TYPE_MSP00 (         tta_all_muli_buf, tta_muli_buf, cta_max_muli_buf);

struct tta00_TemplateDummy {
    tta_buf_state        dummy_tta_buf_state;
    tta_execution_kind   dummy_tta_execution_kind;
    tta_simulation       dummy_tta_simulation;
};


#endif
