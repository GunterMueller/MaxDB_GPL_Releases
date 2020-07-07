
/* some common prolog for every module
 */

#ifndef VSP004_H
#define VSP004_H


/******************************************
 *            I N C L U D E S             *
 ******************************************/


#ifndef GSP00_2_H
#include "gsp00_2.h"
#endif

#ifndef GSP00_H
#include "gsp00.h"
#endif

/******************************************
 *           C O N S T A N T S            *
 ******************************************/


/******************************************
 *               T Y P E S                *
 ******************************************/

typedef tsp00_C132                 tsp4_argline;
typedef tsp00_C18                  tsp4_xuserkey;
typedef tsp00_C8                   tsp4_sqlmode_name;
    /* ******* SQLARGS ******* */

typedef enum tsp4_info_result_Enum
{
    sp4inf_ok,
    sp4inf_illegal_user,
    sp4inf_illegal_component
} tsp4_info_result_Enum;
typedef tsp4_info_result_Enum tsp4_info_result_Param;
typedef ENUM_TYPE_MSP00 (          tsp4_info_result, tsp4_info_result_Enum, sp4inf_illegal_component, unsigned char);

typedef enum tsp4_component_Enum
{
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
    sp4co_sql_param
} tsp4_component_Enum;
typedef tsp4_component_Enum tsp4_component_Param;
typedef ENUM_TYPE_MSP00 (          tsp4_component, tsp4_component_Enum, sp4co_sql_param, unsigned char);

typedef enum tsp4_xuser_Enum
{
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
    sp4xu_sql_dblang
} tsp4_xuser_Enum;
typedef tsp4_xuser_Enum tsp4_xuser_Param;
typedef ENUM_TYPE_MSP00 (          tsp4_xuser, tsp4_xuser_Enum, sp4xu_sql_dblang, unsigned char);
typedef SET_TYPE_MSP00 (           tsp4_xuserset, tsp4_xuser_Enum, sp4xu_sql_dblang, 2);

typedef enum tsp4_command_mode_Enum
{
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
    sp4cm_sql_select
} tsp4_command_mode_Enum;
typedef tsp4_command_mode_Enum tsp4_command_mode_Param;
typedef ENUM_TYPE_MSP00 (          tsp4_command_mode, tsp4_command_mode_Enum, sp4cm_sql_select, unsigned char);

