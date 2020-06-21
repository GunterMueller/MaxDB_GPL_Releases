#include <ptoc.h>

typedef
  short tsp00_Int2;
typedef
  int tsp00_Int4;
typedef
  unsigned char tsp_int1;
typedef
  tsp00_Int2 tsp_int2;
typedef
  tsp00_Int4 tsp_int4;
typedef
  float tsp_shortreal;
typedef
  double tsp_longreal;
typedef
  unsigned char *tsp_addr;
typedef
  unsigned char tsp00_ToolLayer;
typedef
  tsp00_ToolLayer tsp_layer;
typedef
  unsigned char tsp00_C1[1];
typedef
  unsigned char tsp00_C2[2];
typedef
  unsigned char tsp00_C4[4];
typedef
  unsigned char tsp00_C8[8];
typedef
  unsigned char tsp00_MoveObj[8192000];
typedef
  unsigned char tsp00_VFilename[64];
typedef
  unsigned char tsp_trace[16];
typedef
  tsp00_MoveObj *tsp_moveobj_ptr;
typedef
  tsp00_C1 tsp_c1;
typedef
  tsp00_C2 tsp_c2;
typedef
  unsigned char tsp_c3[3];
typedef
  tsp00_C4 tsp_c4;
typedef
  unsigned char tsp_c5[5];
typedef
  unsigned char tsp_c6[6];
typedef
  tsp00_C8 tsp_c8;
typedef
  unsigned char tsp_c12[12];
typedef
  unsigned char tsp_c16[16];
typedef
  unsigned char tsp_c18[18];
typedef
  unsigned char tsp_c30[30];
typedef
  unsigned char tsp_c50[50];
typedef
  unsigned char tsp_c70[70];
typedef
  unsigned char tsp_c132[132];
typedef
  unsigned char tsp_c256[256];
typedef
  unsigned char tsp_cryptpw[24];
typedef
  unsigned char tsp_code_type;
typedef
  unsigned char tsp_dbname[18];
typedef
  unsigned char tsp_decimal[20];
typedef
  unsigned char tsp_knl_identifier[64];
typedef
  unsigned char tsp_line[132];
typedef
  unsigned char tsp_lname[32];
typedef
  tsp00_MoveObj tsp_moveobj;
typedef
  unsigned char tsp_name[18];
typedef
  unsigned char tsp_nodeid[64];
typedef
  int tsp_process_id;
typedef
  unsigned char tsp_resnum[7];
typedef
  unsigned char tsp_sname[12];
typedef
  unsigned char tsp_sqlstate[5];
typedef
  tsp00_VFilename tsp_vfilename;
typedef
  unsigned char tsp_warningset[2];
typedef
  unsigned char tsp_zoned[20];
typedef
  unsigned char tsp00_LdInfo[1];
typedef
  unsigned char tsp00_LdState[1];
typedef
  struct ptoc_Type0
    {
    union  
      {
      struct  
        {
        char ld_just_for_compile_help;
        } C_true;
      struct  
        {
        tsp00_C8 ld_descriptor;
        tsp00_C8 ld_tabid;
        tsp00_Int4 ld_maxlen;
        tsp00_Int4 ld_intern_pos;
        tsp00_LdInfo ld_infoset;
        tsp00_LdState ld_state;
        tsp00_C1 ld_used_in_ak;
        unsigned char ld_valmode;
        tsp00_Int2 ld_valind;
        tsp00_Int2 ld_unused_2;
        tsp00_Int4 ld_valpos;
        tsp00_Int4 ld_vallen;
        } C_false;
      } boolean;
    } tsp_long_descriptor;
typedef
  unsigned char tsp00_LdbChange[1];
typedef
  struct ptoc_Type1
    {
    tsp00_C8 ldb_descriptor;
    tsp00_C2 ldb_serverdb_no;
    tsp00_LdbChange ldb_change;
    char ldb_replicated;
    tsp00_Int4 ldb_curr_pageno;
    tsp00_Int2 ldb_curr_pos;
    tsp00_C1 ldb_colno;
    tsp00_C1 ldb_show_kind;
    tsp00_Int4 ldb_intern_pos;
    unsigned char ldb_infoset[1];
    char ldb_filler1;
    tsp00_C1 ldb_filler1a;
    unsigned char ldb_valmode;
    tsp00_Int2 ldb_valind;
    tsp00_Int2 ldb_oldvallen;
    tsp00_Int4 ldb_valpos;
    tsp00_Int4 ldb_vallen;
    tsp00_Int4 ldb_full_len;
    tsp00_C4 ldb_filler2;
    tsp00_Int4 ldb_last_i_pos;
    tsp00_Int4 ldb_root_check;
    } tsp_long_desc_block;
typedef
  unsigned char tsp1_warningset_intern[2];
typedef
  unsigned char tsp1_command_options[1];
typedef
  unsigned char tsp1_part_attr[1];
typedef
  struct ptoc_Type2
    {
    unsigned char sp1p_part_kind;
    tsp1_part_attr sp1p_attributes;
    tsp_int2 sp1p_arg_count;
    tsp_int4 sp1p_segm_offset;
    tsp_int4 sp1p_buf_len;
    tsp_int4 sp1p_buf_size;
    } tsp1_part_header;
typedef
  struct ptoc_Type3
    {
    union  
      {
      struct  
        {
        tsp1_part_header sp1p_part_header;
        tsp_moveobj sp1p_buf;
        } C_1;
      struct  
        {
        unsigned char sp1p_part_kind;
        tsp1_part_attr sp1p_attributes;
        tsp_int2 sp1p_arg_count;
        tsp_int4 sp1p_segm_offset;
        tsp_int4 sp1p_buf_len;
        tsp_int4 sp1p_buf_size;
        tsp_resnum sp1p_resnum;
        } C_2;
      } integer;
    } tsp1_part;
typedef
  struct ptoc_Type4
    {
    union  
      {
      struct  
        {
        tsp_int4 sp1s_segm_len;
        tsp_int4 sp1s_segm_offset;
        tsp_int2 sp1s_no_of_parts;
        tsp_int2 sp1s_own_index;
        unsigned char sp1s_segm_kind;
        unsigned char sp1c_mess_type;
        unsigned char sp1c_sqlmode;
        unsigned char sp1c_producer;
        char sp1c_commit_immediately;
        char sp1c_ignore_costwarning;
        char sp1c_prepare;
        char sp1c_with_info;
        char sp1c_mass_cmd;
        char sp1c_parsing_again;
        tsp1_command_options sp1c_command_options;
        char sp1c_filler1;
        tsp_c8 sp1c_filler2;
        tsp_c8 sp1c_filler3;
        } C_1;
      struct  
        {
        tsp_c8 sp1s_space2;
        tsp_c4 sp1s_space3;
        unsigned char sp1s_space4;
        tsp_sqlstate sp1r_sqlstate;
        tsp_int2 sp1r_returncode;
        tsp_int4 sp1r_errorpos;
        tsp_warningset sp1r_extern_warning;
        tsp1_warningset_intern sp1r_intern_warning;
        tsp_int2 sp1r_function_code;
        tsp_int1 sp1r_tracelevel;
        char sp1r_filler1;
        tsp_c8 sp1r_filler2;
        } C_2;
      } integer;
    } tsp1_segment_header;
typedef
  struct ptoc_Type5
    {
    union  
      {
      struct  
        {
        tsp1_segment_header sp1s_segm_header;
        tsp1_part sp1p_part;
        } C_1;
      struct  
        {
        tsp1_segment_header sp1s_space1;
        tsp1_part_header sp1p_part_header;
        tsp_moveobj sp1p_buf;
        } C_2;
      struct  
        {
        tsp_int4 sp1s_segm_len;
        tsp_int4 sp1s_segm_offset;
        tsp_int2 sp1s_no_of_parts;
        tsp_int2 sp1s_own_index;
        unsigned char sp1s_segm_kind;
        unsigned char sp1c_mess_type;
        unsigned char sp1c_sqlmode;
        unsigned char sp1c_producer;
        char sp1c_commit_immediately;
        char sp1c_ignore_costwarning;
        char sp1c_prepare;
        char sp1c_with_info;
        char sp1c_mass_cmd;
        char sp1c_parsing_again;
        tsp1_command_options sp1c_command_options;
        char sp1c_filler1;
        tsp_c8 sp1c_filler2;
        tsp_c8 sp1c_filler3;
        } C_3;
      struct  
        {
        tsp_c8 sp1s_space2;
        tsp_c4 sp1s_space3;
        unsigned char sp1s_space4;
        tsp_sqlstate sp1r_sqlstate;
        tsp_int2 sp1r_returncode;
        tsp_int4 sp1r_errorpos;
        tsp_warningset sp1r_extern_warning;
        tsp1_warningset_intern sp1r_intern_warning;
        tsp_int2 sp1r_function_code;
        tsp_int1 sp1r_tracelevel;
        char sp1r_filler1;
        tsp_c8 sp1r_filler2;
        unsigned char sp1p_part_kind;
        tsp1_part_attr sp1p_attributes;
        tsp_int2 sp1p_arg_count;
        tsp_int4 sp1p_segm_offset;
        tsp_int4 sp1p_buf_len;
        tsp_int4 sp1p_buf_size;
        tsp_resnum sp1p_resnum;
        } C_4;
      } integer;
    } tsp1_segment;
typedef
  tsp1_segment *tsp1_segment_ptr;
typedef
  struct ptoc_Type6
    {
    tsp_code_type sp1h_mess_code;
    unsigned char sp1h_mess_swap;
    tsp_int2 sp1h_filler1;
    tsp_c5 sp1h_appl_version;
    tsp_c3 sp1h_application;
    tsp_int4 sp1h_varpart_size;
    tsp_int4 sp1h_varpart_len;
    tsp_int2 sp1h_filler2;
    tsp_int2 sp1h_no_of_segm;
    tsp_c8 sp1h_filler3;
    } tsp1_packet_header;
typedef
  struct ptoc_Type7
    {
    tsp1_packet_header sp1_header;
    union  
      {
      struct  
        {
        tsp_moveobj sp1_varpart;
        } C_1;
      struct  
        {
        tsp1_segment sp1_segm;
        } C_2;
      } integer;
    } tsp1_packet;
typedef
  tsp1_packet *tsp1_packet_ptr;
typedef
  tsp1_packet_ptr tsp1_packet_list[2];
typedef
  unsigned char tsp1_param_opt_set[1];
typedef
  tsp_c132 tsp4_argline;
typedef
  tsp_c18 tsp4_xuserkey;
typedef
  tsp_c8 tsp4_sqlmode_name;
typedef
  unsigned char tsp4_xuserset[2];
typedef
  struct ptoc_Type8
    {
    tsp_int2 opt_filler1;
    char opt_filler2;
    unsigned char opt_component;
    union  
      {
      struct  
        {
        tsp_vfilename opt_modulefn;
        tsp_vfilename opt_tracefile;
        tsp_int2 opt_prognamel;
        tsp_knl_identifier opt_progname;
        tsp_int2 opt_modulenamel;
        tsp_knl_identifier opt_modulename;
        tsp_int2 opt_tpmonidl;
        tsp_name opt_tpmonid;
        tsp_int2 opt_begmar;
        tsp_int2 opt_endmar;
        tsp_int2 opt_trace;
        tsp_int2 opt_fnind;
        tsp_int2 opt_datetime;
        tsp_int2 opt_dyn;
        tsp_int2 opt_tpmon;
        tsp_int2 opt_mode;
        tsp_int2 opt_extern;
        tsp_int2 opt_cansi;
        tsp_int2 opt_comp;
        tsp_int2 opt_prof;
        tsp_int4 opt_packetsize;
        char opt_list;
        char opt_precom;
        char opt_comment;
        char opt_traceswitch;
        char opt_silent;
        char opt_nowarn;
        char opt_lineno;
        char opt_lib;
        char opt_quote;
        char opt_decpoint;
        char opt_tabformat;
        char opt_version;
        char opt_help;
        char opt_unicode;
        tsp_c1 opt_quo;
        tsp_c1 opt_point;
        tsp4_argline opt_puser_args;
        } C_sp4co_sql_precompiler;
      struct  
        {
        tsp_vfilename opt_rtracefile;
        tsp_int2 opt_rtrace;
        tsp_int2 opt_rprof;
        tsp_int2 opt_rmfetch;
        tsp_int2 opt_rtime_limit;
        tsp_int2 opt_rstmt_cnt;
        tsp_int2 opt_rfiller;
        char opt_rnotracetime;
        char opt_rtraceswitch;
        char opt_rnosdf;
        char opt_rfiller1;
        tsp4_argline opt_ruser_args;
        } C_sp4co_sql_pc_runtime;
      struct  
        {
        tsp_vfilename opt_runfile;
        tsp_name opt_object_owner;
        tsp_name opt_object_name1;
        tsp_name opt_object_name2;
        tsp_line opt_parameter;
        char opt_append;
        char opt_prompt;
        char opt_codeonly;
        char opt_utility_session;
        unsigned char opt_comm_mode;
        tsp_code_type opt_code;
        } C_sp4co_sql_easy;
      struct  
        {
        tsp_vfilename opt_aud_control;
        tsp_vfilename opt_aud_logfile;
        tsp_int4 opt_aud_log_blocksize;
        tsp_vfilename opt_aud_output;
        unsigned char opt_aud_comm_mode;
        } C_sp4co_sql_audit;
      struct  
        {
        tsp_vfilename opt_ux_runfile;
        unsigned char opt_ux_comm_mode;
        } C_sp4co_sql_userx;
      } tsp4_component;
    } tsp4_args_options;
typedef
  struct ptoc_Type9
    {
    tsp4_xuserkey xu_key;
    tsp_int2 xu_fill;
    tsp_nodeid xu_servernode;
    tsp_dbname xu_serverdb;
    tsp_c18 xu_user_61;
    tsp_cryptpw xu_password;
    tsp4_sqlmode_name xu_sqlmode;
    tsp_int4 xu_cachelimit;
    tsp_int2 xu_timeout;
    tsp_int2 xu_isolation;
    tsp_c18 xu_dblang_61;
    tsp_knl_identifier xu_user;
    tsp_knl_identifier xu_dblang;
    tsp_knl_identifier xu_userUCS2;
    tsp_cryptpw xu_passwordUCS2;
    } tsp4_xuser_record;
typedef
  tsp_int4 tpr_intaddr;
typedef
  int tpr_cmd_state_Enum;
typedef
  unsigned char tpr_parsid[16];
typedef
  tpr_parsid tpr00_ParseId;
typedef
  tsp00_Int2 tpr00_sp1rFunctionCode;
typedef
  struct ptoc_Type10
    {
    tpr00_ParseId ParseId;
    tpr00_sp1rFunctionCode sp1rFunctionCode;
    } tpr00_ParseInfo;
typedef
  unsigned char tpr00_KnlSessionID[4];
typedef
  struct ptoc_Type11
    {
    tpr00_KnlSessionID KnlSession;
    tsp00_Int4 ClientSession;
    } tpr00_SessionID;
typedef
  unsigned char tpr_sqlline[52];
typedef
  unsigned char tpr_macroline[126];
typedef
  tsp_moveobj tpr_buffer;
typedef
  tsp_int2 tpr_symbol;
typedef
  struct ptoc_Type12
    {
    tsp_int4 syposacc;
    tsp_int4 sypos;
    tsp_int4 sylen;
    tpr_symbol symb;
    tsp_int2 syfill;
    } tpr_scannertype;
typedef
  struct ptoc_Type13
    {
    tsp_int4 descMaxelem;
    tsp_int4 descNelem;
    tsp_int4 descElemSize;
    tsp_int4 descIncr;
    tpr_intaddr descElemPtr;
    } tpr_sqlmdesc;
typedef
  struct ptoc_Type14
    {
    union  
      {
      struct  
        {
        tsp_int2 i2;
        } C_cpr_vint2;
      struct  
        {
        tsp_int4 i4;
        } C_cpr_vint4;
      struct  
        {
        tsp_shortreal r4;
        } C_cpr_vreal4;
      struct  
        {
        tsp_longreal r8;
        } C_cpr_vreal8;
      struct  
        {
        tsp_longreal r8s;
        } C_cpr_vreal8sun;
      struct  
        {
        tsp_decimal dec;
        } C_cpr_vdecimal;
      struct  
        {
        tsp_zoned zon;
        } C_cpr_vzoned;
      struct  
        {
        tpr_buffer ch;
        } C_cpr_vchar;
      struct  
        {
        tpr_buffer buf;
        } C_cpr_vbuf;
      struct  
        {
        tpr_intaddr iaddr;
        } C_cpr_viaddr;
      } tpr_vvvtype;
    } tpr_valuerec;
typedef
  tpr_valuerec *tpr_ptr_to_value;
typedef
  struct ptoc_Type15
    {
    union  
      {
      struct  
        {
        tpr_intaddr intaddr;
        } C_true;
      struct  
        {
        tpr_ptr_to_value vtypep;
        } C_false;
      } boolean;
    } tpr_vtypepointer;
typedef
  struct ptoc_Type16
    {
    tsp_int4 ireclen;
    tsp_int4 oreclen;
    } tpr_sqlrow;
typedef
  struct ptoc_Type17
    {
    unsigned char coltype;
    tsp1_param_opt_set colmode;
    unsigned char colio;
    tsp_int1 colfrac;
    tsp_int4 collen;
    tsp_int4 colpos;
    tsp_int2 coliolen;
    char colisnum;
    char colisuni;
    char colislong;
    tsp_int2 colvartype;
    tsp_int2 colindtype;
    tpr_intaddr coltobuf;
    tpr_intaddr colfrombuf;
    tpr_intaddr coltoindi;
    tpr_intaddr colfromindi;
    } tpr_sqlcol;
typedef
  struct ptoc_Type18
    {
    unsigned char sqldaid[8];
    tsp_int4 sqlmax;
    tsp_int2 sqln;
    tsp_int2 sqld;
    tsp_int4 sqlloop;
    tsp_int4 sqloffset;
    tsp_int2 sqlkano;
    tsp_int2 sqlprno;
    tsp_int2 sqlkamode;
    tsp_int2 sqlfiller;
    tpr_sqlrow sqlrow;
    struct ptoc_Type19
      {
      tsp_knl_identifier colname;
      tsp_int2 colio;
      tsp1_param_opt_set colmode;
      tsp_c3 colfill1;
      tsp_int2 coltype;
      tsp_int4 collength;
      tsp_int2 colfrac;
      tsp_int2 colfill2;
      tsp_int2 hostvartype;
      tsp_int2 hostcolsize;
      tsp_int4 hostindicator;
      tpr_vtypepointer hostvaraddr;
      tpr_vtypepointer hostindaddr;
      tpr_sqlcol sqlcoldb;
      } sqlvar[300];
    } sqldatype;
typedef
  sqldatype *sqldapointer;
typedef
  struct ptoc_Type20
    {
    unsigned char sqldaid[8];
    tsp_int4 sqldabc;
    tsp_int2 sqln;
    tsp_int2 sqld;
    tsp_int4 sqlloop;
    tsp_int4 sqloffset;
    tsp_int2 sqlkano;
    tsp_int2 sqlprno;
    tsp_int2 sqlkamode;
    tsp_int2 sqlfiller;
    tpr_sqlrow sqlrow;
    struct ptoc_Type21
      {
      tsp_int2 sqltype;
      tsp_int2 sqllen;
      tpr_vtypepointer sqldata;
      tpr_vtypepointer sqlind;
      tsp_int2 sqlnamel;
      tsp_c30 sqlname;
      tpr_sqlcol sqlcoldb;
      } sqlvardb2[300];
    } sqldb2type;
typedef
  sqldb2type *sqldb2pointer;
typedef
  short tpr_arr_int2[300];
typedef
  int tpr_arr_int4[300];
typedef
  tpr_vtypepointer tpr_arr_ptrv[300];
typedef
  tpr_sqlcol tpr_arr_dbcolinfo[1];
typedef
  tpr_arr_ptrv *tpr_arr_ptrvp;
typedef
  tpr_arr_int2 *tpr_arr_int2p;
typedef
  tpr_arr_int4 *tpr_arr_int4p;
typedef
  tpr_arr_dbcolinfo *tpr_arr_sqlcolp;
typedef
  struct ptoc_Type22
    {
    union  
      {
      struct  
        {
        tpr_intaddr intaddr;
        } C_true;
      struct  
        {
        tpr_arr_sqlcolp sqlcolp;
        } C_false;
      } boolean;
    } tpr_sqlcolpointer;
typedef
  struct ptoc_Type23
    {
    unsigned char sqldaid[8];
    tsp_int4 sqldabc;
    tsp_int4 sqlfill;
    tsp_int4 sqlloop;
    tsp_int4 sqloffset;
    tsp_int2 sqlkano;
    tsp_int2 sqlprno;
    tsp_int2 sqlkamode;
    tsp_int2 sqlfiller;
    tpr_sqlrow sqlrow;
    tpr_sqlcolpointer sqlcoldb;
    tsp_int4 ora_n;
    tpr_arr_ptrvp ora_v;
    tpr_arr_int4p ora_l;
    tpr_arr_int2p ora_t;
    tpr_arr_ptrvp ora_i;
    tsp_int4 ora_f;
    tpr_arr_ptrvp ora_s;
    tpr_arr_int2p ora_m;
    tpr_arr_int2p ora_c;
    tpr_arr_ptrvp ora_x;
    tpr_arr_int2p ora_y;
    tpr_arr_int2p ora_z;
    tpr_arr_ptrvp ora_fm;
    tpr_arr_int2p ora_fml;
    tpr_arr_int2p ora_fclp;
    tpr_arr_int2p ora_fcrc;
    } sqloratype;
typedef
  sqloratype *sqlorapointer;
typedef
  struct ptoc_Type24
    {
    union  
      {
      struct  
        {
        sqldapointer sqldaptr;
        } C_cpr_kind_internal;
      struct  
        {
        sqldb2pointer db2daptr;
        } C_cpr_kind_db2;
      struct  
        {
        sqlorapointer oradaptr;
        } C_cpr_kind_oracle;
      struct  
        {
        tpr_intaddr intaddr;
        } C_cpr_kind_empty;
      } tpr_kind;
    } sqldaptrvrec;
typedef
  struct ptoc_Type25
    {
    tsp00_VFilename tatracefn;
    tsp_int4 tatracepprt;
    tsp_int4 tatraceno;
    tsp_int2 tatracety;
    tsp_int2 tamaxsecoutp;
    tsp00_VFilename tamaxsecfn;
    tsp_int4 tamaxsecno;
    tsp_int4 tamaxsecpptr;
    tsp_c8 tastartdate;
    tsp_c8 taenddate;
    tsp_c8 tastarttime;
    tsp_c8 taendtime;
    tsp_int2 tacnttrstm;
    tsp_int2 tatrout;
    tsp_int2 tafiller;
    tsp_int2 tastr80l;
    tsp_c256 tastr80;
    } sqltatype;
typedef
  sqltatype *sqltapointer;
typedef
  struct ptoc_Type26
    {
    tsp00_VFilename oamodtracefn;
    tsp_int2 oamodtrace;
    tsp_int2 oamodtracety;
    tsp_int2 oamodsettrty;
    tsp_int2 oamaxtrstm;
    tsp_int2 oanosdf;
    tsp_int2 oamaxsec;
    tsp_int2 oapidflag;
    tsp_int4 oapid;
    } sqloatype;
typedef
  sqloatype *sqloapointer;
typedef
  struct ptoc_Type27
    {
    union  
      {
      struct  
        {
        tpr_intaddr intaddr;
        } C_true;
      struct  
        {
        tsp1_packet_ptr vtypep;
        } C_false;
      } boolean;
    } tpr_ptr_packet;
typedef
  struct ptoc_Type28
    {
    union  
      {
      struct  
        {
        tpr_intaddr intaddr;
        } C_true;
      struct  
        {
        tsp_moveobj_ptr vtypep;
        } C_false;
      } boolean;
    } tpr_ptr_moveobj;
typedef
  struct ptoc_Type29
    {
    tsp_int2 rbcolno;
    tsp_int2 rbfiller1;
    tsp_int4 rbfiller2;
    tsp_int4 rbrestlen;
    tsp_int4 rbhostlensum;
    tsp_int4 rblongcolmax;
    tsp_int4 rbbufsize;
    tpr_ptr_moveobj rbbufpart;
    } tpr_restbuflong;
typedef
  struct ptoc_Type30
    {
    union  
      {
      struct  
        {
        tpr_intaddr intaddr;
        } C_true;
      struct  
        {
        tpr_restbuflong *vtypep;
        } C_false;
      } boolean;
    } tpr_ptr_restbuflong;
typedef
  struct ptoc_Type31
    {
    union  
      {
      struct  
        {
        tpr_sqlcol *addr;
        } C_true;
      struct  
        {
        tpr_intaddr intaddr;
        } C_false;
      } boolean;
    } tpr_colpointer;
typedef
  struct ptoc_Type32
    {
    union  
      {
      struct  
        {
        tsp_int2 ldkind_o;
        tsp_int2 ldlength_o;
        tsp_long_descriptor descold;
        } C_false;
      struct  
        {
        tsp_int2 ldkind_n;
        tsp_int2 ldlength_n;
        tsp_long_desc_block descnew;
        } C_true;
      } boolean;
    } tpr_sqlspldarr[2000];
typedef
  struct ptoc_Type33
    {
    tpr_vtypepointer ldhostaddr;
    tpr_vtypepointer ldindkaddr;
    tpr_colpointer ldcoladdr;
    tsp_int4 ldhostlen;
    tsp_int2 ldhosttype;
    tsp_int2 ldindktype;
    tsp_int2 ldindklen;
    tsp_int2 ldindkfrac;
    tsp_int2 ldrowno;
    tsp_int2 ldcolno;
    tsp_c1 ldsfiotype;
    unsigned char ldsfdtype;
    tsp_int2 ldfill;
    tsp_int4 ldfileno;
    tsp_int4 ldsfbufpos;
    tsp_int4 ldhostlensum;
    tpr_ptr_restbuflong ldrestdata;
    } tpr_sqlpcldarr[2000];
typedef
  tpr_sqlspldarr *tpr_sqlspldptr;
typedef
  tpr_sqlpcldarr *tpr_sqlpcldptr;
typedef
  struct sqlorentry *sqlorentry_ptr;
typedef
  struct ptoc_Type34
    {
    tsp_int2 ldmaxi;
    tsp_int2 ldmaxo;
    tsp_int2 ldindpc;
    tsp_int2 ldindsp;
    tsp_int2 ldcolkind;
    tsp_int2 ldcount;
    char ldputvalsend;
    char ldallsend;
    tsp_int2 ldretmaxi;
    tsp_int2 ldretmaxo;
    tsp_int2 ldretindpc;
    tsp_int2 ldretindsp;
    tpr_ptr_restbuflong ldrestbufp;
    tsp_int4 lderrd3;
    tpr_sqlpcldptr ldpc;
    tpr_sqlspldptr ldsp;
    tpr_sqlmdesc ldpc_mdesc;
    tpr_sqlmdesc ldsp_mdesc;
    tpr_intaddr ldpcarr;
    tpr_intaddr ldsparr;
    tpr_sqlmdesc ldpcarr_mdesc;
    tpr_sqlmdesc ldsparr_mdesc;
    sqlorentry_ptr ldorelastcmd;
    } tpr_sqlld;
typedef
  struct ptoc_Type35
    {
    union  
      {
      struct  
        {
        tpr_intaddr intaddr;
        } C_true;
      struct  
        {
        tpr_sqlld *vtypep;
        } C_false;
      } boolean;
    } tpr_ptr_sqlld;
typedef
  tsp1_part *tpr_part_kind_ptr_record[28];
typedef
  struct ptoc_Type36
    {
    tsp_process_id gareference;
    tsp_c2 gamesscode;
    tsp_int2 gastatus;
    tsp_int2 gavaindex;
    tsp_int2 ganodeindex;
    tsp_int2 gareconnect;
    tsp_int2 ganolog;
    tsp_int2 gaprofinit;
    tsp_int2 gadbcodeset;
    tsp_knl_identifier gaatdbname;
    tsp_int2 gafiller2;
    tpr_intaddr gapatchptr;
    tsp1_packet_ptr gareqptr;
    tsp1_packet_ptr garecptr;
    tsp1_packet_list gapacketlist;
    tsp4_xuser_record gaxuserrec;
    tsp00_Int4 gaKernelVersion;
    tpr_intaddr gaServerNode;
    char gaIsCanceled;
    tpr00_SessionID gaKnlSessionID;
    } sqlgaentry;
typedef
  struct ptoc_Type37
    {
    tsp4_xuser_record gaopxuserrec;
    tsp_int2 gauseropset;
    tsp_int2 gamodisolation;
    tsp_int2 gasqlconntyp;
    tsp_int2 gaencodingcnt;
    tpr_intaddr gaencodingptr;
    tsp_int2 gaasciicodeset;
    tsp_int2 gaunicodeset;
    tsp_int2 gacmpkind;
    tpr_cmd_state_Enum gaCmdState;
    tsp_addr gaConnection;
    sqlgaentry *gaentry[8];
    } sqlgatype;
typedef
  sqlgatype *sqlgapointer;
typedef
  struct ptoc_Type38
    {
    tsp_int2 ereturncode;
    tsp_int2 etextlen;
    tsp_int4 eerrorpos;
    tsp_c16 ewarning;
    tsp_c6 esqlstate;
    unsigned char elzu;
    unsigned char eprerr;
    tsp_c70 etext;
    } tpr_sqlerror;
typedef
  tpr_sqlerror *sqlempointer;
typedef
  struct ptoc_Type39
    {
    tsp_c8 ratermid;
    tsp_process_id raprocessid;
    tsp_c3 racomponent;
    unsigned char rasqlos;
    tsp_int2 ralang;
    tsp_int2 rasqlinit;
    tsp_int2 raactsession;
    tsp_int2 radbmode;
    tsp_int2 rasqlansi;
    tsp_int2 rakamode;
    tsp_int2 ramodeswitch;
    tsp_int2 ramesstype;
    tsp_int4 ranumrows;
    tsp_int2 ranotrtimeoutp;
    tsp_int2 raopprof;
    tsp_int4 raprofsec;
    char raparse_again;
    tsp_c3 rafiller;
    tsp_int4 ra2filler;
    tsp1_segment_ptr rasegptr;
    tpr_part_kind_ptr_record rasegmpartptr;
    tpr_intaddr raprofstmtp;
    tpr_ptr_sqlld rasqlldp;
    sqltapointer rasqltap;
    } sqlratype;
typedef
  sqlratype *sqlrapointer;
typedef
  struct ptoc_Type40
    {
    tsp_knl_identifier snname;
    tsp_int2 sntype;
    tsp_int2 snindex;
    tsp_int2 sncount;
    tsp_int2 snfiller1;
    tpr_vtypepointer snstmtp;
    tsp_int4 snbuflen;
    tsp_int4 snstmtlen;
    tsp_int2 snsession;
    tsp_int2 snfiller2;
    tpr00_ParseInfo snParseInfo;
    } tpr_sqlsnentries[100];
typedef
  struct ptoc_Type41
    {
    union  
      {
      struct  
        {
        tpr_intaddr intaddr;
        } C_true;
      struct  
        {
        tpr_sqlsnentries *vtypep;
        } C_false;
      } boolean;
    } tpr_ptr_sqlsne;
typedef
  struct ptoc_Type42
    {
    tsp_int4 snentcnt;
    tsp_int4 snentact;
    tpr_ptr_sqlsne snentptr;
    } tpr_sqlsn;
typedef
  struct ptoc_Type43
    {
    tsp_int2 malen;
    tpr_macroline maval;
    } sqlmaentries[128];
typedef
  struct ptoc_Type44
    {
    tsp_int4 maversion;
    sqlmaentries maentry;
    } sqlmatype;
typedef
  sqlmatype *sqlmapointer;
typedef
  tsp_addr tpr05_StringEncoding;
typedef
  struct ptoc_Type45
    {
    tsp_int2 mfInit;
    tsp_int2 mfSelectno;
    tsp_int2 mfBufinit;
    tsp_int2 mfReclen;
    tsp_int4 mfBuflen;
    tsp_int4 mfRecpos;
    tsp_int2 mfPrevfaspec;
    tsp_int2 mfLastfaspec;
    tsp_int2 mfRownotfound;
    tsp_int2 mfFetchget;
    tsp_int4 mfRecposbeg;
    tsp_int4 mfMaxselrec;
    tsp_int4 mfLastrecno;
    tsp_int4 mfNextLoopCnt;
    tsp_int4 mfAllRecordsRead;
    tsp_int4 mfActualPos;
    tsp_int4 mfBufbegno;
    tsp_int4 mfBufendno;
    tsp_int4 mfLongcolpos;
    tpr_ptr_packet mfBufpointer;
    tpr_ptr_moveobj mfrecpointer;
    tpr_part_kind_ptr_record mfsegmpartptr;
    tsp_int4 mfrecptrlen;
    tsp_int2 mforindex;
    tsp_int2 mffetchkind;
    tsp_int4 mfsumanzrec;
    tsp_int2 mfopendata;
    tsp_int2 mfPrecomindex;
    tpr_parsid mfSelparsid;
    tsp_knl_identifier mfresultname;
    tpr05_StringEncoding PacketEncoding;
    } sqlmfentarr[32767];
typedef
  sqlmfentarr *sqlmfentptr;
typedef
  struct ptoc_Type46
    {
    tsp_int2 mfselcnt;
    tsp_int2 mfmfetch;
    tsp_int4 mffiller;
    tpr_sqlmdesc mfdesc;
    sqlmfentptr mfentry;
    } sqlmftype;
typedef
  sqlmftype *sqlmfpointer;
typedef
  struct ptoc_Type47
    {
    union  
      {
      struct  
        {
        tsp_int2 va1indi_sc;
        tsp_int2 va1indva2_sc;
        tsp_int2 va1indva3_sc;
        tsp_int2 va1fill;
        tpr_vtypepointer va1addr_sc;
        tpr_intaddr va1fill1;
        } C_sqlvasc;
      struct  
        {
        tsp_int2 va1indi_cm;
        tsp_int2 va1indva2_cm;
        tsp_int2 va1indva3_cm;
        tsp_int2 va1fill_cm;
        tsp_int4 va1offs_cm;
        tsp_int4 va1fill2;
        } C_sqlvacm;
      struct  
        {
        tsp_int2 va1indi_st;
        tsp_int2 va1cmpcnt_st;
        tsp_int4 va1size_st;
        tpr_vtypepointer va1addr_st;
        tsp_int4 va1ix_st;
        } C_sqlvast;
      struct  
        {
        tsp_int2 va1indi_pt;
        tsp_int2 va1fill3;
        tsp_int4 va1ix_pt;
        tpr_vtypepointer va1addr_pt;
        tpr_intaddr va1fill5;
        } C_sqlvapt;
      } tsp_int2;
    } sqlva1type[10000];
typedef
  sqlva1type *sqlva1p;
typedef
  struct ptoc_Type48
    {
    tsp_int2 va2typ;
    tsp_int2 va2digit;
    tsp_int4 va2size;
    tsp_int2 va2frac;
    tsp_int2 va2const;
    } sqlva2type[5000];
typedef
  sqlva2type *sqlva2p;
typedef
  struct ptoc_Type49
    {
    tsp_lname va3name;
    tsp_int2 va3naml;
    tsp_int2 va3fill;
    } sqlva3type[10000];
typedef
  sqlva3type *sqlva3p;
typedef
  struct ptoc_Type50
    {
    union  
      {
      struct  
        {
        tsp_int2 pakindlo;
        tsp_int2 pava1index;
        tsp_int4 paloopcnt;
        tsp_int4 paloopmin;
        } C_sqlparlo;
      struct  
        {
        tsp_int2 pakindst;
        tsp_int2 pavarst;
        tsp_int2 paindst;
        tsp_int2 paelcnt;
        tpr_intaddr pafill4;
        } C_sqlparst;
      struct  
        {
        tsp_int2 pakindel;
        tsp_int2 pavarno;
        tsp_int2 paindno;
        } C_sqlparel;
      } tsp_int2;
    } sqlpartype[5000];
typedef
  sqlpartype *sqlparpointer;
typedef
  struct ptoc_Type51
    {
    tsp_int2 faselectno;
    tsp_int2 famfindex;
    tsp_int4 fapos;
    tsp_int2 faspec;
    tsp_int2 fafetch;
    tsp_int4 fareclen;
    } sqlfaentry;
typedef
  sqlfaentry sqlfatype[200];
typedef
  sqlfatype *sqlfapointer;
typedef
  struct ptoc_Type52
    {
    tsp_int2 katyp;
    tsp_int2 kastate;
    tsp_int2 kapacount;
    tsp_int2 kapaindex;
    tsp_int2 kaStcount;
    tsp_int2 kaStindex;
    tsp_int2 kamacro;
    tsp_int2 kaprindex;
    tsp_int2 kadiindex;
    tsp_int2 kaparserrc;
    tsp_int4 kaversion;
    tpr00_ParseInfo kaParseInfo;
    tsp_int2 kaopendata;
    tsp_int2 kaparamcnt;
    tsp_int2 kalineno;
    tsp_int2 kafaindex;
    tsp_int2 kaatindex;
    tsp_int2 kacuindex;
    tsp_int2 kamode;
    tpr_sqlrow kasqlrow;
    sqlfaentry *kafae;
    } sqlkaentry;
typedef
  sqlkaentry sqlkatype[3000];
typedef
  sqlkatype *sqlkapointer;
struct sqlorentry
  {
  tsp_int4 orcolcnt;
  tsp_int4 orcollen;
  tpr_sqlrow orsqlrow;
  tpr_sqlcolpointer orcolptr;
  tsp_int4 orcolcntacc;
  tsp_int2 oropennew;
  tsp_int4 orrescnt;
  tsp_int4 orreslen;
  tpr_sqlrow orressqlrow;
  tpr_sqlcolpointer orresptr;
  tsp_int4 orrescntacc;
  tsp_int4 orresnamlen;
  tpr_vtypepointer orresnamptr;
  tsp_int4 orresnamacc;
  tpr_vtypepointer orresnamencoding;
  } ;
typedef
  struct sqlorentry sqlortype[3000];
typedef
  sqlortype *sqlorpointer;
typedef
  struct ptoc_Type53
    {
    tsp_int2 prStmtNameIndex;
    tsp_int2 prCursorNameIndex;
    tsp_int2 prstate;
    tsp_knl_identifier prStmtName;
    tsp_int2 prkaindex;
    tsp_int2 prarea;
    tsp00_Int2 prDescribe;
    tsp_knl_identifier prCursorName;
    tsp_int2 prcuindex;
    tsp_int2 prstcount;
    tsp_int2 prstindex;
    } sqlprentry;
typedef
  sqlprentry sqlprtype[500];
typedef
  sqlprtype *sqlprpointer;
typedef
  struct ptoc_Type54
    {
    tsp_int4 stkano;
    tsp_int4 stllen;
    tpr_sqlline stline;
    } sqlsttype[1000];
typedef
  sqlsttype *sqlstpointer;
typedef
  struct ptoc_Type55
    {
    tsp_knl_identifier atdbname;
    tsp_int2 atdbindex;
    tsp_nodeid atdbstring;
    tsp_int2 atgaindex;
    tsp_int2 atdbnamix;
    } sqlattype[500];
typedef
  sqlattype *sqlatpointer;
typedef
  struct ptoc_Type56
    {
    tsp_knl_identifier cuname;
    tsp_int2 cufill;
    tsp_int2 cuatindex;
    tsp_int4 cuerrd3;
    tsp_int2 custate;
    tsp_int2 cukind;
    tsp_int2 cukaopenind;
    tsp_int4 culoopin;
    tsp_int4 culoopout;
    sqldaptrvrec cusqlindap;
    sqldaptrvrec cusqloutdap;
    } sqlcuentry;
typedef
  sqlcuentry sqlcutype[200];
typedef
  sqlcutype *sqlcupointer;
typedef
  struct ptoc_Type57
    {
    tsp_int2 fnkanomin;
    tsp_int2 fnkanomax;
    tsp_knl_identifier fnfilen;
    tsp_int2 fnincllno;
    } sqlfntype[50];
typedef
  sqlfntype *sqlfnpointer;
typedef
  tsp00_Int4 tpr01_DescTypeEnum;
typedef
  tsp00_Int4 tpr01_SQLStateBinary;
typedef
  tsp00_Int2 tpr01_SQLAttribute;
typedef
  tsp00_Int4 tpr00_ComTypeEnum;
typedef
  struct ptoc_Type58
    {
    tpr01_DescTypeEnum DescType;
    tsp_addr SQL;
    tsp_addr StmtNameDesc;
    tsp_addr CursorDesc;
    tsp_addr ErrorDesc;
    tsp_addr ConDesc;
    tpr00_ComTypeEnum ComType;
    unsigned char MessType;
    tpr05_StringEncoding PacketEncoding;
    sqlprentry *pr;
    sqlkaentry *ka;
    struct sqlorentry *ore;
    sqlcuentry *cu;
    tsp_addr sqlxap;
    tpr01_SQLStateBinary State;
    tpr01_SQLAttribute Attribute;
    } tpr01_SQLDesc;
typedef
  struct ptoc_Type59
    {
    tsp_int2 xainit;
    tsp_int2 xacmdinit;
    tsp_int2 xakano;
    tsp_knl_identifier xaprogn;
    tsp_int2 xaprogc;
    tsp_knl_identifier xamodn;
    tsp_int2 xamodc;
    tsp_int2 xalang;
    tsp_addr sqlModDesc;
    tpr01_SQLDesc *xaSQLDesc;
    tsp_int4 xacfetposc;
    tsp_int2 xaprno;
    tsp_sname xalcheck;
    tsp_knl_identifier xaprname;
    tsp_int2 xafiller1;
    sqldaptrvrec xasqldap;
    tsp_int2 xacfetspec;
    tsp_c6 xacfiller;
    } sqlcxatype;
typedef
  sqlcxatype *sqlcxapointer;
typedef
  unsigned char sqlstatetype[6];
typedef
  struct ptoc_Type60
    {
    tsp_int4 sqlenv;
    unsigned char sqlcaid[8];
    tsp_int4 sqlcabc;
    tsp_int4 sqlcode;
    tsp_int2 sqlerrml;
    tsp_c70 sqlerrmc;
    tsp_c8 sqlerrp;
    int sqlerrd[6];
    unsigned char sqlwarn0;
    unsigned char sqlwarn1;
    unsigned char sqlwarn2;
    unsigned char sqlwarn3;
    unsigned char sqlwarn4;
    unsigned char sqlwarn5;
    unsigned char sqlwarn6;
    unsigned char sqlwarn7;
    unsigned char sqlwarn8;
    unsigned char sqlwarn9;
    unsigned char sqlwarna;
    unsigned char sqlwarnb;
    unsigned char sqlwarnc;
    unsigned char sqlwarnd;
    unsigned char sqlwarne;
    unsigned char sqlwarnf;
    unsigned char sqlext[12];
    tsp_knl_identifier sqlresn;
    tsp_int2 sqlfill1;
    tsp_int2 sqlmfetch;
    tsp_int4 sqltermref;
    tsp_int2 sqlfill5;
    tsp_int2 sqldbmode;
    tsp_int2 sqldatetime;
    sqlstatetype sqlstate;
    tsp4_argline sqlargl;
    sqlgapointer sqlgap;
    sqlrapointer sqlrap;
    sqloapointer sqloap;
    sqlmapointer sqlmap;
    sqlmfpointer sqlmfp;
    tpr_intaddr sqlplp;
    sqlempointer sqlemp;
    tpr_sqlmdesc sqlxadesc;
    sqlcxapointer sqlcxap;
    tsp_addr sqlEnvDesc;
    } sqlcatype;
typedef
  struct ptoc_Type61
    {
    tsp_int2 xainit;
    tsp_int2 xacmdinit;
    tsp_int2 xakano;
    tsp_knl_identifier xaprogn;
    tsp_int2 xaprogc;
    tsp_knl_identifier xamodn;
    tsp_int2 xamodc;
    tsp_int2 xalang;
    tsp_addr xaModDesc;
    tpr01_SQLDesc *xaSQLDesc;
    tsp_int2 xaprno;
    tsp_sname xalcheck;
    tsp_knl_identifier xaprname;
    tsp_int2 xastopcnt;
    tsp_int2 xallen;
    tpr_sqlline xaline;
    tsp_int2 xafiller2;
    tsp_int4 xatime;
    tsp_int2 xadescpos;
    tsp_int2 xadesclen;
    tsp_int2 xaatcount;
    tsp_int2 xaatmax;
    tsp_int4 xafiller3;
    sqlparpointer sqlpap;
    sqlparpointer sqlpae;
    sqlkapointer sqlkap;
    sqlkapointer sqlkae;
    sqlprpointer sqlprp;
    sqlprpointer sqlpre;
    sqlstpointer sqlstp;
    sqlstpointer sqlste;
    sqlfapointer sqlfap;
    sqlfapointer sqlfae;
    sqlatpointer sqlatp;
    sqlatpointer sqlate;
    sqlva1p sqlv1p;
    sqlva1p sqlv1e;
    sqlva2p sqlv2p;
    sqlva2p sqlv2e;
    sqlva3p sqlv3p;
    sqlva3p sqlv3e;
    tpr_intaddr sqlfill1;
    tpr_intaddr sqlfill2;
    tpr_intaddr sqlfill3;
    tpr_intaddr sqlfill4;
    sqlcupointer sqlcup;
    sqlcupointer sqlcue;
    sqlorpointer sqlorp;
    sqlorpointer sqlore;
    sqlfnpointer sqlfnp;
    sqlfnpointer sqlfne;
    tpr_sqlsn sqlsnp;
    tpr_sqlmdesc sqlsndesc;
    } sqlxatype;
typedef
  unsigned char tpc_prelinetype[512];
typedef
  unsigned char tpc_pre_errors;
typedef
  tsp_int2 tpc_cob_section;
typedef
  struct ptoc_Type62
    {
    tsp_int4 vffileno;
    tsp00_VFilename vffilen;
    unsigned char vffilemode;
    char vfbinary;
    char vffeof;
    tsp_int2 vfreclen2;
    tsp_int2 vfcnt;
    unsigned char vfform;
    tsp_int2 vfbufcount;
    tsp_int2 vffiller;
    tpr_intaddr vfblock;
    tsp_int4 vfreclen4;
    } tpc_vfentries[8];
typedef
  struct ptoc_Type63
    {
    tsp_int2 vfacc;
    tsp_int4 vfpoolprt;
    tpc_vfentries vffn;
    } tpc_vffiletype;
typedef
  struct ptoc_Type64
    {
    tsp_int2 llen;
    tsp_int2 lbegpos;
    tsp_int2 lendpos;
    tsp_int2 laccpos;
    tpc_prelinetype lline;
    } tpc_tprelinerec;
typedef
  struct ptoc_Type65
    {
    tsp_int2 ancomtyp;
    tsp_int2 ansection;
    char anbegin;
    char ansuccstatem;
    tsp_int2 ancheck;
    char anusage[8];
    } tpc_analysetype;
typedef
  struct ptoc_Type66
    {
    tsp_int2 whlen;
    tsp_int2 whact;
    tsp_c50 whstr;
    tsp_int2 whfiller;
    } tpc_whenevertype[6];
typedef
  struct ptoc_Type67
    {
    tsp_int4 ecnt;
    struct ptoc_Type68
      {
      tsp_sname ename;
      tsp_int2 enaml;
      } eend[5];
    } tpc_endstype;
typedef
  struct ptoc_Type69
    {
    tsp_lname cname;
    tsp_int2 cwert;
    } tpc_csectionp[100];
typedef
  struct ptoc_Type70
    {
    tsp_int2 ccnt;
    tsp_int2 cbot;
    tpc_csectionp *csec;
    } tpc_const_sec;
typedef
  struct ptoc_Type71
    {
    tsp_int2 lnoinp;
    tsp_int2 lnoout;
    tsp_int2 lnoind;
    tsp_int2 lnolast;
    tsp_int2 lnosrc;
    tsp_int2 lnoexp;
    tsp_c12 lnochr;
    } tpc_lnorecord;
typedef
  struct ptoc_Type72
    {
    tsp_lname vaname;
    tsp_int2 vanaml;
    tsp_int2 vacnt;
    tsp_int2 vastrix;
    tsp_int2 vatypix;
    tsp_int2 vasqlix;
    tsp_int2 vaglobl;
    tsp_int2 vablockid;
    tsp_int2 vafiller;
    } tpc_vartable[1000];
typedef
  struct ptoc_Type73
    {
    tsp_int2 tyref;
    tsp_int2 typtr;
    tsp_int2 tydigit;
    tsp_int2 tyindi;
    union  
      {
      struct  
        {
        tsp_int2 tycmcnt;
        tsp_int2 tycmpix;
        tsp_int2 tyvacnt;
        tsp_int2 tyfill1;
        } C_cpr_vstruct;
      struct  
        {
        tsp_int4 tydim;
        tsp_int2 tyfix;
        tsp_int2 tytypix;
        } C_cpr_varray;
      struct  
        {
        tsp_int4 tysize;
        tsp_int2 tyfrac;
        tsp_int2 tyfill3;
        } C_cpr_vint2;
      } tsp_int2;
    } tpc_typtable[1000];
typedef
  short tpc_cmpindex[1000];
typedef
  tpc_vartable *tpc_vartablep;
typedef
  tpc_typtable *tpc_typtablep;
typedef
  tpc_cmpindex *tpc_cmpindexp;
typedef
  struct ptoc_Type74
    {
    tsp_int2 varcnt;
    tsp_int2 varsav;
    tsp_int2 varbot;
    tsp_int2 typcnt;
    tsp_int2 cmpcnt;
    tsp_int2 pcfiller;
    tpc_vartablep vartablep;
    tpc_typtablep typtablep;
    tpc_cmpindexp cmpindexp;
    } tpc_symtab;
typedef
  struct ptoc_Type75
    {
    tsp_int2 ndvarmainix;
    tsp_int2 ndvararrix;
    tsp_int2 ndvarentix;
    tsp_int2 ndtypentix;
    tsp_int2 ndmaintyindi;
    tsp_int2 ndcompexpan;
    tsp_int2 ndarraycnt;
    short ndarrayix[4];
    tsp_int4 ndsqlva1st;
    tsp_int4 ndsqlva1ix;
    tsp_int4 ndsqlva1cnt;
    tsp_int2 ndBlockId;
    tsp_int2 ndkano;
    tsp_int2 ndLoopPar;
    tsp_int2 ndExpanlen;
    tsp_int2 ndCompIx;
    tsp_int2 ndNamelen;
    tpr_intaddr ndNamePtr;
    } tpc_arrndtype[100];
typedef
  tpc_arrndtype *tpc_nd_entryp;
typedef
  struct ptoc_Type76
    {
    tsp_int2 va1cnt;
    tsp_int2 va2cnt;
    tsp_int2 va3cnt;
    tsp_int2 ndmax;
    tsp_int2 ndcnt;
    tsp_int2 ndfiller;
    tpc_nd_entryp ndtabp;
    } tpc_pc_sqlva;
typedef
  struct ptoc_Type77
    {
    tsp_int2 tabclause;
    tsp_int2 asclause;
    tsp_int2 indclause;
    tsp_knl_identifier dcluser;
    tsp_knl_identifier dcltab;
    tsp_knl_identifier dclmodul;
    tsp_int2 dcltabl;
    struct ptoc_Type78
      {
      tsp_c132 dclattr;
      tsp_int2 dclattrl;
      tsp_int2 dclfnlen;
      } clause[2];
    } tpc_dclgen;
typedef
  struct ptoc_Type79
    {
    tsp_name progid;
    tsp_name partptst;
    tsp_int4 cicscount;
    char iscics;
    char pass1;
    } tpc_pc_cics;
typedef
  struct ptoc_Type80
    {
    tsp_int2 uscom;
    tsp_knl_identifier usdobjn1;
    tsp_int2 usfiller;
    tsp_knl_identifier usdobjn2;
    } tpc_pc_usage;
typedef
  unsigned char tpc_partbuffer[16000];
typedef
  tpc_partbuffer *tpc_partbufptr;
typedef
  struct ptoc_Type81
    {
    tsp_int4 part1len;
    tsp_int4 part2len;
    tsp_int4 partmaxlen;
    tsp_int4 partsizlen;
    tpc_partbufptr partbufp;
    } tpc_cmd_part;
typedef
  struct ptoc_Type82
    {
    tpc_pre_errors pcerror;
    tsp_int2 pcerrcnt;
    tsp_int2 pcwrncnt;
    tsp4_xuserset pcxuserset;
    tsp_int2 pcendsymlen;
    tsp_sname pcendsym;
    tsp_c2 pcendcmd;
    tpc_endstype pcendse;
    tsp_int4 pclineperpage;
    tsp_int4 pccharperline;
    tsp_knl_identifier pcfilen;
    tsp_name pcfilet;
    tsp_name pcfilem;
    tsp4_argline pcargs;
    tsp_int2 pcexecsql;
    tsp_int2 pcstmax;
    tsp_int2 pcststm;
    tsp_int4 pcpaloop;
    tsp_int2 pckaatind;
    tsp_int2 pckacuind;
    tsp_int2 pcat;
    tsp_int2 pccu;
    tsp_int2 pcatgacnt;
    tsp_int2 pcpa;
    tsp_int2 pcsa;
    tsp_int2 pcka;
    tsp_int2 pcpr;
    tsp_int2 pcst;
    tsp_int2 pcfa;
    tsp_int2 pcfn;
    tsp_int2 pcdi;
    tsp_int2 pcna;
    tsp_int2 pcop;
    tsp_int2 pcda;
    tsp_int2 pcbnddsc;
    tsp_int2 pcseldsc;
    tsp4_args_options pcopts;
    tpc_vffiletype pcvf;
    tsp_int2 pccodeind;
    tsp_int2 pcinpind;
    tpc_tprelinerec pcinpline;
    tpc_tprelinerec pcoutline;
    char pcinpeof;
    char pcsqlcom;
    char pccomment;
    tpr_scannertype pcscan;
    tpc_analysetype pcan;
    tpc_cob_section pccobsect;
    tpc_whenevertype pcwhe;
    tsp_int2 pcwheset;
    tsp_int2 pcwheno;
    tpc_const_sec pccse;
    tsp_int2 pccmcnt;
    tpc_lnorecord pclno;
    sqlcatype sqlca;
    sqlxatype sqlxa;
    tpc_symtab pcsymtab;
    tpc_pc_sqlva pcsqlva;
    tpc_dclgen pcdclgen;
    tpc_pc_cics pccics;
    tpc_pc_usage pcusage;
    tpc_cmd_part pccmdpart;
    } tpc_globals;
int m90c30();
int m90int();
int m90int4();
int m90int2();
int m90name();
int m90lname();
int m90identifier();
int m90buf();
int p14batdatabase();
int p14bcudeclare();
int p14bcusearch();
short p14symb();
int p14binitsqlvan();
int p14ansqlstatement();
int p14an1sqlstatement();
int p14anaparameter();
int p14btypentget();
int p14putsqlstarea();
int p16tabledclgen();
int p19sqlkaallocate();
int p19sqlprallocate();
int p19sqlfaallocate();
int p19sqlstallocate();
int p19sqlfnallocate();
int p19setprsqlindex();
int p19setsqlstindex();
int p19cparentry();
int p19getname();
int p19sprepareprname();
int p19sstatementprname();
int p19scursorprname();
int p19snextsubchar();
int p19gmacronumber();
int p10getkeyword();
int p10stringliteralget();
int p10int2unsignedget();
int p10rmspace();
int p11vfopen();
int p11precomerror();
int p11wrerrorlisting();
int p11nerrorlisting();
int pr01eInitFileName();
int p14putval();
int p14getval();
int p05up1casebuf();
int p05up2casebuf();
int p05searchword();
int p05inttochr12();
int s10fil();
int s10fil1();
int s10fil2();
int s10mv1();
int s10mv2();
int s10mv3();
int s10mv4();
int s10mv5();
int s10mv6();
int s10mv7();
int s10mv8();
int s10mv9();
int s10mv10();
int s10mv11();
int s10mv12();
int s10mv13();
int s10mv14();
int s10mv15();
int s10mv16();
int s10mv17();
int s10mv18();
int s10mvr();
char s30eq();
int s30len();
int s30len1();
int s30lnr();
int m90print();
int p14end();
int p14sqlfn_areaput();
int p14oracle();
int p14_put_dclgen();
int p14open();
int p14include();
int p14begin();
int p14command();
int p14cancel();
int p14abortsession();
int p14version();
int p14_hostv_get();
int p14_prind_get();
int p14_add_prind();
int p14prepare();
int p14execute();
int p14declare();
int p14describe();
int p14close();
int p14forloop();
int p14fetch();
int p14whenever();
int p14_get_action();
int p14gmacrono();
int p14nolog();
int p14option();
int p14gdbsname();
int p14gtrace();
int p14traceline();
char p14ifstbind();
int p14_dclgen();
int p14getname();
int p14oradescribe();
int p14kaentryinit();



int p14end(next_symbol, apc)
int (*next_symbol)();
tpc_globals *apc;
    {
    int keyind;
    int i;
    tpc_globals *ptoc_Var0;
    tpr_scannertype *ptoc_Var1;
    tpc_cmd_part *ptoc_Var2;
    int ptoc_Var3;
    tpc_const_sec *ptoc_Var4;
    struct ptoc_Type69 *ptoc_Var5;
    m90print("PC14  p14end    ");
    ptoc_Var0 = &*apc;
    ptoc_Var1 = &ptoc_Var0->pcscan;
    ptoc_Var2 = &ptoc_Var0->pccmdpart;
    if ((int ) ptoc_Var0->pcerror == 0)
        {
        (*next_symbol)(&(*apc) , sql__nil(ptoc_Var2->partbufp , "vpc14.p" , 6421) , ptoc_Var2->part1len);
        p10getkeyword(&(*apc) , sql__nil(ptoc_Var2->partbufp , "vpc14.p" , 6422) , &keyind);
        if (keyind == 31)
            {
            (*next_symbol)(&(*apc) , sql__nil(ptoc_Var2->partbufp , "vpc14.p" , 6426) , ptoc_Var2->part1len);
            p10getkeyword(&(*apc) , sql__nil(ptoc_Var2->partbufp , "vpc14.p" , 6427) , &keyind);
            switch (keyind)
                {
                case 20 :
                    if ((int ) ptoc_Var0->pcan.ansection != 1)
                        {
                        p11wrerrorlisting(&(*apc) , 36);
                        }
                    ptoc_Var0->pcan.ansection = 0;
                    (*next_symbol)(&(*apc) , sql__nil(ptoc_Var2->partbufp , "vpc14.p" , 6436) , ptoc_Var2->part1len);
                    p10getkeyword(&(*apc) , sql__nil(ptoc_Var2->partbufp , "vpc14.p" , 6437) , &keyind);
                    if (keyind != 93)
                        {
                        p11wrerrorlisting(&(*apc) , 81);
                        }
                    ptoc_Var3 = ptoc_Var0->pccse.ccnt;
                    if (1 <= ptoc_Var3)
                        {
                        i = 1;
                        do
                            {
                            ptoc_Var4 = &ptoc_Var0->pccse;
                            ptoc_Var5 = &(*(tpc_csectionp *) sql__nil(ptoc_Var4->csec , "vpc14.p" , 6443))[sql__setrang(i , 1 , 100) + -1];
                            m90lname(14 , ptoc_Var5->cname);
                            m90int4(14 , "cwert       " , (int ) ptoc_Var5->cwert);
                            if (i == ptoc_Var3)
                                {
                                break;
                                }
                            i += 1;
                            }
                        while (!0);
                        }
                    break;
                case 129 :
                    if ((int ) ptoc_Var0->pcan.ansection != 2)
                        {
                        p11wrerrorlisting(&(*apc) , 36);
                        }
                    ptoc_Var0->pcan.ansection = 0;
                    (*next_symbol)(&(*apc) , sql__nil(ptoc_Var2->partbufp , "vpc14.p" , 6458) , ptoc_Var2->part1len);
                    p10getkeyword(&(*apc) , sql__nil(ptoc_Var2->partbufp , "vpc14.p" , 6459) , &keyind);
                    if (keyind != 93)
                        {
                        p11wrerrorlisting(&(*apc) , 81);
                        }
                    break;
                case 88 :
                    if ((int ) ptoc_Var0->pcan.ansection != 4)
                        {
                        p11wrerrorlisting(&(*apc) , 36);
                        }
                    ptoc_Var0->pcan.ansection = 5;
                    break;
                case 93 :
                    if ((int ) ptoc_Var0->pcan.ansection != 3)
                        {
                        p11wrerrorlisting(&(*apc) , 36);
                        }
                    ptoc_Var0->pcan.ansection = 0;
                    break;
                default :
                    p11wrerrorlisting(&(*apc) , 81);
                    break;
                }
            if ((int ) ptoc_Var0->pcerror == 0)
                {
                (*next_symbol)(&(*apc) , sql__nil(ptoc_Var2->partbufp , "vpc14.p" , 6488) , ptoc_Var2->part1len);
                if ((int ) ptoc_Var1->symb != 4 && (int ) ptoc_Var1->symb != 23)
                    {
                    p11wrerrorlisting(&(*apc) , 37);
                    }
                }
            }
        else
            {
            p11wrerrorlisting(&(*apc) , 81);
            }
        }
    while ((int ) ptoc_Var1->symb != 4 && (int ) ptoc_Var1->symb != 23)
        (*next_symbol)(&(*apc) , sql__nil(ptoc_Var2->partbufp , "vpc14.p" , 6502) , ptoc_Var2->part1len);
    m90name(22 , "END p14end        ");
    }



int p14sqlfn_areaput(apc)
tpc_globals *apc;
    {
    int i;
    int len;
    int pos;
    tpc_globals *ptoc_Var6;
    sqlxatype *ptoc_Var7;
    struct ptoc_Type62 *ptoc_Var8;
    struct ptoc_Type57 *ptoc_Var9;
    int ptoc_Var10;
    m90print("PC14  p14sqlfn_a");
    ptoc_Var6 = &*apc;
    ptoc_Var7 = &ptoc_Var6->sqlxa;
    ptoc_Var8 = &ptoc_Var6->pcvf.vffn[7];
    p19sqlfnallocate(&ptoc_Var6->pcfn);
    m90int2(14 , "pcfn        " , (short ) sql__setrang(ptoc_Var6->pcfn , -32768 , 32767));
    ptoc_Var9 = &(*(sqlfntype *) sql__nil(ptoc_Var7->sqlfnp , "vpc14.p" , 6527))[sql__setrang((int ) ptoc_Var6->pcfn , 1 , 50) + -1];
    ptoc_Var9->fnkanomin = (short ) sql__setrang((short ) ((int ) ptoc_Var6->pcka + 1) , -32768 , 32767);
    ptoc_Var9->fnkanomax = 32767;
    _ptoc_ASSIGN(unsigned char , 64, ptoc_Var9->fnfilen, "                                                                ")
    ptoc_Var9->fnincllno = (short ) sql__setrang(ptoc_Var6->pclno.lnosrc , -32768 , 32767);
    len = s30len(ptoc_Var8->vffilen , (unsigned char)' ' , 64);
    pos = 1;
    m90int2(14 , "fnkanomin   " , (short ) sql__setrang(ptoc_Var9->fnkanomin , -32768 , 32767));
    m90int2(14 , "fnkanomax   " , (short ) sql__setrang(ptoc_Var9->fnkanomax , -32768 , 32767));
    m90int2(14 , "fnincllno   " , (short ) sql__setrang(ptoc_Var9->fnincllno , -32768 , 32767));
    m90int2(14 , "len         " , (short ) sql__setrang((short ) len , -32768 , 32767));
    m90int2(14 , "pos         " , (short ) sql__setrang((short ) pos , -32768 , 32767));
    ptoc_Var10 = len;
    if (1 <= ptoc_Var10)
        {
        i = 1;
        do
            {
            if ((int ) ptoc_Var8->vffilen[sql__setrang(i , 1 , 64) + -1] == 47)
                {
                pos = i + 1;
                }
            if (i == ptoc_Var10)
                {
                break;
                }
            i += 1;
            }
        while (!0);
        }
    if (len > 18)
        {
        len = 18;
        }
    m90int2(14 , "len  n      " , (short ) sql__setrang((short ) len , -32768 , 32767));
    m90int2(14 , "pos  n      " , (short ) sql__setrang((short ) pos , -32768 , 32767));
    s10mv17(64 , sizeof(unsigned char [64]) , ptoc_Var8->vffilen , pos , ptoc_Var9->fnfilen , 1 , len);
    }



int p14oracle(next_symbol, apc)
int (*next_symbol)();
tpc_globals *apc;
    {
    tsp_int2 retpos;
    tpc_globals *ptoc_Var11;
    tpr_scannertype *ptoc_Var12;
    sqlcatype *ptoc_Var13;
    sqlratype *ptoc_Var14;
    tpc_cmd_part *ptoc_Var15;
    sqlxatype *ptoc_Var16;
    int ptoc_Var17;
    m90print("PC14  p14oracle ");
    ptoc_Var11 = &*apc;
    ptoc_Var12 = &ptoc_Var11->pcscan;
    ptoc_Var13 = &ptoc_Var11->sqlca;
    ptoc_Var14 = (sqlratype *) sql__nil(ptoc_Var13->sqlrap , "vpc14.p" , 6572);
    ptoc_Var15 = &ptoc_Var11->pccmdpart;
    ptoc_Var16 = &ptoc_Var11->sqlxa;
    retpos = (short ) sql__setrang((short ) ptoc_Var12->sypos , -32768 , 32767);
    if ((ptoc_Var17 = (int ) ptoc_Var14->ralang) == 3 || ptoc_Var17 == 4)
        {
        while ((int ) ptoc_Var12->symb != 4 && (int ) ptoc_Var12->symb != 23)
            (*next_symbol)(&(*apc) , sql__nil(ptoc_Var15->partbufp , "vpc14.p" , 6579) , ptoc_Var15->part1len);
        }
    if ((int ) ptoc_Var14->ralang == 4)
        {
        ptoc_Var15->part1len = ptoc_Var15->part1len + 1;
        (*(tpc_partbuffer *) sql__nil(ptoc_Var15->partbufp , "vpc14.p" , 6586))[sql__setrang(ptoc_Var15->part1len , 1 , 16000) + -1] = (unsigned char)'\0';
        }
    ptoc_Var12->syposacc = (int ) retpos;
    ptoc_Var12->symb = 0;
    p11wrerrorlisting(&(*apc) , 6);
    while ((int ) ptoc_Var12->symb != 4 && (int ) ptoc_Var12->symb != 23)
        (*next_symbol)(&(*apc) , sql__nil(ptoc_Var15->partbufp , "vpc14.p" , 6593) , ptoc_Var15->part1len);
    m90name(22 , "END p14oracle     ");
    }



int p14_put_dclgen(apc)
tpc_globals *apc;
    {
    tsp_sname sn;
    int len;
    tpc_globals *ptoc_Var18;
    tpc_cmd_part *ptoc_Var19;
    tpc_dclgen *ptoc_Var20;
    m90print("PC14  p14_put_dc");
    ptoc_Var18 = &*apc;
    ptoc_Var19 = &ptoc_Var18->pccmdpart;
    ptoc_Var20 = &ptoc_Var18->pcdclgen;
    ptoc_Var19->part1len = 0;
    if ((int ) ptoc_Var18->pcerror == 0)
        {
        s10fil(ptoc_Var19->partsizlen , sql__nil(ptoc_Var19->partbufp , "vpc14.p" , 6619) , 1 , 300 , (unsigned char)' ');
        _ptoc_ASSIGN(unsigned char , 12, sn, "   INCLUDE  ")
        s10mv11(12 , ptoc_Var19->partsizlen , sn , 1 , sql__nil(ptoc_Var19->partbufp , "vpc14.p" , 6622) , ptoc_Var19->part1len + 1 , 12);
        ptoc_Var19->part1len = ptoc_Var19->part1len + 13;
        (*(tpc_partbuffer *) sql__nil(ptoc_Var19->partbufp , "vpc14.p" , 6625))[sql__setrang(ptoc_Var19->part1len , 1 , 16000) + -1] = (unsigned char ) sql__setrang(ptoc_Var18->pcopts.tsp4_component.C_sp4co_sql_precompiler.opt_quo[0] , 0 , 255);
        len = (int ) ptoc_Var20->clause[0].dclfnlen;
        s10mv12(64 , ptoc_Var19->partsizlen , ptoc_Var18->pcvf.vffn[7].vffilen , 1 , sql__nil(ptoc_Var19->partbufp , "vpc14.p" , 6629) , ptoc_Var19->part1len + 1 , len);
        ptoc_Var19->part1len = ptoc_Var19->part1len + len + 1;
        (*(tpc_partbuffer *) sql__nil(ptoc_Var19->partbufp , "vpc14.p" , 6631))[sql__setrang(ptoc_Var19->part1len , 1 , 16000) + -1] = (unsigned char ) sql__setrang(ptoc_Var18->pcopts.tsp4_component.C_sp4co_sql_precompiler.opt_quo[0] , 0 , 255);
        if ((int ) ptoc_Var20->tabclause == 122)
            {
            _ptoc_ASSIGN(unsigned char , 12, sn, " TABLE      ")
            }
        else
            {
            _ptoc_ASSIGN(unsigned char , 12, sn, " DBPROC     ")
            }
        s10mv11(12 , ptoc_Var19->partsizlen , sn , 1 , sql__nil(ptoc_Var19->partbufp , "vpc14.p" , 6639) , ptoc_Var19->part1len + 2 , 9);
        ptoc_Var19->part1len = ptoc_Var19->part1len + 10;
        if (_ptoc_MEMCMP(unsigned char , 64, ptoc_Var20->dcluser , "                                                                ") == 0 ? 0 : 1)
            {
            len = s30len1(ptoc_Var20->dcluser , (unsigned char)' ' , sizeof(unsigned char [64]));
            s10mv13(sizeof(unsigned char [64]) , ptoc_Var19->partsizlen , ptoc_Var20->dcluser , 1 , sql__nil(ptoc_Var19->partbufp , "vpc14.p" , 6646) , ptoc_Var19->part1len + 1 , len);
            ptoc_Var19->part1len = ptoc_Var19->part1len + len + 2;
            (*(tpc_partbuffer *) sql__nil(ptoc_Var19->partbufp , "vpc14.p" , 6649))[sql__setrang(ptoc_Var19->part1len , 1 , 16000) + -1] = (unsigned char)'.';
            }
        s10mv13(sizeof(unsigned char [64]) , ptoc_Var19->partsizlen , ptoc_Var20->dcltab , 1 , sql__nil(ptoc_Var19->partbufp , "vpc14.p" , 6653) , ptoc_Var19->part1len + 1 , (int ) ptoc_Var20->dcltabl);
        ptoc_Var19->part1len = ptoc_Var19->part1len + (int ) ptoc_Var20->dcltabl + 1;
        if (_ptoc_MEMCMP(unsigned char , 64, ptoc_Var20->dclmodul , "                                                                ") == 0 ? 0 : 1)
            {
            (*(tpc_partbuffer *) sql__nil(ptoc_Var19->partbufp , "vpc14.p" , 6659))[sql__setrang(ptoc_Var19->part1len + 1 , 1 , 16000) + -1] = (unsigned char)'.';
            len = s30len1(ptoc_Var20->dclmodul , (unsigned char)' ' , sizeof(unsigned char [64]));
            s10mv13(sizeof(unsigned char [64]) , ptoc_Var19->partsizlen , ptoc_Var20->dclmodul , 1 , sql__nil(ptoc_Var19->partbufp , "vpc14.p" , 6662) , ptoc_Var19->part1len + 2 , len);
            ptoc_Var19->part1len = ptoc_Var19->part1len + len + 2;
            }
        switch ((int ) ptoc_Var20->asclause)
            {
            case 3 :
                _ptoc_ASSIGN(unsigned char , 12, sn, " AS RECORD  ")
                break;
            case 1 :
                _ptoc_ASSIGN(unsigned char , 12, sn, " AS TYPE    ")
                break;
            case 2 :
                _ptoc_ASSIGN(unsigned char , 12, sn, " AS STRUCT  ")
                break;
            default :
                _ptoc_ASSIGN(unsigned char , 12, sn, " AS VAR     ")
                break;
            }
        s10mv11(12 , ptoc_Var19->partsizlen , sn , 1 , sql__nil(ptoc_Var19->partbufp , "vpc14.p" , 6679) , ptoc_Var19->part1len + 1 , 12);
        ptoc_Var19->part1len = ptoc_Var19->part1len + 13;
        s10mv14(sizeof(unsigned char [132]) , ptoc_Var19->partsizlen , ptoc_Var20->clause[0].dclattr , 1 , sql__nil(ptoc_Var19->partbufp , "vpc14.p" , 6683) , ptoc_Var19->part1len + 1 , (int ) ptoc_Var20->clause[0].dclattrl);
        ptoc_Var19->part1len = ptoc_Var19->part1len + (int ) ptoc_Var20->clause[0].dclattrl + 1;
        if ((int ) ptoc_Var20->indclause == 1)
            {
            _ptoc_ASSIGN(unsigned char , 12, sn, "IND         ")
            s10mv11(12 , ptoc_Var19->partsizlen , sn , 1 , sql__nil(ptoc_Var19->partbufp , "vpc14.p" , 6691) , ptoc_Var19->part1len + 1 , 5);
            ptoc_Var19->part1len = ptoc_Var19->part1len + 6;
            s10mv14(sizeof(unsigned char [132]) , ptoc_Var19->partsizlen , ptoc_Var20->clause[1].dclattr , 1 , sql__nil(ptoc_Var19->partbufp , "vpc14.p" , 6694) , ptoc_Var19->part1len + 1 , (int ) ptoc_Var20->clause[1].dclattrl);
            ptoc_Var19->part1len = ptoc_Var19->part1len + (int ) ptoc_Var20->clause[1].dclattrl + 1;
            }
        }
    m90name(22 , "END p14_put_dclgen");
    }



int p14open(next_symbol, apc, openno)
int (*next_symbol)();
tpc_globals *apc;
int *openno;
    {
    int keyind;
    int ind;
    int openind;
    char found;
    tpc_pre_errors err;
    tsp_knl_identifier cursorn;
    int cursorl;
    tsp_int2 kaind;
    tsp_int2 macro;
    tsp_int2 param;
    tsp_int2 va1ix;
    tsp_c12 ch12;
    int no;
    tsp_int2 CursorNameIndex;
    tpr_scannertype rscan;
    int ndcuindex;
    tsp_int2 lineno;
    sqlkaentry kaentry;
    tsp_int2 cudiind;
    tsp00_Int2 dummy;
    tpc_globals *ptoc_Var21;
    tpr_scannertype *ptoc_Var22;
    sqlcatype *ptoc_Var23;
    sqlratype *ptoc_Var24;
    tpc_cmd_part *ptoc_Var25;
    sqlxatype *ptoc_Var26;
    sqlxatype *ptoc_Var27;
    sqlprentry *ptoc_Var28;
    sqlkaentry *ptoc_Var29;
    sqlkaentry *ptoc_Var30;
    sqlkaentry *ptoc_Var31;
    m90print("PC14  p14open   ");
    ptoc_Var21 = &*apc;
    ptoc_Var22 = &ptoc_Var21->pcscan;
    ptoc_Var23 = &ptoc_Var21->sqlca;
    ptoc_Var24 = (sqlratype *) sql__nil(ptoc_Var23->sqlrap , "vpc14.p" , 6737);
    ptoc_Var25 = &ptoc_Var21->pccmdpart;
    ptoc_Var26 = &ptoc_Var21->sqlxa;
    ptoc_Var26->xadescpos = 0;
    ptoc_Var26->xadesclen = 0;
    if ((int ) ptoc_Var21->pcerror == 0)
        {
        err = 0;
        *openno = 0;
        (*next_symbol)(&(*apc) , sql__nil(ptoc_Var25->partbufp , "vpc14.p" , 6746) , ptoc_Var25->part1len);
        p10getkeyword(&(*apc) , sql__nil(ptoc_Var25->partbufp , "vpc14.p" , 6747) , &keyind);
        if (keyind == 17)
            {
            ptoc_Var21->pcan.ancomtyp = 1;
            }
        else
            {
            ptoc_Var21->pcan.ancomtyp = 8;
            }
        p14getname(next_symbol , &(*apc) , cursorn , &cursorl , &macro , &param);
        if ((int ) param == 1)
            {
            dummy = 0;
            p14_add_prind(&(*apc) , &ind , &dummy);
            ind = (int ) ptoc_Var21->pcpr;
            rscan = ptoc_Var21->pcscan;
            p14_hostv_get(next_symbol , &(*apc) , &CursorNameIndex , &va1ix);
            no = (int ) va1ix;
            p05inttochr12(no , ch12);
            _ptoc_ASSIGN(unsigned char , 64, cursorn, "                                                                ")
            cursorn[0] = (unsigned char)':';
            s10mv18(12 , sizeof(unsigned char [64]) , ch12 , 2 , cursorn , 2 , 4);
            ptoc_Var21->pcscan = rscan;
            ndcuindex = (int ) ptoc_Var21->pcsqlva.ndcnt;
            }
        else
            {
            dummy = 0;
            p14_add_prind(&(*apc) , &ind , &dummy);
            ind = (int ) ptoc_Var21->pcpr;
            CursorNameIndex = 0;
            ndcuindex = 0;
            }
        if (ind == 0)
            {
            (*next_symbol)(&(*apc) , sql__nil(ptoc_Var25->partbufp , "vpc14.p" , 6790) , ptoc_Var25->part1len);
            p10getkeyword(&(*apc) , sql__nil(ptoc_Var25->partbufp , "vpc14.p" , 6791) , &keyind);
            if ((int ) ptoc_Var22->symb == 4 || keyind == 56 || keyind == 132)
                {
                err = 77;
                }
            else
                {
                p14ansqlstatement(next_symbol , &(*apc));
                }
            }
        else
            {
            ptoc_Var27 = &ptoc_Var21->sqlxa;
            ptoc_Var28 = &(*(sqlprtype *) sql__nil(ptoc_Var27->sqlprp , "vpc14.p" , 6803))[sql__setrang(ind , 1 , 500) + -1];
            cudiind = 0;
            if ((int ) ptoc_Var28->prcuindex > 0)
                {
                cudiind = (short ) sql__setrang((*(sqlprtype *) sql__nil(ptoc_Var27->sqlprp , "vpc14.p" , 6808))[sql__setrang((int ) ptoc_Var28->prcuindex , 1 , 500) + -1].prkaindex , -32768 , 32767);
                }
            m90int2(14 , "cudiind ****" , (short ) sql__setrang(cudiind , -32768 , 32767));
            m90int2(14 , "prcuindex   " , (short ) sql__setrang(ptoc_Var28->prcuindex , -32768 , 32767));
            m90int2(14 , "prkaindex   " , (short ) sql__setrang(ptoc_Var28->prkaindex , -32768 , 32767));
            m90int2(14 , "prStmtNameIn" , (short ) sql__setrang(ptoc_Var28->prStmtNameIndex , -32768 , 32767));
            m90int2(14 , "prarea      " , (short ) sql__setrang(ptoc_Var28->prarea , -32768 , 32767));
            m90int2(14 , "prstate     " , (short ) sql__setrang(ptoc_Var28->prstate , -32768 , 32767));
            m90int2(14 , "prstcount   " , (short ) sql__setrang(ptoc_Var28->prstcount , -32768 , 32767));
            m90int2(14 , "prstindex   " , (short ) sql__setrang(ptoc_Var28->prstindex , -32768 , 32767));
            if ((int ) ptoc_Var21->pcinpind == 2)
                {
                lineno = (short ) sql__setrang(ptoc_Var21->pclno.lnosrc , -32768 , 32767);
                }
            else
                {
                lineno = (short ) sql__setrang((short ) -(int ) ptoc_Var21->pclno.lnoexp , -32768 , 32767);
                }
            (*next_symbol)(&(*apc) , sql__nil(ptoc_Var25->partbufp , "vpc14.p" , 6824) , ptoc_Var25->part1len);
            if ((int ) ptoc_Var22->symb == 4)
                {
                p14kaentryinit(&(*apc));
                _ptoc_ASSIGN(unsigned char , 64, ptoc_Var28->prCursorName, cursorn)
                ptoc_Var28->prCursorNameIndex = (short ) sql__setrang(CursorNameIndex , -32768 , 32767);
                ptoc_Var28->prkaindex = (short ) sql__setrang(ptoc_Var21->pcka , -32768 , 32767);
                ptoc_Var29 = &(*(sqlkatype *) sql__nil(ptoc_Var27->sqlkap , "vpc14.p" , 6835))[sql__setrang((int ) ptoc_Var21->pcka , 1 , 3000) + -1];
                ptoc_Var29->kaprindex = (short ) sql__setrang((short ) ind , -32768 , 32767);
                ptoc_Var29->kamacro = (short ) sql__setrang(macro , -32768 , 32767);
                ptoc_Var29->kacuindex = (short ) sql__setrang(ptoc_Var21->pckacuind , -32768 , 32767);
                ptoc_Var29->kalineno = (short ) sql__setrang(lineno , -32768 , 32767);
                *openno = (int ) ptoc_Var21->pcka;
                if ((int ) ptoc_Var28->prkaindex == 0)
                    {
                    }
                else
                    {
                    openind = (int ) ptoc_Var28->prkaindex;
                    found = 0;
                    do
                        {
                        m90int2(14 , "prkaindex   " , (short ) sql__setrang(ptoc_Var28->prkaindex , -32768 , 32767));
                        m90int2(14 , "openind     " , (short ) sql__setrang((short ) openind , -32768 , 32767));
                        if ((int ) (*(sqlkatype *) sql__nil(ptoc_Var27->sqlkap , "vpc14.p" , 6856))[sql__setrang(openind , 1 , 3000) + -1].katyp != (int ) ptoc_Var21->pcan.ancomtyp || (int ) (*(sqlkatype *) sql__nil(ptoc_Var27->sqlkap , "vpc14.p" , 6857))[sql__setrang(openind , 1 , 3000) + -1].kaprindex != ind)
                            {
                            openind = (int ) (*(sqlkatype *) sql__nil(ptoc_Var27->sqlkap , "vpc14.p" , 6859))[sql__setrang(openind , 1 , 3000) + -1].kadiindex;
                            }
                        else
                            {
                            found = 1;
                            }
                        }
                    while (!(openind == 0 || (char ) sql__setrang(found , 0 , 1)));
                    if (openind == 0)
                        {
                        openind = (int ) ptoc_Var28->prkaindex;
                        }
                    (*(sqlkatype *) sql__nil(ptoc_Var27->sqlkap , "vpc14.p" , 6870))[sql__setrang((int ) ptoc_Var21->pcka , 1 , 3000) + -1] = (*(sqlkatype *) sql__nil(ptoc_Var27->sqlkap , "vpc14.p" , 6870))[sql__setrang(openind , 1 , 3000) + -1];
                    (*(sqlkatype *) sql__nil(ptoc_Var27->sqlkap , "vpc14.p" , 6871))[sql__setrang((int ) ptoc_Var21->pcka , 1 , 3000) + -1].katyp = (short ) sql__setrang(ptoc_Var21->pcan.ancomtyp , -32768 , 32767);
                    (*(sqlkatype *) sql__nil(ptoc_Var27->sqlkap , "vpc14.p" , 6872))[sql__setrang((int ) ptoc_Var21->pcka , 1 , 3000) + -1].kalineno = (short ) sql__setrang(lineno , -32768 , 32767);
                    (*(sqlkatype *) sql__nil(ptoc_Var27->sqlkap , "vpc14.p" , 6873))[sql__setrang(openind , 1 , 3000) + -1].kadiindex = (short ) sql__setrang(ptoc_Var21->pcka , -32768 , 32767);
                    *openno = (int ) ptoc_Var21->pcka;
                    m90int2(14 , "kaprindex   " , (short ) sql__setrang((*(sqlkatype *) sql__nil(ptoc_Var27->sqlkap , "vpc14.p" , 6876))[sql__setrang((int ) ptoc_Var28->prkaindex , 1 , 3000) + -1].kaprindex , -32768 , 32767));
                    m90int2(14 , "prkaindex   " , (short ) sql__setrang(ptoc_Var28->prkaindex , -32768 , 32767));
                    m90int2(14 , "openind     " , (short ) sql__setrang((short ) openind , -32768 , 32767));
                    m90int2(14 , "pcpr        " , (short ) sql__setrang(ptoc_Var21->pcpr , -32768 , 32767));
                    m90int2(14 , "pcka        " , (short ) sql__setrang(ptoc_Var21->pcka , -32768 , 32767));
                    }
                m90int2(14 , "openno      " , (short ) sql__setrang((short ) *openno , -32768 , 32767));
                ptoc_Var21->pcan.ancheck = (short ) sql__setrang(ptoc_Var21->pcopts.tsp4_component.C_sp4co_sql_precompiler.opt_mode , -32768 , 32767);
                ptoc_Var21->sqlxa.xakano = (short ) sql__setrang((short ) *openno , -32768 , 32767);
                if (ndcuindex > 0)
                    {
                    (*(tpc_arrndtype *) sql__nil(ptoc_Var21->pcsqlva.ndtabp , "vpc14.p" , 6889))[sql__setrang(ndcuindex , 1 , 100) + -1].ndkano = (short ) sql__setrang(ptoc_Var21->pcka , -32768 , 32767);
                    }
                }
            else
                {
                p10getkeyword(&(*apc) , sql__nil(ptoc_Var25->partbufp , "vpc14.p" , 6895) , &keyind);
                switch (keyind)
                    {
                    case 56 :
                    case 132 :
                        if ((int ) ptoc_Var28->prkaindex == 0)
                            {
                            kaind = (short ) sql__setrang(ptoc_Var21->pcka , -32768 , 32767);
                            }
                        else
                            {
                            kaind = (short ) sql__setrang(ptoc_Var28->prkaindex , -32768 , 32767);
                            }
                        (*next_symbol)(&(*apc) , sql__nil(ptoc_Var25->partbufp , "vpc14.p" , 6905) , ptoc_Var25->part1len);
                        p10getkeyword(&(*apc) , sql__nil(ptoc_Var25->partbufp , "vpc14.p" , 6906) , &keyind);
                        if (keyind == 33)
                            {
                            p14kaentryinit(&(*apc));
                            (*(sqlkatype *) sql__nil(ptoc_Var27->sqlkap , "vpc14.p" , 6913))[sql__setrang((int ) ptoc_Var21->pcka , 1 , 3000) + -1] = (*(sqlkatype *) sql__nil(ptoc_Var27->sqlkap , "vpc14.p" , 6913))[sql__setrang((int ) kaind , 1 , 3000) + -1];
                            kaind = (short ) sql__setrang(ptoc_Var21->pcka , -32768 , 32767);
                            ptoc_Var30 = &(*(sqlkatype *) sql__nil(ptoc_Var27->sqlkap , "vpc14.p" , 6915))[sql__setrang((int ) kaind , 1 , 3000) + -1];
                            ptoc_Var30->kadiindex = (short ) sql__setrang(ptoc_Var28->prkaindex , -32768 , 32767);
                            ptoc_Var28->prkaindex = (short ) sql__setrang(ptoc_Var21->pcka , -32768 , 32767);
                            _ptoc_ASSIGN(unsigned char , 64, ptoc_Var28->prCursorName, cursorn)
                            ptoc_Var28->prCursorNameIndex = (short ) sql__setrang(CursorNameIndex , -32768 , 32767);
                            if (ndcuindex > 0)
                                {
                                (*(tpc_arrndtype *) sql__nil(ptoc_Var21->pcsqlva.ndtabp , "vpc14.p" , 6924))[sql__setrang(ndcuindex , 1 , 100) + -1].ndkano = (short ) sql__setrang(ptoc_Var21->pcka , -32768 , 32767);
                                }
                            (*next_symbol)(&(*apc) , sql__nil(ptoc_Var25->partbufp , "vpc14.p" , 6927) , ptoc_Var25->part1len);
                            if ((int ) ptoc_Var22->symb != 4)
                                {
                                if ((int ) ptoc_Var24->rakamode != 1)
                                    {
                                    if ((int ) (*(tpc_partbuffer *) sql__nil(ptoc_Var25->partbufp , "vpc14.p" , 6934))[sql__setrang(ptoc_Var22->sypos , 1 , 16000) + -1] == 58)
                                        {
                                        ptoc_Var27->xadescpos = (short ) sql__setrang((short ) (ptoc_Var22->sypos + 1) , -32768 , 32767);
                                        }
                                    else
                                        {
                                        ptoc_Var27->xadescpos = (short ) sql__setrang((short ) ptoc_Var22->sypos , -32768 , 32767);
                                        }
                                    ptoc_Var27->xadesclen = (short ) sql__setrang((short ) (ptoc_Var22->syposacc - (int ) ptoc_Var27->xadescpos) , -32768 , 32767);
                                    (*next_symbol)(&(*apc) , sql__nil(ptoc_Var25->partbufp , "vpc14.p" , 6942) , ptoc_Var25->part1len);
                                    }
                                else
                                    {
                                    if ((int ) (*(tpc_partbuffer *) sql__nil(ptoc_Var25->partbufp , "vpc14.p" , 6946))[sql__setrang(ptoc_Var22->sypos , 1 , 16000) + -1] == 58)
                                        {
                                        ptoc_Var27->xadescpos = (short ) sql__setrang((short ) (ptoc_Var22->sypos + 1) , -32768 , 32767);
                                        }
                                    else
                                        {
                                        p10getkeyword(&(*apc) , sql__nil(ptoc_Var25->partbufp , "vpc14.p" , 6951) , &keyind);
                                        if (keyind != 149)
                                            {
                                            ptoc_Var27->xadescpos = (short ) sql__setrang((short ) ptoc_Var22->sypos , -32768 , 32767);
                                            ptoc_Var27->xadesclen = (short ) sql__setrang((short ) (ptoc_Var22->syposacc - (int ) ptoc_Var27->xadescpos) , -32768 , 32767);
                                            (*next_symbol)(&(*apc) , sql__nil(ptoc_Var25->partbufp , "vpc14.p" , 6957) , ptoc_Var25->part1len);
                                            }
                                        else
                                            {
                                            ptoc_Var27->xadescpos = -1;
                                            }
                                        }
                                    }
                                p10getkeyword(&(*apc) , sql__nil(ptoc_Var25->partbufp , "vpc14.p" , 6968) , &keyind);
                                if (keyind == 149)
                                    {
                                    ptoc_Var21->pcan.ancomtyp = 27;
                                    }
                                while ((int ) ptoc_Var22->symb != 4 && (int ) ptoc_Var22->symb != 23)
                                    (*next_symbol)(&(*apc) , sql__nil(ptoc_Var25->partbufp , "vpc14.p" , 6978) , ptoc_Var25->part1len);
                                }
                            else
                                {
                                if ((int ) ptoc_Var24->rakamode == 1)
                                    {
                                    ptoc_Var27->xadescpos = -1;
                                    }
                                else
                                    {
                                    err = 37;
                                    }
                                }
                            (*(sqlprtype *) sql__nil(ptoc_Var21->sqlxa.sqlprp , "vpc14.p" , 6994))[sql__setrang(ind , 1 , 500) + -1].prDescribe = 1;
                            (*(sqlkatype *) sql__nil(ptoc_Var21->sqlxa.sqlkap , "vpc14.p" , 6996))[sql__setrang((int ) kaind , 1 , 3000) + -1].kaprindex = (short ) sql__setrang((short ) ind , -32768 , 32767);
                            (*(sqlkatype *) sql__nil(ptoc_Var21->sqlxa.sqlkap , "vpc14.p" , 6997))[sql__setrang((int ) kaind , 1 , 3000) + -1].kapacount = -1;
                            (*(sqlkatype *) sql__nil(ptoc_Var21->sqlxa.sqlkap , "vpc14.p" , 6998))[sql__setrang((int ) kaind , 1 , 3000) + -1].katyp = (short ) sql__setrang(ptoc_Var21->pcan.ancomtyp , -32768 , 32767);
                            (*(sqlkatype *) sql__nil(ptoc_Var21->sqlxa.sqlkap , "vpc14.p" , 6999))[sql__setrang((int ) kaind , 1 , 3000) + -1].kalineno = (short ) sql__setrang(lineno , -32768 , 32767);
                            *openno = (int ) kaind;
                            }
                        else
                            {
                            if ((int ) ptoc_Var22->symb != 13)
                                {
                                err = 37;
                                }
                            else
                                {
                                kaentry = (*(sqlkatype *) sql__nil(ptoc_Var27->sqlkap , "vpc14.p" , 7008))[sql__setrang((int ) kaind , 1 , 3000) + -1];
                                p14an1sqlstatement(next_symbol , &(*apc));
                                m90int2(14 , "ind         " , (short ) sql__setrang((short ) ind , -32768 , 32767));
                                m90int2(14 , "kaind       " , (short ) sql__setrang(kaind , -32768 , 32767));
                                m90int2(14 , "pcka        " , (short ) sql__setrang(ptoc_Var21->pcka , -32768 , 32767));
                                m90int2(14 , "prkaindex   " , (short ) sql__setrang(ptoc_Var28->prkaindex , -32768 , 32767));
                                m90int2(14 , "kalineno    " , (short ) sql__setrang(lineno , -32768 , 32767));
                                ptoc_Var31 = &kaentry;
                                ptoc_Var31->kaprindex = (short ) sql__setrang((short ) ind , -32768 , 32767);
                                ptoc_Var31->katyp = (short ) sql__setrang(ptoc_Var21->pcan.ancomtyp , -32768 , 32767);
                                ptoc_Var31->kapacount = (short ) sql__setrang((*(sqlkatype *) sql__nil(ptoc_Var21->sqlxa.sqlkap , "vpc14.p" , 7020))[sql__setrang((int ) ptoc_Var21->pcka , 1 , 3000) + -1].kapacount , -32768 , 32767);
                                ptoc_Var31->kapaindex = (short ) sql__setrang((*(sqlkatype *) sql__nil(ptoc_Var21->sqlxa.sqlkap , "vpc14.p" , 7022))[sql__setrang((int ) ptoc_Var21->pcka , 1 , 3000) + -1].kapaindex , -32768 , 32767);
                                ptoc_Var31->kalineno = (short ) sql__setrang(lineno , -32768 , 32767);
                                if ((int ) cudiind != 0)
                                    {
                                    ptoc_Var31->kadiindex = (short ) sql__setrang(cudiind , -32768 , 32767);
                                    ptoc_Var28->prkaindex = (short ) sql__setrang(ptoc_Var21->pcka , -32768 , 32767);
                                    (*(sqlprtype *) sql__nil(ptoc_Var27->sqlprp , "vpc14.p" , 7030))[sql__setrang((int ) ptoc_Var28->prcuindex , 1 , 500) + -1].prkaindex = (short ) sql__setrang(ptoc_Var21->pcka , -32768 , 32767);
                                    }
                                else
                                    {
                                    ptoc_Var31->kadiindex = (short ) sql__setrang(ptoc_Var28->prkaindex , -32768 , 32767);
                                    ptoc_Var28->prkaindex = (short ) sql__setrang(ptoc_Var21->pcka , -32768 , 32767);
                                    }
                                _ptoc_ASSIGN(unsigned char , 64, ptoc_Var28->prCursorName, cursorn)
                                ptoc_Var28->prCursorNameIndex = (short ) sql__setrang(CursorNameIndex , -32768 , 32767);
                                (*(sqlkatype *) sql__nil(ptoc_Var21->sqlxa.sqlkap , "vpc14.p" , 7041))[sql__setrang((int ) ptoc_Var21->pcka , 1 , 3000) + -1] = kaentry;
                                ptoc_Var27->xakano = (short ) sql__setrang(ptoc_Var21->pcka , -32768 , 32767);
                                *openno = (int ) ptoc_Var21->pcka;
                                }
                            }
                        break;
                    default :
                        err = 37;
                        break;
                    }
                ptoc_Var21->sqlxa.xakano = (short ) sql__setrang((short ) *openno , -32768 , 32767);
                }
            }
        if ((int ) err != 0)
            {
            p11precomerror(&(*apc) , (unsigned char ) sql__setrang(err , 0 , 90));
            }
        }
    while ((int ) ptoc_Var22->symb != 4 && (int ) ptoc_Var22->symb != 23)
        (*next_symbol)(&(*apc) , sql__nil(ptoc_Var25->partbufp , "vpc14.p" , 7067) , ptoc_Var25->part1len);
    m90int2(14 , "xakano      " , (short ) sql__setrang(ptoc_Var26->xakano , -32768 , 32767));
    m90int2(14 , "xadescpos   " , (short ) sql__setrang(ptoc_Var26->xadescpos , -32768 , 32767));
    m90int2(14 , "xadesclen   " , (short ) sql__setrang(ptoc_Var26->xadesclen , -32768 , 32767));
    m90name(22 , "END p14open       ");
    }



int p14include(next_symbol, apc, keyind, dclgen)
int (*next_symbol)();
tpc_globals *apc;
int *keyind;
char *dclgen;
    {
    int len;
    int pos;
    tpr_scannertype rscan;
    char inclfile;
    tpc_globals *ptoc_Var32;
    tpr_scannertype *ptoc_Var33;
    sqlcatype *ptoc_Var34;
    sqlratype *ptoc_Var35;
    tpc_cmd_part *ptoc_Var36;
    tpc_dclgen *ptoc_Var37;
    struct ptoc_Type62 *ptoc_Var38;
    int ptoc_Var39;
    struct ptoc_Type62 *ptoc_Var40;
    struct ptoc_Type62 *ptoc_Var41;
    m90print("PC14  p14include");
    ptoc_Var32 = &*apc;
    ptoc_Var33 = &ptoc_Var32->pcscan;
    ptoc_Var34 = &ptoc_Var32->sqlca;
    ptoc_Var35 = (sqlratype *) sql__nil(ptoc_Var34->sqlrap , "vpc14.p" , 7092);
    ptoc_Var36 = &ptoc_Var32->pccmdpart;
    ptoc_Var37 = &ptoc_Var32->pcdclgen;
    *keyind = 0;
    *dclgen = 0;
    inclfile = 0;
    ptoc_Var37->asclause = 99;
    if ((int ) ptoc_Var32->pcerror == 0)
        {
        (*next_symbol)(&(*apc) , sql__nil(ptoc_Var36->partbufp , "vpc14.p" , 7101) , ptoc_Var36->part1len);
        if ((int ) ptoc_Var33->symb == 8 || (int ) ptoc_Var35->ralang == 3 && (int ) ptoc_Var33->symb != 21 && (int ) ptoc_Var33->symb != 24)
            {
            p10getkeyword(&(*apc) , sql__nil(ptoc_Var36->partbufp , "vpc14.p" , 7108) , &(*keyind));
            switch ((int ) ptoc_Var35->ralang)
                {
                case 11 :
                case 10 :
                case 12 :
                case 9 :
                    switch (*keyind)
                        {
                        case 104 :
                            if ((int ) ptoc_Var32->pccodeind > 3)
                                {
                                p11precomerror(&(*apc) , 19);
                                }
                            ptoc_Var32->pccodeind = 4;
                            break;
                        case 112 :
                            if ((int ) ptoc_Var32->pccodeind > 4)
                                {
                                p11precomerror(&(*apc) , 19);
                                }
                            ptoc_Var32->pccodeind = 5;
                            break;
                        case 113 :
                        case 103 :
                            if ((int ) ptoc_Var32->pccodeind != 4)
                                {
                                p11precomerror(&(*apc) , 19);
                                }
                            break;
                        default :
                            p11wrerrorlisting(&(*apc) , 81);
                            break;
                        }
                    break;
                case 4 :
                case 7 :
                    break;
                case 3 :
                    if (*keyind == 103 || *keyind == 105 || *keyind == 77)
                        {
                        while ((int ) ptoc_Var33->symb != 4 && (int ) ptoc_Var33->symb != 23)
                            (*next_symbol)(&(*apc) , sql__nil(ptoc_Var36->partbufp , "vpc14.p" , 7152) , ptoc_Var36->part1len);
                        }
                    else
                        {
                        if ((int ) ptoc_Var35->rakamode != 4 && (int ) ptoc_Var35->rakamode != 5)
                            {
                            p11wrerrorlisting(&(*apc) , 81);
                            }
                        else
                            {
                            if ((int ) ptoc_Var32->pcinpind == 8)
                                {
                                p11wrerrorlisting(&(*apc) , 22);
                                }
                            else
                                {
                                ptoc_Var38 = &ptoc_Var32->pcvf.vffn[7];
                                inclfile = 1;
                                ptoc_Var38->vffileno = 0;
                                ptoc_Var38->vffilemode = 0;
                                ptoc_Var38->vfbinary = 0;
                                ptoc_Var38->vfreclen2 = 0;
                                ptoc_Var38->vfreclen4 = 0;
                                pr01eInitFileName(ptoc_Var38->vffilen);
                                ptoc_Var38->vfcnt = 0;
                                pos = 1;
                                while ((int ) ptoc_Var33->symb != 4 && (int ) ptoc_Var33->symb != 23)
                                    {
                                    if (ptoc_Var33->sylen + pos > 64)
                                        {
                                        len = 64 - pos;
                                        }
                                    else
                                        {
                                        len = ptoc_Var33->sylen;
                                        }
                                    s10mv5(ptoc_Var36->partsizlen , 64 , sql__nil(ptoc_Var36->partbufp , "vpc14.p" , 7190) , ptoc_Var33->sypos , ptoc_Var38->vffilen , pos , len);
                                    pos = pos + len;
                                    (*next_symbol)(&(*apc) , sql__nil(ptoc_Var36->partbufp , "vpc14.p" , 7193) , ptoc_Var36->part1len);
                                    }
                                }
                            }
                        }
                    break;
                default :
                    p11wrerrorlisting(&(*apc) , 60);
                    break;
                }
            if ((int ) ptoc_Var32->pcerror == 0 && (int ) ptoc_Var33->symb != 4 && (int ) ptoc_Var33->symb != 23 && (int ) ptoc_Var35->ralang != 4)
                {
                rscan = ptoc_Var32->pcscan;
                (*next_symbol)(&(*apc) , sql__nil(ptoc_Var36->partbufp , "vpc14.p" , 7213) , ptoc_Var36->part1len);
                if ((int ) ptoc_Var33->symb != 4 && (int ) ptoc_Var33->symb != 23)
                    {
                    p11wrerrorlisting(&(*apc) , 37);
                    }
                if ((ptoc_Var39 = (int ) ptoc_Var35->ralang) != 3 && ptoc_Var39 != 4)
                    {
                    ptoc_Var32->pcscan = rscan;
                    }
                }
            }
        else
            {
            if ((int ) ptoc_Var33->symb != 21 && (int ) ptoc_Var33->symb != 24)
                {
                p11wrerrorlisting(&(*apc) , 37);
                }
            }
        if ((int ) ptoc_Var33->symb == 21 || (int ) ptoc_Var33->symb == 24)
            {
            p10stringliteralget(&(*apc) , sql__nil(ptoc_Var36->partbufp , "vpc14.p" , 7237));
            if ((int ) ptoc_Var32->pcinpind == 8)
                {
                p11wrerrorlisting(&(*apc) , 22);
                }
            else
                {
                ptoc_Var40 = &ptoc_Var32->pcvf.vffn[7];
                inclfile = 1;
                ptoc_Var40->vffileno = 0;
                ptoc_Var40->vffilemode = 0;
                ptoc_Var40->vfbinary = 0;
                ptoc_Var40->vfreclen2 = 0;
                ptoc_Var40->vfreclen4 = 0;
                pr01eInitFileName(ptoc_Var40->vffilen);
                ptoc_Var40->vfcnt = 0;
                if (ptoc_Var33->sylen > 64)
                    {
                    len = 64;
                    }
                else
                    {
                    len = ptoc_Var33->sylen;
                    }
                s10mv5(ptoc_Var36->partsizlen , 64 , sql__nil(ptoc_Var36->partbufp , "vpc14.p" , 7258) , ptoc_Var33->sypos , ptoc_Var40->vffilen , 1 , len);
                p14_dclgen(next_symbol , &(*apc) , &(*dclgen) , &len);
                }
            }
        if ((char ) sql__setrang(inclfile , 0 , 1))
            {
            ptoc_Var41 = &ptoc_Var32->pcvf.vffn[7];
            if ((int ) ptoc_Var32->pcan.ansection != 0)
                {
                ptoc_Var32->pcsqlcom = 1;
                }
            p11vfopen(&(*apc) , 8);
            if ((int ) ptoc_Var32->pcerror == 0)
                {
                ptoc_Var32->pcinpind = 8;
                p14sqlfn_areaput(&(*apc));
                if ((char ) sql__setrang(*dclgen , 0 , 1))
                    {
                    p11wrerrorlisting(&(*apc) , 20);
                    ptoc_Var32->sqlca.sqlcode = 0;
                    ptoc_Var32->pcerror = 0;
                    }
                *dclgen = 0;
                }
            else
                {
                if ((char ) sql__setrang(*dclgen , 0 , 1) && (int ) ptoc_Var32->pcopts.tsp4_component.C_sp4co_sql_precompiler.opt_mode != 0)
                    {
                    p16tabledclgen(&(*apc) , &(*dclgen));
                    if ((char ) sql__setrang(*dclgen , 0 , 1) && ptoc_Var34->sqlcode == 0)
                        {
                        ptoc_Var32->pcerror = 0;
                        ptoc_Var41->vffilemode = 1;
                        p11vfopen(&(*apc) , 8);
                        p14_put_dclgen(&(*apc));
                        }
                    }
                if (!(char ) sql__setrang(*dclgen , 0 , 1) || (int ) ptoc_Var32->pcerror != 0)
                    {
                    *dclgen = 0;
                    p11wrerrorlisting(&(*apc) , 23);
                    }
                }
            }
        }
    m90name(22 , "END p14include    ");
    }



int p14begin(next_symbol, apc)
int (*next_symbol)();
tpc_globals *apc;
    {
    int keyind;
    tpc_globals *ptoc_Var42;
    tpr_scannertype *ptoc_Var43;
    sqlcatype *ptoc_Var44;
    sqlratype *ptoc_Var45;
    tpc_cmd_part *ptoc_Var46;
    m90print("PC14  p14begin  ");
    ptoc_Var42 = &*apc;
    ptoc_Var43 = &ptoc_Var42->pcscan;
    ptoc_Var44 = &ptoc_Var42->sqlca;
    ptoc_Var45 = (sqlratype *) sql__nil(ptoc_Var44->sqlrap , "vpc14.p" , 7340);
    ptoc_Var46 = &ptoc_Var42->pccmdpart;
    if ((int ) ptoc_Var42->pcerror == 0)
        {
        (*next_symbol)(&(*apc) , sql__nil(ptoc_Var46->partbufp , "vpc14.p" , 7345) , ptoc_Var46->part1len);
        p10getkeyword(&(*apc) , sql__nil(ptoc_Var46->partbufp , "vpc14.p" , 7346) , &keyind);
        if (keyind == 31)
            {
            (*next_symbol)(&(*apc) , sql__nil(ptoc_Var46->partbufp , "vpc14.p" , 7350) , ptoc_Var46->part1len);
            p10getkeyword(&(*apc) , sql__nil(ptoc_Var46->partbufp , "vpc14.p" , 7351) , &keyind);
            switch ((int ) ptoc_Var45->ralang)
                {
                case 11 :
                case 10 :
                case 12 :
                case 9 :
                    if ((int ) ptoc_Var42->pcan.ansection != 0)
                        {
                        p11wrerrorlisting(&(*apc) , 36);
                        }
                    else
                        {
                        switch (keyind)
                            {
                            case 20 :
                                ptoc_Var42->pcan.ansection = 1;
                                (*next_symbol)(&(*apc) , sql__nil(ptoc_Var46->partbufp , "vpc14.p" , 7363) , ptoc_Var46->part1len);
                                p10getkeyword(&(*apc) , sql__nil(ptoc_Var46->partbufp , "vpc14.p" , 7364) , &keyind);
                                if (keyind != 93)
                                    {
                                    p11wrerrorlisting(&(*apc) , 81);
                                    }
                                break;
                            case 129 :
                                ptoc_Var42->pcan.ansection = 2;
                                (*next_symbol)(&(*apc) , sql__nil(ptoc_Var46->partbufp , "vpc14.p" , 7373) , ptoc_Var46->part1len);
                                p10getkeyword(&(*apc) , sql__nil(ptoc_Var46->partbufp , "vpc14.p" , 7374) , &keyind);
                                if (keyind != 93)
                                    {
                                    p11wrerrorlisting(&(*apc) , 81);
                                    }
                                break;
                            case 93 :
                                ptoc_Var42->pcan.ansection = 3;
                                break;
                            default :
                                p11wrerrorlisting(&(*apc) , 81);
                                break;
                            }
                        }
                    break;
                case 3 :
                case 4 :
                case 7 :
                case 8 :
                    if (keyind == 93)
                        {
                        ptoc_Var42->pcan.ansection = 3;
                        }
                    else
                        {
                        p11wrerrorlisting(&(*apc) , 81);
                        }
                    break;
                default :
                    p11wrerrorlisting(&(*apc) , 60);
                    break;
                }
            if ((int ) ptoc_Var42->pcerror == 0)
                {
                (*next_symbol)(&(*apc) , sql__nil(ptoc_Var46->partbufp , "vpc14.p" , 7401) , ptoc_Var46->part1len);
                if ((int ) ptoc_Var43->symb != 4 && (int ) ptoc_Var43->symb != 23)
                    {
                    p11wrerrorlisting(&(*apc) , 37);
                    }
                }
            }
        else
            {
            p11wrerrorlisting(&(*apc) , 81);
            }
        }
    if ((int ) ptoc_Var42->pcan.ansection != 4)
        {
        while ((int ) ptoc_Var43->symb != 4 && (int ) ptoc_Var43->symb != 23)
            (*next_symbol)(&(*apc) , sql__nil(ptoc_Var46->partbufp , "vpc14.p" , 7417) , ptoc_Var46->part1len);
        }
    m90name(22 , "END p14begin      ");
    }



int p14command(next_symbol, apc)
int (*next_symbol)();
tpc_globals *apc;
    {
    tpc_pre_errors err;
    tsp_int2 ptypentix;
    tsp_int2 pvatyp;
    int keyind;
    int pos;
    int len;
    tsp_int2 retpos;
    tsp_int2 lastvatyp;
    int vix;
    tpc_globals *ptoc_Var47;
    sqlxatype *ptoc_Var48;
    tpr_scannertype *ptoc_Var49;
    sqlcatype *ptoc_Var50;
    sqlratype *ptoc_Var51;
    tpc_cmd_part *ptoc_Var52;
    tpc_pc_sqlva *ptoc_Var53;
    int ptoc_Var54;
    sqlkaentry *ptoc_Var55;
    sqlxatype *ptoc_Var56;
    sqlprentry *ptoc_Var57;
    struct ptoc_Type75 *ptoc_Var58;
    struct ptoc_Type75 *ptoc_Var59;
    struct ptoc_Type47 *ptoc_Var60;
    struct ptoc_Type48 *ptoc_Var61;
    m90print("PC14  p14command");
    ptoc_Var47 = &*apc;
    ptoc_Var48 = &ptoc_Var47->sqlxa;
    ptoc_Var49 = &ptoc_Var47->pcscan;
    ptoc_Var50 = &ptoc_Var47->sqlca;
    ptoc_Var51 = (sqlratype *) sql__nil(ptoc_Var50->sqlrap , "vpc14.p" , 7445);
    ptoc_Var52 = &ptoc_Var47->pccmdpart;
    ptoc_Var53 = &ptoc_Var47->pcsqlva;
    retpos = (short ) sql__setrang((short ) ptoc_Var49->sypos , -32768 , 32767);
    ptoc_Var47->pcan.ancheck = (short ) sql__setrang(ptoc_Var47->pcopts.tsp4_component.C_sp4co_sql_precompiler.opt_mode , -32768 , 32767);
    ptoc_Var47->pcusage.uscom = 1;
    _ptoc_ASSIGN(unsigned char , 64, ptoc_Var47->pcusage.usdobjn1, "                                                                ")
    _ptoc_ASSIGN(unsigned char , 64, ptoc_Var47->pcusage.usdobjn2, "                                                                ")
    if ((ptoc_Var54 = (int ) ptoc_Var51->ralang) == 3 || ptoc_Var54 == 4)
        {
        while ((int ) ptoc_Var49->symb != 4 && (int ) ptoc_Var49->symb != 23)
            (*next_symbol)(&(*apc) , sql__nil(ptoc_Var52->partbufp , "vpc14.p" , 7455) , ptoc_Var52->part1len);
        }
    if ((int ) ptoc_Var51->ralang == 4)
        {
        ptoc_Var52->part1len = ptoc_Var52->part1len + 1;
        (*(tpc_partbuffer *) sql__nil(ptoc_Var52->partbufp , "vpc14.p" , 7462))[sql__setrang(ptoc_Var52->part1len , 1 , 16000) + -1] = (unsigned char)'\0';
        }
    ptoc_Var49->syposacc = (int ) retpos;
    ptoc_Var49->symb = 0;
    err = 0;
    p14kaentryinit(&(*apc));
    ptoc_Var55 = &(*(sqlkatype *) sql__nil(ptoc_Var48->sqlkap , "vpc14.p" , 7469))[sql__setrang((int ) ptoc_Var47->pcka , 1 , 3000) + -1];
    p19sqlprallocate(&ptoc_Var47->pcpr);
    ptoc_Var56 = &ptoc_Var47->sqlxa;
    ptoc_Var57 = &(*(sqlprtype *) sql__nil(ptoc_Var56->sqlprp , "vpc14.p" , 7472))[sql__setrang((int ) ptoc_Var47->pcpr , 1 , 500) + -1];
    ptoc_Var55->kaprindex = (short ) sql__setrang(ptoc_Var47->pcpr , -32768 , 32767);
    ptoc_Var57->prstcount = 0;
    ptoc_Var57->prstindex = 0;
    ptoc_Var57->prcuindex = 0;
    ptoc_Var57->prstate = 7;
    ptoc_Var57->prarea = 0;
    ptoc_Var57->prkaindex = 0;
    ptoc_Var57->prDescribe = 0;
    _ptoc_ASSIGN(unsigned char , 64, ptoc_Var57->prStmtName, "                                                                ")
    ptoc_Var57->prStmtNameIndex = 0;
    ptoc_Var57->prCursorNameIndex = 0;
    _ptoc_ASSIGN(unsigned char , 64, ptoc_Var57->prCursorName, "                                                                ")
    (*next_symbol)(&(*apc) , sql__nil(ptoc_Var52->partbufp , "vpc14.p" , 7486) , ptoc_Var52->part1len);
    if ((int ) ptoc_Var49->symb == 8)
        {
        p10getkeyword(&(*apc) , sql__nil(ptoc_Var52->partbufp , "vpc14.p" , 7490) , &keyind);
        switch (keyind)
            {
            case 121 :
                ptoc_Var57->prstate = 7;
                break;
            case 68 :
                ptoc_Var57->prstate = 8;
                break;
            case 6 :
                ptoc_Var57->prstate = 9;
                break;
            default :
                p11precomerror(&(*apc) , 37);
                break;
            }
        (*next_symbol)(&(*apc) , sql__nil(ptoc_Var52->partbufp , "vpc14.p" , 7502) , ptoc_Var52->part1len);
        }
    if ((int ) ptoc_Var49->symb == 13)
        {
        ptoc_Var49->syposacc = ptoc_Var49->sypos + 1;
        ptypentix = 0;
        p14btypentget(next_symbol , &(*apc) , &ptypentix , &pvatyp , &lastvatyp , 1);
        if ((int ) ptypentix == 0)
            {
            p11precomerror(&(*apc) , 79);
            }
        else
            {
            ptoc_Var55->kapacount = 0;
            ptoc_Var55->kapaindex = (short ) sql__setrang((short ) ((int ) ptoc_Var47->pcpa + 1) , -32768 , 32767);
            ptoc_Var49->syposacc = ptoc_Var49->sypos;
            ptoc_Var58 = &(*(tpc_arrndtype *) sql__nil(ptoc_Var53->ndtabp , "vpc14.p" , 7520))[sql__setrang((int ) ptoc_Var53->ndcnt , 1 , 100) + -1];
            if (ptoc_Var58->ndsqlva1st != ptoc_Var58->ndsqlva1ix)
                {
                p19cparentry(&(*apc) , 1 , ptoc_Var58->ndsqlva1st , 0 , ptoc_Var58->ndsqlva1cnt);
                ptoc_Var55->kapacount = 1;
                }
            p19cparentry(&(*apc) , 2 , ptoc_Var58->ndsqlva1ix , 0 , 0);
            ptoc_Var55->kapacount = (short ) sql__setrang((short ) ((int ) ptoc_Var55->kapacount + 1) , -32768 , 32767);
            }
        }
    else
        {
        if ((int ) ptoc_Var49->symb == 21 || (int ) ptoc_Var49->symb == 24)
            {
            p10stringliteralget(&(*apc) , sql__nil(ptoc_Var52->partbufp , "vpc14.p" , 7544));
            ptoc_Var55->kaStindex = (short ) sql__setrang((short ) ((int ) ptoc_Var47->pcst + 1) , -32768 , 32767);
            ptoc_Var57->prarea = 1;
            pos = ptoc_Var49->sypos;
            while (pos < ptoc_Var49->sypos + ptoc_Var49->sylen)
                {
                if (pos + 52 > ptoc_Var49->sypos + ptoc_Var49->sylen)
                    {
                    len = ptoc_Var49->sypos + ptoc_Var49->sylen - pos;
                    }
                else
                    {
                    len = 52;
                    }
                p19sqlstallocate(&ptoc_Var47->pcst);
                if ((int ) ptoc_Var47->pcst > (int ) ptoc_Var47->pcstmax)
                    {
                    ptoc_Var47->pcstmax = (short ) sql__setrang(ptoc_Var47->pcst , -32768 , 32767);
                    }
                ptoc_Var55->kaStcount = (short ) sql__setrang((short ) ((int ) ptoc_Var55->kaStcount + 1) , -32768 , 32767);
                s10mv1(ptoc_Var52->partsizlen , 52 , sql__nil(ptoc_Var52->partbufp , "vpc14.p" , 7564) , pos , (*(sqlsttype *) sql__nil(ptoc_Var56->sqlstp , "vpc14.p" , 7565))[sql__setrang((int ) ptoc_Var47->pcst , 1 , 1000) + -1].stline , 1 , len);
                (*(sqlsttype *) sql__nil(ptoc_Var56->sqlstp , "vpc14.p" , 7566))[sql__setrang((int ) ptoc_Var47->pcst , 1 , 1000) + -1].stllen = len;
                (*(sqlsttype *) sql__nil(ptoc_Var56->sqlstp , "vpc14.p" , 7567))[sql__setrang((int ) ptoc_Var47->pcst , 1 , 1000) + -1].stkano = (int ) ptoc_Var47->pcka;
                pos = pos + len;
                }
            }
        else
            {
            p11precomerror(&(*apc) , 37);
            }
        }
    if ((int ) ptoc_Var49->symb != 4 && (int ) ptoc_Var49->symb != 23)
        {
        (*next_symbol)(&(*apc) , sql__nil(ptoc_Var52->partbufp , "vpc14.p" , 7578) , ptoc_Var52->part1len);
        }
    if ((int ) ptoc_Var57->prstate != 9)
        {
        p10getkeyword(&(*apc) , sql__nil(ptoc_Var52->partbufp , "vpc14.p" , 7584) , &keyind);
        if (keyind != 90)
            {
            p11precomerror(&(*apc) , 37);
            }
        else
            {
            (*next_symbol)(&(*apc) , sql__nil(ptoc_Var52->partbufp , "vpc14.p" , 7590) , ptoc_Var52->part1len);
            if ((int ) ptoc_Var49->symb != 13)
                {
                p11precomerror(&(*apc) , 37);
                }
            else
                {
                ptoc_Var49->syposacc = ptoc_Var49->sypos + 1;
                ptypentix = 0;
                p14btypentget(next_symbol , &(*apc) , &ptypentix , &pvatyp , &lastvatyp , 1);
                if ((int ) ptypentix == 0)
                    {
                    p11precomerror(&(*apc) , 79);
                    }
                else
                    {
                    ptoc_Var57->prkaindex = (short ) sql__setrang((short ) ((int ) ptoc_Var47->pcpa + 1) , -32768 , 32767);
                    ptoc_Var59 = &(*(tpc_arrndtype *) sql__nil(ptoc_Var53->ndtabp , "vpc14.p" , 7606))[sql__setrang((int ) ptoc_Var53->ndcnt , 1 , 100) + -1];
                    if (ptoc_Var59->ndsqlva1st != ptoc_Var59->ndsqlva1ix)
                        {
                        p19cparentry(&(*apc) , 1 , ptoc_Var59->ndsqlva1st , 0 , ptoc_Var59->ndsqlva1cnt);
                        }
                    p19cparentry(&(*apc) , 2 , ptoc_Var59->ndsqlva1ix , 0 , 0);
                    vix = ptoc_Var59->ndsqlva1ix;
                    if ((int ) (*(sqlva1type *) sql__nil(ptoc_Var56->sqlv1p , "vpc14.p" , 7618))[sql__setrang(vix , 1 , 10000) + -1].tsp_int2.C_sqlvasc.va1indi_sc == 3)
                        {
                        vix = (*(sqlva1type *) sql__nil(ptoc_Var56->sqlv1p , "vpc14.p" , 7620))[sql__setrang(vix , 1 , 10000) + -1].tsp_int2.C_sqlvapt.va1ix_pt;
                        }
                    ptoc_Var60 = &(*(sqlva1type *) sql__nil(ptoc_Var56->sqlv1p , "vpc14.p" , 7624))[sql__setrang(vix , 1 , 10000) + -1];
                    ptoc_Var61 = &(*(sqlva2type *) sql__nil(ptoc_Var56->sqlv2p , "vpc14.p" , 7624))[sql__setrang((int ) ptoc_Var60->tsp_int2.C_sqlvasc.va1indva2_sc , 1 , 5000) + -1];
                    switch ((int ) ptoc_Var61->va2typ)
                        {
                        case 0 :
                        case 1 :
                            break;
                        default :
                            p11precomerror(&(*apc) , 5);
                            break;
                        }
                    }
                if ((int ) ptoc_Var49->symb != 4 && (int ) ptoc_Var49->symb != 23)
                    {
                    (*next_symbol)(&(*apc) , sql__nil(ptoc_Var52->partbufp , "vpc14.p" , 7642) , ptoc_Var52->part1len);
                    }
                }
            }
        }
    if ((int ) ptoc_Var49->symb != 4 && (int ) ptoc_Var49->symb != 23)
        {
        p11precomerror(&(*apc) , 37);
        }
    if ((int ) err != 0)
        {
        p11precomerror(&(*apc) , (unsigned char ) sql__setrang(err , 0 , 90));
        }
    ptoc_Var49->symb = 4;
    m90name(22 , "END p14command    ");
    }



int p14cancel(next_symbol, apc)
int (*next_symbol)();
tpc_globals *apc;
    {
    tpc_pre_errors err;
    tsp_int2 ptypentix;
    tsp_int2 pvatyp;
    int keyind;
    tsp_int2 retpos;
    tsp_int2 lastvatyp;
    tsp_int2 sessno;
    tpc_globals *ptoc_Var62;
    sqlxatype *ptoc_Var63;
    tpr_scannertype *ptoc_Var64;
    sqlcatype *ptoc_Var65;
    sqlratype *ptoc_Var66;
    tpc_cmd_part *ptoc_Var67;
    tpc_pc_sqlva *ptoc_Var68;
    int ptoc_Var69;
    sqlkaentry *ptoc_Var70;
    tpc_pc_sqlva *ptoc_Var71;
    tpc_symtab *ptoc_Var72;
    struct ptoc_Type75 *ptoc_Var73;
    m90print("PC14  p14cancel ");
    ptoc_Var62 = &*apc;
    ptoc_Var63 = &ptoc_Var62->sqlxa;
    ptoc_Var64 = &ptoc_Var62->pcscan;
    ptoc_Var65 = &ptoc_Var62->sqlca;
    ptoc_Var66 = (sqlratype *) sql__nil(ptoc_Var65->sqlrap , "vpc14.p" , 7687);
    ptoc_Var67 = &ptoc_Var62->pccmdpart;
    ptoc_Var68 = &ptoc_Var62->pcsqlva;
    retpos = (short ) sql__setrang((short ) ptoc_Var64->syposacc , -32768 , 32767);
    ptoc_Var62->pcan.ancheck = (short ) sql__setrang(ptoc_Var62->pcopts.tsp4_component.C_sp4co_sql_precompiler.opt_mode , -32768 , 32767);
    ptoc_Var62->pcan.ancomtyp = 3;
    ptoc_Var62->pcusage.uscom = 3;
    _ptoc_ASSIGN(unsigned char , 64, ptoc_Var62->pcusage.usdobjn1, "                                                                ")
    _ptoc_ASSIGN(unsigned char , 64, ptoc_Var62->pcusage.usdobjn2, "                                                                ")
    if ((ptoc_Var69 = (int ) ptoc_Var66->ralang) == 3 || ptoc_Var69 == 4)
        {
        while ((int ) ptoc_Var64->symb != 4 && (int ) ptoc_Var64->symb != 23)
            (*next_symbol)(&(*apc) , sql__nil(ptoc_Var67->partbufp , "vpc14.p" , 7698) , ptoc_Var67->part1len);
        }
    if ((int ) ptoc_Var66->ralang == 4)
        {
        ptoc_Var67->part1len = ptoc_Var67->part1len + 1;
        (*(tpc_partbuffer *) sql__nil(ptoc_Var67->partbufp , "vpc14.p" , 7705))[sql__setrang(ptoc_Var67->part1len , 1 , 16000) + -1] = (unsigned char)'\0';
        }
    ptoc_Var64->syposacc = (int ) retpos;
    ptoc_Var64->symb = 0;
    err = 0;
    p14kaentryinit(&(*apc));
    ptoc_Var70 = &(*(sqlkatype *) sql__nil(ptoc_Var63->sqlkap , "vpc14.p" , 7712))[sql__setrang((int ) ptoc_Var62->pcka , 1 , 3000) + -1];
    (*next_symbol)(&(*apc) , sql__nil(ptoc_Var67->partbufp , "vpc14.p" , 7714) , ptoc_Var67->part1len);
    switch ((int ) ptoc_Var64->symb)
        {
        case 8 :
            p10getkeyword(&(*apc) , sql__nil(ptoc_Var67->partbufp , "vpc14.p" , 7718) , &keyind);
            if (keyind == 124)
                {
                ptoc_Var70->kapacount = 1;
                }
            else
                {
                p11wrerrorlisting(&(*apc) , 81);
                }
            (*next_symbol)(&(*apc) , sql__nil(ptoc_Var67->partbufp , "vpc14.p" , 7726) , ptoc_Var67->part1len);
            break;
        case 13 :
            ptoc_Var71 = &ptoc_Var62->pcsqlva;
            ptoc_Var72 = &ptoc_Var62->pcsymtab;
            ptoc_Var64->syposacc = ptoc_Var64->sypos + 1;
            ptypentix = 0;
            p14btypentget(next_symbol , &(*apc) , &ptypentix , &pvatyp , &lastvatyp , 1);
            if ((int ) ptypentix == 0)
                {
                if ((int ) ptoc_Var62->pcerror == 0)
                    {
                    p11precomerror(&(*apc) , 79);
                    p11nerrorlisting(&(*apc) , (*(tpc_vartable *) sql__nil(ptoc_Var72->vartablep , "vpc14.p" , 7744))[sql__setrang((int ) (*(tpc_arrndtype *) sql__nil(ptoc_Var71->ndtabp , "vpc14.p" , 7744))[sql__setrang((int ) ptoc_Var71->ndcnt , 1 , 100) + -1].ndvarentix , 1 , 1000) + -1].vaname , (short ) sql__setrang((*(tpc_vartable *) sql__nil(ptoc_Var72->vartablep , "vpc14.p" , 7745))[sql__setrang((int ) (*(tpc_arrndtype *) sql__nil(ptoc_Var71->ndtabp , "vpc14.p" , 7745))[sql__setrang((int ) ptoc_Var71->ndcnt , 1 , 100) + -1].ndvarentix , 1 , 1000) + -1].vanaml , -32768 , 32767) , 0);
                    }
                }
            else
                {
                if ((ptoc_Var69 = (int ) pvatyp) != 0 && ptoc_Var69 != 1 && ptoc_Var69 != 16 && ptoc_Var69 != 17)
                    {
                    p11precomerror(&(*apc) , 5);
                    p11nerrorlisting(&(*apc) , (*(tpc_vartable *) sql__nil(ptoc_Var72->vartablep , "vpc14.p" , 7756))[sql__setrang((int ) (*(tpc_arrndtype *) sql__nil(ptoc_Var71->ndtabp , "vpc14.p" , 7756))[sql__setrang((int ) ptoc_Var71->ndcnt , 1 , 100) + -1].ndvarentix , 1 , 1000) + -1].vaname , (short ) sql__setrang((*(tpc_vartable *) sql__nil(ptoc_Var72->vartablep , "vpc14.p" , 7757))[sql__setrang((int ) (*(tpc_arrndtype *) sql__nil(ptoc_Var71->ndtabp , "vpc14.p" , 7757))[sql__setrang((int ) ptoc_Var71->ndcnt , 1 , 100) + -1].ndvarentix , 1 , 1000) + -1].vanaml , -32768 , 32767) , 0);
                    }
                else
                    {
                    ptoc_Var70->kapaindex = (short ) sql__setrang((short ) ((int ) ptoc_Var62->pcpa + 1) , -32768 , 32767);
                    ptoc_Var73 = &(*(tpc_arrndtype *) sql__nil(ptoc_Var71->ndtabp , "vpc14.p" , 7763))[sql__setrang((int ) ptoc_Var71->ndcnt , 1 , 100) + -1];
                    if (ptoc_Var73->ndsqlva1st != ptoc_Var73->ndsqlva1ix)
                        {
                        p19cparentry(&(*apc) , 1 , ptoc_Var73->ndsqlva1st , 0 , ptoc_Var73->ndsqlva1cnt);
                        ptoc_Var70->kapacount = 1;
                        }
                    p19cparentry(&(*apc) , 2 , ptoc_Var73->ndsqlva1ix , 0 , 0);
                    ptoc_Var70->kapacount = (short ) sql__setrang((short ) ((int ) ptoc_Var70->kapacount + 1) , -32768 , 32767);
                    }
                }
            break;
        case 22 :
            p10int2unsignedget(&(*apc) , sql__nil(ptoc_Var67->partbufp , "vpc14.p" , 7785) , &sessno);
            if ((int ) sessno < 1 || (int ) sessno > 8)
                {
                p11wrerrorlisting(&(*apc) , 64);
                }
            else
                {
                ptoc_Var70->kapacount = 1;
                ptoc_Var70->kapaindex = (short ) sql__setrang((short ) -(int ) sessno , -32768 , 32767);
                }
            (*next_symbol)(&(*apc) , sql__nil(ptoc_Var67->partbufp , "vpc14.p" , 7797) , ptoc_Var67->part1len);
            break;
        default :
            p11wrerrorlisting(&(*apc) , 81);
            break;
        }
    if ((int ) ptoc_Var64->symb != 4 && (int ) ptoc_Var64->symb != 23)
        {
        p11wrerrorlisting(&(*apc) , 81);
        }
    ptoc_Var64->symb = 4;
    }



int p14abortsession(next_symbol, apc)
int (*next_symbol)();
tpc_globals *apc;
    {
    char nextsa;
    tpc_pre_errors err;
    int pos;
    int len;
    int retpos;
    int keyind;
    tpc_globals *ptoc_Var74;
    tpr_scannertype *ptoc_Var75;
    sqlcatype *ptoc_Var76;
    sqlratype *ptoc_Var77;
    tpc_cmd_part *ptoc_Var78;
    int ptoc_Var79;
    m90print("PC14  p14abortse");
    ptoc_Var74 = &*apc;
    ptoc_Var75 = &ptoc_Var74->pcscan;
    ptoc_Var76 = &ptoc_Var74->sqlca;
    ptoc_Var77 = (sqlratype *) sql__nil(ptoc_Var76->sqlrap , "vpc14.p" , 7832);
    ptoc_Var78 = &ptoc_Var74->pccmdpart;
    retpos = ptoc_Var75->syposacc;
    if ((ptoc_Var79 = (int ) ptoc_Var77->ralang) == 3)
        {
        while ((int ) ptoc_Var75->symb != 4 && (int ) ptoc_Var75->symb != 23)
            (*next_symbol)(&(*apc) , sql__nil(ptoc_Var78->partbufp , "vpc14.p" , 7838) , ptoc_Var78->part1len);
        }
    ptoc_Var75->syposacc = retpos;
    ptoc_Var75->symb = 0;
    err = 0;
    (*next_symbol)(&(*apc) , sql__nil(ptoc_Var78->partbufp , "vpc14.p" , 7844) , ptoc_Var78->part1len);
    p10getkeyword(&(*apc) , sql__nil(ptoc_Var78->partbufp , "vpc14.p" , 7845) , &keyind);
    if (keyind == 97)
        {
        ptoc_Var74->pcan.ancomtyp = 31;
        p14kaentryinit(&(*apc));
        }
    else
        {
        p11wrerrorlisting(&(*apc) , 81);
        }
    m90name(22 , "END p14abortsessio");
    }



int p14version(next_symbol, apc)
int (*next_symbol)();
tpc_globals *apc;
    {
    int i;
    tpc_pre_errors err;
    tsp_int2 ptypentix;
    tsp_int2 pvatyp;
    tsp_int2 retpos;
    tsp_int2 lastvatyp;
    tpc_globals *ptoc_Var80;
    sqlxatype *ptoc_Var81;
    tpr_scannertype *ptoc_Var82;
    sqlcatype *ptoc_Var83;
    sqlratype *ptoc_Var84;
    tpc_cmd_part *ptoc_Var85;
    tpc_pc_sqlva *ptoc_Var86;
    int ptoc_Var87;
    sqlkaentry *ptoc_Var88;
    tpc_pc_sqlva *ptoc_Var89;
    tpc_symtab *ptoc_Var90;
    struct ptoc_Type75 *ptoc_Var91;
    m90print("PC14  p14version");
    ptoc_Var80 = &*apc;
    ptoc_Var81 = &ptoc_Var80->sqlxa;
    ptoc_Var82 = &ptoc_Var80->pcscan;
    ptoc_Var83 = &ptoc_Var80->sqlca;
    ptoc_Var84 = (sqlratype *) sql__nil(ptoc_Var83->sqlrap , "vpc14.p" , 7877);
    ptoc_Var85 = &ptoc_Var80->pccmdpart;
    ptoc_Var86 = &ptoc_Var80->pcsqlva;
    retpos = (short ) sql__setrang((short ) ptoc_Var82->syposacc , -32768 , 32767);
    ptoc_Var80->pcan.ancheck = (short ) sql__setrang(ptoc_Var80->pcopts.tsp4_component.C_sp4co_sql_precompiler.opt_mode , -32768 , 32767);
    ptoc_Var80->pcan.ancomtyp = 3;
    ptoc_Var80->pcusage.uscom = 3;
    _ptoc_ASSIGN(unsigned char , 64, ptoc_Var80->pcusage.usdobjn1, "                                                                ")
    _ptoc_ASSIGN(unsigned char , 64, ptoc_Var80->pcusage.usdobjn2, "                                                                ")
    if ((ptoc_Var87 = (int ) ptoc_Var84->ralang) == 3 || ptoc_Var87 == 4)
        {
        while ((int ) ptoc_Var82->symb != 4 && (int ) ptoc_Var82->symb != 23)
            (*next_symbol)(&(*apc) , sql__nil(ptoc_Var85->partbufp , "vpc14.p" , 7888) , ptoc_Var85->part1len);
        }
    if ((int ) ptoc_Var84->ralang == 4)
        {
        ptoc_Var85->part1len = ptoc_Var85->part1len + 1;
        (*(tpc_partbuffer *) sql__nil(ptoc_Var85->partbufp , "vpc14.p" , 7895))[sql__setrang(ptoc_Var85->part1len , 1 , 16000) + -1] = (unsigned char)'\0';
        }
    ptoc_Var82->syposacc = (int ) retpos;
    ptoc_Var82->symb = 0;
    err = 0;
    p14kaentryinit(&(*apc));
    ptoc_Var88 = &(*(sqlkatype *) sql__nil(ptoc_Var81->sqlkap , "vpc14.p" , 7902))[sql__setrang((int ) ptoc_Var80->pcka , 1 , 3000) + -1];
    ptoc_Var88->kapaindex = (short ) sql__setrang((short ) ((int ) ptoc_Var80->pcpa + 1) , -32768 , 32767);
    i = 1;
    do
        {
        (*next_symbol)(&(*apc) , sql__nil(ptoc_Var85->partbufp , "vpc14.p" , 7907) , ptoc_Var85->part1len);
        switch ((int ) ptoc_Var82->symb)
            {
            case 13 :
                ptoc_Var89 = &ptoc_Var80->pcsqlva;
                ptoc_Var90 = &ptoc_Var80->pcsymtab;
                ptoc_Var82->syposacc = ptoc_Var82->sypos + 1;
                ptypentix = 0;
                p14btypentget(next_symbol , &(*apc) , &ptypentix , &pvatyp , &lastvatyp , 1);
                if ((int ) ptypentix == 0)
                    {
                    if ((int ) ptoc_Var80->pcerror == 0)
                        {
                        p11precomerror(&(*apc) , 79);
                        p11nerrorlisting(&(*apc) , (*(tpc_vartable *) sql__nil(ptoc_Var90->vartablep , "vpc14.p" , 7925))[sql__setrang((int ) (*(tpc_arrndtype *) sql__nil(ptoc_Var89->ndtabp , "vpc14.p" , 7925))[sql__setrang((int ) ptoc_Var89->ndcnt , 1 , 100) + -1].ndvarentix , 1 , 1000) + -1].vaname , (short ) sql__setrang((*(tpc_vartable *) sql__nil(ptoc_Var90->vartablep , "vpc14.p" , 7927))[sql__setrang((int ) (*(tpc_arrndtype *) sql__nil(ptoc_Var89->ndtabp , "vpc14.p" , 7927))[sql__setrang((int ) ptoc_Var89->ndcnt , 1 , 100) + -1].ndvarentix , 1 , 1000) + -1].vanaml , -32768 , 32767) , 0);
                        }
                    }
                else
                    {
                    if ((ptoc_Var87 = (int ) pvatyp) != 6 && ptoc_Var87 != 7 && ptoc_Var87 != 10 && ptoc_Var87 != 15 && ptoc_Var87 != 35)
                        {
                        p11precomerror(&(*apc) , 5);
                        p11nerrorlisting(&(*apc) , (*(tpc_vartable *) sql__nil(ptoc_Var90->vartablep , "vpc14.p" , 7940))[sql__setrang((int ) (*(tpc_arrndtype *) sql__nil(ptoc_Var89->ndtabp , "vpc14.p" , 7940))[sql__setrang((int ) ptoc_Var89->ndcnt , 1 , 100) + -1].ndvarentix , 1 , 1000) + -1].vaname , (short ) sql__setrang((*(tpc_vartable *) sql__nil(ptoc_Var90->vartablep , "vpc14.p" , 7942))[sql__setrang((int ) (*(tpc_arrndtype *) sql__nil(ptoc_Var89->ndtabp , "vpc14.p" , 7942))[sql__setrang((int ) ptoc_Var89->ndcnt , 1 , 100) + -1].ndvarentix , 1 , 1000) + -1].vanaml , -32768 , 32767) , 0);
                        }
                    else
                        {
                        ptoc_Var91 = &(*(tpc_arrndtype *) sql__nil(ptoc_Var89->ndtabp , "vpc14.p" , 7948))[sql__setrang((int ) ptoc_Var89->ndcnt , 1 , 100) + -1];
                        if (ptoc_Var91->ndsqlva1st != ptoc_Var91->ndsqlva1ix)
                            {
                            p19cparentry(&(*apc) , 1 , ptoc_Var91->ndsqlva1st , 0 , ptoc_Var91->ndsqlva1cnt);
                            ptoc_Var88->kapacount = (short ) sql__setrang((short ) ((int ) ptoc_Var88->kapacount + 1) , -32768 , 32767);
                            }
                        p19cparentry(&(*apc) , 2 , ptoc_Var91->ndsqlva1ix , 0 , 0);
                        ptoc_Var88->kapacount = (short ) sql__setrang((short ) ((int ) ptoc_Var88->kapacount + 1) , -32768 , 32767);
                        }
                    }
                break;
            default :
                p11wrerrorlisting(&(*apc) , 81);
                break;
            }
        if (i == 2)
            {
            break;
            }
        i += 1;
        }
    while (!0);
    if ((int ) ptoc_Var82->symb != 4 && (int ) ptoc_Var82->symb != 23)
        {
        p11wrerrorlisting(&(*apc) , 81);
        }
    ptoc_Var82->symb = 4;
    }



int p14_hostv_get(next_symbol, apc, paix, va1ix)
int (*next_symbol)();
tpc_globals *apc;
tsp_int2 *paix;
tsp_int2 *va1ix;
    {
    tsp_int2 ptypentix;
    tsp_int2 pvatyp;
    tsp_int2 lastvatyp;
    tpc_globals *ptoc_Var92;
    sqlxatype *ptoc_Var93;
    tpr_scannertype *ptoc_Var94;
    sqlcatype *ptoc_Var95;
    sqlprentry *ptoc_Var96;
    sqlkaentry *ptoc_Var97;
    tpc_pc_sqlva *ptoc_Var98;
    tpc_symtab *ptoc_Var99;
    int ptoc_Var100;
    struct ptoc_Type75 *ptoc_Var101;
    m90print("PC14  p14_hostv_");
    ptoc_Var92 = &*apc;
    ptoc_Var93 = &ptoc_Var92->sqlxa;
    ptoc_Var94 = &ptoc_Var92->pcscan;
    ptoc_Var95 = &ptoc_Var92->sqlca;
    m90int(14 , "pcpr        " , (int ) ptoc_Var92->pcpr);
    ptoc_Var96 = &(*(sqlprtype *) sql__nil(ptoc_Var93->sqlprp , "vpc14.p" , 8003))[sql__setrang((int ) ptoc_Var92->pcpr , 1 , 500) + -1];
    ptoc_Var97 = &(*(sqlkatype *) sql__nil(ptoc_Var93->sqlkap , "vpc14.p" , 8003))[sql__setrang((int ) ptoc_Var92->pcka , 1 , 3000) + -1];
    ptoc_Var98 = &ptoc_Var92->pcsqlva;
    ptoc_Var99 = &ptoc_Var92->pcsymtab;
    *va1ix = 0;
    ptoc_Var94->syposacc = ptoc_Var94->sypos + 1;
    ptypentix = 0;
    p14btypentget(next_symbol , &(*apc) , &ptypentix , &pvatyp , &lastvatyp , 1);
    if ((int ) ptypentix == 0)
        {
        if ((int ) ptoc_Var92->pcerror == 0)
            {
            p11precomerror(&(*apc) , 79);
            p11nerrorlisting(&(*apc) , (*(tpc_vartable *) sql__nil(ptoc_Var99->vartablep , "vpc14.p" , 8020))[sql__setrang((int ) (*(tpc_arrndtype *) sql__nil(ptoc_Var98->ndtabp , "vpc14.p" , 8020))[sql__setrang((int ) ptoc_Var98->ndcnt , 1 , 100) + -1].ndvarentix , 1 , 1000) + -1].vaname , (short ) sql__setrang((*(tpc_vartable *) sql__nil(ptoc_Var99->vartablep , "vpc14.p" , 8021))[sql__setrang((int ) (*(tpc_arrndtype *) sql__nil(ptoc_Var98->ndtabp , "vpc14.p" , 8021))[sql__setrang((int ) ptoc_Var98->ndcnt , 1 , 100) + -1].ndvarentix , 1 , 1000) + -1].vanaml , -32768 , 32767) , 0);
            }
        }
    else
        {
        if (((ptoc_Var100 = (int ) pvatyp) < 6 || ptoc_Var100 > 10) && ptoc_Var100 != 15 && ptoc_Var100 != 20 && ptoc_Var100 != 52 && ptoc_Var100 != 53 && ptoc_Var100 != 41 && ptoc_Var100 != 42)
            {
            p11precomerror(&(*apc) , 5);
            p11nerrorlisting(&(*apc) , (*(tpc_vartable *) sql__nil(ptoc_Var99->vartablep , "vpc14.p" , 8032))[sql__setrang((int ) (*(tpc_arrndtype *) sql__nil(ptoc_Var98->ndtabp , "vpc14.p" , 8032))[sql__setrang((int ) ptoc_Var98->ndcnt , 1 , 100) + -1].ndvarentix , 1 , 1000) + -1].vaname , (short ) sql__setrang((*(tpc_vartable *) sql__nil(ptoc_Var99->vartablep , "vpc14.p" , 8033))[sql__setrang((int ) (*(tpc_arrndtype *) sql__nil(ptoc_Var98->ndtabp , "vpc14.p" , 8033))[sql__setrang((int ) ptoc_Var98->ndcnt , 1 , 100) + -1].ndvarentix , 1 , 1000) + -1].vanaml , -32768 , 32767) , 0);
            }
        }
    if ((int ) ptoc_Var92->pcerror == 0)
        {
        *paix = (short ) sql__setrang((short ) ((int ) ptoc_Var92->pcpa + 1) , -32768 , 32767);
        ptoc_Var101 = &(*(tpc_arrndtype *) sql__nil(ptoc_Var98->ndtabp , "vpc14.p" , 8041))[sql__setrang((int ) ptoc_Var98->ndcnt , 1 , 100) + -1];
        if (ptoc_Var101->ndsqlva1st != ptoc_Var101->ndsqlva1ix)
            {
            p19cparentry(&(*apc) , 1 , ptoc_Var101->ndsqlva1st , 0 , ptoc_Var101->ndsqlva1cnt);
            }
        p19cparentry(&(*apc) , 2 , ptoc_Var101->ndsqlva1ix , 0 , 0);
        *va1ix = (short ) sql__setrang((short ) ptoc_Var101->ndsqlva1ix , -32768 , 32767);
        }
    m90int2(14 , "paix        " , (short ) sql__setrang(*paix , -32768 , 32767));
    m90name(22 , "END p14_hostv_get ");
    }



int p14_prind_get(next_symbol, apc, prind)
int (*next_symbol)();
tpc_globals *apc;
int *prind;
    {
    tsp_int2 ptypentix;
    tsp_int2 pvatyp;
    tsp_int2 lastvatyp;
    tsp_knl_identifier statemn;
    tsp_int2 paix;
    tsp_c12 ch12;
    int no;
    tpc_globals *ptoc_Var102;
    sqlxatype *ptoc_Var103;
    tpr_scannertype *ptoc_Var104;
    sqlcatype *ptoc_Var105;
    tpc_cmd_part *ptoc_Var106;
    tpc_pc_sqlva *ptoc_Var107;
    tpc_symtab *ptoc_Var108;
    int ptoc_Var109;
    struct ptoc_Type75 *ptoc_Var110;
    m90print("PC14  p14_prind_");
    ptoc_Var102 = &*apc;
    ptoc_Var103 = &ptoc_Var102->sqlxa;
    ptoc_Var104 = &ptoc_Var102->pcscan;
    ptoc_Var105 = &ptoc_Var102->sqlca;
    ptoc_Var106 = &ptoc_Var102->pccmdpart;
    m90int(14 , "pcpr        " , (int ) ptoc_Var102->pcpr);
    ptoc_Var107 = &ptoc_Var102->pcsqlva;
    ptoc_Var108 = &ptoc_Var102->pcsymtab;
    paix = 0;
    _ptoc_ASSIGN(unsigned char , 64, statemn, "                                                                ")
    statemn[0] = (unsigned char)':';
    (*(tpc_partbuffer *) sql__nil(ptoc_Var106->partbufp , "vpc14.p" , 8102))[sql__setrang(ptoc_Var104->sypos , 1 , 16000) + -1] = (unsigned char)' ';
    ptoc_Var104->syposacc = ptoc_Var104->sypos + 1;
    ptypentix = 0;
    p14btypentget(next_symbol , &(*apc) , &ptypentix , &pvatyp , &lastvatyp , 1);
    if ((int ) ptypentix == 0)
        {
        if ((int ) ptoc_Var102->pcerror == 0)
            {
            p11precomerror(&(*apc) , 79);
            p11nerrorlisting(&(*apc) , (*(tpc_vartable *) sql__nil(ptoc_Var108->vartablep , "vpc14.p" , 8115))[sql__setrang((int ) (*(tpc_arrndtype *) sql__nil(ptoc_Var107->ndtabp , "vpc14.p" , 8115))[sql__setrang((int ) ptoc_Var107->ndcnt , 1 , 100) + -1].ndvarentix , 1 , 1000) + -1].vaname , (short ) sql__setrang((*(tpc_vartable *) sql__nil(ptoc_Var108->vartablep , "vpc14.p" , 8116))[sql__setrang((int ) (*(tpc_arrndtype *) sql__nil(ptoc_Var107->ndtabp , "vpc14.p" , 8116))[sql__setrang((int ) ptoc_Var107->ndcnt , 1 , 100) + -1].ndvarentix , 1 , 1000) + -1].vanaml , -32768 , 32767) , 0);
            }
        }
    else
        {
        if (((ptoc_Var109 = (int ) pvatyp) < 6 || ptoc_Var109 > 10) && ptoc_Var109 != 15 && ptoc_Var109 != 20 && ptoc_Var109 != 52 && ptoc_Var109 != 53 && ptoc_Var109 != 41 && ptoc_Var109 != 42)
            {
            p11precomerror(&(*apc) , 5);
            p11nerrorlisting(&(*apc) , (*(tpc_vartable *) sql__nil(ptoc_Var108->vartablep , "vpc14.p" , 8127))[sql__setrang((int ) (*(tpc_arrndtype *) sql__nil(ptoc_Var107->ndtabp , "vpc14.p" , 8127))[sql__setrang((int ) ptoc_Var107->ndcnt , 1 , 100) + -1].ndvarentix , 1 , 1000) + -1].vaname , (short ) sql__setrang((*(tpc_vartable *) sql__nil(ptoc_Var108->vartablep , "vpc14.p" , 8128))[sql__setrang((int ) (*(tpc_arrndtype *) sql__nil(ptoc_Var107->ndtabp , "vpc14.p" , 8128))[sql__setrang((int ) ptoc_Var107->ndcnt , 1 , 100) + -1].ndvarentix , 1 , 1000) + -1].vanaml , -32768 , 32767) , 0);
            }
        }
    if ((int ) ptoc_Var102->pcerror == 0)
        {
        paix = (short ) sql__setrang((short ) ((int ) ptoc_Var102->pcpa + 1) , -32768 , 32767);
        ptoc_Var110 = &(*(tpc_arrndtype *) sql__nil(ptoc_Var107->ndtabp , "vpc14.p" , 8136))[sql__setrang((int ) ptoc_Var107->ndcnt , 1 , 100) + -1];
        if (ptoc_Var110->ndsqlva1st != ptoc_Var110->ndsqlva1ix)
            {
            p19cparentry(&(*apc) , 1 , ptoc_Var110->ndsqlva1st , 0 , ptoc_Var110->ndsqlva1cnt);
            }
        p19cparentry(&(*apc) , 2 , ptoc_Var110->ndsqlva1ix , 0 , 0);
        no = ptoc_Var110->ndsqlva1ix;
        p05inttochr12(no , ch12);
        s10mv18(12 , sizeof(unsigned char [64]) , ch12 , 2 , statemn , 2 , 4);
        }
    p14_add_prind(&(*apc) , &(*prind) , &paix);
    _ptoc_ASSIGN(unsigned char , 64, (*(sqlprtype *) sql__nil(ptoc_Var103->sqlprp , "vpc14.p" , 8157))[sql__setrang(*prind , 1 , 500) + -1].prStmtName, statemn)
    m90int(14 , "prind       " , *prind);
    m90int2(14 , "paix        " , (short ) sql__setrang(paix , -32768 , 32767));
    m90int2(14 , "pcpr        " , (short ) sql__setrang(ptoc_Var102->pcpr , -32768 , 32767));
    m90name(22 , "END p14_prind_get ");
    }



int p14_add_prind(apc, prind, paix)
tpc_globals *apc;
int *prind;
tsp_int2 *paix;
    {
    tpc_globals *ptoc_Var111;
    sqlxatype *ptoc_Var112;
    sqlcatype *ptoc_Var113;
    sqlprentry *ptoc_Var114;
    m90print("PC14  p14_add_pr");
    ptoc_Var111 = &*apc;
    ptoc_Var112 = &ptoc_Var111->sqlxa;
    ptoc_Var113 = &ptoc_Var111->sqlca;
    m90int(14 , "pcpr        " , (int ) ptoc_Var111->pcpr);
    m90int2(14 , "paix        " , (short ) sql__setrang(*paix , -32768 , 32767));
    p19sqlprallocate(&ptoc_Var111->pcpr);
    ptoc_Var114 = &(*(sqlprtype *) sql__nil(ptoc_Var112->sqlprp , "vpc14.p" , 8182))[sql__setrang((int ) ptoc_Var111->pcpr , 1 , 500) + -1];
    ptoc_Var114->prstcount = 0;
    ptoc_Var114->prstindex = 0;
    ptoc_Var114->prcuindex = 0;
    ptoc_Var114->prstate = 0;
    _ptoc_ASSIGN(unsigned char , 64, ptoc_Var114->prStmtName, "                                                                ")
    ptoc_Var114->prStmtNameIndex = (short ) sql__setrang(*paix , -32768 , 32767);
    _ptoc_ASSIGN(unsigned char , 64, ptoc_Var114->prCursorName, "                                                                ")
    ptoc_Var114->prCursorNameIndex = 0;
    ptoc_Var114->prkaindex = (short ) sql__setrang(ptoc_Var111->pcka , -32768 , 32767);
    ptoc_Var114->prarea = 0;
    *prind = (int ) ptoc_Var111->pcpr;
    m90name(22 , "END p14_add_prind ");
    }



int p14prepare(next_symbol, apc)
int (*next_symbol)();
tpc_globals *apc;
    {
    tsp_knl_identifier statemn;
    int stateml;
    int pos;
    int len;
    int ind;
    int posab;
    int posch;
    unsigned char subch;
    tpc_pre_errors err;
    tsp_int2 ptypentix;
    tsp_int2 pvatyp;
    int retpos;
    int keyind;
    tsp_int2 lastvatyp;
    tsp_int2 paix;
    tsp_int2 va1ix;
    tsp_int2 kaind;
    tsp_int2 kaindlast;
    tsp_c12 ch12;
    int no;
    tpc_globals *ptoc_Var115;
    sqlxatype *ptoc_Var116;
    tpr_scannertype *ptoc_Var117;
    sqlcatype *ptoc_Var118;
    sqlratype *ptoc_Var119;
    tpc_cmd_part *ptoc_Var120;
    int ptoc_Var121;
    sqlkaentry *ptoc_Var122;
    sqlxatype *ptoc_Var123;
    sqlprentry *ptoc_Var124;
    sqlprentry *ptoc_Var125;
    sqlkaentry *ptoc_Var126;
    tpc_pc_sqlva *ptoc_Var127;
    tpc_symtab *ptoc_Var128;
    struct ptoc_Type75 *ptoc_Var129;
    m90print("PC14  p14prepare");
    ptoc_Var115 = &*apc;
    ptoc_Var116 = &ptoc_Var115->sqlxa;
    ptoc_Var117 = &ptoc_Var115->pcscan;
    ptoc_Var118 = &ptoc_Var115->sqlca;
    ptoc_Var119 = (sqlratype *) sql__nil(ptoc_Var118->sqlrap , "vpc14.p" , 8233);
    ptoc_Var120 = &ptoc_Var115->pccmdpart;
    ptoc_Var116->xadescpos = 0;
    ptoc_Var116->xadesclen = 0;
    ptoc_Var115->pcan.ancheck = (short ) sql__setrang(ptoc_Var115->pcopts.tsp4_component.C_sp4co_sql_precompiler.opt_mode , -32768 , 32767);
    ptoc_Var115->pcusage.uscom = 2;
    _ptoc_ASSIGN(unsigned char , 64, ptoc_Var115->pcusage.usdobjn1, "                                                                ")
    _ptoc_ASSIGN(unsigned char , 64, ptoc_Var115->pcusage.usdobjn2, "                                                                ")
    retpos = ptoc_Var117->syposacc;
    if ((ptoc_Var121 = (int ) ptoc_Var119->ralang) == 3 || ptoc_Var121 == 4)
        {
        while ((int ) ptoc_Var117->symb != 4 && (int ) ptoc_Var117->symb != 23)
            (*next_symbol)(&(*apc) , sql__nil(ptoc_Var120->partbufp , "vpc14.p" , 8246) , ptoc_Var120->part1len);
        }
    if ((int ) ptoc_Var119->ralang == 4)
        {
        ptoc_Var120->part1len = ptoc_Var120->part1len + 1;
        (*(tpc_partbuffer *) sql__nil(ptoc_Var120->partbufp , "vpc14.p" , 8253))[sql__setrang(ptoc_Var120->part1len , 1 , 16000) + -1] = (unsigned char)'\0';
        }
    ptoc_Var117->syposacc = retpos;
    ptoc_Var117->symb = 0;
    if ((int ) ptoc_Var115->pcerror == 0)
        {
        err = 0;
        m90int(14 , "pcpr   v    " , (int ) ptoc_Var115->pcpr);
        m90int2(14 , "pcka        " , (short ) sql__setrang(ptoc_Var115->pcka , -32768 , 32767));
        m90int2(14 , "ind         " , (short ) sql__setrang((short ) ind , -32768 , 32767));
        p14kaentryinit(&(*apc));
        p19sqlprallocate(&ptoc_Var115->pcpr);
        m90int(14 , "pcpr   n    " , (int ) ptoc_Var115->pcpr);
        m90int2(14 , "pcka        " , (short ) sql__setrang(ptoc_Var115->pcka , -32768 , 32767));
        m90int2(14 , "ind         " , (short ) sql__setrang((short ) ind , -32768 , 32767));
        ptoc_Var122 = &(*(sqlkatype *) sql__nil(ptoc_Var116->sqlkap , "vpc14.p" , 8270))[sql__setrang((int ) ptoc_Var115->pcka , 1 , 3000) + -1];
        ptoc_Var122->kaprindex = (short ) sql__setrang(ptoc_Var115->pcpr , -32768 , 32767);
        paix = 0;
        (*next_symbol)(&(*apc) , sql__nil(ptoc_Var120->partbufp , "vpc14.p" , 8275) , ptoc_Var120->part1len);
        if ((int ) ptoc_Var117->symb == 13 && (int ) err == 0)
            {
            m90c30(14 , "in slow version expand hostvar");
            ind = (int ) ptoc_Var115->pcpr;
            p14_hostv_get(next_symbol , &(*apc) , &paix , &va1ix);
            no = (int ) va1ix;
            p05inttochr12(no , ch12);
            _ptoc_ASSIGN(unsigned char , 64, statemn, "                                                                ")
            statemn[0] = (unsigned char)':';
            s10mv18(12 , sizeof(unsigned char [64]) , ch12 , 2 , statemn , 2 , 4);
            m90int(14 , "pcpr        " , (int ) ptoc_Var115->pcpr);
            m90int2(14 , "pcka        " , (short ) sql__setrang(ptoc_Var115->pcka , -32768 , 32767));
            m90int2(14 , "paix        " , (short ) sql__setrang(paix , -32768 , 32767));
            m90int2(14 , "va1ix       " , (short ) sql__setrang(va1ix , -32768 , 32767));
            }
        else
            {
            p19getname(&(*apc) , statemn , &stateml);
            if ((int ) (*(tpc_partbuffer *) sql__nil(ptoc_Var120->partbufp , "vpc14.p" , 8297))[sql__setrang(ptoc_Var117->sypos + -1 , 1 , 16000) + -1] != 34)
                {
                p05up2casebuf(statemn , 1 , stateml);
                }
            }
        p19sprepareprname(&(*apc) , statemn , &ind);
        m90int(14 , "pcpr        " , (int ) ptoc_Var115->pcpr);
        m90int2(14 , "pcka        " , (short ) sql__setrang(ptoc_Var115->pcka , -32768 , 32767));
        m90int2(14 , "ind         " , (short ) sql__setrang((short ) ind , -32768 , 32767));
        ind = (int ) ptoc_Var115->pcpr;
        (*(sqlprtype *) sql__nil(ptoc_Var116->sqlprp , "vpc14.p" , 8308))[sql__setrang(ind , 1 , 500) + -1].prkaindex = 0;
        (*next_symbol)(&(*apc) , sql__nil(ptoc_Var120->partbufp , "vpc14.p" , 8309) , ptoc_Var120->part1len);
        if ((int ) err == 0)
            {
            ptoc_Var123 = &ptoc_Var115->sqlxa;
            ptoc_Var124 = &(*(sqlprtype *) sql__nil(ptoc_Var123->sqlprp , "vpc14.p" , 8312))[sql__setrang(ind , 1 , 500) + -1];
            ptoc_Var124->prstate = 4;
            ptoc_Var124->prarea = 0;
            ptoc_Var124->prStmtNameIndex = (short ) sql__setrang(paix , -32768 , 32767);
            if ((int ) ptoc_Var124->prkaindex == 0)
                {
                ptoc_Var124->prkaindex = (short ) sql__setrang(ptoc_Var115->pcka , -32768 , 32767);
                }
            ptoc_Var124->prDescribe = 0;
            _ptoc_ASSIGN(unsigned char , 64, ptoc_Var124->prStmtName, statemn)
            ptoc_Var124->prcuindex = 0;
            p10getkeyword(&(*apc) , sql__nil(ptoc_Var120->partbufp , "vpc14.p" , 8326) , &keyind);
            if (keyind == 56)
                {
                ptoc_Var123->xakano = (short ) sql__setrang(ptoc_Var115->pcka , -32768 , 32767);
                (*next_symbol)(&(*apc) , sql__nil(ptoc_Var120->partbufp , "vpc14.p" , 8331) , ptoc_Var120->part1len);
                ptoc_Var123->xadescpos = (short ) sql__setrang((short ) ptoc_Var117->sypos , -32768 , 32767);
                while (keyind != 132 && keyind != 47 && (int ) ptoc_Var117->symb != 4 && (int ) ptoc_Var117->symb != 23)
                    {
                    ptoc_Var123->xadesclen = (short ) sql__setrang((short ) (ptoc_Var117->syposacc - (int ) ptoc_Var123->xadescpos) , -32768 , 32767);
                    (*next_symbol)(&(*apc) , sql__nil(ptoc_Var120->partbufp , "vpc14.p" , 8338) , ptoc_Var120->part1len);
                    p10getkeyword(&(*apc) , sql__nil(ptoc_Var120->partbufp , "vpc14.p" , 8339) , &keyind);
                    }
                if (keyind == 132)
                    {
                    (*next_symbol)(&(*apc) , sql__nil(ptoc_Var120->partbufp , "vpc14.p" , 8345) , ptoc_Var120->part1len);
                    p10getkeyword(&(*apc) , sql__nil(ptoc_Var120->partbufp , "vpc14.p" , 8346) , &keyind);
                    if (keyind != 67 && keyind != 59 && keyind != 3)
                        {
                        p11precomerror(&(*apc) , 60);
                        }
                    (*next_symbol)(&(*apc) , sql__nil(ptoc_Var120->partbufp , "vpc14.p" , 8353) , ptoc_Var120->part1len);
                    }
                p10getkeyword(&(*apc) , sql__nil(ptoc_Var120->partbufp , "vpc14.p" , 8356) , &keyind);
                }
            if (keyind == 47)
                {
                (*next_symbol)(&(*apc) , sql__nil(ptoc_Var120->partbufp , "vpc14.p" , 8361) , ptoc_Var120->part1len);
                }
            }
        ptoc_Var125 = &(*(sqlprtype *) sql__nil(ptoc_Var116->sqlprp , "vpc14.p" , 8368))[sql__setrang(ind , 1 , 500) + -1];
        ptoc_Var126 = &(*(sqlkatype *) sql__nil(ptoc_Var116->sqlkap , "vpc14.p" , 8368))[sql__setrang((int ) ptoc_Var115->pcka , 1 , 3000) + -1];
        ptoc_Var127 = &ptoc_Var115->pcsqlva;
        ptoc_Var128 = &ptoc_Var115->pcsymtab;
        if (((int ) ptoc_Var117->symb == 13 || (int ) ptoc_Var117->symb == 8) && (int ) err == 0)
            {
            if ((int ) ptoc_Var117->symb == 13)
                {
                ptoc_Var117->syposacc = ptoc_Var117->sypos + 1;
                }
            else
                {
                ptoc_Var117->syposacc = ptoc_Var117->sypos;
                }
            ptypentix = 0;
            p14btypentget(next_symbol , &(*apc) , &ptypentix , &pvatyp , &lastvatyp , 1);
            if ((int ) ptypentix == 0)
                {
                if ((int ) ptoc_Var115->pcerror == 0)
                    {
                    p11precomerror(&(*apc) , 79);
                    p11nerrorlisting(&(*apc) , (*(tpc_vartable *) sql__nil(ptoc_Var128->vartablep , "vpc14.p" , 8395))[sql__setrang((int ) (*(tpc_arrndtype *) sql__nil(ptoc_Var127->ndtabp , "vpc14.p" , 8395))[sql__setrang((int ) ptoc_Var127->ndcnt , 1 , 100) + -1].ndvarentix , 1 , 1000) + -1].vaname , (short ) sql__setrang((*(tpc_vartable *) sql__nil(ptoc_Var128->vartablep , "vpc14.p" , 8396))[sql__setrang((int ) (*(tpc_arrndtype *) sql__nil(ptoc_Var127->ndtabp , "vpc14.p" , 8396))[sql__setrang((int ) ptoc_Var127->ndcnt , 1 , 100) + -1].ndvarentix , 1 , 1000) + -1].vanaml , -32768 , 32767) , 0);
                    }
                }
            else
                {
                if (((ptoc_Var121 = (int ) pvatyp) < 6 || ptoc_Var121 > 10) && ptoc_Var121 != 15 && ptoc_Var121 != 20 && ptoc_Var121 != 41 && ptoc_Var121 != 42 && ptoc_Var121 != 52 && ptoc_Var121 != 53)
                    {
                    p11precomerror(&(*apc) , 5);
                    p11nerrorlisting(&(*apc) , (*(tpc_vartable *) sql__nil(ptoc_Var128->vartablep , "vpc14.p" , 8408))[sql__setrang((int ) (*(tpc_arrndtype *) sql__nil(ptoc_Var127->ndtabp , "vpc14.p" , 8408))[sql__setrang((int ) ptoc_Var127->ndcnt , 1 , 100) + -1].ndvarentix , 1 , 1000) + -1].vaname , (short ) sql__setrang((*(tpc_vartable *) sql__nil(ptoc_Var128->vartablep , "vpc14.p" , 8409))[sql__setrang((int ) (*(tpc_arrndtype *) sql__nil(ptoc_Var127->ndtabp , "vpc14.p" , 8409))[sql__setrang((int ) ptoc_Var127->ndcnt , 1 , 100) + -1].ndvarentix , 1 , 1000) + -1].vanaml , -32768 , 32767) , 0);
                    }
                }
            ptoc_Var126->kaStcount = 0;
            ptoc_Var126->kaStindex = (short ) sql__setrang((short ) ((int ) ptoc_Var115->pcpa + 1) , -32768 , 32767);
            ptoc_Var125->prstcount = 0;
            ptoc_Var125->prstindex = (short ) sql__setrang(ptoc_Var126->kaStindex , -32768 , 32767);
            ptoc_Var125->prcuindex = 0;
            _ptoc_ASSIGN(unsigned char , 64, ptoc_Var125->prCursorName, "                                                                ")
            ptoc_Var125->prCursorNameIndex = 0;
            _ptoc_ASSIGN(unsigned char , 64, ptoc_Var125->prStmtName, statemn)
            ptoc_Var129 = &(*(tpc_arrndtype *) sql__nil(ptoc_Var127->ndtabp , "vpc14.p" , 8421))[sql__setrang((int ) ptoc_Var127->ndcnt , 1 , 100) + -1];
            if (ptoc_Var129->ndsqlva1st != ptoc_Var129->ndsqlva1ix)
                {
                p19cparentry(&(*apc) , 1 , ptoc_Var129->ndsqlva1st , 0 , ptoc_Var129->ndsqlva1cnt);
                ptoc_Var126->kaStcount = 1;
                }
            p19cparentry(&(*apc) , 2 , ptoc_Var129->ndsqlva1ix , 0 , 0);
            ptoc_Var126->kaStcount = (short ) sql__setrang((short ) ((int ) ptoc_Var126->kaStcount + 1) , -32768 , 32767);
            }
        else
            {
            if ((int ) ptoc_Var117->symb != 21 && (int ) ptoc_Var117->symb != 24 || (int ) err != 0)
                {
                if ((int ) err == 0)
                    {
                    err = 33;
                    }
                }
            else
                {
                p10stringliteralget(&(*apc) , sql__nil(ptoc_Var120->partbufp , "vpc14.p" , 8451));
                posab = ptoc_Var117->sypos;
                posch = 1;
                while (posch != 0)
                    {
                    p19snextsubchar(&(*apc) , posab , &posch , &subch);
                    if (posch != 0 && (int ) subch != 63)
                        {
                        err = 56;
                        }
                    posab = posch + 1;
                    }
                if ((int ) err == 0)
                    {
                    ptoc_Var126->kaStindex = (short ) sql__setrang((short ) ((int ) ptoc_Var115->pcst + 1) , -32768 , 32767);
                    ptoc_Var125->prstindex = (short ) sql__setrang(ptoc_Var126->kaStindex , -32768 , 32767);
                    ptoc_Var125->prarea = 1;
                    _ptoc_ASSIGN(unsigned char , 64, ptoc_Var125->prStmtName, statemn)
                    _ptoc_ASSIGN(unsigned char , 64, ptoc_Var125->prCursorName, "                                                                ")
                    ptoc_Var125->prCursorNameIndex = 0;
                    ptoc_Var125->prcuindex = 0;
                    pos = ptoc_Var117->sypos;
                    while (pos < ptoc_Var117->sypos + ptoc_Var117->sylen)
                        {
                        if (pos + 52 > ptoc_Var117->sypos + ptoc_Var117->sylen)
                            {
                            len = ptoc_Var117->sypos + ptoc_Var117->sylen - pos;
                            }
                        else
                            {
                            len = 52;
                            }
                        p19sqlstallocate(&ptoc_Var115->pcst);
                        if ((int ) ptoc_Var115->pcst > (int ) ptoc_Var115->pcstmax)
                            {
                            ptoc_Var115->pcstmax = (short ) sql__setrang(ptoc_Var115->pcst , -32768 , 32767);
                            }
                        ptoc_Var126->kaStcount = (short ) sql__setrang((short ) ((int ) ptoc_Var126->kaStcount + 1) , -32768 , 32767);
                        ptoc_Var125->prstcount = (short ) sql__setrang(ptoc_Var126->kaStcount , -32768 , 32767);
                        s10mv1(ptoc_Var120->partsizlen , 52 , sql__nil(ptoc_Var120->partbufp , "vpc14.p" , 8492) , pos , (*(sqlsttype *) sql__nil(ptoc_Var116->sqlstp , "vpc14.p" , 8493))[sql__setrang((int ) ptoc_Var115->pcst , 1 , 1000) + -1].stline , 1 , len);
                        (*(sqlsttype *) sql__nil(ptoc_Var116->sqlstp , "vpc14.p" , 8494))[sql__setrang((int ) ptoc_Var115->pcst , 1 , 1000) + -1].stllen = len;
                        (*(sqlsttype *) sql__nil(ptoc_Var116->sqlstp , "vpc14.p" , 8495))[sql__setrang((int ) ptoc_Var115->pcst , 1 , 1000) + -1].stkano = (int ) ptoc_Var115->pcka;
                        pos = pos + len;
                        }
                    }
                }
            }
        if ((int ) err != 0)
            {
            p11precomerror(&(*apc) , (unsigned char ) sql__setrang(err , 0 , 90));
            }
        }
    ptoc_Var117->symb = 4;
    m90int2(14 , "xakano      " , (short ) sql__setrang(ptoc_Var116->xakano , -32768 , 32767));
    m90int2(14 , "xadescpos   " , (short ) sql__setrang(ptoc_Var116->xadescpos , -32768 , 32767));
    m90int2(14 , "xadesclen   " , (short ) sql__setrang(ptoc_Var116->xadesclen , -32768 , 32767));
    m90name(22 , "END   p14prepa    ");
    }



int p14execute(next_symbol, apc, statemno, immediate)
int (*next_symbol)();
tpc_globals *apc;
int *statemno;
int *immediate;
    {
    tsp_knl_identifier statemn;
    int stateml;
    int keyind;
    int prind;
    int posab;
    int pos;
    int len;
    int posch;
    int mnr;
    unsigned char subch;
    tpc_pre_errors err;
    tsp_int2 ptypentix;
    tsp_int2 pvatyp;
    tsp_int2 kaind;
    tsp_int2 retpos;
    tsp_int2 lastvatyp;
    int ndindex;
    tpc_globals *ptoc_Var130;
    tpr_scannertype *ptoc_Var131;
    sqlcatype *ptoc_Var132;
    sqlratype *ptoc_Var133;
    tpc_cmd_part *ptoc_Var134;
    sqlxatype *ptoc_Var135;
    tpc_pc_sqlva *ptoc_Var136;
    int ptoc_Var137;
    sqlprentry *ptoc_Var138;
    sqlkaentry *ptoc_Var139;
    sqlxatype *ptoc_Var140;
    tpc_pc_sqlva *ptoc_Var141;
    tpc_symtab *ptoc_Var142;
    struct ptoc_Type75 *ptoc_Var143;
    sqlkaentry *ptoc_Var144;
    sqlkaentry *ptoc_Var145;
    sqlkaentry *ptoc_Var146;
    m90print("PC14  p14execute");
    ptoc_Var130 = &*apc;
    ptoc_Var131 = &ptoc_Var130->pcscan;
    ptoc_Var132 = &ptoc_Var130->sqlca;
    ptoc_Var133 = (sqlratype *) sql__nil(ptoc_Var132->sqlrap , "vpc14.p" , 8549);
    ptoc_Var134 = &ptoc_Var130->pccmdpart;
    ptoc_Var135 = &ptoc_Var130->sqlxa;
    ptoc_Var136 = &ptoc_Var130->pcsqlva;
    retpos = (short ) sql__setrang((short ) ptoc_Var131->syposacc , -32768 , 32767);
    ptoc_Var135->xadescpos = 0;
    ptoc_Var135->xadesclen = 0;
    if ((ptoc_Var137 = (int ) ptoc_Var133->ralang) == 3 || ptoc_Var137 == 4)
        {
        while ((int ) ptoc_Var131->symb != 4 && (int ) ptoc_Var131->symb != 23)
            (*next_symbol)(&(*apc) , sql__nil(ptoc_Var134->partbufp , "vpc14.p" , 8558) , ptoc_Var134->part1len);
        }
    if ((int ) ptoc_Var133->ralang == 4)
        {
        ptoc_Var134->part1len = ptoc_Var134->part1len + 1;
        (*(tpc_partbuffer *) sql__nil(ptoc_Var134->partbufp , "vpc14.p" , 8565))[sql__setrang(ptoc_Var134->part1len , 1 , 16000) + -1] = (unsigned char)'\0';
        }
    ptoc_Var131->syposacc = (int ) retpos;
    ptoc_Var131->symb = 0;
    if ((int ) ptoc_Var130->pcerror == 0)
        {
        err = 0;
        if ((int ) err == 0)
            {
            *immediate = 0;
            (*next_symbol)(&(*apc) , sql__nil(ptoc_Var134->partbufp , "vpc14.p" , 8578) , ptoc_Var134->part1len);
            p10getkeyword(&(*apc) , sql__nil(ptoc_Var134->partbufp , "vpc14.p" , 8579) , &keyind);
            if (keyind == 50)
                {
                p14kaentryinit(&(*apc));
                p19sqlprallocate(&ptoc_Var130->pcpr);
                p19sqlfaallocate(&ptoc_Var130->pcfa);
                *statemno = (int ) ptoc_Var130->pcka;
                ptoc_Var138 = &(*(sqlprtype *) sql__nil(ptoc_Var130->sqlxa.sqlprp , "vpc14.p" , 8587))[sql__setrang((int ) ptoc_Var130->pcpr , 1 , 500) + -1];
                ptoc_Var139 = &(*(sqlkatype *) sql__nil(ptoc_Var130->sqlxa.sqlkap , "vpc14.p" , 8588))[sql__setrang((int ) ptoc_Var130->pcka , 1 , 3000) + -1];
                ptoc_Var140 = &ptoc_Var130->sqlxa;
                ptoc_Var141 = &ptoc_Var130->pcsqlva;
                ptoc_Var142 = &ptoc_Var130->pcsymtab;
                ptoc_Var138->prstcount = 0;
                ptoc_Var138->prstindex = 0;
                ptoc_Var138->prcuindex = 0;
                ptoc_Var138->prstate = 4;
                _ptoc_ASSIGN(unsigned char , 64, ptoc_Var138->prStmtName, "                                                                ")
                ptoc_Var138->prStmtNameIndex = 0;
                _ptoc_ASSIGN(unsigned char , 64, ptoc_Var138->prCursorName, "                                                                ")
                ptoc_Var138->prCursorNameIndex = 0;
                ptoc_Var138->prkaindex = (short ) sql__setrang(ptoc_Var130->pcka , -32768 , 32767);
                ptoc_Var138->prarea = 0;
                ptoc_Var138->prDescribe = 0;
                ptoc_Var139->kaprindex = (short ) sql__setrang(ptoc_Var130->pcpr , -32768 , 32767);
                ptoc_Var139->kafaindex = (short ) sql__setrang(ptoc_Var130->pcfa , -32768 , 32767);
                (*next_symbol)(&(*apc) , sql__nil(ptoc_Var134->partbufp , "vpc14.p" , 8603) , ptoc_Var134->part1len);
                if ((int ) ptoc_Var131->symb == 13)
                    {
                    *immediate = 1;
                    ptoc_Var131->syposacc = ptoc_Var131->sypos + 1;
                    ptypentix = 0;
                    p14btypentget(next_symbol , &(*apc) , &ptypentix , &pvatyp , &lastvatyp , 1);
                    if ((int ) ptypentix == 0)
                        {
                        if ((int ) ptoc_Var130->pcerror == 0)
                            {
                            p11precomerror(&(*apc) , 79);
                            p11nerrorlisting(&(*apc) , (*(tpc_vartable *) sql__nil(ptoc_Var142->vartablep , "vpc14.p" , 8621))[sql__setrang((int ) (*(tpc_arrndtype *) sql__nil(ptoc_Var141->ndtabp , "vpc14.p" , 8621))[sql__setrang((int ) ptoc_Var141->ndcnt , 1 , 100) + -1].ndvarentix , 1 , 1000) + -1].vaname , (short ) sql__setrang((*(tpc_vartable *) sql__nil(ptoc_Var142->vartablep , "vpc14.p" , 8623))[sql__setrang((int ) (*(tpc_arrndtype *) sql__nil(ptoc_Var141->ndtabp , "vpc14.p" , 8623))[sql__setrang((int ) ptoc_Var141->ndcnt , 1 , 100) + -1].ndvarentix , 1 , 1000) + -1].vanaml , -32768 , 32767) , 0);
                            }
                        }
                    else
                        {
                        ptoc_Var143 = &(*(tpc_arrndtype *) sql__nil(ptoc_Var141->ndtabp , "vpc14.p" , 8630))[sql__setrang((int ) ptoc_Var141->ndcnt , 1 , 100) + -1];
                        ptoc_Var139->kaStindex = (short ) sql__setrang((short ) ((int ) ptoc_Var130->pcpa + 1) , -32768 , 32767);
                        if (ptoc_Var143->ndsqlva1st != ptoc_Var143->ndsqlva1ix)
                            {
                            p19cparentry(&(*apc) , 1 , ptoc_Var143->ndsqlva1st , 0 , ptoc_Var143->ndsqlva1cnt);
                            }
                        p19cparentry(&(*apc) , 2 , ptoc_Var143->ndsqlva1ix , 0 , 0);
                        }
                    }
                else
                    {
                    p10stringliteralget(&(*apc) , sql__nil(ptoc_Var134->partbufp , "vpc14.p" , 8648));
                    *immediate = 2;
                    s10fil(ptoc_Var134->partsizlen , sql__nil(ptoc_Var134->partbufp , "vpc14.p" , 8650) , 1 , ptoc_Var131->sypos + -1 , (unsigned char)' ');
                    posab = ptoc_Var131->sypos;
                    posch = 1;
                    while (posch != 0)
                        {
                        p19snextsubchar(&(*apc) , posab , &posch , &subch);
                        if ((int ) subch == 37)
                            {
                            ptoc_Var139->kamacro = 1;
                            p19gmacronumber(&(*apc) , posch , &mnr , &posch);
                            err = 90;
                            }
                        posab = posch + 1;
                        }
                    ptoc_Var138->prarea = 1;
                    ptoc_Var139->kaStcount = 0;
                    ptoc_Var139->kaStindex = (short ) sql__setrang((short ) ((int ) ptoc_Var130->pcst + 1) , -32768 , 32767);
                    pos = ptoc_Var131->sypos;
                    while (pos < ptoc_Var131->sypos + ptoc_Var131->sylen)
                        {
                        if (pos + 52 > ptoc_Var131->sypos + ptoc_Var131->sylen)
                            {
                            len = ptoc_Var131->sypos + ptoc_Var131->sylen - pos;
                            }
                        else
                            {
                            len = 52;
                            }
                        p19sqlstallocate(&ptoc_Var130->pcst);
                        if ((int ) ptoc_Var130->pcst > (int ) ptoc_Var130->pcstmax)
                            {
                            ptoc_Var130->pcstmax = (short ) sql__setrang(ptoc_Var130->pcst , -32768 , 32767);
                            }
                        ptoc_Var139->kaStcount = (short ) sql__setrang((short ) ((int ) ptoc_Var139->kaStcount + 1) , -32768 , 32767);
                        s10mv1(ptoc_Var134->partsizlen , 52 , sql__nil(ptoc_Var134->partbufp , "vpc14.p" , 8688) , pos , (*(sqlsttype *) sql__nil(ptoc_Var140->sqlstp , "vpc14.p" , 8689))[sql__setrang((int ) ptoc_Var130->pcst , 1 , 1000) + -1].stline , 1 , len);
                        (*(sqlsttype *) sql__nil(ptoc_Var140->sqlstp , "vpc14.p" , 8690))[sql__setrang((int ) ptoc_Var130->pcst , 1 , 1000) + -1].stllen = len;
                        (*(sqlsttype *) sql__nil(ptoc_Var140->sqlstp , "vpc14.p" , 8691))[sql__setrang((int ) ptoc_Var130->pcst , 1 , 1000) + -1].stkano = (int ) ptoc_Var130->pcka;
                        pos = pos + len;
                        }
                    ptoc_Var131->syposacc = ptoc_Var131->sypos;
                    (*next_symbol)(&(*apc) , sql__nil(ptoc_Var134->partbufp , "vpc14.p" , 8696) , ptoc_Var134->part1len);
                    p10getkeyword(&(*apc) , sql__nil(ptoc_Var134->partbufp , "vpc14.p" , 8697) , &keyind);
                    if (keyind == 19)
                        {
                        ptoc_Var130->pcan.ancomtyp = 2;
                        }
                    }
                if ((int ) err == 0)
                    {
                    ptoc_Var130->pcan.ancheck = (short ) sql__setrang(ptoc_Var130->pcopts.tsp4_component.C_sp4co_sql_precompiler.opt_mode , -32768 , 32767);
                    }
                ptoc_Var130->sqlxa.xakano = (short ) sql__setrang(ptoc_Var130->pcka , -32768 , 32767);
                }
            else
                {
                ndindex = 0;
                if ((int ) ptoc_Var131->symb == 13 && (int ) err == 0)
                    {
                    m90buf(14 , &(*(tpc_partbuffer *) sql__nil(ptoc_Var134->partbufp , "vpc14.p" , 8720))[0] , 1 , ptoc_Var134->part1len);
                    m90int2(14 , "syposacc    " , (short ) sql__setrang((short ) ptoc_Var131->syposacc , -32768 , 32767));
                    m90int2(14 , "sypos       " , (short ) sql__setrang((short ) ptoc_Var131->sypos , -32768 , 32767));
                    p14_prind_get(next_symbol , &(*apc) , &prind);
                    ndindex = (int ) ptoc_Var136->ndcnt;
                    }
                else
                    {
                    p19getname(&(*apc) , statemn , &stateml);
                    if ((int ) (*(tpc_partbuffer *) sql__nil(ptoc_Var134->partbufp , "vpc14.p" , 8729))[sql__setrang(ptoc_Var131->sypos + -1 , 1 , 16000) + -1] != 34)
                        {
                        p05up2casebuf(statemn , 1 , stateml);
                        }
                    p19sstatementprname(&(*apc) , statemn , &prind);
                    m90int2(14 , "prind       " , (short ) sql__setrang((short ) prind , -32768 , 32767));
                    (*next_symbol)(&(*apc) , sql__nil(ptoc_Var134->partbufp , "vpc14.p" , 8735) , ptoc_Var134->part1len);
                    }
                if (prind == 0)
                    {
                    err = 82;
                    }
                else
                    {
                    m90int2(14 , "prind       " , (short ) sql__setrang((short ) prind , -32768 , 32767));
                    m90int2(14 , "pcka        " , (short ) sql__setrang(ptoc_Var130->pcka , -32768 , 32767));
                    m90int2(14 , "prStmtNameIn" , (short ) sql__setrang((*(sqlprtype *) sql__nil(ptoc_Var135->sqlprp , "vpc14.p" , 8745))[sql__setrang(prind , 1 , 500) + -1].prStmtNameIndex , -32768 , 32767));
                    if ((int ) (*(sqlprtype *) sql__nil(ptoc_Var135->sqlprp , "vpc14.p" , 8746))[sql__setrang(prind , 1 , 500) + -1].prStmtNameIndex > 0)
                        {
                        p14kaentryinit(&(*apc));
                        m90int2(14 , "pcka        " , (short ) sql__setrang(ptoc_Var130->pcka , -32768 , 32767));
                        ptoc_Var144 = &(*(sqlkatype *) sql__nil(ptoc_Var135->sqlkap , "vpc14.p" , 8751))[sql__setrang((int ) ptoc_Var130->pcka , 1 , 3000) + -1];
                        kaind = (short ) sql__setrang(ptoc_Var130->pcka , -32768 , 32767);
                        ptoc_Var144->kaprindex = (short ) sql__setrang((short ) prind , -32768 , 32767);
                        }
                    else
                        {
                        kaind = (short ) sql__setrang((*(sqlprtype *) sql__nil(ptoc_Var135->sqlprp , "vpc14.p" , 8759))[sql__setrang(prind , 1 , 500) + -1].prkaindex , -32768 , 32767);
                        }
                    m90int2(14 , "kaind       " , (short ) sql__setrang(kaind , -32768 , 32767));
                    if ((int ) ptoc_Var131->symb == 4)
                        {
                        p14kaentryinit(&(*apc));
                        p19sqlfaallocate(&ptoc_Var130->pcfa);
                        *statemno = (int ) ptoc_Var130->pcka;
                        (*(sqlkatype *) sql__nil(ptoc_Var135->sqlkap , "vpc14.p" , 8769))[sql__setrang((int ) ptoc_Var130->pcka , 1 , 3000) + -1] = (*(sqlkatype *) sql__nil(ptoc_Var135->sqlkap , "vpc14.p" , 8769))[sql__setrang((int ) kaind , 1 , 3000) + -1];
                        ptoc_Var145 = &(*(sqlkatype *) sql__nil(ptoc_Var130->sqlxa.sqlkap , "vpc14.p" , 8770))[sql__setrang((int ) ptoc_Var130->pcka , 1 , 3000) + -1];
                        ptoc_Var145->katyp = (short ) sql__setrang(ptoc_Var130->pcan.ancomtyp , -32768 , 32767);
                        ptoc_Var145->kapaindex = 0;
                        if ((int ) ptoc_Var130->pcinpind == 2)
                            {
                            ptoc_Var145->kalineno = (short ) sql__setrang(ptoc_Var130->pclno.lnosrc , -32768 , 32767);
                            }
                        else
                            {
                            ptoc_Var145->kalineno = (short ) sql__setrang((short ) -(int ) ptoc_Var130->pclno.lnoexp , -32768 , 32767);
                            }
                        ptoc_Var145->kamode = (short ) sql__setrang(ptoc_Var133->rakamode , -32768 , 32767);
                        ptoc_Var145->kaatindex = (short ) sql__setrang(ptoc_Var130->pckaatind , -32768 , 32767);
                        ptoc_Var145->kacuindex = 0;
                        ptoc_Var130->pcan.ancheck = (short ) sql__setrang(ptoc_Var130->pcopts.tsp4_component.C_sp4co_sql_precompiler.opt_mode , -32768 , 32767);
                        ptoc_Var130->pcda = 300;
                        (*(sqlprtype *) sql__nil(ptoc_Var130->sqlxa.sqlprp , "vpc14.p" , 8785))[sql__setrang(prind , 1 , 500) + -1].prDescribe = 1;
                        (*(sqlprtype *) sql__nil(ptoc_Var130->sqlxa.sqlprp , "vpc14.p" , 8788))[sql__setrang(prind , 1 , 500) + -1].prkaindex = (short ) sql__setrang(ptoc_Var130->pcka , -32768 , 32767);
                        ptoc_Var145->kadiindex = (short ) sql__setrang(kaind , -32768 , 32767);
                        ptoc_Var145->kafaindex = (short ) sql__setrang(ptoc_Var130->pcfa , -32768 , 32767);
                        }
                    else
                        {
                        p10getkeyword(&(*apc) , sql__nil(ptoc_Var134->partbufp , "vpc14.p" , 8797) , &keyind);
                        if (keyind == 132)
                            {
                            (*next_symbol)(&(*apc) , sql__nil(ptoc_Var134->partbufp , "vpc14.p" , 8801) , ptoc_Var134->part1len);
                            p10getkeyword(&(*apc) , sql__nil(ptoc_Var134->partbufp , "vpc14.p" , 8802) , &keyind);
                            if (keyind == 33)
                                {
                                p14kaentryinit(&(*apc));
                                p19sqlfaallocate(&ptoc_Var130->pcfa);
                                *statemno = (int ) ptoc_Var130->pcka;
                                (*(sqlkatype *) sql__nil(ptoc_Var135->sqlkap , "vpc14.p" , 8809))[sql__setrang((int ) ptoc_Var130->pcka , 1 , 3000) + -1] = (*(sqlkatype *) sql__nil(ptoc_Var135->sqlkap , "vpc14.p" , 8809))[sql__setrang((int ) kaind , 1 , 3000) + -1];
                                ptoc_Var146 = &(*(sqlkatype *) sql__nil(ptoc_Var130->sqlxa.sqlkap , "vpc14.p" , 8810))[sql__setrang((int ) ptoc_Var130->pcka , 1 , 3000) + -1];
                                ptoc_Var146->katyp = (short ) sql__setrang(ptoc_Var130->pcan.ancomtyp , -32768 , 32767);
                                ptoc_Var146->kapacount = -1;
                                ptoc_Var146->kapaindex = 0;
                                if ((int ) ptoc_Var130->pcinpind == 2)
                                    {
                                    ptoc_Var146->kalineno = (short ) sql__setrang(ptoc_Var130->pclno.lnosrc , -32768 , 32767);
                                    }
                                else
                                    {
                                    ptoc_Var146->kalineno = (short ) sql__setrang((short ) -(int ) ptoc_Var130->pclno.lnoexp , -32768 , 32767);
                                    }
                                ptoc_Var146->kamode = (short ) sql__setrang(ptoc_Var133->rakamode , -32768 , 32767);
                                ptoc_Var146->kaatindex = (short ) sql__setrang(ptoc_Var130->pckaatind , -32768 , 32767);
                                ptoc_Var146->kacuindex = 0;
                                ptoc_Var130->pcan.ancheck = (short ) sql__setrang(ptoc_Var130->pcopts.tsp4_component.C_sp4co_sql_precompiler.opt_mode , -32768 , 32767);
                                ptoc_Var130->pcda = 300;
                                (*(sqlprtype *) sql__nil(ptoc_Var130->sqlxa.sqlprp , "vpc14.p" , 8826))[sql__setrang(prind , 1 , 500) + -1].prDescribe = 1;
                                (*(sqlprtype *) sql__nil(ptoc_Var130->sqlxa.sqlprp , "vpc14.p" , 8829))[sql__setrang(prind , 1 , 500) + -1].prkaindex = (short ) sql__setrang(ptoc_Var130->pcka , -32768 , 32767);
                                ptoc_Var146->kadiindex = (short ) sql__setrang(kaind , -32768 , 32767);
                                ptoc_Var146->kafaindex = (short ) sql__setrang(ptoc_Var130->pcfa , -32768 , 32767);
                                (*next_symbol)(&(*apc) , sql__nil(ptoc_Var134->partbufp , "vpc14.p" , 8834) , ptoc_Var134->part1len);
                                if ((int ) ptoc_Var131->symb != 4)
                                    {
                                    if ((int ) (*(tpc_partbuffer *) sql__nil(ptoc_Var134->partbufp , "vpc14.p" , 8838))[sql__setrang(ptoc_Var131->sypos , 1 , 16000) + -1] == 58)
                                        {
                                        ptoc_Var135->xadescpos = (short ) sql__setrang((short ) (ptoc_Var131->sypos + 1) , -32768 , 32767);
                                        }
                                    else
                                        {
                                        ptoc_Var135->xadescpos = (short ) sql__setrang((short ) ptoc_Var131->sypos , -32768 , 32767);
                                        }
                                    while ((int ) ptoc_Var131->symb != 4 && (int ) ptoc_Var131->symb != 23)
                                        {
                                        ptoc_Var135->xadesclen = (short ) sql__setrang((short ) (ptoc_Var131->syposacc - (int ) ptoc_Var135->xadescpos) , -32768 , 32767);
                                        (*next_symbol)(&(*apc) , sql__nil(ptoc_Var134->partbufp , "vpc14.p" , 8848) , ptoc_Var134->part1len);
                                        }
                                    }
                                else
                                    {
                                    if ((int ) ptoc_Var133->rakamode == 1)
                                        {
                                        ptoc_Var135->xadescpos = -1;
                                        }
                                    else
                                        {
                                        err = 37;
                                        }
                                    }
                                }
                            else
                                {
                                if ((int ) ptoc_Var131->symb != 13)
                                    {
                                    err = 33;
                                    }
                                else
                                    {
                                    p14an1sqlstatement(next_symbol , &(*apc));
                                    *statemno = (int ) ptoc_Var130->pcka;
                                    ptoc_Var130->sqlxa.xakano = (short ) sql__setrang(ptoc_Var130->pcka , -32768 , 32767);
                                    (*(sqlkatype *) sql__nil(ptoc_Var135->sqlkap , "vpc14.p" , 8874))[sql__setrang((int ) ptoc_Var130->pcka , 1 , 3000) + -1].kaprindex = (short ) sql__setrang((short ) prind , -32768 , 32767);
                                    (*(sqlkatype *) sql__nil(ptoc_Var135->sqlkap , "vpc14.p" , 8875))[sql__setrang((int ) ptoc_Var130->pcka , 1 , 3000) + -1].kaStcount = (short ) sql__setrang((*(sqlkatype *) sql__nil(ptoc_Var135->sqlkap , "vpc14.p" , 8876))[sql__setrang((int ) kaind , 1 , 3000) + -1].kaStcount , -32768 , 32767);
                                    (*(sqlkatype *) sql__nil(ptoc_Var135->sqlkap , "vpc14.p" , 8877))[sql__setrang((int ) ptoc_Var130->pcka , 1 , 3000) + -1].kaStindex = (short ) sql__setrang((*(sqlkatype *) sql__nil(ptoc_Var135->sqlkap , "vpc14.p" , 8878))[sql__setrang((int ) kaind , 1 , 3000) + -1].kaStindex , -32768 , 32767);
                                    (*(sqlprtype *) sql__nil(ptoc_Var135->sqlprp , "vpc14.p" , 8880))[sql__setrang(prind , 1 , 500) + -1].prkaindex = (short ) sql__setrang(ptoc_Var130->pcka , -32768 , 32767);
                                    (*(sqlkatype *) sql__nil(ptoc_Var135->sqlkap , "vpc14.p" , 8881))[sql__setrang((int ) ptoc_Var130->pcka , 1 , 3000) + -1].kadiindex = (short ) sql__setrang(kaind , -32768 , 32767);
                                    p19sqlfaallocate(&ptoc_Var130->pcfa);
                                    (*(sqlkatype *) sql__nil(ptoc_Var135->sqlkap , "vpc14.p" , 8883))[sql__setrang((int ) ptoc_Var130->pcka , 1 , 3000) + -1].kafaindex = (short ) sql__setrang(ptoc_Var130->pcfa , -32768 , 32767);
                                    if (ndindex > 0)
                                        {
                                        (*(tpc_arrndtype *) sql__nil(ptoc_Var136->ndtabp , "vpc14.p" , 8886))[sql__setrang(ndindex , 1 , 100) + -1].ndkano = (short ) sql__setrang(ptoc_Var130->pcka , -32768 , 32767);
                                        }
                                    }
                                }
                            }
                        else
                            {
                            err = 37;
                            }
                        }
                    }
                }
            }
        if ((int ) err != 0)
            {
            p11precomerror(&(*apc) , (unsigned char ) sql__setrang(err , 0 , 90));
            }
        }
    m90int2(14 , "xakano      " , (short ) sql__setrang(ptoc_Var135->xakano , -32768 , 32767));
    m90int2(14 , "xadescpos   " , (short ) sql__setrang(ptoc_Var135->xadescpos , -32768 , 32767));
    m90int2(14 , "xadesclen   " , (short ) sql__setrang(ptoc_Var135->xadesclen , -32768 , 32767));
    ptoc_Var131->symb = 4;
    m90name(22 , "END p14execute    ");
    }



int p14declare(next_symbol, apc, keyindex, statemno)
int (*next_symbol)();
tpc_globals *apc;
int *keyindex;
int *statemno;
    {
    tsp_knl_identifier statemn;
    int stateml;
    tsp_knl_identifier cursorn;
    int cursorl;
    int keyind;
    int ind;
    int kaind;
    tsp_int2 macro;
    tsp_int2 param;
    tsp_int2 va1ix;
    tsp_c12 ch12;
    int no;
    tpr_scannertype rscan;
    tsp_int2 StmtNameIndex;
    tsp_int2 retcom;
    char withhold;
    int ndindex;
    int ndcuindex;
    tsp_int2 CursorNameIndex;
    tsp_int2 retStmtNameIndex;
    tpc_globals *ptoc_Var147;
    tpr_scannertype *ptoc_Var148;
    sqlcatype *ptoc_Var149;
    sqlratype *ptoc_Var150;
    tpc_cmd_part *ptoc_Var151;
    tpc_pc_sqlva *ptoc_Var152;
    sqlxatype *ptoc_Var153;
    sqlprentry *ptoc_Var154;
    m90print("PC14  p14declare");
    ptoc_Var147 = &*apc;
    ptoc_Var148 = &ptoc_Var147->pcscan;
    ptoc_Var149 = &ptoc_Var147->sqlca;
    ptoc_Var150 = (sqlratype *) sql__nil(ptoc_Var149->sqlrap , "vpc14.p" , 8949);
    ptoc_Var151 = &ptoc_Var147->pccmdpart;
    ptoc_Var152 = &ptoc_Var147->pcsqlva;
    ndindex = 0;
    *keyindex = 0;
    withhold = 0;
    if ((int ) ptoc_Var147->pcerror == 0)
        {
        p19sqlprallocate(&ptoc_Var147->pcpr);
        (*next_symbol)(&(*apc) , sql__nil(ptoc_Var151->partbufp , "vpc14.p" , 8958) , ptoc_Var151->part1len);
        p10getkeyword(&(*apc) , sql__nil(ptoc_Var151->partbufp , "vpc14.p" , 8959) , &keyind);
        *keyindex = keyind;
        if (keyind != 122)
            {
            p14getname(next_symbol , &(*apc) , cursorn , &cursorl , &macro , &param);
            ptoc_Var153 = &ptoc_Var147->sqlxa;
            ptoc_Var154 = &(*(sqlprtype *) sql__nil(ptoc_Var153->sqlprp , "vpc14.p" , 8967))[sql__setrang((int ) ptoc_Var147->pcpr , 1 , 500) + -1];
            ptoc_Var154->prstcount = 0;
            ptoc_Var154->prstindex = 0;
            ptoc_Var154->prcuindex = 0;
            _ptoc_ASSIGN(unsigned char , 64, ptoc_Var154->prCursorName, "                                                                ")
            ptoc_Var154->prCursorNameIndex = 0;
            _ptoc_ASSIGN(unsigned char , 64, ptoc_Var154->prStmtName, "                                                                ")
            ptoc_Var154->prStmtNameIndex = 0;
            ptoc_Var154->prstate = 3;
            ptoc_Var154->prarea = 0;
            ptoc_Var154->prkaindex = 0;
            ptoc_Var154->prDescribe = 0;
            if ((int ) param == 1)
                {
                ind = (int ) ptoc_Var147->pcpr;
                rscan = ptoc_Var147->pcscan;
                p14_hostv_get(next_symbol , &(*apc) , &CursorNameIndex , &va1ix);
                no = (int ) va1ix;
                p05inttochr12(no , ch12);
                _ptoc_ASSIGN(unsigned char , 64, cursorn, "                                                                ")
                cursorn[0] = (unsigned char)':';
                s10mv18(12 , sizeof(unsigned char [64]) , ch12 , 2 , cursorn , 2 , 4);
                ptoc_Var147->pcscan = rscan;
                ndcuindex = (int ) ptoc_Var152->ndcnt;
                }
            else
                {
                CursorNameIndex = 0;
                ndcuindex = 0;
                }
            (*next_symbol)(&(*apc) , sql__nil(ptoc_Var151->partbufp , "vpc14.p" , 9002) , ptoc_Var151->part1len);
            p10getkeyword(&(*apc) , sql__nil(ptoc_Var151->partbufp , "vpc14.p" , 9003) , &keyind);
            *keyindex = keyind;
            if (keyind == 25 && ((int ) ptoc_Var150->rakamode == 4 || (int ) ptoc_Var150->rakamode == 5))
                {
                ptoc_Var147->pcpr = (short ) sql__setrang((short ) ((int ) ptoc_Var147->pcpr + -1) , -32768 , 32767);
                p19setprsqlindex(&ptoc_Var147->pcpr);
                p14batdatabase(&(*apc) , cursorn);
                }
            else
                {
                if (keyind == 117)
                    {
                    ptoc_Var154->prstate = 10;
                    _ptoc_ASSIGN(unsigned char , 64, ptoc_Var154->prStmtName, cursorn)
                    }
                else
                    {
                    if (keyind != 24)
                        {
                        ptoc_Var147->pcpr = (short ) sql__setrang((short ) ((int ) ptoc_Var147->pcpr + -1) , -32768 , 32767);
                        p19setprsqlindex(&ptoc_Var147->pcpr);
                        p11precomerror(&(*apc) , 37);
                        }
                    else
                        {
                        (*next_symbol)(&(*apc) , sql__nil(ptoc_Var151->partbufp , "vpc14.p" , 9033) , ptoc_Var151->part1len);
                        p10getkeyword(&(*apc) , sql__nil(ptoc_Var151->partbufp , "vpc14.p" , 9034) , &keyind);
                        if (keyind != 44)
                            {
                            p10getkeyword(&(*apc) , sql__nil(ptoc_Var151->partbufp , "vpc14.p" , 9038) , &keyind);
                            if (keyind != 45)
                                {
                                p11precomerror(&(*apc) , 37);
                                }
                            (*next_symbol)(&(*apc) , sql__nil(ptoc_Var151->partbufp , "vpc14.p" , 9045) , ptoc_Var151->part1len);
                            p10getkeyword(&(*apc) , sql__nil(ptoc_Var151->partbufp , "vpc14.p" , 9046) , &keyind);
                            if (keyind != 84)
                                {
                                p11precomerror(&(*apc) , 37);
                                }
                            (*next_symbol)(&(*apc) , sql__nil(ptoc_Var151->partbufp , "vpc14.p" , 9053) , ptoc_Var151->part1len);
                            p10getkeyword(&(*apc) , sql__nil(ptoc_Var151->partbufp , "vpc14.p" , 9054) , &keyind);
                            if (keyind != 44)
                                {
                                p11precomerror(&(*apc) , 37);
                                }
                            withhold = 1;
                            if ((int ) ptoc_Var150->rakamode != 2)
                                {
                                p11precomerror(&(*apc) , 37);
                                }
                            }
                        (*next_symbol)(&(*apc) , sql__nil(ptoc_Var151->partbufp , "vpc14.p" , 9068) , ptoc_Var151->part1len);
                        p10getkeyword(&(*apc) , sql__nil(ptoc_Var151->partbufp , "vpc14.p" , 9069) , &keyind);
                        if (keyind == 95 || keyind == 45 || (int ) ptoc_Var148->symb == 10 || (int ) ptoc_Var148->symb == 9)
                            {
                            ptoc_Var147->pcan.ancheck = 0;
                            retcom = (short ) sql__setrang(ptoc_Var147->pcan.ancomtyp , -32768 , 32767);
                            ptoc_Var147->pcan.ancomtyp = 22;
                            p14an1sqlstatement(next_symbol , &(*apc));
                            ptoc_Var147->pcan.ancomtyp = (short ) sql__setrang(retcom , -32768 , 32767);
                            ptoc_Var147->pcan.ancheck = (short ) sql__setrang(ptoc_Var147->pcopts.tsp4_component.C_sp4co_sql_precompiler.opt_mode , -32768 , 32767);
                            *statemno = (int ) ptoc_Var147->pcka;
                            (*(sqlkatype *) sql__nil(ptoc_Var153->sqlkap , "vpc14.p" , 9084))[sql__setrang((int ) ptoc_Var147->pcka , 1 , 3000) + -1].katyp = (short ) sql__setrang(retcom , -32768 , 32767);
                            (*(sqlkatype *) sql__nil(ptoc_Var153->sqlkap , "vpc14.p" , 9085))[sql__setrang((int ) ptoc_Var147->pcka , 1 , 3000) + -1].kaprindex = (short ) sql__setrang(ptoc_Var147->pcpr , -32768 , 32767);
                            (*(sqlkatype *) sql__nil(ptoc_Var153->sqlkap , "vpc14.p" , 9086))[sql__setrang((int ) ptoc_Var147->pcka , 1 , 3000) + -1].kamacro = (short ) sql__setrang(macro , -32768 , 32767);
                            ptoc_Var154->prkaindex = (short ) sql__setrang(ptoc_Var147->pcka , -32768 , 32767);
                            ptoc_Var154->prarea = 1;
                            ptoc_Var154->prStmtNameIndex = 0;
                            _ptoc_ASSIGN(unsigned char , 64, ptoc_Var154->prCursorName, cursorn)
                            ptoc_Var154->prCursorNameIndex = (short ) sql__setrang(CursorNameIndex , -32768 , 32767);
                            p14putsqlstarea(&(*apc));
                            }
                        else
                            {
                            if ((int ) ptoc_Var148->symb == 13)
                                {
                                rscan = ptoc_Var147->pcscan;
                                p14_hostv_get(next_symbol , &(*apc) , &StmtNameIndex , &va1ix);
                                no = (int ) va1ix;
                                p05inttochr12(no , ch12);
                                _ptoc_ASSIGN(unsigned char , 64, statemn, "                                                                ")
                                statemn[0] = (unsigned char)':';
                                s10mv18(12 , sizeof(unsigned char [64]) , ch12 , 2 , statemn , 2 , 4);
                                ptoc_Var147->pcscan = rscan;
                                ndindex = (int ) ptoc_Var152->ndcnt;
                                }
                            else
                                {
                                ndindex = 0;
                                p19getname(&(*apc) , statemn , &stateml);
                                if ((int ) (*(tpc_partbuffer *) sql__nil(ptoc_Var151->partbufp , "vpc14.p" , 9115))[sql__setrang(ptoc_Var148->sypos + -1 , 1 , 16000) + -1] != 34)
                                    {
                                    p05up2casebuf(statemn , 1 , stateml);
                                    }
                                p19sprepareprname(&(*apc) , statemn , &ind);
                                StmtNameIndex = 0;
                                }
                            if (ind == 0)
                                {
                                p11precomerror(&(*apc) , 82);
                                }
                            else
                                {
                                p14kaentryinit(&(*apc));
                                *statemno = (int ) ptoc_Var147->pcka;
                                _ptoc_ASSIGN(unsigned char , 64, ptoc_Var154->prCursorName, cursorn)
                                ptoc_Var154->prCursorNameIndex = (short ) sql__setrang(CursorNameIndex , -32768 , 32767);
                                _ptoc_ASSIGN(unsigned char , 64, ptoc_Var154->prStmtName, statemn)
                                ptoc_Var154->prStmtNameIndex = (short ) sql__setrang(StmtNameIndex , -32768 , 32767);
                                if ((char ) sql__setrang(withhold , 0 , 1))
                                    {
                                    ptoc_Var154->prstate = 11;
                                    }
                                else
                                    {
                                    ptoc_Var154->prstate = 3;
                                    }
                                ptoc_Var154->prkaindex = (short ) sql__setrang(ptoc_Var147->pcka , -32768 , 32767);
                                (*(sqlkatype *) sql__nil(ptoc_Var153->sqlkap , "vpc14.p" , 9142))[sql__setrang((int ) ptoc_Var147->pcka , 1 , 3000) + -1].kaprindex = (short ) sql__setrang(ptoc_Var147->pcpr , -32768 , 32767);
                                (*(sqlkatype *) sql__nil(ptoc_Var153->sqlkap , "vpc14.p" , 9143))[sql__setrang((int ) ptoc_Var147->pcka , 1 , 3000) + -1].kamacro = (short ) sql__setrang(macro , -32768 , 32767);
                                (*(sqlkatype *) sql__nil(ptoc_Var153->sqlkap , "vpc14.p" , 9144))[sql__setrang((int ) ptoc_Var147->pcka , 1 , 3000) + -1].kapacount = 0;
                                if ((int ) ptoc_Var147->pcinpind == 2)
                                    {
                                    (*(sqlkatype *) sql__nil(ptoc_Var153->sqlkap , "vpc14.p" , 9147))[sql__setrang((int ) ptoc_Var147->pcka , 1 , 3000) + -1].kalineno = (short ) sql__setrang(ptoc_Var147->pclno.lnosrc , -32768 , 32767);
                                    }
                                else
                                    {
                                    (*(sqlkatype *) sql__nil(ptoc_Var153->sqlkap , "vpc14.p" , 9150))[sql__setrang((int ) ptoc_Var147->pcka , 1 , 3000) + -1].kalineno = (short ) sql__setrang((short ) -(int ) ptoc_Var147->pclno.lnoexp , -32768 , 32767);
                                    }
                                (*(sqlprtype *) sql__nil(ptoc_Var153->sqlprp , "vpc14.p" , 9153))[sql__setrang((int ) ptoc_Var147->pcpr , 1 , 500) + -1].prcuindex = (short ) sql__setrang((short ) ind , -32768 , 32767);
                                (*(sqlkatype *) sql__nil(ptoc_Var153->sqlkap , "vpc14.p" , 9154))[sql__setrang((int ) ptoc_Var147->pcka , 1 , 3000) + -1].kafaindex = 0;
                                if (ndcuindex > 0)
                                    {
                                    (*(tpc_arrndtype *) sql__nil(ptoc_Var152->ndtabp , "vpc14.p" , 9158))[sql__setrang(ndcuindex , 1 , 100) + -1].ndkano = (short ) sql__setrang(ptoc_Var147->pcka , -32768 , 32767);
                                    }
                                if (ndindex > 0)
                                    {
                                    (*(tpc_arrndtype *) sql__nil(ptoc_Var152->ndtabp , "vpc14.p" , 9162))[sql__setrang(ndindex , 1 , 100) + -1].ndkano = (short ) sql__setrang(ptoc_Var147->pcka , -32768 , 32767);
                                    }
                                }
                            }
                        p14bcudeclare(&(*apc) , cursorn);
                        }
                    }
                }
            }
        }
    while ((int ) ptoc_Var148->symb != 4 && (int ) ptoc_Var148->symb != 23)
        (*next_symbol)(&(*apc) , sql__nil(ptoc_Var151->partbufp , "vpc14.p" , 9181) , ptoc_Var151->part1len);
    m90name(22 , "END   p14decla    ");
    }



int p14describe(next_symbol, apc)
int (*next_symbol)();
tpc_globals *apc;
    {
    int keyind;
    int ind;
    tsp_knl_identifier staten;
    int statel;
    tsp_int2 orakatyp;
    int ndindex;
    tpc_globals *ptoc_Var155;
    tpr_scannertype *ptoc_Var156;
    sqlcatype *ptoc_Var157;
    sqlratype *ptoc_Var158;
    tpc_cmd_part *ptoc_Var159;
    sqlxatype *ptoc_Var160;
    tpc_pc_sqlva *ptoc_Var161;
    sqlxatype *ptoc_Var162;
    sqlprentry *ptoc_Var163;
    sqlkaentry *ptoc_Var164;
    m90print("PC14  p14describ");
    ptoc_Var155 = &*apc;
    ptoc_Var156 = &ptoc_Var155->pcscan;
    ptoc_Var157 = &ptoc_Var155->sqlca;
    ptoc_Var158 = (sqlratype *) sql__nil(ptoc_Var157->sqlrap , "vpc14.p" , 9205);
    ptoc_Var159 = &ptoc_Var155->pccmdpart;
    ptoc_Var160 = &ptoc_Var155->sqlxa;
    ptoc_Var161 = &ptoc_Var155->pcsqlva;
    ptoc_Var160->xadescpos = 0;
    ptoc_Var160->xadesclen = 0;
    if ((int ) ptoc_Var155->pcerror == 0)
        {
        (*next_symbol)(&(*apc) , sql__nil(ptoc_Var159->partbufp , "vpc14.p" , 9212) , ptoc_Var159->part1len);
        orakatyp = 0;
        if ((int ) ptoc_Var158->rakamode == 4 || (int ) ptoc_Var158->rakamode == 5)
            {
            p14oradescribe(next_symbol , &(*apc) , &orakatyp);
            }
        if ((int ) ptoc_Var156->symb == 13 && (int ) ptoc_Var155->pcerror == 0)
            {
            p14_prind_get(next_symbol , &(*apc) , &ind);
            ndindex = (int ) ptoc_Var161->ndcnt;
            }
        else
            {
            ndindex = 0;
            p19getname(&(*apc) , staten , &statel);
            if ((int ) (*(tpc_partbuffer *) sql__nil(ptoc_Var159->partbufp , "vpc14.p" , 9230))[sql__setrang(ptoc_Var156->sypos + -1 , 1 , 16000) + -1] != 34)
                {
                p05up2casebuf(staten , 1 , statel);
                }
            p19sstatementprname(&(*apc) , staten , &ind);
            (*next_symbol)(&(*apc) , sql__nil(ptoc_Var159->partbufp , "vpc14.p" , 9235) , ptoc_Var159->part1len);
            }
        if (ind == 0)
            {
            p19scursorprname(&(*apc) , staten , &ind);
            }
        if (ind == 0)
            {
            p11precomerror(&(*apc) , 82);
            }
        else
            {
            ptoc_Var162 = &ptoc_Var155->sqlxa;
            ptoc_Var163 = &(*(sqlprtype *) sql__nil(ptoc_Var162->sqlprp , "vpc14.p" , 9246))[sql__setrang(ind , 1 , 500) + -1];
            p14kaentryinit(&(*apc));
            ptoc_Var155->pcda = 300;
            if ((int ) ptoc_Var163->prkaindex == 0)
                {
                ptoc_Var163->prkaindex = (short ) sql__setrang(ptoc_Var155->pcka , -32768 , 32767);
                ptoc_Var164 = &(*(sqlkatype *) sql__nil(ptoc_Var162->sqlkap , "vpc14.p" , 9256))[sql__setrang((int ) ptoc_Var163->prkaindex , 1 , 3000) + -1];
                if ((int ) orakatyp == 0)
                    {
                    ptoc_Var164->katyp = (short ) sql__setrang(ptoc_Var155->pcan.ancomtyp , -32768 , 32767);
                    }
                else
                    {
                    ptoc_Var164->katyp = (short ) sql__setrang(orakatyp , -32768 , 32767);
                    }
                ptoc_Var164->kaprindex = (short ) sql__setrang((short ) ind , -32768 , 32767);
                ptoc_Var164->kapacount = -3;
                }
            else
                {
                (*(sqlkatype *) sql__nil(ptoc_Var162->sqlkap , "vpc14.p" , 9271))[sql__setrang((int ) ptoc_Var155->pcka , 1 , 3000) + -1] = (*(sqlkatype *) sql__nil(ptoc_Var162->sqlkap , "vpc14.p" , 9271))[sql__setrang((int ) ptoc_Var163->prkaindex , 1 , 3000) + -1];
                if ((int ) orakatyp == 0)
                    {
                    (*(sqlkatype *) sql__nil(ptoc_Var162->sqlkap , "vpc14.p" , 9274))[sql__setrang((int ) ptoc_Var155->pcka , 1 , 3000) + -1].katyp = (short ) sql__setrang(ptoc_Var155->pcan.ancomtyp , -32768 , 32767);
                    }
                else
                    {
                    (*(sqlkatype *) sql__nil(ptoc_Var162->sqlkap , "vpc14.p" , 9276))[sql__setrang((int ) ptoc_Var155->pcka , 1 , 3000) + -1].katyp = (short ) sql__setrang(orakatyp , -32768 , 32767);
                    }
                (*(sqlkatype *) sql__nil(ptoc_Var162->sqlkap , "vpc14.p" , 9278))[sql__setrang((int ) ptoc_Var155->pcka , 1 , 3000) + -1].kapacount = -3;
                (*(sqlkatype *) sql__nil(ptoc_Var162->sqlkap , "vpc14.p" , 9279))[sql__setrang((int ) ptoc_Var155->pcka , 1 , 3000) + -1].kapaindex = 0;
                if ((int ) ptoc_Var155->pcinpind == 2)
                    {
                    (*(sqlkatype *) sql__nil(ptoc_Var162->sqlkap , "vpc14.p" , 9282))[sql__setrang((int ) ptoc_Var155->pcka , 1 , 3000) + -1].kalineno = (short ) sql__setrang(ptoc_Var155->pclno.lnosrc , -32768 , 32767);
                    }
                else
                    {
                    (*(sqlkatype *) sql__nil(ptoc_Var162->sqlkap , "vpc14.p" , 9284))[sql__setrang((int ) ptoc_Var155->pcka , 1 , 3000) + -1].kalineno = (short ) sql__setrang((short ) -(int ) ptoc_Var155->pclno.lnoexp , -32768 , 32767);
                    }
                (*(sqlkatype *) sql__nil(ptoc_Var162->sqlkap , "vpc14.p" , 9286))[sql__setrang((int ) ptoc_Var155->pcka , 1 , 3000) + -1].kamode = (short ) sql__setrang(ptoc_Var158->rakamode , -32768 , 32767);
                (*(sqlkatype *) sql__nil(ptoc_Var162->sqlkap , "vpc14.p" , 9287))[sql__setrang((int ) ptoc_Var155->pcka , 1 , 3000) + -1].kaatindex = (short ) sql__setrang(ptoc_Var155->pckaatind , -32768 , 32767);
                (*(sqlkatype *) sql__nil(ptoc_Var162->sqlkap , "vpc14.p" , 9288))[sql__setrang((int ) ptoc_Var155->pcka , 1 , 3000) + -1].kacuindex = 0;
                (*(sqlkatype *) sql__nil(ptoc_Var162->sqlkap , "vpc14.p" , 9290))[sql__setrang((int ) ptoc_Var155->pcka , 1 , 3000) + -1].kadiindex = (short ) sql__setrang(ptoc_Var163->prkaindex , -32768 , 32767);
                (*(sqlkatype *) sql__nil(ptoc_Var162->sqlkap , "vpc14.p" , 9291))[sql__setrang((int ) ptoc_Var155->pcka , 1 , 3000) + -1].kafaindex = 0;
                (*(sqlprtype *) sql__nil(ptoc_Var162->sqlprp , "vpc14.p" , 9292))[sql__setrang(ind , 1 , 500) + -1].prkaindex = (short ) sql__setrang(ptoc_Var155->pcka , -32768 , 32767);
                (*(sqlkatype *) sql__nil(ptoc_Var162->sqlkap , "vpc14.p" , 9293))[sql__setrang((int ) ptoc_Var155->pcka , 1 , 3000) + -1].kaprindex = (short ) sql__setrang((short ) ind , -32768 , 32767);
                }
            ptoc_Var155->pcan.ancheck = (short ) sql__setrang(ptoc_Var155->pcopts.tsp4_component.C_sp4co_sql_precompiler.opt_mode , -32768 , 32767);
            ptoc_Var155->sqlxa.xakano = (short ) sql__setrang(ptoc_Var163->prkaindex , -32768 , 32767);
            if (ndindex > 0)
                {
                (*(tpc_arrndtype *) sql__nil(ptoc_Var161->ndtabp , "vpc14.p" , 9300))[sql__setrang(ndindex , 1 , 100) + -1].ndkano = (short ) sql__setrang(ptoc_Var163->prkaindex , -32768 , 32767);
                }
            p10getkeyword(&(*apc) , sql__nil(ptoc_Var159->partbufp , "vpc14.p" , 9302) , &keyind);
            if ((int ) ptoc_Var156->symb == 4 || (int ) ptoc_Var156->symb == 23)
                {
                if ((int ) ptoc_Var158->rakamode == 1)
                    {
                    ptoc_Var162->xadescpos = -1;
                    }
                else
                    {
                    p11precomerror(&(*apc) , 37);
                    }
                }
            else
                {
                if (keyind == 56)
                    {
                    (*next_symbol)(&(*apc) , sql__nil(ptoc_Var159->partbufp , "vpc14.p" , 9320) , ptoc_Var159->part1len);
                    if ((int ) (*(tpc_partbuffer *) sql__nil(ptoc_Var159->partbufp , "vpc14.p" , 9321))[sql__setrang(ptoc_Var156->sypos , 1 , 16000) + -1] == 58)
                        {
                        ptoc_Var162->xadescpos = (short ) sql__setrang((short ) (ptoc_Var156->sypos + 1) , -32768 , 32767);
                        }
                    else
                        {
                        ptoc_Var162->xadescpos = (short ) sql__setrang((short ) ptoc_Var156->sypos , -32768 , 32767);
                        }
                    while ((int ) ptoc_Var156->symb != 4 && (int ) ptoc_Var156->symb != 23 && keyind != 132)
                        {
                        ptoc_Var162->xadesclen = (short ) sql__setrang((short ) (ptoc_Var156->syposacc - (int ) ptoc_Var162->xadescpos) , -32768 , 32767);
                        (*next_symbol)(&(*apc) , sql__nil(ptoc_Var159->partbufp , "vpc14.p" , 9331) , ptoc_Var159->part1len);
                        p10getkeyword(&(*apc) , sql__nil(ptoc_Var159->partbufp , "vpc14.p" , 9332) , &keyind);
                        }
                    }
                else
                    {
                    p11precomerror(&(*apc) , 37);
                    }
                if (keyind == 132)
                    {
                    (*next_symbol)(&(*apc) , sql__nil(ptoc_Var159->partbufp , "vpc14.p" , 9342) , ptoc_Var159->part1len);
                    p10getkeyword(&(*apc) , sql__nil(ptoc_Var159->partbufp , "vpc14.p" , 9343) , &keyind);
                    if (keyind != 67 && keyind != 59 && keyind != 3)
                        {
                        p11precomerror(&(*apc) , 60);
                        }
                    }
                }
            }
        }
    while ((int ) ptoc_Var156->symb != 4 && (int ) ptoc_Var156->symb != 23)
        (*next_symbol)(&(*apc) , sql__nil(ptoc_Var159->partbufp , "vpc14.p" , 9360) , ptoc_Var159->part1len);
    m90int2(14 , "xakano      " , (short ) sql__setrang(ptoc_Var160->xakano , -32768 , 32767));
    m90int2(14 , "xadescpos   " , (short ) sql__setrang(ptoc_Var160->xadescpos , -32768 , 32767));
    m90int2(14 , "xadesclen   " , (short ) sql__setrang(ptoc_Var160->xadesclen , -32768 , 32767));
    m90name(22 , "END p14describe   ");
    }



int p14close(next_symbol, apc)
int (*next_symbol)();
tpc_globals *apc;
    {
    tsp_int2 retpos;
    tsp_int2 macro;
    tsp_int2 param;
    tsp_knl_identifier cursorn;
    int cursorl;
    tpr_scannertype rscan;
    tsp_int2 va1ix;
    tsp_c12 ch12;
    int no;
    tsp_int2 CursorNameIndex;
    int prind;
    tsp_int2 paix;
    int ndcuindex;
    tpc_globals *ptoc_Var165;
    tpr_scannertype *ptoc_Var166;
    sqlcatype *ptoc_Var167;
    sqlratype *ptoc_Var168;
    tpc_cmd_part *ptoc_Var169;
    sqlxatype *ptoc_Var170;
    sqlxatype *ptoc_Var171;
    sqlprentry *ptoc_Var172;
    sqlxatype *ptoc_Var173;
    sqlprentry *ptoc_Var174;
    m90print("PC14  p14close  ");
    ptoc_Var165 = &*apc;
    ptoc_Var166 = &ptoc_Var165->pcscan;
    ptoc_Var167 = &ptoc_Var165->sqlca;
    ptoc_Var168 = (sqlratype *) sql__nil(ptoc_Var167->sqlrap , "vpc14.p" , 9394);
    ptoc_Var169 = &ptoc_Var165->pccmdpart;
    ptoc_Var170 = &ptoc_Var165->sqlxa;
    retpos = (short ) sql__setrang((short ) ptoc_Var166->syposacc , -32768 , 32767);
    while ((int ) ptoc_Var166->symb != 4 && (int ) ptoc_Var166->symb != 23)
        (*next_symbol)(&(*apc) , sql__nil(ptoc_Var169->partbufp , "vpc14.p" , 9399) , ptoc_Var169->part1len);
    ptoc_Var166->syposacc = (int ) retpos;
    ptoc_Var166->symb = 0;
    (*next_symbol)(&(*apc) , sql__nil(ptoc_Var169->partbufp , "vpc14.p" , 9404) , ptoc_Var169->part1len);
    p14getname(next_symbol , &(*apc) , cursorn , &cursorl , &macro , &param);
    ptoc_Var171 = &ptoc_Var165->sqlxa;
    ptoc_Var172 = &(*(sqlprtype *) sql__nil(ptoc_Var171->sqlprp , "vpc14.p" , 9406))[sql__setrang((int ) ptoc_Var165->pcpr , 1 , 500) + -1];
    paix = 0;
    p14_add_prind(&(*apc) , &prind , &paix);
    ptoc_Var165->pcpr = (short ) sql__setrang((short ) prind , -32768 , 32767);
    ptoc_Var173 = &ptoc_Var165->sqlxa;
    ptoc_Var174 = &(*(sqlprtype *) sql__nil(ptoc_Var173->sqlprp , "vpc14.p" , 9411))[sql__setrang((int ) ptoc_Var165->pcpr , 1 , 500) + -1];
    (*(sqlprtype *) sql__nil(ptoc_Var173->sqlprp , "vpc14.p" , 9413))[sql__setrang((int ) ptoc_Var165->pcpr , 1 , 500) + -1].prarea = 1;
    (*(sqlprtype *) sql__nil(ptoc_Var173->sqlprp , "vpc14.p" , 9414))[sql__setrang((int ) ptoc_Var165->pcpr , 1 , 500) + -1].prDescribe = 0;
    ptoc_Var165->pcan.ancomtyp = 5;
    ptoc_Var165->pcusage.uscom = 2;
    if ((int ) param == 1)
        {
        ptoc_Var165->pcan.ancheck = 0;
        rscan = ptoc_Var165->pcscan;
        p14_hostv_get(next_symbol , &(*apc) , &CursorNameIndex , &va1ix);
        no = (int ) va1ix;
        p05inttochr12(no , ch12);
        _ptoc_ASSIGN(unsigned char , 64, cursorn, "                                                                ")
        cursorn[0] = (unsigned char)':';
        s10mv18(12 , sizeof(unsigned char [64]) , ch12 , 2 , cursorn , 2 , 4);
        ptoc_Var165->pcscan = rscan;
        ndcuindex = (int ) ptoc_Var165->pcsqlva.ndcnt;
        _ptoc_ASSIGN(unsigned char , 64, (*(sqlprtype *) sql__nil(ptoc_Var171->sqlprp , "vpc14.p" , 9434))[sql__setrang((int ) ptoc_Var165->pcpr , 1 , 500) + -1].prCursorName, cursorn)
        (*(sqlprtype *) sql__nil(ptoc_Var171->sqlprp , "vpc14.p" , 9435))[sql__setrang((int ) ptoc_Var165->pcpr , 1 , 500) + -1].prCursorNameIndex = (short ) sql__setrang(CursorNameIndex , -32768 , 32767);
        }
    else
        {
        _ptoc_ASSIGN(unsigned char , 64, (*(sqlprtype *) sql__nil(ptoc_Var165->sqlxa.sqlprp , "vpc14.p" , 9439))[sql__setrang((int ) ptoc_Var165->pcpr , 1 , 500) + -1].prCursorName, cursorn)
        ndcuindex = 0;
        }
    if (ndcuindex > 0)
        {
        (*(tpc_arrndtype *) sql__nil(ptoc_Var165->pcsqlva.ndtabp , "vpc14.p" , 9446))[sql__setrang(ndcuindex , 1 , 100) + -1].ndkano = (short ) sql__setrang((short ) ((int ) ptoc_Var165->pcka + 1) , -32768 , 32767);
        }
    m90name(22 , "END p14close      ");
    }



int p14forloop(next_symbol, apc)
int (*next_symbol)();
tpc_globals *apc;
    {
    int retposfor;
    int retpos;
    tsp_int2 itypentix;
    tsp_int2 ivatyp;
    tsp_int2 iind;
    tsp_int2 loopcnt;
    int keyind;
    int i;
    tsp_int2 lastvatyp;
    int statemno;
    int immediate;
    tpc_pre_errors loopcnterr;
    tpc_globals *ptoc_Var175;
    tpr_scannertype *ptoc_Var176;
    sqlcatype *ptoc_Var177;
    sqlratype *ptoc_Var178;
    tpc_cmd_part *ptoc_Var179;
    tpc_pc_sqlva *ptoc_Var180;
    sqlxatype *ptoc_Var181;
    int ptoc_Var182;
    int ptoc_Var183;
    m90print("PC14  p14forloop");
    ptoc_Var175 = &*apc;
    ptoc_Var176 = &ptoc_Var175->pcscan;
    ptoc_Var177 = &ptoc_Var175->sqlca;
    ptoc_Var178 = (sqlratype *) sql__nil(ptoc_Var177->sqlrap , "vpc14.p" , 9478);
    ptoc_Var179 = &ptoc_Var175->pccmdpart;
    ptoc_Var180 = &ptoc_Var175->pcsqlva;
    ptoc_Var181 = &ptoc_Var175->sqlxa;
    statemno = 0;
    retposfor = ptoc_Var176->sypos;
    retpos = ptoc_Var176->syposacc;
    loopcnterr = 0;
    if ((ptoc_Var182 = (int ) ptoc_Var178->ralang) == 3 || ptoc_Var182 == 4)
        {
        while ((int ) ptoc_Var176->symb != 4 && (int ) ptoc_Var176->symb != 23)
            (*next_symbol)(&(*apc) , sql__nil(ptoc_Var179->partbufp , "vpc14.p" , 9488) , ptoc_Var179->part1len);
        }
    if ((int ) ptoc_Var178->ralang == 4)
        {
        ptoc_Var179->part1len = ptoc_Var179->part1len + 1;
        (*(tpc_partbuffer *) sql__nil(ptoc_Var179->partbufp , "vpc14.p" , 9495))[sql__setrang(ptoc_Var179->part1len , 1 , 16000) + -1] = (unsigned char)'\0';
        }
    ptoc_Var176->symb = 0;
    ptoc_Var176->syposacc = retpos;
    m90buf(14 , &(*(tpc_partbuffer *) sql__nil(ptoc_Var179->partbufp , "vpc14.p" , 9500))[0] , 1 , ptoc_Var179->part1len);
    if ((int ) ptoc_Var175->pcerror == 0)
        {
        (*next_symbol)(&(*apc) , sql__nil(ptoc_Var179->partbufp , "vpc14.p" , 9504) , ptoc_Var179->part1len);
        if ((int ) ptoc_Var176->symb == 13)
            {
            ptoc_Var176->syposacc = ptoc_Var176->sypos + 1;
            itypentix = 0;
            p14btypentget(next_symbol , &(*apc) , &itypentix , &ivatyp , &lastvatyp , 1);
            if ((int ) ivatyp == -1 || (int ) ivatyp == 19)
                {
                p11wrerrorlisting(&(*apc) , 37);
                }
            else
                {
                if ((int ) itypentix != 0)
                    {
                    if (ptoc_Var175->pcpaloop == 0)
                        {
                        iind = (short ) sql__setrang((short ) (*(tpc_arrndtype *) sql__nil(ptoc_Var180->ndtabp , "vpc14.p" , 9525))[sql__setrang((int ) ptoc_Var180->ndcnt , 1 , 100) + -1].ndsqlva1ix , -32768 , 32767);
                        p19cparentry(&(*apc) , 0 , -(int ) iind , 0 , 0);
                        ptoc_Var175->pcpaloop = -(int ) ptoc_Var175->pcpa;
                        (*(tpc_arrndtype *) sql__nil(ptoc_Var180->ndtabp , "vpc14.p" , 9529))[sql__setrang((int ) ptoc_Var180->ndcnt , 1 , 100) + -1].ndkano = (short ) sql__setrang((short ) ((int ) ptoc_Var175->pcka + 1) , -32768 , 32767);
                        ptoc_Var182 = retposfor;
                        ptoc_Var183 = ptoc_Var176->sypos + -1;
                        if (ptoc_Var182 <= ptoc_Var183)
                            {
                            i = ptoc_Var182;
                            do
                                {
                                (*(tpc_partbuffer *) sql__nil(ptoc_Var179->partbufp , "vpc14.p" , 9531))[sql__setrang(i , 1 , 16000) + -1] = (unsigned char)' ';
                                if (i == ptoc_Var183)
                                    {
                                    break;
                                    }
                                i += 1;
                                }
                            while (!0);
                            }
                        }
                    }
                }
            }
        else
            {
            if ((int ) ptoc_Var176->symb == 22)
                {
                p10int2unsignedget(&(*apc) , sql__nil(ptoc_Var179->partbufp , "vpc14.p" , 9545) , &loopcnt);
                if ((int ) loopcnt < 1)
                    {
                    p11wrerrorlisting(&(*apc) , 57);
                    loopcnterr = 57;
                    }
                p19cparentry(&(*apc) , 0 , (int ) loopcnt , (int ) loopcnt , 0);
                ptoc_Var175->pcpaloop = -(int ) ptoc_Var175->pcpa;
                ptoc_Var183 = retposfor;
                ptoc_Var182 = ptoc_Var176->syposacc + -1;
                if (ptoc_Var183 <= ptoc_Var182)
                    {
                    i = ptoc_Var183;
                    do
                        {
                        (*(tpc_partbuffer *) sql__nil(ptoc_Var179->partbufp , "vpc14.p" , 9557))[sql__setrang(i , 1 , 16000) + -1] = (unsigned char)' ';
                        if (i == ptoc_Var182)
                            {
                            break;
                            }
                        i += 1;
                        }
                    while (!0);
                    }
                (*next_symbol)(&(*apc) , sql__nil(ptoc_Var179->partbufp , "vpc14.p" , 9559) , ptoc_Var179->part1len);
                }
            else
                {
                p11wrerrorlisting(&(*apc) , 37);
                }
            }
        }
    if ((int ) ptoc_Var175->pcopts.tsp4_component.C_sp4co_sql_precompiler.opt_comp == 0)
        {
        p11wrerrorlisting(&(*apc) , 18);
        }
    retpos = ptoc_Var176->syposacc;
    p10getkeyword(&(*apc) , sql__nil(ptoc_Var179->partbufp , "vpc14.p" , 9574) , &keyind);
    switch (keyind)
        {
        case 76 :
            p14open(next_symbol , &(*apc) , &statemno);
            if (statemno > 0)
                {
                if ((int ) (*(sqlkatype *) sql__nil(ptoc_Var175->sqlxa.sqlkap , "vpc14.p" , 9581))[sql__setrang(statemno , 1 , 3000) + -1].kapacount < 0)
                    {
                    (*(sqlkatype *) sql__nil(ptoc_Var175->sqlxa.sqlkap , "vpc14.p" , 9583))[sql__setrang(statemno , 1 , 3000) + -1].kapacount = (short ) sql__setrang((short ) ((int ) (*(sqlkatype *) sql__nil(ptoc_Var175->sqlxa.sqlkap , "vpc14.p" , 9584))[sql__setrang(statemno , 1 , 3000) + -1].kapacount + -1) , -32768 , 32767);
                    }
                }
            keyind = 76;
            ptoc_Var181->xakano = (short ) sql__setrang((short ) statemno , -32768 , 32767);
            break;
        case 39 :
            p14execute(next_symbol , &(*apc) , &statemno , &immediate);
            if (statemno > 0)
                {
                if ((int ) (*(sqlkatype *) sql__nil(ptoc_Var175->sqlxa.sqlkap , "vpc14.p" , 9595))[sql__setrang(statemno , 1 , 3000) + -1].kapacount < 0)
                    {
                    (*(sqlkatype *) sql__nil(ptoc_Var175->sqlxa.sqlkap , "vpc14.p" , 9597))[sql__setrang(statemno , 1 , 3000) + -1].kapacount = (short ) sql__setrang((short ) ((int ) (*(sqlkatype *) sql__nil(ptoc_Var175->sqlxa.sqlkap , "vpc14.p" , 9598))[sql__setrang(statemno , 1 , 3000) + -1].kapacount + -1) , -32768 , 32767);
                    }
                }
            keyind = 39;
            ptoc_Var181->xakano = (short ) sql__setrang((short ) statemno , -32768 , 32767);
            break;
        case 41 :
            p14fetch(next_symbol , &(*apc) , &keyind , &retpos);
            if (keyind == 33)
                {
                ptoc_Var176->syposacc = retpos;
                p14ansqlstatement(next_symbol , &(*apc));
                (*(sqlkatype *) sql__nil(ptoc_Var175->sqlxa.sqlkap , "vpc14.p" , 9612))[sql__setrang((int ) ptoc_Var175->pcka , 1 , 3000) + -1].kapacount = -3;
                ptoc_Var175->pcda = 300;
                }
            else
                {
                ptoc_Var176->syposacc = retpos;
                p14ansqlstatement(next_symbol , &(*apc));
                }
            break;
        case 148 :
            p14putval(next_symbol , &(*apc) , &keyind , &retpos);
            p14ansqlstatement(next_symbol , &(*apc));
            if (keyind == 33)
                {
                (*(sqlkatype *) sql__nil(ptoc_Var175->sqlxa.sqlkap , "vpc14.p" , 9629))[sql__setrang((int ) ptoc_Var175->pcka , 1 , 3000) + -1].kapacount = -2;
                ptoc_Var175->pcda = 300;
                }
            break;
        case 147 :
            p14getval(next_symbol , &(*apc) , &keyind , &retpos);
            if (keyind == 33)
                {
                ptoc_Var176->syposacc = retpos;
                p14ansqlstatement(next_symbol , &(*apc));
                (*(sqlkatype *) sql__nil(ptoc_Var175->sqlxa.sqlkap , "vpc14.p" , 9642))[sql__setrang((int ) ptoc_Var175->pcka , 1 , 3000) + -1].kapacount = -2;
                ptoc_Var175->pcda = 300;
                }
            else
                {
                ptoc_Var176->syposacc = retpos;
                p14ansqlstatement(next_symbol , &(*apc));
                }
            break;
        default :
            ptoc_Var176->syposacc = retpos;
            p14ansqlstatement(next_symbol , &(*apc));
            break;
        }
    if ((int ) ptoc_Var181->xakano > 0)
        {
        (*(sqlkatype *) sql__nil(ptoc_Var181->sqlkap , "vpc14.p" , 9662))[sql__setrang((int ) ptoc_Var181->xakano , 1 , 3000) + -1].kapaindex = (short ) sql__setrang((short ) ptoc_Var175->pcpaloop , -32768 , 32767);
        (*(sqlkatype *) sql__nil(ptoc_Var181->sqlkap , "vpc14.p" , 9663))[sql__setrang((int ) ptoc_Var181->xakano , 1 , 3000) + -1].kapacount = (short ) sql__setrang((short ) ((int ) (*(sqlkatype *) sql__nil(ptoc_Var181->sqlkap , "vpc14.p" , 9663))[sql__setrang((int ) ptoc_Var181->xakano , 1 , 3000) + -1].kapacount + 1) , -32768 , 32767);
        m90int2(14 , "pcka        " , (short ) sql__setrang(ptoc_Var175->pcka , -32768 , 32767));
        m90int2(14 , "xakano      " , (short ) sql__setrang(ptoc_Var181->xakano , -32768 , 32767));
        m90int2(14 , "pcpaloop    " , (short ) sql__setrang((short ) ptoc_Var175->pcpaloop , -32768 , 32767));
        m90int2(14 , "kapacount   " , (short ) sql__setrang((*(sqlkatype *) sql__nil(ptoc_Var181->sqlkap , "vpc14.p" , 9667))[sql__setrang((int ) ptoc_Var181->xakano , 1 , 3000) + -1].kapacount , -32768 , 32767));
        }
    if ((int ) ptoc_Var175->pcerror == 0)
        {
        ptoc_Var175->pcerror = (unsigned char ) sql__setrang(loopcnterr , 0 , 90);
        }
    ptoc_Var176->symb = 4;
    m90name(22 , "END p14forloop    ");
    }



int p14fetch(next_symbol, apc, keyind, pos)
int (*next_symbol)();
tpc_globals *apc;
int *keyind;
tsp_int4 *pos;
    {
    int n;
    int f_pos;
    tsp_int4 stmlen;
    tsp_int2 macro;
    tsp_int2 param;
    tsp_knl_identifier cursorn;
    int cursorl;
    tpr_scannertype rscan;
    tsp_int2 va1ix;
    tsp_c12 ch12;
    int no;
    tsp_int2 CursorNameIndex;
    int prind;
    tsp_int2 paix;
    int ndcuindex;
    tpc_globals *ptoc_Var184;
    tpr_scannertype *ptoc_Var185;
    sqlcatype *ptoc_Var186;
    sqlratype *ptoc_Var187;
    tpc_cmd_part *ptoc_Var188;
    sqlxatype *ptoc_Var189;
    int ptoc_Var190;
    sqlxatype *ptoc_Var191;
    sqlprentry *ptoc_Var192;
    sqlxatype *ptoc_Var193;
    sqlprentry *ptoc_Var194;
    m90print("PC14  p14fetch  ");
    ptoc_Var184 = &*apc;
    ptoc_Var185 = &ptoc_Var184->pcscan;
    ptoc_Var186 = &ptoc_Var184->sqlca;
    ptoc_Var187 = (sqlratype *) sql__nil(ptoc_Var186->sqlrap , "vpc14.p" , 9706);
    ptoc_Var188 = &ptoc_Var184->pccmdpart;
    ptoc_Var189 = &ptoc_Var184->sqlxa;
    if ((ptoc_Var190 = (int ) ptoc_Var187->ralang) == 3 || ptoc_Var190 == 4)
        {
        while ((int ) ptoc_Var185->symb != 4 && (int ) ptoc_Var185->symb != 23)
            (*next_symbol)(&(*apc) , sql__nil(ptoc_Var188->partbufp , "vpc14.p" , 9712) , ptoc_Var188->part1len);
        }
    ptoc_Var185->symb = 0;
    ptoc_Var189->xadescpos = 0;
    ptoc_Var189->xadesclen = 0;
    *keyind = 0;
    f_pos = 0;
    ptoc_Var184->pcan.ancomtyp = 23;
    p10rmspace(sql__nil(ptoc_Var188->partbufp , "vpc14.p" , 9721) , (unsigned char)'\'' , (unsigned char)'\'' , 1 , ptoc_Var188->part1len + -1 , &ptoc_Var188->part1len , &stmlen);
    if ((int ) ptoc_Var187->ralang != 4)
        {
        (*(tpc_partbuffer *) sql__nil(ptoc_Var188->partbufp , "vpc14.p" , 9725))[sql__setrang(ptoc_Var188->part1len , 1 , 16000) + -1] = (unsigned char)'\0';
        }
    m90buf(14 , &(*(tpc_partbuffer *) sql__nil(ptoc_Var188->partbufp , "vpc14.p" , 9727))[0] , 1 , ptoc_Var188->part1len);
    ptoc_Var185->syposacc = 1;
    (*next_symbol)(&(*apc) , sql__nil(ptoc_Var188->partbufp , "vpc14.p" , 9730) , ptoc_Var188->part1len);
    p19sqlfaallocate(&ptoc_Var184->pcfa);
    (*next_symbol)(&(*apc) , sql__nil(ptoc_Var188->partbufp , "vpc14.p" , 9733) , ptoc_Var188->part1len);
    p10getkeyword(&(*apc) , sql__nil(ptoc_Var188->partbufp , "vpc14.p" , 9734) , &(*keyind));
    if (*keyind == 81)
        {
        n = 0;
        do
            {
            (*next_symbol)(&(*apc) , sql__nil(ptoc_Var188->partbufp , "vpc14.p" , 9740) , ptoc_Var188->part1len);
            if ((int ) ptoc_Var185->symb == 10)
                {
                n = n + 1;
                }
            if ((int ) ptoc_Var185->symb == 13)
                {
                f_pos = 81;
                }
            if ((int ) ptoc_Var185->symb == 18)
                {
                n = n + -1;
                }
            }
        while (!(n == 0 || (int ) ptoc_Var185->symb == 4 || (int ) ptoc_Var185->symb == 23));
        }
    if (*keyind != 132 && *keyind != 56)
        {
        if (*keyind != 143 && *keyind != 144 && *keyind != 145 && *keyind != 42 && *keyind != 81)
            {
            p14getname(next_symbol , &(*apc) , cursorn , &cursorl , &macro , &param);
            paix = 0;
            p14_add_prind(&(*apc) , &prind , &paix);
            ptoc_Var184->pcpr = (short ) sql__setrang((short ) prind , -32768 , 32767);
            ptoc_Var191 = &ptoc_Var184->sqlxa;
            ptoc_Var192 = &(*(sqlprtype *) sql__nil(ptoc_Var191->sqlprp , "vpc14.p" , 9776))[sql__setrang((int ) ptoc_Var184->pcpr , 1 , 500) + -1];
            (*(sqlprtype *) sql__nil(ptoc_Var191->sqlprp , "vpc14.p" , 9778))[sql__setrang((int ) ptoc_Var184->pcpr , 1 , 500) + -1].prarea = 1;
            (*(sqlprtype *) sql__nil(ptoc_Var191->sqlprp , "vpc14.p" , 9779))[sql__setrang((int ) ptoc_Var184->pcpr , 1 , 500) + -1].prDescribe = 0;
            if ((int ) param == 1)
                {
                ptoc_Var184->pcan.ancomtyp = 23;
                ptoc_Var184->pcan.ancheck = 0;
                ptoc_Var184->pcusage.uscom = 0;
                rscan = ptoc_Var184->pcscan;
                p14_hostv_get(next_symbol , &(*apc) , &CursorNameIndex , &va1ix);
                no = (int ) va1ix;
                p05inttochr12(no , ch12);
                _ptoc_ASSIGN(unsigned char , 64, cursorn, "                                                                ")
                cursorn[0] = (unsigned char)':';
                s10mv18(12 , sizeof(unsigned char [64]) , ch12 , 2 , cursorn , 2 , 4);
                ndcuindex = (int ) ptoc_Var184->pcsqlva.ndcnt;
                if (ndcuindex > 0)
                    {
                    (*(tpc_arrndtype *) sql__nil(ptoc_Var184->pcsqlva.ndtabp , "vpc14.p" , 9800))[sql__setrang(ndcuindex , 1 , 100) + -1].ndkano = (short ) sql__setrang((short ) ((int ) ptoc_Var184->pcka + 1) , -32768 , 32767);
                    }
                ptoc_Var184->pcscan = rscan;
                _ptoc_ASSIGN(unsigned char , 64, (*(sqlprtype *) sql__nil(ptoc_Var189->sqlprp , "vpc14.p" , 9803))[sql__setrang((int ) ptoc_Var184->pcpr , 1 , 500) + -1].prCursorName, cursorn)
                (*(sqlprtype *) sql__nil(ptoc_Var189->sqlprp , "vpc14.p" , 9804))[sql__setrang((int ) ptoc_Var184->pcpr , 1 , 500) + -1].prCursorNameIndex = (short ) sql__setrang(CursorNameIndex , -32768 , 32767);
                }
            else
                {
                _ptoc_ASSIGN(unsigned char , 64, (*(sqlprtype *) sql__nil(ptoc_Var184->sqlxa.sqlprp , "vpc14.p" , 9808))[sql__setrang((int ) ptoc_Var184->pcpr , 1 , 500) + -1].prCursorName, cursorn)
                }
            (*next_symbol)(&(*apc) , sql__nil(ptoc_Var188->partbufp , "vpc14.p" , 9811) , ptoc_Var188->part1len);
            p10getkeyword(&(*apc) , sql__nil(ptoc_Var188->partbufp , "vpc14.p" , 9812) , &(*keyind));
            }
        }
    else
        {
        paix = 0;
        p14_add_prind(&(*apc) , &prind , &paix);
        ptoc_Var184->pcpr = (short ) sql__setrang((short ) prind , -32768 , 32767);
        ptoc_Var193 = &ptoc_Var184->sqlxa;
        ptoc_Var194 = &(*(sqlprtype *) sql__nil(ptoc_Var193->sqlprp , "vpc14.p" , 9821))[sql__setrang((int ) ptoc_Var184->pcpr , 1 , 500) + -1];
        (*(sqlprtype *) sql__nil(ptoc_Var193->sqlprp , "vpc14.p" , 9823))[sql__setrang((int ) ptoc_Var184->pcpr , 1 , 500) + -1].prarea = 1;
        (*(sqlprtype *) sql__nil(ptoc_Var193->sqlprp , "vpc14.p" , 9824))[sql__setrang((int ) ptoc_Var184->pcpr , 1 , 500) + -1].prDescribe = 0;
        }
    *pos = ptoc_Var185->syposacc;
    if (*keyind != 132 && *keyind != 56)
        {
        (*next_symbol)(&(*apc) , sql__nil(ptoc_Var188->partbufp , "vpc14.p" , 9835) , ptoc_Var188->part1len);
        p10getkeyword(&(*apc) , sql__nil(ptoc_Var188->partbufp , "vpc14.p" , 9836) , &(*keyind));
        }
    if (*keyind != 33)
        {
        (*next_symbol)(&(*apc) , sql__nil(ptoc_Var188->partbufp , "vpc14.p" , 9842) , ptoc_Var188->part1len);
        p10getkeyword(&(*apc) , sql__nil(ptoc_Var188->partbufp , "vpc14.p" , 9843) , &(*keyind));
        }
    if (*keyind == 33)
        {
        if (f_pos == 81)
            {
            p11wrerrorlisting(&(*apc) , 37);
            }
        else
            {
            n = ptoc_Var185->syposacc;
            while ((int ) (*(tpc_partbuffer *) sql__nil(ptoc_Var188->partbufp , "vpc14.p" , 9857))[sql__setrang(n , 1 , 16000) + -1] == 32)
                n = n + 1;
            if ((int ) (*(tpc_partbuffer *) sql__nil(ptoc_Var188->partbufp , "vpc14.p" , 9860))[sql__setrang(n , 1 , 16000) + -1] == 58)
                {
                (*(tpc_partbuffer *) sql__nil(ptoc_Var188->partbufp , "vpc14.p" , 9862))[sql__setrang(n , 1 , 16000) + -1] = (unsigned char)' ';
                }
            ptoc_Var189->xadescpos = (short ) sql__setrang((short ) n , -32768 , 32767);
            ptoc_Var189->xadesclen = (short ) sql__setrang((short ) (ptoc_Var188->part1len - (int ) ptoc_Var189->xadescpos) , -32768 , 32767);
            }
        if ((int ) ptoc_Var187->rakamode == 1 && ptoc_Var188->part1len - n == 0)
            {
            ptoc_Var189->xadescpos = -1;
            }
        }
    ptoc_Var185->symb = 4;
    ptoc_Var185->syposacc = 1;
    m90name(22 , "END p14fetch      ");
    }



int p14whenever(next_symbol, apc)
int (*next_symbol)();
tpc_globals *apc;
    {
    int keyind;
    tpc_globals *ptoc_Var195;
    tpr_scannertype *ptoc_Var196;
    tpc_cmd_part *ptoc_Var197;
    m90print("PC14  p14wheneve");
    ptoc_Var195 = &*apc;
    ptoc_Var196 = &ptoc_Var195->pcscan;
    ptoc_Var197 = &ptoc_Var195->pccmdpart;
    if ((int ) ptoc_Var195->pcerror == 0)
        {
        ptoc_Var195->pcwheset = 1;
        (*next_symbol)(&(*apc) , sql__nil(ptoc_Var197->partbufp , "vpc14.p" , 9901) , ptoc_Var197->part1len);
        p10getkeyword(&(*apc) , sql__nil(ptoc_Var197->partbufp , "vpc14.p" , 9902) , &keyind);
        switch (keyind)
            {
            case 114 :
                p14_get_action(next_symbol , &(*apc) , 1);
                break;
            case 108 :
                p14_get_action(next_symbol , &(*apc) , 2);
                break;
            case 110 :
                p14_get_action(next_symbol , &(*apc) , 3);
                break;
            case 71 :
                p14_get_action(next_symbol , &(*apc) , 4);
                break;
            case 70 :
                (*next_symbol)(&(*apc) , sql__nil(ptoc_Var197->partbufp , "vpc14.p" , 9914) , ptoc_Var197->part1len);
                p10getkeyword(&(*apc) , sql__nil(ptoc_Var197->partbufp , "vpc14.p" , 9915) , &keyind);
                if (keyind == 46)
                    {
                    p14_get_action(next_symbol , &(*apc) , 4);
                    }
                else
                    {
                    p11wrerrorlisting(&(*apc) , 81);
                    }
                break;
            case 102 :
                p14_get_action(next_symbol , &(*apc) , 5);
                break;
            case 109 :
                p14_get_action(next_symbol , &(*apc) , 6);
                break;
            default :
                p11wrerrorlisting(&(*apc) , 81);
                break;
            }
        }
    while ((int ) ptoc_Var196->symb != 4 && (int ) ptoc_Var196->symb != 23)
        (*next_symbol)(&(*apc) , sql__nil(ptoc_Var197->partbufp , "vpc14.p" , 9934) , ptoc_Var197->part1len);
    m90name(22 , "END p14whenever   ");
    }



int p14_get_action(next_symbol, apc, ind)
int (*next_symbol)();
tpc_globals *apc;
int ind;
    {
    int i;
    int pos;
    int keyind;
    tpc_globals *ptoc_Var198;
    tpr_scannertype *ptoc_Var199;
    sqlcatype *ptoc_Var200;
    sqlratype *ptoc_Var201;
    tpc_cmd_part *ptoc_Var202;
    struct ptoc_Type66 *ptoc_Var203;
    int ptoc_Var204;
    int ptoc_Var205;
    m90print("PC14  p14_get_ac");
    ptoc_Var198 = &*apc;
    ptoc_Var199 = &ptoc_Var198->pcscan;
    ptoc_Var200 = &ptoc_Var198->sqlca;
    ptoc_Var201 = (sqlratype *) sql__nil(ptoc_Var200->sqlrap , "vpc14.p" , 9954);
    ptoc_Var202 = &ptoc_Var198->pccmdpart;
    ptoc_Var203 = &ptoc_Var198->pcwhe[sql__setrang(ind , 1 , 6) + -1];
    if ((int ) ptoc_Var198->pcerror == 0)
        {
        pos = ptoc_Var199->syposacc;
        if ((ptoc_Var204 = (int ) ptoc_Var201->ralang) == 3 || ptoc_Var204 == 4)
            {
            while ((int ) ptoc_Var199->symb != 4 && (int ) ptoc_Var199->symb != 23)
                (*next_symbol)(&(*apc) , sql__nil(ptoc_Var202->partbufp , "vpc14.p" , 9963) , ptoc_Var202->part1len);
            }
        if ((int ) ptoc_Var201->ralang == 4)
            {
            ptoc_Var202->part1len = ptoc_Var202->part1len + 1;
            (*(tpc_partbuffer *) sql__nil(ptoc_Var202->partbufp , "vpc14.p" , 9970))[sql__setrang(ptoc_Var202->part1len , 1 , 16000) + -1] = (unsigned char)'\0';
            }
        ptoc_Var199->syposacc = pos;
        ptoc_Var199->symb = 0;
        (*next_symbol)(&(*apc) , sql__nil(ptoc_Var202->partbufp , "vpc14.p" , 9975) , ptoc_Var202->part1len);
        p10getkeyword(&(*apc) , sql__nil(ptoc_Var202->partbufp , "vpc14.p" , 9976) , &keyind);
        switch (keyind)
            {
            case 118 :
                ptoc_Var203->whlen = (short ) sql__setrang((short ) ptoc_Var199->sylen , -32768 , 32767);
                ptoc_Var203->whact = 118;
                s10mv4(ptoc_Var202->partsizlen , 50 , sql__nil(ptoc_Var202->partbufp , "vpc14.p" , 9982) , ptoc_Var199->sypos , ptoc_Var203->whstr , 1 , ptoc_Var199->sylen);
                break;
            case 21 :
                ptoc_Var203->whlen = 0;
                ptoc_Var203->whact = 21;
                break;
            case 49 :
            case 48 :
                if (ind == 5 || ind == 6)
                    {
                    p11wrerrorlisting(&(*apc) , 81);
                    }
                else
                    {
                    if ((ptoc_Var204 = (int ) ptoc_Var201->ralang) == 4 || ptoc_Var204 == 7 || ptoc_Var204 == 8)
                        {
                        if (keyind == 48)
                            {
                            (*next_symbol)(&(*apc) , sql__nil(ptoc_Var202->partbufp , "vpc14.p" , 10003) , ptoc_Var202->part1len);
                            p10getkeyword(&(*apc) , sql__nil(ptoc_Var202->partbufp , "vpc14.p" , 10004) , &keyind);
                            if (keyind != 123)
                                {
                                p11wrerrorlisting(&(*apc) , 81);
                                }
                            }
                        pos = ptoc_Var199->syposacc;
                        }
                    else
                        {
                        pos = ptoc_Var199->sypos;
                        }
                    if ((int ) ptoc_Var201->ralang == 3)
                        {
                        if (keyind == 48)
                            {
                            (*next_symbol)(&(*apc) , sql__nil(ptoc_Var202->partbufp , "vpc14.p" , 10022) , ptoc_Var202->part1len);
                            p10getkeyword(&(*apc) , sql__nil(ptoc_Var202->partbufp , "vpc14.p" , 10023) , &keyind);
                            if (keyind != 123)
                                {
                                p11wrerrorlisting(&(*apc) , 81);
                                }
                            }
                        (*(tpc_partbuffer *) sql__nil(ptoc_Var202->partbufp , "vpc14.p" , 10030))[sql__setrang(pos , 1 , 16000) + -1] = (unsigned char)'g';
                        (*(tpc_partbuffer *) sql__nil(ptoc_Var202->partbufp , "vpc14.p" , 10031))[sql__setrang(pos + 1 , 1 , 16000) + -1] = (unsigned char)'o';
                        (*(tpc_partbuffer *) sql__nil(ptoc_Var202->partbufp , "vpc14.p" , 10032))[sql__setrang(pos + 2 , 1 , 16000) + -1] = (unsigned char)'t';
                        (*(tpc_partbuffer *) sql__nil(ptoc_Var202->partbufp , "vpc14.p" , 10033))[sql__setrang(pos + 3 , 1 , 16000) + -1] = (unsigned char)'o';
                        ptoc_Var204 = pos + 4;
                        ptoc_Var205 = ptoc_Var199->syposacc;
                        if (ptoc_Var204 <= ptoc_Var205)
                            {
                            i = ptoc_Var204;
                            do
                                {
                                (*(tpc_partbuffer *) sql__nil(ptoc_Var202->partbufp , "vpc14.p" , 10035))[sql__setrang(i , 1 , 16000) + -1] = (unsigned char)' ';
                                if (i == ptoc_Var205)
                                    {
                                    break;
                                    }
                                i += 1;
                                }
                            while (!0);
                            }
                        }
                    ptoc_Var203->whlen = (short ) sql__setrang((short ) (ptoc_Var202->part1len - pos) , -32768 , 32767);
                    ptoc_Var203->whact = 49;
                    if ((int ) ptoc_Var203->whlen > 50)
                        {
                        ptoc_Var203->whlen = 50;
                        p11wrerrorlisting(&(*apc) , 69);
                        }
                    i = ptoc_Var199->syposacc;
                    while ((int ) (*(tpc_partbuffer *) sql__nil(ptoc_Var202->partbufp , "vpc14.p" , 10049))[sql__setrang(i , 1 , 16000) + -1] == 32 && i < ptoc_Var202->part1len)
                        i = i + 1;
                    if ((int ) (*(tpc_partbuffer *) sql__nil(ptoc_Var202->partbufp , "vpc14.p" , 10052))[sql__setrang(i , 1 , 16000) + -1] == 58)
                        {
                        (*(tpc_partbuffer *) sql__nil(ptoc_Var202->partbufp , "vpc14.p" , 10054))[sql__setrang(i , 1 , 16000) + -1] = (unsigned char)' ';
                        }
                    s10mv4(ptoc_Var202->partsizlen , 50 , sql__nil(ptoc_Var202->partbufp , "vpc14.p" , 10056) , pos , ptoc_Var203->whstr , 1 , (int ) ptoc_Var203->whlen);
                    }
                break;
            case 35 :
                if ((int ) ptoc_Var201->ralang == 4 || (int ) ptoc_Var201->ralang == 7)
                    {
                    (*next_symbol)(&(*apc) , sql__nil(ptoc_Var202->partbufp , "vpc14.p" , 10066) , ptoc_Var202->part1len);
                    p10getkeyword(&(*apc) , sql__nil(ptoc_Var202->partbufp , "vpc14.p" , 10067) , &keyind);
                    if (keyind != 80)
                        {
                        p11wrerrorlisting(&(*apc) , 81);
                        }
                    }
                pos = ptoc_Var199->syposacc;
                ptoc_Var203->whlen = (short ) sql__setrang((short ) (ptoc_Var202->part1len - pos) , -32768 , 32767);
                ptoc_Var203->whact = 11;
                if ((int ) ptoc_Var203->whlen > 50)
                    {
                    ptoc_Var203->whlen = 50;
                    p11wrerrorlisting(&(*apc) , 69);
                    }
                s10mv4(ptoc_Var202->partsizlen , 50 , sql__nil(ptoc_Var202->partbufp , "vpc14.p" , 10084) , pos , ptoc_Var203->whstr , 1 , (int ) ptoc_Var203->whlen);
                break;
            case 11 :
                pos = ptoc_Var199->syposacc;
                ptoc_Var203->whlen = (short ) sql__setrang((short ) (ptoc_Var202->part1len - pos) , -32768 , 32767);
                ptoc_Var203->whact = 11;
                if ((int ) ptoc_Var203->whlen > 50)
                    {
                    ptoc_Var203->whlen = 50;
                    p11wrerrorlisting(&(*apc) , 69);
                    }
                s10mv4(ptoc_Var202->partsizlen , 50 , sql__nil(ptoc_Var202->partbufp , "vpc14.p" , 10099) , pos , ptoc_Var203->whstr , 1 , (int ) ptoc_Var203->whlen);
                break;
            default :
                p11wrerrorlisting(&(*apc) , 81);
                break;
            }
        ptoc_Var199->symb = 4;
        }
    m90name(22 , "END p14_get_action");
    }



int p14gmacrono(next_symbol, apc, macnr)
int (*next_symbol)();
tpc_globals *apc;
tsp_int2 *macnr;
    {
    tpc_pre_errors err;
    tsp_int4 len;
    tsp_lname vnam;
    int vnaml;
    tsp_int2 ptypentix;
    tsp_int2 pvatyp;
    tsp_int2 retpos;
    tsp_int2 lastvatyp;
    char setupper;
    int i;
    int pos;
    int mlen;
    tpc_globals *ptoc_Var206;
    tpr_scannertype *ptoc_Var207;
    sqlcatype *ptoc_Var208;
    sqlratype *ptoc_Var209;
    tpc_cmd_part *ptoc_Var210;
    sqlxatype *ptoc_Var211;
    tpc_pc_sqlva *ptoc_Var212;
    tpc_symtab *ptoc_Var213;
    int ptoc_Var214;
    struct ptoc_Type43 *ptoc_Var215;
    int ptoc_Var216;
    struct ptoc_Type75 *ptoc_Var217;
    m90print("PC14  p14gmacron");
    ptoc_Var206 = &*apc;
    ptoc_Var207 = &ptoc_Var206->pcscan;
    ptoc_Var208 = &ptoc_Var206->sqlca;
    ptoc_Var209 = (sqlratype *) sql__nil(ptoc_Var208->sqlrap , "vpc14.p" , 10136);
    ptoc_Var210 = &ptoc_Var206->pccmdpart;
    ptoc_Var211 = &ptoc_Var206->sqlxa;
    ptoc_Var212 = &ptoc_Var206->pcsqlva;
    ptoc_Var213 = &ptoc_Var206->pcsymtab;
    retpos = (short ) sql__setrang((short ) ptoc_Var207->syposacc , -32768 , 32767);
    if ((ptoc_Var214 = (int ) ptoc_Var209->ralang) == 3)
        {
        while ((int ) ptoc_Var207->symb != 4 && (int ) ptoc_Var207->symb != 23)
            (*next_symbol)(&(*apc) , sql__nil(ptoc_Var210->partbufp , "vpc14.p" , 10143) , ptoc_Var210->part1len);
        }
    ptoc_Var207->syposacc = (int ) retpos;
    ptoc_Var207->symb = 0;
    if ((int ) ptoc_Var206->pcerror == 0)
        {
        err = 0;
        (*next_symbol)(&(*apc) , sql__nil(ptoc_Var210->partbufp , "vpc14.p" , 10152) , ptoc_Var210->part1len);
        if ((int ) ptoc_Var207->symb != 11)
            {
            err = 81;
            }
        else
            {
            p14kaentryinit(&(*apc));
            (*next_symbol)(&(*apc) , sql__nil(ptoc_Var210->partbufp , "vpc14.p" , 10159) , ptoc_Var210->part1len);
            if ((int ) ptoc_Var207->symb != 22)
                {
                err = 29;
                }
            else
                {
                p10int2unsignedget(&(*apc) , sql__nil(ptoc_Var210->partbufp , "vpc14.p" , 10165) , &(*macnr));
                (*next_symbol)(&(*apc) , sql__nil(ptoc_Var210->partbufp , "vpc14.p" , 10166) , ptoc_Var210->part1len);
                if ((int ) *macnr <= 0 || (int ) *macnr > 128)
                    {
                    *macnr = 1;
                    err = 78;
                    }
                else
                    {
                    if ((int ) ptoc_Var207->symb == 5)
                        {
                        (*next_symbol)(&(*apc) , sql__nil(ptoc_Var210->partbufp , "vpc14.p" , 10177) , ptoc_Var210->part1len);
                        if ((int ) ptoc_Var207->symb == 8)
                            {
                            if ((int ) ptoc_Var209->ralang == 4)
                                {
                                retpos = (short ) sql__setrang((short ) ptoc_Var207->sypos , -32768 , 32767);
                                while ((int ) ptoc_Var207->symb != 4 && (int ) ptoc_Var207->symb != 23)
                                    (*next_symbol)(&(*apc) , sql__nil(ptoc_Var210->partbufp , "vpc14.p" , 10187) , ptoc_Var210->part1len);
                                ptoc_Var207->sypos = (int ) retpos;
                                }
                            if ((int ) (*(tpc_partbuffer *) sql__nil(ptoc_Var210->partbufp , "vpc14.p" , 10193))[sql__setrang(ptoc_Var207->sypos + -1 , 1 , 16000) + -1] == 34)
                                {
                                setupper = 0;
                                p10rmspace(sql__nil(ptoc_Var210->partbufp , "vpc14.p" , 10197) , (unsigned char)'\'' , (unsigned char)'\'' , ptoc_Var207->sypos + -1 , ptoc_Var210->part1len + -1 , &ptoc_Var210->part1len , &len);
                                len = len + -2;
                                }
                            else
                                {
                                setupper = 1;
                                p10rmspace(sql__nil(ptoc_Var210->partbufp , "vpc14.p" , 10205) , (unsigned char)'\'' , (unsigned char)'\'' , ptoc_Var207->sypos , ptoc_Var210->part1len + -1 , &ptoc_Var210->part1len , &len);
                                }
                            if (len > 126)
                                {
                                err = 29;
                                }
                            else
                                {
                                m90int2(14 , "len         " , (short ) sql__setrang((short ) len , -32768 , 32767));
                                s10mv6(ptoc_Var210->partsizlen , 126 , sql__nil(ptoc_Var210->partbufp , "vpc14.p" , 10217) , ptoc_Var207->sypos , (*(sqlmatype *) sql__nil(ptoc_Var208->sqlmap , "vpc14.p" , 10218)).maentry[sql__setrang((int ) *macnr , 1 , 128) + -1].maval , 1 , len);
                                mlen = 0;
                                pos = ptoc_Var207->sypos;
                                do
                                    {
                                    if ((int ) (*(tpc_partbuffer *) sql__nil(ptoc_Var210->partbufp , "vpc14.p" , 10223))[sql__setrang(pos , 1 , 16000) + -1] == 34 && (int ) (*(tpc_partbuffer *) sql__nil(ptoc_Var210->partbufp , "vpc14.p" , 10224))[sql__setrang(pos + 1 , 1 , 16000) + -1] == 34)
                                        {
                                        pos = pos + 1;
                                        }
                                    mlen = mlen + 1;
                                    (*(sqlmatype *) sql__nil(ptoc_Var208->sqlmap , "vpc14.p" , 10229)).maentry[sql__setrang((int ) *macnr , 1 , 128) + -1].maval[sql__setrang(mlen , 1 , 126) + -1] = (unsigned char ) sql__setrang((*(tpc_partbuffer *) sql__nil(ptoc_Var210->partbufp , "vpc14.p" , 10230))[sql__setrang(pos , 1 , 16000) + -1] , 0 , 255);
                                    pos = pos + 1;
                                    }
                                while (!(mlen >= 126 || pos >= ptoc_Var207->sypos + len));
                                (*(sqlmatype *) sql__nil(ptoc_Var208->sqlmap , "vpc14.p" , 10236)).maentry[sql__setrang((int ) *macnr , 1 , 128) + -1].malen = (short ) sql__setrang((short ) mlen , -32768 , 32767);
                                if ((char ) sql__setrang(setupper , 0 , 1))
                                    {
                                    ptoc_Var215 = &(*(sqlmatype *) sql__nil(ptoc_Var208->sqlmap , "vpc14.p" , 10240)).maentry[sql__setrang((int ) *macnr , 1 , 128) + -1];
                                    ptoc_Var214 = len;
                                    if (1 <= ptoc_Var214)
                                        {
                                        i = 1;
                                        do
                                            {
                                            if ((ptoc_Var216 = (int ) ptoc_Var215->maval[sql__setrang(i , 1 , 126) + -1]) >= 97 && ptoc_Var216 <= 105 || ptoc_Var216 >= 106 && ptoc_Var216 <= 114 || ptoc_Var216 >= 115 && ptoc_Var216 <= 122)
                                                {
                                                ptoc_Var215->maval[sql__setrang(i , 1 , 126) + -1] = (unsigned char ) sql__setrang((unsigned char ) (sql__ord((int ) ptoc_Var215->maval[sql__setrang(i , 1 , 126) + -1]) + sql__ord(65) - sql__ord(97)) , 0 , 255);
                                                }
                                            if (i == ptoc_Var214)
                                                {
                                                break;
                                                }
                                            i += 1;
                                            }
                                        while (!0);
                                        }
                                    }
                                }
                            }
                        else
                            {
                            if ((int ) ptoc_Var207->symb == 13)
                                {
                                ptoc_Var207->syposacc = ptoc_Var207->sypos + 1;
                                ptypentix = 0;
                                retpos = (short ) sql__setrang((short ) ptoc_Var207->sypos , -32768 , 32767);
                                p14btypentget(next_symbol , &(*apc) , &ptypentix , &pvatyp , &lastvatyp , 1);
                                ptoc_Var207->syposacc = ptoc_Var207->sypos;
                                if ((int ) ptypentix == 0)
                                    {
                                    p11precomerror(&(*apc) , 79);
                                    _ptoc_ASSIGN(unsigned char , 32, vnam, "MACRO_PARAMETER                 ")
                                    vnaml = 16;
                                    p11nerrorlisting(&(*apc) , vnam , (short ) sql__setrang((short ) vnaml , -32768 , 32767) , 0);
                                    }
                                else
                                    {
                                    (*(sqlmatype *) sql__nil(ptoc_Var208->sqlmap , "vpc14.p" , 10274)).maentry[sql__setrang((int ) *macnr , 1 , 128) + -1].malen = (short ) sql__setrang((short ) (-(int ) ptoc_Var206->pcpa + -1) , -32768 , 32767);
                                    ptoc_Var217 = &(*(tpc_arrndtype *) sql__nil(ptoc_Var212->ndtabp , "vpc14.p" , 10276))[sql__setrang((int ) ptoc_Var212->ndcnt , 1 , 100) + -1];
                                    if (ptoc_Var217->ndsqlva1st != ptoc_Var217->ndsqlva1ix)
                                        {
                                        p19cparentry(&(*apc) , 1 , ptoc_Var217->ndsqlva1st , 0 , ptoc_Var217->ndsqlva1cnt);
                                        }
                                    p19cparentry(&(*apc) , 2 , ptoc_Var217->ndsqlva1ix , 0 , 0);
                                    s10fil1(126 , (*(sqlmatype *) sql__nil(ptoc_Var208->sqlmap , "vpc14.p" , 10291)).maentry[sql__setrang((int ) *macnr , 1 , 128) + -1].maval , 1 , 126 , (unsigned char)' ');
                                    if (((ptoc_Var214 = (int ) pvatyp) < 6 || ptoc_Var214 > 10) && ptoc_Var214 != 15 && ptoc_Var214 != 20)
                                        {
                                        err = 5;
                                        }
                                    }
                                if ((int ) ptoc_Var209->ralang == 4)
                                    {
                                    while ((int ) ptoc_Var207->symb != 4 && (int ) ptoc_Var207->symb != 23)
                                        (*next_symbol)(&(*apc) , sql__nil(ptoc_Var210->partbufp , "vpc14.p" , 10304) , ptoc_Var210->part1len);
                                    }
                                if ((int ) ptoc_Var207->symb != 4 && (int ) ptoc_Var207->symb != 23)
                                    {
                                    err = 48;
                                    ptoc_Var207->sypos = (int ) retpos;
                                    }
                                }
                            else
                                {
                                err = 29;
                                }
                            }
                        }
                    else
                        {
                        err = 29;
                        }
                    }
                }
            }
        }
    ptoc_Var207->symb = 4;
    if ((int ) err != 0)
        {
        p11wrerrorlisting(&(*apc) , (unsigned char ) sql__setrang(err , 0 , 90));
        }
    m90name(22 , "END p14gmacrono   ");
    }



int p14nolog(next_symbol, apc)
int (*next_symbol)();
tpc_globals *apc;
    {
    tpc_pre_errors err;
    tsp_int2 retpos;
    tpc_globals *ptoc_Var218;
    tpr_scannertype *ptoc_Var219;
    sqlcatype *ptoc_Var220;
    sqlratype *ptoc_Var221;
    tpc_cmd_part *ptoc_Var222;
    sqlxatype *ptoc_Var223;
    tpc_pc_sqlva *ptoc_Var224;
    tpc_symtab *ptoc_Var225;
    int ptoc_Var226;
    m90print("PC14  p14nolog  ");
    ptoc_Var218 = &*apc;
    ptoc_Var219 = &ptoc_Var218->pcscan;
    ptoc_Var220 = &ptoc_Var218->sqlca;
    ptoc_Var221 = (sqlratype *) sql__nil(ptoc_Var220->sqlrap , "vpc14.p" , 10355);
    ptoc_Var222 = &ptoc_Var218->pccmdpart;
    ptoc_Var223 = &ptoc_Var218->sqlxa;
    ptoc_Var224 = &ptoc_Var218->pcsqlva;
    ptoc_Var225 = &ptoc_Var218->pcsymtab;
    retpos = (short ) sql__setrang((short ) ptoc_Var219->syposacc , -32768 , 32767);
    if ((ptoc_Var226 = (int ) ptoc_Var221->ralang) == 3)
        {
        while ((int ) ptoc_Var219->symb != 4 && (int ) ptoc_Var219->symb != 23)
            (*next_symbol)(&(*apc) , sql__nil(ptoc_Var222->partbufp , "vpc14.p" , 10362) , ptoc_Var222->part1len);
        }
    ptoc_Var219->syposacc = (int ) retpos;
    ptoc_Var219->symb = 0;
    if ((int ) ptoc_Var218->pcerror == 0)
        {
        err = 0;
        (*next_symbol)(&(*apc) , sql__nil(ptoc_Var222->partbufp , "vpc14.p" , 10371) , ptoc_Var222->part1len);
        if ((int ) ptoc_Var219->symb == 4 || (int ) ptoc_Var219->symb == 23)
            {
            (*(sqlgaentry *) sql__nil((*(sqlgatype *) sql__nil(ptoc_Var220->sqlgap , "vpc14.p" , 10376)).gaentry[sql__setrang((int ) ptoc_Var221->raactsession , 1 , 8) + -1] , "vpc14.p" , 10376)).ganolog = 1;
            }
        else
            {
            p11wrerrorlisting(&(*apc) , 37);
            }
        }
    }



int p14option(next_symbol, apc, statemno)
int (*next_symbol)();
tpc_globals *apc;
int *statemno;
    {
    tsp_knl_identifier statemn;
    int stateml;
    int keyind;
    int prind;
    int posab;
    int pos;
    int len;
    int posch;
    int mnr;
    unsigned char subch;
    tpc_pre_errors err;
    tsp_int2 ptypentix;
    tsp_int2 pvatyp;
    tsp_int2 kaind;
    tsp_int2 retpos;
    tsp_int2 lastvatyp;
    int ndindex;
    tpc_globals *ptoc_Var227;
    tpr_scannertype *ptoc_Var228;
    sqlcatype *ptoc_Var229;
    sqlratype *ptoc_Var230;
    tpc_cmd_part *ptoc_Var231;
    sqlxatype *ptoc_Var232;
    tpc_pc_sqlva *ptoc_Var233;
    sqlprentry *ptoc_Var234;
    sqlkaentry *ptoc_Var235;
    sqlxatype *ptoc_Var236;
    tpc_pc_sqlva *ptoc_Var237;
    tpc_symtab *ptoc_Var238;
    struct ptoc_Type75 *ptoc_Var239;
    m90print("PC14  p14option ");
    ptoc_Var227 = &*apc;
    ptoc_Var228 = &ptoc_Var227->pcscan;
    ptoc_Var229 = &ptoc_Var227->sqlca;
    ptoc_Var230 = (sqlratype *) sql__nil(ptoc_Var229->sqlrap , "vpc14.p" , 10415);
    ptoc_Var231 = &ptoc_Var227->pccmdpart;
    ptoc_Var232 = &ptoc_Var227->sqlxa;
    ptoc_Var233 = &ptoc_Var227->pcsqlva;
    retpos = (short ) sql__setrang((short ) ptoc_Var228->syposacc , -32768 , 32767);
    ptoc_Var232->xadescpos = 0;
    ptoc_Var232->xadesclen = 0;
    while ((int ) ptoc_Var228->symb != 4 && (int ) ptoc_Var228->symb != 23)
        (*next_symbol)(&(*apc) , sql__nil(ptoc_Var231->partbufp , "vpc14.p" , 10422) , ptoc_Var231->part1len);
    ptoc_Var228->syposacc = (int ) retpos;
    ptoc_Var228->symb = 0;
    if ((int ) ptoc_Var227->pcerror == 0)
        {
        err = 0;
        if ((int ) err == 0)
            {
            p10getkeyword(&(*apc) , sql__nil(ptoc_Var231->partbufp , "vpc14.p" , 10434) , &keyind);
            p14kaentryinit(&(*apc));
            p19sqlprallocate(&ptoc_Var227->pcpr);
            p19sqlfaallocate(&ptoc_Var227->pcfa);
            *statemno = (int ) ptoc_Var227->pcka;
            ptoc_Var234 = &(*(sqlprtype *) sql__nil(ptoc_Var227->sqlxa.sqlprp , "vpc14.p" , 10439))[sql__setrang((int ) ptoc_Var227->pcpr , 1 , 500) + -1];
            ptoc_Var235 = &(*(sqlkatype *) sql__nil(ptoc_Var227->sqlxa.sqlkap , "vpc14.p" , 10440))[sql__setrang((int ) ptoc_Var227->pcka , 1 , 3000) + -1];
            ptoc_Var236 = &ptoc_Var227->sqlxa;
            ptoc_Var237 = &ptoc_Var227->pcsqlva;
            ptoc_Var238 = &ptoc_Var227->pcsymtab;
            ptoc_Var234->prstcount = 0;
            ptoc_Var234->prstindex = 0;
            ptoc_Var234->prcuindex = 0;
            ptoc_Var234->prstate = 4;
            _ptoc_ASSIGN(unsigned char , 64, ptoc_Var234->prStmtName, "                                                                ")
            ptoc_Var234->prStmtNameIndex = 0;
            _ptoc_ASSIGN(unsigned char , 64, ptoc_Var234->prCursorName, "                                                                ")
            ptoc_Var234->prCursorNameIndex = 0;
            ptoc_Var234->prkaindex = (short ) sql__setrang(ptoc_Var227->pcka , -32768 , 32767);
            ptoc_Var234->prarea = 0;
            ptoc_Var234->prDescribe = 0;
            ptoc_Var235->kaprindex = (short ) sql__setrang(ptoc_Var227->pcpr , -32768 , 32767);
            ptoc_Var235->kafaindex = (short ) sql__setrang(ptoc_Var227->pcfa , -32768 , 32767);
            (*next_symbol)(&(*apc) , sql__nil(ptoc_Var231->partbufp , "vpc14.p" , 10455) , ptoc_Var231->part1len);
            if ((int ) ptoc_Var228->symb == 13)
                {
                ptoc_Var228->syposacc = ptoc_Var228->sypos + 1;
                ptypentix = 0;
                p14btypentget(next_symbol , &(*apc) , &ptypentix , &pvatyp , &lastvatyp , 1);
                if ((int ) ptypentix == 0)
                    {
                    if ((int ) ptoc_Var227->pcerror == 0)
                        {
                        p11precomerror(&(*apc) , 79);
                        p11nerrorlisting(&(*apc) , (*(tpc_vartable *) sql__nil(ptoc_Var238->vartablep , "vpc14.p" , 10472))[sql__setrang((int ) (*(tpc_arrndtype *) sql__nil(ptoc_Var237->ndtabp , "vpc14.p" , 10472))[sql__setrang((int ) ptoc_Var237->ndcnt , 1 , 100) + -1].ndvarentix , 1 , 1000) + -1].vaname , (short ) sql__setrang((*(tpc_vartable *) sql__nil(ptoc_Var238->vartablep , "vpc14.p" , 10474))[sql__setrang((int ) (*(tpc_arrndtype *) sql__nil(ptoc_Var237->ndtabp , "vpc14.p" , 10474))[sql__setrang((int ) ptoc_Var237->ndcnt , 1 , 100) + -1].ndvarentix , 1 , 1000) + -1].vanaml , -32768 , 32767) , 0);
                        }
                    }
                else
                    {
                    ptoc_Var239 = &(*(tpc_arrndtype *) sql__nil(ptoc_Var237->ndtabp , "vpc14.p" , 10481))[sql__setrang((int ) ptoc_Var237->ndcnt , 1 , 100) + -1];
                    ptoc_Var235->kaStindex = (short ) sql__setrang((short ) ((int ) ptoc_Var227->pcpa + 1) , -32768 , 32767);
                    if (ptoc_Var239->ndsqlva1st != ptoc_Var239->ndsqlva1ix)
                        {
                        p19cparentry(&(*apc) , 1 , ptoc_Var239->ndsqlva1st , 0 , ptoc_Var239->ndsqlva1cnt);
                        }
                    p19cparentry(&(*apc) , 2 , ptoc_Var239->ndsqlva1ix , 0 , 0);
                    }
                }
            else
                {
                ptoc_Var227->pcan.ancheck = 0;
                p10stringliteralget(&(*apc) , sql__nil(ptoc_Var231->partbufp , "vpc14.p" , 10500));
                s10fil(ptoc_Var231->partsizlen , sql__nil(ptoc_Var231->partbufp , "vpc14.p" , 10501) , 1 , ptoc_Var228->sypos + -1 , (unsigned char)' ');
                posab = ptoc_Var228->sypos;
                posch = 1;
                while (posch != 0)
                    {
                    p19snextsubchar(&(*apc) , posab , &posch , &subch);
                    if ((int ) subch == 37)
                        {
                        ptoc_Var235->kamacro = 1;
                        p19gmacronumber(&(*apc) , posch , &mnr , &posch);
                        if (mnr <= 0 || mnr > 128)
                            {
                            err = 30;
                            }
                        }
                    posab = posch + 1;
                    }
                ptoc_Var234->prarea = 1;
                ptoc_Var235->kaStcount = 0;
                ptoc_Var235->kaStindex = (short ) sql__setrang((short ) ((int ) ptoc_Var227->pcst + 1) , -32768 , 32767);
                pos = ptoc_Var228->sypos;
                while (pos < ptoc_Var228->sypos + ptoc_Var228->sylen)
                    {
                    if (pos + 52 > ptoc_Var228->sypos + ptoc_Var228->sylen)
                        {
                        len = ptoc_Var228->sypos + ptoc_Var228->sylen - pos;
                        }
                    else
                        {
                        len = 52;
                        }
                    p19sqlstallocate(&ptoc_Var227->pcst);
                    if ((int ) ptoc_Var227->pcst > (int ) ptoc_Var227->pcstmax)
                        {
                        ptoc_Var227->pcstmax = (short ) sql__setrang(ptoc_Var227->pcst , -32768 , 32767);
                        }
                    ptoc_Var235->kaStcount = (short ) sql__setrang((short ) ((int ) ptoc_Var235->kaStcount + 1) , -32768 , 32767);
                    s10mv1(ptoc_Var231->partsizlen , 52 , sql__nil(ptoc_Var231->partbufp , "vpc14.p" , 10542) , pos , (*(sqlsttype *) sql__nil(ptoc_Var236->sqlstp , "vpc14.p" , 10543))[sql__setrang((int ) ptoc_Var227->pcst , 1 , 1000) + -1].stline , 1 , len);
                    (*(sqlsttype *) sql__nil(ptoc_Var236->sqlstp , "vpc14.p" , 10544))[sql__setrang((int ) ptoc_Var227->pcst , 1 , 1000) + -1].stllen = len;
                    (*(sqlsttype *) sql__nil(ptoc_Var236->sqlstp , "vpc14.p" , 10545))[sql__setrang((int ) ptoc_Var227->pcst , 1 , 1000) + -1].stkano = (int ) ptoc_Var227->pcka;
                    pos = pos + len;
                    }
                ptoc_Var228->syposacc = ptoc_Var228->sypos;
                }
            ptoc_Var227->sqlxa.xakano = (short ) sql__setrang(ptoc_Var227->pcka , -32768 , 32767);
            }
        if ((int ) err != 0)
            {
            p11precomerror(&(*apc) , (unsigned char ) sql__setrang(err , 0 , 90));
            }
        }
    m90int2(14 , "xakano      " , (short ) sql__setrang(ptoc_Var232->xakano , -32768 , 32767));
    m90int2(14 , "xadescpos   " , (short ) sql__setrang(ptoc_Var232->xadescpos , -32768 , 32767));
    m90int2(14 , "xadesclen   " , (short ) sql__setrang(ptoc_Var232->xadesclen , -32768 , 32767));
    ptoc_Var228->symb = 4;
    }



int p14gdbsname(next_symbol, apc, ind, dbpos, dblen, dbnodepos, dbnodelen)
int (*next_symbol)();
tpc_globals *apc;
int *ind;
int *dbpos;
int *dblen;
int *dbnodepos;
int *dbnodelen;
    {
    int keyind;
    tsp_dbname dbsname;
    tsp_nodeid dbnode;
    tsp_lname vnam;
    int pcnt;
    int vnaml;
    int len;
    tsp_int2 lastvatyp;
    tsp_int2 ptypentix;
    tsp_int2 pvatyp;
    tsp_int2 vix;
    tsp_int2 pvalen;
    int retpos;
    tpc_globals *ptoc_Var240;
    tpr_scannertype *ptoc_Var241;
    sqlcatype *ptoc_Var242;
    sqlratype *ptoc_Var243;
    tpc_cmd_part *ptoc_Var244;
    sqlxatype *ptoc_Var245;
    tpc_pc_sqlva *ptoc_Var246;
    tpc_symtab *ptoc_Var247;
    int ptoc_Var248;
    struct ptoc_Type75 *ptoc_Var249;
    struct ptoc_Type75 *ptoc_Var250;
    m90print("PC14  p14gdbsnam");
    ptoc_Var240 = &*apc;
    ptoc_Var241 = &ptoc_Var240->pcscan;
    ptoc_Var242 = &ptoc_Var240->sqlca;
    ptoc_Var243 = (sqlratype *) sql__nil(ptoc_Var242->sqlrap , "vpc14.p" , 10595);
    ptoc_Var244 = &ptoc_Var240->pccmdpart;
    ptoc_Var245 = &ptoc_Var240->sqlxa;
    ptoc_Var246 = &ptoc_Var240->pcsqlva;
    ptoc_Var247 = &ptoc_Var240->pcsymtab;
    pcnt = 0;
    *dbpos = 1;
    *dblen = 0;
    *dbnodepos = 1;
    *dbnodelen = 0;
    retpos = ptoc_Var241->syposacc;
    if ((ptoc_Var248 = (int ) ptoc_Var243->ralang) == 3 || ptoc_Var248 == 4)
        {
        while ((int ) ptoc_Var241->symb != 4 && (int ) ptoc_Var241->symb != 23)
            (*next_symbol)(&(*apc) , sql__nil(ptoc_Var244->partbufp , "vpc14.p" , 10608) , ptoc_Var244->part1len);
        }
    if ((int ) ptoc_Var243->ralang == 4)
        {
        ptoc_Var244->part1len = ptoc_Var244->part1len + 1;
        (*(tpc_partbuffer *) sql__nil(ptoc_Var244->partbufp , "vpc14.p" , 10615))[sql__setrang(ptoc_Var244->part1len , 1 , 16000) + -1] = (unsigned char)'\0';
        }
    ptoc_Var241->syposacc = retpos;
    ptoc_Var241->symb = 0;
    if ((int ) ptoc_Var240->pcerror == 0)
        {
        m90buf(14 , &(*(tpc_partbuffer *) sql__nil(ptoc_Var244->partbufp , "vpc14.p" , 10623))[0] , 1 , ptoc_Var244->part1len);
        *ind = (int ) ptoc_Var243->raactsession;
        (*next_symbol)(&(*apc) , sql__nil(ptoc_Var244->partbufp , "vpc14.p" , 10625) , ptoc_Var244->part1len);
        p14kaentryinit(&(*apc));
        if ((int ) ptoc_Var241->symb == 21 || (int ) ptoc_Var241->symb == 24)
            {
            p10stringliteralget(&(*apc) , sql__nil(ptoc_Var244->partbufp , "vpc14.p" , 10632));
            if (ptoc_Var241->sylen > 18)
                {
                p11wrerrorlisting(&(*apc) , 37);
                }
            else
                {
                _ptoc_ASSIGN(unsigned char , 18, dbsname, "                  ")
                (*(sqlgaentry *) sql__nil((*(sqlgatype *) sql__nil(ptoc_Var242->sqlgap , "vpc14.p" , 10639)).gaentry[sql__setrang(*ind , 1 , 8) + -1] , "vpc14.p" , 10639)).gavaindex = 0;
                s10mv15(ptoc_Var244->partsizlen , 18 , sql__nil(ptoc_Var244->partbufp , "vpc14.p" , 10640) , ptoc_Var241->sypos , dbsname , 1 , ptoc_Var241->sylen);
                *dbpos = ptoc_Var241->sypos;
                *dblen = ptoc_Var241->sylen;
                (*next_symbol)(&(*apc) , sql__nil(ptoc_Var244->partbufp , "vpc14.p" , 10644) , ptoc_Var244->part1len);
                if (_ptoc_MEMCMP(unsigned char , 18, (*(sqlgaentry *) sql__nil((*(sqlgatype *) sql__nil(ptoc_Var242->sqlgap , "vpc14.p" , 10645)).gaentry[sql__setrang(*ind , 1 , 8) + -1] , "vpc14.p" , 10645)).gaxuserrec.xu_serverdb , "                  ") == 0 ? 1 : 0)
                    {
                    _ptoc_ASSIGN(unsigned char , 18, (*(sqlgaentry *) sql__nil((*(sqlgatype *) sql__nil(ptoc_Var242->sqlgap , "vpc14.p" , 10648)).gaentry[sql__setrang(*ind , 1 , 8) + -1] , "vpc14.p" , 10648)).gaxuserrec.xu_serverdb, dbsname)
                    }
                }
            }
        else
            {
            if ((int ) ptoc_Var241->symb == 13)
                {
                pcnt = pcnt + 1;
                *dbpos = ptoc_Var241->sypos + 1;
                ptoc_Var241->syposacc = ptoc_Var241->sypos + 1;
                ptypentix = 0;
                p14btypentget(next_symbol , &(*apc) , &ptypentix , &pvatyp , &lastvatyp , 1);
                *dblen = ptoc_Var241->sypos - *dbpos;
                while ((ptoc_Var248 = (int ) (*(tpc_partbuffer *) sql__nil(ptoc_Var244->partbufp , "vpc14.p" , 10665))[sql__setrang(*dbpos + *dblen + -1 , 1 , 16000) + -1]) == 32 || ptoc_Var248 == 33)
                    *dblen = *dblen + -1;
                if ((int ) ptypentix == 0)
                    {
                    p11precomerror(&(*apc) , 79);
                    _ptoc_ASSIGN(unsigned char , 32, vnam, "SERVERDB_PARAMETER              ")
                    vnaml = 20;
                    p11nerrorlisting(&(*apc) , vnam , (short ) sql__setrang((short ) vnaml , -32768 , 32767) , (short ) sql__setrang((short ) pcnt , -32768 , 32767));
                    }
                else
                    {
                    (*(sqlgaentry *) sql__nil((*(sqlgatype *) sql__nil(ptoc_Var242->sqlgap , "vpc14.p" , 10678)).gaentry[sql__setrang(*ind , 1 , 8) + -1] , "vpc14.p" , 10678)).gavaindex = (short ) sql__setrang((short ) ((int ) ptoc_Var240->pcpa + 1) , -32768 , 32767);
                    ptoc_Var249 = &(*(tpc_arrndtype *) sql__nil(ptoc_Var246->ndtabp , "vpc14.p" , 10679))[sql__setrang((int ) ptoc_Var246->ndcnt , 1 , 100) + -1];
                    if (ptoc_Var249->ndsqlva1st != ptoc_Var249->ndsqlva1ix)
                        {
                        p19cparentry(&(*apc) , 1 , ptoc_Var249->ndsqlva1st , 0 , ptoc_Var249->ndsqlva1cnt);
                        }
                    p19cparentry(&(*apc) , 2 , ptoc_Var249->ndsqlva1ix , 0 , 0);
                    vix = (short ) sql__setrang((short ) (*(tpc_arrndtype *) sql__nil(ptoc_Var246->ndtabp , "vpc14.p" , 10690))[sql__setrang((int ) ptoc_Var246->ndcnt , 1 , 100) + -1].ndsqlva1ix , -32768 , 32767);
                    if ((int ) (*(sqlva1type *) sql__nil(ptoc_Var245->sqlv1p , "vpc14.p" , 10691))[sql__setrang((int ) vix , 1 , 10000) + -1].tsp_int2.C_sqlvasc.va1indi_sc == 3)
                        {
                        vix = (short ) sql__setrang((short ) (*(sqlva1type *) sql__nil(ptoc_Var245->sqlv1p , "vpc14.p" , 10693))[sql__setrang((int ) vix , 1 , 10000) + -1].tsp_int2.C_sqlvapt.va1ix_pt , -32768 , 32767);
                        }
                    pvalen = (short ) sql__setrang((short ) (*(sqlva2type *) sql__nil(ptoc_Var245->sqlv2p , "vpc14.p" , 10695))[sql__setrang((int ) (*(sqlva1type *) sql__nil(ptoc_Var245->sqlv1p , "vpc14.p" , 10695))[sql__setrang((int ) vix , 1 , 10000) + -1].tsp_int2.C_sqlvasc.va1indva2_sc , 1 , 5000) + -1].va2size , -32768 , 32767);
                    if (((ptoc_Var248 = (int ) pvatyp) < 6 || ptoc_Var248 > 10) && ptoc_Var248 != 15 && ptoc_Var248 != 20)
                        {
                        p11wrerrorlisting(&(*apc) , 5);
                        }
                    else
                        {
                        if ((int ) pvalen > 0)
                            {
                            len = 18;
                            if ((int ) pvatyp == 7)
                                {
                                len = len + 1;
                                }
                            if (len < (int ) pvalen)
                                {
                                p11wrerrorlisting(&(*apc) , 88);
                                _ptoc_ASSIGN(unsigned char , 32, vnam, "SERVERDB_PARAMETER              ")
                                vnaml = 20;
                                p11nerrorlisting(&(*apc) , vnam , (short ) sql__setrang((short ) vnaml , -32768 , 32767) , (short ) sql__setrang((short ) pcnt , -32768 , 32767));
                                ptoc_Var240->pcerror = 0;
                                }
                            }
                        }
                    }
                }
            else
                {
                p11wrerrorlisting(&(*apc) , 37);
                }
            }
        }
    if ((int ) ptoc_Var241->symb == 4 || (int ) ptoc_Var241->symb == 23)
        {
        *dbnodepos = 1;
        *dbnodelen = 0;
        _ptoc_ASSIGN(unsigned char , 64, (*(sqlgaentry *) sql__nil((*(sqlgatype *) sql__nil(ptoc_Var242->sqlgap , "vpc14.p" , 10740)).gaentry[sql__setrang(*ind , 1 , 8) + -1] , "vpc14.p" , 10740)).gaxuserrec.xu_servernode, "                                                                ")
        (*(sqlgaentry *) sql__nil((*(sqlgatype *) sql__nil(ptoc_Var242->sqlgap , "vpc14.p" , 10741)).gaentry[sql__setrang(*ind , 1 , 8) + -1] , "vpc14.p" , 10741)).ganodeindex = 0;
        }
    else
        {
        p10getkeyword(&(*apc) , sql__nil(ptoc_Var244->partbufp , "vpc14.p" , 10745) , &keyind);
        if (keyind != 75)
            {
            p11wrerrorlisting(&(*apc) , 37);
            }
        else
            {
            (*next_symbol)(&(*apc) , sql__nil(ptoc_Var244->partbufp , "vpc14.p" , 10751) , ptoc_Var244->part1len);
            if ((int ) ptoc_Var241->symb == 21 || (int ) ptoc_Var241->symb == 24)
                {
                p10stringliteralget(&(*apc) , sql__nil(ptoc_Var244->partbufp , "vpc14.p" , 10757));
                if (ptoc_Var241->sylen > 64)
                    {
                    p11wrerrorlisting(&(*apc) , 37);
                    }
                else
                    {
                    _ptoc_ASSIGN(unsigned char , 64, dbnode, "                                                                ")
                    (*(sqlgaentry *) sql__nil((*(sqlgatype *) sql__nil(ptoc_Var242->sqlgap , "vpc14.p" , 10764)).gaentry[sql__setrang(*ind , 1 , 8) + -1] , "vpc14.p" , 10764)).ganodeindex = 0;
                    s10mv16(ptoc_Var244->partsizlen , 64 , sql__nil(ptoc_Var244->partbufp , "vpc14.p" , 10765) , ptoc_Var241->sypos , dbnode , 1 , ptoc_Var241->sylen);
                    *dbnodepos = ptoc_Var241->sypos;
                    *dbnodelen = ptoc_Var241->sylen;
                    (*next_symbol)(&(*apc) , sql__nil(ptoc_Var244->partbufp , "vpc14.p" , 10769) , ptoc_Var244->part1len);
                    if (_ptoc_MEMCMP(unsigned char , 64, (*(sqlgaentry *) sql__nil((*(sqlgatype *) sql__nil(ptoc_Var242->sqlgap , "vpc14.p" , 10770)).gaentry[sql__setrang(*ind , 1 , 8) + -1] , "vpc14.p" , 10770)).gaxuserrec.xu_servernode , "                                                                ") == 0 ? 1 : 0)
                        {
                        _ptoc_ASSIGN(unsigned char , 64, (*(sqlgaentry *) sql__nil((*(sqlgatype *) sql__nil(ptoc_Var242->sqlgap , "vpc14.p" , 10773)).gaentry[sql__setrang(*ind , 1 , 8) + -1] , "vpc14.p" , 10773)).gaxuserrec.xu_servernode, dbnode)
                        }
                    }
                }
            else
                {
                if ((int ) ptoc_Var241->symb == 13)
                    {
                    pcnt = pcnt + 1;
                    *dbnodepos = ptoc_Var241->sypos + 1;
                    ptoc_Var241->syposacc = ptoc_Var241->sypos + 1;
                    ptypentix = 0;
                    p14btypentget(next_symbol , &(*apc) , &ptypentix , &pvatyp , &lastvatyp , 1);
                    *dbnodelen = ptoc_Var241->sypos - *dbnodepos;
                    while ((ptoc_Var248 = (int ) (*(tpc_partbuffer *) sql__nil(ptoc_Var244->partbufp , "vpc14.p" , 10790))[sql__setrang(*dbnodepos + *dbnodelen + -1 , 1 , 16000) + -1]) == 32 || ptoc_Var248 == 33)
                        *dbnodelen = *dbnodelen + -1;
                    if ((int ) ptypentix == 0)
                        {
                        p11precomerror(&(*apc) , 79);
                        _ptoc_ASSIGN(unsigned char , 32, vnam, "SERVERNODE_PARAMETER            ")
                        vnaml = 22;
                        p11nerrorlisting(&(*apc) , vnam , (short ) sql__setrang((short ) vnaml , -32768 , 32767) , (short ) sql__setrang((short ) pcnt , -32768 , 32767));
                        }
                    else
                        {
                        if ((int ) ptoc_Var241->symb != 4 && (int ) ptoc_Var241->symb != 23)
                            {
                            (*next_symbol)(&(*apc) , sql__nil(ptoc_Var244->partbufp , "vpc14.p" , 10806) , ptoc_Var244->part1len);
                            }
                        if ((int ) ptoc_Var241->symb == 4 || (int ) ptoc_Var241->symb == 23)
                            {
                            (*(sqlgaentry *) sql__nil((*(sqlgatype *) sql__nil(ptoc_Var242->sqlgap , "vpc14.p" , 10811)).gaentry[sql__setrang(*ind , 1 , 8) + -1] , "vpc14.p" , 10811)).ganodeindex = (short ) sql__setrang((short ) ((int ) ptoc_Var240->pcpa + 1) , -32768 , 32767);
                            ptoc_Var250 = &(*(tpc_arrndtype *) sql__nil(ptoc_Var246->ndtabp , "vpc14.p" , 10812))[sql__setrang((int ) ptoc_Var246->ndcnt , 1 , 100) + -1];
                            if (ptoc_Var250->ndsqlva1st != ptoc_Var250->ndsqlva1ix)
                                {
                                p19cparentry(&(*apc) , 1 , ptoc_Var250->ndsqlva1st , 0 , ptoc_Var250->ndsqlva1cnt);
                                }
                            p19cparentry(&(*apc) , 2 , ptoc_Var250->ndsqlva1ix , 0 , 0);
                            vix = (short ) sql__setrang((short ) (*(tpc_arrndtype *) sql__nil(ptoc_Var246->ndtabp , "vpc14.p" , 10823))[sql__setrang((int ) ptoc_Var246->ndcnt , 1 , 100) + -1].ndsqlva1ix , -32768 , 32767);
                            if ((int ) (*(sqlva1type *) sql__nil(ptoc_Var245->sqlv1p , "vpc14.p" , 10824))[sql__setrang((int ) vix , 1 , 10000) + -1].tsp_int2.C_sqlvasc.va1indi_sc == 3)
                                {
                                vix = (short ) sql__setrang((short ) (*(sqlva1type *) sql__nil(ptoc_Var245->sqlv1p , "vpc14.p" , 10826))[sql__setrang((int ) vix , 1 , 10000) + -1].tsp_int2.C_sqlvapt.va1ix_pt , -32768 , 32767);
                                }
                            pvalen = (short ) sql__setrang((short ) (*(sqlva2type *) sql__nil(ptoc_Var245->sqlv2p , "vpc14.p" , 10828))[sql__setrang((int ) (*(sqlva1type *) sql__nil(ptoc_Var245->sqlv1p , "vpc14.p" , 10828))[sql__setrang((int ) vix , 1 , 10000) + -1].tsp_int2.C_sqlvasc.va1indva2_sc , 1 , 5000) + -1].va2size , -32768 , 32767);
                            }
                        else
                            {
                            p11wrerrorlisting(&(*apc) , 37);
                            }
                        if (((ptoc_Var248 = (int ) pvatyp) < 6 || ptoc_Var248 > 10) && ptoc_Var248 != 15 && ptoc_Var248 != 20)
                            {
                            p11wrerrorlisting(&(*apc) , 5);
                            }
                        else
                            {
                            if ((int ) pvalen > 0)
                                {
                                len = 64;
                                if ((int ) pvatyp == 7)
                                    {
                                    len = len + 1;
                                    }
                                if (len < (int ) pvalen)
                                    {
                                    p11wrerrorlisting(&(*apc) , 88);
                                    _ptoc_ASSIGN(unsigned char , 32, vnam, "SERVERNODE_PARAMETER            ")
                                    vnaml = 22;
                                    p11nerrorlisting(&(*apc) , vnam , (short ) sql__setrang((short ) vnaml , -32768 , 32767) , (short ) sql__setrang((short ) pcnt , -32768 , 32767));
                                    ptoc_Var240->pcerror = 0;
                                    }
                                }
                            }
                        }
                    }
                else
                    {
                    p11wrerrorlisting(&(*apc) , 37);
                    }
                }
            }
        }
    ptoc_Var241->symb = 4;
    m90name(22 , "END p14gdbsname   ");
    }



int p14gtrace(next_symbol, apc, keyind)
int (*next_symbol)();
tpc_globals *apc;
int keyind;
    {
    tpc_globals *ptoc_Var251;
    tpr_scannertype *ptoc_Var252;
    sqlcatype *ptoc_Var253;
    tpc_cmd_part *ptoc_Var254;
    sqlgatype *ptoc_Var255;
    sqloatype *ptoc_Var256;
    int ptoc_Var257;
    m90print("PC14  p14gtrace ");
    ptoc_Var251 = &*apc;
    ptoc_Var252 = &ptoc_Var251->pcscan;
    ptoc_Var253 = &ptoc_Var251->sqlca;
    ptoc_Var254 = &ptoc_Var251->pccmdpart;
    ptoc_Var255 = (sqlgatype *) sql__nil(ptoc_Var253->sqlgap , "vpc14.p" , 10889);
    ptoc_Var256 = (sqloatype *) sql__nil(ptoc_Var253->sqloap , "vpc14.p" , 10889);
    switch (keyind)
        {
        case 74 :
            ptoc_Var256->oamodsettrty = 1;
            break;
        case 75 :
            ptoc_Var256->oamodsettrty = 2;
            break;
        case 63 :
            ptoc_Var256->oamodsettrty = 3;
            break;
        default :
            p11precomerror(&(*apc) , 81);
            break;
        }
    if ((int ) ptoc_Var251->pcerror == 0)
        {
        (*next_symbol)(&(*apc) , sql__nil(ptoc_Var254->partbufp , "vpc14.p" , 10905) , ptoc_Var254->part1len);
        if ((int ) ptoc_Var252->symb != 4 && (int ) ptoc_Var252->symb != 23)
            {
            p11precomerror(&(*apc) , 48);
            }
        }
    while ((ptoc_Var257 = (int ) ptoc_Var252->symb) != 4 && ptoc_Var257 != 23)
        (*next_symbol)(&(*apc) , sql__nil(ptoc_Var254->partbufp , "vpc14.p" , 10913) , ptoc_Var254->part1len);
    m90name(22 , "END p14gtrace     ");
    }



int p14traceline(next_symbol, apc, statemno)
int (*next_symbol)();
tpc_globals *apc;
int *statemno;
    {
    m90print("PC14  p14traceli");
    p14option(next_symbol , &(*apc) , &(*statemno));
    m90name(22 , "END p14traceline  ");
    }



char p14ifstbind(apc, kano)
tpc_globals *apc;
int kano;
    {
    char _p14ifstbind;
    char stbind;
    char pcstset;
    tpc_globals *ptoc_Var258;
    sqlkaentry *ptoc_Var259;
    m90print("PC14  p14ifstbin");
    ptoc_Var258 = &*apc;
    ptoc_Var259 = &(*(sqlkatype *) sql__nil(ptoc_Var258->sqlxa.sqlkap , "vpc14.p" , 10946))[sql__setrang(kano , 1 , 3000) + -1];
    stbind = 1;
    pcstset = 1;
    ptoc_Var259->kaversion = 0;
    if ((int ) ptoc_Var259->kaprindex != 0)
        {
        if ((int ) (*(sqlprtype *) sql__nil(ptoc_Var258->sqlxa.sqlprp , "vpc14.p" , 10953))[sql__setrang((int ) ptoc_Var259->kaprindex , 1 , 500) + -1].prarea != 1)
            {
            stbind = 0;
            }
        else
            {
            pcstset = 0;
            }
        }
    m90int2(14 , "pcst vor    " , (short ) sql__setrang(ptoc_Var258->pcst , -32768 , 32767));
    m90int2(14 , "kaStindex   " , (short ) sql__setrang(ptoc_Var259->kaStindex , -32768 , 32767));
    if ((char ) sql__setrang(stbind , 0 , 1) && (char ) sql__setrang(pcstset , 0 , 1))
        {
        if ((int ) ptoc_Var259->kaStindex + (int ) ptoc_Var259->kaStcount + -1 == (int ) ptoc_Var258->pcst)
            {
            ptoc_Var258->pcst = (short ) sql__setrang((short ) ((int ) ptoc_Var259->kaStindex + -1) , -32768 , 32767);
            p19setsqlstindex(&ptoc_Var258->pcst);
            }
        }
    m90int2(14 , "pcst nach   " , (short ) sql__setrang(ptoc_Var258->pcst , -32768 , 32767));
    m90int2(14 , "kaStindex   " , (short ) sql__setrang(ptoc_Var259->kaStindex , -32768 , 32767));
    if ((int ) ptoc_Var259->kaStcount > (int ) ptoc_Var258->pcststm)
        {
        ptoc_Var258->pcststm = (short ) sql__setrang(ptoc_Var259->kaStcount , -32768 , 32767);
        }
    _p14ifstbind = (char ) sql__setrang(stbind , 0 , 1);
    return(_p14ifstbind);
    }



int p14_dclgen(next_symbol, apc, dclgen, fnlen)
int (*next_symbol)();
tpc_globals *apc;
char *dclgen;
int *fnlen;
    {
    int keyind;
    int len;
    int i;
    int attrpos;
    int pos;
    tpc_globals *ptoc_Var260;
    tpr_scannertype *ptoc_Var261;
    sqlcatype *ptoc_Var262;
    tpc_cmd_part *ptoc_Var263;
    tpc_dclgen *ptoc_Var264;
    struct ptoc_Type78 *ptoc_Var265;
    struct ptoc_Type78 *ptoc_Var266;
    m90print("PC14  p14_dclgen");
    ptoc_Var260 = &*apc;
    ptoc_Var261 = &ptoc_Var260->pcscan;
    ptoc_Var262 = &ptoc_Var260->sqlca;
    ptoc_Var263 = &ptoc_Var260->pccmdpart;
    ptoc_Var264 = &ptoc_Var260->pcdclgen;
    (*next_symbol)(&(*apc) , sql__nil(ptoc_Var263->partbufp , "vpc14.p" , 11000) , ptoc_Var263->part1len);
    if ((int ) ptoc_Var261->symb == 4 || (int ) ptoc_Var261->symb == 23)
        {
        *dclgen = 0;
        (*(sqldatype *) sql__nil((*(sqlcxatype *) sql__nil(ptoc_Var262->sqlcxap , "vpc14.p" , 11005)).xasqldap.tpr_kind.C_cpr_kind_internal.sqldaptr , "vpc14.p" , 11005)).sqln = 0;
        (*(sqldatype *) sql__nil((*(sqlcxatype *) sql__nil(ptoc_Var262->sqlcxap , "vpc14.p" , 11006)).xasqldap.tpr_kind.C_cpr_kind_internal.sqldaptr , "vpc14.p" , 11006)).sqld = 0;
        }
    else
        {
        *dclgen = 1;
        ptoc_Var264->asclause = 0;
        ptoc_Var264->indclause = 0;
        _ptoc_ASSIGN(unsigned char , 64, ptoc_Var264->dcluser, "                                                                ")
        _ptoc_ASSIGN(unsigned char , 64, ptoc_Var264->dcltab, "                                                                ")
        ptoc_Var264->dcltabl = 0;
        ptoc_Var264->clause[0].dclfnlen = (short ) sql__setrang((short ) *fnlen , -32768 , 32767);
        i = 1;
        do
            {
            ptoc_Var265 = &ptoc_Var264->clause[sql__setrang(i , 1 , 2) + -1];
            s10fil2(sizeof(unsigned char [132]) , ptoc_Var265->dclattr , 1 , sizeof(unsigned char [132]) , (unsigned char)' ');
            ptoc_Var265->dclattrl = 0;
            if (i == 2)
                {
                break;
                }
            i += 1;
            }
        while (!0);
        p10getkeyword(&(*apc) , sql__nil(ptoc_Var263->partbufp , "vpc14.p" , 11027) , &keyind);
        ptoc_Var264->tabclause = 0;
        if (keyind == 122)
            {
            ptoc_Var264->tabclause = 122;
            }
        else
            {
            if (keyind == 29)
                {
                ptoc_Var264->tabclause = 29;
                }
            else
                {
                p11wrerrorlisting(&(*apc) , 37);
                }
            }
        if ((int ) ptoc_Var264->tabclause != 0)
            {
            (*next_symbol)(&(*apc) , sql__nil(ptoc_Var263->partbufp , "vpc14.p" , 11044) , ptoc_Var263->part1len);
            p10getkeyword(&(*apc) , sql__nil(ptoc_Var263->partbufp , "vpc14.p" , 11045) , &keyind);
            p19getname(&(*apc) , ptoc_Var264->dcltab , &len);
            ptoc_Var264->dcltabl = (short ) sql__setrang((short ) len , -32768 , 32767);
            _ptoc_ASSIGN(unsigned char , 64, ptoc_Var264->dclmodul, "                                                                ")
            (*next_symbol)(&(*apc) , sql__nil(ptoc_Var263->partbufp , "vpc14.p" , 11049) , ptoc_Var263->part1len);
            ptoc_Var261->symb = (short ) sql__setrang(p14symb(next_symbol , &(*apc)) , -32768 , 32767);
            if ((int ) ptoc_Var261->symb == 16)
                {
                s10mv8(18 , sizeof(unsigned char [64]) , ptoc_Var264->dcltab , 1 , ptoc_Var264->dcluser , 1 , sizeof(unsigned char [64]));
                _ptoc_ASSIGN(unsigned char , 64, ptoc_Var264->dcltab, "                                                                ")
                (*next_symbol)(&(*apc) , sql__nil(ptoc_Var263->partbufp , "vpc14.p" , 11057) , ptoc_Var263->part1len);
                p19getname(&(*apc) , ptoc_Var264->dcltab , &len);
                ptoc_Var264->dcltabl = (short ) sql__setrang((short ) len , -32768 , 32767);
                (*next_symbol)(&(*apc) , sql__nil(ptoc_Var263->partbufp , "vpc14.p" , 11060) , ptoc_Var263->part1len);
                }
            ptoc_Var261->symb = (short ) sql__setrang(p14symb(next_symbol , &(*apc)) , -32768 , 32767);
            if ((int ) ptoc_Var261->symb == 16)
                {
                (*next_symbol)(&(*apc) , sql__nil(ptoc_Var263->partbufp , "vpc14.p" , 11067) , ptoc_Var263->part1len);
                p19getname(&(*apc) , ptoc_Var264->dclmodul , &len);
                (*next_symbol)(&(*apc) , sql__nil(ptoc_Var263->partbufp , "vpc14.p" , 11069) , ptoc_Var263->part1len);
                }
            p10getkeyword(&(*apc) , sql__nil(ptoc_Var263->partbufp , "vpc14.p" , 11072) , &keyind);
            if (keyind == 5)
                {
                (*next_symbol)(&(*apc) , sql__nil(ptoc_Var263->partbufp , "vpc14.p" , 11076) , ptoc_Var263->part1len);
                p10getkeyword(&(*apc) , sql__nil(ptoc_Var263->partbufp , "vpc14.p" , 11077) , &keyind);
                switch (keyind)
                    {
                    case 88 :
                        ptoc_Var264->asclause = 3;
                        break;
                    case 129 :
                        ptoc_Var264->asclause = 1;
                        break;
                    case 120 :
                        ptoc_Var264->asclause = 2;
                        break;
                    default :
                        ptoc_Var264->asclause = 0;
                        break;
                    }
                (*next_symbol)(&(*apc) , sql__nil(ptoc_Var263->partbufp , "vpc14.p" , 11089) , ptoc_Var263->part1len);
                attrpos = ptoc_Var261->sypos;
                p10getkeyword(&(*apc) , sql__nil(ptoc_Var263->partbufp , "vpc14.p" , 11091) , &keyind);
                if (keyind != 52 && (int ) ptoc_Var261->symb != 4 && (int ) ptoc_Var261->symb != 23)
                    {
                    do
                        {
                        (*next_symbol)(&(*apc) , sql__nil(ptoc_Var263->partbufp , "vpc14.p" , 11097) , ptoc_Var263->part1len);
                        p10getkeyword(&(*apc) , sql__nil(ptoc_Var263->partbufp , "vpc14.p" , 11098) , &keyind);
                        }
                    while (!(keyind == 52 || (int ) ptoc_Var261->symb == 4 || (int ) ptoc_Var261->symb == 23));
                    len = ptoc_Var261->sypos - attrpos;
                    if (len > 80)
                        {
                        len = 80;
                        }
                    s10mv9(ptoc_Var263->partsizlen , sizeof(unsigned char [132]) , sql__nil(ptoc_Var263->partbufp , "vpc14.p" , 11108) , attrpos , ptoc_Var264->clause[0].dclattr , 1 , len);
                    ptoc_Var264->clause[0].dclattrl = (short ) sql__setrang((short ) len , -32768 , 32767);
                    }
                }
            attrpos = ptoc_Var261->syposacc;
            if (keyind == 52)
                {
                ptoc_Var264->indclause = 1;
                do
                    {
                    (*next_symbol)(&(*apc) , sql__nil(ptoc_Var263->partbufp , "vpc14.p" , 11121) , ptoc_Var263->part1len);
                    p10getkeyword(&(*apc) , sql__nil(ptoc_Var263->partbufp , "vpc14.p" , 11122) , &keyind);
                    }
                while (!((int ) ptoc_Var261->symb == 4 || (int ) ptoc_Var261->symb == 23));
                pos = attrpos;
                while (pos < ptoc_Var261->sypos)
                    {
                    pos = pos + 1;
                    if ((int ) (*(tpc_partbuffer *) sql__nil(ptoc_Var263->partbufp , "vpc14.p" , 11130))[sql__setrang(attrpos , 1 , 16000) + -1] == 32)
                        {
                        attrpos = attrpos + 1;
                        }
                    else
                        {
                        pos = ptoc_Var261->sypos;
                        }
                    }
                len = ptoc_Var261->sypos - attrpos;
                if (len > 80)
                    {
                    len = 80;
                    }
                s10mv9(ptoc_Var263->partsizlen , sizeof(unsigned char [132]) , sql__nil(ptoc_Var263->partbufp , "vpc14.p" , 11143) , attrpos , ptoc_Var264->clause[1].dclattr , 1 , len);
                ptoc_Var264->clause[1].dclattrl = (short ) sql__setrang((short ) len , -32768 , 32767);
                }
            if ((int ) ptoc_Var261->symb == 4 || (int ) ptoc_Var261->symb == 23)
                {
                i = 1;
                do
                    {
                    ptoc_Var266 = &ptoc_Var264->clause[sql__setrang(i , 1 , 2) + -1];
                    len = s30lnr(ptoc_Var266->dclattr , (unsigned char)' ' , 1 , (int ) ptoc_Var266->dclattrl);
                    if (len == 0)
                        {
                        ptoc_Var266->dclattrl = (short ) sql__setrang(ptoc_Var264->dcltabl , -32768 , 32767);
                        if (i == 1)
                            {
                            s10mv10(sizeof(unsigned char [64]) , sizeof(unsigned char [132]) , ptoc_Var264->dcltab , 1 , ptoc_Var266->dclattr , 1 , (int ) ptoc_Var266->dclattrl);
                            }
                        else
                            {
                            s10mv10(sizeof(unsigned char [64]) , sizeof(unsigned char [132]) , ptoc_Var264->dcltab , 1 , ptoc_Var266->dclattr , 2 , (int ) ptoc_Var266->dclattrl);
                            ptoc_Var266->dclattrl = (short ) sql__setrang((short ) ((int ) ptoc_Var266->dclattrl + 1) , -32768 , 32767);
                            ptoc_Var266->dclattr[0] = (unsigned char)'I';
                            }
                        p05up1casebuf(ptoc_Var266->dclattr , 1 , (int ) ptoc_Var266->dclattrl);
                        }
                    if (i == 2)
                        {
                        break;
                        }
                    i += 1;
                    }
                while (!0);
                }
            }
        }
    m90name(22 , "END p14_dclgen    ");
    }



int p14getname(next_symbol, apc, nam, len, macro, param)
int (*next_symbol)();
tpc_globals *apc;
tsp_knl_identifier nam;
int *len;
tsp_int2 *macro;
tsp_int2 *param;
    {
    int mlen;
    int mpos;
    int pos;
    tpc_globals *ptoc_Var267;
    tpc_cmd_part *ptoc_Var268;
    tpr_scannertype *ptoc_Var269;
    m90print("PC14  p14getname");
    ptoc_Var267 = &*apc;
    ptoc_Var268 = &ptoc_Var267->pccmdpart;
    ptoc_Var269 = &ptoc_Var267->pcscan;
    if (ptoc_Var269->sylen == 0)
        {
        (*next_symbol)(&(*apc) , sql__nil(ptoc_Var268->partbufp , "vpc14.p" , 11204) , ptoc_Var268->part1len);
        }
    mpos = ptoc_Var269->sypos;
    mlen = ptoc_Var269->sylen;
    if ((int ) ptoc_Var269->symb == 11 || (int ) ptoc_Var269->symb == 13)
        {
        if ((int ) ptoc_Var269->symb == 13)
            {
            *param = 1;
            *macro = 0;
            }
        else
            {
            *param = 0;
            *macro = 1;
            }
        mlen = ptoc_Var269->sylen + 1;
        }
    else
        {
        *macro = 0;
        *param = 0;
        if ((int ) ptoc_Var269->symb != 8 && (int ) ptoc_Var269->symb != 0)
            {
            if ((int ) ptoc_Var269->symb == 21 || (int ) ptoc_Var269->symb == 24)
                {
                m90int2(14 , "len         " , (short ) sql__setrang((short ) *len , -32768 , 32767));
                m90int2(14 , "mpos        " , (short ) sql__setrang((short ) mpos , -32768 , 32767));
                m90int2(14 , "symb        " , (short ) sql__setrang(ptoc_Var269->symb , -32768 , 32767));
                m90int2(14 , "sypos       " , (short ) sql__setrang((short ) ptoc_Var269->sypos , -32768 , 32767));
                m90int2(14 , "sylen       " , (short ) sql__setrang((short ) ptoc_Var269->sylen , -32768 , 32767));
                m90buf(14 , &(*(tpc_partbuffer *) sql__nil(ptoc_Var268->partbufp , "vpc14.p" , 11242))[0] , 1 , ptoc_Var268->part1len);
                if ((int ) (*(tpc_partbuffer *) sql__nil(ptoc_Var268->partbufp , "vpc14.p" , 11243))[sql__setrang(mpos , 1 , 16000) + -1] == 34)
                    {
                    mpos = mpos + 1;
                    mlen = mlen + -2;
                    }
                }
            else
                {
                p11precomerror(&(*apc) , 37);
                }
            }
        }
    _ptoc_ASSIGN(unsigned char , 64, nam, "                                                                ")
    *len = 0;
    pos = mpos;
    do
        {
        if ((int ) (*(tpc_partbuffer *) sql__nil(ptoc_Var268->partbufp , "vpc14.p" , 11261))[sql__setrang(pos , 1 , 16000) + -1] == 34 && (int ) (*(tpc_partbuffer *) sql__nil(ptoc_Var268->partbufp , "vpc14.p" , 11262))[sql__setrang(pos + 1 , 1 , 16000) + -1] == 34)
            {
            pos = pos + 1;
            }
        *len = *len + 1;
        nam[sql__setrang(*len , 1 , 64) + -1] = (unsigned char ) sql__setrang((*(tpc_partbuffer *) sql__nil(ptoc_Var268->partbufp , "vpc14.p" , 11267))[sql__setrang(pos , 1 , 16000) + -1] , 0 , 255);
        pos = pos + 1;
        }
    while (!(*len >= 64 || pos >= mpos + mlen));
    if ((int ) (*(tpc_partbuffer *) sql__nil(ptoc_Var268->partbufp , "vpc14.p" , 11272))[sql__setrang(mpos + -1 , 1 , 16000) + -1] != 34)
        {
        p05up2casebuf(nam , 1 , *len);
        }
    m90identifier(14 , nam);
    m90name(22 , "END p14getname    ");
    }



int p14oradescribe(next_symbol, apc, orakatyp)
int (*next_symbol)();
tpc_globals *apc;
tsp_int2 *orakatyp;
    {
    int keyind;
    tpc_globals *ptoc_Var270;
    tpr_scannertype *ptoc_Var271;
    tpc_cmd_part *ptoc_Var272;
    sqlxatype *ptoc_Var273;
    m90print("PC14  p14oradesc");
    ptoc_Var270 = &*apc;
    ptoc_Var271 = &ptoc_Var270->pcscan;
    ptoc_Var272 = &ptoc_Var270->pccmdpart;
    ptoc_Var273 = &ptoc_Var270->sqlxa;
    *orakatyp = 24;
    p10getkeyword(&(*apc) , sql__nil(ptoc_Var272->partbufp , "vpc14.p" , 11297) , &keyind);
    if (keyind == 95)
        {
        (*next_symbol)(&(*apc) , sql__nil(ptoc_Var272->partbufp , "vpc14.p" , 11301) , ptoc_Var272->part1len);
        p10getkeyword(&(*apc) , sql__nil(ptoc_Var272->partbufp , "vpc14.p" , 11302) , &keyind);
        if (keyind != 62)
            {
            p11precomerror(&(*apc) , 37);
            }
        else
            {
            (*next_symbol)(&(*apc) , sql__nil(ptoc_Var272->partbufp , "vpc14.p" , 11308) , ptoc_Var272->part1len);
            p10getkeyword(&(*apc) , sql__nil(ptoc_Var272->partbufp , "vpc14.p" , 11309) , &keyind);
            if (keyind != 44)
                {
                p11precomerror(&(*apc) , 37);
                }
            else
                {
                (*next_symbol)(&(*apc) , sql__nil(ptoc_Var272->partbufp , "vpc14.p" , 11314) , ptoc_Var272->part1len);
                }
            }
        }
    else
        {
        if (keyind == 9)
            {
            *orakatyp = 10;
            (*next_symbol)(&(*apc) , sql__nil(ptoc_Var272->partbufp , "vpc14.p" , 11324) , ptoc_Var272->part1len);
            p10getkeyword(&(*apc) , sql__nil(ptoc_Var272->partbufp , "vpc14.p" , 11325) , &keyind);
            if (keyind != 136)
                {
                p11precomerror(&(*apc) , 37);
                }
            else
                {
                (*next_symbol)(&(*apc) , sql__nil(ptoc_Var272->partbufp , "vpc14.p" , 11331) , ptoc_Var272->part1len);
                p10getkeyword(&(*apc) , sql__nil(ptoc_Var272->partbufp , "vpc14.p" , 11332) , &keyind);
                if (keyind != 44)
                    {
                    p11precomerror(&(*apc) , 37);
                    }
                else
                    {
                    (*next_symbol)(&(*apc) , sql__nil(ptoc_Var272->partbufp , "vpc14.p" , 11337) , ptoc_Var272->part1len);
                    }
                }
            }
        }
    m90name(22 , "END p14oradescribe");
    }



int p14kaentryinit(apc)
tpc_globals *apc;
    {
    tpc_globals *ptoc_Var274;
    sqlxatype *ptoc_Var275;
    sqlxatype *ptoc_Var276;
    sqlkaentry *ptoc_Var277;
    sqlcatype *ptoc_Var278;
    sqlratype *ptoc_Var279;
    m90print("PC14  p14kaentry");
    ptoc_Var274 = &*apc;
    ptoc_Var275 = &ptoc_Var274->sqlxa;
    p19sqlkaallocate(&ptoc_Var274->pcka);
    ptoc_Var276 = &ptoc_Var274->sqlxa;
    ptoc_Var277 = &(*(sqlkatype *) sql__nil(ptoc_Var276->sqlkap , "vpc14.p" , 11361))[sql__setrang((int ) ptoc_Var274->pcka , 1 , 3000) + -1];
    ptoc_Var278 = &ptoc_Var274->sqlca;
    ptoc_Var279 = (sqlratype *) sql__nil(ptoc_Var278->sqlrap , "vpc14.p" , 11361);
    ptoc_Var277->katyp = (short ) sql__setrang(ptoc_Var274->pcan.ancomtyp , -32768 , 32767);
    ptoc_Var277->kastate = 0;
    ptoc_Var277->kapacount = 0;
    ptoc_Var277->kapaindex = 0;
    ptoc_Var277->kaStcount = 0;
    ptoc_Var277->kaStindex = 0;
    ptoc_Var277->kaprindex = 0;
    ptoc_Var277->kaparamcnt = 0;
    ptoc_Var277->kaversion = -1;
    ptoc_Var277->kadiindex = 0;
    ptoc_Var277->kafaindex = 0;
    ptoc_Var277->kamacro = 0;
    if ((int ) ptoc_Var274->pcinpind == 2)
        {
        ptoc_Var277->kalineno = (short ) sql__setrang(ptoc_Var274->pclno.lnosrc , -32768 , 32767);
        }
    else
        {
        ptoc_Var277->kalineno = (short ) sql__setrang((short ) -(int ) ptoc_Var274->pclno.lnoexp , -32768 , 32767);
        }
    ptoc_Var277->kamode = (short ) sql__setrang(ptoc_Var279->rakamode , -32768 , 32767);
    ptoc_Var277->kaatindex = (short ) sql__setrang(ptoc_Var274->pckaatind , -32768 , 32767);
    ptoc_Var277->kacuindex = 0;
    ptoc_Var276->xakano = (short ) sql__setrang(ptoc_Var274->pcka , -32768 , 32767);
    }
