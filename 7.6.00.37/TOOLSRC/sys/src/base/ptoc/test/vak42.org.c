#include <ptoc.h>

typedef
  unsigned char tsp00_Uint1;
typedef
  short tsp00_Int2;
typedef
  tsp00_Int2 tsp00_Uint2;
typedef
  int tsp00_Int4;
typedef
  double tsp00_Longreal;
typedef
  unsigned char *tsp00_Addr;
typedef
  tsp00_Uint1 tsp_int1;
typedef
  tsp00_Int2 tsp_int2;
typedef
  tsp00_Int4 tsp_int4;
typedef
  float tsp_shortreal;
typedef
  tsp00_Longreal tsp_longreal;
typedef
  double tsp00_8ByteCounter;
typedef
  unsigned char tsp00_NumError;
typedef
  tsp00_NumError tsp_num_error;
typedef
  unsigned char tsp_sqlmode;
typedef
  unsigned char tsp00_VfType;
typedef
  unsigned char tsp00_C1[1];
typedef
  unsigned char tsp00_C2[2];
typedef
  unsigned char tsp00_C3[3];
typedef
  unsigned char tsp00_C4[4];
typedef
  unsigned char tsp00_C6[6];
typedef
  unsigned char tsp00_C8[8];
typedef
  unsigned char tsp00_C10[10];
typedef
  unsigned char tsp00_C12[12];
typedef
  unsigned char tsp00_C14[14];
typedef
  unsigned char tsp00_C16[16];
typedef
  unsigned char tsp00_C18[18];
typedef
  unsigned char tsp00_C20[20];
typedef
  unsigned char tsp00_C24[24];
typedef
  unsigned char tsp00_C30[30];
typedef
  unsigned char tsp00_C32[32];
typedef
  unsigned char tsp00_C40[40];
typedef
  unsigned char tsp00_C48[48];
typedef
  unsigned char tsp00_C64[64];
typedef
  unsigned char tsp00_C120[120];
typedef
  unsigned char tsp00_C256[256];
typedef
  unsigned char tsp00_Buf[8192];
typedef
  tsp00_Uint1 tsp00_CodeType;
typedef
  tsp00_C8 tsp00_Date;
typedef
  tsp00_C18 tsp00_DbName;
typedef
  tsp00_C18 tsp00_Identifier;
typedef
  tsp00_C64 tsp00_KnlIdentifier;
typedef
  unsigned char tsp00_Key[1024];
typedef
  unsigned char tsp00_Line[132];
typedef
  unsigned char tsp00_MoveObj[8192000];
typedef
  tsp00_C64 tsp00_NodeId;
typedef
  tsp00_C20 tsp00_Number;
typedef
  tsp00_Int4 tsp00_PageNo;
typedef
  unsigned char tsp00_ResNum[7];
typedef
  tsp00_C12 tsp00_Sname;
typedef
  tsp00_Int4 tsp00_TaskId;
typedef
  tsp00_C18 tsp00_TermId;
typedef
  tsp00_C8 tsp00_Time;
typedef
  tsp00_C40 tsp00_Version;
typedef
  tsp00_C64 tsp00_VFilename;
typedef
  unsigned char tsp00_WarningSet[2];
typedef
  unsigned char tsp_trace[16];
typedef
  char *tsp00_BoolAddr;
typedef
  tsp00_Buf *tsp_bufaddr;
typedef
  tsp_bufaddr tsp00_BufAddr;
typedef
  tsp00_Key *tsp00_KeyAddr;
typedef
  tsp00_MoveObj *tsp_moveobj_ptr;
typedef
  tsp_moveobj_ptr tsp00_MoveObjPtr;
typedef
  tsp00_C1 tsp_c1;
typedef
  tsp00_C2 tsp_c2;
typedef
  tsp00_C3 tsp_c3;
typedef
  tsp00_C4 tsp_c4;
typedef
  unsigned char tsp_c5[5];
typedef
  tsp00_C6 tsp_c6;
typedef
  tsp00_C8 tsp_c8;
typedef
  tsp00_C10 tsp_c10;
typedef
  tsp00_C12 tsp_c12;
typedef
  tsp00_C16 tsp_c16;
typedef
  tsp00_C18 tsp_c18;
typedef
  tsp00_C20 tsp_c20;
typedef
  tsp00_C24 tsp_c24;
typedef
  unsigned char tsp_c50[50];
typedef
  tsp00_C64 tsp_c64;
typedef
  unsigned char tsp_c80[80];
typedef
  unsigned char tsp_c132[132];
typedef
  tsp00_C256 tsp_c256;
typedef
  tsp00_Buf tsp_buf;
typedef
  unsigned char tsp_cryptpw[24];
typedef
  tsp00_CodeType tsp_code_type;
typedef
  tsp00_DbName tsp_dbname;
typedef
  tsp00_KnlIdentifier tsp_knl_identifier;
typedef
  tsp00_Key tsp_key;
typedef
  tsp00_MoveObj tsp_moveobj;
typedef
  tsp00_NodeId tsp_nodeid;
typedef
  tsp00_Number tsp_number;
typedef
  tsp00_TaskId tsp_process_id;
typedef
  tsp00_ResNum tsp_resnum;
typedef
  unsigned char tsp_sqlstate[5];
typedef
  tsp00_TermId tsp_termid;
typedef
  unsigned char tsp_timestamp[20];
typedef
  tsp00_Version tsp_version;
typedef
  tsp00_WarningSet tsp_warningset;
typedef
  tsp00_KnlIdentifier *tsp_knl_identifier_ptr;
typedef
  struct ptoc_Type0
    {
    union
      {
      struct
        {
        tsp00_Int2 mapInt_sp00;
        } C_true;
      struct
        {
        tsp00_C2 mapC2_sp00;
        } C_false;
      } boolean;
    } tsp00_IntMapC2;
typedef
  struct ptoc_Type1
    {
    union
      {
      struct
        {
        tsp_int2 map_int;
        } C_true;
      struct
        {
        tsp_c2 map_c2;
        } C_false;
      } boolean;
    } tsp_int_map_c2;
typedef
  struct ptoc_Type2
    {
    tsp00_BoolAddr cancelb_ptr;
    tsp00_Int2 msgno;
    char first_call;
    char stop_msg_set;
    tsp00_KnlIdentifier curr_user;
    tsp00_KnlIdentifier curr_group;
    tsp00_KnlIdentifier owner_sysdba;
    tsp00_KnlIdentifier system_user;
    tsp00_KnlIdentifier user_system_user;
    tsp00_C256 stop_msg;
    unsigned char proc_type;
    tsp00_C1 kernel_dca_filler;
    tsp00_Int2 mess_type;
    char firstcall;
    char lastcall;
    tsp00_Addr value_p;
    tsp00_Int4 value_size;
    tsp00_Addr code_p;
    tsp00_Int4 code_size;
    } tsp_kernel_dca;
typedef
  struct ptoc_Type3
    {
    char to_cancel;
    char rte_fill1;
    tsp00_Int2 virt_reads;
    tsp00_Int4 file_root;
    tsp00_Int4 file_record_cnt;
    } tsp00_RteCommRec;
typedef
  tsp00_RteCommRec *tsp00_RteCommAddr;
typedef
  struct ptoc_Type4
    {
    tsp00_Addr idispatch;
    tsp00_Addr ifactory;
    tsp00_Addr session_context;
    tsp00_C4 session;
    tsp00_MoveObjPtr param_stack;
    tsp00_Int4 param_stack_len;
    tsp00_Int4 dispid;
    tsp00_Int4 hresult;
    tsp00_C4 trans;
    tsp00_Int2 monitor_slot_id;
    char monitoring;
    tsp00_C64 method_name;
    tsp00_C64 errtext;
    } tsp_dcom_dispatch_parms;
typedef
  struct ptoc_Type5
    {
    tsp00_Addr session_handle;
    tsp00_Int4 retcode;
    tsp00_Addr idispatch;
    tsp00_C16 iid;
    tsp00_C4 session;
    } tsp_dcom_monitor_parms;
typedef
  unsigned char tsp1_warningset_intern[2];
typedef
  unsigned char tsp1_command_options[1];
typedef
  unsigned char tsp1_part_kind;
typedef
  unsigned char tsp1_part_attr[1];
typedef
  struct ptoc_Type6
    {
    tsp1_part_kind sp1p_part_kind;
    tsp1_part_attr sp1p_attributes;
    tsp_int2 sp1p_arg_count;
    tsp_int4 sp1p_segm_offset;
    tsp_int4 sp1p_buf_len;
    tsp_int4 sp1p_buf_size;
    } tsp1_part_header;
typedef
  struct ptoc_Type7
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
        tsp1_part_kind sp1p_part_kind;
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
  tsp1_part *tsp1_part_ptr;
typedef
  struct ptoc_Type8
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
  struct ptoc_Type9
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
        tsp1_part_kind sp1p_part_kind;
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
  struct ptoc_Type10
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
  struct ptoc_Type11
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
  unsigned char tsp1_param_opt_set[1];
typedef
  struct ptoc_Type12
    {
    tsp1_param_opt_set sp1i_mode;
    unsigned char sp1i_io_type;
    unsigned char sp1i_data_type;
    tsp_int1 sp1i_frac;
    tsp_int2 sp1i_length;
    tsp_int2 sp1i_in_out_len;
    tsp_int4 sp1i_bufpos;
    } tsp1_param_info;
typedef
  struct ptoc_Type13
    {
    unsigned char para_type;
    unsigned char para_frac;
    tsp_int2 para_length;
    } tsp1_param_spec_list[2000];
typedef
  tsp00_Addr tgg00_VoidPtr;
typedef
  struct ptoc_Type14
    {
    tsp00_C4 ci4_gg00;
    } tgg90_Cint4;
typedef
  struct ptoc_Type15
    {
    tsp00_C6 ci6_gg00;
    } tgg90_Cint6;
typedef
  struct ptoc_Type16
    {
    tsp00_C2 ci2_gg00;
    } tgg91_FileVersion;
typedef
  tgg90_Cint4 tgg91_SessionNo;
typedef
  tgg90_Cint6 tgg91_TransNo;
typedef
  int tgg00_SubtransNo;
typedef
  unsigned char tgg00_BdUseSet[1];
typedef
  unsigned char tgg00_CacheType;
typedef
  unsigned char tgg00_Debug;
typedef
  unsigned char tgg00_DumpStateSet[4];
typedef
  unsigned char tgg00_FiletypeSet[1];
typedef
  unsigned char tgg00_HandlingSet[2];
typedef
  unsigned char tgg00_LockReqMode;
typedef
  unsigned char tgg00_LockReqState[1];
typedef
  unsigned char tgg00_MessType;
typedef
  unsigned char tgg00_MessType2;
typedef
  unsigned char tgg00_MonitorPart;
typedef
  unsigned char tgg00_PageMode[1];
typedef
  unsigned char tgg00_PageType;
typedef
  unsigned char tgg00_RefInfoIndex;
typedef
  unsigned char tgg00_ShowKind;
typedef
  unsigned char tgg00_TransState[2];
typedef
  unsigned char tgg00_AuxBuffers[16][8192];
typedef
  tgg00_AuxBuffers *tgg00_AuxBuffersPtr;
typedef
  tsp00_Int2 tgg00_BasisError;
typedef
  unsigned char tgg00_DoubleBuf[16384];
typedef
  tsp00_C16 tgg00_Filename;
typedef
  tsp00_VfType tgg00_Filetypes[32];
typedef
  tsp00_C64 tgg00_MediaName;
typedef
  int tgg00_ObjFileNo;
typedef
  unsigned char tgg00_PageHeadStr[8];
typedef
  tsp00_Int4 tgg00_PagePos;
typedef
  unsigned char tgg00_PrivilegeSet[1];
typedef
  unsigned char tgg00_RecBody[8184];
typedef
  int tgg00_RootList[6];
typedef
  tsp00_C2 tgg00_ServerdbNo;
typedef
  unsigned char tgg00_SfpInitPart[2200];
typedef
  tsp00_C8 tgg00_Surrogate;
typedef
  unsigned char tgg00_Syskey[140];
typedef
  int tgg00_TapeCount[32];
typedef
  tsp00_C64 tgg00_TapeNames[32];
typedef
  tsp00_Int2 tgg00_TransIndex;
typedef
  unsigned char tgg00_FileIdHead[24];
typedef
  tsp00_C4 tgg00_UserId;
typedef
  struct ptoc_Type15 tgg00_UserRef;
typedef
  struct ptoc_Type17
    {
    tsp00_C12 utidId_gg00;
    tsp00_Int4 utidLineNo_gg00;
    } tgg00_UtilCmdId;
typedef
  struct ptoc_Type18
    {
    tsp00_C12 mon_parseid;
    tgg00_Surrogate mon_cmdid;
    tsp00_C40 mon_strat;
    tsp00_C3 mon_copy_result;
    char mon_is_select;
    tsp00_Int4 mon_rows_read;
    tsp00_Int4 mon_rows_qual;
    tsp00_Int4 mon_rows_fetched;
    tsp00_Int4 mon_fetch_calls;
    tsp00_Int4 mon_virt_reads;
    tsp00_Int4 mon_kb_calls;
    tsp00_Int4 mon_waits;
    tsp00_Int4 mon_suspends;
    tsp00_Int4 mon_phys_ios;
    tsp00_Int4 mon_sec;
    tsp00_Int4 mon_microsec;
    tsp00_Int2 mon_strat_len;
    tsp00_Int2 mon_rootlist_elems;
    tgg00_RootList mon_rootlist;
    } tgg00_MonitorRec;
typedef
  struct ptoc_Type19
    {
    tgg00_MonitorRec umi_curr_monitor;
    tgg00_MonitorRec umi_sum_monitor;
    tsp00_KnlIdentifier umi_username;
    tsp00_TermId umi_termid;
    tsp00_C3 umi_parse_again;
    char umi_control_session;
    tsp00_Int4 umi_obj_hist_count;
    tsp00_Int4 umi_values_len;
    tsp00_Int4 umi_values_size;
    tsp00_MoveObjPtr umi_values;
    } tgg00_UserMonitorInfo;
typedef
  tgg00_UserMonitorInfo *tgg00_UserMonitorInfoPtr;
typedef
  struct ptoc_Type20
    {
    tgg00_AuxBuffersPtr tdcAuxBuffers_gg00;
    tgg00_UserMonitorInfoPtr tdc_monitor_info;
    tsp00_PageNo tdc_fdir_root;
    tsp00_Int4 tdc_cache_size;
    char tdc_cache_active;
    char tdc_fill1;
    tsp00_Int2 tdc_fill2;
    tsp00_Int4 tdcAuxBufferCount_gg00;
    } tgg00_TempDataCache;
typedef
  tgg00_TempDataCache *tgg00_TempDataCachePtr;
typedef
  struct ptoc_Type21
    {
    tsp00_Int4 plTop_gg00;
    int plPno_gg00[10];
    } tgg00_PnoList;
typedef
  struct ptoc_Type22
    {
    tsp00_Int2 sripBlocksize_gg00;
    tsp00_Int2 sripHostTapeNum_gg00;
    tsp00_Int4 sripFileVersion_gg00;
    char sripIsAutoLoad_gg00;
    char sripWithCheckpoint_gg00;
    char sripIsRestoreConfig_gg00;
    char sripFiller1_gg00;
    tgg00_TapeNames sripHostTapenames_gg00;
    tgg00_Filetypes sripHostFiletypes_gg00;
    tgg00_TapeCount sripHostTapecount_gg00;
    tgg00_MediaName sripMedianame_gg00;
    tsp00_Date sripUntilDate_gg00;
    tsp00_Time sripUntilTime_gg00;
    tgg00_UtilCmdId sripUtilCmdId_gg00;
    tsp00_DbName sripConfigDbName_gg00;
    tsp00_Int2 sripFiller2_gg00;
    } tgg00_SaveRestoreInputParam;
typedef
  struct ptoc_Type23
    {
    union
      {
      struct
        {
        tsp00_PageNo pageId_gg00;
        tgg00_PageType pageType_gg00;
        unsigned char pageType2_gg00;
        unsigned char pageCheck_gg00;
        tgg00_PageMode pageMode_gg00;
        } C_1;
      struct
        {
        tgg00_PageHeadStr pageHeaderStr_gg00;
        } C_2;
      } integer;
    } tgg00_PageHeader;
typedef
  struct ptoc_Type24
    {
    tsp00_Int2 term_reclen;
    tsp00_Int2 term_keylen;
    tsp00_Int2 term_varcol_offset;
    tsp00_Int2 term_varcol_cnt;
    tgg00_Surrogate term_surrogate;
    tsp00_C2 term_entrytype;
    tsp00_C2 term_linkage;
    tsp00_KnlIdentifier term_name;
    tsp00_C2 term_segmentid;
    tsp00_CodeType term_code;
    char term_valid;
    tsp00_Int2 term_filler1;
    tsp00_Int2 term_count;
    struct ptoc_Type25
      {
      tsp00_C1 td_internal;
      tsp00_C1 td_external;
      tsp00_C8 td_comment;
      tsp00_C2 td_filler;
      } term_desc[128];
    } tgg00_TermsetRecord;
typedef
  struct ptoc_Type26
    {
    tsp00_Int4 timeCmd_gg00;
    tsp00_Int4 timeReq_gg00;
    } tgg00_TimeoutGlobals;
typedef
  struct ptoc_Type27
    {
    union
      {
      struct
        {
        tsp00_Int2 keyRecLenSpace_gg00;
        tsp00_Int2 keyLen_gg00;
        tsp00_Int2 keyVarOffSpace_gg00;
        tsp00_Int2 keyVarCntSpace_gg00;
        tsp00_Key keyVal_gg00;
        } C_1;
      struct
        {
        tsp00_Int2 kfill;
        tsp00_Int2 len;
        tsp00_Int2 kfill1;
        tsp00_Int2 kfill2;
        tsp00_Key k;
        } C_11;
      } integer;
    } tgg00_Lkey;
typedef
  struct ptoc_Type28
    {
    tsp00_Int2 csi_cnt;
    tsp00_Int2 csi_fill1;
    tsp00_Int4 csi_fill2;
    struct ptoc_Type29
      {
      tgg00_Surrogate ocs_seq_surrogate;
      tsp00_Number ocs_seq_value;
      tsp00_Int4 ocs_seq_filler;
      } csi_values[32];
    } tgg00_CurrSeqInfo;
typedef
  tgg00_CurrSeqInfo *tgg00_CurrSeqInfoPtr;
typedef
  struct ptoc_Type30
    {
    tgg91_SessionNo tcdSessionId_gg00;
    tgg91_TransNo tcdTransId_gg00;
    tgg91_TransNo tcdWriteTransId_gg00;
    tgg00_SubtransNo tcdSubtransId_gg00;
    tsp00_Int4 tcdChild_gg00;
    tgg91_TransNo tcdConsistView_gg00;
    tsp00_Uint2 tcdFiller1_gg00;
    tgg00_TransState tcdState_gg00;
    tgg00_BasisError tcdError_gg00;
    tsp00_WarningSet tcdWarning_gg00;
    tsp00_Uint2 tcdObjReqTimeout_gg00;
    tsp00_Int4 tcdSessionAuxCounter_gg00;
    tsp00_Int4 tcdTempCount_gg00;
    tsp00_8ByteCounter tcdIoCount_gg00;
    } tgg00_TransChild;
typedef
  tgg00_TransChild *tgg00_TransChildPtr;
typedef
  struct ptoc_Type31
    {
    tgg91_SessionNo trSessionId_gg00;
    tgg91_TransNo trTransId_gg00;
    tgg91_TransNo trWriteTransId_gg00;
    tgg00_SubtransNo trSubtransId_gg00;
    tsp00_Int4 trChild_gg00;
    tgg91_TransNo trConsistView_gg00;
    tsp00_Uint2 trFiller1_gg00;
    tgg00_TransState trState_gg00;
    tgg00_BasisError trError_gg00;
    tsp00_WarningSet trWarning_gg00;
    tsp00_Uint2 trObjReqTimeout_gg00;
    tsp00_Int4 trSessionAuxCounter_gg00;
    tsp00_Int4 trTempCount_gg00;
    tsp00_8ByteCounter trIoCount_gg00;
    tsp00_TaskId trTaskId_gg00;
    tgg00_TransIndex trIndex_gg00;
    tsp00_Int2 trBdExclFileLocks_gg00;
    tsp00_8ByteCounter trRowLocksPerSqlCmd_gg00;
    tsp00_RteCommAddr trRteCommPtr_gg00;
    tgg00_TempDataCachePtr trBdTcachePtr_gg00;
    tgg00_VoidPtr trAllocator_gg00;
    tgg00_VoidPtr trLogContext_gg00;
    tgg00_CurrSeqInfoPtr trSeqInfoPtr_gg00;
    } tgg00_TransContext;
typedef
  tgg00_TransContext *tgg00_TransContextPtr;
typedef
  struct ptoc_Type32
    {
    tsp00_PageNo tpsPno_gg00;
    tsp00_Int4 tpsIndex_gg00;
    } tgg00_FilePos;
typedef
  struct ptoc_Type33
    {
    union
      {
      struct
        {
        tsp00_Int4 fileRootCheck_gg00;
        tsp00_Int4 fileLeafNodes_gg00;
        tgg00_UserRef fileUserRef_gg00;
        tgg91_FileVersion fileVersion_gg00;
        tsp00_PageNo fileRoot_gg00;
        tgg00_HandlingSet fileHandling_gg00;
        tgg00_BdUseSet fileBdUse_gg00;
        tgg00_FiletypeSet fileType_gg00;
        tgg00_Filename fileName_gg00;
        } C_1;
      struct
        {
        tsp00_C8 fileRootNodes_2_gg00;
        tsp00_C4 fileTriggerKey_gg00;
        tgg00_ServerdbNo fileResultSite_gg00;
        } C_2;
      struct
        {
        tsp00_C8 fileRootNodes_3_gg00;
        tsp00_Int4 fileConstId_gg00;
        } C_3;
      struct
        {
        tgg00_FileIdHead fileHeader__4_gg00;
        unsigned char fileTfn_gg00;
        tsp00_C1 fileTfnNo_gg00;
        tgg00_Surrogate fileTabId_gg00;
        } C_4;
      struct
        {
        tgg00_FileIdHead fileHeader__5_gg00;
        unsigned char fileTfn______5_gg00;
        unsigned char fileTfnNo____5_gg00;
        tsp00_Uint1 fileContObjFileNo_gg00;
        unsigned char fileObjFileType_gg00;
        tsp00_Uint1 fileNumObjKeyFiles_gg00;
        tsp00_C3 fileFiller3_gg00;
        tgg00_ObjFileNo fileObjFileNo_gg00;
        tsp00_C4 fileFiller4_gg00;
        } C_5;
      struct
        {
        tgg00_FileIdHead fileHeader__6_gg00;
        unsigned char fileTfn______6_gg00;
        unsigned char fileTfnLog_gg00;
        tgg91_TransNo fileTransId_gg00;
        } C_6;
      struct
        {
        tgg00_FileIdHead fileHeader__7_gg00;
        unsigned char fileTfn______7_gg00;
        tsp00_C1 fileTfnNo____7_gg00;
        tgg91_TransNo fileTransId__7_gg00;
        tsp00_Int4 fileAuxCnt_gg00;
        unsigned char fileOldTfn_gg00;
        tsp00_C1 fileFiller1_gg00;
        tsp00_C2 fileFiller2_gg00;
        } C_7;
      struct
        {
        tgg00_FileIdHead fileHeader__8_gg00;
        unsigned char fileTfn______8_gg00;
        tsp00_C1 fileTfnNo____8_gg00;
        tsp00_Int2 fileZeroSite_gg00;
        tgg91_SessionNo fileSession_gg00;
        unsigned char fileTfnTemp_gg00;
        tsp00_C1 fileFsNode_gg00;
        tsp00_C2 fileLevel_gg00;
        tsp00_Int4 fileTempCnt_gg00;
        } C_8;
      struct
        {
        tgg00_FileIdHead fileHeader__9_gg00;
        unsigned char fileTfn______9_gg00;
        tsp00_C1 fileTfnNo____9_gg00;
        tsp00_Int2 fileZeroSite_9_gg00;
        tgg91_SessionNo fileSession__9_gg00;
        unsigned char fileTfnTemp__9_gg00;
        tsp00_C1 fileFsNode___9_gg00;
        tgg91_TransNo fileTempTrans_gg00;
        } C_9;
      struct
        {
        tgg00_FileIdHead fileHeader_10_gg00;
        unsigned char fileTfn_____10_gg00;
        tsp00_PageNo fileQueueInvRoot_gg00;
        } C_10;
      struct
        {
        tgg00_FileIdHead fileHeader_11_gg00;
        unsigned char fileTfn_____11_gg00;
        tsp00_C1 fileTfnNo___11_gg00;
        tsp00_Uint1 fileObjKeyFileIndex_gg00;
        tsp00_C1 fileFiller5_gg00;
        tsp00_C8 fileFiller6_gg00;
        tsp00_C4 fileObjKeyFileId_gg00;
        } C_11;
      } integer;
    } tgg00_FileId;
typedef
  struct ptoc_Type34
    {
    tgg00_Surrogate lockTabId_gg00;
    tgg00_ServerdbNo lockSite_gg00;
    tgg00_LockReqMode lockMode_gg00;
    tgg00_LockReqState lockState_gg00;
    tsp00_Int2 lockKeyLen_gg00;
    tsp00_Int2 lockKeyPos_gg00;
    } tgg00_Lock;
typedef
  struct ptoc_Type35
    {
    tgg00_Lkey listkey;
    tgg00_Lkey reckey;
    } tgg00_TwoKeys;
typedef
  tgg00_TwoKeys *tgg00_TwoKeysPtr;
typedef
  struct ptoc_Type36
    {
    tsp00_Int2 sreclen;
    tsp00_Int2 skeylen;
    tsp00_Int2 svarcoloffset;
    tsp00_Int2 svarcolcnt;
    union
      {
      struct
        {
        tsp00_C12 sstandardkey;
        } C_1;
      struct
        {
        tgg00_Surrogate stableid;
        tsp00_C2 sentrytyp;
        tsp00_C2 slinkage;
        tgg00_Surrogate suserid;
        tgg00_Surrogate sgrantuser;
        } C_2;
      struct
        {
        tgg00_Surrogate sauthid;
        tsp00_C2 sentrytyp1;
        tsp00_C2 slinkage1;
        tsp00_Identifier stablen;
        } C_3;
      struct
        {
        tsp00_Int4 stempid;
        tsp00_Int4 stemprest;
        tsp00_C2 sentrytyp2;
        tsp00_Int2 sintlinkage;
        } C_4;
      struct
        {
        tgg00_Surrogate sauthid1;
        tsp00_C2 sentrytyp3;
        tsp00_Int2 slinkage3;
        tsp00_KnlIdentifier sappl;
        tsp00_KnlIdentifier sproc;
        } C_5;
      struct
        {
        tgg00_ServerdbNo ssite;
        tsp00_Int2 sfil;
        tsp00_Int4 stemp;
        } C_6;
      struct
        {
        tgg00_ServerdbNo sserverno;
        } C_7;
      struct
        {
        tgg00_Syskey all;
        } C_8;
      struct
        {
        tgg00_Surrogate sresnidfill;
        tsp00_C2 sresentrytype;
        tsp00_KnlIdentifier srestablen;
        tsp00_KnlIdentifier sresmodul_name;
        } C_9;
      struct
        {
        tgg00_Surrogate szeroid;
        tsp00_C2 sentrytype4;
        tsp00_C2 slinkage4;
        tsp00_Identifier ssetname;
        } C_10;
      struct
        {
        tsp00_C2 sshowid;
        tgg00_ShowKind sshowkind;
        } C_11;
      struct
        {
        tgg00_Surrogate szeroid1;
        tsp00_C2 sentrytype5;
        tsp00_C2 slinkage5;
        tsp00_DbName sdbname;
        } C_12;
      struct
        {
        tgg00_Surrogate sauthid6;
        tsp00_C2 sentrytyp6;
        tsp00_C2 slinkage6;
        tsp00_KnlIdentifier sidentifier;
        } C_13;
      struct
        {
        tgg00_Surrogate sauthid7;
        tsp00_C2 sentrytyp7;
        tsp00_C2 slinkage7;
        tsp00_KnlIdentifier sgwowner;
        tsp00_KnlIdentifier sgwtablen;
        } C_14;
      struct
        {
        tsp00_C2 sclassflag;
        tsp00_C4 sclassid;
        } C_15;
      struct
        {
        tgg00_Surrogate sschema_id;
        tsp00_C2 sentrytype8;
        tsp00_C2 slinkage8;
        tsp00_C16 sguid;
        } C_16;
      } integer;
    } tgg00_SysInfoKey;
typedef
  struct ptoc_Type37
    {
    tsp00_C8 oli_handle;
    tsp00_C12 oli_lockmode;
    tsp00_C12 oli_requestmode;
    tsp00_TaskId oli_taskid;
    tsp00_Int4 oli_timeout;
    } tgg00_OmsLockInfo;
typedef
  struct ptoc_Type38
    {
    union
      {
      struct
        {
        tgg00_SysInfoKey recSysKey_gg00;
        } C_1;
      struct
        {
        tsp00_C8 recSpace1_gg00;
        tsp00_C1 recInsSelInfo_gg00;
        char recIntoTemp_gg00;
        tgg00_Surrogate recTempKey_gg00;
        } C_2;
      struct
        {
        tgg00_SysInfoKey basesyskey;
        } C_11;
      struct
        {
        tgg00_TermsetRecord termsets;
        } C_13;
      struct
        {
        tsp00_Int2 rb_len;
        tsp00_Int2 rb_keylen;
        tsp00_Int2 rb_varcol_offset;
        tsp00_Int2 rb_varcol_cnt;
        tsp00_C1 rb_ins_sel_info;
        char rb_into_temp;
        tgg00_Surrogate rb_tempkey;
        } C_21;
      } integer;
    } tgg00_Info;
typedef
  struct ptoc_Type39
    {
    union
      {
      struct
        {
        tsp00_Int2 hrecLen_gg00;
        tsp00_Int2 hrecKeyLen_gg00;
        tsp00_Int2 hrecVarcolOffset_gg00;
        tsp00_Int2 hrecVarcolCnt_gg00;
        } C_1;
      struct
        {
        tsp00_C2 hrecLenC2_gg00;
        tsp00_C2 hrecKeyLenC2_gg00;
        tsp00_C2 hrecVarcolOffC2_gg00;
        tsp00_C2 hrecVarcolCntC2_gg00;
        } C_2;
      struct
        {
        tsp00_C2 hrecLenSpace_gg00;
        tsp00_C2 hrecKeyLenSpace_gg00;
        tgg90_Cint4 hrecPno_gg00;
        } C_3;
      } integer;
    } tgg00_HeaderRec;
typedef
  struct ptoc_Type40
    {
    union
      {
      struct
        {
        tsp00_Buf recBuf_gg00;
        } C_1;
      struct
        {
        tsp00_Int2 recLen_gg00;
        tsp00_Int2 recKeyLen_gg00;
        tsp00_Int2 recVarcolOffset_gg00;
        tsp00_Int2 recVarcolCnt_gg00;
        tgg00_RecBody recBody_gg00;
        } C_2;
      struct
        {
        tgg00_Lkey recKey_gg00;
        } C_3;
      struct
        {
        unsigned char recPart_gg00[256][32];
        } C_4;
      struct
        {
        tgg00_HeaderRec recHead_gg00;
        } C_5;
      struct
        {
        tsp00_C2 recLenC2_gg00;
        tsp00_C2 recKeyLenC2_gg00;
        tsp00_C2 recVarcolOffC2_gg00;
        tsp00_C2 recVarcolCntC2_gg00;
        } C_6;
      struct
        {
        tsp00_C2 recLenSpace1_gg00;
        tsp00_C2 recKeyLenSpace1_gg00;
        tgg90_Cint4 recPno_gg00;
        } C_7;
      struct
        {
        tsp00_C2 recLenSpace2_gg00;
        tsp00_C2 recKeyLenSpace2_gg00;
        unsigned char recInvListKind_gg00;
        tsp00_Uint1 recAlignment_gg00;
        tsp00_Uint2 recNumPrimKeys_gg00;
        } C_8;
      struct
        {
        tsp00_Buf buf;
        } C_11;
      struct
        {
        tsp00_Int2 len;
        tsp00_Int2 keylen;
        tsp00_Int2 space_var_offset;
        tsp00_Int2 space_var_cnt;
        tgg00_RecBody info;
        } C_12;
      struct
        {
        tgg00_Lkey mkey;
        } C_13;
      struct
        {
        unsigned char recpart[256][32];
        } C_14;
      } integer;
    } tgg00_Rec;
typedef
  struct ptoc_Type41
    {
    union
      {
      struct
        {
        tgg00_Rec rb;
        } C_true;
      struct
        {
        tgg00_Info ri;
        } C_false;
      } boolean;
    } tgg00_BufLenInfo;
typedef
  int tgg00_ColumnDistinctValues[1024];
typedef
  struct ptoc_Type42
    {
    tgg00_FileId TreeStatTreeId_gg00;
    char TreeStatCalculate_gg00;
    char TreeStatFill1_gg00;
    tsp00_Int2 TreeStatFill2_gg00;
    tsp00_Int4 TreeStatNoSampleLeaves_gg00;
    tsp00_Int4 TreeStatNumberOfLeaves_gg00;
    tsp00_Int4 TreeStatNumberOfRecords_gg00;
    } tgg00_TreeStatInfo;
typedef
  struct ptoc_Type43
    {
    tgg00_FileId ColStatTreeId_gg00;
    char ColStatCalculate_gg00;
    char ColStatFill1_gg00;
    tsp00_Int2 ColStatFill2_gg00;
    tsp00_Int4 ColStatNoSampleLeaves_gg00;
    tsp00_Int4 ColStatNumberOfLeaves_gg00;
    tsp00_Int4 ColStatNumberOfRecords_gg00;
    tgg00_ColumnDistinctValues ColStatDistValues_gg00;
    } tgg00_ColStatInfo;
typedef
  struct ptoc_Type44
    {
    tgg00_FileId sam_tree_id;
    tsp00_Int4 sam_rows;
    tsp00_Int4 sam_percent;
    char sam_with_long;
    char sam_fill1;
    tsp00_Int2 sam_fill2;
    tsp00_Int4 leafnodes;
    tsp00_Int4 sec_key_cnt;
    tsp00_Int4 prim_key_cnt;
    tsp00_Int4 avg_prim_per_list;
    tsp00_Int4 records;
    tsp00_Int4 all_stringcol_pages;
    tsp00_Int4 nodes;
    tsp00_Int4 null_value_cnt;
    tsp00_Int4 min_sec_key_len;
    tsp00_Int4 max_sec_key_len;
    tsp00_Int4 avg_sec_key_len;
    tsp00_Int4 treecov;
    tsp00_Int4 rootcov;
    tsp00_Int4 indexcov;
    tsp00_Int4 min_indexcov;
    tsp00_Int4 max_indexcov;
    tsp00_Int4 leafcov;
    tsp00_Int4 min_leafcov;
    tsp00_Int4 max_leafcov;
    tsp00_Int4 rec_per_page;
    tsp00_Int4 ave_rec_length;
    tsp00_Int4 min_rec_length;
    tsp00_Int4 max_rec_length;
    tsp00_Int4 ave_key_length;
    tsp00_Int4 indexnodes;
    tsp00_Int4 ave_sep_length;
    tsp00_Int2 min_key_length;
    tsp00_Int2 max_key_length;
    tsp00_Int2 min_sep_length;
    tsp00_Int2 max_sep_length;
    tsp00_Int2 min_rec_per_page;
    tsp00_Int2 max_rec_per_page;
    tsp00_PageNo rootsegm;
    tgg00_FiletypeSet smplFileType_gg00;
    char pcs_dummy;
    tsp00_Int2 hight;
    tsp00_Int4 defined_stringcols;
    tsp00_Int4 min_len_stringcol;
    tsp00_Int4 max_len_stringcol;
    tsp00_Int4 ave_len_stringcol;
    tsp00_Int4 min_stringcol_pages;
    tsp00_Int4 max_stringcol_pages;
    tsp00_Int4 ave_stringcol_pages;
    tsp00_Int4 invsel_1;
    tsp00_Int4 invsel_5;
    tsp00_Int4 invsel_10;
    tsp00_Int4 invsel_25;
    tsp00_Int4 invsel_notsel;
    tsp00_Int4 min_prim_per_list;
    tsp00_Int4 max_prim_per_list;
    int dist_values[1024];
    } tgg00_SampleInfo;
typedef
  struct ptoc_Type45
    {
    tsp00_Int4 idc_prim_leafnodes;
    tsp00_Int4 idc_sec_leafnodes;
    tsp00_Int4 idc_prim_keycount;
    tsp00_Int4 idc_sec_keycount;
    tsp00_Int4 idc_nullcount;
    } tgg00_IndexCounter;
typedef
  struct ptoc_Type46
    {
    union
      {
      struct
        {
        tsp00_MoveObj mbp_buf;
        } C_1;
      struct
        {
        tsp00_Buf mbp_4kbuf;
        } C_2;
      struct
        {
        tgg00_Rec mbp_rec;
        } C_3;
      struct
        {
        tsp00_Int2 mbp_reclen;
        tsp00_Int2 mbp_keylen;
        tsp00_Int2 mbp_varcol_offset;
        tsp00_Int2 mbp_varcol_cnt;
        } C_4;
      struct
        {
        tgg00_Lkey mbp_key;
        tgg00_Lkey mbp_key2;
        } C_5;
      struct
        {
        tsp00_Int2 mbp_space1;
        tsp00_Int2 mbp_space2;
        tsp00_Int2 mbp_space3;
        tsp00_Int2 mbp_space4;
        tgg00_RecBody mbp_recinfo;
        } C_6;
      struct
        {
        tgg00_SampleInfo mbp_sample;
        } C_7;
      struct
        {
        tgg00_IndexCounter mbp_index_cnt;
        } C_8;
      struct
        {
        tgg00_Info mbp_info;
        } C_9;
      struct
        {
        tgg00_PnoList mbp_pno_list;
        } C_10;
      struct
        {
        tgg00_ColStatInfo mbp_ColStat;
        } C_11;
      struct
        {
        tgg00_TreeStatInfo mbp_TreeStat;
        } C_12;
      } integer;
    } tgg00_DataPart;
typedef
  tgg00_DataPart *tgg00_DataPartPtr;
typedef
  struct ptoc_Type47
    {
    union
      {
      struct
        {
        unsigned char etype;
        unsigned char eop;
        tsp00_Int2 epos;
        tsp00_Int2 elen_var;
        tsp00_C2 ecol_tab;
        } C_1;
      struct
        {
        unsigned char efill2_type;
        unsigned char eop_func;
        tsp00_C2 epos_c2;
        tsp00_C2 var_elen;
        tsp00_Int2 ecol_pos;
        } C_2;
      struct
        {
        unsigned char efill3_type;
        unsigned char eop_out;
        tsp00_Int2 ecolno;
        tsp00_Int4 eval4_var;
        } C_3;
      struct
        {
        unsigned char efill4_type;
        unsigned char eop_build_in;
        unsigned char eformat;
        unsigned char edatatype;
        tsp00_C3 elanguage;
        tsp00_Uint1 elength;
        } C_4;
      struct
        {
        unsigned char efill5_type;
        char eop_new_rec;
        } C_5;
      struct
        {
        unsigned char efill6_type;
        char eop_fill;
        tsp00_Int2 efill7_pos;
        tsp00_Int2 efill8_len;
        tsp00_Int2 eindex;
        } C_6;
      } integer;
    } tgg00_StackEntry;
typedef
  tgg00_StackEntry *tgg_st_entry_addr;
typedef
  tgg00_StackEntry tgg00_StackList[32767];
typedef
  tgg00_StackList *tgg00_StackListPtr;
typedef
  struct ptoc_Type48
    {
    tsp00_Int2 mcol_pos;
    tsp00_Int2 mcol_cnt;
    tsp00_Int2 mmult_pos;
    tsp00_Int2 mmult_cnt;
    tsp00_Int2 mqual_pos;
    tsp00_Int2 mqual_cnt;
    tsp00_Int2 mview_pos;
    tsp00_Int2 mview_cnt;
    tsp00_Int2 mupd_pos;
    tsp00_Int2 mupd_cnt;
    tsp00_Int2 mlink_pos;
    tsp00_Int2 mlink_cnt;
    tsp00_Int2 mstrat_pos;
    tsp00_Int2 mstrat_cnt;
    tsp00_Int2 mstring_pos;
    tsp00_Int2 mstring_cnt;
    tsp00_Int2 mtrigger_pos;
    tsp00_Int2 mtrigger_cnt;
    tsp00_Int2 minvqual_pos;
    tsp00_Int2 minvqual_cnt;
    union
      {
      struct
        {
        tsp00_Int2 mresqual_pos;
        tsp00_Int2 mresqual_cnt;
        tsp00_Int2 mspecial;
        char msubquery;
        char mbool;
        unsigned char msqlmode;
        unsigned char mdatetimeformat;
        tsp00_Int2 malignfill2;
        tsp00_Int4 malignfill3;
        tsp00_Int2 mst_optimize_pos;
        tsp00_Int2 mst_work_depth;
        tsp00_Int2 mfirst_free;
        tsp00_Int2 mst_max;
        tgg00_StackListPtr mst_addr;
        } C_1;
      struct
        {
        tsp00_Int2 mfill1;
        tsp00_Int2 mfill2;
        tsp00_Int2 mfixedcol_len;
        tsp00_C2 mfill3;
        tsp00_C8 mfill4;
        tsp00_C4 mfill5;
        tsp00_C4 mfill6;
        tsp00_C8 maddr_space;
        } C_2;
      struct
        {
        tsp00_Int2 mfill7;
        tsp00_Int2 mfill8;
        char mswap;
        unsigned char mswap_kind;
        } C_3;
      } integer;
    } tgg00_StackDesc;
typedef
  struct ptoc_Type49
    {
    tgg00_Surrogate lq_lock_tabid;
    tsp00_Int4 lq_pos;
    tsp00_Int4 lq_len;
    tsp00_Int4 lq_long_size;
    char lq_long_in_file;
    tsp00_CodeType lq_code;
    union
      {
      struct
        {
        tsp00_C1 lq_expand_char;
        char lq_trunc_req;
        tsp00_Int4 lq_data_offset;
        } C_false;
      struct
        {
        char lq_try_unlock;
        char lq_is_pattern;
        } C_true;
      } boolean;
    } tgg00_LongQual;
typedef
  struct ptoc_Type50
    {
    tgg00_Surrogate lcq_src_lock_tabid;
    tsp00_Int4 lcq_src_pos;
    tsp00_Int4 lcq_len;
    tsp00_Int4 lcq_dest_pos;
    char lcq_src_long_in_file;
    char lcq_dest_long_in_file;
    tgg00_Surrogate lcq_src_tabid;
    tgg00_Surrogate lcq_dest_lock_tabid;
    } tgg00_LongCopyQual;
typedef
  struct ptoc_Type51
    {
    tgg00_FileId m_fill1;
    tgg00_StackDesc m_fill2;
    tsp00_ResNum m_resnum;
    char m_res_build;
    tgg00_FileId m_restree;
    tsp00_ResNum m_keycnt;
    char m_fill3;
    tsp00_Int4 m_searched_pages;
    tgg00_FileId m_fns1;
    tgg00_FileId m_fns2;
    tsp00_Int4 m_rescnt;
    tsp00_Int4 m_rowno;
    tsp00_Int4 m_cnt;
    char m_use_inv;
    char m_use_stop;
    char m_single;
    char m_use_recs;
    tsp00_Int2 m_keylen;
    tsp00_Int2 m_leng;
    tgg00_TwoKeys m_startkeys;
    tgg00_TwoKeys m_stopkeys;
    tgg00_TwoKeys m_firstkeys;
    tgg00_Lkey m_start;
    tsp00_Int2 m_dist_optim;
    tgg00_RefInfoIndex m_strat_info;
    unsigned char m_qual_kind;
    tgg00_DataPartPtr m_data;
    } tgg00_FetchDesc;
typedef
  struct ptoc_Type52
    {
    tgg00_FileId mlp_fill1;
    tgg00_StackDesc mlp_stack_desc;
    tgg00_FileId mlp_linktree;
    tsp00_Int2 mlp_use_fileid;
    tgg00_ServerdbNo mlp_use_site;
    tsp00_Int2 mlp_sa_level;
    tsp00_Int2 mlp_lo_level;
    tgg00_ServerdbNo mlp_sendsite;
    tsp00_KnlIdentifier mlp_owner;
    tsp00_KnlIdentifier mlp_tablen;
    tsp00_KnlIdentifier mlp_ref_name;
    } tgg00_LinkChainInfo;
typedef
  struct ptoc_Type53
    {
    union
      {
      struct
        {
        tsp00_Buf buf;
        } C_1;
      struct
        {
        tgg00_FileId mtree;
        tsp00_Int2 mcol_pos;
        tsp00_Int2 mcol_cnt;
        tsp00_Int2 mmult_pos;
        tsp00_Int2 mmult_cnt;
        tsp00_Int2 mqual_pos;
        tsp00_Int2 mqual_cnt;
        tsp00_Int2 mview_pos;
        tsp00_Int2 mview_cnt;
        tsp00_Int2 mupd_pos;
        tsp00_Int2 mupd_cnt;
        tsp00_Int2 mlink_pos;
        tsp00_Int2 mlink_cnt;
        tsp00_Int2 mstrat_pos;
        tsp00_Int2 mstrat_cnt;
        tsp00_Int2 mstring_pos;
        tsp00_Int2 mstring_cnt;
        tsp00_Int2 mtrigger_pos;
        tsp00_Int2 mtrigger_cnt;
        tsp00_Int2 minvqual_pos;
        tsp00_Int2 minvqual_cnt;
        tsp00_Int2 mresqual_pos;
        tsp00_Int2 mresqual_cnt;
        tsp00_Int2 mspecial;
        char msubquery;
        char mbool;
        unsigned char msqlmode;
        unsigned char mdatetimeformat;
        tsp00_Int2 malignfill2;
        tsp00_Int4 malignfill3;
        tsp00_Int2 mst_optimize_pos;
        tsp00_Int2 mst_work_depth;
        tsp00_Int2 mfirst_free;
        tsp00_Int2 mst_max;
        tgg00_StackListPtr mst_addr;
        } C_2;
      struct
        {
        tgg00_FileId mst_fill;
        tgg00_StackDesc mstack_desc;
        tgg00_FileId mlinktree;
        } C_3;
      struct
        {
        tgg00_FileId mr_fill1;
        tgg00_StackDesc mr_fill2;
        tsp00_ResNum mr_resnum;
        char mr_res_build;
        tgg00_FileId mr_restree;
        tsp00_ResNum mr_keycnt;
        char mr_fill3;
        tsp00_Int4 mr_pagecnt;
        } C_4;
      struct
        {
        tgg00_FileId mtable_tree;
        tsp00_Int2 mold_long_cnt;
        tsp00_Int2 mnew_long_cnt;
        } C_5;
      struct
        {
        tgg00_FileId mut_fill1;
        unsigned char mut_diag_type;
        char mut_config;
        tsp00_Int2 mut_pool_size;
        tsp00_Int2 mut_index_no;
        tsp00_Int4 mut_pno;
        tsp00_Int4 mut_pno2;
        tsp00_Int4 mut_count;
        tgg00_DumpStateSet mut_dump_state;
        tgg00_Surrogate mut_surrogate;
        tsp00_KnlIdentifier mut_authname;
        tsp00_KnlIdentifier mut_tabname;
        tsp00_VFilename mut_dev;
        tsp00_VFilename mut_dev2;
        tsp00_VFilename mut_hostfn;
        } C_6;
      struct
        {
        tsp00_Int2 mlock_cnt;
        tsp00_Int2 mlock_fill1;
        tgg00_Lock mlock[240];
        } C_7;
      struct
        {
        tgg00_SaveRestoreInputParam msave_restore;
        } C_8;
      struct
        {
        tgg00_FetchDesc mf_desc;
        } C_10;
      struct
        {
        tgg00_FileId ml_tree;
        tgg00_StackDesc ml_stack_desc;
        tgg00_LongQual ml_long_qual;
        } C_11;
      struct
        {
        tgg00_FileId mcl_tree;
        tgg00_StackDesc mcl_stack_desc;
        tgg00_LongCopyQual mcl_copy_long;
        } C_12;
      struct
        {
        tgg00_FileId mcfc_tree;
        tsp00_PageNo mcfc_pno;
        tsp00_Int4 mcfc_fill_len;
        } C_13;
      struct
        {
        tgg00_LinkChainInfo mlc_info;
        } C_14;
      } integer;
    } tgg00_QualBuf;
typedef
  tgg00_QualBuf *tgg00_QualBufPtr;
typedef
  struct ptoc_Type54
    {
    tsp00_Int4 mb_qual_len;
    tsp00_Int4 mb_data_len;
    tsp00_Int4 mb_strat_len;
    tsp00_Int4 mb_fill2;
    tgg00_ServerdbNo mb_src_site;
    char mb_reply;
    unsigned char mb_replicated;
    tgg00_MessType mb_type;
    tgg00_MessType2 mb_type2;
    unsigned char mb_struct;
    char mb_fill1;
    } tgg00_MessBufHeader;
typedef
  struct tgg00_MessBlock *tgg00_MessBlockPtr;
typedef
  struct ptoc_Type55
    {
    union
      {
      struct
        {
        tsp00_C8 mb_ptr_space1;
        tsp00_C8 mb_ptr_space2;
        tsp00_C8 mb_ptr_space3;
        tsp00_C8 mb_ptr_space4;
        tsp00_C8 mb_ptr_space5;
        tsp00_C8 mb_ptr_space6;
        tsp00_C8 mb_ptr_space7;
        tsp00_C8 mb_ptr_space8;
        tsp00_Int4 mb_data_size;
        tsp00_Int4 mb_qual_size;
        tsp00_Int4 mb_strat_size;
        tsp00_Int4 mb_st_size;
        tsp00_Int2 mb_st_max;
        tsp00_Int2 mb_work_st_max;
        tsp00_Int4 mb_workbuf_size;
        } C_2;
      struct
        {
        tgg00_DataPartPtr mb_data;
        } C_3;
      struct
        {
        tsp00_C8 mb_pt_space4_1;
        tgg00_QualBufPtr mb_qual;
        } C_4;
      struct
        {
        tsp00_C8 mb_pt_space5_1;
        tsp00_C8 mb_ptr_space5_2;
        tgg00_StackListPtr mb_st;
        } C_5;
      struct
        {
        tsp00_C8 mb_pt_space6_1;
        tsp00_C8 mb_ptr_space6_2;
        tsp00_C8 mb_ptr_space6_3;
        tgg00_StackListPtr mb_work_st;
        } C_6;
      struct
        {
        tsp00_C8 mb_pt_space7_1;
        tsp00_C8 mb_ptr_space7_2;
        tsp00_C8 mb_ptr_space7_3;
        tsp00_C8 mb_ptr_space7_4;
        tsp00_MoveObjPtr mb_workbuf;
        } C_7;
      struct
        {
        tsp00_C8 mb_pt_space8_1;
        tsp00_C8 mb_ptr_space8_2;
        tsp00_C8 mb_ptr_space8_3;
        tsp00_C8 mb_ptr_space8_4;
        tsp00_C8 mb_ptr_space8_5;
        tgg00_MessBlockPtr mb_next_mblock;
        } C_8;
      struct
        {
        tsp00_C8 mb_pt_space9_1;
        tsp00_C8 mb_ptr_space9_2;
        tsp00_C8 mb_ptr_space9_3;
        tsp00_C8 mb_ptr_space9_4;
        tsp00_C8 mb_ptr_space9_5;
        tsp00_C8 mb_ptr_space9_6;
        tgg00_TransContextPtr mb_trns;
        } C_9;
      struct
        {
        tsp00_C8 mb_pt_space10_1;
        tsp00_C8 mb_ptr_space10_2;
        tsp00_C8 mb_ptr_space10_3;
        tsp00_C8 mb_ptr_space10_4;
        tsp00_C8 mb_ptr_space10_5;
        tsp00_C8 mb_ptr_space10_6;
        tgg00_TransChildPtr mb_trans_child;
        } C_10;
      struct
        {
        tsp00_C8 mb_pt_space11_1;
        tsp00_C8 mb_ptr_space11_2;
        tsp00_C8 mb_ptr_space11_3;
        tsp00_C8 mb_ptr_space11_4;
        tsp00_C8 mb_ptr_space11_5;
        tsp00_C8 mb_ptr_space11_6;
        tsp00_C8 mb_ptr_space11_7;
        tsp00_MoveObjPtr mb_strat;
        } C_11;
      } integer;
    } tgg00_MessBufTrailer;
struct tgg00_MessBlock
  {
  union
    {
    struct
      {
      tgg00_MessBufHeader mb_header;
      tgg00_MessBufTrailer mb_trailer;
      } C_1;
    struct
      {
      tsp00_Int4 mb_qual_len;
      tsp00_Int4 mb_data_len;
      tsp00_Int4 mb_strat_len;
      tsp00_Int4 mb_fill2;
      tgg00_ServerdbNo mb_src_site;
      char mb_reply;
      unsigned char mb_replicated;
      tgg00_MessType mb_type;
      tgg00_MessType2 mb_type2;
      unsigned char mb_struct;
      char mb_fill1;
      tsp00_C8 mb_ptr_space1;
      tsp00_C8 mb_ptr_space2;
      tsp00_C8 mb_ptr_space3;
      tsp00_C8 mb_ptr_space4;
      tsp00_C8 mb_ptr_space5;
      tsp00_C8 mb_ptr_space6;
      tsp00_C8 mb_ptr_space7;
      tsp00_C8 mb_ptr_space8;
      tsp00_Int4 mb_data_size;
      tsp00_Int4 mb_qual_size;
      tsp00_Int4 mb_strat_size;
      tsp00_Int4 mb_st_size;
      tsp00_Int2 mb_st_max;
      tsp00_Int2 mb_work_st_max;
      tsp00_Int4 mb_workbuf_size;
      } C_2;
    struct
      {
      tgg00_MessBufHeader mb_space3;
      tgg00_DataPartPtr mb_data;
      } C_3;
    struct
      {
      tgg00_MessBufHeader mb_space4;
      tsp00_C8 mb_ptr_space4_1;
      tgg00_QualBufPtr mb_qual;
      } C_4;
    struct
      {
      tgg00_MessBufHeader mb_space5;
      tsp00_C8 mb_ptr_space5_1;
      tsp00_C8 mb_ptr_space5_2;
      tgg00_StackListPtr mb_st;
      } C_5;
    struct
      {
      tgg00_MessBufHeader mb_space6;
      tsp00_C8 mb_ptr_space6_1;
      tsp00_C8 mb_ptr_space6_2;
      tsp00_C8 mb_ptr_space6_3;
      tgg00_StackListPtr mb_work_st;
      } C_6;
    struct
      {
      tgg00_MessBufHeader mb_space7;
      tsp00_C8 mb_ptr_space7_1;
      tsp00_C8 mb_ptr_space7_2;
      tsp00_C8 mb_ptr_space7_3;
      tsp00_C8 mb_ptr_space7_4;
      tsp00_MoveObjPtr mb_workbuf;
      } C_7;
    struct
      {
      tgg00_MessBufHeader mb_space8;
      tsp00_C8 mb_ptr_space8_1;
      tsp00_C8 mb_ptr_space8_2;
      tsp00_C8 mb_ptr_space8_3;
      tsp00_C8 mb_ptr_space8_4;
      tsp00_C8 mb_ptr_space8_5;
      tgg00_MessBlockPtr mb_next_mblock;
      } C_8;
    struct
      {
      tgg00_MessBufHeader mb_space9;
      tsp00_C8 mb_ptr_space9_1;
      tsp00_C8 mb_ptr_space9_2;
      tsp00_C8 mb_ptr_space9_3;
      tsp00_C8 mb_ptr_space9_4;
      tsp00_C8 mb_ptr_space9_5;
      tsp00_C8 mb_ptr_space9_6;
      tgg00_TransContextPtr mb_trns;
      } C_9;
    struct
      {
      tgg00_MessBufHeader mb_space10;
      tsp00_C8 mb_ptr_space10_1;
      tsp00_C8 mb_ptr_space10_2;
      tsp00_C8 mb_ptr_space10_3;
      tsp00_C8 mb_ptr_space10_4;
      tsp00_C8 mb_ptr_space10_5;
      tsp00_C8 mb_ptr_space10_6;
      tgg00_TransChildPtr mb_trans_child;
      } C_10;
    struct
      {
      tgg00_MessBufHeader mb_space11;
      tsp00_C8 mb_ptr_space11_1;
      tsp00_C8 mb_ptr_space11_2;
      tsp00_C8 mb_ptr_space11_3;
      tsp00_C8 mb_ptr_space11_4;
      tsp00_C8 mb_ptr_space11_5;
      tsp00_C8 mb_ptr_space11_6;
      tsp00_C8 mb_ptr_space11_7;
      tsp00_MoveObjPtr mb_strat;
      } C_11;
    } integer;
  } ;
typedef
  struct ptoc_Type56
    {
    tsp00_Int2 vpl_last;
    tsp00_Int2 vpl_last_fill1;
    tsp00_Int4 vpl_last_fill2;
    short vpl_pos_list[1024];
    } tgg00_VarColPosList;
typedef
  struct ptoc_Type57
    {
    union
      {
      struct
        {
        tsp00_C48 sfp_addr_fill1;
        tsp00_C24 sfp_addr_fill2;
        tsp00_Int4 sfp_data_size;
        tsp00_Int4 sfp_work_st_size;
        tsp00_Int4 sfp_workbuf_size;
        tsp00_Int4 sfp_workbuf_len;
        tsp00_Int4 sfp_act_cntresult;
        tsp00_Int4 sfp_oj_spec;
        tsp00_Int4 sfp_rows_qual;
        tsp00_Int4 sfp_rows_read;
        tsp00_Int4 sfp_m_result_size;
        tsp00_Int4 sfp_m_result_len;
        tsp00_Int4 sfp_m_result_cnt;
        tsp00_Int2 sfp_rec_len;
        tsp00_Int2 sfp_rec_key_len;
        tsp00_Int2 sfp_oldkey_len;
        tsp00_Int2 sfp_oldrec_len;
        tsp00_Int2 sfp_oldrec_pos;
        tsp00_Int2 sfp_result_length;
        tsp00_Int2 sfp_output_offset;
        tsp00_Int2 sfp_work_st_max;
        tsp00_Int2 sfp_resrec_maxlen;
        tgg00_MessType sfp_bd_mess_type;
        tgg00_MessType2 sfp_bd_mess2_type;
        unsigned char sfp_replicated;
        char sfp_check_for_result;
        unsigned char sfp_bd_inv_only;
        char sfp_bd_return_knf;
        char sfp_bd_use_stopkey;
        char sfp_first_qual;
        char sfp_is_first_record;
        char sfp_result_wanted;
        tgg00_VarColPosList sfp_varcol_pos;
        tgg_st_entry_addr sfp_work_st_top;
        tgg_st_entry_addr sfp_work_st_bottom;
        tgg_st_entry_addr sfp_work_st_last;
        tsp00_Int4 sfp_workbuf_top;
        char sfp_check_new_rec;
        char sfp_optimized;
        unsigned char sfp_sqlmode;
        unsigned char sfp_dateTimeFormat;
        tsp00_Addr sfp_acv_addr;
        } C_1;
      struct
        {
        tsp00_MoveObjPtr sfp_data_addr;
        tsp00_BufAddr sfp_rec_addr;
        tsp00_KeyAddr sfp_primkey_addr;
        tgg00_StackListPtr sfp_work_st_addr;
        tsp00_MoveObjPtr sfp_workbuf_addr;
        tsp00_MoveObjPtr sfp_oldrec_addr;
        tsp00_BufAddr sfp_pl_ptr;
        tsp00_MoveObjPtr sfp_m_result_addr;
        tgg00_TwoKeysPtr sfp_m_firstkeys_addr;
        } C_2;
      struct
        {
        tgg00_SfpInitPart sfp_init_part;
        } C_3;
      } integer;
    } tgg00_SelectFieldsParam;
typedef
  struct ptoc_Type58
    {
    tsp00_Int4 db_total_pages;
    tsp00_Int4 db_used_pages;
    tsp00_Int4 db_perm_pages;
    tsp00_Int4 db_temp_pages;
    tsp00_Int2 db_perm_percent;
    tsp00_Int2 db_temp_percent;
    tsp00_Int4 db_real_perm_used;
    tsp00_Int4 db_updated_pages;
    tsp00_Int4 db_max_perm_used;
    tsp00_Int4 db_used_blocks;
    } tgg00_DbSpaceInfo;
typedef
  struct ptoc_Type59
    {
    union
      {
      struct
        {
        tsp00_8ByteCounter data_cache_total;
        tsp00_8ByteCounter data_cache_hit;
        tsp00_8ByteCounter data_cache_miss;
        tsp00_8ByteCounter data_cache_percent;
        tsp00_8ByteCounter ak_cache_total;
        tsp00_8ByteCounter ak_cache_hit;
        tsp00_8ByteCounter ak_cache_miss;
        tsp00_8ByteCounter ak_cache_percent;
        tsp00_8ByteCounter tseq_cache_total;
        tsp00_8ByteCounter tseq_cache_hit;
        tsp00_8ByteCounter tseq_cache_miss;
        tsp00_8ByteCounter tseq_cache_percent;
        tsp00_8ByteCounter data_sql_cache_total;
        tsp00_8ByteCounter data_sql_cache_hit;
        tsp00_8ByteCounter data_sql_cache_miss;
        tsp00_8ByteCounter data_sql_cache_percent;
        tsp00_8ByteCounter data_obj_cache_total;
        tsp00_8ByteCounter data_obj_cache_hit;
        tsp00_8ByteCounter data_obj_cache_miss;
        tsp00_8ByteCounter data_obj_cache_percent;
        tsp00_8ByteCounter data_obj_log_cache_total;
        tsp00_8ByteCounter data_obj_log_cache_hit;
        tsp00_8ByteCounter data_obj_log_cache_miss;
        tsp00_8ByteCounter data_obj_log_cache_percent;
        tsp00_8ByteCounter data_history_cache_total;
        tsp00_8ByteCounter data_history_cache_hit;
        tsp00_8ByteCounter data_history_cache_miss;
        tsp00_8ByteCounter data_history_cache_percent;
        tsp00_8ByteCounter sql_commands;
        tsp00_8ByteCounter prepares;
        tsp00_8ByteCounter prepares_again;
        tsp00_8ByteCounter executes;
        tsp00_8ByteCounter commits;
        tsp00_8ByteCounter rollbacks;
        tsp00_8ByteCounter locks;
        tsp00_8ByteCounter begin_subtrans;
        tsp00_8ByteCounter end_subtrans;
        tsp00_8ByteCounter rollback_subtrans;
        tsp00_8ByteCounter creates;
        tsp00_8ByteCounter alters;
        tsp00_8ByteCounter drops;
        tsp00_8ByteCounter selects;
        tsp00_8ByteCounter selects_rows_read;
        tsp00_8ByteCounter selects_rows_qual;
        tsp00_8ByteCounter inserts;
        tsp00_8ByteCounter inserts_rows;
        tsp00_8ByteCounter updates;
        tsp00_8ByteCounter updates_rows_read;
        tsp00_8ByteCounter updates_rows_upd;
        tsp00_8ByteCounter deletes;
        tsp00_8ByteCounter deletes_rows_read;
        tsp00_8ByteCounter deletes_rows_del;
        tsp00_8ByteCounter internal_dbproc_calls;
        tsp00_8ByteCounter external_dbproc_calls;
        tsp00_8ByteCounter internal_trigger_calls;
        tsp00_8ByteCounter external_trigger_calls;
        tsp00_8ByteCounter prim_key;
        tsp00_8ByteCounter prim_key_read;
        tsp00_8ByteCounter prim_key_qual;
        tsp00_8ByteCounter prim_key_range;
        tsp00_8ByteCounter prim_key_range_read;
        tsp00_8ByteCounter prim_key_range_qual;
        tsp00_8ByteCounter index;
        tsp00_8ByteCounter index_read;
        tsp00_8ByteCounter index_qual;
        tsp00_8ByteCounter index_range;
        tsp00_8ByteCounter index_range_read;
        tsp00_8ByteCounter index_range_qual;
        tsp00_8ByteCounter is_index;
        tsp00_8ByteCounter is_index_read;
        tsp00_8ByteCounter is_index_qual;
        tsp00_8ByteCounter is_index_range;
        tsp00_8ByteCounter is_index_range_read;
        tsp00_8ByteCounter is_index_range_qual;
        tsp00_8ByteCounter table_scan;
        tsp00_8ByteCounter table_scan_read;
        tsp00_8ByteCounter table_scan_qual;
        tsp00_8ByteCounter is_index_scan;
        tsp00_8ByteCounter is_index_scan_read;
        tsp00_8ByteCounter is_index_scan_qual;
        tsp00_8ByteCounter mem_sorts;
        tsp00_8ByteCounter mem_sorts_rows_read;
        tsp00_8ByteCounter ins_sorts;
        tsp00_8ByteCounter ins_sorts_rows_ins;
        tsp00_8ByteCounter avg_lock_used;
        tsp00_8ByteCounter max_lock_used;
        tsp00_8ByteCounter collisions;
        tsp00_8ByteCounter escalations;
        tsp00_8ByteCounter new_rows;
        tsp00_8ByteCounter new_tabs;
        tsp00_8ByteCounter deadlocks;
        tsp00_8ByteCounter request_timeouts;
        tsp00_8ByteCounter read_log;
        tsp00_8ByteCounter write_log;
        double kb1_monitor[8];
        tsp00_8ByteCounter read_string;
        tsp00_8ByteCounter write_string;
        tsp00_8ByteCounter r_v_acc;
        tsp00_8ByteCounter w_v_acc;
        tsp00_8ByteCounter r_p_acc;
        tsp00_8ByteCounter w_p_acc;
        tsp00_8ByteCounter catalog_r_v;
        tsp00_8ByteCounter catalog_w_v;
        tsp00_8ByteCounter catalog_r_p;
        tsp00_8ByteCounter catalog_w_p;
        tsp00_8ByteCounter perm_r_v;
        tsp00_8ByteCounter perm_w_v;
        tsp00_8ByteCounter perm_r_p;
        tsp00_8ByteCounter perm_w_p;
        tsp00_8ByteCounter temp_r_v;
        tsp00_8ByteCounter temp_w_v;
        tsp00_8ByteCounter temp_r_p;
        tsp00_8ByteCounter temp_w_p;
        tsp00_8ByteCounter bstr_r_v;
        tsp00_8ByteCounter bstr_w_v;
        tsp00_8ByteCounter bstr_r_p;
        tsp00_8ByteCounter bstr_w_p;
        tsp00_8ByteCounter level0_r_v;
        tsp00_8ByteCounter level0_w_v;
        tsp00_8ByteCounter level0_r_p;
        tsp00_8ByteCounter level0_w_p;
        tsp00_8ByteCounter level1_r_v;
        tsp00_8ByteCounter level1_w_v;
        tsp00_8ByteCounter level1_r_p;
        tsp00_8ByteCounter level1_w_p;
        tsp00_8ByteCounter level2_r_v;
        tsp00_8ByteCounter level2_w_v;
        tsp00_8ByteCounter level2_r_p;
        tsp00_8ByteCounter level2_w_p;
        tsp00_8ByteCounter level3_r_v;
        tsp00_8ByteCounter level3_w_v;
        tsp00_8ByteCounter level3_r_p;
        tsp00_8ByteCounter level3_w_p;
        tsp00_8ByteCounter level4_r_v;
        tsp00_8ByteCounter level4_w_v;
        tsp00_8ByteCounter level4_r_p;
        tsp00_8ByteCounter level4_w_p;
        tsp00_8ByteCounter level5_r_v;
        tsp00_8ByteCounter level5_w_v;
        tsp00_8ByteCounter level5_r_p;
        tsp00_8ByteCounter level5_w_p;
        tsp00_8ByteCounter psm_r_v;
        tsp00_8ByteCounter psm_w_v;
        tsp00_8ByteCounter psm_r_p;
        tsp00_8ByteCounter psm_w_p;
        tsp00_8ByteCounter add_rec;
        tsp00_8ByteCounter cadd_rec;
        tsp00_8ByteCounter repl_rec;
        tsp00_8ByteCounter crepl_rec;
        tsp00_8ByteCounter del_rec;
        tsp00_8ByteCounter cdel_rec;
        tsp00_8ByteCounter get_rec;
        tsp00_8ByteCounter cget_rec;
        tsp00_8ByteCounter next_rec;
        tsp00_8ByteCounter cnext_rec;
        tsp00_8ByteCounter prev_rec;
        tsp00_8ByteCounter cprev_rec;
        tsp00_8ByteCounter sel_direc;
        tsp00_8ByteCounter sel_next;
        tsp00_8ByteCounter sel_prev;
        tsp00_8ByteCounter add_inv;
        tsp00_8ByteCounter cadd_inv;
        tsp00_8ByteCounter del_inv;
        tsp00_8ByteCounter cdel_inv;
        tsp00_8ByteCounter get_inv;
        tsp00_8ByteCounter cget_inv;
        tsp00_8ByteCounter funccount;
        tsp00_8ByteCounter transcount;
        tsp00_8ByteCounter kb_calls;
        } C_true;
      struct
        {
        double reference_array[177];
        } C_false;
      } boolean;
    } tgg00_ReferenceInfo;
typedef
  unsigned char tgg00_COMMonEnum;
typedef
  struct ptoc_Type60
    {
    tsp00_KnlIdentifier cmiMethodName_gg00;
    tsp00_8ByteCounter cmiCallCnt_gg00;
    double cmiCounters_gg00[63];
    } tgg00_COMMonitorInfo;
typedef
  struct ptoc_Type61
    {
    union
      {
      struct
        {
        tsp00_PageNo logLpno_gg00;
        tgg00_PageType logPageType_gg00;
        unsigned char logPageType2_gg00;
        tsp00_C2 logFiller_gg00;
        tgg00_PagePos logPpos_gg00;
        } C_1;
      struct
        {
        tgg00_PageHeader logHeader_gg00;
        } C_2;
      } integer;
    } tgg00_LogPos;
typedef
  struct ptoc_Type62
    {
    tsp00_Int4 bd_key_check_len;
    tsp00_Int4 bd_max_rec_cnt;
    tsp00_Int4 bd_rec_cnt;
    tsp00_Int4 bd_max_fill_len;
    tsp00_Int4 bd_fill_len;
    char bd_next;
    char bd_drop_page;
    tsp00_Int2 bd_filler1;
    } tgg00_BdSetResultRecord;
typedef
  tgg00_BasisError tgg_basis_error;
typedef
  tgg00_BdSetResultRecord tgg_bd_set_result_record;
typedef
  tgg00_CacheType tgg_cache_type;
typedef
  struct ptoc_Type63
    {
    tsp00_CodeType ctype;
    tsp00_CodeType char_default;
    unsigned char date_time;
    unsigned char kernel_swap;
    } tgg_code_globals;
typedef
  tgg00_DataPartPtr tgg_datapart_ptr;
typedef
  struct ptoc_Type64
    {
    tsp00_Int4 timezone;
    tsp00_Int4 vf_bufpool;
    tgg00_Surrogate sysuser_id;
    tsp00_ResNum rescnt_unknown;
    char datacachecheck;
    tsp00_ResNum rescnt_1;
    char sysmsg_may_exist;
    tsp00_ResNum rescnt_0;
    char db_is_for_sapr3;
    tsp00_KnlIdentifier sysuser_name;
    char treecheck;
    char bd_lock_check;
    char bd_subtree;
    unsigned char kglInstanceType_gg00;
    char filler3;
    tsp00_C1 undef_buf;
    tsp00_Int2 filler4;
    tsp00_MoveObjPtr undef_addr;
    } tgg_kernel_globals;
typedef
  unsigned char tgg_linkset[1];
typedef
  tgg00_Lkey tgg_lkey;
typedef
  struct tgg00_MessBlock tgg_mess_block;
typedef
  tgg00_MessType2 tgg_message2_type;
typedef
  tgg00_MessType tgg_message_type;
typedef
  tgg00_PrivilegeSet tgg_privilege_set;
typedef
  tgg00_QualBuf tgg_qual_buf;
typedef
  tgg00_QualBufPtr tgg_qual_buf_ptr;
typedef
  tgg00_SelectFieldsParam *tgg_select_param_ptr;
typedef
  struct ptoc_Type65
    {
    tsp00_C20 seq_increment;
    tsp00_C20 seq_minvalue;
    tsp00_C20 seq_maxvalue;
    tsp00_C20 seq_cachevalue;
    tgg00_ServerdbNo seq_site;
    char seq_cycle;
    char seq_first;
    tsp00_Int4 seq_filler;
    } tgg_seq_info;
typedef
  tgg00_ServerdbNo tgg_serverdb_no;
typedef
  tgg00_StackEntry tgg_stack_entry;
typedef
  tgg00_StackListPtr tgg_stack_list_ptr;
typedef
  struct ptoc_Type66
    {
    tgg00_FileId str_treeid;
    unsigned char use_mode;
    tsp00_CodeType codespec;
    tgg00_ServerdbNo dhomesite;
    } tgg_string_filedescr;
typedef
  tgg00_Surrogate tgg_surrogate;
typedef
  tgg00_SysInfoKey tgg_sysinfokey;
typedef
  struct ptoc_Type67
    {
    tgg00_FileId sys1_cat;
    tgg00_FileId sys2_cat;
    tgg00_FileId sys_diag_parse;
    tgg00_FileId sys_msg;
    tgg00_FileId obj_hash;
    tgg00_FileId sys_cmd_analyze;
    tgg00_FileId sys_data_analyze;
    } tgg_tabid_globals;
typedef
  struct ptoc_Type68
    {
    tgg00_FileId file_id;
    tgg00_FileId inv_id;
    } tgg_two_tree_ids;
typedef
  struct ptoc_Type69
    {
    char vtrAk_gg00;
    char vtrAkDelete_gg00;
    char vtrAkInsert_gg00;
    char vtrAkPacket_gg00;
    char vtrAkSelect_gg00;
    char vtrAkShortPacket_gg00;
    char vtrAkUpdate_gg00;
    char vtrAll_gg00;
    char vtrAny_gg00;
    char vtrBdIndex_gg00;
    char vtrBdPrim_gg00;
    char vtrBdString_gg00;
    char vtrIoTrace_gg00;
    char vtrKbLock_gg00;
    char vtrStrategy_gg00;
    char vtrTime_gg00;
    char vtrGateway_gg00;
    char vtrBdObject_gg00;
    char vtrOmsNew_gg00;
    char vtrOmsGet_gg00;
    char vtrOmsUpd_gg00;
    char vtrOmsFree_gg00;
    tsp00_Int2 vtrFiller1_gg00;
    char vtrCheck_gg00;
    char vtrRetcodeCheck_gg00;
    tsp00_Int2 vtrStopRetcode_gg00;
    tgg91_SessionNo vtrSession_gg00;
    } tgg_vtrace_state;
typedef
  unsigned char tak00_colpropset[2];
typedef
  struct ptoc_Type70
    {
    union
      {
      struct
        {
        tgg_stack_entry ccolstack;
        tak00_colpropset ccolpropset;
        tsp_int2 cextcolno;
        tsp_int2 creccolno;
        tsp00_Uint1 ctabno;
        unsigned char cdatatyp;
        tsp_int2 cdatalen;
        tsp_int2 cinoutlen;
        tsp00_Uint1 cdatafrac;
        unsigned char cshiftoutchar;
        tsp_int2 cnextind;
        tsp_c2 cudtdatatype;
        unsigned char cfiller02;
        unsigned char ccolumnn_len;
        tsp_knl_identifier ccolumnn;
        } C_1;
      struct
        {
        tgg_stack_entry cfiller1;
        tak00_colpropset cfiller2;
        tsp_int2 cfiller3;
        tsp_int2 cfiller4;
        tsp00_Uint1 cfiller5;
        unsigned char cfiller6;
        tsp_int2 cfiller7;
        tsp_int2 cfiller8;
        tsp00_Uint1 cfiller9;
        char cbinary;
        } C_2;
      struct
        {
        tgg_stack_entry cfiller_1;
        tak00_colpropset cfiller_2;
        tsp_int2 cfiller_3;
        tsp_int2 cfiller_4;
        tsp00_Uint1 cfiller_5;
        unsigned char cfiller_6;
        tsp_int2 cfiller_7;
        tsp_int2 cfiller_8;
        tsp00_Uint1 cfiller_9;
        char cvarchar;
        } C_3;
      } integer;
    } tak00_columninfo;
typedef
  tak00_columninfo *tak00_colinfo_ptr;
typedef
  unsigned char tak_table_attribute_set[1];
typedef
  struct ptoc_Type71
    {
    tsp_int2 sm_error_cnt;
    char sm_must_local;
    char sm_return_req;
    char sm_not_local;
    char sm_filler1;
    tgg_basis_error sm_expected_error;
    short sm_errorlist[16];
    } tak_sm_description;
typedef
  struct ptoc_Type72
    {
    tsp_c3 p_count;
    tsp_c1 p_id;
    tgg_message_type p_kind;
    tsp00_Uint1 p_no;
    tsp_int2 p_filler;
    } tak_parskey;
typedef
  struct ptoc_Type73
    {
    tgg91_SessionNo pid_session;
    union
      {
      struct
        {
        tak_parskey pid_parsk;
        } C_true;
      struct
        {
        tsp_c4 pid_fill1;
        tsp_c2 pid_fill2;
        tsp_c1 pid_appl_info;
        tsp_c1 pid_dtime_info;
        } C_false;
      } boolean;
    } tak_parsid;
typedef
  unsigned char tak_special_node_info[1];
typedef
  struct ptoc_Type74
    {
    unsigned char n_proc;
    tak_special_node_info n_special;
    unsigned char n_symb;
    unsigned char n_datatype;
    tsp_int2 n_subproc;
    tsp_int2 n_length;
    tsp_int4 n_pos;
    tsp_int2 n_lo_level;
    tsp_int2 n_sa_level;
    } tak_ap_max_tree[32768];
typedef
  tak_ap_max_tree *tak_ap_max_tree_ptr;
typedef
  struct ptoc_Type75
    {
    unsigned char sc_symb;
    unsigned char sc_eof_symb;
    tsp_int2 sc_sylength;
    tsp_int4 sc_sypos;
    tsp_int4 sc_newpos;
    tsp_int4 sc_lastpos;
    tsp_int4 sc_missing_kw_pos;
    tsp_int2 sc_double_quote;
    char sc_reserved_check;
    char sc_pack;
    tsp_int2 sc_eof_kw;
    } tak_scanner_glob;
typedef
  unsigned char tak_charset[32];
typedef
  unsigned char tak_columnset[130];
typedef
  short tak_colindex[1024];
typedef
  short tak_colsequence[16];
typedef
  tsp00_Uint1 tak_corlevel[4];
typedef
  unsigned char tak_keyword[18];
typedef
  tsp_c18 tak_oldidentifier;
typedef
  unsigned char tak_order_errortext[256];
typedef
  unsigned char tak_parsbuffer[8076];
typedef
  tgg00_StackEntry tak_16stackentry[16];
typedef
  unsigned char tak_oms_version_id[22];
typedef
  unsigned char tak_sysbufferoffset[20];
typedef
  tsp00_C3 tak_pcountarr[100];
typedef
  struct ptoc_Type76
    {
    tsp_int4 rctxt_cnt;
    tsp00_C8 rctxt_roles[32768];
    } tak_role_context;
typedef
  tak_role_context *tak_role_context_ptr;
typedef
  unsigned char tak_map_set[768];
typedef
  struct ptoc_Type77
    {
    tsp_int2 map_reclen;
    tsp_int2 map_keylen;
    tsp_int2 map_varcol_offset;
    tsp_int2 map_varcol_cnt;
    tgg_surrogate map_surrogate;
    tsp_c2 map_entrytype;
    tsp_c2 map_linkage;
    tsp_knl_identifier map_name;
    tsp_c2 map_segmentid;
    tsp_int2 map_count;
    tsp_code_type map_code;
    char map_filler1;
    tsp_int2 map_filler2;
    tak_map_set map_set;
    } tak_mapset_record;
typedef
  unsigned char tak_priv_desc[1];
typedef
  struct ptoc_Type78
    {
    union
      {
      struct
        {
        tgg_privilege_set priv_all_set;
        tgg_privilege_set priv_all_grant_set;
        tak_priv_desc priv_col_exist;
        char priv_filler;
        tak_columnset priv_sel_set;
        tak_columnset priv_upd_set;
        tak_columnset priv_grant_sel_set;
        tak_columnset priv_grant_upd_set;
        tak_columnset priv_ref_set;
        tak_columnset priv_grant_ref_set;
        } C_1;
      struct
        {
        tsp_c4 priv_filler1;
        tak_columnset priv_col[6];
        } C_2;
      struct
        {
        tsp_c132 priv_c132;
        } C_3;
      struct
        {
        unsigned char priv_c196[196];
        } C_4;
      } integer;
    } tak_privilege;
typedef
  struct ptoc_Type79
    {
    tsp_int2 pr_rec_len;
    tsp_int2 pr_key_len;
    tsp_int2 pr_varcol_offset;
    tsp_int2 pr_varcol_cnt;
    tgg_surrogate pr_tabid;
    tsp_c2 pr_entrytype;
    tsp_c2 pr_linkage;
    tgg_surrogate pr_userid;
    tsp_c2 pr_segmentid;
    tsp_c2 pr_filler;
    tak_privilege pr_priv;
    } tak_privrecord;
typedef
  struct ptoc_Type80
    {
    tsp_int2 pru_rec_len;
    tsp_int2 pru_key_len;
    tsp_int2 pru_varcol_offset;
    tsp_int2 pru_varcol_cnt;
    tgg_surrogate pru_tabid;
    tsp_c2 pru_entrytype;
    tsp_c2 pru_linkage;
    tgg_surrogate pru_grantee;
    tgg_surrogate pru_grantor;
    tsp_c2 pru_segmentid;
    tsp_c2 pru_filler;
    tsp_int4 pru_date;
    tsp_int4 pru_time;
    tak_privilege pru_priv;
    } tak_privuserrecord;
typedef
  struct ptoc_Type81
    {
    tsp_int2 psp_reclen;
    tsp_int2 psp_keylen;
    tsp_int2 psp_varcol_offset;
    tsp_int2 psp_varcol_cnt;
    tgg_surrogate psp_id;
    tsp_c2 psp_type;
    tsp_c2 psp_linkage;
    tgg_surrogate psp_grantor;
    tsp_c2 psp_seqmentid;
    tgg_privilege_set psp_priv;
    char psp_filler;
    tsp_int4 psp_date;
    tsp_int4 psp_time;
    } tak_proc_seq_privilege;
typedef
  struct ptoc_Type82
    {
    tsp_int2 dreclen;
    tsp_int2 dkeylen;
    tsp_int2 dvarcol_offset;
    tsp_int2 dvarcol_cnt;
    tgg_surrogate downer;
    tsp_int2 dentrytype;
    tsp_int2 dseqno;
    tsp_knl_identifier dname;
    tsp_c2 dsegmentid;
    tsp_c2 dfiller;
    tgg_surrogate dsurrogate;
    } tak_domainref;
typedef
  struct ptoc_Type83
    {
    tsp_int2 dom_reclen;
    tsp_int2 dom_keylen;
    tsp_int2 dom_varcol_offset;
    tsp_int2 dom_varcol_cnt;
    tgg_surrogate dom_surrogate;
    tsp_c2 dom_entrytype;
    tsp_int2 dom_seqno;
    tsp_c2 dom_segmentid;
    tsp00_Uint1 dom_filler;
    char dom_constraint;
    tsp_c16 dom_type_uid;
    tgg_surrogate dom_owner;
    tsp_int4 dom_date;
    tsp_int4 dom_time;
    tak00_columninfo dom_colinfo;
    } tak_domainrecord;
typedef
  struct ptoc_Type84
    {
    tsp_int2 cud_reclen;
    tsp_int2 cud_keylen;
    tsp_int2 cud_varcol_offset;
    tsp_int2 cud_varcol_cnt;
    tgg_surrogate cud_surrogate;
    tsp_c2 cud_entrytype;
    tsp_c2 cud_linkage;
    tsp_c2 cud_segmentid;
    tsp_c2 cud_filler1;
    tgg_surrogate cud_owner;
    tsp_knl_identifier cud_name;
    } tak_col_uses_dom;
typedef
  struct ptoc_Type85
    {
    tsp_int2 seq_reclen;
    tsp_int2 seq_keylen;
    tsp_int2 seq_varcol_offset;
    tsp_int2 seq_varcol_cnt;
    tgg_surrogate seq_surrogate;
    tsp_c2 seq_entrytype;
    tsp_c2 seq_linkage;
    tsp_c2 seq_segmentid;
    char seq_comment;
    char seq_public;
    tsp_int4 seq_date;
    tsp_int4 seq_time;
    tsp_number seq_value;
    tgg_surrogate seq_owner;
    tgg_seq_info seq_info;
    tsp_knl_identifier seq_name;
    } tak_sequencerecord;
typedef
  struct ptoc_Type86
    {
    tak_sysbufferoffset rtablerefkey1;
    tsp_knl_identifier rtablerefkey2;
    tsp_c2 rsegmentid;
    char rsystable;
    char rfiller;
    unsigned char rtablekind;
    char rundefbyte;
    tgg_surrogate rtableid;
    tgg_surrogate rsynid;
    } tak_tablereference;
typedef
  tak00_colinfo_ptr tak_columnarr[1536];
typedef
  struct tak_basecolinfo *tak_basecolinfo_ptr;
struct tak_basecolinfo
  {
  tsp_int4 bci_index;
  tsp_int2 bci_rec_cnt;
  tsp_int2 bci_ref_cnt;
  tsp_int2 bci_buf_size;
  tsp_int2 bci_fill1;
  tsp_int4 bci_fill2;
  tak_basecolinfo_ptr bci_next;
  union
    {
    struct
      {
      tsp00_C4 bci_align[1024];
      } C_true;
    struct
      {
      unsigned char bci_buf[4096];
      } C_false;
    } boolean;
  } ;
typedef
  struct ptoc_Type87
    {
    tsp_int2 breclen;
    tsp_int2 bkeylen;
    tsp_int2 bvarcol_offset;
    tsp_int2 bvarcol_cnt;
    tgg_surrogate bsurrogate;
    tsp_c2 bentrytype;
    tsp_c2 bseqno;
    tsp_c2 bsegmentid;
    tsp_int2 bfirstindex;
    tsp_int2 blastindex;
    unsigned char btablekind;
    tgg_linkset blinkexist;
    tgg00_FileId btreeid;
    tgg_surrogate bauthid;
    tsp_int2 breccnt;
    tsp_int2 bmaxcol;
    tsp_int2 bmaxreclen;
    tsp_int2 bkeycolcount;
    tsp_int2 blenfixedcol;
    tsp_int2 bvarcolcount;
    tsp_int2 bcntdropcol;
    tsp_int2 bstringcount;
    tsp00_Uint1 blongvarcolcnt;
    char bcomment;
    tsp_int2 bavgrowlen;
    tsp_int2 blastkeyind;
    tsp_int2 bfirstcolind;
    tsp_int4 bdatecreate;
    tsp_int4 btimecreate;
    tsp_int4 bupdstatdate;
    tsp_int4 bupdstattime;
    tsp_int4 bpages;
    tsp_int4 brows;
    tsp_int4 balterdate;
    tsp_int4 baltertime;
    tsp_int4 bsample;
    tgg_privilege_set bpriv_all_set;
    tak_priv_desc bpriv_col_exist;
    char bunloaded;
    char bindexexist;
    tsp00_Uint1 bv_level;
    tsp00_Uint1 bv_tabcount;
    unsigned char bv_tablekind;
    char bv_checkopt;
    char bv_qualexist;
    unsigned char bv_distinct;
    char bv_viewlist;
    char bdef_stamp;
    char bcorr_view;
    tsp_sqlmode bsqlmode;
    tsp_int2 bnamed_constr;
    unsigned char bshow_kind;
    char brecreate_view;
    unsigned char bshowkind;
    tak_table_attribute_set battributes;
    tsp_knl_identifier_ptr btablen;
    tak_basecolinfo_ptr bptr;
    union
      {
      struct
        {
        tak_colindex bextcolindex;
        } C_1;
      struct
        {
        tak_columnarr bcolumn;
        } C_2;
      } integer;
    } tak_baserecord;
typedef
  unsigned char tak_default_value[8072];
typedef
  struct ptoc_Type88
    {
    tsp_int2 dfreclen;
    tsp_int2 dfkeylen;
    tsp_int2 dfvarcol_offset;
    tsp_int2 dfvarcol_cnt;
    tgg_surrogate dfsurrogate;
    tsp_c2 dfentrytype;
    tsp_c2 dfseqno;
    tsp_c2 dfsegment_id;
    tsp_int2 dfdefault_function;
    tak_default_value dfvalues;
    } tak_defaultrecord;
typedef
  tgg00_StackEntry tak_ak_constr_stack[989];
typedef
  struct ptoc_Type89
    {
    tsp_int2 ad_rec_len;
    tsp_int2 ad_key_len;
    tsp_int2 ad_varcol_offset;
    tsp_int2 ad_varcol_cnt;
    tgg_surrogate ad_surrogate;
    tsp_c2 ad_entrytype;
    tsp_c2 ad_linkage;
    tsp_c2 ad_segmentid;
    tsp_c2 ad_filler;
    tsp_int4 ad_createdate;
    tsp_int4 ad_createtime;
    tsp_int4 ad_alterdate;
    tsp_int4 ad_altertime;
    } tak_alterdaterecord;
typedef
  unsigned char tak_comment_text[8068];
typedef
  struct ptoc_Type90
    {
    tsp_int2 com_rec_len;
    tsp_int2 com_key_len;
    tsp_int2 com_varcol_offset;
    tsp_int2 com_varcol_cnt;
    tgg_surrogate com_surrogate;
    tsp_c2 com_entrytype;
    tsp_c2 com_linkage;
    unsigned char com_type;
    tsp_c1 com_colno;
    tsp_c2 com_segmentid;
    tsp_int4 com_length;
    tak_comment_text com_text;
    } tak_comment_record;
typedef
  struct ptoc_Type91
    {
    tsp_int2 crec_len;
    tsp_int2 ckey_len;
    tsp_int2 cvarcol_offset;
    tsp_int2 cvarcol_cnt;
    tgg_surrogate csurrogate;
    tsp_c2 centrytype;
    tsp_c2 cseqno;
    tsp_c2 csegmentid;
    tsp_int2 cstack_cnt;
    tsp_int2 cvalue_pos;
    char cfunction;
    char coldrange;
    tak_columnset ccolset;
    tak_oldidentifier ccname;
    tsp00_Uint1 cstackstart;
    tsp00_Uint1 csuffixlen;
    tsp_int2 cfiller;
    union
      {
      struct
        {
        tak_ak_constr_stack cstack;
        } C_true;
      struct
        {
        tsp_knl_identifier csuffix;
        } C_false;
      } boolean;
    } tak_constraintrecord;
typedef
  unsigned char tak_funcref_restkey[56];
typedef
  struct ptoc_Type92
    {
    tsp_int2 fct_reclen;
    tsp_int2 fct_keylen;
    tsp_int2 fct_varcol_offset;
    tsp_int2 fct_varcol_cnt;
    tsp_c8 fct_key1;
    tsp_c2 fct_entrytype;
    tsp_c2 fct_linkage;
    tak_funcref_restkey fct_key2;
    tsp_c2 fct_segmentid;
    tsp00_Uint1 fct_proc_cnt;
    char fct_comment;
    tsp_int4 fct_fill2;
    struct ptoc_Type93
      {
      tgg_surrogate ofp_procid;
      tsp_c3 ofp_language;
      unsigned char ofp_sqlmode[1];
      } fct_procids[64];
    } tak_funcrefrecord;
typedef
  struct ptoc_Type94
    {
    tsp_int2 cnreclen;
    tsp_int2 cnkeylen;
    tsp_int2 cnvarcol_offset;
    tsp_int2 cnvarcol_cnt;
    tsp_c12 cnkey1;
    tsp_knl_identifier cnkey2;
    tsp_c2 cnsegmentid;
    tsp_int2 cnid;
    tsp_int2 cnfiller;
    } tak_constraintnamerecord;
typedef
  struct ptoc_Type95
    {
    tsp_int2 ireclen;
    tsp_int2 ikeylen;
    tsp_int2 ivarcol_offset;
    tsp_int2 ivarcol_cnt;
    tgg_surrogate isurrogate;
    tsp_c2 ientrytype;
    tsp_c2 iseqno;
    tsp_c2 isegmentid;
    tsp00_Uint1 indexcount;
    char indexnext;
    struct ptoc_Type96
      {
      tak_oldidentifier indexn_prefix;
      tsp_int2 indexno;
      tsp00_Uint1 icount;
      char ifill;
      tak_colsequence icolseq;
      char iunique;
      char icomment;
      tsp_int4 idatecre;
      tsp_int4 itimecre;
      tsp_int4 irows;
      tsp_int4 ipages;
      tsp_int4 iinvl;
      char ifiller;
      char idisabled;
      char ifiller1;
      tsp00_Uint1 isuffixlen;
      tak_16stackentry icolstack;
      } indexdef[38];
    } tak_mindexrecord;
typedef
  struct ptoc_Type97
    {
    tsp_int2 lreclen;
    tsp_int2 lkeylen;
    tsp_int2 lvarcol_offset;
    tsp_int2 lvarcol_cnt;
    tgg_surrogate lsurrogate;
    tsp_c2 lentrytype;
    tsp_c2 lseqno;
    tsp_c2 lsegmentid;
    tsp_int2 linkcount;
    struct ptoc_Type98
      {
      tgg_surrogate ltableid;
      tak_oldidentifier linkn_prefix;
      tsp_int2 lfiller1;
      tsp_int4 ldatecre;
      tsp_int4 ltimecre;
      unsigned char ldatatyp;
      tsp00_Uint1 laction;
      tsp00_Uint1 lcolcount;
      tsp_c1 lindexid;
      char lcomment;
      tsp00_Uint1 lsuffixlen;
      tsp_int2 lfiller2;
      tak_colsequence lprimcolseq;
      tak_colsequence lseccolseq;
      tak_colsequence lrecpos;
      tak_16stackentry lstack;
      } linkdef[30];
    } tak_linkrecord;
typedef
  struct ptoc_Type99
    {
    tsp_int2 lreclen;
    tsp_int2 lkeylen;
    tsp_int2 lvarcol_offset;
    tsp_int2 lvarcol_cnt;
    tgg_surrogate lsurrogate;
    tsp_c2 lentrytype;
    tsp_c2 lsegno;
    tsp_knl_identifier lkeyname;
    tsp_c2 lsegmentid;
    tsp_int2 ldefault_len;
    tsp_int2 sfiller;
    tsp_key ldefault_key;
    } tak_link_def_record;
typedef
  struct ptoc_Type100
    {
    tsp_int2 lpreclen;
    tsp_int2 lpkeylen;
    tsp_int2 lpvarcol_offset;
    tsp_int2 lpvarcol_cnt;
    tgg_surrogate lpsurrogate;
    tsp_c2 lpentrytype;
    tsp_c2 lpseqno;
    tsp_c2 lpsegmentid;
    tsp_int2 lpcount;
    struct ptoc_Type101
      {
      tgg00_FileId lptreeid;
      tgg00_FileId lpauxfile;
      tsp_int2 lpfiller;
      tsp00_Uint1 lpcolcount;
      char lp_one_to_one;
      tak_16stackentry lpstack;
      } lpinfo[30];
    } tak_linkparsrecord;
typedef
  unsigned char tak_sresstate[1];
typedef
  struct ptoc_Type102
    {
    tsp_int2 resreclen;
    tsp_int2 reskeylen;
    tsp_int2 resvarcol_offset;
    tsp_int2 resvarcol_cnt;
    tgg_surrogate reskey_idfill;
    tsp_c2 reskey_identr;
    tsp_knl_identifier reskey_name;
    tsp_knl_identifier reskey_modul_name;
    tsp00_Uint1 resinfobuf;
    tsp00_Uint1 resparsinfobyte;
    tgg00_FileId restreeid;
    tgg_surrogate resunlocktabid;
    tgg91_TransNo restrans;
    tsp_int2 ressubfill;
    tgg00_SubtransNo ressubtrans;
    tak_parskey reseparsk;
    tak_sresstate resstate;
    tsp00_Uint1 resmaxlinkage;
    unsigned char resdecresdel;
    unsigned char resdatetimeformat;
    char resimpl_upd_stat;
    char resexecuted;
    unsigned char resselect_fetch;
    tgg_surrogate resanalyze_cmdid;
    } tak_resname_record;
typedef
  tgg00_StackEntry tak_trigger_stack[900];
typedef
  struct ptoc_Type103
    {
    tsp_int2 tr_reclen;
    tsp_int2 tr_keylen;
    tsp_int2 tr_varcol_offset;
    tsp_int2 tr_varcol_cnt;
    tgg_surrogate tr_surrogate;
    tsp_c2 tr_entrytype;
    tsp_c2 tr_linkage;
    tsp_c2 tr_segmentid;
    tsp_int2 tr_param_cnt;
    tsp_int2 tr_condition_cnt;
    char tr_succ_exists;
    char tr_comment;
    tgg_surrogate tr_proc_id;
    tsp_int4 tr_date;
    tsp_int4 tr_time;
    tak_columnset tr_upd_set;
    tsp_int2 tr_filler1;
    tsp00_Uint1 tr_filler2;
    tsp00_Uint1 tr_name_len;
    union
      {
      struct
        {
        tsp_knl_identifier tr_name;
        } C_true;
      struct
        {
        tak_trigger_stack tr_stack;
        } C_false;
      } boolean;
    } tak_triggerrecord;
typedef
  struct ptoc_Type104
    {
    tsp_int2 usagereclen;
    tsp_int2 usagekeylen;
    tsp_int2 usagevarcol_offset;
    tsp_int2 usagevarcol_cnt;
    tgg_surrogate usagesurrogate;
    tsp_c2 usageentrytype;
    tsp_c2 usageseqno;
    tsp_c2 usagesegmentid;
    tsp_int2 usagefiller;
    tsp_int2 usagecount;
    char usagenext_exist;
    char usagefull;
    struct ptoc_Type105
      {
      tgg_surrogate usa_tableid;
      tsp_int2 usa_filler;
      unsigned char usa_tablekind;
      char usa_empty;
      } usagedef[672];
    } tak_usagerecord;
typedef
  struct ptoc_Type106
    {
    tsp_int2 usatabreclen;
    tsp_int2 usatabkeylen;
    tsp_int2 usatabvarcol_offset;
    tsp_int2 usatabvarcol_cnt;
    tgg_surrogate usatabsurrogate;
    tsp_c2 usatabentrytype;
    tsp_c2 usatabseqno;
    tsp_c2 usertabsegmentid;
    tsp_int2 usertabfiller;
    char usertabnext_exist;
    char usertabfull;
    tsp_int2 usertabcount;
    struct ptoc_Type107
      {
      tgg_surrogate ut_surrogate;
      char ut_empty;
      unsigned char ut_kind;
      tsp_int2 ut_filler;
      } usertabdef[672];
    } tak_usertablerecord;
typedef
  int tak_userpar_arr[6];
typedef
  unsigned char tak_userrectype[1];
typedef
  struct ptoc_Type108
    {
    tsp_int2 ureclen;
    tsp_int2 ukeylen;
    tsp_int2 uvarcol_offset;
    tsp_int2 uvarcol_cnt;
    tgg_surrogate usurrogate;
    tsp_c2 uentrytype;
    tsp_c2 useqno;
    tsp_c2 usersegmentid;
    char userfiller1;
    unsigned char userroleinfo;
    tak_userrectype urecordtyp;
    unsigned char userkind;
    char usersysdba;
    char uexclusive;
    tsp_int2 userfiller2;
    tsp_int2 userfiller3;
    tsp_cryptpw userpassw;
    tsp_cryptpw usupportpasspw;
    tgg_surrogate usergroup_id;
    tgg_surrogate userowner_id;
    tsp_int4 upwcreatedate;
    tsp_int4 upwcreatetime;
    tsp_int4 ucreatedate;
    tsp_int4 ucreatetime;
    tak_userpar_arr uparams;
    tgg00_UserRef uperm_ref;
    char ucomment;
    tsp00_Uint1 userrolecnt;
    tsp_int4 ualterdate;
    tsp_int4 ualtertime;
    tsp_knl_identifier username;
    } tak_userrecord;
typedef
  struct ptoc_Type109
    {
    tsp_int2 us_reclen;
    tsp_int2 us_keylen;
    tsp_int2 us_varcol_offset;
    tsp_int2 us_varcol_cnt;
    tgg_surrogate us_surrogate;
    tsp_c2 us_entrytype;
    tsp_c2 us_linkage;
    tsp_c2 us_segmentid;
    tsp_c2 us_filler;
    tsp_c4 us_userid;
    } tak_useridrecord;
typedef
  unsigned char tak_uref_restkey[56];
typedef
  struct ptoc_Type110
    {
    tak_sysbufferoffset ru_key_offset;
    tak_uref_restkey ru_rest_key;
    tsp_c2 ru_segmentid;
    tsp_int2 ru_filler;
    tgg_surrogate ru_user_id;
    } tak_userrefrecord;
typedef
  struct ptoc_Type111
    {
    tsp_int2 sysreclen;
    tsp_int2 syskeylen;
    tsp_int2 sysvarcol_offset;
    tsp_int2 sysvarcol_cnt;
    tgg_surrogate syssurrogate;
    tsp_c2 sysentrytyp;
    tsp_c2 syssegno;
    tsp_c2 syssegmentid;
    char sysactivated;
    tsp00_Uint1 sysversion;
    tgg_surrogate sysauthid;
    } tak_sysdbarecord;
typedef
  struct ptoc_Type112
    {
    tsp_int2 sitereclen;
    tsp_int2 sitekeylen;
    tsp_int2 sitevarcol_offset;
    tsp_int2 sitevarcol_cnt;
    tsp_c12 sitekey;
    tsp_c2 sitesegmentid;
    tgg_serverdb_no sitemap_no;
    tsp_dbname siteserverdbname;
    unsigned char sitetype;
    char sitefiller1;
    tsp_nodeid siteid;
    tsp_int4 siterefcount;
    } tak_serverdbrecord;
typedef
  struct ptoc_Type113
    {
    tsp_int2 snreclen;
    tsp_int2 snkeylen;
    tsp_int2 snvarcol_offset;
    tsp_int2 snvarcol_cnt;
    tsp_c12 snkey1;
    tsp_dbname snkey2;
    tsp_c2 snsegmentid;
    tsp_nodeid snnodeid;
    tgg_serverdb_no snsite;
    unsigned char snsitetype;
    tsp_code_type sncode;
    tak_oldidentifier sntermchar;
    tgg_serverdb_no snremote_site;
    tgg_serverdb_no snlocal_site;
    tsp_int2 snfiller2;
    } tak_serverdbnamerecord;
typedef
  tsp1_param_spec_list *tak_param_spec_ptr;
typedef
  struct ptoc_Type114
    {
    tak_param_spec_ptr param_spec_ptr;
    tsp_int2 param_counter;
    tsp_int2 param_filler1;
    tsp_int4 param_filler2;
    } tak_param_list;
typedef
  struct ptoc_Type115
    {
    unsigned char param_datatype;
    tsp00_Uint1 param_datafrac;
    tsp_int2 param_datalength;
    tsp_int2 param_inoutlength;
    tsp_c2 param_in_out;
    tsp_int2 param_com_datatype;
    tsp_c2 param_type_id;
    tsp_int2 param_cpp_offset;
    tsp_int2 param_ascii_offset;
    tsp_int2 param_unicode_offset;
    unsigned char param_new_old;
    unsigned char param_name_len;
    tsp_c256 param_name;
    } tak_param_info;
typedef
  tak_param_info *tak_param_array[1024];
typedef
  struct ptoc_Type116
    {
    tsp_int2 me_reclen;
    tsp_int2 me_keylen;
    tsp_int2 me_varcol_offset;
    tsp_int2 me_varcol_cnt;
    tgg_surrogate me_surrogate;
    tsp_c2 me_entrytype;
    tsp_c2 me_linkage;
    tsp_c2 me_segmentid;
    tsp_int2 me_param_cnt;
    unsigned char me_type;
    unsigned char me_debug;
    char me_sql;
    char me_cursor;
    tsp_int4 me_dispid;
    tsp_c16 me_coclsid;
    tsp_c16 me_iid;
    tgg_surrogate me_package_id;
    tak_param_array me_param_list;
    } tak_methodrecord;
typedef
  struct ptoc_Type117
    {
    tsp_int2 mei_reclen;
    tsp_int2 mei_keylen;
    tsp_int2 mei_varcol_offset;
    tsp_int2 mei_varcol_cnt;
    tgg_surrogate mei_surrogate;
    tsp_c2 mei_entrytype;
    tsp_c2 mei_linkage;
    tsp_c2 mei_segmentid;
    char mei_comment;
    char mei_filler;
    tgg_surrogate mei_owner;
    tsp_int4 mei_date;
    tsp_int4 mei_time;
    tsp_c64 mei_prog_id;
    tsp_knl_identifier mei_name;
    tsp_c132 mei_debug_location;
    } tak_method_inforecord;
typedef
  struct ptoc_Type118
    {
    tsp_int2 mrf_reclen;
    tsp_int2 mrf_keylen;
    tsp_int2 mrf_varcol_offset;
    tsp_int2 mrf_varcol_cnt;
    tgg_surrogate mrf_surrogate;
    tsp_c2 mrf_entrytype;
    tsp_c2 mrf_linkage;
    tsp_knl_identifier mrf_methodname;
    tsp_c2 mrf_segmentid;
    unsigned char mrf_type;
    char mrf_filler;
    tgg_surrogate mrf_method_id;
    } tak_methodrefrecord;
typedef
  struct ptoc_Type119
    {
    tsp_int2 tum_reclen;
    tsp_int2 tum_keylen;
    tsp_int2 tum_varcol_offset;
    tsp_int2 tum_varcol_cnt;
    tsp_c8 tum_uid1;
    tsp_c2 tum_entrytype;
    tsp_c2 tum_linkage;
    tsp_c8 tum_uid2;
    tsp_c2 tum_segmentid;
    tsp_c2 tum_id;
    } tak_type_uid_map_record;
typedef
  struct ptoc_Type120
    {
    tsp_int2 cmr_reclen;
    tsp_int2 cmr_keylen;
    tsp_int2 cmr_varcol_offset;
    tsp_int2 cmr_varcol_cnt;
    tgg_surrogate cmr_surrogate;
    tsp_c2 cmr_entrytype;
    tsp_c2 cmr_linkage;
    tsp_c2 cmr_segmentid;
    tsp_int4 cmr_container_no;
    tsp00_Int4 cmr_schema;
    tsp_c16 cmr_guid;
    } tak_class_map_guid_record;
typedef
  struct ptoc_Type121
    {
    tsp_int2 freclen;
    tsp_int2 fkeylen;
    tsp_int2 fvarcol_offset;
    tsp_int2 fvarcol_cnt;
    tgg00_Surrogate fschema;
    tsp_c2 fentrytype;
    tsp_c2 flinkage;
    tsp_c16 fguid;
    tsp_c2 fsegmentid;
    char findexexist;
    char ffiller;
    tsp_int4 fsize;
    tgg00_FileId ftree;
    tsp_int4 fdate;
    tsp_int4 ftime;
    tsp_knl_identifier fclassname;
    } tak_class_record;
typedef
  struct ptoc_Type122
    {
    tsp_int2 schr_reclen;
    tsp_int2 schr_keylen;
    tsp_int2 schrvarcol_offset;
    tsp_int2 schrvarcol_cnt;
    tgg00_Surrogate schr_surrogate;
    tsp_c2 schr_entrytype;
    tsp_c2 schr_linkage;
    tsp00_KnlIdentifier schr_name;
    tsp_c2 schr_segmentid;
    tsp00_Int4 schr_filler;
    tsp00_Int4 schr_schema_id;
    } tak_schema_ref_record;
typedef
  struct ptoc_Type123
    {
    tsp_int2 sch_reclen;
    tsp_int2 sch_keylen;
    tsp_int2 sch_varcol_offset;
    tsp_int2 sch_varcol_cnt;
    tgg00_Surrogate sch_surrogate;
    tsp_c2 sch_entrytype;
    tsp_c2 sch_linkage;
    tsp_c2 sch_segmentid;
    tsp_c2 sch_filler;
    tsp_int4 sch_date;
    tsp_int4 sch_time;
    tsp_knl_identifier sch_name;
    } tak_schema_record;
typedef
  struct ptoc_Type124
    {
    tsp_int2 vreclen;
    tsp_int2 vkeylen;
    tsp_int2 vvarcol_offset;
    tsp_int2 vvarcol_cnt;
    tgg_surrogate vsurrogate;
    tsp_c2 ventrytype;
    tsp_c2 vlinkage;
    tak_oms_version_id vversion_id;
    tsp_c2 vsegmentid;
    tgg91_TransNo vconsistview;
    char vmarked;
    char vfiller;
    tsp_int4 vdate;
    tsp_int4 vtime;
    tgg00_FileId vupdtree;
    } tak_version_record;
typedef
  struct ptoc_Type125
    {
    tsp_int2 role_reclen;
    tsp_int2 role_keylen;
    tsp_int2 role_varcol_offset;
    tsp_int2 role_varcol_cnt;
    tgg_surrogate role_key1;
    tsp_c2 role_entrytype;
    tsp_c2 role_linkage;
    tak_uref_restkey role_rest_key;
    tsp_c2 role_segmentid;
    tsp_c2 role_filler;
    tgg_surrogate role_owner;
    tgg_surrogate role_id;
    tsp_int4 role_date;
    tsp_int4 role_time;
    tsp_cryptpw role_password;
    } tak_rolerecord;
typedef
  struct ptoc_Type126
    {
    tsp_int2 synreclen;
    tsp_int2 synkeylen;
    tsp_int2 synvarcol_offset;
    tsp_int2 synvarcol_cnt;
    tgg_surrogate synsurrogate;
    tsp_c2 synentrytype;
    tsp_c2 synseqno;
    tsp_c2 synsegmentid;
    tsp_int2 syn_maxcol;
    char syn_internal;
    char synfiller0;
    unsigned char syn_tablekind;
    char syncomment;
    tgg_surrogate syn_tableid;
    tsp_int4 syn_time;
    tsp_int4 syn_date;
    tgg_surrogate syn_owner;
    tgg_surrogate syn_authid;
    tsp_knl_identifier syn_tablen;
    } tak_synonymrecord;
typedef
  struct ptoc_Type127
    {
    tsp_int2 cireclen;
    tsp_int2 cikeylen;
    tsp_int2 civarcol_offset;
    tsp_int2 civarcol_cnt;
    tgg_surrogate cisurrogate;
    tsp_c2 cientrytype;
    tsp_c2 cilinkage;
    tsp00_C8 cicommand_id;
    } tak_commandinforecord;
typedef
  unsigned char tak_jcolpropset[1];
typedef
  unsigned char tak_viewtext[8064];
typedef
  struct ptoc_Type128
    {
    tsp_int2 vtreclen;
    tsp_int2 vtkeylen;
    tsp_int2 vtvarcol_offset;
    tsp_int2 vtvarcol_cnt;
    tgg_surrogate vtsurrogate;
    tsp_c2 vtentrytype;
    tsp_c2 vtsegno;
    tsp_c2 vtsegmentid;
    tsp_int2 vtfromtabcnt;
    tsp_int2 vttabcount;
    tsp_int2 vttextlength;
    char vtnextexist;
    tsp00_Uint1 vtcontext;
    tsp_int2 vtselect_pos;
    union
      {
      struct
        {
        struct ptoc_Type129
          {
          tgg_surrogate vtttableid;
          tgg_surrogate vttreftabid;
          tsp_int2 vttfiller;
          char vttrefname;
          tsp00_Uint1 vtttabcount;
          char vttsynonym;
          char vttcomplex_view;
          tsp_int2 vtttab_n_pos;
          } vttab[64];
        } C_true;
      struct
        {
        tak_viewtext vttbuf;
        } C_false;
      } boolean;
    } tak_viewtextrecord;
typedef
  struct ptoc_Type130
    {
    tsp_int2 vdreclen;
    tsp_int2 vdkeylen;
    tsp_int2 vdvarcol_offset;
    tsp_int2 vdvarcol_cnt;
    tgg_surrogate vdsurrogate;
    tsp_c2 vdentrytype;
    tsp_int2 vdsegno;
    tsp_c2 vsegmentid;
    tsp_int2 vdesc_cnt;
    char vdesc_next;
    char vdesc_fill1;
    tsp_int2 vdesc_fill2;
    struct ptoc_Type131
      {
      tsp_int2 vfromtabno;
      tsp_int2 vn_pos;
      tsp_int2 vfromextcolno;
      tsp_int2 vextcolno;
      unsigned char vdatatype;
      char vfiller;
      tsp_int2 vfiller1;
      } vdescription[672];
    } tak_viewdescrecord;
typedef
  struct ptoc_Type132
    {
    tgg_surrogate vttableid;
    tsp_int2 vtfiller;
    char vtins_for_upd;
    char vtone_to_one;
    tak_charset vtnot_used_links;
    } tak_viewtabarr[183];
typedef
  struct ptoc_Type133
    {
    tsp_int2 j1fieldno;
    tsp_int2 j1startstack;
    tsp00_Uint1 j1tableno;
    tsp00_Uint1 j1cntstack;
    tak_jcolpropset j1propset;
    unsigned char j1fill;
    tsp_int2 j1inoutlen;
    tsp_int2 j2fieldno;
    tsp00_Uint1 j2tableno;
    tsp00_Uint1 j2cntstack;
    tak_jcolpropset j2propset;
    unsigned char j2fill;
    tsp_int2 j2inoutlen;
    unsigned char j12operator;
    unsigned char j12fill1;
    tsp_int4 j12fill2;
    } tak_viewjoins[336];
typedef
  tgg00_StackEntry tak_view_stack_entries[1007];
typedef
  unsigned char tak_viewbuf[8056];
typedef
  struct ptoc_Type134
    {
    tsp_int2 drecno;
    tsp_int2 dextno;
    tsp_int2 dfiller;
    tsp00_Uint1 dexttabno;
    tsp00_Uint1 dtabno;
    } tak_derived_col_info[1009];
typedef
  struct ptoc_Type135
    {
    tsp_int2 vreclen;
    tsp_int2 vkeylen;
    tsp_int2 vvarcol_offset;
    tsp_int2 vvarcol_cnt;
    tgg_surrogate vsurrogate;
    tsp_c2 ventrytype;
    tsp_c2 vlinkage;
    tsp_c2 vsegmentid;
    char vfiller;
    char vjoin_exists;
    char vstack_exists;
    char vderived_exists;
    tsp_int2 vbasetabcnt;
    tak_viewtabarr vtable;
    } tak_viewqual_basis_record;
typedef
  struct ptoc_Type136
    {
    tsp_int2 vreclen;
    tsp_int2 vkeylen;
    tsp_int2 vvarcol_offset;
    tsp_int2 vvarcol_cnt;
    tgg_surrogate vsurrogate;
    tsp_c2 ventrytype;
    tsp_c2 vlinkage;
    tsp_c2 vsegmentid;
    tsp_int2 vjoincount;
    tak_viewjoins vjoin;
    } tak_viewqual_join_record;
typedef
  struct ptoc_Type137
    {
    tsp_int2 vreclen;
    tsp_int2 vkeylen;
    tsp_int2 vvarcol_offset;
    tsp_int2 vvarcol_cnt;
    tgg_surrogate vsurrogate;
    tsp_c2 ventrytype;
    tsp_c2 vlinkage;
    tsp_c2 vsegmentid;
    tsp_c2 vfiller;
    tsp_int2 vview_offs;
    tsp_int2 vstcount;
    tsp_int4 vdatapos;
    tsp_int4 vdatalen;
    union
      {
      struct
        {
        tak_view_stack_entries vstack;
        } C_true;
      struct
        {
        tak_viewbuf vdata;
        } C_false;
      } boolean;
    } tak_viewqual_stack_record;
typedef
  struct ptoc_Type138
    {
    tsp_int2 vreclen;
    tsp_int2 vkeylen;
    tsp_int2 vvarcol_offset;
    tsp_int2 vvarcol_cnt;
    tgg_surrogate vsurrogate;
    tsp_c2 ventrytype;
    tsp_c2 vlinkage;
    tsp_c2 vsegmentid;
    tsp_int2 vderivedcnt;
    tak_derived_col_info vderived_cols;
    } tak_viewqual_derivedcol_record;
typedef
  struct ptoc_Type139
    {
    tsp_int2 vkreclen;
    tsp_int2 vkkeylen;
    tsp_int2 vkvarcol_offset;
    tsp_int2 vkvarcol_cnt;
    tsp_c12 vkkey;
    tsp_c2 vksegmentid;
    tsp_int2 vkjvkeylen;
    tsp_key vkjvkey;
    } tak_viewkeyrecord;
typedef
  struct ptoc_Type140
    {
    tsp_int2 sti_reclen;
    tsp_int2 sti_keylen;
    tsp_int2 sti_varcol_offset;
    tsp_int2 sti_varcol_cnt;
    tgg_surrogate sti_tabid;
    tsp_c2 sti_entrytype;
    tsp_c2 sti_linkage;
    tsp_c2 sti_segmentid;
    tsp_int2 sti_col_cnt;
    tsp_int4 sti_rows;
    tsp_int4 sti_pages;
    tsp_int4 sti_date;
    tsp_int4 sti_time;
    tsp_int2 sti_filler;
    tak_columnset sti_index;
    struct ptoc_Type141
      {
      tsp_int4 cst_dist_values;
      tsp_int4 cst_pages;
      tsp_int4 cst_avg_lst_len;
      } sti_col_stats[660];
    } tak_statisticsinfo;
typedef
  struct ptoc_Type142
    {
    tsp_int2 breclen;
    tsp_int2 bkeylen;
    tsp_int2 bvarcol_offset;
    tsp_int2 bvarcol_cnt;
    tsp_c12 bkey;
    tsp_c2 bsegmentid;
    tsp_int2 bfirstindex;
    tsp_int2 blastindex;
    unsigned char btablekind;
    tgg_linkset blinkexist;
    tgg00_FileId btreeid;
    tgg_surrogate bauthid;
    tsp_int2 breccnt;
    tsp_int2 bmaxcol;
    tsp_int2 bmaxreclen;
    tsp_int2 bkeycolcount;
    tsp_int2 blenfixedcol;
    tsp_int2 bvarcolcount;
    tsp_int2 bcntdropcol;
    tsp_int2 bstringcount;
    tsp00_Uint1 blongvarcolcnt;
    char bcomment;
    tsp_int2 bavgrowlen;
    tsp_int2 blastkeyind;
    tsp_int2 bfirstcolind;
    tsp_int4 bdatecreate;
    tsp_int4 btimecreate;
    tsp_int4 bupdstatdate;
    tsp_int4 bupdstattime;
    tsp_int4 bpages;
    tsp_int4 brows;
    tsp_int4 balterdate;
    tsp_int4 baltertime;
    tsp_int4 bsample;
    tsp_int2 boutcolno;
    char bunloaded;
    char bindexexist;
    tsp00_Uint1 bv_level;
    tsp00_Uint1 bv_tabcount;
    unsigned char bv_tablekind;
    char bv_checkopt;
    char bv_qualexist;
    unsigned char bv_distinct;
    char bv_viewlist;
    char bdef_stamp;
    char bcorr_view;
    tsp_sqlmode bsqlmode;
    tsp_int2 bnamed_constr;
    unsigned char bshow_kind;
    char brecreate_view;
    unsigned char bshowkind;
    tak_table_attribute_set battributes;
    tsp_knl_identifier_ptr btablen;
    tak_basecolinfo_ptr bptr;
    union
      {
      struct
        {
        tak_colindex bextcolindex;
        } C_1;
      struct
        {
        tak_columnarr bcolumn;
        } C_2;
      } integer;
    } tak_resultrecord;
typedef
  struct ptoc_Type143
    {
    tsp_int2 str_reclen;
    tsp_int2 str_keylen;
    tsp_int2 str_varcol_offset;
    tsp_int2 str_varcol_cnt;
    tgg_surrogate str_key;
    tsp_c2 str_etype;
    tsp_c2 str_linkage;
    tsp_int2 str_count;
    tsp_int2 str_filler;
    struct ptoc_Type144
      {
      tsp_knl_identifier str_name;
      tgg00_SubtransNo str_no;
      } str_subtrans[50];
    } tak_subtransrecord;
typedef
  struct ptoc_Type145
    {
    tsp_int2 scd_reclen;
    tsp_int2 scd_keylen;
    tsp_int2 scd_varcol_offset;
    tsp_int2 scd_varcol_cnt;
    tgg_surrogate scd_colkey;
    tsp_c2 scd_etype;
    tsp_c2 scd_filler;
    tgg_string_filedescr scd_stringfd;
    tgg00_Lock scd_lock;
    tsp_key scd_key;
    } tak_scoldescrecord;
typedef
  struct ptoc_Type146
    {
    tsp_int2 ch_startpos;
    unsigned char ch_length;
    unsigned char ch_type[1];
    } tak_column_change_arr[1024];
typedef
  struct ptoc_Type147
    {
    tsp_int2 cr_colcount;
    char cr_was_opt;
    char cr_filler;
    tak_column_change_arr cr_columns;
    } tak_changerecord;
typedef
  struct ptoc_Type148
    {
    tsp_int2 ci_rec_len;
    tsp_int2 ci_key_len;
    tsp_int2 ci_varcol_offset;
    tsp_int2 ci_varcol_cnt;
    tgg_surrogate ci_surrogate;
    tsp_c2 ci_entrytype;
    tsp_int2 ci_segno;
    tsp_int4 ci_fullen;
    tsp_int2 ci_colcount;
    tsp_int2 ci_filler;
    tak_column_change_arr ci_ch_columns;
    } tak_changeinfos;
typedef
  struct ptoc_Type149
    {
    tsp_int2 ks_pos;
    tsp_int2 ks_len;
    } tak_keyspec;
typedef
  struct ptoc_Type150
    {
    tak_keyspec reckeyspec;
    tak_keyspec listkeyspec;
    } tak_two_keyspecs;
typedef
  unsigned char tak_res_keysbuf[13316];
typedef
  struct ptoc_Type151
    {
    tsp_int2 res_rec_len;
    tsp_int2 res_key_len;
    tsp_int2 res_varcol_offset;
    tsp_int2 res_varcol_cnt;
    tgg_surrogate res_surrogate;
    tsp_c2 res_entrytype;
    tsp_int2 res_segno;
    tsp_int4 res_fullen;
    tak_two_keyspecs res_startkeys;
    tak_two_keyspecs res_stopkeys;
    tak_two_keyspecs res_nextkeys;
    tsp_int4 res_nextpos;
    tak_two_keyspecs res_prevkeys;
    tsp_int4 res_prevpos;
    tak_keyspec res_updkey;
    tgg_two_tree_ids res_treeids;
    tgg00_FileId res_restree;
    tsp_knl_identifier res_resultname;
    tsp_int2 res_rowno;
    char res_order;
    char res_build;
    tsp_int4 res_actres;
    tsp_int2 res_reclen;
    tsp_int2 res_length;
    tsp_int2 res_keylen;
    tsp_int2 res_outcolno;
    tak_sresstate res_resstate;
    unsigned char res_decresdel;
    char res_first_fetch;
    char res_eof;
    tsp00_Uint1 res_useupdk;
    tsp_c1 res_updchar;
    char res_for_update;
    tgg_surrogate res_updtabid;
    tgg_surrogate res_unlocktab;
    tsp_int2 res_primkeylen;
    tsp_int2 res_minkeylen;
    tsp_int2 res_inv_only;
    unsigned char res_strat_info;
    tsp00_Uint1 res_parsinfob;
    unsigned char res_pars_dt_format;
    tsp_sqlmode res_pars_sqlmode;
    tsp00_Uint1 res_upd_tabcnt;
    unsigned char res_qual_kind;
    tsp_int4 res_known_pages;
    tsp_int4 res_searched_pages;
    tsp_int4 res_filler3;
    tgg_surrogate res_analyze_cmdid;
    union
      {
      struct
        {
        tak_changerecord res_change;
        } C_true;
      struct
        {
        tak_res_keysbuf res_keysbuf;
        } C_false;
      } boolean;
    } tak_resultkeyrecord;
typedef
  struct ptoc_Type152
    {
    union
      {
      struct
        {
        tak_oldidentifier licolname;
        tsp_c2 lityplen_data;
        tsp_c2 lifrac_inoutl;
        tsp_int2 libufpos;
        } C_true;
      struct
        {
        tsp_c20 lifiller1;
        tsp_int2 lilonglength;
        tsp_int2 lifiller2;
        } C_false;
      } boolean;
    } tak_longinfoarr[336];
typedef
  struct ptoc_Type153
    {
    tsp_int2 lireclen;
    tsp_int2 likeylen;
    tsp_int2 livarcol_offset;
    tsp_int2 livarcol_cnt;
    tgg_surrogate lisurrogate;
    tsp_c2 lientrytype;
    tsp_int2 lisegno;
    tsp_int2 lisl;
    tsp_int2 licount;
    tak_longinfoarr liinfo;
    } tak_longinforecord;
typedef
  tsp1_param_info tak_paraminfoarr[2700];
typedef
  struct ptoc_Type154
    {
    tsp_int2 sireclen;
    tsp_int2 sikeylen;
    tsp_int2 sivarcol_offset;
    tsp_int2 sivarcol_cnt;
    tgg_surrogate sisurrogate;
    tsp_c2 sientrytype;
    tsp_int2 sisegno;
    tsp_int4 sifullen;
    tsp_int2 sisl;
    tsp_int2 sicount;
    tak_paraminfoarr siinfo;
    } tak_shortinforecord;
typedef
  struct tak_systembuffer *tak_sysbufferaddress;
typedef
  unsigned char tak_colnamesbuf[8072];
typedef
  struct ptoc_Type155
    {
    tsp_int2 cnreclen;
    tsp_int2 cnkeylen;
    tsp_int2 cnvarcol_offset;
    tsp_int2 cnvarcol_cnt;
    tgg_surrogate cnsurrogate;
    tsp_c2 cnentrytype;
    tsp_int2 cnsegno;
    tsp_int4 cnbit64fill;
    union
      {
      struct
        {
        tsp_int4 cnfullen;
        tsp_int2 cncount;
        } C_true;
      struct
        {
        tak_colnamesbuf cnbuf;
        tak_sysbufferaddress cnnext;
        tak_sysbufferaddress cnlast;
        } C_false;
      } boolean;
    } tak_columnnamesrecord;
typedef
  unsigned char tak_fcolpropset[1];
typedef
  struct ptoc_Type156
    {
    union
      {
      struct
        {
        tsp_int2 lf_datalen;
        } C_true;
      struct
        {
        tsp00_Uint1 lf_fixed_datalen;
        tsp00_Uint1 lf_frac;
        } C_false;
      } boolean;
    } tak_len_and_frac;
typedef
  struct ptoc_Type157
    {
    tsp_int2 all_lock_count;
    tsp_int2 all_lock_fill;
    tgg00_Lock all_locks[240];
    } tak_ak_all_locks;
typedef
  struct ptoc_Type158
    {
    unsigned char fp_kind;
    unsigned char fp_etype;
    unsigned char fp_datatyp;
    tak_fcolpropset fp_colset;
    union
      {
      struct
        {
        tsp_int4 fp_frompos_v1;
        tsp_int2 fp_movebefore_v1;
        tsp_int2 fp_rangeextcolno;
        tak_len_and_frac fp_len_and_frac_v1;
        tsp_int2 fp_inoutlen_v1;
        } C_fp_catalog_column;
      struct
        {
        tsp_int4 fp_frompos_v2;
        tsp_int2 fp_movebefore_v2;
        tsp_int2 fp_stackpos;
        tak_len_and_frac fp_len_and_frac_v2;
        tsp_int2 fp_inoutlen_v2;
        } C_fp_val_all_without_l;
      struct
        {
        tsp_int4 fp_frompos_v3;
        tsp_int2 fp_movebefore_v3;
        tsp_int2 fp_fill_31;
        tak_len_and_frac fp_len_and_frac_v3;
        tsp_int2 fp_inoutlen_v3;
        } C_fp_lock_val;
      struct
        {
        tsp_int4 fp_fill_41;
        tsp_int2 fp_movebefore_v4;
        tsp_int2 fp_fill_42;
        tsp_int4 fp_fill_43;
        } C_fp_last_lock;
      struct
        {
        tsp_int2 fp_curr_keylen;
        tsp_int2 fp_startpos;
        tsp_int2 fp_movebefore_v5;
        tsp_int2 fp_fill_51;
        tsp_int4 fp_pos_no;
        } C_fp_current_of_key;
      struct
        {
        tsp_int2 fp_res_buf_index;
        unsigned char fp_glob_fp_kind;
        char fp_fill_61;
        tsp_int2 fp_movebefore_v6;
        tsp_int2 fp_rangeextcolno_v6;
        tak_len_and_frac fp_len_and_frac_v6;
        tsp_int2 fp_inoutlen_v6;
        } C_fp_val_from_res_buffer;
      struct
        {
        tsp_int4 fp_fill_71;
        tsp_int4 fp_fill_72;
        tsp_int2 fp_fill_73;
        tsp_int2 fp_len;
        } C_fp_fill_part2_to_len;
      struct
        {
        tsp_int4 fp_fill_81;
        tsp_int2 fp_movebefore_v8;
        tsp_int2 fp_keystack;
        tsp_int2 fp_to_stack;
        tsp_int2 fp_key_low_handl;
        } C_fp_ordered_select_key;
      struct
        {
        tsp_int4 fp_fill_101;
        tsp_int4 fp_fill_102;
        tsp_int2 fp_fill_103;
        tsp_c2 fp_escape_char;
        } C_fp_escape_val;
      struct
        {
        tsp_int4 fp_fill_111;
        tsp_int2 fp_movebeforev11;
        tsp_int2 fp_fill_112;
        tsp_int2 fp_reccolno;
        tsp_int2 fp_fill_113;
        } C_fp_pseudo_ins_select;
      struct
        {
        tsp_int4 fp_fromposv14;
        tsp_int4 fp_fill_141;
        tsp_int2 fp_datalenv14;
        tsp_int2 fp_dataiolenv14;
        } C_fp_result_name;
      struct
        {
        tgg_stack_entry fp_stackentry;
        tsp_int4 fp_fill_151;
        } C_fp_verify_not_null;
      struct
        {
        tsp_int4 fp_frompos_v15;
        tsp_int2 fp_movebefore_v15;
        unsigned char fp_sc_symbol;
        tsp00_C1 fp_objecttype;
        tak_len_and_frac fp_len_and_frac_v15;
        tsp_int2 fp_inoutlen_v15;
        } C_fp_get_name_value;
      } tak_fp_kind_type;
    } tak_parser_fields[2025];
typedef
  struct ptoc_Type159
    {
    union
      {
      struct
        {
        tsp_int2 p_reclen;
        tsp_int2 p_keylen;
        tsp_int2 p_varcol_offset;
        tsp_int2 p_varcol_cnt;
        tgg_surrogate p_surrogate;
        tsp_c2 p_entrytype;
        tsp_int2 p_segno;
        tsp_int4 p_fullen;
        tsp_int4 p_filler;
        tak_parsbuffer p_buf;
        } C_1;
      struct
        {
        tsp_int2 p1reclen;
        tsp_int2 p1keylen;
        tsp_int2 p1varcol_offset;
        tsp_int2 p1varcol_cnt;
        tgg_surrogate p1surrogate;
        tsp_c2 p1entrytype;
        tsp_int2 p1segno;
        tsp_int4 p1_fullen;
        tsp_int4 p1_filler;
        tgg00_FileId p_treeid;
        tsp_int4 p_pagecnt;
        tsp_knl_identifier p_resn;
        tsp_knl_identifier p_modul_name;
        tsp_knl_identifier p_searchname;
        tgg_message_type p_mtyp;
        tgg_message2_type p_mtyp2;
        char p_select;
        char p_single;
        tgg_surrogate p_tabid;
        tsp_int2 p_int_res_cnt;
        tsp_int2 p_joinviewtabcnt;
        tsp_int2 p_p_no;
        tsp_int2 p_subcntlevel;
        tsp_int4 p_posno;
        unsigned char p_statement_kind;
        char p_changeinfos_exist;
        char p_const_param_expr;
        tsp00_Uint1 p_precomp_info_byte;
        char p_reuse;
        char p_prepare;
        tsp_int2 p_function_code;
        tsp_sqlmode p_sqlmode;
        char p_filler0;
        tgg00_HandlingSet p_handling_set;
        tsp00_Uint1 p_session_isolevel;
        char p_use_sess_isolevel;
        tsp_int2 p_resreclen;
        tsp_int2 p_resinfolen;
        tsp_int2 p_j_v_keylen;
        char p_longliteral;
        char p_parsing_again;
        tsp_c2 p_filler1;
        tsp_int2 p_max_infos;
        tsp_int2 p_cnt_infos;
        tak_parser_fields p_pars_infos;
        } C_2;
      struct
        {
        tsp_int2 p2reclen;
        tsp_int2 p2keylen;
        tsp_int2 p2varcol_offset;
        tsp_int2 p2varcol_cnt;
        tgg_surrogate p2surrogate;
        tsp_c2 p2entrytype;
        tsp_int2 p2segno;
        tsp_int4 p2fullen;
        tsp_int4 p2filler;
        tsp_int2 p_use_fileid;
        tsp_int2 p_sa_level;
        tsp_int2 p_lo_level;
        } C_3;
      struct
        {
        tsp_int2 p3reclen;
        tsp_int2 p3keylen;
        tsp_int2 p3varcol_offset;
        tsp_int2 p3varcol_cnt;
        tgg_surrogate p3surrogate;
        tsp_c2 p3entrytype;
        tsp_int2 p3segno;
        unsigned char p_tl_filler1[296];
        tsp_int4 p_temp_lock_atch;
        tak_ak_all_locks p_temp_lock;
        } C_4;
      struct
        {
        unsigned char p_sysbufferoffset[20];
        unsigned char p_pars_header[296];
        tak_paraminfoarr p_short_info;
        } C_5;
      struct
        {
        tsp_int2 p7reclen;
        tsp_int2 p7keylen;
        tsp_int2 p7varcol_offset;
        tsp_int2 p7varcol_cnt;
        tgg_surrogate p7surrogate;
        tsp_c2 p7entrytype;
        tsp_int2 p7segno;
        unsigned char p_tl_filler7[296];
        tgg00_HandlingSet p_handling;
        char p_fetch_object;
        char p7fill;
        } C_7;
      } integer;
    } tak_parsinforecord;
typedef
  unsigned char tak_stored_cmd_buf[8068];
typedef
  struct ptoc_Type160
    {
    tsp_int2 scr_reclen;
    tsp_int2 scr_keylen;
    tsp_int2 scr_varcol_offset;
    tsp_int2 scr_varcol_cnt;
    tgg_surrogate scr_surrogate;
    tsp_c2 scr_entrytype;
    tsp_int2 scr_segno;
    tsp_int4 scr_fullen;
    tsp_int2 scr_movelen;
    unsigned char scr_ddl;
    tsp_sqlmode scr_sql_mode;
    tak_stored_cmd_buf scr_buf;
    } tak_stored_cmd_record;
typedef
  struct ptoc_Type161
    {
    tak_parskey uparsid;
    tsp_int2 ufirstsel;
    tsp_int2 usecondsel;
    tsp_int2 ufiller;
    unsigned char uis_kind;
    char uis_all;
    } tak_union_array[493];
typedef
  struct ptoc_Type162
    {
    tsp_int2 usyskreclen;
    tsp_int2 usyskkeylen;
    tsp_int2 usyskvarcol_offset;
    tsp_int2 usyskvarcol_cnt;
    tgg_surrogate usysksurrogate;
    tsp_c2 usyskentrytype;
    tsp_int2 usysksegno;
    tsp_int2 uselect_cnt;
    tsp_int2 ucolpos;
    tsp_int4 ures_cnt;
    tgg00_FileId ures_tree;
    tsp_knl_identifier ures_name;
    tsp_knl_identifier umodul_name;
    tsp_int2 ukeylen;
    tsp_int2 urowno;
    tsp_int2 usubcount;
    tsp_int2 ucntpar;
    char uinsertselect;
    char uappend_all;
    tsp_int2 ufiller;
    tak_union_array uselect_arr;
    } tak_unionrecord;
typedef
  unsigned char tak_complex_infoset[1];
typedef
  struct ptoc_Type163
    {
    tak_parskey cparsk;
    tsp_int2 cnextpars;
    tak_complex_infoset centryinfo;
    unsigned char cqual_jv_upd;
    } tak_complex_array[669];
typedef
  struct ptoc_Type164
    {
    tsp_int2 compreclen;
    tsp_int2 compkeylen;
    tsp_int2 compvarcol_offset;
    tsp_int2 compvarcol_cnt;
    tgg_surrogate compsurrogate;
    tsp_c2 compentrytype;
    tsp_int2 compsegno;
    tsp_int2 compcnt;
    tsp_int2 compmax_sel_cnt;
    tgg00_FileId comptree;
    tak_complex_array comparr;
    } tak_complexrecord;
typedef
  struct ptoc_Type165
    {
    tsp_int2 mbr_reclen;
    tsp_int2 mbr_keylen;
    tsp_int2 mbr_varcol_offset;
    tsp_int2 mbr_varcol_cnt;
    tgg_surrogate mbr_surrogate;
    tsp_c2 mbr_entrytype;
    tsp_c2 mbr_linkage;
    tsp_c2 mbr_segment_id;
    tsp_int4 mbr_stack_len;
    tsp_c2 mbr_filler1;
    tgg_mess_block mbr_mess_block;
    } tak_messblock_record;
typedef
  struct ptoc_Type166
    {
    tsp_int2 cfieldno;
    tsp_int2 cnode;
    tak_corlevel clevel;
    tsp_int2 csetnode;
    tsp00_Uint1 ccntcol;
    tsp00_Uint1 ctabno;
    unsigned char ccmd_part;
    tsp00_Uint1 cfiller1;
    tsp_int2 cfiller2;
    } tak_all_corcolumns[64];
typedef
  struct ptoc_Type167
    {
    tsp_int2 tc_reclen;
    tsp_int2 tc_keylen;
    tsp_int2 tc_varcol_offset;
    tsp_int2 tc_varcol_cnt;
    tgg_surrogate tc_surrogate;
    tsp_c2 tc_entrytype;
    tsp_int2 tc_segno;
    tsp_int2 tc_no_of_tables;
    tsp_int2 tc_no_of_columns;
    tsp_int2 tc_starttable;
    tsp_int2 tc_oldstarttable;
    struct ptoc_Type168
      {
      tsp00_Uint1 socoldtabno;
      tsp00_Uint1 socselecttabno;
      tsp00_Uint1 socfields;
      tsp00_Uint1 socfill;
      } tc_tables[16];
    tak_all_corcolumns tc_columns;
    } tak_correcord;
typedef
  struct ptoc_Type169
    {
    tsp_int2 lpos;
    tsp_int2 llen;
    } tak_linkposinfo[2048];
typedef
  struct ptoc_Type170
    {
    tsp_int2 lreclen;
    tsp_int2 lvarcol_offset;
    tsp_int2 lvarcol_cnt;
    tsp_int2 lkeylen;
    tgg_surrogate lsurrogate;
    tsp_c2 lentrytype;
    tsp_int2 lseqno;
    tsp_int4 lfullen;
    tgg_message_type lmess_type;
    char lprim_check;
    tsp_int2 lunique_cnt;
    tak_columnset lupd_col_info;
    tak_charset lnot_used;
    tak_linkposinfo lpos_info;
    } tak_linkinforecord;
typedef
  unsigned char tak_directory_state;
typedef
  unsigned char tak_sysdir_state[1];
typedef
  tsp_moveobj *tak_moveobj_ptr;
typedef
  struct tak_cache_dir_entry *tak_cache_dir_e_ptr;
typedef
  struct ptoc_Type171
    {
    union
      {
      struct
        {
        tak_cache_dir_e_ptr cdir_ptr;
        } C_1;
      struct
        {
        tsp_bufaddr buf_ptr;
        } C_2;
      struct
        {
        tak_moveobj_ptr moveobj_ptr;
        } C_3;
      struct
        {
        tak_sysbufferaddress sys_ptr;
        } C_4;
      struct
        {
        tsp_int4 i;
        } C_5;
      } integer;
    } tak_cache_dir_pointer;
typedef
  struct ptoc_Type172
    {
    tak_cache_dir_pointer ci_cache_dir_header;
    tak_cache_dir_pointer ci_cache_dir_trailor;
    tsp_bufaddr ci_cache_p;
    tsp_int4 ci_tmp_read_cnt;
    tsp_int4 ci_displaceCnt;
    char ci_tmp_dislodged;
    char ci_filler1;
    tsp_int2 ci_filler2;
    tsp_int4 ci_tmp_read_found;
    tgg00_FileId ci_cmd_rollback_tree;
    tak_cache_dir_pointer ci_dir[195];
    } tak_sysdirectory;
typedef
  struct ptoc_Type173
    {
    tak_sysbufferaddress px[8];
    tak_sysbufferaddress presnp;
    tak_sysbufferaddress pinfop;
    tak_sysbufferaddress pparsp;
    tak_sysbufferaddress pbasep;
    tak_sysbufferaddress pcolnamep;
    tgg_surrogate psynid;
    char psynfound;
    char psystable;
    tsp_int2 pcount;
    } tak_syspointerarr;
typedef
  struct ptoc_Type174
    {
    tsp_dbname ds_dbname;
    tgg_serverdb_no ds_site;
    char ds_hard_drop;
    char ds_filler1;
    tsp_int2 ds_filler2;
    tsp_nodeid ds_node_id;
    tgg00_FileId ds_local_user_file;
    tgg00_FileId ds_remote_user_file;
    tgg00_FileId ds_remote_local_tables;
    tgg00_FileId ds_dropped_tables;
    tgg00_FileId ds_views;
    tgg00_FileId ds_analyzed_views;
    tgg00_FileId ds_global_tables;
    } tak_drop_serverdb_record;
typedef
  tak_drop_serverdb_record *tak_drop_db_ptr;
typedef
  tgg00_TransState tak_site_states[8][256];
typedef
  tak_site_states *tak_site_state_ptr;
typedef
  struct ptoc_Type175
    {
    tgg00_TransContext tri_trans;
    tgg00_HandlingSet tri_global_state;
    tsp00_Uint1 tri_fill1;
    unsigned char tri_replicated;
    tsp_c6 tri_obj_seq;
    tsp_int2 tri_fill2;
    } tak_transinfo;
typedef
  unsigned char tak_rollback_info[1];
typedef
  struct ptoc_Type176
    {
    tsp_knl_identifier dbp_sysdba;
    tgg00_FileId dbp_tree;
    } tak_dbproc_array[5];
typedef
  struct ptoc_Type177
    {
    tsp_int2 dbc_cnt;
    tsp_int2 dbc_fil;
    tak_sysbufferaddress dbc_ptr;
    tak_dbproc_array dbc_cache;
    } tak_dbproc_cache;
typedef
  struct ptoc_Type178
    {
    tak_sysbufferaddress isr_buf;
    int isr_index;
    tgg_sysinfokey isr_sysk;
    } tak_index_scan_record;
typedef
  struct ptoc_Type179
    {
    char o_do_optimize;
    unsigned char o_query_type;
    tsp_int2 o_mess_type;
    unsigned char o_ex_kind;
    char o_optimized;
    tsp_int2 o_filler1;
    tgg00_FileId o_cmd_tree;
    tsp_c8 o_filler3;
    } tak_optimize_info;
typedef
  struct ptoc_Type180
    {
    tsp_c4 currency;
    tsp_c50 date_format;
    tsp_c3 date_language;
    tsp_c2 numeric_chars;
    tsp_int2 df_length;
    char dl_found;
    char dl_is_german;
    tsp00_Uint1 dl_language_no;
    char nls_binary;
    tsp_knl_identifier nls_sort;
    } tak_nls_param_rec;
typedef
  struct tak_size_info *tak_size_ptr;
struct tak_size_info
  {
  tak_size_ptr csz_next;
  tgg91_SessionNo csz_session;
  tsp_int4 csz_ak_cache;
  char csz_fill1;
  tsp00_Uint1 csz_tracelevel;
  tsp_int2 csz_fill2;
  } ;
typedef
  struct ptoc_Type181
    {
    tsp_int4 cst_top;
    tsp_moveobj_ptr cst_stack[16];
    } tak_cmd_stack;
typedef
  struct ptoc_Type182
    {
    union
      {
      struct
        {
        tsp_longreal d;
        } C_true;
      struct
        {
        tsp_c64 c;
        } C_false;
      } boolean;
    } tak_kernel_sink;
typedef
  struct ptoc_Type183
    {
    tsp00_8ByteCounter ami_MallocMin;
    tsp00_8ByteCounter ami_MallocMax;
    tsp00_8ByteCounter ami_MallocSum;
    tsp00_8ByteCounter ami_MallocCnt;
    tsp00_8ByteCounter ami_FreeMin;
    tsp00_8ByteCounter ami_FreeMax;
    tsp00_8ByteCounter ami_FreeSum;
    tsp00_8ByteCounter ami_FreeCnt;
    tsp00_8ByteCounter ami_Allocated;
    } tak_AllocatorMonitorInfo;
typedef
  struct ptoc_Type184
    {
    tsp00_Int4 ai_uktId;
    tsp00_Int4 ai_badAllocCountDown;
    tak_AllocatorMonitorInfo ai_alloc_sys;
    tak_AllocatorMonitorInfo ai_alloc_usr;
    } tak_allocator_info;
typedef
  struct ptoc_Type185
    {
    tsp1_packet_ptr a_in_packet;
    tsp1_packet_ptr a_out_packet;
    tsp1_segment_ptr a_cmd_segm;
    tsp1_part_ptr a_cmd_part;
    tsp1_part_ptr a_data_part;
    tsp1_part_ptr a_abap_part;
    tsp1_part_ptr a_rescount_part;
    tsp1_part_ptr a_resname_part;
    tsp_moveobj_ptr a_data_ptr;
    tsp1_segment_ptr a_return_segm;
    tsp1_part_ptr a_curr_retpart;
    tak_ap_max_tree_ptr a_ap_tree;
    tak_drop_db_ptr a_drop_serverdb_ptr;
    tsp1_packet_ptr a_dbp_packet_ptr;
    tsp_int4 a_input_data_pos;
    tsp_int4 a_output_data_pos;
    tgg_datapart_ptr a_mb_data_addr;
    tsp00_MoveObjPtr a_mb_strat_addr;
    tgg00_HandlingSet a_cmd_globstate;
    tsp00_Uint1 a_code_to_compare;
    char a_info_output;
    unsigned char a_ex_kind;
    tsp00_Uint1 a_precomp_info_byte;
    tsp_int2 a_scv_index;
    tak_scanner_glob a_scv;
    char a_in_internal_subtr;
    unsigned char a_init_ddl;
    tsp_int2 a_count_variab;
    unsigned char a_command_kind;
    unsigned char a_is_ddl;
    char a_stored_proc_call;
    char a_wait_for_checkpoint;
    char a_outer_join;
    char a_intern_explain;
    char a_pars_explain;
    unsigned char a_internal_sql;
    char a_from_select;
    char a_user_defined_error;
    unsigned char a_allow_functions;
    char a_rowno_allowed;
    char a_rowno_found;
    char a_special_expr;
    char a_oneval_subq_allowed;
    char a_column_factor;
    char a_join_predicate;
    char a_try_again;
    tsp_int2 a_select_node;
    tsp_int4 a_curr_res_id;
    tgg_surrogate a_complex_key;
    tsp_int2 a_max_intern_select;
    tsp_int2 a_intern_select_cnt;
    tgg_surrogate a_shortinfo_key;
    tsp_int4 a_union_cnt;
    tak_parskey a_union_key;
    tsp00_Uint1 a_union_kind;
    char a_shared_result;
    char a_no_local_call;
    char a_costcheck;
    tsp_int4 a_costsum;
    unsigned char a_isolation_info;
    char a_corr_select;
    tgg_surrogate a_error_tableid;
    tsp_int4 a_errpos_offset;
    tsp_int2 a_parameter_offset;
    unsigned char a_qualified_jv_upd;
    char a_rest_equal_second;
    char a_part_rollback;
    tak_rollback_info a_rollback_info;
    tsp_timestamp a_timestamp;
    tsp_int4 a_show_last_key;
    unsigned char a_first_parskey;
    unsigned char a_first_parsid;
    tsp_int2 a_leftpar_cnt;
    tsp_int2 a_rightpar_cnt;
    tgg_surrogate a_tempinfo_key;
    tsp_knl_identifier a_viewname;
    tsp_int2 a_fromsel_n;
    char a_may_be_direct_sel;
    char a_date_time_used;
    char a_compact_varpart;
    tsp00_Uint1 a_max_codewidth;
    tsp00_Uint1 a_session_max_codewidth;
    tgg_basis_error a_ak_b_err;
    tsp_knl_identifier a_result_name;
    tsp_code_type a_mess_code;
    tsp_code_type a_basic_code;
    tsp_code_type a_session_mess_code;
    tak_parskey a_corr_key;
    tak_parskey a_curr_ex_parskey;
    unsigned char a_init_ex_kind;
    char a_in_ddl_trigger;
    tsp00_Uint1 a_iso_level;
    char a_for_fetch_only;
    char a_in_select_list;
    char a_in_installation;
    char a_parsing_again;
    unsigned char a_statement_kind;
    tsp_knl_identifier a_modul_name;
    tak_param_list a_parameter_spec;
    tsp_int4 a_mb_data_size;
    tsp_int4 a_mb_strat_size;
    tsp_int2 a_ap_max_node;
    tsp_bufaddr a_visual_basic_area;
    char a_dynamic_sql;
    unsigned char a_comp_type;
    char a_explain_view;
    unsigned char a_explain_kind;
    tsp_c5 a_comp_vers;
    tsp_c3 a_degree;
    unsigned char a_cpart_type;
    char a_retpart_locked;
    tsp_int2 a_no_of_likes;
    tsp_int4 a_maxnode;
    tsp_int4 a_data_length;
    tsp_resnum a_resultnum;
    char a_trunc_identifier;
    tgg_qual_buf_ptr a_mb_qual_addr;
    tsp_int4 a_mb_qual_size;
    tsp_int4 a_stack_size;
    tgg_stack_list_ptr a_work_st_addr;
    tsp_moveobj_ptr a_work_buf_addr;
    tsp_int4 a_work_buf_size;
    tsp_int2 a_work_st_max;
    unsigned char a_not_swap_f3[10];
    tgg_mess_block a_mblock;
    char a_restarted;
    char a_is_connected;
    char a_in_session;
    tgg_surrogate a_init_user_id;
    tsp_knl_identifier a_connected_name;
    tsp_knl_identifier a_curr_user_name;
    tgg_surrogate a_curr_user_id;
    unsigned char a_current_user_kind;
    char a_is_exclusive;
    tgg00_UserRef a_user_perm_ref;
    tgg_surrogate a_acc_user_id;
    tsp_knl_identifier a_acc_user;
    tsp_knl_identifier a_acc_groupname;
    tgg_surrogate a_acc_dba_id;
    tsp_knl_identifier a_acc_dbaname;
    tsp_int4 a_command_count;
    tsp_int4 a_costwarn_value;
    tsp_int4 a_costlimit_value;
    tsp_int2 a_user_timeout;
    char a_use_timeout;
    char a_check_lock;
    tsp_c3 a_ak_language;
    char a_errormsg_loading;
    tgg00_FileId a_trigger_tree;
    tsp_bufaddr a_dialog_area;
    tsp_int4 a_sample_pages;
    tak_optimize_info a_optimize_info;
    tsp_knl_identifier a_proc_user;
    char a_progusage_add;
    char a_proc_compile;
    tgg_surrogate a_proc_id;
    tsp_int4 a_show_data_cnt;
    tsp_kernel_dca a_dca;
    tak_site_state_ptr a_states;
    tgg_surrogate a_proc_userid;
    tak_sysbufferaddress a_resname_addr[3];
    tsp_int4 a_union_res_id;
    char a_first_union;
    tsp_knl_identifier a_expl_resname;
    char a_sub_found;
    char a_send_messbuf;
    char a_union_append;
    tsp_int2 a_next_upos;
    tak_dbproc_cache a_dbproc_cache;
    tsp_int4 a_max_res_id;
    tgg00_FileId a_show_curr;
    tgg00_FileId a_pars_curr;
    tgg00_FileId a_pars_file;
    tgg00_FileId a_usage_curr;
    tgg00_FileId a_into_tree;
    tgg00_FileId a_intern_tree;
    tgg00_FileId a_intern_res_tree;
    tgg00_FileId a_intern_cmd_tree;
    tak_sysdirectory a_sysdir;
    tak_parskey a_pars_last_key;
    unsigned char a_dt_format;
    char a_fk_check;
    char a_fetch_rest;
    char a_insert_select;
    tak_transinfo a_transinf;
    char a_named_subtrans;
    char a_link_pending;
    tsp_int2 a_rqscrr;
    tsp_int2 a_long_desc_pos;
    tsp_int2 a_result_cnt;
    tsp_knl_identifier a_proc_appl;
    tsp_knl_identifier a_proc_routine;
    tgg_surrogate a_temp_tablekey;
    tsp_int2 a_constraint_cnt;
    tsp_int2 a_fk_check_cnt;
    char a_execute_trigger;
    tsp00_Uint1 a_trigger_level;
    char a_trigger_trace;
    tsp_sqlmode a_sqlmode;
    tsp_c4 a_trigger_trace_key;
    tsp_int2 a_restore_with_string;
    tsp_int2 a_outer_n;
    tsp_sqlmode a_session_sqlmode;
    tsp00_Uint1 a_dbproc_level;
    tsp_termid a_acc_termid;
    tsp_int_map_c2 a_col_file_count;
    tsp_c18 a_collection_id;
    tak_order_errortext a_row_not_found_text;
    tsp_int2 a_len_row_not_found_text;
    tsp_int2 a_col_func_count;
    tak_size_ptr a_size_ptr;
    tgg_surrogate a_func_id;
    tgg_stack_list_ptr a_stack_addr;
    char a_unused_n;
    char a_is_term_driver;
    char a_sys_dbproc;
    char a_new_interpreter;
    tak_syspointerarr a_p_arr1;
    tak_syspointerarr a_p_arr2;
    tak_syspointerarr a_p_arr3;
    tak_syspointerarr a_p_arr4;
    tak_syspointerarr a_p_arr5;
    tak_sysbufferaddress a_ptr1;
    tak_sysbufferaddress a_ptr2;
    tak_sysbufferaddress a_ptr3;
    tak_sysbufferaddress a_ptr4;
    tak_sysbufferaddress a_ptr5;
    tak_sysbufferaddress a_ptr6;
    tak_sysbufferaddress a_ptr7;
    tak_sysbufferaddress a_ptr8;
    tak_sysbufferaddress a_ptr9;
    tak_sysbufferaddress a_ptr10;
    tak_cmd_stack a_cmd_stack;
    tak_sysbufferaddress a_drda_info_ptr;
    tsp1_packet_ptr a_old_packet_p;
    tak_sysbufferaddress a_error_key_ptr;
    tak_sysbufferaddress a_unionrec_ptr;
    tak_nls_param_rec a_nls_params;
    tak_sysbufferaddress a_opt_info_ptr;
    tsp_int2 a_err_parm_cnt;
    tsp_c80 a_error_params;
    tsp1_part_ptr a_uni_cmd_part;
    tak_role_context_ptr a_role_ptr;
    unsigned char a_role_info;
    tsp_int2 a_recursive_no;
    unsigned char a_recursive_state;
    char a_outstanding_packet;
    tsp_int4 a_no_of_com_objects;
    tak_kernel_sink a_kernel_sink;
    tsp_int4 a_mass_all_rescount;
    char a_subq_datatype_problem;
    char a_release_instances;
    char a_init_info_output;
    char a_abap_tab_comm_ok;
    tsp_int2 a_first_hint_node;
    tak_pcountarr a_free_pcounts;
    tsp_int2 a_first_free_pcount;
    tsp_int2 a_last_free_pcount;
    tsp_c3 a_max_used_pcount;
    char a_isInCppDBProcedure;
    tak_allocator_info a_allocatorInfo;
    tgg_serverdb_no a_current_auth_site;
    } tak_all_command_glob;
typedef
  tak_all_command_glob *tak_acv_address;
typedef
  struct ptoc_Type186
    {
    tgg_surrogate llclongid;
    tsp_int4 llcdatapos;
    tsp_int4 llcdatalen;
    char llwith_shortcol;
    char llfiller1;
    tsp_int2 llfiller2;
    tsp_int4 llfiller3;
    } tak_litcoldesc_arr[335];
typedef
  unsigned char tak_longlit_buf[8048];
typedef
  struct ptoc_Type187
    {
    tsp_int2 litreclen;
    tsp_int2 litkeylen;
    tsp_int2 litvarcol_offset;
    tsp_int2 litvarcol_cnt;
    tgg_surrogate litsurrogate;
    tsp_c2 litentrytype;
    tsp_c2 litlinkage;
    tgg_surrogate littabid;
    tsp_int2 litcolcount;
    tsp_int2 litfiller1;
    tsp_int4 litlen;
    tsp_int4 litsize;
    tsp_moveobj_ptr litdataptr;
    union
      {
      struct
        {
        tak_litcoldesc_arr litcoldesc;
        } C_true;
      struct
        {
        tak_longlit_buf litdatabuf;
        } C_false;
      } boolean;
    } tak_longliteral_rec;
struct tak_systembuffer
  {
  union
    {
    struct
      {
      tsp_int2 b_sl;
      tsp_int2 b_kl;
      tsp_int2 b_varcol_offset;
      tsp_int2 b_varcol_cnt;
      } C_0;
    struct
      {
      tak_baserecord sbase;
      } C_1;
    struct
      {
      tak_comment_record scomment;
      } C_2;
    struct
      {
      tak_complexrecord scomplexrec;
      } C_3;
    struct
      {
      tak_constraintrecord sconstraint;
      } C_4;
    struct
      {
      tak_constraintnamerecord sconstrname;
      } C_5;
    struct
      {
      tak_correcord scorr;
      } C_6;
    struct
      {
      tak_defaultrecord sdefault;
      } C_8;
    struct
      {
      tak_domainrecord sdomain;
      } C_9;
    struct
      {
      tak_domainref sdomainref;
      } C_10;
    struct
      {
      tak_linkrecord slink;
      } C_14;
    struct
      {
      tak_link_def_record slinkdefault;
      } C_15;
    struct
      {
      tak_linkparsrecord slinkparsinfo;
      } C_16;
    struct
      {
      tak_linkinforecord slinkposinfo;
      } C_17;
    struct
      {
      tak_serverdbrecord sserverdb;
      } C_18;
    struct
      {
      tak_serverdbnamerecord sserverdbname;
      } C_19;
    struct
      {
      tak_longinforecord slonginfo;
      } C_21;
    struct
      {
      tak_mindexrecord smindex;
      } C_22;
    struct
      {
      tak_parsinforecord sparsinfo;
      } C_23;
    struct
      {
      tak_privrecord spriv;
      } C_24;
    struct
      {
      tak_privuserrecord sprivuser;
      } C_25;
    struct
      {
      tak_resultkeyrecord sreskey;
      } C_26;
    struct
      {
      tak_resultrecord sresult;
      } C_28;
    struct
      {
      tak_scoldescrecord sscoldesc;
      } C_29;
    struct
      {
      tak_sequencerecord ssequence;
      } C_30;
    struct
      {
      tak_shortinforecord sshortinfo;
      } C_31;
    struct
      {
      tak_subtransrecord ssubtrans;
      } C_33;
    struct
      {
      tak_synonymrecord ssynonym;
      } C_34;
    struct
      {
      tak_sysdbarecord ssysdba;
      } C_35;
    struct
      {
      tak_tablereference stableref;
      } C_36;
    struct
      {
      tgg00_TermsetRecord stermset;
      } C_37;
    struct
      {
      tak_triggerrecord strigger;
      } C_38;
    struct
      {
      tak_unionrecord sunionrec;
      } C_40;
    struct
      {
      tak_usagerecord susage;
      } C_41;
    struct
      {
      tak_userrecord suser;
      } C_42;
    struct
      {
      tak_useridrecord suserid;
      } C_43;
    struct
      {
      tak_userrefrecord suserref;
      } C_44;
    struct
      {
      tak_usertablerecord susertab;
      } C_45;
    struct
      {
      tak_viewdescrecord sviewdesc;
      } C_46;
    struct
      {
      tak_viewkeyrecord sviewkey;
      } C_47;
    struct
      {
      tak_viewtextrecord sviewtext;
      } C_49;
    struct
      {
      tgg_sysinfokey syskey;
      } C_50;
    struct
      {
      unsigned char ssysbuf[32760];
      } C_51;
    struct
      {
      tak_resname_record sresname;
      } C_52;
    struct
      {
      tak_mapset_record smapset;
      } C_53;
    struct
      {
      tak_funcrefrecord sfuncref;
      } C_54;
    struct
      {
      tak_proc_seq_privilege sprocseqpriv;
      } C_55;
    struct
      {
      tak_col_uses_dom scol_uses_dom;
      } C_56;
    struct
      {
      tak_alterdaterecord salterdate;
      } C_57;
    struct
      {
      tak_columnnamesrecord scolnames;
      } C_59;
    struct
      {
      tak_stored_cmd_record sstored_cmd;
      } C_60;
    struct
      {
      tak_changeinfos schangeinfos;
      } C_61;
    struct
      {
      tak_messblock_record smessblock;
      } C_62;
    struct
      {
      tak_statisticsinfo sstatistics;
      } C_63;
    struct
      {
      tak_rolerecord srole;
      } C_64;
    struct
      {
      tak_longliteral_rec slongliteral;
      } C_65;
    struct
      {
      tak_methodrecord smethod;
      } C_66;
    struct
      {
      tak_methodrefrecord smethodref;
      } C_67;
    struct
      {
      tak_type_uid_map_record stypeuidmap;
      } C_68;
    struct
      {
      tak_method_inforecord smethodinfo;
      } C_69;
    struct
      {
      tak_class_map_guid_record sclassmap;
      } C_70;
    struct
      {
      tak_class_record sclass;
      } C_71;
    struct
      {
      tak_version_record sversion;
      } C_72;
    struct
      {
      tak_commandinforecord scommand_info;
      } C_73;
    struct
      {
      tak_viewqual_basis_record sviewqual_basis;
      } C_74;
    struct
      {
      tak_viewqual_join_record sviewqual_join;
      } C_75;
    struct
      {
      tak_viewqual_stack_record sviewqual_stack;
      } C_76;
    struct
      {
      tak_viewqual_derivedcol_record sviewqual_derivedcol;
      } C_77;
    struct
      {
      tak_schema_record sschema;
      } C_78;
    struct
      {
      tak_schema_ref_record sschemaref;
      } C_79;
    } integer;
  } ;
struct tak_cache_dir_entry
  {
  union
    {
    struct
      {
      tsp_int4 cd_pos;
      tsp_int4 cd_check;
      tak_cache_dir_pointer cd_less;
      tak_cache_dir_pointer cd_greater;
      tak_cache_dir_pointer cd_fix_prev;
      tak_cache_dir_pointer cd_fix_next;
      tak_cache_dir_pointer cd_lru_prev;
      tak_cache_dir_pointer cd_lru_next;
      tsp_int4 cd_cmd_no;
      tsp_int4 cd_create_cmd_no;
      tak_sysdir_state cd_state;
      unsigned char cd_balance;
      tsp_int2 cd_buf_no;
      tsp_int4 cd_syslen;
      tgg_sysinfokey cd_syskey;
      } C_1;
    struct
      {
      tsp_int4 cd_block_pos;
      tsp_int4 cd_block_check;
      tak_cache_dir_pointer cd_bl_less;
      tak_cache_dir_pointer cd_next_free;
      tak_cache_dir_pointer cd_bl_fix_prev;
      tak_cache_dir_pointer cd_bl_fix_next;
      tak_cache_dir_pointer cd_bl_lru_prev;
      tak_cache_dir_pointer cd_bl_lru_next;
      tsp_int4 cd_free_fill;
      tsp_int4 cd_block_len;
      tak_sysdir_state cd_bl_state;
      unsigned char cd_mem_type;
      tsp_int2 cd_bl_buf_no;
      tsp_int4 cd_bl_syslen;
      struct tak_systembuffer cd_sysinfo;
      } C_2;
    } integer;
  } ;
typedef
  struct ptoc_Type188
    {
    union
      {
      struct
        {
        tsp_c1 i1;
        } C_1;
      struct
        {
        tsp_c2 c2;
        } C_2;
      struct
        {
        tsp_int2 i2;
        } C_3;
      struct
        {
        tsp_int4 i4;
        } C_4;
      struct
        {
        tsp_shortreal r4;
        } C_5;
      struct
        {
        tsp_longreal r8;
        } C_6;
      struct
        {
        tsp_moveobj_ptr ptr;
        } C_7;
      } integer;
    } tak_dbproc_stack_param;
typedef
  tsp00_VFilename tsp2_devname;
typedef
  unsigned char tsp2_dev_type;
typedef
  short tgg07_ColPosArr[20];
typedef
  tsp00_C1 tgg07_StrRaw[236];
typedef
  struct ptoc_Type189
    {
    tgg07_ColPosArr skir_keystart;
    tgg07_ColPosArr skir_keystop;
    tsp00_Int2 skir_IN_SUBQ_stpos;
    tsp00_Int2 skir_inelemcnt;
    } tgg07_StrKeyInRange;
typedef
  unsigned char tgg07_InvColSet[2];
typedef
  struct ptoc_Type190
    {
    tgg07_ColPosArr siir_keystart;
    tgg07_ColPosArr siir_keystop;
    tgg07_ColPosArr siir_invstart;
    tgg07_ColPosArr siir_invstop;
    tsp00_Int2 siir_startcnt;
    tsp00_Int2 siir_stopcnt;
    tsp00_PageNo siir_invroot;
    tsp00_Int2 siir_indexno;
    tsp00_Int2 siir_icount;
    tgg07_InvColSet siir_invcoldesc;
    tsp00_Int2 siir_invlen;
    tsp00_Int2 siir_inelemcnt;
    tsp00_Int2 siir_subq_idx;
    tsp00_Int2 siir_out_invseqlen;
    tsp00_Int2 siir_IN_SUBQ_stpos;
    unsigned char siir_strat_props[1];
    tsp_c3 siir_filler;
    } tgg07_StrInvInRange;
typedef
  struct ptoc_Type191
    {
    tgg07_ColPosArr sji_keystart;
    tgg07_ColPosArr sji_keystop;
    tsp00_PageNo sji_invroot;
    tsp00_Int2 sji_invindex;
    tsp00_Int2 sji_jfiller;
    } tgg07_StrJoinInv;
typedef
  struct ptoc_Type192
    {
    tgg07_ColPosArr sjmf_keystart;
    tgg07_ColPosArr sjmf_keystop;
    tsp00_PageNo sjmf_invroot;
    tsp00_Int2 sjmf_index_no;
    tsp00_Int2 sjmf_cntfields;
    } tgg07_StrJoinMultFields;
typedef
  struct ptoc_Type193
    {
    tsp00_Int2 sjv_lastkeylen;
    tsp00_Int2 sjv_minkeylen;
    } tgg07_StrJoinViewkey;
typedef
  struct ptoc_Type194
    {
    tgg00_ShowKind scat_show_kind;
    char scat_upd_strat;
    tsp00_Int2 scat_authname;
    tsp00_Int2 scat_tablename;
    tsp00_Int2 scat_username;
    tsp00_Int2 scat_columnname;
    tsp00_Int2 scat_fill2;
    char scat_col_needed[80];
    char scat_qual_cols[80];
    tsp00_KnlIdentifier scat_dblink;
    } tgg07_StrCatalog;
typedef
  struct ptoc_Type195
    {
    tgg00_FileId str_result_id;
    unsigned char str_strategy;
    char str_build_result;
    unsigned char str_distinc;
    char str_ordering;
    char str_use_rowno;
    unsigned char str_use_subquery;
    char str_corr_single;
    tsp00_Uint1 str_union_kind;
    char str_res_possible;
    char str_use_order;
    tsp00_Int2 str_out_keyseqlen;
    unsigned char str_qual_kind;
    tsp00_Uint1 str_cnt_strat;
    tsp00_Int2 str_maxresults;
    tsp00_Int4 str_foundresults;
    tsp00_Int2 str_key_len;
    tsp00_Int2 str_rec_len;
    tsp00_Int4 str_union_key_cnt_offs;
    tsp00_Int2 str_stack_output_offs;
    char str_search_first;
    char str_all_files;
    char str_key_order_support;
    tsp_c3 str_filler;
    union
      {
      struct
        {
        tgg07_StrKeyInRange str_key_in_range;
        } C_strat_key_equal;
      struct
        {
        tgg07_StrInvInRange str_inv_in_range;
        } C_strat_inv_range;
      struct
        {
        tgg07_StrJoinInv str_join_inv;
        } C_strat_join_inv;
      struct
        {
        tgg07_StrJoinMultFields str_join_multfields;
        } C_strat_join_all_keys_equal;
      struct
        {
        tgg07_StrJoinViewkey str_join_viewkey;
        } C_strat_join_viewkey;
      struct
        {
        tgg07_StrCatalog str_catalog;
        } C_strat_catalog;
      struct
        {
        tgg07_StrRaw str_raw;
        } C_strat_undecided;
      } tgg07_StratEnum;
    } tgg07_StrategyInfo;
typedef
  struct ptoc_Type196
    {
    char sv_one_result;
    char sv_trunc_part2;
    char sv_no_oj_destroy_f;
    char sv_file_to_drop;
    char sv_call_do_avg;
    char sv_call_later_output;
    char sv_finding_possible;
    tgg00_MessType sv_type;
    tgg00_MessType2 sv_type2;
    unsigned char sv_global_strat;
    tsp00_Int2 sc_filler;
    tsp00_Int2 sv_keylen;
    tsp00_Int2 sv_dis_file_cnt;
    tgg00_FileId sv_tree;
    } tgg07_K720selectVars;
typedef
  struct ptoc_Type197
    {
    tsp00_8ByteCounter lqiCountGroup_kb00;
    tsp00_8ByteCounter lqiCountInsert_kb00;
    tsp00_8ByteCounter lqiCountRequest_kb00;
    tsp00_8ByteCounter lqiCountWait_kb00;
    tsp00_8ByteCounter lqiAvgSumPagewaits_kb00;
    tsp00_Int4 lqiLogQueueSize_kb00;
    tsp00_Int4 lqiMaxQueueUsed_kb00;
    tsp00_Int4 lqiMaxWaitLogpage_kb00;
    tsp00_Int4 lqiFiller_kb00;
    } tkb00_LogQueueInfo;
typedef
  struct ptoc_Type198
    {
    tsp00_Int4 ldiPagesUsed_kb00;
    tsp00_Int4 ldiPagesNotSaved_kb00;
    tsp00_Int4 ldiPagesSinceBup_kb00;
    tsp00_Int4 ldiSaveptCount_kb00;
    } tkb00_LogDeviceInfo;
typedef
  unsigned char tkb05_RowId[56];
typedef
  unsigned char tkb05_TransLockMode;
typedef
  struct ptoc_Type199
    {
    tsp00_Int2 tshRecLen_kb05;
    tsp00_Int2 tshKeyLen_kb05;
    tsp00_Int2 tshVarcolOffset_kb05;
    tsp00_Int2 tshVarcolCnt_kb05;
    tsp00_Int4 tshKey_kb05;
    tsp00_TaskId tshTaskId_kb05;
    tgg91_SessionNo tshSession_kb05;
    tgg91_TransNo tshWriteTransId_kb05;
    tsp00_Int2 tsh_filler1_kb05;
    tgg00_SubtransNo tshSubtrans_kb05;
    tsp00_Int4 tsh_filler2_kb05;
    tsp00_Int4 tshStartDate_kb05;
    tsp00_Int4 tshStartTime_kb05;
    tgg91_TransNo tshSelTransId_kb05;
    tsp00_TermId tshTermId_kb05;
    tgg00_UserId tshUserId_kb05;
    tsp00_Int4 tshReqTimeout_kb05;
    tsp00_Int4 tshTimeWrite_kb05;
    tkb05_TransLockMode tshTrLockMode_kb05;
    tkb05_TransLockMode tshTrReqMode_kb05;
    tgg00_LockReqMode tshLockMode_kb05;
    tgg00_LockReqMode tshReqMode_kb05;
    tgg00_LockReqState tshLockState_kb05;
    tgg00_LockReqState tshReqState_kb05;
    tgg91_TransNo tshConsistView_kb05;
    } tkb05_TransShow;
typedef
  struct ptoc_Type200
    {
    tkb05_TransShow lshTransShow_kb05;
    tgg00_Surrogate lshTabId_kb05;
    tkb05_RowId lshRowId_kb05;
    tsp00_Int2 lshRowIdLen_kb05;
    char lshUndefRowId_kb05;
    char lshFiller1_kb05;
    } tkb05_LockShow;
typedef
  struct ptoc_Type201
    {
    tsp00_8ByteCounter infSqlCollisions_kb05;
    tsp00_8ByteCounter infOmsCollisions_kb05;
    tsp00_8ByteCounter infEscalations_kb05;
    tsp00_8ByteCounter infDeadlocks_kb05;
    tsp00_8ByteCounter infSqlReqTimeouts_kb05;
    tsp00_8ByteCounter infObjReqTimeouts_kb05;
    tsp00_8ByteCounter infRowEntryAmount_kb05;
    tsp00_8ByteCounter infTabEntryAmount_kb05;
    tsp00_8ByteCounter infKbCalls_kb05;
    tsp00_Int4 infLockTrans_kb05;
    tsp00_Int4 infReqTrans_kb05;
    tsp00_Int4 infEscalValue_kb05;
    tsp00_Int4 infObjLockTrans_kb05;
    tsp00_Int4 infMaxEntries_kb05;
    tsp00_Int4 infEntriesUsed_kb05;
    tsp00_Int4 infAvgUsed_kb05;
    tsp00_Int4 infMaxUsed_kb05;
    tsp00_Int4 infConsistViewCnt_kb05;
    tsp00_Int4 infOpenTransCnt_kb05;
    } tkb05_LockInfo;
typedef
  unsigned char tak40_sel_param[3520];
typedef
  unsigned char tak40_get_param[16524];
typedef
  tak40_sel_param *tak40_sel_param_ptr;
typedef
  tak40_get_param *tak40_get_param_ptr;
typedef
  unsigned char tak40_shinfo[5400];
typedef
  struct ptoc_Type202
    {
    tsp_int2 shsl;
    tsp_int2 shkl;
    tsp_int2 shvarcol_offset;
    tsp_int2 shvarcol_cnt;
    tsp_c4 shkey;
    tak40_shinfo shinfo;
    } tak40_show_record;
typedef
  struct ptoc_Type203
    {
    tak_sysbufferaddress a4sysbuf;
    tak00_colinfo_ptr a4col_ptr;
    tak_sysbufferaddress a4dombuf;
    tsp_int4 a4ti;
    tsp_int4 a4ci;
    tsp_int4 a4pos;
    tsp_int4 a4index;
    tsp_int4 a4id_len;
    tsp_c4 a4sh_key;
    tsp_int4 a4LongKey;
    tak_syspointerarr a4p_arr;
    tsp_knl_identifier a4authname;
    tsp_knl_identifier a4tablen;
    tsp_knl_identifier a4user;
    tsp_knl_identifier a4coln;
    tgg_sysinfokey a4sysk;
    tsp_knl_identifier a4showfile_name;
    unsigned char a4sh_kind;
    char a4priv_check;
    tgg_privilege_set a4priv;
    char a4is_synonym;
    char a4del_basetab;
    tsp_int2 a4tab_count;
    tgg00_FileId a4treeid;
    tgg00_FileId a4LongContainer;
    tak40_show_record a4sh_buf;
    tsp_int2 a4col_len;
    tsp_int2 a4tab_len;
    tsp_int4 a4error_pos;
    tsp_knl_identifier a4synname;
    char a4systab_select;
    char a4unique;
    char a4temp;
    char a4private;
    char a4nonprivate;
    char a4public;
    char a4syns;
    char a4all;
    char a4is_systable;
    char a4sys_tables;
    char a4use_base;
    char a4use_view;
    tsp_int2 a4usertab_pos;
    tsp_int2 a4usertab_cnt;
    tsp_int2 a4mfetch_pos;
    tsp_int2 a4mfetch_cnt;
    tgg00_FilePos a4tree_pos;
    tak_sysbufferaddress a4usertab_buf;
    tgg_sysinfokey a4next_key;
    tgg_basis_error a4b_err;
    tsp_knl_identifier a4next_owner;
    tsp_knl_identifier a4next_tablen;
    tgg_surrogate a4next_user;
    tgg_surrogate a4owner_id;
    tsp_buf a4data;
    tgg_mess_block a4mblock;
    tgg07_StrategyInfo a4strategy;
    tgg_select_param_ptr a4sel;
    tak40_sel_param_ptr a4sel_param;
    tak40_get_param_ptr a4get_param;
    tgg07_K720selectVars a4select_vars;
    tsp_int2 a4long_desc_len;
    tgg_datapart_ptr a4res_buf_ptr;
    tsp_code_type a4mess_code;
    tak40_show_record *a4row_ptr;
    tak_acv_address a4acv_ptr;
    char a4all_objects;
    char a4roles;
    tsp_int2 a4role_cnt;
    tsp00_C8 a4role_ids[8];
    } tak40_show_glob;
typedef
  tak40_show_glob *tak40_show_glob_ptr;
typedef
  struct ptoc_Type204
    {
    union
      {
      struct
        {
        tsp00_Addr align;
        } C_1;
      struct
        {
        tgg00_Rec rec;
        } C_2;
      struct
        {
        unsigned char offset[8];
        tsp00_C16 iid;
        tsp00_Int4 dispid;
        tsp00_Int4 filler;
        tgg00_COMMonitorInfo info;
        } C_3;
      } integer;
    } tak42_oms_monitor_rec;
typedef
  struct ptoc_Type205
    {
    union
      {
      struct
        {
        tgg00_Lkey lkey;
        } C_true;
      struct
        {
        unsigned char offset[8];
        tsp00_C16 iid;
        tsp00_Int4 dispid;
        } C_false;
      } boolean;
    } tak42_monitor_rec_key;
tgg_sysinfokey a01defaultkey;
char a01diag_monitor_on;
char a01sm_collect_data;
char a01diag_analyze_on;
char a01diag_ana_coll_data;
int a01char_size;
tsp_int4 a01sm_reads;
tsp_int4 a01sm_selectivity;
tsp_int4 a01sm_milliseconds;
tsp_knl_identifier a01controluser;
tsp_knl_identifier a01_i_current;
tsp_knl_identifier a01_i_domain;
tsp_knl_identifier a01_il_b_identifier;
tsp_knl_identifier a01_zero_res_name;
tgg00_FileId a36xparam_tree;
tsp00_C3 a40yes_no[2];
tgg00_FileId b01niltree_id;
tsp00_VFilename b01blankfilename;
tgg_code_globals g01code;
tgg_kernel_globals g01glob;
char g01stand_alone;
tsp00_Int4 g01autosave_distance;
tsp_int4 g01page_size;
tgg_tabid_globals g01tabid;
tgg00_TimeoutGlobals g01timeout;
char g01unicode;
tgg_vtrace_state g01vtrace;
char a42check_time;
tsp00_Int4 ak42refresh_cnt;
tsp00_Int4 ak42fast_refresh_cnt;
tsp00_C40 a42monitor_arr[177];
tsp_moveobj_ptr a42parseid_list;
int g02gettermsetdesc();
int bd91InfoGCReset();
int bd91InfoGCStats();
int bd91InfoContainerRoots();
int bd91InfoContainerChains();
int bd91InfoContainerKeys();
int gg06Cint6ToLine();
char gg06IsNilTrans();
int gg06SessionGetInt4();
int gg06SessionToLine();
int gg06TransToLine();
int g10mv();
int g10mv1();
int g10mv2();
int g10mv3();
int g10mv4();
int g10mv5();
int g10mv6();
int g10mv7();
int g10mv8();
int g17date_to_line();
int g17intdate_time();
int g17int4to_line();
int g17messtype_to_line();
int g17mess2type_to_line();
int g17pagetype_to_line();
int g17surrogate_to_line();
int g17time_to_line();
int g17trimint4_to_line();
char Kernel_DiagInfop();
char kb03IsBackupCheck();
char kb03IsLockCheck();
char kb03IsLockSupplyCheck();
char kb03IsLogCheck();
char kb03IsLogQueueCheck();
char kb03IsRegionCheck();
char kb03IsTransCheck();
int k38get_autosave_medianame();
char k38is_on_autosave();
int k51info_lock();
char k51is_closed_locklist();
int k51reset_statistics();
int kb560GetHistFileInfo();
int kb560GetLogDeviceSize();
int kb560GetStatistics();
char kb560IsLogDeviceEnabled();
int kb560ResetStatistics();
char kb560LogIsFull();
int k57statistics_reset();
int k57info_seq_cache();
int k57select_restartrec();
int k57select_log_info();
int k720init_ref_statistic();
int k720show_monitor();
int a11get_check_table();
char a21is_owner();
char a260BuildDebugStack();
int a260reset_statistics();
int a260statistics();
int a262reset_statistics();
int a262statistics();
int a24init_index_scan();
int a24finish_index_scan();
int a24fnd_indexno();
int a24find_indexname();
int a24get_indexname();
char a24next_named_index();
int a28sys_upd_statistics();
int ak341OmsHeapInfo();
char ak341GetMonitorInfo();
int ak341GetObjFileInfo();
int ak341MonitorHeapContent();
char ak341IsMonitorOn();
char ak341NextOmsLockObjInfo();
int ak341ResetMonitor();
int ak341StartStopMonitor();
int a36get_catalog_record();
int a36restart_time();
int a37event_state();
int a40get_catalog_table();
int a40move();
int a40move_const();
int a40move_counter();
int a40move_guid();
int a40move_i4();
int a40move_real();
int a40put_date_time();
int a40put_one_value();
int a40init_table_scan();
int a40ignore_down_error();
int a40table_column_to_shbuf();
int a40username_from_syskey();
char a40next_table();
int a51build_userkey();
int a544get_analyze_filter();
int a545sm_get_rowno();
char a722test_col_qual();
int a10BaseRecPersistentOffset();
int a10cache_used();
int a10get_sysinfo();
int a10next_sysinfo();
int a10_key_del();
int a06check_username();
int a06colname_retpart_move();
int a06determine_username();
int a06get_page_cnt();
int a06finish_curr_retpart();
int a06ignore_rsend_mess_buf();
int a06rsend_mess_buf();
int a06retpart_move();
int a06set_mtree();
int a06_systable_get();
char a061exist_columnname();
int a05_identifier_get();
int bd01DataCacheSize();
int bd01ConverterSize();
char b01connect_allowed();
char b01db_full();
int b01dbspace_statistic();
int b01devsize();
int b01empty_file();
int b01get_bad_index_cnt();
int b01get_data_cache_workload();
int b01get_devname();
int b01next_bad_dev();
int b01next_dev_info();
int b01next_fdir_entry();
int b01no_of_data_devs();
int b01no_of_log_devs();
int b01show_monitor();
int b01reset_monitor();
int b01destroy_file();
int b02next_record();
int b07cadd_record();
int b07cget_record();
int b07cnext_record();
int b07crepl_record();
int s10fil();
int s10fil1();
int s10fil2();
int s10mv1();
int s20or4b();
int s40gbyte();
int s41plint();
int s41p4int();
int g01cache_size();
int g01datapart_init();
char g01is_livecache();
int g01log_mode();
int g01maxcpu();
int g01maxlocks();
int g01maxobjgarbagecoll();
int g01maxuser();
int g01mblock_init();
int g01qual_init();
int g01stack_init();
int g01allocate_msg();
int g04build_temp_tree_id();
int g11kernel_version();
int g20unifill();
int vgetapplpid();
int vversion();
int vdcom_dbproc();
int vdcom_monitor_open();
int vdcom_monitor_next();
int vdcom_monitor_close();
int vallocat();
int vgetfirstuserpid();
char RTEMem_AWEAvailable();
int a07ak_system_error();
int a07_b_put_error();
int a07_nb_put_error();
short a07_return_code();
int t01int4();
int t01lidentifier();
int t01buf();
int cccprint();
int a42execute();
int a42get_tablename();
int a42get_username();
int a42_get_pagecount();
int a42_init_time_array();
int a42monitor_init();
int a42_start_semantic();
int ak42ctrl_config();
int ak42ctrl_devspaces();
int ak42ctrl_io_statistics();
int ak42ctrl_locks();
int ak42ctrl_log_statistics();
int ak42ctrl_caches_statistics();
int ak42out_monitor();
int ak42data_cache_statistics();
int ak42connected_users();
int ak42datacache_hits();
int ak42dbm_state();
int ak42HeapContent();
int ak42history_info();
int ak42monitor_object();
int ak42monitor_show();
int a42_monitor_init();
int ak42monitor_oms();
int ak42AggrMethodMonitorInfo();
int ak42MonitorOmsBuildUserResult();
int ak42ResetOmsMonitor();
int ak42init_running_cmd_list();
int a42put_running_cmds();
char a42getparseid();
int a42reset_running_cmds();
int ak42oms_memory();
int ak42OmsLockObjInfo();
int ak42show_running_cmds();
int ak42add_parseid();
int ak42consistent_view();
int ak42roots();
int ak42files();
int ak42InfoGCStats();
int ak42_container_roots();
int ak42_container_chains();
int ak42_container_keys();
int ak42select_lock_trans();
int ak42one_lock_info();
int ak42appl_info();
int ak42lock_mode();
int ak42lock_state();
int ak42lock_req_mode();
int ak42lock_req_state();
int ak42serverdb_statistics();
int ak42state_statistics();
int ak42ref_i4_statistic();
int ak42ref_i4_stat_undef();
int ak42ref_stat_real();
int ak42prefix();
int ak42monitor_type();
int ak42devspace_statistics();
int ak42evalpercent();
int ak42evalpercent_cnt();
int ak42evalpercent_real();
int ak42datab_statistic();
int ak42datab_stat_undef();
int ak42log_statistics();
int ak42user_statistics();
int ak42all_user_st_show();
int ak42one_st_user_show();
int ak42show_config();
int ak42hex();
int ak42show_version();
int a42iindex_statistics();
int a42table_statistics();
int ak42char2hex();
int ak42one_index_statistics();
int ak42table_statistics();
int ak42print_stat_tab_index();
int a42_check_database();
int ak42check_db_shortinfos();
int ak42snullname_statistic();
int ak42sname_statistic();
int a42name_and_val_statistic();
int ak42hostname_statistic();
int ak42get_refinfo();
int ak42get_tablename();
int a42inc_refresh_cnt();
int a42index_inf_to_messbuf();
int ak42session_to_sname();
int ak42xparams();
int ak42ascii_const_move();
int ak42move_trans_no();
int ak42internal_state();
int a42bool_to_intern_state();
int a42cint6_to_intern_state();
int a42counter_to_intern_state();
int a42date_to_intern_state();
int a42int4_to_intern_state();
int a42logpos_to_intern_state();
int a42mess_to_intern_state();
int a42mess2_to_intern_state();
int a42session_to_intern_state();
int a42str_to_intern_state();
int a42surrogate_to_intern_state();
int a42time_to_intern_state();
int a42trans_to_intern_state();
int a42uint1_to_intern_state();
int a42uint2_to_intern_state();
int a42uint4_to_intern_state();



int a42execute(acv, a41v)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
    {
    tak_all_command_glob *ptoc_Var0;
    tak40_show_glob *ptoc_Var1;
    cccprint("AK42  a42execute");
    ptoc_Var0 = &*acv;
    ptoc_Var1 = &*a41v;
    switch ((int ) ptoc_Var1->a4sh_kind)
        {
        case 138 :
            ak42consistent_view(&(*acv) , &(*a41v));
            break;
        case 105 :
            ak42ctrl_config(&(*acv) , &(*a41v));
            break;
        case 106 :
            ak42ctrl_devspaces(&(*acv) , &(*a41v));
            break;
        case 107 :
            ak42ctrl_io_statistics(&(*acv) , &(*a41v));
            break;
        case 111 :
            ak42ctrl_locks(&(*acv) , &(*a41v));
            break;
        case 108 :
            ak42ctrl_log_statistics(&(*acv) , &(*a41v));
            break;
        case 109 :
            ak42ctrl_caches_statistics(&(*acv) , &(*a41v));
            break;
        case 9 :
            ak42dbm_state(&(*acv) , &(*a41v));
            break;
        case 96 :
            ak42roots(&(*acv) , &(*a41v));
            break;
        case 146 :
            ak42HeapContent(&(*acv) , &(*a41v));
            break;
        case 139 :
            ak42history_info(&(*acv) , &(*a41v));
            break;
        case 14 :
        case 77 :
        case 84 :
        case 50 :
            ak42select_lock_trans(&(*acv) , &(*a41v));
            break;
        case 30 :
            ak42show_config(&(*acv) , &(*a41v));
            break;
        case 31 :
            ak42serverdb_statistics(&(*acv) , &(*a41v));
            break;
        case 32 :
            ak42devspace_statistics(&(*acv) , &(*a41v));
            break;
        case 34 :
            ak42log_statistics(&(*acv) , &(*a41v));
            break;
        case 11 :
            ak42monitor_oms(&(*acv) , &(*a41v) , 0);
            break;
        case 36 :
            ak42monitor_show(&(*acv) , &(*a41v) , 12);
            break;
        case 37 :
            ak42state_statistics(&(*acv) , &(*a41v));
            break;
        case 40 :
            ak42user_statistics(&(*acv) , &(*a41v));
            break;
        case 56 :
            ak42show_version(&(*acv) , &(*a41v));
            break;
        case 83 :
            ak42xparams(&(*acv) , &(*a41v));
            break;
        case 115 :
            ak42show_running_cmds(&(*acv) , &(*a41v));
            break;
        case 140 :
            ak42oms_memory(&(*acv) , &(*a41v));
            break;
        case 142 :
            ak42internal_state(&(*acv) , &(*a41v));
            break;
        case 145 :
            ak42OmsLockObjInfo(&(*acv) , &(*a41v));
            break;
        case 114 :
            ak42files(&(*acv) , &(*a41v));
            break;
        case 73 :
            ak42InfoGCStats(&(*acv) , &(*a41v));
            break;
        case 148 :
            ak42_container_roots(&(*acv) , &(*a41v));
            break;
        case 149 :
            ak42_container_chains(&(*acv) , &(*a41v));
            break;
        case 150 :
            ak42_container_keys(&(*acv) , &(*a41v));
            break;
        default :
            (unsigned char ) sql__caseerr("vak42.p" , 16846);
            break;
        }
    cccprint("<a42execute     ");
    }



int a42get_tablename(t, tabid, authid, tablen, b_err)
tgg00_TransContext *t;
tgg00_Surrogate tabid;
tsp00_KnlIdentifier authid;
tsp00_KnlIdentifier tablen;
tgg00_BasisError *b_err;
    {
    int name_pos;
    tgg00_SysInfoKey syskey;
    tgg00_Rec sysbuf;
    tak_sysbufferaddress sysbuf_ptr;
    cccprint("AK42  a42get_tab");
    sysbuf_ptr = (struct tak_systembuffer *) &sysbuf;
    _ptoc_ASSIGN(unsigned char , 64, authid, a01_il_b_identifier)
    _ptoc_ASSIGN(unsigned char , 64, tablen, a01_il_b_identifier)
    syskey = a01defaultkey;
    _ptoc_ASSIGN(unsigned char , 8, syskey.integer.C_2.stableid, tabid)
    a36get_catalog_record(&(*t) , &syskey , sql__nil(sysbuf_ptr , "vak42.p" , 16940));
    if ((int ) (*t).trError_gg00 == 0)
        {
        if ((int ) (*(struct tak_systembuffer *) sql__nil(sysbuf_ptr , "vak42.p" , 16944)).integer.C_1.sbase.btablekind == 8)
            {
            name_pos = sizeof(tak_synonymrecord ) - sizeof(tsp00_C64 ) + 1;
            }
        else
            {
            name_pos = a10BaseRecPersistentOffset() + 1;
            }
        t01int4(5 , "name_pos    " , name_pos);
        _ptoc_ASSIGN(unsigned char , 64, tablen, a01_il_b_identifier)
        g10mv("VAK42 " , 1 , sizeof(tsp00_Buf ) , sizeof(tsp00_C64 ) , sysbuf.integer.C_11.buf , name_pos + 1 , tablen , 1 , sql__ord((int ) sysbuf.integer.C_11.buf[sql__setrang(name_pos , 1 , 8192) + -1]) , &(*t).trError_gg00);
        if ((int ) (*(struct tak_systembuffer *) sql__nil(sysbuf_ptr , "vak42.p" , 16956)).integer.C_1.sbase.btablekind == 8)
            {
            _ptoc_ASSIGN(unsigned char , 8, syskey.integer.C_3.sauthid, (*(struct tak_systembuffer *) sql__nil(sysbuf_ptr , "vak42.p" , 16958)).integer.C_34.ssynonym.syn_authid)
            }
        else
            {
            _ptoc_ASSIGN(unsigned char , 8, syskey.integer.C_3.sauthid, (*(struct tak_systembuffer *) sql__nil(sysbuf_ptr , "vak42.p" , 16960)).integer.C_1.sbase.bauthid)
            }
        _ptoc_ASSIGN(unsigned char , 2, syskey.integer.C_2.sentrytyp, "\0B")
        if ((int ) (*t).trError_gg00 == 0)
            {
            a36get_catalog_record(&(*t) , &syskey , sql__nil(sysbuf_ptr , "vak42.p" , 16965));
            }
        if ((int ) (*t).trError_gg00 == 0)
            {
            _ptoc_ASSIGN(unsigned char , 64, authid, (*(struct tak_systembuffer *) sql__nil(sysbuf_ptr , "vak42.p" , 16969)).integer.C_42.suser.username)
            }
        }
    t01lidentifier(19 , authid);
    t01lidentifier(19 , tablen);
    *b_err = (short ) sql__setrang((*t).trError_gg00 , -32768 , 32767);
    cccprint("<a42get_tablenam");
    }



int a42get_username(t, userid, username, b_err)
tgg00_TransContext *t;
tgg00_UserId userid;
tsp00_KnlIdentifier username;
tgg00_BasisError *b_err;
    {
    int i;
    tgg00_SysInfoKey syskey;
    tak_userrecord userrec;
    tak_sysbufferaddress userrec_ptr;
    int ptoc_Var2;
    cccprint("AK42  a42get_use");
    (*t).trError_gg00 = 0;
    if (_ptoc_MEMCMP(unsigned char , 4, userid , "\0\0\0\001") == 0 ? 1 : 0)
        {
        _ptoc_ASSIGN(unsigned char , 64, username, a01controluser)
        }
    else
        {
        userrec_ptr = (struct tak_systembuffer *) &userrec;
        _ptoc_ASSIGN(unsigned char , 64, username, a01_il_b_identifier)
        syskey = a01defaultkey;
        syskey.integer.C_3.sauthid[0] = (unsigned char)'\377';
        syskey.integer.C_3.sauthid[1] = (unsigned char)'\377';
        ptoc_Var2 = sizeof(tsp00_C4 );
        if (1 <= ptoc_Var2)
            {
            i = 1;
            do
                {
                syskey.integer.C_3.sauthid[sql__setrang(sizeof(tsp00_C8 ) - sizeof(tsp00_C4 ) + i , 1 , 8) + -1] = (unsigned char ) sql__setrang(userid[sql__setrang(i , 1 , 4) + -1] , 0 , 255);
                if (i == ptoc_Var2)
                    {
                    break;
                    }
                i += 1;
                }
            while (!0);
            }
        _ptoc_ASSIGN(unsigned char , 2, syskey.integer.C_2.sentrytyp, "\0B")
        a36get_catalog_record(&(*t) , &syskey , sql__nil(userrec_ptr , "vak42.p" , 17010));
        if ((int ) (*t).trError_gg00 == 0)
            {
            _ptoc_ASSIGN(unsigned char , 64, username, userrec.username)
            }
        }
    *b_err = (short ) sql__setrang((*t).trError_gg00 , -32768 , 32767);
    cccprint("<a42get_username");
    }



int a42_get_pagecount(acv, base_ptr, pagecount)
tak_all_command_glob *acv;
tak_sysbufferaddress base_ptr;
tsp00_Int4 *pagecount;
    {
    tsp00_Int4 pages;
    tak_index_scan_record index_scan_rec;
    struct ptoc_Type206
      {
      union
        {
        struct
          {
          tsp00_Longreal d;
          } C_true;
        struct
          {
          tgg00_DoubleBuf b;
          } C_false;
        } boolean;
      } data;
    tgg00_QualBuf qual;
    struct tgg00_MessBlock mblock;
    tak_all_command_glob *ptoc_Var3;
    tgg00_SampleInfo *ptoc_Var4;
    tak_index_scan_record *ptoc_Var5;
    struct ptoc_Type96 *ptoc_Var6;
    cccprint("AK42  a42_get_pa");
    g01mblock_init(&(*acv).a_transinf.tri_trans , 0 , 0 , &mblock);
    g01datapart_init(&mblock , (tgg00_DataPart *) &data , sizeof(struct ptoc_Type206 ));
    g01stack_init(&mblock , (tgg00_StackList *) (int *) 0 , 0 , (tgg00_QualBuf *) &qual , sizeof(tgg00_QualBuf ));
    ptoc_Var3 = &*acv;
    ak42table_statistics(&(*acv) , &(*(struct tak_systembuffer *) sql__nil(base_ptr , "vak42.p" , 17051)).integer.C_1.sbase , &mblock);
    ptoc_Var4 = &(*(tgg00_DataPart *) sql__nil(mblock.integer.C_3.mb_data , "vak42.p" , 17052)).integer.C_7.mbp_sample;
    pages = ptoc_Var4->nodes + ptoc_Var4->all_stringcol_pages;
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var3->a_return_segm , "vak42.p" , 17055)).integer.C_4.sp1r_returncode == 0)
        {
        if ((char ) sql__setrang((*(struct tak_systembuffer *) sql__nil(base_ptr , "vak42.p" , 17061)).integer.C_1.sbase.bindexexist , 0 , 1))
            {
            a24init_index_scan(&(*acv) , (*(struct tak_systembuffer *) sql__nil(base_ptr , "vak42.p" , 17064)).integer.C_1.sbase.bsurrogate , &index_scan_rec);
            ptoc_Var5 = &index_scan_rec;
            while (a24next_named_index(&(*acv) , &index_scan_rec))
                {
                ptoc_Var6 = &(*(struct tak_systembuffer *) sql__nil(ptoc_Var5->isr_buf , "vak42.p" , 17068)).integer.C_22.smindex.indexdef[sql__setrang(ptoc_Var5->isr_index , 1 , 38) + -1];
                if ((int ) ptoc_Var6->indexno != 0 && (int ) (*(tsp1_segment *) sql__nil(ptoc_Var3->a_return_segm , "vak42.p" , 17070)).integer.C_4.sp1r_returncode == 0)
                    {
                    ak42one_index_statistics(&(*acv) , 112 , &(*(struct tak_systembuffer *) sql__nil(base_ptr , "vak42.p" , 17074)).integer.C_1.sbase , &ptoc_Var6->icolstack[0] , &mblock);
                    pages = pages + (*(tgg00_DataPart *) sql__nil(mblock.integer.C_3.mb_data , "vak42.p" , 17077)).integer.C_7.mbp_sample.nodes;
                    }
                }
            a24finish_index_scan(&(*acv) , &index_scan_rec);
            }
        }
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var3->a_return_segm , "vak42.p" , 17091)).integer.C_4.sp1r_returncode == 0)
        {
        *pagecount = *pagecount + pages;
        }
    cccprint("<a42_get_pagecou");
    }



int a42_init_time_array()
    {
    cccprint("AK42  a42_init_t");
    a42check_time = 1;
    cccprint("<a42_init_time_a");
    }



int a42monitor_init(procid)
tsp00_TaskId procid;
    {
    cccprint("AK42  a42monitor");
    ak42refresh_cnt = 0;
    ak42fast_refresh_cnt = 0;
    a260reset_statistics();
    a262reset_statistics();
    k51reset_statistics(procid);
    kb560ResetStatistics(procid);
    k57statistics_reset(procid);
    k720init_ref_statistic();
    b01reset_monitor(0);
    bd91InfoGCReset();
    cccprint("<a42monitor_init");
    }



int a42_start_semantic(acv)
tak_all_command_glob *acv;
    {
    tsp00_TaskId procid;
    tak_all_command_glob *ptoc_Var7;
    cccprint("AK42  a42_start_");
    ptoc_Var7 = &*acv;
    procid = ptoc_Var7->a_transinf.tri_trans.trTaskId_gg00;
    if ((int ) ptoc_Var7->a_current_user_kind != 4 && (int ) ptoc_Var7->a_current_user_kind != 1 && (int ) ptoc_Var7->a_current_user_kind != 5)
        {
        a07_b_put_error(&(*acv) , 6350 , 1);
        }
    else
        {
        switch ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var7->a_ap_tree , "vak42.p" , 17160))[sql__setrang((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var7->a_ap_tree , "vak42.p" , 17160))[0].n_lo_level , 0 , 32767)].n_subproc)
            {
            case 212 :
                if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var7->a_ap_tree , "vak42.p" , 17163))[sql__setrang((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var7->a_ap_tree , "vak42.p" , 17163))[0].n_lo_level , 0 , 32767)].n_length != 323)
                    {
                    a42monitor_init(procid);
                    }
                ak42ResetOmsMonitor(&(*acv));
                break;
            case 326 :
                break;
            case 325 :
                break;
            case 323 :
                ak42monitor_object(&(*acv) , (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var7->a_ap_tree , "vak42.p" , 17178))[sql__setrang((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var7->a_ap_tree , "vak42.p" , 17178))[0].n_lo_level , 0 , 32767)].n_length);
                break;
            default :
                a07_b_put_error(&(*acv) , 3400 , 1);
                break;
            }
        }
    cccprint("<a42_start_seman");
    }



int ak42ctrl_config(acv, a41v)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
    {
    char autosave_on;
    tsp00_Int4 kb_factor;
    tsp00_Int4 totalPages;
    tsp00_Int4 unMapPages;
    tsp00_Sname sname;
    tsp00_C64 media_name;
    tkb00_LogDeviceInfo log_device_info;
    tkb00_LogQueueInfo log_queue_info;
    cccprint("AK42  ak42ctrl_c");
    bd01DataCacheSize(&totalPages , &unMapPages);
    a40get_catalog_table(&(*acv) , &(*a41v));
    if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak42.p" , 17210)).integer.C_4.sp1r_returncode == 0)
        {
        switch ((int ) g01code.char_default)
            {
            case 0 :
                _ptoc_ASSIGN(unsigned char , 12, sname, "ASCII       ")
                break;
            case 1 :
                _ptoc_ASSIGN(unsigned char , 12, sname, "EBCDIC      ")
                break;
            case 2 :
                _ptoc_ASSIGN(unsigned char , 12, sname, "INTERNAL    ")
                break;
            case 20 :
                _ptoc_ASSIGN(unsigned char , 12, sname, "UNICODE     ")
                break;
            default :
                _ptoc_ASSIGN(unsigned char , 12, sname, "            ")
                break;
            }
        a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) sname , sizeof(tsp00_C12 ));
        switch ((int ) g01code.date_time)
            {
            case 1 :
                _ptoc_ASSIGN(unsigned char , 12, sname, "INTERNAL    ")
                break;
            case 4 :
            case 8 :
                _ptoc_ASSIGN(unsigned char , 12, sname, "EUR         ")
                break;
            case 2 :
                _ptoc_ASSIGN(unsigned char , 12, sname, "ISO         ")
                break;
            case 5 :
                _ptoc_ASSIGN(unsigned char , 12, sname, "JIS         ")
                break;
            case 3 :
                _ptoc_ASSIGN(unsigned char , 12, sname, "USA         ")
                break;
            default :
                _ptoc_ASSIGN(unsigned char , 12, sname, "            ")
                break;
            }
        a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) sname , sizeof(tsp00_C12 ));
        kb560GetStatistics((*acv).a_transinf.tri_trans.trTaskId_gg00 , &log_device_info , &log_queue_info);
        _ptoc_ASSIGN(unsigned char , 12, sname, "SINGLE      ")
        a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) sname , sizeof(tsp00_C12 ));
        a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) a40yes_no[0] , 1);
        a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) a40yes_no[sql__setrang((int ) b01db_full((*acv).a_transinf.tri_trans.trTaskId_gg00) , 0 , 1)] , 1);
        a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) a40yes_no[sql__setrang((int ) kb560LogIsFull((*acv).a_transinf.tri_trans.trTaskId_gg00) , 0 , 1)] , 1);
        a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) a40yes_no[sql__setrang((int ) b01connect_allowed() , 0 , 1)] , 1);
        a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) a40yes_no[sql__setrang((int ) (char ) sql__setrang(a01diag_monitor_on , 0 , 1) , 0 , 1)] , 1);
        a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) a40yes_no[1] , 1);
        a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) a40yes_no[sql__setrang((int ) (char ) sql__setrang(g01vtrace.vtrAny_gg00 , 0 , 1) , 0 , 1)] , 1);
        autosave_on = (char ) sql__setrang(k38is_on_autosave(&(*acv).a_transinf.tri_trans , 0) , 0 , 1);
        a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) a40yes_no[sql__setrang((int ) (char ) sql__setrang(autosave_on , 0 , 1) , 0 , 1)] , 1);
        if ((char ) sql__setrang(autosave_on , 0 , 1))
            {
            k38get_autosave_medianame((*acv).a_transinf.tri_trans.trTaskId_gg00 , media_name);
            a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) media_name , sizeof(tsp00_C64 ));
            }
        else
            {
            a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) (int *) 0 , 0);
            }
        a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) a40yes_no[sql__setrang((int ) k51is_closed_locklist((*acv).a_transinf.tri_trans.trTaskId_gg00) , 0 , 1)] , 1);
        a40move_i4(&(*acv) , &(*a41v) , g01maxuser() , 0 , 0 , 0);
        a40move_i4(&(*acv) , &(*a41v) , g01maxcpu() , 0 , 0 , 0);
        a40move_i4(&(*acv) , &(*a41v) , b01no_of_log_devs() , 0 , 0 , 0);
        a40move_i4(&(*acv) , &(*a41v) , b01no_of_data_devs() , 0 , 0 , 0);
        a40move_i4(&(*acv) , &(*a41v) , b01get_bad_index_cnt((*acv).a_transinf.tri_trans.trTaskId_gg00) , 0 , 0 , 0);
        kb_factor = 8;
        a40move_i4(&(*acv) , &(*a41v) , bd01ConverterSize() * kb_factor , 0 , 0 , 0);
        a40move_i4(&(*acv) , &(*a41v) , totalPages * kb_factor , 0 , 0 , 0);
        a40move_i4(&(*acv) , &(*a41v) , g01cache_size(6) , 0 , 0 , 0);
        a40move_i4(&(*acv) , &(*a41v) , g01maxlocks() , 0 , 0 , 0);
        }
    cccprint("<ak42ctrl_config");
    }



int ak42ctrl_devspaces(acv, a41v)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
    {
    int kb_factor;
    tsp_int4 dev_no;
    tsp00_Int4 dev_used_blocks;
    tsp_c4 devType;
    tsp2_dev_type dev_type;
    tsp2_devname dev_name;
    tsp_int4 dev_size;
    char dev_online;
    tsp_c18 dev_state;
    cccprint("AK42  ak42ctrl_d");
    a40get_catalog_table(&(*acv) , &(*a41v));
    if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak42.p" , 17311)).integer.C_4.sp1r_returncode == 0)
        {
        dev_no = -1;
        kb_factor = 8;
        do
            {
            b01next_dev_info((*acv).a_transinf.tri_trans.trTaskId_gg00 , &dev_no , &dev_type , dev_name , &dev_size , &dev_used_blocks , &dev_online , dev_state);
            if (dev_no >= 0)
                {
                switch ((int ) dev_type)
                    {
                    case 0 :
                        _ptoc_ASSIGN(unsigned char , 4, devType, "SYS ")
                        break;
                    case 2 :
                        _ptoc_ASSIGN(unsigned char , 4, devType, "LOG ")
                        break;
                    case 5 :
                        _ptoc_ASSIGN(unsigned char , 4, devType, "DATA")
                        break;
                    default :
                        _ptoc_ASSIGN(unsigned char , 4, devType, "    ")
                        break;
                    }
                a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) devType , sizeof(tsp00_C4 ));
                a40move_i4(&(*acv) , &(*a41v) , dev_no , 0 , 0 , 0);
                a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) dev_name , sizeof(tsp00_C64 ));
                a40move_i4(&(*acv) , &(*a41v) , dev_size * kb_factor , 0 , 0 , 0);
                a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) dev_state , sizeof(tsp00_C18 ));
                }
            }
        while (!(dev_no == -1));
        }
    cccprint("<ak42ctrl_devspa");
    }



int ak42ctrl_io_statistics(acv, a41v)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
    {
    int rowNo;
    int sum_ix;
    tgg00_RefInfoIndex ind;
    tsp_c10 c10;
    tgg00_ReferenceInfo info;
    double sum[4];
    cccprint("AK42  ak42ctrl_i");
    a40get_catalog_table(&(*acv) , &(*a41v));
    if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak42.p" , 17367)).integer.C_4.sp1r_returncode == 0)
        {
        sum_ix = 1;
        do
            {
            sum[sql__setrang(sum_ix , 1 , 4) + -1] = (double ) 0.00000000000000000000e+000;
            if (sum_ix == 4)
                {
                break;
                }
            sum_ix += 1;
            }
        while (!0);
        rowNo = 0;
        ak42get_refinfo(&(*acv) , &info);
        ind = 108;
        do
            {
            if ((*a41v).a4pos == 1)
                {
                sum_ix = 1;
                rowNo = rowNo + 1;
                switch (rowNo)
                    {
                    case 1 :
                        _ptoc_ASSIGN(unsigned char , 10, c10, "Catalog   ")
                        break;
                    case 2 :
                        _ptoc_ASSIGN(unsigned char , 10, c10, "Perm Data ")
                        break;
                    case 3 :
                        _ptoc_ASSIGN(unsigned char , 10, c10, "Temp Data ")
                        break;
                    case 4 :
                        _ptoc_ASSIGN(unsigned char , 10, c10, "Long Data ")
                        break;
                    case 5 :
                        _ptoc_ASSIGN(unsigned char , 10, c10, "Leaf      ")
                        break;
                    case 6 :
                        _ptoc_ASSIGN(unsigned char , 10, c10, "Level 1   ")
                        break;
                    case 7 :
                        _ptoc_ASSIGN(unsigned char , 10, c10, "Level 2   ")
                        break;
                    case 8 :
                        _ptoc_ASSIGN(unsigned char , 10, c10, "Level 3   ")
                        break;
                    case 9 :
                        _ptoc_ASSIGN(unsigned char , 10, c10, "Level 4   ")
                        break;
                    case 10 :
                        _ptoc_ASSIGN(unsigned char , 10, c10, "Level 5   ")
                        break;
                    case 11 :
                        _ptoc_ASSIGN(unsigned char , 10, c10, "Sum (Tree)")
                        a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) c10 , sizeof(tsp00_C10 ));
                        sum_ix = 1;
                        do
                            {
                            a40move_real(&(*acv) , &(*a41v) , sum[sql__setrang(sum_ix , 1 , 4) + -1] , (char ) sql__setrang(sum[sql__setrang(sum_ix , 1 , 4) + -1] < (double ) 0.00000000000000000000e+000 , 0 , 1) , 0);
                            if (sum_ix == 4)
                                {
                                break;
                                }
                            sum_ix += 1;
                            }
                        while (!0);
                        _ptoc_ASSIGN(unsigned char , 10, c10, "Converter ")
                        break;
                    default :
                        sql__caseerr("vak42.p" , 17383);
                        break;
                    }
                a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) c10 , sizeof(tsp00_C10 ));
                }
            a40move_counter(&(*acv) , &(*a41v) , info.boolean.C_false.reference_array[sql__setrang((int ) ind , 0 , 176)] , 0 , 0 , 0);
            if (rowNo >= 4 && rowNo <= 10)
                {
                if (sum[sql__setrang(sum_ix , 1 , 4) + -1] >= (double ) 0.00000000000000000000e+000)
                    {
                    sum[sql__setrang(sum_ix , 1 , 4) + -1] = sum[sql__setrang(sum_ix , 1 , 4) + -1] + info.boolean.C_false.reference_array[sql__setrang((int ) ind , 0 , 176)];
                    }
                }
            sum_ix = sum_ix + 1;
            if (ind == 151)
                {
                break;
                }
            (unsigned char ) sql__setrang(ind += 1 , 0 , 176);
            }
        while (!0);
        }
    cccprint("<ak42ctrl_io_sta");
    }



int ak42ctrl_locks(acv, a41v)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
    {
    tkb05_LockInfo lock_info;
    tkb05_LockInfo *ptoc_Var8;
    cccprint("AK42  ak42ctrl_l");
    a40get_catalog_table(&(*acv) , &(*a41v));
    if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak42.p" , 17449)).integer.C_4.sp1r_returncode == 0)
        {
        k51info_lock((*acv).a_transinf.tri_trans.trTaskId_gg00 , &lock_info);
        ptoc_Var8 = &lock_info;
        a40move_i4(&(*acv) , &(*a41v) , ptoc_Var8->infMaxEntries_kb05 , 0 , 0 , 0);
        a40move_i4(&(*acv) , &(*a41v) , ptoc_Var8->infAvgUsed_kb05 , 0 , 0 , 0);
        a40move_counter(&(*acv) , &(*a41v) , ptoc_Var8->infSqlCollisions_kb05 , 0 , 0 , 0);
        a40move_counter(&(*acv) , &(*a41v) , ptoc_Var8->infEscalations_kb05 , 0 , 0 , 0);
        a40move_counter(&(*acv) , &(*a41v) , ptoc_Var8->infRowEntryAmount_kb05 , 0 , 0 , 0);
        a40move_counter(&(*acv) , &(*a41v) , ptoc_Var8->infTabEntryAmount_kb05 , 0 , 0 , 0);
        a40move_i4(&(*acv) , &(*a41v) , g01timeout.timeReq_gg00 , 0 , 0 , 0);
        }
    cccprint("<ak42ctrl_locks ");
    }



int ak42ctrl_log_statistics(acv, a41v)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
    {
    int kb_factor;
    tsp00_Int4 log_percent;
    tsp00_Sname sname;
    tgg00_ReferenceInfo ref_info;
    tkb00_LogQueueInfo log_queue_info;
    tkb00_LogDeviceInfo log_device_info;
    tkb00_LogDeviceInfo *ptoc_Var9;
    tgg00_ReferenceInfo *ptoc_Var10;
    tkb00_LogQueueInfo *ptoc_Var11;
    cccprint("AK42  ak42ctrl_l");
    a40get_catalog_table(&(*acv) , &(*a41v));
    if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak42.p" , 17488)).integer.C_4.sp1r_returncode == 0)
        {
        kb_factor = 8;
        ak42get_refinfo(&(*acv) , &ref_info);
        kb560GetStatistics((*acv).a_transinf.tri_trans.trTaskId_gg00 , &log_device_info , &log_queue_info);
        _ptoc_ASSIGN(unsigned char , 12, sname, "SINGLE      ")
        a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) sname , sizeof(tsp00_C12 ));
        a40move_i4(&(*acv) , &(*a41v) , kb560GetLogDeviceSize() * kb_factor , 0 , 0 , 0);
        a40move_i4(&(*acv) , &(*a41v) , g01autosave_distance * kb_factor , 0 , 0 , 0);
        ptoc_Var9 = &log_device_info;
        a40move_i4(&(*acv) , &(*a41v) , ptoc_Var9->ldiPagesUsed_kb00 * kb_factor , 0 , 0 , 0);
        log_percent = ak42evalpercent(ptoc_Var9->ldiPagesUsed_kb00 , kb560GetLogDeviceSize());
        a40move_i4(&(*acv) , &(*a41v) , log_percent , 0 , 0 , 0);
        a40move_i4(&(*acv) , &(*a41v) , ptoc_Var9->ldiPagesNotSaved_kb00 * kb_factor , 0 , 0 , 0);
        log_percent = ak42evalpercent(ptoc_Var9->ldiPagesNotSaved_kb00 , kb560GetLogDeviceSize());
        a40move_i4(&(*acv) , &(*a41v) , log_percent , 0 , 0 , 0);
        a40move_i4(&(*acv) , &(*a41v) , ptoc_Var9->ldiPagesSinceBup_kb00 * kb_factor , 0 , 0 , 0);
        a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) (int *) 0 , 0);
        a40move_i4(&(*acv) , &(*a41v) , ptoc_Var9->ldiSaveptCount_kb00 , 0 , 0 , 0);
        a40move_i4(&(*acv) , &(*a41v) , 0 , 0 , 0 , 0);
        ptoc_Var10 = &ref_info;
        a40move_counter(&(*acv) , &(*a41v) , ptoc_Var10->boolean.C_false.reference_array[92] , 0 , 0 , 0);
        a40move_counter(&(*acv) , &(*a41v) , ptoc_Var10->boolean.C_false.reference_array[93] , 0 , 0 , 0);
        ptoc_Var11 = &log_queue_info;
        a40move_i4(&(*acv) , &(*a41v) , ptoc_Var11->lqiLogQueueSize_kb00 * kb_factor , 0 , 0 , 0);
        a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) (int *) 0 , 0);
        a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) (int *) 0 , 0);
        a40move_counter(&(*acv) , &(*a41v) , ptoc_Var11->lqiCountRequest_kb00 , 0 , 0 , 0);
        a40move_counter(&(*acv) , &(*a41v) , ptoc_Var11->lqiCountGroup_kb00 , 0 , 0 , 0);
        a40move_counter(&(*acv) , &(*a41v) , ptoc_Var11->lqiCountWait_kb00 , 0 , 0 , 0);
        a40move_i4(&(*acv) , &(*a41v) , ptoc_Var11->lqiMaxWaitLogpage_kb00 , 0 , 0 , 0);
        a40move_counter(&(*acv) , &(*a41v) , ptoc_Var11->lqiAvgSumPagewaits_kb00 , 0 , 0 , 0);
        a40move_i4(&(*acv) , &(*a41v) , 0 , 0 , 0 , 0);
        a40move_i4(&(*acv) , &(*a41v) , 0 , 0 , 0 , 0);
        }
    cccprint("<ak42ctrl_log_st");
    }



int ak42ctrl_caches_statistics(acv, a41v)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
    {
    tgg00_RefInfoIndex i;
    tgg00_RefInfoIndex ind;
    int kind_ix;
    tsp00_Longreal total;
    tsp00_Longreal hit;
    tsp00_C18 cache_kind[3];
    tgg00_ReferenceInfo info;
    int ptoc_Var12;
    cccprint("AK42  ak42ctrl_c");
    a40get_catalog_table(&(*acv) , &(*a41v));
    if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak42.p" , 17562)).integer.C_4.sp1r_returncode == 0)
        {
        ak42get_refinfo(&(*acv) , &info);
        _ptoc_ASSIGN(unsigned char , 18, cache_kind[0], "DATA              ")
        _ptoc_ASSIGN(unsigned char , 18, cache_kind[1], "CATALOG           ")
        _ptoc_ASSIGN(unsigned char , 18, cache_kind[2], "SEQUENCE          ")
        kind_ix = 0;
        ind = 0;
        do
            {
            if ((ptoc_Var12 = (int ) ind) == 0 || ptoc_Var12 == 4 || ptoc_Var12 == 8)
                {
                kind_ix = kind_ix + 1;
                a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) cache_kind[sql__setrang(kind_ix , 1 , 3) + -1] , sizeof(tsp00_C18 ));
                }
            else
                {
                if ((ptoc_Var12 = (int ) ind) == 3 || ptoc_Var12 == 7 || ptoc_Var12 == 11)
                    {
                    i = (unsigned char ) sql__setrang((unsigned char ) sql__pred((int ) (unsigned char ) sql__pred((int ) ind , 0 , 176 , "vak42.p" , 17598) , 0 , 176 , "vak42.p" , 17598) , 0 , 176);
                    hit = info.boolean.C_false.reference_array[sql__setrang((int ) i , 0 , 176)];
                    i = (unsigned char ) sql__setrang((unsigned char ) sql__pred((int ) i , 0 , 176 , "vak42.p" , 17600) , 0 , 176);
                    total = info.boolean.C_false.reference_array[sql__setrang((int ) i , 0 , 176)];
                    info.boolean.C_false.reference_array[sql__setrang((int ) ind , 0 , 176)] = (double ) ak42evalpercent_real(hit , total);
                    }
                }
            a40move_counter(&(*acv) , &(*a41v) , info.boolean.C_false.reference_array[sql__setrang((int ) ind , 0 , 176)] , 0 , 0 , 0);
            if (ind == 11)
                {
                break;
                }
            (unsigned char ) sql__setrang(ind += 1 , 0 , 176);
            }
        while (!0);
        }
    cccprint("<ak42ctrl_caches");
    }



int ak42out_monitor(acv, a41v, info, ind)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
tgg00_ReferenceInfo *info;
tgg00_RefInfoIndex ind;
    {
    tsp00_Longreal hit;
    tsp00_Longreal total;
    tgg00_RefInfoIndex i;
    int ptoc_Var13;
    cccprint("AK42  ak42out_mo");
    if ((ptoc_Var13 = (int ) ind) == 3 || ptoc_Var13 == 7 || ptoc_Var13 == 11 || ptoc_Var13 == 19 || ptoc_Var13 == 23 || ptoc_Var13 == 27 || ptoc_Var13 == 15)
        {
        i = (unsigned char ) sql__setrang((unsigned char ) sql__pred((int ) (unsigned char ) sql__pred((int ) ind , 0 , 176 , "vak42.p" , 17648) , 0 , 176 , "vak42.p" , 17648) , 0 , 176);
        hit = (*info).boolean.C_false.reference_array[sql__setrang((int ) i , 0 , 176)];
        i = (unsigned char ) sql__setrang((unsigned char ) sql__pred((int ) i , 0 , 176 , "vak42.p" , 17650) , 0 , 176);
        total = (*info).boolean.C_false.reference_array[sql__setrang((int ) i , 0 , 176)];
        if (hit >= (double ) 0.00000000000000000000e+000 && total >= (double ) 0.00000000000000000000e+000)
            {
            (*info).boolean.C_false.reference_array[sql__setrang((int ) ind , 0 , 176)] = (double ) ak42evalpercent_real(hit , total);
            }
        else
            {
            (*info).boolean.C_false.reference_array[sql__setrang((int ) ind , 0 , 176)] = (double ) -1.00000000000000000000e+000;
            }
        }
    if ((int ) ind >= 0 && (int ) ind <= 27)
        {
        (*a41v).a4ti = 48;
        }
    if ((int ) ind >= 28 && (int ) ind <= 83)
        {
        (*a41v).a4ti = 250;
        }
    if ((int ) ind >= 84 && (int ) ind <= 91)
        {
        (*a41v).a4ti = 253;
        }
    if ((int ) ind >= 92 && (int ) ind <= 101)
        {
        (*a41v).a4ti = 254;
        }
    if ((int ) ind >= 102 && (int ) ind <= 103)
        {
        (*a41v).a4ti = 258;
        }
    if ((int ) ind >= 104 && (int ) ind <= 151)
        {
        (*a41v).a4ti = 340;
        }
    if ((int ) ind >= 152 && (int ) ind <= 172)
        {
        (*a41v).a4ti = 394;
        }
    if ((int ) ind >= 173 && (int ) ind <= 175)
        {
        (*a41v).a4ti = 480;
        }
    if ((*info).boolean.C_false.reference_array[sql__setrang((int ) ind , 0 , 176)] >= (double ) 0.00000000000000000000e+000)
        {
        ak42ref_stat_real(&(*acv) , &(*a41v) , a42monitor_arr[sql__setrang((int ) ind , 0 , 176)] , (*info).boolean.C_false.reference_array[sql__setrang((int ) ind , 0 , 176)] * (double ) 1.00000000000000000000e+000);
        }
    if (g01is_livecache())
        {
        if ((int ) ind == 27)
            {
            ak42data_cache_statistics(&(*acv) , &(*a41v));
            }
        }
    cccprint("<ak42out_monitor");
    }



int ak42data_cache_statistics(acv, a41v)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
    {
    tsp00_Int4 DataCacheSize;
    tsp00_Int4 OmsHistoryPageCnt;
    tsp00_Int4 OmsDataPageCnt;
    tsp00_Int4 SqlDataPageCnt;
    cccprint("AK42  ak42data_c");
    (*a41v).a4ti = 48;
    b01get_data_cache_workload(&(*acv).a_transinf.tri_trans , &DataCacheSize , &OmsHistoryPageCnt , &OmsDataPageCnt , &SqlDataPageCnt);
    ak42ref_i4_statistic(&(*acv) , &(*a41v) , "Data cache no of SQL data pages         " , SqlDataPageCnt);
    ak42ref_i4_statistic(&(*acv) , &(*a41v) , "Data cache no of OMS data pages         " , OmsDataPageCnt);
    ak42ref_i4_statistic(&(*acv) , &(*a41v) , "Data cache no of history/undo pages     " , OmsHistoryPageCnt);
    cccprint("<ak42data_cache_");
    }

static
  unsigned char ptoc_Var14[1] =
  {
  (unsigned char)'\020'
  };


int ak42connected_users(acv, a41v)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
    {
    int _ak42connected_users;
    tgg_basis_error b_err;
    tgg00_FileId aux_file;
    tak_sm_description sm_desc;
    tgg_mess_block mblock;
    tsp_buf data;
    tgg_qual_buf qual;
    int cnt;
    tgg_bd_set_result_record set_result;
    tgg00_FilePos tree_pos;
    tgg_lkey zerokey;
    tsp_buf b;
    tak_all_command_glob *ptoc_Var15;
    tak40_show_glob *ptoc_Var16;
    unsigned char ptoc_Var17[1];
    unsigned char ptoc_Var18[1];
    unsigned char ptoc_Var19[1];
    tak_sm_description *ptoc_Var20;
    tgg00_BdSetResultRecord *ptoc_Var21;
    cccprint("AK42  ak42connec");
    ptoc_Var15 = &*acv;
    ptoc_Var16 = &*a41v;
    g04build_temp_tree_id(&aux_file , &ptoc_Var15->a_transinf.tri_trans);
    aux_file.integer.C_8.fileTempCnt_gg00 = 1;
    aux_file.integer.C_8.fileTfnTemp_gg00 = 17;
    _ptoc_MOVE(unsigned char , 1, aux_file.integer.C_1.fileType_gg00, *(unsigned char (*)[1]) sql__psetadd(ptoc_Var17 , aux_file.integer.C_1.fileType_gg00 , ptoc_Var14 , sizeof(unsigned char [1])));
    g01mblock_init(&ptoc_Var15->a_transinf.tri_trans , 63 , 66 , &mblock);
    g01datapart_init(&mblock , (tgg00_DataPart *) data , sizeof(tsp00_Buf ));
    mblock.integer.C_2.mb_struct = 1;
    (*(tgg00_DataPart *) sql__nil(mblock.integer.C_3.mb_data , "vak42.p" , 17788)).integer.C_1.mbp_buf[0] = (unsigned char ) sql__setrang((unsigned char ) sql__ord((int ) g01code.ctype) , 0 , 255);
    mblock.integer.C_2.mb_data_len = 1;
    g01qual_init(&mblock , (tgg00_QualBuf *) &qual , sizeof(tgg00_QualBuf ));
    a06set_mtree(&(*acv) , &mblock , &aux_file);
    mblock.integer.C_2.mb_qual_len = sizeof(tgg00_FileId );
    ptoc_Var20 = &sm_desc;
    ptoc_Var20->sm_error_cnt = 0;
    ptoc_Var20->sm_errorlist[0] = 4210;
    ptoc_Var20->sm_errorlist[1] = 4215;
    ptoc_Var20->sm_return_req = 0;
    ptoc_Var20->sm_must_local = 1;
    ptoc_Var20->sm_not_local = 0;
    a06ignore_rsend_mess_buf(&(*acv) , &mblock , &sm_desc , &b_err);
    if ((int ) b_err != 0)
        {
        a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , 1);
        }
    else
        {
        ptoc_Var21 = &set_result;
        ptoc_Var21->bd_key_check_len = 0;
        ptoc_Var21->bd_max_rec_cnt = 32767;
        ptoc_Var21->bd_max_fill_len = sizeof(tsp00_Buf );
        ptoc_Var21->bd_next = 0;
        zerokey.integer.C_11.len = 0;
        tree_pos.tpsPno_gg00 = 2147483647;
        cnt = 0;
        do
            {
            b07cnext_record(&ptoc_Var15->a_transinf.tri_trans , &aux_file , &zerokey , &set_result , &tree_pos , b);
            b_err = (short ) sql__setrang(ptoc_Var15->a_transinf.tri_trans.trError_gg00 , -32768 , 32767);
            if ((int ) b_err == 0 || (int ) b_err == 1200 || (int ) b_err == 350)
                {
                cnt = cnt + set_result.bd_rec_cnt;
                if ((int ) b_err == 350)
                    {
                    b_err = 0;
                    }
                else
                    {
                    b_err = 1420;
                    }
                }
            }
        while (!((int ) b_err != 0 || (int ) (*(tsp1_segment *) sql__nil(ptoc_Var15->a_return_segm , "vak42.p" , 17838)).integer.C_4.sp1r_returncode != 0));
        b01destroy_file(&ptoc_Var15->a_transinf.tri_trans , &aux_file);
        _ak42connected_users = cnt;
        }
    cccprint("<ak42connected_u");
    return(_ak42connected_users);
    }



int ak42datacache_hits(acv, datacache_hitrate, datacache_oms_hitrate, datacache_oms_log_hitrate)
tak_all_command_glob *acv;
tsp00_Int4 *datacache_hitrate;
tsp00_Int4 *datacache_oms_hitrate;
tsp00_Int4 *datacache_oms_log_hitrate;
    {
    tgg00_ReferenceInfo info;
    tsp00_Longreal total;
    tsp00_Longreal hit;
    tak_all_command_glob *ptoc_Var22;
    tgg00_ReferenceInfo *ptoc_Var23;
    cccprint("AK42  ak42dataca");
    ptoc_Var22 = &*acv;
    ptoc_Var23 = &info;
    b01show_monitor(ptoc_Var22->a_transinf.tri_trans.trTaskId_gg00 , &info);
    total = info.boolean.C_false.reference_array[0];
    hit = info.boolean.C_false.reference_array[1];
    if (hit >= (double ) 0.00000000000000000000e+000 && total >= (double ) 0.00000000000000000000e+000)
        {
        *datacache_hitrate = ak42evalpercent_real(hit , total);
        }
    else
        {
        *datacache_hitrate = 0;
        }
    *datacache_oms_hitrate = 0;
    *datacache_oms_log_hitrate = 0;
    total = info.boolean.C_false.reference_array[16];
    hit = info.boolean.C_false.reference_array[17];
    if (hit >= (double ) 0.00000000000000000000e+000 && total >= (double ) 0.00000000000000000000e+000)
        {
        *datacache_oms_hitrate = ak42evalpercent_real(hit , total);
        }
    total = info.boolean.C_false.reference_array[20];
    hit = info.boolean.C_false.reference_array[21];
    if (hit >= (double ) 0.00000000000000000000e+000 && total >= (double ) 0.00000000000000000000e+000)
        {
        *datacache_oms_log_hitrate = ak42evalpercent_real(hit , total);
        }
    cccprint("<ak42datacache_h");
    }



int ak42dbm_state(acv, a41v)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
    {
    char autosave_on;
    char really_bad;
    tsp00_Int4 datacache_hitrate;
    tsp00_Int4 datacache_oms_hitrate;
    tsp00_Int4 datacache_oms_log_hitrate;
    tsp00_Int4 cnt;
    tsp00_Int4 devno;
    tsp2_dev_type dev_type;
    tsp2_devname devname;
    tsp00_C3 yes_no[2];
    tsp00_C3 on_off[2];
    cccprint("AK42  ak42dbm_st");
    a40get_catalog_table(&(*acv) , &(*a41v));
    if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak42.p" , 17916)).integer.C_4.sp1r_returncode == 0)
        {
        _ptoc_ASSIGN(unsigned char , 3, yes_no[1], "Yes")
        _ptoc_ASSIGN(unsigned char , 3, yes_no[0], "No ")
        _ptoc_ASSIGN(unsigned char , 3, on_off[1], "On ")
        _ptoc_ASSIGN(unsigned char , 3, on_off[0], "Off")
        a40move_i4(&(*acv) , &(*a41v) , g01page_size / 1024 , 0 , 0 , 0);
        a40move_i4(&(*acv) , &(*a41v) , g01maxuser() , 0 , 0 , 0);
        a40move_i4(&(*acv) , &(*a41v) , ak42connected_users(&(*acv) , &(*a41v)) , 0 , 0 , 0);
        ak42datacache_hits(&(*acv) , &datacache_hitrate , &datacache_oms_hitrate , &datacache_oms_log_hitrate);
        a40move_i4(&(*acv) , &(*a41v) , datacache_hitrate , 0 , 0 , 0);
        a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) yes_no[sql__setrang((int ) b01db_full((*acv).a_transinf.tri_trans.trTaskId_gg00) , 0 , 1)] , 3);
        a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) yes_no[sql__setrang((int ) b01connect_allowed() , 0 , 1)] , 3);
        a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) on_off[sql__setrang((int ) (char ) sql__setrang(a01diag_monitor_on , 0 , 1) , 0 , 1)] , 3);
        a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) on_off[1] , 3);
        a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) on_off[sql__setrang((int ) (char ) sql__setrang(g01vtrace.vtrAny_gg00 , 0 , 1) , 0 , 1)] , 3);
        autosave_on = (char ) sql__setrang(k38is_on_autosave(&(*acv).a_transinf.tri_trans , 0) , 0 , 1);
        a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) on_off[sql__setrang((int ) (char ) sql__setrang(autosave_on , 0 , 1) , 0 , 1)] , 3);
        a40move_i4(&(*acv) , &(*a41v) , b01get_bad_index_cnt((*acv).a_transinf.tri_trans.trTaskId_gg00) , 0 , 0 , 0);
        a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) yes_no[sql__setrang((int ) kb560LogIsFull((*acv).a_transinf.tri_trans.trTaskId_gg00) , 0 , 1)] , 3);
        cnt = 0;
        dev_type = 2;
        devno = 1;
        while ((int ) dev_type != 7)
            {
            b01next_bad_dev((*acv).a_transinf.tri_trans.trTaskId_gg00 , &dev_type , &devno , devname , &really_bad);
            if (_ptoc_MEMCMP(unsigned char , 64, devname , b01blankfilename) == 0 ? 0 : 1)
                {
                if ((char ) sql__setrang(really_bad , 0 , 1))
                    {
                    cnt = sql__succ(cnt , -2147483647 , 2147483647 , "vak42.p" , 17953);
                    }
                }
            }
        a40move_i4(&(*acv) , &(*a41v) , cnt , 0 , 0 , 0);
        a40move_i4(&(*acv) , &(*a41v) , datacache_oms_hitrate , 0 , 0 , 0);
        a40move_i4(&(*acv) , &(*a41v) , datacache_oms_log_hitrate , 0 , 0 , 0);
        }
    cccprint("<ak42dbm_state  ");
    }



int ak42HeapContent(acv, a41v)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
    {
    cccprint("AK42  ak42HeapCo");
    a40get_catalog_table(&(*acv) , &(*a41v));
    if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak42.p" , 17977)).integer.C_4.sp1r_returncode == 0)
        {
        ak341MonitorHeapContent(&(*acv) , &(*a41v));
        }
    cccprint("<ak42HeapContent");
    }



int ak42history_info(acv, a41v)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
    {
    tsp00_Int4 HistFileNo;
    tgg91_TransNo FirstTransId;
    char FirstTransIsUsed;
    tgg91_TransNo LastTransId;
    char LastTransIsUsed;
    tsp00_Int4 PageCount;
    char bFileIsEmpty;
    cccprint("AK42  ak42histor");
    if (!g01is_livecache())
        {
        a07_b_put_error(&(*acv) , 6559 , 1);
        }
    else
        {
        a40get_catalog_table(&(*acv) , &(*a41v));
        if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak42.p" , 18010)).integer.C_4.sp1r_returncode == 0)
            {
            HistFileNo = 0;
            do
                {
                kb560GetHistFileInfo(&(*acv).a_transinf.tri_trans , &HistFileNo , &FirstTransId , &FirstTransIsUsed , &LastTransId , &LastTransIsUsed , &PageCount , &bFileIsEmpty);
                if ((int ) (*acv).a_transinf.tri_trans.trError_gg00 == 0 && HistFileNo >= 0)
                    {
                    a40move_i4(&(*acv) , &(*a41v) , HistFileNo + -1 , 0 , 0 , 0);
                    ak42move_trans_no(&(*acv) , &(*a41v) , &FirstTransId);
                    a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) a40yes_no[sql__setrang((int ) (char ) sql__setrang(FirstTransIsUsed , 0 , 1) , 0 , 1)] , 3);
                    ak42move_trans_no(&(*acv) , &(*a41v) , &LastTransId);
                    a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) a40yes_no[sql__setrang((int ) (char ) sql__setrang(LastTransIsUsed , 0 , 1) , 0 , 1)] , 3);
                    a40move_i4(&(*acv) , &(*a41v) , PageCount , 0 , 0 , 0);
                    a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) a40yes_no[sql__setrang((int ) (char ) sql__setrang(bFileIsEmpty , 0 , 1) , 0 , 1)] , 3);
                    }
                }
            while (!(HistFileNo < 0 || (int ) (*acv).a_transinf.tri_trans.trError_gg00 != 0));
            if ((int ) (*acv).a_transinf.tri_trans.trError_gg00 != 0)
                {
                a07_b_put_error(&(*acv) , (short ) sql__setrang((*acv).a_transinf.tri_trans.trError_gg00 , -32768 , 32767) , 1);
                }
            }
        }
    cccprint("<ak42history_inf");
    }



int ak42monitor_object(acv, kw_index)
tak_all_command_glob *acv;
int kw_index;
    {
    cccprint("AK42  ak42monito");
    ak341StartStopMonitor((char ) sql__setrang(kw_index == 326 , 0 , 1));
    cccprint("<ak42monitor_obj");
    }



int ak42monitor_show(acv, a41v, kw_index)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
int kw_index;
    {
    tgg00_RefInfoIndex ind;
    tgg00_ReferenceInfo info;
    tak_all_command_glob *ptoc_Var24;
    tak40_show_glob *ptoc_Var25;
    tgg00_ReferenceInfo *ptoc_Var26;
    cccprint("AK42  ak42monito");
    ptoc_Var24 = &*acv;
    ptoc_Var25 = &*a41v;
    ptoc_Var26 = &info;
    ptoc_Var25->a4sh_kind = 36;
    a40get_catalog_table(&(*acv) , &(*a41v));
    if ((int ) ptoc_Var24->a_ex_kind != 0 && (int ) (*(tsp1_segment *) sql__nil(ptoc_Var24->a_return_segm , "vak42.p" , 18077)).integer.C_4.sp1r_returncode == 0)
        {
        ak42get_refinfo(&(*acv) , &info);
        switch (kw_index)
            {
            case 12 :
                if (g01is_livecache())
                    {
                    ind = 0;
                    do
                        {
                        ak42out_monitor(&(*acv) , &(*a41v) , &info , (unsigned char ) sql__setrang(ind , 0 , 176));
                        if (ind == 23)
                            {
                            break;
                            }
                        (unsigned char ) sql__setrang(ind += 1 , 0 , 176);
                        }
                    while (!0);
                    }
                else
                    {
                    ind = 0;
                    do
                        {
                        ak42out_monitor(&(*acv) , &(*a41v) , &info , (unsigned char ) sql__setrang(ind , 0 , 176));
                        if (ind == 15)
                            {
                            break;
                            }
                        (unsigned char ) sql__setrang(ind += 1 , 0 , 176);
                        }
                    while (!0);
                    }
                ind = 24;
                do
                    {
                    ak42out_monitor(&(*acv) , &(*a41v) , &info , (unsigned char ) sql__setrang(ind , 0 , 176));
                    if (ind == 27)
                        {
                        break;
                        }
                    (unsigned char ) sql__setrang(ind += 1 , 0 , 176);
                    }
                while (!0);
                ind = 28;
                do
                    {
                    if (g01is_livecache() || (int ) ind != 53 && (int ) ind != 55)
                        {
                        ak42out_monitor(&(*acv) , &(*a41v) , &info , (unsigned char ) sql__setrang(ind , 0 , 176));
                        }
                    if (ind == 175)
                        {
                        break;
                        }
                    (unsigned char ) sql__setrang(ind += 1 , 0 , 176);
                    }
                while (!0);
                break;
            case 48 :
                if (g01is_livecache())
                    {
                    ind = 0;
                    do
                        {
                        ak42out_monitor(&(*acv) , &(*a41v) , &info , (unsigned char ) sql__setrang(ind , 0 , 176));
                        if (ind == 23)
                            {
                            break;
                            }
                        (unsigned char ) sql__setrang(ind += 1 , 0 , 176);
                        }
                    while (!0);
                    }
                else
                    {
                    ind = 0;
                    do
                        {
                        ak42out_monitor(&(*acv) , &(*a41v) , &info , (unsigned char ) sql__setrang(ind , 0 , 176));
                        if (ind == 15)
                            {
                            break;
                            }
                        (unsigned char ) sql__setrang(ind += 1 , 0 , 176);
                        }
                    while (!0);
                    }
                ind = 24;
                do
                    {
                    ak42out_monitor(&(*acv) , &(*a41v) , &info , (unsigned char ) sql__setrang(ind , 0 , 176));
                    if (ind == 27)
                        {
                        break;
                        }
                    (unsigned char ) sql__setrang(ind += 1 , 0 , 176);
                    }
                while (!0);
                break;
            case 250 :
                ind = 28;
                do
                    {
                    if (g01is_livecache() || (int ) ind != 53 && (int ) ind != 55)
                        {
                        ak42out_monitor(&(*acv) , &(*a41v) , &info , (unsigned char ) sql__setrang(ind , 0 , 176));
                        }
                    if (ind == 83)
                        {
                        break;
                        }
                    (unsigned char ) sql__setrang(ind += 1 , 0 , 176);
                    }
                while (!0);
                break;
            case 253 :
                ind = 84;
                do
                    {
                    ak42out_monitor(&(*acv) , &(*a41v) , &info , (unsigned char ) sql__setrang(ind , 0 , 176));
                    if (ind == 91)
                        {
                        break;
                        }
                    (unsigned char ) sql__setrang(ind += 1 , 0 , 176);
                    }
                while (!0);
                break;
            case 254 :
                ind = 92;
                do
                    {
                    ak42out_monitor(&(*acv) , &(*a41v) , &info , (unsigned char ) sql__setrang(ind , 0 , 176));
                    if (ind == 101)
                        {
                        break;
                        }
                    (unsigned char ) sql__setrang(ind += 1 , 0 , 176);
                    }
                while (!0);
                break;
            case 258 :
                ind = 102;
                do
                    {
                    ak42out_monitor(&(*acv) , &(*a41v) , &info , (unsigned char ) sql__setrang(ind , 0 , 176));
                    if (ind == 103)
                        {
                        break;
                        }
                    (unsigned char ) sql__setrang(ind += 1 , 0 , 176);
                    }
                while (!0);
                break;
            case 340 :
                ind = 104;
                do
                    {
                    ak42out_monitor(&(*acv) , &(*a41v) , &info , (unsigned char ) sql__setrang(ind , 0 , 176));
                    if (ind == 151)
                        {
                        break;
                        }
                    (unsigned char ) sql__setrang(ind += 1 , 0 , 176);
                    }
                while (!0);
                break;
            case 394 :
                ind = 152;
                do
                    {
                    ak42out_monitor(&(*acv) , &(*a41v) , &info , (unsigned char ) sql__setrang(ind , 0 , 176));
                    if (ind == 172)
                        {
                        break;
                        }
                    (unsigned char ) sql__setrang(ind += 1 , 0 , 176);
                    }
                while (!0);
                break;
            case 480 :
                ind = 173;
                do
                    {
                    ak42out_monitor(&(*acv) , &(*a41v) , &info , (unsigned char ) sql__setrang(ind , 0 , 176));
                    if (ind == 175)
                        {
                        break;
                        }
                    (unsigned char ) sql__setrang(ind += 1 , 0 , 176);
                    }
                while (!0);
                break;
            default :
                a07_b_put_error(&(*acv) , 3400 , 1);
                break;
            }
        }
    cccprint("<ak42monitor_sho");
    }



int a42_monitor_init(process_id)
tsp00_TaskId process_id;
    {
    cccprint("AK42  a42_monito");
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[0], "Data cache accesses                     ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[1], "Data cache accesses successful          ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[2], "Data cache accesses unsuccessful        ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[3], "Data cache hit rate (%)                 ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[4], "Catalog cache accesses                  ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[5], "Catalog cache accesses successful       ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[6], "Catalog cache accesses unsuccessful     ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[7], "Catalog cache hit rate (%)              ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[8], "Sequence cache accesses                 ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[9], "Sequence cache accesses successful      ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[10], "Sequence cache accesses unsuccessful    ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[11], "Sequence cache hit rate (%)             ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[12], "Data cache sqlpage accesses             ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[13], "Data cache sqlpage accesses successful  ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[14], "Data cache sqlpage accesses unsuccessful")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[15], "Data cache sqlpage hit rate(%)          ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[16], "Data cache OMS accesses                 ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[17], "Data cache OMS accesses successful      ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[18], "Data cache OMS accesses unsuccessful    ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[19], "Data cache OMS hit rate (%)             ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[20], "Data cache OMS log accesses             ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[21], "Data cache OMS log accesses successful  ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[22], "Data cache OMS log accesses unsuccessful")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[23], "Data cache OMS log hit rate (%)         ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[24], "Data history/undo accesses              ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[25], "Data history/undo accesses successful   ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[26], "Data history/undo accesses unsuccessful ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[27], "Data history/undo hit rate (%)          ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[28], "SQL commands                            ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[29], "PREPAREs                                ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[30], "PREPAREs repeated                       ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[31], "EXECUTEs                                ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[32], "COMMITs                                 ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[33], "ROLLBACKs                               ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[34], "LOCKs and UNLOCKs                       ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[35], "SUBTRANS BEGINs                         ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[36], "SUBTRANS ENDs                           ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[37], "SUBTRANS ROLLBACKs                      ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[38], "CREATEs                                 ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[39], "ALTERs                                  ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[40], "DROPs                                   ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[41], "SELECTs and FETCHes                     ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[42], "SELECTs and FETCHes, rows read          ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[43], "SELECTs and FETCHes, rows qual          ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[44], "INSERTs                                 ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[45], "INSERTs, rows inserted                  ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[46], "UPDATEs                                 ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[47], "UPDATEs, rows read                      ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[48], "UPDATEs, rows updated                   ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[49], "DELETEs                                 ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[50], "DELETEs, rows read                      ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[51], "DELETEs, rows deleted                   ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[52], "Internal DBPROC calls                   ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[53], "External DBPROC calls                   ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[54], "Internal trigger calls                  ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[55], "External trigger calls                  ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[56], "Primary key accesses                    ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[57], "Primary key accesses, rows read         ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[58], "Primary key accesses, rows qual         ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[59], "Primary key range accesses              ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[60], "Primary key range accesses, rows read   ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[61], "Primary key range accesses, rows qual   ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[62], "Index accesses                          ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[63], "Index accesses, rows read               ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[64], "Index accesses, rows qual               ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[65], "Index range accesses                    ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[66], "Index range accesses, rows read         ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[67], "Index range accesses, rows qual         ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[68], "Isolated index accesses                 ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[69], "Isolated index accesses, rows read      ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[70], "Isolated index accesses, rows qual      ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[71], "Isolated index range accesses           ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[72], "Isolated index range accesses, rows read")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[73], "Isolated index range accesses, rows qual")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[74], "Table scans                             ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[75], "Table scans, rows read                  ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[76], "Table scans, rows qual                  ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[77], "Isolated index scans                    ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[78], "Isolated index scans, rows read         ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[79], "Isolated index scans, rows qual         ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[80], "Memory sorts / sort&merge               ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[81], "Memory sorts / sort&merge, rows read    ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[82], "Sorts by insertion                      ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[83], "Sorts by insertion, rows inserted       ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[84], "Lock list avg used entries              ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[85], "Lock list max used entries              ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[86], "Lock list collisions                    ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[87], "Lock list escalations                   ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[88], "Lock list inserted row   entries        ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[89], "Lock list inserted table entries        ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[90], "Detected deadlocks                      ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[91], "Request timeouts                        ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[92], "Log page physical reads                 ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[93], "Log page physical writes                ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[94], "Log queue pages                         ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[95], "Log queue max used pages                ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[96], "Log queue inserts                       ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[97], "Log queue overflows                     ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[98], "Log queue group commits                 ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[99], "Log queue waits for log page write      ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[100], "Log queue max waits per log page        ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[101], "Log queue avg waits per log page        ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[102], "BD read   LONG                          ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[103], "BD write  LONG                          ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[104], "Virtual  reads                          ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[105], "Virtual  writes                         ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[106], "Physical reads                          ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[107], "Physical writes                         ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[108], "Catalog        virtual  reads           ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[109], "Catalog        virtual  writes          ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[110], "Catalog        physical reads           ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[111], "Catalog        physical writes          ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[112], "Perm page      virtual  reads           ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[113], "Perm page      virtual  writes          ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[114], "Perm page      physical reads           ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[115], "Perm page      physical writes          ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[116], "Temp page      virtual  reads           ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[117], "Temp page      virtual  writes          ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[118], "Temp page      physical reads           ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[119], "Temp page      physical writes          ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[120], "LONG page      virtual  reads           ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[121], "LONG page      virtual  writes          ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[122], "LONG page      physical reads           ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[123], "LONG page      physical writes          ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[124], "Leaf page      virtual  reads           ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[125], "Leaf page      virtual  writes          ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[126], "Leaf page      physical reads           ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[127], "Leaf page      physical writes          ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[128], "Level1 page    virtual  reads           ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[129], "Level1 page    virtual  writes          ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[130], "Level1 page    physical reads           ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[131], "Level1 page    physical writes          ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[132], "Level2 page    virtual  reads           ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[133], "Level2 page    virtual  writes          ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[134], "Level2 page    physical reads           ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[135], "Level2 page    physical writes          ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[136], "Level3 page    virtual  reads           ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[137], "Level3 page    virtual  writes          ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[138], "Level3 page    physical reads           ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[139], "Level3 page    physical writes          ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[140], "Level4 page    virtual  reads           ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[141], "Level4 page    virtual  writes          ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[142], "Level4 page    physical reads           ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[143], "Level4 page    physical writes          ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[144], "Level5 page    virtual  reads           ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[145], "Level5 page    virtual  writes          ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[146], "Level5 page    physical reads           ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[147], "Level5 page    physical writes          ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[148], "Converter page virtual  reads           ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[149], "Converter page virtual  writes          ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[150], "Converter page physical reads           ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[151], "Converter page physical writes          ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[152], "BD add  record perm                     ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[153], "BD add  record temp                     ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[154], "BD repl record perm                     ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[155], "BD repl record temp                     ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[156], "BD del  record perm                     ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[157], "BD del  record temp                     ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[158], "BD get  record perm                     ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[159], "BD get  record temp                     ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[160], "BD next record perm                     ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[161], "BD next record temp                     ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[162], "BD prev record perm                     ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[163], "BD prev record temp                     ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[164], "BD select direct record                 ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[165], "BD select next   record                 ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[166], "BD select prev   record                 ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[167], "BD add to   index list perm             ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[168], "BD add to   index list temp             ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[169], "BD del from index list perm             ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[170], "BD del from index list temp             ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[171], "BD get      index list perm             ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[172], "BD get      index list temp             ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[173], "SQL commands                            ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[174], "Write transactions                      ")
    _ptoc_ASSIGN(unsigned char , 40, a42monitor_arr[175], "KB calls                                ")
    ak42refresh_cnt = 0;
    ak42fast_refresh_cnt = 0;
    ak42init_running_cmd_list();
    a42monitor_init(process_id);
    cccprint("<a42_monitor_ini");
    }

typedef
  int tsp_int4_ak42monitor_oms;
typedef
  struct ptoc_Type207
    {
    tgg00_MessType ctbMessType_gg00;
    tsp00_Int4 ctbDispId_gg00;
    tgg00_COMMonitorInfo ctbMonInfo_gg00;
    } tgg00_COMMonitorTransferBuf_ak42monitor_oms;
typedef
  struct ptoc_Type208
    {
    tsp_int4_ak42monitor_oms *plong;
    tgg00_COMMonitorTransferBuf_ak42monitor_oms *pBuf;
    } I386Stack_ak42monitor_oms;


int ak42monitor_oms(acv, a41v, reset_statistics)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
char reset_statistics;
    {
    tsp00_Addr handle;
    tsp00_C16 iid;
    tsp00_Int4 dispid;
    tsp_dcom_dispatch_parms dp;
    tsp_int4_ak42monitor_oms bufLen;
    tgg00_COMMonitorTransferBuf_ak42monitor_oms buf;
    struct ptoc_Type209
      {
      tak_dbproc_stack_param dbg_value;
      tsp_int2 dbg_vt;
      unsigned char dbg_inout;
      char dbg_is_ptr;
      tsp_int4 dbg_length;
      } stack[2];
    I386Stack_ak42monitor_oms *pStack;
    tsp_dcom_monitor_parms params;
    cccprint("AK42  ak42monito");
    if (!(char ) sql__setrang(reset_statistics , 0 , 1))
        {
        a40get_catalog_table(&(*acv) , &(*a41v));
        if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak42.p" , 18641)).integer.C_4.sp1r_returncode == 0)
            {
            handle = (unsigned char *) (int *) 0;
            while (ak341GetMonitorInfo(&(*acv) , &handle , iid , &dispid , &buf.ctbMonInfo_gg00))
                ak42AggrMethodMonitorInfo(&(*acv) , iid , &dispid , &buf.ctbMonInfo_gg00);
            }
        }
    if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak42.p" , 18654)).integer.C_4.sp1r_returncode == 0)
        {
        dp.ifactory = (unsigned char *) (int *) 0;
        dp.session_context = (unsigned char *) &*acv;
        dp.dispid = 5;
        dp.hresult = 0;
        dp.param_stack = (tsp00_MoveObj *) stack;
        dp.param_stack_len = sizeof(struct ptoc_Type209 [2]);
        _ptoc_ASSIGN(unsigned char , 64, dp.errtext, "                                                                ")
        dp.monitoring = 0;
        buf.ctbMessType_gg00 = 22;
        if (a260BuildDebugStack())
            {
            stack[0].dbg_value.integer.C_7.ptr = (tsp00_MoveObj *) &bufLen;
            stack[0].dbg_vt = 3;
            stack[0].dbg_inout = 2;
            stack[0].dbg_is_ptr = 1;
            stack[0].dbg_length = sizeof(int );
            stack[1].dbg_value.integer.C_7.ptr = (tsp00_MoveObj *) &buf;
            stack[1].dbg_vt = 29;
            stack[1].dbg_inout = 2;
            stack[1].dbg_is_ptr = 1;
            stack[1].dbg_length = sizeof(struct ptoc_Type207 );
            }
        else
            {
            pStack = (I386Stack_ak42monitor_oms *) stack;
            (*(I386Stack_ak42monitor_oms *) sql__nil(pStack , "vak42.p" , 18685)).plong = (tsp_int4_ak42monitor_oms *) &bufLen;
            (*(I386Stack_ak42monitor_oms *) sql__nil(pStack , "vak42.p" , 18686)).pBuf = (tgg00_COMMonitorTransferBuf_ak42monitor_oms *) &buf;
            }
        vdcom_monitor_open(&params);
        do
            if (params.retcode == 0)
                {
                dp.idispatch = params.idispatch;
                if ((char ) sql__setrang(reset_statistics , 0 , 1))
                    {
                    buf.ctbDispId_gg00 = -1;
                    }
                else
                    {
                    buf.ctbDispId_gg00 = 0;
                    }
                do
                    {
                    bufLen = sizeof(struct ptoc_Type207 ) - sizeof(tgg00_COMMonitorInfo );
                    vdcom_dbproc(&dp);
                    if (!(char ) sql__setrang(reset_statistics , 0 , 1) && buf.ctbDispId_gg00 >= 0 && bufLen > 0)
                        {
                        ak42AggrMethodMonitorInfo(&(*acv) , params.iid , &buf.ctbDispId_gg00 , &buf.ctbMonInfo_gg00);
                        }
                    if (buf.ctbDispId_gg00 >= 0)
                        {
                        buf.ctbDispId_gg00 = buf.ctbDispId_gg00 + 1;
                        }
                    }
                while (!(buf.ctbDispId_gg00 < 0 || (char ) sql__setrang(reset_statistics , 0 , 1) || dp.hresult != 0));
                vdcom_monitor_next(&params);
                if (dp.hresult != 0)
                    {
                    a07ak_system_error(&(*acv) , 42 , 1);
                    }
                }
        while (!(params.retcode != 0));
        vdcom_monitor_close(&params);
        if ((char ) sql__setrang(reset_statistics , 0 , 1))
            {
            ak341ResetMonitor(&(*acv));
            }
        else
            {
            ak42MonitorOmsBuildUserResult(&(*acv) , &(*a41v));
            b01empty_file(&(*acv).a_transinf.tri_trans , &(*acv).a_into_tree);
            }
        }
    cccprint("<ak42monitor_oms");
    }



int ak42AggrMethodMonitorInfo(acv, iid, dispid, monInfo)
tak_all_command_glob *acv;
tsp00_C16 iid;
int *dispid;
tgg00_COMMonitorInfo *monInfo;
    {
    char exitLoop;
    tgg00_COMMonEnum monEnum;
    tak42_monitor_rec_key key;
    tak42_oms_monitor_rec monitor_rec;
    cccprint("AK42  ak42AggrMe");
    _ptoc_ASSIGN(unsigned char , 16, key.boolean.C_false.iid, iid)
    key.boolean.C_false.dispid = *dispid;
    key.boolean.C_true.lkey.integer.C_1.keyLen_gg00 = (short ) sql__setrang((short ) (sizeof(tsp00_C16 ) + sizeof(int )) , -32768 , 32767);
    b07cget_record(&(*acv).a_transinf.tri_trans , &(*acv).a_into_tree , &key.boolean.C_true.lkey , &monitor_rec.integer.C_2.rec);
    if ((int ) (*acv).a_transinf.tri_trans.trError_gg00 == 1200)
        {
        monitor_rec.integer.C_2.rec.integer.C_2.recLen_gg00 = (short ) sql__setrang((short ) (sizeof(unsigned char [8]) + sizeof(tsp00_C16 ) + sizeof(int ) + sizeof(int ) + sizeof(tgg00_COMMonitorInfo )) , -32768 , 32767);
        monitor_rec.integer.C_2.rec.integer.C_2.recKeyLen_gg00 = (short ) sql__setrang((short ) (sizeof(tsp00_C16 ) + sizeof(int )) , -32768 , 32767);
        monitor_rec.integer.C_2.rec.integer.C_2.recVarcolOffset_gg00 = 0;
        monitor_rec.integer.C_2.rec.integer.C_2.recVarcolCnt_gg00 = 0;
        _ptoc_ASSIGN(unsigned char , 16, monitor_rec.integer.C_3.iid, iid)
        monitor_rec.integer.C_3.dispid = *dispid;
        monitor_rec.integer.C_3.info = *monInfo;
        b07cadd_record(&(*acv).a_transinf.tri_trans , &(*acv).a_into_tree , &monitor_rec.integer.C_2.rec);
        }
    else
        {
        monitor_rec.integer.C_3.info.cmiCallCnt_gg00 = monitor_rec.integer.C_3.info.cmiCallCnt_gg00 + (*monInfo).cmiCallCnt_gg00;
        monEnum = 0;
        exitLoop = 0;
        while (!(char ) sql__setrang(exitLoop , 0 , 1))
            {
            switch ((int ) monEnum)
                {
                case 1 :
                case 35 :
                case 38 :
                case 42 :
                case 46 :
                case 50 :
                case 53 :
                case 56 :
                case 59 :
                    if ((*monInfo).cmiCounters_gg00[sql__setrang((int ) monEnum , 0 , 62)] < monitor_rec.integer.C_3.info.cmiCounters_gg00[sql__setrang((int ) monEnum , 0 , 62)])
                        {
                        monitor_rec.integer.C_3.info.cmiCounters_gg00[sql__setrang((int ) monEnum , 0 , 62)] = (*monInfo).cmiCounters_gg00[sql__setrang((int ) monEnum , 0 , 62)];
                        }
                    break;
                case 2 :
                case 20 :
                case 34 :
                case 39 :
                case 43 :
                case 62 :
                case 47 :
                case 51 :
                case 54 :
                case 57 :
                case 60 :
                    if ((*monInfo).cmiCounters_gg00[sql__setrang((int ) monEnum , 0 , 62)] > monitor_rec.integer.C_3.info.cmiCounters_gg00[sql__setrang((int ) monEnum , 0 , 62)])
                        {
                        monitor_rec.integer.C_3.info.cmiCounters_gg00[sql__setrang((int ) monEnum , 0 , 62)] = (*monInfo).cmiCounters_gg00[sql__setrang((int ) monEnum , 0 , 62)];
                        }
                    break;
                case 48 :
                    monitor_rec.integer.C_3.info.cmiCounters_gg00[48] = (monitor_rec.integer.C_3.info.cmiCounters_gg00[48] * (monitor_rec.integer.C_3.info.cmiCallCnt_gg00 - (*monInfo).cmiCallCnt_gg00) + (*monInfo).cmiCounters_gg00[48]) / monitor_rec.integer.C_3.info.cmiCallCnt_gg00;
                    break;
                default :
                    monitor_rec.integer.C_3.info.cmiCounters_gg00[sql__setrang((int ) monEnum , 0 , 62)] = monitor_rec.integer.C_3.info.cmiCounters_gg00[sql__setrang((int ) monEnum , 0 , 62)] + (*monInfo).cmiCounters_gg00[sql__setrang((int ) monEnum , 0 , 62)];
                    break;
                }
            if ((int ) monEnum < 62)
                {
                monEnum = (unsigned char ) sql__setrang((unsigned char ) sql__succ((int ) monEnum , 0 , 62 , "vak42.p" , 18827) , 0 , 62);
                }
            else
                {
                exitLoop = 1;
                }
            }
        b07crepl_record(&(*acv).a_transinf.tri_trans , &(*acv).a_into_tree , &monitor_rec.integer.C_2.rec);
        }
    if ((int ) (*acv).a_transinf.tri_trans.trError_gg00 != 0)
        {
        a07_b_put_error(&(*acv) , (short ) sql__setrang((*acv).a_transinf.tri_trans.trError_gg00 , -32768 , 32767) , 1);
        }
    cccprint("<ak42AggrMethodM");
    }

typedef
  struct ptoc_Type27 tgg_lkey_ak42MonitorOmsBuildUserResult;


int ak42MonitorOmsBuildUserResult(acv, a41v)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
    {
    tgg00_COMMonEnum monEnum;
    tgg00_BasisError b_err;
    tsp00_8ByteCounter avg;
    tak42_oms_monitor_rec monitor_rec;
    tgg_bd_set_result_record set_result;
    tgg00_FilePos tree_pos;
    tgg_lkey_ak42MonitorOmsBuildUserResult *zerokey;
    tgg00_BdSetResultRecord *ptoc_Var27;
    int ptoc_Var28;
    cccprint("AK42  ak42Monito");
    if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak42.p" , 18861)).integer.C_4.sp1r_returncode == 0)
        {
        ptoc_Var27 = &set_result;
        ptoc_Var27->bd_key_check_len = 0;
        ptoc_Var27->bd_max_rec_cnt = 1;
        ptoc_Var27->bd_max_fill_len = sizeof(tak42_oms_monitor_rec );
        ptoc_Var27->bd_next = 1;
        zerokey = (tgg_lkey_ak42MonitorOmsBuildUserResult *) &monitor_rec;
        (*(tgg00_Lkey *) sql__nil(zerokey , "vak42.p" , 18873)).integer.C_11.len = 0;
        tree_pos.tpsPno_gg00 = 2147483647;
        do
            {
            b07cnext_record(&(*acv).a_transinf.tri_trans , &(*acv).a_into_tree , sql__nil(zerokey , "vak42.p" , 18877) , &set_result , &tree_pos , monitor_rec.integer.C_2.rec.integer.C_1.recBuf_gg00);
            b_err = (short ) sql__setrang((*acv).a_transinf.tri_trans.trError_gg00 , -32768 , 32767);
            if ((int ) b_err == 0 || (int ) b_err == 1200)
                {
                b_err = 0;
                a40move_guid(&(*acv) , &(*a41v) , monitor_rec.integer.C_3.iid);
                a40move_i4(&(*acv) , &(*a41v) , monitor_rec.integer.C_3.dispid , 0 , 0 , 0);
                a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) monitor_rec.integer.C_3.info.cmiMethodName_gg00 , sizeof(tsp00_C64 ));
                a40move_counter(&(*acv) , &(*a41v) , monitor_rec.integer.C_3.info.cmiCallCnt_gg00 , 0 , 0 , 0);
                monEnum = 0;
                while ((int ) monEnum < 62)
                    {
                    if ((ptoc_Var28 = (int ) monEnum) == 23 || ptoc_Var28 == 36)
                        {
                        if (monitor_rec.integer.C_3.info.cmiCounters_gg00[sql__setrang((int ) (unsigned char ) sql__succ((int ) monEnum , 0 , 62 , "vak42.p" , 18896) , 0 , 62)] > (double ) 0.00000000000000000000e+000)
                            {
                            avg = (double ) sql__trunc(monitor_rec.integer.C_3.info.cmiCounters_gg00[sql__setrang((int ) monEnum , 0 , 62)] / monitor_rec.integer.C_3.info.cmiCounters_gg00[sql__setrang((int ) (unsigned char ) sql__succ((int ) monEnum , 0 , 62 , "vak42.p" , 18900) , 0 , 62)]);
                            a40move_counter(&(*acv) , &(*a41v) , avg , 0 , 0 , 0);
                            }
                        else
                            {
                            a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) (int *) 0 , 0);
                            }
                        monEnum = (unsigned char ) sql__setrang((unsigned char ) sql__succ((int ) monEnum , 0 , 62 , "vak42.p" , 18906) , 0 , 62);
                        }
                    else
                        {
                        if ((ptoc_Var28 = (int ) monEnum) == 52 || ptoc_Var28 == 55 || ptoc_Var28 == 58 || ptoc_Var28 == 61)
                            {
                            if (monitor_rec.integer.C_3.info.cmiCallCnt_gg00 > (double ) 0.00000000000000000000e+000)
                                {
                                avg = (double ) sql__trunc(monitor_rec.integer.C_3.info.cmiCounters_gg00[sql__setrang((int ) monEnum , 0 , 62)] / monitor_rec.integer.C_3.info.cmiCallCnt_gg00);
                                a40move_counter(&(*acv) , &(*a41v) , avg , 0 , 0 , 0);
                                }
                            else
                                {
                                a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) (int *) 0 , 0);
                                }
                            }
                        else
                            {
                            a40move_counter(&(*acv) , &(*a41v) , monitor_rec.integer.C_3.info.cmiCounters_gg00[sql__setrang((int ) monEnum , 0 , 62)] , (char ) sql__setrang(monitor_rec.integer.C_3.info.cmiCounters_gg00[sql__setrang((int ) monEnum , 0 , 62)] == (double ) 2.14748364700000000000e+009 , 0 , 1) , 0 , 0);
                            }
                        }
                    monEnum = (unsigned char ) sql__setrang((unsigned char ) sql__succ((int ) monEnum , 0 , 62 , "vak42.p" , 18930) , 0 , 62);
                    }
                a40move_counter(&(*acv) , &(*a41v) , monitor_rec.integer.C_3.info.cmiCounters_gg00[62] , (char ) sql__setrang(monitor_rec.integer.C_3.info.cmiCounters_gg00[62] < (double ) 1.00000000000000000000e+000 , 0 , 1) , 0 , 0);
                }
            }
        while (!((int ) b_err != 0 || (int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak42.p" , 18939)).integer.C_4.sp1r_returncode != 0));
        }
    cccprint("<ak42MonitorOmsB");
    }



int ak42ResetOmsMonitor(acv)
tak_all_command_glob *acv;
    {
    tak40_show_glob a41v;
    cccprint("AK42  ak42ResetO");
    ak42monitor_oms(&(*acv) , &a41v , 1);
    cccprint("<ak42ResetOmsMon");
    }



int ak42init_running_cmd_list()
    {
    char ok;
    tsp00_Int4 alloc_sum;
    cccprint("AK42  ak42init_r");
    alloc_sum = g01maxuser() * sizeof(tak_parsid );
    g01allocate_msg("dynpool " , "DYNP_A42_PARSEID       :" , alloc_sum);
    vallocat(alloc_sum , &a42parseid_list , &ok);
    if ((char ) sql__setrang(ok , 0 , 1))
        {
        s10fil2(alloc_sum , sql__nil(a42parseid_list , "vak42.p" , 18981) , 1 , alloc_sum , (unsigned char)'\0');
        }
    cccprint("<ak42init_runnin");
    }



int a42put_running_cmds(acv, parsid)
tak_all_command_glob *acv;
tsp_c12 parsid;
    {
    int pidpos;
    int first_user_pid;
    tak_all_command_glob *ptoc_Var29;
    cccprint("AK42  a42put_run");
    ptoc_Var29 = &*acv;
    vgetfirstuserpid(&first_user_pid);
    if (ptoc_Var29->a_transinf.tri_trans.trTaskId_gg00 >= first_user_pid && ptoc_Var29->a_transinf.tri_trans.trTaskId_gg00 <= first_user_pid + g01maxuser())
        {
        pidpos = (ptoc_Var29->a_transinf.tri_trans.trTaskId_gg00 - first_user_pid) * sizeof(tsp00_C12 ) + 1;
        g10mv7("VAK42 " , 2 , sizeof(tsp00_C12 ) , g01maxuser() * sizeof(tsp00_C12 ) , parsid , 1 , sql__nil(a42parseid_list , "vak42.p" , 19013) , pidpos , sizeof(tsp00_C12 ) , &(*(tsp1_segment *) sql__nil(ptoc_Var29->a_return_segm , "vak42.p" , 19014)).integer.C_4.sp1r_returncode);
        }
    cccprint("<a42put_running_");
    }



char a42getparseid(t, pid, parsid, b_err)
tgg00_TransContext *t;
tsp_process_id *pid;
tsp_c12 parsid;
tgg00_BasisError *b_err;
    {
    char _a42getparseid;
    int pidpos;
    int first_user_pid;
    cccprint("AK42  a42getpars");
    _a42getparseid = 0;
    vgetfirstuserpid(&first_user_pid);
    if (*pid >= first_user_pid && *pid <= first_user_pid + g01maxuser())
        {
        pidpos = (*pid - first_user_pid) * sizeof(tsp00_C12 ) + 1;
        g10mv8("VAK42 " , 3 , g01maxuser() * sizeof(tsp00_C12 ) , sizeof(tsp00_C12 ) , sql__nil(a42parseid_list , "vak42.p" , 19044) , pidpos , parsid , 1 , sizeof(tsp00_C12 ) , &(*b_err));
        if (_ptoc_MEMCMP(unsigned char , 12, parsid , "\0\0\0\0\0\0\0\0\0\0\0\0") == 0 ? 0 : 1)
            {
            parsid[10] = (unsigned char)'\0';
            _a42getparseid = 1;
            }
        }
    cccprint("<a42getparseid  ");
    return(_a42getparseid);
    }



int a42reset_running_cmds(acv)
tak_all_command_glob *acv;
    {
    int pidpos;
    tsp_c12 parsid;
    int first_user_pid;
    tak_all_command_glob *ptoc_Var30;
    cccprint("AK42  a42reset_r");
    ptoc_Var30 = &*acv;
    _ptoc_ASSIGN(unsigned char , 12, parsid, "\0\0\0\0\0\0\0\0\0\0\0\0")
    vgetfirstuserpid(&first_user_pid);
    if (ptoc_Var30->a_transinf.tri_trans.trTaskId_gg00 >= first_user_pid && ptoc_Var30->a_transinf.tri_trans.trTaskId_gg00 <= first_user_pid + g01maxuser())
        {
        pidpos = (ptoc_Var30->a_transinf.tri_trans.trTaskId_gg00 - first_user_pid) * sizeof(tsp00_C12 ) + 1;
        g10mv7("VAK42 " , 4 , sizeof(tsp00_C12 ) , g01maxuser() * sizeof(tsp00_C12 ) , parsid , 1 , sql__nil(a42parseid_list , "vak42.p" , 19087) , pidpos , sizeof(tsp00_C12 ) , &(*(tsp1_segment *) sql__nil(ptoc_Var30->a_return_segm , "vak42.p" , 19088)).integer.C_4.sp1r_returncode);
        }
    cccprint("<a42reset_runnin");
    }



int ak42oms_memory(acv, a41v)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
    {
    tsp00_8ByteCounter totalHeap;
    tsp00_8ByteCounter spaceInFreeChunks;
    tsp00_8ByteCounter reserved;
    tsp00_8ByteCounter mallocated;
    tsp00_8ByteCounter emergencySize;
    tsp00_8ByteCounter emergencyInUse;
    tsp00_8ByteCounter emergencyMaxUsed;
    cccprint("AK42  ak42oms_me");
    a40get_catalog_table(&(*acv) , &(*a41v));
    if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak42.p" , 19115)).integer.C_4.sp1r_returncode == 0 && (int ) (*acv).a_ex_kind != 0 && g01is_livecache())
        {
        ak341OmsHeapInfo(&(*acv) , &totalHeap , &spaceInFreeChunks , &reserved , &mallocated , &emergencySize , &emergencyInUse , &emergencyMaxUsed);
        a40move_counter(&(*acv) , &(*a41v) , totalHeap , 0 , 0 , 0);
        a40move_counter(&(*acv) , &(*a41v) , spaceInFreeChunks , 0 , 0 , 0);
        a40move_counter(&(*acv) , &(*a41v) , reserved , 0 , 0 , 0);
        a40move_counter(&(*acv) , &(*a41v) , mallocated , 0 , 0 , 0);
        a40move_counter(&(*acv) , &(*a41v) , emergencySize , 0 , 0 , 0);
        a40move_counter(&(*acv) , &(*a41v) , emergencyInUse , 0 , 0 , 0);
        a40move_counter(&(*acv) , &(*a41v) , emergencyMaxUsed , 0 , 0 , 0);
        }
    cccprint("<ak42oms_memory ");
    }



int ak42OmsLockObjInfo(acv, a41v)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
    {
    char exitLoop;
    tsp00_Addr handle;
    tgg00_OmsLockInfo lockInfo;
    cccprint("AK42  ak42OmsLoc");
    a40get_catalog_table(&(*acv) , &(*a41v));
    if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak42.p" , 19149)).integer.C_4.sp1r_returncode == 0 && (int ) (*acv).a_ex_kind != 0)
        {
        handle = (unsigned char *) (int *) 0;
        exitLoop = 0;
        while (!(char ) sql__setrang(exitLoop , 0 , 1) && ak341NextOmsLockObjInfo(&handle , &lockInfo))
            {
            a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) lockInfo.oli_handle , sizeof(tsp00_C8 ));
            a40move_i4(&(*acv) , &(*a41v) , lockInfo.oli_taskid , 0 , 0 , 0);
            a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) lockInfo.oli_lockmode , sizeof(tsp00_C12 ));
            a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) lockInfo.oli_requestmode , sizeof(tsp00_C12 ));
            a40move_i4(&(*acv) , &(*a41v) , lockInfo.oli_timeout , (char ) sql__setrang(lockInfo.oli_timeout < 0 , 0 , 1) , 0 , 0);
            if (handle == (unsigned char *) (int *) 0)
                {
                exitLoop = 1;
                }
            }
        }
    cccprint("<ak42OmsLockObjI");
    }

static
  unsigned char ptoc_Var31[1] =
  {
  (unsigned char)'\020'
  };


int ak42show_running_cmds(acv, a41v)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
    {
    tgg00_BasisError b_err;
    tgg00_FileId aux_file;
    struct tgg00_MessBlock mblock;
    tsp_buf data;
    tgg_qual_buf qual;
    tak_all_command_glob *ptoc_Var32;
    tak40_show_glob *ptoc_Var33;
    unsigned char ptoc_Var34[1];
    unsigned char ptoc_Var35[1];
    unsigned char ptoc_Var36[1];
    cccprint("AK42  ak42show_r");
    ptoc_Var32 = &*acv;
    ptoc_Var33 = &*a41v;
    a40get_catalog_table(&(*acv) , &(*a41v));
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var32->a_return_segm , "vak42.p" , 19192)).integer.C_4.sp1r_returncode == 0 && (int ) ptoc_Var32->a_ex_kind != 0)
        {
        g04build_temp_tree_id(&aux_file , &ptoc_Var32->a_transinf.tri_trans);
        aux_file.integer.C_8.fileTempCnt_gg00 = 1;
        aux_file.integer.C_8.fileTfnTemp_gg00 = 17;
        _ptoc_MOVE(unsigned char , 1, aux_file.integer.C_1.fileType_gg00, *(unsigned char (*)[1]) sql__psetadd(ptoc_Var34 , aux_file.integer.C_1.fileType_gg00 , ptoc_Var31 , sizeof(unsigned char [1])));
        g01mblock_init(&(*acv).a_transinf.tri_trans , 63 , 27 , &mblock);
        g01datapart_init(&mblock , (tgg00_DataPart *) data , sizeof(tsp00_Buf ));
        g01stack_init(&mblock , (tgg00_StackList *) (int *) 0 , 0 , (tgg00_QualBuf *) &qual , sizeof(tgg00_QualBuf ));
        (*(tgg00_QualBuf *) sql__nil(mblock.integer.C_4.mb_qual , "vak42.p" , 19203)).integer.C_2.mtree = aux_file;
        a06rsend_mess_buf(&(*acv) , &mblock , 1 , &b_err);
        if ((int ) b_err != 0)
            {
            a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , 1);
            }
        else
            {
            ak42add_parseid(&(*acv) , &(*a41v) , &aux_file);
            }
        b01destroy_file(&ptoc_Var32->a_transinf.tri_trans , &aux_file);
        }
    cccprint("<ak42show_runnin");
    }



int ak42add_parseid(acv, a41v, tree)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
tgg00_FileId *tree;
    {
    tgg_basis_error b_err;
    tsp_int_map_c2 rec_len;
    tsp_int_map_c2 key_len;
    int pos;
    tgg_bd_set_result_record set_result;
    tgg00_FilePos tree_pos;
    tgg_lkey zerokey;
    tsp_buf b;
    tak_all_command_glob *ptoc_Var37;
    tak40_show_glob *ptoc_Var38;
    tgg00_BdSetResultRecord *ptoc_Var39;
    cccprint("AK42  ak42add_pa");
    ptoc_Var37 = &*acv;
    ptoc_Var38 = &*a41v;
    ptoc_Var39 = &set_result;
    ptoc_Var39->bd_key_check_len = 0;
    ptoc_Var39->bd_max_rec_cnt = 32767;
    ptoc_Var39->bd_max_fill_len = sizeof(tsp00_Buf );
    ptoc_Var39->bd_next = 0;
    zerokey.integer.C_11.len = 0;
    tree_pos.tpsPno_gg00 = 2147483647;
    do
        {
        b07cnext_record(&ptoc_Var37->a_transinf.tri_trans , &(*tree) , &zerokey , &set_result , &tree_pos , b);
        b_err = (short ) sql__setrang(ptoc_Var37->a_transinf.tri_trans.trError_gg00 , -32768 , 32767);
        if ((int ) b_err == 0 || (int ) b_err == 1200 || (int ) b_err == 350)
            {
            pos = 1;
            while (pos < set_result.bd_fill_len)
                {
                rec_len.boolean.C_false.map_c2[0] = (unsigned char ) sql__setrang(b[sql__setrang(pos , 1 , 8192) + -1] , 0 , 255);
                rec_len.boolean.C_false.map_c2[1] = (unsigned char ) sql__setrang(b[sql__setrang(pos + 1 , 1 , 8192) + -1] , 0 , 255);
                key_len.boolean.C_false.map_c2[0] = (unsigned char ) sql__setrang(b[sql__setrang(pos + 2 , 1 , 8192) + -1] , 0 , 255);
                key_len.boolean.C_false.map_c2[1] = (unsigned char ) sql__setrang(b[sql__setrang(pos + 3 , 1 , 8192) + -1] , 0 , 255);
                t01buf(5 , b , 1 , (int ) rec_len.boolean.C_true.map_int);
                a40move_i4(&(*acv) , &(*a41v) , s20or4b(b , pos + 8) , 0 , 0 , 0);
                a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) &b[sql__setrang(pos + (int ) key_len.boolean.C_true.map_int + 8 , 1 , 8192) + -1] , 12);
                pos = pos + (int ) rec_len.boolean.C_true.map_int;
                }
            if ((int ) b_err == 350)
                {
                b_err = 0;
                }
            else
                {
                b_err = 1420;
                }
            }
        }
    while (!((int ) b_err != 0 || (int ) (*(tsp1_segment *) sql__nil(ptoc_Var37->a_return_segm , "vak42.p" , 19282)).integer.C_4.sp1r_returncode != 0));
    cccprint("<ak42add_parseid");
    }

typedef
  struct ptoc_Type210
    {
    tsp00_Int2 cshRecLen_kb05;
    tsp00_Int2 cshKeyLen_kb05;
    tsp00_Int2 cshVarcolOffset_kb05;
    tsp00_Int2 cshVarcolCnt_kb05;
    tsp00_Int4 cshKey_kb05;
    tgg91_TransNo cshConsistView_kb05;
    tgg91_TransNo cshOpenTrans_kb05;
    tsp00_Int4 cshStartDate_kb05;
    tsp00_Int4 cshStartTime_kb05;
    tsp00_Int4 cshTimeLastUsed_kb05;
    } tkb05_ConsistViewShow_ak42consistent_view;


int ak42consistent_view(acv, a41v)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
    {
    tgg_basis_error b_err;
    tsp_int_map_c2 rec_len;
    tsp_int_map_c2 key_len;
    int pos;
    tgg_bd_set_result_record set_result;
    tgg00_FilePos tree_pos;
    tgg_lkey zerokey;
    tsp_buf b;
    tgg00_BufLenInfo data;
    tgg00_QualBuf qual;
    struct tgg00_MessBlock mblock;
    tkb05_ConsistViewShow_ak42consistent_view *pShow;
    tak_all_command_glob *ptoc_Var40;
    tak40_show_glob *ptoc_Var41;
    tgg00_BdSetResultRecord *ptoc_Var42;
    cccprint("AK42  ak42consis");
    ptoc_Var40 = &*acv;
    ptoc_Var41 = &*a41v;
    a40get_catalog_table(&(*acv) , &(*a41v));
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var40->a_return_segm , "vak42.p" , 19316)).integer.C_4.sp1r_returncode == 0)
        {
        g01mblock_init(&(*acv).a_transinf.tri_trans , 63 , 89 , &mblock);
        g01datapart_init(&mblock , (tgg00_DataPart *) &data , sizeof(tgg00_BufLenInfo ));
        g01stack_init(&mblock , (tgg00_StackList *) (int *) 0 , 0 , (tgg00_QualBuf *) &qual , sizeof(tgg00_QualBuf ));
        (*(tgg00_QualBuf *) sql__nil(mblock.integer.C_4.mb_qual , "vak42.p" , 19323)).integer.C_2.mtree = ptoc_Var40->a_into_tree;
        a06rsend_mess_buf(&(*acv) , &mblock , 1 , &b_err);
        if ((int ) b_err == 0)
            {
            ptoc_Var42 = &set_result;
            ptoc_Var42->bd_key_check_len = 0;
            ptoc_Var42->bd_max_rec_cnt = 32767;
            ptoc_Var42->bd_max_fill_len = sizeof(tsp00_Buf );
            ptoc_Var42->bd_next = 0;
            zerokey.integer.C_11.len = 0;
            tree_pos.tpsPno_gg00 = 2147483647;
            do
                {
                b07cnext_record(&ptoc_Var40->a_transinf.tri_trans , &ptoc_Var40->a_into_tree , &zerokey , &set_result , &tree_pos , b);
                b_err = (short ) sql__setrang(ptoc_Var40->a_transinf.tri_trans.trError_gg00 , -32768 , 32767);
                if ((int ) b_err == 0 || (int ) b_err == 1200 || (int ) b_err == 350)
                    {
                    pos = 1;
                    while (pos < set_result.bd_fill_len)
                        {
                        rec_len.boolean.C_false.map_c2[0] = (unsigned char ) sql__setrang(b[sql__setrang(pos , 1 , 8192) + -1] , 0 , 255);
                        rec_len.boolean.C_false.map_c2[1] = (unsigned char ) sql__setrang(b[sql__setrang(pos + 1 , 1 , 8192) + -1] , 0 , 255);
                        key_len.boolean.C_false.map_c2[0] = (unsigned char ) sql__setrang(b[sql__setrang(pos + 2 , 1 , 8192) + -1] , 0 , 255);
                        key_len.boolean.C_false.map_c2[1] = (unsigned char ) sql__setrang(b[sql__setrang(pos + 3 , 1 , 8192) + -1] , 0 , 255);
                        t01buf(5 , b , 1 , (int ) rec_len.boolean.C_true.map_int);
                        pShow = (tkb05_ConsistViewShow_ak42consistent_view *) &b[sql__setrang(pos , 1 , 8192) + -1];
                        if (gg06IsNilTrans(&(*(struct ptoc_Type210 *) sql__nil(pShow , "vak42.p" , 19356)).cshConsistView_kb05))
                            {
                            a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) (int *) 0 , 0);
                            }
                        else
                            {
                            a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) &(*(struct ptoc_Type210 *) sql__nil(pShow , "vak42.p" , 19360)).cshConsistView_kb05 , sizeof(tgg90_Cint6 ));
                            }
                        if (gg06IsNilTrans(&(*(struct ptoc_Type210 *) sql__nil(pShow , "vak42.p" , 19363)).cshOpenTrans_kb05))
                            {
                            a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) (int *) 0 , 0);
                            }
                        else
                            {
                            a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) &(*(struct ptoc_Type210 *) sql__nil(pShow , "vak42.p" , 19367)).cshOpenTrans_kb05 , sizeof(tgg90_Cint6 ));
                            }
                        a40put_date_time(&(*acv) , &(*a41v) , &(*(struct ptoc_Type210 *) sql__nil(pShow , "vak42.p" , 19370)).cshStartDate_kb05 , 1 , (char ) sql__setrang((*(struct ptoc_Type210 *) sql__nil(pShow , "vak42.p" , 19371)).cshStartDate_kb05 == 0 , 0 , 1));
                        a40put_date_time(&(*acv) , &(*a41v) , &(*(struct ptoc_Type210 *) sql__nil(pShow , "vak42.p" , 19372)).cshStartTime_kb05 , 0 , (char ) sql__setrang((*(struct ptoc_Type210 *) sql__nil(pShow , "vak42.p" , 19373)).cshStartTime_kb05 == 0 , 0 , 1));
                        a40move_i4(&(*acv) , &(*a41v) , (*(struct ptoc_Type210 *) sql__nil(pShow , "vak42.p" , 19374)).cshTimeLastUsed_kb05 , 0 , 0 , 0);
                        pos = pos + (int ) rec_len.boolean.C_true.map_int;
                        }
                    if ((int ) b_err == 350)
                        {
                        b_err = 0;
                        }
                    else
                        {
                        b_err = 1420;
                        }
                    }
                }
            while (!((int ) b_err != 0 || (int ) (*(tsp1_segment *) sql__nil(ptoc_Var40->a_return_segm , "vak42.p" , 19388)).integer.C_4.sp1r_returncode != 0));
            }
        b01empty_file(&ptoc_Var40->a_transinf.tri_trans , &ptoc_Var40->a_into_tree);
        }
    cccprint("<ak42consistent_");
    }



int ak42roots(acv, a41v)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
    {
    char use_tree;
    char table_found;
    tsp00_NumError res;
    tgg00_FileId tree;
    tsp00_KnlIdentifier owner;
    tsp00_KnlIdentifier tablen;
    tsp00_MoveObjPtr owner_ptr;
    tsp00_MoveObjPtr tablen_ptr;
    tsp00_KnlIdentifier indexname;
    tsp00_MoveObjPtr indexn_ptr;
    int indexn_len;
    tsp00_C18 f_type;
    tak_sysbufferaddress base_ptr;
    tak00_colinfo_ptr col_ptr;
    tsp00_ResNum number;
    tak_index_scan_record index_scan_rec;
    int ptoc_Var43;
    tak_index_scan_record *ptoc_Var44;
    cccprint("AK42  ak42roots ");
    tree = b01niltree_id;
    a40get_catalog_table(&(*acv) , &(*a41v));
    do
        {
        use_tree = 0;
        b01next_fdir_entry(&(*acv).a_transinf.tri_trans , &tree);
        if ((int ) (*acv).a_transinf.tri_trans.trError_gg00 == 0)
            {
            s41p4int(number , 1 , tree.integer.C_1.fileRoot_gg00 , &res);
            if ((int ) res == 0)
                {
                use_tree = (char ) sql__setrang(a722test_col_qual(&(*acv) , &(*a41v) , "ROOT              " , (unsigned char)'\0' , (tsp00_MoveObj *) number , sizeof(tsp00_ResNum )) , 0 , 1);
                }
            else
                {
                use_tree = 1;
                }
            (*acv).a_transinf.tri_trans.trError_gg00 = 0;
            }
        if ((char ) sql__setrang(use_tree , 0 , 1))
            {
            owner_ptr = (tsp00_MoveObj *) (int *) 0;
            tablen_ptr = (tsp00_MoveObj *) (int *) 0;
            indexn_ptr = (tsp00_MoveObj *) (int *) 0;
            if ((ptoc_Var43 = (int ) tree.integer.C_4.fileTfn_gg00) == 7 || ptoc_Var43 == 13 || ptoc_Var43 == 18)
                {
                a06_systable_get(&(*acv) , 1 , tree.integer.C_4.fileTabId_gg00 , &base_ptr , 0 , &table_found);
                if ((char ) sql__setrang(table_found , 0 , 1))
                    {
                    _ptoc_ASSIGN(unsigned char , 64, owner, a01_il_b_identifier)
                    _ptoc_ASSIGN(unsigned char , 64, indexname, a01_il_b_identifier)
                    a06determine_username(&(*acv) , (*(struct tak_systembuffer *) sql__nil(base_ptr , "vak42.p" , 19465)).integer.C_1.sbase.bauthid , owner);
                    _ptoc_ASSIGN(unsigned char , 64, tablen, *(tsp00_C64 *) sql__nil((*(struct tak_systembuffer *) sql__nil(base_ptr , "vak42.p" , 19466)).integer.C_1.sbase.btablen , "vak42.p" , 19466))
                    switch ((int ) tree.integer.C_4.fileTfn_gg00)
                        {
                        case 7 :
                            a24fnd_indexno(&(*acv) , tree.integer.C_4.fileTabId_gg00 , sql__ord((int ) tree.integer.C_4.fileTfnNo_gg00[0]) , &index_scan_rec);
                            if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak42.p" , 19473)).integer.C_4.sp1r_returncode != 0)
                                {
                                (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak42.p" , 19475)).integer.C_4.sp1r_returncode = 0;
                                }
                            else
                                {
                                ptoc_Var44 = &index_scan_rec;
                                if (ptoc_Var44->isr_buf != (struct tak_systembuffer *) (int *) 0)
                                    {
                                    a24get_indexname(&(*acv) , ptoc_Var44->isr_buf , ptoc_Var44->isr_index , indexname);
                                    indexn_ptr = (tsp00_MoveObj *) indexname;
                                    indexn_len = sizeof(tsp00_C64 );
                                    }
                                }
                            _ptoc_ASSIGN(unsigned char , 18, f_type, "NAMED INDEX       ")
                            break;
                        case 13 :
                            _ptoc_ASSIGN(unsigned char , 18, f_type, "TABLE             ")
                            break;
                        case 18 :
                            _ptoc_ASSIGN(unsigned char , 18, f_type, "SHORT STRING FILE ")
                            break;
                        default :
                            (unsigned char ) sql__caseerr("vak42.p" , 19467);
                            break;
                        }
                    owner_ptr = (tsp00_MoveObj *) owner;
                    tablen_ptr = (tsp00_MoveObj *) tablen;
                    }
                }
            else
                {
                (*acv).a_transinf.tri_trans.trError_gg00 = 0;
                switch ((int ) tree.integer.C_4.fileTfn_gg00)
                    {
                    case 0 :
                        _ptoc_ASSIGN(unsigned char , 18, f_type, "SYS               ")
                        break;
                    case 11 :
                        _ptoc_ASSIGN(unsigned char , 18, f_type, "TEMP INV          ")
                        break;
                    case 26 :
                        _ptoc_ASSIGN(unsigned char , 18, f_type, "TEMP              ")
                        break;
                    case 1 :
                        _ptoc_ASSIGN(unsigned char , 18, f_type, "LONG COLUMN       ")
                        break;
                    default :
                        _ptoc_ASSIGN(unsigned char , 18, f_type, "UNKNOWN           ")
                        break;
                    }
                }
            a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) tree.integer.C_4.fileTabId_gg00 , sizeof(tsp00_C8 ));
            a40move(&(*acv) , &(*a41v) , owner_ptr , sizeof(tsp00_C64 ));
            a40move(&(*acv) , &(*a41v) , tablen_ptr , sizeof(tsp00_C64 ));
            a40move(&(*acv) , &(*a41v) , indexn_ptr , indexn_len);
            a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) f_type , sizeof(tsp00_C18 ));
            a40move_i4(&(*acv) , &(*a41v) , tree.integer.C_1.fileRoot_gg00 , 0 , 0 , 0);
            }
        }
    while (!((int ) (*acv).a_transinf.tri_trans.trError_gg00 != 0));
    cccprint("<ak42roots      ");
    }



int ak42files(acv, a41v)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
    {
    char table_found;
    tgg00_BasisError e;
    tgg00_BasisError next_e;
    tsp00_Int4 pageCount;
    tsp00_Int4 NumAllPages;
    tsp00_Int4 NumFreePages;
    tsp00_Int4 NumEmptyPages;
    tsp00_Int4 NumKeyLeafPages;
    tsp00_Int4 Occupancy;
    tgg00_FileId tree;
    tsp00_KnlIdentifier owner;
    tsp00_KnlIdentifier tablen;
    tsp00_MoveObjPtr owner_ptr;
    tsp00_MoveObjPtr tablen_ptr;
    tsp00_KnlIdentifier indexname;
    tsp00_MoveObjPtr indexn_ptr;
    int indexn_len;
    tsp00_C18 f_type;
    tak_sysbufferaddress base_ptr;
    tak00_colinfo_ptr col_ptr;
    tsp00_ResNum number;
    tak_index_scan_record index_scan_rec;
    int ptoc_Var45;
    tak_index_scan_record *ptoc_Var46;
    cccprint("AK42  ak42files ");
    tree = b01niltree_id;
    a40get_catalog_table(&(*acv) , &(*a41v));
    do
        {
        b01next_fdir_entry(&(*acv).a_transinf.tri_trans , &tree);
        next_e = (short ) sql__setrang((*acv).a_transinf.tri_trans.trError_gg00 , -32768 , 32767);
        if ((int ) next_e == 0)
            {
            owner_ptr = (tsp00_MoveObj *) (int *) 0;
            tablen_ptr = (tsp00_MoveObj *) (int *) 0;
            indexn_ptr = (tsp00_MoveObj *) (int *) 0;
            if ((ptoc_Var45 = (int ) tree.integer.C_4.fileTfn_gg00) == 7 || ptoc_Var45 == 13 || ptoc_Var45 == 18)
                {
                a06_systable_get(&(*acv) , 1 , tree.integer.C_4.fileTabId_gg00 , &base_ptr , 0 , &table_found);
                if ((char ) sql__setrang(table_found , 0 , 1))
                    {
                    _ptoc_ASSIGN(unsigned char , 64, owner, a01_il_b_identifier)
                    _ptoc_ASSIGN(unsigned char , 64, indexname, a01_il_b_identifier)
                    a06determine_username(&(*acv) , (*(struct tak_systembuffer *) sql__nil(base_ptr , "vak42.p" , 19594)).integer.C_1.sbase.bauthid , owner);
                    _ptoc_ASSIGN(unsigned char , 64, tablen, *(tsp00_C64 *) sql__nil((*(struct tak_systembuffer *) sql__nil(base_ptr , "vak42.p" , 19595)).integer.C_1.sbase.btablen , "vak42.p" , 19595))
                    switch ((int ) tree.integer.C_4.fileTfn_gg00)
                        {
                        case 7 :
                            a24fnd_indexno(&(*acv) , tree.integer.C_4.fileTabId_gg00 , sql__ord((int ) tree.integer.C_4.fileTfnNo_gg00[0]) , &index_scan_rec);
                            if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak42.p" , 19602)).integer.C_4.sp1r_returncode != 0)
                                {
                                (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak42.p" , 19604)).integer.C_4.sp1r_returncode = 0;
                                }
                            else
                                {
                                ptoc_Var46 = &index_scan_rec;
                                if (ptoc_Var46->isr_buf != (struct tak_systembuffer *) (int *) 0)
                                    {
                                    a24get_indexname(&(*acv) , ptoc_Var46->isr_buf , ptoc_Var46->isr_index , indexname);
                                    indexn_ptr = (tsp00_MoveObj *) indexname;
                                    indexn_len = sizeof(tsp00_C64 );
                                    }
                                }
                            _ptoc_ASSIGN(unsigned char , 18, f_type, "NAMED INDEX       ")
                            break;
                        case 13 :
                            _ptoc_ASSIGN(unsigned char , 18, f_type, "TABLE             ")
                            break;
                        case 18 :
                            _ptoc_ASSIGN(unsigned char , 18, f_type, "SHORT STRING FILE ")
                            break;
                        default :
                            (unsigned char ) sql__caseerr("vak42.p" , 19596);
                            break;
                        }
                    owner_ptr = (tsp00_MoveObj *) owner;
                    tablen_ptr = (tsp00_MoveObj *) tablen;
                    }
                }
            else
                {
                (*acv).a_transinf.tri_trans.trError_gg00 = 0;
                switch ((int ) tree.integer.C_4.fileTfn_gg00)
                    {
                    case 0 :
                        _ptoc_ASSIGN(unsigned char , 18, f_type, "SYS               ")
                        break;
                    case 11 :
                        _ptoc_ASSIGN(unsigned char , 18, f_type, "TEMP INV          ")
                        break;
                    case 26 :
                        _ptoc_ASSIGN(unsigned char , 18, f_type, "TEMP              ")
                        break;
                    case 1 :
                        _ptoc_ASSIGN(unsigned char , 18, f_type, "LONG COLUMN       ")
                        break;
                    case 22 :
                        _ptoc_ASSIGN(unsigned char , 18, f_type, "OBJECT            ")
                        break;
                    default :
                        _ptoc_ASSIGN(unsigned char , 18, f_type, "UNKNOWN           ")
                        break;
                    }
                }
            if ((int ) tree.integer.C_4.fileTfn_gg00 == 22)
                {
                ak341GetObjFileInfo(&(*acv).a_transinf.tri_trans , &tree , &NumAllPages , &NumFreePages , &NumEmptyPages , &NumKeyLeafPages , &Occupancy);
                pageCount = NumAllPages + NumKeyLeafPages;
                }
            else
                {
                a06get_page_cnt(&(*acv) , &tree , 0 , &pageCount , &e);
                if ((int ) e != 0)
                    {
                    pageCount = -1;
                    }
                }
            a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) &tree , sizeof(tgg00_FileId ));
            a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) tree.integer.C_4.fileTabId_gg00 , sizeof(tsp00_C8 ));
            a40move(&(*acv) , &(*a41v) , owner_ptr , sizeof(tsp00_C64 ));
            a40move(&(*acv) , &(*a41v) , tablen_ptr , sizeof(tsp00_C64 ));
            a40move(&(*acv) , &(*a41v) , indexn_ptr , indexn_len);
            a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) f_type , sizeof(tsp00_C18 ));
            a40move_i4(&(*acv) , &(*a41v) , tree.integer.C_1.fileRoot_gg00 , 0 , 0 , 0);
            a40move_i4(&(*acv) , &(*a41v) , pageCount , (char ) sql__setrang(pageCount < 0 , 0 , 1) , 0 , 0);
            }
        }
    while (!((int ) next_e != 0));
    cccprint("<ak42files      ");
    }



int ak42InfoGCStats(acv, a41v)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
    {
    tsp00_TaskId taskId;
    char active;
    tsp00_8ByteCounter activateCount;
    tsp00_8ByteCounter deleteObj;
    tsp00_8ByteCounter relHistCreateObjFile;
    tsp00_8ByteCounter relHistDeleteObj;
    tsp00_8ByteCounter relHistDropObjFile;
    tsp00_8ByteCounter relHistLockObj;
    tsp00_8ByteCounter relHistInsertObj;
    tsp00_8ByteCounter relHistNewObj;
    tsp00_8ByteCounter relHistUpdateObj;
    tsp00_8ByteCounter relHistEntryNormal;
    tsp00_8ByteCounter relHistEntrySensitive;
    tsp00_8ByteCounter relHistEntryTruncate;
    tsp00_8ByteCounter relPageEmpty;
    tsp00_8ByteCounter relPageDropFile;
    tsp00_8ByteCounter relDropFile;
    tsp00_Int4 ix;
    int ptoc_Var47;
    cccprint("AK42  ak42InfoGC");
    a40get_catalog_table(&(*acv) , &(*a41v));
    ptoc_Var47 = g01maxobjgarbagecoll();
    if (1 <= ptoc_Var47)
        {
        ix = 1;
        do
            {
            if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak42.p" , 19721)).integer.C_4.sp1r_returncode == 0)
                {
                bd91InfoGCStats(ix , &taskId , &active , &activateCount , &deleteObj , &relHistEntryNormal , &relHistEntrySensitive , &relHistEntryTruncate , &relHistCreateObjFile , &relHistDeleteObj , &relHistDropObjFile , &relHistLockObj , &relHistInsertObj , &relHistNewObj , &relHistUpdateObj , &relPageEmpty , &relPageDropFile , &relDropFile);
                a40move_i4(&(*acv) , &(*a41v) , taskId , 0 , 0 , 0);
                a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) a40yes_no[sql__setrang((int ) (char ) sql__setrang(active , 0 , 1) , 0 , 1)] , 3);
                a40move_counter(&(*acv) , &(*a41v) , activateCount , 0 , 0 , 0);
                a40move_counter(&(*acv) , &(*a41v) , deleteObj , 0 , 0 , 0);
                a40move_counter(&(*acv) , &(*a41v) , relHistEntryNormal , 0 , 0 , 0);
                a40move_counter(&(*acv) , &(*a41v) , relHistEntrySensitive , 0 , 0 , 0);
                a40move_counter(&(*acv) , &(*a41v) , relHistEntryTruncate , 0 , 0 , 0);
                a40move_counter(&(*acv) , &(*a41v) , relHistCreateObjFile , 0 , 0 , 0);
                a40move_counter(&(*acv) , &(*a41v) , relHistDeleteObj , 0 , 0 , 0);
                a40move_counter(&(*acv) , &(*a41v) , relHistDropObjFile , 0 , 0 , 0);
                a40move_counter(&(*acv) , &(*a41v) , relHistLockObj , 0 , 0 , 0);
                a40move_counter(&(*acv) , &(*a41v) , relHistInsertObj , 0 , 0 , 0);
                a40move_counter(&(*acv) , &(*a41v) , relHistNewObj , 0 , 0 , 0);
                a40move_counter(&(*acv) , &(*a41v) , relHistUpdateObj , 0 , 0 , 0);
                a40move_counter(&(*acv) , &(*a41v) , relPageEmpty , 0 , 0 , 0);
                a40move_counter(&(*acv) , &(*a41v) , relPageDropFile , 0 , 0 , 0);
                a40move_counter(&(*acv) , &(*a41v) , relDropFile , 0 , 0 , 0);
                }
            if (ix == ptoc_Var47)
                {
                break;
                }
            ix += 1;
            }
        while (!0);
        }
    cccprint("<ak42InfoGCStats");
    }



int ak42_container_roots(acv, a41v)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
    {
    tsp00_Addr pObjFileIter;
    tsp00_Addr pContObjFileIter;
    struct ptoc_Type211
      {
      union
        {
        struct
          {
          tsp00_C64 p;
          } C_true;
        struct
          {
          tsp00_Addr align;
          } C_false;
        } boolean;
      } Stackarea;
    tsp00_Int4 ClassId;
    tsp00_Int4 Root;
    tsp00_Int4 ContainerSequelNo;
    tsp00_Int2 NumKeyPartitions;
    tsp00_Int4 MaxObjBodySize;
    tsp00_Int4 MaxObjPerPage;
    tsp00_Int4 MaxChains;
    char VarContainer;
    char DeleteFlag;
    cccprint("AK42  ak42_conta");
    a40get_catalog_table(&(*acv) , &(*a41v));
    ClassId = 0;
    pObjFileIter = (unsigned char *) (int *) 0;
    pContObjFileIter = (unsigned char *) (int *) 0;
    while (ClassId >= 0 && (int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak42.p" , 19793)).integer.C_4.sp1r_returncode == 0)
        {
        bd91InfoContainerRoots(&pObjFileIter , &pContObjFileIter , (unsigned char *) &Stackarea.boolean.C_true.p[0] , (short ) sql__setrang((short ) sizeof(struct ptoc_Type211 ) , -32768 , 32767) , &ClassId , &Root , &ContainerSequelNo , &NumKeyPartitions , &VarContainer , &DeleteFlag , &MaxObjBodySize , &MaxObjPerPage , &MaxChains);
        if (ClassId >= 0)
            {
            a40move_i4(&(*acv) , &(*a41v) , ClassId , 0 , 0 , 0);
            a40move_i4(&(*acv) , &(*a41v) , ContainerSequelNo , 0 , 0 , 0);
            a40move_i4(&(*acv) , &(*a41v) , Root , 0 , 0 , 0);
            a40move_i4(&(*acv) , &(*a41v) , (int ) NumKeyPartitions , 0 , 0 , 0);
            a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) a40yes_no[sql__setrang((int ) (char ) sql__setrang(VarContainer , 0 , 1) , 0 , 1)] , 3);
            a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) a40yes_no[sql__setrang((int ) (char ) sql__setrang(DeleteFlag , 0 , 1) , 0 , 1)] , 3);
            a40move_i4(&(*acv) , &(*a41v) , MaxObjBodySize , 0 , 0 , 0);
            a40move_i4(&(*acv) , &(*a41v) , MaxObjPerPage , 0 , 0 , 0);
            a40move_i4(&(*acv) , &(*a41v) , MaxChains , 0 , 0 , 0);
            }
        }
    cccprint("<ak42_container_");
    }



int ak42_container_chains(acv, a41v)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
    {
    tsp00_Addr pObjFileIter;
    tsp00_Addr pContObjFileIter;
    struct ptoc_Type212
      {
      union
        {
        struct
          {
          tsp00_C64 p;
          } C_true;
        struct
          {
          tsp00_Addr align;
          } C_false;
        } boolean;
      } Stackarea;
    tsp00_Int4 ClassId;
    tsp00_Int4 ContainerSequelNo;
    tsp00_Int4 ChainId;
    tsp00_Int4 Subroot;
    tsp00_Int4 ObjCount;
    tsp00_Int4 PageCount;
    tsp00_Int4 ActiveGC;
    char RelEmptyPages;
    cccprint("AK42  ak42_conta");
    a40get_catalog_table(&(*acv) , &(*a41v));
    ClassId = 0;
    pObjFileIter = (unsigned char *) (int *) 0;
    pContObjFileIter = (unsigned char *) (int *) 0;
    while (ClassId >= 0 && (int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak42.p" , 19857)).integer.C_4.sp1r_returncode == 0)
        {
        bd91InfoContainerChains(&pObjFileIter , &pContObjFileIter , (unsigned char *) &Stackarea.boolean.C_true.p[0] , (short ) sql__setrang((short ) sizeof(struct ptoc_Type212 ) , -32768 , 32767) , &ClassId , &ContainerSequelNo , &ChainId , &Subroot , &ObjCount , &PageCount , &ActiveGC , &RelEmptyPages);
        if (ClassId >= 0)
            {
            a40move_i4(&(*acv) , &(*a41v) , ClassId , 0 , 0 , 0);
            a40move_i4(&(*acv) , &(*a41v) , ContainerSequelNo , 0 , 0 , 0);
            a40move_i4(&(*acv) , &(*a41v) , ChainId , 0 , 0 , 0);
            a40move_i4(&(*acv) , &(*a41v) , Subroot , 0 , 0 , 0);
            a40move_i4(&(*acv) , &(*a41v) , ObjCount , 0 , 0 , 0);
            a40move_i4(&(*acv) , &(*a41v) , PageCount , 0 , 0 , 0);
            a40move_i4(&(*acv) , &(*a41v) , ActiveGC , 0 , 0 , 0);
            a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) a40yes_no[sql__setrang((int ) (char ) sql__setrang(RelEmptyPages , 0 , 1) , 0 , 1)] , 3);
            }
        }
    cccprint("<ak42_container_");
    }



int ak42_container_keys(acv, a41v)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
    {
    tsp00_Addr pObjFileIter;
    struct ptoc_Type213
      {
      union
        {
        struct
          {
          tsp00_C64 p;
          } C_true;
        struct
          {
          tsp00_Addr align;
          } C_false;
        } boolean;
      } Stackarea;
    tsp00_Int4 ClassId;
    tsp00_Int4 KeyRoot;
    tsp00_Int2 KeyIdx;
    cccprint("AK42  ak42_conta");
    a40get_catalog_table(&(*acv) , &(*a41v));
    ClassId = 0;
    pObjFileIter = (unsigned char *) (int *) 0;
    while (ClassId >= 0 && (int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak42.p" , 19912)).integer.C_4.sp1r_returncode == 0)
        {
        bd91InfoContainerKeys(&pObjFileIter , (unsigned char *) &Stackarea.boolean.C_true.p[0] , (short ) sql__setrang((short ) sizeof(struct ptoc_Type213 ) , -32768 , 32767) , &KeyIdx , &ClassId , &KeyRoot);
        if (ClassId >= 0)
            {
            a40move_i4(&(*acv) , &(*a41v) , ClassId , 0 , 0 , 0);
            a40move_i4(&(*acv) , &(*a41v) , (int ) KeyIdx , 0 , 0 , 0);
            a40move_i4(&(*acv) , &(*a41v) , KeyRoot , 0 , 0 , 0);
            }
        }
    cccprint("<ak42_container_");
    }



int ak42select_lock_trans(acv, a41v)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
    {
    tgg00_BasisError b_err;
    tsp00_IntMapC2 rec_len;
    tsp00_IntMapC2 key_len;
    int rec_pos;
    int offset;
    tsp00_Int4 ix;
    tgg00_BdSetResultRecord set_result;
    tgg00_FilePos tree_pos;
    tgg00_Lkey zerokey;
    tkb05_LockShow lock_info;
    tgg00_MessType2 mm_type;
    tgg00_BufLenInfo data;
    tgg00_QualBuf qual;
    struct tgg00_MessBlock mblock;
    tak_all_command_glob *ptoc_Var48;
    tak40_show_glob *ptoc_Var49;
    tgg00_BdSetResultRecord *ptoc_Var50;
    cccprint("AK42  ak42select");
    ptoc_Var48 = &*acv;
    ptoc_Var49 = &*a41v;
    a40get_catalog_table(&(*acv) , &(*a41v));
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var48->a_return_segm , "vak42.p" , 19961)).integer.C_4.sp1r_returncode == 0)
        {
        switch ((int ) ptoc_Var49->a4sh_kind)
            {
            case 14 :
                mm_type = 34;
                break;
            case 77 :
                mm_type = 61;
                break;
            case 84 :
                mm_type = 5;
                break;
            case 50 :
                mm_type = 9;
                break;
            default :
                (unsigned char ) sql__caseerr("vak42.p" , 19964);
                break;
            }
        g01mblock_init(&(*acv).a_transinf.tri_trans , 63 , (unsigned char ) sql__setrang(mm_type , 0 , 95) , &mblock);
        g01datapart_init(&mblock , (tgg00_DataPart *) &data , sizeof(tgg00_BufLenInfo ));
        g01stack_init(&mblock , (tgg00_StackList *) (int *) 0 , 0 , (tgg00_QualBuf *) &qual , sizeof(tgg00_QualBuf ));
        (*(tgg00_QualBuf *) sql__nil(mblock.integer.C_4.mb_qual , "vak42.p" , 19980)).integer.C_2.mtree = ptoc_Var48->a_into_tree;
        a06rsend_mess_buf(&(*acv) , &mblock , 1 , &b_err);
        if ((int ) b_err == 0)
            {
            ptoc_Var50 = &set_result;
            ptoc_Var50->bd_key_check_len = 0;
            ptoc_Var50->bd_max_rec_cnt = 32767;
            ptoc_Var50->bd_max_fill_len = sizeof(tgg00_BufLenInfo );
            ptoc_Var50->bd_next = 0;
            zerokey.integer.C_11.len = 0;
            tree_pos.tpsPno_gg00 = 2147483647;
            do
                {
                b07cnext_record(&ptoc_Var48->a_transinf.tri_trans , &ptoc_Var48->a_into_tree , &zerokey , &set_result , &tree_pos , data.boolean.C_true.rb.integer.C_11.buf);
                b_err = (short ) sql__setrang(ptoc_Var48->a_transinf.tri_trans.trError_gg00 , -32768 , 32767);
                if ((int ) b_err == 0 || (int ) b_err == 1200 || (int ) b_err == 350)
                    {
                    rec_pos = 1;
                    while (rec_pos < set_result.bd_fill_len)
                        {
                        rec_len.boolean.C_false.mapC2_sp00[0] = (unsigned char ) sql__setrang(data.boolean.C_true.rb.integer.C_11.buf[sql__setrang(rec_pos , 1 , 8192) + -1] , 0 , 255);
                        rec_len.boolean.C_false.mapC2_sp00[1] = (unsigned char ) sql__setrang(data.boolean.C_true.rb.integer.C_11.buf[sql__setrang(rec_pos + 1 , 1 , 8192) + -1] , 0 , 255);
                        switch ((int ) ptoc_Var49->a4sh_kind)
                            {
                            case 84 :
                                key_len.boolean.C_false.mapC2_sp00[0] = (unsigned char ) sql__setrang(data.boolean.C_true.rb.integer.C_11.buf[sql__setrang(rec_pos + 2 , 1 , 8192) + -1] , 0 , 255);
                                key_len.boolean.C_false.mapC2_sp00[1] = (unsigned char ) sql__setrang(data.boolean.C_true.rb.integer.C_11.buf[sql__setrang(rec_pos + 3 , 1 , 8192) + -1] , 0 , 255);
                                offset = (int ) key_len.boolean.C_true.mapInt_sp00 + 8;
                                a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) &data.boolean.C_true.rb.integer.C_11.buf[sql__setrang(rec_pos + offset + 1 , 1 , 8192) + -1] , (int ) rec_len.boolean.C_true.mapInt_sp00 - offset - a01char_size * 12 + -2);
                                a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) &data.boolean.C_true.rb.integer.C_11.buf[sql__setrang(rec_pos + offset + a01char_size * 40 + 2 , 1 , 8192) + -1] , a01char_size * 12);
                                break;
                            case 50 :
                                key_len.boolean.C_false.mapC2_sp00[0] = (unsigned char ) sql__setrang(data.boolean.C_true.rb.integer.C_11.buf[sql__setrang(rec_pos + 2 , 1 , 8192) + -1] , 0 , 255);
                                key_len.boolean.C_false.mapC2_sp00[1] = (unsigned char ) sql__setrang(data.boolean.C_true.rb.integer.C_11.buf[sql__setrang(rec_pos + 3 , 1 , 8192) + -1] , 0 , 255);
                                offset = (int ) key_len.boolean.C_true.mapInt_sp00 + 8;
                                ix = 1;
                                do
                                    {
                                    a40move_i4(&(*acv) , &(*a41v) , s20or4b(data.boolean.C_true.rb.integer.C_11.buf , rec_pos + offset + 1) , 0 , 0 , 0);
                                    offset = offset + sizeof(int ) + 1;
                                    if (ix == 4)
                                        {
                                        break;
                                        }
                                    ix += 1;
                                    }
                                while (!0);
                                a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) &data.boolean.C_true.rb.integer.C_11.buf[sql__setrang(rec_pos + offset + 1 , 1 , 8192) + -1] , 12);
                                offset = offset + 13;
                                a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) &data.boolean.C_true.rb.integer.C_11.buf[sql__setrang(rec_pos + offset + 1 , 1 , 8192) + -1] , 5);
                                break;
                            default :
                                g10mv4("VAK42 " , 5 , sizeof(tsp00_Buf ) , sizeof(tkb05_LockShow ) , data.boolean.C_true.rb.integer.C_11.buf , rec_pos , &lock_info , 1 , (int ) rec_len.boolean.C_true.mapInt_sp00 , &b_err);
                                if ((int ) b_err != -9111)
                                    {
                                    ak42one_lock_info(&(*acv) , &(*a41v) , &lock_info);
                                    }
                                break;
                            }
                        rec_pos = rec_pos + (int ) rec_len.boolean.C_true.mapInt_sp00;
                        }
                    if ((int ) b_err == 350)
                        {
                        b_err = 0;
                        }
                    else
                        {
                        b_err = 1420;
                        }
                    }
                }
            while (!((int ) b_err != 0 || (int ) (*(tsp1_segment *) sql__nil(ptoc_Var48->a_return_segm , "vak42.p" , 20079)).integer.C_4.sp1r_returncode != 0));
            }
        else
            {
            a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , 1);
            }
        b01empty_file(&ptoc_Var48->a_transinf.tri_trans , &ptoc_Var48->a_into_tree);
        }
    cccprint("<ak42select_lock");
    }



int ak42one_lock_info(acv, a41v, lock_info)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
tkb05_LockShow *lock_info;
    {
    char ok;
    char table_found;
    char is_obj_lock;
    unsigned char obj_class_char;
    tsp00_C3 c3;
    int p;
    int c_index;
    int rowid_pos;
    int length;
    int val_length;
    int max_length;
    int curr_move_len;
    int help_tabname_len;
    int tabname_pos;
    struct ptoc_Type214
      {
      union
        {
        struct
          {
          tgg00_Surrogate surrogate;
          } C_true;
        struct
          {
          tsp00_C4 fill;
          tgg00_UserId id;
          } C_false;
        } boolean;
      } userid;
    tgg00_SysInfoKey sysk;
    tsp00_KnlIdentifier user;
    tsp00_KnlIdentifier owner;
    tsp00_KnlIdentifier tablename;
    tsp00_Line aux_line;
    tsp00_C20 c20_1;
    tsp00_C120 c120;
    tsp00_C256 key_info;
    char is_trunc;
    tgg00_Rec sysbuf;
    tak_all_command_glob *ptoc_Var51;
    tak40_show_glob *ptoc_Var52;
    tkb05_TransShow *ptoc_Var53;
    tkb05_TransShow *ptoc_Var54;
    tkb05_TransShow *ptoc_Var55;
    tkb05_TransShow *ptoc_Var56;
    tkb05_TransShow *ptoc_Var57;
    tak_baserecord *ptoc_Var58;
    tak00_columninfo *ptoc_Var59;
    int ptoc_Var60;
    cccprint("AK42  ak42one_lo");
    ptoc_Var51 = &*acv;
    ptoc_Var52 = &*a41v;
    ptoc_Var53 = &(*lock_info).lshTransShow_kb05;
    a40move_i4(&(*acv) , &(*a41v) , gg06SessionGetInt4(&ptoc_Var53->tshSession_kb05) , 0 , 0 , 0);
    ak42move_trans_no(&(*acv) , &(*a41v) , &ptoc_Var53->tshSelTransId_kb05);
    a40move_i4(&(*acv) , &(*a41v) , ptoc_Var53->tshSubtrans_kb05 , 0 , 0 , 0);
    if (gg06IsNilTrans(&ptoc_Var53->tshWriteTransId_kb05))
        {
        a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) (int *) 0 , 0);
        }
    else
        {
        a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) &ptoc_Var53->tshWriteTransId_kb05 , sizeof(tgg90_Cint6 ));
        }
    ptoc_Var54 = &(*lock_info).lshTransShow_kb05;
    a40move_i4(&(*acv) , &(*a41v) , ptoc_Var54->tshTaskId_kb05 , 0 , 0 , 0);
    _ptoc_ASSIGN(unsigned char , 8, userid.boolean.C_true.surrogate, ptoc_Var51->a_curr_user_id)
    _ptoc_ASSIGN(unsigned char , 4, userid.boolean.C_false.id, ptoc_Var54->tshUserId_kb05)
    a06determine_username(&(*acv) , userid.boolean.C_true.surrogate , user);
    a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) user , sizeof(tsp00_C64 ));
    a40put_date_time(&(*acv) , &(*a41v) , &ptoc_Var54->tshStartDate_kb05 , 1 , (char ) sql__setrang(ptoc_Var54->tshStartDate_kb05 == 0 , 0 , 1));
    a40put_date_time(&(*acv) , &(*a41v) , &ptoc_Var54->tshStartTime_kb05 , 0 , (char ) sql__setrang(ptoc_Var54->tshStartTime_kb05 == 0 , 0 , 1));
    a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) ptoc_Var54->tshTermId_kb05 , sizeof(tsp00_C18 ));
    a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) (int *) 0 , 0);
    if ((int ) ptoc_Var54->tshTrReqMode_kb05 != 0)
        {
        a40move_i4(&(*acv) , &(*a41v) , ptoc_Var54->tshReqTimeout_kb05 , 0 , 0 , 0);
        }
    else
        {
        a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) (int *) 0 , 0);
        }
    if ((int ) ptoc_Var54->tshTrLockMode_kb05 == 3)
        {
        a40move_i4(&(*acv) , &(*a41v) , ptoc_Var54->tshTimeWrite_kb05 , 0 , 0 , 0);
        }
    else
        {
        a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) (int *) 0 , 0);
        }
    if ((int ) ptoc_Var52->a4sh_kind == 14)
        {
        ptoc_Var55 = &(*lock_info).lshTransShow_kb05;
        ak42lock_req_mode(&(*acv) , &(*a41v) , (unsigned char ) sql__setrang(ptoc_Var55->tshLockMode_kb05 , 0 , 7));
        ak42lock_req_state(&(*acv) , &(*a41v) , ptoc_Var55->tshLockState_kb05);
        ak42lock_req_mode(&(*acv) , &(*a41v) , (unsigned char ) sql__setrang(ptoc_Var55->tshReqMode_kb05 , 0 , 7));
        ak42lock_req_state(&(*acv) , &(*a41v) , ptoc_Var55->tshReqState_kb05);
        }
    else
        {
        ptoc_Var56 = &(*lock_info).lshTransShow_kb05;
        ak42lock_mode(&(*acv) , &(*a41v) , (unsigned char ) sql__setrang(ptoc_Var56->tshTrLockMode_kb05 , 0 , 3));
        ak42lock_state(&(*acv) , &(*a41v) , (unsigned char ) sql__setrang(ptoc_Var56->tshTrLockMode_kb05 , 0 , 3));
        ak42lock_mode(&(*acv) , &(*a41v) , (unsigned char ) sql__setrang(ptoc_Var56->tshTrReqMode_kb05 , 0 , 3));
        ak42lock_state(&(*acv) , &(*a41v) , (unsigned char ) sql__setrang(ptoc_Var56->tshTrReqMode_kb05 , 0 , 3));
        }
    ptoc_Var57 = &(*lock_info).lshTransShow_kb05;
    if (gg06IsNilTrans(&ptoc_Var57->tshConsistView_kb05))
        {
        a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) (int *) 0 , 0);
        }
    else
        {
        a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) &ptoc_Var57->tshConsistView_kb05 , sizeof(tgg90_Cint6 ));
        }
    ak42appl_info(&(*acv) , &(*a41v) , ptoc_Var57->tshTaskId_kb05);
    if ((int ) ptoc_Var52->a4sh_kind == 14)
        {
        is_obj_lock = 0;
        a06_systable_get(&(*acv) , 1 , (*lock_info).lshTabId_kb05 , &ptoc_Var52->a4p_arr.pbasep , 1 , &table_found);
        if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak42.p" , 20248)).integer.C_4.sp1r_returncode == (int ) a07_return_code(1500 , (unsigned char ) sql__setrang((*acv).a_sqlmode , 0 , 3)))
            {
            (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak42.p" , 20251)).integer.C_4.sp1r_returncode = 0;
            }
        if ((char ) sql__setrang(table_found , 0 , 1))
            {
            a40table_column_to_shbuf(&(*acv) , &(*a41v) , ptoc_Var52->a4p_arr.pbasep , -1);
            }
        else
            {
            if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var51->a_return_segm , "vak42.p" , 20257)).integer.C_4.sp1r_returncode == (int ) a07_return_code(5000 , (unsigned char ) sql__setrang((*acv).a_sqlmode , 0 , 3)))
                {
                (*(tsp1_segment *) sql__nil(ptoc_Var51->a_return_segm , "vak42.p" , 20261)).integer.C_4.sp1r_returncode = 0;
                sysk = a01defaultkey;
                _ptoc_ASSIGN(unsigned char , 8, sysk.integer.C_2.stableid, (*lock_info).lshTabId_kb05)
                ptoc_Var52->a4p_arr.pbasep = (struct tak_systembuffer *) &sysbuf;
                a36get_catalog_record(&ptoc_Var51->a_transinf.tri_trans , &sysk , sql__nil(ptoc_Var52->a4p_arr.pbasep , "vak42.p" , 20266));
                if ((int ) ptoc_Var51->a_transinf.tri_trans.trError_gg00 == 0)
                    {
                    table_found = 1;
                    a06determine_username(&(*acv) , (*(struct tak_systembuffer *) sql__nil(ptoc_Var52->a4p_arr.pbasep , "vak42.p" , 20273)).integer.C_1.sbase.bauthid , owner);
                    a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) owner , sizeof(tsp00_C64 ));
                    _ptoc_ASSIGN(unsigned char , 64, tablename, a01_il_b_identifier)
                    tabname_pos = sizeof(tak_baserecord ) - sizeof(tak_columnarr ) - sizeof(tak_basecolinfo_ptr ) - sizeof(tsp00_KnlIdentifier *) + 1;
                    g10mv("VAK42 " , 6 , sizeof(tsp00_Buf ) , sizeof(tsp00_C64 ) , sysbuf.integer.C_11.buf , tabname_pos + 1 , tablename , 1 , sql__ord((int ) sysbuf.integer.C_11.buf[sql__setrang(tabname_pos , 1 , 8192) + -1]) , &(*(tsp1_segment *) sql__nil(ptoc_Var51->a_return_segm , "vak42.p" , 20284)).integer.C_4.sp1r_returncode);
                    a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) tablename , sizeof(tsp00_C64 ));
                    ptoc_Var52->a4p_arr.pbasep = (struct tak_systembuffer *) (int *) 0;
                    }
                }
            }
        if (!(char ) sql__setrang(table_found , 0 , 1))
            {
            if ((char ) sql__setrang(g01unicode , 0 , 1))
                {
                help_tabname_len = 9;
                }
            else
                {
                help_tabname_len = 18;
                }
            a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) (int *) 0 , 0);
            if (_ptoc_MEMCMP(unsigned char , 8, (*lock_info).lshTabId_kb05 , g01tabid.sys2_cat.integer.C_4.fileTabId_gg00) == 0 ? 1 : 0)
                {
                a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) "SYS_2_CATALOG     " , help_tabname_len);
                }
            else
                {
                obj_class_char = (unsigned char ) sql__setrang((*lock_info).lshTabId_kb05[0] , 0 , 255);
                (*lock_info).lshTabId_kb05[0] = (unsigned char)'\0';
                if ((_ptoc_MEMCMP(unsigned char , 8, (*lock_info).lshTabId_kb05 , "\0\0\0\0\0\0\0\0") == 0 ? 1 : 0) && sql__ord((int ) obj_class_char) > 0)
                    {
                    a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) "OBJECT            " , help_tabname_len);
                    is_obj_lock = 1;
                    }
                else
                    {
                    a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) (int *) 0 , 0);
                    }
                }
            }
        a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) (*lock_info).lshTabId_kb05 , 8);
        a40move_i4(&(*acv) , &(*a41v) , (int ) (*lock_info).lshRowIdLen_kb05 , 0 , 0 , 1);
        if ((char ) sql__setrang((*lock_info).lshUndefRowId_kb05 , 0 , 1))
            {
            a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) (int *) 0 , 0);
            }
        else
            {
            if ((int ) (*lock_info).lshRowIdLen_kb05 <= sizeof(tkb05_RowId ))
                {
                length = (int ) (*lock_info).lshRowIdLen_kb05;
                }
            else
                {
                length = sizeof(tkb05_RowId );
                }
            a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) (*lock_info).lshRowId_kb05 , length);
            }
        if ((char ) sql__setrang((*lock_info).lshUndefRowId_kb05 , 0 , 1))
            {
            a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) (int *) 0 , 0);
            }
        else
            {
            if (!(char ) sql__setrang(table_found , 0 , 1) || ptoc_Var52->a4p_arr.pbasep == (struct tak_systembuffer *) (int *) 0)
                {
                if (!(char ) sql__setrang(is_obj_lock , 0 , 1))
                    {
                    a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) (int *) 0 , 0);
                    }
                else
                    {
                    if ((int ) (*lock_info).lshRowIdLen_kb05 <= sizeof(tkb05_RowId ))
                        {
                        length = (int ) (*lock_info).lshRowIdLen_kb05;
                        }
                    else
                        {
                        length = sizeof(tkb05_RowId );
                        }
                    a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) (*lock_info).lshRowId_kb05 , length);
                    }
                }
            else
                {
                if ((int ) (*lock_info).lshRowIdLen_kb05 <= sizeof(unsigned char [44]))
                    {
                    max_length = (int ) (*lock_info).lshRowIdLen_kb05;
                    }
                else
                    {
                    max_length = sizeof(unsigned char [44]);
                    }
                c_index = (int ) (*(struct tak_systembuffer *) sql__nil(ptoc_Var52->a4p_arr.pbasep , "vak42.p" , 20379)).integer.C_1.sbase.bfirstcolind;
                rowid_pos = 1;
                p = 1;
                ok = 1;
                while (c_index != 0 && (char ) sql__setrang(ok , 0 , 1))
                    {
                    ptoc_Var58 = &(*(struct tak_systembuffer *) sql__nil(ptoc_Var52->a4p_arr.pbasep , "vak42.p" , 20385)).integer.C_1.sbase;
                    ptoc_Var59 = (tak00_columninfo *) sql__nil(ptoc_Var58->integer.C_2.bcolumn[sql__setrang(c_index , 1 , 1536) + -1] , "vak42.p" , 20385);
                    if (0 || (ptoc_Var59->integer.C_1.ccolpropset[0] & 1) == 0 || rowid_pos > max_length)
                        {
                        c_index = 0;
                        }
                    else
                        {
                        if (rowid_pos + (int ) ptoc_Var59->integer.C_1.cinoutlen + -1 > max_length)
                            {
                            if ((ptoc_Var60 = (int ) ptoc_Var59->integer.C_1.cdatatyp) == 0 || ptoc_Var60 == 1)
                                {
                                length = 0;
                                }
                            else
                                {
                                length = max_length - rowid_pos + 1;
                                if ((int ) ptoc_Var59->integer.C_1.cdatatyp == 24 && ((ptoc_Var60 = (length + -1) % 2) >= 0 ? ptoc_Var60 : ptoc_Var60 + 2) != 0)
                                    {
                                    length = sql__pred(length , -2147483647 , 2147483647 , "vak42.p" , 20404);
                                    }
                                }
                            }
                        else
                            {
                            length = (int ) ptoc_Var59->integer.C_1.cinoutlen;
                            }
                        if (length > 0)
                            {
                            if ((int ) ptoc_Var59->integer.C_1.cdatatyp == 24)
                                {
                                g20unifill(sizeof(tsp00_C120 ) , (tsp00_MoveObj *) c120 , 1 , sizeof(tsp00_C120 ) , "\0 ");
                                }
                            else
                                {
                                s10fil(sizeof(tsp00_C120 ) , c120 , 1 , sizeof(tsp00_C120 ) , (unsigned char)' ');
                                }
                            a40put_one_value(&(*acv) , &(*a41v) , sql__nil(ptoc_Var58->integer.C_2.bcolumn[sql__setrang(c_index , 1 , 1536) + -1] , "vak42.p" , 20424) , 0 , (*lock_info).lshRowId_kb05 , rowid_pos , length , c120 , &val_length , &ok);
                            if (rowid_pos > 1)
                                {
                                ak42ascii_const_move((tsp00_MoveObj *) "." , 1 , &p , key_info , &(*(tsp1_segment *) sql__nil(ptoc_Var51->a_return_segm , "vak42.p" , 20431)).integer.C_4.sp1r_returncode);
                                }
                            switch ((int ) ptoc_Var59->integer.C_1.cdatatyp)
                                {
                                case 3 :
                                case 2 :
                                case 24 :
                                case 10 :
                                case 11 :
                                case 13 :
                                    ak42ascii_const_move((tsp00_MoveObj *) "\'" , 1 , &p , key_info , &(*(tsp1_segment *) sql__nil(ptoc_Var51->a_return_segm , "vak42.p" , 20439)).integer.C_4.sp1r_returncode);
                                    break;
                                case 4 :
                                    ak42ascii_const_move((tsp00_MoveObj *) "x" , 1 , &p , key_info , &(*(tsp1_segment *) sql__nil(ptoc_Var51->a_return_segm , "vak42.p" , 20444)).integer.C_4.sp1r_returncode);
                                    ak42ascii_const_move((tsp00_MoveObj *) "\'" , 1 , &p , key_info , &(*(tsp1_segment *) sql__nil(ptoc_Var51->a_return_segm , "vak42.p" , 20447)).integer.C_4.sp1r_returncode);
                                    break;
                                default :
                                    break;
                                }
                            if ((char ) sql__setrang(g01unicode , 0 , 1) && (int ) ptoc_Var59->integer.C_1.cdatatyp == 24 || !(char ) sql__setrang(g01unicode , 0 , 1) && (int ) ptoc_Var59->integer.C_1.cdatatyp != 24)
                                {
                                g10mv1("VAK42 " , 7 , sizeof(tsp00_C120 ) , sizeof(tsp00_C256 ) , c120 , 1 , key_info , p , val_length , &(*(tsp1_segment *) sql__nil(ptoc_Var51->a_return_segm , "vak42.p" , 20466)).integer.C_4.sp1r_returncode);
                                p = p + val_length;
                                }
                            else
                                {
                                if ((char ) sql__setrang(g01unicode , 0 , 1) && (int ) ptoc_Var59->integer.C_1.cdatatyp != 24)
                                    {
                                    ak42ascii_const_move((tsp00_MoveObj *) c120 , val_length , &p , key_info , &(*(tsp1_segment *) sql__nil(ptoc_Var51->a_return_segm , "vak42.p" , 20476)).integer.C_4.sp1r_returncode);
                                    }
                                else
                                    {
                                    s40gbyte(c120 , 1 , val_length , key_info , p , val_length * 2 , &is_trunc);
                                    p = p + val_length * 2;
                                    }
                                }
                            if ((ptoc_Var60 = (int ) ptoc_Var59->integer.C_1.cdatatyp) == 2 || ptoc_Var60 == 4 || ptoc_Var60 == 3 || ptoc_Var60 == 24 || ptoc_Var60 == 10 || ptoc_Var60 == 11 || ptoc_Var60 == 13)
                                {
                                ak42ascii_const_move((tsp00_MoveObj *) "\'" , 1 , &p , key_info , &(*(tsp1_segment *) sql__nil(ptoc_Var51->a_return_segm , "vak42.p" , 20492)).integer.C_4.sp1r_returncode);
                                }
                            }
                        rowid_pos = rowid_pos + (int ) ptoc_Var59->integer.C_1.cinoutlen;
                        c_index = (int ) ptoc_Var59->integer.C_1.cnextind;
                        }
                    }
                if ((char ) sql__setrang(ok , 0 , 1))
                    {
                    if (max_length < (int ) (*lock_info).lshRowIdLen_kb05)
                        {
                        _ptoc_ASSIGN(unsigned char , 3, c3, "...")
                        ak42ascii_const_move((tsp00_MoveObj *) c3 , 3 , &p , key_info , &(*(tsp1_segment *) sql__nil(ptoc_Var51->a_return_segm , "vak42.p" , 20512)).integer.C_4.sp1r_returncode);
                        curr_move_len = p + -1;
                        }
                    else
                        {
                        curr_move_len = p + -1;
                        }
                    }
                else
                    {
                    p = 1;
                    ak42ascii_const_move((tsp00_MoveObj *) " " , 1 , &p , key_info , &(*(tsp1_segment *) sql__nil(ptoc_Var51->a_return_segm , "vak42.p" , 20523)).integer.C_4.sp1r_returncode);
                    curr_move_len = p + -1;
                    }
                a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) key_info , curr_move_len);
                }
            }
        }
    cccprint("<ak42one_lock_in");
    }



int ak42appl_info(acv, a41v, process_id)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
tsp00_TaskId process_id;
    {
    char ok;
    tsp00_Int4 appl_pid;
    tsp00_NodeId appl_node;
    cccprint("AK42  ak42appl_i");
    vgetapplpid(process_id , appl_node , &appl_pid , &ok);
    a40move_i4(&(*acv) , &(*a41v) , appl_pid , (char ) sql__setrang(!(char ) sql__setrang(ok , 0 , 1) , 0 , 1) , 0 , 0);
    if ((char ) sql__setrang(ok , 0 , 1))
        {
        a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) appl_node , sizeof(tsp00_C64 ));
        }
    else
        {
        a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) (int *) 0 , 0);
        }
    cccprint("<ak42appl_info  ");
    }



int ak42lock_mode(acv, a41v, lock_mode)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
tkb05_TransLockMode lock_mode;
    {
    char do_move;
    tsp00_C14 id;
    cccprint("AK42  ak42lock_m");
    do_move = 1;
    switch ((int ) lock_mode)
        {
        case 0 :
            do_move = 0;
            a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) (int *) 0 , 0);
            break;
        case 1 :
            _ptoc_ASSIGN(unsigned char , 14, id, "share         ")
            break;
        case 2 :
            _ptoc_ASSIGN(unsigned char , 14, id, "exclusive     ")
            break;
        case 3 :
            _ptoc_ASSIGN(unsigned char , 14, id, "exclusive     ")
            break;
        default :
            (unsigned char ) sql__caseerr("vak42.p" , 20579);
            break;
        }
    if ((char ) sql__setrang(do_move , 0 , 1))
        {
        a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) id , sizeof(tsp00_C14 ));
        }
    cccprint("<ak42lock_mode  ");
    }



int ak42lock_state(acv, a41v, lock_mode)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
tkb05_TransLockMode lock_mode;
    {
    tsp00_C10 id;
    cccprint("AK42  ak42lock_s");
    switch ((int ) lock_mode)
        {
        case 2 :
            _ptoc_ASSIGN(unsigned char , 10, id, "write     ")
            a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) id , sizeof(tsp00_C10 ));
            break;
        default :
            a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) (int *) 0 , 0);
            break;
        }
    cccprint("<ak42lock_state ");
    }



int ak42lock_req_mode(acv, a41v, lock_mode)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
tgg00_LockReqMode lock_mode;
    {
    char do_move;
    tsp00_C14 id;
    cccprint("AK42  ak42lock_r");
    do_move = 1;
    switch ((int ) lock_mode)
        {
        case 0 :
            do_move = 0;
            a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) (int *) 0 , 0);
            break;
        case 1 :
            _ptoc_ASSIGN(unsigned char , 14, id, "sys_share     ")
            break;
        case 2 :
            _ptoc_ASSIGN(unsigned char , 14, id, "sys_exclusive ")
            break;
        case 3 :
            _ptoc_ASSIGN(unsigned char , 14, id, "tab_share     ")
            break;
        case 4 :
            _ptoc_ASSIGN(unsigned char , 14, id, "tab_exclusive ")
            break;
        case 5 :
            _ptoc_ASSIGN(unsigned char , 14, id, "row_share     ")
            break;
        case 6 :
            _ptoc_ASSIGN(unsigned char , 14, id, "row_exclusive ")
            break;
        case 7 :
            _ptoc_ASSIGN(unsigned char , 14, id, "row_optimistic")
            break;
        default :
            (unsigned char ) sql__caseerr("vak42.p" , 20639);
            break;
        }
    if ((char ) sql__setrang(do_move , 0 , 1))
        {
        a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) id , sizeof(tsp00_C14 ));
        }
    cccprint("<ak42lock_req_mo");
    }



int ak42lock_req_state(acv, a41v, ptoc_ptr_lock_state)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
tgg00_LockReqState ptoc_ptr_lock_state;
    {
    char do_move;
    tsp00_C10 id;
    tgg00_LockReqState lock_state;
    _ptoc_MOVE(char, sizeof(tgg00_LockReqState ), lock_state, ptoc_ptr_lock_state);
    cccprint("AK42  ak42lock_r");
    do_move = 1;
    if (1 && (lock_state[0] & 2) != 0)
        {
        _ptoc_ASSIGN(unsigned char , 10, id, "write     ")
        }
    else
        {
        if (1 && (lock_state[0] & 1) != 0)
            {
            _ptoc_ASSIGN(unsigned char , 10, id, "consistent")
            }
        else
            {
            if (1 && (lock_state[0] & 4) != 0)
                {
                _ptoc_ASSIGN(unsigned char , 10, id, "temp      ")
                }
            else
                {
                do_move = 0;
                a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) (int *) 0 , 0);
                }
            }
        }
    if ((char ) sql__setrang(do_move , 0 , 1))
        {
        a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) id , sizeof(tsp00_C10 ));
        }
    cccprint("<ak42lock_req_st");
    }



int ak42serverdb_statistics(acv, a41v)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
    {
    tgg00_DbSpaceInfo db_space_info;
    tsp00_Int4 log_percent;
    tsp00_Int4 restart_date;
    tsp00_Int4 restart_time;
    tsp00_Int4 unused_pages;
    tkb00_LogDeviceInfo log_device_info;
    tkb00_LogQueueInfo log_queue_info;
    tak_all_command_glob *ptoc_Var61;
    tak40_show_glob *ptoc_Var62;
    tgg00_DbSpaceInfo *ptoc_Var63;
    tkb00_LogDeviceInfo *ptoc_Var64;
    cccprint("AK42  ak42server");
    a40get_catalog_table(&(*acv) , &(*a41v));
    ptoc_Var61 = &*acv;
    ptoc_Var62 = &*a41v;
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var61->a_return_segm , "vak42.p" , 20728)).integer.C_4.sp1r_returncode == 0)
        {
        b01dbspace_statistic(&ptoc_Var61->a_transinf.tri_trans , &db_space_info);
        if ((int ) ptoc_Var61->a_transinf.tri_trans.trError_gg00 != 0)
            {
            a07_b_put_error(&(*acv) , (short ) sql__setrang(ptoc_Var61->a_transinf.tri_trans.trError_gg00 , -32768 , 32767) , 1);
            }
        else
            {
            ptoc_Var63 = &db_space_info;
            a40move_i4(&(*acv) , &(*a41v) , ptoc_Var63->db_total_pages , 0 , 0 , 0);
            a40move_i4(&(*acv) , &(*a41v) , 2147483647 , 1 , 0 , 0);
            a40move_i4(&(*acv) , &(*a41v) , ptoc_Var63->db_perm_pages , 0 , 0 , 0);
            a40move_i4(&(*acv) , &(*a41v) , ptoc_Var63->db_max_perm_used , 0 , 0 , 0);
            a40move_i4(&(*acv) , &(*a41v) , ptoc_Var63->db_real_perm_used , 0 , 0 , 0);
            a40move_i4(&(*acv) , &(*a41v) , (int ) ptoc_Var63->db_perm_percent , 0 , 0 , 0);
            a40move_i4(&(*acv) , &(*a41v) , ptoc_Var63->db_temp_pages , 0 , 0 , 0);
            a40move_i4(&(*acv) , &(*a41v) , (int ) ptoc_Var63->db_temp_percent , 0 , 0 , 0);
            unused_pages = ptoc_Var63->db_total_pages - ptoc_Var63->db_used_pages;
            a40move_i4(&(*acv) , &(*a41v) , unused_pages , 0 , 0 , 0);
            a40move_i4(&(*acv) , &(*a41v) , ak42evalpercent(unused_pages , ptoc_Var63->db_total_pages) , 0 , 0 , 0);
            a40move_i4(&(*acv) , &(*a41v) , ptoc_Var63->db_updated_pages , 0 , 0 , 0);
            a40move_i4(&(*acv) , &(*a41v) , ptoc_Var63->db_used_blocks , 0 , 0 , 0);
            a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) a40yes_no[sql__setrang((int ) b01db_full((*acv).a_transinf.tri_trans.trTaskId_gg00) , 0 , 1)] , 3);
            kb560GetStatistics(ptoc_Var61->a_transinf.tri_trans.trTaskId_gg00 , &log_device_info , &log_queue_info);
            ptoc_Var64 = &log_device_info;
            a40move_i4(&(*acv) , &(*a41v) , kb560GetLogDeviceSize() , 0 , 0 , 0);
            a40move_i4(&(*acv) , &(*a41v) , ptoc_Var64->ldiPagesUsed_kb00 , 0 , 0 , 0);
            log_percent = ak42evalpercent(ptoc_Var64->ldiPagesUsed_kb00 , kb560GetLogDeviceSize());
            a40move_i4(&(*acv) , &(*a41v) , log_percent , 0 , 0 , 0);
            a40move_i4(&(*acv) , &(*a41v) , ptoc_Var64->ldiPagesNotSaved_kb00 , 0 , 0 , 0);
            log_percent = ak42evalpercent(ptoc_Var64->ldiPagesNotSaved_kb00 , kb560GetLogDeviceSize());
            a40move_i4(&(*acv) , &(*a41v) , log_percent , 0 , 0 , 0);
            a40move_i4(&(*acv) , &(*a41v) , ptoc_Var64->ldiPagesSinceBup_kb00 , 0 , 0 , 0);
            a40move_i4(&(*acv) , &(*a41v) , g01autosave_distance , 0 , 0 , 0);
            a40move_i4(&(*acv) , &(*a41v) , ptoc_Var64->ldiSaveptCount_kb00 , 0 , 0 , 0);
            a40move_i4(&(*acv) , &(*a41v) , 0 , 0 , 0 , 0);
            a36restart_time(&restart_date , &restart_time);
            a40put_date_time(&(*acv) , &(*a41v) , &restart_date , 1 , 0);
            a40put_date_time(&(*acv) , &(*a41v) , &restart_time , 0 , 0);
            if (g01is_livecache())
                {
                a40move_i4(&(*acv) , &(*a41v) , 0 , 0 , 0 , 0);
                a40move_i4(&(*acv) , &(*a41v) , 0 , 0 , 0 , 0);
                }
            else
                {
                a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) (int *) 0 , 0);
                a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) (int *) 0 , 0);
                }
            }
        }
    cccprint("<ak42serverdb_st");
    }



int ak42state_statistics(acv, a41v)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
    {
    char is_all;
    char found;
    char realy_bad;
    tsp00_CodeType basic_code;
    tsp00_CodeType messcode;
    tsp2_dev_type dev_type;
    int i;
    int requested;
    tsp00_Int4 devno;
    tsp00_Sname nam;
    tsp00_C12 off_on[2];
    tsp00_C30 v30;
    struct ptoc_Type215
      {
      union
        {
        struct
          {
          tsp00_VFilename devn;
          } C_true;
        struct
          {
          tsp00_C64 filter;
          } C_false;
        } boolean;
      } analyze_filter;
    struct ptoc_Type216
      {
      union
        {
        struct
          {
          tsp00_VFilename devn;
          } C_true;
        struct
          {
          tsp00_KnlIdentifier ts_name;
          tsp00_C8 ts_code;
          } C_false;
        } boolean;
      } termsetinfo;
    tsp2_devname devname;
    tak_keyword keyword;
    tak_keyword kindOut;
    tsp00_Sname topicKeyOut;
    int levelOut;
    tak_all_command_glob *ptoc_Var65;
    tak40_show_glob *ptoc_Var66;
    int ptoc_Var67;
    cccprint("AK42  ak42state_");
    ptoc_Var65 = &*acv;
    ptoc_Var66 = &*a41v;
    is_all = (char ) sql__setrang((ptoc_Var67 = (int ) ptoc_Var65->a_current_user_kind) == 4 || ptoc_Var67 == 5 , 0 , 1);
    ptoc_Var66->a4sh_kind = 37;
    a40get_catalog_table(&(*acv) , &(*a41v));
    if ((int ) ptoc_Var65->a_ex_kind != 0 && (int ) (*(tsp1_segment *) sql__nil(ptoc_Var65->a_return_segm , "vak42.p" , 20861)).integer.C_4.sp1r_returncode == 0)
        {
        _ptoc_ASSIGN(unsigned char , 12, off_on[0], "off         ")
        _ptoc_ASSIGN(unsigned char , 12, off_on[1], "on          ")
        _ptoc_ASSIGN(unsigned char , 30, v30, "Monitoring                    ")
        ak42sname_statistic(&(*acv) , &(*a41v) , v30 , off_on[1]);
        _ptoc_ASSIGN(unsigned char , 30, v30, "Archive log writing           ")
        ak42sname_statistic(&(*acv) , &(*a41v) , v30 , off_on[sql__setrang((int ) kb560IsLogDeviceEnabled() , 0 , 1)]);
        if ((char ) sql__setrang(a01diag_monitor_on , 0 , 1))
            {
            if (a01sm_reads < 2147483647)
                {
                ak42datab_statistic(&(*acv) , &(*a41v) , "Diagnose monitor read         " , a01sm_reads);
                }
            if (a01sm_selectivity != -2)
                {
                ak42datab_statistic(&(*acv) , &(*a41v) , "Diagnose monitor selectivity  " , a01sm_selectivity);
                }
            if (a01sm_milliseconds < 2147483647)
                {
                ak42datab_statistic(&(*acv) , &(*a41v) , "Diagnose monitor time         " , a01sm_milliseconds);
                }
            ak42datab_statistic(&(*acv) , &(*a41v) , "Monitor rows                  " , a545sm_get_rowno());
            _ptoc_ASSIGN(unsigned char , 30, v30, "Monitor data collection       ")
            ak42sname_statistic(&(*acv) , &(*a41v) , v30 , off_on[sql__setrang((int ) (char ) sql__setrang(a01sm_collect_data , 0 , 1) , 0 , 1)]);
            }
        if ((char ) sql__setrang(a01diag_analyze_on , 0 , 1))
            {
            _ptoc_ASSIGN(unsigned char , 30, v30, "Diagnose Analyze              ")
            if ((char ) sql__setrang(a01diag_ana_coll_data , 0 , 1))
                {
                _ptoc_ASSIGN(unsigned char , 12, nam, "counting    ")
                }
            else
                {
                _ptoc_ASSIGN(unsigned char , 12, nam, "on          ")
                }
            ak42sname_statistic(&(*acv) , &(*a41v) , v30 , nam);
            _ptoc_ASSIGN(unsigned char , 30, v30, "Diagnose Analyze Filter       ")
            _ptoc_ASSIGN(unsigned char , 64, analyze_filter.boolean.C_true.devn, b01blankfilename)
            a544get_analyze_filter(analyze_filter.boolean.C_false.filter);
            ak42hostname_statistic(&(*acv) , &(*a41v) , v30 , analyze_filter.boolean.C_true.devn);
            }
        _ptoc_ASSIGN(unsigned char , 30, v30, "SERVERDB is for SAP           ")
        if ((char ) sql__setrang(g01glob.db_is_for_sapr3 , 0 , 1))
            {
            _ptoc_ASSIGN(unsigned char , 12, nam, "yes         ")
            }
        else
            {
            _ptoc_ASSIGN(unsigned char , 12, nam, "no          ")
            }
        ak42sname_statistic(&(*acv) , &(*a41v) , v30 , nam);
        _ptoc_ASSIGN(unsigned char , 30, v30, "SERVERDB full                 ")
        if (b01db_full((*acv).a_transinf.tri_trans.trTaskId_gg00))
            {
            _ptoc_ASSIGN(unsigned char , 12, nam, "yes         ")
            }
        else
            {
            _ptoc_ASSIGN(unsigned char , 12, nam, "no          ")
            }
        ak42sname_statistic(&(*acv) , &(*a41v) , v30 , nam);
        _ptoc_ASSIGN(unsigned char , 30, v30, "Connect possible              ")
        if (b01connect_allowed())
            {
            _ptoc_ASSIGN(unsigned char , 12, nam, "yes         ")
            }
        else
            {
            _ptoc_ASSIGN(unsigned char , 12, nam, "no          ")
            }
        ak42sname_statistic(&(*acv) , &(*a41v) , v30 , nam);
        if (g01is_livecache())
            {
            _ptoc_ASSIGN(unsigned char , 30, v30, "Last shutdown finished ok     ")
            ak42snullname_statistic(&(*acv) , &(*a41v) , v30);
            _ptoc_ASSIGN(unsigned char , 30, v30, "Monitor OMS started           ")
            if (ak341IsMonitorOn())
                {
                _ptoc_ASSIGN(unsigned char , 12, nam, "yes         ")
                }
            else
                {
                _ptoc_ASSIGN(unsigned char , 12, nam, "no          ")
                }
            ak42sname_statistic(&(*acv) , &(*a41v) , v30 , nam);
            }
        if ((char ) sql__setrang(is_all , 0 , 1))
            {
            ak42datab_statistic(&(*acv) , &(*a41v) , "Catalog cache pages used      " , a10cache_used());
            }
        _ptoc_ASSIGN(unsigned char , 30, v30, "Checkpoint wanted             ")
        if (k51is_closed_locklist(ptoc_Var65->a_transinf.tri_trans.trTaskId_gg00))
            {
            _ptoc_ASSIGN(unsigned char , 12, nam, "yes         ")
            }
        else
            {
            _ptoc_ASSIGN(unsigned char , 12, nam, "no          ")
            }
        ak42sname_statistic(&(*acv) , &(*a41v) , v30 , nam);
        if (!(char ) sql__setrang(g01stand_alone , 0 , 1))
            {
            _ptoc_ASSIGN(unsigned char , 12, nam, "disconnected")
            _ptoc_ASSIGN(unsigned char , 30, v30, "Distribution state            ")
            ak42sname_statistic(&(*acv) , &(*a41v) , v30 , nam);
            }
        ak42datab_statistic(&(*acv) , &(*a41v) , "No of bad indexes             " , b01get_bad_index_cnt(ptoc_Var65->a_transinf.tri_trans.trTaskId_gg00));
        _ptoc_ASSIGN(unsigned char , 30, v30, "Vtrace                        ")
        if ((char ) sql__setrang(g01vtrace.vtrAk_gg00 , 0 , 1))
            {
            _ptoc_ASSIGN(unsigned char , 12, nam, "AK          ")
            ak42sname_statistic(&(*acv) , &(*a41v) , v30 , nam);
            }
        if ((char ) sql__setrang(g01vtrace.vtrAll_gg00 , 0 , 1))
            {
            _ptoc_ASSIGN(unsigned char , 12, nam, "DEFAULT     ")
            ak42sname_statistic(&(*acv) , &(*a41v) , v30 , nam);
            }
        if ((char ) sql__setrang(g01vtrace.vtrAkDelete_gg00 , 0 , 1))
            {
            _ptoc_ASSIGN(unsigned char , 12, nam, "DELETE      ")
            ak42sname_statistic(&(*acv) , &(*a41v) , v30 , nam);
            }
        if ((char ) sql__setrang(g01vtrace.vtrBdIndex_gg00 , 0 , 1))
            {
            _ptoc_ASSIGN(unsigned char , 12, nam, "INDEX       ")
            ak42sname_statistic(&(*acv) , &(*a41v) , v30 , nam);
            }
        if ((char ) sql__setrang(g01vtrace.vtrAkInsert_gg00 , 0 , 1))
            {
            _ptoc_ASSIGN(unsigned char , 12, nam, "INSERT      ")
            ak42sname_statistic(&(*acv) , &(*a41v) , v30 , nam);
            }
        if ((char ) sql__setrang(g01vtrace.vtrKbLock_gg00 , 0 , 1))
            {
            _ptoc_ASSIGN(unsigned char , 12, nam, "LOCK        ")
            ak42sname_statistic(&(*acv) , &(*a41v) , v30 , nam);
            }
        if ((char ) sql__setrang(g01vtrace.vtrBdString_gg00 , 0 , 1))
            {
            _ptoc_ASSIGN(unsigned char , 12, nam, "LONG        ")
            ak42sname_statistic(&(*acv) , &(*a41v) , v30 , nam);
            }
        if ((char ) sql__setrang(g01vtrace.vtrStrategy_gg00 , 0 , 1))
            {
            _ptoc_ASSIGN(unsigned char , 12, nam, "OPTIMIZE    ")
            ak42sname_statistic(&(*acv) , &(*a41v) , v30 , nam);
            }
        if ((char ) sql__setrang(g01vtrace.vtrAkPacket_gg00 , 0 , 1))
            {
            _ptoc_ASSIGN(unsigned char , 12, nam, "ORDER       ")
            ak42sname_statistic(&(*acv) , &(*a41v) , v30 , nam);
            }
        if ((char ) sql__setrang(g01vtrace.vtrAkShortPacket_gg00 , 0 , 1))
            {
            _ptoc_ASSIGN(unsigned char , 12, nam, "ORDER STNDRD")
            ak42sname_statistic(&(*acv) , &(*a41v) , v30 , nam);
            }
        if ((char ) sql__setrang(g01vtrace.vtrIoTrace_gg00 , 0 , 1))
            {
            _ptoc_ASSIGN(unsigned char , 12, nam, "PAGES       ")
            ak42sname_statistic(&(*acv) , &(*a41v) , v30 , nam);
            }
        if ((char ) sql__setrang(g01vtrace.vtrAkSelect_gg00 , 0 , 1))
            {
            _ptoc_ASSIGN(unsigned char , 12, nam, "SELECT      ")
            ak42sname_statistic(&(*acv) , &(*a41v) , v30 , nam);
            }
        if ((char ) sql__setrang(g01vtrace.vtrBdPrim_gg00 , 0 , 1))
            {
            _ptoc_ASSIGN(unsigned char , 12, nam, "TABLE       ")
            ak42sname_statistic(&(*acv) , &(*a41v) , v30 , nam);
            }
        if ((char ) sql__setrang(g01vtrace.vtrTime_gg00 , 0 , 1))
            {
            _ptoc_ASSIGN(unsigned char , 12, nam, "TIME        ")
            ak42sname_statistic(&(*acv) , &(*a41v) , v30 , nam);
            }
        if ((char ) sql__setrang(g01vtrace.vtrAkUpdate_gg00 , 0 , 1))
            {
            _ptoc_ASSIGN(unsigned char , 12, nam, "UPDATE      ")
            ak42sname_statistic(&(*acv) , &(*a41v) , v30 , nam);
            }
        if ((char ) sql__setrang(g01vtrace.vtrBdObject_gg00 , 0 , 1))
            {
            _ptoc_ASSIGN(unsigned char , 12, nam, "OBJECT      ")
            ak42sname_statistic(&(*acv) , &(*a41v) , v30 , nam);
            }
        if ((char ) sql__setrang(g01vtrace.vtrOmsNew_gg00 , 0 , 1))
            {
            _ptoc_ASSIGN(unsigned char , 12, nam, "OBJECT ADD  ")
            ak42sname_statistic(&(*acv) , &(*a41v) , v30 , nam);
            }
        if ((char ) sql__setrang(g01vtrace.vtrOmsGet_gg00 , 0 , 1))
            {
            _ptoc_ASSIGN(unsigned char , 12, nam, "OBJECT GET  ")
            ak42sname_statistic(&(*acv) , &(*a41v) , v30 , nam);
            }
        if ((char ) sql__setrang(g01vtrace.vtrOmsUpd_gg00 , 0 , 1))
            {
            _ptoc_ASSIGN(unsigned char , 12, nam, "OBJECT ALTER")
            ak42sname_statistic(&(*acv) , &(*a41v) , v30 , nam);
            }
        if ((char ) sql__setrang(g01vtrace.vtrOmsFree_gg00 , 0 , 1))
            {
            _ptoc_ASSIGN(unsigned char , 12, nam, "OBJECT FREE ")
            ak42sname_statistic(&(*acv) , &(*a41v) , v30 , nam);
            }
        requested = 0;
        while (Kernel_DiagInfop(requested , kindOut , topicKeyOut , &levelOut))
            {
            if (levelOut > 0)
                {
                a42name_and_val_statistic(&(*acv) , &(*a41v) , kindOut , levelOut , topicKeyOut);
                }
            requested = requested + 1;
            }
        if ((char ) sql__setrang(g01vtrace.vtrCheck_gg00 , 0 , 1))
            {
            _ptoc_ASSIGN(unsigned char , 30, v30, "Vtrace check                  ")
            _ptoc_ASSIGN(unsigned char , 12, nam, "on          ")
            ak42sname_statistic(&(*acv) , &(*a41v) , v30 , nam);
            }
        _ptoc_ASSIGN(unsigned char , 30, v30, "Check                         ")
        if (kb03IsBackupCheck())
            {
            _ptoc_ASSIGN(unsigned char , 12, nam, "BACKUP      ")
            ak42sname_statistic(&(*acv) , &(*a41v) , v30 , nam);
            }
        if (kb03IsLockCheck())
            {
            _ptoc_ASSIGN(unsigned char , 12, nam, "LOCK        ")
            ak42sname_statistic(&(*acv) , &(*a41v) , v30 , nam);
            }
        if (kb03IsLockSupplyCheck())
            {
            _ptoc_ASSIGN(unsigned char , 12, nam, "LOCK SUPPLY ")
            ak42sname_statistic(&(*acv) , &(*a41v) , v30 , nam);
            }
        if (kb03IsLogCheck())
            {
            _ptoc_ASSIGN(unsigned char , 12, nam, "LOG         ")
            ak42sname_statistic(&(*acv) , &(*a41v) , v30 , nam);
            }
        if (kb03IsLogQueueCheck())
            {
            _ptoc_ASSIGN(unsigned char , 12, nam, "LOG QUEUE   ")
            ak42sname_statistic(&(*acv) , &(*a41v) , v30 , nam);
            }
        if (kb03IsRegionCheck())
            {
            _ptoc_ASSIGN(unsigned char , 12, nam, "KB REGIONS  ")
            ak42sname_statistic(&(*acv) , &(*a41v) , v30 , nam);
            }
        if (kb03IsTransCheck())
            {
            _ptoc_ASSIGN(unsigned char , 12, nam, "TRANSLIST   ")
            ak42sname_statistic(&(*acv) , &(*a41v) , v30 , nam);
            }
        if ((char ) sql__setrang(g01glob.datacachecheck , 0 , 1))
            {
            _ptoc_ASSIGN(unsigned char , 12, nam, "DATACACHE   ")
            ak42sname_statistic(&(*acv) , &(*a41v) , v30 , nam);
            }
        if ((char ) sql__setrang(g01glob.bd_lock_check , 0 , 1))
            {
            _ptoc_ASSIGN(unsigned char , 12, nam, "TREE LOCKS  ")
            ak42sname_statistic(&(*acv) , &(*a41v) , v30 , nam);
            }
        if ((char ) sql__setrang(g01glob.treecheck , 0 , 1))
            {
            _ptoc_ASSIGN(unsigned char , 12, nam, "TREE        ")
            ak42sname_statistic(&(*acv) , &(*a41v) , v30 , nam);
            }
        if (_ptoc_MEMCMP(unsigned char , 4, g01vtrace.vtrSession_gg00.ci4_gg00 , "\377\377\377\377") == 0 ? 0 : 1)
            {
            _ptoc_ASSIGN(unsigned char , 30, v30, "Vtrace session                ")
            ak42session_to_sname(g01vtrace.vtrSession_gg00 , sizeof(tsp00_C12 ) , nam);
            ak42sname_statistic(&(*acv) , &(*a41v) , v30 , nam);
            }
        if ((char ) sql__setrang(g01vtrace.vtrRetcodeCheck_gg00 , 0 , 1))
            {
            _ptoc_ASSIGN(unsigned char , 18, keyword, "Stop on error     ")
            _ptoc_ASSIGN(unsigned char , 12, nam, "on          ")
            a42name_and_val_statistic(&(*acv) , &(*a41v) , keyword , (int ) g01vtrace.vtrStopRetcode_gg00 , nam);
            }
        a37event_state(&(*acv) , &(*a41v));
        dev_type = 2;
        devno = 1;
        while ((int ) dev_type != 7)
            {
            b01next_bad_dev(ptoc_Var65->a_transinf.tri_trans.trTaskId_gg00 , &dev_type , &devno , devname , &realy_bad);
            if (_ptoc_MEMCMP(unsigned char , 64, devname , b01blankfilename) == 0 ? 0 : 1)
                {
                if ((char ) sql__setrang(realy_bad , 0 , 1))
                    {
                    _ptoc_ASSIGN(unsigned char , 30, v30, "Bad DEVSPACE                  ")
                    }
                else
                    {
                    _ptoc_ASSIGN(unsigned char , 30, v30, "DEVSPACE in reintegration     ")
                    }
                ak42hostname_statistic(&(*acv) , &(*a41v) , v30 , devname);
                }
            }
        _ptoc_ASSIGN(unsigned char , 30, v30, "TERMCHAR SET                  ")
        i = 3;
        do
            {
            _ptoc_ASSIGN(unsigned char , 64, termsetinfo.boolean.C_true.devn, b01blankfilename)
            messcode = (unsigned char ) sql__setrang((unsigned char ) i , 0 , 255);
            g02gettermsetdesc(termsetinfo.boolean.C_false.ts_name , &messcode , &basic_code , &found);
            if ((char ) sql__setrang(found , 0 , 1))
                {
                if ((int ) basic_code == 0)
                    {
                    _ptoc_ASSIGN(unsigned char , 8, termsetinfo.boolean.C_false.ts_code, " ASCII  ")
                    }
                else
                    {
                    if ((int ) basic_code == 1)
                        {
                        _ptoc_ASSIGN(unsigned char , 8, termsetinfo.boolean.C_false.ts_code, " EBCDIC ")
                        }
                    else
                        {
                        _ptoc_ASSIGN(unsigned char , 8, termsetinfo.boolean.C_false.ts_code, " error  ")
                        }
                    }
                ak42hostname_statistic(&(*acv) , &(*a41v) , v30 , termsetinfo.boolean.C_true.devn);
                }
            if (i == 10)
                {
                break;
                }
            i += 1;
            }
        while (!0);
        }
    cccprint("<ak42state_stati");
    }



int ak42ref_i4_statistic(acv, a41v, ptoc_ptr_textpar, lint)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
tsp00_C40 ptoc_ptr_textpar;
tsp00_Int4 lint;
    {
    tsp00_C40 textpar;
    _ptoc_MOVE(char, sizeof(tsp00_C40 ), textpar, ptoc_ptr_textpar);
    cccprint("AK42  ak42ref_i4");
    ak42ref_i4_stat_undef(&(*acv) , &(*a41v) , textpar , lint , 0);
    cccprint("<ak42ref_i4_stat");
    }



int ak42ref_i4_stat_undef(acv, a41v, ptoc_ptr_textpar, lint, zero_undef)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
tsp00_C40 ptoc_ptr_textpar;
tsp00_Int4 lint;
char zero_undef;
    {
    int col_cnt;
    tak_all_command_glob *ptoc_Var68;
    tak40_show_glob *ptoc_Var69;
    tsp00_C40 textpar;
    _ptoc_MOVE(char, sizeof(tsp00_C40 ), textpar, ptoc_ptr_textpar);
    cccprint("AK42  ak42ref_i4");
    ptoc_Var68 = &*acv;
    ptoc_Var69 = &*a41v;
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var68->a_return_segm , "vak42.p" , 21323)).integer.C_4.sp1r_returncode == 0)
        {
        ak42prefix(&(*acv) , &(*a41v) , &col_cnt);
        a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) textpar , sizeof(tsp00_C40 ));
        a40move_i4(&(*acv) , &(*a41v) , lint , 0 , 0 , (char ) sql__setrang(zero_undef , 0 , 1));
        col_cnt = col_cnt + 2;
        while (col_cnt < (int ) (*(struct tak_systembuffer *) sql__nil(ptoc_Var69->a4p_arr.pinfop , "vak42.p" , 21330)).integer.C_1.sbase.bmaxcol + -1)
            {
            col_cnt = col_cnt + 1;
            a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) (int *) 0 , 0);
            }
        }
    cccprint("<ak42ref_i4_stat");
    }



int ak42ref_stat_real(acv, a41v, ptoc_ptr_textpar, lreal)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
tsp00_C40 ptoc_ptr_textpar;
tsp00_Longreal lreal;
    {
    int col_cnt;
    tak_all_command_glob *ptoc_Var70;
    tak40_show_glob *ptoc_Var71;
    tsp00_C40 textpar;
    _ptoc_MOVE(char, sizeof(tsp00_C40 ), textpar, ptoc_ptr_textpar);
    cccprint("AK42  ak42ref_st");
    ptoc_Var70 = &*acv;
    ptoc_Var71 = &*a41v;
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var70->a_return_segm , "vak42.p" , 21355)).integer.C_4.sp1r_returncode == 0)
        {
        ak42prefix(&(*acv) , &(*a41v) , &col_cnt);
        a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) textpar , sizeof(tsp00_C40 ));
        a40move_real(&(*acv) , &(*a41v) , lreal , 0 , 0);
        col_cnt = col_cnt + 2;
        while (col_cnt < (int ) (*(struct tak_systembuffer *) sql__nil(ptoc_Var71->a4p_arr.pinfop , "vak42.p" , 21362)).integer.C_1.sbase.bmaxcol + -1)
            {
            col_cnt = col_cnt + 1;
            a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) (int *) 0 , 0);
            }
        }
    cccprint("<ak42ref_stat_re");
    }



int ak42prefix(acv, a41v, col_cnt)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
int *col_cnt;
    {
    tak40_show_glob *ptoc_Var72;
    cccprint("AK42  ak42prefix");
    ptoc_Var72 = &*a41v;
    *col_cnt = 0;
    switch ((int ) ptoc_Var72->a4sh_kind)
        {
        case 33 :
        case 86 :
        case 112 :
            a40table_column_to_shbuf(&(*acv) , &(*a41v) , ptoc_Var72->a4p_arr.pbasep , -1);
            if (ptoc_Var72->a4col_ptr == (tak00_columninfo *) (int *) 0)
                {
                a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) ptoc_Var72->a4coln , sizeof(tsp00_C64 ));
                }
            else
                {
                a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) (int *) 0 , 0);
                }
            if (ptoc_Var72->a4col_ptr != (tak00_columninfo *) (int *) 0)
                {
                a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) (*(tak00_columninfo *) sql__nil(ptoc_Var72->a4col_ptr , "vak42.p" , 21398)).integer.C_1.ccolumnn , sql__ord((int ) (*(tak00_columninfo *) sql__nil(ptoc_Var72->a4col_ptr , "vak42.p" , 21399)).integer.C_1.ccolumnn_len));
                }
            else
                {
                a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) (int *) 0 , 0);
                }
            *col_cnt = 4;
            break;
        case 36 :
            ak42monitor_type(&(*acv) , &(*a41v) , &(*col_cnt));
            break;
        case 38 :
        case 87 :
        case 95 :
            a40table_column_to_shbuf(&(*acv) , &(*a41v) , ptoc_Var72->a4p_arr.pbasep , -1);
            *col_cnt = 2;
            break;
        default :
            break;
        }
    cccprint("<ak42prefix     ");
    }



int ak42monitor_type(acv, a41v, col_cnt)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
int *col_cnt;
    {
    tsp00_C8 c8;
    cccprint("AK42  ak42monito");
    switch ((*a41v).a4ti)
        {
        case 48 :
            _ptoc_ASSIGN(unsigned char , 8, c8, "CACHES  ")
            break;
        case 250 :
            _ptoc_ASSIGN(unsigned char , 8, c8, "LOAD    ")
            break;
        case 253 :
            _ptoc_ASSIGN(unsigned char , 8, c8, "LOCK    ")
            break;
        case 254 :
            _ptoc_ASSIGN(unsigned char , 8, c8, "LOG     ")
            break;
        case 258 :
            _ptoc_ASSIGN(unsigned char , 8, c8, "LONG    ")
            break;
        case 340 :
            _ptoc_ASSIGN(unsigned char , 8, c8, "PAGES   ")
            break;
        case 394 :
            _ptoc_ASSIGN(unsigned char , 8, c8, "ROW     ")
            break;
        case 416 :
            _ptoc_ASSIGN(unsigned char , 8, c8, "SERVERDB")
            break;
        case 480 :
            _ptoc_ASSIGN(unsigned char , 8, c8, "TRANS   ")
            break;
        default :
            sql__caseerr("vak42.p" , 21433);
            break;
        }
    a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) c8 , sizeof(tsp00_C8 ));
    *col_cnt = 1;
    cccprint("<ak42monitor_typ");
    }



int ak42devspace_statistics(acv, a41v)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
    {
    tsp_int4_ak42monitor_oms dev_no;
    tsp00_Int4 dev_used_blocks;
    tsp00_Int4 free_pages;
    tsp_c4 devType;
    tsp2_dev_type dev_type;
    tsp2_devname dev_name;
    tsp_int4_ak42monitor_oms dev_size;
    char dev_online;
    tsp_c18 dev_state;
    tak_all_command_glob *ptoc_Var73;
    tak40_show_glob *ptoc_Var74;
    cccprint("AK42  ak42devspa");
    ptoc_Var73 = &*acv;
    ptoc_Var74 = &*a41v;
    ptoc_Var74->a4sh_kind = 32;
    a40get_catalog_table(&(*acv) , &(*a41v));
    if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak42.p" , 21483)).integer.C_4.sp1r_returncode == 0)
        {
        dev_no = -1;
        do
            {
            b01next_dev_info((*acv).a_transinf.tri_trans.trTaskId_gg00 , &dev_no , &dev_type , dev_name , &dev_size , &dev_used_blocks , &dev_online , dev_state);
            if (dev_no >= 0 && (int ) dev_type == 5)
                {
                a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) dev_name , sizeof(tsp00_C64 ));
                a40move_i4(&(*acv) , &(*a41v) , dev_size , 0 , 0 , 0);
                a40move_i4(&(*acv) , &(*a41v) , dev_used_blocks , 0 , 0 , 0);
                a40move_i4(&(*acv) , &(*a41v) , ak42evalpercent(dev_used_blocks , dev_size) , 0 , 0 , 0);
                free_pages = dev_size - dev_used_blocks;
                a40move_i4(&(*acv) , &(*a41v) , free_pages , 0 , 0 , 0);
                a40move_i4(&(*acv) , &(*a41v) , ak42evalpercent(free_pages , dev_size) , 0 , 0 , 0);
                }
            }
        while (!(dev_no == -1));
        }
    cccprint("<ak42devspace_st");
    }



int ak42evalpercent(cnt, max)
tsp00_Int4 cnt;
tsp00_Int4 max;
    {
    int _ak42evalpercent;
    cccprint("AK42  ak42evalpe");
    if (cnt > 0 && max > 0)
        {
        if (max >= 2147483)
            {
            max = max / 1000;
            cnt = cnt / 1000;
            }
        cnt = cnt * 1000;
        _ak42evalpercent = (cnt / max + 5) / 10;
        }
    else
        {
        _ak42evalpercent = 0;
        }
    cccprint("<ak42evalpercent");
    return(_ak42evalpercent);
    }



int ak42evalpercent_cnt(cnt, max)
tsp00_8ByteCounter cnt;
tsp00_8ByteCounter max;
    {
    int _ak42evalpercent_cnt;
    cccprint("AK42  ak42evalpe");
    if (cnt > (double ) 0.00000000000000000000e+000 && max > (double ) 0.00000000000000000000e+000)
        {
        _ak42evalpercent_cnt = sql__round(cnt * (double ) 1.00000000000000000000e+002 / max);
        }
    else
        {
        _ak42evalpercent_cnt = 0;
        }
    cccprint("<ak42evalpercent");
    return(_ak42evalpercent_cnt);
    }



int ak42evalpercent_real(cnt, max)
tsp00_Longreal cnt;
tsp00_Longreal max;
    {
    int _ak42evalpercent_real;
    cccprint("AK42  ak42evalpe");
    if (cnt > (double ) 0.00000000000000000000e+000 && max > (double ) 0.00000000000000000000e+000)
        {
        _ak42evalpercent_real = sql__round(cnt * (double ) 1.00000000000000000000e+002 / max);
        }
    else
        {
        _ak42evalpercent_real = 0;
        }
    cccprint("<ak42evalpercent");
    return(_ak42evalpercent_real);
    }



int ak42datab_statistic(acv, a41v, ptoc_ptr_v30, lint)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
tsp00_C30 ptoc_ptr_v30;
tsp00_Int4 lint;
    {
    tsp00_C30 v30;
    _ptoc_MOVE(char, sizeof(tsp00_C30 ), v30, ptoc_ptr_v30);
    cccprint("AK42  ak42datab_");
    ak42datab_stat_undef(&(*acv) , &(*a41v) , v30 , lint , 0);
    cccprint("<ak42datab_stati");
    }



int ak42datab_stat_undef(acv, a41v, ptoc_ptr_v30, lint, zero_undef)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
tsp00_C30 ptoc_ptr_v30;
tsp00_Int4 lint;
char zero_undef;
    {
    int col_cnt;
    int frac;
    tak_all_command_glob *ptoc_Var75;
    tak40_show_glob *ptoc_Var76;
    tsp00_C30 v30;
    _ptoc_MOVE(char, sizeof(tsp00_C30 ), v30, ptoc_ptr_v30);
    cccprint("AK42  ak42datab_");
    ptoc_Var75 = &*acv;
    ptoc_Var76 = &*a41v;
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var75->a_return_segm , "vak42.p" , 21613)).integer.C_4.sp1r_returncode == 0)
        {
        ak42prefix(&(*acv) , &(*a41v) , &col_cnt);
        a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) v30 , sizeof(tsp00_C30 ));
        a40move_i4(&(*acv) , &(*a41v) , lint , 0 , 0 , (char ) sql__setrang(zero_undef , 0 , 1));
        a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) (int *) 0 , 0);
        }
    cccprint("<ak42datab_stat_");
    }



int ak42log_statistics(acv, a41v)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
    {
    tsp00_Int4 log_percent;
    tsp00_Sname nam;
    tsp00_C30 v30;
    tkb00_LogDeviceInfo log_device_info;
    tkb00_LogQueueInfo log_queue_info;
    tak_all_command_glob *ptoc_Var77;
    tak40_show_glob *ptoc_Var78;
    tkb00_LogDeviceInfo *ptoc_Var79;
    cccprint("AK42  ak42log_st");
    ptoc_Var77 = &*acv;
    ptoc_Var78 = &*a41v;
    ptoc_Var78->a4sh_kind = 34;
    a40get_catalog_table(&(*acv) , &(*a41v));
    if ((int ) ptoc_Var77->a_ex_kind != 0 && (int ) (*(tsp1_segment *) sql__nil(ptoc_Var77->a_return_segm , "vak42.p" , 21648)).integer.C_4.sp1r_returncode == 0)
        {
        kb560GetStatistics(ptoc_Var77->a_transinf.tri_trans.trTaskId_gg00 , &log_device_info , &log_queue_info);
        _ptoc_ASSIGN(unsigned char , 12, nam, "SINGLE      ")
        _ptoc_ASSIGN(unsigned char , 30, v30, "Log mode                      ")
        ak42sname_statistic(&(*acv) , &(*a41v) , v30 , nam);
        ptoc_Var79 = &log_device_info;
        ak42datab_statistic(&(*acv) , &(*a41v) , "Log pages                     " , kb560GetLogDeviceSize());
        ak42datab_statistic(&(*acv) , &(*a41v) , "Used log pages                " , ptoc_Var79->ldiPagesUsed_kb00);
        log_percent = ak42evalpercent(ptoc_Var79->ldiPagesUsed_kb00 , kb560GetLogDeviceSize());
        ak42datab_statistic(&(*acv) , &(*a41v) , "Used log pages (%)            " , log_percent);
        ak42datab_statistic(&(*acv) , &(*a41v) , "Not saved log pages           " , ptoc_Var79->ldiPagesNotSaved_kb00);
        log_percent = ak42evalpercent(ptoc_Var79->ldiPagesNotSaved_kb00 , kb560GetLogDeviceSize());
        ak42datab_statistic(&(*acv) , &(*a41v) , "Not saved log pages (%)       " , log_percent);
        ak42datab_statistic(&(*acv) , &(*a41v) , "Log pages since data backup   " , ptoc_Var79->ldiPagesSinceBup_kb00);
        ak42datab_statistic(&(*acv) , &(*a41v) , "Log segment size              " , g01autosave_distance);
        ak42datab_statistic(&(*acv) , &(*a41v) , "Savepoints                    " , ptoc_Var79->ldiSaveptCount_kb00);
        ak42datab_statistic(&(*acv) , &(*a41v) , "Checkpoints                   " , 0);
        }
    cccprint("<ak42log_statist");
    }



int ak42user_statistics(acv, a41v)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
    {
    int ti;
    tak_all_command_glob *ptoc_Var80;
    tak40_show_glob *ptoc_Var81;
    cccprint("AK42  ak42user_s");
    ptoc_Var80 = &*acv;
    ptoc_Var81 = &*a41v;
    ptoc_Var81->a4sh_kind = 40;
    a40get_catalog_table(&(*acv) , &(*a41v));
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var80->a_return_segm , "vak42.p" , 21695)).integer.C_4.sp1r_returncode == 0)
        {
        if (_ptoc_MEMCMP(unsigned char , 64, ptoc_Var81->a4authname , a01_il_b_identifier) == 0 ? 1 : 0)
            {
            if ((int ) ptoc_Var80->a_current_user_kind == 4 || (int ) ptoc_Var80->a_current_user_kind == 1 || (int ) ptoc_Var80->a_current_user_kind == 5)
                {
                ak42all_user_st_show(&(*acv) , &(*a41v));
                }
            else
                {
                _ptoc_ASSIGN(unsigned char , 64, ptoc_Var81->a4authname, ptoc_Var80->a_curr_user_name)
                ak42one_st_user_show(&(*acv) , &(*a41v) , 1);
                }
            }
        else
            {
            if (_ptoc_MEMCMP(unsigned char , 64, ptoc_Var81->a4authname , a01_i_current) == 0 ? 1 : 0)
                {
                _ptoc_ASSIGN(unsigned char , 64, ptoc_Var81->a4authname, ptoc_Var80->a_curr_user_name)
                }
            ak42one_st_user_show(&(*acv) , &(*a41v) , 1);
            }
        }
    cccprint("<ak42user_statis");
    }



int ak42all_user_st_show(acv, a41v)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
    {
    tgg00_BasisError b_err;
    tgg00_SysInfoKey sysk;
    tak_all_command_glob *ptoc_Var82;
    tak40_show_glob *ptoc_Var83;
    cccprint("AK42  ak42all_us");
    ptoc_Var82 = &*acv;
    ptoc_Var83 = &*a41v;
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var82->a_return_segm , "vak42.p" , 21741)).integer.C_4.sp1r_returncode == 0 && (int ) ptoc_Var82->a_ex_kind != 0)
        {
        sysk = a01defaultkey;
        _ptoc_ASSIGN(unsigned char , 8, sysk.integer.C_3.sauthid, "\0\0\0\0\0\0\0\0")
        _ptoc_ASSIGN(unsigned char , 2, sysk.integer.C_2.sentrytyp, "\0M")
        do
            {
            a10next_sysinfo(&(*acv) , &sysk , 0 , 1 , "\0M" , &ptoc_Var83->a4sysbuf , &b_err);
            if ((int ) b_err == 0)
                {
                a40username_from_syskey(&sysk , ptoc_Var83->a4authname);
                ak42one_st_user_show(&(*acv) , &(*a41v) , 0);
                a40ignore_down_error(&(*acv));
                }
            }
        while (!((int ) b_err != 0 || (int ) (*(tsp1_segment *) sql__nil(ptoc_Var82->a_return_segm , "vak42.p" , 21760)).integer.C_4.sp1r_returncode != 0));
        if ((int ) b_err != 1420 && (int ) b_err != 0)
            {
            a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , 1);
            }
        }
    cccprint("<ak42all_user_st");
    }



int ak42one_st_user_show(acv, a41v, one_show)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
char one_show;
    {
    char ok;
    char counting;
    char is_dba;
    tgg00_BasisError b_err;
    tsp00_Int4 priv_count;
    tsp00_KnlIdentifier init_username;
    tgg00_Surrogate init_userid;
    tsp00_C8 ukind;
    tgg00_SysInfoKey userkey;
    tak_all_command_glob *ptoc_Var84;
    tak40_show_glob *ptoc_Var85;
    tak_userrecord *ptoc_Var86;
    tak_baserecord *ptoc_Var87;
    tak_userrecord *ptoc_Var88;
    cccprint("AK42  ak42one_st");
    ptoc_Var84 = &*acv;
    ptoc_Var85 = &*a41v;
    a06check_username(&(*acv) , ptoc_Var85->a4authname , &is_dba , &ok);
    counting = (char ) sql__setrang((int ) ptoc_Var84->a_ex_kind != 0 , 0 , 1);
    priv_count = 0;
    if (!(char ) sql__setrang(ok , 0 , 1))
        {
        a07_b_put_error(&(*acv) , 6530 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var84->a_ap_tree , "vak42.p" , 21799))[sql__setrang(ptoc_Var85->a4ti , 0 , 32767)].n_pos);
        }
    else
        {
        userkey = a01defaultkey;
        a51build_userkey(ptoc_Var85->a4authname , &ptoc_Var85->a4sysk);
        a10get_sysinfo(&(*acv) , &ptoc_Var85->a4sysk , 1 , &ptoc_Var85->a4sysbuf , &b_err);
        if ((int ) b_err == 0)
            {
            _ptoc_ASSIGN(unsigned char , 8, userkey.integer.C_3.sauthid, (*(struct tak_systembuffer *) sql__nil(ptoc_Var85->a4sysbuf , "vak42.p" , 21807)).integer.C_44.suserref.ru_user_id)
            }
        else
            {
            a07_b_put_error(&(*acv) , 6530 , 1);
            }
        }
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var84->a_return_segm , "vak42.p" , 21813)).integer.C_4.sp1r_returncode == 0)
        {
        _ptoc_ASSIGN(unsigned char , 2, userkey.integer.C_2.sentrytyp, "\0B")
        a10get_sysinfo(&(*acv) , &userkey , 0 , &ptoc_Var85->a4sysbuf , &b_err);
        }
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var84->a_return_segm , "vak42.p" , 21820)).integer.C_4.sp1r_returncode == 0)
        {
        ptoc_Var86 = &(*(struct tak_systembuffer *) sql__nil(ptoc_Var85->a4sysbuf , "vak42.p" , 21822)).integer.C_42.suser;
        if (!(char ) sql__setrang(one_show , 0 , 1))
            {
            if (_ptoc_MEMCMP(unsigned char , 64, ptoc_Var85->a4authname , ptoc_Var84->a_curr_user_name) == 0 ? 0 : 1)
                {
                if (!a21is_owner(&(*acv) , &(*(struct tak_systembuffer *) sql__nil(ptoc_Var85->a4sysbuf , "vak42.p" , 21828)).integer.C_42.suser))
                    {
                    counting = 0;
                    }
                }
            }
        else
            {
            if (0 || (ptoc_Var86->urecordtyp[0] & 2) == 0)
                {
                a07_b_put_error(&(*acv) , 6278 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var84->a_ap_tree , "vak42.p" , 21839))[sql__setrang(ptoc_Var85->a4ti , 0 , 32767)].n_pos);
                }
            else
                {
                if ((_ptoc_MEMCMP(unsigned char , 64, ptoc_Var85->a4authname , ptoc_Var84->a_curr_user_name) == 0 ? 0 : 1) && (_ptoc_MEMCMP(unsigned char , 8, userkey.integer.C_3.sauthid , ptoc_Var86->usergroup_id) == 0 ? 0 : 1))
                    {
                    if (!a21is_owner(&(*acv) , &(*(struct tak_systembuffer *) sql__nil(ptoc_Var85->a4sysbuf , "vak42.p" , 21844)).integer.C_42.suser))
                        {
                        a07_b_put_error(&(*acv) , 6530 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var84->a_ap_tree , "vak42.p" , 21847))[sql__setrang(ptoc_Var85->a4ti , 0 , 32767)].n_pos);
                        }
                    }
                }
            }
        }
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var84->a_return_segm , "vak42.p" , 21854)).integer.C_4.sp1r_returncode == 0 && (char ) sql__setrang(counting , 0 , 1))
        {
        _ptoc_ASSIGN(unsigned char , 64, init_username, ptoc_Var84->a_curr_user_name)
        _ptoc_ASSIGN(unsigned char , 8, init_userid, ptoc_Var84->a_curr_user_id)
        _ptoc_ASSIGN(unsigned char , 64, ptoc_Var84->a_curr_user_name, ptoc_Var85->a4authname)
        _ptoc_ASSIGN(unsigned char , 8, ptoc_Var84->a_curr_user_id, (*(struct tak_systembuffer *) sql__nil(ptoc_Var85->a4sysbuf , "vak42.p" , 21860)).integer.C_50.syskey.integer.C_3.sauthid)
        a40init_table_scan(&(*acv) , &(*a41v) , 0 , 1 , 0 , 0 , 0 , 0);
        while (a40next_table(&(*acv) , &(*a41v)))
            {
            ptoc_Var87 = &(*(struct tak_systembuffer *) sql__nil(ptoc_Var85->a4p_arr.pbasep , "vak42.p" , 21864)).integer.C_1.sbase;
            if (((int ) ptoc_Var87->btablekind == 1 || (int ) ptoc_Var87->btablekind == 2) && (int ) ptoc_Var87->btreeid.integer.C_4.fileTfn_gg00 != 26)
                {
                a42_get_pagecount(&(*acv) , ptoc_Var85->a4p_arr.pbasep , &priv_count);
                }
            }
        _ptoc_ASSIGN(unsigned char , 64, ptoc_Var84->a_curr_user_name, init_username)
        _ptoc_ASSIGN(unsigned char , 8, ptoc_Var84->a_curr_user_id, init_userid)
        }
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var84->a_return_segm , "vak42.p" , 21880)).integer.C_4.sp1r_returncode == 0 && (char ) sql__setrang(counting , 0 , 1))
        {
        ptoc_Var88 = &(*(struct tak_systembuffer *) sql__nil(ptoc_Var85->a4sysbuf , "vak42.p" , 21886)).integer.C_42.suser;
        a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) ptoc_Var85->a4authname , sizeof(tsp00_C64 ));
        switch ((int ) ptoc_Var88->userkind)
            {
            case 1 :
                _ptoc_ASSIGN(unsigned char , 8, ukind, "DBA     ")
                break;
            case 2 :
                _ptoc_ASSIGN(unsigned char , 8, ukind, "RESOURCE")
                break;
            default :
                _ptoc_ASSIGN(unsigned char , 8, ukind, "STANDARD")
                break;
            }
        a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) ukind , sizeof(tsp00_C8 ));
        a40move_i4(&(*acv) , &(*a41v) , priv_count , (char ) sql__setrang((int ) ptoc_Var88->userkind == 3 , 0 , 1) , 0 , 0);
        if (_ptoc_MEMCMP(unsigned char , 64, ptoc_Var85->a4authname , ptoc_Var84->a_curr_user_name) == 0 ? 0 : 1)
            {
            a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) (int *) 0 , 0);
            }
        else
            {
            a40move_i4(&(*acv) , &(*a41v) , ptoc_Var84->a_transinf.tri_trans.trTempCount_gg00 , 0 , 0 , 0);
            }
        }
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var84->a_return_segm , "vak42.p" , 21913)).integer.C_4.sp1r_returncode == 0)
        {
        a10_key_del(&(*acv) , &(*(struct tak_systembuffer *) sql__nil(ptoc_Var85->a4sysbuf , "vak42.p" , 21915)).integer.C_50.syskey);
        }
    cccprint("<ak42one_st_user");
    }



int ak42show_config(acv, a41v)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
    {
    int kb_factor;
    tsp00_Int4 i;
    tsp00_Int4 no_of_data_devs;
    tsp00_Int4 no_of_log_devs;
    tsp00_Int4 totalPages;
    tsp00_Int4 unMapPages;
    tsp00_Sname nam;
    tsp00_C30 v30;
    tsp2_devname devname;
    tak_all_command_glob *ptoc_Var89;
    tak40_show_glob *ptoc_Var90;
    int ptoc_Var91;
    cccprint("AK42  ak42show_c");
    bd01DataCacheSize(&totalPages , &unMapPages);
    ptoc_Var89 = &*acv;
    ptoc_Var90 = &*a41v;
    ptoc_Var90->a4sh_kind = 30;
    a40get_catalog_table(&(*acv) , &(*a41v));
    if ((int ) ptoc_Var89->a_ex_kind != 0 && (int ) (*(tsp1_segment *) sql__nil(ptoc_Var89->a_return_segm , "vak42.p" , 21951)).integer.C_4.sp1r_returncode == 0)
        {
        _ptoc_ASSIGN(unsigned char , 30, v30, "Default code                  ")
        switch ((int ) g01code.char_default)
            {
            case 0 :
                _ptoc_ASSIGN(unsigned char , 12, nam, "ASCII       ")
                break;
            case 1 :
                _ptoc_ASSIGN(unsigned char , 12, nam, "EBCDIC      ")
                break;
            case 2 :
                _ptoc_ASSIGN(unsigned char , 12, nam, "INTERNAL    ")
                break;
            case 20 :
                _ptoc_ASSIGN(unsigned char , 12, nam, "UNICODE     ")
                break;
            default :
                _ptoc_ASSIGN(unsigned char , 12, nam, "            ")
                break;
            }
        ak42sname_statistic(&(*acv) , &(*a41v) , v30 , nam);
        _ptoc_ASSIGN(unsigned char , 30, v30, "Date/time format              ")
        switch ((int ) g01code.date_time)
            {
            case 1 :
                _ptoc_ASSIGN(unsigned char , 12, nam, "INTERNAL    ")
                break;
            case 4 :
            case 8 :
                _ptoc_ASSIGN(unsigned char , 12, nam, "EUR         ")
                break;
            case 2 :
                _ptoc_ASSIGN(unsigned char , 12, nam, "ISO         ")
                break;
            case 5 :
                _ptoc_ASSIGN(unsigned char , 12, nam, "JIS         ")
                break;
            case 3 :
                _ptoc_ASSIGN(unsigned char , 12, nam, "USA         ")
                break;
            default :
                _ptoc_ASSIGN(unsigned char , 12, nam, "            ")
                break;
            }
        ak42sname_statistic(&(*acv) , &(*a41v) , v30 , nam);
        ak42datab_statistic(&(*acv) , &(*a41v) , "SESSION TIMEOUT               " , g01timeout.timeCmd_gg00);
        ak42datab_statistic(&(*acv) , &(*a41v) , "REQUEST TIMEOUT               " , g01timeout.timeReq_gg00);
        _ptoc_ASSIGN(unsigned char , 30, v30, "Log mode                      ")
        g01log_mode(nam);
        ak42sname_statistic(&(*acv) , &(*a41v) , v30 , nam);
        ak42datab_statistic(&(*acv) , &(*a41v) , "Log segment size              " , g01autosave_distance);
        no_of_log_devs = b01no_of_log_devs();
        ak42datab_statistic(&(*acv) , &(*a41v) , "No of logs                    " , no_of_log_devs);
        no_of_data_devs = b01no_of_data_devs();
        ak42datab_statistic(&(*acv) , &(*a41v) , "No of DATA DEVSPACEs          " , no_of_data_devs);
        _ptoc_ASSIGN(unsigned char , 30, v30, "Mirrored DEVSPACEs            ")
        _ptoc_ASSIGN(unsigned char , 12, nam, "no          ")
        ak42sname_statistic(&(*acv) , &(*a41v) , v30 , nam);
        ptoc_Var91 = no_of_log_devs;
        if (1 <= ptoc_Var91)
            {
            i = 1;
            do
                {
                _ptoc_ASSIGN(unsigned char , 30, v30, "LOG size                      ")
                g17int4to_line(i , 0 , 2 , 12 , v30);
                ak42datab_statistic(&(*acv) , &(*a41v) , v30 , b01devsize(2 , i));
                if (i == ptoc_Var91)
                    {
                    break;
                    }
                i += 1;
                }
            while (!0);
            }
        ptoc_Var91 = no_of_log_devs;
        if (1 <= ptoc_Var91)
            {
            i = 1;
            do
                {
                _ptoc_ASSIGN(unsigned char , 30, v30, "LOG name                      ")
                g17int4to_line(i , 0 , 2 , 12 , v30);
                b01get_devname(2 , i , devname);
                ak42hostname_statistic(&(*acv) , &(*a41v) , v30 , devname);
                g01log_mode(nam);
                if (_ptoc_MEMCMP(unsigned char , 12, nam , "DUAL        ") == 0 ? 1 : 0)
                    {
                    v30[13] = (unsigned char)'\'';
                    b01get_devname(3 , i , devname);
                    ak42hostname_statistic(&(*acv) , &(*a41v) , v30 , devname);
                    }
                if (i == ptoc_Var91)
                    {
                    break;
                    }
                i += 1;
                }
            while (!0);
            }
        ptoc_Var91 = no_of_data_devs;
        if (1 <= ptoc_Var91)
            {
            i = 1;
            do
                {
                _ptoc_ASSIGN(unsigned char , 30, v30, "DATA DEVSPACE     size        ")
                g17int4to_line(i , 0 , 3 , 14 , v30);
                ak42datab_statistic(&(*acv) , &(*a41v) , v30 , b01devsize(5 , i));
                if (i == ptoc_Var91)
                    {
                    break;
                    }
                i += 1;
                }
            while (!0);
            }
        ptoc_Var91 = no_of_data_devs;
        if (1 <= ptoc_Var91)
            {
            i = 1;
            do
                {
                _ptoc_ASSIGN(unsigned char , 30, v30, "DATA DEVSPACE     name        ")
                g17int4to_line(i , 0 , 3 , 14 , v30);
                b01get_devname(5 , i , devname);
                ak42hostname_statistic(&(*acv) , &(*a41v) , v30 , devname);
                if (i == ptoc_Var91)
                    {
                    break;
                    }
                i += 1;
                }
            while (!0);
            }
        kb_factor = 8;
        ak42datab_statistic(&(*acv) , &(*a41v) , "Converter size                " , bd01ConverterSize() * kb_factor);
        ak42datab_statistic(&(*acv) , &(*a41v) , "Datacache size                " , totalPages * kb_factor);
        if (RTEMem_AWEAvailable())
            {
            ak42datab_statistic(&(*acv) , &(*a41v) , "AWE mapped size               " , (totalPages - unMapPages) * kb_factor);
            ak42datab_statistic(&(*acv) , &(*a41v) , "AWE unmapped size             " , unMapPages * kb_factor);
            }
        }
    cccprint("<ak42show_config");
    }



int ak42hex(alpha_char, outnam, outpos)
unsigned char alpha_char;
tsp00_C32 outnam;
int outpos;
    {
    int i;
    int dec;
    int ptoc_Var92;
    int ptoc_Var93;
    cccprint("AK42  ak42hex   ");
    dec = sql__ord((int ) alpha_char);
    outnam[sql__setrang(outpos , 1 , 32) + -1] = (unsigned char ) sql__setrang((unsigned char ) (dec / 16) , 0 , 255);
    outnam[sql__setrang(outpos + 1 , 1 , 32) + -1] = (unsigned char ) sql__setrang((unsigned char ) ((ptoc_Var92 = dec % 16) >= 0 ? ptoc_Var92 : ptoc_Var92 + 16) , 0 , 255);
    ptoc_Var92 = outpos;
    ptoc_Var93 = outpos + 1;
    if (ptoc_Var92 <= ptoc_Var93)
        {
        i = ptoc_Var92;
        do
            {
            if (sql__ord((int ) outnam[sql__setrang(i , 1 , 32) + -1]) > 9)
                {
                outnam[sql__setrang(i , 1 , 32) + -1] = (unsigned char ) sql__setrang((unsigned char ) (sql__ord(65) + sql__ord((int ) outnam[sql__setrang(i , 1 , 32) + -1]) + -10) , 0 , 255);
                }
            else
                {
                outnam[sql__setrang(i , 1 , 32) + -1] = (unsigned char ) sql__setrang((unsigned char ) (sql__ord(48) + sql__ord((int ) outnam[sql__setrang(i , 1 , 32) + -1])) , 0 , 255);
                }
            if (i == ptoc_Var93)
                {
                break;
                }
            i += 1;
            }
        while (!0);
        }
    cccprint("<ak42hex        ");
    }



int ak42show_version(acv, a41v)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
    {
    tsp00_Version rdsvers;
    tsp00_Version lzuvers;
    tak_all_command_glob *ptoc_Var94;
    tak40_show_glob *ptoc_Var95;
    cccprint("AK42  ak42show_v");
    ptoc_Var94 = &*acv;
    ptoc_Var95 = &*a41v;
    ptoc_Var95->a4sh_kind = 56;
    a40get_catalog_table(&(*acv) , &(*a41v));
    if ((int ) ptoc_Var94->a_ex_kind != 0 && (int ) (*(tsp1_segment *) sql__nil(ptoc_Var94->a_return_segm , "vak42.p" , 22105)).integer.C_4.sp1r_returncode == 0)
        {
        g11kernel_version(rdsvers);
        vversion(rdsvers , lzuvers);
        a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) rdsvers , sizeof(tsp00_C40 ));
        a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) lzuvers , sizeof(tsp00_C40 ));
        }
    cccprint("<ak42show_versio");
    }



int a42iindex_statistics(acv, a41v)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
    {
    int jx;
    int colindex;
    tsp00_KnlIdentifier null_value;
    tak00_colinfo_ptr col_ptr;
    tsp00_KnlIdentifier indexname;
    tak_index_scan_record index_scan_rec;
    tgg00_SampleInfo data;
    tgg00_QualBuf qual;
    struct tgg00_MessBlock mblock;
    tak_all_command_glob *ptoc_Var96;
    tak40_show_glob *ptoc_Var97;
    tak_index_scan_record *ptoc_Var98;
    struct ptoc_Type96 *ptoc_Var99;
    tak_baserecord *ptoc_Var100;
    cccprint("AK42  a42iindex_");
    g01mblock_init(&(*acv).a_transinf.tri_trans , 0 , 0 , &mblock);
    g01stack_init(&mblock , (tgg00_StackList *) (int *) 0 , 0 , (tgg00_QualBuf *) &qual , sizeof(tgg00_QualBuf ));
    g01datapart_init(&mblock , (tgg00_DataPart *) &data , sizeof(tgg00_SampleInfo ));
    ptoc_Var96 = &*acv;
    ptoc_Var97 = &*a41v;
    if ((char ) sql__setrang((*(struct tak_systembuffer *) sql__nil(ptoc_Var97->a4p_arr.pbasep , "vak42.p" , 22148)).integer.C_1.sbase.bindexexist , 0 , 1))
        {
        a24init_index_scan(&(*acv) , (*(struct tak_systembuffer *) sql__nil(ptoc_Var97->a4p_arr.pbasep , "vak42.p" , 22151)).integer.C_1.sbase.bsurrogate , &index_scan_rec);
        ptoc_Var98 = &index_scan_rec;
        while (a24next_named_index(&(*acv) , &index_scan_rec))
            {
            ptoc_Var99 = &(*(struct tak_systembuffer *) sql__nil(ptoc_Var98->isr_buf , "vak42.p" , 22155)).integer.C_22.smindex.indexdef[sql__setrang(ptoc_Var98->isr_index , 1 , 38) + -1];
            if ((int ) ptoc_Var99->indexno != 0)
                {
                ptoc_Var100 = &(*(struct tak_systembuffer *) sql__nil(ptoc_Var97->a4p_arr.pbasep , "vak42.p" , 22158)).integer.C_1.sbase;
                a24get_indexname(&(*acv) , ptoc_Var98->isr_buf , ptoc_Var98->isr_index , indexname);
                colindex = (int ) ptoc_Var100->integer.C_1.bextcolindex[sql__setrang((int ) ptoc_Var99->icolseq[0] , 1 , 1024) + -1] + (int ) ptoc_Var100->bfirstindex;
                if (a722test_col_qual(&(*acv) , &(*a41v) , "INDEXNAME         " , (unsigned char)' ' , (tsp00_MoveObj *) indexname , sizeof(tsp00_C64 )) && a722test_col_qual(&(*acv) , &(*a41v) , "COLUMNNAME        " , (unsigned char)'\377' , (tsp00_MoveObj *) null_value , 1))
                    {
                    ptoc_Var97->a4col_ptr = (tak00_columninfo *) (int *) 0;
                    _ptoc_ASSIGN(unsigned char , 64, ptoc_Var97->a4coln, indexname)
                    ak42one_index_statistics(&(*acv) , (unsigned char ) sql__setrang((*a41v).a4sh_kind , 0 , 151) , &(*(struct tak_systembuffer *) sql__nil(ptoc_Var97->a4p_arr.pbasep , "vak42.p" , 22178)).integer.C_1.sbase , &ptoc_Var99->icolstack[0] , &mblock);
                    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var96->a_return_segm , "vak42.p" , 22180)).integer.C_4.sp1r_returncode == 0)
                        {
                        ak42print_stat_tab_index(&(*acv) , &(*a41v) , &mblock , 1);
                        }
                    }
                }
            }
        a24finish_index_scan(&(*acv) , &index_scan_rec);
        }
    cccprint("<a42iindex_stati");
    }

static
  tgg00_FiletypeSet ptoc_Var101 =
  {
  (unsigned char)'\n'
  };


int a42table_statistics(acv, a41v, pages_only)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
char pages_only;
    {
    char estimate_pages;
    tgg00_BasisError b_err;
    tsp_int4_ak42monitor_oms page_cnt;
    tsp_int4_ak42monitor_oms val1;
    tsp_int4_ak42monitor_oms val2;
    tgg00_FileId treeid;
    tgg00_QualBuf qual;
    tgg00_SampleInfo data;
    struct tgg00_MessBlock mblock;
    tak_all_command_glob *ptoc_Var102;
    tak40_show_glob *ptoc_Var103;
    tak_baserecord *ptoc_Var104;
    cccprint("AK42  a42table_s");
    ptoc_Var102 = &*acv;
    ptoc_Var103 = &*a41v;
    t01int4(5 , "a4sh_kind   " , sql__ord((int ) ptoc_Var103->a4sh_kind));
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var102->a_return_segm , "vak42.p" , 22228)).integer.C_4.sp1r_returncode == 0 && (int ) ptoc_Var102->a_ex_kind != 0)
        {
        if ((char ) sql__setrang(pages_only , 0 , 1))
            {
            page_cnt = 2147483647;
            treeid = (*(struct tak_systembuffer *) sql__nil(ptoc_Var103->a4p_arr.pbasep , "vak42.p" , 22235)).integer.C_1.sbase.btreeid;
            treeid.integer.C_1.fileLeafNodes_gg00 = -1;
            estimate_pages = (char ) sql__setrang(ptoc_Var103->a4strategy.tgg07_StratEnum.C_strat_catalog.str_catalog.scat_col_needed[5] , 0 , 1);
            a06get_page_cnt(&(*acv) , &treeid , (char ) sql__setrang(estimate_pages , 0 , 1) , &page_cnt , &b_err);
            if ((int ) b_err == 0)
                {
                if ((char ) sql__setrang(estimate_pages , 0 , 1))
                    {
                    val1 = 0;
                    val2 = page_cnt;
                    }
                else
                    {
                    val1 = page_cnt;
                    val2 = 0;
                    }
                ak42datab_statistic(&(*acv) , &(*a41v) , "Leaf  pages                   " , val1);
                a40move_i4(&(*acv) , &(*a41v) , val2 , 0 , 0 , 1);
                val1 = 0;
                if ((int ) (*(struct tak_systembuffer *) sql__nil(ptoc_Var103->a4p_arr.pbasep , "vak42.p" , 22258)).integer.C_1.sbase.bstringcount > 0 && (char ) sql__setrang(ptoc_Var103->a4strategy.tgg07_StratEnum.C_strat_catalog.str_catalog.scat_col_needed[6] , 0 , 1))
                    {
                    _ptoc_MOVE(unsigned char , 1, treeid.integer.C_1.fileType_gg00, ptoc_Var101);
                    treeid.integer.C_4.fileTfn_gg00 = 18;
                    treeid.integer.C_1.fileRoot_gg00 = 2147483647;
                    _ptoc_ASSIGN(unsigned char , 2, treeid.integer.C_1.fileVersion_gg00.ci2_gg00, "\377\377")
                    a06get_page_cnt(&(*acv) , &treeid , (char ) sql__setrang(estimate_pages , 0 , 1) , &page_cnt , &b_err);
                    if ((int ) b_err == 0)
                        {
                        val1 = page_cnt;
                        }
                    }
                a40move_i4(&(*acv) , &(*a41v) , val1 , 0 , 0 , 1);
                }
            else
                {
                a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , 1);
                }
            }
        else
            {
            g01mblock_init(&ptoc_Var102->a_transinf.tri_trans , 0 , 0 , &mblock);
            g01stack_init(&mblock , (tgg00_StackList *) (int *) 0 , 0 , (tgg00_QualBuf *) &qual , sizeof(tgg00_QualBuf ));
            g01datapart_init(&mblock , (tgg00_DataPart *) &data , sizeof(tgg00_SampleInfo ));
            ak42table_statistics(&(*acv) , &(*(struct tak_systembuffer *) sql__nil(ptoc_Var103->a4p_arr.pbasep , "vak42.p" , 22287)).integer.C_1.sbase , &mblock);
            if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var102->a_return_segm , "vak42.p" , 22288)).integer.C_4.sp1r_returncode == 0)
                {
                ak42print_stat_tab_index(&(*acv) , &(*a41v) , &mblock , 0);
                ptoc_Var104 = &(*(struct tak_systembuffer *) sql__nil(ptoc_Var103->a4p_arr.pbasep , "vak42.p" , 22293)).integer.C_1.sbase;
                a28sys_upd_statistics(&(*acv) , &ptoc_Var104->btreeid , 61 , 41 , ptoc_Var104->bpages , (*(tgg00_DataPart *) sql__nil(mblock.integer.C_3.mb_data , "vak42.p" , 22295)).integer.C_7.mbp_sample.nodes);
                }
            }
        }
    cccprint("<a42table_statis");
    }



int ak42char2hex(input, output)
unsigned char input;
tsp00_C2 output;
    {
    int i;
    int dec;
    int hex_byte[2];
    int ptoc_Var105;
    cccprint("AK42  ak42char2h");
    dec = sql__ord((int ) input);
    hex_byte[0] = dec / 16;
    hex_byte[1] = (ptoc_Var105 = dec % 16) >= 0 ? ptoc_Var105 : ptoc_Var105 + 16;
    i = 1;
    do
        {
        if (hex_byte[sql__setrang(i , 1 , 2) + -1] > 9)
            {
            output[sql__setrang(i , 1 , 2) + -1] = (unsigned char ) sql__setrang((unsigned char ) (sql__ord(65) + hex_byte[sql__setrang(i , 1 , 2) + -1] + -10) , 0 , 255);
            }
        else
            {
            output[sql__setrang(i , 1 , 2) + -1] = (unsigned char ) sql__setrang((unsigned char ) (sql__ord(48) + hex_byte[sql__setrang(i , 1 , 2) + -1]) , 0 , 255);
            }
        if (i == 2)
            {
            break;
            }
        i += 1;
        }
    while (!0);
    cccprint("<ak42char2hex   ");
    }



int ak42one_index_statistics(acv, show_kind, baserec, st_entry, mblock)
tak_all_command_glob *acv;
tgg00_ShowKind show_kind;
tak_baserecord *baserec;
tgg00_StackEntry *st_entry;
struct tgg00_MessBlock *mblock;
    {
    tgg00_BasisError b_err;
    tgg00_FileId tree;
    tgg00_QualBuf *ptoc_Var106;
    cccprint("AK42  ak42one_in");
    if ((int ) show_kind == 112)
        {
        tree = (*baserec).btreeid;
        tree.integer.C_4.fileTfn_gg00 = 7;
        tree.integer.C_4.fileTfnNo_gg00[0] = (unsigned char ) sql__setrang((*st_entry).integer.C_1.ecol_tab[0] , 0 , 255);
        tree.integer.C_1.fileRoot_gg00 = 2147483647;
        _ptoc_ASSIGN(unsigned char , 2, tree.integer.C_1.fileVersion_gg00.ci2_gg00, "\377\377")
        a06get_page_cnt(&(*acv) , &tree , 0 , &(*(tgg00_DataPart *) sql__nil((*mblock).integer.C_3.mb_data , "vak42.p" , 22360)).integer.C_7.mbp_sample.nodes , &b_err);
        if ((int ) b_err == 715)
            {
            (*(tgg00_DataPart *) sql__nil((*mblock).integer.C_3.mb_data , "vak42.p" , 22364)).integer.C_7.mbp_sample.nodes = 0;
            b_err = 0;
            }
        }
    else
        {
        (*mblock).integer.C_2.mb_type = 32;
        (*mblock).integer.C_2.mb_type2 = 63;
        g01stack_init(&(*mblock) , (tgg00_StackList *) &*st_entry , sizeof(tgg00_StackEntry ) , (*mblock).integer.C_4.mb_qual , (*mblock).integer.C_2.mb_qual_size);
        a06set_mtree(&(*acv) , &(*mblock) , &(*baserec).btreeid);
        (*(tgg00_DataPart *) sql__nil((*mblock).integer.C_3.mb_data , "vak42.p" , 22377)).integer.C_7.mbp_sample.sam_rows = -1;
        (*(tgg00_DataPart *) sql__nil((*mblock).integer.C_3.mb_data , "vak42.p" , 22378)).integer.C_7.mbp_sample.sam_percent = 100;
        (*(tgg00_DataPart *) sql__nil((*mblock).integer.C_3.mb_data , "vak42.p" , 22380)).integer.C_7.mbp_sample.sam_with_long = 1;
        (*(tgg00_DataPart *) sql__nil((*mblock).integer.C_3.mb_data , "vak42.p" , 22381)).integer.C_7.mbp_sample.sam_fill1 = 0;
        (*(tgg00_DataPart *) sql__nil((*mblock).integer.C_3.mb_data , "vak42.p" , 22382)).integer.C_7.mbp_sample.sam_fill2 = 0;
        ptoc_Var106 = (tgg00_QualBuf *) sql__nil((*mblock).integer.C_4.mb_qual , "vak42.p" , 22383);
        ptoc_Var106->integer.C_2.mmult_pos = 1;
        ptoc_Var106->integer.C_2.mmult_cnt = 1;
        ptoc_Var106->integer.C_2.mfirst_free = 2;
        a06rsend_mess_buf(&(*acv) , &(*mblock) , 1 , &b_err);
        if ((int ) b_err == 715)
            {
            s10fil1((*mblock).integer.C_2.mb_qual_size , (*(tgg00_QualBuf *) sql__nil((*mblock).integer.C_4.mb_qual , "vak42.p" , 22394)).integer.C_1.buf , 1 , (*mblock).integer.C_2.mb_qual_size , (unsigned char)'\0');
            b_err = 0;
            }
        }
    if ((int ) b_err != 0)
        {
        a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , 1);
        }
    cccprint("<ak42one_index_s");
    }

static
  tgg00_HandlingSet ptoc_Var107 =
  {
  (unsigned char)'\001', (unsigned char)'\0'
  };


int ak42table_statistics(acv, baserec, mblock)
tak_all_command_glob *acv;
tak_baserecord *baserec;
struct tgg00_MessBlock *mblock;
    {
    tgg00_BasisError b_err;
    int colindex;
    tak_baserecord *ptoc_Var108;
    int ptoc_Var109;
    int ptoc_Var110;
    tak00_columninfo *ptoc_Var111;
    cccprint("AK42  ak42table_");
    (*mblock).integer.C_2.mb_type = 69;
    (*mblock).integer.C_2.mb_type2 = 0;
    if ((int ) (*baserec).bstringcount > 0)
        {
        (*mblock).integer.C_2.mb_type2 = 60;
        ptoc_Var108 = &*baserec;
        ptoc_Var109 = ptoc_Var108->bfirstindex;
        ptoc_Var110 = ptoc_Var108->blastindex;
        if (ptoc_Var109 <= ptoc_Var110)
            {
            colindex = ptoc_Var109;
            do
                {
                ptoc_Var111 = (tak00_columninfo *) sql__nil(ptoc_Var108->integer.C_2.bcolumn[sql__setrang(colindex , 1 , 1536) + -1] , "vak42.p" , 22429);
                if ((0 || (ptoc_Var111->integer.C_1.ccolpropset[0] & 128) == 0) && ((ptoc_Var109 = (int ) ptoc_Var111->integer.C_1.cdatatyp) == 6 || ptoc_Var109 == 7 || ptoc_Var109 == 8 || ptoc_Var109 == 9 || ptoc_Var109 == 34))
                    {
                    (*mblock).integer.C_2.mb_type2 = 0;
                    }
                if (colindex == ptoc_Var110)
                    {
                    break;
                    }
                colindex += 1;
                }
            while (!0);
            }
        }
    (*(tgg00_QualBuf *) sql__nil((*mblock).integer.C_4.mb_qual , "vak42.p" , 22440)).integer.C_2.mtree = (*baserec).btreeid;
    (*(tgg00_QualBuf *) sql__nil((*mblock).integer.C_4.mb_qual , "vak42.p" , 22441)).integer.C_2.mtree.integer.C_1.fileLeafNodes_gg00 = -1;
    (_ptoc_MOVE(unsigned char , 2, (*(tgg00_QualBuf *) sql__nil((*mblock).integer.C_4.mb_qual , "vak42.p" , 22442)).integer.C_2.mtree.integer.C_1.fileHandling_gg00, ptoc_Var107) , (*(tgg00_QualBuf *) sql__nil((*mblock).integer.C_4.mb_qual , "vak42.p" , 22442)).integer.C_2.mtree.integer.C_1.fileHandling_gg00);
    (*(tgg00_DataPart *) sql__nil((*mblock).integer.C_3.mb_data , "vak42.p" , 22443)).integer.C_7.mbp_sample.sam_rows = -1;
    (*(tgg00_DataPart *) sql__nil((*mblock).integer.C_3.mb_data , "vak42.p" , 22444)).integer.C_7.mbp_sample.sam_percent = 100;
    (*(tgg00_DataPart *) sql__nil((*mblock).integer.C_3.mb_data , "vak42.p" , 22446)).integer.C_7.mbp_sample.sam_with_long = 1;
    (*(tgg00_DataPart *) sql__nil((*mblock).integer.C_3.mb_data , "vak42.p" , 22447)).integer.C_7.mbp_sample.sam_fill1 = 0;
    (*(tgg00_DataPart *) sql__nil((*mblock).integer.C_3.mb_data , "vak42.p" , 22448)).integer.C_7.mbp_sample.sam_fill2 = 0;
    a06rsend_mess_buf(&(*acv) , &(*mblock) , 1 , &b_err);
    if ((int ) b_err != 0)
        {
        a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , 1);
        }
    cccprint("<ak42table_stati");
    }



int ak42print_stat_tab_index(acv, a41v, mblock, selectivity)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
struct tgg00_MessBlock *mblock;
char selectivity;
    {
    int col_cnt;
    tsp00_C30 v30;
    tsp00_Sname nam;
    tak_all_command_glob *ptoc_Var112;
    tak40_show_glob *ptoc_Var113;
    tgg00_SampleInfo *ptoc_Var114;
    int ptoc_Var115;
    cccprint("AK42  ak42print_");
    ptoc_Var112 = &*acv;
    ptoc_Var113 = &*a41v;
    ptoc_Var114 = &(*(tgg00_DataPart *) sql__nil((*mblock).integer.C_3.mb_data , "vak42.p" , 22471)).integer.C_7.mbp_sample;
    switch ((int ) ptoc_Var113->a4sh_kind)
        {
        case 86 :
        case 87 :
            ak42prefix(&(*acv) , &(*a41v) , &col_cnt);
            if ((int ) ptoc_Var113->a4sh_kind == 86)
                {
                a40move_i4(&(*acv) , &(*a41v) , ptoc_Var114->sec_key_cnt , 0 , 0 , 0);
                }
            else
                {
                a40move_i4(&(*acv) , &(*a41v) , ptoc_Var114->records , 0 , 0 , 0);
                }
            a40move_i4(&(*acv) , &(*a41v) , ptoc_Var114->nodes , 0 , 0 , 0);
            break;
        case 112 :
            ak42datab_statistic(&(*acv) , &(*a41v) , "Leaf  pages                   " , ptoc_Var114->nodes);
            break;
        default :
            ak42datab_statistic(&(*acv) , &(*a41v) , "Root pno                      " , ptoc_Var114->rootsegm);
            if (1 && (ptoc_Var114->smplFileType_gg00[0] & 1) != 0)
                {
                if (1 && (ptoc_Var114->smplFileType_gg00[0] & 8) != 0)
                    {
                    _ptoc_ASSIGN(unsigned char , 12, nam, "temp concurr")
                    }
                else
                    {
                    _ptoc_ASSIGN(unsigned char , 12, nam, "temp        ")
                    }
                }
            else
                {
                if (1 && (ptoc_Var114->smplFileType_gg00[0] & 2) != 0)
                    {
                    _ptoc_ASSIGN(unsigned char , 12, nam, "perm        ")
                    }
                else
                    {
                    if (1 && (ptoc_Var114->smplFileType_gg00[0] & 4) != 0)
                        {
                        _ptoc_ASSIGN(unsigned char , 12, nam, "BYTESTR     ")
                        }
                    else
                        {
                        _ptoc_ASSIGN(unsigned char , 12, nam, "unknown     ")
                        }
                    }
                }
            _ptoc_ASSIGN(unsigned char , 30, v30, "Filetype                      ")
            ak42sname_statistic(&(*acv) , &(*a41v) , v30 , nam);
            ak42datab_statistic(&(*acv) , &(*a41v) , "Used  pages                   " , ptoc_Var114->nodes);
            ak42datab_statistic(&(*acv) , &(*a41v) , "Index pages                   " , ptoc_Var114->indexnodes);
            ak42datab_statistic(&(*acv) , &(*a41v) , "Leaf  pages                   " , ptoc_Var114->leafnodes);
            ak42datab_statistic(&(*acv) , &(*a41v) , "Index levels                  " , (int ) ptoc_Var114->hight);
            ak42datab_statistic(&(*acv) , &(*a41v) , "Space used in all   pages (%) " , ptoc_Var114->treecov);
            ak42datab_statistic(&(*acv) , &(*a41v) , "Space used in root  page  (%) " , ptoc_Var114->rootcov);
            ak42datab_statistic(&(*acv) , &(*a41v) , "Space used in index pages (%) " , ptoc_Var114->indexcov);
            ak42ref_i4_statistic(&(*acv) , &(*a41v) , "Space used in index pages (%) min       " , ptoc_Var114->min_indexcov);
            ak42ref_i4_statistic(&(*acv) , &(*a41v) , "Space used in index pages (%) max       " , ptoc_Var114->max_indexcov);
            ak42datab_statistic(&(*acv) , &(*a41v) , "Space used in leaf  pages (%) " , ptoc_Var114->leafcov);
            ak42ref_i4_statistic(&(*acv) , &(*a41v) , "Space used in leaf  pages (%) min       " , ptoc_Var114->min_leafcov);
            ak42ref_i4_statistic(&(*acv) , &(*a41v) , "Space used in leaf  pages (%) max       " , ptoc_Var114->max_leafcov);
            if ((ptoc_Var115 = (int ) ptoc_Var113->a4sh_kind) == 38)
                {
                ak42datab_statistic(&(*acv) , &(*a41v) , "Rows                          " , ptoc_Var114->records);
                ak42datab_stat_undef(&(*acv) , &(*a41v) , "Avg rows per page             " , ptoc_Var114->rec_per_page , (char ) sql__setrang(ptoc_Var114->records != 0 , 0 , 1));
                ak42datab_statistic(&(*acv) , &(*a41v) , "Min rows per page             " , (int ) ptoc_Var114->min_rec_per_page);
                ak42datab_statistic(&(*acv) , &(*a41v) , "Max rows per page             " , (int ) ptoc_Var114->max_rec_per_page);
                ak42datab_stat_undef(&(*acv) , &(*a41v) , "Avg row length                " , ptoc_Var114->ave_rec_length , (char ) sql__setrang(ptoc_Var114->max_rec_length == 0 , 0 , 1));
                ak42datab_stat_undef(&(*acv) , &(*a41v) , "Min row length                " , ptoc_Var114->min_rec_length , (char ) sql__setrang(ptoc_Var114->max_rec_length == 0 , 0 , 1));
                ak42datab_stat_undef(&(*acv) , &(*a41v) , "Max row length                " , ptoc_Var114->max_rec_length , 1);
                ak42datab_stat_undef(&(*acv) , &(*a41v) , "Avg key length                " , ptoc_Var114->ave_key_length , (char ) sql__setrang((int ) ptoc_Var114->max_key_length == 0 , 0 , 1));
                ak42datab_stat_undef(&(*acv) , &(*a41v) , "Min key length                " , (int ) ptoc_Var114->min_key_length , (char ) sql__setrang((int ) ptoc_Var114->max_key_length == 0 , 0 , 1));
                ak42datab_stat_undef(&(*acv) , &(*a41v) , "Max key length                " , (int ) ptoc_Var114->max_key_length , 1);
                ak42datab_stat_undef(&(*acv) , &(*a41v) , "Avg separator length          " , ptoc_Var114->ave_sep_length , (char ) sql__setrang((int ) ptoc_Var114->max_sep_length == 0 , 0 , 1));
                ak42datab_stat_undef(&(*acv) , &(*a41v) , "Min separator length          " , (int ) ptoc_Var114->min_sep_length , (char ) sql__setrang((int ) ptoc_Var114->max_sep_length == 0 , 0 , 1));
                ak42datab_stat_undef(&(*acv) , &(*a41v) , "Max separator length          " , (int ) ptoc_Var114->max_sep_length , 1);
                ak42datab_statistic(&(*acv) , &(*a41v) , "Defined LONG columns          " , ptoc_Var114->defined_stringcols);
                ak42datab_stat_undef(&(*acv) , &(*a41v) , "Avg LONG   column length      " , ptoc_Var114->ave_len_stringcol , (char ) sql__setrang(ptoc_Var114->defined_stringcols == 0 , 0 , 1));
                ak42datab_stat_undef(&(*acv) , &(*a41v) , "Min LONG   column length      " , ptoc_Var114->min_len_stringcol , (char ) sql__setrang(ptoc_Var114->defined_stringcols == 0 , 0 , 1));
                ak42datab_stat_undef(&(*acv) , &(*a41v) , "Max LONG   column length      " , ptoc_Var114->max_len_stringcol , (char ) sql__setrang(ptoc_Var114->defined_stringcols == 0 , 0 , 1));
                ak42datab_statistic(&(*acv) , &(*a41v) , "LONG column pages             " , ptoc_Var114->all_stringcol_pages);
                ak42datab_stat_undef(&(*acv) , &(*a41v) , "Avg pages per LONG column     " , ptoc_Var114->ave_stringcol_pages , (char ) sql__setrang(ptoc_Var114->all_stringcol_pages == 0 , 0 , 1));
                ak42datab_stat_undef(&(*acv) , &(*a41v) , "Min pages per LONG column     " , ptoc_Var114->min_stringcol_pages , (char ) sql__setrang(ptoc_Var114->all_stringcol_pages == 0 , 0 , 1));
                ak42datab_stat_undef(&(*acv) , &(*a41v) , "Max pages per LONG column     " , ptoc_Var114->max_stringcol_pages , (char ) sql__setrang(ptoc_Var114->all_stringcol_pages == 0 , 0 , 1));
                }
            else
                {
                ak42datab_statistic(&(*acv) , &(*a41v) , "Secondary keys (index lists)  " , ptoc_Var114->sec_key_cnt);
                ak42datab_stat_undef(&(*acv) , &(*a41v) , "Avg secondary key length      " , ptoc_Var114->avg_sec_key_len , (char ) sql__setrang(ptoc_Var114->max_sec_key_len == 0 , 0 , 1));
                ak42datab_statistic(&(*acv) , &(*a41v) , "Min secondary key length      " , ptoc_Var114->min_sec_key_len);
                ak42datab_statistic(&(*acv) , &(*a41v) , "Max secondary key length      " , ptoc_Var114->max_sec_key_len);
                ak42datab_stat_undef(&(*acv) , &(*a41v) , "Avg separator length          " , ptoc_Var114->ave_sep_length , (char ) sql__setrang((int ) ptoc_Var114->max_sep_length == 0 , 0 , 1));
                ak42datab_stat_undef(&(*acv) , &(*a41v) , "Min separator length          " , (int ) ptoc_Var114->min_sep_length , (char ) sql__setrang((int ) ptoc_Var114->max_sep_length == 0 , 0 , 1));
                ak42datab_stat_undef(&(*acv) , &(*a41v) , "Max separator length          " , (int ) ptoc_Var114->max_sep_length , 1);
                ak42datab_statistic(&(*acv) , &(*a41v) , "Primary keys                  " , ptoc_Var114->prim_key_cnt);
                ak42datab_stat_undef(&(*acv) , &(*a41v) , "Avg primary keys per list     " , ptoc_Var114->avg_prim_per_list / 10 , (char ) sql__setrang(ptoc_Var114->prim_key_cnt == 0 , 0 , 1));
                ak42datab_statistic(&(*acv) , &(*a41v) , "Min primary keys per list     " , ptoc_Var114->min_prim_per_list);
                ak42datab_statistic(&(*acv) , &(*a41v) , "Max primary keys per list     " , ptoc_Var114->max_prim_per_list);
                if ((char ) sql__setrang(selectivity , 0 , 1))
                    {
                    ak42datab_stat_undef(&(*acv) , &(*a41v) , "Values with selectivity <=  1%" , ptoc_Var114->invsel_1 , 0);
                    ak42datab_stat_undef(&(*acv) , &(*a41v) , "Values with selectivity <=  5%" , ptoc_Var114->invsel_5 , 0);
                    ak42datab_stat_undef(&(*acv) , &(*a41v) , "Values with selectivity <= 10%" , ptoc_Var114->invsel_10 , 0);
                    ak42datab_stat_undef(&(*acv) , &(*a41v) , "Values with selectivity <= 25%" , ptoc_Var114->invsel_25 , 0);
                    ak42datab_stat_undef(&(*acv) , &(*a41v) , "Values with selectivity >  25%" , ptoc_Var114->invsel_notsel , 0);
                    }
                }
            break;
        }
    cccprint("<ak42print_stat_");
    }



int a42_check_database(acv)
tak_all_command_glob *acv;
    {
    tsp00_NumError res;
    tsp00_Number number;
    int i;
    int sizes[2];
    tak_parsid parsid;
    tgg00_DbSpaceInfo db_space_info;
    tkb00_LogDeviceInfo log_device_info;
    tkb00_LogQueueInfo log_queue_info;
    tak_oldidentifier name;
    tak_all_command_glob *ptoc_Var116;
    cccprint("AK42  a42_check_");
    ptoc_Var116 = &*acv;
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var116->a_return_segm , "vak42.p" , 22615)).integer.C_4.sp1r_returncode == 0)
        {
        if ((int ) ptoc_Var116->a_ex_kind == 0)
            {
            ak42check_db_shortinfos(&(*acv));
            parsid.pid_session = ptoc_Var116->a_transinf.tri_trans.trSessionId_gg00;
            parsid.boolean.C_true.pid_parsk = ptoc_Var116->a_pars_last_key;
            parsid.boolean.C_true.pid_parsk.p_kind = 4;
            parsid.boolean.C_false.pid_appl_info[0] = (unsigned char ) sql__setrang((unsigned char ) sql__ord((int ) ptoc_Var116->a_precomp_info_byte) , 0 , 255);
            parsid.boolean.C_false.pid_dtime_info[0] = (unsigned char ) sql__setrang((unsigned char ) sql__ord(0) , 0 , 255);
            a06retpart_move(&(*acv) , (tsp00_MoveObj *) &parsid , sizeof(tak_parsid ));
            a06finish_curr_retpart(&(*acv) , 10 , 1);
            }
        else
            {
            b01dbspace_statistic(&ptoc_Var116->a_transinf.tri_trans , &db_space_info);
            if ((int ) ptoc_Var116->a_transinf.tri_trans.trError_gg00 != 0)
                {
                a07_b_put_error(&(*acv) , (short ) sql__setrang(ptoc_Var116->a_transinf.tri_trans.trError_gg00 , -32768 , 32767) , 1);
                }
            else
                {
                kb560GetStatistics(ptoc_Var116->a_transinf.tri_trans.trTaskId_gg00 , &log_device_info , &log_queue_info);
                sizes[0] = ak42evalpercent(db_space_info.db_used_pages , db_space_info.db_total_pages);
                sizes[1] = ak42evalpercent(log_device_info.ldiPagesUsed_kb00 , kb560GetLogDeviceSize());
                _ptoc_ASSIGN(unsigned char , 18, name, "SERVERDB          ")
                a06colname_retpart_move(&(*acv) , (tsp00_MoveObj *) name , 8 , 0);
                _ptoc_ASSIGN(unsigned char , 18, name, "LOG               ")
                a06colname_retpart_move(&(*acv) , (tsp00_MoveObj *) name , 3 , 0);
                a06finish_curr_retpart(&(*acv) , 2 , 2);
                ak42check_db_shortinfos(&(*acv));
                i = 1;
                do
                    {
                    s41plint(number , 2 , 2 , 0 , sizes[sql__setrang(i , 1 , 2) + -1] , &res);
                    if ((int ) res != 0)
                        {
                        number[0] = (unsigned char)'\377';
                        }
                    else
                        {
                        number[0] = (unsigned char)'\0';
                        }
                    a06retpart_move(&(*acv) , (tsp00_MoveObj *) &number[0] , sizeof(tsp00_C3 ));
                    if (i == 2)
                        {
                        break;
                        }
                    i += 1;
                    }
                while (!0);
                a06finish_curr_retpart(&(*acv) , 5 , 2);
                }
            }
        if ((int ) ptoc_Var116->a_comp_type == 0 || (_ptoc_MEMCMP(unsigned char , 5, ptoc_Var116->a_comp_vers , "62000") < 0 ? 1 : 0))
            {
            _ptoc_ASSIGN(unsigned char , 64, ptoc_Var116->a_result_name, a01_il_b_identifier)
            }
        else
            {
            _ptoc_ASSIGN(unsigned char , 64, ptoc_Var116->a_result_name, a01_zero_res_name)
            }
        }
    cccprint("<a42_check_datab");
    }

static
  tsp1_param_opt_set ptoc_Var117 =
  {
  (unsigned char)'\001'
  };


int ak42check_db_shortinfos(acv)
tak_all_command_glob *acv;
    {
    tsp1_param_info param_info;
    tsp1_param_info *ptoc_Var118;
    cccprint("AK42  ak42check_");
    ptoc_Var118 = &param_info;
    _ptoc_MOVE(unsigned char , 1, ptoc_Var118->sp1i_mode, ptoc_Var117);
    ptoc_Var118->sp1i_io_type = 1;
    ptoc_Var118->sp1i_data_type = 0;
    ptoc_Var118->sp1i_frac = (unsigned char)'\0';
    ptoc_Var118->sp1i_length = 2;
    ptoc_Var118->sp1i_in_out_len = 3;
    ptoc_Var118->sp1i_bufpos = 1;
    a06retpart_move(&(*acv) , (tsp00_MoveObj *) &param_info , sizeof(tsp1_param_info ));
    ptoc_Var118->sp1i_bufpos = ptoc_Var118->sp1i_bufpos + (int ) ptoc_Var118->sp1i_in_out_len;
    a06retpart_move(&(*acv) , (tsp00_MoveObj *) &param_info , sizeof(tsp1_param_info ));
    a06finish_curr_retpart(&(*acv) , 14 , 2);
    cccprint("<ak42check_db_sh");
    }



int ak42snullname_statistic(acv, a41v, v30)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
tsp00_C30 v30;
    {
    int col_cnt;
    tak_all_command_glob *ptoc_Var119;
    tak40_show_glob *ptoc_Var120;
    cccprint("AK42  ak42snulln");
    ptoc_Var119 = &*acv;
    ptoc_Var120 = &*a41v;
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var119->a_return_segm , "vak42.p" , 22733)).integer.C_4.sp1r_returncode == 0)
        {
        ak42prefix(&(*acv) , &(*a41v) , &col_cnt);
        a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) v30 , sizeof(tsp00_C30 ));
        a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) (int *) 0 , 0);
        a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) (int *) 0 , 0);
        }
    cccprint("<ak42snullname_s");
    }



int ak42sname_statistic(acv, a41v, v30, nam)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
tsp00_C30 v30;
tsp00_Sname nam;
    {
    int col_cnt;
    tak_all_command_glob *ptoc_Var121;
    tak40_show_glob *ptoc_Var122;
    cccprint("AK42  ak42sname_");
    ptoc_Var121 = &*acv;
    ptoc_Var122 = &*a41v;
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var121->a_return_segm , "vak42.p" , 22759)).integer.C_4.sp1r_returncode == 0)
        {
        ak42prefix(&(*acv) , &(*a41v) , &col_cnt);
        a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) v30 , sizeof(tsp00_C30 ));
        a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) (int *) 0 , 0);
        a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) nam , sizeof(tsp00_C12 ));
        }
    cccprint("<ak42sname_stati");
    }



int a42name_and_val_statistic(acv, a41v, vkw, value, nam)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
tak_keyword vkw;
tsp00_Int4 value;
tsp00_Sname nam;
    {
    int col_cnt;
    tak_all_command_glob *ptoc_Var123;
    tak40_show_glob *ptoc_Var124;
    cccprint("AK42  a42name_an");
    ptoc_Var123 = &*acv;
    ptoc_Var124 = &*a41v;
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var123->a_return_segm , "vak42.p" , 22785)).integer.C_4.sp1r_returncode == 0)
        {
        ak42prefix(&(*acv) , &(*a41v) , &col_cnt);
        a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) vkw , sizeof(tak_keyword ));
        if (value == 2147483647)
            {
            a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) (int *) 0 , 0);
            }
        else
            {
            a40move_i4(&(*acv) , &(*a41v) , value , 0 , 0 , 0);
            }
        a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) nam , sizeof(tsp00_C12 ));
        }
    cccprint("<a42name_and_val");
    }



int ak42hostname_statistic(acv, a41v, v30, nam)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
tsp00_C30 v30;
tsp00_VFilename nam;
    {
    tak_all_command_glob *ptoc_Var125;
    cccprint("AK42  ak42hostna");
    ptoc_Var125 = &*acv;
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var125->a_return_segm , "vak42.p" , 22814)).integer.C_4.sp1r_returncode == 0)
        {
        a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) v30 , sizeof(tsp00_C30 ));
        a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) (int *) 0 , 0);
        a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) nam , sizeof(tsp00_C64 ));
        }
    cccprint("<ak42hostname_st");
    }



int ak42get_refinfo(acv, info)
tak_all_command_glob *acv;
tgg00_ReferenceInfo *info;
    {
    tsp00_TaskId procid;
    tsp00_8ByteCounter _sseq_cache_total;
    tsp00_8ByteCounter _sseq_cache_hit;
    tsp00_8ByteCounter _sseq_cache_miss;
    tkb05_LockInfo lock_info;
    tkb00_LogQueueInfo log_queue_info;
    tkb00_LogDeviceInfo log_device_info;
    tak_all_command_glob *ptoc_Var126;
    tgg00_ReferenceInfo *ptoc_Var127;
    tkb00_LogQueueInfo *ptoc_Var128;
    tkb05_LockInfo *ptoc_Var129;
    cccprint("AK42  ak42get_re");
    ptoc_Var126 = &*acv;
    ptoc_Var127 = &*info;
    b01show_monitor(ptoc_Var126->a_transinf.tri_trans.trTaskId_gg00 , &(*info));
    k720show_monitor(&(*info));
    procid = ptoc_Var126->a_transinf.tri_trans.trTaskId_gg00;
    kb560GetStatistics(procid , &log_device_info , &log_queue_info);
    ptoc_Var128 = &log_queue_info;
    ptoc_Var127->boolean.C_false.reference_array[94] = (double ) ptoc_Var128->lqiLogQueueSize_kb00;
    ptoc_Var127->boolean.C_false.reference_array[98] = ptoc_Var128->lqiCountGroup_kb00;
    ptoc_Var127->boolean.C_false.reference_array[96] = ptoc_Var128->lqiCountInsert_kb00;
    ptoc_Var127->boolean.C_false.reference_array[97] = ptoc_Var128->lqiCountRequest_kb00;
    ptoc_Var127->boolean.C_false.reference_array[99] = ptoc_Var128->lqiCountWait_kb00;
    ptoc_Var127->boolean.C_false.reference_array[95] = (double ) ptoc_Var128->lqiMaxQueueUsed_kb00;
    ptoc_Var127->boolean.C_false.reference_array[100] = (double ) ptoc_Var128->lqiMaxWaitLogpage_kb00;
    ptoc_Var127->boolean.C_false.reference_array[101] = ptoc_Var128->lqiAvgSumPagewaits_kb00;
    k51info_lock(procid , &lock_info);
    ptoc_Var129 = &lock_info;
    ptoc_Var127->boolean.C_false.reference_array[84] = (double ) ptoc_Var129->infAvgUsed_kb05;
    ptoc_Var127->boolean.C_false.reference_array[85] = (double ) ptoc_Var129->infMaxUsed_kb05;
    ptoc_Var127->boolean.C_false.reference_array[86] = ptoc_Var129->infSqlCollisions_kb05;
    ptoc_Var127->boolean.C_false.reference_array[87] = ptoc_Var129->infEscalations_kb05;
    ptoc_Var127->boolean.C_false.reference_array[90] = ptoc_Var129->infDeadlocks_kb05;
    ptoc_Var127->boolean.C_false.reference_array[91] = ptoc_Var129->infSqlReqTimeouts_kb05 + ptoc_Var129->infObjReqTimeouts_kb05;
    ptoc_Var127->boolean.C_false.reference_array[88] = ptoc_Var129->infRowEntryAmount_kb05;
    ptoc_Var127->boolean.C_false.reference_array[89] = ptoc_Var129->infTabEntryAmount_kb05;
    ptoc_Var127->boolean.C_false.reference_array[175] = ptoc_Var129->infKbCalls_kb05;
    ptoc_Var127->boolean.C_false.reference_array[174] = (double ) 0.00000000000000000000e+000;
    k57info_seq_cache(procid , &_sseq_cache_total , &_sseq_cache_hit , &_sseq_cache_miss);
    ptoc_Var127->boolean.C_false.reference_array[8] = _sseq_cache_total;
    ptoc_Var127->boolean.C_false.reference_array[9] = _sseq_cache_hit;
    ptoc_Var127->boolean.C_false.reference_array[10] = _sseq_cache_miss;
    a260statistics(&ptoc_Var127->boolean.C_true.external_dbproc_calls , &ptoc_Var127->boolean.C_true.internal_dbproc_calls);
    a262statistics(&ptoc_Var127->boolean.C_true.external_trigger_calls , &ptoc_Var127->boolean.C_true.internal_trigger_calls);
    ptoc_Var127->boolean.C_false.reference_array[12] = ptoc_Var127->boolean.C_false.reference_array[0];
    ptoc_Var127->boolean.C_false.reference_array[13] = ptoc_Var127->boolean.C_false.reference_array[1];
    ptoc_Var127->boolean.C_false.reference_array[14] = ptoc_Var127->boolean.C_false.reference_array[2];
    if (g01is_livecache())
        {
        ptoc_Var127->boolean.C_false.reference_array[0] = ptoc_Var127->boolean.C_false.reference_array[0] + ptoc_Var127->boolean.C_false.reference_array[16] + ptoc_Var127->boolean.C_false.reference_array[20] + ptoc_Var127->boolean.C_false.reference_array[24];
        ptoc_Var127->boolean.C_false.reference_array[1] = ptoc_Var127->boolean.C_false.reference_array[1] + ptoc_Var127->boolean.C_false.reference_array[17] + ptoc_Var127->boolean.C_false.reference_array[21] + ptoc_Var127->boolean.C_false.reference_array[25];
        ptoc_Var127->boolean.C_false.reference_array[2] = ptoc_Var127->boolean.C_false.reference_array[2] + ptoc_Var127->boolean.C_false.reference_array[18] + ptoc_Var127->boolean.C_false.reference_array[22] + ptoc_Var127->boolean.C_false.reference_array[26];
        }
    cccprint("<ak42get_refinfo");
    }

static
  tgg00_PrivilegeSet ptoc_Var130 =
  {
  (unsigned char)'\0'
  };


int ak42get_tablename(acv, a41v, ptoc_ptr_required_priv)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
tgg00_PrivilegeSet ptoc_ptr_required_priv;
    {
    tak_all_command_glob *ptoc_Var131;
    tak40_show_glob *ptoc_Var132;
    tgg00_PrivilegeSet ptoc_Var133;
    int ptoc_Var134;
    tgg00_PrivilegeSet required_priv;
    _ptoc_MOVE(char, sizeof(tgg00_PrivilegeSet ), required_priv, ptoc_ptr_required_priv);
    cccprint("AK42  ak42get_ta");
    ptoc_Var131 = &*acv;
    ptoc_Var132 = &*a41v;
    ptoc_Var132->a4ti = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var131->a_ap_tree , "vak42.p" , 22930))[sql__setrang(ptoc_Var132->a4ti , 0 , 32767)].n_lo_level;
    a11get_check_table(&(*acv) , 0 , 0 , 0 , required_priv , (char ) sql__setrang((int ) ptoc_Var131->a_is_ddl == 48 && sql__nre(sizeof(tgg00_PrivilegeSet ) , required_priv , ptoc_Var130) , 0 , 1) , 1 , 0 , &ptoc_Var132->a4ti , ptoc_Var132->a4authname , ptoc_Var132->a4tablen , &ptoc_Var132->a4p_arr);
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var131->a_return_segm , "vak42.p" , 22934)).integer.C_4.sp1r_returncode == 0)
        {
        if ((ptoc_Var134 = (int ) (*(struct tak_systembuffer *) sql__nil(ptoc_Var132->a4p_arr.pbasep , "vak42.p" , 22936)).integer.C_1.sbase.btablekind) != 1 && ptoc_Var134 != 2)
            {
            a07_nb_put_error(&(*acv) , 6340 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var131->a_ap_tree , "vak42.p" , 22940))[sql__setrang(ptoc_Var132->a4ti , 0 , 32767)].n_pos , ptoc_Var132->a4tablen);
            }
        }
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var131->a_return_segm , "vak42.p" , 22943)).integer.C_4.sp1r_returncode == 0)
        {
        ptoc_Var132->a4ti = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var131->a_ap_tree , "vak42.p" , 22946))[sql__setrang(ptoc_Var132->a4ti , 0 , 32767)].n_sa_level;
        if (ptoc_Var132->a4ti != 0 && (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var131->a_ap_tree , "vak42.p" , 22947))[sql__setrang(ptoc_Var132->a4ti , 0 , 32767)].n_proc != 33)
            {
            a05_identifier_get(&(*acv) , ptoc_Var132->a4ti , sizeof(tsp00_C64 ) , ptoc_Var132->a4coln);
            ptoc_Var132->a4ti = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var131->a_ap_tree , "vak42.p" , 22951))[sql__setrang(ptoc_Var132->a4ti , 0 , 32767)].n_sa_level;
            }
        }
    cccprint("<ak42get_tablena");
    }



int a42inc_refresh_cnt(is_fast)
char is_fast;
    {
    cccprint("AK42  a42inc_ref");
    ak42refresh_cnt = ak42refresh_cnt + 1;
    if ((char ) sql__setrang(is_fast , 0 , 1))
        {
        ak42fast_refresh_cnt = ak42fast_refresh_cnt + 1;
        }
    cccprint("<a42inc_refresh_");
    }

static
  tgg00_PrivilegeSet ptoc_Var135 =
  {
  (unsigned char)'\0'
  };
static
  tgg00_PrivilegeSet ptoc_Var136 =
  {
  (unsigned char)'\020'
  };


int a42index_inf_to_messbuf(acv, mblock, a41v, indexn, selectivity, diagnose_index)
tak_all_command_glob *acv;
struct tgg00_MessBlock *mblock;
tak40_show_glob *a41v;
tsp00_KnlIdentifier indexn;
char *selectivity;
char diagnose_index;
    {
    char found;
    char send_messbuf;
    tgg00_PrivilegeSet required_priv;
    tgg00_BasisError b_err;
    int a4save;
    int j;
    tsp_int2 mindex_no;
    tgg00_StackEntry stack[16];
    tak_index_scan_record index_scan_rec;
    tak_all_command_glob *ptoc_Var137;
    tak40_show_glob *ptoc_Var138;
    struct ptoc_Type74 *ptoc_Var139;
    int ptoc_Var140;
    tak_index_scan_record *ptoc_Var141;
    struct ptoc_Type96 *ptoc_Var142;
    tak_index_scan_record *ptoc_Var143;
    struct ptoc_Type96 *ptoc_Var144;
    struct tgg00_MessBlock *ptoc_Var145;
    tgg00_QualBuf *ptoc_Var146;
    cccprint("AK42  a42index_i");
    ptoc_Var137 = &*acv;
    ptoc_Var138 = &*a41v;
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var137->a_return_segm , "vak42.p" , 23003)).integer.C_4.sp1r_returncode == 0)
        {
        b_err = 0;
        send_messbuf = (char ) sql__setrang(!(char ) sql__setrang(*selectivity , 0 , 1) , 0 , 1);
        a4save = ptoc_Var138->a4ti;
        ptoc_Var138->a4ti = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var137->a_ap_tree , "vak42.p" , 23009))[sql__setrang(ptoc_Var138->a4ti , 0 , 32767)].n_lo_level;
        ptoc_Var138->a4priv_check = 1;
        _ptoc_ASSIGN(unsigned char , 64, indexn, a01_il_b_identifier)
        ptoc_Var139 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var137->a_ap_tree , "vak42.p" , 23012))[sql__setrang(ptoc_Var138->a4ti , 0 , 32767)];
        if ((int ) ptoc_Var139->n_symb == 42)
            {
            a05_identifier_get(&(*acv) , ptoc_Var138->a4ti , sizeof(tsp00_C64 ) , indexn);
            }
        else
            {
            ptoc_Var138->a4ti = a4save;
            }
        if ((char ) sql__setrang(diagnose_index , 0 , 1) && ((ptoc_Var140 = (int ) ptoc_Var137->a_current_user_kind) == 4 || ptoc_Var140 == 5) || (int ) ptoc_Var137->a_is_ddl == 48 && (_ptoc_MEMCMP(unsigned char , 64, (*acv).a_curr_user_name , a01_i_domain) == 0 ? 1 : 0))
            {
            _ptoc_MOVE(unsigned char , 1, required_priv, ptoc_Var135);
            }
        else
            {
            _ptoc_MOVE(unsigned char , 1, required_priv, ptoc_Var136);
            }
        ak42get_tablename(&(*acv) , &(*a41v) , required_priv);
        }
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var137->a_return_segm , "vak42.p" , 23036)).integer.C_4.sp1r_returncode == 0 && ((int ) ptoc_Var137->a_ex_kind != 0 || (int ) ptoc_Var138->a4sh_kind == 0))
        {
        *selectivity = (char ) sql__setrang(!(char ) sql__setrang(*selectivity , 0 , 1) && ptoc_Var138->a4ti != 0 , 0 , 1);
        (*mblock).integer.C_2.mb_struct = 9;
        if ((char ) sql__setrang(diagnose_index , 0 , 1))
            {
            (*mblock).integer.C_2.mb_type = 22;
            (*mblock).integer.C_2.mb_type2 = 28;
            }
        else
            {
            (*mblock).integer.C_2.mb_type = 32;
            if ((char ) sql__setrang(*selectivity , 0 , 1))
                {
                (*mblock).integer.C_2.mb_type2 = 63;
                }
            else
                {
                (*mblock).integer.C_2.mb_type2 = 0;
                }
            }
        found = 0;
        if (_ptoc_MEMCMP(unsigned char , 64, indexn , a01_il_b_identifier) == 0 ? 1 : 0)
            {
            if (!a061exist_columnname(&(*(struct tak_systembuffer *) sql__nil(ptoc_Var138->a4p_arr.pbasep , "vak42.p" , 23063)).integer.C_1.sbase , ptoc_Var138->a4coln , &ptoc_Var138->a4col_ptr))
                {
                a07_b_put_error(&(*acv) , 6492 , 1);
                }
            else
                {
                if ((char ) sql__setrang((*(struct tak_systembuffer *) sql__nil(ptoc_Var138->a4p_arr.pbasep , "vak42.p" , 23069)).integer.C_1.sbase.bindexexist , 0 , 1))
                    {
                    a24init_index_scan(&(*acv) , (*(struct tak_systembuffer *) sql__nil(ptoc_Var138->a4p_arr.pbasep , "vak42.p" , 23072)).integer.C_1.sbase.btreeid.integer.C_4.fileTabId_gg00 , &index_scan_rec);
                    while (!(char ) sql__setrang(found , 0 , 1) && a24next_named_index(&(*acv) , &index_scan_rec))
                        {
                        ptoc_Var141 = &index_scan_rec;
                        ptoc_Var142 = &(*(struct tak_systembuffer *) sql__nil(ptoc_Var141->isr_buf , "vak42.p" , 23077)).integer.C_22.smindex.indexdef[sql__setrang(ptoc_Var141->isr_index , 1 , 38) + -1];
                        if ((int ) ptoc_Var142->icount == 1 && (int ) ptoc_Var142->icolseq[0] == (int ) (*(tak00_columninfo *) sql__nil(ptoc_Var138->a4col_ptr , "vak42.p" , 23079)).integer.C_1.creccolno)
                            {
                            found = 1;
                            a24get_indexname(&(*acv) , ptoc_Var141->isr_buf , ptoc_Var141->isr_index , indexn);
                            }
                        }
                    }
                if (!(char ) sql__setrang(found , 0 , 1))
                    {
                    a07_b_put_error(&(*acv) , 6212 , 1);
                    }
                }
            }
        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var137->a_return_segm , "vak42.p" , 23099)).integer.C_4.sp1r_returncode == 0 && (_ptoc_MEMCMP(unsigned char , 64, indexn , a01_il_b_identifier) == 0 ? 0 : 1))
            {
            ptoc_Var143 = &index_scan_rec;
            if (!(char ) sql__setrang(found , 0 , 1))
                {
                if ((char ) sql__setrang((*(struct tak_systembuffer *) sql__nil(ptoc_Var138->a4p_arr.pbasep , "vak42.p" , 23108)).integer.C_1.sbase.bindexexist , 0 , 1))
                    {
                    a24find_indexname(&(*acv) , (*(struct tak_systembuffer *) sql__nil(ptoc_Var138->a4p_arr.pbasep , "vak42.p" , 23112)).integer.C_1.sbase.bsurrogate , indexn , &index_scan_rec);
                    found = (char ) sql__setrang(ptoc_Var143->isr_buf != (struct tak_systembuffer *) (int *) 0 , 0 , 1);
                    }
                }
            if ((char ) sql__setrang(found , 0 , 1))
                {
                (*(tgg00_QualBuf *) sql__nil((*mblock).integer.C_4.mb_qual , "vak42.p" , 23122)).integer.C_2.mtree = (*(struct tak_systembuffer *) sql__nil(ptoc_Var138->a4p_arr.pbasep , "vak42.p" , 23122)).integer.C_1.sbase.btreeid;
                (*mblock).integer.C_5.mb_st = (tgg00_StackList *) &stack[0];
                (*(tgg00_QualBuf *) sql__nil((*mblock).integer.C_4.mb_qual , "vak42.p" , 23124)).integer.C_2.mst_addr = (*mblock).integer.C_5.mb_st;
                (*mblock).integer.C_2.mb_st_size = sizeof(tgg00_StackEntry ) * 16;
                (*mblock).integer.C_2.mb_st_max = 16;
                ptoc_Var144 = &(*(struct tak_systembuffer *) sql__nil(ptoc_Var143->isr_buf , "vak42.p" , 23128)).integer.C_22.smindex.indexdef[sql__setrang(ptoc_Var143->isr_index , 1 , 38) + -1];
                ptoc_Var145 = &*mblock;
                ptoc_Var146 = (tgg00_QualBuf *) sql__nil(ptoc_Var145->integer.C_4.mb_qual , "vak42.p" , 23129);
                ptoc_Var146->integer.C_2.mmult_pos = 1;
                ptoc_Var146->integer.C_2.mmult_cnt = 1;
                (*(tgg00_StackList *) sql__nil(ptoc_Var145->integer.C_5.mb_st , "vak42.p" , 23133))[0] = ptoc_Var144->icolstack[0];
                ptoc_Var146->integer.C_2.mfirst_free = 2;
                if ((char ) sql__setrang(diagnose_index , 0 , 1))
                    {
                    ptoc_Var140 = ptoc_Var144->icount;
                    if (2 <= ptoc_Var140)
                        {
                        j = 2;
                        do
                            {
                            (*(tgg00_StackList *) sql__nil(ptoc_Var145->integer.C_5.mb_st , "vak42.p" , 23139))[sql__setrang(j , 1 , 32767) + -1] = ptoc_Var144->icolstack[sql__setrang(j , 1 , 16) + -1];
                            ptoc_Var146->integer.C_2.mmult_cnt = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var146->integer.C_2.mmult_cnt , -32768 , 32767 , "vak42.p" , 23140) , -32768 , 32767);
                            ptoc_Var146->integer.C_2.mfirst_free = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var146->integer.C_2.mfirst_free , -32768 , 32767 , "vak42.p" , 23141) , -32768 , 32767);
                            if (j == ptoc_Var140)
                                {
                                break;
                                }
                            j += 1;
                            }
                        while (!0);
                        }
                    }
                (*(tgg00_DataPart *) sql__nil((*mblock).integer.C_3.mb_data , "vak42.p" , 23146)).integer.C_7.mbp_sample.sam_rows = -1;
                (*(tgg00_DataPart *) sql__nil((*mblock).integer.C_3.mb_data , "vak42.p" , 23147)).integer.C_7.mbp_sample.sam_percent = 100;
                (*(tgg00_DataPart *) sql__nil((*mblock).integer.C_3.mb_data , "vak42.p" , 23149)).integer.C_7.mbp_sample.sam_with_long = 1;
                (*(tgg00_DataPart *) sql__nil((*mblock).integer.C_3.mb_data , "vak42.p" , 23150)).integer.C_7.mbp_sample.sam_fill1 = 0;
                (*(tgg00_DataPart *) sql__nil((*mblock).integer.C_3.mb_data , "vak42.p" , 23151)).integer.C_7.mbp_sample.sam_fill2 = 0;
                if ((char ) sql__setrang(send_messbuf , 0 , 1))
                    {
                    a06rsend_mess_buf(&(*acv) , &(*mblock) , 1 , &b_err);
                    }
                }
            else
                {
                a07_nb_put_error(&(*acv) , 6502 , 1 , indexn);
                }
            }
        if ((int ) b_err != 0)
            {
            a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , 1);
            }
        }
    cccprint("<a42index_inf_to");
    }



int ak42session_to_sname(session, max_len, n)
tgg91_SessionNo session;
int max_len;
tsp00_Sname n;
    {
    int pos;
    int ln_len;
    tsp00_Line ln;
    int ptoc_Var147;
    cccprint("AK42  ak42sessio");
    ptoc_Var147 = sizeof(tsp00_C12 );
    if (1 <= ptoc_Var147)
        {
        ln_len = 1;
        do
            {
            ln[sql__setrang(ln_len , 1 , 132) + -1] = (unsigned char)' ';
            if (ln_len == ptoc_Var147)
                {
                break;
                }
            ln_len += 1;
            }
        while (!0);
        }
    ln_len = 0;
    gg06SessionToLine(&session , &ln_len , ln);
    if (ln_len > sizeof(tsp00_C12 ))
        {
        ln_len = sizeof(tsp00_C12 );
        }
    if (ln_len > max_len)
        {
        ln_len = max_len;
        }
    if ((ptoc_Var147 = (int ) ln[0]) >= 48 && ptoc_Var147 <= 57 && max_len <= sizeof(tsp00_C12 ))
        {
        pos = max_len - ln_len + 1;
        }
    else
        {
        pos = 1;
        }
    _ptoc_ASSIGN(unsigned char , 12, n, "            ")
    s10mv1(sizeof(unsigned char [132]) , sizeof(tsp00_C12 ) , ln , 1 , n , pos , ln_len);
    cccprint("<ak42session_to_");
    }



int ak42xparams(acv, a41v)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
    {
    tgg00_BasisError b_err;
    tgg00_Lkey curr_key;
    tgg00_Rec data;
    tsp00_C18 valname;
    tak_all_command_glob *ptoc_Var148;
    cccprint("AK42  ak42xparam");
    a40get_catalog_table(&(*acv) , &(*a41v));
    if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak42.p" , 23234)).integer.C_4.sp1r_returncode == 0 && a36xparam_tree.integer.C_1.fileRoot_gg00 != 2147483647)
        {
        ptoc_Var148 = &*acv;
        curr_key.integer.C_11.len = 0;
        do
            {
            b02next_record(&ptoc_Var148->a_transinf.tri_trans , &a36xparam_tree , &curr_key , 0 , &data);
            b_err = (short ) sql__setrang(ptoc_Var148->a_transinf.tri_trans.trError_gg00 , -32768 , 32767);
            if ((int ) b_err == 0 || (int ) b_err == 1200)
                {
                b_err = 0;
                curr_key.integer.C_11.len = (short ) sql__setrang(data.integer.C_12.keylen , -32768 , 32767);
                g10mv2("VAK42 " , 8 , sizeof(tgg00_RecBody ) , sizeof(tsp00_Key ) , data.integer.C_12.info , 1 , curr_key.integer.C_11.k , 1 , (int ) data.integer.C_12.keylen , &b_err);
                g10mv6("VAK42 " , 9 , sizeof(tgg00_RecBody ) , sizeof(tsp00_C18 ) , data.integer.C_12.info , 1 , valname , 1 , sizeof(tsp00_C18 ) , &b_err);
                if (_ptoc_MEMCMP(unsigned char , 18, valname , "CONTROLPASSWORD   ") == 0 ? 0 : 1)
                    {
                    a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) &data.integer.C_12.info[0] , 18);
                    a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) &data.integer.C_12.info[18] , (int ) data.integer.C_12.len + -26);
                    }
                }
            }
        while (!((int ) b_err != 0 || (int ) (*(tsp1_segment *) sql__nil(ptoc_Var148->a_return_segm , "vak42.p" , 23270)).integer.C_4.sp1r_returncode != 0));
        }
    cccprint("<ak42xparams    ");
    }



int ak42ascii_const_move(src, srclen, destpos, dest, e)
tsp00_MoveObjPtr src;
int srclen;
int *destpos;
tsp00_C256 dest;
tsp00_Int2 *e;
    {
    int ix;
    int ptoc_Var149;
    cccprint("AK42  ak42ascii_");
    if ((char ) sql__setrang(g01unicode , 0 , 1))
        {
        ptoc_Var149 = srclen;
        if (1 <= ptoc_Var149)
            {
            ix = 1;
            do
                {
                dest[sql__setrang(*destpos , 1 , 256) + -1] = (unsigned char)'\0';
                dest[sql__setrang(*destpos + 1 , 1 , 256) + -1] = (unsigned char ) sql__setrang((*(tsp00_MoveObj *) sql__nil(src , "vak42.p" , 23295))[sql__setrang(ix , 1 , 8192000) + -1] , 0 , 255);
                *destpos = *destpos + 2;
                if (ix == ptoc_Var149)
                    {
                    break;
                    }
                ix += 1;
                }
            while (!0);
            }
        }
    else
        {
        g10mv3("VAK42 " , 10 , srclen , sizeof(tsp00_C256 ) , sql__nil(src , "vak42.p" , 23302) , 1 , dest , *destpos , srclen , &(*e));
        *destpos = *destpos + srclen;
        }
    cccprint("<ak42ascii_const");
    }



int ak42move_trans_no(acv, a41v, trans)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
tgg91_TransNo *trans;
    {
    int length;
    tsp00_C20 c20;
    tsp00_Line aux_line;
    cccprint("AK42  ak42move_t");
    length = 0;
    if (gg06IsNilTrans(&(*trans)))
        {
        a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) (int *) 0 , 0);
        }
    else
        {
        gg06TransToLine(&(*trans) , &length , aux_line);
        if (length > sizeof(tsp00_C20 ))
            {
            length = sizeof(tsp00_C20 );
            }
        _ptoc_ASSIGN(unsigned char , 20, c20, "                    ")
        g10mv5("VAK42 " , 11 , sizeof(unsigned char [132]) , sizeof(tsp00_C20 ) , aux_line , 1 , c20 , sizeof(tsp00_C20 ) - length + 1 , length , &(*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak42.p" , 23340)).integer.C_4.sp1r_returncode);
        a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) c20 , sizeof(tsp00_C20 ));
        }
    cccprint("<ak42move_trans_");
    }



int ak42internal_state(acv, a41v)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
    {
    tsp00_C20 component;
    tsp00_C40 description;
    tsp00_C12 value;
    tak_all_command_glob *ptoc_Var150;
    tak40_show_glob *ptoc_Var151;
    cccprint("AK42  ak42intern");
    ptoc_Var150 = &*acv;
    ptoc_Var151 = &*a41v;
    a40get_catalog_table(&(*acv) , &(*a41v));
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var150->a_return_segm , "vak42.p" , 23365)).integer.C_4.sp1r_returncode == 0 && (int ) ptoc_Var150->a_ex_kind != 0)
        {
        k57select_restartrec((*acv).a_transinf.tri_trans.trTaskId_gg00 , (unsigned char *) &*acv , (unsigned char *) &*a41v);
        k57select_log_info((*acv).a_transinf.tri_trans.trTaskId_gg00 , (unsigned char *) &*acv , (unsigned char *) &*a41v);
        }
    cccprint("<ak42internal_st");
    }



int a42bool_to_intern_state(acv_ptr, a41v_ptr, ptoc_ptr_component, ptoc_ptr_description, bool_val)
tak_acv_address acv_ptr;
tak40_show_glob_ptr a41v_ptr;
tsp00_C20 ptoc_ptr_component;
tsp00_C40 ptoc_ptr_description;
char bool_val;
    {
    tsp00_Sname n;
    tsp00_C20 component;
    tsp00_C40 description;
    _ptoc_MOVE(char, sizeof(tsp00_C20 ), component, ptoc_ptr_component);
    _ptoc_MOVE(char, sizeof(tsp00_C40 ), description, ptoc_ptr_description);
    cccprint("AK42  a42bool_to");
    if ((char ) sql__setrang(bool_val , 0 , 1))
        {
        _ptoc_ASSIGN(unsigned char , 12, n, "yes         ")
        }
    else
        {
        _ptoc_ASSIGN(unsigned char , 12, n, "no          ")
        }
    a40move_const(sql__nil(acv_ptr , "vak42.p" , 23398) , sql__nil(a41v_ptr , "vak42.p" , 23398) , (tsp00_MoveObj *) component , sizeof(tsp00_C20 ));
    a40move_const(sql__nil(acv_ptr , "vak42.p" , 23399) , sql__nil(a41v_ptr , "vak42.p" , 23399) , (tsp00_MoveObj *) description , sizeof(tsp00_C40 ));
    a40move_const(sql__nil(acv_ptr , "vak42.p" , 23400) , sql__nil(a41v_ptr , "vak42.p" , 23400) , (tsp00_MoveObj *) n , sizeof(tsp00_C12 ));
    cccprint("<a42bool_to_inte");
    }



int a42cint6_to_intern_state(acv_ptr, a41v_ptr, ptoc_ptr_component, ptoc_ptr_description, cint6)
tak_acv_address acv_ptr;
tak40_show_glob_ptr a41v_ptr;
tsp00_C20 ptoc_ptr_component;
tsp00_C40 ptoc_ptr_description;
tgg90_Cint6 cint6;
    {
    int ln_len;
    tsp00_Line ln;
    tsp00_C20 component;
    tsp00_C40 description;
    _ptoc_MOVE(char, sizeof(tsp00_C20 ), component, ptoc_ptr_component);
    _ptoc_MOVE(char, sizeof(tsp00_C40 ), description, ptoc_ptr_description);
    cccprint("AK42  a42cint6_t");
    ln_len = 0;
    gg06Cint6ToLine(&cint6 , &ln_len , ln);
    a40move_const(sql__nil(acv_ptr , "vak42.p" , 23421) , sql__nil(a41v_ptr , "vak42.p" , 23421) , (tsp00_MoveObj *) component , sizeof(tsp00_C20 ));
    a40move_const(sql__nil(acv_ptr , "vak42.p" , 23422) , sql__nil(a41v_ptr , "vak42.p" , 23422) , (tsp00_MoveObj *) description , sizeof(tsp00_C40 ));
    a40move_const(sql__nil(acv_ptr , "vak42.p" , 23423) , sql__nil(a41v_ptr , "vak42.p" , 23423) , (tsp00_MoveObj *) ln , ln_len);
    cccprint("<a42cint6_to_int");
    }



int a42counter_to_intern_state(acv_ptr, a41v_ptr, ptoc_ptr_component, ptoc_ptr_description, counter)
tak_acv_address acv_ptr;
tak40_show_glob_ptr a41v_ptr;
tsp00_C20 ptoc_ptr_component;
tsp00_C40 ptoc_ptr_description;
tsp00_8ByteCounter counter;
    {
    tsp00_C20 component;
    tsp00_C40 description;
    _ptoc_MOVE(char, sizeof(tsp00_C20 ), component, ptoc_ptr_component);
    _ptoc_MOVE(char, sizeof(tsp00_C40 ), description, ptoc_ptr_description);
    cccprint("AK42  a42counter");
    a40move_const(sql__nil(acv_ptr , "vak42.p" , 23438) , sql__nil(a41v_ptr , "vak42.p" , 23438) , (tsp00_MoveObj *) component , sizeof(tsp00_C20 ));
    a40move_const(sql__nil(acv_ptr , "vak42.p" , 23439) , sql__nil(a41v_ptr , "vak42.p" , 23439) , (tsp00_MoveObj *) description , sizeof(tsp00_C40 ));
    a40move_counter(sql__nil(acv_ptr , "vak42.p" , 23440) , sql__nil(a41v_ptr , "vak42.p" , 23440) , counter , 0 , 0 , 0);
    cccprint("<a42counter_to_i");
    }



int a42date_to_intern_state(acv_ptr, a41v_ptr, ptoc_ptr_component, ptoc_ptr_description, int_date)
tak_acv_address acv_ptr;
tak40_show_glob_ptr a41v_ptr;
tsp00_C20 ptoc_ptr_component;
tsp00_C40 ptoc_ptr_description;
tsp00_Int4 int_date;
    {
    int ln_len;
    tsp00_Date str_date;
    tsp00_Time dummy_time;
    tsp00_Line ln;
    tsp00_C20 component;
    tsp00_C40 description;
    _ptoc_MOVE(char, sizeof(tsp00_C20 ), component, ptoc_ptr_component);
    _ptoc_MOVE(char, sizeof(tsp00_C40 ), description, ptoc_ptr_description);
    cccprint("AK42  a42date_to");
    ln_len = 0;
    g17intdate_time(int_date , 0 , str_date , dummy_time);
    g17date_to_line(str_date , &ln_len , ln);
    a40move_const(sql__nil(acv_ptr , "vak42.p" , 23464) , sql__nil(a41v_ptr , "vak42.p" , 23464) , (tsp00_MoveObj *) component , sizeof(tsp00_C20 ));
    a40move_const(sql__nil(acv_ptr , "vak42.p" , 23465) , sql__nil(a41v_ptr , "vak42.p" , 23465) , (tsp00_MoveObj *) description , sizeof(tsp00_C40 ));
    a40move_const(sql__nil(acv_ptr , "vak42.p" , 23466) , sql__nil(a41v_ptr , "vak42.p" , 23466) , (tsp00_MoveObj *) ln , ln_len);
    cccprint("<a42date_to_inte");
    }



int a42int4_to_intern_state(acv_ptr, a41v_ptr, ptoc_ptr_component, ptoc_ptr_description, int_val)
tak_acv_address acv_ptr;
tak40_show_glob_ptr a41v_ptr;
tsp00_C20 ptoc_ptr_component;
tsp00_C40 ptoc_ptr_description;
tsp00_Int4 int_val;
    {
    tsp00_C20 component;
    tsp00_C40 description;
    _ptoc_MOVE(char, sizeof(tsp00_C20 ), component, ptoc_ptr_component);
    _ptoc_MOVE(char, sizeof(tsp00_C40 ), description, ptoc_ptr_description);
    cccprint("AK42  a42int4_to");
    a40move_const(sql__nil(acv_ptr , "vak42.p" , 23481) , sql__nil(a41v_ptr , "vak42.p" , 23481) , (tsp00_MoveObj *) component , sizeof(tsp00_C20 ));
    a40move_const(sql__nil(acv_ptr , "vak42.p" , 23482) , sql__nil(a41v_ptr , "vak42.p" , 23482) , (tsp00_MoveObj *) description , sizeof(tsp00_C40 ));
    a40move_i4(sql__nil(acv_ptr , "vak42.p" , 23483) , sql__nil(a41v_ptr , "vak42.p" , 23483) , int_val , 0 , 0 , 0);
    cccprint("<a42int4_to_inte");
    }



int a42logpos_to_intern_state(acv_ptr, a41v_ptr, ptoc_ptr_component, ptoc_ptr_description, logpos)
tak_acv_address acv_ptr;
tak40_show_glob_ptr a41v_ptr;
tsp00_C20 ptoc_ptr_component;
tsp00_C40 ptoc_ptr_description;
tgg00_LogPos logpos;
    {
    int ln_len;
    tsp00_Line ln;
    tsp00_C20 component;
    tsp00_C40 description;
    _ptoc_MOVE(char, sizeof(tsp00_C20 ), component, ptoc_ptr_component);
    _ptoc_MOVE(char, sizeof(tsp00_C40 ), description, ptoc_ptr_description);
    cccprint("AK42  a42logpos_");
    ln_len = 0;
    g17trimint4_to_line(logpos.integer.C_1.logLpno_gg00 , &ln_len , ln);
    ln_len = ln_len + 1;
    ln[sql__setrang(ln_len , 1 , 132) + -1] = (unsigned char)'.';
    g17trimint4_to_line(logpos.integer.C_1.logPpos_gg00 , &ln_len , ln);
    ln_len = ln_len + 1;
    ln[sql__setrang(ln_len , 1 , 132) + -1] = (unsigned char)' ';
    g17pagetype_to_line((unsigned char ) sql__setrang(logpos.integer.C_1.logPageType_gg00 , 0 , 12) , &ln_len , ln);
    a40move_const(sql__nil(acv_ptr , "vak42.p" , 23510) , sql__nil(a41v_ptr , "vak42.p" , 23510) , (tsp00_MoveObj *) component , sizeof(tsp00_C20 ));
    a40move_const(sql__nil(acv_ptr , "vak42.p" , 23511) , sql__nil(a41v_ptr , "vak42.p" , 23511) , (tsp00_MoveObj *) description , sizeof(tsp00_C40 ));
    a40move_const(sql__nil(acv_ptr , "vak42.p" , 23512) , sql__nil(a41v_ptr , "vak42.p" , 23512) , (tsp00_MoveObj *) ln , ln_len);
    cccprint("<a42logpos_to_in");
    }



int a42mess_to_intern_state(acv_ptr, a41v_ptr, ptoc_ptr_component, ptoc_ptr_description, messtype)
tak_acv_address acv_ptr;
tak40_show_glob_ptr a41v_ptr;
tsp00_C20 ptoc_ptr_component;
tsp00_C40 ptoc_ptr_description;
tgg00_MessType messtype;
    {
    int ln_len;
    tsp00_Line ln;
    tsp00_C20 component;
    tsp00_C40 description;
    _ptoc_MOVE(char, sizeof(tsp00_C20 ), component, ptoc_ptr_component);
    _ptoc_MOVE(char, sizeof(tsp00_C40 ), description, ptoc_ptr_description);
    cccprint("AK42  a42mess_to");
    ln_len = 0;
    g17messtype_to_line((unsigned char ) sql__setrang(messtype , 0 , 95) , &ln_len , ln);
    a40move_const(sql__nil(acv_ptr , "vak42.p" , 23533) , sql__nil(a41v_ptr , "vak42.p" , 23533) , (tsp00_MoveObj *) component , sizeof(tsp00_C20 ));
    a40move_const(sql__nil(acv_ptr , "vak42.p" , 23534) , sql__nil(a41v_ptr , "vak42.p" , 23534) , (tsp00_MoveObj *) description , sizeof(tsp00_C40 ));
    a40move_const(sql__nil(acv_ptr , "vak42.p" , 23535) , sql__nil(a41v_ptr , "vak42.p" , 23535) , (tsp00_MoveObj *) ln , ln_len);
    cccprint("<a42mess_to_inte");
    }



int a42mess2_to_intern_state(acv_ptr, a41v_ptr, ptoc_ptr_component, ptoc_ptr_description, mess2type)
tak_acv_address acv_ptr;
tak40_show_glob_ptr a41v_ptr;
tsp00_C20 ptoc_ptr_component;
tsp00_C40 ptoc_ptr_description;
tgg00_MessType2 mess2type;
    {
    int ln_len;
    tsp00_Line ln;
    tsp00_C20 component;
    tsp00_C40 description;
    _ptoc_MOVE(char, sizeof(tsp00_C20 ), component, ptoc_ptr_component);
    _ptoc_MOVE(char, sizeof(tsp00_C40 ), description, ptoc_ptr_description);
    cccprint("AK42  a42mess2_t");
    ln_len = 0;
    g17mess2type_to_line((unsigned char ) sql__setrang(mess2type , 0 , 95) , &ln_len , ln);
    a40move_const(sql__nil(acv_ptr , "vak42.p" , 23556) , sql__nil(a41v_ptr , "vak42.p" , 23556) , (tsp00_MoveObj *) component , sizeof(tsp00_C20 ));
    a40move_const(sql__nil(acv_ptr , "vak42.p" , 23557) , sql__nil(a41v_ptr , "vak42.p" , 23557) , (tsp00_MoveObj *) description , sizeof(tsp00_C40 ));
    a40move_const(sql__nil(acv_ptr , "vak42.p" , 23558) , sql__nil(a41v_ptr , "vak42.p" , 23558) , (tsp00_MoveObj *) ln , ln_len);
    cccprint("<a42mess2_to_int");
    }



int a42session_to_intern_state(acv_ptr, a41v_ptr, ptoc_ptr_component, ptoc_ptr_description, session)
tak_acv_address acv_ptr;
tak40_show_glob_ptr a41v_ptr;
tsp00_C20 ptoc_ptr_component;
tsp00_C40 ptoc_ptr_description;
tgg91_SessionNo session;
    {
    int ln_len;
    tsp00_Line ln;
    tsp00_C20 component;
    tsp00_C40 description;
    _ptoc_MOVE(char, sizeof(tsp00_C20 ), component, ptoc_ptr_component);
    _ptoc_MOVE(char, sizeof(tsp00_C40 ), description, ptoc_ptr_description);
    cccprint("AK42  a42session");
    ln_len = 0;
    gg06SessionToLine(&session , &ln_len , ln);
    a40move_const(sql__nil(acv_ptr , "vak42.p" , 23579) , sql__nil(a41v_ptr , "vak42.p" , 23579) , (tsp00_MoveObj *) component , sizeof(tsp00_C20 ));
    a40move_const(sql__nil(acv_ptr , "vak42.p" , 23580) , sql__nil(a41v_ptr , "vak42.p" , 23580) , (tsp00_MoveObj *) description , sizeof(tsp00_C40 ));
    a40move_const(sql__nil(acv_ptr , "vak42.p" , 23581) , sql__nil(a41v_ptr , "vak42.p" , 23581) , (tsp00_MoveObj *) ln , ln_len);
    cccprint("<a42session_to_i");
    }



int a42str_to_intern_state(acv_ptr, a41v_ptr, ptoc_ptr_component, ptoc_ptr_description, str_ptr, str_len)
tak_acv_address acv_ptr;
tak40_show_glob_ptr a41v_ptr;
tsp00_C20 ptoc_ptr_component;
tsp00_C40 ptoc_ptr_description;
tsp_moveobj_ptr str_ptr;
int str_len;
    {
    int aux_len;
    tsp00_C20 component;
    tsp00_C40 description;
    _ptoc_MOVE(char, sizeof(tsp00_C20 ), component, ptoc_ptr_component);
    _ptoc_MOVE(char, sizeof(tsp00_C40 ), description, ptoc_ptr_description);
    cccprint("AK42  a42str_to_");
    if (str_len > 40)
        {
        aux_len = 40;
        }
    else
        {
        aux_len = str_len;
        }
    a40move_const(sql__nil(acv_ptr , "vak42.p" , 23605) , sql__nil(a41v_ptr , "vak42.p" , 23605) , (tsp00_MoveObj *) component , sizeof(tsp00_C20 ));
    a40move_const(sql__nil(acv_ptr , "vak42.p" , 23606) , sql__nil(a41v_ptr , "vak42.p" , 23606) , (tsp00_MoveObj *) description , sizeof(tsp00_C40 ));
    a40move_const(sql__nil(acv_ptr , "vak42.p" , 23607) , sql__nil(a41v_ptr , "vak42.p" , 23607) , str_ptr , aux_len);
    cccprint("<a42str_to_inter");
    }



int a42surrogate_to_intern_state(acv_ptr, a41v_ptr, ptoc_ptr_component, ptoc_ptr_description, ptoc_ptr_surrogate)
tak_acv_address acv_ptr;
tak40_show_glob_ptr a41v_ptr;
tsp00_C20 ptoc_ptr_component;
tsp00_C40 ptoc_ptr_description;
tgg00_Surrogate ptoc_ptr_surrogate;
    {
    int ln_len;
    tsp00_Line ln;
    tsp00_C20 component;
    tsp00_C40 description;
    tgg00_Surrogate surrogate;
    _ptoc_MOVE(char, sizeof(tsp00_C20 ), component, ptoc_ptr_component);
    _ptoc_MOVE(char, sizeof(tsp00_C40 ), description, ptoc_ptr_description);
    _ptoc_MOVE(char, sizeof(tgg00_Surrogate ), surrogate, ptoc_ptr_surrogate);
    cccprint("AK42  a42surroga");
    ln_len = 0;
    g17surrogate_to_line(surrogate , &ln_len , ln);
    a40move_const(sql__nil(acv_ptr , "vak42.p" , 23628) , sql__nil(a41v_ptr , "vak42.p" , 23628) , (tsp00_MoveObj *) component , sizeof(tsp00_C20 ));
    a40move_const(sql__nil(acv_ptr , "vak42.p" , 23629) , sql__nil(a41v_ptr , "vak42.p" , 23629) , (tsp00_MoveObj *) description , sizeof(tsp00_C40 ));
    a40move_const(sql__nil(acv_ptr , "vak42.p" , 23630) , sql__nil(a41v_ptr , "vak42.p" , 23630) , (tsp00_MoveObj *) ln , ln_len);
    cccprint("<a42surrogate_to");
    }



int a42time_to_intern_state(acv_ptr, a41v_ptr, ptoc_ptr_component, ptoc_ptr_description, int_time)
tak_acv_address acv_ptr;
tak40_show_glob_ptr a41v_ptr;
tsp00_C20 ptoc_ptr_component;
tsp00_C40 ptoc_ptr_description;
tsp00_Int4 int_time;
    {
    int ln_len;
    tsp00_Date dummy_date;
    tsp00_Time str_time;
    tsp00_Line ln;
    tsp00_C20 component;
    tsp00_C40 description;
    _ptoc_MOVE(char, sizeof(tsp00_C20 ), component, ptoc_ptr_component);
    _ptoc_MOVE(char, sizeof(tsp00_C40 ), description, ptoc_ptr_description);
    cccprint("AK42  a42time_to");
    ln_len = 0;
    g17intdate_time(0 , int_time , dummy_date , str_time);
    g17time_to_line(str_time , &ln_len , ln);
    a40move_const(sql__nil(acv_ptr , "vak42.p" , 23654) , sql__nil(a41v_ptr , "vak42.p" , 23654) , (tsp00_MoveObj *) component , sizeof(tsp00_C20 ));
    a40move_const(sql__nil(acv_ptr , "vak42.p" , 23655) , sql__nil(a41v_ptr , "vak42.p" , 23655) , (tsp00_MoveObj *) description , sizeof(tsp00_C40 ));
    a40move_const(sql__nil(acv_ptr , "vak42.p" , 23656) , sql__nil(a41v_ptr , "vak42.p" , 23656) , (tsp00_MoveObj *) ln , ln_len);
    cccprint("<a42time_to_inte");
    }



int a42trans_to_intern_state(acv_ptr, a41v_ptr, ptoc_ptr_component, ptoc_ptr_description, trans)
tak_acv_address acv_ptr;
tak40_show_glob_ptr a41v_ptr;
tsp00_C20 ptoc_ptr_component;
tsp00_C40 ptoc_ptr_description;
tgg91_TransNo trans;
    {
    int ln_len;
    tsp00_Line ln;
    tsp00_C20 component;
    tsp00_C40 description;
    _ptoc_MOVE(char, sizeof(tsp00_C20 ), component, ptoc_ptr_component);
    _ptoc_MOVE(char, sizeof(tsp00_C40 ), description, ptoc_ptr_description);
    cccprint("AK42  a42trans_t");
    ln_len = 0;
    gg06TransToLine(&trans , &ln_len , ln);
    a40move_const(sql__nil(acv_ptr , "vak42.p" , 23677) , sql__nil(a41v_ptr , "vak42.p" , 23677) , (tsp00_MoveObj *) component , sizeof(tsp00_C20 ));
    a40move_const(sql__nil(acv_ptr , "vak42.p" , 23678) , sql__nil(a41v_ptr , "vak42.p" , 23678) , (tsp00_MoveObj *) description , sizeof(tsp00_C40 ));
    a40move_const(sql__nil(acv_ptr , "vak42.p" , 23679) , sql__nil(a41v_ptr , "vak42.p" , 23679) , (tsp00_MoveObj *) ln , ln_len);
    cccprint("<a42trans_to_int");
    }



int a42uint1_to_intern_state(acv_ptr, a41v_ptr, ptoc_ptr_component, ptoc_ptr_description, int_val)
tak_acv_address acv_ptr;
tak40_show_glob_ptr a41v_ptr;
tsp00_C20 ptoc_ptr_component;
tsp00_C40 ptoc_ptr_description;
tsp00_Int2 int_val;
    {
    tsp00_C20 component;
    tsp00_C40 description;
    _ptoc_MOVE(char, sizeof(tsp00_C20 ), component, ptoc_ptr_component);
    _ptoc_MOVE(char, sizeof(tsp00_C40 ), description, ptoc_ptr_description);
    cccprint("AK42  a42uint1_t");
    a40move_const(sql__nil(acv_ptr , "vak42.p" , 23701) , sql__nil(a41v_ptr , "vak42.p" , 23701) , (tsp00_MoveObj *) component , sizeof(tsp00_C20 ));
    a40move_const(sql__nil(acv_ptr , "vak42.p" , 23702) , sql__nil(a41v_ptr , "vak42.p" , 23702) , (tsp00_MoveObj *) description , sizeof(tsp00_C40 ));
    a40move_i4(sql__nil(acv_ptr , "vak42.p" , 23703) , sql__nil(a41v_ptr , "vak42.p" , 23703) , (int ) int_val , 0 , 0 , 0);
    cccprint("<a42uint1_to_int");
    }



int a42uint2_to_intern_state(acv_ptr, a41v_ptr, ptoc_ptr_component, ptoc_ptr_description, int_val)
tak_acv_address acv_ptr;
tak40_show_glob_ptr a41v_ptr;
tsp00_C20 ptoc_ptr_component;
tsp00_C40 ptoc_ptr_description;
tsp00_Int4 int_val;
    {
    tsp00_C20 component;
    tsp00_C40 description;
    _ptoc_MOVE(char, sizeof(tsp00_C20 ), component, ptoc_ptr_component);
    _ptoc_MOVE(char, sizeof(tsp00_C40 ), description, ptoc_ptr_description);
    cccprint("AK42  a42uint2_t");
    a40move_const(sql__nil(acv_ptr , "vak42.p" , 23724) , sql__nil(a41v_ptr , "vak42.p" , 23724) , (tsp00_MoveObj *) component , sizeof(tsp00_C20 ));
    a40move_const(sql__nil(acv_ptr , "vak42.p" , 23725) , sql__nil(a41v_ptr , "vak42.p" , 23725) , (tsp00_MoveObj *) description , sizeof(tsp00_C40 ));
    a40move_i4(sql__nil(acv_ptr , "vak42.p" , 23726) , sql__nil(a41v_ptr , "vak42.p" , 23726) , int_val , 0 , 0 , 0);
    cccprint("<a42uint2_to_int");
    }



int a42uint4_to_intern_state(acv_ptr, a41v_ptr, ptoc_ptr_component, ptoc_ptr_description, int_val)
tak_acv_address acv_ptr;
tak40_show_glob_ptr a41v_ptr;
tsp00_C20 ptoc_ptr_component;
tsp00_C40 ptoc_ptr_description;
tsp00_8ByteCounter int_val;
    {
    tsp00_C20 component;
    tsp00_C40 description;
    _ptoc_MOVE(char, sizeof(tsp00_C20 ), component, ptoc_ptr_component);
    _ptoc_MOVE(char, sizeof(tsp00_C40 ), description, ptoc_ptr_description);
    cccprint("AK42  a42uint4_t");
    a40move_const(sql__nil(acv_ptr , "vak42.p" , 23747) , sql__nil(a41v_ptr , "vak42.p" , 23747) , (tsp00_MoveObj *) component , sizeof(tsp00_C20 ));
    a40move_const(sql__nil(acv_ptr , "vak42.p" , 23748) , sql__nil(a41v_ptr , "vak42.p" , 23748) , (tsp00_MoveObj *) description , sizeof(tsp00_C40 ));
    a40move_counter(sql__nil(acv_ptr , "vak42.p" , 23749) , sql__nil(a41v_ptr , "vak42.p" , 23749) , int_val , 0 , 0 , 0);
    cccprint("<a42uint4_to_int");
    }