typedef struct                     tsp4_args_options
{
    tsp00_Int2          opt_filler1;
    pasbool             opt_filler2;
    tsp4_component      opt_component;
    union
    {
    /* sp4co_sql_precompiler */
        struct tsp4_args_options_sp4co_sql_precompiler
        {
            tsp00_VFilename     opt_modulefn_F;
            tsp00_VFilename     opt_tracefile_F;
            tsp00_Int2          opt_prognamel_F;
            tsp00_KnlIdentifier opt_progname_F;
            tsp00_Int2          opt_modulenamel_F;
            tsp00_KnlIdentifier opt_modulename_F;
            tsp00_Int2          opt_tpmonidl_F;
            tsp00_Name          opt_tpmonid_F;
            tsp00_Int2          opt_begmar_F;
            tsp00_Int2          opt_endmar_F;
            tsp00_Int2          opt_trace_F;
            tsp00_Int2          opt_fnind_F;
            tsp00_Int2          opt_datetime_F;
            tsp00_Int2          opt_dyn_F;
            tsp00_Int2          opt_tpmon_F;
            tsp00_Int2          opt_mode_F;
            tsp00_Int2          opt_extern_F;
            tsp00_Int2          opt_cansi_F;
            tsp00_Int2          opt_comp_F;
            tsp00_Int2          opt_prof_F;
            tsp00_Int4          opt_packetsize_F;
            pasbool             opt_list_F;
            pasbool             opt_precom_F;
            pasbool             opt_comment_F;
            pasbool             opt_traceswitch_F;
            pasbool             opt_silent_F;
            pasbool             opt_nowarn_F;
            pasbool             opt_lineno_F;
            pasbool             opt_lib_F;
            pasbool             opt_quote_F;
            pasbool             opt_decpoint_F;
            pasbool             opt_tabformat_F;
            pasbool             opt_version_F;
            pasbool             opt_help_F;
            pasbool             opt_unicode_F;
            tsp00_C1            opt_quo_F;
            tsp00_C1            opt_point_F;
            tsp4_argline        opt_puser_args_F;
        } C_sp4co_sql_precompiler;
    /* sp4co_sql_pc_runtime */
        struct tsp4_args_options_sp4co_sql_pc_runtime
        {
            tsp00_VFilename     opt_rtracefile_F;
            tsp00_Int2          opt_rtrace_F;
            tsp00_Int2          opt_rprof_F;
            tsp00_Int2          opt_rmfetch_F;
            tsp00_Int2          opt_rtime_limit_F;
            tsp00_Int2          opt_rstmt_cnt_F;
            tsp00_Int2          opt_rfiller_F;
            pasbool             opt_rnotracetime_F;
            pasbool             opt_rtraceswitch_F;
            pasbool             opt_rnosdf_F;
            pasbool             opt_rfiller1_F;
            tsp4_argline        opt_ruser_args_F;
        } C_sp4co_sql_pc_runtime;
    /* sp4co_sql_easy */
        struct tsp4_args_options_sp4co_sql_easy
        {
            tsp00_VFilename     opt_runfile_F;
            tsp00_Name          opt_object_owner_F;
            tsp00_Name          opt_object_name1_F;
            tsp00_Name          opt_object_name2_F;
            tsp00_Line          opt_parameter_F;
            pasbool             opt_append_F;
            pasbool             opt_prompt_F;
            pasbool             opt_codeonly_F;
            pasbool             opt_utility_session_F;
            tsp4_command_mode   opt_comm_mode_F;
            tsp00_CodeType      opt_code_F;
        } C_sp4co_sql_easy;
    /* sp4co_sql_audit */
        struct tsp4_args_options_sp4co_sql_audit
        {
            tsp00_VFilename     opt_aud_control_F;
            tsp00_VFilename     opt_aud_logfile_F;
            tsp00_Int4          opt_aud_log_blocksize_F;
            tsp00_VFilename     opt_aud_output_F;
            tsp4_command_mode   opt_aud_comm_mode_F;
        } C_sp4co_sql_audit;
    /* sp4co_sql_userx */
        struct tsp4_args_options_sp4co_sql_userx
        {
            tsp00_VFilename     opt_ux_runfile_F;
            tsp4_command_mode   opt_ux_comm_mode_F;
        } C_sp4co_sql_userx;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tsp00_VFilename    &opt_modulefn           ()  { return this->variant.C_sp4co_sql_precompiler.opt_modulefn_F; };
    tsp00_VFilename    &opt_tracefile          ()  { return this->variant.C_sp4co_sql_precompiler.opt_tracefile_F; };
    tsp00_Int2         &opt_prognamel          ()  { return this->variant.C_sp4co_sql_precompiler.opt_prognamel_F; };
    tsp00_KnlIdentifier &opt_progname           ()  { return this->variant.C_sp4co_sql_precompiler.opt_progname_F; };
    tsp00_Int2         &opt_modulenamel        ()  { return this->variant.C_sp4co_sql_precompiler.opt_modulenamel_F; };
    tsp00_KnlIdentifier &opt_modulename         ()  { return this->variant.C_sp4co_sql_precompiler.opt_modulename_F; };
    tsp00_Int2         &opt_tpmonidl           ()  { return this->variant.C_sp4co_sql_precompiler.opt_tpmonidl_F; };
    tsp00_Name         &opt_tpmonid            ()  { return this->variant.C_sp4co_sql_precompiler.opt_tpmonid_F; };
    tsp00_Int2         &opt_begmar             ()  { return this->variant.C_sp4co_sql_precompiler.opt_begmar_F; };
    tsp00_Int2         &opt_endmar             ()  { return this->variant.C_sp4co_sql_precompiler.opt_endmar_F; };
    tsp00_Int2         &opt_trace              ()  { return this->variant.C_sp4co_sql_precompiler.opt_trace_F; };
    tsp00_Int2         &opt_fnind              ()  { return this->variant.C_sp4co_sql_precompiler.opt_fnind_F; };
    tsp00_Int2         &opt_datetime           ()  { return this->variant.C_sp4co_sql_precompiler.opt_datetime_F; };
    tsp00_Int2         &opt_dyn                ()  { return this->variant.C_sp4co_sql_precompiler.opt_dyn_F; };
    tsp00_Int2         &opt_tpmon              ()  { return this->variant.C_sp4co_sql_precompiler.opt_tpmon_F; };
    tsp00_Int2         &opt_mode               ()  { return this->variant.C_sp4co_sql_precompiler.opt_mode_F; };
    tsp00_Int2         &opt_extern             ()  { return this->variant.C_sp4co_sql_precompiler.opt_extern_F; };
    tsp00_Int2         &opt_cansi              ()  { return this->variant.C_sp4co_sql_precompiler.opt_cansi_F; };
    tsp00_Int2         &opt_comp               ()  { return this->variant.C_sp4co_sql_precompiler.opt_comp_F; };
    tsp00_Int2         &opt_prof               ()  { return this->variant.C_sp4co_sql_precompiler.opt_prof_F; };
    tsp00_Int4         &opt_packetsize         ()  { return this->variant.C_sp4co_sql_precompiler.opt_packetsize_F; };
    pasbool            &opt_list               ()  { return this->variant.C_sp4co_sql_precompiler.opt_list_F; };
    pasbool            &opt_precom             ()  { return this->variant.C_sp4co_sql_precompiler.opt_precom_F; };
    pasbool            &opt_comment            ()  { return this->variant.C_sp4co_sql_precompiler.opt_comment_F; };
    pasbool            &opt_traceswitch        ()  { return this->variant.C_sp4co_sql_precompiler.opt_traceswitch_F; };
    pasbool            &opt_silent             ()  { return this->variant.C_sp4co_sql_precompiler.opt_silent_F; };
    pasbool            &opt_nowarn             ()  { return this->variant.C_sp4co_sql_precompiler.opt_nowarn_F; };
    pasbool            &opt_lineno             ()  { return this->variant.C_sp4co_sql_precompiler.opt_lineno_F; };
    pasbool            &opt_lib                ()  { return this->variant.C_sp4co_sql_precompiler.opt_lib_F; };
    pasbool            &opt_quote              ()  { return this->variant.C_sp4co_sql_precompiler.opt_quote_F; };
    pasbool            &opt_decpoint           ()  { return this->variant.C_sp4co_sql_precompiler.opt_decpoint_F; };
    pasbool            &opt_tabformat          ()  { return this->variant.C_sp4co_sql_precompiler.opt_tabformat_F; };
    pasbool            &opt_version            ()  { return this->variant.C_sp4co_sql_precompiler.opt_version_F; };
    pasbool            &opt_help               ()  { return this->variant.C_sp4co_sql_precompiler.opt_help_F; };
    pasbool            &opt_unicode            ()  { return this->variant.C_sp4co_sql_precompiler.opt_unicode_F; };
    tsp00_C1           &opt_quo                ()  { return this->variant.C_sp4co_sql_precompiler.opt_quo_F; };
    tsp00_C1           &opt_point              ()  { return this->variant.C_sp4co_sql_precompiler.opt_point_F; };
    tsp4_argline       &opt_puser_args         ()  { return this->variant.C_sp4co_sql_precompiler.opt_puser_args_F; };
    tsp00_VFilename    &opt_rtracefile         ()  { return this->variant.C_sp4co_sql_pc_runtime.opt_rtracefile_F; };
    tsp00_Int2         &opt_rtrace             ()  { return this->variant.C_sp4co_sql_pc_runtime.opt_rtrace_F; };
    tsp00_Int2         &opt_rprof              ()  { return this->variant.C_sp4co_sql_pc_runtime.opt_rprof_F; };
    tsp00_Int2         &opt_rmfetch            ()  { return this->variant.C_sp4co_sql_pc_runtime.opt_rmfetch_F; };
    tsp00_Int2         &opt_rtime_limit        ()  { return this->variant.C_sp4co_sql_pc_runtime.opt_rtime_limit_F; };
    tsp00_Int2         &opt_rstmt_cnt          ()  { return this->variant.C_sp4co_sql_pc_runtime.opt_rstmt_cnt_F; };
    tsp00_Int2         &opt_rfiller            ()  { return this->variant.C_sp4co_sql_pc_runtime.opt_rfiller_F; };
    pasbool            &opt_rnotracetime       ()  { return this->variant.C_sp4co_sql_pc_runtime.opt_rnotracetime_F; };
    pasbool            &opt_rtraceswitch       ()  { return this->variant.C_sp4co_sql_pc_runtime.opt_rtraceswitch_F; };
    pasbool            &opt_rnosdf             ()  { return this->variant.C_sp4co_sql_pc_runtime.opt_rnosdf_F; };
    pasbool            &opt_rfiller1           ()  { return this->variant.C_sp4co_sql_pc_runtime.opt_rfiller1_F; };
    tsp4_argline       &opt_ruser_args         ()  { return this->variant.C_sp4co_sql_pc_runtime.opt_ruser_args_F; };
    tsp00_VFilename    &opt_runfile            ()  { return this->variant.C_sp4co_sql_easy.opt_runfile_F; };
    tsp00_Name         &opt_object_owner       ()  { return this->variant.C_sp4co_sql_easy.opt_object_owner_F; };
    tsp00_Name         &opt_object_name1       ()  { return this->variant.C_sp4co_sql_easy.opt_object_name1_F; };
    tsp00_Name         &opt_object_name2       ()  { return this->variant.C_sp4co_sql_easy.opt_object_name2_F; };
    tsp00_Line         &opt_parameter          ()  { return this->variant.C_sp4co_sql_easy.opt_parameter_F; };
    pasbool            &opt_append             ()  { return this->variant.C_sp4co_sql_easy.opt_append_F; };
    pasbool            &opt_prompt             ()  { return this->variant.C_sp4co_sql_easy.opt_prompt_F; };
    pasbool            &opt_codeonly           ()  { return this->variant.C_sp4co_sql_easy.opt_codeonly_F; };
    pasbool            &opt_utility_session    ()  { return this->variant.C_sp4co_sql_easy.opt_utility_session_F; };
    tsp4_command_mode  &opt_comm_mode          ()  { return this->variant.C_sp4co_sql_easy.opt_comm_mode_F; };
    tsp00_CodeType     &opt_code               ()  { return this->variant.C_sp4co_sql_easy.opt_code_F; };
    tsp00_VFilename    &opt_aud_control        ()  { return this->variant.C_sp4co_sql_audit.opt_aud_control_F; };
    tsp00_VFilename    &opt_aud_logfile        ()  { return this->variant.C_sp4co_sql_audit.opt_aud_logfile_F; };
    tsp00_Int4         &opt_aud_log_blocksize  ()  { return this->variant.C_sp4co_sql_audit.opt_aud_log_blocksize_F; };
    tsp00_VFilename    &opt_aud_output         ()  { return this->variant.C_sp4co_sql_audit.opt_aud_output_F; };
    tsp4_command_mode  &opt_aud_comm_mode      ()  { return this->variant.C_sp4co_sql_audit.opt_aud_comm_mode_F; };
    tsp00_VFilename    &opt_ux_runfile         ()  { return this->variant.C_sp4co_sql_userx.opt_ux_runfile_F; };
    tsp4_command_mode  &opt_ux_comm_mode       ()  { return this->variant.C_sp4co_sql_userx.opt_ux_comm_mode_F; };
    /* const accessor functions */
    const tsp00_VFilename &opt_modulefn           () const  { return this->variant.C_sp4co_sql_precompiler.opt_modulefn_F; };
    const tsp00_VFilename &opt_tracefile          () const  { return this->variant.C_sp4co_sql_precompiler.opt_tracefile_F; };
    const tsp00_Int2   &opt_prognamel          () const  { return this->variant.C_sp4co_sql_precompiler.opt_prognamel_F; };
    const tsp00_KnlIdentifier &opt_progname           () const  { return this->variant.C_sp4co_sql_precompiler.opt_progname_F; };
    const tsp00_Int2   &opt_modulenamel        () const  { return this->variant.C_sp4co_sql_precompiler.opt_modulenamel_F; };
    const tsp00_KnlIdentifier &opt_modulename         () const  { return this->variant.C_sp4co_sql_precompiler.opt_modulename_F; };
    const tsp00_Int2   &opt_tpmonidl           () const  { return this->variant.C_sp4co_sql_precompiler.opt_tpmonidl_F; };
    const tsp00_Name   &opt_tpmonid            () const  { return this->variant.C_sp4co_sql_precompiler.opt_tpmonid_F; };
    const tsp00_Int2   &opt_begmar             () const  { return this->variant.C_sp4co_sql_precompiler.opt_begmar_F; };
    const tsp00_Int2   &opt_endmar             () const  { return this->variant.C_sp4co_sql_precompiler.opt_endmar_F; };
    const tsp00_Int2   &opt_trace              () const  { return this->variant.C_sp4co_sql_precompiler.opt_trace_F; };
    const tsp00_Int2   &opt_fnind              () const  { return this->variant.C_sp4co_sql_precompiler.opt_fnind_F; };
    const tsp00_Int2   &opt_datetime           () const  { return this->variant.C_sp4co_sql_precompiler.opt_datetime_F; };
    const tsp00_Int2   &opt_dyn                () const  { return this->variant.C_sp4co_sql_precompiler.opt_dyn_F; };
    const tsp00_Int2   &opt_tpmon              () const  { return this->variant.C_sp4co_sql_precompiler.opt_tpmon_F; };
    const tsp00_Int2   &opt_mode               () const  { return this->variant.C_sp4co_sql_precompiler.opt_mode_F; };
    const tsp00_Int2   &opt_extern             () const  { return this->variant.C_sp4co_sql_precompiler.opt_extern_F; };
    const tsp00_Int2   &opt_cansi              () const  { return this->variant.C_sp4co_sql_precompiler.opt_cansi_F; };
    const tsp00_Int2   &opt_comp               () const  { return this->variant.C_sp4co_sql_precompiler.opt_comp_F; };
    const tsp00_Int2   &opt_prof               () const  { return this->variant.C_sp4co_sql_precompiler.opt_prof_F; };
    const tsp00_Int4   &opt_packetsize         () const  { return this->variant.C_sp4co_sql_precompiler.opt_packetsize_F; };
    const pasbool      &opt_list               () const  { return this->variant.C_sp4co_sql_precompiler.opt_list_F; };
    const pasbool      &opt_precom             () const  { return this->variant.C_sp4co_sql_precompiler.opt_precom_F; };
    const pasbool      &opt_comment            () const  { return this->variant.C_sp4co_sql_precompiler.opt_comment_F; };
    const pasbool      &opt_traceswitch        () const  { return this->variant.C_sp4co_sql_precompiler.opt_traceswitch_F; };
    const pasbool      &opt_silent             () const  { return this->variant.C_sp4co_sql_precompiler.opt_silent_F; };
    const pasbool      &opt_nowarn             () const  { return this->variant.C_sp4co_sql_precompiler.opt_nowarn_F; };
    const pasbool      &opt_lineno             () const  { return this->variant.C_sp4co_sql_precompiler.opt_lineno_F; };
    const pasbool      &opt_lib                () const  { return this->variant.C_sp4co_sql_precompiler.opt_lib_F; };
    const pasbool      &opt_quote              () const  { return this->variant.C_sp4co_sql_precompiler.opt_quote_F; };
    const pasbool      &opt_decpoint           () const  { return this->variant.C_sp4co_sql_precompiler.opt_decpoint_F; };
    const pasbool      &opt_tabformat          () const  { return this->variant.C_sp4co_sql_precompiler.opt_tabformat_F; };
    const pasbool      &opt_version            () const  { return this->variant.C_sp4co_sql_precompiler.opt_version_F; };
    const pasbool      &opt_help               () const  { return this->variant.C_sp4co_sql_precompiler.opt_help_F; };
    const pasbool      &opt_unicode            () const  { return this->variant.C_sp4co_sql_precompiler.opt_unicode_F; };
    const tsp00_C1     &opt_quo                () const  { return this->variant.C_sp4co_sql_precompiler.opt_quo_F; };
    const tsp00_C1     &opt_point              () const  { return this->variant.C_sp4co_sql_precompiler.opt_point_F; };
    const tsp4_argline &opt_puser_args         () const  { return this->variant.C_sp4co_sql_precompiler.opt_puser_args_F; };
    const tsp00_VFilename &opt_rtracefile         () const  { return this->variant.C_sp4co_sql_pc_runtime.opt_rtracefile_F; };
    const tsp00_Int2   &opt_rtrace             () const  { return this->variant.C_sp4co_sql_pc_runtime.opt_rtrace_F; };
    const tsp00_Int2   &opt_rprof              () const  { return this->variant.C_sp4co_sql_pc_runtime.opt_rprof_F; };
    const tsp00_Int2   &opt_rmfetch            () const  { return this->variant.C_sp4co_sql_pc_runtime.opt_rmfetch_F; };
    const tsp00_Int2   &opt_rtime_limit        () const  { return this->variant.C_sp4co_sql_pc_runtime.opt_rtime_limit_F; };
    const tsp00_Int2   &opt_rstmt_cnt          () const  { return this->variant.C_sp4co_sql_pc_runtime.opt_rstmt_cnt_F; };
    const tsp00_Int2   &opt_rfiller            () const  { return this->variant.C_sp4co_sql_pc_runtime.opt_rfiller_F; };
    const pasbool      &opt_rnotracetime       () const  { return this->variant.C_sp4co_sql_pc_runtime.opt_rnotracetime_F; };
    const pasbool      &opt_rtraceswitch       () const  { return this->variant.C_sp4co_sql_pc_runtime.opt_rtraceswitch_F; };
    const pasbool      &opt_rnosdf             () const  { return this->variant.C_sp4co_sql_pc_runtime.opt_rnosdf_F; };
    const pasbool      &opt_rfiller1           () const  { return this->variant.C_sp4co_sql_pc_runtime.opt_rfiller1_F; };
    const tsp4_argline &opt_ruser_args         () const  { return this->variant.C_sp4co_sql_pc_runtime.opt_ruser_args_F; };
    const tsp00_VFilename &opt_runfile            () const  { return this->variant.C_sp4co_sql_easy.opt_runfile_F; };
    const tsp00_Name   &opt_object_owner       () const  { return this->variant.C_sp4co_sql_easy.opt_object_owner_F; };
    const tsp00_Name   &opt_object_name1       () const  { return this->variant.C_sp4co_sql_easy.opt_object_name1_F; };
    const tsp00_Name   &opt_object_name2       () const  { return this->variant.C_sp4co_sql_easy.opt_object_name2_F; };
    const tsp00_Line   &opt_parameter          () const  { return this->variant.C_sp4co_sql_easy.opt_parameter_F; };
    const pasbool      &opt_append             () const  { return this->variant.C_sp4co_sql_easy.opt_append_F; };
    const pasbool      &opt_prompt             () const  { return this->variant.C_sp4co_sql_easy.opt_prompt_F; };
    const pasbool      &opt_codeonly           () const  { return this->variant.C_sp4co_sql_easy.opt_codeonly_F; };
    const pasbool      &opt_utility_session    () const  { return this->variant.C_sp4co_sql_easy.opt_utility_session_F; };
    const tsp4_command_mode &opt_comm_mode          () const  { return this->variant.C_sp4co_sql_easy.opt_comm_mode_F; };
    const tsp00_CodeType &opt_code               () const  { return this->variant.C_sp4co_sql_easy.opt_code_F; };
    const tsp00_VFilename &opt_aud_control        () const  { return this->variant.C_sp4co_sql_audit.opt_aud_control_F; };
    const tsp00_VFilename &opt_aud_logfile        () const  { return this->variant.C_sp4co_sql_audit.opt_aud_logfile_F; };
    const tsp00_Int4   &opt_aud_log_blocksize  () const  { return this->variant.C_sp4co_sql_audit.opt_aud_log_blocksize_F; };
    const tsp00_VFilename &opt_aud_output         () const  { return this->variant.C_sp4co_sql_audit.opt_aud_output_F; };
    const tsp4_command_mode &opt_aud_comm_mode      () const  { return this->variant.C_sp4co_sql_audit.opt_aud_comm_mode_F; };
    const tsp00_VFilename &opt_ux_runfile         () const  { return this->variant.C_sp4co_sql_userx.opt_ux_runfile_F; };
    const tsp4_command_mode &opt_ux_comm_mode       () const  { return this->variant.C_sp4co_sql_userx.opt_ux_comm_mode_F; };
#endif    /* defined (__cplusplus) */
} tsp4_args_options;

typedef struct                     tsp4_xuser_record
{
    tsp4_xuserkey       xu_key;
    tsp00_Int2          xu_fill;
    tsp00_NodeId        xu_servernode;
    tsp00_DbName        xu_serverdb;
    tsp00_C18           xu_user_61;
    tsp00_CryptPw       xu_password;
    tsp4_sqlmode_name   xu_sqlmode;
    tsp00_Int4          xu_cachelimit;
    tsp00_Int2          xu_timeout;
    tsp00_Int2          xu_isolation;
    tsp00_C18           xu_dblang_61;
    tsp00_KnlIdentifier xu_user;
    tsp00_KnlIdentifier xu_dblang;
    tsp00_KnlIdentifier xu_userUCS2;
    tsp00_CryptPw       xu_passwordUCS2;
} tsp4_xuser_record;

struct tsp004_TemplateDummy {
    tsp4_xuser           dummy_tsp4_xuser;
    tsp4_info_result     dummy_tsp4_info_result;
    tsp4_command_mode    dummy_tsp4_command_mode;
    tsp4_component       dummy_tsp4_component;
    tsp4_xuserset        dummy_tsp4_xuserset;
};


#endif
